#ifndef __FIDDLER_JNI__
#define __FIDDLER_JNI__


#include <jni.h>
#include <list>
#include <windows.h>
#include <iostream>
#define  JNI_VERSION JNI_VERSION_1_6

// java �� FiddlerData ��class
jclass g_fdDataCls;
// jvm
JavaVM* g_jvm;


// ����jvm��c++�����ĵı��벻һ���������Ҫת�롣 utf8/16ת����gb2312
char* jstringToChar(JNIEnv *env, jstring jstr) {

	if (!jstr)
	{
		return nullptr; //const_cast<char*>("");
	}
	int length = (env)->GetStringLength(jstr);

	if (length == 0)
	{
		return nullptr;
	}

	const jchar* jcstr = (env)->GetStringChars(jstr, 0);
	char* rtn = new char[length * 2 + 1];// (char*)malloc(length * 2 + 1);
	int size = 0;

	size = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)jcstr, length, rtn, (length * 2 + 1), NULL, NULL);
	if (size <= 0)
	{
		return nullptr;
	}

	(env)->ReleaseStringChars(jstr, jcstr);
	rtn[size] = 0;
	return rtn;
}

// ����jvm��c++�����ĵı��벻һ���������Ҫת�롣gb2312ת����utf8/16
jstring charTojstring(JNIEnv* env, const char* szStr) {
	if (!szStr)
	{
		return nullptr;
	}

	jstring rtn = 0;
	int slen = strlen(szStr);
	unsigned short * buffer = 0;

	if (slen == 0)
	{
		rtn = (env)->NewStringUTF(szStr);
	}
	else
	{
		int length = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)szStr, slen, NULL, 0);

		buffer = (unsigned short *)malloc(length * 2 + 1);
		if (MultiByteToWideChar(CP_ACP, 0, (LPCSTR)szStr, slen, (LPWSTR)buffer, length) > 0)
			rtn = (env)->NewString((jchar*)buffer, length);

		// wstring wstr =StringToWString(szStr);
		// rtn = (env)->NewString((jchar*)wstr.c_str(), wstr.length());

		// �ͷ��ڴ�
		free(buffer);
	}
	return rtn;
}


// new java FiddlerData����
jobject inline NewFiddlerData(JNIEnv* env)
{
	return env->NewObject(g_fdDataCls, env->GetMethodID(g_fdDataCls, "<init>", "()V"));
}
#endif