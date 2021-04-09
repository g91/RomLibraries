// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include	<string>
#include <tchar.h>
#include "MinG_Debug.h"

//¦r¦ê©w¸q
#if 1 // GKy 100224 for IspaniaPatchMaker
//#if 0 // GKy 100224 for IspaniaPatch, etc.

 #include <atlstr.h>
#else 
 #include <atlbase.h>
 #include <atlapp.h>
 #include <atlmisc.h>
#endif 

using namespace std;
#ifdef UNICODE 
#define CTString  std::wstring
#else
#define CTString  std::string
#endif
#include "curlpp\curlpp.hpp"

#include "MD5\MD5Checksum.h"
// TODO: reference additional headers your program requires here
