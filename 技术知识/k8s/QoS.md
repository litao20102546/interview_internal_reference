### kubernetes 中的 Qos

QoS(Quality of Service) 即服务质量，QoS 是一种控制机制，它提供了针对不同用户或者不同数据流采用相应不同的优先级，或者是根据应用程序的要求，保证数据流的性能达到一定的水准。kubernetes 中有三种 Qos，分别为：

- 1、`Guaranteed`：pod 的 requests 与 limits 设定的值相等；
- 2、`Burstable`：pod requests 小于 limits 的值且不为 0；
- 3、`BestEffort`：pod 的 requests 与 limits 均为 0；

三者的优先级如下所示，依次递增：

```
BestEffort -> Burstable -> Guaranteed
```

## 区别

三种 Qos 在调度和底层表现上都不一样：

- 1、在调度时调度器只会根据 request 值进行调度；
- 2、二是当系统 OOM上时对于处理不同 OOMScore 的进程表现不同，OOMScore 是针对 memory 的，当宿主上 memory 不足时系统会优先 kill 掉 OOMScore 值高的进程，可以使用 `$ cat /proc/$PID/oom_score` 查看进程的 OOMScore。OOMScore 的取值范围为 [-1000, 1000]，`Guaranteed` pod 的默认值为 -998，`Burstable` pod 的值为 2~999，`BestEffort` pod 的值为 1000，也就是说当系统 OOM 时，首先会 kill 掉 `BestEffort` pod 的进程，若系统依然处于 OOM 状态，然后才会 kill 掉 `Burstable` pod，最后是 `Guaranteed` pod；
- 3、三是 cgroup 的配置不同，kubelet 为会三种 Qos 分别创建对应的 QoS level cgroups，`Guaranteed` Pod Qos 的 cgroup level 会直接创建在 `RootCgroup/kubepods` 下，`Burstable` Pod Qos 的创建在 `RootCgroup/kubepods/burstable` 下，`BestEffort` Pod Qos 的创建在 `RootCgroup/kubepods/BestEffort` 下，上文已经说了 root cgroup 可以通过 `$ mount | grep cgroup`看到，在 cgroup 的每个子系统下都会创建 Qos level cgroups， 此外在对应的 QoS level cgroups 还会为 pod 创建 Pod level cgroups；

## kubernetes 中的 cgroup level

* 目前 kubernetes 仅支持 cpu、memory、pids 、hugetlb 四个 cgroup 子系统。

当 kubernetes 在收到一个 pod 的资源申请信息后通过 kubelet 为 pod 分配资源，kubelet 基于 pod 申请的资源以及 pod 对应的 QoS 级别来通过 cgroup 机制最终为这个 pod 分配资源的，针对每一种资源，它会做以下几件事情：

- 首先判断 pod 属于**哪种 Qos**，在对应的 Qos level cgroup 下对 pod 中的每一个容器在 cgroup 所有子系统下都创建一个 pod level cgroup 以及 container level cgroup，并且 pod level cgroup 是 container level cgroup 的父 cgroup，Qos level cgroup 在 kubelet 初始化时已经创建完成了；
- 然后根据 pod 的资源信息更新 QoS level cgroup 中的值；
- 最后会更新 `kubepods` level cgroup 中的值；

对于每一个 pod 设定的 requests 和 limits，kubernetes 都会转换为 cgroup 中的计算方式，CPU 的转换方式如下所示：

- cpu.shares = (cpu in millicores * 1024) / 1000
- cpu.cfs_period_us = 100000 (i.e. 100ms)
- cpu.cfs_quota_us = quota = (cpu in millicores * 100000) / 1000
- memory.limit_in_bytes

CPU 最终都会转换为以微秒为单位，memory 会转换为以 bytes 为单位。

<!image src=/>img 



<img src=Qos-level.png />

当创建资源如下所示的 pod, 首先会根据 pod 的 Qos 该 pod 为 burstable 在其所属 Qos 下创建 `ROOT/kubepods/burstable/pod<UID>/container<UID>` 两个 cgroup level，然后会更新 pod 的父 cgroup 也就是 `burstable/` cgroup 中的值，最后会更新 `kubepods` cgroup 中的值，