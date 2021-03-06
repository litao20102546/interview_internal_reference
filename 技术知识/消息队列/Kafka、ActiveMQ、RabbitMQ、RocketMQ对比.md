1. Kafka、ActiveMQ、RabbitMQ、RocketMQ 有什么优缺点？
| 指标 | ActiveMQ | RabbitMQ | RocketMQ | Kafka | ZeroMQ|
|:------------ |:------------ |:------------ |:------------|:------------ |:------------ 
| 单机吞吐量 | 比RabbitMQ低 | 2.6w/s | （消息做持久化）| 11.6w/s | 17.3w/s | 29w/s
| 开发语言 | Java | Erlang | Java | Scala/Java | C
| 成熟度 | 成熟 | 成熟 | 开源版本不够成熟 | 比较成熟 | 只有C、PHP等版本成熟
| 持久化 | 支持少量堆积 | 支持少量堆积 | 支持大量堆积 | 支持大量堆积 | 不支持
| 性能稳定性 | 好 | 好 | 一般 | 较差 | 很好
| 集群方式 | 支持简单集群模式，比如’主-备’，对高级集群模式支持不好。 支持简单集群，'复制’模式，对高级集群模式支持不好。 | 常用 多对’Master-Slave’ 模式，开源版本需手动切换Slave变成Master | 天然的‘Leader-Slave’无状态集群，每台服务器既是Master也是Slave | 不支持

   * 所以中小型公司，技术实力较为一般，技术挑战不是特别高，用 RabbitMQ 是不错的选择；大型公司，基础架构研发实力较强，用 RocketMQ 是很好的选择。

   * 如果是大数据领域的实时计算、日志采集等场景，用 Kafka 是业内标准的，绝对没问题，社区活跃度很高，绝对不会黄，何况几乎是全世界这个领域的事实性规范。
---
2. 设计MQ思路
比如说这个消息队列系统，我们从以下几个角度来考虑一下：
* 伸缩性，
   就是需要的时候快速扩容，就可以增加吞吐量和容量，那怎么搞？设计个分布式的系统呗，参照一下 kafka 的设计理念，broker -> topic -> partition，每个 partition 放一个机器，就存一部分数据。如果现在资源不够了，简单啊，给 topic 增加 partition，然后做数据迁移，增加机器，不就可以存放更多数据，提供更高的吞吐量了？
* 持久化
	其次你得考虑一下这个 mq 的数据要不要落地磁盘吧？那肯定要了，落磁盘才能保证别进程挂了数据就丢了。那落磁盘的时候怎么落啊？顺序写，这样就没有磁盘随机读写的寻址开销，磁盘顺序读写的性能是很高的，这就是 kafka 的思路。
* 可用性
	其次你考虑一下你的 mq 的可用性啊？这个事儿，具体参考之前可用性那个环节讲解的 kafka 的高可用保障机制。多副本 -> leader & follower -> broker 挂了重新选举 leader 即可对外服务。
* 数据丢失
	能不能支持数据 0 丢失啊？可以的，参考我们之前说的那个 kafka 数据零丢失方案。
