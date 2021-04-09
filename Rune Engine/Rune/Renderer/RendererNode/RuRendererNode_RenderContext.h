/*!
	@file RuRendererNode_RenderContext.h

	Copyright (c) 2004-2008 All rights reserved
*/

#pragma once

#include "../../Renderer/RuRenderer.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuRendererNode_RenderContext;

// ************************************************************************************************************************************************************

class RuRendererEvent_Submit_Args : public RuEventArgs
{
public:
	CRuRendererNode_RenderContext*			m_renderContext;				//!< Render context
	CRuCamera*								m_camera;						//!< Camera used for submission
	CRuEntity*								m_entity;						//!< Entity that was submitted

	BOOL									m_continueSubmission;			//!< Return value: Continue with render submission for this entry?

	RuRendererEvent_Submit_Args(CRuRendererNode_RenderContext *renderContext, CRuCamera *camera, CRuEntity *entity, BOOL continueSubmission)
	:	m_renderContext(renderContext),
		m_camera(camera),
		m_entity(entity),
		m_continueSubmission(continueSubmission)
	{
	}
};

class RuRendererEvent_Submitted_Args : public RuEventArgs
{
public:
	CRuRendererNode_RenderContext*			m_renderContext;				//!< Render context
	CRuCamera*								m_camera;						//!< Camera used for submission
	CRuEntity*								m_entity;						//!< Entity that was submitted

	BOOL									m_continueSubmission;			//!< Return value: Continue with render submission for this entry?

	RuRendererEvent_Submitted_Args(CRuRendererNode_RenderContext *renderContext, CRuCamera *camera, CRuEntity *entity, BOOL continueSubmission)
	:	m_renderContext(renderContext),
		m_camera(camera),
		m_entity(entity),
		m_continueSubmission(continueSubmission)
	{
	}
};

class RuRendererEvent_Culled_Args : public RuEventArgs
{
public:
	CRuRendererNode_RenderContext*			m_renderContext;				//!< Render context
	CRuCamera*								m_camera;						//!< Camera used for submission
	CRuEntity*								m_entity;						//!< Entity that was culled

	RuRendererEvent_Culled_Args(CRuRendererNode_RenderContext *renderContext, CRuCamera *camera, CRuEntity *entity)
	:	m_renderContext(renderContext),
		m_camera(camera),
		m_entity(entity)
	{
	}
};

// ************************************************************************************************************************************************************

class CRuRendererNode_RenderContext : public CRuRendererNode
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	typedef boost::function<IRuMaterial * (IRuMaterial *)> MaterialOverrideCallback;		//!< Defined: IRuMaterial* MaterialOverrideCallback(IRuMaterial *sourceMaterial);

	struct EventListenerDesc
	{
		IRuEntityEventListener*				m_listener;										//!< Event listener
		void*								m_userData;										//!< User-specified listener data
		EventListenerDesc*					m_next;											//!< Pointer to the next event listener descriptor
	};

	// Base render context information
	CRuRenderer*							m_renderer;										//!< Owner of this context
	IRuObject*								m_renderTarget;									//!< Render target on which this render context acts upon
	IRuSurface*								m_depthStencilSurface;							//!< Depth stencil surface, NULL = use default
	IRuViewport*							m_defaultViewport;								//!< Default viewport object
	IRuViewport*							m_customViewport;								//!< Custom viewport, overrides default viewport
	CRuRectangle							m_scissorRect;

	CRuMatrix4x4							m_identityTransform;

	INT32									m_iteration;									//!< Current iteration number

	// Event system data
	CRuArrayList<RuEntityEvent>				m_eventSuppressionList;							//!< List of suppressed events
	CRuEvent								m_event_Submit;
	CRuEvent								m_event_Submitted;
	CRuEvent								m_event_Culled;

	// Parameter cache
	CRuParameterCache						m_paramCache1;									// Inner loop parameter cache

	// Currents
	CRuClipPlaneSet*						m_curClipPlaneSet;

	// Submit stacks
	CRuStack<CRuClipPlaneSet *>				m_clipPlaneStack;								//!< Clip plane stack
	CRuStack<CRuCamera *>					m_cameraStack;									//!< Camera stack

	CRuArrayList<CRuParameterBlock *>		m_extraParameterBlocks;

	// Decal
	CRuArrayList<CRuEntity *>				m_validDecalTargets;							//!< List of valid decal targets

	// Material override
	CRuStack<CRuRendererNode_RenderContext::MaterialOverrideCallback>	m_materialOverrideCallbackStack;
	CRuRendererNode_RenderContext::MaterialOverrideCallback				m_materialOverrideCallback;

	// Render queue
	CRuArrayList<RuRenderQueueEntry>		m_renderQueue;									//!< Rendering queue

	// Support objects
	RuLightingInfo							m_defaultLightingInfo;							//!< Default lighting info used when lighting is disabled
	BOOL									m_enableLighting;								//!< Standard lighting enabled?
	BOOL									m_enableCulling;								//!< Enable culling?
	BOOL									m_sortRenderQueue;								//!< Sort the render queue when rendering?
	
	//CRuArrayList<UINT32>					m_sortCriterias[7];
	CRuArrayList<UINT32> m_sortCriterias0;
	CRuArrayList<UINT32> m_sortCriterias1;
	CRuArrayList<UINT32> m_sortCriterias2;
	CRuArrayList<UINT64> m_sortCriterias3;
	CRuArrayList<UINT32> m_sortCriterias4;
	CRuArrayList<UINT64> m_sortCriterias5;
	CRuArrayList<UINT64> m_sortCriterias6;

	CRuArrayList<UINT64>					m_sortCriterias[7];								//!< Sorting criteria
	CRuRadixSort							m_radixSort;									//!< Rendering queue sorting mechanism

	// Traversal stacks & queues
	CRuStack<CRuEntity *>					m_entityStack;									//!< Stack used for traversing an entity tree
	CRuQueue<const CRuParameterBlock*>		m_parameterBlocks;

	// Matrix stacks
	CRuStack<CRuMatrix4x4 *>				m_usedMatrixStack;								//!< Used temporary matrix stack
	CRuStack<CRuMatrix4x4 *>				m_unusedMatrixStack;							//!< Unused temporary matrix stack

	// Parameter cache stacks
	CRuStack<CRuParameterCache *>			m_usedParameterCacheStack;
	CRuStack<CRuParameterCache *>			m_unusedParameterCacheStack;

	// Statistics
	UINT32									m_numPrimitivesRendered;						//!< Total number of rendered primitives this frame
	UINT32									m_numBatchesIssued;								//!< Total number of batches issued this frame

	// Probes
	INT32									m_probe_3D_Submit;
	INT32									m_probe_3D_FaceCount;
	INT32									m_probe_3D_BatchCount;
	INT32									m_probe_3D_Flush;
	INT32									m_probe_3D_Skinning;
	INT32									m_probe_3D_Morphing;
	INT32									m_probe_3D_Sorting;
	INT32									m_probe_3D_Parameters;
	INT32									m_probe_3D_Lighting;
	INT32									m_probe_3D_TransformSetup;
	INT32									m_probe_3D_ParameterSetup;
	INT32									m_probe_3D_TextureSetup;
	INT32									m_probe_3D_RenderAllPasses;
	INT32									m_probe_3D_Render;
	INT32									m_probe_3D_RenderD3D;
	INT32									m_probe_Engine_InstancingTime;
	INT32									m_probe_Misc_AntiPortalDiscarded;

public:
	// Other options
	BOOL									m_disableRender;

public:
											CRuRendererNode_RenderContext();
	virtual									~CRuRendererNode_RenderContext();

	BOOL									SetRenderQueueSize(INT32 renderQueueSize);

	IRuObject*								GetRenderTarget();
	BOOL									SetRenderTarget(IRuObject *renderTarget);
	BOOL									SetViewport(IRuViewport *viewport);
	BOOL									SetDefaultClearParameters(DWORD clearFlags, RUCOLOR color, REAL z, DWORD stencil);
	BOOL									SetScissorRect(CRuRectangle *scissorRect);

	void									EnableLighting(BOOL enable);
	void									EnableCulling(BOOL enable);
	void									EnableRenderQueueSorting(BOOL enable);

	CRuEvent&								Event_Submit();
	CRuEvent&								Event_Submitted();
	CRuEvent&								Event_Culled();

	BOOL									ClipPlaneSet_Push(CRuClipPlaneSet *clipPlaneSet);
	BOOL									ClipPlaneSet_Pop();

	BOOL									ParameterBlock_Push(CRuParameterBlock *paramBlock);
	BOOL									ParameterBlock_Pop();

	BOOL									Camera_Push(CRuCamera *camera);
	BOOL									Camera_Pop();

	BOOL									EventSuppression_Push(RuEntityEvent suppressedEvent);
	BOOL									EventSuppression_Pop();

	BOOL									MaterialOverrideCallback_Push(CRuRendererNode_RenderContext::MaterialOverrideCallback overrideCallback);
	BOOL									MaterialOverrideCallback_Pop();

	BOOL									Submit(CRuEntity *sceneRoot, BOOL noStatusMod = FALSE);
	BOOL									Submit(CRuEntity *sceneRoot, IRuMesh *mesh, IRuMaterial *material, CRuMatrix4x4 *worldTransform = NULL);

	virtual void							SetParentRenderer(IRuObject *renderer);
	virtual void							Begin();
	virtual void							End();
	virtual void							Execute(CRuKeyHash<PTRVALUE> &executionMap);

protected:
	BOOL									IsEventSuppressed(RuEntityEvent entityEvent);

	void									Submit_Internal(CRuEntity *sceneRoot, CRuMatrix4x4 const *worldTransform, IRuMesh *mesh, IRuMaterial *material, BOOL firstEntityInSeries);

	void									BuildAllLightingInfo();
	void									SetupAllTransforms();
	void									InstanceAllGeometries();
	void									InstanceAllTextures();

	BOOL									SelectRenderTarget();
	void									UpdateDefaultViewportParameters();
	void									SortRenderQueue();

	CRuMatrix4x4*							CreateMatrix();
	void									FlushMatrixStack();

	CRuParameterCache*						CreateParameterCache();
	void									FlushParameterCacheStack();

	RuRenderQueueEntry&						AllocateRenderQueueEntry();
	void									UndoAllocateRenderQueueEntry();

	friend CRuRenderer;
};

// ************************************************************************************************************************************************************

__forceinline RuRenderQueueEntry& CRuRendererNode_RenderContext::AllocateRenderQueueEntry()
{
	INT32 allocatedIndex = m_renderQueue.Count();

	if(m_renderQueue.SetNumEntries(allocatedIndex + 1) == FALSE)
	{
		m_renderQueue.Add(RuRenderQueueEntry());
	}

	return m_renderQueue[allocatedIndex];
}

__forceinline void CRuRendererNode_RenderContext::UndoAllocateRenderQueueEntry()
{
	if(m_renderQueue.Count() > 0)
	{
		m_renderQueue.SetNumEntries(m_renderQueue.Count() - 1);
	}
}

// ************************************************************************************************************************************************************

extern CRuRendererNode_RenderContext*		g_ruDefaultRenderContext;

// ************************************************************************************************************************************************************

CRuRendererNode_RenderContext*				RuCreateRenderContext();
void										RuDestroyRenderContext(CRuRendererNode_RenderContext *renderContext);

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_RendererNode_RenderContext, CRuRendererNode_RenderContext)

// ************************************************************************************************************************************************************

#pragma managed(pop)
