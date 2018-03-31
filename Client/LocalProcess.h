#pragma once

#include "WinsockClient.h"

class LocalProcess
{
public:
	LocalProcess(WinsockClient* client, CComBSTR name);
	~LocalProcess();

public:
	void Init();
	DWORD WaitProcess();

private:
	void PrintStdOutputs(char* str, DWORD& readedBytes);

private:
	CComBSTR                 m_Name;
	PROCESS_INFORMATION      m_ProcessInfo;
	STARTUPINFO              m_StartupInfo;
	HANDLE                   m_HookStdOut, m_ReadStdOut;
	SmartPtr<WinsockClient>  m_Client;
};