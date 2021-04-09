/*!
	@file RuRendererPlugin_AlphaZ.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../../Engine/Base/RuShader_Composite.h"
#include "../../Renderer/Plugin/RuRendererPlugin_AlphaZ.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuRendererPlugin_AlphaZ, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuRendererPlugin_AlphaZ, "CRuRendererPlugin_AlphaZ", "IRuObject")
ruCLASSGUID_DEFINE(CRuRendererPlugin_AlphaZ, 0xD89D3FDF, 0x130B488b, 0x8187EBFA, 0xD5F95DAE)

// ************************************************************************************************************************************************************

CRuRendererPlugin_AlphaZ::CRuRendererPlugin_AlphaZ()
{
	m_registeredEntities = ruNEW CRuKeyHash<CRuGUID>(1024);
	m_materialHash = ruNEW CRuHashMap<CRuGUID, MaterialDesc>(512);
}

CRuRendererPlugin_AlphaZ::~CRuRendererPlugin_AlphaZ()
{
	ruSAFE_DELETE(m_registeredEntities);

	m_materialHash->ForAllElements(boost::bind(&CRuRendererPlugin_AlphaZ::MaterialClearCallback, this, _1, _2));
	ruSAFE_DELETE(m_materialHash);
}

BOOL CRuRendererPlugin_AlphaZ::RegisterPlugin(CRuRendererNode_RenderContext *renderContext)
{
	// Register event handlers
	renderContext->Event_Begin().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_AlphaZ::HandleEvent_Begin, this, _1));
	renderContext->Event_Submitted().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_AlphaZ::HandleEvent_Submitted, this, _1));

	return TRUE;
}

BOOL CRuRendererPlugin_AlphaZ::RegisterEntity(CRuEntity *entity)
{
	const CRuGUID &guid = entity->GetGUID();

	if(m_registeredEntities->ContainsKey(guid, guid.HashCode()) == FALSE)
	{
		// Add entity guid to hash
		m_registeredEntities->Add(guid, guid.HashCode());

		// Register dispose handler on entity
		entity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_AlphaZ::HandleEvent_Dispose, this, _1));
	}

	return TRUE;
}

BOOL CRuRendererPlugin_AlphaZ::HandleEvent_Begin(RuEventArgs *eventArgs)
{
	// Run a GC on material hash
	m_materialHash->ForAllElements(boost::bind(&CRuRendererPlugin_AlphaZ::MaterialGCCallback, this, _1, _2));

	for(INT32 i = 0; i < m_gc_Materials.Count(); ++i)
	{
		MaterialDesc matDesc = { NULL, 0 };

		if(m_materialHash->Get(m_gc_Materials[i], m_gc_Materials[i].HashCode(), matDesc))
		{
			// Remove material
			m_materialHash->Remove(m_gc_Materials[i], m_gc_Materials[i].HashCode());

			// Release material
			ruSAFE_RELEASE(matDesc.m_material);
		}
	}

	m_gc_Materials.Clear();

	return TRUE;
}

BOOL CRuRendererPlugin_AlphaZ::HandleEvent_Submitted(RuEventArgs *eventArgs)
{
	RuRendererEvent_Submitted_Args *submittedArgs = static_cast<RuRendererEvent_Submitted_Args *>(eventArgs);
	CRuRendererNode_RenderContext *renderContext = submittedArgs->m_renderContext;
	CRuCamera *camera = submittedArgs->m_camera;
	CRuEntity *entity = submittedArgs->m_entity;

	if(entity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
	{
		const CRuGUID &guid = entity->GetGUID();

		if(m_registeredEntities->ContainsKey(guid, guid.HashCode()))
		{
			// Determine effective alpha
			REAL effectiveAlpha = 1.0f;

			CRuEntity *parentEntity = entity;
			while(parentEntity)
			{
				UINT64 paramVal;
				if(parentEntity->GetParameterBlock()->GetParameter(ruPARAMTYPE_MATERIAL_ALPHA, paramVal))
				{
					effectiveAlpha *= RuCastINT64ToFloat(paramVal);
				}

				// Traverse up to parent
				parentEntity = parentEntity->GetParent();
			}

			if(effectiveAlpha + ruEPSILON < 1.0f)
			{
				// Iterate through every mesh
				IRuEntity_Renderable *renderableEntity = static_cast<IRuEntity_Renderable *>(entity);

				for(INT32 i = 0, numMeshes = renderableEntity->GetNumMeshes(); i < numMeshes; ++i)
				{
					IRuMesh *mesh = NULL;
					IRuMaterial *material = NULL;

					if(renderableEntity->GetMesh(i, &mesh, &material))
					{
						const CRuGUID &matGUID = material->GetGUID();

						MaterialDesc matDesc = { NULL, 0 };
						CRuMaterial *alternateMaterial = NULL;

						if(m_materialHash->Get(matGUID, matGUID.HashCode(), matDesc) == FALSE)
						{
							// Create alternate material
							alternateMaterial = static_cast<CRuMaterial *>(material->Clone());

							// Disable color write
							alternateMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_COLORWRITEENABLE, FALSE);

							alternateMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(1.0f));

							// Ignore alpha sorting
							alternateMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_SYSTEM_ALPHAZPASS, TRUE);

							// Insert into hash
							matDesc.m_material = alternateMaterial;
							matDesc.m_gcAge = 0;
							m_materialHash->Add(matGUID, matGUID.HashCode(), matDesc);
						}
						else
						{
							// Reset GC age
							matDesc.m_gcAge = 0;
							m_materialHash->Replace(matGUID, matGUID.HashCode(), matDesc);

							// Use material
							alternateMaterial = matDesc.m_material;
						}

						if(alternateMaterial)
						{
							// Push suppressed events
							renderContext->EventSuppression_Push(ruENTITYEVENT_BROADCAST_SUBMITTED);

							// Submit
							renderContext->Submit(entity, mesh, alternateMaterial);

							// Pop suppressed events
							renderContext->EventSuppression_Pop();
						}
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL CRuRendererPlugin_AlphaZ::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	RuObjectEvent_Dispose_Args *disposeArgs = static_cast<RuObjectEvent_Dispose_Args *>(eventArgs);

	const CRuGUID &guid = disposeArgs->m_object->GetGUID();

	if(m_registeredEntities->ContainsKey(guid, guid.HashCode()))
	{
		m_registeredEntities->Remove(guid, guid.HashCode());
	}

	return TRUE;
}

BOOL CRuRendererPlugin_AlphaZ::MaterialClearCallback(const void *key, void *data)
{
	MaterialDesc matDesc = *reinterpret_cast<MaterialDesc *>(data);
	ruSAFE_RELEASE(matDesc.m_material);
	return TRUE;
}

BOOL CRuRendererPlugin_AlphaZ::MaterialGCCallback(const void *key, void *data)
{
	const CRuGUID &matKey = *reinterpret_cast<const CRuGUID *>(key);
	MaterialDesc &matDesc = *reinterpret_cast<MaterialDesc *>(data);

	++matDesc.m_gcAge;

	if(matDesc.m_gcAge > 100)
	{
		m_gc_Materials.Add(matKey);
	}
	else
	{
		// Update material GC age
		m_materialHash->Replace(matKey, matKey.HashCode(), matDesc);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuRendererPlugin_AlphaZ*		g_ruRendererPlugin_AlphaZ = NULL;

// ************************************************************************************************************************************************************

#pragma managed(pop)
