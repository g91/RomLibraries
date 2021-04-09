#include "../../Scene/Terrain/RuWorld_TriggerSystem.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuWorld_TriggerContainer, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_TriggerContainer, "CRuWorld_TriggerContainer", "IRuObject")
ruCLASSGUID_DEFINE(CRuWorld_TriggerContainer, 0xFB31E6D1, 0xB19C4aa1, 0x9B879545, 0x2978349E)

ruRTTI_DEFINE_SUBCLASS(CRuWorld_TriggerManager, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_TriggerManager, "CRuWorld_TriggerManager", "CRuEntity")
ruCLASSGUID_DEFINE(CRuWorld_TriggerManager, 0xB1AD4869, 0x8A6A4c2f, 0x93D5B9F2, 0x33C5B294)

// ************************************************************************************************************************************************************

static const char *worldTriggerContainerDS_TableName_Trigger = "worldTriggerContainer_Trigger";

// ************************************************************************************************************************************************************

CRuWorld_TriggerContainer::CRuWorld_TriggerContainer()
:	m_dataStore(NULL),
	m_triggerQueue(NULL)
{
}

CRuWorld_TriggerContainer::~CRuWorld_TriggerContainer()
{
	// Release data store
	ruSAFE_RELEASE(m_dataStore);

	// Free trigger queue
	ClearTriggerQueue();
	delete m_triggerQueue;

	// Release any remaining references in the change list
	for(INT32 i = 0; i < m_changedTriggerList.Count(); ++i)
	{
		ruSAFE_RELEASE(m_changedTriggerList[i]);
	}
}

BOOL CRuWorld_TriggerContainer::Attach(CRuDataStore_Base *dataStore)
{
	// Save reference to data store
	m_dataStore = dataStore;
	m_dataStore->AddRef();

	// Get data tables
	m_tableHandle_Trigger = m_dataStore->GetTableHandle(worldTriggerContainerDS_TableName_Trigger);

	if(m_tableHandle_Trigger < 0)
	{
		m_tableHandle_Trigger = m_dataStore->CreateTable(worldTriggerContainerDS_TableName_Trigger, 16);
	}

	// Construct queue
	m_triggerQueue = ruNEW CRuLRUQueue<CRuGUID, CRuWorld_Trigger *>(1024, 2048);

	return TRUE;
}

BOOL CRuWorld_TriggerContainer::SaveChanges()
{
	CRuMemoryStream *bufferStream = ruNEW CRuMemoryStream();

	// Save all changed trigger
	for(INT32 i = 0; i < m_changedTriggerList.Count(); ++i)
	{
		// Reset buffer and serialize trigger
		bufferStream->SetStreamSize(0);
		m_changedTriggerList[i]->SerializeTo(bufferStream);

		// Store stream
		m_dataStore->Store(m_tableHandle_Trigger, m_changedTriggerList[i]->GetGUID().Data(), bufferStream);
	
		// Release reference to trigger
		ruSAFE_RELEASE(m_changedTriggerList[i]);
	}

	// Clear changed templates list
	m_changedTriggerList.Clear();

	// Delete all deleted descriptors
	for(INT32 i = 0; i < m_deletedTriggerList.Count(); ++i)
	{
		m_dataStore->Delete(m_tableHandle_Trigger, m_deletedTriggerList[i].Data());
	}

	delete bufferStream;

	return TRUE;
}

BOOL CRuWorld_TriggerContainer::CopyTo(CRuDataStore_Base *targetDataStore)
{
	if(m_dataStore)
	{
		INT32 srcTableList[1];
		srcTableList[0] = m_tableHandle_Trigger;

		// Create tables
		INT32 targetTableList[1];
		targetTableList[0] = targetDataStore->CreateTable(worldTriggerContainerDS_TableName_Trigger, 16);

		// Copy data in each table
		for(INT32 i = 0; i < 1; ++i)
		{
			INT32 numEntries = m_dataStore->GetNumRecords(srcTableList[i]);
			INT32 keyLength = m_dataStore->GetTableKeyLength(srcTableList[i]);
			BYTE *key = ruNEW BYTE [keyLength];

			for(INT32 j = 0; j < numEntries; ++j)
			{
				// Retrieve key and data
				IRuStream *stream = m_dataStore->RetrieveByIndex(srcTableList[i], j);

				if(stream)
				{
					m_dataStore->RetrieveKeyByIndex(srcTableList[i], j, key);
					targetDataStore->Store(targetTableList[i], key, stream);
					delete stream;
				}
			}

			delete[] key;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuWorld_TriggerContainer::InsertTrigger(CRuWorld_Trigger *trigger)
{
	// Verify that the trigger GUID does not already exist in the change list
	if(GetTriggerFromChangeList(trigger->GetGUID()))
	{
		return FALSE;
	}

	// Verify that the trigger GUID does not already exist in the data store
	IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_Trigger, trigger->GetGUID().Data());
	if(stream)
	{
		delete stream;
		return FALSE;
	}

	// Ensure the delete list does not include the trigger
	m_deletedTriggerList.Remove(trigger->GetGUID());

	// Insert trigger into change list
	m_changedTriggerList.Add(trigger);

	return TRUE;
}

BOOL CRuWorld_TriggerContainer::DeleteTrigger(const CRuGUID &guid)
{
	if(m_deletedTriggerList.Contains(guid) == FALSE)
	{
		// Remove trigger from change list
		CRuWorld_Trigger *trigger = GetTriggerFromChangeList(guid);
		if(trigger)
		{
			m_changedTriggerList.Remove(trigger);
			ruSAFE_RELEASE(trigger);
		}

		// Remove trigger from queue
		m_triggerQueue->Remove(guid, guid.HashCode());

		// Add trigger GUID to delete list
		m_deletedTriggerList.Add(guid);
	}

	return TRUE;
}

BOOL CRuWorld_TriggerContainer::MarkTriggerForUpdate(const CRuGUID &guid)
{
	if(GetTriggerFromChangeList(guid) == NULL)
	{
		// Retrieve trigger for the specified GUID
		CRuWorld_Trigger *trigger = GetTrigger(guid);

		// Add trigger GUID to change list
		if(trigger)
		{
			trigger->AddRef();
			m_changedTriggerList.Add(trigger);
		}
	}

	return TRUE;
}

CRuWorld_Trigger *CRuWorld_TriggerContainer::GetTrigger(const CRuGUID &guid)
{
	CRuWorld_Trigger *trigger = NULL;

	// Search the change list
	if(trigger = GetTriggerFromChangeList(guid))
	{
		return trigger;
	}

	// Search the trigger queue for matching GUID
	if(m_triggerQueue->ContainsKey(guid, guid.HashCode()))
	{
		return m_triggerQueue->Get(guid, guid.HashCode());
	}

	// Make sure the trigger is not in the delete list
	if(m_deletedTriggerList.Contains(guid))
	{
		return NULL;
	}

	// Retrieve trigger stream
	IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_Trigger, guid.Data());
	if(stream)
	{
		// Serialize trigger from stream
		trigger = ruNEW CRuWorld_Trigger();
		trigger->SerializeFrom(stream);
		delete stream;

		// Make space in the LRU queue
		CRuWorld_Trigger *reclaimedTrigger;
		while(m_triggerQueue->Reclaim(guid, guid.HashCode(), 1, reclaimedTrigger))
		{
			// Release reclaimed trigger
			ruSAFE_RELEASE(reclaimedTrigger);
		}

		// Add trigger to LRU queue
		m_triggerQueue->Add(guid, guid.HashCode(), trigger, 1);

		return trigger;
	}

	return NULL;
}

BOOL CRuWorld_TriggerContainer::ClearTriggerQueue()
{
	// Iterate through each trigger and destroy
	m_triggerQueue->ForAllElements(boost::bind(&CRuWorld_TriggerContainer::ClearTriggerQueueCallback, this, _1, _2));

	// Clear all triggers
	m_triggerQueue->Clear();

	return TRUE;
}

CRuWorld_Trigger *CRuWorld_TriggerContainer::GetTriggerFromChangeList(const CRuGUID &guid)
{
	for(INT32 i = 0; i < m_changedTriggerList.Count(); ++i)
	{
		if(m_changedTriggerList[i]->GetGUID() == guid)
		{
			return m_changedTriggerList[i];
		}
	}

	return NULL;
}

BOOL CRuWorld_TriggerContainer::ClearTriggerQueueCallback(const void *key, void *data)
{
	CRuWorld_Trigger *trigger = *((CRuWorld_Trigger **) data);

	ruSAFE_RELEASE(trigger);

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuWorld_TriggerManager::CRuWorld_TriggerManager()
:	m_triggerContainer(NULL),
	m_enumerationMarginSize(120.0f)
{
	m_enumerationHash = ruNEW CRuKeyHash<CRuGUID>(2048);
}

CRuWorld_TriggerManager::~CRuWorld_TriggerManager()
{
	ruSAFE_RELEASE(m_triggerContainer);
	delete m_enumerationHash;
}

BOOL CRuWorld_TriggerManager::Attach(CRuWorld_TriggerContainer *container)
{
	m_triggerContainer = container;
	m_triggerContainer->AddRef();

	return TRUE;
}

BOOL CRuWorld_TriggerManager::SetEnumerator(RuWorld::TriggerEnumerator enumerator)
{
	m_enumerator = enumerator;
	return TRUE;
}

BOOL CRuWorld_TriggerManager::ClearEnumerationCache()
{
	// Reset enumeration bounds
	m_enumerationBounds.Set(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	m_enumerationHash->Clear();

	// Remove all triggers from the tree
	this->RemoveAllChildren();

	return TRUE;
}

BOOL CRuWorld_TriggerManager::UpdateEnumeration(const CRuAABB &enumerationBounds)
{
	if(m_enumerationBounds.Contains(enumerationBounds) == FALSE)
	{
		// Clear enumeration cache
		this->ClearEnumerationCache();

		// Set enumeration bounds to the specified bounds plus the margin
		m_enumerationBounds = enumerationBounds;
		m_enumerationBounds.Minima().m_x -= m_enumerationMarginSize;
		m_enumerationBounds.Minima().m_y -= m_enumerationMarginSize;
		m_enumerationBounds.Minima().m_z -= m_enumerationMarginSize;
		m_enumerationBounds.Maxima().m_x += m_enumerationMarginSize;
		m_enumerationBounds.Maxima().m_y += m_enumerationMarginSize;
		m_enumerationBounds.Maxima().m_z += m_enumerationMarginSize;

		// Iterate and find all unique GUIDs
		CRuArrayList<CRuGUID> &triggerGUIDList = m_enumerator();
		for(INT32 i = 0; i < triggerGUIDList.Count(); ++i)
		{
			if(m_enumerationHash->ContainsKey(triggerGUIDList[i], triggerGUIDList[i].HashCode()) == FALSE)
			{
				CRuWorld_Trigger *trigger = m_triggerContainer->GetTrigger(triggerGUIDList[i]);

				if(trigger && Intersects(enumerationBounds, trigger->GetWorldBounds()))
				{
					// Attach trigger to tree
					this->AddChild(trigger);

					// Append GUID to unique list
					m_enumerationHash->Add(triggerGUIDList[i], triggerGUIDList[i].HashCode());
				}
			}
		}
	}

	return TRUE;
}

BOOL CRuWorld_TriggerManager::Update(REAL elapsedTime)
{
	return CRuEntity::Update(elapsedTime);
}

// ************************************************************************************************************************************************************

#pragma managed
