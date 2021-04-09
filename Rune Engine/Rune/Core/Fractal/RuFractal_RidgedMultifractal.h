#ifndef _RUFRACTAL_RIDGEDMULTIFRACTAL_
#define _RUFRACTAL_RIDGEDMULTIFRACTAL_

// Base types
#include "../../Core/Fractal/RuFractal_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuFractal_RidgedMultifractal : public CRuFractal_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	REAL											m_H;
	REAL											m_lacunarity;
	REAL											m_octaves;
	REAL											m_offset;
	REAL											m_gain;

	REAL*											m_exponentArray;

public:
													CRuFractal_RidgedMultifractal();
	virtual											~CRuFractal_RidgedMultifractal();

	REAL											GetH() const;
	void											SetH(REAL H);

	REAL											GetLacunarity() const;
	void											SetLacunarity(REAL lacunarity);

	REAL											GetOctaves() const;
	void											SetOctaves(REAL octaves);

	REAL											GetOffset() const;
	void											SetOffset(REAL offset);

	REAL											GetGain() const;
	void											SetGain(REAL gain);

	virtual BOOL									SerializeFrom(IRuStream *stream);
	virtual BOOL									SerializeTo(IRuStream *stream);

	virtual REAL									Evaluate(REAL x, REAL y, REAL z);

protected:
	void											BuildExponentArray();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Fractal_RidgedMultifractal, CRuFractal_RidgedMultifractal)

// ************************************************************************************************************************************************************

#pragma managed

#endif
