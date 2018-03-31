#include "stdafx.h"

// General
#include "Common.h"

CComBSTR operator+(const wchar_t* sp0, const CComBSTR & sp1)
{
	size_t lengthA = wcslen(sp0);
	UINT lengthB = SysStringLen(sp1);

	BSTR result = SysAllocStringLen(NULL, lengthA + lengthB);

	memcpy(result, sp0, lengthA * sizeof(OLECHAR));
	memcpy(result + lengthA, sp1, lengthB * sizeof(OLECHAR));

	result[lengthA + lengthB] = 0;
	return CComBSTR(result);
}

BSTR IntegerToString(int number)
{
	wchar_t temp_str[11];
	_itow(number, temp_str, 10);
	return SysAllocString(temp_str);
}