#include "../../Scene/Controller/RuController_Highlighter.h"

#include "../../Renderer/RuRenderer.h"
#include "../../Renderer/RendererNode/RuRendererNode_RenderContext.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_Highlighter, IRuEntity_Controller)
ruHEAP_DEFINE_SUBCLASS(CRuController_Highlighter, "CRuController_Highlighter", "IRuEntity_Controller")
ruCLASSGUID_DEFINE(CRuController_Highlighter, 0x7ED1695A, 0x4B0243fb, 0xA435A04A, 0x66D32EB3)

// ************************************************************************************************************************************************************

CRuController_Highlighter::CRuController_Highlighter()
:	m_materialHash(NULL),
	m_targetEntity(NULL),
	m_useCelHighlight(FALSE),
	m_pulsePeriod(1.0f),
	m_highlightColor_0(0xFFFFFFFF),
	m_highlightColor_1(0xFFFFFFFF)
{
	// Override parameters
	m_startTime = 0.0f;
	m_duration = 0.0f;
	m_loopTime = 0.0f;

	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Highlighter::HandleEvent_Dispose, this, _1), NULL);
}

CRuController_Highlighter::~CRuController_Highlighter()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

void CRuController_Highlighter::UseCelHighlight(BOOL useCelHighlight)
{
	m_useCelHighlight = useCelHighlight;
}

void CRuController_Highlighter::SetPulsePeriod(REAL pulsePeriod)
{
	m_pulsePeriod = pulsePeriod;
}

void CRuController_Highlighter::SetHighlightColor0(const CRuColor4 &highlightColor0)
{
	m_highlightColor_0 = highlightColor0;
}

void CRuController_Highlighter::SetHighlightColor1(const CRuColor4 &highlightColor1)
{
	m_highlightColor_1 = highlightColor1;
}

REAL CRuController_Highlighter::GetPulsePeriod() const
{
	return m_pulsePeriod;
}

const CRuColor4 &CRuController_Highlighter::GetHighlightColor0() const
{
	return m_highlightColor_0;
}

const CRuColor4 &CRuController_Highlighter::GetHighlightColor1() const
{
	return m_highlightColor_1;
}

BOOL CRuController_Highlighter::Update(REAL elapsedTime)
{
	// Override parameters
	m_startTime = 0.0f;
	m_duration = 0.0f;
	m_loopTime = 0.0f;

	// Chain update call to base controller class
	return IRuEntity_Controller::Update(elapsedTime);
}

IRuObject *CRuController_Highlighter::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_Highlighter::SetTarget(IRuObject *target)
{
	// If the target has not changed, ignore
	if(target == m_targetEntity)
	{
		return TRUE;
	}

	// If we had a previous target, release it
	ReleaseTarget();

	if(target && target->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		// Create material hash
		m_materialHash = ruNEW CRuHashMap<CRuGUID, MatDesc>(32);

		// Store target reference and setup event listener
		m_targetEntity = static_cast<CRuEntity *>(target);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Highlighter::HandleEvent_Dispose, this, _1), NULL);
		m_targetEntity->Event_Submitted().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Highlighter::HandleEvent_Submitted, this, _1), NULL);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuController_Highlighter::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	ReleaseTarget();
	return TRUE;
}

BOOL CRuController_Highlighter::HandleEvent_Submitted(RuEventArgs *eventArgs)
{
	RuRendererEvent_Submitted_Args *submittedArgs = static_cast<RuRendererEvent_Submitted_Args *>(eventArgs);
	CRuRendererNode_RenderContext *renderContext = submittedArgs->m_renderContext;
	CRuCamera *camera = submittedArgs->m_camera;
	CRuEntity *rootEntity = submittedArgs->m_entity;

	m_workStack.Push(rootEntity);

	while(m_workStack.IsEmpty() == FALSE)
	{
		CRuEntity *entity = m_workStack.Pop();

		CRuEntity *curChild = entity->GetFirstChild();
		while(curChild)
		{
			m_workStack.Push(curChild);
			curChild = curChild->GetNextSibling();
		}

		if(entity->GetType().IsSubClassOf(CRuModel::Type()))
		{
			IRuEntity_Renderable *renderableEntity = static_cast<IRuEntity_Renderable *>(entity);

			RUCOLOR currentColor;
			if(m_pulsePeriod > ruEPSILON)
			{
				REAL relativeT = fmod(m_age, m_pulsePeriod) / m_pulsePeriod;

				if(relativeT <= 0.5f)
				{
					relativeT = relativeT * 2.0f;
				}
				else
				{
					relativeT = 1.0f - (relativeT - 0.5f) * 2.0f;
				}

				currentColor = (m_highlightColor_1 * relativeT  + m_highlightColor_0 * (1.0f - relativeT)).GetARGB();
			}

			// Iterate through every mesh
			for(INT32 i = 0, numMeshes = renderableEntity->GetNumMeshes(); i < numMeshes; ++i)
			{
				IRuMesh *mesh = NULL;
				IRuMaterial *material = NULL;

				if(renderableEntity->GetMesh(i, &mesh, &material))
				{
					const CRuGUID &matGUID = material->GetGUID();

					MatDesc matDesc = { NULL, NULL };

					if(m_materialHash->Get(matGUID, matGUID.HashCode(), matDesc) == FALSE)
					{
						// Create transform matrix
						matDesc.m_transform = ruNEW CRuMatrix4x4();

						// Create alternate material
						matDesc.m_material = static_cast<CRuMaterial *>(material->Clone());

						if(m_useCelHighlight)
						{
							matDesc.m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, ruALPHAMODE_NORMAL);
							matDesc.m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_TRANSLUCENT, FALSE);
							matDesc.m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ZWRITEMODE, ruZWRITEMODE_ENABLE);

							// Eliminate texture
							matDesc.m_material->SetTexture(ruTEXCHANNEL_DIFFUSEMAP, NULL);

							// Reverse culling
							matDesc.m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_FX_REVERSECULL, TRUE);

							// Set extrude scale
							matDesc.m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_FX_EXTRUDESCALE, RuCastFloatToINT64(0.25f));
						}
						else
						{
							matDesc.m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, ruALPHAMODE_ADD);
							matDesc.m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_TRANSLUCENT, TRUE);
							matDesc.m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ZWRITEMODE, ruZWRITEMODE_DISABLE);
						}

						// Insert into hash
						m_materialHash->Add(matGUID, matGUID.HashCode(), matDesc);
					}

					// Update color
					matDesc.m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATIONCOLOR, currentColor);
					matDesc.m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(1.0f));

					// Push suppressed events
					renderContext->EventSuppression_Push(ruENTITYEVENT_BROADCAST_SUBMITTED);

					// Submit
					renderContext->Submit(entity, mesh, matDesc.m_material);

					// Pop suppressed events
					renderContext->EventSuppression_Pop();
				}
			}
		}
	}

	return TRUE;
}

void CRuController_Highlighter::ReleaseTarget()
{
	if(m_targetEntity)
	{
		// Unregister target entity
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity->Event_Submitted().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	// Clear and delete material hash
	if(m_materialHash)
	{
		m_materialHash->ForAllElements(boost::bind(&CRuController_Highlighter::ClearMaterialHashCallback, this, _1, _2));
		ruSAFE_DELETE(m_materialHash);
	}
}

BOOL CRuController_Highlighter::ClearMaterialHashCallback(const void *key, void *data)
{
	MatDesc &matDesc = *reinterpret_cast<MatDesc *>(data);

	ruSAFE_DELETE(matDesc.m_transform);
	ruSAFE_RELEASE(matDesc.m_material);

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
