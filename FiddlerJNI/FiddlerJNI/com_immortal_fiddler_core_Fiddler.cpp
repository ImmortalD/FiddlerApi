#include "com_immortal_fiddler_core_Fiddler.h"
#include "FiddlerApi.h"
#include "FiddlerJNI.h"

#ifdef _WIN32
#pragma comment(lib,"../lib/x86/FiddlerApi.lib") 
#endif // _WIN32

#ifdef _WIN64
#pragma comment(lib,"../lib/x64/FiddlerApi.lib") 
#endif // _WIN64


struct JNICallBackInfo
{
	jobject callBackObj;
	jmethodID callBackMethodId;
};


JNICallBackInfo g_JNICallBack = { NULL };
bool JNICallBack(FiddlerData& data);


jboolean JNICALL Java_com_immortal_fiddler_core_Fiddler_start0
(JNIEnv *, jclass)
{
	return Start();
}


jboolean JNICALL Java_com_immortal_fiddler_core_Fiddler_close0
(JNIEnv *, jclass)
{
	return Close();
}


jboolean JNICALL Java_com_immortal_fiddler_core_Fiddler_setCallBack0
(JNIEnv *env, jclass, jobject callBackObj)
{
	SetCallBack(JNICallBack);
	// 有回调
	if (callBackObj)
	{
		jclass callBackCls = env->GetObjectClass(callBackObj);
		jmethodID callBackMethodId = env->GetMethodID(callBackCls, "callBack", "(Lcom/immortal/fiddler/core/FiddlerData;)Z");

		g_JNICallBack.callBackMethodId = callBackMethodId;
	}

	if (g_JNICallBack.callBackObj)
	{
		env->DeleteGlobalRef(g_JNICallBack.callBackObj);
	}
	g_JNICallBack.callBackObj = env->NewGlobalRef(callBackObj);
	return true;
}


jboolean JNICALL Java_com_immortal_fiddler_core_Fiddler_createAndTrustRootCert0
(JNIEnv *, jclass)
{
	return CreateAndTrustRootCert();
}


jboolean JNICALL Java_com_immortal_fiddler_core_Fiddler_removeCert0
(JNIEnv *env, jclass, jboolean bRemoveRoot)
{
	return RemoveCert(bRemoveRoot);
}


JNIEXPORT void JNICALL Java_com_immortal_fiddler_core_Fiddler_writeLog0
(JNIEnv *env, jclass, jint level, jstring str)
{
	WriteLog((LogLevel)level, jstringToChar(env, str));
}

jobject JNICALL Java_com_immortal_fiddler_core_Fiddler_getFiddlerConfig0
(JNIEnv *env, jclass)
{
	jclass fdConfCls = env->FindClass("com/immortal/fiddler/core/FiddlerConfig");

	static jfieldID iListenPort = env->GetFieldID(fdConfCls, "iListenPort", "I");
	static jfieldID bRegisterAsSystemProxy = env->GetFieldID(fdConfCls, "bRegisterAsSystemProxy", "Z");
	static jfieldID bAlwaysModifyResponseBody = env->GetFieldID(fdConfCls, "bAlwaysModifyResponseBody", "Z");
	static jfieldID bDecryptSSL = env->GetFieldID(fdConfCls, "bDecryptSSL", "Z");
	static jfieldID bAllowRemote = env->GetFieldID(fdConfCls, "bAllowRemote", "Z");
	static jfieldID iSecureEndpointPort = env->GetFieldID(fdConfCls, "iSecureEndpointPort", "I");
	static jfieldID logLevel = env->GetFieldID(fdConfCls, "logLevel", "I");
	static jfieldID notifyType = env->GetFieldID(fdConfCls, "notifyType", "I");
	static jfieldID callBack = env->GetFieldID(fdConfCls, "callBack", "Lcom/immortal/fiddler/core/FiddlerCallBack;");
	static jfieldID szSecureEndpointHostname = env->GetFieldID(fdConfCls, "szSecureEndpointHostname", "Ljava/lang/String;");
	static jfieldID szLogFileName = env->GetFieldID(fdConfCls, "szLogFileName", "Ljava/lang/String;");
	static jfieldID szRegexProcessId = env->GetFieldID(fdConfCls, "szRegexProcessId", "Ljava/lang/String;");
	static jfieldID szRegexProcessName = env->GetFieldID(fdConfCls, "szRegexProcessName", "Ljava/lang/String;");

	// 创建对象
	jobject fdConfObj = env->NewObject(fdConfCls, env->GetMethodID(fdConfCls, "<init>", "()V"));

	FiddlerConfig FdConfig;
	GetFiddlerConfig(&FdConfig);
	env->SetIntField(fdConfObj, iListenPort, FdConfig.iListenPort);
	env->SetBooleanField(fdConfObj, bRegisterAsSystemProxy, FdConfig.bRegisterAsSystemProxy);
	env->SetBooleanField(fdConfObj, bAlwaysModifyResponseBody, FdConfig.bAlwaysModifyResponseBody);
	env->SetBooleanField(fdConfObj, bDecryptSSL, FdConfig.bDecryptSSL);
	env->SetBooleanField(fdConfObj, bAllowRemote, FdConfig.bAllowRemote);
	env->SetIntField(fdConfObj, iSecureEndpointPort, FdConfig.iSecureEndpointPort);
	env->SetIntField(fdConfObj, logLevel, FdConfig.logLevel);
	env->SetIntField(fdConfObj, notifyType, FdConfig.notifyType);
	env->SetObjectField(fdConfObj, callBack, g_JNICallBack.callBackObj);
	env->SetObjectField(fdConfObj, szSecureEndpointHostname, charTojstring(env, FdConfig.szSecureEndpointHostname));
	env->SetObjectField(fdConfObj, szLogFileName, charTojstring(env, FdConfig.szLogFileName));
	env->SetObjectField(fdConfObj, szRegexProcessId, charTojstring(env, FdConfig.szRegexProcessId));
	env->SetObjectField(fdConfObj, szRegexProcessName, charTojstring(env, FdConfig.szRegexProcessName));

	env->DeleteLocalRef(fdConfCls);
	return fdConfObj;
}


jboolean JNICALL Java_com_immortal_fiddler_core_Fiddler_setFiddlerConfig0
(JNIEnv *env, jclass, jobject confObj)
{
	jclass fdConfCls = env->GetObjectClass(confObj);
	static jfieldID iListenPort = env->GetFieldID(fdConfCls, "iListenPort", "I");
	static jfieldID bRegisterAsSystemProxy = env->GetFieldID(fdConfCls, "bRegisterAsSystemProxy", "Z");
	static jfieldID bAlwaysModifyResponseBody = env->GetFieldID(fdConfCls, "bAlwaysModifyResponseBody", "Z");
	static jfieldID bDecryptSSL = env->GetFieldID(fdConfCls, "bDecryptSSL", "Z");
	static jfieldID bAllowRemote = env->GetFieldID(fdConfCls, "bAllowRemote", "Z");
	static jfieldID iSecureEndpointPort = env->GetFieldID(fdConfCls, "iSecureEndpointPort", "I");
	static jfieldID logLevel = env->GetFieldID(fdConfCls, "logLevel", "I");
	static jfieldID notifyType = env->GetFieldID(fdConfCls, "notifyType", "I");
	static jfieldID callBack = env->GetFieldID(fdConfCls, "callBack", "Lcom/immortal/fiddler/core/FiddlerCallBack;");
	static jfieldID szSecureEndpointHostname = env->GetFieldID(fdConfCls, "szSecureEndpointHostname", "Ljava/lang/String;");
	static jfieldID szLogFileName = env->GetFieldID(fdConfCls, "szLogFileName", "Ljava/lang/String;");
	static jfieldID szRegexProcessId = env->GetFieldID(fdConfCls, "szRegexProcessId", "Ljava/lang/String;");
	static jfieldID szRegexProcessName = env->GetFieldID(fdConfCls, "szRegexProcessName", "Ljava/lang/String;");

	FiddlerConfig cf;
	cf.iListenPort = env->GetIntField(confObj, iListenPort);
	cf.bRegisterAsSystemProxy = env->GetBooleanField(confObj, bRegisterAsSystemProxy);
	cf.bAlwaysModifyResponseBody = env->GetBooleanField(confObj, bAlwaysModifyResponseBody);
	cf.bDecryptSSL = env->GetBooleanField(confObj, bDecryptSSL);
	cf.bAllowRemote = env->GetBooleanField(confObj, bAllowRemote);
	cf.iSecureEndpointPort = env->GetIntField(confObj, iSecureEndpointPort);
	cf.logLevel = (LogLevel)env->GetIntField(confObj, logLevel);
	cf.notifyType = (NotifyType)env->GetIntField(confObj, notifyType);

	jobject callBackObj = env->GetObjectField(confObj, callBack);
	Java_com_immortal_fiddler_core_Fiddler_setCallBack0(env, nullptr, callBackObj);

	cf.szSecureEndpointHostname = jstringToChar(env, (jstring)env->GetObjectField(confObj, szSecureEndpointHostname));
	cf.szLogFileName = jstringToChar(env, (jstring)env->GetObjectField(confObj, szLogFileName));
	cf.szRegexProcessId = jstringToChar(env, (jstring)env->GetObjectField(confObj, szRegexProcessId));
	cf.szRegexProcessName = jstringToChar(env, (jstring)env->GetObjectField(confObj, szRegexProcessName));

	SetFiddlerConfig(&cf);
	return true;
}


jboolean JNICALL Java_com_immortal_fiddler_core_Fiddler_exportRootCert0
(JNIEnv *env, jclass, jstring filePath)
{
	return ExportRootCert(jstringToChar(env, filePath));
}

void JFiddlerData2FiddlerData(JNIEnv *env, jobject obj, FiddlerData  *data)
{
	static jfieldID id = env->GetFieldID(g_fdDataCls, "id", "I");
	static jfieldID event = env->GetFieldID(g_fdDataCls, "event", "I");
	static jfieldID processId = env->GetFieldID(g_fdDataCls, "processId", "I");
	static jfieldID bModifyResponseBody = env->GetFieldID(g_fdDataCls, "bModifyResponseBody", "Z");
	static jfieldID modifyFlags = env->GetFieldID(g_fdDataCls, "modifyFlags", "I");
	static jfieldID notifyType = env->GetFieldID(g_fdDataCls, "notifyType", "I");
	static jfieldID url = env->GetFieldID(g_fdDataCls, "url", "Ljava/lang/String;");
	static jfieldID httpMethod = env->GetFieldID(g_fdDataCls, "httpMethod", "Ljava/lang/String;");
	static jfieldID httpVersion = env->GetFieldID(g_fdDataCls, "httpVersion", "Ljava/lang/String;");
	static jfieldID header = env->GetFieldID(g_fdDataCls, "header", "Ljava/lang/String;");
	static jfieldID body = env->GetFieldID(g_fdDataCls, "body", "Ljava/lang/String;");
	
	memset(data, 0, sizeof(FiddlerData));
	if (obj)
	{
		data->id = env->GetIntField(obj, id);
		data->event = env->GetIntField(obj, event);
		data->processId = env->GetIntField(obj, processId);
		data->bModifyResponseBody = env->GetIntField(obj, bModifyResponseBody);
		data->modifyFlags = (ModifyFlag)env->GetIntField(obj, modifyFlags);
		data->notifyType = (NotifyType)env->GetIntField(obj, notifyType);
		data->url = jstringToChar(env, (jstring)env->GetObjectField(obj, url));
		data->httpMethod = jstringToChar(env, (jstring)env->GetObjectField(obj, httpMethod));
		data->httpVersion = jstringToChar(env, (jstring)env->GetObjectField(obj, httpVersion));
		data->header = jstringToChar(env, (jstring)env->GetObjectField(obj, header));
		data->body = jstringToChar(env, (jstring)env->GetObjectField(obj, body));
	}

}

jobject FiddlerData2JFiddlerData(JNIEnv *env, const  FiddlerData& data)
{
	static jfieldID id = env->GetFieldID(g_fdDataCls, "id", "I");
	static jfieldID event = env->GetFieldID(g_fdDataCls, "event", "I");
	static jfieldID processId = env->GetFieldID(g_fdDataCls, "processId", "I");
	static jfieldID bModifyResponseBody = env->GetFieldID(g_fdDataCls, "bModifyResponseBody", "Z");
	static jfieldID modifyFlags = env->GetFieldID(g_fdDataCls, "modifyFlags", "I");
	static jfieldID notifyType = env->GetFieldID(g_fdDataCls, "notifyType", "I");
	static jfieldID url = env->GetFieldID(g_fdDataCls, "url", "Ljava/lang/String;");
	static jfieldID httpMethod = env->GetFieldID(g_fdDataCls, "httpMethod", "Ljava/lang/String;");
	static jfieldID httpVersion = env->GetFieldID(g_fdDataCls, "httpVersion", "Ljava/lang/String;");
	static jfieldID header = env->GetFieldID(g_fdDataCls, "header", "Ljava/lang/String;");
	static jfieldID body = env->GetFieldID(g_fdDataCls, "body", "Ljava/lang/String;");

	jobject obj = NewFiddlerData(env);

	env->SetIntField(obj, id, data.id);
	env->SetIntField(obj, event, data.event);
	env->SetIntField(obj, processId, data.processId);
	env->SetBooleanField(obj, bModifyResponseBody, data.bModifyResponseBody);
	env->SetIntField(obj, modifyFlags, data.modifyFlags);
	env->SetIntField(obj, notifyType, data.notifyType);
	env->SetObjectField(obj, url, charTojstring(env, data.url));
	env->SetObjectField(obj, httpMethod, charTojstring(env, data.httpMethod));
	env->SetObjectField(obj, httpVersion, charTojstring(env, data.httpVersion));
	env->SetObjectField(obj, header, charTojstring(env, data.header));
	env->SetObjectField(obj, body, charTojstring(env, data.body));
	return obj;
}

jboolean JNICALL Java_com_immortal_fiddler_core_Fiddler_modifyFiddlerData0
(JNIEnv *env, jclass, jobject obj)
{
	FiddlerData  data;
	JFiddlerData2FiddlerData(env, obj, &data);
	return ModifyFiddlerData(&data);
}

bool InvokJNICallBack(JNIEnv* env, jobject obj)
{
	if (g_JNICallBack.callBackObj)
	{
		return  env->CallBooleanMethod(g_JNICallBack.callBackObj, g_JNICallBack.callBackMethodId, obj);
	}
}

bool JNICallBack(FiddlerData& data)
{

	JNIEnv *env;
	bool isAttach = false;
	if (g_jvm->GetEnv((void**)&env, JNI_VERSION) < 0)
	{
		isAttach = !isAttach;
		g_jvm->AttachCurrentThread((void**)&env, nullptr);
	}

	jobject obj = FiddlerData2JFiddlerData(env, data);
	bool  res = InvokJNICallBack(env, obj);
	env->DeleteLocalRef(obj);

	// detach
	if (isAttach)
	{
		g_jvm->DetachCurrentThread();
	}

	return res;
}