/*!
	@file RuEntity_Particle_Emitter.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Scene/Particle/RuEntity_Particle_Emitter.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Particle_Emitter, IRuEntity_Particle_Emitter)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Particle_Emitter, "CRuEntity_Particle_Emitter", "IRuEntity_Particle_Emitter")
ruCLASSGUID_DEFINE(CRuEntity_Particle_Emitter, 0xAAC47EFC, 0x3AB24516, 0x9036F7D6, 0xEA87EB97)

// ************************************************************************************************************************************************************

CRuEntity_Particle_Emitter::CRuEntity_Particle_Emitter()
:	m_nextParticleIteration(0),
	m_currentEmitterPosition(0.0f, 0.0f, 0.0f),
	m_currentCollectorPosition(0.0f, 0.0f, 0.0f),
	m_updatePeriod(0.030f),
	m_t(0.0f),
	m_accumT(0.0f),
	m_particleGenerationDebt(0.0f),
	m_deviationAxis0(1.0f, 0.0f, 0.0f),
	m_deviationAxis1(0.0f, 0.0f, 1.0f),
	m_particles(256),
	m_maxParticles(200),
	m_prtSysRadius(5.0f),
	m_frameType(ruPRTFRAMETYPE_LOCAL),
	m_emitterType(ruPRTEMITTERTYPE_POINT),
	m_emitterPosition(0.0f, 0.0f, 0.0f),
	m_emitterExtents(0.0f, 0.0f, 0.0f),
	m_emitterRadius(0.0f),
	m_emitterMinTheta(0.0f),
	m_emitterMaxTheta(0.0f),
	m_emitterMinPhi(0.0f),
	m_emitterMaxPhi(0.0f),
	m_emitterMotionType(ruPRTEMITTERMOTIONTYPE_STATIC),
	m_emitterSpeed(0.0f),
	m_emissionRate(1.0f),
	m_emitterPrerunTime(0.0f),
	m_emitDirection(0.0f, 1.0f, 0.0f),
	m_emitDeviation(0.0f),
	m_emitterTimeOffset(0.0f),
	m_emitterPathFrame(ruFRAMETYPE_LOCAL),
	m_emitterPathInterpolator(NULL),
	m_emitterOrientation(ruAXIS_Z_NEGATIVE),
	m_particleMinExtinction(1.0f),
	m_particleMaxExtinction(1.0f),
	m_particleMinSpeed(5.0f),
	m_particleMaxSpeed(10.0f),
	m_particleMinScale(1.0f),
	m_particleMaxScale(1.0f),
	m_particleMinOrientation(0.0f, 1.0f, 0.0f),
	m_particleMaxOrientation(0.0f, 1.0f, 0.0f),
	m_particlePathFrame(ruPRTFRAMETYPE_WORLD),
	m_particlePathRandomMirror(FALSE),
	m_particlePathRandomMirrorPlane(ruPLANE_XY),
	m_particlePathRotationAxis(0.0f, 1.0f, 0.0f),
	m_particlePathRotationDeviation(0.0f),
	m_particlePathInterpolator(NULL),
	m_force0(0.0f, 0.0f, 0.0f),
	m_force1(0.0f, 0.0f, 0.0f),
	m_collectorPosition(0.0f, 0.0f, 0.0f),
	m_collectorStrength(0.0f),
	m_collectorFalloff0(0.0f),
	m_collectorFalloff1(0.0f),
	m_collectorFalloff2(0.0f)
{
}

CRuEntity_Particle_Emitter::~CRuEntity_Particle_Emitter()
{
	ruSAFE_RELEASE(m_particlePathInterpolator);
	ruSAFE_RELEASE(m_emitterPathInterpolator);
}

IRuObject *CRuEntity_Particle_Emitter::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuEntity_Particle_Emitter();

	// Chain cloning call to parent class
	IRuEntity_Particle_Emitter::Clone(clonedObject);

	// Shortcut reference to the cloned entity
	CRuEntity_Particle_Emitter *clonedPSys = static_cast<CRuEntity_Particle_Emitter *>(clonedObject);

	// Copy runtime information
	clonedPSys->m_accumT = 0.0f;
	clonedPSys->m_deviationAxis0 = m_deviationAxis0;
	clonedPSys->m_deviationAxis1 = m_deviationAxis1;

	// Copy emitter settings
	clonedPSys->m_maxParticles = m_maxParticles;
	clonedPSys->m_frameType = m_frameType;

	clonedPSys->m_emitterType = m_emitterType;
	clonedPSys->m_emitterPosition = m_emitterPosition;
	clonedPSys->m_emitterExtents = m_emitterExtents;
	clonedPSys->m_emitterRadius = m_emitterRadius;

	clonedPSys->m_emitterMotionType = m_emitterMotionType;
	clonedPSys->m_emitterSpeed = m_emitterSpeed;
	clonedPSys->m_emissionRate = m_emissionRate;

	clonedPSys->m_emitDirection = m_emitDirection;
	clonedPSys->m_emitDeviation = m_emitDeviation;

	clonedPSys->m_particleMinExtinction = m_particleMinExtinction;
	clonedPSys->m_particleMaxExtinction = m_particleMaxExtinction;

	clonedPSys->m_particleMinSpeed = m_particleMinSpeed;
	clonedPSys->m_particleMaxSpeed = m_particleMaxSpeed;

	clonedPSys->m_force0 = m_force0;
	clonedPSys->m_force1 = m_force1;

	return clonedPSys;
}

BOOL CRuEntity_Particle_Emitter::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	stream->Read(&m_maxParticles, sizeof(INT32));

	stream->Read(&m_frameType, sizeof(INT32));

	if(m_frameType != ruPRTFRAMETYPE_WORLD && m_frameType != ruPRTFRAMETYPE_LOCAL)
	{
		m_frameType = ruPRTFRAMETYPE_WORLD;
	}

	stream->Read(&m_emitterType, sizeof(INT32));
	stream->Read(&m_emitterPosition.x, sizeof(REAL));
	stream->Read(&m_emitterPosition.y, sizeof(REAL));
	stream->Read(&m_emitterPosition.z, sizeof(REAL));
	stream->Read(&m_emitterExtents.x, sizeof(REAL));
	stream->Read(&m_emitterExtents.y, sizeof(REAL));
	stream->Read(&m_emitterExtents.z, sizeof(REAL));
	stream->Read(&m_emitterRadius, sizeof(REAL));

	stream->Read(&m_emitterMotionType, sizeof(INT32));
	stream->Read(&m_emitterSpeed, sizeof(REAL));
	stream->Read(&m_emissionRate, sizeof(REAL));

	stream->Read(&m_emitDirection.x, sizeof(REAL));
	stream->Read(&m_emitDirection.y, sizeof(REAL));
	stream->Read(&m_emitDirection.z, sizeof(REAL));
	stream->Read(&m_emitDeviation, sizeof(REAL));

	stream->Read(&m_particleMinExtinction, sizeof(REAL));
	stream->Read(&m_particleMaxExtinction, sizeof(REAL));

	stream->Read(&m_particleMinSpeed, sizeof(REAL));
	stream->Read(&m_particleMaxSpeed, sizeof(REAL));

	stream->Read(&m_force0.x, sizeof(REAL));
	stream->Read(&m_force0.y, sizeof(REAL));
	stream->Read(&m_force0.z, sizeof(REAL));
	stream->Read(&m_force1.x, sizeof(REAL));
	stream->Read(&m_force1.y, sizeof(REAL));
	stream->Read(&m_force1.z, sizeof(REAL));

	return TRUE;
}

BOOL CRuEntity_Particle_Emitter::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	stream->Write(&m_maxParticles, sizeof(INT32));

	stream->Write(&m_frameType, sizeof(INT32));

	stream->Write(&m_emitterType, sizeof(INT32));
	stream->Write(&m_emitterPosition.x, sizeof(REAL));
	stream->Write(&m_emitterPosition.y, sizeof(REAL));
	stream->Write(&m_emitterPosition.z, sizeof(REAL));
	stream->Write(&m_emitterExtents.x, sizeof(REAL));
	stream->Write(&m_emitterExtents.y, sizeof(REAL));
	stream->Write(&m_emitterExtents.z, sizeof(REAL));
	stream->Write(&m_emitterRadius, sizeof(REAL));

	stream->Write(&m_emitterMotionType, sizeof(INT32));
	stream->Write(&m_emitterSpeed, sizeof(REAL));
	stream->Write(&m_emissionRate, sizeof(REAL));

	stream->Write(&m_emitDirection.x, sizeof(REAL));
	stream->Write(&m_emitDirection.y, sizeof(REAL));
	stream->Write(&m_emitDirection.z, sizeof(REAL));
	stream->Write(&m_emitDeviation, sizeof(REAL));

	stream->Write(&m_particleMinExtinction, sizeof(REAL));
	stream->Write(&m_particleMaxExtinction, sizeof(REAL));

	stream->Write(&m_particleMinSpeed, sizeof(REAL));
	stream->Write(&m_particleMaxSpeed, sizeof(REAL));

	stream->Write(&m_force0.x, sizeof(REAL));
	stream->Write(&m_force0.y, sizeof(REAL));
	stream->Write(&m_force0.z, sizeof(REAL));
	stream->Write(&m_force1.x, sizeof(REAL));
	stream->Write(&m_force1.y, sizeof(REAL));
	stream->Write(&m_force1.z, sizeof(REAL));

	return TRUE;
}

BOOL CRuEntity_Particle_Emitter::ResetStates()
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

BOOL CRuEntity_Particle_Emitter::Update(REAL elapsedTime)
{
	BOOL retVal = IRuEntity_Particle_Emitter::Update(elapsedTime);

	REAL degToRad = ruPI / 180.0f;
	const CRuMatrix4x4 &worldTransform = this->GetWorldTransform();

	CRuVector3 m_accel = m_force0 + m_force1;

	BOOL initialIteration = (m_t <= 0.0f) ? TRUE : FALSE;

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

	// Determine time scale
	REAL timeScale = 0.0f;
	if(m_particlePathInterpolator)
	{
		if(m_particlePathInterpolator->GetType() == CRuAnimationInterpolator::Type())
		{
			timeScale = static_cast<CRuAnimationInterpolator *>(m_particlePathInterpolator)->GetAnimationLength();
		}
		else if(m_particlePathInterpolator->GetType() == CRuAnimationInterpolator_GR2::Type())
		{
			timeScale = static_cast<CRuAnimationInterpolator_GR2 *>(m_particlePathInterpolator)->GetAnimationLength();
		}
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

		// Update emitter and collector position
		if(m_emitterPathInterpolator)
		{
			if(m_emitterPathInterpolator->GetType() == CRuAnimationInterpolator::Type())
			{
				CRuAnimationInterpolator *interpolator = static_cast<CRuAnimationInterpolator *>(m_emitterPathInterpolator);

				interpolator->SetAnimationTime(m_t - m_emitterTimeOffset);

				CRuMatrix4x4 moo;
				moo.SetToBoneSRT(interpolator->GetKeyFrames()[0].m_scale, interpolator->GetKeyFrames()[0].m_rotation, interpolator->GetKeyFrames()[0].m_translation);

				// Modify emitter position by the interpolated path
				moo.TransformPoint(m_emitterPosition, m_currentEmitterPosition);

				// Modify collector position by the interpolated path
				moo.TransformPoint(m_collectorPosition, m_currentCollectorPosition);
			}
			else if(m_emitterPathInterpolator->GetType() == CRuAnimationInterpolator_GR2::Type())
			{
				CRuAnimationInterpolator_GR2 *interpolator = static_cast<CRuAnimationInterpolator_GR2 *>(m_emitterPathInterpolator);

				CRuVector3 pos;
				CRuQuaternion rot;
				CRuVector3 scale;

				interpolator->Sample(m_t - m_emitterTimeOffset, pos, rot, scale);

				m_currentEmitterPosition = m_emitterPosition + pos;
				m_currentCollectorPosition = m_collectorPosition + pos;
			}
		}
		else
		{
			m_currentEmitterPosition = m_emitterPosition;
			m_currentCollectorPosition = m_collectorPosition;
		}

		// Calculate effective emitter and collector position
		CRuVector3 effectiveEmitterPosition;
		CRuVector3 effectiveCollectorPosition;

		switch(m_frameType)
		{
			case ruPRTFRAMETYPE_WORLD:
				{
					// Update emitter position
					worldTransform.TransformPoint(m_currentEmitterPosition, effectiveEmitterPosition);

					// Update collector position
					worldTransform.TransformPoint(m_currentCollectorPosition, effectiveCollectorPosition);
				}

				break;

			case ruPRTFRAMETYPE_LOCAL:
				{
					effectiveEmitterPosition = m_currentEmitterPosition;
					effectiveCollectorPosition = m_currentCollectorPosition;
				}

				break;
		}

		CRuAnimationInterpolator *interpolator_Legacy = NULL;
		CRuAnimationInterpolator_GR2 *interpolator_GR2 = NULL;

		if(m_particlePathInterpolator)
		{
			if(m_particlePathInterpolator->GetType() == CRuAnimationInterpolator::Type())
			{
				interpolator_Legacy = static_cast<CRuAnimationInterpolator *>(m_particlePathInterpolator);
			}
			else if(m_particlePathInterpolator->GetType() == CRuAnimationInterpolator_GR2::Type())
			{
				interpolator_GR2 = static_cast<CRuAnimationInterpolator_GR2 *>(m_particlePathInterpolator);
			}
		}

		// Update particles
		for(INT32 i = 0; i < m_particles.Count(); ++i)
		{
			CRuParticleDesc &particle = m_particles[i];

			if(particle.m_active)
			{
				// If there is a path interpolator available, use it
				if(m_particlePathInterpolator)
				{
					REAL t = particle.m_age / particle.m_extinction;

					CRuVector3 newPosition;

					if(interpolator_Legacy)
					{
						interpolator_Legacy->SetAnimationTime(t * timeScale);

						newPosition = interpolator_Legacy->GetKeyFrames()[0].m_translation;
					}
					else if(interpolator_GR2)
					{
						CRuVector3 pos;
						CRuQuaternion rot;
						CRuVector3 scale;


						interpolator_GR2->Sample(t * timeScale, pos, rot, scale);

						newPosition = pos;
					}

					if(particle.m_mirrorPath)
					{
						CRuPlane4 mirrorPlane;

						switch(m_particlePathRandomMirrorPlane)
						{
							case ruPLANE_XY:
								{
									mirrorPlane.SetPlane(CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 0.0f, 0.f));
								}

								break;

							case ruPLANE_XZ:
								{
									mirrorPlane.SetPlane(CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 0.f));
								}

								break;

							case ruPLANE_YZ:
								{
									mirrorPlane.SetPlane(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 0.f));
								}

								break;
						}

						newPosition = mirrorPlane.GetClosestPointOnPlane(newPosition) - mirrorPlane.GetNormal() * mirrorPlane.GetDistance(newPosition);
					}

					if(particle.m_pathRotation > 0.0f)
					{
						CRuQuaternion pathRotation;
						pathRotation.FromAngleAxis(m_particlePathRotationAxis, particle.m_pathRotation);

						pathRotation.TransformVector(newPosition);
					}

					// Offset position to the origin of the particle
					newPosition = newPosition + particle.m_origin;

					// Setup particle position and direction
					CRuVector3 direction = newPosition - particle.m_position;
					if(direction.SquaredMagnitude() > ruEPSILON)
					{
						particle.m_direction = direction;
						particle.m_direction.Normalize();
					}

					particle.m_position.Set(newPosition);
				}
				else
				{
					CRuVector3 dirCont;

					// Calculate collector influence if strength is non-zero
					if(m_collectorStrength != 0.0f)
					{
						CRuVector3 collectorDirection = effectiveCollectorPosition - particle.m_position;
						REAL distanceToCollector = collectorDirection.Magnitude();

						if(distanceToCollector > ruEPSILON)
						{
							REAL attenuation = 1.0f;
							REAL attenuationDenominator = (m_collectorFalloff0 + m_collectorFalloff1 * distanceToCollector + m_collectorFalloff2 * distanceToCollector * distanceToCollector);

							if(attenuationDenominator > ruEPSILON)
							{
								attenuation = 1.0f / attenuationDenominator;
							}

							collectorDirection.Normalize();

							dirCont = collectorDirection * (m_collectorStrength * attenuation * m_updatePeriod);

							particle.m_position += dirCont;
		//					particle.m_velocity += dirCont;
						}
					}

					particle.m_velocity += m_accel * m_updatePeriod;
					particle.m_position += particle.m_velocity * m_updatePeriod;

					dirCont += particle.m_velocity * m_updatePeriod;

					if(dirCont.SquaredMagnitude() > ruEPSILON)
					{
						particle.m_direction = dirCont;
						particle.m_direction.Normalize();
					}

		/*
					if(particle.m_velocity.SquaredMagnitude() > ruEPSILON)
					{
						particle.m_direction = particle.m_velocity;
						particle.m_direction.Normalize();
					}
		*/
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

		// Move the emitter based on the motion type
		switch(m_emitterMotionType)
		{
			default:
			case ruPRTEMITTERMOTIONTYPE_STATIC:
				{
				}

				break;
		}

		// Add to the particle generation debt
		m_particleGenerationDebt += m_updatePeriod * m_emissionRate;

		INT32 numActiveParticles = m_particles.Count() - m_inactiveParticles.Count();
		INT32 particlesToGenerate = min(static_cast<INT32>(m_particleGenerationDebt), max(0, m_maxParticles - numActiveParticles));

		if(particlesToGenerate > 0)
		{
			CRuQuaternion quat0;
			CRuQuaternion quat1;
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

				// Generate particle position and direction based on the emitter type
				BOOL directionGenerated = FALSE;
				switch(m_emitterType)
				{
					case ruPRTEMITTERTYPE_POINT:
						{
							particle.m_origin.Set(effectiveEmitterPosition);
						}

						break;

					case ruPRTEMITTERTYPE_VOLUME:
						{
							particle.m_origin.Set(	effectiveEmitterPosition.x + (RuGlobalMTRand().GenerateFloat() - 0.5f) * 2.0f * m_emitterExtents.x,
														effectiveEmitterPosition.y + (RuGlobalMTRand().GenerateFloat() - 0.5f) * 2.0f * m_emitterExtents.y,
														effectiveEmitterPosition.z + (RuGlobalMTRand().GenerateFloat() - 0.5f) * 2.0f * m_emitterExtents.z);
						}

						break;

					case ruPRTEMITTERTYPE_SPHERE:
						{
							particle.m_origin.Set(effectiveEmitterPosition);
						}

						break;

					case ruPRTEMITTERTYPE_SHELL:
						{
							REAL z = (RuGlobalMTRand().GenerateFloat() * 2.0f - 1.0f) * m_emitterRadius;
							REAL t = RuGlobalMTRand().GenerateFloat() * 2.0f * ruPI;
							REAL moo = m_emitterRadius * m_emitterRadius - z * z;
							REAL r = RuSqrt(moo);
//							REAL r = sqrt(m_emitterRadius * m_emitterRadius - z * z);
							REAL x = r * cos(t);
							REAL y = r * sin(t);

							particle.m_origin.Set(effectiveEmitterPosition.x + x, effectiveEmitterPosition.y + y, effectiveEmitterPosition.z + z);
						}

						break;

					case ruPRTEMITTERTYPE_DISC:
						{
							REAL t = RuGlobalMTRand().GenerateFloat() * 2.0f * ruPI;
							REAL r = RuGlobalMTRand().GenerateFloat() * m_emitterRadius;

							CRuVector3 direction;
							CRuVector3 position;

							switch(m_emitterOrientation)
							{
								case ruAXIS_X_POSITIVE: direction.Set(0.0f, sin(t), cos(t)); position.Set(0.0f, r * direction.m_y, r * direction.m_z); break;
								case ruAXIS_Y_POSITIVE: direction.Set(sin(t), 0.0f, cos(t)); position.Set(r * direction.m_x, 0.0f, r * direction.m_z); break;
								case ruAXIS_Z_POSITIVE: direction.Set(sin(t), cos(t), 0.0f); position.Set(r * direction.m_x, r * direction.m_y, 0.0f); break;
								case ruAXIS_X_NEGATIVE: direction.Set(0.0f, sin(t), cos(t)); position.Set(0.0f, r * direction.m_y, r * direction.m_z); break;
								case ruAXIS_Y_NEGATIVE: direction.Set(sin(t), 0.0f, cos(t)); position.Set(r * direction.m_x, 0.0f, r * direction.m_z); break;
								case ruAXIS_Z_NEGATIVE: direction.Set(sin(t), cos(t), 0.0f); position.Set(r * direction.m_x, r * direction.m_y, 0.0f); break;
							}

							particle.m_origin = position + effectiveEmitterPosition;
							particle.m_direction = direction;

							directionGenerated = TRUE;
						}

						break;

					case ruPRTEMITTERTYPE_RING:
						{
							REAL t = RuGlobalMTRand().GenerateFloat() * 2.0f * ruPI;
							REAL r = m_emitterRadius;

							CRuVector3 direction;
							CRuVector3 position;

							switch(m_emitterOrientation)
							{
								case ruAXIS_X_POSITIVE: direction.Set(0.0f, sin(t), cos(t)); position.Set(0.0f, r * direction.m_y, r * direction.m_z); break;
								case ruAXIS_Y_POSITIVE: direction.Set(sin(t), 0.0f, cos(t)); position.Set(r * direction.m_x, 0.0f, r * direction.m_z); break;
								case ruAXIS_Z_POSITIVE: direction.Set(sin(t), cos(t), 0.0f); position.Set(r * direction.m_x, r * direction.m_y, 0.0f); break;
								case ruAXIS_X_NEGATIVE: direction.Set(0.0f, sin(t), cos(t)); position.Set(0.0f, r * direction.m_y, r * direction.m_z); break;
								case ruAXIS_Y_NEGATIVE: direction.Set(sin(t), 0.0f, cos(t)); position.Set(r * direction.m_x, 0.0f, r * direction.m_z); break;
								case ruAXIS_Z_NEGATIVE: direction.Set(sin(t), cos(t), 0.0f); position.Set(r * direction.m_x, r * direction.m_y, 0.0f); break;
							}

							particle.m_origin = position + effectiveEmitterPosition;
							particle.m_direction = direction;

							directionGenerated = TRUE;
						}

						break;
				}

				// Calculate the particle's emission direction if necessary
				if(directionGenerated == FALSE)
				{
					// Calculate normalized velocity
					CRuVector3 direction(m_emitDirection);
					quat0.FromAngleAxis(m_deviationAxis0, (RuGlobalMTRand().GenerateFloat() - 0.5f) * m_emitDeviation * 2.0f);
					quat1.FromAngleAxis(m_deviationAxis1, (RuGlobalMTRand().GenerateFloat() - 0.5f) * m_emitDeviation * 2.0f);
					quat0 = quat0 * quat1;
					quat0.TransformVector(direction);

					particle.m_direction = direction;
				}

				// If there is a path interpolator available, use it (deprecated, should animate emitter to achieve effects instead)
				if(m_particlePathInterpolator)
				{
					// Other path parameters
					if(m_particlePathRandomMirror)
					{
						if(RuGlobalMTRand().GenerateFloat() > 0.5f)
						{
							particle.m_mirrorPath = TRUE;
						}
						else
						{
							particle.m_mirrorPath = FALSE;
						}
					}
					else
					{
						particle.m_mirrorPath = FALSE;
					}

					if(m_particlePathRotationDeviation > 0.0f)
					{
						particle.m_pathRotation = (RuGlobalMTRand().GenerateFloat() * m_particlePathRotationDeviation) * degToRad;
					}
					else
					{
						particle.m_pathRotation = 0.0f;
					}

					CRuVector3 newPosition;

					if(m_particlePathInterpolator->GetType() == CRuAnimationInterpolator::Type())
					{
						CRuAnimationInterpolator *interpolator = static_cast<CRuAnimationInterpolator *>(m_particlePathInterpolator);

						interpolator->SetAnimationTime(0.0f);
						newPosition = interpolator->GetKeyFrames()[0].m_translation;
//						particle.m_position.Set(interpolator->GetKeyFrames()[0].m_translation + particle.m_origin);
					}
					else if(m_particlePathInterpolator->GetType() == CRuAnimationInterpolator_GR2::Type())
					{
						CRuAnimationInterpolator_GR2 *interpolator = static_cast<CRuAnimationInterpolator_GR2 *>(m_particlePathInterpolator);

						CRuVector3 pos;
						CRuQuaternion rot;
						CRuVector3 scale;

						interpolator->Sample(0.0f, pos, rot, scale);
						newPosition = pos;
//						particle.m_position.Set(pos + particle.m_origin);
					}


					if(particle.m_mirrorPath)
					{
						CRuPlane4 mirrorPlane;

						switch(m_particlePathRandomMirrorPlane)
						{
						case ruPLANE_XY:
							{
								mirrorPlane.SetPlane(CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 0.0f, 0.f));
							}

							break;

						case ruPLANE_XZ:
							{
								mirrorPlane.SetPlane(CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 0.f));
							}

							break;

						case ruPLANE_YZ:
							{
								mirrorPlane.SetPlane(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 0.f));
							}

							break;
						}

						newPosition = mirrorPlane.GetClosestPointOnPlane(newPosition) - mirrorPlane.GetNormal() * mirrorPlane.GetDistance(newPosition);
					}

					if(particle.m_pathRotation > 0.0f)
					{
						CRuQuaternion pathRotation;
						pathRotation.FromAngleAxis(m_particlePathRotationAxis, particle.m_pathRotation);

						pathRotation.TransformVector(newPosition);
					}

					particle.m_position = particle.m_origin + newPosition;
				}
				else
				{
					particle.m_position = particle.m_origin;
				}

				// Calculate particle's initial orientation
				for(INT32 j = 0; j < 3; ++j)
					particle.m_orientation[j] = (RuGlobalMTRand().GenerateFloat() * (m_particleMaxOrientation[j] - m_particleMinOrientation[j]) + m_particleMinOrientation[j]) * (2.0f * ruPI / 360.0f);

				particle.m_active = TRUE;
				particle.m_particleIteration = ++m_nextParticleIteration;

				particle.m_extinction = m_particleMinExtinction + RuGlobalMTRand().GenerateFloat() * extinctionRange;
				particle.m_age = 0.0f;

				particle.m_velocity = particle.m_direction * (m_particleMinSpeed + (RuGlobalMTRand().GenerateFloat() * speedRange));

				particle.m_scale = RuGlobalMTRand().GenerateFloat() * (m_particleMaxScale - m_particleMinScale) + m_particleMinScale;
			}
		}
	}
/*
	// Set the emitter's local bounds (FIX ME! This is incorrect)
	switch(m_emitterType)
	{
		case ruPRTEMITTERTYPE_POINT:
			{
				this->SetLocalEntityBounds(CRuSphere(m_currentEmitterPosition, m_emitterRadius));
			}

			break;

		case ruPRTEMITTERTYPE_VOLUME:
			{
				this->SetLocalEntityBounds(CRuSphere(m_currentEmitterPosition, max(max(m_emitterExtents.x, m_emitterExtents.y), m_emitterExtents.z)));
			}

			break;

		case ruPRTEMITTERTYPE_SPHERE:
			{
				this->SetLocalEntityBounds(CRuSphere(m_currentEmitterPosition, m_emitterRadius));
			}

			break;

		case ruPRTEMITTERTYPE_SHELL:
			{
				this->SetLocalEntityBounds(CRuSphere(m_currentEmitterPosition, m_emitterRadius));
			}

			break;

		case ruPRTEMITTERTYPE_DISC:
			{
				this->SetLocalEntityBounds(CRuSphere(m_currentEmitterPosition, m_emitterRadius));
			}

			break;

		case ruPRTEMITTERTYPE_RING:
			{
				this->SetLocalEntityBounds(CRuSphere(m_currentEmitterPosition, m_emitterRadius));
			}

			break;
	}
*/
	return retVal;
}

BOOL CRuEntity_Particle_Emitter::QueryCollision(CRuCollisionQuery *colQuery)
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

void CRuEntity_Particle_Emitter::CalculateEntityBounds()
{
	CRuVector3 origin(0.0f, 0.0f, 0.0f);

	this->SetLocalEntityBounds(CRuSphere(origin, m_prtSysRadius));
}

BOOL CRuEntity_Particle_Emitter::IsEmbeddedEmitter() const
{
	return TRUE;
}

CRuArrayList<CRuParticleDesc> *CRuEntity_Particle_Emitter::GetParticleDescriptors()
{
	return &m_particles;
}

IRuEntity_Particle_Visualizer *CRuEntity_Particle_Emitter::GetVisualizer()
{
	return NULL;
}

BOOL CRuEntity_Particle_Emitter::SetVisualizer(IRuEntity_Particle_Visualizer *visualizer)
{
	return FALSE;
}

INT32 CRuEntity_Particle_Emitter::GetMaxParticles() const
{
	return m_maxParticles;
}

void CRuEntity_Particle_Emitter::SetMaxParticles(INT32 maxParticles)
{
	m_maxParticles = maxParticles;
}

REAL CRuEntity_Particle_Emitter::GetParticleSystemRadius() const
{
	return m_prtSysRadius;
}

void CRuEntity_Particle_Emitter::SetParticleSystemRadius(REAL radius)
{
	if(m_prtSysRadius != radius)
	{
		m_prtSysRadius = radius;

		// Force an update of the local entity bounds
		this->MarkLocalEntityBoundsDirty();
	}
}

RuParticleFrameType CRuEntity_Particle_Emitter::GetFrameType() const
{
	return m_frameType;
}

void CRuEntity_Particle_Emitter::SetFrameType(RuParticleFrameType frameType)
{
	m_frameType = frameType;
}

RuParticleEmitterType CRuEntity_Particle_Emitter::GetEmitterType() const
{
	return m_emitterType;
}

void CRuEntity_Particle_Emitter::SetEmitterType(RuParticleEmitterType emitterType)
{
	m_emitterType = emitterType;
}

const CRuVector3 &CRuEntity_Particle_Emitter::GetEmitterPosition() const
{
	return m_emitterPosition;
}

void CRuEntity_Particle_Emitter::SetEmitterPosition(CRuVector3 &position)
{
	m_emitterPosition = position;
}

const CRuVector3 &CRuEntity_Particle_Emitter::GetEmitterExtents() const
{
	return m_emitterExtents;
}

void CRuEntity_Particle_Emitter::SetEmitterExtents(CRuVector3 &extents)
{
	m_emitterExtents = extents;
}

REAL CRuEntity_Particle_Emitter::GetEmitterRadius() const
{
	return m_emitterRadius;
}

void CRuEntity_Particle_Emitter::SetEmitterRadius(REAL radius)
{
	m_emitterRadius = radius;
}

RuParticleEmitterMotionType CRuEntity_Particle_Emitter::GetEmitterMotionType() const
{
	return m_emitterMotionType;
}

void CRuEntity_Particle_Emitter::SetEmitterMotionType(RuParticleEmitterMotionType emitterMotionType)
{
	m_emitterMotionType = emitterMotionType;
}

REAL CRuEntity_Particle_Emitter::GetEmissionRate() const
{
	return m_emissionRate;
}

void CRuEntity_Particle_Emitter::SetEmissionRate(REAL emissionRate)
{
	m_emissionRate = emissionRate;
}

REAL CRuEntity_Particle_Emitter::GetEmitterPrerunTime() const
{
	return m_emitterPrerunTime;
}

void CRuEntity_Particle_Emitter::SetEmitterPrerunTime(REAL prerunTime)
{
	m_emitterPrerunTime = prerunTime;
}

REAL CRuEntity_Particle_Emitter::GetMinSpeed() const
{
	return m_particleMinSpeed;
}

void CRuEntity_Particle_Emitter::SetMinSpeed(REAL minSpeed)
{
	m_particleMinSpeed = minSpeed;
}

REAL CRuEntity_Particle_Emitter::GetMaxSpeed() const
{
	return m_particleMaxSpeed;
}

void CRuEntity_Particle_Emitter::SetMaxSpeed(REAL maxSpeed)
{
	m_particleMaxSpeed = maxSpeed;
}

const CRuVector3 &CRuEntity_Particle_Emitter::GetDirection() const
{
	return m_emitDirection;
}

void CRuEntity_Particle_Emitter::SetDirection(CRuVector3 &direction)
{
	m_emitDirection = direction;

	if(m_emitDirection.Magnitude() <= ruEPSILON)
	{
		m_emitDirection.Set(0.0f, 1.0f, 0.0f);
		m_deviationAxis0.Set(1.0f, 0.0f, 0.0f);
		m_deviationAxis1.Set(0.0f, 0.0f, 1.0f);
	}
	else
	{
		m_emitDirection.Normalize();
		m_deviationAxis0.Set(1.0f, 0.0f, 0.0f);
		m_deviationAxis1 = CrossProduct(m_emitDirection, m_deviationAxis0);
		if(m_deviationAxis1.Magnitude() < ruEPSILON)
		{
			m_deviationAxis0.Set(0.0f, 1.0f, 0.0f);
			m_deviationAxis1 = CrossProduct(m_emitDirection, m_deviationAxis0);
		}

		m_deviationAxis0 = CrossProduct(m_emitDirection, m_deviationAxis1);

		m_deviationAxis0.Normalize();
		m_deviationAxis1.Normalize();
	}
}

REAL CRuEntity_Particle_Emitter::GetDeviation() const
{
	return m_emitDeviation;
}

void CRuEntity_Particle_Emitter::SetDeviation(REAL deviation)
{
	m_emitDeviation = deviation;
}

REAL CRuEntity_Particle_Emitter::GetMinExtinction() const
{
	return m_particleMinExtinction;
}

void CRuEntity_Particle_Emitter::SetMinExtinction(REAL extinction)
{
	m_particleMinExtinction = extinction;
}

REAL CRuEntity_Particle_Emitter::GetMaxExtinction() const
{
	return m_particleMaxExtinction;
}

void CRuEntity_Particle_Emitter::SetMaxExtinction(REAL extinction)
{
	m_particleMaxExtinction = extinction;
}

REAL CRuEntity_Particle_Emitter::GetMinScale() const
{
	return m_particleMinScale;
}

void CRuEntity_Particle_Emitter::SetMinScale(REAL scale)
{
	m_particleMinScale = scale;
}

REAL CRuEntity_Particle_Emitter::GetMaxScale() const
{
	return m_particleMaxScale;
}

void CRuEntity_Particle_Emitter::SetMaxScale(REAL scale)
{
	m_particleMaxScale = scale;
}

const CRuVector3 &CRuEntity_Particle_Emitter::GetMinOrientation() const
{
	return m_particleMinOrientation;
}

void CRuEntity_Particle_Emitter::SetMinOrientation(CRuVector3 &orientation)
{
	m_particleMinOrientation = orientation;
}

const CRuVector3 &CRuEntity_Particle_Emitter::GetMaxOrientation() const
{
	return m_particleMaxOrientation;
}

void CRuEntity_Particle_Emitter::SetMaxOrientation(CRuVector3 &orientation)
{
	m_particleMaxOrientation = orientation;
}

RuParticleFrameType CRuEntity_Particle_Emitter::GetPathFrameType() const
{
	return m_particlePathFrame;
}

void CRuEntity_Particle_Emitter::SetPathFrameType(RuParticleFrameType frameType)
{
	m_particlePathFrame = frameType;
}

BOOL CRuEntity_Particle_Emitter::GetPathRandomMirror() const
{
	return m_particlePathRandomMirror;
}

void CRuEntity_Particle_Emitter::SetPathRandomMirror(BOOL randomMirror)
{
	m_particlePathRandomMirror = randomMirror;
}

RuOrthogonalPlane CRuEntity_Particle_Emitter::GetPathRandomMirrorPlane() const
{
	return m_particlePathRandomMirrorPlane;
}

void CRuEntity_Particle_Emitter::SetPathRandomMirrorPlane(RuOrthogonalPlane mirrorPlane)
{
	m_particlePathRandomMirrorPlane = mirrorPlane;
}

const CRuVector3 &CRuEntity_Particle_Emitter::GetPathRotationAxis() const
{
	return m_particlePathRotationAxis;
}

void CRuEntity_Particle_Emitter::SetPathRotationAxis(const CRuVector3 &rotationAxis)
{
	if(rotationAxis.Magnitude() > ruEPSILON)
	{
		m_particlePathRotationAxis = rotationAxis;
		m_particlePathRotationAxis.Normalize();
	}
}

REAL CRuEntity_Particle_Emitter::GetPathRotationDeviation() const
{
	return m_particlePathRotationDeviation;
}

void CRuEntity_Particle_Emitter::SetPathRotationDeviation(REAL rotationDeviation)
{
	m_particlePathRotationDeviation = rotationDeviation;
}

IRuObject *CRuEntity_Particle_Emitter::GetPathInterpolator()
{
	return m_particlePathInterpolator;
}

void CRuEntity_Particle_Emitter::SetPathInterpolator(IRuObject *interpolator)
{
	if(interpolator)
	{
		interpolator->AddRef();
		ruSAFE_RELEASE(m_particlePathInterpolator);
		m_particlePathInterpolator = interpolator;
	}
	else
	{
		ruSAFE_RELEASE(m_particlePathInterpolator);
	}
}

RuFrameType CRuEntity_Particle_Emitter::GetEmitterPathFrameType() const
{
	return m_emitterPathFrame;
}

void CRuEntity_Particle_Emitter::SetEmitterPathFrameType(RuFrameType frameType)
{
	m_emitterPathFrame = frameType;
}

IRuObject *CRuEntity_Particle_Emitter::GetEmitterPathInterpolator()
{
	return m_emitterPathInterpolator;
}

void CRuEntity_Particle_Emitter::SetEmitterPathInterpolator(IRuObject *interpolator)
{
	if(interpolator)
	{
		interpolator->AddRef();
		ruSAFE_RELEASE(m_emitterPathInterpolator);
		m_emitterPathInterpolator = interpolator;
	}
	else
	{
		ruSAFE_RELEASE(m_emitterPathInterpolator);
	}

	m_emitterTimeOffset = m_t;
}

RuOrthogonalAxis CRuEntity_Particle_Emitter::GetEmitterOrientation() const
{
	return m_emitterOrientation;
}

void CRuEntity_Particle_Emitter::SetEmitterOrientation(RuOrthogonalAxis orientation)
{
	m_emitterOrientation = orientation;
}

const CRuVector3 &CRuEntity_Particle_Emitter::GetForce0() const
{
	return m_force0;
}

void CRuEntity_Particle_Emitter::SetForce0(CRuVector3 &force)
{
	m_force0 = force;
}

const CRuVector3 &CRuEntity_Particle_Emitter::GetForce1() const
{
	return m_force1;
}

void CRuEntity_Particle_Emitter::SetForce1(CRuVector3 &force)
{
	m_force1 = force;
}

const CRuVector3 &CRuEntity_Particle_Emitter::GetCollectorPosition() const
{
	return m_collectorPosition;
}

void CRuEntity_Particle_Emitter::SetCollectorPosition(CRuVector3 &position)
{
	m_collectorPosition = position;
}

REAL CRuEntity_Particle_Emitter::GetCollectorStrength() const
{
	return m_collectorStrength;
}

void CRuEntity_Particle_Emitter::SetCollectorStrength(REAL strength)
{
	m_collectorStrength = strength;
}

REAL CRuEntity_Particle_Emitter::GetCollectorFalloff0() const
{
	return m_collectorFalloff0;
}

void CRuEntity_Particle_Emitter::SetCollectorFalloff0(REAL falloff)
{
	m_collectorFalloff0 = falloff;
}

REAL CRuEntity_Particle_Emitter::GetCollectorFalloff1() const
{
	return m_collectorFalloff1;
}

void CRuEntity_Particle_Emitter::SetCollectorFalloff1(REAL falloff)
{
	m_collectorFalloff1 = falloff;
}

REAL CRuEntity_Particle_Emitter::GetCollectorFalloff2() const
{
	return m_collectorFalloff2;
}

void CRuEntity_Particle_Emitter::SetCollectorFalloff2(REAL falloff)
{
	m_collectorFalloff2 = falloff;
}

// ************************************************************************************************************************************************************

#pragma managed
