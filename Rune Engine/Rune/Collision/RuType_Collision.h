/*!
	@file RuType_Collision.h

	Copyright (c) 2004 All rights reserved
*/

#ifndef _RUTYPE_COLLISION_H_
#define _RUTYPE_COLLISION_H_

#include "../Core/Collection/RuCollections.h"
#include "../Core/Math/RuMathCollision.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class IRuMesh;

// ************************************************************************************************************************************************************

/*!
	Enumeration of all possible collision query types.

	@version 2004/12/29
*/
enum RuCollisionQueryType
{
	ruCOLQUERY_RAY							= 1,							//!< Test collision with ray (CRuCollisionQuery_Ray)
	ruCOLQUERY_SEGMENT						= 2,							//!< Test collision with segment
	ruCOLQUERY_SPHERE						= 3,							//!< Test collision with sphere
	ruCOLQUERY_FRUSTUM						= 5,							//!< Test collision with frustum (six bounding planes)

	ruCOLQUERY_RAY_HITTEST					= 11,							//!< Test collision with ray
	ruCOLQUERY_RAY_NEAREST					= 12,							//!< Test collision with ray
	ruCOLQUERY_RAY_PICK						= 13,							//!< Test collision with ray, in pick test mode (CRuCollisionQuery_Ray_Pick)
	ruCOLQUERY_RAY_TRIPICK					= 14,							//!< Test collision with ray, in triangle picking mode (CRuCollisionQuery_Ray_TriPick)
	ruCOLQUERY_RAY_SUBOBJECT				= 15,							//!< Test collision with ray, in sub-object picking mode (CRuCollisionQuery_Ray_SubObject)

	ruCOLQUERY_SEGMENT_HITTEST				= 21,							//!< Test collision with segment, early-exit hit test only (CRuCollisionQuery_Segment_HitTest)
	ruCOLQUERY_SEGMENT_NEAREST				= 22,							//!< Test collision with segment, nearest result only (CRuCollisionQuery_Segment_Nearest)
	ruCOLQUERY_SEGMENT_PICK					= 23,							//!< Test collision with segment, in pick test mode (CRuCollisionQuery_Segment_Pick)
	ruCOLQUERY_SEGMENT_TRIPICK				= 24,							//!< Test collision with segment, in triangle picking mode (CRuCollisionQuery_Segment_TriPick)
	ruCOLQUERY_SEGMENT_SUBOBJECT			= 25,							//!< Test collision with segment, in sub-object picking mode (CRuCollisionQuery_Segment_SubObject)

	ruCOLQUERY_OBB_HITTEST					= 35,							//!< Test collision with OBB (CRuCollisionQuery_OBB_HitTest)
	ruCOLQUERY_OBB_TRIENUM					= 36,							//!< Test collision with OBB, in triangle enumeration mode (CRuCollisionQuery_OBB_TriEnum)

	ruCOLQUERY_FRUSTUM_SUBOBJECT			= 41,							//!< Test collision with Frustum, in sub-object picking mode (CRuCollisionQuery_Frustum_SubObject)

	ruCOLQUERY_SPHERE_HITTEST				= 51,							//!< Test collision with sphere
	ruCOLQUERY_SPHERE_TRIENUM				= 54,							//!< Test collision with sphere

	ruCOLQUERY_ENUMERATE					= 61,							//!< Full triangle enumeration mode (CRuCollisionQuery_Enumerate)

	ruCOLQUERY_USER_FLAG_SET				= 71,							//!< Unconditional ID marking mode (CRuCollisionQuery_User_Flag_Set)
	ruCOLQUERY_USER_FLAG_SET_RAY			= 72,							//!< ID marking mode (CRuCollisionQuery_User_Flag_Set_Ray)
	ruCOLQUERY_USER_FLAG_SET_SPHERE			= 74,							//!< ID marking mode (CRuCollisionQuery_User_Flag_Set_Sphere)
	ruCOLQUERY_USER_FLAG_GET_SEGMENT		= 75,							//!< ID marking mode (CRuCollisionQuery_User_Flag_Get_Segment)
	ruCOLQUERY_USER_FLAG_ENUMERATE			= 76,							//!< Full triangle enumeration mode with flag enumeration (CRuCollisionQuery_User_Flag_Enumerate)

	ruCOLQUERY_FORCE_DWORD					= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

/*!
	Conditional callback for the user flag set operations. The callback will receive vertices
	of the triangle whos user flags are being set. It should return TRUE if the flags should
	be set, and FALSE if the flags should not be altered.

*/
typedef BOOL(*RuCollisionQuery_SetConditionCallback)(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2);

// ************************************************************************************************************************************************************

/*!
	Base class from which all collision queries derive. The base class contains members specifying
	the query type and transforms used to transform the collision primitive.

	For callers, this class is never instanced directly. Instead, one of the derived classes should
	be instanced. See specific query classes for details regarding their usage.

	For callees, after determining the query type by checking m_queryType, the collision query should
	be casted to the appropriate class. Before performing the collision test, the collision primitive
	must be transformed by m_invWorldTransform. Wherever relevant, before storing the results of the
	collision test, the results must be transformed by m_worldTransform.

	The implication of the above transforms is that all queries must be specified relative to some
	top-level coordinate frame. This ensures that results returned from a complex hierarchy of entities
	are in a consistent coordinate frame. Of course, if identity transforms are used, that simply
	means all collision testing is done in a single local frame, which may or may not make sense.

	@author John Tang
	@version 2004/11/01
*/
class CRuCollisionQuery
{
public:
	RuCollisionQueryType					m_queryType;					//!< Collision query type
	CRuMatrix4x4							*m_worldTransform;				//!< World transform
	CRuMatrix4x4							*m_invWorldTransform;			//!< Inverse world transform

	UINT32									m_exclusionMask;				//!< Surfaces matching the exclusion mask are ignored
	UINT32									m_inclusionMask;				//!< Surfaces not matching the inclusion mask are ignored
	BOOL									m_ignoreCollisionFlag;			//!< Perform collision regardless of the collision flag status
	BOOL									m_queryOnOriginalGeometry;		//!< Perform collision query on original geometry only?

	BOOL									m_resetColStruct;

public:
											CRuCollisionQuery();
	virtual									~CRuCollisionQuery();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against a ray. To be used as a base class for all segment-based queries.

	@author John Tang
	@version 2004/11/01
*/
class CRuCollisionQuery_Ray_Base : public CRuCollisionQuery
{
public:
	CRuRay3									m_colRay;						//!< Collision ray
	BOOL									m_ignoreBackface;				//!< Ignore backfaces?

public:
											CRuCollisionQuery_Ray_Base();
	virtual									~CRuCollisionQuery_Ray_Base();
};
// ************************************************************************************************************************************************************

/*!
	Collision query against a segment. To be used as a base class for all segment-based queries.

	@author John Tang
	@version 2005/04/01
*/
class CRuCollisionQuery_Segment_Base : public CRuCollisionQuery
{
public:
	CRuSegment3								m_colSegment;					//!< Collision segment
	BOOL									m_ignoreBackface;				//!< Ignore backfaces?

public:
											CRuCollisionQuery_Segment_Base();
	virtual									~CRuCollisionQuery_Segment_Base();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against an OBB. To be used as a base class for all OBB-based queries.

	@author John Tang
	@version 2005/09/29
*/
class CRuCollisionQuery_OBB_Base : public CRuCollisionQuery
{
public:
	CRuOBB									m_colOBB;					//!< Collision OBB

public:
											CRuCollisionQuery_OBB_Base();
	virtual									~CRuCollisionQuery_OBB_Base();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against a sphere. To be used as a base class for all sphere-based queries.

	@author John Tang
	@version 2005/09/12
*/
class CRuCollisionQuery_Sphere_Base : public CRuCollisionQuery
{
public:
	CRuSphere								m_colSphere;					//!< Collision sphere
	BOOL									m_ignoreBackface;				//!< Ignore backfaces?

public:
											CRuCollisionQuery_Sphere_Base();
	virtual									~CRuCollisionQuery_Sphere_Base();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against a frustum. To be used as a base class for all frustum-based queries.

	@author John Tang
	@version 2005/09/23
*/
class CRuCollisionQuery_Frustum_Base : public CRuCollisionQuery
{
public:
	CRuPlane4								m_colFrustum[6];				//!< Collision frustum planes
	CRuAABB									m_colFrustumBounds;				//!< Collision frustum bounds

public:
											CRuCollisionQuery_Frustum_Base(const CRuPlane4 *frustumPlanes = NULL);
	virtual									~CRuCollisionQuery_Frustum_Base();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against a ray.

	Callers should create an instance of this class, then set m_colRay to the collision ray to test
	with. If m_ignoreBackface is true, then the collision query will ignore backfacing primitives.

	Point intersections are returned in the two array lists. If their lengths are zero, then there
	were no collisions.

	@author John Tang
	@version 2004/11/01
*/
class CRuCollisionQuery_Ray : public CRuCollisionQuery_Ray_Base
{
public:
	CRuArrayList<CRuVector3>*				m_colPoints;					//!< Output collision points
	CRuArrayList<float>*					m_colDistances;					//!< Output collision distances

public:
											CRuCollisionQuery_Ray();
	virtual									~CRuCollisionQuery_Ray();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against a ray.

	Callers should create an instance of this class, then set m_colRay to the collision ray to test
	with. If m_ignoreBackface is true, then the collision query will ignore backfacing primitives.

	Point intersections are returned in the two array lists. If their lengths are zero, then there
	were no collisions.

	@author John Tang
	@version 2004/11/01
*/
class CRuCollisionQuery_Ray_Nearest : public CRuCollisionQuery_Ray_Base
{
public:
	BOOL									m_positiveHit;					//!< Is the result positive?
	REAL									m_collisionT;					//!< Collision t, in [0, 1]
	CRuVector3								m_collisionPoint;				//!< Collision point
	UINT32									m_collisionFlag;				//!< Flags of the collision triangle

public:
											CRuCollisionQuery_Ray_Nearest();
	virtual									~CRuCollisionQuery_Ray_Nearest();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against a ray.

	Callers should create an instance of this class, then set m_colRay to the collision ray to test
	with. If m_ignoreBackface is true, then the collision query will ignore backfacing primitives.

	@author John Tang
	@version 2005/06/02
*/
class CRuCollisionQuery_Ray_Pick : public CRuCollisionQuery_Ray_Base
{
public:
	BOOL									m_allowRoughPicking;			//!< Allow rough, imprecise picking?

	BOOL									m_positiveHit;					//!< Is the result positive?
	REAL									m_colDistance;					//!< Collision distance

public:
											CRuCollisionQuery_Ray_Pick();
	virtual									~CRuCollisionQuery_Ray_Pick();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against a ray; Return results as triangles.

	Callers should create an instance of this class, then set m_colRay to the collision ray to test
	with. If m_ignoreBackface is true, then the collision query will ignore backfacing primitives.

	Results are returned in the three array lists. If their lengths are zero, then there were no
	collisions. m_colMeshes specifies the mesh at which collision occurred. m_colTriIndices specifies
	the index of the colliding primitive. m_colDistances specifies the collision distance.

	@author John Tang
	@version 2004/11/29
*/
class CRuCollisionQuery_Ray_TriPick : public CRuCollisionQuery_Ray_Base
{
public:
	CRuArrayList<IRuMesh *>*				m_colMeshes;					//!< Output collision meshes
	CRuArrayList<INT32>*					m_colTriIndices;				//!< Output collision triangles (by index)
	CRuArrayList<float>*					m_colDistances;					//!< Output collision distances

public:
											CRuCollisionQuery_Ray_TriPick();
	virtual									~CRuCollisionQuery_Ray_TriPick();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against a ray; Return results as sub-objects.

	Callers should create an instance of this class, then set m_colRay to the collision ray to test
	with. If m_ignoreBackface is true, then the collision query will ignore backfacing primitives.

	Results are returned in two array lists. This query returns results are a series of entity
	collisions.

	@author John Tang
	@version 2005/01/17
*/
class CRuCollisionQuery_Ray_SubObject : public CRuCollisionQuery_Ray_Base
{
public:
	CRuArrayList<IRuObject *>*				m_colObjects;					//!< Output collision objects
	CRuArrayList<float>*					m_colDistances;					//!< Output collision distances

public:
											CRuCollisionQuery_Ray_SubObject();
	virtual									~CRuCollisionQuery_Ray_SubObject();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against a segment.

	Callers should create an instance of this class, then set m_colSegment to the collision segment to test
	with. If m_ignoreBackface is true, then the collision query will ignore backfacing primitives.

	Point intersections are returned in the two array lists. If their lengths are zero, then there
	were no collisions.

	@author John Tang
	@version 2005/04/01
*/
class CRuCollisionQuery_Segment : public CRuCollisionQuery_Segment_Base
{
public:
	CRuArrayList<CRuVector3>*				m_colPoints;					//!< Output collision points
	CRuArrayList<float>*					m_colDistances;					//!< Output collision distances

public:
											CRuCollisionQuery_Segment();
	virtual									~CRuCollisionQuery_Segment();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against a segment, checking only if the segment hits.

	Callers should create an instance of this class, then set m_colSegment to the collision segment to test
	with. If m_ignoreBackface is true, then the collision query will ignore backfacing primitives.

	@author John Tang
	@version 2005/04/01
*/
class CRuCollisionQuery_Segment_HitTest : public CRuCollisionQuery_Segment_Base
{
public:
	BOOL									m_positiveHit;					//!< Is the result positive?

public:
											CRuCollisionQuery_Segment_HitTest();
	virtual									~CRuCollisionQuery_Segment_HitTest();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against a segment, checking only if the segment hits.

	Callers should create an instance of this class, then set m_colSegment to the collision segment to test
	with. If m_ignoreBackface is true, then the collision query will ignore backfacing primitives.

	@author John Tang
	@version 2005/04/01
*/
class CRuCollisionQuery_Segment_Nearest : public CRuCollisionQuery_Segment_Base
{
public:
	BOOL									m_positiveHit;					//!< Is the result positive?
	REAL									m_collisionT;					//!< Collision t, in [0, 1]
	CRuVector3								m_collisionPoint;				//!< Collision point
	CRuVector3								m_collisionNormal;				//!< Collision normal
	UINT32									m_collisionFlag;				//!< Flags of the collision triangle

public:
											CRuCollisionQuery_Segment_Nearest();
	virtual									~CRuCollisionQuery_Segment_Nearest();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against a segment; Return results as triangles.

	Callers should create an instance of this class, then set m_colSegment to the collision segment to test
	with. If m_ignoreBackface is true, then the collision query will ignore backfacing primitives.

	Results are returned in the three array lists. If their lengths are zero, then there were no
	collisions. m_colMeshes specifies the mesh at which collision occurred. m_colTriIndices specifies
	the index of the colliding primitive. m_colDistances specifies the collision distance.

	@author John Tang
	@version 2004/11/29
*/
class CRuCollisionQuery_Segment_TriPick : public CRuCollisionQuery_Segment_Base
{
public:
	CRuArrayList<IRuMesh *>*				m_colMeshes;					//!< Output collision meshes
	CRuArrayList<INT32>*					m_colTriIndices;				//!< Output collision triangles (by index)
	CRuArrayList<float>*					m_colDistances;					//!< Output collision distances

public:
											CRuCollisionQuery_Segment_TriPick();
	virtual									~CRuCollisionQuery_Segment_TriPick();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against a segment; Return results as sub-objects.

	Callers should create an instance of this class, then set m_colSegment to the collision segment to test
	with. If m_ignoreBackface is true, then the collision query will ignore backfacing primitives.

	Results are returned in two array lists. This query returns results are a series of entity
	collisions.

	@author John Tang
	@version 2005/01/17
*/
class CRuCollisionQuery_Segment_SubObject : public CRuCollisionQuery_Segment_Base
{
public:
	CRuArrayList<IRuObject *>*				m_colObjects;					//!< Output collision objects
	CRuArrayList<float>*					m_colDistances;					//!< Output collision distances

public:
											CRuCollisionQuery_Segment_SubObject();
	virtual									~CRuCollisionQuery_Segment_SubObject();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against an OBB, checking only if the OBB hits

	Callers should create an instance of this class, then set m_colOBB to the collision OBB to test
	with.

	@author John Tang
	@version 2005/11/14
*/
class CRuCollisionQuery_OBB_HitTest : public CRuCollisionQuery_OBB_Base
{
public:
	BOOL									m_positiveHit;					//!< Is the result positive?

public:
											CRuCollisionQuery_OBB_HitTest();
	virtual									~CRuCollisionQuery_OBB_HitTest();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against an OBB; Return results as triangles.

	Callers should create an instance of this class, then set m_colOBB to the collision OBB to test
	with.

	Results are returned in the vertex array list. If the length is zero, then there were no
	collisions.

	@author John Tang
	@version 2005/03/29
*/
class CRuCollisionQuery_OBB_TriEnum : public CRuCollisionQuery_OBB_Base
{
public:
	CRuArrayList<CRuVector3>*				m_colTriVerts;					//!< Output collision triangles
	CRuArrayList<CRuVector3>*				m_colTriNormals;				//!< Output collision triangle normals
	CRuArrayList<UINT32>*					m_colTriFlags;					//!< Output collision triangle flags

public:
											CRuCollisionQuery_OBB_TriEnum();
	virtual									~CRuCollisionQuery_OBB_TriEnum();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against a frustum; Return results as sub-objects.

	Callers should create an instance of this class, then set m_colFrustum to the collision Frustum to test
	with.

	Results are returned in one array list. This query returns results are a series of entity
	collisions.

	@author John Tang
	@version 2005/09/23
*/
class CRuCollisionQuery_Frustum_SubObject : public CRuCollisionQuery_Frustum_Base
{
public:
	CRuArrayList<IRuObject *>*				m_colObjects;					//!< Output collision objects

public:
											CRuCollisionQuery_Frustum_SubObject(const CRuPlane4 *frustumPlanes = NULL);
	virtual									~CRuCollisionQuery_Frustum_SubObject();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against a sphere, checking only if the sphere hits.

	Callers should create an instance of this class, then set m_colSphere to the collision sphere to test
	with. If m_ignoreBackface is true, then the collision query will ignore backfacing primitives.

	@author John Tang
	@version 2005/04/01
*/
class CRuCollisionQuery_Sphere_HitTest : public CRuCollisionQuery_Sphere_Base
{
public:
	BOOL									m_positiveHit;					//!< Is the result positive?

	REAL									m_colDistance;					//!< Collision distance
	CRuVector3								m_colNormal;					//1< Collision normal

public:
											CRuCollisionQuery_Sphere_HitTest();
	virtual									~CRuCollisionQuery_Sphere_HitTest();
};

// ************************************************************************************************************************************************************

/*!
	Collision query against a sphere.

	Callers should create an instance of this class, then set m_colSphere to the collision sphere to test
	with. If m_ignoreBackface is true, then the collision query will ignore backfacing primitives.

	@author John Tang
	@version 2006/01/25
*/
class CRuCollisionQuery_Sphere_TriEnum : public CRuCollisionQuery_Sphere_Base
{
public:
	CRuArrayList<CRuVector3>*				m_colTriVerts;					//!< Output collision triangles
	CRuArrayList<CRuVector3>*				m_colTriNormals;				//!< Output collision triangle normals
	CRuArrayList<UINT32>*					m_colTriFlags;					//!< Output collision triangle flags

public:
											CRuCollisionQuery_Sphere_TriEnum();
	virtual									~CRuCollisionQuery_Sphere_TriEnum();
};

// ************************************************************************************************************************************************************

/*!
	Full enumeration query; Return results as triangles.

	Results are returned in the vertex array list. If the length is zero, then there were no
	triangles to enumerate.

	@author John Tang
	@version 2005/03/29
*/
class CRuCollisionQuery_Enumerate : public CRuCollisionQuery
{
public:
	CRuArrayList<CRuVector3>*				m_colTriVerts;					//!< Output collision triangles

public:
											CRuCollisionQuery_Enumerate();
	virtual									~CRuCollisionQuery_Enumerate();
};

// ************************************************************************************************************************************************************

/*!
	Flag setting query. This query will set collision triangles with the specified flag and mask value.

	Number of affected collision triangles are recorded and returned.

	@author John Tang
	@version 2005/03/29
*/
class CRuCollisionQuery_User_Flag_Set : public CRuCollisionQuery
{
public:
	UINT32									m_markingFlag;					//!< Flag value to set
	UINT32									m_flagMask;						//!< Mask value to use when setting the flag value

	INT32									m_numAffectedPrimitives;		//!< Number of affected primitives
	CRuArrayList<void *>*					m_affectedObjects;				//!< List of pointers to affected objects

public:
											CRuCollisionQuery_User_Flag_Set();
	virtual									~CRuCollisionQuery_User_Flag_Set();
};

// ************************************************************************************************************************************************************

/*!
	Flag setting query. This query will set collision triangles with the specified flag and mask value.

	Number of affected collision triangles are recorded and returned.

	@author John Tang
	@version 2005/03/29
*/
class CRuCollisionQuery_User_Flag_Set_Ray : public CRuCollisionQuery
{
public:
	CRuRay3									m_markingRay;					//!< Marking collision ray

	UINT32									m_markingFlag;					//!< Flag value to set
	UINT32									m_flagMask;						//!< Mask value to use when setting the flag value

	INT32									m_numAffectedPrimitives;		//!< Number of affected primitives
	CRuArrayList<void *>*					m_affectedObjects;				//!< List of pointers to affected objects

public:
											CRuCollisionQuery_User_Flag_Set_Ray();
	virtual									~CRuCollisionQuery_User_Flag_Set_Ray();
};

// ************************************************************************************************************************************************************

/*!
	Flag setting query. This query will set collision triangles with the specified flag and mask value.

	Number of affected collision triangles are recorded and returned.

	@author John Tang
	@version 2005/03/29
*/
class CRuCollisionQuery_User_Flag_Set_Sphere : public CRuCollisionQuery
{
public:
	CRuSphere								m_markingSphere;				//!< Marking collision sphere

	UINT32									m_markingFlag;					//!< Flag value to set
	UINT32									m_flagMask;						//!< Mask value to use when setting the flag value

	INT32									m_numAffectedPrimitives;		//!< Number of affected primitives
	CRuArrayList<void *>*					m_affectedObjects;				//!< List of pointers to affected objects

public:
											CRuCollisionQuery_User_Flag_Set_Sphere();
	virtual									~CRuCollisionQuery_User_Flag_Set_Sphere();
};

// ************************************************************************************************************************************************************

/*!
	Flag retrieving query. This query will return the flag at the collision point.

	@author John Tang
	@version 2005/03/29
*/
class CRuCollisionQuery_User_Flag_Get_Segment : public CRuCollisionQuery
{
public:
	CRuSegment3								m_colSegment;					//!< Collision segment
	BOOL									m_ignoreBackface;				//!< Ignore backfaces?

	BOOL									m_positiveHit;					//!< Is the result positive?
	REAL									m_collisionT;					//!< Collision t, in [0, 1]
	CRuVector3								m_collisionPoint;				//!< Collision point
	UINT32									m_flag;							//!< Flag value to set

public:
											CRuCollisionQuery_User_Flag_Get_Segment();
	virtual									~CRuCollisionQuery_User_Flag_Get_Segment();
};

// ************************************************************************************************************************************************************

/*!
	Full enumeration query; Return results as triangles.

	Results are returned in the vertex array list. If the length is zero, then there were no
	triangles to enumerate.

	@author John Tang
	@version 2005/03/29
*/
class CRuCollisionQuery_User_Flag_Enumerate : public CRuCollisionQuery
{
public:
	CRuArrayList<CRuVector3>*				m_colTriVerts;					//!< Output collision triangles
	CRuArrayList<UINT32>*					m_colTriFlags;					//!< Output collision triangle flags

public:
											CRuCollisionQuery_User_Flag_Enumerate();
	virtual									~CRuCollisionQuery_User_Flag_Enumerate();
};

// ************************************************************************************************************************************************************

inline CRuCollisionQuery::CRuCollisionQuery()
:	m_exclusionMask(0x00000000),
	m_inclusionMask(0x00000000),
	m_ignoreCollisionFlag(FALSE),
	m_queryOnOriginalGeometry(FALSE),
	m_resetColStruct(TRUE)
{
}

inline CRuCollisionQuery::~CRuCollisionQuery()
{
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Ray_Base::CRuCollisionQuery_Ray_Base()
{
	// Default to ignore backfaces
	m_ignoreBackface = TRUE;
}

inline CRuCollisionQuery_Ray_Base::~CRuCollisionQuery_Ray_Base()
{
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Segment_Base::CRuCollisionQuery_Segment_Base()
{
	// Default to ignore backfaces
	m_ignoreBackface = TRUE;
}

inline CRuCollisionQuery_Segment_Base::~CRuCollisionQuery_Segment_Base()
{
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_OBB_Base::CRuCollisionQuery_OBB_Base()
{
}

inline CRuCollisionQuery_OBB_Base::~CRuCollisionQuery_OBB_Base()
{
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Sphere_Base::CRuCollisionQuery_Sphere_Base()
{
	// Default to ignore backfaces
	m_ignoreBackface = TRUE;
}

inline CRuCollisionQuery_Sphere_Base::~CRuCollisionQuery_Sphere_Base()
{
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Frustum_Base::CRuCollisionQuery_Frustum_Base(const CRuPlane4 *frustumPlanes)
{
	if(frustumPlanes)
	{
		// Copy frustum planes
		for(INT32 i = 0; i < 6; ++i)
		{
			m_colFrustum[i] = frustumPlanes[i];
		}

		// Calculate frustum bounds
		CRuVector3 pt[8];

		FindIntersection(m_colFrustum[4], m_colFrustum[0], m_colFrustum[2], pt[0]);
		FindIntersection(m_colFrustum[4], m_colFrustum[0], m_colFrustum[3], pt[1]);
		FindIntersection(m_colFrustum[4], m_colFrustum[1], m_colFrustum[2], pt[2]);
		FindIntersection(m_colFrustum[4], m_colFrustum[1], m_colFrustum[3], pt[3]);

		FindIntersection(m_colFrustum[5], m_colFrustum[0], m_colFrustum[2], pt[4]);
		FindIntersection(m_colFrustum[5], m_colFrustum[0], m_colFrustum[3], pt[5]);
		FindIntersection(m_colFrustum[5], m_colFrustum[1], m_colFrustum[2], pt[6]);
		FindIntersection(m_colFrustum[5], m_colFrustum[1], m_colFrustum[3], pt[7]);

		m_colFrustumBounds.Set(pt[0].x, pt[0].y, pt[0].z, pt[0].x, pt[0].y, pt[0].z);
		for(INT32 i = 1; i < 8; ++i)
		{
			m_colFrustumBounds.Merge(pt[i]);
		}
	}
}

inline CRuCollisionQuery_Frustum_Base::~CRuCollisionQuery_Frustum_Base()
{
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Ray::CRuCollisionQuery_Ray()
{
	// Set query type to a ray query
	m_queryType = ruCOLQUERY_RAY;

	// Create arrays to hold query results
	m_colPoints = ruNEW CRuArrayList<CRuVector3>();
	m_colDistances = ruNEW CRuArrayList<float>();
}

inline CRuCollisionQuery_Ray::~CRuCollisionQuery_Ray()
{
	delete m_colPoints;
	delete m_colDistances;
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Ray_Nearest::CRuCollisionQuery_Ray_Nearest()
{
	// Set query type to a segment query
	m_queryType = ruCOLQUERY_RAY_NEAREST;

	// Reset result
	m_positiveHit = FALSE;
	m_collisionT = FLT_MAX;
	m_collisionFlag = 0x00000000;
}

inline CRuCollisionQuery_Ray_Nearest::~CRuCollisionQuery_Ray_Nearest()
{
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Ray_Pick::CRuCollisionQuery_Ray_Pick()
{
	// Set query type to a ray query returning triangle picks
	m_queryType = ruCOLQUERY_RAY_PICK;

	// Set defaults
	m_allowRoughPicking = FALSE;

	// Reset result
	m_positiveHit = FALSE;
}

inline CRuCollisionQuery_Ray_Pick::~CRuCollisionQuery_Ray_Pick()
{
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Ray_TriPick::CRuCollisionQuery_Ray_TriPick()
{
	// Set query type to a ray query returning triangle picks
	m_queryType = ruCOLQUERY_RAY_TRIPICK;

	// Create arrays to hold query results
	m_colMeshes = ruNEW CRuArrayList<IRuMesh *>();
	m_colTriIndices = ruNEW CRuArrayList<INT32>();
	m_colDistances = ruNEW CRuArrayList<float>();
}

inline CRuCollisionQuery_Ray_TriPick::~CRuCollisionQuery_Ray_TriPick()
{
	delete m_colMeshes;
	delete m_colTriIndices;
	delete m_colDistances;
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Ray_SubObject::CRuCollisionQuery_Ray_SubObject()
{
	// Set query type to a ray query returning subobject picks
	m_queryType = ruCOLQUERY_RAY_SUBOBJECT;

	// Create arrays to hold query results
	m_colObjects = ruNEW CRuArrayList<IRuObject *>();
	m_colDistances = ruNEW CRuArrayList<float>();
}

inline CRuCollisionQuery_Ray_SubObject::~CRuCollisionQuery_Ray_SubObject()
{
	delete m_colObjects;
	delete m_colDistances;
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Segment::CRuCollisionQuery_Segment()
{
	// Set query type to a segment query
	m_queryType = ruCOLQUERY_SEGMENT;

	// Create arrays to hold query results
	m_colPoints = ruNEW CRuArrayList<CRuVector3>();
	m_colDistances = ruNEW CRuArrayList<float>();
}

inline CRuCollisionQuery_Segment::~CRuCollisionQuery_Segment()
{
	delete m_colPoints;
	delete m_colDistances;
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Segment_HitTest::CRuCollisionQuery_Segment_HitTest()
{
	// Set query type to a segment query
	m_queryType = ruCOLQUERY_SEGMENT_HITTEST;

	// Reset result
	m_positiveHit = FALSE;
}

inline CRuCollisionQuery_Segment_HitTest::~CRuCollisionQuery_Segment_HitTest()
{
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Segment_Nearest::CRuCollisionQuery_Segment_Nearest()
{
	// Set query type to a segment query
	m_queryType = ruCOLQUERY_SEGMENT_NEAREST;

	// Reset result
	m_positiveHit = FALSE;
	m_collisionT = FLT_MAX;
	m_collisionFlag = 0x00000000;
}

inline CRuCollisionQuery_Segment_Nearest::~CRuCollisionQuery_Segment_Nearest()
{
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Segment_TriPick::CRuCollisionQuery_Segment_TriPick()
{
	// Set query type to a segment query returning triangle picks
	m_queryType = ruCOLQUERY_SEGMENT_TRIPICK;

	// Create arrays to hold query results
	m_colMeshes = ruNEW CRuArrayList<IRuMesh *>();
	m_colTriIndices = ruNEW CRuArrayList<INT32>();
	m_colDistances = ruNEW CRuArrayList<float>();
}

inline CRuCollisionQuery_Segment_TriPick::~CRuCollisionQuery_Segment_TriPick()
{
	delete m_colMeshes;
	delete m_colTriIndices;
	delete m_colDistances;
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Segment_SubObject::CRuCollisionQuery_Segment_SubObject()
{
	// Set query type to a segment query returning subobject picks
	m_queryType = ruCOLQUERY_SEGMENT_SUBOBJECT;

	// Create arrays to hold query results
	m_colObjects = ruNEW CRuArrayList<IRuObject *>();
	m_colDistances = ruNEW CRuArrayList<float>();
}

inline CRuCollisionQuery_Segment_SubObject::~CRuCollisionQuery_Segment_SubObject()
{
	delete m_colObjects;
	delete m_colDistances;
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_OBB_HitTest::CRuCollisionQuery_OBB_HitTest()
{
	// Set query type to an OBB query
	m_queryType = ruCOLQUERY_OBB_HITTEST;

	// Reset result
	m_positiveHit = FALSE;
}

inline CRuCollisionQuery_OBB_HitTest::~CRuCollisionQuery_OBB_HitTest()
{
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_OBB_TriEnum::CRuCollisionQuery_OBB_TriEnum()
{
	// Set query type to a OBB query returning triangle enumerations
	m_queryType = ruCOLQUERY_OBB_TRIENUM;

	// Create arrays to hold query results
	m_colTriVerts = ruNEW CRuArrayList<CRuVector3>();
	m_colTriNormals = ruNEW CRuArrayList<CRuVector3>();
	m_colTriFlags = ruNEW CRuArrayList<UINT32>();
}

inline CRuCollisionQuery_OBB_TriEnum::~CRuCollisionQuery_OBB_TriEnum()
{
	delete m_colTriVerts;
	delete m_colTriNormals;
	delete m_colTriFlags;
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Frustum_SubObject::CRuCollisionQuery_Frustum_SubObject(const CRuPlane4 *frustumPlanes)
:	CRuCollisionQuery_Frustum_Base(frustumPlanes)
{
	// Set query type to an frustum query returning subobject picks
	m_queryType = ruCOLQUERY_FRUSTUM_SUBOBJECT;

	// Create arrays to hold query results
	m_colObjects = ruNEW CRuArrayList<IRuObject *>();
}

inline CRuCollisionQuery_Frustum_SubObject::~CRuCollisionQuery_Frustum_SubObject()
{
	delete m_colObjects;
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Sphere_HitTest::CRuCollisionQuery_Sphere_HitTest()
{
	// Set query type to a sphere query
	m_queryType = ruCOLQUERY_SPHERE_HITTEST;

	// Reset result
	m_positiveHit = FALSE;

	// Set collision distance to max float value
	m_colDistance = FLT_MAX;
}

inline CRuCollisionQuery_Sphere_HitTest::~CRuCollisionQuery_Sphere_HitTest()
{
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Sphere_TriEnum::CRuCollisionQuery_Sphere_TriEnum()
{
	// Set query type to a sphere query
	m_queryType = ruCOLQUERY_SPHERE_TRIENUM;

	// Create arrays to hold query results
	m_colTriVerts = ruNEW CRuArrayList<CRuVector3>();
	m_colTriNormals = ruNEW CRuArrayList<CRuVector3>();
	m_colTriFlags = ruNEW CRuArrayList<UINT32>();
}

inline CRuCollisionQuery_Sphere_TriEnum::~CRuCollisionQuery_Sphere_TriEnum()
{
	delete m_colTriVerts;
	delete m_colTriNormals;
	delete m_colTriFlags;
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_Enumerate::CRuCollisionQuery_Enumerate()
{
	// Set query type to a query returning triangle enumerations
	m_queryType = ruCOLQUERY_ENUMERATE;

	// Create arrays to hold query results
	m_colTriVerts = ruNEW CRuArrayList<CRuVector3>();
}

inline CRuCollisionQuery_Enumerate::~CRuCollisionQuery_Enumerate()
{
	delete m_colTriVerts;
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_User_Flag_Set::CRuCollisionQuery_User_Flag_Set()
{
	// Set query type to a query returning triangle enumerations
	m_queryType = ruCOLQUERY_USER_FLAG_SET;

	// Reset results
	m_numAffectedPrimitives = 0;
	m_affectedObjects = ruNEW CRuArrayList<void *>();
}

inline CRuCollisionQuery_User_Flag_Set::~CRuCollisionQuery_User_Flag_Set()
{
	delete m_affectedObjects;
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_User_Flag_Set_Ray::CRuCollisionQuery_User_Flag_Set_Ray()
{
	// Set query type to a query returning triangle enumerations
	m_queryType = ruCOLQUERY_USER_FLAG_SET_RAY;

	// Reset results
	m_numAffectedPrimitives = 0;
	m_affectedObjects = ruNEW CRuArrayList<void *>();
}

inline CRuCollisionQuery_User_Flag_Set_Ray::~CRuCollisionQuery_User_Flag_Set_Ray()
{
	delete m_affectedObjects;
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_User_Flag_Set_Sphere::CRuCollisionQuery_User_Flag_Set_Sphere()
{
	m_queryType = ruCOLQUERY_USER_FLAG_SET_SPHERE;

	// Reset results
	m_numAffectedPrimitives = 0;
	m_affectedObjects = ruNEW CRuArrayList<void *>();
}

inline CRuCollisionQuery_User_Flag_Set_Sphere::~CRuCollisionQuery_User_Flag_Set_Sphere()
{
	delete m_affectedObjects;
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_User_Flag_Get_Segment::CRuCollisionQuery_User_Flag_Get_Segment()
{
	// Set query type to a segment query
	m_queryType = ruCOLQUERY_USER_FLAG_GET_SEGMENT;

	// Default to ignore backfaces
	m_ignoreBackface = TRUE;

	// Reset result
	m_positiveHit = FALSE;
	m_collisionT = FLT_MAX;
	m_flag = 0;
}

inline CRuCollisionQuery_User_Flag_Get_Segment::~CRuCollisionQuery_User_Flag_Get_Segment()
{
}

// ************************************************************************************************************************************************************

inline CRuCollisionQuery_User_Flag_Enumerate::CRuCollisionQuery_User_Flag_Enumerate()
{
	// Set query type to a query returning triangle enumerations
	m_queryType = ruCOLQUERY_USER_FLAG_ENUMERATE;

	// Create arrays to hold query results
	m_colTriVerts = ruNEW CRuArrayList<CRuVector3>();
	m_colTriFlags = ruNEW CRuArrayList<UINT32>();
}

inline CRuCollisionQuery_User_Flag_Enumerate::~CRuCollisionQuery_User_Flag_Enumerate()
{
	delete m_colTriVerts;
	delete m_colTriFlags;
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
