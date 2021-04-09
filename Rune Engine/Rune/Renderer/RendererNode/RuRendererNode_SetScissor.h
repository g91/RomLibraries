/*!
	@file RuRendererNode_SetScissor.h

	Copyright (c) 2004-2008 All rights reserved
*/

#ifndef _RURENDERERNODE_SETSCISSOR_H_
#define _RURENDERERNODE_SETSCISSOR_H_

#include "../../Renderer/RuRenderer.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuRendererNode_SetScissor : public CRuRendererNode
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuRectangle							m_scissorRect;

public:
											CRuRendererNode_SetScissor();
	virtual									~CRuRendererNode_SetScissor();

	const CRuRectangle*						GetScissorRect();
	void									SetScissorRect(const CRuRectangle *scissorRect);

	virtual void							SetParentRenderer(IRuObject *renderer);
	virtual void							Begin();
	virtual void							End();
	virtual void							Execute(CRuKeyHash<PTRVALUE> &executionMap);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_RendererNode_SetScissor, CRuRendererNode_SetScissor)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
