#ifndef _RUCONTROLLER_UVFX_H_
#define _RUCONTROLLER_UVFX_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!

	@author John Tang
	@version 2007.11.08
*/
class CRuController_UVFX : public IRuEntity_KeyableController
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEntity*								m_targetEntity;									//!< Entity that the controller affects

	// Controller tracks
	CRuAnimTrack<REAL>						m_track_U;
	CRuAnimTrack<REAL>						m_track_V;

	// Runtime
	CRuMatrix4x4							m_uvTransform;

	CRuArrayList<IRuMaterial *>				m_materialList;

public:
											CRuController_UVFX();
	virtual									~CRuController_UVFX();

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
	BOOL									HandleEvent_MaterialDispose(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_UVFX, CRuController_UVFX)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
