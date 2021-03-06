#include "FiddlerApi.h"
 
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"../../FiddlerApi/x64/Static_Lib_Debug/FiddlerApi.lib")
#else
#pragma comment(lib,"../../FiddlerApi/x64/Static_Lib_Release/FiddlerApi.lib")
#endif // _DEBUG
#else // _WIN64
#ifdef _DEBUG
#pragma comment(lib,"../../FiddlerApi/Static_Lib_Debug/FiddlerApi.lib")
#else
#pragma comment(lib,"../../FiddlerApi/Static_Lib_Release/FiddlerApi.lib")
#endif // _DEBUG
#endif


#include "FiddlerJNI.h"

#include<iostream>

using namespace std;

struct JNICallBackInfo
{
	jobject callBackObj;
	jmethodID lpBeforeRequestMethodId;
	jmethodID lpBeforeResponseMethodId;
	jmethodID lpAfterSessionCompleteMethodId;
	jmethodID lpOnWebSocketMessageMethodId;
};


JNICallBackInfo g_JNICallBack = { 0 };

bool JVM_BeforeRequest(FiddlerData& data);
bool JVM_BeforeResponse(FiddlerData& data);
bool JVM_AfterSessionComplete(FiddlerData& data);
bool JVM_OnWebSocketMessage(FiddlerData& data);

FiddlerCallBack g_CallBack = { JVM_BeforeRequest ,JVM_BeforeResponse ,JVM_AfterSessionComplete,JVM_OnWebSocketMessage };

JNIEXPORT  jboolean  JNICALL start0(JNIEnv *);
JNIEXPORT  jboolean   JNICALL    close0(JNIEnv *, jclass);
JNIEXPORT   jobject  JNICALL    getFiddlerConfig0(JNIEnv *, jclass);
JNIEXPORT  jboolean   JNICALL    setFiddlerConfig0(JNIEnv *, jclass, jobject);
JNIEXPORT    jboolean JNICALL    setCallBack0(JNIEnv *, jclass, jobject);
JNIEXPORT   jboolean  JNICALL    modifyFiddlerData0(JNIEnv *, jclass, jobject);
JNIEXPORT  jboolean   JNICALL    exportRootCert0(JNIEnv *, jclass, jstring);
JNIEXPORT  jboolean   JNICALL    createAndTrustRootCert0(JNIEnv *, jclass);
JNIEXPORT   jboolean  JNICALL    removeCert0(JNIEnv *, jclass, jboolean);
JNIEXPORT    void JNICALL    writeLog0(JNIEnv *, jclass, jint, jstring);
JNIEXPORT   void  JNICALL    initConfig0(JNIEnv *, jclass);
JNIEXPORT  void   JNICALL    registerNatives(JNIEnv *, jclass);
 

#define  STR(x)  const_cast<char*>(x)

 
static const JNINativeMethod methods[] = {
	{ STR("start0"),STR("()Z"),  (void*)start0 },
	{ STR("close0"),STR("()Z"),  (void*)close0 },
	{ STR("getFiddlerConfig0"),STR("()Lcom/immortal/fiddler/core/FiddlerConfig;"),  (void*)getFiddlerConfig0 },
	{ STR("setFiddlerConfig0"),STR("(Lcom/immortal/fiddler/core/FiddlerConfig;)Z"),  (void*)setFiddlerConfig0 },
	{ STR("setCallBack0"),STR("(Lcom/immortal/fiddler/core/FiddlerCallBack;)Z"),  (void*)setCallBack0 },
	{ STR("modifyFiddlerData0"),STR("(Lcom/immortal/fiddler/core/FiddlerData;)Z"),  (void*)modifyFiddlerData0 },
	{ STR("exportRootCert0"),STR("(Ljava/lang/String;)Z"),  (void*)exportRootCert0 },
	{ STR("createAndTrustRootCert0"),STR("()Z"),  (void*)createAndTrustRootCert0 },
	{ STR("removeCert0"),STR("(Z)Z"),  (void*)removeCert0 },
	{ STR("writeLog0"),STR("(ILjava/lang/String;)V"),  (void*)writeLog0 },
	{ STR("initConfig0"),STR("()Z"),  (void*)initConfig0 }
	//{ STR("registerNatives"),STR("()V"),  (void*)registerNatives }
};
 
// JNI 初始化会调用本方法,用于获取JavaVM
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	WriteLog(INFO, "jni OnLoad FiddlerApi");
	g_jvm = vm;
	JNIEnv * env = NULL;
	if (vm->GetEnv((void **)&env, JNI_VERSION) != JNI_OK) {
		WriteLog(INFO, "load FiddlerApi error!需要JDK8以上版本");
		return JNI_ERR;
	}
	else
	{
		WriteLog(INFO, "load FiddlerApi success!");
	}

	g_fdDataCls = env->FindClass("com/immortal/fiddler/core/FiddlerData");
	jclass FiddlerCls = env->FindClass("com/immortal/fiddler/core/Fiddler");

	if (nullptr == g_fdDataCls || nullptr == FiddlerCls)
	{
		goto error;
	}
	// env和thread紧密关联的变量,需要使用NewGlobalRef()生成全局变量才能用
	g_fdDataCls = (jclass)env->NewGlobalRef(g_fdDataCls);

	
	if (env->RegisterNatives(FiddlerCls, methods, sizeof(methods) / sizeof(methods[0])) < 0)
	{
		goto error;
	}

	env->DeleteLocalRef(FiddlerCls);
	return JNI_VERSION;

error:
	if (g_fdDataCls)
	{
		env->DeleteGlobalRef(g_fdDataCls);
	 }

	if (FiddlerCls)
	{
		env->DeleteLocalRef(FiddlerCls);
	}

	return JNI_ERR;
}


JNIEXPORT  jboolean  JNICALL start0(JNIEnv *)
{
	return Start();
}
JNIEXPORT  jboolean   JNICALL    close0(JNIEnv *, jclass)
{
	return Close();
}
JNIEXPORT  jboolean   JNICALL    exportRootCert0(JNIEnv *env, jclass, jstring filePath)
{
	return ExportRootCert(jstringToChar(env, filePath));
}
JNIEXPORT  jboolean   JNICALL    createAndTrustRootCert0(JNIEnv *, jclass)
{
	return CreateAndTrustRootCert();
}

JNIEXPORT   jboolean  JNICALL    removeCert0(JNIEnv *, jclass, jboolean bRemoveRoot)
{
	return RemoveCert(bRemoveRoot);
}

JNIEXPORT    void JNICALL    writeLog0(JNIEnv *env, jclass, jint level, jstring str)
{
	WriteLog(level, jstringToChar(env, str));
}

JNIEXPORT   void  JNICALL    initConfig0(JNIEnv *, jclass)
{
	InitConfig(LANGUAGE_JAVA);
	SetCallBack(g_CallBack.lpCallBackBeforeRequest, g_CallBack.lpCallBackBeforeResponse, \
		g_CallBack.lpCallBackAfterSessionComplete, g_CallBack.lpCallBackOnWebSocketMessage);

}


JNIEXPORT jobject JNICALL getFiddlerConfig0(JNIEnv *env, jclass)
{
	jclass fdConfCls = env->FindClass("com/immortal/fiddler/core/FiddlerConfig");

	static jfieldID nListenPort = env->GetFieldID(fdConfCls, "listenPort", "I");
	static jfieldID nConfigEx = env->GetFieldID(fdConfCls, "configEx", "I");
	static jfieldID nSecureEndpointPort = env->GetFieldID(fdConfCls, "secureEndpointPort", "I");
	static jfieldID nLogLevel = env->GetFieldID(fdConfCls, "logLevel", "I");
	static jfieldID nNotifyType = env->GetFieldID(fdConfCls, "notifyType", "I");
	static jfieldID lpCallBack = env->GetFieldID(fdConfCls, "callBack", "Lcom/immortal/fiddler/core/FiddlerCallBack;");
	static jfieldID lpszSecureEndpointHostname = env->GetFieldID(fdConfCls, "secureEndpointHostname", "Ljava/lang/String;");
	static jfieldID lpszLogFileName = env->GetFieldID(fdConfCls, "logFileName", "Ljava/lang/String;");
	static jfieldID lpszRegexProcessId = env->GetFieldID(fdConfCls, "regexProcessId", "Ljava/lang/String;");
	static jfieldID lpszRegexProcessName = env->GetFieldID(fdConfCls, "regexProcessName", "Ljava/lang/String;");
	static jfieldID lpszUpstreamGatewayTo = env->GetFieldID(fdConfCls, "upstreamGatewayTo", "Ljava/lang/String;");
	
	FiddlerConfig FdConfig;
	FdConfig.lpCallBack = new FiddlerCallBack();
	if (!GetFiddlerConfig(&FdConfig))
	{
		return nullptr;
	}

	// 创建对象
	jobject fdConfObj = env->NewObject(fdConfCls, env->GetMethodID(fdConfCls, "<init>", "()V"));

	env->SetIntField(fdConfObj, nListenPort, FdConfig.nListenPort);
	env->SetIntField(fdConfObj, nConfigEx, FdConfig.nConfigEx);
	env->SetIntField(fdConfObj, nSecureEndpointPort, FdConfig.nSecureEndpointPort);
	env->SetIntField(fdConfObj, nLogLevel, FdConfig.nLogLevel);
	env->SetIntField(fdConfObj, nNotifyType, FdConfig.nNotifyType);
	env->SetObjectField(fdConfObj, lpCallBack, g_JNICallBack.callBackObj);
	env->SetObjectField(fdConfObj, lpszSecureEndpointHostname, charTojstring(env, FdConfig.lpszSecureEndpointHostname));
	env->SetObjectField(fdConfObj, lpszLogFileName, charTojstring(env, FdConfig.lpszLogFileName));
	env->SetObjectField(fdConfObj, lpszRegexProcessId, charTojstring(env, FdConfig.lpszRegexProcessId));
	env->SetObjectField(fdConfObj, lpszRegexProcessName, charTojstring(env, FdConfig.lpszRegexProcessName));
    env->SetObjectField(fdConfObj, lpszUpstreamGatewayTo, charTojstring(env, FdConfig.lpszUpstreamGatewayTo));

	env->DeleteLocalRef(fdConfCls);
	free(FdConfig.lpCallBack);
	return fdConfObj;
}
// ---------------------------------------------

// -----------------------------------------------

JNIEXPORT  jboolean   JNICALL    setFiddlerConfig0(JNIEnv *env, jclass, jobject confObj)
{
	jclass fdConfCls = env->GetObjectClass(confObj);
	static jfieldID nListenPort = env->GetFieldID(fdConfCls, "listenPort", "I");
	static jfieldID nConfigEx = env->GetFieldID(fdConfCls, "configEx", "I");
	static jfieldID nSecureEndpointPort = env->GetFieldID(fdConfCls, "secureEndpointPort", "I");
	static jfieldID nLogLevel = env->GetFieldID(fdConfCls, "logLevel", "I");
	static jfieldID nNotifyType = env->GetFieldID(fdConfCls, "notifyType", "I");
	static jfieldID lpCallBack = env->GetFieldID(fdConfCls, "callBack", "Lcom/immortal/fiddler/core/FiddlerCallBack;");
	static jfieldID lpszSecureEndpointHostname = env->GetFieldID(fdConfCls, "secureEndpointHostname", "Ljava/lang/String;");
	static jfieldID lpszLogFileName = env->GetFieldID(fdConfCls, "logFileName", "Ljava/lang/String;");
	static jfieldID lpszRegexProcessId = env->GetFieldID(fdConfCls, "regexProcessId", "Ljava/lang/String;");
	static jfieldID lpszRegexProcessName = env->GetFieldID(fdConfCls, "regexProcessName", "Ljava/lang/String;");
	static jfieldID lpszUpstreamGatewayTo = env->GetFieldID(fdConfCls, "upstreamGatewayTo", "Ljava/lang/String;");
 
	FiddlerConfig* cf = new FiddlerConfig();
	cf->nListenPort = env->GetIntField(confObj, nListenPort);
	cf->nConfigEx = env->GetIntField(confObj, nConfigEx);
	cf->nSecureEndpointPort = env->GetIntField(confObj, nSecureEndpointPort);
	cf->nLogLevel = env->GetIntField(confObj, nLogLevel);
	cf->nNotifyType = env->GetIntField(confObj, nNotifyType);
 
	jobject callBackObj = env->GetObjectField(confObj, lpCallBack);
	setCallBack0(env, nullptr, callBackObj);
 
	cf->lpCallBack = &g_CallBack;
	cf->lpszSecureEndpointHostname = jstringToChar(env, (jstring)env->GetObjectField(confObj, lpszSecureEndpointHostname));
	cf->lpszLogFileName = jstringToChar(env, (jstring)env->GetObjectField(confObj, lpszLogFileName));
	cf->lpszRegexProcessId = jstringToChar(env, (jstring)env->GetObjectField(confObj, lpszRegexProcessId));
	cf->lpszRegexProcessName = jstringToChar(env, (jstring)env->GetObjectField(confObj, lpszRegexProcessName));
	cf->lpszUpstreamGatewayTo = jstringToChar(env, (jstring)env->GetObjectField(confObj, lpszUpstreamGatewayTo));

	SetFiddlerConfig(cf);
	delete cf;
	return true;
}

jobject FiddlerData2JFiddlerData(JNIEnv *env, const  FiddlerData& data)
{
	static jfieldID id = env->GetFieldID(g_fdDataCls, "id", "I");
	static jfieldID eventType = env->GetFieldID(g_fdDataCls, "eventType", "I");
	static jfieldID webSocketId = env->GetFieldID(g_fdDataCls, "webSocketId", "I");
	static jfieldID processId = env->GetFieldID(g_fdDataCls, "processId", "I");
	static jfieldID isModifyResponseBody = env->GetFieldID(g_fdDataCls, "isModifyResponseBody", "Z");
	static jfieldID responseCode = env->GetFieldID(g_fdDataCls, "responseCode", "I");
	static jfieldID nModifyFlags = env->GetFieldID(g_fdDataCls, "modifyFlags", "I");
	static jfieldID nNotifyType = env->GetFieldID(g_fdDataCls, "notifyType", "I");

	static jfieldID processName = env->GetFieldID(g_fdDataCls, "processName", "Ljava/lang/String;");
	static jfieldID url = env->GetFieldID(g_fdDataCls, "url", "Ljava/lang/String;");
	static jfieldID httpMethod = env->GetFieldID(g_fdDataCls, "httpMethod", "Ljava/lang/String;");
	static jfieldID httpVersion = env->GetFieldID(g_fdDataCls, "httpVersion", "Ljava/lang/String;");
	static jfieldID responseStatus = env->GetFieldID(g_fdDataCls, "responseStatus", "Ljava/lang/String;");
	static jfieldID header = env->GetFieldID(g_fdDataCls, "header", "Ljava/lang/String;");
	static jfieldID body = env->GetFieldID(g_fdDataCls, "body", "Ljava/lang/String;");

	jobject obj = NewFiddlerData(env);

	env->SetIntField(obj, id, data.nId);
	env->SetIntField(obj, eventType, data.eventType);
	env->SetIntField(obj, webSocketId, data.nWebSocketId);
	env->SetIntField(obj, processId, data.nProcessId);
	env->SetBooleanField(obj, isModifyResponseBody, data.bModifyResponseBody);
	env->SetIntField(obj, responseCode, data.nResponseCode);
	env->SetIntField(obj, nModifyFlags, data.nModifyFlags);
	env->SetIntField(obj, nNotifyType, data.nNotifyType);

	env->SetObjectField(obj, processName, charTojstring(env, data.lpszProcessName));
	env->SetObjectField(obj, url, charTojstring(env, data.lpszUrl));
	env->SetObjectField(obj, httpMethod, charTojstring(env, data.lpszHttpMethod));
	env->SetObjectField(obj, httpVersion, charTojstring(env, data.lpszHttpVersion));
	env->SetObjectField(obj, responseStatus, charTojstring(env, data.lpszResponseStatus));
	env->SetObjectField(obj, header, charTojstring(env, data.lpszHeader));
	env->SetObjectField(obj, body, charTojstring(env, data.lpszBody));
	return obj;
}

bool JNICallBack(FiddlerData& data,jmethodID methodId)
{

	JNIEnv *env;
	bool isAttach = false;
	if (g_jvm->GetEnv((void**)&env, JNI_VERSION) < 0)
	{
		isAttach = !isAttach;
		g_jvm->AttachCurrentThread((void**)&env, nullptr);
	}

	jobject obj = FiddlerData2JFiddlerData(env, data);
	bool  res = env->CallBooleanMethod(g_JNICallBack.callBackObj, methodId, obj);
	//env->DeleteLocalRef(obj);

	// detach
	if (isAttach)
	{
		g_jvm->DetachCurrentThread();
	}

	return res;	

}


bool JVM_BeforeRequest(FiddlerData& data)
{
	return g_JNICallBack.lpBeforeRequestMethodId ? JNICallBack(data, g_JNICallBack.lpBeforeRequestMethodId) : true;
}

bool JVM_BeforeResponse(FiddlerData& data)
{
	return g_JNICallBack.lpBeforeResponseMethodId ? JNICallBack(data, g_JNICallBack.lpBeforeResponseMethodId) : true;
}

bool JVM_AfterSessionComplete(FiddlerData& data)
{
	return g_JNICallBack.lpAfterSessionCompleteMethodId ? JNICallBack(data, g_JNICallBack.lpAfterSessionCompleteMethodId) : true;
}
 
bool JVM_OnWebSocketMessage(FiddlerData& data)
{
	return g_JNICallBack.lpOnWebSocketMessageMethodId ? JNICallBack(data, g_JNICallBack.lpOnWebSocketMessageMethodId) : true;
}



JNIEXPORT    jboolean JNICALL    setCallBack0(JNIEnv *env, jclass, jobject callBackObj)
{
	// 有回调
	if (callBackObj)
	{
		jclass callBackCls = env->GetObjectClass(callBackObj);
		jmethodID lpBeforeRequestMethodId = env->GetMethodID(callBackCls, "beforeRequest", "(Lcom/immortal/fiddler/core/FiddlerData;)Z");
		jmethodID lpBeforeResponseMethodId = env->GetMethodID(callBackCls, "beforeResponse", "(Lcom/immortal/fiddler/core/FiddlerData;)Z");
		jmethodID lpAfterSessionCompleteMethodId = env->GetMethodID(callBackCls, "afterSessionComplete", "(Lcom/immortal/fiddler/core/FiddlerData;)Z");
		jmethodID lpOnWebSocketMessageMethodId = env->GetMethodID(callBackCls, "onWebSocketMessage", "(Lcom/immortal/fiddler/core/FiddlerData;)Z");

		g_JNICallBack.lpBeforeRequestMethodId = lpBeforeRequestMethodId;
		g_JNICallBack.lpBeforeResponseMethodId = lpBeforeResponseMethodId;
		g_JNICallBack.lpAfterSessionCompleteMethodId = lpAfterSessionCompleteMethodId;
		g_JNICallBack.lpOnWebSocketMessageMethodId = lpOnWebSocketMessageMethodId;

		if (g_JNICallBack.callBackObj)
		{
			env->DeleteGlobalRef(g_JNICallBack.callBackObj);
		}
		g_JNICallBack.callBackObj = env->NewGlobalRef(callBackObj);
	}
	else
	{
		g_JNICallBack.lpBeforeRequestMethodId = nullptr;
		g_JNICallBack.lpBeforeResponseMethodId = nullptr;
		g_JNICallBack.lpAfterSessionCompleteMethodId = nullptr;
		g_JNICallBack.lpOnWebSocketMessageMethodId = nullptr;

		if (g_JNICallBack.callBackObj)
		{
			env->DeleteGlobalRef(g_JNICallBack.callBackObj);
		}
	}


	return true;
}

void JFiddlerData2FiddlerData(JNIEnv *env, jobject obj, FiddlerData  *data)
{
	static jfieldID id = env->GetFieldID(g_fdDataCls, "id", "I");
	static jfieldID eventType = env->GetFieldID(g_fdDataCls, "eventType", "I");
	static jfieldID webSocketId = env->GetFieldID(g_fdDataCls, "webSocketId", "I");
	static jfieldID processId = env->GetFieldID(g_fdDataCls, "processId", "I");
	static jfieldID isModifyResponseBody = env->GetFieldID(g_fdDataCls, "isModifyResponseBody", "Z");
	static jfieldID responseCode = env->GetFieldID(g_fdDataCls, "responseCode", "I");
	static jfieldID nModifyFlags = env->GetFieldID(g_fdDataCls, "modifyFlags", "I");
	static jfieldID nNotifyType = env->GetFieldID(g_fdDataCls, "notifyType", "I");

	static jfieldID processName = env->GetFieldID(g_fdDataCls, "processName", "Ljava/lang/String;");
	static jfieldID url = env->GetFieldID(g_fdDataCls, "url", "Ljava/lang/String;");
	static jfieldID httpMethod = env->GetFieldID(g_fdDataCls, "httpMethod", "Ljava/lang/String;");
	static jfieldID responseStatus = env->GetFieldID(g_fdDataCls, "responseStatus", "Ljava/lang/String;");
	static jfieldID header = env->GetFieldID(g_fdDataCls, "header", "Ljava/lang/String;");
	static jfieldID body = env->GetFieldID(g_fdDataCls, "body", "Ljava/lang/String;");
	static jfieldID httpVersion = env->GetFieldID(g_fdDataCls, "httpVersion", "Ljava/lang/String;");

	memset(data, 0, sizeof(FiddlerData));
	if (obj)
	{
		data->nId = env->GetIntField(obj, id);
		data->eventType =(EventType) env->GetIntField(obj, eventType);
		data->nWebSocketId = env->GetIntField(obj, webSocketId);
		data->nProcessId = env->GetIntField(obj, processId);
		data->bModifyResponseBody = env->GetIntField(obj, isModifyResponseBody);
		data->nResponseCode = env->GetIntField(obj, responseCode);
		data->nModifyFlags = env->GetIntField(obj, nModifyFlags);
		data->nNotifyType = env->GetIntField(obj, nNotifyType);

		data->lpszProcessName = jstringToChar(env, (jstring)env->GetObjectField(obj, processName));
		data->lpszUrl = jstringToChar(env, (jstring)env->GetObjectField(obj, url));
		data->lpszHttpMethod = jstringToChar(env, (jstring)env->GetObjectField(obj, httpMethod));
		data->lpszResponseStatus = jstringToChar(env, (jstring)env->GetObjectField(obj, responseStatus));
		data->lpszHeader = jstringToChar(env, (jstring)env->GetObjectField(obj, header));
		data->lpszBody = jstringToChar(env, (jstring)env->GetObjectField(obj, body));
		data->lpszHttpVersion = jstringToChar(env, (jstring)env->GetObjectField(obj, httpVersion));
	}

} 

JNIEXPORT   jboolean  JNICALL  modifyFiddlerData0(JNIEnv *env, jclass, jobject obj)
{
	FiddlerData  *data = new FiddlerData();
	JFiddlerData2FiddlerData(env, obj,data);
	bool res = ModifyFiddlerData(data);
	
	if (data->lpszProcessName) delete[] data->lpszProcessName;
	if (data->lpszUrl) delete[] data->lpszUrl;
	if (data->lpszHttpMethod) delete[] data->lpszHttpMethod;
	if (data->lpszResponseStatus) delete[] data->lpszResponseStatus;
	if (data->lpszHeader) delete[] data->lpszHeader; 
	if (data->lpszBody) delete[] data->lpszBody; 
	if (data->lpszHttpVersion) delete[] data->lpszHttpVersion;
	delete data;

	return  res;
}


JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void*   reserved) {

	JNIEnv * env = NULL;
	if (vm->GetEnv((void **)&env, JNI_VERSION) != JNI_OK) {
		WriteLog(WARN, "unload FiddlerJNI err");
	}

	if (g_fdDataCls)
	{
		env->DeleteGlobalRef(g_fdDataCls);
	}

	if (g_JNICallBack.callBackObj)
	{
		env->DeleteGlobalRef(g_JNICallBack.callBackObj);
	}

	OnExit();
	return  ;
}

 