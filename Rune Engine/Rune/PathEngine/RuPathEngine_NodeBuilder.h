/*!
	@project Rune
	@file RuPathEngine_NodeBuilder.h

	Copyright (c) 2004-2009 All rights reserved

	@author John Tang
	@date 2009-02-24
*/

#pragma once

#include "../Core/Collection/RuCollection_2DGrid.h"

#include "../PathEngine/RuPathEngine_Base.h"
#include "../PathEngine/RuPathFinder_Base.h"
#include "../PathEngine/RuPathGraph_Base.h"
#include "../PathEngine/RuPathRover_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuPathEngine_NodeBuilder : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct Triangle
	{
		CRuVector3							m_v[3];
		CRuVector3							m_centroid;
		BOOL								m_walkable;

		Triangle()
		{
			m_walkable = TRUE;
		}

		Triangle(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2)
		{
			m_v[0] = v0;
			m_v[1] = v1;
			m_v[2] = v2;
			m_centroid = (m_v[0] + m_v[1] + m_v[2]) / 3.0f;
			m_walkable = TRUE;
		}
	};

	struct CandidatePoint
	{
		CRuVector3							m_position;									//!< Candidate center position
		INT32								m_iX;
		INT32								m_iZ;

		UINT8								m_state;									//!< Candidate state: 0 = unprocessed, 1 = merged, 2 = consumed

		CandidatePoint()
		{
		}

		CandidatePoint(const CRuVector3 &position, INT32 iX, INT32 iZ)
		{
			m_position = position;
			m_iX = iX;
			m_iZ = iZ;
		}

		BOOL operator==(const CandidatePoint &pt) const
		{
			return m_position == pt.m_position ? TRUE : FALSE;
		}
	};

	CRuAABB									m_bounds;									//!< Defining boundary for the node builder to work within
	REAL									m_unitSize;									//!< Unit size for the grid
	REAL									m_stepHeight;								//!< Character step height
	DWORD									m_triExclusionMask;
	CRuArrayList<Triangle>					m_triangles;								//!< Triangles

	CRuArrayList<IRuObject *>				m_geometries;

	CRuCollection_2DGrid<CandidatePoint *>	m_candidateGrid;

	BOOL									m_previewCentroids;
	IRuObject*								m_previewModel;								//!< Preview model

public:
											CRuPathEngine_NodeBuilder();
	virtual									~CRuPathEngine_NodeBuilder();

	BOOL									Initialize(const CRuAABB &bounds, REAL unitSize);					//!< Initialize node builder to specified bounds
	BOOL									SetTriangleExclusionMask(DWORD triExclusionMask);
	BOOL									AppendGeometry(IRuObject *object);			//!< Append new geometry to the node builder's data set

	BOOL									ClassifyGeometry(const CRuVector3 &upVector, REAL slopeLimit);
	BOOL									TessellateGeometry();

	BOOL									GenerateCandidatePoints();
	BOOL									GeneratePathNode(IRuPathGraph_Base *pathGraph);

	IRuObject*								GetPreviewModel();

protected:
	BOOL									AnchorPoint(CRuVector3 &point);
	BOOL									IsValidCandidatePosition(const CRuVector3 &point);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
