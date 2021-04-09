#pragma once

// Base types
#include "../../Core/Thread/RuThread_SyncObject.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuThread_CriticalSection : public CRuThread_SyncObject
{
protected:
	CRITICAL_SECTION								m_criticalSection;

public:
													CRuThread_CriticalSection();
	virtual											~CRuThread_CriticalSection();

	void											Enter();
	BOOL											TryEnter();
	void											Leave();

	virtual void									Lock();
	virtual void									Unlock();

protected:
	// Prevent copying by declaring copy constructors as protected
													CRuThread_CriticalSection(const CRuThread_CriticalSection &obj);
	CRuThread_CriticalSection&						operator=(const CRuThread_CriticalSection & obj);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
