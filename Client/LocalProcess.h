#pragma once

#include "WinsockClient.h"

class LocalProcess
{
public:
	LocalProcess(WinsockClient* client, CComBSTR name);
	~LocalProcess();

public:
	DWORD WaitExitCode();
	DWORD WaitMessages();

private:
	CComBSTR                 m_Name;
	PROCESS_INFORMATION      m_ProcessInfo;
	STARTUPINFO              m_StartupInfo;
	HANDLE                   m_HookStdOut, m_ReadStdOut;
	SmartPtr<WinsockClient>  m_Client;
};