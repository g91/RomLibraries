/*!
	@file RuRendererPlugin_AlphaZ.h

	Copyright (c) 2004-2008 All rights reserved
*/

#ifndef _RURENDERERPLUGIN_ALPHAZ_H_
#define _RURENDERERPLUGIN_ALPHAZ_H_

#include "../../Renderer/RuRenderer.h"
#include "../../Renderer/RendererNode/RuRendererNode_RenderContext.h"

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuRendererPlugin_AlphaZ : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct MaterialDesc
	{
		CRuMaterial*							m_material;
		INT32									m_gcAge;
	};

	CRuKeyHash<CRuGUID>*						m_registeredEntities;
	CRuHashMap<CRuGUID, MaterialDesc>*			m_materialHash;

	CRuArrayList<CRuGUID>						m_gc_Materials;

public:
												CRuRendererPlugin_AlphaZ();
	virtual										~CRuRendererPlugin_AlphaZ();

	BOOL										RegisterPlugin(CRuRendererNode_RenderContext *renderContext);

	BOOL										RegisterEntity(CRuEntity *entity);

	BOOL										HandleEvent_Begin(RuEventArgs *eventArgs);
	BOOL										HandleEvent_Submitted(RuEventArgs *eventArgs);
	BOOL										HandleEvent_Dispose(RuEventArgs *eventArgs);

protected:
	BOOL										MaterialClearCallback(const void *key, void *data);
	BOOL										MaterialGCCallback(const void *key, void *data);
};

// ************************************************************************************************************************************************************

extern CRuRendererPlugin_AlphaZ*				g_ruRendererPlugin_AlphaZ;

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
