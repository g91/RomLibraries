/*!
	@project Rune
	@file RuPathEngine_Base.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/12/21
*/

#pragma once

#include "../Collision/RuCollision_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

enum RuPathNodeLinkType
{
	ruPATHNODELINKTYPE_NONE					= 0,					//!< The link is invalid
	ruPATHNODELINKTYPE_FIXED				= 1,					//!< The link is a fixed block-to-block
	ruPATHNODELINKTYPE_RELOCATABLE			= 2,					//!< The link is a generic first ID match

	ruPATHNODELINKTYPE_FORCE_DWORD			= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

inline INT32 RuPathEngine_MakeGraphKey(INT32 x, INT32 z)
{
	return ((x & 0x000001FF) << 9) | (z & 0x000001FF);
}

inline INT32 RuPathEngine_MakeNodeKey(INT32 graphKey, INT32 nodeID)
{
	return (graphKey << 14) | (nodeID & 0x00003FFF);
}

inline INT32 RuPathEngine_GraphKeyFromNodeKey(INT32 nodeKey)
{
	return (nodeKey >> 14) & 0x0003FFFF;
}

inline INT32 RuPathEngine_NodeIDFromNodeKey(INT32 nodeKey)
{
	return nodeKey & 0x00003FFF;
}

// ************************************************************************************************************************************************************

class CRuPathNode
{
public:
	// BOOL ConnectionValidationCallback(const CRuSegment2 &edge, const CRuVector3 &centroid0, const CRuVector3 &centroid1)
	typedef boost::function<BOOL (const CRuSegment2 &, const CRuVector3 &, const CRuVector3 &)> ConnectionValidationCallback;

protected:
	struct NeighborDesc
	{
		// Basic link information
		BOOL								m_transient;				//!< Link is transient
		CRuPathNode*						m_node;						//!< Neighbor node
		REAL								m_cost;						//!< Cost of the link

		// Crossing edges
		INT32								m_numCrossingEdges;			//!< Lists the number of crossing edges - maximum of four
		CRuSegment2							m_crossingEdges[4];			//!< Edges which cross into the neighbor
	};

	INT32									m_nodeID;					//!< Node ID
	CRuAABB									m_nodeBounds;				//!< Bounding volume of the node within which any point is reachable by straight-line travel

	INT32									m_numNeighbors;
	NeighborDesc*							m_neighbors;

public:
											CRuPathNode();
											CRuPathNode(const CRuAABB &nodeBounds);
											~CRuPathNode();

	void									SerializeFrom(IRuStream *stream);
	void									SerializeTo(IRuStream *stream);

	INT32									Connect(CRuPathNode *neighborPathNode, BOOL transient, REAL connectiveEpsilon = 0.1f, ConnectionValidationCallback validationCallback = NULL);
	BOOL									Disconnect(CRuPathNode *neighborPathNode);
	BOOL									ClearAllNeighbors();
	BOOL									ClearNonTransientNeighbors();
	BOOL									ClearTransientNeighbors();

	BOOL									IsIsolated(INT32 minGroupSize);

	INT32&									NodeID() { return m_nodeID; }
	CRuAABB&								NodeBounds() { return m_nodeBounds; }
	const CRuAABB&							NodeBounds() const { return m_nodeBounds; }

	CRuVector3								Centroid() const { return (m_nodeBounds.Minima() + m_nodeBounds.Maxima()) * 0.5f; }

	INT32									GetNumNeighbors() const { return m_numNeighbors; }
	CRuPathNode*							GetNeighborNode(INT32 index) const { return m_neighbors[index].m_node; }
	REAL									GetNeighborCost(INT32 index) const { return m_neighbors[index].m_cost; }

	BOOL									FindClosestCrossingPoint(CRuPathNode *neighborNode, const CRuVector3 &point, CRuVector3 &crossingPointOut);
};

// ************************************************************************************************************************************************************

class CRuPathNodePartition
{
private:
	static INT32							s_partitionDensity;

	CRuAABB									m_partitionBounds_Strict;
	CRuAABB									m_partitionBounds_Relaxed;
	CRuArrayList<CRuPathNode *>				m_pathNodes;

	CRuPathNodePartition*					m_parentPartition;
	CRuPathNodePartition*					m_firstChildPartition;
	CRuPathNodePartition*					m_nextSiblingPartition;

public:
											CRuPathNodePartition(const CRuAABB &bounds);
											~CRuPathNodePartition();

	BOOL									IsLeaf() const { return m_firstChildPartition == NULL; }
	
	const CRuAABB&							Bounds() const { return m_partitionBounds_Strict; }
	CRuAABB&								Bounds() { return m_partitionBounds_Strict; }

	// Tree-building
	BOOL									InsertPathNode(CRuPathNode *pathNode);
	BOOL									DeletePathNode(CRuPathNode *pathNode);

	// Spatial queries
	CRuPathNode*							GetPathNodeAt(const CRuVector3 &point);
	CRuPathNode*							GetPathNodeClosestTo(const CRuVector3 &point, REAL searchRadius);
	BOOL									GetNearestPathNode(const CRuSegment3 &segment, CRuPathNode **pathNodeOut, REAL &distance, BOOL initialCall = TRUE);

	void									Enumerate(const CRuSphere &bounds, CRuArrayList<INT32> &pathNodesOut);

	void									Enumerate(const CRuAABB &bounds, CRuArrayList<CRuPathNode *> &pathNodesOut);
	void									Enumerate(const CRuSegment3 &segment, CRuArrayList<CRuPathNode *> &pathNodesOut);
	void									Enumerate(const CRuSphere &bounds, CRuArrayList<CRuPathNode *> &pathNodesOut);

protected:
	void									RelaxBounds();
	CRuPathNode*							GetPathNodeClosestToImpl(const CRuVector3 &point, const CRuAABB &searchBounds, REAL &nearestPathNodeDistance);
	BOOL									DeletePathNodeImpl(CRuPathNode *pathNode);
	void									Subdivide(INT32 targetDensity);
};

// ************************************************************************************************************************************************************

#pragma managed(push, off)
