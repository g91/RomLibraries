#ifndef _RUCONTROLLER_MORPH_H_
#define _RUCONTROLLER_MORPH_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	A controller that affects morphing animations. This controller is a simple morphing controller which
	morphs between all morph targets of a given mesh at a constant frame rate, with option to loop the
	morphing animation.

	@author John Tang
	@version 2004/12/10
*/
class CRuController_Morph : public IRuEntity_Controller
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEntity*								m_target;										// Entity that the controller affects

	INT32									m_morphTargetCount;

	// Settings
	BOOL									m_enableCurveMode;

	REAL									m_curve_t0;
	REAL									m_curve_t1;

	REAL									m_frameRate;
	INT32									m_startFrame;
	INT32									m_endFrame;

public:
											CRuController_Morph();
	virtual									~CRuController_Morph();

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *target);

	void									EnableCurveMode(BOOL enableCurveMode);
	void									SetCurveTimeRange(REAL t0, REAL t1);

	void									SetFrameRate(REAL frameRate);
	void									SetFrameRange(INT32 startFrame, INT32 endFrame);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);

protected:
	void									CalculateDuration();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_Morph, CRuController_Morph)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
