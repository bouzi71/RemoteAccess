#include "stdafx.h"

// General
#include "WinsockServer_RemoteClient.h"

// Additional
#include "WMIObject.h"
#include "WMIRoutine.h"

extern WMIRoutine* routine;

WinsockServer_RemoteClient::WinsockServer_RemoteClient(SOCKET socket) :
	m_Socket(socket)
{
	Thread<WinsockServer_RemoteClient> receiveThread(this, &WinsockServer_RemoteClient::Recv);
	receiveThread.start();
	receiveThread.join();
}

WinsockServer_RemoteClient::~WinsockServer_RemoteClient()
{
	closesocket(m_Socket);
}

DWORD WinsockServer_RemoteClient::Recv()
{
	EnableCommand();

	char buffer[DEFAULT_BUFLEN];
	int32_t bufferLenght = DEFAULT_BUFLEN;

	int res;
	do {
		res = recv(m_Socket, buffer, bufferLenght, 0);
		if (res > 0)
		{
			USES_CONVERSION;
			ProcessCommand(A2W(buffer));
		}
		else
		{
			Log::Warn(L"Connection closed");
		}

	} while (res > 0);

	return 0;
}

void WinsockServer_RemoteClient::Send(const char* data, int32_t size)
{
	int sizeOrError = send(m_Socket, data, size, 0);
	if (sizeOrError == SOCKET_ERROR)
	{
		Log::Error(L"[send] failed with error [%d].", WSAGetLastError());
	}
}

void WinsockServer_RemoteClient::ProcessCommand(BSTR command)
{
	switch (command[0])
	{
		// Process started
	case L'S':
	{
		Log::Green(L"%s", &command[1]);
	}
	break;

	// Process exited
	case L'X':
	{
		Log::Green(L"%s", &command[1]);
		EnableCommand();
	}
	break;

	// Info
	case L'I':
	{
		Log::Info(L"Client: [%s]", &command[1]);
	}
	break;

	// Error
	case L'E':
	{
		Log::Warn(L"Client error: [%s].", &command[1]);
		EnableCommand();
	}
	break;

	default:
	{
		Log::Print(L"%s", command);
	}
	break;
	}
}

void WinsockServer_RemoteClient::EnableCommand()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

	printf("Enter the command: ");

	std::string command;
	std::getline(std::cin, command);

	// Check exit
	if (command == "/exit")
	{
		Send("E", 2);
	}
	else if (command == "/procs")
	{
		Log::Green(L"Processes");
		std::vector<WMIObject*> processes = routine->GetWMIQueryObject(L"SELECT * FROM Win32_Process");
		for (auto it = processes.begin(); it != processes.end(); it++)
		{
			Log::Print(L"-- %s -- %d -- %d",
				(*it)->Get(L"Name"),
				(*it)->GetUINT(L"ProcessId"),
				(*it)->GetUINT(L"WorkingSetSize")
			);
		}
		EnableCommand();
	}
	else
	{
		command = 'S' + command;
		Send(command.c_str(), command.length() + 1);
	}
}
