#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
typedef const std::wstring& cwstring;
#include <sstream>
#include <queue>

#include <algorithm>
#include <fstream>
#include <cmath>
#include <typeinfo>
#include <memory>
#include <iostream>
#include <conio.h> 

#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN 1
#endif

#ifndef NOMINMAX
#   define NOMINMAX
#endif

//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define _ATL_NO_AUTOMATIC_NAMESPACE
#include <atlbase.h> // ATL
//#include <comutil.h>   comsuppw.lib
#include <wbemidl.h> // WMI
#include <windows.h> 
#include <winsock2.h>
#pragma comment (lib, "Mswsock.lib")
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#include <locale.h>

using namespace ATL;

//--

#include "Common.h"
#include "Debug.h"
#include "Log.h"
#include "SmartPtr.h"
#include "Thread.h"
