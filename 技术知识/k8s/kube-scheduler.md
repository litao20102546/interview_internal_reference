## 每次对一个 pod 进行调度，主要有以下步骤

- 从 scheduler 调度队列中取出一个 pod，如果该 pod 处于删除状态则跳过
- 执行调度逻辑 `sched.schedule()` 返回通过预算及优选算法过滤后选出的最佳 node
- 如果过滤算法没有选出合适的 node，则返回 core.FitError
- 若没有合适的 node 会判断是否启用了抢占策略，若启用了则执行抢占机制
- 判断是否需要 VolumeScheduling 特性
- 执行 reserve plugin
- pod 对应的 spec.NodeName 写上 scheduler 最终选择的 node，更新 scheduler cache
- 请求 apiserver 异步处理最终的绑定操作，写入到 etcd
- 执行 permit plugin
- 执行 prebind plugin
- 执行 postbind plugin

### VolumeScheduling特性

（https://blog.csdn.net/congcheng1062/article/details/100445289）

> Pod借用PVC描述需要的存储资源，PVC是PV的抽象，所以在PVC创建之后，还需要将PVC跟卷进行绑定，也即是PV。
>
> PVC绑定在Pod调度之前，PersistentVolume Controller不会等待Scheduler调度结果，在Statefulset中PVC先于Pod创建，所以PVC/PV绑定可能完成在Pod调度之前。

问题：

* Scheduler不感知卷的“位置”，仅考虑存储容量、访问权限、存储类型、还有第三方CloudProvider上的限制（譬如在AWS、GCE、Aure上使用Disk数量的限制）

* 当应用对卷的**“位置”有要求**，比如使用本地卷，可能出现Pod被Scheduler调度到NodeB，但PersistentVolume Controller绑定了在NodeD上的本地卷，以致PV和Pod不在一个节点

解决办法：

* 如果Pod使用的Volume对“位置”有要求（又叫Topology-Aware Volume），通过延时绑定（DelayBinding）使PersistentVolume Controller不再参与，PVC绑定的工作全部由Scheduler完成。
* 如何标记Topology-Aware Volume ？
* 如何让PersistentVolume Controller不再参与，同时不影响原有流程 ？

1. 如何标记Topology-Aware Volume

* 通过NodeAffinity方式标记Topology-Aware Volume和“位置”信息：

* 创建StorageClass，通过StorageClass间接标记PVC的绑定需要延后（绑定延时）

  标记该PVC需要延后到Node选择出来之后再绑定：

  * 创建StorageClass “X”（无需Provisioner），并设置StorageClass.VolumeBindingMode = 	VolumeBindingWaitForFirstConsumer
  * PVC.StorageClass设置为X

* 依照原有流程创建PVC和Pod，但对于需要延时绑定的PVC，PersistentVolume Controller不再参与。
  通过PVC.StorageClass，PersistentVolume Controller得知PVC是否需要延时绑定。

* 执行原有Predicates函数

* 执行添加Predicate函数CheckVolumeBinding校验候选Node是否满足PV物理拓扑（主要逻辑由FindPodVolumes提供）：

  * 已绑定PVC：对应PV.NodeAffinity需匹配候选Node，否则该节点需要pass
  * 未绑定PVC**：该PVC是否需要延时绑定，如需要，遍历未绑定PV，其NodeAffinity是否匹配候选Node，如满足，**记录PVC和PV的映射关系到缓存bindingInfo**中，留待节点最终选出来之后进行最终的绑定。

* 执行原有Priorities函数

  执行添加Priority函数PrioritizeVolumes。Volume容量匹配越高越好，避免本地存储资源浪费。

* Scheduler选出Node

* 由Scheduler进行API update，完成最终的PVC/PV绑定（异步操作，时间具有不确定性，可能失败）

* 从缓存bindingInfo中获取候选Node上PVC和PV的绑定关系，并通过API完成实际的绑定

* 绑定被调度Pod和Node

**高可用风险需要考虑**：
当Pod调度到某个节点后，将会跟该节点产生亲和，一旦Node发生故障，Pod不能调度到其他节点，只能等待该节点恢复，你能做的就是等待“Node恢复”，如果部署3节点MySQL集群，再挂一个Node，集群将无法提供服务，你能做的还是“等待Node恢复”。这么设计也是合理的，社区认为该Node为Stateful节点，Pod被调度到其他可用Node会导致数据丢失。

- Node不可用后，等待阈值超时，以确定Node无法恢复
- 如确认Node不可恢复，删除PVC，通过解除PVC和PV绑定的方式，解除Pod和Node的绑定
- Scheduler将Pod调度到其他可用Node，PVC重新绑定到可用Node的PV。
- Operator查找MySQL最新备份，拷贝到新的PV
- MySQL集群通过增量同步方式恢复实例数据
- 增量同步变为实时同步，MySQL集群恢复

## `sched.Algorithm.Schedule()` 调用具体调度算法的过程：

- 检查 pod pvc 信息
- 执行 prefilter plugins
- 获取 scheduler cache 的快照，每次调度 pod 时都会获取一次快照
- 执行 `g.findNodesThatFit()` 预选算法
- 执行 postfilter plugin
- 若 node 为 0 直接返回失败的 error，若 node 数为1 直接返回该 node
- 执行 `g.priorityMetaProducer()` 获取 metaPrioritiesInterface，计算 pod 的metadata，检查该 node 上是否有相同 meta 的 pod
- 执行 `PrioritizeNodes()` 算法
- 执行 `g.selectHost()` 通过得分选择一个最佳的 node

## 调度流程
1、预选调度过程，即遍历所有目标Node,筛选出符合要求的候选节点，kubernetes内置了多种预选策略（xxx Predicates）供用户选择

2、确定最优节点，在第一步的基础上采用优选策略（xxx Priority）计算出每个候选节点的积分，取最高积分。

调度流程通过插件式加载的“调度算法提供者”（AlgorithmProvider）具体实现，一个调度算法提供者就是包括一组预选策略与一组优选策略的结构体。

3. 预选策略
    说明：返回true表示该节点满足该Pod的调度条件；返回false表示该节点不满足该Pod的调度条件。

 3.1. NoDiskConflict
  判断备选Pod的数据卷是否与该Node上已存在Pod挂载的数据卷冲突，如果是则返回false，否则返回true。

3.2. PodFitsResources
  判断备选节点的资源是否满足备选Pod的需求，即节点的剩余资源满不满足该Pod的资源使用。

  计算备选Pod和节点中已用资源（该节点所有Pod的使用资源）的总和。
  获取备选节点的状态信息，包括节点资源信息。
  如果（备选Pod+节点已用资源>该节点总资源）则返回false，即剩余资源不满足该Pod使用；否则返回true。
 3.3. PodSelectorMatches
  判断节点是否包含备选Pod的标签选择器指定的标签，即通过标签来选择Node。

  如果Pod中没有指定spec.nodeSelector，则返回true。
  否则获得备选节点的标签信息，判断该节点的标签信息中是否包含该Pod的spec.nodeSelector中指定的标签，如果包含返回true，否则返回false。
3.4. PodFitsHost
  判断备选Pod的spec.nodeName所指定的节点名称与备选节点名称是否一致，如果一致返回true，否则返回false。

3.5. CheckNodeLabelPresence
  检查备选节点中是否有Scheduler配置的标签，如果有返回true，否则返回false。

 3.6. CheckServiceAffinity
  判断备选节点是否包含Scheduler配置的标签，如果有返回true，否则返回false。

  3.7. PodFitsPorts
  判断备选Pod所用的端口列表中的端口是否在备选节点中已被占用，如果被占用返回false，否则返回true。

3.8 PodToleratesNodeTaints

3.9 CheckVolumeBinding

3.10 CheckNodeMemoryPressure

3.11 CheckNodePIDPressure

3.12 CheckNodeDiskPressure

3.13 CheckNodeUnschedulable

4. 优选策略
    4.1. LeastRequestedPriority
    优先从备选节点列表中选择资源消耗最小的节点（CPU+内存）。

  4.2. CalculateNodeLabelPriority
    优先选择含有指定Label的节点。

  4.3. **BalancedResourceAllocation**
    优先从备选节点列表中选择各项资源使用率最均衡的节点。

  4.4 NodeAffinityPriority

  4.5 TaintTolerationPriority

* 节点亲和性、反亲和性

  * nodeAffinity

  * 硬亲和性 requiredDuringSchedulingIgnoredDuringExecution

  * 软亲和性 preferredDuringSchedulingIgnoredDuringExecution

    weight

  * topologyKey

* pod亲和性、反亲和性

  * podAffinity

  * 硬亲和性 requiredDuringSchedulingIgnoredDuringExecution

  * 软亲和性 preferredDuringSchedulingIgnoredDuringExecution

    weight

  * 反亲和性

    podAntiAffinity

  * topologyKey

* 节点污点taints

  * NoSchedule            不允许pod不能容忍的调度
  * PreferNoSchedule 不允许pod不能容忍的调度，但是没有其他节点可选的时候可以调度到此节点
  * NoExecute               不允许pod不能容忍的调度+污点变动驱逐
  * kubectl taint nodes <node-name> <key>=<value>:<effect>

* 节点容忍度tolerations

  * spec.tolerations: