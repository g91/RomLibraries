#ifndef _RUCONTROLLER_PARTICLE_EMITTER_H_
#define _RUCONTROLLER_PARTICLE_EMITTER_H_

#include "../../Scene/Base/RuEntityBase.h"

#include "../../Scene/Particle/RuEntity_Particle_Emitter.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Particle emitter controller.

	@author John Tang
	@version 2005/12/08
*/
class CRuController_Particle_Emitter : public IRuEntity_KeyableController
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Particle emitter setting
	REAL									m_prtSysRadius;
	RuParticleFrameType						m_frameType;
	RuParticleFrameType						m_particlePathFrameType;
	BOOL									m_particlePathRandomMirror;			//!< Randomly mirror particle path?
	RuOrthogonalPlane						m_particlePathRandomMirrorPlane;	//!< Plane about which to mirror the path
	CRuVector3								m_particlePathRotationAxis;
	REAL									m_particlePathRotationDeviation;
	char*									m_particlePathName;
	REAL									m_particlePathLoopDelay;

	RuFrameType								m_emitterPathFrameType;
	char*									m_emitterPathName;
	REAL									m_emitterPathLoopDelay;
	REAL									m_emitterPrerunTime;
	RuOrthogonalAxis						m_emitterOrientation;				//!< Emitter orientation

	CRuAnimTrack<INT32>						m_track_MaxParticles;
	CRuAnimTrack<RuParticleEmitterType>		m_track_EmitterType;
	CRuAnimTrack<CRuVector3>				m_track_EmitterPosition;
	CRuAnimTrack<CRuVector3>				m_track_EmitterExtents;
	CRuAnimTrack<REAL>						m_track_EmitterRadius;
	CRuAnimTrack<REAL>						m_track_EmitterMinTheta;
	CRuAnimTrack<REAL>						m_track_EmitterMaxTheta;
	CRuAnimTrack<REAL>						m_track_EmitterMinPhi;
	CRuAnimTrack<REAL>						m_track_EmitterMaxPhi;

	CRuAnimTrack<REAL>						m_track_EmissionRate;

	CRuAnimTrack<CRuVector3>				m_track_EmitDirection;
	CRuAnimTrack<REAL>						m_track_EmitDeviation;

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

	CRuAnimTrack<CRuVector3>				m_track_CollectorPosition;
	CRuAnimTrack<REAL>						m_track_CollectorStrength;
	CRuAnimTrack<REAL>						m_track_CollectorFalloff0;
	CRuAnimTrack<REAL>						m_track_CollectorFalloff1;
	CRuAnimTrack<REAL>						m_track_CollectorFalloff2;

	// Run-time data
	CRuEntity_Particle_Emitter*				m_particleEmitter;								//!< Target emitter

public:
											CRuController_Particle_Emitter();
	virtual									~CRuController_Particle_Emitter();

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

	REAL									GetParticleSystemRadius() const;
	void									SetParticleSystemRadius(REAL radius);

	RuParticleFrameType						GetFrameType() const;
	void									SetFrameType(RuParticleFrameType frameType);

	RuParticleFrameType						GetParticlePathFrameType() const;
	void									SetParticlePathFrameType(RuParticleFrameType frameType);

	BOOL									GetParticlePathRandomMirror() const;
	void									SetParticlePathRandomMirror(BOOL randomMirror);

	RuOrthogonalPlane						GetParticlePathRandomMirrorPlane() const;
	void									SetParticlePathRandomMirrorPlane(RuOrthogonalPlane mirrorPlane);

	const CRuVector3&						GetParticlePathRotationAxis() const;
	void									SetParticlePathRotationAxis(const CRuVector3 &rotationAxis);

	REAL									GetParticlePathRotationDeviation() const;
	void									SetParticlePathRotationDeviation(REAL rotationDeviation);

	const char*								GetParticlePathName() const;
	void									SetParticlePathName(const char *particlePathName);

	REAL									GetParticlePathLoopDelay() const;
	void									SetParticlePathLoopDelay(REAL loopDelay);

	RuFrameType								GetEmitterPathFrameType() const;
	void									SetEmitterPathFrameType(RuFrameType frameType);

	const char*								GetEmitterPathName() const;
	void									SetEmitterPathName(const char *emitterPathName);

	REAL									GetEmitterPathLoopDelay() const;
	void									SetEmitterPathLoopDelay(REAL loopDelay);

	virtual RuOrthogonalAxis				GetEmitterOrientation() const;
	void									SetEmitterOrientation(RuOrthogonalAxis orientation);

	REAL									GetEmitterPrerunTime() const;
	void									SetEmitterPrerunTime(REAL prerunTime);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_Particle_Emitter, CRuController_Particle_Emitter)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
