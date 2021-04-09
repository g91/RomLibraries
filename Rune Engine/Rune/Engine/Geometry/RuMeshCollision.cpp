/*!
	@file RuMeshCollision.cpp

	Copyright (c) 2004 All rights reserved
*/

#include "../../Engine/Geometry/RuMeshCollision.h"
#include "../../Engine/Geometry/RuMeshBase.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(IRuCollisionStructure, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuCollisionStructure, "IRuCollisionStructure", "IRuObject")
ruCLASSGUID_DEFINE(IRuCollisionStructure, 0x7A2A418A, 0x7F974edb, 0x8E8CAD55, 0x88C2497E)

ruRTTI_DEFINE_SUBCLASS(CRuCollisionStructure_Simple, IRuCollisionStructure)
ruHEAP_DEFINE_SUBCLASS(CRuCollisionStructure_Simple, "CRuCollisionStructure_Simple", "IRuCollisionStructure")
ruCLASSGUID_DEFINE(CRuCollisionStructure_Simple, 0x2119818B, 0x3C724cdb, 0xB695FEE4, 0x4E2D62E0)

ruRTTI_DEFINE_SUBCLASS(CRuCollisionStructure_Quadtree, IRuCollisionStructure)
ruHEAP_DEFINE_SUBCLASS(CRuCollisionStructure_Quadtree, "CRuCollisionStructure_Quadtree", "IRuCollisionStructure")
ruCLASSGUID_DEFINE(CRuCollisionStructure_Quadtree, 0x7E19D5AE, 0x23ED4354, 0x8562A1DD, 0x21A2E831)

ruRTTI_DEFINE_SUBCLASS(CRuCollisionStructure_kdTree, IRuCollisionStructure)
ruHEAP_DEFINE_SUBCLASS(CRuCollisionStructure_kdTree, "CRuCollisionStructure_kdTree", "IRuCollisionStructure")
ruCLASSGUID_DEFINE(CRuCollisionStructure_kdTree, 0xB8DC23BE, 0x3F224d40, 0xAEA928EE, 0x4AC9ECBC)

// ************************************************************************************************************************************************************

CRuCollisionStructure_Simple::CRuCollisionStructure_Simple()
:	m_mesh(NULL),
	m_colFlags(NULL),
	m_colIndices(NULL)
{
}

CRuCollisionStructure_Simple::~CRuCollisionStructure_Simple()
{
	delete[] m_colFlags;
	delete[] m_colIndices;
}

BOOL CRuCollisionStructure_Simple::AttachToMesh(IRuMesh *mesh)
{
	 if(mesh->GetNumMorphTargets() == 0)
		 return FALSE;

	// Save new mesh reference
	m_mesh = mesh;

	return TRUE;
}

BOOL CRuCollisionStructure_Simple::BuildCollisionStructure(IRuMesh *mesh, CRuArrayList<INT32> *batchIndices)
{
	if(mesh->GetNumMorphTargets() == 0)
		return FALSE;

	m_mesh = mesh;

	// Setup bounds for the collision structure
	m_bounds = m_mesh->GetBounds();

	// Count the total number of primitives
	m_numColPrimitives = 0;
	INT32 curBatch = 0;
	do
	{
		INT32 primitiveOffset, primitiveCount;

		if(batchIndices == NULL)
		{
			// If a valid batch array is not provided, set the collision structure to work over the entire mesh
			primitiveOffset = 0;
			primitiveCount = m_mesh->GetNumIndices() / 3;
		}
		else
		{
			if(m_mesh->GetBatch(batchIndices->Get(curBatch), primitiveOffset, primitiveCount) == FALSE)
			{
				break;
			}
		}

		// Accumulate primitive count
		m_numColPrimitives += primitiveCount;

		// Increment to next batch
		++curBatch;

	} while(batchIndices && curBatch < batchIndices->Count());

	// Allocate flag array
	m_colFlags = ruNEW UINT32 [m_numColPrimitives];
	memset(m_colFlags, 0, sizeof(UINT32) * m_numColPrimitives);

	// Allocate index array
	m_colIndices = ruNEW UINT16 [m_numColPrimitives * 3];

	// Add triangles to the collision structure
	curBatch = 0;
	const UINT16 *indices = m_mesh->GetIndices();
	do
	{
		INT32 primitiveOffset, primitiveCount;

		if(batchIndices == NULL)
		{
			// If a valid batch array is not provided, set the collision structure to work over the entire mesh
			primitiveOffset = 0;
			primitiveCount = m_mesh->GetNumIndices() / 3;
		}
		else
		{
			if(m_mesh->GetBatch(batchIndices->Get(curBatch), primitiveOffset, primitiveCount) == FALSE)
			{
				break;
			}
		}

		// Put the current batch indices into collision array
		for(INT32 i = 0, j = primitiveOffset * 3, k = 0; i < primitiveCount; ++i)
		{
			m_colIndices[k++] = indices[j++];
			m_colIndices[k++] = indices[j++];
			m_colIndices[k++] = indices[j++];
		}

		// Increment to next batch
		++curBatch;

	} while(batchIndices && curBatch < batchIndices->Count());

	return TRUE;
}

BOOL CRuCollisionStructure_Simple::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	switch(version)
	{
		case 1:
			{
				// Read bounds
				stream->Read(&m_bounds.Center().x, sizeof(REAL));
				stream->Read(&m_bounds.Center().y, sizeof(REAL));
				stream->Read(&m_bounds.Center().z, sizeof(REAL));
				stream->Read(&m_bounds.Radius(), sizeof(REAL));

				// Read number of primitives
				stream->Read(&m_numColPrimitives, sizeof(INT32));

				// Read collision flags
				m_colFlags = ruNEW UINT32 [m_numColPrimitives];
				stream->Read(m_colFlags, sizeof(UINT32) * m_numColPrimitives);

				// Read collision indices
				m_colIndices = ruNEW UINT16 [m_numColPrimitives * 3];
				stream->Read(m_colIndices, sizeof(UINT16) * m_numColPrimitives * 3);
			}

			break;
	}

	return TRUE;
}

BOOL CRuCollisionStructure_Simple::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Write bounds
	stream->Write(&m_bounds.Center().x, sizeof(REAL));
	stream->Write(&m_bounds.Center().y, sizeof(REAL));
	stream->Write(&m_bounds.Center().z, sizeof(REAL));
	stream->Write(&m_bounds.Radius(), sizeof(REAL));

	// Write number of primitives
	stream->Write(&m_numColPrimitives, sizeof(INT32));

	// Write collision flags
	stream->Write(m_colFlags, sizeof(UINT32) * m_numColPrimitives);

	// Write collision indices
	stream->Write(m_colIndices, sizeof(UINT16) * m_numColPrimitives * 3);

	return TRUE;
}

BOOL CRuCollisionStructure_Simple::QueryCollision(CRuCollisionQuery *colQuery)
{
	switch(colQuery->m_queryType)
	{
		case ruCOLQUERY_RAY:
			{
				CRuCollisionQuery_Ray *rayQuery = static_cast<CRuCollisionQuery_Ray *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 localRay(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));

				// Test ray with bounding box
				if(Intersects(localRay, m_bounds))
				{
					// Test with every triangle
					CRuVector3 intPt;
					float intT;
					const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
					for(INT32 i = 0, j = 0; i < m_numColPrimitives; ++i)
					{
						INT32 faceIndices[3] = { m_colIndices[j++], m_colIndices[j++], m_colIndices[j++] };

						if(FindIntersection(localRay, vertices[faceIndices[0]], vertices[faceIndices[1]], vertices[faceIndices[2]], intPt, intT))
						{
							if(rayQuery->m_ignoreBackface)
							{
								// Perform a backface check
								CRuVector3 e0(vertices[faceIndices[1]] - vertices[faceIndices[0]]);
								CRuVector3 e1(vertices[faceIndices[2]] - vertices[faceIndices[0]]);
								CRuVector3 n = CrossProduct(e0, e1);

								// Normal and ray direction must be opposite
								if(DotProduct(localRay.Direction(), n) < 0.0f)
								{
									rayQuery->m_colPoints->Add(intPt * *(colQuery->m_worldTransform));
									rayQuery->m_colDistances->Add(intT);
								}
							}
							else
							{
								rayQuery->m_colPoints->Add(intPt * *(colQuery->m_worldTransform));
								rayQuery->m_colDistances->Add(intT);
							}
						}
					}
				}

				return TRUE;
			}

			break;

		case ruCOLQUERY_SEGMENT:
			{
				CRuCollisionQuery_Segment *segQuery = static_cast<CRuCollisionQuery_Segment *>(colQuery);

				// Get the segment in local coordinates
				CRuSegment3 localSegment(segQuery->m_colSegment);
				segQuery->m_invWorldTransform->TransformPoint(localSegment.Origin());
				segQuery->m_invWorldTransform->TransformVector(localSegment.Direction());

				// Test segment with bounding box
				if(Intersects(localSegment, m_bounds))
				{
					// Test with every triangle
					CRuVector3 intPt;
					float intT;
					const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
					for(INT32 i = 0, j = 0; i < m_numColPrimitives; ++i)
					{
						INT32 faceIndices[3] = { m_colIndices[j++], m_colIndices[j++], m_colIndices[j++] };

						if(FindIntersection(localSegment, vertices[faceIndices[0]], vertices[faceIndices[1]], vertices[faceIndices[2]], intPt, intT))
						{
							if(segQuery->m_ignoreBackface)
							{
								// Perform a backface check
								CRuVector3 e0(vertices[faceIndices[1]] - vertices[faceIndices[0]]);
								CRuVector3 e1(vertices[faceIndices[2]] - vertices[faceIndices[0]]);
								CRuVector3 n = CrossProduct(e0, e1);

								// Normal and ray direction must be opposite
								if(DotProduct(localSegment.Direction(), n) < 0.0f)
								{
									segQuery->m_colPoints->Add(intPt * *(colQuery->m_worldTransform));
									segQuery->m_colDistances->Add(intT);
								}
							}
							else
							{
								segQuery->m_colPoints->Add(intPt * *(colQuery->m_worldTransform));
								segQuery->m_colDistances->Add(intT);
							}
						}
					}
				}

				return TRUE;
			}

			break;

		case ruCOLQUERY_SEGMENT_NEAREST:
			{
				CRuCollisionQuery_Segment_Nearest *segQuery = static_cast<CRuCollisionQuery_Segment_Nearest *>(colQuery);

				// Get the segment in local coordinates
				CRuSegment3 localSegment(segQuery->m_colSegment);
				segQuery->m_invWorldTransform->TransformPoint(localSegment.Origin());
				segQuery->m_invWorldTransform->TransformVector(localSegment.Direction());

				// Test segment with bounding box
				if(Intersects(localSegment, m_bounds))
				{
					// Test with every triangle
					CRuVector3 intPt;
					float intT;
					const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
					for(INT32 i = 0, j = 0; i < m_numColPrimitives; ++i)
					{
						INT32 faceIndices[3] = { m_colIndices[j++], m_colIndices[j++], m_colIndices[j++] };

						if(FindIntersection(localSegment, vertices[faceIndices[0]], vertices[faceIndices[1]], vertices[faceIndices[2]], intPt, intT))
						{
							CRuVector3 e0(vertices[faceIndices[1]] - vertices[faceIndices[0]]);
							CRuVector3 e1(vertices[faceIndices[2]] - vertices[faceIndices[0]]);
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
										segQuery->m_collisionPoint = intPt * *(colQuery->m_worldTransform);
										colQuery->m_worldTransform->TransformVector(n, segQuery->m_collisionNormal);
									}
								}
							}
							else
							{
								if(intT < segQuery->m_collisionT)
								{
									segQuery->m_positiveHit = TRUE;
									segQuery->m_collisionT = intT;
									segQuery->m_collisionPoint = intPt * *(colQuery->m_worldTransform);
									colQuery->m_worldTransform->TransformVector(n, segQuery->m_collisionNormal);
								}
							}
						}
					}
				}

				return TRUE;
			}

			break;

		case ruCOLQUERY_RAY_PICK:
			{
				CRuCollisionQuery_Ray_Pick *rayQuery = static_cast<CRuCollisionQuery_Ray_Pick *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 localRay(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));

				// Test ray with bounding box
				if(Intersects(localRay, m_bounds))
				{
					rayQuery->m_positiveHit = TRUE;

					// temp: return arbitrary distance
					FindIntersection(localRay, m_bounds, rayQuery->m_colDistance);

/*
					// TODO: Support skinned meshes and all that good stuff

					// Test with every triangle
					CRuVector3 intPt;
					float intT;
					const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
					for(INT32 i = 0, j = 0; i < m_numColPrimitives; ++i)
					{
						INT32 faceIndices[3] = { m_colIndices[j++], m_colIndices[j++], m_colIndices[j++] };

						if(FindIntersection(localRay, vertices[faceIndices[0]], vertices[faceIndices[1]], vertices[faceIndices[2]], intPt, intT))
						{
							if(rayQuery->m_ignoreBackface)
							{
								// Perform a backface check
								CRuVector3 e0(vertices[faceIndices[1]] - vertices[faceIndices[0]]);
								CRuVector3 e1(vertices[faceIndices[2]] - vertices[faceIndices[0]]);
								CRuVector3 n = CrossProduct(e0, e1);

								// Normal and ray direction must be opposite
								if(DotProduct(localRay.Direction(), n) < 0.0f)
								{
									rayQuery->m_positiveHit = TRUE;
								}
							}
							else
							{
								rayQuery->m_positiveHit = TRUE;
							}
						}
					}
*/
				}

				return TRUE;
			}

			break;

		case ruCOLQUERY_RAY_TRIPICK:
			{
				CRuCollisionQuery_Ray_TriPick *rayQuery = static_cast<CRuCollisionQuery_Ray_TriPick *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 localRay(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));

				// Test ray with bounding box
				if(Intersects(localRay, m_bounds))
				{
					// Test with every triangle
					CRuVector3 intPt;
					float intT;
					const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
					for(INT32 i = 0, j = 0; i < m_numColPrimitives; ++i)
					{
						INT32 faceIndices[3] = { m_colIndices[j++], m_colIndices[j++], m_colIndices[j++] };

						if(FindIntersection(localRay, vertices[faceIndices[0]], vertices[faceIndices[1]], vertices[faceIndices[2]], intPt, intT))
						{
							if(rayQuery->m_ignoreBackface)
							{
								// Perform a backface check
								CRuVector3 e0(vertices[faceIndices[1]] - vertices[faceIndices[0]]);
								CRuVector3 e1(vertices[faceIndices[2]] - vertices[faceIndices[0]]);
								CRuVector3 n = CrossProduct(e0, e1);

								// Normal and ray direction must be opposite
								if(DotProduct(localRay.Direction(), n) < 0.0f)
								{
									rayQuery->m_colMeshes->Add(m_mesh);
									rayQuery->m_colTriIndices->Add(i);
									rayQuery->m_colDistances->Add(intT);
								}
							}
							else
							{
								rayQuery->m_colMeshes->Add(m_mesh);
								rayQuery->m_colTriIndices->Add(i);
								rayQuery->m_colDistances->Add(intT);
							}
						}
					}
				}

				return TRUE;
			}

			break;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuCollisionStructure_Quadtree::CRuCollisionStructure_Quadtree()
:	m_maxFacesPerLeaf(32),
	m_mesh(NULL),
	m_numTriangles(0),
	m_triFlags(NULL),
	m_rootNode(NULL)
{
}

CRuCollisionStructure_Quadtree::~CRuCollisionStructure_Quadtree()
{
	DestroyCollisionStructure(m_rootNode);
	delete[] m_triFlags;
}

BOOL CRuCollisionStructure_Quadtree::AttachToMesh(IRuMesh *mesh)
{
	// Save new mesh reference
	m_mesh = mesh;

	return TRUE;
}

BOOL CRuCollisionStructure_Quadtree::BuildCollisionStructure(IRuMesh *mesh, CRuArrayList<INT32> *batchIndices)
{
	// Ignore meshes that are not triangle lists and invalid meshes
	if(mesh->GetPrimitiveType() != ruPRIMTYPE_TRIANGLELIST || mesh->GetNumVertices() == 0)
	{
		return FALSE;
	}

	// Save new mesh reference
	m_mesh = mesh;

	// Destroy any previous collision data
	DestroyCollisionStructure(m_rootNode);
	delete[] m_triFlags;
	m_triFlags = NULL;

	INT32 curBatch = 0;
	const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
	const UINT16 *indices = m_mesh->GetIndices();

	// Initialize new root node
	m_rootNode = ruNEW QuadtreeNode();

	// Setup bounding box for the entire tree
	do
	{
		INT32 primitiveOffset, primitiveCount;

		if(batchIndices == NULL)
		{
			// If a valid batch array is not provided, set the collision structure to work over the entire mesh
			primitiveOffset = 0;
			primitiveCount = m_mesh->GetNumIndices() / 3;
		}
		else
		{
			if(m_mesh->GetBatch(batchIndices->Get(curBatch), primitiveOffset, primitiveCount) == FALSE)
			{
				break;
			}
		}

		// Initialize bounding box if this is the first batch
		if(curBatch == 0)
		{
			m_rootNode->m_bounds.SetBounds(vertices[indices[primitiveOffset * 3]], vertices[indices[primitiveOffset * 3]]);
		}

		// Expand bounding box to enclose all vertices in the batch
		for(INT32 i = 0, j = primitiveOffset * 3; i < primitiveCount; ++i)
		{
			m_rootNode->m_bounds.Merge(vertices[indices[j++]]);
			m_rootNode->m_bounds.Merge(vertices[indices[j++]]);
			m_rootNode->m_bounds.Merge(vertices[indices[j++]]);
		}

		// Accumulate number of triangles
		m_numTriangles += primitiveCount;

		// Increment to next batch
		++curBatch;

	} while(batchIndices && curBatch < batchIndices->Count());

	// Extend root node minimum and maximum Y by epsilon to avoid precision errors
	m_rootNode->m_bounds.Minima().m_y -= 1.0f;
	m_rootNode->m_bounds.Maxima().m_y += 1.0f;

	// Add triangles to the tree
	curBatch = 0;
	do
	{
		INT32 primitiveOffset, primitiveCount;

		if(batchIndices == NULL)
		{
			// If a valid batch array is not provided, set the collision structure to work over the entire mesh
			primitiveOffset = 0;
			primitiveCount = m_mesh->GetNumIndices() / 3;
		}
		else
		{
			if(m_mesh->GetBatch(batchIndices->Get(curBatch), primitiveOffset, primitiveCount) == FALSE)
			{
				break;
			}
		}

		// Populate triangle face list
		for(INT32 i = 0; i < primitiveCount; ++i)
		{
			INT32 i0 = (i + primitiveOffset) * 3;
			INT32 vertexIndices[3] = { indices[i0], indices[i0 + 1], indices[i0 + 2] };

			// Ignore degenerates
			if(vertexIndices[0] == vertexIndices[1] || vertexIndices[0] == vertexIndices[2] || vertexIndices[1] == vertexIndices[2])
				continue;

			// Find centroid of the triangle
			CRuVector3 centroid = (vertices[vertexIndices[0]] + vertices[vertexIndices[1]] + vertices[vertexIndices[2]]) / 3.0f;

			// Add triangle to the structure
			AddTriangle(m_rootNode, i + primitiveOffset, centroid, 0);
		}

		// Increment to next batch
		++curBatch;

	} while(batchIndices && curBatch < batchIndices->Count());

	// After the tree is built, the nodes may have triangles protruding from them -- so now we relax the bounding boxes for these nodes
	RelaxTree(m_rootNode);

	// Initialize array for triangle flags
	m_triFlags = ruNEW INT32 [m_numTriangles];
	memset(m_triFlags, 0, sizeof(INT32) * m_numTriangles);

	return TRUE;
}

BOOL CRuCollisionStructure_Quadtree::BuildCollisionStructureForRegularGrid(const CRuAABB &gridBounds, INT32 gridXTiles, INT32 gridZTiles)
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

BOOL CRuCollisionStructure_Quadtree::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read max faces per leaf
	stream->Read(&m_maxFacesPerLeaf, sizeof(INT32));

	// Read number of triangles
	stream->Read(&m_numTriangles, sizeof(INT32));

	// Read triangle flags
	m_triFlags = ruNEW INT32 [m_numTriangles];
	stream->Read(m_triFlags, sizeof(INT32) * m_numTriangles);

	// Read LODMeshInfo structure
	m_rootNode = ruNEW QuadtreeNode();
	
	CRuStack<QuadtreeNode *> nodeStack;
	nodeStack.Push(m_rootNode);

	do
	{
		QuadtreeNode *curNode = nodeStack.Pop();

		// Read node bounds
		stream->Read(&curNode->m_bounds.Minima().x, sizeof(float));
		stream->Read(&curNode->m_bounds.Minima().y, sizeof(float));
		stream->Read(&curNode->m_bounds.Minima().z, sizeof(float));
		stream->Read(&curNode->m_bounds.Maxima().x, sizeof(float));
		stream->Read(&curNode->m_bounds.Maxima().y, sizeof(float));
		stream->Read(&curNode->m_bounds.Maxima().z, sizeof(float));

		// Read number of faces
		stream->Read(&curNode->m_numFaces, sizeof(INT32));

		if(curNode->m_numFaces > 0)
		{
			// Read indices
			curNode->m_faceIndices = ruNEW UINT16 [curNode->m_numFaces];
			stream->Read(curNode->m_faceIndices, sizeof(UINT16) * curNode->m_numFaces);
		}
		else
		{
			curNode->m_faceIndices = NULL;
		}

		// Read whether the node has children
		INT32 hasChildren = 0;
		stream->Read(&hasChildren, sizeof(INT32));

		// Build hierarchy
		if(hasChildren)
		{
			// Push empty QuadtreeNode onto the stack in reverse child order
			for(INT32 i = 3; i >= 0; --i)
			{
				curNode->m_children[i] = ruNEW QuadtreeNode();
				nodeStack.Push(curNode->m_children[i]);
			}
		}
		else
		{
			for(INT32 i = 0; i < 4; ++i)
			{
				curNode->m_children[i] = NULL;
			}
		}
	} while(!nodeStack.IsEmpty());

	return TRUE;
}

BOOL CRuCollisionStructure_Quadtree::SerializeTo(IRuStream *stream)
{
	CRuStack<QuadtreeNode *> nodeStack;
	nodeStack.Push(m_rootNode);

	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write max faces per leaf
	stream->Write(&m_maxFacesPerLeaf, sizeof(INT32));

	// Write number of triangles
	stream->Write(&m_numTriangles, sizeof(INT32));

	// Write triangle flags
	stream->Write(m_triFlags, sizeof(INT32) * m_numTriangles);

	while(!nodeStack.IsEmpty())
	{
		QuadtreeNode *curNode = nodeStack.Pop();

		// Write node bounds
		stream->Write(&curNode->m_bounds.Minima().x, sizeof(float));
		stream->Write(&curNode->m_bounds.Minima().y, sizeof(float));
		stream->Write(&curNode->m_bounds.Minima().z, sizeof(float));
		stream->Write(&curNode->m_bounds.Maxima().x, sizeof(float));
		stream->Write(&curNode->m_bounds.Maxima().y, sizeof(float));
		stream->Write(&curNode->m_bounds.Maxima().z, sizeof(float));

		// Write number of faces
		stream->Write(&curNode->m_numFaces, sizeof(INT32));

		// Write triangle indices
		if(curNode->m_numFaces > 0)
		{
			stream->Write(curNode->m_faceIndices, sizeof(UINT16) * curNode->m_numFaces);
		}

		// Write whether the node has children
		INT32 hasChildren = curNode->IsLeaf() ? 0 : 1;
		stream->Write(&hasChildren, sizeof(INT32));

		// If the node has children, push them onto the stack in reverse order
		if(hasChildren)
		{
			for(INT32 i = 3; i >= 0; --i)
			{
				nodeStack.Push(curNode->m_children[i]);
			}
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_Quadtree::QueryCollision(CRuCollisionQuery *colQuery)
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

void CRuCollisionStructure_Quadtree::RelaxTree()
{
	RelaxTree(m_rootNode);
}

BOOL CRuCollisionStructure_Quadtree::AddTriangle(QuadtreeNode *node, UINT16 triIndex, const CRuVector3 &centroid, INT32 depth)
{
	// Make sure the node contains the centroid of the triangle
	if(node->m_bounds.Contains(centroid))
	{
		// If the node is a leaf, attempt to add the triangle to the node
		if(node->IsLeaf())
		{
			if(node->m_numFaces < m_maxFacesPerLeaf)
			{
				// Extend the face indices array
				UINT16 *newFaceIndices = ruNEW UINT16 [node->m_numFaces + 1];
				memcpy(newFaceIndices, node->m_faceIndices, sizeof(UINT16) * node->m_numFaces);
				delete[] node->m_faceIndices;
				node->m_faceIndices = newFaceIndices;

				// Save new face
				node->m_faceIndices[node->m_numFaces] = triIndex;

				// Increment face count
				++node->m_numFaces;

				return TRUE;
			}
			else
			{
				// Subdivide the node
				SubdivideNode(node, depth);

				// Recurse to children
				for(INT32 i = 0; i < 4; ++i)
				{
					if(AddTriangle(node->m_children[i], triIndex, centroid, depth + 1))
					{
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
				if(AddTriangle(node->m_children[i], triIndex, centroid, depth + 1))
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

void CRuCollisionStructure_Quadtree::SubdivideNode(QuadtreeNode *node, INT32 depth)
{
	// Construct children
	for(INT32 i = 0; i < 4; ++i)
	{
		node->m_children[i] = ruNEW QuadtreeNode();
		node->m_children[i]->m_bounds.SetBoundsToQuadrantXZ(node->m_bounds, i);
	}

	// Dump triangle list into children
	if(node->m_numFaces > 0)
	{
		const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
		const UINT16 *indices = m_mesh->GetIndices();

		for(INT32 i = 0; i < node->m_numFaces; ++i)
		{
			// Get vertex indices
			INT32 i0 = node->m_faceIndices[i] * 3;
			INT32 vertexIndices[3] = { indices[i0], indices[i0 + 1], indices[i0 + 2] };

			// Find centroid of the triangle
			CRuVector3 centroid = (vertices[vertexIndices[0]] + vertices[vertexIndices[1]] + vertices[vertexIndices[2]]) / 3.0f;

			// Dump triangle to one of the children
			for(INT32 j = 0; j < 4; ++j)
			{
				if(AddTriangle(node->m_children[j], node->m_faceIndices[i], centroid, depth + 1))
					break;
			}
		}

		// Delete all triangles at this node
		delete[] node->m_faceIndices;
		node->m_numFaces = 0;
		node->m_faceIndices = NULL;
	}
}

void CRuCollisionStructure_Quadtree::RelaxTree(QuadtreeNode *node)
{
	if(node)
	{
		if(node->IsLeaf())
		{
			// Adjust the bounding box to enclose all contained triangles
			const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
			const UINT16 *indices = m_mesh->GetIndices();

			for(INT32 i = 0, j = 0; i < node->m_numFaces; ++i)
			{
				// Get vertex indices
				INT32 i0 = node->m_faceIndices[i] * 3;
				INT32 vertexIndices[3] = { indices[i0], indices[i0 + 1], indices[i0 + 2] };

				node->m_bounds.Merge(vertices[vertexIndices[0]]);
				node->m_bounds.Merge(vertices[vertexIndices[1]]);
				node->m_bounds.Merge(vertices[vertexIndices[2]]);
			}
		}
		else
		{
			// Relax children first
			for(INT32 i = 0; i < 4; ++i)
			{
				RelaxTree(node->m_children[i]);
			}

			// The node's bounding box is an aggregate of its children's bounding boxes
			node->m_bounds = node->m_children[0]->m_bounds;
			for(INT32 i = 1; i < 4; ++i)
			{
				node->m_bounds.Merge(node->m_children[i]->m_bounds);
			}
		}
	}
}

void CRuCollisionStructure_Quadtree::DestroyCollisionStructure(QuadtreeNode *node)
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

BOOL CRuCollisionStructure_Quadtree::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Ray *rayQuery)
{
	// Get the ray in local coordinates
	CRuRay3 localRay = rayQuery->m_colRay;

	// Test collision with the node's bounding box
	if(Intersects(localRay, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			// Test with every triangle
			const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
			const UINT16 *indices = m_mesh->GetIndices();

			CRuVector3 intPt;
			float intT;

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

				// Get vertex indices
				INT32 i0 = node->m_faceIndices[i] * 3;
				INT32 vertexIndices[3] = { indices[i0], indices[i0 + 1], indices[i0 + 2] };

				if(FindIntersection(localRay, vertices[vertexIndices[0]], vertices[vertexIndices[1]], vertices[vertexIndices[2]], intPt, intT))
				{
					if(rayQuery->m_ignoreBackface)
					{
						// Perform a backface check
						CRuVector3 e0(vertices[vertexIndices[1]] - vertices[vertexIndices[0]]);
						CRuVector3 e1(vertices[vertexIndices[2]] - vertices[vertexIndices[0]]);
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

BOOL CRuCollisionStructure_Quadtree::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Ray_Nearest *rayQuery)
{
	// Get the ray in local coordinates
	CRuRay3 localRay = rayQuery->m_colRay;

	// Test collision with the node's bounding box
	if(Intersects(localRay, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			// Test with every triangle
			const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
			const UINT16 *indices = m_mesh->GetIndices();

			CRuVector3 intPt;
			float intT;

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

				// Get vertex indices
				INT32 i0 = node->m_faceIndices[i] * 3;
				INT32 vertexIndices[3] = { indices[i0], indices[i0 + 1], indices[i0 + 2] };

				if(FindIntersection(localRay, vertices[vertexIndices[0]], vertices[vertexIndices[1]], vertices[vertexIndices[2]], intPt, intT))
				{
					if(rayQuery->m_ignoreBackface)
					{
						// Perform a backface check
						CRuVector3 e0(vertices[vertexIndices[1]] - vertices[vertexIndices[0]]);
						CRuVector3 e1(vertices[vertexIndices[2]] - vertices[vertexIndices[0]]);
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

BOOL CRuCollisionStructure_Quadtree::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Segment *segQuery)
{
	// Get the segment in local coordinates
	CRuSegment3 localSegment(segQuery->m_colSegment);

	// Test collision with the node's bounding box
	if(Intersects(localSegment, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			// Test with every triangle
			const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
			const UINT16 *indices = m_mesh->GetIndices();

			CRuVector3 intPt;
			float intT;

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

				// Get vertex indices
				INT32 i0 = node->m_faceIndices[i] * 3;
				INT32 vertexIndices[3] = { indices[i0], indices[i0 + 1], indices[i0 + 2] };

				if(FindIntersection(localSegment, vertices[vertexIndices[0]], vertices[vertexIndices[1]], vertices[vertexIndices[2]], intPt, intT))
				{
					if(segQuery->m_ignoreBackface)
					{
						// Perform a backface check
						CRuVector3 e0(vertices[vertexIndices[1]] - vertices[vertexIndices[0]]);
						CRuVector3 e1(vertices[vertexIndices[2]] - vertices[vertexIndices[0]]);
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

BOOL CRuCollisionStructure_Quadtree::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Segment_HitTest *segQuery)
{
	// Get the segment in local coordinates
	CRuSegment3 localSegment(segQuery->m_colSegment);

	// Test collision with the node's bounding box
	if(Intersects(localSegment, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			// Test with every triangle
			const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
			const UINT16 *indices = m_mesh->GetIndices();

			CRuVector3 intPt;
			float intT;

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

				// Get vertex indices
				INT32 i0 = node->m_faceIndices[i] * 3;
				INT32 vertexIndices[3] = { indices[i0], indices[i0 + 1], indices[i0 + 2] };

				if(FindIntersection(localSegment, vertices[vertexIndices[0]], vertices[vertexIndices[1]], vertices[vertexIndices[2]], intPt, intT))
				{
					if(segQuery->m_ignoreBackface)
					{
						// Perform a backface check
						CRuVector3 e0(vertices[vertexIndices[1]] - vertices[vertexIndices[0]]);
						CRuVector3 e1(vertices[vertexIndices[2]] - vertices[vertexIndices[0]]);
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

BOOL CRuCollisionStructure_Quadtree::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Segment_Nearest *segQuery)
{
	// Get the segment in local coordinates
	CRuSegment3 localSegment(segQuery->m_colSegment);

	// Test collision with the node's bounding box
	if(Intersects(localSegment, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			// Test with every triangle
			const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
			const UINT16 *indices = m_mesh->GetIndices();

			CRuVector3 intPt;
			float intT;

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

				// Get vertex indices
				INT32 i0 = node->m_faceIndices[i] * 3;
				INT32 vertexIndices[3] = { indices[i0], indices[i0 + 1], indices[i0 + 2] };

				if(FindIntersection(localSegment, vertices[vertexIndices[0]], vertices[vertexIndices[1]], vertices[vertexIndices[2]], intPt, intT))
				{
					CRuVector3 e0(vertices[vertexIndices[1]] - vertices[vertexIndices[0]]);
					CRuVector3 e1(vertices[vertexIndices[2]] - vertices[vertexIndices[0]]);
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

BOOL CRuCollisionStructure_Quadtree::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_OBB_HitTest *boxQuery)
{
	// Test collision with the node's bounding box
	if(Intersects(node->m_bounds, boxQuery->m_colOBB))
	{
		if(node->IsLeaf())
		{
			// Test with every triangle
			const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
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

				// Get vertex indices
				INT32 i0 = node->m_faceIndices[i] * 3;

				const CRuVector3 &v0 = vertices[indices[i0]];
				const CRuVector3 &v1 = vertices[indices[i0 + 1]];
				const CRuVector3 &v2 = vertices[indices[i0 + 2]];

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

BOOL CRuCollisionStructure_Quadtree::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_OBB_TriEnum *boxQuery)
{
	// Test collision with the node's bounding box
	if(Intersects(node->m_bounds, boxQuery->m_colOBB))
	{
		if(node->IsLeaf())
		{
			// Test with every triangle
			const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
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

				// Get vertex indices
				INT32 i0 = node->m_faceIndices[i] * 3;

				const CRuVector3 &v0 = vertices[indices[i0]];
				const CRuVector3 &v1 = vertices[indices[i0 + 1]];
				const CRuVector3 &v2 = vertices[indices[i0 + 2]];

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

BOOL CRuCollisionStructure_Quadtree::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Sphere_HitTest *sphereQuery)
{
	// Test collision with the node's bounding box
	if(Intersects(node->m_bounds, sphereQuery->m_colSphere))
	{
		if(node->IsLeaf())
		{
			// Test with every triangle
			const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
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

				// Get vertex indices
				INT32 i0 = node->m_faceIndices[i] * 3;

				const CRuVector3 &v0 = vertices[indices[i0]];
				const CRuVector3 &v1 = vertices[indices[i0 + 1]];
				const CRuVector3 &v2 = vertices[indices[i0 + 2]];

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

BOOL CRuCollisionStructure_Quadtree::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Sphere_TriEnum *sphereQuery)
{
	// Test collision with the node's bounding box
	if(Intersects(node->m_bounds, sphereQuery->m_colSphere))
	{
		if(node->IsLeaf())
		{
			// Test with every triangle
			const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
			const UINT16 *indices = m_mesh->GetIndices();

			for(INT32 i = 0; i < node->m_numFaces; ++i)
			{
				// Get vertex indices
				INT32 i0 = node->m_faceIndices[i] * 3;

				const CRuVector3 &v0 = vertices[indices[i0]];
				const CRuVector3 &v1 = vertices[indices[i0 + 1]];
				const CRuVector3 &v2 = vertices[indices[i0 + 2]];

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

BOOL CRuCollisionStructure_Quadtree::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Enumerate *enumQuery)
{
	if(node->IsLeaf())
	{
		// Test with every triangle
		const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
		const UINT16 *indices = m_mesh->GetIndices();

		for(INT32 i = 0; i < node->m_numFaces; ++i)
		{
			// Get vertex indices
			INT32 i0 = node->m_faceIndices[i] * 3;

			const CRuVector3 &v0 = vertices[indices[i0]];
			const CRuVector3 &v1 = vertices[indices[i0 + 1]];
			const CRuVector3 &v2 = vertices[indices[i0 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[node->m_faceIndices[i]] & enumQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(enumQuery->m_inclusionMask && (m_triFlags[node->m_faceIndices[i]] & enumQuery->m_inclusionMask) == 0)
			{
				continue;
			}

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

BOOL CRuCollisionStructure_Quadtree::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_User_Flag_Set *setQuery)
{
	for(INT32 i = 0; i < m_numTriangles; ++i)
	{
		m_triFlags[i] = (m_triFlags[i] & ~setQuery->m_flagMask) | (setQuery->m_markingFlag & setQuery->m_flagMask);
	}

	setQuery->m_affectedObjects->Add(reinterpret_cast<void *>(this));

	return TRUE;
}

BOOL CRuCollisionStructure_Quadtree::QueryCollision(QuadtreeNode *node, CRuCollisionQuery_User_Flag_Get_Segment *segQuery)
{
	// Test collision with the node's bounding box
	if(Intersects(segQuery->m_colSegment, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			// Test with every triangle
			const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
			const UINT16 *indices = m_mesh->GetIndices();

			CRuVector3 intPt;
			float intT;

			for(INT32 i = 0; i < node->m_numFaces; ++i)
			{
				// Get vertex indices
				INT32 i0 = node->m_faceIndices[i] * 3;
				INT32 vertexIndices[3] = { indices[i0], indices[i0 + 1], indices[i0 + 2] };

				if(FindIntersection(segQuery->m_colSegment, vertices[vertexIndices[0]], vertices[vertexIndices[1]], vertices[vertexIndices[2]], intPt, intT))
				{
					if(segQuery->m_ignoreBackface)
					{
						// Perform a backface check
						CRuVector3 e0(vertices[vertexIndices[1]] - vertices[vertexIndices[0]]);
						CRuVector3 e1(vertices[vertexIndices[2]] - vertices[vertexIndices[0]]);
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

CRuCollisionStructure_kdTree::CRuCollisionStructure_kdTree()
:	m_numTriangles(0),
	m_triFlags(NULL),
	m_numNodes(0),
	m_numLeaves(0),
	m_nodeArray(NULL),
	m_leafArray(NULL)
{
}

CRuCollisionStructure_kdTree::~CRuCollisionStructure_kdTree()
{
	// Clear shit in arrays
	for(INT32 i = 0; i < m_numLeaves; ++i)
	{
		delete[] m_leafArray[i].m_triFaces;
	}

	// Delete arrays
	delete[] m_triFlags;
	delete[] m_nodeArray;
	delete[] m_leafArray;
}

BOOL CRuCollisionStructure_kdTree::AttachToMesh(IRuMesh *mesh)
{
	// Save new mesh reference
	m_mesh = mesh;

	return TRUE;
}

BOOL CRuCollisionStructure_kdTree::BuildCollisionStructure(IRuMesh *mesh, CRuArrayList<INT32> *batchIndices)
{
	// Ignore meshes that are not triangle lists and invalid meshes
	if(mesh->GetPrimitiveType() != ruPRIMTYPE_TRIANGLELIST || mesh->GetNumVertices() == 0)
	{
		return FALSE;
	}

	// Save new mesh reference
	m_mesh = mesh;

	// Destroy any previous collision data
//	DestroyCollisionStructure(m_rootNode);

	INT32 curBatch = 0;
	const CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
	const UINT16 *indices = m_mesh->GetIndices();

	// Iterate through batches to find bounding box and the total number of triangles
	do
	{
		INT32 primitiveOffset, primitiveCount;

		if(batchIndices == NULL)
		{
			// If a valid batch array is not provided, set the collision structure to work over the entire mesh
			primitiveOffset = 0;
			primitiveCount = m_mesh->GetNumIndices() / 3;
		}
		else
		{
			if(m_mesh->GetBatch(batchIndices->Get(curBatch), primitiveOffset, primitiveCount) == FALSE)
			{
				break;
			}
		}

		// Initialize bounding box if this is the first batch
		if(curBatch == 0)
		{
			m_bounds.SetBounds(vertices[indices[primitiveOffset * 3]], vertices[indices[primitiveOffset * 3]]);
		}

		// Expand bounding box to enclose all vertices in the batch
		for(INT32 i = 0, j = primitiveOffset * 3; i < primitiveCount; ++i)
		{
			m_bounds.Merge(vertices[indices[j++]]);
			m_bounds.Merge(vertices[indices[j++]]);
			m_bounds.Merge(vertices[indices[j++]]);
		}

		// Accumulate number of triangles
		m_numTriangles += primitiveCount;

		// Increment to next batch
		++curBatch;

	} while(batchIndices && curBatch < batchIndices->Count());

	// Initialize array for triangle flags
	m_triFlags = ruNEW INT32 [m_numTriangles];
	memset(m_triFlags, 0, sizeof(INT32) * m_numTriangles);

	CRuArrayList<UINT16> triFaces(m_numTriangles);

	curBatch = 0;

	// Build triangle face list
	do
	{
		INT32 primitiveOffset, primitiveCount;

		if(batchIndices == NULL)
		{
			// If a valid batch array is not provided, set the collision structure to work over the entire mesh
			primitiveOffset = 0;
			primitiveCount = m_mesh->GetNumIndices() / 3;
		}
		else
		{
			if(m_mesh->GetBatch(batchIndices->Get(curBatch), primitiveOffset, primitiveCount) == FALSE)
			{
				break;
			}
		}

		// Populate triangle face list
		for(INT32 i = 0; i < primitiveCount; ++i)
		{
			triFaces.Add(i + primitiveOffset);
		}

		// Increment to next batch
		++curBatch;

	} while(batchIndices && curBatch < batchIndices->Count());

	// Determine maximum number of nodes
	INT32 maxTriPerLeaf = 100;
	INT32 maxDepth = 12;
	INT32 maxNodes = 0;
	for(INT32 i = 0; i < maxDepth; ++i)
	{
		maxNodes += (INT32) pow(2.0f, i);
	}

	// Build tree
	CRuArrayList<kdNode> nodes(maxNodes);
	CRuArrayList<kdLeafData> leaves;

	kdNode node;
	for(INT32 i = 0; i < maxNodes; ++i)
	{
		nodes.Add(node);
	}

	INT32 nodesUsed = BuildTree(0, vertices, indices, triFaces, 1, m_bounds, maxTriPerLeaf, maxDepth, nodes, leaves);

	// Copy crap
	m_numNodes = nodesUsed;
	m_numLeaves = leaves.Count();
	m_nodeArray = ruNEW kdNode [m_numNodes];
	m_leafArray = ruNEW kdLeafData [m_numLeaves];

	memcpy(m_nodeArray, nodes.GetArray(), sizeof(kdNode) * m_numNodes);
	memcpy(m_leafArray, leaves.GetArray(), sizeof(kdLeafData) * m_numLeaves);

	INT32 totalFaces = 0;
	for(INT32 i = 0; i < m_numLeaves; ++i)
	{
		totalFaces += m_leafArray[i].m_numTriangles;
	}

	return TRUE;
}


BOOL CRuCollisionStructure_kdTree::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;
	INT32 version = 1;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	stream->Read(&version, sizeof(INT32));

	// Read bounds
	stream->Read(&m_bounds.Minima().x, sizeof(float));
	stream->Read(&m_bounds.Minima().y, sizeof(float));
	stream->Read(&m_bounds.Minima().z, sizeof(float));
	stream->Read(&m_bounds.Maxima().x, sizeof(float));
	stream->Read(&m_bounds.Maxima().y, sizeof(float));
	stream->Read(&m_bounds.Maxima().z, sizeof(float));

	// Read number of vertices
	stream->Read(&m_numTriangles, sizeof(INT32));

	// Read number of nodes
	stream->Read(&m_numNodes, sizeof(INT32));

	// Read number of leaves
	stream->Read(&m_numLeaves, sizeof(INT32));

	// Read flags array
	m_triFlags = ruNEW INT32 [m_numTriangles];
	for(INT32 i = 0; i < m_numTriangles; ++i)
	{
		stream->Read(&m_triFlags[i], sizeof(INT32));
	}

	// Allocate nodes and read from stream
	m_nodeArray = ruNEW kdNode [m_numNodes];
	stream->Read(m_nodeArray, sizeof(kdNode) * m_numNodes);

	// Allocate leaves and read from stream
	m_leafArray = ruNEW kdLeafData [m_numLeaves];
	for(INT32 i = 0; i < m_numLeaves; ++i)
	{
		// Read number of triangles
		stream->Read(&m_leafArray[i].m_numTriangles, sizeof(INT32));

		// Read triangles
		m_leafArray[i].m_triFaces = ruNEW UINT16 [m_leafArray[i].m_numTriangles];
		for(INT32 j = 0; j < m_leafArray[i].m_numTriangles; ++j)
		{
			stream->Read(&m_leafArray[i].m_triFaces[j], sizeof(UINT16));
		}
	}


	return TRUE;
}

BOOL CRuCollisionStructure_kdTree::SerializeTo(IRuStream *stream)
{
	const INT32 version = 1;

	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	stream->Write(&version, sizeof(INT32));

	// Write bounds
	stream->Write(&m_bounds.Minima().x, sizeof(float));
	stream->Write(&m_bounds.Minima().y, sizeof(float));
	stream->Write(&m_bounds.Minima().z, sizeof(float));
	stream->Write(&m_bounds.Maxima().x, sizeof(float));
	stream->Write(&m_bounds.Maxima().y, sizeof(float));
	stream->Write(&m_bounds.Maxima().z, sizeof(float));

	// Write number of vertices
	stream->Write(&m_numTriangles, sizeof(INT32));

	// Write number of nodes
	stream->Write(&m_numNodes, sizeof(INT32));

	// Write number of leaves
	stream->Write(&m_numLeaves, sizeof(INT32));

	// Write flags array
	stream->Write(m_triFlags, sizeof(INT32) * m_numTriangles);

	// Write nodes
	stream->Write(m_nodeArray, sizeof(kdNode) * m_numNodes);

	// Write leaves
	for(INT32 i = 0; i < m_numLeaves; ++i)
	{
		// Write number of triangles
		stream->Write(&m_leafArray[i].m_numTriangles, sizeof(INT32));

		// Write triangle face indices
		stream->Write(m_leafArray[i].m_triFaces, sizeof(UINT16) * m_leafArray[i].m_numTriangles);
	}

	return TRUE;
}

INT32 CRuCollisionStructure_kdTree::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuCollisionStructure::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	footprint += sizeof(m_bounds);
	footprint += sizeof(m_numNodes);
	footprint += sizeof(m_numLeaves);
	footprint += sizeof(m_nodeArray) + (m_nodeArray ? sizeof(kdNode) * m_numNodes : 0);
	footprint += sizeof(m_leafArray) + (m_leafArray ? sizeof(kdLeafData) * m_numLeaves : 0);
	for(INT32 i = 0; i < m_numLeaves; ++i)
	{
		footprint += sizeof(m_leafArray[i].m_numTriangles);
		footprint += sizeof(m_leafArray[i].m_triFaces) + (m_leafArray[i].m_triFaces ? sizeof(UINT16) * m_leafArray[i].m_numTriangles : 0);
	}

	footprint += sizeof(m_triTested);

	return footprint;
}

const CRuAABB &CRuCollisionStructure_kdTree::GetBounds() const
{
	return m_bounds;
}

BOOL CRuCollisionStructure_kdTree::QueryCollision(CRuCollisionQuery *colQuery)
{
	BOOL retVal = TRUE;

	switch(colQuery->m_queryType)
	{
		case ruCOLQUERY_RAY:
			{
				CRuCollisionQuery_Ray_Base *rayQuery = static_cast<CRuCollisionQuery_Ray_Base *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 oldRay = rayQuery->m_colRay;
				rayQuery->m_colRay = CRuRay3(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));

				// Make collision query
				if(Intersects(rayQuery->m_colRay, m_bounds))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					retVal = QueryCollision(static_cast<CRuCollisionQuery_Ray *>(colQuery), 0, 0.0f, FLT_MAX);
				}

				// Restore old collision ray
				rayQuery->m_colRay = oldRay;
			}

			break;

		case ruCOLQUERY_RAY_NEAREST:
			{
				CRuCollisionQuery_Ray_Base *rayQuery = static_cast<CRuCollisionQuery_Ray_Base *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 oldRay = rayQuery->m_colRay;
				rayQuery->m_colRay = CRuRay3(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));

				// Make collision query
				if(Intersects(rayQuery->m_colRay, m_bounds))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					retVal = QueryCollision(static_cast<CRuCollisionQuery_Ray_Nearest *>(colQuery), 0, 0.0f, FLT_MAX);
				}

				// Restore old collision ray
				rayQuery->m_colRay = oldRay;
			}

			break;

		case ruCOLQUERY_RAY_PICK:
			{
				CRuCollisionQuery_Ray_Pick *rayQuery = static_cast<CRuCollisionQuery_Ray_Pick *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 oldRay = rayQuery->m_colRay;
				rayQuery->m_colRay = CRuRay3(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));

				// Make collision query
				if(Intersects(rayQuery->m_colRay, m_bounds))
				{
					rayQuery->m_positiveHit = TRUE;

					// temp: return arbitrary distance
					CRuVector3 dummyPt;
					FindIntersection(rayQuery->m_colRay, m_bounds, dummyPt, rayQuery->m_colDistance);

					retVal = TRUE;
				}

				// Restore old collision ray
				rayQuery->m_colRay = oldRay;
			}

			break;

		case ruCOLQUERY_RAY_TRIPICK:
			{
			}

			break;

		case ruCOLQUERY_SEGMENT:
			{
				CRuCollisionQuery_Segment_Base *segQuery = static_cast<CRuCollisionQuery_Segment_Base *>(colQuery);

				// Get the segment in local coordinates
				CRuSegment3 oldSegment(segQuery->m_colSegment);
				segQuery->m_invWorldTransform->TransformPoint(segQuery->m_colSegment.Origin());
				segQuery->m_invWorldTransform->TransformVector(segQuery->m_colSegment.Direction());

				// Make collision query
				if(Intersects(segQuery->m_colSegment, m_bounds))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					retVal = QueryCollision(static_cast<CRuCollisionQuery_Segment *>(colQuery), 0, 0.0f, 1.0f);
				}

				// Restore old collision segment
				segQuery->m_colSegment = oldSegment;
			}

			break;

		case ruCOLQUERY_SEGMENT_HITTEST:
			{
				CRuCollisionQuery_Segment_Base *segQuery = static_cast<CRuCollisionQuery_Segment_Base *>(colQuery);

				// Get the segment in local coordinates
				CRuSegment3 oldSegment(segQuery->m_colSegment);
				segQuery->m_invWorldTransform->TransformPoint(segQuery->m_colSegment.Origin());
				segQuery->m_invWorldTransform->TransformVector(segQuery->m_colSegment.Direction());

				// Make collision query
				if(Intersects(segQuery->m_colSegment, m_bounds))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					retVal = QueryCollision(static_cast<CRuCollisionQuery_Segment_HitTest *>(colQuery), 0, 0.0f, 1.0f);
				}

				// Restore old collision segment
				segQuery->m_colSegment = oldSegment;
			}

			break;

		case ruCOLQUERY_SEGMENT_NEAREST:
			{
				CRuCollisionQuery_Segment_Base *segQuery = static_cast<CRuCollisionQuery_Segment_Base *>(colQuery);

				// Get the segment in local coordinates
				CRuSegment3 oldSegment(segQuery->m_colSegment);
				segQuery->m_invWorldTransform->TransformPoint(segQuery->m_colSegment.Origin());
				segQuery->m_invWorldTransform->TransformVector(segQuery->m_colSegment.Direction());

				// Make collision query
				if(Intersects(segQuery->m_colSegment, m_bounds))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					retVal = QueryCollision(static_cast<CRuCollisionQuery_Segment_Nearest *>(colQuery), 0, 0.0f, 1.0f);
				}

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

				if(Intersects(m_bounds, static_cast<CRuCollisionQuery_OBB_HitTest *>(colQuery)->m_colOBB))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					QueryCollision(static_cast<CRuCollisionQuery_OBB_HitTest *>(colQuery), 0, m_bounds);
				}

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

				if(Intersects(m_bounds, static_cast<CRuCollisionQuery_OBB_TriEnum *>(colQuery)->m_colOBB))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					QueryCollision(static_cast<CRuCollisionQuery_OBB_TriEnum *>(colQuery), 0, m_bounds);
				}

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
				if(Intersects(m_bounds, sphereQuery->m_colSphere))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					CRuVector3 volNearPt = sphereQuery->m_colSphere.Center();
					retVal = QueryCollision(sphereQuery, 0, volNearPt);
				}

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
				if(Intersects(m_bounds, sphereQuery->m_colSphere))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					CRuVector3 volNearPt = sphereQuery->m_colSphere.Center();
					retVal = QueryCollision(static_cast<CRuCollisionQuery_Sphere_TriEnum *>(colQuery), 0, volNearPt);
				}

				// Restore old sphere
				sphereQuery->m_colSphere = oldSphere;
			}

			break;

		case ruCOLQUERY_ENUMERATE:
			// TODO: Not verified
			return QueryCollision(static_cast<CRuCollisionQuery_Enumerate *>(colQuery), 0);

		case ruCOLQUERY_USER_FLAG_SET:
			{
				CRuCollisionQuery_User_Flag_Set *flagSetQuery = static_cast<CRuCollisionQuery_User_Flag_Set *>(colQuery);

				retVal = QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Set *>(colQuery), 0);
			}

			break;

		case ruCOLQUERY_USER_FLAG_SET_RAY:
			{
				CRuCollisionQuery_User_Flag_Set_Ray *rayQuery = static_cast<CRuCollisionQuery_User_Flag_Set_Ray *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 oldRay = rayQuery->m_markingRay;
				rayQuery->m_markingRay = CRuRay3(rayQuery->m_markingRay, *(rayQuery->m_invWorldTransform));

				// Make collision query
				if(Intersects(rayQuery->m_markingRay, m_bounds))
				{
					retVal = QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Set_Ray *>(colQuery), 0, 0.0f, FLT_MAX);
				}

				// Restore old collision ray
				rayQuery->m_markingRay = oldRay;
			}

			break;

		case ruCOLQUERY_USER_FLAG_SET_SPHERE:
			{
				CRuCollisionQuery_User_Flag_Set_Sphere *sphereQuery = static_cast<CRuCollisionQuery_User_Flag_Set_Sphere *>(colQuery);

				// Get the sphere in local coordinates
				CRuSphere oldSphere(sphereQuery->m_markingSphere);
				sphereQuery->m_invWorldTransform->TransformPoint(sphereQuery->m_markingSphere.Center());

				CRuVector3 extents(1.0f, 1.0f, 1.0f);
				sphereQuery->m_invWorldTransform->TransformVector(extents);
				sphereQuery->m_markingSphere.Radius() = max(max(fabs(extents.x), fabs(extents.y)), fabs(extents.z)) * sphereQuery->m_markingSphere.Radius();

				// Make collision query
				if(Intersects(m_bounds, sphereQuery->m_markingSphere))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					CRuVector3 volNearPt = sphereQuery->m_markingSphere.Center();
					retVal = QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Set_Sphere *>(colQuery), 0, volNearPt);
				}

				// Restore old sphere
				sphereQuery->m_markingSphere = oldSphere;
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
				if(Intersects(segQuery->m_colSegment, m_bounds))
				{
					retVal = QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Get_Segment *>(colQuery), 0, 0.0f, 1.0f);
				}

				// Restore old collision segment
				segQuery->m_colSegment = oldSegment;
			}

			break;

		case ruCOLQUERY_USER_FLAG_ENUMERATE:
			// TODO: Not verified
			return QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Enumerate *>(colQuery), 0);
	}

	return retVal;
}

// Compute the "value" of splitting the given set of faces, bounded by
// the given box, along the plane [axis]=offset.  A value of 0 means
// that a split is possible, but has no value.  A negative value means
// that the split is not valid at all.  Positive values indicate
// increasing goodness.
//
// *posOffset is computed based on the minimum coord of the faces
// that don't fit behind the negOffset.  Could be greater or less
// than negOffset.
//
// This is kinda heuristicy -- it's where the "special sauce" comes
// in.
REAL CRuCollisionStructure_kdTree::EvaluateSplit(const CRuVector3 *triVertices, const UINT16 *triIndices, const CRuArrayList<UINT16> &triFaces, INT32 depth, const CRuAABB &bounds, INT32 axis, REAL splitValue)
{
	// Count the faces that will end up in the groups
	// back,front.
	INT32 backCount = 0;
	INT32 frontCount = 0;

	INT32 numTriangles = triFaces.Count();

	for(INT32 i = 0; i < numTriangles; ++i)
	{
		INT32 result = ClassifyFace(triVertices, triIndices, triFaces, i, axis, splitValue);
		if(result == -1)
		{
			// Neg.
			backCount++;
		}
		else if(result == 0)
		{
			// Neg.
			backCount++;

			// Pos.
			frontCount++;
		}
		else
		{
			// Pos.
			frontCount++;
		}
	}

	// No faces are separated by this split; this split is entirely useless (not so! check to see if we are cutting off empty space!)
	if((backCount == 0 && splitValue <= bounds.Minima()[axis]) || (frontCount == 0 && splitValue >= bounds.Maxima()[axis]))
	{
		return -1;
	}

	REAL center = ((bounds.Minima() + bounds.Maxima()) / 2.0f)[axis];
	REAL extent = bounds.Maxima()[axis] - bounds.Minima()[axis];

	CRuAABB backBounds(bounds);
	backBounds.Maxima()[axis] = splitValue;

	CRuAABB	frontBounds(bounds);
	frontBounds.Minima()[axis] = splitValue;



	// MacDonald and Booth's metric, as quoted by Havran, endorsed by Ville Miettinen and Atman Binstock:

	REAL costBack = backBounds.GetSurfaceArea() * backCount;
	REAL costFront = frontBounds.GetSurfaceArea() * frontCount;

	REAL havranCost = costBack + costFront;

	REAL parentCost = bounds.GetSurfaceArea() * numTriangles;

	// We need to turn the cost into a quality, so subtract it from a
	// big number.
	return parentCost - havranCost;
}

INT32 CRuCollisionStructure_kdTree::BuildTree(INT32 nodeIndex, const CRuVector3 *triVertices, const UINT16 *triIndices, const CRuArrayList<UINT16> &triFaces, INT32 depth, const CRuAABB &bounds, INT32 maxTriPerLeaf, INT32 maxDepth, CRuArrayList<kdNode> &nodeListOut, CRuArrayList<kdLeafData> &leafDataListOut)
{
	INT32 treeSize = nodeIndex + 1;
	const INT32 maxSplitPlanesTested = 100;

	// If stop conditions are not met, subdivide node
	INT32 numTriangles = triFaces.Count();
	if(numTriangles > maxTriPerLeaf && depth < maxDepth)
	{
		// Find a good splitting plane
		REAL bestSplitQuality = 0.0f;
		INT32 bestSplitAxis = -1;
		REAL bestSplitVal = 0.0f;

		for(INT32 axis = 0; axis < 3; ++axis)
		{
			// Don't try to divide if the extent in current direction is degenerate
			if(bounds.Maxima()[axis] - bounds.Minima()[axis] < ruEPSILON)
			{
				continue;
			}

			// Try offsets that correspond to existing face boundaries
			INT32 stepSize = 1;
			if(numTriangles > maxSplitPlanesTested)
			{
				// For the sake of speed & sanity, only try the bounds of every N faces
				stepSize = numTriangles / maxSplitPlanesTested;
			}
			assert(stepSize > 0);

			REAL lastSplitValueTried = -FLT_MAX;
			REAL posOffset = 0;
			for(INT32 i = 0; i < numTriangles; i += stepSize)
			{
				REAL splitValue = max(max(triVertices[triIndices[triFaces[i] * 3]][axis], triVertices[triIndices[triFaces[i] * 3 + 1]][axis]), triVertices[triIndices[triFaces[i] * 3 + 2]][axis]);

				// Already tried this offset, skip
				if(fabsf(splitValue - lastSplitValueTried) < ruEPSILON)
				{
					continue;
				}

				lastSplitValueTried = splitValue;

				// How good is this split?
				REAL quality = EvaluateSplit(triVertices, triIndices, triFaces, depth, bounds, axis, splitValue);
				if(quality > bestSplitQuality)
				{
					// Best so far
					bestSplitQuality = quality;
					bestSplitAxis = axis;
					bestSplitVal = RuCastINT64ToFloat((RuCastFloatToINT64(splitValue) & 0xFFFFFFFC) | axis);
				}
			}
		}

		if(bestSplitAxis >= 0)
		{
			// Distribute triangles to each child
			INT32 childNodeIndex0 = nodeIndex * 2 + 1;
			INT32 childNodeIndex1 = nodeIndex * 2 + 2;
			CRuArrayList<UINT16> childTriFaces0;			// Negative half-space
			CRuArrayList<UINT16> childTriFaces1;			// Positive half-space

			for(INT32 i = 0; i < triFaces.Count(); ++i)
			{
				REAL d0 = triVertices[triIndices[triFaces[i] * 3]][bestSplitAxis] - bestSplitVal;
				REAL d1 = triVertices[triIndices[triFaces[i] * 3 + 1]][bestSplitAxis] - bestSplitVal;
				REAL d2 = triVertices[triIndices[triFaces[i] * 3 + 2]][bestSplitAxis] - bestSplitVal;

				// If the triangle straddles the negative half space
				if(d0 < 0.0f || d1 < 0.0f || d2 < 0.0f)
				{
					childTriFaces0.Add(triFaces[i]);
				}

				// If the triangle straddles the positive half space
				if(d0 >= 0.0f || d1 >= 0.0f || d2 >= 0.0f)
				{
					childTriFaces1.Add(triFaces[i]);
				}
			}

			CRuAABB childBounds0(bounds);
			childBounds0.Maxima()[bestSplitAxis] = bestSplitVal;

			CRuAABB	childBounds1(bounds);
			childBounds1.Minima()[bestSplitAxis] = bestSplitVal;

			INT32 treeSize0 = BuildTree(childNodeIndex0, triVertices, triIndices, childTriFaces0, depth + 1, childBounds0, maxTriPerLeaf, maxDepth, nodeListOut, leafDataListOut);
			INT32 treeSize1 = BuildTree(childNodeIndex1, triVertices, triIndices, childTriFaces1, depth + 1, childBounds1, maxTriPerLeaf, maxDepth, nodeListOut, leafDataListOut);

			// Setup kdNode data
			nodeListOut[nodeIndex].m_splitVal = bestSplitVal;

			return max(treeSize0, treeSize1);
		}
	}

	// Fill node data
	kdLeafData leafData;
	leafData.m_numTriangles = numTriangles;
	leafData.m_triFaces = ruNEW UINT16 [triFaces.Count()];
	for(INT32 i = 0; i < triFaces.Count(); ++i)
	{
		leafData.m_triFaces[i] = triFaces[i];
	}

	// Append new leaf data to leaf data list
	INT32 leafIndex = leafDataListOut.Count();
	leafDataListOut.Add(leafData);

	// Setup kdNode data
	nodeListOut[nodeIndex].m_leafIndex = (leafIndex << 2) | 0x00000003;

	return treeSize;
}

BOOL CRuCollisionStructure_kdTree::QueryCollision(CRuCollisionQuery_Ray *rayQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;
	const CRuVector3 *triVertices = m_mesh->GetMorphTarget(0)->GetPosition();
	const UINT16 *triIndices = m_mesh->GetIndices();

//	++m_nodesChecked;

	if(axis >= 3)
	{
		CRuVector3 intPt;
		REAL intT;

		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

//			++m_trisChecked;

			const CRuVector3 &v0 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & rayQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(rayQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & rayQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(FindIntersection(rayQuery->m_colRay, v0, v1, v2, intPt, intT))
			{
				if(rayQuery->m_ignoreBackface)
				{
					// Perform a backface check
					CRuVector3 e0(v1 - v0);
					CRuVector3 e1(v2 - v0);
					CRuVector3 n = CrossProduct(e0, e1);

					// Normal and ray direction must be opposite
					if(DotProduct(rayQuery->m_colRay.Direction(), n) < 0.0f)
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

		return TRUE;
	}

	INT32 nearNodeOffset = rayQuery->m_colRay.Origin()[axis] >= m_nodeArray[nodeIndex].m_splitVal;

	if(rayQuery->m_colRay.Direction()[axis] == 0.0f)
	{
		INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

		// Ray parallel to splitting plane, visit near side only
		QueryCollision(rayQuery, nearNodeIndex, rayMin, rayMax);
	}
	else
	{
		static const REAL m_epsilon_DropBranch = 1e-4f;

		// Find t value for intersection between ray and split plane
		REAL t = (m_nodeArray[nodeIndex].m_splitVal - rayQuery->m_colRay.Origin()[axis]) / rayQuery->m_colRay.Direction()[axis];

		// Test if ray straddles splitting plane
		if(t < 0.0f || t > rayMax + m_epsilon_DropBranch)
		{
			// Traverse near side
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

			QueryCollision(rayQuery, nearNodeIndex, rayMin, rayMax);
		}
		else if(t < rayMin - m_epsilon_DropBranch)
		{
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			QueryCollision(rayQuery, farNodeIndex, rayMin, rayMax);
		}
		else
		{
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			// Traverse near side first, then far side
			QueryCollision(rayQuery, nearNodeIndex, rayMin, t);
			QueryCollision(rayQuery, farNodeIndex, t, rayMax);
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_kdTree::QueryCollision(CRuCollisionQuery_Ray_Nearest *rayQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;
	const CRuVector3 *triVertices = m_mesh->GetMorphTarget(0)->GetPosition();
	const UINT16 *triIndices = m_mesh->GetIndices();

	if(axis >= 3)
	{
		CRuVector3 intPt;
		REAL intT;

		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

			const CRuVector3 &v0 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & rayQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(rayQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & rayQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(FindIntersection(rayQuery->m_colRay, v0, v1, v2, intPt, intT))
			{
				if(rayQuery->m_ignoreBackface)
				{
					// Perform a backface check
					CRuVector3 e0(v1 - v0);
					CRuVector3 e1(v2 - v0);
					CRuVector3 n = CrossProduct(e0, e1);

					// Normal and ray direction must be opposite
					if(DotProduct(rayQuery->m_colRay.Direction(), n) < 0.0f)
					{
						if(intT < rayQuery->m_collisionT)
						{
							rayQuery->m_positiveHit = TRUE;
							rayQuery->m_collisionT = intT;
							rayQuery->m_collisionPoint = intPt * *(rayQuery->m_worldTransform);
							rayQuery->m_collisionFlag = m_triFlags[m_leafArray[leafIndex].m_triFaces[i]];
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
						rayQuery->m_collisionFlag = m_triFlags[m_leafArray[leafIndex].m_triFaces[i]];
					}
				}
			}
		}

		return TRUE;
	}

	INT32 nearNodeOffset = rayQuery->m_colRay.Origin()[axis] >= m_nodeArray[nodeIndex].m_splitVal;

	if(rayQuery->m_colRay.Direction()[axis] == 0.0f)
	{
		INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

		// Ray parallel to splitting plane, visit near side only
		QueryCollision(rayQuery, nearNodeIndex, rayMin, rayMax);
	}
	else
	{
		static const REAL m_epsilon_DropBranch = 1e-4f;

		// Find t value for intersection between ray and split plane
		REAL t = (m_nodeArray[nodeIndex].m_splitVal - rayQuery->m_colRay.Origin()[axis]) / rayQuery->m_colRay.Direction()[axis];

		// Test if ray straddles splitting plane
		if(t < 0.0f || t > rayMax + m_epsilon_DropBranch)
		{
			// Traverse near side
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

			QueryCollision(rayQuery, nearNodeIndex, rayMin, rayMax);
		}
		else if(t < rayMin - m_epsilon_DropBranch)
		{
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			QueryCollision(rayQuery, farNodeIndex, rayMin, rayMax);
		}
		else
		{
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			// Traverse near side first, then far side
			QueryCollision(rayQuery, nearNodeIndex, rayMin, t);
			QueryCollision(rayQuery, farNodeIndex, t, rayMax);
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_kdTree::QueryCollision(CRuCollisionQuery_Segment *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;
	const CRuVector3 *triVertices = m_mesh->GetMorphTarget(0)->GetPosition();
	const UINT16 *triIndices = m_mesh->GetIndices();

//	++m_nodesChecked;

	if(axis >= 3)
	{
		CRuVector3 intPt;
		REAL intT;

		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

//			++m_trisChecked;

			const CRuVector3 &v0 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & segmentQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(segmentQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & segmentQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(FindIntersection(segmentQuery->m_colSegment, v0, v1, v2, intPt, intT))
			{
				if(segmentQuery->m_ignoreBackface)
				{
					// Perform a backface check
					CRuVector3 e0(v1 - v0);
					CRuVector3 e1(v2 - v0);
					CRuVector3 n = CrossProduct(e0, e1);

					// Normal and ray direction must be opposite
					if(DotProduct(segmentQuery->m_colSegment.Direction(), n) < 0.0f)
					{
						segmentQuery->m_colPoints->Add(intPt * *(segmentQuery->m_worldTransform));
						segmentQuery->m_colDistances->Add(intT);
					}
				}
				else
				{
					segmentQuery->m_colPoints->Add(intPt * *(segmentQuery->m_worldTransform));
					segmentQuery->m_colDistances->Add(intT);
				}
			}
		}

		return TRUE;
	}

	INT32 nearNodeOffset = segmentQuery->m_colSegment.Origin()[axis] >= m_nodeArray[nodeIndex].m_splitVal;

	if(segmentQuery->m_colSegment.Direction()[axis] == 0.0f)
	{
		INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

		// Ray parallel to splitting plane, visit near side only
		QueryCollision(segmentQuery, nearNodeIndex, rayMin, rayMax);
	}
	else
	{
		static const REAL m_epsilon_DropBranch = 1e-4f;

		// Find t value for intersection between ray and split plane
		REAL t = (m_nodeArray[nodeIndex].m_splitVal - segmentQuery->m_colSegment.Origin()[axis]) / segmentQuery->m_colSegment.Direction()[axis];

		// Test if ray straddles splitting plane
		if(t < 0.0f || t > rayMax + m_epsilon_DropBranch)
		{
			// Traverse near side
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

			QueryCollision(segmentQuery, nearNodeIndex, rayMin, rayMax);
		}
		else if(t < rayMin - m_epsilon_DropBranch)
		{
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			QueryCollision(segmentQuery, farNodeIndex, rayMin, rayMax);
		}
		else
		{
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			// Traverse near side first, then far side
			QueryCollision(segmentQuery, nearNodeIndex, rayMin, t);
			QueryCollision(segmentQuery, farNodeIndex, t, rayMax);
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_kdTree::QueryCollision(CRuCollisionQuery_Segment_HitTest *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;
	const CRuVector3 *triVertices = m_mesh->GetMorphTarget(0)->GetPosition();
	const UINT16 *triIndices = m_mesh->GetIndices();

//	++m_nodesChecked;

	if(axis >= 3)
	{
		CRuVector3 intPt;
		REAL intT;

		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

//			++m_trisChecked;

			const CRuVector3 &v0 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & segmentQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(segmentQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & segmentQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(FindIntersection(segmentQuery->m_colSegment, v0, v1, v2, intPt, intT))
			{
				if(segmentQuery->m_ignoreBackface)
				{
					// Perform a backface check
					CRuVector3 e0(v1 - v0);
					CRuVector3 e1(v2 - v0);
					CRuVector3 n = CrossProduct(e0, e1);

					// Normal and ray direction must be opposite
					if(DotProduct(segmentQuery->m_colSegment.Direction(), n) < 0.0f)
					{
						segmentQuery->m_positiveHit = TRUE;
						return TRUE;
					}
				}
				else
				{
					segmentQuery->m_positiveHit = TRUE;
					return TRUE;
				}
			}
		}

		return TRUE;
	}

	INT32 nearNodeOffset = segmentQuery->m_colSegment.Origin()[axis] >= m_nodeArray[nodeIndex].m_splitVal;

	if(segmentQuery->m_colSegment.Direction()[axis] == 0.0f)
	{
		INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

		// Ray parallel to splitting plane, visit near side only
		QueryCollision(segmentQuery, nearNodeIndex, rayMin, rayMax);
	}
	else
	{
		static const REAL m_epsilon_DropBranch = 1e-4f;

		// Find t value for intersection between ray and split plane
		REAL t = (m_nodeArray[nodeIndex].m_splitVal - segmentQuery->m_colSegment.Origin()[axis]) / segmentQuery->m_colSegment.Direction()[axis];

		// Test if ray straddles splitting plane
		if(t < 0.0f || t > rayMax + m_epsilon_DropBranch)
		{
			// Traverse near side
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

			QueryCollision(segmentQuery, nearNodeIndex, rayMin, rayMax);
		}
		else if(t < rayMin - m_epsilon_DropBranch)
		{
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			QueryCollision(segmentQuery, farNodeIndex, rayMin, rayMax);
		}
		else
		{
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			// Traverse near side first, then far side
			QueryCollision(segmentQuery, nearNodeIndex, rayMin, t);

			if(segmentQuery->m_positiveHit == FALSE)
			{
				QueryCollision(segmentQuery, farNodeIndex, t, rayMax);
			}
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_kdTree::QueryCollision(CRuCollisionQuery_Segment_Nearest *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;
	const CRuVector3 *triVertices = m_mesh->GetMorphTarget(0)->GetPosition();
	const UINT16 *triIndices = m_mesh->GetIndices();

//	++m_nodesChecked;

	if(axis >= 3)
	{
		CRuVector3 intPt;
		REAL intT;

		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

//			++m_trisChecked;

			const CRuVector3 &v0 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & segmentQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(segmentQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & segmentQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(FindIntersection(segmentQuery->m_colSegment, v0, v1, v2, intPt, intT))
			{
				CRuVector3 e0(v1 - v0);
				CRuVector3 e1(v2 - v0);
				CRuVector3 n = CrossProduct(e0, e1);

				// Normalize
				n.Normalize();

				// Perform a backface check
				if(segmentQuery->m_ignoreBackface)
				{
					// Normal and ray direction must be opposite
					if(DotProduct(segmentQuery->m_colSegment.Direction(), n) < 0.0f)
					{
						if(intT < segmentQuery->m_collisionT)
						{
							segmentQuery->m_positiveHit = TRUE;
							segmentQuery->m_collisionT = intT;
							segmentQuery->m_collisionPoint = intPt * *(segmentQuery->m_worldTransform);
							segmentQuery->m_worldTransform->TransformVector(n, segmentQuery->m_collisionNormal);
							segmentQuery->m_collisionFlag = m_triFlags[m_leafArray[leafIndex].m_triFaces[i]];
						}
					}
				}
				else
				{
					if(intT < segmentQuery->m_collisionT)
					{
						segmentQuery->m_positiveHit = TRUE;
						segmentQuery->m_collisionT = intT;
						segmentQuery->m_collisionPoint = intPt * *(segmentQuery->m_worldTransform);
						segmentQuery->m_worldTransform->TransformVector(n, segmentQuery->m_collisionNormal);
						segmentQuery->m_collisionFlag = m_triFlags[m_leafArray[leafIndex].m_triFaces[i]];
					}
				}
			}
		}

		return TRUE;
	}

	INT32 nearNodeOffset = segmentQuery->m_colSegment.Origin()[axis] >= m_nodeArray[nodeIndex].m_splitVal;

	if(segmentQuery->m_colSegment.Direction()[axis] == 0.0f)
	{
		INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

		// Ray parallel to splitting plane, visit near side only
		QueryCollision(segmentQuery, nearNodeIndex, rayMin, rayMax);
	}
	else
	{
		static const REAL m_epsilon_DropBranch = 1e-4f;

		// Find t value for intersection between ray and split plane
		REAL t = (m_nodeArray[nodeIndex].m_splitVal - segmentQuery->m_colSegment.Origin()[axis]) / segmentQuery->m_colSegment.Direction()[axis];

		// Test if ray straddles splitting plane
		if(t < 0.0f || t > rayMax + m_epsilon_DropBranch)
		{
			// Traverse near side
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

			QueryCollision(segmentQuery, nearNodeIndex, rayMin, rayMax);
		}
		else if(t < rayMin - m_epsilon_DropBranch)
		{
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			QueryCollision(segmentQuery, farNodeIndex, rayMin, rayMax);
		}
		else
		{
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			// Traverse near side first, then far side
			QueryCollision(segmentQuery, nearNodeIndex, rayMin, t);
			QueryCollision(segmentQuery, farNodeIndex, t, rayMax);
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_kdTree::QueryCollision(CRuCollisionQuery_OBB_HitTest *boxQuery, INT32 nodeIndex, CRuAABB &volBounds)
{
	BOOL flagSet = FALSE;
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;
	const CRuVector3 *triVertices = m_mesh->GetMorphTarget(0)->GetPosition();
	const UINT16 *triIndices = m_mesh->GetIndices();

	if(axis >= 3)
	{
		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

			const CRuVector3 &v0 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & boxQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(boxQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & boxQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			// If an intersection occurs, mark as positive hit and return immediately
			if(IntersectsTriangle(boxQuery->m_colOBB, v0, v1, v2))
			{
				boxQuery->m_positiveHit = TRUE;

				return TRUE;
			}
		}

		return TRUE;
	}

	// near node is one that is on the side of the box minima
	INT32 nearNodeIndex = nodeIndex * 2 + 1;
	INT32 farNodeIndex = nearNodeIndex + 1;

	CRuAABB nearBounds(volBounds);
	CRuAABB farBounds(volBounds);

	nearBounds.Maxima()[axis] = m_nodeArray[nodeIndex].m_splitVal;
	farBounds.Minima()[axis] = m_nodeArray[nodeIndex].m_splitVal;

	// Query on near side
	if(boxQuery->m_positiveHit == FALSE && Intersects(nearBounds, boxQuery->m_colOBB))
	{
		QueryCollision(boxQuery, nearNodeIndex, nearBounds);
	}

	// Query on far side
	if(boxQuery->m_positiveHit == FALSE && Intersects(farBounds, boxQuery->m_colOBB))
	{
		QueryCollision(boxQuery, farNodeIndex, farBounds);
	}

	return TRUE;
}

BOOL CRuCollisionStructure_kdTree::QueryCollision(CRuCollisionQuery_OBB_TriEnum *boxQuery, INT32 nodeIndex, CRuAABB &volBounds)
{
	BOOL flagSet = FALSE;
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;
	const CRuVector3 *triVertices = m_mesh->GetMorphTarget(0)->GetPosition();
	const UINT16 *triIndices = m_mesh->GetIndices();

	if(axis >= 3)
	{
		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

			const CRuVector3 &v0 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & boxQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(boxQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & boxQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			// If an intersection occurs, mark as positive hit and return immediately
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
					boxQuery->m_colTriFlags->Add(m_triFlags[m_leafArray[leafIndex].m_triFaces[i]]);
				}
			}
		}

		return TRUE;
	}

	// near node is one that is on the side of the box minima
	INT32 nearNodeIndex = nodeIndex * 2 + 1;
	INT32 farNodeIndex = nearNodeIndex + 1;

	CRuAABB nearBounds(volBounds);
	CRuAABB farBounds(volBounds);

	nearBounds.Maxima()[axis] = m_nodeArray[nodeIndex].m_splitVal;
	farBounds.Minima()[axis] = m_nodeArray[nodeIndex].m_splitVal;

	// Query on near side
	if(Intersects(nearBounds, boxQuery->m_colOBB))
	{
		QueryCollision(boxQuery, nearNodeIndex, nearBounds);
	}

	// Query on far side
	if(Intersects(farBounds, boxQuery->m_colOBB))
	{
		QueryCollision(boxQuery, farNodeIndex, farBounds);
	}

	return TRUE;
}

BOOL CRuCollisionStructure_kdTree::QueryCollision(CRuCollisionQuery_Sphere_HitTest *sphereQuery, INT32 nodeIndex, CRuVector3 &volNearPt)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;
	const CRuVector3 *triVertices = m_mesh->GetMorphTarget(0)->GetPosition();
	const UINT16 *triIndices = m_mesh->GetIndices();

	if(axis >= 3)
	{
		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

			const CRuVector3 &v0 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & sphereQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(sphereQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & sphereQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(IntersectsTriangle(sphereQuery->m_colSphere, v0, v1, v2))
			{
				sphereQuery->m_positiveHit = TRUE;
				return TRUE;
			}
		}

		return TRUE;
	}

	INT32 nearNodeOffset = sphereQuery->m_colSphere.Center()[axis] >= m_nodeArray[nodeIndex].m_splitVal;
	INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

	// Always visit near side child
	QueryCollision(sphereQuery, nearNodeIndex, volNearPt);

	// Update nearest point on volume when traversing far side.
	float oldValue = volNearPt[axis];
	volNearPt[axis] = m_nodeArray[nodeIndex].m_splitVal;

	// If the sphere overlaps the volume of the far node, recurse into it
	CRuVector3 diff = volNearPt - sphereQuery->m_colSphere.Center();
	if(diff.SquaredMagnitude() < sphereQuery->m_colSphere.Radius() * sphereQuery->m_colSphere.Radius())
	{
		INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);
		QueryCollision(sphereQuery, farNodeIndex, volNearPt);
	}

	// Restore component of nearest point on volume
	volNearPt[axis] = oldValue;

	return TRUE;
}

BOOL CRuCollisionStructure_kdTree::QueryCollision(CRuCollisionQuery_Sphere_TriEnum *sphereQuery, INT32 nodeIndex, CRuVector3 &volNearPt)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;
	const CRuVector3 *triVertices = m_mesh->GetMorphTarget(0)->GetPosition();
	const UINT16 *triIndices = m_mesh->GetIndices();

	if(axis >= 3)
	{
		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

			const CRuVector3 &v0 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & sphereQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(sphereQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & sphereQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(IntersectsTriangle(sphereQuery->m_colSphere, v0, v1, v2))
			{
				REAL colDepth = sphereQuery->m_colSphere.Radius() - RuSqrt(SquaredDistance(sphereQuery->m_colSphere.Center(), v0, v1, v2));

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
					sphereQuery->m_colTriFlags->Add(m_triFlags[m_leafArray[leafIndex].m_triFaces[i]]);
				}
			}
		}

		return TRUE;
	}

	INT32 nearNodeOffset = sphereQuery->m_colSphere.Center()[axis] >= m_nodeArray[nodeIndex].m_splitVal;
	INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

	// Always visit near side child
	QueryCollision(sphereQuery, nearNodeIndex, volNearPt);

	// Update nearest point on volume when traversing far side.
	float oldValue = volNearPt[axis];
	volNearPt[axis] = m_nodeArray[nodeIndex].m_splitVal;

	// If the sphere overlaps the volume of the far node, recurse into it
	CRuVector3 diff = volNearPt - sphereQuery->m_colSphere.Center();
	if(diff.SquaredMagnitude() < sphereQuery->m_colSphere.Radius() * sphereQuery->m_colSphere.Radius())
	{
		INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);
		QueryCollision(sphereQuery, farNodeIndex, volNearPt);
	}

	// Restore component of nearest point on volume
	volNearPt[axis] = oldValue;

	return TRUE;
}

BOOL CRuCollisionStructure_kdTree::QueryCollision(CRuCollisionQuery_Enumerate *enumQuery, INT32 nodeIndex)
{
	INT32 numVertices = m_mesh->GetNumVertices();
	const CRuVector3 *triVertices = m_mesh->GetMorphTarget(0)->GetPosition();
	const UINT16 *triIndices = m_mesh->GetIndices();

	for(INT32 i = 0, j = 0; j < m_numTriangles; i += 3, ++j)
	{
		// Check triangle's flags against the exclusion mask
		if((m_triFlags[j] & enumQuery->m_exclusionMask) != 0)
		{
			continue;
		}

		// Check triangle's flags against the inclusion mask
		if(enumQuery->m_inclusionMask && (m_triFlags[j] & enumQuery->m_inclusionMask) == 0)
		{
			continue;
		}

		enumQuery->m_colTriVerts->Add(triVertices[triIndices[i]]);
		enumQuery->m_colTriVerts->Add(triVertices[triIndices[i + 1]]);
		enumQuery->m_colTriVerts->Add(triVertices[triIndices[i + 2]]);
	}

	return TRUE;
}

BOOL CRuCollisionStructure_kdTree::QueryCollision(CRuCollisionQuery_User_Flag_Set *setQuery, INT32 nodeIndex)
{
	for(INT32 i = 0; i < m_numTriangles; ++i)
	{
		m_triFlags[i] = (m_triFlags[i] & ~setQuery->m_flagMask) | (setQuery->m_markingFlag & setQuery->m_flagMask);
	}

	setQuery->m_affectedObjects->Add(reinterpret_cast<void *>(this));

	return TRUE;
}

BOOL CRuCollisionStructure_kdTree::QueryCollision(CRuCollisionQuery_User_Flag_Set_Ray *setQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	BOOL flagSet = FALSE;
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;
	const CRuVector3 *triVertices = m_mesh->GetMorphTarget(0)->GetPosition();
	const UINT16 *triIndices = m_mesh->GetIndices();

//	++m_nodesChecked;

	if(axis >= 3)
	{
		CRuVector3 intPt;
		REAL intT;

		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
//			++m_trisChecked;

			const CRuVector3 &v0 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & setQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(setQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & setQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(FindIntersection(setQuery->m_markingRay, v0, v1, v2, intPt, intT))
			{
				// Set flag...
				flagSet = TRUE;
				m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] = (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & ~setQuery->m_flagMask) | (setQuery->m_markingFlag & setQuery->m_flagMask);
			}
		}

		if(flagSet)
		{
			for(INT32 i = 0; i < setQuery->m_affectedObjects->Count(); ++i)
			{
				if(setQuery->m_affectedObjects->Get(i) == reinterpret_cast<void *>(this))
				{
					return TRUE;
				}
			}

			setQuery->m_affectedObjects->Add(reinterpret_cast<void *>(this));
		}

		return TRUE;
	}

	INT32 nearNodeOffset = setQuery->m_markingRay.Origin()[axis] >= m_nodeArray[nodeIndex].m_splitVal;

	if(setQuery->m_markingRay.Direction()[axis] == 0.0f)
	{
		INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

		// Ray parallel to splitting plane, visit near side only
		QueryCollision(setQuery, nearNodeIndex, rayMin, rayMax);
	}
	else
	{
		static const REAL m_epsilon_DropBranch = 1e-4f;

		// Find t value for intersection between ray and split plane
		REAL t = (m_nodeArray[nodeIndex].m_splitVal - setQuery->m_markingRay.Origin()[axis]) / setQuery->m_markingRay.Direction()[axis];

		// Test if ray straddles splitting plane
		if(t < 0.0f || t > rayMax + m_epsilon_DropBranch)
		{
			// Traverse near side
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

			QueryCollision(setQuery, nearNodeIndex, rayMin, rayMax);
		}
		else if(t < rayMin - m_epsilon_DropBranch)
		{
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			QueryCollision(setQuery, farNodeIndex, rayMin, rayMax);
		}
		else
		{
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			// Traverse near side first, then far side
			QueryCollision(setQuery, nearNodeIndex, rayMin, t);
			QueryCollision(setQuery, farNodeIndex, t, rayMax);
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_kdTree::QueryCollision(CRuCollisionQuery_User_Flag_Set_Sphere *setQuery, INT32 nodeIndex, CRuVector3 &volNearPt)
{
	BOOL flagSet = FALSE;
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;
	const CRuVector3 *triVertices = m_mesh->GetMorphTarget(0)->GetPosition();
	const UINT16 *triIndices = m_mesh->GetIndices();

//	++m_nodesChecked;

	if(axis >= 3)
	{
		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
//			++m_trisChecked;

			const CRuVector3 &v0 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & setQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(setQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & setQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(IntersectsTriangle(setQuery->m_markingSphere, v0, v1, v2))
			{
				// Set flag...
				flagSet = TRUE;
				m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] = (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & ~setQuery->m_flagMask) | (setQuery->m_markingFlag & setQuery->m_flagMask);
			}
		}

		if(flagSet)
		{
			for(INT32 i = 0; i < setQuery->m_affectedObjects->Count(); ++i)
			{
				if(setQuery->m_affectedObjects->Get(i) == reinterpret_cast<void *>(this))
				{
					return TRUE;
				}
			}

			setQuery->m_affectedObjects->Add(reinterpret_cast<void *>(this));
		}

		return TRUE;
	}

	INT32 nearNodeOffset = setQuery->m_markingSphere.Center()[axis] >= m_nodeArray[nodeIndex].m_splitVal;
	INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

	// Always visit near side child
	QueryCollision(setQuery, nearNodeIndex, volNearPt);

	// Update nearest point on volume when traversing far side.
	float oldValue = volNearPt[axis];
	volNearPt[axis] = m_nodeArray[nodeIndex].m_splitVal;

	// If the sphere overlaps the volume of the far node, recurse into it
	CRuVector3 diff = volNearPt - setQuery->m_markingSphere.Center();
	if(diff.SquaredMagnitude() < setQuery->m_markingSphere.Radius() * setQuery->m_markingSphere.Radius())
	{
		INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);
		QueryCollision(setQuery, farNodeIndex, volNearPt);
	}

	// Restore component of nearest point on volume
	volNearPt[axis] = oldValue;

	return TRUE;
}

BOOL CRuCollisionStructure_kdTree::QueryCollision(CRuCollisionQuery_User_Flag_Get_Segment *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;
	const CRuVector3 *triVertices = m_mesh->GetMorphTarget(0)->GetPosition();
	const UINT16 *triIndices = m_mesh->GetIndices();

//	++m_nodesChecked;

	if(axis >= 3)
	{
		CRuVector3 intPt;
		REAL intT;

		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
//			++m_trisChecked;

			const CRuVector3 &v0 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = triVertices[triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			if(FindIntersection(segmentQuery->m_colSegment, v0, v1, v2, intPt, intT))
			{
				if(segmentQuery->m_ignoreBackface)
				{
					// Perform a backface check
					CRuVector3 e0(v1 - v0);
					CRuVector3 e1(v2 - v0);
					CRuVector3 n = CrossProduct(e0, e1);

					// Normal and ray direction must be opposite
					if(DotProduct(segmentQuery->m_colSegment.Direction(), n) < 0.0f)
					{
						if(intT < segmentQuery->m_collisionT)
						{
							segmentQuery->m_positiveHit = TRUE;
							segmentQuery->m_collisionT = intT;
							segmentQuery->m_collisionPoint = intPt * *(segmentQuery->m_worldTransform);
							segmentQuery->m_flag = m_triFlags[m_leafArray[leafIndex].m_triFaces[i]];
						}
					}
				}
				else
				{
					if(intT < segmentQuery->m_collisionT)
					{
						segmentQuery->m_positiveHit = TRUE;
						segmentQuery->m_collisionT = intT;
						segmentQuery->m_collisionPoint = intPt * *(segmentQuery->m_worldTransform);
						segmentQuery->m_flag = m_triFlags[m_leafArray[leafIndex].m_triFaces[i]];
					}
				}
			}
		}

		return TRUE;
	}

	INT32 nearNodeOffset = segmentQuery->m_colSegment.Origin()[axis] >= m_nodeArray[nodeIndex].m_splitVal;

	if(segmentQuery->m_colSegment.Direction()[axis] == 0.0f)
	{
		INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

		// Ray parallel to splitting plane, visit near side only
		QueryCollision(segmentQuery, nearNodeIndex, rayMin, rayMax);
	}
	else
	{
		static const REAL m_epsilon_DropBranch = 1e-4f;

		// Find t value for intersection between ray and split plane
		REAL t = (m_nodeArray[nodeIndex].m_splitVal - segmentQuery->m_colSegment.Origin()[axis]) / segmentQuery->m_colSegment.Direction()[axis];

		// Test if ray straddles splitting plane
		if(t < 0.0f || t > rayMax + m_epsilon_DropBranch)
		{
			// Traverse near side
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

			QueryCollision(segmentQuery, nearNodeIndex, rayMin, rayMax);
		}
		else if(t < rayMin - m_epsilon_DropBranch)
		{
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			QueryCollision(segmentQuery, farNodeIndex, rayMin, rayMax);
		}
		else
		{
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			// Traverse near side first, then far side
			QueryCollision(segmentQuery, nearNodeIndex, rayMin, t);

			if(segmentQuery->m_positiveHit == FALSE)
			{
				QueryCollision(segmentQuery, farNodeIndex, t, rayMax);
			}
		}
	}

	return TRUE;
}

BOOL CRuCollisionStructure_kdTree::QueryCollision(CRuCollisionQuery_User_Flag_Enumerate *enumQuery, INT32 nodeIndex)
{
	INT32 numVertices = m_mesh->GetNumVertices();
	const CRuVector3 *triVertices = m_mesh->GetMorphTarget(0)->GetPosition();
	const UINT16 *triIndices = m_mesh->GetIndices();

	// Iterate through all triangles
	for(INT32 i = 0, j = 0; i < m_numTriangles; ++i)
	{
		// If the triangle matches the exclusion mask, skip it
		if((m_triFlags[i] & enumQuery->m_exclusionMask) != 0x00000000)
		{
			j += 3;
			continue;
		}

		// Add vertices of the triangle to output list
		for(INT32 k = 0; k < 3; ++k, ++j)
		{
			enumQuery->m_colTriVerts->Add(triVertices[triIndices[j]]);
		}

		// Add triangle's flag to output list
		enumQuery->m_colTriFlags->Add(m_triFlags[i]);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
