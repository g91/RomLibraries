#include "../../Core/Noise/RuNoise_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuNoise_Base, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuNoise_Base, "CRuNoise_Base", "IRuObject")
ruCLASSGUID_DEFINE(CRuNoise_Base, 0x74E25CB9, 0x1F274d5d, 0x97D034E8, 0x25967C3A)

// ************************************************************************************************************************************************************

CRuNoise_Base::CRuNoise_Base()
{
}

CRuNoise_Base::~CRuNoise_Base()
{
}

BOOL CRuNoise_Base::SerializeFrom(IRuStream *stream)
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
	IRuObject::SerializeFrom(stream);

	return TRUE;
}

BOOL CRuNoise_Base::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeTo(stream);

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
