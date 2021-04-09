/*!
	@file RuEntity_Widget_TileGrid.cpp

	Copyright (c) 2004-2007 All rights reserved
*/

#include "../../Scene/Widget/RuEntity_Widget_TileGrid.h"

#include "../../Renderer/RuRenderer.h"
#include "../../Renderer/RendererNode/RuRendererNode_RenderContext.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Widget_TileGrid, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Widget_TileGrid, "CRuEntity_Widget_TileGrid", "CRuEntity")
ruCLASSGUID_DEFINE(CRuEntity_Widget_TileGrid, 0xD6DA62BB, 0xD40A453b, 0x8D9EF438, 0xB0FE14C7)

// ************************************************************************************************************************************************************

CRuEntity_Widget_TileGrid::CRuEntity_Widget_TileGrid()
{
	m_tileMesh = NULL;
	m_tileMaterial = NULL;
	m_tileModel = NULL;

	// Register event handlers
	Event_Submitted().RegisterHandler(this->GetGUID(), boost::bind(&CRuEntity_Widget_TileGrid::HandleEvent_Submitted, this, _1));
}

CRuEntity_Widget_TileGrid::~CRuEntity_Widget_TileGrid()
{
	ruSAFE_RELEASE(m_tileMesh);
	ruSAFE_RELEASE(m_tileMaterial);
	ruSAFE_RELEASE(m_tileModel);

	// Clear off used tile models
	while(m_usedTileModels.IsEmpty() == FALSE)
	{
		m_unusedTileModels.Push(m_usedTileModels.Pop());
	}

	while(m_unusedTileModels.IsEmpty() == FALSE)
	{
		CRuModel *model = m_unusedTileModels.Pop();
		ruSAFE_RELEASE(model);
	}
}

BOOL CRuEntity_Widget_TileGrid::Initialize(const CRuVector3 &gridOrigin, INT32 gridSize, REAL tileSize)
{
	REAL halfTileSize = tileSize / 2.0f;

	// Create generic tile mesh
	m_tileMesh = ruNEW CRuMesh();
	m_tileMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, 4, 2, ruVERTEXFORMAT_POSITION);

	CRuVector3 *position = m_tileMesh->GetMorphTarget(0)->GetPosition();

	position[0].Set(-halfTileSize, -5.0f, -halfTileSize);
	position[1].Set( halfTileSize, -5.0f, -halfTileSize);
	position[2].Set(-halfTileSize, -5.0f,  halfTileSize);
	position[3].Set( halfTileSize, -5.0f,  halfTileSize);

	UINT16 *indices = m_tileMesh->GetMeshTopology()->GetIndices();

	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;
	indices[3] = 1;
	indices[4] = 2;
	indices[5] = 3;

	m_tileMesh->CalculateBounds();

	// Create generic tile material
	m_tileMaterial = ruNEW CRuMaterial();

	// Create generic tile model
	m_tileModel = ruNEW CRuModel();
	m_tileModel->AddSubModel(m_tileMesh, m_tileMaterial);

	// Calculate grid bounds
	REAL gridExtents = gridSize * tileSize;
	CRuVector3 gridMax = gridOrigin + CRuVector3(gridExtents, 0.0f, gridExtents);
	CRuVector3 gridCenter = (gridMax + gridOrigin) / 2.0f;

	CRuSphere gridBounds(gridCenter, 0.0f);
	gridBounds.Merge(gridOrigin, 0.0f);
	gridBounds.Merge(gridMax, 0.0f);

	// Set grid bounds as local entity bounds
	this->SetLocalEntityBounds(gridBounds);

	m_gridOrigin = gridOrigin;
	m_gridSize = gridSize;
	m_tileSize = tileSize;

	m_tileBits.SetNumBits(m_gridSize * m_gridSize);

	return TRUE;
}

BOOL CRuEntity_Widget_TileGrid::Reinitialize(const CRuVector3 &gridOrigin, CRuBitField &bitField)
{
	// Calculate grid bounds
	REAL gridExtents = m_gridSize * m_tileSize;
	CRuVector3 gridMax = gridOrigin + CRuVector3(gridExtents, 0.0f, gridExtents);
	CRuVector3 gridCenter = (gridMax + gridOrigin) / 2.0f;

	CRuSphere gridBounds(gridCenter, 0.0f);
	gridBounds.Merge(gridOrigin, 0.0f);
	gridBounds.Merge(gridMax, 0.0f);

	// Set grid bounds as local entity bounds
	this->SetLocalEntityBounds(gridBounds);

	// Store grid origin
	m_gridOrigin = gridOrigin;

	// Clear tile bits dirty
	m_tileBitsDirty = FALSE;

	// Copy entire bitfield
	m_tileBits = bitField;

	// Reset stuff?
	RefreshGridTiles();

	return TRUE;
}

BOOL CRuEntity_Widget_TileGrid::PickTileCoordinatesAt(const CRuSegment3 &segment, INT32 &xOut, INT32 &zOut)
{
	CRuEntity *curChild = this->GetFirstChild();
	while(curChild)
	{
		CRuCollisionQuery_Segment_Nearest segQuery;
		segQuery.m_colSegment = segment;

		curChild->QueryCollision(&segQuery);

		if(segQuery.m_positiveHit)
		{
			RuPropertyType propertyValue;
			curChild->GetProperty("tile coordinates", propertyValue);

			INT32 tileCoords = boost::get<INT32>(propertyValue);

			xOut = (tileCoords >> 16) & 0xFFFF;
			zOut = tileCoords & 0xFFFF;

			return TRUE;
		}

		curChild = curChild->GetNextSibling();
	}

	return FALSE;
}

BOOL CRuEntity_Widget_TileGrid::GetTileBit(INT32 x, INT32 z)
{
	if(x >= 0 && x < m_gridSize && z >= 0 && z < m_gridSize)
	{
		return m_tileBits[x + z * m_gridSize];
	}

	return FALSE;
}

BOOL CRuEntity_Widget_TileGrid::SetTileBit(INT32 x, INT32 z, BOOL bitOn)
{
	if(x >= 0 && x < m_gridSize && z >= 0 && z < m_gridSize)
	{
		if(bitOn)
			m_tileBits.Set(x + z * m_gridSize);
		else
			m_tileBits.Clear(x + z * m_gridSize);

		m_tileBitsDirty = TRUE;

		return TRUE;
	}

	return FALSE;
}

CRuEntity *CRuEntity_Widget_TileGrid::Clone(CRuEntity *clonedEntity)
{
	return NULL;
}

BOOL CRuEntity_Widget_TileGrid::Update(float elapsedTime)
{
	// Chain update call to base class
	CRuEntity::Update(elapsedTime);

	if(m_tileBitsDirty)
	{
		// Refresh grid tiles
		RefreshGridTiles();
		m_tileBitsDirty = FALSE;
	}

	return TRUE;
}

BOOL CRuEntity_Widget_TileGrid::HandleEvent_Submitted(RuEventArgs *eventArgs)
{
	RuEntityEvent_Submitted_Args *submittedEventArgs = static_cast<RuEntityEvent_Submitted_Args *>(eventArgs);
	CRuCamera *camera = submittedEventArgs->m_camera;

	CRuSphere curEnumerationBounds(camera->GetCameraPosition(), 1000.0f);

	if(m_lastEnumerationBounds.Contains(curEnumerationBounds) == FALSE)
	{
		// Pad the enumeration bounds a bit
		m_lastEnumerationBounds = curEnumerationBounds;
		m_lastEnumerationBounds.Radius() += 200.0f;

		// Refresh grid tiles
		RefreshGridTiles();
		m_tileBitsDirty = FALSE;
	}

	return TRUE;
}

BOOL CRuEntity_Widget_TileGrid::RefreshGridTiles()
{
	// Clear off used tile models
	while(m_usedTileModels.IsEmpty() == FALSE)
	{
		m_unusedTileModels.Push(m_usedTileModels.Pop());
	}

	// Detach all children
	this->RemoveAllChildren();

	// Get the enumeration bounds into local space
	CRuSphere localEnumerationBounds(m_lastEnumerationBounds);
	this->GetInverseWorldTransform().TransformPoint(localEnumerationBounds.Center());

	// Determine min and max
	CRuVector3 minPt = localEnumerationBounds.Center() - CRuVector3(localEnumerationBounds.Radius(), localEnumerationBounds.Radius(), localEnumerationBounds.Radius());
	CRuVector3 maxPt = localEnumerationBounds.Center() + CRuVector3(localEnumerationBounds.Radius(), localEnumerationBounds.Radius(), localEnumerationBounds.Radius());

	// Convert min pt into grid space
	INT32 x0 = (INT32) max(0, (minPt.m_x - m_gridOrigin.m_x) / m_tileSize);
	INT32 z0 = (INT32) max(0, (minPt.m_z - m_gridOrigin.m_z) / m_tileSize);
	INT32 x1 = (INT32) min(m_gridSize, x0 + ((maxPt.m_x - minPt.m_x) / m_tileSize));
	INT32 z1 = (INT32) min(m_gridSize, z0 + ((maxPt.m_z - minPt.m_z) / m_tileSize));

	REAL halfTileSize = m_tileSize / 2.0f;

	for(INT32 z = z0; z < z1; ++z)
	{
		for(INT32 x = x0; x < x1; ++x)
		{
			CRuModel *tile = m_unusedTileModels.IsEmpty() ? static_cast<CRuModel *>(m_tileModel->Clone()) : m_unusedTileModels.Pop();
			m_usedTileModels.Push(tile);

			CRuVector3 offset(m_gridOrigin.m_x + x * m_tileSize + halfTileSize, m_gridOrigin.m_y, m_gridOrigin.m_z + z * m_tileSize + halfTileSize);

			tile->SetTranslation(offset);

			INT32 tileCoords = (x << 16) | z;
			tile->SetProperty("tile coordinates", tileCoords);

			// Set tile color
			if(m_tileBits[x + z * m_gridSize])
			{
				tile->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, RUCOLOR_ARGB(255, 255, 0, 0));
			}
			else
			{
				tile->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, RUCOLOR_ARGB(255, 255, 255, 255));
			}

			// Do not show the tile
			tile->SetOption(ruENTITYOPTION_HIDDEN, TRUE);

			this->AddChild(tile);
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
