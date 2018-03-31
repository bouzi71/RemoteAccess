#pragma once

#include "WinsockServer_RemoteClient.h"

class WinsockServer
{
public:
	WinsockServer();
	~WinsockServer();

private:
	addrinfo* m_AddrInfo;
	SOCKET m_Socket;
	WinsockServer_RemoteClient* m_Client;
};