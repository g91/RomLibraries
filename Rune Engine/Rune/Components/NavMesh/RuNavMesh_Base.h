#ifndef _RUNAVMESH_BASE_H_
#define _RUNAVMESH_BASE_H_

// Rune system core
#include "../../Core/Type/RuType_Object.h"
#include "../../Core/Math/RuMathVector.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuNavMeshGenerator : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct Polygon
	{
		CRuVector3						m_normal;
		CRuVector3						m_centroid;
		CRuArrayList<CRuVector3>		m_vertices;
	};

	CRuArrayList<Polygon *>				m_polygons;

public:
										CRuNavMeshGenerator();
	virtual								~CRuNavMeshGenerator();

	BOOL								InsertIndexedTriangles(INT32 numVertices, INT32 numIndices, CRuVector3 *vertices, UINT16 *indices);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
