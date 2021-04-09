#include "../../Core/Thread/RuThread_Worker.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuThread_Worker::CRuThread_Worker()
:	m_sleepPeriod(1),
	m_workerFunctions(32)
{
	// Create critical sections
	m_workerFunctions_CS = ruNEW CRuThread_CriticalSection();

	// Create thread termination events
	m_threadTerminationEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_threadTerminationFinishEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Create worker thread
	m_workerThread = CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(CRuThread_Worker::WorkerThreadFunc), this, 0, &m_workerThreadID);
}

CRuThread_Worker::~CRuThread_Worker()
{
	// Set thread termination event and wait for actual termination
	SetEvent(m_threadTerminationEvent);
	WaitForSingleObject(m_threadTerminationFinishEvent, INFINITE);

	// Close all thread-related handles
	CloseHandle(m_threadTerminationEvent);
	CloseHandle(m_threadTerminationFinishEvent);
	CloseHandle(m_workerThread);

	// Delete worker function descriptors
	for(INT32 i = 0; i < m_workerFunctions.Count(); ++i)
	{
		delete m_workerFunctions[i];
	}

	// Destroy critical sections
	delete m_workerFunctions_CS;
}

void CRuThread_Worker::SetSleepPeriod(INT32 sleepPeriod)
{
	m_sleepPeriod = sleepPeriod;

	if(m_sleepPeriod > 10)
	{
		m_sleepPeriod = 10;
	}
}

BOOL CRuThread_Worker::RegisterWorkerFunction(const CRuGUID &guid, WorkerFunction workerFunction)
{
	BOOL retVal = TRUE;

	m_workerFunctions_CS->Enter();

	// Make sure there are no other worker functions registered under the GUID
	for(INT32 i = 0; i < m_workerFunctions.Count(); ++i)
	{
		if(m_workerFunctions[i]->m_guid == guid)
		{
			retVal = FALSE;
			break;
		}
	}

	// If there are no worker functions registered under the GUID, perform registration
	if(retVal != false)
	{
		WorkerFunctionDesc *workerFunctionDesc = ruNEW WorkerFunctionDesc();
		workerFunctionDesc->m_guid = guid;
		workerFunctionDesc->m_function = workerFunction;

		m_workerFunctions.Add(workerFunctionDesc);
	}

	m_workerFunctions_CS->Leave();

	return retVal;
}

BOOL CRuThread_Worker::UnregisterWorkerFunction(const CRuGUID &guid)
{
	BOOL retVal = FALSE;

	m_workerFunctions_CS->Enter();

	for(INT32 i = 0; i < m_workerFunctions.Count(); ++i)
	{
		if(m_workerFunctions[i]->m_guid == guid)
		{
			delete m_workerFunctions[i];
			m_workerFunctions.RemoveAt(i);

			retVal = TRUE;
			break;
		}
	}

	m_workerFunctions_CS->Leave();

	return retVal;
}

DWORD WINAPI CRuThread_Worker::WorkerThreadFunc(LPVOID *parameter)
{
	CRuThread_Worker *threadWorker = reinterpret_cast<CRuThread_Worker *>(parameter);

	while(TRUE)
	{
		// Execute all worker functions
		threadWorker->m_workerFunctions_CS->Enter();

		for(INT32 i = 0; i < threadWorker->m_workerFunctions.Count(); ++i)
		{
			threadWorker->m_workerFunctions[i]->m_function();
		}

		threadWorker->m_workerFunctions_CS->Leave();

		// Sleep
		Sleep(threadWorker->m_sleepPeriod);

		// Check for exit condition
		if(WaitForSingleObject(threadWorker->m_threadTerminationEvent, 0) == WAIT_OBJECT_0)
		{
			SetEvent(threadWorker->m_threadTerminationFinishEvent);
			return TRUE;
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
