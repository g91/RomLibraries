/*!
	@project Rune
	@file RuHelper_EventManager.cpp

	Copyright (c) 2004-2006 All rights reserved

	@author John Tang
	@date 2006/10/30
*/

#include "../Helper/RuHelper_EventManager.h"

#include "../Scene/Controller/RuController_Entity.h"
#include "../Scene/Controller/RuController_GhostTrail.h"
#include "../Symphony/RuSymphony_Base.h"

//#include "../../../Interface/chat/ChatFrame.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

CRuGlobalEventManager_Sample::CRuGlobalEventManager_Sample()
{
	m_controllerFrame = ruNEW CRuEntity_Container_Impl();
	m_audioLibrary = NULL;
	m_camera = NULL;

	m_soundEvents = ruNEW CRuHashMap<PTRVALUE, SoundEventDescriptor *>(256);

	m_masterSoundLevel = 1.000f;
	m_ambientSoundLevel = 0.500f;
	m_musicSoundLevel = 0.400f;
	m_soundFXSoundLevel = 1.000f;
	m_uiSoundLevel = 1.000f;

	m_musicFrequency = 100.0f;

	m_enableSoundFX = TRUE;
	m_enableMusic = TRUE;

	m_timeGroup = 0;
	m_zoneSettings = NULL;
	m_musicTrigger = NULL;
	m_ambienceTrigger = NULL;
	m_supplementAmbienceTrigger = NULL;
	m_randomAmbienceTrigger = NULL;

	m_3DAmbienceTriggerHash = ruNEW CRuHashMap<CRuGUID, CRuWorld_Trigger *>(256);
	m_active3DAmbienceSounds = ruNEW CRuHashMap<CRuGUID, PTRVALUE>(128);

	m_musicSoundHandle = 0;
	m_ambientSoundHandle = 0;
	m_supplementAmbientSoundHandle = 0;

	m_rerollMusic = TRUE;
	m_activeMusicName = "";
	m_activeAmbienceName = "";
	m_activeSupplementAmbienceName = "";

	// Apply settings
	GetSettingsFromEngine();

	// Register playback finished listener on Symphony
	g_ruSymphony->Event_PlaybackFinished().RegisterHandler(m_GUID, boost::bind(&CRuGlobalEventManager_Sample::PlaybackFinishedHandler, this, _1));

	// Register listener on engine settings
	g_ruEngineSettings->RegisterSettingsChangeHandler(m_GUID, boost::bind(&CRuGlobalEventManager_Sample::EngineSettingsChangedHandler, this, _1, _2));
}

CRuGlobalEventManager_Sample::~CRuGlobalEventManager_Sample()
{
	ClearSoundEventQueue();
	delete m_soundEvents;

	ruSAFE_RELEASE(m_controllerFrame);
	ruSAFE_RELEASE(m_audioLibrary);
	ruSAFE_RELEASE(m_camera);

	ruSAFE_RELEASE(m_zoneSettings);
	ruSAFE_RELEASE(m_musicTrigger);
	ruSAFE_RELEASE(m_ambienceTrigger);
	ruSAFE_RELEASE(m_supplementAmbienceTrigger);
	ruSAFE_RELEASE(m_randomAmbienceTrigger);

	delete m_3DAmbienceTriggerHash;

	// TODO: also gotta stop all ambience sounds here
	delete m_active3DAmbienceSounds;
}

void CRuGlobalEventManager_Sample::SetAudioLibrary(CRuFusion_AudioLibrary *audioLibrary)
{
	if(audioLibrary)
	{
		ruSAFE_RELEASE(m_audioLibrary);
		m_audioLibrary = audioLibrary;
		m_audioLibrary->AddRef();
	}
}

void CRuGlobalEventManager_Sample::SetCamera(CRuCamera *camera)
{
	if(camera)
	{
		camera->AddRef();
		ruSAFE_RELEASE(m_camera);
		m_camera = camera;
	}
}

void CRuGlobalEventManager_Sample::Update(REAL elapsedTime)
{
	// Chain update call to the controller container
	m_controllerFrame->Update(elapsedTime);

	// Process ambient sound
	const char *ambientSound = "";

	if(m_ambienceTrigger)
	{
		const char *propertyNames[4] = { "CRuWorld_Trigger:Ambience Dawn", "CRuWorld_Trigger:Ambience Day", "CRuWorld_Trigger:Ambience Dusk", "CRuWorld_Trigger:Ambience Night"};
		const char *propertyName = propertyNames[m_timeGroup];

		RuPropertyType propertyValue;
		ambientSound  = m_ambienceTrigger->GetProperty(propertyName, propertyValue) ? boost::get<const char *>(propertyValue) : "";
	}
	else if(m_zoneSettings)
	{
		ambientSound = m_zoneSettings->GetAmbientSound();
	}

	// If the ambience sound is different from the one currently playing
	if(m_ambientSoundHandle == 0 || stricmp(m_activeAmbienceName, ambientSound) != 0)
	{
		// Stop currently playing ambient sound
		if(m_ambientSoundHandle)
		{
			g_ruSymphony->FadeOutSound(m_ambientSoundHandle, 2000.0f);
			m_ambientSoundHandle = NULL;
		}

		// Look for ambient sound in the audio library - if not found, try to use the search string as a path
		const char *audioResource = m_audioLibrary->GetAssociatedAudio("ambience", ambientSound);
		audioResource = audioResource ? audioResource : ambientSound;

		// Play the ambient sound
		m_ambientSoundHandle = g_ruSymphony->PlaySoundByPath(audioResource, TRUE, m_ambientSoundLevel, ruSOUNDTYPE_AMBIENCE);

		// Store current ambient sound
		m_activeAmbienceName = ambientSound;
	}

	// Process supplement ambient sound
	const char *supplementAmbientSound = "";

	if(m_supplementAmbienceTrigger)
	{
		const char *propertyNames[4] = { "CRuWorld_Trigger:Supplement Ambience Dawn", "CRuWorld_Trigger:Supplement Ambience Day", "CRuWorld_Trigger:Supplement Ambience Dusk", "CRuWorld_Trigger:Supplement Ambience Night"};
		const char *propertyName = propertyNames[m_timeGroup];

		RuPropertyType propertyValue;
		supplementAmbientSound  = m_supplementAmbienceTrigger->GetProperty(propertyName, propertyValue) ? boost::get<const char *>(propertyValue) : "";
	}
	else if(m_zoneSettings)
	{
		RuPropertyType propertyValue;
		supplementAmbientSound = m_zoneSettings->GetProperty("CRuWorld_ZoneSettings:Looping Ambience", propertyValue) ? boost::get<const char *>(propertyValue) : "";
	}

	// If the supplement ambience sound is different from the one currently playing
	if(m_supplementAmbientSoundHandle == 0 || stricmp(m_activeSupplementAmbienceName, supplementAmbientSound) != 0)
	{
		// Stop currently playing supplement ambient sound
		if(m_supplementAmbientSoundHandle)
		{
			g_ruSymphony->FadeOutSound(m_supplementAmbientSoundHandle, 2000.0f);
			m_supplementAmbientSoundHandle = NULL;
		}

		const char *audioResource = m_audioLibrary->GetAssociatedAudio("ambience", supplementAmbientSound);
		audioResource = audioResource ? audioResource : supplementAmbientSound;

		// Play the supplement ambient sound
		m_supplementAmbientSoundHandle = g_ruSymphony->PlaySoundByPath(audioResource, TRUE, m_ambientSoundLevel, ruSOUNDTYPE_AMBIENCE);

		// Store current supplement ambient sound
		m_activeSupplementAmbienceName = supplementAmbientSound;
	}

	// Process random ambient sounds
	m_timeToNextRandomAmbienceRoll -= elapsedTime;
	if(m_timeToNextRandomAmbienceRoll < 0.0f)
	{
		RuPropertyType propertyValue;

		const char *randomAmbience = "";
		REAL occurrenceRate = 0.0f;

		if(m_randomAmbienceTrigger)
		{
			const char *propertyNames[4] = { "CRuWorld_Trigger:Random Ambience Dawn", "CRuWorld_Trigger:Random Ambience Day", "CRuWorld_Trigger:Random Ambience Dusk", "CRuWorld_Trigger:Random Ambience Night"};
			const char *propertyName = propertyNames[m_timeGroup];

			// Get the occurrence rate
			REAL occurrenceRate  = m_randomAmbienceTrigger->GetProperty("CRuWorld_Trigger:Random Ambience Occurrence Rate", propertyValue) ? boost::get<REAL>(propertyValue) : 0.01f;

			// Get the associated sound
			randomAmbience  = m_randomAmbienceTrigger->GetProperty(propertyName, propertyValue) ? boost::get<const char *>(propertyValue) : "";
		}
		else if(m_zoneSettings)
		{
			// Get the occurrence rate
			REAL occurrenceRate  = m_zoneSettings->GetProperty("CRuWorld_ZoneSettings:Random Ambience Occurrence Rate", propertyValue) ? boost::get<REAL>(propertyValue) : 0.01f;

			// Get the associated sound
			randomAmbience  = m_zoneSettings->GetProperty("CRuWorld_ZoneSettings:Random Ambience", propertyValue) ? boost::get<const char *>(propertyValue) : "";
		}

		if(RuGlobalMTRand().GenerateFloat() < occurrenceRate)
		{
			// Look for the sound in the audio library
			const char *audioResource = m_audioLibrary->GetAssociatedAudio("random ambience", randomAmbience);

			if(audioResource)
			{
				g_ruSymphony->PlaySoundByPath(audioResource, FALSE, m_ambientSoundLevel, ruSOUNDTYPE_AMBIENCE);
			}
		}

		// Reset time to next roll
		m_timeToNextRandomAmbienceRoll = 1.0f;
	}

	// Process music sounds
	m_timeToNextMusicRoll -= elapsedTime;
	if(m_timeToNextMusicRoll < 0.0f)
	{
		const char *music = "";
		REAL musicOccurrenceRate = 0.0f;

		if(m_musicTrigger)
		{
			const char *musicPropertyNames[4] = { "CRuWorld_Trigger:Dawn Music", "CRuWorld_Trigger:Day Music", "CRuWorld_Trigger:Dusk Music", "CRuWorld_Trigger:Night Music"};
			const char *propertyName = musicPropertyNames[m_timeGroup];

			RuPropertyType propertyValue;
			music  = m_musicTrigger->GetProperty(propertyName, propertyValue) ? boost::get<const char *>(propertyValue) : "";

			musicOccurrenceRate  = m_musicTrigger->GetProperty("CRuWorld_Trigger:Music Occurrence Rate", propertyValue) ? boost::get<REAL>(propertyValue) : 0.01f;
		}
		else if(m_zoneSettings)
		{
			music = m_zoneSettings->GetThemeMusic();
			musicOccurrenceRate = m_zoneSettings->GetThemeMusicOccurrenceRate();
		}

		// Stop currently playing music if it is different from what is specified
		if(m_musicSoundHandle && strlen(music) > 0 && stricmp(music, m_activeMusicName) != 0)
		{
			g_ruSymphony->FadeOutSound(m_musicSoundHandle, 2000.0f);
			m_musicSoundHandle = NULL;
		}

		if(m_musicSoundHandle == 0)
		{
			if(m_rerollMusic || RuGlobalMTRand().GenerateFloat() < (musicOccurrenceRate * m_musicFrequency))
			{
				// Look for music in the audio library - if not found, try to use the search string as a path
				const char *audioResource = m_audioLibrary->GetAssociatedAudio("music", music);
				audioResource = audioResource ? audioResource : music;

				// Play the music
				m_musicSoundHandle = g_ruSymphony->PlaySoundByPath(audioResource, FALSE, m_musicSoundLevel, ruSOUNDTYPE_MUSIC);

				// Save the music name
				m_activeMusicName = music;

				// Clear reroll music
				m_rerollMusic = FALSE;
			}
		}
		else
		{
		}

		// Reset time to next roll
		m_timeToNextMusicRoll = 1.0f;
	}

	// Update 3D ambience triggers
	Update3DAmbienceTriggers(elapsedTime);

	// Symphony update cycle
	g_ruSymphony->Update(elapsedTime);
}

void CRuGlobalEventManager_Sample::RegisterEventHandlers(CRuEntity *entity, PTRVALUE userData)
{
	if(entity)
	{
		entity->Event_Dispose().RegisterHandler(CRuGUID(), boost::bind(&CRuGlobalEventManager_Sample::HandleEvent_Dispose, this, _1), userData);
		entity->Event_Trigger().RegisterHandler(CRuGUID(), boost::bind(&CRuGlobalEventManager_Sample::HandleEvent_Trigger, this, _1), userData);
	}
}

BOOL CRuGlobalEventManager_Sample::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	RuObjectEvent_Dispose_Args *disposeArgs = static_cast<RuObjectEvent_Dispose_Args *>(eventArgs);
	StopSpriteSounds(disposeArgs->m_object);

	return TRUE;
}

BOOL CRuGlobalEventManager_Sample::HandleEvent_Trigger(RuEventArgs *eventArgs)
{
	RuEntityEvent_Trigger_Args *triggerArgs = static_cast<RuEntityEvent_Trigger_Args *>(eventArgs);

	switch(triggerArgs->m_triggerID)
	{
		case ruENTITYEVENT_HIT:
			{
			}

			return TRUE;

		case ruENTITYEVENT_FOOTSTEP:
			{
				// Play footstep sound
				const CRuVector3 &spritePosition = triggerArgs->m_entity->GetTranslation(ruFRAMETYPE_WORLD);
				PTRVALUE soundHandle = g_ruSymphony->Play3DSound("sound\\common\\footstep\\footstep_generic.mp3", ruSOUNDTYPE_SFX, m_soundFXSoundLevel, FALSE, spritePosition, TRUE);
			}

			return TRUE;

		case ruENTITYEVENT_GHOSTTRAIL:
			{
				CRuController_GhostTrail *gtController = ruNEW CRuController_GhostTrail();
				gtController->SetFrameType(ruFRAMETYPE_WORLD);
//				gtController->SetInterval(0.001f);
				gtController->SetMaxAge(triggerArgs->m_duration);
				gtController->SetFadeCoefficients(0.0f, 0.0f, 1.0f);
				gtController->SetMaxSegments(64);
				gtController->SetTrailAnchors("effect01", "effect02");
				gtController->SetTrailTextureName("texture\\fx\\weapon_trail\\blade_flash");
				gtController->SetDistortFXEnable(TRUE);
				gtController->SetDistortTextureName("texture\\common\\caustic_2");
				gtController->SetDistortFrameCount(1);
				gtController->SetTarget(triggerArgs->m_entity);

				gtController->SetActiveDuration(triggerArgs->m_duration);
				gtController->SetFadeOutDuration(0.5f);

				gtController->SetStartTime(0.0f);
				gtController->SetDuration(triggerArgs->m_duration + 0.5f);
				gtController->SetLoopTime(0.0f);
				gtController->SetSelfDestructTimer(triggerArgs->m_duration + 2.0f);

				// Add controller to the container frame
				m_controllerFrame->AddEntity(gtController);
			}

			return TRUE;

		case ruENTITYEVENT_AUDIO_DIRECT:
			{
				BOOL loop = (BOOL)triggerArgs->m_parameter0;

				PTRVALUE soundHandle = g_ruSymphony->PlaySound(triggerArgs->m_triggerString, ruSOUNDTYPE_SFX, m_soundFXSoundLevel, loop);

				// If the sample is looping, store the sound handle somewhere
				if(loop)
				{
					if(triggerArgs->m_proxy && triggerArgs->m_proxy->GetType().IsSubClassOf(CRuEntity::Type()))
					{
						// Attach listener to the event proxy
						triggerArgs->m_proxy->Event_Dispose().RegisterHandler(CRuGUID(), boost::bind(&CRuGlobalEventManager_Sample::HandleEvent_Dispose, this, _1), NULL);

						// Map the event proxy to the sound handle
						PushSoundEvent(triggerArgs->m_proxy, soundHandle);
					}
				}
			}

			return TRUE;

		case ruENTITYEVENT_AUDIO_TAGGED:
			{
				// Get classification of the ACT entity
				CRuACTEntity *actEntity = RuEntity_FindParentACTEntity(triggerArgs->m_entity);
				const char *classification = actEntity ? actEntity->GetTemplate()->GetClassification() : "hm_female";

				BOOL loop = (BOOL)triggerArgs->m_parameter0;
				const char *audioResource = m_audioLibrary->GetAssociatedAudio(classification, triggerArgs->m_triggerString);
				if(audioResource)
				{
					g_ruSymphony->PlaySoundByPath(audioResource, loop, m_soundFXSoundLevel, ruSOUNDTYPE_SFX);
				}
			}

			return FALSE;

		case ruENTITYEVENT_AUDIO_3D_DIRECT:
			{
				BOOL loop = (BOOL)triggerArgs->m_parameter0;
				const char *audioResource = triggerArgs->m_triggerString;

				const CRuVector3 &spritePosition = triggerArgs->m_entity->GetTranslation(ruFRAMETYPE_WORLD);

				PTRVALUE soundHandle = g_ruSymphony->Play3DSound(audioResource, ruSOUNDTYPE_SFX, m_soundFXSoundLevel, loop != ruAUDIOLOOPMODE_NONE, spritePosition, FALSE);

				// If the sample is looping, store the sound handle somewhere
				if(loop)
				{
					if(triggerArgs->m_proxy && triggerArgs->m_proxy->GetType().IsSubClassOf(CRuEntity::Type()))
					{
						// Attach listener to the event proxy
						triggerArgs->m_proxy->Event_Dispose().RegisterHandler(CRuGUID(), boost::bind(&CRuGlobalEventManager_Sample::HandleEvent_Dispose, this, _1), NULL);

						// Map the event proxy to the sound handle
						PushSoundEvent(triggerArgs->m_proxy, soundHandle);
					}
				}
			}

			return TRUE;

		case ruENTITYEVENT_AUDIO_3D_TAGGED:
			{
				// Get classification of the ACT entity
				CRuACTEntity *actEntity = RuEntity_FindParentACTEntity(triggerArgs->m_entity);
				const char *classification = actEntity ? actEntity->GetTemplate()->GetClassification() : "hm_female";

				BOOL loop = (BOOL)triggerArgs->m_parameter0;
				const char *audioResource = m_audioLibrary->GetAssociatedAudio(classification, triggerArgs->m_triggerString);
				if(audioResource)
				{
					const CRuVector3 &position = triggerArgs->m_entity->GetTranslation(ruFRAMETYPE_WORLD);

					PTRVALUE soundHandle = g_ruSymphony->Play3DSound(audioResource, ruSOUNDTYPE_SFX, m_soundFXSoundLevel, loop != ruAUDIOLOOPMODE_NONE, position, FALSE);

					// If the sample is looping, store the sound handle somewhere
					if(loop)
					{
						if(triggerArgs->m_proxy && triggerArgs->m_proxy->GetType().IsSubClassOf(CRuEntity::Type()))
						{
							// Attach listener to the event proxy
							triggerArgs->m_proxy->Event_Dispose().RegisterHandler(CRuGUID(), boost::bind(&CRuGlobalEventManager_Sample::HandleEvent_Dispose, this, _1), NULL);

							// Map the event proxy to the sound handle
							PushSoundEvent(triggerArgs->m_proxy, soundHandle);
						}
					}
				}
			}

			return TRUE;

		case ruENTITYEVENT_CAMERA_JIGGLE:
			{
				if(m_camera)
				{
					m_camera->Jiggle(25.0f, 300.0f, 10.0f);
				}
			}

			return TRUE;
	}

	return TRUE;
}

void CRuGlobalEventManager_Sample::SetTimeGroup(INT32 timeGroup)
{
	m_timeGroup = timeGroup % 4;
}

void CRuGlobalEventManager_Sample::SetZoneSettings(CRuWorld_ZoneSettings *zoneSettings)
{
	CRuWorld_ZoneSettings *oldZoneSettings = m_zoneSettings;

	if(zoneSettings)
	{
		zoneSettings->AddRef();
	}

	m_zoneSettings = zoneSettings;

	ruSAFE_RELEASE(oldZoneSettings);
}

void CRuGlobalEventManager_Sample::SetMusicTrigger(CRuWorld_Trigger *musicTrigger)
{
	if(m_musicTrigger != musicTrigger)
	{
		CRuWorld_Trigger *oldTrigger = m_musicTrigger;

		if(musicTrigger)
		{
			m_musicTrigger = musicTrigger;
			m_musicTrigger->AddRef();

			RuPropertyType propertyValue;
			m_rerollMusic  = m_musicTrigger->GetProperty("CRuWorld_Trigger:Always Trigger Music", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;
		}
		else
		{
			m_musicTrigger = NULL;
		}

		ruSAFE_RELEASE(oldTrigger);
	}
}

void CRuGlobalEventManager_Sample::SetAmbienceTrigger(CRuWorld_Trigger *ambienceTrigger)
{
	if(m_ambienceTrigger != ambienceTrigger)
	{
		CRuWorld_Trigger *oldTrigger = m_ambienceTrigger;

		if(ambienceTrigger)
		{
			m_ambienceTrigger = ambienceTrigger;
			m_ambienceTrigger->AddRef();
		}
		else
		{
			m_ambienceTrigger = NULL;
		}

		ruSAFE_RELEASE(oldTrigger);
	}
}

void CRuGlobalEventManager_Sample::SetSupplementAmbienceTrigger(CRuWorld_Trigger *supplementAmbienceTrigger)
{
	if(m_supplementAmbienceTrigger != supplementAmbienceTrigger)
	{
		CRuWorld_Trigger *oldTrigger = m_supplementAmbienceTrigger;

		if(supplementAmbienceTrigger)
		{
			m_supplementAmbienceTrigger = supplementAmbienceTrigger;
			m_supplementAmbienceTrigger->AddRef();
		}
		else
		{
			m_supplementAmbienceTrigger = NULL;
		}

		ruSAFE_RELEASE(oldTrigger);
	}
}

void CRuGlobalEventManager_Sample::SetRandomAmbienceTrigger(CRuWorld_Trigger *randomAmbienceTrigger)
{
	if(m_randomAmbienceTrigger != randomAmbienceTrigger)
	{
		CRuWorld_Trigger *oldTrigger = m_randomAmbienceTrigger;

		if(randomAmbienceTrigger)
		{
			m_randomAmbienceTrigger = randomAmbienceTrigger;
			m_randomAmbienceTrigger->AddRef();
		}
		else
		{
			m_randomAmbienceTrigger = NULL;
		}

		ruSAFE_RELEASE(oldTrigger);
	}
}

void CRuGlobalEventManager_Sample::AddRef3DAmbienceTriggers()
{
	// Increment local reference on all triggers
	for(INT32 i = 0; i < m_3DAmbienceTriggers.Count(); ++i)
	{
		m_3DAmbienceTriggers[i]->AddRef();
	}
}

void CRuGlobalEventManager_Sample::Clear3DAmbienceTriggers()
{
	// Release local reference to all triggers
	for(INT32 i = 0; i < m_3DAmbienceTriggers.Count(); ++i)
	{
		ruSAFE_RELEASE(m_3DAmbienceTriggers[i]);
	}

	// Clear triggers array
	m_3DAmbienceTriggers.Clear();
}

CRuArrayList<CRuWorld_Trigger *> &CRuGlobalEventManager_Sample::Get3DAmbienceTriggers()
{
	return m_3DAmbienceTriggers;
}

void CRuGlobalEventManager_Sample::Update3DAmbienceTriggers(REAL elapsedTime)
{
	CRuVector3 listenerPos = g_ruSymphony->GetListenerPosition();

	// Pre-process the trigger list for any grouped triggers
	for(INT32 i = 0; i < m_3DAmbienceTriggers.Count(); ++i)
	{
		CRuWorld_Trigger *trigger = m_3DAmbienceTriggers[i];

		RuPropertyType propertyValue;
		const char *sound  = trigger->GetProperty("CRuWorld_Trigger:3D Ambience Sound", propertyValue) ? boost::get<const char *>(propertyValue) : "";
		BOOL groupedSound = trigger->GetProperty("CRuWorld_Trigger:3D Ambience Grouped", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;

		if(groupedSound)
		{
			CRuOBB triggerBounds = trigger->GetTriggerBounds();
			REAL triggerDistance = triggerBounds.GetDistance(listenerPos);

			if(m_3DAmbienceGroupTriggers.ContainsKey(sound) == FALSE)
			{
				// Duplicate the trigger
				CRuWorld_Trigger *masterTrigger = ruNEW CRuWorld_Trigger();
				masterTrigger->SetTriggerBounds(triggerBounds);
				masterTrigger->SetTriggerType(trigger->GetTriggerType());
				masterTrigger->SetZoneID(trigger->GetZoneID());
				masterTrigger->SetZoneType(trigger->GetZoneType());
				masterTrigger->SetProperty("CRuWorld_Trigger:3D Ambience Sound", sound);
				masterTrigger->SetProperty("CRuWorld_Trigger:3D Ambience Grouped", groupedSound);

				// Insert into the group trigger tree
				m_3DAmbienceGroupTriggers.Add(sound, masterTrigger);
			}
			else
			{
				// If this trigger is closer than the current master trigger, use this trigger's bounds
				CRuWorld_Trigger *masterTrigger = m_3DAmbienceGroupTriggers.Get(sound);
				CRuOBB masterTriggerBounds = masterTrigger->GetTriggerBounds();
				REAL masterTriggerDistance = masterTriggerBounds.GetDistance(listenerPos);

				if(triggerDistance < masterTriggerDistance)
				{
					masterTrigger->SetTriggerBounds(triggerBounds);
				}

			}

			// Remove trigger from the array
			m_3DAmbienceTriggers.RemoveAt(i);
			--i;
		}
	}

	// Insert 3D ambience group triggers into list
	Insert3DAmbienceGroupTriggers();

	// Update the trigger hash
	m_3DAmbienceTriggerHash->Clear();
	for(INT32 i = 0; i < m_3DAmbienceTriggers.Count(); ++i)
	{
		CRuWorld_Trigger *trigger = m_3DAmbienceTriggers[i];
		m_3DAmbienceTriggerHash->Add(trigger->GetGUID(), trigger->GetGUID().HashCode(), trigger);
	}

	// Start all sounds which should be active
	StartActive3DAmbienceTriggers();

	// Update all sound positions
	UpdateActive3DAmbienceTriggers();

	// Stop all sounds which no longer should exist
	StopInactive3DAmbienceTriggers();
}

void CRuGlobalEventManager_Sample::StartActive3DAmbienceTriggers()
{
	for(INT32 i = 0; i < m_3DAmbienceTriggers.Count(); ++i)
	{
		CRuWorld_Trigger *trigger = m_3DAmbienceTriggers[i];

		// If the trigger is already active, skip it
		if(m_active3DAmbienceSounds->ContainsKey(trigger->GetGUID(), trigger->GetGUID().HashCode()))
		{
			continue;
		}

		// Calculate distance to trigger
		CRuOBB triggerBounds = trigger->GetTriggerBounds();
		REAL distance = triggerBounds.GetDistance(g_ruSymphony->GetListenerPosition());

		if(distance > g_ruSymphony->GetDefault3DMaxDist())
		{
			continue;
		}

		// Activate trigger
		RuPropertyType propertyValue;
		const char *sound  = trigger->GetProperty("CRuWorld_Trigger:3D Ambience Sound", propertyValue) ? boost::get<const char *>(propertyValue) : "";
		BOOL groupedSound = trigger->GetProperty("CRuWorld_Trigger:3D Ambience Grouped", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;

		// Look for sound in the audio library - if not found, try to use the search string as a path
		const char *audioResource = m_audioLibrary->GetAssociatedAudio("ambience", sound);
		audioResource = audioResource ? audioResource : sound;

		// Play the sound
		CRuVector3 nearestPt = triggerBounds.GetClosestPoint(g_ruSymphony->GetListenerPosition());
		PTRVALUE soundHandle = g_ruSymphony->Play3DSound(audioResource, ruSOUNDTYPE_SFX, m_soundFXSoundLevel, TRUE, nearestPt, FALSE);

		// Store sound handle into active ambient sound list
		m_active3DAmbienceSounds->Add(trigger->GetGUID(), trigger->GetGUID().HashCode(), soundHandle);
	}
}

void CRuGlobalEventManager_Sample::UpdateActive3DAmbienceTriggers()
{
	for(INT32 i = 0; i < m_3DAmbienceTriggers.Count(); ++i)
	{
		CRuWorld_Trigger *trigger = m_3DAmbienceTriggers[i];

		// If the trigger is active, update its sound position
		if(m_active3DAmbienceSounds->ContainsKey(trigger->GetGUID(), trigger->GetGUID().HashCode()))
		{
			CRuOBB triggerBounds = trigger->GetTriggerBounds();
			CRuVector3 nearestPt = triggerBounds.GetClosestPoint(g_ruSymphony->GetListenerPosition());

			RuPropertyType propertyValue;
			const char *sound  = trigger->GetProperty("CRuWorld_Trigger:3D Ambience Sound", propertyValue) ? boost::get<const char *>(propertyValue) : "";
			BOOL groupedSound = trigger->GetProperty("CRuWorld_Trigger:3D Ambience Grouped", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;

			PTRVALUE soundHandle = m_active3DAmbienceSounds->Get(trigger->GetGUID(), trigger->GetGUID().HashCode());
			g_ruSymphony->Set3DSoundPosition(soundHandle, nearestPt);
		}
	}
}

void CRuGlobalEventManager_Sample::Insert3DAmbienceGroupTriggers()
{
	m_3DAmbienceGroupTriggers.ForAllElements(boost::bind(&CRuGlobalEventManager_Sample::Insert3DAmbienceGroupTriggersCallback, this, _1, _2));
}

BOOL CRuGlobalEventManager_Sample::Insert3DAmbienceGroupTriggersCallback(const char *key, void *data)
{
	CRuWorld_Trigger *trigger = *reinterpret_cast<CRuWorld_Trigger **>(data);

	trigger->AddRef();
	m_3DAmbienceTriggers.Add(trigger);

	return TRUE;
}

void CRuGlobalEventManager_Sample::StopInactive3DAmbienceTriggers()
{
	// Clear deleted trigger list
	m_deleted3DAmbienceTriggers.Clear();

	// Iterate through all triggers
	m_active3DAmbienceSounds->ForAllElements(boost::bind(&CRuGlobalEventManager_Sample::StopInactive3DAmbienceTriggersCallback, this, _1, _2));

	// Remove all deleted triggers
	for(INT32 i = 0; i < m_deleted3DAmbienceTriggers.Count(); ++i)
	{
		m_active3DAmbienceSounds->Remove(m_deleted3DAmbienceTriggers[i], m_deleted3DAmbienceTriggers[i].HashCode());
	}
}

BOOL CRuGlobalEventManager_Sample::StopInactive3DAmbienceTriggersCallback(const void *key, void *data)
{
	// Cast key & data into respective values
	const CRuGUID &guid = *reinterpret_cast<const CRuGUID *>(key);
	PTRVALUE soundHandle = *reinterpret_cast<PTRVALUE *>(data);

	BOOL soundInactive = FALSE;
	const char *sound = "";

	// Is the trigger GUID still in the hash?
	soundInactive = (m_3DAmbienceTriggerHash->ContainsKey(guid, guid.HashCode()) == FALSE);

	// Is the trigger farther than the maximum distance?
	if(!soundInactive)
	{
		CRuWorld_Trigger *trigger = m_3DAmbienceTriggerHash->Get(guid, guid.HashCode());
		CRuOBB triggerBounds = trigger->GetTriggerBounds();
		REAL distance = triggerBounds.GetDistance(g_ruSymphony->GetListenerPosition());

		RuPropertyType propertyValue;
		sound  = trigger->GetProperty("CRuWorld_Trigger:3D Ambience Sound", propertyValue) ? boost::get<const char *>(propertyValue) : "";

		if(distance > g_ruSymphony->GetDefault3DMaxDist())
		{
			soundInactive = TRUE;
		}
	}

	if(soundInactive)
	{
		// Fade out sound and add to list of stopping sound handles
		g_ruSymphony->FadeOutSound(soundHandle, 250.0f);

		// Add to list of triggers to be removed
		m_deleted3DAmbienceTriggers.Add(guid);

		// If trigger is a group trigger, remove it from the group trigger tree
		if(m_3DAmbienceGroupTriggers.ContainsKey(sound))
		{
			CRuWorld_Trigger *trigger = m_3DAmbienceGroupTriggers.Get(sound);
			m_3DAmbienceGroupTriggers.Remove(sound);
			ruSAFE_RELEASE(trigger);
		}
	}

	return TRUE;
}

void CRuGlobalEventManager_Sample::StopSpriteSounds(void *sprite)
{
	PTRVALUE spriteKey = reinterpret_cast<PTRVALUE>(sprite);
	DWORD spriteKeyHash = RuGlobalHashGen().GetHashCode(spriteKey);
	SoundEventDescriptor *soundEventDesc = NULL;

	if(m_soundEvents->ContainsKey(spriteKey, spriteKeyHash))
	{
		soundEventDesc = m_soundEvents->Get(spriteKey, spriteKeyHash);

		// Stop all active sound handles
		for(INT32 i = 0; i < soundEventDesc->m_soundHandles.Count(); ++i)
		{
			// Start volume ramp to zero
			g_ruSymphony->FadeOutSound(soundEventDesc->m_soundHandles[i]);
		}

		// Remove sprite's sound event descriptor from the hash map and destroy it
		m_soundEvents->Remove(spriteKey, spriteKeyHash);
		delete soundEventDesc;
	}
}

void CRuGlobalEventManager_Sample::PushSoundEvent(void *sprite, PTRVALUE soundHandle)
{
	PTRVALUE spriteKey = reinterpret_cast<PTRVALUE>(sprite);
	DWORD spriteKeyHash = RuGlobalHashGen().GetHashCode(spriteKey);
	SoundEventDescriptor *soundEventDesc = NULL;

	if(m_soundEvents->ContainsKey(spriteKey, spriteKeyHash))
	{
		soundEventDesc = m_soundEvents->Get(spriteKey, spriteKeyHash);
	}
	else
	{
		soundEventDesc = ruNEW SoundEventDescriptor();
		m_soundEvents->Add(spriteKey, spriteKeyHash, soundEventDesc);
	}

	// Append new sound handle to the sprite's list of active sound handles
	soundEventDesc->m_soundHandles.Add(soundHandle);
}

void CRuGlobalEventManager_Sample::ClearSoundEventQueue()
{
	// Iterate through each sound event and destroy
	m_soundEvents->ForAllElements(boost::bind(&CRuGlobalEventManager_Sample::ClearSoundEventQueueCallback, this, _1, _2));

	// Clear all triggers
	m_soundEvents->Clear();
}

BOOL CRuGlobalEventManager_Sample::ClearSoundEventQueueCallback(const void *key, void *data)
{
	SoundEventDescriptor *soundEvent = *((SoundEventDescriptor **) data);

	delete soundEvent;

	return TRUE;
}

BOOL CRuGlobalEventManager_Sample::PlaybackFinishedHandler(RuEventArgs *eventArgs)
{
	RuSymphonyEvent_PlaybackFinished_Args *finArgs = static_cast<RuSymphonyEvent_PlaybackFinished_Args *>(eventArgs);

	// Is the finished handle the music handle?
	if(finArgs->m_soundHandle == m_musicSoundHandle)
	{
		m_musicSoundHandle = 0;
	}

	return TRUE;
}

BOOL CRuGlobalEventManager_Sample::EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting)
{
	GetSettingsFromEngine();
	return TRUE;
}

void CRuGlobalEventManager_Sample::GetSettingsFromEngine()
{
	RuPropertyType propertyValue;

	if(g_ruEngineSettings->GetSetting("audioFX:sound fx enable", propertyValue))
	{
		m_enableSoundFX = boost::get<INT32>(propertyValue);
	}
	else
	{
		m_enableSoundFX = TRUE;
	}

	if(g_ruEngineSettings->GetSetting("audioFX:music enable", propertyValue))
	{
		m_enableMusic = boost::get<INT32>(propertyValue);
	}
	else
	{
		m_enableMusic = TRUE;
	}

	if(g_ruEngineSettings->GetSetting("audioFX:master volume", propertyValue))
	{
		m_masterSoundLevel = boost::get<REAL>(propertyValue);

		g_ruSymphony->SetMasterSoundVolume(m_masterSoundLevel);
	}

	if(g_ruEngineSettings->GetSetting("audioFX:ambience volume", propertyValue))
	{
		m_ambientSoundLevel = boost::get<REAL>(propertyValue);

		g_ruSymphony->AdjustSoundVolumeByType(ruSOUNDTYPE_AMBIENCE, m_ambientSoundLevel);
	}

	if(g_ruEngineSettings->GetSetting("audioFX:music volume", propertyValue))
	{
		m_musicSoundLevel = boost::get<REAL>(propertyValue);

		g_ruSymphony->AdjustSoundVolumeByType(ruSOUNDTYPE_MUSIC, m_musicSoundLevel);
	}

	if(g_ruEngineSettings->GetSetting("audioFX:sound fx volume", propertyValue))
	{
		m_soundFXSoundLevel = boost::get<REAL>(propertyValue);

		g_ruSymphony->AdjustSoundVolumeByType(ruSOUNDTYPE_SFX, m_soundFXSoundLevel);
	}

	if(g_ruEngineSettings->GetSetting("audioFX:interface sfx volume", propertyValue))
	{
		m_uiSoundLevel = boost::get<REAL>(propertyValue);

		g_ruSymphony->AdjustSoundVolumeByType(ruSOUNDTYPE_INTERFACE, m_uiSoundLevel);
	}

	if(g_ruEngineSettings->GetSetting("audioFX:music frequency", propertyValue))
	{
		m_musicFrequency = boost::get<REAL>(propertyValue);
	}

	if(g_ruEngineSettings->GetSetting("eventmgr:time group", propertyValue))
	{
		m_timeGroup = boost::get<INT32>(propertyValue);
	}
}

// ************************************************************************************************************************************************************

#pragma managed
