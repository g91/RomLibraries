#ifndef _RUCONTROLLER_PARTICLE_EMITTER_ENVIRO_H_
#define _RUCONTROLLER_PARTICLE_EMITTER_ENVIRO_H_

#include "../../Scene/Base/RuEntityBase.h"

#include "../../Scene/Particle/RuEntity_Particle_Emitter_Enviro.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Environment emitter controller.

	@author John Tang
	@version 2007/12/21
*/
class CRuController_Particle_Emitter_Enviro : public IRuEntity_KeyableController
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Particle emitter setting
	REAL									m_emitterPrerunTime;

	REAL									m_particleOBKillTime;

	CRuAnimTrack<INT32>						m_track_MaxParticles;
	CRuAnimTrack<CRuVector3>				m_track_EmitterPosition;
	CRuAnimTrack<CRuVector3>				m_track_EmitterExtents;

	CRuAnimTrack<REAL>						m_track_EmissionRate;

	CRuAnimTrack<CRuVector3>				m_track_EmitDirection;
	CRuAnimTrack<REAL>						m_track_EmitDeviation;

	CRuAnimTrack<CRuVector3>				m_track_ParticleActiveExtents;

	CRuAnimTrack<REAL>						m_track_ParticleMinExtinction;
	CRuAnimTrack<REAL>						m_track_ParticleMaxExtinction;

	CRuAnimTrack<REAL>						m_track_ParticleMinSpeed;
	CRuAnimTrack<REAL>						m_track_ParticleMaxSpeed;

	CRuAnimTrack<REAL>						m_track_ParticleMinScale;
	CRuAnimTrack<REAL>						m_track_ParticleMaxScale;

	CRuAnimTrack<CRuVector3>				m_track_ParticleMinOrientation;
	CRuAnimTrack<CRuVector3>				m_track_ParticleMaxOrientation;

	CRuAnimTrack<CRuVector3>				m_track_Force0;
	CRuAnimTrack<CRuVector3>				m_track_Force1;

	CRuAnimTrack<REAL>						m_track_GustFrequency;
	CRuAnimTrack<REAL>						m_track_GustAffectProbability;
	CRuAnimTrack<REAL>						m_track_GustTTL;
	CRuAnimTrack<REAL>						m_track_GustMinStrength;
	CRuAnimTrack<REAL>						m_track_GustMaxStrength;
	CRuAnimTrack<REAL>						m_track_GustAscent;
	CRuAnimTrack<REAL>						m_track_GustDescent;

	// Run-time data
	CRuEntity_Particle_Emitter_Enviro*		m_particleEmitter;								//!< Target emitter

public:
											CRuController_Particle_Emitter_Enviro();
	virtual									~CRuController_Particle_Emitter_Enviro();

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);
	virtual BOOL							ResetStates();

	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *targetEntity);

	// IRuEntity_KeyableController implementation
	virtual INT32							GetNumTracks() const;
	virtual BOOL							GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);
	BOOL									HandleEvent_UpdateStep(RuEventArgs *eventArgs);

	REAL									GetEmitterPrerunTime() const;
	void									SetEmitterPrerunTime(REAL prerunTime);

	REAL									GetOBKillTime() const;
	void									SetOBKillTime(REAL obKillTime);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_Particle_Emitter_Enviro, CRuController_Particle_Emitter_Enviro)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
