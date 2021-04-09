

#include "debug.h"
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#ifdef _DEBUGX 
	#define dEnable true
#else
	#define dEnable false
#endif
bool g_bDebugTable[16]=
{
	dEnable,dEnable,dEnable,dEnable,
	dEnable,dEnable,dEnable,dEnable,
	dEnable,dEnable,dEnable,dEnable,
	dEnable,dEnable,dEnable,dEnable
};
void DBG_Set(int layer,bool value )
{

	g_bDebugTable[layer]=value;
}
bool DBG_Get(int layer)
{

	return g_bDebugTable[layer];

}

void DBG_Printf(int layer,char * fmt, ...)
{
	if (layer==-1||g_bDebugTable[layer])
	{	
		char buffer[2048];
		va_list args;
		va_start(args,fmt);

		vsprintf(buffer,fmt,args);
		va_end(args);
		OutputDebugStringA(buffer);
	}
}