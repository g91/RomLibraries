#ifndef MING_DEBUG_H 
#define MING_DEBUG_H 

#include "Windows.h"

#define MGEL_API 
//namespace MGEL
//{

/* 
**----------------------------------------------------------------------------- 
**  Defines 
**----------------------------------------------------------------------------- 
*/ 
#ifdef _DEBUG 
// Note:  Define DEBUG_PROMPT_ME if you want MessageBox Error prompting 
//  This can get annoying quickly... 
// #define DEBUG_PROMPT_ME  
// Pre and Post debug string info 
#define START_STR TEXT("Debug: ") 
#define END_STR TEXT("\r\n") 
#endif // _DEBUG 

// Debug Levels 
#define DEBUG_ALWAYS 0L 
#define DEBUG_CRITICAL 1L 
#define DEBUG_ERROR 2L 
#define DEBUG_MINOR 3L 
#define DEBUG_WARN 4L 
#define DEBUG_DETAILS 5L 



/* 
**----------------------------------------------------------------------------- 
**  Macros 
**----------------------------------------------------------------------------- 
*/ 

class MGEL_API Debug
{
public:
	
#ifdef _DEBUG
	static DWORD g_dwDebugLevel;  
#endif
};



#ifdef _DEBUG
	#define DPF dprintf

	#define ASSERT(x) \
			if(!(x))\
			{\
				DPF(\
					DEBUG_ALWAYS,\
					TEXT("Assertion violated: %s,\
					File = %s, \
					Line = #%ld\n"),\
					TEXT(#x),\
					TEXT(__FILE__), \
					(DWORD)__LINE__ \
				);\
				abort();\
			}             

	//	#define REPORTERR(x) \ 
		//		ReportDDError ((x), TEXT("File = %s, Line = #%ld\n"), \ 
		//		TEXT(__FILE__), (DWORD)__LINE__ );     
	//	#define FATALERR(x) \	
			//	ReportDDError ((x), TEXT("File = %s, Line = #%ld\n"), \ 
			//	TEXT(__FILE__), (DWORD)__LINE__ ); \ 
			//	OnPause (TRUE); \
			//	DestroyWindow (g_hMainWindow); 
#else    
//	#define REPORTERR(x)   
	#define DPF 1 ? (void)0 : (void)   
	#define ASSERT(x)   
//	#define FATALERR(x) \    
		//	OnPause (TRUE); \       
		//	DestroyWindow (g_hMainWindow); 
#endif // _DEBUG 

/* 
**----------------------------------------------------------------------------- 
**  Global Variables 
**----------------------------------------------------------------------------- 
*/ 


//extern BOOL  g_fDebug;  
/* 
**----------------------------------------------------------------------------- 
** Function Prototypes 
**----------------------------------------------------------------------------- 
*/ 

// Debug Routines 
#ifdef _DEBUG 
MGEL_API void __cdecl dprintf(DWORD dwDebugLevel, LPCTSTR szFormat, ...); 
#endif //DEBUG  
//void _cdecl ReportDDError (HRESULT hResult, LPCTSTR szFormat, ...); 



//}
#endif//MING_DEBUG_H