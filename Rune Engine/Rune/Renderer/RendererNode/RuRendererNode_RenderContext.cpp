/*!
	@file RuRenderer.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../../Renderer/RendererNode/RuRendererNode_RenderContext.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuRendererNode_RenderContext, CRuRendererNode)
ruHEAP_DEFINE_SUBCLASS(CRuRendererNode_RenderContext, "CRuRendererNode_RenderContext", "CRuRendererNode")
ruCLASSGUID_DEFINE(CRuRendererNode_RenderContext, 0x92734C98, 0x9AE544d9, 0xA6F1353E, 0x5FF06FDB)

// ************************************************************************************************************************************************************

CRuRendererNode_RenderContext::CRuRendererNode_RenderContext()
	: m_renderer(NULL),
	m_renderTarget(NULL),
	m_depthStencilSurface(NULL),
	m_defaultViewport(NULL),
	m_customViewport(NULL),
	m_curClipPlaneSet(NULL),
	m_renderQueue(8),
	m_enableLighting(TRUE),
	m_enableCulling(TRUE),
	m_sortRenderQueue(TRUE)
{
	// Setup probes
	m_probe_3D_Submit = g_ruProfiler->CreateProbe("3D Submit", "Scene");
	m_probe_3D_FaceCount = g_ruProfiler->CreateProbe("3D Faces", "Scene");
	m_probe_3D_BatchCount = g_ruProfiler->CreateProbe("3D Batches", "Scene");
	m_probe_3D_Flush = g_ruProfiler->CreateProbe("3D Flush", "Scene");
	m_probe_3D_Skinning = g_ruProfiler->CreateProbe("Skinning", "Scene");
	m_probe_3D_Morphing = g_ruProfiler->CreateProbe("Morphing", "Scene");
	m_probe_3D_Sorting = g_ruProfiler->CreateProbe("3D Sorting", "Scene");
	m_probe_3D_Parameters = g_ruProfiler->CreateProbe("3D Parameters", "Scene");
	m_probe_3D_Lighting = g_ruProfiler->CreateProbe("3D Lighting", "Scene");
	m_probe_3D_TransformSetup = g_ruProfiler->CreateProbe("Transform Setup", "Scene");
	m_probe_3D_TextureSetup = g_ruProfiler->CreateProbe("Texture Setup", "Scene");
	m_probe_3D_RenderAllPasses = g_ruProfiler->CreateProbe("3D Render Passes", "Scene");
	m_probe_3D_Render = g_ruProfiler->CreateProbe("3D Render", "Scene");
	m_probe_3D_RenderD3D = g_ruProfiler->CreateProbe("3D Render D3D", "Scene");

	m_probe_Engine_InstancingTime = g_ruProfiler->CreateProbe("Engine Instancing Time", "Scene");

	m_probe_Misc_AntiPortalDiscarded = g_ruProfiler->CreateProbe("Anti-Portal Discarded", "Scene");

	m_disableRender = FALSE;
}

CRuRendererNode_RenderContext::~CRuRendererNode_RenderContext()
{
	m_renderer = NULL;
	m_renderTarget = NULL;

	// Release depth stencil surface

	// Release default viewport
	ruSAFE_RELEASE(m_defaultViewport);

	// Release custom viewport
	ruSAFE_RELEASE(m_customViewport);

	// Release matrix stack
	FlushMatrixStack();
	while (m_unusedMatrixStack.IsEmpty() == FALSE)
	{
		delete m_unusedMatrixStack.Pop();
	}

	// Release parameter cache stack
	FlushParameterCacheStack();
	while (m_unusedParameterCacheStack.IsEmpty() == FALSE)
	{
		delete m_unusedParameterCacheStack.Pop();
	}
}

BOOL CRuRendererNode_RenderContext::SetRenderQueueSize(INT32 renderQueueSize)
{
	m_renderQueue.SetNumEntries(0);
	m_renderQueue.SetArraySize(renderQueueSize);
	return TRUE;
}

IRuObject* CRuRendererNode_RenderContext::GetRenderTarget()
{
	return m_renderTarget;
}

BOOL CRuRendererNode_RenderContext::SetRenderTarget(IRuObject* renderTarget)
{
	// Store reference to render target
	m_renderTarget = renderTarget;

	// Create default viewport
	ruSAFE_RELEASE(m_defaultViewport);
	m_defaultViewport = g_ruEngine->CreateViewport(0, 0, 32, 32);

	// Update default viewport parameters
	UpdateDefaultViewportParameters();

	// Set default viewport clear parameters
	m_defaultViewport->SetClearColor(0x00000000);
	m_defaultViewport->SetClearFlags(RUCLEAR_TARGET | RUCLEAR_ZBUFFER | RUCLEAR_STENCIL);
	m_defaultViewport->SetClearStencil(0);
	m_defaultViewport->SetClearZ(1.0f);

	// Clear custom viewport
	ruSAFE_RELEASE(m_customViewport);

	// Clear scissor rect
	m_scissorRect.Set(0.0f, 0.0f, -1.0f, -1.0f);

	return TRUE;
}

BOOL CRuRendererNode_RenderContext::SetViewport(IRuViewport* viewport)
{
	if (viewport)
	{
		viewport->AddRef();
		ruSAFE_RELEASE(m_customViewport);
		m_customViewport = viewport;
	}
	else
	{
		ruSAFE_RELEASE(m_customViewport);
	}

	return TRUE;
}

BOOL CRuRendererNode_RenderContext::SetDefaultClearParameters(DWORD clearFlags, RUCOLOR color, REAL z, DWORD stencil)
{
	if (m_defaultViewport)
	{
		m_defaultViewport->SetClearFlags(clearFlags);
		m_defaultViewport->SetClearColor(color);
		m_defaultViewport->SetClearZ(z);
		m_defaultViewport->SetClearStencil(stencil);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuRendererNode_RenderContext::SetScissorRect(CRuRectangle* scissorRect)
{
	if (scissorRect)
	{
		m_scissorRect = *scissorRect;
	}
	else
	{
		m_scissorRect.Set(0.0f, 0.0f, -1.0f, -1.0f);
	}

	return TRUE;
}

void CRuRendererNode_RenderContext::EnableLighting(BOOL enable)
{
	m_enableLighting = enable;
}

void CRuRendererNode_RenderContext::EnableCulling(BOOL enable)
{
	m_enableCulling = enable;
}

void CRuRendererNode_RenderContext::EnableRenderQueueSorting(BOOL enable)
{
	m_sortRenderQueue = enable;
}

CRuEvent& CRuRendererNode_RenderContext::Event_Submit()
{
	return m_event_Submit;
}

CRuEvent& CRuRendererNode_RenderContext::Event_Submitted()
{
	return m_event_Submitted;
}

CRuEvent& CRuRendererNode_RenderContext::Event_Culled()
{
	return m_event_Culled;
}

void CRuRendererNode_RenderContext::UpdateDefaultViewportParameters()
{
	if (m_renderTarget)
	{
		if (m_renderTarget->GetType().IsSubClassOf(IRuSurface::Type()))
		{
			IRuSurface* surface = static_cast<IRuSurface*>(m_renderTarget);
			m_defaultViewport->SetViewportParameters(0, 0, surface->GetWidth(), surface->GetHeight(), 0.0f, 1.0f);
		}
		else if (m_renderTarget->GetType().IsSubClassOf(IRuSwapChain::Type()))
		{
			IRuSwapChain* swapChain = static_cast<IRuSwapChain*>(m_renderTarget);
			m_defaultViewport->SetViewportParameters(0, 0, swapChain->GetWidth(), swapChain->GetHeight(), 0.0f, 1.0f);
		}
		else if (m_renderTarget->GetType().IsSubClassOf(IRuTexture::Type()))
		{
			IRuTexture* texture = static_cast<IRuTexture*>(m_renderTarget);
			m_defaultViewport->SetViewportParameters(0, 0, texture->GetLevelWidth(0), texture->GetLevelHeight(0), 0.0f, 1.0f);
		}
		else
		{
			// Log an error
		}
	}
}

BOOL CRuRendererNode_RenderContext::ClipPlaneSet_Push(CRuClipPlaneSet* clipPlaneSet)
{
	// Push new clip plane set onto stack
	m_clipPlaneStack.Push(clipPlaneSet);

	return TRUE;
}

BOOL CRuRendererNode_RenderContext::ClipPlaneSet_Pop()
{
	if (m_clipPlaneStack.IsEmpty() == FALSE)
	{
		m_clipPlaneStack.Pop();
		return TRUE;
	}

	return FALSE;
}

BOOL CRuRendererNode_RenderContext::ParameterBlock_Push(CRuParameterBlock* paramBlock)
{
	m_extraParameterBlocks.Add(paramBlock);

	return TRUE;
}

BOOL CRuRendererNode_RenderContext::ParameterBlock_Pop()
{
	if (m_extraParameterBlocks.Count())
	{
		m_extraParameterBlocks.RemoveAt(m_extraParameterBlocks.Count() - 1);
		return TRUE;
	}

	return FALSE;
}

BOOL CRuRendererNode_RenderContext::Camera_Push(CRuCamera* camera)
{
	m_cameraStack.Push(camera);
	return TRUE;
}

BOOL CRuRendererNode_RenderContext::Camera_Pop()
{
	if (m_cameraStack.IsEmpty() == FALSE)
	{
		m_cameraStack.Pop();
		return TRUE;
	}

	return FALSE;
}

BOOL CRuRendererNode_RenderContext::EventSuppression_Push(RuEntityEvent suppressedEvent)
{
	m_eventSuppressionList.Add(suppressedEvent);

	return TRUE;
}

BOOL CRuRendererNode_RenderContext::EventSuppression_Pop()
{
	if (m_eventSuppressionList.Count() > 0)
	{
		RuEntityEvent poppedEvent = m_eventSuppressionList[m_eventSuppressionList.Count() - 1];

		m_eventSuppressionList.RemoveAt(m_eventSuppressionList.Count() - 1);

		return poppedEvent;
	}

	return ruENTITYEVENT_NONE;
}

BOOL CRuRendererNode_RenderContext::MaterialOverrideCallback_Push(CRuRendererNode_RenderContext::MaterialOverrideCallback overrideCallback)
{
	// Push previous callback onto push stack
	m_materialOverrideCallbackStack.Push(m_materialOverrideCallback);

	// Set new callback
	m_materialOverrideCallback = overrideCallback;

	return TRUE;
}

BOOL CRuRendererNode_RenderContext::MaterialOverrideCallback_Pop()
{
	// Assert on stack empty
	assert(m_materialOverrideCallbackStack.IsEmpty() == FALSE);

	// Pop previous callback from push stack
	m_materialOverrideCallback = m_materialOverrideCallbackStack.Pop();

	return TRUE;
}

// NOTE: This function is not re-entrant!!! Should probably correct this problem.
BOOL CRuRendererNode_RenderContext::Submit(CRuEntity* sceneRoot, BOOL noStatusMod)
{
	RUPROFILE_SAMPLE_BEGIN(m_probe_3D_Submit, 2);

	// If the entity is a valid decal target, put it into the list
	if (sceneRoot->GetOption(ruENTITYOPTION_DECALTARGET))
	{
		m_validDecalTargets.Add(sceneRoot);
	}

	// Assertions
	assert(m_cameraStack.IsEmpty() == FALSE);

	// Shortcut to camera
	CRuCamera* camera = m_cameraStack.Peek();

	// Clear entity stack and then push scene root onto entity stack
	m_entityStack.Clear();
	m_entityStack.Push(sceneRoot);

	BOOL eventSurpressed_SceneSubmit = IsEventSuppressed(ruENTITYEVENT_BROADCAST_SUBMITTED);

	// Enumerate objects for rendering
	while (!m_entityStack.IsEmpty())
	{
		CRuEntity* curEntity = m_entityStack.Pop();

		// Notify attached listeners that an entity is being submitted for rendering
		if (eventSurpressed_SceneSubmit == FALSE)
		{
			RuRendererEvent_Submit_Args eventArgs(this, camera, curEntity, TRUE);

			// Notify attached listeners that an entity that it is being submitted for rendering
			m_event_Submit.Trigger(&eventArgs);

			// If we are not to continue with this submission, abort
			if (eventArgs.m_continueSubmission == FALSE)
			{
				continue;
			}
		}

		// Get basic entity options
		BOOL backdropEntity = curEntity->GetOption(ruENTITYOPTION_BACKDROP);
		BOOL ignoreVisibility = curEntity->GetOption(ruENTITYOPTION_IGNOREVISIBILITY);
		RuEntityCullMode entityCullMode = static_cast<RuEntityCullMode>(curEntity->GetOption(ruENTITYOPTION_CULLMODE));

		if (m_enableCulling)
		{
			// Check for visibility in frustum
			if (!ignoreVisibility && entityCullMode == ruENTITYCULLMODE_NORMAL)
			{
				const CRuSphere& worldBounds = curEntity->GetWorldBounds();

				if (!backdropEntity)
				{
					if (IntersectsFrustum(camera->GetFrustumPlanes(), worldBounds) == FALSE)
					{
						// Trigger culled event
						RuRendererEvent_Culled_Args eventArgs(this, camera, curEntity);
						m_event_Culled.Trigger(&eventArgs);

						if (!noStatusMod)
							curEntity->SetStatusFlag(ruENTITYSTATUS_VISIBLE, FALSE);

						continue;
					}
				}
				else
				{
					if (IntersectsFrustum(camera->GetBackdropFrustumPlanes(), worldBounds) == FALSE)
					{
						// Trigger culled event
						RuRendererEvent_Culled_Args eventArgs(this, camera, curEntity);
						m_event_Culled.Trigger(&eventArgs);

						if (!noStatusMod)
							curEntity->SetStatusFlag(ruENTITYSTATUS_VISIBLE, FALSE);

						continue;
					}
				}
			}

			// Mark the entity as visible
			if (!noStatusMod)
				curEntity->SetStatusFlag(ruENTITYSTATUS_VISIBLE, TRUE);

			// If the entity is marked hidden, skip
			if (curEntity->GetOption(ruENTITYOPTION_HIDDEN) || curEntity->GetOption(ruENTITYOPTION_HIDDENBYDISTANCE))
			{
				// Trigger culled event
				RuRendererEvent_Culled_Args eventArgs(this, camera, curEntity);
				m_event_Culled.Trigger(&eventArgs);

				continue;
			}
		}
		else
		{
			// Mark the entity as visible
			if (!noStatusMod)
				curEntity->SetStatusFlag(ruENTITYSTATUS_VISIBLE, TRUE);
		}

		// Broadcast scene submit event to all listeners
		if (eventSurpressed_SceneSubmit == FALSE)
		{
			// Notify entity that it has been submitted for rendering
			RuEntityEvent_Submitted_Args entityEventArgs(this, camera, curEntity, TRUE);
			curEntity->Event_Submitted().Trigger(&entityEventArgs);

			// If we are not to continue with this submission, abort
			if (entityEventArgs.m_continueSubmission == FALSE)
			{
				continue;
			}

			// Notify attached listeners that an entity that it has been submitted for rendering
			RuRendererEvent_Submitted_Args eventArgs(this, camera, curEntity, TRUE);
			m_event_Submitted.Trigger(&eventArgs);

			// If we are not to continue with this submission, abort
			if (eventArgs.m_continueSubmission == FALSE)
			{
				continue;
			}
		}

		// Push all children onto entity stack
		CRuEntity* curChild = curEntity->GetFirstChild();
		while (curChild)
		{
			// Push child onto work stack
			m_entityStack.Push(curChild);

			// Advance to next child
			curChild = curChild->GetNextSibling();
		}

		// If the entity is a subclass of the type portal, set its view camera (TODO: Move this code somewhere else)
		if (curEntity->GetType().IsSubClassOf(CRuContainer_Portal::Type()))
		{
			static_cast<CRuContainer_Portal*>(curEntity)->SetCamera(camera);
		}
		/*
				// If the entity is a subclass of the type anti-portal, insert it into the list of anti-portals
				if(curEntity->GetType().IsSubClassOf(CRuEntity_AntiPortal::Type()))
				{
					m_antiPortals.Add(static_cast<CRuEntity_AntiPortal *>(curEntity));

					// Broadcast acting camera to the entity
					BOOL eventSurpressed_Broadcast_Camera = IsEventSuppressed(ruENTITYEVENT_BROADCAST_CAMERA);

					if(eventSurpressed_Broadcast_Camera == FALSE)
					{
						curEntity->RaiseEvent(NULL, ruENTITYEVENT_BROADCAST_CAMERA, reinterpret_cast<PTRVALUE>(camera), 0, NULL, 0.0f);
					}
				}
		*/
		// If the entity is renderable, process it
		if (curEntity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
		{
			IRuEntity_Renderable* renderableEntity = static_cast<IRuEntity_Renderable*>(curEntity);

			// Create a render queue entry for every mesh in the renderable entity
			for (INT32 i = 0, numMeshes = renderableEntity->GetNumMeshes(); i < numMeshes; ++i)
			{
				// Get the i-th mesh and material
				IRuMesh* mesh = NULL;
				IRuMaterial* material = NULL;

				renderableEntity->GetMesh(i, &mesh, &material);

				// Submit to queue
				Submit_Internal(renderableEntity, NULL, mesh, material, i == 0);
			}
		}
	}

	RUPROFILE_SAMPLE_END(m_probe_3D_Submit, 2);

	return TRUE;
}

BOOL CRuRendererNode_RenderContext::Submit(CRuEntity* sceneRoot, IRuMesh* mesh, IRuMaterial* material, CRuMatrix4x4* worldTransform)
{
	RUPROFILE_SAMPLE_BEGIN(m_probe_3D_Submit, 2);

	// If the entity is a valid decal target, put it into the list
	if (sceneRoot && sceneRoot->GetOption(ruENTITYOPTION_DECALTARGET))
	{
		m_validDecalTargets.Add(sceneRoot);
	}

	BOOL eventSurpressed_SceneSubmit = IsEventSuppressed(ruENTITYEVENT_BROADCAST_SUBMITTED);
	BOOL abortSubmission = FALSE;

	// Broadcast scene submit event to all listeners
	if (eventSurpressed_SceneSubmit == FALSE)
	{
		if (sceneRoot)
		{
			// Notify entity that it has been submitted for rendering
			RuEntityEvent_Submitted_Args entityEventArgs(this, m_cameraStack.Peek(), sceneRoot, TRUE);
			sceneRoot->Event_Submitted().Trigger(&entityEventArgs);

			// If we are not to continue with this submission, abort
			if (entityEventArgs.m_continueSubmission == FALSE)
			{
				abortSubmission = TRUE;
			}
		}

		// Notify attached listeners that an entity that it has been submitted for rendering
		RuRendererEvent_Submitted_Args eventArgs(this, m_cameraStack.Peek(), sceneRoot, TRUE);
		m_event_Submitted.Trigger(&eventArgs);

		// If we are not to continue with this submission, abort
		if (eventArgs.m_continueSubmission == FALSE)
		{
			abortSubmission = TRUE;
		}
	}

	if (!abortSubmission)
	{
		// Submit to queue
		Submit_Internal(sceneRoot, worldTransform, mesh, material, TRUE);
	}

	RUPROFILE_SAMPLE_END(m_probe_3D_Submit, 2);

	return TRUE;
}

void CRuRendererNode_RenderContext::SetParentRenderer(IRuObject* renderer)
{
	if (renderer)
	{
		if (renderer->GetType() == CRuRenderer::Type())
		{
			m_renderer = static_cast<CRuRenderer*>(renderer);
		}
	}
	else
	{
		renderer = NULL;
	}
}

void CRuRendererNode_RenderContext::Begin()
{
	// Chain call to base class
	CRuRendererNode::Begin();

	// Reset statistics
	m_numPrimitivesRendered = 0;
	m_numBatchesIssued = 0;

	// Reset variables
	m_curClipPlaneSet = NULL;

	// Update default viewport size
	UpdateDefaultViewportParameters();

	// In case the render queue has not been executed
	for (INT32 i = 0; i < m_renderQueue.Count(); ++i)
	{
		ruSAFE_RELEASE(m_renderQueue[i].m_mesh);
		ruSAFE_RELEASE(m_renderQueue[i].m_material);
	}
	m_renderQueue.Clear();

	// Increment iteration
	++m_iteration;
}

void CRuRendererNode_RenderContext::End()
{
	// Chain call to base class
	CRuRendererNode::End();

	// Clear the render queue
	for (INT32 i = 0; i < m_renderQueue.Count(); ++i)
	{
		ruSAFE_RELEASE(m_renderQueue[i].m_mesh);
		ruSAFE_RELEASE(m_renderQueue[i].m_material);
	}
	m_renderQueue.Clear();

	// Clear rendering structures
	m_clipPlaneStack.Clear();
	m_cameraStack.Clear();

	// Clear valid decal targets list
	m_validDecalTargets.Clear();

	// Clear matrix stack
	FlushMatrixStack();

	// Clear parameter cache stack
	FlushParameterCacheStack();

	// Reset variables
	m_curClipPlaneSet = NULL;

	// Accumulate face count
	g_ruProfiler->AccumulateProbeValue(m_probe_3D_FaceCount, static_cast<REAL>(m_numPrimitivesRendered));

	// Accumulate batch count
	g_ruProfiler->AccumulateProbeValue(m_probe_3D_BatchCount, static_cast<REAL>(m_numBatchesIssued));
}

void CRuRendererNode_RenderContext::Execute(CRuKeyHash<PTRVALUE>& executionMap)
{
	// Chain execute call to base class
	CRuRendererNode::Execute(executionMap);

	UINT64 param;

	if (m_renderQueue.Count() == 0)
		return;

	// Error check
	if (!g_ruInstanceManager)
		return;

	// Begin time sampling for the flush
	RUPROFILE_SAMPLE_BEGIN(m_probe_3D_Flush, 0);

	// Build lighting info
	BuildAllLightingInfo();

	// Setup transforms
	SetupAllTransforms();

	// Instance geometry objects
	InstanceAllGeometries();

	// Instance textures
	InstanceAllTextures();

	// Setup render target
	SelectRenderTarget();

	// Set scissor rect
	if ((m_scissorRect.m_right - m_scissorRect.m_left) > 0.0f && (m_scissorRect.m_bottom - m_scissorRect.m_top) > 0.0f)
	{
		RECT rect;

		rect.left = static_cast<INT32>(m_scissorRect.m_left);
		rect.right = static_cast<INT32>(m_scissorRect.m_right);
		rect.top = static_cast<INT32>(m_scissorRect.m_top);
		rect.bottom = static_cast<INT32>(m_scissorRect.m_bottom);

		g_ruEngine->SetScissorRect(&rect);
	}
	else
	{
		g_ruEngine->SetScissorRect(NULL);
	}

	// Select viewport
	IRuViewport* effectiveViewport = m_customViewport ? m_customViewport : m_defaultViewport;

	effectiveViewport->SelectAsViewport();

	// Clear viewport according to its clear parameters
	if (effectiveViewport->GetClearFlags())
	{
		g_ruEngine->Clear(effectiveViewport->GetClearFlags(), effectiveViewport->GetClearColor(), effectiveViewport->GetClearZ(), effectiveViewport->GetClearStencil());
	}

	// Sort the rendering queue
	RUPROFILE_SAMPLE_BEGIN(m_probe_3D_Sorting, 0);
	if (m_sortRenderQueue)
	{
		SortRenderQueue();
	}
	RUPROFILE_SAMPLE_END(m_probe_3D_Sorting, 0);

	// Iterate through rendering queue and render each submodel
	UINT32* sortedIndices = m_radixSort.GetSortedIndices();

	if (m_sortRenderQueue)
	{
		//OutputDebugString("------------------------\n");
	}

	for (INT32 i = 0, numRQEntries = m_renderQueue.Count(); i < numRQEntries; ++i)
	{
		INT32 index = m_sortRenderQueue ? sortedIndices[i] : i;
		RuRenderQueueEntry& curEntry = m_renderQueue[index];
		if (m_sortRenderQueue)
		{
			//char moo[256];
			//sprintf(moo, "i:%d index:%d name:%s name:%s\n", i, index, curEntry.m_entity->GetObjectName(), curEntry.m_entity->GetResourceName());
			//OutputDebugString(moo);
		}
		// If the render entry has a clip plane set specified, apply it now
		if (m_curClipPlaneSet != curEntry.m_clipPlaneSet || i == 0)
		{
			// Set current clip plane set
			m_curClipPlaneSet = curEntry.m_clipPlaneSet;

			if (m_curClipPlaneSet)
			{
				for (INT32 cpIdx = 0; cpIdx < 6; ++cpIdx)
				{
					const CRuPlane4& clipPlane = m_curClipPlaneSet->GetClipPlane(cpIdx);

					if (clipPlane.IsZero() == FALSE)
					{
						g_ruEngine->SetClipPlane(cpIdx, clipPlane);
						g_ruEngine->SetClipPlaneEnable(cpIdx, TRUE);
					}
					else
					{
						g_ruEngine->SetClipPlaneEnable(cpIdx, FALSE);
					}
				}
			}
			else
			{
				for (INT32 cpIdx = 0; cpIdx < 6; ++cpIdx)
				{
					g_ruEngine->SetClipPlaneEnable(cpIdx, FALSE);
				}
			}
		}

		// Ensure the mesh, material, and shader are valid
		if (curEntry.m_mesh && curEntry.m_material && curEntry.m_shader && curEntry.m_mesh->GetNumVertices() > 0 && curEntry.m_mesh->GetNumIndices() > 0)
		{
			IRuShader* shader = curEntry.m_shader;

			// Clear the parameter cache
			m_paramCache1.ClearParameterCache();

			RUPROFILE_SAMPLE_BEGIN(m_probe_3D_ParameterSetup, 2);

			// Set default parameters into shader
			shader->SetDefaultParameters();

			// Set entity parameters into shader
			shader->SetParameters(curEntry.m_paramCache);

			RUPROFILE_SAMPLE_END(m_probe_3D_ParameterSetup, 2);

			RUPROFILE_SAMPLE_BEGIN(m_probe_3D_RenderAllPasses, 4);

			// Iterate through all passes of the shader
			for (INT32 curPass = 0, numPasses = shader->GetNumPasses(); curPass < numPasses; ++curPass)
			{
				//				RUPROFILE_SAMPLE_BEGIN(m_probe_3D_TextureSetup, 5);

								// Abort if the mesh could not be instanced
				if (curEntry.m_vertexAssembly == NULL || curEntry.m_instancedIB == NULL)
					continue;

				// Setup texture bindings
				for (INT32 curSampler = 0, numSamplers = shader->GetNumSamplers(curPass); curSampler < numSamplers; ++curSampler)
				{
					if (curEntry.m_instancedTextures[curPass][curSampler])
					{
						curEntry.m_instancedTextures[curPass][curSampler]->Select(curSampler);
					}
				}

				//				RUPROFILE_SAMPLE_END(m_probe_3D_TextureSetup, 5);

								// NOTE: Since we want to do everything in grouped loops, we must make sure each morphing mesh is a separate instance... so change this
				if (curEntry.m_paramCache->GetParameter(ruPARAMTYPE_TWEEN_FACTOR, param))
				{
					RUPROFILE_SAMPLE_BEGIN(m_probe_3D_Morphing, 5);

					REAL tweenFactor = RuCastINT64ToFloat(param);
					curEntry.m_mesh->SetMorphCurveTime(tweenFactor);

					RUPROFILE_SAMPLE_END(m_probe_3D_Morphing, 5);
				}

				//				if(m_disableRender)
				//					continue;

								// Render each batch
				for (INT32 curBatch = 0; ; ++curBatch)
				{
					INT32 batchOffset, batchNumPrimitives;

					// Get batch settings
					if (curEntry.m_mesh->GetBatch(curBatch, batchOffset, batchNumPrimitives) == FALSE)
						break;

					// Set batch parameters into shader
					const CRuParameterBlock* paramBlock = curEntry.m_mesh->GetBatchParameterBlock(curBatch);
					if (paramBlock)
					{
						m_paramCache1.CacheParameterBlock(paramBlock);
					}
					shader->SetParameters(&m_paramCache1);

					// Render
					RUPROFILE_SAMPLE_BEGIN(m_probe_3D_Render, 5);

					shader->BeginPass(curPass);

					RUPROFILE_SAMPLE_BEGIN(m_probe_3D_RenderD3D, 5);

					if (m_disableRender == FALSE)
					{
						curEntry.m_vertexAssembly->Select();
						curEntry.m_instancedIB->Select();
						g_ruEngine->Render(curEntry.m_mesh->GetNumVertices(), batchNumPrimitives, batchOffset, curEntry.m_mesh->GetPrimitiveType());
					}

					RUPROFILE_SAMPLE_END(m_probe_3D_RenderD3D, 5);

					++m_numBatchesIssued;
					shader->EndPass(curPass);

					RUPROFILE_SAMPLE_END(m_probe_3D_Render, 5);
				}
			}

			RUPROFILE_SAMPLE_END(m_probe_3D_RenderAllPasses, 4);
		}


		// debugging
//		assert(curEntry.m_mesh->GetRefCount() > 1);
//		assert(curEntry.m_material->GetRefCount() > 1);

		ruSAFE_RELEASE(curEntry.m_mesh);
		ruSAFE_RELEASE(curEntry.m_material);
	}

	// Clear the render queue
	m_renderQueue.Clear();

	// End time sampling for the flush
	RUPROFILE_SAMPLE_END(m_probe_3D_Flush, 0);

	return;
}

BOOL CRuRendererNode_RenderContext::IsEventSuppressed(RuEntityEvent entityEvent)
{
	for (INT32 i = 0; i < m_eventSuppressionList.Count(); ++i)
	{
		if (m_eventSuppressionList[i] == entityEvent)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CRuRendererNode_RenderContext::Submit_Internal(CRuEntity* sceneRoot, CRuMatrix4x4 const* worldTransform, IRuMesh* mesh, IRuMaterial* material, BOOL firstEntityInSeries)
{
	// Assertions
	assert(m_cameraStack.IsEmpty() == FALSE);

	if (worldTransform == NULL)
	{
		if (sceneRoot)
		{
			worldTransform = &sceneRoot->GetWorldTransform();
		}
		else
		{
			worldTransform = &m_identityTransform;
		}
	}

	RuRenderQueueEntry& rqEntry = AllocateRenderQueueEntry();

	rqEntry.m_camera = m_cameraStack.Peek();

	rqEntry.m_clipPlaneSet = m_clipPlaneStack.IsEmpty() ? NULL : m_clipPlaneStack.Peek();
	rqEntry.m_worldTransform = worldTransform;
	rqEntry.m_paramCache = CreateParameterCache();

	rqEntry.m_mesh = mesh;
	rqEntry.m_material = material;

	rqEntry.m_isRootEntity = firstEntityInSeries;

	// Override material if necessary
	if (m_materialOverrideCallback)
	{
		IRuMaterial* overrideMaterial = m_materialOverrideCallback(rqEntry.m_material);
		if (overrideMaterial)
		{
			rqEntry.m_material = overrideMaterial;
		}
	}

	// Store pointer to shader
	rqEntry.m_shader = rqEntry.m_material->GetShader();

	// Make sure material has a valid shader
	if (rqEntry.m_shader == NULL)
	{
		UndoAllocateRenderQueueEntry();
		return;
	}

	// Increment reference on mesh & material
	rqEntry.m_mesh->AddRef();
	rqEntry.m_material->AddRef();

	// debugging
	rqEntry.m_entity = sceneRoot;

	// Clear parameter block queue
	m_parameterBlocks.Clear();

	// Additional parameter blocks
	for (INT32 i = 0; i < m_extraParameterBlocks.Count(); ++i)
	{
		m_parameterBlocks.Queue(m_extraParameterBlocks[i]);
	}

	// Set material parameters into shader
	const CRuParameterBlock* matParamBlock = rqEntry.m_material->GetParameterBlock();
	if (matParamBlock)
	{
		m_parameterBlocks.Queue(matParamBlock);
	}

	if (sceneRoot)
	{
		// Find all parameter blocks and queue them into the parameter block queue
		CRuEntity* curEntity = sceneRoot;
		while (curEntity != NULL)
		{
			// NOTE: multiple parameter blocks for an entity must be added backwards!
			// otherwise the order of execution is not correct!

			// If the parent entity has any shader parameter blocks, append them to the queue
			m_parameterBlocks.Queue(curEntity->GetParameterBlock());

			// Go up one level in the tree
			curEntity = curEntity->GetParent();
		}

		// Setup information required to build lighting data
		// Go up the hierarchy until ruENTITYOPTION_USEPARENTLIGHTING is false or we run out of ancestors
		CRuEntity* lightingEntity = sceneRoot;
		while (lightingEntity->GetOption(ruENTITYOPTION_USEPARENTLIGHTING) && lightingEntity->GetParent())
		{
			lightingEntity = lightingEntity->GetParent();
		}

		rqEntry.m_lightingGUID = lightingEntity->GetGUID();
		rqEntry.m_lightingInvTransform.SetToIdentity();
		rqEntry.m_lightingInvTransform.Rotate(lightingEntity->GetRotation(ruFRAMETYPE_WORLD));
		rqEntry.m_lightingInvTransform.Translate(lightingEntity->GetTranslation(ruFRAMETYPE_WORLD));
		rqEntry.m_lightingInvTransform = rqEntry.m_lightingInvTransform.GetInverse();
		rqEntry.m_worldBounds = &lightingEntity->GetWorldBounds();

		// Get the entity's billboard type
		rqEntry.m_billboardType = sceneRoot->GetOption(ruENTITYOPTION_BILLBOARD);

		// Cache the projection transform
		rqEntry.m_isBackdrop = sceneRoot->GetOption(ruENTITYOPTION_BACKDROP);

		// Broadcast events
		if (firstEntityInSeries)
		{
			BOOL eventSurpressed_Broadcast_Camera = IsEventSuppressed(ruENTITYEVENT_BROADCAST_CAMERA);
			BOOL eventSurpressed_Broadcast_DecalTarget = IsEventSuppressed(ruENTITYEVENT_BROADCAST_DECALTARGET);

			// Broadcast acting camera to the entity
			if (eventSurpressed_Broadcast_Camera == FALSE)
			{
				RuEntityEvent_Camera_Args cameraArgs(rqEntry.m_camera);
				sceneRoot->Event_Camera().Trigger(&cameraArgs);
			}

			// Decal handling -- broadcast valid decal target array to the entity
			if (eventSurpressed_Broadcast_DecalTarget == FALSE)
			{
				RuEntityEvent_DecalTarget_Args decalTargetArgs(&m_validDecalTargets);
				sceneRoot->Event_DecalTarget().Trigger(&decalTargetArgs);
			}
		}
	}
	else
	{
		rqEntry.m_lightingGUID = CRuGUID::Invalid;
		rqEntry.m_worldBounds = NULL;
	}

	// Set default scene parameters into shader
	rqEntry.m_paramCache->CacheParameterBlock(&m_renderer->GetDefaultParameterBlock());

	// Apply parameter blocks backwards to ensure correct priority
	while (m_parameterBlocks.IsEmpty() == FALSE)
	{
		// Cache all parameters in the parameter block
		rqEntry.m_paramCache->CacheParameterBlock(m_parameterBlocks.DequeueTail());
	}

	// Retrieve alpha value and store entity priority
	// Retrieve force alpha blend
	UINT64 parameter;
	BOOL forceAlphaBlend = FALSE;
	REAL globalAlpha = 1.0f;
	BOOL invertSortOrder = FALSE;
	BOOL ignoreAlphaSort = FALSE;
	BOOL alphaZPass = FALSE;

	if (rqEntry.m_paramCache->GetParameter(ruPARAMTYPE_MATERIAL_TRANSLUCENT, parameter))
	{
		forceAlphaBlend = parameter ? TRUE : FALSE;
	}

	if (rqEntry.m_paramCache->GetParameter(ruPARAMTYPE_MATERIAL_ALPHA, parameter))
	{
		globalAlpha = RuCastINT64ToFloat(parameter) * globalAlpha;
	}

	if (rqEntry.m_paramCache->GetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODIFIER, parameter))
	{
		globalAlpha = RuCastINT64ToFloat(parameter) * globalAlpha;
	}

	if (rqEntry.m_paramCache->GetParameter(ruPARAMTYPE_SYSTEM_INVERTSORTORDER, parameter))
	{
		invertSortOrder = (BOOL)parameter;
	}

	if (rqEntry.m_paramCache->GetParameter(ruPARAMTYPE_SYSTEM_IGNOREALPHASORT, parameter))
	{
		ignoreAlphaSort = (BOOL)parameter;
	}

	if (rqEntry.m_paramCache->GetParameter(ruPARAMTYPE_SYSTEM_ALPHAZPASS, parameter))
	{
		alphaZPass = (BOOL)parameter;
	}

	// If alpha blending is enabled, set the priority accordingly
	if (alphaZPass)
	{
		rqEntry.m_priority = ruEPSILON;
	}
	else if ((globalAlpha < 1.0f || forceAlphaBlend) && sceneRoot && !ignoreAlphaSort)
	{
		CRuVector3 entityPos(sceneRoot->GetWorldBounds().Center());
		rqEntry.m_camera->GetFrustumTransform().TransformPointProjected(entityPos);

		if (invertSortOrder)
		{
			rqEntry.m_priority = min(0.0f, -entityPos.z) - ruEPSILON;
		}
		else
		{
			rqEntry.m_priority = max(0.0f, 1.0f - entityPos.z) + ruEPSILON;
		}
	}
	else
	{
		rqEntry.m_priority = 0.0f;
	}

	// Set default alpha
	rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, ruALPHAMODE_NORMAL);
	rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(1.0f));

	// Store pointer to camera
	rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_CAMERA, reinterpret_cast<UINT64>(rqEntry.m_camera));

	// If the mesh is skinned and there is a hierarchy available, skin it
	if (sceneRoot && sceneRoot->GetOption(ruENTITYOPTION_SKINNED))
	{
		// Find the frame hierarchy to use for this entity ...
		CRuFrameHierarchy* frameHierarchy = NULL;
		CRuHierarchy_GR2* hierarchy_GR2 = NULL;
		CRuEntity* parentEntity = sceneRoot->GetParent();
		while (parentEntity != NULL)
		{
			// Use the parent entity as the frame hierarchy if it is one and if a frame hierarchy hasn't already been found
			if (frameHierarchy == NULL && parentEntity->GetType().IsSubClassOf(CRuFrameHierarchy::Type()))
			{
				frameHierarchy = static_cast<CRuFrameHierarchy*>(parentEntity);
			}

			if (hierarchy_GR2 == NULL && parentEntity->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
			{
				hierarchy_GR2 = static_cast<CRuHierarchy_GR2*>(parentEntity);
			}

			// Go up one level in the tree
			parentEntity = parentEntity->GetParent();
		}

		if (frameHierarchy || hierarchy_GR2)
		{
			// Enable skinning
			rqEntry.m_material->SetSkinned(TRUE);

			// Store bone data in parameter cache
			if (frameHierarchy)
			{
				rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_SKIN_NODECOUNT, static_cast<UINT64>(frameHierarchy->GetNumSubNodes()));
				rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_SKIN_TRANSFORMS, reinterpret_cast<UINT64>(frameHierarchy->GetSubNodeTransforms()));
			}
			else if (hierarchy_GR2)
			{
				rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_SKIN_NODECOUNT, static_cast<UINT64>(hierarchy_GR2->GetNumSubNodes()));
				rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_SKIN_TRANSFORMS, reinterpret_cast<UINT64>(hierarchy_GR2->GetSubNodeTransforms()));
			}

			// Get cached skin from the skin cache
			RuCachedSkin cachedSkin = m_renderer->GetSkinCache()->GetCachedSkin(rqEntry.m_mesh);

			if (cachedSkin.m_subdividedMeshes)
			{
				RuRenderQueueEntry queueEntryCopy = rqEntry;

				// Queue the first subdivided mesh (there is always at least one mesh)
				ruSAFE_RELEASE(rqEntry.m_mesh);
				rqEntry.m_mesh = cachedSkin.m_subdividedMeshes->Get(0);
				rqEntry.m_mesh->AddRef();

				// Queue the remaining subdivided meshes
				for (INT32 k = 1; k < cachedSkin.m_subdividedMeshes->Count(); ++k)
				{
					RuRenderQueueEntry& copiedEntry = AllocateRenderQueueEntry();
					copiedEntry = queueEntryCopy;

					copiedEntry.m_mesh = cachedSkin.m_subdividedMeshes->Get(k);
					copiedEntry.m_mesh->AddRef();
					copiedEntry.m_material->AddRef();
				}
			}
		}
	}
}

void CRuRendererNode_RenderContext::BuildAllLightingInfo()
{
	RUPROFILE_SAMPLE_BEGIN(m_probe_3D_Lighting, 0);

	if (m_enableLighting)
	{
		for (INT32 i = 0; i < m_renderQueue.Count(); ++i)
		{
			RuRenderQueueEntry& rqEntry = m_renderQueue[i];

			// Set lighting information into the shader (TODO: Only do this if required by shader)

			// Build lighting information
			if (rqEntry.m_lightingGUID != CRuGUID::Invalid)
			{
				RuLightingInfo* lightingInfo = m_renderer->BuildLightingInfo(rqEntry.m_lightingGUID, rqEntry.m_lightingInvTransform, *rqEntry.m_worldBounds);

				// Set lighting parameters
				rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_LIGHT_AMBIENT, (UINT64)&lightingInfo->m_ambientLight);
				rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_LIGHT_OBJ_SH9, (UINT64)lightingInfo->m_shCoefficients);
				rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_LIGHT_SPECULAR_DIR, (UINT64)&lightingInfo->m_specularDirection);
				rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_LIGHT_SPECULAR_COLOR, (UINT64)lightingInfo->m_specularColor);
			}
			else
			{
			}
		}
	}
	else
	{
		m_defaultLightingInfo.m_ambientLight.Set(0x00000000);
		for (INT32 i = 0; i < 9; ++i)
		{
			m_defaultLightingInfo.m_shCoefficients[i].Set(0x00000000);
		}
		m_defaultLightingInfo.m_specularColor = 0x00000000;

		for (INT32 i = 0; i < m_renderQueue.Count(); ++i)
		{
			RuRenderQueueEntry& rqEntry = m_renderQueue[i];

			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_LIGHT_AMBIENT, (UINT64)&m_defaultLightingInfo.m_ambientLight);
			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_LIGHT_OBJ_SH9, (UINT64)m_defaultLightingInfo.m_shCoefficients);
			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_LIGHT_SPECULAR_DIR, (UINT64)&m_defaultLightingInfo.m_specularDirection);
			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_LIGHT_SPECULAR_COLOR, (UINT64)m_defaultLightingInfo.m_specularColor);
		}
	}

	RUPROFILE_SAMPLE_END(m_probe_3D_Lighting, 0);
}

void CRuRendererNode_RenderContext::SetupAllTransforms()
{
	RUPROFILE_SAMPLE_BEGIN(m_probe_3D_TransformSetup, 0);

	for (INT32 i = 0; i < m_renderQueue.Count(); ++i)
	{
		RuRenderQueueEntry& rqEntry = m_renderQueue[i];

		// Cache projection transform
		if (rqEntry.m_isBackdrop)
		{
			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_TRANSFORM_PROJECTION, (UINT64)&rqEntry.m_camera->GetBackdropProjectionTransform());
		}
		else
		{
			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_TRANSFORM_PROJECTION, (UINT64)&rqEntry.m_camera->GetExtendedProjectionTransform());
		}

		// Check for billboarding here? And override all of the transforms?
		switch (rqEntry.m_billboardType)
		{
			// Normal entity, no billboarding
		case ruBILLBOARDTYPE_NONE:
		{
			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_TRANSFORM_WORLD, (UINT64)rqEntry.m_worldTransform);
			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_TRANSFORM_VIEW, (UINT64)&rqEntry.m_camera->GetCameraTransform());

			if (rqEntry.m_isRootEntity && rqEntry.m_entity)
			{
				RuEntityEvent_Transform_Args transformArgs(rqEntry.m_worldTransform, &rqEntry.m_camera->GetCameraTransform());
				rqEntry.m_entity->Event_Transform().Trigger(&transformArgs);
			}
		}

		break;

		// Unconstrained billboard
		case ruBILLBOARDTYPE_UNCONSTRAINED:
		{
			CRuMatrix4x4 viewBillboardingMatrix;
			viewBillboardingMatrix.SetToBillboard(rqEntry.m_camera->GetCameraTransform(), CRuVector3(0.0f, 0.0f, -1.0f), CRuVector3(0.0f, 1.0f, 0.0f));

			CRuMatrix4x4 inverseTranslationMatrix;
			inverseTranslationMatrix.SetToTranslation(-(*rqEntry.m_worldTransform)._41, -(*rqEntry.m_worldTransform)._42, -(*rqEntry.m_worldTransform)._43);

			CRuMatrix4x4 translationMatrix;
			translationMatrix.SetToTranslation((*rqEntry.m_worldTransform)._41, (*rqEntry.m_worldTransform)._42, (*rqEntry.m_worldTransform)._43);

			CRuMatrix4x4* modifiedWorldMatrix = CreateMatrix();
			*modifiedWorldMatrix = viewBillboardingMatrix * translationMatrix;

			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_TRANSFORM_WORLD, (UINT64)modifiedWorldMatrix);
			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_TRANSFORM_VIEW, (UINT64)&rqEntry.m_camera->GetCameraTransform());

			if (rqEntry.m_isRootEntity && rqEntry.m_entity)
			{
				RuEntityEvent_Transform_Args transformArgs(modifiedWorldMatrix, &rqEntry.m_camera->GetCameraTransform());
				rqEntry.m_entity->Event_Transform().Trigger(&transformArgs);
			}
		}

		break;

		// Unconstrained billboard (Alt)
		case ruBILLBOARDTYPE_UNCONSTRAINED_ALTERNATE:
		{
			CRuMatrix4x4 viewBillboardingMatrix;
			viewBillboardingMatrix.SetToBillboard(rqEntry.m_camera->GetCameraTransform(), CRuVector3(0.0f, 0.0f, -1.0f), CRuVector3(0.0f, 1.0f, 0.0f));

			CRuMatrix4x4 inverseTranslationMatrix;
			inverseTranslationMatrix.SetToTranslation(-(*rqEntry.m_worldTransform)._41, -(*rqEntry.m_worldTransform)._42, -(*rqEntry.m_worldTransform)._43);

			CRuMatrix4x4 translationMatrix;
			translationMatrix.SetToTranslation((*rqEntry.m_worldTransform)._41, (*rqEntry.m_worldTransform)._42, (*rqEntry.m_worldTransform)._43);

			CRuMatrix4x4* modifiedWorldMatrix = CreateMatrix();
			*modifiedWorldMatrix = (*rqEntry.m_worldTransform) * inverseTranslationMatrix * viewBillboardingMatrix * translationMatrix;

			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_TRANSFORM_WORLD, (UINT64)modifiedWorldMatrix);
			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_TRANSFORM_VIEW, (UINT64)&rqEntry.m_camera->GetCameraTransform());

			if (rqEntry.m_isRootEntity && rqEntry.m_entity)
			{
				RuEntityEvent_Transform_Args transformArgs(modifiedWorldMatrix, &rqEntry.m_camera->GetCameraTransform());
				rqEntry.m_entity->Event_Transform().Trigger(&transformArgs);
			}
		}

		break;

		case ruBILLBOARDTYPE_AXIAL:
		{
			CRuVector3 billboardAxis(0.0f, 1.0f, 0.0f);
			rqEntry.m_worldTransform->TransformVector(billboardAxis);
			if (billboardAxis.Magnitude() > ruEPSILON)
			{
				billboardAxis.Normalize();
			}
			else
			{
				billboardAxis.Set(0.0f, 0.0f, -1.0f);
			}

			CRuVector3 forwardAxis(0.0f, 0.0f, -1.0f);
			rqEntry.m_worldTransform->TransformVector(forwardAxis);
			if (forwardAxis.Magnitude() > ruEPSILON)
			{
				forwardAxis.Normalize();
			}
			else
			{
				forwardAxis.Set(0.0f, 0.0f, -1.0f);
			}

			CRuMatrix4x4 viewBillboardingMatrix;
			viewBillboardingMatrix.SetToAxialBillboard(rqEntry.m_camera->GetCameraTransform(), forwardAxis, billboardAxis);

			CRuMatrix4x4 inverseTranslationMatrix;
			inverseTranslationMatrix.SetToTranslation(-(*rqEntry.m_worldTransform)._41, -(*rqEntry.m_worldTransform)._42, -(*rqEntry.m_worldTransform)._43);

			CRuMatrix4x4 translationMatrix;
			translationMatrix.SetToTranslation((*rqEntry.m_worldTransform)._41, (*rqEntry.m_worldTransform)._42, (*rqEntry.m_worldTransform)._43);

			CRuMatrix4x4* modifiedWorldMatrix = CreateMatrix();
			*modifiedWorldMatrix = viewBillboardingMatrix * translationMatrix;

			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_TRANSFORM_WORLD, (UINT64)modifiedWorldMatrix);
			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_TRANSFORM_VIEW, (UINT64)&rqEntry.m_camera->GetCameraTransform());

			if (rqEntry.m_isRootEntity && rqEntry.m_entity)
			{
				RuEntityEvent_Transform_Args transformArgs(modifiedWorldMatrix, &rqEntry.m_camera->GetCameraTransform());
				rqEntry.m_entity->Event_Transform().Trigger(&transformArgs);
			}
		}

		break;

		case ruBILLBOARDTYPE_AXIAL_ALTERNATE:
		{
			CRuVector3 billboardAxis(0.0f, 1.0f, 0.0f);
			rqEntry.m_worldTransform->TransformVector(billboardAxis);
			if (billboardAxis.Magnitude() > ruEPSILON)
			{
				billboardAxis.Normalize();
			}
			else
			{
				billboardAxis.Set(0.0f, 0.0f, -1.0f);
			}

			CRuVector3 forwardAxis(0.0f, 0.0f, -1.0f);
			rqEntry.m_worldTransform->TransformVector(forwardAxis);
			if (forwardAxis.Magnitude() > ruEPSILON)
			{
				forwardAxis.Normalize();
			}
			else
			{
				forwardAxis.Set(0.0f, 0.0f, -1.0f);
			}

			CRuMatrix4x4 viewBillboardingMatrix;
			viewBillboardingMatrix.SetToAxialBillboard(rqEntry.m_camera->GetCameraTransform(), forwardAxis, billboardAxis);

			CRuMatrix4x4 inverseTranslationMatrix;
			inverseTranslationMatrix.SetToTranslation(-(*rqEntry.m_worldTransform)._41, -(*rqEntry.m_worldTransform)._42, -(*rqEntry.m_worldTransform)._43);

			CRuMatrix4x4 translationMatrix;
			translationMatrix.SetToTranslation((*rqEntry.m_worldTransform)._41, (*rqEntry.m_worldTransform)._42, (*rqEntry.m_worldTransform)._43);

			CRuMatrix4x4* modifiedWorldMatrix = CreateMatrix();
			*modifiedWorldMatrix = (*rqEntry.m_worldTransform) * inverseTranslationMatrix * viewBillboardingMatrix * translationMatrix;

			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_TRANSFORM_WORLD, (UINT64)modifiedWorldMatrix);
			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_TRANSFORM_VIEW, (UINT64)&rqEntry.m_camera->GetCameraTransform());

			if (rqEntry.m_isRootEntity && rqEntry.m_entity)
			{
				RuEntityEvent_Transform_Args transformArgs(modifiedWorldMatrix, &rqEntry.m_camera->GetCameraTransform());
				rqEntry.m_entity->Event_Transform().Trigger(&transformArgs);
			}
		}

		break;

		case ruBILLBOARDTYPE_AXIAL_CUSTOM:
		{
			CRuMatrix4x4 combinedWorldViewTransform = (*rqEntry.m_worldTransform) * rqEntry.m_camera->GetCameraTransform();

			CRuMatrix4x4* identityMatrix = CreateMatrix();
			identityMatrix->SetToIdentity();

			CRuMatrix4x4* aggregateMatrix = CreateMatrix();
			aggregateMatrix->SetToAxialBillboard(combinedWorldViewTransform, CRuVector3(0.0f, 0.0f, -1.0f), CRuVector3(0.0f, 1.0f, 0.0f));
			*aggregateMatrix = (*aggregateMatrix) * combinedWorldViewTransform;

			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_TRANSFORM_WORLD, (UINT64)identityMatrix);
			rqEntry.m_paramCache->CacheParameter(ruPARAMTYPE_TRANSFORM_VIEW, (UINT64)aggregateMatrix);
		}

		break;
		}
	}

	RUPROFILE_SAMPLE_END(m_probe_3D_TransformSetup, 0);
}

void CRuRendererNode_RenderContext::InstanceAllGeometries()
{
	// Instance geometry objects
	RUPROFILE_SAMPLE_BEGIN(m_probe_Engine_InstancingTime, 0);

	for (INT32 i = 0, numRQEntries = m_renderQueue.Count(); i < numRQEntries; ++i)
	{
		RuRenderQueueEntry& curEntry = m_renderQueue[i];

		if (curEntry.m_mesh && curEntry.m_shader && curEntry.m_mesh->GetNumVertices() > 0 && curEntry.m_mesh->GetNumIndices() > 0)
		{
			curEntry.m_vertexAssembly = g_ruInstanceManager->GetVertexAssembly(curEntry.m_mesh, curEntry.m_shader->GetVertexDeclaration());
			curEntry.m_instancedIB = g_ruInstanceManager->GetInstancedIndexBuffer(curEntry.m_mesh);
		}
		else
		{
			curEntry.m_vertexAssembly = NULL;
			curEntry.m_instancedIB = NULL;
		}
	}

	RUPROFILE_SAMPLE_END(m_probe_Engine_InstancingTime, 0);
}

void CRuRendererNode_RenderContext::InstanceAllTextures()
{
	// Instance textures
	RUPROFILE_SAMPLE_BEGIN(m_probe_3D_TextureSetup, 0);

	// Get local references to system textures
	IRuTexture* nullTexture = g_ruResourceManager->LoadTexture("RUSYSTEM_NULL");
	IRuTexture* blackTexture = g_ruResourceManager->LoadTexture("RUSYSTEM_BLACK");
	IRuTexture* clampTexture = g_ruResourceManager->LoadTexture("RUSYSTEM_CLAMP");

	for (INT32 i = 0, numRQEntries = m_renderQueue.Count(); i < numRQEntries; ++i)
	{
		RuRenderQueueEntry& curEntry = m_renderQueue[i];

		if (curEntry.m_shader)
		{
			memset(curEntry.m_instancedTextures, 0, sizeof(IRuBaseTexture*) * 4 * 16);

			for (INT32 curPass = 0, numPasses = curEntry.m_shader->GetNumPasses(); curPass < numPasses; ++curPass)
			{
				for (INT32 curSampler = 0, numSamplers = curEntry.m_shader->GetNumSamplers(curPass); curSampler < numSamplers; ++curSampler)
				{
					RuTextureChannel texChannel = curEntry.m_shader->GetTextureChannel(curPass, curSampler);
					IRuBaseTexture* texture = curEntry.m_material->GetTexture(texChannel);
					IRuBaseTexture* instancedTexture = NULL;

					if (texture == NULL || texture->IsInvalidated())
					{
						switch (texChannel)
						{
						case ruTEXCHANNEL_SYSTEM_CLAMP:
							texture = clampTexture;
							break;

						case ruTEXCHANNEL_GLOSSMAP:
						case ruTEXCHANNEL_GLOWMAP:
							texture = blackTexture;
							break;

						default:
							texture = nullTexture;
							break;
						}
					}

					// Instantiate texture
					curEntry.m_instancedTextures[curPass][curSampler] = g_ruInstanceManager->GetInstance(texture);
				}
			}
		}
	}

	// Release local references to textures
	ruSAFE_RELEASE(nullTexture);
	ruSAFE_RELEASE(blackTexture);
	ruSAFE_RELEASE(clampTexture);

	RUPROFILE_SAMPLE_END(m_probe_3D_TextureSetup, 0);
}

BOOL CRuRendererNode_RenderContext::SelectRenderTarget()
{
	// Debug assert
	assert(m_renderTarget);

	// Select the render target
	if (m_renderTarget->GetType().IsSubClassOf(IRuSurface::Type()))
	{
		static_cast<IRuSurface*>(m_renderTarget)->SelectAsRenderTarget(0);
	}
	else if (m_renderTarget->GetType().IsSubClassOf(IRuSwapChain::Type()))
	{
		static_cast<IRuSwapChain*>(m_renderTarget)->SelectAsRenderTarget(0);
	}
	else if (m_renderTarget->GetType().IsSubClassOf(IRuTexture::Type()))
	{
		// NOTE: When we're given a texture instead of a surface, level 0 is always selected as the render target
		static_cast<IRuTexture*>(m_renderTarget)->SelectAsRenderTarget(0, 0);
	}
	else if (m_renderTarget->GetType().IsSubClassOf(IRuTextureView::Type()))
	{
		IRuTextureView* textureView = static_cast<IRuTextureView*>(m_renderTarget);
		IRuTexture* rtTexture = textureView->GetTexture();
		IRuViewport* viewport = textureView->GetViewport();

		if (rtTexture)
		{
			rtTexture->SelectAsRenderTarget(0, 0);
		}

		// Select texture view's viewport as the viewport
		this->SetViewport(viewport);

		//		SetViewport(viewport);
	}
	else
	{
		// Log an error
		return FALSE;
	}

	// Select the depth stencil surface
	if (m_depthStencilSurface)
	{
		m_depthStencilSurface->SelectDepthStencil();
	}
	else
	{
		g_ruEngine->RestoreDefaultDepthStencil();
	}

	return TRUE;
}

inline DWORD RuCastFloatToDWORD(float sourceFloat)
{
	return *((DWORD*)(&sourceFloat));
}

void CRuRendererNode_RenderContext::SortRenderQueue()
{
	for (INT32 i = 0; i < 7; ++i)
	{
		m_sortCriterias[i].SetNumEntries(m_renderQueue.Count());
	}

	m_sortCriterias0.SetNumEntries(m_renderQueue.Count());
	m_sortCriterias1.SetNumEntries(m_renderQueue.Count());
	m_sortCriterias2.SetNumEntries(m_renderQueue.Count());
	m_sortCriterias3.SetNumEntries(m_renderQueue.Count());
	m_sortCriterias4.SetNumEntries(m_renderQueue.Count());
	m_sortCriterias5.SetNumEntries(m_renderQueue.Count());
	m_sortCriterias6.SetNumEntries(m_renderQueue.Count());

	for (INT32 i = 0, j = m_renderQueue.Count(); i < j; ++i)
	{
		RuRenderQueueEntry& curEntry = m_renderQueue[i];

		IRuShader* shader = curEntry.m_shader;

		// Use shader class as the third sorting criteria
		m_sortCriterias[0][i] = shader->GetShaderClass(); // uint32
		m_sortCriterias0[i] = shader->GetShaderClass();

		// Use entity priority as the sixth sorting criteria
		m_sortCriterias[1][i] = RuCastFloatToINT64(curEntry.m_priority); // uint32
		m_sortCriterias1[i] = RuCastFloatToDWORD(curEntry.m_priority);

		// Use shader priority as the third sorting criteria
		m_sortCriterias[2][i] = shader->GetShaderPriority(); // uint32
		m_sortCriterias2[i] = shader->GetShaderPriority(); // uint32

		// Use shader address as the fourth sorting criteria
		m_sortCriterias[3][i] = reinterpret_cast<UINT64>(shader); // uint64
		m_sortCriterias3[i] = reinterpret_cast<UINT64>(shader); // uint64

		// Use material priority as the fifth sorting criteria
		m_sortCriterias[4][i] = curEntry.m_material->GetMaterialPriority(); // uint32
		m_sortCriterias4[i] = curEntry.m_material->GetMaterialPriority(); // uint32

		// Fill the rest of the sorting criteria using textures
		for (INT32 curSampler = 0; curSampler < 2; ++curSampler)
		{
			RuTextureChannel samplerChannel = shader->GetTextureChannel(0, curSampler);
			IRuBaseTexture* samplerTexture = NULL;

			if (samplerChannel != ruTEXCHANNEL_INVALID)
			{
				samplerTexture = curEntry.m_material->GetTexture(samplerChannel);
			}

			m_sortCriterias[curSampler + 5][i] = reinterpret_cast<UINT64>(samplerTexture); // uint64

			if (curSampler == 5)
				m_sortCriterias5[i] = reinterpret_cast<UINT64>(samplerTexture);
			else
				m_sortCriterias6[i] = reinterpret_cast<UINT64>(samplerTexture);
		}
	}

	// Perform radix sort backwards
	//for (INT32 i = 6; i >= 0; --i)
	//{
	//	//if(i == 3)
	//	//	m_radixSort.Sort((REAL *) m_sortCriterias[i].GetArray(), m_renderQueue.Count(), (i == 6) ? FALSE : TRUE);
	//	//else
	//	m_radixSort.Sort((UINT64*)m_sortCriterias[i].GetArray(), m_renderQueue.Count(), FALSE, (i == 6) ? FALSE : TRUE);
	//}

	m_radixSort.Sort((UINT64*)m_sortCriterias6.GetArray(), m_renderQueue.Count(), FALSE, FALSE);
	m_radixSort.Sort((UINT64*)m_sortCriterias5.GetArray(), m_renderQueue.Count(), FALSE, TRUE);
	m_radixSort.Sort((UINT32*)m_sortCriterias4.GetArray(), m_renderQueue.Count(), FALSE, TRUE);
	m_radixSort.Sort((UINT64*)m_sortCriterias3.GetArray(), m_renderQueue.Count(), TRUE);
	m_radixSort.Sort((UINT32*)m_sortCriterias2.GetArray(), m_renderQueue.Count(), FALSE, TRUE);
	m_radixSort.Sort((UINT32*)m_sortCriterias1.GetArray(), m_renderQueue.Count(), FALSE, TRUE);
	m_radixSort.Sort((UINT32*)m_sortCriterias0.GetArray(), m_renderQueue.Count(), FALSE, TRUE);
}

CRuMatrix4x4* CRuRendererNode_RenderContext::CreateMatrix()
{
	CRuMatrix4x4* newMatrix = NULL;

	// Create matrix
	if (m_unusedMatrixStack.IsEmpty())
	{
		newMatrix = ruNEW CRuMatrix4x4();
	}
	else
	{
		newMatrix = m_unusedMatrixStack.Pop();
	}

	// Push matrix onto used matrix stack
	m_usedMatrixStack.Push(newMatrix);

	return newMatrix;
}

void CRuRendererNode_RenderContext::FlushMatrixStack()
{
	while (m_usedMatrixStack.IsEmpty() == FALSE)
	{
		m_unusedMatrixStack.Push(m_usedMatrixStack.Pop());
	}
}

CRuParameterCache* CRuRendererNode_RenderContext::CreateParameterCache()
{
	CRuParameterCache* paramCache = NULL;

	// Get a parameter cache object
	if (m_unusedParameterCacheStack.IsEmpty())
	{
		paramCache = ruNEW CRuParameterCache();
	}
	else
	{
		paramCache = m_unusedParameterCacheStack.Pop();
	}

	// Push parameter cache object onto used stack
	m_usedParameterCacheStack.Push(paramCache);

	return paramCache;
}

void CRuRendererNode_RenderContext::FlushParameterCacheStack()
{
	while (m_usedParameterCacheStack.IsEmpty() == FALSE)
	{
		// Pop cache off used stack and clear it
		CRuParameterCache* paramCache = m_usedParameterCacheStack.Pop();
		paramCache->ClearParameterCache();

		// Push onto unused stack
		m_unusedParameterCacheStack.Push(paramCache);
	}
}

// ************************************************************************************************************************************************************

CRuRendererNode_RenderContext* g_ruDefaultRenderContext = NULL;

// ************************************************************************************************************************************************************

CRuRendererNode_RenderContext* RuCreateRenderContext()
{
	return static_cast<CRuRendererNode_RenderContext*>(RuObjectFactory().CreatePooledObject(CRuRendererNode_RenderContext::ClassGUID()));
}

// ************************************************************************************************************************************************************

void RuDestroyRenderContext(CRuRendererNode_RenderContext* renderContext)
{
	RuObjectFactory().DestroyPooledObject(renderContext);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
