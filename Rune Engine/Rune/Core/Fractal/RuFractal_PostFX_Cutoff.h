#ifndef _RUFRACTAL_POSTFX_CUTOFF_
#define _RUFRACTAL_POSTFX_CUTOFF_

// Base types
#include "../../Core/Fractal/RuFractal_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuFractal_PostFX_Cutoff : public CRuFractal_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	REAL											m_cutoff;

public:
													CRuFractal_PostFX_Cutoff();
	virtual											~CRuFractal_PostFX_Cutoff();

	REAL											GetCutoff();
	void											SetCutoff(REAL cutoff);

	virtual BOOL									SerializeFrom(IRuStream *stream);
	virtual BOOL									SerializeTo(IRuStream *stream);

	virtual REAL									Evaluate(REAL x, REAL y, REAL z);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Fractal_PostFX_Cutoff, CRuFractal_PostFX_Cutoff)

// ************************************************************************************************************************************************************

#pragma managed

#endif
