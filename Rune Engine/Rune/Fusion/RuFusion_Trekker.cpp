#include "../Fusion/RuFusion_Trekker.h"

#include "../Core/DataStore/RuDataStore_Disk.h"
#include "../PathEngine/RuPathEngine_NodeBuilder.h"
#include "../PathEngine/RuPathGraph_AStar.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFusion_Trekker, IRuFusion_Trekker_Interface)
ruHEAP_DEFINE_SUBCLASS(CRuFusion_Trekker, "CRuFusion_Trekker", "IRuFusion_Trekker_Interface")
ruCLASSGUID_DEFINE(CRuFusion_Trekker, 0x4A4DA205, 0x3232483a, 0xA974E971, 0x9333854B)

// ************************************************************************************************************************************************************

CRuFusion_Trekker::CRuFusion_Trekker(const char *terrainPath)
:	m_defaultNodeSearchRadius(80.0f),
	m_defaultLocationSearchRange(20.0f),
	m_pathMaximumExpansion(1024),
	m_indPH_Hierarchy(NULL),
	m_runTime_PathFinder(NULL)
{
	// Open terrain data store
	CRuDataStore_Disk *mapFileDataStore = ruNEW CRuDataStore_Disk();
	mapFileDataStore->Open(terrainPath, TRUE);

	// Create terrain data source
	CRuOptimalTerrain_DataSource_DS *terrainDS = ruNEW CRuOptimalTerrain_DataSource_DS();

	// Enable collision-only mode
	terrainDS->SetCollisionOnlyMode(TRUE);

	if(terrainDS->Open(mapFileDataStore))
	{
		// Create new terrain using new data source
		m_terrain = ruNEW CRuOptimalTerrain();
		m_terrain->Initialize();
		m_terrain->AttachDataSource(terrainDS, TRUE);

		// Enable server mode for the object manager
		m_terrain->GetObjectManager()->EnableDistanceCulling(FALSE);

		// Setup simple path rover
		m_runTime_PathRover = ruNEW CRuPathRover_Simple();
		m_runTime_PathRover->EnableSimpleRoving(TRUE);
		m_runTime_PathRover->SetCollisionObject(m_terrain);

		// Setup internal path finder
		m_runTime_PathFinder = ruNEW CRuPathFinder_AStar();
		m_runTime_PathFinder->SetMaximumExpansions(m_pathMaximumExpansion);

		// Get path hierarchy from terrain object
		m_pathHierarchy = m_terrain->GetPathHierarchy();

		// Set path rover into the hierarchy object
		m_pathHierarchy->SetPathRover(m_runTime_PathRover);

		// Set path finder for the path hierarchy
		m_pathHierarchy->SetPathFinder(m_runTime_PathFinder);

		// Set path graph queue size
		if(m_pathHierarchy->GetType() == CRuPathHierarchy_DS::Type())
		{
			static_cast<CRuPathHierarchy_DS *>(m_pathHierarchy)->SetPathGraphQueueSize(2048);
		}
	}

	// Initialize special event filter to none
	SetSpecialEventFilter("none");
}

CRuFusion_Trekker::CRuFusion_Trekker(CRuOptimalTerrain *terrain)
{
	// Store reference to terrain
	m_terrain = terrain;
	m_terrain->AddRef();

	// Setup simple path rover
	m_runTime_PathRover = ruNEW CRuPathRover_Simple();
	m_runTime_PathRover->EnableSimpleRoving(TRUE);
	m_runTime_PathRover->SetCollisionObject(m_terrain);

	// Setup internal path finder
	m_runTime_PathFinder = ruNEW CRuPathFinder_AStar();
	m_runTime_PathFinder->SetMaximumExpansions(m_pathMaximumExpansion);

	// Get path hierarchy from terrain object
	m_pathHierarchy = m_terrain->GetPathHierarchy();

	// Set path rover into the hierarchy object
	m_pathHierarchy->SetPathRover(m_runTime_PathRover);

	// Set path finder for the path hierarchy
	m_pathHierarchy->SetPathFinder(m_runTime_PathFinder);

	// Set path graph queue size
	if(m_pathHierarchy->GetType() == CRuPathHierarchy_DS::Type())
	{
		static_cast<CRuPathHierarchy_DS *>(m_pathHierarchy)->SetPathGraphQueueSize(2048);
	}
}

CRuFusion_Trekker::~CRuFusion_Trekker()
{
	// Delete independent path hierarchy
	DeleteIndependentPathHierarchy();

	// Release terrain object
	ruSAFE_RELEASE(m_terrain);

	// Release path finder
	ruSAFE_RELEASE(m_runTime_PathFinder);
}

BOOL CRuFusion_Trekker::SetPathMaximumExpansion(INT32 pathMaximumExpansion)
{
	m_pathMaximumExpansion = pathMaximumExpansion;
	m_runTime_PathFinder->SetMaximumExpansions(m_pathMaximumExpansion);

	return TRUE;
}

BOOL CRuFusion_Trekker::SetSpecialEventFilter(const char *filterString)
{
	if(m_terrain)
	{
		CRuWorld_ObjectManager *objectManager = m_terrain->GetObjectManager();

		if(objectManager)
		{
			objectManager->SetSpecialEventFilter(filterString);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuFusion_Trekker::PreLoad(const CRuAABB &bounds)
{
	if(m_terrain && m_pathHierarchy)
	{
		// Force terrain object to load chunks for the specified region
		m_terrain->PreLoadChunks(bounds, ruOPTIMALTERRAINCHUNKLOADMODE_COLLISION);

		// Force path hierarchy to load path graphs for the specified region
		if(m_pathHierarchy->GetType() == CRuPathHierarchy_DS::Type())
		{
			static_cast<CRuPathHierarchy_DS *>(m_pathHierarchy)->PreLoadPathGraphs(bounds);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuFusion_Trekker::IsPathClear(const CRuVector3 &origin, const CRuVector3 &destination, BOOL ignoreBackface)
{
	// Setup collision query
	m_runTime_SegHitQuery.m_ignoreBackface = ignoreBackface;
	m_runTime_SegHitQuery.m_colSegment = CRuSegment3(origin, destination);		// Setup collision segment
	m_runTime_SegHitQuery.m_exclusionMask = 0x00A00000;							// Exclude water and foliage surfaces
	m_runTime_SegHitQuery.m_positiveHit = FALSE;								// Reset result

	// Query collision
	m_terrain->QueryCollision(&m_runTime_SegHitQuery);

	for(INT32 i = 0; i < m_indPH_AdditionalColObjs.Count(); ++i)
	{
		m_indPH_AdditionalColObjs[i]->QueryCollision(&m_runTime_SegHitQuery);
	}

	return (m_runTime_SegHitQuery.m_positiveHit == FALSE);
}

BOOL CRuFusion_Trekker::IsRestingPointValid(const CRuVector3 &origin)
{
	return TRUE;
}

BOOL CRuFusion_Trekker::FindRestingHeight(const CRuVector3 &origin, REAL testDistance, REAL &restingHeightOut)
{
	REAL nearestRestingHeightDistance = FLT_MAX;
	REAL nearestRestingHeight = origin.m_y;

	// Make downward query
	m_runTime_NearestSegQuery.m_colSegment = CRuSegment3(CRuVector3(origin.m_x, origin.m_y, origin.m_z), CRuVector3(origin.m_x, origin.m_y - testDistance, origin.m_z));		// Setup collision segment
	m_runTime_NearestSegQuery.m_exclusionMask = 0x00A00000;														// Exclude water and foliage surfaces
	m_runTime_NearestSegQuery.m_ignoreBackface = FALSE;
	m_runTime_NearestSegQuery.m_positiveHit = FALSE;															// Reset result
	m_runTime_NearestSegQuery.m_collisionT = FLT_MAX;

	// Query collision
	m_terrain->QueryCollision(&m_runTime_NearestSegQuery);

	if(m_runTime_NearestSegQuery.m_positiveHit && m_runTime_NearestSegQuery.m_collisionT < nearestRestingHeightDistance)
	{
		nearestRestingHeightDistance = m_runTime_NearestSegQuery.m_collisionT;
		nearestRestingHeight = m_runTime_NearestSegQuery.m_collisionPoint.m_y;
	}

	// Make upward query
	m_runTime_NearestSegQuery.m_colSegment = CRuSegment3(CRuVector3(origin.m_x, origin.m_y, origin.m_z), CRuVector3(origin.m_x, origin.m_y + testDistance, origin.m_z));			// Setup collision segment
	m_runTime_NearestSegQuery.m_exclusionMask = 0x00A00000;														// Exclude water and foliage surfaces
	m_runTime_NearestSegQuery.m_ignoreBackface = FALSE;
	m_runTime_NearestSegQuery.m_positiveHit = FALSE;															// Reset result
	m_runTime_NearestSegQuery.m_collisionT = FLT_MAX;

	// Query collision
	m_terrain->QueryCollision(&m_runTime_NearestSegQuery);

	if(m_runTime_NearestSegQuery.m_positiveHit && m_runTime_NearestSegQuery.m_collisionT < nearestRestingHeightDistance)
	{
		nearestRestingHeightDistance = m_runTime_NearestSegQuery.m_collisionT;
		nearestRestingHeight = m_runTime_NearestSegQuery.m_collisionPoint.m_y;
	}

	// Store result
	restingHeightOut = nearestRestingHeight;

	return nearestRestingHeightDistance < FLT_MAX;
}

BOOL CRuFusion_Trekker::FindNeighborPoints(const CRuVector3 &origin, REAL radius, REAL testDistance, REAL heightOffset, INT32 divisions, CRuArrayList<CRuVector3> *neighborPointsOut)
{
	// Build step-by-step rotation quaternion
	REAL stepRotAngle = ruPI * 2.0f / divisions;
	CRuQuaternion stepRotQuat;
	stepRotQuat.FromAngleAxis(CRuVector3(0.0f, 1.0f, 0.0f), stepRotAngle);

	// Make list of potential neighbor points
	CRuVector3 neighborPoint;
	CRuVector3 neighborPointOffset(0.0f, 0.0f, radius);

	for(INT32 i = 0; i < divisions; ++i)
	{
		// Calculate world position of neighbor point
		neighborPoint = neighborPointOffset + origin;

		// Determine whether the point is OK for resting
		REAL restingHeight = 0.0f;
		if(this->FindRestingHeight(neighborPoint, testDistance, restingHeight))
		{
			// Save resting height
			neighborPoint.m_y = restingHeight;

			// Determine straight-line visibility
			if(this->IsPathClear(CRuVector3(origin.m_x, origin.m_y + heightOffset, origin.m_z), CRuVector3(neighborPoint.m_x, neighborPoint.m_y + heightOffset, neighborPoint.m_z)))
			{
				neighborPointsOut->Add(neighborPoint);
			}
		}

		// Rotate neighbor point offset
		stepRotQuat.TransformVector(neighborPointOffset);
	}

	return TRUE;
}

PTRVALUE CRuFusion_Trekker::FindPathNode(const CRuVector3 &point, REAL searchRadius)
{
	if(m_pathHierarchy)
	{
		return reinterpret_cast<PTRVALUE>(m_pathHierarchy->FindPathNodeAt(point, searchRadius));
	}

	return 0;
}

BOOL CRuFusion_Trekker::FindPath(const PTRVALUE origin, const PTRVALUE destination, CRuArrayList<PTRVALUE> *pathNodeOut)
{
	if(m_pathHierarchy && origin && destination)
	{
		// Clear path nodes
		pathNodeOut->Clear();
		m_runTime_PathNodes.Clear();

		// Perform path query
		BOOL retVal = m_pathHierarchy->FindPath(reinterpret_cast<CRuPathNode *>(origin), reinterpret_cast<CRuPathNode *>(destination), m_runTime_PathNodes);

		// Copy data
		for(INT32 i = 0; i < m_runTime_PathNodes.Count(); ++i)
		{
			pathNodeOut->Add(reinterpret_cast<PTRVALUE>(m_runTime_PathNodes[i]));
		}

		return retVal;
	}

	return FALSE;
}

BOOL CRuFusion_Trekker::FindPath(const CRuVector3 &origin, const CRuVector3 &destination, CRuArrayList<CRuVector3> *pathPositionOut)
{
	if(m_pathHierarchy)
	{
		return m_pathHierarchy->FindPath(origin, destination, *pathPositionOut);
	}

	return FALSE;
}

BOOL CRuFusion_Trekker::CalculatePathPoints(const CRuVector3 &p0, const CRuVector3 &p1, CRuArrayList<PTRVALUE> *pathNodes, CRuArrayList<CRuVector3> *pathPositionOut)
{
	// Clear output array
	pathPositionOut->Clear();

	// Push start onto output
	pathPositionOut->Add(p0);

	// For each node in the path, find closest points to each section and smooth out the path
	CRuVector3 curP = p0;

	for(INT32 i = 0; i < pathNodes->Count() - 1; ++i)
	{
		CRuVector3 xPoint;
		reinterpret_cast<CRuPathNode *>(pathNodes->Get(i))->FindClosestCrossingPoint(reinterpret_cast<CRuPathNode *>(pathNodes->Get(i + 1)), curP, xPoint);
		pathPositionOut->Add(xPoint);

		curP = xPoint;
	}

	// Push end onto output
	pathPositionOut->Add(p1);

	return TRUE;
}

BOOL CRuFusion_Trekker::CreateIndependentPathHierarchy()
{
	if(m_terrain->GetPathHierarchy()->GetType() == CRuPathHierarchy_DS::Type())
	{
		// Form the temporary file path and store it in m_indPH_Filename
		char tempPath[MAX_PATH];
		GetTempPathA(MAX_PATH, tempPath);
		GetTempFileName(tempPath, "trk", 0, m_indPH_Filename);

		// Create the file
		CRuDataStore_Disk *pathHierarachyDS = ruNEW CRuDataStore_Disk();
		pathHierarachyDS->Create(m_indPH_Filename);

		// Copy all existing data from the terrain's path hierarchy
		static_cast<CRuPathHierarchy_DS *>(m_terrain->GetPathHierarchy())->CopyTo(pathHierarachyDS);

		// Create the path hierarchy
		m_indPH_Hierarchy = ruNEW CRuPathHierarchy_DS();
		m_indPH_Hierarchy->Bind(pathHierarachyDS, m_terrain->GetTerrainBounds());

		// Release local reference to the data store
		ruSAFE_RELEASE(pathHierarachyDS);

		// Set as the default path hierarchy
		m_pathHierarchy = m_indPH_Hierarchy;

		// Set path rover into the hierarchy object
		m_pathHierarchy->SetPathRover(m_runTime_PathRover);

		// Set path finder for the path hierarchy
		m_pathHierarchy->SetPathFinder(m_runTime_PathFinder);

		// Set path graph queue size
		if(m_pathHierarchy->GetType() == CRuPathHierarchy_DS::Type())
		{
			static_cast<CRuPathHierarchy_DS *>(m_pathHierarchy)->SetPathGraphQueueSize(2048);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuFusion_Trekker::DeleteIndependentPathHierarchy()
{
	if(m_indPH_Hierarchy)
	{
		// Release the hierarchy
		ruSAFE_RELEASE(m_indPH_Hierarchy);

		// Delete the file
		DeleteFile(m_indPH_Filename);

		// Restore original path hierarchy
		m_pathHierarchy = m_terrain->GetPathHierarchy();
	}

	return TRUE;
}

void CRuFusion_Trekker::InsertModel(CRuEntity *model)
{
	m_indPH_AdditionalColObjs.Add(model);
}

void CRuFusion_Trekker::UpdateIndependentPathHierarchy()
{
	IRuOptimalTerrain_DataSource *terrainDS = m_terrain->GetDataSource();
	REAL chunkGridSpacing = terrainDS->GetChunkDimension() * terrainDS->GetGridSize();

	CRuAABB terrainBounds = m_terrain->GetTerrainBounds();

	CRuKeyHash<UINT32> processedHash(256);

	// Work out all cells that require rebuild
	for(INT32 i = 0; i < m_indPH_AdditionalColObjs.Count(); ++i)
	{
		// Get object bounds
		CRuSphere objBounds = m_indPH_AdditionalColObjs[i]->GetWorldBounds();

		// Get object bounds relative to minimum point on terrain
		objBounds.Center() = objBounds.Center() - terrainBounds.Minima();

		CRuVector3 minima = objBounds.Center() - objBounds.Radius();
		CRuVector3 maxima = objBounds.Center() + objBounds.Radius();

		// Clamp minima
		minima.m_x = max(0.0f, minima.m_x);
		minima.m_z = max(0.0f, minima.m_z);

		// Clamp maxima
		maxima.m_x = min(terrainBounds.Maxima().m_x - terrainBounds.Minima().m_x, maxima.m_x);
		maxima.m_z = min(terrainBounds.Maxima().m_z - terrainBounds.Minima().m_z, maxima.m_z);

		INT32 iMinX = (INT32) floor(minima.m_x / chunkGridSpacing);
		INT32 iMinZ = (INT32) floor(minima.m_z / chunkGridSpacing);

		INT32 iMaxX = (INT32) floor(maxima.m_x / chunkGridSpacing);
		INT32 iMaxZ = (INT32) floor(maxima.m_z / chunkGridSpacing);

		for(INT32 z = iMinZ; z <= iMaxZ; ++z)
		{
			for(INT32 x = iMinX; x <= iMaxX; ++x)
			{
				UINT32 chunkKey = (x << 16) | z;

				if(processedHash.ContainsKey(chunkKey) == FALSE)
				{
					// Build sector bounds
					REAL maxVertical = 10000.0f;
					CRuAABB sectorBounds	(
												x * chunkGridSpacing + terrainBounds.Minima().m_x, -maxVertical, z * chunkGridSpacing + terrainBounds.Minima().m_z,
												(x + 1) * chunkGridSpacing + terrainBounds.Minima().m_x, maxVertical, (z + 1) * chunkGridSpacing + terrainBounds.Minima().m_z
											);

					// Construct new path graph
					CRuPathGraph_AStar *pathGraph = ruNEW CRuPathGraph_AStar(sectorBounds);

					// Set graph key
					INT32 graphKey = RuPathEngine_MakeGraphKey(x, z);
					pathGraph->SetGraphKey(graphKey);
					pathGraph->SetCollisionObject(m_terrain);

					// New node builder
					CRuPathEngine_NodeBuilder *path_NodeBuilder = ruNEW CRuPathEngine_NodeBuilder();
					path_NodeBuilder->Initialize(sectorBounds, 8.0f);
					path_NodeBuilder->SetTriangleExclusionMask(RuWorld::ruTRIFLAGS_WATER | RuWorld::ruTRIFLAGS_FOLIAGE);

					// Append geometry to node builder
					path_NodeBuilder->AppendGeometry(m_terrain);

					for(INT32 j = 0; j < m_indPH_AdditionalColObjs.Count(); ++j)
					{
						path_NodeBuilder->AppendGeometry(m_indPH_AdditionalColObjs[j]);
					}

					path_NodeBuilder->ClassifyGeometry(CRuVector3(0.0f, 1.0f, 0.0f), 0.5f);
					path_NodeBuilder->TessellateGeometry();
					path_NodeBuilder->GenerateCandidatePoints();
					path_NodeBuilder->GeneratePathNode(pathGraph);

					ruSAFE_RELEASE(path_NodeBuilder);

					// Insert path graph into the path hierarchy
					pathGraph->BuildPathNodeLinks();
					m_indPH_Hierarchy->InsertPathGraph(pathGraph);

					// Release local reference to the path graph
					ruSAFE_RELEASE(pathGraph);

					// Add to processed hash
					processedHash.Add(chunkKey);
				}
			}
		}
	}



/*
	// Get data source underlying the terrain
	IRuTerrain_DataSource *terrainDS = m_terrain->GetDataSource();

	// Determine origin of the sector bounding box
	REAL chunkGridSpacing = terrainDS->GetChunkDimension() * terrainDS->GetGridSize();
	CRuVector3 relativeGenPoint = segQuery.m_collisionPoint - terrainDS->GetTerrainBounds().Minima();
	INT32 iX = (INT32) floor(relativeGenPoint.x / chunkGridSpacing);
	INT32 iZ = (INT32) floor(relativeGenPoint.z / chunkGridSpacing);

	relativeGenPoint.x = iX * chunkGridSpacing + terrainDS->GetTerrainBounds().Minima().x;
	relativeGenPoint.z = iZ * chunkGridSpacing + terrainDS->GetTerrainBounds().Minima().z;

	// Calculate sector bounding box
	float maxVertical = 10000.0f;
	CRuAABB sectorBounds(relativeGenPoint.x, -maxVertical, relativeGenPoint.z, relativeGenPoint.x + chunkGridSpacing, maxVertical, relativeGenPoint.z + chunkGridSpacing);

	// Generate path block for the sector
	GenerateBlock(sectorBounds, iX, iZ);
*/
}





/*
	void CrEditor_MapEx::GenerateBlock(const CRuAABB &sectorBounds, INT32 iX, INT32 iZ)
	{
		// Construct new path graph
		CRuPathGraph_AStar *pathGraph = ruNEW CRuPathGraph_AStar(sectorBounds);
//		pathGraph->SetPathRover(m_pathRover);

		// Set graph key
		INT32 graphKey = RuPathEngine_MakeGraphKey(iX, iZ);
		pathGraph->SetGraphKey(graphKey);
		pathGraph->SetCollisionObject(m_terrain);


		DWORD t0, t1, et;
		char outputStr[2048];

		// Release old node builder
		ruSAFE_RELEASE(m_path_NodeBuilder);

		// New node builder
		m_path_NodeBuilder = ruNEW CRuPathEngine_NodeBuilder();
		m_path_NodeBuilder->Initialize(sectorBounds, 8.0f);
		m_path_NodeBuilder->SetTriangleExclusionMask(RuWorld::ruTRIFLAGS_WATER | RuWorld::ruTRIFLAGS_FOLIAGE);

		// Append geometry to node builder
		m_path_NodeBuilder->AppendGeometry(m_terrain);

		t0 = timeGetTime();
		m_path_NodeBuilder->ClassifyGeometry(CRuVector3(0.0f, 1.0f, 0.0f), 0.5f);
		t1 = timeGetTime();
		et = t1 - t0;
		sprintf(outputStr, "Classify: %d\n", et);
		OutputDebugString(outputStr);

		t0 = timeGetTime();
		m_path_NodeBuilder->TessellateGeometry();
		t1 = timeGetTime();
		et = t1 - t0;
		sprintf(outputStr, "Tessellate: %d\n", et);
		OutputDebugString(outputStr);

		t0 = timeGetTime();
		m_path_NodeBuilder->GenerateCandidatePoints();
		t1 = timeGetTime();
		et = t1 - t0;
		sprintf(outputStr, "Generate Candidate: %d\n", et);
		OutputDebugString(outputStr);

		t0 = timeGetTime();
		m_path_NodeBuilder->GeneratePathNode(pathGraph);
		t1 = timeGetTime();
		et = t1 - t0;
		sprintf(outputStr, "Generate Path Node: %d\n", et);
		OutputDebugString(outputStr);

		ruSAFE_RELEASE(m_path_NodeBuilder);


		// Build path node links
		t0 = timeGetTime();
		pathGraph->BuildPathNodeLinks();
		t1 = timeGetTime();
		et = t1 - t0;
		sprintf(outputStr, "Path Node Link: %d\n", et);
		OutputDebugString(outputStr);




		// Insert path graph into the path hierarchy
		IRuPathHierarchy_Base *pathHierarchy = m_terrain->GetPathHierarchy();
		pathHierarchy->InsertPathGraph(pathGraph);

		// Release local reference to the path graph
		ruSAFE_RELEASE(pathGraph);
	}
*/










IRuObject *CRuFusion_Trekker::GetTerrain()
{
	return m_terrain;
}

BOOL CRuFusion_Trekker::GetMapBounds(CRuAABB *mapBoundsOut)
{
	*mapBoundsOut = m_terrain->GetTerrainBounds();

	return TRUE;
}

const char *CRuFusion_Trekker::FindZoneName(const CRuVector3 &point)
{
	CRuWorld_ZoneDescriptor *zoneDescriptor = m_terrain->GetZoneDescriptor(point);

	if(zoneDescriptor)
	{
		return zoneDescriptor->GetName();
	}

	return "";
}

BOOL CRuFusion_Trekker::FindZoneRespawnPoint(const CRuVector3 &point, CRuVector3 &respawnPointOut)
{
	CRuWorld_ZoneDescriptor *zoneDescriptor = m_terrain->GetZoneDescriptor(point);

	while(zoneDescriptor)
	{
		// Get current zone's respawn point
		respawnPointOut = zoneDescriptor->GetRespawnPoint();

		// If the respawn point is valid, return
		if(fabs(respawnPointOut.m_x) > 0.0f || fabs(respawnPointOut.m_y) > 0.0f || fabs(respawnPointOut.m_z) > 0.0f)
		{
			return TRUE;
		}

		// Otherwise chain up to parent
		zoneDescriptor = m_terrain->GetZoneDescriptorByID(zoneDescriptor->GetParentZoneID());
	}

	return FALSE;
}

BOOL CRuFusion_Trekker::FindMaximalZoneControlBounds(INT32 zoneID, CRuAABB *controlBoundsOut)
{
	// Enumerate all sub-zones belonging to the specified zone ID
	CRuArrayList<CRuWorld_ZoneDescriptor *> zoneDescriptors;
	EnumerateSubZones(zoneID, &zoneDescriptors);

	// Enumerate all triggers with the specified zone ID and all of its sub-zones
	CRuArrayList<CRuWorld_Trigger *> triggers;
	for(INT32 i = 0; i < zoneDescriptors.Count(); ++i)
	{
		m_terrain->EnumerateAllTriggersByZoneID(zoneDescriptors[i]->GetZoneID(), triggers);
	}

	// Build maximal bounding volume
	CRuAABB maximalBounds;
	for(INT32 i = 0; i < triggers.Count(); ++i)
	{
		CRuOBB obb = triggers[i]->GetGizmo()->GetBox();
		obb.Transform(triggers[i]->GetGizmo()->GetWorldTransform());
		CRuAABB aabb(obb);

		if(i == 0)
		{
			maximalBounds = aabb;
		}
		else
		{
			maximalBounds.Merge(aabb);
		}
	}

	// Store output bounds
	*controlBoundsOut = maximalBounds;

	return triggers.Count() > 0;
}

BOOL CRuFusion_Trekker::FindZoneControlRects(INT32 zoneID, REAL controlRectSize, CRuArrayList<CRuAABB> *controlRectsOut)
{
	// Enumerate all sub-zones belonging to the specified zone ID
	CRuArrayList<CRuWorld_ZoneDescriptor *> zoneDescriptors;
	EnumerateSubZones(zoneID, &zoneDescriptors);

	// Place all sub-zone IDs into one array list
	CRuArrayList<INT32> zoneIDList;
	for(INT32 i = 0; i < zoneDescriptors.Count(); ++i)
	{
		zoneIDList.Add(zoneDescriptors[i]->GetZoneID());
	}

	// Enumerate all triggers with the specified zone ID and all of its sub-zones
	CRuArrayList<CRuWorld_Trigger *> triggers;
	m_terrain->EnumerateAllTriggersByZoneID(zoneIDList, triggers);
/*
	for(INT32 i = 0; i < zoneDescriptors.Count(); ++i)
	{
		m_terrain->EnumerateAllTriggersByZoneID(zoneDescriptors[i]->GetZoneID(), triggers);
	}
*/
	// Build maximal bounding volume
	CRuAABB maximalBounds;
	for(INT32 i = 0; i < triggers.Count(); ++i)
	{
		CRuOBB obb = triggers[i]->GetGizmo()->GetBox();
		obb.Transform(triggers[i]->GetGizmo()->GetWorldTransform());
		CRuAABB aabb(obb);

		if(i == 0)
		{
			maximalBounds = aabb;
		}
		else
		{
			maximalBounds.Merge(aabb);
		}
	}

	// Adjust maximal bounds such that it is a multiple of the control rectangle size
	for(INT32 i = 0; i < 3; ++i)
	{
		maximalBounds.Minima()[i] = (maximalBounds.Minima()[i] >= 0.0f) ? ceil(maximalBounds.Minima()[i] / controlRectSize) : floor(maximalBounds.Minima()[i] / controlRectSize);
		maximalBounds.Maxima()[i] = (maximalBounds.Maxima()[i] >= 0.0f) ? ceil(maximalBounds.Maxima()[i] / controlRectSize) : floor(maximalBounds.Maxima()[i] / controlRectSize);

		maximalBounds.Minima()[i] = maximalBounds.Minima()[i] * controlRectSize;
		maximalBounds.Maxima()[i] = maximalBounds.Maxima()[i] * controlRectSize;
	}

	// Split maximal bounding volume into control rectangles
	for(REAL z = maximalBounds.Minima().m_z; z < maximalBounds.Maxima().m_z; z += controlRectSize)
	{
		for(REAL x = maximalBounds.Minima().m_x; x < maximalBounds.Maxima().m_x; x += controlRectSize)
		{
			// Initialize the sub control volume
			CRuAABB controlRect(x, -ruMAXFLOATRANGE, z, x + controlRectSize, ruMAXFLOATRANGE, z + controlRectSize);

			// Ensure that the volume intersects with one of the triggers
			for(INT32 i = 0; i < triggers.Count(); ++i)
			{
				CRuOBB obb = triggers[i]->GetGizmo()->GetBox();
				obb.Transform(triggers[i]->GetGizmo()->GetWorldTransform());

				if(Intersects(obb, controlRect))
				{
					controlRectsOut->Add(controlRect);
					break;
				}
			}
		}
	}

	// Free enumerated triggers
	for(INT32 i = 0; i < triggers.Count(); ++i)
	{
		ruSAFE_RELEASE(triggers[i]);
	}

	return TRUE;
}

INT32 CRuFusion_Trekker::FindRootZoneID(const CRuVector3 &point)
{
	CRuWorld_ZoneDescriptor *zoneDescriptor = m_terrain->GetZoneDescriptor(point);

	CRuWorld_ZoneDescriptorContainer *zoneDescContainer = m_terrain->GetZoneDescriptorContainer();

	if(zoneDescContainer)
	{
		while(zoneDescriptor)
		{
			if(zoneDescriptor->GetParentZoneID() < 0)
			{
				return zoneDescriptor->GetZoneID();
			}

			zoneDescriptor = zoneDescContainer->GetZoneDescriptor(zoneDescriptor->GetParentZoneID());
		}
	}

	return -1;
}

INT32 CRuFusion_Trekker::FindZoneID(const CRuVector3 &point)
{
	CRuWorld_ZoneDescriptor *zoneDescriptor = m_terrain->GetZoneDescriptor(point);

	if(zoneDescriptor)
	{
		return zoneDescriptor->GetZoneID();
	}

	return -1;
}

BOOL CRuFusion_Trekker::EnumerateRootZones(CRuArrayList<CRuWorld_ZoneDescriptor *> *zoneDescriptorsOut)
{
	CRuWorld_ZoneDescriptorContainer *zoneDescContainer = m_terrain->GetZoneDescriptorContainer();

	if(zoneDescContainer)
	{
		// Clear output array
		zoneDescriptorsOut->Clear();

		INT32 zoneDescCount = zoneDescContainer->GetNumZoneDescriptors();

		for(INT32 i = 0; i < zoneDescCount; ++i)
		{
			CRuWorld_ZoneDescriptor *zoneDesc = zoneDescContainer->GetZoneDescriptorByIndex(i);

			if(zoneDesc)
			{
				// Make sure the zone is a root zone
				if(zoneDesc->GetParentZoneID() == -1)
				{
					zoneDescriptorsOut->Add(zoneDesc);
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuFusion_Trekker::EnumerateSubZones(INT32 rootZoneID, CRuArrayList<CRuWorld_ZoneDescriptor *> *zoneDescriptorsOut)
{
	CRuWorld_ZoneDescriptorContainer *zoneDescContainer = m_terrain->GetZoneDescriptorContainer();

	if(zoneDescContainer)
	{
		// Clear output array
		zoneDescriptorsOut->Clear();

		// Retrieve root zone descriptor and append to output list
		CRuWorld_ZoneDescriptor *rootZoneDesc = zoneDescContainer->GetZoneDescriptor(rootZoneID);
		if(rootZoneDesc)
		{
			zoneDescriptorsOut->Add(rootZoneDesc);
		}

		// Enumerate through all zone descriptors
		for(INT32 i = 0, zoneDescCount = zoneDescContainer->GetNumZoneDescriptors(); i < zoneDescCount; ++i)
		{
			CRuWorld_ZoneDescriptor *zoneDesc = zoneDescContainer->GetZoneDescriptorByIndex(i);

			if(zoneDesc)
			{
				// Determine if the current zone descriptor is a descendent of the specified root zone
				CRuWorld_ZoneDescriptor *curZoneDesc = zoneDesc;

				while(curZoneDesc)
				{
					if(curZoneDesc->GetParentZoneID() == rootZoneID)
					{
						zoneDescriptorsOut->Add(zoneDesc);
						break;
					}

					curZoneDesc = zoneDescContainer->GetZoneDescriptor(curZoneDesc->GetParentZoneID());
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

RuTrekkerZoneType CRuFusion_Trekker::GetZoneType(const CRuVector3 &point, REAL *damageFactorOut)
{
	RuTrekkerZoneType zoneType = ruTREKKERZONETYPE_NORMAL;
	RuWorld::TriggerType allowableTriggerTypes[] = { RuWorld::ruTRIGGERTYPE_DEADZONE, RuWorld::ruTRIGGERTYPE_SAFEZONE, RuWorld::ruTRIGGERTYPE_FORCE_DWORD };

	CRuWorld_Trigger *trigger = m_terrain->GetApplicableTrigger(allowableTriggerTypes, point, TRUE);

	if(trigger)
	{
		switch(trigger->GetTriggerType())
		{
			case RuWorld::ruTRIGGERTYPE_DEADZONE:
				{
					zoneType = ruTREKKERZONETYPE_DEADZONE;

					// Check for immediate death
				}

				break;

			case RuWorld::ruTRIGGERTYPE_SAFEZONE:
				{
					zoneType = ruTREKKERZONETYPE_NORMAL;
				}

				break;
		}
	}

	// Check against water bodies
	CRuWorld_WaterBodyDescriptor *waterBodyDesc = m_terrain->GetWaterBodyDescriptorAtPoint(point);

	if(waterBodyDesc)
	{
		RuPropertyType propertyValue;
		if(waterBodyDesc->GetProperty("waterDesc:damage factor", propertyValue))
		{
			REAL damageFactor = boost::get<REAL>(propertyValue);
			if(damageFactor != 0.0f)
			{
				zoneType = ruTREKKERZONETYPE_DAMAGEZONE;

				if(damageFactorOut)
				{
					*damageFactorOut = damageFactor;
				}
			}
		}
	}

	return zoneType;
}

BOOL CRuFusion_Trekker::IsDeadZone(const CRuVector3 &point)
{
	CRuWorld_Trigger *deadZoneTrigger = m_terrain->GetApplicableTrigger(RuWorld::ruTRIGGERTYPE_DEADZONE, point, TRUE);

	if(deadZoneTrigger)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CRuFusion_Trekker::IsSafeZone(const CRuVector3 &point)
{
	CRuWorld_Trigger *safeZoneTrigger = m_terrain->GetApplicableTrigger(RuWorld::ruTRIGGERTYPE_SAFEZONE, point, TRUE);

	if(safeZoneTrigger)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CRuFusion_Trekker::GetWarpTriggerParameters(const CRuVector3 &point, char *targetMapOut, INT32 targetMapBufLen, CRuVector3 *targetPositionOut)
{
	// Enumerate triggers at the specified location
	CRuArrayList<CRuWorld_Trigger *> triggers(16);
	m_terrain->GetTriggers(point, triggers);

	// Build common collision query
	CRuCollisionQuery_Segment_HitTest segQuery;
	CRuMatrix4x4 identityMatrix;
	segQuery.m_worldTransform = &identityMatrix;
	segQuery.m_invWorldTransform = &identityMatrix;
	segQuery.m_ignoreCollisionFlag = TRUE;
	segQuery.m_colSegment.SetFromPoints(CRuVector3(point.m_x, -10000.0f, point.m_z), CRuVector3(point.m_x, 10000.0f, point.m_z));

	// Find the first warp trigger which contains the specified location
	for(INT32 i = 0; i < triggers.Count(); ++i)
	{
		if(triggers[i]->GetTriggerType() == RuWorld::ruTRIGGERTYPE_WARP)
		{
			// Make query
			triggers[i]->QueryCollision(&segQuery);

			if(segQuery.m_positiveHit)
			{
				return RuTrigger_GetWarpParameters(triggers[i], targetMapOut, targetMapBufLen, targetPositionOut);
			}
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
