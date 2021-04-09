#ifndef _RUCONTROLLER_HIERARCHY_H_
#define _RUCONTROLLER_HIERARCHY_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityContainer.h"
#include "../../Scene/Base/RuHierarchy_GR2.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	@author John Tang
	@version 2004/12/10
*/
class CRuController_Hierarchy : public IRuEntity_Controller
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEntity*								m_targetEntity;									//!< Target entity that owns the target interpolator
	CRuAnimationInterpolator*				m_targetInterpolator;							//!< Interpolator that the controller operates on

	BOOL									m_applied;										//!< Has the animation been applied to the target interpolator?
	CRuAnimation*							m_animation;									//!< Instanced animation data

	granny_control*							m_animation_Control;
	CRuAnimation_GR2*						m_animation_GR2;								//!< Instanced animation data

	// Controller settings
	BOOL									m_embedAnimation;								//!< Embed animation during serialization?
	char*									m_animationName;								//!< Animation name
	REAL									m_blendInTime;									//!< Blend-in time
	REAL									m_blendOutTime;									//!< Blend-out time
	REAL									m_baseTimeMultiplier;							//!< Base time multiplier

	// Runtime settings
	UINT32									m_motionChannel;
	REAL									m_timeMultiplier;

public:
											CRuController_Hierarchy();
	virtual									~CRuController_Hierarchy();

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							Update(REAL elapsedTime);

	virtual REAL							GetDuration();									//!< Override GetDuration to provide on-demand animation loading

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *target);

	const char*								GetAnimationName();
	REAL									GetBlendInTime();
	REAL									GetBlendOutTime();

	UINT32									GetMotionChannel();

	void									SetAnimation(const char *animationName);
	void									SetAnimation(CRuAnimation *animation);
	void									SetAnimation(CRuAnimation_GR2 *animation);
	void									SetBlendInTime(REAL blendInTime);
	void									SetBlendOutTime(REAL blendOutTime);

	REAL									GetBaseTimeMultiplier();
	void									SetBaseTimeMultiplier(REAL baseTimeMultiplier);

	void									SetMotionChannel(UINT32 motionChannel);
	void									SetTimeMultiplier(REAL timeMultiplier);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);

protected:
	BOOL									InstanceAnimation();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_Hierarchy, CRuController_Hierarchy)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
