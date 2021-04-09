#include "../../Core/Fractal/RuFractal_PostFX_Square.h"

#include <math.h>

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFractal_PostFX_Square, CRuFractal_Base)
ruHEAP_DEFINE_SUBCLASS(CRuFractal_PostFX_Square, "CRuFractal_PostFX_Square", "CRuFractal_Base")
ruCLASSGUID_DEFINE(CRuFractal_PostFX_Square, 0x31B6409E, 0x1E8B4e2d, 0xBBA82AA3, 0xA189A961)

// ************************************************************************************************************************************************************

CRuFractal_PostFX_Square::CRuFractal_PostFX_Square()
{
}

CRuFractal_PostFX_Square::~CRuFractal_PostFX_Square()
{
}

BOOL CRuFractal_PostFX_Square::SerializeFrom(IRuStream *stream)
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

BOOL CRuFractal_PostFX_Square::SerializeTo(IRuStream *stream)
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

REAL CRuFractal_PostFX_Square::Evaluate(REAL x, REAL y, REAL z)
{
	return x * x;
}

// ************************************************************************************************************************************************************

#pragma managed
