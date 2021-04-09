#ifndef _RUCONTROLLER_PARTICLE_VISUALIZER_ENTITY_H_
#define _RUCONTROLLER_PARTICLE_VISUALIZER_ENTITY_H_

#include "../../Scene/Base/RuEntityBase.h"

#include "../../Scene/Particle/RuEntity_Particle_Visualizer_Entity.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Entity visualizer controller.

	@author John Tang
	@version 2005/11/30
*/
class CRuController_Particle_Visualizer_Entity : public IRuEntity_KeyableController
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Particle visualizer setting
	char*									m_entityName;									//!< Entity name
	BOOL									m_orientToDirectionOfTravel;

	CRuAnimTrack<char *>					m_track_MotionName;								//!< Motion name track
	CRuAnimTrack<CRuColor4>					m_track_Color;									//!< Color track
	CRuAnimTrack<REAL>						m_track_Alpha;									//!< Alpha track
	CRuAnimTrack<REAL>						m_track_Scale;									//!< Scale track

	// Run-time data
	CRuEntity_Particle_Visualizer_Entity*	m_particleVisualizer;							//!< Target visualizer

public:
											CRuController_Particle_Visualizer_Entity();
	virtual									~CRuController_Particle_Visualizer_Entity();

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *targetEntity);

	// IRuEntity_KeyableController implementation
	virtual INT32							GetNumTracks() const;
	virtual BOOL							GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);

	const char*								GetEntityName() const;
	void									SetEntityName(const char *entityName);

	BOOL									GetOrientToDirectionOfTravel() const;
	void									SetOrientToDirectionOfTravel(BOOL orientToDirectionOfTravel);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_Particle_Visualizer_Entity, CRuController_Particle_Visualizer_Entity)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
