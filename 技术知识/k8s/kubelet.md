

# kubelet 中事件处理机制

当集群中的 node 或 pod 异常时，大部分用户会使用 kubectl 查看对应的 events，那么 events 是从何而来的？其实 k8s 中的各个组件会将运行时产生的各种事件汇报到 apiserver

* Kubelet 在启动的时候会初始化一个 EventBroadcaster，它主要是对接收到的 events 做一些后续的处理(保存、上报等
  * StartEventWatcher() ： EventBroadcaster 中的核心方法，接收各模块产生的 events，参数为一个处理 events 的函数，用户可以使用 StartEventWatcher() 接收 events 然后使用自定义的 handle 进行处理
  * StartRecordingToSink() ： 调用 StartEventWatcher() 接收 events，并将收到的 events 发送到 apiserver
  * StartLogging() ：也是调用 StartEventWatcher() 接收 events，然后保存 events 到日志
  * NewRecorder() ：会创建一个指定 EventSource 的 EventRecorder，EventSource 指明了哪个节点的哪个组件

* EventRecorder
* Broadcaster
  * Broadcaster 会启动一个 goroutine 接收各组件产生的 events 并广播到每一个 watcher
  * Broadcaster 中有一个 map 会保存每一个注册的 watcher

# kubelet 上报状态

分布式系统中服务端会通过心跳机制确认客户端是否存活，在 k8s 中，kubelet 也会定时上报心跳到 apiserver，以此判断该 node 是否存活，若 node 超过一定时间没有上报心跳，其状态会被置为 NotReady，宿主上容器的状态也会被置为 Nodelost 或者 Unknown 状态

kubelet 自身会定期更新状态到 apiserver，通过参数 `--node-status-update-frequency` 指定上报频率，默认是 10s 上报一次，kubelet 不止上报心跳信息还会上报自身的一些数据信息。

## kubelet 上报哪些状态

- [Addresses](https://kubernetes.io/docs/concepts/architecture/nodes/#addresses)
- [Condition](https://kubernetes.io/docs/concepts/architecture/nodes/#condition) 字段描述了所有 `Running` nodes 的状态
- [Capacity](https://kubernetes.io/docs/concepts/architecture/nodes/#capacity) 描述 node 上的可用资源：CPU、内存和可以调度到该 node 上的最大 pod 数量。
- [Info](https://kubernetes.io/docs/concepts/architecture/nodes/#info) 描述 node 的一些通用信息，例如内核版本、Kubernetes 版本（kubelet 和 kube-proxy 版本）、Docker 版本 （如果使用了）和系统版本
- 对于二次开发而言，如果我们需要 APIServer 掌握更多的 Node 信息，可以在此处添加自定义函数defaultNodeStatusFuncs

# kubelet创建pod流程

1. kubelet 的控制循环（syncLoop）
2. 监听 pod 变化（syncLoopIteration）
3. 处理新增 pod（HandlePodAddtions）
4. 下发任务（dispatchWork）
5. 更新事件的 channel（UpdatePod）
6. 调用 syncPodFn 方法同步 pod（managePodLoop）
7. 完成创建容器前的准备工作（SyncPod）
8. 创建容器
9. 启动容器

### 1、kubelet 的控制循环（syncLoop）

syncLoop 中首先定义了一个 syncTicker 和 housekeeping**Ticker**，即使没有需要更新的 pod 配置，kubelet 也会定时去做同步和清理 pod 的工作。然后在 for 循环中一直调用 syncLoopIteration，如果在每次循环过程中出现比较严重的错误，kubelet 会记录到 runtimeState 中，遇到错误就等待 5 秒中继续循环。

### 2、监听 pod 变化（syncLoopIteration）

syncLoopIteration 这个方法就会对多个管道进行遍历，发现任何一个管道有消息就交给 handler 去处理。它会从以下管道中获取消息：

- configCh：该信息源由 kubeDeps 对象中的 PodConfig 子模块提供，该模块将同时 watch 3 个不同来源的 pod 信息的变化（file，http，apiserver），一旦某个来源的 pod 信息发生了更新（创建/更新/删除），这个 channel 中就会出现被更新的 pod 信息和更新的具体操作。
- syncCh：定时器管道，每隔一秒去同步最新保存的 pod 状态
- houseKeepingCh：housekeeping 事件的管道，做 pod 清理工作
- plegCh：该信息源由 kubelet 对象中的 pleg 子模块提供，该模块主要用于周期性地向 container runtime 查询当前所有容器的状态，如果状态发生变化，则这个 channel 产生事件。
- livenessManager.Updates()：健康检查发现某个 pod 不可用，kubelet 将根据 Pod 的restartPolicy 自动执行正确的操作

### 3、处理新增 pod（HandlePodAddtions）

对于事件中的每个 pod，执行以下操作：

- 1、把所有的 pod 按照创建日期进行排序，保证最先创建的 pod 会最先被处理
- 2、把它加入到 podManager 中，podManager 子模块负责管理这台机器上的 pod 的信息，pod 和 mirrorPod 之间的对应关系等等。所有被管理的 pod 都要出现在里面，如果 podManager 中找不到某个 pod，就认为这个 pod 被删除了
- 3、如果是 mirror pod 调用其单独的方法
- 4、验证 pod 是否能在该节点运行，如果不可以直接拒绝
- 5、通过 dispatchWork 把创建 pod 的工作下发给 podWorkers 子模块做异步处理
- 6、在 probeManager 中添加 pod，如果 pod 中定义了 readiness 和 liveness 健康检查，启动 goroutine 定期进行检测

### 4、下发任务（dispatchWork）

dispatchWorker 的主要作用是把某个对 Pod 的操作（创建/更新/删除）下发给 podWorkers。

### 5、更新事件的 channel（UpdatePod）

podWorkers 子模块主要的作用就是处理针对每一个的 Pod 的更新事件，比如 Pod 的创建，删除，更新。而 podWorkers 采取的基本思路是：为每一个 Pod 都单独创建一个 goroutine 和更新事件的 channel，goroutine 会阻塞式的等待 channel 中的事件，并且对获取的事件进行处理。而 podWorkers 对象自身则主要负责对更新事件进行下发。

### 6、调用 syncPodFn 方法同步 pod（managePodLoop）

managePodLoop 调用 syncPodFn 方法去同步 pod，syncPodFn 实际上就是kubelet.SyncPod。在完成这次 sync 动作之后，会调用 wrapUp 函数，这个函数将会做几件事情:

- 将这个 pod 信息插入 kubelet 的 workQueue 队列中，等待下一次周期性的对这个 pod 的状态进行 sync
- 将在这次 sync 期间堆积的没有能够来得及处理的最近一次 update 操作加入 goroutine 的事件 channel 中，立即处理。

### 7. 完成创建容器前的准备工作（SyncPod）

在这个方法中，主要完成以下几件事情：

- 如果是删除 pod，立即执行并返回
- 同步 podStatus 到 kubelet.statusManager
- 检查 pod 是否能运行在本节点，主要是权限检查（是否能使用主机网络模式，是否可以以 privileged 权限运行等）。如果没有权限，就删除本地旧的 pod 并返回错误信息
- 创建 containerManagar 对象，并且创建 pod level cgroup，更新 Qos level cgroup
- 如果是 static Pod，就创建或者更新对应的 mirrorPod
- 创建 pod 的数据目录，存放 volume 和 plugin 信息,如果定义了 pv，等待所有的 volume mount 完成（volumeManager 会在后台做这些事情）,如果有 image secrets，去 apiserver 获取对应的 secrets 数据
- 然后调用 kubelet.volumeManager 组件，等待它将 pod 所需要的所有外挂的 volume 都准备好。
- 调用 container runtime 的 SyncPod 方法，去实现真正的容器创建逻辑

### 8、创建容器

containerRuntime（pkg/kubelet/kuberuntime）子模块的 SyncPod 函数才是真正完成 pod 内容器实体的创建。 syncPod 主要执行以下几个操作：

- 1、计算 sandbox 和 container 是否发生变化
- 2、创建 sandbox 容器
- 3、启动 init 容器
- 4、启动业务容器

### 9、启动容器

最终由 startContainer 完成容器的启动，其主要有以下几个步骤：

- 1、拉取镜像
- 2、生成业务容器的配置信息
- 3、调用 docker api 创建容器
- 4、启动容器
- 5、执行 post start hook