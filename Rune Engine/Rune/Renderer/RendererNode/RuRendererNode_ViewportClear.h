/*!
	@file RuRendererNode_ViewportClear.h

	Copyright (c) 2004-2008 All rights reserved
*/

#ifndef _RURENDERERNODE_VIEWPORTCLEAR_H_
#define _RURENDERERNODE_VIEWPORTCLEAR_H_

#include "../../Renderer/RuRenderer.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuRendererNode_ViewportClear : public CRuRendererNode
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuViewport*							m_viewport;

public:
											CRuRendererNode_ViewportClear();
	virtual									~CRuRendererNode_ViewportClear();

	IRuViewport*							GetViewport();
	void									SetViewport(IRuViewport *viewport);

	virtual void							SetParentRenderer(IRuObject *renderer);
	virtual void							Begin();
	virtual void							End();
	virtual void							Execute(CRuKeyHash<PTRVALUE> &executionMap);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_RendererNode_ViewportClear, CRuRendererNode_ViewportClear)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
