#include "stdafx.h"

// General
#include "WinsockServer_RemoteClient.h"

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
	char buffer[DEFAULT_BUFLEN];
	int32_t bufferLenght = DEFAULT_BUFLEN;

	const char* rr = "Sping 127.0.0.1";
	Send(rr, strlen(rr) + 1);

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
			return 0;
		}

	} while (res > 0);
	return -1;
}

void WinsockServer_RemoteClient::Send(const char* data, int32_t size)
{
	int sizeOrError = send(m_Socket, data, size, 0);
	if (sizeOrError == SOCKET_ERROR)
	{
		Log::Error(L"send failed with error: [%d]", WSAGetLastError());
	}

	USES_CONVERSION;
	Log::Info(L"->[%s][%d]", A2W(data), sizeOrError);
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
	}
	break;

	// Info
	case L'I':
	{
		Log::Info(L"Client: [%s]", &command[1]);
	}
	break;

	// Message
	case L'M':
	{
		Log::Print(L"%s", &command[1]);
	}
	break;

	// Error
	case L'E':
	{
		Log::Warn(L"Client error [%s].", &command[1]);
	}
	break;

	default:
	{
		Log::Warn(L"Client: Unknown token [%c].", command[0]);
	}
	break;
	}
}
