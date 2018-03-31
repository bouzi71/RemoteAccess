#include "stdafx.h"

// General
#include "WMIObject.h"

// Additional
#include "Log.h"

WMIObject::WMIObject(IWbemClassObject* wmiObject):
	m_WMIObject(wmiObject)
{}

bool WMIObject::Exists()
{
	return m_WMIObject != NULL;
}

CComBSTR WMIObject::GetWMIObjectProperty(const CComBSTR& property_name)
{
	CComVariant prop_value;
	HRESULT hr = m_WMIObject->Get((wchar_t*)property_name, 0, &prop_value, 0, 0);
	if (FAILED(hr))
	{
		Log::Error(L"Could not get ['%s'] property of WMI object. ", property_name, hr);
	}

	CComBSTR result = CComBSTR(V_BSTR(&prop_value));
	return result;
}