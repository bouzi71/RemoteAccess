#include "stdafx.h"

// Include
#include "DebugOutput.h"

// General
#include "Log.h"

// Additional
#include "DebugOutput_ConsoleWindows.h"

std::vector<DebugOutput*> Log::debugOutputs;

bool Log::Init()
{
	AddDebugOutput(new DebugOutput_ConsoleWindows());

	return true;
}

void Log::Destroy()
{
	debugOutputs.clear();
}

// Log outputs functional

bool Log::AddDebugOutput(DebugOutput* _debugOutput)
{
	assert1(_debugOutput != nullptr);

	if (find(debugOutputs.begin(), debugOutputs.end(), _debugOutput) != debugOutputs.end())
	{
		return false;
	}

	if (!_debugOutput->Init())
	{
		Log::Error(L"Debug[]: Can't init debug output.");
		return false;
	}

	debugOutputs.push_back(_debugOutput);

	return true;
}

bool Log::DeleteDebugOutput(DebugOutput* _debugOutput)
{
	assert1(_debugOutput != nullptr);

	auto _debugOutputsIt = find(debugOutputs.begin(), debugOutputs.end(), _debugOutput);

	// Not exists
	if (_debugOutputsIt == debugOutputs.end())
	{
		return false;
	}

	debugOutputs.erase(_debugOutputsIt);

	return true;
}

// Logs

void Log::Info(const wchar_t* _message, ...)
{
	va_list args;
	va_start(args, _message);

	PushMessageToAllDebugOutputs(_message, DebugOutput::DebugMessageType::TYPE_INFO, args);

	va_end(args);
}

void Log::Print(const wchar_t* _message, ...)
{
	va_list args;
	va_start(args, _message);

	PushMessageToAllDebugOutputs(_message, DebugOutput::DebugMessageType::TYPE_PRINT, args);

	va_end(args);
}

void Log::Green(const wchar_t* _message, ...)
{
	va_list args;
	va_start(args, _message);

	PushMessageToAllDebugOutputs(_message, DebugOutput::DebugMessageType::TYPE_GREEN, args);

	va_end(args);
}

void Log::Warn(const wchar_t* _message, ...)
{
	va_list args;
	va_start(args, _message);

	PushMessageToAllDebugOutputs(_message, DebugOutput::DebugMessageType::TYPE_WARNING, args);

	va_end(args);
}

void Log::Error(const wchar_t* _message, ...)
{
	va_list args;
	va_start(args, _message);

	PushMessageToAllDebugOutputs(_message, DebugOutput::DebugMessageType::TYPE_ERROR, args);

	va_end(args);
}

// Fatal & exit

void Log::Fatal(const wchar_t* _message, ...)
{
	FatalMessageBox(_message, L"Fatal");
	exit(-1);
}

//

void Log::PushMessageToAllDebugOutputs(const wchar_t* _message, int _type, va_list& _vaList)
{
	if (debugOutputs.empty())
	{
		return;
	}

	for (auto it : debugOutputs)
	{
		it->PushMessage(static_cast<DebugOutput::DebugMessageType>(_type), _message, _vaList);
	}
}