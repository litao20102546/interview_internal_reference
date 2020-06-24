## [golang 配置goproxy 几个可选的地址](https://www.cnblogs.com/rongfengliang/p/11419210.html)

对于golang 语言的开发，对于国内来说有点被动，需要想各种方法，一般的解决方法如下：

- 使用代理工具（翻墙）
- 配置goproxy

## 目前发现的几个不错的goproxy

- 阿里云 
  配置如下：

 

```
export GOPROXY=https://mirrors.aliyun.com/goproxy/
```

- nexus社区提供的 
  配置如下：

```
export GOPROXY=https://gonexus.dev
```

- goproxy.io 的 
  配置如下：

```
export GOPROXY=https://goproxy.io/
```

- 基于athens的公共服务 
  配置如下：

```
export GOPROXY=https://athens.azurefd.net
```

- 官方提供的(jfrog,golang)

```
export GOPROXY=https://gocenter.io
export GOPROXY=https://proxy.golang.org
```

- 七牛云赞助支持的

```
export GOPROXY=https://goproxy.cn
```