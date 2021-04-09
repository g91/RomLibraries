/*!
	@file RuFMODChannel.cpp

	Copyright (c) 2004-2011 Runewaker Entertainment Ltd. All rights reserved.
*/

#include "RuFMODChannel.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFMODChannel, IRuSoundChannel)
ruHEAP_DEFINE_SUBCLASS(CRuFMODChannel, "CRuFMODChannel", "IRuSoundChannel")
ruCLASSGUID_DEFINE(CRuFMODChannel, 0x010F807E, 0xCCE6403a, 0x99152B1B, 0x0C9C6FCB)

// ************************************************************************************************************************************************************

float smoothstep(float t)
{
	return 3.0f * pow(t, 2.0f) - 2.0f * pow(t, 3.0f);
}

// ************************************************************************************************************************************************************

CRuFMODChannel::CRuFMODChannel()
:	m_fmod(NULL),
	m_channel(NULL),
	m_sound(NULL),
	m_host(NULL),
	m_fadeClock(0.0f),
	m_fadeTime(-1.0f)
{
}

CRuFMODChannel::~CRuFMODChannel()
{
//	if(m_host)
//	{
//		m_host->Event_Dispose().UnregisterHandler(m_disposeGUID);
//		m_host = NULL;
//	}
}

void CRuFMODChannel::Initialize(FMOD::System *fmod, FMOD::Channel *fmodChannel, FMOD::Sound *fmodSound)
{
	m_fmod = fmod;
	m_channel = fmodChannel;
	m_sound = fmodSound;
}

void CRuFMODChannel::CopySettingsFrom(IRuSoundChannel *srcChannel)
{
	// Copy 3D attributes
	CRuVector3 pos, vel;
	srcChannel->Get3DAttributes(pos, vel);
	this->Set3DAttributes(pos, vel);

	// Copy MIN-MAX distance
	REAL minDist, maxDist;
	srcChannel->Get3DMinMaxDistance(minDist, maxDist);
	this->Set3DMinMaxDistance(minDist, maxDist);

	// Copy position
	this->SetPosition(srcChannel->GetPosition());

	// Copy volume
	this->SetVolume(srcChannel->GetVolume());

	if(srcChannel->GetType() == CRuFMODChannel::Type())
	{
		CRuFMODChannel *fmodSrcChan = static_cast<CRuFMODChannel *>(srcChannel);
	}
}

void CRuFMODChannel::Swap(IRuSoundChannel *srcChannel)
{
	if(srcChannel->GetType() == CRuFMODChannel::Type())
	{
		CRuFMODChannel *fmodSrcChan = static_cast<CRuFMODChannel *>(srcChannel);

		IRuObject *oldHost = this->m_host;
		IRuObject *oldSrcHost = fmodSrcChan->m_host;

		RuSwap(fmodSrcChan->m_channel, this->m_channel);
		RuSwap(fmodSrcChan->m_sound, this->m_sound);

		RuSwap(fmodSrcChan->m_fadeClock, this->m_fadeClock);
		RuSwap(fmodSrcChan->m_fadeTime, this->m_fadeTime);

		this->SetHost(oldSrcHost);
		fmodSrcChan->SetHost(oldHost);
	}
}

IRuObject *CRuFMODChannel::GetHost()
{
	return m_host;
}

void CRuFMODChannel::SetHost(IRuObject *hostObj)
{
//	if(m_host)
//	{
//		m_host->Event_Dispose().UnregisterHandler(m_disposeGUID);
//	}

	m_host = hostObj;

//	if(m_host)
//	{
//		m_host->Event_Dispose().RegisterHandler(m_disposeGUID, boost::bind(&CRuFMODChannel::OnHostDisposed, this, _1));
//	}
}

INT32 CRuFMODChannel::Get3DAttributes(CRuVector3 &positionOut, CRuVector3 &velocityOut)
{
	FMOD_VECTOR fmodPos;
	FMOD_VECTOR fmodVel;

	FMOD_RESULT fmodResult = m_channel->get3DAttributes(&fmodPos, &fmodVel);

	// Transcribe results
	positionOut.Set(fmodPos.x, fmodPos.y, fmodPos.z);
	velocityOut.Set(fmodVel.x, fmodVel.y, fmodVel.z);

	return fmodResult == FMOD_OK;
}

INT32 CRuFMODChannel::Set3DAttributes(const CRuVector3 &position, const CRuVector3 &velocity)
{
	FMOD_VECTOR fmodPos = { position.m_x, position.m_y, position.m_z };
	FMOD_VECTOR fmodVel = { velocity.m_x, velocity.m_y, velocity.m_z };

	FMOD_RESULT fmodResult = m_channel->set3DAttributes(&fmodPos, &fmodVel);

	return fmodResult == FMOD_OK;
}

void CRuFMODChannel::Get3DMinMaxDistance(REAL &minDist, REAL &maxDist)
{
	m_channel->get3DMinMaxDistance(&minDist, &maxDist);
}

void CRuFMODChannel::Set3DMinMaxDistance(REAL minDist, REAL maxDist)
{
	m_channel->set3DMinMaxDistance(minDist, maxDist);

	// HACK: Set spread to 90 degrees
	m_channel->set3DSpread(160.0f);
}

UINT32 CRuFMODChannel::GetPosition()
{
	UINT32 position = 0;
	m_channel->getPosition(&position, FMOD_TIMEUNIT_MS);
	return position;
}

void CRuFMODChannel::SetPosition(UINT32 positionMS)
{
	m_channel->setPosition(positionMS, FMOD_TIMEUNIT_MS);
}

INT32 CRuFMODChannel::FadeAndStop(REAL fadeTime)
{
	m_fadeClock = 0.0f;
	m_fadeTime = fadeTime;

	// Adjust fade clock based on current volume
	REAL curVolume = 1.0f;
	m_channel->getVolume(&curVolume);

	m_fadeClock = curVolume > ruEPSILON ? m_fadeTime * (1.0f - curVolume) : m_fadeTime;
	return TRUE;
}

INT32 CRuFMODChannel::Is3D()
{
	FMOD::Sound *sound = NULL;

	if(m_channel->getCurrentSound(&sound) == FMOD_OK)
	{
		FMOD_MODE mode;

		if(sound->getMode(&mode) == FMOD_OK)
		{
			if(mode & FMOD_3D)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

INT32 CRuFMODChannel::IsPlaying()
{
	bool isPlaying = false;

	FMOD_RESULT fmodResult = m_channel->isPlaying(&isPlaying);

	return fmodResult != FMOD_ERR_INVALID_HANDLE && isPlaying != false;
}

INT32 CRuFMODChannel::IsPaused()
{
	bool isPaused = false;
	FMOD_RESULT fmodResult = m_channel->getPaused(&isPaused);

	return fmodResult == FMOD_OK && isPaused ? TRUE : FALSE;
}

INT32 CRuFMODChannel::Pause(INT32 pause)
{
	FMOD_RESULT fmodResult = m_channel->setPaused(pause ? true : false);

	return fmodResult == FMOD_OK;
}

INT32 CRuFMODChannel::GetLoopCount()
{
	INT32 loopCount;
	
	m_channel->getLoopCount(&loopCount);
	
	return loopCount;
}

INT32 CRuFMODChannel::SetLoopCount(INT32 loopCount)
{
	FMOD_RESULT fmodResult = m_channel->setLoopCount(loopCount);

	return fmodResult == FMOD_OK;
}

DWORD CRuFMODChannel::GetMode()
{
	FMOD_MODE mode;

	m_channel->getMode(&mode);

	return mode;
}

INT32 CRuFMODChannel::SetMode(DWORD mode)
{
	FMOD_RESULT fmodResult = m_channel->setMode(mode);

	return fmodResult == FMOD_OK;
}

void CRuFMODChannel::SetFrequency(REAL frequencyRatio)
{
	float defFrequency;
	int defPriority;
	if(m_sound->getDefaults(&defFrequency, &defPriority) == FMOD_OK)
	{
		// Set frequency by the specified ratio
		m_channel->setFrequency(defFrequency * frequencyRatio);
	}
}

void CRuFMODChannel::SetMute(INT32 mute)
{
	m_channel->setMute(mute ? true : false);
}

REAL CRuFMODChannel::GetVolume()
{
	float volume = 1.0f;
	FMOD_RESULT fmodResult = m_channel->getVolume(&volume);

	return fmodResult == FMOD_OK ? volume : 1.0f;
}

INT32 CRuFMODChannel::SetVolume(REAL volume)
{
	FMOD_RESULT fmodResult = m_channel->setVolume(volume);

	return fmodResult == FMOD_OK;
}

INT32 CRuFMODChannel::Stop()
{
	FMOD_RESULT fmodResult = m_channel->stop();

	return fmodResult == FMOD_OK;
}

void CRuFMODChannel::Update(REAL elapsedTime)
{
	if(m_fadeTime > 0.0f)
	{
		// Increment fade clock
		m_fadeClock += elapsedTime;
		//char stopStr[2048];
		//sprintf(stopStr, "CRuFMODChannel::Update m_fadeClock:%f elapsedTime:%f m_fadeTime:%f\n", m_fadeClock, elapsedTime, m_fadeTime);
		//OutputDebugString(stopStr);
		if(m_fadeClock >= m_fadeTime)
		{
			// Force stop
			this->Stop();
		}
		else
		{
			// Calculate volume as a linear function of time
			REAL volume = (m_fadeTime - m_fadeClock) / m_fadeTime;
			// Set volume
			m_channel->setVolume(volume);
		}
	}
}

INT32 CRuFMODChannel::OnHostDisposed(RuEventArgs *eventArgs)
{
	m_host = NULL;

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
