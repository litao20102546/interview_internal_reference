

## iptables

https://www.bookstack.cn/read/source-code-reading-notes/kubernetes-kube_proxy_iptables.md

### iptables 的功能

在前面的文章中已经介绍过 iptable 的一些基本信息，本文会深入介绍 kube-proxy iptables 模式下的工作原理，本文中多处会与 iptables 的知识相关联，若没有 iptables 基础，请先自行补充。

iptables 的功能：

- 流量转发：DNAT 实现 IP 地址和端口的映射；
- 负载均衡：statistic 模块为每个后端设置权重；
- 会话保持：recent 模块设置会话保持时间；

iptables 有五张表和五条链，五条链分别对应为：

- PREROUTING 链：数据包进入路由之前，可以在此处进行 DNAT；
- INPUT 链：一般处理本地进程的数据包，目的地址为本机；
- FORWARD 链：一般处理转发到其他机器或者 network namespace 的数据包；
- OUTPUT 链：原地址为本机，向外发送，一般处理本地进程的输出数据包；
- POSTROUTING 链：发送到网卡之前，可以在此处进行 SNAT；

五张表分别为：

- filter 表：用于控制到达某条链上的数据包是继续放行、直接丢弃(drop)还是拒绝(reject)；
- nat 表：network address translation 网络地址转换，用于修改数据包的源地址和目的地址；
- mangle 表：用于修改数据包的 IP 头信息；
- raw 表：iptables 是有状态的，其对数据包有链接追踪机制，连接追踪信息在 /proc/net/nf_conntrack 中可以看到记录，而 raw 是用来去除链接追踪机制的；
- security 表：最不常用的表，用在 SELinux 上；

这五张表是对 iptables 所有规则的逻辑集群且是有顺序的，当数据包到达某一条链时会按表的顺序进行处理，表的优先级为：raw、mangle、nat、filter、security。





## ipvs

[https://www.bookstack.cn/read/source-code-reading-notes/kubernetes-kube_proxy_ipvs.md#clusterIP%20%E8%AE%BF%E9%97%AE%E6%96%B9%E5%BC%8F](https://www.bookstack.cn/read/source-code-reading-notes/kubernetes-kube_proxy_ipvs.md#clusterIP 访问方式)

#### ipvs 与 iptables 的区别与联系

**区别**：

- 底层数据结构：iptables 使用链表，ipvs 使用哈希表
- 负载均衡算法：iptables 只支持随机、轮询两种负载均衡算法而 ipvs 支持的多达 8 种；
- 操作工具：iptables 需要使用 iptables 命令行工作来定义规则，ipvs 需要使用 ipvsadm 来定义规则。

此外 ipvs 还支持 realserver 运行状况检查、连接重试、端口映射、会话保持等功能。

**联系**：

ipvs 和 iptables 都是基于 [netfilter内核模块](https://www.netfilter.org/)，两者都是在内核中的五个钩子函数处工作.



此外，由于 linux 内核原生的 ipvs 模式只支持 DNAT，不支持 SNAT，所以，在以下几种场景中 ipvs 仍需要依赖 iptables 规则：

- 1、kube-proxy 启动时指定 `–-masquerade-all=true` 参数，即集群中所有经过 kube-proxy 的包都做一次 SNAT；
- 2、kube-proxy 启动时指定 `--cluster-cidr=` 参数；
- 3、对于 Load Balancer 类型的 service，用于配置白名单；
- 4、对于 NodePort 类型的 service，用于配置 MASQUERADE；
- 5、对于 externalIPs 类型的 service；

但对于 ipvs 模式的 kube-proxy，无论有多少 pod/service，iptables 的规则数都是固定的。



当创建 ClusterIP type 的 service 时，IPVS proxier 会执行以下三个操作：

- 确保本机已创建 **dummy 网卡**，默认为 kube-ipvs0。为什么要创建 dummy 网卡？因为 ipvs netfilter 的 DNAT 钩子挂载在 INPUT 链上，当访问 ClusterIP 时，将 ClusterIP 绑定在 dummy 网卡上为了让内核识别该 IP 就是本机 IP，进而进入 INPUT 链，然后通过钩子函数 ip_vs_in 转发到 POSTROUTING 链；
- 将 ClusterIP **绑定**到 dummy 网卡；
- 为每个 ClusterIP 创建 IPVS virtual servers 和 real server，分别对应 service 和 endpoints；

# BPF

BPF（Berkeley Packet Filter）是 [Linux](http://www.codercto.com/category/linux.html) 内核中的一个虚拟机

BPF技术本质上是一个运行在内核态的虚拟机。它最初的目的是为了避免包过滤时从内核态到用户态的数据复制，因此提供了一种方法让用户在内核态中定义处理数据包，这种方法就是BPF JIT虚拟机。它允许用户注册一段受到严格限制的自定义代码到BPF上，这段代码通过了Verifier检测后，JIT会将其编译为机器码，并将其插桩到特定的内核代码上，实现类似于探针的效果。

<img src=bpf1.png />

* 监控工具bcc
* 

https://baijiahao.baidu.com/s?id=1621543051336579437&wfr=spider&for=pc

https://zhuanlan.zhihu.com/p/137960916

https://www.kubernetes.org.cn/5323.html

https://linux.cn/article-9032-1.html