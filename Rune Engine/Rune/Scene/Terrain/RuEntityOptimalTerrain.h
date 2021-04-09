#ifndef _RUENTITYOPTIMALTERRAIN_H_
#define _RUENTITYOPTIMALTERRAIN_H_

// Rune entity base
#include "../../Core/DataStore/RuDataStore_Base.h"

#include "../../Scene/Base/RuEntityBase.h"

#include "../../Scene/Terrain/RuEntityTerrain_Common.h"
#include "../../Scene/Terrain/RuWorld_Backdrop.h"
#include "../../Scene/Terrain/RuWorld_Base.h"
#include "../../Scene/Terrain/RuWorld_DoodadSystem.h"
#include "../../Scene/Terrain/RuWorld_EntitySystem.h"
#include "../../Scene/Terrain/RuWorld_TriggerSystem.h"
#include "../../Scene/Terrain/RuWorld_Water.h"
#include "../../Scene/Terrain/RuWorld_WaterBodyDescriptor.h"
#include "../../Scene/Terrain/RuWorld_ZoneDescriptorContainer.h"

#include "../../Engine/Collision/RuCollisionStructure_PackedGrid.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

enum RuOptimalTerrainChunkLoadMode
{
	ruOPTIMALTERRAINCHUNKLOADMODE_ANY		= 1,								//!< Load chunk without any preference
	ruOPTIMALTERRAINCHUNKLOADMODE_FULL		= 2,								//!< Load chunk as a full chunk
	ruOPTIMALTERRAINCHUNKLOADMODE_COLLISION	= 3,								//!< Load a chunk as a collision-only chunk
};

// ************************************************************************************************************************************************************

class CRuOptimalTerrain_Chunk : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32									m_chunkKey;

	INT32									m_useIteration;						//!< Usage iteration number

	INT32									m_chunkIteration;					//!< Chunk iteration number
	BOOL									m_dataValidated;					//!< Is all data loaded and validated?
	BOOL									m_isMaskedChunk;					//!< Is the chunk masked? e.g. does it have masked triangles?

	INT32									m_dimension;						//!< Dimensions of the chunk (N x N)
	float									m_gridSize;							//!< Size of grids defining the chunk vertices
	CRuAABB									m_bounds_AABB;						//!< Bounding box for the chunk
	CRuVector3								m_origin;							//!< Origin of the chunk

	REAL									m_chunk_Y_Base;
	REAL									m_chunk_Y_Increment;

	INT32									m_lodLevels;						//!< Number of LOD levels
	INT32									m_numVertices;						//!< Number of vertices, including the skirt vertices
	INT32									m_numIndices;						//!< Number of indices, including all LOD and skirt indices

	BYTE*									m_vertexFlags;
	CRuMesh*								m_mesh;								//!< Chunk mesh data
	CRuCollisionStructure_PackedGrid*		m_colStructure;						//!< Chunk mesh collision structure

	CRuArrayList<CRuWorld_DoodadCluster *>	m_doodadLists;						//!< List of doodad arrays
	CRuArrayList<CRuWorld_Water *>			m_waterLayers;						//!< List of water layers in this chunk

	CRuMaterial*							m_material;							//!< Material used by the chunk
	CRuArrayList<CRuProxyMesh *>			m_lodMeshes;						//!< Array of LOD meshes
	CRuArrayList<CRuProxyMesh **>			m_tileLinkMeshes;					//!< Array of tile locking meshes
	INT32									m_currentLOD;						//!< Currently selected LOD mesh
	INT32									m_currentTileLink;					//!< Currently selected tile link mesh

	INT32									m_splatResolution;					//!< Splat ID map resolution
	CRuSplatIDMap*							m_splatIDMap;						//!< Splat ID map

	INT32									m_alphaTextureLOD;					//!< Current alpha texture LOD

	REAL									m_alphaUVModifier[4];
	REAL									m_textureUVScale[6][4];

public:
											CRuOptimalTerrain_Chunk();
	virtual									~CRuOptimalTerrain_Chunk();

	INT32									GetChunkKey();
	void									SetChunkKey(INT32 chunkKey);

	// Usage iteration
	INT32									GetUseIteration() const;
	INT32									SetUseIteration(INT32 useIteration);

	// IRuObject serialization
	BOOL									SerializeFrom(IRuStream *stream, RuOptimalTerrainChunkLoadMode loadMode, IRuMeshTopology *meshTopology);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Memory reporting
	virtual INT32							GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	// Collision structure
	BOOL									AttachCollisionStructure(CRuCollisionStructure_PackedGrid *colStructure);

	// IRuEntity_Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// LOD selection
	void									SetCamera(CRuCamera *viewCamera);
	INT32									GetLOD();
	void									SetLOD(INT32 lod);
	void									SetTileLink(INT32 tileLink);

	// Splat ID map management
	void									ReleaseSplatIDMap();

	void									SetTexture(INT32 layer, IRuBaseTexture *texture, REAL tileScale);

	// Information query
	INT32									GetChunkIteration() const;
	BOOL									IsMaskedChunk() const;
	const CRuAABB&							GetBoundingBox() const;
	CRuSplatIDMap*							GetSplatIDMap();

	// Doodads
	INT32									GetNumDoodadClusters() const;
	CRuWorld_DoodadCluster*					GetDoodadCluster(INT32 index);

	// Water surfaces
	INT32									GetNumWaterLayers() const;
	CRuWorld_Water*							GetWaterLayerByIndex(INT32 index);

	INT32									GetMeshDataSize() const;
	INT32									GetSplatDataSize() const;

	void									SetAlphaTextureLOD(INT32 alphaTextureLOD);

protected:
	void									BuildChunkTopology();
	void									UpdateSkirtVertices(CRuVector3 *vertices, DWORD *diffuse);
};

// ************************************************************************************************************************************************************

class IRuOptimalTerrain_DataSource : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuOptimalTerrain_DataSource() { } 

	virtual INT32							IncrementIteration() = 0;
	virtual BOOL							Update(REAL deltaT) = 0;

	virtual BOOL							SetCollisionOnlyMode(BOOL collisionOnlyMode) = 0;

	virtual CRuOptimalTerrain_Chunk*		LoadChunk(INT32 x, INT32 z, RuOptimalTerrainChunkLoadMode loadMode = ruOPTIMALTERRAINCHUNKLOADMODE_ANY) = 0;
	virtual CRuWorld_Backdrop*				LoadBackdrop(INT32 x, INT32 z) = 0;

	virtual BOOL							ChunkLoadFailed(UINT32 chunkKey) = 0;
	virtual BOOL							BackdropLoadFailed(UINT32 chunkKey) = 0;

	virtual CRuTextureList*					GetTextureList() = 0;

	// Water body descriptor
	virtual INT32							GetNumWaterBodyDescriptors() = 0;
	virtual CRuWorld_WaterBodyDescriptor*	LoadWaterBodyDescriptorByIndex(INT32 index) = 0;

	// Chunk header information
	virtual INT32							GetChunkDimension() const  = 0;
	virtual INT32							GetNumXChunks() const = 0;
	virtual INT32							GetNumZChunks() const = 0;
	virtual INT32							GetSplatResolution() const = 0;
	virtual REAL							GetGridSize() const = 0;
	virtual CRuAABB							GetBounds(INT32 x, INT32 z) const = 0;

	// Backdrop header information
	virtual CRuAABB							GetAllBackdropBounds() = 0;
	virtual CRuAABB							GetBackdropBounds(INT32 x, INT32 z) const = 0;
	virtual INT32							GetNumBackdropXChunks() const = 0;
	virtual INT32							GetNumBackdropZChunks() const = 0;

	virtual void							ClearWorkQueues() = 0;

	virtual void							DumpDebugInfo() = 0;
};

// ************************************************************************************************************************************************************

class CRuOptimalTerrain_DataSource_DS : public IRuOptimalTerrain_DataSource
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Data store
	CRuDataStore_Base*						m_dataStore;

	// Other shit
	IRuMeshTopology*						m_commonMeshTopology;
	IRuMeshTopology*						m_commonBackdropMeshTopology;
	CRuCollisionStructure_PackedGrid*		m_commonCollisionStructure;

	// Property settings
	BOOL									m_collisionOnlyMode;				//!< Load only collision data? (For server use)

	// Table handles
	INT32									m_tableHandle_Header;
	INT32									m_tableHandle_Chunk;
	INT32									m_tableHandle_ChunkIteration;
	INT32									m_tableHandle_BackdropHeader;
	INT32									m_tableHandle_Backdrop;
	INT32									m_tableHandle_TextureList;
	INT32									m_tableHandle_WaterDescriptor;

	// Header information
	CRuVector3								m_origin;							//!< Origin of the terrain
	INT32									m_chunkDimension;					//!< Dimensions of each chunk
	INT32									m_numXChunks;						//!< Number of chunks along the X axis
	INT32									m_numZChunks;						//!< Number of chunks along the Z axis
	INT32									m_splatResolution;					//!< Resolution of the splat ID map, excluding padding
	REAL									m_gridSize;							//!< Size of grids defining the chunk vertices
	INT32									m_lodLevels;						//!< Number of LOD levels

	// Backdrop header information
	CRuVector3								m_backdropOrigin;					//!< Origin of the backdrop terrain
	INT32									m_backdropDimension;				//!< Dimensions of each backdrop
	REAL									m_backdropGridSpacing;
	INT32									m_backdropXChunks;					//!< Number of chunks along the X axis
	INT32									m_backdropZChunks;					//!< Number of chunks along the Z axis

	// Cache
	INT32									m_iteration;						//!< Iteration number
	CRuArrayList<DWORD>						m_gc_Chunks;
	CRuHashMap<DWORD, CRuOptimalTerrain_Chunk *>*			m_chunkHash;		//!< Chunk hash
	CRuLRUQueue<INT32, CRuWorld_Backdrop *>*				m_backdropQueue;					//!<

	// Texture list
	CRuTextureList*							m_textureList;

	// Chunk preloading
	CRuThread_WorkQueue<INT32, CRuOptimalTerrain_Chunk *>*	m_chunkWorkQueue;
	CRuThread_WorkQueue<INT32, CRuWorld_Backdrop *>*		m_backdropWorkQueue;

	// Critical sections
	CRuThread_CriticalSection*				m_commonCollisionStructure_CS;

	// Worker thread
	CRuStreamCompressor*					m_worker_StreamCompressor;
	CRuMemoryStream*						m_worker_CompressedChunkStream;
	CRuMemoryStream*						m_worker_DecompressedChunkStream;

	INT32									m_temp_Footprint;
	CRuKeyHash<PTRVALUE>*					m_temp_EnumHash;

public:
											CRuOptimalTerrain_DataSource_DS();
	virtual									~CRuOptimalTerrain_DataSource_DS();

	// Memory reporting
	virtual INT32							GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);
	BOOL									ChunkQueueMemoryFootprintCallback(const void *key, void *data);
	BOOL									BackdropQueueMemoryFootprintCallback(const void *key, void *data);

	virtual INT32							IncrementIteration();
	virtual BOOL							Update(REAL deltaT);

	virtual BOOL							SetCollisionOnlyMode(BOOL collisionOnlyMode);

	virtual CRuOptimalTerrain_Chunk*		LoadChunk(INT32 x, INT32 z, RuOptimalTerrainChunkLoadMode loadMode = ruOPTIMALTERRAINCHUNKLOADMODE_ANY);
	virtual CRuWorld_Backdrop*				LoadBackdrop(INT32 x, INT32 z);

	virtual BOOL							ChunkLoadFailed(UINT32 chunkKey);
	virtual BOOL							BackdropLoadFailed(UINT32 chunkKey);

	virtual CRuTextureList*					GetTextureList();

	// Water descriptors
	virtual INT32							GetNumWaterBodyDescriptors();
	virtual CRuWorld_WaterBodyDescriptor*	LoadWaterBodyDescriptorByIndex(INT32 index);

	// Chunk header information
	virtual INT32							GetChunkDimension() const;
	virtual INT32							GetNumXChunks() const;
	virtual INT32							GetNumZChunks() const;
	virtual INT32							GetSplatResolution() const;
	virtual REAL							GetGridSize() const;
	virtual CRuAABB							GetBounds(INT32 x, INT32 z) const;

	// Backdrop header information
	virtual CRuAABB							GetAllBackdropBounds();
	virtual CRuAABB							GetBackdropBounds(INT32 x, INT32 z) const;
	virtual INT32							GetNumBackdropXChunks() const;
	virtual INT32							GetNumBackdropZChunks() const;

	virtual void							ClearWorkQueues();

	virtual void							DumpDebugInfo() { }

	CRuDataStore_Base*						GetDataStore();

	BOOL									Open(CRuDataStore_Base *dataStore);

protected:
	BOOL									LoadHeader();
	BOOL									LoadBackdropHeader();
	BOOL									LoadTextureList(CRuTextureList *textureList);

	BOOL									GCChunkQueue();
	BOOL									GCChunkQueueCallback(const void *key, void *data);

	BOOL									ClearChunkQueue();
	static BOOL								ClearChunkQueueCallback(const void *key, const void *data, void *userData);

	BOOL									ClearBackdropQueue();
	static BOOL								ClearBackdropQueueCallback(const void *key, const void *data, void *userData);

	BOOL									AttachCollisionStructure(CRuOptimalTerrain_Chunk *chunk);
	BOOL									DetachCollisionStructure(CRuOptimalTerrain_Chunk *chunk);

	CRuOptimalTerrain_Chunk*				CreateChunk(BOOL minimalChunk);
	BOOL									DestroyChunk(CRuOptimalTerrain_Chunk *chunk, BOOL minimalChunk);

	BOOL									WorkerFunction();

#ifdef KALYDO
	static void KalydoBackdropDownloaded(const BYTE* key, void* userData);
	static void KalydoChunkDownloaded(const BYTE* key, void* userData);

	void KalydoQueue(DWORD key, BOOL isBackdrop);

#endif // KALYDO
};

// ************************************************************************************************************************************************************

class CRuOptimalTerrain : public IRuEntity_Container
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Options
	BOOL									m_enableCollisionData;

	// Settings
	BOOL									m_enableWaterReflection;
	BOOL									m_enableWaterRefraction;

	// Data source
	IRuOptimalTerrain_DataSource*			m_dataSource;

	// Spatial partitioning structure
	CRuAABB									m_terrainBounds;
	CRuWorld_ChunkTree*						m_chunkTree;
	CRuWorld_ChunkTree*						m_backdropTree;

	// Current view volume
	REAL									m_configuredViewDistance;			//!< View distance for which this terrain object is configured for
	REAL									m_viewVolumeMarginSize;				//!< Size of margins to insert to the camera sweep volume
	CRuAABB									m_viewVolume;
	CRuAABB									m_standardViewVolume;

	// Backdrop system
//	CRuLRUQueue<DWORD, CRuWorld_Backdrop *>*m_backdropQueue;

	// Entity manager
	CRuWorld_ObjectContainer*				m_objectContainer;					//!<
	CRuWorld_ObjectManager*					m_objectManager;

	CRuWorld_DoodadContainer*				m_doodadContainer;					//!< Doodad container
	CRuWorld_DoodadManager*					m_doodadManager;					//!< Doodad manager

	CRuWorld_ZoneDescriptorContainer*		m_zoneDescriptorContainer;			//!< Zone descriptor container

	CRuPathHierarchy_DS*					m_pathHierarchy;					//!< Pathing hierarchy

	// Visible entities
	CRuHashMap<INT32, CRuOptimalTerrain_Chunk *>*				m_enumeratedChunkHash;
	CRuArrayList<CRuOptimalTerrain_Chunk *>	m_enumeratedChunks;					//!< List of enumerated chunks

	CRuArrayList<INT32>						m_enumeratedDoodadKeys;
	CRuArrayList<CRuWorld_DoodadCluster *>	m_enumeratedDoodadClusters;

	CRuEntity*								m_chunkContainer;
	CRuEntity*								m_backdropChunkContainer;
	CRuEntity*								m_waterContainer;

	// Water body
	CRuArrayList<CRuWorld_WaterBodyDescriptor *>				m_waterBodyDescriptors;		//!< List of water body descriptors
	CRuHashMap<CRuGUID, CRuWorld_WaterBodyDescriptor *>*		m_waterBodyDescriptorMap;	//!< Map of GUID -> Water Body Descriptor

	// Texture list
	CRuTextureList*							m_textureList;

	CRuVector3								m_movementDirection;

	// View options
	INT32									m_terrainDetailLevel;				//!< Terrain LOD detail level
	INT32									m_terrainSplatDetailLevel;			//!< Terrain splat detail level
	INT32									m_terrainSplatTechniqueLevel;		//!< Technique level to use for splatting
	INT32									m_detailClassLevel;					//!< Terrain objects detail class level
	BOOL									m_renderTerrain;
	BOOL									m_renderTerrainTexture;
	BOOL									m_renderBackdropTerrain;
	BOOL									m_renderObjects;
	BOOL									m_renderGrass;
	BOOL									m_renderWater;

	// Workspace
	CRuArrayList<DWORD>						m_ws_chunkKeyList;
	CRuArrayList<CRuAABB>					m_ws_chunkBoundsList;

	// Enumeration work queue
	CRuQueue<DWORD>							m_chunkWorkQueue;
	CRuQueue<DWORD>							m_backdropWorkQueue;

	// Metrics
	INT32									m_metric_NumCacheMisses;
	INT32									m_metric_NumBackdropCacheMisses;
	INT32									m_metric_NumPreloadedChunks;
	INT32									m_metric_NumPreloadedBackdropChunks;
	INT32									m_metric_NumEnumeratedEntities;

	INT32									m_metric_NumChunksLastEnumeration;

	// Probes
	INT32									m_probe_NumCacheMisses;
	INT32									m_probe_NumBackdropCacheMisses;
	INT32									m_probe_NumPreloadedChunks;
	INT32									m_probe_NumPreloadedBackdropChunks;
	INT32									m_probe_CollisionQuery;

public:
											CRuOptimalTerrain();
	virtual									~CRuOptimalTerrain();

	// IRuObject serialization
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Memory reporting
	virtual INT32							GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	// Entity interface
	virtual BOOL							ResetStates();
	virtual BOOL							Update(float elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// Initialization
	BOOL									Initialize();
	BOOL									AttachDataSource(IRuOptimalTerrain_DataSource *dataSource, BOOL enableCollisionData = FALSE);
	BOOL									PreLoadChunks(const CRuAABB &bounds, RuOptimalTerrainChunkLoadMode chunkLoadMode);

	// Client-side load and feedback
	REAL									GetChunkLoadingStatus();
	REAL									GetLoadingStatus();

	// Queue management
	BOOL									ModifyViewDistanceConfiguration(REAL viewDistance);

	// LOD selection and frustum culling
	void									SetMovementDirection(const CRuVector3 &direction);
	void									SetCamera(CRuCamera *viewCamera);

	const CRuAABB&							GetTerrainBounds() const;
	const CRuAABB&							GetEnumerationBounds() const;

	INT32									GetNumXChunks() const;
	INT32									GetNumZChunks() const;

	void									GetDoodadEnumeration(CRuArrayList<INT32> **enumeratedKeys, CRuArrayList<CRuWorld_DoodadCluster *> **enumeratedClusters)
											{
												*enumeratedKeys = &m_enumeratedDoodadKeys;
												*enumeratedClusters = &m_enumeratedDoodadClusters;
											}


	BOOL									ClearEnumerationCache();

	// Other shit
	INT32									IsChunkLoadedAt(const CRuVector3 &point);
	DWORD									GetChunkKeyAt(const CRuVector3 &point);

	BOOL									EnumerateTriggers(RuWorld::TriggerType triggerType, CRuArrayList<CRuWorld_Trigger *> &triggersOut);

	BOOL									GetTriggers(const CRuVector3 &point, CRuArrayList<CRuWorld_Trigger *> &triggersOut);
	CRuWorld_Trigger*						GetApplicableTrigger(RuWorld::TriggerType triggerType, const CRuVector3 &point, BOOL limitRange);
	CRuWorld_Trigger*						GetApplicableTrigger(RuWorld::TriggerType *triggerTypes, const CRuVector3 &point, BOOL limitRange);

	INT32									GetZoneID(const CRuVector3 &point);
	RuWorld::ZoneType						GetZoneType(const CRuVector3 &point);
	CRuWorld_ZoneDescriptor*				GetZoneDescriptor(const CRuVector3 &point);
	CRuWorld_ZoneDescriptor*				GetZoneDescriptorByID(INT32 zoneID);
	BOOL									IsSubZoneOf(INT32 subZoneID, INT32 parentZoneID);

	CRuWorld_WaterBodyDescriptor*			GetWaterBodyDescriptorAtPoint(const CRuVector3 &point);

	// Trigger enumeration
	BOOL									EnumerateAllTriggersByZoneID(INT32 zoneID, CRuArrayList<CRuWorld_Trigger *> &triggersOut);
	BOOL									EnumerateAllTriggersByZoneID(CRuArrayList<INT32> zoneIDList, CRuArrayList<CRuWorld_Trigger *> &triggersOut);

	// Sub containers and managers
	IRuOptimalTerrain_DataSource*			GetDataSource();
	CRuWorld_ObjectContainer*				GetObjectContainer();
	CRuWorld_ObjectManager*					GetObjectManager();
	CRuWorld_DoodadContainer*				GetDoodadContainer();
	CRuWorld_ZoneDescriptorContainer*		GetZoneDescriptorContainer();
	IRuPathHierarchy_Base*					GetPathHierarchy();

	// Viewing options
	void									SetTerrainSplatDetailLevel(INT32 detailLevel);
	void									ToggleOption_RenderTerrain();
	void									ToggleOption_RenderTerrainTexture();
	void									ToggleOption_RenderBackdropTerrain();
	void									ToggleOption_RenderObjects();
	void									ToggleOption_RenderGrass();
	void									ToggleOption_RenderWater();

	// Event handlers
	BOOL									EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting);

protected:
	BOOL									ApplyEngineSettings();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_OptimalTerrain_Chunk, CRuOptimalTerrain_Chunk)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
