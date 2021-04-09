// *** MatrixLibrary.cpp ***

#include "RuVectorMath.h"

#pragma unmanaged

float GetDistance(const CRuAABB &aabb, const CRuVector3 &pt)
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

float GetAngle(const CRuVector2 &src, const CRuVector2 &dst)
{
	float dist = src.Distance(dst);
	float width  = (dst.x - src.x);
	float rads = (float) acos(width / dist); // angle in radians

	if(src.y > dst.y)
		rads = 2.0f * PI - rads;

	return rads;
}

void ExtractPlanesFromTransform(CRuPlane4 *planes, const CRuMatrix4x4 &transform, BOOL normalize)
{
	// Left clipping plane
	planes[0].A = transform._14 + transform._11;
	planes[0].B = transform._24 + transform._21;
	planes[0].C = transform._34 + transform._31;
	planes[0].D = transform._44 + transform._41;

	// Right clipping plane
	planes[1].A = transform._14 - transform._11;
	planes[1].B = transform._24 - transform._21;
	planes[1].C = transform._34 - transform._31;
	planes[1].D = transform._44 - transform._41;

	// Top clipping plane
	planes[2].A = transform._14 - transform._12;
	planes[2].B = transform._24 - transform._22;
	planes[2].C = transform._34 - transform._32;
	planes[2].D = transform._44 - transform._42;

	// Bottom clipping plane
	planes[3].A = transform._14 + transform._12;
	planes[3].B = transform._24 + transform._22;
	planes[3].C = transform._34 + transform._32;
	planes[3].D = transform._44 + transform._42;

	// Near clipping plane
	planes[4].A = transform._13;
	planes[4].B = transform._23;
	planes[4].C = transform._33;
	planes[4].D = transform._43;

	// Far clipping plane
	planes[5].A = transform._14 - transform._13;
	planes[5].B = transform._24 - transform._23;
	planes[5].C = transform._34 - transform._33;
	planes[5].D = transform._44 - transform._43;

	if(normalize)
	{
		planes[0].Normalize();
		planes[1].Normalize();
		planes[2].Normalize();
		planes[3].Normalize();
		planes[4].Normalize();
		planes[5].Normalize();
	}
}

UINT32 ClipPolygon(CRuVector3 *verticesIn, UINT32 numVerticesIn, CRuVector3 *verticesOut, const CRuPlane4 &plane)
{
	// NOTE: Standard Sutherland-Hodgeman line clipper implementation, except in 3D, clipping against planes.
	UINT32 i, numVerticesOut = 0;

	CRuVector3 s = verticesIn[numVerticesIn - 1], p, intPt;
	for(i = 0; i < numVerticesIn; i++)
	{
		p = verticesIn[i];

		float sDist = plane.GetDistance(s), pDist = plane.GetDistance(p);

		if(pDist >= 0.0f)
		{
			if(sDist >= 0.0f)
			{
				verticesOut[numVerticesOut++] = p;
			}
			else
			{
				Intersects(plane, CRuSegment3(s, p), intPt);
				verticesOut[numVerticesOut++] = intPt;
				verticesOut[numVerticesOut++] = p;
			}
		}
		else
		{
			if(sDist >= 0.0f)
			{
				Intersects(plane, CRuSegment3(s, p), intPt);
				verticesOut[numVerticesOut++] = intPt;
			}
			else
			{
				// No action for case 3
			}
		}

		s = p;
	}

	return numVerticesOut;
}

// verticesOut must have enough space for 6 vertices
UINT32 ClipTriangle(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, const CRuPlane4 &plane, CRuVector3 *verticesOut)
{
	CRuVector3 vertices[3];
	vertices[0] = v0;
	vertices[1] = v1;
	vertices[2] = v2;

	UINT32 numVertices = ClipPolygon(vertices, 3, verticesOut, plane);

	if(numVertices == 0)
		return 0;

	// If numVertices is 3, no further processing needed
	if(numVertices == 3)
		return 1;
	
	// Rearrange output array if numVertices is 4
	if(numVertices == 4)
	{
		verticesOut[5] = verticesOut[0];
		verticesOut[4] = verticesOut[3];
		verticesOut[3] = verticesOut[2];

		return 2;
	}
/*
	// Critical error with the clipper...
	char moo[200];
	sprintf(moo, "... %u", numVertices);
	MessageBox(NULL, moo, "", MB_OK);
*/
	return 0;
}

BOOL TrianglesAreIdentical(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2)
{
	if(u0.IsEquivalent(v0))
	{
		if(u1.IsEquivalent(v1))
		{
			if(u2.IsEquivalent(v2))
				return TRUE;
		}
		if(u1.IsEquivalent(v2))
		{
			if(u2.IsEquivalent(v1))
				return TRUE;
		}
	}
	if(u0.IsEquivalent(v1))
	{
		if(u1.IsEquivalent(v0))
		{
			if(u2.IsEquivalent(v2))
				return TRUE;
		}
		if(u1.IsEquivalent(v2))
		{
			if(u2.IsEquivalent(v0))
				return TRUE;
		}
	}
	if(u0.IsEquivalent(v2))
	{
		if(u1.IsEquivalent(v0))
		{
			if(u2.IsEquivalent(v1))
				return TRUE;
		}
		if(u1.IsEquivalent(v1))
		{
			if(u2.IsEquivalent(v0))
				return TRUE;
		}
	}

	return FALSE;
}

BOOL TrianglesAreAdjacent(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2)
{
	if(u0.IsEquivalent(v0))
	{
		if(u1.IsEquivalent(v1) || u1.IsEquivalent(v2) || u2.IsEquivalent(v1) || u2.IsEquivalent(v2))
			return TRUE;
	}
	else if(u0.IsEquivalent(v1))
	{
		if(u1.IsEquivalent(v0) || u1.IsEquivalent(v2) || u2.IsEquivalent(v0) || u2.IsEquivalent(v2))
			return TRUE;
	}
	else if(u0.IsEquivalent(v2))
	{
		if(u1.IsEquivalent(v0) || u1.IsEquivalent(v1) || u2.IsEquivalent(v0) || u2.IsEquivalent(v1))
			return TRUE;
	}
	else if(u1.IsEquivalent(v0))
	{
		if(u0.IsEquivalent(v1) || u0.IsEquivalent(v2) || u2.IsEquivalent(v1) || u2.IsEquivalent(v2))
			return TRUE;
	}
	else if(u1.IsEquivalent(v1))
	{
		if(u0.IsEquivalent(v0) || u0.IsEquivalent(v2) || u2.IsEquivalent(v0) || u2.IsEquivalent(v2))
			return TRUE;
	}
	else if(u1.IsEquivalent(v2))
	{
		if(u0.IsEquivalent(v0) || u0.IsEquivalent(v1) || u2.IsEquivalent(v0) || u2.IsEquivalent(v1))
			return TRUE;
	}
	else if(u2.IsEquivalent(v0))
	{
		if(u0.IsEquivalent(v1) || u0.IsEquivalent(v2) || u1.IsEquivalent(v1) || u1.IsEquivalent(v2))
			return TRUE;
	}
	else if(u2.IsEquivalent(v1))
	{
		if(u0.IsEquivalent(v0) || u0.IsEquivalent(v2) || u1.IsEquivalent(v0) || u1.IsEquivalent(v2))
			return TRUE;
	}
	else if(u2.IsEquivalent(v2))
	{
		if(u0.IsEquivalent(v0) || u0.IsEquivalent(v1) || u1.IsEquivalent(v0) || u1.IsEquivalent(v1))
			return TRUE;
	}

	return FALSE;
}

BOOL TriangleSharesEdge(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, CRuVector3 &nonadjacentVertex)
{
	if(u0.IsEquivalent(v0))
	{
		if(u1.IsEquivalent(v1))
		{
			nonadjacentVertex = u2;
			return TRUE;
		}
		else if(u2.IsEquivalent(v1))
		{
			nonadjacentVertex = u1;
			return TRUE;
		}
	}
	else if(u0.IsEquivalent(v1))
	{
		if(u1.IsEquivalent(v0))
		{
			nonadjacentVertex = u2;
			return TRUE;
		}
		else if(u2.IsEquivalent(v0))
		{
			nonadjacentVertex = u1;
			return TRUE;
		}
	}
	else if(u1.IsEquivalent(v0))
	{
		if(u0.IsEquivalent(v1))
		{
			nonadjacentVertex = u2;
			return TRUE;
		}
		else if(u2.IsEquivalent(v1))
		{
			nonadjacentVertex = u0;
			return TRUE;
		}
	}
	else if(u1.IsEquivalent(v1))
	{
		if(u0.IsEquivalent(v0))
		{
			nonadjacentVertex = u2;
			return TRUE;
		}
		else if(u2.IsEquivalent(v0))
		{
			nonadjacentVertex = u0;
			return TRUE;
		}
	}
	else if(u2.IsEquivalent(v0))
	{
		if(u0.IsEquivalent(v1))
		{
			nonadjacentVertex = u1;
			return TRUE;
		}
		else if(u1.IsEquivalent(v1))
		{
			nonadjacentVertex = u0;
			return TRUE;
		}
	}
	else if(u2.IsEquivalent(v1))
	{
		if(u0.IsEquivalent(v0))
		{
			nonadjacentVertex = u1;
			return TRUE;
		}
		else if(u1.IsEquivalent(v0))
		{
			nonadjacentVertex = u0;
			return TRUE;
		}
	}

	return FALSE;
}

float GetTriangleYDistance(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2)
{
	float uMin = min(min(u0.y, u1.y), u2.y);
	float uMax = max(max(u0.y, u1.y), u2.y);
	float vMin = min(min(v0.y, v1.y), v2.y);
	float vMax = max(max(v0.y, v1.y), v2.y);

	// If triangles overlap, distance is zero
	if((uMin <= vMin && uMax >= vMin) || (uMin <= vMax && uMax >= vMax))
		return 0.0f;

	if(vMin >= uMax)
		return vMin - uMax;
	else
		return uMin - vMax;
}

float FindClosestPointOnSegment(const CRuVector3 &u0, const CRuVector3 &v0, const CRuVector3 &v1, float &pfParam)
{
	CRuVector3 dir = v1 - v0;
	CRuVector3 kDiff = u0 - v0;
	float fT = kDiff.Dot(dir);

	if(fT <= 0.0f)
	{
		fT = 0.0f;
	}
	else
	{
		float fSqrLen = dir.SquaredMagnitude();
		if(fT >= fSqrLen)
		{
			fT = 1.0f;
			kDiff -= dir;
		}
		else
		{
			fT /= fSqrLen;
			kDiff -= (dir * fT);
		}
	}

	pfParam = fT;

	return kDiff.SquaredMagnitude();
}

static BOOL Find(const CRuVector2 &rkP0, const CRuVector2 &rkD0, const CRuVector2 &rkP1, const CRuVector2 &rkD1, CRuVector2 &rkDiff, float &rfD0SqrLen, int &riQuantity, float afT[2])
{
	// Intersection is a solution to P0+s*D0 = P1+t*D1.  Rewrite as
	// s*D0 - t*D1 = P1 - P0, a 2x2 system of equations.  If D0 = (x0,y0)
	// and D1 = (x1,y1) and P1 - P0 = (c0,c1), then the system is
	// x0*s - x1*t = c0 and y0*s - y1*t = c1.  The error tests are relative
	// to the size of the direction vectors, |Cross(D0,D1)| >= e*|D0|*|D1|
	// rather than absolute tests |Cross(D0,D1)| >= e.  The quantities
	// P1-P0, |D0|^2, and |D1|^2 are returned for use by calling functions.

	float fDet = rkD1.Cross(rkD0);
	rkDiff = rkP1 - rkP0;
	rfD0SqrLen = rkD0.SquaredMagnitude();

	if(fDet * fDet > EPSILON * rfD0SqrLen * rkD1.SquaredMagnitude())
	{
		// Lines intersect in a single point.  Return both s and t values for
		// use by calling functions.
		float fInvDet = 1.0f / fDet;
		riQuantity = 1;
		afT[0] = rkD1.Cross(rkDiff) * fInvDet;
		afT[1] = rkD0.Cross(rkDiff) * fInvDet;
	}
	else
	{
		// lines are parallel
		fDet = rkD0.Cross(rkDiff);
		float fRHS = EPSILON * rfD0SqrLen * rkDiff.SquaredMagnitude();
		if(fDet * fDet > fRHS)
		{
			// lines are disjoint
			riQuantity = 0;
		}
		else
		{
			// lines are the same
			riQuantity = 2;
		}
	}

	return riQuantity != 0;
}

BOOL FindSegmentIntersection(const CRuVector2 &u0, const CRuVector2 &u1, const CRuVector2 &v0, const CRuVector2 &v1, int &riQuantity, float afT[2])
{
	CRuVector2 kDiff, uDir = u1 - u0, vDir = v1 - v0;
	float fD0SqrLen;
	BOOL bIntersects = Find(u0, uDir, v0, vDir, kDiff, fD0SqrLen, riQuantity, afT);

	if(bIntersects)
	{
		if(riQuantity == 1)
		{
			if(afT[0] < 0.0f || afT[0] >  1.0f || afT[1] < 0.0f || afT[1] > 1.0f)
			{
				// lines intersect, but segments do not
				riQuantity = 0;
			}
		}
		else
		{
			// segments are on the same line
			float fDotRS = uDir.Dot(vDir);
			float fDot0, fDot1;
			if(fDotRS > 0.0f)
			{
				fDot0 = kDiff.Dot(uDir);
				fDot1 = fDot0 + fDotRS;
			}
			else
			{
				fDot1 = kDiff.Dot(uDir);
				fDot0 = fDot1 + fDotRS;
			}

			// compute intersection of [t0,t1] and [0,1]
			if(fDot1 < 0.0f || fDot0 > fD0SqrLen)
			{
				riQuantity = 0;
			}
			else if(fDot1 > 0.0f)
			{
				if(fDot0 < fD0SqrLen)
				{
					float fInvLen = 1.0f / fD0SqrLen;
					riQuantity = 2;
					afT[0] = (fDot0 < 0.0f ? 0.0f : fDot0 * fInvLen);
					afT[1] = (fDot1 > fD0SqrLen ? 1.0f : fDot1 * fInvLen);
				}
				else  // fT0 == 1
				{
					riQuantity = 1;
					afT[0] = 1.0f;
				}
			}
			else  // fT1 == 0
			{
				riQuantity = 1;
				afT[0] = 0.0f;
			}
		}
	}

	return riQuantity != 0;
}

static void ClipConvexPolygonAgainstLine(const CRuVector2 &rkN, float fC, int &riQuantity, CRuVector2 akV[6])
{
	// The input vertices are assumed to be in counterclockwise order.  The
	// ordering is an invariant of this function.

	// test on which side of line are the vertices
	int iPositive = 0, iNegative = 0, iPIndex = -1;
	float afTest[6];
	int i;
	for(i = 0; i < riQuantity; i++)
	{
		afTest[i] = rkN.Dot(akV[i]) - fC;
		if(afTest[i] > 0.0f)
		{
			iPositive++;
			if(iPIndex < 0)
				iPIndex = i;
		}
		else if(afTest[i] < 0.0f)
		{
			iNegative++;
		}
	}

	if(iPositive > 0)
	{
		if(iNegative > 0)
		{
			// line transversely intersects polygon
			CRuVector2 akCV[6];
			int iCQuantity = 0, iCur, iPrv;
			float fT;

			if(iPIndex > 0)
			{
				// first clip vertex on line
				iCur = iPIndex;
				iPrv = iCur - 1;
				fT = afTest[iCur] / (afTest[iCur] - afTest[iPrv]);
				akCV[iCQuantity++] = akV[iCur] + (akV[iPrv] - akV[iCur]) * fT;

				// vertices on positive side of line
				while(iCur < riQuantity && afTest[iCur] > 0.0f)
					akCV[iCQuantity++] = akV[iCur++];

				// last clip vertex on line
				if(iCur < riQuantity)
				{
					iPrv = iCur - 1;
				}
				else
				{
					iCur = 0;
					iPrv = riQuantity - 1;
				}
				fT = afTest[iCur] / (afTest[iCur] - afTest[iPrv]);
				akCV[iCQuantity++] = akV[iCur] + (akV[iPrv] - akV[iCur]) * fT;
			}
			else  // iPIndex is 0
			{
				// vertices on positive side of line
				iCur = 0;
				while(iCur < riQuantity && afTest[iCur] > 0.0f)
					akCV[iCQuantity++] = akV[iCur++];

				// last clip vertex on line
				iPrv = iCur - 1;
				fT = afTest[iCur] / (afTest[iCur] - afTest[iPrv]);
				akCV[iCQuantity++] = akV[iCur] + (akV[iPrv] - akV[iCur]) * fT;

				// skip vertices on negative side
				while(iCur < riQuantity && afTest[iCur] <= 0.0f)
					iCur++;

				// first clip vertex on line
				if(iCur < riQuantity)
				{
					iPrv = iCur - 1;
					fT = afTest[iCur] / (afTest[iCur] - afTest[iPrv]);
					akCV[iCQuantity++] = akV[iCur] + (akV[iPrv] - akV[iCur]) * fT;

					// vertices on positive side of line
					while(iCur < riQuantity && afTest[iCur] > 0.0f)
						akCV[iCQuantity++] = akV[iCur++];
				}
				else
				{
					// iCur = 0
					iPrv = riQuantity - 1;
					fT = afTest[0] / (afTest[0] - afTest[iPrv]);
					akCV[iCQuantity++] = akV[0] + (akV[iPrv] - akV[0]) * fT;
				}
			}

			riQuantity = iCQuantity;
			memcpy(akV, akCV, iCQuantity * sizeof(CRuVector2));
		}
		// else polygon fully on positive side of line, nothing to do
	}
	else
	{
		// polygon does not intersect positive side of line, clip all
		riQuantity = 0;
	}
}

BOOL FindTriangleIntersection(const CRuVector2 &u0, const CRuVector2 &u1, const CRuVector2 &u2, const CRuVector2 &v0, const CRuVector2 &v1, const CRuVector2 &v2, int &riQuantity, CRuVector2 akVertex[6])
{
	CRuVector2 akV0[3] = { u0, u1, u2 };

	// The potential intersection is initialized to	the	V1-triangle. The
	// set of vertices is refined based	on clipping	against	each edge of the
	// V0-triangle.
	riQuantity = 3;
	akVertex[0]	= v0;
	akVertex[1]	= v1;
	akVertex[2]	= v2;

	for(int iI1 = 2, iI2 =	0; iI2 < 3;	iI1	= iI2, iI2++)
	{
		// clip	against	edge <V0[iI1], V0[iI2]>
		CRuVector2 kN(akV0[iI1].y - akV0[iI2].y, akV0[iI2].x - akV0[iI1].x);
		float fC = kN.Dot(akV0[iI1]);

		ClipConvexPolygonAgainstLine(kN, fC, riQuantity, akVertex);
		if(riQuantity == 0)
		{
			// triangle	completely clipped,	no intersection	occurs
			return FALSE;
		}
	}

	return TRUE;
}

































/* Triangle/triangle intersection test routine,
 * by Tomas Moller, 1997.
 * See article "A Fast Triangle-Triangle Intersection Test",
 * Journal of Graphics Tools, 2(2), 1997
 * updated: 2001-06-20 (added line of intersection)
 *
 * int tri_tri_intersect(float V0[3],float V1[3],float V2[3],
 *                       float U0[3],float U1[3],float U2[3])
 *
 * parameters: vertices of triangle 1: V0,V1,V2
 *             vertices of triangle 2: U0,U1,U2
 * result    : returns 1 if the triangles intersect, otherwise 0
 *
 * Here is a version withouts divisions (a little faster)
 * int NoDivTriTriIsect(float V0[3],float V1[3],float V2[3],
 *                      float U0[3],float U1[3],float U2[3]);
 * 
 * This version computes the line of intersection as well (if they are not coplanar):
 * int tri_tri_intersect_with_isectline(float V0[3],float V1[3],float V2[3], 
 *				        float U0[3],float U1[3],float U2[3],int *coplanar,
 *				        float isectpt1[3],float isectpt2[3]);
 * coplanar returns whether the tris are coplanar
 * isectpt1, isectpt2 are the endpoints of the line of intersection
 */

/* this edge to edge test is based on Franlin Antonio's gem:
   "Faster Line Segment Intersection", in Graphics Gems III,
   pp. 199-202 */ 
/*
__forceinline static BOOL EdgeEdgeTest(const CRuVector3 &v0, const CRuVector3 &u0, const CRuVector3  &u1, int i0, int i1, float aX, float aY)
{
	float bX = u0[i0] - u1[i0];
	float bY = u0[i1] - u1[i1];
	float cX = v0[i0] - u0[i0];
	float cY = v0[i1] - u0[i1];

	float f = aY * bX - aX * bY;
	float d = bY * cX - bX * cY;

	if((f > 0.0f && d >= 0.0f && d <= f) || (f < 0.0f && d <= 0.0f && d >= f))
	{
		float e = aX * cY - aY * cX;

		if(f > 0.0f && e >= 0.0f && e <= f)
			return TRUE;
		else if(e <= 0.0f && e >= f)
			return TRUE;
	}

	return FALSE;
}

__forceinline static BOOL EdgeAgainstTriangleEdges(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, int i0, int i1)
{
	float aX = v1[i0] - v0[i0];
	float aY = v1[i1] - v0[i1];

	// Test edge u0, u1 against v0, v1
	if(EdgeEdgeTest(v0, u0, u1, i0, i1, aX, aY))
		return TRUE;

	// Test edge u1, u2 against v0, v1
	if(EdgeEdgeTest(v0, u1, u2, i0, i1, aX, aY))
		return TRUE;

	// Test edge u2, u1 against v0, v1
	if(EdgeEdgeTest(v0, u2, u1, i0, i1, aX, aY))
		return TRUE;

	return FALSE;
}

__forceinline static BOOL PointInTriangle(const CRuVector3 &v0, const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, int i0, int i1)
{
	float a, b, c, d0, d1, d2;

	// Is T1 completly inside T2?
	// Check if v0 is inside tri(u0, u1, u2)
	a = u1[i1] - u0[i1];
	b = -(u1[i0] - u0[i0]);
	c = -a * u0[i0] - b * u0[i1];
	d0 = a * v0[i0] + b * v0[i1] + c;

	a = u2[i1] - u1[i1];
	b = -(u2[i0] - u1[i0]);
	c = -a * u1[i0] - b * u1[i1];
	d1 = a * v0[i0] + b * v0[i1] + c;

	a = u0[i1] - u2[i1];
	b = -(u0[i0] - u2[i0]);
	c = -a * u2[i0] - b * u2[i1];
	d2 = a * v0[i0] + b * v0[i1] + c;
	if(d0 * d1 > 0.0f)
	{
		if(d0 * d2 > 0.0f)
			return TRUE;
	}

	return FALSE;
}

__forceinline static void TriTriISect(float vv0, float vv1, float vv2, float d0, float d1, float d2, float &isect0, float &isect1)
{
	isect0 = vv0 + (vv1 - vv0) * d0 / (d0 - d1);
	isect1 = vv0 + (vv2 - vv0) * d0 / (d0 - d2);
}

__forceinline static BOOL TriTriComputeIntervals(float vv0, float vv1, float vv2, float d0, float d1, float d2, float d0d1, float d0d2, float &isect0, float &isect1)
{
	if(d0d1 > 0.0f)                                         
	{
		// here we know that d0d2<=0.0
		// that is d0,  d1 are on the same side,  d2 on the other or on the plane
		TriTriISect(vv2, vv0, vv1, d2, d0, d1, isect0, isect1);
	}
	else if(d0d2 > 0.0f)
	{
		// here we know that d0d1<=0.0
		TriTriISect(vv1, vv0, vv2, d1, d0, d2, isect0, isect1);
	}
	else if(d1 * d2 > 0.0f || d0 != 0.0f)
	{
		// here we know that d0d1<=0.0 or that d0 != 0.0
		TriTriISect(vv0, vv1, vv2, d0, d1, d2, isect0, isect1);
	}
	else if(d1 != 0.0f)
	{
		TriTriISect(vv1, vv0, vv2, d1, d0, d2, isect0, isect1);
	}
	else if(d2 != 0.0f)
	{
		TriTriISect(vv2, vv0, vv1, d2, d0, d1, isect0, isect1);
	}
	else
	{
		// Triangles are coplanar - return to callee for further processing
		return TRUE;
	}

	return FALSE;
}

BOOL CoplanarTriangles(const CRuVector3 &n, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, const CRuVector3 &u0,const CRuVector3 &u1,const CRuVector3 &u2)
{
	float a[3];
	int i0, i1;

	// First project onto an axis-aligned plane that maximizes the area of the triangles. Compute indices: i0, i1.
	a[0] = fabs(n.x);
	a[1] = fabs(n.y);
	a[2] = fabs(n.z);

	if(a[0] > a[1])
	{
		if(a[0] > a[2])  
		{
			// a[0] is greatest
			i0 = 1;
			i1 = 2;
		}
		else
		{
			// a[2] is greatest
			i0 = 0;
			i1 = 1;
		}
	}
	else
	{
		// a[0] <= a[1]
		if(a[2] > a[1])
		{
			// a[2] is greatest
			i0 = 0;
			i1 = 1;
		}
		else
		{
			// a[1] is greatest
			i0 = 0;
			i1 = 2;
		}
	}

	// Test all edges of triangle 1 against the edges of triangle 2 
	if(EdgeAgainstTriangleEdges(v0, v1, u0, u1, u2, i0, i1))
		return TRUE;

	if(EdgeAgainstTriangleEdges(v1, v2, u0, u1, u2, i0, i1))
		return TRUE;

	if(EdgeAgainstTriangleEdges(v2, v0, u0, u1, u2, i0, i1))
		return TRUE;

	// Finally, test if tri1 is totally contained in tri2 or vice versa 
	if(PointInTriangle(v0, u0, u1, u2, i0, i1))
		return TRUE;

	if(PointInTriangle(u0, v0, v1, v2, i0, i1))
		return TRUE;

	return FALSE;
}

// NOTE: Precision seems to be good for up to EPSILON
BOOL TriangleIntersectsTriangle(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2)
{
	CRuVector3 e1, e2, n1, n2;
	float d1, d2;
	float du0, du1, du2, dv0, dv1, dv2;
	CRuVector3 D;
	float isect1[2],  isect2[2], temp;
	float du0du1, du0du2, dv0dv1, dv0dv2;
	short index;
	float vp0, vp1, vp2;
	float up0, up1, up2;
	float b, c, max;

	// Compute plane equation of triangle(v0, v1, v2)
	// Plane equation 1: n1.x + d1 = 0
	e1 = v1 - v0;
	e2 = v2 - v0;
	n1.SetToCrossProduct(e1, e2);
	d1 = -DotProduct(n1, v0);

	// Put u0, u1, u2 into plane equation 1 to compute signed distances to the plane
	du0 = DotProduct(n1, u0) + d1;
	du1 = DotProduct(n1, u1) + d1;
	du2 = DotProduct(n1, u2) + d1;

	// Coplanarity robustness check
	if(fabs(du0) < EPSILON)
		du0 = 0.0f;
	if(fabs(du1) < EPSILON)
		du1 = 0.0f;
	if(fabs(du2) < EPSILON)
		du2 = 0.0f;

	du0du1 = du0 * du1;
	du0du2 = du0 * du2;

	// Same sign on all of them + not equal 0 ?
	// No intersection occurs
	if(du0du1 > 0.0f && du0du2 > 0.0f) 
		return FALSE;

	// Compute plane of triangle (u0, u1, u2)
	// Plane equation 2: n2.x + d2 = 0
	e1 = u1 - u0;
	e2 = u2 - u0;
	n2.SetToCrossProduct(e1, e2);
	d2 = -DotProduct(n2, u0);

	// Put v0, v1, v2 into plane equation 2
	dv0 = DotProduct(n2, v0) + d2;
	dv1 = DotProduct(n2, v1) + d2;
	dv2 = DotProduct(n2, v2) + d2;

	if(fabs(dv0) < EPSILON)
		dv0 = 0.0f;
	if(fabs(dv1) < EPSILON)
		dv1 = 0.0f;
	if(fabs(dv2) < EPSILON)
		dv2 = 0.0f;

	dv0dv1 = dv0 * dv1;
	dv0dv2 = dv0 * dv2;

	// Same sign on all of them + not equal 0 ?
	// No intersection occurs
	if(dv0dv1 > 0.0f && dv0dv2 > 0.0f)
		return FALSE;

	// Compute direction of intersection line
	D.SetToCrossProduct(n1, n2);

	// Compute and index to the largest component of D
	index = 0;
	max = fabs(D.x);
	b = fabs(D.y);
	c = fabs(D.z);
	if(b > max)
	{
		max = b;
		index = 1;
	}
	if(c > max)
	{
		max = c;
		index = 2;
	}

	// This is the simplified projection onto L
	vp0 = v0[index];
	vp1 = v1[index];
	vp2 = v2[index];

	up0 = u0[index];
	up1 = u1[index];
	up2 = u2[index];

	// Compute interval for triangle 1
	if(TriTriComputeIntervals(vp0, vp1, vp2, dv0, dv1, dv2, dv0dv1, dv0dv2, isect1[0], isect1[1]))
	{
		// triangles are coplanar
		return CoplanarTriangles(n1, v0, v1, v2, u0, u1, u2);
	}

	// Compute interval for triangle 2
	if(TriTriComputeIntervals(up0, up1, up2, du0, du1, du2, du0du1, du0du2, isect2[0], isect2[1]))
	{
		// triangles are coplanar
		return CoplanarTriangles(n1, v0, v1, v2, u0, u1, u2);
	}

	// Make sure isect1 & 2's elements are in ascending order
	if(isect1[0] > isect1[1])
	{
		temp = isect1[0];
		isect1[0] = isect1[1];
		isect1[1] = temp;
	}
	if(isect2[0] > isect2[1])
	{
		temp = isect2[0];
		isect2[0] = isect2[1];
		isect2[1] = temp;
	}

	if(isect1[1] < isect2[0] || isect2[1] < isect1[0])
		return FALSE;

	return TRUE;
}

__forceinline void ISect2(const CRuVector3 &vtx0, const CRuVector3 &vtx1, const CRuVector3 &vtx2, float vv0, float vv1, float vv2, float d0, float d1, float d2, float &isect0, float &isect1, CRuVector3 &isectPoint0, CRuVector3 &isectPoint1)
{
	float tmp = d0 / (d0 - d1);
	isect0 = vv0 + (vv1 - vv0) * tmp;
	isectPoint0 = ((vtx1 - vtx0) * tmp) + vtx0;

	tmp = d0 / (d0 - d2);
	isect1 = vv0 + (vv2 - vv0) * tmp;
	isectPoint1 = ((vtx2 - vtx0) * tmp) + vtx0;
}

__forceinline BOOL TriTriComputeIntervalsISectLine(const CRuVector3 &vert0, const CRuVector3 &vert1, const CRuVector3 &vert2, float vv0, float vv1, float vv2, float d0, float d1, float d2, float d0d1, float d0d2, float &isect0, float &isect1, CRuVector3 &isectPoint0, CRuVector3 &isectPoint1)
{
	if(d0d1 > 0.0f)
	{
		// here we know that d0d2<=0.0
		// that is d0, d1 are on the same side, d2 on the other or on the plane
		ISect2(vert2, vert0, vert1, vv2, vv0, vv1, d2, d0, d1, isect0, isect1, isectPoint0, isectPoint1);
	}
	else if(d0d2 > 0.0f)
	{
		// here we know that d0d1<=0.0
		ISect2(vert1, vert0, vert2, vv1, vv0, vv2, d1, d0, d2, isect0, isect1, isectPoint0, isectPoint1);
	}
	else if(d1 * d2 > 0.0f || d0 != 0.0f)
	{
		// here we know that d0d1<=0.0 or that d0!=0.0
		ISect2(vert0, vert1, vert2, vv0, vv1, vv2, d0, d1, d2, isect0, isect1, isectPoint0, isectPoint1);
	}
	else if(d1 != 0.0f)
	{
		ISect2(vert1, vert0, vert2, vv1, vv0, vv2, d1, d0, d2, isect0, isect1, isectPoint0, isectPoint1);
	}
	else if(d2 != 0.0f)
	{
		ISect2(vert2, vert0, vert1, vv2, vv0, vv1, d2, d0, d1, isect0, isect1, isectPoint0, isectPoint1);
	}
	else
	{
		// triangles are coplanar
		return TRUE;
	}

	return FALSE;
}

BOOL FindTriangleIntersection(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, BOOL &coplanar, CRuVector3 &iPoint1Out, CRuVector3 &iPoint2Out)
{
	CRuVector3 e1, e2, n1, n2;
	float d1, d2;
	float du0, du1, du2, dv0, dv1, dv2;
	CRuVector3 D;
	float isect1[2],  isect2[2], temp;
	float du0du1, du0du2, dv0dv1, dv0dv2;
	int index;
	float vp0, vp1, vp2;
	float up0, up1, up2;
	float b, c, max;
	int smallest1 = 0, smallest2 = 0;

	// Compute plane equation of triangle(v0, v1, v2)
	// Plane equation 1: n1.x + d1 = 0
	e1 = v1 - v0;
	e2 = v2 - v0;
	n1.SetToCrossProduct(e1, e2);
	d1 = -DotProduct(n1, v0);

	// Put u0, u1, u2 into plane equation 1 to compute signed distances to the plane
	du0 = DotProduct(n1, u0) + d1;
	du1 = DotProduct(n1, u1) + d1;
	du2 = DotProduct(n1, u2) + d1;

	// Coplanarity robustness check
	if(fabs(du0) < EPSILON)
		du0 = 0.0f;
	if(fabs(du1) < EPSILON)
		du1 = 0.0f;
	if(fabs(du2) < EPSILON)
		du2 = 0.0f;

	du0du1 = du0 * du1;
	du0du2 = du0 * du2;

	// Same sign on all of them + not equal 0 ?
	// No intersection occurs
	if(du0du1 > 0.0f && du0du2 > 0.0f) 
		return FALSE;

	// Compute plane of triangle (u0, u1, u2)
	// Plane equation 2: n2.x + d2 = 0
	e1 = u1 - u0;
	e2 = u2 - u0;
	n2.SetToCrossProduct(e1, e2);
	d2 = -DotProduct(n2, u0);

	// Put v0, v1, v2 into plane equation 2
	dv0 = DotProduct(n2, v0) + d2;
	dv1 = DotProduct(n2, v1) + d2;
	dv2 = DotProduct(n2, v2) + d2;

	if(fabs(dv0) < EPSILON)
		dv0 = 0.0f;
	if(fabs(dv1) < EPSILON)
		dv1 = 0.0f;
	if(fabs(dv2) < EPSILON)
		dv2 = 0.0f;

	dv0dv1 = dv0 * dv1;
	dv0dv2 = dv0 * dv2;

	// Same sign on all of them + not equal 0 ?
	// No intersection occurs
	if(dv0dv1 > 0.0f && dv0dv2 > 0.0f)
		return FALSE;

	// Compute direction of intersection line
	D.SetToCrossProduct(n1, n2);

	// Compute and index to the largest component of D
	index = 0;
	max = fabs(D.x);
	b = fabs(D.y);
	c = fabs(D.z);
	if(b > max)
	{
		max = b;
		index = 1;
	}
	if(c > max)
	{
		max = c;
		index = 2;
	}

	// This is the simplified projection onto L
	vp0 = v0[index];
	vp1 = v1[index];
	vp2 = v2[index];

	up0 = u0[index];
	up1 = u1[index];
	up2 = u2[index];

	// Compute interval for triangle 1
	CRuVector3 isectPointA1, isectPointA2, isectPointB1, isectPointB2;
	coplanar = TriTriComputeIntervalsISectLine(v0, v1, v2, vp0, vp1, vp2, dv0, dv1, dv2, dv0dv1, dv0dv2, isect1[0], isect1[1], isectPointA1, isectPointA2);
	if(coplanar)
		return CoplanarTriangles(n1, v0, v1, v2, u0, u1, u2);

	// Compute interval for triangle 2
	TriTriComputeIntervalsISectLine(u0, u1, u2, up0, up1, up2, du0, du1, du2, du0du1, du0du2, isect2[0], isect2[1], isectPointB1, isectPointB2);

	// Make sure isect1 & 2's elements are in ascending order
	if(isect1[0] > isect1[1])
	{
		temp = isect1[0];
		isect1[0] = isect1[1];
		isect1[1] = temp;
		smallest1 = 1;
	}
	if(isect2[0] > isect2[1])
	{
		temp = isect2[0];
		isect2[0] = isect2[1];
		isect2[1] = temp;
		smallest2 = 1;
	}

	if(isect1[1] < isect2[0] || isect2[1] < isect1[0])
		return FALSE;

	// Triangles intersect - determine points of intersection
	if(isect2[0] < isect1[0])
	{
		if(smallest1 == 0)
			iPoint1Out.Set(isectPointA1);
		else
			iPoint1Out.Set(isectPointA2);

		if(isect2[1] < isect1[1])
		{
			if(smallest2 == 0)
				iPoint2Out.Set(isectPointB2);
			else
				iPoint2Out.Set(isectPointB1);
		}
		else
		{
			if(smallest1 == 0)
				iPoint2Out.Set(isectPointA2);
			else
				iPoint2Out.Set(isectPointA1);
		}
	}
	else
	{
		if(smallest2 == 0)
			iPoint1Out.Set(isectPointB1);
		else
			iPoint1Out.Set(isectPointB2);

		if(isect2[1] > isect1[1])
		{
			if(smallest1 == 0)
				iPoint2Out.Set(isectPointA2);
			else
				iPoint2Out.Set(isectPointA1);
		}
		else
		{
			if(smallest2 == 0)
				iPoint2Out.Set(isectPointB2);
			else
				iPoint2Out.Set(isectPointB1);
		}
	}

	return TRUE;
}






















//*/

/*
// ERIT version

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
BOOL TriangleIntersectsTriangle2(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2)
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
*/








void ClipConvexPolygonAgainstPlane(const CRuVector3& rkNormal, float fConstant, int& riQuantity, CRuVector3* akP)
{
    // The input vertices are assumed to be in counterclockwise order.  The
    // ordering is an invariant of this function.  The size of array akP is
    // assumed to be large enough to store the clipped polygon vertices.

    // test on which side of line are the vertices
    int iPositive = 0, iNegative = 0, iPIndex = -1;
    int iQuantity = riQuantity;

    float afTest[8];
    int i;
    for (i = 0; i < riQuantity; i++)
    {

        // An epsilon is used here beCRuse it is possible for the dot product
        // and fC to be exactly equal to each other (in theory), but differ
        // slightly beCRuse of floating point problems.  Thus, add a little
        // to the test number to push actually equal numbers over the edge
        // towards the positive.

        // TO DO: This should probably be somehow a relative tolerance, and
        // I do not think multiplying by the constant is somehow the best
        // way to do this.
//        afTest[i] = DotProduct(rkNormal, akP[i]) - fConstant;
        afTest[i] = DotProduct(rkNormal, akP[i]) - fConstant + (float) fabs(fConstant) * FLT_EPSILON;
//        afTest[i] = DotProduct(rkNormal, akP[i]) - fConstant + fabs(fConstant) * EPSILON;

        if ( afTest[i] >= (float)0.0 )
        {
            iPositive++;
            if ( iPIndex < 0 )
                iPIndex = i;
        }
        else
        {
            iNegative++;
        }
    }

    if ( riQuantity == 2 ) 
    {
        // Lines are a little different, in that clipping the segment
        // cannot create a new segment, as clipping a polygon would
        if ( iPositive > 0 )
        {
            if ( iNegative > 0 ) 
            {
                int iClip;

                if ( iPIndex == 0 )
                {
                    // vertex0 positive, vertex1 is clipped
                    iClip = 1;
                }
                else // iPIndex == 1
                {
                    // vertex1 positive, vertex0 clipped
                    iClip = 0;
                }

                float fT = afTest[iPIndex]/(afTest[iPIndex] - afTest[iClip]);
                akP[iClip] = akP[iPIndex] + (akP[iClip] - akP[iPIndex]) * fT;                
            }
            // otherwise both positive, no clipping
        }
        else
        {
            //assert( !"Entire line got clipped.  Shouldn't happen." );
            riQuantity = 0;
        }
    }
    else
    {
        if ( iPositive > 0 )
        {
            if ( iNegative > 0 )
            {
                // plane transversely intersects polygon
                CRuVector3 akCV[8];
                int iCQuantity = 0, iCur, iPrv;
                float fT;

                if ( iPIndex > 0 )
                {
                    // first clip vertex on line
                    iCur = iPIndex;
                    iPrv = iCur-1;
                    fT = afTest[iCur]/(afTest[iCur]-afTest[iPrv]);
                    akCV[iCQuantity++] = akP[iCur]+(akP[iPrv]-akP[iCur])*fT;

                    // vertices on positive side of line
                    while ( iCur < iQuantity && afTest[iCur] >= (float)0.0 )
                        akCV[iCQuantity++] = akP[iCur++];

                    // last clip vertex on line
                    if ( iCur < iQuantity )
                    {
                        iPrv = iCur-1;
                    }
                    else
                    {
                        iCur = 0;
                        iPrv = iQuantity - 1;
                    }
                    fT = afTest[iCur]/(afTest[iCur]-afTest[iPrv]);
                    akCV[iCQuantity++] = akP[iCur]+(akP[iPrv]-akP[iCur])*fT;
                }
                else  // iPIndex is 0
                {
                    // vertices on positive side of line
                    iCur = 0;
                    while ( iCur < iQuantity && afTest[iCur] >= (float)0.0 )
                        akCV[iCQuantity++] = akP[iCur++];

                    // last clip vertex on line
                    iPrv = iCur-1;
                    fT = afTest[iCur]/(afTest[iCur]-afTest[iPrv]);
                    akCV[iCQuantity++] = akP[iCur]+(akP[iPrv]-akP[iCur])*fT;

                    // skip vertices on negative side
                    while ( iCur < iQuantity && afTest[iCur] < (float)0.0 )
                        iCur++;

                    // first clip vertex on line
                    if ( iCur < iQuantity )
                    {
                        iPrv = iCur-1;
                        fT = afTest[iCur]/(afTest[iCur] - afTest[iPrv]);
                        akCV[iCQuantity++] = akP[iCur]+(akP[iPrv]-akP[iCur])*fT;

                        // vertices on positive side of line
                        while ( iCur < iQuantity
                        &&      afTest[iCur] >= (float)0.0 )
                        {
                            akCV[iCQuantity++] = akP[iCur++];
                        }
                    }
                    else
                    {
                        // iCur = 0
                        iPrv = iQuantity - 1;
                        fT = afTest[0]/(afTest[0]-afTest[iPrv]);
                        akCV[iCQuantity++] = akP[0]+(akP[iPrv]-akP[0])*fT;
                    }
                }

                iQuantity = iCQuantity;
                memcpy(akP,akCV,iCQuantity*sizeof(CRuVector3));
            }
            // else polygon fully on positive side of plane, nothing to do

            riQuantity = iQuantity;
        }
        else
        {
            // Polygon does not intersect positive side of plane, clip all.
            // This should not ever happen if called by the findintersect
            // routines after an intersection has been determined.
            riQuantity = 0;
        }    
    }
}

void FindContactSetColinearLineTri(const CRuVector3 akU[2], const CRuVector3 akV[3], int& riQuantity, CRuVector3* akP)
{
    // The potential intersection is initialized to the line segment and then
    // clipped against the three sides of the tri

    riQuantity = 2;
    memcpy(akP,akU,2*sizeof(CRuVector3));

    CRuVector3 akSide[3] =
    {
        akV[1] - akV[0],
        akV[2] - akV[1],
        akV[0] - akV[2]
    };

    CRuVector3 kN = CrossProduct(akSide[0], akSide[1]);
    for (int i = 0; i < 3; i++)
    {
        // normal pointing inside the triangle
        CRuVector3 kSideN = CrossProduct(kN, akSide[i]);
        float fConstant = kSideN.Dot(akV[i]);
        ClipConvexPolygonAgainstPlane(kSideN,fConstant,riQuantity,akP);
    }
}

void FindContactSetNonCoplanarTri (const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, int &riQuantity, CRuVector3 akP[6])
{
	CRuVector3 akU[3] = { u0, u1, u2 };
	CRuVector3 akV[3] = { v0, v1, v2 };

	// This must necessarily return a line or a point, depending on the
    // orientation of the triangles.

    // On an infinite plane coplanar with U, it necessarily crosses
    // two segments of the triangle V (if not coplanar)

    // This routine picks those two segments and projects their
    // intersection onto that plane, so that it is the same case
    // as a colinear line-tri intersection.

    CRuVector3 kEdge1 = akU[1] - akU[0];
    CRuVector3 kEdge2 = akU[2] - akU[0];
    CRuVector3 kNormU = CrossProduct(kEdge1, kEdge2);

    float afV[3], fU = kNormU.Dot(akU[0]);
    int iV[3] = { 0, 1, 2 };
    int iTemp;

    // Project V and sort those projections
    for (int i = 0; i < 3; i++)
    {
        afV[i] = kNormU.Dot( akV[i] );
        for (int i2 = i; i2 > 0; i2--)
        {
            // insertion sort vertex order in iV for values in fV
            if ( afV[iV[i2]] < afV[iV[i2-1]] )
            {
                // swap
                iTemp = iV[i2];
                iV[i2] = iV[i2-1];
                iV[i2-1] = iTemp;
            }
        }
    }

    // At this point, afV contains the projections and iV[0] is the 
    // index into afV of the smallest value, and iV[2] is the largest.

    // Four cases (for an infinite plane coplanar with U through V
    // 1) Plane through a line
    // 2) Plane through a vertex
    // 3) Plane through an edge and a vertex
    // 4) Plane through two edges

    // In all cases but the 2nd, the resulting contact set is a line
    // This will attempt to find two points on that line (to determine
    // endpoints) by picking two segments from the triangle to make
    // that line.  This is done by picking a single vertex that is
    // contained by both of those segments, and then getting the points.

    int iVertex;

    if ( afV[iV[0]] == fU )
    {
        if ( afV[iV[1]] == fU )
        {
            // plane through a line
            iVertex = 2;
        }
        else
        {
            // plane through a vertex (vertex iV[0])
            riQuantity = 1;
            akP[0] = akV[iV[0]];
            return;
        }
    }
    else if ( afV[iV[2]] == fU )
    {
        if ( afV[iV[1]] == fU )
        {
            // plane through a line
            iVertex = 0;
        }
        else
        {
            // plane through a vertex (vertex iV[2])
            riQuantity = 1;
            akP[0] = akV[iV[2]];
            return;
        }
    }
    else if ( afV[iV[1]] == fU )
    {
        // plane through an edge and a vertex
        // either vertex is fine to pick lines from (except for 1)
        iVertex = 0;
    }
    else
    {
        if ( afV[iV[1]] < fU )  // two points below the plane
            iVertex = 2;
        else  // two points above the plane
            iVertex = 0;
    }

    // Now, find the intersecting points on those two segments in U
    // segment i1-i2 and i2-i3

    int i1 = iV[(iVertex + 2) % 3];
    int i2 = iV[iVertex];
    int i3 = iV[(iVertex + 1) % 3];

    CRuVector3 akProj[2];
    akProj[0] = akV[i1] + (akV[i2] - akV[i1]) * ((fU - afV[i1]) / (afV[i2] - afV[i1]));
    akProj[1] = akV[i2] + (akV[i3] - akV[i2]) * ((fU - afV[i2]) / (afV[i3] - afV[i2]));

    FindContactSetColinearLineTri(akProj,akU,riQuantity,akP);
}

void FindContactSetCoplanarTri(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, int& riQuantity, CRuVector3 akP[6])
{
    // The potential intersection is initialized to the V-triangle.  The
    // set of vertices is refined based on clipping against each edge of the
    // U-triangle.

	CRuVector3 akU[3] = { u0, u1, u2 };
	CRuVector3 akV[3] = { v0, v1, v2 };

    riQuantity = 3;
    memcpy(akP,akV,3*sizeof(CRuVector3));

    CRuVector3 kEdge1 = akU[1] - akU[0];
    CRuVector3 kEdge2 = akU[2] - akU[0];
    CRuVector3 kN = CrossProduct(kEdge1, kEdge2);

    for (int i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        // This points inward to U.
        CRuVector3 kNxSeg = CrossProduct(kN, akU[i1] - akU[i0]);
        float fConstant = kNxSeg.Dot(akU[i0]);
        ClipConvexPolygonAgainstPlane(kNxSeg,fConstant,riQuantity,akP);
    }
}
/*
BOOL FindTriangleIntersection(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, int &numIntPoints, CRuVector3 intPoints[6])
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

        FindContactSetNonCoplanarTri(u0, u1, u2, v0, v1, v2, numIntPoints, intPoints);
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

        FindContactSetCoplanarTri(u0, u1, u2, v0, v1, v2, numIntPoints, intPoints);
    }

    return TRUE;
}
*/



#define ORIENT_2D(a, b, c)  ((a[0]-c[0])*(b[1]-c[1])-(a[1]-c[1])*(b[0]-c[0]))



#define INTERSECTION_TEST_VERTEX(P1, Q1, R1, P2, Q2, R2) {\
  if (ORIENT_2D(R2, P2, Q1) >= 0.0f)\
    if (ORIENT_2D(R2, Q2, Q1) <= 0.0f)\
      if (ORIENT_2D(P1, P2, Q1) > 0.0f) {\
	if (ORIENT_2D(P1, Q2, Q1) <= 0.0f) return TRUE; \
	else return FALSE;} else {\
	if (ORIENT_2D(P1, P2, R1) >= 0.0f)\
	  if (ORIENT_2D(Q1, R1, P2) >= 0.0f) return TRUE; \
	  else return FALSE;\
	else return FALSE;}\
    else \
      if (ORIENT_2D(P1, Q2, Q1) <= 0.0f)\
	if (ORIENT_2D(R2, Q2, R1) <= 0.0f)\
	  if (ORIENT_2D(Q1, R1, Q2) >= 0.0f) return TRUE; \
	  else return FALSE;\
	else return FALSE;\
      else return FALSE;\
  else\
    if (ORIENT_2D(R2, P2, R1) >= 0.0f) \
      if (ORIENT_2D(Q1, R1, R2) >= 0.0f)\
	if (ORIENT_2D(P1, P2, R1) >= 0.0f) return TRUE;\
	else return FALSE;\
      else \
	if (ORIENT_2D(Q1, R1, Q2) >= 0.0f) {\
	  if (ORIENT_2D(R2, R1, Q2) >= 0.0f) return TRUE; \
	  else return FALSE; }\
	else return FALSE; \
    else  return FALSE; \
 };



#define INTERSECTION_TEST_EDGE(P1, Q1, R1, P2, Q2, R2) { \
  if (ORIENT_2D(R2, P2, Q1) >= 0.0f) {\
    if (ORIENT_2D(P1, P2, Q1) >= 0.0f) { \
        if (ORIENT_2D(P1, Q1, R2) >= 0.0f) return TRUE; \
        else return FALSE;} else { \
      if (ORIENT_2D(Q1, R1, P2) >= 0.0f){ \
	if (ORIENT_2D(R1, P1, P2) >= 0.0f) return TRUE; else return FALSE;} \
      else return FALSE; } \
  } else {\
    if (ORIENT_2D(R2, P2, R1) >= 0.0f) {\
      if (ORIENT_2D(P1, P2, R1) >= 0.0f) {\
	if (ORIENT_2D(P1, R1, R2) >= 0.0f) return TRUE;  \
	else {\
	  if (ORIENT_2D(Q1, R1, R2) >= 0.0f) return TRUE; else return FALSE;}}\
      else  return FALSE; }\
    else return FALSE; }}

BOOL ccw_tri_tri_intersection_2d(float p1[2], float q1[2], float r1[2], float p2[2], float q2[2], float r2[2])
{
	if(ORIENT_2D(p2, q2, p1) >= 0.0f)
	{
		if(ORIENT_2D(q2, r2, p1) >= 0.0f)
		{
			if(ORIENT_2D(r2, p2, p1) >= 0.0f)
				return TRUE;
			else
				INTERSECTION_TEST_EDGE(p1, q1, r1, p2, q2, r2)
		}
		else
		{
			if(ORIENT_2D(r2, p2, p1) >= 0.0f)
				INTERSECTION_TEST_EDGE(p1, q1, r1, r2, p2, q2)
			else
				INTERSECTION_TEST_VERTEX(p1, q1, r1, p2, q2, r2)
		}
	}
	else
	{
		if(ORIENT_2D(q2, r2, p1) >= 0.0f)
		{
			if(ORIENT_2D(r2, p2, p1) >= 0.0f)
				INTERSECTION_TEST_EDGE(p1, q1, r1, q2, r2, p2)
			else
				INTERSECTION_TEST_VERTEX(p1, q1, r1, q2, r2, p2)
		}
		else
			INTERSECTION_TEST_VERTEX(p1, q1, r1, r2, p2, q2)
	}
}

BOOL tri_tri_overlap_test_2d(float p1[2], float q1[2], float r1[2], float p2[2], float q2[2], float r2[2])
{
	if(ORIENT_2D(p1, q1, r1) < 0.0f)
		if(ORIENT_2D(p2, q2, r2) < 0.0f)
			return ccw_tri_tri_intersection_2d(p1, r1, q1, p2, r2, q2);
		else
			return ccw_tri_tri_intersection_2d(p1, r1, q1, p2, q2, r2);
	else
		if(ORIENT_2D(p2, q2, r2) < 0.0f)
			return ccw_tri_tri_intersection_2d(p1, q1, r1, p2, r2, q2);
		else
			return ccw_tri_tri_intersection_2d(p1, q1, r1, p2, q2, r2);
}

BOOL coplanar_tri_tri3d(const CRuVector3 &p1, const CRuVector3 &q1, const CRuVector3 &r1, const CRuVector3 &p2, const CRuVector3 &q2, const CRuVector3 &r2, const CRuVector3 &normal_1, const CRuVector3 &normal_2)
{
	float P1[2], Q1[2], R1[2];
	float P2[2], Q2[2], R2[2];

	float n_x, n_y, n_z;

	n_x =((normal_1[0]<0)?-normal_1[0]:normal_1[0]);
	n_y =((normal_1[1]<0)?-normal_1[1]:normal_1[1]);
	n_z =((normal_1[2]<0)?-normal_1[2]:normal_1[2]);


	// Projection of the triangles in 3D onto 2D such that the area of
	// the projection is maximized.

	if((n_x > n_z) &&(n_x >= n_y))
	{
		// Project onto plane YZ
		P1[0] = q1[2]; P1[1] = q1[1];
		Q1[0] = p1[2]; Q1[1] = p1[1];
		R1[0] = r1[2]; R1[1] = r1[1]; 

		P2[0] = q2[2]; P2[1] = q2[1];
		Q2[0] = p2[2]; Q2[1] = p2[1];
		R2[0] = r2[2]; R2[1] = r2[1]; 

	}
	else if((n_y > n_z) &&(n_y >= n_x))
	{
		// Project onto plane XZ
		P1[0] = q1[0]; P1[1] = q1[2];
		Q1[0] = p1[0]; Q1[1] = p1[2];
		R1[0] = r1[2]; R1[1] = r1[1]; 

		P2[0] = q2[0]; P2[1] = q2[2];
		Q2[0] = p2[0]; Q2[1] = p2[2];
		R2[0] = r2[2]; R2[1] = r2[1]; 

	}
	else
	{
		// Project onto plane XY
		P1[0] = p1[0]; P1[1] = p1[1]; 
		Q1[0] = q1[0]; Q1[1] = q1[1]; 
		R1[0] = r1[0]; R1[1] = r1[1]; 

		P2[0] = p2[0]; P2[1] = p2[1]; 
		Q2[0] = q2[0]; Q2[1] = q2[1]; 
		R2[0] = r2[0]; R2[1] = r2[1]; 
	}

	return tri_tri_overlap_test_2d(P1, Q1, R1, P2, Q2, R2);
}

BOOL CONSTRUCT_INTERSECTION(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, const CRuVector3 &N1, const CRuVector3 &N2, CRuVector3 &source, CRuVector3 &target)
{
	float alpha;
	CRuVector3 N, e, e1, e2;

	e1 = u1 - u0;
	e2 = v2 - u0;
	N.SetToCrossProduct(e1, e2);
	e = v0 - u0;

	if(DotProduct(e, N) > 0.0f)
	{
		e1 = u2 - u0;
		N.SetToCrossProduct(e1, e2);
		if(DotProduct(e, N) <= 0.0f)
		{
			e2 = v1 - u0;
			N.SetToCrossProduct(e1, e2);
			if(DotProduct(e, N) > 0.0f)
			{
				e1 = u0 - v0;
				e2 = u0 - u2;
				alpha = DotProduct(e1, N2) / DotProduct(e2, N2);
				e1 = e2 * alpha;
				source = u0 - e1;
				e1 = v0 - u0;
				e2 = v0 - v2;
				alpha = DotProduct(e1, N1) / DotProduct(e2, N1);
				e1 = e2 * alpha;
				target = v0 - e1;

				return TRUE;
			}
			else
			{
				e1 = v0 - u0;
				e2 = v0 - v1;
				alpha = DotProduct(e1, N1) / DotProduct(e2, N1);
				e1 = e2 * alpha;
				source = v0 - e1;
				e1 = v0 - u0;
				e2 = v0 - v2;
				alpha = DotProduct(e1, N1) / DotProduct(e2, N1);
				e1 = e2 * alpha;
				target = v0 - e1;

				return TRUE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		e2 = v1 - u0;
		N.SetToCrossProduct(e1, e2);
		if(DotProduct(e, N) < 0.0f)
		{
			return FALSE;
		}
		else
		{
			e1 = u2 - u0;
			N.SetToCrossProduct(e1, e2);
			if(DotProduct(e, N) >= 0.0f)
			{
				e1 = u0 - v0;
				e2 = u0 - u2;
				alpha = DotProduct(e1, N2) / DotProduct(e2, N2);
				e1 = e2 * alpha;
				source = u0 - e1;
				e1 = u0 - v0;
				e2 = u0 - u1;
				alpha = DotProduct(e1, N2) / DotProduct(e2, N2);
				e1 = e2 * alpha;
				target = u0 - e1;

				return TRUE;
			}
			else
			{
				e1 = v0 - u0;
				e2 = v0 - v1;
				alpha = DotProduct(e1, N1) / DotProduct(e2, N1);
				e1 = e2 * alpha;
				source = v0 - e1;
				e1 = u0 - v0;
				e2 = u0 - u1;
				alpha = DotProduct(e1, N2) / DotProduct(e2, N2);
				e1 = e2 * alpha;
				target = u0 - e1;

				return TRUE;
			}
		}
	}
}

BOOL TRI_TRI_INTER_3D(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, float dv0, float dv1, float dv2, const CRuVector3 &N1, const CRuVector3 &N2, BOOL &coplanar, CRuVector3 &source, CRuVector3 &target)
{
	if(dv0 > 0.0f)
	{
		if(dv1 > 0.0f)
			return CONSTRUCT_INTERSECTION(u0, u2, u1, v2, v0, v1, N1, N2, source, target);
		else if(dv2 > 0.0f)
			return CONSTRUCT_INTERSECTION(u0, u2, u1, v1, v2, v0, N1, N2, source, target);
		else
			return CONSTRUCT_INTERSECTION(u0, u1, u2, v0, v1, v2, N1, N2, source, target);
	}
	else if(dv0 < 0.0f)
	{
		if(dv1 < 0.0f)
			return CONSTRUCT_INTERSECTION(u0, u1, u2, v2, v0, v1, N1, N2, source, target);
		else if(dv2 < 0.0f)
			return CONSTRUCT_INTERSECTION(u0, u1, u2, v1, v2, v0, N1, N2, source, target);
		else
			return CONSTRUCT_INTERSECTION(u0, u2, u1, v0, v1, v2, N1, N2, source, target);
	}
	else
	{
		if(dv1 < 0.0f)
		{
			if(dv2 >= 0.0f)
				return CONSTRUCT_INTERSECTION(u0, u2, u1, v1, v2, v0, N1, N2, source, target);
			else
				return CONSTRUCT_INTERSECTION(u0, u1, u2, v0, v1, v2, N1, N2, source, target);
		}
		else if(dv1 > 0.0f)
		{
			if(dv2 > 0.0f)
				return CONSTRUCT_INTERSECTION(u0, u2, u1, v0, v1, v2, N1, N2, source, target);
			else
				return CONSTRUCT_INTERSECTION(u0, u1, u2, v1, v2, v0, N1, N2, source, target);
		}
		else
		{
			if(dv2 > 0.0f)
				return CONSTRUCT_INTERSECTION(u0, u1, u2, v2, v0, v1, N1, N2, source, target);
			else if(dv2 < 0.0f)
				return CONSTRUCT_INTERSECTION(u0, u2, u1, v2, v0, v1, N1, N2, source, target);
			else
			{
				coplanar = TRUE; 
				return coplanar_tri_tri3d(u0, u1, u2, v0, v1, v2, N1, N2);
			}
		}
	}

	return FALSE;
}

BOOL tri_tri_intersection_test_3d(const CRuVector3 &u0, const CRuVector3 &u1, const CRuVector3 &u2, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, BOOL &coplanar, CRuVector3 &source, CRuVector3 &target)
{
	float du0, du1, du2, dv0, dv1, dv2;
	CRuVector3 e1, e2;
	CRuVector3 N1, N2;

	// Compute distance signs  of u0, u1 and u2 to the plane of triangle(v0, v1, v2)
	e1 = v0 - v2;
	e2 = v1 - v2;
	N2.SetToCrossProduct(e1, e2);

	e1 = u0 - v2;
	du0 = DotProduct(e1, N2);
	e1 = u1 - v2;
	du1 = DotProduct(e1, N2);
	e1 = u2 - v2;
	du2 = DotProduct(e1, N2);

	if(((du0 * du1) > 0.0f) &&((du0 * du2) > 0.0f))
		return FALSE;

	// Compute distance signs  of v0, v1 and v2 to the plane of triangle(u0, u1, u2)
	e1 = u1 - u0;
	e2 = u2 - u0;
	N1.SetToCrossProduct(e1, e2);

	e1 = v0 - u2;
	dv0 = DotProduct(e1, N1);
	e1 = v1 - u2;
	dv1 = DotProduct(e1, N1);
	e1 = v2 - u2;
	dv2 = DotProduct(e1, N1);

	if(((dv0 * dv1) > 0.0f) &&((dv0 * dv2) > 0.0f))
		return FALSE;

	// Permutation in a canonical form of T1's vertices
	if(du0 > 0.0f)
	{
		if(du1 > 0.0f)
			return TRI_TRI_INTER_3D(u2, u0, u1, v0, v2, v1, dv0, dv2, dv1, N1, N2, coplanar, source, target);
		else if(du2 > 0.0f)
			return TRI_TRI_INTER_3D(u1, u2, u0, v0, v2, v1, dv0, dv2, dv1, N1, N2, coplanar, source, target);
		else
			return TRI_TRI_INTER_3D(u0, u1, u2, v0, v1, v2, dv0, dv1, dv2, N1, N2, coplanar, source, target);
	}
	else if(du0 < 0.0f)
	{
		if(du1 < 0.0f)
			return TRI_TRI_INTER_3D(u2, u0, u1, v0, v1, v2, dv0, dv1, dv2, N1, N2, coplanar, source, target);
		else if(du2 < 0.0f)
			return TRI_TRI_INTER_3D(u1, u2, u0, v0, v1, v2, dv0, dv1, dv2, N1, N2, coplanar, source, target);
		else
			return TRI_TRI_INTER_3D(u0, u1, u2, v0, v2, v1, dv0, dv2, dv1, N1, N2, coplanar, source, target);
	}
	else
	{
		if(du1 < 0.0f)
		{
			if(du2 >= 0.0f)
				return TRI_TRI_INTER_3D(u1, u2, u0, v0, v2, v1, dv0, dv2, dv1, N1, N2, coplanar, source, target);
			else
				return TRI_TRI_INTER_3D(u0, u1, u2, v0, v1, v2, dv0, dv1, dv2, N1, N2, coplanar, source, target);
		}
		else if(du1 > 0.0f)
		{
			if(du2 > 0.0f)
				return TRI_TRI_INTER_3D(u0, u1, u2, v0, v2, v1, dv0, dv2, dv1, N1, N2, coplanar, source, target);
			else
				return TRI_TRI_INTER_3D(u1, u2, u0, v0, v1, v2, dv0, dv1, dv2, N1, N2, coplanar, source, target);
		}
		else
		{
			if(du2 > 0.0f)
				return TRI_TRI_INTER_3D(u2, u0, u1, v0, v1, v2, dv0, dv1, dv2, N1, N2, coplanar, source, target);
			else if(du2 < 0.0f)
				return TRI_TRI_INTER_3D(u2, u0, u1, v0, v2, v1, dv0, dv2, dv1, N1, N2, coplanar, source, target);
			else
			{
				// triangles are co-planar
				coplanar = TRUE;
				return coplanar_tri_tri3d(u0, u1, u2, v0, v1, v2, N1, N2);
			}
		}
	}

	// Should never get here
	return FALSE;
}










float PointToLineSquareDistance(const CRuVector3 &pt, const CRuVector3 &v0, const CRuVector3 &v1)
{
	CRuVector3 dir = v1 - v0;
	CRuVector3 diff = pt - v0;

	float t = DotProduct(diff, dir);

	if(t <= 0.0f)
		t = 0.0f;
	else
	{
		float len = dir.SquaredMagnitude();

		if(t >= len)
		{
			t = 1.0f;
			diff = diff - dir;
		}
		else
		{
			t = t / len;
			diff = diff - (dir * t);
		}
	}

	return diff.Magnitude();
}

__forceinline BOOL PointIsOnEdge(const CRuVector3 &pt, const CRuVector3 &v0, const CRuVector3 &v1)
{
	if(PointToLineSquareDistance(pt, v0, v1) <= 0.0001f)	// Gotta adjust this tolerance term...
		return TRUE;
	return FALSE;
}

__forceinline BOOL PointIsOnTriangleVertex(const CRuVector3 &pt, const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2)
{
	if(pt.IsEquivalent(v0) || pt.IsEquivalent(v1) || pt.IsEquivalent(v2))
		return TRUE;
	return FALSE;
}

// vOut is assumed to be an array of 15 vectors(5 triangles)
int SplitTriangle(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, const CRuVector3 &p0, const CRuVector3 &p1, CRuVector3 *vOut)
{
	// Assume p0 and p1 are distinct if numIntPoints == 2!
	int numIntPoints = 2;

	if(p0.IsEquivalent(p1))
		numIntPoints = 1;

	switch(numIntPoints)
	{
		case 2:
			{
				CRuVector3 u0, u1, u2, cpE, cpV;

				// Case 5 - Points of intersection are on the triangle's vertices
				if(PointIsOnTriangleVertex(p0, v0, v1, v2) && PointIsOnTriangleVertex(p1, v0, v1, v2))
				{
					vOut[0] = v0;
					vOut[1] = v1;
					vOut[2] = v2;

					//MessageBox(NULL, "Case 5", "", MB_OK);

					return 1;
				}

				if(!PointIsOnTriangleVertex(p0, v0, v1, v2) && !PointIsOnTriangleVertex(p1, v0, v1, v2))
				{
					BOOL pointsOnSameEdge = TRUE, pointsOnDifferentEdges = TRUE;

					// Case 1 - Points are interior
					if(!PointIsOnEdge(p0, v0, v1) && !PointIsOnEdge(p0, v1, v2) && !PointIsOnEdge(p0, v2, v0) && !PointIsOnEdge(p1, v0, v1) && !PointIsOnEdge(p1, v1, v2) && !PointIsOnEdge(p1, v2, v0))
					{
						float v0DP = DotProduct((p0 - v0).Normalize(), (p1 - v0).Normalize());
						float v1DP = DotProduct((p0 - v1).Normalize(), (p1 - v1).Normalize());
						float v2DP = DotProduct((p0 - v2).Normalize(), (p1 - v2).Normalize());

						// Find triangle vertices with largest angles with respect to p0-p1
						if(v0DP <= v2DP && v1DP <= v2DP)
						{
							u0 = v0;		u1 = v1;		u2 = v2;
						}
						else if(v0DP <= v1DP && v2DP <= v1DP)
						{
							u0 = v0;		u1 = v2;		u2 = v1;
						}
						else if(v1DP <= v0DP && v2DP <= v0DP)// <-- implied condition
						{
							u0 = v1;		u1 = v2;		u2 = v0;
						}
						else
							//MessageBox(NULL, "UH OH", "", MB_OK);

						// Find intersection closest to u0-u1
						if(PointToLineSquareDistance(p0, u0, u1) < PointToLineSquareDistance(p1, u0, u1))
						{
							cpE = p0;
							cpV = p1;
						}
						else
						{
							cpE = p1;
							cpV = p0;
						}

						vOut[0] = u0;		vOut[1] = p0;		vOut[2] = p1;
						vOut[3] = u1;		vOut[4] = p1;		vOut[5] = p0;
						vOut[6] = u0;		vOut[7] = u1;		vOut[8] = cpE;
						vOut[9] = cpV;		vOut[10] = u2;		vOut[11] = u0;
						vOut[12] = u2;		vOut[13] = cpV;		vOut[14] = u1;

						//MessageBox(NULL, "Case 1", "", MB_OK);

						return 5;
					}

					// Case 3 - Points on same edge (UNTESTED)
					if(PointIsOnEdge(p0, v0, v1) && PointIsOnEdge(p1, v0, v1))
					{
						u0 = v0;
						u1 = v1;
						u2 = v2;
					}
					else if(PointIsOnEdge(p0, v1, v2) && PointIsOnEdge(p1, v1, v2))
					{
						u0 = v1;
						u1 = v2;
						u2 = v0;
					}
					else if(PointIsOnEdge(p0, v2, v0) && PointIsOnEdge(p1, v2, v0))
					{
						u0 = v2;
						u1 = v0;
						u2 = v1;
					}
					else
					{
						pointsOnSameEdge = FALSE;
					}

					if(pointsOnSameEdge)
					{
						// Find point closest to u0 and form u0-x-u2 and y-u1-u2, then u2-x-y
						if((p0 - u0).SquaredMagnitude() < (p1 - u0).SquaredMagnitude())
						{
							vOut[0] = u0;		vOut[1] = p0;		vOut[2] = u2;
							vOut[3] = p1;		vOut[4] = u1;		vOut[5] = u2;
							vOut[6] = u2;		vOut[7] = p0;		vOut[8] = p1;
						}
						else
						{
							vOut[0] = u0;		vOut[1] = p1;		vOut[2] = u2;
							vOut[3] = p0;		vOut[4] = u1;		vOut[5] = u2;
							vOut[6] = u2;		vOut[7] = p1;		vOut[8] = p0;
						}

						//MessageBox(NULL, "Case 3", "", MB_OK);

						return 3;
					}

					// Cases 2-6 determination
					if(PointIsOnEdge(p0, v0, v1))
					{
						if(!PointIsOnEdge(p1, v1, v2) && !PointIsOnEdge(p1, v2, v0))
						{
							u0 = v0;		u1 = v1;		u2 = v2;
							cpV = p1;		cpE = p0;
							pointsOnDifferentEdges = FALSE;
						}
						else if(PointIsOnEdge(p1, v1, v2))
						{
							u0 = v0;		u1 = v2;		u2 = v1;
						}
						else
						{
							u0 = v1;		u1 = v2;		u2 = v0;
						}
					}
					else if(PointIsOnEdge(p0, v1, v2))
					{
						if(!PointIsOnEdge(p1, v2, v0) && !PointIsOnEdge(p1, v0, v1))
						{
							u0 = v1;		u1 = v2;		u2 = v0;
							cpV = p1;		cpE = p0;
							pointsOnDifferentEdges = FALSE;
						}
						else if(PointIsOnEdge(p1, v2, v0))
						{
							u0 = v0;		u1 = v1;		u2 = v2;
						}
						else
						{
							u0 = v0;		u1 = v2;		u2 = v1;
						}
					}
					else if(PointIsOnEdge(p0, v2, v0))
					{
						if(!PointIsOnEdge(p1, v0, v1) && !PointIsOnEdge(p1, v1, v2))
						{
							u0 = v2;		u1 = v0;		u2 = v1;
							cpV = p1;		cpE = p0;
							pointsOnDifferentEdges = FALSE;
						}
						else if(PointIsOnEdge(p1, v0, v1))
						{
							u0 = v1;		u1 = v2;		u2 = v0;
						}
						else
						{
							u0 = v0;		u1 = v1;		u2 = v2;
						}
					}
					else if(PointIsOnEdge(p1, v0, v1))
					{
						if(!PointIsOnEdge(p0, v1, v2) && !PointIsOnEdge(p0, v2, v0))
						{
							u0 = v0;		u1 = v1;		u2 = v2;
							cpV = p0;		cpE = p1;
							pointsOnDifferentEdges = FALSE;
						}
						else if(PointIsOnEdge(p0, v1, v2))
						{
							u0 = v0;		u1 = v2;		u2 = v1;
						}
						else
						{
							u0 = v1;		u1 = v2;		u2 = v0;
						}
					}
					else if(PointIsOnEdge(p1, v1, v2))
					{
						if(!PointIsOnEdge(p0, v2, v0) && !PointIsOnEdge(p0, v0, v1))
						{
							u0 = v1;		u1 = v2;		u2 = v0;
							cpV = p0;		cpE = p1;
							pointsOnDifferentEdges = FALSE;
						}
						else if(PointIsOnEdge(p0, v2, v0))
						{
							u0 = v0;		u1 = v1;		u2 = v2;
						}
						else
						{
							u0 = v0;		u1 = v2;		u2 = v1;
						}
					}
					else if(PointIsOnEdge(p1, v2, v0))
					{
						if(!PointIsOnEdge(p0, v0, v1) && !PointIsOnEdge(p0, v1, v2))
						{
							u0 = v2;		u1 = v0;		u2 = v1;
							cpV = p0;		cpE = p1;
							pointsOnDifferentEdges = FALSE;
						}
						else if(PointIsOnEdge(p0, v0, v1))
						{
							u0 = v1;		u1 = v2;		u2 = v0;
						}
						else
						{
							u0 = v0;		u1 = v1;		u2 = v2;
						}
					}
					else
					{
						//MessageBox(NULL, "WHAT THE", "", MB_OK);
					}

					// Case 6 - Points on different edges
					if(pointsOnDifferentEdges)
					{
						vOut[0] = p0;		vOut[1] = p1;		vOut[2] = u2;
						vOut[3] = u0;		vOut[4] = p0;		vOut[5] = p1;

						if((p0 - u1).Magnitude() < (p1 - u1).Magnitude())
						{
							vOut[6] = p0;		vOut[7] = u0;		vOut[8] = u1;
						}
						else
						{
							vOut[6] = p1;		vOut[7] = u0;		vOut[8] = u1;
						}

						//MessageBox(NULL, "Case 6", "", MB_OK);

						return 3;
					}

					// Case 2 - Point on edge, point inside
					vOut[0] = p0;		vOut[1] = p1;		vOut[2] = u0;
					vOut[3] = p0;		vOut[4] = p1;		vOut[5] = u1;
					vOut[6] = cpV;		vOut[7] = u0;		vOut[8] = u2;
					vOut[9] = cpV;		vOut[10] = u1;		vOut[11] = u2;

					//MessageBox(NULL, "Case 2", "", MB_OK);

					return 4;
				}

				// Case 9 - One point on vertex, one on edge
				BOOL case9 = TRUE;
				if(PointIsOnTriangleVertex(p1, v0, v1, v2))
				{
					cpE = p0;

					if(PointIsOnEdge(p0, v0, v1))
					{
						u0 = v0;		u1 = v1;		u2 = v2;
					}
					else if(PointIsOnEdge(p0, v1, v2))
					{
						u0 = v1;		u1 = v2;		u2 = v0;
					}
					else if(PointIsOnEdge(p0, v2, v0))
					{
						u0 = v2;		u1 = v0;		u2 = v1;
					}
					else
						case9 = FALSE;
				}
				else if(PointIsOnTriangleVertex(p0, v0, v1, v2))
				{
					cpE = p1;

					if(PointIsOnEdge(p1, v0, v1))
					{
						u0 = v0;		u1 = v1;		u2 = v2;
					}
					else if(PointIsOnEdge(p1, v1, v2))
					{
						u0 = v1;		u1 = v2;		u2 = v0;
					}
					else if(PointIsOnEdge(p1, v2, v0))
					{
						u0 = v2;		u1 = v0;		u2 = v1;
					}
					else
						case9 = FALSE;
				}

				if(case9)
				{
					vOut[0] = u2;		vOut[1] = cpE;		vOut[2] = u0;
					vOut[3] = u2;		vOut[4] = cpE;		vOut[5] = u1;

					//MessageBox(NULL, "Case 9", "", MB_OK);

					return 2;
				}

				// Case 4 - One point on vertex, one interior point
				BOOL case4 = TRUE;
				if(PointIsOnTriangleVertex(p0, v0, v1, v2))
				{
					cpV = p1;
				}
				else if(PointIsOnTriangleVertex(p1, v0, v1, v2))
				{
					cpV = p0;
				}
				else
				{
					case4 = FALSE;
				}

				if(case4)
				{
					vOut[0] = v0;		vOut[1] = v1;		vOut[2] = cpV;
					vOut[3] = v1;		vOut[4] = v2;		vOut[5] = cpV;
					vOut[6] = v2;		vOut[7] = v0;		vOut[8] = cpV;

					//MessageBox(NULL, "Case 4", "", MB_OK);

					return 3;
				}
			}

			break;

		case 1:	// Case 7 & 8
		default:
			vOut[0] = v0;
			vOut[1] = v1;
			vOut[2] = v2;

			//MessageBox(NULL, "NO CLIP", "", MB_OK);

			return 1;
	}

	//MessageBox(NULL, "UnCRught", "", MB_OK);

	return 0;
}

#pragma managed
