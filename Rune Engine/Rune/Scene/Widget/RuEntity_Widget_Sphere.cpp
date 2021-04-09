/*!
	@file RuEntity_Widget_Sphere.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Scene/Widget/RuEntity_Widget_Sphere.h"

#include "../../Engine/Geometry/RuPrimitiveToolkit.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Widget_Sphere, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Widget_Sphere, "CRuEntity_Widget_Sphere", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuEntity_Widget_Sphere, 0x5FF05AC8, 0x3EC8495f, 0x963B7EBA, 0x5FF96C91)

// ************************************************************************************************************************************************************

CRuEntity_Widget_Sphere::CRuEntity_Widget_Sphere()
{
	m_numSegments = 20;

	m_sphereMesh = NULL;
	m_sphereMaterial = NULL;
}

CRuEntity_Widget_Sphere::~CRuEntity_Widget_Sphere()
{
	ruSAFE_RELEASE(m_sphereMesh);
	ruSAFE_RELEASE(m_sphereMaterial);
}

void CRuEntity_Widget_Sphere::SetSegments(INT32 segments)
{
	m_numSegments = segments;
}

void CRuEntity_Widget_Sphere::SetSphere(const CRuSphere &sphere, RUCOLOR color)
{
	INT32 vertexCount = CRuPrimitiveToolkit::Sphere_VertexCount(m_numSegments);
	INT32 primitiveCount = CRuPrimitiveToolkit::Sphere_PrimitiveCount(m_numSegments);

	if(m_sphereMesh)
	{
		if(m_sphereMesh->GetNumVertices() < vertexCount)
		{
			// Create new sphere mesh
			ruSAFE_RELEASE(m_sphereMesh);
			m_sphereMesh = ruNEW CRuDynamicMesh();
			m_sphereMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, vertexCount, primitiveCount, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);
		}
	}
	else
	{
		// Create new sphere mesh
		m_sphereMesh = ruNEW CRuDynamicMesh();
		m_sphereMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, vertexCount, primitiveCount, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);

		// Create new sphere material
		m_sphereMaterial = ruNEW CRuMaterial();
		m_sphereMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_EDITOR_TRANSLUCENT"));
		m_sphereMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, FALSE);
	}

	// Set primitive and vertex count
	m_sphereMesh->SetNumPrimitives(primitiveCount);
	m_sphereMesh->SetNumVertices(vertexCount);

	// Set sphere material color
	m_sphereMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, color);
	m_sphereMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(((color >> 24) & 0xFF) / 255.0f));

	// Set sphere scale and location
	CRuVector3 *position = static_cast<CRuMorphTarget *>(m_sphereMesh->GetMorphTarget(0))->GetPosition();
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_sphereMesh->GetMorphTarget(0))->GetDiffuse();
	UINT16 *indices = m_sphereMesh->GetIndices();

	INT32 vertexOffset = 0;
	INT32 indexOffset = 0;
	CRuPrimitiveToolkit::Sphere_Build(m_numSegments, sphere, vertexOffset, indexOffset, position, indices, diffuse);

	// Update mesh iteration and bounds
	m_sphereMesh->IncrementMeshIteration();
	m_sphereMesh->CalculateBounds();

	// Set local entity bounds
	this->SetLocalEntityBounds(m_sphereMesh->GetBounds());
}

BOOL CRuEntity_Widget_Sphere::Update(float elapsedTime)
{
	// Chain update call to base class
	IRuEntity_Renderable::Update(elapsedTime);

	return TRUE;
}

INT32 CRuEntity_Widget_Sphere::GetNumMeshes() const
{
	if(m_sphereMesh)
		return 1;

	return 0;
}

BOOL CRuEntity_Widget_Sphere::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex == 0)
	{
		*meshOut = m_sphereMesh;
		*materialOut = m_sphereMaterial;

		return TRUE;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed
