#include "stdafx.h"

// General
#include "LocalProcess.h"


LocalProcess::LocalProcess(WinsockClient* client, CComBSTR name) :
	m_Name(name),
	m_Client(client)
{
	Init();
}

LocalProcess::~LocalProcess()
{
	CloseHandle(m_ProcessInfo.hThread);
	CloseHandle(m_ProcessInfo.hProcess);
	CloseHandle(m_HookStdOut);
	CloseHandle(m_ReadStdOut);
}

//--

void LocalProcess::Init()
{
	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, true, NULL, false);

	SECURITY_ATTRIBUTES sa;
	sa.lpSecurityDescriptor = &sd;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = true;

	if (!CreatePipe(&m_ReadStdOut, &m_HookStdOut, &sa, 0))
	{
		Log::Warn(L"CreatePipe error");
		return;
	}

	GetStartupInfo(&m_StartupInfo);
	m_StartupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	m_StartupInfo.wShowWindow = SW_HIDE;
	m_StartupInfo.hStdOutput = m_HookStdOut;
	m_StartupInfo.hStdError = m_HookStdOut;

	if (!CreateProcess(
		NULL,
		L"cmd.exe /c \"" + m_Name + "\"",
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&m_StartupInfo,
		&m_ProcessInfo)
		)
	{
		Log::Warn(L"CreateProcess error");
	}

	// Start thread
	Thread<LocalProcess> thread(this, &LocalProcess::WaitProcess);
	if (thread.start())
	{
		Log::Info(L"Process [%s] thread started", m_Name);
	}
}

DWORD LocalProcess::WaitProcess()
{
	char buffer[1024];
	bzero(buffer);

	while (true)
	{
		unsigned long exitCode = 0;
		GetExitCodeProcess(m_ProcessInfo.hProcess, &exitCode);
		if (exitCode != STILL_ACTIVE)
		{
			Log::Green(L"[%s] -> Exit code is [%d]", m_Name, exitCode);
			return exitCode;
		}

		DWORD readedBytes;
		DWORD avaliableBytes;
		PeekNamedPipe(m_ReadStdOut, buffer, 1023, &readedBytes, &avaliableBytes, NULL);
		if (readedBytes == 0)
		{
			continue;
		}
		
		if (avaliableBytes > 1023)
		{
			while (readedBytes >= 1023)
			{
				ReadFile(m_ReadStdOut, buffer, 1023, &readedBytes, NULL);
				m_Client->Send(L"[%s]", buffer);
				bzero(buffer);
			}
		}
		else
		{
			ReadFile(m_ReadStdOut, buffer, 1023, &readedBytes, NULL);
			m_Client->Send(L"[%s]", buffer);
			bzero(buffer);
		}
	}

	return -1;
}

void LocalProcess::PrintStdOutputs(char* str, DWORD& readedBytes)
{

}
