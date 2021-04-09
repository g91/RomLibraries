/*!
	@file RuCollisionStructure_PackedGrid.cpp

	Copyright (c) 2004-2009 All rights reserved
*/

#include "../../Engine/Collision/RuCollisionStructure_PackedGrid.h"
#include "../../Engine/Geometry/RuMeshBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuCollisionStructure_PackedGrid, IRuCollisionStructure)
ruHEAP_DEFINE_SUBCLASS(CRuCollisionStructure_PackedGrid, "CRuCollisionStructure_PackedGrid", "IRuCollisionStructure")
ruCLASSGUID_DEFINE(CRuCollisionStructure_PackedGrid, 0x401C7C90, 0xA0784e9f, 0x819F5D8F, 0xDBAF0AB4)

// ************************************************************************************************************************************************************

CRuCollisionStructure_PackedGrid::CRuCollisionStructure_PackedGrid()
:	m_maxFacesPerLeaf(32),
	m_mesh(NULL),
	m_numTriangles(0),
	m_triFlags(NULL),
	m_rootNode(NULL)
{
}

CRuCollisionStructure_PackedGrid::~CRuCollisionStructure_PackedGrid()
{
	DestroyCollisionStructure(m_rootNode);
	delete[] m_triFlags;
}

BOOL CRuCollisionStructure_PackedGrid::AttachToMesh(IRuMesh *mesh)
{
	// Save new mesh reference
	m_mesh = mesh;

	return TRUE;
}

BOOL CRuCollisionStructure_PackedGrid::SetGridParameters(REAL gridSpacing, REAL yBase, REAL yIncrement)
{
	m_gridSpacing = gridSpacing;
	m_yBase = yBase;
	m_yIncrement = yIncrement;
	return TRUE;
}

BOOL CRuCollisionStructure_PackedGrid::BuildCollisionStructure(IRuMesh *mesh, CRuArrayList<INT32> *batchIndices)
{
	return FALSE;
}

BOOL CRuCollisionStructure_PackedGrid::BuildCollisionStructureForRegularGrid(const CRuAABB &gridBounds, INT32 gridXTiles, INT32 gridZTiles)
{
	// Destroy any previous collision data
	DestroyCollisionStructure(m_rootNode);
	delete[] m_triFlags;
	m_triFlags = NULL;

	m_numTriangles = gridXTiles * gridZTiles * 2;

	// Initialize array for triangle flags
	m_triFlags = ruNEW INT32 [m_numTriangles];
	memset(m_triFlags, 0, sizeof(INT32) * m_numTriangles);

	// Initialize new root node
	m_rootNode = ruNEW QuadtreeNode();
	m_rootNode->m_bounds = gridBounds;

	// Build node structure
	struct NodeDesc
	{
		QuadtreeNode*			m_node;
		INT32					m_depth;
		INT32					m_facesAtNode;
		INT32					m_x;
		INT32					m_z;
		INT32					m_width;
		INT32					m_height;
	};

	CRuStack<NodeDesc> nodeStack;

	NodeDesc rootNodeDesc = { m_rootNode, 0, m_numTriangles, 0, 0, gridXTiles, gridZTiles };
	nodeStack.Push(rootNodeDesc);

	while(nodeStack.IsEmpty() == FALSE)
	{
		NodeDesc nodeDesc = nodeStack.Pop();
		QuadtreeNode *node = nodeDesc.m_node;

		if(nodeDesc.m_facesAtNode > m_maxFacesPerLeaf)
		{
			// Construct children and push onto work stack
			INT32 childX[4] = { nodeDesc.m_x, nodeDesc.m_x + nodeDesc.m_width / 2, nodeDesc.m_x + nodeDesc.m_width / 2, nodeDesc.m_x };
			INT32 childZ[4] = { nodeDesc.m_z, nodeDesc.m_z, nodeDesc.m_z + nodeDesc.m_height / 2, nodeDesc.m_z + nodeDesc.m_height / 2 };

			for(INT32 i = 0; i < 4; ++i)
			{
				node->m_children[i] = ruNEW QuadtreeNode();
				node->m_children[i]->m_bounds.SetBoundsToQuadrantXZ(node->m_bounds, i);

				NodeDesc childNodeDesc = { node->m_children[i], nodeDesc.m_depth + 1, nodeDesc.m_facesAtNode / 4, childX[i], childZ[i], nodeDesc.m_width / 2, nodeDesc.m_height / 2 };
				nodeStack.Push(childNodeDesc);
			}
		}
		else
		{
			// Build node data
			node->m_numFaces = nodeDesc.m_facesAtNode;
			node->m_faceIndices = ruNEW UINT16 [node->m_numFaces];

			for(INT32 z = 0, i = 0; z < nodeDesc.m_height; ++z)
			{
				for(INT32 x = 0; x < nodeDesc.m_width; ++x)
				{
					INT32 tileIndex = (nodeDesc.m_x + x) + ((nodeDesc.m_z + z) * gridXTiles);
					INT32 f0Index = tileIndex * 2;
					INT32 f1Index = f0Index + 1;

					node->m_faceIndices[i++] = f0Index;
					node->m_faceIndices[i++] = f1Index;
				}
			}
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_PackedGrid::SerializeFrom(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuCollisionStructure_PackedGrid::SerializeTo(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuCollisionStructure_PackedGrid::QueryCollision(CRuCollisionQuery *colQuery)
{
	BOOL retVal = TRUE;

	switch(colQuery->m_queryType)
	{
		case ruCOLQUERY_RAY:
			{
				CRuCollisionQuery_Ray *rayQuery = static_cast<CRuCollisionQuery_Ray *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 oldRay = rayQuery->m_colRay;
				rayQuery->m_colRay = CRuRay3(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));

				// Make collision query
				retVal = QueryCollision(m_rootNode, rayQuery);;

				// Restore old collision ray
				rayQuery->m_colRay = oldRay;
			}

			break;

		case ruCOLQUERY_RAY_NEAREST:
			{
				CRuCollisionQuery_Ray_Nearest *rayQuery = static_cast<CRuCollisionQuery_Ray_Nearest *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 oldRay = rayQuery->m_colRay;
				rayQuery->m_colRay = CRuRay3(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));

				// Make collision query
				retVal = QueryCollision(m_rootNode, rayQuery);;

				// Restore old collision ray
				rayQuery->m_colRay = oldRay;
			}

			break;

		case ruCOLQUERY_SEGMENT:
			{
				CRuCollisionQuery_Segment *segQuery = static_cast<CRuCollisionQuery_Segment *>(colQuery);

				// Get the segment in local coordinates
				CRuSegment3 oldSegment(segQuery->m_colSegment);
				segQuery->m_invWorldTransform->TransformPoint(segQuery->m_colSegment.Origin());
				segQuery->m_invWorldTransform->TransformVector(segQuery->m_colSegment.Direction());

				// Make collision query
				retVal = QueryCollision(m_rootNode, segQuery);

				// Restore old collision segment
				segQuery->m_colSegment = oldSegment;
			}

			break;

		case ruCOLQUERY_SEGMENT_HITTEST:
			{
				CRuCollisionQuery_Segment_HitTest *segQuery = static_cast<CRuCollisionQuery_Segment_HitTest *>(colQuery);

				// Get the segment in local coordinates
				CRuSegment3 oldSegment(segQuery->m_colSegment);
				segQuery->m_invWorldTransform->TransformPoint(segQuery->m_colSegment.Origin());
				segQuery->m_invWorldTransform->TransformVector(segQuery->m_colSegment.Direction());

				// Make collision query
				retVal = QueryCollision(m_rootNode, segQuery);

				// Restore old collision segment
				segQuery->m_colSegment = oldSegment;
			}

			break;

		case ruCOLQUERY_SEGMENT_NEAREST:
			{
				CRuCollisionQuery_Segment_Nearest *segQuery = static_cast<CRuCollisionQuery_Segment_Nearest *>(colQuery);

				// Get the segment in local coordinates
				CRuSegment3 oldSegment(segQuery->m_colSegment);
				segQuery->m_invWorldTransform->TransformPoint(segQuery->m_colSegment.Origin());
				segQuery->m_invWorldTransform->TransformVector(segQuery->m_colSegment.Direction());

				// Make collision query
				retVal = QueryCollision(m_rootNode, segQuery);

				// Restore old collision segment
				segQuery->m_colSegment = oldSegment;
			}

			break;

		case ruCOLQUERY_OBB_HITTEST:
			{
				CRuCollisionQuery_OBB_HitTest *boxQuery = static_cast<CRuCollisionQuery_OBB_HitTest *>(colQuery);

				// Get the collision OBB in local coordinates
				CRuOBB oldBox(boxQuery->m_colOBB);
				boxQuery->m_colOBB.Transform(*boxQuery->m_invWorldTransform);

				// Make collision query
				retVal = QueryCollision(m_rootNode, boxQuery);

				// Restore old box
				boxQuery->m_colOBB = oldBox;
			}

			break;

		case ruCOLQUERY_OBB_TRIENUM:
			{
				CRuCollisionQuery_OBB_TriEnum *boxQuery = static_cast<CRuCollisionQuery_OBB_TriEnum *>(colQuery);

				// Get the collision OBB in local coordinates
				CRuOBB oldBox(boxQuery->m_colOBB);
				boxQuery->m_colOBB.Transform(*boxQuery->m_invWorldTransform);

				// Make collision query
				retVal = QueryCollision(m_rootNode, boxQuery);

				// Restore old box
				boxQuery->m_colOBB = oldBox;
			}

			break;

		case ruCOLQUERY_SPHERE_HITTEST:
			{
				CRuCollisionQuery_Sphere_HitTest *sphereQuery = static_cast<CRuCollisionQuery_Sphere_HitTest *>(colQuery);

				// Get the sphere in local coordinates
				CRuSphere oldSphere(sphereQuery->m_colSphere);
				sphereQuery->m_invWorldTransform->TransformPoint(sphereQuery->m_colSphere.Center());

				CRuVector3 extents(1.0f, 1.0f, 1.0f);
				sphereQuery->m_invWorldTransform->TransformVector(extents);
				sphereQuery->m_colSphere.Radius() = max(max(fabs(extents.x), fabs(extents.y)), fabs(extents.z)) * sphereQuery->m_colSphere.Radius();

				// Make collision query
				retVal = QueryCollision(m_rootNode, sphereQuery);

				// Restore old sphere
				sphereQuery->m_colSphere = oldSphere;
			}

			break;

		case ruCOLQUERY_SPHERE_TRIENUM:
			{
				CRuCollisionQuery_Sphere_TriEnum *sphereQuery = static_cast<CRuCollisionQuery_Sphere_TriEnum *>(colQuery);

				// Get the sphere in local coordinates
				CRuSphere oldSphere(sphereQuery->m_colSphere);
				sphereQuery->m_invWorldTransform->TransformPoint(sphereQuery->m_colSphere.Center());

				CRuVector3 extents(1.0f, 1.0f, 1.0f);
				sphereQuery->m_invWorldTransform->TransformVector(extents);
				sphereQuery->m_colSphere.Radius() = max(max(fabs(extents.x), fabs(extents.y)), fabs(extents.z)) * sphereQuery->m_colSphere.Radius();

				// Make collision query
				retVal = QueryCollision(m_rootNode, sphereQuery);

				// Restore old sphere
				sphereQuery->m_colSphere = oldSphere;
			}

			break;

		case ruCOLQUERY_ENUMERATE:
			{
				CRuCollisionQuery_Enumerate *enumQuery = static_cast<CRuCollisionQuery_Enumerate *>(colQuery);

				// TODO: Not verified
				retVal = QueryCollision(m_rootNode, enumQuery);
			}

			break;

		case ruCOLQUERY_USER_FLAG_SET:
			{
				CRuCollisionQuery_User_Flag_Set *flagSetQuery = static_cast<CRuCollisionQuery_User_Flag_Set *>(colQuery);

				retVal = QueryCollision(m_rootNode, flagSetQuery);
			}

			break;

		case ruCOLQUERY_USER_FLAG_GET_SEGMENT:
			{
				CRuCollisionQuery_User_Flag_Get_Segment *segQuery = static_cast<CRuCollisionQuery_User_Flag_Get_Segment *>(colQuery);

				// Get the segment in local coordinates
				CRuSegment3 oldSegment(segQuery->m_colSegment);
				segQuery->m_invWorldTransform->TransformPoint(segQuery->m_colSegment.Origin());
				segQuery->m_invWorldTransform->TransformVector(segQuery->m_colSegment.Direction());

				// Make collision query
				retVal = QueryCollision(m_rootNode, segQuery);

				// Restore old collision segment
				segQuery->m_colSegment = oldSegment;
			}

			break;
	}

	return retVal;
}

void CRuCollisionStructure_PackedGrid::DestroyCollisionStructure(QuadtreeNode *node)
{
	if(node)
	{
		// Destroy children
		for(INT32 i = 0; i < 4; ++i)
		{
			DestroyCollisionStructure(node->m_children[i]);
		}

		// Destroy node data
		delete[] node->m_faceIndices;

		// Destroy node
		delete node;
	}
}

BOOL CRuCollisionStructure_PackedGrid::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Ray *rayQuery)
{
	// Get the ray in local coordinates
	CRuRay3 localRay = rayQuery->m_colRay;

	// Test collision with the node's bounding box
	if(Intersects(localRay, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			CRuVector3 intPt;
			REAL intT;

			// Get pointers to vertices & indices
			IRuMorphTarget *morphTarget = m_mesh->GetMorphTarget(0);
			const PackedVertex *vertices = reinterpret_cast<const PackedVertex *>(morphTarget->GetBlendIndex());
			const UINT16 *indices = m_mesh->GetIndices();

			for(INT32 i = 0; i < node->m_numFaces; ++i)
			{
				// Check triangle's flags against the exclusion mask
				if((m_triFlags[node->m_faceIndices[i]] & rayQuery->m_exclusionMask) != 0)
				{
					continue;
				}

				// Check triangle's flags against the inclusion mask
				if(rayQuery->m_inclusionMask && (m_triFlags[node->m_faceIndices[i]] & rayQuery->m_inclusionMask) == 0)
				{
					continue;
				}

				// Setup shorthand for packed vertices
				INT32 i0 = node->m_faceIndices[i] * 3;
				const PackedVertex &pv0 = vertices[indices[i0]];
				const PackedVertex &pv1 = vertices[indices[i0 + 1]];
				const PackedVertex &pv2 = vertices[indices[i0 + 2]];

				// Unpack vertices
				CRuVector3 v0(pv0.m_x * m_gridSpacing, (pv0.m_yH * 255.0f + pv0.m_yL) * m_yIncrement + m_yBase, pv0.m_z * m_gridSpacing);
				CRuVector3 v1(pv1.m_x * m_gridSpacing, (pv1.m_yH * 255.0f + pv1.m_yL) * m_yIncrement + m_yBase, pv1.m_z * m_gridSpacing);
				CRuVector3 v2(pv2.m_x * m_gridSpacing, (pv2.m_yH * 255.0f + pv2.m_yL) * m_yIncrement + m_yBase, pv2.m_z * m_gridSpacing);

				if(FindIntersection(localRay, v0, v1, v2, intPt, intT))
				{
					if(rayQuery->m_ignoreBackface)
					{
						// Perform a backface check
						CRuVector3 e0(v1 - v0);
						CRuVector3 e1(v2 - v0);
						CRuVector3 n = CrossProduct(e0, e1);

						// Normal and ray direction must be opposite
						if(DotProduct(localRay.Direction(), n) < 0.0f)
						{
							rayQuery->m_colPoints->Add(intPt * *(rayQuery->m_worldTransform));
							rayQuery->m_colDistances->Add(intT);
						}
					}
					else
					{
						rayQuery->m_colPoints->Add(intPt * *(rayQuery->m_worldTransform));
						rayQuery->m_colDistances->Add(intT);
					}
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				QueryCollision(node->m_children[i], rayQuery);
			}
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_PackedGrid::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Ray_Nearest *rayQuery)
{
	// Get the ray in local coordinates
	CRuRay3 localRay = rayQuery->m_colRay;

	// Test collision with the node's bounding box
	if(Intersects(localRay, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			CRuVector3 intPt;
			float intT;

			// Get pointers to vertices & indices
			IRuMorphTarget *morphTarget = m_mesh->GetMorphTarget(0);
			const PackedVertex *vertices = reinterpret_cast<const PackedVertex *>(morphTarget->GetBlendIndex());
			const UINT16 *indices = m_mesh->GetIndices();

			for(INT32 i = 0; i < node->m_numFaces; ++i)
			{
				// Check triangle's flags against the exclusion mask
				if((m_triFlags[node->m_faceIndices[i]] & rayQuery->m_exclusionMask) != 0)
				{
					continue;
				}

				// Check triangle's flags against the inclusion mask
				if(rayQuery->m_inclusionMask && (m_triFlags[node->m_faceIndices[i]] & rayQuery->m_inclusionMask) == 0)
				{
					continue;
				}

				// Setup shorthand for packed vertices
				INT32 i0 = node->m_faceIndices[i] * 3;
				const PackedVertex &pv0 = vertices[indices[i0]];
				const PackedVertex &pv1 = vertices[indices[i0 + 1]];
				const PackedVertex &pv2 = vertices[indices[i0 + 2]];

				// Unpack vertices
				CRuVector3 v0(pv0.m_x * m_gridSpacing, (pv0.m_yH * 255.0f + pv0.m_yL) * m_yIncrement + m_yBase, pv0.m_z * m_gridSpacing);
				CRuVector3 v1(pv1.m_x * m_gridSpacing, (pv1.m_yH * 255.0f + pv1.m_yL) * m_yIncrement + m_yBase, pv1.m_z * m_gridSpacing);
				CRuVector3 v2(pv2.m_x * m_gridSpacing, (pv2.m_yH * 255.0f + pv2.m_yL) * m_yIncrement + m_yBase, pv2.m_z * m_gridSpacing);

				if(FindIntersection(localRay, v0, v1, v2, intPt, intT))
				{
					if(rayQuery->m_ignoreBackface)
					{
						// Perform a backface check
						CRuVector3 e0(v1 - v0);
						CRuVector3 e1(v2 - v0);
						CRuVector3 n = CrossProduct(e0, e1);

						// Normal and ray direction must be opposite
						if(DotProduct(localRay.Direction(), n) < 0.0f)
						{
							if(intT < rayQuery->m_collisionT)
							{
								rayQuery->m_positiveHit = TRUE;
								rayQuery->m_collisionT = intT;
								rayQuery->m_collisionPoint = intPt * *(rayQuery->m_worldTransform);
//								rayQuery->m_collisionFlag = m_triFlags[m_leafArray[leafIndex].m_triFaces[i]];
							}
						}
					}
					else
					{
						if(intT < rayQuery->m_collisionT)
						{
							rayQuery->m_positiveHit = TRUE;
							rayQuery->m_collisionT = intT;
							rayQuery->m_collisionPoint = intPt * *(rayQuery->m_worldTransform);
//							rayQuery->m_collisionFlag = m_triFlags[m_leafArray[leafIndex].m_triFaces[i]];
						}
					}
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				QueryCollision(node->m_children[i], rayQuery);
			}
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_PackedGrid::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Segment *segQuery)
{
	// Get the segment in local coordinates
	CRuSegment3 localSegment(segQuery->m_colSegment);

	// Test collision with the node's bounding box
	if(Intersects(localSegment, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			CRuVector3 intPt;
			float intT;

			// Get pointers to vertices & indices
			IRuMorphTarget *morphTarget = m_mesh->GetMorphTarget(0);
			const PackedVertex *vertices = reinterpret_cast<const PackedVertex *>(morphTarget->GetBlendIndex());
			const UINT16 *indices = m_mesh->GetIndices();

			for(INT32 i = 0; i < node->m_numFaces; ++i)
			{
				// Check triangle's flags against the exclusion mask
				if((m_triFlags[node->m_faceIndices[i]] & segQuery->m_exclusionMask) != 0)
				{
					continue;
				}

				// Check triangle's flags against the inclusion mask
				if(segQuery->m_inclusionMask && (m_triFlags[node->m_faceIndices[i]] & segQuery->m_inclusionMask) == 0)
				{
					continue;
				}

				// Setup shorthand for packed vertices
				INT32 i0 = node->m_faceIndices[i] * 3;
				const PackedVertex &pv0 = vertices[indices[i0]];
				const PackedVertex &pv1 = vertices[indices[i0 + 1]];
				const PackedVertex &pv2 = vertices[indices[i0 + 2]];

				// Unpack vertices
				CRuVector3 v0(pv0.m_x * m_gridSpacing, (pv0.m_yH * 255.0f + pv0.m_yL) * m_yIncrement + m_yBase, pv0.m_z * m_gridSpacing);
				CRuVector3 v1(pv1.m_x * m_gridSpacing, (pv1.m_yH * 255.0f + pv1.m_yL) * m_yIncrement + m_yBase, pv1.m_z * m_gridSpacing);
				CRuVector3 v2(pv2.m_x * m_gridSpacing, (pv2.m_yH * 255.0f + pv2.m_yL) * m_yIncrement + m_yBase, pv2.m_z * m_gridSpacing);

				if(FindIntersection(localSegment, v0, v1, v2, intPt, intT))
				{
					if(segQuery->m_ignoreBackface)
					{
						// Perform a backface check
						CRuVector3 e0(v1 - v0);
						CRuVector3 e1(v2 - v0);
						CRuVector3 n = CrossProduct(e0, e1);

						// Normal and ray direction must be opposite
						if(DotProduct(localSegment.Direction(), n) < 0.0f)
						{
							segQuery->m_colPoints->Add(intPt * *(segQuery->m_worldTransform));
							segQuery->m_colDistances->Add(intT);
						}
					}
					else
					{
						segQuery->m_colPoints->Add(intPt * *(segQuery->m_worldTransform));
						segQuery->m_colDistances->Add(intT);
					}
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				QueryCollision(node->m_children[i], segQuery);
			}
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_PackedGrid::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Segment_HitTest *segQuery)
{
	// Get the segment in local coordinates
	CRuSegment3 localSegment(segQuery->m_colSegment);

	// Test collision with the node's bounding box
	if(Intersects(localSegment, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			CRuVector3 intPt;
			float intT;

			// Get pointers to vertices & indices
			IRuMorphTarget *morphTarget = m_mesh->GetMorphTarget(0);
			const PackedVertex *vertices = reinterpret_cast<const PackedVertex *>(morphTarget->GetBlendIndex());
			const UINT16 *indices = m_mesh->GetIndices();

			for(INT32 i = 0; i < node->m_numFaces; ++i)
			{
				// Check triangle's flags against the exclusion mask
				if((m_triFlags[node->m_faceIndices[i]] & segQuery->m_exclusionMask) != 0)
				{
					continue;
				}

				// Check triangle's flags against the inclusion mask
				if(segQuery->m_inclusionMask && (m_triFlags[node->m_faceIndices[i]] & segQuery->m_inclusionMask) == 0)
				{
					continue;
				}

				// Setup shorthand for packed vertices
				INT32 i0 = node->m_faceIndices[i] * 3;
				const PackedVertex &pv0 = vertices[indices[i0]];
				const PackedVertex &pv1 = vertices[indices[i0 + 1]];
				const PackedVertex &pv2 = vertices[indices[i0 + 2]];

				// Unpack vertices
				CRuVector3 v0(pv0.m_x * m_gridSpacing, (pv0.m_yH * 255.0f + pv0.m_yL) * m_yIncrement + m_yBase, pv0.m_z * m_gridSpacing);
				CRuVector3 v1(pv1.m_x * m_gridSpacing, (pv1.m_yH * 255.0f + pv1.m_yL) * m_yIncrement + m_yBase, pv1.m_z * m_gridSpacing);
				CRuVector3 v2(pv2.m_x * m_gridSpacing, (pv2.m_yH * 255.0f + pv2.m_yL) * m_yIncrement + m_yBase, pv2.m_z * m_gridSpacing);

				if(FindIntersection(localSegment, v0, v1, v2, intPt, intT))
				{
					if(segQuery->m_ignoreBackface)
					{
						// Perform a backface check
						CRuVector3 e0(v1 - v0);
						CRuVector3 e1(v2 - v0);
						CRuVector3 n = CrossProduct(e0, e1);

						// Normal and ray direction must be opposite
						if(DotProduct(localSegment.Direction(), n) < 0.0f)
						{
							segQuery->m_positiveHit = TRUE;
							return TRUE;
						}
					}
					else
					{
						segQuery->m_positiveHit = TRUE;
						return TRUE;
					}
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				QueryCollision(node->m_children[i], segQuery);
			}
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_PackedGrid::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Segment_Nearest *segQuery)
{
	// Get the segment in local coordinates
	CRuSegment3 localSegment(segQuery->m_colSegment);

	// Test collision with the node's bounding box
	if(Intersects(localSegment, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			CRuVector3 intPt;
			float intT;

			// Get pointers to vertices & indices
			IRuMorphTarget *morphTarget = m_mesh->GetMorphTarget(0);
			const PackedVertex *vertices = reinterpret_cast<const PackedVertex *>(morphTarget->GetBlendIndex());
			const UINT16 *indices = m_mesh->GetIndices();

			for(INT32 i = 0; i < node->m_numFaces; ++i)
			{
				// Check triangle's flags against the exclusion mask
				if((m_triFlags[node->m_faceIndices[i]] & segQuery->m_exclusionMask) != 0)
				{
					continue;
				}

				// Check triangle's flags against the inclusion mask
				if(segQuery->m_inclusionMask && (m_triFlags[node->m_faceIndices[i]] & segQuery->m_inclusionMask) == 0)
				{
					continue;
				}

				// Setup shorthand for packed vertices
				INT32 i0 = node->m_faceIndices[i] * 3;
				const PackedVertex &pv0 = vertices[indices[i0]];
				const PackedVertex &pv1 = vertices[indices[i0 + 1]];
				const PackedVertex &pv2 = vertices[indices[i0 + 2]];

				// Unpack vertices
				CRuVector3 v0(pv0.m_x * m_gridSpacing, (pv0.m_yH * 255.0f + pv0.m_yL) * m_yIncrement + m_yBase, pv0.m_z * m_gridSpacing);
				CRuVector3 v1(pv1.m_x * m_gridSpacing, (pv1.m_yH * 255.0f + pv1.m_yL) * m_yIncrement + m_yBase, pv1.m_z * m_gridSpacing);
				CRuVector3 v2(pv2.m_x * m_gridSpacing, (pv2.m_yH * 255.0f + pv2.m_yL) * m_yIncrement + m_yBase, pv2.m_z * m_gridSpacing);

				if(FindIntersection(localSegment, v0, v1, v2, intPt, intT))
				{
					CRuVector3 e0(v1 - v0);
					CRuVector3 e1(v2 - v0);
					CRuVector3 n = CrossProduct(e0, e1);

					// Normalize
					n.Normalize();

					// Perform a backface check
					if(segQuery->m_ignoreBackface)
					{
						// Normal and ray direction must be opposite
						if(DotProduct(localSegment.Direction(), n) < 0.0f)
						{
							if(intT < segQuery->m_collisionT)
							{
								segQuery->m_positiveHit = TRUE;
								segQuery->m_collisionT = intT;
								segQuery->m_collisionPoint = intPt * *(segQuery->m_worldTransform);
								segQuery->m_worldTransform->TransformVector(n, segQuery->m_collisionNormal);
								segQuery->m_collisionFlag = m_triFlags[node->m_faceIndices[i]];
							}
						}
					}
					else
					{
						if(intT < segQuery->m_collisionT)
						{
							segQuery->m_positiveHit = TRUE;
							segQuery->m_collisionT = intT;
							segQuery->m_collisionPoint = intPt * *(segQuery->m_worldTransform);
							segQuery->m_worldTransform->TransformVector(n, segQuery->m_collisionNormal);
							segQuery->m_collisionFlag = m_triFlags[node->m_faceIndices[i]];
						}
					}
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				QueryCollision(node->m_children[i], segQuery);
			}
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_PackedGrid::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_OBB_HitTest *boxQuery)
{
	// Test collision with the node's bounding box
	if(Intersects(node->m_bounds, boxQuery->m_colOBB))
	{
		if(node->IsLeaf())
		{
			// Get pointers to vertices & indices
			IRuMorphTarget *morphTarget = m_mesh->GetMorphTarget(0);
			const PackedVertex *vertices = reinterpret_cast<const PackedVertex *>(morphTarget->GetBlendIndex());
			const UINT16 *indices = m_mesh->GetIndices();

			for(INT32 i = 0; i < node->m_numFaces; ++i)
			{
				// Check triangle's flags against the exclusion mask
				if((m_triFlags[node->m_faceIndices[i]] & boxQuery->m_exclusionMask) != 0)
				{
					continue;
				}

				// Check triangle's flags against the inclusion mask
				if(boxQuery->m_inclusionMask && (m_triFlags[node->m_faceIndices[i]] & boxQuery->m_inclusionMask) == 0)
				{
					continue;
				}

				// Setup shorthand for packed vertices
				INT32 i0 = node->m_faceIndices[i] * 3;
				const PackedVertex &pv0 = vertices[indices[i0]];
				const PackedVertex &pv1 = vertices[indices[i0 + 1]];
				const PackedVertex &pv2 = vertices[indices[i0 + 2]];

				// Unpack vertices
				CRuVector3 v0(pv0.m_x * m_gridSpacing, (pv0.m_yH * 255.0f + pv0.m_yL) * m_yIncrement + m_yBase, pv0.m_z * m_gridSpacing);
				CRuVector3 v1(pv1.m_x * m_gridSpacing, (pv1.m_yH * 255.0f + pv1.m_yL) * m_yIncrement + m_yBase, pv1.m_z * m_gridSpacing);
				CRuVector3 v2(pv2.m_x * m_gridSpacing, (pv2.m_yH * 255.0f + pv2.m_yL) * m_yIncrement + m_yBase, pv2.m_z * m_gridSpacing);

				if(IntersectsTriangle(boxQuery->m_colOBB, v0, v1, v2))
				{
					boxQuery->m_positiveHit = TRUE;
					return TRUE;
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				QueryCollision(node->m_children[i], boxQuery);
			}
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_PackedGrid::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_OBB_TriEnum *boxQuery)
{
	// Test collision with the node's bounding box
	if(Intersects(node->m_bounds, boxQuery->m_colOBB))
	{
		if(node->IsLeaf())
		{
			// Get pointers to vertices & indices
			IRuMorphTarget *morphTarget = m_mesh->GetMorphTarget(0);
			const PackedVertex *vertices = reinterpret_cast<const PackedVertex *>(morphTarget->GetBlendIndex());
			const UINT16 *indices = m_mesh->GetIndices();

			for(INT32 i = 0; i < node->m_numFaces; ++i)
			{
				// Check triangle's flags against the exclusion mask
				if((m_triFlags[node->m_faceIndices[i]] & boxQuery->m_exclusionMask) != 0)
				{
					continue;
				}

				// Check triangle's flags against the inclusion mask
				if(boxQuery->m_inclusionMask && (m_triFlags[node->m_faceIndices[i]] & boxQuery->m_inclusionMask) == 0)
				{
					continue;
				}

				// Setup shorthand for packed vertices
				INT32 i0 = node->m_faceIndices[i] * 3;
				const PackedVertex &pv0 = vertices[indices[i0]];
				const PackedVertex &pv1 = vertices[indices[i0 + 1]];
				const PackedVertex &pv2 = vertices[indices[i0 + 2]];

				// Unpack vertices
				CRuVector3 v0(pv0.m_x * m_gridSpacing, (pv0.m_yH * 255.0f + pv0.m_yL) * m_yIncrement + m_yBase, pv0.m_z * m_gridSpacing);
				CRuVector3 v1(pv1.m_x * m_gridSpacing, (pv1.m_yH * 255.0f + pv1.m_yL) * m_yIncrement + m_yBase, pv1.m_z * m_gridSpacing);
				CRuVector3 v2(pv2.m_x * m_gridSpacing, (pv2.m_yH * 255.0f + pv2.m_yL) * m_yIncrement + m_yBase, pv2.m_z * m_gridSpacing);

				if(IntersectsTriangle(boxQuery->m_colOBB, v0, v1, v2))
				{
					// Calculate hit normal
					CRuVector3 e0(v1 - v0);
					CRuVector3 e1(v2 - v0);
					CRuVector3 n = CrossProduct(e0, e1);

					if(n.SquaredMagnitude() > ruEPSILON)
					{
						CRuVector3 p0 = v0;
						CRuVector3 p1 = v1;
						CRuVector3 p2 = v2;
						boxQuery->m_worldTransform->TransformPoint(p0);
						boxQuery->m_worldTransform->TransformPoint(p1);
						boxQuery->m_worldTransform->TransformPoint(p2);

						n.Normalize();
						boxQuery->m_worldTransform->TransformVector(n);

						boxQuery->m_colTriVerts->Add(p0);
						boxQuery->m_colTriVerts->Add(p1);
						boxQuery->m_colTriVerts->Add(p2);

						boxQuery->m_colTriNormals->Add(n);
						boxQuery->m_colTriFlags->Add(m_triFlags[node->m_faceIndices[i]]);
					}
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				QueryCollision(node->m_children[i], boxQuery);
			}
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_PackedGrid::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Sphere_HitTest *sphereQuery)
{
	// Test collision with the node's bounding box
	if(Intersects(node->m_bounds, sphereQuery->m_colSphere))
	{
		if(node->IsLeaf())
		{
			// Get pointers to vertices & indices
			IRuMorphTarget *morphTarget = m_mesh->GetMorphTarget(0);
			const PackedVertex *vertices = reinterpret_cast<const PackedVertex *>(morphTarget->GetBlendIndex());
			const UINT16 *indices = m_mesh->GetIndices();

			for(INT32 i = 0; i < node->m_numFaces; ++i)
			{
				// Check triangle's flags against the exclusion mask
				if((m_triFlags[node->m_faceIndices[i]] & sphereQuery->m_exclusionMask) != 0)
				{
					continue;
				}

				// Check triangle's flags against the inclusion mask
				if(sphereQuery->m_inclusionMask && (m_triFlags[node->m_faceIndices[i]] & sphereQuery->m_inclusionMask) == 0)
				{
					continue;
				}

				// Setup shorthand for packed vertices
				INT32 i0 = node->m_faceIndices[i] * 3;
				const PackedVertex &pv0 = vertices[indices[i0]];
				const PackedVertex &pv1 = vertices[indices[i0 + 1]];
				const PackedVertex &pv2 = vertices[indices[i0 + 2]];

				// Unpack vertices
				CRuVector3 v0(pv0.m_x * m_gridSpacing, (pv0.m_yH * 255.0f + pv0.m_yL) * m_yIncrement + m_yBase, pv0.m_z * m_gridSpacing);
				CRuVector3 v1(pv1.m_x * m_gridSpacing, (pv1.m_yH * 255.0f + pv1.m_yL) * m_yIncrement + m_yBase, pv1.m_z * m_gridSpacing);
				CRuVector3 v2(pv2.m_x * m_gridSpacing, (pv2.m_yH * 255.0f + pv2.m_yL) * m_yIncrement + m_yBase, pv2.m_z * m_gridSpacing);

				if(IntersectsTriangle(sphereQuery->m_colSphere, v0, v1, v2))
				{
					sphereQuery->m_positiveHit = TRUE;
					return TRUE;
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				QueryCollision(node->m_children[i], sphereQuery);
			}
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_PackedGrid::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Sphere_TriEnum *sphereQuery)
{
	// Test collision with the node's bounding box
	if(Intersects(node->m_bounds, sphereQuery->m_colSphere))
	{
		if(node->IsLeaf())
		{
			// Get pointers to vertices & indices
			IRuMorphTarget *morphTarget = m_mesh->GetMorphTarget(0);
			const PackedVertex *vertices = reinterpret_cast<const PackedVertex *>(morphTarget->GetBlendIndex());
			const UINT16 *indices = m_mesh->GetIndices();

			for(INT32 i = 0; i < node->m_numFaces; ++i)
			{
				// Check triangle's flags against the exclusion mask
				if((m_triFlags[node->m_faceIndices[i]] & sphereQuery->m_exclusionMask) != 0)
				{
					continue;
				}

				// Check triangle's flags against the inclusion mask
				if(sphereQuery->m_inclusionMask && (m_triFlags[node->m_faceIndices[i]] & sphereQuery->m_inclusionMask) == 0)
				{
					continue;
				}

				// Setup shorthand for packed vertices
				INT32 i0 = node->m_faceIndices[i] * 3;
				const PackedVertex &pv0 = vertices[indices[i0]];
				const PackedVertex &pv1 = vertices[indices[i0 + 1]];
				const PackedVertex &pv2 = vertices[indices[i0 + 2]];

				// Unpack vertices
				CRuVector3 v0(pv0.m_x * m_gridSpacing, (pv0.m_yH * 255.0f + pv0.m_yL) * m_yIncrement + m_yBase, pv0.m_z * m_gridSpacing);
				CRuVector3 v1(pv1.m_x * m_gridSpacing, (pv1.m_yH * 255.0f + pv1.m_yL) * m_yIncrement + m_yBase, pv1.m_z * m_gridSpacing);
				CRuVector3 v2(pv2.m_x * m_gridSpacing, (pv2.m_yH * 255.0f + pv2.m_yL) * m_yIncrement + m_yBase, pv2.m_z * m_gridSpacing);

				if(IntersectsTriangle(sphereQuery->m_colSphere, v0, v1, v2))
				{
					// Calculate hit normal
					CRuVector3 e0(v1 - v0);
					CRuVector3 e1(v2 - v0);
					CRuVector3 n = CrossProduct(e0, e1);

					if(n.SquaredMagnitude() > ruEPSILON)
					{
						CRuVector3 p0 = v0;
						CRuVector3 p1 = v1;
						CRuVector3 p2 = v2;
						sphereQuery->m_worldTransform->TransformPoint(p0);
						sphereQuery->m_worldTransform->TransformPoint(p1);
						sphereQuery->m_worldTransform->TransformPoint(p2);

						n.Normalize();
						sphereQuery->m_worldTransform->TransformVector(n);

						sphereQuery->m_colTriVerts->Add(p0);
						sphereQuery->m_colTriVerts->Add(p1);
						sphereQuery->m_colTriVerts->Add(p2);

						sphereQuery->m_colTriNormals->Add(n);
						sphereQuery->m_colTriFlags->Add(m_triFlags[node->m_faceIndices[i]]);
					}
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				QueryCollision(node->m_children[i], sphereQuery);
			}
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_PackedGrid::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Enumerate *enumQuery)
{
	if(node->IsLeaf())
	{
		// Get pointers to vertices & indices
		IRuMorphTarget *morphTarget = m_mesh->GetMorphTarget(0);
		const PackedVertex *vertices = reinterpret_cast<const PackedVertex *>(morphTarget->GetBlendIndex());
		const UINT16 *indices = m_mesh->GetIndices();

		for(INT32 i = 0; i < node->m_numFaces; ++i)
		{
			// Setup shorthand for packed vertices
			INT32 i0 = node->m_faceIndices[i] * 3;
			const PackedVertex &pv0 = vertices[indices[i0]];
			const PackedVertex &pv1 = vertices[indices[i0 + 1]];
			const PackedVertex &pv2 = vertices[indices[i0 + 2]];

			// Unpack vertices
			CRuVector3 v0(pv0.m_x * m_gridSpacing, (pv0.m_yH * 255.0f + pv0.m_yL) * m_yIncrement + m_yBase, pv0.m_z * m_gridSpacing);
			CRuVector3 v1(pv1.m_x * m_gridSpacing, (pv1.m_yH * 255.0f + pv1.m_yL) * m_yIncrement + m_yBase, pv1.m_z * m_gridSpacing);
			CRuVector3 v2(pv2.m_x * m_gridSpacing, (pv2.m_yH * 255.0f + pv2.m_yL) * m_yIncrement + m_yBase, pv2.m_z * m_gridSpacing);

			enumQuery->m_colTriVerts->Add(v0);
			enumQuery->m_colTriVerts->Add(v1);
			enumQuery->m_colTriVerts->Add(v2);
		}
	}
	else
	{
		// Recurse to children
		for(INT32 i = 0; i < 4; ++i)
		{
			QueryCollision(node->m_children[i], enumQuery);
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_PackedGrid::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_User_Flag_Set *setQuery)
{
	for(INT32 i = 0; i < m_numTriangles; ++i)
	{
		m_triFlags[i] = (m_triFlags[i] & ~setQuery->m_flagMask) | (setQuery->m_markingFlag & setQuery->m_flagMask);
	}

	setQuery->m_affectedObjects->Add(reinterpret_cast<void *>(this));

	return TRUE;
}

BOOL CRuCollisionStructure_PackedGrid::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_User_Flag_Get_Segment *segQuery)
{
	// Test collision with the node's bounding box
	if(Intersects(segQuery->m_colSegment, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			CRuVector3 intPt;
			float intT;

			// Get pointers to vertices & indices
			IRuMorphTarget *morphTarget = m_mesh->GetMorphTarget(0);
			const PackedVertex *vertices = reinterpret_cast<const PackedVertex *>(morphTarget->GetBlendIndex());
			const UINT16 *indices = m_mesh->GetIndices();

			for(INT32 i = 0; i < node->m_numFaces; ++i)
			{
				// Setup shorthand for packed vertices
				INT32 i0 = node->m_faceIndices[i] * 3;
				const PackedVertex &pv0 = vertices[indices[i0]];
				const PackedVertex &pv1 = vertices[indices[i0 + 1]];
				const PackedVertex &pv2 = vertices[indices[i0 + 2]];

				// Unpack vertices
				CRuVector3 v0(pv0.m_x * m_gridSpacing, (pv0.m_yH * 255.0f + pv0.m_yL) * m_yIncrement + m_yBase, pv0.m_z * m_gridSpacing);
				CRuVector3 v1(pv1.m_x * m_gridSpacing, (pv1.m_yH * 255.0f + pv1.m_yL) * m_yIncrement + m_yBase, pv1.m_z * m_gridSpacing);
				CRuVector3 v2(pv2.m_x * m_gridSpacing, (pv2.m_yH * 255.0f + pv2.m_yL) * m_yIncrement + m_yBase, pv2.m_z * m_gridSpacing);

				if(FindIntersection(segQuery->m_colSegment, v0, v1, v2, intPt, intT))
				{
					if(segQuery->m_ignoreBackface)
					{
						// Perform a backface check
						CRuVector3 e0(v1 - v0);
						CRuVector3 e1(v2 - v0);
						CRuVector3 n = CrossProduct(e0, e1);

						// Normal and ray direction must be opposite
						if(DotProduct(segQuery->m_colSegment.Direction(), n) < 0.0f)
						{
							if(intT < segQuery->m_collisionT)
							{
								segQuery->m_positiveHit = TRUE;
								segQuery->m_collisionT = intT;
								segQuery->m_collisionPoint = intPt * *(segQuery->m_worldTransform);
								segQuery->m_flag = m_triFlags[node->m_faceIndices[i]];
							}
						}
					}
					else
					{
						if(intT < segQuery->m_collisionT)
						{
							segQuery->m_positiveHit = TRUE;
							segQuery->m_collisionT = intT;
							segQuery->m_collisionPoint = intPt * *(segQuery->m_worldTransform);
							segQuery->m_flag = m_triFlags[node->m_faceIndices[i]];
						}
					}
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				QueryCollision(node->m_children[i], segQuery);
			}
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
