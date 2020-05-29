1. select 基本用法
	* 如果多个通道都阻塞了，会等待直到其中一个通道可以处理。
	* 如果多个通道都可以处理，随机选取一个处理。
	* 如果没有通道操作可以操作并且写了default语句，会执行：default（永远是可以运行的）
	* 如果防止select堵塞，可以写default来确保发送不被堵塞，没有case的select就会一直堵塞。
	* 当select做选择case和default操作时，case的优先级大于default。
	* select语句实现了一种监听模式，通常在无限循环中使用，通过在某种情况下，通过break退出select循环, return 跳出无限循环。