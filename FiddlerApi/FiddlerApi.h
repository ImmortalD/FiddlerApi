#pragma once

#include <list>
#include <string>
using namespace std;

#ifdef __FIDDLER_API_SOURCE

#define __DECLSPEC     __declspec(dllexport)  
#include <stdlib.h>
#include <iostream>
#include <fstream> 
#include <map>
#include <sstream>
#include <msclr\marshal_cppstd.h>    
#using "..\\resources\\FiddlerCore4.dll"
using namespace System::Collections::Generic;
using namespace msclr::interop;
using namespace System;
using namespace Fiddler;
using namespace System::Threading;

#else
#define __DECLSPEC    __declspec(dllimport)  
#endif // __FIDDLER_API_SOURCE

#define  __FIDDLER_CALL _stdcall

typedef struct HttpHeader
{
	string name;
	string value;
}*pHttpHeader;

typedef struct FiddlerData
{
	// id
	const int id;
	// event is send(0), recv(1) or complete(2)
	const int event;
	// Http请求的进程ID
	const int processId;
	// 是否要修改Http响应body,如果需要修改需要在回调函数event=SEND的时候把bModifyResponseBody修改为true
	bool bModifyResponseBody;
	int modifyFlags;
	// url
	string*   url;
	string*   httpMethod;
	string*   httpVersion;
	// http header 
	string*   header;
	// http body
	string*   body;
} *pFiddlerData;

// FiddlerData modifyFlags values
enum ModifyFlag
{
	MODIFY_NO_ALL = 0,
	MODIFY_URL = 1,
	MODIFY_BODY = 2,
	MODIFY_HEADERS = 4,
	MODIFY_ALL = MODIFY_URL | MODIFY_BODY | MODIFY_HEADERS
};

// FiddlerData event values
enum EventType
{
	SEND = 0,
	RECV,
	COMPLETE
};


enum LogLevel
{
	CLOSE = 0,
	ERR = 1,
	WARN = 2 | ERR,
	INFO = 4 | WARN,
	DEBUG = 8 | INFO
};

typedef struct FiddlerConfig
{
	int iListenPort;
	bool bRegisterAsSystemProxy;
	// 默认是否都修改ResponseBody
	bool bAlwaysModifyResponseBody;
	bool bDecryptSSL;
	bool bAllowRemote;
	int iSecureEndpointPort;
	LogLevel logLevel;
	char *sSecureEndpointHostname;
	char* szLogFileName;
} *pFiddlerConfig;

typedef struct Version
{
	// 主要版本
	int major;
	// 次版本
	int minor;
	// 内部版本号的不同表示对相同源所作的重新编译
	int build;
	// 名称、主版本号和次版本号都相同但修订号不同的程序集应是完全可互换的。这适用于修复以前发布的程序集中的安全漏洞
	int revision;
	// 名称
	char*  name;

}*pVersion ,_Version;

// 消息回调处理函数
typedef bool(*lpCallBack)(FiddlerData& data);

typedef struct  CallBackInfo
{
	// 回调函数
	lpCallBack lpAddr;
	// 版本信息
	pVersion version;

}*pCallBackData;
 
#ifdef __cplusplus
extern "C" {
#endif //  _cplusplus

	// 初始化Fiddler
	__DECLSPEC bool __FIDDLER_CALL Start();
	// 关闭Fiddler
	__DECLSPEC bool __FIDDLER_CALL Close();
	// 添加回调处理
	__DECLSPEC  bool __FIDDLER_CALL   AddSimpleCallBack(__in const lpCallBack callBack);
	// 添加回调处理
	__DECLSPEC void __FIDDLER_CALL AddCallBack(__in const CallBackInfo&);
	// 创建和信任Fiddler根证书,https需要
	__DECLSPEC bool __FIDDLER_CALL CreateAndTrustRootCert();
	// 删除fiddler生成的证书
	__DECLSPEC bool __FIDDLER_CALL RemoveCert(bool bRemoveRoot);
	// 写日志
	__DECLSPEC void __FIDDLER_CALL WriteLog(LogLevel level, const char* const str);
	// 写日志
	__DECLSPEC void __FIDDLER_CALL WriteLogEx(__in LogLevel level, __in string& str);
	// 获取配置
	__DECLSPEC  void __FIDDLER_CALL GetFiddlerConfig(FiddlerConfig& data);
	// 设置Fiddler配置
	__DECLSPEC bool __FIDDLER_CALL SetFiddlerConfig(const FiddlerConfig&);
	// 初始化默认Fiddler配置信息,DLL加载是自动初始化,外部不需要调用这个函数
	 __DECLSPEC  bool  InitConfig();

#ifdef __cplusplus
}
#endif //  _cplusplus

#define  CRLF "\r\n"
#define  COLON ":"
#define  SPACE " "
