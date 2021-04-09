/*!
	@file RuTextureAtlas_Aggregate.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUTEXTUREATLAS_AGGREGATE_H_
#define _RUTEXTUREATLAS_AGGREGATE_H_

#include "../../Engine/Texture/RuTextureAtlas_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	CRuTextureAtlas_Aggregate

	The eviction marker is used for controlling how the atlas evicts bound textures
	when there is no available space to bind a new texture. When a texture is bound,
	it is marked with the current eviction marker. This marker makes the guarantee
	that the newly bound texture will not be unbound by the atlas until at least
	the next time the eviction marker is incremented.

	@author John Tang
	@version 2006/09/21
*/
class CRuTextureAtlas_Aggregate : public CRuTextureAtlas_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct SubAtlasDesc
	{
		CRuTextureAtlas_Base*				m_subAtlas;
		CRuLRUQueue<CRuGUID, INT32>*		m_bindQueue;
	};

	struct TextureDesc
	{
		INT32								m_evictionMarker;									//!< Eviction marker, used for controlling atlas evictions
		IRuObject*							m_bindingObject;									//!< Binding object reference
		CRuTextureAtlas_Base*				m_atlas;											//!< Atlas that the texture resides within
		INT32								m_handle;											//!< Slot that the texture resides within
		INT32								m_width;
		INT32								m_height;
	};

	INT32									m_evictionMarker;

	CRuArrayList<SubAtlasDesc>				m_subAtlases;										//!< List of sub atlases
	CRuArrayList<TextureDesc>				m_boundTextures;									//!< List of bound textures
	CRuStack<INT32>							m_unusedHandles;									//!< Stack of unused handles

	CRuLRUQueue<CRuGUID, INT32>*			m_textureQueue;

public:
											CRuTextureAtlas_Aggregate();
	virtual									~CRuTextureAtlas_Aggregate();

	BOOL									BindSubAtlas(CRuTextureAtlas_Base *subAtlas);
	BOOL									UnbindSubAtlas(CRuTextureAtlas_Base *subAtlas);

	INT32									IncrementEvictionMarker();

	virtual IRuTexture*						GetAtlasTexture();
	virtual IRuTexture*						GetTexture(INT32 textureHandle);
	virtual UINT32							GetTextureWidth(INT32 textureHandle);
	virtual UINT32							GetTextureHeight(INT32 textureHandle);

	virtual BOOL							CanBindIntoAtlas(IRuObject *bindingObject);
	virtual INT32							BindIntoAtlas(IRuObject *bindingObject, BOOL atlasBindingRequired);
	virtual BOOL							UnbindFromAtlas(INT32 textureHandle);
	virtual BOOL							UnbindAllFromAtlas();
	virtual BOOL							TranslateUV(INT32 textureHandle, REAL u, REAL v, REAL uBias, REAL vBias, REAL texelOffset, REAL &uOut, REAL &vOut);

protected:
	void									FlushTextureQueue();
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
