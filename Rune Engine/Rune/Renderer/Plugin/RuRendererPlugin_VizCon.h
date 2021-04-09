/*!
	@file RuRendererPlugin_VizCon.h

	Copyright (c) 2004-2007 All rights reserved
*/

#ifndef _RURENDERERPLUGIN_VIZCON_H_
#define _RURENDERERPLUGIN_VIZCON_H_

#include "../../Renderer/RuRenderer.h"
#include "../../Renderer/RendererNode/RuRendererNode_RenderContext.h"

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuRendererPlugin_VizCon : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	BOOL									m_enabled;

public:
											CRuRendererPlugin_VizCon();
	virtual									~CRuRendererPlugin_VizCon();

	BOOL									RegisterPlugin(CRuRendererNode_RenderContext *renderContext);

	BOOL									HandleEvent_Submit(RuEventArgs *eventArgs);

protected:
	RuEntityCullMode						GetEffectiveCullMode(CRuEntity *entity);
	void									GetSettingsFromEngine();

	// Event handlers
	BOOL									EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
