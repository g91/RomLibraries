/*!
	@file RuTextureAtlas_Paged.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUTEXTUREATLAS_PAGED_H_
#define _RUTEXTUREATLAS_PAGED_H_

#include "../../Engine/Texture/RuTextureAtlas_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuTextureAtlas_Paged : public CRuTextureAtlas_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct AtlasPageDesc
	{
		IRuTexture*							m_atlasTexture;										//!< The atlas texture

		INT32								m_pageOrigin_X;										//!< Page origin x in pixels
		INT32								m_pageOrigin_Y;										//!< Page origin y in pixels
		INT32								m_pageSize_X;										//!< Dimension of the texture atlas page
		INT32								m_pageSize_Y;

		REAL								m_uOffset;
		REAL								m_vOffset;
		REAL								m_uScale;
		REAL								m_vScale;

		INT32								m_textureWidth;
		INT32								m_textureHeight;

		INT32								m_numSlots;
		CRuStack<INT32>						m_unoccupiedSlots;

											AtlasPageDesc(IRuTexture *atlasTexture, INT32 originX, INT32 originY, INT32 pageSize_X, INT32 pageSize_Y);
											~AtlasPageDesc();

		BOOL								Assign(INT32 textureWidth, INT32 textureHeight);
		BOOL								Unassign();

		INT32								BindIntoPage(INT32 textureWidth, INT32 textureHeight, RuSurfaceFormat textureFormat);
		INT32								BindIntoPage(IRuObject *bindingObject);
		BOOL								UnbindFromPage(INT32 slotIndex);

		BOOL								TranslateUV(INT32 slotIndex, REAL u, REAL v, REAL uBias, REAL vBias, REAL texelOffset, REAL &uOut, REAL &vOut);

		BOOL								IsUnused() const;
		BOOL								HasAvailableSlots() const;
	};

	struct TextureDesc
	{
		IRuObject*							m_bindingObject;									//!< Source binding object
		AtlasPageDesc*						m_page;												//!< Atlas page that the texture resides within
		INT32								m_slotIndex;										//!< Slot that the texture resides within
	};

	// Settings
	INT32									m_pageSize_X;										//!< Dimension of the texture atlas page
	INT32									m_pageSize_Y;

	IRuTexture*								m_atlasTexture;										//!< The atlas texture
	RuSurfaceFormat							m_atlasFormat;										//!< Texture format of the atlas
	CRuArrayList<AtlasPageDesc *>			m_pages;											//!< List of texture pages
	CRuStack<AtlasPageDesc *>				m_unoccupiedPages;									//!< Stack of unoccupied texture pages
	CRuArrayList<TextureDesc>				m_boundTextures;									//!< List of bound textures
	CRuStack<INT32>							m_unusedHandles;									//!< Stack of unused handles

public:
											CRuTextureAtlas_Paged();
	virtual									~CRuTextureAtlas_Paged();

	BOOL									Initialize(IRuTexture *atlasTexture, INT32 pageSize_X, INT32 pageSize_Y, INT32 reserved_X, INT32 reserved_Y);

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
	INT32									FindAvailablePage(INT32 textureWidth, INT32 textureHeight, RuSurfaceFormat textureFormat);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
