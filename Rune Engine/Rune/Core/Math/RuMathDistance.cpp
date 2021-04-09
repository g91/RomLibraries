#include "../../Core/Math/RuMathDistance.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	REAL SquaredDistance(const CRuVector3 &point, const CRuRay3 &ray)
	Returns the distance between a point and a ray
	@param point
	@param ray
	@return REAL
*/
REAL SquaredDistance(const CRuVector3 &point, const CRuRay3 &ray)
{
	CRuVector3 diff = point - ray.Origin();
	REAL t = diff.Dot(ray.Direction());

	if(t <= 0.0f)
	{
		t = 0.0f;
	}
	else
	{
		t /= ray.Direction().SquaredMagnitude();
		diff -= ray.Direction() * t;
	}

	return diff.SquaredMagnitude();
}

// ************************************************************************************************************************************************************

/*!
	REAL SquaredDistance(const CRuVector3 &point, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2)
	Returns the distance between a point and a triangle
	@param point
	@param v0
	@param v1
	@param v2
	@return REAL
*/
REAL SquaredDistance(const CRuVector3 &point, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2)
{
	CRuVector3 e0(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
	CRuVector3 e1(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
	CRuVector3 diff(v0.x - point.x, v0.y - point.y, v0.z - point.z);

	REAL a00 = e0.SquaredMagnitude();
	REAL a01 = e0.Dot(e1);
	REAL a11 = e1.SquaredMagnitude();
	REAL b0 = diff.Dot(e0);
	REAL b1 = diff.Dot(e1);
	REAL c = diff.SquaredMagnitude();
	REAL det = (REAL) fabs(a00 * a11 - a01 * a01);
	REAL s = a01 * b1 - a11 * b0;
	REAL t = a01 * b0 - a00 * b1;
	REAL sqrDist;

	if(s + t <= det)
	{
		if(s < 0.0f)
		{
			if(t < 0.0f)  // region 4
			{
				if(b0 < 0.0f)
				{
					t = 0.0f;
					if(-b0 >= a00)
					{
						s = 1.0f;
						sqrDist = a00 + 2.0f * b0 + c;
					}
					else
					{
						s = -b0 / a00;
						sqrDist = b0 * s + c;
					}
				}
				else
				{
					s = 0.0f;
					if(b1 >= 0.0f)
					{
						t = 0.0f;
						sqrDist = c;
					}
					else if(-b1 >= a11)
					{
						t = 1.0f;
						sqrDist = a11 + 2.0f * b1 + c;
					}
					else
					{
						t = -b1 / a11;
						sqrDist = b1 * t + c;
					}
				}
			}
			else  // region 3
			{
				s = 0.0f;
				if(b1 >= 0.0f)
				{
					t = 0.0f;
					sqrDist = c;
				}
				else if(-b1 >= a11)
				{
					t = 1.0f;
					sqrDist = a11 + 2.0f * b1 + c;
				}
				else
				{
					t = -b1 / a11;
					sqrDist = b1 * t + c;
				}
			}
		}
		else if(t < 0.0f)  // region 5
		{
			t = 0.0f;
			if(b0 >= 0.0f)
			{
				s = 0.0f;
				sqrDist = c;
			}
			else if(-b0 >= a00)
			{
				s = 1.0f;
				sqrDist = a00 + 2.0f * b0 + c;
			}
			else
			{
				s = -b0 / a00;
				sqrDist = b0 * s + c;
			}
		}
		else  // region 0
		{
			// minimum at interior point
			REAL fInvDet = (1.0f) / det;
			s *= fInvDet;
			t *= fInvDet;
			sqrDist = s * (a00 * s + a01 * t + 2.0f * b0) + t * (a01 * s + a11 * t + 2.0f * b1) + c;
		}
	}
	else
	{
		REAL fTmp0, fTmp1, fNumer, fDenom;

		if(s < 0.0f)  // region 2
		{
			fTmp0 = a01  +  b0;
			fTmp1 = a11  +  b1;
			if(fTmp1 > fTmp0)
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = a00-2.0f * a01 + a11;
				if(fNumer >= fDenom)
				{
					s = 1.0f;
					t = 0.0f;
					sqrDist = a00 + 2.0f * b0 + c;
				}
				else
				{
					s = fNumer / fDenom;
					t = 1.0f - s;
					sqrDist = s * (a00 * s + a01 * t + 2.0f * b0) + t * (a01 * s + a11 * t + 2.0f * b1) + c;
				}
			}
			else
			{
				s = 0.0f;
				if(fTmp1 <= 0.0f)
				{
					t = 1.0f;
					sqrDist = a11 + 2.0f * b1 + c;
				}
				else if(b1 >= 0.0f)
				{
					t = 0.0f;
					sqrDist = c;
				}
				else
				{
					t = -b1 / a11;
					sqrDist = b1 * t + c;
				}
			}
		}
		else if(t < 0.0f)  // region 6
		{
			fTmp0 = a01  +  b1;
			fTmp1 = a00  +  b0;
			if(fTmp1 > fTmp0)
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = a00-2.0f * a01 + a11;
				if(fNumer >= fDenom)
				{
					t = 1.0f;
					s = 0.0f;
					sqrDist = a11 + 2.0f * b1 + c;
				}
				else
				{
					t = fNumer / fDenom;
					s = 1.0f - t;
					sqrDist = s * (a00 * s + a01 * t + 2.0f * b0) + t * (a01 * s + a11 * t + 2.0f * b1) + c;
				}
			}
			else
			{
				t = 0.0f;
				if(fTmp1 <= 0.0f)
				{
					s = 1.0f;
					sqrDist = a00 + 2.0f * b0 + c;
				}
				else if(b0 >= 0.0f)
				{
					s = 0.0f;
					sqrDist = c;
				}
				else
				{
					s = -b0 / a00;
					sqrDist = b0 * s + c;
				}
			}
		}
		else  // region 1
		{
			fNumer = a11  +  b1 - a01 - b0;
			if(fNumer <= 0.0f)
			{
				s = 0.0f;
				t = 1.0f;
				sqrDist = a11 + 2.0f * b1 + c;
			}
			else
			{
				fDenom = a00-2.0f * a01 + a11;
				if(fNumer >= fDenom)
				{
					s = 1.0f;
					t = 0.0f;
					sqrDist = a00 + 2.0f * b0 + c;
				}
				else
				{
					s = fNumer / fDenom;
					t = 1.0f - s;
					sqrDist = s * (a00 * s + a01 * t + 2.0f * b0) + t * (a01 * s + a11 * t + 2.0f * b1) + c;
				}
			}
		}
	}

	return (REAL) fabs(sqrDist);
}

// ************************************************************************************************************************************************************

CRuVector3 ClosestPointOnAABB(const CRuAABB &box, const CRuVector3 &point)
{
	CRuVector3 closestPoint(point);

	if(point.x < box.Minima().x)
	{
		closestPoint.x = box.Minima().x;
	}
	else if(point.x > box.Maxima().x)
	{
		closestPoint.x = box.Maxima().x;
	}

	if(point.y < box.Minima().y)
	{
		closestPoint.y = box.Minima().y;
	}
	else if(point.y > box.Maxima().y)
	{
		closestPoint.y = box.Maxima().y;
	}

	if(point.z < box.Minima().z)
	{
		closestPoint.z = box.Minima().z;
	}
	else if(point.z > box.Maxima().z)
	{
		closestPoint.z = box.Maxima().z;
	}

	return closestPoint;
}

REAL SquaredDistance(const CRuVector3 &point, const CRuAABB &box)
{
	REAL sqrDistance = 0.0f;
	REAL delta;

	if(point.x < box.Minima().x)
	{
		delta = point.x - box.Minima().x;
		sqrDistance += delta * delta;
	}
	else if(point.x > box.Maxima().x)
	{
		delta = point.x - box.Maxima().x;
		sqrDistance += delta * delta;
	}

	if(point.y < box.Minima().y)
	{
		delta = point.y - box.Minima().y;
		sqrDistance += delta * delta;
	}
	else if(point.y > box.Maxima().y)
	{
		delta = point.y - box.Maxima().y;
		sqrDistance += delta * delta;
	}

	if(point.z < box.Minima().z)
	{
		delta = point.z - box.Minima().z;
		sqrDistance += delta * delta;
	}
	else if(point.z > box.Maxima().z)
	{
		delta = point.z - box.Maxima().z;
		sqrDistance += delta * delta;
	}

	return sqrDistance;
}

// ************************************************************************************************************************************************************

REAL SquaredDistance(const CRuVector3 &point, const CRuSphere &sphere)
{
	float sqrRadius = sphere.Radius() * sphere.Radius();
	float distanceToCenter = (point - sphere.Center()).SquaredMagnitude();

	if(distanceToCenter >= sqrRadius)
	{
		return distanceToCenter - sqrRadius;
	}

	return 0.0f;
}

// ************************************************************************************************************************************************************

REAL Distance(const CRuAABB &aabb, const CRuVector3 &pt)
{
	CRuVector3 delta(0.0f, 0.0f, 0.0f);

	if(pt.x < aabb.GetMinPoint().x)
		delta.x = aabb.GetMinPoint().x - pt.x;
	else if(pt.x > aabb.GetMaxPoint().x)
		delta.x = pt.x - aabb.GetMaxPoint().x;

	if(pt.y < aabb.GetMinPoint().y)
		delta.y = aabb.GetMinPoint().y - pt.y;
	else if(pt.y > aabb.GetMaxPoint().y)
		delta.y = pt.y - aabb.GetMaxPoint().y;

	if(pt.z < aabb.GetMinPoint().z)
		delta.z = aabb.GetMinPoint().z - pt.z;
	else if(pt.z > aabb.GetMaxPoint().z)
		delta.z = pt.z - aabb.GetMaxPoint().z;

	return delta.Magnitude();
}

// ************************************************************************************************************************************************************

REAL Distance(const CRuSphere &sphere, const CRuVector3 &pt)
{
	float distanceToCenter = (pt - sphere.Center()).Magnitude();

	if(distanceToCenter >= sphere.Radius())
	{
		return distanceToCenter - sphere.Radius();
	}

	return 0.0f;
}

// ************************************************************************************************************************************************************

CRuVector3 ClosestPointOnSegment(const CRuVector3 &a, const CRuVector3 &b, const CRuVector3 &point)
{
	CRuVector3 ab = b - a;
	REAL abDotab = DotProduct(ab, ab);

	// ab dot ab is zero... a and b are the same point!
	if(abDotab < ruEPSILON)
	{
		return a;
	}

	REAL t = DotProduct(point - a, ab) / abDotab;

	if(t < 0.0f)
		t = 0.0f;
	if(t > 1.0f)
		t = 1.0f;

	return a + ab * t;
/*
	// Determine t (the length of the vector from a to point)
	CRuVector3 c = point - a;
	CRuVector3 v = b - a;
	v.Normalize();

	REAL d = (a - b).Magnitude();
	REAL t = DotProduct(v, c);

	// Check to see if t is beyond the extents of the line segment
	if(t < 0.0f)
		return a;
	if(t > d)
		return b;

	// Return the point between a and b
	return a + v * t;
*/
}

// ************************************************************************************************************************************************************

CRuVector3 ClosestPointOnLine(const CRuVector3 &a, const CRuVector3 &b, const CRuVector3 &point)
{
	// Determine t (the length of the vector from a to point)
	CRuVector3 c = point - a;
	CRuVector3 v = b - a;
	v.Normalize();

	REAL d = (a - b).Magnitude();
	REAL t = DotProduct(v, c);

	// Return the point between a and b
	return a + v * t;
}

// ************************************************************************************************************************************************************

CRuVector3 ClosestPointOnTriangle(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, const CRuVector3 &point)
{
	// Check if point is in vertex region outside v0
	CRuVector3 ab = v1 - v0;
	CRuVector3 ac = v2 - v0;
	CRuVector3 ap = point - v0;

	REAL d1 = DotProduct(ab, ap);
	REAL d2 = DotProduct(ac, ap);

	// Barycentric coordinates (1, 0, 0)
	if(d1 <= 0.0f && d2 <= 0.0f)
		return v0;

	// Check if point is in vertex region outside v1
	CRuVector3 bp = point - v1;
	REAL d3 = DotProduct(ab, bp);
	REAL d4 = DotProduct(ac, bp);

	// Barycentric coordinates (0, 1, 0)
	if(d3 >= 0.0f && d4 <= d3)
		return v1;

	// Check if point is in edge region of v0-v1, if so return projection of point onto v0-v1
	REAL vc = d1 * d4 - d3 * d2;
	if(vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		REAL v = d1 / (d1 - d3);
		return v0 + ab * v;
	}

	// Check if point is in vertex region outside C
	CRuVector3 cp = point - v2;
	REAL d5 = DotProduct(ab, cp);
	REAL d6 = DotProduct(ac, cp);

	// Barycentric coordinates (0, 0, 1)
	if(d6 >= 0.0f && d5 <= d6)
		return v2;

	// Check if point is in edge region of v0-v2, if so return projection of point onto v0-v2
	REAL vb = d5 * d2 - d1 * d6;
	if(vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		REAL w = d2 / (d2 - d6);
		return v0 + ac * w;
	}

	// Check if point is in edge region of v1-v2, if so return projection of point onto v1-v2
	REAL va = d3 * d6 - d5 * d4;
	if(va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		REAL w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return v1 + (v2 - v1) * w;
	}

	// point is inside face region, compute Q through its barycentric coordinates (u, v, w)
	REAL denom = 1.0f / (va + vb + vc);
	REAL v = vb * denom;
	REAL w = vc * denom;
	return v0 + ab * v + ac * w;
}

// ************************************************************************************************************************************************************

#pragma managed
