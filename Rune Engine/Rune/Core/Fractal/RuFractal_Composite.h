#ifndef _RUFRACTAL_COMPOSITE_
#define _RUFRACTAL_COMPOSITE_

// Base types
#include "../../Core/Fractal/RuFractal_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

enum RuFractalCompositeOp
{
	ruFRACTALCOMPOSITEOP_NONE						= 0,
	ruFRACTALCOMPOSITEOP_ADD						= 1,
	ruFRACTALCOMPOSITEOP_SUBTRACT					= 2,
	ruFRACTALCOMPOSITEOP_MULTIPLY					= 3,
	ruFRACTALCOMPOSITEOP_DIVIDE						= 4,
	ruFRACTALCOMPOSITEOP_SET						= 5,
	ruFRACTALCOMPOSITEOP_POSTFX						= 6,
	ruFRACTALCOMPOSITEOP_PARAM_X					= 7,
	ruFRACTALCOMPOSITEOP_PARAM_Y					= 8,

	ruFRACTALCOMPOSITEOP_FORCE_DWORD				= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

class CRuFractal_Composite : public CRuFractal_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct FractalDesc
	{
		CRuFractal_Base*							m_fractal;
		RuFractalCompositeOp						m_compositeOp;

													FractalDesc()
													{
														m_fractal = NULL;
														m_compositeOp = ruFRACTALCOMPOSITEOP_ADD;
													}

													FractalDesc(CRuFractal_Base *fractal, RuFractalCompositeOp compositeOp)
													{
														m_fractal = fractal;
														m_compositeOp = compositeOp;

														m_fractal->AddRef();
													}
	};

	CRuArrayList<FractalDesc>						m_componentFractals;

public:
													CRuFractal_Composite();
	virtual											~CRuFractal_Composite();

	void											AppendFractal(CRuFractal_Base *fractal, RuFractalCompositeOp compositeOp);
	void											DeleteFractal(INT32 index);

	INT32											GetNumFractals() const;
	INT32											GetFractalIndex(CRuFractal_Base *fractal);
	CRuFractal_Base*								GetFractal(INT32 index);
	RuFractalCompositeOp							GetFractalCompositeOp(INT32 index);
	void											SetFractalCompositeOp(INT32 index, RuFractalCompositeOp compositeOp);

	virtual BOOL									SerializeFrom(IRuStream *stream);
	virtual BOOL									SerializeTo(IRuStream *stream);

	virtual REAL									Evaluate(REAL x, REAL y, REAL z);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Fractal_Composite, CRuFractal_Composite)

// ************************************************************************************************************************************************************

#pragma managed

#endif
