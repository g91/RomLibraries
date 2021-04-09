#include "../../Engine/Geometry/RuPolySoup.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuPolySoup_Quadtree, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuPolySoup_Quadtree, "CRuPolySoup_Quadtree", "IRuObject")
ruCLASSGUID_DEFINE(CRuPolySoup_Quadtree, 0xDF0FC0EB, 0x7D62403f, 0xA358D0EE, 0x061AC180)

// ************************************************************************************************************************************************************

BOOL CRuPolySoup_Quadtree::QuadtreeNode::SerializeFrom(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuPolySoup_Quadtree::QuadtreeNode::SerializeTo(IRuStream *stream)
{
	if(IsLeaf())
	{
		for(INT32 i = 0; i < m_numFaces; ++i)
		{
			stream->Write(&m_faceIndices[i], sizeof(INT32));
		}
	}
	else
	{
		for(INT32 i = 0; i < 4; ++i)
		{
			m_children[i]->SerializeTo(stream);
		}
	}

	return TRUE;
}

void CRuPolySoup_Quadtree::QuadtreeNode::AddTriangle(const CRuVector3 *v, INT32 maxFacesPerLeaf, CRuArrayList<CRuVector3> &vertices, CRuArrayList<INT32> &indices)
{
	if(IsLeaf())
	{
		// If the node is not full, directly perform the addition
		if(m_numFaces < maxFacesPerLeaf)
		{
			INT32 triangleIndex = indices.Count() / 3;

			CRuVector3 oldCenter = (m_bounds.Minima() + m_bounds.Maxima()) / 2.0f;

			for(INT32 i = 0; i < 3; ++i)
			{
				INT32 vertIndex = -1;

				// Scan through the triangle list subset used by this node to find an identical vertex
				for(INT32 j = 0; j < m_numFaces && vertIndex < 0; ++j)
				{
					// Scan through each of the triangle's indices
					for(INT32 k = 0, l = m_faceIndices[j] * 3; k < 3; ++k, ++l)
					{
						// Is the vertex identical with the one at offset l?
						if(v[i] == vertices[indices[l]])
						{
							vertIndex = indices[l];
							break;
						}
					}
				}

				// No identical vertex was found -- create a new vertex
				if(vertIndex < 0)
				{
					vertIndex = vertices.Count();
					vertices.Add(v[i]);

					if(m_numFaces == 0 && i == 0)
					{
						// If there are no existing faces in the node and this is the first vertex, set bounds to the vertex
						m_bounds.Set(v[i].x, v[i].y, v[i].z, v[i].x, v[i].y, v[i].z);
					}
					else
					{
						// Vertex is not the first vertex in the node, merge it into the bounding box
						m_bounds.Merge(v[i]);
					}
				}

				// Add the triangle's vertex index
				indices.Add(vertIndex);
			}

			// If there are existing faces in the node, merge bounds
			if(m_numFaces > 0)
			{
				CRuVector3 newCenter = (m_bounds.Minima() + m_bounds.Maxima()) / 2.0f;
				CRuVector3 offset = newCenter - oldCenter;
				
				for(INT32 i = 0; i < 3; ++i)
				{
					if(offset[i] > 0.0f)
					{
						m_bounds.Minima()[i] -= offset[i];
					}
					else if(offset.x < 0.0f)
					{
						m_bounds.Maxima()[i] -= offset[i];
					}
				}
			}

			// Add triangle index to the node's triangle list
			INT32 *newFaceIndices = ruNEW INT32 [m_numFaces + 1];
			memcpy(newFaceIndices, m_faceIndices, sizeof(INT32) * m_numFaces);
			newFaceIndices[m_numFaces] = triangleIndex;
			delete[] m_faceIndices;
			m_faceIndices = newFaceIndices;
			++m_numFaces;

			return;
		}
		else
		{
			SplitNode(maxFacesPerLeaf, vertices, indices);
		}
	}

	INT32 childIndex = 0;
	float smallestExpansion = FLT_MAX;

	CRuVector3 triCenter = (v[0] + v[1] + v[2]) / 3.0f;
	for(INT32 i = 0; i < 4; ++i)
	{
		CRuVector3 childCenter = (m_children[i]->m_bounds.Minima() + m_children[i]->m_bounds.Maxima()) / 2.0f;

		float distance = (childCenter - triCenter).SquaredMagnitude();
		if(distance < smallestExpansion)
		{
			childIndex = i;
			smallestExpansion = distance;
		}
	}

	// Add triangle to the child that expands the last
	m_children[childIndex]->AddTriangle(v, maxFacesPerLeaf, vertices, indices);

	// Merge the child's bounds into the node's bounds
	m_bounds.Merge(m_children[childIndex]->m_bounds);
}

void CRuPolySoup_Quadtree::QuadtreeNode::AddTriangle(INT32 triangleIndex, INT32 maxFacesPerLeaf, CRuArrayList<CRuVector3> &vertices, CRuArrayList<INT32> &indices)
{
	if(IsLeaf())
	{
		// If the node is not full, directly perform the addition
		if(m_numFaces < maxFacesPerLeaf)
		{
			CRuVector3 oldCenter = (m_bounds.Minima() + m_bounds.Maxima()) / 2.0f;

			for(INT32 i = 0; i < 3; ++i)
			{
				if(m_numFaces == 0 && i == 0)
				{
					// If there are no existing faces in the node and this is the first vertex, set bounds to the vertex
					m_bounds.Set(vertices[indices[triangleIndex * 3 + i]].x, vertices[indices[triangleIndex * 3 + i]].y, vertices[indices[triangleIndex * 3 + i]].z, vertices[indices[triangleIndex * 3 + i]].x, vertices[indices[triangleIndex * 3 + i]].y, vertices[indices[triangleIndex * 3 + i]].z);
				}
				else
				{
					// Vertex is not the first vertex in the node, merge it into the bounding box
					m_bounds.Merge(vertices[indices[triangleIndex * 3 + i]]);
				}
			}

			// If there are existing faces in the node, merge bounds
			if(m_numFaces > 0)
			{
				CRuVector3 newCenter = (m_bounds.Minima() + m_bounds.Maxima()) / 2.0f;
				CRuVector3 offset = newCenter - oldCenter;

				for(INT32 i = 0; i < 3; ++i)
				{
					if(offset[i] > 0.0f)
					{
						m_bounds.Minima()[i] -= offset[i];
					}
					else if(offset.x < 0.0f)
					{
						m_bounds.Maxima()[i] -= offset[i];
					}
				}
			}

			// Add triangle index to the node's triangle list
			INT32 *newFaceIndices = ruNEW INT32 [m_numFaces + 1];
			memcpy(newFaceIndices, m_faceIndices, sizeof(INT32) * m_numFaces);
			newFaceIndices[m_numFaces] = triangleIndex;
			delete[] m_faceIndices;
			m_faceIndices = newFaceIndices;
			++m_numFaces;

			return;
		}
		else
		{
			SplitNode(maxFacesPerLeaf, vertices, indices);
		}
	}

	INT32 childIndex = 0;
	float smallestExpansion = FLT_MAX;

	// Iterate through children and find the one with the smallest volume expansion after adding the triangle
	CRuVector3 triCenter = (vertices[indices[triangleIndex * 3]] + vertices[indices[triangleIndex * 3 + 1]] + vertices[indices[triangleIndex * 3 + 2]]) / 3.0f;
	for(INT32 i = 0; i < 4; ++i)
	{
		CRuVector3 childCenter = (m_children[i]->m_bounds.Minima() + m_children[i]->m_bounds.Maxima()) / 2.0f;

		float distance = (childCenter - triCenter).SquaredMagnitude();
		if(distance < smallestExpansion)
		{
			childIndex = i;
			smallestExpansion = distance;
		}
	}

	// Add triangle to the child that expands the last
	m_children[childIndex]->AddTriangle(triangleIndex, maxFacesPerLeaf, vertices, indices);

	// Merge the child's bounds into the node's bounds
	m_bounds.Merge(m_children[childIndex]->m_bounds);
}

void CRuPolySoup_Quadtree::QuadtreeNode::RemoveTriangle(const CRuRay3 &removalRay, CRuArrayList<CRuVector3> &vertices, CRuArrayList<INT32> &indices)
{
	// Test ray with bounding box
	if(Intersects(removalRay, m_bounds))
	{
		if(IsLeaf())
		{
			// Test with every triangle
			CRuVector3 intPt;
			float intT;

			for(INT32 i = 0; i < m_numFaces; ++i)
			{
				INT32 faceIndices[3] = { indices[m_faceIndices[i] * 3], indices[m_faceIndices[i] * 3 + 1], indices[m_faceIndices[i] * 3 + 2] };

				if(FindIntersection(removalRay, vertices[faceIndices[0]], vertices[faceIndices[1]], vertices[faceIndices[2]], intPt, intT))
				{
					// Remove face
					memmove(&m_faceIndices[i], &m_faceIndices[i + 1], sizeof(INT32) * (m_numFaces - i - 1));

					--i;
					--m_numFaces;
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				m_children[i]->RemoveTriangle(removalRay, vertices, indices);
			}
		}
	}
}

void CRuPolySoup_Quadtree::QuadtreeNode::RemoveTriangle(const CRuAABB &removalAABB, CRuArrayList<CRuVector3> &vertices, CRuArrayList<INT32> &indices)
{
	// Test ray with bounding box
	if(Intersects(removalAABB, m_bounds))
	{
		if(IsLeaf())
		{
			// Test with every triangle
			for(INT32 i = 0; i < m_numFaces; ++i)
			{
				INT32 faceIndices[3] = { indices[m_faceIndices[i] * 3], indices[m_faceIndices[i] * 3 + 1], indices[m_faceIndices[i] * 3 + 2] };

				if(IntersectsTriangle(removalAABB, vertices[faceIndices[0]], vertices[faceIndices[1]], vertices[faceIndices[2]]))
				{
					// Remove face
					memmove(&m_faceIndices[i], &m_faceIndices[i + 1], sizeof(INT32) * (m_numFaces - i - 1));

					--i;
					--m_numFaces;
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				m_children[i]->RemoveTriangle(removalAABB, vertices, indices);
			}
		}
	}
}

INT32 CRuPolySoup_Quadtree::QuadtreeNode::GetNumTriangles()
{
	if(IsLeaf())
	{
		return m_numFaces;
	}

	return m_children[0]->GetNumTriangles() + m_children[1]->GetNumTriangles() + m_children[2]->GetNumTriangles() + m_children[3]->GetNumTriangles();
}

void CRuPolySoup_Quadtree::QuadtreeNode::TranslatePolySoup(const CRuVector3 &translation)
{
	m_bounds.Minima() += translation;
	m_bounds.Maxima() += translation;

	if(!IsLeaf())
	{
		// Recurse to children
		for(INT32 i = 0; i < 4; ++i)
		{
			m_children[i]->TranslatePolySoup(translation);
		}
	}
}

void CRuPolySoup_Quadtree::QuadtreeNode::GetUniquePartitions(INT32 trianglesPerPartition, BOOL divideMarker, CRuArrayList<INT32> &faceCountList, CRuArrayList<INT32 *> &faceIndexList)
{
	if(IsLeaf())
	{
		if(m_numFaces > 0)
		{
			faceCountList.Add(m_numFaces);
			faceIndexList.Add(m_faceIndices);

			if(divideMarker == FALSE)
			{
				// Insert divide marker
				faceCountList.Add(-1);
			}
		}
	}
	else
	{
		if(divideMarker == FALSE && GetNumTriangles() <= trianglesPerPartition)
		{
			divideMarker = TRUE;

			for(INT32 i = 0; i < 4; ++i)
			{
				m_children[i]->GetUniquePartitions(trianglesPerPartition, divideMarker, faceCountList, faceIndexList);
			}

			// Insert divide marker
			faceCountList.Add(-1);
		}
		else
		{
			for(INT32 i = 0; i < 4; ++i)
			{
				m_children[i]->GetUniquePartitions(trianglesPerPartition, divideMarker, faceCountList, faceIndexList);
			}
		}
	}
}

BOOL CRuPolySoup_Quadtree::QuadtreeNode::QueryCollision(CRuCollisionQuery *colQuery, CRuArrayList<CRuVector3> &vertices, CRuArrayList<INT32> &indices)
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
					if(IsLeaf())
					{
						// Test with every triangle
						CRuVector3 intPt;
						float intT;

						for(INT32 i = 0; i < m_numFaces; ++i)
						{
							INT32 faceIndices[3] = { indices[m_faceIndices[i] * 3], indices[m_faceIndices[i] * 3 + 1], indices[m_faceIndices[i] * 3 + 2] };

							if(FindIntersection(localRay, vertices[faceIndices[0]], vertices[faceIndices[1]], vertices[faceIndices[2]], intPt, intT))
							{
								if(rayQuery->m_ignoreBackface)
								{
									// Perform a backface check
									CRuVector3 e0(vertices[faceIndices[1]] - vertices[faceIndices[0]]);
									CRuVector3 e1(vertices[faceIndices[2]] - vertices[faceIndices[0]]);
									CRuVector3 n = CrossProduct(e0, e1);

									// Normal and ray direction must be opposite
									if(DotProduct(rayQuery->m_colRay.Direction(), n) < 0.0f)
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
					else
					{
						for(INT32 i = 0; i < 4; ++i)
						{
							if(m_children[i]->QueryCollision(colQuery, vertices, indices) == FALSE)
							{
								return FALSE;
							}
						}
					}
				}

				return TRUE;
			}

			break;

		case ruCOLQUERY_RAY_NEAREST:
			{
				CRuCollisionQuery_Ray_Nearest *rayQuery = static_cast<CRuCollisionQuery_Ray_Nearest *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 localRay(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));

				// Test ray with bounding box
				if(Intersects(localRay, m_bounds))
				{
					if(IsLeaf())
					{
						// Test with every triangle
						CRuVector3 intPt;
						float intT;

						for(INT32 i = 0; i < m_numFaces; ++i)
						{
							INT32 faceIndices[3] = { indices[m_faceIndices[i] * 3], indices[m_faceIndices[i] * 3 + 1], indices[m_faceIndices[i] * 3 + 2] };

							if(FindIntersection(localRay, vertices[faceIndices[0]], vertices[faceIndices[1]], vertices[faceIndices[2]], intPt, intT))
							{
								if(rayQuery->m_ignoreBackface)
								{
									// Perform a backface check
									CRuVector3 e0(vertices[faceIndices[1]] - vertices[faceIndices[0]]);
									CRuVector3 e1(vertices[faceIndices[2]] - vertices[faceIndices[0]]);
									CRuVector3 n = CrossProduct(e0, e1);

									// Normal and ray direction must be opposite
									if(DotProduct(rayQuery->m_colRay.Direction(), n) < 0.0f)
									{
										if(intT < rayQuery->m_collisionT)
										{
											rayQuery->m_positiveHit = TRUE;
											rayQuery->m_collisionT = intT;
											rayQuery->m_collisionPoint = intPt;
											rayQuery->m_collisionFlag = 0;
										}
									}
								}
								else
								{
									if(intT < rayQuery->m_collisionT)
									{
										rayQuery->m_positiveHit = TRUE;
										rayQuery->m_collisionT = intT;
										rayQuery->m_collisionPoint = intPt;
										rayQuery->m_collisionFlag = 0;
									}
								}
							}
						}
					}
					else
					{
						for(INT32 i = 0; i < 4; ++i)
						{
							if(m_children[i]->QueryCollision(colQuery, vertices, indices) == FALSE)
							{
								return FALSE;
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

void CRuPolySoup_Quadtree::QuadtreeNode::SplitNode(INT32 maxFacesPerLeaf, CRuArrayList<CRuVector3> &vertices, CRuArrayList<INT32> &indices)
{
	// Make sure the node is a leaf
	if(IsLeaf())
	{
		// Create children for the node
		for(INT32 i = 0; i < 4; ++i)
		{
			m_children[i] = ruNEW QuadtreeNode();
			m_children[i]->m_bounds.SetBoundsToQuadrantXZ(m_bounds, i);
		}

		// Distribute triangles to children
		for(INT32 i = 0; i < m_numFaces; ++i)
		{
			AddTriangle(m_faceIndices[i], maxFacesPerLeaf, vertices, indices);
		}

		// Delete triangles at this node
		m_numFaces = 0;
		delete[] m_faceIndices;
		m_faceIndices = NULL;
	}
}

CRuPolySoup_Quadtree::CRuPolySoup_Quadtree()
:	m_maxFacesPerLeaf(50),
	m_rootNode(NULL)
{
	m_rootNode = ruNEW QuadtreeNode();
}

CRuPolySoup_Quadtree::~CRuPolySoup_Quadtree()
{
	delete m_rootNode;
}

IRuObject *CRuPolySoup_Quadtree::Clone(IRuObject *clonedObject)
{
	return NULL;
}

BOOL CRuPolySoup_Quadtree::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read max faces per leaf
	stream->Read(&m_maxFacesPerLeaf, sizeof(INT32));

	// Read number of vertices
	INT32 vertexCount;
	stream->Read(&vertexCount, sizeof(INT32));

	// Read vertices
	CRuVector3 vertex;
	for(INT32 i = 0; i < vertexCount; ++i)
	{
		stream->Read(&vertex.x, sizeof(float));
		stream->Read(&vertex.y, sizeof(float));
		stream->Read(&vertex.z, sizeof(float));

		m_vertices.Add(vertex);
	}

	// Read number of indices
	INT32 indexCount;
	stream->Read(&indexCount, sizeof(INT32));

	// Read indices
	INT32 index;
	for(INT32 i = 0; i < indexCount; ++i)
	{
		stream->Read(&index, sizeof(INT32));

		m_indices.Add(index);
	}

	// Read number of triangles
	INT32 numTriangles;
	stream->Read(&numTriangles, sizeof(INT32));

	// Create root node
	delete m_rootNode;
	m_rootNode = ruNEW QuadtreeNode();

	for(INT32 i = 0; i < numTriangles; ++i)
	{
		INT32 faceIndex;
		stream->Read(&faceIndex, sizeof(INT32));
		m_rootNode->AddTriangle(faceIndex, m_maxFacesPerLeaf, m_vertices, m_indices);
	}

	return TRUE;
}

BOOL CRuPolySoup_Quadtree::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write max faces per leaf
	stream->Write(&m_maxFacesPerLeaf, sizeof(INT32));

	// Write number of vertices
	INT32 vertexCount = m_vertices.Count();
	stream->Write(&vertexCount, sizeof(INT32));

	// Write vertices
	for(INT32 i = 0; i < m_vertices.Count(); ++i)
	{
		stream->Write(&m_vertices[i].x, sizeof(float));
		stream->Write(&m_vertices[i].y, sizeof(float));
		stream->Write(&m_vertices[i].z, sizeof(float));
	}

	// Write number of indices
	INT32 indexCount = m_indices.Count();
	stream->Write(&indexCount, sizeof(INT32));

	// Write indices
	stream->Write(m_indices.GetArray(), sizeof(INT32) * m_indices.Count());

	// Write number of triangles
	INT32 numTriangles = m_rootNode->GetNumTriangles();
	stream->Write(&numTriangles, sizeof(INT32));

	// Allow each node to write its own triangle list
	m_rootNode->SerializeTo(stream);

	return TRUE;
}

BOOL CRuPolySoup_Quadtree::AddTriangle(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2)
{
	CRuVector3 v[3] = { v0, v1, v2 };
	m_rootNode->AddTriangle(v, m_maxFacesPerLeaf, m_vertices, m_indices);

	return TRUE;
}

BOOL CRuPolySoup_Quadtree::RemoveTriangle(const CRuRay3 &removalRay)
{
	m_rootNode->RemoveTriangle(removalRay, m_vertices, m_indices);

	return TRUE;
}

BOOL CRuPolySoup_Quadtree::RemoveTriangle(const CRuAABB &removalAABB)
{
	m_rootNode->RemoveTriangle(removalAABB, m_vertices, m_indices);

	return TRUE;
}

BOOL CRuPolySoup_Quadtree::TranslatePolySoup(const CRuVector3 &translation)
{
	// Translate all vertices
	for(INT32 i = 0; i < m_vertices.Count(); ++i)
	{
		m_vertices[i] += translation;
	}

	// Translate node bounds
	m_rootNode->TranslatePolySoup(translation);

	return TRUE;
}

const CRuAABB &CRuPolySoup_Quadtree::GetBounds() const
{
	return m_rootNode->m_bounds;
}

INT32 CRuPolySoup_Quadtree::GetNumTriangles() const
{
	return m_rootNode->GetNumTriangles();
}

void CRuPolySoup_Quadtree::GetUniquePartitions(INT32 trianglesPerPartition, CRuArrayList<INT32> &faceCountList, CRuArrayList<INT32 *> &faceIndexList)
{
	m_rootNode->GetUniquePartitions(trianglesPerPartition, FALSE, faceCountList, faceIndexList);
}

BOOL CRuPolySoup_Quadtree::QueryCollision(CRuCollisionQuery *colQuery)
{
	if(m_rootNode)
	{
		return m_rootNode->QueryCollision(colQuery, m_vertices, m_indices);
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed
