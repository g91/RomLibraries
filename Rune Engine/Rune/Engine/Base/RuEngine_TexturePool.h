/*!
	@file RuEngine_TexturePool.h

	Copyright (c) 2004-2009 All rights reserved
*/

#pragma once

// Rune Engine Base
#include "../../Engine/Base/RuEngine_Engine.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Texture pool

	@author John Tang
	@version 2009.02.09
*/
class CRuEngine_TexturePool
{
protected:
	// Definition: IRuBaseTexture *CreateTextureCallback(INT32 dim0, INT32 dim1, INT32 dim2, INT32 levelCount, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool);
	typedef boost::function<IRuBaseTexture * (INT32 dim0, INT32 dim1, INT32 dim2, INT32 levelCount, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool)> CreateTextureCallback;

	struct SubPool
	{
		CRuArrayList<IRuBaseTexture *>		m_usedTextures;		// used textures with ref count > 1
		CRuQueue<IRuBaseTexture *>			m_unusedTextures;	// unused textures with ref count = 1, start using GCMarker to prepare for collection
	};

	CRuGUID									m_guid;

	CRuHashMap<CRuGUID, SubPool *>*			m_subPools;

	IRuEngine*								m_engine;
	CreateTextureCallback					m_createTextureFn;
	BOOL									m_enableAutoRestore;

	INT32									m_temp_Counter0;

public:
											CRuEngine_TexturePool();
	virtual									~CRuEngine_TexturePool();

	BOOL									Initialize(IRuEngine *engine, CreateTextureCallback createTextureFn, BOOL enableAutoRestore);
	IRuBaseTexture*							CreateTexture(INT32 dim0, INT32 dim1, INT32 dim2, INT32 levelCount, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool);
	BOOL									Invalidate();
	BOOL									Restore();
	BOOL									GarbageCollect();

	INT32									GetTexturePoolFootprint();

protected:
	CRuGUID									BuildSubPoolKey(INT32 dim0, INT32 dim1, INT32 dim2, INT32 levelCount, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool);

	BOOL									InvalidateCallback(const void *key, void *data);
	BOOL									RestoreCallback(const void *key, void *data);
	BOOL									DestroyCallback(const void *key, void *data);
	BOOL									GarbageCollectCallback(const void *key, void *data);
	BOOL									GetTexturePoolFootprintCallback(const void *key, void *data);

	BOOL									HandleEvent_BeginScene(RuEventArgs *eventArgs);
	BOOL									HandleEvent_DeviceReset(RuEventArgs *eventArgs);
	BOOL									HandleEvent_DeviceRestore(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Shutdown(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
