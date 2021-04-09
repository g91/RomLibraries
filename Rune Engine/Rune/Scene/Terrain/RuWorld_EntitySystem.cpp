#include "../../Scene/Terrain/RuWorld_EntitySystem.h"

#include "../../Helper/RuHelper_Entity.h"
#include "../../Scene/Particle/RuEntity_Particle_Visualizer_Sprite.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuWorld_CellLink, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_CellLink, "CRuWorld_CellLink", "IRuObject")
ruCLASSGUID_DEFINE(CRuWorld_CellLink, 0x1CB23FAD, 0xEF37462d, 0xAD44BDE3, 0xD89BA4C2)

ruRTTI_DEFINE_SUBCLASS(CRuWorld_ObjectCell, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_ObjectCell, "CRuWorld_ObjectCell", "IRuObject")
ruCLASSGUID_DEFINE(CRuWorld_ObjectCell, 0x13DCD6DE, 0x08004463, 0x9CFD9491, 0x4B602444)

ruRTTI_DEFINE_SUBCLASS(CRuWorld_ObjectContainer, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_ObjectContainer, "CRuWorld_ObjectContainer", "IRuObject")
ruCLASSGUID_DEFINE(CRuWorld_ObjectContainer, 0x83E36878, 0x3C324a32, 0x97AD4DD7, 0x6A7E7350)

ruRTTI_DEFINE_SUBCLASS(CRuWorld_ObjectManager, CRuEntity_Container_Impl)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_ObjectManager, "CRuWorld_ObjectManager", "CRuEntity_Container_Impl")
ruCLASSGUID_DEFINE(CRuWorld_ObjectManager, 0xBF6CF945, 0x01654a7e, 0x8328944D, 0x087D9ACA)

// ************************************************************************************************************************************************************

static const char *worldObjectContainerDS_TableName_Cells = "worldObjectContainer_Cells";
static const char *worldObjectContainerDS_TableName_Objects = "worldEntityContainer_EntityDescriptor";	// need to rename this.. somehow...
static const char *worldObjectContainerDS_TableName_ObjectLink = "worldObjectContainer_ObjectLink";

// ************************************************************************************************************************************************************

CRuWorld_CellLink::CRuWorld_CellLink()
{
}

CRuWorld_CellLink::~CRuWorld_CellLink()
{
}

IRuObject *CRuWorld_CellLink::Clone(IRuObject *clonedObject)
{
	return NULL;
}

BOOL CRuWorld_CellLink::SerializeFrom(IRuStream *stream)
{
	// Validate GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeFrom(stream);

	// Read number of links
	INT32 linkCount = 0;
	stream->Read(&linkCount, sizeof(INT32));

	// Read each object GUID
	for(INT32 i = 0; i < linkCount; ++i)
	{
		UINT32 linkCellKey = 0xFFFFFFFF;
		stream->Read(&linkCellKey, sizeof(UINT32));

		m_cellLinks.Add(linkCellKey);
	}

	return TRUE;
}

BOOL CRuWorld_CellLink::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeTo(stream);

	// Write number of links
	INT32 linkCount = m_cellLinks.Count();
	stream->Write(&linkCount, sizeof(INT32));

	// Write each object GUID
	for(INT32 i = 0; i < linkCount; ++i)
	{
		stream->Write(&m_cellLinks[i], sizeof(UINT32));
	}

	return TRUE;
}

INT32 CRuWorld_CellLink::GetLinkCount()
{
	return m_cellLinks.Count();
}

UINT32 CRuWorld_CellLink::GetLink(INT32 index)
{
	return m_cellLinks[index];
}

BOOL CRuWorld_CellLink::InsertLink(UINT32 link)
{
	if(m_cellLinks.Contains(link) == FALSE)
	{
		m_cellLinks.Add(link);
		return TRUE;
	}

	return FALSE;
}

BOOL CRuWorld_CellLink::DeleteLink(UINT32 link)
{
	if(m_cellLinks.Contains(link))
	{
		m_cellLinks.Remove(link);
		return TRUE;
	}

	return FALSE;
}

BOOL CRuWorld_CellLink::ClearLinks()
{
	m_cellLinks.Clear();
	return TRUE;
}

// ************************************************************************************************************************************************************

CRuWorld_ObjectCell::CRuWorld_ObjectCell()
:	m_cellKey(0),
	m_dirty(FALSE)
{
}

CRuWorld_ObjectCell::~CRuWorld_ObjectCell()
{
}

IRuObject *CRuWorld_ObjectCell::Clone(IRuObject *clonedObject)
{
	return NULL;
}

BOOL CRuWorld_ObjectCell::SerializeFrom(IRuStream *stream)
{
	// Validate GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeFrom(stream);

	// Read cell key
	stream->Read(&m_cellKey, sizeof(UINT32));

	// Read cell bounds
	::SerializeTo(stream, &m_cellBounds);

	// Read number of objects
	INT32 objCount = 0;
	stream->Read(&objCount, sizeof(INT32));

	// Read each object GUID
	UINT32 guid[4];
	for(INT32 i = 0; i < objCount; ++i)
	{
		// Read GUID data
		stream->Read(guid, sizeof(UINT32) * 4);

		// Insert into object GUID list
		m_objectGUIDs.Add(CRuGUID(guid[0], guid[1], guid[2], guid[3]));
	}

	return TRUE;
}

BOOL CRuWorld_ObjectCell::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeTo(stream);

	// Write cell key
	stream->Write(&m_cellKey, sizeof(UINT32));

	// Write cell bounds
	::SerializeTo(stream, &m_cellBounds);

	// Write number of objects
	INT32 objCount = m_objectGUIDs.Count();
	stream->Write(&objCount, sizeof(INT32));

	// Write each object GUID
	for(INT32 i = 0; i < objCount; ++i)
	{
		stream->Write(m_objectGUIDs[i].Data(), m_objectGUIDs[i].Size());
	}

	return TRUE;
}

BOOL CRuWorld_ObjectCell::Initialize(UINT32 cellKey, const CRuAABB &cellBounds)
{
	m_cellKey = cellKey;
	m_cellBounds = cellBounds;

	MarkAsDirty();

	return TRUE;
}

UINT32 CRuWorld_ObjectCell::GetCellKey() const
{
	return m_cellKey;
}

const CRuAABB &CRuWorld_ObjectCell::GetCellBounds() const
{
	return m_cellBounds;
}

BOOL CRuWorld_ObjectCell::IsDirty() const
{
	return m_dirty;
}

void CRuWorld_ObjectCell::MarkAsNotDirty()
{
	m_dirty = FALSE;
}

void CRuWorld_ObjectCell::MarkAsDirty()
{
	m_dirty = TRUE;
}

BOOL CRuWorld_ObjectCell::InsertObjectGUID(const CRuGUID &guid)
{
	for(INT32 i = 0; i < m_objectGUIDs.Count(); ++i)
	{
		if(m_objectGUIDs[i] == guid)
		{
			return FALSE;
		}
	}

	m_objectGUIDs.Add(guid);

	MarkAsDirty();

	return TRUE;
}

BOOL CRuWorld_ObjectCell::RemoveObjectGUID(const CRuGUID &guid)
{
	for(INT32 i = 0; i < m_objectGUIDs.Count(); ++i)
	{
		if(m_objectGUIDs[i] == guid)
		{
			m_objectGUIDs.RemoveAt(i);

			MarkAsDirty();

			return TRUE;
		}
	}

	return FALSE;
}

INT32 CRuWorld_ObjectCell::GetObjectGUIDCount()
{
	return m_objectGUIDs.Count();
}

const CRuGUID &CRuWorld_ObjectCell::GetObjectGUID(INT32 index)
{
	return m_objectGUIDs[index];
}

// ************************************************************************************************************************************************************

CRuWorld_ObjectContainer::CRuWorld_ObjectContainer(const CRuAABB &containerBounds, REAL cellSize)
:	m_containerBounds(containerBounds),
	m_cellSize(cellSize),
	m_cellTree(NULL),
	m_cellHash(NULL),
	m_enumeratedObjectGUIDHash(1024),
	m_dataStore(NULL),
	m_objectQueue(NULL),
	m_instanceQueue(NULL),
	m_enableDeferredLoading(FALSE)
{
	// Setup special event filter
	m_filteredObjects = ruNEW CRuKeyHash<CRuGUID>(1024);

	// Calculate number of cells in the X and Z directions
	CRuVector3 containerSize = m_containerBounds.Maxima() - m_containerBounds.Minima();
	INT32 numXCells = static_cast<INT32>(ceil(containerSize.m_x / cellSize));
	INT32 numZCells = static_cast<INT32>(ceil(containerSize.m_z / cellSize));

	// Initialize cell tree
	m_cellTree = ruNEW CRuWorld_ChunkTree();
	m_cellTree->Initialize(m_containerBounds, 32);

	// Insert cells into the partitioning structure
	for(INT32 z = 0; z < numZCells; ++z)
	{
		for(INT32 x = 0; x < numXCells; ++x)
		{
			// Calculate current cell bounds
			CRuAABB cellBounds;
			cellBounds.Minima() = CRuVector3(x * cellSize, 0.0f, z * cellSize) + m_containerBounds.Minima();
			cellBounds.Maxima() = cellBounds.Minima() + CRuVector3(cellSize, m_containerBounds.Maxima().m_y - m_containerBounds.Minima().m_y, cellSize);

			// Insert cell into tree
			m_cellTree->InsertChunk(x, z, cellBounds);
		}
	}

	// Initialize background work queues
	m_cellWorkQueue = ruNEW CRuThread_WorkQueue<UINT32, CRuWorld_ObjectCell *>(512, 512, 512);
	m_workQueue = ruNEW CRuThread_WorkQueue<CRuGUID, IRuObject *>(512, 512, 512);

	// Set last GC time to the current time and reset time since last GC
	m_lastGCTime = timeGetTime();
	m_timeSinceLastGC = 0;

	// Register worker function with worker thread
	g_ruGlobalWorkerThread->RegisterWorkerFunction(this->GetGUID(), boost::bind(&CRuWorld_ObjectContainer::WorkerFunction, this));
}

CRuWorld_ObjectContainer::~CRuWorld_ObjectContainer()
{
	// Unregister worker function
	g_ruGlobalWorkerThread->UnregisterWorkerFunction(this->GetGUID());

	// Clear the background cell work queue
	UINT32 orphanedCellKey;
	CRuWorld_ObjectCell *orphanedCell = NULL;

	while(m_cellWorkQueue->DequeueResult(orphanedCellKey, orphanedCell))
	{
		ruSAFE_RELEASE(orphanedCell);
	}

	delete m_cellWorkQueue;

	// Clear the background work queue
	CRuGUID orphanedResultKey;
	IRuObject *orphanedResult = NULL;
	while(m_workQueue->DequeueResult(orphanedResultKey, orphanedResult))
	{
		ruSAFE_RELEASE(orphanedResult);
	}

	delete m_workQueue;

	// Release data store
	ruSAFE_RELEASE(m_dataStore);

	// Release cell hash
	ClearCellHash();
	delete m_cellHash;

	// Release object queue
	ClearObjectQueue();
	delete m_objectQueue;

	// Release instancing queue
	ClearInstanceQueue();
	delete m_instanceQueue;

	// Release any remaining references in the change list
	for(INT32 i = 0; i < m_changedObjectList.Count(); ++i)
	{
		ruSAFE_RELEASE(m_changedObjectList[i]);
	}

	// Release the cell tree
	ruSAFE_RELEASE(m_cellTree);

	// Delete special event filter resources
	for(INT32 i = 0; i < m_specialEventFilter.Count(); ++i)
	{
		ruSAFE_DELETE_ARRAY(m_specialEventFilter[i]);
	}
	m_specialEventFilter.Clear();
	ruSAFE_DELETE(m_filteredObjects);
}

INT32 CRuWorld_ObjectContainer::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuObject::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	m_temp_Footprint = 0;
	m_temp_EnumHash = enumHash;
	m_objectQueue->ForAllElements(boost::bind(&CRuWorld_ObjectContainer::ObjectQueueMemoryFootprintCallback, this, _1, _2));
	m_instanceQueue->ForAllElements(boost::bind(&CRuWorld_ObjectContainer::InstanceQueueMemoryFootprintCallback, this, _1, _2));
	footprint += m_temp_Footprint;

	return footprint;
}

BOOL CRuWorld_ObjectContainer::Attach(CRuDataStore_Base *dataStore)
{
	// Save reference to data store
	m_dataStore = dataStore;
	m_dataStore->AddRef();

	// Get data tables
	m_tableHandle_Cells = m_dataStore->CreateTable(worldObjectContainerDS_TableName_Cells, 4);
	m_tableHandle_Objects = m_dataStore->CreateTable(worldObjectContainerDS_TableName_Objects, 16);

	// Construct queues & hashes
	m_cellHash = ruNEW CRuHashMap<UINT32, CRuWorld_ObjectCell *>(1024);
	m_objectQueue = ruNEW CRuLRUQueue<CRuGUID, IRuObject *>(8192, 16384);
	m_instanceQueue = ruNEW CRuHashMap<CRuGUID, InstanceDesc>(4096);

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::SaveChanges()
{
	CRuMemoryStream *bufferStream = ruNEW CRuMemoryStream();

	// Save all dirty cells
	SaveCells();

	// Save all changed descriptors
	for(INT32 i = 0; i < m_changedObjectList.Count(); ++i)
	{
		// ERROR CHECKING
		assert(m_deletedObjectList.Contains(m_changedObjectList[i]->GetGUID()) == FALSE);

		// Reset buffer and serialize descriptor
		bufferStream->SetStreamSize(0);
		m_changedObjectList[i]->SerializeTo(bufferStream);

		// Store stream
		m_dataStore->Store(m_tableHandle_Objects, m_changedObjectList[i]->GetGUID().Data(), bufferStream);

		// Release reference to object
		ruSAFE_RELEASE(m_changedObjectList[i]);
	}

	// Clear changed object list
	m_changedObjectList.Clear();

	// Delete all deleted descriptors
	for(INT32 i = 0; i < m_deletedObjectList.Count(); ++i)
	{
		m_dataStore->Delete(m_tableHandle_Objects, m_deletedObjectList[i].Data());
	}

	// Clear deleted object GUIDS
	m_deletedObjectList.Clear();

	delete bufferStream;

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::CopyTo(CRuDataStore_Base *targetDataStore)
{
	if(m_dataStore)
	{
		INT32 srcTableList[2];
		srcTableList[0] = m_tableHandle_Cells;
		srcTableList[1] = m_tableHandle_Objects;

		// Create tables
		INT32 targetTableList[2];
		targetTableList[0] = targetDataStore->CreateTable(worldObjectContainerDS_TableName_Cells, 4);
		targetTableList[1] = targetDataStore->CreateTable(worldObjectContainerDS_TableName_Objects, 16);

		// Copy data in each table
		for(INT32 i = 0; i < 2; ++i)
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

BOOL CRuWorld_ObjectContainer::EnableDeferredLoading(BOOL enableDeferredLoading)
{
	m_enableDeferredLoading = enableDeferredLoading;

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::InsertObject(IRuObject *object, const CRuSphere &objectBounds)
{
	// Verify that the object GUID does not already exist in the change list
	if(GetObjectFromChangeList(object->GetGUID()))
	{
		return FALSE;
	}

	// Verify that the object GUID is either deleted or does not exist at all in the data store
	if(m_deletedObjectList.Contains(object->GetGUID()) == FALSE)
	{
		IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_Objects, object->GetGUID().Data());
		if(stream)
		{
			delete stream;
			return FALSE;
		}
	}

	// Get the CRuWorld_CellLink structure from the object
	CRuWorld_CellLink *cellLink = GetCellLinkObject(object);

	// Get a list of keys to cells to be updated
	CRuArrayList<DWORD> cellKeyList;
	m_cellTree->EnumerateChunks(objectBounds, cellKeyList);

	// Process the list of cell keys to ensure that all cells are loaded
	for(INT32 i = 0; i < cellKeyList.Count(); ++i)
	{
		// Wait until the cell is loaded
		while(this->GetCell(cellKeyList[i]) == NULL)
		{
			Sleep(1);
		}
	}

	// Insert the object GUID into all cells
	for(INT32 i = 0; i < cellKeyList.Count(); ++i)
	{
		// Insert object into cell
		CRuWorld_ObjectCell *cell = this->GetCell(cellKeyList[i]);
		cell->InsertObjectGUID(object->GetGUID());

		// Add cell key to the object's cell link structure
		cellLink->InsertLink(cellKeyList[i]);
	}

	// Ensure the delete list does not include the object
	m_deletedObjectList.Remove(object->GetGUID());

	// Insert object into change list
	m_changedObjectList.Add(object);

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::DeleteObject(const CRuGUID &guid, const CRuSphere &objectBounds)
{
	if(m_deletedObjectList.Contains(guid) == FALSE)
	{
		// Get the object
		BOOL asyncPending;
		IRuObject *object = GetObject(guid, asyncPending);
		while(object == NULL && asyncPending)
		{
			Sleep(1);
			object = GetObject(guid, asyncPending);
		}

		// Get a list of keys to cells to be updated
		CRuArrayList<DWORD> cellKeyList;
		m_cellTree->EnumerateChunks(objectBounds, cellKeyList);

		// Process the list of cell keys to ensure that all cells are loaded
		for(INT32 i = 0; i < cellKeyList.Count(); ++i)
		{
			// Wait until the cell is loaded
			while(this->GetCell(cellKeyList[i]) == NULL)
			{
				Sleep(1);
			}
		}

		// Remove the object GUID from all cells
		for(INT32 i = 0; i < cellKeyList.Count(); ++i)
		{
			CRuWorld_ObjectCell *cell = this->GetCell(cellKeyList[i]);
			cell->RemoveObjectGUID(guid);
		}

		if(object)
		{
			// Get the CRuWorld_CellLink structure from the object
			CRuWorld_CellLink *cellLink = GetCellLinkObject(object);

			// Iterate through all cell links and delete the links
			for(INT32 i = 0; i < cellLink->GetLinkCount(); ++i)
			{
				CRuWorld_ObjectCell *cell = this->GetCell(cellLink->GetLink(i));

				// Make this operation synchronous
				while(cell == NULL)
				{
					Sleep(1);
					cell = this->GetCell(cellLink->GetLink(i));
				}

				cell->RemoveObjectGUID(guid);
			}

			cellLink->ClearLinks();
		}

		// Remove object from change list
		object = GetObjectFromChangeList(guid);
		if(object)
		{
			m_changedObjectList.Remove(object);
			ruSAFE_RELEASE(object);
		}

		// Remove object from queue
		m_objectQueue->Remove(guid, guid.HashCode());

		// Add object GUID to delete list
		m_deletedObjectList.Add(guid);
	}

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::UpdateObjectCellLinks(IRuObject *object, const CRuSphere &objectBounds)
{
	if(m_deletedObjectList.Contains(object->GetGUID()) == FALSE)
	{
		// Get a list of keys to cells to be updated
		CRuArrayList<DWORD> cellKeyList;
		m_cellTree->EnumerateChunks(objectBounds, cellKeyList);

		// Process the list of cell keys to ensure that all cells are loaded
		for(INT32 i = 0; i < cellKeyList.Count(); ++i)
		{
			// Wait until the cell is loaded
			while(this->GetCell(cellKeyList[i]) == NULL)
			{
				Sleep(1);
			}
		}

		// Get the CRuWorld_CellLink structure from the object
		CRuWorld_CellLink *cellLink = GetCellLinkObject(object);

		// Iterate through all cell links and delete the links
		for(INT32 i = 0; i < cellLink->GetLinkCount(); ++i)
		{
			CRuWorld_ObjectCell *cell = this->GetCell(cellLink->GetLink(i));

			// Make this operation synchronous
			while(cell == NULL)
			{
				Sleep(1);
				cell = this->GetCell(cellLink->GetLink(i));
			}

			cell->RemoveObjectGUID(object->GetGUID());
		}

		// Clear all existing cell links
		cellLink->ClearLinks();

		// Insert the object GUID into all cells
		for(INT32 i = 0; i < cellKeyList.Count(); ++i)
		{
			// Insert object into cell
			CRuWorld_ObjectCell *cell = this->GetCell(cellKeyList[i]);
			cell->InsertObjectGUID(object->GetGUID());

			// Add cell key to the object's cell link structure
			cellLink->InsertLink(cellKeyList[i]);
		}

		// Insert object into change list
		m_changedObjectList.Add(object);
	}

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::MarkObjectForUpdate(const CRuGUID &guid)
{
	if(GetObjectFromChangeList(guid) == NULL)
	{
		// Retrieve object for the specified GUID
		IRuObject *object = NULL;
		BOOL asyncPending = TRUE;

		// Wait for the read request to end
		while(asyncPending)
		{
			object = GetObject(guid, asyncPending);
		}

		// Add object GUID to change list
		if(object)
		{
			object->AddRef();
			m_changedObjectList.Add(object);
		}
	}

	return TRUE;
}

#ifdef KALYDO

void CRuWorld_ObjectContainer::DownloadCallback(const BYTE* key, void* userData)
{
	std::pair<const CRuGUID*, CRuWorld_ObjectContainer*>* pair = reinterpret_cast<std::pair<const CRuGUID*, CRuWorld_ObjectContainer*>*>(userData);

	pair->second->m_workQueue->QueueWork(*pair->first);

	delete pair;
}

void CRuWorld_ObjectContainer::KalydoQueue(const CRuGUID &guid)
{
	std::pair<const CRuGUID*, CRuWorld_ObjectContainer*>* pair = new std::pair<const CRuGUID*, CRuWorld_ObjectContainer*>(&guid, this);

	if (m_dataStore->KalydoDownloadData(m_tableHandle_Objects, guid.Data(), pair, &CRuWorld_ObjectContainer::DownloadCallback) == TRUE)
		return;

	delete pair;

	m_workQueue->QueueWork(guid);
}

#endif // KALYDO

IRuObject *CRuWorld_ObjectContainer::GetObject(const CRuGUID &guid, BOOL &asyncPendingOut)
{
	IRuObject *object = NULL;

	// Denote that there is no asynchronous read pending
	asyncPendingOut = FALSE;

	// Search the change list
	if(object = GetObjectFromChangeList(guid))
	{
		return object;
	}

	// Search the object queue for matching GUID
	if(m_objectQueue->ContainsKey(guid, guid.HashCode()))
	{
		return m_objectQueue->Get(guid, guid.HashCode());
	}

	// Make sure the object is not in the delete list
	if(m_deletedObjectList.Contains(guid))
	{
		return NULL;
	}

	// If a previous attempt to load the object failed, then do not try to load it again
	if(m_workQueue->WorkFailed(guid, FALSE))
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
		// Retrieve object stream
		IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_Objects, guid.Data());
		if(stream)
		{
			// Create object from stream using object factory
			IRuObject *object = RuObjectFactory().CreateObject(stream);
			delete stream;

			if(object)
			{
				// Make space in the LRU queue
				IRuObject *reclaimedObject;
				while(m_objectQueue->Reclaim(guid, guid.HashCode(), 1, reclaimedObject))
				{
					// Release reclaimed object
					ruSAFE_RELEASE(reclaimedObject);
				}

				// Add object to LRU queue
				m_objectQueue->Add(guid, guid.HashCode(), object, 1);

				return object;
			}
		}
		else
		{
			m_workQueue->MarkWorkFailed(guid);
		}
	}

	// Process the preloaded objects queue
	INT32 dequeuedObjectCount = 0;
	CRuGUID dequeuedObjectKey;
	IRuObject *dequeuedObject = NULL;

	while((dequeuedObjectCount < 5000) && (m_workQueue->DequeueResult(dequeuedObjectKey, dequeuedObject)))
	{
		const CRuGUID &dequeuedObjectGUID = dequeuedObject->GetGUID();

		// Make space in the LRU queue
		IRuObject *reclaimedObject;
		while(m_objectQueue->Reclaim(dequeuedObjectGUID, dequeuedObjectGUID.HashCode(), 1, reclaimedObject))
		{
			// Release reclaimed object
			ruSAFE_RELEASE(reclaimedObject);
		}

		// Add object to LRU queue
		m_objectQueue->Add(dequeuedObjectGUID, dequeuedObjectGUID.HashCode(), dequeuedObject, 1);

		++dequeuedObjectCount;
	}

	return NULL;
}

IRuObject *CRuWorld_ObjectContainer::GetObjectWithFilter(const CRuGUID &guid, BOOL &asyncPendingOut)
{
	// Get object normally
	IRuObject *obj = GetObject(guid, asyncPendingOut);

	// Run object against filter
	obj = FilterObject(obj, guid);

	return obj;
}

IRuObject *CRuWorld_ObjectContainer::PickObject(const CRuSegment3 &segment, CRuVector3 &intPtOut, float &intDistOut)
{
	REAL nearestDistance = FLT_MAX;
	CRuVector3 nearestPoint;
	IRuObject *nearestObject = NULL;

	// Build collision query
	CRuCollisionQuery_Segment_Nearest segQuery;
	CRuMatrix4x4 identityMatrix;
	segQuery.m_worldTransform = &identityMatrix;
	segQuery.m_invWorldTransform = &identityMatrix;
	segQuery.m_ignoreCollisionFlag = TRUE;
	segQuery.m_colSegment = segment;

	for(INT32 i = 0; i < m_enumeratedObjectGUIDList.Count(); ++i)
	{
		// Retrieve object
		BOOL asyncPending;
		IRuObject *obj = GetObject(m_enumeratedObjectGUIDList[i], asyncPending);
		if(obj)
		{
			if(obj->GetType() == CRuWorld_EntityDescriptor::Type())
			{
				CRuEntity *instancedEntity = GetEntityInstance(m_enumeratedObjectGUIDList[i], asyncPending);
				if(instancedEntity)
				{
					segQuery.m_positiveHit = FALSE;
					segQuery.m_collisionT = FLT_MAX;
					instancedEntity->QueryCollision(&segQuery);

					if(segQuery.m_positiveHit)
					{
						if(segQuery.m_collisionT < nearestDistance)
						{
							nearestDistance = segQuery.m_collisionT;
							nearestPoint = segQuery.m_collisionPoint;
							nearestObject = obj;
						}
					}
				}
			}
		}
	}

	// Return query results
	if(nearestDistance < FLT_MAX)
	{
		intPtOut = nearestPoint;
		intDistOut = nearestDistance;

		// Run object against filter
		if(nearestObject)
		{
			nearestObject = FilterObject(nearestObject, nearestObject->GetGUID());
		}

		return nearestObject;
	}

	return NULL;
}

BOOL CRuWorld_ObjectContainer::SetSpecialEventFilter(const char *filter)
{
	// Save new special event filter
	for(INT32 i = 0; i < m_specialEventFilter.Count(); ++i)
	{
		ruSAFE_DELETE_ARRAY(m_specialEventFilter[i]);
	}
	m_specialEventFilter.Clear();

	if(filter)
	{
		CRuStringTokenizer filterTokenizer;
		filterTokenizer.Tokenize(filter, ";");

		for(INT32 i = 0; i < filterTokenizer.GetNumTokens(); ++i)
		{
			char *token = ruNEW char [filterTokenizer.GetToken(i).GetLength() + 1];
			strcpy(token, filterTokenizer.GetToken(i));
			m_specialEventFilter.Add(token);
		}
	}

	// Clear filter hash
	m_filteredObjects->Clear();

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::EnumerateAllObjectsByType(const CRuRTTI &objectType, CRuArrayList<IRuObject *> &objectsOut)
{
	INT32 numObjects = m_dataStore->GetNumRecords(m_tableHandle_Objects);

	for(INT32 i = 0; i < numObjects; ++i)
	{
		// Retrieve key and data
		IRuStream *stream = m_dataStore->RetrieveByIndex(m_tableHandle_Objects, i);

		if(stream)
		{
			// Create object from stream using object factory
			IRuObject *object = RuObjectFactory().CreateObject(stream);
			delete stream;

			if(object)
			{
				// If object is of the specified subtype, add it to the output list, otherwise free it
				if(object->GetType().IsSubClassOf(objectType) && m_deletedObjectList.Contains(object->GetGUID()) == FALSE)
				{
					objectsOut.Add(object);
				}
				else
				{
					ruSAFE_RELEASE(object);
				}
			}
		}
	}

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::EnumerateVisibleObjects(CRuArrayList<IRuObject *> &objectsOut)
{
	for(INT32 i = 0; i < m_enumeratedObjectGUIDList.Count(); ++i)
	{
		BOOL asyncPending = FALSE;
		IRuObject *obj = GetObject(m_enumeratedObjectGUIDList[i], asyncPending);

		if(obj && !asyncPending)
		{
			objectsOut.Add(obj);
		}
	}

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::EnumerateVisibleObjectsByType(CRuArrayList<IRuObject *> &objectsOut, const CRuRTTI &objectType)
{
	for(INT32 i = 0; i < m_enumeratedObjectGUIDList.Count(); ++i)
	{
		BOOL asyncPending = FALSE;
		IRuObject *obj = GetObject(m_enumeratedObjectGUIDList[i], asyncPending);

		if(obj && !asyncPending && obj->GetType().IsSubClassOf(objectType))
		{
			objectsOut.Add(obj);
		}
	}

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::EnumerateVisibleObjectsByTypeWithFilter(CRuArrayList<IRuObject *> &objectsOut, const CRuRTTI &objectType)
{
	for(INT32 i = 0; i < m_enumeratedObjectGUIDList.Count(); ++i)
	{
		BOOL asyncPending = FALSE;
		IRuObject *obj = GetObjectWithFilter(m_enumeratedObjectGUIDList[i], asyncPending);

		if(obj && !asyncPending && obj->GetType().IsSubClassOf(objectType))
		{
			objectsOut.Add(obj);
		}
	}

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::UpdateEnumeration(const CRuAABB &enumerationBounds)
{
	// Get a list of keys to visible cells
	CRuArrayList<DWORD> cellKeyList;
	m_cellTree->EnumerateChunks(enumerationBounds, cellKeyList);

	// Process the list of cell keys
	for(INT32 i = 0; i < cellKeyList.Count(); ++i)
	{
		// Insert cell key into the enumeration queue
		m_cellEumerationQueue.Queue(cellKeyList[i]);
	}

	m_enumeratedCellCount = m_cellEumerationQueue.Count();
	m_enumeratedCells.Clear();

	// Clear the object enumeration list & hash
	m_enumeratedObjectGUIDList.Clear();
	m_enumeratedObjectGUIDHash.Clear();

	m_nextEnumeratedObjectIndex = 0;

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::Update(REAL elapsedTime)
{
	// Process the cell enumeration queue
	for(INT32 i = 0, j = m_cellEumerationQueue.Count(); i < j; ++i)
	{
		// Dequeue the cell key and attempt to load the cell
		UINT32 cellKey = m_cellEumerationQueue.Dequeue();
		CRuWorld_ObjectCell *cell = this->GetCell(cellKey);

		if(cell)
		{
			// Add the loaded cell into the list of enumerated cells
			m_enumeratedCells.Add(cell);

			// Process the loaded cell's object GUIDs
			for(INT32 k = 0; k < cell->GetObjectGUIDCount(); ++k)
			{
				// Get current object GUID
				const CRuGUID &objGUID = cell->GetObjectGUID(k);

				// If the object GUID is not already in the enumeration, insert it
				if(m_enumeratedObjectGUIDHash.ContainsKey(objGUID, objGUID.HashCode()) == FALSE)
				{
					m_enumeratedObjectGUIDList.Add(objGUID);
					m_enumeratedObjectGUIDHash.Add(objGUID, objGUID.HashCode());
				}
			}
		}
		else
		{
			// Cell has not been loaded yet, so push the cell key back onto the queue
			m_cellEumerationQueue.Queue(cellKey);
		}
	}

	// Garbage collect the instance queue
	GarbageCollectInstanceQueue();

	return TRUE;
}

#ifdef KALYDO

void CRuWorld_ObjectContainer::DownloadCellCallback(const BYTE* key, void* userData)
{
	CRuWorld_ObjectContainer* oc = reinterpret_cast<CRuWorld_ObjectContainer*>(userData);
	
	oc->m_cellWorkQueue->QueueWork(*reinterpret_cast<const UINT32*>(key));

	delete [] key;
}

void CRuWorld_ObjectContainer::KalydoQueueCell(UINT32 cell)
{
	BYTE* key = new BYTE[sizeof(UINT32)];
	memcpy(key, &cell, sizeof(UINT32));

	if (m_dataStore->KalydoDownloadData(m_tableHandle_Cells, key, this, &CRuWorld_ObjectContainer::DownloadCellCallback) == TRUE)
		return;

	delete [] key;

	m_cellWorkQueue->QueueWork(cell);
}

#endif // KALYDO

CRuWorld_ObjectCell *CRuWorld_ObjectContainer::GetCell(UINT32 cellKey)
{
	CRuWorld_ObjectCell *cell = NULL;

	// Dequeue all loaded cells from the cell work queue
	UINT32 dequeuedCellKey;
	CRuWorld_ObjectCell *dequeuedCell;

	while(m_cellWorkQueue->DequeueResult(dequeuedCellKey, dequeuedCell))
	{
		// Generate a hash for the cell key
		DWORD dequeuedCellKeyHash = RuGlobalHashGen().GetHashCode(dequeuedCellKey);

		// Insert the dequeued cell into the cell hash
		m_cellHash->Add(dequeuedCellKey, dequeuedCellKeyHash, dequeuedCell);
	}

	// Retrieve cell from the hash if possible
	DWORD cellKeyHash = RuGlobalHashGen().GetHashCode(cellKey);
	if(m_cellHash->Get(cellKey, cellKeyHash, cell) == FALSE)
	{
		// If a previous attempt to load the cell failed, then the cell does not exist in the store
		if(m_cellWorkQueue->WorkFailed(cellKey, TRUE))
		{
			// Construct a new cell based on the cell key
			INT32 x = (cellKey >> 16) & 0x0000FFFF;
			INT32 z = cellKey & 0x0000FFFF;

			CRuAABB cellBounds;
			cellBounds.Minima() = CRuVector3(x * m_cellSize, 0.0f, z * m_cellSize) + m_containerBounds.Minima();
			cellBounds.Maxima() = cellBounds.Minima() + CRuVector3(m_cellSize, m_containerBounds.Maxima().m_y - m_containerBounds.Minima().m_y, m_cellSize);

			cell = ruNEW CRuWorld_ObjectCell();
			cell->Initialize(cellKey, cellBounds);

			// Insert the newly generated cell into the cell hash
			m_cellHash->Add(cellKey, cellKeyHash, cell);
		}
		else
		{
#ifdef KALYDO
			KalydoQueueCell(cellKey);
#else
			// Insert into the cell work queue
			m_cellWorkQueue->QueueWork(cellKey);
#endif // KALYDO
		}
	}

	// If deferred loading is disabled, wait until the cell is loaded
	while(!m_enableDeferredLoading && cell == NULL)
	{
		RuThread_Sleep(10);

		// Dequeue all loaded cells from the cell work queue
		UINT32 dequeuedCellKey;
		CRuWorld_ObjectCell *dequeuedCell;

		while(m_cellWorkQueue->DequeueResult(dequeuedCellKey, dequeuedCell))
		{
			// Generate a hash for the cell key
			DWORD dequeuedCellKeyHash = RuGlobalHashGen().GetHashCode(dequeuedCellKey);

			// Insert the dequeued cell into the cell hash
			m_cellHash->Add(dequeuedCellKey, dequeuedCellKeyHash, dequeuedCell);
		}

		// Retrieve cell from the hash if possible
		DWORD cellKeyHash = RuGlobalHashGen().GetHashCode(cellKey);
		if(m_cellHash->Get(cellKey, cellKeyHash, cell) == FALSE)
		{
		}
	}

	return cell;
}

CRuGUID CRuWorld_ObjectContainer::GetNextEnumeratedObjectGUID()
{
	// Return the invalid GUID if there are no more enumerated objects to go through
	if(m_nextEnumeratedObjectIndex >= m_enumeratedObjectGUIDList.Count())
	{
		return CRuGUID::Invalid;
	}

	// Increment to next object index
	++m_nextEnumeratedObjectIndex;

	return m_enumeratedObjectGUIDList[m_nextEnumeratedObjectIndex - 1];
}

CRuArrayList<CRuGUID> &CRuWorld_ObjectContainer::GetEnumeratedObjectGUIDList()
{
	return m_enumeratedObjectGUIDList;
}

REAL CRuWorld_ObjectContainer::GetLoadingStatus()
{
	REAL loadingStatus = 1.0f - ((REAL) m_cellEumerationQueue.Count() / (REAL) m_enumeratedCellCount) + ruEPSILON;

	return loadingStatus;
}

CRuWorld_EntityDescriptor *CRuWorld_ObjectContainer::GetEntityDescriptor(const CRuGUID &guid, BOOL &asyncPendingOut)
{
	BOOL asyncPending = FALSE;
	IRuObject *object = this->GetObject(guid, asyncPending);

	if(object && object->GetType() == CRuWorld_EntityDescriptor::Type())
	{
		return static_cast<CRuWorld_EntityDescriptor *>(object);
	}

	return NULL;
}

CRuEntity *CRuWorld_ObjectContainer::GetEntityInstance(const CRuGUID &guid, BOOL &asyncPendingOut)
{
	// Reset asynchronous pending status
	asyncPendingOut = FALSE;

	// Search the instance dictionary for matching GUID
	InstanceDesc instanceDesc;
	if(m_instanceQueue->Get(guid, guid.HashCode(), instanceDesc))
	{
		// Reset GC age and update back into instance dictionary
		instanceDesc.m_gcAge = 0;
		m_instanceQueue->Replace(guid, guid.HashCode(), instanceDesc);

		// Return entity instance
		return instanceDesc.m_instance;
	}

	CRuWorld_EntityDescriptor *descriptor = GetEntityDescriptor(guid, asyncPendingOut);
	if(descriptor)
	{
		if(m_enableDeferredLoading)
		{
			CRuEntity *entity = NULL;
			if(g_ruResourceManager->LoadEntity(descriptor->GetResourceName(), &entity))
			{
				if(entity)
				{
/*					char outputStr[512];
					sprintf(outputStr, "{\"%s\"},\n", descriptor->GetResourceName());
					OutputDebugString(outputStr);	*/					
					// Setup frame orientation
					entity->ResetLocalTransform();
					entity->Scale(descriptor->GetScale());
					entity->Rotate(descriptor->GetRotation());
					entity->Translate(descriptor->GetTranslation());
					entity->Update(0.0f);

					// Add descriptor to LRU queue
					InstanceDesc instanceDesc = { 0, entity };
					m_instanceQueue->Add(guid, guid.HashCode(), instanceDesc);

					// Enable/disable distance fade on the entity
					RuPropertyType propertyValue;
					BOOL alwaysVisible = descriptor->GetProperty("CRuWorld_EntityDescriptor:Always Visible", propertyValue) ? (boost::get<BOOL>(propertyValue) != false) : FALSE;
					entity->SetProperty("vizCon:enable", !alwaysVisible);

					return entity;
				}

				asyncPendingOut = TRUE;
			}
			else
			{
				// Load the missing entity box
				entity = g_ruResourceManager->LoadEntity("model\\element\\common\\prop\\missingbox\\missingbox.ros");
				//char outputStr[512];
				//sprintf(outputStr, "Failed load model %s\n", descriptor->GetResourceName());
				//OutputDebugString(outputStr);	
				if(entity)
				{
					// Setup frame orientation
					entity->ResetLocalTransform();
					entity->Scale(descriptor->GetScale());
					entity->Rotate(descriptor->GetRotation());
					entity->Translate(descriptor->GetTranslation());
					entity->Update(0.0f);

					// Add descriptor to LRU queue
					InstanceDesc instanceDesc = { 0, entity };
					m_instanceQueue->Add(guid, guid.HashCode(), instanceDesc);

					return entity;
				}
				else
				{
					// Add descriptor to LRU queue
					InstanceDesc instanceDesc = { 0, NULL };
					m_instanceQueue->Add(guid, guid.HashCode(), instanceDesc);
				}
			}
		}
		else
		{
			CRuEntity *entity = g_ruResourceManager->LoadEntity(descriptor->GetResourceName());

			if(entity == NULL)
			{
				//char outputStr[512];
				//sprintf(outputStr, "Failed load model %s\n", descriptor->GetResourceName());
				//OutputDebugString(outputStr);				
				entity = g_ruResourceManager->LoadEntity("model\\element\\common\\prop\\missingbox\\missingbox.ros");
			}

			if(entity)
			{
				// Setup frame orientation
				entity->ResetLocalTransform();
				entity->Scale(descriptor->GetScale());
				entity->Rotate(descriptor->GetRotation());
				entity->Translate(descriptor->GetTranslation());
				entity->Update(0.0f);

				// Add descriptor to LRU queue
				InstanceDesc instanceDesc = { 0, entity };
				m_instanceQueue->Add(guid, guid.HashCode(), instanceDesc);

				// Enable/disable distance fade on the entity
				RuPropertyType propertyValue;
				BOOL alwaysVisible = descriptor->GetProperty("CRuWorld_EntityDescriptor:Always Visible", propertyValue) ? (boost::get<BOOL>(propertyValue) != false) : FALSE;
				entity->SetProperty("vizCon:enable", !alwaysVisible);

				return entity;
			}
			else
			{
				// Add descriptor to LRU queue
				InstanceDesc instanceDesc = { 0, NULL };
				m_instanceQueue->Add(guid, guid.HashCode(), instanceDesc);
			}
		}
	}

	return NULL;
}

BOOL CRuWorld_ObjectContainer::ClearObjectQueue()
{
	// Iterate through each object and destroy
	m_objectQueue->ForAllElements(boost::bind(&CRuWorld_ObjectContainer::ClearObjectQueueCallback, this, _1, _2));

	// Clear object queue
	m_objectQueue->Clear();

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::ClearInstanceQueue()
{
	// Iterate through each entity instance and destroy
	m_instanceQueue->ForAllElements(boost::bind(&CRuWorld_ObjectContainer::ClearInstanceQueueCallback, this, _1, _2));

	// Clear instance queue
	m_instanceQueue->Clear();

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::GarbageCollectInstanceQueue()
{
	DWORD t = timeGetTime();
	m_timeSinceLastGC = t - m_lastGCTime;

	if(m_timeSinceLastGC > 15000)
	{
		// Iterate through each entity instance and perform garbage collection
		m_gc_Instance.Clear();
		m_instanceQueue->ForAllElements(boost::bind(&CRuWorld_ObjectContainer::GarbageCollectInstanceQueueCallback, this, _1, _2));

		// Collect garbage
		for(INT32 i = 0; i < m_gc_Instance.Count(); ++i)
		{
			InstanceDesc instanceDesc;
			if(m_instanceQueue->Get(m_gc_Instance[i], m_gc_Instance[i].HashCode(), instanceDesc))
			{
/*
				char moo[2000];
				sprintf(moo, "OBJCNT: GC %s\n", instanceDesc.m_instance->GetResourceName());
				OutputDebugString(moo);
*/
				// Free instance
				ruSAFE_RELEASE(instanceDesc.m_instance);

				// Remove instance from dictionary
				m_instanceQueue->Remove(m_gc_Instance[i], m_gc_Instance[i].HashCode());
			}
		}

		// Update last GC time
		m_lastGCTime = t;
	}

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::ReinsertObjects()
{
	INT32 numObjects = m_dataStore->GetNumRecords(m_tableHandle_Objects);

	for(INT32 i = 0; i < numObjects; ++i)
	{
		// Retrieve key and data
		IRuStream *stream = m_dataStore->RetrieveByIndex(m_tableHandle_Objects, i);

		if(stream)
		{
			// Create object from stream using object factory
			IRuObject *object = RuObjectFactory().CreateObject(stream);
			delete stream;

			if(object)
			{
				if(object->GetType().IsSubClassOf(CRuWorld_EntityDescriptor::Type()))
				{
					CRuWorld_EntityDescriptor *entityDesc = static_cast<CRuWorld_EntityDescriptor *>(object);

					if(entityDesc->GetNumLinkedChunks() > 0)
					{
						entityDesc->AddRef();
						this->DeleteObject(entityDesc->GetGUID(), entityDesc->GetWorldBounds());
						this->InsertObject(entityDesc, entityDesc->GetWorldBounds());
					}
				}
				else if(object->GetType().IsSubClassOf(CRuWorld_Trigger::Type()))
				{
					CRuWorld_Trigger *trigger = static_cast<CRuWorld_Trigger *>(object);

					trigger->AddRef();
					this->DeleteObject(trigger->GetGUID(), trigger->GetWorldBounds());
					this->InsertObject(trigger, trigger->GetWorldBounds());
				}

				ruSAFE_RELEASE(object);
			}
		}
	}

	return TRUE;
}

CRuWorld_CellLink *CRuWorld_ObjectContainer::GetCellLinkObject(IRuObject *object)
{
	// Get the CRuWorld_CellLink structure from the object, or create one if it does not exist
	CRuWorld_CellLink *cellLink = NULL;

	RuPropertyType propertyValue;
	if(object->GetProperty("CRuWorld_CellLink", propertyValue) && boost::get<IRuObject *>(propertyValue))
	{
		cellLink = static_cast<CRuWorld_CellLink *>(boost::get<IRuObject *>(propertyValue));
	}
	else
	{
		cellLink = ruNEW CRuWorld_CellLink();
		object->SetProperty("CRuWorld_CellLink", cellLink);

		// Release local reference
		cellLink->Release();
	}

	return cellLink;
}

IRuObject *CRuWorld_ObjectContainer::GetObjectFromChangeList(const CRuGUID &guid)
{
	for(INT32 i = 0; i < m_changedObjectList.Count(); ++i)
	{
		if(m_changedObjectList[i]->GetGUID() == guid)
		{
			return m_changedObjectList[i];
		}
	}

	return NULL;
}

BOOL CRuWorld_ObjectContainer::ObjectQueueMemoryFootprintCallback(const void *key, void *data)
{
	IRuObject *obj = *reinterpret_cast<IRuObject **>(data);

	if(obj)
	{
		m_temp_Footprint += obj->GetMemoryFootprint(m_temp_EnumHash);
	}

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::InstanceQueueMemoryFootprintCallback(const void *key, void *data)
{
	InstanceDesc *instanceDesc = reinterpret_cast<InstanceDesc *>(data);

	if(instanceDesc->m_instance)
	{
		m_temp_Footprint += instanceDesc->m_instance->GetMemoryFootprint(m_temp_EnumHash);
	}

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::ClearObjectQueueCallback(const void *key, void *data)
{
	IRuObject *object = *((IRuObject **) data);

	ruSAFE_RELEASE(object);

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::ClearInstanceQueueCallback(const void *key, void *data)
{
	InstanceDesc *instanceDesc = reinterpret_cast<InstanceDesc *>(data);

	ruSAFE_RELEASE(instanceDesc->m_instance);

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::GarbageCollectInstanceQueueCallback(const void *key, void *data)
{
	InstanceDesc *instanceDesc = reinterpret_cast<InstanceDesc *>(data);

	if(instanceDesc->m_instance && instanceDesc->m_instance->GetRefCount() == 1)
	{
		// Update GC age
		instanceDesc->m_gcAge += m_timeSinceLastGC;

		// If instance's GC age exceeds threshold, insert it into the GC list
		if(instanceDesc->m_gcAge > 5000)
		{
			m_gc_Instance.Add(*reinterpret_cast<const CRuGUID *>(key));
		}
	}

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::ClearCellHash()
{
	m_cellHash->ForAllElements(boost::bind(&CRuWorld_ObjectContainer::ClearCellHashCallback, this, _1, _2));
	m_cellHash->Clear();

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::ClearCellHashCallback(const void *key, void *data)
{
	CRuWorld_ObjectCell *cell = *reinterpret_cast<CRuWorld_ObjectCell **>(data);
	ruSAFE_RELEASE(cell);
	return TRUE;
}

BOOL CRuWorld_ObjectContainer::SaveCells()
{
	// Invoke the save procedure on all cells
	m_cellHash->ForAllElements(boost::bind(&CRuWorld_ObjectContainer::SaveCellsCallback, this, _1, _2));

	return TRUE;
}

BOOL CRuWorld_ObjectContainer::SaveCellsCallback(const void *key, void *data)
{
	CRuWorld_ObjectCell *cell = *reinterpret_cast<CRuWorld_ObjectCell **>(data);

	if(cell->IsDirty())
	{
		// Serialize cell to a stream
		CRuMemoryStream memStream;
		memStream.SetBufferSize(4096);
		cell->SerializeTo(&memStream);

		// Store stream to data store
		UINT32 cellKey = cell->GetCellKey();
		m_dataStore->Store(m_tableHandle_Cells, reinterpret_cast<BYTE *>(&cellKey), &memStream);

		// Mark cell as not dirty
		cell->MarkAsNotDirty();
	}

	return TRUE;
}

IRuObject *CRuWorld_ObjectContainer::FilterObject(IRuObject *obj, const CRuGUID &objGUID)
{
	// Check object GUID against already filtered objects
	if(m_filteredObjects->ContainsKey(objGUID, objGUID.HashCode()))
	{
		return NULL;
	}

	// Check object against filter
	if(obj && m_specialEventFilter.Count() > 0)
	{
		// Get special event tag from the object
		RuPropertyType propertyValue;
		const char *specialEvent  = obj->GetProperty("CRuWorld_EntityDescriptor:Special Event", propertyValue) ? boost::get<const char *>(propertyValue) : "";

		if(specialEvent[0] != NULL)
		{
			BOOL filtered = TRUE;

			// Check against all filter strings
			for(INT32 i = 0; i < m_specialEventFilter.Count(); ++i)
			{
				if(m_specialEventFilter[i])
				{
					// If the special event tag exists and it does not match the current special event, filter the object
					if(strcmpi(m_specialEventFilter[i], specialEvent) == 0)
					{
						filtered = FALSE;
						break;
					}
				}
			}

			if(filtered)
			{
				// Add object to filter
				m_filteredObjects->Add(objGUID, objGUID.HashCode());

				return NULL;
			}
		}
	}

	return obj;
}

BOOL CRuWorld_ObjectContainer::WorkerFunction()
{
	// Process the cell work queue
	UINT32 cellKey = 0xFFFFFFFF;
	if(m_cellWorkQueue->DequeueWork(cellKey))
	{
		// Retrieve object stream
		IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_Cells, reinterpret_cast<BYTE *>(&cellKey));
		if(stream)
		{
			// Create object from stream using object factory
			IRuObject *object = RuObjectFactory().CreateObject(stream);
			delete stream;

			if(object)
			{
				CRuWorld_ObjectCell *cell = static_cast<CRuWorld_ObjectCell *>(object);
				m_cellWorkQueue->QueueResult(cellKey, cell);
			}
		}
		else
		{
			m_cellWorkQueue->MarkWorkFailed(cellKey);
		}
	}

	// Process the object work queue
	CRuGUID guid(CRuGUID::Invalid);
	if(m_workQueue->DequeueWork(guid))
	{
		// Retrieve object stream
		IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_Objects, guid.Data());
		if(stream)
		{
			// Create object from stream using object factory
			IRuObject *object = RuObjectFactory().CreateObject(stream);
			delete stream;

			if(object)
			{
				m_workQueue->QueueResult(guid, object);
			}
			else
			{
				m_workQueue->MarkWorkFailed(guid);
			}
		}
		else
		{
			m_workQueue->MarkWorkFailed(guid);
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuWorld_ObjectManager::CRuWorld_ObjectManager()
:	m_objectContainer(NULL),
	m_enumerationMarginSize(120.0f),
	m_enableDistanceCulling(TRUE),
	m_enableTriggerDisplay(FALSE),
	m_enableTriggerDisplayFilter(FALSE),
	m_detailClass(3),
	m_cameraPositionIsIndoor(FALSE),
	m_objectTree(NULL)
{
	m_enumerationHash = ruNEW CRuKeyHash<CRuGUID>(2048);
	m_descriptorHash = ruNEW CRuHashMap<CRuGUID, EntityDesc>(2048);

	m_probe_NumObjects = g_ruProfiler->CreateProbe("Object Count", "World");
	m_probe_NumMissingObjects = g_ruProfiler->CreateProbe("Missing Objects", "World");
	m_probe_NumDeferredObjects = g_ruProfiler->CreateProbe("Deferred Objects", "World");
	m_probe_NumSpritePrtSystems = g_ruProfiler->CreateProbe("PrtSys Count", "World");
	m_probe_ColQuery_TreeTraversal = g_ruProfiler->CreateProbe("ColTree Traversal", "World");
}

CRuWorld_ObjectManager::~CRuWorld_ObjectManager()
{
	ClearEnumerationCache();

	ruSAFE_RELEASE(m_objectContainer);
	delete m_enumerationHash;
	delete m_descriptorHash;

	// Release object partitioning tree
	ruSAFE_RELEASE(m_objectTree);
}

BOOL CRuWorld_ObjectManager::Attach(CRuWorld_ObjectContainer *container)
{
	// Store reference to object container
	m_objectContainer = container;
	m_objectContainer->AddRef();

	return TRUE;
}

BOOL CRuWorld_ObjectManager::ClearEnumerationCache()
{
	// Remove all attached entities
	this->RemoveAllChildren();

	// Reset enumeration bounds
	m_enumerationBounds.Set(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	// Clear all enumeration hashes and queues
	m_enumerationHash->Clear();

	m_descriptorHash->ForAllElements(boost::bind(&CRuWorld_ObjectManager::ClearDescriptorHashCallback, this, _1, _2));
	m_descriptorHash->Clear();

	m_entityWorkQueue.Clear();

	// Set the time of this enumeration cache clear
	m_lastEnumerationClearTick = GetTickCount();

	return TRUE;
}

const CRuAABB &CRuWorld_ObjectManager::GetEnumerationBounds()
{
	return m_enumerationBounds;
}


BOOL CRuWorld_ObjectManager::EnableDistanceCulling(BOOL enable)
{
	m_enableDistanceCulling = enable;
	return TRUE;
}

BOOL CRuWorld_ObjectManager::EnableTriggerDisplay(BOOL enable)
{
	if(m_enableTriggerDisplay != enable)
	{
		m_enableTriggerDisplay = enable;
		ClearEnumerationCache();
	}

	return TRUE;
}

BOOL CRuWorld_ObjectManager::EnableTriggerDisplayFilter(BOOL enable)
{
	if(m_enableTriggerDisplayFilter != enable)
	{
		m_enableTriggerDisplayFilter = enable;
		ClearEnumerationCache();
	}

	return TRUE;
}

BOOL CRuWorld_ObjectManager::SetTriggerDisplayFilterID(INT32 filterID)
{
	if(m_triggerDisplayFilterID != filterID)
	{
		m_triggerDisplayFilterID = filterID;
		ClearEnumerationCache();
	}

	return TRUE;
}

BOOL CRuWorld_ObjectManager::SetDetailClass(INT32 detailClass)
{
	m_detailClass = detailClass;

	return TRUE;
}

BOOL CRuWorld_ObjectManager::SetSpecialEventFilter(const char *filter)
{
	// Set special event filter at container
	m_objectContainer->SetSpecialEventFilter(filter);

	// Clear enumeration cache to force a re-enumeration
	ClearEnumerationCache();

	return TRUE;
}

BOOL CRuWorld_ObjectManager::IsGUIDEnumerated(const CRuGUID &guid)
{
	return m_enumerationHash->ContainsKey(guid, guid.HashCode());
}

BOOL CRuWorld_ObjectManager::CameraPositionIsIndoor()
{
	return m_cameraPositionIsIndoor;
}

BOOL CRuWorld_ObjectManager::UpdateEnumeration(const CRuAABB &enumerationBounds)
{
	if(m_enumerationBounds.Contains(enumerationBounds) == FALSE)
	{
		// Reset metrics
		m_numObjects = 0;
		m_numMissingObjects = 0;
		m_numSpritePrtSystems = 0;

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

		// Update object container enumeration
		m_objectContainer->UpdateEnumeration(m_enumerationBounds);


		if(m_objectTree == NULL)
		{
			// Initialize object partitioning tree
			m_objectTree = ruNEW CRuPartition_QuadTree();
			m_objectTree->Initialize(m_enumerationBounds, 32);
		}
		else
		{
			const CRuAABB &objectTreeBounds = m_objectTree->GetBounds();

			CRuVector3 objectTreeExtents = (objectTreeBounds.Maxima() - objectTreeBounds.Minima()) + ruEPSILON;
			CRuVector3 enumerationExtents = m_enumerationBounds.Maxima() - m_enumerationBounds.Minima();

			if(enumerationExtents.m_x > objectTreeExtents.m_x || enumerationExtents.m_y > objectTreeExtents.m_y || enumerationExtents.m_z > objectTreeExtents.m_z)
			{
				// Release existing tree
				ruSAFE_RELEASE(m_objectTree);

				// Initialize object partitioning tree
				m_objectTree = ruNEW CRuPartition_QuadTree();
				m_objectTree->Initialize(m_enumerationBounds, 32);
			}
			else
			{
				CRuVector3 enumerationCenter = (m_enumerationBounds.Minima() + m_enumerationBounds.Maxima()) * 0.5f;
				m_objectTree->SetPosition(enumerationCenter);
			}
		}

		// Clear the object tree
		m_objectTree->Clear();
	}

	// Update numeration center
	m_enumerationCenter = (enumerationBounds.Minima() + enumerationBounds.Maxima()) / 2.0f;

	return TRUE;
}

BOOL CRuWorld_ObjectManager::Update(REAL elapsedTime)
{
	// Reset metrics
	m_numDeferredObjects = 0;

	// Garbage collection & Update
	m_objectContainer->Update(elapsedTime);

	// Check for newly enumerated object GUIDs and add them to the work queue
	for(CRuGUID objGUID = m_objectContainer->GetNextEnumeratedObjectGUID(); objGUID != CRuGUID::Invalid; objGUID = m_objectContainer->GetNextEnumeratedObjectGUID())
	{
		if(m_enumerationHash->ContainsKey(objGUID, objGUID.HashCode()) == FALSE)
		{
			// Queue GUID
			m_entityWorkQueue.Queue(objGUID);

			// Append GUID to unique list
			m_enumerationHash->Add(objGUID, objGUID.HashCode());

			// Increment total object count
			++m_numObjects;
		}
	}
/*
	char moo[200];
	sprintf(moo, "Entities waiting for instancing: %d\n", m_entityWorkQueue.Count());
	OutputDebugString(moo);
*/

	// Iterate through entity work queue once
	INT32 totalGUIDCount = m_entityWorkQueue.Count();
	INT32 currentGUIDCount = 0;

	while(m_entityWorkQueue.Count() > 0 && currentGUIDCount < totalGUIDCount)
	{
		// Increment processing count
		++currentGUIDCount;

		// Attempt to get the object specified by current GUID
		CRuGUID guid = m_entityWorkQueue.Dequeue();

		BOOL asyncPending;
		IRuObject *object = m_objectContainer->GetObjectWithFilter(guid, asyncPending);

		// Asynchronous read pending, throw object into work queue
		if(asyncPending)
		{
			++m_numDeferredObjects;
			m_entityWorkQueue.Queue(guid);
			continue;
		}

		if(object)
		{
			if(object->GetType() == CRuWorld_EntityDescriptor::Type())
			{
				CRuWorld_EntityDescriptor *descriptor = static_cast<CRuWorld_EntityDescriptor *>(object);

				if(descriptor)
				{
					// Insert entity descriptor into object tree
					m_objectTree->InsertObject(descriptor, descriptor->GetWorldBounds());
				}

				if(descriptor && Intersects(m_enumerationBounds, descriptor->GetWorldBounds()) && descriptor->GetDetailClass() <= m_detailClass)
				{
					CRuEntity *instancedEntity = m_objectContainer->GetEntityInstance(guid, asyncPending);

					// Asynchronous read pending, throw object into work queue
					if(asyncPending)
					{
						++m_numDeferredObjects;
						m_entityWorkQueue.Queue(guid);
						continue;
					}

					if(instancedEntity)
					{
						// If the entity does not have a descriptor, create one now
						if(m_descriptorHash->ContainsKey(guid, guid.HashCode()) == FALSE)
						{
							EntityDesc entityDescriptor;

							entityDescriptor.m_descriptor = descriptor;
							entityDescriptor.m_entity = instancedEntity;
							entityDescriptor.m_entityBounds.Set(instancedEntity->GetTranslation(), instancedEntity->GetLocalBounds().Radius());
							entityDescriptor.m_objectState = ruOBJMOTION_NONE;

							entityDescriptor.m_descriptor->AddRef();
							entityDescriptor.m_entity->AddRef();

							m_descriptorHash->Add(guid, guid.HashCode(), entityDescriptor);
						}

						// If distance culling is disabled, add the entity to the child list directly
						if(m_enableDistanceCulling == FALSE)
						{
							this->AddChild(instancedEntity);
						}

						// Run additional setup on instanced entity
						RuVizCon_SetupVizConState(instancedEntity);
						RuPrtSys_SetUpdateInterval(instancedEntity, 0.0555f);
						instancedEntity->SetMinUpdateInterval(0.0555f);
					}
				}
			}
			else if(object->GetType().IsSubClassOf(CRuWorld_Trigger::Type()) && m_enableTriggerDisplay)
			{
				if(!m_enableTriggerDisplayFilter || static_cast<CRuWorld_Trigger *>(object)->GetZoneID() == m_triggerDisplayFilterID)
				{
					// Instance entity
					this->AddEntity(static_cast<CRuEntity *>(object));
				}
			}
		}
		else
		{
			++m_numMissingObjects;
		}

//		++m_numObjects;
	}

	// Step through all enumerated entities
	m_cameraPositionIsIndoor = FALSE;
	m_descriptorHash->ForAllElements(boost::bind(&CRuWorld_ObjectManager::EnumerateVisibleEntities, this, _1, _2));

	// Remote deleted entities
	for(INT32 i = 0; i < m_deletedDescriptorGUIDs.Count(); ++i)
	{
		EntityDesc entityDesc;

		if(m_descriptorHash->Get(m_deletedDescriptorGUIDs[i], m_deletedDescriptorGUIDs[i].HashCode(), entityDesc))
		{
			ruSAFE_RELEASE(entityDesc.m_descriptor);
			ruSAFE_RELEASE(entityDesc.m_entity);

			m_descriptorHash->Remove(m_deletedDescriptorGUIDs[i], m_deletedDescriptorGUIDs[i].HashCode());
		}
	}

	m_deletedDescriptorGUIDs.Clear();

	// Record profiler values
	g_ruProfiler->SetProbeValue(m_probe_NumObjects, static_cast<REAL>(m_numObjects));
	g_ruProfiler->SetProbeValue(m_probe_NumMissingObjects, static_cast<REAL>(m_numMissingObjects));
	g_ruProfiler->SetProbeValue(m_probe_NumDeferredObjects, static_cast<REAL>(m_numDeferredObjects));
	g_ruProfiler->SetProbeValue(m_probe_NumSpritePrtSystems, static_cast<REAL>(m_numSpritePrtSystems));

	BOOL retVal = CRuEntity_Container_Impl::Update(elapsedTime);

	return retVal;

//	return TRUE;


//	return CRuEntity_Container_Impl::Update(elapsedTime);
}

BOOL CRuWorld_ObjectManager::QueryCollision(CRuCollisionQuery *colQuery)
{
/*
	DWORD t0, t1, et;
	t0 = GetTickCount();
*/
	// Clear object array
	m_ws_Objects.Clear();

	if(m_objectTree)
	{
		switch(colQuery->m_queryType)
		{
			case ruCOLQUERY_RAY:
			case ruCOLQUERY_RAY_HITTEST:
			case ruCOLQUERY_RAY_NEAREST:
			case ruCOLQUERY_RAY_PICK:
			case ruCOLQUERY_RAY_TRIPICK:
			case ruCOLQUERY_RAY_SUBOBJECT:
				{
					// Enumerate all intersecting objects
					CRuCollisionQuery_Ray_Base *rayQuery = static_cast<CRuCollisionQuery_Ray_Base *>(colQuery);
					m_objectTree->EnumerateObjects(rayQuery->m_colRay, m_ws_Objects);
				}
				break;

			case ruCOLQUERY_SEGMENT:
			case ruCOLQUERY_SEGMENT_HITTEST:
			case ruCOLQUERY_SEGMENT_NEAREST:
				{
					// Enumerate all intersecting objects
					CRuCollisionQuery_Segment_Base *segQuery = static_cast<CRuCollisionQuery_Segment_Base *>(colQuery);
					m_objectTree->EnumerateObjects(segQuery->m_colSegment, m_ws_Objects);
				}
				break;

			case ruCOLQUERY_OBB_HITTEST:
			case ruCOLQUERY_OBB_TRIENUM:
				{
					// Enumerate all intersecting objects
					CRuCollisionQuery_OBB_Base *obbQuery = static_cast<CRuCollisionQuery_OBB_Base *>(colQuery);
					m_objectTree->EnumerateObjects(obbQuery->m_colOBB, m_ws_Objects);
				}
				break;

			case ruCOLQUERY_SPHERE_HITTEST:
			case ruCOLQUERY_SPHERE_TRIENUM:
				{
					// Enumerate all intersecting objects
					CRuCollisionQuery_Sphere_Base *sphereQuery = static_cast<CRuCollisionQuery_Sphere_Base *>(colQuery);
					m_objectTree->EnumerateObjects(sphereQuery->m_colSphere, m_ws_Objects);
				}
				break;

			case ruCOLQUERY_USER_FLAG_GET_SEGMENT:
				{
					// Enumerate all intersecting objects
					CRuCollisionQuery_User_Flag_Get_Segment *flagQuery = static_cast<CRuCollisionQuery_User_Flag_Get_Segment *>(colQuery);
					m_objectTree->EnumerateObjects(flagQuery->m_colSegment, m_ws_Objects);
				}
				break;
		}
	}

	for(INT32 i = 0; i < m_ws_Objects.Count(); ++i)
	{
		// Retrieve entity instance from entity container and perform collision query with it
		BOOL asyncPending;
		CRuEntity *instancedEntity = m_objectContainer->GetEntityInstance(m_ws_Objects[i]->GetGUID(), asyncPending);
		if(instancedEntity)
		{
			instancedEntity->QueryCollision(colQuery);
		}
	}
/*
	t1 = GetTickCount();
	et = t1 - t0;

	RUPROFILE_SAMPLE_ACCUMULATE(m_probe_ColQuery_TreeTraversal, 1, et);
*/
	return TRUE;
}

BOOL CRuWorld_ObjectManager::QueryCollision(const CRuGUID &guid, CRuCollisionQuery *colQuery)
{
	BOOL asyncPending;
	CRuWorld_EntityDescriptor *entityDesc = m_objectContainer->GetEntityDescriptor(guid, asyncPending);

	if(entityDesc && entityDesc->GetDetailClass() <= m_detailClass)
	{
		CRuEntity *instancedEntity = m_objectContainer->GetEntityInstance(guid, asyncPending);

		if(instancedEntity)
		{
			return instancedEntity->QueryCollision(colQuery);
		}
	}

	return TRUE;
}

REAL CRuWorld_ObjectManager::GetLoadingStatus()
{
/*
	// Forcibly wait two seconds before reporting the correct loading status to avoid problems with missing objects
	if(GetTickCount() - m_lastEnumerationClearTick < 2000)
	{
		return 0.0f;
	}
*/
	REAL cellLoadingStatus = m_objectContainer->GetLoadingStatus() * 0.5f;
	REAL entityLoadingStatus = ((m_numObjects > 0) ? 1.0f - (REAL) m_entityWorkQueue.Count() / (REAL) m_numObjects : 1.0f) * 0.5f;
	REAL loadingStatus = cellLoadingStatus + entityLoadingStatus + ruEPSILON;

	return loadingStatus;
}

BOOL CRuWorld_ObjectManager::ChainObjectVisibility(const CRuGUID &guid)
{
	REAL nearestObjDist = FLT_MAX;
	CRuGUID nearestObjGUID = CRuGUID::Invalid;
	CRuGUID nearestObjSubPath = CRuGUID::Invalid;

	// Get target entity descriptor
	CRuWorld_EntityDescriptor *targetDescriptor = NULL;
	BOOL asyncPending = FALSE;
	do
	{
		targetDescriptor = m_objectContainer->GetEntityDescriptor(guid, asyncPending);
	} while(asyncPending);

	// Get target entity instance
	CRuEntity *targetEntityInstance = NULL;
	asyncPending = FALSE;
	do 
	{
		targetEntityInstance = m_objectContainer->GetEntityInstance(guid, asyncPending);
	} while(asyncPending);

	if(targetDescriptor && targetEntityInstance)
	{
		// Enumerate all visible entity descriptors
		m_visibleEntityGUIDs.Clear();
		m_descriptorHash->ForAllElements(boost::bind(&CRuWorld_ObjectManager::ChainObjectVisibilityCallback, this, _1, _2));

		// Step through all enumerated entities
		for(INT32 i = 0; i < m_visibleEntityGUIDs.Count(); ++i)
		{
			// Get entity descriptor
			CRuWorld_EntityDescriptor *descriptor = NULL;
			asyncPending = FALSE;
			do
			{
				descriptor = m_objectContainer->GetEntityDescriptor(m_visibleEntityGUIDs[i], asyncPending);
			} while(asyncPending);

			if(descriptor)
			{
				// Do not chain to the object that is to supposed to be chained (no self-reference loops)
				if(descriptor->GetGUID() != guid)
				{
					// Get entity instance
					CRuEntity *entityInstance = NULL;
					asyncPending = FALSE;
					do
					{
						entityInstance = m_objectContainer->GetEntityInstance(descriptor->GetGUID(), asyncPending);
					} while(asyncPending);

					// Query collision against entity
					if(entityInstance)
					{
						CRuContainer_Portal *portalContainer = RuEntity_FindFirstPortalContainer(entityInstance);

						// If a portal container exists, we use only the portal container for visibility chaining
						if(portalContainer)
						{
							for(INT32 j = 0; j < portalContainer->GetNumRooms(); ++j)
							{
								CRuEntity *subEntity = portalContainer->GetRoomEntity(j);

								// Build collision query
								CRuCollisionQuery_Segment_Nearest colQuery;
								colQuery.m_colSegment = CRuSegment3(targetEntityInstance->GetTranslation(ruFRAMETYPE_WORLD) + CRuVector3(0.0f, 500.0f, 0.0f), targetEntityInstance->GetTranslation(ruFRAMETYPE_WORLD) - CRuVector3(0.0f, 1000.0f, 0.0f));

								// perform collision query against sub entity
								subEntity->QueryCollision(&colQuery);

								if(colQuery.m_positiveHit && colQuery.m_collisionT < nearestObjDist)
								{
									nearestObjDist = colQuery.m_collisionT;
									nearestObjGUID = m_visibleEntityGUIDs[i];
									nearestObjSubPath = portalContainer->GetRoomGUID(j);
								}
							}
						}
						else
						{
/*
							// Build collision query
							CRuCollisionQuery_Segment_Nearest colQuery;
							colQuery.m_colSegment = CRuSegment3(targetEntityInstance->GetTranslation(ruFRAMETYPE_WORLD), targetEntityInstance->GetTranslation(ruFRAMETYPE_WORLD) - CRuVector3(0.0f, 1000.0f, 0.0f));

							// Perform collision query
							entityInstance->QueryCollision(&colQuery);

							if(colQuery.m_positiveHit && colQuery.m_collisionT < nearestObjDist)
							{
								nearestObjDist = colQuery.m_collisionT;
								nearestObjGUID = m_visibleEntityGUIDs[i];
								nearestObjSubPath = m_visibleEntityGUIDs[i];
							}
*/
						}
					}
				}
			}
		}

		// Use nearest object for chaining
		if(nearestObjDist < FLT_MAX)
		{
			targetDescriptor->SetVisibilityChainSource(nearestObjGUID);
			targetDescriptor->SetVisibilityChainSubPath(nearestObjSubPath);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuWorld_ObjectManager::UnchainObjectVisibility(const CRuGUID &guid)
{
	// Get target entity descriptor
	CRuWorld_EntityDescriptor *targetDescriptor = NULL;
	BOOL asyncPending = FALSE;
	do
	{
		targetDescriptor = m_objectContainer->GetEntityDescriptor(guid, asyncPending);
	} while(asyncPending);

	if(targetDescriptor)
	{
		targetDescriptor->SetVisibilityChainSource(CRuGUID::Invalid);
		targetDescriptor->SetVisibilityChainSubPath(CRuGUID::Invalid);
		return TRUE;
	}

	return FALSE;
}

BOOL CRuWorld_ObjectManager::EnumerateVisibleObjectsByType(CRuArrayList<IRuObject *> &objectsOut, const CRuRTTI &objectType)
{
	return m_objectContainer->EnumerateVisibleObjectsByTypeWithFilter(objectsOut, objectType);
}

BOOL CRuWorld_ObjectManager::EnumerateVisibleEntities(const void *key, void *data)
{
	const CRuGUID &entityGUID = *reinterpret_cast<const CRuGUID *>(key);
	EntityDesc *entityDescriptor = reinterpret_cast<EntityDesc *>(data);

	if(m_enumerationHash->ContainsKey(entityGUID, entityGUID.HashCode()))
	{
		// If entity is slave to a visibility chain, apply it now
		const CRuGUID &vizChainSrc = entityDescriptor->m_descriptor->GetVisibilityChainSource();
		const CRuGUID &vizChainSubPath = entityDescriptor->m_descriptor->GetVisibilityChainSubPath();
		if(vizChainSrc != CRuGUID::Invalid && vizChainSubPath != CRuGUID::Invalid)
		{
			EntityDesc vizChainSrcDesc;
			if(m_descriptorHash->Get(vizChainSrc, vizChainSrc.HashCode(), vizChainSrcDesc))
			{
				CRuContainer_Portal *portalContainer = RuEntity_FindFirstPortalContainer(vizChainSrcDesc.m_entity);
				if(portalContainer)
				{
					portalContainer->AddToVisibilityChain(vizChainSubPath, entityDescriptor->m_entity);
				}
			}
		}

		// If the entity has not been attached to the object manager, attach it now
		if(entityDescriptor->m_entity->GetParent() == NULL)
		{
			this->AddChild(entityDescriptor->m_entity);
		}

		// If the entity state is inconsistent with current setting, apply now
		if(entityDescriptor->m_objectState != ruOBJMOTION_IDLE)
		{
			RuACT_PlayMotionEx(entityDescriptor->m_entity, 0xFFFFFFFF, "idle");
			entityDescriptor->m_objectState = ruOBJMOTION_IDLE;
		}

		// Check portal container for camera position type
		CRuContainer_Portal *portalContainer = RuEntity_FindFirstPortalContainer(entityDescriptor->m_entity);
		if(portalContainer)
		{
			if(portalContainer->CameraPositionIsIndoor() && portalContainer->HideOutdoor())
			{
				m_cameraPositionIsIndoor = TRUE;
			}
		}
	}
	else
	{
		// Detach entity
		entityDescriptor->m_entity->DetachFromParent();

		// Mark for deletion
		m_deletedDescriptorGUIDs.Add(entityGUID);
	}

	return TRUE;
}

BOOL CRuWorld_ObjectManager::ClearDescriptorHashCallback(const void *key, void *data)
{
	const CRuGUID &entityGUID = *reinterpret_cast<const CRuGUID *>(key);
	EntityDesc *entityDescriptor = reinterpret_cast<EntityDesc *>(data);

	// Release reference to descriptor
	ruSAFE_RELEASE(entityDescriptor->m_descriptor);

	// Release reference to entity
	ruSAFE_RELEASE(entityDescriptor->m_entity);

	return TRUE;
}

BOOL CRuWorld_ObjectManager::ChainObjectVisibilityCallback(const void *key, void *data)
{
	const CRuGUID &entityGUID = *reinterpret_cast<const CRuGUID *>(key);

	m_visibleEntityGUIDs.Add(entityGUID);

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
