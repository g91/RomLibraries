#ifndef _RUFRACTAL_POSTFX_REMAP_
#define _RUFRACTAL_POSTFX_REMAP_

// Base types
#include "../../Core/Fractal/RuFractal_Base.h"
#include "../../Core/Math/RuMath.h"
#include "../../Core/Math/RuMathVector.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuFractal_PostFX_Remap : public CRuFractal_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	REAL											m_rangeMin;
	REAL											m_rangeMax;

	CRuArrayList<CRuVector2>						m_curve;

public:
													CRuFractal_PostFX_Remap();
	virtual											~CRuFractal_PostFX_Remap();

	REAL											GetRangeMin();
	void											SetRangeMin(REAL rangeMin);

	REAL											GetRangeMax();
	void											SetRangeMax(REAL rangeMax);

	CRuArrayList<CRuVector2>&						GetCurve();
	void											SetCurve(CRuArrayList<CRuVector2> &curve);

	virtual BOOL									SerializeFrom(IRuStream *stream);
	virtual BOOL									SerializeTo(IRuStream *stream);

	virtual REAL									Evaluate(REAL x, REAL y, REAL z);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Fractal_PostFX_Remap, CRuFractal_PostFX_Remap)

// ************************************************************************************************************************************************************

#pragma managed

#endif
