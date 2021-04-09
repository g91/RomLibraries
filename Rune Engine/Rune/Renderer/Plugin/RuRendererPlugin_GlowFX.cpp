/*!
	@file RuRendererPlugin_GlowFX.cpp

	Copyright (c) 2004-2007 All rights reserved
*/

#include "../../Renderer/Plugin/RuRendererPlugin_GlowFX.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuRendererPlugin_GlowFX, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuRendererPlugin_GlowFX, "CRuRendererPlugin_GlowFX", "IRuObject")
ruCLASSGUID_DEFINE(CRuRendererPlugin_GlowFX, 0xCF20350D, 0x51AF49bc, 0xB487CABF, 0x8D4ED1BD)

// ************************************************************************************************************************************************************

CRuRendererPlugin_GlowFX::CRuRendererPlugin_GlowFX()
:	m_enableGlow(FALSE),
	m_glowTextureSize(512),
	m_blurIterations(4),
	m_dummyCamera(NULL),
	m_rootRendererNode(NULL),
	m_texCopyNode(NULL)
{
	// Apply settings
	GetSettingsFromEngine();

	// Register listener on engine settings
	g_ruEngineSettings->RegisterSettingsChangeHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_GlowFX::EngineSettingsChangedHandler, this, _1, _2));
}

CRuRendererPlugin_GlowFX::~CRuRendererPlugin_GlowFX()
{
	ruSAFE_RELEASE(m_dummyCamera);
	ruSAFE_RELEASE(m_rootRendererNode);
	ruSAFE_RELEASE(m_texCopyNode);
}

BOOL CRuRendererPlugin_GlowFX::RegisterPlugin(CRuRendererNode_RenderContext *renderContext)
{
	// Register event handlers
	renderContext->Event_Begin().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_GlowFX::HandleEvent_Begin, this, _1));
	renderContext->Event_Execute().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_GlowFX::HandleEvent_Execute, this, _1));

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

BOOL CRuRendererPlugin_GlowFX::HandleEvent_Begin(RuEventArgs *eventArgs)
{
	// Claim render target textures from the resource manager
	m_srcTex = g_ruResourceManager->CreateRenderTarget(m_glowTextureSize, m_glowTextureSize, ruSURFACEFMT_A8R8G8B8);
	m_tex0 = g_ruResourceManager->CreateRenderTarget(m_glowTextureSize, m_glowTextureSize, ruSURFACEFMT_A8R8G8B8);
	m_tex1 = g_ruResourceManager->CreateRenderTarget(m_glowTextureSize,m_glowTextureSize, ruSURFACEFMT_A8R8G8B8);

	m_handling = FALSE;

	// Attach the root node to the renderer
	g_ruRenderer->AttachRendererNode(m_rootRendererNode);

	// Setup dependency
	m_rootRendererNode->AddDependency(g_ruDefaultRenderContext, TRUE);

	return TRUE;
}

BOOL CRuRendererPlugin_GlowFX::HandleEvent_Execute(RuEventArgs *eventArgs)
{
	CRuRendererEvent_Execute_Args *executeArgs = static_cast<CRuRendererEvent_Execute_Args *>(eventArgs);

	if(m_enableGlow == FALSE)
	{
		return TRUE;
	}

	// If the flushed event is already being handled, or if either scratch texture is unavailable, abort
	if(m_handling || m_srcTex == NULL || m_tex0 == NULL || m_tex1 == NULL)
	{
		return TRUE;
	}

	// Mark the plugin state to be in flush handling mode
	m_handling = TRUE;

	if(executeArgs->m_rendererNode->GetType() == CRuRendererNode_RenderContext::Type())
	{
		CRuRendererNode_RenderContext *targetRC = static_cast<CRuRendererNode_RenderContext *>(executeArgs->m_rendererNode);

		IRuObject *rtObject = targetRC->GetRenderTarget();

		if(rtObject->GetType().IsSubClassOf(IRuEngineResource::Type()))
		{
			// Copy source render target onto our source texture
			m_texCopyNode->SetSourceTexture(static_cast<IRuEngineResource *>(rtObject));
			m_texCopyNode->SetTargetTexture(m_srcTex);
			m_rootRendererNode->AddChild(m_texCopyNode);

			// Begin isolated UI rendering
			g_ruUIRenderer->IsolationRender_Begin();

			// Set rect sizes
			CRuRectangle srcRect, destRect;
			srcRect.Set(0.0f, 0.0f, static_cast<REAL>(m_tex0->GetLevelWidth(0)), static_cast<REAL>(m_tex0->GetLevelHeight(0)));
			destRect.Set(0.0f, 0.0f, static_cast<REAL>(m_tex1->GetLevelWidth(0)), static_cast<REAL>(m_tex1->GetLevelHeight(0)));

			// Push dummy camera into renderer
			g_ruUIRenderer->Camera_Push(m_dummyCamera);

			// Disable 2D viewport scale
			g_ruUIRenderer->ViewportScale_Push(0.0f, 0.0f);

			// Set 2D output mode to direct addressing mode
			g_ruUIRenderer->SetUVMode(ru2DUVMODE_DIRECT);

			// Blur render target texture
			for(INT32 i = 0; i < m_blurIterations; ++i)
			{
				// Pick target texture (scratch texture 1) and begin 2D rendering
				g_ruUIRenderer->RenderTarget_Push(m_tex1);

				// Set up Gaussian filter
				g_ruUIRenderer->SetEffectType(ru2DFXTYPE_GAUSSIAN);
				g_ruUIRenderer->SetEffectParameter(ru2DFXPARAM_COLOR, RUCOLOR_ARGB(255, 255, 255, 255));
				g_ruUIRenderer->SetTexture(i == 0 ? m_srcTex : m_tex0, FALSE);
				g_ruUIRenderer->SetShaderParameter(ruPARAMTYPE_DWORD_0, 0);

				// Submit and flush
				g_ruUIRenderer->Submit(srcRect, destRect);
				g_ruUIRenderer->RenderTarget_Pop();

				// Pick target texture (scratch texture 0) and begin 2D rendering
				g_ruUIRenderer->RenderTarget_Push(m_tex0);

				// Set up Gaussian filter
				g_ruUIRenderer->SetEffectType(ru2DFXTYPE_GAUSSIAN);
				g_ruUIRenderer->SetEffectParameter(ru2DFXPARAM_COLOR, RUCOLOR_ARGB(255, 255, 255, 255));
				g_ruUIRenderer->SetTexture(m_tex1, FALSE);
				g_ruUIRenderer->SetShaderParameter(ruPARAMTYPE_DWORD_0, 1);

				// Submit and flush
				g_ruUIRenderer->Submit(srcRect, destRect);
				g_ruUIRenderer->RenderTarget_Pop();
			}

			// *** Blend glow texture onto back buffer

			// Select affected swap chain and begin 2D rendering
			g_ruUIRenderer->RenderTarget_Push(rtObject);

			// Setup alpha blit
			g_ruUIRenderer->SetEffectType(ru2DFXTYPE_BLIT_FILTER);
			g_ruUIRenderer->SetEffectParameter(ru2DFXPARAM_COLOR, RUCOLOR_ARGB(64, 255, 255, 255));
			g_ruUIRenderer->SetTexture(m_tex0, FALSE);

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

			// *** Run exponentiation filter

			srcRect.Set(0.0f, 0.0f, static_cast<REAL>(m_tex0->GetLevelWidth(0)), static_cast<REAL>(m_tex0->GetLevelHeight(0)));
			destRect.Set(0.0f, 0.0f, static_cast<REAL>(m_tex1->GetLevelWidth(0)), static_cast<REAL>(m_tex1->GetLevelHeight(0)));

			// Pick target texture (scratch texture 0) and begin 2D rendering
			g_ruUIRenderer->RenderTarget_Push(m_tex0);

			// Set up exponentiation filter
			g_ruUIRenderer->SetEffectType(ru2DFXTYPE_EXPONENTIATE);
			g_ruUIRenderer->SetEffectParameter(ru2DFXPARAM_COLOR, RUCOLOR_ARGB(255, 255, 255, 255));
			g_ruUIRenderer->SetTexture(m_srcTex, FALSE);

			// Submit and flush
			g_ruUIRenderer->Submit(srcRect, destRect);
			g_ruUIRenderer->RenderTarget_Pop();

			// *** Blur render target texture
			for(INT32 i = 0; i < m_blurIterations; ++i)
			{
				// Pick target texture (scratch texture 1) and begin 2D rendering
				g_ruUIRenderer->RenderTarget_Push(m_tex1);

				// Set up Gaussian filter
				g_ruUIRenderer->SetEffectType(ru2DFXTYPE_GAUSSIAN);
				g_ruUIRenderer->SetEffectParameter(ru2DFXPARAM_COLOR, RUCOLOR_ARGB(255, 255, 255, 255));
				g_ruUIRenderer->SetTexture(m_tex0, FALSE);
				g_ruUIRenderer->SetShaderParameter(ruPARAMTYPE_DWORD_0, 0);

				// Submit and flush
				g_ruUIRenderer->Submit(srcRect, destRect);
				g_ruUIRenderer->RenderTarget_Pop();

				// Pick target texture (scratch texture 0) and begin 2D rendering
				g_ruUIRenderer->RenderTarget_Push(m_tex0);

				// Set up Gaussian filter
				g_ruUIRenderer->SetEffectType(ru2DFXTYPE_GAUSSIAN);
				g_ruUIRenderer->SetEffectParameter(ru2DFXPARAM_COLOR, RUCOLOR_ARGB(255, 255, 255, 255));
				g_ruUIRenderer->SetTexture(m_tex1, FALSE);
				g_ruUIRenderer->SetShaderParameter(ruPARAMTYPE_DWORD_0, 1);

				// Submit and flush
				g_ruUIRenderer->Submit(srcRect, destRect);
				g_ruUIRenderer->RenderTarget_Pop();
			}

			// *** Add glow texture onto back buffer

			// Select affected swap chain and begin 2D rendering
			g_ruUIRenderer->RenderTarget_Push(rtObject);

			// Setup alpha blit
			g_ruUIRenderer->SetEffectType(ru2DFXTYPE_BLIT_ADD);
			g_ruUIRenderer->SetEffectParameter(ru2DFXPARAM_COLOR, RUCOLOR_ARGB(128, 255, 255, 255));
			g_ruUIRenderer->SetTexture(m_tex0, FALSE);

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
		}
	}

	return TRUE;
}

void CRuRendererPlugin_GlowFX::GetSettingsFromEngine()
{
	RuPropertyType propertyValue;

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
}

BOOL CRuRendererPlugin_GlowFX::EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting)
{
	GetSettingsFromEngine();
	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
