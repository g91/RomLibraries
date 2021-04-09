#include "../../Scene/Controller/RuController_DistanceFader.h"

#include "../../Helper/RuHelper_Entity.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_DistanceFader, IRuEntity_Controller)
ruHEAP_DEFINE_SUBCLASS(CRuController_DistanceFader, "CRuController_DistanceFader", "IRuEntity_Controller")
ruCLASSGUID_DEFINE(CRuController_DistanceFader, 0x38EB64FE, 0x89764230, 0xACD9EC7F, 0x8CB52D75)

// ************************************************************************************************************************************************************

CRuController_DistanceFader::CRuController_DistanceFader()
:	m_targetEntity(NULL),
	m_isFadeOut(FALSE)
{
	m_fadeShader = g_ruShaderManager->GetShader("ruSHADER_STATIC_FADER");

	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_DistanceFader::HandleEvent_Dispose, this, _1), NULL);
}

CRuController_DistanceFader::~CRuController_DistanceFader()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

BOOL CRuController_DistanceFader::Update(REAL elapsedTime)
{
	if(m_enabled)
	{
		// Calculate weights
		REAL relativeT = 1.0f;
		if(m_duration > ruEPSILON)
		{
			relativeT = m_age / m_duration;
			relativeT = max(0.0f, min(relativeT, 1.0f));
		}

		REAL alpha = 1.0f;
		DWORD diffuse = 0xFFFFFFFF;

		if(m_isFadeOut)
		{
			diffuse = RUCOLOR_ARGB((BYTE) ((1.0f - relativeT) * 255.0f), 0xFF, 0xFF, 0xFF);
			alpha = 1.0f - relativeT;
		}
		else
		{
			diffuse = RUCOLOR_ARGB((BYTE) (relativeT * 255.0f), 0xFF, 0xFF, 0xFF);
			alpha = relativeT;
		}

		// Iterate through all target materials
		for(INT32 i = 0; i < m_targetMaterials.Count(); ++i)
		{
			CRuParameterBlock *parameterBlock = m_targetMaterials[i].m_targetMaterial->GetParameterBlock();

			// Update the internal shader parameter block
			parameterBlock->SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(alpha));
			parameterBlock->SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, (UINT64) ruALPHAMODE_NORMAL);

			// Update shader on the target material
			m_targetMaterials[i].m_targetMaterial->SetShader(m_fadeShader);
		}
	}

	// Chain update call to base controller class
	return IRuEntity_Controller::Update(elapsedTime);
}

IRuObject *CRuController_DistanceFader::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_DistanceFader::SetTarget(IRuObject *target)
{
	if(target->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		// If we had a previous target, unregister it
		if(m_targetEntity)
		{
			m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		// Store target reference and setup event listener
		m_targetEntity = static_cast<CRuEntity *>(target);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_DistanceFader::HandleEvent_Dispose, this, _1), NULL);

		// Iterate through target entity's renderables
		CRuStack<CRuEntity *> entityStack;
		entityStack.Push(static_cast<CRuEntity *>(target));

		while(!entityStack.IsEmpty())
		{
			CRuEntity *curEntity = entityStack.Pop();

			if(curEntity->GetType().IsSubClassOf(IRuEntity_Container::Type()))
			{
				IRuEntity_Container *container = static_cast<IRuEntity_Container *>(curEntity);

				// Push all entities held by the container onto the entity stack
				for(INT32 i = 0, numEntities = container->GetNumEntities(); i < numEntities; ++i)
				{
					entityStack.Push(container->GetEntity(i));
				}
			}
			else if(curEntity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
			{
				CRuModel *model = static_cast<CRuModel *>(curEntity);

				for(INT32 i = 0, numMeshes = model->GetNumMeshes(); i < numMeshes; ++i)
				{
					IRuMesh *mesh = NULL;
					IRuMaterial *material = NULL;

					if(model->GetMesh(i, &mesh, &material))
					{
						TargetMaterialDesc tMatDesc;
						tMatDesc.m_targetMaterial = material;
						tMatDesc.m_oldShader = material->GetShader();
						m_targetMaterials.Add(tMatDesc);
					}
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

void CRuController_DistanceFader::SetFadeType(BOOL isFadeOut)
{
	m_isFadeOut = isFadeOut;
}

BOOL CRuController_DistanceFader::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_targetEntity)
	{
		// Unregister target entity
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;

		// Iterate through all target materials
		for(INT32 i = 0; i < m_targetMaterials.Count(); ++i)
		{
			// Clear the internal shader parameter block
			CRuParameterBlock *parameterBlock = m_targetMaterials[i].m_targetMaterial->GetParameterBlock();

			if(m_isFadeOut)
			{
				parameterBlock->SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(0.0f));
				parameterBlock->SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, (UINT64) ruALPHAMODE_NORMAL);
			}
			else
			{
				parameterBlock->SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(1.0f));
				parameterBlock->SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, (UINT64) ruALPHAMODE_NORMAL);
			}
			parameterBlock->ClearParameter(ruPARAMTYPE_MATERIAL_ALPHA);
			parameterBlock->ClearParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE);

			// Replace old shader on the target material
			m_targetMaterials[i].m_targetMaterial->SetShader(m_targetMaterials[i].m_oldShader);
		}

		m_targetMaterials.Clear();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
