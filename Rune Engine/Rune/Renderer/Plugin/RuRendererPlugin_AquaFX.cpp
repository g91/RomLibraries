/*!
	@file RuRendererPlugin_AquaFX.cpp

	Copyright (c) 2004-2007 All rights reserved
*/

#include "../../Renderer/Plugin/RuRendererPlugin_AquaFX.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuRendererPlugin_AquaFX, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuRendererPlugin_AquaFX, "CRuRendererPlugin_AquaFX", "IRuObject")
ruCLASSGUID_DEFINE(CRuRendererPlugin_AquaFX, 0xFA66063A, 0x7F714766, 0xBCCFC828, 0xB617A579)

// ************************************************************************************************************************************************************

CRuRendererPlugin_AquaFX::CRuRendererPlugin_AquaFX()
:	m_effectTextureSize(256),
	m_minimumReflectSize(6.0f),
	m_textureDescriptors(32),
	m_candidateEntities(2048),
	m_dummyCamera(NULL),
	m_rootRendererNode_0(NULL),
	m_rootRendererNode_1(NULL),
	m_rootRendererNode_2(NULL),
	m_texCopyNode(NULL),
	m_waterRenderContext(NULL),
	m_noZRenderContext(NULL)
{
	// Apply settings
	GetSettingsFromEngine();

	// Register listener on engine settings
	g_ruEngineSettings->RegisterSettingsChangeHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_AquaFX::EngineSettingsChangedHandler, this, _1, _2));
}

CRuRendererPlugin_AquaFX::~CRuRendererPlugin_AquaFX()
{
	for(INT32 i = 0; i < m_textureDescriptors.Count(); ++i)
	{
		// Push texture descriptor resources onto unused stack
		m_unusedCameras.Push(m_textureDescriptors[i].m_camera);
		m_unusedParamBlocks.Push(m_textureDescriptors[i].m_paramBlock);
		m_unusedRenderContexts.Push(m_textureDescriptors[i].m_renderContext);
		m_unusedClipPlaneSets.Push(m_textureDescriptors[i].m_clipPlaneSet);
	}

	ruSAFE_RELEASE(m_dummyCamera);
	ruSAFE_RELEASE(m_rootRendererNode_0);
	ruSAFE_RELEASE(m_rootRendererNode_1);
	ruSAFE_RELEASE(m_rootRendererNode_2);
	ruSAFE_RELEASE(m_texCopyNode);
	ruSAFE_RELEASE(m_waterRenderContext);
	ruSAFE_RELEASE(m_noZRenderContext);

	while(m_unusedCameras.IsEmpty() == FALSE)
	{
		CRuCamera *camera = m_unusedCameras.Pop();
		ruSAFE_RELEASE(camera);
	}

	while(m_unusedParamBlocks.IsEmpty() == FALSE)
	{
		CRuParameterBlock *paramBlock = m_unusedParamBlocks.Pop();
		ruSAFE_DELETE(paramBlock);
	}

	while(m_unusedRenderContexts.IsEmpty() == FALSE)
	{
		CRuRendererNode_RenderContext *renderContext = m_unusedRenderContexts.Pop();
		ruSAFE_RELEASE(renderContext);
	}

	while(m_unusedClipPlaneSets.IsEmpty() == FALSE)
	{
		CRuClipPlaneSet *clipPlaneSet = m_unusedClipPlaneSets.Pop();
		ruSAFE_RELEASE(clipPlaneSet);
	}
}

BOOL CRuRendererPlugin_AquaFX::RegisterPlugin(CRuRendererNode_RenderContext *renderContext)
{
	// Register event handlers
	renderContext->Event_Submitted().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_AquaFX::HandleEvent_Submitted, this, _1));
	renderContext->Event_Begin().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_AquaFX::HandleEvent_Begin, this, _1));
	renderContext->Event_Execute().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_AquaFX::HandleEvent_Execute, this, _1));

	// Initialize dummy camera
	m_dummyCamera = ruNEW CRuCamera();

	// Initialize the root renderer node
	m_rootRendererNode_0 = ruNEW CRuRendererNode();
	m_rootRendererNode_0->SetObjectName("AquaFX-0");
	m_rootRendererNode_0->PersistAcrossFrames(FALSE);
	m_rootRendererNode_0->Event_Execute().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_AquaFX::HandleEvent_Execute_2, this, _1));

	m_rootRendererNode_1 = ruNEW CRuRendererNode();
	m_rootRendererNode_1->SetObjectName("AquaFX-1");
	m_rootRendererNode_1->PersistAcrossFrames(FALSE);

	m_rootRendererNode_2 = ruNEW CRuRendererNode();
	m_rootRendererNode_2->SetObjectName("AquaFX-2");
	m_rootRendererNode_2->PersistAcrossFrames(FALSE);

	// Create the texture copy node
	m_texCopyNode = ruNEW CRuRendererNode_TextureCopy();
	m_texCopyNode->PersistAcrossFrames(FALSE);

	// Water surface render context
	m_waterRenderContext = ruNEW CRuRendererNode_RenderContext();
	m_waterRenderContext->PersistAcrossFrames(FALSE);

	// No-Z model render context
	m_noZRenderContext = ruNEW CRuRendererNode_RenderContext();
	m_noZRenderContext->PersistAcrossFrames(FALSE);

	return TRUE;
}

BOOL CRuRendererPlugin_AquaFX::HandleEvent_Submitted(RuEventArgs *eventArgs)
{
	RuRendererEvent_Submitted_Args *submittedArgs = reinterpret_cast<RuRendererEvent_Submitted_Args *>(eventArgs);

	CRuEntity *entity = submittedArgs->m_entity;
	CRuCamera *camera = submittedArgs->m_camera;

	if(entity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
	{
		IRuEntity_Renderable *renderableEntity = static_cast<IRuEntity_Renderable *>(entity);

		// Iterate through every mesh
		BOOL hasFXMap = FALSE;
		INT32 entityClass = 0;

		const CRuSphere &localBounds = entity->GetLocalBounds();

		for(INT32 i = 0, numMeshes = renderableEntity->GetNumMeshes(); i < numMeshes; ++i)
		{
			IRuMesh *mesh = NULL;
			IRuMaterial *material = NULL;

			if(renderableEntity->GetMesh(i, &mesh, &material))
			{
				IRuShader *shader = material->GetShader();

				if(shader)
				{
					// Store entity shader class
					entityClass = max(entityClass, shader->GetShaderClass());

					// Iterate through all samplers
					for(INT32 curSampler = 0; curSampler < 8; ++curSampler)
					{
						RuTextureChannel samplerChannel = shader->GetTextureChannel(0, curSampler);

						switch(samplerChannel)
						{
							case ruTEXCHANNEL_REFLECTIONMAP:
								{
									// Mark the entity as having an effect map
									hasFXMap = TRUE;

									// get clip plane info from somewhere... the material possibly
									CRuPlane4 clipPlane(CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 0.0f));

									UINT64 splitPlane;
									if(renderableEntity->GetParameterBlock()->GetParameter(ruPARAMTYPE_WATER_SPLITPLANE, splitPlane))
									{
										clipPlane = *reinterpret_cast<CRuPlane4 *>(reinterpret_cast<void *>(splitPlane));
									}

									// Search existing texture maps for one that has equivalent split plane (reflection plane)
									BOOL equivalentMapFound = FALSE;
									for(INT32 i = 0; i < m_textureDescriptors.Count(); ++i)
									{
										// If there is an equivalent map, use it
										REAL dotNormal = m_textureDescriptors[i].m_splitPlane.DotNormal(clipPlane);
										if(dotNormal >= 0.9f && fabs(m_textureDescriptors[i].m_splitPlane.m_d - clipPlane.m_d) < 1.0f)
										{
											// Update merged bounding sphere
											m_textureDescriptors[i].m_bodySphere.Merge(renderableEntity->GetWorldBounds());

											// Apply texture to material
											material->SetTexture(samplerChannel, m_textureDescriptors[i].m_renderTarget);
											equivalentMapFound = TRUE;
											break;
										}
									}

									if(equivalentMapFound)
										break;

									// No existing texture map is compatible; allocate a new RT texture now
									IRuTexture *renderTarget = g_ruResourceManager->CreateRenderTarget(m_effectTextureSize, m_effectTextureSize, ruSURFACEFMT_A8R8G8B8);

									if(renderTarget)
									{
										material->SetTexture(samplerChannel, renderTarget);

										// Submit the texture to a list of textures that need to be rendered to
										TextureDescriptor textureDesc;
										textureDesc.m_renderTarget = renderTarget;
										textureDesc.m_mapChannel = samplerChannel;

										textureDesc.m_paramBlock = m_unusedParamBlocks.IsEmpty() ? ruNEW CRuParameterBlock() : m_unusedParamBlocks.Pop();

										// Make a copy of the camera
										textureDesc.m_camera = m_unusedCameras.IsEmpty() ? ruNEW CRuCamera() : m_unusedCameras.Pop();
										*textureDesc.m_camera = *camera;

										// Reflect current camera
										textureDesc.m_camera->Reflect(TRUE, clipPlane);

										textureDesc.m_paramBlock->SetParameter(ruPARAMTYPE_FX_REVERSECULL, TRUE);

										// Store world bounding sphere for the reflection body
										textureDesc.m_bodySphere = renderableEntity->GetWorldBounds();

										// Store clip plane
										textureDesc.m_splitPlane = clipPlane;

										// Create a render context for use with this render target
										textureDesc.m_renderContext = m_unusedRenderContexts.IsEmpty() ? ruNEW CRuRendererNode_RenderContext() : m_unusedRenderContexts.Pop();
										textureDesc.m_renderContext->SetParentRenderer(g_ruRenderer);
										textureDesc.m_renderContext->SetRenderTarget(renderTarget);
										textureDesc.m_renderContext->Camera_Push(textureDesc.m_camera);
										textureDesc.m_renderContext->PersistAcrossFrames(FALSE);

										// Create a clip plane set
										textureDesc.m_clipPlaneSet = m_unusedClipPlaneSets.IsEmpty() ? ruNEW CRuClipPlaneSet() : m_unusedClipPlaneSets.Pop();

										m_textureDescriptors.Add(textureDesc);
									}
									else
									{
										// No RT texture available; set the channel texture to NULL
										material->SetTexture(samplerChannel, NULL);
									}
								}

								break;

							case ruTEXCHANNEL_REFRACTIONMAP:
								{
									// Mark the entity as having an effect map
									hasFXMap = TRUE;

									if(m_refractionTexture)
									{
										material->SetTexture(samplerChannel, m_refractionTexture);
									}
									else
									{
										// No existing refraction map is allocated; allocate a new RT texture now
										IRuTexture *renderTarget = g_ruResourceManager->CreateRenderTarget(m_effectTextureSize, m_effectTextureSize, ruSURFACEFMT_A8R8G8B8);
										m_refractionTexture = renderTarget;

										if(renderTarget)
										{
											material->SetTexture(samplerChannel, renderTarget);

											// Submit the texture to a list of textures that need to be rendered to
											TextureDescriptor textureDesc;
											textureDesc.m_renderTarget = renderTarget;

											// Make a copy of the camera
											textureDesc.m_camera = m_unusedCameras.IsEmpty() ? ruNEW CRuCamera() : m_unusedCameras.Pop();
											*textureDesc.m_camera = *camera;
//											textureDesc.m_camera = camera;
//											textureDesc.m_camera->AddRef();

											textureDesc.m_mapChannel = samplerChannel;

											textureDesc.m_paramBlock = m_unusedParamBlocks.IsEmpty() ? ruNEW CRuParameterBlock() : m_unusedParamBlocks.Pop();

											textureDesc.m_renderContext = NULL;
											textureDesc.m_clipPlaneSet = NULL;

											m_textureDescriptors.Add(textureDesc);
										}
										else
										{
											// No RT texture available; set the channel texture to NULL
											material->SetTexture(samplerChannel, NULL);
										}
									}
								}

								break;
					}
					}
				}
			}
		}

		// If the entity does not require a reflection/refraction map, add it to the list of candidate entities
		if(hasFXMap)
		{
			// Stop submission of the mesh -- this mesh will be submitted by this plugin later
			submittedArgs->m_continueSubmission = FALSE;

			m_waterRenderContext->Camera_Push(submittedArgs->m_camera);
			m_waterRenderContext->Submit(entity);
			m_waterRenderContext->Camera_Pop();

			// A water surface with FX map exists, thus we must proceed with the no-Z render
			m_requiresNoZReRender = TRUE;
		}
		else
		{
			if(localBounds.Radius() > m_minimumReflectSize)
			{
				m_candidateEntities.Add(entity);
			}

			// Entity is supposed to be rendered after the water surface...
			if(entityClass > 1500)
			{
				// NOTE: For models that do not write to Z, we get a visual artifact after rendering the reflective
				//       water surface. This visual artifact results in the no-Z models being behind the water
				//       surface. So here we correct that visual artifact by rendering these no-Z models again.
				m_noZRenderContext->Camera_Push(submittedArgs->m_camera);
				m_noZRenderContext->Submit(entity);
				m_noZRenderContext->Camera_Pop();
			}
		}
	}

	return TRUE;
}

BOOL CRuRendererPlugin_AquaFX::HandleEvent_Begin(RuEventArgs *eventArgs)
{
	// Iterate through texture descriptors and release the cameras
	for(INT32 i = 0; i < m_textureDescriptors.Count(); ++i)
	{
		// Push texture descriptor resources onto unused stack
		m_unusedCameras.Push(m_textureDescriptors[i].m_camera);

		m_unusedParamBlocks.Push(m_textureDescriptors[i].m_paramBlock);

		if(m_textureDescriptors[i].m_renderContext)
			m_unusedRenderContexts.Push(m_textureDescriptors[i].m_renderContext);

		if(m_textureDescriptors[i].m_clipPlaneSet)
			m_unusedClipPlaneSets.Push(m_textureDescriptors[i].m_clipPlaneSet);
	}

	m_requiresNoZReRender = FALSE;
	m_refractionTexture = NULL;

	// Clear texture descriptor list
	m_textureDescriptors.Clear();

	m_candidateEntities.Clear();

	// Attach the root node to the renderer
	g_ruRenderer->AttachRendererNode(m_rootRendererNode_0);
	g_ruRenderer->AttachRendererNode(m_rootRendererNode_1);
	g_ruRenderer->AttachRendererNode(m_rootRendererNode_2);

	// Setup dependency
	g_ruDefaultRenderContext->AddDependency(m_rootRendererNode_0, TRUE);
	m_rootRendererNode_1->AddDependency(g_ruDefaultRenderContext, TRUE);
	m_rootRendererNode_2->AddDependency(m_rootRendererNode_1, TRUE);

	// Attach water render context to the root node
	m_rootRendererNode_2->AddChild(m_waterRenderContext);
	m_waterRenderContext->SetParentRenderer(g_ruRenderer);

	// Attach no-Z model render context to the root node
	m_rootRendererNode_2->AddChild(m_noZRenderContext);
	m_noZRenderContext->SetParentRenderer(g_ruRenderer);

	// Invoke begin on all root renderer nodes
	m_rootRendererNode_0->Begin();
	m_rootRendererNode_1->Begin();
	m_rootRendererNode_2->Begin();

	return TRUE;
}

BOOL CRuRendererPlugin_AquaFX::HandleEvent_Execute(RuEventArgs *eventArgs)
{
	CRuRendererEvent_Execute_Args *executeArgs = static_cast<CRuRendererEvent_Execute_Args *>(eventArgs);

	CRuRendererNode_RenderContext *targetRC = static_cast<CRuRendererNode_RenderContext *>(executeArgs->m_rendererNode);
	IRuObject *rtObject = targetRC->GetRenderTarget();

	if(rtObject->GetType().IsSubClassOf(IRuEngineResource::Type()))
	{
		// Set render target for the water render context
		m_waterRenderContext->SetRenderTarget(rtObject);
		m_waterRenderContext->SetDefaultClearParameters(0, 0, 1.0f, 0);

		// Set render target for the no-Z model render context
		m_noZRenderContext->SetRenderTarget(rtObject);
		m_noZRenderContext->SetDefaultClearParameters(0, 0, 1.0f, 0);

		// If we do not need the no-Z re-render, detach the render context
		if(m_requiresNoZReRender == FALSE)
		{
			m_noZRenderContext->DetachFromParent();
			m_noZRenderContext->End();
		}

		// Prepare refraction texture
		if(m_refractionTexture)
		{
			// Copy source render target onto the refraction texture
			m_texCopyNode->SetSourceTexture(static_cast<IRuEngineResource *>(rtObject));
			m_texCopyNode->SetTargetTexture(m_refractionTexture);
			m_rootRendererNode_1->AddChild(m_texCopyNode);

			m_waterRenderContext->AddDependency(m_texCopyNode, TRUE);
		}
	}


	return TRUE;
}

BOOL CRuRendererPlugin_AquaFX::HandleEvent_Execute_2(RuEventArgs *eventArgs)
{
	BOOL refractionMapRequired = FALSE;

	// Iterate through texture descriptors
	for(INT32 i = 0; i < m_textureDescriptors.Count(); ++i)
	{
		if(m_textureDescriptors[i].m_mapChannel == ruTEXCHANNEL_REFLECTIONMAP)
		{
			CRuRendererNode_RenderContext *renderContext = m_textureDescriptors[i].m_renderContext;

			// Forcibly call begin on render context
			renderContext->Begin();

			// Clip plane....
			CRuPlane4 clipPlane = m_textureDescriptors[i].m_splitPlane * m_textureDescriptors[i].m_camera->GetFrustumTransform();
			m_textureDescriptors[i].m_clipPlaneSet->SetClipPlane(0, clipPlane);
			renderContext->ClipPlaneSet_Push(m_textureDescriptors[i].m_clipPlaneSet);

			renderContext->ParameterBlock_Push(m_textureDescriptors[i].m_paramBlock);

			const CRuMatrix4x4 &cameraTransform = m_textureDescriptors[i].m_camera->GetCameraTransform();

			CRuSphere checkBounds = m_textureDescriptors[i].m_bodySphere;
			cameraTransform.TransformPoint(checkBounds.Center());

			for(INT32 j = 0; j < m_candidateEntities.Count(); ++j)
			{
				// Get candidate entity's reflect class
				INT32 reflectClass = m_candidateEntities[j]->GetOption(ruENTITYOPTION_REFLECTCLASS);
				if(reflectClass < 1)
					continue;

				// NOTE: Add constraint, such that entities IN FRONT OF the water body are simply discarded!
				CRuSphere candidateEntityBounds = m_candidateEntities[j]->GetWorldBounds();

				// Transform candidate entity bounds to view space
				cameraTransform.TransformPoint(candidateEntityBounds.Center());

				// Determine whether to skip
				if((candidateEntityBounds.Center().m_z + candidateEntityBounds.Radius()) < (checkBounds.Center().m_z - checkBounds.Radius()))
				{
					continue;
				}

				BOOL disableClipPlane = m_candidateEntities[j]->GetOption(ruENTITYOPTION_NOCLIP);

				if(disableClipPlane)
				{
					renderContext->ClipPlaneSet_Pop();
					renderContext->Submit(m_candidateEntities[j], TRUE);
					renderContext->ClipPlaneSet_Push(m_textureDescriptors[i].m_clipPlaneSet);
				}
				else
				{
					renderContext->Submit(m_candidateEntities[j], TRUE);
				}
			}

			renderContext->ParameterBlock_Pop();

			renderContext->ClipPlaneSet_Pop();

			m_rootRendererNode_0->AddChild(renderContext);
		}
	}

	return TRUE;
}

void CRuRendererPlugin_AquaFX::GetSettingsFromEngine()
{
	RuPropertyType propertyValue;

	if(g_ruEngineSettings->GetSetting("aquaFX:texture size", propertyValue))
	{
		m_effectTextureSize = boost::get<INT32>(propertyValue);
	}
}

BOOL CRuRendererPlugin_AquaFX::EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting)
{
	GetSettingsFromEngine();
	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
