#ifndef _RUCONTROLLER_MOTION_H_
#define _RUCONTROLLER_MOTION_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!

	@author John Tang
	@version 2007.11.08
*/
class CRuController_Motion : public IRuEntity_KeyableController
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEntity*								m_targetEntity;									//!< Entity that the controller affects

	// Controller tracks
	CRuAnimTrack<char *>					m_track_Motion;

public:
											CRuController_Motion();
	virtual									~CRuController_Motion();

	// Properties

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
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_Motion, CRuController_Motion)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
