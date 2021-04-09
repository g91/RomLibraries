/*!
	@project Rune
	@file RuPathGraph_AStar.cpp

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/12/22
*/

#include "../PathEngine/RuPathGraph_AStar.h"

#include "../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuPathGraph_AStar, IRuPathGraph_Base)
ruHEAP_DEFINE_SUBCLASS(CRuPathGraph_AStar, "CRuPathGraph_AStar", "IRuPathGraph_Base")
ruCLASSGUID_DEFINE(CRuPathGraph_AStar, 0xB527F358,0x5D534157, 0xAEB3CD42, 0x212076CC)

// ************************************************************************************************************************************************************

CRuPathGraph_AStar::CRuPathGraph_AStar()
:	m_pathNodeCellSize(5.0f),
	m_pathNodeMinSize(5.0f),
	m_pathNodeExpandLimit(24.0f),
	m_pathNodePartition(NULL),
	m_pathFinder(NULL),
	m_pathRover(NULL),
	m_collisionObject(NULL),
	m_markedPathNodes(NULL)
{
}

CRuPathGraph_AStar::CRuPathGraph_AStar(const CRuAABB &bounds)
:	m_bounds(bounds),
	m_pathNodeCellSize(5.0f),
	m_pathNodeMinSize(5.0f),
	m_pathNodeExpandLimit(24.0f),
	m_pathNodePartition(NULL),
	m_pathFinder(NULL),
	m_pathRover(NULL),
	m_collisionObject(NULL),
	m_markedPathNodes(NULL)
{
	m_pathNodePartition = ruNEW CRuPathNodePartition(m_bounds);
}

CRuPathGraph_AStar::~CRuPathGraph_AStar()
{
	// Delete path nodes
	for(INT32 i = 0; i < m_pathNodes.Count(); ++i)
	{
		delete m_pathNodes[i];
	}
	m_pathNodes.Clear();

	// Destroy the path node partition
	delete m_pathNodePartition;

	// Release path finder
	ruSAFE_RELEASE(m_pathFinder);

	// Release path rover
	ruSAFE_RELEASE(m_pathRover);

	// Delete marked path nodes key hash
	delete m_markedPathNodes;
}

BOOL CRuPathGraph_AStar::SerializeFrom(IRuStream *stream)
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

	// Chain serialization to base class
	IRuPathGraph_Base::SerializeFrom(stream);

	switch(version)
	{
		case 1:
			{
				m_bounds.Set(0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 10.0f);

				// Construct path node partition if it does not already exist
				if(m_pathNodePartition == NULL)
				{
					m_pathNodePartition = ruNEW CRuPathNodePartition(m_bounds);
				}
			}

			break;

		case 2:
			{
				// Read graph key
				stream->Read(&m_graphKey, sizeof(INT32));

				// Read number of path nodes
				INT32 numPathNodes = 0;
				stream->Read(&numPathNodes, sizeof(UINT32));

				// Read path nodes
				m_pathNodes.SetArraySize(numPathNodes);
				m_pathNodes.SetNumEntries(numPathNodes);
				for(INT32 i = 0; i < numPathNodes; ++i)
				{
					m_pathNodes[i] = ruNEW CRuPathNode();
					m_pathNodes[i]->SerializeFrom(stream);
				}

				// Save graph bounds
				::SerializeFrom(stream, &m_bounds);

				// Construct path node partition if it does not already exist
				if(m_pathNodePartition == NULL)
				{
					m_pathNodePartition = ruNEW CRuPathNodePartition(m_bounds);
				}

				// Insert path nodes into the partitioning structure
				for(INT32 i = 0; i < numPathNodes; ++i)
				{
					m_pathNodePartition->InsertPathNode(m_pathNodes[i]);
				}
			}

			break;
	}

	return TRUE;
}

BOOL CRuPathGraph_AStar::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 2;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuPathGraph_Base::SerializeTo(stream);

	// Write graph key
	stream->Write(&m_graphKey, sizeof(INT32));

	// Save path nodes
	INT32 numPathNodes = m_pathNodes.Count();
	stream->Write(&numPathNodes, sizeof(UINT32));
	for(INT32 i = 0; i < numPathNodes; ++i)
	{
		m_pathNodes[i]->SerializeTo(stream);
	}

	// Save graph bounds
	::SerializeTo(stream, &m_bounds);

	return TRUE;
}

BOOL CRuPathGraph_AStar::SetCollisionObject(IRuObject *colObject)
{
	m_collisionObject = colObject;
	return TRUE;
}

BOOL CRuPathGraph_AStar::SetPathFinder(IRuPathFinder_Base *pathFinder)
{
	if(pathFinder)
	{
		// Increment reference on the new path finder
		pathFinder->AddRef();

		// Release old path finder and store reference to new path finder
		ruSAFE_RELEASE(m_pathFinder);
		m_pathFinder = pathFinder;
	}

	return TRUE;
}

BOOL CRuPathGraph_AStar::SetPathRover(IRuPathRover_Base *pathRover)
{
	pathRover->AddRef();
	ruSAFE_RELEASE(m_pathRover);
	m_pathRover = pathRover;

	return TRUE;
}

void CRuPathGraph_AStar::SetGraphKey(INT32 graphKey)
{
	m_graphKey = graphKey;
}

INT32 CRuPathGraph_AStar::GetGraphKey() const
{
	return m_graphKey;
}

const CRuAABB &CRuPathGraph_AStar::Bounds()
{
	return m_bounds;
}

INT32 CRuPathGraph_AStar::GetNumPathNodes()
{
	return m_pathNodes.Count();
}

CRuPathNode *CRuPathGraph_AStar::GetPathNode(INT32 nodeID)
{
	return m_pathNodes[nodeID];
}

INT32 CRuPathGraph_AStar::InsertPathNode(const CRuVector3 &position, BOOL deleteOverlappingNodes)
{
	// Construct path node partition if it does not already exist
	if(m_pathNodePartition == NULL)
	{
		m_pathNodePartition = ruNEW CRuPathNodePartition(m_bounds);
	}

	CRuVector3 m_defaultPathNodeExtents(20.0f, 12.5f, 20.0f);
	CRuAABB pathNodeBounds(position - m_defaultPathNodeExtents, position + m_defaultPathNodeExtents);

	if(deleteOverlappingNodes)
	{
		DeleteOverlappingPathNodes(pathNodeBounds);
	}

	return InsertPathNode_Internal(pathNodeBounds);

}

INT32 CRuPathGraph_AStar::InsertPathNode(const CRuAABB &nodeBounds, BOOL deleteOverlappingNodes)
{
	// Construct path node partition if it does not already exist
	if(m_pathNodePartition == NULL)
	{
		m_pathNodePartition = ruNEW CRuPathNodePartition(m_bounds);
	}

	if(deleteOverlappingNodes)
	{
		DeleteOverlappingPathNodes(nodeBounds);
	}

	// Construct new path node with the specified node bounds
	CRuPathNode *pathNode = ruNEW CRuPathNode();
	pathNode->NodeBounds() = nodeBounds;

	// Insert node into the path node list
	pathNode->NodeID() = m_pathNodes.Count();
	m_pathNodes.Add(pathNode);

	// Insert node into the partitioning structure
	m_pathNodePartition->InsertPathNode(pathNode);

	return TRUE;
}

BOOL CRuPathGraph_AStar::DeletePathNode(CRuPathNode *pathNode)
{
	for(INT32 i = 0; i < m_pathNodes.Count(); ++i)
	{
		if(m_pathNodes[i] == pathNode)
		{
			// Delete node from the partitioning structure
			m_pathNodePartition->DeletePathNode(pathNode);

			// Disconnect from all neighbors
			for(INT32 j = 0; j < pathNode->GetNumNeighbors(); ++j)
			{
				pathNode->GetNeighborNode(j)->Disconnect(pathNode);
			}

			// Delete node from the path node list
			delete m_pathNodes[i];
			m_pathNodes.RemoveAt(i);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuPathGraph_AStar::DeleteIsolatedPathNodes(INT32 minGroupSize)
{
	CRuStack<CRuPathNode *> isolatedNodeStack;

	// Check for node isolation
	for(INT32 i = 0; i < m_pathNodes.Count(); ++i)
	{
		if(m_pathNodes[i]->IsIsolated(minGroupSize))
		{
			isolatedNodeStack.Push(m_pathNodes[i]);
		}
	}

	// Remove isolated nodes
	while(isolatedNodeStack.IsEmpty() == FALSE)
	{
		DeletePathNode(isolatedNodeStack.Pop());
	}

	return TRUE;
}

BOOL CRuPathGraph_AStar::BuildPathNodeLinks()
{
	// Bind the validation callback
	CRuPathNode::ConnectionValidationCallback validateCallback = boost::bind(&CRuPathGraph_AStar::ValidateConnection, this, _1, _2, _3);

	CRuArrayList<CRuPathNode *> neighborPathNodes;

	INT32 connectsAttempted = 0;

	// Iterate through each path node
	for(INT32 i = 0; i < m_pathNodes.Count(); ++i)
	{
		// Pad the path node bounds by the connective epsilon
		CRuAABB enumBounds(m_pathNodes[i]->NodeBounds());
		enumBounds.Minima() -= 0.1f;
		enumBounds.Maxima() += 0.1f;

		// Enumerate intersecting path nodes
		neighborPathNodes.Clear();
		m_pathNodePartition->Enumerate(enumBounds, neighborPathNodes);

		// Clear all prior non-transient connections
		m_pathNodes[i]->ClearNonTransientNeighbors();

		// Connect intersecting path nodes
		for(INT32 j = 0; j < neighborPathNodes.Count(); ++j)
		{
			++connectsAttempted;

			m_pathNodes[i]->Connect(neighborPathNodes[j], FALSE, 0.1f, validateCallback);
		}
	}

	return TRUE;
}

BOOL CRuPathGraph_AStar::BuildPathNodeLinks(CRuPathNode *pathNode)
{
	// Bind the validation callback
	CRuPathNode::ConnectionValidationCallback validateCallback = boost::bind(&CRuPathGraph_AStar::ValidateConnection, this, _1, _2, _3);

	CRuArrayList<CRuPathNode *> neighborPathNodes;

	// Pad the path node bounds by the connective epsilon
	CRuAABB enumBounds(pathNode->NodeBounds());
	enumBounds.Minima() -= 0.1f;
	enumBounds.Maxima() += 0.1f;

	// Enumerate intersecting path nodes
	neighborPathNodes.Clear();
	m_pathNodePartition->Enumerate(enumBounds, neighborPathNodes);

	// Clear all prior non-transient connections
	pathNode->ClearNonTransientNeighbors();

	// Connect intersecting path nodes
	for(INT32 j = 0; j < neighborPathNodes.Count(); ++j)
	{
		pathNode->Connect(neighborPathNodes[j], FALSE, 0.1f, validateCallback);
	}

	return TRUE;
}

BOOL CRuPathGraph_AStar::ClearTransientPathNodeLinks()
{
	// Iterate through each path node
	for(INT32 i = 0; i < m_pathNodes.Count(); ++i)
	{
		// Clear transient links
		m_pathNodes[i]->ClearTransientNeighbors();
	}

	return TRUE;
}

CRuPathNode *CRuPathGraph_AStar::GetPathNodeAt(const CRuVector3 &point)
{
	return m_pathNodePartition->GetPathNodeAt(point);
}

CRuPathNode *CRuPathGraph_AStar::GetPathNodeClosestTo(const CRuVector3 &point, REAL searchRadius)
{
	return m_pathNodePartition->GetPathNodeClosestTo(point, searchRadius);
}

BOOL CRuPathGraph_AStar::GetNearestPathNode(const CRuSegment3 &segment, CRuPathNode **pathNodeOut, REAL &distance)
{
	if(m_pathNodePartition)
	{
		return m_pathNodePartition->GetNearestPathNode(segment, pathNodeOut, distance);
	}

	return FALSE;
}

BOOL CRuPathGraph_AStar::MarkPathNode(CRuPathNode *pathNode, RUCOLOR color)
{
	// If the marked path nodes key hash has not been allocated, allocate it now
	if(m_markedPathNodes == NULL)
	{
		m_markedPathNodes = ruNEW CRuHashMap<PTRVALUE, RUCOLOR>(128);
	}

	// Insert address of path node into the marked path node list
	m_markedPathNodes->Add(reinterpret_cast<PTRVALUE>(pathNode), color);

	return TRUE;
}

BOOL CRuPathGraph_AStar::MarkNearestPathNode(const CRuSegment3 &segment, RUCOLOR color)
{
	if(m_pathNodePartition)
	{
		CRuPathNode *pathNode;
		REAL distance;

		if(m_pathNodePartition->GetNearestPathNode(segment, &pathNode, distance))
		{
			// If the marked path nodes key hash has not been allocated, allocate it now
			if(m_markedPathNodes == NULL)
			{
				m_markedPathNodes = ruNEW CRuHashMap<PTRVALUE, RUCOLOR>(128);
			}

			// Insert address of path node into the marked path node list
			m_markedPathNodes->Add(reinterpret_cast<PTRVALUE>(pathNode), color);
		}
	}

	return FALSE;
}

void CRuPathGraph_AStar::UnmarkAllPathNodes()
{
	if(m_markedPathNodes)
	{
		m_markedPathNodes->Clear();
	}
}

BOOL CRuPathGraph_AStar::IsPathNodeMarked(INT32 nodeID, RUCOLOR markedColor)
{
	if(m_markedPathNodes)
	{
		if(markedColor == 0x00000000)
		{
			if(m_markedPathNodes->ContainsKey(reinterpret_cast<PTRVALUE>(m_pathNodes[nodeID])) == FALSE)
			{
				return TRUE;
			}
		}
		else
		{
			if(m_markedPathNodes->ContainsKey(reinterpret_cast<PTRVALUE>(m_pathNodes[nodeID])))
			{
				if(m_markedPathNodes->Get(reinterpret_cast<PTRVALUE>(m_pathNodes[nodeID])) == markedColor)
				{
					return TRUE;
				}
			}
		}
	}
	else
	{
		if(markedColor == 0x00000000)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuPathGraph_AStar::EnumeratePathNodes(const CRuSphere &bounds, CRuArrayList<INT32> &pathNodesOut)
{
	m_pathNodePartition->Enumerate(bounds, pathNodesOut);

	return TRUE;
}

BOOL CRuPathGraph_AStar::EnumeratePathNodes(const CRuAABB &bounds, CRuArrayList<CRuPathNode *> &pathNodesOut)
{
	m_pathNodePartition->Enumerate(bounds, pathNodesOut);

	return TRUE;
}

BOOL CRuPathGraph_AStar::EnumeratePathNodes(const CRuSegment3 &segment, CRuArrayList<CRuPathNode *> &pathNodesOut)
{
	m_pathNodePartition->Enumerate(segment, pathNodesOut);

	return TRUE;
}

BOOL CRuPathGraph_AStar::EnumeratePathNodes(const CRuSphere &bounds, CRuArrayList<CRuPathNode *> &pathNodesOut)
{
	m_pathNodePartition->Enumerate(bounds, pathNodesOut);

	return TRUE;
}

BOOL CRuPathGraph_AStar::FindPath(CRuPathNode *startNode, CRuPathNode *endNode, CRuArrayList<CRuPathNode *> &pathOut)
{
	if(m_pathFinder == NULL)
	{
		m_pathFinder = ruNEW CRuPathFinder_AStar();
	}

	return m_pathFinder->FindPath(startNode, endNode, pathOut);
}

BOOL CRuPathGraph_AStar::InsertPathNode_Internal(const CRuAABB &nodeBounds)
{
	// Query for potential obstructing triangles inside node bounds
	CRuMatrix4x4 identityMatrix;
	CRuCollisionQuery_OBB_TriEnum boxQuery;
	boxQuery.m_worldTransform = &identityMatrix;
	boxQuery.m_invWorldTransform = &identityMatrix;
	boxQuery.m_exclusionMask = 0x00A00000;		// Exclude foliage and water surfaces
	boxQuery.m_colOBB.Set(nodeBounds);
	
	if(m_collisionObject->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		static_cast<CRuEntity *>(m_collisionObject)->QueryCollision(&boxQuery);
	}
	else if(m_collisionObject->GetType().IsSubClassOf(IRuCollisionObject::Type()))
	{
		static_cast<IRuCollisionObject *>(m_collisionObject)->QueryCollision(&boxQuery);
	}

	// Make a list of cells
	CRuArrayList<CRuAABB> cells;
	SubdivideIntoCells(nodeBounds, cells);

	// Initialize bit field for cell obstruction
	CRuBitField cellObstructed(cells.Count());

	// Iterate through all enumerated triangles
	CRuVector3 verticalVec(0.0f, 1.0f, 0.0f);
	REAL walkableThreshold = cos(45.0f * (ruPI / 180.0f));

	for(INT32 i = 0; i < boxQuery.m_colTriFlags->Count(); ++i)
	{
		REAL dot = DotProduct(boxQuery.m_colTriNormals->Get(i), verticalVec);

		// The triangle is potentially obstructing if it is non-walkable
		if(dot < walkableThreshold)
		{
			// Check triangle for obstruction against all cells
			CRuVector3 &v0 = boxQuery.m_colTriVerts->Get(i * 3);
			CRuVector3 &v1 = boxQuery.m_colTriVerts->Get(i * 3 + 1);
			CRuVector3 &v2 = boxQuery.m_colTriVerts->Get(i * 3 + 2);

			for(INT32 j = 0; j < cells.Count(); ++j)
			{
/*
				if(IntersectsTriangle(cells[j], v0, v1, v2))
				{
					// Run rover in a cross route (run minima-to-maxima for now)

					// Anchor to surface at cell minima and maxima
					CRuVector3 p0 = cells[j].Minima();
					CRuVector3 p1 = cells[j].Maxima();

					p0.m_y = p1.m_y;

					if(	m_pathRover->ReAnchorPoint(p0) == FALSE || m_pathRover->ReAnchorPoint(p1) == FALSE || 
						m_pathRover->IsPathValid(p0, p1) == FALSE || m_pathRover->IsPathValid(p1, p0) == FALSE)
					{
						// For now, just subdivide until a threshold
						REAL curSubdivisionSize = min(min(nodeBounds.GetXSize(), nodeBounds.GetYSize()), nodeBounds.GetZSize()) / 2.0f;

						if(curSubdivisionSize >= m_pathNodeMinSize)
						{
							for(INT32 i = 0; i < 4; ++i)
							{
								CRuAABB subBounds;
								subBounds.SetBoundsToQuadrantXZ(nodeBounds, i);

								InsertPathNode(subBounds);
							}
						}

						return TRUE;
					}
				}
*/
				if(cellObstructed[j] == FALSE && IntersectsTriangle(cells[j], v0, v1, v2))
				{
					// Run rover in a cross route (run minima-to-maxima for now)

					// Anchor to surface at cell minima and maxima
					CRuVector3 p0 = cells[j].Minima();
					CRuVector3 p1 = cells[j].Maxima();

					p0.m_y = p1.m_y;

					if(m_pathRover->ReAnchorPoint(p0) == FALSE || m_pathRover->ReAnchorPoint(p1) == FALSE)
					{
						cellObstructed.Set(j);
						continue;
					}

					// Test accessibility in both directions
					if(m_pathRover->IsPathValid(p0, p1) == FALSE || m_pathRover->IsPathValid(p1, p0) == FALSE)
					{
						cellObstructed.Set(j);
						continue;
					}
				}

			}
/*
			// Find the X & Z extents of the non-walkable triangle

			// Test connectivity to all cells occupied by the non-walkable triangle

			// For now, just subdivide until a threshold
			REAL curSubdivisionSize = min(min(nodeBounds.GetXSize(), nodeBounds.GetYSize()), nodeBounds.GetZSize()) / 2.0f;

			if(curSubdivisionSize >= m_pathNodeMinSize)
			{
				for(INT32 i = 0; i < 4; ++i)
				{
					CRuAABB subBounds;
					subBounds.SetBoundsToQuadrantXZ(nodeBounds, i);

					InsertPathNode(subBounds);
				}
			}

			return TRUE;
*/
		}
	}

	InsertPathNode_Internal(nodeBounds, cells, cellObstructed);
/*
	// Construct new path node with the specified node bounds
	CRuPathNode *pathNode = ruNEW CRuPathNode();
	pathNode->NodeBounds() = nodeBounds;

	// Insert node into the path node list
	pathNode->NodeID() = m_pathNodes.Count();
	m_pathNodes.Add(pathNode);

	// Insert node into the partitioning structure
	m_pathNodePartition->InsertPathNode(pathNode);
*/
	return TRUE;
}

BOOL CRuPathGraph_AStar::InsertPathNode_Internal(const CRuAABB &nodeBounds, CRuArrayList<CRuAABB> &cells, CRuBitField &cellObstructed)
{
	CRuAABB insetNodeBounds(nodeBounds);

	insetNodeBounds.Minima() += ruEPSILON;
	insetNodeBounds.Maxima() -= ruEPSILON;

	for(INT32 i = 0; i < cells.Count(); ++i)
	{
		if(cellObstructed[i] && Intersects(insetNodeBounds, cells[i]))
		{
			REAL curSubdivisionSize = min(min(nodeBounds.GetXSize(), nodeBounds.GetYSize()), nodeBounds.GetZSize()) / 2.0f;

			if(curSubdivisionSize >= m_pathNodeMinSize)
			{
				for(INT32 i = 0; i < 4; ++i)
				{
					CRuAABB subBounds;
					subBounds.SetBoundsToQuadrantXZ(nodeBounds, i);

					InsertPathNode_Internal(subBounds, cells, cellObstructed);
				}
			}

			return TRUE;
		}
	}

	// Construct new path node with the specified node bounds
	CRuPathNode *pathNode = ruNEW CRuPathNode();
	pathNode->NodeBounds() = nodeBounds;

	// Insert node into the path node list
	pathNode->NodeID() = m_pathNodes.Count();
	m_pathNodes.Add(pathNode);

	// Insert node into the partitioning structure
	m_pathNodePartition->InsertPathNode(pathNode);

	return TRUE;
}

BOOL CRuPathGraph_AStar::SubdivideIntoCells(const CRuAABB &bounds, CRuArrayList<CRuAABB> &cellsOut)
{
	// Need to add a condition check to make sure the input is actually larger than the cell size

	for(REAL z = bounds.Minima().m_z; z < bounds.Maxima().m_z; z += m_pathNodeCellSize)
	{
		for(REAL x = bounds.Minima().m_x; x < bounds.Maxima().m_x; x += m_pathNodeCellSize)
		{
			cellsOut.Add(CRuAABB(x, bounds.Minima().m_y, z, x + m_pathNodeCellSize, bounds.Maxima().m_y, z + m_pathNodeCellSize));
		}
	}

	return TRUE;
}

BOOL CRuPathGraph_AStar::DeleteOverlappingPathNodes(const CRuAABB &bounds)
{
	// Enumerate all path nodes straddling the bounds
	CRuArrayList<CRuPathNode *> pathNodes;
	EnumeratePathNodes(bounds, pathNodes);

	for(INT32 i = 0; i < pathNodes.Count(); ++i)
	{
		CRuAABB nodeBounds = pathNodes[i]->NodeBounds();
		REAL originalVolume = nodeBounds.GetVolume();
		if(originalVolume > 0.0f)
		{
			nodeBounds.Intersection(bounds);
			REAL intersectionVolume = nodeBounds.GetVolume();
			REAL remainder = 1.0f - (intersectionVolume / originalVolume);

			if(remainder < 0.5f)
			{
				DeletePathNode(pathNodes[i]);
				--i;
			}
		}
	}

	return TRUE;
}

BOOL CRuPathGraph_AStar::ValidateConnection(const CRuSegment2 &edge, const CRuVector3 &centroid0, const CRuVector3 &centroid1)
{
	if(m_pathRover)
	{
		// Convert edge into 3-dimensional line
		CRuSegment3 edge3D;
		edge3D.Origin().Set(edge.Origin().m_x, (centroid0.m_y + centroid1.m_y) * 0.5f, edge.Origin().m_y);
		edge3D.Direction().Set(edge.Direction().m_x, 0.0f, edge.Direction().m_y);

		// Find point nearest centroid 0
		CRuVector3 crossingPt0 = ClosestPointOnSegment(edge3D.Origin(), edge3D.Origin() + edge3D.Direction(), centroid0);

		// Find point nearest centroid 1
		CRuVector3 crossingPt1 = ClosestPointOnSegment(edge3D.Origin(), edge3D.Origin() + edge3D.Direction(), centroid1);

		// Walk from centroid 0 to nearest point to centroid 1
		if(!m_pathRover->IsPathValid(centroid0, crossingPt0) || !m_pathRover->IsPathValid(crossingPt0, centroid1))
		{
			return FALSE;
		}

		// Walk from centroid 1 to nearest point to centroid 0
		if(!m_pathRover->IsPathValid(centroid1, crossingPt1) || !m_pathRover->IsPathValid(crossingPt1, centroid0))
		{
			return FALSE;
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
