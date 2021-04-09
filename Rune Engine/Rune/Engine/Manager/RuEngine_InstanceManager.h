#pragma once

#include "../../Engine/Manager/RuEngine_InstanceManager_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuEngine_InstanceManager : public IRuEngine_InstanceManager
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

private:
	struct BufferGCTuple
	{
		PTRVALUE										m_poolKey;
		CRuGUID											m_bufferKey;
	};

	struct TexDesc
	{
		PTRVALUE										m_texturePool;
		IRuBaseTexture*									m_sourceTexture;
		IRuBaseTexture*									m_instancedTexture;
	};

	// Convenience typedefs
	typedef CRuHashMap<CRuGUID, IRuVertexAssembly *>	VertexAssemblyPool;
	typedef CRuHashMap<CRuGUID, IRuVertexBuffer *>		VertexBufferPool;

	// Cycle counter
	INT32												m_currentCycle;

	// Mesh instantiators
	CRuHashMap<PTRVALUE, IRuMeshInstantiator *>*		m_meshInstantiatorHash;

	IRuEngine*											m_engine;							//!< Engine core

	// Device buffer manager states
	CRuHashMap<PTRVALUE, VertexAssemblyPool *>*			m_vertexAssemblyPoolHash;
	CRuHashMap<PTRVALUE, VertexBufferPool *>*			m_vbPoolHash;
	CRuHashMap<CRuGUID, IRuIndexBuffer *>*				m_ibHash;

	PTRVALUE											m_gc_CurPoolKey;
	CRuQueue<BufferGCTuple>								m_gc_Buffers;

	// Texture instantiation pools
	CRuHashMap<CRuGUID, TexDesc *>*						m_instancedTextures;				//!< Hash of instanced textures

	CRuArrayList<CRuGUID>								m_gcInstancedTextures;

	// Metrics
	char												m_metric_String[1024];
	INT32												m_metric_NumIBAllocations;
	INT32												m_metric_NumVBAllocations;
	INT32												m_metric_NumIBInstanceOps;
	INT32												m_metric_NumVBInstanceOps;

	INT32												m_metric_TotalVBSize;
	INT32												m_metric_TotalIBSize;

public:
														CRuEngine_InstanceManager();
	virtual												~CRuEngine_InstanceManager();

	// Settings
	virtual UINT32										GetCacheSize();

	BOOL												SetEngine(IRuEngine *engine);
	virtual IRuEngine*									GetEngine();

	// Mesh instantiator registration
	virtual BOOL										RegisterMeshInstantiator(IRuMeshInstantiator *meshInstantiator);
	IRuMeshInstantiator*								GetMeshInstantiator(const IRuVertexStreamLayout *vertexStreamLayout);

	// Instance retrieval
	virtual IRuVertexAssembly*							GetVertexAssembly(IRuMesh *mesh, IRuVertexDeclaration *vertexDecl);

	virtual IRuVertexBuffer*							GetInstancedVertexBuffer(IRuMesh *mesh, IRuVertexStreamLayout *streamLayout);
	virtual IRuIndexBuffer*								GetInstancedIndexBuffer(IRuMesh *mesh);

	// Instance retrieval for textures
	virtual IRuBaseTexture*								GetInstance(IRuBaseTexture *texture);
	virtual BOOL										DeInstance(IRuBaseTexture *texture);

	// Data invalidation
	virtual BOOL										ClearCache();

	// Metrics
	virtual INT32										GetNumMetricStrings();
	virtual const char*									GetMetricString(INT32 stringIndex);

	// Debugging
	virtual BOOL										OutputInstantiationManifest();

private:
	// Event handlers
	BOOL												HandleEvent_BeginScene(RuEventArgs *eventArgs);
	BOOL												HandleEvent_DeviceReset(RuEventArgs *eventArgs);

	// Mesh instantiator management
	void												ClearMeshInstantiators();
	BOOL												ClearMeshInstantiatorsCallback(const void *key, void *data);

	// Vertex assembly management
	VertexAssemblyPool*									GetVertexAssemblyPool(PTRVALUE poolKey, DWORD poolKeyHash);

	BOOL												DeleteVertexAssemblyPools();
	BOOL												DeleteVertexAssemblyPoolsCallback(const void *key, void *data);

	BOOL												ClearVertexAssemblyPools();
	BOOL												ClearVertexAssemblyPoolsCallback(const void *key, void *data);

	BOOL												ClearVertexAssemblies(VertexAssemblyPool *vaPool);
	BOOL												ClearVertexAssembliesCallback(const void *key, void *data);

	BOOL												GCVertexAssemblyPools();
	BOOL												GCVertexAssemblyPoolsCallback(const void *key, void *data);

	BOOL												GCVertexAssemblies(VertexAssemblyPool *vaPool);
	BOOL												GCVertexAssembliesCallback(const void *key, void *data);

	// Vertex buffer management
	VertexBufferPool*									GetVertexBufferPool(PTRVALUE poolKey, DWORD poolKeyHash);

	BOOL												DeleteInstancedVertexBufferPools();
	BOOL												DeleteInstancedVertexBufferPoolsCallback(const void *key, void *data);

	BOOL												ClearInstancedVertexBufferPools();
	BOOL												ClearInstancedVertexBufferPoolsCallback(const void *key, void *data);

	BOOL												ClearInstancedVertexBuffers(VertexBufferPool *vbPool);
	BOOL												ClearInstancedVertexBuffersCallback(const void *key, void *data);

	BOOL												GCInstancedVertexBufferPools();
	BOOL												GCInstancedVertexBufferPoolsCallback(const void *key, void *data);

	BOOL												GCInstancedVertexBuffers(VertexBufferPool *vbPool);
	BOOL												GCInstancedVertexBuffersCallback(const void *key, void *data);

	// Index buffer management
	BOOL												ClearInstancedIndexBuffers();
	BOOL												ClearInstancedIndexBuffersCallback(const void *key, void *data);

	BOOL												GCInstancedIndexBuffers();
	BOOL												GCInstancedIndexBuffersCallback(const void *key, void *data);

	// Texture management utilities
	CRuGUID												BuildTexturePoolKey(IRuBaseTexture *texture);
	IRuBaseTexture*										CreatePooledTexture(IRuBaseTexture *texture, PTRVALUE texturePool);
	IRuBaseTexture*										CreateScratchTexture(IRuBaseTexture *texture);

	// Instanced texture
	BOOL												ReleaseTextureInstance(CRuEngine_InstanceManager::TexDesc *texDesc);
	BOOL												ClearInstancedTextures();
	BOOL												ClearInstancedTexturesCallback(const void *key, void *data);
	BOOL												InstancedTextureGCCallback(const void *key, void *data);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
