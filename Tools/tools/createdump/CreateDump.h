#pragma once
#include <DbgHelp.h>

enum DumpTypeENUM
{
	EM_DumpType_MID		,
	EM_DumpType_MAX		,
};


bool	CreateDump( const wchar_t* fileName , DumpTypeENUM type );
int		GenerateCrashDumpFile(EXCEPTION_POINTERS* pExcptr, char* appName, char* pathOut = nullptr, char* dumpName = nullptr);
int		GenerateCrashDumpFile(EXCEPTION_POINTERS* pExcptr, char* appName, MINIDUMP_TYPE type, char* pathOut = nullptr, char* dumpName = nullptr);
