#include <windows.h>
#include <time.h>
#include <strsafe.h>
#include <DbgHelp.h>
#include "CreateDump.h"
#include "../DebugLog/ErrorMessage.h"
#include <winnt.h>

bool CreateDump( const wchar_t* lpszFileName , DumpTypeENUM type )
{
	BOOL Result = FALSE;

	HANDLE hFile = CreateFileW( lpszFileName, GENERIC_READ | GENERIC_WRITE, 
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( ( hFile != NULL ) && ( hFile != INVALID_HANDLE_VALUE ) ) 
	{
		MINIDUMP_TYPE mdt = MiniDumpNormal;

		switch (type)
		{
		case EM_DumpType_MID:
			mdt = (MINIDUMP_TYPE)(MiniDumpWithPrivateReadWriteMemory | 
				MiniDumpWithDataSegs | 
				MiniDumpWithHandleData |
				MiniDumpWithUnloadedModules ); 
			break;
		case EM_DumpType_MAX:
			mdt = (MINIDUMP_TYPE)(MiniDumpWithFullMemory | 
				MiniDumpWithHandleData | 
				MiniDumpWithUnloadedModules ); 
			break;
		}

		HANDLE hProcess = GetCurrentProcess();
		unsigned long dwPID =  GetCurrentProcessId();

		Result = MiniDumpWriteDump( hProcess, dwPID, hFile, mdt, NULL, NULL, NULL ); 

		CloseHandle( hFile );
		CloseHandle( hProcess );

		if( !Result )
			Print(LV5, "CreateDump", "MiniDumpWriteDump failed. Error: %u", GetLastError() ); 

	}
	else
	{
		Print(LV5, "CreateDump", "Create file %s failed. ", lpszFileName ); 
	}

	return (Result == TRUE);
}




#define MAXIMUM_DUMP_PATH_SIZE	250
int GenerateCrashDumpFile(EXCEPTION_POINTERS* pExcptr, char* appName, char* pathOut/* = nullptr */, char* dumpName /*= nullptr*/ )
{
	return GenerateCrashDumpFile(pExcptr, appName, MiniDumpWithFullMemory, pathOut, dumpName);
}

int GenerateCrashDumpFile(EXCEPTION_POINTERS* pExcptr, char* appName, MINIDUMP_TYPE type, char* pathOut, char* dumpName)
{
	HANDLE hDumpFile;
	DWORD dwBufferSize = MAXIMUM_DUMP_PATH_SIZE;
	char szPath[MAXIMUM_DUMP_PATH_SIZE];
	char* pszPathName = "CrashDumps";
	char szFileName[MAXIMUM_DUMP_PATH_SIZE];

	//if(MessageBox(NULL, _T("Do you want to save the Dump File ?"), _T("Save Dialog"), MB_YESNO) == IDYES)
	{
		GetCurrentDirectory(MAXIMUM_DUMP_PATH_SIZE, szPath);
		//GetTempPath(MAXIMUM_DUMP_PATH_SIZE, szPath);
		StringCchPrintf(szFileName, MAXIMUM_DUMP_PATH_SIZE, "%s\\%s", szPath, pszPathName);

		CreateDirectory(szFileName, NULL);

		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];

		time (&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer,sizeof(buffer),"%d-%m-%Y-%H-%M-%S",timeinfo);
		std::string str(buffer);



		StringCchPrintf(szFileName, MAXIMUM_DUMP_PATH_SIZE, "%s\\%s\\%s-%s.dmp", szPath, pszPathName, appName, buffer);
		
		if (pathOut && dumpName)
		{
			sprintf(dumpName, "%s-%s.dmp", appName, buffer);
			strcpy(pathOut, szFileName);
		}


		//Get Handle to Dump File
		hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
		
		MINIDUMP_EXCEPTION_INFORMATION* ExpInfoPtr = nullptr;
		MINIDUMP_EXCEPTION_INFORMATION ExpInfo;

		if (pExcptr)
		{
			ExpInfo.ThreadId = GetCurrentThreadId();
			ExpInfo.ExceptionPointers = pExcptr;
			ExpInfo.ClientPointers = TRUE;
			ExpInfoPtr = &ExpInfo;
		}

		bool bSuccessful = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, type, ExpInfoPtr, NULL, NULL );
		CloseHandle(hDumpFile);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}
