#ifndef __UTIL_H__
#define __UTIL_H__
#include <regex>
using namespace std;

void SafeFree(void* ptr);
bool CopyData(char** dest, const char* source);
bool CheckRegexIsValid(const char* const szRegex);
bool RegexIsMatch(const char* const szRegex, const char* const szStr);
#endif // !__UTIL_H__

