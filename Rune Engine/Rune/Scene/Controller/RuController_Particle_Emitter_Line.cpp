#include "../../Scene/Controller/RuController_Particle_Emitter_Line.h"
#include "../../Helper/RuHelper_Entity.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_Particle_Emitter_Line, IRuEntity_KeyableController)
ruHEAP_DEFINE_SUBCLASS(CRuController_Particle_Emitter_Line, "CRuController_Particle_Emitter_Line", "IRuEntity_KeyableController")
ruCLASSGUID_DEFINE(CRuController_Particle_Emitter_Line, 0x5C04780B, 0x0C47443e, 0xA90B71BC, 0xFDF93FEC)

// ************************************************************************************************************************************************************

CRuController_Particle_Emitter_Line::CRuController_Particle_Emitter_Line()
:	m_emitterPrerunTime(0.0f),
	m_particleEmitter(NULL),
	m_prevLinkPt0Name(NULL),
	m_prevLinkPt1Name(NULL)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Particle_Emitter_Line::HandleEvent_Dispose, this, _1), NULL);

	// Override default controller clock update
	this->m_overrideClockUpdate = TRUE;

	m_track_MaxParticles.SetTrackCategory("Emitter");
	m_track_MaxParticles.SetTrackName("Maximum Particles");
	m_track_MaxParticles.AppendKey(200);

	m_track_EmissionLinkPt0.SetTrackCategory("Emitter");
	m_track_EmissionLinkPt0.SetTrackName("Emission Link Pt 0");

	m_track_EmissionLinkPt1.SetTrackCategory("Emitter");
	m_track_EmissionLinkPt1.SetTrackName("Emission Link Pt 1");

	m_track_EmissionPositionWeight.SetTrackCategory("Emitter");
	m_track_EmissionPositionWeight.SetTrackName("Emission Pos Weight");
	m_track_EmissionPositionWeight.AppendKey(0.0f);

	m_track_EmissionPositionDeviation.SetTrackCategory("Emitter");
	m_track_EmissionPositionDeviation.SetTrackName("Emission Pos Deviation");
	m_track_EmissionPositionDeviation.AppendKey(0.0f);

	m_track_EmissionRadialDeviation.SetTrackCategory("Emitter");
	m_track_EmissionRadialDeviation.SetTrackName("Emission Radial Deviation");
	m_track_EmissionRadialDeviation.AppendKey(1.0f);

	m_track_EmissionRate.SetTrackCategory("Emitter");
	m_track_EmissionRate.SetTrackName("Emission Rate");
	m_track_EmissionRate.AppendKey(5.0f);

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
}

CRuController_Particle_Emitter_Line::~CRuController_Particle_Emitter_Line()
{
	// NOTE: Destruction of resources is done during the ruENTITYEVENT_ONDESTROY event.
}

IRuObject *CRuController_Particle_Emitter_Line::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_Particle_Emitter_Line();
	CRuController_Particle_Emitter_Line *controller = static_cast<CRuController_Particle_Emitter_Line *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_KeyableController::Clone(clonedObject);

	// Clone parameters
	controller->m_emitterPrerunTime = m_emitterPrerunTime;

	// Clone tracks
	CloneTracks(controller);

	return clonedObject;
}

BOOL CRuController_Particle_Emitter_Line::SerializeFrom(IRuStream *stream)
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
				m_track_EmissionLinkPt0.SerializeFrom(stream);
				m_track_EmissionLinkPt1.SerializeFrom(stream);
				m_track_EmissionPositionWeight.SerializeFrom(stream);
				m_track_EmissionPositionDeviation.SerializeFrom(stream);
				m_track_EmissionRadialDeviation.SerializeFrom(stream);
				m_track_EmissionRate.SerializeFrom(stream);
				m_track_ParticleMinExtinction.SerializeFrom(stream);
				m_track_ParticleMaxExtinction.SerializeFrom(stream);
				m_track_ParticleMinSpeed.SerializeFrom(stream);
				m_track_ParticleMaxSpeed.SerializeFrom(stream);
				m_track_ParticleMinScale.SerializeFrom(stream);
				m_track_ParticleMaxScale.SerializeFrom(stream);
				m_track_ParticleMinOrientation.SerializeFrom(stream);
				m_track_ParticleMaxOrientation.SerializeFrom(stream);
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_Particle_Emitter_Line::SerializeTo(IRuStream *stream)
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
	m_track_EmissionLinkPt0.SerializeTo(stream);
	m_track_EmissionLinkPt1.SerializeTo(stream);
	m_track_EmissionPositionWeight.SerializeTo(stream);
	m_track_EmissionPositionDeviation.SerializeTo(stream);
	m_track_EmissionRadialDeviation.SerializeTo(stream);
	m_track_EmissionRate.SerializeTo(stream);
	m_track_ParticleMinExtinction.SerializeTo(stream);
	m_track_ParticleMaxExtinction.SerializeTo(stream);
	m_track_ParticleMinSpeed.SerializeTo(stream);
	m_track_ParticleMaxSpeed.SerializeTo(stream);
	m_track_ParticleMinScale.SerializeTo(stream);
	m_track_ParticleMaxScale.SerializeTo(stream);
	m_track_ParticleMinOrientation.SerializeTo(stream);
	m_track_ParticleMaxOrientation.SerializeTo(stream);

	return TRUE;
}

BOOL CRuController_Particle_Emitter_Line::ResetStates()
{
	m_age = 0.0f;

	return IRuEntity_KeyableController::ResetStates();
}

BOOL CRuController_Particle_Emitter_Line::Update(REAL elapsedTime)
{
	if(m_enabled)
	{
	}

	// Chain update call to base controller class
	return IRuEntity_KeyableController::Update(elapsedTime);
}

IRuObject *CRuController_Particle_Emitter_Line::GetTarget()
{
	return m_particleEmitter;
}

BOOL CRuController_Particle_Emitter_Line::SetTarget(IRuObject *targetEntity)
{
	CRuEntity_Particle_Emitter_Line *emitter = NULL;

	if(targetEntity->GetType().IsSubClassOf(IRuEntity_Particle_Visualizer::Type()))
	{
		IRuEntity_Particle_Emitter *emitterInterface = static_cast<IRuEntity_Particle_Visualizer *>(targetEntity)->GetEmitter();

		if(emitterInterface->GetType() == CRuEntity_Particle_Emitter_Line::Type())
		{
			emitter = static_cast<CRuEntity_Particle_Emitter_Line *>(emitterInterface);
		}
	}
	else if(targetEntity->GetType().IsSubClassOf(CRuEntity_Particle_Emitter_Line::Type()))
	{
		emitter = static_cast<CRuEntity_Particle_Emitter_Line *>(targetEntity);
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
		m_particleEmitter->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Particle_Emitter_Line::HandleEvent_Dispose, this, _1), NULL);
		m_particleEmitter->Event_UpdateStep().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Particle_Emitter_Line::HandleEvent_UpdateStep, this, _1));

		m_particleEmitter->SetEmitterPrerunTime(m_emitterPrerunTime);

		// If the emitter pre-run time is set, run a update cycle to update the controller's time
		if(m_emitterPrerunTime > 0.0f)
		{
			this->Update(m_emitterPrerunTime);
		}
	}

	return TRUE;
}

INT32 CRuController_Particle_Emitter_Line::GetNumTracks() const
{
	return 15;
}

BOOL CRuController_Particle_Emitter_Line::GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut)
{
	switch(trackIndex)
	{
		case 0:
			trackOut = &m_track_MaxParticles;
			return TRUE;

		case 1:
			trackOut = &m_track_EmissionLinkPt0;
			return TRUE;

		case 2:
			trackOut = &m_track_EmissionLinkPt1;
			return TRUE;

		case 3:
			trackOut = &m_track_EmissionPositionWeight;
			return TRUE;

		case 4:
			trackOut = &m_track_EmissionPositionDeviation;
			return TRUE;

		case 5:
			trackOut = &m_track_EmissionRadialDeviation;
			return TRUE;

		case 6:
			trackOut = &m_track_EmissionRate;
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
	}

	return FALSE;
}

BOOL CRuController_Particle_Emitter_Line::HandleEvent_Dispose(RuEventArgs *eventArgs)
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

BOOL CRuController_Particle_Emitter_Line::HandleEvent_UpdateStep(RuEventArgs *eventArgs)
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
		m_particleEmitter->SetEmissionPositionWeight(m_track_EmissionPositionWeight.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetEmissionPositionDeviation(m_track_EmissionPositionDeviation.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetEmissionRadialDeviation(m_track_EmissionRadialDeviation.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetEmissionRate(m_track_EmissionRate.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMinSpeed(m_track_ParticleMinSpeed.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMaxSpeed(m_track_ParticleMaxSpeed.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMinExtinction(m_track_ParticleMinExtinction.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMaxExtinction(m_track_ParticleMaxExtinction.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMinScale(m_track_ParticleMinScale.GetInterpolatedKey(relativeT));
		m_particleEmitter->SetMaxScale(m_track_ParticleMaxScale.GetInterpolatedKey(relativeT));

		// Find the entity specified by link points
		char *linkPt0Name = m_track_EmissionLinkPt0.GetDiscreteKey(relativeT);
		if(linkPt0Name)
		{
			if(linkPt0Name != m_prevLinkPt0Name)
			{
				m_prevLinkPt0Name = linkPt0Name;

				if(strlen(linkPt0Name) > 0)
				{
					CRuEntity *closestTargetMatch = RuEntity_FindNearestEntityByName(m_particleEmitter, linkPt0Name);

					if(closestTargetMatch)
					{
						m_particleEmitter->SetEmissionLinkPt0(closestTargetMatch);
					}
				}
			}
		}

		char *linkPt1Name = m_track_EmissionLinkPt1.GetDiscreteKey(relativeT);
		if(linkPt1Name)
		{
			if(linkPt1Name != m_prevLinkPt1Name)
			{
				m_prevLinkPt1Name = linkPt1Name;

				if(strlen(linkPt1Name) > 0)
				{
					CRuEntity *closestTargetMatch = RuEntity_FindNearestEntityByName(m_particleEmitter, linkPt1Name);

					if(closestTargetMatch)
					{
						m_particleEmitter->SetEmissionLinkPt1(closestTargetMatch);
					}
				}
			}
		}
	}

	return TRUE;
}

REAL CRuController_Particle_Emitter_Line::GetEmitterPrerunTime() const
{
	return m_emitterPrerunTime;
}

void CRuController_Particle_Emitter_Line::SetEmitterPrerunTime(REAL prerunTime)
{
	m_emitterPrerunTime = prerunTime;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
