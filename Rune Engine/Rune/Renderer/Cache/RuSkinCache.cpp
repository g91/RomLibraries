/*!
	@file RuSkinCache.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../../Renderer/Cache/RuSkinCache.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuSkinCache, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuSkinCache, "CRuSkinCache", "IRuObject")
ruCLASSGUID_DEFINE(CRuSkinCache, 0xFE27AC86, 0x517643ae, 0x86F8FDA4, 0x6CC492DE)

// ************************************************************************************************************************************************************

CRuSkinCache::CRuSkinCache()
{
	// Create skinning cache
	m_skinningCache = ruNEW CRuHashMap<CRuGUID, RuCachedSkin>(1024);
}

CRuSkinCache::~CRuSkinCache()
{
	// Flush the skinning cache
	FlushSkinningCache();

	// Release the skinning cache
	delete m_skinningCache;
}

RuCachedSkin CRuSkinCache::GetCachedSkin(IRuMesh *mesh)
{
	RuCachedSkin cachedSkin;

	const CRuGUID &meshKey = mesh->GetGUID();
	DWORD meshKeyHash = meshKey.HashCode();

	// Locate the cached skin mesh for the current entry
	if(m_skinningCache->Get(meshKey, meshKeyHash, cachedSkin) == FALSE)
	{
		DWORD t0, t1, et;

		t0 = timeGetTime();

		if(mesh->GetType() == CRuMesh::Type())
		{
			// Set skinnable mesh to NULL
			cachedSkin.m_mesh = NULL;

			// Set GC marker to zero
			cachedSkin.m_gcMarker = 0;

			// Subdivide mesh
			cachedSkin.m_subdividedMeshes = ruNEW CRuArrayList<IRuMesh *>();
			static_cast<CRuMesh *>(mesh)->SkinnedMesh_Subdivide(74, cachedSkin.m_subdividedMeshes);

			// Put cached skin entry into list
			m_skinningCache->Add(meshKey, meshKeyHash, cachedSkin);
		}

		t1 = timeGetTime();
		et = t1 - t0;
		//char outputStr[256];
		//sprintf(outputStr, "Skinned mesh subdivision in %u ms.\n", et);
		//OutputDebugString(outputStr);
	}
	else
	{
		// Set GC marker to zero
		cachedSkin.m_gcMarker = 0;

		// Update cache entry
		m_skinningCache->Replace(meshKey, meshKeyHash, cachedSkin);
	}

	return cachedSkin;
}

void CRuSkinCache::GarbageCollectSkinningCache()
{
	m_gcSkinningCache.Clear();

	m_skinningCache->ForAllElements(boost::bind(&CRuSkinCache::GarbageCollectSkinningCacheCallback, this, _1, _2));

	for(INT32 i = 0; i < m_gcSkinningCache.Count(); ++i)
	{
		RuCachedSkin cachedSkin;
		if(m_skinningCache->Get(m_gcSkinningCache[i], m_gcSkinningCache[i].HashCode(), cachedSkin))
		{
/*
			char outputStr[2048];
			sprintf(outputStr, "GC skinning hash entry\n");
			OutputDebugString(outputStr);
*/
			// Release the skinnable mesh
			ruSAFE_RELEASE(cachedSkin.m_mesh);

			// Release any subdivided meshes
			if(cachedSkin.m_subdividedMeshes)
			{
				for(INT32 i = 0; i < cachedSkin.m_subdividedMeshes->Count(); ++i)
				{
					ruSAFE_RELEASE(cachedSkin.m_subdividedMeshes->Get(i));
				}

				ruSAFE_DELETE(cachedSkin.m_subdividedMeshes);
			}

			// Remove cache entry
			m_skinningCache->Remove(m_gcSkinningCache[i], m_gcSkinningCache[i].HashCode());
		}
	}
}

BOOL CRuSkinCache::GarbageCollectSkinningCacheCallback(const void *key, void *data)
{
	RuCachedSkin &cachedSkin = *reinterpret_cast<RuCachedSkin *>(data);

	// Update GC marker
	++cachedSkin.m_gcMarker;

	if(cachedSkin.m_gcMarker > 500)
	{
		m_gcSkinningCache.Add(*reinterpret_cast<const CRuGUID *>(key));
	}

	return TRUE;
}

void CRuSkinCache::FlushSkinningCache()
{
	m_skinningCache->ForAllElements(boost::bind(&CRuSkinCache::FlushSkinningCacheCallback, this, _1, _2));
	m_skinningCache->Clear();
}

BOOL CRuSkinCache::FlushSkinningCacheCallback(const void *key, void *data)
{
	RuCachedSkin &cachedSkin = *reinterpret_cast<RuCachedSkin *>(data);

	// Release the skinnable mesh
	ruSAFE_RELEASE(cachedSkin.m_mesh);

	// Release any subdivided meshes
	if(cachedSkin.m_subdividedMeshes)
	{
		for(INT32 i = 0; i < cachedSkin.m_subdividedMeshes->Count(); ++i)
		{
			ruSAFE_RELEASE(cachedSkin.m_subdividedMeshes->Get(i));
		}

		ruSAFE_DELETE(cachedSkin.m_subdividedMeshes);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
