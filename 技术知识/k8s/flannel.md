
Flannel实质上是一种“覆盖网络(overlaynetwork)”，也就是将TCP数据包装在另一种网络包里面进行路由转发和通信，目前已经支持udp、vxlan、host-gw、aws-vpc、gce和alloc路由等数据转发方式，默认的节点间数据通信方式是VxLan。

# Vxlan基础知识
VXLAN 全称是 Virtual eXtensible Local Area Network，虚拟可扩展的局域网。它是一种 overlay 技术，通过三层的网络来搭建虚拟的二层网络。

 

每一个技术出来都有它要解决的问题，VXLAN 也不例外，那么我们先看看 VXLAN 到底要解决哪些问题。

* 虚拟化（虚拟机和容器）的兴起使得一个数据中心会有成千上万的机器需要通信，而传统的 VLAN 技术只能支持 4096 个网络上限，已经满足不了不断扩展的数据中心规模
* 越来越多的数据中心（尤其是公有云服务）需要提供多租户的功能，不同用户之间需要独立地分配 ip 和 MAC 地址，如何保证这个功能的扩展性和正确性也是一个待解决的问题
* 云计算业务对业务灵活性要求很高，虚拟机可能会大规模迁移，并保证网络一直可用，也就是大二层的概念。解决这个问题同时保证二层的广播域不会过分扩大，也是云计算网络的要求

## Vxlan模型
vxlan 这类隧道网络的一个特点是对原有的网络架构影响小，原来的网络不需要做任何改动，在原来网络基础上架设一层新的网络。

vxlan 自然会引入一些新的概念，这部分就讲讲它们。下面这张图 是 vxlan 的工作模型，它创建在原来的 IP 网络（三层）上，只要是三层可达（能够通过 IP 互相通信）的网络就能部署 vxlan。在每个端点上都有一个 vtep 负责 vxlan 协议报文的封包和解包，也就是在虚拟报文上封装 vtep 通信的报文头部。物理网络上可以创建多个 vxlan 网络，这些 vxlan 网络可以认为是一个隧道，不同节点的虚拟机能够通过隧道直连。每个 vxlan 网络由唯一的 VNI 标识，不同的 vxlan 可以不相互影响。

<img src=vxlan1.png />

* VTEP（VXLAN Tunnel Endpoints）：vxlan 网络的边缘设备，用来进行 vxlan 报文的处理（封包和解包）。vtep 可以是网络设备（比如交换机），也可以是一台机器（比如虚拟化集群中的宿主机）
* VNI（VXLAN Network Identifier）：VNI 是每个 vxlan 的标识，是个 24 位整数，一共有 2^24 = 16,777,216（一千多万），一般每个 VNI 对应一个租户，也就是说使用 vxlan 搭建的公有云可以理论上可以支撑千万级别的租户
* Tunnel：隧道是一个逻辑上的概念，在 vxlan 模型中并没有具体的物理实体想对应。隧道可以看做是一种虚拟通道，vxlan 通信双方（图中的虚拟机）认为自己是在直接通信，并不知道底层网络的存在。从整体来说，每个 vxlan 网络像是为通信的虚拟机搭建了一个单独的通信通道，也就是隧道

## vxlan报文解析
vxlan 在三层网络上构建一个虚拟的二层网络出来，这一点能够在 vxlan 的报文上很明显地体现出来。

下图是 vxlan 协议的报文，白色的部分是虚拟机发送报文（二层帧，包含了 MAC 头部、IP 头部和传输层头部的报文），前面加了 vxlan 头部用来专门保存 vxlan 相关的内容，在前面是标准的 UDP 协议头部（UDP 头部、IP 头部和 MAC 头部）用来在底层网路上传输报文。

<img src=vxlan2.png />

从这个报文中可以看到三个部分：

* 最外层的 UDP 协议报文用来在底层网络上传输，也就是 vtep
  之间互相通信的基础
* 中间是 VXLAN 头部，vtep 接受到报文之后，去除前面的 UDP 协议部分，根据这部分来处理 vxlan 的逻辑，主要是根据 VNI 发送到最终的虚拟机
* 最里面是原始的报文，也就是虚拟机看到的报文内容


报文各个部分的意义如下：

* VXLAN header：vxlan 协议相关的部分，一共 8 个字节
  * VXLAN flags：标志位
  * Reserved：保留位
  * VNID：24 位的 VNI 字段，这也是 vxlan 能支持千万租户的地方
  * Reserved：保留字段
* UDP 头部，8 个字节
  UDP 应用通信双方是 vtep 应用，其中目的端口就是接收方 vtep 使用的端口，IANA 分配的端口是 4789
* IP 头部：20 字节
  主机之间通信的地址，可能是主机的网卡 IP 地址，也可能是多播 IP 地址
* MAC 头部：14 字节
  主机之间通信的 MAC 地址，源 MAC 地址为主机 MAC 地址，目的 MAC 地址为下一跳设备的 MAC 地址


可以看出 vxlan 协议比原始报文多 50 字节的内容，这会降低网络链路传输有效数据的比例。vxlan 头部最重要的是 VNID 字段，其他的保留字段主要是为了未来的扩展，目前留给不同的厂商用这些字段添加自己的功能。

## vxlan网络通信过程
* 问题
  通过上节的内容，我们大致了解 vxlan 报文的发送过程。虚拟机的报文通过 vtep 添加上 vxlan 以及外部的报文层，然后发送出去，对方 vtep 收到之后拆除 vxlan 头部然后根据 VNI 把原始报文发送到目的虚拟机。

  上面的过程是双方已经知道所有通信信息的过程，但是在第一次通信之前还有很多问题有解决：

  * 哪些 vtep 需要加到一个相同的 VNI 组？
  * 发送方虚拟机怎么知道对方的 MAC 地址？
  * vtep 怎么知道目的虚拟机在哪一台宿主机上？

* 解决方法
  这三个问题可以归结为同一个问题：vxlan 网络怎么感知彼此的存在并选择正确的路径传输报文？

  而且第一个问题也是不用回答的，因为 vtep 形成的组是虚构的概念，只有某些 vtep 能够正确地传递报文，它们就是在同一个组内。也就是说，我们只要回答后面两个问题就行。

  要回答这两个问题，我们还是回到 vxlan 协议报文上，看看一个完整的 vxlan 报文需要哪些信息。

  内层报文：通信的虚拟机双方要么直接使用 IP 地址，要么通过 DNS 等方式已经获取了对方的 IP 地址因此网络层地址已经知道。同一个网络的虚拟机需要通信，还需要知道对方虚拟机的 MAC 地址，vxlan 需要一个机制来实现传统网络 ARP 的功能

  * vxlan 头部：只需要知道 VNI，这一般是直接配置在 vtep 上的，要么是提前规划写死的，要么是根据内部报文自动生成的，也不需要担心
  * UDP 头部：最重要的是源地址和目的地址的端口，源地址端口是系统生成并管理的，目的端口也是写死的，比如 IANA 规定的 4789 端口，这部分也不需要担心
  * IP 头部：IP 头部关心的是 vtep 双方的 IP 地址，源地址可以很简单确定，目的地址是虚拟机所在地址宿主机 vtep 的 IP 地址，这个也需要由某种方式来确定
  * MAC 头部：如果 vtep 的 IP 地址确定了，MAC 地址可以通过经典的 ARP 方式来获取，毕竟 vtep 网络在同一个三层，经典网络架构那一套就能直接用了


总结一下，一个 vxlan 报文需要确定两个地址信息：目的虚拟机的 MAC 地址和目的 vtep 的 IP 地址，如果 VNI 也是动态感知的，那么 vtep 就需要一个三元组：

* 内部 MAC <--> VNI <--> VTEP IP



**根据实现的不同，一般分为两种方式：多播和控制中心**

### 多播

简单来说，每个多播组对应一个多播 IP 地址，往这个多播 IP 地址发送的报文会发给多播组的所有主机。

为什么要使用多播？因为 vxlan 的底层网络是三层的，广播地址无法穿越三层网络，要给 vxlan 网络所有 vtep 发送报文只能通过多播。

下图是在多播模式下，vxlan 的报文工作流程，位于左下方的 机器 A 要通过 vxlan 网络发送报文给右下方的机器 B。

<img src=vxlan3.png />

 

vtep 建立的时候会通过配置加入到多播组，图中的多播组 IP 地址是 239.1.1.1。

1. 机器 A 只知道对方的 IP 地址，不知道 MAC 地址，因此会发送 ARP 报文进行查询，内部的 ARP 报文很普通，目标地址为全 1 的广播地址
2. vtep 收到 ARP 报文，发现虚拟机目的 MAC 为广播地址，封装上 vxlan 协议头部之后（外层 IP 为多播组 IP，MAC 地址为多播组的 MAC 地址），发送给多播组 239.1.1.1，支持多播的底层网络设备（交换机和路由器）会把报文发送给组内所有的成员
3. vtep 接收到 vxlan 封装的 ARP 请求，去掉 vxlan 头部，并通过报文学习到发送方 <虚拟机 MAC - VNI - Vtep IP> 三元组保存起来，把原来的 ARP 报文广播给主机
4. 主机接收到 ARP 请求报文，根据目的IP判断如果 ARP 报文请求的是自己的 MAC 地址，就返回 ARP 应答
   vtep-2 此时已经知道发送方的虚拟机和 vtep 信息，把 ARP 应答添加上 vxlan 头部（外部 IP 地址为 vtep-1 的 IP 地址，VNI 是原来报文的 VNI）之后通过单播发送出去
5. vtep-1 接收到报文，并学习到报文中的三元组，记录下来。然后 vtep 进行解包，知道内部的 IP 和 MAC 地址，并转发给目的虚拟机
6. 虚拟机拿到 ARP 应答报文，就知道了到目的虚拟机的 MAC 地址

在这个过程中，只有一次多播，因为 vtep 有自动学习的能力，后续的报文都是通过单播直接发送的。可以看到，多播报文非常浪费，每次的多播其实只有一个报文是有效的，如果某个多播组的 vtep 数量很多，这个浪费是非常大的。但是多播组也有它的实现起来比较简单，不需要中心化的控制，只有底层网络支持多播，只有配置好多播组就能自动发现了。

单播报文的发送过程就是上述应答报文的逻辑，应该也非常容易理解了。还有一种通信方式，那就是不同 VNI 网络之间的通信，这个需要用到 vxlan 网关（可以是物理网络设备，也可以是软件），它接收到一个 vxlan 网络报文之后解压，根据特定的逻辑添加上另外一个 vxlan 头部转发出去。

因为并不是所有的网络设备都支持多播，再加上多播方式带来的报文浪费，在实际生产中这种方式很少用到。



### 分布式控制中心

从多播的流程可以看出来，其实 vtep 发送报文最关键的就是知道对方虚拟机的 MAC 地址和虚拟机所在主机的 vtep IP 地址。如果能够事先知道这两个信息，直接告诉 vtep，那么就不需要多播了。

在虚拟机和容器的场景中，当虚拟机或者容器启动还没有进行网络通讯时，我们就可以知道它的 IP 和 MAC（可能是用某种方式获取，也有可能是事先控制这两个地址），分布式控制中心保存了这些信息。除此之外，控制中心还保存了每个 vxlan 网络有哪些 vtep，这些 vtep 的地址是多少。有了这些信息，vtep 就能发送报文时直接查询并添加头部，不需要多播去满网络地问了。

一般情况下，在每个 vtep 所在的节点都会有一个 agent，它会和控制中心通信，获取 vtep 需要的信息以某种方式告诉 vtep。具体的做法取决于具体的实现，每种实现可能会更新不同的信息给 vtep，比如 HER（Head End Replication）只是把多播组替换成多个单播报文，也就是把多播组所有的 VTEP IP 地址告诉 vtep，这样查询的时候不是发送多播，而是给组内每个 vtep 发送一个单播报文；有些实现只是告诉 vtep 目的虚拟机的 MAC 地址信息；有些实现告诉 MAC 地址对应的 vtep IP 地址。

此外，什么时候告诉 vtep 这些信息也是有区别的。一般有两种方式：常见的是一旦知道了虚拟机的三元组信息就告诉 vtep（即使某个 vtep 用不到这个信息，因为它管理的虚拟机不会和这个地址通信），一般这时候第一次通信还没有发生；另外一种方式是在第一次通信时，当 vtep 需要这些信息的时候以某种方式通知 agent，然后 agent 这时候才告诉 vtep 信息。

分布式控制的 vxlan 是一种典型的 SDN 架构，也是目前使用最广泛的方式。因为它的实现多样，而且每种实现都有些许差距，这里不便来具体的例子来说明，只要明白了上面的原理，不管是什么样的实现，都能很快上手

**K8S Flannel 就是利用分布式控制中心实现的，用ETCD数据库作为控制中心，保存每个节点的vtep信息，利用flanneld进程和ETCD数据库通信完成对端的vtep信息的查询。**

# flannel

## install 
[root@master ~]# kubectl apply -f https://raw.githubusercontent.com/coreos/flannel/master/Documentation/kube-flannel.yml
[root@master ~]# curl -sSL "https://github.com/coreos/flannel/blob/master/Documentation/kube-flannel.yml?raw=true" | kubectl create -f -

## flannel对网络提出的解决方法
* ip冲突
  flannel利用Kubernetes API或者etcd用于存储整个集群的网络配置，根据配置记录集群使用的网段。
  flannel在每个主机中运行flanneld作为agent，它会为所在主机从集群的网络地址空间中，获取一个小的网段subnet，本主机内所有容器的IP地址都将从中分配。
* pod之间互相访问
  flanneld将本主机获取的subnet以及用于主机间通信的Public IP通过etcd存储起来，需要时发送给相应模块。
  flannel利用各种backend mechanism，例如udp，vxlan等等，跨主机转发容器间的网络流量，完成容器间的跨主机通信。

## flannel基本原理
### 架构图
<img src=flannel.png />



* Cni0:网桥设备，每创建一个pod都会创建一对 veth pair。其中一端是pod中的eth0，另一端是Cni0网桥中的端口（网卡）。
* Pod中从网卡eth0发出的流量都会发送到Cni0网桥设备的端口（网卡）上。
* Flannel.1: overlay网络的设备，用来进行 vxlan 报文的处理（封包和解包）。不同node之间的pod数据流量都从overlay设备以隧道的形式发送到对端。
* Flanneld：flannel在每个主机中运行flanneld作为agent，它会为所在主机从集群的网络地址空间中，获取一个小的网段subnet，本主机内所有容器的IP地址都将从中分配。
  同时Flanneld监听K8s集群数据库，为flannel.1设备提供封装数据时必要的mac，ip等网络数据信息。

## 不同node上pod间的通信流程
1. pod->cni0

根据路由匹配到最后一条路由表项，找到所有的包转发到cni0

2. cni0->flannel.1

当ICMP包到达cni0后，cni0根据dst ip 查找主机路由表，根据最小匹配原则匹配到去往flannel.1的路由表并转发包到网关设备flannel.1



3. flannel.1->对端flanne.1

**内层封装**

flannel.1为vxlan设备，当数据包来到flannel.1时，需要将数据包封装起来。但是需要知道对端目的pod的mac地址，此时，**flannel.1不会发送arp请求去获得该mac地址，而是由Linux kernel将一个“L2 Miss”事件请求发送的用户空间的flanned程序。Flanned程序收到内核的请求事件之后，从etcd查找能够匹配该地址的子网的对端flannel.1设备的mac地址，即发往的pod所在host中flannel.1设备的mac地址**。（Flannel在为Node节点分配ip网段时记录了所有的网段和mac等信息，所以能够知道。）flanned将查询到的信息放入本地的arp cache表中，然后用对端的flannel.1的Mac地址作为目的pod的Mac地址（此处是为了ARP欺骗）。

**外层封装**

kernel需要查看node上的fdb(forwarding database)以获得内层封包中目的vtep设备所在的node地址。因为已经从arp table中查到目的设备mac地址，同时在fdb中存在该mac地址对应的node节点的IP地址。如果fdb中没有这个信息，那么kernel会向用户空间的flanned程序发起”L2 MISS”事件。flanneld收到该事件后，会查询etcd，获取该vtep设备对应的node的”Public IP“，并将信息注册到fdb中。

当内核获得了发往机器的ip地址后，arp得到mac地址，之后就能完成vxlan的外层封装。



4. flannel.1->cni0（对端）

Node节点的eth0网卡接收到vxlan设备包，kernal将识别出这是一个vxlan包，将包拆开之后转给节点上的flannel.1设备。这样数据包就从发送节点到达目的节点。

flannel.1查找自己的路由表，根据路由表完成转发。根据最下匹配原则，flannel.1将流量转发到cni0上去。

5. cni0->pod（对端）

cni0是一个网桥设备。当cni0拿到数据包之后，通过veth pair，将数据包发送给pod。

# APV整合遇到的问题及解决方法
​       K8S master interface information:

```shell
ens160: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.4.31  netmask 255.255.255.0  broadcast 192.168.4.255
        inet6 fe80::11d:156a:dd86:5e0b  prefixlen 64  scopeid 0x20<link>
        ether 00:50:56:9f:c3:42  txqueuelen 1000  (Ethernet)
        RX packets 2465242  bytes 1122687259 (1.0 GiB)
        RX errors 0  dropped 3072  overruns 0  frame 0
        TX packets 2252579  bytes 990149615 (944.2 MiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
flannel.1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1450
        inet 10.244.0.0  netmask 255.255.255.255  broadcast 0.0.0.0
        ether a6:53:49:c4:59:95  txqueuelen 0  (Ethernet)
        RX packets 9123  bytes 766500 (748.5 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 9417  bytes 799680 (780.9 KiB)
        TX errors 0  dropped 1 overruns 0  carrier 0  collisions 0
```

​       K8S node1 interface information:

```
ens160: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.4.32  netmask 255.255.255.0  broadcast 192.168.4.255
        inet6 fe80::92cb:f2d8:bfd4:7da2  prefixlen 64  scopeid 0x20<link>
        ether 00:50:56:9f:65:98  txqueuelen 1000  (Ethernet)
        RX packets 3614950  bytes 2566445027 (2.3 GiB)
        RX errors 0  dropped 3047  overruns 0  frame 0
        TX packets 2311959  bytes 218853232 (208.7 MiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
flannel.1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1450
        inet 10.244.1.0  netmask 255.255.255.255  broadcast 0.0.0.0
        ether 46:d5:68:bb:99:bc  txqueuelen 0  (Ethernet)
        RX packets 10596  bytes 838348 (818.6 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 15553  bytes 1306508 (1.2 MiB)
        TX errors 0  dropped 901 overruns 0  carrier 0  collisions 0
```

APV vxlan配置：

```shell
AN(config)#show vxlan all
vxlan enable
vxlan port 8472 
vxlan mode p2mp 
vxlan learn on 
vxlan tunnel "t1" 192.168.4.19 192.168.4.32
vxlan interface "vxlan1" 1
ip address "vxlan1" 10.244.3.99 255.255.0.0
vxlan bind "vxlan1" "t1"
AN(config)#show ip add
ip address "port1" 192.168.4.19 255.255.255.0
ip address "vxlan1" 10.244.3.99 255.255.0.0
AN(config)#show ip arp
(10.244.1.3) at 46:d5:68:bb:99:bc permanent [ethernet]
(10.244.3.99) at fa:c0:fe:48:2d:20 permanent [ethernet]
(192.0.0.1) at a2:f0:a8:00:0a:54 permanent [ethernet]
(192.168.4.1) at 00:25:90:38:cf:49 [ethernet]
(192.168.4.26) at 00:50:56:9f:59:7c [ethernet]
(192.168.4.19) at 00:50:56:9f:4e:cc permanent [ethernet]
(192.168.4.32) at 00:50:56:9f:65:98 [ethernet]
(192.168.4.11) at (incomplete) [ethernet]
AN(config)#
```



* 端口不一致+OTV
  vxlan默认端口是4789，OTV默认端口是8472,vxlan是OTV的子集。
  开始配置Vxlan端口用的默认的4789，数据包到了对端物理口就不会再往对端flannel.1发包。必须配置成8472，因为flannel发vxlan包默认目的端口就是8472

* remote vtep IP
  在apv上配置local ip为本地管理口IP，remote ip为K8S节点物理口IP：

  vxlan tunnel "t1" 192.168.4.19 192.168.4.32
  在K8S上配置APV节点，注意VtepMac为vxlan1口的Mac地址。因为Flannel利用ARP欺骗，vxlan内层包的Mac地址会给到APV 接口vxlan1，vxlan1对这个Mac地址进行判断是否是自己的Mac地址，如果不是就会丢掉。

```
[root@master k8s]# cat add-apv.yaml 
apiVersion: v1
kind: Node
metadata:
  name: apv
  annotations:
    flannel.alpha.coreos.com/backend-data: '{"VtepMAC":"fa:c0:fe:48:2d:20"}'
    flannel.alpha.coreos.com/backend-type: "vxlan"
    flannel.alpha.coreos.com/kube-subnet-manager: "true"
    flannel.alpha.coreos.com/public-ip: 192.168.4.19
spec:
  podCIDR: 10.244.3.0/24
[root@master k8s]# 
```

* 回包 +RTS
  在K8S pod中ping apv vxlan1接口IP时：vxlan收到包之后返回的时候会是一个不一样的子网：10.244.1.0/24，网络不知道怎么走会把包丢掉，F5利用FDB的方式干预了回包方式，APV这边没有相应的功能，考虑到可以利用从哪来往哪回，所以打开RTS功能即可完成回包。
* 指定vxlan口ping
  从APVping K8S中的pod IP，因为没有10.244.1.0/24子网的路由信息，所以需要强制指定接口vxlan1，然后实现按照Vxlan协议发包。
  最后想到可以扩大本地子网范围，变成10.244.0.0/16: ip address "vxlan1" 10.244.3.99 255.255.0.0 来解决这个问题。这种操作本来会影响不同节点和APV的地址重叠问题，但是APV使用的内网IP只用vxlan1这一个接口，而且APV也不会承接K8S调度任务。只要按照规划好的小网段10.244.3.0/24中网址即可。这种使用场景并不会带来什么影响。


## flannel 中UDP和Vxlan实现方式的区别
UDP方式性能更差，因为会涉及到从flannel0拷贝包到用户态flanneld，通过flanneld转发包。

# 参考文档

https://www.cnblogs.com/goldsunshine/p/10740928.html

https://blog.csdn.net/u012707739/article/details/78185960