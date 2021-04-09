/*!
	@file RuFMODReverb.h

	Copyright (c) 2004-2013 Runewaker Entertainment Ltd. All rights reserved.
*/

#pragma once

#include "RuSoundReverb.h"

#include <fmod.hpp>

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuFMODReverb : public IRuSoundReverb
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	FMOD::Reverb3D*							m_reverb;

public:
											CRuFMODReverb();
	virtual									~CRuFMODReverb();

	void									Initialize(FMOD::Reverb3D *fmodReverb);

	virtual INT32							Set3DAttributes(const CRuVector3 &position, REAL minDist, REAL maxDist);
	virtual INT32							SetProperties(const RuSoundReverbProperties &reverbProps);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
