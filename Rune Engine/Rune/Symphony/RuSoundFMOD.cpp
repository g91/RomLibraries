/*!
	@file RuSoundFMOD.cpp

	Copyright (c) 2004-2011 Runewaker Entertainment Ltd. All rights reserved.
*/

#include "RuSoundFMOD.h"

#include "RuFMODChannel.h"
#include "RuFMODReverb.h"

#include <fmod.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <fmod_errors.h>
#include "../Core/Stream/RuStreamIO.h"
#include "../Engine/Manager/RuEngine_ResourceManager_Base.h"

#define ERR_NOSOUND     "FMOD error! (%d) %s. No sound will be playing.\n"
#define ERR_OLDVERSION  "Error! You are using an old version of FMOD %08x. This program requires %08x\n"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuSoundFMOD, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuSoundFMOD, "CRuSoundFMOD", "IRuObject")
ruCLASSGUID_DEFINE(CRuSoundFMOD, 0x3B5C9BD9, 0xFB1D4d2c, 0xABCF0A6C, 0x7D4E499C)

// ************************************************************************************************************************************************************

CRuSoundFMOD::Custom3DRolloffCallback CRuSoundFMOD::s_rolloffCallback = 0;

float F_CALLBACK FmodRolloffCallback(FMOD_CHANNEL* channel, float distance)
{
	return CRuSoundFMOD::s_rolloffCallback(NULL, distance);
}

// ************************************************************************************************************************************************************

CRuSoundFMOD::CRuSoundFMOD()
	: m_cs_Global(NULL),
	m_boInitialized(false),
	m_fmod(0),
	m_lastUpdateTime(0),
	m_suspended(FALSE)
{
	//m_soundCache[0] = ruNEW CRuStringHash<SoundDesc>(16);
	//m_soundCache[1] = ruNEW CRuStringHash<SoundDesc>(16);
	// ...
	m_cs_Global = ruNEW CRuThread_CriticalSection();

	// ...
	m_sysSFX_WhiteNoise[0] = 0;
	m_sysSFX_WhiteNoise[1] = 0;
}

CRuSoundFMOD::~CRuSoundFMOD()
{
}

void CRuSoundFMOD::Dispose()
{
	if (m_suspended)
	{
		m_fmod->mixerResume();
		m_suspended = FALSE;
	}

	// Terminate active channels
	TerminateActiveChannels();

	if (m_fmod)
	{
		m_fmod->release();
		m_fmod = NULL;
	}

	m_boInitialized = FALSE;

	// ...
	ruSAFE_DELETE(m_cs_Global);
}

INT32 CRuSoundFMOD::Initialize()
{
	FMOD_RESULT      result;

	if (m_boInitialized)
		return TRUE;

	// Create FMOD system
	result = FMOD::System_Create(&m_fmod);
	if (result != FMOD_OK)
	{
		fprintf(stderr, ERR_NOSOUND, result, FMOD_ErrorString(result));
		m_fmod = NULL;
		return FALSE;
	}

	// Get FMOD version
	unsigned int version;
	result = m_fmod->getVersion(&version);
	if (result != FMOD_OK)
	{
		fprintf(stderr, ERR_NOSOUND, result, FMOD_ErrorString(result));
		m_fmod->release();
		m_fmod = NULL;
		return FALSE;
	}

	// Check FMOD version against compiled version
	if (version < FMOD_VERSION)
	{
		FMODError("FMOD ERROR 1\n");

		fprintf(stderr, ERR_OLDVERSION, version, FMOD_VERSION);
		m_fmod->release();
		m_fmod = NULL;
		return FALSE;
	}

	// ...
	void* extradriverdata = 0;
	result = m_fmod->init(128, FMOD_INIT_NORMAL, extradriverdata);

	// ...
	FMOD_SPEAKERMODE  speakermode = FMOD_SPEAKERMODE_STEREO;
	result = m_fmod->getSoftwareFormat(0, &speakermode, 0);

	// Create required static sounds
	CreateSound("system\\sound\\white_noise.mp3", 0, ruSOUNDFLAG_2D, m_sysSFX_WhiteNoise[0]);
	CreateSound("system\\sound\\white_noise.mp3", 0, ruSOUNDFLAG_3D, m_sysSFX_WhiteNoise[1]);

	// ...
	m_boInitialized = TRUE;

	return TRUE;
}

INT32 CRuSoundFMOD::IsInitialized()
{
	return m_boInitialized;
}

FMOD::System* CRuSoundFMOD::GetFMOD()
{
	return m_fmod;
}

BOOL CRuSoundFMOD::CreateSound(const char* resourceName, UINT32 flags, DWORD soundFlags, PTRVALUE& soundHandleOut)
{

	// Sanity check
	if (!m_fmod || !resourceName)
		return FALSE;

	// Suspend check
	if (m_suspended)
	{
		FMODError("[ERROR] Attempting to access audio engine while suspended.");
		return FALSE;
	}

	// Determine cache index based on 2D/3D selection
	INT32 cacheIdx = soundFlags & FMOD_3D ? 1 : 0;

	// Search for sound from sound cache
	m_cs_Global->Enter();

	SoundDesc sndDesc;
	if (m_soundCache[cacheIdx].Get(resourceName, sndDesc) == FALSE)
	{
		m_cs_Global->Leave();

		// Load sound resource from resource manager
		IRuStream* soundStream = g_ruResourceManager->LoadStream(resourceName);

		if (soundStream == NULL)
		{
			return FALSE;
		}

		UINT32 fmodMode = FMOD_CREATECOMPRESSEDSAMPLE | FMOD_OPENMEMORY | 0;

		BYTE* soundBuffer = soundStream->OpenStreamMapping();
		INT32 soundBufferSize = soundStream->GetStreamSize();

		FMOD_CREATESOUNDEXINFO exInfo;

		memset(&exInfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
		exInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		exInfo.length = soundBufferSize;

		FMOD::Sound* fmodSound = NULL;
		FMOD_RESULT fmodResult = m_fmod->createSound(reinterpret_cast<const char*>(soundBuffer), fmodMode | soundFlags, &exInfo, &fmodSound);

		if (fmodResult == FMOD_OK)
		{

			CRuThread_ScopeLock scopeLock(m_cs_Global);

			// Setup sound descriptor
			sndDesc.m_state = ruSOUNDSTATE_VALID;
			sndDesc.m_soundFlags = soundFlags;
			sndDesc.m_soundHandle = reinterpret_cast<PTRVALUE>(fmodSound);
			sndDesc.m_soundStream = soundStream;

			// ...
			ruSAFE_DELETE(sndDesc.m_soundStream);

			// Store sound descriptor
			m_soundCache[cacheIdx].Add(resourceName, sndDesc);

			// Store to output
			soundHandleOut = sndDesc.m_soundHandle;

			return TRUE;

		}
		else
		{
			FMODError(FMOD_ErrorString(fmodResult));

			return TRUE;
		}

	}
	else
	{
		m_cs_Global->Leave();
	}
	soundHandleOut = sndDesc.m_soundHandle;
	return TRUE;
}

void CRuSoundFMOD::DiscardSound(const char* resourceName)
{
	if (!resourceName)
		return;

	// Suspend check
	if (m_suspended)
	{
		FMODError("[ERROR] Attempting to access audio engine while suspended.");
		return;
	}

	// Discard sound from all sound caches
	for (INT32 i = 0; i < 2; ++i)
	{
		PTRVALUE soundHandle = 0;

		// Search for sound from sound cache
		CRuThread_ScopeLock scopeLock(m_cs_Global);

		SoundDesc sndDesc;
		if (m_soundCache[i].Get(resourceName, sndDesc))
		{
			switch (sndDesc.m_state)
			{
			case ruSOUNDSTATE_INVALID:
				break;

			case ruSOUNDSTATE_PENDING_CREATE:
			case ruSOUNDSTATE_VALID:
			{
				// Release fmod sound
				FMOD::Sound* fmodSound = reinterpret_cast<FMOD::Sound*>(sndDesc.m_soundHandle);
				fmodSound->release();

				// Delete sound stream
				ruSAFE_DELETE(sndDesc.m_soundStream);

				// Remove from sound cache
				m_soundCache[i].Remove(resourceName);
			}
			break;
			}
		}
	}
}

IRuSoundChannel* CRuSoundFMOD::PlaySound(PTRVALUE soundHandle, INT32 startPaused)
{
	// Suspend check
	if (m_suspended)
	{
		FMODError("[ERROR] Attempting to access audio engine while suspended.");
		return NULL;
	}

	CRuFMODChannel* soundChannel = NULL;

	FMOD::Channel* fmodChannel;
	FMOD::Sound* fmodSound = reinterpret_cast<FMOD::Sound*>(soundHandle);
	FMOD_RESULT fmodResult = m_fmod->playSound(fmodSound, NULL, startPaused ? true : false, &fmodChannel);

	if (fmodResult == FMOD_OK)
	{
		// Initialize the sound channel
		soundChannel = ruNEW CRuFMODChannel();
		soundChannel->Initialize(m_fmod, fmodChannel, fmodSound);

		// Store a reference to the sound channel
		soundChannel->AddRef();
		m_activeChannels.Add(soundChannel);
	}
	else
	{
		FMODError(FMOD_ErrorString(fmodResult));
		FMODError("\n");
	}

	return soundChannel;
}

IRuSoundReverb* CRuSoundFMOD::CreateReverb()
{
	// Suspend check
	if (m_suspended)
	{
		FMODError("[ERROR] Attempting to access audio engine while suspended.");
		return NULL;
	}

	if (!m_fmod)
		return NULL;

	FMOD::Reverb3D* fmodReverb = NULL;
	FMOD_RESULT fmodResult = m_fmod->createReverb3D(&fmodReverb);

	if (fmodResult == FMOD_OK)
	{
		CRuFMODReverb* reverb = ruNEW CRuFMODReverb();
		reverb->Initialize(fmodReverb);

		return reverb;
	}

	return NULL;
}

RuSoundReverbProperties CRuSoundFMOD::GetReverbPreset(const char* presetName)
{
	struct ReverbPresetDesc
	{
		const char* m_name;
		FMOD_REVERB_PROPERTIES				m_reverbProps;
	};

	ReverbPresetDesc reverbPresets[] = {
											"alley",				FMOD_PRESET_ALLEY,
											"arena",				FMOD_PRESET_ARENA,
											"auditorium",			FMOD_PRESET_AUDITORIUM,
											"bathroom",				FMOD_PRESET_BATHROOM,
											"carpeted hallway",		FMOD_PRESET_CARPETTEDHALLWAY,
											"cave",					FMOD_PRESET_CAVE,
											"city",					FMOD_PRESET_CITY,
											"concert hall",			FMOD_PRESET_CONCERTHALL,
											"forest",				FMOD_PRESET_FOREST,
											"generic",				FMOD_PRESET_GENERIC,
											"hanger",				FMOD_PRESET_HANGAR,
											"living room",			FMOD_PRESET_LIVINGROOM,
											"mountains",			FMOD_PRESET_MOUNTAINS,
											"off",					FMOD_PRESET_OFF,
											"padded cell",			FMOD_PRESET_PADDEDCELL,
											"parking lot",			FMOD_PRESET_PARKINGLOT,
											"plain",				FMOD_PRESET_PLAIN,
											"quarry",				FMOD_PRESET_QUARRY,
											"room",					FMOD_PRESET_ROOM,
											"sewer pipe",			FMOD_PRESET_SEWERPIPE,
											"stone corridor",		FMOD_PRESET_STONECORRIDOR,
											"stone room",			FMOD_PRESET_STONEROOM,
											"underwater",			FMOD_PRESET_UNDERWATER,
											NULL,					FMOD_PRESET_OFF
	};

	RuSoundReverbProperties reverbProps;

	INT32 i = 0;
	for (; reverbPresets[i].m_name; ++i)
	{
		if (stricmp(presetName, reverbPresets[i].m_name) == 0)
		{
			memcpy(&reverbProps, &reverbPresets[i].m_reverbProps, sizeof(RuSoundReverbProperties));
			break;
		}
	}

	if (reverbPresets[i].m_name == NULL)
	{
		memcpy(&reverbProps, &reverbPresets[i].m_reverbProps, sizeof(RuSoundReverbProperties));
	}

	return reverbProps;
}

INT32 CRuSoundFMOD::Set3DListenerAttributes(INT32 listenerIdx, const CRuVector3& position, const CRuVector3& velocity, const CRuVector3& forward, const CRuVector3& up)
{
	// Suspend check
	if (m_suspended)
	{
		FMODError("[ERROR] Attempting to access audio engine while suspended.");
		return FALSE;
	}

	if (!m_fmod)
		return FALSE;

	FMOD_VECTOR fmodPos = { position.m_x, position.m_y, position.m_z };
	FMOD_VECTOR fmodVel = { velocity.m_x, velocity.m_y, velocity.m_z };
	FMOD_VECTOR fmodForward = { forward.m_x, forward.m_y, forward.m_z };
	FMOD_VECTOR fmodUp = { up.m_x, up.m_y, up.m_z };

	FMOD_RESULT fmodResult = m_fmod->set3DListenerAttributes(listenerIdx, &fmodPos, &fmodVel, &fmodForward, &fmodUp);

	m_fmod->set3DNumListeners(1);

	return fmodResult == FMOD_OK;
}

INT32 CRuSoundFMOD::Set3DRolloffCallback(Custom3DRolloffCallback callback)
{
	s_rolloffCallback = callback;

	if (s_rolloffCallback)
	{
		m_fmod->set3DRolloffCallback((FMOD_3D_ROLLOFF_CALLBACK)FmodRolloffCallback);
	}
	else
	{
		m_fmod->set3DRolloffCallback(NULL);
	}

	return TRUE;
}

INT32 CRuSoundFMOD::Set3DSettings(REAL dopplerScale, REAL distanceFactor, REAL rolloffScale)
{
	// Suspend check
	if (m_suspended)
	{
		FMODError("[ERROR] Attempting to access audio engine while suspended.");
		return FALSE;
	}

	if (!m_fmod)
		return FALSE;

	FMOD_RESULT fmodResult = m_fmod->set3DSettings(dopplerScale, distanceFactor, rolloffScale);

	return fmodResult == FMOD_OK;
}

INT32 CRuSoundFMOD::SetAmbientReverb(const RuSoundReverbProperties& reverbProps)
{
	// Suspend check
	if (m_suspended)
	{
		FMODError("[ERROR] Attempting to access audio engine while suspended.");
		return FALSE;
	}

	if (!m_fmod)
		return FALSE;

	// Transcribe properties to FMOD structure
	FMOD_REVERB_PROPERTIES fmodReverbProps;
	memcpy(&fmodReverbProps, &reverbProps, sizeof(RuSoundReverbProperties));

	// Set ambient reverb
	FMOD_RESULT fmodResult = m_fmod->setReverbProperties(0, &fmodReverbProps);

	return fmodResult == FMOD_OK;
}

void CRuSoundFMOD::Resume()
{
	if (!m_fmod)
		return;

	// Resume mixer
	m_fmod->mixerResume();

	m_suspended = FALSE;
}

void CRuSoundFMOD::Suspend()
{
	if (!m_fmod)
		return;

	// Suspend mixer
	m_fmod->mixerSuspend();

	m_suspended = TRUE;
}

INT32 CRuSoundFMOD::Update(PTRVALUE userData)
{
	if (!m_fmod || m_suspended)
		return FALSE;

	// Process pending sound streams
	{
		CRuThread_ScopeLock scopeLock(m_cs_Global);

		for (INT32 i = 0; i < 2; ++i)
		{
			for (INT32 j = 0, count = m_pendingSoundStreams[i].Count(); j < count; ++j)
			{
				char* sndResName = m_pendingSoundStreams[i].Dequeue();

				// Retrieve sound descriptor for the pending sound
				SoundDesc sndDesc;
				if (!m_soundCache[i].Get(sndResName, sndDesc))
				{
					// Cleanup
					FMODError("[ERROR] Sound discarded during FMOD creation.\n");
					ruSAFE_DELETE_ARRAY(sndResName);

					continue;
				}

				FMOD_OPENSTATE openState;
				FMOD_RESULT fmodResult = reinterpret_cast<FMOD::Sound*>(sndDesc.m_soundHandle)->getOpenState(&openState, NULL, NULL, NULL);
				if (fmodResult == FMOD_OK)
				{
					if (openState == FMOD_OPENSTATE_READY)
					{
						// ...
						ruSAFE_DELETE(sndDesc.m_soundStream);

						// Set sound descriptor state to valid
						sndDesc.m_state = ruSOUNDSTATE_VALID;
						m_soundCache[i].Set(sndResName, sndDesc);

						// Cleanup
						ruSAFE_DELETE_ARRAY(sndResName);
					}
					else
					{
						// Not ready, enqueue for future processing
						m_pendingSoundStreams[i].Queue(sndResName);
					}
				}
				else
				{
					char outStr[2048];
					sprintf(outStr, "[ERROR] [FMOD] %s\n", FMOD_ErrorString(fmodResult));
					FMODError(outStr);
				}
			}
		}
	}

	// ...
	DWORD t = timeGetTime();
	REAL elapsedTime = (t - m_lastUpdateTime) * 0.001f;
	m_lastUpdateTime = t;

	// Process active channels
	for (INT32 i = 0; i < m_activeChannels.Count(); ++i)
	{
		// Update channel
		m_activeChannels[i]->Update(elapsedTime);

		// If channel is no longer active, free it
		if (m_activeChannels[i]->IsPlaying() == FALSE)
		{
			RuSymphonyEvent_PlaybackFinished_Args playbackFinishedArgs(reinterpret_cast<PTRVALUE>(m_activeChannels[i]));
			m_event_PlaybackFinished.Trigger(&playbackFinishedArgs);

			ruSAFE_RELEASE(m_activeChannels[i]);

			m_activeChannels.RemoveAt(i);
			--i;
		}
	}

	// TODO: Garbage collect unused sounds

	// Pump FMOD
	m_fmod->update();

	return TRUE;
}

void CRuSoundFMOD::TerminateActiveChannels()
{
	for (INT32 i = 0; i < m_activeChannels.Count(); ++i)
	{
		// Stop playback
		m_activeChannels[i]->Stop();

		// Release channel
		ruSAFE_RELEASE(m_activeChannels[i]);
	}

	m_activeChannels.Clear();
}

void CRuSoundFMOD::FMODError(const char* format, ...)
{
	static char buf[4096];
	if (format == NULL)
		return;
	va_list args;
	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);
	OutputDebugString(buf);
}

CRuEvent& CRuSoundFMOD::Event_PlaybackFinished()
{
	return m_event_PlaybackFinished;
}

CRuSoundFMOD* g_ruSoundFMOD = NULL;
// ************************************************************************************************************************************************************

#pragma managed(pop)
