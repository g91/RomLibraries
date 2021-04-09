#ifndef _RUENTITYTERRAIN_H_
#define _RUENTITYTERRAIN_H_

// Rune entity base
#include "../../Core/DataStore/RuDataClient.h"

#include "../../PathEngine/RuPathHierarchy_DS.h"

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Controller/RuController_Highlighter.h"

#include "../../Scene/Terrain/RuEntityTerrain_Common.h"
#include "../../Scene/Terrain/RuWorld_Backdrop.h"
#include "../../Scene/Terrain/RuWorld_Base.h"
#include "../../Scene/Terrain/RuWorld_DoodadSystem.h"
#include "../../Scene/Terrain/RuWorld_EntitySystem.h"
#include "../../Scene/Terrain/RuWorld_Water.h"
#include "../../Scene/Terrain/RuWorld_WaterBodyDescriptor.h"
#include "../../Scene/Terrain/RuWorld_ZoneDescriptorContainer.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

enum RuTerrainChunkLoadMode
{
	ruTERRAINCHUNKLOADMODE_ANY			= 1,								//!< Load chunk without any preference
	ruTERRAINCHUNKLOADMODE_FULL			= 2,								//!< Load chunk as a full chunk
	ruTERRAINCHUNKLOADMODE_PARTIAL		= 3,								//!< Load chunk as a partial chunk
	ruTERRAINCHUNKLOADMODE_COLLISION	= 4,								//!< Load a chunk as a collision-only chunk
	ruTERRAINCHUNKLOADMODE_MINIMAL		= 5,								//!< Load a chunk as a minimal chunk
	ruTERRAINCHUNKLOADMODE_OOB			= 6									//!< Force out-of-band chunk loading and load as a partial chunk (not yet implemented)
};

// ************************************************************************************************************************************************************

class CRuTerrain_Chunk : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32									m_chunkIteration;					//!< Chunk iteration number

	INT32									m_dimension;						//!< Dimensions of the chunk (N x N)
	INT32									m_heightFieldDimension;				//!< Dimensions of the heightfield [(m_dimension + 3) x (m_dimension + 3)]
	float									m_gridSize;							//!< Size of grids defining the chunk vertices
	CRuAABB									m_bounds;							//!< Bounding box for the chunk
	CRuAABB									m_heightFieldBounds;				//!< Bounding box for the height field
	CRuVector3								m_origin;							//!< Origin of the chunk

	INT32									m_lodLevels;						//!< Number of LOD levels
	INT32									m_numVertices;						//!< Number of vertices, including the skirt vertices
	INT32									m_numIndices;						//!< Number of indices, including all LOD and skirt indices

	DWORD									m_chunkKey;							//!< Chunk key
	UINT32									m_chunkFlags;						//!< Flags shared by all triangles of the chunk
																				//!< | 3 3 2 2 2 2 2 2 2 2 2 2 | 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0 |
																				//!< | 1 0 9 8 7 6 5 4 3 2 1 0 | 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 |
																				//!< ---------------------------------------------------------------------
																				//!< |                         |                   ID                    |

	CRuMojo_AttributeParcel<REAL>*			m_hfObject;							//!< Height field object
	REAL*									m_heightField;						//!< Shortcut reference pointer to the height field in m_hfObject
	REAL*									m_heightAutoOffset;					//!< Height offset caused by auto-painting of height
	BYTE*									m_heightAutoPainted;				//!< 1 if the height field point was painted, 0 otherwise
	BYTE*									m_vertexFlags;

	CRuMesh*								m_mesh;								//!< Chunk mesh
	IRuCollisionStructure*					m_colStructure;						//!< Chunk mesh collision structure

	CRuMesh*								m_outlineMesh;						//!< Chunk outline mesh
	CRuMaterial*							m_outlineMaterial;					//!< Chunk outline material

	CRuArrayList<CRuWorld_DoodadCluster *>	m_doodadLists;						//!< List of doodad arrays
	CRuArrayList<CRuWorld_Water *>			m_waterLayers;						//!< List of water layers in this chunk

	CRuMaterial*							m_material;							//!< Material used by the chunk
	CRuController_Highlighter*				m_highlightController;
	CRuProxyMesh**							m_lodMeshes;						//!< Array of LOD meshes
	INT32									m_currentLOD;						//!< Currently selected LOD mesh

	BOOL									m_highlightOn;						//!< Is the chunk highlighted?
	BOOL									m_outlineOn;						//!< Is the chunk outlined?

	INT32									m_splatResolution;					//!< Splat ID map resolution
	CRuSplatIDMap*							m_splatIDMap;						//!< Splat ID map

	// Run-time flags
	BOOL									m_offsetPainted;

	REAL									m_textureUVScale[6][4];

public:
											CRuTerrain_Chunk();
	virtual									~CRuTerrain_Chunk();

	// New chunk initialization
	BOOL									Allocate(INT32 dimension, float gridSize, const CRuVector3 &chunkOrigin, INT32 lodLevels, INT32 splatResolution);

	// IRuObject serialization
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	BOOL									SerializeFrom(IRuStream *stream, RuTerrainChunkLoadMode loadMode);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);
	BOOL									SerializeToOptimizedChunk(IRuStream *stream, CRuSplatMapBuilder *splatMapBuilder);

	// Collision structure
	BOOL									AttachCollisionStructure(IRuCollisionStructure *colStructure);

	// IRuEntity_Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	virtual void							CalculateEntityBounds();

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// Misc
	DWORD									GetChunkKey() const;
	void									SetChunkKey(DWORD chunkKey);
	UINT32									GetChunkFlags() const;
	void									SetChunkFlags(UINT32 chunkFlags);

	// LOD selection
	void									SetCamera(CRuCamera *viewCamera);
	void									SetHighlight(BOOL highlight, RUCOLOR color = 0xFFFFFF00, REAL alpha = 0.3f);
	void									SetOutline(BOOL outline);

	// Information query
	INT32									GetChunkIteration() const;
	REAL									GetGridSize() const;
	INT32									GetLODLevels() const;
	const CRuAABB&							GetBoundingBox() const;
	CRuSplatIDMap*							GetSplatIDMap();
	CRuMojo_AttributeParcel<REAL>*			GetHeightField();

	// Flags
	BOOL									PaintFlags(const CRuVector3 &center, float radius, UINT32 flags, UINT32 flagMask);

	// Height field deformation
	BOOL									PaintHeight(const CRuVector3 &center, float radius, float delta, float refHeight, BOOL adaptivePainting, RuFalloffFunction falloffFunction);
	BOOL									NoiseHeight(const CRuVector3 &center, float radius, float variation);
	BOOL									SetHeight(const CRuVector3 &center, float radius, float height, float brushStrength, RuFalloffFunction falloffFunction);
	BOOL									SmoothHeight(const CRuVector3 &center, float radius, float delta);

	BOOL									Stitch(INT32 neighborID, CRuTerrain_Chunk *chunk);

	BOOL									ExportHeight(float *destHeightMap, INT32 destOffset, INT32 destPitch);
	BOOL									ImportHeight(float *srcHeightMap, INT32 srcOffset, INT32 srcPitch);

	BOOL									AutoPaintHeight(const CRuVector3 &center, float radius, CRuFractal_Base *srcFractal, const CRuSplatIDMap_PaintSetting &paintSetting);

	// Masking
	BOOL									PaintMask(const CRuVector3 &center, float radius, BOOL maskOn);

	// Offsetting
	BOOL									PaintOffset(const CRuVector3 &center, float radius, REAL offset);

	// Splatting
	BOOL									PaintSplat(const CRuVector3 &center, float radius, REAL power, RuFalloffFunction falloffFunc, INT32 iteration, UINT16 splatID);
	BOOL									SprayPaintSplat(const CRuVector3 &center, float radius, REAL power, RuFalloffFunction falloffFunc, INT32 iteration, UINT16 splatID, float sprayProbability);
	BOOL									BlockFillSplat(const CRuVector3 &center, float radius, UINT16 splatID);
	BOOL									FloodFillSplat(const CRuVector3 &center, float radius, UINT16 splatID);
	BOOL									SmudgeSplat(const CRuVector3 &center, float radius, float smudgeProbability);
	BOOL									ReplaceSplat(const CRuVector3 &center, float radius, UINT16 splatID);
	BOOL									AttractSplat(const CRuVector3 &center, float radius, UINT16 attractedSplatID, float attractProbability);
	UINT16									PickSplatID(const CRuVector3 &center);

	void									SetTexture(INT32 layer, IRuBaseTexture *texture, REAL tileScale);

	// Auto & fractal painting
	BOOL									MojoPaint(const CRuVector3 &center, float radius, CRuMojo_AttributeParcel<REAL> *refHF, CRuFractal_Base *srcFractal, const CRuSplatIDMap_PaintSetting &paintSetting);

	// Doodad manipulation
	void									PaintDoodad(const CRuVector3 &center, const CRuWorld_DoodadPaintSetting &paintSetting, CRuEntity *colObject);
	void									SprayDoodad(const CRuVector3 &center, REAL radius, INT32 numDoodads, const CRuWorld_DoodadPaintSetting &paintSetting, CRuEntity *colObject);
	void									DeleteDoodad(const CRuVector3 &center, REAL radius);
	void									RealignDoodad(const CRuVector3 &center, REAL radius);
	INT32									GetNumDoodadClusters();
	CRuWorld_DoodadCluster*					GetDoodadCluster(INT32 index);


	CRuWorld_DoodadCluster*					GetDoodadListDescriptor(const CRuGUID &guid);

	// Water surfaces
	INT32									GetNumWaterLayers() const;
	CRuEntity*								GetWaterLayerEntity(INT32 index);

	CRuWorld_Water*							GetNearestWaterLayer(const CRuSegment3 &segment, CRuVector3 &intPtOut, float &intDistOut);
	CRuWorld_Water*							GetWaterLayerByGUID(const CRuGUID &waterBodyGUID);
	CRuWorld_Water*							GetWaterLayerByIndex(INT32 index);
	INT32									GetWaterLayerIndexByGUID(const CRuGUID &waterBodyGUID);

	BOOL									PaintWaterHeight(const CRuGUID &waterBodyGUID, const CRuVector3 &center, float radius, float delta, RuFalloffFunction falloffFunction);
	BOOL									SetWaterHeight(const CRuGUID &waterBodyGUID, const CRuVector3 &center, float radius, float height, float brushStrength, RuFalloffFunction falloffFunction);
	BOOL									SmoothWaterHeight(const CRuGUID &waterBodyGUID, const CRuVector3 &center, float radius, float delta);
	BOOL									PaintWaterSurface(const CRuGUID &waterBodyGUID, const CRuVector3 &center, float radius, BOOL waterVisible, REAL waterBodyTessellationScale);
	BOOL									PaintWaterBodyAlpha(const CRuGUID &waterBodyGUID, const CRuVector3 &center, REAL radius, REAL alpha, REAL delta, RuFalloffFunction falloffFunction);
	BOOL									PaintWaterBodyColor(const CRuGUID &waterBodyGUID, const CRuVector3 &center, REAL radius, RUCOLOR color, REAL delta, RuFalloffFunction falloffFunction);
	RUCOLOR									PickWaterBodyColor(const CRuGUID &waterBodyGUID, const CRuVector3 &center);

	BOOL									StitchWaterLayers(INT32 neighborID, CRuTerrain_Chunk *chunk);

	BOOL									DeleteWaterLayerIfUnused(const CRuGUID &waterBodyGUID);

	// Static helpers
	static INT32							GetChunkSize(INT32 dimension, INT32 splatResolution);

	// Utilities
	void									CalculateBounds();
	void									CalculateNormals();

protected:
	// Basic parameter setup
	void									CalculateChunkParameters();

	// Vertex and normal
	void									InitializeMeshVertices(float xOrigin, float zOrigin);

	// Topology building
	void									BuildChunkTopology();
	INT32									BuildGridIndices(INT32 gridDimension, INT32 vertexGap, INT32 stride, UINT16 *indicesOut);
	void									UpdateVertices();
	void									UpdateSkirtVertices();

	// Splat ID map
	void									InitializeSplatIDMap(INT32 splatResolution);
	void									FixSplatIDMapBounds(INT32 splatResolution);
};

// ************************************************************************************************************************************************************

class IRuTerrain_DataSource : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuTerrain_DataSource() { } 

	// Undo/redo
	virtual BOOL							Undo_Begin() = 0;
	virtual BOOL							Undo_End() = 0;
	virtual BOOL							Undo() = 0;
	virtual BOOL							Redo() = 0;

	// Chunk
	virtual CRuTerrain_Chunk*				LockChunk(INT32 x, INT32 z) = 0;
	virtual BOOL							UnlockChunk(INT32 x, INT32 z) = 0;
	virtual CRuTerrain_Chunk*				MarkChunk(INT32 x, INT32 z) = 0;
	virtual BOOL							UnmarkChunk(INT32 x, INT32 z) = 0;
	virtual CRuTerrain_Chunk*				LoadChunk(INT32 x, INT32 z) = 0;
	virtual CRuTerrain_Chunk*				LoadMinimalChunk(INT32 x, INT32 z) = 0;
	virtual BOOL							SaveChunk(INT32 x, INT32 z, CRuTerrain_Chunk *chunk) = 0;

	// Editor crap
	virtual void							SetEditingEnvironmentColors(DWORD primaryColor, DWORD secondaryColor, DWORD specularColor, DWORD fogColor, float theta, float phi) = 0;
	virtual void							GetEditingEnvironmentColors(DWORD &primaryColor, DWORD &secondaryColor, DWORD &specularColor, DWORD &fogColor, float &theta, float &phi) = 0;

	// Texture list
	virtual BOOL							LockTextureList() = 0;
	virtual BOOL							UnlockTextureList() = 0;
	virtual BOOL							LoadTextureList(CRuTextureList *textureList) = 0;
	virtual BOOL							SaveTextureList(CRuTextureList *textureList, BOOL forceSave = FALSE) = 0;

	// Water body descriptor
	virtual INT32							GetNumWaterBodyDescriptors() = 0;
	virtual CRuWorld_WaterBodyDescriptor*	LoadWaterBodyDescriptorByIndex(INT32 index) = 0;
	virtual BOOL							SaveWaterBodyDescriptor(CRuWorld_WaterBodyDescriptor *waterBodyDescriptor) = 0;
	virtual BOOL							DeleteWaterBodyDescriptorByIndex(const CRuGUID &waterBodyGUID) = 0;

	virtual BOOL							FlushChunkQueue() = 0;
	virtual BOOL							FlushLockedChunks() = 0;

	virtual BOOL							FlushEditingEnvironment() = 0;

	virtual const CRuVector3&				GetOrigin() const = 0;
	virtual INT32							GetChunkDimension() const  = 0;
	virtual INT32							GetNumXChunks() const = 0;
	virtual INT32							GetNumZChunks() const = 0;
	virtual INT32							GetSplatResolution() const = 0;
	virtual INT32							SetSplatResolution(INT32 splatResolution) = 0;
	virtual REAL							GetGridSize() const = 0;
	virtual INT32							GetLODLevels() const = 0;
	virtual const CRuAABB&					GetTerrainBounds() const = 0;
	virtual CRuAABB							GetChunkBounds(INT32 x, INT32 z) const = 0;
	virtual INT32							GetChunkIterationNumber(INT32 x, INT32 z) = 0;

	virtual BOOL							ChunkIsMarked(CRuTerrain_Chunk *chunk) const = 0;
	virtual INT32							GetNumMarkedChunks() const = 0;
	virtual CRuTerrain_Chunk*				GetMarkedChunkByIndex(INT32 index) = 0;
};

// ************************************************************************************************************************************************************

class CRuTerrain_DataSource_Remote : public IRuTerrain_DataSource
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Data client
	CRuDataStore_Base*						m_dataStore;

	// Other shit
	CRuCollisionStructure_Quadtree*			m_commonCollisionStructure;

	// Table handles
	INT32									m_tableHandle_Header;
	INT32									m_tableHandle_Chunk;
	INT32									m_tableHandle_TextureList;
	INT32									m_tableHandle_WaterDescriptor;

	// Runtime information
	INT32									m_chunkHeaderSize;					//!< Size of each chunk header
	INT32									m_textureListSize;					//!< Size of the texture table
	INT32									m_chunkHeaderOffset;				//!< Offset to the first chunk header(0, 0)
	INT32									m_textureTableOffset;				//!< Offset to the texture table
	INT32									m_entityHeaderOffset;				//!< Offset to the entity container header
	CRuAABB									m_terrainBounds;					//!< Full terrain bounding box

	// Header information (non-trampleable area)
	INT32									m_version;							//!< Version of the file
	CRuVector3								m_origin;							//!< Origin of the terrain
	INT32									m_chunkDimension;					//!< Dimensions of each chunk
	INT32									m_numXChunks;						//!< Number of chunks along the X axis
	INT32									m_numZChunks;						//!< Number of chunks along the Z axis
	INT32									m_splatResolution;					//!< Resolution of the splat ID map, excluding padding
	REAL									m_gridSize;							//!< Size of grids defining the chunk vertices
	INT32									m_lodLevels;						//!< Number of LOD levels

	// Editor crap (trampleable area)
	DWORD									m_editSetting_PrimaryColor;
	DWORD									m_editSetting_SecondaryColor;
	DWORD									m_editSetting_SpecularColor;
	DWORD									m_editSetting_fogColor;
	float									m_editSetting_Theta;
	float									m_editSetting_Phi;

	// Cache
	CRuLRUQueue<DWORD, CRuTerrain_Chunk *>*	m_chunkQueue;						//!< LRU queue of chunks

	// Lists of marked and locked chunks
	CRuArrayList<CRuTerrain_Chunk *>		m_lockedChunks;						//!< List of locked chunks
	CRuArrayList<CRuTerrain_Chunk *>		m_markedChunks;						//!< List of marked chunks

	// Undo/Redo stacks
	BOOL									m_undoActive;
	CRuArrayList<DWORD>						m_currentUndoList;
	CRuStack<INT64>							m_undoStack;

	// Utility objects
	CRuStreamCompressor*					m_streamCompressor;					//!< Stream compression/decompression utility object
	CRuMemoryStream*						m_compressedChunkStream;			//!< Temporary buffer space for compressed streams
	CRuMemoryStream*						m_decompressedChunkStream;			//!< Temporary buffer space for decompressed streams

public:
											CRuTerrain_DataSource_Remote();
	virtual									~CRuTerrain_DataSource_Remote();

	// Undo/redo
	virtual BOOL							Undo_Begin();
	virtual BOOL							Undo_End();
	virtual BOOL							Undo();
	virtual BOOL							Redo();

	// Chunk
	virtual CRuTerrain_Chunk*				LockChunk(INT32 x, INT32 z);
	virtual BOOL							UnlockChunk(INT32 x, INT32 z);
	virtual CRuTerrain_Chunk*				MarkChunk(INT32 x, INT32 z);
	virtual BOOL							UnmarkChunk(INT32 x, INT32 z);
	virtual CRuTerrain_Chunk*				LoadChunk(INT32 x, INT32 z);
	virtual CRuTerrain_Chunk*				LoadMinimalChunk(INT32 x, INT32 z);
	virtual BOOL							SaveChunk(INT32 x, INT32 z, CRuTerrain_Chunk *chunk);

	// Editor crap
	virtual void							SetEditingEnvironmentColors(DWORD primaryColor, DWORD secondaryColor, DWORD specularColor, DWORD fogColor, float theta, float phi);
	virtual void							GetEditingEnvironmentColors(DWORD &primaryColor, DWORD &secondaryColor, DWORD &specularColor, DWORD &fogColor, float &theta, float &phi);

	// Texture list
	virtual BOOL							LockTextureList();
	virtual BOOL							UnlockTextureList();
	virtual BOOL							LoadTextureList(CRuTextureList *textureList);
	virtual BOOL							SaveTextureList(CRuTextureList *textureList, BOOL forceSave = FALSE);

	// Water descriptors
	virtual INT32							GetNumWaterBodyDescriptors();
	virtual CRuWorld_WaterBodyDescriptor*	LoadWaterBodyDescriptorByIndex(INT32 index);
	virtual BOOL							SaveWaterBodyDescriptor(CRuWorld_WaterBodyDescriptor *waterBodyDescriptor);
	virtual BOOL							DeleteWaterBodyDescriptorByIndex(const CRuGUID &waterBodyGUID);

	BOOL									LoadHeader();
	BOOL									SaveHeader(BOOL saveChunkHeaders = TRUE, BOOL saveEditingEnvironmentOnly = FALSE);

	virtual BOOL							FlushChunkQueue();
	virtual BOOL							FlushLockedChunks();

	virtual BOOL							FlushEditingEnvironment();

	virtual const CRuVector3&				GetOrigin() const;
	virtual INT32							GetChunkDimension() const;
	virtual INT32							GetNumXChunks() const;
	virtual INT32							GetNumZChunks() const;
	virtual INT32							GetSplatResolution() const;
	virtual INT32							SetSplatResolution(INT32 splatResolution);
	virtual REAL							GetGridSize() const;
	virtual INT32							GetLODLevels() const;
	virtual const CRuAABB&					GetTerrainBounds() const;
	virtual CRuAABB							GetChunkBounds(INT32 x, INT32 z) const;
	virtual INT32							GetChunkIterationNumber(INT32 x, INT32 z);

	virtual BOOL							ChunkIsMarked(CRuTerrain_Chunk *chunk) const;
	virtual INT32							GetNumMarkedChunks() const;
	virtual CRuTerrain_Chunk*				GetMarkedChunkByIndex(INT32 index);

	BOOL									Initialize(const char *serverName, INT32 port, const char *filename, const CRuVector3 &origin, float gridSize, INT32 chunkDimension, INT32 numXChunks, INT32 numZChunks, INT32 lodLevels, INT32 splatResolution);
	BOOL									Initialize(CRuDataStore_Base *dataStore, const CRuVector3 &origin, float gridSize, INT32 chunkDimension, INT32 numXChunks, INT32 numZChunks, INT32 lodLevels, INT32 splatResolution);
	BOOL									Open(const char *serverName, INT32 port, const char *filename);
	BOOL									SaveToOptimizedTerrain(CRuDataStore_Base *dataStore);

	CRuDataStore_Base*						GetDataStore();
	BOOL									RestreamChunks();
	BOOL									CloneFromDS(const char *serverName, INT32 port, const char *filename, IRuTerrain_DataSource *dataSource);
	BOOL									ExpandFromDS(const char *serverName, INT32 port, const char *filename, INT32 xSize, INT32 zSize, IRuTerrain_DataSource *dataSource);
	BOOL									CopyTo(CRuDataStore_Base *dstDataStore);

protected:
	BOOL									Undo_PushChunk(INT32 x, INT32 z, CRuTerrain_Chunk *chunk);
	BOOL									Undo_Flush();
	BOOL									Undo_Trim(INT32 undoDepth);

	BOOL									ClearChunkQueue();
	static BOOL								ClearChunkQueueCallback(const void *key, const void *data, void *userData);
};

// ************************************************************************************************************************************************************

class CRuTerrain : public IRuEntity_Container
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	class ExportWorker
	{
	public:
		CRuSplatMapBuilder*						m_splatMapBuilder;

		CRuStreamCompressor*					m_streamCompressor;
		CRuMemoryStream*						m_compressedStream;
		CRuMemoryStream*						m_decompressedStream;

		CRuTerrain*								m_terrain;

		BOOL									WorkerFunction();
	};

	friend ExportWorker;

	CRuThread_WorkQueue<CRuTerrain_Chunk *, IRuStream *>*m_queue;

	// Current view volume
	REAL									m_viewVolumeMarginSize;				//!< Size of margins to insert to the camera sweep volume
	CRuAABB									m_viewVolume;

	// Terrain system objects
	IRuTerrain_DataSource*					m_dataSource;						//!< Terrain object data source
	CRuWorld_ChunkTree*						m_chunkTree;
	CRuTextureList*							m_textureList;						//!< Terrain texture list

	CRuWorld_ObjectContainer*				m_objectContainer;					//!< Entity descriptor container
	CRuWorld_ObjectManager*					m_objectManager;					//!< Entity manager

	CRuWorld_DoodadContainer*				m_doodadContainer;					//!< Doodad container
	CRuWorld_DoodadManager*					m_doodadManager;					//!< Doodad manager

	CRuWorld_ZoneDescriptorContainer*		m_zoneDescriptorContainer;			//!< Zone descriptor container

	CRuPathHierarchy_DS*					m_pathHierarchy;					//!< Pathing hierarchy

	// Visible entities
	CRuArrayList<CRuTerrain_Chunk *>		m_enumeratedChunks;					//!< List of enumerated chunks

	CRuArrayList<INT32>						m_enumeratedDoodadKeys;
	CRuArrayList<CRuWorld_DoodadCluster *>	m_enumeratedDoodadClusters;

	CRuEntity*								m_chunkContainer;
	CRuEntity*								m_waterContainer;

	CRuArrayList<CRuEntity *>				m_visibleChunks;					//!< List of visible terrain chunks
	CRuArrayList<CRuEntity *>				m_visibleGrass;						//!< List of visible grass groups
	CRuArrayList<CRuEntity *>				m_visibleEntities;					//!< List of visible entities
	CRuArrayList<CRuEntity *>				m_visibleWaterEntities;				//!< List of visible water bodies

	// Workspace
	CRuArrayList<CRuGUID>					m_entityDescriptorGUIDList;			//!< Temporary list for storing entity descriptor GUIDs
	CRuKeyHash<CRuGUID>*					m_entityDescriptorGUIDHash;			//!< Temporary hash for storing entity descriptor GUIDs

	// Water body
	CRuArrayList<CRuWorld_WaterBodyDescriptor *>	m_waterBodyDescriptors;				//!< List of water body descriptors
	CRuHashMap<CRuGUID, CRuWorld_WaterBodyDescriptor *>*			m_waterBodyDescriptorMap;	//!< Map of GUID -> Water Body Descriptor
	CRuGUID									m_selectedWaterBodyGUID;			//!< GUID of the currently selected water body

	CRuLRUQueue<DWORD, CRuMojo_AttributeParcel<REAL> *>*	m_hfQueue;				//!< LRU queue of reference chunk height fields

	// Trigger stuff
	CRuSphere								m_trigger_ActiveBounds;
	BOOL									m_trigger_EnableFilter;
	INT32									m_trigger_FilterID;

	// Utility objects
	CRuSplatMapBuilder*						m_splatMapBuilder;					//!< Splat map builder

	// Editor settings
	BOOL									m_enableSplatFilter;				//!< Enable filtering when drawing splats

	BOOL									m_highlightOn;						//!< Enable active chunk highlight
	CRuSegment3								m_highlightSegment;					//!< Highlighting segment
	UINT32									m_highlightFlags;					//!< Highlighting flags

	BOOL									m_outlineOn;						//!< Enable chunk outlining

	// Settings
	BOOL									m_enableWaterReflection;
	BOOL									m_enableWaterRefraction;

	// Run-time settings
	BOOL									m_renderTerrain;					//!< Enable rendering of terrain chunks
	BOOL									m_renderObjects;					//!< Enable rendering of entities
	BOOL									m_renderGrass;						//!< Enable rendering of grass groups
	BOOL									m_renderWater;						//!< Enable rendering of water bodies

public:
											CRuTerrain();
	virtual									~CRuTerrain();

	// IRuObject serialization
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);
	BOOL									SerializeToOptimizedTerrain(CRuDataStore_Base *dataStore, BOOL incremental = FALSE);
	BOOL									CopyTo(CRuDataStore_Base *dstDataStore);

	// Entity interface
	virtual BOOL							ResetStates();
	virtual BOOL							Update(float elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// Initialization
	BOOL									Initialize();
	BOOL									AttachDataSource(IRuTerrain_DataSource *dataSource);
	IRuTerrain_DataSource*					GetDataSource() { return m_dataSource; }

	// LOD selection and frustum culling
	void									SetCamera(CRuCamera *viewCamera, CRuAABB *overrideViewVolume = NULL);

	// Highlights, toggles, etcetera
	void									SetHighlightOn(BOOL highlightOn);
	void									SetHighlightSegment(const CRuSegment3 &segment);
	void									SetHighlightFlags(UINT32 highlightFlags);
	void									SetOutlineOn(BOOL outlineOn);

	INT32									GetNumSplatsAtCursor(const CRuSegment3 &segment, char *splatIDListOut);

	void									ToggleOption_RenderTerrain();
	void									ToggleOption_RenderObjects();
	void									ToggleOption_RenderGrass();
	void									ToggleOption_RenderWater();
	void									ToggleOption_RenderTriggers(BOOL renderTriggers);
	void									ToggleOption_FilterTriggers(BOOL filterTriggers);
	void									SetOption_FilterTriggerID(INT32 id);

	// Editing-mode specific
	void									SetEditingEnvironmentColors(DWORD primaryColor, DWORD secondaryColor, DWORD specularColor, DWORD fogColor, float theta, float phi);
	void									GetEditingEnvironmentColors(DWORD &primaryColor, DWORD &secondaryColor, DWORD &specularColor, DWORD &fogColor, float &theta, float &phi);

	// Ray picking
	BOOL									FindIntersection(const CRuSegment3 &segment, CRuVector3 &intPtOut, float &intDistOut);
	CRuGUID									FindIntersectingEntityDescriptor(const CRuSegment3 &segment, CRuVector3 &intPtOut, float &intDistOut);
	CRuGUID									FindWaterBodyIntersection(const CRuSegment3 &segment, CRuVector3 &intPtOut, float &intDistOut);
	CRuTerrain_Chunk*						FindIntersectingChunk(const CRuSegment3 &segment, CRuVector3 &intPtOut, float &intDistOut);

	void									EnableSplatFilter(BOOL enableSplatFilter);

	// Undo-Redo
	BOOL									Undo_Begin();
	BOOL									Undo_End();
	BOOL									Undo();
	BOOL									Redo();

	// Chunk modification support
	void									MarkChunkAsChanged(const CRuAABB &box);

	// Flags
	void									PaintFlags(const CRuVector3 &center, float radius, UINT32 flags, UINT32 flagMask);

	// Heightfield deformation
	void									PaintHeight(const CRuVector3 &center, float radius, float delta, float refHeight, BOOL adaptivePainting, RuFalloffFunction falloffFunction);
	void									NoiseHeight(const CRuVector3 &center, float radius, float variation);
	void									SetHeight(const CRuVector3 &center, float radius, float height, float brushStrength, RuFalloffFunction falloffFunction = NULL);
	void									SmoothHeight(const CRuVector3 &center, float radius, float delta);
	void									AutoPaintHeight(const CRuVector3 &center, float radius, CRuFractal_Base *srcFractal, const CRuSplatIDMap_PaintSetting &paintSetting);

	// Masking
	void									PaintMask(const CRuVector3 &center, float radius, BOOL maskOn);

	// Offsetting
	void									PaintOffset(const CRuVector3 &center, float radius, REAL offset);

	// Texture alteration
	void									PaintSplat(const CRuVector3 &center, float radius, REAL power, RuFalloffFunction falloffFunc, INT32 iteration, UINT16 splatID);
	void									SprayPaintSplat(const CRuVector3 &center, float radius, REAL power, RuFalloffFunction falloffFunc, INT32 iteration, UINT16 splatID, float sprayProbability);
	void									BlockFillSplat(const CRuVector3 &center, float radius, UINT16 splatID);
	void									FloodFillSplat(const CRuVector3 &center, float radius, UINT16 splatID);
	CRuSplatIDMap*							FloodFill_GetSplatIDMap(INT32 chunkKey, INT32 neighborID);
	void									SmudgeSplat(const CRuVector3 &center, float radius, float smudgeProbability);
	void									ReplaceSplat(const CRuVector3 &center, float radius, UINT16 splatID);
	void									AttractSplat(const CRuVector3 &center, float radius, UINT16 attractedSplatID, float attractProbability);
	UINT16									PickSplatID(const CRuVector3 &center);

	// Auto & fractal painting
	void									MojoPaint(const CRuVector3 &center, float radius, CRuFractal_Base *srcFractal, const CRuSplatIDMap_PaintSetting &paintSetting);

	// Doodad manipulation
	void									PaintDoodad(const CRuVector3 &center, const CRuWorld_DoodadPaintSetting &paintSetting);
	void									SprayDoodad(const CRuVector3 &center, REAL radius, INT32 numDoodads, const CRuWorld_DoodadPaintSetting &paintSetting);
	void									DeleteDoodad(const CRuVector3 &center, REAL radius);
	void									RealignDoodad(const CRuVector3 &center, REAL radius);

	// Entity manipulation
	const CRuGUID&							AddEntity(const char *resName, const CRuMatrix4x4 &transform, const CRuSphere &bounds);
	BOOL									DetachEntityFromTerrainChunks(const CRuGUID &guid);
	BOOL									RemoveEntity(const CRuGUID &guid);
	BOOL									UpdateEntity(const CRuGUID &guid, const CRuMatrix4x4 &transform, const CRuSphere &bounds);
	CRuWorld_EntityDescriptor*				GetEntityDescriptor(const CRuGUID &guid);
	CRuEntity*								GetEntityInstance(const CRuGUID &guid);
	BOOL									MarkEntityDescriptorForUpdate(const CRuGUID &guid);
	BOOL									EnumerateVisibleEntityDescriptors(CRuArrayList<CRuWorld_EntityDescriptor *> &descriptorsOut);
	void									RemoveEntitiesByResourceName(const char *resourceName);
	void									ReplaceEntitiesByResourceName(const char *oldResourceName, const char *newResourceName, const CRuSphere &localBounds);

	// Water body manipulation
	BOOL									CreateWaterBody(const CRuVector3 &center, REAL radius, REAL tessellationScale);
	BOOL									ExtendWaterBody(const CRuVector3 &center, REAL radius, REAL tessellationScale);
	BOOL									ShrinkWaterBody(const CRuVector3 &center, REAL radius, REAL tessellationScale);
	BOOL									PaintWaterBodyHeight(const CRuVector3 &center, float radius, float delta, RuFalloffFunction falloffFunction);
	BOOL									SetWaterBodyHeight(const CRuVector3 &center, float radius, float height, float brushStrength, RuFalloffFunction falloffFunction = NULL);
	BOOL									SmoothWaterBodyHeight(const CRuVector3 &center, float radius, float delta);
	BOOL									PaintWaterBodyAlpha(const CRuVector3 &center, REAL radius, REAL alpha, REAL delta, RuFalloffFunction falloffFunction);
	BOOL									PaintWaterBodyColor(const CRuVector3 &center, REAL radius, RUCOLOR color, REAL delta, RuFalloffFunction falloffFunction);
	RUCOLOR									PickWaterBodyColor(const CRuVector3 &center);
	BOOL									SelectWaterBody(const CRuSegment3 &segment, CRuVector3 &intPtOut);
	BOOL									StitchWaterBodies(CRuArrayList<DWORD> &chunkKeyList);

	CRuWorld_WaterBodyDescriptor*			GetSelectedWaterBodyDescriptor();
	CRuWorld_WaterBodyDescriptor*			GetWaterBodyDescriptorAtPoint(const CRuVector3 &point);

	BOOL									IsUnderWaterSurface(const CRuVector3 &position);

	// Trigger manipulation
	CRuWorld_Trigger*						FindIntersectingTrigger(const CRuSegment3 &segment, CRuVector3 &intPtOut, float &intDistOut);
	CRuWorld_Trigger*						CreateTrigger(const CRuSegment3 &segment, BOOL snapToGrid, REAL initialSize, REAL initialHeight);
	BOOL									InsertTrigger(CRuWorld_Trigger *trigger);
	BOOL									DeleteTrigger(CRuWorld_Trigger *trigger);
	BOOL									StartTriggerUpdate(CRuWorld_Trigger *trigger);
	BOOL									UpdateTrigger(CRuWorld_Trigger *trigger);
	BOOL									EndTriggerUpdate(CRuWorld_Trigger *trigger);

	// Queries for event manager
	BOOL									EnumerateTriggers(RuWorld::TriggerType triggerType, CRuArrayList<CRuWorld_Trigger *> &triggersOut);

	CRuWorld_Trigger*						GetApplicableTrigger(RuWorld::TriggerType triggerType, const CRuVector3 &point, BOOL limitRange);

	INT32									GetZoneID(const CRuVector3 &point);
	RuWorld::ZoneType						GetZoneType(const CRuVector3 &point);
	CRuWorld_ZoneDescriptor*				GetZoneDescriptor(const CRuVector3 &point);
	CRuWorld_ZoneDescriptor*				GetZoneDescriptorByID(INT32 zoneID);
	BOOL									IsSubZoneOf(INT32 subZoneID, INT32 parentZoneID);

	// Marking
	BOOL									MarkIntersectingChunk(const CRuSegment3 &segment);

	// Sub containers and managers
	CRuWorld_ObjectContainer*				GetObjectContainer();
	CRuWorld_ObjectManager*					GetObjectManager();
	CRuWorld_DoodadContainer*				GetDoodadContainer();
	CRuWorld_DoodadManager*					GetDoodadManager();
	CRuWorld_ZoneDescriptorContainer*		GetZoneDescriptorContainer();
	IRuPathHierarchy_Base*					GetPathHierarchy();

	// Texture databases
	CRuTextureList*							GetTextureList();

	// Saving & related functions
	BOOL									Save();
	BOOL									CopyTerrainSubsetToDataStore(CRuDataStore_Base *dataStore, const CRuAABB &subsetBounds);

	void									GetDoodadEnumeration(CRuArrayList<INT32> **enumeratedKeys, CRuArrayList<CRuWorld_DoodadCluster *> **enumeratedClusters);

	BOOL									ClearEnumerationCache();

	BOOL									ClearHeightFieldQueue();
	static BOOL								ClearHeightFieldQueueCallback(const void *key, const void *data, void *userData);

	// Event handlers
	BOOL									EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting);

protected:

	BOOL									ApplyEngineSettings();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Terrain_Chunk, CRuTerrain_Chunk)

// ************************************************************************************************************************************************************

#pragma managed

#endif		// _RUENTITYTERRAIN_H_
