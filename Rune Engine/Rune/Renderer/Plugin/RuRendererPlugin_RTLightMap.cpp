/*!
	@file RuRendererPlugin_RTLightMap.cpp

	Copyright (c) 2004-2009 Runewaker Entertainment Corp. All rights reserved
*/

#include "../../Renderer/Plugin/RuRendererPlugin_RTLightMap.h"

#include "../../Scene/Terrain/RuEntityOptimalTerrain.h"
#include "../../Scene/Particle/RuEntity_Particle_Visualizer_Sprite.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuRendererPlugin_RTLightMap, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuRendererPlugin_RTLightMap, "CRuRendererPlugin_RTLightMap", "IRuObject")
ruCLASSGUID_DEFINE(CRuRendererPlugin_RTLightMap, 0xD91747FB, 0xD59D4f25, 0x85E03CA3, 0x55BD2247)

// ************************************************************************************************************************************************************

CRuRendererPlugin_RTLightMap::CRuRendererPlugin_RTLightMap()
{
	m_rtLightMapHash = ruNEW CRuHashMap<CRuGUID, RTLightMapDesc>(256);

	m_enabled = FALSE;
	m_rtLightMapSize = 256;
	m_renderTarget = NULL;

	m_commonParamBlock = ruNEW CRuParameterBlock();
	m_commonParamBlock->SetParameter(ruPARAMTYPE_FOG_START, RuCastFloatToINT64(10000.0f));
	m_commonParamBlock->SetParameter(ruPARAMTYPE_FOG_END, RuCastFloatToINT64(10001.0f));

	m_targetEntityParamBlock = ruNEW CRuParameterBlock();
	m_targetEntityParamBlock->SetParameter(ruPARAMTYPE_MATERIAL_COLORWRITEENABLE, FALSE);

	// Apply settings
	GetSettingsFromEngine();

	// Register listener on engine settings
	g_ruEngineSettings->RegisterSettingsChangeHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_RTLightMap::EngineSettingsChangedHandler, this, _1, _2));
}

CRuRendererPlugin_RTLightMap::~CRuRendererPlugin_RTLightMap()
{
	// Unregister event handlers
	g_ruEngine->Event_DeviceReset().UnregisterHandler(this->GetGUID());

	// Process light map descriptors
	m_rtLightMapHash->ForAllElements(boost::bind(&CRuRendererPlugin_RTLightMap::ShutdownCallback, this, _1, _2));

	// Release camera pool
	while(m_unusedCameras.IsEmpty() == FALSE)
	{
		CRuCamera *camera = m_unusedCameras.Pop();
		ruSAFE_RELEASE(camera);
	}

	// Release texture pool
	while(m_unusedTextures.IsEmpty() == FALSE)
	{
		IRuTexture *texture = m_unusedTextures.Pop();
		ruSAFE_RELEASE(texture);
	}

	// Release render contexts
	while(m_unusedRenderContexts.IsEmpty() == FALSE)
	{
		CRuRendererNode_RenderContext *rContext = m_unusedRenderContexts.Pop();
		ruSAFE_RELEASE(rContext);
	}

	while(m_usedRenderContexts.IsEmpty() == FALSE)
	{
		CRuRendererNode_RenderContext *rContext = m_usedRenderContexts.Pop();
		ruSAFE_RELEASE(rContext);
	}

	ruSAFE_RELEASE(m_renderTarget);
	ruSAFE_DELETE(m_rtLightMapHash);
	ruSAFE_DELETE(m_commonParamBlock);
	ruSAFE_DELETE(m_targetEntityParamBlock);
}

BOOL CRuRendererPlugin_RTLightMap::RegisterPlugin(CRuRendererNode_RenderContext *renderContext)
{
	// Register event handlers
	renderContext->Event_Submit().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_RTLightMap::HandleEvent_Submit, this, _1));
	renderContext->Event_Begin().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_RTLightMap::HandleEvent_Begin, this, _1));
	renderContext->Event_Execute().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_RTLightMap::HandleEvent_Execute, this, _1));

	g_ruEngine->Event_DeviceReset().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_RTLightMap::HandleEvent_DeviceReset, this, _1));

	return TRUE;
}

BOOL CRuRendererPlugin_RTLightMap::HandleEvent_Submit(RuEventArgs *eventArgs)
{
	RuRendererEvent_Submit_Args *submitArgs = reinterpret_cast<RuRendererEvent_Submit_Args *>(eventArgs);

	CRuEntity *entity = submitArgs->m_entity;
	CRuCamera *camera = submitArgs->m_camera;

	if(m_enabled == FALSE)
	{
		if(entity->GetType() == CRuOptimalTerrain_Chunk::Type())
		{
			CRuOptimalTerrain_Chunk *chunk = static_cast<CRuOptimalTerrain_Chunk *>(entity);

			for(INT32 i = 0, numMeshes = chunk->GetNumMeshes(); i < numMeshes; ++i)
			{
				IRuMesh *mesh = NULL;
				IRuMaterial *material = NULL;

				if(chunk->GetMesh(i, &mesh, &material))
				{
					IRuShader *shader = material->GetShader();

					if(shader)
					{
						// Iterate through all samplers
						for(INT32 curSampler = 0; curSampler < 8; ++curSampler)
						{
							RuTextureChannel samplerChannel = shader->GetTextureChannel(0, curSampler);

							switch(samplerChannel)
							{
								case ruTEXCHANNEL_RTLIGHTMAP:
									{
										// Apply NULL as texture for the texture channel
										material->SetTexture(samplerChannel, NULL);
									}

									break;
							}
						}
					}
				}
			}
		}

		return TRUE;
	}

	if(entity->GetType().IsSubClassOf(CRuWorld_ObjectManager::Type()))
	{
		m_workStack.Push(entity);

		while(m_workStack.IsEmpty() == FALSE)
		{
			CRuEntity *curEntity = m_workStack.Pop();

			// If the entity is renderable, add to candidates list
			if(curEntity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
			{
				m_candidateEntities.Add(curEntity);
			}

			// If the entity is a particle system, ignore it completely
			if(curEntity->GetType().IsSubClassOf(CRuEntity_Particle_Visualizer_Sprite::Type()))
			{
				continue;
			}

			// Push all children onto stack
			CRuEntity *curChild = curEntity->GetFirstChild();
			while(curChild)
			{
				m_workStack.Push(curChild);
				curChild = curChild->GetNextSibling();
			}
		}
	}

	if(entity->GetType() == CRuOptimalTerrain_Chunk::Type())
	{
		CRuOptimalTerrain_Chunk *chunk = static_cast<CRuOptimalTerrain_Chunk *>(entity);

		for(INT32 i = 0, numMeshes = chunk->GetNumMeshes(); i < numMeshes; ++i)
		{
			IRuMesh *mesh = NULL;
			IRuMaterial *material = NULL;

			if(chunk->GetMesh(i, &mesh, &material))
			{
				IRuShader *shader = material->GetShader();

				if(shader)
				{
					// Iterate through all samplers
					for(INT32 curSampler = 0; curSampler < 8; ++curSampler)
					{
						RuTextureChannel samplerChannel = shader->GetTextureChannel(0, curSampler);

						switch(samplerChannel)
						{
							case ruTEXCHANNEL_RTLIGHTMAP:
								{
									// Look in RTLightMap hash for existing map
									const CRuGUID &key = entity->GetGUID();

									RTLightMapDesc lightMapDesc;

									if(m_rtLightMapHash->ContainsKey(key, key.HashCode()))
									{
										lightMapDesc = m_rtLightMapHash->Get(key, key.HashCode());

										if(lightMapDesc.m_rtLightMap == NULL)
										{
											lightMapDesc.m_rtLightMap = CreateTexture();
											lightMapDesc.m_snapshotTTL = 0;
											m_rtLightMapHash->Replace(key, key.HashCode(), lightMapDesc);
										}

										// Update TTL if necessary
										if(lightMapDesc.m_TTL < 30)
										{
											lightMapDesc.m_TTL = 300;
											m_rtLightMapHash->Replace(key, key.HashCode(), lightMapDesc);
										}
									}
									else
									{
										// Create a descriptor for the entity
										lightMapDesc.m_lightMapCamera = CreateCamera();
										lightMapDesc.m_rtLightMap = CreateTexture();
										lightMapDesc.m_targetEntity = entity;
										lightMapDesc.m_snapshotTTL = 0;
										lightMapDesc.m_TTL = 300;

										// Build orthographic projection for light map camera
										REAL vpWidth = chunk->GetBoundingBox().Maxima().m_x - chunk->GetBoundingBox().Minima().m_x;
										REAL vpHeight = chunk->GetBoundingBox().Maxima().m_z - chunk->GetBoundingBox().Minima().m_z;
										CRuVector3 entityCenter = (chunk->GetBoundingBox().Maxima() + chunk->GetBoundingBox().Minima()) / 2.0f;
										lightMapDesc.m_lightMapCamera->SetOrthographicProjection(1.0f, 10000.0f, vpWidth, vpHeight);
										lightMapDesc.m_lightMapCamera->SetCameraPosition(entityCenter + CRuVector3(0.0f, 5000.0f, 0.0f));
										lightMapDesc.m_lightMapCamera->SetTargetPosition(entityCenter);
										lightMapDesc.m_lightMapCamera->SetCameraUpVector(CRuVector3(0.0f, 0.0f, 1.0f));

										entity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_RTLightMap::HandleEvent_Dispose, this, _1));

										// Insert descriptor into the RTLightMap hash
										m_rtLightMapHash->Add(key, key.HashCode(), lightMapDesc);
									}

									// Apply RTLightMap as texture for the texture channel
									material->SetTexture(samplerChannel, lightMapDesc.m_rtLightMap);
								}

								break;
						}
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL CRuRendererPlugin_RTLightMap::HandleEvent_Begin(RuEventArgs *eventArgs)
{
	m_blargh = 0;

	// Clear list of candidate entities
	m_candidateEntities.Clear();
	m_rootCandidates.Clear();

	// Push all used render contexts onto the unused stack
	while(m_usedRenderContexts.IsEmpty() == FALSE)
	{
		m_unusedRenderContexts.Push(m_usedRenderContexts.Pop());
	}

	// Process light map descriptors
	m_gcDescriptors.Clear();
	m_rtLightMapHash->ForAllElements(boost::bind(&CRuRendererPlugin_RTLightMap::GCCallback, this, _1, _2));

	for(INT32 i = 0; i < m_gcDescriptors.Count(); ++i)
	{
		m_rtLightMapHash->Remove(m_gcDescriptors[i], m_gcDescriptors[i].HashCode());
	}

	return TRUE;
}

BOOL CRuRendererPlugin_RTLightMap::HandleEvent_Execute(RuEventArgs *eventArgs)
{
	if(m_enabled == FALSE)
	{
		return TRUE;
	}

	// Get a render target for use
	if(m_renderTarget == NULL)
	{
		m_renderTarget = g_ruEngine->CreateRenderTarget(m_rtLightMapSize, m_rtLightMapSize, 1, ruSURFACEFMT_A8R8G8B8);
	}

	// Process light map descriptors
	m_rtLightMapHash->ForAllElements(boost::bind(&CRuRendererPlugin_RTLightMap::ExecuteHashCallback, this, _1, _2));

	return TRUE;
}

BOOL CRuRendererPlugin_RTLightMap::HandleEvent_DeviceReset(RuEventArgs *eventArgs)
{
	// Release internal render target
	ruSAFE_RELEASE(m_renderTarget);

	// Release texture pool
	while(m_unusedTextures.IsEmpty() == FALSE)
	{
		IRuTexture *texture = m_unusedTextures.Pop();
		ruSAFE_RELEASE(texture);
	}

	// Process light map descriptors
	m_rtLightMapHash->ForAllElements(boost::bind(&CRuRendererPlugin_RTLightMap::DeviceResetCallback, this, _1, _2));

	return TRUE;
}

BOOL CRuRendererPlugin_RTLightMap::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	RuObjectEvent_Dispose_Args *disposeArgs = static_cast<RuObjectEvent_Dispose_Args *>(eventArgs);

	const CRuGUID &key = disposeArgs->m_object->GetGUID();

	if(m_rtLightMapHash->ContainsKey(key, key.HashCode()))
	{
		RTLightMapDesc rtLightMapDesc = m_rtLightMapHash->Get(key, key.HashCode());

		m_unusedCameras.Push(rtLightMapDesc.m_lightMapCamera);
		m_unusedTextures.Push(rtLightMapDesc.m_rtLightMap);

		m_rtLightMapHash->Remove(key, key.HashCode());
	}

	return TRUE;
}

CRuCamera *CRuRendererPlugin_RTLightMap::CreateCamera()
{
	CRuCamera *camera = NULL;

	if(m_unusedCameras.IsEmpty())
	{
		camera = ruNEW CRuCamera();
	}
	else
	{
		camera = m_unusedCameras.Pop();
	}

	return camera;
}

IRuTexture *CRuRendererPlugin_RTLightMap::CreateTexture()
{
	IRuTexture *texture = NULL;

	if(m_unusedTextures.IsEmpty())
	{
		texture = g_ruEngine->CreateRenderTarget(m_rtLightMapSize, m_rtLightMapSize, 1, ruSURFACEFMT_A8R8G8B8);
	}
	else
	{
		texture = m_unusedTextures.Pop();
	}

	return texture;
}

CRuRendererNode_RenderContext *CRuRendererPlugin_RTLightMap::CreateRenderContext()
{
	CRuRendererNode_RenderContext *renderContext = NULL;

	// Get render context whichever way possible
	if(m_unusedRenderContexts.IsEmpty())
	{
		renderContext = ruNEW CRuRendererNode_RenderContext();
	}
	else
	{
		renderContext = m_unusedRenderContexts.Pop();
	}

	// Push render context onto stack of used render contexts
	m_usedRenderContexts.Push(renderContext);

	return renderContext;
}

BOOL CRuRendererPlugin_RTLightMap::ExecuteHashCallback(const void *key, void *data)
{
	RTLightMapDesc &lightMapDesc = *reinterpret_cast<RTLightMapDesc *>(data);

	// Check light map descriptor for things that need to be updated, discarded, etc
	if(lightMapDesc.m_snapshotTTL <= 0 && m_blargh <= 10)
	{
		++m_blargh;

		if(lightMapDesc.m_rtLightMap == NULL)
		{
			lightMapDesc.m_rtLightMap = CreateTexture();
		}

		// Setup render context
		CRuRendererNode_RenderContext *renderContext = CreateRenderContext();
		renderContext->SetObjectName("RTLightMap-Main Render");
		renderContext->SetParentRenderer(g_ruRenderer);
		renderContext->SetRenderTarget(lightMapDesc.m_rtLightMap);
		renderContext->SetDefaultClearParameters(RUCLEAR_TARGET | RUCLEAR_ZBUFFER | RUCLEAR_STENCIL, 0xFFFFFFFF, 1.0f, 0);
		renderContext->Camera_Push(lightMapDesc.m_lightMapCamera);
		renderContext->PersistAcrossFrames(FALSE);
		renderContext->EnableLighting(FALSE);
//		renderContext->EnableCulling(FALSE);

		// Forcibly call begin on render context
		renderContext->Begin();

		// Suppress submitted event
		renderContext->EventSuppression_Push(ruENTITYEVENT_BROADCAST_SUBMITTED);

		// Push common parameter block into render context
		renderContext->ParameterBlock_Push(m_commonParamBlock);

		// Lay down Z (need to override priority)
		renderContext->ParameterBlock_Push(m_targetEntityParamBlock);
		renderContext->Submit(lightMapDesc.m_targetEntity, TRUE);
		renderContext->ParameterBlock_Pop();

		// Submit all candidate entities for rendering
		for(INT32 i = 0; i < m_candidateEntities.Count(); ++i)
		{
			renderContext->Submit(m_candidateEntities[i], TRUE);
		}

		// Pop parameter block
		renderContext->ParameterBlock_Pop();

		// Pop event suppression
		renderContext->EventSuppression_Pop();

		// Begin isolated UI rendering
		g_ruUIRenderer->IsolationRender_Begin();

		// Set rect sizes
		CRuRectangle srcRect, destRect;
		srcRect.Set(0.0f, 0.0f, static_cast<REAL>(m_rtLightMapSize), static_cast<REAL>(m_rtLightMapSize));
		destRect.Set(0.0f, 0.0f, static_cast<REAL>(m_rtLightMapSize), static_cast<REAL>(m_rtLightMapSize));

		// Push dummy camera into renderer
		g_ruUIRenderer->Camera_Push(lightMapDesc.m_lightMapCamera);

		// Disable 2D viewport scale
		g_ruUIRenderer->ViewportScale_Push(0.0f, 0.0f);

		// Set 2D output mode to direct addressing mode
		g_ruUIRenderer->SetUVMode(ru2DUVMODE_DIRECT);

		// Blur render target texture
		for(INT32 i = 0; i < 8; ++i)
		{
			// Pick target texture (scratch texture 1) and begin 2D rendering
			g_ruUIRenderer->RenderTarget_Push(m_renderTarget);

			// Set up Gaussian filter
			g_ruUIRenderer->SetEffectType(ru2DFXTYPE_GAUSSIAN);
			g_ruUIRenderer->SetEffectParameter(ru2DFXPARAM_COLOR, RUCOLOR_ARGB(255, 255, 255, 255));
			g_ruUIRenderer->SetTexture(lightMapDesc.m_rtLightMap, FALSE);
			g_ruUIRenderer->SetShaderParameter(ruPARAMTYPE_DWORD_0, 0);

			// Submit and flush
			g_ruUIRenderer->Submit(srcRect, destRect);
			g_ruUIRenderer->RenderTarget_Pop();

			// Pick target texture (scratch texture 0) and begin 2D rendering
			g_ruUIRenderer->RenderTarget_Push(lightMapDesc.m_rtLightMap);

			// Set up Gaussian filter
			g_ruUIRenderer->SetEffectType(ru2DFXTYPE_GAUSSIAN);
			g_ruUIRenderer->SetEffectParameter(ru2DFXPARAM_COLOR, RUCOLOR_ARGB(255, 255, 255, 255));
			g_ruUIRenderer->SetTexture(m_renderTarget, FALSE);
			g_ruUIRenderer->SetShaderParameter(ruPARAMTYPE_DWORD_0, 1);

			// Submit and flush
			g_ruUIRenderer->Submit(srcRect, destRect);
			g_ruUIRenderer->RenderTarget_Pop();
		}

		// Set 2D output mode to offset mode (default setting)
		g_ruUIRenderer->SetUVMode(ru2DUVMODE_OFFSET);

		// Restore 2D viewport scale
		g_ruUIRenderer->ViewportScale_Pop();

		// Terminate isolation render
		CRuRendererNode *fxRendererNode = g_ruUIRenderer->IsolationRender_End();
		fxRendererNode->PersistAcrossFrames(FALSE);
		fxRendererNode->AddDependency(renderContext, TRUE);

		// Dependencies to the render context...
		CRuRendererNode *rootNode = ruNEW CRuRendererNode();
		rootNode->SetObjectName("RTLightMap-Root");
		rootNode->PersistAcrossFrames(FALSE);
		rootNode->AddChild(renderContext);
		rootNode->AddChild(fxRendererNode);

		g_ruDefaultRenderContext->AddChild(rootNode);

		ruSAFE_RELEASE(fxRendererNode);
		ruSAFE_RELEASE(rootNode);

		// Presumably, we need to post-process the light map so all darkened areas should be black.. or maybe we just set lighting to zero to be easier

		// Update snapshot TTL for the light map to a fixed value plus random fluctuation
		lightMapDesc.m_snapshotTTL = 300 + m_randGen.GenerateUINT32() % 600;
	}

	return TRUE;
}

BOOL CRuRendererPlugin_RTLightMap::DeviceResetCallback(const void *key, void *data)
{
	RTLightMapDesc &lightMapDesc = *reinterpret_cast<RTLightMapDesc *>(data);

	// Release RTLightMap
	ruSAFE_RELEASE(lightMapDesc.m_rtLightMap);

	// Set TTL to zero
	lightMapDesc.m_snapshotTTL = 0;

	return TRUE;
}

BOOL CRuRendererPlugin_RTLightMap::GCCallback(const void *key, void *data)
{
	RTLightMapDesc &lightMapDesc = *reinterpret_cast<RTLightMapDesc *>(data);

	// Deduct TTL
	--lightMapDesc.m_snapshotTTL;
	--lightMapDesc.m_TTL;

	// If RTLightMapDesc TTL is zero or less, time to clean up
	if(lightMapDesc.m_TTL <= 0)
	{
		// Add to list of descriptors to clean up
		m_gcDescriptors.Add(lightMapDesc.m_targetEntity->GetGUID());

		// Free descriptor resources
		m_unusedCameras.Push(lightMapDesc.m_lightMapCamera);
		lightMapDesc.m_lightMapCamera = NULL;

		m_unusedTextures.Push(lightMapDesc.m_rtLightMap);
		lightMapDesc.m_rtLightMap = NULL;
	}

	return TRUE;
}

BOOL CRuRendererPlugin_RTLightMap::ShutdownCallback(const void *key, void *data)
{
	RTLightMapDesc &lightMapDesc = *reinterpret_cast<RTLightMapDesc *>(data);

	// Unregister dispose handler from the target entity
	lightMapDesc.m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());

	// Push camera onto unused stack
	m_unusedCameras.Push(lightMapDesc.m_lightMapCamera);
	lightMapDesc.m_lightMapCamera = NULL;

	// Push RTLightMap onto unused stack
	m_unusedTextures.Push(lightMapDesc.m_rtLightMap);
	lightMapDesc.m_rtLightMap = NULL;

	return TRUE;
}

void CRuRendererPlugin_RTLightMap::GetSettingsFromEngine()
{
	RuPropertyType propertyValue;

	if(g_ruEngineSettings->GetSetting("RTLightMap:enable", propertyValue))
	{
		m_enabled = boost::get<BOOL>(propertyValue);
	}

	if(g_ruEngineSettings->GetSetting("RTLightMap:texture size", propertyValue))
	{
		INT32 newTextureSize = boost::get<INT32>(propertyValue);

		if(m_rtLightMapSize != newTextureSize)
		{
			m_rtLightMapSize = newTextureSize;

			// Invalidate everything
			HandleEvent_DeviceReset(NULL);
		}
	}
}

BOOL CRuRendererPlugin_RTLightMap::EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting)
{
	GetSettingsFromEngine();
	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
