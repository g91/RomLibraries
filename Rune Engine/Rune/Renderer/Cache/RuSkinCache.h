/*!
	@file RuSkinCache.h

	Copyright (c) 2004-2008 All rights reserved
*/

#ifndef _RUSKINCACHE_H_
#define _RUSKINCACHE_H_

#include "../../Engine/Geometry/RuMeshBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

struct RuCachedSkin
{
	INT32									m_gcMarker;										//!< GC marker
	CRuSkinnableMesh*						m_mesh;											//!< Cached skinnable mesh

	CRuArrayList<IRuMesh *>*				m_subdividedMeshes;								//!< List of sub-divided meshes
};

// ************************************************************************************************************************************************************

class CRuSkinCache : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuHashMap<CRuGUID, RuCachedSkin>*		m_skinningCache;
	CRuArrayList<CRuGUID>					m_gcSkinningCache;

public:
											CRuSkinCache();
	virtual									~CRuSkinCache();

	RuCachedSkin							GetCachedSkin(IRuMesh *mesh);

	void									GarbageCollectSkinningCache();

protected:
	BOOL									GarbageCollectSkinningCacheCallback(const void *key, void *data);

	void									FlushSkinningCache();
	BOOL									FlushSkinningCacheCallback(const void *key, void *data);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
