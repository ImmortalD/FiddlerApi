# FiddlerAPI

## Fiddler是什么

- `FiddlerAPI`是基于`FiddlerCore4`的封装,FiddlerAPI包含`C++`版本和`Java`的JNI版本,这是的很多编程语言能直接调用

-  [Java版本说明](../FiddlerJNI/README_JNI.md)

## FiddlerAPI说明

### FiddlerAPI使用的数据结构

- FiddlerData成员说明

```C++

typedef struct FiddlerData
{
    // id,每一次http请求的唯一标识,在消息回调中不要修改
    int id;
    // 回调消息类型 0表示数据发送阶段,1数据接收阶段,2表示数据处理完成阶段.这个阶段的数据不能在修改了.取值对应与EVENT的枚举值
    int event;
    // Http请求的进程ID,在回调中不要修改
    int processId;
    // 是否要修改Http响应body,如果需要修改需要在回调函数event=SEND(0)的时候把bModifyResponseBody修改为true
    bool bModifyResponseBody;
    // 修改的数据标志取值是`ModifyFlag`的枚举值,需要修改多项的值其取值可以是`MODIFY_BODY|MODIFY_HEADERS`这样的
    ModifyFlag modifyFlags;
    // 通知类型
    NotifyType notifyType;
    // http请求的url
    char*   url;
    // http请求方式,GET POST CONNECT等
    char*   httpMethod;
    // http协议版本,如 HTTP/1.1
    char*   httpVersion;
    // http头信息,enevt为SEND时表示请求头,否则是响应头 
    char*   header;
    // http 正文数据
    char*   body;
}

```
- ModifyFlag 修改类型标记

```C++
enum ModifyFlag
{
    // 不修改数据
    MODIFY_NO_ALL = 0,
    // 修改Url
    MODIFY_URL = 1,
    // 修改http body
    MODIFY_BODY = 2,
    // 修改http 头
    MODIFY_HEADERS = 4,
    // 修改通知类型
    MODIFY_NOTIFY = 8,
    // 修改url,http header以及http body
    MODIFY_ALL = MODIFY_URL | MODIFY_BODY | MODIFY_HEADERS | MODIFY_NOTIFY
};
```

- EventType event 的类型

```C++
enum EventType
{
    // 数据发送阶段,可修改请求数据
    SEND = 0, 
    // 数据接收阶段,可修改响应数据
    RECV,     
    // 数据接收阶段,数据不可修改
    COMPLETE  
};
```

- LogLevel 日志级别

```C++
enum LogLevel
{
    // 不打印日志,这个只是在设置日志级别是使用,在打印日志中不要使用
    CLOSE = 0,       
    //  只打印ERR级别日志 
    ERR = 1,         
    // 打印WARN和ERR级别日志
    WARN = 2 | ERR,  
    // 打印INFO WARN ERR 级别日志
    INFO = 4 | WARN, 
    // 打印DEBUIG NFO WARN ERR 级别日志
    DEBUG = 8 | INFO 
};

```


- FiddlerConfig Fiddler配置信息

```C++
typedef struct FiddlerConfig
{   
    // Fiddler代理端口
    int iListenPort;
    // 是否开启系统代理
    bool bRegisterAsSystemProxy;
    // 默认是否都修改ResponseBody
    bool bAlwaysModifyResponseBody;
    // 是否解密https的流量,这个需要信任Fiddler的根证书
    bool bDecryptSSL;
    // 是否允许远程连接
    bool bAllowRemote;
    // https流量解密使用
    int iSecureEndpointPort;
    // 日志级别
    LogLevel logLevel;
    // 通知类型
    NotifyType notifyType;
    // 回调
    lpCallBack callBack;
    // szSecureEndpointHostname
    char *szSecureEndpointHostname;
    // 日志文件全路径
    char* szLogFileName;
    // 进程PID过滤正则表达式,和进程名过滤正则表达式只要有一个匹配上就表示匹配上
    char* szRegexProcessId;
    // 进程名过滤正则表达式
    char* szRegexProcessName;
} 
```

### 函数说明

- 消息回调处理函数

```C++
// 消息回调处理函数
typedef bool(*lpCallBack)(FiddlerData& data);
```

-  开启Fiddler

```C++
__DECLSPEC bool __FIDDLER_CALL Start();
```


-  关闭Fiddler
```C++
__DECLSPEC bool __FIDDLER_CALL Close();
```

- 获取配置
```C++
__DECLSPEC  void __FIDDLER_CALL GetFiddlerConfig(__inout FiddlerConfig* const data);
```

- 设置Fiddler配置, 注意,这个要在 Start()之前调用才生效
```C++
__DECLSPEC bool __FIDDLER_CALL SetFiddlerConfig(__in const FiddlerConfig* const);
```


- 添加回调处理
```C++
DECLSPEC bool __FIDDLER_CALL SetCallBack(__in const lpCallBack callBack);
```

- 修改Fiddler数据
```C++
__DECLSPEC bool __FIDDLER_CALL ModifyFiddlerData(__in const FiddlerData* const newData);
```

- 导出Fiddler根证书
```C++
__DECLSPEC bool __FIDDLER_CALL ExportRootCert(__in const char* const fileName);
```

- 创建和信任Fiddler根证书,https需要
```C++
__DECLSPEC bool __FIDDLER_CALL CreateAndTrustRootCert();
```

- 删除fiddler生成的证书
```C++
__DECLSPEC bool __FIDDLER_CALL RemoveCert(__in const bool bRemoveRoot);
```

- 释放FidderConfig
```C++
__DECLSPEC void __FIDDLER_CALL FreeFiddlerConfig(__in FiddlerConfig* data);
```

- 释放FreeFiddlerData
```C++
__DECLSPEC void __FIDDLER_CALL  FreeFiddlerData(__in FiddlerData* data);
```

- 写日志,注意,这个要在 Start()之后调用才生效
```C++
__DECLSPEC void __FIDDLER_CALL WriteLog(LogLevel level, const char* const str);
```

- 写日志,注意,这个要在 Start()之后调用才生效
```C++
__DECLSPEC void __FIDDLER_CALL WriteLogEx(LogLevel level, const char* const format, ...);
```
