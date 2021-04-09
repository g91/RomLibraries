#ifndef _RUCONTROLLER_EVENTTRIGGER_H_
#define _RUCONTROLLER_EVENTTRIGGER_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	@author John Tang
	@version 2004/12/10
*/
class CRuController_EventTrigger : public IRuEntity_Controller
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEntity*								m_targetEntity;									//!< Entity that the controller affects

	REAL									m_accumulatedT;

	// Settings
	INT32									m_eventID;										//!< Event ID to trigger
	INT32									m_eventParameter0;								//!< Event parameter
	INT32									m_eventParameter1;								//!< Event parameter
	char*									m_eventString;									//!< Event string
	REAL									m_eventDuration;								//!< Event duration

public:
											CRuController_EventTrigger();
	virtual									~CRuController_EventTrigger();

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							Update(REAL elapsedTime);

	virtual REAL							SetDuration(REAL duration);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *targetEntity);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);

	INT32									GetEventID() const;
	INT32									GetEventParameter0() const;
	INT32									GetEventParameter1() const;
	const char*								GetEventString() const;
	REAL									GetEventDuration() const;

	void									SetEventID(INT32 eventID);
	void									SetEventParameter0(INT32 eventParameter0);
	void									SetEventParameter1(INT32 eventParameter1);
	void									SetEventString(const char *eventString);
	void									SetEventDuration(REAL eventDuration);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_EventTrigger, CRuController_EventTrigger)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
