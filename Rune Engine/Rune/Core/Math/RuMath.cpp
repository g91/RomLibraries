#include "../../Core/Math/RuMath.h"

#pragma unmanaged

/*
	Description	: Interpolates a Catmull-Rom spline.
	Parameters	: Four points (p1 - p4) which form the spline, parameter t in [0, 1].
	Output		: Interpolated point between p2 and p3 in output. Also returns reference to output.
*/
CRuVector3 CatmullRomSpline(const CRuVector3 &p1, const CRuVector3 &p2, const CRuVector3 &p3, const CRuVector3 &p4, float t)
{
	CRuVector3 output;

	// Calculate tangents for p2 and p3
	CRuVector3 d2 = (p3 - p1) * 0.5f;
	CRuVector3 d3 = (p4 - p2) * 0.5f;

	REAL tSqr = t * t;
	REAL tCube = t * tSqr;

	output = p2 * (2 * tCube - 3 * tSqr + 1) + p3 * (-2 * tCube + 3 * tSqr) + d2 * (tCube - 2 * tSqr + t) + d3 * (tCube - tSqr);

	return output;
}

REAL CatmullRomSpline(REAL p1, REAL p2, REAL p3, REAL p4, float t)
{
	REAL output;

	// Calculate tangents for p2 and p3
	REAL d2 = (p3 - p1) * 0.5f;
	REAL d3 = (p4 - p2) * 0.5f;

	REAL tSqr = t * t;
	REAL tCube = t * tSqr;

	output = p2 * (2 * tCube - 3 * tSqr + 1) + p3 * (-2 * tCube + 3 * tSqr) + d2 * (tCube - 2 * tSqr + t) + d3 * (tCube - tSqr);

	return output;
}

/*
	Description	: Interpolates a basic hermite spline. t = 0 returns p1, t = 1 returns p2.
	Parameters	: Start and end points (p1 & p2), entry and exit slopes (d1 & d2), parameter t in [0, 1].
	Output		: Interpolated point between p1 and p2 in output. Also returns reference to output.
*/
CRuVector3 &HermiteSpline(const CRuVector3 &p1, const CRuVector3 &p2, const CRuVector3 &d1, const CRuVector3 &d2, float t, CRuVector3 &output)
{
	float tSqr = t * t;
	float tCube = t * tSqr;

	output = p1 * (2 * tCube - 3 * tSqr + 1) + p2 * (-2 * tCube + 3 * tSqr) + d1 * (tCube - 2 * tSqr + t) + d2 * (tCube - tSqr);

	return output;
}

/*
	Description	: Interpolates a basic hermite spline. t = 0 returns p1, t = 1 returns p2.
	Parameters	: Start and end points (p1 & p2), entry and exit slopes (d1 & d2), parameter t in [0, 1].
	Output		: Returns interpolated point between p1 and p2.
*/
float HermiteSpline(float p1, float p2, float d1, float d2, float t)
{
	float output;
	float tSqr = t * t;
	float tCube = t * tSqr;

	output = p1 * (2 * tCube - 3 * tSqr + 1) + p2 * (-2 * tCube + 3 * tSqr) + d1 * (tCube - 2 * tSqr + t) + d2 * (tCube - tSqr);

	return output;
}

/*
	Description	: Interpolates a simple three data point hermite spline.. t = 0 returns p1, t = 1 returns p2. 
	Parameters	: Sample points p0 - p2, parameter t in [0, 1]. Slopes are generated from the p0->p1 and p1->p2 segments.
	Output		: Returns interpolated point between p1 and p2. Also returns reference to output.
*/
CRuVector3 &HermiteSpline(const CRuVector3 &p0, const CRuVector3 &p1, const CRuVector3 &p2, float t, CRuVector3& output)
{
	return HermiteSpline(p1, p2, p1 - p0, p2 - p1, t, output);
}

/*
	Description	: Interpolates a simple three data point hermite spline.. t = 0 returns p1, t = 1 returns p2. 
	Parameters	: Sample points p0 - p2, parameter t in [0, 1]. Slopes are generated from the p0->p1 and p1->p2 segments.
	Output		: Returns interpolated point between p1 and p2. Also returns reference to output.
*/
float HermiteSpline(float p0, float p1, float p2, float t)
{
	return HermiteSpline(p1, p2, p1 - p0, p2 - p1, t);
}

#pragma managed
