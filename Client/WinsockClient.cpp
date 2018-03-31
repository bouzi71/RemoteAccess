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
		Log::Error(L"WSAStartup failed with error: [%d]", res);
	}

	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	addrinfo* addrInfo = NULL;
	res = getaddrinfo("AWW", DEFAULT_PORT, &hints, &addrInfo);
	if (res != 0)
	{
		Log::Error(L"getaddrinfo failed with error: [%d]", res);
		WSACleanup();
	}

	for (addrinfo* ptr = addrInfo; ptr != NULL; ptr = ptr->ai_next)
	{
		m_Socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (m_Socket == INVALID_SOCKET)
		{
			Log::Error(L"[socket] failed with error: [%ld]", WSAGetLastError());
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
		Log::Error(L"Unable to connect to server!");
	}

	CComBSTR rr = L"Connected";
	Send(rr);

	Thread<WinsockClient> receiveThread(this, &WinsockClient::Recv);
	receiveThread.start();
	receiveThread.join();
}

WinsockClient::~WinsockClient()
{
	int res = shutdown(m_Socket, SD_SEND);
	if (res == SOCKET_ERROR)
	{
		Log::Error(L"[shutdown] failed with error: [%d]", WSAGetLastError());
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
		Log::Info(L"Wait for command...");
		res = recv(m_Socket, buffer, bufferLenght, 0);
		if (res > 0)
		{
			USES_CONVERSION;
			BSTR wBuffer = A2W(buffer);

			Log::Info(L"<- Received: [%s][%d]", wBuffer, res);

			switch (buffer[0])
			{
			case L'S':
			{
				BSTR ProcessName = &wBuffer[1];
				Log::Print(L"Start process [%s]", ProcessName);
				LocalProcess* proc = new LocalProcess(this, ProcessName);
			}
			break;

			default:
			{
				Log::Warn(L"Unknown token [%c]!", buffer[0]);
				return 0;
			}
			break;
			}
		}
		else
		{
			Log::Warn(L"Connection closed");
		}
	} while (res > 0);
	return -1;
}

void WinsockClient::Send(const char* data, ...)
{
	int res = send(m_Socket, data, strlen(data) + 1, 0);
	if (res == SOCKET_ERROR)
	{
		Log::Error(L"send failed with error: [%d]", WSAGetLastError());
	}

	USES_CONVERSION;
	Log::Info(L"-> [%s]", A2W(data));
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
			Log::Error(L"send failed with error: [%d]", WSAGetLastError());
		}

		Log::Info(L"-> [%s]", buff.c_str());
	}

	va_end(args);

	
}
