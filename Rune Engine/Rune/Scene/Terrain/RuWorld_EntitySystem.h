#ifndef _RUWORLD_ENTITYSYSTEM_H_
#define _RUWORLD_ENTITYSYSTEM_H_

#include "../../Core/DataStore/RuDataStore_Base.h"
#include "../../Partition/RuPartition_QuadTree.h"
#include "../../Scene/Terrain/RuWorld_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

#define ruWORLD_OBJECTCELLMULTIPLIER 2.0f

// ************************************************************************************************************************************************************

class CRuWorld_CellLink : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuArrayList<UINT32>					m_cellLinks;							//!< List of cell links

public:
											CRuWorld_CellLink();
	virtual									~CRuWorld_CellLink();

	// Cloning
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);

	// Serialization interface
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	INT32									GetLinkCount();
	UINT32									GetLink(INT32 index);
	BOOL									InsertLink(UINT32 link);
	BOOL									DeleteLink(UINT32 link);
	BOOL									ClearLinks();
};

// ************************************************************************************************************************************************************

class CRuWorld_ObjectCell : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	UINT32									m_cellKey;								//!< Cell key
	CRuAABB									m_cellBounds;							//!< Cell bounds
	CRuArrayList<CRuGUID>					m_objectGUIDs;							//!< List of object GUIDs in this cell

	BOOL									m_dirty;								//!< Cell is dirty

public:
											CRuWorld_ObjectCell();
	virtual									~CRuWorld_ObjectCell();

	// Cloning
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);

	// Serialization interface
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	BOOL									Initialize(UINT32 cellKey, const CRuAABB &cellBounds);

	UINT32									GetCellKey() const;
	const CRuAABB&							GetCellBounds() const;

	BOOL									IsDirty() const;
	void									MarkAsNotDirty();
	void									MarkAsDirty();

	BOOL									InsertObjectGUID(const CRuGUID &guid);
	BOOL									RemoveObjectGUID(const CRuGUID &guid);

	INT32									GetObjectGUIDCount();
	const CRuGUID&							GetObjectGUID(INT32 index);
};

// ************************************************************************************************************************************************************

class CRuWorld_ObjectContainer : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct InstanceDesc
	{
		DWORD											m_gcAge;
		CRuEntity*										m_instance;
	};

	// Container properties
	CRuAABB												m_containerBounds;						//!< Container maximal bounds
	REAL												m_cellSize;								//!< Size of each cell

	// Spatial subdivision for cells
	CRuWorld_ChunkTree*									m_cellTree;

	CRuHashMap<UINT32, CRuWorld_ObjectCell *>*			m_cellHash;								//!< Hash of entity descriptors/objects

	INT32												m_enumeratedCellCount;
	CRuQueue<UINT32>									m_cellEumerationQueue;					//!< Queue of enumerated cells which are waiting for loading
	CRuArrayList<CRuWorld_ObjectCell *>					m_enumeratedCells;						//!< List of enumerated cells

	CRuArrayList<CRuGUID>								m_enumeratedObjectGUIDList;				//!< List of enumerated object GUIDs
	CRuKeyHash<CRuGUID>									m_enumeratedObjectGUIDHash;				//!< Hash of enumerated object GUIDs
	INT32												m_nextEnumeratedObjectIndex;			//!< Index of object GUID to return on the next call to GetNextEnumeratedObjectGUID

	// Data store
	CRuDataStore_Base*									m_dataStore;							//!< Data store
	INT32												m_tableHandle_Cells;					//!< Handle to cells table
	INT32												m_tableHandle_Objects;					//!< Handle to objects table

	// Special event
	CRuArrayList<char *>								m_specialEventFilter;
//	char*												m_specialEventFilter;
	CRuKeyHash<CRuGUID>*								m_filteredObjects;

	// Entity descriptor cache
	CRuLRUQueue<CRuGUID, IRuObject *>*					m_objectQueue;							//!< LRU queue of entity descriptors/objects
	CRuHashMap<CRuGUID, InstanceDesc>*					m_instanceQueue;

	// Update list
	CRuArrayList<IRuObject *>							m_changedObjectList;					//!< List of changed objects
	CRuArrayList<CRuGUID>								m_deletedObjectList;					//!< List of deleted object GUIDs

	// Worker thread
	BOOL												m_enableDeferredLoading;
	CRuThread_WorkQueue<UINT32, CRuWorld_ObjectCell*>*	m_cellWorkQueue;
	CRuThread_WorkQueue<CRuGUID, IRuObject*>*			m_workQueue;

	// GC
	DWORD												m_lastGCTime;
	DWORD												m_timeSinceLastGC;
	CRuArrayList<CRuGUID>								m_gc_Instance;

	// Temp
	INT32												m_temp_Footprint;
	CRuKeyHash<PTRVALUE>*								m_temp_EnumHash;

public:
											CRuWorld_ObjectContainer(const CRuAABB &containerBounds, REAL cellSize);
	virtual									~CRuWorld_ObjectContainer();

	// Memory reporting
	virtual INT32							GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	// Data source management
	BOOL									Attach(CRuDataStore_Base *dataStore);
	BOOL									SaveChanges();
	BOOL									CopyTo(CRuDataStore_Base *targetDataStore);
	BOOL									EnableDeferredLoading(BOOL enableDeferredLoading);

	// Object management
	BOOL									InsertObject(IRuObject *object, const CRuSphere &objectBounds);
	BOOL									DeleteObject(const CRuGUID &guid, const CRuSphere &objectBounds);
	BOOL									UpdateObjectCellLinks(IRuObject *object, const CRuSphere &objectBounds);
	BOOL									MarkObjectForUpdate(const CRuGUID &guid);
	IRuObject*								GetObject(const CRuGUID &guid, BOOL &asyncPendingOut);
	IRuObject*								GetObjectWithFilter(const CRuGUID &guid, BOOL &asyncPendingOut);

	// Picking
	IRuObject*								PickObject(const CRuSegment3 &segment, CRuVector3 &intPtOut, float &intDistOut);

	// Visibility chaining
	BOOL									ChainObjectVisibility(const CRuGUID &guid);

	// 
	BOOL									SetSpecialEventFilter(const char *filter);

	// Enumeration
	BOOL									EnumerateAllObjectsByType(const CRuRTTI &objectType, CRuArrayList<IRuObject *> &objectsOut);
	BOOL									EnumerateVisibleObjects(CRuArrayList<IRuObject *> &objectsOut);
	BOOL									EnumerateVisibleObjectsByType(CRuArrayList<IRuObject *> &objectsOut, const CRuRTTI &objectType);

	BOOL									EnumerateVisibleObjectsByTypeWithFilter(CRuArrayList<IRuObject *> &objectsOut, const CRuRTTI &objectType);

	BOOL									UpdateEnumeration(const CRuAABB &enumerationBounds);
	BOOL									Update(REAL elapsedTime);

	CRuWorld_ObjectCell*					GetCell(UINT32 cellKey);
	CRuGUID									GetNextEnumeratedObjectGUID();
	CRuArrayList<CRuGUID>&					GetEnumeratedObjectGUIDList();

	REAL									GetLoadingStatus();

	// CRuWorld_EntityDescriptor-specific stuff
	CRuWorld_EntityDescriptor*				GetEntityDescriptor(const CRuGUID &guid, BOOL &asyncPendingOut);
	CRuEntity*								GetEntityInstance(const CRuGUID &guid, BOOL &asyncPendingOut);

	// Queue management
	BOOL									ClearObjectQueue();
	BOOL									ClearInstanceQueue();
	BOOL									GarbageCollectInstanceQueue();

	// Conversion & tools
	BOOL									ReinsertObjects();

protected:
	CRuWorld_CellLink*						GetCellLinkObject(IRuObject *object);

	IRuObject*								GetObjectFromChangeList(const CRuGUID &guid);

	BOOL									ObjectQueueMemoryFootprintCallback(const void *key, void *data);
	BOOL									InstanceQueueMemoryFootprintCallback(const void *key, void *data);

	BOOL									ClearObjectQueueCallback(const void *key, void *data);
	BOOL									ClearInstanceQueueCallback(const void *key, void *data);

	BOOL									GarbageCollectInstanceQueueCallback(const void *key, void *data);

	BOOL									ClearCellHash();
	BOOL									ClearCellHashCallback(const void *key, void *data);

	BOOL									SaveCells();
	BOOL									SaveCellsCallback(const void *key, void *data);

	IRuObject*								FilterObject(IRuObject *obj, const CRuGUID &objGUID);

	BOOL									WorkerFunction();

#ifdef KALYDO
	static void								DownloadCallback(const BYTE* key, void* userData);
	void									KalydoQueue(const CRuGUID &guid);

	static void								DownloadCellCallback(const BYTE* key, void* userData);
	void									KalydoQueueCell(UINT32 cell);
#endif // KALYDO
};

// ************************************************************************************************************************************************************

class CRuWorld_ObjectManager : public CRuEntity_Container_Impl
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	enum ObjectState
	{
		ruOBJMOTION_NONE					= 0,
		ruOBJMOTION_IDLE					= 1
	};

	struct EntityDesc
	{
		CRuWorld_EntityDescriptor*			m_descriptor;
		CRuEntity*							m_entity;
		CRuSphere							m_entityBounds;

		ObjectState							m_objectState;
	};

	CRuWorld_ObjectContainer*				m_objectContainer;					//!< Entity container

	DWORD									m_lastEnumerationClearTick;			//!< Time of last enumeration clear
	REAL									m_enumerationMarginSize;			//!< Size of margin to add to enumeration bounds
	CRuAABB									m_enumerationBounds;				//!< Current enumeration bounds including margin
	CRuVector3								m_enumerationCenter;

	CRuKeyHash<CRuGUID>*					m_enumerationHash;					//!< Temporary hash for storing entity descriptor GUIDs
	CRuHashMap<CRuGUID, EntityDesc>*		m_descriptorHash;

	CRuQueue<CRuGUID>						m_entityWorkQueue;

	CRuArrayList<CRuGUID>					m_deletedDescriptorGUIDs;

	REAL									m_alphaBufferDistance;

	BOOL									m_enableDistanceCulling;
	BOOL									m_enableTriggerDisplay;
	BOOL									m_enableTriggerDisplayFilter;
	INT32									m_triggerDisplayFilterID;
	INT32									m_detailClass;

	BOOL									m_cameraPositionIsIndoor;

	// Object partitioning
	CRuPartition_QuadTree*					m_objectTree;							//!< Object partitioning tree

	// Visibility chaining callback data
	CRuArrayList<CRuGUID>					m_visibleEntityGUIDs;

	// Profiler probe handles
	INT32									m_probe_NumObjects;
	INT32									m_probe_NumMissingObjects;
	INT32									m_probe_NumDeferredObjects;
	INT32									m_probe_NumSpritePrtSystems;
	INT32									m_probe_ColQuery_TreeTraversal;

	// Metrics
	INT32									m_numObjects;
	INT32									m_numMissingObjects;
	INT32									m_numDeferredObjects;
	INT32									m_numSpritePrtSystems;

	// Workspace
	CRuArrayList<IRuObject *>				m_ws_Objects;

public:
											CRuWorld_ObjectManager();
	virtual									~CRuWorld_ObjectManager();

	// Container management
	BOOL									Attach(CRuWorld_ObjectContainer *container);
	BOOL									ClearEnumerationCache();
	const CRuAABB&							GetEnumerationBounds();

	BOOL									EnableDistanceCulling(BOOL enable);
	BOOL									EnableTriggerDisplay(BOOL enable);
	BOOL									EnableTriggerDisplayFilter(BOOL enable);
	BOOL									SetTriggerDisplayFilterID(INT32 filterID);
	BOOL									SetDetailClass(INT32 detailClass);
	BOOL									SetSpecialEventFilter(const char *filter);

	BOOL									IsGUIDEnumerated(const CRuGUID &guid);
	BOOL									CameraPositionIsIndoor();

	BOOL									UpdateEnumeration(const CRuAABB &enumerationBounds);
	virtual BOOL							Update(REAL elapsedTime);

	// Collision
	BOOL									QueryCollision(CRuCollisionQuery *colQuery);
	BOOL									QueryCollision(const CRuGUID &guid, CRuCollisionQuery *colQuery);

	REAL									GetLoadingStatus();

	// Visibility chaining
	BOOL									ChainObjectVisibility(const CRuGUID &guid);
	BOOL									UnchainObjectVisibility(const CRuGUID &guid);

	// Enumeration
	BOOL									EnumerateVisibleObjectsByType(CRuArrayList<IRuObject *> &objectsOut, const CRuRTTI &objectType);

protected:
	BOOL									EnumerateVisibleEntities(const void *key, void *data);
	BOOL									ClearDescriptorHashCallback(const void *key, void *data);

	BOOL									ChainObjectVisibilityCallback(const void *key, void *data);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_RuWorld_CellLink, CRuWorld_CellLink)
ruOBJECTMODULE_DEFINE(CRuObjectModule_RuWorld_ObjectCell, CRuWorld_ObjectCell)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
