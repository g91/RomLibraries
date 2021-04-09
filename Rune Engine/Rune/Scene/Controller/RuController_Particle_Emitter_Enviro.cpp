#include "../../Scene/Controller/RuController_Particle_Emitter_Enviro.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_Particle_Emitter_Enviro, IRuEntity_KeyableController)
ruHEAP_DEFINE_SUBCLASS(CRuController_Particle_Emitter_Enviro, "CRuController_Particle_Emitter_Enviro", "IRuEntity_KeyableController")
ruCLASSGUID_DEFINE(CRuController_Particle_Emitter_Enviro, 0xA4B6682D, 0x0FA946eb, 0x9DA7628F, 0x41E08217)

// ************************************************************************************************************************************************************

CRuController_Particle_Emitter_Enviro::CRuController_Particle_Emitter_Enviro()
:	m_emitterPrerunTime(0.0f),
	m_particleOBKillTime(0.3f),
	m_particleEmitter(NULL)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Particle_Emitter_Enviro::HandleEvent_Dispose, this, _1), NULL);

	// Override default controller clock update
	this->m_overrideClockUpdate = TRUE;

	m_track_MaxParticles.SetTrackCategory("Emitter");
	m_track_MaxParticles.SetTrackName("Maximum Particles");
	m_track_MaxParticles.AppendKey(200);

	m_track_EmitterPosition.SetTrackCategory("Emitter");
	m_track_EmitterPosition.SetTrackName("Position");
	m_track_EmitterPosition.AppendKey(CRuVector3(0.0f, 0.0f, 0.0f));

	m_track_EmitterExtents.SetTrackCategory("Emitter");
	m_track_EmitterExtents.SetTrackName("Extents");
	m_track_EmitterExtents.AppendKey(CRuVector3(100.0f, 5.0f, 100.0f));

	m_track_EmissionRate.SetTrackCategory("Emitter");
	m_track_EmissionRate.SetTrackName("Emission Rate");
	m_track_EmissionRate.AppendKey(5.0f);

	m_track_EmitDirection.SetTrackCategory("Emitter");
	m_track_EmitDirection.SetTrackName("Emission Direction");
	m_track_EmitDirection.AppendKey(CRuVector3(0.0f, 1.0f, 0.0f));

	m_track_EmitDeviation.SetTrackCategory("Emitter");
	m_track_EmitDeviation.SetTrackName("Emission Deviation");
	m_track_EmitDeviation.AppendKey(0.2f);

	m_track_ParticleActiveExtents.SetTrackCategory("Particle");
	m_track_ParticleActiveExtents.SetTrackName("Active Extents");
	m_track_ParticleActiveExtents.AppendKey(CRuVector3(100.0f, 400.0f, 100.0f));

	m_track_ParticleMinExtinction.SetTrackCategory("Particle");
	m_track_ParticleMinExtinction.SetTrackName("Minimum Extinction");
	m_track_ParticleMinExtinction.AppendKey(3.0f);

	m_track_ParticleMaxExtinction.SetTrackCategory("Particle");
	m_track_ParticleMaxExtinction.SetTrackName("Maximum Extinction");
	m_track_ParticleMaxExtinction.AppendKey(3.0f);

	m_track_ParticleMinSpeed.SetTrackCategory("Particle");
	m_track_ParticleMinSpeed.SetTrackName("Minimum Speed");
	m_track_ParticleMinSpeed.AppendKey(3.0f);

	m_track_ParticleMaxSpeed.SetTrackCategory("Particle");
	m_track_ParticleMaxSpeed.SetTrackName("Maximum Speed");
	m_track_ParticleMaxSpeed.AppendKey(3.0f);

	m_track_ParticleMinScale.SetTrackCategory("Particle");
	m_track_ParticleMinScale.SetTrackName("Minimum Scale");
	m_track_ParticleMinScale.AppendKey(1.0f);

	m_track_ParticleMaxScale.SetTrackCategory("Particle");
	m_track_ParticleMaxScale.SetTrackName("Maximum Scale");
	m_track_ParticleMaxScale.AppendKey(1.0f);

	m_track_ParticleMinOrientation.SetTrackCategory("Particle");
	m_track_ParticleMinOrientation.SetTrackName("Minimum Orientation");
	m_track_ParticleMinOrientation.AppendKey(CRuVector3(0.0f, 0.0f, 0.0f));

	m_track_ParticleMaxOrientation.SetTrackCategory("Particle");
	m_track_ParticleMaxOrientation.SetTrackName("Maximum Orientation");
	m_track_ParticleMaxOrientation.AppendKey(CRuVector3(0.0f, 0.0f, 0.0f));

	m_track_Force0.SetTrackName("Force 0");
	m_track_Force0.AppendKey(CRuVector3(0.0f, 0.0f, 0.0f));

	m_track_Force1.SetTrackName("Force 1");
	m_track_Force1.AppendKey(CRuVector3(0.0f, 0.0f, 0.0f));

	m_track_GustFrequency.SetTrackName("Gust Frequency");
	m_track_GustFrequency.AppendKey(0.50f);

	m_track_GustAffectProbability.SetTrackName("Gust Affect Probability");
	m_track_GustAffectProbability.AppendKey(0.10f);

	m_track_GustTTL.SetTrackName("Gust TTL");
	m_track_GustTTL.AppendKey(0.50f);

	m_track_GustMinStrength.SetTrackName("Gust Min Strength");
	m_track_GustMinStrength.AppendKey(8.00f);

	m_track_GustMaxStrength.SetTrackName("Gust Max Strength");
	m_track_GustMaxStrength.AppendKey(15.00f);

	m_track_GustAscent.SetTrackName("Gust Ascent");
	m_track_GustAscent.AppendKey(ruPI * 0.2f);

	m_track_GustDescent.SetTrackName("Gust Descent");
	m_track_GustDescent.AppendKey(ruPI * 0.2f);
}

CRuController_Particle_Emitter_Enviro::~CRuController_Particle_Emitter_Enviro()
{
	// NOTE: Destruction of resources is done during the ruENTITYEVENT_ONDESTROY event.
}

IRuObject *CRuController_Particle_Emitter_Enviro::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_Particle_Emitter_Enviro();
	CRuController_Particle_Emitter_Enviro *controller = static_cast<CRuController_Particle_Emitter_Enviro *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_KeyableController::Clone(clonedObject);

	// Clone parameters
	controller->m_emitterPrerunTime = m_emitterPrerunTime;

	// Clone tracks
	CloneTracks(controller);

	return clonedObject;
}

BOOL CRuController_Particle_Emitter_Enviro::SerializeFrom(IRuStream *stream)
{
	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_KeyableController::SerializeFrom(stream);

	switch(version)
	{
		case 1:
			{
				stream->Read(&m_emitterPrerunTime, sizeof(REAL));

				// Serialize Tracks
				m_track_MaxParticles.SerializeFrom(stream);
				m_track_EmitterPosition.SerializeFrom(stream);
				m_track_EmitterExtents.SerializeFrom(stream);
				m_track_EmissionRate.SerializeFrom(stream);
				m_track_EmitDirection.SerializeFrom(stream);
				m_track_EmitDeviation.SerializeFrom(stream);
				m_track_ParticleActiveExtents.SerializeFrom(stream);
				m_track_ParticleMinExtinction.SerializeFrom(stream);
				m_track_ParticleMaxExtinction.SerializeFrom(stream);
				m_track_ParticleMinSpeed.SerializeFrom(stream);
				m_track_ParticleMaxSpeed.SerializeFrom(stream);
				m_track_ParticleMinScale.SerializeFrom(stream);
				m_track_ParticleMaxScale.SerializeFrom(stream);
				m_track_ParticleMinOrientation.SerializeFrom(stream);
				m_track_ParticleMaxOrientation.SerializeFrom(stream);
				m_track_Force0.SerializeFrom(stream);
				m_track_Force1.SerializeFrom(stream);
				m_track_GustFrequency.SerializeFrom(stream);
				m_track_GustAffectProbability.SerializeFrom(stream);
				m_track_GustTTL.SerializeFrom(stream);
				m_track_GustMinStrength.SerializeFrom(stream);
				m_track_GustMaxStrength.SerializeFrom(stream);
				m_track_GustAscent.SerializeFrom(stream);
				m_track_GustDescent.SerializeFrom(stream);
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_Particle_Emitter_Enviro::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_KeyableController::SerializeTo(stream);

	stream->Write(&m_emitterPrerunTime, sizeof(REAL));

	// Serialize Tracks
	m_track_MaxParticles.SerializeTo(stream);
	m_track_EmitterPosition.SerializeTo(stream);
	m_track_EmitterExtents.SerializeTo(stream);
	m_track_EmissionRate.SerializeTo(stream);
	m_track_EmitDirection.SerializeTo(stream);
	m_track_EmitDeviation.SerializeTo(stream);
	m_track_ParticleActiveExtents.SerializeTo(stream);
	m_track_ParticleMinExtinction.SerializeTo(stream);
	m_track_ParticleMaxExtinction.SerializeTo(stream);
	m_track_ParticleMinSpeed.SerializeTo(stream);
	m_track_ParticleMaxSpeed.SerializeTo(stream);
	m_track_ParticleMinScale.SerializeTo(stream);
	m_track_ParticleMaxScale.SerializeTo(stream);
	m_track_ParticleMinOrientation.SerializeTo(stream);
	m_track_ParticleMaxOrientation.SerializeTo(stream);
	m_track_Force0.SerializeTo(stream);
	m_track_Force1.SerializeTo(stream);
	m_track_GustFrequency.SerializeTo(stream);
	m_track_GustAffectProbability.SerializeTo(stream);
	m_track_GustTTL.SerializeTo(stream);
	m_track_GustMinStrength.SerializeTo(stream);
	m_track_GustMaxStrength.SerializeTo(stream);
	m_track_GustAscent.SerializeTo(stream);
	m_track_GustDescent.SerializeTo(stream);

	return TRUE;
}

BOOL CRuController_Particle_Emitter_Enviro::ResetStates()
{
	m_age = 0.0f;

	return IRuEntity_KeyableController::ResetStates();
}

BOOL CRuController_Particle_Emitter_Enviro::Update(REAL elapsedTime)
{
	if(m_enabled)
	{
	}

	// Chain update call to base controller class
	return IRuEntity_KeyableController::Update(elapsedTime);
}

IRuObject *CRuController_Particle_Emitter_Enviro::GetTarget()
{
	return m_particleEmitter;
}

BOOL CRuController_Particle_Emitter_Enviro::SetTarget(IRuObject *targetEntity)
{
	CRuEntity_Particle_Emitter_Enviro *emitter = NULL;

	if(targetEntity->GetType().IsSubClassOf(IRuEntity_Particle_Visualizer::Type()))
	{
		IRuEntity_Particle_Emitter *emitterInterface = static_cast<IRuEntity_Particle_Visualizer *>(targetEntity)->GetEmitter();

		if(emitterInterface->GetType() == CRuEntity_Particle_Emitter_Enviro::Type())
		{
			emitter = static_cast<CRuEntity_Particle_Emitter_Enviro *>(emitterInterface);
		}
	}
	else if(targetEntity->GetType().IsSubClassOf(CRuEntity_Particle_Emitter_Enviro::Type()))
	{
		emitter = static_cast<CRuEntity_Particle_Emitter_Enviro *>(targetEntity);
	}

	if(emitter)
	{
		// If we had a previous target, unregister it
		if(m_particleEmitter)
		{
			m_particleEmitter->Event_Dispose().UnregisterHandler(this->GetGUID());
			m_particleEmitter->Event_UpdateStep().UnregisterHandler(this->GetGUID());
		}

		// Store target reference, setup event listener, and setup target parameter block
		m_particleEmitter = emitter;
		m_particleEmitter->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Particle_Emitter_Enviro::HandleEvent_Dispose, this, _1), NULL);
		m_particleEmitter->Event_UpdateStep().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Particle_Emitter_Enviro::HandleEvent_UpdateStep, this, _1));

		m_particleEmitter->SetEmitterPrerunTime(m_emitterPrerunTime);
		m_particleEmitter->SetOBKillTime(m_particleOBKillTime);

		// If the emitter pre-run time is set, run a update cycle to update the controller's time
		if(m_emitterPrerunTime > 0.0f)
		{
			this->Update(m_emitterPrerunTime);
		}
	}

	return TRUE;
}

INT32 CRuController_Particle_Emitter_Enviro::GetNumTracks() const
{
	return 24;
}

BOOL CRuController_Particle_Emitter_Enviro::GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut)
{
	switch(trackIndex)
	{
		case 0:
			trackOut = &m_track_MaxParticles;
			return TRUE;

		case 1:
			trackOut = &m_track_EmitterPosition;
			return TRUE;

		case 2:
			trackOut = &m_track_EmitterExtents;
			return TRUE;

		case 3:
			trackOut = &m_track_EmissionRate;
			return TRUE;

		case 4:
			trackOut = &m_track_EmitDirection;
			return TRUE;

		case 5:
			trackOut = &m_track_EmitDeviation;
			return TRUE;

		case 6:
			trackOut = &m_track_ParticleActiveExtents;
			return TRUE;

		case 7:
			trackOut = &m_track_ParticleMinExtinction;
			return TRUE;

		case 8:
			trackOut = &m_track_ParticleMaxExtinction;
			return TRUE;

		case 9:
			trackOut = &m_track_ParticleMinSpeed;
			return TRUE;

		case 10:
			trackOut = &m_track_ParticleMaxSpeed;
			return TRUE;

		case 11:
			trackOut = &m_track_ParticleMinScale;
			return TRUE;

		case 12:
			trackOut = &m_track_ParticleMaxScale;
			return TRUE;

		case 13:
			trackOut = &m_track_ParticleMinOrientation;
			return TRUE;

		case 14:
			trackOut = &m_track_ParticleMaxOrientation;
			return TRUE;

		case 15:
			trackOut = &m_track_Force0;
			return TRUE;

		case 16:
			trackOut = &m_track_Force1;
			return TRUE;

		case 17:
			trackOut = &m_track_GustFrequency;
			return TRUE;

		case 18:
			trackOut = &m_track_GustAffectProbability;
			return TRUE;

		case 19:
			trackOut = &m_track_GustTTL;
			return TRUE;

		case 20:
			trackOut = &m_track_GustMinStrength;
			return TRUE;

		case 21:
			trackOut = &m_track_GustMaxStrength;
			return TRUE;

		case 22:
			trackOut = &m_track_GustAscent;
			return TRUE;

		case 23:
			trackOut = &m_track_GustDescent;
			return TRUE;
	}

	return FALSE;
}

BOOL CRuController_Particle_Emitter_Enviro::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_particleEmitter)
	{
		// Unregister from target entity
		m_particleEmitter->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_particleEmitter->Event_UpdateStep().UnregisterHandler(this->GetGUID());
		m_particleEmitter = NULL;
	}

	return TRUE;
}

BOOL CRuController_Particle_Emitter_Enviro::HandleEvent_UpdateStep(RuEventArgs *eventArgs)
{
	RuEntityEvent_UpdateStep_Args *updateArgs = static_cast<RuEntityEvent_UpdateStep_Args *>(eventArgs);

	// Update controller age
	m_age += updateArgs->m_elapsedTime;

	if(m_particleEmitter)
	{
		REAL relativeT = m_age - m_startTime;

		// If looping is enabled, clamp time range to the duration
		if(relativeT > m_duration && m_duration > 0.0f && m_loopTime >= 0.0f)
		{
			relativeT = fmod(relativeT, m_duration);
		}

		m_particleEmitter->SetMaxParticles(m_track_MaxParticles.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetEmitterPosition(m_track_EmitterPosition.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetEmitterExtents(m_track_EmitterExtents.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetEmissionRate(m_track_EmissionRate.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMinSpeed(m_track_ParticleMinSpeed.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMaxSpeed(m_track_ParticleMaxSpeed.GetInterpolatedKey(relativeT));
//		m_particleEmitter->SetDirection(m_track_EmitDirection.GetInterpolatedKey(relativeT));
//		m_particleEmitter->SetDeviation(m_track_EmitDeviation.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetParticleActiveExtents(m_track_ParticleActiveExtents.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMinExtinction(m_track_ParticleMinExtinction.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMaxExtinction(m_track_ParticleMaxExtinction.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMinScale(m_track_ParticleMinScale.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMaxScale(m_track_ParticleMaxScale.GetInterpolatedKey(relativeT));
//		m_particleEmitter->SetMinOrientation(m_track_ParticleMinOrientation.GetInterpolatedKey(relativeT));
//		m_particleEmitter->SetMaxOrientation(m_track_ParticleMaxOrientation.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetForce0(m_track_Force0.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetForce1(m_track_Force1.GetInterpolatedKey(relativeT));

		m_particleEmitter->SetGustFrequency(m_track_GustFrequency.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetGustAffectProbability(m_track_GustAffectProbability.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetGustTTL(m_track_GustTTL.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetGustMinStrength(m_track_GustMinStrength.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetGustMaxStrength(m_track_GustMaxStrength.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetGustAscent(m_track_GustAscent.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetGustDescent(m_track_GustDescent.GetInterpolatedKey(relativeT));
	}

	return TRUE;
}

REAL CRuController_Particle_Emitter_Enviro::GetEmitterPrerunTime() const
{
	return m_emitterPrerunTime;
}

void CRuController_Particle_Emitter_Enviro::SetEmitterPrerunTime(REAL prerunTime)
{
	m_emitterPrerunTime = prerunTime;
}

REAL CRuController_Particle_Emitter_Enviro::GetOBKillTime() const
{
	return m_particleOBKillTime;
}

void CRuController_Particle_Emitter_Enviro::SetOBKillTime(REAL obKillTime)
{
	m_particleOBKillTime = obKillTime;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
