#ifndef _RUFRACTAL_SHAPES_
#define _RUFRACTAL_SHAPES_

// Base types
#include "../../Core/Fractal/RuFractal_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

enum RuFractalShapeFunction
{
	ruFRACTALSHAPEFN_SINE							= 0,
	ruFRACTALSHAPEFN_COSINE							= 1,
	ruFRACTALSHAPEFN_SAWTOOTH						= 2,

	ruFRACTALSHAPEFN_FORCE_DWORD					= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

class CRuFractal_Shapes : public CRuFractal_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	RuFractalShapeFunction							m_xShapeFunction;
	RuFractalShapeFunction							m_yShapeFunction;

	REAL											m_H;
	REAL											m_lacunarity;
	REAL											m_octaves;

	REAL*											m_exponentArray;

public:
													CRuFractal_Shapes();
	virtual											~CRuFractal_Shapes();

	RuFractalShapeFunction							GetXShapeFunction();
	void											SetXShapeFunction(RuFractalShapeFunction fn);

	RuFractalShapeFunction							GetYShapeFunction();
	void											SetYShapeFunction(RuFractalShapeFunction fn);

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
	REAL											EvaluateFunction(REAL t, RuFractalShapeFunction fn);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Fractal_Shapes, CRuFractal_Shapes)

// ************************************************************************************************************************************************************

#pragma managed

#endif
