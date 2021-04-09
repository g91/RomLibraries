#include "../../Core/Thread/RuThread_CriticalSection.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuThread_CriticalSection::CRuThread_CriticalSection()
{
	InitializeCriticalSection(&m_criticalSection);
}

CRuThread_CriticalSection::~CRuThread_CriticalSection()
{
	DeleteCriticalSection(&m_criticalSection);
}

void CRuThread_CriticalSection::Enter()
{
	EnterCriticalSection(&m_criticalSection);
}

BOOL CRuThread_CriticalSection::TryEnter()
{
	return TryEnterCriticalSection(&m_criticalSection);
}

void CRuThread_CriticalSection::Leave()
{
	LeaveCriticalSection(&m_criticalSection);
}

void CRuThread_CriticalSection::Lock()
{
	EnterCriticalSection(&m_criticalSection);
}

void CRuThread_CriticalSection::Unlock()
{
	LeaveCriticalSection(&m_criticalSection);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
