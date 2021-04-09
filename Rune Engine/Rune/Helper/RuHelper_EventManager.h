/*!
	@project Rune
	@file RuHelper_EventManager.h

	Copyright (c) 2004-2006 All rights reserved

	@author John Tang
	@date 2006/10/30
*/

#ifndef _RUHELPER_EVENTMANAGER_H_
#define _RUHELPER_EVENTMANAGER_H_

#include "../Scene/Base/RuEntityBase.h"
#include "../Rune.h"
#include "../Rune Engine Audio.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuGlobalEventManager_Sample
{
protected:
	struct SoundEventDescriptor
	{
		CRuArrayList<PTRVALUE>						m_soundHandles;					//!< List of active sound handles for the sprite
	};

	CRuGUID											m_GUID;							//!< GUID used for internal event registration

	CRuEntity_Container_Impl*						m_controllerFrame;				//!< Frame that contains all controllers
	CRuFusion_AudioLibrary*							m_audioLibrary;
	CRuCamera*										m_camera;

	CRuHashMap<PTRVALUE, SoundEventDescriptor *>*	m_soundEvents;					//!< Hash of active sound events

	// Sound volume levels
	REAL											m_masterSoundLevel;
	REAL											m_ambientSoundLevel;
	REAL											m_musicSoundLevel;
	REAL											m_soundFXSoundLevel;
	REAL											m_uiSoundLevel;

	REAL											m_musicFrequency;

	// Settings
	BOOL											m_enableSoundFX;
	BOOL											m_enableMusic;

	// Zone & triggers
	INT32											m_timeGroup;
	CRuWorld_ZoneSettings*							m_zoneSettings;
	CRuWorld_Trigger*								m_musicTrigger;
	CRuWorld_Trigger*								m_ambienceTrigger;
	CRuWorld_Trigger*								m_supplementAmbienceTrigger;
	CRuWorld_Trigger*								m_randomAmbienceTrigger;

	CRuArrayList<CRuWorld_Trigger *>				m_3DAmbienceTriggers;
	CRuHashMap<CRuGUID, CRuWorld_Trigger *>*		m_3DAmbienceTriggerHash;

	CRuTernaryStringTree<CRuWorld_Trigger *>		m_3DAmbienceGroupTriggers;

	CRuHashMap<CRuGUID, PTRVALUE>*					m_active3DAmbienceSounds;

	CRuArrayList<CRuGUID>							m_deleted3DAmbienceTriggers;

	// Persistent sound handles
	PTRVALUE										m_musicSoundHandle;
	PTRVALUE										m_ambientSoundHandle;
	PTRVALUE										m_supplementAmbientSoundHandle;

	BOOL											m_rerollMusic;
	CRuString										m_activeMusicName;
	CRuString										m_activeAmbienceName;
	CRuString										m_activeSupplementAmbienceName;

	REAL											m_timeToNextRandomAmbienceRoll;
	REAL											m_timeToNextMusicRoll;

public:
													CRuGlobalEventManager_Sample();
	virtual											~CRuGlobalEventManager_Sample();

	void											SetAudioLibrary(CRuFusion_AudioLibrary *audioLibrary);
	void											SetCamera(CRuCamera *camera);

	void											Update(REAL elapsedTime);

	void											RegisterEventHandlers(CRuEntity *entity, PTRVALUE userData);

	// Event handlers
	BOOL											HandleEvent_Dispose(RuEventArgs *eventArgs);
	BOOL											HandleEvent_Trigger(RuEventArgs *eventArgs);

	void											StopSpriteSounds(void *sprite);

	// Zone and trigger handling
	void											SetTimeGroup(INT32 timeGroup);
	void											SetZoneSettings(CRuWorld_ZoneSettings *zoneSettings);
	void											SetMusicTrigger(CRuWorld_Trigger *musicTrigger);
	void											SetAmbienceTrigger(CRuWorld_Trigger *ambienceTrigger);
	void											SetSupplementAmbienceTrigger(CRuWorld_Trigger *supplementAmbienceTrigger);
	void											SetRandomAmbienceTrigger(CRuWorld_Trigger *randomAmbienceTrigger);

	// 3D ambience trigger handling
	void											AddRef3DAmbienceTriggers();
	void											Clear3DAmbienceTriggers();
	CRuArrayList<CRuWorld_Trigger *>&				Get3DAmbienceTriggers();

	void											Update3DAmbienceTriggers(REAL elapsedTime);

	void											StartActive3DAmbienceTriggers();
	void											UpdateActive3DAmbienceTriggers();

	void											Insert3DAmbienceGroupTriggers();
	BOOL											Insert3DAmbienceGroupTriggersCallback(const char *key, void *data);

	void											StopInactive3DAmbienceTriggers();
	BOOL											StopInactive3DAmbienceTriggersCallback(const void *key, void *data);

protected:
	void											PushSoundEvent(void *sprite, PTRVALUE soundHandle);

	void											ClearSoundEventQueue();
	BOOL											ClearSoundEventQueueCallback(const void *key, void *data);

	// Event handlers
	BOOL											PlaybackFinishedHandler(RuEventArgs *eventArgs);
	BOOL											EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting);
	void											GetSettingsFromEngine();
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
