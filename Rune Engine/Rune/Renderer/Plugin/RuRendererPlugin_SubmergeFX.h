/*!
	@file RuRendererPlugin_SubmergeFX.h

	Copyright (c) 2004-2009 All rights reserved
*/

#pragma  once

#include "../../Renderer/RuRenderer.h"
#include "../../Renderer/RendererNode/RuRendererNode_TextureCopy.h"
#include "../../Renderer/RendererNode/RuRendererNode_UI.h"

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuRendererPlugin_SubmergeFX : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Settings
	CRuAnimTrack<char *>					m_track_BumpTexture;								//!< Texture name track
	REAL									m_bumpStrength;

	// Run-time
	CRuRendererNode_RenderContext*			m_renderContext;

	DWORD									m_lastExecuteTime;
	REAL									m_t;
	IRuTexture*								m_srcTex;

	BOOL									m_handling;

	CRuCamera*								m_dummyCamera;

	CRuRendererNode*						m_rootRendererNode;
	CRuRendererNode_TextureCopy*			m_texCopyNode;

public:
											CRuRendererPlugin_SubmergeFX();
	virtual									~CRuRendererPlugin_SubmergeFX();

	BOOL									RegisterPlugin(CRuRendererNode_RenderContext *renderContext);

	BOOL									SetDistortionTextureTrack(CRuAnimTrack<char *> *track);
	BOOL									SetDistortionStrength(REAL strength);

	BOOL									HandleEvent_Begin(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Execute(RuEventArgs *eventArgs);

protected:
	void									GetSettingsFromEngine();

	// Event handlers
	BOOL									DisposeHandler(RuEventArgs *eventArgs);
	BOOL									EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
