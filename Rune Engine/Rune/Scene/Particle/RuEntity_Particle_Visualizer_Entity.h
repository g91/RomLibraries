/*!
	@file RuEntity_Particle_Visualizer_Entity.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITY_PARTICLE_VISUALIZER_ENTITY_H_
#define _RUENTITY_PARTICLE_VISUALIZER_ENTITY_H_

#include "../../Scene/Particle/RuEntity_Particle_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuEntity_Particle_Visualizer_Entity : public IRuEntity_Particle_Visualizer
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct ParticleDesc
	{
		INT32								m_particleIteration;
		CRuEntity*							m_entity;

		ParticleDesc()
		:	m_particleIteration(0),
			m_entity(NULL)
		{
		}
	};

	// Particle visualization settings
	char*									m_entityName;						//!< Entity name
	BOOL									m_orientToDirectionOfTravel;

	CRuAnimTrack<char *>					m_track_MotionName;					//!< Motion name track
	CRuAnimTrack<CRuColor4>					m_track_Color;						//!< Color track
	CRuAnimTrack<REAL>						m_track_Alpha;						//!< Alpha track
	CRuAnimTrack<REAL>						m_track_Scale;						//!< Scale track

	// Particle emitter
	IRuEntity_Particle_Emitter*				m_emitter;							//!< Particle emitter

	// Runtime data
	CRuEntity*								m_particleEntity;					//!< Representation entity to use for each particle
	CRuArrayList<ParticleDesc>				m_entities;							//!< List of allocated particle entities

	REAL									m_timeSinceLastUpdate;
	REAL									m_updateInterval;

	INT32									m_iterationCount;

public:
											CRuEntity_Particle_Visualizer_Entity();
	virtual									~CRuEntity_Particle_Visualizer_Entity();

	// Base entity interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);
	virtual BOOL							ResetStates();

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// Visualizer interface
	virtual IRuEntity_Particle_Emitter*		GetEmitter();
	virtual BOOL							SetEmitter(IRuEntity_Particle_Emitter *emitter);

	// Track access
	CRuAnimTrack<char *>&					GetTrack_MotionName();
	CRuAnimTrack<CRuColor4>&				GetTrack_Color();
	CRuAnimTrack<REAL>&						GetTrack_Alpha();
	CRuAnimTrack<REAL>&						GetTrack_Scale();

	const char*								GetEntityName() const;
	void									SetEntityName(const char *entityName);

	BOOL									GetOrientToDirectionOfTravel() const;
	void									SetOrientToDirectionOfTravel(BOOL orientToDirectionOfTravel);

	// Deprecated stuff
	const char*								GetMotionName() const;
	void									SetMotionName(const char *motionName);

	DWORD									GetStartColor() const;
	void									SetStartColor(DWORD color);

	DWORD									GetEndColor() const;
	void									SetEndColor(DWORD color);

	REAL									GetStartAlpha() const;
	void									SetStartAlpha(REAL alpha);

	REAL									GetEndAlpha() const;
	void									SetEndAlpha(REAL alpha);

	REAL									GetStartScale() const;
	void									SetStartScale(REAL scale);

	REAL									GetEndScale() const;
	void									SetEndScale(REAL scale);

protected:
	void									FlushParticleStack();
	void									ReloadParticleEntity();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_Particle_Visualizer_Entity, CRuEntity_Particle_Visualizer_Entity)

// ************************************************************************************************************************************************************

#pragma managed

#endif
