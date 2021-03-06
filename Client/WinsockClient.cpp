#include "stdafx.h"

// General
#include "WinsockClient.h"

// Additiona
#include "LocalProcess.h"

WinsockClient::WinsockClient()
{
	WSADATA wsaData;

	int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res != 0)
	{
		Log::Error(L"E[WSAStartup] failed with error: [%d]", res);
		return;
	}

	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	addrinfo* addrInfo = NULL;
	res = getaddrinfo(LOC_HOST, DEFAULT_PORT, &hints, &addrInfo);
	if (res != 0)
	{
		Log::Error(L"E[getaddrinfo] failed with error: [%d]", res);
		return;
	}

	for (addrinfo* ptr = addrInfo; ptr != NULL; ptr = ptr->ai_next)
	{
		m_Socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (m_Socket == INVALID_SOCKET)
		{
			Log::Error(L"E[socket] failed with error: [%ld]", WSAGetLastError());
			WSACleanup();
		}

		res = connect(m_Socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (res == SOCKET_ERROR)
		{
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(addrInfo);

	//--

	if (m_Socket == INVALID_SOCKET)
	{
		Log::Error(L"EUnable to connect to server!");
		return;
	}

	Send(L"IConnected");

	Thread<WinsockClient> receiveThread(this, &WinsockClient::Recv);
	receiveThread.start();
	receiveThread.join();
}

WinsockClient::~WinsockClient()
{
	int res = shutdown(m_Socket, SD_SEND);
	if (res == SOCKET_ERROR)
	{
		Log::Error(L"E[shutdown] failed with error: [%d]", WSAGetLastError());
	}

	closesocket(m_Socket);
	WSACleanup();
}

DWORD WinsockClient::Recv()
{
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
			Log::Error(L"EConnection closed");
		}
	} while (res > 0);

	return 0;
}


void WinsockClient::Send(CComBSTR data, ...)
{
	va_list args;
	va_start(args, data);

	int len = _vsnwprintf(NULL, 0, data, args);
	std::wstring buff;
	if (len > 0)
	{
		buff.resize(len + 1);
		_vsnwprintf(&buff[0], len + 1, data, args);

		USES_CONVERSION;
		const char* str = W2A(buff.c_str());

		int res = send(m_Socket, str, strlen(str) + 1, 0);
		if (res == SOCKET_ERROR)
		{
			Send(L"Esend failed with error: [%d]", WSAGetLastError());
		}
	}

	va_end(args);
}

void WinsockClient::ProcessCommand(BSTR command)
{
	switch (command[0])
	{
	// Start process
	case L'S':
	{
		new LocalProcess(this, &command[1]);
	}
	break;

	// Exit client
	case L'X':
	{
		int res = shutdown(m_Socket, SD_SEND);
		if (res == SOCKET_ERROR)
		{
			Log::Error(L"E[shutdown] failed with error: [%d]", WSAGetLastError());
		}
	}
	break;

	/*Error*/
	default:
	{
		Send(L"EUnknown token [%c]!", command[0]);
	}
	break;
	}
}
