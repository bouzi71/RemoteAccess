#pragma once

class DebugOutput;

class Log
{
public:
	static bool Init();
	static void Destroy();

	static bool AddDebugOutput(DebugOutput* _debugOutput);
	static bool DeleteDebugOutput(DebugOutput* _debugOutput);

	static void Info(const wchar_t* _message, ...);
	static void Print(const wchar_t* _message, ...);
	static void Green(const wchar_t* _message, ...);
	static void Warn(const wchar_t* _message, ...);
	static void Error(const wchar_t* _message, ...);

	static void Fatal(const wchar_t* _message, ...);

private:
	static void PushMessageToAllDebugOutputs(const wchar_t* _message, int _type, va_list& _vaList);

private:
	static std::vector<DebugOutput*> debugOutputs;
};