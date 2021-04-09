/*!
	@project Rune
	@file RuPathEngine_Base.cpp

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/12/21
*/

#include "../PathEngine/RuPathEngine_Base.h"

#include "../Helper/RuHelper_StreamIO.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuPathNode::CRuPathNode()
:	m_nodeID(0),
	m_numNeighbors(0),
	m_neighbors(NULL)
{
}

CRuPathNode::CRuPathNode(const CRuAABB &nodeBounds)
:	m_nodeID(0),
	m_nodeBounds(nodeBounds),
	m_numNeighbors(0),
	m_neighbors(NULL)
{
}

CRuPathNode::~CRuPathNode()
{
	delete[] m_neighbors;
}

void CRuPathNode::SerializeTo(IRuStream *stream)
{
	// Write node ID
	stream->Write(&m_nodeID, sizeof(INT32));

	// Write node bounds
	::SerializeTo(stream, &m_nodeBounds);
}

void CRuPathNode::SerializeFrom(IRuStream *stream)
{
	// Read node ID
	stream->Read(&m_nodeID, sizeof(INT32));

	// Read node bounds
	::SerializeFrom(stream, &m_nodeBounds);
}

INT32 CRuPathNode::Connect(CRuPathNode *neighborPathNode, BOOL transient, REAL connectiveEpsilon, ConnectionValidationCallback validationCallback)
{
	// If the specified neighbor is already connected, just return it
	for(INT32 i = 0; i < m_numNeighbors; ++i)
	{
		if(m_neighbors[i].m_node == neighborPathNode)
		{
			return i;
		}
	}

	CRuAABB thisAABB(this->NodeBounds());
	CRuAABB neighborAABB(neighborPathNode->NodeBounds());

	thisAABB.Minima() -= connectiveEpsilon;
	thisAABB.Maxima() += connectiveEpsilon;

	neighborAABB.Minima() -= connectiveEpsilon;
	neighborAABB.Maxima() += connectiveEpsilon;

	// Check for intersection withthe neighbor
	if(this != neighborPathNode && Intersects(thisAABB, neighborAABB))
	{
		// Calculate centroid for this path node and the neighbor path node
		CRuVector3 thisCentroid = (thisAABB.Minima() + thisAABB.Maxima()) * 0.5f;
		CRuVector3 neighborCentroid = (neighborAABB.Minima() + neighborAABB.Maxima()) * 0.5f;

		// Initialize 2D bounding boxes for path node and the potential neighbor
		CRuAABB2D pathNodeAABB2D(CRuVector2(thisAABB.Minima().m_x, thisAABB.Minima().m_z), CRuVector2(thisAABB.Maxima().m_x, thisAABB.Maxima().m_z));
		CRuAABB2D neighborPathNodeAABB2D(CRuVector2(neighborAABB.Minima().m_x, neighborAABB.Minima().m_z), CRuVector2(neighborAABB.Maxima().m_x, neighborAABB.Maxima().m_z));

		// Initialize neighbor descriptor
		NeighborDesc neighborDesc;
		neighborDesc.m_transient = transient;
		neighborDesc.m_node = neighborPathNode;
		neighborDesc.m_cost = 1.0f;
		neighborDesc.m_numCrossingEdges = 0;

		// Iterate through each edge of the path node
		for(INT32 k = 0; k < 4; ++k)
		{
			// Get edge segment
			CRuSegment2 edge;
			pathNodeAABB2D.GetEdge(k, edge);

			// Build reverse of edge segment
			CRuSegment2 reversedEdge;
			reversedEdge.Origin() = edge.Origin() + edge.Direction();
			reversedEdge.Direction() = edge.Direction() * -1.0f;

			// Find overlapping segment
			CRuVector2 intPt0, intPt1;
			REAL intT0, intT1;

			BOOL overlappingEdgeExists = FALSE;
			CRuSegment2 overlappingEdge;

			if(FindIntersection(edge, neighborPathNodeAABB2D, intPt0, intT0) && FindIntersection(reversedEdge, neighborPathNodeAABB2D, intPt1, intT1))
			{
				overlappingEdgeExists = TRUE;
				overlappingEdge.SetFromPoints(intPt0, intPt1);
			}
			else
			{
				// NOTE: This happens when one node completely encloses another node. In this case,
				//       we try to find the crossing edge again, but this time reverse the
				//       intersection tests.

				// Get edge segment from neighbor node
				CRuSegment2 neighborEdge;
				neighborPathNodeAABB2D.GetEdge(k, neighborEdge);

				// Build reverse of edge segment from neighbor node
				CRuSegment2 reversedNeighborEdge;
				reversedNeighborEdge.Origin() = neighborEdge.Origin() + neighborEdge.Direction();
				reversedNeighborEdge.Direction() = neighborEdge.Direction() * -1.0f;

				// Deduce the crossing edge from the neighbor
				if(FindIntersection(neighborEdge, pathNodeAABB2D, intPt0, intT0) && FindIntersection(reversedNeighborEdge, pathNodeAABB2D, intPt1, intT1))
				{
					overlappingEdgeExists = TRUE;
					overlappingEdge.SetFromPoints(intPt0, intPt1);
				}
			}

			// Insert overlapping edge into the crossing edges list
			if(overlappingEdgeExists && overlappingEdge.Direction().Magnitude() > 1.0f)
			{
				// Discard edges that already exist in the list
				BOOL discardEdge = FALSE;
				for(INT32 n = 0; n < neighborDesc.m_numCrossingEdges; ++n)
				{
					CRuVector2 pt0 = neighborDesc.m_crossingEdges[n].Origin();
					CRuVector2 pt1 = pt0 + neighborDesc.m_crossingEdges[n].Direction();

					if(((pt0 - intPt0).Magnitude() < ruEPSILON && (pt1 - intPt1).Magnitude() < ruEPSILON) && ((pt0 - intPt1).Magnitude() < ruEPSILON && (pt1 - intPt0).Magnitude() < ruEPSILON))
					{
						discardEdge = TRUE;
						break;
					}
				}

				// If a validation callback is supplied, perform the validation call
				if(!discardEdge && (validationCallback == NULL || validationCallback(overlappingEdge, thisCentroid, neighborCentroid)))
				{
					// Add to crossing edge list
					neighborDesc.m_crossingEdges[neighborDesc.m_numCrossingEdges] = overlappingEdge;
					++neighborDesc.m_numCrossingEdges;
				}
			}
		}

		// If there are crossing edges, add neighbor descriptor into list of neighbors
		if(neighborDesc.m_numCrossingEdges > 0)
		{
			NeighborDesc *newNeighbors = ruNEW NeighborDesc [m_numNeighbors + 1];

			if(m_neighbors)
			{
				memcpy(newNeighbors, m_neighbors, sizeof(NeighborDesc) * m_numNeighbors);
				delete[] m_neighbors;
			}

			newNeighbors[m_numNeighbors] = neighborDesc;
			++m_numNeighbors;

			m_neighbors = newNeighbors;
		}
	}

	return -1;
}

BOOL CRuPathNode::Disconnect(CRuPathNode *neighborPathNode)
{
	for(INT32 i = 0; i < m_numNeighbors; ++i)
	{
		if(m_neighbors[i].m_node == neighborPathNode)
		{
			// Copy last neighbor to the current index
			if(i + 1 < m_numNeighbors)
			{
				m_neighbors[i] = m_neighbors[m_numNeighbors - 1];
			}

			--m_numNeighbors;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuPathNode::ClearAllNeighbors()
{
	delete[] m_neighbors;

	m_numNeighbors = 0;
	m_neighbors = NULL;

	return TRUE;
}

BOOL CRuPathNode::ClearNonTransientNeighbors()
{
	// Determine the total number of non-transient neighbors
	INT32 numNonTransientNeighbors = 0;

	for(INT32 i = 0; i < m_numNeighbors; ++i)
	{
		if(m_neighbors[i].m_transient == FALSE)
		{
			++numNonTransientNeighbors;
		}
	}

	// If there are non-transient neighbors, delete them now
	if(numNonTransientNeighbors > 0)
	{
		INT32 newNumNeighbors = m_numNeighbors - numNonTransientNeighbors;

		if(newNumNeighbors > 0)
		{
			NeighborDesc *newNeighbors = ruNEW NeighborDesc [newNumNeighbors];

			for(INT32 i = 0, j = 0; i < m_numNeighbors; ++i)
			{
				if(m_neighbors[i].m_transient)
				{
					newNeighbors[j] = m_neighbors[i];
					++j;
				}
			}

			delete[] m_neighbors;

			m_numNeighbors = newNumNeighbors;
			m_neighbors = newNeighbors;
		}
		else
		{
			delete[] m_neighbors;

			m_numNeighbors = 0;
			m_neighbors = 0;
		}
	}

	return TRUE;
}

BOOL CRuPathNode::ClearTransientNeighbors()
{
	// Determine the total number of transient neighbors
	INT32 numTransientNeighbors = 0;

	for(INT32 i = 0; i < m_numNeighbors; ++i)
	{
		if(m_neighbors[i].m_transient)
		{
			++numTransientNeighbors;
		}
	}

	// If there are transient neighbors, delete them now
	if(numTransientNeighbors > 0)
	{
		INT32 newNumNeighbors = m_numNeighbors - numTransientNeighbors;

		if(newNumNeighbors > 0)
		{
			NeighborDesc *newNeighbors = ruNEW NeighborDesc [newNumNeighbors];

			for(INT32 i = 0, j = 0; i < m_numNeighbors; ++i)
			{
				if(m_neighbors[i].m_transient == FALSE)
				{
					newNeighbors[j] = m_neighbors[i];
					++j;
				}
			}

			delete[] m_neighbors;

			m_numNeighbors = newNumNeighbors;
			m_neighbors = newNeighbors;
		}
		else
		{
			delete[] m_neighbors;

			m_numNeighbors = 0;
			m_neighbors = 0;
		}
	}

	return TRUE;
}

BOOL CRuPathNode::IsIsolated(INT32 minGroupSize)
{
	CRuKeyHash<PTRVALUE> uniqueNodes(256);
	CRuStack<CRuPathNode *> workStack;
	workStack.Push(this);

	while(workStack.IsEmpty() == FALSE)
	{
		// Pop current node
		CRuPathNode *curNode = workStack.Pop();

		// Insert node neighbors
		for(INT32 i = 0; i < curNode->GetNumNeighbors(); ++i)
		{
			// If the neighbor has not been processed, push it onto the work stack
			CRuPathNode *curNeighbor = curNode->GetNeighborNode(i);
			if(uniqueNodes.ContainsKey(reinterpret_cast<PTRVALUE>(curNeighbor)) == FALSE)
			{
				workStack.Push(curNeighbor);
			}
		}

		// Insert node address into unique nodes hash
		if(uniqueNodes.ContainsKey(reinterpret_cast<PTRVALUE>(curNode)) == FALSE)
		{
			uniqueNodes.Add(reinterpret_cast<PTRVALUE>(curNode));
		}

		// Check to see whether we have satisfied the minimum group size requirement
		if(uniqueNodes.Count() >= minGroupSize)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CRuPathNode::FindClosestCrossingPoint(CRuPathNode *neighborNode, const CRuVector3 &point, CRuVector3 &crossingPointOut)
{
	REAL nearestPointDist = FLT_MAX;

	for(INT32 i = 0; i < m_numNeighbors; ++i)
	{
		if(m_neighbors[i].m_node == neighborNode)
		{
			for(INT32 j = 0; j < m_neighbors[i].m_numCrossingEdges; ++j)
			{
				// Determine points of cross edge in 3-space
				CRuVector3 p0(m_neighbors[i].m_crossingEdges[j].Origin().m_x, this->m_nodeBounds.Maxima().m_y, m_neighbors[i].m_crossingEdges[j].Origin().m_y);
				CRuVector3 p1(p0.m_x + m_neighbors[i].m_crossingEdges[j].Direction().m_x, p0.m_y, p0.m_z + m_neighbors[i].m_crossingEdges[j].Direction().m_y);

				// Find closest point
				CRuVector3 closestPoint = ClosestPointOnSegment(p0, p1, point);

				REAL pointDistance = (closestPoint - point).Magnitude();

				if(pointDistance < nearestPointDist)
				{
					nearestPointDist = pointDistance;
					crossingPointOut = closestPoint;
				}
			}
		}
	}

	return nearestPointDist < FLT_MAX;
}

// ************************************************************************************************************************************************************

INT32 CRuPathNodePartition::s_partitionDensity = 50;

CRuPathNodePartition::CRuPathNodePartition(const CRuAABB &bounds)
:	m_partitionBounds_Strict(bounds),
	m_partitionBounds_Relaxed(bounds)
{
	// Ignore Y parameter, since this is a quadtree
	m_partitionBounds_Strict.Minima().m_y = -100000.0f;
	m_partitionBounds_Strict.Maxima().m_y = 100000.0f;

	m_partitionBounds_Relaxed.Minima().m_y = -100000.0f;
	m_partitionBounds_Relaxed.Maxima().m_y = 100000.0f;

	// Clear hierarchy pointers
	m_parentPartition = NULL;
	m_firstChildPartition = NULL;
	m_nextSiblingPartition = NULL;
}

CRuPathNodePartition::~CRuPathNodePartition()
{
	// Destroy children
	CRuPathNodePartition *childPartition = m_firstChildPartition;

	while(childPartition)
	{
		// Save pointer to next sibling
		CRuPathNodePartition *nextChildPartition = childPartition->m_nextSiblingPartition;

		// Delete child partition
		delete childPartition;

		// Advance to next child partition
		childPartition = nextChildPartition;
	}
}

BOOL CRuPathNodePartition::InsertPathNode(CRuPathNode *pathNode)
{
	// If the partition's strict bounds does not contain the path node centroid, abort
	CRuVector3 pathNodeCentroid = (pathNode->NodeBounds().Minima() + pathNode->NodeBounds().Maxima()) / 2.0f;
	if(!m_partitionBounds_Strict.Contains(pathNodeCentroid))
	{
		return FALSE;
	}

	if(IsLeaf())
	{
		// Merge path node bounds
		this->m_partitionBounds_Relaxed.Merge(pathNode->NodeBounds());

		// Propagate bounds relaxation up the tree
		this->RelaxBounds();

		// Insert path node into path node list
		m_pathNodes.Add(pathNode);

		// Subdivide
		Subdivide(s_partitionDensity);

		return TRUE;
	}
	else
	{
		// Try adding path node into one of the child partitions
		CRuPathNodePartition *childPartition = m_firstChildPartition;

		while(childPartition)
		{
			if(childPartition->InsertPathNode(pathNode))
			{
				return TRUE;
			}

			// Advance to next child partition
			childPartition = childPartition->m_nextSiblingPartition;
		}
	}

	return FALSE;
}

BOOL CRuPathNodePartition::DeletePathNode(CRuPathNode *pathNode)
{
	return DeletePathNodeImpl(pathNode);
}

CRuPathNode *CRuPathNodePartition::GetPathNodeAt(const CRuVector3 &point)
{
	if(m_partitionBounds_Relaxed.Contains(point))
	{
		if(IsLeaf())
		{
			for(INT32 i = 0; i < m_pathNodes.Count(); ++i)
			{
				if(m_pathNodes[i]->NodeBounds().Contains(point))
				{
					return m_pathNodes[i];
				}
			}
		}
		else
		{
			for(CRuPathNodePartition *childPartition = m_firstChildPartition; childPartition != NULL; childPartition = childPartition->m_nextSiblingPartition)
			{
				// Recursively invoke on child
				CRuPathNode *pathNode = childPartition->GetPathNodeAt(point);

				if(pathNode)
				{
					return pathNode;
				}
			}
		}
	}

	return NULL;
}

CRuPathNode *CRuPathNodePartition::GetPathNodeClosestTo(const CRuVector3 &point, REAL searchRadius)
{
	// Use some arbitrarily large search area
	CRuAABB searchBounds(CRuVector3(point.m_x - searchRadius, point.m_y - searchRadius, point.m_z - searchRadius), CRuVector3(point.m_x + searchRadius, point.m_y + searchRadius, point.m_z + searchRadius));
	REAL nearestPathNodeDistance = FLT_MAX;

	return GetPathNodeClosestToImpl(point, searchBounds, nearestPathNodeDistance);
}

BOOL CRuPathNodePartition::GetNearestPathNode(const CRuSegment3 &segment, CRuPathNode **pathNodeOut, REAL &distance, BOOL initialCall)
{
	BOOL returnValue = FALSE;

	if(initialCall)
	{
		distance = FLT_MAX;
	}

	if(Intersects(segment, m_partitionBounds_Relaxed))
	{
		if(IsLeaf())
		{
			for(INT32 i = 0; i < m_pathNodes.Count(); ++i)
			{
				CRuVector3 intPt;
				REAL intT;

				if(FindIntersection(segment, m_pathNodes[i]->NodeBounds(), intPt, intT))
				{
					if(intT < distance)
					{
						*pathNodeOut = m_pathNodes[i];
						distance = intT;

						returnValue = TRUE;
					}
				}
			}
		}
		else
		{
			for(CRuPathNodePartition *childPartition = m_firstChildPartition; childPartition != NULL; childPartition = childPartition->m_nextSiblingPartition)
			{
				BOOL childReturnValue = childPartition->GetNearestPathNode(segment, pathNodeOut, distance, FALSE);

				returnValue = returnValue | childReturnValue;
			}
		}
	}

	return returnValue;
}

void CRuPathNodePartition::Enumerate(const CRuSphere &bounds, CRuArrayList<INT32> &pathNodesOut)
{
	if(Intersects(m_partitionBounds_Relaxed, bounds))
	{
		if(IsLeaf())
		{
			for(INT32 i = 0; i < m_pathNodes.Count(); ++i)
			{
				CRuVector3 pathNodeCentroid = (m_pathNodes[i]->NodeBounds().Minima() + m_pathNodes[i]->NodeBounds().Maxima()) / 2.0f;

				if(bounds.Contains(pathNodeCentroid))
				{
					pathNodesOut.Add(m_pathNodes[i]->NodeID());
				}
			}
		}
		else
		{
			for(CRuPathNodePartition *childPartition = m_firstChildPartition; childPartition != NULL; childPartition = childPartition->m_nextSiblingPartition)
			{
				// Call enumerate on child partition
				childPartition->Enumerate(bounds, pathNodesOut);
			}
		}
	}
}

void CRuPathNodePartition::Enumerate(const CRuAABB &bounds, CRuArrayList<CRuPathNode *> &pathNodesOut)
{
	if(Intersects(m_partitionBounds_Relaxed, bounds))
	{
		if(IsLeaf())
		{
			for(INT32 i = 0; i < m_pathNodes.Count(); ++i)
			{
				if(Intersects(m_pathNodes[i]->NodeBounds(), bounds))
				{
					pathNodesOut.Add(m_pathNodes[i]);
				}
			}
		}
		else
		{
			for(CRuPathNodePartition *childPartition = m_firstChildPartition; childPartition != NULL; childPartition = childPartition->m_nextSiblingPartition)
			{
				// Call enumerate on child partition
				childPartition->Enumerate(bounds, pathNodesOut);
			}
		}
	}
}

void CRuPathNodePartition::Enumerate(const CRuSegment3 &segment, CRuArrayList<CRuPathNode *> &pathNodesOut)
{
	if(Intersects(segment, m_partitionBounds_Relaxed))
	{
		if(IsLeaf())
		{
			for(INT32 i = 0; i < m_pathNodes.Count(); ++i)
			{
				if(Intersects(segment, m_pathNodes[i]->NodeBounds()))
				{
					pathNodesOut.Add(m_pathNodes[i]);
				}
			}
		}
		else
		{
			for(CRuPathNodePartition *childPartition = m_firstChildPartition; childPartition != NULL; childPartition = childPartition->m_nextSiblingPartition)
			{
				// Call enumerate on child partition
				childPartition->Enumerate(segment, pathNodesOut);
			}
		}
	}
}

void CRuPathNodePartition::Enumerate(const CRuSphere &bounds, CRuArrayList<CRuPathNode *> &pathNodesOut)
{
	if(Intersects(m_partitionBounds_Relaxed, bounds))
	{
		if(IsLeaf())
		{
			for(INT32 i = 0; i < m_pathNodes.Count(); ++i)
			{
				if(Intersects(m_pathNodes[i]->NodeBounds(), bounds))
				{
					pathNodesOut.Add(m_pathNodes[i]);
				}
			}
		}
		else
		{
			for(CRuPathNodePartition *childPartition = m_firstChildPartition; childPartition != NULL; childPartition = childPartition->m_nextSiblingPartition)
			{
				// Call enumerate on child partition
				childPartition->Enumerate(bounds, pathNodesOut);
			}
		}
	}
}

void CRuPathNodePartition::RelaxBounds()
{
	// Merge relaxed partition bounds from each of the children
	for(CRuPathNodePartition *childPartition = m_firstChildPartition; childPartition != NULL; childPartition = childPartition->m_nextSiblingPartition)
	{
		this->m_partitionBounds_Relaxed.Merge(childPartition->m_partitionBounds_Relaxed);
	}

	// Recursively invoke on parent partition
	if(m_parentPartition)
	{
		m_parentPartition->RelaxBounds();
	}
}

CRuPathNode *CRuPathNodePartition::GetPathNodeClosestToImpl(const CRuVector3 &point, const CRuAABB &searchBounds, REAL &nearestPathNodeDistance)
{
	if(Intersects(m_partitionBounds_Relaxed, searchBounds))
	{
		if(IsLeaf())
		{
			REAL currentNearestPathNodeDistance = FLT_MAX;
			CRuPathNode *currentNearestPathNode = NULL;

			for(INT32 i = 0; i < m_pathNodes.Count(); ++i)
			{
				if(Intersects(m_pathNodes[i]->NodeBounds(), searchBounds))
				{
					REAL distanceToPoint = (point - m_pathNodes[i]->Centroid()).Magnitude();

					if(distanceToPoint < currentNearestPathNodeDistance)
					{
						currentNearestPathNodeDistance = distanceToPoint;
						currentNearestPathNode = m_pathNodes[i];
					}
				}
			}

			if(currentNearestPathNodeDistance < nearestPathNodeDistance)
			{
				nearestPathNodeDistance = currentNearestPathNodeDistance;
				return currentNearestPathNode;
			}

			return NULL;
		}
		else
		{
			CRuPathNode *result = NULL;

			for(CRuPathNodePartition *childPartition = m_firstChildPartition; childPartition != NULL; childPartition = childPartition->m_nextSiblingPartition)
			{
				// Recursively invoke on child
				CRuPathNode *pathNode = childPartition->GetPathNodeClosestToImpl(point, searchBounds, nearestPathNodeDistance);

				if(pathNode)
				{
					result = pathNode;
				}
			}

			return result;
		}
	}

	return NULL;
}

BOOL CRuPathNodePartition::DeletePathNodeImpl(CRuPathNode *pathNode)
{
	if(IsLeaf())
	{
		for(INT32 i = 0; i < m_pathNodes.Count(); ++i)
		{
			if(m_pathNodes[i] == pathNode)
			{
				// Remove path node from array
				m_pathNodes.RemoveAt(i);

				return TRUE;
			}
		}
	}
	else
	{
		// Try removing path node from one of the child partitions
		for(CRuPathNodePartition *childPartition = m_firstChildPartition; childPartition != NULL; childPartition = childPartition->m_nextSiblingPartition)
		{
			// Call enumerate on child partition
			if(childPartition->DeletePathNodeImpl(pathNode))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CRuPathNodePartition::Subdivide(INT32 targetDensity)
{
	if(targetDensity < m_pathNodes.Count())
	{
		// Initialize child partitions
		for(INT32 i = 0; i < 4; ++i)
		{
			// Calculate child partition's strict bounds
			CRuAABB childBounds;
			childBounds.SetBoundsToQuadrantXZ(m_partitionBounds_Strict, i);

			// Create child partition
			CRuPathNodePartition *childPartition = ruNEW CRuPathNodePartition(childBounds);

			// Set child partition's parent
			childPartition->m_parentPartition = this;

			// Assign new partition as first child partition
			childPartition->m_nextSiblingPartition = this->m_firstChildPartition;
			this->m_firstChildPartition = childPartition;
		}

		// Distribute path nodes to children
		for(INT32 i = 0; i < m_pathNodes.Count(); ++i)
		{
			for(CRuPathNodePartition *childPartition = m_firstChildPartition; childPartition != NULL; childPartition = childPartition->m_nextSiblingPartition)
			{
				if(childPartition->InsertPathNode(m_pathNodes[i]))
				{
					break;
				}
			}
		}

		// Guard against the degenerate case of everything going to one child
		CRuPathNodePartition *curChild = m_firstChildPartition;
		while(curChild)
		{
			if(curChild->m_pathNodes.Count() == m_pathNodes.Count())
			{
				CRuPathNodePartition *childPartition = m_firstChildPartition;
				while(childPartition)
				{
					// Save pointer to next sibling
					CRuPathNodePartition *nextChildPartition = childPartition->m_nextSiblingPartition;

					// Delete child partition
					delete childPartition;

					// Advance to next child partition
					childPartition = nextChildPartition;
				}

				return;
			}

			curChild = curChild->m_nextSiblingPartition;
		}

		// Clear path nodes list
		m_pathNodes.Clear();
		m_pathNodes.SetArraySize(0);
	}
}

// ************************************************************************************************************************************************************

#pragma managed(push, off)
