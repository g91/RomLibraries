/*!
	@file RuRendererPlugin_AquaFX.h

	Copyright (c) 2004-2007 All rights reserved
*/

#ifndef _RURENDERERPLUGIN_AQUAFX_H_
#define _RURENDERERPLUGIN_AQUAFX_H_

#include "../../Renderer/RuRenderer.h"
#include "../../Renderer/RendererNode/RuRendererNode_TextureCopy.h"
#include "../../Renderer/RendererNode/RuRendererNode_UI.h"

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuRendererPlugin_AquaFX : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct TextureDescriptor
	{
		IRuTexture*							m_renderTarget;
		CRuCamera*							m_camera;

		RuTextureChannel					m_mapChannel;									//!< Channel this map applies to

		CRuSphere							m_bodySphere;									//!< Merged bounding sphere of all water bodies using this texture
		CRuPlane4							m_splitPlane;									//!< Splitting plane (if applicable)

		CRuParameterBlock*					m_paramBlock;

		CRuRendererNode_RenderContext*		m_renderContext;
		CRuClipPlaneSet*					m_clipPlaneSet;
	};

	// Settings
	INT32									m_effectTextureSize;
	REAL									m_minimumReflectSize;

	// Run-time
	BOOL									m_requiresNoZReRender;
	IRuTexture*								m_refractionTexture;
	CRuArrayList<TextureDescriptor>			m_textureDescriptors;							//!< Texture descriptors for allocated reflection/refraction maps
	CRuArrayList<CRuEntity *>				m_candidateEntities;							//!< Candidate entities for reflection/refraction

	CRuCamera*								m_dummyCamera;

	CRuRendererNode*						m_rootRendererNode_0;							//!< Root node for things that must execute before the default render context
	CRuRendererNode*						m_rootRendererNode_1;							//!< Root node for things that must execute after the default render context
	CRuRendererNode*						m_rootRendererNode_2;							//!< Root node for things that must execute after m_rootRendererNode_1
	CRuRendererNode_TextureCopy*			m_texCopyNode;									//!< Texture copy node used for copying the refraction texture
	CRuRendererNode_RenderContext*			m_waterRenderContext;							//!< Render context used for rendering water surfaces
	CRuRendererNode_RenderContext*			m_noZRenderContext;								//!< Render context used for rendering no-Z models after the water surface render

	CRuStack<CRuCamera *>						m_unusedCameras;
	CRuStack<CRuParameterBlock *>				m_unusedParamBlocks;
	CRuStack<CRuRendererNode_RenderContext *>	m_unusedRenderContexts;
	CRuStack<CRuClipPlaneSet *>					m_unusedClipPlaneSets;

public:
											CRuRendererPlugin_AquaFX();
	virtual									~CRuRendererPlugin_AquaFX();

	BOOL									RegisterPlugin(CRuRendererNode_RenderContext *renderContext);

	BOOL									HandleEvent_Submitted(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Begin(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Execute(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Execute_2(RuEventArgs *eventArgs);

protected:
	void									GetSettingsFromEngine();

	// Event handlers
	BOOL									EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
