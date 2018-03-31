#include "stdafx.h"

// General
#include "WMIRoutine.h"

// Additional
#include "Common.h"
#include "Log.h"


WMIRoutine::WMIRoutine(CComBSTR host) :
	m_HostName(host),
	m_Domain(NULL),
	m_UserName(NULL),
	m_Password(NULL),
	m_IsLocal(true)
{
	CreateWbemLocatorObject();
	ConnectToWMIService();
}

WMIRoutine::WMIRoutine(CComBSTR host, CComBSTR domain, CComBSTR user, CComBSTR password) :
	m_HostName(host),
	m_Domain(domain),
	m_UserName(user),
	m_Password(password),
	m_IsLocal(false)
{
	CreateWbemLocatorObject();
	ConnectToWMIService();
}

WMIRoutine::~WMIRoutine()
{
	SecureZeroMemory(m_HostName, sizeof(m_HostName));
	SecureZeroMemory(m_Domain, sizeof(m_Domain));
	SecureZeroMemory(m_UserName, sizeof(m_UserName));
	SecureZeroMemory(m_Password, sizeof(m_Password));
}

//--

void WMIRoutine::CreateWbemLocatorObject()
{
	HRESULT hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&m_WbemLocator);
	if (FAILED(hr))
	{
		Log::Error(L"Failed to create IWbemLocator object. [%d].", hr);
	}

	Log::Info(L"IWbemLocator object created!");
}

void WMIRoutine::WMISetProxyBlanket(IUnknown* proxy)
{
	SEC_WINNT_AUTH_IDENTITY_W pAuthIdentity = { 0 };
	//if (!m_IsLocal)
	//{
		pAuthIdentity.User = (unsigned short*)(const wchar_t*)m_UserName;
		pAuthIdentity.UserLength = m_UserName.Length();
		pAuthIdentity.Domain = (unsigned short*)(const wchar_t*)m_Domain;
		pAuthIdentity.DomainLength = m_Domain.Length();
		pAuthIdentity.Password = (unsigned short*)(const wchar_t*)m_Password;
		pAuthIdentity.PasswordLength = m_Password.Length();
		pAuthIdentity.Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;
	//}

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
		Log::Error(L"Could not set proxy blanket. [%d]", hr);
	}
	//Log::Green(L"Proxy blanket setted! [%s, %s, %s]!", m_HostName, m_Domain, m_UserName);
}

void WMIRoutine::ConnectToWMIService()
{
	HRESULT hr = m_WbemLocator->ConnectServer(
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
		Log::Error(L"Could not connect to remote WMI services on [%s] host. [%d]", m_HostName, hr);
	}
	Log::Green(L"Connected to remote proxy [%s, %s]!", m_HostName, m_UserName);

	WMISetProxyBlanket(m_WbemServices);
}

WMIObject* WMIRoutine::GetWMIObject(const CComBSTR& wmi_query)
{
	CComPtr<IEnumWbemClassObject> enum_query_result;
	HRESULT hr = m_WbemServices->ExecQuery(
		BSTR(L"WQL"),
		wmi_query,
		WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY,
		NULL,
		&enum_query_result
	);
	if (FAILED(hr))
	{
		Log::Error(L"Could not execute WMI query:\n [%d]", wmi_query, hr);
	}

	WMISetProxyBlanket(enum_query_result);

	IWbemClassObject* obj[1];

	ULONG uReturned;
	hr = enum_query_result->Next(WBEM_INFINITE, 1, obj, &uReturned);
	if (FAILED(hr))
	{
		Log::Error(L"Could not get object from WMI query:\n $s \n [%s]", wmi_query, hr);
	}

	return new WMIObject(uReturned != 0 ? obj[0] : NULL);
}

CComBSTR WMIRoutine::WMIGetProcessID(const CComBSTR& process_name)
{
	SmartPtr<WMIObject> proc_object = GetWMIObject(CComBSTR(L"SELECT * FROM Win32_Process WHERE Caption=\"" + process_name + L"\""));

	if (proc_object == NULL)
	{
		return NULL;
	}

	return proc_object != NULL && proc_object->GetWMIObjectProperty(L"Handle");
}

bool WMIRoutine::WMIProcessExists(const CComBSTR& pid)
{
	CComBSTR query = L"SELECT * FROM Win32_Process WHERE Handle=" + pid;

	SmartPtr<WMIObject> process_object = GetWMIObject(query);

	bool exists = process_object->Exists();
	return exists;
}

void WMIRoutine::WMICreateProcess(const CComBSTR& command_line)
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
		Log::Error(L"Could not get Win32_Process WMI object. ", hr);
	}

	CComPtr<IWbemClassObject> pInParams;
	hr = proc_obj->GetMethod(L"Create", 0, &pInParams, NULL);
	if (FAILED(hr))
	{
		Log::Error(L"Could not get information about Win32_Process.Create method. ", hr);
	}

	CComVariant v = command_line;
	hr = pInParams->Put(L"CommandLine", 0, &v, VT_BSTR);
	if (FAILED(hr))
	{
		Log::Error(L"Unable to set 'CommandLine' parameter value of Win32_Process.Create method.", hr);
	}

	CComPtr<IWbemClassObject> ppOutParams;
	CComPtr<IWbemCallResult> ppCallResult;

	hr = m_WbemServices->ExecMethod(
		CComBSTR(L"Win32_Process"),
		CComBSTR(L"Create"),
		0,
		NULL,
		pInParams,
		&ppOutParams,
		&ppCallResult
	);
	if (FAILED(hr))
	{
		Log::Error(L"Unable to execute Win32_Process.Create method.", hr);
	}

	LONG plStatus;
	WMISetProxyBlanket(ppCallResult);
	hr = ppCallResult->GetCallStatus(WBEM_INFINITE, &plStatus);
	if (FAILED(hr))
	{
		Log::Error(L"Unable to get call status of Win32_Process.Create method.", hr);
	}

	if (plStatus != 0)
	{
		Log::Error(L"Call status of Win32_Process.Create method is incorrect: %d", plStatus);
	}

	CComVariant return_value;
	hr = ppOutParams->Get(CComBSTR(L"ReturnValue"), 0, &return_value, NULL, 0);
	if (FAILED(hr))
	{
		Log::Error(L"Unable to get ReturnValue of Win32_Process.Create method.", hr);
	}
	if (V_INT(&return_value) != 0)
	{
		Log::Error(L"Win32_Process.Create method returned error: %d", V_INT(&return_value));
	}

	hr = ppOutParams->Get(CComBSTR(L"ProcessId"), 0, &return_value, NULL, 0);
	if (FAILED(hr))
	{
		Log::Error(L"Unable to get ProcessId of Win32_Process.Create method.", hr);
	}

	CComBSTR pid = IntegerToString(V_INT(&return_value));

	Log::Info(L"Process with pid [%d] started.", pid);

	int i;
	bool process_exists;
	for (i = 0; i < 10; ++i)
	{
		process_exists = WMIProcessExists(pid);
		if (!process_exists)
		{
			Log::Info(L"Process [%s] finished.\n", command_line);
			break;
		}
		Sleep(1000);
		Log::Info(L"Waiting for [%s]...", command_line);
	}

	if (process_exists)
	{
		Log::Error(L"Process was not closed within 20 seconds: [%s]", command_line);
	}
}

void WMIRoutine::ExecuteCommand(const CComBSTR & command_line)
{
	WMICreateProcess(L"cmd.exe /c /u \"" + command_line + " > C:\\Temp\\log_%TIME:~0,2%_%TIME:~3,2%_%TIME:~6,2%.txt\"");
}
