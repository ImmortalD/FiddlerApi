#ifndef __FILLDER_API__H__
#define __FILLDER_API__H__

struct FiddlerData;

// 消息回调处理函数
typedef bool(*lpCallBack)(FiddlerData& data);

/*
typedef struct HttpHeader
{
	char* name;
	char* value;
}*pHttpHeader;
*/

// 通知类型

	// 不通知任何类型
#define	NOTIFY_NO_ALL  0
	// 发送之前通知
#define NOTIFY_BEFORE_SEND  1
	// 响应之前通知
#define NOTIFY_BEFORE_RESPONSE  2
	// 响应完成通知
#define	NOTIFY_COMPOLE_RESPONSE  4
	// 通知所有类型
#define	NOTIFY_ALL  NOTIFY_BEFORE_SEND | NOTIFY_BEFORE_RESPONSE | NOTIFY_COMPOLE_RESPONSE


// FiddlerData modifyFlags values
	// 不修改数据
#define	MODIFY_NO_ALL  0
	// 修改Url
#define MODIFY_URL  1
	// 修改http body
#define MODIFY_BODY  2
	// 修改http 头
#define MODIFY_HEADERS  4
	// 修改http method
#define MODIFY_HTTPMETHOD  8
	// 修改httpversion
#define MODIFY_HTTPVERSION  16
	// 修改通知
#define MODIFY_NOTIFY  32
	// 修改响应状态
#define MODIFY_REPONSE_STATUS  64
	// 修改响应code
#define MODIFY_REPONSE_CODE  128
	// 修改所有
#define MODIFY_ALL  MODIFY_URL | MODIFY_BODY | MODIFY_HEADERS | MODIFY_HTTPMETHOD | MODIFY_HTTPVERSION \
	| MODIFY_NOTIFY | MODIFY_REPONSE_STATUS | MODIFY_REPONSE_CODE


	// 只打印ERR级别日志 
#define	ERR 1
	// 打印WARN和ERR级别日志
#define WARN 3
	// 打印INFO WARN ERR 级别日志
#define INFO 7
	// 打印DEBUIG NFO WARN ERR 级别日志
#define DEBUG 15


// FiddlerData event values
enum EventType
{
	// 数据发送阶段,可修改请求数据
	SEND = 0,
	// 数据接收阶段,可修改响应数据
	RECV,
	// 数据接收阶段,数据不可修改
	COMPLETE
};

typedef struct Version
{
	int nMajor;
	int nMinor;
	int nRevision;
	int nBuild;
}*pVersion;

typedef struct FiddlerData
{
	// 每一次http请求的唯一标识,在消息回调中不要修改
	int nId;
	// 回调消息类型 0表示数据发送阶段,1数据接收阶段,2表示数据处理完成阶段.这个阶段的数据不能在修改了.取值对应与EVENT的枚举值
	EventType eventType;
	// 如果是websockt消息则有值,否则是-1
	int nWebSocketId;
	// Http请求的进程ID,在回调中不要修改
	int nProcessId;
	// 是否要修改Http响应body,如果需要修改需要在回调函数event=SEND(0)的时候把bModifyResponseBody修改为true
	bool bModifyResponseBody;
	// 响应code
	int nResponseCode;
	// 修改的数据标志取值是Modify_XX的宏,需要修改多项的值其取值可以是`MODIFY_BODY|MODIFY_HEADERS`这样的
	int nModifyFlags;
	// 通知类型
	int nNotifyType;
	// 进程名
	char * lpszProcessName;
	// http请求的url
	char* lpszUrl;
	// http请求方式,GET POST CONNECT等
	char* lpszHttpMethod;
	// http协议版本,如 HTTP/1.1
	char* lpszHttpVersion;
	// http响应状态
	char* lpszResponseStatus;
	// http 正文数据
	char* lpszBody;
	// http header
	char* lpszHeader;

} *pFiddlerData;

#define LANGUAGE_C 1
#define LANGUAGE_C_PULS_PLUS 2
#define LANGUAGE_JAVA 3
#define LANGUAGE_E 4
#define LANGUAGE_OTHER 5


#define FC_ALLOW_REMOTE_CLIENTS 1
#define FC_CAPTURE_FTP 2
#define FC_CAPTURE_LOCALHOST_TRAFFIC 4
#define FC_CHAINTO_UPSTREAM_GATEWAY 8
#define FC_DECRYPT_SSL 16
#define FC_HOOKUSING_PACFILE 32
#define FC_MONITORALL_CONNECTIONS 64
#define FC_OPTIMIZE_THREADPOOL 128
#define FC_REGISTER_AS_SYSTEM_PROXY 256
#define FC_ALWAYS_MODIFY_RESPONSE_BODY 512
#define FC_DEFAULT FC_ALLOW_REMOTE_CLIENTS | FC_CAPTURE_LOCALHOST_TRAFFIC \
 | FC_DECRYPT_SSL | FC_MONITORALL_CONNECTIONS | FC_OPTIMIZE_THREADPOOL \
 | FC_REGISTER_AS_SYSTEM_PROXY

typedef struct FiddlerCallBack
{
	lpCallBack lpCallBackBeforeRequest;
	lpCallBack lpCallBackBeforeResponse;
	lpCallBack lpCallBackAfterSessionComplete;
	lpCallBack lpCallBackOnWebSocketMessage;
} *lpFiddlerCallBack;

typedef struct FiddlerConfig
{
	// 使用的编程语言
	int nLanguage;
	// Fiddler代理端口
	int nListenPort;
	// 其他配置,取值是 FC_开头的常量
	int nConfigEx;
	// https流量解密使用
	int nSecureEndpointPort;
	// 日志级别
	int nLogLevel;
	// 通知类型
	int nNotifyType;
	// 回调
	lpFiddlerCallBack lpCallBack;
	// szSecureEndpointHostname
	char *lpszSecureEndpointHostname;
	// 日志文件路径
	char* lpszLogFileName;
	// 进程PID过滤正则表达式,和进程名过滤正则表达式只要有一个匹配上就表示匹配上
	char* lpszRegexProcessId;
	// 进程名过滤正则表达式
	char* lpszRegexProcessName;
	// UpstreamGatewayTo
	char* lpszUpstreamGatewayTo;

} *pFiddlerConfig;



#ifdef __FIDDLER_API_SOURCE__
#define __DECLSPEC  __declspec( dllexport )
#else
#define __DECLSPEC  //__declspec( dllimport )
#define __inout
#define __in
#define __out
#endif // __FIDDLERAPI_SOURCE__

#define __FIDDLER_CALL __stdcall

#ifdef __cplusplus 
extern "C"
{
#endif // _cplusplus

	// 开启Fiddler
	__DECLSPEC bool __FIDDLER_CALL Start();
	// 关闭Fiddler
	__DECLSPEC bool __FIDDLER_CALL Close();
	// 获取配置
	__DECLSPEC bool __FIDDLER_CALL GetFiddlerConfig(__inout FiddlerConfig* const);
	// 设置Fiddler配置, 注意,这个要在 _Start()之前调用才生效
	__DECLSPEC bool __FIDDLER_CALL SetFiddlerConfig(__in const FiddlerConfig* const);
	// 添加回调处理
	__DECLSPEC bool __FIDDLER_CALL SetCallBack(
		__in lpCallBack lpCallBackBeforeRequest, __in lpCallBack lpCallBackBeforeResponse,
		__in lpCallBack lpCallBackAfterSessionComplete, __in lpCallBack lpCallBackOnWebSocketMessage
	);

	// 修改Fiddler数据
	__DECLSPEC bool __FIDDLER_CALL ModifyFiddlerData(__in const FiddlerData* const);
	// 导出Fiddler根证书
	__DECLSPEC bool __FIDDLER_CALL ExportRootCert(__in const char* const lpszFileName);
	// 创建和信任Fiddler根证书,https需要
	__DECLSPEC bool __FIDDLER_CALL CreateAndTrustRootCert();
	// 删除fiddler生成的证书
	__DECLSPEC bool __FIDDLER_CALL RemoveCert(__in const bool bRemoveRoot);
	// 释放FidderConfig
	__DECLSPEC void __FIDDLER_CALL FreeFiddlerConfig(__in FiddlerConfig* data);
	// 释放FreeFiddlerData
	__DECLSPEC void __FIDDLER_CALL FreeFiddlerData(__in FiddlerData* data);
	//
	__DECLSPEC void __FIDDLER_CALL WriteLog(const int nLogLevel, const char* const lpszLog);
	// 初始化默认Fiddler配置信息
	__DECLSPEC bool __FIDDLER_CALL   InitConfig(int nLangueue);

	__DECLSPEC void __FIDDLER_CALL OnExit();

	__DECLSPEC bool __FIDDLER_CALL GetDotNetVersion(pVersion pVer);
	__DECLSPEC void  __FIDDLER_CALL FreeVersion(pVersion ver);

#ifdef __cplusplus
}
#endif // __cplusplus

#define __FILLDER_API__H__
#endif // __FILLDER_API__H__

