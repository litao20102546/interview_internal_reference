# calico

<img src=bgp.png />

* Felix，即Calicoagent，跑在kubernetes的node节点上，主要负责配置路由及ACLs等信息来确保endpoint的连通状态；
* etcd，分布式键值存储，主要负责网络元数据一致性，确保Calico网络状态的准确性，可以与kubernetes共用；
* BGPClient(BIRD), 主要负责把 Felix写入 kernel的路由信息分发到当前 Calico网络，确保 workload间的通信的有效性；
* BGPRoute Reflector(BIRD), 大规模部署时使用，摒弃所有节点互联的mesh模式，通过一个或者多个 BGPRoute Reflector 来完成集中式的路由分发；

# profile

Profile 的实现基于 Iptables 和 IPSet。

**利用 Profile 实现 ACL**

```json
[node1]# ./calicoctl profile db rule json

{
  "id": "db",
  "inbound_rules": [
    {
      "action": "allow",
      "src_tag": "db"
    }
  ],
  "outbound_rules": [
    {
      "action": "allow"
    }
  ]
}
```

**Profile 高级特性：Tag**

　　有同学可能说，在现实环境中，会有多组不同的 APP 都需要访问 DB，如果每个 APP 都在 db 中增加一条规则也很麻烦同时还容易出错。

　　这里我们可以利用Profile 的高级特性 Tag 来简化操作：

　　每个 Profile 默认拥有一个和 Profile 名字相同的 Tag

　　每个 Profile 可以有多个 Tag，以 List 形式保存

　　利用 Tag 我们可以将一条规则适配到指定的一组 Profile 上。

　　参照上面的例子，我们给所有需要访问 DB 的 APP 的 Profile 都加上 db-users 这个 Tag：

1. [node1]# ./calicoctl profile app1 tag add db-users
2. [node1]# ./calicoctl profile app2 tag add db-users
3. [node1]# ./calicoctl profile app3 tag add db-users

# 租户隔离

k8s 提供了 networkpolicy 来定义网络策略，从而实现网络隔离以满足租户隔离及部分租户下业务隔离等。Network Policy 提供了基于策略的网络控制，用于隔离应用并减少攻击面。它使用标签选择器模拟传统的分段网络，并通过策略控制它们之间的流量以及来自外部的流量。但这个 networkpolicy 需要有第外方外接网络插件的支持，如Calico、Romana、Weave Net和trireme等。

```yaml
kind: NetworkPolicy
apiVersion: networking.k8s.io/v1
metadata:
  name: default-deny
  namespace: demo
spec:
  podSelector: {}

---

kind: NetworkPolicy
apiVersion: networking.k8s.io/v1
metadata:
  name: access-demo
  namespace: demo
spec:
  podSelector:
    matchLabels:
      environment: demo
  ingress:
    - from:
      - podSelector:
          matchLabels:
            environment: demo
```

