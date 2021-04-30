// dllmain.cpp : 定义 DLL 应用程序的入口点。


#ifndef _FIDDLE_TEST_

#include <windows.h>
#include "FiddlerApi.h"

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{

	case DLL_PROCESS_ATTACH:
		// InitConfig();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE; 
}
#endif // !_FIDDLE_TEST_