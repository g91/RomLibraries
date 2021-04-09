#ifndef _RUCONTROLLER_FRAME_H_
#define _RUCONTROLLER_FRAME_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Engine/Animation/RuAnimBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!

	@author John Tang
	@version 2006.04.28
*/
class CRuController_Frame : public IRuEntity_KeyableController
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEntity*								m_targetEntity;									//!< Entity that the controller affects

	// Interpolators
	CRuAnimationInterpolator*				m_animInterpolator0;							//!< Currently playing animation
	CRuAnimationInterpolator*				m_animInterpolator1;							//!< Animation to transition to

	// Runtime data
	const char*								m_currentAnimation;
	REAL									m_blendTimeRemaining;

	// Settings
	REAL									m_loopBlendTime;
	REAL									m_blendTime;

	// Controller tracks
	CRuAnimTrack<char *>					m_track_Animation;

public:
											CRuController_Frame();
	virtual									~CRuController_Frame();

	// Properties
	REAL									GetLoopBlendTime();
	void									SetLoopBlendTime(REAL loopBlendTime);

	REAL									GetBlendTime();
	void									SetBlendTime(REAL blendTime);

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

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_Frame, CRuController_Frame)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
