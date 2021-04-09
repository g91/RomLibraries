#ifndef _RUFUSION_TREKKER_INTERFACE_H_
#define _RUFUSION_TREKKER_INTERFACE_H_

// Base types
#include "../Core/Type/RuType_Object.h"
#include "../Core/Collection/RuCollections.h"
#include "../Core/Math/RuMathVector.h"
#include "../Core/Math/RuMathGeometry.h"
#include "../Scene/Terrain/RuEntityOptimalTerrain.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

enum RuTrekkerZoneType
{
	ruTREKKERZONETYPE_NORMAL				= 0,
	ruTREKKERZONETYPE_DEADZONE				= 1,
	ruTREKKERZONETYPE_DAMAGEZONE			= 2,
	ruTREKKERZONETYPE_IMMEDIATEDEATHZONE	= 3,

	ruTREKKERZONETYPE_FORCE_DWORD			= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

class IRuFusion_Trekker_Interface : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual								~IRuFusion_Trekker_Interface();

	// Setup
	virtual BOOL						SetPathMaximumExpansion(INT32 pathMaximumExpansion) = 0;
	virtual BOOL						PreLoad(const CRuAABB &bounds) = 0;

	// Collision testing
	virtual BOOL						IsPathClear(const CRuVector3 &origin, const CRuVector3 &destination, BOOL ignoreBackface = TRUE) = 0;

	// Location query
	virtual BOOL						IsRestingPointValid(const CRuVector3 &origin) = 0;
	virtual BOOL						FindRestingHeight(const CRuVector3 &origin, REAL testDistance, REAL &restingHeightOut) = 0;
	virtual BOOL						FindNeighborPoints(const CRuVector3 &origin, REAL radius, REAL testDistance, REAL heightOffset, INT32 divisions, CRuArrayList<CRuVector3> *neighborPointsOut) = 0;

	// Path finding
	virtual PTRVALUE					FindPathNode(const CRuVector3 &point, REAL searchRadius) = 0;
	virtual BOOL						FindPath(const PTRVALUE origin, const PTRVALUE destination, CRuArrayList<PTRVALUE> *pathNodeOut) = 0;
	virtual BOOL						FindPath(const CRuVector3 &origin, const CRuVector3 &destination, CRuArrayList<CRuVector3> *pathPositionOut) = 0;
	virtual BOOL						CalculatePathPoints(const CRuVector3 &p0, const CRuVector3 &p1, CRuArrayList<PTRVALUE> *pathNodes, CRuArrayList<CRuVector3> *pathPositionOut) = 0;

	// Path hierarchy management
	virtual BOOL						CreateIndependentPathHierarchy() = 0;
	virtual BOOL						DeleteIndependentPathHierarchy() = 0;
	virtual void						InsertModel(CRuEntity *model) = 0;
	virtual void						UpdateIndependentPathHierarchy() = 0;

	// Map query
	virtual IRuObject*					GetTerrain() = 0;
	virtual BOOL						GetMapBounds(CRuAABB *mapBoundsOut) = 0;

	// Zone query
	virtual const char*					FindZoneName(const CRuVector3 &point) = 0;
	virtual BOOL						FindZoneRespawnPoint(const CRuVector3 &point, CRuVector3 &respawnPointOut) = 0;
	virtual BOOL						FindMaximalZoneControlBounds(INT32 zoneID, CRuAABB *controlBoundsOut) = 0;
	virtual BOOL						FindZoneControlRects(INT32 zoneID, REAL controlRectSize, CRuArrayList<CRuAABB> *controlRectsOut) = 0;
	virtual INT32						FindRootZoneID(const CRuVector3 &point) = 0;
	virtual INT32						FindZoneID(const CRuVector3 &point) = 0;
	virtual BOOL						EnumerateRootZones(CRuArrayList<CRuWorld_ZoneDescriptor *> *zoneDescriptorsOut) = 0;
	virtual BOOL						EnumerateSubZones(INT32 rootZoneID, CRuArrayList<CRuWorld_ZoneDescriptor *> *zoneDescriptorsOut) = 0;

	virtual RuTrekkerZoneType			GetZoneType(const CRuVector3 &point, REAL *damageFactorOut) = 0;
	virtual BOOL						IsDeadZone(const CRuVector3 &point) = 0;
	virtual BOOL						IsSafeZone(const CRuVector3 &point) = 0;

	// Trigger utilities
	virtual BOOL						GetWarpTriggerParameters(const CRuVector3 &point, char *targetMapOut, INT32 targetMapBufLen, CRuVector3 *targetPositionOut) = 0;
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
