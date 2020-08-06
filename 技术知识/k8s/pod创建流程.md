client->apiserver()->etcd(更新元数据)->apiserver()->kube-scheduler->apiserver->etcd

## apiserver:

 http filter chain 中注册的过滤(认证，鉴权)->解码 -》准入控制 -》检查 object 中字段的合法性

```
v1beta1 ⇒ internal ⇒    |    ⇒       |    ⇒  v1  ⇒ json/yaml ⇒ etcd                     										admission    validation
```

## kube-scheduler

1. 获取优先队列中一个优先级队列最高的待调度pod资源
2. 预选
3. 优选
4. 【抢占】
5. 选取一个score分最高的节点
6. bind

 ## kubelet 创建 pod 的流程

- [1、kubelet 的控制循环（syncLoop）](https://www.bookstack.cn/read/source-code-reading-notes/kubernetes-kubelet_create_pod.md#1、kubelet 的控制循环（syncLoop）)
- [2、监听 pod 变化（syncLoopIteration）](https://www.bookstack.cn/read/source-code-reading-notes/kubernetes-kubelet_create_pod.md#2、监听 pod 变化（syncLoopIteration）)
- [3、处理新增 pod（HandlePodAddtions）](https://www.bookstack.cn/read/source-code-reading-notes/kubernetes-kubelet_create_pod.md#3、处理新增 pod（HandlePodAddtions）)
- [4、下发任务（dispatchWork）](https://www.bookstack.cn/read/source-code-reading-notes/kubernetes-kubelet_create_pod.md#4、下发任务（dispatchWork）)
- [5、更新事件的 channel（UpdatePod）](https://www.bookstack.cn/read/source-code-reading-notes/kubernetes-kubelet_create_pod.md#5、更新事件的 channel（UpdatePod）)
- [6、调用 syncPodFn 方法同步 pod（managePodLoop）](https://www.bookstack.cn/read/source-code-reading-notes/kubernetes-kubelet_create_pod.md#6、调用 syncPodFn 方法同步 pod（managePodLoop）)
- [7、完成创建容器前的准备工作（SyncPod）](https://www.bookstack.cn/read/source-code-reading-notes/kubernetes-kubelet_create_pod.md#7、完成创建容器前的准备工作（SyncPod）)
  - 检查 pod 是否能运行在本节点，主要是权限检查（是否能使用主机网络模式，是否可以以 privileged 权限运行等）。如果没有权限，就删除本地旧的 pod 并返回错误信息
  - 创建 containerManagar 对象，并且创建 pod level cgroup，更新 Qos level cgroup
  - 如果是 static Pod，就创建或者更新对应的 mirrorPod
  - 创建 pod 的数据目录，存放 volume 和 plugin 信息,如果定义了 pv，等待所有的 volume mount 完成（volumeManager 会在后台做这些事情）,如果有 image secrets，去 apiserver 获取对应的 secrets 数据
  - 然后调用 kubelet.volumeManager 组件，等待它将 pod 所需要的所有外挂的 volume 都准备好。
- [8、创建容器](https://www.bookstack.cn/read/source-code-reading-notes/kubernetes-kubelet_create_pod.md#8、创建容器)
  - 1、计算 sandbox 和 container 是否发生变化
  - 2、创建 sandbox 容器
  - 3、启动 init 容器
  - 4、启动业务容器
- [9、启动容器](https://www.bookstack.cn/read/source-code-reading-notes/kubernetes-kubelet_create_pod.md#9、启动容器)
  - 1、拉取镜像
  - 2、生成业务容器的配置信息
  - 3、调用 docker api 创建容器
  - 4、启动容器
  - 5、执行 post start hook