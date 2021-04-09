#include "../../Core/Fractal/RuFractal_PostFX_Gain.h"

#include <math.h>

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFractal_PostFX_Gain, CRuFractal_Base)
ruHEAP_DEFINE_SUBCLASS(CRuFractal_PostFX_Gain, "CRuFractal_PostFX_Gain", "CRuFractal_Base")
ruCLASSGUID_DEFINE(CRuFractal_PostFX_Gain, 0x3D983BA1, 0x452E499d, 0xAF6F5A13, 0xCD2DEF86)

// ************************************************************************************************************************************************************

CRuFractal_PostFX_Gain::CRuFractal_PostFX_Gain()
:	m_gain(0.0f)
{
}

CRuFractal_PostFX_Gain::~CRuFractal_PostFX_Gain()
{
}

REAL CRuFractal_PostFX_Gain::GetGain()
{
	return m_gain;
}

void CRuFractal_PostFX_Gain::SetGain(REAL gain)
{
	m_gain = gain;
}

BOOL CRuFractal_PostFX_Gain::SerializeFrom(IRuStream *stream)
{
	// Read class GUID
	CRuGUID classGUID;
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuFractal_Base::SerializeFrom(stream);

	// Read cutoff
	stream->Read(&m_gain, sizeof(REAL));

	return TRUE;
}

BOOL CRuFractal_PostFX_Gain::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuFractal_Base::SerializeTo(stream);

	// Write cutoff
	stream->Write(&m_gain, sizeof(REAL));

	return TRUE;
}

REAL CRuFractal_PostFX_Gain::Evaluate(REAL x, REAL y, REAL z)
{
	REAL p = log(1.0f - m_gain) / log(0.5f);

	if(x < 0.001f)
		return 0.0f;
	else if (x > 0.999f)
		return 1.0f;
	if(x < 0.5f)
		return pow(2.0f * x, p) / 2.0f;
	else
		return 1.0f - pow(2.0f * (1.0f - x), p) / 2.0f;
/*
	REAL value = 0.0f;

	if(x < 0.001f)
		value = 0.0f;
	else if (x > 0.999f)
		value = 1.0f;
	if(x < 0.5f)
		value = pow(2.0f * x, p) / 2.0f;
	else
		value = 1.0f - pow(2.0f * (1.0f - x), p) / 2.0f;

	value = (value + 1.0f) / 2.0f;

	return value;
*/
}

// ************************************************************************************************************************************************************

#pragma managed
