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
#include <functional>
#include <fstream>
#include <cmath>
#include <typeinfo>
#include <memory>

#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN 1
#endif

#ifndef NOMINMAX
#   define NOMINMAX
#endif

/*#define WINVER         0x0500
#define _WIN32_WINNT   0x0501
#define _WIN32_WINDOWS 0x0400
#define _WIN32_IE      0x0500*/

//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define _ATL_NO_AUTOMATIC_NAMESPACE
#include <atlbase.h> // ATL
//#include <comutil.h>   comsuppw.lib
#include <wbemidl.h> // WMI
#include <windows.h> 

using namespace std;
using namespace ATL;

//--

#include "Debug.h"
#include "SmartPtr.h"
