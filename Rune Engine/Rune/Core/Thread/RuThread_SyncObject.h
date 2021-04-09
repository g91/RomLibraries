#pragma once

// Base types
#include "../../Core/Type/RuType_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuThread_SyncObject
{
public:
	virtual								~CRuThread_SyncObject() { }

	virtual void						Lock() = 0;
	virtual void						Unlock() = 0;
};

// ************************************************************************************************************************************************************

class CRuThread_ScopeLock
{
protected:
	CRuThread_SyncObject*				m_syncObject;

public:
										CRuThread_ScopeLock(CRuThread_SyncObject *syncObj)
										:	m_syncObject(syncObj)
										{
											assert(m_syncObject);
											m_syncObject->Lock();
										}

										~CRuThread_ScopeLock()
										{
											m_syncObject->Unlock();
										}

private:
	// Prevent copying by declaring copy constructors as protected
										CRuThread_ScopeLock(const CRuThread_ScopeLock &obj);
	CRuThread_ScopeLock&				operator=(const CRuThread_ScopeLock &obj);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
