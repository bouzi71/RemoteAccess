#pragma once

namespace StaticAssert
{
	template<bool>
	struct StaticAssertFailed;

	template<>
	struct StaticAssertFailed<true> {};
}
#define assert_static(exp) (StaticAssert::StaticAssertFailed<(exp) != 0>())

//

inline void FatalMessageBox(const wchar_t* _title, const wchar_t* _message, ...)
{
	va_list args;
	va_start(args, _message);

	int len = _vsnwprintf(NULL, 0, _message, args);
	wchar_t* buff = nullptr;
	if (len > 0)
	{
		buff = new wchar_t[len + 1];
		_vsnwprintf(&buff[0], len + 1, _message, args);
		MessageBox(HWND_DESKTOP, buff, _title, MB_ICONERROR | MB_OK);
		delete buff;
	}

	va_end(args);
}

//

#define fail1() \
{ \
	FatalMessageBox(L"Assertion failed!", L"File: [%s]\nLine: [%d]\nFunction: [%s]\n", __FILE__, __LINE__, __FUNCTION__); \
    abort();\
}

#define fail2(message) \
{ \
	FatalMessageBox(L"Assertion failed!", L"File: [%s]\nLine: [%d]\nFunction: [%s]\nDescription: [%s]\n", __FILE__, __LINE__, __FUNCTION__, message); \
    abort();\
}

#define assert1(expr) \
if(!(expr))\
{\
    FatalMessageBox(L"Assertion failed!", L"File: [%s]\nLine: [%d]\nFunction: [%s]\nExpression: [%s]\n", __FILE__, __LINE__, __FUNCTION__, #expr);\
    abort();\
}

#define assert2(expr, message) \
if(!(expr))\
{\
    FatalMessageBox(L"Assertion failed!", L"File: [%s]\nLine: [%d]\nFunction: [%s]\nExpression: [%s]\nDescription: [%s]\n", __FILE__, __LINE__, __FUNCTION__, #expr, message);\
    abort();\
}

#define assert3(expr, message, arg0) \
if(!(expr))\
{\
    FatalMessageBox(L"Assertion failed!", L"File: [%s]\nLine: [%d]\nFunction: [%s]\nExpression: [%s]\nDescription: [%s]\nArgument0: [%s]\n", __FILE__, __LINE__, __FUNCTION__, #expr, message, arg0);\
    abort();\
}

#define assert4(expr, message, arg0, arg1) \
if(!(expr))\
{\
    FatalMessageBox(L"Assertion failed!", L"File: [%s]\nLine: [%d]\nFunction: [%s]\nExpression: [%s]\nDescription: [%s]\nArgument0: [%s]\nArgument1: [%s]\n", __FILE__, __LINE__, __FUNCTION__, #expr, message, arg0, arg1);\
    abort();\
}

#define assert5(expr, message, arg0, arg1, arg2) \
if(!(expr))\
{\
    FatalMessageBox(L"Assertion failed!", L"File: [%s]\nLine: [%d]\nFunction: [%s]\nExpression: [%s]\nDescription: [%s]\nArgument0: [%s]\nArgument1: [%s]\nArgument2: [%s]\n", __FILE__, __LINE__, __FUNCTION__, #expr, message, arg0, arg1, arg2);\
    abort();\
}

#define assert6(expr, message, arg0, arg1, arg2, arg3) \
if(!(expr))\
{\
    FatalMessageBox(L"Assertion failed!", L"File: [%s]\nLine: [%d]\nFunction: [%s]\nExpression: [%s]\nDescription: [%s]\nArgument0: [%s]\nArgument1: [%s]\nArgument2: [%s]\nArgument3: [%s]\n", __FILE__, __LINE__, __FUNCTION__, #expr, message, arg0, arg1, arg2, arg3);\
    abort();\
}