#include <list>
#include <string>
#include<unordered_map>
#include <fstream>

#include <msclr\marshal_cppstd.h>  
using namespace msclr::interop;

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace System::Collections::Concurrent;
using namespace System::Threading;
using System::Text::StringBuilder;

#define CRLF "\r\n"
#define COLON ":"
#define SPACE " "
// easyloggingpp相关
#include "../easyloggingpp/easylogging++.h"
INITIALIZE_EASYLOGGINGPP
// #define ELPP_NO_DEFAULT_LOG_FILE
// #define ELPP_THREAD_SAFE

#include "FiddlerApi.h"
#using "../FiddlerCore/FiddlerCore4.dll"
using namespace Fiddler;
#include "util.h"

FiddlerConfig g_FdConfig;
unordered_map<int /* sessionId */, int /* NotifyType */ >  g_Notifys;
// 临界区结构对象
CRITICAL_SECTION g_cs_Notifys;
CRITICAL_SECTION g_cs_Log;
// 是否调用默认初始化
bool bInitConfig = false;
bool bHaveEventProcess = false;
void ProcessCallBack(Session^ oS, EventType eventType, int webSocketId, int nNotifyType, lpCallBack callBack, String^ webSocketMsg = nullptr);
void InitFiddlerData(__in Session^ const oS, __in  const EventType eventType, __in  int  webSocketId, int nNotifyType, __out FiddlerData**   data);

int g_nLanguage = 0;
lpCallBack g_lpECallBackBeforeRequest;
lpCallBack g_lpECallBackBeforeResponse;
lpCallBack g_lpECallBackAfterSessionComplete;
lpCallBack g_lpECallBackOnWebSocketMessage;

el::Logger* g_Logger = nullptr;

// 写日志
void __FIDDLER_CALL WriteLog(const int nLogLevel, const char* const szLog)
{
	if (g_FdConfig.nLogLevel < INFO)
	{
		return;
	}

	EnterCriticalSection(&g_cs_Log);
	if (ERR == nLogLevel)
	{
		g_Logger->error(szLog);
	}
	else if (WARN == nLogLevel)
	{
		g_Logger->warn(szLog);
	}
	else if (INFO == nLogLevel)
	{
		g_Logger->info(szLog);
	}
	else if (DEBUG == nLogLevel)
	{
		g_Logger->debug(szLog);
	}
	else
	{
		g_Logger->error("unknow log level %d : %s", nLogLevel, szLog);
	}

	LeaveCriticalSection(&g_cs_Log);
}

void WriteLogEx(const int nLogLevel, const char* const format, ...)
{
	int final_n, n = strlen(format) * 2; /* Reserve two times as much as the length of the fmt_str */
	std::unique_ptr<char[]> formatted;
	va_list ap;
	while (1) {
		formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
		strcpy_s(&formatted[0], strlen(format) + 1, format);
		va_start(ap, format);
		final_n = vsnprintf(&formatted[0], n, format, ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}

	WriteLog(nLogLevel, formatted.get());
}

bool IsNeedCallBack(const int dwProcessId, String^ ProcessName)
{
	char* szProcessName = (char*)Marshal::StringToHGlobalAnsi(ProcessName).ToPointer();

	// 有通知,进程id或进程名匹配   
	bool res = !(NOTIFY_NO_ALL == g_FdConfig.nNotifyType)
		&&
		(RegexIsMatch(g_FdConfig.lpszRegexProcessId, to_string(dwProcessId).c_str())
			|| RegexIsMatch(g_FdConfig.lpszRegexProcessName, szProcessName));

	Marshal::FreeHGlobal(IntPtr(szProcessName));
	return res;
}

void inline LockNotifysData()
{
	EnterCriticalSection(&g_cs_Notifys);
}

void inline UnLockNotifysData()
{
	LeaveCriticalSection(&g_cs_Notifys);
}

char* GetHeader(IEnumerator<HTTPHeaderItem^>^ header)
{
	StringBuilder^ res = gcnew StringBuilder("");
	while (header->MoveNext())
	{
		res->Append(header->Current->Name)->Append(COLON)->Append(SPACE)
			->Append(header->Current->Value)->Append(CRLF);
	}
	return (char*)Marshal::StringToHGlobalAnsi(res->ToString(0, res->Length > 2 ? res->Length - 2 : res->Length)).ToPointer();
}
// 把Session的一些属性copy到FiddlerData
void InitFiddlerData(__in Session^ const oS, __in  const EventType eventType, __in  int  webSocketId, int nNotiftType, __out FiddlerData**   data)
{
	*data = (FiddlerData*)malloc(sizeof(FiddlerData));
	memset(*data, 0, sizeof(FiddlerData));
	(*data)->nId = oS->id;
	(*data)->eventType = eventType;
	(*data)->nWebSocketId = webSocketId;
	(*data)->nProcessId = oS->LocalProcessID;
	(*data)->bModifyResponseBody = g_FdConfig.nConfigEx & FC_ALWAYS_MODIFY_RESPONSE_BODY;
	(*data)->nModifyFlags = MODIFY_NO_ALL;
	(*data)->nNotifyType = nNotiftType;
	(*data)->lpszProcessName = (char*)Marshal::StringToHGlobalAnsi(oS->LocalProcess->Substring(0, oS->LocalProcess->IndexOf(":"))).ToPointer();
	(*data)->lpszUrl = (char*)Marshal::StringToHGlobalAnsi(oS->fullUrl).ToPointer();

	if (SEND == eventType)
	{
		(*data)->lpszHttpMethod = (char*)Marshal::StringToHGlobalAnsi(oS->RequestHeaders->HTTPMethod).ToPointer();
		(*data)->lpszHttpVersion = (char*)Marshal::StringToHGlobalAnsi(oS->RequestHeaders->HTTPVersion).ToPointer();
		(*data)->nResponseCode = 0;
		(*data)->lpszResponseStatus = nullptr;
	}
	else if (RECV == eventType || COMPLETE == eventType)
	{
		(*data)->lpszHttpMethod = (char*)Marshal::StringToHGlobalAnsi(oS->RequestHeaders->HTTPMethod).ToPointer();
		(*data)->lpszHttpVersion = (char*)Marshal::StringToHGlobalAnsi(oS->RequestHeaders->HTTPVersion).ToPointer();
		(*data)->nResponseCode = oS->ResponseHeaders->HTTPResponseCode;
		(*data)->lpszResponseStatus = (char*)Marshal::StringToHGlobalAnsi(oS->ResponseHeaders->HTTPResponseStatus).ToPointer();
	}

	if (SEND == eventType)
	{
		(*data)->lpszHeader = GetHeader(oS->RequestHeaders->GetEnumerator()); // (char*)Marshal::StringToHGlobalAnsi(oS->RequestHeaders->ToString()).ToPointer();
		if (webSocketId ==0)
		{
			(*data)->lpszBody = (char*)Marshal::StringToHGlobalAnsi(oS->GetRequestBodyAsString()).ToPointer();
		}
	}
	else if (RECV == eventType || COMPLETE == eventType)
	{
		(*data)->lpszHeader = GetHeader(oS->ResponseHeaders->GetEnumerator()); // (char*)Marshal::StringToHGlobalAnsi(oS->ResponseHeaders->ToString()).ToPointer();
		if (webSocketId == 0)
		{
			(*data)->lpszBody = (char*)Marshal::StringToHGlobalAnsi(oS->GetResponseBodyAsString()).ToPointer();
		}
	}
}

public ref struct EventReceiver
{
public:
	static Proxy ^ oSecureEndpoint;
	//static   System::Threading::Reader    rWriterLockSlim^ rwl = gcnew ReaderWriterLockSlim();
	static IDictionary<int/*sessionId*/, Session^>^ oAllSessions = gcnew ConcurrentDictionary<int, Session^>(Environment::ProcessorCount,16);
	static IDictionary<int /*sessionId << 16 + websocketId */, WebSocketMessageEventArgs^>^ webSocktMsg = gcnew ConcurrentDictionary<int, WebSocketMessageEventArgs^>(Environment::ProcessorCount, 4);

	void OnNotification(Object^ sender, NotificationEventArgs^ e)
	{
		char* szStr = (char*)Marshal::StringToHGlobalAnsi(e->NotifyString).ToPointer();
		WriteLogEx(DEBUG, "[OnNotification] %s", szStr);
		Marshal::FreeHGlobal(IntPtr(szStr));
	}

	void OnLogString(Object^ sender, LogEventArgs ^e)
	{
		char* szStr = (char*)Marshal::StringToHGlobalAnsi(e->LogString).ToPointer();
		WriteLogEx(DEBUG, "[OnLogString] %s", szStr);
		Marshal::FreeHGlobal(IntPtr(szStr));
	};

	// Http请求之前
	void BeforeRequest(Session^ oS)
	{
		WriteLogEx(DEBUG, "[BeforeRequest Start] id=%d", oS->id);
		if (g_FdConfig.lpCallBack->lpCallBackBeforeRequest 	&&  IsNeedCallBack(oS->LocalProcessID, oS->LocalProcess->Substring(0, oS->LocalProcess->IndexOf(":"))))
		{
			EventReceiver::oAllSessions[oS->id] = oS;
			LockNotifysData();
			g_Notifys[oS->id] = g_FdConfig.nNotifyType;
			UnLockNotifysData();
			ProcessCallBack(oS, SEND, 0, g_FdConfig.nNotifyType, g_FdConfig.lpCallBack->lpCallBackBeforeRequest);
		}
		WriteLogEx(DEBUG, "[BeforeRequest End] id=%d", oS->id);
	}

	// Http响应之前
	void BeforeResponse(Session^ oS)
	{
		WriteLogEx(DEBUG, "[BeforeResponse Start] id=%d", oS->id);

		LockNotifysData();
		int t = g_Notifys[oS->id];
		UnLockNotifysData();

		if (t & NOTIFY_BEFORE_RESPONSE 	&& g_FdConfig.lpCallBack->lpCallBackBeforeResponse)
		{
			if (IsNeedCallBack(oS->LocalProcessID, oS->LocalProcess->Substring(0, oS->LocalProcess->IndexOf(":"))))
			{
				ProcessCallBack(oS, RECV, 0, t, g_FdConfig.lpCallBack->lpCallBackBeforeResponse);
			}
		}

		WriteLogEx(DEBUG, "[BeforeResponse End] id=%d", oS->id);
	}

	// 处理完成
	void AfterSessionComplete(Session^ oS)
	{
		WriteLogEx(DEBUG, "[AfterSessionComplete Start] id=%d", oS->id);

		LockNotifysData();
		int t = g_Notifys[oS->id];
		g_Notifys.erase(oS->id);
		UnLockNotifysData();
		oAllSessions->Remove(oS->id);
		if (t & NOTIFY_COMPOLE_RESPONSE && g_FdConfig.lpCallBack->lpCallBackAfterSessionComplete)
		{
			if (IsNeedCallBack(oS->LocalProcessID, oS->LocalProcess->Substring(0, oS->LocalProcess->IndexOf(":"))))
			{
				ProcessCallBack(oS, COMPLETE, 0, t, g_FdConfig.lpCallBack->lpCallBackAfterSessionComplete);
			}
		}

		WriteLogEx(DEBUG, "[AfterSessionComplete End] id=%d", oS->id);
	}

	void  OnWebSocketMessage(Object ^ sender, WebSocketMessageEventArgs^ e)
	{
		Session^ oS = (Session^)sender;
		WriteLogEx(DEBUG, "[OnWebSocketMessage Start] id=%d,webSocketId=%d", oS->id, e->oWSM->ID);
		if (g_FdConfig.lpCallBack->lpCallBackOnWebSocketMessage &&  IsNeedCallBack(oS->LocalProcessID, oS->LocalProcess->Substring(0, oS->LocalProcess->IndexOf(":"))))
		{
			int id = oS->id << 16 | e->oWSM->ID;
			webSocktMsg[id] = e;
			ProcessCallBack((Session^)sender, e->oWSM->IsOutbound ? SEND : RECV, e->oWSM->ID, NOTIFY_ALL, g_FdConfig.lpCallBack->lpCallBackOnWebSocketMessage, e->oWSM->PayloadAsString());
			webSocktMsg->Remove(id);
		}
		WriteLogEx(DEBUG, "[OnWebSocketMessage End] id=%d,webSocketId=%d", oS->id, e->oWSM->ID);
	}

	void BeforeReturningError(Session^ oS)
	{

	}

	void AfterSocketAccept(Object ^ sender, ConnectionEventArgs^ e)
	{
	}

	void AfterSocketConnect(Object ^ sender, ConnectionEventArgs^ e)
	{
	}
	void OnValidateServerCertificate(Object ^ sender, ValidateServerCertificateEventArgs^ e)
	{
	 
	}
};


void __FIDDLER_CALL  InternFreeFiddlerData(__in FiddlerData** data)
{
	if (*data)
	{
		if((*data)->lpszUrl) Marshal::FreeHGlobal(IntPtr((*data)->lpszUrl));
		if((*data)->lpszHttpMethod) Marshal::FreeHGlobal(IntPtr((*data)->lpszHttpMethod));
		if((*data)->lpszHttpVersion) Marshal::FreeHGlobal(IntPtr((*data)->lpszHttpVersion));
		if((*data)->lpszHeader) Marshal::FreeHGlobal(IntPtr((*data)->lpszHeader));
		if((*data)->lpszBody) Marshal::FreeHGlobal(IntPtr((*data)->lpszBody));
		
		SafeFree(*data);
	}
	*data = nullptr;
}

void ProcessCallBack(Session^ oS, EventType nEventType, int nWebSocketId, int nNotifyType, lpCallBack callBack, String^ webSocketMsg)
{
	// 初始化数据
	FiddlerData *data;
	InitFiddlerData(oS, nEventType, nWebSocketId, nNotifyType, &data);
	if (nWebSocketId > 0 && webSocketMsg)
	{
		data->lpszBody = (char*)Marshal::StringToHGlobalAnsi(webSocketMsg).ToPointer();
	}
	// 保存原来数据地址,使其能正常释放内存
	FiddlerData* CallBackData = (FiddlerData*)malloc(sizeof(FiddlerData));
	memcpy(CallBackData, data, sizeof(FiddlerData)); 

	if (LANGUAGE_E == g_nLanguage)
	{

		if (nWebSocketId > 0)
		{
			callBack = g_lpECallBackOnWebSocketMessage;
		}
		else if(SEND == nEventType)
		{
			callBack = g_lpECallBackBeforeRequest;
		}
		else if (RECV == nEventType)
		{
			callBack = g_lpECallBackBeforeResponse;
		}
		else if (COMPLETE == nEventType)
		{
			callBack = g_lpECallBackAfterSessionComplete;
		}
		 
		if (!callBack) 
		{
			return;
	     }
	}
	
	// 回调
	if (!callBack(*CallBackData))
	{
		WriteLogEx(DEBUG, "request abort id=%d,webSocketId=%d", oS->id, nWebSocketId);
		oS->Abort();
		LockNotifysData();
		g_Notifys.erase(oS->id);
		UnLockNotifysData();
		EventReceiver::oAllSessions->Remove(oS->id);
	}

	if (LANGUAGE_E == g_nLanguage)
	{
		return;
	}
	// 释放
	SafeFree(CallBackData);
	CallBackData = nullptr; 

	// 释放由于InitFiddlerData中由托管字符串转换到非托管字符串的非托管字符串,已经
	InternFreeFiddlerData(&data);
	return; 
}

bool InitLogConfig()
{
	el::Configurations defaultConf;
	defaultConf.setToDefault();
	ofstream LogFile;
	LogFile.open("log.conf", ios::in);
	// 配置文件存在
	if (LogFile)
	{
		el::Configurations conf("log.conf");
		el::Loggers::reconfigureAllLoggers(conf);
		g_Logger = el::Loggers::getLogger("default");
		WriteLogEx(INFO, "use log.conf file config");
	}
	else
	{
		// 配置文件不存在,默认配置如下
		defaultConf.parseFromText("*GLOBAL:\n ENABLED = true");
		defaultConf.parseFromText("*GLOBAL:\n TO_FILE = true");
		defaultConf.parseFromText("*GLOBAL:\n TO_STANDARD_OUTPUT = true");
		char buf[512] = { 0 };
		sprintf_s(buf, 512, "*GLOBAL:\n FILENAME = %s", g_FdConfig.lpszLogFileName);
		defaultConf.parseFromText(buf);
		defaultConf.parseFromText("*GLOBAL:\n MILLISECONDS_WIDTH = 3");
		defaultConf.parseFromText("*GLOBAL:\n PERFORMANCE_TRACKING = false");
		defaultConf.parseFromText("*GLOBAL:\n MAX_LOG_FILE_SIZE = 1048576");
		defaultConf.parseFromText("*GLOBAL:\n LOG_FLUSH_THRESHOLD = 0");
		defaultConf.parseFromText("*GLOBAL:\n FORMAT = [%level | %datetime] | %msg");
		el::Loggers::reconfigureAllLoggers(defaultConf);
		g_Logger = el::Loggers::getLogger("default");
		WriteLogEx(INFO, "log.conf file not exist,use default config");
	}

	return true;
}

bool StartFiddler()
{
	FiddlerApplication::SetAppDisplayName("FiddlerCoreApp");
	// 禁止连接到HTTPS使用无效证书的站点 
	CONFIG::IgnoreServerCertErrors = false;
	// 创建一个https侦听器，用于伪装成https服务器
	FiddlerApplication::Prefs->SetBoolPref("fiddler.network.streaming.abortifclientaborts", true);

	if (!bHaveEventProcess)
	{
		EventReceiver^ pR = gcnew EventReceiver;
		// OnWebSocketMessage回调
		FiddlerApplication::OnWebSocketMessage += gcnew EventHandler<WebSocketMessageEventArgs^>(pR, &EventReceiver::OnWebSocketMessage);
		// OnNotification 回调
		FiddlerApplication::OnNotification += gcnew EventHandler<NotificationEventArgs^>(pR, &EventReceiver::OnNotification);
		// OnLogString 回调
		FiddlerApplication::Log->OnLogString += gcnew EventHandler<LogEventArgs^>(pR, &EventReceiver::OnLogString);
		// AfterSocketAccept回调
		FiddlerApplication::AfterSocketAccept += gcnew EventHandler<ConnectionEventArgs^>(pR, &EventReceiver::AfterSocketAccept);
		// AfterSocketConnect回调
		FiddlerApplication::AfterSocketConnect += gcnew EventHandler<ConnectionEventArgs^>(pR, &EventReceiver::AfterSocketConnect);

		// BeforeReturningError回调
		FiddlerApplication::BeforeReturningError += gcnew SessionStateHandler(pR, &EventReceiver::BeforeReturningError);
		// BeforeRequest 回调
		FiddlerApplication::BeforeRequest += gcnew SessionStateHandler(pR, &EventReceiver::BeforeRequest);
		// BeforeResponse 回调
		FiddlerApplication::BeforeResponse += gcnew SessionStateHandler(pR, &EventReceiver::BeforeResponse);
		// AfterSessionComplete 回调
		FiddlerApplication::AfterSessionComplete += gcnew SessionStateHandler(pR, &EventReceiver::AfterSessionComplete);

		FiddlerApplication::OnValidateServerCertificate += gcnew   EventHandler<ValidateServerCertificateEventArgs^>(pR, &EventReceiver::OnValidateServerCertificate);
	}
	bHaveEventProcess = true;

	FiddlerCoreStartupSettingsBuilder ^ SettingsBuilder = gcnew FiddlerCoreStartupSettingsBuilder();

	if (g_FdConfig.nConfigEx & FC_ALLOW_REMOTE_CLIENTS) {
		SettingsBuilder->AllowRemoteClients();
	}
	if (g_FdConfig.nConfigEx & FC_CAPTURE_FTP) {
		SettingsBuilder->CaptureFTP();
	}
	if (g_FdConfig.nConfigEx &  FC_CAPTURE_LOCALHOST_TRAFFIC) {
		SettingsBuilder->CaptureLocalhostTraffic();
	}
	if (g_FdConfig.nConfigEx &  FC_CHAINTO_UPSTREAM_GATEWAY) {
		SettingsBuilder->ChainToUpstreamGateway();
	}
	if (g_FdConfig.nConfigEx &  FC_DECRYPT_SSL) {
		SettingsBuilder->DecryptSSL();
	}
	if (g_FdConfig.nConfigEx &  FC_HOOKUSING_PACFILE) {
		SettingsBuilder->HookUsingPACFile();
	}
	if (g_FdConfig.nConfigEx &  FC_MONITORALL_CONNECTIONS) {
		SettingsBuilder->MonitorAllConnections();
	}
	if (g_FdConfig.nConfigEx &  FC_OPTIMIZE_THREADPOOL) {
		SettingsBuilder->OptimizeThreadPool();
	}
	if (g_FdConfig.nConfigEx &  FC_REGISTER_AS_SYSTEM_PROXY) {
		SettingsBuilder->RegisterAsSystemProxy();
	}
	if (g_FdConfig.lpszUpstreamGatewayTo) {
		SettingsBuilder->SetUpstreamGatewayTo(marshal_as<String^>(string(g_FdConfig.lpszUpstreamGatewayTo)));
	}

	SettingsBuilder->ListenOnPort(g_FdConfig.nListenPort);
	Fiddler::CONFIG::QuietMode = true;
	FiddlerApplication::Startup(SettingsBuilder->Build());
	// 我们还将创建一个HTTPS监听器，当FiddlerCore被伪装成HTTPS服务器有用  
	// 而不是作为一个正常的CERN样式代理服务器。  
	EventReceiver::oSecureEndpoint = FiddlerApplication::CreateProxyEndpoint(g_FdConfig.nSecureEndpointPort, true, marshal_as<String^>(string(g_FdConfig.lpszSecureEndpointHostname)));

	return EventReceiver::oSecureEndpoint != nullptr;
}

bool __FIDDLER_CALL Start()
{
	if (!bInitConfig)
	{
		WriteLog(ERR, "还没有初始化,请先调用InitConfig");
		return false;
	}
	if (g_FdConfig.nConfigEx & FC_DECRYPT_SSL)
	{
		CreateAndTrustRootCert();
	}
	if (FiddlerApplication::IsStarted())
	{
		WriteLogEx(WARN, "you already call Start");
		return true;
	}

	InitLogConfig();
	
	pVersion ver = (pVersion)malloc(sizeof(int) * 4);
	GetDotNetVersion(ver);
	WriteLogEx(INFO, ".Net Framework Version:%d.%d.%d Build:%d", ver->nMajor, ver->nMinor, ver->nRevision, ver->nBuild);
	FreeVersion(ver);

	StartFiddler();
	return true;
}

// 默认初始化配置
bool __FIDDLER_CALL InitConfig(int nLanguage)
{
	g_nLanguage = nLanguage;
	// 初始化临界区
	InitializeCriticalSection(&g_cs_Notifys);
	InitializeCriticalSection(&g_cs_Log);

	// 初始化g_FdConfig
	memset(&g_FdConfig, 0, sizeof(FiddlerConfig));
	g_FdConfig.nListenPort = 6666;
	g_FdConfig.nConfigEx = FC_DEFAULT;
	g_FdConfig.nSecureEndpointPort = 7777;
	g_FdConfig.nLogLevel = ERR;
	g_FdConfig.nNotifyType = NOTIFY_BEFORE_SEND | NOTIFY_BEFORE_RESPONSE;
	g_FdConfig.lpCallBack = (lpFiddlerCallBack)malloc(sizeof(FiddlerCallBack));
	memset(g_FdConfig.lpCallBack, 0, sizeof(FiddlerCallBack));
	// g_FdConfig = { 6666,FC_DEFAULT,7777,CLOSE, (NotifyType)(NOTIFY_BEFORE_SEND | NOTIFY_BEFORE_RESPONSE),nullptr };

	CopyData(&g_FdConfig.lpszSecureEndpointHostname, "localhost");
	CopyData(&g_FdConfig.lpszSecureEndpointHostname, g_FdConfig.lpszSecureEndpointHostname);
	CopyData(&g_FdConfig.lpszLogFileName, "./fiddler.log");
	CopyData(&g_FdConfig.lpszRegexProcessId, ".*");
	CopyData(&g_FdConfig.lpszRegexProcessName, ".*");

	bInitConfig = true;
	WriteLog(DEBUG, "初始化配置完毕");
	return true;
}

bool __FIDDLER_CALL GetFiddlerConfig(FiddlerConfig* const data)
{
	if (!bInitConfig)
	{
		WriteLog(ERR, "还没有初始化,请先调用InitConfig");
		return false;
	}
	memcpy(data, &g_FdConfig, sizeof(FiddlerConfig));
	data->lpCallBack->lpCallBackBeforeRequest = g_FdConfig.lpCallBack->lpCallBackBeforeRequest;
	data->lpCallBack->lpCallBackBeforeResponse = g_FdConfig.lpCallBack->lpCallBackBeforeResponse;
	data->lpCallBack->lpCallBackAfterSessionComplete = g_FdConfig.lpCallBack->lpCallBackAfterSessionComplete;
	data->lpCallBack->lpCallBackOnWebSocketMessage = g_FdConfig.lpCallBack->lpCallBackOnWebSocketMessage;

	CopyData(&data->lpszSecureEndpointHostname, g_FdConfig.lpszSecureEndpointHostname);
	CopyData(&data->lpszLogFileName, g_FdConfig.lpszLogFileName);
	CopyData(&data->lpszRegexProcessId, g_FdConfig.lpszRegexProcessId);
	CopyData(&data->lpszRegexProcessName, g_FdConfig.lpszRegexProcessName);
	CopyData(&data->lpszUpstreamGatewayTo, g_FdConfig.lpszUpstreamGatewayTo);
	return true;
}

// 设置Fiddler配置
bool __FIDDLER_CALL SetFiddlerConfig(const FiddlerConfig* const data)
{
	if (!bInitConfig)
	{
		WriteLog(ERR, "还没有初始化,请先调用InitConfig");
		return false;
	}

	if (!data)
	{
		WriteLogEx(ERR, "调用SetFiddlerConfig参数不能是null");
		return false;
	}

	if (FiddlerApplication::IsStarted())
	{
		// 开启Fiddler后不能修改
		WriteLogEx(ERR, "you should call SetFiddlerConfig before Start");
		return false;
	}

	memcpy(&g_FdConfig, data, sizeof(FiddlerConfig));
	if (data->lpCallBack)
	{
		SetCallBack(data->lpCallBack->lpCallBackBeforeRequest, data->lpCallBack->lpCallBackBeforeResponse,
			data->lpCallBack->lpCallBackAfterSessionComplete, data->lpCallBack->lpCallBackOnWebSocketMessage);
	}
	else 
	{
		SetCallBack(nullptr, nullptr, nullptr, nullptr);
	}
		 
	if (data->lpszSecureEndpointHostname) CopyData(&g_FdConfig.lpszSecureEndpointHostname, data->lpszSecureEndpointHostname);
	if (data->lpszLogFileName) CopyData(&g_FdConfig.lpszLogFileName, data->lpszLogFileName);
	if (data->lpszRegexProcessId)	CopyData(&g_FdConfig.lpszRegexProcessId, CheckRegexIsValid(data->lpszRegexProcessId) ? data->lpszRegexProcessId : ".*");
	if (data->lpszRegexProcessName) CopyData(&g_FdConfig.lpszRegexProcessName, CheckRegexIsValid(data->lpszRegexProcessName) ? data->lpszRegexProcessName : ".*");
	if (data->lpszUpstreamGatewayTo) CopyData(&g_FdConfig.lpszUpstreamGatewayTo, data->lpszUpstreamGatewayTo);
	return true;
}

void SetHeader(__inout HTTPHeaders ^ const headers, __in const char* const lpszHeader)
{
	string hs = string(lpszHeader);
	int index;
	int preIndex = 0;
	while (string::npos != (index = hs.find_first_of(CRLF, preIndex)))
	{
		string h = hs.substr(preIndex, index - preIndex);
		headers->Add(Marshal::PtrToStringAnsi((IntPtr)(char *)h.substr(0, h.find(COLON)).c_str()),
			Marshal::PtrToStringAnsi((IntPtr)(char *)h.substr(h.find(":") + 2).c_str()));

		preIndex = index + 2;
	}
}

// 修改Http数据
bool __FIDDLER_CALL  ModifyFiddlerData(__in const FiddlerData* const newData)
{
	if (!(RECV == newData->eventType || SEND == newData->eventType))
	{
		return false;
	}

	Session^ oS;
	if (!EventReceiver::oAllSessions->TryGetValue(newData->nId, oS))
	{
		WriteLogEx(ERR, "id=%d,不存在,你不应该修改数据的id", newData->nId);
		return false;
	}

	if (SEND == newData->eventType)
	{
		WriteLogEx(DEBUG, "id=%d eventType=%d %d MODIFY_RESPONSEBODY", oS->id, newData->eventType, newData->bModifyResponseBody);
		oS->bBufferResponse = newData->bModifyResponseBody;
	}


	if (newData->nModifyFlags & MODIFY_NOTIFY)
	{
		WriteLogEx(DEBUG, "id=%d eventType=%d MODIFY_NOTIFY", oS->id, newData->eventType);
		LockNotifysData();
		g_Notifys[oS->id] = newData->nNotifyType;
		UnLockNotifysData();
	}
	// http method 
	if (newData->nModifyFlags && MODIFY_HTTPMETHOD && SEND == newData->eventType)
	{
		oS->RequestHeaders->HTTPMethod = Marshal::PtrToStringAnsi((IntPtr)(char *)newData->lpszHttpMethod);
	}
	// httpversion
	if (newData->nModifyFlags && MODIFY_HTTPVERSION)
	{
		if (SEND == newData->eventType)
		{
			oS->RequestHeaders->HTTPVersion = Marshal::PtrToStringAnsi((IntPtr)(char *)newData->lpszHttpVersion);
		}
		else if (RECV == newData->eventType)
		{
			oS->ResponseHeaders->HTTPVersion = Marshal::PtrToStringAnsi((IntPtr)(char *)newData->lpszHttpVersion);
		}
	}

	// 处理headers的修改
	if (newData->nModifyFlags & MODIFY_HEADERS)
	{
		WriteLogEx(DEBUG, "id=%d eventType=%d MODIFY_HEADERS", oS->id, newData->eventType);

		HTTPHeaders ^ headers;
		// 获取headers
		if (SEND == newData->eventType)
		{
			headers = oS->RequestHeaders;
		}
		else if (RECV == newData->eventType)
		{
			headers = oS->ResponseHeaders;
		}
		headers->RemoveAll();
		SetHeader(headers, newData->lpszHeader);

	}

	// 处理body的修改
	if (newData->nModifyFlags & MODIFY_BODY)
	{
		WriteLogEx(DEBUG, "id=%d eventType=%d MODIFY_BODY", oS->id, newData->eventType);
		if (SEND == newData->eventType)
		{
			WebSocketMessageEventArgs^ e;
			if (newData->nWebSocketId > 0)
			{
				if (EventReceiver::webSocktMsg->TryGetValue(newData->nId << 16 | newData->nWebSocketId, e))
				{
					e->oWSM->SetPayload(Marshal::PtrToStringAnsi((IntPtr)(char *)newData->lpszBody));
				}
				else
				{
					WriteLogEx(WARN, "不要修改webSocketId的值,webSocketId=%d", newData->nWebSocketId);
				}
			}
			else
			{
				oS->utilSetRequestBody(Marshal::PtrToStringAnsi((IntPtr)(char *)newData->lpszBody));
			}
		}
		else if (RECV == newData->eventType)
		{
			WebSocketMessageEventArgs^ e;
			if (newData->nWebSocketId > 0)
			{
				if (EventReceiver::webSocktMsg->TryGetValue(newData->nId << 16 | newData->nWebSocketId, e))
				{
					e->oWSM->SetPayload(Marshal::PtrToStringAnsi((IntPtr)(char *)newData->lpszBody));
				}
				else
				{
					WriteLogEx(WARN, "不要修改webSocketId的值,webSocketId=%d", newData->nWebSocketId);
				}
			}
			else  {
				!oS->utilDecodeResponse() && !oS->utilDecodeResponse(true) && !oS->utilDecodeResponse(false);
				oS->utilSetResponseBody(Marshal::PtrToStringAnsi((IntPtr)(char *)newData->lpszBody));
			}
		}
	}

	// 处理url的修改
	if (SEND == newData->eventType && (newData->nModifyFlags & MODIFY_URL))
	{
		WriteLogEx(DEBUG, "id=%d eventType=%d MODIFY_URL", oS->id, newData->eventType);
		oS->fullUrl = Marshal::PtrToStringAnsi((IntPtr)(char *)newData->lpszUrl);
	}

	// MODIFY_REPONSE_STATUS
	if (RECV == newData->eventType)
	{
		if (newData->nModifyFlags & MODIFY_REPONSE_STATUS)
		{
			oS->ResponseHeaders->HTTPResponseStatus = Marshal::PtrToStringAnsi((IntPtr)(char *)newData->lpszResponseStatus);
		}

		if (newData->nModifyFlags & MODIFY_REPONSE_STATUS)
		{
			oS->ResponseHeaders->HTTPResponseCode = newData->nResponseCode;
		}
	}

	return true;
}

bool __FIDDLER_CALL Close()
{
	
	if (!FiddlerApplication::IsStarted())
	{
		return true;
	}

	if (nullptr != EventReceiver::oSecureEndpoint)
	{
		EventReceiver::oSecureEndpoint->Detach();
	}
	WriteLogEx(INFO, "Fiddler Close Start...");
	FiddlerApplication::Shutdown();

	Sleep(1000);

	WriteLogEx(INFO, "Fiddler Close End");
	WriteLogEx(INFO, "未处理完的请求有%d个", g_Notifys.size());
	WriteLogEx(INFO, "未处理完的Session有%d个", EventReceiver::oAllSessions->Count);
	g_Logger->flush();
	g_Notifys.clear();
	EventReceiver::oAllSessions->Clear();

	return true;
}

// dll 卸载时调用
void __FIDDLER_CALL OnExit()
{
	// 释放资源
	DeleteCriticalSection(&g_cs_Notifys);
	DeleteCriticalSection(&g_cs_Log);

	SafeFree(g_FdConfig.lpCallBack);
	SafeFree(g_FdConfig.lpszLogFileName);
	SafeFree(g_FdConfig.lpszRegexProcessId);
	SafeFree(g_FdConfig.lpszRegexProcessName);
	SafeFree(g_FdConfig.lpszSecureEndpointHostname);
	SafeFree(g_FdConfig.lpszUpstreamGatewayTo);
	memset(&g_FdConfig, 0, sizeof(FiddlerConfig));
}

bool __FIDDLER_CALL SetCallBack(
	__in	lpCallBack lpCallBackBeforeRequest, __in lpCallBack lpCallBackBeforeResponse,
	__in lpCallBack lpCallBackAfterSessionComplete, __in lpCallBack lpCallBackOnWebSocketMessage
)
{
	g_FdConfig.lpCallBack->lpCallBackBeforeRequest = lpCallBackBeforeRequest;
	g_FdConfig.lpCallBack->lpCallBackBeforeResponse = lpCallBackBeforeResponse;
	g_FdConfig.lpCallBack->lpCallBackAfterSessionComplete = lpCallBackAfterSessionComplete;
	g_FdConfig.lpCallBack->lpCallBackOnWebSocketMessage = lpCallBackOnWebSocketMessage;
 
	if (LANGUAGE_E == g_nLanguage)
	{
		g_lpECallBackBeforeRequest = lpCallBackBeforeRequest;
		g_lpECallBackBeforeResponse = lpCallBackBeforeResponse;
		g_lpECallBackAfterSessionComplete = lpCallBackAfterSessionComplete;
		g_lpECallBackOnWebSocketMessage = lpCallBackOnWebSocketMessage;
	}
	return true;
}


bool __FIDDLER_CALL ExportRootCert(__in const char* const fileName)
{
	WriteLogEx(DEBUG, "导出FiddlerRootCert:%s", fileName);

	Security::Cryptography::X509Certificates::X509Certificate2^  FiddlerRootCert =
		Fiddler::CertMaker::FindCert("DO_NOT_TRUST_FiddlerAPIRoot");

	cli::array<unsigned char> ^CertData = FiddlerRootCert->GetRawCertData();

	ofstream CertFile(fileName, ios::binary);
	if (!CertFile)
	{
		WriteLogEx(DEBUG, "导出FiddlerRootCert失败,创建文件:%s失败", fileName);
		return false;
	}
	for (int i = 0; i < CertData->Length; i++)
	{
		CertFile << CertData[i];
	}
	CertFile.flush();
	CertFile.close();
	WriteLogEx(DEBUG, "导出FiddlerRootCert成功,证书文件:%s", fileName);
	return true;
}

bool __FIDDLER_CALL CreateAndTrustRootCert()
{
	if (!CertMaker::rootCertExists())
	{
		WriteLogEx(INFO, "创建Fiddler根证书");
		if (!CertMaker::createRootCert())
		{
			WriteLogEx(ERR, "创建Fiddler根证书失败");
			return false;
		}

		if (!CertMaker::trustRootCert())
		{
			WriteLogEx(WARN, "用户取消了信任Fiddler根证书");
			return false;
		}

	}
	else
	{
		if (!CertMaker::trustRootCert())
		{
			WriteLogEx(WARN, "用户取消了信任Fiddler根证书");
			return false;
		}
	}
	return true;
}

bool __FIDDLER_CALL RemoveCert(__in const bool bRemoveRoot)
{
	WriteLogEx(INFO, "移除Fiddler证书");
	return CertMaker::removeFiddlerGeneratedCerts(bRemoveRoot);
}

void __FIDDLER_CALL FreeFiddlerConfig(__in FiddlerConfig* data)
{
	if (data)
	{
		SafeFree(data->lpszLogFileName);
		SafeFree(data->lpszRegexProcessId);
		SafeFree(data->lpszRegexProcessName);
		SafeFree(data->lpszSecureEndpointHostname);
		SafeFree(data);
	}
}

void __FIDDLER_CALL  FreeFiddlerData(__in FiddlerData* data)
{
	if (data)
	{
		SafeFree(data->lpszUrl);
		SafeFree(data->lpszHttpMethod);
		SafeFree(data->lpszHttpVersion);
		SafeFree(data->lpszHeader);
		SafeFree(data->lpszBody);
		SafeFree(data);
	}
}

bool __FIDDLER_CALL GetDotNetVersion(pVersion pVer)
{
	System::Version^ version = Environment::Version;
	if (version && pVer)
	{
		pVer->nBuild = version->Build;
		pVer->nMajor = version->Major;
		pVer->nMinor = version->Minor;
		pVer->nRevision = Environment::Version->Revision;
		return true;
	}
	else {
		return false;
	}
}

void  __FIDDLER_CALL FreeVersion(pVersion ver)
{
	if (ver)
	{
		free(ver);
	}
}
