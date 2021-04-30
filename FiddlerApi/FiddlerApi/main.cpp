#pragma once
#include "util.h"
#include "FiddlerApi.h"
#include <windows.h>
#include <iostream>


#ifdef _FIDDLE_TEST_


bool TestCallBackBefore(FiddlerData& data)
{
	if (!strcmp("CONNECT", data.lpszHttpMethod))
	{
		return true;
	}
	data.bModifyResponseBody = true;

	ModifyFiddlerData(&data);

	return true;
}
bool TestCallBackBeforeRpone(FiddlerData& data)
{
	if (!strcmp("CONNECT", data.lpszHttpMethod))
	{
		return true;
	}
	string body = " <!--  c++ add  header --> " + string(data.lpszBody);
	data.lpszBody = (const_cast<char*>(body.c_str()));
	data.nModifyFlags = MODIFY_BODY | MODIFY_HEADERS;

	string h = "----------: ssss\r\n" + string(data.lpszHeader);
	data.lpszHeader = const_cast<char*>(h.c_str());
	ModifyFiddlerData(&data);
	return true;
}
bool TestCallBackCom(FiddlerData& data)
{
	return true;
}

bool OnWebSocketMessage(FiddlerData& data)
{
	string body = "Fiddler --- " + string(data.lpszBody);
	data.lpszBody = (const_cast<char*>(body.c_str()));
	data.bModifyResponseBody = true;
	data.nModifyFlags = MODIFY_BODY;
	ModifyFiddlerData(&data);
	return true;
}


#define _CRTDBG_MAP_ALLOC
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
int main()
{
	cout << sizeof(int) << "  " << sizeof(long long) << "  " << sizeof(double) << endl;
	InitConfig(LANGUAGE_C_PULS_PLUS);

	CreateAndTrustRootCert();
	FiddlerConfig fcng;
	GetFiddlerConfig(&fcng);
	fcng.nNotifyType = NOTIFY_ALL;
	fcng.lpCallBack->lpCallBackBeforeRequest = TestCallBackBefore;
	fcng.lpCallBack->lpCallBackBeforeResponse = TestCallBackBeforeRpone;
	fcng.lpCallBack->lpCallBackAfterSessionComplete = TestCallBackCom;
	fcng.lpCallBack->lpCallBackOnWebSocketMessage = OnWebSocketMessage;
	fcng.nLogLevel = DEBUG;
	fcng.nLanguage = LANGUAGE_C_PULS_PLUS;

	SetFiddlerConfig(&fcng);

	Start();

	Sleep(1000 * 200 * 5);
	Close();
	cout << "endl----------" << endl;
	system("pause");
	_CrtDumpMemoryLeaks();
	return 0;
}

// Dumping objects
#endif // _FIDDLE_TEST_
