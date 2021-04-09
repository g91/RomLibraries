/*!
	@file RuAudible.h

	Copyright (c) 2004-2012 Runewaker Entertainment Ltd. All rights reserved.
*/

#pragma once
#include "../Core/Math/RuMathVector.h"
#include "../Core/Thread/RuThread_CriticalSection.h"
#include "../Core/Type/RuType_Object.h"
#include "../Engine/Base/RuCore.h"
#include "../Engine/Manager/RuEngine_ResourceManager_Base.h"
#include "../Core/Collection/RuSortedList.h"
#include "../Core/Collection/RuStringHashNew.h"
#include "RuSoundFMOD.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	CRuAudible
	@version 2013.02.24

	Provides high-level sound management services at the game logic layer.

	This class performs multiple game audio functions including interpreting WES results from WorldGard to enable
	playback of environmental sounds, responding to entity trigger notifications, and more.

	Trigger notification targets registered with this class can have two additional properties set:
		[audible: src cls] - Specifies the source class tag to use when interpreting sound tags (e.g. "human_male_warrior")
		[audible: tgt cls] - Specifies the target class tag to use when interpreting sound tags (e.g. "metallic_target")
*/
class CRuAudible : public IRuObject
{
	ruRTTI_DECLARE
		ruHEAP_DECLARE
		ruCLASSGUID_DECLARE

public:
	enum AudibleType
	{
		//ruAUDIBLETYPE_AMBIENCE = 0,
		//ruAUDIBLETYPE_EFFECTS = 1,
		//ruAUDIBLETYPE_MUSIC = 2,
		//ruAUDIBLETYPE_SPEECH = 3
		ruSOUNDTYPE_SFX = 0, //!< Sound FX
		ruSOUNDTYPE_AMBIENCE = 1, //!< Ambience
		ruSOUNDTYPE_MUSIC = 2, //!< Music
		ruSOUNDTYPE_INTERFACE = 3 //!< Interface
	};

	enum ListenerMode
	{
		ruLISTENERMODE_STANDARD = 0,
		ruLISTENERMODE_ISOCAM = 1
	};

	struct ChannelDesc : public IRuReferenceCounted
	{
		AudibleType							m_audibleType;
		PTRVALUE							m_soundHandle;
		IRuSoundChannel* m_soundChannel;
		REAL								m_soundDefVol;
		PTRVALUE							m_tag;														//!< Custom tag

		ChannelDesc(AudibleType audibleType, PTRVALUE soundHandle, IRuSoundChannel* channel, REAL defVol, PTRVALUE tag)
			: m_audibleType(audibleType),
			m_soundHandle(soundHandle),
			m_soundChannel(channel),
			m_soundDefVol(defVol),
			m_tag(tag)
		{
		}
	};
protected:
	CRuHashGenerator m_hashGen;
	CRuMTRandom m_randGen;
	CRuSoundFMOD* m_audioEngine; //!< The audio engine

	CRuThread_CriticalSection* m_cs_Global;

	// ...
	ListenerMode m_listenerMode;

	REAL m_proximityThreshold;

	REAL m_trkInvokeOverride; //!< Track invocation override [0, 1] / -1 to disable override

	REAL m_volume_Master;
	REAL m_volume_SFX;
	REAL m_volume_AMBIENCE;
	REAL m_volume_MUSIC;
	REAL m_volume_INTERFACE;

	// ...
	CRuCamera* m_camera;

	UINT32 m_clock;
	UINT64 m_nextHandle;

	// Common audio data
	CRuArrayList<ChannelDesc*> m_fadingChannels;

	// Manual playback data
	CRuSortedList<UINT64, ChannelDesc*>	m_manualChannels;

	CRuStringHashNew<ChannelDesc*> m_tracks; //!< Audio tracks
	CRuStringHashNew<ChannelDesc*> m_trackOverrides; //!< Audio track overrides
public:
	CRuAudible();
	virtual ~CRuAudible();

	// IRuObject interface
	virtual void Dispose();

	// ...
	void Initialize(CRuSoundFMOD* audioEngine);
	void SetCamera(CRuCamera* camera);

	void SetListenerMode(ListenerMode mode);
	void SetProximityMuteThreshold(REAL distance);
	void SetTrackInvokeOverride(REAL overrideProbability);
	void SetVolumeLevels(REAL master, REAL sfx, REAL ambience, REAL music, REAL interf);

	// ...
	UINT64 Play(const char* soundTag, AudibleType audibleType, INT32 loop = FALSE, const CRuVector3* position = NULL);
	INT32 Stop(UINT64 handle, REAL fadeDuration);
	INT32 IsPlaying(UINT64 handle);
	INT32 UpdatePosition(UINT64 handle, const CRuVector3* position);

	INT32 Play(const char* soundTag, AudibleType audibleType, const char* track, INT32 loop);
	INT32 Stop(const char* track);

	IRuSoundChannel* GetSoundChannel(UINT64 handle);
	IRuSoundChannel* GetSoundChannel(const char* track);

	// Sound bank
	void ClearSoundBank();

	// ...
	void Update(REAL elapsedTime);

	void Reset();
	void FadeAndStop(ChannelDesc* channelDesc, REAL fadeDuration = 0.250f);

	float GetDefault3DMaxDist();
	const CRuVector3& GetListenerPosition();
protected:
	REAL GetVolumeByType(AudibleType audibleType);

	void FC_Clear();
	void FC_GarbageCollect();

	void MC_Clear();
	void MC_GarbageCollect();

	INT32 Host_OnTransformChanged(RuEventArgs* eventArgs);

	INT32 TRK_Clear();
	void TRK_CheckOverrides();
	INT32 TRK_CheckTrackExpiration(const char* key, void* data, void* userData);
	ChannelDesc* TRK_GetChannelDesc(const char* track);

	ChannelDesc* CHN_Play(IRuObject* host, const char* sndResPath, AudibleType type, REAL defVolume, UINT32 mode, INT32 loopCount);
	ChannelDesc* CHN_Start(IRuObject* host, PTRVALUE sndHandle, AudibleType type, REAL defVolume, UINT32 mode, INT32 loopCount);

	void UTIL_UnregisterHost(IRuSoundChannel* soundChannel);

	REAL IsoCamRolloffCallback(IRuSoundChannel* channel, REAL distance);
};
extern CRuAudible* g_ruAudible;
// ************************************************************************************************************************************************************

#pragma managed(pop)
