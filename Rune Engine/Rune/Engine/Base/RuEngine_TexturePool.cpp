/*!
	@file RuEngine_TexturePool.cpp

	Copyright (c) 2004-2009 All rights reserved
*/

#include "../../Engine/Base/RuEngine_TexturePool.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuEngine_TexturePool::CRuEngine_TexturePool()
:	m_subPools(NULL),
	m_engine(NULL),
	m_enableAutoRestore(FALSE)
{
}

CRuEngine_TexturePool::~CRuEngine_TexturePool()
{
	// Manually handle shutdown
	HandleEvent_Shutdown(NULL);
}

BOOL CRuEngine_TexturePool::Initialize(IRuEngine *engine, CreateTextureCallback createTextureFn, BOOL enableAutoRestore)
{
	// Create sub pool hash
	m_subPools = ruNEW CRuHashMap<CRuGUID, SubPool *>(256);

	// Store pointer to engine
	m_engine = engine;

	// Store pointer to create buffer callback
	m_createTextureFn = createTextureFn;

	// Enable auto-restore?
	m_enableAutoRestore = enableAutoRestore;

	// Install event handlers
	m_engine->Event_BeginScene().RegisterHandler(m_guid, boost::bind(&CRuEngine_TexturePool::HandleEvent_BeginScene, this, _1));
	m_engine->Event_DeviceReset().RegisterHandler(m_guid, boost::bind(&CRuEngine_TexturePool::HandleEvent_DeviceReset, this, _1));
	m_engine->Event_DeviceRestore().RegisterHandler(m_guid, boost::bind(&CRuEngine_TexturePool::HandleEvent_DeviceRestore, this, _1));
	m_engine->Event_Shutdown().RegisterHandler(m_guid, boost::bind(&CRuEngine_TexturePool::HandleEvent_Shutdown, this, _1));

	return TRUE;
}

IRuBaseTexture *CRuEngine_TexturePool::CreateTexture(INT32 dim0, INT32 dim1, INT32 dim2, INT32 levelCount, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool)
{
	IRuBaseTexture *textureOut = NULL;

	// Build a 128-bit key based on texture parameters
	CRuGUID guid(BuildSubPoolKey(dim0, dim1, dim2, levelCount, surfaceFormat, resourcePool));

	// Locate the sub pool
	SubPool *subPool = NULL;
	m_subPools->Get(guid, guid.HashCode(), subPool);

	// If the sub pool does not exist, create it now
	if(subPool == NULL)
	{
		subPool = ruNEW SubPool();
		m_subPools->Add(guid, guid.HashCode(), subPool);
	}

	if(subPool->m_unusedTextures.Count() > 0)
	{
		// Dequeue unused textures off queue
		textureOut = subPool->m_unusedTextures.Dequeue();
	}
	else
	{
		// Create texture using create texture callback
		textureOut = m_createTextureFn(dim0, dim1, dim2, levelCount, surfaceFormat, resourcePool);
	}

	// Insert into list of used textures
	if(textureOut)
	{
		textureOut->AddRef();
		subPool->m_usedTextures.Add(textureOut);
	}

	return textureOut;
}

BOOL CRuEngine_TexturePool::Invalidate()
{
	if(m_subPools)
	{
		if(m_enableAutoRestore)
		{
			// Invoke invalidation on all sub pools
			m_subPools->ForAllElements(boost::bind(&CRuEngine_TexturePool::InvalidateCallback, this, _1, _2));
		}
		else
		{
			// Invoke invalidation on all sub pools
			m_subPools->ForAllElements(boost::bind(&CRuEngine_TexturePool::InvalidateCallback, this, _1, _2));

			// Clear subpool
			m_subPools->Clear();
		}
	}

	return TRUE;
}

BOOL CRuEngine_TexturePool::Restore()
{
	if(m_subPools)
	{
		if(m_enableAutoRestore)
		{
			// Invoke restore on all sub pools
			m_subPools->ForAllElements(boost::bind(&CRuEngine_TexturePool::RestoreCallback, this, _1, _2));
		}
	}

	return TRUE;
}

BOOL CRuEngine_TexturePool::GarbageCollect()
{
	if(m_subPools)
	{
		// Invoke garbage collect on all sub pools
		m_subPools->ForAllElements(boost::bind(&CRuEngine_TexturePool::GarbageCollectCallback, this, _1, _2));
	}

	return TRUE;
}

INT32 CRuEngine_TexturePool::GetTexturePoolFootprint()
{
	m_temp_Counter0 = 0;

	if(m_subPools)
	{
		m_subPools->ForAllElements(boost::bind(&CRuEngine_TexturePool::GetTexturePoolFootprintCallback, this, _1, _2));
	}

	return m_temp_Counter0;
}

CRuGUID CRuEngine_TexturePool::BuildSubPoolKey(INT32 dim0, INT32 dim1, INT32 dim2, INT32 levelCount, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool)
{
	UINT32 key0 = ((dim0 & 0xFFFF) << 16) | (dim1 & 0xFFFF);
	UINT32 key1 = ((dim2 & 0xFFFF) << 16) | (levelCount & 0xFFFF);
	UINT32 key2 = static_cast<UINT32>(surfaceFormat);
	UINT32 key3 = static_cast<UINT32>(resourcePool);

	return CRuGUID(key0, key1, key2, key3);
}

BOOL CRuEngine_TexturePool::InvalidateCallback(const void *key, void *data)
{
	SubPool *subPool = *reinterpret_cast<SubPool **>(data);

	if(subPool)
	{
		if(m_enableAutoRestore)
		{
			// Invalidate all textures on the used pool
			for(INT32 i = 0; i < subPool->m_usedTextures.Count(); ++i)
			{
				subPool->m_usedTextures[i]->Invalidate();
			}

			while(subPool->m_unusedTextures.Count() > 0)
			{
				// Dequeue texture off unused texture list
				IRuBaseTexture *texture = subPool->m_unusedTextures.Dequeue();

				// Invalidate texture
				texture->Invalidate();

				// Release texture
				ruSAFE_RELEASE(texture);
			}
		}
		else
		{
			for(INT32 i = 0; i < subPool->m_usedTextures.Count(); ++i)
			{
				// Invalidate texture
				subPool->m_usedTextures[i]->Invalidate();

				// Release texture
				ruSAFE_RELEASE(subPool->m_usedTextures[i]);
			}

			subPool->m_usedTextures.Clear();

			while(subPool->m_unusedTextures.Count() > 0)
			{
				// Dequeue texture off unused texture list
				IRuBaseTexture *texture = subPool->m_unusedTextures.Dequeue();

				// Invalidate texture
				texture->Invalidate();

				// Release texture
				ruSAFE_RELEASE(texture);
			}

			ruSAFE_DELETE(subPool);
		}
	}

	return TRUE;
}

BOOL CRuEngine_TexturePool::DestroyCallback(const void *key, void *data)
{
	SubPool *subPool = *reinterpret_cast<SubPool **>(data);

	if(subPool)
	{
		for(INT32 i = 0; i < subPool->m_usedTextures.Count(); ++i)
		{
			// Invalidate texture
			subPool->m_usedTextures[i]->Invalidate();

			// Release texture
			ruSAFE_RELEASE(subPool->m_usedTextures[i]);
		}

		subPool->m_usedTextures.Clear();

		while(subPool->m_unusedTextures.Count() > 0)
		{
			// Dequeue texture off unused texture list
			IRuBaseTexture *texture = subPool->m_unusedTextures.Dequeue();

			// Invalidate texture
			texture->Invalidate();

			// Release texture
			ruSAFE_RELEASE(texture);
		}

		ruSAFE_DELETE(subPool);
	}

	return TRUE;
}

BOOL CRuEngine_TexturePool::RestoreCallback(const void *key, void *data)
{
	SubPool *subPool = *reinterpret_cast<SubPool **>(data);

	if(subPool)
	{
		// Restore textures on the used pool
		for(INT32 i = 0; i < subPool->m_usedTextures.Count(); ++i)
		{
			subPool->m_usedTextures[i]->Restore();
		}
	}

	return TRUE;
}

BOOL CRuEngine_TexturePool::GarbageCollectCallback(const void *key, void *data)
{
	SubPool *subPool = *reinterpret_cast<SubPool **>(data);

	if(subPool)
	{
		// Process textures which are in use
		for(INT32 i = 0; i < subPool->m_usedTextures.Count(); ++i)
		{
			// If the texture has a ref count of one, it is no longer being used
			if(subPool->m_usedTextures[i]->GetRefCount() == 1)
			{
				// Set GC marker to zero
				subPool->m_usedTextures[i]->SetGCMarker(0);

				// Queue into unused textures queue
				subPool->m_unusedTextures.Queue(subPool->m_usedTextures[i]);

				// Remove from used textures list
				subPool->m_usedTextures.RemoveAt(i);
				--i;
			}
		}

		// Process textures which are not in use
		INT32 unusedTextureCount = subPool->m_unusedTextures.Count();
		while(unusedTextureCount > 0)
		{
			// Dequeue texture off unused texture list
			IRuBaseTexture *texture = subPool->m_unusedTextures.Dequeue();

			// Increment GC marker
			INT32 gcMarker = texture->GetGCMarker() + 1;
			texture->SetGCMarker(gcMarker);

			// If the GC collection limit has been reached, collect the texture
			if(gcMarker > 30)
			{
				// Invalidate texture
				texture->Invalidate();

				// Release texture
				ruSAFE_RELEASE(texture);
			}
			else
			{
				// Place texture back into queue
				subPool->m_unusedTextures.Queue(texture);
			}

			// Decrement loop counter
			--unusedTextureCount;
		}
	}

	return TRUE;
}

BOOL CRuEngine_TexturePool::GetTexturePoolFootprintCallback(const void *key, void *data)
{
	SubPool *subPool = *reinterpret_cast<SubPool **>(data);

	if(subPool)
	{
		IRuBaseTexture *representativeTexture = NULL;

		if(subPool->m_usedTextures.Count() > 0)
		{
			representativeTexture = subPool->m_usedTextures[0];
		}
		else if(subPool->m_unusedTextures.Count() > 0)
		{
			representativeTexture = subPool->m_unusedTextures.Dequeue();
			subPool->m_unusedTextures.Queue(representativeTexture);
		}

		if(representativeTexture)
		{
			INT32 textureCount = subPool->m_usedTextures.Count() + subPool->m_unusedTextures.Count();
			m_temp_Counter0 += representativeTexture->GetMemoryFootprint() * textureCount;
		}
	}

	return TRUE;
}

BOOL CRuEngine_TexturePool::HandleEvent_BeginScene(RuEventArgs *eventArgs)
{
	// Perform automatic garbage collection on managed textures
	return this->GarbageCollect();
}

BOOL CRuEngine_TexturePool::HandleEvent_DeviceReset(RuEventArgs *eventArgs)
{
	// Perform automatic invalidation on managed textures
	return this->Invalidate();
}

BOOL CRuEngine_TexturePool::HandleEvent_DeviceRestore(RuEventArgs *eventArgs)
{
	return this->Restore();
}

BOOL CRuEngine_TexturePool::HandleEvent_Shutdown(RuEventArgs *eventArgs)
{
	// Call destroy on all subpools
	if(m_subPools)
	{
		m_subPools->ForAllElements(boost::bind(&CRuEngine_TexturePool::DestroyCallback, this, _1, _2));
	}

	// Delete dynamic arrays
	ruSAFE_DELETE(m_subPools);

	// Unregister all event handlers
	if(m_engine)
	{
		m_engine->Event_BeginScene().UnregisterHandler(m_guid);
		m_engine->Event_DeviceReset().UnregisterHandler(m_guid);
		m_engine->Event_Shutdown().UnregisterHandler(m_guid);
		m_engine = NULL;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
