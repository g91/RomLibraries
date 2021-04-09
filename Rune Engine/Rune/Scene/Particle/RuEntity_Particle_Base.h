/*!
	@file RuEntity_Particle_Base.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITY_PARTICLE_BASE_H_
#define _RUENTITY_PARTICLE_BASE_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class IRuEntity_Particle_Visualizer;

// ************************************************************************************************************************************************************

class CRuParticleDesc
{
public:
	BOOL								m_active;							//!< Is the particle active?
	INT32								m_particleIteration;				//!< Particle iteration number

	CRuVector3							m_origin;							//!< Initial position
	CRuVector3							m_position;							//!< Current position
	CRuVector3							m_velocity;							//!< Current velocity
	CRuVector3							m_direction;						//!< Current direction (used for orientation only)
	CRuVector3							m_orientation;						//!< Initial orientation of the	particle, defined as Euler rotations about each axis
	REAL								m_extinction;						//!< Particle extinction value
	REAL								m_age;								//!< Particle age
	REAL								m_scale;							//!< Base particle scale

	BOOL								m_mirrorPath;						//!< Mirror the particle path?
	REAL								m_pathRotation;						//!< Path rotation value

	REAL								m_transientAccelerationTTL;			//!< Transient acceleration TTL
	CRuVector3							m_transientAcceleration;			//!< Transient acceleration vector
};

// ************************************************************************************************************************************************************

class IRuEntity_Particle_Emitter : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
											IRuEntity_Particle_Emitter();
	virtual									~IRuEntity_Particle_Emitter();

	// Emitter versioning
	virtual BOOL							IsEmbeddedEmitter() const = 0;

	// Emitter interface
	virtual CRuArrayList<CRuParticleDesc>*	GetParticleDescriptors() = 0;
	virtual RuParticleFrameType				GetFrameType() const = 0;

	virtual RuOrthogonalAxis				GetEmitterOrientation() const = 0;

	// Visualizer access
	virtual IRuEntity_Particle_Visualizer*	GetVisualizer() = 0;
	virtual BOOL							SetVisualizer(IRuEntity_Particle_Visualizer *visualizer) = 0;
};

// ************************************************************************************************************************************************************

class IRuEntity_Particle_Visualizer : public IRuEntity_Container
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
											IRuEntity_Particle_Visualizer();
	virtual									~IRuEntity_Particle_Visualizer();

	// Visualizer interface
	virtual IRuEntity_Particle_Emitter*		GetEmitter() = 0;
	virtual BOOL							SetEmitter(IRuEntity_Particle_Emitter *emitter) = 0;
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
