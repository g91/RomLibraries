/*!
	@project Rune
	@file RuPathHierarchy_Base.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/12/27
*/

#ifndef _RUPATHHIERARCHY_BASE_H_
#define _RUPATHHIERARCHY_BASE_H_

#include "../PathEngine/RuPathGraph_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuPathGraphKeyPartition
{
private:
	struct GraphKeyDesc
	{
		INT32								m_graphKey;
		CRuAABB								m_graphBounds;

											GraphKeyDesc() { }
											GraphKeyDesc(INT32 graphKey, const CRuAABB &graphBounds) : m_graphKey(graphKey), m_graphBounds(graphBounds) { }
	};

	static INT32							s_partitionDensity;

	CRuAABB									m_partitionBounds;
	CRuArrayList<GraphKeyDesc>				m_pathGraphKeys;
	CRuPathGraphKeyPartition*				m_children[4];

	typedef boost::function<BOOL (INT32)>	ForAllElementsCallback;		//!< BOOL ForAllElementsCallback(INT32 graphKey)

public:
											CRuPathGraphKeyPartition(const CRuAABB &bounds);
											~CRuPathGraphKeyPartition();

	BOOL									IsLeaf() const { return m_children[0] == NULL; }

	// Tree-building
	BOOL									InsertPathGraphKey(INT32 graphKey, const CRuAABB &graphBounds);
	BOOL									DeletePathGraphKey(INT32 graphKey);

	// Spatial queries
	void									Enumerate(const CRuVector3 &point, CRuArrayList<INT32> &pathGraphKeysOut);
	void									Enumerate(const CRuSegment3 &segment, CRuArrayList<INT32> &pathGraphKeysOut);
	void									Enumerate(const CRuAABB &bounds, CRuArrayList<INT32> &pathGraphKeysOut);
	void									Enumerate(const CRuSphere &bounds, CRuArrayList<INT32> &pathGraphKeysOut);

	void									ForAllElements(ForAllElementsCallback callback);

protected:
	void									Subdivide(INT32 targetDensity);
};

// ************************************************************************************************************************************************************

class IRuPathHierarchy_Base : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuPathHierarchy_Base();

	virtual BOOL							SetCollisionObject(IRuObject *colObject) = 0;
	virtual BOOL							SetPathFinder(IRuPathFinder_Base *pathFinder) = 0;
	virtual BOOL							SetPathRover(IRuPathRover_Base *pathRover) = 0;

	// Path graph management
	virtual BOOL							InsertPathGraph(IRuPathGraph_Base *pathGraph) = 0;
	virtual BOOL							DeletePathGraph(IRuPathGraph_Base *pathGraph) = 0;

	virtual BOOL							MarkPathGraphAsDirty(IRuPathGraph_Base *pathGraph) = 0;

	virtual IRuPathGraph_Base*				GetPathGraph(INT32 graphKey) = 0;
	virtual IRuPathGraph_Base*				GetPathGraphAt(const CRuVector3 &point) = 0;

	virtual BOOL							EnumeratePathGraphKeys(const CRuSphere &bounds, CRuArrayList<INT32> &pathGraphKeysOut) = 0;
	virtual CRuPathNode*					FindPathNodeAt(const CRuVector3 &point, REAL proximitySearchRadius = 0.0f) = 0;
	virtual BOOL							FindIntersectingPathNodes(const CRuSegment3 &segment, CRuArrayList<CRuPathNode *> &pathNodesOut) = 0;
	virtual CRuPathNode*					FindNearestPathNode(const CRuSphere &sphere) = 0;
	virtual CRuPathNode*					FindNearestPathNode(const CRuSegment3 &segment) = 0;
	virtual BOOL							DeleteNearestPathNode(const CRuSegment3 &segment) = 0;

	virtual BOOL							MarkNearestPathNode(const CRuSegment3 &segment) = 0;
	virtual void							UnmarkAllPathNodes() = 0;

	virtual BOOL							PreLoadPathGraphs(const CRuAABB &bounds) = 0;

	// Path finding
	virtual BOOL							FindPath(CRuPathNode *startNode, CRuPathNode *endNode, CRuArrayList<CRuPathNode *> &pathOut) = 0;
	virtual BOOL							FindPath(const CRuVector3 &p0, const CRuVector3 &p1, CRuArrayList<CRuVector3> &pathOut) = 0;
};

// ************************************************************************************************************************************************************

inline IRuPathHierarchy_Base::~IRuPathHierarchy_Base()
{
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
