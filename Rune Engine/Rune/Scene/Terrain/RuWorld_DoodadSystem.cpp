#include "../../Scene/Terrain/RuWorld_DoodadSystem.h"

#include "../../Helper/RuHelper_Entity.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuWorld_DoodadContainer, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_DoodadContainer, "CRuWorld_DoodadContainer", "IRuObject")
ruCLASSGUID_DEFINE(CRuWorld_DoodadContainer, 0xED6F84A3, 0x4DC143b0,0xAD0C66A8, 0xA0D4630A)

ruRTTI_DEFINE_SUBCLASS(CRuWorld_DoodadManager, CRuEntity_Container_Impl)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_DoodadManager, "CRuWorld_DoodadManager", "CRuEntity_Container_Impl")
ruCLASSGUID_DEFINE(CRuWorld_DoodadManager, 0x042581D3, 0x4D334693, 0x8D211342, 0xB62FA820)

// ************************************************************************************************************************************************************

static const char *worldDoodadContainerDS_TableName_DoodadTemplate = "worldDoodadContainer_DoodadTemplate";

// ************************************************************************************************************************************************************

CRuWorld_DoodadContainer::CRuWorld_DoodadContainer()
:	m_dataStore(NULL),
	m_tableHandle_DoodadTemplate(-1),
	m_templateQueue(NULL),
	m_templateList(NULL),
	m_enableDeferredLoading(FALSE)
{
	// Initialize background work queue
	m_workQueue = ruNEW CRuThread_WorkQueue<CRuGUID, CRuDoodad_Template *>(32, 32, 64);

	// Register worker function with worker thread
	g_ruGlobalWorkerThread->RegisterWorkerFunction(this->GetGUID(), boost::bind(&CRuWorld_DoodadContainer::WorkerFunction, this));
}

CRuWorld_DoodadContainer::~CRuWorld_DoodadContainer()
{
	// Unregister worker function
	g_ruGlobalWorkerThread->UnregisterWorkerFunction(this->GetGUID());

	// Clear the background work queue
	CRuGUID orphanedResultKey;
	CRuDoodad_Template *orphanedResult = NULL;
	while(m_workQueue->DequeueResult(orphanedResultKey, orphanedResult))
	{
		ruSAFE_RELEASE(orphanedResult);
	}

	delete m_workQueue;

	// Release data store
	ruSAFE_RELEASE(m_dataStore);

	// Release doodad template queue
	ClearTemplateQueue();
	delete m_templateQueue;

	// If the template list exists, release all templates in the list
	if(m_templateList)
	{
		for(INT32 i = 0; i < m_templateList->Count(); ++i)
		{
			ruSAFE_RELEASE(m_templateList->Get(i));
		}

		delete m_templateList;
	}

	// Release any remaining references in the change list
	for(INT32 i = 0; i < m_changedTemplateList.Count(); ++i)
	{
		ruSAFE_RELEASE(m_changedTemplateList[i]);
	}
}

BOOL CRuWorld_DoodadContainer::Attach(CRuDataStore_Base *dataStore, BOOL readOnly)
{
	// Save reference to data store
	m_dataStore = dataStore;
	m_dataStore->AddRef();

	// Get data tables
	m_tableHandle_DoodadTemplate = m_dataStore->GetTableHandle(worldDoodadContainerDS_TableName_DoodadTemplate);

	if(m_tableHandle_DoodadTemplate < 0)
	{
		m_tableHandle_DoodadTemplate = m_dataStore->CreateTable(worldDoodadContainerDS_TableName_DoodadTemplate, 16);
	}

	// Construct template queue
	m_templateQueue = ruNEW CRuLRUQueue<CRuGUID, CRuDoodad_Template *>(512, 1024);

	if(readOnly == FALSE)
	{
		// Construct template list
		m_templateList = ruNEW CRuArrayList<CRuDoodad_Template *>();

		INT32 numTemplates = m_dataStore->GetNumRecords(m_tableHandle_DoodadTemplate);

		for(INT32 i = 0; i < numTemplates; ++i)
		{
			// Retrieve entity descriptor stream
			IRuStream *stream = m_dataStore->RetrieveByIndex(m_tableHandle_DoodadTemplate, i);
			if(stream)
			{
				// Serialize descriptor from stream
				CRuDoodad_Template *doodadTemplate = ruNEW CRuDoodad_Template();
				doodadTemplate->SerializeFrom(stream);
				delete stream;

				// Add template to list
				m_templateList->Add(doodadTemplate);
			}
		}
	}

	return TRUE;
}

BOOL CRuWorld_DoodadContainer::SaveChanges()
{
	CRuMemoryStream *bufferStream = ruNEW CRuMemoryStream();

	// Save all changed descriptors
	for(INT32 i = 0; i < m_changedTemplateList.Count(); ++i)
	{
		// Reset buffer and serialize descriptor
		bufferStream->SetStreamSize(0);
		m_changedTemplateList[i]->SerializeTo(bufferStream);

		// Store stream
		m_dataStore->Store(m_tableHandle_DoodadTemplate, m_changedTemplateList[i]->GetGUID().Data(), bufferStream);

		// Release reference to template
		ruSAFE_RELEASE(m_changedTemplateList[i]);
	}

	// Clear changed templates list
	m_changedTemplateList.Clear();

	// Delete all deleted descriptors
	for(INT32 i = 0; i < m_deletedTemplateList.Count(); ++i)
	{
		m_dataStore->Delete(m_tableHandle_DoodadTemplate, m_deletedTemplateList[i].Data());
	}

	delete bufferStream;

	return TRUE;
}

BOOL CRuWorld_DoodadContainer::CopyTo(CRuDataStore_Base *targetDataStore)
{
	if(m_dataStore)
	{
		INT32 srcTableList[1];
		srcTableList[0] = m_tableHandle_DoodadTemplate;

		// Create tables
		INT32 targetTableList[1];
		targetTableList[0] = targetDataStore->CreateTable(worldDoodadContainerDS_TableName_DoodadTemplate, 16);

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

BOOL CRuWorld_DoodadContainer::EnableDeferredLoading(BOOL enableDeferredLoading)
{
	m_enableDeferredLoading = enableDeferredLoading;
	m_enableDeferredLoading = FALSE;
	return TRUE;
}

INT32 CRuWorld_DoodadContainer::GetNumDoodadTemplates() const
{
	if(m_templateList)
	{
		return m_templateList->Count();
	}

	return 0;
}

BOOL CRuWorld_DoodadContainer::InsertDoodadTemplate(CRuDoodad_Template *doodadTemplate)
{
	// Verify that we are not in read-only mode
	if(m_templateList == NULL)
	{
		return FALSE;
	}

	// Verify that the template GUID does not already exist in the change list
	if(GetDoodadTemplateFromChangeList(doodadTemplate->GetGUID()))
	{
		return FALSE;
	}

	// Verify that the template GUID does not already exist in the template list
	if(GetDoodadTemplateFromTemplateList(doodadTemplate->GetGUID()))
	{
		return FALSE;
	}

	// Ensure the delete list does not include the template
	m_deletedTemplateList.Remove(doodadTemplate->GetGUID());

	// Insert template into change list
	m_changedTemplateList.Add(doodadTemplate);
	doodadTemplate->AddRef();

	// Insert template into template list
	m_templateList->Add(doodadTemplate);
	doodadTemplate->AddRef();

	return TRUE;
}

BOOL CRuWorld_DoodadContainer::DeleteDoodadTemplate(const CRuGUID &guid)
{
	// Verify that we are not in read-only mode
	if(m_templateList == NULL)
	{
		return FALSE;
	}

	// Verify that the template is not already in the delete list
	if(m_deletedTemplateList.Contains(guid) == FALSE)
	{
		CRuDoodad_Template *doodadTemplate = NULL;

		// Remove template from queue
		if(m_templateQueue->Get(guid, guid.HashCode(), doodadTemplate))
		{
			m_templateQueue->Remove(guid, guid.HashCode());
			ruSAFE_RELEASE(doodadTemplate);
		}

		// Remove descriptor from change list
		doodadTemplate = GetDoodadTemplateFromChangeList(guid);
		if(doodadTemplate)
		{
			m_changedTemplateList.Remove(doodadTemplate);
			ruSAFE_RELEASE(doodadTemplate);
		}

		// Remove template from template list
		doodadTemplate = GetDoodadTemplateFromTemplateList(guid);
		if(doodadTemplate)
		{
			m_templateList->Remove(doodadTemplate);
			ruSAFE_RELEASE(doodadTemplate);
		}

		// Add template GUID to delete list
		m_deletedTemplateList.Add(guid);
	}

	return TRUE;
}

BOOL CRuWorld_DoodadContainer::MarkDoodadTemplateForUpdate(const CRuGUID &guid)
{
	if(GetDoodadTemplateFromChangeList(guid) == NULL)
	{
		// Retrieve template for the specified GUID
		BOOL asyncPending = FALSE;
		CRuDoodad_Template *doodadTemplate = GetDoodadTemplate(guid, asyncPending);

		// Add template GUID to change list
		if(doodadTemplate)
		{
			m_changedTemplateList.Add(doodadTemplate);
			doodadTemplate->AddRef();
		}
	}

	return TRUE;
}

CRuDoodad_Template *CRuWorld_DoodadContainer::GetDoodadTemplate(const CRuGUID &guid, BOOL &asyncPendingOut)
{
	CRuDoodad_Template *doodadTemplate = NULL;

	// Search the change list
	if(doodadTemplate = GetDoodadTemplateFromChangeList(guid))
	{
		return doodadTemplate;
	}

	// Search the descriptor queue for matching GUID
	if(m_templateQueue->ContainsKey(guid, guid.HashCode()))
	{
		return m_templateQueue->Get(guid, guid.HashCode());
	}

	// Search the template list for matching GUID
	if(doodadTemplate = GetDoodadTemplateFromTemplateList(guid))
	{
		return doodadTemplate;
	}

	// Make sure the descriptor is not in the delete list
	if(m_deletedTemplateList.Contains(guid))
	{
		return NULL;
	}

	if(m_enableDeferredLoading)
	{
		// Queue the object for deferred loading
		m_workQueue->QueueWork(guid);

		// Denote that asynchronous read is pending
		asyncPendingOut = TRUE;
	}
	else
	{
		// Retrieve entity descriptor stream
		IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_DoodadTemplate, guid.Data());
		if(stream)
		{
			// Serialize descriptor from stream
			doodadTemplate = ruNEW CRuDoodad_Template();
			doodadTemplate->SerializeFrom(stream);
			delete stream;

			// Make space in the LRU queue
			CRuDoodad_Template *reclaimedTemplate;
			while(m_templateQueue->Reclaim(guid, guid.HashCode(), 1, reclaimedTemplate))
			{
				// Release reclaimed descriptor
				ruSAFE_RELEASE(reclaimedTemplate);
			}

			// Add descriptor to LRU queue
			m_templateQueue->Add(guid, guid.HashCode(), doodadTemplate, 1);

			return doodadTemplate;
		}
	}

	// Process the preloaded objects queue
	INT32 dequeuedObjectCount = 0;
	CRuGUID dequeuedObjectKey;
	CRuDoodad_Template *dequeuedObject = NULL;

	while((dequeuedObjectCount < 5) && (m_workQueue->DequeueResult(dequeuedObjectKey, dequeuedObject)))
	{
		const CRuGUID &dequeuedObjectGUID = dequeuedObject->GetGUID();

		// Make space in the LRU queue
		CRuDoodad_Template *reclaimedTemplate;
		while(m_templateQueue->Reclaim(guid, guid.HashCode(), 1, reclaimedTemplate))
		{
			// Release reclaimed descriptor
			ruSAFE_RELEASE(reclaimedTemplate);
		}

		// Add object to LRU queue
		m_templateQueue->Add(dequeuedObjectGUID, dequeuedObjectGUID.HashCode(), dequeuedObject, 1);

		++dequeuedObjectCount;
	}

	return NULL;
}

CRuDoodad_Template *CRuWorld_DoodadContainer::GetDoodadTemplateByIndex(INT32 index)
{
	if(m_templateList)
	{
		if(index >= 0 && index < m_templateList->Count())
		{
			return m_templateList->Get(index);
		}
	}

	return NULL;
}

BOOL CRuWorld_DoodadContainer::ClearTemplateQueue()
{
	if(m_templateQueue)
	{
		// Iterate through each doodad template and destroy
		m_templateQueue->ForAllElements(boost::bind(&CRuWorld_DoodadContainer::ClearTemplateQueueCallback, this, _1, _2));

		// Clear all doodad template entries
		m_templateQueue->Clear();
	}

	return TRUE;
}

CRuDoodad_Template *CRuWorld_DoodadContainer::GetDoodadTemplateFromChangeList(const CRuGUID &guid)
{
	for(INT32 i = 0; i < m_changedTemplateList.Count(); ++i)
	{
		if(m_changedTemplateList[i]->GetGUID() == guid)
		{
			return m_changedTemplateList[i];
		}
	}

	return NULL;
}

CRuDoodad_Template *CRuWorld_DoodadContainer::GetDoodadTemplateFromTemplateList(const CRuGUID &guid)
{
	if(m_templateList == NULL)
	{
		return NULL;
	}

	for(INT32 i = 0; i < m_templateList->Count(); ++i)
	{
		if(m_templateList->Get(i)->GetGUID() == guid)
		{
			return m_templateList->Get(i);
		}
	}

	return NULL;
}

BOOL CRuWorld_DoodadContainer::ClearTemplateQueueCallback(const void *key, void *data)
{
	CRuDoodad_Template *doodadTemplate = *((CRuDoodad_Template **) data);

	ruSAFE_RELEASE(doodadTemplate);

	return TRUE;
}

BOOL CRuWorld_DoodadContainer::WorkerFunction()
{
	CRuGUID guid(CRuGUID::Invalid);

	if(m_workQueue->DequeueWork(guid))
	{
		// Retrieve entity descriptor stream
		IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_DoodadTemplate, guid.Data());
		if(stream)
		{
			// Serialize descriptor from stream
			CRuDoodad_Template *doodadTemplate = ruNEW CRuDoodad_Template();
			doodadTemplate->SerializeFrom(stream);
			delete stream;

			m_workQueue->QueueResult(guid, doodadTemplate);
		}
		else
		{
			m_workQueue->MarkWorkFailed(guid);
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuWorld_DoodadManager::CRuWorld_DoodadManager()
:	m_doodadContainer(NULL),
	m_enumerationMarginSize(120.0f),
	m_clusterQueue(NULL)
{
}

CRuWorld_DoodadManager::~CRuWorld_DoodadManager()
{
	// Release reference to doodad container
	ruSAFE_RELEASE(m_doodadContainer);

	// Clear cluster queue
	ClearClusterQueue();
	delete m_clusterQueue;
}

BOOL CRuWorld_DoodadManager::Attach(CRuWorld_DoodadContainer *container)
{
	if(container)
	{
		// Store reference to doodad container
		container->AddRef();
		ruSAFE_RELEASE(m_doodadContainer);
		m_doodadContainer = container;

		// Create cluster queue
		m_clusterQueue = ruNEW CRuLRUQueue<CRuGUID, CRuDoodad_Cluster *>(512, 2048);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuWorld_DoodadManager::SetEnumerator(RuWorld::DoodadEnumerator enumerator)
{
	m_enumerator = enumerator;
	return TRUE;
}

BOOL CRuWorld_DoodadManager::ClearEnumerationCache()
{
	// Reset enumeration bounds
	m_enumerationBounds.Set(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	// Clear all clusters without deleting them
	m_clusterQueue->ForAllElements(boost::bind(&CRuWorld_DoodadManager::ClearClusterCallback, this, _1, _2));

	// Clear all entities from the entity container
	this->ClearEntities();

	return TRUE;
}

BOOL CRuWorld_DoodadManager::UpdateEnumeration(const CRuAABB &enumationBounds)
{
	if(m_enumerationBounds.Contains(enumationBounds) == FALSE)
	{
		// Clear enumeration cache
		this->ClearEnumerationCache();

		// Set enumeration bounds to the specified bounds plus the margin
		m_enumerationBounds = enumationBounds;
		m_enumerationBounds.Minima().m_x -= m_enumerationMarginSize;
		m_enumerationBounds.Minima().m_y -= m_enumerationMarginSize;
		m_enumerationBounds.Minima().m_z -= m_enumerationMarginSize;
		m_enumerationBounds.Maxima().m_x += m_enumerationMarginSize;
		m_enumerationBounds.Maxima().m_y += m_enumerationMarginSize;
		m_enumerationBounds.Maxima().m_z += m_enumerationMarginSize;

		// Execute enumerator to get cluster list
		CRuArrayList<INT32> *keyList;
		CRuArrayList<CRuWorld_DoodadCluster *> *clusterList;
		m_enumerator(&keyList, &clusterList);

		// Iterate through clusters
		for(INT32 i = 0, numKeys = keyList->Count(); i < numKeys; ++i)
		{
			m_clusterWorkQueue.Queue(ClusterTuple(keyList->Get(i), clusterList->Get(i)));
		}




/*
		// Iterate through clusters
		for(INT32 i = 0, numKeys = keyList->Count(); i < numKeys; ++i)
		{
			BOOL asyncPending = FALSE;
			CRuDoodad_Cluster *doodadCluster = GetDoodadCluster(clusterList->Get(i)->m_guid, asyncPending);

			if(asyncPending)
			{
				continue;
			}

			if(doodadCluster)
			{
				doodadCluster->InsertDoodadList(keyList->Get(i), clusterList->Get(i)->m_doodads);
			}
		}
*/
		this->RemoveAllChildren();

		// Add all active clusters to entity container
		m_clusterQueue->ForAllElements(boost::bind(&CRuWorld_DoodadManager::EnumerateActiveClusterCallback, this, _1, _2));
	}

	return TRUE;
}

BOOL CRuWorld_DoodadManager::Update(REAL elapsedTime)
{
	INT32 totalClusterCount = m_clusterWorkQueue.Count();
	INT32 curClusterCount = 0;

	while(curClusterCount < totalClusterCount && m_clusterWorkQueue.Count() > 0)
	{
		++curClusterCount;

		ClusterTuple tuple = m_clusterWorkQueue.Dequeue();

		BOOL asyncPending = FALSE;
		CRuDoodad_Cluster *doodadCluster = GetDoodadCluster(tuple.m_cluster->m_guid, asyncPending);

		if(asyncPending)
		{
			m_clusterWorkQueue.Queue(tuple);
			continue;
		}

		if(doodadCluster)
		{
			doodadCluster->InsertDoodadList(tuple.m_key, tuple.m_cluster->m_doodads);
		}
	}

	// Add all active clusters to entity container
	m_clusterQueue->ForAllElements(boost::bind(&CRuWorld_DoodadManager::EnumerateActiveClusterCallback, this, _1, _2));

	return CRuEntity_Container_Impl::Update(elapsedTime);
}

BOOL CRuWorld_DoodadManager::ClearClusterQueue()
{
	if(m_clusterQueue)
	{
		// Iterate through each doodad cluster and destroy
		m_clusterQueue->ForAllElements(boost::bind(&CRuWorld_DoodadManager::ClearClusterQueueCallback, this, _1, _2));

		// Clear all doodad cluster entries
		m_clusterQueue->Clear();
	}

	return TRUE;
}

CRuDoodad_Cluster *CRuWorld_DoodadManager::GetDoodadCluster(const CRuGUID &guid, BOOL &asyncPendingOut)
{
	CRuDoodad_Cluster *doodadCluster = NULL;

	// Attempt to locate the doodad cluster in the queue
	if(m_clusterQueue->Get(guid, guid.HashCode(), doodadCluster))
	{
		return doodadCluster;
	}

	// Get doodad template
	CRuDoodad_Template *doodadTemplate = m_doodadContainer->GetDoodadTemplate(guid, asyncPendingOut);

	if(doodadTemplate)
	{
		// Create new cluster
		doodadCluster = ruNEW CRuDoodad_Cluster();
		doodadCluster->SetDoodadTemplate(doodadTemplate);

		// Make space in the LRU queue
		CRuDoodad_Cluster *reclaimedCluster;
		while(m_clusterQueue->Reclaim(guid, guid.HashCode(), 1, reclaimedCluster))
		{
			// Release reclaimed cluster
			ruSAFE_RELEASE(reclaimedCluster);
		}

		// Add descriptor to LRU queue
		m_clusterQueue->Add(guid, guid.HashCode(), doodadCluster, 1);

		return doodadCluster;
	}

	return NULL;
}

BOOL CRuWorld_DoodadManager::ClearClusterQueueCallback(const void *key, void *data)
{
	CRuDoodad_Cluster *cluster = *((CRuDoodad_Cluster **) data);

	ruSAFE_RELEASE(cluster);

	return TRUE;
}

BOOL CRuWorld_DoodadManager::ClearClusterCallback(const void *key, void *data)
{
	CRuDoodad_Cluster *cluster = *((CRuDoodad_Cluster **) data);

	cluster->RemoveAllDoodadLists();

	return TRUE;
}

BOOL CRuWorld_DoodadManager::EnumerateActiveClusterCallback(const void *key, void *data)
{
	CRuDoodad_Cluster *cluster = *((CRuDoodad_Cluster **) data);

	// Force update of cluster
	cluster->Update(0.0f);

	// If cluster has active models, add it to entity container
	if(cluster->GetFirstChild() != NULL)
	{
		// Force update world bounds
		cluster->GetWorldBounds();

		// Add to graph
		this->AddChild(cluster);
	}


	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
