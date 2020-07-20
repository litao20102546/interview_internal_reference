kube-apiserver 是 kubernetes 中与 etcd 直接交互的一个组件，其控制着 kubernetes 中核心资源的变化。它主要提供了以下几个功能：

- 提供 [Kubernetes API](https://kubernetes.io/docs/concepts/overview/kubernetes-api/)，包括认证授权、数据校验以及集群状态变更等，供客户端及其他组件调用；
- 代理集群中的一些附加组件组件，如 Kubernetes UI、metrics-server、npd 等；
- 创建 kubernetes 服务，即提供 apiserver 的 Service，kubernetes Service；
- 资源在不同版本之间的转换；

# kube-apiserver 处理流程

假如POST 请求：

1. 当请求到达 kube-apiserver 时，kube-apiserver 首先会执行在 http filter chain 中注册的过滤器链，该过滤器对其执行一系列过滤操作，主要有认证、鉴权等检查操作。

2. 当 filter chain 处理完成后，请求会通过 route 进入到对应的 handler 中，handler 中的操作主要是与 etcd 的交互。

3.  handler 中的主要处理流程如下所示：

   ```
   v1beta1 ⇒ internal ⇒    |    ⇒       |    ⇒  v1  ⇒ json/yaml ⇒ etcd                     										admission    validation
   ```

   1. 解码：

   * 在解码时，首先从 HTTP path 中获取期待的 version，然后调用scheme.convert(), 以正确的 version 创建一个与之匹配的空对象，并使用 JSON 或 protobuf 解码器进行转换，在转换的第一步中，如果用户省略了某些字段，Decoder 会把其设置为默认值。

     > knowledge base: 一个 resource 可能会对应多个 version。kube-apiserver 必须要知道如何在每一对版本之间进行转换。 因此其使用了一个特殊的`internal version`，`internal version` 作为一个通用的 version 会包含所有 version 的字段。Decoder 会首先把 creater object 转换到 `internal version`，然后将其转换为 `storage version`，`storage version` 是在 etcd 中存储时的另一个 version。

   2. 准入控制

   * 在解码完成后，需要通过验证集群的全局约束来检查是否可以创建或更新对象。这包括安全机制，node调度（podnodeselector，nodetaint，limitrange, antiaffinity, 证书，deny...）

     > 在 `k8s.io/kubernetes/plugin/pkg/admission` 目录下可以看到 kube-apiserver 可以使用的所有全局约束插件，kube-apiserver 在启动时通过设置 `--enable-admission-plugins` 参数来开启需要使用的插件，通过 `ValidatingAdmissionWebhook` 或 `MutatingAdmissionWebhook` 添加的插件也都会在此处进行工作。

   3. 检查合法性

   * 主要检查 object 中字段的合法性。

   在 handler 中执行完以上操作后最后会执行与 etcd 相关的操作，POST 操作会将数据写入到 etcd 中

# kube-apiserver 中 apiserver service 的实现

* apiserver service 是通过 bootstrap controller 控制的，bootstrap controller 会保证 apiserver service 以及其 endpoint 处于正常状态。

* 在 kubernetes，可以从集群外部和内部两种方式访问 kubernetes API，在集群外直接访问 apiserver 提供的 API，在集群内即 pod 中可以通过访问 service 为 kubernetes 的 ClusterIP

kubernetes service其主要以下几个功能：

- 创建 kubernetes service；
- 创建 default、kube-system 和 kube-public 命名空间，如果启用了 `NodeLease` 特性还会创建 kube-node-lease 命名空间；
- 提供基于 Service ClusterIP 的修复及检查功能；
- 提供基于 Service NodePort 的修复及检查功能；