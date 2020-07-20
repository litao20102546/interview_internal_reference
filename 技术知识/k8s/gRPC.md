 GRPC是google开源的一个高性能、跨语言的RPC框架，基于HTTP2协议，基于protobuf 3.x，基于Netty 4.x +。

# 原理解析

https://blog.csdn.net/iteye_19607/article/details/82651139

* GRPC的Client与Server，均通过Netty Channel作为数据通信，
* 序列化、反序列化则使用Protobuf，每个请求都将被封装成HTTP2的Stream
* 在整个生命周期中，客户端Channel应该保持长连接，而不是每次调用重新创建Channel、响应结束后关闭Channel（即短连接、交互式的RPC），目的就是达到链接的复用，进而提高交互效率。

 对于开发者而言：

    1）需要使用protobuf定义接口，即.proto文件
    
    2）然后使用compile工具生成特定语言的执行代码，比如JAVA、C/C++、Python等。类似于thrift，为了解决跨语言问题。
    
    3）启动一个Server端，server端通过侦听指定的port，来等待Client链接请求，通常使用Netty来构建，GRPC内置了Netty的支持。
    
    4）启动一个或者多个Client端，Client也是基于Netty，Client通过与Server建立TCP长链接，并发送请求；Request与Response均被封装成HTTP2的stream Frame，通过Netty Channel进行交互。
