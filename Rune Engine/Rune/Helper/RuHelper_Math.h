/*!
	@project Rune
	@file RuHelper_Math.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/07/13
*/

#ifndef _RUHELPER_MATH_H_
#define _RUHELPER_MATH_H_

#include "../Core/Math/RuMathVector.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

extern RUAPI CRuVector3			RuMathHelper_GenerateLightingVector(REAL theta, REAL phi);
extern RUAPI REAL				RuMathHelper_StepInterpolate(REAL v0, REAL v1, REAL factor, REAL minStep, REAL elapsedTime);

// ************************************************************************************************************************************************************

#pragma managed

#endif
