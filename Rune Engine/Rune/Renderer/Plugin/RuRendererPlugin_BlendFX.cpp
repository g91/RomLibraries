/*!
	@file RuRendererPlugin_BlendFX.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../../Renderer/Plugin/RuRendererPlugin_BlendFX.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuRendererPlugin_BlendFX, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuRendererPlugin_BlendFX, "CRuRendererPlugin_BlendFX", "IRuObject")
ruCLASSGUID_DEFINE(CRuRendererPlugin_BlendFX, 0xFCED5EDB, 0x93D34f24, 0x8392DF70, 0x9ADEC5E1)

// ************************************************************************************************************************************************************

CRuRendererPlugin_BlendFX::CRuRendererPlugin_BlendFX()
{
	m_materialHash = ruNEW CRuHashMap<CRuGUID, MaterialDesc>(512);
}

CRuRendererPlugin_BlendFX::~CRuRendererPlugin_BlendFX()
{
	m_materialHash->ForAllElements(boost::bind(&CRuRendererPlugin_BlendFX::MaterialClearCallback, this, _1, _2));
	ruSAFE_DELETE(m_materialHash);
}

BOOL CRuRendererPlugin_BlendFX::RegisterPlugin(CRuRendererNode_RenderContext *renderContext)
{
	// Register event handlers
	renderContext->Event_Begin().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_BlendFX::HandleEvent_Begin, this, _1));
	renderContext->Event_Submitted().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_BlendFX::HandleEvent_Submitted, this, _1));

	return TRUE;
}

BOOL CRuRendererPlugin_BlendFX::HandleEvent_Begin(RuEventArgs *eventArgs)
{
	// Run a GC on material hash
	m_materialHash->ForAllElements(boost::bind(&CRuRendererPlugin_BlendFX::MaterialGCCallback, this, _1, _2));

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

BOOL CRuRendererPlugin_BlendFX::HandleEvent_Submitted(RuEventArgs *eventArgs)
{
	RuRendererEvent_Submitted_Args *submittedArgs = static_cast<RuRendererEvent_Submitted_Args *>(eventArgs);
	CRuRendererNode_RenderContext *renderContext = submittedArgs->m_renderContext;
	CRuCamera *camera = submittedArgs->m_camera;
	CRuEntity *entity = submittedArgs->m_entity;

	if(entity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
	{
		IRuEntity_Renderable *renderableEntity = static_cast<IRuEntity_Renderable *>(entity);

		// Iterate through every mesh
		for(INT32 i = 0, numMeshes = renderableEntity->GetNumMeshes(); i < numMeshes; ++i)
		{
			IRuMesh *mesh = NULL;
			IRuMaterial *material = NULL;

			if(renderableEntity->GetMesh(i, &mesh, &material))
			{
				UINT64 param;
				if(material->GetParameterBlock()->GetParameter(ruPARAMTYPE_MATERIAL_SMOOTHBLEND, param))
				{
					BOOL smoothBlend = static_cast<BOOL>(param);

					if(smoothBlend != false)
					{
						const CRuGUID &matGUID = material->GetGUID();

						MaterialDesc matDesc = { NULL, 0 };
						CRuMaterial *alternateMaterial = NULL;

						if(m_materialHash->Get(matGUID, matGUID.HashCode(), matDesc) == FALSE)
						{
							// Create alternate material
							alternateMaterial = static_cast<CRuMaterial *>(material->Clone());

							alternateMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, ruALPHAMODE_PREMULTIPLIED_NORMAL);
							alternateMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_TRANSLUCENT, TRUE);
							alternateMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_LESSEQUAL);
							alternateMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ZWRITEMODE, ruZWRITEMODE_DISABLE);
							alternateMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 250);

							// Set original material's test value
							material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 250);

							// Forcibly ignore alpha sorting - we want these things to be drawn directly after the main body!
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

BOOL CRuRendererPlugin_BlendFX::MaterialClearCallback(const void *key, void *data)
{
	MaterialDesc matDesc = *reinterpret_cast<MaterialDesc *>(data);
	ruSAFE_RELEASE(matDesc.m_material);
	return TRUE;
}

BOOL CRuRendererPlugin_BlendFX::MaterialGCCallback(const void *key, void *data)
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

IRuMaterial *CRuRendererPlugin_BlendFX::MaterialOverrideFunctor(IRuMaterial *sourceMaterial)
{
	return NULL;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
