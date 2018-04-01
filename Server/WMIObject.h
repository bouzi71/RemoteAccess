#pragma once

class WMIObject
{
public:
	WMIObject(IWbemClassObject* wmiObject);

public:
	bool Exists();
	CComBSTR Get(const CComBSTR& property_name);
	UINT GetUINT(const CComBSTR& property_nam);

private:
	CComPtr<IWbemClassObject> m_WMIObject;
};