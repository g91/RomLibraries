/*!
	@file RuController_AnimCompensator.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUCONTROLLER_BONESYNC_H_
#define _RUCONTROLLER_BONESYNC_H_

#include "../../Scene/Base/RuEntityController.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	Animation compensation controller. This controller is designed to compensate
	for the difference between two interpolated keyframes.
	
	Typical usage scenario involves three interpolators. Two interpolators act as
	the source for different bones in the skeleton which are then combined into the
	final output interpolator. In a usage scenario like this, if animations are not
	designed for combination use, one or both of the interpolators will suffer from
	deviations from the intended animation.

	This compensator will reduce deviation from the intended animation at any
	specified bone by calculating the delta between the two source interpolators
	and then applying the delta modulated by a user-specified compensation strength
	to the final output interpolator.

	@author John Tang
	@version 2005/09/19
*/
class CRuController_AnimCompensator : public IRuEntity_Controller
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32									m_syncNodeID;							//!< ID of the node to synchronize
	INT32									m_syncNodeIndex;
	REAL									m_syncStrength;

	CRuFrameHierarchy*						m_hierarchy;							//!< Target hierarchy
	CRuAnimationInterpolator*				m_sourceInterpolator;					//!<
	CRuAnimationInterpolator*				m_targetInterpolator;					//!<

public:
											CRuController_AnimCompensator();
	virtual									~CRuController_AnimCompensator();

	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *targetEntity);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Update(RuEventArgs *eventArgs);

	BOOL									SetCompensatorSource(CRuAnimationInterpolator *source, CRuAnimationInterpolator *target, INT32 syncNodeID, REAL syncStrength);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
