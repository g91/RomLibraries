#ifndef _RUCONTROLLER_ENTITY_H_
#define _RUCONTROLLER_ENTITY_H_

#include "../../Scene/Base/RuEntityController.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuController_Entity : public IRuEntity_Controller
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuEntity_Container_Mutable*			m_targetEntity;									//!< Entity that the controller affects

	REAL									m_accumulatedT;

	BOOL									m_attached;
	BOOL									m_active;
	CRuEntity*								m_childEntity;									//!< Child entity

	// Spawning parameters
	char*									m_entityName;
	char*									m_motionName;
	REAL									m_fadeInTime;
	REAL									m_fadeOutTime;

public:
											CRuController_Entity();
	virtual									~CRuController_Entity();

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *targetEntity);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);

	const char*								GetSpawnedEntityName();
	const char*								GetSpawnedMotionName();
	REAL									GetFadeInTime();
	REAL									GetFadeOutTime();

	void									SetSpawnedEntityName(const char *entityName);
	void									SetSpawnedMotionName(const char *motionName);
	void									SetFadeInTime(REAL fadeInTime);
	void									SetFadeOutTime(REAL fadeOutTime);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_Entity, CRuController_Entity)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
