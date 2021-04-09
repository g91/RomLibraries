#ifndef _RUCONTROLLER_GLOBALALPHA_H_
#define _RUCONTROLLER_GLOBALALPHA_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Global alpha controller. This controller acts as a fader control on the diffuse
	alpha component of the target entity. 

	@author John Tang
	@version 2005/11/03
*/
class CRuController_GlobalAlpha : public IRuEntity_KeyableController
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Target
	CRuEntity*								m_targetEntity;									//!< Entity that the controller affects

	// Run-time data
	RuAlphaMode								m_currentAlphaMode;
	REAL									m_currentAlpha;
	BOOL									m_currentAlphaTestEnable;
	BYTE									m_currentAlphaTestValue;
	RuAlphaTestMode							m_currentAlphaTestMode;

	CRuArrayList<IRuMaterial *>				m_materialList;

	// Tracks
	BOOL									m_dataFormatIsOld;

	CRuAnimTrack<RuAlphaMode>				m_track_AlphaMode;								//!< Alpha mode track
	CRuAnimTrack<REAL>						m_track_Alpha;									//!< Alpha track
	CRuAnimTrack<BOOL>						m_track_AlphaTestEnable;
	CRuAnimTrack<BYTE>						m_track_AlphaTestValue;
	CRuAnimTrack<RuAlphaTestMode>			m_track_AlphaTestMode;

public:
											CRuController_GlobalAlpha();
	virtual									~CRuController_GlobalAlpha();

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							Update(REAL elapsedTime);

	virtual REAL							SetDuration(REAL duration);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *targetEntity);

	// IRuEntity_KeyableController implementation
	virtual INT32							GetNumTracks() const;
	virtual BOOL							GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);

	// Deprecated interface
	void									SetAlphaMode(RuAlphaMode alphaMode);
	void									SetStartAlpha(REAL alpha);
	void									SetEndAlpha(REAL alpha);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_GlobalAlpha, CRuController_GlobalAlpha)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
