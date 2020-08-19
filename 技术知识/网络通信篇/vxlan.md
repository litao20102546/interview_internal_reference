当目的IP为接收端的VTEP的IP时，假如不知道这个IP地址，则需要执行ARP请求来获取，步骤如下：

1. 目标IP被替换成与源虚拟机具有相同VNI的多播组IP地址；
2. 所有VTEP端都接收该多播报文，VTEP查找所在主机上的全部虚拟机来匹配源虚拟机的Inner 目的MAC。
3. 目标VTEP的虚拟机会回应该多播包，从而获得目标VTEP的IP地址。
4. 发送端VTEP添加VNI-VTEP-虚拟机MAC的映射关系到自己的VXLAN表中，以避免再次组播学习。

<img src=vxlan.png />

<img src=arp-go.png />

<img src=arp-come.png />

