#pragma once

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "27015"

#define bzero(a) memset(a, 0, sizeof(a))

CComBSTR operator+(const wchar_t* sp0, const CComBSTR& sp1);
BSTR IntegerToString(int number);