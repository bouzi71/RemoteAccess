#pragma once

#include "WMIObject.h"

class WMIRoutine
{
public:
	WMIRoutine(CComBSTR host);
	WMIRoutine(CComBSTR host, CComBSTR domain, CComBSTR user, CComBSTR password);
	~WMIRoutine();

public:
	WMIObject* GetWMIObject(const CComBSTR& wmi_query);
	CComBSTR WMIGetProcessID(const CComBSTR& process_name);
	bool WMIProcessExists(const CComBSTR& pid);
	void WMICreateProcess(const CComBSTR& command_line);
	void ExecuteCommand(const CComBSTR& command_line);

private:
	void CreateWbemLocatorObject();
	void ConnectToWMIService();
	void WMISetProxyBlanket(IUnknown* proxy);

private:
	CComPtr<IWbemLocator>   m_WbemLocator;
	CComPtr<IWbemServices>  m_WbemServices;

private:
	CComBSTR        m_HostName;
	CComBSTR        m_UserName;
	CComBSTR        m_Domain;
	CComBSTR        m_Password;
};