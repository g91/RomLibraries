/*!
	@file RuFMODChannel.h

	Copyright (c) 2004-2011 Runewaker Entertainment Ltd. All rights reserved.
*/

#pragma once

#include "RuSoundChannel.h"

#include <fmod.hpp>

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuFMODChannel : public IRuSoundChannel
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuGUID									m_disposeGUID;

	FMOD::System*							m_fmod;
	FMOD::Channel*							m_channel;
	FMOD::Sound*							m_sound;

	IRuObject*								m_host;

	REAL									m_fadeClock;
	REAL									m_fadeTime;

public:
											CRuFMODChannel();
	virtual									~CRuFMODChannel();

	void									Initialize(FMOD::System *fmod, FMOD::Channel *fmodChannel, FMOD::Sound *fmodSound);

	virtual void							CopySettingsFrom(IRuSoundChannel *srcChannel);
	virtual void							Swap(IRuSoundChannel *srcChannel);

	virtual IRuObject*						GetHost();
	virtual void							SetHost(IRuObject *hostObj);

	virtual INT32							Get3DAttributes(CRuVector3 &positionOut, CRuVector3 &velocityOut);
	virtual INT32							Set3DAttributes(const CRuVector3 &position, const CRuVector3 &velocity);
	virtual void							Get3DMinMaxDistance(REAL &minDist, REAL &maxDist);
	virtual void							Set3DMinMaxDistance(REAL minDist, REAL maxDist);

	virtual UINT32							GetPosition();
	virtual void							SetPosition(UINT32 positionMS);

	virtual INT32							FadeAndStop(REAL fadeTime);
	virtual INT32							Is3D();
	virtual INT32							IsPaused();
	virtual INT32							IsPlaying();
	virtual INT32							Pause(INT32 pause);

	virtual INT32							GetLoopCount();
	virtual INT32							SetLoopCount(INT32 loopCount);

	virtual DWORD							GetMode();
	virtual INT32							SetMode(DWORD mode);

	virtual REAL							GetVolume();
	virtual INT32							SetVolume(REAL volume);

	virtual void							SetFrequency(REAL frequencyRatio);
	virtual void							SetMute(INT32 mute);
	virtual INT32							Stop();
	virtual void							Update(REAL elapsedTime);

protected:
	INT32									OnHostDisposed(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
