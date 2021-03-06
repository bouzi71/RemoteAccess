#include "stdafx.h"

// General
#include "main.h"

// Additional
#include "COMLoader.h"
#include "WinsockServer.h"


int main()
{
	setlocale(LC_ALL, "Russian");
	Log::Init();

	// COM
	COMLoader loader;

	// WMI
	routine = new WMIRoutine(REM_HOST, REM_DOMAIN, REM_USER, REM_PASSWORD);

	// OS info
	SmartPtr<WMIObject> os = routine->GetWMIObject(L"SELECT * FROM Win32_OperatingSystem");
	CComBSTR name = os->Get(L"Name");
	Log::Green(L"OS Name: %s", name);
	CComBSTR memory = os->Get(L"FreePhysicalMemory");
	Log::Green(L"OS Memory: %s", memory);

	routine->ExecuteCommand(CComBSTR(L"md C:\\") + REM_FOLDERNAME + " \"");
	routine->ExecuteCommand(CComBSTR(L"net use \\\\") + REM_HOST + "\\" + REM_FOLDERNAME + " " + REM_PASSWORD + " /user:" + REM_USER);
	routine->ExecuteCommand(CComBSTR(L"del C:\\") + REM_FOLDERNAME + "\\" + REM_CLIENTNAME);
	routine->ExecuteCommand(CComBSTR(L"xcopy \\\\") + REM_HOST + "\\" + REM_FOLDERNAME + "\\" + REM_CLIENTNAME + " C:\\" + REM_FOLDERNAME + " /y");
	routine->ExecuteCommand(CComBSTR(L"net use \\\\") + REM_HOST + "\\" + REM_FOLDERNAME + " /delete");

	// Client
	routine->ExecuteProcess(CComBSTR(L"C:\\") + REM_FOLDERNAME + "\\Client.exe");

	// Server
	WinsockServer server;

	system("pause");
	return 0;
}

