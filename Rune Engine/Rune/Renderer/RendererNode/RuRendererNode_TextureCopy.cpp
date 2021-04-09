/*!
	@file RuRendererNode_TextureCopy.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../../Renderer/RendererNode/RuRendererNode_TextureCopy.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuRendererNode_TextureCopy, CRuRendererNode)
ruHEAP_DEFINE_SUBCLASS(CRuRendererNode_TextureCopy, "CRuRendererNode_TextureCopy", "CRuRendererNode")
ruCLASSGUID_DEFINE(CRuRendererNode_TextureCopy, 0x452EB883, 0xFE9D41b3, 0x82C73941, 0xF2EEF26C)

// ************************************************************************************************************************************************************

CRuRendererNode_TextureCopy::CRuRendererNode_TextureCopy()
:	m_srcTexture(NULL),
	m_tgtTexture(NULL)
{
}

CRuRendererNode_TextureCopy::~CRuRendererNode_TextureCopy()
{
	ruSAFE_RELEASE(m_srcTexture);
	ruSAFE_RELEASE(m_tgtTexture);
}

IRuEngineResource *CRuRendererNode_TextureCopy::GetSourceTexture()
{
	return m_srcTexture;
}

void CRuRendererNode_TextureCopy::SetSourceTexture(IRuEngineResource *srcTexture)
{
	srcTexture->AddRef();
	ruSAFE_RELEASE(m_srcTexture);
	m_srcTexture = srcTexture;
}

IRuEngineResource *CRuRendererNode_TextureCopy::GetTargetTexture()
{
	return m_tgtTexture;
}

void CRuRendererNode_TextureCopy::SetTargetTexture(IRuEngineResource *tgtTexture)
{
	tgtTexture->AddRef();
	ruSAFE_RELEASE(m_tgtTexture);
	m_tgtTexture = tgtTexture;
}

void CRuRendererNode_TextureCopy::SetParentRenderer(IRuObject *renderer)
{
}

void CRuRendererNode_TextureCopy::Begin()
{
	// Chain call to base class
	CRuRendererNode::Begin();

}

void CRuRendererNode_TextureCopy::End()
{
	// Chain call to base class
	CRuRendererNode::End();

	ruSAFE_RELEASE(m_srcTexture);
	ruSAFE_RELEASE(m_tgtTexture);
}

void CRuRendererNode_TextureCopy::Execute(CRuKeyHash<PTRVALUE> &executionMap)
{
	// Chain execute call to base class
	CRuRendererNode::Execute(executionMap);

	if(m_srcTexture && m_tgtTexture)
	{
		g_ruEngine->CopyTexture(m_srcTexture, m_tgtTexture, 0);
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
