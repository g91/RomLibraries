#include "../../Components/NavMesh/RuNavMesh_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuNavMeshGenerator, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuNavMeshGenerator, "CRuNavMeshGenerator", "IRuObject")
ruCLASSGUID_DEFINE(CRuNavMeshGenerator, 0x400035A9, 0x8F144ca1, 0x9196C663, 0xED846995)

// ************************************************************************************************************************************************************

CRuNavMeshGenerator::CRuNavMeshGenerator()
{
}

CRuNavMeshGenerator::~CRuNavMeshGenerator()
{
}

BOOL CRuNavMeshGenerator::InsertIndexedTriangles(INT32 numVertices, INT32 numIndices, CRuVector3 *vertices, UINT16 *indices)
{
	for(INT32 i = 0; i < numIndices; )
	{
		Polygon *poly = ruNEW Polygon();

		poly->m_vertices.Add(vertices[indices[i++]]);
		poly->m_vertices.Add(vertices[indices[i++]]);
		poly->m_vertices.Add(vertices[indices[i++]]);

		CRuVector3 e0 = poly->m_vertices[1] - poly->m_vertices[0];
		CRuVector3 e1 = poly->m_vertices[2] - poly->m_vertices[1];

		poly->m_normal = CrossProduct(e0, e1).Normalize();
		poly->m_centroid = (poly->m_vertices[0] + poly->m_vertices[1] + poly->m_vertices[2]) / 3.0f;

		m_polygons.Add(poly);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
