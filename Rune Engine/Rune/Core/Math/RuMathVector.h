#ifndef _RUMATHVECTOR_H_
#define _RUMATHVECTOR_H_

// Direct3D includes
#include <d3d9.h>
#include <d3dx9.h>

#include "../../Core/Math/RuMathBase.h"

#pragma unmanaged

#pragma pack(push, 16) /* Must ensure class & union 16-B aligned */

// ************************************************************************************************************************************************************

class CRuQuaternion;

// ************************************************************************************************************************************************************

class CRuVector2
{
public:
	union
	{
		struct
		{
			union
			{
				REAL m_x;
				REAL x;
			};
			union
			{
				REAL m_y;
				REAL y;
			};
		};

		REAL m_elements[2];
		REAL elements[2];
	};

public:
	inline							CRuVector2() : m_x(0.0f), m_y(0.0f) { }
	inline							CRuVector2(REAL newX, REAL newY) : m_x(newX), m_y(newY) { }
	inline							CRuVector2(const CRuVector2 &vec) : m_x(vec.x), m_y(vec.y) { }

	// Set operations
	inline void						Set(const CRuVector2 &vec) { x = vec.x; y = vec.y; }
	inline void						Set(REAL newX, REAL newY) { x = newX;	y = newY; }

	// Arithmetic operations
	inline REAL						SquaredMagnitude() const { return (x * x) + (y * y); }
	inline REAL						Magnitude() const { return (REAL) RuSqrt(x * x + y * y); }
	inline REAL						Distance(const CRuVector2 &vec) const { return (REAL) RuSqrt((vec.x - x) * (vec.x - x) + (vec.y - y) * (vec.y - y)); }
	inline REAL						Cross(const CRuVector2 &vec) const { return x * vec.y - y * vec.x; }
	inline REAL						Dot(const CRuVector2 &vec) const { return x * vec.x + y * vec.y; }

	// Arithmetic operator overloads
	inline CRuVector2				operator+(const CRuVector2 &vec) const { return CRuVector2(x + vec.x, y + vec.y); }
	inline CRuVector2				operator-(const CRuVector2 &vec) const { return CRuVector2(x - vec.x, y - vec.y); }
	inline CRuVector2				operator*(const REAL multiplier) const { return CRuVector2(x * multiplier, y * multiplier); }
	inline CRuVector2				operator/(const REAL divisor) const { return CRuVector2(x / divisor, y / divisor); }

	// Comparison operations and operator overloads
	inline BOOL						operator==(const CRuVector2 &vec) const { return (x == vec.x && y == vec.y) ? TRUE : FALSE; }
	inline BOOL						operator!=(const CRuVector2 &vec) const { return (x != vec.x || y != vec.y) ? TRUE : FALSE; }

	// Array-style vector accessors (For convenience and algorithmic clarity only, no error checking!)
	inline REAL&					operator[](const int index) { return m_elements[index]; }
	inline const REAL&				operator[](const int index) const { return m_elements[index]; }
};

// ************************************************************************************************************************************************************

// Standard 3-Component Vector
class CRuVector3
{
public:
	union
	{
		struct
		{
			union
			{
				REAL		m_x;
				REAL		x;
			};
			union
			{
				REAL		m_y;
				REAL		y;
			};
			union
			{
				REAL		m_z;
				REAL		z;
			};
		};

		REAL				m_elements[3];
		REAL				elements[3];
	};

public:
	inline							CRuVector3() : x(0.0f), y(0.0f), z(0.0f) { }
	inline							CRuVector3(REAL newX, REAL newY, REAL newZ) : x(newX), y(newY), z(newZ) { }
	inline							CRuVector3(const CRuVector3 &vec) : x(vec.x), y(vec.y), z(vec.z) { }

	// Set operations
	inline void						Set(const CRuVector3 &vec) { x = vec.x; y = vec.y; z = vec.z; }
	inline void						Set(REAL newX, REAL newY, REAL newZ) { x = newX;	y = newY; z = newZ; }

	// Arithmetic operations
	inline REAL						SquaredMagnitude() const { return (x * x) + (y * y) + (z * z); }
	inline REAL						Magnitude() const { return RuSqrt(x * x + y * y + z * z); }
	REAL							Factor();
	CRuVector3&						Normalize();
	inline void						Add(const CRuVector3 &vec1) { x = x + vec1.x; y = y + vec1.y; z = z + vec1.z; }
	inline void						Subtract(const CRuVector3 &vec1) { x = x - vec1.x; y = y - vec1.y; z = z - vec1.z; }
	inline void						Multiply(REAL scale) { x = x * scale; y = y * scale; z = z * scale; }
	CRuVector3&						Cross(const CRuVector3 &vec);
	inline REAL						Dot(const CRuVector3 &vec) const { return x * vec.x + y * vec.y + z * vec.z; }

	// Arithmetic set operations
	inline void						SetToSum(const CRuVector3 &vec1, const CRuVector3 &vec2) { x = vec1.x + vec2.x; y = vec1.y + vec2.y; z = vec1.z + vec2.z; }
	inline void						SetToDifference(const CRuVector3 &vec1, const CRuVector3 &vec2) { x = vec1.x - vec2.x; y = vec1.y - vec2.y; z = vec1.z - vec2.z; }
	inline void						SetToProduct(const CRuVector3 &vec1, REAL scale) { x = vec1.x * scale; y = vec1.y * scale; z = vec1.z * scale; }
	void							SetToCrossProduct(const CRuVector3 &vec1, const CRuVector3 &vec2);

	// Projection
	CRuVector3&						ProjectOnto(const CRuVector3 &u);

	// Assignment operator overload
	inline CRuVector3&				operator=(const CRuVector3 &vec) { x = vec.x; y = vec.y; z = vec.z; return *this; }

	inline CRuVector3&				operator=(REAL val) { x = val; y = val; z = val; return *this; }

	// Arithmetic operator overloads
	inline CRuVector3				operator+(const CRuVector3 &vec) const { return CRuVector3(x + vec.x, y + vec.y, z + vec.z); }
	inline CRuVector3				operator-(const CRuVector3 &vec) const { return CRuVector3(x - vec.x, y - vec.y, z - vec.z); }
	inline CRuVector3				operator*(const CRuVector3 &vec) const { return CRuVector3(x * vec.x, y * vec.y, z * vec.z); }
	inline CRuVector3				operator/(const CRuVector3 &vec) const { return CRuVector3(x / vec.x, y / vec.y, z / vec.z); }

	inline CRuVector3				operator+(const REAL val) const { return CRuVector3(x + val, y + val, z + val); }
	inline CRuVector3				operator-(const REAL val) const { return CRuVector3(x - val, y - val, z - val); }
	inline CRuVector3				operator*(const REAL val) const { return CRuVector3(x * val, y * val, z * val); }
	inline CRuVector3				operator/(const REAL val) const { return CRuVector3(x / val, y / val, z / val); }

	// Arithmetic update operator overloads
	inline CRuVector3&				operator+=(const CRuVector3 &vec) { x = x + vec.x; y = y + vec.y; z = z + vec.z; return *this; }
	inline CRuVector3&				operator-=(const CRuVector3 &vec) { x = x - vec.x; y = y - vec.y; z = z - vec.z; return *this; }
	inline CRuVector3&				operator*=(const CRuVector3 &vec) { x = x * vec.x; y = y * vec.y; z = z * vec.z; return *this; }
	inline CRuVector3&				operator/=(const CRuVector3 &vec) { x = x / vec.x; y = y / vec.y; z = z / vec.z; return *this; }

	inline CRuVector3&				operator+=(REAL val) { x = x + val; y = y + val; z = z + val; return *this; }
	inline CRuVector3&				operator-=(REAL val) { x = x - val; y = y - val; z = z - val; return *this; }
	inline CRuVector3&				operator*=(REAL val) { x = x * val; y = y * val; z = z * val; return *this; }
	inline CRuVector3&				operator/=(REAL val) { x = x / val; y = y / val; z = z / val; return *this; }

	// Comparison operations and operator overloads
	inline BOOL						operator==(const CRuVector3 &vec) const { return (x == vec.x && y == vec.y && z == vec.z) ? TRUE : FALSE; }
	inline BOOL						operator!=(const CRuVector3 &vec) const { return (x != vec.x || y != vec.y || z != vec.z) ? TRUE : FALSE; }

	inline BOOL						operator==(REAL val) const { return (x == val && y == val && z == val) ? TRUE : FALSE; }
	inline BOOL						operator!=(REAL val) const { return (x != val || y != val || z != val) ? TRUE : FALSE; }

	inline BOOL						IsEquivalent(const CRuVector3 &vec) const
									{
										if(fabs(x - vec.x) <= (0.005f) && fabs(y - vec.y) <= (0.005f) && fabs(z - vec.z) <= (0.005f))
											return TRUE;
										return FALSE;
									}

	// Array-style vector accessors (For convenience and algorithmic clarity only, no error checking!)
	inline REAL&					operator[](const int index) { return m_elements[index]; }
	inline const REAL&				operator[](const int index) const { return m_elements[index]; }

	// Other stuff
	// Input W must be initialized to a nonzero vector, output is {U,V,W},
	// an orthonormal basis.  A hint is provided about whether or not W
	// is already unit length.
	static void						GenerateOrthonormalBasis(CRuVector3 &rkU, CRuVector3 &rkV, CRuVector3 &rkW, BOOL unitLengthW);
};

// ************************************************************************************************************************************************************

class CRuVector4
{
public:
	union
	{
		struct
		{
			union
			{
				REAL m_x;
				REAL x;
			};
			union
			{
				REAL m_y;
				REAL y;
			};
			union
			{
				REAL m_z;
				REAL z;
			};
			union
			{
				REAL m_w;
				REAL w;
			};
		};

		REAL m_elements[4];
		REAL elements[4];
	};

public:
};

// ************************************************************************************************************************************************************

// Plane equation defined as Ax + By + Cz + D = 0
class CRuPlane4
{
public:
	union
	{
		struct
		{
			union
			{
				REAL m_a;
				REAL a;
				REAL A;
			};
			union
			{
				REAL m_b;
				REAL b;
				REAL B;
			};
			union
			{
				REAL m_c;
				REAL c;
				REAL C;
			};
			union
			{
				REAL m_d;
				REAL d;
				REAL D;
			};
		};

		REAL m_elements[4];
		REAL elements[4];
	};

public:
						CRuPlane4();
						CRuPlane4(const CRuVector3 &normal, const CRuVector3 &p0);
						CRuPlane4(REAL va, REAL vb, REAL vc, REAL vd);

	// Plane properties
	void				SetPlane(const CRuVector3 &normal, const CRuVector3 &p0);
	void				SetPlane(const CRuVector3 &p0, const CRuVector3 &p1, const CRuVector3 &p2);
	void				Normalize();

	// Intersection testing
//	RuIntersectResult	Intersects(const CRuSegment3 &segment, CRuVector3 &intPtOut) const;
//	RuIntersectResult	Intersects(const CRuVector3 &origin, const CRuVector3 &direction, CRuVector3 &intPtOut) const;

	// Queries
	inline REAL			GetDistance(const CRuVector3 &pt) const { return pt.x * m_a + pt.y * m_b + pt.z * m_c + m_d; }
	inline REAL			GetDistance(const REAL x, const REAL y, const REAL z) const { return x * m_a + y * m_b + z * m_c + m_d; }
	inline BOOL			Contains(const CRuVector3 &pt) const { return (pt.x * m_a + pt.y * m_b + pt.z * m_c + m_d) >= 0.0f; }
	inline REAL			DotNormal(const CRuVector3 &vec) const { return vec.x * m_a + vec.y * m_b + vec.z * m_c; }
	inline REAL			DotNormal(const CRuPlane4 &plane) const { return plane.m_a * m_a + plane.m_b * m_b + plane.m_c * m_c; }
	REAL				SolveForY(REAL x, REAL z) const;
	inline CRuVector3	GetNormal() const { return CRuVector3(m_a, m_b, m_c); }
	CRuVector3			GetClosestPointOnPlane(const CRuVector3 &point) const;
	inline BOOL			IsZero() const { return m_a == 0.0f && m_b == 0.0f && m_c == 0.0f && m_d == 0.0f; }

	inline CRuPlane4	operator*(const REAL multiplier) const { return CRuPlane4(m_a * multiplier, m_b * multiplier, m_c * multiplier, m_d); }
	inline BOOL			operator==(const CRuPlane4 &plane) const { return (m_a == plane.m_a && m_b == plane.m_b && m_c == plane.m_c && m_d == plane.m_d) ? TRUE : FALSE; }
};

// ************************************************************************************************************************************************************

class CRuMatrix4x4
{
public:
	union
	{
		struct
		{
			REAL					_11, _12, _13, _14;
			REAL					_21, _22, _23, _24;
			REAL					_31, _32, _33, _34;
			REAL					_41, _42, _43, _44;
		};

		REAL						m_elements[4][4];
		REAL						elements[4][4];
	};

public:
									CRuMatrix4x4();
									CRuMatrix4x4(const CRuMatrix4x4 &mat);

	// Basic setup
	CRuMatrix4x4&					SetToIdentity();
	inline CRuMatrix4x4&			SetToTranslation(const CRuVector3 &vec)
									{
										return SetToTranslation(vec.x, vec.y, vec.z);
									}
	CRuMatrix4x4&					SetToTranslation(REAL x, REAL y, REAL z);
	CRuMatrix4x4&					SetToScaling(REAL xScale, REAL yScale, REAL zScale);
	CRuMatrix4x4&					SetToRotationEulerXYZ(REAL x, REAL y, REAL z);
	CRuMatrix4x4&					SetToRotation(const CRuVector3 &axis, REAL angle);
	CRuMatrix4x4&					SetToRotationSxT(const CRuVector3 &s, const CRuVector3 &t);
	CRuMatrix4x4&					SetToBoneSRT(REAL scale, const CRuQuaternion &rotation, const CRuVector3 &translation);
	CRuMatrix4x4&					SetToBoneSRT(const CRuVector3 &scale, const CRuQuaternion &rotation, const CRuVector3 &translation);
	CRuMatrix4x4&					SetToReflection(const CRuPlane4 &reflectionPlane);

	// Frustum transform setup
	CRuMatrix4x4&					SetToProjection(REAL nearPlane, REAL farPlane, REAL fovHorz, REAL fovVert);	// deprecate
	CRuMatrix4x4&					SetToPerspectiveProjection(REAL nearPlane, REAL farPlane, REAL w, REAL h);

	// Basis & coordinate systems setup
	CRuMatrix4x4&					SetToBasis(const CRuVector3 &xAxis, const CRuVector3 &yAxis, const CRuVector3 &zAxis);
	CRuMatrix4x4&					SetToInverseBasis(const CRuVector3 &xAxis, const CRuVector3 &yAxis, const CRuVector3 &zAxis);

	// Billboarding
	CRuMatrix4x4&					SetToBillboard(const CRuMatrix4x4 &viewTransform, const CRuVector3 &geometryForward, const CRuVector3 &geometryUp);
	CRuMatrix4x4&					SetToAxialBillboard(const CRuMatrix4x4 &viewTransform, const CRuVector3 &geometryForward, const CRuVector3 &constraintAxis);
	CRuMatrix4x4&					SetToWorldBillboard(const CRuMatrix4x4 &viewTransform, const CRuVector3 &geometryForward, const CRuVector3 &constraintAxis);

	// Post-multiply concatenation transforms
	CRuMatrix4x4&					Scale(const CRuVector3 &scale);
	CRuMatrix4x4&					Rotate(const CRuQuaternion &quat);
	CRuMatrix4x4&					Rotate(const CRuVector3 &axis, REAL angle);
	CRuMatrix4x4&					Translate(const CRuVector3 &translation);
	CRuMatrix4x4&					Reflect(const CRuPlane4 &reflectionPlane);
	CRuMatrix4x4&					ChangeBasis(const CRuVector3 &xAxis, const CRuVector3 &yAxis, const CRuVector3 &zAxis);
	CRuMatrix4x4&					ChangeInverseBasis(const CRuVector3 &xAxis, const CRuVector3 &yAxis, const CRuVector3 &zAxis);
	CRuMatrix4x4&					Orthonormalize();

	// Transformation functions
	CRuVector3&						TransformPoint(CRuVector3 &point) const;
	CRuVector3&						TransformPoint(const CRuVector3 &point, CRuVector3 &pointOut) const;
	CRuVector4&						TransformPoint(const CRuVector3 &point, CRuVector4 &pointOut) const;
	CRuVector3&						TransformPointProjected(CRuVector3 &point) const;
	CRuVector3&						TransformPointProjected(const CRuVector3 &point, CRuVector3 &pointOut) const;
	CRuVector3&						TransformVector(CRuVector3 &vec) const;
	CRuVector3&						TransformVector(const CRuVector3 &vec, CRuVector3 &vecOut) const;
	CRuVector3&						TransformVectorProjected(CRuVector3 &vec) const;
	CRuVector3&						TransformVectorProjected(const CRuVector3 &vec, CRuVector3 &vecOut) const;

	// Matrix operations
	CRuMatrix4x4					GetTranspose() const;
	CRuMatrix4x4					GetInverse() const;
	CRuMatrix4x4					GetInverse_Gaussian() const;
	REAL							Get3x3Determinant(int i, int j) const;
	CRuMatrix4x4					GetOrthogonalInverse() const;
	BOOL							GetDecomposition(CRuVector3 &scaleOut, CRuVector3 &shearOut, CRuVector3 &rotationOut, CRuVector3 &translationOut) const;

	// Assignment operator overload
	CRuMatrix4x4&					operator=(const CRuMatrix4x4 &mat);

	// Arithmetic operator overloads
	CRuMatrix4x4					operator*(const CRuMatrix4x4 &mat) const;
	CRuMatrix4x4					operator*(const REAL multiplier) const;
	CRuMatrix4x4					operator/(const REAL divisor) const;

	// Comparison operations and operator overloads
	BOOL							operator==(const CRuMatrix4x4 &mat) const;
	inline BOOL						operator!=(const CRuMatrix4x4 &mat) const { return !operator==(mat); }

	// Miscellaneous
	void							GetStringRepresentation(char *strOut) const;



	// Deprecated
	void							SetTranslation(REAL x, REAL y, REAL z);
	D3DMATRIX						GetD3DMatrix() const;
};

// ************************************************************************************************************************************************************

// Quaternions for rotations
class CRuQuaternion
{
public:
	union
	{
		struct
		{
			union
			{
				REAL m_x;
				REAL x;
			};
			union
			{
				REAL m_y;
				REAL y;
			};
			union
			{
				REAL m_z;
				REAL z;
			};
			union
			{
				REAL m_w;
				REAL w;
			};
		};

		REAL m_elements[4];
		REAL elements[4];
	};

public:
	inline						CRuQuaternion() : m_x(0.0f), m_y(0.0f), m_z(0.0f), m_w(1.0f) { }
	inline						CRuQuaternion(REAL newX, REAL newY, REAL newZ, REAL newW) : m_x(newX), m_y(newY), m_z(newZ), m_w(newW) { }
	inline						CRuQuaternion(const CRuQuaternion &quat) : m_x(quat.m_x), m_y(quat.m_y), m_z(quat.m_z), m_w(quat.m_w) { }
								CRuQuaternion(REAL x, REAL y, REAL z);

	// Set operations
	inline void					Set(const CRuQuaternion &quat) { m_x = quat.m_x; m_y = quat.m_y; m_z = quat.m_z; m_w = quat.m_w; }
	inline void					Set(REAL newX, REAL newY, REAL newZ, REAL newW) { m_x = newX; m_y = newY; m_z = newZ; m_w = newW; }

	inline CRuQuaternion&		Normalize()
	{
		REAL sqrMagnitude = m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w;
		REAL magnitude = (REAL) RuSqrt(sqrMagnitude);

		m_x /= magnitude;
		m_y /= magnitude;
		m_z /= magnitude;
		m_w /= magnitude;

		return *this;
	}

	// Arithmetic operations
	CRuQuaternion				GetInverse() const;
	CRuQuaternion				GetConjugate() const;
	CRuQuaternion				GetExp() const;
	CRuQuaternion				GetLog() const;

	// Queries
	inline BOOL					IsIdentity() const { return (m_x == 0.0f && m_y == 0.0f && m_z == 0.0f && m_w == 1.0f); }

	// Rotations
	BOOL						FromAngleAxis(const CRuVector3 &axisOfRotation, REAL rotationAngle);
	BOOL						ToAngleAxis(CRuVector3 &axisOfRotationOut, REAL &rotationAngleOut);
	BOOL						FromRotationMatrix(const CRuMatrix4x4 &matrix);
	
	BOOL						MakeEulerRotation(REAL x, REAL y, REAL z);
	BOOL						MakeEulerRotation(CRuVector3 rotAngles);
	BOOL						RotateIntoVector(CRuVector3 targetAxis, CRuVector3 sourceAxis);
	CRuQuaternion&				RotateIntoVector(CRuVector3 targetAxis, CRuVector3 sourceAxis, const CRuVector3 &constraintAxis);

	// Billboarding
	CRuQuaternion&				SetToAxialBillboard(const CRuMatrix4x4 &viewTransform, const CRuVector3 &geometryForward, const CRuVector3 &constraintAxis);

	// Quaternion conversion
	void						GetAngleAxis(CRuVector3 &axis, REAL &angle) const;
	CRuMatrix4x4				GetMatrix() const;
	void						GetMatrix(CRuMatrix4x4 &matOut) const;

	// Transforms
	CRuVector3&					TransformVector(CRuVector3 &vec) const;
	CRuVector3&					TransformVector(const CRuVector3 &vec, CRuVector3 &vecOut) const;

	// Assignment operator overload
	inline CRuQuaternion&		operator=(const CRuQuaternion &quat) { m_x = quat.m_x; m_y = quat.m_y; m_z = quat.m_z; m_w = quat.m_w; return *this; }

	// Arithmetic operator overloads
	inline CRuQuaternion		operator+(const CRuQuaternion &quat) const { return CRuQuaternion(m_x + quat.m_x, m_y + quat.m_y, m_z + quat.m_z, m_w + quat.m_w); }
	inline CRuQuaternion		operator-(const CRuQuaternion &quat) const  { return CRuQuaternion(m_x - quat.m_x, m_y - quat.m_y, m_z - quat.m_z, m_w - quat.m_w); }
	CRuQuaternion				operator*(const CRuQuaternion &quat) const;
	CRuVector3					operator*(const CRuVector3 &vec) const;
	inline CRuQuaternion		operator*(REAL scalar) const { return CRuQuaternion(m_x * scalar, m_y * scalar, m_z * scalar, m_w * scalar); }
	inline CRuQuaternion		operator/(REAL scalar) const { return CRuQuaternion(m_x / scalar, m_y / scalar, m_z / scalar, m_w / scalar); }
	inline CRuQuaternion		operator-() const { return CRuQuaternion(-m_x, -m_y, -m_z, -m_w); }

	// Arithmetic update operator overloads
	inline CRuQuaternion&		operator+=(const CRuQuaternion &quat) { m_x += quat.m_x; m_y += quat.m_y; m_z += quat.m_z; m_w += quat.m_w; return *this; }
	inline CRuQuaternion&		operator-=(const CRuQuaternion &quat) { m_x -= quat.m_x; m_y -= quat.m_y; m_z -= quat.m_z; m_w -= quat.m_w; return *this; }
	inline CRuQuaternion&		operator*=(REAL scalar) { m_x *= scalar; m_y *= scalar; m_z *= scalar; m_w *= scalar; return *this; }
	inline CRuQuaternion&		operator/=(REAL scalar) { m_x /= scalar; m_y /= scalar; m_z /= scalar; m_w /= scalar; return *this; }

	// Comparison operator overloads
	inline BOOL					operator==(const CRuQuaternion &quat) const { return (m_x == quat.m_x && m_y == quat.m_y && m_z == quat.m_z && m_w == quat.m_w) ? TRUE : FALSE; }
	inline BOOL					operator!=(const CRuQuaternion &quat) const { return (m_x != quat.m_x || m_y != quat.m_y || m_z != quat.m_z || m_w != quat.m_w) ? TRUE : FALSE; }

	// Array-style vector accessors (For convenience and algorithmic clarity only, no error checking!)
	inline REAL&				operator[](const int index) { return m_elements[index]; }
	inline const REAL&			operator[](const int index) const { return m_elements[index]; }
};

// ************************************************************************************************************************************************************

// Other miscellaneous vector math functions
extern void					MatrixMultiply(const CRuMatrix4x4 &mat1, const CRuMatrix4x4 &mat2, CRuMatrix4x4 &outMat);
extern inline CRuVector3	CrossProduct(const CRuVector3 &vector1, const CRuVector3 &vector2);
extern inline REAL			DotProduct(const CRuVector3 &vector1, const CRuVector3 &vector2);
extern inline REAL			TripleProduct_Scalar(const CRuVector3 &vector1, const CRuVector3 &vector2, const CRuVector3 &u);
extern inline REAL			TripleProduct_Scalar(const CRuVector3 &vector1, const CRuVector3 &vector2);
extern inline REAL			CalculateAngleDifferential(const CRuVector3 &originVector, const CRuVector3 &targetVector, const CRuVector3 &rotAxis, REAL &rotAngleOut);
extern inline void			VectorTransform(const CRuVector3 &vec, const CRuMatrix4x4 &matrix, CRuVector3 &out);
extern inline void			VectorTransform3(const CRuVector3 &vec, const CRuMatrix4x4 &matrix, CRuVector3 &out);

extern inline CRuVector3	operator*(const CRuVector3 &vec, const CRuMatrix4x4 &matrix);
extern CRuPlane4			operator*(const CRuPlane4 &plane, const CRuMatrix4x4 &matrix);

// Quaternion support
extern CRuQuaternion		QuaternionSlerp(const CRuQuaternion &from, const CRuQuaternion &to, REAL t);
extern CRuQuaternion		QuaternionLerp(const CRuQuaternion &from, const CRuQuaternion &to, REAL t);

// ************************************************************************************************************************************************************

#pragma pack(pop) 

#pragma managed

#endif
