/*!
	@project Rune
	@file RuHelper_Math.cpp

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/07/13
*/

#include "../Helper/RuHelper_Math.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	Generates a lighting vector according to the specified theta and phi values.
	An undisturbed lighting vector is one that points along the negative y axis.
	Theta determines the longitudinal angle, and phi determines the latitudinal
	angle.

	@param theta Angle along XZ plane.
	@param phi Angle along XY plane.


	@return A vector value denoting the light direction.
*/
CRuVector3 RuMathHelper_GenerateLightingVector(REAL theta, REAL phi)
{
	CRuVector3 unitDirection(-1.0f, 0.0f, 0.0f);
	CRuVector3 phiAxis(0.0f, 0.0f, 1.0f);
	CRuMatrix4x4 transform;

	transform.Rotate(CRuVector3(0.0f, 1.0f, 0.0f), theta);
	transform.TransformVector(phiAxis);

	transform.Rotate(phiAxis, phi);
	transform.TransformVector(unitDirection);

	return unitDirection;
}

// ************************************************************************************************************************************************************

REAL RuMathHelper_StepInterpolate(REAL v0, REAL v1, REAL factor, REAL minStep, REAL elapsedTime)
{
	// Factor is given as percentage per second, so multiply it by the elapsed time
	factor = factor * elapsedTime;

	// Min step is also given as units per second, so multiply it by the elapsed time
	minStep = minStep * elapsedTime;

	if(v1 > v0)
	{
		REAL step = max(minStep, (v1 - v0) * factor);
		v0 = min(v0 + step, v1);
	}
	else if(v1 < v0)
	{
		REAL step = min(-minStep, (v1 - v0) * factor);
		v0 = max(v0 + step, v1);
	}

	return v0;
}

// ************************************************************************************************************************************************************

#pragma managed
