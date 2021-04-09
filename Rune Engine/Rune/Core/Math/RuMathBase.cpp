#include "../../Core/Math/RuMathBase.h"

#pragma unmanaged

/*
	Description	: Solve a * x^2 + b * x + c = 0.
	Parameters	: Constants a, b, c, and root outputs.
	Output		: Returns TRUE if solution exists, stores solutions in root1Out and root2Out.
*/
BOOL SolveQuadratic(float a, float b, float c, float &root1Out, float &root2Out)
{
	if(a == 0)
	{
		if(b != 0)
		{
			// No x^2 component, it's a linear system
			root1Out = root2Out = -c / b;
			return TRUE;
		}

		if(c == 0)
		{
			// All zeros
			root1Out = root2Out = 0;
			return TRUE;
		}

		return FALSE;
	}

	float tmp = b * b - 4.0f * a * c;
	if(tmp < 0)
	{
		// Imaginary number, no solution.
		return FALSE;
	}

	tmp = RuSqrt(tmp);
	root1Out = (-b + tmp) / (2.0f * a);
	root2Out = (-b - tmp) / (2.0f * a);

	return TRUE;
}

REAL Spline_Evaluate(REAL t, REAL *knots, INT32 numKnots)
{
	if(t < 0.0f) 
		t = 0.0f;
	if(t > 1.0f)
		t = 1.0f;
	t *= numKnots - 3.0f;

	int s = (int) t;
	if(s > numKnots - 4)
		s = numKnots - 4;
	t -= s;

	float c3 = (float) (-0.5f * knots[s] + 1.5f * knots[s + 1] - 1.5f * knots[s + 2] + 0.5f * knots[s + 3]);
	float c2 = (float) (knots[s] - 2.5 * knots[s + 1] + 2.0f * knots[s + 2] - 0.5f * knots[s + 3]);
	float c1 = (float) (-0.5f * knots[s] + 0.5f * knots[s + 2]);
	float c0 = knots[s + 1];

	return ((c3 * t + c2) * t + c1) * t + c0;
}

#pragma managed
