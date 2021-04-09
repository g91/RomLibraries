#ifndef _RUFRACTAL_POSTFX_ABS_
#define _RUFRACTAL_POSTFX_ABS_

// Base types
#include "../../Core/Fractal/RuFractal_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuFractal_PostFX_ABS : public CRuFractal_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
													CRuFractal_PostFX_ABS();
	virtual											~CRuFractal_PostFX_ABS();

	virtual BOOL									SerializeFrom(IRuStream *stream);
	virtual BOOL									SerializeTo(IRuStream *stream);

	virtual REAL									Evaluate(REAL x, REAL y, REAL z);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Fractal_PostFX_ABS, CRuFractal_PostFX_ABS)

// ************************************************************************************************************************************************************

#pragma managed

#endif
