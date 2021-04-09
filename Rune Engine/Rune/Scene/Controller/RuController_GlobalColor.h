#ifndef _RUCONTROLLER_GLOBALCOLOR_H_
#define _RUCONTROLLER_GLOBALCOLOR_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Global color controller. This controller acts as a fader control on the diffuse
	color component of the target entity. 

	@author John Tang
	@version 2005/11/03
*/
class CRuController_GlobalColor : public IRuEntity_KeyableController
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEntity*								m_targetEntity;									//!< Entity that the controller affects
	CRuColor4								m_currentColor;

	// Controller tracks
	CRuAnimTrack<CRuColor4>					m_track_Color;									//!< Color track

public:
											CRuController_GlobalColor();
	virtual									~CRuController_GlobalColor();

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

	// Deprecated interface
	void									SetStartColor(const CRuColor4 &color);
	void									SetEndColor(const CRuColor4 &color);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_GlobalColor, CRuController_GlobalColor)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
