#ifndef _RUMATHCOLLISION_H_
#define _RUMATHCOLLISION_H_

#include "../../Core/Math/RuMathBase.h"
#include "../../Core/Math/RuMathVector.h"
#include "../../Core/Math/RuMathGeometry.h"
#include "../../Core/Math/RuMathDistance.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

extern BOOL					Intersects(const CRuAABB &box, const CRuSphere &sphere);
extern BOOL					Intersects(const CRuOBB &obb1, const CRuOBB &obb2);
extern BOOL					Intersects(const CRuOBB &obb, const CRuRay3 &ray);
extern BOOL					Intersects(const CRuOBB &obb, const CRuAABB &box);
extern BOOL					Intersects(const CRuOBB &obb, const CRuSphere &sphere);
extern BOOL					Intersects(const CRuSphere &sphere0, const CRuSphere &sphere1);
extern BOOL					Intersects(const CRuCapsule &capsule, const CRuSegment3 &segment);

inline BOOL					Intersects(const CRuAABB &box, const CRuOBB &obb) { return Intersects(obb, box); }

extern BOOL					Intersects(const CRuAABB &box0, const CRuAABB &box1);

extern BOOL					Intersects(const CRuRay3 &ray, const CRuAABB &box);
extern BOOL					Intersects(const CRuRay3 &ray, const CRuSphere &sphere);

extern BOOL					Intersects(const CRuSegment3 &segment, const CRuAABB &box);
extern BOOL					Intersects(const CRuSegment3 &segment, const CRuSphere &sphere);

extern BOOL					Intersects(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2);

extern BOOL					IntersectsTriangle(const CRuSphere &sphere, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2);
extern BOOL					IntersectsTriangle(const CRuAABB &box, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2);
extern BOOL					IntersectsTriangle(const CRuOBB &obb, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2);
extern BOOL					IntersectsTriangleList(const CRuSphere &sphere, const CRuVector3 *vertices, const UINT16 *indices, UINT32 numTriangles);

extern BOOL					IntersectsFrustum(const CRuPlane4 *frustumPlanes, const CRuAABB &box);
extern BOOL					IntersectsFrustum(const CRuPlane4 *frustumPlanes, const CRuSphere &sphere);

extern BOOL					FindIntersection(const CRuRay3 &ray, const CRuAABB &box, CRuVector3 &intPtOut, REAL &intTOut);
extern BOOL					FindIntersection(const CRuRay3 &ray, const CRuOBB &obb, CRuVector3 &intPtOut, REAL &intTOut);
extern BOOL					FindIntersection(const CRuRay3 &ray, const CRuPlane4 &plane, CRuVector3 &intPtOut, REAL &intTOut);
extern BOOL					FindIntersection(const CRuRay3 &ray, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, CRuVector3 &intPtOut, REAL &intTOut);
extern BOOL					FindIntersection(const CRuSegment2 &segment, const CRuAABB2D &box, CRuVector2 &intPtOut, REAL &intTOut);
extern BOOL					FindIntersection(const CRuSegment3 &segment, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, CRuVector3 &intPtOut, REAL &intTOut);
extern BOOL					FindIntersection(const CRuSegment3 &segment, const CRuAABB &box, CRuVector3 &intPtOut, REAL &intTOut);
extern BOOL					FindIntersection(const CRuPlane4 &plane0, const CRuPlane4 &plane1, const CRuPlane4 &plane2, CRuVector3 &intPtOut);
extern BOOL					FindIntersection(const CRuRay3 &ray, const CRuSphere &sphere, REAL &intTOut);
extern INT32				FindIntersection(const CRuLine3 &line, const CRuCylinder &cylinder, REAL *afT);
extern BOOL					FindIntersection(const CRuSegment3 &segment, const CRuCylinder &cylinder, REAL &intTOut);
extern BOOL					FindIntersection(const CRuRay3 &ray, const CRuCylinder &cylinder, REAL &intTOut);
extern BOOL					FindIntersection(const CRuSphere &sphere, const CRuVector3 &velocity, const CRuPlane4 &plane, REAL &tOut, CRuVector3 &pOut);

extern BOOL					InsideTriangle(const CRuVector3 &tv0, const CRuVector3 &tv1, const CRuVector3 &tv2, const CRuVector3 &tn, const CRuVector3 &point);

// Intersection testing (deprecated)
extern RuIntersectResult	Intersects(const CRuPlane4 &plane, const CRuSegment3 &segment, CRuVector3 &intPtOut);
extern RuIntersectResult	Intersects(const CRuPlane4 &plane, const CRuVector3 &origin, const CRuVector3 &direction, CRuVector3 &intPtOut);
extern BOOL					RayIntersectsTriangle(const CRuVector3 &origin, const CRuVector3 &direction, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2);
extern BOOL					RayIntersectsTriangle(const CRuVector3 &origin, const CRuVector3 &direction, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, REAL &uOut, REAL &vOut, REAL &tOut);

// ************************************************************************************************************************************************************

// inlined helpers

// ************************************************************************************************************************************************************

#pragma managed

#endif
