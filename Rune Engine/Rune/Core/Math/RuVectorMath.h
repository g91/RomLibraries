// *** AuVectorMath.h ***

#ifndef _RUVECTORMATH_H_
#define _RUVECTORMATH_H_

#include <math.h>

#include "RuMathBase.h"
#include "RuMathVector.h"
#include "RuMathGeometry.h"
#include "RuMathCollision.h"

#pragma unmanaged

// NOTE!!!! MAKE MY OWN SINE & COSINE FUNCTIONS!
// NOTE!!!! BY DEFAULT, VECTORS AND MATRICES ARE ASSUMED TO BE ROW-MAJOR
// NOTE!!!! LEFT-HANDED COORDINATE SYSTEM

// the rest of this should mostly go to AuMathCollision.h... maybe separate into AuMathDistance.h as well

// For legacy compatibility
typedef CRuVector2 CVector2;
typedef CRuVector3 CRuVector3;
typedef CRuVector4 CVector4;
typedef CRuPlane4 CPlane4;
typedef CRuMatrix4x4 CMatrix4x4;
typedef CRuQuaternion CQuaternion;
typedef CRuAABB CAABB;
typedef CRuOBB COBB;

#pragma pack(push, 16) /* Must ensure class & union 16-B aligned */

#pragma pack(pop) 

// Miscellaneous support
extern float			GetDistance(const CRuAABB &aabb, const CRuVector3 &pt);
extern float			GetAngle(const CRuVector2 &src, const CRuVector2 &dst);

// Frustum-planes support
extern void				ExtractPlanesFromTransform(CRuPlane4 *planes, const CRuMatrix4x4 &transform, BOOL normalize = TRUE);

// Clipping
extern UINT32			ClipPolygon(CRuVector3 *verticesIn, UINT32 numVerticesIn, CRuVector3 *verticesOut, const CRuPlane4 &plane);
extern UINT32			ClipTriangle(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, const CRuPlane4 &plane, CRuVector3 *verticesOut);

// Triangle support
extern BOOL				TrianglesAreIdentical(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2);
extern BOOL				TrianglesAreAdjacent(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2);
extern BOOL				TriangleSharesEdge(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, CRuVector3 &nonadjacentVertex);
extern float			GetTriangleYDistance(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2);

// Intersection testing
extern float			FindClosestPointOnSegment(const CRuVector3 &u0, const CRuVector3 &v0, const CRuVector3 &v1, float &pfParam);
extern BOOL				FindSegmentIntersection(const CRuVector2 &u0, const CRuVector2 &u1, const CRuVector2 &v0, const CRuVector2 &v1, int &riQuantity, float afT[2]);
extern BOOL				FindTriangleIntersection(const CRuVector2 &u0, const CRuVector2 &u1, const CRuVector2 &u2, const CRuVector2 &v0, const CRuVector2 &v1, const CRuVector2 &v2, int &riQuantity, CRuVector2 akVertex[6]);



// *** Lots and lots of other crap ***

//extern BOOL CoplanarTriangles(const CRuVector3 &n, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, const CRuVector3 &u0,const CRuVector3 &u1,const CRuVector3 &u2);
//extern BOOL TriangleIntersectsTriangle(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2);
//extern BOOL FindTriangleIntersection(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, BOOL &coplanar, CRuVector3 &iPoint1Out, CRuVector3 &iPoint2Out);

//extern BOOL TriangleIntersectsTriangle(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2);
//extern BOOL FindTriangleIntersection(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, int &numIntPoints, CRuVector3 intPoints[6]);

extern BOOL tri_tri_intersection_test_3d(const CRuVector3 &p1, const CRuVector3 &q1, const CRuVector3 &r1, const CRuVector3 &p2, const CRuVector3 &q2, const CRuVector3 &r2, BOOL &coplanar, CRuVector3 &source, CRuVector3 &target);


extern float PointToLineSquareDistance(const CRuVector3 &pt, const CRuVector3 &v0, const CRuVector3 &v1);

extern int SplitTriangle(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, const CRuVector3 &p0, const CRuVector3 &p1, CRuVector3 *vOut);

#pragma managed


#endif
