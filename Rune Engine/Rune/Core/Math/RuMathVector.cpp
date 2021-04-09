#include "RuMathVector.h"

#include "RuMathSSE.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

REAL CRuVector3::Factor()
{
	REAL factorNumber;
	CRuVector3 unitVec;

	unitVec.x = x;
	unitVec.y = y;
	unitVec.z = z;
	unitVec.Normalize();

	if(unitVec.x != 0)
		factorNumber = x / unitVec.x;
	else if(unitVec.y != 0)
		factorNumber = y / unitVec.y;
	else
		factorNumber = z / unitVec.z;
	
	return factorNumber;
}

CRuVector3 &CRuVector3::Normalize()
{
#ifdef _DEBUG

	REAL vecMag = Magnitude();
	assert(vecMag > ruEPSILON);

	// No check for NaN or near-zero value
	x = x / vecMag;
	y = y / vecMag;
	z = z / vecMag;

#else

	REAL invVecMag = RuInvSqrt(m_x * m_x + m_y * m_y + m_z * m_z);

	x = x * invVecMag;
	y = y * invVecMag;
	z = z * invVecMag;

#endif

	return *this;
}

CRuVector3 &CRuVector3::Cross(const CRuVector3 &vec)
{
	REAL newX = y * vec.z - z * vec.y;
	REAL newY = z * vec.x - x * vec.z;
	z = x * vec.y - y * vec.x;
	
	x = newX;
	y = newY;

	return *this;
}

void CRuVector3::SetToCrossProduct(const CRuVector3 &vector1, const CRuVector3 &vector2)
{
	// NOTE: vector1 and vector2 must not be equal to *this!!
	x = vector1.y * vector2.z - vector1.z * vector2.y;
	y = vector1.z * vector2.x - vector1.x * vector2.z;
	z = vector1.x * vector2.y - vector1.y * vector2.x;
}

CRuVector3 &CRuVector3::ProjectOnto(const CRuVector3 &u)
{
	CRuVector3 uNormalized = u;
	uNormalized.Normalize();
	REAL t = DotProduct(*this, uNormalized);

	m_x = uNormalized.m_x * t;
	m_y = uNormalized.m_y * t;
	m_z = uNormalized.m_z * t;

	return *this;
}

void CRuVector3::GenerateOrthonormalBasis(CRuVector3 &rkU, CRuVector3 &rkV, CRuVector3 &rkW, BOOL unitLengthW)
{
	if(!unitLengthW)
	{
		rkW.Normalize();
	}

	REAL fInvLength;

	if(fabs(rkW.m_elements[0]) >= fabs(rkW.m_elements[1]))
	{
		// W.x or W.z is the largest magnitude component, swap them
		fInvLength = RuInvSqrt(rkW.m_elements[0] * rkW.m_elements[0] + rkW.m_elements[2] * rkW.m_elements[2]);
		rkU.m_elements[0] = -rkW.m_elements[2] * fInvLength;
		rkU.m_elements[1] =  0.0f;
		rkU.m_elements[2] =  rkW.m_elements[0] * fInvLength;
		rkV.m_elements[0] =  rkW.m_elements[1] * rkU.m_elements[2];
		rkV.m_elements[1] =  rkW.m_elements[2] * rkU.m_elements[0] - rkW.m_elements[0] * rkU.m_elements[2];
		rkV.m_elements[2] = -rkW.m_elements[1] * rkU.m_elements[0];
	}
	else
	{
		// W.y or W.z is the largest magnitude component, swap them
		fInvLength = RuInvSqrt(rkW.m_elements[1] * rkW.m_elements[1] + rkW.m_elements[2] * rkW.m_elements[2]);
		rkU.m_elements[0] = 0.0f;
		rkU.m_elements[1] =  rkW.m_elements[2] * fInvLength;
		rkU.m_elements[2] = -rkW.m_elements[1] * fInvLength;
		rkV.m_elements[0] =  rkW.m_elements[1] * rkU.m_elements[2] - rkW.m_elements[2] * rkU.m_elements[1];
		rkV.m_elements[1] = -rkW.m_elements[0] * rkU.m_elements[2];
		rkV.m_elements[2] =  rkW.m_elements[0] * rkU.m_elements[1];
	}
}

// ************************************************************************************************************************************************************

CRuPlane4::CRuPlane4()
:	m_a(0.0f),
	m_b(0.0f),
	m_c(0.0f),
	m_d(0.0f)
{
}

CRuPlane4::CRuPlane4(const CRuVector3 &normal, const CRuVector3 &p0)
{
	SetPlane(normal, p0);
}

CRuPlane4::CRuPlane4(REAL va, REAL vb, REAL vc, REAL vd)
{
	m_a = va;
	m_b = vb;
	m_c = vc;
	m_d = vd;
}

void CRuPlane4::SetPlane(const CRuVector3 &normal, const CRuVector3 &p0)
{
	// TODO: change this to assume normal is length 1....
	REAL normalLength = normal.Magnitude();
	m_a = normal.x / normalLength;
	m_b = normal.y / normalLength;
	m_c = normal.z / normalLength;
	m_d = -(m_a * p0.x + m_b * p0.y + m_c * p0.z);
}

void CRuPlane4::SetPlane(const CRuVector3 &p0, const CRuVector3 &p1, const CRuVector3 &p2)
{
	SetPlane(CrossProduct((p1 - p0), (p2 - p0)).Normalize(), p0);
}

void CRuPlane4::Normalize()
{
	REAL invMagnitude = RuInvSqrt(m_a * m_a + m_b * m_b + m_c * m_c);

	m_a = m_a * invMagnitude;
	m_b = m_b * invMagnitude;
	m_c = m_c * invMagnitude;
	m_d = m_d * invMagnitude;
}

REAL CRuPlane4::SolveForY(REAL x, REAL z) const
{
	if(m_b != 0.0f)
		return -(m_a * x + m_c * z + m_d) / m_b;
	else
		return 0.0f;
}

CRuVector3 CRuPlane4::GetClosestPointOnPlane(const CRuVector3 &point) const
{
	CRuVector3 planeNormal(m_a, m_b, m_c);
	return point - planeNormal * (DotProduct(planeNormal, point) + m_d);
}

// ************************************************************************************************************************************************************

CRuMatrix4x4::CRuMatrix4x4()
:	_11(1.0f), _12(0.0f), _13(0.0f), _14(0.0f),
	_21(0.0f), _22(1.0f), _23(0.0f), _24(0.0f),
	_31(0.0f), _32(0.0f), _33(1.0f), _34(0.0f),
	_41(0.0f), _42(0.0f), _43(0.0f), _44(1.0f)
{
}

CRuMatrix4x4::CRuMatrix4x4(const CRuMatrix4x4 &mat)
:	_11(mat._11), _12(mat._12), _13(mat._13), _14(mat._14),
	_21(mat._21), _22(mat._22), _23(mat._23), _24(mat._24),
	_31(mat._31), _32(mat._32), _33(mat._33), _34(mat._34),
	_41(mat._41), _42(mat._42), _43(mat._43), _44(mat._44)
{
}

CRuMatrix4x4 &CRuMatrix4x4::SetToIdentity()
{
	_11 = 1.0f;
	_21 = 0.0f;
	_31 = 0.0f;
	_41 = 0.0f;

	_12 = 0.0f;
	_22 = 1.0f;
	_32 = 0.0f;
	_42 = 0.0f;

	_13 = 0.0f;
	_23 = 0.0f;
	_33 = 1.0f;
	_43 = 0.0f;

	_14 = 0.0f;
	_24 = 0.0f;
	_34 = 0.0f;
	_44 = 1.0f;

	// Return reference to self
	return *this;
}

CRuMatrix4x4 &CRuMatrix4x4::SetToTranslation(REAL x, REAL y, REAL z)
{
	SetToIdentity();
	_41 = x;
	_42 = y;
	_43 = z;

	// Return reference to self
	return *this;
}

CRuMatrix4x4 &CRuMatrix4x4::SetToScaling(REAL xScale, REAL yScale, REAL zScale)
{
	SetToIdentity();
	_11 = xScale;
	_22 = yScale;
	_33 = zScale;

	// Return reference to self
	return *this;
}

/*
	Description	: Construct a rotation matrix as euler rotations X-Y-Z(??? Am I sure?)
	Parameters	: Rotation angles x, y, and z in radians.
	Output		: None.
*/
CRuMatrix4x4 &CRuMatrix4x4::SetToRotationEulerXYZ(REAL x, REAL y, REAL z)
{
	double cosX = cos(x);
	double sinX = sin(x);
	double cosY = cos(y);
	double sinY = sin(y);
	double cosZ = cos(z);
	double sinZ = sin(z);

	double sinXsinY = sinX * sinY;
	double cosXsinY = cosX * sinY;

	_11 = (REAL) (cosY * cosZ);
	_12 = (REAL) (cosY * sinZ);
	_13 = (REAL) (-sinY);
	_14 = 0.0f;

	_21 = (REAL) (sinXsinY * cosZ - cosX * sinZ);
	_22 = (REAL) (sinXsinY * sinZ + cosX * cosZ);
	_23 = (REAL) (sinX * cosY);
	_24 = 0.0f;

	_31 = (REAL) (cosXsinY * cosZ + sinX * sinZ);
	_32 = (REAL) (cosXsinY * sinZ - sinX * cosZ);
	_33 = (REAL) (cosX * cosY);
	_34 = 0.0f;

	_41 = 0.0f;
	_42 = 0.0f;
	_43 = 0.0f;
	_44 = 1.0f;

	// Return reference to self
	return *this;
}

/*
	Description	: Construct a matrix from an angle-axis rotation.
	Parameters	: Unit vector axis specifying the axis of rotation, and the rotation angle in radians.
	Output		: None.
*/
CRuMatrix4x4 &CRuMatrix4x4::SetToRotation(const CRuVector3 &axis, REAL angle)
{
	REAL sinPhi = (REAL) sin(angle);
	REAL cosPhi = (REAL) cos(angle);
	REAL invCosPhi = 1.0f - cosPhi;

	_11 = cosPhi + invCosPhi * axis.x * axis.x;				_12 = invCosPhi * axis.x * axis.y + axis.z * sinPhi;	_13 = invCosPhi * axis.x * axis.z - axis.y * sinPhi;	_14 = 0.0f;
	_21 = invCosPhi * axis.x * axis.y - axis.z * sinPhi;	_22 = cosPhi + invCosPhi * axis.y * axis.y;				_23 = invCosPhi * axis.y * axis.z + axis.x * sinPhi;	_24 = 0.0f;
	_31 = invCosPhi * axis.x * axis.z + axis.y * sinPhi;	_32 = invCosPhi * axis.y * axis.z - axis.x * sinPhi;	_33 = cosPhi + invCosPhi * axis.z * axis.z;				_34 = 0.0f;
	_41 = 0.0f;												_42 = 0.0f;												_43 = 0.0f;												_44 = 1.0f;

	// Return reference to self
	return *this;
}

/*
	Description	: Construct a matrix that rotates from the vector s into the vector t.
	Parameters	: Unit vectors s(starting vector) and t(target vector).
	Output		: None.
*/
CRuMatrix4x4 &CRuMatrix4x4::SetToRotationSxT(const CRuVector3 &s, const CRuVector3 &t)
{
	CRuVector3 v = CrossProduct(s, t);
	REAL e = DotProduct(s, t);

	if(v.Magnitude() <= ruEPSILON)
	{
		// Is the angle between s and t almost zero? If so, return the identity matrix
		if(e >= (1.0f - ruEPSILON))
		{
			SetToIdentity();
		}
		// Otherwise we rotate pi radians around any axis
		else
		{
			// Find an arbitrary axis u which is perpendicular to s
			CRuVector3 u = CrossProduct(s, CRuVector3(1.0f, 0.0f, 0.0f));
			if(u.Magnitude() <= ruEPSILON)
				u = CrossProduct(s, CRuVector3(0.0f, 1.0f, 0.0f));
			SetToRotation(u, PI);
		}
	}
	else
	{
		REAL h = (1.0f - e) / DotProduct(v, v);

		_11 = e + h * v.x * v.x;	_12 = h * v.x * v.y + v.z;	_13 = h * v.x * v.z - v.y;	_14 = 0.0f;
		_21 = h * v.x * v.y - v.z;	_22 = e + h * v.y * v.y;	_23 = h * v.y * v.z + v.x;	_24 = 0.0f;
		_31 = h * v.x * v.z + v.y;	_32 = h * v.y * v.z - v.x;	_33 = e + h * v.z * v.z;	_34 = 0.0f;
		_41 = 0.0f;					_42 = 0.0f;					_43 = 0.0f;					_44 = 1.0f;
	}

	// Return reference to self
	return *this;
}

CRuMatrix4x4 &CRuMatrix4x4::SetToBoneSRT(REAL scale, const CRuQuaternion &rotation, const CRuVector3 &translation)
{
	REAL x2, y2, w2, xy2, xz2, yz2, wx2, wy2, wz2, xx2, yy2, zz2;

	// Pre-calculate needed temporaries for quaternion conversion
	x2 = 2.0f * rotation.x;
	y2 = 2.0f * rotation.y;
	w2 = 2.0f * rotation.w;
	xy2 = x2 * rotation.y;
	xz2 = x2 * rotation.z;
	yz2 = y2 * rotation.z;
	wx2 = w2 * rotation.x;
	wy2 = w2 * rotation.y;
	wz2 = w2 * rotation.z;
	xx2 = x2 * rotation.x;
	yy2 = y2 * rotation.y;
	zz2 = 2.0f * rotation.z * rotation.z;

	// --- Assignment Order for Row-Major Matrices ---

	_11 = (1.0f - yy2 - zz2) * scale;
	_21 = (xy2 + wz2) * scale;
	_31 = (xz2 - wy2) * scale;
	_41 = translation.x;

	_12 = (xy2 - wz2) * scale;
	_22 = (1.0f - xx2 - zz2) * scale;
	_32 = (yz2 + wx2) * scale;
	_42 = translation.y;

	_13 = (xz2 + wy2) * scale;
	_23 = (yz2 - wx2) * scale;
	_33 = (1.0f - xx2 - yy2) * scale;
	_43 = translation.z;

	_14 = 0.0f;
	_24 = 0.0f;
	_34 = 0.0f;
	_44 = 1.0f;

	// Return reference to self
	return *this;
}

CRuMatrix4x4 &CRuMatrix4x4::SetToBoneSRT(const CRuVector3 &scale, const CRuQuaternion &rotation, const CRuVector3 &translation)
{
	REAL x2, y2, w2, xy2, xz2, yz2, wx2, wy2, wz2, xx2, yy2, zz2;

	// Pre-calculate needed temporaries for quaternion conversion
	x2 = 2.0f * rotation.x;
	y2 = 2.0f * rotation.y;
	w2 = 2.0f * rotation.w;
	xy2 = x2 * rotation.y;
	xz2 = x2 * rotation.z;
	yz2 = y2 * rotation.z;
	wx2 = w2 * rotation.x;
	wy2 = w2 * rotation.y;
	wz2 = w2 * rotation.z;
	xx2 = x2 * rotation.x;
	yy2 = y2 * rotation.y;
	zz2 = 2.0f * rotation.z * rotation.z;

	// --- Assignment Order for Row-Major Matrices ---

	_11 = (1.0f - yy2 - zz2) * scale.x;
	_21 = (xy2 + wz2) * scale.y;
	_31 = (xz2 - wy2) * scale.z;
	_41 = translation.x;

	_12 = (xy2 - wz2) * scale.x;
	_22 = (1.0f - xx2 - zz2) * scale.y;
	_32 = (yz2 + wx2) * scale.z;
	_42 = translation.y;

	_13 = (xz2 + wy2) * scale.x;
	_23 = (yz2 - wx2) * scale.y;
	_33 = (1.0f - xx2 - yy2) * scale.z;
	_43 = translation.z;

	_14 = 0.0f;
	_24 = 0.0f;
	_34 = 0.0f;
	_44 = 1.0f;

	// Return reference to self
	return *this;
}

CRuMatrix4x4 &CRuMatrix4x4::SetToReflection(const CRuPlane4 &reflectionPlane)
{
	_11 = -2.0f * reflectionPlane.m_a * reflectionPlane.m_a + 1.0f;
	_21 = -2.0f * reflectionPlane.m_a * reflectionPlane.m_b;
	_31 = -2.0f * reflectionPlane.m_a * reflectionPlane.m_c;
	_41 = -2.0f * reflectionPlane.m_a * reflectionPlane.m_d;

	_12 = -2.0f * reflectionPlane.m_b * reflectionPlane.m_a;
	_22 = -2.0f * reflectionPlane.m_b * reflectionPlane.m_b + 1.0f;
	_32 = -2.0f * reflectionPlane.m_b * reflectionPlane.m_c;
	_42 = -2.0f * reflectionPlane.m_b * reflectionPlane.m_d;

	_13 = -2.0f * reflectionPlane.m_c * reflectionPlane.m_a;
	_23 = -2.0f * reflectionPlane.m_c * reflectionPlane.m_b;
	_33 = -2.0f * reflectionPlane.m_c * reflectionPlane.m_c + 1.0f;
	_43 = -2.0f * reflectionPlane.m_c * reflectionPlane.m_d;

	_14 = 0.0f;
	_24 = 0.0f;
	_34 = 0.0f;
	_44 = 1.0f;

	// Return reference to self
	return *this;
}

/*!
	Sets the matrix to a perspective projection with the specified field of view values.

	@param nearPlane Near view-plane distance.
	@param farPlane Far view-plane distance.
	@param fovHorz Horizontal field of view, in radians.
	@param fovVert Vertical field of view, in radians.

	@return Reference to the matrix itself.
*/
CRuMatrix4x4 &CRuMatrix4x4::SetToProjection(REAL nearPlane, REAL farPlane, REAL fovHorz, REAL fovVert)
{
	// Reset matrix
	SetToIdentity();

	// Calculate intermediaries
	REAL w = (REAL) cos(fovHorz * 0.5f) / (REAL) sin(fovHorz * 0.5f);
	REAL h = (REAL) cos(fovVert * 0.5f) / (REAL) sin(fovVert * 0.5f);
	REAL Q = farPlane / (farPlane - nearPlane);

	// Fill out matrix
	m_elements[0][0] = w;
	m_elements[1][1] = h;
	m_elements[2][2] = Q;
	m_elements[3][2] = -Q * nearPlane;
	m_elements[2][3] = 1.0f;
	m_elements[3][3] = 0.0f;

	// Return reference to self
	return *this;
}

/*!
	Sets the matrix to a perspective projection with the specified view volume size.

	@param nearPlane Near view-plane distance.
	@param farPlane Far view-plane distance.
	@param w Width of the view volume at the near view-plane.
	@param h Height of the view volume at the near view-plane.

	@return Reference to the matrix itself.
*/
CRuMatrix4x4 &CRuMatrix4x4::SetToPerspectiveProjection(REAL nearPlane, REAL farPlane, REAL w, REAL h)
{
	// Reset matrix
	SetToIdentity();

	m_elements[0][0] = 2.0f * nearPlane / w;
	m_elements[1][1] = 2.0f * nearPlane / h;
	m_elements[2][2] = farPlane / (farPlane - nearPlane);
	m_elements[3][2] = nearPlane * farPlane / (nearPlane - farPlane);
	m_elements[2][3] = 1.0f;
	m_elements[3][3] = 0.0f;

	// Return reference to self
	return *this;
}

/*!
	Sets the matrix to the basis formed by the three specified axis.

	@param xAxis X axis of the basis coordinate frame.
	@param yAxis Y axis of the basis coordinate frame.
	@param zAxis Z axis of the basis coordinate frame.

	@return Reference to the matrix itself.
*/
CRuMatrix4x4 &CRuMatrix4x4::SetToBasis(const CRuVector3 &xAxis, const CRuVector3 &yAxis, const CRuVector3 &zAxis)
{
	_11 = xAxis.x;	_12 = xAxis.y;	_13 = xAxis.z;	_14 = 0.0f;
	_21 = yAxis.x;	_22 = yAxis.y;	_23 = yAxis.z;	_24 = 0.0f;
	_31 = zAxis.x;	_32 = zAxis.y;	_33 = zAxis.z;	_34 = 0.0f;
	_41 = 0.0f;		_42 = 0.0f;		_43 = 0.0f;		_44 = 1.0f;

	// Return reference to self
	return *this;
}

/*!
	Sets the matrix to the inverse of the basis formed by the three specified axis.

	@param xAxis X axis of the basis coordinate frame.
	@param yAxis Y axis of the basis coordinate frame.
	@param zAxis Z axis of the basis coordinate frame.

	@return Reference to the matrix itself.
*/
CRuMatrix4x4 &CRuMatrix4x4::SetToInverseBasis(const CRuVector3 &xAxis, const CRuVector3 &yAxis, const CRuVector3 &zAxis)
{
	_11 = xAxis.x;	_12 = yAxis.x;	_13 = zAxis.x;	_14 = 0.0f;
	_21 = xAxis.y;	_22 = yAxis.y;	_23 = zAxis.y;	_24 = 0.0f;
	_31 = xAxis.z;	_32 = yAxis.z;	_33 = zAxis.z;	_34 = 0.0f;
	_41 = 0.0f;		_42 = 0.0f;		_43 = 0.0f;		_44 = 1.0f;

	// Return reference to self
	return *this;
}

CRuMatrix4x4 &CRuMatrix4x4::SetToBillboard(const CRuMatrix4x4 &viewTransform, const CRuVector3 &geometryForward, const CRuVector3 &geometryUp)
{
	CRuVector3 geometrySide;
	CRuVector3 invViewDir, viewUp, ivXd;

	// Figure out the geometry's side vector
	geometrySide = CrossProduct(geometryForward, geometryUp);

	// Get camera forward and up vectors
	invViewDir.Set(viewTransform._13, viewTransform._23, viewTransform._33);
	viewUp.Set(viewTransform._12, viewTransform._22, viewTransform._32);

	// Invert camera forward vector
	invViewDir.x = -invViewDir.x;
	invViewDir.y = -invViewDir.y;
	invViewDir.z = -invViewDir.z;

	// Calculate side vector
	ivXd = CrossProduct(invViewDir, viewUp);

	// Build normalizing matrix
	CRuMatrix4x4 normMatrix;
	normMatrix._11 = geometryForward.x;		normMatrix._12 = geometryUp.x;			normMatrix._13 = geometrySide.x;	normMatrix._14 = 0.0f;
	normMatrix._21 = geometryForward.y;		normMatrix._22 = geometryUp.y;			normMatrix._23 = geometrySide.y;	normMatrix._24 = 0.0f;
	normMatrix._31 = geometryForward.z;		normMatrix._32 = geometryUp.z;			normMatrix._33 = geometrySide.z;	normMatrix._34 = 0.0f;
	normMatrix._41 = 0.0f;					normMatrix._42 = 0.0f;					normMatrix._43 = 0.0f;				normMatrix._44 = 1.0f;

	// Build billboard matrix (Transforms +x axis into -z axis)
	CRuMatrix4x4 bbMatrix;
	bbMatrix._11 = invViewDir.x;			bbMatrix._12 = invViewDir.y;			bbMatrix._13 = invViewDir.z;		bbMatrix._14 = 0.0f;
	bbMatrix._21 = viewUp.x;				bbMatrix._22 = viewUp.y;				bbMatrix._23 = viewUp.z;			bbMatrix._24 = 0.0f;
	bbMatrix._31 = ivXd.x;					bbMatrix._32 = ivXd.y;					bbMatrix._33 = ivXd.z;				bbMatrix._34 = 0.0f;
	bbMatrix._41 = 0.0f;					bbMatrix._42 = 0.0f;					bbMatrix._43 = 0.0f;				bbMatrix._44 = 1.0f;

	// Output billboard matrix
	*this = normMatrix * bbMatrix;

	return *this;
}

CRuMatrix4x4 &CRuMatrix4x4::SetToAxialBillboard(const CRuMatrix4x4 &viewTransform, const CRuVector3 &geometryForward, const CRuVector3 &constraintAxis)
{
	CRuVector3 invViewDir, viewUp, ivXd;
	CRuVector3 s, t;

	// Invert camera forward vector
	invViewDir.Set(viewTransform._13, viewTransform._23, viewTransform._33);
	invViewDir.x = -invViewDir.x;
	invViewDir.y = -invViewDir.y;
	invViewDir.z = -invViewDir.z;

	s.SetToCrossProduct(invViewDir, constraintAxis);
	t.SetToCrossProduct(constraintAxis, s);
	t.Normalize();

	REAL rotAngle = atan2(CrossProduct(geometryForward, t).Magnitude(), DotProduct(geometryForward, t));

	// Build rotation matrix
	this->SetToIdentity();
	if(DotProduct(CrossProduct(geometryForward, t), constraintAxis) > 0.0f)
	{
		this->Rotate(constraintAxis, rotAngle);
	}
	else
	{
		this->Rotate(constraintAxis, -rotAngle);
	}

	return *this;
}

CRuMatrix4x4 &CRuMatrix4x4::SetToWorldBillboard(const CRuMatrix4x4 &viewTransform, const CRuVector3 &geometryForward, const CRuVector3 &constraintAxis)
{
	return *this;
}

/*!
	Post multiplies the matrix by the scaling factor.

	@param scale (x, y, z) scaling factor.

	@return Reference to the matrix itself.
*/
CRuMatrix4x4 &CRuMatrix4x4::Scale(const CRuVector3 &scale)
{
	// Expanded post-multiply scale
	_11 = (_11 * scale.m_x);
	_12 = (_12 * scale.m_y);
	_13 = (_13 * scale.m_z);
	_14 = 0.0f;

	_21 = (_21 * scale.m_x);
	_22 = (_22 * scale.m_y);
	_23 = (_23 * scale.m_z);
	_24 = 0.0f;

	_31 = (_31 * scale.m_x);
	_32 = (_32 * scale.m_y);
	_33 = (_33 * scale.m_z);
	_34 = 0.0f;

	_41 = (_41 * scale.m_x);
	_42 = (_42 * scale.m_y);
	_43 = (_43 * scale.m_z);

	// Return reference to self
	return *this;
}

/*!
	Post multiplies the matrix by the quaternion rotation

	@param quat Rotation quaternion.

	@return Reference to the matrix itself.
*/
CRuMatrix4x4 &CRuMatrix4x4::Rotate(const CRuQuaternion &quat)
{
	CRuMatrix4x4 rotMat, tempMat;

	REAL x2, y2, w2, xy2, xz2, yz2, wx2, wy2, wz2, xx2, yy2, zz2;

	x2 = 2.0f * quat.x;
	y2 = 2.0f * quat.y;
	w2 = 2.0f * quat.w;
	xy2 = x2 * quat.y;
	xz2 = x2 * quat.z;
	yz2 = y2 * quat.z;
	wx2 = w2 * quat.x;
	wy2 = w2 * quat.y;
	wz2 = w2 * quat.z;
	xx2 = x2 * quat.x;
	yy2 = y2 * quat.y;
	zz2 = 2.0f * quat.z * quat.z;

	// Store rotation into rotMat
	rotMat._11 = 1.0f - yy2 - zz2;
	rotMat._21 = xy2 + wz2;
	rotMat._31 = xz2 - wy2;
	rotMat._41 = 0.0f;

	rotMat._12 = xy2 - wz2;
	rotMat._22 = 1.0f - xx2 - zz2;
	rotMat._32 = yz2 + wx2;
	rotMat._42 = 0.0f;

	rotMat._13 = xz2 + wy2;
	rotMat._23 = yz2 - wx2;
	rotMat._33 = 1.0f - xx2 - yy2;
	rotMat._43 = 0.0f;

	rotMat._14 = 0.0f;
	rotMat._24 = 0.0f;
	rotMat._34 = 0.0f;
	rotMat._44 = 1.0f;

	// Copy values of the original matrix into tempMat
	tempMat = *this;

	// Multiply matrices
	_11 = (tempMat._11 * rotMat._11) + (tempMat._12 * rotMat._21) + (tempMat._13 * rotMat._31) + (tempMat._14 * rotMat._41);
	_12 = (tempMat._11 * rotMat._12) + (tempMat._12 * rotMat._22) + (tempMat._13 * rotMat._32) + (tempMat._14 * rotMat._42);
	_13 = (tempMat._11 * rotMat._13) + (tempMat._12 * rotMat._23) + (tempMat._13 * rotMat._33) + (tempMat._14 * rotMat._43);
	_14 = (tempMat._11 * rotMat._14) + (tempMat._12 * rotMat._24) + (tempMat._13 * rotMat._34) + (tempMat._14 * rotMat._44);

	_21 = (tempMat._21 * rotMat._11) + (tempMat._22 * rotMat._21) + (tempMat._23 * rotMat._31) + (tempMat._24 * rotMat._41);
	_22 = (tempMat._21 * rotMat._12) + (tempMat._22 * rotMat._22) + (tempMat._23 * rotMat._32) + (tempMat._24 * rotMat._42);
	_23 = (tempMat._21 * rotMat._13) + (tempMat._22 * rotMat._23) + (tempMat._23 * rotMat._33) + (tempMat._24 * rotMat._43);
	_24 = (tempMat._21 * rotMat._14) + (tempMat._22 * rotMat._24) + (tempMat._23 * rotMat._34) + (tempMat._24 * rotMat._44);

	_31 = (tempMat._31 * rotMat._11) + (tempMat._32 * rotMat._21) + (tempMat._33 * rotMat._31) + (tempMat._34 * rotMat._41);
	_32 = (tempMat._31 * rotMat._12) + (tempMat._32 * rotMat._22) + (tempMat._33 * rotMat._32) + (tempMat._34 * rotMat._42);
	_33 = (tempMat._31 * rotMat._13) + (tempMat._32 * rotMat._23) + (tempMat._33 * rotMat._33) + (tempMat._34 * rotMat._43);
	_34 = (tempMat._31 * rotMat._14) + (tempMat._32 * rotMat._24) + (tempMat._33 * rotMat._34) + (tempMat._34 * rotMat._44);

	_41 = (tempMat._41 * rotMat._11) + (tempMat._42 * rotMat._21) + (tempMat._43 * rotMat._31) + (tempMat._44 * rotMat._41);
	_42 = (tempMat._41 * rotMat._12) + (tempMat._42 * rotMat._22) + (tempMat._43 * rotMat._32) + (tempMat._44 * rotMat._42);
	_43 = (tempMat._41 * rotMat._13) + (tempMat._42 * rotMat._23) + (tempMat._43 * rotMat._33) + (tempMat._44 * rotMat._43);
	_44 = (tempMat._41 * rotMat._14) + (tempMat._42 * rotMat._24) + (tempMat._43 * rotMat._34) + (tempMat._44 * rotMat._44);

	// Return reference to self
	return *this;
}

/*!
	Post multiplies the matrix by the angle-axis rotation.

	@param axis Axis of rotation.
	@param angle Rotation angle, in radians.

	@return Reference to the matrix itself.
*/
CRuMatrix4x4 &CRuMatrix4x4::Rotate(const CRuVector3 &axis, REAL angle)
{
	CRuMatrix4x4 rotMat, tempMat;

	REAL sinPhi = (REAL) sin(angle);
	REAL cosPhi = (REAL) cos(angle);
	REAL invCosPhi = 1.0f - cosPhi;

	REAL xy = axis.x * axis.y;
	REAL xz = axis.x * axis.z;
	REAL yz = axis.y * axis.z;

	// Store rotation into rotMat
	rotMat._11 = cosPhi + invCosPhi * axis.x * axis.x;	rotMat._12 = invCosPhi * xy + axis.z * sinPhi;		rotMat._13 = invCosPhi * xz - axis.y * sinPhi;		rotMat._14 = 0.0f;
	rotMat._21 = invCosPhi * xy - axis.z * sinPhi;		rotMat._22 = cosPhi + invCosPhi * axis.y * axis.y;	rotMat._23 = invCosPhi * yz + axis.x * sinPhi;		rotMat._24 = 0.0f;
	rotMat._31 = invCosPhi * xz + axis.y * sinPhi;		rotMat._32 = invCosPhi * yz - axis.x * sinPhi;		rotMat._33 = cosPhi + invCosPhi * axis.z * axis.z;	rotMat._34 = 0.0f;
	rotMat._41 = 0.0f;									rotMat._42 = 0.0f;									rotMat._43 = 0.0f;									rotMat._44 = 1.0f;

	// Copy values of the original matrix into tempMat
	tempMat = *this;

	// Multiply matrices
	_11 = (tempMat._11 * rotMat._11) + (tempMat._12 * rotMat._21) + (tempMat._13 * rotMat._31) + (tempMat._14 * rotMat._41);
	_12 = (tempMat._11 * rotMat._12) + (tempMat._12 * rotMat._22) + (tempMat._13 * rotMat._32) + (tempMat._14 * rotMat._42);
	_13 = (tempMat._11 * rotMat._13) + (tempMat._12 * rotMat._23) + (tempMat._13 * rotMat._33) + (tempMat._14 * rotMat._43);
	_14 = (tempMat._11 * rotMat._14) + (tempMat._12 * rotMat._24) + (tempMat._13 * rotMat._34) + (tempMat._14 * rotMat._44);

	_21 = (tempMat._21 * rotMat._11) + (tempMat._22 * rotMat._21) + (tempMat._23 * rotMat._31) + (tempMat._24 * rotMat._41);
	_22 = (tempMat._21 * rotMat._12) + (tempMat._22 * rotMat._22) + (tempMat._23 * rotMat._32) + (tempMat._24 * rotMat._42);
	_23 = (tempMat._21 * rotMat._13) + (tempMat._22 * rotMat._23) + (tempMat._23 * rotMat._33) + (tempMat._24 * rotMat._43);
	_24 = (tempMat._21 * rotMat._14) + (tempMat._22 * rotMat._24) + (tempMat._23 * rotMat._34) + (tempMat._24 * rotMat._44);

	_31 = (tempMat._31 * rotMat._11) + (tempMat._32 * rotMat._21) + (tempMat._33 * rotMat._31) + (tempMat._34 * rotMat._41);
	_32 = (tempMat._31 * rotMat._12) + (tempMat._32 * rotMat._22) + (tempMat._33 * rotMat._32) + (tempMat._34 * rotMat._42);
	_33 = (tempMat._31 * rotMat._13) + (tempMat._32 * rotMat._23) + (tempMat._33 * rotMat._33) + (tempMat._34 * rotMat._43);
	_34 = (tempMat._31 * rotMat._14) + (tempMat._32 * rotMat._24) + (tempMat._33 * rotMat._34) + (tempMat._34 * rotMat._44);

	_41 = (tempMat._41 * rotMat._11) + (tempMat._42 * rotMat._21) + (tempMat._43 * rotMat._31) + (tempMat._44 * rotMat._41);
	_42 = (tempMat._41 * rotMat._12) + (tempMat._42 * rotMat._22) + (tempMat._43 * rotMat._32) + (tempMat._44 * rotMat._42);
	_43 = (tempMat._41 * rotMat._13) + (tempMat._42 * rotMat._23) + (tempMat._43 * rotMat._33) + (tempMat._44 * rotMat._43);
	_44 = (tempMat._41 * rotMat._14) + (tempMat._42 * rotMat._24) + (tempMat._43 * rotMat._34) + (tempMat._44 * rotMat._44);

	// Return reference to self
	return *this;
}

/*!
	Post multiplies the matrix by the translation factor.

	@param translation (x, y, z) translation factor.

	@return Reference to the matrix itself.
*/
CRuMatrix4x4 &CRuMatrix4x4::Translate(const CRuVector3 &translation)
{
	// Expanded post-multiply translation
	_11 = _11 + (_14 * translation.m_x);
	_12 = _12 + (_14 * translation.m_y);
	_13 = _13 + (_14 * translation.m_z);

	_21 = _21 + (_24 * translation.m_x);
	_22 = _22 + (_24 * translation.m_y);
	_23 = _23 + (_24 * translation.m_z);

	_31 = _31 + (_34 * translation.m_x);
	_32 = _32 + (_34 * translation.m_y);
	_33 = _33 + (_34 * translation.m_z);

	_41 = _41 + (_44 * translation.m_x);
	_42 = _42 + (_44 * translation.m_y);
	_43 = _43 + (_44 * translation.m_z);

	// Return reference to self
	return *this;
}

CRuMatrix4x4 &CRuMatrix4x4::Reflect(const CRuPlane4 &reflectionPlane)
{
	CRuMatrix4x4 reflectMat, tempMat;

	reflectMat._11 = -2.0f * reflectionPlane.m_a * reflectionPlane.m_a + 1.0f;
	reflectMat._21 = -2.0f * reflectionPlane.m_a * reflectionPlane.m_b;
	reflectMat._31 = -2.0f * reflectionPlane.m_a * reflectionPlane.m_c;
	reflectMat._41 = -2.0f * reflectionPlane.m_a * reflectionPlane.m_d;

	reflectMat._12 = -2.0f * reflectionPlane.m_b * reflectionPlane.m_a;
	reflectMat._22 = -2.0f * reflectionPlane.m_b * reflectionPlane.m_b + 1.0f;
	reflectMat._32 = -2.0f * reflectionPlane.m_b * reflectionPlane.m_c;
	reflectMat._42 = -2.0f * reflectionPlane.m_b * reflectionPlane.m_d;

	reflectMat._13 = -2.0f * reflectionPlane.m_c * reflectionPlane.m_a;
	reflectMat._23 = -2.0f * reflectionPlane.m_c * reflectionPlane.m_b;
	reflectMat._33 = -2.0f * reflectionPlane.m_c * reflectionPlane.m_c + 1.0f;
	reflectMat._43 = -2.0f * reflectionPlane.m_c * reflectionPlane.m_d;

	reflectMat._14 = 0.0f;
	reflectMat._24 = 0.0f;
	reflectMat._34 = 0.0f;
	reflectMat._44 = 1.0f;

	// Copy values of the original matrix into tempMat
	tempMat = *this;

	// Multiply matrices
	_11 = (tempMat._11 * reflectMat._11) + (tempMat._12 * reflectMat._21) + (tempMat._13 * reflectMat._31) + (tempMat._14 * reflectMat._41);
	_12 = (tempMat._11 * reflectMat._12) + (tempMat._12 * reflectMat._22) + (tempMat._13 * reflectMat._32) + (tempMat._14 * reflectMat._42);
	_13 = (tempMat._11 * reflectMat._13) + (tempMat._12 * reflectMat._23) + (tempMat._13 * reflectMat._33) + (tempMat._14 * reflectMat._43);
	_14 = (tempMat._11 * reflectMat._14) + (tempMat._12 * reflectMat._24) + (tempMat._13 * reflectMat._34) + (tempMat._14 * reflectMat._44);

	_21 = (tempMat._21 * reflectMat._11) + (tempMat._22 * reflectMat._21) + (tempMat._23 * reflectMat._31) + (tempMat._24 * reflectMat._41);
	_22 = (tempMat._21 * reflectMat._12) + (tempMat._22 * reflectMat._22) + (tempMat._23 * reflectMat._32) + (tempMat._24 * reflectMat._42);
	_23 = (tempMat._21 * reflectMat._13) + (tempMat._22 * reflectMat._23) + (tempMat._23 * reflectMat._33) + (tempMat._24 * reflectMat._43);
	_24 = (tempMat._21 * reflectMat._14) + (tempMat._22 * reflectMat._24) + (tempMat._23 * reflectMat._34) + (tempMat._24 * reflectMat._44);

	_31 = (tempMat._31 * reflectMat._11) + (tempMat._32 * reflectMat._21) + (tempMat._33 * reflectMat._31) + (tempMat._34 * reflectMat._41);
	_32 = (tempMat._31 * reflectMat._12) + (tempMat._32 * reflectMat._22) + (tempMat._33 * reflectMat._32) + (tempMat._34 * reflectMat._42);
	_33 = (tempMat._31 * reflectMat._13) + (tempMat._32 * reflectMat._23) + (tempMat._33 * reflectMat._33) + (tempMat._34 * reflectMat._43);
	_34 = (tempMat._31 * reflectMat._14) + (tempMat._32 * reflectMat._24) + (tempMat._33 * reflectMat._34) + (tempMat._34 * reflectMat._44);

	_41 = (tempMat._41 * reflectMat._11) + (tempMat._42 * reflectMat._21) + (tempMat._43 * reflectMat._31) + (tempMat._44 * reflectMat._41);
	_42 = (tempMat._41 * reflectMat._12) + (tempMat._42 * reflectMat._22) + (tempMat._43 * reflectMat._32) + (tempMat._44 * reflectMat._42);
	_43 = (tempMat._41 * reflectMat._13) + (tempMat._42 * reflectMat._23) + (tempMat._43 * reflectMat._33) + (tempMat._44 * reflectMat._43);
	_44 = (tempMat._41 * reflectMat._14) + (tempMat._42 * reflectMat._24) + (tempMat._43 * reflectMat._34) + (tempMat._44 * reflectMat._44);

	// Return reference to self
	return *this;
}


/*!
	Post multiplies the matrix by the basis formed by the three specified axis.

	@param xAxis X axis of the basis coordinate frame.
	@param yAxis Y axis of the basis coordinate frame.
	@param zAxis Z axis of the basis coordinate frame.

	@return Reference to the matrix itself.
*/
CRuMatrix4x4 &CRuMatrix4x4::ChangeBasis(const CRuVector3 &xAxis, const CRuVector3 &yAxis, const CRuVector3 &zAxis)
{
	// Copy values of the original matrix into tempMat
	CRuMatrix4x4 tempMat = *this;

	_11 = (tempMat._11 * xAxis.x) + (tempMat._12 * yAxis.x) + (tempMat._13 * zAxis.x);
	_12 = (tempMat._11 * xAxis.y) + (tempMat._12 * yAxis.y) + (tempMat._13 * zAxis.y);
	_13 = (tempMat._11 * xAxis.z) + (tempMat._12 * yAxis.z) + (tempMat._13 * zAxis.z);

	_21 = (tempMat._21 * xAxis.x) + (tempMat._22 * yAxis.x) + (tempMat._23 * zAxis.x);
	_22 = (tempMat._21 * xAxis.y) + (tempMat._22 * yAxis.y) + (tempMat._23 * zAxis.y);
	_23 = (tempMat._21 * xAxis.z) + (tempMat._22 * yAxis.z) + (tempMat._23 * zAxis.z);

	_31 = (tempMat._31 * xAxis.x) + (tempMat._32 * yAxis.x) + (tempMat._33 * zAxis.x);
	_32 = (tempMat._31 * xAxis.y) + (tempMat._32 * yAxis.y) + (tempMat._33 * zAxis.y);
	_33 = (tempMat._31 * xAxis.z) + (tempMat._32 * yAxis.z) + (tempMat._33 * zAxis.z);

	_41 = (tempMat._41 * xAxis.x) + (tempMat._42 * yAxis.x) + (tempMat._43 * zAxis.x);
	_42 = (tempMat._41 * xAxis.y) + (tempMat._42 * yAxis.y) + (tempMat._43 * zAxis.y);
	_43 = (tempMat._41 * xAxis.z) + (tempMat._42 * yAxis.z) + (tempMat._43 * zAxis.z);

	// Return reference to self
	return *this;
}

/*!
	Post multiplies the matrix by the inverse basis formed by the three specified axis.

	@param xAxis X axis of the basis coordinate frame.
	@param yAxis Y axis of the basis coordinate frame.
	@param zAxis Z axis of the basis coordinate frame.

	@return Reference to the matrix itself.
*/
CRuMatrix4x4 &CRuMatrix4x4::ChangeInverseBasis(const CRuVector3 &xAxis, const CRuVector3 &yAxis, const CRuVector3 &zAxis)
{
	_11 = xAxis.x;	_12 = xAxis.y;	_13 = xAxis.z;	_14 = 0.0f;
	_21 = yAxis.x;	_22 = yAxis.y;	_23 = yAxis.z;	_24 = 0.0f;
	_31 = zAxis.x;	_32 = zAxis.y;	_33 = zAxis.z;	_34 = 0.0f;
	_41 = 0.0f;		_42 = 0.0f;		_43 = 0.0f;		_44 = 1.0f;

	// Copy values of the original matrix into tempMat
	CRuMatrix4x4 tempMat = *this;

	_11 = (tempMat._11 * xAxis.x) + (tempMat._12 * xAxis.x) + (tempMat._13 * xAxis.x);
	_12 = (tempMat._11 * yAxis.y) + (tempMat._12 * yAxis.y) + (tempMat._13 * yAxis.y);
	_13 = (tempMat._11 * zAxis.z) + (tempMat._12 * zAxis.z) + (tempMat._13 * zAxis.z);

	_21 = (tempMat._21 * xAxis.x) + (tempMat._22 * xAxis.x) + (tempMat._23 * xAxis.x);
	_22 = (tempMat._21 * yAxis.y) + (tempMat._22 * yAxis.y) + (tempMat._23 * yAxis.y);
	_23 = (tempMat._21 * zAxis.z) + (tempMat._22 * zAxis.z) + (tempMat._23 * zAxis.z);

	_31 = (tempMat._31 * xAxis.x) + (tempMat._32 * xAxis.x) + (tempMat._33 * xAxis.x);
	_32 = (tempMat._31 * yAxis.y) + (tempMat._32 * yAxis.y) + (tempMat._33 * yAxis.y);
	_33 = (tempMat._31 * zAxis.z) + (tempMat._32 * zAxis.z) + (tempMat._33 * zAxis.z);

	_41 = (tempMat._41 * xAxis.x) + (tempMat._42 * xAxis.x) + (tempMat._43 * xAxis.x);
	_42 = (tempMat._41 * yAxis.y) + (tempMat._42 * yAxis.y) + (tempMat._43 * yAxis.y);
	_43 = (tempMat._41 * zAxis.z) + (tempMat._42 * zAxis.z) + (tempMat._43 * zAxis.z);

	// Return reference to self
	return *this;
}

/*!
	Orthonormalizes the upper 3x3 part of the matrix.

	@return Reference to the matrix itself.
*/
CRuMatrix4x4 &CRuMatrix4x4::Orthonormalize()
{
	// Algorithm uses Gram-Schmidt orthogonalization. If 'this' matrix is
	// M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
	//
	// q0 = m0/|m0|
	// q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
	// q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
	//
	// where |V| indicates length of vector V and A*B indicates dot
	// product of vectors A and B.

	// compute q0
	REAL fInvLength = RuInvSqrt(m_elements[0][0] * m_elements[0][0] + m_elements[1][0] * m_elements[1][0] + m_elements[2][0] * m_elements[2][0]);

	m_elements[0][0] *= fInvLength;
	m_elements[1][0] *= fInvLength;
	m_elements[2][0] *= fInvLength;

	// compute q1
	REAL fDot0 = m_elements[0][0] * m_elements[0][1] + m_elements[1][0] * m_elements[1][1] + m_elements[2][0] * m_elements[2][1];

	m_elements[0][1] -= fDot0 * m_elements[0][0];
	m_elements[1][1] -= fDot0 * m_elements[1][0];
	m_elements[2][1] -= fDot0 * m_elements[2][0];

	fInvLength = RuInvSqrt(m_elements[0][1] * m_elements[0][1] + m_elements[1][1] * m_elements[1][1] + m_elements[2][1] * m_elements[2][1]);

	m_elements[0][1] *= fInvLength;
	m_elements[1][1] *= fInvLength;
	m_elements[2][1] *= fInvLength;

	// compute q2
	REAL fDot1 = m_elements[0][1] * m_elements[0][2] + m_elements[1][1] * m_elements[1][2] + m_elements[2][1] * m_elements[2][2];

	fDot0 = m_elements[0][0] * m_elements[0][2] + m_elements[1][0] * m_elements[1][2] + m_elements[2][0] * m_elements[2][2];

	m_elements[0][2] -= fDot0 * m_elements[0][0] + fDot1 * m_elements[0][1];
	m_elements[1][2] -= fDot0 * m_elements[1][0] + fDot1 * m_elements[1][1];
	m_elements[2][2] -= fDot0 * m_elements[2][0] + fDot1 * m_elements[2][1];

	fInvLength = RuInvSqrt(m_elements[0][2] * m_elements[0][2] + m_elements[1][2] * m_elements[1][2] + m_elements[2][2] * m_elements[2][2]);

	m_elements[0][2] *= fInvLength;
	m_elements[1][2] *= fInvLength;
	m_elements[2][2] *= fInvLength;

	// Return reference to self
	return *this;
}

CRuVector3 &CRuMatrix4x4::TransformPoint(CRuVector3 &point) const
{
	REAL x = point.x, y = point.y, z = point.z;

	point.x = x * _11 + y * _21 + z * _31 + _41;
	point.y = x * _12 + y * _22 + z * _32 + _42;
	point.z = x * _13 + y * _23 + z * _33 + _43;

	return point;
}

CRuVector3 &CRuMatrix4x4::TransformPoint(const CRuVector3 &point, CRuVector3 &pointOut) const
{
	pointOut.x = point.x * _11 + point.y * _21 + point.z * _31 + _41;
	pointOut.y = point.x * _12 + point.y * _22 + point.z * _32 + _42;
	pointOut.z = point.x * _13 + point.y * _23 + point.z * _33 + _43;

	return pointOut;
}

CRuVector4 &CRuMatrix4x4::TransformPoint(const CRuVector3 &point, CRuVector4 &pointOut) const
{
	pointOut.x = point.x * _11 + point.y * _21 + point.z * _31 + _41;
	pointOut.y = point.x * _12 + point.y * _22 + point.z * _32 + _42;
	pointOut.z = point.x * _13 + point.y * _23 + point.z * _33 + _43;
	pointOut.w = point.x * _14 + point.y * _24 + point.z * _34 + _44;

	return pointOut;
}

CRuVector3 &CRuMatrix4x4::TransformPointProjected(CRuVector3 &point) const
{
	REAL x = point.x, y = point.y, z = point.z, w;

	w = x * _14 + y * _24 + z * _34 + _44;
	point.x = (x * _11 + y * _21 + z * _31 + _41) / w;
	point.y = (x * _12 + y * _22 + z * _32 + _42) / w;
	point.z = (x * _13 + y * _23 + z * _33 + _43) / w;

	return point;
}

CRuVector3 &CRuMatrix4x4::TransformPointProjected(const CRuVector3 &point, CRuVector3 &pointOut) const
{
	// Calculate w-component
	REAL w = point.x * _14 + point.y * _24 + point.z * _34 + _44;

	pointOut.x = (point.x * _11 + point.y * _21 + point.z * _31 + _41) / w;
	pointOut.y = (point.x * _12 + point.y * _22 + point.z * _32 + _42) / w;
	pointOut.z = (point.x * _13 + point.y * _23 + point.z * _33 + _43) / w;

	return pointOut;
}

CRuVector3 &CRuMatrix4x4::TransformVector(CRuVector3 &vec) const
{
	REAL x = vec.x, y = vec.y, z = vec.z;

	vec.x = x * _11 + y * _21 + z * _31;
	vec.y = x * _12 + y * _22 + z * _32;
	vec.z = x * _13 + y * _23 + z * _33;

	return vec;
}

CRuVector3 &CRuMatrix4x4::TransformVector(const CRuVector3 &vec, CRuVector3 &vecOut) const
{
	vecOut.x = vec.x * _11 + vec.y * _21 + vec.z * _31;
	vecOut.y = vec.x * _12 + vec.y * _22 + vec.z * _32;
	vecOut.z = vec.x * _13 + vec.y * _23 + vec.z * _33;

	return vecOut;
}

CRuVector3 &CRuMatrix4x4::TransformVectorProjected(CRuVector3 &vec) const
{
	REAL x = vec.x, y = vec.y, z = vec.z;
	REAL w = x * _14 + y * _24 + z * _34 + _44;

	vec.x = (x * _11 + y * _21 + z * _31) / w;
	vec.y = (x * _12 + y * _22 + z * _32) / w;
	vec.z = (x * _13 + y * _23 + z * _33) / w;

	return vec;
}

CRuVector3 &CRuMatrix4x4::TransformVectorProjected(const CRuVector3 &vec, CRuVector3 &vecOut) const
{
	// Calculate w-component
	REAL w = vec.x * _14 + vec.y * _24 + vec.z * _34 + _44;

	vecOut.x = vec.x * _11 + vec.y * _21 + vec.z * _31 / w;
	vecOut.y = vec.x * _12 + vec.y * _22 + vec.z * _32 / w;
	vecOut.z = vec.x * _13 + vec.y * _23 + vec.z * _33 / w;

	return vecOut;
}

CRuMatrix4x4 CRuMatrix4x4::GetTranspose() const
{
	CRuMatrix4x4 tMatrix;
	tMatrix._11 = _11; tMatrix._12 = _21; tMatrix._13 = _31; tMatrix._14 = _41;
	tMatrix._21 = _12; tMatrix._22 = _22; tMatrix._23 = _32; tMatrix._24 = _42;
	tMatrix._31 = _13; tMatrix._32 = _23; tMatrix._33 = _33; tMatrix._34 = _43;
	tMatrix._41 = _14; tMatrix._42 = _24; tMatrix._43 = _34; tMatrix._44 = _44;
	return tMatrix;
}

CRuMatrix4x4 CRuMatrix4x4::GetInverse() const
{
	CRuMatrix4x4 workMat = *this;

	RuSSE_Matrix4x4Invert((REAL *) workMat.m_elements);

	return workMat;
}

CRuMatrix4x4 CRuMatrix4x4::GetInverse_Gaussian() const
{
	CRuMatrix4x4 invMat = *this;
	CRuMatrix4x4 workMat;
	workMat.SetToIdentity();

	// Perform standard Gaussian elimination to obtain inverse
	for(INT32 j = 0; j < 4; ++j)
	{
		// Find a row to work with
		INT32 i;
		for(i = j; i < 4; ++i)
		{
			if(invMat.m_elements[i][j] > ruEPSILON || invMat.m_elements[i][j] < -ruEPSILON)
				break;
		}

		// Cannot invert
		if(i >= 4)
		{
			assert(FALSE);
			break;
		}

		// i != j, swap rows i and j
		if(i != j)
		{
			for(INT32 k = 0; k < 4; ++k)
			{
				REAL temp = invMat.m_elements[i][k];
				invMat.m_elements[i][k] = invMat.m_elements[j][k];
				invMat.m_elements[j][k] = temp;
				temp = workMat.m_elements[i][k];
				workMat.m_elements[i][k] = workMat.m_elements[j][k];
				workMat.m_elements[j][k] = temp;
			}
		}

		// Multiply row j by 1 / M(jj)
		REAL Mjj = invMat.m_elements[j][j];
		for(INT32 k = 0; k < 4; ++k)
		{
			invMat.m_elements[j][k] = invMat.m_elements[j][k] / Mjj;
			workMat.m_elements[j][k] = workMat.m_elements[j][k] / Mjj;
		}

		for(INT32 r = 0; r < 4; ++r)
		{
			if(r != j)
			{
				REAL negMrj = -invMat.m_elements[r][j];
                
				for(INT32 k = 0; k < 4; ++k)
				{
					invMat.m_elements[r][k] = negMrj * invMat.m_elements[j][k] + invMat.m_elements[r][k];
					workMat.m_elements[r][k] = negMrj * workMat.m_elements[j][k] + workMat.m_elements[r][k];
				}
			}
		}
	}

	return workMat;
}

REAL CRuMatrix4x4::Get3x3Determinant(int i, int j) const
{
	REAL det;
	REAL subMatrix[3][3];

	for(int row = 0, y = 0;row < 4;row++)
	{
		if(row != i)
		{
			for(int col = 0, x = 0;col < 4;col++)
			{
				if(col != j)
					subMatrix[y][x++] = m_elements[row][col];
			}
			y++;
		}
	}

	det = subMatrix[0][0] * (subMatrix[1][1] * subMatrix[2][2] - subMatrix[1][2] * subMatrix[2][1])
		- subMatrix[0][1] * (subMatrix[1][0] * subMatrix[2][2] - subMatrix[1][2] * subMatrix[2][0])
		- subMatrix[0][2] * (subMatrix[1][0] * subMatrix[2][1] - subMatrix[1][1] * subMatrix[2][0]);

	return det;
}

CRuMatrix4x4 CRuMatrix4x4::GetOrthogonalInverse() const
{
	CRuMatrix4x4 invMat;

	// Invert the 3x3 rotation matrix
	invMat._11 = _11; invMat._12 = _21; invMat._13 = _31; invMat._14 = 0.0f;
	invMat._21 = _12; invMat._22 = _22; invMat._23 = _32; invMat._24 = 0.0f;
	invMat._31 = _13; invMat._32 = _23; invMat._33 = _33; invMat._34 = 0.0f;
	invMat._44 = 1.0f;

    // Set translation element
	invMat._41 = invMat._11 * -_41 + invMat._21 * -_42 + invMat._31 * -_43;
	invMat._42 = invMat._12 * -_41 + invMat._22 * -_42 + invMat._32 * -_43;
	invMat._43 = invMat._13 * -_41 + invMat._23 * -_42 + invMat._33 * -_43;

	return invMat;
}

BOOL CRuMatrix4x4::GetDecomposition(CRuVector3 &scaleOut, CRuVector3 &shearOut, CRuVector3 &rotationOut, CRuVector3 &translationOut) const
{
	CRuMatrix4x4 localMatrix = *this;

	// Normalize the matrix
	if(fabs(localMatrix.m_elements[3][3]) <= ruEPSILON)
	{
		return FALSE;
	}

	for(INT32 i = 0; i < 4; ++i)
	{
		for(INT32 j = 0; j < 4; ++j)
		{
			localMatrix.m_elements[i][j] = localMatrix.m_elements[i][j] / localMatrix.m_elements[3][3];
		}
	}

	// Extract translation component
	for(INT32 i = 0; i < 3; ++i)
	{
		translationOut[i] = localMatrix.m_elements[3][i];
		localMatrix.m_elements[3][i] = 0.0f;
	}

	// Get scale and shear components
	CRuVector3 rows[3];

	for(INT32 i = 0; i < 3; ++i)
	{
		rows[i].Set(localMatrix.m_elements[i][0], localMatrix.m_elements[i][1], localMatrix.m_elements[i][2]);
	}

	// Compute X scale factor and normalize first row
	scaleOut.x = rows[0].Magnitude();

	if(scaleOut.x <= ruEPSILON)
	{
		return FALSE;
	}

	rows[0].Normalize();

	// Compute XY shear factor and make second row orthogonal to first row
	shearOut[0] = rows[0].Dot(rows[1]);
	rows[1] = rows[1] - (rows[0] * shearOut[0]);

	// Compute Y scale and normalize second row
	scaleOut.y = rows[1].Magnitude();

	if(scaleOut.y <= ruEPSILON)
	{
		return FALSE;
	}

	rows[1].Normalize();
	shearOut[0] = shearOut[0] / scaleOut.y;

	// Compute XZ and YZ shears, and orthogonalize third row
	shearOut[1] = rows[0].Dot(rows[1]);
	rows[2] = rows[2] - (rows[0] * shearOut[1]);

	shearOut[2] = rows[1].Dot(rows[2]);
	rows[2] = rows[2] - (rows[1] * shearOut[2]);

	// Get Z scale and normalize third row
	scaleOut.z = rows[2].Magnitude();

	if(scaleOut.z <= ruEPSILON)
	{
		return FALSE;
	}

	rows[2].Normalize();

	shearOut[1] = shearOut[1] / scaleOut.z;
	shearOut[2] = shearOut[2] / scaleOut.z;

	// At this point, the matrix in rows[] is orthonormal.
	// Check for a coordinate system flip. If the determinant
	// is -1, then negate the matrix and the scaling factor.

	if(rows[0].Dot(CrossProduct(rows[1], rows[2])) < 0.0f)
	{
		for(INT32 i = 0; i < 3; ++i)
		{
			scaleOut[i] = scaleOut[i] * -1.0f;
			rows[i] = rows[i] * -1.0f;
		}
	}

	// Get the rotations out
	rotationOut.y = asin(-rows[0][2]);

	// Numerical instabilities -- check the rows for 0, 0 parameters
	if(cos(rotationOut.y) != 0.0f)
	{
		rotationOut.x = atan2(rows[1][2], rows[2][2]);
		rotationOut.z = atan2(rows[0][1], rows[0][0]);
	}
	else
	{
		rotationOut.x = atan2(rows[1][0], rows[1][1]);
		rotationOut.z = 0.0f;
	}

	return TRUE;
}

CRuMatrix4x4 &CRuMatrix4x4::operator=(const CRuMatrix4x4 &mat)
{
	memcpy(m_elements, mat.m_elements, sizeof(REAL) * 16);

	return *this;
}

CRuMatrix4x4 CRuMatrix4x4::operator*(const CRuMatrix4x4 &mat) const
{
	CRuMatrix4x4 result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.elements[i][j] = (elements[i][0] * mat.elements[0][j]) + (elements[i][1] * mat.elements[1][j]) + (elements[i][2] * mat.elements[2][j]) + (elements[i][3] * mat.elements[3][j]);
		}
	}		
	return result;
}

CRuMatrix4x4 CRuMatrix4x4::operator*(const REAL multiplier) const
{
	CRuMatrix4x4 result;
	int i, j;
	for(i = 0;i < 4;i++)
	{
		for(j = 0;j < 4;j++)
		{
			result.m_elements[i][j] = m_elements[i][j] * multiplier;
		}
	}
	return result;
}

CRuMatrix4x4 CRuMatrix4x4::operator/(const REAL divisor) const
{
	CRuMatrix4x4 result;
	int i, j;
	for(i = 0;i < 4;i++)
	{
		for(j = 0;j < 4;j++)
		{
			result.m_elements[i][j] = m_elements[i][j] / divisor;
		}
	}
	return result;
}

BOOL CRuMatrix4x4::operator==(const CRuMatrix4x4 &mat) const
{
	for(INT32 i = 0; i < 4; ++i)
	{
		for(INT32 j = 0; j < 4; ++j)
		{
			// If the elements at (i, j) are not equal, return FALSE
			if(m_elements[i][j] != mat.m_elements[i][j])
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}







void CRuMatrix4x4::SetTranslation(REAL x, REAL y, REAL z)
{
	_41 = x;
	_42 = y;
	_43 = z;
}

D3DMATRIX CRuMatrix4x4::GetD3DMatrix() const
{
	D3DMATRIX d3dMat;
	d3dMat._11 = _11; d3dMat._12 = _12; d3dMat._13 = _13; d3dMat._14 = _14;
	d3dMat._21 = _21; d3dMat._22 = _22; d3dMat._23 = _23; d3dMat._24 = _24;
	d3dMat._31 = _31; d3dMat._32 = _32; d3dMat._33 = _33; d3dMat._34 = _34;
	d3dMat._41 = _41; d3dMat._42 = _42; d3dMat._43 = _43; d3dMat._44 = _44;
	return d3dMat;
}


// ************************************************************************************************************************************************************

CRuQuaternion::CRuQuaternion(REAL x, REAL y, REAL z)
{
	MakeEulerRotation(x, y, z);
}

CRuQuaternion CRuQuaternion::GetInverse() const
{
	REAL norm = x * x + y * y + z * z + w * w;

	return CRuQuaternion(-x / norm, -y / norm, -z / norm, w / norm);
}

CRuQuaternion CRuQuaternion::GetConjugate() const
{
	// ASSUME QUATERNION IS UNIT LENGTH
	return CRuQuaternion(-x, -y, -z, w);
}

CRuQuaternion CRuQuaternion::GetExp() const
{
	// If q = A*(x*i + y*j + z*k) where (x, y, z) is unit length, then
	// exp(q) = cos(A) + sin(A) * (x*i + y*j + z*k).  If sin(A) is near zero,
	// use exp(q) = cos(A) + A * (x*i + y*j + z*k) since A / sin(A) has limit 1.

	REAL angle = (REAL) RuSqrt(m_x * m_x + m_y * m_y + m_z * m_z);
	REAL sine = (REAL) sin(angle);

	if(fabs(sine) >= ruEPSILON)
	{
		REAL coeff = sine / angle;
		return CRuQuaternion(m_x * coeff, m_y * coeff, m_z * coeff, (REAL) cos(angle));
	}
	else
		return CRuQuaternion(m_x, m_y, m_z, (REAL) cos(angle));
}

CRuQuaternion CRuQuaternion::GetLog() const
{
	// If q = cos(A) + sin(A) * (x*i + y*j + z*k) where (x, y, z) is unit length, then
	// log(q) = A * (x*i + y*j + z*k).  If sin(A) is near zero, use log(q) =
	// sin(A) * (x*i + y*j + z*k) since sin(A) / A has limit 1.

	if(fabs(m_w) < 1.0f)
	{
		REAL angle = (REAL) acos(m_w);
		REAL sine = (REAL) sin(angle);

		if(fabs(sine) >= ruEPSILON)
		{
			REAL coeff = angle / sine;
			return CRuQuaternion(m_x * coeff, m_y * coeff, m_z * coeff, 0.0f);
		}
	}

	return CRuQuaternion(m_x, m_y, m_z, 0.0f);
}

BOOL CRuQuaternion::FromAngleAxis(const CRuVector3 &axisOfRotation, REAL rotationAngle)
{
	// ASSUME AXIS IS UNIT LENGTH
	REAL halfAngle = rotationAngle / 2.0f;
	REAL sine = (REAL) sin(halfAngle);

	// Set components
	m_x = axisOfRotation.x * sine;
	m_y = axisOfRotation.y * sine;
	m_z = axisOfRotation.z * sine;
	m_w = (REAL) cos(halfAngle);

	return TRUE;
}

BOOL CRuQuaternion::ToAngleAxis(CRuVector3 &axisOfRotationOut, REAL &rotationAngleOut)
{
	REAL vMag = RuSqrt(1.0f - m_w * m_w);

	if(vMag > ruEPSILON)
	{
		axisOfRotationOut = CRuVector3(m_x, m_y, m_z) / vMag;
		rotationAngleOut = 2.0f * acos(m_w);
	}
	else
	{
		axisOfRotationOut.Set(0.0f, 1.0f, 0.0f);
		rotationAngleOut = 0.0f;
	}

	return TRUE;
}

BOOL CRuQuaternion::FromRotationMatrix(const CRuMatrix4x4 &matrix)
{

	return TRUE;
}

BOOL CRuQuaternion::MakeEulerRotation(REAL x, REAL y, REAL z)
{
	return MakeEulerRotation(CRuVector3(x, y, z));
}

BOOL CRuQuaternion::MakeEulerRotation(CRuVector3 rotAngles)
{
	// Invert the rotations, since Euler rotations are clockwise looking down the
	// positive axis while Quaternion rotations are counter-clockwise.
	rotAngles.x = -rotAngles.x * 0.5f;
	rotAngles.y = -rotAngles.y * 0.5f;
	rotAngles.z = -rotAngles.z * 0.5f;
/*
//	-- this section is bad code pulled from a sample... rotations are not the same as erotAngles.xpected
	double sr, sp, sy, cr, cp, cy;

	sr = sin(rotAngles.x);
	cr = cos(rotAngles.x);
	sp = sin(rotAngles.y);
	cp = cos(rotAngles.y);
	sy = sin(rotAngles.z);
	cy = cos(rotAngles.z);

	double crcp = cr * cp;
	double srsp = sr * sp;

	x = (REAL) (sr * cp * cy - cr * sp * sy);
	y = (REAL) (cr * sp * cy + sr * cp * sy);
	z = (REAL) (crcp * sy - srsp * cy);
	w = (REAL) (crcp * cy + srsp * sy); 
*/

	// Can optimirotAngles.ze this brotAngles.y erotAngles.xpanding the quaternion multiplications
	CRuQuaternion quat1((REAL) sin(rotAngles.x), 0.0f, 0.0f, (REAL) cos(rotAngles.x));
	CRuQuaternion quat2(0.0f, (REAL) sin(rotAngles.y), 0.0f, (REAL) cos(rotAngles.y));
	CRuQuaternion quat3(0.0f, 0.0f, (REAL) sin(rotAngles.z), (REAL) cos(rotAngles.z));
	*this = quat1 * quat2 * quat3;

	return TRUE;
}

BOOL CRuQuaternion::RotateIntoVector(CRuVector3 targetAxis, CRuVector3 sourceAxis)
{
	targetAxis.Normalize();
	sourceAxis.Normalize();

	CRuVector3 sXt = CrossProduct(sourceAxis, targetAxis);
	REAL sDt = DotProduct(sourceAxis, targetAxis);
	REAL sXtMag = sXt.Magnitude();
	CRuVector3 u = sXt / sXtMag;

	// Clamp sDt to [-1, 1]
	sDt = max(-1.0f, sDt);
	sDt = min(1.0f, sDt);

	// If the two axis are near parallel, handle the rotation differently
	if(sXtMag < ruEPSILON)
	{
		// If dot product is greater than 0, then the two axis are parallel in the same direction, so do not rotate
		if(sDt > 0.0f)
		{
			FromAngleAxis(CRuVector3(0.0f, 1.0f, 0.0f), 0.0f);
		}
		else
		{
			// Find the rotation axis by multiplying sourceAxis with an arbitrary basis vec
			CRuVector3 newRotAxis = CrossProduct(sourceAxis, CRuVector3(0.0f, 1.0f, 0.0f));
			if(newRotAxis.Magnitude() < ruEPSILON)
				newRotAxis = CrossProduct(sourceAxis, CRuVector3(1.0f, 0.0f, 0.0f));

			FromAngleAxis(newRotAxis, ruPI);
		}

		return TRUE;
	}

	// Use half-angle rule to retrieve cos(phi) and sin(phi)
	REAL sinPhi = (REAL) RuSqrt((1.0f - sDt) / 2.0f);
	REAL cosPhi = (REAL) RuSqrt((1.0f + sDt) / 2.0f);

	// --- Quaternion for left-handed coordinate systems ---

	// Invert the rotation angle(left-handed coordinate system)
	x = sinPhi * u.x;
	y = sinPhi * u.y;
	z = sinPhi * u.z;
	w = -cosPhi;

	// --- Quaternion for right-handed coordinate systems ---
/*
	x = sinPhi * u.x;
	y = sinPhi * u.y;
	z = sinPhi * u.z;
	w = cosPhi;
*/

	return TRUE;
}

CRuQuaternion &CRuQuaternion::RotateIntoVector(CRuVector3 targetAxis, CRuVector3 sourceAxis, const CRuVector3 &constraintAxis)
{
	CRuVector3 s, t;

	s.SetToCrossProduct(targetAxis, constraintAxis);
	t.SetToCrossProduct(constraintAxis, s);
	t.Normalize();

	REAL rotAngle = atan2(CrossProduct(sourceAxis, t).Magnitude(), DotProduct(sourceAxis, t));

	// Build rotation matrix
	if(DotProduct(CrossProduct(sourceAxis, t), constraintAxis) < 0.0f)
	{
		this->FromAngleAxis(constraintAxis, rotAngle);
	}
	else
	{
		this->FromAngleAxis(constraintAxis, -rotAngle);
	}

	return *this;
}

CRuQuaternion &CRuQuaternion::SetToAxialBillboard(const CRuMatrix4x4 &viewTransform, const CRuVector3 &geometryForward, const CRuVector3 &constraintAxis)
{
	CRuVector3 invViewDir, viewUp, ivXd;
	CRuVector3 s, t;

	// Invert camera forward vector
	invViewDir.Set(viewTransform._13, viewTransform._23, viewTransform._33);
	invViewDir.x = -invViewDir.x;
	invViewDir.y = -invViewDir.y;
	invViewDir.z = -invViewDir.z;

	s.SetToCrossProduct(invViewDir, constraintAxis);
	t.SetToCrossProduct(constraintAxis, s);
	t.Normalize();

	REAL rotAngle = atan2(CrossProduct(geometryForward, t).Magnitude(), DotProduct(geometryForward, t));

	// Build rotation matrix
	if(DotProduct(CrossProduct(geometryForward, t), constraintAxis) > 0.0f)
	{
		this->FromAngleAxis(constraintAxis, rotAngle);
	}
	else
	{
		this->FromAngleAxis(constraintAxis, -rotAngle);
	}

	return *this;
}

void CRuQuaternion::GetAngleAxis(CRuVector3 &axis, REAL &angle) const
{
	if(w >= 1.0f || w <= -1.0f)
	{
		axis.x = 0.0f;
		axis.y = 1.0f;
		axis.z = 0.0f;
		angle = 0;
		return;
	}

	REAL denominator = (REAL) RuSqrt(1.0f - w * w);
	angle = 2.0f * (REAL) acos(w);
	axis.x = x / denominator;
	axis.y = y / denominator;
	axis.z = z / denominator;
}

CRuMatrix4x4 CRuQuaternion::GetMatrix() const
{
	CRuMatrix4x4 quatMatrix;
	REAL x2, y2, w2, xy2, xz2, yz2, wx2, wy2, wz2, xx2, yy2, zz2;

	// Pre-calculate needed temporaries
	x2 = 2.0f * x;
	y2 = 2.0f * y;
	w2 = 2.0f * w;
	xy2 = x2 * y;
	xz2 = x2 * z;
	yz2 = y2 * z;
	wx2 = w2 * x;
	wy2 = w2 * y;
	wz2 = w2 * z;
	xx2 = x2 * x;
	yy2 = y2 * y;
	zz2 = 2.0f * z * z;

	// --- Assignment Order for Row-Major Matrices ---

	quatMatrix._11 = 1.0f - yy2 - zz2;
	quatMatrix._21 = xy2 + wz2;
	quatMatrix._31 = xz2 - wy2;
	quatMatrix._41 = 0.0f;

	quatMatrix._12 = xy2 - wz2;
	quatMatrix._22 = 1.0f - xx2 - zz2;
	quatMatrix._32 = yz2 + wx2;
	quatMatrix._42 = 0.0f;

	quatMatrix._13 = xz2 + wy2;
	quatMatrix._23 = yz2 - wx2;
	quatMatrix._33 = 1.0f - xx2 - yy2;
	quatMatrix._43 = 0.0f;

	quatMatrix._14 = 0.0f;
	quatMatrix._24 = 0.0f;
	quatMatrix._34 = 0.0f;
	quatMatrix._44 = 1.0f;

	return quatMatrix;
}

void CRuQuaternion::GetMatrix(CRuMatrix4x4 &matOut) const
{
	REAL x2, y2, w2, xy2, xz2, yz2, wx2, wy2, wz2, xx2, yy2, zz2;

	// Pre-calculate needed temporaries
	x2 = 2.0f * x;
	y2 = 2.0f * y;
	w2 = 2.0f * w;
	xy2 = x2 * y;
	xz2 = x2 * z;
	yz2 = y2 * z;
	wx2 = w2 * x;
	wy2 = w2 * y;
	wz2 = w2 * z;
	xx2 = x2 * x;
	yy2 = y2 * y;
	zz2 = 2.0f * z * z;

	// --- Assignment Order for Row-Major Matrices ---

	matOut._11 = 1.0f - yy2 - zz2;
	matOut._21 = xy2 + wz2;
	matOut._31 = xz2 - wy2;
	matOut._41 = 0.0f;

	matOut._12 = xy2 - wz2;
	matOut._22 = 1.0f - xx2 - zz2;
	matOut._32 = yz2 + wx2;
	matOut._42 = 0.0f;

	matOut._13 = xz2 + wy2;
	matOut._23 = yz2 - wx2;
	matOut._33 = 1.0f - xx2 - yy2;
	matOut._43 = 0.0f;

	matOut._14 = 0.0f;
	matOut._24 = 0.0f;
	matOut._34 = 0.0f;
	matOut._44 = 1.0f;
}

CRuVector3 &CRuQuaternion::TransformVector(CRuVector3 &vec) const
{
	REAL vMult = 2.0f * (m_x * vec.m_x + m_y * vec.m_y + m_z * vec.m_z);
	REAL crossMult = 2.0f * m_w;
	REAL pMult = crossMult * w - 1.0f;

	/*
		In Essential Mathematics for Games & Interactive Applications, the formula is given as:

			V' = (2 * W^2 - 1) * P + 2 * [(V . P) * V] + [(V x P) * W]

		While this form is correct, it is given in the right-handed orientation. For this
		to work, the cross product must be reversed. Thus, the following is used:

			V' = (2 * W^2 - 1) * P + 2 * [(V . P) * V] + [(P x V) * W]
	*/

	CRuVector3 origVec = vec;

	vec.m_x = pMult * origVec.m_x + vMult * m_x + crossMult * (origVec.m_y * m_z - origVec.m_z * m_y);
	vec.m_y = pMult * origVec.m_y + vMult * m_y + crossMult * (origVec.m_z * m_x - origVec.m_x * m_z);
	vec.m_z = pMult * origVec.m_z + vMult * m_z + crossMult * (origVec.m_x * m_y - origVec.m_y * m_x);

	return vec;
}

CRuVector3 &CRuQuaternion::TransformVector(const CRuVector3 &vec, CRuVector3 &vecOut) const
{
	REAL vMult = 2.0f * (m_x * vec.m_x + m_y * vec.m_y + m_z * vec.m_z);
	REAL crossMult = 2.0f * m_w;
	REAL pMult = crossMult * w - 1.0f;

	CRuVector3 resultVec = vec;

	resultVec.m_x = pMult * vec.m_x + vMult * m_x + crossMult * (vec.m_y * m_z - vec.m_z * m_y);
	resultVec.m_y = pMult * vec.m_y + vMult * m_y + crossMult * (vec.m_z * m_x - vec.m_x * m_z);
	resultVec.m_z = pMult * vec.m_z + vMult * m_z + crossMult * (vec.m_x * m_y - vec.m_y * m_x);

	vecOut = resultVec;

	return vecOut;
}

CRuQuaternion CRuQuaternion::operator*(const CRuQuaternion &quat) const
{
	return CRuQuaternion(	m_w * quat.m_x + m_x * quat.m_w + m_y * quat.m_z - m_z * quat.m_y,
							m_w * quat.m_y + m_y * quat.m_w + m_z * quat.m_x - m_x * quat.m_z,
							m_w * quat.m_z + m_z * quat.m_w + m_x * quat.m_y - m_y * quat.m_x,
							m_w * quat.m_w - m_x * quat.m_x - m_y * quat.m_y - m_z * quat.m_z);
}

CRuVector3 CRuQuaternion::operator*(const CRuVector3 &vec) const
{
	REAL pMult = 1.0f - m_w * m_w;
	REAL vMult = 2.0f * (m_x * vec.m_x + m_y * vec.m_y + m_z * vec.m_z);
	REAL crossMult = 2.0f * m_w;

	return CRuVector3(	pMult * vec.m_x + vMult * m_x + crossMult * (m_y * vec.m_z - m_z * vec.m_y),
						pMult * vec.m_y + vMult * m_y + crossMult * (m_z * vec.m_x - m_x * vec.m_z),
						pMult * vec.m_z + vMult * m_z + crossMult * (m_x * vec.m_y - m_y * vec.m_x));
}










void MatrixMultiply(const CRuMatrix4x4 &mat1, const CRuMatrix4x4 &mat2, CRuMatrix4x4 &outMat)
{
	outMat._11 = (mat1._11 * mat2._11) + (mat1._12 * mat2._21) + (mat1._13 * mat2._31) + (mat1._14 * mat2._41);
	outMat._12 = (mat1._11 * mat2._12) + (mat1._12 * mat2._22) + (mat1._13 * mat2._32) + (mat1._14 * mat2._42);
	outMat._13 = (mat1._11 * mat2._13) + (mat1._12 * mat2._23) + (mat1._13 * mat2._33) + (mat1._14 * mat2._43);
	outMat._14 = (mat1._11 * mat2._14) + (mat1._12 * mat2._24) + (mat1._13 * mat2._34) + (mat1._14 * mat2._44);

	outMat._21 = (mat1._21 * mat2._11) + (mat1._22 * mat2._21) + (mat1._23 * mat2._31) + (mat1._24 * mat2._41);
	outMat._22 = (mat1._21 * mat2._12) + (mat1._22 * mat2._22) + (mat1._23 * mat2._32) + (mat1._24 * mat2._42);
	outMat._23 = (mat1._21 * mat2._13) + (mat1._22 * mat2._23) + (mat1._23 * mat2._33) + (mat1._24 * mat2._43);
	outMat._24 = (mat1._21 * mat2._14) + (mat1._22 * mat2._24) + (mat1._23 * mat2._34) + (mat1._24 * mat2._44);

	outMat._31 = (mat1._31 * mat2._11) + (mat1._32 * mat2._21) + (mat1._33 * mat2._31) + (mat1._34 * mat2._41);
	outMat._32 = (mat1._31 * mat2._12) + (mat1._32 * mat2._22) + (mat1._33 * mat2._32) + (mat1._34 * mat2._42);
	outMat._33 = (mat1._31 * mat2._13) + (mat1._32 * mat2._23) + (mat1._33 * mat2._33) + (mat1._34 * mat2._43);
	outMat._34 = (mat1._31 * mat2._14) + (mat1._32 * mat2._24) + (mat1._33 * mat2._34) + (mat1._34 * mat2._44);

	outMat._41 = (mat1._41 * mat2._11) + (mat1._42 * mat2._21) + (mat1._43 * mat2._31) + (mat1._44 * mat2._41);
	outMat._42 = (mat1._41 * mat2._12) + (mat1._42 * mat2._22) + (mat1._43 * mat2._32) + (mat1._44 * mat2._42);
	outMat._43 = (mat1._41 * mat2._13) + (mat1._42 * mat2._23) + (mat1._43 * mat2._33) + (mat1._44 * mat2._43);
	outMat._44 = (mat1._41 * mat2._14) + (mat1._42 * mat2._24) + (mat1._43 * mat2._34) + (mat1._44 * mat2._44);
}

CRuVector3 CrossProduct(const CRuVector3 &vector1, const CRuVector3 &vector2)
{
	CRuVector3 result;
	result.x = vector1.y * vector2.z - vector1.z * vector2.y;
	result.y = vector1.z * vector2.x - vector1.x * vector2.z;
	result.z = vector1.x * vector2.y - vector1.y * vector2.x;
	return result;
}

REAL DotProduct(const CRuVector3 &vector1, const CRuVector3 &vector2)
{
	return vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z;
}

REAL TripleProduct_Scalar(const CRuVector3 &vector1, const CRuVector3 &vector2, const CRuVector3 &u)
{
	return DotProduct(u, CrossProduct(vector1, vector2));
}

REAL TripleProduct_Scalar(const CRuVector3 &vector1, const CRuVector3 &vector2)
{
	CRuVector3 u = CrossProduct(vector1, vector2);

	return DotProduct(u, CrossProduct(vector1, vector2));
}

REAL CalculateAngleDifferential(const CRuVector3 &originVector, const CRuVector3 &targetVector, const CRuVector3 &rotAxis, REAL &rotAngleOut)
{
	REAL dot = DotProduct(originVector, targetVector);

	if(dot > 1.0f - ruEPSILON)
	{
		rotAngleOut = 0.0f;
		return 0.0f;
	}
	else if(dot < -1.0f + ruEPSILON)
	{
		// this aint' quite right
		rotAngleOut = ruPI;
		return ruPI;
	}
	else
	{
		REAL tripleProduct = TripleProduct_Scalar(rotAxis, targetVector, originVector);
		rotAngleOut = acos(dot);

		if(tripleProduct < 0.0f)
		{
			rotAngleOut = -rotAngleOut;
		}
	}

	return rotAngleOut;
}

void VectorTransform(const CRuVector3 &vec, const CRuMatrix4x4 &matrix, CRuVector3 &out)
{
	out.x = vec.x * matrix._11 + vec.y * matrix._21 + vec.z * matrix._31 + matrix._41;
	out.y = vec.x * matrix._12 + vec.y * matrix._22 + vec.z * matrix._32 + matrix._42;
	out.z = vec.x * matrix._13 + vec.y * matrix._23 + vec.z * matrix._33 + matrix._43;
}

void VectorTransform3(const CRuVector3 &vec, const CRuMatrix4x4 &matrix, CRuVector3 &out)
{
	out.x = vec.x * matrix._11 + vec.y * matrix._21 + vec.z * matrix._31;
	out.y = vec.x * matrix._12 + vec.y * matrix._22 + vec.z * matrix._32;
	out.z = vec.x * matrix._13 + vec.y * matrix._23 + vec.z * matrix._33;
}

CRuVector3 operator*(const CRuVector3 &vec, const CRuMatrix4x4 &matrix)
{
	CRuVector3 result;
	result.x = vec.x * matrix._11 + vec.y * matrix._21 + vec.z * matrix._31 + matrix._41;
	result.y = vec.x * matrix._12 + vec.y * matrix._22 + vec.z * matrix._32 + matrix._42;
	result.z = vec.x * matrix._13 + vec.y * matrix._23 + vec.z * matrix._33 + matrix._43;
	return result;
}

CRuPlane4 operator*(const CRuPlane4 &plane, const CRuMatrix4x4 &matrix)
{
	CRuPlane4 result;
	CRuMatrix4x4 invTransposeMatrix = matrix.GetInverse().GetTranspose();

	result.m_a = plane.m_a * invTransposeMatrix._11 + plane.m_b * invTransposeMatrix._21 + plane.m_c * invTransposeMatrix._31 + plane.m_d * invTransposeMatrix._41;
	result.m_b = plane.m_a * invTransposeMatrix._12 + plane.m_b * invTransposeMatrix._22 + plane.m_c * invTransposeMatrix._32 + plane.m_d * invTransposeMatrix._42;
	result.m_c = plane.m_a * invTransposeMatrix._13 + plane.m_b * invTransposeMatrix._23 + plane.m_c * invTransposeMatrix._33 + plane.m_d * invTransposeMatrix._43;
	result.m_d = plane.m_a * invTransposeMatrix._14 + plane.m_b * invTransposeMatrix._24 + plane.m_c * invTransposeMatrix._34 + plane.m_d * invTransposeMatrix._44;

	return result;
}


CRuQuaternion QuaternionSlerp(const CRuQuaternion &from, const CRuQuaternion &to, REAL t)
{
	REAL tol[4];
	REAL omega, cosom, sinom, scale0, scale1;

	// Calculate cosine
	cosom = from.x * to.x + from.y * to.y + from.z * to.z + from.w * to.w;

	// Adjust signs as necessary
	if(cosom < 0.0)
	{
		cosom = -cosom;
		tol[0] = -to.x;
		tol[1] = -to.y;
		tol[2] = -to.z;
		tol[3] = -to.w;
	}
	else
	{
		tol[0] = to.x;
		tol[1] = to.y;
		tol[2] = to.z;
		tol[3] = to.w;
	}

	// Calculate coefficients
	if((1.0f - cosom) > ruEPSILON)
	{
		// Standard case, use SLERP
		omega = (REAL) acos(cosom);
		sinom = (REAL) sin(omega);
		scale0 = (REAL) sin((1.0f - t) * omega) / sinom;
		scale1 = (REAL) sin(t * omega) / sinom;
	}
	else
	{
		// Too close to zero, use linear interpolation
		scale0 = 1.0f - t;
		scale1 = t;
	}

	// Return final quaternion

	return CRuQuaternion(scale0 * from.x + scale1 * tol[0], scale0 * from.y + scale1 * tol[1], scale0 * from.z + scale1 * tol[2], scale0 * from.w + scale1 * tol[3]);
}

CRuQuaternion QuaternionLerp(const CRuQuaternion &from, const CRuQuaternion &to, REAL t)
{
	REAL t0 = 1.0f - t, t1 = t;

	// Calculate cosine
	REAL cosom = from.x * to.x + from.y * to.y + from.z * to.z + from.w * to.w;

	if(cosom < 0.0)
		return CRuQuaternion(from.x * t0 + to.x * -t1, from.y * t0 + to.y * -t1, from.z * t0 + to.z * -t1, from.w * t0 + to.w * -t1);
	else
		return CRuQuaternion(from.x * t0 + to.x * t1, from.y * t0 + to.y * t1, from.z * t0 + to.z * t1, from.w * t0 + to.w * t1);
}

// ************************************************************************************************************************************************************

#pragma managed
