#pragma once

#include "WMIObject.h"

class WMIRoutine
{
public:
	WMIRoutine(CComBSTR host, CComBSTR domain, CComBSTR user, CComBSTR password);
	~WMIRoutine();

public:
	WMIObject* GetWMIObject(const CComBSTR& wmiQuery);
	std::vector<WMIObject*> GetWMIQueryObject(const CComBSTR& wmiQuery);
	CComBSTR GetProcessID(const CComBSTR& processName);
	bool IsProcessExists(const CComBSTR& pid);
	void ExecuteProcess(const CComBSTR& commandLine);
	void ExecuteCommand(const CComBSTR& commandLine);

private:
	void SetProxy(IUnknown* proxy);
	CComBSTR ExecuteProcessInternal(const CComBSTR& commandLine);

private:
	CComPtr<IWbemLocator>   m_WbemLocator;
	CComPtr<IWbemServices>  m_WbemServices;

private:
	CComBSTR        m_HostName;
	CComBSTR        m_UserName;
	CComBSTR        m_Domain;
	CComBSTR        m_Password;
};