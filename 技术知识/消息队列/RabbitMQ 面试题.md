https://blog.csdn.net/jerryDzan/article/details/89183625
1. 使用RabbitMQ有什么好处？
   * 解耦，系统A在代码中直接调用系统B和系统C的代码，如果将来D系统接入，系统A还需要修改代码，过于麻烦！
   * 异步，将消息写入消息队列，非必要的业务逻辑以异步的方式运行，加快响应速度
   * 削峰，并发量大的时候，所有的请求直接怼到数据库，造成数据库连接异常
   * 日志收集

2. RabbitMQ 中的 broker 是指什么？cluster 又是指什么？
   * broker 是指一个或多个 erlang node 的逻辑分组，且 node 上运行着 RabbitMQ 应用程序。cluster 是在 broker 的基础之上，增加了 node 之间共享元数据的约束。

3. RabbitMQ 概念里的 channel、exchange 和 queue 是逻辑概念，还是对应着进程实体？分别起什么作用？
   * queue 具有自己的 erlang 进程；
   * exchange 内部实现为保存 binding 关系的查找表；
   * channel 是实际进行路由工作的实体，即负责按照 routing_key 将 message 投递给 queue 。由 AMQP 协议描述可知，channel是真实TCP连接之上的虚拟连接，所有AMQP命令都是通过 channel发送的，且每一个 channel 有唯一的 ID。一个channel 只能被单独一个操作系统线程使用，故投递到特定 channel 上的 message 是有顺序的。但一个操作系统线程上允许使用多个 channel 。
4. vhost 是什么？起什么作用？
   * vhost 可以理解为虚拟 broker ，即 mini-RabbitMQ  server。
   * 其内部均含有独立的queue,exchange和binding等，但最最重要的是，其拥有独立的权限系统，可以做到vhost范围的用户控制.当然，从RabbitMQ的全局角度,vhost可以作为不同权限隔离的手段（一个典型的例子就是不同的应用可以跑在不同的vhost中）。
5. 消息基于什么传输？
   * 由于TCP连接的创建和销毁开销较大，且并发数受系统资源限制，会造成性能瓶颈。RabbitMQ使用信道的方式来传输数据。信道是建立在真实的TCP连接内的虚拟连接，且每条TCP连接上的信道数量没有限制。
6. 消息如何分发？
   * 若该队列至少有一个消费者订阅，消息将以循环（round-robin）的方式发送给消费者。每条消息只会分发给一个订阅的消费者（前提是消费者能够正常处理消息并进行确认）。
7. 消息怎么路由？
   * 从概念上来说，消息路由必须有三部分：交换器、路由、绑定。生产者把消息发布到交换器上；绑定决定了消息如何从路由器路由到特定的队列；消息最终到达队列，并被消费者接收。
   * 消息发布到交换器时，消息将拥有一个路由键（routing key），在消息创建时设定。
   * 通过队列路由键，可以把队列绑定到交换器上。
   * 消息到达交换器后，RabbitMQ会将消息的路由键与队列的路由键进行匹配（针对不同的交换器有不同的路由规则）。如果能够匹配到队列，则消息会投递到相应队列中；如果不能匹配到任何队列，消息将进入 “黑洞”。
8. 常用方式
   * direct：如果路由键完全匹配，消息就被投递到相应的队列
   * fanout：如果交换器收到消息，将会广播到所有绑定的队列上
   * topic：可以使来自不同源头的消息能够到达同一个队列。 使用topic交换器时，可以使用通配符
9. 如何确保消息正确地发送至RabbitMQ？
   * RabbitMQ使用发送方确认模式，确保消息正确地发送到RabbitMQ。发送方确认模式：将信道设置成confirm模式（发送方确认模式），则所有在信道上发布的消息都会被指派一个唯一的ID。一旦消息被投递到目的队列后，或者消息被写入磁盘后（可持久化的消息），信道会发送一个确认给生产者（包含消息唯一ID）。
   * 如果RabbitMQ发生内部错误从而导致消息丢失，会发送一条nack（not acknowledged，未确认）消息。发送方确认模式是异步的，生产者应用程序在等待确认的同时，可以继续发送消息。当确认消息到达生产者应用程序，生产者应用程序的回调方法就会被触发来处理确认消息。
10. 如何确保消息接收方消费了消息？
   * 接收方消息确认机制：消费者接收每一条消息后都必须进行确认（消息接收和消息确认是两个不同操作）。只有消费者确认了消息，RabbitMQ才能安全地把消息从队列中删除。这里并没有用到超时机制，RabbitMQ仅通过Consumer的连接中断来确认是否需要重新发送消息。也就是说，只要连接不中断，RabbitMQ给了Consumer足够长的时间来处理消息。
11. 如何避免消息重复投递或重复消费？
在消息生产时，MQ内部针对每条生产者发送的消息生成一个inner-msg-id，作为去重和幂等的依据（消息投递失败并重传），避免重复的消息进入队列；在消息消费时，要求消息体中必须要有一个bizId（对于同一业务全局唯一，如支付ID、订单ID、帖子ID等）作为去重和幂等的依据，避免同一条消息被重复消费。
12. 如何解决丢数据的问题?
    1. 生产者丢数据
       * transaction机制. 吞吐量下降了
          * 发送消息前，开启事物(channel.txSelect())，然后发送消息，如果发送过程中出现什么异常, 事物就会回滚(channel.txRollback()), 如果发送成功则提交事物(channel.txCommit())。
       * confirm模式
          * 一旦channel进入confirm模式，所有在该信道上面发布的消息都将会被指派一个唯一的ID(从1开始)，一旦消息被投递到所有匹配的队列之后，rabbitMQ就会发送一个Ack给生产者(包含消息的唯一ID)，这就使得生产者知道消息已经正确到达目的队列了.如果rabiitMQ没能处理该消息，则会发送一个Nack消息给你，你可以进行重试操作。
    2. 消息队列丢数据
       * 处理消息队列丢数据的情况，一般是开启持久化磁盘的配置。这个持久化配置可以和confirm机制配合使用，你可以在消息持久化磁盘后，再给生产者发送一个Ack信号。这样，如果消息持久化磁盘之前，rabbitMQ阵亡了，那么生产者收不到Ack信号，生产者会自动重发。
	   * 那么如何持久化呢，这里顺便说一下吧，其实也很容易，就下面两步
          ①、将queue的持久化标识durable设置为true,则代表是一个持久的队列
          ②、发送消息的时候将deliveryMode=2
       * 这样设置以后，rabbitMQ就算挂了，重启后也能恢复数据。在消息还没有持久化到硬盘时，可能服务已经死掉，这种情况可以通过引入mirrored-queue即镜像队列，但也不能保证消息百分百不丢失（整个集群都挂掉）
    3. 消费者丢数据
       * 启用手动确认模式可以解决这个问题
       * ①自动确认模式，消费者挂掉，待ack的消息回归到队列中。消费者抛出异常，消息会不断的被重发，直到处理成功。不会丢失消息，即便服务挂掉，没有处理完成的消息会重回队列，但是异常会让消息不断重试。
       * ②手动确认模式，如果消费者来不及处理就死掉时，没有响应ack时会重复发送一条信息给其他消费者；如果监听程序处理异常了，且未对异常进行捕获，会一直重复接收消息，然后一直抛异常；如果对异常进行了捕获，但是没有在finally里ack，也会一直重复发送消息(重试机制)。
       * ③不确认模式，acknowledge="none" 不使用确认机制，只要消息发送完成会立即在队列移除，无论客户端异常还是断开，只要发送完就移除，不会重发。
13. 死信队列和延迟队列的使用
    1. 死信消息：
       * 消息被拒绝（Basic.Reject或Basic.Nack）并且设置 requeue 参数的值为 false
       * 消息过期了
       * 队列达到最大的长度
    2. 过期消息：
       * 队列设置：x-message-ttl
       * 单个消息设置 expiration
    3. 延时队列
       * 在rabbitmq中不存在延时队列，但是我们可以通过设置消息的过期时间和死信队列来模拟出延时队列。
       * 订单超过支付时间自动取消

producer：
	获取连接
	创建channel
	定义producer 
	发送消息	
consumer：
	获取连接
	创建channel
	定义消费者
	监听队列
---
```
ConnectionFactory factory = new ConnectionFactory(); 
factory.setHost("127.0.0.1"); 
factory.setPort(AMQP.PROTOCOL.PORT); 
factory.setUsername("guest"); 
factory.setPassword("guest"); 
Connection connection = factory.newConnection(); 
Channel channel = connection.createChannel();
// 声明一个接收被删除的消息的交换机和队列 
String EXCHANGE_DEAD_NAME = "exchange.dead"; 
String QUEUE_DEAD_NAME = "queue_dead"; 
channel.exchangeDeclare(EXCHANGE_DEAD_NAME, BuiltinExchangeType.DIRECT); 
channel.queueDeclare(QUEUE_DEAD_NAME, false, false, false, null); 
channel.queueBind(QUEUE_DEAD_NAME, EXCHANGE_DEAD_NAME, "routingkey.dead"); 
String EXCHANGE_NAME = "exchange.fanout"; 
String QUEUE_NAME = "queue_name"; 
channel.exchangeDeclare(EXCHANGE_NAME, BuiltinExchangeType.FANOUT); 
Map<String, Object> arguments = new HashMap<String, Object>(); 
// 统一设置队列中的所有消息的过期时间 
arguments.put("x-message-ttl", 30000); 
// 设置超过多少毫秒没有消费者来访问队列，就删除队列的时间 
arguments.put("x-expires", 20000); 
// 设置队列的最新的N条消息，如果超过N条，前面的消息将从队列中移除掉 
arguments.put("x-max-length", 4); 
// 设置队列的内容的最大空间，超过该阈值就删除之前的消息
arguments.put("x-max-length-bytes", 1024); 
// 将删除的消息推送到指定的交换机，一般x-dead-letter-exchange和x-dead-letter-routing-key需要同时设置
arguments.put("x-dead-letter-exchange", "exchange.dead"); 
// 将删除的消息推送到指定的交换机对应的路由键 
arguments.put("x-dead-letter-routing-key", "routingkey.dead"); 
// 设置消息的优先级，优先级大的优先被消费 
arguments.put("x-max-priority", 10); 
channel.queueDeclare(QUEUE_NAME, false, false, false, arguments); 
channel.queueBind(QUEUE_NAME, EXCHANGE_NAME, ""); 
String message = "Hello RabbitMQ: "; 
for(int i = 1; i <= 5; i++) { 
	// expiration: 设置单条消息的过期时间 
	AMQP.BasicProperties.Builder properties = new AMQP.BasicProperties().builder()
			.priority(i).expiration( i * 1000 + ""); 
	channel.basicPublish(EXCHANGE_NAME, "", properties.build(), (message + i).getBytes("UTF-8")); 
} 
channel.close(); 
connection.close();
```