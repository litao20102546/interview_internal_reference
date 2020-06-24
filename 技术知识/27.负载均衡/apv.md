method
	RR(round_robin)
	SR(shortest response)
	LC(letest connection)

每个core都有一个线程对应一个lc list,lc list 中有多个bucket list,每个bucket list存储了RS信息，并且这些bucket list是按照连接数的大小顺序在lc list中排序的. 