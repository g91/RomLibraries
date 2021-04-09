#ifndef _RUCONTROLLER_VISIBILITYCHAIN_H_
#define _RUCONTROLLER_VISIBILITYCHAIN_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!

	@author John Tang
	@version 2007.12.25
*/
class CRuController_VisibilityChain : public IRuEntity_Controller
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEntity*								m_targetEntity;									//!< Entity that the controller affects
	CRuEntity*								m_chainSource;									//!< Source of the visibility chain

public:
											CRuController_VisibilityChain();
	virtual									~CRuController_VisibilityChain();

	// Properties

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *targetEntity);

	CRuEntity*								GetChainSource();
	BOOL									SetChainSource(CRuEntity *chainSource);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_VisibilityChain, CRuController_VisibilityChain)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
