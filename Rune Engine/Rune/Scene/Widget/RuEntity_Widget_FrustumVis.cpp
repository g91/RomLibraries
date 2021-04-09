/*!
	@file RuEntity_Widget_FrustumVis.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Scene/Widget/RuEntity_Widget_FrustumVis.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Widget_FrustumVis, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Widget_FrustumVis, "CRuEntity_Widget_FrustumVis", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuEntity_Widget_FrustumVis, 0x7E1F9400, 0x595E4ad3, 0xA681267C, 0x34BECFE8)

// ************************************************************************************************************************************************************

CRuEntity_Widget_FrustumVis::CRuEntity_Widget_FrustumVis()
{
	m_frustumMesh = NULL;
	m_frustumMaterial = NULL;
}

CRuEntity_Widget_FrustumVis::~CRuEntity_Widget_FrustumVis()
{
	ruSAFE_RELEASE(m_frustumMesh);
	ruSAFE_RELEASE(m_frustumMaterial);
}

void CRuEntity_Widget_FrustumVis::SetTarget(IRuObject *target)
{
	if(target->GetType().IsSubClassOf(CRuCamera::Type()))
	{
		ruSAFE_RELEASE(m_frustumMesh);
		ruSAFE_RELEASE(m_frustumMaterial);

		m_camera = static_cast<CRuCamera *>(target);

		m_frustumMesh = ruNEW CRuDynamicMesh();
		m_frustumMesh->Initialize(ruPRIMTYPE_LINELIST, 24 + 8, 12 + 4, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);
		m_frustumMesh->SetNumPrimitives(12);
		m_frustumMesh->SetNumVertices(24);

		m_frustumMaterial = ruNEW CRuMaterial();
		m_frustumMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_STATIC_PRELIT"));
	}
}

void CRuEntity_Widget_FrustumVis::SetTSMTarget(IRuObject *target)
{
	if(m_camera && target->GetType().IsSubClassOf(CRuCamera::Type()))
	{
		m_tsmCamera = static_cast<CRuCamera *>(target);

		m_frustumMesh->SetNumPrimitives(12 + 4);
		m_frustumMesh->SetNumVertices(24 + 8);
	}
}

CRuEntity *CRuEntity_Widget_FrustumVis::Clone(CRuEntity *clonedEntity)
{
	return NULL;
}

BOOL CRuEntity_Widget_FrustumVis::ResetStates()
{
	return TRUE;
}

BOOL CRuEntity_Widget_FrustumVis::Update(float elapsedTime)
{
	// Chain update call to base class
	IRuEntity_Renderable::Update(elapsedTime);

	if(m_camera)
	{
		INT32 i = 0;
		CRuVector3 *position = static_cast<CRuMorphTarget *>(m_frustumMesh->GetMorphTarget(0))->GetPosition();
		DWORD *diffuse = static_cast<CRuMorphTarget *>(m_frustumMesh->GetMorphTarget(0))->GetDiffuse();
		UINT16 *indices = m_frustumMesh->GetIndices();

		DWORD colors[4] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFFFFFF00 };

		// Get frustum points in world space
		CRuVector3 frustumPoints[9];
		m_camera->CalculateViewFrustumPoints(m_camera, frustumPoints);
		frustumPoints[8] = m_camera->GetCameraPosition();
/*
		// Setup TSM representation
		if(m_tsmCamera)
		{
			m_tsmCamera->Temp(m_camera, frustumPoints);
		}
*/

		// Build near plane geometry
		position[i] = frustumPoints[0];
		diffuse[i++] = colors[0];
		position[i] = frustumPoints[1];
		diffuse[i++] = colors[0];
		position[i] = frustumPoints[1];
		diffuse[i++] = colors[0];
		position[i] = frustumPoints[2];
		diffuse[i++] = colors[0];
		position[i] = frustumPoints[2];
		diffuse[i++] = colors[0];
		position[i] = frustumPoints[3];
		diffuse[i++] = colors[0];
		position[i] = frustumPoints[3];
		diffuse[i++] = colors[0];
		position[i] = frustumPoints[0];
		diffuse[i++] = colors[0];

		// Build far plane geometry
		position[i] = frustumPoints[4];
		diffuse[i++] = colors[1];
		position[i] = frustumPoints[5];
		diffuse[i++] = colors[1];
		position[i] = frustumPoints[5];
		diffuse[i++] = colors[1];
		position[i] = frustumPoints[6];
		diffuse[i++] = colors[1];
		position[i] = frustumPoints[6];
		diffuse[i++] = colors[1];
		position[i] = frustumPoints[7];
		diffuse[i++] = colors[1];
		position[i] = frustumPoints[7];
		diffuse[i++] = colors[1];
		position[i] = frustumPoints[4];
		diffuse[i++] = colors[1];

		// Build shaft geometry
		position[i] = frustumPoints[0];
		diffuse[i++] = colors[2];
		position[i] = frustumPoints[4];
		diffuse[i++] = colors[2];
		position[i] = frustumPoints[1];
		diffuse[i++] = colors[2];
		position[i] = frustumPoints[5];
		diffuse[i++] = colors[2];
		position[i] = frustumPoints[2];
		diffuse[i++] = colors[2];
		position[i] = frustumPoints[6];
		diffuse[i++] = colors[2];
		position[i] = frustumPoints[3];
		diffuse[i++] = colors[2];
		position[i] = frustumPoints[7];
		diffuse[i++] = colors[2];

		// Setup TSM representation
		if(m_tsmCamera)
		{
			CRuVector3 t0, t1, t2, t3, apex;
			m_tsmCamera->CalculateTrapezoid(m_camera, t0, t1, t2, t3, apex, TRUE);

			position[i] = t3;
			diffuse[i++] = colors[0];
			position[i] = t2;
			diffuse[i++] = colors[2];
			position[i] = t2;
			diffuse[i++] = colors[3];
			position[i] = t1;
			diffuse[i++] = colors[3];
			position[i] = t1;
			diffuse[i++] = colors[2];
			position[i] = t0;
			diffuse[i++] = colors[1];
			position[i] = t0;
			diffuse[i++] = colors[3];
			position[i] = t3;
			diffuse[i++] = colors[3];
		}

		// Build index list
		for(INT32 j = 0; j < i; ++j)
		{
			indices[j] = j;
		}

		// Update mesh iteration and bounds
		m_frustumMesh->IncrementMeshIteration();
		m_frustumMesh->CalculateBounds();

		// Set local entity bounds
		this->SetLocalEntityBounds(m_frustumMesh->GetBounds());
	}

	return TRUE;
}

INT32 CRuEntity_Widget_FrustumVis::GetNumMeshes() const
{
	if(m_frustumMesh)
		return 1;

	return 0;
}

BOOL CRuEntity_Widget_FrustumVis::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex == 0)
	{
		*meshOut = m_frustumMesh;
		*materialOut = m_frustumMaterial;

		return TRUE;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed
