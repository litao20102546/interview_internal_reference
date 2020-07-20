Mixer 主要负责获取注册中心用户的配置信息，然后根据配置信息来处理Envoy请求。

Mixer主要功能是配置规则发现，访问策略执行（多个维度分配和释放），无侵入遥测



配置规则发现默认用MCP，也可以用kubernetes,文件系统，MCP可以使得Mixer无需感知底层平台的差异

MCP client通过gRPC协议与MCP server即Galley组件通信，然后基于此链接向服务断发送资源订阅请求，以获取配置规则

服务模型：

* template

  主要定义了传输给Adapter的数据格式

* Adapter

  主要实现所有template定义的接口，需要定义builder和handler两个对象，builder对象实现template定义的所有handlerBuilder接口，然后在builder方法中初始化handler对象，handler对象实现了主要实现所有template定义的所有handler接口，在请求到来时处理生成的instance。

Mixer server主要实现两个重要接口，check，report

* check，访问策略控制，配额管理
* report，遥测数据收集

四层数据模型

* template
* namespace
* destination
* instanceGroup

Mixer设计亮点

* template扩展
* adapter扩展

如何开发Mixer adapter

* adapter加载和初始化，主要调用Getinfo接口
* 根据用户配置信息进一步初始化Adapter
* 请求到来时，利用配置好的Adapter实例对象Hander处理请求，并将其同步到基础设施后端

<img src=mixer启动流程.png />