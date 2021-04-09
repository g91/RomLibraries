/*!
	@file RuEntity_Widget_Grid.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITY_WIDGET_GRID_H_
#define _RUENTITY_WIDGET_GRID_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	Grid widget.

	@version 2005.04.09
*/
class CRuEntity_Widget_Grid : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuVector3								m_gridRight;
	CRuVector3								m_gridUp;
	CRuVector3								m_gridAt;
	CRuVector3								m_gridOrigin;
	float									m_gridHalfSize;
	float									m_gridSpacing;

	CRuPlane4								m_gridPlane;

	CRuMesh*								m_gridMesh;
	CRuMaterial*							m_gridMaterial;
	CRuModel*								m_gridModel;

	BOOL									m_highlightEnabled;
	CRuMesh*								m_highlightGridMesh;
	CRuMaterial*							m_highlightGridMaterial;
	CRuModel*								m_highlightGridModel;

public:
											CRuEntity_Widget_Grid();
	virtual									~CRuEntity_Widget_Grid();

	void									SetGridProperties(const CRuVector3 &gridRight, const CRuVector3 &gridUp, const CRuVector3 &gridOrigin, float gridHalfSize, float gridSpacing);
	void									SnapGrid(const CRuVector3 &absOrigin, const CRuVector3 &snapPoint);
	void									HighlightGrid(BOOL highlight, const CRuRay3 &hitRay);
	BOOL									GetHighlightPoint(const CRuRay3 &hitRay, CRuVector3 &intPtOut);

	// Base entity interface
	virtual CRuEntity*						Clone(CRuEntity *clonedEntity = NULL);
	virtual BOOL							ResetStates();

	// Base interface
	virtual BOOL							Update(float elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
