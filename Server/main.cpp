#include "stdafx.h"

// Additional
#include "COMLoader.h"
#include "WinsockServer.h"
#include "WMIRoutine.h"

int main()
{
	setlocale(LC_ALL, "Russian");
	Log::Init();

	//--

	/*COMLoader loader;
	WMIRoutine routine("AWW", "WORKGROUP", "ky397", "AKLakl54341");

	// GET Os
	SmartPtr<WMIObject> os = routine.GetWMIObject("Select * from Win32_OperatingSystem");
	
	// Get the value of the Name property
	CComBSTR name = os->GetWMIObjectProperty(L"Name");
	Log::Green(L"OS Name [%s]", name);

	// Get the value of the FreePhysicalMemory property
	CComBSTR memory = os->GetWMIObjectProperty(L"FreePhysicalMemory");
	Log::Green(L"OS Memory [%s]", memory);*/
	
	//routine.ExecuteCommand(L"C:\\Temp\\send.bat");
	
	//routine.ExecuteCommand(L"ping 127.0.0.1");
	//routine.ExecuteCommand(L"regedit.exe");
	
	//routine.ExecuteCommand(L"md C:\\Temp \"");
	//routine.ExecuteCommand(L"net use \\\\AWW\\Temp  /user:ky397");
	//routine.ExecuteCommand(L"del C:\\Temp\\new.txt");
	//routine.ExecuteCommand(L"xcopy \\\\AWW\\Temp\\new.txt C:\\Temp /y");
	//routine.ExecuteCommand(L"net use \\\\AWW\\Temp /delete");
	

	//--

	WinsockServer server;
	/*const char* rr = "Sping 127.0.0.1";
	Send(rr, strlen(rr) + 1);*/

	//--

	system("pause");
    return 0;
}

