## XDS协议

* XDS协议是Envoy动态获取配置的传输协议
* XDS是一类服务，包括LDS，RDS，EDS，CDS（cluster），SDS
* 一次XDS流程三个步骤：
  * Envoy主动向pilot发起Discovery Request类型请求
  * pilot根据请求生成相应的Discovery Response类型的响应
  * Envoy接受Discovery Response，加载配置，成功后发ACK否则发NACK

## pilot工作流程

<img src=pilot工作流程.png />

## adapter工作原理

<img src=adapter工作原理.png />

## pilot主被动模式分发

<img src=pilot被动模式分发.png />

* 主动模式

  <img src=pilot主动模式分发.png />

## 设计的亮点

* 三级缓存

  <img src=pilot三级缓存.png style="zoom:33%;" />

* 去抖动分发

  * 最小静默时间， 在Tn之后的最小静默时间没有新事件则分发
  * 最大延迟时间，超过t0+最大延迟时间则分发

* 增量EDS

  * 不用传LDS,SDS,RDS...

    <img src=增量EDS异步分发.png style="zoom:33%;" />

    <img src=envoyXdsServer增量EDS.png style="zoom: 33%;" />

* 资源隔离

