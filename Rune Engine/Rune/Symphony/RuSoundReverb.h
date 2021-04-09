/*!
	@file RuSoundReverb.h

	Copyright (c) 2004-2013 Runewaker Entertainment Ltd. All rights reserved.
*/

#pragma once

#include "../Core/Type/RuType_Object.h"
#include "../Core/Math/RuMathVector.h"
#include "../Core/Thread/RuThread_CriticalSection.h"
#pragma managed(push, off)

// ************************************************************************************************************************************************************

struct RuSoundReverbProperties
{
	REAL m_decayTime;
	REAL m_earlyDelay;
	REAL m_lateDelay;
	REAL m_hfReference;
	REAL m_hfDecayRatio;
	REAL m_diffusion;
	REAL m_density;
	REAL m_lowShelfFrequency;
	REAL m_lowShelfGain;
	REAL m_highCut;
	REAL m_earlyLateMix;
	REAL m_wetLevel;
};

// ************************************************************************************************************************************************************

class IRuSoundReverb : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuSoundReverb();

	virtual INT32							Set3DAttributes(const CRuVector3 &position, REAL minDist, REAL maxDist) = 0;
	virtual INT32							SetProperties(const RuSoundReverbProperties &reverbProps) = 0;
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
