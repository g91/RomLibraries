/*!
	@file RuEntity_Widget_LineArray.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Scene/Widget/RuEntity_Widget_Line.h"

#include "../../Engine/Geometry/RuPrimitiveToolkit.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Widget_Line, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Widget_Line, "CRuEntity_Widget_Line", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuEntity_Widget_Line, 0x8460547D, 0xEE1E49cb, 0x99BB114C, 0x777CAAA9)

// ************************************************************************************************************************************************************

CRuEntity_Widget_Line::CRuEntity_Widget_Line()
:	m_mesh(NULL),
	m_material(NULL)
{
}

CRuEntity_Widget_Line::~CRuEntity_Widget_Line()
{
	ruSAFE_RELEASE(m_mesh);
	ruSAFE_RELEASE(m_material);
}

void CRuEntity_Widget_Line::SetLine(const CRuVector3 &v0, const CRuVector3 &v1, BOOL v0Arrow, BOOL v1Arrow, RUCOLOR color)
{
	RUCOLOR m_color = color;
	BOOL m_arrow0Enable = v0Arrow;
	BOOL m_arrow1Enable = v1Arrow;

	INT32 vertexCount = (m_arrow0Enable ? 3 : 0) + (m_arrow1Enable ? 3 : 0) + 4;
	INT32 primitiveCount = (m_arrow0Enable ? 1 : 0) + (m_arrow1Enable ? 1 : 0) + 2;

	ruSAFE_RELEASE(m_mesh);

	// Create new mesh
	m_mesh = ruNEW CRuMesh();
	m_mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, vertexCount, primitiveCount, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);

	// Create new  material
	m_material = ruNEW CRuMaterial();
	m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_EDITOR_TRANSLUCENT"));
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, FALSE);

	// Set material color
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, m_color);
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(((m_color >> 24) & 0xFF) / 255.0f));

	// Build geometry
	CRuVector3 *position = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetDiffuse();
	UINT16 *indices = m_mesh->GetIndices();

	CRuVector3 direction = v1 - v0;
	REAL requiredLength = direction.Magnitude();

	if(requiredLength > ruEPSILON)
	{
		direction.Normalize();
	}
	else
	{
		requiredLength = 1.0f;
		direction.Set(1.0f, 0.0f, 0.0f);
	}

	REAL arrowHeight = 0.10f * requiredLength;
	REAL arrowWidth = 0.025f * requiredLength;
	REAL lineThickness = 0.20f;
	REAL halfArrowWidth = arrowWidth / 2.0f;
	REAL halfLineThickness = lineThickness / 2.0f;
	REAL lineTop = requiredLength;
	REAL lineBottom = 0.0f;

	INT32 vIdx = 0;
	INT32 iIdx = 0;

	if(m_arrow0Enable)
	{
		lineBottom += arrowHeight;

		indices[iIdx++] = vIdx + 0;
		indices[iIdx++] = vIdx + 1;
		indices[iIdx++] = vIdx + 2;

		position[vIdx++].Set(0.0f, 0.0f, 0.0f);
		position[vIdx++].Set(-halfArrowWidth, lineBottom, 0.0f);
		position[vIdx++].Set(halfArrowWidth, lineBottom, 0.0f);
	}

	if(m_arrow1Enable)
	{
		lineTop -= arrowHeight;

		indices[iIdx++] = vIdx + 0;
		indices[iIdx++] = vIdx + 1;
		indices[iIdx++] = vIdx + 2;

		position[vIdx++].Set(0.0f, requiredLength, 0.0f);
		position[vIdx++].Set(halfArrowWidth, lineTop, 0.0f);
		position[vIdx++].Set(-halfArrowWidth, lineTop, 0.0f);
	}

	indices[iIdx++] = vIdx + 0;
	indices[iIdx++] = vIdx + 1;
	indices[iIdx++] = vIdx + 2;

	indices[iIdx++] = vIdx + 1;
	indices[iIdx++] = vIdx + 3;
	indices[iIdx++] = vIdx + 2;

	position[vIdx++].Set(-halfLineThickness, lineTop, 0.0f);
	position[vIdx++].Set(halfLineThickness, lineTop, 0.0f);
	position[vIdx++].Set(-halfLineThickness, lineBottom, 0.0f);
	position[vIdx++].Set(halfLineThickness, lineBottom, 0.0f);

	for(INT32 i = 0; i < vertexCount; ++i)
	{
		diffuse[i] = 0x7FFFFFFF;
	}

	// Calculate mesh bounds
	m_mesh->CalculateBounds();

	// Set local entity bounds
	this->SetLocalEntityBounds(m_mesh->GetBounds());

	// Get model into right orientation
	direction.Normalize();

	CRuQuaternion quat;
	quat.RotateIntoVector(direction, CRuVector3(0.0f, 1.0f, 0.0f));

	ResetLocalTransform();
	Transform(quat.GetMatrix());
	SetTranslation(v0);

	Update(0.0f);
}

BOOL CRuEntity_Widget_Line::Update(float elapsedTime)
{
	// Chain update call to base class
	IRuEntity_Renderable::Update(elapsedTime);

	return TRUE;
}

INT32 CRuEntity_Widget_Line::GetNumMeshes() const
{
	if(m_mesh)
		return 1;

	return 0;
}

BOOL CRuEntity_Widget_Line::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
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
