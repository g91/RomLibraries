#include "../../Core/Math/RuMathCollision.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

BOOL Intersects(const CRuAABB &box, const CRuSphere &sphere)
{
	REAL s, d = 0.0f; 

	// Find the square of the distance from the sphere to the box
	if(sphere.Center().x < box.Minima().x)
	{
		s = sphere.Center().x - box.Minima().x;
		d += s * s; 
	}
	else if(sphere.Center().x > box.Maxima().x)
	{
		s = sphere.Center().x - box.Maxima().x;
		d += s * s; 
	}

	if(sphere.Center().y < box.Minima().y)
	{
		s = sphere.Center().y - box.Minima().y;
		d += s * s; 
	}
	else if(sphere.Center().y > box.Maxima().y)
	{
		s = sphere.Center().y - box.Maxima().y;
		d += s * s; 
	}

	if(sphere.Center().z < box.Minima().z)
	{
		s = sphere.Center().z - box.Minima().z;
		d += s * s; 
	}
	else if(sphere.Center().z > box.Maxima().z)
	{
		s = sphere.Center().z - box.Maxima().z;
		d += s * s; 
	}

	return d <= sphere.Radius() * sphere.Radius();
}

// ************************************************************************************************************************************************************

BOOL Intersects(const CRuOBB &obb1, const CRuOBB &obb2)
{
	// WARNING: NOT YET TESTED
	// Fairly standard separation of axis theorem implementation

	// Cutoff for cosine of angles between box axes.  This is used to catch
	// the cases when at least one pair of axes are parallel.  If this happens,
	// there is no need to test for separation along the Cross(A[i],B[j])
	// directions.
	const REAL fCutoff = 0.999999f;
	BOOL parallelPairExists = FALSE;
	int i;

	// convenience variables
	const CRuVector3 *axisA = obb1.Axis();
	const CRuVector3 *axisB = obb2.Axis();
	const REAL *extentsA = obb1.Extents();
	const REAL *extentsB = obb2.Extents();

	// compute difference of box centers, D = C1-C0
	CRuVector3 diff = obb2.Center() - obb1.Center();

	REAL matrixC[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
	REAL absC[3][3];  // |c_{ij}|
	REAL ad[3];        // Dot(A_i,D)
	REAL r0, r1, r;   // interval radii and distance between centers
	REAL r01;           // = R0 + R1

	// axis C0 + t * A0
	for(i = 0; i < 3; i++)
	{
		matrixC[0][i] = axisA[0].Dot(axisB[i]);
		absC[0][i] = (REAL) fabs(matrixC[0][i]);
		if(absC[0][i] > fCutoff)
			parallelPairExists = TRUE;
	}
	ad[0] = axisA[0].Dot(diff);
	r = (REAL) fabs(ad[0]);
	r1 = extentsB[0] * absC[0][0] + extentsB[1] * absC[0][1] + extentsB[2] * absC[0][2];
	r01 = extentsA[0] + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A1
	for(i = 0; i < 3; i++)
	{
		matrixC[1][i] = axisA[1].Dot(axisB[i]);
		absC[1][i] = (REAL) fabs(matrixC[1][i]);
		if(absC[1][i] > fCutoff)
			parallelPairExists = TRUE;
	}
	ad[1] = axisA[1].Dot(diff);
	r = (REAL) fabs(ad[1]);
	r1 = extentsB[0] * absC[1][0] + extentsB[1] * absC[1][1] + extentsB[2] * absC[1][2];
	r01 = extentsA[1] + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A2
	for(i = 0; i < 3; i++)
	{
		matrixC[2][i] = axisA[2].Dot(axisB[i]);
		absC[2][i] = (REAL) fabs(matrixC[2][i]);
		if(absC[2][i] > fCutoff)
			parallelPairExists = TRUE;
	}
	ad[2] = axisA[2].Dot(diff);
	r = (REAL) fabs(ad[2]);
	r1 = extentsB[0] * absC[2][0] + extentsB[1] * absC[2][1] + extentsB[2] * absC[2][2];
	r01 = extentsA[2] + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * B0
	r = (REAL) fabs(axisB[0].Dot(diff));
	r0 = extentsA[0] * absC[0][0] + extentsA[1] * absC[1][0] + extentsA[2] * absC[2][0];
	r01 = r0 + extentsB[0];
	if(r > r01)
		return FALSE;

	// axis C0 + t * B1
	r = (REAL) fabs(axisB[1].Dot(diff));
	r0 = extentsA[0] * absC[0][1] + extentsA[1] * absC[1][1] + extentsA[2] * absC[2][1];
	r01 = r0 + extentsB[1];
	if(r > r01)
		return FALSE;

	// axis C0 + t * B2
	r = (REAL) fabs(axisB[2].Dot(diff));
	r0 = extentsA[0] * absC[0][2] + extentsA[1] * absC[1][2] + extentsA[2] * absC[2][2];
	r01 = r0 + extentsB[2];
	if(r > r01)
		return FALSE;

	// At least one pair of box axes was parallel, so the separation is
	// effectively in 2D where checking the "edge" normals is sufficient for
	// the separation of the boxes.
	if(parallelPairExists)
		return TRUE;

	// axis C0 + t * A0xB0
	r = (REAL) fabs(ad[2] * matrixC[1][0] - ad[1] * matrixC[2][0]);
	r0 = extentsA[1] * absC[2][0] + extentsA[2] * absC[1][0];
	r1 = extentsB[1] * absC[0][2] + extentsB[2] * absC[0][1];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A0xB1
	r = (REAL) fabs(ad[2] * matrixC[1][1] - ad[1] * matrixC[2][1]);
	r0 = extentsA[1] * absC[2][1] + extentsA[2] * absC[1][1];
	r1 = extentsB[0] * absC[0][2] + extentsB[2] * absC[0][0];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A0xB2
	r = (REAL) fabs(ad[2] * matrixC[1][2] - ad[1] * matrixC[2][2]);
	r0 = extentsA[1] * absC[2][2] + extentsA[2] * absC[1][2];
	r1 = extentsB[0] * absC[0][1] + extentsB[1] * absC[0][0];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A1xB0
	r = (REAL) fabs(ad[0] * matrixC[2][0] - ad[2] * matrixC[0][0]);
	r0 = extentsA[0] * absC[2][0] + extentsA[2] * absC[0][0];
	r1 = extentsB[1] * absC[1][2] + extentsB[2] * absC[1][1];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A1xB1
	r = (REAL) fabs(ad[0] * matrixC[2][1] - ad[2] * matrixC[0][1]);
	r0 = extentsA[0] * absC[2][1] + extentsA[2] * absC[0][1];
	r1 = extentsB[0] * absC[1][2] + extentsB[2] * absC[1][0];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A1xB2
	r = (REAL) fabs(ad[0] * matrixC[2][2] - ad[2] * matrixC[0][2]);
	r0 = extentsA[0] * absC[2][2] + extentsA[2] * absC[0][2];
	r1 = extentsB[0] * absC[1][1] + extentsB[1] * absC[1][0];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A2xB0
	r = (REAL) fabs(ad[1] * matrixC[0][0] - ad[0] * matrixC[1][0]);
	r0 = extentsA[0] * absC[1][0] + extentsA[1] * absC[0][0];
	r1 = extentsB[1] * absC[2][2] + extentsB[2] * absC[2][1];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A2xB1
	r = (REAL) fabs(ad[1] * matrixC[0][1] - ad[0] * matrixC[1][1]);
	r0 = extentsA[0] * absC[1][1] + extentsA[1] * absC[0][1];
	r1 = extentsB[0] * absC[2][2] + extentsB[2] * absC[2][0];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A2xB2
	r = (REAL) fabs(ad[1] * matrixC[0][2] - ad[0] * matrixC[1][2]);
	r0 = extentsA[0] * absC[1][2] + extentsA[1] * absC[0][2];
	r1 = extentsB[0] * absC[2][1] + extentsB[1] * absC[2][0];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL Intersects(const CRuOBB &obb, const CRuRay3 &ray)
{
	REAL wDu[3], awDu[3], dDu[3], adDu[3], awXdDu[3], rhs;

	const CRuVector3 *axis = obb.Axis();
	const REAL *extents = obb.Extents();
	CRuVector3 diff = ray.Origin() - obb.Center();

	wDu[0] = ray.Direction().Dot(axis[0]);
	awDu[0] = (REAL) fabs(wDu[0]);
	dDu[0] = diff.Dot(axis[0]);
	adDu[0] = (REAL) fabs(dDu[0]);
	if(adDu[0] > extents[0] && dDu[0] * wDu[0] >= 0.0f)
		return FALSE;

	wDu[1] = ray.Direction().Dot(axis[1]);
	awDu[1] = (REAL) fabs(wDu[1]);
	dDu[1] = diff.Dot(axis[1]);
	adDu[1] = (REAL) fabs(dDu[1]);
	if(adDu[1] > extents[1] && dDu[1] * wDu[1] >= 0.0f)
		return FALSE;

	wDu[2] = ray.Direction().Dot(axis[2]);
	awDu[2] = (REAL) fabs(wDu[2]);
	dDu[2] = diff.Dot(axis[2]);
	adDu[2] = (REAL) fabs(dDu[2]);
	if(adDu[2] > extents[2] && dDu[2] * wDu[2] >= 0.0f)
		return FALSE;

	CRuVector3 wXd = CrossProduct(ray.Direction(), diff);

	awXdDu[0] = (REAL) fabs(wXd.Dot(axis[0]));
	rhs = extents[1] * awDu[2] + extents[2] * awDu[1];
	if(awXdDu[0] > rhs)
		return FALSE;

	awXdDu[1] = (REAL) fabs(wXd.Dot(axis[1]));
	rhs = extents[0] * awDu[2] + extents[2] * awDu[0];
	if(awXdDu[1] > rhs)
		return FALSE;

	awXdDu[2] = (REAL) fabs(wXd.Dot(axis[2]));
	rhs = extents[0] * awDu[1] + extents[1] * awDu[0];
	if(awXdDu[2] > rhs)
		return FALSE;

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL Intersects(const CRuOBB &obb, const CRuAABB &box)
{
	// Convert box into an obb
	CRuMatrix4x4 identityTransform;
	identityTransform.SetToIdentity();
	CRuOBB obb2(box, identityTransform);

	return Intersects(obb, obb2);
}

// ************************************************************************************************************************************************************

BOOL Intersects(const CRuOBB &obb, const CRuSphere &sphere)
{
	CRuVector3 d = sphere.Center() - obb.Center();
	CRuVector3 q = obb.Center();

	for(INT32 i = 0; i < 3; ++i)
	{
		REAL dist = DotProduct(d, obb.Axis()[i]);

		// If distance is farther than the box extents, clamp to the box
		if(dist > obb.Extents()[i])
		{
			dist = obb.Extents()[i];
		}

		if(dist < -obb.Extents()[i])
		{
			dist = -obb.Extents()[i];
		}

		// Step that distance along the axis to get world coordinate
		q = q + obb.Axis()[i] * dist;
	}

	CRuVector3 v = q - sphere.Center();

	return DotProduct(v, v) <= sphere.Radius() * sphere.Radius();
}

// ************************************************************************************************************************************************************

BOOL Intersects(const CRuSphere &sphere0, const CRuSphere &sphere1)
{
	REAL sqrDist = (sphere0.Center() - sphere1.Center()).SquaredMagnitude();
	REAL contactDist = sphere0.Radius() + sphere1.Radius();

	return sqrDist <= (contactDist * contactDist);
}

// ************************************************************************************************************************************************************

BOOL Intersects(const CRuCapsule &capsule, const CRuSegment3 &segment)
{
	return capsule.Segment().GetSquareDistance(segment) <= (capsule.Radius() * capsule.Radius());
}

// ************************************************************************************************************************************************************

BOOL Intersects(const CRuAABB &box0, const CRuAABB &box1)
{
	if(box0.Minima().x > box1.Maxima().x || box1.Minima().x > box0.Maxima().x)
		return FALSE;

	if(box0.Minima().y > box1.Maxima().y || box1.Minima().y > box0.Maxima().y)
		return FALSE;

	if(box0.Minima().z > box1.Maxima().z || box1.Minima().z > box0.Maxima().z)
		return FALSE;

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL Intersects(const CRuRay3 &ray, const CRuAABB &box)
{
	BOOL inside = TRUE;
	int quadrant[3];		// 0 = larger than, 1 = smaller than, 2 = inside
	int i;
	int whichPlane;
	REAL maxT[3], candidatePlane[3];
	REAL intersectCoord[3];

	// Find candidate planes - this finds the three plane coordinates (orthogonal to each major axis) nearest the origin of the ray
	for(i = 0; i < 3; i++)
	{
		if(ray.Origin()[i] < box.Minima()[i])
		{
			quadrant[i] = 1;
			candidatePlane[i] = box.Minima()[i];
			inside = FALSE;
		}
		else if(ray.Origin()[i] > box.Maxima()[i])
		{
			quadrant[i] = 0;
			candidatePlane[i] = box.Maxima()[i];
			inside = FALSE;
		}
		else
			quadrant[i] = 2;
	}

	// Ray is inside the bounding box
	if(inside)
		return TRUE;

	// Calculate T distances to candidate planes
	for(i = 0; i < 3; i++)
	{
		if(quadrant[i] != 2 && ray.Direction()[i] != 0.0f)
			maxT[i] = (candidatePlane[i] - ray.Origin()[i]) / ray.Direction()[i];
		else
			maxT[i] = -1.0f;
	}

	// Get largest of the T distances as the final choice of intersection
	whichPlane = 0;
	for(i = 0; i < 3; i++)
	{
		if(maxT[whichPlane] < maxT[i])
			whichPlane = i;
	}

	// Check the final choice is actually inside the box
	if(maxT[whichPlane] < 0.0f)
		return FALSE;

	for(i = 0; i < 3; i++)
	{
		if(whichPlane != i)
		{
			intersectCoord[i] = ray.Origin()[i] + maxT[whichPlane] * ray.Direction()[i];
			if(intersectCoord[i] < box.Minima()[i] || intersectCoord[i] > box.Maxima()[i])
				return FALSE;
		}
		else
		{
			intersectCoord[i] = candidatePlane[i];
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL Intersects(const CRuRay3 &ray, const CRuSphere &sphere)
{
	REAL sqrDist = SquaredDistance(sphere.Center(), ray);
	return sqrDist <= sphere.Radius() * sphere.Radius();
}

// ************************************************************************************************************************************************************
BOOL Intersects(const CRuSegment3 &segment, const CRuAABB &box)
{
	CRuVector3 c = (box.Minima() + box.Maxima()) * 0.5f;			// Box center-point
	CRuVector3 e = box.Maxima() - c;								// Box half-length extents
	CRuVector3 d = segment.Direction() * 0.5f;						// Segment half-length vector
	CRuVector3 m = segment.Origin() + d;							// Segment midpoint

	m = m - c;														// Translate box and segment to origin

	// Try world coordinate axes as separating axes
	REAL adx = fabs(d.m_x);

	if(fabs(m.m_x) > e.m_x + adx)
		return FALSE;

	REAL ady = fabs(d.m_y);

	if(fabs(m.m_y) > e.m_y + ady)
		return FALSE;

	REAL adz = fabs(d.m_z);

	if(fabs(m.m_z) > e.m_z + adz)
		return FALSE;

	adx += ruEPSILON;
	ady += ruEPSILON;
	adz += ruEPSILON;

	// Try cross products of segment direction vector with coordinate axes
	if(fabs(m.m_y * d.m_z - m.m_z * d.m_y) > e.m_y * adz + e.m_z * ady)
		return FALSE;

	if(fabs(m.m_z * d.m_x - m.m_x * d.m_z) > e.m_x * adz + e.m_z * adx)
		return FALSE;

	if(fabs(m.m_x * d.m_y - m.m_y * d.m_x) > e.m_x * ady + e.m_y * adx)
		return FALSE;

	// No separating axis found; segment must be overlapping AABB
	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL Intersects(const CRuSegment3 &segment, const CRuSphere &sphere)
{
	CRuVector3 kDiff = segment.Origin() - sphere.Center();

	REAL fA0 = DotProduct(kDiff, kDiff) - sphere.Radius() * sphere.Radius();
	REAL fA1 = DotProduct(segment.Direction(), kDiff);
	REAL fDiscr = fA1 * fA1 - fA0;
	if(fDiscr < 0.0f)
		return FALSE;

	REAL segmentExtent = segment.Direction().SquaredMagnitude();

	REAL fTmp0 = segmentExtent * segmentExtent + fA0;
	REAL fTmp1 = 2.0f * fA1 * segmentExtent;
	REAL fQM = fTmp0 - fTmp1;
	REAL fQP = fTmp0 + fTmp1;
	if(fQM * fQP <= 0.0f )
		return TRUE;

	return fQM > 0.0f && fabs(fA1) < segmentExtent;
}

// ************************************************************************************************************************************************************

// Projects vertices of triangle onto vector rkD...? Then stores the minima & maxima of the three vertices...
void TriProjection(const CRuVector3& rkD, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, float &minima, float &maxima)
{
	float dotProducts[3] = { DotProduct(rkD, v0), DotProduct(rkD, v1), DotProduct(rkD, v2) };

	minima = dotProducts[0];
	maxima = minima;

	if(dotProducts[1] < minima)
		minima = dotProducts[1];
	else if(dotProducts[1] > maxima)
		maxima = dotProducts[1];

	if(dotProducts[2] < minima)
		minima = dotProducts[2];
	else if(dotProducts[2] > maxima)
		maxima = dotProducts[2];
}

// This is far more numerically stable....... fuck speed..
BOOL Intersects(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2)
{
	CRuVector3 direction;
	float uMinima, uMaxima, vMinima, vMaxima;
	int i0, i1;

	// direction N
	CRuVector3 e[3] = { u1 - u0, u2 - u1, u0 - u2 };
	CRuVector3 n = CrossProduct(e[0], e[1]);

	float nDu0 = n.Dot(u0);
	TriProjection(n, v0, v1, v2, vMinima, vMaxima);

	if(nDu0 < vMinima || nDu0 > vMaxima)
		return FALSE;

	// direction M
	CRuVector3 f[3] = { v1 - v0, v2 - v1, v0 - v2 };
	CRuVector3 m = CrossProduct(f[0], f[1]);
	CRuVector3 nXm = CrossProduct(n, m);

	if(nXm.Dot(nXm) >= FLT_EPSILON * (n.Dot(n) * m.Dot(m)))
	{
		// triangles are not parallel
		float mDv0 = m.Dot(v0);
		TriProjection(m, u0, u1, u2, uMinima, uMaxima);

		if(mDv0 < uMinima || mDv0 > uMaxima)
			return FALSE;

		// directions E[i0]xF[i1]
		for (i1 = 0; i1 < 3; i1++)
		{
			for (i0 = 0; i0 < 3; i0++)
			{
				direction = CrossProduct(e[i0], f[i1]);
				TriProjection(direction, u0, u1, u2, uMinima, uMaxima);
				TriProjection(direction, v0, v1, v2, vMinima, vMaxima);
				if(uMaxima < vMinima || vMaxima < uMinima)
					return FALSE;
			}
		}
	}
	else  // triangles are parallel (and, in fact, coplanar)
	{
		// directions NxE[i0]
		for (i0 = 0; i0 < 3; i0++)
		{
			direction = CrossProduct(n, e[i0]);
			TriProjection(direction, u0, u1, u2, uMinima, uMaxima);
			TriProjection(direction, v0, v1, v2, vMinima, vMaxima);
			if(uMaxima < vMinima || vMaxima < uMinima)
				return FALSE;
		}

		// directions NxF[i1]
		for (i1 = 0; i1 < 3; i1++)
		{
			direction = CrossProduct(m, f[i1]);
			TriProjection(direction, u0, u1, u2, uMinima, uMaxima);
			TriProjection(direction, v0, v1, v2, vMinima, vMaxima);
			if(uMaxima < vMinima || vMaxima < uMinima)
				return FALSE;
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL IntersectsTriangle(const CRuSphere &sphere, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2)
{
	return SquaredDistance(sphere.Center(), v0, v1, v2) < sphere.Radius() * sphere.Radius();
}

// ************************************************************************************************************************************************************

inline void FindMinMax(REAL x0, REAL x1, REAL x2, REAL &minOut, REAL &maxOut)
{
	minOut = maxOut = x0;

	if(x1 < minOut)
		minOut = x1;

	if(x1 > maxOut)
		maxOut = x1;

	if(x2 < minOut)
		minOut = x2;

	if(x2 > maxOut)
		maxOut = x2;
}

// ************************************************************************************************************************************************************

inline BOOL PlaneBoxOverlap(const CRuVector3 &normal, REAL d, const CRuVector3 &maxbox)
{
	CRuVector3 vmin, vmax;

	if(normal.x > 0.0f)
	{
		vmin.x = -maxbox.x;
		vmax.x = maxbox.x;
	}
	else
	{
		vmin.x = maxbox.x;
		vmax.x = -maxbox.x;
	}
	if(normal.y > 0.0f)
	{
		vmin.y = -maxbox.y;
		vmax.y = maxbox.y;
	}
	else
	{
		vmin.y = maxbox.y;
		vmax.y = -maxbox.y;
	}
	if(normal.z > 0.0f)
	{
		vmin.z = -maxbox.z;
		vmax.z = maxbox.z;
	}
	else
	{
		vmin.z = maxbox.z;
		vmax.z = -maxbox.z;
	}

	if(DotProduct(normal, vmin) + d > 0.0f)
		return FALSE;
	if(DotProduct(normal, vmax) + d >= 0.0f)
		return TRUE;

	return FALSE;
}

// ************************************************************************************************************************************************************

BOOL IntersectsTriangle(const CRuAABB &box, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2)
{
	CRuVector3 boxCenter, boxHalfSize, rv0, rv1, rv2, normal;
	CRuVector3 e0, e1, e2;
	REAL min, max, d, p0, p1, p2, rad, fex, fey, fez;  

	boxHalfSize = (box.Maxima() - box.Minima()) / 2.0f;
	boxCenter = box.Minima() + boxHalfSize;

	// Move everything so that the box center is at (0,0,0)
	rv0 = v0 - boxCenter;
	rv1 = v1 - boxCenter;
	rv2 = v2 - boxCenter;

	// Compute triangle edges
	e0 = rv1 - rv0;
	e1 = rv2 - rv1;
	e2 = rv0 - rv2;

	// Bullet 3:
	// Test the 9 tests first (this was faster)
	fex = (REAL) fabs(e0.x);
	fey = (REAL) fabs(e0.y);
	fez = (REAL) fabs(e0.z);

//	AXISTEST_X01(e0.z, e0.y, fez, fey);
	p0 = e0.z * rv0.y - e0.y * rv0.z;
	p2 = e0.z * rv2.y - e0.y * rv2.z;
	if(p0 < p2)
	{
		min = p0;
		max = p2;
	}
	else
	{
		min = p2;
		max = p0;
	}
	rad = fez * boxHalfSize.y + fey * boxHalfSize.z;
	if(min > rad || max < -rad)
		return FALSE;

//	AXISTEST_Y02(e0.z, e0.x, fez, fex);
	p0 = -e0.z * rv0.x + e0.x * rv0.z;
	p2 = -e0.z * rv2.x + e0.x * rv2.z;
	if(p0 < p2)
	{
		min = p0;
		max = p2;
	}
	else
	{
		min = p2;
		max = p0;
	}
	rad = fez * boxHalfSize.x + fex * boxHalfSize.z;
	if(min > rad || max < -rad)
		return FALSE;

//	AXISTEST_Z12(e0.y, e0.x, fey, fex);
	p1 = e0.y * rv1.x - e0.x * rv1.y;
	p2 = e0.y * rv2.x - e0.x * rv2.y;
	if(p2 < p1)
	{
		min = p2;
		max = p1;
	}
	else
	{
		min = p1;
		max = p2;
	}
	rad = fey * boxHalfSize.x + fex * boxHalfSize.y;
	if(min > rad || max < -rad)
		return FALSE;

	fex = (REAL) fabs(e1.x);
	fey = (REAL) fabs(e1.y);
	fez = (REAL) fabs(e1.z);
	
//	AXISTEST_X01(e1.z, e1.y, fez, fey);
	p0 = e1.z * rv0.y - e1.y * rv0.z;
	p2 = e1.z * rv2.y - e1.y * rv2.z;
	if(p0 < p2)
	{
		min = p0;
		max = p2;
	}
	else
	{
		min = p2;
		max = p0;
	}
	rad = fez * boxHalfSize.y + fey * boxHalfSize.z;
	if(min > rad || max < -rad)
		return FALSE;

//	AXISTEST_Y02(e1.z, e1.x, fez, fex);
	p0 = -e1.z * rv0.x + e1.x * rv0.z;
	p2 = -e1.z * rv2.x + e1.x * rv2.z;
	if(p0 < p2)
	{
		min = p0;
		max = p2;
	}
	else
	{
		min = p2;
		max = p0;
	}
	rad = fez * boxHalfSize.x + fex * boxHalfSize.z;
	if(min > rad || max < -rad)
		return FALSE;

//	AXISTEST_Z0(e1.y, e1.x, fey, fex);
	p0 = e1.y * rv0.x - e1.x * rv0.y;
	p1 = e1.y * rv1.x - e1.x * rv1.y;
	if(p0 < p1)
	{
		min = p0;
		max = p1;
	}
	else
	{
		min = p1;
		max = p0;
	}
	rad = fey * boxHalfSize.x + fex * boxHalfSize.y;
	if(min > rad || max < -rad)
		return FALSE;

	fex = (REAL) fabs(e2.x);
	fey = (REAL) fabs(e2.y);
	fez = (REAL) fabs(e2.z);

//	AXISTEST_X2(e2.z, e2.y, fez, fey);
	p0 = e2.z * rv0.y - e2.y * rv0.z;
	p1 = e2.z * rv1.y - e2.y * rv1.z;
	if(p0 < p1)
	{
		min = p0;
		max = p1;
	}
	else
	{
		min = p1;
		max = p0;
	}
	rad = fez * boxHalfSize.y + fey * boxHalfSize.z;
	if(min > rad || max < -rad)
		return FALSE;

//	AXISTEST_Y1(e2.z, e2.x, fez, fex);
	p0 = -e2.z * rv0.x + e2.x * rv0.z;
	p1 = -e2.z * rv1.x + e2.x * rv1.z;
	if(p0 < p1)
	{
		min = p0;
		max = p1;
	}
	else
	{
		min = p1;
		max = p0;
	}
	rad = fez * boxHalfSize.x + fex * boxHalfSize.z;
	if(min > rad || max < -rad)
		return FALSE;

//	AXISTEST_Z12(e2.y, e2.x, fey, fex);
	p1 = e2.y * rv1.x - e2.x * rv1.y;
	p2 = e2.y * rv2.x - e2.x * rv2.y;
	if(p2 < p1)
	{
		min = p2;
		max = p1;
	}
	else
	{
		min = p1;
		max = p2;
	}
	rad = fey * boxHalfSize.x + fex * boxHalfSize.y;
	if(min > rad || max < -rad)
		return FALSE;

	// Bullet 1:
	// First test overlap in the {x,y,z}-directions.
	// Find min, max of the triangle each direction, and test for overlap in that
	// direction -- this is equivalent to testing a minimal AABB around the triangle
	// against the AABB

	// Test in X-direction
	FindMinMax(rv0.x, rv1.x, rv2.x, min, max);
	if(min > boxHalfSize.x || max < -boxHalfSize.x)
		return FALSE;

	// Test in Y-direction
	FindMinMax(rv0.y, rv1.y, rv2.y, min, max);
	if(min > boxHalfSize.y || max < -boxHalfSize.y)
		return FALSE;

	// Test in Z-direction
	FindMinMax(rv0.z, rv1.z, rv2.z, min, max);
	if(min > boxHalfSize.z || max < -boxHalfSize.z)
		return FALSE;

	// Bullet 2:
	// Test if the box intersects the plane of the triangle.
	// Compute plane equation of triangle: normal * x + d = 0
	normal.SetToCrossProduct(e0, e1);
	d = -DotProduct(normal, rv0);  // plane eq: normal.x + d=0
	if(!PlaneBoxOverlap(normal, d, boxHalfSize))
		return FALSE;

	// Box and triangle overlaps
	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL IntersectsTriangle(const CRuOBB &obb, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2)
{
	// Build inverse basis to neutralize the OBB's world-space position (THIS IS REALLY SLOW!!! Make it so the OBB contains this transform...or have some alternate thing...)
	CRuMatrix4x4 invOBBTransform;
	invOBBTransform.SetToBasis(obb.Axis()[0], obb.Axis()[1], obb.Axis()[2]);

	CRuMatrix4x4 blargh0, blargh1;

	blargh0.SetToTranslation(obb.Center() * -1.0f);
	blargh1.SetToTranslation(obb.Center() * 1.0f);

	invOBBTransform = blargh0 * invOBBTransform * blargh1;

	// Build generic AABB
	CRuVector3 extents(obb.Extents()[0], obb.Extents()[1], obb.Extents()[2]);
	CRuAABB box(obb.Center() - extents, obb.Center() + extents);

	// Transform triangle vertices into local coordinate frame (without translation)
	CRuVector3 vp0, vp1, vp2;

	invOBBTransform.TransformPoint(v0, vp0);
	invOBBTransform.TransformPoint(v1, vp1);
	invOBBTransform.TransformPoint(v2, vp2);

	// Perform AABB-triangle intersection test
	return IntersectsTriangle(box, vp0, vp1, vp2);
}

BOOL FindIntersection(const CRuRay3 &ray, const CRuOBB &obb, CRuVector3 &intPtOut, REAL &intTOut)
{
	// Build inverse basis to neutralize the OBB's world-space position (THIS IS REALLY SLOW!!! Make it so the OBB contains this transform...or have some alternate thing...)
	CRuMatrix4x4 invOBBTransform;
	invOBBTransform.SetToBasis(obb.Axis()[0], obb.Axis()[1], obb.Axis()[2]);

	CRuMatrix4x4 blargh0, blargh1;

	blargh0.SetToTranslation(obb.Center() * -1.0f);
	blargh1.SetToTranslation(obb.Center() * 1.0f);

	invOBBTransform = blargh0 * invOBBTransform * blargh1;

	// Build generic AABB
	CRuVector3 extents(obb.Extents()[0], obb.Extents()[1], obb.Extents()[2]);
	CRuAABB box(obb.Center() - extents, obb.Center() + extents);

	// Build transformed ray
	CRuRay3 localRay(ray);
	invOBBTransform.TransformPoint(localRay.Origin());
	invOBBTransform.TransformVector(localRay.Direction());

	// Perform ray-AABB intersection test
	return FindIntersection(ray, box, intPtOut, intTOut);
}

// ************************************************************************************************************************************************************

BOOL IntersectsTriangleList(const CRuSphere &sphere, const CRuVector3 *vertices, const UINT16 *indices, UINT32 numTriangles)
{
	REAL squaredSphereRadius = sphere.Radius() * sphere.Radius();

	UINT32 numIndices = numTriangles * 3;
	for(UINT32 i = 0; i < numIndices; )
	{
		UINT16 v0 = indices[i++];
		UINT16 v1 = indices[i++];
		UINT16 v2 = indices[i++];

		if(SquaredDistance(sphere.Center(), vertices[v0], vertices[v1], vertices[v2]) < squaredSphereRadius)
		{
			return TRUE;
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

BOOL IntersectsFrustum(const CRuPlane4 *frustumPlanes, const CRuAABB &box)
{
	CRuVector3 min;

	// Check each of the six planes
	for(INT32 k = 0; k < 6; ++k)
	{
		// Find the diagonal points which are most aligned with the plane normal
		if(frustumPlanes[k].A >= 0.0f)
			min.x = box.Maxima().x;
		else
			min.x = box.Minima().x;

		if(frustumPlanes[k].B >= 0.0f)
			min.y = box.Maxima().y;
		else
			min.y = box.Minima().y;

		if(frustumPlanes[k].C >= 0.0f)
			min.z = box.Maxima().z;
		else
			min.z = box.Minima().z;

		if(frustumPlanes[k].GetDistance(min) < 0.0f)
			return FALSE;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL IntersectsFrustum(const CRuPlane4 *frustumPlanes, const CRuSphere &sphere)
{
	// Check each of the six planes
	for(INT32 k = 0; k < 6; ++k)
	{
		if(frustumPlanes[k].GetDistance(sphere.Center()) < -sphere.Radius())
			return FALSE;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL FindIntersection(const CRuRay3 &ray, const CRuAABB &box, CRuVector3 &intPtOut, REAL &intTOut)
{
	BOOL inside = TRUE;
	int quadrant[3];		// 0 = larger than, 1 = smaller than, 2 = inside
	int i;
	int whichPlane;
	REAL maxT[3], candidatePlane[3];
	REAL intersectCoord[3];

	// Find candidate planes - this finds the three plane coordinates (orthogonal to each major axis) nearest the ray.Origin() of the ray
	for(i = 0; i < 3; i++)
	{
		if(ray.Origin()[i] < box.Minima()[i])
		{
			quadrant[i] = 1;
			candidatePlane[i] = box.Minima()[i];
			inside = FALSE;
		}
		else if(ray.Origin()[i] > box.Maxima()[i])
		{
			quadrant[i] = 0;
			candidatePlane[i] = box.Maxima()[i];
			inside = FALSE;
		}
		else
			quadrant[i] = 2;
	}

	// Ray is inside the bounding box
	if(inside)
	{
		intPtOut = ray.Origin();
		intTOut = 0.0f;
		return TRUE;
	}

	// Calculate T distances to candidate planes
	for(i = 0; i < 3; i++)
	{
		if(quadrant[i] != 2 && ray.Direction()[i] != 0.0f)
			maxT[i] = (candidatePlane[i] - ray.Origin()[i]) / ray.Direction()[i];
		else
			maxT[i] = -1.0f;
	}

	// Get largest of the T distances as the final choice of intersection
	whichPlane = 0;
	for(i = 0; i < 3; i++)
	{
		if(maxT[whichPlane] < maxT[i])
			whichPlane = i;
	}

	// Check the final choice is actually inside the box
	if(maxT[whichPlane] < 0.0f)
		return FALSE;

	for(i = 0; i < 3; i++)
	{
		if(whichPlane != i)
		{
			intersectCoord[i] = ray.Origin()[i] + maxT[whichPlane] * ray.Direction()[i];
			if(intersectCoord[i] < box.Minima()[i] || intersectCoord[i] > box.Maxima()[i])
				return FALSE;
		}
		else
		{
			intersectCoord[i] = candidatePlane[i];
		}
	}

	intPtOut.x = intersectCoord[0];
	intPtOut.y = intersectCoord[1];
	intPtOut.z = intersectCoord[2];
	intTOut = maxT[whichPlane];

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL FindIntersection(const CRuSegment2 &segment, const CRuAABB2D &box, CRuVector2 &intPtOut, REAL &intTOut)
{
	BOOL inside = TRUE;
	int quadrant[2];		// 0 = larger than, 1 = smaller than, 2 = inside
	int i;
	int whichPlane;
	REAL maxT[2], candidatePlane[2];
	REAL intersectCoord[2];

	// Find candidate planes - this finds the two plane coordinates (orthogonal to each major axis) nearest the ray.Origin() of the ray
	for(i = 0; i < 2; i++)
	{
		if(segment.Origin()[i] < box.Minima()[i])
		{
			quadrant[i] = 1;
			candidatePlane[i] = box.Minima()[i];
			inside = FALSE;
		}
		else if(segment.Origin()[i] > box.Maxima()[i])
		{
			quadrant[i] = 0;
			candidatePlane[i] = box.Maxima()[i];
			inside = FALSE;
		}
		else
			quadrant[i] = 2;
	}

	// Ray is inside the bounding box
	if(inside)
	{
		intPtOut = segment.Origin();
		intTOut = 0.0f;
		return TRUE;
	}

	// Calculate T distances to candidate planes
	for(i = 0; i < 2; i++)
	{
		if(quadrant[i] != 2 && segment.Direction()[i] != 0.0f)
			maxT[i] = (candidatePlane[i] - segment.Origin()[i]) / segment.Direction()[i];
		else
			maxT[i] = -1.0f;
	}

	// Get largest of the T distances as the final choice of intersection
	whichPlane = 0;
	for(i = 0; i < 2; i++)
	{
		if(maxT[whichPlane] < maxT[i])
			whichPlane = i;
	}

	// Check the final choice is actually inside the box
	if(maxT[whichPlane] < 0.0f)
		return FALSE;

	for(i = 0; i < 2; i++)
	{
		if(whichPlane != i)
		{
			intersectCoord[i] = segment.Origin()[i] + maxT[whichPlane] * segment.Direction()[i];
			if(intersectCoord[i] < box.Minima()[i] || intersectCoord[i] > box.Maxima()[i])
				return FALSE;
		}
		else
		{
			intersectCoord[i] = candidatePlane[i];
		}
	}

	intPtOut.x = intersectCoord[0];
	intPtOut.y = intersectCoord[1];
	intTOut = maxT[whichPlane];

	if(intTOut > 1.0f)
		return FALSE;

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL FindIntersection(const CRuSegment3 &segment, const CRuAABB &box, CRuVector3 &intPtOut, REAL &intTOut)
{
	BOOL inside = TRUE;
	int quadrant[3];		// 0 = larger than, 1 = smaller than, 2 = inside
	int i;
	int whichPlane;
	REAL maxT[3], candidatePlane[3];
	REAL intersectCoord[3];

	// Find candidate planes - this finds the three plane coordinates (orthogonal to each major axis) nearest the ray.Origin() of the ray
	for(i = 0; i < 3; i++)
	{
		if(segment.Origin()[i] < box.Minima()[i])
		{
			quadrant[i] = 1;
			candidatePlane[i] = box.Minima()[i];
			inside = FALSE;
		}
		else if(segment.Origin()[i] > box.Maxima()[i])
		{
			quadrant[i] = 0;
			candidatePlane[i] = box.Maxima()[i];
			inside = FALSE;
		}
		else
			quadrant[i] = 2;
	}

	// Ray is inside the bounding box
	if(inside)
	{
		intPtOut = segment.Origin();
		intTOut = 0.0f;
		return TRUE;
	}

	// Calculate T distances to candidate planes
	for(i = 0; i < 3; i++)
	{
		if(quadrant[i] != 2 && segment.Direction()[i] != 0.0f)
			maxT[i] = (candidatePlane[i] - segment.Origin()[i]) / segment.Direction()[i];
		else
			maxT[i] = -1.0f;
	}

	// Get largest of the T distances as the final choice of intersection
	whichPlane = 0;
	for(i = 0; i < 3; i++)
	{
		if(maxT[whichPlane] < maxT[i])
			whichPlane = i;
	}

	// Check the final choice is actually inside the box
	if(maxT[whichPlane] < 0.0f)
		return FALSE;

	for(i = 0; i < 3; i++)
	{
		if(whichPlane != i)
		{
			intersectCoord[i] = segment.Origin()[i] + maxT[whichPlane] * segment.Direction()[i];
			if(intersectCoord[i] < box.Minima()[i] || intersectCoord[i] > box.Maxima()[i])
				return FALSE;
		}
		else
		{
			intersectCoord[i] = candidatePlane[i];
		}
	}

	intPtOut.x = intersectCoord[0];
	intPtOut.y = intersectCoord[1];
	intPtOut.z = intersectCoord[2];
	intTOut = maxT[whichPlane];

	if(intTOut > 1.0f)
		return FALSE;

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL FindIntersection(const CRuRay3 &ray, const CRuPlane4 &plane, CRuVector3 &intPtOut, REAL &intTOut)
{
	// Segment defined as P(t) = Q + tV
	// To find intersection, we solve the equation N.P(t) + D = 0.
	// Re-arranging yields t = -(N.Q + D) / (N.V).
	// If N.V is zero, then the segment lies parallel to the plane - the segment
	// lies on the plane only if N.Q + D is also zero. Otherwise, solve for t
	// and accept as intersecting if t is in [0, infinity]

	REAL nDv = plane.DotNormal(ray.Direction());
	REAL nDq_d = plane.DotNormal(ray.Origin()) + plane.m_d;

	if(nDv == 0.0f)
	{
		// Ray lies exactly on the plane
		if(nDq_d == 0.0f)
		{
			intPtOut = ray.Origin();
			intTOut = 0.0f;

			return TRUE;
		}
		else
			return FALSE;
	}
	else
	{
		REAL t = -nDq_d / nDv;

		if(t >= 0.0f)
		{
			intPtOut.x = ray.Origin().x + ray.Direction().x * t;
			intPtOut.y = ray.Origin().y + ray.Direction().y * t;
			intPtOut.z = ray.Origin().z + ray.Direction().z * t;
			intTOut = t;

			return TRUE;
		}
		else
			return FALSE;
	}
}

// ************************************************************************************************************************************************************

BOOL FindIntersection(const CRuRay3 &ray, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, CRuVector3 &intPtOut, REAL &intTOut)
{
	CRuVector3 e1(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
	CRuVector3 e2(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
	CRuVector3 p = CrossProduct(ray.Direction(), e2);
	REAL a = DotProduct(e1, p);

	if(a > -ruEPSILON && a < ruEPSILON)
		return FALSE;

	REAL f = 1.0f / a;

	CRuVector3 s(ray.Origin().x - v0.x, ray.Origin().y - v0.y, ray.Origin().z - v0.z);
	REAL u = f * DotProduct(s, p);

	if(u < 0.0f || u > 1.0f)
		return FALSE;

	p.SetToCrossProduct(s, e1);
	REAL v = f * DotProduct(ray.Direction(), p);
	if(v < 0.0f || u + v > 1.0f)
		return FALSE;

	intTOut = f * DotProduct(e2, p);

	if(intTOut < 0.0f)
		return FALSE;

	intPtOut = ray.Origin() + ray.Direction() * intTOut;

	return TRUE;
}

/*
BOOL FindIntersection(const CRuRay3 &ray, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, CRuVector3 &intPtOut, REAL &intTOut)
{
	CRuVector3 ab = v1 - v0;
	CRuVector3 ac = v2 - v0;
	CRuVector3 qp = ray.Direction() * -1.0f;

	CRuVector3 n = CrossProduct(ab, ac);

	REAL d = DotProduct(ray.Direction(), n);
	if(d <= ruEPSILON)
		return FALSE;

	CRuVector3 ap = (ray.Origin() + ray.Direction()) - v0;

	intTOut = DotProduct(ap, n);
	if(intTOut <= 0.0f)
		return FALSE;

	CRuVector3 e = CrossProduct(qp, ap);
	REAL v = DotProduct(ac, e);
	if(v < 0.0f || v > d)
		return FALSE;
	REAL w = -DotProduct(ab, e);
	if(w < 0.0f || v + w > d)
		return FALSE;

	intPtOut = ray.Origin() + ray.Direction() * intTOut;

	return TRUE;
}
*/
// ************************************************************************************************************************************************************

BOOL FindIntersection(const CRuSegment3 &segment, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, CRuVector3 &intPtOut, REAL &intTOut)
{
	CRuVector3 e1(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
	CRuVector3 e2(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
	CRuVector3 p = CrossProduct(segment.Direction(), e2);
	REAL a = DotProduct(e1, p);

	if(a > -ruEPSILON && a < ruEPSILON)
		return FALSE;

	REAL f = 1.0f / a;

	CRuVector3 s(segment.Origin().x - v0.x, segment.Origin().y - v0.y, segment.Origin().z - v0.z);
	REAL u = f * DotProduct(s, p);

	if(u < 0.0f || u > 1.0f)
		return FALSE;

	p.SetToCrossProduct(s, e1);
	REAL v = f * DotProduct(segment.Direction(), p);
	if(v < 0.0f || u + v > 1.0f)
		return FALSE;

	intTOut = f * DotProduct(e2, p);

	if(intTOut < 0.0f || intTOut > 1.0f)
		return FALSE;

	intPtOut = segment.Origin() + segment.Direction() * intTOut;

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL FindIntersection(const CRuPlane4 &plane0, const CRuPlane4 &plane1, const CRuPlane4 &plane2, CRuVector3 &intPtOut)
{
	CRuVector3 N1xN2(plane1.b * plane2.c - plane1.c * plane2.b, plane1.c * plane2.a - plane1.a * plane2.c, plane1.a * plane2.b - plane1.b * plane2.a);
	REAL denom = plane0.a * N1xN2.x + plane0.b * N1xN2.y + plane0.c * N1xN2.z;

	if(denom <= EPSILON && denom >= -EPSILON)
	{
		return FALSE;
	}

	CRuVector3 N0xN1(plane0.b * plane1.c - plane0.c * plane1.b, plane0.c * plane1.a - plane0.a * plane1.c, plane0.a * plane1.b - plane0.b * plane1.a);
	CRuVector3 N2xN0(plane2.b * plane0.c - plane2.c * plane0.b, plane2.c * plane0.a - plane2.a * plane0.c, plane2.a * plane0.b - plane2.b * plane0.a);

	intPtOut = ((N1xN2 * -plane0.d) + (N2xN0 * -plane1.d) + (N0xN1 * -plane2.d)) / denom;

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL FindIntersection(const CRuRay3 &ray, const CRuSphere &sphere, REAL &intTOut)
{
	CRuVector3 m = ray.Origin() - sphere.Center();
	REAL b = DotProduct(m, ray.Direction());
	REAL c = DotProduct(m, m) - sphere.Radius() * sphere.Radius();

	// If ray's origin outside sphere and is pointing away from the sphere
	if(c > 0.0f && b > 0.0f)
		return FALSE;

	REAL discr = b * b - c;

	// A negative discriminant corresponds to ray missing sphere
	if(discr < 0.0f)
		return FALSE;

	// Ray now found to intersect sphere, compute smallest t value of intersection
	intTOut = -b - RuSqrt(discr);

	// If t is negative, ray started inside sphere so clamp t to zero
	if(intTOut < 0.0f)
		intTOut = 0.0f;

	return TRUE;
}

// ************************************************************************************************************************************************************

INT32 FindIntersection(const CRuLine3 &line, const CRuCylinder &cylinder, REAL *afT)
{
	CRuVector3 cylinderCenter = cylinder.Origin() + cylinder.Direction() * 0.5f;
	REAL cylinderHeight = cylinder.Direction().Magnitude();

	// Create a coordinate system for the cylinder.  In this system, the
	// cylinder segment center C is the origin and the cylinder axis direction
	// W is the z-axis.  U and V are the other coordinate axis directions.
	// If P = x*U+y*V+z*W, the cylinder is x^2 + y^2 = r^2, where r is the
	// cylinder radius.  The end caps are |z| = h/2, where h is the cylinder
	// height.
	CRuVector3 kU, kV, kW = cylinder.Direction();
	CRuVector3::GenerateOrthonormalBasis(kU, kV, kW, FALSE);
	REAL fHalfHeight = 0.5f * cylinderHeight;
	REAL fRSqr = cylinder.Radius() * cylinder.Radius();

	// convert incoming line origin to cylinder coordinates
	CRuVector3 kDiff = line.Origin() - cylinderCenter;
	CRuVector3 kP(kU.Dot(kDiff), kV.Dot(kDiff), kW.Dot(kDiff));

	// Get the z-value, in cylinder coordinates, of the incoming line's
	// unit-length direction.
	REAL fDz = kW.Dot(line.Direction());

	if(fabs(fDz) >= 1.0f - ruEPSILON)
	{
		// The line is parallel to the cylinder axis.  Determine if the line intersects the cylinder end disks.
		REAL fRadialSqrDist = fRSqr - kP.x * kP.x - kP.y * kP.y;
		if(fRadialSqrDist < 0.0f)
		{
			// line outside the cylinder, no intersection
			return 0;
		}

		// line intersects the cylinder end disks
		if(fDz > 0.0f)
		{
			afT[0] = -kP.z - fHalfHeight;
			afT[1] = -kP.z + fHalfHeight;
		}
		else
		{
			afT[0] = kP.z - fHalfHeight;
			afT[1] = kP.z + fHalfHeight;
		}

		return 2;
	}

	// convert incoming line unit-length direction to cylinder coordinates
	CRuVector3 kD(kU.Dot(line.Direction()), kV.Dot(line.Direction()), fDz);

	REAL fA0, fA1, fA2, fDiscr, fRoot, fInv, fT;

	if(fabs(kD.z) <= ruEPSILON)
	{
		// The line is perpendicular to the cylinder axis.
		if(fabs(kP.z) > fHalfHeight)
		{
			// line is outside the planes of the cylinder end disks
			return 0;
		}

		// Test intersection of line P+t*D with infinite cylinder
		// x^2+y^2 = r^2.  This reduces to computing the roots of a
		// quadratic equation.  If P = (px,py,pz) and D = (dx,dy,dz),
		// then the quadratic equation is
		//   (dx^2+dy^2)*t^2 + 2*(px*dx+py*dy)*t + (px^2+py^2-r^2) = 0
		fA0 = kP.x * kP.x + kP.y * kP.y - fRSqr;
		fA1 = kP.x * kD.x + kP.y * kD.y;
		fA2 = kD.x * kD.x + kD.y * kD.y;
		fDiscr = fA1 * fA1 - fA0 * fA2;
		if(fDiscr < 0.0f)
		{
			// line does not intersect cylinder
			return 0;
		}
		else if(fDiscr > ruEPSILON)
		{
			// line intersects cylinder in two places
			fRoot = RuSqrt(fDiscr);
			fInv = 1.0f / fA2;
			afT[0] = (-fA1 - fRoot) * fInv;
			afT[1] = (-fA1 + fRoot) * fInv;
			return 2;
		}
		else
		{
			// line is tangent to the cylinder
			afT[0] = -fA1 / fA2;
			return 1;
		}
	}

	// test plane intersections first
	INT32 iQuantity = 0;
	fInv = 1.0f / kD.z;

	REAL fT0 = (-fHalfHeight - kP.z) * fInv;
	REAL fXTmp = kP.x + fT0 * kD.x;
	REAL fYTmp = kP.y + fT0 * kD.y;
	if(fXTmp * fXTmp + fYTmp * fYTmp <= fRSqr)
	{
			// planar intersection inside the top cylinder end disk
		afT[iQuantity++] = fT0;
	}

	REAL fT1 = (+fHalfHeight - kP.z) * fInv;
	fXTmp = kP.x + fT1 * kD.x;
	fYTmp = kP.y + fT1 * kD.y;
	if(fXTmp * fXTmp + fYTmp * fYTmp <= fRSqr)
	{
		// planar intersection inside the bottom cylinder end disk
		afT[iQuantity++] = fT1;
	}

	// line intersects both top and bottom cylinder end disks
	if(iQuantity == 2)
	{
		// Swap result so first result is always closer
		if(afT[0] > afT[1])
		{
			REAL fSave = afT[0];
			afT[0] = afT[1];
			afT[1] = fSave;
		}

		return 2;
	}

	// If iQuantity == 1, then the line must intersect cylinder wall in a
	// single point somewhere between the end disks.  This case is detected
	// in the following code that tests for intersection between line and
	// cylinder wall.
	fA0 = kP.x * kP.x + kP.y * kP.y - fRSqr;
	fA1 = kP.x * kD.x + kP.y * kD.y;
	fA2 = kD.x * kD.x + kD.y * kD.y;
	fDiscr = fA1 * fA1 - fA0 * fA2;
	if(fDiscr < 0.0f)
	{
		// line does not intersect cylinder wall
		assert(iQuantity == 0);
		return 0;
	}
	else if(fDiscr > ruEPSILON)
	{
		fRoot = RuSqrt(fDiscr);
		fInv = 1.0f / fA2;
		fT = (-fA1 - fRoot) * fInv;
		if(fT0 <= fT1)
		{
			if(fT0 <= fT && fT <= fT1)
			{
				afT[iQuantity++] = fT;
			}
		}
		else
		{
			if(fT1 <= fT && fT <= fT0)
			{
				afT[iQuantity++] = fT;
			}
		}

		// Line intersects one of the cylinder end disks and once on the cylinder wall.
		if(iQuantity == 2)
		{
			if(afT[0] > afT[1])
			{
				REAL fSave = afT[0];
				afT[0] = afT[1];
				afT[1] = fSave;
			}
			return 2;
		}

		fT = (-fA1 + fRoot) * fInv;
		if(fT0 <= fT1)
		{
			if(fT0 <= fT && fT <= fT1)
			{
				afT[iQuantity++] = fT;
			}
		}
		else
		{
			if(fT1 <= fT && fT <= fT0)
			{
				afT[iQuantity++] = fT;
			}
		}
	}
	else
	{
		fT = -fA1 / fA2;
		if(fT0 <= fT1)
		{
			if(fT0 <= fT && fT <= fT1)
			{
				afT[iQuantity++] = fT;
			}
		}
		else
		{
			if(fT1 <= fT && fT <= fT0)
			{
				afT[iQuantity++] = fT;
			}
		}
	}

	if(iQuantity == 2)
	{
		if(afT[0] > afT[1])
		{
			REAL fSave = afT[0];
			afT[0] = afT[1];
			afT[1] = fSave;
		}
	}

	return iQuantity;
}

// ************************************************************************************************************************************************************

BOOL FindIntersection(const CRuSegment3 &segment, const CRuCylinder &cylinder, REAL &intTOut)
{
	CRuVector3 sa = segment.Origin();
	CRuVector3 sb = sa + segment.Direction();
	CRuVector3 p = cylinder.Origin();
	CRuVector3 q = p + cylinder.Direction();
	REAL r = cylinder.Radius();

	CRuVector3 d = q - p;
	CRuVector3 m = sa - p;
	CRuVector3 n = sb - sa;
	REAL md = DotProduct(m, d);
	REAL nd = DotProduct(n, d);
	REAL dd = DotProduct(d, d);

	// Test if segment fully outside either endcap of cylinder
	if(md < 0.0f && md + nd < 0.0f)		// Segment outside 'p' side of cylinder
		return FALSE;

	if(md > dd && md + nd > dd)			// Segment outside 'q' side of cylinder
		return FALSE;

	REAL nn = DotProduct(n, n);
	REAL mn = DotProduct(m, n);
	REAL a = dd * nn - nd * nd;
	REAL k = DotProduct(m, m) - r * r;
	REAL c = dd * k - md * md;

	if(fabs(a) < ruEPSILON)
	{
		// Segment runs parallel to cylinder axis
		if(c > 0.0f)					// 'a' and thus the segment lie outside cylinder
			return FALSE;

		// Now known that segment intersects cylinder; figure out how it intersects
		if(md < 0.0f)
			intTOut = -mn / nn;			// Intersect segment against 'p' endcap
		else if(md > dd)
			intTOut = (nd - mn) / nn;	// Intersect segment against 'q' endcap
		else
			intTOut = 0.0f;				// 'a' lies inside cylinder

		return TRUE;
	}

	REAL b = dd * mn - nd * md;
	REAL discr = b * b - a * c;

	if(discr < 0.0f)					// No real roots; no intersection
		return FALSE;

	intTOut = (-b - RuSqrt(discr)) / a;

	if(intTOut < 0.0f || intTOut > 1.0f)					// Intersection is behind segment
		return FALSE;

	if(md + intTOut * nd < 0.0f)
	{
		// Intersection outside cylinder on 'p' side
		if(nd <= 0.0f)					// Segment pointing away from endcap
			return FALSE;

		intTOut = -md / nd;

		// Keep intersection if Dot(S(t) - p, S(t) - p) <= r^2
		return k + 2 * intTOut * (mn + intTOut * nn) <= 0.0f;
	}
	else if(md + intTOut * nd > dd)
	{
		// Intersection outside cylinder on 'q' side
		if(nd >= 0.0f)					// Segment pointing away from endcap
			return FALSE;

		intTOut = (dd - md) / nd;

		// Keep intersection if Dot(S(t) - q, S(t) - q) <= r^2
		return k + dd - 2 * md + intTOut * (2 * (mn - nd) + intTOut * nn) <= 0.0f;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

// NOTE!!! THIS IS SEVERELY BUGGED!! This algorithm is designed for a SEGMENT, not a RAY!!!
BOOL FindIntersection(const CRuRay3 &ray, const CRuCylinder &cylinder, REAL &intTOut)
{
	REAL extendFactor = 10000.0f;
	CRuVector3 sa = ray.Origin();
	CRuVector3 sb = sa + ray.Direction() * extendFactor;	// For the moment, we extend the ray to a segment of a very large length to get around the bug
	CRuVector3 p = cylinder.Origin();
	CRuVector3 q = p + cylinder.Direction();
	REAL r = cylinder.Radius();

	CRuVector3 d = q - p;
	CRuVector3 m = sa - p;
	CRuVector3 n = sb - sa;
	REAL md = DotProduct(m, d);
	REAL nd = DotProduct(n, d);
	REAL dd = DotProduct(d, d);

	// Test if segment fully outside either endcap of cylinder
	if(md < 0.0f && md + nd < 0.0f)		// Segment outside 'p' side of cylinder
		return FALSE;

	if(md > dd && md + nd > dd)			// Segment outside 'q' side of cylinder
		return FALSE;

	REAL nn = DotProduct(n, n);
	REAL mn = DotProduct(m, n);
	REAL a = dd * nn - nd * nd;
	REAL k = DotProduct(m, m) - r * r;
	REAL c = dd * k - md * md;

	if(fabs(a) < ruEPSILON)
	{
		// Segment runs parallel to cylinder axis
		if(c > 0.0f)					// 'a' and thus the segment lie outside cylinder
			return FALSE;

		// Now known that segment intersects cylinder; figure out how it intersects
		if(md < 0.0f)
			intTOut = -mn / nn;			// Intersect segment against 'p' endcap
		else if(md > dd)
			intTOut = (nd - mn) / nn;	// Intersect segment against 'q' endcap
		else
			intTOut = 0.0f;				// 'a' lies inside cylinder

		intTOut = intTOut * extendFactor;

		return TRUE;
	}

	REAL b = dd * mn - nd * md;
	REAL discr = b * b - a * c;

	if(discr < 0.0f)					// No real roots; no intersection
		return FALSE;

	intTOut = (-b - RuSqrt(discr)) / a;

	if(intTOut < 0.0f || intTOut > 1.0f)					// Intersection is behind segment
		return FALSE;

	if(md + intTOut * nd < 0.0f)
	{
		// Intersection outside cylinder on 'p' side
		if(nd <= 0.0f)					// Segment pointing away from endcap
			return FALSE;

		intTOut = -md / nd;

		// Keep intersection if Dot(S(t) - p, S(t) - p) <= r^2
		BOOL keepIntersection = k + 2 * intTOut * (mn + intTOut * nn) <= 0.0f;
		intTOut = intTOut * extendFactor;
		return keepIntersection;
	}
	else if(md + intTOut * nd > dd)
	{
		// Intersection outside cylinder on 'q' side
		if(nd >= 0.0f)					// Segment pointing away from endcap
			return FALSE;

		intTOut = (dd - md) / nd;

		// Keep intersection if Dot(S(t) - q, S(t) - q) <= r^2
		BOOL keepIntersection = k + dd - 2 * md + intTOut * (2 * (mn - nd) + intTOut * nn) <= 0.0f;
		intTOut = intTOut * extendFactor;
		return keepIntersection;
	}

	intTOut = intTOut * extendFactor;
	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL FindIntersection(const CRuSphere &sphere, const CRuVector3 &velocity, const CRuPlane4 &plane, REAL &tOut, CRuVector3 &pOut)
{
	// Compute distance of sphere to plane
	REAL dist = plane.GetDistance(sphere.Center());
	REAL absDist = fabs(dist);

	if(absDist + ruEPSILON < sphere.Radius())
	{
/*
		// Calculate reciprocal of speed
		REAL rcpSpeed = 1.0f / velocity.Magnitude();

		REAL rt;
		CRuVector3 rp;

		CRuRay3 sphereRay(sphere.Center(), velocity);
		if(FindIntersection(sphereRay, plane, rp, rt))
		{
			tOut = rt - sphere.Radius() * rcpSpeed;
			pOut = rp;
			return TRUE;
		}

		sphereRay.Direction() = sphereRay.Direction() * -1.0f;
		if(FindIntersection(sphereRay, plane, rp, rt))
		{
			tOut = -rt - sphere.Radius() * rcpSpeed;
			pOut = rp;
			return TRUE;
		}

		// Sphere is overlapping the plane and moving parallel to the plane
		tOut = 0.0f;
		pOut = sphere.Center();
*/

		// project sphere center onto plane
//		CRuVector3 centerProj = plane.GetClosestPointOnPlane(sphere.Center());

		// project velocity onto plane


		// The sphere is already overlapping the plane. Set time of intersection to zero and q to sphere center.
		tOut = 0.0f;
		pOut = sphere.Center();

		return TRUE;
	}
	else
	{
		REAL denom = plane.DotNormal(velocity);
		if(denom * dist >= 0.0f)
		{
			// No intersection as sphere is moving parallel to or away from plane
			return FALSE;
		}
		else
		{
			// Sphere is moving towards the plane
			// Use +r in computations is sphere in front of plane, else -r
			REAL r = dist > 0.0f ? sphere.Radius() : -sphere.Radius();

			tOut = (r - dist) / denom;
			pOut = sphere.Center() + velocity * tOut - plane.GetNormal() * r;
			return TRUE;
		}
	}
}

// ************************************************************************************************************************************************************

BOOL InsideTriangle(const CRuVector3 &tv0, const CRuVector3 &tv1, const CRuVector3 &tv2, const CRuVector3 &tn, const CRuVector3 &point)
{
	CRuVector3 a = tv2 - point;
	CRuVector3 b = tv1 - point;
	CRuVector3 c = tv0 - point;

	CRuVector3 u = CrossProduct(b, c);
	CRuVector3 v = CrossProduct(c, a);

	if(DotProduct(u, v) < 0.0f)
		return FALSE;

	CRuVector3 w = CrossProduct(a, b);

	if(DotProduct(u, w) < 0.0f)
		return FALSE;

	return TRUE;
}

// ************************************************************************************************************************************************************

RuIntersectResult Intersects(const CRuPlane4 &plane, const CRuSegment3 &segment, CRuVector3 &intPtOut)
{
	// Segment defined as P(t) = Q + tV
	// To find intersection, we solve the equation N.P(t) + D = 0.
	// Re-arranging yields t = -(N.Q + D) / (N.V).
	// If N.V is zero, then the segment lies parallel to the plane - the segment
	// lies on the plane only if N.Q + D is also zero. Otherwise, solve for t
	// and accept as intersecting if t is in [0, 1].

	REAL nDv = plane.DotNormal(segment.Direction());
	REAL nDq_d = plane.DotNormal(segment.Origin()) + plane.m_d;

	if(nDv == 0.0f)
	{
		if(nDq_d == 0.0f)
			return ruINTRESULT_Coplanar;
		else
			return ruINTRESULT_NoIntersect;
	}
	else
	{
		REAL t = -nDq_d / nDv;

		if(t >= 0.0f && t <= 1.0f)
		{
			intPtOut.x = segment.Origin().x + segment.Direction().x * t;
			intPtOut.y = segment.Origin().y + segment.Direction().y * t;
			intPtOut.z = segment.Origin().z + segment.Direction().z * t;

			return ruINTRESULT_Intersect;
		}
		else
			return ruINTRESULT_NoIntersect;
	}
}

// ************************************************************************************************************************************************************

RuIntersectResult Intersects(const CRuPlane4 &plane, const CRuVector3 &origin, const CRuVector3 &direction, CRuVector3 &intPtOut)
{
	// Segment defined as P(t) = Q + tV
	// To find intersection, we solve the equation N.P(t) + D = 0.
	// Re-arranging yields t = -(N.Q + D) / (N.V).
	// If N.V is zero, then the segment lies parallel to the plane - the segment
	// lies on the plane only if N.Q + D is also zero. Otherwise, solve for t
	// and accept as intersecting if t is in [0, infinity]

	REAL nDv = plane.DotNormal(direction);
	REAL nDq_d = plane.DotNormal(origin) + plane.m_d;

	if(nDv == 0.0f)
	{
		if(nDq_d == 0.0f)
			return ruINTRESULT_Coplanar;
		else
			return ruINTRESULT_NoIntersect;
	}
	else
	{
		REAL t = -nDq_d / nDv;

		if(t >= 0.0f)
		{
			intPtOut.x = origin.x + direction.x * t;
			intPtOut.y = origin.y + direction.y * t;
			intPtOut.z = origin.z + direction.z * t;

			return ruINTRESULT_Intersect;
		}
		else
			return ruINTRESULT_NoIntersect;
	}
}

// ************************************************************************************************************************************************************

BOOL RayIntersectsTriangle(const CRuVector3 &origin, const CRuVector3 &direction, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2)
{
	CRuVector3 e1 = v1 - v0;
	CRuVector3 e2 = v2 - v0;
	CRuVector3 p = CrossProduct(direction, e2);
	REAL a = DotProduct(e1, p);

	if(a > -EPSILON && a < EPSILON)
		return FALSE;

	REAL f = 1.0f / a;

	CRuVector3 s = origin - v0;
	REAL u = f * DotProduct(s, p);

	if(u < 0.0f || u > 1.0f)
		return FALSE;

	CRuVector3 q = CrossProduct(s, e1);
	REAL v = f * DotProduct(direction, q);
	if(v < 0.0f || u + v > 1.0f)
		return FALSE;

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RayIntersectsTriangle(const CRuVector3 &origin, const CRuVector3 &direction, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, REAL &uOut, REAL &vOut, REAL &tOut)
{
	CRuVector3 e1(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
	CRuVector3 e2(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
	CRuVector3 p = CrossProduct(direction, e2);
	REAL a = DotProduct(e1, p);

	if(a > -EPSILON && a < EPSILON)
		return FALSE;

	REAL f = 1.0f / a;

	CRuVector3 s(origin.x - v0.x, origin.y - v0.y, origin.z - v0.z);
	REAL u = f * DotProduct(s, p);

	if(u < 0.0f || u > 1.0f)
		return FALSE;

	p.SetToCrossProduct(s, e1);
	REAL v = f * DotProduct(direction, p);
	if(v < 0.0f || u + v > 1.0f)
		return FALSE;

	uOut = u;
	vOut = v;
	tOut = f * DotProduct(e2, p);

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
