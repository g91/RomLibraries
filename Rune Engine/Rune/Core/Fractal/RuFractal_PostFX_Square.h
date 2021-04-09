#ifndef _RUFRACTAL_POSTFX_SQUARE_
#define _RUFRACTAL_POSTFX_SQUARE_

// Base types
#include "../../Core/Fractal/RuFractal_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuFractal_PostFX_Square : public CRuFractal_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
													CRuFractal_PostFX_Square();
	virtual											~CRuFractal_PostFX_Square();

	virtual BOOL									SerializeFrom(IRuStream *stream);
	virtual BOOL									SerializeTo(IRuStream *stream);

	virtual REAL									Evaluate(REAL x, REAL y, REAL z);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Fractal_PostFX_Square, CRuFractal_PostFX_Square)

// ************************************************************************************************************************************************************

#pragma managed

#endif
