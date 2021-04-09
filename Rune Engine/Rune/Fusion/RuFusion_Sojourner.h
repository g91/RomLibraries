#pragma once

// Base types
#include "../Collision/RuCollision_Base.h"
#include "../Scene/Base/RuEntityBase.h"

#include "../Scene/Base/RuEntityModel.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

enum CRuSojournerMovementType
{
	ruSOJMOVETYPE_FULL						= 0,
	ruSOJMOVETYPE_NPC						= 1,

	ruSOJMOVETYPE_FORCE_DWORD				= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

class CRuFusion_Sojourner : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	enum RuCollisionType
	{
		ruCOLTYPE_NONE			= 0,
		ruCOLTYPE_SURFACE		= 1,
		ruCOLTYPE_EDGE			= 2,
		ruCOLTYPE_POINT			= 3,
		ruCOLTYPE_EMBEDDED		= 4
	};

	struct RuCollider
	{
		INT32						m_triangleIndex;
		CRuPlane4					m_trianglePlane;
		REAL						m_collisionDistance;
		CRuVector3					m_collisionPoint;
		RuCollisionType				m_collisionType;
	};

	// Collision objects
	IRuObject*						m_collisionObject;					//!< Static collision object
	DWORD							m_collisionMask;					//!< Collision exclusion mask

	// Movement settings
	CRuSojournerMovementType		m_movementType;						//!< Movement mode
	BOOL							m_enableWalkingSurfaceSnapping;		//!< Enable snap to surface while walking
	REAL							m_slopeSlideThreshold;				//!< Angle beyond which sliding will cease to go upwards (in degrees)
	REAL							m_restingHeightSearchUpLimit;
	REAL							m_restingHeightSearchDownLimit;

	// Current-run query info
	CRuAAEllipsoid					m_ellipsoid;						//!< The colliding ellipsoid
	CRuVector3						m_velocity;							//!< Ellipsoid velocity
	CRuSphere						m_sphere;
	CRuArrayList<CRuVector3>		m_movementEndPoints;				//!< Movement end points during the last update cycle

	// Termination settings
	REAL							m_iterationAbortThreshold;			//!< Number of iterations beyond which no computations will be done

	// Friction settings
	CRuVector3						m_frictionSurfaceNormal;			//!< Normal used for threshold comparison
	REAL							m_frictionSurfaceThreshold;			//!< Threshold at which friction begins to apply
	REAL							m_frictionConstant;					//!< Friction constant

	// Currently enumerated triangles
	INT32							m_triEnumIteration;
	CRuSphere						m_lastQueryBounds;					//!< Enumeration bounds
	CRuArrayList<CRuVector3>		m_triangleVerts0;					//!< Enumerated triangle vertices
	CRuArrayList<CRuVector3>		m_triangleNormals0;					//!< Enumerated triangle normals
	CRuArrayList<UINT32>			m_triangleFlags0;					//!< Enumerated triangle surface flags

	// Currently enumerated triangles for water testing
	CRuOBB							m_waterTri_LastQueryBounds;
	CRuArrayList<CRuVector3>		m_waterTri_Vertices;				//!< Enumerated triangle vertices
	CRuArrayList<CRuVector3>		m_waterTri_Normals;					//!< Enumerated triangle normals
	CRuArrayList<UINT32>			m_waterTri_Flags;					//!< Enumerated triangle surface flags


	// Temporaries
	CRuVector3						m_ellipsoidOrigin;
	CRuMatrix4x4					m_ellipsoidDeformation;
	CRuArrayList<CRuVector3>		m_triangleVerts;
	CRuArrayList<CRuVector3>		m_triangleNormals;
	CRuArrayList<CRuPlane4>			m_trianglePlanes;

	CRuArrayList<RuCollider>		m_colliders;
	CRuArrayList<INT32>				m_skipList;

	// Results
	REAL							m_lastIterationCoverage;
	BOOL							m_lastIterationCollided;			//!< Last iteration caused a collision?
	CRuVector3						m_lastCollisionNormal;				//!< Plane normal of the last collision
	CRuVector3						m_lastCollisionPoint;
	CRuVector3						m_lastRestingPoint;
	CRuVector3						m_lastRestingPointNormal;

	// Statistics
	INT32							m_passed;
	INT32							m_path_Embedded;
	INT32							m_path_Surface;
	INT32							m_path_Point;
	INT32							m_path_Edge;
/*
	// Debugging model
	CRuDynamicMesh*					m_visualizerMesh;
	CRuMaterial*					m_visualizerMaterial;
	CRuModel*						m_visualizerModel;
*/
public:
									CRuFusion_Sojourner(IRuObject *collisionObject);
	virtual							~CRuFusion_Sojourner();

	void							SetMovementType(CRuSojournerMovementType movementType);
	void							SetWalkingSurfaceSnapping(BOOL enableSnapping);
	void							SetCollisionObject(IRuObject *collisionObject);
	void							SetCollisionMask(DWORD mask);
	void							SetFrictionParameters(const CRuVector3 &frictionSurfaceNormal, REAL frictionSurfaceThreshold, REAL frictionConstant);
	void							SetRestingHeightSearchLimits(REAL upLimit, REAL downLimit);

//	CRuModel*						GetVisualizer();

	void							ResetCollisionData();

	BOOL							Move(const CRuVector3 &origin, const CRuVector3 &velocity, const CRuVector3 &radius, CRuVector3 &adjustedPositionOut);
	BOOL							MoveWithClimbAssist(const CRuVector3 &origin, const CRuVector3 &velocity, const CRuVector3 &radius, CRuVector3 &adjustedPositionOut);

	BOOL							Slide(const CRuVector3 &origin, const CRuVector3 &velocity, const CRuVector3 &radius, CRuVector3 &adjustedPositionOut);
	BOOL							NPCSlide(const CRuVector3 &origin, const CRuVector3 &velocity, const CRuVector3 &radius, CRuVector3 &adjustedPositionOut);
	BOOL							IntersectsWaterSurface(const CRuSegment3 &segment);
	BOOL							FindRestingHeight(const CRuVector3 &origin, CRuVector3 &restingPtOut);
	BOOL							FindRestingHeightAlt(const CRuVector3 &origin, CRuVector3 &restingPtOut, CRuVector3 &restingPtNormalOut);

	BOOL							IsAboveWaterSurface(const CRuVector3 &origin);
	BOOL							IsUnderWaterSurface(const CRuVector3 &origin);

	BOOL							FindClimbAssistOrigin(const CRuVector3 &origin, CRuVector3 &assistedOriginOut);

	REAL							GetLastIterationCoverage();
	BOOL							LastIterationCollided();
	const CRuVector3&				GetLastCollisionNormal();
	const CRuVector3&				GetLastCollisionPoint();
	const CRuVector3&				GetLastRestingPoint();
	const CRuVector3&				GetLastRestingPointNormal();
	CRuArrayList<CRuVector3>&		GetMovementEndPoints();

protected:
	BOOL							CollisionContainerChangedHandler(RuEventArgs *eventArgs);
	BOOL							CollisionContainerDisposeHandler(RuEventArgs *eventArgs);

	BOOL							EnumeratePotentialColliders(const CRuSphere &bounds, REAL boundsPadding);
	BOOL							WaterTri_EnumeratePotentialColliders(const CRuVector3 &position, REAL boundsPadding);

	CRuVector3						CollideAndStop(const CRuRay3 &collisionRay, const REAL speed);
	CRuVector3						CollideAndSlide(const CRuRay3 &collisionRay, const REAL speed);

	BOOL							IsEmbedded(INT32 triangleIndex, const CRuVector3 &sphereCenter, CRuVector3 &closestPointOut);
	void							ProcessColliders(const CRuRay3 &collisionRay, const REAL speed);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
