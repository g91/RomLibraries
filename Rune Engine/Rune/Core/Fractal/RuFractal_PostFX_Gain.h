#ifndef _RUFRACTAL_POSTFX_GAIN_
#define _RUFRACTAL_POSTFX_GAIN_

// Base types
#include "../../Core/Fractal/RuFractal_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuFractal_PostFX_Gain : public CRuFractal_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	REAL											m_gain;

public:
													CRuFractal_PostFX_Gain();
	virtual											~CRuFractal_PostFX_Gain();

	REAL											GetGain();
	void											SetGain(REAL gain);

	virtual BOOL									SerializeFrom(IRuStream *stream);
	virtual BOOL									SerializeTo(IRuStream *stream);

	virtual REAL									Evaluate(REAL x, REAL y, REAL z);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Fractal_PostFX_Gain, CRuFractal_PostFX_Gain)

// ************************************************************************************************************************************************************

#pragma managed

#endif
