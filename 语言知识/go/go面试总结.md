golang 基础
1.切片实现原理，切片中的指针是存储在堆上还是栈上？切片的使用有哪些坑？
2.map的实现原理？详细说一下拉链法？map是协程安全的吗？
3.sync.Map的实现原理？
4.写一个 golang 的单例实现？
5.golang IO 是同步的吗？如果 CPU 只有一核会不会阻塞？
6.golang 的内存管理机制？详细描述 GC 过程？
7.defer 的使用方法？在 return 前执行还是 return 后？ 后
8.说一下 PMG 模型
9.context 的使用方法和遇到的坑？
10.golang 和 java 对比
11.channel 的使用方法，注意的事项，原理？
12.go module 如何解决循环引用的？required 什么意思？
13.golang 如何实现乐观锁，CAS 原理和实现
14.用过哪些设计模式

网络
1.http,https,tcp 原理和过程,五层协议
2.tcp 最后等待2ML是为什么,tcp最后一个收到服务器 FIN 后的状态时什么？
3.什么是 RPC,GRPC 和 RPC 有什么关系，GRPC 如何实现的，GRPC 连接是持续连接吗？keep-alive 的作用是什么？GRPC 连接发生最大长度是多少？
4.详述一下 PING 的执行流程？描述同一个局域网和不同局域网两个主机通讯过程？
5.GRPC 相对于 RESTFUL 有什么优势，为什么选择 GRPC,Grcp 为什么使用 http2.0 而不是 TCP
6.proto 和 JSON 有什么区别和联系
7.session 和 cookie 
8.JWT 原理，Oauth2.0 认证过程


数据库
1.mysql 四种隔离级别，MVCC 实现方法，bin_log,undo_log,redo_log 关系和区别
2.数据库三范式
3.索引的分类，聚簇索引，二级索引
4.myisam innodb 引擎对比
5.mysql 的底层实现是否了解，说一下了解的
6.explain 怎么使用
7.redis 的五种数据格式
8.redis zset底层实现
9.redis watch 使用和原理
10.如何使用 redis 实现分布式锁
11.redis AOF 和 RDB
12.redis 的使用场景

中间件
1.说一下你熟悉的 MQ,比如 kafka 等
2.consul,etcd 原理,raft 选举机制
3.gorm 是如何实现的
4.gin 的 handler 底层数据结构

docker,k8s
1.docker 实现原理
2.docker 常用命令
3.docker file 文件命令
4.k8s master,slave 各组件
5.k8s 不同 node 上的 pod 是如何通信的，说一下流程
6.k8s controller manager 的种类
7.PV,PVC,storage-class
8.etcd 的作用
9.kubectl create -f file 执行后发生了什么

算法：首先要熟悉常用排序，要做到灵活变通。链表反转，双向链表。二分法，二叉树遍历，窗口滑动法，动态规划。