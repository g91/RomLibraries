/*!
	@file RuFX2D_Model.cpp

	Copyright (c) 2004-2006 All rights reserved
*/

#include "../../Scene/FX2D/RuFX2D_Model.h"

#include "../../Helper/RuHelper_Entity.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFX2D_Model, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(CRuFX2D_Model, "CRuFX2D_Model", "CRuEntity")
ruCLASSGUID_DEFINE(CRuFX2D_Model, 0xEB1733B5, 0xFBD94ec5, 0x81370B1A, 0x454E5DF1)

// ************************************************************************************************************************************************************

CRuFX2D_Model::CRuFX2D_Model()
:	m_maskingTexture(NULL),
	m_model(NULL),
	m_modelOffset(0.0f, 0.0f, 0.0f),
	m_modelScale(1.0f),
	m_targetHierarchy(NULL),
	m_targetBonesDirty(TRUE),
	m_validTargetBoneExists(FALSE),
	m_alignmentFrame_0(NULL),
	m_alignmentFrame_1(NULL),
	m_alignmentBias(0.5f, 0.5f, 0.5f),
	m_alignmentScale(1.0f),
	m_relativeCameraPosition(0.0f, 0.0f, 1.0f),
	m_modelBoundsDirty(TRUE),
	m_enableModelAutoPlacement(FALSE),
	m_viewportRect(0.0f, 0.0f, 0.0f, 0.0f),
	m_modelCamera(NULL)
{
	m_modelCamera = ruNEW CRuCamera();

	if(g_ruEngine)
	{
		m_viewport = g_ruEngine->CreateViewport(0, 0, 512, 512);
		m_viewport->SetClearFlags(RUCLEAR_ZBUFFER);
		m_viewport->SetClearColor(0x00000000);
		m_viewport->SetClearStencil(0);
		m_viewport->SetClearZ(1.0f);
	}

	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuFX2D_Model::HandleEvent_Dispose, this, _1), NULL);
}

CRuFX2D_Model::~CRuFX2D_Model()
{
	// Release masking texture
	ruSAFE_RELEASE(m_maskingTexture);

	// Release alignment frame name strings
	delete[] m_alignmentFrame_0;
	m_alignmentFrame_0 = NULL;

	delete[] m_alignmentFrame_1;
	m_alignmentFrame_1 = NULL;

	// Release model camera
	ruSAFE_RELEASE(m_modelCamera);

	// Release viewport object
	ruSAFE_RELEASE(m_viewport);
}

void CRuFX2D_Model::SetMaskingTexture(IRuTexture *maskingTexture)
{
	if(maskingTexture)
	{
		maskingTexture->AddRef();
		ruSAFE_RELEASE(m_maskingTexture);
		m_maskingTexture = maskingTexture;
	}
	else
	{
		ruSAFE_RELEASE(m_maskingTexture);
	}
}

BOOL CRuFX2D_Model::SetModel(CRuEntity *model)
{
	if(model)
	{
		// Increment reference count on new model
		model->AddRef();

		// If there is an existing model, release it
		if(m_model)
		{
			m_model->Event_Dispose().UnregisterHandler(this->GetGUID());
			ruSAFE_RELEASE(m_model);
		}

		// Store reference to model and register self  as listener
		m_model = model;
		m_model->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuFX2D_Model::HandleEvent_Dispose, this, _1), NULL);

		// Find first hierarchy contained by the target model
		m_targetHierarchy = RuEntity_FindFirstHierarchy_GR2(m_model);
	}
	else
	{
		// Release existing target entity
		if(m_model)
		{
			// Unregister as event listener
			m_model->Event_Dispose().UnregisterHandler(this->GetGUID());

			// Release references
			ruSAFE_RELEASE(m_model);
			m_targetHierarchy = NULL;
		}
	}

	// Mark target bones dirty
	m_targetBonesDirty = TRUE;

	return TRUE;
}

BOOL CRuFX2D_Model::SetModelOffset(const CRuVector3 &modelOffset)
{
	m_modelOffset = modelOffset;

	return TRUE;
}

BOOL CRuFX2D_Model::SetModelScale(REAL scale)
{
	m_modelScale = scale;

	return TRUE;
}

BOOL CRuFX2D_Model::AddTargetBone(const char *targetBoneName)
{
	if(targetBoneName)
	{
		// Create new target bone descriptor
		TargetBoneDesc *targetBoneDesc = ruNEW TargetBoneDesc();
		targetBoneDesc->m_boneName = targetBoneName;

		// Add descriptor to list of target bones
		m_targetBones.Add(targetBoneDesc);

		// Mark target bones dirty
		m_targetBonesDirty = TRUE;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuFX2D_Model::ClearTargetBones()
{
	for(INT32 i = 0; i < m_targetBones.Count(); ++i)
	{
		delete m_targetBones[i];
	}

	m_targetBones.Clear();

	return TRUE;
}

BOOL CRuFX2D_Model::SetAlignmentFrames(const char *frame0, const char *frame1, REAL bias, REAL scale)
{
	return SetAlignmentFrames(frame0, frame1, CRuVector3(bias, bias, bias), scale);
}

BOOL CRuFX2D_Model::SetAlignmentFrames(const char *frame0, const char *frame1, const CRuVector3 &bias, REAL scale)
{
	if(frame0 && frame1)
	{
		delete[] m_alignmentFrame_0;
		m_alignmentFrame_0 = ruNEW char [strlen(frame0) + 1];
		strcpy(m_alignmentFrame_0, frame0);

		delete[] m_alignmentFrame_1;
		m_alignmentFrame_1 = ruNEW char [strlen(frame1) + 1];
		strcpy(m_alignmentFrame_1, frame1);

		m_alignmentBias = bias;

		m_alignmentScale = scale;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuFX2D_Model::ClearAlignmentFrames()
{
	delete[] m_alignmentFrame_0;
	m_alignmentFrame_0 = NULL;

	delete[] m_alignmentFrame_1;
	m_alignmentFrame_1 = NULL;

	m_alignmentBias.Set(0.5f, 0.5f, 0.5f);

	m_alignmentScale = 1.0f;

	return TRUE;
}

void CRuFX2D_Model::SetRelativeCameraPosition(const CRuVector3 &relativeCamPos)
{
	m_relativeCameraPosition = relativeCamPos;
}

void CRuFX2D_Model::SetViewRect(const CRuRectangle &viewRect)
{
	m_enableModelAutoPlacement = TRUE;
	m_viewportRect = viewRect;
}

void CRuFX2D_Model::Render(CRuRendererNode_UI *uiRenderer)
{
	// Update target bones
	UpdateTargetBones();

	// Update model bounds
	UpdateModelBounds();

	// Set model camera position & target
	CRuVector3 camDir(m_relativeCameraPosition);

	if(camDir.Magnitude() < ruEPSILON)
	{
		camDir.Set(0.0f, 0.0f, 1.0f);
	}
	else
	{
		camDir.Normalize();
	}

	m_modelCamera->SetCameraPosition(CRuVector3(0.0f, 0.0f, 0.0f) - camDir * 2.0f);
	m_modelCamera->SetTargetPosition(CRuVector3(0.0f, 0.0f, 0.0f));
	m_modelCamera->SetCameraUpVector(CRuVector3(0.0f, 1.0f, 0.0f));

	// Convert model position into world coordinates
	CRuVector3 modelPos_View(0.0f, 0.0f, 2.0f);
	m_modelCamera->GetInverseCameraTransform().TransformPoint(modelPos_View);

	// Find the closest distance to the frustum planes
	REAL minDist = 1.0f;
	const CRuPlane4 *modelFrustumPlanes = m_modelCamera->GetFrustumPlanes();
	for(INT32 i = 0; i < 6; ++i)
	{
		minDist = min(minDist, fabs(modelFrustumPlanes[i].GetDistance(modelPos_View)));
	}

	// Calculate model scale to fill the viewport
	REAL scaleToFit = minDist / m_modelBounds.Radius();

	// Position model such that its center point y is at the camera target position
	CRuVector3 modelPosition = m_modelBounds.Center() * -1.0f * scaleToFit;

	// Offset the model
	modelPosition = modelPosition + (m_modelOffset * scaleToFit);

	// Modify scale-to-fit by the specified model scale
	scaleToFit = scaleToFit * m_modelScale;

	if(m_model)
	{
		UINT8 stencilID = uiRenderer->GetNextStencilID();

		// Reset all model transforms and set model scale
		m_model->ResetAllTransforms();
		m_model->Scale(scaleToFit);

		// Apply world position to model
		m_model->Translate(modelPosition);

		// Convert viewport coordinates into relative screen coordinates
		CRuRectangle convertedViewportRect = m_viewportRect;
		uiRenderer->ConvertCoordinates(convertedViewportRect);
		m_viewport->SetViewportParameters(static_cast<INT32>(convertedViewportRect.m_left), static_cast<INT32>(convertedViewportRect.m_top), static_cast<INT32>(convertedViewportRect.m_right - convertedViewportRect.m_left), static_cast<INT32>(convertedViewportRect.m_bottom - convertedViewportRect.m_top), 0.0f, 1.0f);

		REAL convertedViewportWidth = convertedViewportRect.m_right - convertedViewportRect.m_left;
		REAL convertedViewportHeight = convertedViewportRect.m_bottom - convertedViewportRect.m_top;

		// Apply masking texture if it exists
		if(m_maskingTexture)
		{
			CRuRectangle srcRect(0, 0, static_cast<REAL>(m_maskingTexture->GetLevelWidth(0)), static_cast<REAL>(m_maskingTexture->GetLevelHeight(0)));

			// Clear viewport
			uiRenderer->ClearViewport(m_viewport);

			// Blit mask texture using stencil op
			uiRenderer->SetEffectType(ru2DFXTYPE_BLIT_MASK);
			uiRenderer->SetEffectParameter(ru2DFXPARAM_STENCILID, stencilID);
			uiRenderer->SetTexture(m_maskingTexture);
			uiRenderer->Submit(srcRect, m_viewportRect);

			// Setup target model for stenciled render
			m_model->GetParameterBlock()->SetParameter(ruPARAMTYPE_SYSTEM_STENCILENABLE, TRUE);
			m_model->GetParameterBlock()->SetParameter(ruPARAMTYPE_SYSTEM_STENCILID, stencilID);
		}

		// Setup camera projection
		m_modelCamera->SetPerspectiveProjectionFOV(1.0f, 500.0f, ruPI * 0.10f, convertedViewportWidth / convertedViewportHeight);

		// Push viewport into renderer
		uiRenderer->Viewport_Push(m_viewport);

		// Push camera
		uiRenderer->Camera_Push(m_modelCamera);

		// Submit model for render
		uiRenderer->Submit(m_model);

		// Pop camera
		uiRenderer->Camera_Pop();

		// Pop viewport from renderer
		uiRenderer->Viewport_Pop();

		if(m_maskingTexture)
		{
			m_model->GetParameterBlock()->ClearParameter(ruPARAMTYPE_SYSTEM_STENCILENABLE);
			m_model->GetParameterBlock()->ClearParameter(ruPARAMTYPE_SYSTEM_STENCILID);
		}
	}
}

BOOL CRuFX2D_Model::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	// If there is an existing model, release it
	if(m_model)
	{
		m_model->Event_Dispose().UnregisterHandler(this->GetGUID());
		ruSAFE_RELEASE(m_model);
	}

	return TRUE;
}

void CRuFX2D_Model::UpdateTargetBones()
{
	if(m_targetBonesDirty)
	{
		// Reset dirty flag
		m_targetBonesDirty = FALSE;

		// Clear target bone existence flag
		m_validTargetBoneExists = FALSE;

		if(m_targetHierarchy)
		{
			// Locate bone index for each bone
			for(INT32 i = 0; i < m_targetBones.Count(); ++i)
			{
				m_targetBones[i]->m_boneIndex = m_targetHierarchy->GetSubNodeIndexByName(m_targetBones[i]->m_boneName);

				if(m_targetBones[i]->m_boneIndex >= 0)
				{
					m_validTargetBoneExists = TRUE;
				}
			}
		}

		// Reset target bounds
		m_modelBounds.Set(CRuVector3(0.0f, 0.0f, 0.0f), 0.0f);
	}
}

void CRuFX2D_Model::UpdateModelBounds()
{
	// If there is a target hierarchy, and there are valid target bones, get bounds for the target bones
	if(m_targetHierarchy && m_validTargetBoneExists)
	{
		// Update aggregated target bones bounds
		BOOL firstBone = TRUE;
		CRuVector3 firstTargetPos;

		for(INT32 i = 0; i < m_targetBones.Count(); ++i)
		{
			if(m_targetBones[i]->m_boneIndex >= 0)
			{
				if(firstBone)
				{
					m_modelBounds = m_targetHierarchy->GetSubNodeBounds(m_targetBones[i]->m_boneIndex, ruFRAMETYPE_LOCAL);
					firstTargetPos = m_modelBounds.Center();
					firstBone = FALSE;
				}
				else
				{
					m_modelBounds.Merge(m_targetHierarchy->GetSubNodeBounds(m_targetBones[i]->m_boneIndex, ruFRAMETYPE_LOCAL));
				}
			}
		}
	}

	if(m_model && m_modelBoundsDirty)
	{
		// Store model local bounds if there is a target model
		m_modelBounds = m_model->GetLocalBounds();

		if(m_alignmentFrame_0 && m_alignmentFrame_1)
		{
			CRuEntity *frame0 = RuEntity_FindLinkFrame(m_model, m_alignmentFrame_0, TRUE);
			CRuEntity *frame1 = RuEntity_FindLinkFrame(m_model, m_alignmentFrame_1, TRUE);

			if(frame0 && frame1)
			{
				REAL hierarchyScale0 = frame0->GetParent() ? frame0->GetParent()->GetScale(ruFRAMETYPE_LOCAL) : 1.0f;
				REAL hierarchyScale1 = frame1->GetParent() ? frame1->GetParent()->GetScale(ruFRAMETYPE_LOCAL) : 1.0f;

				//生產類的家具有"p_name_title" 會造成不斷閃爍
				if( strcmp( m_alignmentFrame_0, "p_name_title" ) == 0 && strcmp( m_alignmentFrame_1, "p_down" ) == 0 )
				{
					hierarchyScale0 = 1.0f;
					hierarchyScale1 = 1.0f;
				}
								
				//REAL hierarchyScale0 = 1.0f;
				//REAL hierarchyScale1 = 1.0f;

				CRuVector3 v0 = frame1->GetTranslation(ruFRAMETYPE_LOCAL) * hierarchyScale0 - frame0->GetTranslation(ruFRAMETYPE_LOCAL) * hierarchyScale1;

				CRuVector3 center = frame0->GetTranslation(ruFRAMETYPE_LOCAL) * hierarchyScale0 + (v0 * m_alignmentBias);
				REAL radius = v0.Magnitude() * m_alignmentScale;

				m_modelBounds.Set(center, radius);
			}
		}

//		m_modelBoundsDirty = FALSE;
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
