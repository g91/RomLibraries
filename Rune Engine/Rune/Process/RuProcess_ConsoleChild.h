/*!
	@file RuProcess_ConsoleChild.h

	Copyright (c) 2004-2008 All rights reserved
*/

#ifndef _RUPROCESS_CONSOLECHILD_H_
#define _RUPROCESS_CONSOLECHILD_H_

#include "../Core/Type/RuType_Object.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuProcess_ConsoleChild : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct OutputLine
	{
		char*								m_lineText;
		BOOL								m_terminated;
	};

	PROCESS_INFORMATION						m_processInfo;
	STARTUPINFO								m_si;

	HANDLE									m_hStdinWrite;
	HANDLE									m_hStdoutRead;

	CRuQueue<OutputLine>					m_childOutputQueue;

	char*									m_lastErrorString;

	BOOL									m_processStarted;

public:
											CRuProcess_ConsoleChild();
	virtual									~CRuProcess_ConsoleChild();

	// Start and termination
	BOOL									Start(const char *programPath, const char *cmdLine);
	BOOL									Terminate();

	// Communication
	BOOL									ReadLine(char *buffer, INT32 bufferLen, INT32 *bufferLenRequired);
	BOOL									WriteLine(const char *buffer, INT32 bufferLen);

	// Status reporting
	BOOL									IsProcessStarted();
	BOOL									IsProcessNominal();

	// Error reporting
	const char*								GetLastErrorString() const;

	// Process Information
	DWORD                                   GetProcessId(){return m_processInfo.dwProcessId;}

protected:
	void									ProcessChildOutput();

	void									SetLastErrorString(const char *lastErrorString);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
