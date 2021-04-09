/*!
	@file RuRendererPlugin_GlowFX.h

	Copyright (c) 2004-2007 All rights reserved
*/

#ifndef _RURENDERERPLUGIN_GLOWFX_H_
#define _RURENDERERPLUGIN_GLOWFX_H_

#include "../../Renderer/RuRenderer.h"
#include "../../Renderer/RendererNode/RuRendererNode_TextureCopy.h"
#include "../../Renderer/RendererNode/RuRendererNode_UI.h"

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuRendererPlugin_GlowFX : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Settings
	BOOL									m_enableGlow;
	INT32									m_glowTextureSize;
	INT32									m_blurIterations;

	// Run-time
	IRuTexture*								m_srcTex;
	IRuTexture*								m_tex0;
	IRuTexture*								m_tex1;

	BOOL									m_handling;

	CRuCamera*								m_dummyCamera;

	CRuRendererNode*						m_rootRendererNode;
	CRuRendererNode_TextureCopy*			m_texCopyNode;

public:
											CRuRendererPlugin_GlowFX();
	virtual									~CRuRendererPlugin_GlowFX();

	BOOL									RegisterPlugin(CRuRendererNode_RenderContext *renderContext);

	BOOL									HandleEvent_Begin(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Execute(RuEventArgs *eventArgs);

protected:
	void									GetSettingsFromEngine();

	// Event handlers
	BOOL									EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
