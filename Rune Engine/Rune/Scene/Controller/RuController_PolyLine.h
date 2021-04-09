#ifndef _RUCONTROLLER_POLYLINE_H_
#define _RUCONTROLLER_POLYLINE_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	@author John Tang
	@version 2007.07.08
*/
class CRuController_PolyLine : public IRuEntity_KeyableController
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEntity*								m_targetEntity;									//!< Entity that the controller affects

	char*									m_prevTargetName;

	// Controller tracks
	CRuAnimTrack<char *>					m_track_Target;
	CRuAnimTrack<REAL>						m_track_SegmentLength;
	CRuAnimTrack<REAL>						m_track_SegmentWidth;
	CRuAnimTrack<REAL>						m_track_Deviation;
	CRuAnimTrack<REAL>						m_track_UVRatio;

public:
											CRuController_PolyLine();
	virtual									~CRuController_PolyLine();

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

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_PolyLine, CRuController_PolyLine)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
