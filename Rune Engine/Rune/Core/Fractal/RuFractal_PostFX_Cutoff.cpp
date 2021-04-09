#include "../../Core/Fractal/RuFractal_PostFX_Cutoff.h"

#include <math.h>

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFractal_PostFX_Cutoff, CRuFractal_Base)
ruHEAP_DEFINE_SUBCLASS(CRuFractal_PostFX_Cutoff, "CRuFractal_PostFX_Cutoff", "CRuFractal_Base")
ruCLASSGUID_DEFINE(CRuFractal_PostFX_Cutoff, 0x6B7D3364, 0x6B8F4eb8, 0x8FD5A0E8, 0x5CBD98AE)

// ************************************************************************************************************************************************************

CRuFractal_PostFX_Cutoff::CRuFractal_PostFX_Cutoff()
:	m_cutoff(0.0f)
{
}

CRuFractal_PostFX_Cutoff::~CRuFractal_PostFX_Cutoff()
{
}

REAL CRuFractal_PostFX_Cutoff::GetCutoff()
{
	return m_cutoff;
}

void CRuFractal_PostFX_Cutoff::SetCutoff(REAL cutoff)
{
	m_cutoff = cutoff;
}

BOOL CRuFractal_PostFX_Cutoff::SerializeFrom(IRuStream *stream)
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
	stream->Read(&m_cutoff, sizeof(REAL));

	return TRUE;
}

BOOL CRuFractal_PostFX_Cutoff::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuFractal_Base::SerializeTo(stream);

	// Write cutoff
	stream->Write(&m_cutoff, sizeof(REAL));

	return TRUE;
}

REAL CRuFractal_PostFX_Cutoff::Evaluate(REAL x, REAL y, REAL z)
{
	return max(m_cutoff, x);
}

// ************************************************************************************************************************************************************

#pragma managed
