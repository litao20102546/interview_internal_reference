`context`包实现的主要功能为：

其一，在程序单元之间共享状态变量。

其二，在被调用程序单元的外部，通过设置ctx变量值，将“过期或撤销这些信号”传递给“被调用的程序单元”。

## Context接口

```go
type Context interface {
    Deadline() (deadline time.Time, ok bool)
    Done() <-chan struct{}
    Err() error
    Value(key interface{}) interface{}
}
	Deadline方法    返回一个超时时间。到了该超时时间，该Context所代表的工作将被取消继续执行。Goroutine获得了超时时间后，可以对某些io操作设定超时时间。
	Done方法    返回一个通道（channel）。当Context被撤销或过期时，该通道被关闭。它是一个表示Context是否已关闭的信号。
	Err方法    当Done通道关闭后，Err方法返回值为Context被撤的原因。
	Value方法    可以让Goroutine共享一些数据，当然获得数据是协程安全的。但使用这些数据的时候要注意同步，比如返回了一个map，而这个map的读写则要加锁。

```

## Context的使用

Goroutine的创建和调用关系是分层级的。更靠顶部的Goroutine应有办法主动关闭其下属的Goroutine的执行（否则，程序就可能失控）。为了实现这种关系，Context结构像一棵树，叶子节点须总是由根节点衍生出来的。

* 根节点context.Background()。它不能被取消、没有值、也没有过期时间，常常作为处理Request的顶层context存在。通过WithCancel、WithTimeout函数来创建子对象，其可以获得cancel、timeout的能力。
* 子节点

```go
ctx := context.WithValue(context.Background(),"trace_id","123456")
func add(ctx context.Context,a,b int) int {
    traceId := ctx.Value("trace_id").(string)
    fmt.Printf("trace_id:%v\n",traceId)
    return a+b
}
ctx,cancel := context.WithTimeout(context.Background(),2*time.Second)
defer cancel()
ctx, cancel := context.WithCancel(context.Background())
defer cancel() // cancel when we are finished consuming integers
ctx, cancel := context.WithDeadline(context.Background(), d)
defer cancel()
```

## Context使用原则

* 使用Context的程序包需要遵循如下的原则来满足接口的一致性以及便于静态分析。在子Context被传递到的goroutine中，应该对该子Context的Done通道（channel）进行监控。一旦该通道被关闭（即上层运行环境撤销了本goroutine的执行），应主动终止对当前请求信息的处理，释放资源并返回。

* 不要把Context存在一个结构体当中，要显式地传入函数。Context变量需要作为第一个参数使用，一般命名为ctx

* 即使方法允许，也不要传入一个nil的Context。如果你不确定要用什么Context，那么传一个context.TODO；

* 使用context的Value方法时，只应该在程序和接口中传递“和请求相关的元数据”，不要用它来传递一些可选的参数；

  