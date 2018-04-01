#include "stdafx.h"

// General
#include "WinsockServer.h"

WinsockServer::WinsockServer()
{
	WSADATA wsaData;
	int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res != 0)
	{
		Log::Error(L"[WSAStartup] failed with error: [%d]", res);
		return;
	}

	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	res = getaddrinfo(NULL, DEFAULT_PORT, &hints, &m_AddrInfo);
	if (res != 0)
	{
		Log::Error(L"[getaddrinfo] failed with error: [%d]", res);
		return;
	}

	m_Socket = socket(m_AddrInfo->ai_family, m_AddrInfo->ai_socktype, m_AddrInfo->ai_protocol);
	if (m_Socket == INVALID_SOCKET)
	{
		Log::Error(L"[socket] failed with error: [%ld]", WSAGetLastError());
		return;
	}

	res = bind(m_Socket, m_AddrInfo->ai_addr, (int)m_AddrInfo->ai_addrlen);
	if (res == SOCKET_ERROR)
	{
		Log::Error(L"[bind] failed with error: [%d]", WSAGetLastError());
		return;
	}

	freeaddrinfo(m_AddrInfo);

	if (listen(m_Socket, SOMAXCONN) == SOCKET_ERROR)
	{
		Log::Error(L"[listen] failed with error: [%d]", WSAGetLastError());
	}

	Log::Warn(L"Waiting for client connection..."); /*gt*/
	SOCKET ClientSocket = accept(m_Socket, NULL, NULL);
	if (ClientSocket != INVALID_SOCKET)
	{
		Log::Warn(L"The client is connected."); /*gt*/

		m_Client = new WinsockServer_RemoteClient(ClientSocket);
	}
}

WinsockServer::~WinsockServer()
{
	freeaddrinfo(m_AddrInfo);
	closesocket(m_Socket);
	WSACleanup();
}

