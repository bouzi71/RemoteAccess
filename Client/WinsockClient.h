#pragma once

class WinsockClient
{
public:
	WinsockClient();
	~WinsockClient();

public:
	DWORD Recv();
	void Send(const char* data, ...);
	void Send(CComBSTR data, ...);

private:
	SOCKET m_Socket;
};