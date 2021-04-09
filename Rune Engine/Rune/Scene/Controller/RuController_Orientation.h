/*!
	@file RuController_Orientation.h

	Copyright (c) 2004-2007 All rights reserved
*/

#ifndef _RUCONTROLLER_ORIENTATION_H_
#define _RUCONTROLLER_ORIENTATION_H_

#include "../../Scene/Base/RuEntityController.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuController_Orientation : public IRuEntity_Controller
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEntity*								m_targetEntity;							//!< Target entity whose orientation is being controlled
	CRuEntity*								m_referenceEntity;						//!< Reference entity

public:
											CRuController_Orientation();
	virtual									~CRuController_Orientation();

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *target);

	CRuEntity*								GetReferenceEntity();
	BOOL									SetReferenceEntity(CRuEntity *referenceEntity);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
