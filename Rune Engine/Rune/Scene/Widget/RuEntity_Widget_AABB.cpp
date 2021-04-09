/*!
	@file RuEntity_Widget_AABB.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Scene/Widget/RuEntity_Widget_AABB.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Widget_AABB, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Widget_AABB, "CRuEntity_Widget_AABB", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuEntity_Widget_AABB, 0x6F33E772, 0xEE584b87, 0xA4EF36D5, 0x4138DB87)

// ************************************************************************************************************************************************************

CRuEntity_Widget_AABB::CRuEntity_Widget_AABB()
:	m_mesh(NULL),
	m_material(NULL)
{
}

CRuEntity_Widget_AABB::~CRuEntity_Widget_AABB()
{
	ruSAFE_RELEASE(m_mesh);
	ruSAFE_RELEASE(m_material);
}

void CRuEntity_Widget_AABB::SetAABB(const CRuAABB &box, RUCOLOR color)
{
	if(m_mesh == NULL)
	{
		m_mesh = ruNEW CRuDynamicMesh();
		m_mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, 8, 12, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);
		m_mesh->SetNumPrimitives(12);
		m_mesh->SetNumVertices(8);
	}

	if(m_material == NULL)
	{
		m_material = ruNEW CRuMaterial();
		m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_EDITOR_TRANSLUCENT"));
		m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, FALSE);
	}

	// Set sphere material color
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, color);
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(((color >> 24) & 0xFF) / 255.0f));

	// Initialize vertices
	CRuVector3 *position = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetDiffuse();

	position[0].Set(box.Minima().x, box.Minima().y, box.Minima().z);
	position[1].Set(box.Maxima().x, box.Minima().y, box.Minima().z);
	position[2].Set(box.Maxima().x, box.Maxima().y, box.Minima().z);
	position[3].Set(box.Minima().x, box.Maxima().y, box.Minima().z);
	position[4].Set(box.Minima().x, box.Minima().y, box.Maxima().z);
	position[5].Set(box.Maxima().x, box.Minima().y, box.Maxima().z);
	position[6].Set(box.Maxima().x, box.Maxima().y, box.Maxima().z);
	position[7].Set(box.Minima().x, box.Maxima().y, box.Maxima().z);

	for(INT32 i = 0; i < 8; ++i)
	{
		diffuse[i] = 0xFFFFFFFF;
	}

	// Initialize indices
	INT32 j = 0;
	UINT16 *indices = m_mesh->GetIndices();

	indices[j++] = 0;		indices[j++] = 3;		indices[j++] = 2;
	indices[j++] = 0;		indices[j++] = 2;		indices[j++] = 1;

	indices[j++] = 4;		indices[j++] = 5;		indices[j++] = 6;
	indices[j++] = 4;		indices[j++] = 6;		indices[j++] = 7;

	indices[j++] = 1;		indices[j++] = 2;		indices[j++] = 6;
	indices[j++] = 1;		indices[j++] = 6;		indices[j++] = 5;

	indices[j++] = 0;		indices[j++] = 4;		indices[j++] = 7;
	indices[j++] = 0;		indices[j++] = 7;		indices[j++] = 3;

	indices[j++] = 0;		indices[j++] = 1;		indices[j++] = 5;
	indices[j++] = 0;		indices[j++] = 5;		indices[j++] = 4;

	indices[j++] = 3;		indices[j++] = 7;		indices[j++] = 6;
	indices[j++] = 3;		indices[j++] = 6;		indices[j++] = 2;

	// Update mesh iteration and bounds
	m_mesh->IncrementMeshIteration();
	m_mesh->CalculateBounds();

	// Update local entity bounds
	this->SetLocalEntityBounds(m_mesh->GetBounds());
}

CRuEntity *CRuEntity_Widget_AABB::Clone(CRuEntity *clonedEntity)
{
	return NULL;
}

BOOL CRuEntity_Widget_AABB::ResetStates()
{
	return TRUE;
}

BOOL CRuEntity_Widget_AABB::Update(float elapsedTime)
{
	// Chain update call to base class
	IRuEntity_Renderable::Update(elapsedTime);

	return TRUE;
}

INT32 CRuEntity_Widget_AABB::GetNumMeshes() const
{
	if(m_mesh)
		return 1;

	return 0;
}

BOOL CRuEntity_Widget_AABB::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex == 0)
	{
		*meshOut = m_mesh;
		*materialOut = m_material;

		return TRUE;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed
