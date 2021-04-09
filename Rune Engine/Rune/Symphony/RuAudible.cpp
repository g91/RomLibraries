/*!
	@file RuAudible.cpp

	Copyright (c) 2004-2012 Runewaker Entertainment Ltd. All rights reserved.
*/

#include "RuAudible.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuAudible, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuAudible, "CRuAudible", "IRuObject")
ruCLASSGUID_DEFINE(CRuAudible, 0xF1F900B8, 0x76624cd7, 0x922F8E10, 0x27A44166)

// ************************************************************************************************************************************************************

CRuAudible::CRuAudible()
	: m_audioEngine(NULL),
	m_listenerMode(ruLISTENERMODE_STANDARD),
	m_proximityThreshold(100.0f),
	m_trkInvokeOverride(0.0f),
	m_volume_SFX(1.0f),
	m_volume_AMBIENCE(1.0f),
	m_volume_Master(1.0f),
	m_volume_MUSIC(1.0f),
	m_volume_INTERFACE(1.0f),
	m_nextHandle(0),
	m_camera(NULL)
{
	//m_tracks = ruNEW CRuStringHashNew<ChannelDesc*>();
	//m_trackOverrides = ruNEW CRuStringHashNew<ChannelDesc*>();
	m_cs_Global = ruNEW CRuThread_CriticalSection();
}

CRuAudible::~CRuAudible()
{
}

void CRuAudible::Dispose()
{
	FC_Clear();
	MC_Clear();
	TRK_Clear();

	ruSAFE_RELEASE(m_audioEngine);
	//ruSAFE_RELEASE(m_camera);

	ruSAFE_DELETE(m_cs_Global);

	// Chain call to base class
	IRuObject::Dispose();
}

void CRuAudible::Initialize(CRuSoundFMOD* audioEngine)
{
	m_audioEngine = audioEngine;
	// Set default 3D settings
	m_audioEngine->Set3DSettings(1.0f, 10.0f, 0.35f);

	//	m_audioEngine->SetAmbientReverb(m_audioEngine->GetReverbPreset("off"));
}

void CRuAudible::SetCamera(CRuCamera* camera)
{
	m_camera = camera;
}

void CRuAudible::SetListenerMode(ListenerMode mode)
{
	m_listenerMode = mode;
	m_audioEngine->Set3DSettings(1.0f, 10.0f, 0.35f);
	m_audioEngine->Set3DRolloffCallback(0);
}

void CRuAudible::SetProximityMuteThreshold(REAL distance)
{
	m_proximityThreshold = distance;
}

void CRuAudible::SetTrackInvokeOverride(REAL overrideProbability)
{
	m_trkInvokeOverride = overrideProbability;
}

void CRuAudible::SetVolumeLevels(REAL master, REAL sfx, REAL ambience, REAL music, REAL interf)
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	// ...
	m_volume_Master = master;
	m_volume_SFX = sfx;
	m_volume_AMBIENCE = ambience;
	m_volume_MUSIC = music;
	m_volume_INTERFACE = interf;


	//char stopStr[2048];
	//sprintf(stopStr, "CRuAudible::SetVolumeLevels m_volume_Master:%f m_volume_SFX:%f m_volume_AMBIENCE:%f m_volume_MUSIC:%f m_volume_INTERFACE:%f\n", m_volume_Master, m_volume_SFX, m_volume_AMBIENCE, m_volume_MUSIC, m_volume_INTERFACE);
	//OutputDebugString(stopStr);

	// ...
	CRuArrayList<ChannelDesc*> channels;

	m_tracks.Enumerate(channels);

	for (INT32 i = 0; i < channels.Count(); ++i)
	{
		if (channels[i]->m_soundChannel)
		{
			channels[i]->m_soundChannel->SetVolume(channels[i]->m_soundDefVol * GetVolumeByType(channels[i]->m_audibleType));
		}
	}

	// ...
	for (INT32 i = 0; i < m_manualChannels.Count(); ++i)
	{
		ChannelDesc* channelDesc = m_manualChannels.GetByIndex(i);

		if (channelDesc->m_soundChannel)
		{
			channelDesc->m_soundChannel->SetVolume(channelDesc->m_soundDefVol * GetVolumeByType(channelDesc->m_audibleType));
		}
	}

	// ...
	CRuArrayList<const char*> overrideTracks;
	m_trackOverrides.EnumerateKeys(overrideTracks);

	for (INT32 i = 0; i < overrideTracks.Count(); ++i)
	{
		ChannelDesc* channelDesc = NULL;
		m_trackOverrides.Get(overrideTracks[i], channelDesc);
		if (channelDesc && channelDesc->m_soundChannel)
		{
			channelDesc->m_soundChannel->SetVolume(channelDesc->m_soundDefVol * GetVolumeByType(channelDesc->m_audibleType));
		}
	}
}

UINT64 CRuAudible::Play(const char* soundTag, AudibleType audibleType, INT32 loop, const CRuVector3* position)
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	// Translate sound tag using defaults
	const char* sndResPath = soundTag;

	if (sndResPath)
	{
		ChannelDesc* channelDesc = CHN_Play(NULL, sndResPath, audibleType, 1.0f, position ? ruSOUNDFLAG_3D : ruSOUNDFLAG_2D, loop ? -1 : 0);

		if (channelDesc)
		{
			// Insert into manual channel list
			UINT64 handle = ++m_nextHandle;
			m_manualChannels.Add(handle, channelDesc);

			// Setup 3D position
			if (position)
			{
				CRuVector3 velocity(0.0f, 0.0f, 0.0f);
				channelDesc->m_soundChannel->Set3DAttributes(*position, velocity);
			}

			// Unpause channel
			channelDesc->m_soundChannel->Pause(FALSE);

			return handle;
		}
	}

	return 0;
}

INT32 CRuAudible::Stop(UINT64 handle, REAL fadeDuration)
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	INT32 chanIdx = m_manualChannels.Find(handle, CRuSortedList<UINT64, ChannelDesc*>::FIND_EXACT);

	if (chanIdx > -1)
	{
		ChannelDesc* channelDesc = m_manualChannels.GetByIndex(chanIdx);

		// Fade & stop
		FadeAndStop(channelDesc, fadeDuration);

		// Remove
		m_manualChannels.RemoveAt(chanIdx);

		return TRUE;
	}

	return FALSE;
}

INT32 CRuAudible::IsPlaying(UINT64 handle)
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	INT32 chanIdx = m_manualChannels.Find(handle, CRuSortedList<UINT64, ChannelDesc*>::FIND_EXACT);

	if (chanIdx > -1)
	{
		ChannelDesc* channelDesc = m_manualChannels.GetByIndex(chanIdx);

		// TODO: Check playback status?

		return TRUE;
	}

	return FALSE;
}

INT32 CRuAudible::UpdatePosition(UINT64 handle, const CRuVector3* position)
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	INT32 chanIdx = m_manualChannels.Find(handle, CRuSortedList<UINT64, ChannelDesc*>::FIND_EXACT);

	if (chanIdx > -1)
	{
		ChannelDesc* channelDesc = m_manualChannels.GetByIndex(chanIdx);

		CRuVector3 velocity(0.0f, 0.0f, 0.0f);
		channelDesc->m_soundChannel->Set3DAttributes(*position, velocity);

		return TRUE;
	}

	return FALSE;
}

INT32 CRuAudible::Play(const char* soundTag, AudibleType audibleType, const char* track, INT32 loop)
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	// Remove existing override
	if (m_trackOverrides.ContainsKey(track))
	{
		ChannelDesc* channelDesc = NULL;
		m_trackOverrides.Get(track, channelDesc);

		// Fade & stop
		FadeAndStop(channelDesc);

		// Remove from overrides
		m_trackOverrides.Remove(track);
	}

	// Translate sound tag using defaults
	const char* sndResPath = soundTag;

	if (sndResPath)
	{
		ChannelDesc* channelDesc = CHN_Play(NULL, sndResPath, audibleType, 1.0f, ruSOUNDFLAG_2D, loop ? -1 : 0);

		if (channelDesc)
		{
			// Unpause channel
			channelDesc->m_soundChannel->Pause(FALSE);

			// ...
			m_trackOverrides.Add(track, channelDesc);

			return TRUE;
		}
	}

	return FALSE;
}

INT32 CRuAudible::Stop(const char* track)
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	if (track)
	{
		if (m_trackOverrides.ContainsKey(track))
		{
			ChannelDesc* channelDesc = NULL;
			m_trackOverrides.Get(track, channelDesc);

			// Fade & stop
			FadeAndStop(channelDesc);

			// Remove override
			m_trackOverrides.Remove(track);

			return TRUE;
		}
	}
	else
	{
		CRuArrayList<const char*> overrideTracks;
		m_trackOverrides.EnumerateKeys(overrideTracks);

		for (INT32 i = 0; i < overrideTracks.Count(); ++i)
		{
			this->Stop(overrideTracks[i]);
		}
	}

	return FALSE;
}

IRuSoundChannel* CRuAudible::GetSoundChannel(UINT64 handle)
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	INT32 chanIdx = m_manualChannels.Find(handle, CRuSortedList<UINT64, ChannelDesc*>::FIND_EXACT);

	if (chanIdx > -1)
	{
		ChannelDesc* channelDesc = m_manualChannels.GetByIndex(chanIdx);

		return channelDesc->m_soundChannel;
	}

	return NULL;
}

IRuSoundChannel* CRuAudible::GetSoundChannel(const char* track)
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	if (track)
	{
		if (m_trackOverrides.ContainsKey(track))
		{
			ChannelDesc* channelDesc = NULL;
			m_trackOverrides.Get(track, channelDesc);

			return channelDesc->m_soundChannel;
		}
	}

	return NULL;
}

void CRuAudible::Update(REAL elapsedTime)
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	// Update clock
	m_clock += static_cast<UINT32>(elapsedTime * 1000);

	{
		// Perform garbage collection on fading channels
		FC_GarbageCollect();

		// ...
		MC_GarbageCollect();

		// ...
		TRK_CheckOverrides();
	}

	if (m_camera == NULL)
		return;

	m_audioEngine->Set3DListenerAttributes(0, m_camera->GetTargetPosition() - m_camera->GetCameraLookAtVector() * 2.0f, CRuVector3(0.0f, 0.0f, 0.0f), m_camera->GetCameraLookAtVector(), m_camera->GetCameraUpVector());
}

void CRuAudible::Reset()
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);
}

void CRuAudible::FadeAndStop(CRuAudible::ChannelDesc* channelDesc, REAL fadeDuration)
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	if (channelDesc)
	{
		if (channelDesc->m_soundChannel)
		{

			// ...
			channelDesc->m_soundChannel->FadeAndStop(fadeDuration);
		}

		// Add to fading channels list -- the descriptor now belongs to the FC list
		m_fadingChannels.Add(channelDesc);
	}
}

REAL CRuAudible::GetVolumeByType(AudibleType audibleType)
{
	REAL volume = 1.0f;

	switch (audibleType)
	{
	case ruSOUNDTYPE_SFX:
		volume = m_volume_SFX;
		break;

	case ruSOUNDTYPE_AMBIENCE:
		volume = m_volume_AMBIENCE;
		break;

	case ruSOUNDTYPE_MUSIC:
		volume = m_volume_MUSIC;
		break;

	case ruSOUNDTYPE_INTERFACE:
		volume = m_volume_INTERFACE;
		break;
	}

	return volume * m_volume_Master;
}

void CRuAudible::FC_Clear()
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	for (INT32 i = 0; i < m_fadingChannels.Count(); ++i)
	{
		ChannelDesc* channelDesc = m_fadingChannels[i];

		if (channelDesc->m_soundChannel)
		{
			// ...
			UTIL_UnregisterHost(channelDesc->m_soundChannel);

			// Stop playback
			channelDesc->m_soundChannel->Stop();

			// Release the sound channel object
			channelDesc->m_soundChannel->Release();

			// Clear sound channel
			channelDesc->m_soundChannel = NULL;
		}

		// Release the channel descriptor
		channelDesc->Release();
	}

	m_fadingChannels.Clear();
}

void CRuAudible::FC_GarbageCollect()
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	// Perform garbage collection on fading channels
	for (INT32 i = 0; i < m_fadingChannels.Count(); ++i)
	{
		ChannelDesc* channelDesc = m_fadingChannels[i];

		if (channelDesc->m_soundChannel == NULL || channelDesc->m_soundChannel->IsPlaying() == FALSE)
		{
			// ...
			UTIL_UnregisterHost(channelDesc->m_soundChannel);

			// Release the sound channel object
			ruSAFE_RELEASE(channelDesc->m_soundChannel);

			// Release the channel descriptor
			channelDesc->Release();

			// Remove from channel list
			m_fadingChannels.RemoveAt(i);
			--i;
		}
	}
}

void CRuAudible::MC_Clear()
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	for (INT32 i = 0; i < m_manualChannels.Count(); ++i)
	{
		ChannelDesc* channelDesc = m_manualChannels.GetByIndex(i);

		if (channelDesc->m_soundChannel)
		{
			// ...
			UTIL_UnregisterHost(channelDesc->m_soundChannel);

			// Stop playback
			channelDesc->m_soundChannel->Stop();

			// Release the sound channel object
			channelDesc->m_soundChannel->Release();

			// Clear sound channel
			channelDesc->m_soundChannel = NULL;
		}

		// Release the channel descriptor
		channelDesc->Release();
	}

	m_manualChannels.Clear();
}

void CRuAudible::MC_GarbageCollect()
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	for (INT32 i = 0; i < m_manualChannels.Count(); ++i)
	{
		ChannelDesc* channelDesc = m_manualChannels.GetByIndex(i);

		if (channelDesc->m_soundChannel == NULL || channelDesc->m_soundChannel->IsPlaying() == FALSE)
		{
			// ...
			UTIL_UnregisterHost(channelDesc->m_soundChannel);

			// Release the sound channel object
			ruSAFE_RELEASE(channelDesc->m_soundChannel);

			// Release the channel descriptor
			channelDesc->Release();

			// Remove from channel list
			m_manualChannels.RemoveAt(i);
			--i;
		}
	}
}

//TODO вызывать для смены векторов
INT32 CRuAudible::Host_OnTransformChanged(RuEventArgs* eventArgs)
{
	//CRuThread_ScopeLock scopeLock(m_cs_Global);

	//RuEntityEvent_TransformChanged_Args* xformChangedArgs = static_cast<RuEntityEvent_TransformChanged_Args*>(eventArgs);
	//IRuSoundChannel* soundChannel = reinterpret_cast<IRuSoundChannel*>(eventArgs->m_userData);

	//// Set 3D attributes
	//soundChannel->Set3DAttributes(static_cast<CRuEntity*>(xformChangedArgs->m_entity)->GetTranslation(ruFRAMETYPE_WORLD), CRuVector3(0.0f, 0.0f, 0.0f));

	return TRUE;
}

INT32 CRuAudible::TRK_Clear()
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	CRuArrayList<ChannelDesc*> channelList;

	// Enumerate all track channels
	m_tracks.Enumerate(channelList);

	for (INT32 i = 0; i < channelList.Count(); ++i)
	{
		// Perform fade-and-stop on the channel
		FadeAndStop(channelList[i]);
	}

	m_tracks.Clear();

	return TRUE;
}

void CRuAudible::TRK_CheckOverrides()
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	// Check overrides for expiration
	CRuArrayList<const char*> expiredTracks;
	m_trackOverrides.ForAllElements(boost::bind(&CRuAudible::TRK_CheckTrackExpiration, this, _1, _2, _3), &expiredTracks);

	for (INT32 i = 0; i < expiredTracks.Count(); ++i)
	{
		m_trackOverrides.Remove(expiredTracks[i]);
	}

	// Check tracks against overrides
	CRuArrayList<const char*> trackNames;
	m_tracks.EnumerateKeys(trackNames);

	for (INT32 i = 0; i < trackNames.Count(); ++i)
	{
		ChannelDesc* channelDesc = NULL;
		m_tracks.Get(trackNames[i], channelDesc);

		if (channelDesc && channelDesc->m_soundChannel)
		{
			channelDesc->m_soundChannel->SetMute(m_trackOverrides.ContainsKey(trackNames[i]));
		}
	}
}

INT32 CRuAudible::TRK_CheckTrackExpiration(const char* key, void* data, void* userData)
{
	ChannelDesc* channelDesc = *reinterpret_cast<ChannelDesc**>(data);
	CRuArrayList<const char*>& expiredTracks = *reinterpret_cast<CRuArrayList<const char*>*>(userData);

	// The track has expired if the sound channel does not exist or it is not playing
	if (channelDesc->m_soundChannel == NULL || channelDesc->m_soundChannel->IsPlaying() == FALSE)
	{
		// ...
		UTIL_UnregisterHost(channelDesc->m_soundChannel);

		// Release the sound channel object
		ruSAFE_RELEASE(channelDesc->m_soundChannel);

		// Release the channel descriptor
		channelDesc->Release();

		// ...
		expiredTracks.Add(key);
	}

	return TRUE;
}

CRuAudible::ChannelDesc* CRuAudible::TRK_GetChannelDesc(const char* track)
{
	CRuThread_ScopeLock scopeLock(m_cs_Global);

	// Create channel descriptor for the track if it does not exist
	if (m_tracks.ContainsKey(track) == FALSE)
	{
		return NULL;
	}

	ChannelDesc* channelDesc = NULL;
	m_tracks.Get(track, channelDesc);
	return channelDesc;
}

CRuAudible::ChannelDesc* CRuAudible::CHN_Play(IRuObject* host, const char* sndResPath, AudibleType type, REAL defVolume, UINT32 mode, INT32 loopCount)
{
	ChannelDesc* channelDesc = NULL;

	// Modify sound mode flags for looping parameter
	mode &= ~(ruSOUNDFLAG_LOOP_OFF | ruSOUNDFLAG_LOOP_NORMAL | ruSOUNDFLAG_LOOP_BIDIR);
	mode |= loopCount == 0 ? ruSOUNDFLAG_LOOP_OFF : ruSOUNDFLAG_LOOP_NORMAL;

	// Create sound handle
	PTRVALUE soundHandle = 0;
	BOOL loadResult = m_audioEngine->CreateSound(sndResPath, 0, mode & ruSOUNDFLAG_3D ? ruSOUNDFLAG_3D : ruSOUNDFLAG_2D, soundHandle);

	if (loadResult)
	{
		channelDesc = CHN_Start(host, soundHandle, type, defVolume, mode, loopCount);
	}

	if (channelDesc)
	{
		REAL minDistance = 5.0f;
		REAL maxDistance = 480.0f;
		if (mode & ruSOUNDFLAG_3D)
		{
			// If there is no host and it's a 3D sound, give it default min max distance
			channelDesc->m_soundChannel->Set3DMinMaxDistance(minDistance, maxDistance);
		}
	}

	return channelDesc;
}

CRuAudible::ChannelDesc* CRuAudible::CHN_Start(IRuObject* host, PTRVALUE sndHandle, AudibleType type, REAL defVolume, UINT32 mode, INT32 loopCount)
{
	ChannelDesc* channelDesc = NULL;

	// Play the requested sound handle
	IRuSoundChannel* soundChannel = m_audioEngine->PlaySound(sndHandle, TRUE);

	if (soundChannel)
	{
		// Create new channel descriptor
		channelDesc = ruNEW ChannelDesc(type, 0, NULL, defVolume, 0);

		channelDesc->m_soundHandle = sndHandle;
		channelDesc->m_soundChannel = soundChannel;

		// Set default sound parameters
		channelDesc->m_soundChannel->SetMode(mode);
		channelDesc->m_soundChannel->SetLoopCount(loopCount);
		channelDesc->m_soundChannel->SetVolume(channelDesc->m_soundDefVol * GetVolumeByType(channelDesc->m_audibleType));

		// Store host object
		channelDesc->m_soundChannel->SetHost(host);

		// TODO: Тут можно установить позицию звука
		// channelDesc->m_soundChannel->Set3DAttributes(hostEntityPos, CRuVector3(0.0f, 0.0f, 0.0f));
	}

	return channelDesc;
}

float CRuAudible::GetDefault3DMaxDist()
{
	return 480;
}

const CRuVector3& CRuAudible::GetListenerPosition()
{
	return m_camera->GetTargetPosition();
}

void CRuAudible::UTIL_UnregisterHost(IRuSoundChannel* soundChannel)
{

}
// ************************************************************************************************************************************************************
CRuAudible* g_ruAudible = NULL;
#pragma managed(pop)
