#include "stdafx.h"

// General
#include "DebugOutput.h"

void DebugOutput::PushMessage(DebugOutput::DebugMessageType _type, const wchar_t* _message, va_list& _vaList)
{
	// Parse args end
	int len = _vsnwprintf(NULL, 0, _message, _vaList);
	std::wstring buff;
	if (len > 0)
	{
		buff.resize(len + 1);
		_vsnwprintf(&buff[0], len + 1, _message, _vaList);
		PushMessage(buff, _type);
	}
}

void DebugOutput::PushMessage(cwstring _message, DebugOutput::DebugMessageType _type)
{
	Print(_message, _type);
}