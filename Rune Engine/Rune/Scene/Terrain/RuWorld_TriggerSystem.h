#ifndef _RUWORLD_TRIGGERSYSTEM_H_
#define _RUWORLD_TRIGGERSYSTEM_H_

#include "../../Core/DataStore/RuDataStore_Base.h"
#include "../../Scene/Terrain/RuWorld_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

namespace RuWorld
{
	typedef boost::function<CRuArrayList<CRuGUID> & ()> TriggerEnumerator;		// Parameters: (enumeratedGUIDListOut)
}

// ************************************************************************************************************************************************************

class CRuWorld_TriggerContainer : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Data store
	CRuDataStore_Base*								m_dataStore;							//!< Data store
	INT32											m_tableHandle_Trigger;					//!< Handle to trigger table

	// Entity descriptor cache
	CRuLRUQueue<CRuGUID, CRuWorld_Trigger *>*		m_triggerQueue;							//!< LRU queue of triggers

	// Update list
	CRuArrayList<CRuWorld_Trigger *>				m_changedTriggerList;					//!< List of changed triggers
	CRuArrayList<CRuGUID>							m_deletedTriggerList;					//!< List of deleted GUIDs

public:
											CRuWorld_TriggerContainer();
	virtual									~CRuWorld_TriggerContainer();

	// Data source management
	BOOL									Attach(CRuDataStore_Base *dataStore);
	BOOL									SaveChanges();
	BOOL									CopyTo(CRuDataStore_Base *targetDataStore);

	// Entity management
	BOOL									InsertTrigger(CRuWorld_Trigger *trigger);
	BOOL									DeleteTrigger(const CRuGUID &guid);
	BOOL									MarkTriggerForUpdate(const CRuGUID &guid);
	CRuWorld_Trigger*						GetTrigger(const CRuGUID &guid);

	// Queue management
	BOOL									ClearTriggerQueue();

protected:
	CRuWorld_Trigger*						GetTriggerFromChangeList(const CRuGUID &guid);

	BOOL									ClearTriggerQueueCallback(const void *key, void *data);
};

// ************************************************************************************************************************************************************

class CRuWorld_TriggerManager : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuWorld_TriggerContainer*				m_triggerContainer;					//!< Trigger container
	RuWorld::TriggerEnumerator				m_enumerator;						//!< Function used for trigger GUID enumeration

	REAL									m_enumerationMarginSize;			//!< Size of margin to add to enumeration bounds
	CRuAABB									m_enumerationBounds;				//!< Current enumeration bounds including margin

	CRuKeyHash<CRuGUID>*					m_enumerationHash;					//!< Temporary hash for storing trigger GUIDs

public:
											CRuWorld_TriggerManager();
	virtual									~CRuWorld_TriggerManager();

	// Container management
	BOOL									Attach(CRuWorld_TriggerContainer *container);
	BOOL									SetEnumerator(RuWorld::TriggerEnumerator enumerator);
	BOOL									ClearEnumerationCache();

	BOOL									UpdateEnumeration(const CRuAABB &enumerationBounds);
	virtual BOOL							Update(REAL elapsedTime);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
