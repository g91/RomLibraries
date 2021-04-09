/*!
	@file RuEntity_Widget_Grid.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Scene/Widget/RuEntity_Widget_Grid.h"

#include "../../Engine/Geometry/RuPrimitiveToolkit.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Widget_Grid, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Widget_Grid, "CRuEntity_Widget_Grid", "CRuEntity")
ruCLASSGUID_DEFINE(CRuEntity_Widget_Grid, 0x7244AB66, 0xCEA24554, 0x990DD94C, 0x4BE72DEA)

// ************************************************************************************************************************************************************

CRuEntity_Widget_Grid::CRuEntity_Widget_Grid()
{
	m_gridMesh = NULL;
	m_gridMaterial = NULL;
	m_gridModel = NULL;

	m_highlightEnabled = FALSE;
	m_highlightGridMesh = NULL;
	m_highlightGridMaterial = NULL;
	m_highlightGridModel = NULL;
}

CRuEntity_Widget_Grid::~CRuEntity_Widget_Grid()
{
	ruSAFE_RELEASE(m_gridMesh);
	ruSAFE_RELEASE(m_gridMaterial);
	ruSAFE_RELEASE(m_gridModel);

	ruSAFE_RELEASE(m_highlightGridMesh);
	ruSAFE_RELEASE(m_highlightGridMaterial);
	ruSAFE_RELEASE(m_highlightGridModel);
}

void CRuEntity_Widget_Grid::SetGridProperties(const CRuVector3 &gridRight, const CRuVector3 &gridUp, const CRuVector3 &gridOrigin, float gridHalfSize, float gridSpacing)
{
	// Store grid parameters
	m_gridRight = gridRight;
	m_gridUp = gridUp;
	m_gridAt.SetToCrossProduct(m_gridUp, m_gridRight);
	m_gridOrigin = gridOrigin;
	m_gridHalfSize = gridHalfSize;
	m_gridSpacing = gridSpacing;
	m_gridPlane.SetPlane(m_gridUp, m_gridOrigin);

	// Define grid geometry
	INT32 numSegments = (INT32) (gridHalfSize / gridSpacing);
	INT32 vertexCount = CRuPrimitiveToolkit::Grid_VertexCount(numSegments);
	INT32 primitiveCount = CRuPrimitiveToolkit::Grid_PrimitiveCount(numSegments);
	m_gridMesh = ruNEW CRuMesh();
	m_gridMesh->Initialize(ruPRIMTYPE_LINELIST, vertexCount, primitiveCount, ruVERTEXFORMAT_POSITION, 1);

	CRuVector3 *position = static_cast<CRuMorphTarget *>(m_gridMesh->GetMorphTarget(0))->GetPosition();
	UINT16 *indices = m_gridMesh->GetIndices();

	INT32 vertexOffset = 0;
	INT32 indexOffset = 0;
	CRuPrimitiveToolkit::Grid_Build(numSegments, gridSpacing, m_gridAt, m_gridRight, vertexOffset, indexOffset, position, indices);

	m_gridMesh->CalculateBounds();

	// Define grid material
	m_gridMaterial = ruNEW CRuMaterial();
	m_gridMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_NULL"));

	// Define grid model
	m_gridModel = ruNEW CRuModel();
	m_gridModel->AddSubModel(m_gridMesh, m_gridMaterial);
	this->AddChild(m_gridModel);

	// Define highlight grid geometry
	vertexCount = CRuPrimitiveToolkit::PlaneXZ_VertexCount();
	primitiveCount = CRuPrimitiveToolkit::PlaneXZ_PrimitiveCount();
	m_highlightGridMesh = ruNEW CRuMesh();
	m_highlightGridMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, vertexCount, primitiveCount, ruVERTEXFORMAT_POSITION, 1);

	position = static_cast<CRuMorphTarget *>(m_highlightGridMesh->GetMorphTarget(0))->GetPosition();
	indices = m_highlightGridMesh->GetIndices();

	vertexOffset = 0;
	indexOffset = 0;
	CRuPrimitiveToolkit::PlaneXZ_Build(gridSpacing, vertexOffset, indexOffset, position, indices);

	m_highlightGridMaterial = ruNEW CRuMaterial();
	m_highlightGridMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_NULL"));

	m_highlightGridModel = ruNEW CRuModel();
	m_highlightGridModel->AddSubModel(m_highlightGridMesh, m_highlightGridMaterial);

	// Set grid location
	SnapGrid(gridOrigin, CRuVector3(0.0f, 0.0f, 0.0f));
}

void CRuEntity_Widget_Grid::SnapGrid(const CRuVector3 &absOrigin, const CRuVector3 &snapPoint)
{
	CRuVector3 relativeSnapPoint = snapPoint - absOrigin;

	// Clamp snap point to multiple of m_gridSpacing
	relativeSnapPoint.x = floor(relativeSnapPoint.x / m_gridSpacing) * m_gridSpacing;
	relativeSnapPoint.y = floor(relativeSnapPoint.y / m_gridSpacing) * m_gridSpacing;
	relativeSnapPoint.z = floor(relativeSnapPoint.z / m_gridSpacing) * m_gridSpacing;

	m_gridModel->ResetLocalTransform();
	m_gridModel->Translate(relativeSnapPoint + absOrigin);
	m_gridModel->Update(0.0f);
}

void CRuEntity_Widget_Grid::HighlightGrid(BOOL highlight, const CRuRay3 &hitRay)
{
	CRuVector3 intPt;
	float intT;

	// Remove highlight model from tree
	this->RemoveChild(m_highlightGridModel);

	if(FindIntersection(hitRay, m_gridPlane, intPt, intT))
	{
		m_highlightEnabled = highlight;

		CRuVector3 highlightOrigin;

		highlightOrigin.x = floor(intPt.x / m_gridSpacing) * m_gridSpacing + m_gridSpacing / 2.0f;
		highlightOrigin.y = floor(intPt.y / m_gridSpacing) * m_gridSpacing;
		highlightOrigin.z = floor(intPt.z / m_gridSpacing) * m_gridSpacing + m_gridSpacing / 2.0f;

		m_highlightGridModel->ResetLocalTransform();
		m_highlightGridModel->Translate(highlightOrigin);
		m_highlightGridModel->Update(0.0f);

		// Add highlight model to tree
		this->AddChild(m_highlightGridModel);
	}
	else
	{
		m_highlightEnabled = FALSE;
	}
}

BOOL CRuEntity_Widget_Grid::GetHighlightPoint(const CRuRay3 &hitRay, CRuVector3 &intPtOut)
{
	CRuVector3 intPt;
	float intT;

	if(FindIntersection(hitRay, m_gridPlane, intPtOut, intT))
	{
		return TRUE;
	}

	return FALSE;
}

CRuEntity *CRuEntity_Widget_Grid::Clone(CRuEntity *clonedEntity)
{
	return NULL;
}

BOOL CRuEntity_Widget_Grid::ResetStates()
{
	return TRUE;
}

BOOL CRuEntity_Widget_Grid::Update(float elapsedTime)
{
	// Chain update call to base class
	CRuEntity::Update(elapsedTime);

	return TRUE;
}

BOOL CRuEntity_Widget_Grid::QueryCollision(CRuCollisionQuery *colQuery)
{
	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
