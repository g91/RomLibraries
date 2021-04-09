/*!
	@file RuSoundFMOD.h

	Copyright (c) 2004-2011 Runewaker Entertainment Ltd. All rights reserved.
*/

#pragma once
#include "RuSymphony_Base.h"
#include "../Core/Type/RuType_Object.h"
#include "../Core/Math/RuMathVector.h"
#include "../Core/Thread/RuThread_CriticalSection.h"
#include "RuSoundChannel.h"
#include "RuSoundReverb.h"
#include "../Core/Collection/RuStringHashNew.h"
#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*
	The following are forward declarations for FMOD-related classes. This is done to avoid having to include the fmod.hpp header
	from this header file, which would incur additional dependencies.
*/
namespace FMOD
{
	class Channel;
	class System;
}

// ************************************************************************************************************************************************************

/*!
	CRuSoundFMOD
	@version 2011.06.22

	FMOD-based implementation of IRuAudioEngine.

	The object's properties array contains the following additional properties:

	Property Name                                                Semantics                      Description
	-------------------------------------------------------------------------------------------------------------------------------------
*/
class CRuSoundFMOD : public IRuObject
{
	ruRTTI_DECLARE
		ruHEAP_DECLARE
		ruCLASSGUID_DECLARE

public:
	typedef boost::function<REAL(IRuSoundChannel * channel, REAL distance)> Custom3DRolloffCallback;

protected:
	enum SoundState
	{
		ruSOUNDSTATE_INVALID = 0,
		ruSOUNDSTATE_PENDING_CREATE = 1,
		ruSOUNDSTATE_VALID = 2
	};

	struct SoundDesc
	{
		SoundState							m_state;
		UINT32								m_soundFlags;
		PTRVALUE							m_soundHandle;
		IRuStream* m_soundStream;												//!< Stream containing sound data
	};

	CRuThread_CriticalSection* m_cs_Global;

	INT32									m_boInitialized;
	FMOD::System* m_fmod;

	UINT32									m_lastGCTime;
	DWORD									m_lastUpdateTime;

	CRuStringHashNew<SoundDesc> m_soundCache[2];											//!< Sound cache, 0 = 2D, 1 = 3D
	CRuQueue<char*>						m_pendingSoundStreams[2];

	CRuArrayList<IRuSoundChannel*>			m_activeChannels;

	PTRVALUE								m_sysSFX_WhiteNoise[2];

	struct
	{
		INT32								m_suspended : 1;									//!< Mixer suspended?
	};

	CRuEvent m_event_PlaybackFinished; //!< Sample playback finished
public:
	static Custom3DRolloffCallback			s_rolloffCallback;

public:
	CRuSoundFMOD();
	virtual									~CRuSoundFMOD();

	// IRuObject interface
	virtual void							Dispose();

	INT32									Initialize();

	INT32									IsInitialized();
	FMOD::System* GetFMOD();

	// IRuAudioEngine interface
	virtual BOOL			CreateSound(const char* resourceName, UINT32 flags, DWORD soundFlags, PTRVALUE& soundHandleOut);
	virtual void							DiscardSound(const char* resourceName);
	virtual IRuSoundChannel* PlaySound(PTRVALUE soundHandle, INT32 startPaused);

	virtual IRuSoundReverb* CreateReverb();
	virtual RuSoundReverbProperties			GetReverbPreset(const char* presetName);

	virtual INT32							Set3DListenerAttributes(INT32 listenerIdx, const CRuVector3& position, const CRuVector3& velocity, const CRuVector3& forward, const CRuVector3& up);
	virtual INT32							Set3DRolloffCallback(Custom3DRolloffCallback callback);
	virtual INT32							Set3DSettings(REAL dopplerScale, REAL distanceFactor, REAL rolloffScale);

	virtual INT32							SetAmbientReverb(const RuSoundReverbProperties& reverbProps);

	virtual void							Resume();
	virtual void							Suspend();

	virtual INT32							Update(PTRVALUE userData);

	CRuEvent& Event_PlaybackFinished();
protected:
	void									TerminateActiveChannels();
	void FMODError(const char* format, ...);
};
extern CRuSoundFMOD* g_ruSoundFMOD;
// ************************************************************************************************************************************************************

#pragma managed(pop)
