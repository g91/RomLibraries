/*!
	@file RuRendererNode_SetScissor.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../../Renderer/RendererNode/RuRendererNode_SetScissor.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuRendererNode_SetScissor, CRuRendererNode)
ruHEAP_DEFINE_SUBCLASS(CRuRendererNode_SetScissor, "CRuRendererNode_SetScissor", "CRuRendererNode")
ruCLASSGUID_DEFINE(CRuRendererNode_SetScissor, 0x3AB8391C, 0x3FDA4899, 0x8B7B6C2A, 0x7FFF6A74)

// ************************************************************************************************************************************************************

CRuRendererNode_SetScissor::CRuRendererNode_SetScissor()
:	m_scissorRect(0.0f, 0.0f, -1.0f, -1.0f)
{
}

CRuRendererNode_SetScissor::~CRuRendererNode_SetScissor()
{
}

const CRuRectangle *CRuRendererNode_SetScissor::GetScissorRect()
{
	if((m_scissorRect.m_right - m_scissorRect.m_left) > 0.0f && (m_scissorRect.m_bottom - m_scissorRect.m_top) > 0.0f)
	{
		return &m_scissorRect;
	}

	return NULL;
}

void CRuRendererNode_SetScissor::SetScissorRect(const CRuRectangle *scissorRect)
{
	if(scissorRect)
	{
		m_scissorRect = *scissorRect;
	}
	else
	{
		m_scissorRect.Set(0.0f, 0.0f, -1.0f, -1.0f);
	}
}

void CRuRendererNode_SetScissor::SetParentRenderer(IRuObject *renderer)
{
	// Chain call to base class
	CRuRendererNode::SetParentRenderer(renderer);
}

void CRuRendererNode_SetScissor::Begin()
{
	// Chain call to base class
	CRuRendererNode::Begin();
}

void CRuRendererNode_SetScissor::End()
{
	// Chain call to base class
	CRuRendererNode::End();
}

void CRuRendererNode_SetScissor::Execute(CRuKeyHash<PTRVALUE> &executionMap)
{
	// Chain execute call to base class
	CRuRendererNode::Execute(executionMap);

	if((m_scissorRect.m_right - m_scissorRect.m_left) > 0.0f && (m_scissorRect.m_bottom - m_scissorRect.m_top) > 0.0f)
	{
		RECT rect;

		rect.left = (LONG)m_scissorRect.m_left;
		rect.right = (LONG)m_scissorRect.m_right;
		rect.top = (LONG)m_scissorRect.m_top;
		rect.bottom = (LONG)m_scissorRect.m_bottom;

		g_ruEngine->SetScissorRect(&rect);
	}
	else
	{
		g_ruEngine->SetScissorRect(NULL);
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
