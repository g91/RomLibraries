#include "../Collision/RuCollision_kdTree.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuCollision_kdTree, IRuCollisionObject)
ruHEAP_DEFINE_SUBCLASS(CRuCollision_kdTree, "CRuCollision_kdTree", "IRuCollisionObject")
ruCLASSGUID_DEFINE(CRuCollision_kdTree, 0xC89C8348, 0x561D4e82, 0x931D765F, 0x01566B8E)

// ************************************************************************************************************************************************************

CRuCollision_kdTree::CRuCollision_kdTree()
:	m_numNodes(0),
	m_numLeaves(0),
	m_triVertices(NULL),
	m_triFlags(NULL),
	m_nodeArray(NULL),
	m_leafArray(NULL)
{
}

CRuCollision_kdTree::~CRuCollision_kdTree()
{
	// Clear shit in arrays
	for(INT32 i = 0; i < m_numLeaves; ++i)
	{
		delete[] m_leafArray[i].m_triFaces;
	}

	// Delete arrays
	delete[] m_triVertices;
	delete[] m_triFlags;
	delete[] m_nodeArray;
	delete[] m_leafArray;
}

void CRuCollision_kdTree::BuildTree(const CRuArrayList<CRuVector3> &triVertices, const CRuArrayList<UINT32> &triFlags, INT32 maxTriPerLeaf, INT32 maxDepth)
{
	CRuArrayList<INT32> triIndices;

	for(INT32 j = 0; j < triVertices.Count(); ++j)
	{
		triIndices.Add(j);
	}

	// Determine maximum number of nodes
	INT32 maxNodes = 0;
	for(INT32 i = 0; i < maxDepth; ++i)
	{
		maxNodes += (INT32) pow(2.0f, i);
	}

	CRuArrayList<kdNode> nodes(maxNodes);
	CRuArrayList<kdLeafData> leaves;

	kdNode node;
	for(INT32 i = 0; i < maxNodes; ++i)
	{
		nodes.Add(node);
	}

	// Allocate array to store triangle vertices
	m_numVertices = triVertices.Count();
	m_triVertices = ruNEW CRuVector3 [m_numVertices];

	// Compute bounds while copying the entire vertex array
	m_bounds.Set(triVertices[0].x, triVertices[0].y, triVertices[0].z, triVertices[0].x, triVertices[0].y, triVertices[0].z);
	for(INT32 i = 0; i < triVertices.Count(); ++i)
	{
		m_bounds.Merge(triVertices[i]);
		m_triVertices[i] = triVertices[i];
	}

	INT32 numTriangles = m_numVertices / 3;
	m_triFlags = ruNEW INT32 [numTriangles];
	CRuArrayList<INT32> triFaces(numTriangles);

	if(triVertices.Count() / 3 == triFlags.Count())
	{
		for(INT32 i = 0; i < numTriangles; ++i)
		{
			triFaces.Add(i);
			m_triFlags[i] = triFlags[i];
		}
	}
	else
	{
		for(INT32 i = 0; i < numTriangles; ++i)
		{
			triFaces.Add(i);
			m_triFlags[i] = 0;
		}
	}

	// Drop degenerate triangles

	// Build tree
	INT32 nodesUsed = BuildTree(0, m_triVertices, triFaces, 1, m_bounds, maxTriPerLeaf, maxDepth, nodes, leaves);

	// Copy crap
	m_numNodes = nodesUsed;
	m_numLeaves = leaves.Count();
	m_nodeArray = ruNEW kdNode [m_numNodes];
	m_leafArray = ruNEW kdLeafData [m_numLeaves];

	memcpy(m_nodeArray, nodes.GetArray(), sizeof(kdNode) * m_numNodes);
	memcpy(m_leafArray, leaves.GetArray(), sizeof(kdLeafData) * m_numLeaves);
}

BOOL CRuCollision_kdTree::SerializeFrom(IRuStream *stream)
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
	stream->Read(&m_numVertices, sizeof(INT32));

	// Read number of nodes
	stream->Read(&m_numNodes, sizeof(INT32));

	// Read number of leaves
	stream->Read(&m_numLeaves, sizeof(INT32));

	// Read triangle array
	m_triVertices = ruNEW CRuVector3 [m_numVertices];
	for(INT32 i = 0; i < m_numVertices; ++i)
	{
		stream->Read(&m_triVertices[i].x, sizeof(float));
		stream->Read(&m_triVertices[i].y, sizeof(float));
		stream->Read(&m_triVertices[i].z, sizeof(float));
	}

	// Read flags array
	INT32 numTriangles = m_numVertices / 3;
	m_triFlags = ruNEW INT32 [numTriangles];
	for(INT32 i = 0; i < numTriangles; ++i)
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
		m_leafArray[i].m_triFaces = ruNEW INT32 [m_leafArray[i].m_numTriangles];
		for(INT32 j = 0; j < m_leafArray[i].m_numTriangles; ++j)
		{
			stream->Read(&m_leafArray[i].m_triFaces[j], sizeof(INT32));
		}
	}


	return TRUE;
}

BOOL CRuCollision_kdTree::SerializeTo(IRuStream *stream)
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
	stream->Write(&m_numVertices, sizeof(INT32));

	// Write number of nodes
	stream->Write(&m_numNodes, sizeof(INT32));

	// Write number of leaves
	stream->Write(&m_numLeaves, sizeof(INT32));

	// Write triangle array
	for(INT32 i = 0; i < m_numVertices; ++i)
	{
		stream->Write(&m_triVertices[i].x, sizeof(float));
		stream->Write(&m_triVertices[i].y, sizeof(float));
		stream->Write(&m_triVertices[i].z, sizeof(float));
	}

	// Write flags array
	INT32 numTriangles = m_numVertices / 3;
	stream->Write(m_triFlags, sizeof(INT32) * numTriangles);

	// Write nodes
	stream->Write(m_nodeArray, sizeof(kdNode) * m_numNodes);

	// Write leaves
	for(INT32 i = 0; i < m_numLeaves; ++i)
	{
		// Write number of triangles
		stream->Write(&m_leafArray[i].m_numTriangles, sizeof(INT32));

		// Write triangle face indices
		stream->Write(m_leafArray[i].m_triFaces, sizeof(INT32) * m_leafArray[i].m_numTriangles);
	}

	return TRUE;
}

const CRuAABB &CRuCollision_kdTree::GetBounds() const
{
	return m_bounds;
}

BOOL CRuCollision_kdTree::QueryCollision(CRuCollisionQuery *colQuery)
{
	m_nodesChecked = 0;
	m_trisChecked = 0;

	switch(colQuery->m_queryType)
	{
		case ruCOLQUERY_RAY:
			{
			}

			break;

		case ruCOLQUERY_RAY_NEAREST:
			{
				if(Intersects(static_cast<CRuCollisionQuery_Ray_Nearest *>(colQuery)->m_colRay, m_bounds))
				{
					return QueryCollision(static_cast<CRuCollisionQuery_Ray_Nearest *>(colQuery), 0, 0.0f, FLT_MAX);
				}
			}

			break;

		case ruCOLQUERY_RAY_TRIPICK:
			{
			}

			break;

		case ruCOLQUERY_SEGMENT:
			{
				if(Intersects(static_cast<CRuCollisionQuery_Segment *>(colQuery)->m_colSegment, m_bounds))
				{
					return QueryCollision(static_cast<CRuCollisionQuery_Segment *>(colQuery), 0, 0.0f, 1.0f);
				}
			}

			break;

		case ruCOLQUERY_SEGMENT_HITTEST:
			{
				if(Intersects(static_cast<CRuCollisionQuery_Segment_HitTest *>(colQuery)->m_colSegment, m_bounds))
				{
					return QueryCollision(static_cast<CRuCollisionQuery_Segment_HitTest *>(colQuery), 0, 0.0f, 1.0f);
				}
			}

			break;

		case ruCOLQUERY_SEGMENT_NEAREST:
			{
				if(Intersects(static_cast<CRuCollisionQuery_Segment_Nearest *>(colQuery)->m_colSegment, m_bounds))
				{
					return QueryCollision(static_cast<CRuCollisionQuery_Segment_Nearest *>(colQuery), 0, 0.0f, 1.0f);
				}
			}

			break;

		case ruCOLQUERY_ENUMERATE:
			return QueryCollision(static_cast<CRuCollisionQuery_Enumerate *>(colQuery), 0);

		case ruCOLQUERY_USER_FLAG_SET_RAY:
			{
				if(Intersects(static_cast<CRuCollisionQuery_User_Flag_Set_Ray *>(colQuery)->m_markingRay, m_bounds))
				{
					return QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Set_Ray *>(colQuery), 0, 0.0f, FLT_MAX);
				}
			}

			break;

		case ruCOLQUERY_USER_FLAG_SET_SPHERE:
			{
				if(Intersects(m_bounds, static_cast<CRuCollisionQuery_User_Flag_Set_Sphere *>(colQuery)->m_markingSphere))
				{
					CRuVector3 volNearPt = static_cast<CRuCollisionQuery_User_Flag_Set_Sphere *>(colQuery)->m_markingSphere.Center();
					return QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Set_Sphere *>(colQuery), 0, volNearPt);
				}
			}

			break;

		case ruCOLQUERY_USER_FLAG_GET_SEGMENT:
			{
				if(Intersects(static_cast<CRuCollisionQuery_User_Flag_Get_Segment *>(colQuery)->m_colSegment, m_bounds))
				{
					return QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Get_Segment *>(colQuery), 0, 0.0f, FLT_MAX);
				}
			}

			break;

		case ruCOLQUERY_USER_FLAG_ENUMERATE:
			return QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Enumerate *>(colQuery), 0);
	}

	return TRUE;
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
REAL CRuCollision_kdTree::EvaluateSplit(const CRuVector3 *triVertices, const CRuArrayList<INT32> &triFaces, INT32 depth, const CRuAABB &bounds, INT32 axis, REAL splitValue)
{
	// Count the faces that will end up in the groups
	// back,front.
	INT32 backCount = 0;
	INT32 frontCount = 0;

	INT32 numTriangles = triFaces.Count();

	for(INT32 i = 0; i < numTriangles; ++i)
	{
		INT32 result = ClassifyFace(triVertices, triFaces, i, axis, splitValue);
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

INT32 CRuCollision_kdTree::BuildTree(INT32 nodeIndex, const CRuVector3 *triVertices, const CRuArrayList<INT32> &triFaces, INT32 depth, const CRuAABB &bounds, INT32 maxTriPerLeaf, INT32 maxDepth, CRuArrayList<kdNode> &nodeListOut, CRuArrayList<kdLeafData> &leafDataListOut)
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
				REAL splitValue = max(max(triVertices[triFaces[i] * 3][axis], triVertices[triFaces[i] * 3 + 1][axis]), triVertices[triFaces[i] * 3 + 2][axis]);

				// Already tried this offset, skip
				if(fabsf(splitValue - lastSplitValueTried) < ruEPSILON)
				{
					continue;
				}

				lastSplitValueTried = splitValue;

				// How good is this split?
				REAL quality = EvaluateSplit(triVertices, triFaces, depth, bounds, axis, splitValue);
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
			CRuArrayList<INT32> childTriFaces0;			// Negative half-space
			CRuArrayList<INT32> childTriFaces1;			// Positive half-space

			for(INT32 i = 0; i < triFaces.Count(); ++i)
			{
				REAL d0 = triVertices[triFaces[i] * 3][bestSplitAxis] - bestSplitVal;
				REAL d1 = triVertices[triFaces[i] * 3 + 1][bestSplitAxis] - bestSplitVal;
				REAL d2 = triVertices[triFaces[i] * 3 + 2][bestSplitAxis] - bestSplitVal;

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

			INT32 treeSize0 = BuildTree(childNodeIndex0, triVertices, childTriFaces0, depth + 1, childBounds0, maxTriPerLeaf, maxDepth, nodeListOut, leafDataListOut);
			INT32 treeSize1 = BuildTree(childNodeIndex1, triVertices, childTriFaces1, depth + 1, childBounds1, maxTriPerLeaf, maxDepth, nodeListOut, leafDataListOut);

			// Setup kdNode data
			nodeListOut[nodeIndex].m_splitVal = bestSplitVal;

			return max(treeSize0, treeSize1);
		}
	}

	// Fill node data
	kdLeafData leafData;
	leafData.m_numTriangles = numTriangles;
	leafData.m_triFaces = ruNEW INT32 [triFaces.Count()];
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

BOOL CRuCollision_kdTree::QueryCollision(CRuCollisionQuery_Ray *rayQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

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

			const CRuVector3 &v0 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3];
			const CRuVector3 &v1 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1];
			const CRuVector3 &v2 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & rayQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(rayQuery->m_inclusionMask && rayQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & rayQuery->m_inclusionMask) == 0)
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
						rayQuery->m_colPoints->Add(intPt);
						rayQuery->m_colDistances->Add(intT);
					}
				}
				else
				{
					rayQuery->m_colPoints->Add(intPt);
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

BOOL CRuCollision_kdTree::QueryCollision(CRuCollisionQuery_Ray_Nearest *rayQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

	if(axis >= 3)
	{
		CRuVector3 intPt;
		REAL intT;

		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			const CRuVector3 &v0 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3];
			const CRuVector3 &v1 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1];
			const CRuVector3 &v2 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2];

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
							rayQuery->m_collisionPoint = intPt;
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
						rayQuery->m_collisionPoint = intPt;
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

BOOL CRuCollision_kdTree::QueryCollision(CRuCollisionQuery_Segment *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

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

			const CRuVector3 &v0 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3];
			const CRuVector3 &v1 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1];
			const CRuVector3 &v2 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2];

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
						segmentQuery->m_colPoints->Add(intPt);
						segmentQuery->m_colDistances->Add(intT);
					}
				}
				else
				{
					segmentQuery->m_colPoints->Add(intPt);
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

BOOL CRuCollision_kdTree::QueryCollision(CRuCollisionQuery_Segment_HitTest *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

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

			const CRuVector3 &v0 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3];
			const CRuVector3 &v1 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1];
			const CRuVector3 &v2 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2];

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

BOOL CRuCollision_kdTree::QueryCollision(CRuCollisionQuery_Segment_Nearest *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

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

			const CRuVector3 &v0 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3];
			const CRuVector3 &v1 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1];
			const CRuVector3 &v2 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2];

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
							segmentQuery->m_collisionPoint = intPt;
							segmentQuery->m_collisionNormal = n;
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
						segmentQuery->m_collisionPoint = intPt;
						segmentQuery->m_collisionNormal = n;
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

BOOL CRuCollision_kdTree::QueryCollision(CRuCollisionQuery_Enumerate *enumQuery, INT32 nodeIndex)
{
	for(INT32 i = 0; i < m_numVertices; ++i)
	{
		enumQuery->m_colTriVerts->Add(m_triVertices[i]);
	}

	return TRUE;
}

BOOL CRuCollision_kdTree::QueryCollision(CRuCollisionQuery_User_Flag_Set_Ray *setQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	BOOL flagSet = FALSE;
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

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

			const CRuVector3 &v0 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3];
			const CRuVector3 &v1 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1];
			const CRuVector3 &v2 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2];

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

BOOL CRuCollision_kdTree::QueryCollision(CRuCollisionQuery_User_Flag_Set_Sphere *setQuery, INT32 nodeIndex, CRuVector3 &volNearPt)
{
	BOOL flagSet = FALSE;
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

//	++m_nodesChecked;

	if(axis >= 3)
	{
		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
//			++m_trisChecked;

			const CRuVector3 &v0 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3];
			const CRuVector3 &v1 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1];
			const CRuVector3 &v2 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2];

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

BOOL CRuCollision_kdTree::QueryCollision(CRuCollisionQuery_User_Flag_Get_Segment *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

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

			const CRuVector3 &v0 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3];
			const CRuVector3 &v1 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1];
			const CRuVector3 &v2 = m_triVertices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2];

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
							segmentQuery->m_collisionPoint = intPt;
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
						segmentQuery->m_collisionPoint = intPt;
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

BOOL CRuCollision_kdTree::QueryCollision(CRuCollisionQuery_User_Flag_Enumerate *enumQuery, INT32 nodeIndex)
{
	// Iterate through all triangles
	INT32 numTriangles = m_numVertices / 3;
	for(INT32 i = 0, j = 0; i < numTriangles; ++i)
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
			enumQuery->m_colTriVerts->Add(m_triVertices[j]);
		}

		// Add triangle's flag to output list
		enumQuery->m_colTriFlags->Add(m_triFlags[i]);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
