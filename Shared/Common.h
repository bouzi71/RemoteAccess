#pragma once

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "500"//"27015"

#define bzero(a) memset(a, 0, sizeof(a))

CComBSTR operator+(const wchar_t* sp0, const CComBSTR& sp1);
BSTR IntegerToString(int number);