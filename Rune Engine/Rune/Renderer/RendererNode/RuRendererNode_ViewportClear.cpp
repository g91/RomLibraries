/*!
	@file RuRendererNode_ViewportClear.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../../Renderer/RendererNode/RuRendererNode_ViewportClear.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuRendererNode_ViewportClear, CRuRendererNode)
ruHEAP_DEFINE_SUBCLASS(CRuRendererNode_ViewportClear, "CRuRendererNode_ViewportClear", "CRuRendererNode")
ruCLASSGUID_DEFINE(CRuRendererNode_ViewportClear, 0x0506B1C7, 0x6C3647ec, 0x888F69B6, 0x5EEA63CA)

// ************************************************************************************************************************************************************

CRuRendererNode_ViewportClear::CRuRendererNode_ViewportClear()
:	m_viewport(NULL)
{
}

CRuRendererNode_ViewportClear::~CRuRendererNode_ViewportClear()
{
	ruSAFE_RELEASE(m_viewport);
}

IRuViewport *CRuRendererNode_ViewportClear::GetViewport()
{
	return m_viewport;
}

void CRuRendererNode_ViewportClear::SetViewport(IRuViewport *viewport)
{
	if(viewport)
	{
		viewport->AddRef();
		ruSAFE_RELEASE(m_viewport);
		m_viewport = viewport;
	}
	else
	{
		ruSAFE_RELEASE(m_viewport);
	}
}

void CRuRendererNode_ViewportClear::SetParentRenderer(IRuObject *renderer)
{
	// Chain call to base class
	CRuRendererNode::SetParentRenderer(renderer);
}

void CRuRendererNode_ViewportClear::Begin()
{
	// Chain call to base class
	CRuRendererNode::Begin();
}

void CRuRendererNode_ViewportClear::End()
{
	// Chain call to base class
	CRuRendererNode::End();

	// Release reference to viewport object
	ruSAFE_RELEASE(m_viewport);
}

void CRuRendererNode_ViewportClear::Execute(CRuKeyHash<PTRVALUE> &executionMap)
{
	// Chain execute call to base class
	CRuRendererNode::Execute(executionMap);

	if(m_viewport)
	{
		// Select viewport
		m_viewport->SelectAsViewport();

		// Clear viewport according to its clear parameters
		g_ruEngine->Clear(m_viewport->GetClearFlags(), m_viewport->GetClearColor(), m_viewport->GetClearZ(), m_viewport->GetClearStencil());
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
