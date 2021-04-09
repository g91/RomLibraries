#include "../../Scene/Paperdoll/RuEntityPaperdoll.h"

#include "../../Engine/Geometry/RuMesh_SkinTools.h"

#include "../../Helper/RuHelper_Entity.h"
#include "../../Helper/RuHelper_IRuEntity_Renderable.h"
#include "../../Engine/Texture/RuTexture_Proxy.h"
#include "../../ImageFX/RuImageFX_Blitter.h"

#include "../../Scene/Base/RuHierarchyCoupler.h"

#pragma managed(push, off)

#include <luabind/luabind.hpp>

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuPaperdollTemplate, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuPaperdollTemplate, "CRuPaperdollTemplate", "IRuObject")
ruCLASSGUID_DEFINE(CRuPaperdollTemplate, 0xA3335E93, 0xD8E34940, 0xAD79A30D, 0x9906E1FC)

ruRTTI_DEFINE_SUBCLASS(IRuPaperdoll, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(IRuPaperdoll, "IRuPaperdoll", "CRuEntity")
ruCLASSGUID_DEFINE(IRuPaperdoll, 0x1143ABA3, 0xC27E4d32, 0xBBCC7D1C, 0xB965147E)

ruRTTI_DEFINE_SUBCLASS(CRuPaperdoll_Placeholder, IRuPaperdoll)
ruHEAP_DEFINE_SUBCLASS(CRuPaperdoll_Placeholder, "CRuPaperdoll_Placeholder", "IRuPaperdoll")
ruCLASSGUID_DEFINE(CRuPaperdoll_Placeholder, 0xDB0FBCCC, 0x73894286, 0xB2763B93, 0x0E2B158E)

ruRTTI_DEFINE_SUBCLASS(CRuPaperdoll, IRuPaperdoll)
ruHEAP_DEFINE_SUBCLASS(CRuPaperdoll, "CRuPaperdoll", "IRuPaperdoll")
ruCLASSGUID_DEFINE(CRuPaperdoll, 0xA098CF24, 0x5A064ee4, 0xB709479D, 0xF2780E94)

// ************************************************************************************************************************************************************

CRuPaperdollTemplate::FindItemQuery::FindItemQuery()
:	m_maxMipLevel(0),
	m_resultItem(NULL),
	m_modelPath(NULL),
	m_diffuseTexturePath(NULL),
	m_compositeLayerTexturePath(NULL),
	m_glossTexturePath(NULL),
	m_glowTexturePath(NULL)
{
	for(INT32 i = 0; i < 4; ++i)
	{
		m_layerTexturePath[i] = NULL;
	}

	StopQuery();
}

CRuPaperdollTemplate::FindItemQuery::~FindItemQuery()
{
	StopQuery();
}

void CRuPaperdollTemplate::FindItemQuery::StartQuery(INT32 maxMipLevel, INT32 partIndex, const char *itemName, CRuArrayList<const char *> *acceptableItemCodeList, const char *baseItemKeyOverride)
{
	if(m_queryState == 0)
	{
		m_maxMipLevel = maxMipLevel;
		m_partIdx = partIndex;
		m_itemName = itemName;

		for(INT32 i = 0; i < acceptableItemCodeList->Count(); ++i)
		{
			char *codeStr = ruNEW char [strlen(acceptableItemCodeList->Get(i)) + 1];
			strcpy(codeStr, acceptableItemCodeList->Get(i));
			m_acceptableItemCodeList.Add(codeStr);
		}

//		m_acceptableItemCodeList.CloneFrom(*acceptableItemCodeList);
		m_baseItemKeyOverride = baseItemKeyOverride;

		m_queryState = 1;
		m_instanceComplete = FALSE;
		m_itemIsAvailable = FALSE;
		m_baseItemIdx = 0;
		m_acceptableItemCodeIdx = 0;
		m_validItemCodeIdx = 0;

		ruSAFE_RELEASE(m_resultItem);
		m_baseItemKeyOut = "";
		m_itemCodeOut = "";
		m_itemAuxCodeOut = "";
	}
}

void CRuPaperdollTemplate::FindItemQuery::StopQuery()
{
	m_maxMipLevel = 0;
	m_partIdx = 0;
	m_itemName = NULL;

	for(INT32 i = 0; i < m_acceptableItemCodeList.Count(); ++i)
	{
		delete[] m_acceptableItemCodeList[i];
	}

	m_acceptableItemCodeList.Clear();
	m_baseItemKeyOverride = NULL;

	m_queryState = 0;
	m_baseItemIdx = 0;
	m_acceptableItemCodeIdx = 0;
	m_validItemCodeIdx = 0;

	ruSAFE_RELEASE(m_resultItem);
	m_baseItemKeyOut = "";
	m_itemCodeOut = "";
	m_itemAuxCodeOut = "";

	delete[] m_modelPath;
	m_modelPath = NULL;

	delete[] m_diffuseTexturePath;
	m_diffuseTexturePath = NULL;

	for(INT32 i = 0; i < 4; ++i)
	{
		delete[] m_layerTexturePath[i];
		m_layerTexturePath[i] = NULL;
	}
	delete[] m_compositeLayerTexturePath;
	m_compositeLayerTexturePath = NULL;

	delete[] m_glossTexturePath;
	m_glossTexturePath = NULL;

	delete[] m_glowTexturePath;
	m_glowTexturePath = NULL;
}

BOOL CRuPaperdollTemplate::FindItemQuery::InstanceItem(BOOL enableDelayedLoad)
{
	// Retrieve the resource
	BOOL loadStatus = FALSE;

	if(enableDelayedLoad)
	{
		loadStatus = m_resultItem ? FALSE : g_ruResourceManager->LoadEntity(m_modelPath, &m_resultItem);
	}
	else
	{
		m_resultItem = g_ruResourceManager->LoadEntity(m_modelPath);
	}

	if(m_resultItem)
	{
		BOOL waitingForTexture = TRUE;
		BOOL colorMasksReady = TRUE;

		// Apply diffuse map to item
		ApplyTextureToItem(m_resultItem, ruTEXCHANNEL_DIFFUSEMAP, m_diffuseTexturePath, ruSURFACEFMT_DEFAULT, m_maxMipLevel, enableDelayedLoad);

		// Apply gloss map to item
		ApplyTextureToItem(m_resultItem, ruTEXCHANNEL_GLOSSMAP, m_glossTexturePath, ruSURFACEFMT_DEFAULT, m_maxMipLevel, enableDelayedLoad);

		// Apply glow map to item
		ApplyTextureToItem(m_resultItem, ruTEXCHANNEL_GLOWMAP, m_glowTexturePath, ruSURFACEFMT_DEFAULT, m_maxMipLevel, enableDelayedLoad);

		// Apply color layer masks to item
		for(INT32 layer = 1; layer <= 4; ++layer)
		{
			// Calculate current channel
			RuTextureChannel texChannel = static_cast<RuTextureChannel>(ruTEXCHANNEL_PAPERDOLL_MASK_0 + layer - 1);

			// Apply color layer mask to item
			ApplyTextureToItem(m_resultItem, texChannel, m_layerTexturePath[layer - 1], ruSURFACEFMT_A8R8G8B8, m_maxMipLevel, enableDelayedLoad);

			// Determine whether the color mask is loaded and ready
			IRuMaterial *material = GetItemMaterial(m_resultItem);
			IRuBaseTexture *texture = material->GetTexture(texChannel);
			if(texture)
			{
				if(!texture->IsDiscarded() && texture->IsInvalidated())
				{
					colorMasksReady = FALSE;
				}
			}
		}

		if(colorMasksReady)
		{
			IRuMaterial *material = GetItemMaterial(m_resultItem);

			if(material)
			{
				IRuTexture *compLayerMaskTex = g_ruResourceManager->LoadTexture(m_compositeLayerTexturePath);
				if(compLayerMaskTex == NULL)
				{
					INT32 maskTextureWidth = 0;
					INT32 maskTextureHeight = 0;
					INT32 maskTextureLevels = 0;

					for(INT32 layer = 0; layer < 4; ++layer)
					{
						IRuTexture *srcTexture = (IRuTexture *) material->GetTexture(static_cast<RuTextureChannel>(ruTEXCHANNEL_PAPERDOLL_MASK_0 + layer));
						if(srcTexture && !srcTexture->IsDiscarded())
						{
							maskTextureWidth = srcTexture->GetLevelWidth(0);
							maskTextureHeight = srcTexture->GetLevelHeight(0);
							maskTextureLevels = srcTexture->GetLevelCount();
							break;
						}
					}

					if(maskTextureWidth <= 0 || maskTextureHeight <= 0)
					{
						// Create the composite mask texture
						compLayerMaskTex = ruNEW CRuTexture_Proxy();
						static_cast<CRuTexture_Proxy *>(compLayerMaskTex)->Allocate(1, 1, 1, ruSURFACEFMT_A4R4G4B4);
						compLayerMaskTex->SetTextureName(m_compositeLayerTexturePath);
						g_ruResourceManager->AddTexture(compLayerMaskTex);

						UINT16 *destBits;
						INT32 destPitch;

						// Lock destination and source
						compLayerMaskTex->Lock(0, (void **) &destBits, &destPitch);

						// Blit
						RuImageFX_Clear_16(destBits, 0, 0, 1, 1, destPitch);

						// Unlock destination and source
						compLayerMaskTex->Unlock(0);
					}
					else
					{
						// Create the composite mask texture
						compLayerMaskTex = ruNEW CRuTexture_Proxy();
						static_cast<CRuTexture_Proxy *>(compLayerMaskTex)->Allocate(maskTextureWidth, maskTextureHeight, maskTextureLevels, ruSURFACEFMT_A4R4G4B4);
						compLayerMaskTex->SetTextureName(m_compositeLayerTexturePath);
						g_ruResourceManager->AddTexture(compLayerMaskTex);

						for(INT32 layer = 0; layer < 4; ++layer)
						{
							IRuTexture *srcTexture = (IRuTexture *) material->GetTexture(static_cast<RuTextureChannel>(ruTEXCHANNEL_PAPERDOLL_MASK_0 + layer));

							if(srcTexture && !srcTexture->IsDiscarded())
							{
								INT32 srcLevel = 0;
								INT32 destLevel = 0;

								// 檢查來源與目標貼圖需要大小一致
								// 來源比目標貼圖大
								if (srcTexture->GetLevelWidth(0) > compLayerMaskTex->GetLevelWidth(0))
								{
									srcLevel++;
									while (srcTexture->GetLevelWidth(srcLevel) > compLayerMaskTex->GetLevelWidth(0))
										srcLevel++;
								}
								else if (srcTexture->GetLevelWidth(0) < compLayerMaskTex->GetLevelWidth(0))
								{
									destLevel++;
									while (srcTexture->GetLevelWidth(0) < compLayerMaskTex->GetLevelWidth(destLevel))
										destLevel++;
								}

								UINT32 numLevels = min(srcTexture->GetLevelCount() - srcLevel, compLayerMaskTex->GetLevelCount() - destLevel);

								for(UINT32 j = 0; j < numLevels; ++j, ++srcLevel, ++destLevel)
								{
									UINT32 *srcBits;
									UINT16 *destBits;
									INT32 srcPitch, destPitch;

									// Lock destination and source
									srcTexture->Lock(srcLevel, (void **) &srcBits, &srcPitch);
									compLayerMaskTex->Lock(destLevel, (void **) &destBits, &destPitch);

									// Blit
									RuImageFX_Blit_A8R8G8B8ToA4R4G4B4(destBits, 0, 0, destPitch, layer, srcBits, srcTexture->GetLevelWidth(srcLevel), srcTexture->GetLevelHeight(srcLevel), srcPitch, 0);

									// Unlock destination and source
									srcTexture->Unlock(srcLevel);
									compLayerMaskTex->Unlock(destLevel);
								}
							}
						}
					}
				}

				material->SetTexture(ruTEXCHANNEL_PAPERDOLL_MASK_COMPOSITE, compLayerMaskTex);
				ruSAFE_RELEASE(compLayerMaskTex);
			}

			// Mark as no longer waiting for texture
			waitingForTexture = FALSE;
		}

		// Finalize item query state
		if(waitingForTexture == FALSE)
		{
			// Clear layer masks
			IRuMaterial *material = GetItemMaterial(m_resultItem);

			if(material)
			{
				for(INT32 layer = 0; layer < 4; ++layer)
				{
					// Purge the layer texture
					IRuBaseTexture *layerTexture = material->GetTexture(static_cast<RuTextureChannel>(ruTEXCHANNEL_PAPERDOLL_MASK_0 + layer));
					g_ruResourceManager->PurgeTexture(layerTexture);

					// Clear layer texture setting
					material->SetTexture(static_cast<RuTextureChannel>(ruTEXCHANNEL_PAPERDOLL_MASK_0 + layer), NULL);
				}
			}

			// All resources have completed load, terminate instancing
			m_instanceComplete = TRUE;

			return TRUE;
		}
		else
		{
			// Some or all resources are still waiting for load, continue instancing
			m_instanceComplete = FALSE;

			return TRUE;
		}
	}
	else if(loadStatus == FALSE)
	{
		// Model resource was not loaded, terminate instancing
		m_instanceComplete = TRUE;
	}

	return TRUE;
}

BOOL CRuPaperdollTemplate::FindItemQuery::QueryIsComplete()
{
	return m_queryState == 2;
}

BOOL CRuPaperdollTemplate::FindItemQuery::QueryIsRunning()
{
	return m_queryState == 1;
}

BOOL CRuPaperdollTemplate::FindItemQuery::QueryIsStopped()
{
	return m_queryState == 0;
}

BOOL CRuPaperdollTemplate::FindItemQuery::ItemIsInstanced()
{
	return m_instanceComplete;
}

BOOL CRuPaperdollTemplate::FindItemQuery::ApplyTextureToItem(CRuEntity *item, RuTextureChannel channel, const char *textureName, RuSurfaceFormat targetSurfaceFormat, INT32 maxMipLevel, BOOL enableDelayedLoad) const
{
//	IRuEntity_Renderable *renderableEntity = RuEntity_FindFirstRenderableEntity(item);
	const CRuRTTI *allowableAncestorTypes[] = { &CRuFrame::Type(), &CRuACTEntity::Type(), &CRuHierarchy_GR2::Type(), NULL };
	IRuEntity_Renderable *renderableEntity = static_cast<IRuEntity_Renderable *>(RuEntity_FindFirstEntityOfType(item, IRuEntity_Renderable::Type(), allowableAncestorTypes));

	if(renderableEntity)
	{
		IRuMesh *mesh;
		IRuMaterial *material;

		if(static_cast<IRuEntity_Renderable *>(renderableEntity)->GetMesh(0, &mesh, &material))
		{
			if(material)
			{
				IRuTexture *texture = g_ruResourceManager->LoadTexture(textureName, targetSurfaceFormat, maxMipLevel, enableDelayedLoad);

				material->SetTexture(channel, texture);

				ruSAFE_RELEASE(texture);
			}
		}
		else
		{
			OutputDebugString("[PAPERDOLL] Error: Paperdoll mesh is not retrievable.");
		}
	}
	else
	{
		OutputDebugString("[PAPERDOLL] Error: Paperdoll model is not renderable.");
	}

	return TRUE;
}

IRuMaterial *CRuPaperdollTemplate::FindItemQuery::GetItemMaterial(CRuEntity *item) const
{
//	IRuEntity_Renderable *renderableEntity = RuEntity_FindFirstRenderableEntity(item);
	const CRuRTTI *allowableAncestorTypes[] = { &CRuFrame::Type(), &CRuACTEntity::Type(), &CRuHierarchy_GR2::Type(), NULL };
	IRuEntity_Renderable *renderableEntity = static_cast<IRuEntity_Renderable *>(RuEntity_FindFirstEntityOfType(item, IRuEntity_Renderable::Type(), allowableAncestorTypes));

	if(renderableEntity)
	{
		IRuMesh *mesh;
		IRuMaterial *material;

		if(static_cast<IRuEntity_Renderable *>(renderableEntity)->GetMesh(0, &mesh, &material))
		{
			return material;
		}
	}

	return NULL;
}

// ************************************************************************************************************************************************************

CRuPaperdollTemplate::CRuPaperdollTemplate()
:	m_modelTemplate(NULL)
{
	m_resourceNamePrefix = "";
	m_resourceNamePostfix = ".ros";

	m_baseItemResourcePath = "";
	m_itemResourcePath = "";
}

CRuPaperdollTemplate::~CRuPaperdollTemplate()
{
	if(m_modelTemplate)
	{
		m_modelTemplate->Release();
		m_modelTemplate = NULL;
	}

	// Iterate through each part structure and clean them up
	for(INT32 i = 0; i < m_parts.Count(); ++i)
	{
		Part *curPart = m_parts[i];

		// Enumerate items from the tree, delete them all
		INT32 numItems = curPart->m_baseItems.Count();
		for(INT32 j = 0; j < numItems; ++j)
		{
			// Retrieve the item from the tree
			BaseItem *item = curPart->m_baseItems.GetAtIndex(j);

			// Iterate through each item code
			INT32 numItemCodes = item->m_validBaseItemCodes.Count();
			for(INT32 k = 0; k < numItemCodes; ++k)
			{
				// Delete item code
				delete[] item->m_validBaseItemCodes[k];
			}

			// Iterate through each derived item
			for(INT32 k = 0; k < item->m_items.Count(); ++k)
			{
				delete item->m_items.GetAtIndex(k);
			}

			// Delete item
			delete item;
		}

		// Clear tree
		curPart->m_baseItems.Clear();

		// Delete part
		delete curPart;
	}

	// Clear part array
	m_parts.Clear();

	// Delete resource name buffers
	while(m_unusedResourceNameBuffers.IsEmpty() == FALSE)
	{
		delete[] m_unusedResourceNameBuffers.Pop();
	}
}

BOOL CRuPaperdollTemplate::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Release old data

	// Read length of base item resource path, including NULL
	INT32 baseItemResourcePathLength;
	stream->Read(&baseItemResourcePathLength, sizeof(INT32));

	// Read base item resource path
	char baseItemResourcePath[4096];
	stream->Read(baseItemResourcePath, baseItemResourcePathLength);
	m_baseItemResourcePath = baseItemResourcePath;

	// Read length of item resource path, including NULL
	INT32 itemResourcePathLength;
	stream->Read(&itemResourcePathLength, sizeof(INT32));

	// Read item resource path
	char itemResourcePath[4096];
	stream->Read(itemResourcePath, itemResourcePathLength);
	m_itemResourcePath = itemResourcePath;

	// Serialize a copy of the model template from the stream
	IRuObject *templateObject = RuObjectFactory().CreateObject(stream);
	if(templateObject->GetType() == CRuCompositeModelTemplate::Type())
		m_modelTemplate = static_cast<CRuCompositeModelTemplate *>(templateObject);
	else
	{
		templateObject->Release();
		return FALSE;
	}

	// Read number of parts
	INT32 numParts;
	stream->Read(&numParts, sizeof(INT32));

	// Iterate through each part
	for(INT32 i = 0; i < numParts; ++i)
	{
		// Create part structure
		Part *part = ruNEW Part();

		// Read part ID
		stream->Read(&part->m_partID, sizeof(INT32));

		// Read number of items for this part
		INT32 numItems;
		stream->Read(&numItems, sizeof(INT32));

		// Iterate through each item
		for(INT32 j = 0; j < numItems; ++j)
		{
			// Create item structure
			BaseItem *item = ruNEW BaseItem();

			// Read length of the item name including null
			INT32 itemNameLength;
			stream->Read(&itemNameLength, sizeof(INT32));

			// Read the item name
			char itemName[4096];
			stream->Read(itemName, itemNameLength);

			// Read length of the item prefix string including null
			INT32 itemPrefixLength;
			stream->Read(&itemPrefixLength, sizeof(INT32));

			// Read the item prefix string
			char itemPrefix[4096];
			stream->Read(itemPrefix, itemPrefixLength);
			item->m_baseItemPrefix = itemPrefix;

			// Read number of item codes for this item
			INT32 numItemCodes;
			stream->Read(&numItemCodes, sizeof(INT32));

			// Iterate through each item code
			for(INT32 k = 0; k < numItemCodes; ++k)
			{
				// Read length of the item code string including NULL
				INT32 itemCodeLength;
				stream->Read(&itemCodeLength, sizeof(INT32));

				// Read the item code string
				char *itemCode = ruNEW char [itemCodeLength];
				stream->Read(itemCode, itemCodeLength);

				// Add item code to the list of valid item codes
				item->m_validBaseItemCodes.Add(itemCode);
			}

			// Read length of the item prefix string including null
			INT32 itemAuxCodeLength;
			stream->Read(&itemAuxCodeLength, sizeof(INT32));

			// Read the item prefix string
			char itemAuxCode[4096];
			stream->Read(itemAuxCode, itemAuxCodeLength);
			item->m_baseItemAuxCode = itemAuxCode;

			// Read number of derived items
			INT32 numDerivedItems;
			stream->Read(&numDerivedItems, sizeof(INT32));

			// Iterate through each derived item
			for(INT32 k = 0; k < numDerivedItems; ++k)
			{
				Item *derivedItem = ruNEW Item();

				// Read length of the derived item name including null
				INT32 derivedItemNameLength;
				stream->Read(&derivedItemNameLength, sizeof(INT32));

				// Read the item name
				char derivedItemName[4096];
				stream->Read(derivedItemName, derivedItemNameLength);

				// Read length of the item prefix string including null
				INT32 derivedItemPrefixLength;
				stream->Read(&derivedItemPrefixLength, sizeof(INT32));

				// Read the item prefix string
				char derivedItemPrefix[4096];
				stream->Read(derivedItemPrefix, derivedItemPrefixLength);
				derivedItem->m_itemPrefix = derivedItemPrefix;

				// Read number of valid color codes
				INT32 numValidColorCodes;
				stream->Read(&numValidColorCodes, sizeof(INT32));

				// Iterate through each color code
				ColorCode colorCode;
				for(INT32 l = 0; l < numValidColorCodes; ++l)
				{
					// Read color codes
					stream->Read(&colorCode.m_color0Code, sizeof(INT32));
					stream->Read(&colorCode.m_color1Code, sizeof(INT32));

					// Add color code
					derivedItem->m_validColorCodes.Add(colorCode);
				}

				item->m_items.Add(derivedItemName, derivedItem);
			}

			// Add item to the tree
			CRuString key(itemName);
			part->m_baseItems.Add(key, item);
		}

		// Balance the items contained by the part
		part->m_baseItems.BalanceTree();

		// Add part to the array
		m_parts.Add(part);
	}

	return TRUE;
}

BOOL CRuPaperdollTemplate::SerializeTo(IRuStream *stream)
{
	// Make sure we have an assigned model template
	if(!m_modelTemplate)
		return FALSE;

	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write length of base item resource path, including NULL
	INT32 baseItemResourcePathLength = m_baseItemResourcePath.GetLength() + 1;
	stream->Write(&baseItemResourcePathLength, sizeof(INT32));

	// Write base item resource path
	stream->Write((char *) m_baseItemResourcePath, baseItemResourcePathLength);

	// Write length of item resource path, including NULL
	INT32 itemResourcePathLength = m_itemResourcePath.GetLength() + 1;
	stream->Write(&itemResourcePathLength, sizeof(INT32));

	// Write item resource path
	stream->Write((char *) m_itemResourcePath, itemResourcePathLength);

	// Serialize a copy of the model template
	m_modelTemplate->SerializeTo(stream);

	// Write number of parts
	INT32 numParts = m_parts.Count();
	stream->Write(&numParts, sizeof(INT32));

	// Iterate through each part
	for(INT32 i = 0; i < numParts; ++i)
	{
		// Write part ID
		stream->Write(&m_parts[i]->m_partID, sizeof(INT32));

		// Write number of items for this part
		INT32 numItems = m_parts[i]->m_baseItems.Count();
		stream->Write(&numItems, sizeof(INT32));

		// Iterate through each item
		for(INT32 j = 0; j < numItems; ++j)
		{
			// Retrieve the item from the tree
			BaseItem *item = m_parts[i]->m_baseItems.GetAtIndex(j);

			// Write length of the item name including null
			const CRuString &itemName = m_parts[i]->m_baseItems.GetKeyAtIndex(j);
			INT32 itemNameLength = itemName.GetLength() + 1;
			stream->Write(&itemNameLength, sizeof(INT32));

			// Write the item name
			stream->Write((const char *) itemName, itemNameLength);

			// Write length of the item prefix string including null
			INT32 itemPrefixLength = item->m_baseItemPrefix.GetLength() + 1;
			stream->Write(&itemPrefixLength, sizeof(INT32));

			// Write the item prefix string
			stream->Write((const char *) item->m_baseItemPrefix, itemPrefixLength);

			// Write number of item codes for this item
			INT32 numItemCodes = item->m_validBaseItemCodes.Count();
			stream->Write(&numItemCodes, sizeof(INT32));

			// Iterate through each item code
			for(INT32 k = 0; k < numItemCodes; ++k)
			{
				// Write length of the item code string including NULL
				INT32 nameLength = (INT32)strlen(item->m_validBaseItemCodes[k]) + 1;
				stream->Write(&nameLength, sizeof(INT32));

				// Write the item code string
				stream->Write((char *) item->m_validBaseItemCodes[k], nameLength);
			}

			// Write length of the item auxiliary code including null
			INT32 itemAuxCodeLength = item->m_baseItemAuxCode.GetLength() + 1;
			stream->Write(&itemAuxCodeLength, sizeof(INT32));

			// Write the item auxiliary code
			stream->Write((const char *) item->m_baseItemAuxCode, itemAuxCodeLength);

			// Write number of derived items
			INT32 numDerivedItems = item->m_items.Count();
			stream->Write(&numDerivedItems, sizeof(INT32));

			// Iterate through each derived item
			for(INT32 k = 0; k < numDerivedItems; ++k)
			{
				Item *derivedItem = item->m_items.GetAtIndex(k);

				// Write length of the derived item name including null
				const CRuString &derivedItemName = item->m_items.GetKeyAtIndex(k);
				INT32 derivedItemNameLength = derivedItemName.GetLength() + 1;
				stream->Write(&derivedItemNameLength, sizeof(INT32));

				// Write the derived item name
				stream->Write((const char *) derivedItemName, derivedItemNameLength);

				// Write length of the item prefix string including null
				INT32 derivedItemPrefixLength = derivedItem->m_itemPrefix.GetLength() + 1;
				stream->Write(&derivedItemPrefixLength, sizeof(INT32));

				// Write the item prefix string
				stream->Write((const char *) derivedItem->m_itemPrefix, derivedItemPrefixLength);

				// Write number of valid color codes
				INT32 numValidColorCodes = derivedItem->m_validColorCodes.Count();
				stream->Write(&numValidColorCodes, sizeof(INT32));

				// Iterate through each color code
				for(INT32 l = 0; l < numValidColorCodes; ++l)
				{
					// Write color codes
					stream->Write(&derivedItem->m_validColorCodes[l].m_color0Code, sizeof(INT32));
					stream->Write(&derivedItem->m_validColorCodes[l].m_color1Code, sizeof(INT32));
				}
			}
		}
	}

	return TRUE;
}

BOOL CRuPaperdollTemplate::Initialize(CRuCompositeModelTemplate *modelTemplate)
{
	// Release here...

	// Store model template and increment its reference
	m_modelTemplate = modelTemplate;
	m_modelTemplate->AddRef();

	// Setup part arrays
	INT32 numParts = m_modelTemplate->GetNumParts();
	for(INT32 i = 0; i < numParts; ++i)
	{
		// Add new part structure and setup its part ID
		m_parts.Add(ruNEW Part());
		m_parts[i]->m_partID = m_modelTemplate->GetPartID(i);
	}

	return TRUE;
}

const char *CRuPaperdollTemplate::GetBaseItemResourcePath() const
{
	return m_baseItemResourcePath;
}

void CRuPaperdollTemplate::SetBaseItemResourcePath(const char *baseItemResourcePath)
{
	m_baseItemResourcePath = baseItemResourcePath;
}

const char *CRuPaperdollTemplate::GetItemResourcePath() const
{
	return m_itemResourcePath;
}

void CRuPaperdollTemplate::SetItemResourcePath(const char *itemResourcePath)
{
	m_itemResourcePath = itemResourcePath;
}

BOOL CRuPaperdollTemplate::AddBaseItem(INT32 partID, const char *baseItemName, const char *baseItemCode, const char *baseItemAuxCode, const char *baseItemPrefix)
{
	// Make sure we have an assigned model template
	if(!m_modelTemplate)
		return FALSE;

	// Locate matching part index for the specified part ID
	INT32 partIndex = m_modelTemplate->GetPartIndex(partID);
	if(partIndex < 0)
		return FALSE;

	// Locate the item structure for the specified base item
	CRuString ruBaseItemName(baseItemName);
	BaseItem *baseItem = NULL;

	if(m_parts[partIndex]->m_baseItems.ContainsKey(ruBaseItemName))
	{
		// Retrieve the base item from the tree
		baseItem = m_parts[partIndex]->m_baseItems.Get(ruBaseItemName);
	}
	else
	{
		// The base item has not been created, so create a new item structure and add it to the tree
		baseItem = ruNEW BaseItem();
		m_parts[partIndex]->m_baseItems.Add(ruBaseItemName, baseItem);
	}

	// Set the base item prefix
	baseItem->m_baseItemPrefix = baseItemPrefix;

	// Make sure the base item code is not already in the template
	for(INT32 i = 0; i < baseItem->m_validBaseItemCodes.Count(); ++i)
	{
		if(strcmpi(baseItem->m_validBaseItemCodes[i], baseItemCode) == 0)
			return TRUE;
	}

	// Make a copy of the base item code
	INT32 baseItemCodeLength = (INT32)strlen(baseItemCode) + 1;
	char *newBaseItemCode = ruNEW char [baseItemCodeLength];
	memcpy(newBaseItemCode, baseItemCode, baseItemCodeLength);

	// Add new item code to the list of valid item codes
	baseItem->m_validBaseItemCodes.Add(newBaseItemCode);

	// If there is a base item auxiliary code specified, set it now
	if(baseItemAuxCode && strlen(baseItemAuxCode) > 0)
	{
		baseItem->m_baseItemAuxCode = baseItemAuxCode;
	}

	return TRUE;
}

BOOL CRuPaperdollTemplate::AddItem(const char *partName, const char *baseItemName, const char *itemName, const char *itemPrefix, INT32 color0Code, INT32 color1Code)
{
	// Find the part index
	INT32 partIndex = GetPartIndex(partName);

	if(partIndex >= 0)
	{
		// Locate the item structure for the specified base item
		CRuString ruBaseItemName(baseItemName);
		CRuString ruItemName(itemName);
		BaseItem *baseItem = NULL;
		Item *item = NULL;

		if(m_parts[partIndex]->m_baseItems.ContainsKey(ruBaseItemName))
		{
			// Retrieve the base item from the tree
			baseItem = m_parts[partIndex]->m_baseItems.Get(ruBaseItemName);

			if(baseItem->m_items.ContainsKey(ruItemName))
			{
				// Retrieve the derived item from the tree
				item = baseItem->m_items.Get(ruItemName);
			}
			else
			{
				// The item does not exist, so create a new entry for it
				item = ruNEW Item();
				baseItem->m_items.Add(ruItemName, item);
			}

			if(item)
			{
				// Set item prefix (the prefix is the resource name up to but not including the backslash and filename)
				item->m_itemPrefix = itemPrefix;

				// Make sure the specified color code combination does not exist
				for(INT32 i = 0; i < item->m_validColorCodes.Count(); ++i)
				{
					if(item->m_validColorCodes[i].m_color0Code == color0Code && item->m_validColorCodes[i].m_color1Code == color1Code)
					{
						return TRUE;
					}
				}

				// Add the specified color code combination
				ColorCode colorCode;
				colorCode.m_color0Code = color0Code;
				colorCode.m_color1Code = color1Code;
				item->m_validColorCodes.Add(colorCode);

				return TRUE;
			}
		}
	}

	return FALSE;
}

INT32 CRuPaperdollTemplate::GetNumParts() const
{
	// Make sure we have an assigned model template
	if(!m_modelTemplate)
		return 0;

	return m_parts.Count();
}

INT32 CRuPaperdollTemplate::GetPartIndex(INT32 partID) const
{
	// Make sure we have an assigned model template
	if(!m_modelTemplate)
		return -1;

	return m_modelTemplate->GetPartIndex(partID);
}

INT32 CRuPaperdollTemplate::GetPartIndex(const char *partName) const
{
	// Make sure we have an assigned model template
	if(!m_modelTemplate)
		return -1;

	for(INT32 i = 0; i < m_modelTemplate->GetNumParts(); ++i)
	{
		if(stricmp(m_modelTemplate->GetPartName(i), partName) == 0)
			return i;
	}

	return -1;
}

INT32 CRuPaperdollTemplate::GetPartID(INT32 partIndex) const
{
	// Make sure we have an assigned model template
	if(!m_modelTemplate)
		return -1;

	return m_modelTemplate->GetPartID(partIndex);
}

INT32 CRuPaperdollTemplate::GetPartID(const char *partName) const
{
	// Make sure we have an assigned model template
	if(!m_modelTemplate)
		return -1;

	for(INT32 i = 0; i < m_modelTemplate->GetNumParts(); ++i)
	{
		if(stricmp(m_modelTemplate->GetPartName(i), partName) == 0)
			return m_modelTemplate->GetPartID(i);
	}

	return -1;
}

const char *CRuPaperdollTemplate::GetPartName(INT32 partIndex) const
{
	// Make sure we have an assigned model template
	if(!m_modelTemplate)
		return "";

	return m_modelTemplate->GetPartName(partIndex);
}

INT32 CRuPaperdollTemplate::GetNumBaseItems(INT32 partIndex) const
{
	// Make sure we have an assigned model template
	if(!m_modelTemplate)
		return 0;

	// Return the number of items in the tree
	return m_parts[partIndex]->m_baseItems.Count();
}

const char *CRuPaperdollTemplate::GetBaseItemName(INT32 partIndex, INT32 baseItemIndex) const
{
	return m_parts[partIndex]->m_baseItems.GetKeyAtIndex(baseItemIndex);
}

const char *CRuPaperdollTemplate::GetBaseItemPrefix(INT32 partIndex, INT32 baseItemIndex) const
{
	BaseItem *item = m_parts[partIndex]->m_baseItems.GetAtIndex(baseItemIndex);

	return item->m_baseItemPrefix;
}

char *CRuPaperdollTemplate::GetBaseItemCodesAsString(INT32 partIndex, INT32 baseItemIndex) const
{
	BaseItem *item = m_parts[partIndex]->m_baseItems.GetAtIndex(baseItemIndex);

	CRuString ruItemCodeString;

	// Iterate through all acceptable item codes
	for(INT32 i = 0; i < item->m_validBaseItemCodes.Count(); ++i)
	{
		ruItemCodeString.Append(item->m_validBaseItemCodes[i]);
		ruItemCodeString.Append(" ");
	}

	char *itemCodeString = ruNEW char [ruItemCodeString.GetLength() + 1];
	memcpy(itemCodeString, (char *) ruItemCodeString, ruItemCodeString.GetLength() + 1);

	return itemCodeString;
}

INT32 CRuPaperdollTemplate::GetNumItems(INT32 partIndex, INT32 baseItemIndex) const
{
	// Return the number of items in the base item
	return m_parts[partIndex]->m_baseItems.GetAtIndex(baseItemIndex)->m_items.Count();
}

const char *CRuPaperdollTemplate::GetItemName(INT32 partIndex, INT32 baseItemIndex, INT32 itemIndex) const
{
	return m_parts[partIndex]->m_baseItems.GetAtIndex(baseItemIndex)->m_items.GetKeyAtIndex(itemIndex);
}

char *CRuPaperdollTemplate::GetItemColorCodesAsString(INT32 partIndex, INT32 baseItemIndex, INT32 itemIndex) const
{
	BaseItem *baseItem = m_parts[partIndex]->m_baseItems.GetAtIndex(baseItemIndex);
	Item* item = baseItem->m_items.GetAtIndex(itemIndex);

	CRuString ruColorCodeString;

	// Iterate through all acceptable item codes
	char tempStr[128];
	for(INT32 i = 0; i < item->m_validColorCodes.Count(); ++i)
	{
		sprintf(tempStr, "s%dh%d", item->m_validColorCodes[i].m_color0Code, item->m_validColorCodes[i].m_color1Code);
		ruColorCodeString.Append(tempStr);
		ruColorCodeString.Append(" ");
	}

	char *colorCodeString = ruNEW char [ruColorCodeString.GetLength() + 1];
	memcpy(colorCodeString, (char *) ruColorCodeString, ruColorCodeString.GetLength() + 1);

	return colorCodeString;
}

CRuPaperdoll *CRuPaperdollTemplate::Construct()
{
	CRuPaperdoll *paperdoll = ruNEW CRuPaperdoll();

	// Retrieve assembly rule from the composite model template
	const char *assemblyRule = m_modelTemplate->GetAssemblyRule();
	if(strlen(assemblyRule) > 0)
	{
		// Initialize our function state
		if(luaL_dostring(paperdoll->m_luaState, assemblyRule))
		{
			MessageBox(NULL, "Error initializing custom assembly rule", "ERROR", MB_OK | MB_ICONSTOP);
		}
	}
	else
	{
		InitializeDefaultAssemblyRule(paperdoll->m_luaState);
	}

	// Set template into paperdoll object and increment reference on self
	paperdoll->m_paperdollTemplate = this;
	paperdoll->SetResourceName(this->GetResourceName());
	this->AddRef();

	// Create a new composite model object
	paperdoll->m_paperdollModel = m_modelTemplate->Construct();
	paperdoll->AddChild(paperdoll->m_paperdollModel);

	return paperdoll;
}

char *CRuPaperdollTemplate::CreateResourceNameBuffer()
{
	return ruNEW char [512];
//	return m_unusedResourceNameBuffers.IsEmpty() ? ruNEW char [512] : m_unusedResourceNameBuffers.Pop();
}

void CRuPaperdollTemplate::DestroyResourceNameBuffer(char *buffer)
{
	delete[] buffer;
//	m_unusedResourceNameBuffers.Push(buffer);
}

char *CRuPaperdollTemplate::BuildModelResourceName(const char *prefix, const char *baseItemPrefix, const char *partName, const char *baseItemKey, const char *itemCode0, const char *itemCode1, const char *postfix)
{
	// Build model resource name
	INT32 requiredStringLength =	(INT32)(strlen(prefix) +
									strlen(postfix) +
									strlen(baseItemPrefix) +
									strlen(partName) +
									strlen(baseItemKey) +
									strlen(itemCode0) +
									strlen(itemCode1) + 1 +
									3 + 1);

	char *resourceName = CreateResourceNameBuffer();
	resourceName[0] = NULL;

	strcat(resourceName, prefix);									// Optional resource name prefix (not used at the moment)
	strcat(resourceName, baseItemPrefix);							// Item prefix, e.g. hm_female
	strcat(resourceName, "_");
	strcat(resourceName, partName);									// Part name, e.g. torso, leg
	strcat(resourceName, "_");
	strcat(resourceName, baseItemKey);
	strcat(resourceName, "_");
	strcat(resourceName, itemCode0);								// Item codes, e.g. a0, a1, b0, b1. Defines model assembly rules.
	if(strlen(itemCode1) > 0)										// Optional auxiliary item code, e.g. blank or x
	{
		strcat(resourceName, "_");
		strcat(resourceName, itemCode1);
	}
	strcat(resourceName, postfix);									// Optional resource name postfix (used for file extension .ros)

	return resourceName;
}

char *CRuPaperdollTemplate::BuildBaseTextureResourceName(const char *prefix, const char *partName, const char *baseItemKey,	const char *itemKey)
{
	// Build diffuse texture resource name
	INT32 requiredStringLength =	(INT32)(strlen(prefix) +
									strlen(partName) +
									strlen(baseItemKey) +
									strlen(itemKey) + 3 + 1);


	char *texResourceName = CreateResourceNameBuffer();
	texResourceName[0] = NULL;

	strcat(texResourceName, prefix);
	strcat(texResourceName, "_");
	strcat(texResourceName, partName);
	strcat(texResourceName, "_");
	strcat(texResourceName, baseItemKey);
	strcat(texResourceName, "_");
	strcat(texResourceName, itemKey);

	return texResourceName;
}

char *CRuPaperdollTemplate::BuildLayerMaskResourceName(const char *baseTextureResourceName, INT32 layerIdx)
{
	// Build layer mask texture resource name
	INT32 requiredStringLength =	(INT32)strlen(baseTextureResourceName) +
									7 + 1;


	char *texResourceName = CreateResourceNameBuffer();
	texResourceName[0] = NULL;

	strcat(texResourceName, baseTextureResourceName);
	strcat(texResourceName, "_layer");
	texResourceName[requiredStringLength - 2] = '1' + layerIdx - 1;
	texResourceName[requiredStringLength - 1] = 0;

	return texResourceName;
}

char *CRuPaperdollTemplate::BuildCompositeLayerMaskResourceName(const char *baseTextureResourceName)
{
	// Build layer mask texture resource name
	INT32 requiredStringLength =	(INT32)strlen(baseTextureResourceName) +
									(INT32)strlen("_compmask") + 1;


	char *texResourceName = CreateResourceNameBuffer();
	texResourceName[0] = NULL;

	strcat(texResourceName, baseTextureResourceName);
	strcat(texResourceName, "_compmask");
	texResourceName[requiredStringLength - 1] = 0;

	return texResourceName;
}

char *CRuPaperdollTemplate::BuildGlossTextureResourceName(const char *baseTextureResourceName)
{
	// Build layer mask texture resource name
	INT32 requiredStringLength =	(INT32)strlen(baseTextureResourceName) +
									6 + 1;


	char *texResourceName = CreateResourceNameBuffer();
	texResourceName[0] = NULL;

	strcat(texResourceName, baseTextureResourceName);
	strcat(texResourceName, "_gloss");

	return texResourceName;
}

char *CRuPaperdollTemplate::BuildGlowTextureResourceName(const char *baseTextureResourceName)
{
	// Build layer mask texture resource name
	INT32 requiredStringLength =	(INT32)strlen(baseTextureResourceName) +
									5 + 1;


	char *texResourceName = CreateResourceNameBuffer();
	texResourceName[0] = NULL;

	strcat(texResourceName, baseTextureResourceName);
	strcat(texResourceName, "_glow");

	return texResourceName;
}

BOOL CRuPaperdollTemplate::FindSuitableItem(CRuPaperdollTemplate::FindItemQuery &itemQuery, BOOL enableDelayedLoad)
{
	const char *partName = GetPartName(itemQuery.m_partIdx);
	CRuString itemKey = itemQuery.m_itemName;

	// Iterate through all base items for the specified part index
	for(INT32 i = itemQuery.m_baseItemIdx; i < m_parts[itemQuery.m_partIdx]->m_baseItems.Count(); ++i)
	{
		BaseItem *baseItem = m_parts[itemQuery.m_partIdx]->m_baseItems.GetAtIndex(i);
		const CRuString &baseItemKey = m_parts[itemQuery.m_partIdx]->m_baseItems.GetKeyAtIndex(i);

		// Attempt to locate the specified item name in the base item database
		if(baseItem->m_items.ContainsKey(itemKey))
		{
			Item *item = baseItem->m_items.Get(itemKey);

			// Iterate through all acceptable item codes
			for(INT32 j = itemQuery.m_acceptableItemCodeIdx; j < itemQuery.m_acceptableItemCodeList.Count(); ++j)
			{
				// Determine whether the acceptable item code is a valid item code
				for(INT32 k = itemQuery.m_validItemCodeIdx; k < baseItem->m_validBaseItemCodes.Count(); ++k)
				{
					// Save current running state
					itemQuery.m_queryState = 1;
					itemQuery.m_baseItemIdx = i;
					itemQuery.m_acceptableItemCodeIdx = j;
					itemQuery.m_validItemCodeIdx = k;

					if(strcmpi(itemQuery.m_acceptableItemCodeList[j], baseItem->m_validBaseItemCodes[k]) == 0)
					{
						// Build the model path
						itemQuery.m_modelPath = BuildModelResourceName	(
																			m_resourceNamePrefix,
																			baseItem->m_baseItemPrefix,
																			partName,
																			itemQuery.m_baseItemKeyOverride ? (itemQuery.m_baseItemKeyOverride[0] ? itemQuery.m_baseItemKeyOverride : baseItemKey) : baseItemKey,
																			baseItem->m_validBaseItemCodes[k],
																			baseItem->m_baseItemAuxCode,
																			m_resourceNamePostfix
																		);

						// Build the diffuse texture path
						itemQuery.m_diffuseTexturePath = BuildBaseTextureResourceName(item->m_itemPrefix, partName, baseItemKey, itemKey);

						// Build the layer texture paths
						for(INT32 layer = 1; layer <= 4; ++layer)
						{
							itemQuery.m_layerTexturePath[layer - 1] = BuildLayerMaskResourceName(itemQuery.m_diffuseTexturePath, layer);
						}

						// Build the composite layer texture path
						itemQuery.m_compositeLayerTexturePath = BuildCompositeLayerMaskResourceName(itemQuery.m_diffuseTexturePath);

						// Build the gloss texture path
						itemQuery.m_glossTexturePath = BuildGlossTextureResourceName(itemQuery.m_diffuseTexturePath);

						// Build the glow texture path
						itemQuery.m_glowTexturePath = BuildGlowTextureResourceName(itemQuery.m_diffuseTexturePath);

						// Mark query as complete
						itemQuery.m_queryState = 2;
						itemQuery.m_itemIsAvailable = TRUE;

						// Store query results
						itemQuery.m_baseItemKeyOut = baseItemKey;
						itemQuery.m_itemCodeOut = baseItem->m_validBaseItemCodes[k];
						itemQuery.m_itemAuxCodeOut = baseItem->m_baseItemAuxCode;

						// Terminate the query
						return TRUE;
					}
				}

				// Reset current running state
				itemQuery.m_validItemCodeIdx = 0;
			}

			// Reset current running state
			itemQuery.m_acceptableItemCodeIdx = 0;
		}
	}

	// Mark query as complete
	itemQuery.m_queryState = 2;

	return TRUE;
}

void CRuPaperdollTemplate::InitializeDefaultAssemblyRule(lua_State *luaState)
{
	// Default paperdoll assembly function
	const char *assemblyFunc = 
		"																																		\n\r\
		function lua_AssembleComponents (paperdoll, enableDelayedLoad)																			\n\r\
			paperdollTemplate = paperdoll:GetPaperdollTemplate()																				\n\r\
																																				\n\r\
			handIdx = paperdollTemplate:GetPartIndex(\"hand\")																					\n\r\
			torsoIdx = paperdollTemplate:GetPartIndex(\"torso\")																				\n\r\
			legIdx = paperdollTemplate:GetPartIndex(\"leg\")																					\n\r\
			footIdx = paperdollTemplate:GetPartIndex(\"foot\")																					\n\r\
			beltIdx = paperdollTemplate:GetPartIndex(\"belt\")																					\n\r\
			headIdx = paperdollTemplate:GetPartIndex(\"head\")																					\n\r\
			shoulderIdx = paperdollTemplate:GetPartIndex(\"shoulder\")																			\n\r\
			backIdx = paperdollTemplate:GetPartIndex(\"back\")																					\n\r\
			helmetIdx = paperdollTemplate:GetPartIndex(\"helmet\")																				\n\r\
			hairIdx = paperdollTemplate:GetPartIndex(\"hair\")																					\n\r\
																																				\n\r\
			handPart = paperdoll:GetComponent(\"hand\")																							\n\r\
			torsoPart = paperdoll:GetComponent(\"torso\")																						\n\r\
			legPart = paperdoll:GetComponent(\"leg\")																							\n\r\
			footPart = paperdoll:GetComponent(\"foot\")																							\n\r\
			beltPart = paperdoll:GetComponent(\"belt\")																							\n\r\
			headPart = paperdoll:GetComponent(\"head\")																							\n\r\
			shoulderPart = paperdoll:GetComponent(\"shoulder\")																					\n\r\
			backPart = paperdoll:GetComponent(\"back\")																							\n\r\
			helmetPart = paperdoll:GetComponent(\"helmet\")																						\n\r\
			hairPart = paperdoll:GetComponent(\"hair\")																							\n\r\
																																				\n\r\
			if paperdoll:GetBuildStage() <= 0 then																								\n\r\
				-- Query for a full-body torso model that has precedence over everything														\n\r\
				if paperdoll:QueryForItem(torsoIdx, torsoPart, \"g\", \"\", enableDelayedLoad) == 0 then										\n\r\
					return 0																													\n\r\
				end																																\n\r\
																																				\n\r\
				-- Update build stage																											\n\r\
				paperdoll:SetBuildStage(1)																										\n\r\
			end																																	\n\r\
																																				\n\r\
			if paperdoll:GetBuildStage() <= 1 then																								\n\r\
				if paperdoll:ComponentExists(torsoIdx) > 0 then																					\n\r\
					-- Update build stage to skip directly to head selection																	\n\r\
					paperdoll:SetBuildStage(80)																									\n\r\
				else																															\n\r\
					-- Reset the torso query so we can use it again																				\n\r\
					paperdoll:StopQuery(torsoIdx)																								\n\r\
																																				\n\r\
					-- Update build stage																										\n\r\
					paperdoll:SetBuildStage(5)																									\n\r\
				end																																\n\r\
			end																																	\n\r\
																																				\n\r\
			if paperdoll:GetBuildStage() <= 5 then																								\n\r\
				-- Query for a torso model that has precedence over the hand																	\n\r\
				if paperdoll:QueryForItem(torsoIdx, torsoPart, \"b d f\", \"\", enableDelayedLoad) == 0 then									\n\r\
					return 0																													\n\r\
				end																																\n\r\
																																				\n\r\
				-- Update build stage																											\n\r\
				paperdoll:SetBuildStage(10)																										\n\r\
			end																																	\n\r\
																																				\n\r\
			if paperdoll:GetBuildStage() <= 10 then																								\n\r\
				if paperdoll:ComponentExists(torsoIdx) > 0 then																					\n\r\
					-- Handle model is cut to the wrist																							\n\r\
					if paperdoll:QueryForItem(handIdx, handPart, \"b\", \"\", enableDelayedLoad) == 0 then										\n\r\
						return 0																												\n\r\
					end																															\n\r\
				else																															\n\r\
					-- There is no torso model that has precedence over the hand, so search for the hand first									\n\r\
					if paperdoll:QueryForItem(handIdx, handPart, \"a b\", \"\", enableDelayedLoad) == 0 then									\n\r\
						return 0																												\n\r\
					end																															\n\r\
																																				\n\r\
					if paperdoll:ComponentExists(handIdx) > 0 then																				\n\r\
						-- Reset the torso query so we can use it again																			\n\r\
						paperdoll:StopQuery(torsoIdx)																							\n\r\
																																				\n\r\
						if paperdoll:MatchComponentItemCode(handIdx, \"a\") > 0 then															\n\r\
							if paperdoll:QueryForItem(torsoIdx, torsoPart, \"a0 c0 e0\", \"\", enableDelayedLoad) == 0 then						\n\r\
								return 0																										\n\r\
							end																													\n\r\
						else																													\n\r\
							if paperdoll:QueryForItem(torsoIdx, torsoPart, \"a1 c1 e1\", \"\", enableDelayedLoad) == 0 then						\n\r\
								return 0																										\n\r\
							end																													\n\r\
						end																														\n\r\
					end																															\n\r\
				end																																\n\r\
																																				\n\r\
				-- Update build stage																											\n\r\
				paperdoll:SetBuildStage(20)																										\n\r\
			end																																	\n\r\
																																				\n\r\
			if paperdoll:GetBuildStage() <= 40 then																								\n\r\
				if (paperdoll:MatchComponentItemCode(torsoIdx, \"a0\") > 0) or (paperdoll:MatchComponentItemCode(torsoIdx, \"a1\") > 0) or (paperdoll:MatchComponentItemCode(torsoIdx, \"b\") > 0) then		\n\r\
					if paperdoll:GetBuildStage() <= 20 then																						\n\r\
						-- Leg exists, look for leg covering foot up to knee																	\n\r\
						if paperdoll:QueryForItem(legIdx, legPart, \"d\", \"\", enableDelayedLoad) == 0 then									\n\r\
							return 0																											\n\r\
						end																														\n\r\
																																				\n\r\
						-- Update build stage																									\n\r\
						paperdoll:SetBuildStage(25)																								\n\r\
					end																															\n\r\
																																				\n\r\
					if (paperdoll:GetBuildStage() <= 25) and(paperdoll:ComponentExists(legIdx) > 0) then										\n\r\
						-- Leg covers foot up to knee, look for foot																			\n\r\
						if paperdoll:QueryForItem(footIdx, footPart, \"b\", \"\", enableDelayedLoad) == 0 then									\n\r\
							return 0																											\n\r\
						end																														\n\r\
					else																														\n\r\
						if paperdoll:GetBuildStage() <= 25 then																					\n\r\
							-- Reset the leg query so we can use it again																		\n\r\
							paperdoll:StopQuery(legIdx)																							\n\r\
																																				\n\r\
							-- Update build stage																								\n\r\
							paperdoll:SetBuildStage(26)																							\n\r\
						end																														\n\r\
																																				\n\r\
						if paperdoll:GetBuildStage() <= 26 then																					\n\r\
							-- Leg exists, look for leg covering foot up to ankle																\n\r\
							if paperdoll:QueryForItem(legIdx, legPart, \"b\", \"\", enableDelayedLoad) == 0 then								\n\r\
								return 0																										\n\r\
							end																													\n\r\
																																				\n\r\
							-- Update build stage																								\n\r\
							paperdoll:SetBuildStage(30)																							\n\r\
						end																														\n\r\
																																				\n\r\
						if (paperdoll:GetBuildStage() <= 30) and(paperdoll:ComponentExists(legIdx) > 0) then									\n\r\
							-- Leg covers foot up to ankle, look for foot																		\n\r\
							if paperdoll:QueryForItem(footIdx, footPart, \"c\", \"\", enableDelayedLoad) == 0 then								\n\r\
								return 0																										\n\r\
							end																													\n\r\
						else																													\n\r\
							if paperdoll:GetBuildStage() <= 30 then																				\n\r\
								-- Leg is covered by foot, look for available foot first														\n\r\
								if paperdoll:QueryForItem(footIdx, footPart, \"a b c\", \"\", enableDelayedLoad) == 0 then						\n\r\
									return 0																									\n\r\
								end																												\n\r\
																																				\n\r\
								-- Reset the leg query so we can use it again																	\n\r\
								paperdoll:StopQuery(legIdx)																						\n\r\
																																				\n\r\
								-- Update build stage																							\n\r\
								paperdoll:SetBuildStage(40)																						\n\r\
							end																													\n\r\
																																				\n\r\
							if paperdoll:GetBuildStage() <= 40 then																				\n\r\
								-- Look for compatible leg																						\n\r\
								if paperdoll:MatchComponentItemCode(footIdx, \"a\") > 0 then													\n\r\
									if paperdoll:QueryForItem(legIdx, legPart, \"a0\", \"\", enableDelayedLoad) == 0 then						\n\r\
										return 0																								\n\r\
									end																											\n\r\
								elseif paperdoll:MatchComponentItemCode(footIdx, \"b\") > 0 then												\n\r\
									if paperdoll:QueryForItem(legIdx, legPart, \"a1\", \"\", enableDelayedLoad) == 0 then						\n\r\
										return 0																								\n\r\
									end																											\n\r\
								else																											\n\r\
									if paperdoll:QueryForItem(legIdx, legPart, \"a2\", \"\", enableDelayedLoad) == 0 then						\n\r\
										return 0																								\n\r\
									end																											\n\r\
								end																												\n\r\
							end																													\n\r\
						end																														\n\r\
					end																															\n\r\
																																				\n\r\
					-- Disable component color override on leg																					\n\r\
					paperdoll:SetComponentColorOverride(legIdx, -1)																				\n\r\
																																				\n\r\
				elseif (paperdoll:MatchComponentItemCode(torsoIdx, \"c0\") > 0) or (paperdoll:MatchComponentItemCode(torsoIdx, \"c1\") > 0) or (paperdoll:MatchComponentItemCode(torsoIdx, \"d\") > 0) then	\n\r\
					-- Leg does not exist, robe is knee-length, look for matching lower leg part												\n\r\
					if paperdoll:QueryForItem(legIdx, torsoPart, \"c\", \"\", enableDelayedLoad) == 0 then										\n\r\
						return 0																												\n\r\
					end																															\n\r\
																																				\n\r\
					-- Set leg to use same component color as torso																				\n\r\
					paperdoll:SetComponentColorOverride(legIdx, torsoIdx)																		\n\r\
																																				\n\r\
					-- Find compatible foot																										\n\r\
					if paperdoll:QueryForItem(footIdx, footPart, \"b a\", \"\", enableDelayedLoad) == 0 then									\n\r\
						return 0																												\n\r\
					end																															\n\r\
				elseif (paperdoll:MatchComponentItemCode(torsoIdx, \"e0\") > 0) or (paperdoll:MatchComponentItemCode(torsoIdx, \"e1\") > 0) or (paperdoll:MatchComponentItemCode(torsoIdx, \"f\") > 0) then	\n\r\
					-- Leg does not exist, robe is ankle-length, look for matching lower leg part												\n\r\
					if paperdoll:QueryForItem(legIdx, torsoPart, \"c\", \"\", enableDelayedLoad) == 0 then										\n\r\
						return 0																												\n\r\
					end																															\n\r\
																																				\n\r\
					-- Set leg to use same component color as torso																				\n\r\
					paperdoll:SetComponentColorOverride(legIdx, torsoIdx)																		\n\r\
																																				\n\r\
					-- Find compatible foot																										\n\r\
					if paperdoll:QueryForItem(footIdx, footPart, \"c b a\", \"\", enableDelayedLoad) == 0 then									\n\r\
						return 0																												\n\r\
					end																															\n\r\
				end																																\n\r\
																																				\n\r\
				-- Update build stage																											\n\r\
				paperdoll:SetBuildStage(50)																										\n\r\
			end																																	\n\r\
																																				\n\r\
			if paperdoll:GetBuildStage() <= 50 then																								\n\r\
				-- Belt uses same item name as torso																							\n\r\
				if paperdoll:QueryForItem(beltIdx, beltPart, \"x\", paperdoll:GetBaseItemKey(torsoIdx), enableDelayedLoad) == 0 then			\n\r\
					return 0																													\n\r\
				end																																\n\r\
																																				\n\r\
				-- Update build stage																											\n\r\
				paperdoll:SetBuildStage(60)																										\n\r\
			end																																	\n\r\
																																				\n\r\
			if paperdoll:GetBuildStage() <= 60 then																								\n\r\
				-- Shoulder																														\n\r\
				if paperdoll:QueryForItem(shoulderIdx, shoulderPart, \"x\", \"\", enableDelayedLoad) == 0 then									\n\r\
					return 0																													\n\r\
				end																																\n\r\
																																				\n\r\
				-- Update build stage																											\n\r\
				paperdoll:SetBuildStage(70)																										\n\r\
			end																																	\n\r"
			"																																	\n\r\
			if paperdoll:GetBuildStage() <= 70 then																								\n\r\
				-- Back																															\n\r\
				if paperdoll:MatchComponentItemAuxCode(torsoIdx, \"x\") > 0 then																\n\r\
					if paperdoll:QueryForItem(backIdx, backPart, \"a\", \"\", enableDelayedLoad) == 0 then										\n\r\
						return 0																												\n\r\
					end																															\n\r\
				else																															\n\r\
					if paperdoll:QueryForItem(backIdx, backPart, \"b\", \"\", enableDelayedLoad) == 0 then										\n\r\
						return 0																												\n\r\
					end																															\n\r\
				end																																\n\r\
																																				\n\r\
				-- Update build stage																											\n\r\
				paperdoll:SetBuildStage(80)																										\n\r\
			end																																	\n\r\
																																				\n\r\
			if paperdoll:GetBuildStage() <= 80 then																								\n\r\
				-- Locate helmet																												\n\r\
				if paperdoll:QueryForItem(helmetIdx, helmetPart, \"a b c d\", \"\", enableDelayedLoad) == 0 then								\n\r\
					return 0																													\n\r\
				end																																\n\r\
																																				\n\r\
				if paperdoll:ComponentExists(helmetIdx) > 0 then																				\n\r\
					if paperdoll:MatchComponentItemCode(helmetIdx, \"a\") > 0 then																\n\r\
						-- Helmet is a mask type																								\n\r\
						if paperdoll:QueryForItem(hairIdx, hairPart, \"a\", \"\", enableDelayedLoad) == 0 then									\n\r\
							return 0																											\n\r\
						end																														\n\r\
																																				\n\r\
						-- Head																													\n\r\
						if paperdoll:QueryForItem(headIdx, headPart, \"x\", \"\", enableDelayedLoad) == 0 then									\n\r\
							return 0																											\n\r\
						end																														\n\r\
					elseif paperdoll:MatchComponentItemCode(helmetIdx, \"b\") > 0 then															\n\r\
						-- Helmet is a mask type																								\n\r\
						if paperdoll:QueryForItem(hairIdx, hairPart, \"b\", \"\", enableDelayedLoad) == 0 then									\n\r\
							return 0																											\n\r\
						end																														\n\r\
																																				\n\r\
						-- Head																													\n\r\
						if paperdoll:QueryForItem(headIdx, headPart, \"x\", \"\", enableDelayedLoad) == 0 then									\n\r\
							return 0																											\n\r\
						end																														\n\r\
					elseif paperdoll:MatchComponentItemCode(helmetIdx, \"c\") > 0 then															\n\r\
						-- Helmet is a mask type																								\n\r\
						if paperdoll:QueryForItem(hairIdx, hairPart, \"c\", \"\", enableDelayedLoad) == 0 then									\n\r\
							return 0																											\n\r\
						end																														\n\r\
																																				\n\r\
						-- Head																													\n\r\
						if paperdoll:QueryForItem(headIdx, headPart, \"x\", \"\", enableDelayedLoad) == 0 then									\n\r\
							return 0																											\n\r\
						end																														\n\r\
					elseif paperdoll:MatchComponentItemCode(helmetIdx, \"d\") > 0 then															\n\r\
						-- Helmet is a fully coverying helm																						\n\r\
					end																															\n\r\
				else																															\n\r\
					-- Full hair																												\n\r\
					if paperdoll:QueryForItem(hairIdx, hairPart, \"a\", \"\", enableDelayedLoad) == 0 then										\n\r\
						return 0																												\n\r\
					end																															\n\r\
																																				\n\r\
					-- Head																														\n\r\
					if paperdoll:QueryForItem(headIdx, headPart, \"x\", \"\", enableDelayedLoad) == 0 then										\n\r\
						return 0																												\n\r\
					end																															\n\r\
				end																																\n\r\
																																				\n\r\
				-- Update build stage																											\n\r\
				paperdoll:SetBuildStage(90)																										\n\r\
			end																																	\n\r\
																																				\n\r\
			return 1																															\n\r\
		end																																		\n\r\
		";

	// Initialize our function state
	if(luaL_dostring(luaState, assemblyFunc))
	{
		MessageBox(NULL, "Error initializing default assembly rule", "ERROR", MB_OK | MB_ICONSTOP);
	}
}

// ************************************************************************************************************************************************************

CRuPaperdoll_Placeholder::CRuPaperdoll_Placeholder()
:	m_paperdollBuilding(FALSE)
{
}

CRuPaperdoll_Placeholder::~CRuPaperdoll_Placeholder()
{
	ClearComponentTree();
}

CRuPaperdollTemplate *CRuPaperdoll_Placeholder::GetPaperdollTemplate()
{
	return NULL;
}

const char *CRuPaperdoll_Placeholder::GetComponent(const char *partName)
{
	ComponentSetting compSetting;

	if(partName == NULL)
	{
		return "";
	}

	if(m_componentTree.Get(partName, compSetting))
	{
		return compSetting.m_itemName;
	}

	return "";
}

BOOL CRuPaperdoll_Placeholder::SetComponent(const char *partName, const char *itemName)
{
	ComponentSetting compSetting;

	if(partName == NULL || itemName == NULL)
	{
		return FALSE;
	}

	if(m_componentTree.Get(partName, compSetting))
	{
		// Copy item name
		delete[] compSetting.m_itemName;
		compSetting.m_itemName = ruNEW char [strlen(itemName) + 1];
		strcpy(compSetting.m_itemName, itemName);

		// Update component setting into tree
		m_componentTree.Set(partName, compSetting);
	}
	else
	{
		// Copy item name
		compSetting.m_itemName = ruNEW char [strlen(itemName) + 1];
		strcpy(compSetting.m_itemName, itemName);

		// Insert component setting into tree
		m_componentTree.Add(partName, compSetting);
	}

	return TRUE;
}

BOOL CRuPaperdoll_Placeholder::GetComponentColors(const char *partName, BOOL *layerActivatedOut, RUCOLOR *layerColorOut)
{
	ComponentSetting compSetting;

	if(partName == NULL)
	{
		return FALSE;
	}
	if(m_componentTree.Get(partName, compSetting))
	{
		// Copy settings
		memcpy(layerActivatedOut, compSetting.m_layerActivated, sizeof(BOOL) * 4);
		memcpy(layerColorOut, compSetting.m_layerColor, sizeof(RUCOLOR) * 4);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuPaperdoll_Placeholder::SetComponentColors(const char *partName, const BOOL *layerActivated, const RUCOLOR *layerColor)
{
	ComponentSetting compSetting;

	if(partName == NULL)
	{
		return FALSE;
	}

	if(m_componentTree.Get(partName, compSetting))
	{
		// Copy settings
		memcpy(compSetting.m_layerActivated, layerActivated, sizeof(BOOL) * 4);
		memcpy(compSetting.m_layerColor, layerColor, sizeof(RUCOLOR) * 4);

		// Update component setting into tree
		m_componentTree.Set(partName, compSetting);
	}
	else
	{
		// Copy settings
		memcpy(compSetting.m_layerActivated, layerActivated, sizeof(BOOL) * 4);
		memcpy(compSetting.m_layerColor, layerColor, sizeof(RUCOLOR) * 4);

		// Insert component setting into tree
		m_componentTree.Add(partName, compSetting);
	}

	return TRUE;
}

BOOL CRuPaperdoll_Placeholder::BuildPaperdoll()
{
	m_paperdollBuilding = TRUE;
	return TRUE;
}

BOOL CRuPaperdoll_Placeholder::BuildPaperdollEx()
{
	m_paperdollBuilding = TRUE;
	return TRUE;
}

BOOL CRuPaperdoll_Placeholder::IsPaperdollBuilding() const
{
	return m_paperdollBuilding;
}

void CRuPaperdoll_Placeholder::ClearComponentTree()
{
	m_componentTree.ForAllElements(boost::bind(&CRuPaperdoll_Placeholder::ClearComponentTreeCallback, this, _1, _2));
}

BOOL CRuPaperdoll_Placeholder::ClearComponentTreeCallback(const char *key, void *data)
{
	ComponentSetting &compSetting = *reinterpret_cast<ComponentSetting *>(data);

	delete[] compSetting.m_itemName;
	compSetting.m_itemName = NULL;

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuPaperdoll::CRuPaperdoll()
:	m_maxMipLevel(0),
	m_maxMipLevelOverride(0),
	m_paperdollTemplate(NULL),
	m_paperdollModel(NULL),
	m_buildInProgress(FALSE),
	m_rebuildRequired(FALSE),
	m_paperdollIsValid(FALSE),
	m_luaState(NULL)
{
	// Apply settings
	GetSettingsFromEngine();

	// Clear rebuild required flag
	m_rebuildRequired = FALSE;

	if(g_ruEngineSettings)
	{
		// Register listener on engine settings
		g_ruEngineSettings->RegisterSettingsChangeHandler(this->GetGUID(), boost::bind(&CRuPaperdoll::EngineSettingsChangedHandler, this, _1, _2));
	}

	// Initialize lua VM
	m_luaState = lua_open();

	// Initialize base lua libraries
	luaopen_base(m_luaState);
	luaopen_math(m_luaState);

	// Initialize luabind
	luabind::open(m_luaState);

	// Setup VM modules
	luabind::module(m_luaState)
		[
			luabind::class_<CRuPaperdollTemplate>("CRuPaperdollTemplate")
				.def("getpartindex", (INT32 (CRuPaperdollTemplate::*)(const char *) const) &CRuPaperdollTemplate::GetPartIndex)

				.def("GetPartIndex", (INT32 (CRuPaperdollTemplate::*)(const char *) const) &CRuPaperdollTemplate::GetPartIndex),

			luabind::class_<CRuPaperdoll>("CRuPaperdoll")
				.def("getpaperdolltemplate", &CRuPaperdoll::GetPaperdollTemplate)
				.def("getcomponent", (const char * (CRuPaperdoll::*)(const char *)) &CRuPaperdoll::GetComponent)
				.def("setcomponentcoloroverride", &CRuPaperdoll::SetComponentColorOverride)
				.def("getbuildstage", &CRuPaperdoll::GetBuildStage)
				.def("setbuildstage", &CRuPaperdoll::SetBuildStage)
				.def("componentexists", &CRuPaperdoll::ComponentExists)
				.def("matchcomponentitemcode", &CRuPaperdoll::MatchComponentItemCode)
				.def("matchcomponentitemauxcode", &CRuPaperdoll::MatchComponentItemAuxCode)
				.def("getbaseitemkey", &CRuPaperdoll::GetBaseItemKey)
				.def("stopquery", &CRuPaperdoll::StopQuery)
				.def("queryforitem", (BOOL (CRuPaperdoll::*)(INT32, const char *, const char *, const void *, BOOL)) &CRuPaperdoll::QueryForItem)
				.def("queryforitem", (BOOL (CRuPaperdoll::*)(INT32, const char *, const char *, const char *, BOOL)) &CRuPaperdoll::QueryForItem)

				.def("GetPaperdollTemplate", &CRuPaperdoll::GetPaperdollTemplate)
				.def("GetComponent", (const char * (CRuPaperdoll::*)(const char *)) &CRuPaperdoll::GetComponent)
				.def("SetComponentColorOverride", &CRuPaperdoll::SetComponentColorOverride)
				.def("GetBuildStage", &CRuPaperdoll::GetBuildStage)
				.def("SetBuildStage", &CRuPaperdoll::SetBuildStage)
				.def("ComponentExists", &CRuPaperdoll::ComponentExists)
				.def("MatchComponentItemCode", &CRuPaperdoll::MatchComponentItemCode)
				.def("MatchComponentItemAuxCode", &CRuPaperdoll::MatchComponentItemAuxCode)
				.def("GetBaseItemKey", &CRuPaperdoll::GetBaseItemKey)
				.def("StopQuery", &CRuPaperdoll::StopQuery)
				.def("QueryForItem", (BOOL (CRuPaperdoll::*)(INT32, const char *, const char *, const void *, BOOL)) &CRuPaperdoll::QueryForItem)
				.def("QueryForItem", (BOOL (CRuPaperdoll::*)(INT32, const char *, const char *, const char *, BOOL)) &CRuPaperdoll::QueryForItem)
		];


	// NOTE: QueryForItem used for lua has a void parameter for passing the override string because there is no implicit type conversion from nil!

}

CRuPaperdoll::~CRuPaperdoll()
{
	// Unregister listener on engine settings
	g_ruEngineSettings->UnregisterSettingsChangeHandler(this->GetGUID());

	// Release reference to paperdoll template
	ruSAFE_RELEASE(m_paperdollTemplate);

	// Detach model and release reference to it
	ruSAFE_DETACH(m_paperdollModel);
	ruSAFE_RELEASE(m_paperdollModel);

	// Release component entities
	for(INT32 i = 0; i < m_componentEntities.Count(); ++i)
	{
		ruSAFE_RELEASE(m_componentEntities[i]);
	}

	// Release component descriptors
	for(INT32 i = 0; i < m_componentDescriptors.Count(); ++i)
	{
		delete m_componentDescriptors[i];
	}

	// Clean up component queries
	for(INT32 i = 0; i < m_componentQueries.Count(); ++i)
	{
		m_componentQueries[i].StopQuery();
	}

	// Shutdown lua VM
	lua_close(m_luaState);
}

CRuEvent &CRuPaperdoll::Event_BuildComplete()
{
	return m_event_BuildComplete;
}

IRuObject *CRuPaperdoll::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuPaperdoll();

	// Chain cloning call to parent class
	CRuEntity::Clone(clonedObject);



	// When we clone a paperdoll, we make an explicit mesh/texture combination...? NO!



	return clonedObject;
}

BOOL CRuPaperdoll::SerializeFrom(IRuStream *stream)
{
	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuEntity::SerializeFrom(stream);

	return TRUE;
}

BOOL CRuPaperdoll::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuEntity::SerializeTo(stream);

	return TRUE;
}

BOOL CRuPaperdoll::Update(REAL elapsedTime)
{
	if(m_buildInProgress)
	{
		BuildPaperdollEx_Internal(m_enableDelayedBuild);
	}

	if(m_rebuildRequired)
	{
		BuildPaperdoll();
		m_rebuildRequired = FALSE;
	}

	// Chain update call to parent class
	return CRuEntity::Update(elapsedTime);
}

BOOL CRuPaperdoll::SetPaperdollBuildCompleteCallback(PaperdollBuildCompleteCallback callback)
{
	m_buildCompleteCallback = callback;
	return TRUE;
}

CRuPaperdollTemplate *CRuPaperdoll::GetPaperdollTemplate()
{
	return m_paperdollTemplate;
}

CRuCompositeModel *CRuPaperdoll::GetCompositeModel()
{
	return m_paperdollModel;
}

void CRuPaperdoll::SetMaxMipLevelOverride(INT32 maxMipLevel)
{
	m_maxMipLevelOverride = maxMipLevel;
}

const char *CRuPaperdoll::GetComponent(const char *partName)
{
	// Ensure the paperdoll is properly initialized
	if(!m_paperdollTemplate)
		return FALSE;

	// Find the part index for the specified part
	INT32 partIndex = m_paperdollTemplate->GetPartIndex(partName);

	return GetComponent(partIndex);
}

const char *CRuPaperdoll::GetComponent(INT32 partIndex)
{
	if(partIndex >= 0 && partIndex < m_components.Count())
	{
		// Return the component name
		return m_components[partIndex];
	}

	return "";
}

BOOL CRuPaperdoll::SetComponent(const char *partName, const char *itemName)
{
	// Ensure the paperdoll is properly initialized
	if(!m_paperdollTemplate)
		return FALSE;

	// Find the part index for the specified part
	INT32 partIndex = m_paperdollTemplate->GetPartIndex(partName);

	return SetComponent(partIndex, itemName);
}

BOOL CRuPaperdoll::SetComponent(INT32 partIndex, const char *itemName)
{
	// Ensure the paperdoll is properly initialized
	if(!m_paperdollTemplate)
		return FALSE;

	CRuString moo;
	while(m_components.Count() < m_paperdollTemplate->GetNumParts())
	{
		m_components.Add(moo);
	}

	while(m_componentEntities.Count() < m_paperdollTemplate->GetNumParts())
	{
		m_componentEntities.Add(NULL);
	}

	while(m_componentItemCodes.Count() < m_paperdollTemplate->GetNumParts())
	{
		m_componentItemCodes.Add("x");
	}

	while(m_componentDescriptors.Count() < m_paperdollTemplate->GetNumParts())
	{
		m_componentDescriptors.Add(ruNEW ComponentDesc());
	}

	m_componentQueries.SetArraySize(m_paperdollTemplate->GetNumParts());
	m_componentQueries.SetNumEntries(m_paperdollTemplate->GetNumParts());

	if(partIndex >= 0 && partIndex < m_components.Count())
	{
		// Store the specified item name
		m_components[partIndex] = itemName;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuPaperdoll::GetComponentColors(const char *partName, BOOL *layerActivatedOut, RUCOLOR *layerColorOut)
{
	// Ensure the paperdoll is properly initialized
	if(!m_paperdollTemplate)
		return FALSE;

	// Find the part index for the specified part
	INT32 partIndex = m_paperdollTemplate->GetPartIndex(partName);

	return GetComponentColors(partIndex, layerActivatedOut, layerColorOut);
}

BOOL CRuPaperdoll::GetComponentColors(INT32 partIndex, BOOL *layerActivatedOut, RUCOLOR *layerColorOut)
{
	if(partIndex >= 0 && partIndex < m_components.Count())
	{
		for(INT32 i = 0; i < 4; ++i)
		{
			layerActivatedOut[i] = m_componentDescriptors[partIndex]->m_layerActivated[i];
			layerColorOut[i] = m_componentDescriptors[partIndex]->m_layerColor[i];
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuPaperdoll::SetComponentColors(const char *partName, const BOOL *layerActivated, const RUCOLOR *layerColor)
{
	// Ensure the paperdoll is properly initialized
	if(!m_paperdollTemplate)
		return FALSE;

	// Find the part index for the specified part
	INT32 partIndex = m_paperdollTemplate->GetPartIndex(partName);

	return SetComponentColors(partIndex, layerActivated, layerColor);
}

BOOL CRuPaperdoll::SetComponentColors(INT32 partIndex, const BOOL *layerActivated, const RUCOLOR *layerColor)
{
	// Ensure the paperdoll is properly initialized
	if(!m_paperdollTemplate)
		return FALSE;

	CRuString moo;
	while(m_components.Count() < m_paperdollTemplate->GetNumParts())
	{
		m_components.Add(moo);
	}

	while(m_componentEntities.Count() < m_paperdollTemplate->GetNumParts())
	{
		m_componentEntities.Add(NULL);
	}

	while(m_componentItemCodes.Count() < m_paperdollTemplate->GetNumParts())
	{
		m_componentItemCodes.Add("x");
	}

	while(m_componentDescriptors.Count() < m_paperdollTemplate->GetNumParts())
	{
		m_componentDescriptors.Add(ruNEW ComponentDesc());
	}

	m_componentQueries.SetArraySize(m_paperdollTemplate->GetNumParts());
	m_componentQueries.SetNumEntries(m_paperdollTemplate->GetNumParts());

	if(partIndex >= 0 && partIndex < m_components.Count())
	{
		for(INT32 i = 0; i < 4; ++i)
		{
			m_componentDescriptors[partIndex]->m_layerActivated[i] = layerActivated[i] ? TRUE : FALSE;
			m_componentDescriptors[partIndex]->m_layerColor[i] = layerColor[i];
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuPaperdoll::SetComponentColorOverride(INT32 partIndex, INT32 overrideIndex)
{
	// Ensure the paperdoll is properly initialized
	if(!m_paperdollTemplate)
		return FALSE;

	if(partIndex >= 0 && partIndex < m_components.Count())
	{
		for(INT32 i = 0; i < 4; ++i)
		{
			m_componentDescriptors[partIndex]->m_overrideUsingIndex = overrideIndex;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuPaperdoll::BuildPaperdoll()
{
	// NOTE: Clear m_componentResourceNames pointers here
	for(INT32 i = 0; i < m_componentEntities.Count(); ++i)
	{
		ruSAFE_RELEASE(m_componentEntities[i]);
		m_componentEntities[i] = NULL;
	}

	for(INT32 i = 0; i < m_componentQueries.Count(); ++i)
	{
		m_componentQueries[i].StopQuery();
	}

	m_enableDelayedBuild = FALSE;
	m_buildInProgress = TRUE;
	m_stageIndex = -1;

	return BuildPaperdollEx_Internal(FALSE);
}

BOOL CRuPaperdoll::BuildPaperdollEx()
{
	// NOTE: Clear m_componentResourceNames pointers here
	for(INT32 i = 0; i < m_componentEntities.Count(); ++i)
	{
		ruSAFE_RELEASE(m_componentEntities[i]);
		m_componentEntities[i] = NULL;
	}

	for(INT32 i = 0; i < m_componentQueries.Count(); ++i)
	{
		m_componentQueries[i].StopQuery();
	}

	m_enableDelayedBuild = TRUE;
	m_buildInProgress = TRUE;
	m_stageIndex = -1;

	return TRUE;
}

BOOL CRuPaperdoll::IsPaperdollBuilding() const
{
	return m_buildInProgress;
}

BOOL CRuPaperdoll::IsPaperdollInValidState() const
{
	return m_paperdollIsValid;
}

BOOL CRuPaperdoll::BuildPaperdollEx_Internal(BOOL enableDelayedLoad)
{
	// Ensure the paperdoll is properly initialized
	if(!m_paperdollTemplate || m_components.Count() == 0)
		return FALSE;
/*
	DWORD t0, t1, et;

	t0 = timeGetTime();
*/
	if(m_stageIndex < 0)
	{
		// Invoke the template assembly routine
		INT32 retVal = AssembleComponents(enableDelayedLoad);
		switch(retVal)
		{
			case 0:			// Still assembling
				return TRUE;

			default:
			case 1:			// Component select OK, set stage index to zero
				m_stageIndex = 0;
				break;

			case -1:		// Failed
				return FALSE;
		}
/*
		t1 = timeGetTime();
		et = t1 - t0;
		char outputStr[256];
		sprintf(outputStr, "AssembleComponents call completed in %u ms.\n", et);
		OutputDebugString(outputStr);
*/
	}

	// Instance component parts
	for(INT32 i = 0; i < m_componentQueries.Count(); ++i)
	{
		// Skip items that are already instanced
		if(m_componentQueries[i].ItemIsInstanced())
			continue;

		// Invoke item instantiation
		m_componentQueries[i].InstanceItem(enableDelayedLoad);
	}

	// If any components are still not instanced, return
	for(INT32 i = 0; i < m_componentQueries.Count(); ++i)
	{
		if(!m_componentQueries[i].ItemIsInstanced())
		{
			return TRUE;
		}

		// Store instantiation result
		if(m_componentQueries[i].m_resultItem)
		{
			// Increment reference count on result item
			m_componentQueries[i].m_resultItem->AddRef();

			// Release existing component entity
			ruSAFE_RELEASE(m_componentEntities[i]);

			// Store reference to the result item
			m_componentEntities[i] = m_componentQueries[i].m_resultItem;
		}
	}

	// Assemble paperdoll
	for(INT32 i = 0; i < m_paperdollTemplate->GetNumParts(); ++i)
	{
		m_paperdollModel->SetPart(m_paperdollTemplate->GetPartID(i), NULL);
	}

	for(INT32 i = 0; i < m_componentEntities.Count(); ++i)
	{
		if(m_componentEntities[i])
		{
			if(m_componentEntities[i])
			{
				m_paperdollModel->SetPart(m_paperdollTemplate->GetPartID(i), m_componentEntities[i]);

				// Play persistent motion
				RuACT_PlayPersistentMotion(m_componentEntities[i], FALSE);
			}

			// Check the source color index
			INT32 colorSrcIdx = m_componentDescriptors[i]->m_overrideUsingIndex >= 0 ? m_componentDescriptors[i]->m_overrideUsingIndex : i;

			// Set part colors
			m_paperdollModel->SetPartColors(m_paperdollTemplate->GetPartID(i), m_componentDescriptors[colorSrcIdx]->m_layerActivated, m_componentDescriptors[colorSrcIdx]->m_layerColor);
		}
	}

	// Build the composite model
	m_paperdollModel->SetMaxMipLevel(max(m_maxMipLevel, m_maxMipLevelOverride));
	m_paperdollModel->BuildModel();

	// Enable skinning
	m_paperdollModel->SetOption(ruENTITYOPTION_SKINNED, 1);

	// Enable alpha testing
	GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, TRUE);
	GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 170);

	// Enable smooth blending for both hair and head models if they exist
	INT32 hairIndex = m_paperdollTemplate->GetPartIndex("hair");
	INT32 headIndex = m_paperdollTemplate->GetPartIndex("head");

	if(hairIndex >= 0 && headIndex >= 0)
	{
		if(m_componentEntities[hairIndex])
		{
			CRuArrayList<IRuMaterial *> hairMaterials;
			RuEntity_EnumerateMaterials(m_componentEntities[hairIndex], hairMaterials);

			for(INT32 i = 0; i < hairMaterials.Count(); ++i)
			{
				hairMaterials[i]->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SMOOTHBLEND, TRUE);
			}
		}

		if(m_componentEntities[headIndex])
		{
			CRuArrayList<IRuMaterial *> headMaterials;
			RuEntity_EnumerateMaterials(m_componentEntities[headIndex], headMaterials);

			for(INT32 i = 0; i < headMaterials.Count(); ++i)
			{
				headMaterials[i]->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SMOOTHBLEND, TRUE);
			}
		}
	}

	// Terminate build
	m_buildInProgress = FALSE;
	m_rebuildRequired = FALSE;
	m_paperdollIsValid = TRUE;

	// Invoke build complete callback
	if(m_buildCompleteCallback)
	{
		m_buildCompleteCallback(this);
	}

	// HACK - detach and re-attach to parent (this forces the GR2 hierarchy to rebuild its sparse bone array)
	CRuEntity *parent = this->GetParent();
	if(parent)
	{
		parent->AddChild(this);
	}

	// Force all descendants to use parent lighting
	RuEntity_UseParentLighting(this, TRUE);

	// Trigger paperdoll build complete event
	RuPaperdollEvent_BuildComplete_Args eventArgs(this);
	m_event_BuildComplete.Trigger(&eventArgs);


	// TEMP CODE - Should move this somewhere else.. this is too specialized, used for establish transform coupling on specific types of link points
	//             AND this does not yet search for link frames... we search only for frames
	for(INT32 i = 0; i < m_componentEntities.Count(); ++i)
	{
		if(m_componentEntities[i])
		{
			// Enumerate all meshes within the component entity
			CRuArrayList<IRuMesh *> meshes(8);
			RuEntity_EnumerateMeshes(m_componentEntities[i], meshes);

			// Enumerate all frames and link frames within the component entity
			CRuArrayList<CRuEntity *> candidateEntities(16);
			RuEntity_EnumerateEntities(m_componentEntities[i], CRuFrame::Type(), TRUE, candidateEntities);
			RuEntity_EnumerateEntities(m_componentEntities[i], CRuLinkFrame::Type(), TRUE, candidateEntities);

			for(INT32 i = 0; i < candidateEntities.Count(); ++i)
			{
				// Only process entities whose name begins with a "p_" (this is our naming convention for link points)
				if(strnicmp(candidateEntities[i]->GetObjectName(), "p_", 2) == 0)
				{
					// Find the bone with the greatest influence on the link point
					INT32 boneIdx = RuSkinTool_FindGreatestInfluencingBone(meshes.GetArray(), meshes.Count(), candidateEntities[i]->GetTranslation(ruFRAMETYPE_LOCAL), 2.00f);

					if(boneIdx >= 0)
					{
						// Find an available parent hierarchy
						CRuHierarchy_GR2 *parentHierarchy = RuEntity_FindParentHierarchy_GR2(this);

						if(parentHierarchy)
						{
							// Establish coupling with parent hierarchy
							CRuHierarchyCoupler *coupler = ruNEW CRuHierarchyCoupler();
							coupler->EstablishCoupling(candidateEntities[i], parentHierarchy, boneIdx);
						}
					}
				}
			}
		}
	}


	return TRUE;
}

INT32 CRuPaperdoll::AssembleComponents(BOOL enableDelayedLoad)
{
	/*
		NOTES
		-----
		The assemble components code originally ran in a lua VM under the paperdoll template as
		a common shared unit. However, this resulted in paperdolls being built incorrectly when
		more than one paperdoll is being built. The behavior is highly erratic. No cause was
		found. Need to investigate further into the way luabind works -- it could be a bug in
		the binding code. Or, hopefully, it's simply a logic error on my part that causes
		states to get messed up.
	*/
	int retVal = 0;

	try
	{
		retVal = luabind::call_function<int>(m_luaState, "lua_AssembleComponents", this, enableDelayedLoad);
	}

	catch(luabind::error &e)
	{
		// Return -1 to indicate that lua has failed
		retVal = -1;

		// Error processing
		luabind::object error_msg(luabind::from_stack(e.state(), -1));

		OutputDebugString(e.what());
		OutputDebugString("\n");
		OutputDebugString(lua_tostring(m_luaState, lua_gettop(m_luaState)));
		OutputDebugString("\n");
	}

	if(retVal == -1)
	{
		try
		{
			// Try calling again with lower-case function
			retVal = luabind::call_function<int>(m_luaState, "lua_assemblecomponents", this, enableDelayedLoad);
		}

		catch(luabind::error &e)
		{
			// Return -1 to indicate that lua has failed
			retVal = -1;

			// Error processing
			luabind::object error_msg(luabind::from_stack(e.state(), -1));
		}
	}

	// If an exception has occurred... what to do?
	if(retVal == -1)
	{
	}

	return retVal;
}

INT32 CRuPaperdoll::GetBuildStage()
{
	return m_stageIndex;
}

INT32 CRuPaperdoll::SetBuildStage(INT32 buildStage)
{
	m_stageIndex = buildStage;
	return m_stageIndex;
}

BOOL CRuPaperdoll::ComponentExists(INT32 partIndex)
{
//	return m_componentEntities[partIndex] != NULL;
	return m_componentQueries[partIndex].m_itemIsAvailable;
}

BOOL CRuPaperdoll::MatchComponentItemCode(INT32 partIndex, const char *itemCode)
{
	return stricmp(m_componentQueries[partIndex].m_itemCodeOut, itemCode) == 0;
}

BOOL CRuPaperdoll::MatchComponentItemAuxCode(INT32 partIndex, const char *auxCode)
{
	return m_componentQueries[partIndex].m_itemAuxCodeOut && (stricmp(m_componentQueries[partIndex].m_itemAuxCodeOut, auxCode) == 0);
}

const char *CRuPaperdoll::GetBaseItemKey(INT32 partIndex)
{
	return m_componentQueries[partIndex].m_baseItemKeyOut;
}

BOOL CRuPaperdoll::StopQuery(INT32 partIdx)
{
	m_componentQueries[partIdx].StopQuery();
	return TRUE;
}

BOOL CRuPaperdoll::QueryForItem(INT32 partIndex, const char *itemName, const char *acceptableItemCodeList, const void *dummy, BOOL enableDelayedLoad)
{
	return QueryForItem(partIndex, itemName, acceptableItemCodeList, "", enableDelayedLoad);
}

BOOL CRuPaperdoll::QueryForItem(INT32 partIndex, const char *itemName, const char *acceptableItemCodeList, const char *baseItemKeyOverride, BOOL enableDelayedLoad)
{
	CRuArrayList<const char *> convertedItemCodeList;

	CRuStringTokenizer tokenizer;
	tokenizer.Tokenize(acceptableItemCodeList, " ");

	for(INT32 i = 0; i < tokenizer.GetNumTokens(); ++i)
	{
		convertedItemCodeList.Add(tokenizer.GetToken(i));
	}

	return QueryForItem(partIndex, itemName, &convertedItemCodeList, reinterpret_cast<const char *>(baseItemKeyOverride), enableDelayedLoad);
}

BOOL CRuPaperdoll::QueryForItem(INT32 partIndex, const char *itemName, CRuArrayList<const char *> *acceptableItemCodeList, const char *baseItemKeyOverride, BOOL enableDelayedLoad)
{
	if(!m_componentQueries[partIndex].QueryIsComplete())
	{
		// NOTE: We always load textures according to the max mip level, because these textures are shared among all paperdolls
		m_componentQueries[partIndex].StartQuery(m_maxMipLevel, partIndex, itemName, acceptableItemCodeList, baseItemKeyOverride);
		m_paperdollTemplate->FindSuitableItem(m_componentQueries[partIndex], enableDelayedLoad);

		// Check query completion
		if(!m_componentQueries[partIndex].QueryIsComplete())
			return FALSE;
	}

	return TRUE;
}

void CRuPaperdoll::GetSettingsFromEngine()
{
	INT32 oldMaxMipLevel = m_maxMipLevel;

	RuPropertyType propertyValue;
	m_maxMipLevel = g_ruEngineSettings->GetSetting("paperdoll:max mip level", propertyValue) ? boost::get<INT32>(propertyValue) : 0;

	if(m_maxMipLevel > 3)
	{
		m_maxMipLevel = 3;
	}

	if(m_paperdollModel)
	{
		m_paperdollModel->SetMaxMipLevel(max(m_maxMipLevel, m_maxMipLevelOverride));
	}

	if(m_maxMipLevel != oldMaxMipLevel)
	{
		m_rebuildRequired = TRUE;
	}
}

BOOL CRuPaperdoll::EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting)
{
	GetSettingsFromEngine();
	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
