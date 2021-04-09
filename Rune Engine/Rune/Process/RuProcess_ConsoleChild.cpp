/*!
	@file RuProcess_ConsoleChild.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../Process/RuProcess_ConsoleChild.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuProcess_ConsoleChild, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuProcess_ConsoleChild, "CRuProcess_ConsoleChild", "IRuObject")
ruCLASSGUID_DEFINE(CRuProcess_ConsoleChild, 0x444319E9, 0xC79C43be, 0x9818EE95, 0xC79E7D80)

// ************************************************************************************************************************************************************

CRuProcess_ConsoleChild::CRuProcess_ConsoleChild()
:	m_hStdinWrite(INVALID_HANDLE_VALUE),
	m_hStdoutRead(INVALID_HANDLE_VALUE),
	m_lastErrorString(NULL),
	m_processStarted(FALSE)
{
}

CRuProcess_ConsoleChild::~CRuProcess_ConsoleChild()
{
	Terminate();

	delete[] m_lastErrorString;
}

BOOL CRuProcess_ConsoleChild::Start(const char *programPath, const char *cmdLine)
{
	// Setup a security attributes structure
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	// Create pipes
	HANDLE hStdoutReadTmp;				// parent stdout read handle
	HANDLE hStdoutWrite, hStderrWrite;	// child stdout write handle
	HANDLE hStdinWriteTmp;				// parent stdin write handle
	HANDLE hStdinRead;					// child stdin read handle

	// Create a child stdout pipe.
	if (!CreatePipe(&hStdoutReadTmp, &hStdoutWrite, &sa, 0))
	{
		SetLastErrorString("Cannot create pipe");
		return FALSE;
	}

	// Create a duplicate of the stdout write handle for the std error write handle. This is necessary in case the child application closes one of its std output handles.
	if (!DuplicateHandle(GetCurrentProcess(), hStdoutWrite, GetCurrentProcess(), &hStderrWrite, 0, TRUE, DUPLICATE_SAME_ACCESS))
	{
		SetLastErrorString("Cannot create pipe");
		return FALSE;
	}

	// Create a child stdin pipe.
	if (!CreatePipe(&hStdinRead, &hStdinWriteTmp, &sa, 0))
	{
		SetLastErrorString("Cannot create pipe");
		return FALSE;
	}

	// Create new stdout read handle and the stdin write handle.
	// Set the inheritance properties to FALSE. Otherwise, the child inherits the these handles; resulting in non-closeable handles to the pipes being created.
	if (!DuplicateHandle(GetCurrentProcess(), hStdoutReadTmp, GetCurrentProcess(), &m_hStdoutRead, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		SetLastErrorString("Cannot create pipe");
		return FALSE;
	}

	if (!DuplicateHandle(GetCurrentProcess(), hStdinWriteTmp, GetCurrentProcess(), &m_hStdinWrite, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		SetLastErrorString("Cannot create pipe");
		return FALSE;
	}

	// Close inheritable copies of the handles we do not want to be inherited.
	CloseHandle(hStdoutReadTmp);
	CloseHandle(hStdinWriteTmp);

	// Spawn a child thread
	ZeroMemory(&m_processInfo, sizeof(PROCESS_INFORMATION));

	ZeroMemory(&m_si, sizeof(STARTUPINFO));
	m_si.cb = sizeof(STARTUPINFO);
	m_si.dwFlags = STARTF_USESTDHANDLES;
	m_si.hStdOutput = hStdoutWrite;
	m_si.hStdInput = hStdinRead;
	m_si.hStdError = hStderrWrite;

	// Setup current directory string
	CRuString curDirectory(programPath);
	curDirectory.TruncateAt(max(curDirectory.LastIndexOf("\\"), curDirectory.LastIndexOf("/")));

	// Setup the commandline string
	char *combinedCmdLine = new char [strlen(programPath) + strlen(cmdLine) + 2];
	sprintf(combinedCmdLine, "%s %s", programPath, cmdLine);

	BOOL succeeded = CreateProcess(programPath, combinedCmdLine, NULL, NULL, TRUE, CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS, NULL, (const char *) curDirectory, &m_si, &m_processInfo);

	delete[] combinedCmdLine;

	if(!succeeded)
	{
		DWORD errorCode = GetLastError();

		char errorString[256];
		sprintf(errorString, "Failed to start process, error code is %x.", errorCode);
		SetLastErrorString(errorString);

		return FALSE;
	}

	// Child is launched. Close the parents copy of those pipe handles that only the child should have open.
	// Make sure that no handles to the write end of the stdout pipe are maintained in this process or else the pipe will not close when the child process exits and ReadFile will hang.
	CloseHandle(hStdoutWrite);
	CloseHandle(hStdinRead);
	CloseHandle(hStderrWrite);

	// Mark process as having started
	m_processStarted = TRUE;

	return TRUE;
}

BOOL CRuProcess_ConsoleChild::Terminate()
{
	// Forcibly terminate the child process
	TerminateProcess(m_processInfo.hProcess, 0);

	// Close child process handles
	CloseHandle(m_processInfo.hProcess);
	CloseHandle(m_processInfo.hThread);

	// The following can be closed just fine
	CloseHandle(m_hStdinWrite);
	CloseHandle(m_hStdoutRead);

	// Clean out the output queue
	while(m_childOutputQueue.IsEmpty() == FALSE)
	{
		OutputLine outLine = m_childOutputQueue.Dequeue();
		delete[] outLine.m_lineText;
	}

	// Mark process as having ended
	m_processStarted = FALSE;

	return TRUE;
}

BOOL CRuProcess_ConsoleChild::ReadLine(char *buffer, INT32 bufferLen, INT32 *bufferLenRequired)
{
	// Process child output
	ProcessChildOutput();

	// Retrieve next output line
	if(m_childOutputQueue.IsEmpty() == FALSE)
	{
		OutputLine outLine = m_childOutputQueue.Dequeue();

		if(outLine.m_terminated)
		{
			*bufferLenRequired = (INT32)strlen(outLine.m_lineText) + 1;

			if(bufferLen < *bufferLenRequired)
			{
				// The buffer is too small, queue it back at head of queue
				m_childOutputQueue.QueueAtHead(outLine);
				return FALSE;
			}
			else
			{
				memcpy(buffer, outLine.m_lineText, *bufferLenRequired);
				delete[] outLine.m_lineText;
				return TRUE;
			}
		}
		else
		{
			// The entry has not been terminated, queue it back at head of queue
			m_childOutputQueue.QueueAtHead(outLine);

			// No output available, set required buffer length to zero and return false
			*bufferLenRequired = 0;
			return FALSE;
		}
	}

	// No output available, set required buffer length to zero and return false
	*bufferLenRequired = 0;
	return FALSE;
}

BOOL CRuProcess_ConsoleChild::WriteLine(const char *buffer, INT32 bufferLen)
{
	char *combinedLine = ruNEW char [bufferLen + 2];
	memcpy(combinedLine, buffer, bufferLen);
	combinedLine[bufferLen] = '\n';
	combinedLine[bufferLen + 1] = NULL;

	DWORD bytesWritten;
	WriteFile(m_hStdinWrite, combinedLine, bufferLen + 1, &bytesWritten, NULL);

	delete[] combinedLine;

	return TRUE;
}

BOOL CRuProcess_ConsoleChild::IsProcessStarted()
{
	return m_processStarted;
}

BOOL CRuProcess_ConsoleChild::IsProcessNominal()
{
	DWORD exitCode;

	if(GetExitCodeProcess(m_processInfo.hProcess, &exitCode))
	{
		if(exitCode == STILL_ACTIVE)
			return TRUE;

		return FALSE;
	}

	DWORD errorCode = GetLastError();

	return FALSE;
}

const char *CRuProcess_ConsoleChild::GetLastErrorString() const
{
	return m_lastErrorString ? m_lastErrorString : "";
}

void CRuProcess_ConsoleChild::ProcessChildOutput()
{
	DWORD bytesAvailable;
	if(PeekNamedPipe(m_hStdoutRead, NULL, NULL, NULL, &bytesAvailable, NULL) && bytesAvailable > 0)
	{
		// Allocate temporary buffer for read
		char *buffer = ruNEW char [bytesAvailable + 1];

		// Read from pipe
		DWORD bytesRead = 0;
		ReadFile(m_hStdoutRead, buffer, bytesAvailable, &bytesRead, NULL);

		// NULL-terminate the buffer
		buffer[bytesRead] = NULL;

		DWORD bolIdx = 0, eolIdx = 0;
		do
		{
			// Find the next newline or carriage return
			while(eolIdx < bytesRead && (buffer[eolIdx] != '\n' && buffer[eolIdx] != '\r'))
			{
				++eolIdx;
			}

			if(eolIdx >= bolIdx)
			{
				OutputLine outLine = { NULL, FALSE };

				if(m_childOutputQueue.IsEmpty() == FALSE)
				{
					// Dequeue the last entry in the output queue
					outLine = m_childOutputQueue.DequeueTail();

					// If the last entry in the output queue has already been terminated, put it back
					if(outLine.m_terminated)
					{
						m_childOutputQueue.Queue(outLine);

						outLine.m_lineText = NULL;
						outLine.m_terminated = FALSE;
					}
				}

				// Append newly read bytes to the existing output entry
				char *oldLineText = outLine.m_lineText;
				INT32 oldLineLen = oldLineText ? (INT32)strlen(oldLineText) : 0;

				outLine.m_lineText = ruNEW char [oldLineLen + eolIdx - bolIdx + 1];

				// Copy the previously non-terminated line into new buffer
				if(oldLineLen > 0)
				{
					memcpy(outLine.m_lineText, oldLineText, oldLineLen);
				}

				// Copy newly read bytes to the new buffer
				memcpy(&outLine.m_lineText[oldLineLen], &buffer[bolIdx], eolIdx - bolIdx);

				// NULL-terminate the buffer
				outLine.m_lineText[oldLineLen + eolIdx - bolIdx] = NULL;

				// Delete the old buffer
				delete[] oldLineText;

				// Insert output entry into queue
				m_childOutputQueue.Queue(outLine);
			}

			// If this line is terminated, mark it
			if(eolIdx < bytesRead && (buffer[eolIdx] != '\n' || buffer[eolIdx] != '\r'))
			{
				if(m_childOutputQueue.IsEmpty() == FALSE)
				{
					OutputLine outLine = m_childOutputQueue.DequeueTail();
					outLine.m_terminated = TRUE;
					m_childOutputQueue.Queue(outLine);
				}
			}

			// Skip to next line-feed or carriage-return
			while(eolIdx < bytesRead && (buffer[eolIdx] != '\n' && buffer[eolIdx] != '\r'))
			{
				++eolIdx;
			}

			// Skip all consecutive line-feeds or carriage-returns
			while(eolIdx < bytesRead && (buffer[eolIdx] == '\n' || buffer[eolIdx] == '\r'))
			{
				++eolIdx;
			}

			bolIdx = eolIdx;
			
		} while (bolIdx < bytesRead);

		delete[] buffer;
	}
}

void CRuProcess_ConsoleChild::SetLastErrorString(const char *lastErrorString)
{
	delete[] m_lastErrorString;
	m_lastErrorString = ruNEW char [strlen(lastErrorString) + 1];
	strcpy(m_lastErrorString, lastErrorString);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
