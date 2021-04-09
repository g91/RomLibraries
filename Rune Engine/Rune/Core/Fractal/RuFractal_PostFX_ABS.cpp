#include "../../Core/Fractal/RuFractal_PostFX_ABS.h"

#include <math.h>

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFractal_PostFX_ABS, CRuFractal_Base)
ruHEAP_DEFINE_SUBCLASS(CRuFractal_PostFX_ABS, "CRuFractal_PostFX_ABS", "CRuFractal_Base")
ruCLASSGUID_DEFINE(CRuFractal_PostFX_ABS, 0xD76B9F03, 0xEEDA4d73, 0x8277BE3C, 0x9B12AEBD)

// ************************************************************************************************************************************************************

CRuFractal_PostFX_ABS::CRuFractal_PostFX_ABS()
{
}

CRuFractal_PostFX_ABS::~CRuFractal_PostFX_ABS()
{
}

BOOL CRuFractal_PostFX_ABS::SerializeFrom(IRuStream *stream)
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

	return TRUE;
}

BOOL CRuFractal_PostFX_ABS::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuFractal_Base::SerializeTo(stream);

	return TRUE;
}

REAL CRuFractal_PostFX_ABS::Evaluate(REAL x, REAL y, REAL z)
{
	return fabs(x);
}

// ************************************************************************************************************************************************************

#pragma managed
