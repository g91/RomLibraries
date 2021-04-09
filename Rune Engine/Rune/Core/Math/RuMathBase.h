#ifndef _RUMATHBASE_H_
#define _RUMATHBASE_H_

#include <math.h>

#include "../../Core/Type/RuType_Base.h"
#include "../../Core/Type/RuType_Object.h"

#pragma unmanaged

#define PI				3.14159265f
#define ZERODELTA		1e-5f
#define EPSILON			0.000001f

#define ruPI			3.14159265f
#define ruEPSILON		0.00001f

// Algebraic functions
extern BOOL			SolveQuadratic(float a, float b, float c, float &root1Out, float &root2Out);
inline float		Falloff_Linear(float t, float range);
inline float		Falloff_Quadratic(float t, float range);

// Trignometric functions
inline void			RuSinCos(float radians, float *sineOut, float *cosineOut);

// Square root functions
inline float		fastsqrt(float r);
inline float		rsqrt(float v);			// Calculates reciprocal square root

inline REAL RuInvSqrt(REAL f)
{
	return 1.0f / (REAL)sqrt(f);
}

inline REAL RuSqrt(REAL f)
{
	return (REAL)sqrt(f);
}

// Interpolation
REAL				Spline_Evaluate(REAL t, REAL *knots, INT32 numKnots);

// ************************************************************************************************************************************************************

typedef float (*RuFalloffFunction)(float t, float range);

/**************************** INLINE FUNCTION DEFINITIONS ****************************/

inline float RuFalloff_None(float t, float range)
{
	return 1.0f;
}

inline float RuFalloff_All(float t, float range)
{
	return 0.0f;
}

inline float RuFalloff_Linear(float t, float range)
{
	return max(0.0f, 1.0f - (t / range));
}

inline float RuFalloff_Quadratic(float t, float range)
{
	t = t / range;
	return max(0.0f, 1.0f - (t * t));
}

inline float RuFalloff_Cubic(float t, float range)
{
	t = t / range;
	return max(0.0f, 1.0f - (t * t * t));
}

inline float RuFalloff_Quartic(float t, float range)
{
	t = t / range;
	return max(0.0f, 1.0f - (t * t * t * t));
}

inline float RuFalloff_Quintic(float t, float range)
{
	t = t / range;
	return max(0.0f, 1.0f - (t * t * t * t * t));
}

inline float RuFalloff_Bevel(float t, float range)
{
	t = t / range;

	if(t > 0.5f)
	{
		t = (float)pow(1.0f - (t - 0.5f) * 2.0f, 2.0f);
	}
	else
	{
		t = 1.0f;
	}

	return t;
}

inline float RuFalloff_SmoothBevel(float t, float range)
{
	t = t / range;

	if(t > 0.5f)
	{
		t = (float)pow(1.0f - (t - 0.5f) * 2.0f, 1.5f);
	}
	else
	{
		t = 1.0f;
	}

	return t;
}

inline float RuFalloff_InverseLinear(float t, float range)
{
	return min(max(0.0f, t / range), 1.0f);
}

inline float RuFalloff_InverseQuadratic(float t, float range)
{
	return min(max(0.0f, ((t / range) * (t / range))), 1.0f);
}

/*
	Description	: Calculates sine and cosine using fsincos instruction.
	Parameters	: Radians to solve for, output values sineOut and cosineOut.
	Output		: Stores sine and cosine values in sineOut and cosineOut
*/
inline void RuSinCos(float radians, float *sineOut, float *cosineOut)
{
	*sineOut = (float)sin(radians);
	*cosineOut = (float)cos(radians);
}

// this is really slow compared to MSVC's intrinsic sqrt!!!!
inline float fastsqrt(float r)
{
	float x, y;
	float tempf;
	unsigned long *tfptr = (unsigned long *) &tempf;

	tempf = r;
	*tfptr = (0xbe6f0000 - *tfptr) >> 1;
	x = tempf;
	y = r * 0.5f; 
	x *= 1.5f - x * x * y; 
	x *= 1.5f - x * x * y;
	x *= 1.5f - x * x * y; 
	x *= 1.5f - x * x * y; 

	return x * r;
}

inline float rsqrt(float v)
{
	float h = v * 0.5f;
	INT32 &i = (INT32 &) v;
	i = 0x5F3759DF - (i >> 1);
	v *= 1.5f - (h * v * v);
	v *= 1.5f - (h * v * v);
	return v;
}

template <class T>
inline void RuSwap(T& a, T& b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

#pragma managed

#endif
