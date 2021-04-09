/*!
	@file RuRendererPlugin_DistortFX.cpp

	Copyright (c) 2004-2007 All rights reserved
*/

#include "../../Renderer/Plugin/RuRendererPlugin_DistortFX.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuRendererPlugin_DistortFX, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuRendererPlugin_DistortFX, "CRuRendererPlugin_DistortFX", "IRuObject")
ruCLASSGUID_DEFINE(CRuRendererPlugin_DistortFX, 0x1D950CA4, 0x531D4c0d, 0x98A2AD96, 0xF85E1E91)

// ************************************************************************************************************************************************************

CRuRendererPlugin_DistortFX::CRuRendererPlugin_DistortFX()
:	m_distortFXEnabled(FALSE),
	m_effectTextureSize(256),
	m_dummyCamera(NULL),
	m_rootRendererNode_0(NULL),
	m_rootRendererNode_1(NULL),
	m_texCopyNode(NULL)
{
	// Apply settings
	GetSettingsFromEngine();

	// Register listener on engine settings
	g_ruEngineSettings->RegisterSettingsChangeHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_DistortFX::EngineSettingsChangedHandler, this, _1, _2));

	m_materialHash = ruNEW CRuHashMap<CRuGUID, CRuMaterial*>(512);
}

CRuRendererPlugin_DistortFX::~CRuRendererPlugin_DistortFX()
{
	m_materialHash->ForAllElements(boost::bind(&CRuRendererPlugin_DistortFX::MaterialClearCallback, this, _1, _2));
	delete m_materialHash;

	ruSAFE_RELEASE(m_dummyCamera);
	ruSAFE_RELEASE(m_rootRendererNode_0);
	ruSAFE_RELEASE(m_rootRendererNode_1);
	ruSAFE_RELEASE(m_texCopyNode);
}

BOOL CRuRendererPlugin_DistortFX::RegisterPlugin(CRuRendererNode_RenderContext *renderContext)
{
	// Register event handlers
	renderContext->Event_Submitted().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_DistortFX::HandleEvent_Submitted, this, _1));
	renderContext->Event_Begin().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_DistortFX::HandleEvent_Begin, this, _1));
	renderContext->Event_Execute().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_DistortFX::HandleEvent_Execute, this, _1));

	// Initialize dummy camera
	m_dummyCamera = ruNEW CRuCamera();

	// Initialize the root renderer node
	m_rootRendererNode_0 = ruNEW CRuRendererNode();
	m_rootRendererNode_0->PersistAcrossFrames(FALSE);

	m_rootRendererNode_1 = ruNEW CRuRendererNode();
	m_rootRendererNode_1->PersistAcrossFrames(FALSE);

	// Create the texture copy node
	m_texCopyNode = ruNEW CRuRendererNode_TextureCopy();
	m_texCopyNode->PersistAcrossFrames(FALSE);

	return TRUE;
}

BOOL CRuRendererPlugin_DistortFX::HandleEvent_Submitted(RuEventArgs *eventArgs)
{
	RuRendererEvent_Submitted_Args *submittedArgs = reinterpret_cast<RuRendererEvent_Submitted_Args *>(eventArgs);

	CRuEntity *entity = submittedArgs->m_entity;
	CRuCamera *camera = submittedArgs->m_camera;

	return TRUE;
}

BOOL CRuRendererPlugin_DistortFX::HandleEvent_Begin(RuEventArgs *eventArgs)
{
	m_managedEntitiesFlushed = FALSE;
	m_managedEntities.Clear();
	m_managedMeshes.Clear();
	m_backbufferTexture = NULL;

	// Attach the root node to the renderer
	g_ruRenderer->AttachRendererNode(m_rootRendererNode_0);
	g_ruRenderer->AttachRendererNode(m_rootRendererNode_1);

	// Setup dependency
	m_rootRendererNode_0->AddDependency(g_ruDefaultRenderContext, TRUE);
	m_rootRendererNode_1->AddDependency(m_rootRendererNode_0, TRUE);

	return TRUE;
}

BOOL CRuRendererPlugin_DistortFX::HandleEvent_Execute(RuEventArgs *eventArgs)
{
	CRuRendererEvent_Execute_Args *executeArgs = static_cast<CRuRendererEvent_Execute_Args *>(eventArgs);

	CRuRendererNode_RenderContext *targetRC = static_cast<CRuRendererNode_RenderContext *>(executeArgs->m_rendererNode);
	IRuObject *rtObject = targetRC->GetRenderTarget();

	if(rtObject->GetType().IsSubClassOf(IRuEngineResource::Type()))
	{
		// Prepare distortion texture
		if(m_backbufferTexture)
		{
			// Copy source render target onto the distortion texture
			m_texCopyNode->SetSourceTexture(static_cast<IRuEngineResource *>(rtObject));
			m_texCopyNode->SetTargetTexture(m_backbufferTexture);
			m_rootRendererNode_0->AddChild(m_texCopyNode);
		}
	}

	return TRUE;
}

BOOL CRuRendererPlugin_DistortFX::HandleEvent_Execute_2(RuEventArgs *eventArgs)
{
	return TRUE;
}
void CRuRendererPlugin_DistortFX::GetSettingsFromEngine()
{
	RuPropertyType propertyValue;

	m_distortFXEnabled = g_ruEngineSettings->GetSetting("distortFX:enable", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;
	m_effectTextureSize = g_ruEngineSettings->GetSetting("distortFX:texture size", propertyValue) ? boost::get<INT32>(propertyValue) : m_effectTextureSize;
}

BOOL CRuRendererPlugin_DistortFX::EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting)
{
	GetSettingsFromEngine();
	return TRUE;
}

BOOL CRuRendererPlugin_DistortFX::MaterialClearCallback(const void *key, void *data)
{
	CRuMaterial *material = *reinterpret_cast<CRuMaterial **>(data);
	ruSAFE_RELEASE(material);

	return TRUE;
}

IRuMaterial *CRuRendererPlugin_DistortFX::MaterialOverrideFunctor(IRuMaterial *sourceMaterial)
{
	return NULL;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
