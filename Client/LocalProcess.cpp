#include "stdafx.h"

// General
#include "LocalProcess.h"

LocalProcess::LocalProcess(WinsockClient* client, CComBSTR name) :
	m_Name(name),
	m_Client(client)
{
	m_Client->Send(L"SStarting [%s] process.", m_Name);

	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, true, NULL, false);

	SECURITY_ATTRIBUTES sa;
	sa.lpSecurityDescriptor = &sd;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = true;

	if (!CreatePipe(&m_ReadStdOut, &m_HookStdOut, &sa, 0))
	{
		m_Client->Send(L"ECreatePipe error.");
		return;
	}

	GetStartupInfo(&m_StartupInfo);
	m_StartupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	m_StartupInfo.wShowWindow = SW_HIDE;
	m_StartupInfo.hStdOutput = m_HookStdOut;
	m_StartupInfo.hStdError = m_HookStdOut;

	if (!CreateProcess(
		NULL,
		m_Name,
		NULL,
		NULL,
		TRUE,
		NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
		NULL,
		NULL,
		&m_StartupInfo,
		&m_ProcessInfo)
		)
	{
		m_Client->Send(L"ECreateProcess error.");
		return;
	}

	m_Client->Send(L"SThe process [%s] was started.", m_Name);
	Sleep(100);
	Thread<LocalProcess> threadWaiterExiteCode(this, &LocalProcess::WaitExitCode);
	threadWaiterExiteCode.start();
	Sleep(100);
}

LocalProcess::~LocalProcess()
{
	CloseHandle(m_ProcessInfo.hThread);
	CloseHandle(m_ProcessInfo.hProcess);
	CloseHandle(m_HookStdOut);
	CloseHandle(m_ReadStdOut);
}

DWORD LocalProcess::WaitExitCode()
{
	char buffer[DEFAULT_BUFLEN];
	bzero(buffer);

	while (true)
	{
		unsigned long exitCode = 0;
		GetExitCodeProcess(m_ProcessInfo.hProcess, &exitCode);
		Sleep(50);
		if (exitCode != STILL_ACTIVE)
		{
			m_Client->Send(L"XThe process [%s] was exited with code [%d].", m_Name, exitCode);
			return exitCode;
		}

		DWORD readedBytes;
		DWORD avaliableBytes;
		PeekNamedPipe(m_ReadStdOut, buffer, DEFAULT_BUFLEN - 1, &readedBytes, &avaliableBytes, NULL);
		if (readedBytes == 0)
		{
			continue;
		}
		
		if (avaliableBytes > DEFAULT_BUFLEN - 1)
		{
			while (readedBytes >= DEFAULT_BUFLEN - 1)
			{
				ReadFile(m_ReadStdOut, buffer, DEFAULT_BUFLEN - 1, &readedBytes, NULL);
				USES_CONVERSION;
				m_Client->Send(L"%s", A2W(buffer));
				bzero(buffer);
			}
		}
		else
		{
			ReadFile(m_ReadStdOut, buffer, DEFAULT_BUFLEN - 1, &readedBytes, NULL);
			USES_CONVERSION;
			m_Client->Send(L"%s", A2W(buffer));
			bzero(buffer);
		}
	}

	return -1;
}
