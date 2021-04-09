#ifndef _RUMATHDISTANCE_H_
#define _RUMATHDISTANCE_H_

#include "../../Core/Math/RuMathBase.h"
#include "../../Core/Math/RuMathVector.h"
#include "../../Core/Math/RuMathGeometry.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

extern REAL				SquaredDistance(const CRuVector3 &point, const CRuRay3 &ray);
extern REAL				SquaredDistance(const CRuVector3 &point, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2);
extern REAL				SquaredDistance(const CRuVector3 &point, const CRuAABB &box);
extern REAL				SquaredDistance(const CRuVector3 &point, const CRuSphere &sphere);

extern REAL				Distance(const CRuAABB &aabb, const CRuVector3 &pt);
extern REAL				Distance(const CRuSphere &sphere, const CRuVector3 &pt);

extern CRuVector3		ClosestPointOnSegment(const CRuVector3 &a, const CRuVector3 &b, const CRuVector3 &point);
extern CRuVector3		ClosestPointOnLine(const CRuVector3 &a, const CRuVector3 &b, const CRuVector3 &point);
extern CRuVector3		ClosestPointOnTriangle(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, const CRuVector3 &point);
extern CRuVector3		ClosestPointOnAABB(const CRuAABB &box, const CRuVector3 &point);

inline BOOL				IsPointInsideEdge(const CRuVector3 &a, const CRuVector3 &b, const CRuVector3 &p)
						{
							return DotProduct(CRuVector3(a - p), CRuVector3(b - p)) <= 0.0f;
						}


// ************************************************************************************************************************************************************

#pragma managed

#endif
