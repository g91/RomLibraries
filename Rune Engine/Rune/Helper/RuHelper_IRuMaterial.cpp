/*!
	@project Rune
	@file RuHelper_IRuMaterial.cpp

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/04/13
*/

#include "../Helper/RuHelper_IRuMaterial.h"
#include "../Engine/Texture/RuTexture_Proxy.h"
#include "../ImageFX/RuImageFX_Blitter.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Loads color dye mask textures based on the diffuse texture name of the target material,
	and composites the dye mask textures into one combine dye mask texture.

	@param material Material to act upon

	@return TRUE if successful, FALSE otherwise.
*/
BOOL RuMaterial_BuildDyeFXMask(IRuMaterial *material)
{
	// If the composite color dye mask already exists, return
	if(material->ChannelExists(ruTEXCHANNEL_PAPERDOLL_MASK_COMPOSITE))
	{
		return TRUE;
	}

	// Retrieve diffuse texture
	IRuBaseTexture *diffuseTexture = material->GetTexture(ruTEXCHANNEL_DIFFUSEMAP);

	if(diffuseTexture)
	{
		BOOL dyeMasksReady = TRUE;

		for(INT32 i = 0; i < 4; ++i)
		{
			// If the color dye mask channel does not exist, create it
			if(material->ChannelExists(static_cast<RuTextureChannel>(ruTEXCHANNEL_PAPERDOLL_MASK_0 + i)) == FALSE)
			{
				// Strip extension from the texture name if necessary
				CRuString diffuseTextureName = diffuseTexture->GetTextureName();

				INT32 lastSlashIndex = diffuseTextureName.LastIndexOf("\\");
				INT32 lastPeriodIndex = diffuseTextureName.LastIndexOf(".");

				if(lastPeriodIndex > lastSlashIndex)
				{
					diffuseTextureName.TruncateAt(lastPeriodIndex);
				}

				// Build the four color dye mask texture names
				const char *layerKeywordPrefix = "_layer";
				INT32 layerKeywordPrefixLen = (INT32)strlen(layerKeywordPrefix);

				// Build full dye mask texture path
				INT32 dyeLayerTexLen = diffuseTextureName.GetLength() + layerKeywordPrefixLen + 2;
				char *dyeLayerTexName = ruNEW char [dyeLayerTexLen];
				sprintf(dyeLayerTexName, "%s%s%d", static_cast<char *>(diffuseTextureName), layerKeywordPrefix, i + 1);

				// Set dye mask texture path into material
				material->SetTextureByName(static_cast<RuTextureChannel>(ruTEXCHANNEL_PAPERDOLL_MASK_0 + i), dyeLayerTexName, ruSURFACEFMT_A8R8G8B8);

				// Delete allocated memory
				delete[] dyeLayerTexName;
			}

			// Bind material textures
			material->BindTextures(TRUE);

			// Determine if the texture is ready
			dyeMasksReady = dyeMasksReady & material->IsTextureBound(static_cast<RuTextureChannel>(ruTEXCHANNEL_PAPERDOLL_MASK_0 + i));
		}

		if(dyeMasksReady)
		{
/*
			// Determine mask texture size & levels
			INT32 maskTextureWidth = 0;
			INT32 maskTextureHeight = 0;
			INT32 maskTextureLevels = 0;

			for(INT32 layer = 0; layer < 4; ++layer)
			{
				IRuTexture *srcTexture = (IRuTexture *) material->GetTexture(static_cast<RuTextureChannel>(ruTEXCHANNEL_PAPERDOLL_MASK_0 + layer));
				if(srcTexture)
				{
					maskTextureWidth = srcTexture->GetLevelWidth(0);
					maskTextureHeight = srcTexture->GetLevelHeight(0);
					maskTextureLevels = srcTexture->GetLevelCount();
					break;
				}
			}
*/
			// Strip extension from the texture name if necessary
			CRuString diffuseTextureName = diffuseTexture->GetTextureName();

			INT32 lastSlashIndex = diffuseTextureName.LastIndexOf("\\");
			INT32 lastPeriodIndex = diffuseTextureName.LastIndexOf(".");

			if(lastPeriodIndex > lastSlashIndex)
			{
				diffuseTextureName.TruncateAt(lastPeriodIndex);
			}

			// Build full dye mask texture path
			INT32 compMaskTexNameLen = diffuseTextureName.GetLength() + (INT32)strlen("_compmask") + 1;
			char *compMaskTexName = ruNEW char [compMaskTexNameLen];
			sprintf(compMaskTexName, "%s_compmask", static_cast<char *>(diffuseTextureName));

			// Attempt to load the composited texture from the resource manager
			IRuTexture *compLayerMaskTex = g_ruResourceManager->LoadTexture(compMaskTexName);
			if(compLayerMaskTex == NULL)
			{
				INT32 maskTextureWidth = 0;
				INT32 maskTextureHeight = 0;
				INT32 maskTextureLevels = 0;

				for(INT32 layer = 0; layer < 4; ++layer)
				{
					IRuTexture *srcTexture = (IRuTexture *) material->GetTexture(static_cast<RuTextureChannel>(ruTEXCHANNEL_PAPERDOLL_MASK_0 + layer));
					if(srcTexture && !srcTexture->IsDiscarded() && !srcTexture->IsInvalidated())
					{
						maskTextureWidth = max(maskTextureWidth, (INT32)srcTexture->GetLevelWidth(0));
						maskTextureHeight = max(maskTextureHeight, (INT32)srcTexture->GetLevelHeight(0));
						maskTextureLevels = max(maskTextureLevels, (INT32)srcTexture->GetLevelCount());
					}
				}

				if(maskTextureWidth <= 0 || maskTextureHeight <= 0)
				{
					// Create the composite mask texture
					compLayerMaskTex = ruNEW CRuTexture_Proxy();
					static_cast<CRuTexture_Proxy *>(compLayerMaskTex)->Allocate(1, 1, 1, ruSURFACEFMT_A4R4G4B4);
					compLayerMaskTex->SetTextureName(compMaskTexName);
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
					compLayerMaskTex->SetTextureName(compMaskTexName);
					g_ruResourceManager->AddTexture(compLayerMaskTex);

					for(INT32 layer = 0; layer < 4; ++layer)
					{
						// Initialize layer exist boolean to false
						char keyString[64];
						sprintf(keyString, "material:dyeFX layer %d exists", layer + 1);
						compLayerMaskTex->SetProperty(keyString, FALSE);

						// Get mask texture for current layer
						IRuTexture *srcTexture = (IRuTexture *) material->GetTexture(static_cast<RuTextureChannel>(ruTEXCHANNEL_PAPERDOLL_MASK_0 + layer));

						if(srcTexture)
						{
							// Process the mask texture only if it is a valid texture
							if(!srcTexture->IsDiscarded() && !srcTexture->IsInvalidated())
							{
								INT32 srcLevel = 0;
								INT32 destLevel = 0;

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

								compLayerMaskTex->SetProperty(keyString, TRUE);
							}

							// Purge the layer texture
							g_ruResourceManager->PurgeTexture(srcTexture);

							// Discard layer texture
							material->SetTexture(static_cast<RuTextureChannel>(ruTEXCHANNEL_PAPERDOLL_MASK_0 + layer), NULL);
						}
					}
				}
			}

			// Set composite color dye mask texture into material
			material->SetTexture(ruTEXCHANNEL_PAPERDOLL_MASK_COMPOSITE, compLayerMaskTex);
			ruSAFE_RELEASE(compLayerMaskTex);

			// Delete allocated memory
			delete[] compMaskTexName;

			return TRUE;
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

BOOL RuMaterial_GetDyeFXColors(IRuMaterial *material, BOOL *layerActivatedOut, RUCOLOR *layerColorOut)
{
	// Retrieve  dye colors and activation states from the material
	for(INT32 i = 0; i < 4; ++i)
	{
		char keyString[64];
		RuPropertyType propertyValue;

		sprintf(keyString, "material:dyeFX layer %d active", i + 1);
		layerActivatedOut[i] = material->GetProperty(keyString, propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;

		sprintf(keyString, "material:dyeFX layer %d color", i + 1);
		layerColorOut[i] = material->GetProperty(keyString, propertyValue) ? boost::get<RUCOLOR>(propertyValue) : 0xFFFFFFFF;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuMaterial_SetDyeFXColors(IRuMaterial *material, const BOOL *layerActivated, const RUCOLOR *layerColor)
{
	IRuVolumeTexture *destColorMapTexture = static_cast<IRuVolumeTexture *>(material->GetTexture(ruTEXCHANNEL_PAPERDOLL_COLORMAP));

	if(destColorMapTexture == NULL)
	{
		IRuBaseTexture *colorMapTexture = ruNEW CRuVolumeTexture_Proxy(1, 1, 4, 1, ruSURFACEFMT_A8R8G8B8);
		material->SetTexture(ruTEXCHANNEL_PAPERDOLL_COLORMAP, colorMapTexture);
		destColorMapTexture = (IRuVolumeTexture *) colorMapTexture;
		ruSAFE_RELEASE(colorMapTexture);
	}

	UINT32 *destBits;
	INT32 destRowPitch, destSlicePitch;

	// Lock destination and source
	destColorMapTexture->Lock(0, (void **) &destBits, &destRowPitch, &destSlicePitch);

	for(INT32 j = 0; j < 4; ++j)
	{
		if(layerActivated[j])
		{
			RuImageFX_Clear_32(static_cast<UINT32 *>(destBits + j * (destSlicePitch / 4)), 0, 0, destColorMapTexture->GetLevelWidth(0), destColorMapTexture->GetLevelHeight(0), destRowPitch, layerColor[j] | 0xFF000000);
		}
		else
		{
			RuImageFX_Clear_32(static_cast<UINT32 *>(destBits + j * (destSlicePitch / 4)), 0, 0, destColorMapTexture->GetLevelWidth(0), destColorMapTexture->GetLevelHeight(0), destRowPitch, 0x00000000);
		}
	}

	// Unlock destination and source
	destColorMapTexture->Unlock(0);

	// Save dye colors and activation states into the material
	for(INT32 i = 0; i < 4; ++i)
	{
		char keyString[64];

		sprintf(keyString, "material:dyeFX layer %d active", i + 1);
		material->SetProperty(keyString, layerActivated[i]);

		sprintf(keyString, "material:dyeFX layer %d color", i + 1);
		material->SetProperty(keyString, layerColor[i]);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuMaterial_GetDyeFXExists(IRuMaterial *material, INT32 layerIdx)
{
	if(material && layerIdx >= 1 && layerIdx <= 4)
	{
		char keyString[64];
		RuPropertyType propertyValue;

		IRuBaseTexture *compMaskTexture = material->GetTexture(ruTEXCHANNEL_PAPERDOLL_MASK_COMPOSITE);

		if(compMaskTexture)
		{
			sprintf(keyString, "material:dyeFX layer %d exists", layerIdx);
			return compMaskTexture->GetProperty(keyString, propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
