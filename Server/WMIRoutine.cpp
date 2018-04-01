#include "stdafx.h"

// General
#include "WMIRoutine.h"

WMIRoutine::WMIRoutine(CComBSTR host, CComBSTR domain, CComBSTR user, CComBSTR password) :
	m_HostName(host),
	m_Domain(domain),
	m_UserName(user),
	m_Password(password)
{
	HRESULT hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&m_WbemLocator);
	if (FAILED(hr))
	{
		Log::Error(L"Failed to create IWbemLocator object. [0x%x].", hr);
	}

	hr = m_WbemLocator->ConnectServer(
		BSTR(L"\\\\" + m_HostName + L"\\root\\cimv2"),
		m_Domain + L"\\" + m_UserName,
		(BSTR)m_Password,
		0,
		NULL,
		0,
		0,
		&m_WbemServices
	);
	if (FAILED(hr))
	{
		Log::Error(L"Could not connect to remote WMI services on [%s] host. [0x%x]", m_HostName, hr);
	}
	Log::Info(L"Connected to remote proxy [%s, %s]!", m_HostName, m_UserName);

	SetProxy(m_WbemServices);
}

WMIRoutine::~WMIRoutine()
{
	SecureZeroMemory(m_HostName, sizeof(m_HostName));
	SecureZeroMemory(m_Domain, sizeof(m_Domain));
	SecureZeroMemory(m_UserName, sizeof(m_UserName));
	SecureZeroMemory(m_Password, sizeof(m_Password));
}

//--

void WMIRoutine::SetProxy(IUnknown* proxy)
{
	SEC_WINNT_AUTH_IDENTITY_W pAuthIdentity = { 0 };
	pAuthIdentity.User = (unsigned short*)(const wchar_t*)m_UserName;
	pAuthIdentity.UserLength = m_UserName.Length();
	pAuthIdentity.Domain = (unsigned short*)(const wchar_t*)m_Domain;
	pAuthIdentity.DomainLength = m_Domain.Length();
	pAuthIdentity.Password = (unsigned short*)(const wchar_t*)m_Password;
	pAuthIdentity.PasswordLength = m_Password.Length();
	pAuthIdentity.Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;

	HRESULT hr = CoSetProxyBlanket(
		proxy,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		&pAuthIdentity,
		EOAC_NONE
	);
	if (FAILED(hr))
	{
		Log::Error(L"Could not set proxy. [0x%x]", hr);
	}
}

WMIObject* WMIRoutine::GetWMIObject(const CComBSTR& wmiQuery)
{
	CComPtr<IEnumWbemClassObject> result;
	HRESULT hr = m_WbemServices->ExecQuery(
		BSTR(L"WQL"),
		wmiQuery,
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&result
	);
	if (FAILED(hr))
	{
		Log::Error(L"Could not execute WMI query [%s] [0x%x]", wmiQuery, hr);
	}

	SetProxy(result);

	IWbemClassObject* obj[1];

	ULONG ret;
	hr = result->Next(WBEM_INFINITE, 1, obj, &ret);
	if (FAILED(hr))
	{
		Log::Error(L"Could not get object from WMI query: [$s] [0x%x]", wmiQuery, hr);
	}

	return new WMIObject(ret != 0 ? obj[0] : NULL);
}

std::vector<WMIObject*> WMIRoutine::GetWMIQueryObject(const CComBSTR& wmiQuery)
{
	CComPtr<IEnumWbemClassObject> result;
	HRESULT hr = m_WbemServices->ExecQuery(
		BSTR(L"WQL"),
		wmiQuery,
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&result
	);
	if (FAILED(hr))
	{
		Log::Error(L"Could not get object from WMI query: [$s] [0x%x]", wmiQuery, hr);
	}

	SetProxy(result);

	std::vector<WMIObject*> objects;

	while (result)
	{
		IWbemClassObject* pclsObj;
		ULONG ret = 0;
		hr = result->Next(WBEM_INFINITE, 1, &pclsObj, &ret);
		if (FAILED(hr) || ret == 0)
		{
			break;
		}

		objects.push_back(new WMIObject(pclsObj));
	}

	return objects;
}

CComBSTR WMIRoutine::GetProcessID(const CComBSTR& processName)
{
	SmartPtr<WMIObject> proc_object = GetWMIObject(CComBSTR(L"SELECT * FROM Win32_Process WHERE Caption=\"" + processName + L"\""));

	if (proc_object == NULL)
	{
		return NULL;
	}

	return proc_object != NULL && proc_object->Get(L"Handle");
}

bool WMIRoutine::IsProcessExists(const CComBSTR& pid)
{
	CComBSTR query = L"SELECT * FROM Win32_Process WHERE Handle=" + pid;

	SmartPtr<WMIObject> process_object = GetWMIObject(query);

	bool exists = process_object->Exists();
	return exists;
}

void WMIRoutine::ExecuteProcess(const CComBSTR& processName)
{
	CComBSTR pid = ExecuteProcessInternal(processName);

	bool process_exists;
	for (int i = 0; i < 25; i++)
	{
		process_exists = IsProcessExists(pid);
		if (process_exists)
		{
			Log::Info(L"Process [%s] started.", processName);
			break;
		}
		Sleep(1000);
	}
}

void WMIRoutine::ExecuteCommand(const CComBSTR& commandLine)
{
	//CComBSTR pid = ExecuteProcessInternal(L"cmd.exe /c /u \"" + commandLine + " > C:\\Temp\\log_%TIME:~0,2%_%TIME:~3,2%_%TIME:~6,2%.txt\"");
	CComBSTR pid = ExecuteProcessInternal(L"cmd.exe /c /u \"" + commandLine + "\"");

	bool process_exists;
	for (int i = 0; i < 25; i++)
	{
		process_exists = IsProcessExists(pid);
		if (!process_exists)
		{
			Log::Info(L"Command [%s] was finished.", commandLine);
			break;
		}
		Sleep(1000);
	}

	if (process_exists)
	{
		Log::Error(L"Command line [%s] was not closed in 25 sec.", commandLine);
	}
}

CComBSTR WMIRoutine::ExecuteProcessInternal(const CComBSTR& commandLine)
{
	CComPtr<IWbemClassObject> proc_obj;
	HRESULT hr = m_WbemServices->GetObject(
		CComBSTR(L"Win32_Process"),
		WBEM_FLAG_RETURN_WBEM_COMPLETE,
		NULL,
		&proc_obj,
		NULL
	);
	if (FAILED(hr))
	{
		Log::Error(L"[0x%x]", hr);
	}

	CComPtr<IWbemClassObject> pInParams;
	hr = proc_obj->GetMethod(L"Create", 0, &pInParams, NULL);
	if (FAILED(hr))
	{
		Log::Error(L"[0x%x]", hr);
	}

	CComVariant cmdLine = commandLine;
	hr = pInParams->Put(L"CommandLine", 0, &cmdLine, VT_BSTR);
	if (FAILED(hr))
	{
		Log::Error(L"[0x%x]", hr);
	}

	CComPtr<IWbemClassObject> outParams;
	CComPtr<IWbemCallResult> callResult;

	hr = m_WbemServices->ExecMethod(
		CComBSTR(L"Win32_Process"),
		CComBSTR(L"Create"),
		0,
		NULL,
		pInParams,
		&outParams,
		&callResult
	);
	if (FAILED(hr))
	{
		Log::Error(L"Unable to execute Win32_Process.Create method.", hr);
	}

	LONG status;
	SetProxy(callResult);
	hr = callResult->GetCallStatus(WBEM_INFINITE, &status);
	if (FAILED(hr))
	{
		Log::Error(L"[0x%x]", hr);
	}

	if (status != 0)
	{
		Log::Error(L"[0x%x]", status);
	}

	CComVariant return_value;
	hr = outParams->Get(CComBSTR(L"ReturnValue"), 0, &return_value, NULL, 0);
	if (FAILED(hr))
	{
		Log::Error(L"[0x%x]", hr);
	}

	if (V_INT(&return_value) != 0)
	{
		Log::Error(L"[0x%x]", V_INT(&return_value));
	}

	hr = outParams->Get(CComBSTR(L"ProcessId"), 0, &return_value, NULL, 0);
	if (FAILED(hr))
	{
		Log::Error(L"Unable to get ProcessId.", hr);
	}

	CComBSTR pid = IntegerToString(V_INT(&return_value));

	Log::Info(L"Process with pid [%s] was started.", pid);

	return pid;
}
