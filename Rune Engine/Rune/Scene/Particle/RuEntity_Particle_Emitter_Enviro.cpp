/*!
	@file RuEntity_Particle_Emitter_Enviro.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Scene/Particle/RuEntity_Particle_Emitter_Enviro.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Particle_Emitter_Enviro, IRuEntity_Particle_Emitter)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Particle_Emitter_Enviro, "CRuEntity_Particle_Emitter_Enviro", "IRuEntity_Particle_Emitter")
ruCLASSGUID_DEFINE(CRuEntity_Particle_Emitter_Enviro, 0x7B132E91, 0x08DC4bed, 0xAF251E1D, 0x4793FF27)

// ************************************************************************************************************************************************************

CRuEntity_Particle_Emitter_Enviro::CRuEntity_Particle_Emitter_Enviro()
:	m_nextParticleIteration(0),
	m_currentEmitterPosition(0.0f, 0.0f, 0.0f),
	m_updatePeriod(0.030f),
	m_t(0.0f),
	m_accumT(0.0f),
	m_particleGenerationDebt(0.0f),
	m_visualizer(NULL),
	m_maxParticles(200),
	m_emitterPosition(0.0f, 0.0f, 0.0f),
	m_emitterExtents(10.0f, 5.0f, 10.0f),
	m_emissionRate(50.0f),
	m_emitterPrerunTime(0.0f),
	m_particleOBKillTime(0.3f),
	m_particleMinExtinction(1.0f),
	m_particleMaxExtinction(1.0f),
	m_particleMinSpeed(5.0f),
	m_particleMaxSpeed(10.0f),
	m_particleMinScale(1.0f),
	m_particleMaxScale(1.0f),
	m_force0(0.0f, 0.0f, 0.0f),
	m_force1(0.0f, 0.0f, 0.0f),
	m_gustFrequency(0.50f),
	m_gustAffectProbability(0.10f),
	m_gustTTL(0.5f),
	m_gustMinStrength(5.0f),
	m_gustMaxStrength(10.0f),
	m_gustAscent(ruPI * 0.2f),
	m_gustDescent(ruPI * 0.2f)
{
}

CRuEntity_Particle_Emitter_Enviro::~CRuEntity_Particle_Emitter_Enviro()
{
}

IRuObject *CRuEntity_Particle_Emitter_Enviro::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuEntity_Particle_Emitter_Enviro();

	// Chain cloning call to parent class
	IRuEntity_Particle_Emitter::Clone(clonedObject);

	// Shortcut reference to the cloned entity
	CRuEntity_Particle_Emitter_Enviro *clonedPSys = static_cast<CRuEntity_Particle_Emitter_Enviro *>(clonedObject);

	// Copy runtime information
	clonedPSys->m_accumT = 0.0f;

	// Copy emitter settings
	clonedPSys->m_maxParticles = m_maxParticles;

	clonedPSys->m_emitterPosition = m_emitterPosition;
	clonedPSys->m_emitterExtents = m_emitterExtents;

	clonedPSys->m_emissionRate = m_emissionRate;

	clonedPSys->m_emitterPrerunTime = m_emitterPrerunTime;

	clonedPSys->m_particleOBKillTime = m_particleOBKillTime;

	clonedPSys->m_particleActiveExtents = m_particleActiveExtents;

	clonedPSys->m_particleMinExtinction = m_particleMinExtinction;
	clonedPSys->m_particleMaxExtinction = m_particleMaxExtinction;

	clonedPSys->m_particleMinSpeed = m_particleMinSpeed;
	clonedPSys->m_particleMaxSpeed = m_particleMaxSpeed;

	clonedPSys->m_particleMinScale = m_particleMinScale;
	clonedPSys->m_particleMaxScale = m_particleMaxScale;

	clonedPSys->m_force0 = m_force0;
	clonedPSys->m_force1 = m_force1;

	clonedPSys->m_gustFrequency = m_gustFrequency;
	clonedPSys->m_gustAffectProbability = m_gustAffectProbability;
	clonedPSys->m_gustTTL = m_gustTTL;
	clonedPSys->m_gustMinStrength = m_gustMinStrength;
	clonedPSys->m_gustMaxStrength = m_gustMaxStrength;
	clonedPSys->m_gustAscent = m_gustAscent;
	clonedPSys->m_gustDescent = m_gustDescent;

	// Clone the visualizer
	if(m_visualizer)
	{
		IRuObject *clonedVisualier = m_visualizer->Clone();
		if(clonedVisualier && clonedVisualier->GetType().IsSubClassOf(IRuEntity_Particle_Visualizer::Type()))
		{
			clonedPSys->m_visualizer = static_cast<IRuEntity_Particle_Visualizer *>(clonedVisualier);

			// Disable automatic cloning and serialization
			clonedPSys->m_visualizer->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
			clonedPSys->m_visualizer->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);
			clonedPSys->m_visualizer->SetProperty("act:no enumerate", TRUE);

			clonedPSys->m_visualizer->SetEmitter(clonedPSys);
			clonedPSys->AddChild(clonedPSys->m_visualizer);
		}
	}

	return clonedPSys;
}

BOOL CRuEntity_Particle_Emitter_Enviro::SerializeFrom(IRuStream *stream)
{
	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain to base class
	IRuEntity_Particle_Emitter::SerializeFrom(stream);

	// If particle emitter is available, serialize particle emitter
	BOOL visualizerAvailable = TRUE;
	stream->Read(&visualizerAvailable, sizeof(BOOL));

	if(visualizerAvailable)
	{
		IRuObject *object = RuObjectFactory().CreateObject(stream);
		if(object && object->GetType().IsSubClassOf(IRuEntity_Particle_Visualizer::Type()))
		{
			m_visualizer = static_cast<IRuEntity_Particle_Visualizer *>(object);

			// Disable automatic cloning and serialization
			m_visualizer->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
			m_visualizer->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);
			m_visualizer->SetProperty("act:no enumerate", TRUE);

			m_visualizer->SetEmitter(this);
			this->AddChild(m_visualizer);
		}
		else
		{
			ruSAFE_RELEASE(object);
		}
	}

	return TRUE;
}

BOOL CRuEntity_Particle_Emitter_Enviro::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain to base class
	IRuEntity_Particle_Emitter::SerializeTo(stream);

	// If particle visualizer is available, serialize particle visualizer
	if(m_visualizer)
	{
		BOOL visualizerAvailable = TRUE;
		stream->Write(&visualizerAvailable, sizeof(BOOL));
		m_visualizer->SerializeTo(stream);
	}
	else
	{
		BOOL visualizerAvailable = FALSE;
		stream->Write(&visualizerAvailable, sizeof(BOOL));
	}

	return TRUE;
}

BOOL CRuEntity_Particle_Emitter_Enviro::ResetStates()
{
	m_t = 0.0f;
	m_accumT = 0.0f;

	for(INT32 i = 0; i < m_particles.Count(); ++i)
	{
		if(m_particles[i].m_active)
		{
			m_particles[i].m_active = FALSE;
			m_inactiveParticles.Push(i);
		}
	}

	return IRuEntity_Particle_Emitter::ResetStates();
}

BOOL CRuEntity_Particle_Emitter_Enviro::Update(REAL elapsedTime)
{
	BOOL retVal = IRuEntity_Particle_Emitter::Update(elapsedTime);

	const CRuMatrix4x4 &worldTransform = this->GetWorldTransform();
	BOOL initialIteration = (m_t <= 0.0f) ? TRUE : FALSE;
	CRuVector3 m_accel = m_force0 + m_force1;

	// If emitter clock is at zero, modify elapsed time by the pre-run time
	BOOL prerunRunning = FALSE;
	if(m_t <= 0.0f && m_emitterPrerunTime > 0.0f)
	{
		prerunRunning = TRUE;
		elapsedTime += m_emitterPrerunTime;
	}

	// Update time accumulator
	m_accumT += elapsedTime;

	// Clamp update to a minimum period of 20 fps if this is not the pre-run loop
	if(prerunRunning == FALSE)
	{
		m_accumT = min(m_accumT, 0.50f);
	}

	while(m_accumT >= m_updatePeriod)
	{
		// Make sure that we always run one iteration at time = 0!
		if(initialIteration)
		{
			// Trigger update step event
			RuEntityEvent_UpdateStep_Args updateArgs(this, 0.0f);
			Event_UpdateStep().Trigger(&updateArgs);

			initialIteration = FALSE;
		}
		else
		{
			// Trigger update step event
			RuEntityEvent_UpdateStep_Args updateArgs(this, m_updatePeriod);
			Event_UpdateStep().Trigger(&updateArgs);

			// Decrement time accumulator
			m_accumT -= m_updatePeriod;

			// Update emitter clock
			m_t += m_updatePeriod;
		}

		// Update emitter position
		m_currentEmitterPosition = m_emitterPosition;

		// Calculate effective emitter and collector position
		CRuVector3 effectiveEmitterPosition;
		worldTransform.TransformPoint(m_currentEmitterPosition, effectiveEmitterPosition);

		// Build a world-space bounding box for the particle active bounds
		CRuAABB particleActiveBounds(effectiveEmitterPosition - m_particleActiveExtents, effectiveEmitterPosition + m_particleActiveExtents);

		// Determine whether to enable gust effect during this update cycle
		REAL gustStrength = 0.0f;
		CRuVector3 gustVector(0.0f, 0.0f, 1.0f);
		BOOL enableGust = (RuGlobalMTRand().GenerateFloat() < m_updatePeriod * m_gustFrequency);
		if(enableGust)
		{
			// Pick a gust strength
			gustStrength = RuGlobalMTRand().GenerateFloat() * (m_gustMaxStrength - m_gustMinStrength) + m_gustMinStrength;

			// Pick an elevation and rotation
			REAL gustElevation = RuGlobalMTRand().GenerateFloat() * (m_gustAscent + m_gustDescent) - m_gustDescent;
			REAL gustRotation = RuGlobalMTRand().GenerateFloat() * ruPI * 2.0f;

			// Transform gust vector by the elevation
			CRuQuaternion elevationQuat;
			elevationQuat.FromAngleAxis(CRuVector3(1.0f, 0.0f, 0.0f), gustElevation);
			elevationQuat.TransformVector(gustVector);

			// Transform gust vector by the rotation
			CRuQuaternion rotationQuat;
			rotationQuat.FromAngleAxis(CRuVector3(0.0f, 1.0f, 0.0f), gustRotation);
			rotationQuat.TransformVector(gustVector);
		}

		// Update particles
		for(INT32 i = 0; i < m_particles.Count(); ++i)
		{
			CRuParticleDesc &particle = m_particles[i];

			if(particle.m_active)
			{
				// Update gust
				if(enableGust)
				{
					if(RuGlobalMTRand().GenerateFloat() < m_gustAffectProbability)
					{
						particle.m_transientAccelerationTTL = 0.5f;
						particle.m_transientAcceleration = gustVector * gustStrength;
					}
				}

				// Decrement transient acceleration TTL and reset transient acceleration if TTL has expired
				particle.m_transientAccelerationTTL -= m_updatePeriod;
				if(particle.m_transientAccelerationTTL <= 0.0f)
				{
					particle.m_transientAccelerationTTL = 0.0f;
					particle.m_transientAcceleration.Set(0.0f, 0.0f, 0.0f);
				}

				// Update particle velocity and position
				particle.m_velocity += m_accel * m_updatePeriod + particle.m_transientAcceleration * m_updatePeriod;
				particle.m_position += particle.m_velocity * m_updatePeriod;

				// Update particle movement direction vector
				CRuVector3 dirCont;
				dirCont += particle.m_velocity * m_updatePeriod;

				if(dirCont.SquaredMagnitude() > ruEPSILON)
				{
					particle.m_direction = dirCont;
					particle.m_direction.Normalize();
				}

				// Check to see if the particle is beyond the active extents
				if(!particleActiveBounds.Contains(particle.m_position))
				{
					// Forcibly set particle age such that it will disappear within the allotted time
					particle.m_age = max(particle.m_age, particle.m_extinction - m_particleOBKillTime);
				}

				// If the particle's age is longer than the extinction time, deactivate it
				particle.m_age += m_updatePeriod;
				if(particle.m_age >= particle.m_extinction)
				{
					particle.m_active = FALSE;
					m_inactiveParticles.Push(i);
				}
			}
		}

		// Add to the particle generation debt
		m_particleGenerationDebt += m_updatePeriod * m_emissionRate;

		INT32 numActiveParticles = m_particles.Count() - m_inactiveParticles.Count();
		INT32 particlesToGenerate = min(static_cast<INT32>(m_particleGenerationDebt), max(0, m_maxParticles - numActiveParticles));

		if(particlesToGenerate > 0)
		{
			REAL speedRange = m_particleMaxSpeed - m_particleMinSpeed;
			REAL extinctionRange = m_particleMaxExtinction - m_particleMinExtinction;

			// Decrement particle generation debt
			m_particleGenerationDebt -= particlesToGenerate;

			// Generate particles
			for(INT32 i = 0; i < particlesToGenerate; ++i)
			{
				// Get an available particle slot
				INT32 particleIndex;
				if(m_inactiveParticles.IsEmpty())
				{
					particleIndex = m_particles.Count();
					m_particles.Add(CRuParticleDesc());
				}
				else
				{
					particleIndex = m_inactiveParticles.Pop();
				}

				// Shortcut to the particle
				CRuParticleDesc &particle = m_particles[particleIndex];

				// Generate particle position
				particle.m_origin.Set(	effectiveEmitterPosition.x + (RuGlobalMTRand().GenerateFloat() - 0.5f) * 2.0f * m_emitterExtents.x,
										effectiveEmitterPosition.y + (RuGlobalMTRand().GenerateFloat() - 0.5f) * 2.0f * m_emitterExtents.y,
										effectiveEmitterPosition.z + (RuGlobalMTRand().GenerateFloat() - 0.5f) * 2.0f * m_emitterExtents.z);

				particle.m_position = particle.m_origin;

				particle.m_orientation.Set(0.0f, 0.0f, 0.0f);

				particle.m_active = TRUE;
				particle.m_particleIteration = ++m_nextParticleIteration;

				particle.m_extinction = m_particleMinExtinction + RuGlobalMTRand().GenerateFloat() * extinctionRange;
				particle.m_age = 0.0f;

				particle.m_velocity = particle.m_direction * (m_particleMinSpeed + (RuGlobalMTRand().GenerateFloat() * speedRange));

				particle.m_scale = RuGlobalMTRand().GenerateFloat() * (m_particleMaxScale - m_particleMinScale) + m_particleMinScale;

				particle.m_transientAccelerationTTL = 0.0f;
				particle.m_transientAcceleration.Set(0.0f, 0.0f, 0.0f);
			}
		}
	}

	return TRUE;
}

BOOL CRuEntity_Particle_Emitter_Enviro::QueryCollision(CRuCollisionQuery *colQuery)
{
	// Save previous collision transform
	CRuMatrix4x4 *originalWorldTransform = colQuery->m_worldTransform;
	CRuMatrix4x4 *originalInvWorldTransform = colQuery->m_invWorldTransform;

	// Store the inverse of the current world transform into the collision query
	colQuery->m_worldTransform = const_cast<CRuMatrix4x4 *>(&this->GetWorldTransform());
	colQuery->m_invWorldTransform = const_cast<CRuMatrix4x4 *>(&this->GetInverseWorldTransform());

	switch(colQuery->m_queryType)
	{
		case ruCOLQUERY_RAY:
			{
				CRuCollisionQuery_Ray *rayQuery = static_cast<CRuCollisionQuery_Ray *>(colQuery);

				// Test ray with bounding box
				if(Intersects(rayQuery->m_colRay, this->GetWorldBounds()))
				{
					rayQuery->m_colPoints->Add(m_currentEmitterPosition);
					rayQuery->m_colDistances->Add(0.0f);
				}
			}

			break;
	}

	// Restore original collision transform
	colQuery->m_worldTransform = originalWorldTransform;
	colQuery->m_invWorldTransform = originalInvWorldTransform;

	return TRUE;
}

void CRuEntity_Particle_Emitter_Enviro::CalculateEntityBounds()
{
	REAL radius = (m_particleActiveExtents * 2.0f).Magnitude();

	this->SetLocalEntityBounds(CRuSphere(CRuVector3(0.0f, 0.0f, 0.0f), radius));
}

BOOL CRuEntity_Particle_Emitter_Enviro::IsEmbeddedEmitter() const
{
	return FALSE;
}

CRuArrayList<CRuParticleDesc> *CRuEntity_Particle_Emitter_Enviro::GetParticleDescriptors()
{
	return &m_particles;
}

RuParticleFrameType CRuEntity_Particle_Emitter_Enviro::GetFrameType() const
{
	return ruPRTFRAMETYPE_WORLD;
}

RuOrthogonalAxis CRuEntity_Particle_Emitter_Enviro::GetEmitterOrientation() const
{
	return ruAXIS_Z_NEGATIVE;
}

IRuEntity_Particle_Visualizer *CRuEntity_Particle_Emitter_Enviro::GetVisualizer()
{
	return m_visualizer;
}

BOOL CRuEntity_Particle_Emitter_Enviro::SetVisualizer(IRuEntity_Particle_Visualizer *visualizer)
{
	if(visualizer)
	{
		// Increment reference count on new visualizer
		visualizer->AddRef();

		// If there was an existing visualizer, remove it
		if(m_visualizer)
		{
			m_visualizer->DetachFromParent();
			ruSAFE_RELEASE(m_visualizer);
		}

		// Store reference to visualizer
		m_visualizer = visualizer;
		m_visualizer->SetEmitter(this);
		this->AddChild(m_visualizer);

		// Disable automatic cloning and serialization
		m_visualizer->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
		m_visualizer->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);
		m_visualizer->SetProperty("act:no enumerate", TRUE);

		return TRUE;
	}

	return FALSE;
}

INT32 CRuEntity_Particle_Emitter_Enviro::GetMaxParticles() const
{
	return m_maxParticles;
}

void CRuEntity_Particle_Emitter_Enviro::SetMaxParticles(INT32 maxParticles)
{
	m_maxParticles = maxParticles;
}

const CRuVector3 &CRuEntity_Particle_Emitter_Enviro::GetEmitterPosition() const
{
	return m_emitterPosition;
}

void CRuEntity_Particle_Emitter_Enviro::SetEmitterPosition(CRuVector3 &position)
{
	m_emitterPosition = position;
}

const CRuVector3 &CRuEntity_Particle_Emitter_Enviro::GetEmitterExtents() const
{
	return m_emitterExtents;
}

void CRuEntity_Particle_Emitter_Enviro::SetEmitterExtents(CRuVector3 &extents)
{
	m_emitterExtents = extents;
}

REAL CRuEntity_Particle_Emitter_Enviro::GetEmissionRate() const
{
	return m_emissionRate;
}

void CRuEntity_Particle_Emitter_Enviro::SetEmissionRate(REAL emissionRate)
{
	m_emissionRate = emissionRate;
}

REAL CRuEntity_Particle_Emitter_Enviro::GetEmitterPrerunTime() const
{
	return m_emitterPrerunTime;
}

void CRuEntity_Particle_Emitter_Enviro::SetEmitterPrerunTime(REAL prerunTime)
{
	m_emitterPrerunTime = prerunTime;
}

REAL CRuEntity_Particle_Emitter_Enviro::GetOBKillTime() const
{
	return m_particleOBKillTime;
}

void CRuEntity_Particle_Emitter_Enviro::SetOBKillTime(REAL obKillTime)
{
	m_particleOBKillTime = obKillTime;
}

const CRuVector3 &CRuEntity_Particle_Emitter_Enviro::GetParticleActiveExtents() const
{
	return m_particleActiveExtents;
}

void CRuEntity_Particle_Emitter_Enviro::SetParticleActiveExtents(CRuVector3 &extents)
{
	m_particleActiveExtents = extents;

	// Force an update of the local entity bounds
	this->MarkLocalEntityBoundsDirty();
}

REAL CRuEntity_Particle_Emitter_Enviro::GetMinExtinction() const
{
	return m_particleMinExtinction;
}

void CRuEntity_Particle_Emitter_Enviro::SetMinExtinction(REAL extinction)
{
	m_particleMinExtinction = extinction;
}

REAL CRuEntity_Particle_Emitter_Enviro::GetMaxExtinction() const
{
	return m_particleMaxExtinction;
}

void CRuEntity_Particle_Emitter_Enviro::SetMaxExtinction(REAL extinction)
{
	m_particleMaxExtinction = extinction;
}

REAL CRuEntity_Particle_Emitter_Enviro::GetMinSpeed() const
{
	return m_particleMinSpeed;
}

void CRuEntity_Particle_Emitter_Enviro::SetMinSpeed(REAL minSpeed)
{
	m_particleMinSpeed = minSpeed;
}

REAL CRuEntity_Particle_Emitter_Enviro::GetMaxSpeed() const
{
	return m_particleMaxSpeed;
}

void CRuEntity_Particle_Emitter_Enviro::SetMaxSpeed(REAL maxSpeed)
{
	m_particleMaxSpeed = maxSpeed;
}

REAL CRuEntity_Particle_Emitter_Enviro::GetMinScale() const
{
	return m_particleMinScale;
}

void CRuEntity_Particle_Emitter_Enviro::SetMinScale(REAL scale)
{
	m_particleMinScale = scale;
}

REAL CRuEntity_Particle_Emitter_Enviro::GetMaxScale() const
{
	return m_particleMaxScale;
}

void CRuEntity_Particle_Emitter_Enviro::SetMaxScale(REAL scale)
{
	m_particleMaxScale = scale;
}

const CRuVector3 &CRuEntity_Particle_Emitter_Enviro::GetForce0() const
{
	return m_force0;
}

void CRuEntity_Particle_Emitter_Enviro::SetForce0(CRuVector3 &force)
{
	m_force0 = force;
}

const CRuVector3 &CRuEntity_Particle_Emitter_Enviro::GetForce1() const
{
	return m_force1;
}

void CRuEntity_Particle_Emitter_Enviro::SetForce1(CRuVector3 &force)
{
	m_force1 = force;
}

REAL CRuEntity_Particle_Emitter_Enviro::GetGustFrequency() const
{
	return m_gustFrequency;
}

void CRuEntity_Particle_Emitter_Enviro::SetGustFrequency(REAL gustFrequency)
{
	m_gustFrequency = gustFrequency;
}

REAL CRuEntity_Particle_Emitter_Enviro::GetGustAffectProbability() const
{
	return m_gustAffectProbability;
}

void CRuEntity_Particle_Emitter_Enviro::SetGustAffectProbability(REAL gustAffectProbability)
{
	m_gustAffectProbability = gustAffectProbability;
}

REAL CRuEntity_Particle_Emitter_Enviro::GetGustTTL() const
{
	return m_gustTTL;
}

void CRuEntity_Particle_Emitter_Enviro::SetGustTTL(REAL gustTTL)
{
	m_gustTTL = gustTTL;
}

REAL CRuEntity_Particle_Emitter_Enviro::GetGustMinStrength() const
{
	return m_gustMinStrength;
}

void CRuEntity_Particle_Emitter_Enviro::SetGustMinStrength(REAL minStr)
{
	m_gustMinStrength = minStr;
}

REAL CRuEntity_Particle_Emitter_Enviro::GetGustMaxStrength() const
{
	return m_gustMaxStrength;
}

void CRuEntity_Particle_Emitter_Enviro::SetGustMaxStrength(REAL maxStr)
{
	m_gustMaxStrength = maxStr;
}

REAL CRuEntity_Particle_Emitter_Enviro::GetGustAscent() const
{
	return m_gustAscent;
}

void CRuEntity_Particle_Emitter_Enviro::SetGustAscent(REAL gustAscent)
{
	m_gustAscent = gustAscent;
}

REAL CRuEntity_Particle_Emitter_Enviro::GetGustDescent() const
{
	return m_gustDescent;
}

void CRuEntity_Particle_Emitter_Enviro::SetGustDescent(REAL gustDescent)
{
	m_gustDescent = gustDescent;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
