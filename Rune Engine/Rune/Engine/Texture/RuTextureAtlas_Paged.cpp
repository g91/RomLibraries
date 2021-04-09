/*!
	@file RuTextureAtlas_Paged.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Engine/Texture/RuTextureAtlas_Paged.h"

#include "../../../Tools/dxtLib/dxtutility.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuTextureAtlas_Paged, CRuTextureAtlas_Base)
ruHEAP_DEFINE_SUBCLASS(CRuTextureAtlas_Paged, "CRuTextureAtlas_Paged", "CRuTextureAtlas_Base")
ruCLASSGUID_DEFINE(CRuTextureAtlas_Paged, 0x65EBE69D, 0xDA674ba1, 0x8802CE07, 0x82E719E4)

// ************************************************************************************************************************************************************

CRuTextureAtlas_Paged::AtlasPageDesc::AtlasPageDesc(IRuTexture *atlasTexture, INT32 originX, INT32 originY, INT32 pageSize_X, INT32 pageSize_Y)
:	m_atlasTexture(atlasTexture),
	m_pageOrigin_X(originX),
	m_pageOrigin_Y(originY),
	m_pageSize_X(pageSize_X),
	m_pageSize_Y(pageSize_Y),
	m_numSlots(0)
{
}

CRuTextureAtlas_Paged::AtlasPageDesc::~AtlasPageDesc()
{
}

BOOL CRuTextureAtlas_Paged::AtlasPageDesc::Assign(INT32 textureWidth, INT32 textureHeight)
{
	// If the page has already been assigned, fail
	if(m_numSlots > 0)
	{
		return FALSE;
	}

	// Save assigned texture dimensions
	m_textureWidth = textureWidth;
	m_textureHeight = textureHeight;

	m_uOffset = (REAL) m_pageOrigin_X / m_atlasTexture->GetLevelWidth(0);
	m_vOffset = (REAL) m_pageOrigin_Y / m_atlasTexture->GetLevelHeight(0);
	m_uScale = (REAL) m_textureWidth / m_atlasTexture->GetLevelWidth(0);
	m_vScale = (REAL) m_textureHeight / m_atlasTexture->GetLevelHeight(0);

	// Allocate slot handle array and initialize unoccupied slot indices
	m_numSlots = (m_pageSize_X / m_textureWidth) * (m_pageSize_Y / m_textureHeight);

	for(INT32 i = m_numSlots - 1; i >= 0; --i)
	{
		m_unoccupiedSlots.Push(i);
	}

	return TRUE;
}

BOOL CRuTextureAtlas_Paged::AtlasPageDesc::Unassign()
{
	if(IsUnused())
	{
		m_numSlots = 0;
		m_unoccupiedSlots.Clear();

		return TRUE;
	}

	return FALSE;
}

INT32 CRuTextureAtlas_Paged::AtlasPageDesc::BindIntoPage(INT32 textureWidth, INT32 textureHeight, RuSurfaceFormat textureFormat)
{
	if(m_unoccupiedSlots.IsEmpty() || textureWidth != m_textureWidth || textureHeight != m_textureHeight)
	{
		return -1;
	}

	// Pop an available slot off the stack
	return m_unoccupiedSlots.Pop();
}

INT32 CRuTextureAtlas_Paged::AtlasPageDesc::BindIntoPage(IRuObject *bindingObject)
{
	INT32 textureWidth = 0;
	INT32 textureHeight = 0;
	RuSurfaceFormat textureFormat = ruSURFACEFMT_A8R8G8B8;

	INT32 slotIndex = -1;

	if(bindingObject->GetType().IsSubClassOf(IRuTextureView::Type()))
	{
		// Get properties of the texture view
		IRuTextureView *textureView = static_cast<IRuTextureView *>(bindingObject);
		textureWidth = textureView->GetViewWidth();
		textureHeight = textureView->GetViewHeight();
		textureFormat = textureView->GetViewFormat();

		// Bind texture into a page
		slotIndex = BindIntoPage(textureWidth, textureHeight, textureFormat);

		// Calculate the texture position within the page
		INT32 slotsPerRow = (m_pageSize_X / m_textureWidth);
		INT32 slotX = slotIndex % slotsPerRow;
		INT32 slotY = slotIndex / slotsPerRow;
		INT32 textureOrigin_X = slotX * m_textureWidth + m_pageOrigin_X;
		INT32 textureOrigin_Y = slotY * m_textureHeight + m_pageOrigin_Y;

		// Set atlas texture as basis texture for the texture view
		textureView->SetBasisTexture(m_atlasTexture);

		// Set viewport parameters
		textureView->SetViewOrigin(textureOrigin_X, textureOrigin_Y);
	}
	else if(bindingObject->GetType().IsSubClassOf(IRuTexture::Type()))
	{
		// Get properties of the texture
		IRuTexture *texture = static_cast<IRuTexture *>(bindingObject);
		textureWidth = texture->GetLevelWidth(0);
		textureHeight = texture->GetLevelHeight(0);
		textureFormat = texture->GetSurfaceFormat();

		// Bind texture into a page
		slotIndex = BindIntoPage(textureWidth, textureHeight, textureFormat);

		// Copy texture data
		if(slotIndex >= 0)
		{
			// Calculate the texture position within the page
			INT32 slotsPerRow = (m_pageSize_X / m_textureWidth);
			INT32 slotX = slotIndex % slotsPerRow;
			INT32 slotY = slotIndex / slotsPerRow;
			INT32 textureOrigin_X = slotX * m_textureWidth + m_pageOrigin_X;
			INT32 textureOrigin_Y = slotY * m_textureHeight + m_pageOrigin_Y;

			// Store the texture into the page
			UINT32 numLevels = min(texture->GetLevelCount(), m_atlasTexture->GetLevelCount());
			for(UINT32 i = 0, srcLevel = 0, destLevel = 0, curTexX = textureOrigin_X, curTexY = textureOrigin_Y; i < numLevels; ++i, ++srcLevel, ++destLevel, curTexX /= 2, curTexY /= 2)
			{
				void *srcBits, *destBits;
				INT32 srcPitch, destPitch;

				// Lock destination and source
				texture->Lock(srcLevel, &srcBits, &srcPitch);
				m_atlasTexture->Lock(destLevel, &destBits, &destPitch);

				switch(textureFormat)
				{
					case ruSURFACEFMT_A8R8G8B8:
						{
							INT32 srcWidth = texture->GetLevelWidth(srcLevel);
							INT32 srcHeight = texture->GetLevelHeight(srcLevel);

							BYTE *dstPtr = (BYTE *) destBits;
							BYTE *srcPtr = (BYTE *) srcBits;

							for(INT32 y = 0; y < srcHeight; ++y)
							{
								memcpy(&dstPtr[curTexX * 4 + (curTexY + y) * destPitch], &srcPtr[y * srcPitch], srcWidth * 4);
							}
						}

						break;

					case ruSURFACEFMT_DXT1:
						BltDXT(destBits, TEXTURE_DXT1, curTexX, curTexY, m_atlasTexture->GetLevelWidth(destLevel), m_atlasTexture->GetLevelHeight(destLevel), srcBits, TEXTURE_DXT1, texture->GetLevelWidth(srcLevel), texture->GetLevelHeight(srcLevel), 0xFFFFFFFF, DXT_ROP_SELECTSRC);
						break;

					case ruSURFACEFMT_DXT3:
						BltDXT(destBits, TEXTURE_DXT3, curTexX, curTexY, m_atlasTexture->GetLevelWidth(destLevel), m_atlasTexture->GetLevelHeight(destLevel), srcBits, TEXTURE_DXT3, texture->GetLevelWidth(srcLevel), texture->GetLevelHeight(srcLevel), 0xFFFFFFFF, DXT_ROP_SELECTSRC);
						break;

					case ruSURFACEFMT_DXT5:
						BltDXT(destBits, TEXTURE_DXT5, curTexX, curTexY, m_atlasTexture->GetLevelWidth(destLevel), m_atlasTexture->GetLevelHeight(destLevel), srcBits, TEXTURE_DXT5, texture->GetLevelWidth(srcLevel), texture->GetLevelHeight(srcLevel), 0xFFFFFFFF, DXT_ROP_SELECTSRC);
						break;
				}

				// Unlock destination and source
				texture->Unlock(srcLevel);
				m_atlasTexture->Unlock(destLevel);
			}
		}
	}

	// Return slot index
	return slotIndex;
}

BOOL CRuTextureAtlas_Paged::AtlasPageDesc::UnbindFromPage(INT32 slotIndex)
{
	if(slotIndex >= 0 && slotIndex < m_numSlots)
	{
		if(m_unoccupiedSlots.Contains(slotIndex) == FALSE)
		{
			m_unoccupiedSlots.Push(slotIndex);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuTextureAtlas_Paged::AtlasPageDesc::TranslateUV(INT32 slotIndex, REAL u, REAL v, REAL uBias, REAL vBias, REAL texelOffset, REAL &uOut, REAL &vOut)
{
	if(slotIndex >= 0 && slotIndex < m_numSlots)
	{
		INT32 slotsPerRow = (m_pageSize_X / m_textureWidth);
		INT32 slotX = slotIndex % slotsPerRow;
		INT32 slotY = slotIndex / slotsPerRow;
		INT32 textureOrigin_X = slotX * m_textureWidth + m_pageOrigin_X;
		INT32 textureOrigin_Y = slotY * m_textureHeight + m_pageOrigin_Y;

		REAL subTextureUOffset = (REAL) textureOrigin_X / m_atlasTexture->GetLevelWidth(0);
		REAL subTextureVOffset = (REAL) textureOrigin_Y / m_atlasTexture->GetLevelHeight(0);

		REAL texelOffsetX = texelOffset / m_atlasTexture->GetLevelWidth(0);
		REAL texelOffsetY = texelOffset / m_atlasTexture->GetLevelHeight(0);

		uOut = u * m_uScale + subTextureUOffset + texelOffsetX * uBias;
		vOut = v * m_vScale + subTextureVOffset + texelOffsetY * vBias;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTextureAtlas_Paged::AtlasPageDesc::IsUnused() const
{
	return m_unoccupiedSlots.Count() == m_numSlots;
}

BOOL CRuTextureAtlas_Paged::AtlasPageDesc::HasAvailableSlots() const
{
	return m_unoccupiedSlots.IsEmpty() == FALSE;
}

// ************************************************************************************************************************************************************

CRuTextureAtlas_Paged::CRuTextureAtlas_Paged()
:	m_atlasTexture(NULL)
{
}

CRuTextureAtlas_Paged::~CRuTextureAtlas_Paged()
{
	ruSAFE_RELEASE(m_atlasTexture);

	for(INT32 i = 0; i < m_pages.Count(); ++i)
	{
		delete m_pages[i];
	}

	while(m_unoccupiedPages.IsEmpty() == FALSE)
	{
		delete m_unoccupiedPages.Pop();
	}
}

BOOL CRuTextureAtlas_Paged::Initialize(IRuTexture *atlasTexture, INT32 pageSize_X, INT32 pageSize_Y, INT32 reserved_X, INT32 reserved_Y)
{
	if(m_atlasTexture != NULL || atlasTexture == NULL)
	{
		return FALSE;
	}

	// Save reference to the atlas texture
	m_atlasTexture = atlasTexture;
	m_atlasTexture->AddRef();
	m_atlasFormat = m_atlasTexture->GetSurfaceFormat();

	m_pageSize_X = pageSize_X;
	m_pageSize_Y = pageSize_Y;

	// Initialize texture pages
	INT32 xPages = m_atlasTexture->GetLevelWidth(0) / m_pageSize_X;
	INT32 yPages = m_atlasTexture->GetLevelHeight(0) / m_pageSize_Y;
	INT32 numPages = xPages * yPages;

	for(INT32 i = 0; i < yPages; ++i)
	{
		for(INT32 j = 0; j < xPages; ++j)
		{
			INT32 pageOrigin_X = j * m_pageSize_X;
			INT32 pageOrigin_Y = i * m_pageSize_Y;

			// If the page resides within the reserved area, skip it
			if(pageOrigin_X < reserved_X && pageOrigin_Y < reserved_Y)
			{
				continue;
			}

			// Construct page descriptor
			AtlasPageDesc *pageDesc = ruNEW AtlasPageDesc(m_atlasTexture, pageOrigin_X, pageOrigin_Y, m_pageSize_X, m_pageSize_Y);

			// Push page descriptor onto unoccupied stack
			m_unoccupiedPages.Push(pageDesc);
		}
	}

	return TRUE;
}

IRuTexture *CRuTextureAtlas_Paged::GetAtlasTexture()
{
	return m_atlasTexture;
}

IRuTexture *CRuTextureAtlas_Paged::GetTexture(INT32 textureHandle)
{
	return m_atlasTexture;
}

UINT32 CRuTextureAtlas_Paged::GetTextureWidth(INT32 textureHandle)
{
	if(textureHandle >= 0 && textureHandle < m_boundTextures.Count())
	{
		if(m_boundTextures[textureHandle].m_page)
		{
			return m_boundTextures[textureHandle].m_page->m_textureWidth;
		}
	}

	return 4;
}

UINT32 CRuTextureAtlas_Paged::GetTextureHeight(INT32 textureHandle)
{
	if(textureHandle >= 0 && textureHandle < m_boundTextures.Count())
	{
		if(m_boundTextures[textureHandle].m_page)
		{
			return m_boundTextures[textureHandle].m_page->m_textureHeight;
		}
	}

	return 4;
}

BOOL CRuTextureAtlas_Paged::CanBindIntoAtlas(IRuObject *bindingObject)
{
	if(bindingObject->GetType().IsSubClassOf(IRuTextureView::Type()))
	{
		// Get properties of the texture view
		IRuTextureView *textureView = static_cast<IRuTextureView *>(bindingObject);
		INT32 textureWidth = textureView->GetViewWidth();
		INT32 textureHeight = textureView->GetViewHeight();
		RuSurfaceFormat textureFormat = textureView->GetViewFormat();

		// Verify that the surface format is identical and that the texture fits in a page (reject all textures that is the same size as the page for performance reasons)
		if(textureFormat != m_atlasFormat || textureWidth > m_pageSize_X || textureHeight > m_pageSize_Y || (textureWidth == m_pageSize_X && textureHeight == m_pageSize_Y))
		{
			return FALSE;
		}
	}
	else if(bindingObject->GetType().IsSubClassOf(IRuTexture::Type()))
	{
		// Get properties of the texture
		IRuTexture *texture = static_cast<IRuTexture *>(bindingObject);
		RuSurfaceFormat textureFormat = texture->GetSurfaceFormat();
		INT32 textureWidth = texture->GetLevelWidth(0);
		INT32 textureHeight = texture->GetLevelHeight(0);

		// Verify that the surface format is identical and that the texture fits in a page (reject all textures that is the same size as the page for performance reasons)
		if(textureFormat != m_atlasFormat || textureWidth > m_pageSize_X || textureHeight > m_pageSize_Y || (textureWidth == m_pageSize_X && textureHeight == m_pageSize_Y))
		{
			return FALSE;
		}
	}

	return TRUE;
}

INT32 CRuTextureAtlas_Paged::BindIntoAtlas(IRuObject *bindingObject, BOOL atlasBindingRequired)
{
	INT32 textureWidth = 0;
	INT32 textureHeight = 0;
	RuSurfaceFormat textureFormat = ruSURFACEFMT_A8R8G8B8;

	if(bindingObject->GetType().IsSubClassOf(IRuTextureView::Type()))
	{
		// Get properties of the texture view
		IRuTextureView *textureView = static_cast<IRuTextureView *>(bindingObject);
		textureWidth = textureView->GetViewWidth();
		textureHeight = textureView->GetViewHeight();
		textureFormat = textureView->GetViewFormat();
	}
	else if(bindingObject->GetType().IsSubClassOf(IRuTexture::Type()))
	{
		// Get properties of the texture
		IRuTexture *texture = static_cast<IRuTexture *>(bindingObject);
		textureWidth = texture->GetLevelWidth(0);
		textureHeight = texture->GetLevelHeight(0);
		textureFormat = texture->GetSurfaceFormat();
	}

	// Find an available page to use
	INT32 pageIndex = FindAvailablePage(textureWidth, textureHeight, textureFormat);

	if(pageIndex == -1)
	{
		return -1;
	}

	// Bind binding object into page
	INT32 textureHandle = m_unusedHandles.IsEmpty() ? m_boundTextures.Count() : m_unusedHandles.Pop();
	INT32 slotIndex = m_pages[pageIndex]->BindIntoPage(bindingObject);

	if(textureHandle >= m_boundTextures.Count())
	{
		m_boundTextures.Add(TextureDesc());
	}

	if(slotIndex >= 0)
	{
		// Store reference to binding object
		m_boundTextures[textureHandle].m_bindingObject = bindingObject;
		m_boundTextures[textureHandle].m_bindingObject->AddRef();

		// Store page and slot index
		m_boundTextures[textureHandle].m_page = m_pages[pageIndex];
		m_boundTextures[textureHandle].m_slotIndex = slotIndex;

		return textureHandle;
	}
	else
	{
		m_unusedHandles.Push(textureHandle);
	}

	return -1;
}

BOOL CRuTextureAtlas_Paged::UnbindFromAtlas(INT32 textureHandle)
{
	if(textureHandle >= 0 && textureHandle < m_boundTextures.Count())
	{
		if(m_boundTextures[textureHandle].m_page)
		{
			if(m_boundTextures[textureHandle].m_page->UnbindFromPage(m_boundTextures[textureHandle].m_slotIndex) != false)
			{
				// Return handle to stack of unused handles
				m_unusedHandles.Push(textureHandle);

				// If the page is no longer used, return it
				if(m_boundTextures[textureHandle].m_page->IsUnused() != false)
				{
					// Unassign page and push page back onto unoccupied page stack
					m_boundTextures[textureHandle].m_page->Unassign();
					m_unoccupiedPages.Push(m_boundTextures[textureHandle].m_page);

					// Remove page from list of active pages
					for(INT32 i = 0; i < m_pages.Count(); ++i)
					{
						if(m_pages[i] == m_boundTextures[textureHandle].m_page)
						{
							m_pages.RemoveAt(i);
							break;
						}
					}
				}

				if(m_boundTextures[textureHandle].m_bindingObject->GetType().IsSubClassOf(IRuTextureView::Type()))
				{
					// Get properties of the texture view
					IRuTextureView *textureView = static_cast<IRuTextureView *>(m_boundTextures[textureHandle].m_bindingObject);
					textureView->Invalidate();
				}
				else if(m_boundTextures[textureHandle].m_bindingObject->GetType().IsSubClassOf(IRuTexture::Type()))
				{
					// Get properties of the texture
					IRuTexture *texture = static_cast<IRuTexture *>(m_boundTextures[textureHandle].m_bindingObject);
				}

				// Release reference to binding object
				ruSAFE_RELEASE(m_boundTextures[textureHandle].m_bindingObject);

				// Clear page and slot index
				m_boundTextures[textureHandle].m_page = NULL;
				m_boundTextures[textureHandle].m_slotIndex = -1;

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CRuTextureAtlas_Paged::UnbindAllFromAtlas()
{
	for(INT32 i = 0; i < m_boundTextures.Count(); ++i)
	{
		UnbindFromAtlas(i);
	}

	return TRUE;
}

BOOL CRuTextureAtlas_Paged::TranslateUV(INT32 textureHandle, REAL u, REAL v, REAL uBias, REAL vBias, REAL texelOffset, REAL &uOut, REAL &vOut)
{
	if(textureHandle >= 0 && textureHandle < m_boundTextures.Count())
	{
		if(m_boundTextures[textureHandle].m_page)
		{
			return m_boundTextures[textureHandle].m_page->TranslateUV(m_boundTextures[textureHandle].m_slotIndex, u, v, uBias, vBias, texelOffset, uOut, vOut);
		}
	}

	return FALSE;
}

INT32 CRuTextureAtlas_Paged::FindAvailablePage(INT32 textureWidth, INT32 textureHeight, RuSurfaceFormat textureFormat)
{
	// Verify that the surface format is identical and that the texture fits in a page
	if(textureFormat != m_atlasFormat || textureWidth > m_pageSize_X || textureHeight > m_pageSize_Y)
	{
		return -1;
	}

	// Search existing texture pages for one that matches the texture
	INT32 pageIndex = -1;
	for(INT32 i = 0; i < m_pages.Count(); ++i)
	{
		if(m_pages[i]->m_textureWidth == textureWidth && m_pages[i]->m_textureHeight == textureHeight && m_pages[i]->HasAvailableSlots())
		{
			pageIndex = i;
			break;
		}
	}

	// Allocate a new page if necessary
	if(pageIndex == -1)
	{
		// If all pages are occupied, then return failure
		if(m_unoccupiedPages.IsEmpty())
		{
			return -1;
		}

		// Assign an empty page to the texture dimensions
		AtlasPageDesc *pageDesc = m_unoccupiedPages.Pop();
		pageDesc->Assign(textureWidth, textureHeight);
		m_pages.Add(pageDesc);

		pageIndex = m_pages.Count() - 1;
	}

	return pageIndex;
}

// ************************************************************************************************************************************************************

#pragma managed
