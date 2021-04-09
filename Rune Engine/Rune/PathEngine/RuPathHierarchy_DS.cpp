/*!
	@project Rune
	@file RuPathHierarchy_DS.cpp

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/12/27
*/

#include "../PathEngine/RuPathHierarchy_DS.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuPathHierarchy_DS, IRuPathHierarchy_Base)
ruHEAP_DEFINE_SUBCLASS(CRuPathHierarchy_DS, "CRuPathHierarchy_DS", "IRuPathHierarchy_Base")
ruCLASSGUID_DEFINE(CRuPathHierarchy_DS, 0x843AE4AA, 0x72864d80, 0x8A0DC1BB, 0xAEB8FAAA)

// ************************************************************************************************************************************************************

static const char *pathDSInternal_TableName_Header = "pathds_header";
static const char *pathDSInternal_TableName_GraphKey = "pathds_graphkeys";
static const char *pathDSInternal_TableName_Graph = "pathds_graphs";

// ************************************************************************************************************************************************************

CRuPathHierarchy_DS::CRuPathHierarchy_DS()
:	m_connectiveEpsilon(0.1f),
	m_dataStore(NULL),
	m_graphKeyPartition(NULL),
	m_graphQueue(NULL),
	m_collisionObject(NULL),
	m_pathFinder(NULL),
	m_pathRover(NULL)
{
	// Initialize utility objects
	m_streamCompressor = ruNEW CRuStreamCompressor();
	m_compressedStream = ruNEW CRuMemoryStream();
	m_decompressedStream = ruNEW CRuMemoryStream();
}

CRuPathHierarchy_DS::~CRuPathHierarchy_DS()
{
	// Clear graph queue
	ClearGraphQueue();

	// Clear uncommitted graph list
	for(INT32 i = 0; i < m_uncommittedGraphs.Count(); ++i)
	{
		ruSAFE_RELEASE(m_uncommittedGraphs[i]);
	}
	m_uncommittedGraphs.Clear();

	// Release graph descriptors
	for(INT32 i = 0; i < m_graphDescriptors.Count(); ++i)
	{
		delete m_graphDescriptors[i];
	}

	// Release graph nodes
	for(INT32 i = 0; i < m_graphNodes.Count(); ++i)
	{
		delete m_graphNodes[i];
	}

	// Release data store
	ruSAFE_RELEASE(m_dataStore);

	// Delete key partition
	delete m_graphKeyPartition;

	// Delete graph map
	delete m_graphDescriptorHash;

	// Delete graph queue
	delete m_graphQueue;

	// Delete utility objects
	delete m_streamCompressor;
	delete m_compressedStream;
	delete m_decompressedStream;

	ruSAFE_RELEASE(m_pathRover);
	ruSAFE_RELEASE(m_pathFinder);
}

BOOL CRuPathHierarchy_DS::Bind(CRuDataStore_Base *dataStore, const CRuAABB &bounds)
{
	// Construct the graph key map
	m_graphDescriptorHash = ruNEW CRuHashMap<INT32, PathGraphDesc *>(8192);

	// Create LRU queue for path graphs
	m_graphQueue = ruNEW CRuLRUQueue<INT32, IRuPathGraph_Base *>(256, 1024);

	// Save reference to data store
	m_dataStore = dataStore;
	m_dataStore->AddRef();

	// Get data table handles
	m_tableHandle_Header = m_dataStore->GetTableHandle(pathDSInternal_TableName_Header);
	m_tableHandle_PathGraphKey = m_dataStore->GetTableHandle(pathDSInternal_TableName_GraphKey);
	m_tableHandle_PathGraph = m_dataStore->GetTableHandle(pathDSInternal_TableName_Graph);

	// Create data tables if they do not exist
	if(m_tableHandle_Header < 0)
	{
		m_tableHandle_Header = m_dataStore->CreateTable(pathDSInternal_TableName_Header, 4);
	}
	if(m_tableHandle_PathGraphKey < 0)
	{
		m_tableHandle_PathGraphKey = m_dataStore->CreateTable(pathDSInternal_TableName_GraphKey, 4);
	}
	if(m_tableHandle_PathGraph < 0)
	{
		m_tableHandle_PathGraph = m_dataStore->CreateTable(pathDSInternal_TableName_Graph, 4);
	}

	// Construct the graph key partition
	m_graphKeyPartition = ruNEW CRuPathGraphKeyPartition(bounds);

	// Read data source header
	if(m_tableHandle_Header >= 0)
	{
		INT32 dsHeaderKey = 0;
		IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_Header, (BYTE *) &dsHeaderKey);

		if(stream)
		{
			// Decompress header data block
			m_streamCompressor->Decompress(stream, 0, stream->GetStreamSize(), m_decompressedStream);
			delete stream;

			// Read DS version
			INT32 dsVersion = 1;
			m_decompressedStream->Read(&dsVersion, sizeof(INT32));

			// Load graph descriptors
			INT32 numGraphDescriptors = 0;
			m_decompressedStream->Read(&numGraphDescriptors, sizeof(INT32));
			for(INT32 i = 0; i < numGraphDescriptors; ++i)
			{
				// Read key
				INT32 graphKey;
				m_decompressedStream->Read(&graphKey, sizeof(INT32));
				DWORD keyHash = RuGlobalHashGen().GetHashCode(graphKey);

				// Create path graph descriptor
				PathGraphDesc *pathGraphDesc = ruNEW PathGraphDesc();
				pathGraphDesc->m_graphKey = graphKey;

				// Read number of neighbors
				INT32 numNeighbors = 0;
				m_decompressedStream->Read(&numNeighbors, sizeof(INT32));
				pathGraphDesc->m_neighbors.SetArraySize(numNeighbors);
				pathGraphDesc->m_neighbors.SetNumEntries(numNeighbors);

				// Read neighbors
				for(INT32 j = 0; j < numNeighbors; ++j)
				{
					m_decompressedStream->Read(&pathGraphDesc->m_neighbors[j], sizeof(INT32));
				}

				// Insert path graph descriptor into the map & list
				m_graphDescriptors.Add(pathGraphDesc);
				m_graphDescriptorHash->Add(graphKey, keyHash, pathGraphDesc);
			}

			// Load graph nodes
			INT32 numGraphNodes = 0;
			m_decompressedStream->Read(&numGraphNodes, sizeof(INT32));
			m_graphNodes.SetArraySize(numGraphNodes);
			m_graphNodes.SetNumEntries(numGraphNodes);
			for(INT32 i = 0; i < numGraphNodes; ++i)
			{
				m_graphNodes[i] = ruNEW CRuPathNode();
				m_graphNodes[i]->SerializeFrom(m_decompressedStream);
			}
		}
	}

	// Read path graph key descriptors
	if(m_tableHandle_PathGraphKey >= 0)
	{
		// Read graph keys into graph key partition
		INT32 numGraphs = m_dataStore->GetNumRecords(m_tableHandle_PathGraphKey);
		for(INT32 i = 0; i < numGraphs; ++i)
		{
			// Retrieve graph key record
			IRuStream *stream = m_dataStore->RetrieveByIndex(m_tableHandle_PathGraphKey, i);

			if(stream == NULL)
				continue;

			// Read graph key from stream
			INT32 graphKey;
			stream->Read(&graphKey, sizeof(INT32));

			// Read graph bounds from stream
			CRuAABB graphBounds;
			stream->Read(&graphBounds.Minima().x, sizeof(REAL));
			stream->Read(&graphBounds.Minima().y, sizeof(REAL));
			stream->Read(&graphBounds.Minima().z, sizeof(REAL));
			stream->Read(&graphBounds.Maxima().x, sizeof(REAL));
			stream->Read(&graphBounds.Maxima().y, sizeof(REAL));
			stream->Read(&graphBounds.Maxima().z, sizeof(REAL));

			// Release stream
			delete stream;

			// Insert graph key into graph key partition
			m_graphKeyPartition->InsertPathGraphKey(graphKey, graphBounds);
		}
	}

	return TRUE;
}

BOOL CRuPathHierarchy_DS::CopyTo(CRuDataStore_Base *targetDataStore)
{
	INT32 srcTableList[3];
	srcTableList[0] = m_tableHandle_Header;
	srcTableList[1] = m_tableHandle_PathGraphKey;
	srcTableList[2] = m_tableHandle_PathGraph;

	// Create tables
	INT32 targetTableList[3];
	targetTableList[0] = targetDataStore->CreateTable(pathDSInternal_TableName_Header, 4);
	targetTableList[1] = targetDataStore->CreateTable(pathDSInternal_TableName_GraphKey, 4);
	targetTableList[2] = targetDataStore->CreateTable(pathDSInternal_TableName_Graph, 4);

	// Copy data in each table
	for(INT32 i = 0; i < 3; ++i)
	{
		INT32 numEntries = m_dataStore->GetNumRecords(srcTableList[i]);
		INT32 keyLength = m_dataStore->GetTableKeyLength(srcTableList[i]);
		BYTE *key = ruNEW BYTE [keyLength];

		for(INT32 j = 0; j < numEntries; ++j)
		{
			// Retrieve key and data
			IRuStream *stream = m_dataStore->RetrieveByIndex(srcTableList[i], j);

			if(stream)
			{
				m_dataStore->RetrieveKeyByIndex(srcTableList[i], j, key);
				targetDataStore->Store(targetTableList[i], key, stream);
				delete stream;
			}
		}

		delete[] key;
	}

	return TRUE;
}

BOOL CRuPathHierarchy_DS::Save()
{
	// Stream DS header
	m_decompressedStream->SetStreamSize(0);

	// Write DS version
	INT32 dsVersion = 1;
	m_decompressedStream->Write(&dsVersion, sizeof(INT32));

	// Save graph descriptors
	INT32 numGraphDescriptors = m_graphDescriptors.Count();
	m_decompressedStream->Write(&numGraphDescriptors, sizeof(INT32));
	for(INT32 i = 0; i < numGraphDescriptors; ++i)
	{
		PathGraphDesc *pathGraphDesc = m_graphDescriptors[i];

		// Calculate key and key hash
		INT32 graphKey = pathGraphDesc->m_graphKey;
		DWORD keyHash = RuGlobalHashGen().GetHashCode(graphKey);

		// Write key
		m_decompressedStream->Write(&graphKey, sizeof(INT32));

		// Write number of neighbors
		INT32 numNeighbors = pathGraphDesc->m_neighbors.Count();
		m_decompressedStream->Write(&numNeighbors, sizeof(INT32));

		// Write neighbors
		for(INT32 j = 0; j < numNeighbors; ++j)
		{
			m_decompressedStream->Write(&pathGraphDesc->m_neighbors[j], sizeof(INT32));
		}
	}

	// Save graph nodes
	INT32 numGraphNodes = m_graphNodes.Count();
	m_decompressedStream->Write(&numGraphNodes, sizeof(INT32));
	for(INT32 i = 0; i < numGraphNodes; ++i)
	{
		m_graphNodes[i]->SerializeTo(m_decompressedStream);
	}

	// Compress
	m_compressedStream->SetStreamSize(0);
	m_streamCompressor->Compress(m_decompressedStream, m_compressedStream);

	// Store to data store
	INT32 dsHeaderKey = 0;
	m_dataStore->Store(m_tableHandle_Header, (BYTE *) &dsHeaderKey, m_compressedStream);

	// Store uncommitted graphs to data store
	for(INT32 i = 0; i < m_uncommittedGraphs.Count(); ++i)
	{
		INT32 graphKey = m_uncommittedGraphs[i]->GetGraphKey();
		DWORD keyHash = RuGlobalHashGen().GetHashCode(graphKey);

		// Stream graph header
		m_decompressedStream->SetStreamSize(0);

		// Write graph key
		m_decompressedStream->Write(&graphKey, sizeof(INT32));

		// Write graph bounds
		m_decompressedStream->Write(&m_uncommittedGraphs[i]->Bounds().Minima().x, sizeof(REAL));
		m_decompressedStream->Write(&m_uncommittedGraphs[i]->Bounds().Minima().y, sizeof(REAL));
		m_decompressedStream->Write(&m_uncommittedGraphs[i]->Bounds().Minima().z, sizeof(REAL));
		m_decompressedStream->Write(&m_uncommittedGraphs[i]->Bounds().Maxima().x, sizeof(REAL));
		m_decompressedStream->Write(&m_uncommittedGraphs[i]->Bounds().Maxima().y, sizeof(REAL));
		m_decompressedStream->Write(&m_uncommittedGraphs[i]->Bounds().Maxima().z, sizeof(REAL));

		// Store to data store
		m_dataStore->Store(m_tableHandle_PathGraphKey, (BYTE *) &graphKey, m_decompressedStream);

		// Stream graph
		m_decompressedStream->SetStreamSize(0);
		m_uncommittedGraphs[i]->SerializeTo(m_decompressedStream);

		// Compress
		m_compressedStream->SetStreamSize(0);
		m_streamCompressor->Compress(m_decompressedStream, m_compressedStream);

		// Store to data store
		m_dataStore->Store(m_tableHandle_PathGraph, (BYTE *) &graphKey, m_compressedStream);

		// Release reference
		ruSAFE_RELEASE(m_uncommittedGraphs[i]);
	}

	// Clear list of uncommitted graphs
	m_uncommittedGraphs.Clear();

	// Remove deleted graphs

	return TRUE;
}

BOOL CRuPathHierarchy_DS::SetPathGraphQueueSize(INT32 pathGraphQueueSize)
{
	// Flush all path graphs
	ClearGraphQueue();

	// Destroy and re-create LRU queue for path graphs
	delete m_graphQueue;
	m_graphQueue = ruNEW CRuLRUQueue<INT32, IRuPathGraph_Base *>(pathGraphQueueSize, pathGraphQueueSize * 4);

	return TRUE;
}

BOOL CRuPathHierarchy_DS::SetCollisionObject(IRuObject *colObject)
{
	m_collisionObject = colObject;
	return TRUE;
}

BOOL CRuPathHierarchy_DS::SetPathFinder(IRuPathFinder_Base *pathFinder)
{
	if(pathFinder)
	{
		// Increment reference on the new path finder
		pathFinder->AddRef();

		// Release old path finder and store reference to new path finder
		ruSAFE_RELEASE(m_pathFinder);
		m_pathFinder = pathFinder;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuPathHierarchy_DS::SetPathRover(IRuPathRover_Base *pathRover)
{
	if(pathRover)
	{
		pathRover->AddRef();
		ruSAFE_RELEASE(m_pathRover);
		m_pathRover = pathRover;
	}
	else
	{
		ruSAFE_RELEASE(m_pathRover);
	}

	return TRUE;
}

BOOL CRuPathHierarchy_DS::InsertPathGraph(IRuPathGraph_Base *pathGraph)
{
	// Calculate graph key hash
	INT32 graphKey = pathGraph->GetGraphKey();
	DWORD graphKeyHash = RuGlobalHashGen().GetHashCode(graphKey);

	// Remove path graph from graph key partition
	m_graphKeyPartition->DeletePathGraphKey(graphKey);

	// Remove path graph from graph queue
	DWORD keyHash = RuGlobalHashGen().GetHashCode(pathGraph->GetGraphKey());

	if(m_graphQueue->ContainsKey(graphKey,graphKeyHash))
	{
		IRuPathGraph_Base *reclaimedGraph;
		m_graphQueue->Get(graphKey, graphKeyHash, reclaimedGraph);
		ruSAFE_RELEASE(reclaimedGraph);

		m_graphQueue->Remove(graphKey, graphKeyHash);
	}

	// Insert path graph into graph key partition
	m_graphKeyPartition->InsertPathGraphKey(pathGraph->GetGraphKey(), pathGraph->Bounds());

	// Override path finder setting
	pathGraph->SetPathFinder(m_pathFinder);

	// If the uncommitted graph list already has a path graph with the same key, overwrite it
	BOOL graphInserted = FALSE;
	for(INT32 i = 0; i < m_uncommittedGraphs.Count(); ++i)
	{
		if(pathGraph->GetGraphKey() == m_uncommittedGraphs[i]->GetGraphKey())
		{
			pathGraph->AddRef();

			// Replace path graph
			ruSAFE_RELEASE(m_uncommittedGraphs[i]);
			m_uncommittedGraphs[i] = pathGraph;

			// Break
			graphInserted = TRUE;
			break;
		}
	}

	// Otherwise append the path graph to the uncommitted graph list
	if(graphInserted == FALSE)
	{
		pathGraph->AddRef();
		m_uncommittedGraphs.Add(pathGraph);
	}

	// Get path graph descriptor, or create one if it does not already exist
	PathGraphDesc *pathGraphDesc = NULL;
	if(m_graphDescriptorHash->Get(pathGraph->GetGraphKey(), keyHash, pathGraphDesc) == FALSE)
	{
		// Create path graph descriptor
		pathGraphDesc = ruNEW PathGraphDesc();
		pathGraphDesc->m_graphKey = pathGraph->GetGraphKey();

		// Insert path graph descriptor into the map & list
		m_graphDescriptors.Add(pathGraphDesc);
		m_graphDescriptorHash->Add(pathGraph->GetGraphKey(), keyHash, pathGraphDesc);

		// Create matching path node
		CRuPathNode *pathGraphNode = ruNEW CRuPathNode(pathGraph->Bounds());
		m_graphNodes.Add(pathGraphNode);
	}

	// Increment path graph reference count
	pathGraph->AddRef();

	// Insert into graph queue
	if(m_graphQueue->ContainsKey(graphKey, graphKeyHash))
	{
		// Free existing path graph
		IRuPathGraph_Base *reclaimedPathGraph;
		m_graphQueue->Get(graphKey, graphKeyHash, reclaimedPathGraph);
		ruSAFE_RELEASE(reclaimedPathGraph);

		// Re-insert graph into graph queue
		m_graphQueue->Add(graphKey, graphKeyHash, pathGraph, 1);
	}
	else
	{
		// Free up space in the graph queue
		IRuPathGraph_Base *reclaimedPathGraph;
		while(m_graphQueue->Reclaim(graphKey, graphKeyHash, 1, reclaimedPathGraph))
		{
			// Do something to clear all transient nodes attached to this graph

			// Release instanced path graph
			ruSAFE_RELEASE(reclaimedPathGraph);
		}

		// Insert graph into graph queue
		m_graphQueue->Add(graphKey, graphKeyHash, pathGraph, 1);
	}

	// Connect neighboring graphs
	ConnectPathGraphAndNeighbors(pathGraph);

	return TRUE;
}

BOOL CRuPathHierarchy_DS::DeletePathGraph(IRuPathGraph_Base *pathGraph)
{
	return TRUE;
}

BOOL CRuPathHierarchy_DS::MarkPathGraphAsDirty(IRuPathGraph_Base *pathGraph)
{
	IRuPathGraph_Base *targetPathGraph = GetExistingPathGraph(pathGraph->GetGraphKey());

	if(targetPathGraph)
	{
		if(m_uncommittedGraphs.Contains(targetPathGraph) == FALSE)
		{
			targetPathGraph->AddRef();
			m_uncommittedGraphs.Add(targetPathGraph);
		}

		return TRUE;
	}

	return FALSE;
}

IRuPathGraph_Base *CRuPathHierarchy_DS::GetPathGraph(INT32 graphKey)
{
	IRuPathGraph_Base *pathGraph = GetExistingPathGraph(graphKey);

	if(pathGraph)
	{
		return pathGraph;
	}

	// Attempt to locate the specified graph in the data store
	if(pathGraph == NULL)
	{
		IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_PathGraph, (BYTE *) &graphKey);

		if(stream)
		{
			// Decompress chunk data block
			m_streamCompressor->Decompress(stream, 0, stream->GetStreamSize(), m_decompressedStream);
			delete stream;

			IRuObject *pathGraphObject = RuObjectFactory().CreateObject(m_decompressedStream);

			if(pathGraphObject && pathGraphObject->GetType().IsSubClassOf(IRuPathGraph_Base::Type()))
			{
				pathGraph = static_cast<IRuPathGraph_Base *>(pathGraphObject);
				pathGraph->SetCollisionObject(m_collisionObject);
				pathGraph->SetPathFinder(m_pathFinder);
			}
		}
	}

	if(pathGraph)
	{
		DWORD graphKeyHash = RuGlobalHashGen().GetHashCode(graphKey);

		// Free up space in the graph queue
		IRuPathGraph_Base *reclaimedPathGraph;
		while(m_graphQueue->Reclaim(graphKey, graphKeyHash, 1, reclaimedPathGraph))
		{
			// Do something to clear all transient nodes attached to this graph

			// Release instanced path graph
			ruSAFE_RELEASE(reclaimedPathGraph);
		}

		// Insert graph into graph queue
		m_graphQueue->Add(graphKey, graphKeyHash, pathGraph, 1);

		// Build internal path links
		pathGraph->BuildPathNodeLinks();

		// Connect neighboring graphs
		ConnectPathGraphAndNeighbors(pathGraph);
	}

	return pathGraph;
}

IRuPathGraph_Base *CRuPathHierarchy_DS::GetPathGraphAt(const CRuVector3 &point)
{
	// Enumerate keys of path graphs that contain this point
	CRuArrayList<INT32> containingPathGraphKeys;
	m_graphKeyPartition->Enumerate(point, containingPathGraphKeys);

	// Return the first path graph that contains this point
	for(INT32 i = 0; i < containingPathGraphKeys.Count(); ++i)
	{
		IRuPathGraph_Base *pathGraph = this->GetPathGraph(containingPathGraphKeys[i]);

		if(pathGraph)
		{
			return pathGraph;
		}
	}

	return NULL;
}

BOOL CRuPathHierarchy_DS::EnumeratePathGraphKeys(const CRuSphere &bounds, CRuArrayList<INT32> &pathGraphKeysOut)
{
	m_graphKeyPartition->Enumerate(bounds, pathGraphKeysOut);

	return TRUE;
}

CRuPathNode *CRuPathHierarchy_DS::FindPathNodeAt(const CRuVector3 &point, REAL proximitySearchRadius)
{
	// Enumerate keys of path graphs that contain this point
	CRuArrayList<INT32> containingPathGraphKeys;
	m_graphKeyPartition->Enumerate(point, containingPathGraphKeys);

	// Find the first path node that contains this point
	for(INT32 i = 0; i < containingPathGraphKeys.Count(); ++i)
	{
		IRuPathGraph_Base *pathGraph = this->GetPathGraph(containingPathGraphKeys[i]);

		if(pathGraph)
		{
			CRuPathNode *pathNode = pathGraph->GetPathNodeAt(point);

			if(pathNode)
			{
				return pathNode;
			}
		}
	}

	// No path node was found -- now try to find closest path node
	if(proximitySearchRadius > 0.0f)
	{
		for(INT32 i = 0; i < containingPathGraphKeys.Count(); ++i)
		{
			IRuPathGraph_Base *pathGraph = this->GetPathGraph(containingPathGraphKeys[i]);

			if(pathGraph)
			{
				CRuPathNode *pathNode = pathGraph->GetPathNodeClosestTo(point, proximitySearchRadius);

				if(pathNode)
				{
					return pathNode;
				}
			}
		}
	}

	return NULL;
}

BOOL CRuPathHierarchy_DS::FindIntersectingPathNodes(const CRuSegment3 &segment, CRuArrayList<CRuPathNode *> &pathNodesOut)
{
	// Enumerate keys of path graphs that intersect the segment
	CRuArrayList<INT32> intersectingPathGraphKeys;
	m_graphKeyPartition->Enumerate(segment, intersectingPathGraphKeys);

	// Enumerate all path nodes that intersect the segment
	for(INT32 i = 0; i < intersectingPathGraphKeys.Count(); ++i)
	{
		IRuPathGraph_Base *pathGraph = this->GetPathGraph(intersectingPathGraphKeys[i]);

		if(pathGraph)
		{
			pathGraph->EnumeratePathNodes(segment, pathNodesOut);
		}
	}

	return TRUE;
}

CRuPathNode *CRuPathHierarchy_DS::FindAnyPathNode(const CRuSphere &sphere)
{
	// Enumerate keys of path graphs that intersect the sphere
	CRuArrayList<INT32> containingPathGraphKeys;
	m_graphKeyPartition->Enumerate(sphere, containingPathGraphKeys);

	// Iterate through all enumerated graphs
	for(INT32 i = 0; i < containingPathGraphKeys.Count(); ++i)
	{
		IRuPathGraph_Base *pathGraph = this->GetPathGraph(containingPathGraphKeys[i]);

		if(pathGraph)
		{
			// Enumerate all path nodes intersecting the sphere
			CRuArrayList<CRuPathNode *> pathNodes;
			pathGraph->EnumeratePathNodes(sphere, pathNodes);

			for(INT32 j = 0; j < pathNodes.Count(); ++j)
			{
				REAL dist = GetDistance(pathNodes[j]->NodeBounds(), sphere.Center());

				// Check against collision object
				if(m_pathRover)
				{
					if(m_pathRover->IsPathValid(sphere.Center(), pathNodes[j]->Centroid()) == FALSE)
					{
						continue;
					}
				}

				return pathNodes[j];
			}
		}
	}

	return NULL;
}

CRuPathNode *CRuPathHierarchy_DS::FindNearestPathNode(const CRuSphere &sphere)
{
	// Enumerate keys of path graphs that intersect the sphere
	CRuArrayList<INT32> containingPathGraphKeys;
	m_graphKeyPartition->Enumerate(sphere, containingPathGraphKeys);

	// Find the nearest path node
	IRuPathGraph_Base *nearestPathNodeGraph = NULL;
	CRuPathNode *nearestPathNode = NULL;
	REAL nearestDistance = FLT_MAX;

	for(INT32 i = 0; i < containingPathGraphKeys.Count(); ++i)
	{
		IRuPathGraph_Base *pathGraph = this->GetPathGraph(containingPathGraphKeys[i]);

		if(pathGraph)
		{
			// Enumerate all path nodes intersecting the sphere
			CRuArrayList<CRuPathNode *> pathNodes;
			pathGraph->EnumeratePathNodes(sphere, pathNodes);

			for(INT32 j = 0; j < pathNodes.Count(); ++j)
			{
				REAL dist = GetDistance(pathNodes[j]->NodeBounds(), sphere.Center());

				if(dist < nearestDistance)
				{
					// Check against collision object
					if(m_pathRover)
					{
						if(m_pathRover->IsPathValid(sphere.Center(), pathNodes[j]->Centroid()) == FALSE)
						{
							continue;
						}
					}

					// Store as current nearest path node
					nearestPathNodeGraph = pathGraph;
					nearestPathNode = pathNodes[j];
					nearestDistance = dist;
				}
			}
		}
	}

	if(nearestPathNodeGraph && nearestPathNode)
	{
		return nearestPathNode;
	}

	return NULL;
}

CRuPathNode *CRuPathHierarchy_DS::FindNearestPathNode(const CRuSegment3 &segment)
{
	// Enumerate keys of path graphs that intersect the segment
	CRuArrayList<INT32> containingPathGraphKeys;
	m_graphKeyPartition->Enumerate(segment, containingPathGraphKeys);

	// Find the nearest path node
	IRuPathGraph_Base *nearestPathNodeGraph = NULL;
	CRuPathNode *nearestPathNode = NULL;
	REAL nearestDistance = FLT_MAX;

	for(INT32 i = 0; i < containingPathGraphKeys.Count(); ++i)
	{
		IRuPathGraph_Base *pathGraph = this->GetPathGraph(containingPathGraphKeys[i]);

		if(pathGraph)
		{
			CRuPathNode *subPathNode;
			REAL subDistance;

			if(pathGraph->GetNearestPathNode(segment, &subPathNode, subDistance))
			{
				if(subDistance < nearestDistance)
				{
					nearestPathNodeGraph = pathGraph;
					nearestPathNode = subPathNode;
					nearestDistance = subDistance;
				}
			}
		}
	}

	if(nearestPathNodeGraph && nearestPathNode)
	{
		return nearestPathNode;
	}

	return NULL;
}

BOOL CRuPathHierarchy_DS::DeleteNearestPathNode(const CRuSegment3 &segment)
{
	// Enumerate keys of path graphs that intersect the segment
	CRuArrayList<INT32> containingPathGraphKeys;
	m_graphKeyPartition->Enumerate(segment, containingPathGraphKeys);

	// Find the nearest path node
	IRuPathGraph_Base *nearestPathNodeGraph = NULL;
	CRuPathNode *nearestPathNode = NULL;
	REAL nearestDistance = FLT_MAX;

	for(INT32 i = 0; i < containingPathGraphKeys.Count(); ++i)
	{
		IRuPathGraph_Base *pathGraph = this->GetPathGraph(containingPathGraphKeys[i]);

		if(pathGraph)
		{
			CRuPathNode *subPathNode;
			REAL subDistance;

			if(pathGraph->GetNearestPathNode(segment, &subPathNode, subDistance))
			{
				if(subDistance < nearestDistance)
				{
					nearestPathNodeGraph = pathGraph;
					nearestPathNode = subPathNode;
					nearestDistance = subDistance;
				}
			}
		}
	}

	// Delete the nearest path node
	if(nearestPathNodeGraph && nearestPathNode)
	{
		BOOL retVal = nearestPathNodeGraph->DeletePathNode(nearestPathNode);

		// Rebuild internal path node links
		nearestPathNodeGraph->BuildPathNodeLinks();

		// Re-connect neighboring graphs
		ConnectPathGraphAndNeighbors(nearestPathNodeGraph);

		// Mark graph as dirty
		this->MarkPathGraphAsDirty(nearestPathNodeGraph);

		return retVal;
	}

	return FALSE;
}

BOOL CRuPathHierarchy_DS::MarkNearestPathNode(const CRuSegment3 &segment)
{
	// Enumerate keys of path graphs that intersect the segment
	CRuArrayList<INT32> containingPathGraphKeys;
	m_graphKeyPartition->Enumerate(segment, containingPathGraphKeys);

	// Find the nearest path node
	IRuPathGraph_Base *nearestPathNodeGraph = NULL;
	CRuPathNode *nearestPathNode = NULL;
	REAL nearestDistance = FLT_MAX;

	for(INT32 i = 0; i < containingPathGraphKeys.Count(); ++i)
	{
		IRuPathGraph_Base *pathGraph = this->GetPathGraph(containingPathGraphKeys[i]);

		if(pathGraph)
		{
			CRuPathNode *subPathNode;
			REAL subDistance;

			if(pathGraph->GetNearestPathNode(segment, &subPathNode, subDistance))
			{
				if(subDistance < nearestDistance)
				{
					nearestPathNodeGraph = pathGraph;
					nearestPathNode = subPathNode;
					nearestDistance = subDistance;
				}
			}
		}
	}

	if(nearestPathNodeGraph && nearestPathNode)
	{
		nearestPathNodeGraph->MarkPathNode(nearestPathNode, 0xFFFF9900);

		for(INT32 i = 0; i < nearestPathNode->GetNumNeighbors(); ++i)
		{
			nearestPathNodeGraph->MarkPathNode(nearestPathNode->GetNeighborNode(i), 0xFFFFFF00);
		}

		return TRUE;
	}

	return FALSE;
}

void CRuPathHierarchy_DS::UnmarkAllPathNodes()
{
	if(m_graphKeyPartition)
	{
		m_graphKeyPartition->ForAllElements(boost::bind(&CRuPathHierarchy_DS::UnmarkAllPathNodesCallback, this, _1));
	}
}

BOOL CRuPathHierarchy_DS::PreLoadPathGraphs(const CRuAABB &bounds)
{
	// Enumerate keys of path graphs that intersect the bounding box
	CRuArrayList<INT32> preloadPathGraphKeys;
	m_graphKeyPartition->Enumerate(bounds, preloadPathGraphKeys);

	// Iterate and load each path graph
	for(INT32 i = 0; i < preloadPathGraphKeys.Count(); ++i)
	{
		IRuPathGraph_Base *pathGraph = this->GetPathGraph(preloadPathGraphKeys[i]);
	}

	return TRUE;
}

BOOL CRuPathHierarchy_DS::FindPath(CRuPathNode *startNode, CRuPathNode *endNode, CRuArrayList<CRuPathNode *> &pathOut)
{
	if(m_pathFinder == NULL)
	{
		m_pathFinder = ruNEW CRuPathFinder_AStar();
	}

	return m_pathFinder->FindPath(startNode, endNode, pathOut);
}

BOOL CRuPathHierarchy_DS::FindPath(const CRuVector3 &p0, const CRuVector3 &p1, CRuArrayList<CRuVector3> &pathOut)
{
	CRuPathNode *p0Node = FindPathNodeAt(p0);
	CRuPathNode *p1Node = FindPathNodeAt(p1);

	if(p0Node == NULL)
	{
		CRuSphere ptSph(p0, 40.0f);
		p0Node = FindAnyPathNode(ptSph);
	}

	if(p1Node == NULL)
	{
		CRuSphere ptSph(p1, 40.0f);
		p1Node = FindAnyPathNode(ptSph);
	}

	if(p0Node && p1Node)
	{
		CRuArrayList<CRuPathNode *> path(1024);

//		DWORD t0 = timeGetTime();

		if(FindPath(p0Node, p1Node, path))
		{
			// Push start onto output
			pathOut.Add(p0);

			// For each node in the path, find closest points to each section and smooth out the path
			CRuVector3 curP = p0;

			for(INT32 i = 0; i < path.Count() - 1; ++i)
			{
				CRuVector3 xPoint;
				if(path[i]->FindClosestCrossingPoint(path[i + 1], curP, xPoint))
				{
					pathOut.Add(xPoint);

					curP = xPoint;
				}
				else
				{
					break;
				}
			}

			// Push end onto output
			pathOut.Add(p1);

/*
			DWORD t1 = timeGetTime();

			DWORD d = t1 - t0;

			char moo[200];
			sprintf(moo, "%u", d);
			MessageBox(NULL, moo, moo, MB_OK);
*/

			return TRUE;
		}
		else
		{
		}
	}

	// Set a default straight-line path in the event that no path was found
	pathOut.Clear();
	pathOut.Add(p1);

	// Return false to indicate path finding failure
	return FALSE;
}

BOOL CRuPathHierarchy_DS::ClearGraphQueue()
{
	if(m_graphQueue)
	{
		m_graphQueue->ForAllElements(ClearGraphQueueCallback, this);
		m_graphQueue->Clear();
	}

	return TRUE;
}

BOOL CRuPathHierarchy_DS::ClearGraphQueueCallback(const void *key, const void *data, void *userData)
{
	IRuPathGraph_Base *graph = *((IRuPathGraph_Base **) data);
	ruSAFE_RELEASE(graph);

	return TRUE;
}

IRuPathGraph_Base *CRuPathHierarchy_DS::GetExistingPathGraph(INT32 graphKey)
{
	IRuPathGraph_Base *pathGraph = NULL;
	DWORD graphKeyHash = RuGlobalHashGen().GetHashCode(graphKey);

	// Attempt to locate the specified graph in the graph LRU queue
	if(m_graphQueue->Get(graphKey, graphKeyHash, pathGraph))
	{
		return pathGraph;
	}

	// Attempt to locate the specified graph in the uncommitted graph list
	for(INT32 i = 0; i < m_uncommittedGraphs.Count(); ++i)
	{
		if(m_uncommittedGraphs[i]->GetGraphKey() == graphKey)
		{
			pathGraph = m_uncommittedGraphs[i];
			pathGraph->AddRef();
		}
	}

	return pathGraph;
}

void CRuPathHierarchy_DS::ConnectPathGraph(IRuPathGraph_Base *graph)
{
	// Enumerate keys of neighboring path graphs
	CRuArrayList<INT32> neighboringGraphKeys(16);

	CRuAABB bufferedBounds = graph->Bounds();
	bufferedBounds.Minima() -= m_connectiveEpsilon;
	bufferedBounds.Maxima() += m_connectiveEpsilon;

	m_graphKeyPartition->Enumerate(bufferedBounds, neighboringGraphKeys);

	// Iterate through each neighboring path graph
	for(INT32 i = 0; i < neighboringGraphKeys.Count(); ++i)
	{
		// Get existing instanced patch graph
		IRuPathGraph_Base *neighborGraph = GetExistingPathGraph(neighboringGraphKeys[i]);

		if(neighborGraph && neighborGraph->GetGraphKey() != graph->GetGraphKey())
		{
			// Enumerate graph nodes which can potentially link with the neighbor graph
			CRuArrayList<CRuPathNode *> candidateNodes;

			CRuAABB bufferedNeighborBounds = neighborGraph->Bounds();
			bufferedNeighborBounds.Minima() -= m_connectiveEpsilon;
			bufferedNeighborBounds.Maxima() += m_connectiveEpsilon;

			graph->EnumeratePathNodes(bufferedNeighborBounds, candidateNodes);

			// Iterate through each candidate node
			for(INT32 j = 0; j < candidateNodes.Count(); ++j)
			{
				// Enumerate intersecting path nodes from the neighbor graph
				CRuArrayList<CRuPathNode *> neighborPathNodes;

				CRuAABB bufferedNodeBounds = candidateNodes[j]->NodeBounds();
				bufferedNodeBounds.Minima() -= m_connectiveEpsilon;
				bufferedNodeBounds.Maxima() += m_connectiveEpsilon;

				neighborGraph->EnumeratePathNodes(bufferedNodeBounds, neighborPathNodes);

				// Connect intersecting path nodes
				for(INT32 k = 0; k < neighborPathNodes.Count(); ++k)
				{
					// One-way transient connection
					candidateNodes[j]->Connect(neighborPathNodes[k], TRUE, m_connectiveEpsilon);
				}
			}

			// Enumerate graph nodes from the neighbor which can potentially link with this graph
			candidateNodes.Clear();
			neighborGraph->EnumeratePathNodes(bufferedBounds, candidateNodes);

			// Iterate through each candidate node
			for(INT32 j = 0; j < candidateNodes.Count(); ++j)
			{
				// Enumerate intersecting path nodes from the graph
				CRuArrayList<CRuPathNode *> pathNodes;

				CRuAABB bufferedNodeBounds = candidateNodes[j]->NodeBounds();
				bufferedNodeBounds.Minima() -= m_connectiveEpsilon;
				bufferedNodeBounds.Maxima() += m_connectiveEpsilon;

				neighborGraph->EnumeratePathNodes(bufferedNodeBounds, pathNodes);

				// Connect intersecting path nodes
				for(INT32 k = 0; k < pathNodes.Count(); ++k)
				{
					// One-way transient connection
					pathNodes[k]->Connect(candidateNodes[j], TRUE);
				}
			}
		}
	}
}

void CRuPathHierarchy_DS::ConnectPathGraphAndNeighbors(IRuPathGraph_Base *graph)
{
	// Enumerate keys of neighboring path graphs
	CRuArrayList<INT32> neighboringGraphKeys(16);

	CRuAABB bufferedBounds = graph->Bounds();
	bufferedBounds.Minima() -= m_connectiveEpsilon;
	bufferedBounds.Maxima() += m_connectiveEpsilon;

	m_graphKeyPartition->Enumerate(bufferedBounds, neighboringGraphKeys);

	// Iterate through each neighboring path graph
	for(INT32 i = 0; i < neighboringGraphKeys.Count(); ++i)
	{
		// Get existing instanced patch graph
		IRuPathGraph_Base *neighborGraph = GetExistingPathGraph(neighboringGraphKeys[i]);

		if(neighborGraph && neighborGraph->GetGraphKey() != graph->GetGraphKey())
		{
			// Clear all transient path node links
			neighborGraph->ClearTransientPathNodeLinks();

			// Connect graph
			ConnectPathGraph(neighborGraph);
		}
	}

	// Clear all transient path node links
	graph->ClearTransientPathNodeLinks();
	ConnectPathGraph(graph);
}

BOOL CRuPathHierarchy_DS::UnmarkAllPathNodesCallback(INT32 graphKey)
{
	IRuPathGraph_Base *pathGraph = this->GetExistingPathGraph(graphKey);

	if(pathGraph)
	{
		pathGraph->UnmarkAllPathNodes();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
