/*!
	@file RuFMODReverb.cpp

	Copyright (c) 2004-2013 Runewaker Entertainment Ltd. All rights reserved.
*/

#include "RuFMODReverb.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFMODReverb, IRuSoundReverb)
ruHEAP_DEFINE_SUBCLASS(CRuFMODReverb, "CRuFMODReverb", "IRuSoundReverb")
ruCLASSGUID_DEFINE(CRuFMODReverb, 0x1E31589B, 0xB28F49a0, 0x819B6325, 0xB0250059)

// ************************************************************************************************************************************************************

CRuFMODReverb::CRuFMODReverb()
:	m_reverb(NULL)
{
}

CRuFMODReverb::~CRuFMODReverb()
{
	if(m_reverb)
	{
		m_reverb->release();
		m_reverb = NULL;
	}
}

void CRuFMODReverb::Initialize(FMOD::Reverb3D *fmodReverb)
{
	m_reverb = fmodReverb;
}

INT32 CRuFMODReverb::Set3DAttributes(const CRuVector3 &position, REAL minDist, REAL maxDist)
{
	// ...
	FMOD_VECTOR fmodPos = { position.m_x, position.m_y, position.m_z };

	// ...
	FMOD_RESULT fmodResult = m_reverb->set3DAttributes(&fmodPos, minDist, maxDist);

	return fmodResult == FMOD_OK ? TRUE : FALSE;
}

INT32 CRuFMODReverb::SetProperties(const RuSoundReverbProperties &reverbProps)
{
	// Transcribe properties to FMOD structure
	FMOD_REVERB_PROPERTIES fmodReverbProps;
	memcpy(&fmodReverbProps, &reverbProps, sizeof(RuSoundReverbProperties));

	// ...
	FMOD_RESULT fmodResult = m_reverb->setProperties(&fmodReverbProps);

	return fmodResult == FMOD_OK ? TRUE : FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
