#ifndef _RUMATH_UVTRANSFORM_H_
#define _RUMATH_UVTRANSFORM_H_

#include <math.h>

#include "../../Core/Math/RuMathVector.h"

#pragma unmanaged

extern CRuMatrix4x4 RuUVTransform_Projection(const CRuVector3 &origin, const CRuVector3 basisVectors[3], REAL range, REAL uOffset, REAL vOffset);
extern CRuMatrix4x4 RuUVTransform_Projection(const CRuVector3 &origin, const CRuMatrix4x4 &preTransform, const CRuVector3 basisVectors[3], REAL range, REAL uOffset, REAL vOffset);

#pragma managed

#endif
