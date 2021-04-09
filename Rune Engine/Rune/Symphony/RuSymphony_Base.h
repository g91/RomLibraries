#pragma once
// Base types
#include "../Core/Type/RuType_Object.h"
#include "../Core/Math/RuMathVector.h"
#include "../Core/Thread/RuThread_CriticalSection.h"

//#include "fmod.hpp"
//#include "fmod_errors.h"
// Miles Sound System
//#include "mss.h"
// Import Miles Sound System library
//#pragma comment(lib, "mss64.lib")
#define FMOD_SOUND_ENABLE 0
#ifdef FMOD_SOUND_ENABLE
#include "fmod.hpp"
#include "fmod_errors.h"
#include "vector"
#pragma comment(lib, "fmod_vc.lib")
#endif
#pragma managed(push, off)
// ************************************************************************************************************************************************************
enum RuAudioLoopMode
{
	ruAUDIOLOOPMODE_NONE = 0,
	ruAUDIOLOOPMODE_LOOP_250MS = 1,									//!< Loop cross-fade 250 MS
	ruAUDIOLOOPMODE_LOOP_500MS = 2,									//!< Loop cross-fade 250 MS
	ruAUDIOLOOPMODE_LOOP_1000MS = 3,									//!< Loop cross-fade 1000 MS
	ruAUDIOLOOPMODE_LOOP_RANDOM_1000MS = 4,									//!< Random loop cross-fade 250 - 1000 MS
	ruAUDIOLOOPMODE_LOOP_RANDOM_2000MS = 5,									//!< Random loop cross-fade 500 - 2000 MS
	ruAUDIOLOOPMODE_LOOP_RANDOM_PGM_0 = 6,
	ruAUDIOLOOPMODE_FORCE_DWORD = 0x7FFFFFFF
};
// ************************************************************************************************************************************************************
enum RuSoundType
{
	ruSOUNDTYPE_SFX = 0,									//!< Sound FX
	ruSOUNDTYPE_AMBIENCE = 1,									//!< Ambience
	ruSOUNDTYPE_MUSIC = 2,									//!< Music
	ruSOUNDTYPE_INTERFACE = 3										//!< Interface
};
// ************************************************************************************************************************************************************
class RuSymphonyEvent_PlaybackFinished_Args : public RuEventArgs
{
public:
	PTRVALUE									m_soundHandle;						//!< Sound handle which finished playback
	RuSymphonyEvent_PlaybackFinished_Args(PTRVALUE soundHandle)
		: m_soundHandle(soundHandle)
	{
	}
};
struct FMODSoundDescriptor
{
	FMOD::Sound* m_sound;
	FMOD::Channel* m_channel;
	FMOD::System* m_system;
	REAL m_fadeClock;
	REAL m_fadeTime;
	INT32 m_soundType;
	bool m_enable3DPositioning;
	bool m_pinToListener;
	CRuVector3 m_3dPosition;
	FMODSoundDescriptor();
	~FMODSoundDescriptor();
	void Initialize(FMOD::System* fmod, FMOD::Channel* fmodChannel, FMOD::Sound* fmodSound, INT32 soundType);
	void Update(REAL elapsedTime);
	int IsPlaying();
	int Stop();
	void Release();
	int FadeAndStop(REAL fadeTime);
	int SetVolume(REAL volume);
	REAL GetVolume();
	void FMODError(const char* format, ...);
};
// ************************************************************************************************************************************************************
class CRuSymphony : public IRuObject
{
	ruRTTI_DECLARE
		ruHEAP_DECLARE
		ruCLASSGUID_DECLARE
protected:
	// Events
	CRuEvent m_event_PlaybackFinished;			//!< Sample playback finished
	// Settings
	REAL m_defaults_3D_MinDist;
	REAL m_defaults_3D_MaxDist;
	REAL m_defaults_3D_Rolloff;				//!< Default rolloff factor
	CRuVector3 m_listenerPosition;					//!< Position of the listener
	CRuVector3 m_listenerForward;					//!< Forward direction of the listener
	CRuVector3 m_listenerUp;						//!< Upward direction of the listener
public:
	CRuSymphony();
	virtual ~CRuSymphony();
	// Events
	CRuEvent& Event_PlaybackFinished();
	// Startup & Shutdown
	BOOL Startup();
	BOOL Shutdown();
	// Listener positioning and falloff
	const CRuVector3& GetListenerPosition();
	BOOL SetListenerPosition(const CRuVector3& listenerPosition);
	BOOL SetListenerOrientation(const CRuVector3& listenerForward, const CRuVector3& listenerUp);
	BOOL SetDefault3DSoundDistances(REAL minDist, REAL maxDist);
	BOOL SetDefault3DSoundRolloff(REAL rolloff);
	REAL GetDefault3DMaxDist();
	// Playback
	PTRVALUE PlaySoundByPath(const char* soundPath, BOOL loop, REAL volumeLevel = 0.881f, INT32 soundType = ruSOUNDTYPE_SFX);
	PTRVALUE PlaySound(const char* soundPath, INT32 soundType, REAL volumeLevel, BOOL loop);
	// PTRVALUE Play3DSoundByPath(const char* soundPath, REAL x, REAL y, REAL z, RuAudioLoopMode loop, BOOL pinToListener = FALSE, INT32 soundType = ruSOUNDTYPE_SFX, REAL volumeLevel = 0.881f);	
	PTRVALUE Play3DSound(const char* soundPath, INT32 soundType, REAL volumeLevel, BOOL loop, const CRuVector3& position, BOOL pinToListener);
	CRuVector3 Get3DSoundPosition(PTRVALUE soundHandle);
	BOOL Set3DSoundPosition(PTRVALUE soundHandle, const CRuVector3& position);
	// Playback (Deprecated)
	// Playback alteration
	BOOL SetMasterSoundVolume(REAL volumeLevel);
	BOOL AdjustSoundVolumeByType(INT32 soundType, REAL volumeLevel);
	// Termination
	BOOL FadeOutSound(PTRVALUE soundHandle, REAL fadeOutTimeMS = 250.0f);
	BOOL StopSound(PTRVALUE soundHandle);
	// Update
	BOOL Update(REAL elapsedTime);
	// Cache management
	BOOL ClearUnusedSoundDescriptors();
	REAL GetVolume(REAL volume);
protected:
	// Resource retrieval
	BOOL VolumeRamp(PTRVALUE sampleHandle, REAL level0, REAL level1, REAL duration);
	BOOL SoundVolumeRampIsZero(PTRVALUE soundHandle);
	BOOL Update3DSoundChannelLevels(FMODSoundDescriptor* descriptor);
	BOOL ClearUnusedSoundDescriptorsCallback(const void* key, void* data);
	BOOL GCSoundDescriptors();
	BOOL GCSoundDescriptorsCallback(const void* key, void* data);
	BOOL ClearSoundDescriptors();
	BOOL ClearSoundDescriptorsCallback(const void* key, void* data);

protected:
	FMOD::System* m_fmodSystem;
	CRuArrayList<FMODSoundDescriptor*> m_activeDescriptors;
	REAL m_masterVolume;

	void FMODError(const char* format, ...);
};
// ************************************************************************************************************************************************************
extern CRuSymphony* g_ruSymphony;
// ************************************************************************************************************************************************************
#pragma managed(pop)