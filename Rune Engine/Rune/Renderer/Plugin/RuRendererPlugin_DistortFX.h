/*!
	@file RuRendererPlugin_DistortFX.h

	Copyright (c) 2004-2007 All rights reserved
*/

#ifndef _RURENDERERPLUGIN_DISTORTFX_H_
#define _RURENDERERPLUGIN_DISTORTFX_H_

#include "../../Renderer/RuRenderer.h"
#include "../../Renderer/RendererNode/RuRendererNode_TextureCopy.h"
#include "../../Renderer/RendererNode/RuRendererNode_UI.h"

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuRendererPlugin_DistortFX : public IRuObject, public IRuEntityEventListener
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct ManagedEntityDesc
	{
		CRuEntity*							m_entity;
		CRuCamera*							m_camera;
	};

	struct ManagedMeshDesc
	{
		CRuEntity*							m_entity;
		CRuCamera*							m_camera;
		IRuMesh*							m_mesh;
		IRuMaterial*						m_material;
	};

	// Settings
	BOOL									m_distortFXEnabled;
	INT32									m_effectTextureSize;

	// Run-time
	CRuHashMap<CRuGUID, CRuMaterial*>*		m_materialHash;

	BOOL									m_managedEntitiesFlushed;
	CRuArrayList<ManagedEntityDesc>			m_managedEntities;
	CRuArrayList<ManagedMeshDesc>			m_managedMeshes;
	IRuTexture*								m_backbufferTexture;

	CRuCamera*								m_dummyCamera;

	CRuRendererNode*						m_rootRendererNode_0;
	CRuRendererNode*						m_rootRendererNode_1;
	CRuRendererNode_TextureCopy*			m_texCopyNode;

public:
											CRuRendererPlugin_DistortFX();
	virtual									~CRuRendererPlugin_DistortFX();

	BOOL									RegisterPlugin(CRuRendererNode_RenderContext *renderContext);

	BOOL									HandleEvent_Submitted(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Begin(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Execute(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Execute_2(RuEventArgs *eventArgs);

protected:
	void									GetSettingsFromEngine();

	// Event handlers
	BOOL									EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting);

	BOOL									MaterialClearCallback(const void *key, void *data);
	IRuMaterial*							MaterialOverrideFunctor(IRuMaterial *sourceMaterial);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
