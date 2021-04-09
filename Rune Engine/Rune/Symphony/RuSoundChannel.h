/*!
	@file RuSoundChannel.h

	Copyright (c) 2004-2011 Runewaker Entertainment Ltd. All rights reserved.
*/

#pragma once

#include "../Core/Type/RuType_Object.h"
#include "../Core/Math/RuMathVector.h"
#include "../Core/Thread/RuThread_CriticalSection.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

enum RuSoundFlag
{
	ruSOUNDFLAG_2D						= 0x00000008,													//!< 2D sound (DEFAULT)
	ruSOUNDFLAG_3D						= 0x00000010,													//!< 3D sound
	ruSOUNDFLAG_LOOP_OFF				= 0x00000001,													//!< Looping off (DEFAULT)
	ruSOUNDFLAG_LOOP_NORMAL				= 0x00000002,													//!< Unidirectional looping
	ruSOUNDFLAG_LOOP_BIDIR				= 0x00000004,													//!< Bidirectional looping
	ruSOUNDFLAG_3D_HEADRELATIVE			= 0x00040000,													//!< 3D placement relative to the listener
	ruSOUNDFLAG_3D_WORLDRELATIVE		= 0x00080000,													//!< 3D placement in absolute world frame
	ruSOUNDFLAG_FALLOFF_LOG				= 0x00100000,													//!< Logarithmic falloff
	ruSOUNDFLAG_FALLOFF_LINEAR			= 0x00200000,													//!< Linear falloff
	ruSOUNDFLAG_FALLOFF_CUSTOM			= 0x04000000													//!< Custom falloff
};

// ************************************************************************************************************************************************************

class IRuSoundChannel : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuSoundChannel();

	virtual void							CopySettingsFrom(IRuSoundChannel *srcChannel) = 0;
	virtual void							Swap(IRuSoundChannel *srcChannel) = 0;

	virtual IRuObject*						GetHost() = 0;
	virtual void							SetHost(IRuObject *hostObj) = 0;

	virtual INT32							Get3DAttributes(CRuVector3 &positionOut, CRuVector3 &velocityOut) = 0;
	virtual INT32							Set3DAttributes(const CRuVector3 &position, const CRuVector3 &velocity) = 0;
	virtual void							Get3DMinMaxDistance(REAL &minDist, REAL &maxDist) = 0;
	virtual void							Set3DMinMaxDistance(REAL minDist, REAL maxDist) = 0;

	virtual UINT32							GetPosition() = 0;
	virtual void							SetPosition(UINT32 positionMS) = 0;

	virtual INT32							FadeAndStop(REAL fadeTime) = 0;
	virtual INT32							Is3D() = 0;
	virtual INT32							IsPaused() = 0;
	virtual INT32							IsPlaying() = 0;
	virtual INT32							Pause(INT32 pause) = 0;

	virtual INT32							GetLoopCount() = 0;
	virtual INT32							SetLoopCount(INT32 loopCount) = 0;

	virtual DWORD							GetMode() = 0;
	virtual INT32							SetMode(DWORD mode) = 0;

	virtual REAL							GetVolume() = 0;
	virtual INT32							SetVolume(REAL volume) = 0;

	virtual void							SetFrequency(REAL frequencyRatio) = 0;
	virtual void							SetMute(INT32 mute) = 0;
	virtual INT32							Stop() = 0;
	virtual void							Update(REAL elapsedTime) = 0;
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
