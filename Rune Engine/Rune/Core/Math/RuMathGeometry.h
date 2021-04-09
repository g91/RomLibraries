/*!
	@project Rune
	@file RuMathGeometry.h

	Copyright (c) 2004 All rights reserved

	@author Ceranos
	@date 2004-03-09
*/

#ifndef _RUMATHGEOMETRY_H_
#define _RUMATHGEOMETRY_H_

#include "../../Core/Math/RuMathBase.h"
#include "../../Core/Math/RuMathVector.h"

#pragma unmanaged
#pragma pack(16) /* Must ensure class & union 16-B aligned */

// ************************************************************************************************************************************************************

class CRuOBB;
class CRuSphere;

// ************************************************************************************************************************************************************

enum RuOrthogonalAxis
{
	ruAXIS_Invalid						= 0,
	ruAXIS_X_POSITIVE					= 1,
	ruAXIS_Y_POSITIVE					= 2,
	ruAXIS_Z_POSITIVE					= 3,
	ruAXIS_X_NEGATIVE					= 4,
	ruAXIS_Y_NEGATIVE					= 5,
	ruAXIS_Z_NEGATIVE					= 6,
	ruAXIS_FORCE_DWORD					= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuOrthogonalPlane
{
	ruPLANE_Invalid						= 0,
	ruPLANE_XY							= 1,
	ruPLANE_YZ							= 2,
	ruPLANE_XZ							= 3,
	ruPLANE_FORCE_DWORD					= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuIntersectResult
{
	ruINTRESULT_NoIntersect				= 0,
	ruINTRESULT_Intersect				= 1,
	ruINTRESULT_Colinear				= 2,
	ruINTRESULT_Coplanar				= 3,
	ruINTRESULT_FORCE_DWORD				= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

class CRuPoint2 : public CRuVector2
{
};

// ************************************************************************************************************************************************************

class CRuPoint3 : public CRuVector3
{
};

// ************************************************************************************************************************************************************

class CRuPoint4 : public CRuVector4
{
};

// ************************************************************************************************************************************************************

// Segment is defined as P(t) = Origin + t * Direction, where t = [0, 1] and Direction is not necessarily unit length.
class CRuSegment2
{
private:
	CRuVector2					m_origin;
	CRuVector2					m_direction;

public:
	inline						CRuSegment2() { }
	inline						CRuSegment2(const CRuSegment2 &segment) : m_origin(segment.m_origin), m_direction(segment.m_direction) { }
	inline						CRuSegment2(const CRuVector2 &pt0, const CRuVector2 &pt1) { SetFromPoints(pt0, pt1); }
								~CRuSegment2() { }

	// Segment initialization
	inline void					Set(const CRuSegment2 &segment);
	inline void					SetFromPoints(const CRuVector2 &pt0, const CRuVector2 &pt1);
	inline void					SetFromRay(const CRuVector2 &rayOrigin, const CRuVector2 &rayDirection);

	// C#-style(almost) property accessors
	inline CRuVector2&			Origin() { return m_origin; }
	inline CRuVector2&			Direction() { return m_direction; }
	inline const CRuVector2&	Origin() const { return m_origin; }
	inline const CRuVector2&	Direction() const { return m_direction; }

	// Property accessors
	inline CRuVector2&			GetOrigin() { return m_origin; }
	inline CRuVector2&			GetDirection() { return m_direction; }
	inline const CRuVector2&	GetOrigin() const { return m_origin; }
	inline const CRuVector2&	GetDirection() const { return m_direction; }
};

// ************************************************************************************************************************************************************

// Segment is defined as P(t) = Origin + t * Direction, where t = [0, 1] and Direction is not necessarily unit length.
class CRuSegment3
{
private:
	CRuVector3					m_origin;
	CRuVector3					m_direction;

public:
	inline						CRuSegment3() { }
	inline						CRuSegment3(const CRuSegment3 &segment) : m_origin(segment.m_origin), m_direction(segment.m_direction) { }
	inline						CRuSegment3(const CRuVector3 &pt0, const CRuVector3 &pt1) { SetFromPoints(pt0, pt1); }
								~CRuSegment3() { }

	// Segment initialization
	inline void					Set(const CRuSegment3 &segment);
	inline void					SetFromPoints(const CRuVector3 &pt0, const CRuVector3 &pt1);
	inline void					SetFromRay(const CRuVector3 &rayOrigin, const CRuVector3 &rayDirection);

	// C#-style(almost) property accessors
	inline CRuVector3&			Origin() { return m_origin; }
	inline CRuVector3&			Direction() { return m_direction; }
	inline const CRuVector3&	Origin() const { return m_origin; }
	inline const CRuVector3&	Direction() const { return m_direction; }

	// Property accessors
	inline CRuVector3&			GetOrigin() { return m_origin; }
	inline CRuVector3&			GetDirection() { return m_direction; }
	inline const CRuVector3&	GetOrigin() const { return m_origin; }
	inline const CRuVector3&	GetDirection() const { return m_direction; }

	float						GetSquareDistance(const CRuSegment3 &segment) const;
};

// ************************************************************************************************************************************************************

// Line is defined as P(t) = Origin + t * Direction, where t = [-oo, +oo] and direction is unit length.
class CRuLine3
{
public:
	CRuVector3					m_origin;
	CRuVector3					m_direction;

public:
	inline						CRuLine3() { }
	inline						CRuLine3(const CRuLine3 &line) : m_origin(line.m_origin), m_direction(line.m_direction) { }
	inline						CRuLine3(const CRuVector3 &lineOrigin, const CRuVector3 &lineDirection, BOOL normalizeDirection = FALSE);
	inline						CRuLine3(float oX, float oY, float oZ, float dX, float dY, float dZ, BOOL normalizeDirection = FALSE);
	inline						~CRuLine3() { }

	inline CRuVector3&			Origin() { return m_origin; }
	inline CRuVector3&			Direction() { return m_direction; }
	inline const CRuVector3&	Origin() const { return m_origin; }
	inline const CRuVector3&	Direction() const { return m_direction; }

	inline CRuVector3&			GetOrigin() { return m_origin; }
	inline CRuVector3&			GetDirection() { return m_direction; }
	inline const CRuVector3&	GetOrigin() const { return m_origin; }
	inline const CRuVector3&	GetDirection() const { return m_direction; }
};

// ************************************************************************************************************************************************************

// Ray is defined as P(t) = Origin + t * Direction, where t = [0, 1] and direction is unit length.
class CRuRay3
{
private:
	CRuVector3					m_origin;
	CRuVector3					m_direction;

public:
	inline						CRuRay3() { }
	inline						CRuRay3(const CRuRay3 &ray) : m_origin(ray.m_origin), m_direction(ray.m_direction) { }
								CRuRay3(const CRuRay3 &ray, const CRuMatrix4x4 &transform);
	inline						CRuRay3(const CRuVector3 &rayOrigin, const CRuVector3 &rayDirection, BOOL normalizeDirection = FALSE);
	inline						CRuRay3(float oX, float oY, float oZ, float dX, float dY, float dZ, BOOL normalizeDirection = FALSE);
	inline						~CRuRay3() { }

	inline CRuVector3&			Origin() { return m_origin; }
	inline CRuVector3&			Direction() { return m_direction; }
	inline const CRuVector3&	Origin() const { return m_origin; }
	inline const CRuVector3&	Direction() const { return m_direction; }

	inline CRuVector3&			GetOrigin() { return m_origin; }
	inline CRuVector3&			GetDirection() { return m_direction; }
	inline const CRuVector3&	GetOrigin() const { return m_origin; }
	inline const CRuVector3&	GetDirection() const { return m_direction; }
};

// ************************************************************************************************************************************************************

class CRuRectangle
{
public:
	float						m_left;
	float						m_top;
	float						m_right;
	float						m_bottom;

public:
								CRuRectangle(float left = 0.0f, float top = 0.0f, float right = 0.0f, float bottom = 0.0f) { m_top = top; m_left = left; m_bottom = bottom; m_right = right; }

	inline void					Set(float left, float top, float right, float bottom) { m_top = top; m_left = left; m_bottom = bottom; m_right = right; }

	// Arithmetic update operator overloads
	inline CRuRectangle&		operator/=(float scale) { m_top /= scale; m_left /= scale; m_bottom /= scale; m_right /= scale; return *this; }
	inline CRuRectangle&		operator*=(float scale) { m_top *= scale; m_left *= scale; m_bottom *= scale; m_right *= scale; return *this; }
};

// ************************************************************************************************************************************************************

class CRuAABB
{
private:
	CRuVector3					m_minPoint;						// Minimum extents of AABB
	CRuVector3					m_maxPoint;						// Maximum extents of AABB

public:
								CRuAABB();
								CRuAABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
								CRuAABB(const CRuVector3 &minima, const CRuVector3 &maxima);
								CRuAABB(const CRuAABB &box);		// Copy constructor
								CRuAABB(const CRuAABB &box0, const CRuAABB &box1);
								CRuAABB(const CRuOBB &box);
								~CRuAABB();

	// Setup
	void						Set(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

	void						SetBounds(const CRuAABB &box);
	void						SetBounds(const CRuVector3 &minima, const CRuVector3 &maxima);
	void						SetBounds(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
	void						SetBoundsToQuadrantXZ(const CRuAABB &box, int quadIndex);
	void						SetBoundsToOctant(const CRuAABB &box, int octantIndex);

	// Box merging
	void						Merge(const CRuAABB &box);
	void						Merge(const CRuSphere &sphere);
	void						Merge(const CRuVector3 &point);

	void						Intersection(const CRuAABB &box);

	// "Deformations" (this being an AABB, valid deformations are extremely limited)
	void						Translate(const CRuVector3 &vec);
	void						Translate(float x, float y, float z);
	void						Transform(const CRuMatrix4x4 &mat);

	inline CRuVector3&			Minima() { return m_minPoint; }
	inline CRuVector3&			Maxima() { return m_maxPoint; }
	inline const CRuVector3&	Minima() const { return m_minPoint; }
	inline const CRuVector3&	Maxima() const { return m_maxPoint; }

	// Queries
	inline CRuVector3&			GetMinPoint() { return m_minPoint; }
	inline CRuVector3&			GetMaxPoint() { return m_maxPoint; }
	inline const CRuVector3&	GetMinPoint() const { return m_minPoint; }
	inline const CRuVector3&	GetMaxPoint() const { return m_maxPoint; }
	inline float				GetXSize() const { return m_maxPoint.x - m_minPoint.x; }
	inline float				GetYSize() const { return m_maxPoint.y - m_minPoint.y; }
	inline float				GetZSize() const { return m_maxPoint.z - m_minPoint.z; }
	inline float				GetVolume() const { return (m_maxPoint.x - m_minPoint.x) * (m_maxPoint.y - m_minPoint.y) * (m_maxPoint.z - m_minPoint.z); }
	float						GetSurfaceArea() const;
	
	// Collision queries
	BOOL						IsCoincident(const CRuAABB &boxB) const;
	BOOL						Contains(const CRuAABB &boxB) const;
	BOOL						Contains(const CRuSphere &sphere) const;
	BOOL						Contains(const CRuVector3 &pt) const;
	BOOL						Intersects(const CRuVector3 &origin, const CRuVector3 &direction) const;
	BOOL						Intersects(const CRuVector3 &origin, const CRuVector3 &direction, float &tOut) const;
	BOOL						Intersects(const CRuVector3 &origin, const CRuVector3 &direction, int &faceIDOut) const;
	BOOL						Intersects(const CRuVector3 &origin, const CRuVector3 &direction, CRuVector3 &intPointOut) const;
	BOOL						Intersects(const CRuVector3 &origin, const CRuVector3 &direction, CRuVector3 &intPointOut, float &tOut) const;
	BOOL						Intersects(const CRuAABB &boxB) const;
	BOOL						Intersects(const CRuAABB &boxB, RuOrthogonalPlane checkPlane) const;
	BOOL						Intersects(const CRuAABB &boxB, BOOL strict) const;
	BOOL						Intersects(const CRuAABB &boxB, RuOrthogonalPlane checkPlane, BOOL strict) const;
	BOOL						IntersectsTriangle(const CRuVector3 &tv0, const CRuVector3 &tv1, const CRuVector3 &tv2) const;
	BOOL						IntersectsFrustum(CRuPlane4 *planes) const;
	BOOL						IntersectsFrustum(CRuPlane4 *planes, INT32 numPlanes) const;
	int							InsideFrustum(CRuPlane4 *planes) const;

	// Sort-of collision queries
	BOOL						IsSnappable(const CRuAABB &boxB, float &snapDistanceOut) const;
};

// ************************************************************************************************************************************************************

class CRuOBB
{
private:
	CRuVector3					m_center;						// Center of OBB
	CRuVector3					m_axis[3];						// Orienting axis set
	float						m_extents[3];					// Extents of each orienting axis (x2 = extents of OBB in each axial direction)

public:
								CRuOBB();
								CRuOBB(const CRuOBB &obb);
								CRuOBB(const CRuVector3 &center, CRuVector3 extents[3]);
								CRuOBB(const CRuVector3 &center, CRuVector3 axis[3], float extents[3]);
								CRuOBB(const CRuAABB &aabb);
								CRuOBB(const CRuAABB &aabb, const CRuMatrix4x4 &transform);
								CRuOBB(const CRuSphere &sphere);
								~CRuOBB();

	// Setup
	void						Set(const CRuVector3 &center, CRuVector3 extents[3]);
	void						Set(const CRuVector3 &center, CRuVector3 axis[3], float extents[3]);
	void						Set(const CRuVector3 &center, const CRuVector3 &extents);
	void						Set(const CRuAABB &aabb);
	void						Set(const CRuAABB &aabb, const CRuMatrix4x4 &transform);
	void						Set(const CRuSphere &sphere);

	// Transform
	void						Transform(const CRuMatrix4x4 &transform);

	// C#-style(almost) property accessors
	inline CRuVector3&			Center() { return m_center; }
	inline CRuVector3*			Axis() { return m_axis; }
	inline float*				Extents() { return m_extents; }
	inline const CRuVector3&	Center() const { return m_center; }
	inline const CRuVector3*	Axis() const { return m_axis; }
	inline const float*			Extents() const { return m_extents; }

	// Property accessors
	inline CRuVector3&			GetCenter() { return m_center; }
	inline CRuVector3*			GetAxis() { return m_axis; }
	inline float*				GetExtents() { return m_extents; }
	inline const CRuVector3&	GetCenter() const { return m_center; }
	inline const CRuVector3*	GetAxis() const { return m_axis; }
	inline const float*			GetExtents() const { return m_extents; }

	// Simple yes-no collision queries
	BOOL						Contains(const CRuVector3 &pt) const;

	BOOL						Intersects(const CRuOBB &obb) const;
	BOOL						Intersects(const CRuVector3 &origin, const CRuVector3 &direction) const;

	// Collision queries
	BOOL						Intersects(const CRuVector3 &origin, const CRuVector3 &direction, CRuVector3 &intPointOut) const;

	// Other stuff
	CRuVector3					GetClosestPoint(const CRuVector3 &pt);
	REAL						GetDistance(const CRuVector3 &pt);

	// Point-fitting
	void						ComputeBestFitOBB(CRuVector3 *points, INT32 numPoints);

	CRuOBB						ComputeOBB(CRuVector3 *points, INT32 numPoints, CRuMatrix4x4 *matrix);

	// Queries
	void						GetVertices(CRuVector3 vertices[8]) const;

private:
	// Intersection testing helpers
	static BOOL					Clip(float denominator, float numerator, float &t0, float &t1);
	static BOOL					FindIntersection(const CRuVector3 &origin, const CRuVector3 &direction, const float extents[3], float &t0, float &t1);
};

// ************************************************************************************************************************************************************

class CRuCapsule
{
private:
	CRuSegment3					m_segment;				// Segment along which this capsule is defined
	float						m_radius;				// Radius along the defined segment

public:
								CRuCapsule();
								CRuCapsule(const CRuCapsule &capsule);
								CRuCapsule(const CRuSegment3 &segment, const float radius);

	void						Set(const CRuCapsule &capsule);
	void						Set(const CRuSegment3 &segment, const float radius);

	// C#-style(almost) property accessors
	inline CRuVector3&			Origin() { return m_segment.Origin(); }
	inline CRuVector3&			Direction() { return m_segment.Direction(); }
	inline CRuSegment3&			Segment() { return m_segment; }
	inline float&				Radius() { return m_radius; }
	inline const CRuVector3&	Origin() const { return m_segment.Origin(); }
	inline const CRuVector3&	Direction() const { return m_segment.Direction(); }
	inline const CRuSegment3&	Segment() const { return m_segment; }
	inline float				Radius() const { return m_radius; }

	// Property accessors
	inline CRuVector3&			GetOrigin() { return m_segment.Origin(); }
	inline CRuVector3&			GetDirection() { return m_segment.Direction(); }
	inline CRuSegment3&			GetSegment() { return m_segment; }
	inline float&				GetRadius() { return m_radius; }
	inline const CRuVector3&	GetOrigin() const { return m_segment.Origin(); }
	inline const CRuVector3&	GetDirection() const { return m_segment.Direction(); }
	inline const CRuSegment3&	GetSegment() const { return m_segment; }
	inline float				GetRadius() const { return m_radius; }

	// Collision queries
	BOOL						Intersects(const CRuSegment3 &segment) const;
};

// ************************************************************************************************************************************************************

class CRuCylinder
{
private:
	CRuSegment3					m_segment;				// Segment along which this cylinder is defined
	float						m_radius;				// Radius of the cylinder

public:
								CRuCylinder();
								CRuCylinder(const CRuCylinder &cylinder);
								CRuCylinder(const CRuSegment3 &segment, const float radius);

	void						Set(const CRuCylinder &cylinder);
	void						Set(const CRuSegment3 &segment, const float radius);

	// C#-style(almost) property accessors
	inline CRuVector3&			Origin() { return m_segment.Origin(); }
	inline CRuVector3&			Direction() { return m_segment.Direction(); }
	inline CRuSegment3&			Segment() { return m_segment; }
	inline float&				Radius() { return m_radius; }
	inline const CRuVector3&	Origin() const { return m_segment.Origin(); }
	inline const CRuVector3&	Direction() const { return m_segment.Direction(); }
	inline const CRuSegment3&	Segment() const { return m_segment; }
	inline float				Radius() const { return m_radius; }

	// Property accessors
	inline CRuVector3&			GetOrigin() { return m_segment.Origin(); }
	inline CRuVector3&			GetDirection() { return m_segment.Direction(); }
	inline CRuSegment3&			GetSegment() { return m_segment; }
	inline float&				GetRadius() { return m_radius; }
	inline const CRuVector3&	GetOrigin() const { return m_segment.Origin(); }
	inline const CRuVector3&	GetDirection() const { return m_segment.Direction(); }
	inline const CRuSegment3&	GetSegment() const { return m_segment; }
	inline float				GetRadius() const { return m_radius; }
};

// ************************************************************************************************************************************************************

class CRuSphere
{
private:
	CRuVector3					m_center;
	float						m_radius;

public:
								CRuSphere();
								CRuSphere(const CRuSphere &sphere);
								CRuSphere(const CRuVector3 &center, const float radius);
								~CRuSphere();

	void						Set(const CRuSphere &sphere);
	void						Set(const CRuVector3 &center, const float radius);

	// C#-style(almost) property accessors
	inline CRuVector3&			Center() { return m_center; }
	inline float&				Radius() { return m_radius; }
	inline const CRuVector3&	Center() const { return m_center; }
	inline float				Radius() const { return m_radius; }

	// Property accessors
	inline CRuVector3&			GetCenter() { return m_center; }
	inline float&				GetRadius() { return m_radius; }
	inline const CRuVector3&	GetCenter() const { return m_center; }
	inline float				GetRadius() const { return m_radius; }

	inline void					Merge(const CRuSphere &sphere) { Merge(sphere.m_center, sphere.m_radius); }
	void						Merge(const CRuVector3 &center, float radius);

	// Collision queries
	BOOL						Contains(const CRuVector3 &pt) const;
	BOOL						Contains(const CRuSphere &sphere) const;
	BOOL						Intersects(const CRuSegment3 &segment) const;
	BOOL						SweptIntersects(const CRuSegment3 &segment, const CRuVector3 &sweepRay) const;
	BOOL						SweptIntersectsTriangle(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, const CRuVector3 &sweepRay, float &tFirst) const;
	BOOL						SweptIntersectsTriangleList(const CRuVector3 &sweepRay, float &tFirst, const CRuVector3 *vertices, const UINT16 *indices, UINT32 numTriangles) const;

	BOOL						FindIntersection(const CRuSegment3 &segment, int &numIntersectionsOut, CRuVector3 intPointsOut[2]) const;

	inline CRuSphere&			operator=(const CRuSphere &sphere) { m_center = sphere.m_center; m_radius = sphere.m_radius; return *this; }
	inline BOOL					operator==(const CRuSphere &sphere) const { return (m_radius == sphere.m_radius && m_center == sphere.m_center) ? TRUE : FALSE; }

private:
	BOOL						FindTriSphrCoplanarIntersection(int vertIdx, const CRuVector3 triVertices[3], const CRuVector3 &sideNormal, const CRuVector3 &side, const CRuVector3 &sweepRay, float &rfTFirst, float fTMax) const;
	BOOL						FindSphereVertexIntersection(const CRuVector3 &vertex, const CRuVector3 &sweepRay, float &rfTFirst, float fTMax) const;
};

// ************************************************************************************************************************************************************

class CRuAAEllipsoid
{
private:
	CRuVector3					m_center;									//!< Center of the ellipsoid
	CRuVector3					m_radius;									//!< Radius of the ellipsoid along each world axis

public:
								CRuAAEllipsoid();
								CRuAAEllipsoid(const CRuAAEllipsoid &ellipsoid);
								CRuAAEllipsoid(const CRuVector3 &center, const CRuVector3 &radius);

	void						Set(const CRuAAEllipsoid &ellipsoid);
	void						Set(const CRuVector3 &center, const CRuVector3 &radius);

	inline CRuVector3&			Center() { return m_center; }
	inline CRuVector3&			Radius() { return m_radius; }
	inline const CRuVector3&	Center() const { return m_center; }
	inline const CRuVector3&	Radius() const { return m_radius; }

	CRuSphere					GetBoundingSphere() const;
};

// ************************************************************************************************************************************************************

class CRuAABB2D
{
private:
	CRuVector2					m_minima;
	CRuVector2					m_maxima;

public:
								CRuAABB2D(const CRuAABB2D &aabb2D);
								CRuAABB2D(const CRuVector2 &minima, const CRuVector2 &maxima);
								~CRuAABB2D();

	inline CRuVector2&			Minima() { return m_minima; }
	inline CRuVector2&			Maxima() { return m_maxima; }
	inline const CRuVector2&	Minima() const { return m_minima; }
	inline const CRuVector2&	Maxima() const { return m_maxima; }

	BOOL						GetEdge(INT32 edgeIndex, CRuSegment2 &edgeOut) const;
};

// ************************************************************************************************************************************************************

// Compute the minimum volume sphere containing the input set of points.  The
// algorithm randomly permutes the input points so that the construction
// occurs in 'expected' O(N) time.

class CRuMinSphereBuilder
{
private:
	// indices of points that support current minimum volume sphere
	class Support
	{
	public:
		BOOL Contains(int index, CRuVector3 **points, float epsilon)
		{
			for(int i = 0; i < Quantity; i++)
			{
				CRuVector3 diff = *points[index] - *points[Index[i]];
				if(diff.SquaredMagnitude() < epsilon)
					return TRUE;
			}

			return FALSE;
		}

		int Quantity;
		int Index[4];
	};

	float			m_epsilon;

public:
					CRuMinSphereBuilder();

	BOOL			BuildSphere(const CRuVector3 *points, int numPoints, CRuSphere &minimalSphereOut);

private:
	// test if point P is inside sphere S
	BOOL			Contains(const CRuVector3 &point, const CRuSphere &sphere, REAL &distDiff);

	CRuSphere		ExactSphere1(const CRuVector3 &point);
	CRuSphere		ExactSphere2(const CRuVector3 &point0, const CRuVector3 &point1);
	CRuSphere		ExactSphere3(const CRuVector3 &point0, const CRuVector3 &point1, const CRuVector3 &point2);
	CRuSphere		ExactSphere4(const CRuVector3 &point0, const CRuVector3 &point1, const CRuVector3 &point2, const CRuVector3 &point3);

	CRuSphere		UpdateSupport1(int i, CRuVector3 **permutation, Support &support);
	CRuSphere		UpdateSupport2(int i, CRuVector3 **permutation, Support &support);
	CRuSphere		UpdateSupport3(int i, CRuVector3 **permutation, Support &support);
	CRuSphere		UpdateSupport4(int i, CRuVector3 **permutation, Support &support);
};

// ************************************************************************************************************************************************************

inline void CRuSegment2::Set(const CRuSegment2 &segment)
{
	m_origin = segment.m_origin;
	m_direction = segment.m_direction;
}

inline void CRuSegment2::SetFromPoints(const CRuVector2 &pt0, const CRuVector2 &pt1)
{
	m_origin = pt0;
	m_direction.x = pt1.x - pt0.x;
	m_direction.y = pt1.y - pt0.y;
}

inline void CRuSegment2::SetFromRay(const CRuVector2 &rayOrigin, const CRuVector2 &rayDirection)
{
	m_origin = rayOrigin;
	m_direction = rayDirection;
}

// ************************************************************************************************************************************************************

inline void CRuSegment3::Set(const CRuSegment3 &segment)
{
	m_origin = segment.m_origin;
	m_direction = segment.m_direction;
}

inline void CRuSegment3::SetFromPoints(const CRuVector3 &pt0, const CRuVector3 &pt1)
{
	m_origin = pt0;
	m_direction.x = pt1.x - pt0.x;
	m_direction.y = pt1.y - pt0.y;
	m_direction.z = pt1.z - pt0.z;
}

inline void CRuSegment3::SetFromRay(const CRuVector3 &rayOrigin, const CRuVector3 &rayDirection)
{
	m_origin = rayOrigin;
	m_direction = rayDirection;
}

// ************************************************************************************************************************************************************

/*!
	CRuLine3::CRuLine3(const CRuVector3 &lineOrigin, const CRuVector3 &lineDirection, BOOL normalizeDirection)
	Constructor
	@param lineOrigin Origin of the line
	@param lineDirection Direction of the line
	@param normalizeDirection Whether to normalize the line direction vector
	@return none
*/
inline CRuLine3::CRuLine3(const CRuVector3 &lineOrigin, const CRuVector3 &lineDirection, BOOL normalizeDirection)
: m_origin(lineOrigin), m_direction(lineDirection)
{
	if(normalizeDirection)
	{
		m_direction.Normalize();
	}
}

/*!
	CRuLine3::CRuLine3(float oX, float oY, float oZ, float dX, float dY, float dZ, BOOL normalizeDirection)
	Constructor
	@param oX X component for the origin of the line
	@param oY Y component for the origin of the line
	@param oZ Z component for the origin of the line
	@param dX X component for the direction of the line
	@param dY Y component for the direction of the line
	@param dZ Z component for the direction of the line
	@param normalizeDirection Whether to normalize the line direction vector
	@return none
*/
inline CRuLine3::CRuLine3(float oX, float oY, float oZ, float dX, float dY, float dZ, BOOL normalizeDirection)
: m_origin(oX, oY, oZ), m_direction(dX, dY, dZ)
{
	if(normalizeDirection)
	{
		m_direction.Normalize();
	}
}

/*!
	CRuRay3::CRuRay3(const CRuVector3 &rayOrigin, const CRuVector3 &rayDirection, BOOL normalizeDirection)
	Constructor
	@param rayOrigin Origin of the ray
	@param rayDirection Direction of the ray
	@param normalizeDirection Whether to normalize the ray direction vector
	@return none
*/
inline CRuRay3::CRuRay3(const CRuVector3 &rayOrigin, const CRuVector3 &rayDirection, BOOL normalizeDirection)
: m_origin(rayOrigin), m_direction(rayDirection)
{
	if(normalizeDirection && m_direction.Magnitude() > ruEPSILON)
	{
		m_direction.Normalize();
	}
}

/*!
	CRuRay3::CRuRay3(float oX, float oY, float oZ, float dX, float dY, float dZ, BOOL normalizeDirection)
	Constructor
	@param oX X component for the origin of the ray
	@param oY Y component for the origin of the ray
	@param oZ Z component for the origin of the ray
	@param dX X component for the direction of the ray
	@param dY Y component for the direction of the ray
	@param dZ Z component for the direction of the ray
	@param normalizeDirection Whether to normalize the ray direction vector
	@return none
*/
inline CRuRay3::CRuRay3(float oX, float oY, float oZ, float dX, float dY, float dZ, BOOL normalizeDirection)
: m_origin(oX, oY, oZ), m_direction(dX, dY, dZ)
{
	if(normalizeDirection)
	{
		m_direction.Normalize();
	}
}

// ************************************************************************************************************************************************************

inline float CRuAABB::GetSurfaceArea() const
{
	CRuVector3 sides(m_maxPoint);
	sides -= m_minPoint;

	return (sides.x * sides.y + sides.x * sides.z + sides.y * sides.z) * 2.0f;
}

// ************************************************************************************************************************************************************

#pragma pack()
#pragma managed

#endif
