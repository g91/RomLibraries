/*!
	@project Rune
	@file RuPathHierarchy_DS.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/12/27
*/

#pragma once

#include "../PathEngine/RuPathHierarchy_Base.h"
#include "../PathEngine/RuPathFinder_AStar.h"
#include "../Core/DataStore/RuDataStore_Base.h"

#include "../Core/Utility/RuUtility_Stream.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuPathHierarchy_DS : public IRuPathHierarchy_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct PathGraphDesc
	{
		BOOL								m_dirty;

		INT32								m_graphKey;

		// Neighbors?
		CRuArrayList<INT32>					m_neighbors;				//!< Neighbor path graph's graph keys

											PathGraphDesc() : m_dirty(FALSE) { }
	};

	// Path hierarchy settings
	REAL									m_connectiveEpsilon;

	// Data source
	CRuDataStore_Base*						m_dataStore;
	INT32									m_tableHandle_Header;
	INT32									m_tableHandle_PathGraphKey;
	INT32									m_tableHandle_PathGraph;

	// Hierarchy information
	CRuAABB									m_bounds;
	CRuPathGraphKeyPartition*				m_graphKeyPartition;
	CRuArrayList<PathGraphDesc *>			m_graphDescriptors;			//!< List of path graph descriptors (Primary holder)
	CRuHashMap<INT32, PathGraphDesc *>*		m_graphDescriptorHash;		//!< Hash of graph key to graph descriptor
	CRuArrayList<CRuPathNode *>				m_graphNodes;

	CRuLRUQueue<INT32, IRuPathGraph_Base *>*m_graphQueue;
	CRuArrayList<IRuPathGraph_Base *>		m_uncommittedGraphs;

	// Other runtime data
	IRuObject*								m_collisionObject;
	IRuPathFinder_Base*						m_pathFinder;
	IRuPathRover_Base*						m_pathRover;

	CRuArrayList<INT32>						m_dirtyGraphKeys;

	// Utility objects
	CRuStreamCompressor*					m_streamCompressor;					//!< Stream compression/decompression utility object
	CRuMemoryStream*						m_compressedStream;					//!< Temporary buffer space for compressed streams
	CRuMemoryStream*						m_decompressedStream;				//!< Temporary buffer space for decompressed streams

public:
											CRuPathHierarchy_DS();
	virtual									~CRuPathHierarchy_DS();

	// Data store stuff
	BOOL									Bind(CRuDataStore_Base *dataStore, const CRuAABB &bounds);
	BOOL									CopyTo(CRuDataStore_Base *targetDataStore);
	BOOL									Save();

	// Settings
	BOOL									SetPathGraphQueueSize(INT32 pathGraphQueueSize);

	virtual BOOL							SetCollisionObject(IRuObject *colObject);
	virtual BOOL							SetPathFinder(IRuPathFinder_Base *pathFinder);
	virtual BOOL							SetPathRover(IRuPathRover_Base *pathRover);

	// Path graph management
	virtual BOOL							InsertPathGraph(IRuPathGraph_Base *pathGraph);
	virtual BOOL							DeletePathGraph(IRuPathGraph_Base *pathGraph);

	virtual BOOL							MarkPathGraphAsDirty(IRuPathGraph_Base *pathGraph);

	virtual IRuPathGraph_Base*				GetPathGraph(INT32 graphKey);
	virtual IRuPathGraph_Base*				GetPathGraphAt(const CRuVector3 &point);

	virtual BOOL							EnumeratePathGraphKeys(const CRuSphere &bounds, CRuArrayList<INT32> &pathGraphKeysOut);
	virtual CRuPathNode*					FindPathNodeAt(const CRuVector3 &point, REAL proximitySearchRadius = 0.0f);
	virtual BOOL							FindIntersectingPathNodes(const CRuSegment3 &segment, CRuArrayList<CRuPathNode *> &pathNodesOut);
	virtual CRuPathNode*					FindAnyPathNode(const CRuSphere &sphere);
	virtual CRuPathNode*					FindNearestPathNode(const CRuSphere &sphere);
	virtual CRuPathNode*					FindNearestPathNode(const CRuSegment3 &segment);
	virtual BOOL							DeleteNearestPathNode(const CRuSegment3 &segment);

	virtual BOOL							MarkNearestPathNode(const CRuSegment3 &segment);
	virtual void							UnmarkAllPathNodes();

	virtual BOOL							PreLoadPathGraphs(const CRuAABB &bounds);

	// Path finding
	virtual BOOL							FindPath(CRuPathNode *startNode, CRuPathNode *endNode, CRuArrayList<CRuPathNode *> &pathOut);
	virtual BOOL							FindPath(const CRuVector3 &p0, const CRuVector3 &p1, CRuArrayList<CRuVector3> &pathOut);

protected:
	BOOL									ClearGraphQueue();
	static BOOL								ClearGraphQueueCallback(const void *key, const void *data, void *userData);

	IRuPathGraph_Base*						GetExistingPathGraph(INT32 graphKey);
	void									ConnectPathGraph(IRuPathGraph_Base *graph);

public:
	void									ConnectPathGraphAndNeighbors(IRuPathGraph_Base *graph);

	BOOL									UnmarkAllPathNodesCallback(INT32 graphKey);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
