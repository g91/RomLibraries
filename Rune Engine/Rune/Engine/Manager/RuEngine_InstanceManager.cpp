#include "../../Engine/Manager/RuEngine_InstanceManager.h"

#include "../../Engine/Texture/RuTextureUtility.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEngine_InstanceManager, IRuEngine_InstanceManager)
ruHEAP_DEFINE_SUBCLASS(CRuEngine_InstanceManager, "CRuEngine_InstanceManager", "IRuEngine_InstanceManager")
ruCLASSGUID_DEFINE(CRuEngine_InstanceManager, 0x68FE9F08, 0x123F4777, 0x8537BF9F, 0xE4C1362C)

// ************************************************************************************************************************************************************

CRuEngine_InstanceManager::CRuEngine_InstanceManager()
:	m_vertexAssemblyPoolHash(NULL),
	m_vbPoolHash(NULL),
	m_ibHash(NULL),
	m_meshInstantiatorHash(NULL),
	m_metric_NumIBAllocations(0),
	m_metric_NumVBAllocations(0),
	m_metric_NumIBInstanceOps(0),
	m_metric_NumVBInstanceOps(0)
{
	m_currentCycle = 0;

	m_meshInstantiatorHash = ruNEW CRuHashMap<PTRVALUE, IRuMeshInstantiator *>(256);

	m_engine = NULL;

	m_instancedTextures = NULL;
}

CRuEngine_InstanceManager::~CRuEngine_InstanceManager()
{
	// Unregister event handlers
	m_engine->Event_BeginScene().UnregisterHandler(this->GetGUID());
	m_engine->Event_DeviceReset().UnregisterHandler(this->GetGUID());

	// Clear cache
	ClearCache();

	// Clear mesh instantiators
	ClearMeshInstantiators();

	// Delete vertex assembly pools
	DeleteVertexAssemblyPools();

	// Delete instanced vertex buffer pools
	DeleteInstancedVertexBufferPools();

	ruSAFE_DELETE(m_meshInstantiatorHash);

	ruSAFE_DELETE(m_vertexAssemblyPoolHash);
	ruSAFE_DELETE(m_vbPoolHash);
	ruSAFE_DELETE(m_ibHash);
	ruSAFE_DELETE(m_instancedTextures);
}

UINT32 CRuEngine_InstanceManager::GetCacheSize()
{
	return 0;
}

BOOL CRuEngine_InstanceManager::SetEngine(IRuEngine *engine)
{
	// Store pointer to engine
	m_engine = engine;

	// Setup begin scene event handler
	m_engine->Event_BeginScene().RegisterHandler(this->GetGUID(), boost::bind(&CRuEngine_InstanceManager::HandleEvent_BeginScene, this, _1));

	// Setup device reset event handler
	m_engine->Event_DeviceReset().RegisterHandler(this->GetGUID(), boost::bind(&CRuEngine_InstanceManager::HandleEvent_DeviceReset, this, _1));

	// Invalidate and release current cache
	ClearCache();

	// Setup queues according to engine parameters
	ruSAFE_DELETE(m_vertexAssemblyPoolHash);
	m_vertexAssemblyPoolHash = ruNEW CRuHashMap<PTRVALUE, VertexAssemblyPool *>(128);

	ruSAFE_DELETE(m_vbPoolHash);
	m_vbPoolHash = ruNEW CRuHashMap<PTRVALUE, VertexBufferPool *>(128);

	ruSAFE_DELETE(m_ibHash);
	m_ibHash = ruNEW CRuHashMap<CRuGUID, IRuIndexBuffer *>(2048);

	// Setup instanced textures hash
	ruSAFE_DELETE(m_instancedTextures);
	m_instancedTextures = ruNEW CRuHashMap<CRuGUID, TexDesc *>(2048);

	return TRUE;
}

IRuEngine *CRuEngine_InstanceManager::GetEngine()
{
	return m_engine;
}

BOOL CRuEngine_InstanceManager::RegisterMeshInstantiator(IRuMeshInstantiator *meshInstantiator)
{
	if(meshInstantiator)
	{
		// Increment reference on mesh instantiator
		meshInstantiator->AddRef();

		// Get key and key hash for the vertex declaration
		PTRVALUE streamLayoutKey = reinterpret_cast<PTRVALUE>(meshInstantiator->GetVertexStreamLayout());
		DWORD streamLayoutKeyHash = RuGlobalHashGen().GetHashCode(streamLayoutKey);

		// If an instantiator already exists, remove and release it
		if(m_meshInstantiatorHash->ContainsKey(streamLayoutKey, streamLayoutKeyHash))
		{
			// Get the existing instantiator and release it
			IRuMeshInstantiator *existingInstantiator = m_meshInstantiatorHash->Get(streamLayoutKey, streamLayoutKeyHash);
			ruSAFE_RELEASE(existingInstantiator);

			// Remove the existing instantiator from the hash
			m_meshInstantiatorHash->Remove(streamLayoutKey, streamLayoutKeyHash);
		}

		// Store reference to mesh instantiator
		m_meshInstantiatorHash->Add(streamLayoutKey, streamLayoutKeyHash, meshInstantiator);

		return TRUE;
	}

	return FALSE;
}

IRuMeshInstantiator *CRuEngine_InstanceManager::GetMeshInstantiator(const IRuVertexStreamLayout *vertexStreamLayout)
{
	// Get key and key hash for the vertex declaration
	PTRVALUE streamLayoutKey = reinterpret_cast<PTRVALUE>(vertexStreamLayout);
	DWORD streamLayoutKeyHash = RuGlobalHashGen().GetHashCode(streamLayoutKey);

	IRuMeshInstantiator *meshInstantiator = NULL;

	if(m_meshInstantiatorHash->Get(streamLayoutKey, streamLayoutKeyHash, meshInstantiator))
	{
		return meshInstantiator;
	}

	return NULL;
}

IRuVertexAssembly *CRuEngine_InstanceManager::GetVertexAssembly(IRuMesh *mesh, IRuVertexDeclaration *vertexDecl)
{
	// Assert valid mesh & vertex declaration
	assert(mesh && vertexDecl);

	// Hash the vertex declaration
	PTRVALUE vertexDeclKey = reinterpret_cast<PTRVALUE>(vertexDecl);
	DWORD vertexDeclKeyHash = vertexDecl->HashCode();

	// Get the vertex assembly
	VertexAssemblyPool *vaPool = GetVertexAssemblyPool(vertexDeclKey, vertexDeclKeyHash);
	if(vaPool)
	{
		IRuVertexAssembly *vertexAssembly = NULL;

		const CRuGUID &meshKey = mesh->GetGUID();
		DWORD meshKeyHash = meshKey.HashCode();

		if(vaPool->Get(meshKey, meshKeyHash, vertexAssembly) == FALSE)
		{
			// Create new vertex assembly
			vertexAssembly = ruNEW CRuVertexAssembly();

			// Insert vertex assembly into hash
			vaPool->Add(meshKey, meshKeyHash, vertexAssembly);
		}
		else
		{
			// If the GC marker is zero, the vertex assembly is fresh and does not need to be re-bound
			if(vertexAssembly->GetGCMarker() == 0 && vertexAssembly->GetIteration() == mesh->GetMeshIteration())
			{
				return vertexAssembly;
			}

			// Set GC marker
			vertexAssembly->SetGCMarker(0);

			// Set iteration marker
			vertexAssembly->SetIteration(mesh->GetMeshIteration());
		}

		// Instance all vertex streams
		for(INT32 i = 0, numStreams = vertexDecl->GetNumStreams(); i < numStreams; ++i)
		{
			IRuVertexStreamLayout *streamLayout = vertexDecl->GetVertexStreamLayout(i);

			if(streamLayout)
			{
				IRuVertexBuffer *vertexBuffer = this->GetInstancedVertexBuffer(mesh, streamLayout);

				if(vertexBuffer)
				{
					vertexAssembly->BindVertexBuffer(i, vertexBuffer, streamLayout->GetStreamStride());
				}
				else
				{
					// Forcibly return NULL assembly
					vertexAssembly = NULL;
					break;
				}
			}
			else
			{
				vertexAssembly->BindVertexBuffer(i, NULL, 0);
			}
		}

		return vertexAssembly;
	}

	return NULL;
}

IRuVertexBuffer *CRuEngine_InstanceManager::GetInstancedVertexBuffer(IRuMesh *mesh, IRuVertexStreamLayout *streamLayout)
{
	// Assert valid mesh & stream layout
	assert(mesh && streamLayout);

	// Hash the stream layout pointer
	PTRVALUE streamLayoutKey = reinterpret_cast<PTRVALUE>(streamLayout);
	DWORD streamLayoutKeyHash = streamLayout->HashCode();

	// Get the vertex buffer pool
	VertexBufferPool *vbPool = GetVertexBufferPool(streamLayoutKey, streamLayoutKeyHash);
	if(vbPool)
	{
		IRuVertexBuffer *vertexBuffer = NULL;

		BOOL reinstanceVB = FALSE;
		BOOL vbIsDynamic = mesh->IsMeshDynamic();

		const CRuGUID &meshKey = mesh->GetGUID();
		DWORD meshKeyHash = meshKey.HashCode();

		// Calculate size needed for the vertex buffer
		UINT32 requiredVBSize = streamLayout->GetStreamStride() * mesh->GetNumVertices();

		// Locate an existing vertex buffer
		if(vbPool->Get(meshKey, meshKeyHash, vertexBuffer))
		{
			// If the existing VB is too small, destroy it
			if(requiredVBSize > vertexBuffer->GetSize())
			{
				// Destroy vertex buffer
				ruSAFE_RELEASE(vertexBuffer);
			}
		}
		else
		{
			// Create a new entry for the current mesh
			vbPool->Add(meshKey, meshKeyHash, vertexBuffer);
		}

		if(vertexBuffer == NULL)
		{
			// Create vertex buffer
			vertexBuffer = vbIsDynamic ? m_engine->CreateDynamicVertexBuffer(requiredVBSize) : m_engine->CreateVertexBuffer(requiredVBSize);

			// If vertex buffer creation failed, return
			if(vertexBuffer == NULL)
			{
				vbPool->Remove(meshKey, meshKeyHash);
				return NULL;
			}

			// Update hash entry
			vbPool->Replace(meshKey, meshKeyHash, vertexBuffer);

			// Vertex buffer is new, so re-instance the vertex buffer
			reinstanceVB = TRUE;
		}
		else
		{
			// Check to make sure the iteration numbers are the same
			if(vertexBuffer->GetIteration() != mesh->GetMeshIteration())
			{
				// Iteration numbers are different, so re-instance the vertex buffer
				reinstanceVB = TRUE;
			}
		}

		// Set GC marker
		vertexBuffer->SetGCMarker(0);

		// Re-instance if necessary
		if(reinstanceVB)
		{
			void *verticesOut = vertexBuffer->Lock();

			if(verticesOut)
			{
				IRuMeshInstantiator *meshInstantiator = this->GetMeshInstantiator(streamLayout);
				if(meshInstantiator)
				{
					meshInstantiator->Instantiate(mesh, 0, verticesOut);
				}
				else
				{
					OutputDebugString("No mesh instantiator!\n");
				}

				vertexBuffer->Unlock();

				// Update iteration number
				vertexBuffer->SetIteration(mesh->GetMeshIteration());
			}
			else
			{
				// Forcibly return a NULL buffer
				vertexBuffer = NULL;
			}
		}

		return vertexBuffer;
	}

	return NULL;
}

IRuIndexBuffer *CRuEngine_InstanceManager::GetInstancedIndexBuffer(IRuMesh *mesh)
{
	// Assert valid mesh
	assert(mesh);

	// Declare empty index buffer descriptor
	IRuMeshTopology *meshTopology = mesh->GetMeshTopology();
	const CRuGUID &topologyKey = meshTopology->GetGUID();
	DWORD topologyKeyHash = topologyKey.HashCode();

	IRuIndexBuffer *indexBuffer = NULL;

	BOOL reinstanceIB = FALSE;
	BOOL ibIsDynamic = mesh->IsTopologyDynamic();

	// Calculate size needed for the index buffer
	UINT32 ibSize = sizeof(UINT16) * meshTopology->GetNumIndices();

	// Locate an existing index buffer
	if(m_ibHash->Get(topologyKey, topologyKeyHash, indexBuffer))
	{
		// If the existing IB is too small, destroy it
		if(ibSize > indexBuffer->GetSize())
		{
			// Destroy index buffer
			ruSAFE_RELEASE(indexBuffer);
		}
	}
	else
	{
		// Create a new entry for the current mesh topology
		m_ibHash->Add(topologyKey, topologyKeyHash, indexBuffer);
	}

	if(indexBuffer == NULL)
	{
		// Create index buffer
		indexBuffer = ibIsDynamic ? m_engine->CreateDynamicIndexBuffer(ibSize) : m_engine->CreateIndexBuffer(ibSize);

		// If index buffer creation failed, return
		if(indexBuffer == NULL)
		{
			m_ibHash->Remove(topologyKey, topologyKeyHash);
			return NULL;
		}

		// Update hash entry
		m_ibHash->Replace(topologyKey, topologyKeyHash, indexBuffer);

		// Index buffer is new, so re-instance the index buffer
		reinstanceIB = TRUE;
	}
	else
	{
		// Check to make sure the iteration numbers are the same
		if(indexBuffer->GetIteration() != meshTopology->GetIteration())
		{
			// Iteration numbers are different, so re-instance the index buffer
			reinstanceIB = TRUE;
		}
	}

	// Set GC marker
	indexBuffer->SetGCMarker(0);

	// Instance indices if necessary
	if(reinstanceIB && meshTopology->GetIndices() && meshTopology->GetNumIndices() > 0)
	{
		// Lock index buffer
		UINT16 *indicesOut = reinterpret_cast<UINT16 *>(indexBuffer->Lock());

		if(indicesOut)
		{
			// Copy
			RuMemcpy(indicesOut, meshTopology->GetIndices(), sizeof(UINT16) * meshTopology->GetNumIndices());

			// Unlock index buffer
			indexBuffer->Unlock();

			// Update iteration number
			indexBuffer->SetIteration(mesh->GetMeshIteration());
		}
		else
		{
			// Forcibly return a NULL index buffer
			indexBuffer = NULL;
		}
	}

	return indexBuffer;
}

IRuBaseTexture *CRuEngine_InstanceManager::GetInstance(IRuBaseTexture *texture)
{
	// If texture is NULL or in an invalidated state, it cannot be instanced
	assert(texture && !texture->IsInvalidated());

	// Check to see if the texture already has an instance attached to it
	IRuBaseTexture *instancedTexture = reinterpret_cast<IRuBaseTexture *>(texture->GetInstanceData());

	if(instancedTexture)
	{
		// Make sure the texture iteration has not changed since the last instantiation
		if(instancedTexture->GetIteration() == texture->GetIteration())
		{
			// Reset GC marker to zero
			instancedTexture->SetGCMarker(0);

			// Return instanced texture
			return instancedTexture;
		}
	}

	// No existing instance attached to texture, perform instancing
	TexDesc *texDesc = NULL;

	switch(texture->GetPool())
	{
		case ruRESPOOL_DEFAULT:
		case ruRESPOOL_MANAGED:
			{
				instancedTexture = texture;
			}

			break;

		case ruRESPOOL_SYSTEMMEMORY:
		case ruRESPOOL_SCRATCH:
			{
				const CRuGUID &texKey = texture->GetGUID();
				BOOL requiresCopy = FALSE;

				if(m_instancedTextures->Get(texKey, texKey.HashCode(), texDesc))
				{
					// If the instanced texture copy does not have the same iteration as the source, a copy is required
					if(texDesc->m_instancedTexture->GetIteration() != texDesc->m_sourceTexture->GetIteration())
						requiresCopy = TRUE;

					// Reset GC marker to zero
					texDesc->m_instancedTexture->SetGCMarker(0);

					// Update texture descriptor
					m_instancedTextures->Replace(texKey, texKey.HashCode(), texDesc);

					// Store output texture pointer
					instancedTexture = texDesc->m_instancedTexture;
				}
				else
				{
					// Copy operation is required
					requiresCopy = TRUE;

					// Create a texture based on texture type
					instancedTexture = CreatePooledTexture(texture, 0);

					if(instancedTexture)
					{
						// Create a descriptor
						texDesc = ruNEW TexDesc();
						texDesc->m_texturePool = 0;
						texDesc->m_sourceTexture = texture;
						texDesc->m_sourceTexture->AddRef();
						texDesc->m_instancedTexture = instancedTexture;
						texDesc->m_instancedTexture->SetGCMarker(0);

						// Set instanced texture descriptor into the source texture
						texDesc->m_sourceTexture->SetInstanceData(reinterpret_cast<PTRVALUE>(texDesc->m_instancedTexture));

						// Update texture descriptor
						m_instancedTextures->Add(texKey, texKey.HashCode(), texDesc);
					}
				}

				if(requiresCopy && instancedTexture)
				{
/*
					DWORD t0, t1, et;
					t0 = timeGetTime();
*/
					if(RuTextureUtility_Copy(texture, instancedTexture))
					{
						// Texture copy successful, re-purpose the source texture such that it is now a proxy to the instanced texture to save memory
						if(texture->GetType() == CRuTexture_Proxy::Type())
						{
							static_cast<CRuTexture_Proxy *>(texture)->Allocate(static_cast<IRuTexture *>(instancedTexture));
						}
						else if(texture->GetType() == CRuVolumeTexture_Proxy::Type())
						{
							static_cast<CRuVolumeTexture_Proxy *>(texture)->Allocate(static_cast<IRuVolumeTexture *>(instancedTexture));
						}

						// Equalize the iteration counter
						texDesc->m_instancedTexture->SetIteration(texDesc->m_sourceTexture->GetIteration());

						// Update texture descriptor
						m_instancedTextures->Replace(texKey, texKey.HashCode(), texDesc);
					}
					else
					{
						// Force copy on the next instantiation
						texDesc->m_instancedTexture->SetIteration(texDesc->m_instancedTexture->GetIteration() - 1);

						// Update texture descriptor
						m_instancedTextures->Replace(texKey, texKey.HashCode(), texDesc);
					}
/*
					t1 = timeGetTime();
					et = t1 - t0;
					char outputStr[256];
					sprintf(outputStr, "Texture instantiation done in %u ms.\n", et);
					OutputDebugString(outputStr);
*/
				}
			}

			break;
	}

	return instancedTexture;
}

BOOL CRuEngine_InstanceManager::DeInstance(IRuBaseTexture *texture)
{
	const CRuGUID &texKey = texture->GetGUID();
	TexDesc *texDesc = NULL;

	if(m_instancedTextures->Get(texKey, texKey.HashCode(), texDesc))
	{
		// Release texture instance
		ReleaseTextureInstance(texDesc);

		// Delete the texture instance descriptor
		ruSAFE_DELETE(texDesc);

		// Remove from hash
		m_instancedTextures->Remove(texKey, texKey.HashCode());
	}

	return FALSE;
}

BOOL CRuEngine_InstanceManager::ClearCache()
{
	// Clear vertex assembly pools
	ClearVertexAssemblyPools();

	// Clear instanced vertex buffers
	ClearInstancedVertexBufferPools();

	// Clear instanced index buffers
	ClearInstancedIndexBuffers();

	// Clear instanced textures
	ClearInstancedTextures();

	return TRUE;
}

INT32 CRuEngine_InstanceManager::GetNumMetricStrings()
{
	return 2;
}

const char *CRuEngine_InstanceManager::GetMetricString(INT32 stringIndex)
{
	switch(stringIndex)
	{
		case 0:
			sprintf(m_metric_String, "[Instance Manager] [VB Size : %d] [IB Size : %d]\n", m_metric_TotalVBSize, m_metric_TotalIBSize);
			return m_metric_String;
		case 1:
			sprintf(m_metric_String, "[Instance Manager] [VB Alloc : %d] [IB Alloc : %d] [VB Instance Op : %d] [IB Instance Op : %d]", m_metric_NumVBAllocations, m_metric_NumIBAllocations, m_metric_NumVBInstanceOps, m_metric_NumIBInstanceOps);
			return m_metric_String;
	}

	sprintf(m_metric_String, "[Instance Manager] [VB Alloc : %d] [IB Alloc : %d] [VB Instance Op : %d] [IB Instance Op : %d]", m_metric_NumVBAllocations, m_metric_NumIBAllocations, m_metric_NumVBInstanceOps, m_metric_NumIBInstanceOps);
	return m_metric_String;
}

BOOL CRuEngine_InstanceManager::OutputInstantiationManifest()
{
	return TRUE;
}

BOOL CRuEngine_InstanceManager::HandleEvent_BeginScene(RuEventArgs *eventArgs)
{
	// Increment cycle count
	++m_currentCycle;

	INT32 gcPeriod = 10;
	if((m_currentCycle % gcPeriod) == 0)
	{
		m_metric_TotalVBSize = 0;
		m_metric_TotalIBSize = 0;

		// Run a garbage collection on vertex assemblies
		GCVertexAssemblyPools();

		// Run a garbage collection on instanced vertex buffers
		GCInstancedVertexBufferPools();

		// Run a garbage collection on instanced index buffers
		GCInstancedIndexBuffers();

		// Run a clean up on instanced textures
		m_instancedTextures->ForAllElements(boost::bind(&CRuEngine_InstanceManager::InstancedTextureGCCallback, this, _1, _2));

		for(INT32 i = 0; i < m_gcInstancedTextures.Count(); ++i)
		{
			m_instancedTextures->Remove(m_gcInstancedTextures[i], m_gcInstancedTextures[i].HashCode());
		}

		m_gcInstancedTextures.Clear();
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::HandleEvent_DeviceReset(RuEventArgs *eventArgs)
{
	return ClearCache();
}

void CRuEngine_InstanceManager::ClearMeshInstantiators()
{
	m_meshInstantiatorHash->ForAllElements(boost::bind(&CRuEngine_InstanceManager::ClearMeshInstantiatorsCallback, this, _1, _2));
}

BOOL CRuEngine_InstanceManager::ClearMeshInstantiatorsCallback(const void *key, void *data)
{
	IRuMeshInstantiator *instantiator = *reinterpret_cast<IRuMeshInstantiator **>(data);
	ruSAFE_RELEASE(instantiator);
	return TRUE;
}

CRuEngine_InstanceManager::VertexAssemblyPool *CRuEngine_InstanceManager::GetVertexAssemblyPool(PTRVALUE poolKey, DWORD poolKeyHash)
{
	VertexAssemblyPool *vaPool = NULL;

	// Find an existing vertex buffer pool
	if(m_vertexAssemblyPoolHash->Get(poolKey, poolKeyHash, vaPool) == NULL)
	{
		// Create new vertex buffer pool
		vaPool = ruNEW VertexAssemblyPool(1024);

		// Add vertex buffer pool array into hash
		m_vertexAssemblyPoolHash->Add(poolKey, poolKeyHash, vaPool);
	}

	return vaPool;
}

BOOL CRuEngine_InstanceManager::DeleteVertexAssemblyPools()
{
	if(m_vertexAssemblyPoolHash)
	{
		m_vertexAssemblyPoolHash->ForAllElements(boost::bind(&CRuEngine_InstanceManager::DeleteVertexAssemblyPoolsCallback, this, _1, _2));
		m_vertexAssemblyPoolHash->Clear();
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::DeleteVertexAssemblyPoolsCallback(const void *key, void *data)
{
	// Cast parameters
	PTRVALUE poolKey = *reinterpret_cast<const PTRVALUE *>(key);
	VertexAssemblyPool *vaPool = *reinterpret_cast<VertexAssemblyPool **>(data);

	if(vaPool)
	{
		// Clear vertex assembly pool
		ClearVertexAssemblies(vaPool);

		// Delete vertex assembly pool
		ruSAFE_DELETE(vaPool);

		return TRUE;
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::ClearVertexAssemblyPools()
{
	if(m_vertexAssemblyPoolHash)
	{
		m_vertexAssemblyPoolHash->ForAllElements(boost::bind(&CRuEngine_InstanceManager::ClearVertexAssemblyPoolsCallback, this, _1, _2));
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::ClearVertexAssemblyPoolsCallback(const void *key, void *data)
{
	// Cast parameters
	PTRVALUE poolKey = *reinterpret_cast<const PTRVALUE *>(key);
	VertexAssemblyPool *vaPool = *reinterpret_cast<VertexAssemblyPool **>(data);

	if(vaPool)
	{
		ClearVertexAssemblies(vaPool);
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::ClearVertexAssemblies(VertexAssemblyPool *vaPool)
{
	if(vaPool)
	{
		vaPool->ForAllElements(boost::bind(&CRuEngine_InstanceManager::ClearVertexAssembliesCallback, this, _1, _2));
		vaPool->Clear();
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::ClearVertexAssembliesCallback(const void *key, void *data)
{
	// Cast parameters
	const CRuGUID &vaKey = *reinterpret_cast<const CRuGUID *>(key);
	IRuVertexAssembly *vertexAssembly = *reinterpret_cast<IRuVertexAssembly **>(data);

	// Release vertex assembly
	ruSAFE_RELEASE(vertexAssembly);

	return TRUE;
}

BOOL CRuEngine_InstanceManager::GCVertexAssemblyPools()
{
	if(m_vertexAssemblyPoolHash)
	{
		// Clear the GC queue
		m_gc_Buffers.Clear();

		// Run GC callback on all vertex assembly pools
		m_vertexAssemblyPoolHash->ForAllElements(boost::bind(&CRuEngine_InstanceManager::GCVertexAssemblyPoolsCallback, this, _1, _2));

		// Post-process the GC queue
		while(m_gc_Buffers.IsEmpty() == FALSE)
		{
			BufferGCTuple tuple = m_gc_Buffers.Dequeue();

			VertexAssemblyPool *vaPool = GetVertexAssemblyPool(tuple.m_poolKey, RuGlobalHashGen().GetHashCode(tuple.m_poolKey));
			if(vaPool == NULL)
			{
				// Critical error
				continue;
			}

			IRuVertexAssembly *vertexAssembly = NULL;
			if(vaPool->Get(tuple.m_bufferKey, tuple.m_bufferKey.HashCode(), vertexAssembly) == FALSE)
			{
				// Critical error
				continue;
			}

			// Release vertex assembly
			ruSAFE_RELEASE(vertexAssembly);

			// Remove from hash
			vaPool->Remove(tuple.m_bufferKey, tuple.m_bufferKey.HashCode());
		}
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::GCVertexAssemblyPoolsCallback(const void *key, void *data)
{
	// Cast parameters
	PTRVALUE poolKey = *reinterpret_cast<const PTRVALUE *>(key);
	VertexAssemblyPool *vaPool = *reinterpret_cast<VertexAssemblyPool **>(data);

	if(vaPool)
	{
		// Set current pool key
		m_gc_CurPoolKey = poolKey;

		// Run GC on all vertex assemblies
		GCVertexAssemblies(vaPool);
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::GCVertexAssemblies(VertexAssemblyPool *vaPool)
{
	if(vaPool)
	{
		// Iterate through all vertex assemblies in the pool
		vaPool->ForAllElements(boost::bind(&CRuEngine_InstanceManager::GCVertexAssembliesCallback, this, _1, _2));
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::GCVertexAssembliesCallback(const void *key, void *data)
{
	// Cast parameters
	const CRuGUID &vaKey = *reinterpret_cast<const CRuGUID *>(key);
	IRuVertexAssembly *vertexAssembly = *reinterpret_cast<IRuVertexAssembly **>(data);

	// Update GC marker
	INT32 gcMarker = vertexAssembly->GetGCMarker() + 1;
	vertexAssembly->SetGCMarker(gcMarker);

	// Collect the vertex assembly if it has not been used in the given number of cycles
	if(gcMarker > 120)
	{
		// Queue the pool key/buffer key tuple in the GC queue
		BufferGCTuple tuple = { m_gc_CurPoolKey, vaKey };
		m_gc_Buffers.Queue(tuple);
	}

	return TRUE;
}

CRuEngine_InstanceManager::VertexBufferPool *CRuEngine_InstanceManager::GetVertexBufferPool(PTRVALUE poolKey, DWORD poolKeyHash)
{
	VertexBufferPool *vbPool = NULL;

	// Find an existing vertex buffer pool
	if(m_vbPoolHash->Get(poolKey, poolKeyHash, vbPool) == NULL)
	{
		// Create new vertex buffer pool
		vbPool = ruNEW VertexBufferPool(1024);

		// Add vertex buffer pool array into hash
		m_vbPoolHash->Add(poolKey, poolKeyHash, vbPool);
	}

	return vbPool;
}

BOOL CRuEngine_InstanceManager::DeleteInstancedVertexBufferPools()
{
	if(m_vbPoolHash)
	{
		m_vbPoolHash->ForAllElements(boost::bind(&CRuEngine_InstanceManager::DeleteInstancedVertexBufferPoolsCallback, this, _1, _2));
		m_vbPoolHash->Clear();
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::DeleteInstancedVertexBufferPoolsCallback(const void *key, void *data)
{
	// Cast parameters
	PTRVALUE poolKey = *reinterpret_cast<const PTRVALUE *>(key);
	VertexBufferPool *vbPool = *reinterpret_cast<VertexBufferPool **>(data);

	if(vbPool)
	{
		// Clear vertex buffer pool
		ClearInstancedVertexBuffers(vbPool);

		// Delete vertex buffer pool
		ruSAFE_DELETE(vbPool);

		return TRUE;
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::ClearInstancedVertexBufferPools()
{
	if(m_vbPoolHash)
	{
		m_vbPoolHash->ForAllElements(boost::bind(&CRuEngine_InstanceManager::ClearInstancedVertexBufferPoolsCallback, this, _1, _2));
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::ClearInstancedVertexBufferPoolsCallback(const void *key, void *data)
{
	// Cast parameters
	PTRVALUE poolKey = *reinterpret_cast<const PTRVALUE *>(key);
	VertexBufferPool *vbPool = *reinterpret_cast<VertexBufferPool **>(data);

	if(vbPool)
	{
		ClearInstancedVertexBuffers(vbPool);
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::ClearInstancedVertexBuffers(VertexBufferPool *vbPool)
{
	if(vbPool)
	{
		vbPool->ForAllElements(boost::bind(&CRuEngine_InstanceManager::ClearInstancedVertexBuffersCallback, this, _1, _2));
		vbPool->Clear();
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::ClearInstancedVertexBuffersCallback(const void *key, void *data)
{
	// Cast parameters
	const CRuGUID &vbKey = *reinterpret_cast<const CRuGUID *>(key);
	IRuVertexBuffer *vertexBuffer = *reinterpret_cast<IRuVertexBuffer **>(data);

	// Release vertex buffer
	ruSAFE_RELEASE(vertexBuffer);

	return TRUE;
}

BOOL CRuEngine_InstanceManager::GCInstancedVertexBufferPools()
{
	if(m_vbPoolHash)
	{
		// Clear the GC queue
		m_gc_Buffers.Clear();

		// Run GC callback on all vertex buffer pools
		m_vbPoolHash->ForAllElements(boost::bind(&CRuEngine_InstanceManager::GCInstancedVertexBufferPoolsCallback, this, _1, _2));

		// Post-process the GC queue
		while(m_gc_Buffers.IsEmpty() == FALSE)
		{
			BufferGCTuple tuple = m_gc_Buffers.Dequeue();

			VertexBufferPool *vbPool = GetVertexBufferPool(tuple.m_poolKey, RuGlobalHashGen().GetHashCode(tuple.m_poolKey));
			if(vbPool == NULL)
			{
				// Critical error
				continue;
			}

			IRuVertexBuffer *vertexBuffer = NULL;
			if(vbPool->Get(tuple.m_bufferKey, tuple.m_bufferKey.HashCode(), vertexBuffer) == FALSE)
			{
				// Critical error
				continue;
			}

			// Release vertex buffer
			ruSAFE_RELEASE(vertexBuffer);

			// Remove from hash
			vbPool->Remove(tuple.m_bufferKey, tuple.m_bufferKey.HashCode());
		}
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::GCInstancedVertexBufferPoolsCallback(const void *key, void *data)
{
	// Cast parameters
	PTRVALUE poolKey = *reinterpret_cast<const PTRVALUE *>(key);
	VertexBufferPool *vbPool = *reinterpret_cast<VertexBufferPool **>(data);

	if(vbPool)
	{
		// Set current pool key
		m_gc_CurPoolKey = poolKey;

		// Run GC on all vertex buffers
		GCInstancedVertexBuffers(vbPool);
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::GCInstancedVertexBuffers(VertexBufferPool *vbPool)
{
	if(vbPool)
	{
		// Iterate through all vertex buffers in the pool
		vbPool->ForAllElements(boost::bind(&CRuEngine_InstanceManager::GCInstancedVertexBuffersCallback, this, _1, _2));
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::GCInstancedVertexBuffersCallback(const void *key, void *data)
{
	// Cast parameters
	const CRuGUID &vbKey = *reinterpret_cast<const CRuGUID *>(key);
	IRuVertexBuffer *vertexBuffer = *reinterpret_cast<IRuVertexBuffer **>(data);

	// Update GC marker
	INT32 gcMarker = vertexBuffer->GetGCMarker() + 1;
	vertexBuffer->SetGCMarker(gcMarker);

	// Collect the buffer if it has not been used in the given number of cycles
	if(gcMarker > 120)
	{
		// Queue the pool key/buffer key tuple in the GC queue
		BufferGCTuple tuple = { m_gc_CurPoolKey, vbKey };
		m_gc_Buffers.Queue(tuple);
	}

	// Accumulate to total VB size
	m_metric_TotalVBSize += vertexBuffer->GetSize();

	return TRUE;
}

BOOL CRuEngine_InstanceManager::ClearInstancedIndexBuffers()
{
	if(m_ibHash)
	{
		m_ibHash->ForAllElements(boost::bind(&CRuEngine_InstanceManager::ClearInstancedIndexBuffersCallback, this, _1, _2));
		m_ibHash->Clear();
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::ClearInstancedIndexBuffersCallback(const void *key, void *data)
{
	// Cast parameters
	const CRuGUID &ibKey = *reinterpret_cast<const CRuGUID *>(key);
	IRuIndexBuffer *indexBuffer = *reinterpret_cast<IRuIndexBuffer **>(data);

	// Release index buffer
	ruSAFE_RELEASE(indexBuffer);

	return TRUE;
}

BOOL CRuEngine_InstanceManager::GCInstancedIndexBuffers()
{
	if(m_ibHash)
	{
		// Clear garbage collection queue
		m_gc_Buffers.Clear();

		// Run garbage collection callback on all index buffers
		m_ibHash->ForAllElements(boost::bind(&CRuEngine_InstanceManager::GCInstancedIndexBuffersCallback, this, _1, _2));

		// Post-process garbage collection queue
		while(m_gc_Buffers.IsEmpty() == FALSE)
		{
			BufferGCTuple tuple = m_gc_Buffers.Dequeue();

			IRuIndexBuffer *indexBuffer = NULL;
			if(m_ibHash->Get(tuple.m_bufferKey, tuple.m_bufferKey.HashCode(), indexBuffer) == FALSE)
			{
				// Critical error
				continue;
			}

			// Release index buffer
			ruSAFE_RELEASE(indexBuffer);

			// Remove from hash
			m_ibHash->Remove(tuple.m_bufferKey, tuple.m_bufferKey.HashCode());
		}
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::GCInstancedIndexBuffersCallback(const void *key, void *data)
{
	// Cast parameters
	const CRuGUID &ibKey = *reinterpret_cast<const CRuGUID *>(key);
	IRuIndexBuffer *indexBuffer = *reinterpret_cast<IRuIndexBuffer **>(data);

	// Update GC marker
	INT32 gcMarker = indexBuffer->GetGCMarker() + 1;
	indexBuffer->SetGCMarker(gcMarker);

	// Collect the buffer if it has not been used in the given number of cycles
	if(gcMarker > 120)
	{
		// Queue the pool key/buffer key tuple in the GC queue
		BufferGCTuple tuple = { 0, ibKey };
		m_gc_Buffers.Queue(tuple);
	}
	// Accumulate to total IB size
	m_metric_TotalIBSize += indexBuffer->GetSize();

	return TRUE;
}

CRuGUID CRuEngine_InstanceManager::BuildTexturePoolKey(IRuBaseTexture *texture)
{
	if(texture->GetType().IsSubClassOf(IRuTexture::Type()))
	{
		// Build a 128-bit key based on texture dimensions, level count, and format
		IRuTexture *srcTexture = static_cast<IRuTexture *>(texture);
		INT32 key0 = ((srcTexture->GetLevelWidth(0) & 0x0000FFFF) << 16) | (srcTexture->GetLevelHeight(0) & 0x0000FFFF);
		INT32 key1 = srcTexture->GetLevelCount() & 0x0000FFFF;
		INT32 key2 = srcTexture->GetSurfaceFormat();

		return CRuGUID(key0, key1, key2, 0);
	}
	else if(texture->GetType().IsSubClassOf(IRuVolumeTexture::Type()))
	{
		// Build a 128-bit key based on texture dimensions, level count, and format
		IRuVolumeTexture *srcTexture = static_cast<IRuVolumeTexture *>(texture);
		INT32 key0 = ((srcTexture->GetLevelWidth(0) & 0x0000FFFF) << 16) | (srcTexture->GetLevelHeight(0) & 0x0000FFFF);
		INT32 key1 = ((srcTexture->GetLevelDepth(0) & 0x0000FFFF) << 16) | (srcTexture->GetLevelCount() & 0x0000FFFF);
		INT32 key2 = srcTexture->GetSurfaceFormat();

		return CRuGUID(key0, key1, key2, 0);
	}

	return CRuGUID::Invalid;
}

IRuBaseTexture *CRuEngine_InstanceManager::CreatePooledTexture(IRuBaseTexture *texture, PTRVALUE texturePool)
{
	IRuBaseTexture *pooledTexture = NULL;

	if(texture->GetType().IsSubClassOf(IRuTexture::Type()))
	{
		IRuTexture *srcTexture = static_cast<IRuTexture *>(texture);
		pooledTexture = m_engine->CreateTexture(srcTexture->GetLevelWidth(0), srcTexture->GetLevelHeight(0), srcTexture->GetLevelCount(), srcTexture->GetSurfaceFormat(), ruRESPOOL_MANAGED);
	}
	else if(texture->GetType().IsSubClassOf(IRuVolumeTexture::Type()))
	{
		IRuVolumeTexture *srcTexture = static_cast<IRuVolumeTexture *>(texture);
		pooledTexture = m_engine->CreateVolumeTexture(srcTexture->GetLevelWidth(0), srcTexture->GetLevelHeight(0), srcTexture->GetLevelDepth(0), srcTexture->GetLevelCount(), srcTexture->GetSurfaceFormat(), ruRESPOOL_MANAGED);
	}

	return pooledTexture;
}

IRuBaseTexture *CRuEngine_InstanceManager::CreateScratchTexture(IRuBaseTexture *texture)
{
	IRuBaseTexture *scratchTexture = NULL;

	if(texture->GetType().IsSubClassOf(IRuTexture::Type()))
	{
		IRuTexture *srcTexture = static_cast<IRuTexture *>(texture);
		scratchTexture = m_engine->CreateTexture(srcTexture->GetLevelWidth(0), srcTexture->GetLevelHeight(0), srcTexture->GetLevelCount(), srcTexture->GetSurfaceFormat(), ruRESPOOL_SYSTEMMEMORY);
	}
	else if(texture->GetType().IsSubClassOf(IRuVolumeTexture::Type()))
	{
		IRuVolumeTexture *srcTexture = static_cast<IRuVolumeTexture *>(texture);
		scratchTexture = m_engine->CreateVolumeTexture(srcTexture->GetLevelWidth(0), srcTexture->GetLevelHeight(0), srcTexture->GetLevelDepth(0), srcTexture->GetLevelCount(), srcTexture->GetSurfaceFormat(), ruRESPOOL_SYSTEMMEMORY);
	}

	return scratchTexture;
}

BOOL CRuEngine_InstanceManager::ReleaseTextureInstance(CRuEngine_InstanceManager::TexDesc *texDesc)
{
	BOOL retVal = TRUE;

	// Copy-back data to source texture
	if(texDesc->m_sourceTexture->GetType() == CRuTexture_Proxy::Type())
	{
		// Re-allocate the source texture
		CRuTexture_Proxy *texture = static_cast<CRuTexture_Proxy *>(texDesc->m_sourceTexture);
		texture->Allocate(texture->GetLevelWidth(0), texture->GetLevelHeight(0), texture->GetLevelCount(), texture->GetSurfaceFormat());

		// Copy-back to source texture
		if(RuTextureUtility_Copy(texDesc->m_instancedTexture, texture) == FALSE)
		{
			// Copy-back to source texture failed, discard the texture
			texture->Discard();

			retVal = FALSE;
		}
	}
	else if(texDesc->m_sourceTexture->GetType() == CRuVolumeTexture_Proxy::Type())
	{
		// Re-allocate the source texture
		CRuVolumeTexture_Proxy *texture = static_cast<CRuVolumeTexture_Proxy *>(texDesc->m_sourceTexture);
		texture->Allocate(texture->GetLevelWidth(0), texture->GetLevelHeight(0), texture->GetLevelDepth(0), texture->GetLevelCount(), texture->GetSurfaceFormat());

		// Copy-back to source texture
		if(RuTextureUtility_Copy(texDesc->m_instancedTexture, texture) == FALSE)
		{
			// Copy-back to source texture failed, discard the texture
			texture->Discard();

			retVal = FALSE;
		}
	}

	// Reset instanced texture pointer in source texture
	texDesc->m_sourceTexture->SetInstanceData(NULL);

	// Release reference to source texture
	ruSAFE_RELEASE(texDesc->m_sourceTexture);

	// Release reference to instanced texture
	ruSAFE_RELEASE(texDesc->m_instancedTexture);

	return retVal;
}

BOOL CRuEngine_InstanceManager::ClearInstancedTextures()
{
	if(m_instancedTextures)
	{
		// Release the entire instanced mesh list
		m_instancedTextures->ForAllElements(boost::bind(&CRuEngine_InstanceManager::ClearInstancedTexturesCallback, this, _1, _2));
		m_instancedTextures->Clear();
	}

	return TRUE;
}

BOOL CRuEngine_InstanceManager::ClearInstancedTexturesCallback(const void *key, void *data)
{
	// Cast parameters
	const CRuGUID &texKey = *reinterpret_cast<const CRuGUID *>(key);
	TexDesc *texDesc= *reinterpret_cast<TexDesc **>(data);

	// Release the texture instance
	ReleaseTextureInstance(texDesc);

	// Delete the texture instance descriptor
	ruSAFE_DELETE(texDesc);

	return TRUE;
}

BOOL CRuEngine_InstanceManager::InstancedTextureGCCallback(const void *key, void *data)
{
	// Cast parameters
	const CRuGUID &texKey = *reinterpret_cast<const CRuGUID *>(key);
	TexDesc *texDesc= *reinterpret_cast<TexDesc **>(data);

	// Increment GC marker
	INT32 gcMarker = texDesc->m_instancedTexture->GetGCMarker() + 1;
	texDesc->m_instancedTexture->SetGCMarker(gcMarker);

	// Collect the texture if it has not been used in the given number of cycles
	if(gcMarker > 30)
	{
		// Add texture key to the list of textures to be removed
		m_gcInstancedTextures.Add(texKey);

		// Release the texture instance
		ReleaseTextureInstance(texDesc);

		// Delete the texture instance descriptor
		ruSAFE_DELETE(texDesc);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
