
当一个客户端进行写数据请求时，会指定ZooKeeper集群中的一个server节点，如果该节点为Follower，则该节点会把写请求转发给Leader，Leader通过内部的协议进行原子广播，直到一半以上的server节点都成功写入了数据，这次写请求便算是成功，然后Leader便会通知相应Follower节点写请求成功，该节点向client返回写入成功响应。

<img src="zookeeper-write.png" />