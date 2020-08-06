# Broker 和 Trigger

* 从 v0.5 开始，Knative Eventing 定义 Broker 和 Trigger 对象，从而能方便的对事件进行过滤（亦如通过 ingress 和 ingress controller 对网络流量的过滤一样）。
* Broker 提供一个事件集，可以通过属性选择该事件集。它负责接收事件并将其转发给由一个或多个匹配 Trigger 定义的订阅者。
* Trigger 描述基于事件属性的过滤器。同时可以根据需要创建多个 Trigger。





Broker/Trigger 模型出现的意义不仅在于其提供了消息过滤机制，更充分解耦了消息通道的实现，目前除了系统自身支持的基于内存的消息通道 InMemoryChannel 之外，还支持 Kafka、NATS Streaming 等消息服务。

此外结合 CloudEvent 进行事件统一标准传输，无论对于客户端接入事件源，还是消费端提供的消费事件服务，都能极大的提升了应用的跨平台可移植性。

## Broker (事件代理)

通过Broker Reconciler进行处理 broker，对于每一个 broker, 会进行一下处理操作：

1. 创建 'trigger'Channel。所有在 Broker 中的 event 事件都会发送到这个Channel， 所有的 Trigger 会订阅这个Channel。

2. 创建'filter'Deployment。这个 Deployment 会运行cmd/broker/filter。其目的是处理与此 Broker 相关的所有 Trigger 的数据平面。说白了其实就做了两件事情，从 Channel 中接收事件，然后转发给事件的订阅者。

3. 创建'filter' Kubernetes Service。通过该 Service 提供'filter' Deployment的服务访问。

4. 创建'ingress' Deployment。这个 Deployment 会运行 cmd/broker/ingress。其目的是检查进入 Broker的所有事件

5. 创建'ingress' Kubernetes Service。通过该 Service提供'Ingress' Deployment的服务访问。

6. 创建'ingress' Channel。这是一个 Trigger 应答的 Channel。目的是将 Trigger 中返回的事件通过 Ingress Deployment 回写到 Broker。理想情况下，其实不需要这个，可以直接将 Trigger 的响应发送给 k8s Service。但是作为订阅的场景，只允许我们向 Channel 发送响应信息，所以我们需要这个作为中介。
7. 创建'ingress' Subscription。它通过'ingress' Channel来订阅'ingress' Service

## Trigger (触发器)

通过Trigger Reconciler进行处理 trigger，对于每一个 trigger, 会进行一下处理操作：

1. 验证 Broker 是否存在

2. 获取对应 Broker 的 Trigger Channel、 Ingress Channel 以及 Filter Service

3. 确定订阅者的 URI

4. 创建一个从 Broker 特定的 Channel 到这个 Trigger 的 kubernetes Service 的订阅。reply 被发送到Broker 的 ingress Channel。

5. 检查是否包含 knative.dev/dependency 的注释。

# 事件注册

## 创建 EventType

```yaml
apiVersion: eventing.knative.dev/v1alpha1
kind: EventType
metadata:
	name: com.github.pullrequest
	namespace: default
spec:
	type: com.github.pull_request
	source: github.com
	schema: //github.com/schemas/pull_request
	description: "GitHub pull request"
	broker: default
```

## 获取 Registry 的事件注册

$ kubectl get eventtypes -n default

## 订阅事件

```yaml
apiVersion: eventing.knative.dev/v1alpha1
kind: Trigger
metadata:
	name: my-service-trigger
	namespace: default
spec:
	filter:
		sourceAndType:
		type: dev.knative.source.github.push
		source: github.com
	subscriber:
		ref:
			apiVersion: serving.knative.dev/v1alpha1
			kind: Service
			name: my-service
```

# Sequeue

Sequence 在 Knative Eventing 中提供的如下 4 种使用场景：

* 直接访问 Service

  ![image-20200720163539021](/Users/orangeli/Library/Application Support/typora-user-images/image-20200720163539021.png)

* 面向事件处理

  ![image-20200720163504175](/Users/orangeli/Library/Application Support/typora-user-images/image-20200720163504175.png)

* 级联 Sequence

  ![image-20200720163445917](/Users/orangeli/Library/Application Support/typora-user-images/image-20200720163445917.png)

* 面向 Broker/Trigger

  ![image-20200720163613544](/Users/orangeli/Library/Application Support/typora-user-images/image-20200720163613544.png)

## 创建 Sequence

```yaml
apiVersion: messaging.knative.dev/v1alpha1
kind: Sequence
metadata:
	name: sequence
spec:
	channelTemplate:
		apiVersion: messaging.knative.dev/v1alpha1
		kind: InMemoryChannel
	steps:
		- ref:
			apiVersion: serving.knative.dev/v1alpha1
			kind: Service
			name: first
		- ref:
			apiVersion: serving.knative.dev/v1alpha1
			kind: Service
			name: second
		- ref:
			apiVersion: serving.knative.dev/v1alpha1
			kind: Service
			name: third
	reply:
		kind: Service
		apiVersion: serving.knative.dev/v1alpha1
		name: event-display
```



# Parallel 

![image-20200720163809901](/Users/orangeli/Library/Application Support/typora-user-images/image-20200720163809901.png)

## 创建资源定义

```yaml
apiVersion: messaging.knative.dev/v1alpha1
kind: Parallel
metadata:
	name: me-odd-even-parallel
spec:
	channelTemplate:
	apiVersion: messaging.knative.dev/v1alpha1
	kind: InMemoryChannel
	cases:
		- filter:
			uri: "http://me-even-odd-switcher.default.svc.cluster.local/0"
			subscriber:
				ref:
					apiVersion: serving.knative.dev/v1alpha1
					kind: Service
					name: me-even-transformer
		- filter:
			uri: "http://me-even-odd-switcher.default.svc.cluster.local/1"
			subscriber:
				ref:
					apiVersion: serving.knative.dev/v1alpha1
					kind: Service
					name: me-odd-transformer
	reply:
		apiVersion: serving.knative.dev/v1alpha1
		kind: Service
		name: me-event-display
```

## 代码实现

关键代码实现如下：

1. 首先为 Parallel 创建一个全局的 Channel。然后为每一个case创建一个过滤 Channel

2. 在每个case中做了如下处理：

* 为全局的 Channel创建一个 Subscription，订阅条件为filter信息，并且把 reply 响应发送给当前 case中的过滤 Channel
* 为过滤 Channel 创建一个 Subscription，将订阅信息发送给每个case中的 Reply。如果当前case中没有设置Reply，则发送的全局Reply。





!!!!

事件源对应的slink可以是service, broker，sequeue，parallel