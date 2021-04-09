/*!
	@project Rune
	@file RuPathGraph_Base.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/12/21
*/

#pragma once

#include "../PathEngine/RuPathEngine_Base.h"
#include "../PathEngine/RuPathFinder_Base.h"
#include "../PathEngine/RuPathRover_Base.h"

#include "../Core/Type/RuType_BitField.h"
#include "../Helper/RuHelper_StreamIO.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class IRuPathGraph_Base : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuArrayList<IRuPathGraph_Base *>		m_neighbors;									//!< Path graph neighbors

public:
	virtual									~IRuPathGraph_Base();

	INT32									InsertNeighbor(IRuPathGraph_Base *neighbor);
	BOOL									ClearAllNeighbors();

	virtual BOOL							SetCollisionObject(IRuObject *colObject) = 0;
	virtual BOOL							SetPathFinder(IRuPathFinder_Base *pathFinder) = 0;
	virtual BOOL							SetPathRover(IRuPathRover_Base *pathRover) = 0;

	virtual void							SetGraphKey(INT32 graphKey) = 0;
	virtual INT32							GetGraphKey() const = 0;

	virtual const CRuAABB&					Bounds() = 0;

	virtual INT32							GetNumPathNodes() = 0;
	virtual CRuPathNode*					GetPathNode(INT32 nodeID) = 0;

	virtual INT32							InsertPathNode(const CRuVector3 &position, BOOL deleteOverlappingNodes) = 0;
	virtual INT32							InsertPathNode(const CRuAABB &nodeBounds, BOOL deleteOverlappingNodes) = 0;
	virtual BOOL							DeletePathNode(CRuPathNode *pathNode) = 0;
	virtual BOOL							DeleteIsolatedPathNodes(INT32 minGroupSize) = 0;

	virtual BOOL							BuildPathNodeLinks() = 0;
	virtual BOOL							BuildPathNodeLinks(CRuPathNode *pathNode) = 0;
	virtual BOOL							ClearTransientPathNodeLinks() = 0;

	virtual CRuPathNode*					GetPathNodeAt(const CRuVector3 &point) = 0;
	virtual CRuPathNode*					GetPathNodeClosestTo(const CRuVector3 &point, REAL searchRadius) = 0;
	virtual BOOL							GetNearestPathNode(const CRuSegment3 &segment, CRuPathNode **pathNodeOut, REAL &distance) = 0;

	virtual BOOL							MarkPathNode(CRuPathNode *pathNode, RUCOLOR color) = 0;
	virtual BOOL							MarkNearestPathNode(const CRuSegment3 &segment, RUCOLOR color) = 0;
	virtual void							UnmarkAllPathNodes() = 0;
	virtual BOOL							IsPathNodeMarked(INT32 nodeID, RUCOLOR markedColor) = 0;

	virtual BOOL							EnumeratePathNodes(const CRuSphere &bounds, CRuArrayList<INT32> &pathNodesOut) = 0;

	virtual BOOL							EnumeratePathNodes(const CRuAABB &bounds, CRuArrayList<CRuPathNode *> &pathNodesOut) = 0;
	virtual BOOL							EnumeratePathNodes(const CRuSegment3 &segment, CRuArrayList<CRuPathNode *> &pathNodesOut) = 0;
	virtual BOOL							EnumeratePathNodes(const CRuSphere &bounds, CRuArrayList<CRuPathNode *> &pathNodesOut) = 0;

	virtual BOOL							FindPath(CRuPathNode *startNode, CRuPathNode *endNode, CRuArrayList<CRuPathNode *> &pathOut) = 0;
};

// ************************************************************************************************************************************************************

inline IRuPathGraph_Base::~IRuPathGraph_Base()
{
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
