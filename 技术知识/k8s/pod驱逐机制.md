**evictionManager 当节点的内存、磁盘或 inode 等资源不足时，达到了配置的 evict 策略， node 会变为 pressure 状态，此时 kubelet 会按照 qosClass 顺序来驱赶 pod，以此来保证节点的稳定性。可以通过配置 kubelet 启动参数 `--eviction-hard=` 来决定 evict 的策略值。**

## pod驱逐机制

* kubelet 持续监控主机的资源使用情况
* kubelet主动终止一个或者多个pod的运行

## 驱逐信号

* memory.available
* nodefs.available
* nodefs.inodesFree
* imagefs.available
* imagefs.inodesFree

## 驱逐阈值

软阈值有宽限期，硬阈值没有。

* 软阈值 --eviction-soft --eviction-soft-grace-period（驱逐宽限期）
* 硬阈值 --eviction-hard

## 节点状况

* MemoryPressure
* DiskPressure
* 防抖动 --eviction-pressure-transition-period

## 驱逐顺序

1. 节点级别的资源

   1.1 删除死掉的pod, container

   1.2 删除无用的镜像

2. 用户的pod

   2.1 Guaranteed (消耗最多的值优先被驱逐)

   2.2 Burstable（相对值大的优先被驱逐，相同情况消耗最多的值优先被驱逐）

   2.3 BestEffort（相对值大的优先被驱逐，相同情况消耗最多的值优先被驱逐）

## 资源最小回收量

* 避免因为回收少然后反复触发驱逐
* eviction-minimum-reclaim

## PDB (主动驱逐保护)

* 对于需要保持存活POD数量非常有用，系统每终止一个Pod后一定会在其他节点启动新Pod，然后再终止下一个Pod
* PDB本身无法保证Pod的存活的百分比和数量。
* minAvailable 驱逐过程中需要保证的最小数量