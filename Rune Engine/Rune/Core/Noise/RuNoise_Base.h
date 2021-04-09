#ifndef _RUNOISE_BASE_
#define _RUNOISE_BASE_

// Base types
#include "../../Core/Type/RuType_Object.h"
#include "../../Core/Math/RuMathBase.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuNoise_Base : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
													CRuNoise_Base();
	virtual											~CRuNoise_Base();

	virtual BOOL									SerializeFrom(IRuStream *stream);
	virtual BOOL									SerializeTo(IRuStream *stream);

	virtual REAL									Evaluate(REAL x, REAL y, REAL z) = 0;
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
