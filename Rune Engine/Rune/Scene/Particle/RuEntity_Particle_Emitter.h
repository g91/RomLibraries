/*!
	@file RuEntity_Particle_Model.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITY_PARTICLE_EMITTER_H_
#define _RUENTITY_PARTICLE_EMITTER_H_

#include "../../Scene/Particle/RuEntity_Particle_Base.h"
#include "../../Engine/Animation/RuAnimBase.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuEntity_Particle_Emitter : public IRuEntity_Particle_Emitter
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Runtime data
	INT32									m_nextParticleIteration;
	CRuVector3								m_currentEmitterPosition;			//!< Current emitter position
	CRuVector3								m_currentCollectorPosition;
	REAL									m_updatePeriod;
	REAL									m_t;								//!< Emitter clock
	REAL									m_accumT;							//!< Accumulated t during which no particles were generated
	REAL									m_particleGenerationDebt;
	CRuVector3								m_deviationAxis0;					//!< Deviation axis, calculated from m_emitDirection
	CRuVector3								m_deviationAxis1;					//!< Deviation axis, calculated from m_emitDirection

	CRuArrayList<CRuParticleDesc>			m_particles;						//!< List of particles
	CRuStack<INT32>							m_inactiveParticles;				//!< Stack of indices to inactive particles

	// Emitter settings
	INT32									m_maxParticles;
	REAL									m_prtSysRadius;						//!< Particle system radius
	RuParticleFrameType						m_frameType;						//!< Frame of reference mode
	RuParticleEmitterType					m_emitterType;						//!< Emitter type
	CRuVector3								m_emitterPosition;					//!< Original emitter position
	CRuVector3								m_emitterExtents;					//!< Emitter extents, used by volume emitters only
	REAL									m_emitterRadius;					//!< Emitter radius, used by sphere emitters only
	REAL									m_emitterMinTheta;					//!< Parameter specifying spherical coordinate constraints during particle generation
	REAL									m_emitterMaxTheta;					//!< Parameter specifying spherical coordinate constraints during particle generation
	REAL									m_emitterMinPhi;					//!< Parameter specifying spherical coordinate constraints during particle generation
	REAL									m_emitterMaxPhi;					//!< Parameter specifying spherical coordinate constraints during particle generation

	RuParticleEmitterMotionType				m_emitterMotionType;				//!< Emitter motion type
	REAL									m_emitterSpeed;						//!< Emitter speed
	REAL									m_emissionRate;						//!< Number of particles to generate per second

	REAL									m_emitterPrerunTime;				//!< Number of seconds to pre-run the particle emitter at time 0

	CRuVector3								m_emitDirection;					//!< Emission direction
	REAL									m_emitDeviation;					//!< Deviation degree in radians

	REAL									m_emitterTimeOffset;
	RuFrameType								m_emitterPathFrame;					//!< Relative frame of emitter path
	IRuObject*								m_emitterPathInterpolator;			//!< Emitter path interpolator
	RuOrthogonalAxis						m_emitterOrientation;				//!< Emitter orientation

	// Particle settings
	REAL									m_particleMinExtinction;			//!< Particle extinction time (minimum particle life)
	REAL									m_particleMaxExtinction;			//!< Particle extinction time (maximum particle life)

	REAL									m_particleMinSpeed;					//!< Minimum particle speed
	REAL									m_particleMaxSpeed;					//!< Maximum particle speed

	REAL									m_particleMinScale;					//!< Minimum scale
	REAL									m_particleMaxScale;					//!< Maximum scale

	CRuVector3								m_particleMinOrientation;			//!< Minimum orientation angle
	CRuVector3								m_particleMaxOrientation;			//!< Maximum orientation angle

	RuParticleFrameType						m_particlePathFrame;				//!< Relative frame of particle path
	BOOL									m_particlePathRandomMirror;			//!< Randomly mirror particle path?
	RuOrthogonalPlane						m_particlePathRandomMirrorPlane;	//!< Plane about which to mirror the path
	CRuVector3								m_particlePathRotationAxis;
	REAL									m_particlePathRotationDeviation;
	IRuObject*								m_particlePathInterpolator;			//!< Particle path interpolator

	// Global forces
	CRuVector3								m_force0;
	CRuVector3								m_force1;

	// Collector settings
	CRuVector3								m_collectorPosition;				//!< Collector position
	REAL									m_collectorStrength;				//!< Collector strength
	REAL									m_collectorFalloff0;
	REAL									m_collectorFalloff1;
	REAL									m_collectorFalloff2;

public:
											CRuEntity_Particle_Emitter();
	virtual									~CRuEntity_Particle_Emitter();

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

	// Emitter particle descriptor access
	virtual CRuArrayList<CRuParticleDesc>*	GetParticleDescriptors();

	// Visualizer access
	virtual IRuEntity_Particle_Visualizer*	GetVisualizer();
	virtual BOOL							SetVisualizer(IRuEntity_Particle_Visualizer *visualizer);

	// Emitter settings
	INT32									GetMaxParticles() const;
	void									SetMaxParticles(INT32 maxParticles);

	REAL									GetParticleSystemRadius() const;
	void									SetParticleSystemRadius(REAL radius);

	virtual RuParticleFrameType				GetFrameType() const;
	void									SetFrameType(RuParticleFrameType frameType);

	RuParticleEmitterType					GetEmitterType() const;
	void									SetEmitterType(RuParticleEmitterType emitterType);

	const CRuVector3&						GetEmitterPosition() const;
	void									SetEmitterPosition(CRuVector3 &position);

	const CRuVector3&						GetEmitterExtents() const;
	void									SetEmitterExtents(CRuVector3 &extents);

	REAL									GetEmitterRadius() const;
	void									SetEmitterRadius(REAL radius);

	RuParticleEmitterMotionType				GetEmitterMotionType() const;
	void									SetEmitterMotionType(RuParticleEmitterMotionType emitterMotionType);

	REAL									GetEmissionRate() const;
	void									SetEmissionRate(REAL emissionRate);

	REAL									GetEmitterPrerunTime() const;
	void									SetEmitterPrerunTime(REAL prerunTime);

	REAL									GetMinSpeed() const;
	void									SetMinSpeed(REAL minSpeed);

	REAL									GetMaxSpeed() const;
	void									SetMaxSpeed(REAL maxSpeed);

	const CRuVector3&						GetDirection() const;
	void									SetDirection(CRuVector3 &direction);

	REAL									GetDeviation() const;
	void									SetDeviation(REAL deviation);

	REAL									GetMinExtinction() const;
	void									SetMinExtinction(REAL extinction);

	REAL									GetMaxExtinction() const;
	void									SetMaxExtinction(REAL extinction);

	REAL									GetMinScale() const;
	void									SetMinScale(REAL scale);

	REAL									GetMaxScale() const;
	void									SetMaxScale(REAL scale);

	const CRuVector3&						GetMinOrientation() const;
	void									SetMinOrientation(CRuVector3 &orientation);

	const CRuVector3&						GetMaxOrientation() const;
	void									SetMaxOrientation(CRuVector3 &orientation);

	RuParticleFrameType						GetPathFrameType() const;
	void									SetPathFrameType(RuParticleFrameType frameType);

	BOOL									GetPathRandomMirror() const;
	void									SetPathRandomMirror(BOOL randomMirror);

	RuOrthogonalPlane						GetPathRandomMirrorPlane() const;
	void									SetPathRandomMirrorPlane(RuOrthogonalPlane mirrorPlane);

	const CRuVector3&						GetPathRotationAxis() const;
	void									SetPathRotationAxis(const CRuVector3 &rotationAxis);

	REAL									GetPathRotationDeviation() const;
	void									SetPathRotationDeviation(REAL rotationDeviation);

	IRuObject*								GetPathInterpolator();
	void									SetPathInterpolator(IRuObject *interpolator);

	RuFrameType								GetEmitterPathFrameType() const;
	void									SetEmitterPathFrameType(RuFrameType frameType);

	IRuObject*								GetEmitterPathInterpolator();
	void									SetEmitterPathInterpolator(IRuObject *interpolator);

	virtual RuOrthogonalAxis				GetEmitterOrientation() const;
	void									SetEmitterOrientation(RuOrthogonalAxis orientation);

	const CRuVector3&						GetForce0() const;
	void									SetForce0(CRuVector3 &force);

	const CRuVector3&						GetForce1() const;
	void									SetForce1(CRuVector3 &force);

	const CRuVector3&						GetCollectorPosition() const;
	void									SetCollectorPosition(CRuVector3 &position);

	REAL									GetCollectorStrength() const;
	void									SetCollectorStrength(REAL strength);

	REAL									GetCollectorFalloff0() const;
	void									SetCollectorFalloff0(REAL falloff);

	REAL									GetCollectorFalloff1() const;
	void									SetCollectorFalloff1(REAL falloff);

	REAL									GetCollectorFalloff2() const;
	void									SetCollectorFalloff2(REAL falloff);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_Particle_Emitter, CRuEntity_Particle_Emitter)

// ************************************************************************************************************************************************************

#pragma managed

#endif
