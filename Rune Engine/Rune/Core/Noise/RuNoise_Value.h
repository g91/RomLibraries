#ifndef _RUNOISE_VALUE_
#define _RUNOISE_VALUE_

// Base types
#include "../../Core/Noise/RuNoise_Base.h"

#include "../../Core/Math/RuMathVector.h"
#include "../../Core/Utility/RuUtility.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuNoise_Value : public CRuNoise_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuMTRandom										m_mtRand;
	CRuMatrix4x4									m_splineBasis;
	INT32											m_valueTableSize;
	REAL*											m_valueTable;
	INT32*											m_permutationTable;

public:
													CRuNoise_Value();
	virtual											~CRuNoise_Value();

	virtual BOOL									SerializeFrom(IRuStream *stream);
	virtual BOOL									SerializeTo(IRuStream *stream);

	virtual REAL									Evaluate(REAL x, REAL y, REAL z);

protected:
	void											InitializeValueTable(INT32 valueTableSize, INT32 seed);
	INT32											Permute(INT32 x);
	INT32											Index(INT32 x, INT32 y, INT32 z);
	REAL											SampleLattice(INT32 x, INT32 y, INT32 z);
	REAL											Clamp(REAL x, REAL a, REAL b);
	REAL											Spline(REAL x, INT32 nKnots, REAL *knot);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Noise_Value, CRuNoise_Value)

// ************************************************************************************************************************************************************

#pragma managed

#endif
