/*!
	@file RuEntity_Particle_Emitter_Line.h

	Copyright (c) 2004-2008 Runewaker Entertainment. All rights reserved.
*/

#ifndef _RUENTITY_PARTICLE_EMITTER_LINE_H_
#define _RUENTITY_PARTICLE_EMITTER_LINE_H_

#include "../../Scene/Particle/RuEntity_Particle_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuEntity_Particle_Emitter_Line : public IRuEntity_Particle_Emitter
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Runtime data
	INT32									m_nextParticleIteration;
	REAL									m_updatePeriod;
	REAL									m_t;								//!< Emitter clock
	REAL									m_accumT;							//!< Accumulated t during which no particles were generated
	REAL									m_particleGenerationDebt;

	CRuArrayList<CRuParticleDesc>			m_particles;						//!< List of particles
	CRuStack<INT32>							m_inactiveParticles;				//!< Stack of indices to inactive particles

	IRuEntity_Particle_Visualizer*			m_visualizer;

	// Emitter settings
	INT32									m_maxParticles;						//!< Maximum particle count

	CRuEntity*								m_emissionLinkPt0;					//!< Emission link point 0
	CRuEntity*								m_emissionLinkPt1;					//!< Emission link point 1
	REAL									m_emissionPositionWeight;			//!< 0 = all emissions at Pt0, 1 = all emissions at Pt1
	REAL									m_emissionPositionDeviation;		//!< 0 = no deviation, 1 = deviation along full length of the two emission links
	REAL									m_emissionRadialDeviation;
	REAL									m_emissionRate;						//!< Number of particles to generate per second

	REAL									m_emitterPrerunTime;				//!< Number of seconds to pre-run the particle emitter at time 0

	// Particle settings
	REAL									m_particleMinExtinction;			//!< Particle extinction time (minimum particle life)
	REAL									m_particleMaxExtinction;			//!< Particle extinction time (maximum particle life)

	REAL									m_particleMinSpeed;					//!< Minimum particle speed
	REAL									m_particleMaxSpeed;					//!< Maximum particle speed

	REAL									m_particleMinScale;					//!< Minimum scale
	REAL									m_particleMaxScale;					//!< Maximum scale

	// Runtime
	CRuVector3								m_lastEmissionPoint;
	BOOL									m_emissionLinksDirty;

public:
											CRuEntity_Particle_Emitter_Line();
	virtual									~CRuEntity_Particle_Emitter_Line();

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

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);

	// Emitter settings
	INT32									GetMaxParticles() const;
	void									SetMaxParticles(INT32 maxParticles);

	CRuEntity*								GetEmissionLinkPt0() const;
	void									SetEmissionLinkPt0(CRuEntity *link);

	CRuEntity*								GetEmissionLinkPt1() const;
	void									SetEmissionLinkPt1(CRuEntity *link);

	REAL									GetEmissionPositionWeight() const;
	void									SetEmissionPositionWeight(REAL weight);

	REAL									GetEmissionPositionDeviation() const;
	void									SetEmissionPositionDeviation(REAL deviation);

	REAL									GetEmissionRadialDeviation() const;
	void									SetEmissionRadialDeviation(REAL deviation);

	REAL									GetEmissionRate() const;
	void									SetEmissionRate(REAL emissionRate);

	REAL									GetEmitterPrerunTime() const;
	void									SetEmitterPrerunTime(REAL prerunTime);

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

};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_Particle_Emitter_Line, CRuEntity_Particle_Emitter_Line)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
