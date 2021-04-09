/*!
	@file RuRendererNode_TextureCopy.h

	Copyright (c) 2004-2008 All rights reserved
*/

#ifndef _RURENDERERNODE_TEXTURECOPY_H_
#define _RURENDERERNODE_TEXTURECOPY_H_

#include "../../Renderer/RuRenderer.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuRendererNode_TextureCopy : public CRuRendererNode
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuEngineResource*						m_srcTexture;									//!< Source texture object
	IRuEngineResource*						m_tgtTexture;									//!< Target texture object

public:
											CRuRendererNode_TextureCopy();
	virtual									~CRuRendererNode_TextureCopy();

	IRuEngineResource*						GetSourceTexture();
	void									SetSourceTexture(IRuEngineResource *srcTexture);

	IRuEngineResource*						GetTargetTexture();
	void									SetTargetTexture(IRuEngineResource *tgtTexture);

	virtual void							SetParentRenderer(IRuObject *renderer);
	virtual void							Begin();
	virtual void							End();
	virtual void							Execute(CRuKeyHash<PTRVALUE> &executionMap);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_RendererNode_TextureCopy, CRuRendererNode_TextureCopy)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
