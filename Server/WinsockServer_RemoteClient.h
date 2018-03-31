#pragma once

class WinsockServer_RemoteClient
{
public:
	WinsockServer_RemoteClient(SOCKET socket);
	~WinsockServer_RemoteClient();

public:
	DWORD Recv();
	void Send(const char* data, int32_t size);

private:
	SOCKET m_Socket;
};