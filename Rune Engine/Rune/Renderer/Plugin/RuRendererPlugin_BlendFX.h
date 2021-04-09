/*!
	@file RuRendererPlugin_BlendFX.h

	Copyright (c) 2004-2008 All rights reserved
*/

#ifndef _RURENDERERPLUGIN_BLENDFX_H_
#define _RURENDERERPLUGIN_BLENDFX_H_

#include "../../Renderer/RuRenderer.h"
#include "../../Renderer/RendererNode/RuRendererNode_RenderContext.h"

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuRendererPlugin_BlendFX : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct MaterialDesc
	{
		CRuMaterial*						m_material;
		INT32								m_gcAge;
	};

	CRuHashMap<CRuGUID, MaterialDesc>*		m_materialHash;

	CRuArrayList<CRuGUID>					m_gc_Materials;

public:
											CRuRendererPlugin_BlendFX();
	virtual									~CRuRendererPlugin_BlendFX();

	BOOL									RegisterPlugin(CRuRendererNode_RenderContext *renderContext);

	BOOL									HandleEvent_Begin(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Submitted(RuEventArgs *eventArgs);

protected:
	BOOL									MaterialClearCallback(const void *key, void *data);
	BOOL									MaterialGCCallback(const void *key, void *data);
	IRuMaterial*							MaterialOverrideFunctor(IRuMaterial *sourceMaterial);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
