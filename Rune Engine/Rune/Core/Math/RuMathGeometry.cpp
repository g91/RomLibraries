/*!
	@project Rune
	@file RuMathGeometry.cpp

	Copyright (c) 2004 All rights reserved

	@author Ceranos
	@date 2004-03-09
*/

#include "../../Core/Math/RuMathGeometry.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

float CRuSegment3::GetSquareDistance(const CRuSegment3 &segment) const
{
	CRuVector3 diff = m_origin - segment.Origin();
	float a00 = m_direction.SquaredMagnitude();
	float a01 = -m_direction.Dot(segment.Direction());
	float a11 = segment.Direction().SquaredMagnitude();
	float b0 = diff.Dot(m_direction);
	float c = diff.SquaredMagnitude();
	float determinant = (float) fabs(a00 * a11-a01 * a01);
	float b1, s, t, sqrDist, tmp;

	if(determinant >= EPSILON)
	{
		// line segments are not parallel
		b1 = -diff.Dot(segment.Direction());
		s = a01 * b1-a11 * b0;
		t = a01 * b0-a00 * b1;

		if(s >= 0.0f)
		{
			if(s <= determinant)
			{
				if(t >= 0.0f)
				{
					if(t <= determinant)   // region 0 (interior)
					{
						// minimum at two interior points of 3D lines
						float fInvDet = (1.0f) / determinant;
						s *= fInvDet;
						t *= fInvDet;
						sqrDist = s * (a00 * s + a01 * t + 2.0f * b0) + t * (a01 * s + a11 * t + 2.0f * b1) + c;
					}
					else  // region 3 (side)
					{
						t = 1.0f;
						tmp = a01 + b0;
						if(tmp >= 0.0f)
						{
							s = 0.0f;
							sqrDist = a11 + 2.0f * b1 + c;
						}
						else if(-tmp >= a00)
						{
							s = 1.0f;
							sqrDist = a00 + a11 + c + 2.0f * (b1 + tmp);
						}
						else
						{
							s = -tmp / a00;
							sqrDist = tmp * s + a11 + 2.0f * b1 + c;
						}
					}
				}
				else  // region 7 (side)
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
			}
			else
			{
				if(t >= 0.0f)
				{
					if(t <= determinant)  // region 1 (side)
					{
						s = 1.0f;
						tmp = a01 + b1;
						if(tmp >= 0.0f)
						{
							t = 0.0f;
							sqrDist = a00 + 2.0f * b0 + c;
						}
						else if(-tmp >= a11)
						{
							t = 1.0f;
							sqrDist = a00 + a11 + c + 2.0f * (b0 + tmp);
						}
						else
						{
							t = -tmp / a11;
							sqrDist = tmp * t + a00 + 2.0f * b0 + c;
						}
					}
					else  // region 2 (corner)
					{
						tmp = a01 + b0;
						if(-tmp <= a00)
						{
							t = 1.0f;
							if(tmp >= 0.0f)
							{
								s = 0.0f;
								sqrDist = a11 + 2.0f * b1 + c;
							}
							else
							{
								s = -tmp / a00;
								sqrDist = tmp * s + a11 + 2.0f * b1 + c;
							}
						}
						else
						{
							s = 1.0f;
							tmp = a01 + b1;
							if(tmp >= 0.0f)
							{
								t = 0.0f;
								sqrDist = a00 + 2.0f * b0 + c;
							}
							else if(-tmp >= a11)
							{
								t = 1.0f;
								sqrDist = a00 + a11 + c + 
									2.0f * (b0 + tmp);
							}
							else
							{
								t = -tmp / a11;
								sqrDist = tmp * t + a00 + 2.0f * b0 + c;
							}
						}
					}
				}
				else  // region 8 (corner)
				{
					if(-b0 < a00)
					{
						t = 0.0f;
						if(b0 >= 0.0f)
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
					else
					{
						s = 1.0f;
						tmp = a01 + b1;
						if(tmp >= 0.0f)
						{
							t = 0.0f;
							sqrDist = a00 + 2.0f * b0 + c;
						}
						else if(-tmp >= a11)
						{
							t = 1.0f;
							sqrDist = a00 + a11 + c + 2.0f * (b0 + tmp);
						}
						else
						{
							t = -tmp / a11;
							sqrDist = tmp * t + a00 + 2.0f * b0 + c;
						}
					}
				}
			}
		}
		else 
		{
			if(t >= 0.0f)
			{
				if(t <= determinant)  // region 5 (side)
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
				else  // region 4 (corner)
				{
					tmp = a01 + b0;
					if(tmp < 0.0f)
					{
						t = 1.0f;
						if(-tmp >= a00)
						{
							s = 1.0f;
							sqrDist = a00 + a11 + c + 2.0f * (b1 + tmp);
						}
						else
						{
							s = -tmp / a00;
							sqrDist = tmp * s + a11 + 2.0f * b1 + c;
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
			}
			else   // region 6 (corner)
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
		}
	}
	else
	{
		// line segments are parallel
		if(a01 > 0.0f)
		{
			// direction vectors form an obtuse angle
			if(b0 >= 0.0f)
			{
				s = 0.0f;
				t = 0.0f;
				sqrDist = c;
			}
			else if(-b0 <= a00)
			{
				s = -b0 / a00;
				t = 0.0f;
				sqrDist = b0 * s + c;
			}
			else
			{
				b1 = -diff.Dot(segment.Direction());
				s = 1.0f;
				tmp = a00 + b0;
				if(-tmp >= a01)
				{
					t = 1.0f;
					sqrDist = a00 + a11 + c + 2.0f * (a01 + b0 + b1);
				}
				else
				{
					t = -tmp / a01;
					sqrDist = a00 + 2.0f * b0 + c + t * (a11 * t + 2.0f * (a01 + b1));
				}
			}
		}
		else
		{
			// direction vectors form an acute angle
			if(-b0 >= a00)
			{
				s = 1.0f;
				t = 0.0f;
				sqrDist = a00 + 2.0f * b0 + c;
			}
			else if(b0 <= 0.0f)
			{
				s = -b0 / a00;
				t = 0.0f;
				sqrDist = b0 * s + c;
			}
			else
			{
				b1 = -diff.Dot(segment.Direction());
				s = 0.0f;
				if(b0 >= -a01)
				{
					t = 1.0f;
					sqrDist = a11 + 2.0f * b1 + c;
				}
				else
				{
					t = -b0 / a01;
					sqrDist = c + t * (2.0f * b1 + a11 * t);
				}
			}
		}
	}

	return (float) fabs(sqrDist);
}

// ************************************************************************************************************************************************************

CRuRay3::CRuRay3(const CRuRay3 &ray, const CRuMatrix4x4 &transform)
:	m_origin(ray.m_origin),
	m_direction(ray.m_direction)
{
	transform.TransformPoint(m_origin);
	transform.TransformVector(m_direction);
	if(m_direction.Magnitude() > ruEPSILON)
	{
		m_direction.Normalize();	//<-- unnecessary?
	}
}

// ************************************************************************************************************************************************************

CRuAABB::CRuAABB()
{
}

CRuAABB::CRuAABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
:	m_minPoint(minX, minY, minZ),
	m_maxPoint(maxX, maxY, maxZ)
{
}

CRuAABB::CRuAABB(const CRuVector3 &minima, const CRuVector3 &maxima)
:	m_minPoint(minima),
	m_maxPoint(maxima)
{
}

CRuAABB::CRuAABB(const CRuAABB &box)
:	m_minPoint(box.m_minPoint),
	m_maxPoint(box.m_maxPoint)
{
}

CRuAABB::CRuAABB(const CRuAABB &box0, const CRuAABB &box1)
:	m_minPoint(min(box0.m_minPoint.x, box1.m_minPoint.x), min(box0.m_minPoint.y, box1.m_minPoint.y), min(box0.m_minPoint.z, box1.m_minPoint.z)),
	m_maxPoint(max(box0.m_maxPoint.x, box1.m_maxPoint.x), max(box0.m_maxPoint.y, box1.m_maxPoint.y), max(box0.m_maxPoint.z, box1.m_maxPoint.z))
{
}

CRuAABB::CRuAABB(const CRuOBB &box)
:	m_minPoint(-box.Extents()[0], -box.Extents()[1], -box.Extents()[2]),
	m_maxPoint( box.Extents()[0],  box.Extents()[1],  box.Extents()[2])
{
	// Build basis transform for the OBB
	CRuMatrix4x4 basisTransform;
	basisTransform.SetToBasis(box.Axis()[0], box.Axis()[1], box.Axis()[2]);

	// Transform AABB points
	this->Transform(basisTransform);

	// Offset to center position
	m_minPoint.m_x += box.Center()[0];
	m_minPoint.m_y += box.Center()[1];
	m_minPoint.m_z += box.Center()[2];
	m_maxPoint.m_x += box.Center()[0];
	m_maxPoint.m_y += box.Center()[1];
	m_maxPoint.m_z += box.Center()[2];
}

CRuAABB::~CRuAABB()
{
}

void CRuAABB::Set(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	m_minPoint.x = minX;	m_minPoint.y = minY;	m_minPoint.z = minZ;
	m_maxPoint.x = maxX;	m_maxPoint.y = maxY;	m_maxPoint.z = maxZ;
}

void CRuAABB::SetBounds(const CRuAABB &box)
{
	m_minPoint.x = box.m_minPoint.x;	m_minPoint.y = box.m_minPoint.y;	m_minPoint.z = box.m_minPoint.z;
	m_maxPoint.x = box.m_maxPoint.x;	m_maxPoint.y = box.m_maxPoint.y;	m_maxPoint.z = box.m_maxPoint.z;
}

void CRuAABB::SetBounds(const CRuVector3 &minima, const CRuVector3 &maxima)
{
	m_minPoint.x = minima.x;	m_minPoint.y = minima.y;	m_minPoint.z = minima.z;
	m_maxPoint.x = maxima.x;	m_maxPoint.y = maxima.y;	m_maxPoint.z = maxima.z;
}

void CRuAABB::SetBounds(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	m_minPoint.x = minX;	m_minPoint.y = minY;	m_minPoint.z = minZ;
	m_maxPoint.x = maxX;	m_maxPoint.y = maxY;	m_maxPoint.z = maxZ;
}

void CRuAABB::SetBoundsToQuadrantXZ(const CRuAABB &box, int quadIndex)
{
	float centerX = box.m_minPoint.x + (box.m_maxPoint.x - box.m_minPoint.x) / 2.0f;
	float centerZ = box.m_minPoint.z + (box.m_maxPoint.z - box.m_minPoint.z) / 2.0f;

	m_minPoint.y = box.m_minPoint.y;
	m_maxPoint.y = box.m_maxPoint.y;

	switch(quadIndex)
	{
		case 0:
			m_minPoint.x = box.m_minPoint.x;
			m_minPoint.z = box.m_minPoint.z;
			m_maxPoint.x = centerX;
			m_maxPoint.z = centerZ;
			break;
		case 1:
			m_minPoint.x = centerX;
			m_minPoint.z = box.m_minPoint.z;
			m_maxPoint.x = box.m_maxPoint.x;
			m_maxPoint.z = centerZ;
			break;
		case 2:
			m_minPoint.x = centerX;
			m_minPoint.z = centerZ;
			m_maxPoint.x = box.m_maxPoint.x;
			m_maxPoint.z = box.m_maxPoint.z;
			break;
		case 3:
			m_minPoint.x = box.m_minPoint.x;
			m_minPoint.z = centerZ;
			m_maxPoint.x = centerX;
			m_maxPoint.z = box.m_maxPoint.z;
			break;
		default:
			break;
	}
}

void CRuAABB::SetBoundsToOctant(const CRuAABB &box, int octantIndex)
{
	float centerX = box.m_minPoint.x + (box.m_maxPoint.x - box.m_minPoint.x) / 2.0f;
	float centerY = box.m_minPoint.y + (box.m_maxPoint.y - box.m_minPoint.y) / 2.0f;
	float centerZ = box.m_minPoint.z + (box.m_maxPoint.z - box.m_minPoint.z) / 2.0f;

	switch(octantIndex)
	{
		case 0:
			m_minPoint.x = box.m_minPoint.x;
			m_minPoint.y = box.m_minPoint.y;
			m_minPoint.z = box.m_minPoint.z;
			m_maxPoint.x = centerX;
			m_maxPoint.y = centerY;
			m_maxPoint.z = centerZ;
			break;
		case 1:
			m_minPoint.x = centerX;
			m_minPoint.y = box.m_minPoint.y;
			m_minPoint.z = box.m_minPoint.z;
			m_maxPoint.x = box.m_maxPoint.x;
			m_maxPoint.y = centerY;
			m_maxPoint.z = centerZ;
			break;
		case 2:
			m_minPoint.x = centerX;
			m_minPoint.y = box.m_minPoint.y;
			m_minPoint.z = centerZ;
			m_maxPoint.x = box.m_maxPoint.x;
			m_maxPoint.y = centerY;
			m_maxPoint.z = box.m_maxPoint.z;
			break;
		case 3:
			m_minPoint.x = box.m_minPoint.x;
			m_minPoint.y = box.m_minPoint.y;
			m_minPoint.z = centerZ;
			m_maxPoint.x = centerX;
			m_maxPoint.y = centerY;
			m_maxPoint.z = box.m_maxPoint.z;
			break;
		case 4:
			m_minPoint.x = box.m_minPoint.x;
			m_minPoint.y = centerY;
			m_minPoint.z = box.m_minPoint.z;
			m_maxPoint.x = centerX;
			m_maxPoint.y = box.m_maxPoint.y;
			m_maxPoint.z = centerZ;
			break;
		case 5:
			m_minPoint.x = centerX;
			m_minPoint.y = centerY;
			m_minPoint.z = box.m_minPoint.z;
			m_maxPoint.x = box.m_maxPoint.x;
			m_maxPoint.y = box.m_maxPoint.y;
			m_maxPoint.z = centerZ;
			break;
		case 6:
			m_minPoint.x = centerX;
			m_minPoint.y = centerY;
			m_minPoint.z = centerZ;
			m_maxPoint.x = box.m_maxPoint.x;
			m_maxPoint.y = box.m_maxPoint.y;
			m_maxPoint.z = box.m_maxPoint.z;
			break;
		case 7:
			m_minPoint.x = box.m_minPoint.x;
			m_minPoint.y = centerY;
			m_minPoint.z = centerZ;
			m_maxPoint.x = centerX;
			m_maxPoint.y = box.m_maxPoint.y;
			m_maxPoint.z = box.m_maxPoint.z;
			break;
		default:
			break;
	}
}

void CRuAABB::Merge(const CRuAABB &box)
{
	m_minPoint.x = min(m_minPoint.x, box.m_minPoint.x);
	m_minPoint.y = min(m_minPoint.y, box.m_minPoint.y);
	m_minPoint.z = min(m_minPoint.z, box.m_minPoint.z);
	m_maxPoint.x = max(m_maxPoint.x, box.m_maxPoint.x);
	m_maxPoint.y = max(m_maxPoint.y, box.m_maxPoint.y);
	m_maxPoint.z = max(m_maxPoint.z, box.m_maxPoint.z);
}

void CRuAABB::Merge(const CRuSphere &sphere)
{
	m_minPoint.x = min(m_minPoint.x, sphere.Center().x - sphere.Radius());
	m_minPoint.y = min(m_minPoint.y, sphere.Center().y - sphere.Radius());
	m_minPoint.z = min(m_minPoint.z, sphere.Center().z - sphere.Radius());
	m_maxPoint.x = max(m_maxPoint.x, sphere.Center().x + sphere.Radius());
	m_maxPoint.y = max(m_maxPoint.y, sphere.Center().y + sphere.Radius());
	m_maxPoint.z = max(m_maxPoint.z, sphere.Center().z + sphere.Radius());
}

void CRuAABB::Merge(const CRuVector3 &point)
{
	m_minPoint.x = min(m_minPoint.x, point.x);
	m_minPoint.y = min(m_minPoint.y, point.y);
	m_minPoint.z = min(m_minPoint.z, point.z);
	m_maxPoint.x = max(m_maxPoint.x, point.x);
	m_maxPoint.y = max(m_maxPoint.y, point.y);
	m_maxPoint.z = max(m_maxPoint.z, point.z);
}

void CRuAABB::Intersection(const CRuAABB &box)
{
	for(INT32 i = 0; i < 3; ++i)
	{
		if(box.m_maxPoint[i] >= m_maxPoint[i])
		{
			if(box.m_minPoint[i] <= m_minPoint[i])
			{
				// Do nothing - box range completely encloses our range
			}
			else if(box.m_minPoint[i] <= m_maxPoint[i])
			{
				m_minPoint[i] = box.m_minPoint[i];
			}
			else
			{
				// Range does not overlap at all
				m_minPoint[i] = m_maxPoint[i];
			}
		}
		else
		{
			m_maxPoint[i] = box.m_maxPoint[i];

			if(box.m_minPoint[i] <= m_minPoint[i])
			{
				// Do nothing
			}
			else
			{
				m_minPoint[i] = box.m_minPoint[i];
			}
		}
	}
}

void CRuAABB::Translate(const CRuVector3 &vec)
{
	Translate(vec.x, vec.y, vec.z);
}

void CRuAABB::Translate(float x, float y, float z)
{
	m_minPoint.x += x;		m_minPoint.y += y;		m_minPoint.z += z;
	m_maxPoint.x += x;		m_maxPoint.y += y;		m_maxPoint.z += z;
}

void CRuAABB::Transform(const CRuMatrix4x4 &mat)
{
	// Do a real transform of the bounding box..
	// Note... never ever continually transform a previously transformed
	// bounding box... especially if the transform contains rotations..
	// otherwise you're in for a world of joy as the box expands to
	// infinity.

	// 1. expand box to eight points, and transform all eight
	CRuVector3 extrema[8];

	extrema[0].Set(m_minPoint.x, m_minPoint.y, m_minPoint.z);
	extrema[1].Set(m_maxPoint.x, m_minPoint.y, m_minPoint.z);
	extrema[2].Set(m_minPoint.x, m_maxPoint.y, m_minPoint.z);
	extrema[3].Set(m_minPoint.x, m_minPoint.y, m_maxPoint.z);
	extrema[4].Set(m_maxPoint.x, m_maxPoint.y, m_minPoint.z);
	extrema[5].Set(m_minPoint.x, m_maxPoint.y, m_maxPoint.z);
	extrema[6].Set(m_maxPoint.x, m_minPoint.y, m_maxPoint.z);
	extrema[7].Set(m_maxPoint.x, m_maxPoint.y, m_maxPoint.z);

	for(INT32 i = 0; i < 8; ++i)
	{
		mat.TransformPoint(extrema[i]);
	}

	m_minPoint = extrema[0];
	m_maxPoint = extrema[0];

	for(INT32 i = 1; i < 8; ++i)
	{
		m_minPoint.x = min(m_minPoint.x, extrema[i].x);
		m_minPoint.y = min(m_minPoint.y, extrema[i].y);
		m_minPoint.z = min(m_minPoint.z, extrema[i].z);

		m_maxPoint.x = max(m_maxPoint.x, extrema[i].x);
		m_maxPoint.y = max(m_maxPoint.y, extrema[i].y);
		m_maxPoint.z = max(m_maxPoint.z, extrema[i].z);
	}
}

BOOL CRuAABB::IsCoincident(const CRuAABB &boxB) const
{
	if(	m_minPoint.x == boxB.m_minPoint.x && m_minPoint.y == boxB.m_minPoint.y && m_minPoint.z == boxB.m_minPoint.z &&
		m_maxPoint.x == boxB.m_maxPoint.x && m_maxPoint.y == boxB.m_maxPoint.y && m_maxPoint.z == boxB.m_maxPoint.z)
		return TRUE;

	return FALSE;
}

BOOL CRuAABB::Contains(const CRuAABB &boxB) const
{
	if(	m_minPoint.x <= boxB.m_minPoint.x && m_minPoint.y <= boxB.m_minPoint.y && m_minPoint.z <= boxB.m_minPoint.z &&
		m_maxPoint.x >= boxB.m_maxPoint.x && m_maxPoint.y >= boxB.m_maxPoint.y && m_maxPoint.z >= boxB.m_maxPoint.z)
		return TRUE;

	return FALSE;
}

BOOL CRuAABB::Contains(const CRuSphere &sphere) const
{
	if(	(sphere.Center().x - m_minPoint.x) >= sphere.Radius() &&
		(sphere.Center().y - m_minPoint.y) >= sphere.Radius() &&
		(sphere.Center().z - m_minPoint.z) >= sphere.Radius() &&
		(m_maxPoint.x - sphere.Center().x) >= sphere.Radius() &&
		(m_maxPoint.y - sphere.Center().y) >= sphere.Radius() &&
		(m_maxPoint.z - sphere.Center().z) >= sphere.Radius())
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CRuAABB::Contains(const CRuVector3 &pt) const
{
	if(m_minPoint.x <= pt.x && m_minPoint.y <= pt.y && m_minPoint.z <= pt.z &&
		m_maxPoint.x >= pt.x && m_maxPoint.y >= pt.y && m_maxPoint.z >= pt.z)
		return TRUE;

	return FALSE;
}

BOOL CRuAABB::Intersects(const CRuVector3 &origin, const CRuVector3 &direction) const
{
	BOOL inside = TRUE;
	int quadrant[3];		// 0 = larger than, 1 = smaller than, 2 = inside
	int i;
	int whichPlane;
	float maxT[3], candidatePlane[3];
	float intersectCoord[3];

	// Find candidate planes - this finds the three plane coordinates (orthogonal to each major axis) nearest the origin of the ray
	for(i = 0; i < 3; i++)
	{
		if(origin[i] < m_minPoint[i])
		{
			quadrant[i] = 1;
			candidatePlane[i] = m_minPoint[i];
			inside = FALSE;
		}
		else if(origin[i] > m_maxPoint[i])
		{
			quadrant[i] = 0;
			candidatePlane[i] = m_maxPoint[i];
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
		if(quadrant[i] != 2 && direction[i] != 0.0f)
			maxT[i] = (candidatePlane[i] - origin[i]) / direction[i];
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
			intersectCoord[i] = origin[i] + maxT[whichPlane] * direction[i];
			if(intersectCoord[i] < m_minPoint[i] || intersectCoord[i] > m_maxPoint[i])
				return FALSE;
		}
		else
		{
			intersectCoord[i] = candidatePlane[i];
		}
	}

	return TRUE;
}

BOOL CRuAABB::Intersects(const CRuVector3 &origin, const CRuVector3 &direction, float &tOut) const
{
	BOOL inside = TRUE;
	int quadrant[3];		// 0 = larger than, 1 = smaller than, 2 = inside
	int i;
	int whichPlane;
	float maxT[3], candidatePlane[3];
	float intersectCoord[3];

	// Find candidate planes - this finds the three plane coordinates (orthogonal to each major axis) nearest the origin of the ray
	for(i = 0; i < 3; i++)
	{
		if(origin[i] < m_minPoint[i])
		{
			quadrant[i] = 1;
			candidatePlane[i] = m_minPoint[i];
			inside = FALSE;
		}
		else if(origin[i] > m_maxPoint[i])
		{
			quadrant[i] = 0;
			candidatePlane[i] = m_maxPoint[i];
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
		if(quadrant[i] != 2 && direction[i] != 0.0f)
			maxT[i] = (candidatePlane[i] - origin[i]) / direction[i];
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
			intersectCoord[i] = origin[i] + maxT[whichPlane] * direction[i];
			if(intersectCoord[i] < m_minPoint[i] || intersectCoord[i] > m_maxPoint[i])
				return FALSE;
		}
		else
		{
			intersectCoord[i] = candidatePlane[i];
		}
	}

	tOut = maxT[whichPlane];

	return TRUE;
}

BOOL CRuAABB::Intersects(const CRuVector3 &origin, const CRuVector3 &direction, int &faceIDOut) const
{
	BOOL inside = TRUE;
	int quadrant[3];		// 0 = larger than, 1 = smaller than, 2 = inside
	int i;
	int whichPlane;
	float maxT[3], candidatePlane[3];
	float originVec[3], directionVec[3], minVec[3], maxVec[3], intersectCoord[3];

	// Initialize the origin/direction vectors into an array for our looping code
	originVec[0] = origin.x;			originVec[1] = origin.y;			originVec[2] = origin.z;
	directionVec[0] = direction.x;		directionVec[1] = direction.y;		directionVec[2] = direction.z;
	minVec[0] = m_minPoint.x;			minVec[1] = m_minPoint.y;			minVec[2] = m_minPoint.z;
	maxVec[0] = m_maxPoint.x;			maxVec[1] = m_maxPoint.y;			maxVec[2] = m_maxPoint.z;

	// Find candidate planes - this finds the three plane coordinates (orthogonal to each major axis) nearest the origin of the ray
	for(i = 0; i < 3; i++)
	{
		if(originVec[i] < minVec[i])
		{
			quadrant[i] = 1;
			candidatePlane[i] = minVec[i];
			inside = FALSE;
		}
		else if(originVec[i] > maxVec[i])
		{
			quadrant[i] = 0;
			candidatePlane[i] = maxVec[i];
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
		if(quadrant[i] != 2 && directionVec[i] != 0.0f)
			maxT[i] = (candidatePlane[i] - originVec[i]) / directionVec[i];
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
			intersectCoord[i] = originVec[i] + maxT[whichPlane] * directionVec[i];
			if(intersectCoord[i] < minVec[i] || intersectCoord[i] > maxVec[i])
				return FALSE;
		}
		else
		{
			intersectCoord[i] = candidatePlane[i];
		}
	}

	switch(whichPlane)
	{
		case 0:			// YZ Plane
			if(quadrant[whichPlane] == 0)
				faceIDOut = 3;
			else
				faceIDOut =	5;
			break;
		case 1:			// XZ Plane
			if(quadrant[whichPlane] == 0)
				faceIDOut = 1;
			else
				faceIDOut =	0;
			break;
		case 2:			// XY Plane
			if(quadrant[whichPlane] == 0)
				faceIDOut = 4;
			else
				faceIDOut =	2;
			break;
	}

	return TRUE;
}

BOOL CRuAABB::Intersects(const CRuVector3 &origin, const CRuVector3 &direction, CRuVector3 &intPointOut) const
{
	BOOL inside = TRUE;
	int quadrant[3];
	int i;
	int whichPlane;
	float maxT[3], candidatePlane[3];
	float originVec[3], directionVec[3], minVec[3], maxVec[3], intersectCoord[3];

	// Initialize the origin/direction vectors into an array for our looping code
	originVec[0] = origin.x;			originVec[1] = origin.y;			originVec[2] = origin.z;
	directionVec[0] = direction.x;		directionVec[1] = direction.y;		directionVec[2] = direction.z;
	minVec[0] = m_minPoint.x;			minVec[1] = m_minPoint.y;			minVec[2] = m_minPoint.z;
	maxVec[0] = m_maxPoint.x;			maxVec[1] = m_maxPoint.y;			maxVec[2] = m_maxPoint.z;

	// Find candidate planes
	for(i = 0; i < 3; i++)
	{
		if(originVec[i] < minVec[i])
		{
			quadrant[i] = 1;
			candidatePlane[i] = minVec[i];
			inside = FALSE;
		}
		else if(originVec[i] > maxVec[i])
		{
			quadrant[i] = 0;
			candidatePlane[i] = maxVec[i];
			inside = FALSE;
		}
		else
			quadrant[i] = 2;
	}

	// Ray is inside the bounding box
	if(inside)
	{
		intPointOut = origin;
		return TRUE;
	}

	// Calculate T distances to candidate planes
	for(i = 0; i < 3; i++)
	{
		if(quadrant[i] != 2 && directionVec[i] != 0.0f)
			maxT[i] = (candidatePlane[i] - originVec[i]) / directionVec[i];
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
			intersectCoord[i] = originVec[i] + maxT[whichPlane] * directionVec[i];
			if(intersectCoord[i] < minVec[i] || intersectCoord[i] > maxVec[i])
				return FALSE;
		}
		else
		{
			intersectCoord[i] = candidatePlane[i];
		}
	}

	intPointOut.x = intersectCoord[0];
	intPointOut.y = intersectCoord[1];
	intPointOut.z = intersectCoord[2];

	return TRUE;
}

BOOL CRuAABB::Intersects(const CRuVector3 &origin, const CRuVector3 &direction, CRuVector3 &intPointOut, float &tOut) const
{
	BOOL inside = TRUE;
	int quadrant[3];
	int i;
	int whichPlane;
	float maxT[3], candidatePlane[3];
	float originVec[3], directionVec[3], minVec[3], maxVec[3], intersectCoord[3];

	// Initialize the origin/direction vectors into an array for our looping code
	originVec[0] = origin.x;			originVec[1] = origin.y;			originVec[2] = origin.z;
	directionVec[0] = direction.x;		directionVec[1] = direction.y;		directionVec[2] = direction.z;
	minVec[0] = m_minPoint.x;			minVec[1] = m_minPoint.y;			minVec[2] = m_minPoint.z;
	maxVec[0] = m_maxPoint.x;			maxVec[1] = m_maxPoint.y;			maxVec[2] = m_maxPoint.z;

	// Find candidate planes
	for(i = 0; i < 3; i++)
	{
		if(originVec[i] < minVec[i])
		{
			quadrant[i] = 1;
			candidatePlane[i] = minVec[i];
			inside = FALSE;
		}
		else if(originVec[i] > maxVec[i])
		{
			quadrant[i] = 0;
			candidatePlane[i] = maxVec[i];
			inside = FALSE;
		}
		else
			quadrant[i] = 2;
	}

	// Ray is inside the bounding box
	if(inside)
	{
		intPointOut = origin;
		return TRUE;
	}

	// Calculate T distances to candidate planes
	for(i = 0; i < 3; i++)
	{
		if(quadrant[i] != 2 && directionVec[i] != 0.0f)
			maxT[i] = (candidatePlane[i] - originVec[i]) / directionVec[i];
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
			intersectCoord[i] = originVec[i] + maxT[whichPlane] * directionVec[i];
			if(intersectCoord[i] < minVec[i] || intersectCoord[i] > maxVec[i])
				return FALSE;
		}
		else
		{
			intersectCoord[i] = candidatePlane[i];
		}
	}

	intPointOut.x = intersectCoord[0];
	intPointOut.y = intersectCoord[1];
	intPointOut.z = intersectCoord[2];
	tOut = maxT[whichPlane];

	return TRUE;
}

BOOL CRuAABB::Intersects(const CRuAABB &boxB) const
{
	if(m_minPoint.x > boxB.m_maxPoint.x || boxB.m_minPoint.x > m_maxPoint.x)
		return FALSE;

	if(m_minPoint.y > boxB.m_maxPoint.y || boxB.m_minPoint.y > m_maxPoint.y)
		return FALSE;

	if(m_minPoint.z > boxB.m_maxPoint.z || boxB.m_minPoint.z > m_maxPoint.z)
		return FALSE;

	return TRUE;
}

BOOL CRuAABB::Intersects(const CRuAABB &boxB, RuOrthogonalPlane checkPlane) const
{
	switch(checkPlane)
	{
		case ruPLANE_XY:
			{
				if(m_minPoint.x > boxB.m_maxPoint.x || boxB.m_minPoint.x > m_maxPoint.x)
					return FALSE;

				if(m_minPoint.y > boxB.m_maxPoint.y || boxB.m_minPoint.y > m_maxPoint.y)
					return FALSE;

				return TRUE;
			}

		case ruPLANE_YZ:
			{
				if(m_minPoint.y > boxB.m_maxPoint.y || boxB.m_minPoint.y > m_maxPoint.y)
					return FALSE;

				if(m_minPoint.z > boxB.m_maxPoint.z || boxB.m_minPoint.z > m_maxPoint.z)
					return FALSE;

				return TRUE;
			}

		case ruPLANE_XZ:
			{
				if(m_minPoint.x > boxB.m_maxPoint.x || boxB.m_minPoint.x > m_maxPoint.x)
					return FALSE;

				if(m_minPoint.z > boxB.m_maxPoint.z || boxB.m_minPoint.z > m_maxPoint.z)
					return FALSE;

				return TRUE;
			}
			break;

		default:
			break;
	}

	return FALSE;
}

BOOL CRuAABB::Intersects(const CRuAABB &boxB, BOOL strict) const
{
	if(strict)
	{
		if(m_minPoint.x >= boxB.m_maxPoint.x || boxB.m_minPoint.x >= m_maxPoint.x)
			return FALSE;

		if(m_minPoint.y >= boxB.m_maxPoint.y || boxB.m_minPoint.y >= m_maxPoint.y)
			return FALSE;

		if(m_minPoint.z >= boxB.m_maxPoint.z || boxB.m_minPoint.z >= m_maxPoint.z)
			return FALSE;
	}
	else
	{
		if(m_minPoint.x > boxB.m_maxPoint.x || boxB.m_minPoint.x > m_maxPoint.x)
			return FALSE;

		if(m_minPoint.y > boxB.m_maxPoint.y || boxB.m_minPoint.y > m_maxPoint.y)
			return FALSE;

		if(m_minPoint.z > boxB.m_maxPoint.z || boxB.m_minPoint.z > m_maxPoint.z)
			return FALSE;
	}

	return TRUE;
}

BOOL CRuAABB::Intersects(const CRuAABB &boxB, RuOrthogonalPlane checkPlane, BOOL strict) const
{
	if(strict)
	{
		switch(checkPlane)
		{
			case ruPLANE_XY:
				{
					if(m_minPoint.x >= boxB.m_maxPoint.x || boxB.m_minPoint.x >= m_maxPoint.x)
						return FALSE;

					if(m_minPoint.y >= boxB.m_maxPoint.y || boxB.m_minPoint.y >= m_maxPoint.y)
						return FALSE;

					return TRUE;
				}

			case ruPLANE_YZ:
				{
					if(m_minPoint.y >= boxB.m_maxPoint.y || boxB.m_minPoint.y >= m_maxPoint.y)
						return FALSE;

					if(m_minPoint.z >= boxB.m_maxPoint.z || boxB.m_minPoint.z >= m_maxPoint.z)
						return FALSE;

					return TRUE;
				}

			case ruPLANE_XZ:
				{
					if(m_minPoint.x >= boxB.m_maxPoint.x || boxB.m_minPoint.x >= m_maxPoint.x)
						return FALSE;

					if(m_minPoint.z >= boxB.m_maxPoint.z || boxB.m_minPoint.z >= m_maxPoint.z)
						return FALSE;

					return TRUE;
				}
				break;

			default:
				break;
		}
	}
	else
	{
		switch(checkPlane)
		{
			case ruPLANE_XY:
				{
					if(m_minPoint.x > boxB.m_maxPoint.x || boxB.m_minPoint.x > m_maxPoint.x)
						return FALSE;

					if(m_minPoint.y > boxB.m_maxPoint.y || boxB.m_minPoint.y > m_maxPoint.y)
						return FALSE;

					return TRUE;
				}

			case ruPLANE_YZ:
				{
					if(m_minPoint.y > boxB.m_maxPoint.y || boxB.m_minPoint.y > m_maxPoint.y)
						return FALSE;

					if(m_minPoint.z > boxB.m_maxPoint.z || boxB.m_minPoint.z > m_maxPoint.z)
						return FALSE;

					return TRUE;
				}

			case ruPLANE_XZ:
				{
					if(m_minPoint.x > boxB.m_maxPoint.x || boxB.m_minPoint.x > m_maxPoint.x)
						return FALSE;

					if(m_minPoint.z > boxB.m_maxPoint.z || boxB.m_minPoint.z > m_maxPoint.z)
						return FALSE;

					return TRUE;
				}
				break;

			default:
				break;
		}
	}

	return FALSE;
}

/*======================== X-tests ========================*/
#define AXISTEST_X01(a, b, fa, fb)			   \
	p0 = a * v0.y - b * v0.z;			       	   \
	p2 = a * v2.y - b * v2.z;			       	   \
        if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
	rad = fa * boxHalfSize.y + fb * boxHalfSize.z;   \
	if(min>rad || max<-rad) return FALSE;

#define AXISTEST_X2(a, b, fa, fb)			   \
	p0 = a * v0.y - b * v0.z;			           \
	p1 = a * v1.y - b * v1.z;			       	   \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxHalfSize.y + fb * boxHalfSize.z;   \
	if(min>rad || max<-rad) return FALSE;

/*======================== Y-tests ========================*/
#define AXISTEST_Y02(a, b, fa, fb)			   \
	p0 = -a * v0.x + b * v0.z;		      	   \
	p2 = -a * v2.x + b * v2.z;	       	       	   \
        if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
	rad = fa * boxHalfSize.x + fb * boxHalfSize.z;   \
	if(min>rad || max<-rad) return FALSE;

#define AXISTEST_Y1(a, b, fa, fb)			   \
	p0 = -a * v0.x + b * v0.z;		      	   \
	p1 = -a * v1.x + b * v1.z;	     	       	   \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxHalfSize.x + fb * boxHalfSize.z;   \
	if(min>rad || max<-rad) return FALSE;

/*======================== Z-tests ========================*/

#define AXISTEST_Z12(a, b, fa, fb)			   \
	p1 = a * v1.x - b * v1.y;			           \
	p2 = a * v2.x - b * v2.y;			       	   \
        if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
	rad = fa * boxHalfSize.x + fb * boxHalfSize.y;   \
	if(min>rad || max<-rad) return FALSE;

#define AXISTEST_Z0(a, b, fa, fb)			   \
	p0 = a * v0.x - b * v0.y;				   \
	p1 = a * v1.x - b * v1.y;			           \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxHalfSize.x + fb * boxHalfSize.y;   \
	if(min>rad || max<-rad) return FALSE;

#define FINDMINMAX(x0,x1,x2,min,max) \
  min = max = x0;   \
  if(x1<min) min=x1;\
  if(x1>max) max=x1;\
  if(x2<min) min=x2;\
  if(x2>max) max=x2;


inline BOOL planeBoxOverlap(const CRuVector3 &normal, float d, const CRuVector3 &maxbox)
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

/// FIXME: This is a mess of multiple functions & macros -- clean it up!
BOOL CRuAABB::IntersectsTriangle(const CRuVector3 &tv0, const CRuVector3 &tv1, const CRuVector3 &tv2) const
{
	CRuVector3 boxCenter, boxHalfSize, v0, v1, v2, normal;
	CRuVector3 e0, e1, e2;				// Triangle edges
	float min, max, d, p0, p1, p2, rad, fex, fey, fez;  

	boxHalfSize = (m_maxPoint - m_minPoint) / 2.0f;
	boxCenter = m_minPoint + boxHalfSize;

	/* This is the fastest branch on Sun */
	/* move everything so that the boxcenter is in (0,0,0) */
	v0 = tv0 - boxCenter;
	v1 = tv1 - boxCenter;
	v2 = tv2 - boxCenter;

	/* compute triangle edges */
	e0 = v1 - v0;
	e1 = v2 - v1;
	e2 = v0 - v2;

	/* Bullet 3:  */
	/*  test the 9 tests first (this was faster) */
	fex = (float) fabs(e0.x);
	fey = (float) fabs(e0.y);
	fez = (float) fabs(e0.z);
	AXISTEST_X01(e0.z, e0.y, fez, fey);
	AXISTEST_Y02(e0.z, e0.x, fez, fex);
	AXISTEST_Z12(e0.y, e0.x, fey, fex);

	fex = (float) fabs(e1.x);
	fey = (float) fabs(e1.y);
	fez = (float) fabs(e1.z);
	AXISTEST_X01(e1.z, e1.y, fez, fey);
	AXISTEST_Y02(e1.z, e1.x, fez, fex);
	AXISTEST_Z0(e1.y, e1.x, fey, fex);

	fex = (float) fabs(e2.x);
	fey = (float) fabs(e2.y);
	fez = (float) fabs(e2.z);
	AXISTEST_X2(e2.z, e2.y, fez, fey);
	AXISTEST_Y1(e2.z, e2.x, fez, fex);
	AXISTEST_Z12(e2.y, e2.x, fey, fex);

	/* Bullet 1: */
	/*  first test overlap in the {x,y,z}-directions */
	/*  find min, max of the triangle each direction, and test for overlap in */
	/*  that direction -- this is equivalent to testing a minimal AABB around */
	/*  the triangle against the AABB */

	/* test in X-direction */
	FINDMINMAX(v0.x,v1.x,v2.x,min,max);
	if(min>boxHalfSize.x || max<-boxHalfSize.x) return FALSE;

	/* test in Y-direction */
	FINDMINMAX(v0.y,v1.y,v2.y,min,max);
	if(min>boxHalfSize.y || max<-boxHalfSize.y) return FALSE;

	/* test in Z-direction */
	FINDMINMAX(v0.z,v1.z,v2.z,min,max);
	if(min>boxHalfSize.z || max<-boxHalfSize.z) return FALSE;

	/* Bullet 2: */
	/*  test if the box intersects the plane of the triangle */
	/*  compute plane equation of triangle: normal*x+d=0 */
	normal = CrossProduct(e0, e1);
	d = -DotProduct(normal, v0);  /* plane eq: normal.x+d=0 */
	if(!planeBoxOverlap(normal, d, boxHalfSize)) return FALSE;

	return TRUE;   /* box and triangle overlaps */
}

BOOL CRuAABB::IntersectsFrustum(CRuPlane4 *planes) const
{
	CRuVector3 min;

	// Check each of the six planes
	for(int k = 0; k < 6; k++)
	{
		// Find the diagonal points which are most aligned with the plane normal
		if(planes[k].A >= 0.0f)
			min.x = m_maxPoint.x;
		else
			min.x = m_minPoint.x;

		if(planes[k].B >= 0.0f)
			min.y = m_maxPoint.y;
		else
			min.y = m_minPoint.y;

		if(planes[k].C >= 0.0f)
			min.z = m_maxPoint.z;
		else
			min.z = m_minPoint.z;

		if(planes[k].GetDistance(min) < 0.0f)
			return FALSE;
	}

	return TRUE;
}

BOOL CRuAABB::IntersectsFrustum(CRuPlane4 *planes, INT32 numPlanes) const
{
	CRuVector3 min;

	// Check each of the six planes
	for(int k = 0; k < numPlanes; k++)
	{
		// Find the diagonal points which are most aligned with the plane normal
		if(planes[k].A >= 0.0f)
			min.x = m_maxPoint.x;
		else
			min.x = m_minPoint.x;

		if(planes[k].B >= 0.0f)
			min.y = m_maxPoint.y;
		else
			min.y = m_minPoint.y;

		if(planes[k].C >= 0.0f)
			min.z = m_maxPoint.z;
		else
			min.z = m_minPoint.z;

		if(planes[k].GetDistance(min) < 0.0f)
			return FALSE;
	}

	return TRUE;
}

int CRuAABB::InsideFrustum(CRuPlane4 *planes) const
{
	CRuVector3 min, max;
	BOOL intersecting = FALSE;

	// Check each of the six planes
	for(int k = 0; k < 6; k++)
	{
		// Find the diagonal points which are most aligned with the plane normal
		if(planes[k].A >= 0.0f)
		{
			min.x = m_maxPoint.x;
			max.x = m_minPoint.x;
		}
		else
		{
			min.x = m_minPoint.x;
			max.x = m_maxPoint.x;
		}

		if(planes[k].B >= 0.0f)
		{
			min.y = m_maxPoint.y;
			max.y = m_minPoint.y;
		}
		else
		{
			min.y = m_minPoint.y;
			max.y = m_maxPoint.y;
		}

		if(planes[k].C >= 0.0f)
		{
			min.z = m_maxPoint.z;
			max.z = m_minPoint.z;
		}
		else
		{
			min.z = m_minPoint.z;
			max.z = m_maxPoint.z;
		}

		if(planes[k].GetDistance(min) < 0.0f)
			return 0;

		if(planes[k].GetDistance(max) <= 0.0f)
			intersecting = TRUE;
	}

	if(intersecting)
		return 1;

	return 2;
}

BOOL CRuAABB::IsSnappable(const CRuAABB &boxB, float &snapDistanceOut) const
{
	int snappingAxis = 0;
	int numOverlappingAxis = 3;

	// Make sure the boxes overlap on at least two axis
	if(m_minPoint.x > boxB.m_maxPoint.x || boxB.m_minPoint.x > m_maxPoint.x)
	{
		numOverlappingAxis--;
	}

	if(m_minPoint.y > boxB.m_maxPoint.y || boxB.m_minPoint.y > m_maxPoint.y)
	{
		snappingAxis = 1;
		numOverlappingAxis--;
	}

	if(m_minPoint.z > boxB.m_maxPoint.z || boxB.m_minPoint.z > m_maxPoint.z)
	{
		snappingAxis = 2;
		numOverlappingAxis--;
	}

	if(numOverlappingAxis != 2)
		return FALSE;

	float threshold = 5.0f;
	switch(snappingAxis)
	{
		case 0:
			snapDistanceOut = min((float) fabs(m_minPoint.x - boxB.m_maxPoint.x), (float) fabs(m_maxPoint.x - boxB.m_minPoint.x));
			if(snapDistanceOut < threshold)
				return TRUE;

			break;
		case 1:
			snapDistanceOut = min((float) fabs(m_minPoint.y - boxB.m_maxPoint.y), (float) fabs(m_maxPoint.y - boxB.m_minPoint.y));
			if(snapDistanceOut < threshold)
				return TRUE;

			break;
		case 2:
			snapDistanceOut = min((float) fabs(m_minPoint.z - boxB.m_maxPoint.z), (float) fabs(m_maxPoint.z - boxB.m_minPoint.z));
			if(snapDistanceOut < threshold)
				return TRUE;

			break;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

CRuOBB::CRuOBB()
{
	m_axis[0].Set(1.0f, 0.0f, 0.0f);
	m_axis[1].Set(0.0f, 1.0f, 0.0f);
	m_axis[2].Set(0.0f, 0.0f, 1.0f);
	m_extents[0] = 0.0f;
	m_extents[1] = 0.0f;
	m_extents[2] = 0.0f;
}

CRuOBB::CRuOBB(const CRuOBB &obb)
{
	m_center = obb.m_center;
	m_axis[0] = obb.m_axis[0];
	m_axis[1] = obb.m_axis[1];
	m_axis[2] = obb.m_axis[2];
	m_extents[0] = obb.m_extents[0];
	m_extents[1] = obb.m_extents[1];
	m_extents[2] = obb.m_extents[2];
}

CRuOBB::CRuOBB(const CRuVector3 &center, CRuVector3 extents[3])
{
	Set(center, extents);
}

CRuOBB::CRuOBB(const CRuVector3 &center, CRuVector3 axis[3], float extents[3])
{
	Set(center, axis, extents);
}

CRuOBB::CRuOBB(const CRuAABB &aabb)
{
	Set(aabb);
}

CRuOBB::CRuOBB(const CRuAABB &aabb, const CRuMatrix4x4 &transform)
{
	Set(aabb, transform);
}

CRuOBB::CRuOBB(const CRuSphere &sphere)
{
	Set(sphere);
}

CRuOBB::~CRuOBB()
{
}

void CRuOBB::Set(const CRuVector3 &center, CRuVector3 extents[3])
{
	m_center = center;

	m_axis[0] = extents[0];
	m_axis[1] = extents[1];
	m_axis[2] = extents[2];

	m_extents[0] = extents[0].Magnitude();
	m_extents[1] = extents[1].Magnitude();
	m_extents[2] = extents[2].Magnitude();

	m_axis[0].Normalize();
	m_axis[1].Normalize();
	m_axis[2].Normalize();
}

void CRuOBB::Set(const CRuVector3 &center, CRuVector3 axis[3], float extents[3])
{
	m_center = center;

	m_axis[0] = axis[0];
	m_axis[1] = axis[1];
	m_axis[2] = axis[2];

	m_extents[0] = extents[0];
	m_extents[1] = extents[1];
	m_extents[2] = extents[2];
}

void CRuOBB::Set(const CRuVector3 &center, const CRuVector3 &extents)
{
	m_center = center;

	m_axis[0].Set(1.0f, 0.0f, 0.0f);
	m_axis[1].Set(0.0f, 1.0f, 0.0f);
	m_axis[2].Set(0.0f, 0.0f, 1.0f);

	m_extents[0] = extents.m_x;
	m_extents[1] = extents.m_y;
	m_extents[2] = extents.m_z;
}

void CRuOBB::Set(const CRuAABB &aabb)
{
	m_center = (aabb.GetMinPoint() + aabb.GetMaxPoint()) / 2.0f;

	m_axis[0].Set(1.0f, 0.0f, 0.0f);
	m_axis[1].Set(0.0f, 1.0f, 0.0f);
	m_axis[2].Set(0.0f, 0.0f, 1.0f);

	m_extents[0] = aabb.GetXSize() / 2.0f;
	m_extents[1] = aabb.GetYSize() / 2.0f;
	m_extents[2] = aabb.GetZSize() / 2.0f;
}

void CRuOBB::Set(const CRuAABB &aabb, const CRuMatrix4x4 &transform)
{
	m_center = (aabb.GetMinPoint() + aabb.GetMaxPoint()) / 2.0f;

	m_axis[0].Set(1.0f, 0.0f, 0.0f);
	m_axis[1].Set(0.0f, 1.0f, 0.0f);
	m_axis[2].Set(0.0f, 0.0f, 1.0f);

	m_extents[0] = aabb.GetXSize() / 2.0f;
	m_extents[1] = aabb.GetYSize() / 2.0f;
	m_extents[2] = aabb.GetZSize() / 2.0f;

	Transform(transform);
}

void CRuOBB::Set(const CRuSphere &sphere)
{
	m_center = sphere.Center();

	m_axis[0].Set(1.0f, 0.0f, 0.0f);
	m_axis[1].Set(0.0f, 1.0f, 0.0f);
	m_axis[2].Set(0.0f, 0.0f, 1.0f);

	m_extents[0] = sphere.Radius();
	m_extents[1] = sphere.Radius();
	m_extents[2] = sphere.Radius();
}

void CRuOBB::Transform(const CRuMatrix4x4 &transform)
{
	// Assume local-to-world transform

	// Transform center
	m_center = m_center * transform;

	// Transform orienting axis set
	transform.TransformVector(m_axis[0]);
	transform.TransformVector(m_axis[1]);
	transform.TransformVector(m_axis[2]);

	for(INT32 i = 0; i < 3; ++i)
	{
		if(m_axis[i].SquaredMagnitude() > ruEPSILON)
		{
			m_extents[i] = m_extents[i] * m_axis[i].Magnitude();
			m_axis[i].Normalize();
		}
	}
}

BOOL CRuOBB::Contains(const CRuVector3 &pt) const
{
	CRuVector3 d = pt - m_center;
	CRuVector3 q = m_center;

	for(INT32 i = 0; i < 3; ++i)
	{
		REAL dist = DotProduct(d, m_axis[i]);

		// If distance is farther than the box extents, clamp to the box
		if(dist > m_extents[i])
		{
			dist = m_extents[i];
		}

		if(dist < -m_extents[i])
		{
			dist = -m_extents[i];
		}

		// Step that distance along the axis to get world coordinate
		q = q + m_axis[i] * dist;
	}

	CRuVector3 v = q - pt;

	return DotProduct(v, v) <= 0.0f;
}

BOOL CRuOBB::Intersects(const CRuOBB &obb) const
{
	// WARNING: NOT YET TESTED
	// Fairly standard separation of axis theorem implementation

	// Cutoff for cosine of angles between box axes.  This is used to catch
	// the cases when at least one pair of axes are parallel.  If this happens,
	// there is no need to test for separation along the Cross(A[i],B[j])
	// directions.
	const float fCutoff = 0.999999f;
	BOOL parallelPairExists = FALSE;
	int i;

	// convenience variables
	const CRuVector3 *axisA = m_axis;
	const CRuVector3 *axisB = obb.m_axis;
	const float *extentsA = m_extents;
	const float *extentsB = obb.m_extents;

	// compute difference of box centers, D = C1-C0
	CRuVector3 diff = obb.m_center - m_center;

	float matrixC[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
	float absC[3][3];  // |c_{ij}|
	float ad[3];        // Dot(A_i,D)
	float r0, r1, r;   // interval radii and distance between centers
	float r01;           // = R0 + R1

	// axis C0 + t * A0
	for(i = 0; i < 3; i++)
	{
		matrixC[0][i] = axisA[0].Dot(axisB[i]);
		absC[0][i] = (float) fabs(matrixC[0][i]);
		if(absC[0][i] > fCutoff)
			parallelPairExists = TRUE;
	}
	ad[0] = axisA[0].Dot(diff);
	r = (float) fabs(ad[0]);
	r1 = extentsB[0] * absC[0][0] + extentsB[1] * absC[0][1] + extentsB[2] * absC[0][2];
	r01 = extentsA[0] + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A1
	for(i = 0; i < 3; i++)
	{
		matrixC[1][i] = axisA[1].Dot(axisB[i]);
		absC[1][i] = (float) fabs(matrixC[1][i]);
		if(absC[1][i] > fCutoff)
			parallelPairExists = TRUE;
	}
	ad[1] = axisA[1].Dot(diff);
	r = (float) fabs(ad[1]);
	r1 = extentsB[0] * absC[1][0] + extentsB[1] * absC[1][1] + extentsB[2] * absC[1][2];
	r01 = extentsA[1] + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A2
	for(i = 0; i < 3; i++)
	{
		matrixC[2][i] = axisA[2].Dot(axisB[i]);
		absC[2][i] = (float) fabs(matrixC[2][i]);
		if(absC[2][i] > fCutoff)
			parallelPairExists = TRUE;
	}
	ad[2] = axisA[2].Dot(diff);
	r = (float) fabs(ad[2]);
	r1 = extentsB[0] * absC[2][0] + extentsB[1] * absC[2][1] + extentsB[2] * absC[2][2];
	r01 = extentsA[2] + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * B0
	r = (float) fabs(axisB[0].Dot(diff));
	r0 = extentsA[0] * absC[0][0] + extentsA[1] * absC[1][0] + extentsA[2] * absC[2][0];
	r01 = r0 + extentsB[0];
	if(r > r01)
		return FALSE;

	// axis C0 + t * B1
	r = (float) fabs(axisB[1].Dot(diff));
	r0 = extentsA[0] * absC[0][1] + extentsA[1] * absC[1][1] + extentsA[2] * absC[2][1];
	r01 = r0 + extentsB[1];
	if(r > r01)
		return FALSE;

	// axis C0 + t * B2
	r = (float) fabs(axisB[2].Dot(diff));
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
	r = (float) fabs(ad[2] * matrixC[1][0] - ad[1] * matrixC[2][0]);
	r0 = extentsA[1] * absC[2][0] + extentsA[2] * absC[1][0];
	r1 = extentsB[1] * absC[0][2] + extentsB[2] * absC[0][1];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A0xB1
	r = (float) fabs(ad[2] * matrixC[1][1] - ad[1] * matrixC[2][1]);
	r0 = extentsA[1] * absC[2][1] + extentsA[2] * absC[1][1];
	r1 = extentsB[0] * absC[0][2] + extentsB[2] * absC[0][0];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A0xB2
	r = (float) fabs(ad[2] * matrixC[1][2] - ad[1] * matrixC[2][2]);
	r0 = extentsA[1] * absC[2][2] + extentsA[2] * absC[1][2];
	r1 = extentsB[0] * absC[0][1] + extentsB[1] * absC[0][0];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A1xB0
	r = (float) fabs(ad[0] * matrixC[2][0] - ad[2] * matrixC[0][0]);
	r0 = extentsA[0] * absC[2][0] + extentsA[2] * absC[0][0];
	r1 = extentsB[1] * absC[1][2] + extentsB[2] * absC[1][1];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A1xB1
	r = (float) fabs(ad[0] * matrixC[2][1] - ad[2] * matrixC[0][1]);
	r0 = extentsA[0] * absC[2][1] + extentsA[2] * absC[0][1];
	r1 = extentsB[0] * absC[1][2] + extentsB[2] * absC[1][0];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A1xB2
	r = (float) fabs(ad[0] * matrixC[2][2] - ad[2] * matrixC[0][2]);
	r0 = extentsA[0] * absC[2][2] + extentsA[2] * absC[0][2];
	r1 = extentsB[0] * absC[1][1] + extentsB[1] * absC[1][0];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A2xB0
	r = (float) fabs(ad[1] * matrixC[0][0] - ad[0] * matrixC[1][0]);
	r0 = extentsA[0] * absC[1][0] + extentsA[1] * absC[0][0];
	r1 = extentsB[1] * absC[2][2] + extentsB[2] * absC[2][1];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A2xB1
	r = (float) fabs(ad[1] * matrixC[0][1] - ad[0] * matrixC[1][1]);
	r0 = extentsA[0] * absC[1][1] + extentsA[1] * absC[0][1];
	r1 = extentsB[0] * absC[2][2] + extentsB[2] * absC[2][0];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	// axis C0 + t * A2xB2
	r = (float) fabs(ad[1] * matrixC[0][2] - ad[0] * matrixC[1][2]);
	r0 = extentsA[0] * absC[1][2] + extentsA[1] * absC[0][2];
	r1 = extentsB[0] * absC[2][1] + extentsB[1] * absC[2][0];
	r01 = r0 + r1;
	if(r > r01)
		return FALSE;

	return TRUE;
}

BOOL CRuOBB::Intersects(const CRuVector3 &origin, const CRuVector3 &direction) const
{
	float wDu[3], awDu[3], dDu[3], adDu[3], awXdDu[3], rhs;

	CRuVector3 diff = origin - m_center;

	wDu[0] = direction.Dot(m_axis[0]);
	awDu[0] = (float) fabs(wDu[0]);
	dDu[0] = diff.Dot(m_axis[0]);
	adDu[0] = (float) fabs(dDu[0]);
	if(adDu[0] > m_extents[0] && dDu[0] * wDu[0] >= 0.0f)
		return FALSE;

	wDu[1] = direction.Dot(m_axis[1]);
	awDu[1] = (float) fabs(wDu[1]);
	dDu[1] = diff.Dot(m_axis[1]);
	adDu[1] = (float) fabs(dDu[1]);
	if(adDu[1] > m_extents[1] && dDu[1] * wDu[1] >= 0.0f)
		return FALSE;

	wDu[2] = direction.Dot(m_axis[2]);
	awDu[2] = (float) fabs(wDu[2]);
	dDu[2] = diff.Dot(m_axis[2]);
	adDu[2] = (float) fabs(dDu[2]);
	if(adDu[2] > m_extents[2] && dDu[2] * wDu[2] >= 0.0f)
		return FALSE;

	CRuVector3 wXd = CrossProduct(direction, diff);

	awXdDu[0] = (float) fabs(wXd.Dot(m_axis[0]));
	rhs = m_extents[1]*awDu[2] + m_extents[2]*awDu[1];
	if(awXdDu[0] > rhs)
		return FALSE;

	awXdDu[1] = (float) fabs(wXd.Dot(m_axis[1]));
	rhs = m_extents[0]*awDu[2] + m_extents[2]*awDu[0];
	if(awXdDu[1] > rhs)
		return FALSE;

	awXdDu[2] = (float) fabs(wXd.Dot(m_axis[2]));
	rhs = m_extents[0]*awDu[1] + m_extents[1]*awDu[0];
	if(awXdDu[2] > rhs)
		return FALSE;

	return TRUE;
}

BOOL CRuOBB::Intersects(const CRuVector3 &origin, const CRuVector3 &direction, CRuVector3 &intPointOut) const
{
	// WARNING: NOT YET TESTED

	// Convert ray to box coordinates
	CRuVector3 kDiff = origin - m_center;
	CRuVector3 localOrigin(kDiff.Dot(m_axis[0]), kDiff.Dot(m_axis[1]), kDiff.Dot(m_axis[2]));
	CRuVector3 localDirection(direction.Dot(m_axis[0]), direction.Dot(m_axis[1]), direction.Dot(m_axis[2]));

	// Return the nearer intersection only
	float t0 = 0.0f, t1 = FLT_MAX;
	if(FindIntersection(localOrigin, localDirection, m_extents, t0, t1))
	{
		if(t0 > 0.0f)
		{
			if(t0 < t1)
				intPointOut = origin + direction * t0;
			else
				intPointOut = origin + direction * t1;
		}
		else  // t0 == 0
		{
			intPointOut = origin + direction * t1;
		}

		return TRUE;
	}

	return FALSE;
}

CRuVector3 CRuOBB::GetClosestPoint(const CRuVector3 &pt)
{
	CRuVector3 q;
	CRuVector3 d = pt - m_center;

	// Start result at center of box; make steps from there
	q = m_center;

	// For each OBB axis...
	for(INT32 i = 0; i < 3; ++i)
	{
		// Project d onto that axis to get the distance along the axis of d from the box center
		REAL dist = DotProduct(d, m_axis[i]);

		// If distance farther than the box extents, clamp to the box
		if(dist > m_extents[i])
		{
			dist = m_extents[i];
		}

		if(dist < -m_extents[i])
		{
			dist = -m_extents[i];
		}

		// Step that distance along the axis to get world coordinate
		q = q + m_axis[i] * dist;
	}

	return q;
}

REAL CRuOBB::GetDistance(const CRuVector3 &pt)
{
	return (pt - GetClosestPoint(pt)).Magnitude();
}

void CRuOBB::ComputeBestFitOBB(CRuVector3 *points, INT32 numPoints)
{
}

// computes the OBB for this set of points relative to this transform matrix.
CRuOBB CRuOBB::ComputeOBB(CRuVector3 *points, INT32 numPoints, CRuMatrix4x4 *matrix)
{
	CRuVector3 minima(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	CRuVector3 maxima(FLT_MAX, FLT_MAX, FLT_MAX);

	CRuMatrix4x4 inverseMatrix = matrix->GetInverse();

	for(INT32 i = 0; i < numPoints; ++i)
	{
		CRuVector3 &p = points[i];
		CRuVector3 t;

		// Transform point by inverse of the matrix
		inverseMatrix.TransformPoint(p, t);

		// Update box bounds
		for(INT32 j = 0; j < 3; ++j)
		{
			minima[j] = min(minima[j], t[j]);
			maxima[j] = max(maxima[j], t[j]);
		}
	}

	CRuOBB computedOBB;

	for(INT32 i = 0; i < 3; ++i)
	{
		computedOBB.Extents()[i] = (maxima[i] - minima[i]) * 0.5f;
		computedOBB.Center()[i] = minima[i] + computedOBB.Extents()[i];
	}

	computedOBB.Transform(*matrix);

	return computedOBB;
}

void CRuOBB::GetVertices(CRuVector3 vertices[8]) const
{
	CRuVector3 extentAxis[3];

	extentAxis[0] = m_axis[0] * m_extents[0];
	extentAxis[1] = m_axis[1] * m_extents[1];
	extentAxis[2] = m_axis[2] * m_extents[2];

	vertices[0] = m_center - extentAxis[0] - extentAxis[1] - extentAxis[2];
	vertices[1] = m_center + extentAxis[0] - extentAxis[1] - extentAxis[2];
	vertices[2] = m_center + extentAxis[0] + extentAxis[1] - extentAxis[2];
	vertices[3] = m_center - extentAxis[0] + extentAxis[1] - extentAxis[2];
	vertices[4] = m_center - extentAxis[0] - extentAxis[1] + extentAxis[2];
	vertices[5] = m_center + extentAxis[0] - extentAxis[1] + extentAxis[2];
	vertices[6] = m_center + extentAxis[0] + extentAxis[1] + extentAxis[2];
	vertices[7] = m_center - extentAxis[0] + extentAxis[1] + extentAxis[2];
}

BOOL CRuOBB::Clip(float denominator, float numerator, float &t0, float &t1)
{
	// Return value is 'TRUE' if line segment intersects the current test
	// plane.  Otherwise 'FALSE' is returned in which case the line segment
	// is entirely clipped.

	if(denominator > 0.0f)
	{
		if(numerator > denominator * t1)
			return FALSE;
		if(numerator > denominator * t0)
			t0 = numerator / denominator;
		return TRUE;
	}
	else if(denominator < 0.0f)
	{
		if(numerator > denominator * t0)
			return FALSE;
		if(numerator > denominator * t1)
			t1 = numerator / denominator;

		return TRUE;
	}
	else
	{
		return numerator <= 0.0f;
	}
}

BOOL CRuOBB::FindIntersection(const CRuVector3 &origin, const CRuVector3 &direction, const float extents[3], float &t0, float &t1)
{
	float oldT0 = t0, oldT1 = t1;

	BOOL notEntirelyClipped =
		Clip( direction.x, -origin.x - extents[0], t0, t1) &&
		Clip(-direction.x,  origin.x - extents[0], t0, t1) &&
		Clip( direction.y, -origin.y - extents[1], t0, t1) &&
		Clip(-direction.y,  origin.y - extents[1], t0, t1) &&
		Clip( direction.z, -origin.z - extents[2], t0, t1) &&
		Clip(-direction.z,  origin.z - extents[2], t0, t1);

	return notEntirelyClipped && (t0 != oldT0 || t1 != oldT1);
}

// ************************************************************************************************************************************************************

CRuCapsule::CRuCapsule()
: m_segment(), m_radius(0.0f)
{
}

CRuCapsule::CRuCapsule(const CRuCapsule &capsule)
: m_segment(capsule.Segment()), m_radius(capsule.Radius())
{
}

CRuCapsule::CRuCapsule(const CRuSegment3 &segment, const float radius)
: m_segment(segment), m_radius(radius)
{
}

void CRuCapsule::Set(const CRuCapsule &capsule)
{
	m_segment.Set(capsule.Segment());
	m_radius = capsule.Radius();
}

void CRuCapsule::Set(const CRuSegment3 &segment, const float radius)
{
	m_segment.Set(segment);
	m_radius = radius;
}

BOOL CRuCapsule::Intersects(const CRuSegment3 &segment) const
{
	return m_segment.GetSquareDistance(segment) <= (m_radius * m_radius);
}

// ************************************************************************************************************************************************************

CRuCylinder::CRuCylinder()
: m_segment(), m_radius(0.0f)
{
}

CRuCylinder::CRuCylinder(const CRuCylinder &cylinder)
: m_segment(cylinder.Segment()), m_radius(cylinder.Radius())
{
}

CRuCylinder::CRuCylinder(const CRuSegment3 &segment, const float radius)
: m_segment(segment), m_radius(radius)
{
}

void CRuCylinder::Set(const CRuCylinder &cylinder)
{
	m_segment.Set(cylinder.Segment());
	m_radius = cylinder.Radius();
}

void CRuCylinder::Set(const CRuSegment3 &segment, const float radius)
{
	m_segment.Set(segment);
	m_radius = radius;
}

// ************************************************************************************************************************************************************

CRuSphere::CRuSphere()
: m_center(0.0f, 0.0f, 0.0f), m_radius(0.0f)
{
}

CRuSphere::CRuSphere(const CRuSphere &sphere)
{
	m_center = sphere.m_center;
	m_radius = sphere.m_radius;
}

CRuSphere::CRuSphere(const CRuVector3 &center, const float radius)
{
	m_center = center;
	m_radius = radius;
}

CRuSphere::~CRuSphere()
{
}

void CRuSphere::Set(const CRuSphere &sphere)
{
	m_center = sphere.m_center;
	m_radius = sphere.m_radius;
}

void CRuSphere::Set(const CRuVector3 &center, const float radius)
{
	m_center = center;
	m_radius = radius;
}

void CRuSphere::Merge(const CRuVector3 &center, float radius)
{
	// Calculate distance between two spheres
	float sqrDist = (m_center - center).SquaredMagnitude();

	if(m_radius > radius)
	{
		// Check for containment
		if(sqrDist <= (m_radius - radius) * (m_radius - radius))
		{
			return;
		}

		m_center = (m_center + center) / 2.0f;
		m_radius = m_radius + RuSqrt(sqrDist) / 2.0f;
	}
	else
	{
		// Check for containment
		if(sqrDist <= (radius - m_radius) * (radius - m_radius))
		{
			m_center = center;
			m_radius = radius;

			return;
		}

		m_center = (m_center + center) / 2.0f;
		m_radius = radius + RuSqrt(sqrDist) / 2.0f;
	}
}

BOOL CRuSphere::Contains(const CRuVector3 &pt) const
{
	REAL sqrRadius = m_radius * m_radius;

	if((m_center - pt).SquaredMagnitude() <= sqrRadius)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CRuSphere::Contains(const CRuSphere &sphere) const
{
	if(m_radius < sphere.m_radius)
		return FALSE;

	CRuVector3 centerOffset = m_center - sphere.m_center;
	REAL testVal = m_radius - sphere.m_radius;
	testVal = testVal * testVal;

	if(testVal >= centerOffset.SquaredMagnitude())
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CRuSphere::Intersects(const CRuSegment3 &segment) const
{
	// set up quadratic Q(t) = a*t^2 + 2*b*t + c
	CRuVector3 diff = segment.Origin() - m_center;
	float a = segment.Direction().SquaredMagnitude();
	float b = diff.Dot(segment.Direction());
	float c = diff.SquaredMagnitude() - m_radius * m_radius;

	// no intersection if Q(t) has no float roots
	float t[2];
	float discriminant = b * b - a * c;
	if(discriminant < 0.0f)
	{
		return FALSE;
	}
	else if(discriminant > 0.0f)
	{
		float root = (float) RuSqrt(discriminant);
		float inverseA = 1.0f / a;
		t[0] = (-b - root) * inverseA;
		t[1] = (-b + root) * inverseA;

		// assert: t0 < t1 since A > 0

		if(t[0] > 1.0f || t[1] < 0.0f)
		{
			return FALSE;
		}
		else if(t[0] >= 0.0f)
		{
			if(t[1] > 1.0f)
			{
				return TRUE;
			}
			else
			{
				return TRUE;
			}
		}
		else  // t[1] >= 0
		{
			return TRUE;
		}
	}
	else
	{
		t[0] = -b / a;
		if(t[0] >= 0.0f && t[0] <= 1.0f)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	return FALSE;
}

BOOL CRuSphere::SweptIntersects(const CRuSegment3 &segment, const CRuVector3 &sweepRay) const
{
	// TO DO.  The parameter fTMax is not being used.  The value rfTFirst
	// should be compared to it to decide if the first time of contact is
	// within the specified time interval.
	// translation: sweep ray is not unit length, so we derive tmax from the ray length and check collision time...

	// check if initially intersecting
	if(Intersects(segment))
		return TRUE;

	// In this system, the line is of course stationary.  The sphere spans
	// a capsule, but instead we will "grow" the segment by the sphere radius
	// and shrink the sphere to its center.  The problem is now to detect
	// the first time the moving center intersects the capsule formed by
	// the line segment and sphere radius.

	CRuCapsule capsule(segment, m_radius);
	CRuSegment3 path;
	path.SetFromRay(m_center, sweepRay);

	if(!capsule.Intersects(path))
		return FALSE;

	// We now know the sphere will intersect the segment.  This can happen
	// either at a segment end point or at a segment interior point.

	return TRUE;
}

BOOL CRuSphere::SweptIntersectsTriangle(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, const CRuVector3 &sweepRay, float &tFirst) const
{
	float tMax = 1.0f;

	CRuVector3 triVertices[3] =
	{
		v0,
		v1,
		v2
	};

	CRuVector3 triEdges[3] = 
	{
		v1 - v0,
		v2 - v1,
		v0 - v2
	};

	// triangle normal
	CRuVector3 triNormal;
	triNormal.SetToCrossProduct(triEdges[1], triEdges[0]);

	// sphere center projection on triangle normal
	float normalDotCenter = triNormal.Dot(m_center);

	// Radius projected length in normal direction.  This defers the square
	// root to normalize triNormal until absolutely needed.
	float normRadiusSqr = triNormal.SquaredMagnitude() * m_radius * m_radius;

	// triangle projection on triangle normal
	float normalDotTri = triNormal.Dot(v0);

	// Distance from sphere to triangle along the normal
	float distance = normalDotCenter - normalDotTri;

	// normals for the plane formed by edge i and the triangle normal
	CRuVector3 edgeCrossNormal[3] =
	{
		CrossProduct(triEdges[0], triNormal),
		CrossProduct(triEdges[1], triNormal),
		CrossProduct(triEdges[2], triNormal)
	};

	CRuSegment3 segment;

	if(distance * distance <= normRadiusSqr)
	{
		// sphere currently intersects the plane of the triangle

		// see which edges the sphere center is inside/outside of
		BOOL inside[3];
		for(int i = 0; i < 3; i++)
		{
			inside[i] = (edgeCrossNormal[i].Dot(m_center) >= edgeCrossNormal[i].Dot(triVertices[i]));
		}

		if(inside[0])
		{
			if(inside[1])
			{
				if(inside[2])
				{
					// triangle inside sphere
					return FALSE;
				}
				else // !inside[2]
				{
					// potential intersection with edge 2
					segment.Origin() = triVertices[2];
					segment.Direction() = triEdges[2];
					return SweptIntersects(segment, sweepRay);
				}
			}
			else // !inside[1]
			{
				if(inside[2])
				{
					// potential intersection with edge 1
					segment.Origin() = triVertices[1];
					segment.Direction() = triEdges[1];
					return SweptIntersects(segment, sweepRay);
				}
				else // !inside[2]
				{
					// potential intersection with edges 1,2
					return FindTriSphrCoplanarIntersection(2, triVertices, edgeCrossNormal[2], triEdges[2], sweepRay, tFirst, tMax);
				}            
			}
		} 
		else // !inside[0]
		{
			if(inside[1])
			{
				if(inside[2])
				{
					// potential intersection with edge 0
					segment.Origin() = triVertices[0];
					segment.Direction() = triEdges[0];
					return SweptIntersects(segment, sweepRay);
				}
				else // !inside[2]
				{
					// potential intersection with edges 2,0
					return FindTriSphrCoplanarIntersection(0, triVertices, edgeCrossNormal[0], triEdges[0], sweepRay, tFirst, tMax);
				}
			}
			else // !inside[1]
			{
				if(inside[2])
				{
					// potential intersection with edges 0,1
					return FindTriSphrCoplanarIntersection(1, triVertices, edgeCrossNormal[1], triEdges[1], sweepRay, tFirst, tMax);
				}
				else // !inside[2]
				{
					// we should not get here
//					assert( FALSE);
					return FALSE;
				}            
			}
		}
	}
	else
	{
		// sphere does not currently intersect the plane of the triangle

		// Find point of intersection of the sphere and the triangle
		// plane.  Where this point occurs on the plane relative to the
		// triangle determines the potential kind of intersection.

		triNormal.Normalize();

		// Point on sphere we care about intersecting the triangle plane
		CRuVector3 spherePoint;

		// Which side of the triangle is the sphere on?
		if(normalDotCenter > normalDotTri)
		{
			// positive side
			if(sweepRay.Dot(triNormal) >= 0.0f)
			{
				// moving away, easy out
				return FALSE;
			}

			spherePoint = m_center - triNormal * m_radius;
		}
		else
		{
			// negative side
			if(sweepRay.Dot(triNormal) <= 0.0f)
			{
				// moving away, easy out
				return FALSE;
			}

			spherePoint = m_center + triNormal * m_radius;
		}

		// find intersection of velocity ray and triangle plane

		// project ray and plane onto the plane normal
		float fPlane = triNormal.Dot(triVertices[0]);
		float fPoint = triNormal.Dot(spherePoint);
		float fVel = triNormal.Dot(sweepRay);
		float fTime = (fPlane - fPoint)/fVel;

		// where this intersects
		CRuVector3 kIntrPoint = spherePoint + sweepRay * fTime;

		// see which edges this intersection point is inside/outside of
		BOOL inside[3];
		for(int i = 0; i < 3; i++)
			inside[i] = (edgeCrossNormal[i].Dot(kIntrPoint) >= edgeCrossNormal[i].Dot(triVertices[i]));

		if(inside[0])
		{
			if(inside[1])
			{
				if(inside[2])
				{
					// intersects face at time fTime

					if(fTime > tMax)
					{
						// intersection after tMax
						return FALSE;
					}
					else
					{
						tFirst = fTime;
						return TRUE;
					}
				}
				else // !inside[2]
				{
					// potential intersection with edge 2
					segment.Origin() = triVertices[2];
					segment.Direction() = triEdges[2];
					return SweptIntersects(segment, sweepRay);
				}
			}
			else // !inside[1]
			{
				if(inside[2])
				{
					// potential intersection with edge 1
					segment.Origin() = triVertices[1];
					segment.Direction() = triEdges[1];
					return SweptIntersects(segment, sweepRay);
				}
				else // !inside[2]
				{
					// potential intersection with vertex 2
					return FindSphereVertexIntersection(triVertices[2], sweepRay, tFirst, tMax);
				}            
			}
		} 
		else // !inside[0]
		{
			if(inside[1])
			{
				if(inside[2])
				{
					// potential intersection with edge 0
					segment.Origin() = triVertices[0];
					segment.Direction() = triEdges[0];
					return SweptIntersects(segment, sweepRay);
				}
				else // !inside[2]
				{
					// potential intersection with vertex 0
					return FindSphereVertexIntersection(triVertices[0], sweepRay, tFirst, tMax);
				}
			}
			else // !inside[1]
			{
				if(inside[2])
				{
					// potential intersection with vertex 1
					return FindSphereVertexIntersection(triVertices[1], sweepRay, tFirst, tMax);
				}
				else // !inside[2]
				{
					// we should not get here
//					assert( FALSE);
					return FALSE;
				}            
			}
		}
	}
}

BOOL CRuSphere::SweptIntersectsTriangleList(const CRuVector3 &sweepRay, float &tFirst, const CRuVector3 *vertices, const UINT16 *indices, UINT32 numTriangles) const
{
	float tMax = 1.0f;

	UINT32 numIndices = numTriangles * 3;
	for(UINT32 k = 0; k < numIndices; )
	{

		CRuVector3 triVertices[3] =
		{
			vertices[indices[k++]],
			vertices[indices[k++]],
			vertices[indices[k++]]
		};

		CRuVector3 triEdges[3] = 
		{
			triVertices[1] - triVertices[0],
			triVertices[2] - triVertices[1],
			triVertices[0] - triVertices[2]
		};

		// triangle normal
		CRuVector3 triNormal;
		triNormal.SetToCrossProduct(triEdges[1], triEdges[0]);

		// sphere center projection on triangle normal
		float normalDotCenter = triNormal.Dot(m_center);

		// Radius projected length in normal direction.  This defers the square
		// root to normalize triNormal until absolutely needed.
		float normRadiusSqr = triNormal.SquaredMagnitude() * m_radius * m_radius;

		// triangle projection on triangle normal
		float normalDotTri = triNormal.Dot(triVertices[0]);

		// Distance from sphere to triangle along the normal
		float distance = normalDotCenter - normalDotTri;

		// normals for the plane formed by edge i and the triangle normal
		CRuVector3 edgeCrossNormal[3] =
		{
			CrossProduct(triEdges[0], triNormal),
			CrossProduct(triEdges[1], triNormal),
			CrossProduct(triEdges[2], triNormal)
		};

		CRuSegment3 segment;

		if(distance * distance <= normRadiusSqr)
		{
			// sphere currently intersects the plane of the triangle

			// see which edges the sphere center is inside/outside of
			BOOL inside[3];
			for(int i = 0; i < 3; i++)
			{
				inside[i] = (edgeCrossNormal[i].Dot(m_center) >= edgeCrossNormal[i].Dot(triVertices[i]));
			}

			if(inside[0])
			{
				if(inside[1])
				{
					if(inside[2])
					{
						// triangle inside sphere
						continue;
					}
					else // !inside[2]
					{
						// potential intersection with edge 2
						segment.Origin() = triVertices[2];
						segment.Direction() = triEdges[2];
						if(SweptIntersects(segment, sweepRay))
							return TRUE;
					}
				}
				else // !inside[1]
				{
					if(inside[2])
					{
						// potential intersection with edge 1
						segment.Origin() = triVertices[1];
						segment.Direction() = triEdges[1];
						if(SweptIntersects(segment, sweepRay))
							return TRUE;
					}
					else // !inside[2]
					{
						// potential intersection with edges 1,2
						if(FindTriSphrCoplanarIntersection(2, triVertices, edgeCrossNormal[2], triEdges[2], sweepRay, tFirst, tMax))
							return TRUE;
					}            
				}
			} 
			else // !inside[0]
			{
				if(inside[1])
				{
					if(inside[2])
					{
						// potential intersection with edge 0
						segment.Origin() = triVertices[0];
						segment.Direction() = triEdges[0];
						if(SweptIntersects(segment, sweepRay))
							return TRUE;
					}
					else // !inside[2]
					{
						// potential intersection with edges 2,0
						if(FindTriSphrCoplanarIntersection(0, triVertices, edgeCrossNormal[0], triEdges[0], sweepRay, tFirst, tMax))
							return TRUE;
					}
				}
				else // !inside[1]
				{
					if(inside[2])
					{
						// potential intersection with edges 0,1
						if(FindTriSphrCoplanarIntersection(1, triVertices, edgeCrossNormal[1], triEdges[1], sweepRay, tFirst, tMax))
							return TRUE;
					}
					else // !inside[2]
					{
						// we should not get here
						continue;
					}            
				}
			}
		}
		else
		{
			// sphere does not currently intersect the plane of the triangle

			// Find point of intersection of the sphere and the triangle
			// plane.  Where this point occurs on the plane relative to the
			// triangle determines the potential kind of intersection.

			triNormal.Normalize();

			// Point on sphere we care about intersecting the triangle plane
			CRuVector3 spherePoint;

			// Which side of the triangle is the sphere on?
			if(normalDotCenter > normalDotTri)
			{
				// positive side
				if(sweepRay.Dot(triNormal) >= 0.0f)
				{
					// moving away, easy out
					continue;
				}

				spherePoint = m_center - triNormal * m_radius;
			}
			else
			{
				// negative side
				if(sweepRay.Dot(triNormal) <= 0.0f)
				{
					// moving away, easy out
					continue;
				}

				spherePoint = m_center + triNormal * m_radius;
			}

			// find intersection of velocity ray and triangle plane

			// project ray and plane onto the plane normal
			float fPlane = triNormal.Dot(triVertices[0]);
			float fPoint = triNormal.Dot(spherePoint);
			float fVel = triNormal.Dot(sweepRay);
			float fTime = (fPlane - fPoint)/fVel;

			// where this intersects
			CRuVector3 kIntrPoint = spherePoint + sweepRay * fTime;

			// see which edges this intersection point is inside/outside of
			BOOL inside[3];
			for(int i = 0; i < 3; i++)
				inside[i] = (edgeCrossNormal[i].Dot(kIntrPoint) >= edgeCrossNormal[i].Dot(triVertices[i]));

			if(inside[0])
			{
				if(inside[1])
				{
					if(inside[2])
					{
						// intersects face at time fTime

						if(fTime > tMax)
						{
							// intersection after tMax
							continue;
						}
						else
						{
							tFirst = fTime;
							return TRUE;
						}
					}
					else // !inside[2]
					{
						// potential intersection with edge 2
						segment.Origin() = triVertices[2];
						segment.Direction() = triEdges[2];
						if(SweptIntersects(segment, sweepRay))
							return TRUE;
					}
				}
				else // !inside[1]
				{
					if(inside[2])
					{
						// potential intersection with edge 1
						segment.Origin() = triVertices[1];
						segment.Direction() = triEdges[1];
						if(SweptIntersects(segment, sweepRay))
							return TRUE;
					}
					else // !inside[2]
					{
						// potential intersection with vertex 2
						if(FindSphereVertexIntersection(triVertices[2], sweepRay, tFirst, tMax))
							return TRUE;
					}            
				}
			} 
			else // !inside[0]
			{
				if(inside[1])
				{
					if(inside[2])
					{
						// potential intersection with edge 0
						segment.Origin() = triVertices[0];
						segment.Direction() = triEdges[0];
						if(SweptIntersects(segment, sweepRay))
							return TRUE;
					}
					else // !inside[2]
					{
						// potential intersection with vertex 0
						if(FindSphereVertexIntersection(triVertices[0], sweepRay, tFirst, tMax))
							return TRUE;
					}
				}
				else // !inside[1]
				{
					if(inside[2])
					{
						// potential intersection with vertex 1
						if(FindSphereVertexIntersection(triVertices[1], sweepRay, tFirst, tMax))
							return TRUE;
					}
					else // !inside[2]
					{
						// we should not get here
						continue;
					}            
				}
			}
		}

	}

	return FALSE;
}

BOOL CRuSphere::FindIntersection(const CRuSegment3 &segment, int &numIntersectionsOut, CRuVector3 intPointsOut[2]) const
{
	// set up quadratic Q(t) = a*t^2 + 2*b*t + c
	CRuVector3 diff = segment.Origin() - m_center;
	float a = segment.Direction().SquaredMagnitude();
	float b = diff.Dot(segment.Direction());
	float c = diff.SquaredMagnitude() - m_radius * m_radius;

	// no intersection if Q(t) has no float roots
	float t[2];
	float discriminant = b * b - a * c;
	if(discriminant < 0.0f)
	{
		numIntersectionsOut = 0;
		return FALSE;
	}
	else if(discriminant > 0.0f)
	{
		float root = (float) RuSqrt(discriminant);
		float inverseA = 1.0f / a;
		t[0] = (-b - root) * inverseA;
		t[1] = (-b + root) * inverseA;

		// assert: t0 < t1 since A > 0

		if(t[0] > 1.0f || t[1] < 0.0f)
		{
			numIntersectionsOut = 0;
			return FALSE;
		}
		else if(t[0] >= 0.0f)
		{
			if(t[1] > 1.0f)
			{
				numIntersectionsOut = 1;
				intPointsOut[0] = segment.Origin() + segment.Direction() * t[0];
				return TRUE;
			}
			else
			{
				numIntersectionsOut = 2;
				intPointsOut[0] = segment.Origin() + segment.Direction() * t[0];
				intPointsOut[1] = segment.Origin() + segment.Direction() * t[1];
				return TRUE;
			}
		}
		else  // t[1] >= 0
		{
			numIntersectionsOut = 1;
			intPointsOut[0] = segment.Origin() + segment.Direction() * t[1];
			return TRUE;
		}
	}
	else
	{
		t[0] = -b / a;
		if(t[0] >= 0.0f && t[0] <= 1.0f)
		{
			numIntersectionsOut = 1;
			intPointsOut[0] = segment.Origin() + segment.Direction() * t[0];
			return TRUE;
		}
		else
		{
			numIntersectionsOut = 0;
			return FALSE;
		}
	}

	return FALSE;
}

BOOL CRuSphere::FindTriSphrCoplanarIntersection(int vertIdx, const CRuVector3 triVertices[3], const CRuVector3 &sideNormal, const CRuVector3 &side, const CRuVector3 &sweepRay, float &rfTFirst, float fTMax) const
{
	// TO DO.  The parameter rkNormal is not used here.  Is this an error?
	// Or does the caller make some adjustments to the other inputs to
	// account for the normal?

	// vertIdx is the "hinge" vertex that the two potential edges that can
	// be intersected by the sphere connect to, and it indexes into triVertices.

	// sideNormal is the normal of the plane formed by (vertIdx,vertIdx+1)
	// and the tri norm, passed so as not to recalculate

	// check for intersections at time 0
	CRuVector3 kDist = triVertices[vertIdx] - m_center;
	if(kDist.SquaredMagnitude() < m_radius * m_radius)
	{
		// already intersecting that vertex
		rfTFirst = 0.0f;
		return FALSE;
	}

	// check for easy out
	if(sweepRay.Dot(kDist) <= 0.0f)
	{
		// moving away
		return FALSE;
	}

	// find intersection of velocity ray and side normal

	// project ray and plane onto the plane normal
	float fPlane = sideNormal.Dot(triVertices[vertIdx]);
	float fCenter = sideNormal.Dot(m_center);
	float fVel = sideNormal.Dot(sweepRay);
	float fFactor = (fPlane - fCenter) / fVel;

	CRuVector3 kPoint = m_center + sweepRay * fFactor;

	// now, find which side of the hinge vertex this lies by projecting
	// both the vertex and this new point onto the triangle edge (the same
	// edge whose "normal" was used to find this point)

	float fVertex = side.Dot(triVertices[vertIdx]);
	float fPoint = side.Dot(kPoint);

	CRuSegment3 kSeg;

	if(fPoint >= fVertex)
	{
		// intersection with edge (vertIdx,vertIdx+1)
		kSeg.Origin() = triVertices[vertIdx];
		kSeg.Direction() = triVertices[(vertIdx + 1 ) %3] - triVertices[vertIdx];
	}
	else
	{
		// intersection with edge (vertIdx-1,vertIdx)
		if(vertIdx != 0)
			kSeg.Origin() = triVertices[vertIdx - 1];
		else
			kSeg.Origin() = triVertices[2];
		kSeg.Direction() = triVertices[vertIdx] - kSeg.Origin();
	}

	// This could be either an sphere-edge or a sphere-vertex intersection
	// (this test isn't enough to differentiate), so use the full-on
	// line-sphere test.
	return SweptIntersects(kSeg, sweepRay);
}

BOOL CRuSphere::FindSphereVertexIntersection(const CRuVector3 &vertex, const CRuVector3 &sweepRay, float &rfTFirst, float fTMax) const
{
	// Finds the time and place (and possible occurance, it may miss) of an
	// intersection between a sphere of fRadius at rkOrigin moving in rkDir
	// towards a vertex at rkVertex.

	CRuVector3 kD = m_center - vertex;
	CRuVector3 kCross = kD.Cross(sweepRay);
	float fRSqr = m_radius * m_radius;
	float fVSqr = sweepRay.SquaredMagnitude();

	if(kCross.SquaredMagnitude() > fRSqr * fVSqr)
	{
		// ray overshoots the sphere
		return FALSE;
	}

	// find time of intersection
	float fDot = kD.Dot(sweepRay);
	float fDiff = kD.SquaredMagnitude() - fRSqr;
	float fInv = RuInvSqrt(fabs(fDot * fDot - fVSqr * fDiff));

	rfTFirst = fDiff * fInv / (1.0f - fDot * fInv);
	if(rfTFirst > fTMax)
	{
		// intersection after max time
		return FALSE;
	}

	// place of intersection is triangle vertex
	return TRUE;
}

// ************************************************************************************************************************************************************

CRuAAEllipsoid::CRuAAEllipsoid()
{
}

CRuAAEllipsoid::CRuAAEllipsoid(const CRuAAEllipsoid &ellipsoid)
{
	m_center = ellipsoid.m_center;
	m_radius = ellipsoid.m_radius;
}

CRuAAEllipsoid::CRuAAEllipsoid(const CRuVector3 &center, const CRuVector3 &radius)
{
	m_center = center;
	m_radius = radius;
}

void CRuAAEllipsoid::Set(const CRuAAEllipsoid &ellipsoid)
{
	m_center = ellipsoid.m_center;
	m_radius = ellipsoid.m_radius;
}

void CRuAAEllipsoid::Set(const CRuVector3 &center, const CRuVector3 &radius)
{
	m_center = center;
	m_radius = radius;
}

CRuSphere CRuAAEllipsoid::GetBoundingSphere() const
{
	return CRuSphere(m_center, max(max(m_radius.x, m_radius.y), m_radius.z));
}

// ************************************************************************************************************************************************************

CRuAABB2D::CRuAABB2D(const CRuAABB2D &aabb2D)
:	m_minima(aabb2D.m_minima),
	m_maxima(aabb2D.m_maxima)
{
}

CRuAABB2D::CRuAABB2D(const CRuVector2 &minima, const CRuVector2 &maxima)
:	m_minima(minima),
	m_maxima(maxima)
{
}

CRuAABB2D::~CRuAABB2D()
{
}

BOOL CRuAABB2D::GetEdge(INT32 edgeIndex, CRuSegment2 &edgeOut) const
{
	switch(edgeIndex)
	{
		case 0:
			{
				edgeOut.Origin() = m_minima;
				edgeOut.Direction().Set(m_maxima.m_x - m_minima.m_x, 0.0f);
			}

			break;

		case 1:
			{
				edgeOut.Origin() = m_maxima;
				edgeOut.Direction().Set(0.0f, m_minima.m_y - m_maxima.m_y);
			}

			break;

		case 2:
			{
				edgeOut.Origin() = m_maxima;
				edgeOut.Direction().Set(m_minima.m_x - m_maxima.m_x, 0.0f);
			}

			break;

		case 3:
			{
				edgeOut.Origin() = m_minima;
				edgeOut.Direction().Set(0.0f, m_maxima.m_y - m_minima.m_y);
			}

			break;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

// All internal minimal sphere calculations store the squared radius in the
// radius member of CRuSphere.  Only at the end is a sqrt computed.

CRuMinSphereBuilder::CRuMinSphereBuilder()
{
	m_epsilon = 0.001f;
}

BOOL CRuMinSphereBuilder::BuildSphere(const CRuVector3 *points, int numPoints, CRuSphere &minimalSphereOut)
{
	Support support;
	REAL distDiff;

	if(numPoints >= 1)
	{
		// Create identity permutation (0, 1, ..., numPoints - 1)
		CRuVector3 **permutation = ruNEW CRuVector3 * [numPoints];
		INT32 i;
		for(i = 0; i < numPoints; i++)
		{
			permutation[i] = (CRuVector3 *) &points[i];
		}

		// Generate random permutation
		for (i = numPoints - 1; i > 0; i--)
		{
			int j = rand() % (i + 1);
			if(j != i)
			{
				CRuVector3 *tempVector = permutation[i];
				permutation[i] = permutation[j];
				permutation[j] = tempVector;
			}
		}

		minimalSphereOut = ExactSphere1(*permutation[0]);
		support.Quantity = 1;
		support.Index[0] = 0;
		i = 1;
		while(i < numPoints)
		{
			if(!support.Contains(i, permutation, m_epsilon))
			{
				if(!Contains(*permutation[i], minimalSphereOut, distDiff))
				{
					CRuSphere sphere;

					switch(support.Quantity)
					{
						default:
						case 0:
							// Assert here
							break;
						case 1:
							sphere = UpdateSupport1(i, permutation, support);
							break;
						case 2:
							sphere = UpdateSupport2(i, permutation, support);
							break;
						case 3:
							sphere = UpdateSupport3(i, permutation, support);
							break;
						case 4:
							sphere = UpdateSupport4(i, permutation, support);
							break;
					}

					if(sphere.Radius() > minimalSphereOut.Radius())
					{
						minimalSphereOut = sphere;
						i = 0;
						continue;
					}
				}
			}

			i++;
		}

		delete[] permutation;
	}
	else
	{
		return FALSE;
	}

	minimalSphereOut.Radius() = RuSqrt(minimalSphereOut.Radius());

	return TRUE;
}

BOOL CRuMinSphereBuilder::Contains(const CRuVector3 &point, const CRuSphere &sphere, REAL &distDiff)
{
	CRuVector3 diff = point - sphere.Center();
	REAL test = diff.SquaredMagnitude();

	// NOTE:  In this algorithm, Sphere3 is storing the *squared radius*,
	// so the next line of code is not in error.
	distDiff = test - sphere.Radius();

	return distDiff <= 0.0f;
}

CRuSphere CRuMinSphereBuilder::ExactSphere1(const CRuVector3 &point)
{
	CRuSphere minimalSphere;

	minimalSphere.Center() = point;
	minimalSphere.Radius() = 0.0f;

	return minimalSphere;
}

CRuSphere CRuMinSphereBuilder::ExactSphere2(const CRuVector3 &point0, const CRuVector3 &point1)
{
	CRuSphere minimalSphere;
	minimalSphere.Center() = (point0 + point1) * 0.5f;

	CRuVector3 kDiff = point1 - point0;
	minimalSphere.Radius() = 0.25f * kDiff.SquaredMagnitude();

	return minimalSphere;
}

CRuSphere CRuMinSphereBuilder::ExactSphere3(const CRuVector3 &point0, const CRuVector3 &point1, const CRuVector3 &point2)
{
	// Compute the circle (in 3D) containing p0, p1, and p2.  The Center() in
	// barycentric coordinates is K = u0 * p0 + u1 * p1 + u2 * p2 where u0 + u1 + u2 = 1.
	// The Center() is equidistant from the three points, so |K-p0| = |K-p1| =
	// |K-p2| = R where R is the radius of the circle.
	//
	// From these conditions,
	//   K-p0 = u0*A + u1*B - A
	//   K-p1 = u0*A + u1*B - B
	//   K-p2 = u0*A + u1*B
	// where A = p0-p2 and B = p1-p2, which leads to
	//   r^2 = |u0*A+u1*B|^2 - 2*Dot(A,u0*A+u1*B) + |A|^2
	//   r^2 = |u0*A+u1*B|^2 - 2*Dot(B,u0*A+u1*B) + |B|^2
	//   r^2 = |u0*A+u1*B|^2
	// Subtracting the last equation from the first two and writing
	// the equations as a linear system,
	//
	// +-                 -++   -+       +-        -+
	// | Dot(A,A) Dot(A,B) || u0 | = 0.5 | Dot(A,A) |
	// | Dot(B,A) Dot(B,B) || u1 |       | Dot(B,B) |
	// +-                 -++   -+       +-        -+
	//
	// The following code solves this system for u0 and u1, then
	// evaluates the third equation in r^2 to obtain r.

	CRuVector3 a = point0 - point2;
	CRuVector3 b = point1 - point2;

	REAL aDa = a.Dot(a);
	REAL aDb = a.Dot(b);
	REAL bDb = b.Dot(b);

	// Determinant is equal to zero when the three points are collinear
	REAL det = aDa * bDb - aDb * aDb;

	CRuSphere minimalSphere;

	if(fabs(det) > m_epsilon)
	{
		REAL halfInvDet = 0.5f / det;
		REAL u0 = halfInvDet * bDb * (aDa - aDb);
		REAL u1 = halfInvDet * aDa * (bDb - aDb);
		REAL u2 = 1.0f - u0 - u1;

		minimalSphere.Center() = point0 * u0 + point1 * u1 + point2 * u2;

		CRuVector3 temp = a * u0 + b * u1;
		minimalSphere.Radius() = temp.SquaredMagnitude();
	}
	else
	{
		// A cop-out for now: Just calculate some sort of average sphere
		minimalSphere.Center().Set((point0 + point1 + point2) / 3.0f);
		CRuVector3 a0 = point0 - minimalSphere.Center();
		CRuVector3 a1 = point1 - minimalSphere.Center();
		CRuVector3 a2 = point2 - minimalSphere.Center();
		minimalSphere.Radius() = max(max(a0.SquaredMagnitude(), a1.SquaredMagnitude()), a2.SquaredMagnitude());
	}

	return minimalSphere;
}

CRuSphere CRuMinSphereBuilder::ExactSphere4(const CRuVector3 &point0, const CRuVector3 &point1, const CRuVector3 &point2, const CRuVector3 &point3)
{
	// Compute the sphere containing p0, p1, p2, and p3.  The Center() in
	// barycentric coordinates is K = u0*p0+u1*p1+u2*p2+u3*p3 where
	// u0+u1+u2+u3=1.  The Center() is equidistant from the three points, so
	// |K-p0| = |K-p1| = |K-p2| = |K-p3| = R where R is the radius of the
	// sphere.
	//
	// From these conditions,
	//   K-p0 = u0*A + u1*B + u2*C - A
	//   K-p1 = u0*A + u1*B + u2*C - B
	//   K-p2 = u0*A + u1*B + u2*C - C
	//   K-p3 = u0*A + u1*B + u2*C
	// where A = p0-p3, B = p1-p3, and C = p2-p3 which leads to
	//   r^2 = |u0*A+u1*B+u2*C|^2 - 2*Dot(A,u0*A+u1*B+u2*C) + |A|^2
	//   r^2 = |u0*A+u1*B+u2*C|^2 - 2*Dot(B,u0*A+u1*B+u2*C) + |B|^2
	//   r^2 = |u0*A+u1*B+u2*C|^2 - 2*Dot(C,u0*A+u1*B+u2*C) + |C|^2
	//   r^2 = |u0*A+u1*B+u2*C|^2
	// Subtracting the last equation from the first three and writing
	// the equations as a linear system,
	//
	// +-                          -++   -+       +-        -+
	// | Dot(A,A) Dot(A,B) Dot(A,C) || u0 | = 0.5 | Dot(A,A) |
	// | Dot(B,A) Dot(B,B) Dot(B,C) || u1 |       | Dot(B,B) |
	// | Dot(C,A) Dot(C,B) Dot(C,C) || u2 |       | Dot(C,C) |
	// +-                          -++   -+       +-        -+
	//
	// The following code solves this system for u0, u1, and u2, then
	// evaluates the fourth equation in r^2 to obtain r.

	CRuVector3 e10 = point0 - point3;
	CRuVector3 e20 = point1 - point3;
	CRuVector3 e30 = point2 - point3;

	REAL aafA[3][3];
	aafA[0][0] = e10.Dot(e10);
	aafA[0][1] = e10.Dot(e20);
	aafA[0][2] = e10.Dot(e30);
	aafA[1][0] = aafA[0][1];
	aafA[1][1] = e20.Dot(e20);
	aafA[1][2] = e20.Dot(e30);
	aafA[2][0] = aafA[0][2];
	aafA[2][1] = aafA[1][2];
	aafA[2][2] = e30.Dot(e30);

	REAL afB[3];
	afB[0] = 0.5f * aafA[0][0];
	afB[1] = 0.5f * aafA[1][1];
	afB[2] = 0.5f * aafA[2][2];

	REAL aafAInv[3][3];
	aafAInv[0][0] = aafA[1][1] * aafA[2][2] - aafA[1][2] * aafA[2][1];
	aafAInv[0][1] = aafA[0][2] * aafA[2][1] - aafA[0][1] * aafA[2][2];
	aafAInv[0][2] = aafA[0][1] * aafA[1][2] - aafA[0][2] * aafA[1][1];
	aafAInv[1][0] = aafA[1][2] * aafA[2][0] - aafA[1][0] * aafA[2][2];
	aafAInv[1][1] = aafA[0][0] * aafA[2][2] - aafA[0][2] * aafA[2][0];
	aafAInv[1][2] = aafA[0][2] * aafA[1][0] - aafA[0][0] * aafA[1][2];
	aafAInv[2][0] = aafA[1][0] * aafA[2][1] - aafA[1][1] * aafA[2][0];
	aafAInv[2][1] = aafA[0][1] * aafA[2][0] - aafA[0][0] * aafA[2][1];
	aafAInv[2][2] = aafA[0][0] * aafA[1][1] - aafA[0][1] * aafA[1][0];

	// As with the three point fitting function, determinant is zero if any of the four points are collinear
	REAL det = aafA[0][0] * aafAInv[0][0] + aafA[0][1] * aafAInv[1][0] + aafA[0][2] * aafAInv[2][0];

	CRuSphere minimalSphere;

	if(fabs(det) > m_epsilon)
	{
		int row, col;

		REAL invDev = 1.0f / det;
		for(row = 0; row < 3; row++)
		{
			for(col = 0; col < 3; col++)
				aafAInv[row][col] *= invDev;
		}

		REAL afU[4];
		for(row = 0; row < 3; row++)
		{
			afU[row] = 0.0f;
			for (col = 0; col < 3; col++)
				afU[row] += aafAInv[row][col] * afB[col];
		}
		afU[3] = 1.0f - afU[0] - afU[1] - afU[2];

		// NOTE: Strange behavior, sometimes the u's go totally out of the [0, 1] range! So we code around that...
		if(fabs(afU[0]) > 1.0f || fabs(afU[1]) > 1.0f || fabs(afU[2]) > 1.0f || fabs(afU[3]) > 1.0f)
		{
			// A cop-out for now: Just calculate some sort of average sphere
			minimalSphere.Center().Set((point0 + point1 + point2 + point3) / 4.0f);
			CRuVector3 a0 = point0 - minimalSphere.Center();
			CRuVector3 a1 = point1 - minimalSphere.Center();
			CRuVector3 a2 = point2 - minimalSphere.Center();
			CRuVector3 a3 = point3 - minimalSphere.Center();
			minimalSphere.Radius() = max(max(max(a0.SquaredMagnitude(), a1.SquaredMagnitude()), a2.SquaredMagnitude()), a3.SquaredMagnitude());
		}
		else
		{
			minimalSphere.Center() = point0 * afU[0] + point1 * afU[1] + point2 * afU[2] + point3 * afU[3];
			CRuVector3 temp = e10 * afU[0] + e20 * afU[1] + e30 * afU[2];
			minimalSphere.Radius() = temp.SquaredMagnitude();
		}
	}
	else
	{
		// A cop-out for now: Just calculate some sort of average sphere
		minimalSphere.Center().Set((point0 + point1 + point2 + point3) / 4.0f);
		CRuVector3 a0 = point0 - minimalSphere.Center();
		CRuVector3 a1 = point1 - minimalSphere.Center();
		CRuVector3 a2 = point2 - minimalSphere.Center();
		CRuVector3 a3 = point3 - minimalSphere.Center();
		minimalSphere.Radius() = max(max(max(a0.SquaredMagnitude(), a1.SquaredMagnitude()), a2.SquaredMagnitude()), a3.SquaredMagnitude());
	}

	return minimalSphere;
}

CRuSphere CRuMinSphereBuilder::UpdateSupport1(int i, CRuVector3 **permutation, Support &support)
{
	const CRuVector3 &point0 = *permutation[support.Index[0]];
	const CRuVector3 &point1 = *permutation[i];

	CRuSphere minimalSphere = ExactSphere2(point0, point1);
	support.Quantity = 2;
	support.Index[1] = i;

	return minimalSphere;
}

CRuSphere CRuMinSphereBuilder::UpdateSupport2(int i, CRuVector3 **permutation, Support &support)
{
	const CRuVector3 &point0 = *permutation[support.Index[0]];
	const CRuVector3 &point1 = *permutation[support.Index[1]];
	const CRuVector3 &point2 = *permutation[i];

	CRuSphere akS[3];
	REAL fMinRSqr = FLT_MAX;
	REAL fDistDiff;
	int iIndex = -1;

	akS[0] = ExactSphere2(point0, point2);
	if(Contains(point1, akS[0], fDistDiff))
	{
		fMinRSqr = akS[0].Radius();
		iIndex = 0;
	}

	akS[1] = ExactSphere2(point1, point2);
	if(akS[1].Radius() < fMinRSqr)
	{
		if(Contains(point0, akS[1], fDistDiff))
		{
			fMinRSqr = akS[1].Radius();
			iIndex = 1;
		}
	}

	CRuSphere minimalSphere;

	if(iIndex != -1)
	{
		minimalSphere = akS[iIndex];
		support.Index[1 - iIndex] = i;
	}
	else
	{
		minimalSphere = ExactSphere3(point0, point1, point2);
//		assert( minimalSphere.Radius() <= fMinRSqr);
		support.Quantity = 3;
		support.Index[2] = i;
	}

	return minimalSphere;
}

CRuSphere CRuMinSphereBuilder::UpdateSupport3(int i, CRuVector3 **permutation, Support &support)
{
	const CRuVector3 &point0 = *permutation[support.Index[0]];
	const CRuVector3 &point1 = *permutation[support.Index[1]];
	const CRuVector3 &point2 = *permutation[support.Index[2]];
	const CRuVector3 &point3 = *permutation[i];

	CRuSphere akS[6];
	REAL fMinRSqr = FLT_MAX;
	REAL fDistDiff;
	int iIndex = -1;

	akS[0] = ExactSphere2(point0, point3);
	if(Contains(point1, akS[0], fDistDiff) && Contains(point2, akS[0], fDistDiff))
	{
		fMinRSqr = akS[0].Radius();
		iIndex = 0;
	}

	akS[1] = ExactSphere2(point1 ,point3);
	if(akS[1].Radius() < fMinRSqr && Contains(point0, akS[1], fDistDiff) && Contains(point2, akS[1], fDistDiff))
	{
		fMinRSqr = akS[1].Radius();
		iIndex = 1;
	}

	akS[2] = ExactSphere2(point2, point3);
	if(akS[2].Radius() < fMinRSqr && Contains(point0, akS[2], fDistDiff) && Contains(point1, akS[2], fDistDiff))
	{
		fMinRSqr = akS[2].Radius();
		iIndex = 2;
	}

	akS[3] = ExactSphere3(point0, point1, point3);
	if(akS[3].Radius() < fMinRSqr && Contains(point2, akS[3], fDistDiff))
	{
		fMinRSqr = akS[3].Radius();
		iIndex = 3;
	}

	akS[4] = ExactSphere3(point0, point2, point3);
	if(akS[4].Radius() < fMinRSqr && Contains(point1, akS[4], fDistDiff))
	{
		fMinRSqr = akS[4].Radius();
		iIndex = 4;
	}

	akS[5] = ExactSphere3(point1, point2, point3);
	if(akS[5].Radius() < fMinRSqr && Contains(point0, akS[5], fDistDiff))
	{
		fMinRSqr = akS[5].Radius();
		iIndex = 5;
	}

	CRuSphere minimalSphere;

	switch(iIndex)
	{
		case 0:
			minimalSphere = akS[0];
			support.Quantity = 2;
			support.Index[1] = i;
			break;
		case 1:
			minimalSphere = akS[1];
			support.Quantity = 2;
			support.Index[0] = i;
			break;
		case 2:
			minimalSphere = akS[2];
			support.Quantity = 2;
			support.Index[0] = support.Index[2];
			support.Index[1] = i;
			break;
		case 3:
			minimalSphere = akS[3];
			support.Index[2] = i;
			break;
		case 4:
			minimalSphere = akS[4];
			support.Index[1] = i;
			break;
		case 5:
			minimalSphere = akS[5];
			support.Index[0] = i;
			break;
		default:
			minimalSphere = ExactSphere4(point0, point1, point2, point3);
//			assert( minimalSphere.Radius() <= fMinRSqr);
			support.Quantity = 4;
			support.Index[3] = i;
			break;
	}

	return minimalSphere;
}

CRuSphere CRuMinSphereBuilder::UpdateSupport4(int i, CRuVector3 **permutation, Support &support)
{
	const CRuVector3* apkPt[4] =
	{
		permutation[support.Index[0]],
		permutation[support.Index[1]],
		permutation[support.Index[2]],
		permutation[support.Index[3]]
	};

	const CRuVector3 &point4 = *permutation[i];

	// permutations of type 1
	int aiT1[4][4] =
	{
		{0, /*4*/ 1,2,3},
		{1, /*4*/ 0,2,3},
		{2, /*4*/ 0,1,3},
		{3, /*4*/ 0,1,2}
	};

	// permutations of type 2
	int aiT2[6][4] =
	{
		{0,1, /*4*/ 2,3},
		{0,2, /*4*/ 1,3},
		{0,3, /*4*/ 1,2},
		{1,2, /*4*/ 0,3},
		{1,3, /*4*/ 0,2},
		{2,3, /*4*/ 0,1}
	};

	// permutations of type 3
	int aiT3[4][4] =
	{
		{0,1,2, /*4*/ 3},
		{0,1,3, /*4*/ 2},
		{0,2,3, /*4*/ 1},
		{1,2,3, /*4*/ 0}
	};

	CRuSphere akS[14];
	REAL fMinRSqr = FLT_MAX;
	int iIndex = -1;
	REAL fDistDiff, fMinDistDiff = FLT_MAX;
	int iMinIndex = -1;
	int k = 0;  // sphere index

	// permutations of type 1
	int j;
	for (j = 0; j < 4; j++, k++)
	{
		akS[k] = ExactSphere2(*apkPt[aiT1[j][0]],point4);
		if(akS[k].Radius() < fMinRSqr)
		{
			if(Contains(*apkPt[aiT1[j][1]],akS[k],fDistDiff) && Contains(*apkPt[aiT1[j][2]],akS[k],fDistDiff) && Contains(*apkPt[aiT1[j][3]],akS[k],fDistDiff))
			{
				fMinRSqr = akS[k].Radius();
				iIndex = k;
			}
			else if(fDistDiff < fMinDistDiff)
			{
				fMinDistDiff = fDistDiff;
				iMinIndex = k;
			}
		}
	}

	// permutations of type 2
	for (j = 0; j < 6; j++, k++)
	{
		akS[k] = ExactSphere3(*apkPt[aiT2[j][0]],*apkPt[aiT2[j][1]],point4);
		if(akS[k].Radius() < fMinRSqr)
		{
			if(Contains(*apkPt[aiT2[j][2]],akS[k],fDistDiff)
				&&   Contains(*apkPt[aiT2[j][3]],akS[k],fDistDiff))
			{
				fMinRSqr = akS[k].Radius();
				iIndex = k;
			}
			else if(fDistDiff < fMinDistDiff)
			{
				fMinDistDiff = fDistDiff;
				iMinIndex = k;
			}
		}
	}

	// permutations of type 3
	for (j = 0; j < 4; j++, k++)
	{
		akS[k] = ExactSphere4(*apkPt[aiT3[j][0]],*apkPt[aiT3[j][1]],
			*apkPt[aiT3[j][2]],point4);
		if(akS[k].Radius() < fMinRSqr)
		{
			if(Contains(*apkPt[aiT3[j][3]],akS[k],fDistDiff))
			{
				fMinRSqr = akS[k].Radius();
				iIndex = k;
			}
			else if(fDistDiff < fMinDistDiff)
			{
				fMinDistDiff = fDistDiff;
				iMinIndex = k;
			}
		}
	}

	// Theoretically, iIndex >= 0 should happen, but floating point round-off
	// error can lead to this.  When this happens, the sphere is chosen that
	// has the minimum absolute errors between points (barely) outside the
	// sphere and the sphere.
	if(iIndex == -1)
		iIndex = iMinIndex;

	CRuSphere minimalSphere = akS[iIndex];

	switch ( iIndex)
	{
		case 0:
			support.Quantity = 2;
			support.Index[1] = i;
			break;
		case 1:
			support.Quantity = 2;
			support.Index[0] = i;
			break;
		case 2:
			support.Quantity = 2;
			support.Index[0] = support.Index[2];
			support.Index[1] = i;
			break;
		case 3:
			support.Quantity = 2;
			support.Index[0] = support.Index[3];
			support.Index[1] = i;
			break;
		case 4:
			support.Quantity = 3;
			support.Index[2] = i;
			break;
		case 5:
			support.Quantity = 3;
			support.Index[1] = i;
			break;
		case 6:
			support.Quantity = 3;
			support.Index[1] = support.Index[3];
			support.Index[2] = i;
			break;
		case 7:
			support.Quantity = 3;
			support.Index[0] = i;
			break;
		case 8:
			support.Quantity = 3;
			support.Index[0] = support.Index[3];
			support.Index[2] = i;
			break;
		case 9:
			support.Quantity = 3;
			support.Index[0] = support.Index[3];
			support.Index[1] = i;
			break;
		case 10:
			support.Index[3] = i;
			break;
		case 11:
			support.Index[2] = i;
			break;
		case 12:
			support.Index[1] = i;
			break;
		case 13:
			support.Index[0] = i;
			break;
	}

	return minimalSphere;
}

// ************************************************************************************************************************************************************

#pragma managed
