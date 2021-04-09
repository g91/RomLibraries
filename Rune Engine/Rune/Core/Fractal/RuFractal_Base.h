#ifndef _RUFRACTAL_BASE_
#define _RUFRACTAL_BASE_

// Base types
#include "../../Core/Type/RuType_Object.h"
#include "../../Core/Noise/RuNoise_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuFractal_Base : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	REAL											m_scale;
	REAL											m_bias;
	REAL											m_factor;
	REAL											m_xOffset;
	REAL											m_yOffset;

	CRuNoise_Base*									m_noiseFunction;							//!< Noise function to use as basis

public:
													CRuFractal_Base();
	virtual											~CRuFractal_Base();

	virtual BOOL									SerializeFrom(IRuStream *stream);
	virtual BOOL									SerializeTo(IRuStream *stream);

	virtual REAL									GetScale();
	virtual void									SetScale(REAL scale);
	virtual REAL									GetBias();
	virtual void									SetBias(REAL bias);
	virtual REAL									GetFactor();
	virtual void									SetFactor(REAL factor);
	virtual REAL									GetXOffset();
	virtual void									SetXOffset(REAL offset);
	virtual REAL									GetYOffset();
	virtual void									SetYOffset(REAL offset);

	virtual CRuNoise_Base*							GetNoiseFunction();
	virtual BOOL									SetNoiseFunction(CRuNoise_Base *noiseFunction);
	virtual REAL									Evaluate(REAL x, REAL y, REAL z) = 0;
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
