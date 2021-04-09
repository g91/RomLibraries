#ifndef _RUMATH_H_
#define _RUMATH_H_

#include "../../Core/Math/RuVectorMath.h"

#pragma unmanaged

// Spline interpolators
extern CRuVector3	CatmullRomSpline(const CRuVector3 &p1, const CRuVector3 &p2, const CRuVector3 &p3, const CRuVector3 &p4, float t);
extern REAL			CatmullRomSpline(REAL p1, REAL p2, REAL p3, REAL p4, float t);

extern CRuVector3&	HermiteSpline(const CRuVector3 &p1, const CRuVector3 &p2, const CRuVector3 &d1, const CRuVector3 &d2, float t, CRuVector3& output);
extern float		HermiteSpline(float p1, float p2, float d1, float d2, float t);
extern CRuVector3&	HermiteSpline(const CRuVector3 &p0, const CRuVector3 &p1, const CRuVector3 &p2, float t, CRuVector3& output);
extern float		HermiteSpline(float p0, float p1, float p2, float t);

#pragma managed

#endif
