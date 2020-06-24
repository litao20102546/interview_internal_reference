# OVS 架构

OVS 是产品级的虚拟交换机，大量应用在生产环境中，支撑整个数据中心虚拟网络的运转。OVS 基于 SDN 的思想，将整个核心架构分为控制面和数据面，数据面负责数据的交换工作，控制面实现交换策略，指导数据面工作。

![img](https://images2017.cnblogs.com/blog/431521/201712/431521-20171224102317818-1090106576.png)

从整体上看，OVS 可以划分为三大块，管理面、数据面和控制面。

数据面就是以用户态的 ovs-vswitchd 和内核态的 datapath 为主的转发模块，以及与之相关联的数据库模块 ovsdb-server，控制面主要是由 ovs-ofctl 模块负责，基于 OpenFlow 协议与数据面进行交互。而管理面则是由 OVS 提供的各种工具来负责，这些工具的提供也是为了方便用户对底层各个模块的控制管理，提高用户体验。下面就对这些工具进行一个逐一的阐述。

**`ovs-ofctl`**：这个是控制面的模块，但本质上它也是一个管理工具，主要是基于 OpenFlow 协议对 OpenFlow 交换机进行监控和管理，通过它可以显示一个 OpenFlow 交换机的当前状态，包括功能、配置和表中的项。使用时，有很多参数，我们可以通过 ovs-ofctl --help 查看。

```
Copy常用命令：

ovs-ofctl show switch-name ：输出交换机信息，包括其流量表和端口信息。

ovs-ofctl dump-ports switch-name：输出交换机的端口统计信息，包括收发包、丢包、错误包等数量。

ovs-ofctl add-flow switch-name：为交换机配置流策略。
```

**`ovs-dpctl`**：用来配置交换机的内核模块 datapath，它可以创建，修改和删除 datapath，一般，单个机器上的 datapath 有 256 条（0-255）。一条 datapath 对应一个虚拟网络设备。该工具还可以统计每条 datapath 上的设备通过的流量，打印流的信息等，更过参数通过 ovs-dpctl --help 查看。

```
Copy常用命令：

ovs-dpctl show ：显示所有 datapath 的基本信息。

ovs-dpctl dump-dps ：显示所有 datapath 的名字。

ovs-dpctl dump-flows DP ：显示一条 datapath DP 上的流信息。
```

**`ovs-appctl`**：查询和控制运行中的 OVS 守护进程，包括 ovs-switchd，datapath，OpenFlow 控制器等，兼具 ovs-ofctl、ovs-dpctl 的功能，是一个非常强大的命令。ovs-vswitchd 等进程启动之后就以一个守护进程的形式运行，为了能够很好的让用户控制这些进程，就有了这个命令。详细可以 ovs-appctl --help 查看。

**`ovs-vsctl`**：查询和更新 ovs-vswitchd 的配置，这也是一个很强大的命令，网桥、端口、协议等相关的命令都由它来完成。此外，还负责和 ovsdb-server 相关的数据库操作。

```
Copy常用命令：

ovs-vsctl show ：显示主机上已有的网桥及端口信息。

ovs-vsctl add-br br0：添加网桥 br0。
```

**`ovsdb-client`**：访问 ovsdb-server 的客户端程序，通过 ovsdb-server 执行一些数据库操作。

```
Copy常用命令：

ovsdb-client dump：用来查看ovsdb内容。

ovsdb-client transact ：用来执行一条类 sql。
```

**`ovsdb-tool`**：和 ovsdb-client 要借助 ovsdb-server 才能进行相关数据库操作不同，ovsdb-tool 可以直接操作数据库。

# 数据转发流程

通过一个例子来看看 OVS 中数据包是如何进行转发的。

![img](https://images2017.cnblogs.com/blog/431521/201712/431521-20171224102412506-951322711.png)

1）ovs 的 datapath 接收到从 ovs 连接的某个网络端口发来的数据包，从数据包中提取源/目的 IP、源/目的 MAC、端口等信息。

2）ovs 在内核态查看流表结构（通过 hash），如果命中，则快速转发。

3）如果没有命中，内核态不知道如何处置这个数据包，所以，通过 **netlink upcall 机制**从内核态通知用户态，发送给 ovs-vswitchd 组件处理。

4）ovs-vswitchd 查询用户态精确流表和模糊流表，如果还不命中，在 SDN 控制器接入的情况下，经过 OpenFlow 协议，通告给控制器，由控制器处理。

5）如果模糊命中， ovs-vswitchd 会同时刷新用户态精确流表和内核态精确流表，如果精确命中，则只更新内核态流表。

6）刷新后，重新把该数据包注入给内核态 datapath 模块处理。

7）datapath 重新发起选路，查询内核流表，匹配；报文转发，结束。



## 常用命令

* ovs-vsctl list port
* ovs-vsctl set Port <port_name> tag=100
* ofctl add-flow ...设置流表

**关键的是看文档Openvswitch教程实例详解**