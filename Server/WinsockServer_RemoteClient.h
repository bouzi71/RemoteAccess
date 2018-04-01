#pragma once

class WinsockServer_RemoteClient
{
public:
	WinsockServer_RemoteClient(SOCKET socket);
	~WinsockServer_RemoteClient();

public:
	DWORD Recv();
	void Send(const char* data, int32_t size);
	void ProcessCommand(BSTR command);
	void EnableCommand();

private:
	SOCKET m_Socket;
};