#pragma once

class WinsockClient
{
public:
	WinsockClient();
	~WinsockClient();

public:
	DWORD Recv();
	void Send(CComBSTR data, ...);
	void ProcessCommand(BSTR command);

private:
	SOCKET m_Socket;
};