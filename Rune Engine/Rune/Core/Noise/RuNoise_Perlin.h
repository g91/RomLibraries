#ifndef _RUNOISE_PERLIN_
#define _RUNOISE_PERLIN_

// Base types
#include "../../Core/Noise/RuNoise_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuNoise_Perlin : public CRuNoise_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuMTRandom										m_mtRand;

	// Parameters
	INT32											m_seed;

	// Fixed
	INT32											m_B;
	INT32											m_BMask;
	INT32											m_N;
	INT32											m_NMask;
	INT32											m_NP;

	// Runtime data
	INT32*											m_permutationTable;
	REAL*											m_g1;
	REAL**											m_g2;
	REAL**											m_g3;

public:
													CRuNoise_Perlin();
	virtual											~CRuNoise_Perlin();

	INT32											GetSeed();
	void											SetSeed(INT32 seed);

	virtual BOOL									SerializeFrom(IRuStream *stream);
	virtual BOOL									SerializeTo(IRuStream *stream);

	virtual REAL									Evaluate(REAL x, REAL y, REAL z);

protected:
	void											Normalize2(REAL *v);
	void											Normalize3(REAL *v);
	void											Initialize(INT32 seed);
	void											Reset();

	REAL											Noise3(REAL *vec);

	REAL											s_curve(REAL t);
	REAL											lerp(REAL t, REAL a, REAL b);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Noise_Perlin, CRuNoise_Perlin)

// ************************************************************************************************************************************************************

#pragma managed

#endif
