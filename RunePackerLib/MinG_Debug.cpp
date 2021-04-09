// MinG_Debug.cpp: implementation of the Debug class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MinG_Debug.h"
#ifdef MMGR
#include "MinG_MMGr.h"
#endif

//namespace MGEL
//{
	
	
	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////
	
	/* 
	**----------------------------------------------------------------------------- 
	**  Name:       dprintf 
	**  Purpose:Printf to debug output 
	**----------------------------------------------------------------------------- 
	*/  
	
#ifdef _DEBUG 
	
	
	DWORD Debug::g_dwDebugLevel;  
	
	
	void __cdecl dprintf (DWORD dwDebugLevel, LPCTSTR szFormat, ...) 
	{     
		TCHAR   szBuffer[1024];     
		va_list va;  
		// Check if current debug level     
		if (dwDebugLevel <= Debug::g_dwDebugLevel)     
			return;          
		lstrcpy (szBuffer, START_STR);     
		va_start( va, szFormat );     
		wvsprintf (szBuffer+lstrlen (szBuffer), szFormat, va);     
		va_end( va );     
		lstrcat (szBuffer, END_STR);     
		OutputDebugString (szBuffer); 
	} 
#endif // DEBUG   
//}