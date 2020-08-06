# 概况

快速部署 Serverless 容器

* 支持自动扩缩容和缩到 0 实例
* 基于 Istio 组件，提供路由和网络编程
* 支持部署快照

Knative Serving 中定义了以下 CRD 资源：

* Service: 自动管理工作负载整个生命周期。负责创建 Route、Configuration 以及 Revision 资源。通过 Service 可以指定路由流量使用最新的 Revision 还是固定的 Revision
* Route：负责映射网络端点到一个或多个 Revision。可以通过多种方式管理流量。包括灰度流量和重命名路由
* Configuration: 负责保持 Deployment 的期望状态，提供了代码和配置之间清晰的分离，并遵循应用开发的 12 要素。修改一次 Configuration 产生一个 Revision
* Revision：Revision 资源是对工作负载进行的每个修改的代码和配置的时间点快照。Revision 是不可变对象，可以长期保留。每个 revision 对应了一组 deployment 管理的 pod

```yaml
apiVersion: serving.knative.dev/v1
kind: Service
metadata:
	name: traffic-example
	namespace: default
spec:
	template:
		metadata:
			name: traffic-example-v2
		spec:
			containers:
				- image: registry.cn-hangzhou.aliyuncs.com/knative-sample/rest-api-go:v1
				env:
					- name: RESOURCE
						value: v2
				readinessProbe:
					httpGet:
						path: /
	traffic:
	- tag: v1
		revisionName: traffic-example-v1
		percent: 80
	- tag: v2
		revisionName: traffic-example-v2
		percent: 20
	- tag: latest
		latestRevision: true
		percent: 0
```

# 为什么要引入 Queue-Proxy

Serverless 的一个核心诉求就是把业务的复杂度下沉到基础平台，让业务代码快速的迭代并且按需使用资源。不过现在更多的还是聚焦在按需使用资源层面。

如果想要**按需使用**资源我们就需要收集相关的 Metrics，并根据这些 Metrics 信息来指导资源的伸缩。Knative 首先实现的就是 KPA 策略，这个策略是根据请求数来判断是否需要扩容的。所以 Knative 需要有一个机制收集业务请求数量。除了业务请求数还有如下信息也是需要统一处理：

* 访问日志的管理
* Tracing
* Pod 健康检查机制
* 需要实现 Pod 和 Activator 的交互，当 Pod 缩容到零的时候如何接收 Activator 转发过来的流量
* 其他诸如判断 Ingress 是否 Ready 的逻辑也是基于 Queue-Proxy 实现的

为了保持和业务的低耦合关系，还需要实现上述这些功能所以就引入了 Queue-Proxy 负责这些事情。这样可以在业务无感知的情况下把 Serverless 的功能实现

# KPA

## Autoscaler实现了两种操作模式的缩放算法

Autoscaler实现了两种操作模式的缩放算法：S

​	table/稳定模式和Panic/恐慌模式。

* 稳定模式

  在稳定模式下，Autoscaler 调整 Deployment 的大小，以实现每个 POD 所需的平均并发数。 POD 的并发数是根据60秒窗口内接收所有数据请求的平均数来计算得出。

* 恐慌模式

  Autoscaler 计算 60 秒窗口内的平均并发数，系统需要 1 分钟稳定在所需的并发级别。但是，Autoscaler 也会计算 6 秒的恐慌窗口，如果该窗口达到目标并发的2倍，则会进入恐慌模式。在恐慌模式下，Autoscaler 在更短、更敏感的紧急窗口上工作。一旦紧急情况持续 60 秒后，Autoscaler 将返回初始的 60 秒稳定窗口。

## 配置 KPA

```yaml
apiVersion: v1
kind: ConfigMap
metadata:
	name: config-autoscaler
	namespace: knative-serving
data:
  container-concurrency-target-percentage: "70"
  container-concurrency-target-default: "100"
  requests-per-second-target-default: "200"
  target-burst-capacity: "200"
  stable-window: "60s"
  panic-window-percentage: "10.0"
  panic-threshold-percentage: "200.0"
  max-scale-up-rate: "1000.0"
  max-scale-down-rate: "2.0"
  enable-scale-to-zero: "true"
  tick-interval: "2s"
  scale-to-zero-grace-period: "30s"
```

# kubelet检查pod

* Knative 最终生成的 Pod 是需要落实到 Kubernetes 集群的，Kubernetes 中 Pod 有两个健康检查的机制 ReadinessProber 和 LivenessProber。其中 LivenessProber 是判断 Pod 是否活着，如果检查失败 Kubelet 就会尝试重启 Container，ReadinessProber 是来判断业务是否 Ready，**只有业务 Ready 的情况下才会把 Pod 挂载到 Kubernetes Service 的 EndPoint 中，这样可以保证 Pod 故障时对业务无损**。
* 所以 Knative 最终是把 user-container 的 ReadinessProber 收敛到 Queue-Proxy 中，通过 Queue-Proxy 的结果来决定 Pod 的状态。
* 在启动 istio 的情况下，kubelet 发起的 tcp 检查可能会被 Envoy 拦截，所以给 user-container 配置 TCP 探测器判断 user-container 是否 ready也是不准的。这也是需要把 Readiness 收敛到 Queue-Proxy 的一个动机。

# Knative 收敛 user-container 健康检查能力的方法



* 置空 user-container 的 ReadinessProber
* 把 user-container 的 ReadinessProber 配置的 json String 配置到 Queue-Proxy 的 env 中
* Queue-Proxy 的 Readinessprober 命令里面解析 user-container 的 ReadinessProber 的 json String 然后实现健康检查逻辑。并且这个检查的机制和前面提到的 Activator 的健康检查机制合并到了一起。这样做也保证了 Activator 向 Pod 转发流量时 user-container 一定是 Ready 状态

# VirtualService 的健康检查

Knative Revision 部署完成以后就会自动创建一个 Ingress(以前叫做 ClusterIngress), 这个 Ingress 最终会被 Ingress Controller 解析成 Istio 的 VirtualService 配置，然后 Istio Gateway 才能把相应的流量转发给相关的 Revision。

所以每添加一个新的 Revision 都需要同步创建 Ingress 和 Istio 的 VirtualService ，而 VirtualService 是没有状态表示 Istio 的管理的 Envoy 是否配置生效的能力的。所以 Ingress Controller 需要发起一个 http 请求来监测 VirtualService 是否 ready。这个 http 的检查最终也会打到 Pod 的 8012 端口上。标识 Header 是 K-Network-Probe=probe 。Queue-Proxy 需要基于此来判断，然后执行相应的逻辑。

相关代码如下所示：