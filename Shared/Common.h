#pragma once

#define LOC_HOST "<this computer name>"

#define REM_HOST L"<remote computer name>"
#define REM_DOMAIN L"<workgroup or domain>"
#define REM_USER L"<user name>"
#define REM_PASSWORD L"<password>"
#define REM_FOLDERNAME L"Temp"
#define REM_CLIENTNAME L"Client.exe"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "500"

#define bzero(a) memset(a, 0, sizeof(a))

CComBSTR operator+(const wchar_t* sp0, const CComBSTR& sp1);
BSTR IntegerToString(int number);