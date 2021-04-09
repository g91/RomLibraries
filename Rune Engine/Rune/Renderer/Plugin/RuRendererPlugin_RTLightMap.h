/*!
	@file RuRendererPlugin_RTLightMap.h

	Copyright (c) 2004-2009 Runewaker Entertainment Corp. All rights reserved
*/

#pragma once

#include "../../Renderer/RuRenderer.h"
#include "../../Renderer/RendererNode/RuRendererNode_TextureCopy.h"
#include "../../Renderer/RendererNode/RuRendererNode_UI.h"

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuRendererPlugin_RTLightMap : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct RTLightMapDesc
	{
		CRuCamera*							m_lightMapCamera;								//!< Camera used for RTLightMap rendering
		IRuTexture*							m_rtLightMap;									//!< RTLightMap texture
		CRuEntity*							m_targetEntity;
		INT32								m_snapshotTTL;									//!< Time-to-Live for the current RTLightMap snapshot
		INT32								m_TTL;											//!< TTL for the RTLightMapDesc
	};

	CRuMTRandom								m_randGen;
	CRuHashGenerator						m_hashGen;
	CRuHashMap<CRuGUID, RTLightMapDesc>*	m_rtLightMapHash;								//!< RTLightMap hash
	CRuArrayList<CRuEntity *>				m_candidateEntities;							//!< Candidate entities for RTLightMap
	CRuArrayList<CRuEntity *>				m_rootCandidates;

	// Runtime stuff
	BOOL									m_enabled;
	INT32									m_rtLightMapSize;
	IRuTexture*								m_renderTarget;
	CRuParameterBlock*						m_commonParamBlock;
	CRuParameterBlock*						m_targetEntityParamBlock;

	CRuStack<CRuCamera *>						m_unusedCameras;								//!< Stack of unused cameras
	CRuStack<IRuTexture *>						m_unusedTextures;								//!< Stack of unused textures
	CRuStack<CRuRendererNode_RenderContext *>	m_unusedRenderContexts;
	CRuStack<CRuRendererNode_RenderContext *>	m_usedRenderContexts;

	CRuStack<CRuEntity *>					m_workStack;
	CRuArrayList<CRuGUID>					m_gcDescriptors;

	INT32									m_blargh;

public:
											CRuRendererPlugin_RTLightMap();
	virtual									~CRuRendererPlugin_RTLightMap();

	BOOL									RegisterPlugin(CRuRendererNode_RenderContext *renderContext);

	BOOL									HandleEvent_Submit(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Begin(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Execute(RuEventArgs *eventArgs);

	BOOL									HandleEvent_DeviceReset(RuEventArgs *eventArgs);

	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);

protected:
	CRuCamera*								CreateCamera();
	IRuTexture*								CreateTexture();
	CRuRendererNode_RenderContext*			CreateRenderContext();

	BOOL									ExecuteHashCallback(const void *key, void *data);
	BOOL									DeviceResetCallback(const void *key, void *data);
	BOOL									GCCallback(const void *key, void *data);
	BOOL									ShutdownCallback(const void *key, void *data);

	void									GetSettingsFromEngine();

	// Event handlers
	BOOL									EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
