/*!
	@file RuTextureAtlas_Aggregate.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Engine/Texture/RuTextureAtlas_Aggregate.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuTextureAtlas_Aggregate, CRuTextureAtlas_Base)
ruHEAP_DEFINE_SUBCLASS(CRuTextureAtlas_Aggregate, "CRuTextureAtlas_Aggregate", "CRuTextureAtlas_Base")
ruCLASSGUID_DEFINE(CRuTextureAtlas_Aggregate, 0x833DB671, 0x66FB4488, 0x88323D81, 0x30826B1C)

// ************************************************************************************************************************************************************

CRuTextureAtlas_Aggregate::CRuTextureAtlas_Aggregate()
{
	m_evictionMarker = 0;

	m_textureQueue = ruNEW CRuLRUQueue<CRuGUID, INT32>(4096, 16384);
}

CRuTextureAtlas_Aggregate::~CRuTextureAtlas_Aggregate()
{
	UnbindAllFromAtlas();

	for(INT32 i = 0; i < m_subAtlases.Count(); ++i)
	{
		ruSAFE_RELEASE(m_subAtlases[i].m_subAtlas);
		delete m_subAtlases[i].m_bindQueue;
	}

	delete m_textureQueue;
}

BOOL CRuTextureAtlas_Aggregate::BindSubAtlas(CRuTextureAtlas_Base *subAtlas)
{
	INT32 atlasIndex = m_subAtlases.Count();

	m_subAtlases.Add(SubAtlasDesc());
	m_subAtlases[atlasIndex].m_subAtlas = subAtlas;
	m_subAtlases[atlasIndex].m_bindQueue = ruNEW CRuLRUQueue<CRuGUID, INT32>(4096, 16384);

	m_subAtlases[atlasIndex].m_subAtlas->AddRef();

	return TRUE;
}

BOOL CRuTextureAtlas_Aggregate::UnbindSubAtlas(CRuTextureAtlas_Base *subAtlas)
{
	return FALSE;
}

INT32 CRuTextureAtlas_Aggregate::IncrementEvictionMarker()
{
	return ++m_evictionMarker;
}

IRuTexture *CRuTextureAtlas_Aggregate::GetAtlasTexture()
{
	return NULL;
}

IRuTexture *CRuTextureAtlas_Aggregate::GetTexture(INT32 textureHandle)
{
	if(textureHandle >= 0 && textureHandle < m_boundTextures.Count())
	{
		// If the texture's atlas is non-null, then the texture is bound to it with handle specified by
		// m_handle. Otherwise, the texture is not bound.
		if(m_boundTextures[textureHandle].m_atlas)
		{
			return m_boundTextures[textureHandle].m_atlas->GetTexture(m_boundTextures[textureHandle].m_handle);
		}
		else
		{
			if(m_boundTextures[textureHandle].m_bindingObject->GetType().IsSubClassOf(IRuTextureView::Type()))
			{
				return NULL;
			}
			else if(m_boundTextures[textureHandle].m_bindingObject->GetType().IsSubClassOf(IRuTexture::Type()))
			{
				return static_cast<IRuTexture *>(m_boundTextures[textureHandle].m_bindingObject);
			}
		}
	}

	return NULL;
}

UINT32 CRuTextureAtlas_Aggregate::GetTextureWidth(INT32 textureHandle)
{
	if(textureHandle >= 0 && textureHandle < m_boundTextures.Count())
	{
		if(m_boundTextures[textureHandle].m_atlas)
		{
			return m_boundTextures[textureHandle].m_atlas->GetTextureWidth(m_boundTextures[textureHandle].m_handle);
		}
		else
		{
			if(m_boundTextures[textureHandle].m_bindingObject->GetType().IsSubClassOf(IRuTextureView::Type()))
			{
				return NULL;
			}
			else if(m_boundTextures[textureHandle].m_bindingObject->GetType().IsSubClassOf(IRuTexture::Type()))
			{
				return static_cast<IRuTexture *>(m_boundTextures[textureHandle].m_bindingObject)->GetLevelWidth(0);
			}
		}
	}

	return 4;
}

UINT32 CRuTextureAtlas_Aggregate::GetTextureHeight(INT32 textureHandle)
{
	if(textureHandle >= 0 && textureHandle < m_boundTextures.Count())
	{
		if(m_boundTextures[textureHandle].m_atlas)
		{
			return m_boundTextures[textureHandle].m_atlas->GetTextureHeight(m_boundTextures[textureHandle].m_handle);
		}
		else
		{
			if(m_boundTextures[textureHandle].m_bindingObject->GetType().IsSubClassOf(IRuTextureView::Type()))
			{
				return NULL;
			}
			else if(m_boundTextures[textureHandle].m_bindingObject->GetType().IsSubClassOf(IRuTexture::Type()))
			{
				return static_cast<IRuTexture *>(m_boundTextures[textureHandle].m_bindingObject)->GetLevelHeight(0);
			}
		}
	}

	return 4;
}

BOOL CRuTextureAtlas_Aggregate::CanBindIntoAtlas(IRuObject *bindingObject)
{
	if(bindingObject->GetType().IsSubClassOf(IRuTextureView::Type()))
	{
		// Binding object is a texture view, so we must be certain that one of the sub-atlases can
		// bind the view.
		for(INT32 i = 0; i < m_subAtlases.Count(); ++i)
		{
			if(m_subAtlases[i].m_subAtlas->CanBindIntoAtlas(bindingObject))
			{
				return TRUE;
			}
		}
	}
	else if(bindingObject->GetType().IsSubClassOf(IRuTexture::Type()))
	{
		// Basically we can bind any texture into the atlas as a discrete texture, so this will always return TRUE
		return TRUE;
	}

	return FALSE;
}

INT32 CRuTextureAtlas_Aggregate::BindIntoAtlas(IRuObject *bindingObject, BOOL atlasBindingRequired)
{
	INT32 textureHandle = -1;
	INT32 subHandle = -1;

	// Attempt to locate the texture
	if(m_textureQueue->ContainsKey(bindingObject->GetGUID(), bindingObject->GetGUID().HashCode()))
	{
		// Get the texture handle
		m_textureQueue->Get(bindingObject->GetGUID(), bindingObject->GetGUID().HashCode(), textureHandle);
	}

	// If the binding object is not found in the binding queue, create a new entry for it
	if(textureHandle < 0)
	{
		// Get a free texture handle
		textureHandle = m_unusedHandles.IsEmpty() ? m_boundTextures.Count() : m_unusedHandles.Pop();

		if(textureHandle >= m_boundTextures.Count())
		{
			m_boundTextures.Add(TextureDesc());
		}

		// Save eviction marker
		m_boundTextures[textureHandle].m_evictionMarker = m_evictionMarker;

		// Store reference to texture
		m_boundTextures[textureHandle].m_bindingObject = bindingObject;
		m_boundTextures[textureHandle].m_bindingObject->AddRef();

		// Set texture status to unbound
		m_boundTextures[textureHandle].m_atlas = NULL;
		m_boundTextures[textureHandle].m_handle = -1;

		// If binding object is a texture, save its width & height
		if(bindingObject->GetType().IsSubClassOf(IRuTexture::Type()))
		{
			IRuTexture *texture = static_cast<IRuTexture *>(bindingObject);

			m_boundTextures[textureHandle].m_width = texture->GetLevelWidth(0);
			m_boundTextures[textureHandle].m_height = texture->GetLevelHeight(0);
		}
		else
		{
			m_boundTextures[textureHandle].m_width = 1;
			m_boundTextures[textureHandle].m_height = 1;
		}

		// Store into binding queue
		m_textureQueue->Add(bindingObject->GetGUID(), bindingObject->GetGUID().HashCode(), textureHandle, 1);
	}
	else
	{
		// Update eviction marker
		m_boundTextures[textureHandle].m_evictionMarker = m_evictionMarker;
	}

	// If the binding object is not bound to an atlas, attempt to bind it
	if(m_boundTextures[textureHandle].m_atlas == NULL)
	{
		// First pass - attempt to bind into an atlas without reclaiming
		for(INT32 i = 0; i < m_subAtlases.Count(); ++i)
		{
			if(m_subAtlases[i].m_subAtlas->CanBindIntoAtlas(bindingObject))
			{
				subHandle = m_subAtlases[i].m_subAtlas->BindIntoAtlas(bindingObject, atlasBindingRequired);

				if(subHandle >= 0)
				{
					// Store pointer to atlas and sub handle
					m_boundTextures[textureHandle].m_atlas = m_subAtlases[i].m_subAtlas;
					m_boundTextures[textureHandle].m_handle = subHandle;

					// Store into sub atlas bind queue
					m_subAtlases[i].m_bindQueue->Add(bindingObject->GetGUID(), bindingObject->GetGUID().HashCode(), textureHandle, 1);

					break;
				}
			}
		}
	}

	// If the binding object is not bound to an atlas, attempt to bind it
	if(m_boundTextures[textureHandle].m_atlas == NULL)
	{
		// Second pass
		for(INT32 i = 0; i < m_subAtlases.Count(); ++i)
		{
			if(m_subAtlases[i].m_subAtlas->CanBindIntoAtlas(bindingObject))
			{
				while(subHandle < 0)
				{
					// Perform a reclaim first
					INT32 nextReclaimHandle;
					if(m_subAtlases[i].m_bindQueue->PeekNextReclaimTarget(nextReclaimHandle))
					{
						// If eviction marker is the same as the current eviction marker, do not do any evictions
						if(m_boundTextures[nextReclaimHandle].m_evictionMarker == m_evictionMarker)
						{
							break;
						}

						// Perform actual eviction
						INT32 freedHandle;
						if(m_subAtlases[i].m_bindQueue->ReclaimUnconditional(freedHandle))
						{
							// Unbind from the atlas
							if(m_boundTextures[freedHandle].m_atlas)
							{
								m_boundTextures[freedHandle].m_atlas->UnbindFromAtlas(m_boundTextures[freedHandle].m_handle);
							}

							// Reset freed entry
							m_boundTextures[freedHandle].m_atlas = NULL;
							m_boundTextures[freedHandle].m_handle = -1;
						}
					}
					else
					{
						// Nothing is reclaimable, quit (this should never happen)
						break;
					}

					subHandle = m_subAtlases[i].m_subAtlas->BindIntoAtlas(bindingObject, atlasBindingRequired);

					if(subHandle >= 0)
					{
						m_boundTextures[textureHandle].m_atlas = m_subAtlases[i].m_subAtlas;
						m_boundTextures[textureHandle].m_handle = subHandle;

						// Store into sub atlas bind queue
						m_subAtlases[i].m_bindQueue->Add(bindingObject->GetGUID(), bindingObject->GetGUID().HashCode(), textureHandle, 1);
					}
				}

				// If we have successfully bound, break from loop
				if(subHandle != -1)
					break;
			}
		}

		// If the texture was never bound into an atlas and the binding is required, recycle the allocated texture descriptor
		if(subHandle == -1 && atlasBindingRequired)
		{
			// Recycle texture descriptor
			m_unusedHandles.Push(textureHandle);
			m_boundTextures[textureHandle].m_atlas = NULL;
			m_boundTextures[textureHandle].m_handle = -1;

			// Remove from binding queue
			m_textureQueue->Remove(bindingObject->GetGUID(), bindingObject->GetGUID().HashCode());

			// Reset texture handle
			textureHandle = -1;
		}
	}

	return textureHandle;
}

BOOL CRuTextureAtlas_Aggregate::UnbindFromAtlas(INT32 textureHandle)
{
	return FALSE;
/*
	if(textureHandle >= 0 && textureHandle < m_boundTextures.Count())
	{
		// Release texture
		ruSAFE_RELEASE(m_boundTextures[textureHandle].m_texture);

		// If the texture is bound to an atlas, unbind it
		if(m_boundTextures[textureHandle].m_atlas)
		{
			if(m_boundTextures[textureHandle].m_atlas->UnbindFromAtlas(m_boundTextures[textureHandle].m_handle) == FALSE)
			{
				return FALSE;
			}
		}

		// Recycle texture descriptor
		m_unusedHandles.Push(textureHandle);
		m_boundTextures[textureHandle].m_texture = NULL;
		m_boundTextures[textureHandle].m_atlas = NULL;
		m_boundTextures[textureHandle].m_handle = -1;

		return TRUE;
	}
*/
	return FALSE;
}

BOOL CRuTextureAtlas_Aggregate::UnbindAllFromAtlas()
{
	// Unbind all textures from all sub atlases
	for(INT32 i = 0; i < m_subAtlases.Count(); ++i)
	{
		m_subAtlases[i].m_subAtlas->UnbindAllFromAtlas();
		m_subAtlases[i].m_bindQueue->Clear();
	}

	// Release all binding objects and push texture descriptor handles onto the unused stack
	for(INT32 i = 0; i < m_boundTextures.Count(); ++i)
	{
		ruSAFE_RELEASE(m_boundTextures[i].m_bindingObject);
		m_boundTextures[i].m_atlas = NULL;
		m_boundTextures[i].m_handle = -1;

		m_unusedHandles.Push(i);
	}

	// Clear the texture queue
	FlushTextureQueue();

	return TRUE;
}

BOOL CRuTextureAtlas_Aggregate::TranslateUV(INT32 textureHandle, REAL u, REAL v, REAL uBias, REAL vBias, REAL texelOffset, REAL &uOut, REAL &vOut)
{
	if(textureHandle >= 0 && textureHandle < m_boundTextures.Count())
	{
		if(m_boundTextures[textureHandle].m_atlas)
		{
			return m_boundTextures[textureHandle].m_atlas->TranslateUV(m_boundTextures[textureHandle].m_handle, u, v, uBias, vBias, texelOffset, uOut, vOut);
		}
		else
		{
			if(m_boundTextures[textureHandle].m_bindingObject->GetType().IsSubClassOf(IRuTexture::Type()))
			{
				IRuTexture *texture = static_cast<IRuTexture *>(m_boundTextures[textureHandle].m_bindingObject);

				REAL texelOffsetX = texelOffset / texture->GetLevelWidth(0);
				REAL texelOffsetY = texelOffset / texture->GetLevelHeight(0);

//				REAL texelOffsetX = texelOffset / m_boundTextures[textureHandle].m_width;
//				REAL texelOffsetY = texelOffset / m_boundTextures[textureHandle].m_height;

				uOut = u + texelOffsetX * uBias;
				vOut = v + texelOffsetY * vBias;
			}
			else
			{
				uOut = u;
				vOut = v;
			}

			return TRUE;
		}
	}

	return FALSE;
}

void CRuTextureAtlas_Aggregate::FlushTextureQueue()
{
	m_textureQueue->Clear();
}

// ************************************************************************************************************************************************************

#pragma managed
