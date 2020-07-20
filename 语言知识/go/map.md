https://cloud.tencent.com/developer/article/1468799

# hash冲突

* 开放定址法 

  也就是说当我们存储一个key，value时，发现hashkey(key)的下标已经被别key占用，那我们在这个数组中空间中重新找一个没被占用的存储这个冲突的key,那么没被占用的有很多，找哪个好呢？常见的有线性探测法，线性补偿探测法，随机探测法

  **线性探测**，字面意思就是按照顺序来，从冲突的下标处开始往后探测，到达数组末尾时，从数组开始处探测，直到找到一个空位置存储这个key，当数组都找不到的情况下回扩容（事实上当数组容量快满的时候就会扩容了）；

* 拉链法

  何为拉链，简单理解为链表，当key的hash冲突时，我们在冲突位置的元素上形成一个链表，通过指针互连接，当查找时，发现key冲突，顺着链表一直往下找，直到链表的尾节点，找不到则返回空，

# Go中Map的实现原理

在go中，map同样也是数组存储的的，每个数组下标处存储的是一个bucket,这个bucket的类型见下面代码，每个bucket中可以存储8个kv键值对，当每个bucket存储的kv对到达8个之后，会通过overflow指针指向一个新的bucket，从而形成一个链表

当往map中存储一个kv对时，通过k获取hash值，hash值的低八位和bucket数组长度取余，定位到在数组中的那个下标，hash值的高八位存储在bucket中的tophash中，用来快速判断key是否存在，key和value的具体值则通过指针运算存储，当一个bucket满时，通过overfolw指针链接到下一个bucket。

<img src=map-data.png />

巧妙设计：还有kv的存放，为什么不是k1v1，k2v2..... 而是k1k2...v1v2...，我们看上面的注释说的 map[int64]int8,key是int64（8个字节），value是int8（一个字节），kv的长度不同，如果按照kv格式存放，则考虑内存对齐v也会占用int64，而按照后者存储时，8个v刚好占用一个int64,从这个就可以看出go的map设计之巧妙。

# 协程安全

* map 不是协程安全的
* 用sync.Map 做协程安全的访问
* 或者用map + channel实现控制

```go
package main
 
import (
    "fmt"
    //"time"
)
 
var (
    ADD  interface{} = 1
    DEL  interface{} = 2
    GET  interface{} = 3
)
 
 
type safeMap struct {
    Msq     chan *[3] interface{}       //['type','id','value',channle]
    data    map[interface{}]interface{}
    chanl   chan interface{}
}
 
func NewSafeMap() *safeMap {
    tem := &safeMap{}
    tem.init()
    return tem
}
 
func (this *safeMap) init() {
    this.Msq   = make(chan *[3]interface{},10)
    this.data  = make(map[interface{}]interface{})
    this.chanl = make(chan interface{},0)
    go this.run()
}
 
func (this *safeMap) run() {
    for {
        select {
        case msg := <- this.Msq :
            switch msg[0] {
            case ADD :
                this.dataAdd(msg[1],msg[2])
            case DEL :
                this.dataDel(msg[1])
            case GET :
                this.dataGet(msg[1])
            }
        }
    }
}
 
func (this *safeMap) msqChan (typ,id,val interface{}) *[3]interface{}{
    return &[...]interface{}{typ,id,val}
}
 
//保存 或者更新元素
func (this *safeMap) dataAdd (id , value interface{}) {
    this.data[id] = value
}
 
//删除元素
func (this *safeMap) dataDel (id interface{}) {
    delete(this.data,id)
}
 
//获得元素
func (this *safeMap) dataGet (id interface{}) {
    if val ,exit := this.data[id] ;exit {
        this.chanl <- val
        return
    }
    this.chanl <- nil
}
 
//----------------------------------------------------对外接口--------------------------------
func (this *safeMap) Add (id ,value interface{}) {
    this.Msq <- this.msqChan(ADD,id,value)
}
 
func (this *safeMap) Del (id interface{}) {
    this.Msq <- this.msqChan(DEL,id ,nil)
}
 
func (this *safeMap) Get (id interface{}) interface{} {
    this.Msq <- this.msqChan(GET,id,nil)
    res := <- this.chanl
    return res
}
 
//获得 长度
func (this *safeMap) GetLength() uint32{
    return uint32(len(this.data))
}
 
 
func main() {
    sa := NewSafeMap()
 
//  sa.Add(1,1)
    sa.Add(2,3)
    fmt.Println(2,sa.Get(2))
    sa.Del(2)
    fmt.Println(2,sa.Get(2))
}
```

