/*!
	@file RuRendererPlugin_SubmergeFX.cpp

	Copyright (c) 2004-2009 All rights reserved
*/

#include "../../Renderer/Plugin/RuRendererPlugin_SubmergeFX.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuRendererPlugin_SubmergeFX, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuRendererPlugin_SubmergeFX, "CRuRendererPlugin_SubmergeFX", "IRuObject")
ruCLASSGUID_DEFINE(CRuRendererPlugin_SubmergeFX, 0xEEB5DC55, 0x29FC4382, 0xBC904DE9, 0xF51E9A0F)

// ************************************************************************************************************************************************************

CRuRendererPlugin_SubmergeFX::CRuRendererPlugin_SubmergeFX()
:	m_bumpStrength(0.05f),
	m_lastExecuteTime(0),
	m_t(0.0f),
	m_renderContext(NULL),
	m_dummyCamera(NULL),
	m_rootRendererNode(NULL),
	m_texCopyNode(NULL)
{
	// Apply settings
	GetSettingsFromEngine();

	// Register dispose event on self
	this->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_SubmergeFX::DisposeHandler, this, _1));

	// Register listener on engine settings
	g_ruEngineSettings->RegisterSettingsChangeHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_SubmergeFX::EngineSettingsChangedHandler, this, _1, _2));
}

CRuRendererPlugin_SubmergeFX::~CRuRendererPlugin_SubmergeFX()
{
}

BOOL CRuRendererPlugin_SubmergeFX::RegisterPlugin(CRuRendererNode_RenderContext *renderContext)
{
	// Register event handlers
	m_renderContext = renderContext;
	m_renderContext->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_SubmergeFX::DisposeHandler, this, _1));
	m_renderContext->Event_Begin().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_SubmergeFX::HandleEvent_Begin, this, _1));
	m_renderContext->Event_Execute().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_SubmergeFX::HandleEvent_Execute, this, _1));

	// Initialize dummy camera
	m_dummyCamera = ruNEW CRuCamera();

	// Initialize the root renderer node
	m_rootRendererNode = ruNEW CRuRendererNode();
	m_rootRendererNode->PersistAcrossFrames(FALSE);

	// Create the texture copy node
	m_texCopyNode = ruNEW CRuRendererNode_TextureCopy();
	m_texCopyNode->PersistAcrossFrames(FALSE);

	return TRUE;
}

BOOL CRuRendererPlugin_SubmergeFX::SetDistortionTextureTrack(CRuAnimTrack<char *> *track)
{
	track->CopyTo(&m_track_BumpTexture);

	return TRUE;
}

BOOL CRuRendererPlugin_SubmergeFX::SetDistortionStrength(REAL strength)
{
	m_bumpStrength = strength;
	return TRUE;
}

BOOL CRuRendererPlugin_SubmergeFX::HandleEvent_Begin(RuEventArgs *eventArgs)
{
	CRuRendererEvent_Begin_Args *beginArgs = static_cast<CRuRendererEvent_Begin_Args *>(eventArgs);

	// Ensure that the renderer node is a render context
	if(beginArgs->m_rendererNode->GetType() != CRuRendererNode_RenderContext::Type())
	{
		return FALSE;
	}

	// Retrieve source render target from the render context
	CRuRendererNode_RenderContext *targetRC = static_cast<CRuRendererNode_RenderContext *>(beginArgs->m_rendererNode);
	IRuObject *rtObject = targetRC->GetRenderTarget();

	// Retrieve render target dimensions
	INT32 rtWidth = 512, rtHeight = 512;
	if(rtObject->GetType().IsSubClassOf(IRuSwapChain::Type()))
	{
		rtWidth = static_cast<INT32>(static_cast<IRuSwapChain *>(rtObject)->GetWidth());
		rtHeight = static_cast<INT32>(static_cast<IRuSwapChain *>(rtObject)->GetHeight());
	}
	else if(rtObject->GetType().IsSubClassOf(IRuTexture::Type()))
	{
		rtWidth = static_cast<INT32>(static_cast<IRuTexture *>(rtObject)->GetLevelWidth(0));
		rtHeight = static_cast<INT32>(static_cast<IRuTexture *>(rtObject)->GetLevelHeight(0));
	}

	// Claim render target textures from the resource manager
	m_srcTex = g_ruResourceManager->CreateRenderTarget(rtWidth, rtHeight, ruSURFACEFMT_A8R8G8B8);

	m_handling = FALSE;

	// Attach the root node to the renderer
	g_ruRenderer->AttachRendererNode(m_rootRendererNode);

	// Setup transient dependency
	m_rootRendererNode->AddDependency(g_ruDefaultRenderContext, TRUE);

	return TRUE;
}

BOOL CRuRendererPlugin_SubmergeFX::HandleEvent_Execute(RuEventArgs *eventArgs)
{
	CRuRendererEvent_Execute_Args *executeArgs = static_cast<CRuRendererEvent_Execute_Args *>(eventArgs);

	// If the execute event is already being handled, or if either scratch texture is unavailable, abort
	if(m_handling || m_srcTex == NULL)
	{
		return TRUE;
	}

	// Mark the plugin state to be in execute handling mode
	m_handling = TRUE;

	// Ensure that the renderer node is a render context
	if(executeArgs->m_rendererNode->GetType() != CRuRendererNode_RenderContext::Type())
	{
		return TRUE;
	}

	// Retrieve source render target from the render context
	CRuRendererNode_RenderContext *targetRC = static_cast<CRuRendererNode_RenderContext *>(executeArgs->m_rendererNode);
	IRuObject *rtObject = targetRC->GetRenderTarget();

	// Ensure that the source render target is a valid engine resource type
	if(!rtObject->GetType().IsSubClassOf(IRuEngineResource::Type()))
	{
		return TRUE;
	}

	// Copy source render target onto our source texture
	m_texCopyNode->SetSourceTexture(static_cast<IRuEngineResource *>(rtObject));
	m_texCopyNode->SetTargetTexture(m_srcTex);
	m_rootRendererNode->AddChild(m_texCopyNode);

	// Begin isolated UI rendering
	g_ruUIRenderer->IsolationRender_Begin();

	// Set rect sizes
	CRuRectangle srcRect, destRect;
	srcRect.Set(0.0f, 0.0f, static_cast<REAL>(m_srcTex->GetLevelWidth(0)), static_cast<REAL>(m_srcTex->GetLevelHeight(0)));
	destRect.Set(0.0f, 0.0f, 0.0f, 0.0f);

	// Push dummy camera into renderer
	g_ruUIRenderer->Camera_Push(m_dummyCamera);

	// Disable 2D viewport scale
	g_ruUIRenderer->ViewportScale_Push(0.0f, 0.0f);

	// Set 2D output mode to direct addressing mode
	g_ruUIRenderer->SetUVMode(ru2DUVMODE_DIRECT);

	// Select affected swap chain and begin 2D rendering
	g_ruUIRenderer->RenderTarget_Push(rtObject);

	// Setup alpha blit
	g_ruUIRenderer->SetEffectType(ru2DFXTYPE_DISTORTION);
	g_ruUIRenderer->SetEffectParameter(ru2DFXPARAM_COLOR, RUCOLOR_ARGB(255, 255, 255, 255));
	g_ruUIRenderer->SetTexture(m_srcTex, FALSE);
	g_ruUIRenderer->SetShaderParameter(ruPARAMTYPE_CUSTOM_4, RuCastFloatToINT64(m_bumpStrength));

	// Update clock
	DWORD curExecTime = GetTickCount();
	m_t += (min(curExecTime - m_lastExecuteTime, 100) * 0.001f);
	m_t = fmod(m_t, m_track_BumpTexture.GetTrackLength());
	m_lastExecuteTime = curExecTime;

	// Load bump texture
	IRuTexture *bumpTex = g_ruResourceManager->LoadTexture(m_track_BumpTexture.GetDiscreteKey(m_t));
	g_ruUIRenderer->SetTexture(bumpTex, FALSE, ruTEXCHANNEL_BUMPMAP);
	ruSAFE_RELEASE(bumpTex);

	// Setup destination rectangle
	destRect.m_left = 0.0f;
	destRect.m_top = 0.0f;

	if(rtObject->GetType().IsSubClassOf(IRuSwapChain::Type()))
	{
		destRect.m_right = static_cast<REAL>(static_cast<IRuSwapChain *>(rtObject)->GetWidth());
		destRect.m_bottom = static_cast<REAL>(static_cast<IRuSwapChain *>(rtObject)->GetHeight());
	}
	else if(rtObject->GetType().IsSubClassOf(IRuTexture::Type()))
	{
		destRect.m_right = static_cast<REAL>(static_cast<IRuTexture *>(rtObject)->GetLevelWidth(0));
		destRect.m_bottom = static_cast<REAL>(static_cast<IRuTexture *>(rtObject)->GetLevelHeight(0));
	}

	// Submit and flush
	g_ruUIRenderer->Submit(srcRect, destRect);
	g_ruUIRenderer->RenderTarget_Pop();

	// Set 2D output mode to offset mode (default setting)
	g_ruUIRenderer->SetUVMode(ru2DUVMODE_OFFSET);

	// Restore 2D viewport scale
	g_ruUIRenderer->ViewportScale_Pop();

	// Terminate isolation render
	CRuRendererNode *fxRendererNode = g_ruUIRenderer->IsolationRender_End();
	fxRendererNode->PersistAcrossFrames(FALSE);

	m_rootRendererNode->AddChild(fxRendererNode);
	ruSAFE_RELEASE(fxRendererNode);

	return TRUE;
}

void CRuRendererPlugin_SubmergeFX::GetSettingsFromEngine()
{
	RuPropertyType propertyValue;
/*
	if(g_ruEngineSettings->GetSetting("glowFX:enable", propertyValue))
	{
		m_enableGlow = boost::get<BOOL>(propertyValue);
	}

	if(g_ruEngineSettings->GetSetting("glowFX:texture size", propertyValue))
	{
		m_glowTextureSize = boost::get<INT32>(propertyValue);
	}

	if(g_ruEngineSettings->GetSetting("glowFX:blur iterations", propertyValue))
	{
		m_blurIterations = boost::get<INT32>(propertyValue);
	}
*/
}

BOOL CRuRendererPlugin_SubmergeFX::DisposeHandler(RuEventArgs *eventArgs)
{
	// Unregister handlers
	this->Event_Dispose().UnregisterHandler(this->GetGUID());
	g_ruEngineSettings->UnregisterSettingsChangeHandler(this->GetGUID());

	if(m_renderContext)
	{
		m_renderContext->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_renderContext->Event_Begin().UnregisterHandler(this->GetGUID());
		m_renderContext->Event_Execute().UnregisterHandler(this->GetGUID());
	}

	m_renderContext = NULL;

	// Release resources
	ruSAFE_RELEASE(m_dummyCamera);
	ruSAFE_RELEASE(m_rootRendererNode);
	ruSAFE_RELEASE(m_texCopyNode);

	return TRUE;
}

BOOL CRuRendererPlugin_SubmergeFX::EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting)
{
	GetSettingsFromEngine();
	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
