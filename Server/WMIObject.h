#pragma once

class WMIObject
{
public:
	WMIObject(IWbemClassObject* wmiObject);

public:
	bool Exists();
	CComBSTR GetWMIObjectProperty(const CComBSTR& property_name);

private:
	CComPtr<IWbemClassObject> m_WMIObject;
};