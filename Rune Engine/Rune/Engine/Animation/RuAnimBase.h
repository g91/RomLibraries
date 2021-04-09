#ifndef _RUANIMBASE_H_
#define _RUANIMBASE_H_

#include "../../Core/Memory/RuMemoryManager.h"

// Aurora system core
#include "../../Engine/Base/RuCore.h"

#include "../../ExtLink/RuExtLink_Granny.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuAnimKeyFrame
{
public:
	float					m_time;							// Time of this key frame
	CRuVector3				m_scale;						// Non-uniform scale
	CRuQuaternion			m_rotation;						// Rotation at the key frame
	CRuVector3				m_translation;					// Translation at the key frame
};

// ************************************************************************************************************************************************************

class CRuAnimation : public IRuReferenceCounted
{
	ruHEAP_DECLARE

protected:
	float						m_duration;					//!< Length of this animation
	INT32						m_numFrames;				//!< Number of key frames
	CRuAnimKeyFrame*			m_keyFrames;				//!< Animation key frames

public:
	INT32						m_numUpdateNodes;
	INT32*						m_updateNodes;

//public:
//	INT32						m_startNodeIndex;
//	INT32						m_endNodeIndex;

public:
								CRuAnimation(INT32 numFrames, float duration);
	virtual						~CRuAnimation();

	// Interpolation
	BOOL						InterpolateKeyFrames(float t, float loopBlendDuration, float weight, UINT32 activeGroupIDs, UINT32 *nodeGroupIDs, CRuAnimKeyFrame *keyFramesOut);
	BOOL						InterpolateKeyFrames(INT32 framesToInterpolate, float t, float loopBlendDuration, float weight, UINT32 activeGroupIDs, UINT32 *nodeGroupIDs, CRuAnimKeyFrame *keyFramesOut);

	// Property access
	float						GetDuration();
	INT32						GetNumKeyFrames();
	CRuAnimKeyFrame*			GetKeyFrames();

	INT32						GetNumNodes();

	friend CRuAnimation*		LoadRuAnimation(IRuStream *stream);
	friend BOOL					SaveRuAnimation(IRuStream *stream, CRuAnimation* animation);
};

extern CRuAnimation*			LoadRuAnimation(IRuStream *stream);
extern BOOL						SaveRuAnimation(IRuStream *stream, CRuAnimation* animation);

// ************************************************************************************************************************************************************

class CRuFrameAnimationInterpolator : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:

public:
											CRuFrameAnimationInterpolator();
	virtual									~CRuFrameAnimationInterpolator();

	// IRuObject serialization
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);
};

// ************************************************************************************************************************************************************

class CRuAnimationInterpolator : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct AnimationInfo
	{
		float						m_startTime;				//!< Time at which the animation should begin playing
		float						m_blendInTime;				//!< Length of time during which the animation should be blended in
		float						m_blendOutTime;				//!< Length of time during which the animation should be blended in
		float						m_localTime;				//!< Local time of the animation
		float						m_weight;					//!< Current weighting of the animation
		float						m_weightFlux;				//!< Direction in which the weight is drifting

		CRuAnimation*				m_animation;				//!< Animation data
		float						m_loopBlendDuration;		//!< Time gap from the last frame to frame 0 when looping
		UINT32						m_activeGroupIDs;			//!< Active group IDs bit field

		// Status data
		BOOL						m_inFrame0;
		BOOL						m_inFrame2;
	};

	CRuArrayList<AnimationInfo>		m_animations;				//!< Currently active animations

	float							m_time;						//!< Global animation time
	INT32							m_numNodes;					//!< Number of animated nodes
	UINT32*							m_groupIDs;					//!< Group ID bit field array
	CRuAnimKeyFrame*				m_keyFrames;				//!< Interpolated key frames

	// Animation set transitions
	BOOL							m_transitionStarted;
	float							m_transitionTime;
	float							m_transitionDuration;
	CRuAnimKeyFrame*				m_transitionKeyFrames;		//!< Interpolated key frames

	// Channel data
	UINT32							m_channel;

public:
									CRuAnimationInterpolator(INT32 numNodes);
	virtual							~CRuAnimationInterpolator();

	CRuAnimationInterpolator*		Clone() const;

	// Setup
	void							SetChannel(UINT32 channel);
	void							SetInitialOrientation(INT32 keyFrameIndex, const CRuVector3 &scale, const CRuQuaternion &rotation, const CRuVector3 &translation);
	void							SetGroupIDs(UINT32 *groupIDs);

	// Animation settings
	void							ClearAnimations(float transitionDuration);
	BOOL							PlayAnimation(CRuAnimation *animation, float startTime, float blendInTime, float blendOutTime, float loopBlendDuration);

	// Animation time settings
	BOOL							SetAnimationTime(float animTime);
	BOOL							UpdateAnimationTime(float deltaTime, BOOL pauseTransition = FALSE);

	// Blending
	BOOL							BlendDeltaKey(INT32 nodeIndex, const CRuVector3 &scale, const CRuQuaternion &rotation, const CRuVector3 &position);
	BOOL							Blend(const CRuAnimationInterpolator *interpolatorA, const CRuAnimationInterpolator *interpolatorB, float t);

	// Mixing
	BOOL							Mix(const CRuAnimationInterpolator *interpolator, UINT32 mixGroupIDs, REAL mixWeight);

	BOOL							OverrideKeyFrame_Rotation(INT32 index, CRuQuaternion rotQuat);
	BOOL							OverrideKeyFrame(INT32 index, const CRuVector3 &scale, const CRuQuaternion &rotQuat, const CRuVector3 &translation);

	BOOL							Synchronize(CRuAnimationInterpolator *keyedInterpolator);

	// Property access
	REAL							GetTime() const;
	INT32							GetNumNodes() const;
	CRuAnimKeyFrame*				GetKeyFrames();
	BOOL							IsAnimationPlaying() const;
	REAL							GetAnimationLength() const;

protected:
//	void							UpdateKeyFrames(CRuAnimKeyFrame *keyFrames);
};

// ************************************************************************************************************************************************************

class CRuAnimation_GR2 : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuMemoryStream*			m_animationStream;
	REAL						m_loopDelay;

	granny_file*				m_grannyFile;
	granny_animation*			m_animation;

public:
								CRuAnimation_GR2();
	virtual						~CRuAnimation_GR2();

	void						SetAnimationStream(IRuStream *stream);
	IRuStream*					GetAnimationStream();
	granny_animation*			GetGR2Animation();
	void						ReverseAnimation();

	REAL						GetLoopDelay();
	void						SetLoopDelay(REAL loopDelay);

	REAL						GetDuration();

	// IRuObject serialization
	virtual BOOL				SerializeFrom(IRuStream *stream);
	virtual BOOL				SerializeTo(IRuStream *stream);

protected:
	void						ReleaseResources();
};

// ************************************************************************************************************************************************************

class CRuAnimationInterpolator_GR2 : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuAnimation_GR2*			m_animation;
	REAL						m_duration;
	REAL						m_loopDelay;

public:
								CRuAnimationInterpolator_GR2();
	virtual						~CRuAnimationInterpolator_GR2();

	BOOL						SetAnimation(CRuAnimation_GR2 *animation);
	BOOL						SetLoopDelay(REAL loopDelay);

	REAL						GetAnimationLength();

	BOOL						Sample(REAL t, CRuVector3 &position, CRuQuaternion &rotation, CRuVector3 &scale);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
