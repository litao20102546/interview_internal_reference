# 多进程
​	multiprocessing
# 多线程
​	多线程共享全局变量
​	一个线程挂掉则会影响到所有线程，所以不够稳定。
​	import threading
​	t = threading.Thread(target="", args=())
​	threading.enumberate()
​	t.start() 当调用threading.Thread不会创建线程，当调用.start才会创建线程
​	class MyThread(threading.Thread):
​	    def run(self):

# 协程

gevent() gevent.spawn 
死锁
互斥同步锁（悲观锁）
	哪些劣势：
		1. 阻塞，唤醒，性能低
		2. 永久阻塞
		3. 优先级，阻塞的优先级越高，持有锁的优先级越低，导致优先级反转问题
非互斥同步锁（乐观锁）