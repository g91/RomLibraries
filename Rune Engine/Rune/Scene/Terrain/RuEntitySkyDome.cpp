#include "../../Scene/Terrain/RuEntitySkyDome.h"

#include "../../Engine/Geometry/RuPrimitiveToolkit.h"

#include "../../Helper/RuHelper_Entity.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuSkyDome, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(CRuSkyDome, "CRuSkyDome", "CRuEntity")
ruCLASSGUID_DEFINE(CRuSkyDome, 0xE4D6FD45, 0x705B4bcf, 0x9F602069, 0x672CBFDD)

// ************************************************************************************************************************************************************

CRuSkyDome::CRuSkyDome()
:	m_domeGradientTDelta(0.0f),
	m_domeGradientTexture0(NULL),
	m_domeGradientTexture1(NULL),
	m_domeModel(NULL),
	m_domeMesh(NULL),
	m_domeMaterial(NULL)
{
	// Register event handlers
	Event_Camera().RegisterHandler(this->GetGUID(), boost::bind(&CRuSkyDome::HandleEvent_Camera, this, _1));
}

CRuSkyDome::~CRuSkyDome()
{
	ruSAFE_RELEASE(m_domeGradientTexture0);
	ruSAFE_RELEASE(m_domeGradientTexture1);

	ruENTITY_SAFE_RELEASE_CHILD(m_domeModel);
	ruSAFE_RELEASE(m_domeMesh);
	ruSAFE_RELEASE(m_domeMaterial);

	ruSAFE_RELEASE(m_cloudPlane);
}

BOOL CRuSkyDome::SerializeFrom(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuSkyDome::SerializeTo(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuSkyDome::ResetStates()
{
	return TRUE;
}

BOOL CRuSkyDome::Update(float elapsedTime)
{
	// Forcibly set reflect class to 3
	RuEntity_SetOption(this, ruENTITYOPTION_REFLECTCLASS, 3);

	// Chain update call to parent class
	if(CRuEntity::Update(elapsedTime) == FALSE)
	{
		return FALSE;
	}

	if(m_domeGradientTDelta > 0.0f)
	{
		m_domeGradientT -= (m_domeGradientTDelta * elapsedTime);

		if(m_domeGradientT <= 0.0f)
		{
			ruSAFE_RELEASE(m_domeGradientTexture0);

			// Reset gradient t to 1.0
			m_domeGradientT = 1.0f;

			m_domeGradientTDelta = 0.0f;

			m_domeGradientTexture0 = m_domeGradientTexture1;
			m_domeGradientTexture1 = NULL;
		}

		// Set gradient t
		m_domeMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_SKYDOME_GRADIENT_T, RuCastFloatToINT64(m_domeGradientT));

		// Set gradient texture
		m_domeMaterial->SetTexture(ruTEXCHANNEL_SKYDOME_GRADIENT_0, m_domeGradientTexture0);
		m_domeMaterial->SetTexture(ruTEXCHANNEL_SKYDOME_GRADIENT_1, m_domeGradientTexture1);
	}

	// Update dome model
	m_domeModel->Update(elapsedTime);

	// Update cloud plane parameters
	if(m_cloudPlane)
	{
		// Attach or detach cloud plane depending on sky detail setting
		RuPropertyType propertyValue;
		INT32 skyDetailLevel = g_ruEngineSettings->GetSetting("gfx:sky detail", propertyValue) ? boost::get<INT32>(propertyValue) : 1;

		if(skyDetailLevel < 1)
		{
			m_cloudPlane->DetachFromParent();
		}
		else
		{
			this->AddChild(m_cloudPlane);
		}

		// Update
		m_cloudCover.Update(elapsedTime);
		m_cloudSharpness.Update(elapsedTime);

		m_cloudPlane->SetCloudCover(m_cloudCover);
		m_cloudPlane->SetCloudSharpness(m_cloudSharpness);

		CRuColor4 color0 = m_cloudColor0;
		CRuColor4 color1 = m_cloudColor1;

		m_cloudPlane->SetCloudColor0(color0.GetARGB());
		m_cloudPlane->SetCloudColor1(color1.GetARGB());

		REAL octaveWeight[8];
		REAL octaveScrollSpeed[8];

		for(INT32 i = 0; i < 8; ++i)
		{
			octaveWeight[i] = m_octaveWeight[i];
			octaveScrollSpeed[i] = m_octaveScrollSpeed[i];
		}

		m_cloudPlane->SetOctaveWeight(octaveWeight);
		m_cloudPlane->SetOctaveScrollSpeed(octaveScrollSpeed);
	}


	return TRUE;
}

BOOL CRuSkyDome::HandleEvent_Camera(RuEventArgs *eventArgs)
{
	RuEntityEvent_Camera_Args *cameraArgs = static_cast<RuEntityEvent_Camera_Args *>(eventArgs); 
	CRuCamera *primaryCamera = cameraArgs->m_camera;

	// Update local transform
	this->ResetLocalTransform();
	this->SetTranslation(CRuVector3(primaryCamera->GetCameraPosition().x, primaryCamera->GetCameraPosition().y + m_domeCenterY, primaryCamera->GetCameraPosition().z));

	return TRUE;
}

/*
void CRuSkyDome::UpdateEntityTransform()
{
	// Chain transform update call to base class
	CRuEntity::UpdateEntityTransform();

	// Invoke update call on model
	if(m_domeModel)
	{
		m_domeModel->UpdateEntityTransform();
	}

	if(m_horizonCloudModel)
	{
		m_horizonCloudModel->UpdateEntityTransform();
	}

	if(m_overheadCloudModel)
	{
		m_overheadCloudModel->UpdateEntityTransform();
	}
}

void CRuSkyDome::UpdateEntityBounds()
{
	// Chain bounds update call to base class
	CRuEntity::UpdateEntityBounds();

	if(m_domeModel)
	{
		m_domeModel->UpdateEntityBounds();
		m_entityBounds = m_domeModel->GetBounds();
	}

	if(m_horizonCloudModel)
	{
		m_horizonCloudModel->UpdateEntityBounds();
		m_entityBounds.Merge(m_horizonCloudModel->GetBounds());
	}

	if(m_overheadCloudModel)
	{
		m_overheadCloudModel->UpdateEntityBounds();
		m_entityBounds.Merge(m_overheadCloudModel->GetBounds());
	}
}
*/
BOOL CRuSkyDome::Initialize(INT32 horzSegments, INT32 vertSegments, float vertSweep, float radius, float vertScale, float horzCompression, INT32 cloudPlaneSegments)
{
	m_horizontalSegments = horzSegments;
	m_verticalSegments = vertSegments;

	// Construct the dome mesh
	m_domeMesh = ruNEW CRuMesh();
	m_domeMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, CRuPrimitiveToolkit::HalfCapsule_VertexCount(horzSegments, vertSegments), CRuPrimitiveToolkit::HalfCapsule_PrimitiveCount(horzSegments, vertSegments), ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);

	CRuVector3 *positions = static_cast<CRuMorphTarget *>(m_domeMesh->GetMorphTarget(0))->GetPosition();
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_domeMesh->GetMorphTarget(0))->GetDiffuse();
	UINT16 *indices = m_domeMesh->GetIndices();

	INT32 vertexOffset = 0;
	INT32 indexOffset = 0;
	CRuPrimitiveToolkit::HalfCapsule_Build(horzSegments, vertSegments, vertSweep, ruPI * 2.0f, radius, vertScale, 1.0f, 2000.0f, vertexOffset, indexOffset, positions, indices, diffuse);

	// Calculate mesh bounds
	m_domeMesh->CalculateBounds();

	// Construct dome material
	m_domeMaterial = ruNEW CRuMaterial();
	m_domeMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_SKYDOME"));

	// Construct dome model
	m_domeModel = ruNEW CRuModel();
	m_domeModel->AddSubModel(m_domeMesh, m_domeMaterial);
	this->AddChild(m_domeModel);

	// Construct cloud plane
	m_cloudPlane = ruNEW CRuWorld_CloudPlane();
	this->AddChild(m_cloudPlane);

	// Mark this entity and all children to be no-clip
	RuEntity_SetOption(this, ruENTITYOPTION_NOCLIP, TRUE);

	return TRUE;
}

void CRuSkyDome::SetCamera(CRuCamera *viewCamera)
{
//	m_domeCenterY = -500.0f;
	m_domeCenterY = 0.0f;

	// Update local transform
	this->ResetLocalTransform();
	this->SetTranslation(CRuVector3(viewCamera->GetCameraPosition().x, viewCamera->GetCameraPosition().y + m_domeCenterY, viewCamera->GetCameraPosition().z));

/*
	// Invert translation
	if(m_cloudPlane)
	{
		m_cloudPlane->SetTranslation(CRuVector3(0.0f, 300, 0.0f));
	}
*/
}

CRuWorld_CloudPlane *CRuSkyDome::GetCloudPlane()
{
	return m_cloudPlane;
}

BOOL CRuSkyDome::IsInTransition() const
{
	if(m_domeGradientTDelta > 0.0f)
	{
		return TRUE;
	}

	return FALSE;
}

void CRuSkyDome::SetSkyDomeGradientTexture(IRuTexture *gradientTexture)
{
	ruSAFE_RELEASE(m_domeGradientTexture0);

	m_domeGradientT = 1.0f;
	m_domeGradientTDelta = 0.0f;

	m_domeGradientTexture0 = gradientTexture;
	m_domeGradientTexture0->AddRef();

	m_domeMaterial->SetTexture(ruTEXCHANNEL_SKYDOME_GRADIENT_0, m_domeGradientTexture0);
	m_domeMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_SKYDOME_GRADIENT_T, RuCastFloatToINT64(m_domeGradientT));
}

void CRuSkyDome::TransitionSkyDomeGradientTexture(IRuTexture *gradientTexture, float transitionTime)
{
	ruSAFE_RELEASE(m_domeGradientTexture1);

	m_domeGradientT = 1.0f;
	m_domeGradientTDelta = 1.0f / transitionTime;

	m_domeGradientTexture1 = gradientTexture;
	m_domeGradientTexture1->AddRef();

	m_domeMaterial->SetTexture(ruTEXCHANNEL_SKYDOME_GRADIENT_1, m_domeGradientTexture0);
	m_domeMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_SKYDOME_GRADIENT_T, RuCastFloatToINT64(m_domeGradientT));
}

void CRuSkyDome::SetCloudCover(REAL cloudCover)
{
	m_cloudCover = cloudCover;
}

void CRuSkyDome::TransitionCloudCover(REAL cloudCover, float transitionTime)
{
	m_cloudCover.Transition(cloudCover, 1.0f / transitionTime);
}

void CRuSkyDome::SetCloudSharpness(REAL cloudSharpness)
{
	m_cloudSharpness = cloudSharpness;
}

void CRuSkyDome::SetCloudColor0(CRuColor4 cloudColor)
{
	m_cloudColor0 = cloudColor;
}

void CRuSkyDome::TransitionCloudColor0(CRuColor4 cloudColor, float transitionTime)
{
	m_cloudColor0.Transition(cloudColor, transitionTime);
}

void CRuSkyDome::SetCloudColor1(CRuColor4 cloudColor)
{
	m_cloudColor1 = cloudColor;
}

void CRuSkyDome::TransitionCloudColor1(CRuColor4 cloudColor, float transitionTime)
{
	m_cloudColor1.Transition(cloudColor, transitionTime);
}

void CRuSkyDome::TransitionCloudSharpness(REAL cloudSharpness, float transitionTime)
{
	m_cloudSharpness.Transition(cloudSharpness, transitionTime);
}

void CRuSkyDome::SetOctaveWeight(REAL octaveWeight[8])
{
	for(INT32 i = 0; i < 8; ++i)
	{
		m_octaveWeight[i] = octaveWeight[i];
	}
}

void CRuSkyDome::TransitionOctaveWeight(REAL octaveWeight[8], REAL transitionTime)
{
	for(INT32 i = 0; i < 8; ++i)
	{
		m_octaveWeight[i].Transition(octaveWeight[i], transitionTime);
	}
}

void CRuSkyDome::SetOctaveScrollSpeed(REAL octaveScrollSpeed[8])
{
	for(INT32 i = 0; i < 8; ++i)
	{
		m_octaveScrollSpeed[i] = octaveScrollSpeed[i];
	}
}

void CRuSkyDome::TransitionOctaveScrollSpeed(REAL octaveScrollSpeed[8], REAL transitionTime)
{
	for(INT32 i = 0; i < 8; ++i)
	{
		m_octaveScrollSpeed[i].Transition(octaveScrollSpeed[i], transitionTime);
	}
}

// ************************************************************************************************************************************************************

#pragma managed
