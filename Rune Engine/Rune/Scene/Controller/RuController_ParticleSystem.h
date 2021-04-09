#ifndef _RUCONTROLLER_PARTICLESYSTEM_H_
#define _RUCONTROLLER_PARTICLESYSTEM_H_

#include "../../Scene/Base/RuEntityBase.h"

#include "../../Scene/Particle/RuEntity_Particle_Sprite.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!

	@author John Tang
	@version 2004/12/10
*/
class CRuController_ParticleSystem : public IRuEntity_Controller
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEntity_Container_Impl*				m_targetEntity;									//!< Entity that the controller affects

	// Particle system setting
	BOOL									m_systemAttached;
	BOOL									m_systemActive;
	CRuEntity*								m_particleSystemFrame;							//!< Particle system to attach (temp: change to a PSys interface later)
	CRuEntity*								m_particleSystem;								//!< Particle system to attach (temp: change to a PSys interface later)

	// Run-time settings
	REAL									m_emissionRate;

	// Settings
	char*									m_particleSystemName;

public:
											CRuController_ParticleSystem();
	virtual									~CRuController_ParticleSystem();

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *targetEntity);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);

	const char*								GetParticleSystemName();
	void									SetParticleSystem(CRuEntity *particleSystem);
	void									SetParticleSystem(const char *particleSystemName);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_ParticleSystem, CRuController_ParticleSystem)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
