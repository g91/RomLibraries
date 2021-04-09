#ifndef _RUTHREAD_WORKER_
#define _RUTHREAD_WORKER_

// Base types
#include "../../Core/Type/RuType_Object.h"
#include "../../Core/Thread/RuThread_CriticalSection.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************


// ************************************************************************************************************************************************************

class CRuThread_Worker
{
public:
	typedef boost::function<BOOL ()> WorkerFunction;		//!< BOOL WorkerFunction();

protected:
	struct WorkerFunctionDesc
	{
		CRuGUID										m_guid;
		WorkerFunction								m_function;
	};

	// Settings
	INT32											m_sleepPeriod;

	// ...
	CRuArrayList<WorkerFunctionDesc *>				m_workerFunctions;

	// Critical sections
	CRuThread_CriticalSection*						m_workerFunctions_CS;

	// Thread stuff
	HANDLE											m_threadTerminationEvent;
	HANDLE											m_threadTerminationFinishEvent;
	HANDLE											m_workerThread;
	DWORD											m_workerThreadID;

public:
													CRuThread_Worker();
													~CRuThread_Worker();

	void											SetSleepPeriod(INT32 sleepPeriod);

	BOOL											RegisterWorkerFunction(const CRuGUID &guid, WorkerFunction workerFunction);
	BOOL											UnregisterWorkerFunction(const CRuGUID &guid);

protected:
	// Worker thread function
	static DWORD WINAPI								WorkerThreadFunc(LPVOID *parameter);

	// Prevent copying by declaring copy constructors as protected
													CRuThread_Worker(const CRuThread_Worker &obj);
	CRuThread_Worker&								operator=(const CRuThread_Worker & obj);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
