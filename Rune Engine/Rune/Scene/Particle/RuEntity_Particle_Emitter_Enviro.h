/*!
	@file RuEntity_Particle_Emitter_Enviro.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITY_PARTICLE_EMITTER_ENVIRO_H_
#define _RUENTITY_PARTICLE_EMITTER_ENVIRO_H_

#include "../../Scene/Particle/RuEntity_Particle_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuEntity_Particle_Emitter_Enviro : public IRuEntity_Particle_Emitter
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Runtime data
	INT32									m_nextParticleIteration;
	CRuVector3								m_currentEmitterPosition;			//!< Current emitter position
	REAL									m_updatePeriod;
	REAL									m_t;								//!< Emitter clock
	REAL									m_accumT;							//!< Accumulated t during which no particles were generated
	REAL									m_particleGenerationDebt;

	CRuArrayList<CRuParticleDesc>			m_particles;						//!< List of particles
	CRuStack<INT32>							m_inactiveParticles;				//!< Stack of indices to inactive particles

	IRuEntity_Particle_Visualizer*			m_visualizer;

	// Emitter settings
	INT32									m_maxParticles;						//!< Maximum particle count

	CRuVector3								m_emitterPosition;					//!< Original emitter position
	CRuVector3								m_emitterExtents;					//!< Emitter extents

	REAL									m_emissionRate;						//!< Number of particles to generate per second

	REAL									m_emitterPrerunTime;				//!< Number of seconds to pre-run the particle emitter at time 0

	// Particle settings
	REAL									m_particleOBKillTime;				//!< TTL for particles which stray out of emitter bounds

	CRuVector3								m_particleActiveExtents;			//!< Active extents for particles outside of which the particle gets killed

	REAL									m_particleMinExtinction;			//!< Particle extinction time (minimum particle life)
	REAL									m_particleMaxExtinction;			//!< Particle extinction time (maximum particle life)

	REAL									m_particleMinSpeed;					//!< Minimum particle speed
	REAL									m_particleMaxSpeed;					//!< Maximum particle speed

	REAL									m_particleMinScale;					//!< Minimum scale
	REAL									m_particleMaxScale;					//!< Maximum scale

	// Global forces
	CRuVector3								m_force0;							//!< Constant global force 0
	CRuVector3								m_force1;							//!< Constant global force 1

	// Gusts
	REAL									m_gustFrequency;					//!< Frequency at which gusts occur - in gusts per second
	REAL									m_gustAffectProbability;			//!< Probability that each gust would affect a particle
	REAL									m_gustTTL;							//!< Gust effect TTL
	REAL									m_gustMinStrength;					//!< Minimum gust strength
	REAL									m_gustMaxStrength;					//!< Maximum gust strength
	REAL									m_gustAscent;						//!< Maximum gust ascent angle in radians
	REAL									m_gustDescent;						//!< Maximum gust descent angle in radians

public:
											CRuEntity_Particle_Emitter_Enviro();
	virtual									~CRuEntity_Particle_Emitter_Enviro();

	// Base entity interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);
	virtual BOOL							ResetStates();

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// Local entity bounds calculator
	virtual void							CalculateEntityBounds();

	// Emitter versioning
	virtual BOOL							IsEmbeddedEmitter() const;

	// Emitter interface
	virtual CRuArrayList<CRuParticleDesc>*	GetParticleDescriptors();
	virtual RuParticleFrameType				GetFrameType() const;

	virtual RuOrthogonalAxis				GetEmitterOrientation() const;

	// Visualizer access
	virtual IRuEntity_Particle_Visualizer*	GetVisualizer();
	virtual BOOL							SetVisualizer(IRuEntity_Particle_Visualizer *visualizer);

	// Emitter settings
	INT32									GetMaxParticles() const;
	void									SetMaxParticles(INT32 maxParticles);

	const CRuVector3&						GetEmitterPosition() const;
	void									SetEmitterPosition(CRuVector3 &position);

	const CRuVector3&						GetEmitterExtents() const;
	void									SetEmitterExtents(CRuVector3 &extents);

	REAL									GetEmissionRate() const;
	void									SetEmissionRate(REAL emissionRate);

	REAL									GetEmitterPrerunTime() const;
	void									SetEmitterPrerunTime(REAL prerunTime);

	REAL									GetOBKillTime() const;
	void									SetOBKillTime(REAL obKillTime);

	const CRuVector3&						GetParticleActiveExtents() const;
	void									SetParticleActiveExtents(CRuVector3 &extents);

	REAL									GetMinExtinction() const;
	void									SetMinExtinction(REAL extinction);

	REAL									GetMaxExtinction() const;
	void									SetMaxExtinction(REAL extinction);

	REAL									GetMinSpeed() const;
	void									SetMinSpeed(REAL minSpeed);

	REAL									GetMaxSpeed() const;
	void									SetMaxSpeed(REAL maxSpeed);

	REAL									GetMinScale() const;
	void									SetMinScale(REAL scale);

	REAL									GetMaxScale() const;
	void									SetMaxScale(REAL scale);

	const CRuVector3&						GetForce0() const;
	void									SetForce0(CRuVector3 &force);

	const CRuVector3&						GetForce1() const;
	void									SetForce1(CRuVector3 &force);

	REAL									GetGustFrequency() const;
	void									SetGustFrequency(REAL gustFrequency);

	REAL									GetGustAffectProbability() const;
	void									SetGustAffectProbability(REAL gustAffectProbability);

	REAL									GetGustTTL() const;
	void									SetGustTTL(REAL gustTTL);

	REAL									GetGustMinStrength() const;
	void									SetGustMinStrength(REAL minStr);

	REAL									GetGustMaxStrength() const;
	void									SetGustMaxStrength(REAL maxStr);

	REAL									GetGustAscent() const;
	void									SetGustAscent(REAL gustAscent);

	REAL									GetGustDescent() const;
	void									SetGustDescent(REAL gustDescent);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_Particle_Emitter_Enviro, CRuEntity_Particle_Emitter_Enviro)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
