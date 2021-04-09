/*!
	@file RuRenderer.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../Renderer/RuRenderer.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuRenderer, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuRenderer, "CRuRenderer", "IRuObject")
ruCLASSGUID_DEFINE(CRuRenderer, 0xC14D6BEC, 0xC7704353, 0x87DA4271, 0x8F1A397B)

// ************************************************************************************************************************************************************

CRuRenderer::CRuRenderer()
:	m_rootRendererNode(NULL),
	m_nullTexture(NULL),
	m_blackTexture(NULL),
	m_clampTexture(NULL),
	m_grid1x1Texture(NULL),
	m_executionMap(512)
{
	// Create root renderer node
	m_rootRendererNode = ruNEW CRuRendererNode();
	m_rootRendererNode->SetObjectName("Root Renderer Node");

	// Set default parameters
	m_defaultParamBlock.SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(1.0f));
	m_defaultParamBlock.SetParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, 0xFFFFFFFF);

	m_defaultParamBlock.SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(0.0f));

	m_defaultParamBlock.SetParameter(ruPARAMTYPE_MATERIAL_2SIDED, FALSE);
	m_defaultParamBlock.SetParameter(ruPARAMTYPE_MATERIAL_FACETED, FALSE);
	m_defaultParamBlock.SetParameter(ruPARAMTYPE_MATERIAL_WIRE, FALSE);
	m_defaultParamBlock.SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, TRUE);
	m_defaultParamBlock.SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 150);

	m_defaultParamBlock.SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, ruALPHAMODE_NORMAL);

	// Set default fog
	REAL fogStart = 100.0f, fogEnd = 1500.0f;
	RUCOLOR fogColor = RUCOLOR_ARGB(255, 100, 100, 100);
	m_defaultParamBlock.SetParameter(ruPARAMTYPE_FOG_START, RuCastFloatToINT64(fogStart));
	m_defaultParamBlock.SetParameter(ruPARAMTYPE_FOG_END, RuCastFloatToINT64(fogEnd));

	// Reverse cull disabled by default
	m_defaultParamBlock.SetParameter(ruPARAMTYPE_FX_REVERSECULL, FALSE);

	// Create skin cache
	m_skinCache = ruNEW CRuSkinCache();

	// Create the light tree
	m_lightTree = ruNEW CRuAABBTree();

	// Create lighting info hash
	m_lightingInfoHash = ruNEW CRuHashMap<CRuGUID, RuLightingInfo *>(2048);

	// Setup time counters
	m_gc_LastGCTime = GetTickCount();
	m_gc_ElapsedTTL = 0;

	// Initialize the NULL texture
	if(m_nullTexture == NULL)
	{
		// Allocate texture and add to dictionary
		m_nullTexture = ruNEW CRuTexture_Proxy();
		m_nullTexture->Allocate(1, 1, 1, ruSURFACEFMT_A8R8G8B8);
		m_nullTexture->SetTextureName("RUSYSTEM_NULL");
		g_ruResourceManager->AddTexture(m_nullTexture);

		// Disable garbage collection
		g_ruResourceManager->EnableTextureGarbageCollection(m_nullTexture, FALSE);

		// Pin texture in cache
		g_ruResourceManager->PinTexture(m_nullTexture, TRUE);

		DWORD *pixels;
		INT32 pitch;

		if(m_nullTexture->Lock(0, (void **) &pixels, &pitch))
		{
			pixels[0] = 0xFFFFFFFF;

			m_nullTexture->Unlock(0);
		}
	}

	if(m_blackTexture == NULL)
	{
		// Allocate texture and add to dictionary
		m_blackTexture = ruNEW CRuTexture_Proxy();
		m_blackTexture->Allocate(1, 1, 1, ruSURFACEFMT_A8R8G8B8);
		m_blackTexture->SetTextureName("RUSYSTEM_BLACK");
		g_ruResourceManager->AddTexture(m_blackTexture);

		// Disable garbage collection
		g_ruResourceManager->EnableTextureGarbageCollection(m_blackTexture, FALSE);

		// Pin texture in cache
		g_ruResourceManager->PinTexture(m_blackTexture, TRUE);

		DWORD *bits;
		INT32 pitch;

		if(m_blackTexture->Lock(0, reinterpret_cast<void **>(&bits), &pitch))
		{
			bits[0] = 0;

			m_blackTexture->Unlock(0);
		}
	}

	// Initialize the CLAMP texture
	if(m_clampTexture == NULL)
	{
		// Allocate texture and add to dictionary
		m_clampTexture = ruNEW CRuTexture_Proxy();
		m_clampTexture->Allocate(128, 128, 1, ruSURFACEFMT_A8R8G8B8);
		m_clampTexture->SetTextureName("RUSYSTEM_CLAMP");
		g_ruResourceManager->AddTexture(m_clampTexture);

		// Disable garbage collection
		g_ruResourceManager->EnableTextureGarbageCollection(m_clampTexture, FALSE);

		// Pin texture in cache
		g_ruResourceManager->PinTexture(m_clampTexture, TRUE);

		DWORD *pixels;
		INT32 pitch;

		if(m_clampTexture->Lock(0, (void **) &pixels, &pitch))
		{
			for(INT32 z = 0; z < 128; ++z)
			{
				for(INT32 x = 0; x < 128; ++x)
				{
					if(z == 0 || x == 0 || z == 127 || x == 127)
						pixels[x + z * pitch / 4] = 0x00000000;
					else
						pixels[x + z * pitch / 4] = 0xFFFFFFFF;
				}
			}

			m_clampTexture->Unlock(0);
		}
	}

	if(m_grid1x1Texture == NULL)
	{
		// Allocate texture and add to dictionary
		m_grid1x1Texture = ruNEW CRuTexture_Proxy();
		m_grid1x1Texture->Allocate(128, 128, 1, ruSURFACEFMT_A8R8G8B8);
		m_grid1x1Texture->SetTextureName("RUSYSTEM_GRID1X1");
		g_ruResourceManager->AddTexture(m_grid1x1Texture);

		// Disable garbage collection
		g_ruResourceManager->EnableTextureGarbageCollection(m_grid1x1Texture, FALSE);

		// Pin texture in cache
		g_ruResourceManager->PinTexture(m_grid1x1Texture, TRUE);

		DWORD *pixels;
		INT32 pitch;

		if(m_grid1x1Texture->Lock(0, (void **) &pixels, &pitch))
		{
			for(INT32 z = 0; z < 128; ++z)
			{
				for(INT32 x = 0; x < 128; ++x)
				{
					if(z == 0 || x == 0 || z == 127 || x == 127)
						pixels[x + z * pitch / 4] = 0xFFFFFFFF;
					else
						pixels[x + z * pitch / 4] = 0xFF999999;
				}
			}

			m_grid1x1Texture->Unlock(0);
		}
	}

/*
	SH9 Constants
	-------------

	normalisation = PI * 16 / 17
	const1 = 0.282095^2 * normalisation * 1
	const2 = 0.488603^2 * normalisation * (2 / 3)
	const3 = 1.092548^2 * normalisation * (1 / 4)
	const4 = 0.315392^2 * normalisation * (1 / 4)
	const5 = 0.546274^2 * normalisation * (1 / 4)
*/

	// Pre-calculate 3rd-order spherical harmonic constants
	REAL normalization = PI * 16.0f / 17.0f;
	m_sh9_Const1 = 0.282095f * 0.282095f * normalization * 1.0f;
	m_sh9_Const2 = 0.488603f * 0.488603f * normalization * (2.0f /3.0f);
	m_sh9_Const3 = 1.092548f * 1.092548f * normalization * (1.0f /4.0f);
	m_sh9_Const4 = 0.315392f * 0.315392f * normalization * (1.0f /4.0f);
	m_sh9_Const5 = 0.546274f * 0.546274f * normalization * (1.0f /4.0f);

	// Schedule garbage collect function
	g_ruFunctionSchedule->ScheduleFunction(m_guid, 0, 0, 0, boost::bind(&CRuRenderer::GarbageCollect, this, _1), NULL);
}

CRuRenderer::~CRuRenderer()
{
	// Clear the lighting info hash
	ClearLightingInfoHash();

	// Delete lighting info hash
	delete m_lightingInfoHash;

	for(INT32 i = 0; i < m_recycledLightEntities.Count(); ++i)
	{
		ruSAFE_RELEASE(m_recycledLightEntities[i]);
	}
	m_recycledLightEntities.Clear();

	// Free lighting tree
	ruSAFE_RELEASE(m_lightTree);

	// Free skin cache
	ruSAFE_RELEASE(m_skinCache);

	// Release root renderer node
	ruSAFE_RELEASE(m_rootRendererNode);

	// Release system textures
	ruSAFE_RELEASE(m_nullTexture);
	ruSAFE_RELEASE(m_blackTexture);
	ruSAFE_RELEASE(m_clampTexture);
	ruSAFE_RELEASE(m_grid1x1Texture);
}

BOOL CRuRenderer::AttachRendererNode(CRuRendererNode *rendererNode)
{
	// Set parent renderer to self
	rendererNode->SetParentRenderer(this);

	// Attach renderer node to the root renderer node
	m_rootRendererNode->AddChild(rendererNode);

	return TRUE;
}

BOOL CRuRenderer::DetachRendererNode(CRuRendererNode *rendererNode)
{
	// Reset parent renderer to NULL
	rendererNode->SetParentRenderer(NULL);

	// Detach renderer node from the root renderer node
	m_rootRendererNode->RemoveChild(rendererNode);

	return TRUE;
}

void CRuRenderer::Begin()
{
	// Increment eviction marker on the 2D texture atlas to allow eviction of old textures
//	m_ru2D_TextureAtlas->IncrementEvictionMarker();

	// Reset render target pools
	g_ruResourceManager->ResetRenderTargetPools();
	g_ruResourceManager->ResetRenderTargetViews();

	// Garbage collect the skinning cache
	m_skinCache->GarbageCollectSkinningCache();

	// Call begin on root renderer node
	m_rootRendererNode->Begin();

	// Increment iteration number
	++m_iterationNumber;
}

void CRuRenderer::End()
{
	// Call execute on root renderer node
	m_rootRendererNode->Execute();

	// Call end on root renderer node
	m_rootRendererNode->End();

	// Detach all non-persistent nodes
	m_rootRendererNode->DetachNonPersistentChildren();
}

void CRuRenderer::SetLightingProp(CRuEntity *lightingProp)
{
	INT32 lightCount = 0;

	// Clear old lighting tree
	m_lightTree->Clear();

	// Parse the lighting prop and extract lights
	m_entityStack.Clear();
	m_entityStack.Push(lightingProp);

	// Enumerate lights
	while(!m_entityStack.IsEmpty())
	{
		CRuEntity *curEntity = m_entityStack.Pop();

		if(curEntity->GetType().IsSubClassOf(IRuEntity_Container::Type()))
		{
			// Push all entities held by the frame onto the entity stack
			IRuEntity_Container *frame = (IRuEntity_Container *) curEntity;
			for(INT32 i = 0; i < frame->GetNumEntities(); ++i)
			{
				// Push entity onto entity stack
				m_entityStack.Push(frame->GetEntity(i));
			}
		}
		else if(curEntity->GetType().IsSubClassOf(CRuEntity_Light::Type()))
		{
			CRuEntity_Light *light;

			// Try to recycle a light entity from our existing list
			if(lightCount < m_recycledLightEntities.Count())
			{
				light = m_recycledLightEntities[lightCount];
			}
			// Otherwise create a new light entity
			else
			{
				light = ruNEW CRuEntity_Light();
				m_recycledLightEntities.Add(light);
			}

			// Set light parameters
			light->SetLight(((CRuEntity_Light *) curEntity)->GetLight());

			// Append light to tree and increment light count
			m_lightTree->AddEntity(light);
			++lightCount;
		}
	}
}

void CRuRenderer::SetSpecularColor(DWORD specularColor)
{
	m_specularColor = specularColor;
}

void CRuRenderer::SetSpecularDirection(const CRuVector3 &specularDir)
{
	m_specularDirection = specularDir;
}

void CRuRenderer::AddtoSH9(CRuColor4 *shCoefficients, CRuColor4 &lightColor, CRuVector3 &dirN)
{
	shCoefficients[0] += lightColor * m_sh9_Const1;
	shCoefficients[1] += lightColor * (m_sh9_Const2 * dirN.x);
	shCoefficients[2] += lightColor * (m_sh9_Const2 * dirN.y);
	shCoefficients[3] += lightColor * (m_sh9_Const2 * dirN.z);
	shCoefficients[4] += lightColor * m_sh9_Const3 * (dirN.x * dirN.z);
	shCoefficients[5] += lightColor * m_sh9_Const3 * (dirN.z * dirN.y);
	shCoefficients[6] += lightColor * m_sh9_Const3 * (dirN.y * dirN.x);
	shCoefficients[7] += lightColor * m_sh9_Const4 * (3.0f * dirN.z * dirN.z - 1.0f);
	shCoefficients[8] += lightColor * m_sh9_Const5 * (dirN.x * dirN.x - dirN.y * dirN.y);
}

RuLightingInfo *CRuRenderer::BuildLightingInfo(const CRuGUID &guid, const CRuMatrix4x4 &invWorldTransform, const CRuSphere &worldBounds)
{
	// Retrieve existing lighting info using the GUID as key
	RuLightingInfo *lightInfo = NULL;

	if(m_lightingInfoHash->Get(guid, guid.HashCode(), lightInfo) == FALSE)
	{
		// Create a new lighting info structure
		lightInfo = ruNEW RuLightingInfo();
		lightInfo->m_sceneIteration = m_iterationNumber - 1;
		lightInfo->m_entityIteration = 0;
		lightInfo->m_lightingIteration = m_iterationNumber - 1;

		// Insert lighting info into hash
		m_lightingInfoHash->Add(guid, guid.HashCode(), lightInfo);
	}

	// Reset TTL (Preset to 15 seconds)
	lightInfo->m_TTL = 15000;

	// If the lighting info is not at the current iteration, rebuild it
//	if(lightInfo->m_entityIteration != renderableEntity->GetIteration() || lightInfo->m_lightingIteration != m_iterationNumber)
	if(lightInfo->m_sceneIteration != m_iterationNumber)
	{
		// Clear existing lighting
		lightInfo->m_directionalLights.Clear();
		lightInfo->m_pointLights.Clear();
		lightInfo->m_spotLights.Clear();

		// Set default ambient level
		lightInfo->m_ambientLight.Set(1.0f, 0.1f, 0.1f, 0.1f);

		// Reset lighting coefficients
		for(INT32 i = 0; i < 9; ++i)
		{
			lightInfo->m_shCoefficients[i].Set(0.0f, 0.0f, 0.0f, 0.0f);
		}

		// Update iteration number
		lightInfo->m_sceneIteration = m_iterationNumber;
//		lightInfo->m_entityIteration = renderableEntity->GetIteration();
//		lightInfo->m_lightingIteration = m_iterationNumber;
//		lightInfo->m_lightingIteration = m_iterationNumber - 1;

		// NOTE: Except for spot lights, it is enough to simply invert the translation. For now, we hack around this. Will need to be changed later.

		// Transform specular position from world space into object space
		CRuVector3 localSpecularDirection;
		invWorldTransform.TransformVector(m_specularDirection, localSpecularDirection);
		lightInfo->m_specularDirection = localSpecularDirection;
		lightInfo->m_specularColor = m_specularColor;

		// Query lighting tree for nearby lights
		m_enumeratedLights.Clear();
		m_enumeratedLightDistances.Clear();
		m_lightTree->GetEntities(worldBounds, m_enumeratedLights);

		// Sort enumerated lights
		for(INT32 i = 0; i < m_enumeratedLights.Count(); ++i)
		{
			const RuLightSettings &light = static_cast<CRuEntity_Light *>(m_enumeratedLights[i])->GetLight();

			switch(light.m_type)
			{
				default:
					{
						m_enumeratedLightDistances.Add(0.0f);
					}

					break;

				case ruLIGHTTYPE_POINT:
					{
						m_enumeratedLightDistances.Add((worldBounds.Center() - light.m_position).Magnitude());
					}

					break;
			}
		}

		m_radixSort2.Sort(m_enumeratedLightDistances.GetArray(), m_enumeratedLightDistances.Count());

		CRuVector3 objSpaceDirection;

		// Sort lights into directional, ambient, point, and spot bins
		UINT32 *sortedIndices = m_radixSort2.GetSortedIndices();
		for(INT32 i = 0; i < m_enumeratedLights.Count(); ++i)
		{
			const RuLightSettings &light = static_cast<CRuEntity_Light *>(m_enumeratedLights[sortedIndices[i]])->GetLight();

			switch(light.m_type)
			{
				case ruLIGHTTYPE_AMBIENT:
					{
						lightInfo->m_ambientLight.Set(light.m_diffuse.GetARGB());
					}

					break;

				case ruLIGHTTYPE_DIRECTIONAL:
					{
						// Add light settings to directional lights
						lightInfo->m_directionalLights.Add(light);


						// *** OLD SH CODE

						// Transform light direction into object space
						invWorldTransform.TransformVector(light.m_direction, objSpaceDirection);

						// Represent directional lights in the SH term
						REAL d = m_enumeratedLightDistances[sortedIndices[i]];
						REAL attenuation = 1.0f - min(1.0f, d / light.m_range);
						AddtoSH9(lightInfo->m_shCoefficients, light.m_diffuse * 1.0f, objSpaceDirection * -1.0f);
					}

					break;

				case ruLIGHTTYPE_POINT:
					{
						// Add light settings to point lights
						lightInfo->m_pointLights.Add(light);

/*
						if(lightInfo->m_numPointLights < 2)
						{
							lightInfo->m_pointLights[lightInfo->m_numPointLights] = light;

							invWorldTransform.TransformPoint(lightInfo->m_pointLights[lightInfo->m_numPointLights].m_position);

							++lightInfo->m_numPointLights;
						}
*/
						// Set one specular light

						// Approximate point lights in the SH term
						REAL d = m_enumeratedLightDistances[sortedIndices[i]];
						REAL attenuation = 1.0f - min(1.0f, d / light.m_range);
						AddtoSH9(lightInfo->m_shCoefficients, light.m_diffuse * attenuation, ((worldBounds.Center() - light.m_position) / d) * -1.0f);
					}

					break;
			}
		}
	}

	return lightInfo;
}

CRuParameterBlock &CRuRenderer::GetDefaultParameterBlock()
{
	return m_defaultParamBlock;
}

CRuSkinCache *CRuRenderer::GetSkinCache()
{
	return m_skinCache;
}

UINT32 CRuRenderer::GetNumPrimitivesRendered()
{
	return 0;
}

UINT32 CRuRenderer::GetNumBatchesIssued()
{
	return 0;
}

BOOL CRuRenderer::ClearLightingInfoHash()
{
	// Clear the lighting info hash
	m_lightingInfoHash->ForAllElements(boost::bind(&CRuRenderer::ClearLightingInfoHashCallback, this, _1, _2));
	m_lightingInfoHash->Clear();

	return TRUE;
}

BOOL CRuRenderer::ClearLightingInfoHashCallback(const void *key, void *data)
{
	// Cast parameters into respective objects
	const CRuGUID &guid = *reinterpret_cast<const CRuGUID *>(key);
	RuLightingInfo *lightingInfo = *reinterpret_cast<RuLightingInfo **>(data);

	// Delete lighting info
	delete lightingInfo;

	return TRUE;
}

BOOL CRuRenderer::GarbageCollect(PTRVALUE userData)
{
	// Update garbage collection time
	DWORD curTime = GetTickCount();
	m_gc_ElapsedTTL = curTime - m_gc_LastGCTime;
	m_gc_LastGCTime = curTime;

	// Garbage collect the lighting info hash
	m_lightingInfoHash->ForAllElements(boost::bind(&CRuRenderer::GarbageCollectLightingInfoCallback, this, _1, _2));

	while(m_gc_LightingInfoGUID.IsEmpty() == FALSE)
	{
		// Remove expired lighting info from the hash
		CRuGUID guid = m_gc_LightingInfoGUID.Dequeue();
		m_lightingInfoHash->Remove(guid, guid.HashCode());
	}

	return TRUE;
}

BOOL CRuRenderer::GarbageCollectLightingInfoCallback(const void *key, void *data)
{
	// Cast parameters into respective objects
	const CRuGUID &guid = *reinterpret_cast<const CRuGUID *>(key);
	RuLightingInfo *lightingInfo = *reinterpret_cast<RuLightingInfo **>(data);

	if(lightingInfo->m_TTL <= m_gc_ElapsedTTL)
	{
		// If the TTL has elapsed, queue for removal and delete the lighting info
		m_gc_LightingInfoGUID.Queue(guid);
		delete lightingInfo;
	}
	else
	{
		// Deduct the elapsed TTL
		lightingInfo->m_TTL -= m_gc_ElapsedTTL;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuRenderer*					g_ruRenderer = NULL;

// ************************************************************************************************************************************************************

#pragma managed(pop)
