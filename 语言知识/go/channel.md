## channel 原理

https://blog.csdn.net/u010853261/article/details/85231944

https://segmentfault.com/a/1190000019172554

# Go channel的常见使用

go关键字可以用来开启一个goroutine(协程))进行任务处理，而多个任务之间如果需要通信，就需要用到channel了。

```
func testSimple(){
    intChan := make(chan int)

    go func() {
        intChan <- 1
    }()

    value := <- intChan
    fmt.Println("value : ", value)

}
```

上面这个简单的例子就是新开启的goroutine向intChan发送了一个1的值，那么在主线程的intChan就会收到这个值的信息。

**channel类型：无缓冲和缓冲类型**

channel有两种形式的，一种是无缓冲的，一个线程向这个channel发送了消息后，会阻塞当前的这个线程，知道其他线程去接收这个channel的消息。无缓冲的形式如下：

intChan := make(chan int)

带缓冲的channel，是可以指定缓冲的消息数量，当消息数量小于指定值时，不会出现阻塞，超过之后才会阻塞，需要等待其他线程去接收channel处理，带缓冲的形式如下：

//3为缓冲数量 intChan := make(chan int， 3)

**传输struct结构数据**

channel可以传输基本类型的数据如int, string，同时也可以传输struct数据

```

type Person struct {
    Name    string
    Age     uint8
    Address Addr
}

type Addr struct {
    city     string
    district string
}

/*
测试channel传输复杂的Struct数据
 */
func testTranslateStruct() {
    personChan := make(chan Person, 1)

    person := Person{"xiaoming", 10, Addr{"shenzhen", "longgang"}}
    personChan <- person

    person.Address = Addr{"guangzhou", "huadu"}
    fmt.Printf("src person : %+v \n", person)

    newPerson := <-personChan
    fmt.Printf("new person : %+v \n", newPerson)
}
```

这里可以看到可以通过channel传输自定义的Person对象，同时一端修改了数据，不影响另一端的数据，也就是说通过channel传递后的数据是独立的。

**关闭channel**

channel可以进行关闭，例如写的一段关闭了channel，那么读的一端读取时就可以检测读取失败

```
/*
测试关闭channel
 */
func testClose() {
    ch := make(chan int, 5)
    sign := make(chan int, 2)

    go func() {
        for i := 1; i <= 5; i++ {
            ch <- i
            time.Sleep(time.Second)
        }

        close(ch)

        fmt.Println("the channel is closed")

        sign <- 0

    }()

    go func() {
        for {
            i, ok := <-ch
            fmt.Printf("%d, %v \n", i, ok)

            if !ok {
                break
            }

            time.Sleep(time.Second * 2)
        }

        sign <- 1

    }()

    <-sign
    <-sign
}
```

**合并多个channel的输出**

可以将多个channel的数据合并到一个channel进行输出，形成一个消息队列

```
/**
将多个输入的channel进行合并成一个channel
 */
func testMergeInput() {
    input1 := make(chan int)
    input2 := make(chan int)
    output := make(chan int)

    go func(in1, in2 <-chan int, out chan<- int) {
        for {
            select {
            case v := <-in1:
                out <- v
            case v := <-in2:
                out <- v
            }
        }
    }(input1, input2, output)

    go func() {
        for i := 0; i < 10; i++ {
            input1 <- i
            time.Sleep(time.Millisecond * 100)
        }
    }()

    go func() {
        for i := 20; i < 30; i++ {
            input2 <- i
            time.Sleep(time.Millisecond * 100)
        }
    }()

    go func() {
        for {
            select {
            case value := <-output:
                fmt.Println("输出：", value)
            }
        }
    }()

    time.Sleep(time.Second * 5)
    fmt.Println("主线程退出")
}

```

**通过channel实现退出的通知**

定义一个用于退出的channel比如quit，不断执行任务的线程通过select监听quit的读取，当读取到quit中的消息时，退出当前的任务线程，这里是主线程通知任务线程退出。

```
/*
测试channel用于通知中断退出的问题
 */
func testQuit() {
    g := make(chan int)
    quit := make(chan bool)

    go func() {
        for {
            select {
            case v := <-g:
                fmt.Println(v)
            case <-quit:
                fmt.Println("B退出")
                return
            }
        }
    }()

    for i := 0; i < 3; i++ {
        g <- i
    }
    quit <- true
    fmt.Println("testAB退出")
}
```

**生产者消费者问题**

通过channel可以比较方便的实现生产者消费者模型，这里开启一个生产者线程，一个消费者线程，生产者线程往channel中发送消息，同时阻塞，消费者线程轮询获取channel中的消息，

进行处理，然后阻塞，这时生产者线程唤醒继续后面的逻辑，如此便形成了简单的生产者消费者模型。同时生产者在完成了所有的消息发送后，可以通过quit这个channel通知消费者线程退出，

而消费者线程退出时，通知主线程退出，整个程序完成退出。

```
/**
生产者消费者问题
 */
func testPCB() {
    fmt.Println("test PCB")

    intchan := make(chan int)
    quitChan := make(chan bool)
    quitChan2 := make(chan bool)

    value := 0

    go func() {
        for i := 0; i < 3; i++ {

            value = value + 1
            intchan <- value

            fmt.Println("write finish, value ", value)

            time.Sleep(time.Second)
        }
        quitChan <- true
    }()
    go func() {
        for {
            select {
            case v := <-intchan:
                fmt.Println("read finish, value ", v)
            case <-quitChan:
                quitChan2 <- true
                return
            }
        }

    }()

    <-quitChan2
    fmt.Println("task is done ")
}
```

**输出顺序问题**

```
/*
这个结果输出是1,2, 也可能是2,1， 也可能是2，顺序是不一定的
 */
func testSequnse() {
    ch := make(chan int)

    go func() {
        v := <-ch
        fmt.Println(v)
    }()
    ch <- 1
    fmt.Println("2")
}
```

上面这个输出结果是什么呢？运行一下会发现，可能是1,2，也可能是2,1， 也可能是2，顺序是不一定的，那么为什么会是这样的，我觉得因为这是两个不同的线程，

它们是独立运行的，当v := <-ch 执行之后，主线程和当前线程都是运行状态（非阻塞），先执行主线程还是新线程的输出就看cpu运行了，所以结果是不确定的。

**channel的超时处理**

通过time可以实现channel的超时处理，当一个channel读取超过一定时间没有消息到来时，就可以得到超时通知处理，防止一直阻塞当前线程

```
/*
检查channel读写超时，并做超时的处理
 */
func testTimeout() {
    g := make(chan int)
    quit := make(chan bool)

    go func() {
        for {
            select {
            case v := <-g:
                fmt.Println(v)
            case <-time.After(time.Second * time.Duration(3)):
                quit <- true
                fmt.Println("超时，通知主线程退出")
                return
            }
        }
    }()

    for i := 0; i < 3; i++ {
        g <- i
    }

    <-quit
    fmt.Println("收到退出通知，主线程退出")
}
```

**channel的输入输出类型指定**

channel可以在显示指定它是输入型还是输出型的，指定为输入型，则不能使用它输出消息，否则出错编译不通过，同理，输出型不能接受消息输入，

这样可以在编写代码时防止手误写错误输入输出类型而导致程序错误的问题。指定输入输出类型可以在方法参数时设定，那么它只在当前方法中会做输入输出限制，

可看下面实现。

```
/*
指定channel是输入还是输出型的，防止编写时写错误输入输出，指定了的话，可以在编译时期作错误的检查
 */
func testInAndOutChan() {
    ch := make(chan int)
    quit := make(chan bool)

    //输入型的chan是这种格式的：inChan chan<- int，如果换成输出型的，则编译时会报错
    go func(inChan chan<- int) {
        for i := 0; i < 10; i++ {
            inChan <- i
            time.Sleep(time.Millisecond * 500)
        }
        quit <- true
        quit <- true
    }(ch)

    go func(outChan <-chan int) {
        for {
            select {
            case v := <-outChan:
                fmt.Println("print out value : ", v)
            case <-quit:
                fmt.Println("收到退出通知，退出")
                return
            }
        }
    }(ch)

    <-quit
    fmt.Println("收到退出通知，主线程退出")
}
```

**channel实现并发数量控制**

通过设置一个带缓冲数量的的channel来实现最大并发数量，最大并发数量即为缓冲数量，任务开始时想limit这个channel发送消息，

任务执行完成后从这个limit读取消息，这样就可以保证当并发数量达到limit的缓冲数量时，limit <- true 这里会发生阻塞，停止

创建新的线程，知道某个线程执行完成任务后，从limit读取数据，这样就能保证最大并发数量控制在缓冲数量。

```
/*
测试通过channel来控制最大并发数，来处理事件
 */
func testMaxNumControl()  {
    maxNum := 3
    limit := make(chan bool, maxNum)
    quit := make(chan bool)

    for i:=0; i<100; i++{
        fmt.Println("start worker : ", i)

        limit <- true

        go func(i int) {
            fmt.Println("do worker start: ", i)
            time.Sleep(time.Millisecond * 20)
            fmt.Println("do worker finish: ", i)

            <- limit

            if i == 99{
                fmt.Println("完成任务")
                quit <- true
            }

        }(i)
    }

    <-quit
    fmt.Println("收到退出通知，主程序退出")
}
```

**监听中断信号的channel**

可以创建一个signal信号的channel，同时通过signal.Notify来监听os.Interrupt这个中断信号，因此执行到<- quit时就会阻塞在这里，

直到收到了os.Interrupt这个中断信号，比如按Ctrl+C中断程序的时候，主程序就会退出了。当然还可以监听其他信号，例如os.Kill等。

```
/*
监听中断信号的channel
*/
func testSignal() {
quit := make(chan os.Signal)
signal.Notify(quit, os.Interrupt)
go func() {
    time.Sleep(time.Second * 2)

    number := 0;
    for{
        number++
        println("number : ", number)
        time.Sleep(time.Second)
    }
}()

fmt.Println("按Ctrl+C可退出程序")
<- quit
fmt.Println("主程序退出")
}
```

**channel实现同步控制，生产者消费者模型**

开启多个线程做赚钱和花钱的操作，共享读写remainMoney这个剩余金额变量，实现生产者消费者模型

```
//同步控制模型，生产者模型
var lockChan = make(chan int, 1)
var remainMoney = 1000
func testSynchronize()  {
    quit := make(chan bool, 2)

    go func() {
        for i:=0; i<10; i++{
            money := (rand.Intn(12) + 1) * 100
            go testSynchronize_expense(money)

            time.Sleep(time.Millisecond * time.Duration(rand.Intn(500)))
        }

        quit <- true
    }()

    go func() {
        for i:=0; i<10; i++{
            money := (rand.Intn(12) + 1) * 100
            go testSynchronize_gain(money)

            time.Sleep(time.Millisecond * time.Duration(rand.Intn(500)))
        }

        quit <- true
    }()

    <- quit
    <- quit

    fmt.Println("主程序退出")
}

func testSynchronize_expense(money int)  {
    lockChan <- 0

    if(remainMoney >= money){
        srcRemainMoney := remainMoney
        remainMoney -= money
        fmt.Printf("原来有%d, 花了%d，剩余%d\n", srcRemainMoney, money, remainMoney)
    }else{
        fmt.Printf("想消费%d钱不够了, 只剩%d\n", money, remainMoney)
    }

    <- lockChan
}

func testSynchronize_gain(money int)  {
    lockChan <- 0

    srcRemainMoney := remainMoney
    remainMoney += money
    fmt.Printf("原来有%d, 赚了%d，剩余%d\n", srcRemainMoney, money, remainMoney)

    <- lockChan
}
```