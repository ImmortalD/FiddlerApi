#ifndef __FILLDER_API__H__
#define __FILLDER_API__H__

struct FiddlerData;

// ��Ϣ�ص�������
typedef bool(*lpCallBack)(FiddlerData& data);

/*
typedef struct HttpHeader
{
	char* name;
	char* value;
}*pHttpHeader;
*/

// ֪ͨ����

	// ��֪ͨ�κ�����
#define	NOTIFY_NO_ALL  0
	// ����֮ǰ֪ͨ
#define NOTIFY_BEFORE_SEND  1
	// ��Ӧ֮ǰ֪ͨ
#define NOTIFY_BEFORE_RESPONSE  2
	// ��Ӧ���֪ͨ
#define	NOTIFY_COMPOLE_RESPONSE  4
	// ֪ͨ��������
#define	NOTIFY_ALL  NOTIFY_BEFORE_SEND | NOTIFY_BEFORE_RESPONSE | NOTIFY_COMPOLE_RESPONSE


// FiddlerData modifyFlags values
	// ���޸�����
#define	MODIFY_NO_ALL  0
	// �޸�Url
#define MODIFY_URL  1
	// �޸�http body
#define MODIFY_BODY  2
	// �޸�http ͷ
#define MODIFY_HEADERS  4
	// �޸�http method
#define MODIFY_HTTPMETHOD  8
	// �޸�httpversion
#define MODIFY_HTTPVERSION  16
	// �޸�֪ͨ
#define MODIFY_NOTIFY  32
	// �޸���Ӧ״̬
#define MODIFY_REPONSE_STATUS  64
	// �޸���Ӧcode
#define MODIFY_REPONSE_CODE  128
	// �޸�����
#define MODIFY_ALL  MODIFY_URL | MODIFY_BODY | MODIFY_HEADERS | MODIFY_HTTPMETHOD | MODIFY_HTTPVERSION \
	| MODIFY_NOTIFY | MODIFY_REPONSE_STATUS | MODIFY_REPONSE_CODE


	// ֻ��ӡERR������־ 
#define	ERR 1
	// ��ӡWARN��ERR������־
#define WARN 3
	// ��ӡINFO WARN ERR ������־
#define INFO 7
	// ��ӡDEBUIG NFO WARN ERR ������־
#define DEBUG 15


// FiddlerData event values
enum EventType
{
	// ���ݷ��ͽ׶�,���޸���������
	SEND = 0,
	// ���ݽ��ս׶�,���޸���Ӧ����
	RECV,
	// ���ݽ��ս׶�,���ݲ����޸�
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
	// ÿһ��http�����Ψһ��ʶ,����Ϣ�ص��в�Ҫ�޸�
	int nId;
	// �ص���Ϣ���� 0��ʾ���ݷ��ͽ׶�,1���ݽ��ս׶�,2��ʾ���ݴ�����ɽ׶�.����׶ε����ݲ������޸���.ȡֵ��Ӧ��EVENT��ö��ֵ
	EventType eventType;
	// �����websockt��Ϣ����ֵ,������-1
	int nWebSocketId;
	// Http����Ľ���ID,�ڻص��в�Ҫ�޸�
	int nProcessId;
	// �Ƿ�Ҫ�޸�Http��Ӧbody,�����Ҫ�޸���Ҫ�ڻص�����event=SEND(0)��ʱ���bModifyResponseBody�޸�Ϊtrue
	bool bModifyResponseBody;
	// ��Ӧcode
	int nResponseCode;
	// �޸ĵ����ݱ�־ȡֵ��Modify_XX�ĺ�,��Ҫ�޸Ķ����ֵ��ȡֵ������`MODIFY_BODY|MODIFY_HEADERS`������
	int nModifyFlags;
	// ֪ͨ����
	int nNotifyType;
	// ������
	char * lpszProcessName;
	// http�����url
	char* lpszUrl;
	// http����ʽ,GET POST CONNECT��
	char* lpszHttpMethod;
	// httpЭ��汾,�� HTTP/1.1
	char* lpszHttpVersion;
	// http��Ӧ״̬
	char* lpszResponseStatus;
	// http ��������
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
	// ʹ�õı������
	int nLanguage;
	// Fiddler����˿�
	int nListenPort;
	// ��������,ȡֵ�� FC_��ͷ�ĳ���
	int nConfigEx;
	// https��������ʹ��
	int nSecureEndpointPort;
	// ��־����
	int nLogLevel;
	// ֪ͨ����
	int nNotifyType;
	// �ص�
	lpFiddlerCallBack lpCallBack;
	// szSecureEndpointHostname
	char *lpszSecureEndpointHostname;
	// ��־�ļ�·��
	char* lpszLogFileName;
	// ����PID����������ʽ,�ͽ���������������ʽֻҪ��һ��ƥ���Ͼͱ�ʾƥ����
	char* lpszRegexProcessId;
	// ����������������ʽ
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

	// ����Fiddler
	__DECLSPEC bool __FIDDLER_CALL Start();
	// �ر�Fiddler
	__DECLSPEC bool __FIDDLER_CALL Close();
	// ��ȡ����
	__DECLSPEC bool __FIDDLER_CALL GetFiddlerConfig(__inout FiddlerConfig* const);
	// ����Fiddler����, ע��,���Ҫ�� _Start()֮ǰ���ò���Ч
	__DECLSPEC bool __FIDDLER_CALL SetFiddlerConfig(__in const FiddlerConfig* const);
	// ��ӻص�����
	__DECLSPEC bool __FIDDLER_CALL SetCallBack(
		__in lpCallBack lpCallBackBeforeRequest, __in lpCallBack lpCallBackBeforeResponse,
		__in lpCallBack lpCallBackAfterSessionComplete, __in lpCallBack lpCallBackOnWebSocketMessage
	);

	// �޸�Fiddler����
	__DECLSPEC bool __FIDDLER_CALL ModifyFiddlerData(__in const FiddlerData* const);
	// ����Fiddler��֤��
	__DECLSPEC bool __FIDDLER_CALL ExportRootCert(__in const char* const lpszFileName);
	// ����������Fiddler��֤��,https��Ҫ
	__DECLSPEC bool __FIDDLER_CALL CreateAndTrustRootCert();
	// ɾ��fiddler���ɵ�֤��
	__DECLSPEC bool __FIDDLER_CALL RemoveCert(__in const bool bRemoveRoot);
	// �ͷ�FidderConfig
	__DECLSPEC void __FIDDLER_CALL FreeFiddlerConfig(__in FiddlerConfig* data);
	// �ͷ�FreeFiddlerData
	__DECLSPEC void __FIDDLER_CALL FreeFiddlerData(__in FiddlerData* data);
	//
	__DECLSPEC void __FIDDLER_CALL WriteLog(const int nLogLevel, const char* const lpszLog);
	// ��ʼ��Ĭ��Fiddler������Ϣ
	__DECLSPEC bool __FIDDLER_CALL   InitConfig(int nLangueue);

	__DECLSPEC void __FIDDLER_CALL OnExit();

	__DECLSPEC bool __FIDDLER_CALL GetDotNetVersion(pVersion pVer);
	__DECLSPEC void  __FIDDLER_CALL FreeVersion(pVersion ver);

#ifdef __cplusplus
}
#endif // __cplusplus

#define __FILLDER_API__H__
#endif // __FILLDER_API__H__

