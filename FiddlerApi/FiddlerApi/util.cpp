
#include "util.h"
#include  <malloc.h>
#include <string.h>
#include <regex>
using  namespace std;

#include "FiddlerApi.h"
extern void WriteLogEx(const int nLogLevel, const char* const format, ...);

void  SafeFree(void* ptr)
{
	if (ptr)
	{
		free(ptr);
		ptr = NULL;
	}

}

bool CopyData(char** dest, const char* source)
{
	if (!dest)
	{
		return false;
	}

	if (nullptr == source)
	{
		*dest = nullptr;
		return true;
	}

	int iLen = strlen(source) + 1;
	*dest = (char*)malloc(iLen);
	strcpy_s(*dest, iLen, source);
	return true;
}

bool CheckRegexIsValid(__in const char* const szRegex)
{
	try
	{
		regex reg(szRegex);
		return true;
	}
	catch (std::regex_error e)
	{
		WriteLogEx(ERR, "������ʽ����.�������:%d,����:%s", to_string(e.code()), e.what());
		return false;
	}

	return false;
}

bool RegexIsMatch(__in const char* const szRegex, __in const char* const szStr)
{
	regex reg(szRegex);
	string str = szStr;
	smatch match;
	if (regex_match(str, match, reg))
	{
		WriteLogEx(DEBUG, "ƥ��ɹ�.�ַ�����:[%s],regex:[%s],ƥ��������:[%s]", szStr, szRegex, match.str().c_str());
		return true;
	}
	else
	{
		WriteLogEx(DEBUG, "ƥ��ʧ��.�ַ�����:[%s],regex:[%s]", szStr, szRegex);
		return false;
	}

}