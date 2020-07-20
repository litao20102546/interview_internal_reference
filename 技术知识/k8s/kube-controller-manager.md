Controller Manager作为集群内部的管理控制中心，负责集群内的Node、Pod副本、服务端点（Endpoint）、命名空间（Namespace）、服务账号（ServiceAccount）、资源定额（ResourceQuota）的管理，当某个Node意外宕机时，Controller Manager会及时发现并执行自动化修复流程，确保集群始终处于预期的工作状态。

每个Controller通过API Server提供的接口实时监控整个集群的每个资源对象的当前状态，当发生各种故障导致系统状态发生变化时，会尝试将系统状态修复到“期望状态”。

# Replication Controller

## Replication Controller的职责

* 确保集群中有且仅有N个Pod实例，N是RC中定义的Pod副本数量。 
* 通过调整RC中的spec.replicas属性值来实现系统扩容或缩容。 
* 通过改变RC中的Pod模板来实现系统的滚动升级。

## Replication Controller使用场景

* 重新调度 当发生节点故障或Pod被意外终止运行时，可以重新调度保证集群中仍然运行指定的副本数。
* 弹性伸缩 通过手动或自动扩容代理修复副本控制器的spec.replicas属性，可以实现弹性伸缩。 
* kubectl scale滚动更新

# Node Controller

kubelet在启动时会通过API Server注册自身的节点信息，并定时向API Server汇报状态信息，API Server接收到信息后将信息更新到etcd中。

# Garbage collector controller 

## 删除策略

* `Orphan` 策略：非级联删除，删除对象时，不会自动删除它的依赖或者是子对象，这些依赖被称作是原对象的孤儿对象，例如当执行以下命令时会使用 `Orphan` 策略进行删除，此时 ds 的依赖对象 `controllerrevision` 不会被删除；
* `Background` 策略：在该模式下，kubernetes 会立即删除该对象，然后垃圾收集器会在后台删除这些该对象的依赖对象；
* `Foreground` 策略：在该模式下，对象首先进入“删除中”状态，即会设置对象的 `deletionTimestamp` 字段并且对象的 `metadata.finalizers` 字段包含了值 “foregroundDeletion”，此时该对象依然存在，然后垃圾收集器会删除该对象的所有依赖对象，垃圾收集器在删除了所有“Blocking” 状态的依赖对象（指其子对象中 `ownerReference.blockOwnerDeletion=true`的对象）之后，然后才会删除对象本身；

## finalizer 机制

* finalizer 是在删除对象时设置的一个 hook，其目的是为了让对象在删除前确认其子对象已经被完全删除
* k8s 中默认有两种 finalizer：`OrphanFinalizer` 和 `ForegroundFinalizer`
* finalizer 存在于对象的 ObjectMeta 中，当一个对象的依赖对象被删除后其对应的 finalizers 字段也会被移除，只有 finalizers 字段为空时，apiserver 才会删除该对象。

## 流程

1. GarbageCollectorController 是一种典型的生产者消费者模型，所有 `deletableResources` 的 informer 都是生产者，每种资源的 informer 监听到变化后都会将对应的事件 push 到 graphChanges 中，graphChanges 是 GraphBuilder 对象中的一个数据结构
2. GraphBuilder 会启动另外的 goroutine 对 graphChanges 中的事件进行分类并放在其 attemptToDelete 和 attemptToOrphan 两个队列中
3. garbageCollector 会启动多个 goroutine 对 attemptToDelete 和 attemptToOrphan 两个队列中的事件进行处理，处理的结果就是回收一些需要被删除的对象。最后，再用一个流程图总结一下 

```
                 monitors (producer)                          
                     |                            
                     |                           
                     ∨                    
                 graphChanges queue                            
                     |                            
                     |                           
                     ∨                    
                 processGraphChanges                            
                     |                            
                     |                           
                     ∨            
           -------------------------------           
           |                             |           
           |                             |           
           ∨                             ∨ 
           attemptToDelete queue         attemptToOrphan queue           
           |                             |           
           |                             |            
           ∨                             ∨    
           AttemptToDeleteWorker       AttemptToOrphanWorker        
           (consumer)                    (consumer)
```

## 实际的删除动作

GarbageCollectorController 会监听集群中所有可删除资源产生的所有事件，这些事件会被放入到一个队列中，然后 controller 会启动多个 goroutine 处理队列中的事件，若为删除事件会根据对象的删除策略删除关联的对象，对于非删除事件会更新对象之间的依赖关系。

`gc.attemptToDeleteItem` 执行实际的删除操作：

- 1、判断 node 是否处于删除状态；

- 2、从 apiserver 获取该 node 最新的状态，该 node 可能为 virtual node，若为 virtual node 则从 apiserver 中获取不到该 node 的对象，此时会将该 node 重新加入到 graphChanges 队列中，再次处理该 node 时会将其从 uidToNode 中删除；

- 3、判断该 node 最新状态的 uid 是否等于本地缓存中的 uid，若不匹配说明该 node 已更新过此时将其设置为 virtual node 并重新加入到 graphChanges 队列中，再次处理该 node 时会将其从 uidToNode 中删除；

- 4、通过 node 的 `deletingDependents` 字段判断该 node 当前是否处于删除 dependents 的状态，若该 node 处于删除 dependents 的状态则调用 `processDeletingDependentsItem` 方法检查 node 的 `blockingDependents` 是否被完全删除，若 `blockingDependents` 已完全被删除则删除该 node 对应的 finalizer，若 `blockingDependents` 还未删除完，将未删除的 `blockingDependents` 加入到 attemptToDelete 中；

  上文中在 GraphBuilder 处理 graphChanges 中的事件时，若发现 node 处于删除状态，会将 node 的 dependents 加入到 attemptToDelete 中并标记 node 的 `deletingDependents` 为 true；

- 5、调用 `gc.classifyReferences` 将 node 的 `ownerReferences` 分类为 `solid`, `dangling`, `waitingForDependentsDeletion` 三类：`dangling`(owner 不存在)、`waitingForDependentsDeletion`(owner 存在，owner 处于删除状态且正在等待其 dependents 被删除)、`solid`(至少有一个 owner 存在且不处于删除状态)；

- 6、对以上分类进行不同的处理，若 `solid`不为 0 即当前 node 至少存在一个 owner，该对象还不能被回收，此时需要将 `dangling` 和 `waitingForDependentsDeletion` 列表中的 owner 从 node 的 `ownerReferences` 删除，即已经被删除或等待删除的引用从对象中删掉；

- 7、第二种情况是该 node 的 owner 处于 `waitingForDependentsDeletion` 状态并且 node 的 dependents 未被完全删除，该 node 需要等待删除完所有的 dependents 后才能被删除；

- 8、第三种情况就是该 node 已经没有任何 dependents 了，此时按照 node 中声明的删除策略调用 apiserver 的接口删除即可；



# deployment controller 

deployment 是 kubernetes 中用来部署无状态应用的一个对象

deployment 的本质是控制 replicaSet，replicaSet 会控制 pod，然后由 controller 驱动各个对象达到期望状态。

<img src=deployment.png style="zoom:50%;" />

DeploymentController 是 Deployment 资源的控制器，其通过 DeploymentInformer、ReplicaSetInformer、PodInformer 监听三种资源，当三种资源变化时会触发 DeploymentController 中的 syncLoop 操作。