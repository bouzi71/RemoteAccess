#pragma once

class DebugOutput
{
public:
	enum DebugMessageType
	{
		TYPE_INFO = 0,
		TYPE_PRINT,
		TYPE_GREEN,
		TYPE_WARNING,
		TYPE_ERROR
	};

public:
	virtual bool Init() = 0;
	virtual void Destroy() = 0;

	//

	void PushMessage(cwstring _message, DebugOutput::DebugMessageType _type);
	void PushMessage(DebugOutput::DebugMessageType _type, const wchar_t* _message, va_list& _vaList);

protected:
	virtual void Print(std::wstring _messageFmt, DebugOutput::DebugMessageType _type) = 0;
};