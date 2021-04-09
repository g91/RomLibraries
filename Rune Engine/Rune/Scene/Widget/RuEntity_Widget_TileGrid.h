/*!
	@file RuEntity_Widget_TileGrid.h

	Copyright (c) 2004-2007 All rights reserved
*/

#ifndef _RUENTITY_WIDGET_TILEGRID_H_
#define _RUENTITY_WIDGET_TILEGRID_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Tile grid widget.

	@version 2007.07.27
*/
class CRuEntity_Widget_TileGrid : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Runtime settings
	CRuVector3								m_gridOrigin;
	INT32									m_gridSize;
	REAL									m_tileSize;

	CRuMesh*								m_tileMesh;
	CRuMaterial*							m_tileMaterial;
	CRuModel*								m_tileModel;

	BOOL									m_tileBitsDirty;
	CRuBitField								m_tileBits;

	CRuSphere								m_lastEnumerationBounds;

	CRuStack<CRuModel *>					m_usedTileModels;
	CRuStack<CRuModel *>					m_unusedTileModels;

public:
											CRuEntity_Widget_TileGrid();
	virtual									~CRuEntity_Widget_TileGrid();

	BOOL									Initialize(const CRuVector3 &gridOrigin, INT32 gridSize, REAL tileSize);

	BOOL									Reinitialize(const CRuVector3 &gridOrigin, CRuBitField &bitField);

	BOOL									PickTileCoordinatesAt(const CRuSegment3 &segment, INT32 &xOut, INT32 &zOut);
	BOOL									GetTileBit(INT32 x, INT32 z);
	BOOL									SetTileBit(INT32 x, INT32 z, BOOL bitOn);

	// Base entity interface
	virtual CRuEntity*						Clone(CRuEntity *clonedEntity = NULL);

	// Base interface
	virtual BOOL							Update(float elapsedTime);

	// Event handlers
	BOOL									HandleEvent_Submitted(RuEventArgs *eventArgs);

protected:
	BOOL									RefreshGridTiles();
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
