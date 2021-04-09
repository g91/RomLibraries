#ifndef _RUCONTROLLER_PARTICLE_EMITTER_LINE_H_
#define _RUCONTROLLER_PARTICLE_EMITTER_LINE_H_

#include "../../Scene/Base/RuEntityBase.h"

#include "../../Scene/Particle/RuEntity_Particle_Emitter_Line.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Environment emitter controller.

	@author John Tang
	@version 2007/12/21
*/
class CRuController_Particle_Emitter_Line : public IRuEntity_KeyableController
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Particle emitter setting
	REAL									m_emitterPrerunTime;

	CRuAnimTrack<INT32>						m_track_MaxParticles;

	CRuAnimTrack<char *>					m_track_EmissionLinkPt0;
	CRuAnimTrack<char *>					m_track_EmissionLinkPt1;
	CRuAnimTrack<REAL>						m_track_EmissionPositionWeight;
	CRuAnimTrack<REAL>						m_track_EmissionPositionDeviation;
	CRuAnimTrack<REAL>						m_track_EmissionRadialDeviation;
	CRuAnimTrack<REAL>						m_track_EmissionRate;

	CRuAnimTrack<REAL>						m_track_ParticleMinExtinction;
	CRuAnimTrack<REAL>						m_track_ParticleMaxExtinction;

	CRuAnimTrack<REAL>						m_track_ParticleMinSpeed;
	CRuAnimTrack<REAL>						m_track_ParticleMaxSpeed;

	CRuAnimTrack<REAL>						m_track_ParticleMinScale;
	CRuAnimTrack<REAL>						m_track_ParticleMaxScale;

	CRuAnimTrack<CRuVector3>				m_track_ParticleMinOrientation;
	CRuAnimTrack<CRuVector3>				m_track_ParticleMaxOrientation;

	// Run-time data
	CRuEntity_Particle_Emitter_Line*		m_particleEmitter;								//!< Target emitter
	const char*								m_prevLinkPt0Name;
	const char*								m_prevLinkPt1Name;

public:
											CRuController_Particle_Emitter_Line();
	virtual									~CRuController_Particle_Emitter_Line();

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);
	virtual BOOL							ResetStates();

	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *targetEntity);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);
	BOOL									HandleEvent_UpdateStep(RuEventArgs *eventArgs);

	// IRuEntity_KeyableController implementation
	virtual INT32							GetNumTracks() const;
	virtual BOOL							GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut);

	REAL									GetEmitterPrerunTime() const;
	void									SetEmitterPrerunTime(REAL prerunTime);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_Particle_Emitter_Line, CRuController_Particle_Emitter_Line)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
