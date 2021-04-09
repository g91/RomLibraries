#ifndef _RUFUSION_TREKKER_H_
#define _RUFUSION_TREKKER_H_

// Base types
#include "../PathEngine/RuPathRover_Simple.h"
#include "../Fusion/RuFusion_Trekker_Interface.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuFusion_Trekker : public IRuFusion_Trekker_Interface
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuOptimalTerrain*					m_terrain;
	IRuPathHierarchy_Base*				m_pathHierarchy;

	// Settings
	REAL								m_defaultNodeSearchRadius;
	REAL								m_defaultLocationSearchRange;
	INT32								m_pathMaximumExpansion;

	// Independent path hierarchy data
	char								m_indPH_Filename[MAX_PATH];
	CRuPathHierarchy_DS*				m_indPH_Hierarchy;
	CRuArrayList<CRuEntity *>			m_indPH_AdditionalColObjs;

	// Runtime temporaries
	CRuPathRover_Simple*				m_runTime_PathRover;
	IRuPathFinder_Base*					m_runTime_PathFinder;
	CRuCollisionQuery_Segment			m_runTime_SegQuery;
	CRuCollisionQuery_Segment_Nearest	m_runTime_NearestSegQuery;
	CRuCollisionQuery_Segment_HitTest	m_runTime_SegHitQuery;
	CRuCollisionQuery_Sphere_TriEnum	m_runTime_SphereEnumQuery;
	CRuArrayList<INT32>					m_runTime_PathGraphKeys;
	CRuArrayList<INT32>					m_runTime_PathNodeKeys;

	CRuArrayList<CRuPathNode *>			m_runTime_PathNodes;

public:
										CRuFusion_Trekker(const char *terrainPath);
										CRuFusion_Trekker(CRuOptimalTerrain *terrain);
	virtual								~CRuFusion_Trekker();

	// Setup
	virtual BOOL						SetPathMaximumExpansion(INT32 pathMaximumExpansion);
	virtual BOOL						SetSpecialEventFilter(const char *filterString);
	virtual BOOL						PreLoad(const CRuAABB &bounds);

	// Collision testing
	virtual BOOL						IsPathClear(const CRuVector3 &origin, const CRuVector3 &destination, BOOL ignoreBackface = TRUE);

	// Location query
	virtual BOOL						IsRestingPointValid(const CRuVector3 &origin);
	virtual BOOL						FindRestingHeight(const CRuVector3 &origin, REAL testDistance, REAL &restingHeightOut);
	virtual BOOL						FindNeighborPoints(const CRuVector3 &origin, REAL radius, REAL testDistance, REAL heightOffset, INT32 divisions, CRuArrayList<CRuVector3> *neighborPointsOut);

	// Path finding
	virtual PTRVALUE					FindPathNode(const CRuVector3 &point, REAL searchRadius);
	virtual BOOL						FindPath(const PTRVALUE origin, const PTRVALUE destination, CRuArrayList<PTRVALUE> *pathNodeOut);
	virtual BOOL						FindPath(const CRuVector3 &origin, const CRuVector3 &destination, CRuArrayList<CRuVector3> *pathPositionOut);
	virtual BOOL						CalculatePathPoints(const CRuVector3 &p0, const CRuVector3 &p1, CRuArrayList<PTRVALUE> *pathNodes, CRuArrayList<CRuVector3> *pathPositionOut);

	// Path hierarchy management
	virtual BOOL						CreateIndependentPathHierarchy();
	virtual BOOL						DeleteIndependentPathHierarchy();
	virtual void						InsertModel(CRuEntity *model);
	virtual void						UpdateIndependentPathHierarchy();

	// Map query
	virtual IRuObject*					GetTerrain();
	virtual BOOL						GetMapBounds(CRuAABB *mapBoundsOut);

	// Zone query
	virtual const char*					FindZoneName(const CRuVector3 &point);
	virtual BOOL						FindZoneRespawnPoint(const CRuVector3 &point, CRuVector3 &respawnPointOut);
	virtual BOOL						FindMaximalZoneControlBounds(INT32 zoneID, CRuAABB *controlBoundsOut);
	virtual BOOL						FindZoneControlRects(INT32 zoneID, REAL controlRectSize, CRuArrayList<CRuAABB> *controlRectsOut);
	virtual INT32						FindRootZoneID(const CRuVector3 &point);
	virtual INT32						FindZoneID(const CRuVector3 &point);
	virtual BOOL						EnumerateRootZones(CRuArrayList<CRuWorld_ZoneDescriptor *> *zoneDescriptorsOut);
	virtual BOOL						EnumerateSubZones(INT32 rootZoneID, CRuArrayList<CRuWorld_ZoneDescriptor *> *zoneDescriptorsOut);

	virtual RuTrekkerZoneType			GetZoneType(const CRuVector3 &point, REAL *damageFactorOut);
	virtual BOOL						IsDeadZone(const CRuVector3 &point);
	virtual BOOL						IsSafeZone(const CRuVector3 &point);

	// Trigger utilities
	virtual BOOL						GetWarpTriggerParameters(const CRuVector3 &point, char *targetMapOut, INT32 targetMapBufLen, CRuVector3 *targetPositionOut);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
