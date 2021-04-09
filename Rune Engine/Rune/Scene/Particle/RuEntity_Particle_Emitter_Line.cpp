/*!
	@file RuEntity_Particle_Emitter_Line.cpp

	Copyright (c) 2004-2008 Runewaker Entertainment. All rights reserved.
*/

#include "../../Scene/Particle/RuEntity_Particle_Emitter_Line.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Particle_Emitter_Line, IRuEntity_Particle_Emitter)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Particle_Emitter_Line, "CRuEntity_Particle_Emitter_Line", "IRuEntity_Particle_Emitter")
ruCLASSGUID_DEFINE(CRuEntity_Particle_Emitter_Line, 0xEEE335E1, 0x6EBC490d, 0xB41F82C5, 0xDF5FE5A8)

// ************************************************************************************************************************************************************

CRuEntity_Particle_Emitter_Line::CRuEntity_Particle_Emitter_Line()
:	m_nextParticleIteration(0),
	m_updatePeriod(0.030f),
	m_t(0.0f),
	m_accumT(0.0f),
	m_particleGenerationDebt(0.0f),
	m_visualizer(NULL),
	m_maxParticles(200),
	m_emissionLinkPt0(NULL),
	m_emissionLinkPt1(NULL),
	m_emissionPositionWeight(0.0f),
	m_emissionPositionDeviation(0.0f),
	m_emissionRadialDeviation(1.0f),
	m_emissionRate(50.0f),
	m_emitterPrerunTime(0.0f),
	m_particleMinExtinction(1.0f),
	m_particleMaxExtinction(1.0f),
	m_particleMinSpeed(5.0f),
	m_particleMaxSpeed(10.0f),
	m_particleMinScale(1.0f),
	m_particleMaxScale(1.0f),
	m_lastEmissionPoint(0.0f, 0.0f, 0.0f),
	m_emissionLinksDirty(TRUE)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuEntity_Particle_Emitter_Line::HandleEvent_Dispose, this, _1), NULL);
}

CRuEntity_Particle_Emitter_Line::~CRuEntity_Particle_Emitter_Line()
{
}

IRuObject *CRuEntity_Particle_Emitter_Line::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuEntity_Particle_Emitter_Line();

	// Chain cloning call to parent class
	IRuEntity_Particle_Emitter::Clone(clonedObject);

	// Shortcut reference to the cloned entity
	CRuEntity_Particle_Emitter_Line *clonedPSys = static_cast<CRuEntity_Particle_Emitter_Line *>(clonedObject);

	// Copy runtime information
	clonedPSys->m_accumT = 0.0f;

	// Copy emitter settings
	clonedPSys->m_maxParticles = m_maxParticles;

	clonedPSys->m_emissionRate = m_emissionRate;

	clonedPSys->m_emitterPrerunTime = m_emitterPrerunTime;

	clonedPSys->m_particleMinExtinction = m_particleMinExtinction;
	clonedPSys->m_particleMaxExtinction = m_particleMaxExtinction;

	clonedPSys->m_particleMinSpeed = m_particleMinSpeed;
	clonedPSys->m_particleMaxSpeed = m_particleMaxSpeed;

	clonedPSys->m_particleMinScale = m_particleMinScale;
	clonedPSys->m_particleMaxScale = m_particleMaxScale;

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

BOOL CRuEntity_Particle_Emitter_Line::SerializeFrom(IRuStream *stream)
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

BOOL CRuEntity_Particle_Emitter_Line::SerializeTo(IRuStream *stream)
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

BOOL CRuEntity_Particle_Emitter_Line::ResetStates()
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

BOOL CRuEntity_Particle_Emitter_Line::Update(REAL elapsedTime)
{
	BOOL retVal = IRuEntity_Particle_Emitter::Update(elapsedTime);

	const CRuMatrix4x4 &worldTransform = this->GetWorldTransform();
	BOOL initialIteration = (m_t <= 0.0f) ? TRUE : FALSE;

	// Update emission links if necessary
	if(m_emissionLinksDirty)
	{
		CalculateEntityBounds();
		m_emissionLinksDirty = FALSE;
	}

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

		// Calculate generation points
		CRuVector3 genPt0 = m_emissionLinkPt0 ? m_emissionLinkPt0->GetTranslation(ruFRAMETYPE_WORLD) : this->GetTranslation(ruFRAMETYPE_WORLD);
		CRuVector3 genPt1 = m_emissionLinkPt1 ? m_emissionLinkPt1->GetTranslation(ruFRAMETYPE_WORLD) : genPt0 + CRuVector3(0.0f, 5.0f, 0.0f);

		// Convert generation points into local coordinates
		this->GetInverseWorldTransform().TransformPoint(genPt0);
		this->GetInverseWorldTransform().TransformPoint(genPt1);

		CRuVector3 genVec = genPt1 - genPt0;
		CRuVector3 genVecUnit = genVec;
		CRuVector3 devVec;

		CRuVector3 sideVec0(1.0f, 0.0f, 0.0f);
		CRuVector3 sideVec1(0.0f, 0.0f, 1.0f);

		if(fabs(DotProduct(sideVec0, genVec)) > (1.0f - ruEPSILON))
		{
			devVec = CrossProduct(genVecUnit, sideVec1);
			devVec.Normalize();
		}
		else
		{
			devVec = CrossProduct(genVecUnit, sideVec0);
			devVec.Normalize();
		}

		if(genVecUnit.Magnitude())
		{
			genVecUnit.Normalize();
		}

		// Update particles
		for(INT32 i = 0; i < m_particles.Count(); ++i)
		{
			CRuParticleDesc &particle = m_particles[i];

			if(particle.m_active)
			{
				// Update particle velocity and position
				particle.m_position += particle.m_velocity * m_updatePeriod;

				// Update particle movement direction vector
				CRuVector3 dirCont;
				dirCont += particle.m_velocity * m_updatePeriod;

				if(dirCont.SquaredMagnitude() > ruEPSILON)
				{
					particle.m_direction = dirCont;
					particle.m_direction.Normalize();
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
				REAL deviation = ((RuGlobalMTRand().GenerateFloat() - 0.5f) * 2.0f) * m_emissionPositionDeviation;

				REAL radialDeviationR = RuGlobalMTRand().GenerateFloat() * m_emissionRadialDeviation;
				REAL radialDeviationT = RuGlobalMTRand().GenerateFloat() * ruPI * 2.0f;

				CRuQuaternion quat;
				quat.FromAngleAxis(genVecUnit, radialDeviationT);

				CRuVector3 devVecOut = devVec * radialDeviationR;
				quat.TransformVector(devVecOut);

				particle.m_origin = (genPt0 + genVec * m_emissionPositionWeight) + (genVec * deviation) + devVecOut;

				particle.m_position = particle.m_origin;

				particle.m_orientation.Set(0.0f, 0.0f, 0.0f);

				particle.m_active = TRUE;
				particle.m_particleIteration = ++m_nextParticleIteration;

				particle.m_extinction = m_particleMinExtinction + RuGlobalMTRand().GenerateFloat() * extinctionRange;
				particle.m_age = 0.0f;

				particle.m_velocity = genVecUnit * (m_particleMinSpeed + (RuGlobalMTRand().GenerateFloat() * speedRange));

				particle.m_scale = RuGlobalMTRand().GenerateFloat() * (m_particleMaxScale - m_particleMinScale) + m_particleMinScale;

				particle.m_transientAccelerationTTL = 0.0f;
				particle.m_transientAcceleration.Set(0.0f, 0.0f, 0.0f);
			}
		}
	}

	return TRUE;
}

BOOL CRuEntity_Particle_Emitter_Line::QueryCollision(CRuCollisionQuery *colQuery)
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
			break;
	}

	// Restore original collision transform
	colQuery->m_worldTransform = originalWorldTransform;
	colQuery->m_invWorldTransform = originalInvWorldTransform;

	return TRUE;
}

void CRuEntity_Particle_Emitter_Line::CalculateEntityBounds()
{
	CRuVector3 genPt0 = m_emissionLinkPt0 ? m_emissionLinkPt0->GetTranslation(ruFRAMETYPE_WORLD) : this->GetTranslation(ruFRAMETYPE_WORLD);
	CRuVector3 genPt1 = m_emissionLinkPt1 ? m_emissionLinkPt1->GetTranslation(ruFRAMETYPE_WORLD) : genPt0 + CRuVector3(0.0f, 5.0f, 0.0f);

	REAL radius = (genPt0 - genPt1).Magnitude();

	// Multiply radius by some arbitrary factor (dangerous...)
	radius = radius * 1.5f;

	this->SetLocalEntityBounds(CRuSphere(CRuVector3(0.0f, 0.0f, 0.0f), radius));
}

BOOL CRuEntity_Particle_Emitter_Line::IsEmbeddedEmitter() const
{
	return FALSE;
}

CRuArrayList<CRuParticleDesc> *CRuEntity_Particle_Emitter_Line::GetParticleDescriptors()
{
	return &m_particles;
}

RuParticleFrameType CRuEntity_Particle_Emitter_Line::GetFrameType() const
{
	return ruPRTFRAMETYPE_LOCAL;
}

RuOrthogonalAxis CRuEntity_Particle_Emitter_Line::GetEmitterOrientation() const
{
	return ruAXIS_Z_NEGATIVE;
}

IRuEntity_Particle_Visualizer *CRuEntity_Particle_Emitter_Line::GetVisualizer()
{
	return m_visualizer;
}

BOOL CRuEntity_Particle_Emitter_Line::SetVisualizer(IRuEntity_Particle_Visualizer *visualizer)
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

BOOL CRuEntity_Particle_Emitter_Line::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_emissionLinkPt0)
	{
		m_emissionLinkPt0->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_emissionLinkPt0 = NULL;
	}

	if(m_emissionLinkPt1)
	{
		m_emissionLinkPt1->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_emissionLinkPt1 = NULL;
	}

	return TRUE;
}

INT32 CRuEntity_Particle_Emitter_Line::GetMaxParticles() const
{
	return m_maxParticles;
}

void CRuEntity_Particle_Emitter_Line::SetMaxParticles(INT32 maxParticles)
{
	m_maxParticles = maxParticles;
}

CRuEntity *CRuEntity_Particle_Emitter_Line::GetEmissionLinkPt0() const
{
	return m_emissionLinkPt0;
}

void CRuEntity_Particle_Emitter_Line::SetEmissionLinkPt0(CRuEntity *link)
{
	if(m_emissionLinkPt0)
	{
		m_emissionLinkPt0->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_emissionLinkPt0 = NULL;
	}

	if(link)
	{
		m_emissionLinkPt0 = link;
		m_emissionLinkPt0->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuEntity_Particle_Emitter_Line::HandleEvent_Dispose, this, _1), NULL);
	}

	// Mark emission links as dirty
	m_emissionLinksDirty = TRUE;
}

CRuEntity *CRuEntity_Particle_Emitter_Line::GetEmissionLinkPt1() const
{
	return m_emissionLinkPt1;
}

void CRuEntity_Particle_Emitter_Line::SetEmissionLinkPt1(CRuEntity *link)
{
	if(m_emissionLinkPt1)
	{
		m_emissionLinkPt1->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_emissionLinkPt1 = NULL;
	}

	if(link)
	{
		m_emissionLinkPt1 = link;
		m_emissionLinkPt1->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuEntity_Particle_Emitter_Line::HandleEvent_Dispose, this, _1), NULL);
	}

	// Mark emission links as dirty
	m_emissionLinksDirty = TRUE;
}

REAL CRuEntity_Particle_Emitter_Line::GetEmissionPositionWeight() const
{
	return m_emissionPositionWeight;
}

void CRuEntity_Particle_Emitter_Line::SetEmissionPositionWeight(REAL weight)
{
	m_emissionPositionWeight = weight;
}

REAL CRuEntity_Particle_Emitter_Line::GetEmissionPositionDeviation() const
{
	return m_emissionPositionDeviation;
}

void CRuEntity_Particle_Emitter_Line::SetEmissionPositionDeviation(REAL deviation)
{
	m_emissionPositionDeviation = deviation;
}

REAL CRuEntity_Particle_Emitter_Line::GetEmissionRadialDeviation() const
{
	return m_emissionRadialDeviation;
}

void CRuEntity_Particle_Emitter_Line::SetEmissionRadialDeviation(REAL deviation)
{
	m_emissionRadialDeviation = deviation;
}

REAL CRuEntity_Particle_Emitter_Line::GetEmissionRate() const
{
	return m_emissionRate;
}

void CRuEntity_Particle_Emitter_Line::SetEmissionRate(REAL emissionRate)
{
	m_emissionRate = emissionRate;
}

REAL CRuEntity_Particle_Emitter_Line::GetEmitterPrerunTime() const
{
	return m_emitterPrerunTime;
}

void CRuEntity_Particle_Emitter_Line::SetEmitterPrerunTime(REAL prerunTime)
{
	m_emitterPrerunTime = prerunTime;
}

REAL CRuEntity_Particle_Emitter_Line::GetMinExtinction() const
{
	return m_particleMinExtinction;
}

void CRuEntity_Particle_Emitter_Line::SetMinExtinction(REAL extinction)
{
	m_particleMinExtinction = extinction;
}

REAL CRuEntity_Particle_Emitter_Line::GetMaxExtinction() const
{
	return m_particleMaxExtinction;
}

void CRuEntity_Particle_Emitter_Line::SetMaxExtinction(REAL extinction)
{
	m_particleMaxExtinction = extinction;
}

REAL CRuEntity_Particle_Emitter_Line::GetMinSpeed() const
{
	return m_particleMinSpeed;
}

void CRuEntity_Particle_Emitter_Line::SetMinSpeed(REAL minSpeed)
{
	m_particleMinSpeed = minSpeed;
}

REAL CRuEntity_Particle_Emitter_Line::GetMaxSpeed() const
{
	return m_particleMaxSpeed;
}

void CRuEntity_Particle_Emitter_Line::SetMaxSpeed(REAL maxSpeed)
{
	m_particleMaxSpeed = maxSpeed;
}

REAL CRuEntity_Particle_Emitter_Line::GetMinScale() const
{
	return m_particleMinScale;
}

void CRuEntity_Particle_Emitter_Line::SetMinScale(REAL scale)
{
	m_particleMinScale = scale;
}

REAL CRuEntity_Particle_Emitter_Line::GetMaxScale() const
{
	return m_particleMaxScale;
}

void CRuEntity_Particle_Emitter_Line::SetMaxScale(REAL scale)
{
	m_particleMaxScale = scale;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
