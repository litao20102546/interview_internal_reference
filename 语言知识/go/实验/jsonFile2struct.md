```go
package main

import (
        "encoding/json"
        "fmt"
        "io/ioutil"
)

func main() {
    s := demoStruct{}
    demo("/tmp/demo.json", &s)
    fmt.Println(s)
}

type demoStruct struct {
                T int64  `json:"timestamp"`
                M string `json:"metricname"`
        }

func demo(filename string, JsonStruct interface{}) {
        str, err:= ioutil.ReadFile(filename)
        if err != nil {
                fmt.Printf("err1\n")
                return
        }
        err = json.Unmarshal(str, &JsonStruct)
        if err != nil {
                fmt.Println(JsonStruct)
        }
}
```

