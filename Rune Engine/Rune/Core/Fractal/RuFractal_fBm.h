#ifndef _RUFRACTAL_FBM_
#define _RUFRACTAL_FBM_

// Base types
#include "../../Core/Fractal/RuFractal_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuFractal_fBm : public CRuFractal_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	REAL											m_H;
	REAL											m_lacunarity;
	REAL											m_octaves;

	REAL*											m_exponentArray;

public:
													CRuFractal_fBm();
	virtual											~CRuFractal_fBm();

	REAL											GetH() const;
	void											SetH(REAL H);

	REAL											GetLacunarity() const;
	void											SetLacunarity(REAL lacunarity);

	REAL											GetOctaves() const;
	void											SetOctaves(REAL octaves);

	virtual BOOL									SerializeFrom(IRuStream *stream);
	virtual BOOL									SerializeTo(IRuStream *stream);

	virtual REAL									Evaluate(REAL x, REAL y, REAL z);

protected:
	void											BuildExponentArray();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Fractal_fBm, CRuFractal_fBm)

// ************************************************************************************************************************************************************

#pragma managed

#endif
