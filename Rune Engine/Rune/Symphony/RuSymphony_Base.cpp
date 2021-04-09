#include "../Symphony/RuSymphony_Base.h"
#include "../Core/Stream/RuStreamIO.h"
#include "../Engine/Manager/RuEngine_ResourceManager_Base.h"
#include <string.h>
#pragma managed(push, off)
// ************************************************************************************************************************************************************
ruRTTI_DEFINE_SUBCLASS(CRuSymphony, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuSymphony, "CRuSymphony", "IRuObject")
ruCLASSGUID_DEFINE(CRuSymphony, 0x6136F6FF, 0x768844e4, 0x9B1E91BE, 0x694C20AD)
// ************************************************************************************************************************************************************
CRuSymphony::CRuSymphony()
{
	m_defaults_3D_MinDist = 1.0f;
	m_defaults_3D_MaxDist = 480.0f;
	m_defaults_3D_Rolloff = 0.10f;
#ifdef FMOD_SOUND_ENABLE
	m_activeDescriptors.Clear();
	m_fmodSystem = NULL;
	m_masterVolume = 1.0f;
#endif
}
CRuSymphony::~CRuSymphony()
{
}
CRuEvent& CRuSymphony::Event_PlaybackFinished()
{
	return m_event_PlaybackFinished;
}
BOOL CRuSymphony::Startup()
{
	FMODError("CRuSymphony::Startup\n");

	FMOD_RESULT result;
	result = FMOD::System_Create(&m_fmodSystem);
	if (result != FMOD_OK)
	{
		FMODError("Startup FMOD 1 error! (%d) %s\n", result, FMOD_ErrorString(result));
		return FALSE;
	}

	result = m_fmodSystem->init(512, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
	{
		FMODError("Startup FMOD 2 error! (%d) %s\n", result, FMOD_ErrorString(result));
		return FALSE;
	}

	return TRUE;
}
BOOL CRuSymphony::Shutdown()
{
	FMODError("CRuSymphony::Shutdown\n");
	if (m_fmodSystem == NULL)
		return FALSE;

	for (INT32 i = 0; i < m_activeDescriptors.Count(); ++i)
	{
		FMODSoundDescriptor* descriptor = m_activeDescriptors[i];
		descriptor->Stop();
		ruSAFE_RELEASE(descriptor);
		delete descriptor;
	}

	m_activeDescriptors.Clear();

	FMOD_RESULT result = m_fmodSystem->release();
	if (result != FMOD_OK)
	{
		FMODError("Shutdown FMOD 1 error! (%d) %s\n", result, FMOD_ErrorString(result));
		return FALSE;
	}

	return TRUE;
}
const CRuVector3& CRuSymphony::GetListenerPosition()
{
	return m_listenerPosition;
}
BOOL CRuSymphony::SetListenerPosition(const CRuVector3& listenerPosition)
{
	if (m_fmodSystem == NULL)
		return FALSE;

	CRuVector3 oldListenerPosition = m_listenerPosition;
	m_listenerPosition = listenerPosition;
	for (INT32 i = 0; i < m_activeDescriptors.Count(); ++i)
	{
		if (m_activeDescriptors[i]->m_enable3DPositioning && m_activeDescriptors[i]->m_pinToListener)
		{
			if (m_activeDescriptors[i]->IsPlaying())
			{
				CRuVector3 relativePosition = m_activeDescriptors[i]->m_3dPosition - oldListenerPosition;
				m_activeDescriptors[i]->m_3dPosition = relativePosition + m_listenerPosition;
			}
		}
	}

	FMOD_VECTOR fmodPos = { m_listenerPosition.m_x, m_listenerPosition.m_y, m_listenerPosition.m_z };
	FMOD_VECTOR fmodForward = { m_listenerForward.m_x, m_listenerForward.m_y, m_listenerForward.m_z };
	FMOD_VECTOR fmodUp = { m_listenerUp.m_x, m_listenerUp.m_y, m_listenerUp.m_z };

	FMOD_RESULT result = m_fmodSystem->set3DListenerAttributes(0, &fmodPos, 0, &fmodForward, &fmodUp);
	//FMOD_RESULT result = m_fmodSystem->set3DListenerAttributes(0, &fmodPos, 0, 0, 0);
	if (result != FMOD_OK)
	{
		FMODError("SetListenerPosition FMOD 1 error! code:%d error:%s\n", result, FMOD_ErrorString(result));
		return NULL;
	}

	result = m_fmodSystem->set3DNumListeners(1);
	if (result != FMOD_OK)
	{
		FMODError("SetListenerPosition FMOD 2 error! code:%d error:%s\n", result, FMOD_ErrorString(result));
		return NULL;
	}

	return TRUE;
}
BOOL CRuSymphony::SetListenerOrientation(const CRuVector3& listenerForward, const CRuVector3& listenerUp)
{
	m_listenerForward = listenerForward;
	m_listenerUp = listenerUp;
	return TRUE;
}
BOOL CRuSymphony::SetDefault3DSoundDistances(REAL minDist, REAL maxDist)
{
	OutputDebugString("CRuSymphony::SetDefault3DSoundDistances\n");
	if (m_fmodSystem == NULL)
		return FALSE;
	m_defaults_3D_MinDist = minDist;
	m_defaults_3D_MaxDist = maxDist;
	return TRUE;
}
BOOL CRuSymphony::SetDefault3DSoundRolloff(REAL rolloff)
{
	OutputDebugString("CRuSymphony::SetDefault3DSoundRolloff\n");
	if (m_fmodSystem == NULL)
		return FALSE;
	m_defaults_3D_Rolloff = rolloff;
	return FALSE;
}
REAL CRuSymphony::GetDefault3DMaxDist()
{
	//OutputDebugString("CRuSymphony::GetDefault3DMaxDist\n");
	return m_defaults_3D_MaxDist;
}
PTRVALUE CRuSymphony::PlaySoundByPath(const char* soundPath, BOOL loop, REAL volumeLevel, INT32 soundType)
{
	return PlaySound(soundPath, soundType, volumeLevel, loop);
}
PTRVALUE CRuSymphony::PlaySound(const char* soundPath, INT32 soundType, REAL volumeLevel, BOOL loop)
{
	if (m_fmodSystem == NULL)
		return NULL;

	IRuStream* sampleStream = g_ruResourceManager->LoadStream(soundPath);
	if (sampleStream == NULL)
		return NULL;

	FMOD_CREATESOUNDEXINFO exInfo;
	memset(&exInfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	exInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	exInfo.length = sampleStream->GetStreamSize();

	char* soundData = new char[sampleStream->GetStreamSize()];
	sampleStream->Read(soundData, sampleStream->GetStreamSize());
	delete sampleStream;

	FMOD::Sound* sound = NULL;
	FMOD_MODE mode = FMOD_2D | FMOD_OPENMEMORY;
	if (loop)
	{
		FMODError("PlaySound loop not realised file:%s\n", soundPath);
		mode |= FMOD_LOOP_NORMAL;
	}
	else
		mode |= FMOD_LOOP_OFF;

	FMOD_RESULT result = m_fmodSystem->createSound(soundData, mode, &exInfo, &sound);
	delete[]soundData;
	if (result != FMOD_OK)
	{
		FMODError("PlaySound FMOD 1 error! code:%d error:%s file:%s\n", result, FMOD_ErrorString(result), soundPath);
		return NULL;
	}

	FMOD::Channel* channel;
	result = m_fmodSystem->playSound(sound, NULL, true, &channel);
	if (result != FMOD_OK)
	{
		FMODError("PlaySound FMOD 2 error! code:%d error:%s file:%s\n", result, FMOD_ErrorString(result), soundPath);
		return NULL;
	}

	channel->setVolumeRamp(true);
	if (result != FMOD_OK)
	{
		FMODError("PlaySound FMOD 3 error! code:%d error:%s file:%s\n", result, FMOD_ErrorString(result), soundPath);
		return NULL;
	}

	channel->setVolume(GetVolume(volumeLevel));
	if (result != FMOD_OK)
	{
		FMODError("PlaySound FMOD 4 error! code:%d error:%s file:%s\n", result, FMOD_ErrorString(result), soundPath);
		return NULL;
	}

	// TODO: Предлагается установить рампу от нуля.

	channel->setPaused(false);
	if (result != FMOD_OK)
	{
		FMODError("PlaySound FMOD 5 error! code:%d error:%s file:%s\n", result, FMOD_ErrorString(result), soundPath);
		return NULL;
	}

	FMODSoundDescriptor* descriptor = new FMODSoundDescriptor;
	descriptor->Initialize(m_fmodSystem, channel, sound, soundType);
	m_activeDescriptors.Add(descriptor);

	return (PTRVALUE)sound;
}
PTRVALUE CRuSymphony::Play3DSound(const char* soundPath, INT32 soundType, REAL volumeLevel, BOOL loop, const CRuVector3& position, BOOL pinToListener)
{
	if (m_fmodSystem == NULL)
		return NULL;

	REAL maxThreshold = m_defaults_3D_MaxDist * 1.10f;
	REAL distance = (position - m_listenerPosition).Magnitude();

	if (distance > maxThreshold)
		return NULL;

	IRuStream* sampleStream = g_ruResourceManager->LoadStream(soundPath);
	if (sampleStream == NULL)
		return NULL;

	FMOD_CREATESOUNDEXINFO exInfo;
	memset(&exInfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	exInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	exInfo.length = sampleStream->GetStreamSize();

	char* soundData = new char[sampleStream->GetStreamSize()];
	sampleStream->Read(soundData, sampleStream->GetStreamSize());
	delete sampleStream;

	FMOD::Sound* sound = NULL;
	FMOD_MODE mode = FMOD_3D | FMOD_OPENMEMORY;
	if (loop)
	{
		FMODError("Play3DSound loop not realised file:%s\n", soundPath);
		mode |= FMOD_LOOP_NORMAL;
	}
	else
		mode |= FMOD_LOOP_OFF;

	FMOD_RESULT result = m_fmodSystem->createSound(soundData, mode, &exInfo, &sound);
	delete[]soundData;
	if (result != FMOD_OK)
	{
		FMODError("Play3DSound FMOD 1 error! code:%d error:%s file:%s\n", result, FMOD_ErrorString(result), soundPath);
		return NULL;
	}

	FMOD::Channel* channel;
	result = m_fmodSystem->playSound(sound, NULL, true, &channel);
	if (result != FMOD_OK)
	{
		FMODError("Play3DSound FMOD 2 error! code:%d error:%s file:%s\n", result, FMOD_ErrorString(result), soundPath);
		return NULL;
	}

	channel->setVolumeRamp(true);
	if (result != FMOD_OK)
	{
		FMODError("Play3DSound FMOD 3 error! code:%d error:%s file:%s\n", result, FMOD_ErrorString(result), soundPath);
		return NULL;
	}

	channel->setVolume(GetVolume(volumeLevel));
	if (result != FMOD_OK)
	{
		FMODError("Play3DSound FMOD 4 error! code:%d error:%s file:%s\n", result, FMOD_ErrorString(result), soundPath);
		return NULL;
	}

	// TODO: Предлагается установить рампу от нуля.

	channel->setPaused(false);
	if (result != FMOD_OK)
	{
		FMODError("Play3DSound FMOD 5 error! code:%d error:%s file:%s\n", result, FMOD_ErrorString(result), soundPath);
		return NULL;
	}

	FMODSoundDescriptor* descriptor = new FMODSoundDescriptor;
	descriptor->Initialize(m_fmodSystem, channel, sound, soundType);
	descriptor->m_enable3DPositioning = TRUE;
	descriptor->m_pinToListener = pinToListener;
	descriptor->m_3dPosition = position;
	m_activeDescriptors.Add(descriptor);

	Update3DSoundChannelLevels(descriptor);

	return (PTRVALUE)sound;
}
BOOL CRuSymphony::Set3DSoundPosition(PTRVALUE soundHandle, const CRuVector3& position)
{
	FMODError("CRuSymphony::Set3DSoundPosition\n");
	if (m_fmodSystem == NULL)
		return FALSE;

	for (INT32 i = 0; i < m_activeDescriptors.Count(); ++i)
	{
		if ((PTRVALUE)m_activeDescriptors[i]->m_sound == soundHandle)
		{
			m_activeDescriptors[i]->m_3dPosition = position;
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CRuSymphony::SetMasterSoundVolume(REAL volumeLevel)
{
	//FMODError("CRuSymphony::SetMasterSoundVolume\n");
	if (m_fmodSystem == NULL)
		return FALSE;

	m_masterVolume = volumeLevel;
	for (INT32 i = 0; i < m_activeDescriptors.Count(); ++i)
	{
		m_activeDescriptors[i]->SetVolume(GetVolume(m_activeDescriptors[i]->GetVolume()));
	}

	return TRUE;
}
BOOL CRuSymphony::AdjustSoundVolumeByType(INT32 soundType, REAL volumeLevel)
{
	//FMODError("CRuSymphony::AdjustSoundVolumeByType\n");
	if (m_fmodSystem == NULL)
		return FALSE;

	for (INT32 i = 0; i < m_activeDescriptors.Count(); ++i)
	{
		if (m_activeDescriptors[i]->m_soundType == soundType)
		{
			if (m_activeDescriptors[i]->IsPlaying())
			{
				m_activeDescriptors[i]->SetVolume(GetVolume(volumeLevel));
			}
		}
	}
	return TRUE;
}
BOOL CRuSymphony::FadeOutSound(PTRVALUE soundHandle, REAL fadeOutTimeMS)
{
	//FMODError("CRuSymphony::FadeOutSound\n");
	if (m_fmodSystem == NULL)
		return FALSE;

	for (INT32 i = 0; i < m_activeDescriptors.Count(); ++i)
	{
		if ((PTRVALUE)m_activeDescriptors[i]->m_sound == soundHandle)
		{
			m_activeDescriptors[i]->FadeAndStop(fadeOutTimeMS / 1000.0f);
			return TRUE;
		}
	}

	return FALSE;
}
BOOL CRuSymphony::StopSound(PTRVALUE soundHandle)
{
	FMODError("CRuSymphony::StopSound\n");
	if (m_fmodSystem == NULL)
		return FALSE;

	for (INT32 i = 0; i < m_activeDescriptors.Count(); ++i)
	{
		if ((PTRVALUE)m_activeDescriptors[i]->m_sound == soundHandle)
		{
			m_activeDescriptors[i]->Stop();
			return TRUE;
		}
	}

	return FALSE;
}
BOOL CRuSymphony::Update(REAL elapsedTime)
{
	//FMODError("CRuSymphony::Update\n");
	if (m_fmodSystem == NULL)
		return FALSE;
	m_fmodSystem->update();
	for (INT32 i = 0; i < m_activeDescriptors.Count(); ++i)
	{
		// Update channel
		m_activeDescriptors[i]->Update(elapsedTime);
		// Update 3D sound channel level
		Update3DSoundChannelLevels(m_activeDescriptors[i]);
		// If channel is no longer active, free it
		if (m_activeDescriptors[i]->IsPlaying() == FALSE)
		{
			FMODSoundDescriptor* descriptor = m_activeDescriptors[i];
			// TODO: Вызываем калбэк для закончевшейся музыки
			RuSymphonyEvent_PlaybackFinished_Args playbackFinishedArgs(reinterpret_cast<PTRVALUE>(descriptor->m_sound));
			m_event_PlaybackFinished.Trigger(&playbackFinishedArgs);
			// TODO: Возможно нужна реализация Release
			ruSAFE_RELEASE(descriptor);
			m_activeDescriptors.RemoveAt(i);
			delete descriptor;
			--i;
		}
	}

	return TRUE;
}
BOOL CRuSymphony::ClearUnusedSoundDescriptors()
{
	OutputDebugString("CRuSymphony::ClearUnusedSoundDescriptors\n");
	return FALSE;
}
BOOL CRuSymphony::VolumeRamp(PTRVALUE sampleHandle, REAL level0, REAL level1, REAL duration)
{
	OutputDebugString("CRuSymphony::VolumeRamp\n");
	return FALSE;
}
BOOL CRuSymphony::SoundVolumeRampIsZero(PTRVALUE soundHandle)
{
	OutputDebugString("CRuSymphony::SoundVolumeRampIsZero\n");
	return FALSE;
}
BOOL CRuSymphony::Update3DSoundChannelLevels(FMODSoundDescriptor* descriptor)
{
	//FMODError("CRuSymphony::Update3DSoundChannelLevels\n");
	if (m_fmodSystem == NULL)
		return FALSE;

	if (!descriptor->m_enable3DPositioning)
		return FALSE;

	if (!descriptor->IsPlaying())
		return FALSE;

	FMOD_VECTOR fmodPos = { descriptor->m_3dPosition.m_x, descriptor->m_3dPosition.m_y, descriptor->m_3dPosition.m_z };
	descriptor->m_channel->set3DAttributes(&fmodPos, 0);
	descriptor->m_channel->set3DMinMaxDistance(m_defaults_3D_MinDist, m_defaults_3D_MaxDist);
	return FALSE;
}
BOOL CRuSymphony::ClearUnusedSoundDescriptorsCallback(const void* key, void* data)
{
	OutputDebugString("CRuSymphony::ClearUnusedSoundDescriptorsCallback\n");
	return FALSE;
}
BOOL CRuSymphony::GCSoundDescriptors()
{
	OutputDebugString("CRuSymphony::GCSoundDescriptors\n");
	return FALSE;
}
BOOL CRuSymphony::GCSoundDescriptorsCallback(const void* key, void* data)
{
	OutputDebugString("CRuSymphony::GCSoundDescriptorsCallback\n");
	return FALSE;
}
BOOL CRuSymphony::ClearSoundDescriptors()
{
	OutputDebugString("CRuSymphony::ClearSoundDescriptors\n");
	return FALSE;
}
BOOL CRuSymphony::ClearSoundDescriptorsCallback(const void* key, void* data)
{
	OutputDebugString("CRuSymphony::ClearSoundDescriptorsCallback\n");
	return FALSE;
}
FMODSoundDescriptor::FMODSoundDescriptor()
{
	m_system = NULL;
	m_sound = NULL;
	m_channel = NULL;
	m_fadeTime = -1.0f;
	m_fadeClock = 0.0f;
	m_soundType = 0;
	m_enable3DPositioning = false;
	m_pinToListener = false;
}
FMODSoundDescriptor::~FMODSoundDescriptor()
{
	m_system = NULL;
	m_sound = NULL;
	m_channel = NULL;
}
void FMODSoundDescriptor::Initialize(FMOD::System* fmod, FMOD::Channel* fmodChannel, FMOD::Sound* fmodSound, INT32 soundType)
{
	m_system = fmod;
	m_channel = fmodChannel;
	m_sound = fmodSound;
	m_soundType = soundType;
}
int FMODSoundDescriptor::IsPlaying()
{
	bool isPlaying = false;
	FMOD_RESULT fmodResult = m_channel->isPlaying(&isPlaying);
	return fmodResult != FMOD_ERR_INVALID_HANDLE && isPlaying != false;
}
void FMODSoundDescriptor::Update(REAL elapsedTime)
{
	if (m_fadeTime > 0.0f)
	{
		// Increment fade clock
		m_fadeClock += elapsedTime;
		if (m_fadeClock >= m_fadeTime)
		{
			// Force stop
			this->Stop();
		}
		else
		{
			// Calculate volume as a linear function of time
			REAL volume = (m_fadeTime - m_fadeClock) / m_fadeTime;
			// Set volume
			FMOD_RESULT fmodResult = m_channel->setVolume(volume);
			if (fmodResult != FMOD_OK)
			{
				FMODError("Update FMOD 1 error! code:%d error:%s\n", fmodResult, FMOD_ErrorString(fmodResult));
			}
		}
	}
}
int FMODSoundDescriptor::Stop()
{
	FMOD_RESULT fmodResult = m_channel->stop();
	if (fmodResult != FMOD_OK)
	{
		FMODError("Stop FMOD 1 error! code:%d error:%s\n", fmodResult, FMOD_ErrorString(fmodResult));
	}
	return fmodResult == FMOD_OK;
}
void FMODSoundDescriptor::Release()
{
	m_sound->release();
}
int FMODSoundDescriptor::FadeAndStop(REAL fadeTime)
{
	m_fadeClock = 0.0f;
	m_fadeTime = fadeTime;
	// Adjust fade clock based on current volume
	REAL curVolume = 1.0f;
	m_channel->getVolume(&curVolume);
	m_fadeClock = curVolume > ruEPSILON ? m_fadeTime * (1.0f - curVolume) : m_fadeTime;
	return TRUE;
}
int FMODSoundDescriptor::SetVolume(REAL volume)
{
	FMOD_RESULT fmodResult = m_channel->setVolume(volume);
	if (fmodResult != FMOD_OK)
	{
		FMODError("SetVolume FMOD 1 error! code:%d error:%s\n", fmodResult, FMOD_ErrorString(fmodResult));
	}
	return fmodResult == FMOD_OK;
}
REAL FMODSoundDescriptor::GetVolume()
{
	float volume = 1.0f;
	FMOD_RESULT fmodResult = m_channel->getVolume(&volume);
	if (fmodResult != FMOD_OK)
	{
		FMODError("GetVolume FMOD 1 error! code:%d error:%s\n", fmodResult, FMOD_ErrorString(fmodResult));
	}
	return fmodResult == FMOD_OK ? volume : 1.0f;
}
void CRuSymphony::FMODError(const char* format, ...)
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
REAL CRuSymphony::GetVolume(REAL volume)
{
	if (m_fmodSystem == NULL)
		return 0.0f;
	return m_masterVolume / 1.0f * volume;
}
void FMODSoundDescriptor::FMODError(const char* format, ...)
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
// ************************************************************************************************************************************************************
CRuSymphony* g_ruSymphony = NULL;
// ************************************************************************************************************************************************************
#pragma managed(pop)