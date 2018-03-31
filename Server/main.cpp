#include "stdafx.h"

// Additional
#include <locale.h>
#include "Log.h"
#include "COMLoader.h"
#include "WMIRoutine.h"

int main()
{
	setlocale(LC_ALL, "Russian");
	Log::Init();

	//--

	COMLoader loader;
	//WMIRoutine routine("", "WORKGROUP", "", "");
	WMIRoutine routine("", "WORKGROUP", "", "");
	//WMIRoutine routine("");

	// GET Os
	SmartPtr<WMIObject> os = routine.GetWMIObject("Select * from Win32_OperatingSystem");
	
	// Get the value of the Name property
	CComBSTR name = os->GetWMIObjectProperty(L"Name");
	Log::Green(L"OS Name [%s]", name);

	// Get the value of the FreePhysicalMemory property
	CComBSTR memory = os->GetWMIObjectProperty(L"FreePhysicalMemory");
	Log::Green(L"OS Memory [%s]", memory);

	//routine.WMICreateProcess(L"cmd.exe /c \"ping 127.0.0.1\"");
	routine.WMICreateProcess(L"notepad.exe");

	//--

	system("pause");
    return 0;
}

