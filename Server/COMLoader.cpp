#include "stdafx.h"

// General
#include "COMLoader.h"

// Addition
#include "Log.h"

COMLoader::COMLoader()
{
	HRESULT hr;

	// Initialize COM. ------------------------------------------

	hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		Log::Error(L"Failed to initialize COM library.Error code = %d", hr);
	}

	// Set general COM security levels --------------------------

	hr = CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IDENTIFY,    // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
	);
	if (FAILED(hr))
	{
		Log::Error(L"Failed to initialize security.Error code= %d", hr);
	}
}

COMLoader::~COMLoader()
{
	CoUninitialize();
}
