/*!
	@project Rune
	@file RuPathGraph_AStar.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/12/22
*/

#pragma once

#include "../PathEngine/RuPathGraph_Base.h"
#include "../PathEngine/RuPathFinder_AStar.h"
#include "../Core/Type/RuType_BitField.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuPathGraph_AStar : public IRuPathGraph_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32									m_graphKey;						//!< Graph key
	CRuAABB									m_bounds;						//!< Graph bounds
	REAL									m_pathNodeCellSize;
	REAL									m_pathNodeMinSize;
	REAL									m_pathNodeExpandLimit;

	CRuArrayList<CRuPathNode *>				m_pathNodes;

	CRuPathNodePartition*					m_pathNodePartition;

	// Other runtime data
	IRuPathFinder_Base*						m_pathFinder;
	IRuPathRover_Base*						m_pathRover;

	IRuObject*								m_collisionObject;				//!< Build-time collision object

	CRuHashMap<PTRVALUE, RUCOLOR>*			m_markedPathNodes;

public:
											CRuPathGraph_AStar();
											CRuPathGraph_AStar(const CRuAABB &bounds);
	virtual									~CRuPathGraph_AStar();

	// Serialization interface
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							SetCollisionObject(IRuObject *colObject);
	virtual BOOL							SetPathFinder(IRuPathFinder_Base *pathFinder);
	virtual BOOL							SetPathRover(IRuPathRover_Base *pathRover);

	virtual void							SetGraphKey(INT32 graphKey);
	virtual INT32							GetGraphKey() const;

	virtual const CRuAABB&					Bounds();

	virtual INT32							GetNumPathNodes();
	virtual CRuPathNode*					GetPathNode(INT32 nodeID);

	virtual INT32							InsertPathNode(const CRuVector3 &position, BOOL deleteOverlappingNodes);
	virtual INT32							InsertPathNode(const CRuAABB &nodeBounds, BOOL deleteOverlappingNodes);
	virtual BOOL							DeletePathNode(CRuPathNode *pathNode);
	virtual BOOL							DeleteIsolatedPathNodes(INT32 minGroupSize);

	virtual BOOL							BuildPathNodeLinks();
	virtual BOOL							BuildPathNodeLinks(CRuPathNode *pathNode);
	virtual BOOL							ClearTransientPathNodeLinks();

	virtual CRuPathNode*					GetPathNodeAt(const CRuVector3 &point);
	virtual CRuPathNode*					GetPathNodeClosestTo(const CRuVector3 &point, REAL searchRadius);
	virtual BOOL							GetNearestPathNode(const CRuSegment3 &segment, CRuPathNode **pathNodeOut, REAL &distance);

	virtual BOOL							MarkPathNode(CRuPathNode *pathNode, RUCOLOR color);
	virtual BOOL							MarkNearestPathNode(const CRuSegment3 &segment, RUCOLOR color);
	virtual void							UnmarkAllPathNodes();
	virtual BOOL							IsPathNodeMarked(INT32 nodeID, RUCOLOR markedColor);

	virtual BOOL							EnumeratePathNodes(const CRuSphere &bounds, CRuArrayList<INT32> &pathNodesOut);

	virtual BOOL							EnumeratePathNodes(const CRuAABB &bounds, CRuArrayList<CRuPathNode *> &pathNodesOut);
	virtual BOOL							EnumeratePathNodes(const CRuSegment3 &segment, CRuArrayList<CRuPathNode *> &pathNodesOut);
	virtual BOOL							EnumeratePathNodes(const CRuSphere &bounds, CRuArrayList<CRuPathNode *> &pathNodesOut);

	virtual BOOL							FindPath(CRuPathNode *startNode, CRuPathNode *endNode, CRuArrayList<CRuPathNode *> &pathOut);

protected:
	BOOL									InsertPathNode_Internal(const CRuAABB &nodeBounds);
	BOOL									InsertPathNode_Internal(const CRuAABB &nodeBounds, CRuArrayList<CRuAABB> &cells, CRuBitField &cellObstructed);
	BOOL									SubdivideIntoCells(const CRuAABB &bounds, CRuArrayList<CRuAABB> &cellsOut);
	BOOL									DeleteOverlappingPathNodes(const CRuAABB &bounds);

	BOOL									ValidateConnection(const CRuSegment2 &edge, const CRuVector3 &centroid0, const CRuVector3 &centroid1);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_PathGraph_AStar, CRuPathGraph_AStar)

// ************************************************************************************************************************************************************

#pragma managed(pop)
