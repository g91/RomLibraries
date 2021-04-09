#include "RuAnimBase.h"

#include "../../Helper/RuHelper_GR2.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruHEAP_DEFINE_ROOTCLASS(CRuAnimation, "CRuAnimation");

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFrameAnimationInterpolator, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuFrameAnimationInterpolator, "CRuFrameAnimation", "IRuObject")
ruCLASSGUID_DEFINE(CRuFrameAnimationInterpolator, 0x1C0FE213, 0x2A494445, 0xB638A81A, 0x64F242CD)

ruRTTI_DEFINE_SUBCLASS(CRuAnimationInterpolator, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuAnimationInterpolator, "CRuAnimationInterpolator", "IRuObject")
ruCLASSGUID_DEFINE(CRuAnimationInterpolator, 0xA6C5B8EB, 0x09DB4eb6, 0xB5E8CE6E, 0x8F816CB8)

ruRTTI_DEFINE_SUBCLASS(CRuAnimation_GR2, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuAnimation_GR2, "CRuAnimation_GR2", "IRuObject")
ruCLASSGUID_DEFINE(CRuAnimation_GR2, 0x1B720372, 0xA02A4e22, 0x9BD45298, 0x886B897B)

ruRTTI_DEFINE_SUBCLASS(CRuAnimationInterpolator_GR2, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuAnimationInterpolator_GR2, "CRuAnimationInterpolator_GR2", "IRuObject")
ruCLASSGUID_DEFINE(CRuAnimationInterpolator_GR2, 0x09AF7CB2, 0x720A4a01, 0x8CFB4245, 0x29A32AD6)

// ************************************************************************************************************************************************************

CRuAnimation::CRuAnimation(INT32 numFrames, float duration)
{
	m_duration = duration;
	m_numFrames = numFrames;

	m_keyFrames = ruNEW CRuAnimKeyFrame [m_numFrames];

	m_numUpdateNodes = 0;
	m_updateNodes = NULL;
}

CRuAnimation::~CRuAnimation()
{
	delete[] m_keyFrames;
	delete[] m_updateNodes;
}

BOOL CRuAnimation::InterpolateKeyFrames(float t, float loopBlendDuration, float weight, UINT32 activeGroupIDs, UINT32 *nodeGroupIDs, CRuAnimKeyFrame *keyFramesOut)
{
	return InterpolateKeyFrames(0xFFFFFFFF, t, loopBlendDuration, weight, activeGroupIDs, nodeGroupIDs, keyFramesOut);
}

/*!
	Performs key frame interpolation at time t and outputs results into keyFramesOut.

	The list of active group IDs instructs this interpolator to apply key frames only to
	those nodes that have matching group IDs. If activeGroupIDs is 0xFFFFFFFF, that means
	every node will be interpolated.

	@param t Time to interpolate at.
	@param loopBlendDuration If zero or positive, specifies the animation loop blending duration, otherwise no looping is performed.
	@param weight Specifies the weight to apply to the interpolated key frames.
	@param activeGroupIDs Bit field of active group IDs.
	@param nodeGroupIDs Array of group ID bit fields for all nodes.
	@param keyFramesOut Output key frame array. May contain prior interpolation data.

	@return TRUE if successful, FALSE otherwise.
*/
BOOL CRuAnimation::InterpolateKeyFrames(INT32 framesToInterpolate, float t, float loopBlendDuration, float weight, UINT32 activeGroupIDs, UINT32 *nodeGroupIDs, CRuAnimKeyFrame *keyFramesOut)
{
	INT32 curKeyFrameIndex = 0;
	INT32 curNodeIndex = 0;
	INT32 updateNodeIndex = 0;

	// If t is out of playing range, return
	if((t < 0.0f) || (t > m_duration && loopBlendDuration < 0.0f))
		return TRUE;

	// If the animation is looping, adjust t so that it is in valid range
	if(loopBlendDuration >= 0.0f)
	{
		if(t > m_duration + loopBlendDuration)
		{
			// When loopBlendDuration >= 0, the animation is looped, otherwise the animation stops playing
			t = fmod(t, m_duration + loopBlendDuration);
		}
	}

	while(curKeyFrameIndex < m_numFrames && curNodeIndex < framesToInterpolate)
	{
//		if(activeGroupIDs == 0xFFFFFFFF || (activeGroupIDs & nodeGroupIDs[curNodeIndex]))
		{
			++updateNodeIndex;

			INT32 keyFrame0, keyFrame1;
			float timeGap;

			if(t > m_duration)
			{
				// Set key frame 1 to the initial key frame
				keyFrame0 = curKeyFrameIndex;
				keyFrame1 = curKeyFrameIndex;

				// Set key frame 0 to the last key frame
				while(m_keyFrames[keyFrame0].m_time < m_duration)
				{
					++keyFrame0;
				}

				// Calculate time gap between key frames
				timeGap = loopBlendDuration;
			}
			else
			{
				keyFrame0 = curKeyFrameIndex;
				keyFrame1 = curKeyFrameIndex + 1;

				// Find the frames to interpolate
				while(keyFrame1 < m_numFrames)
				{
					if(m_keyFrames[keyFrame1].m_time >= t)
					{
						keyFrame0 = keyFrame1 - 1;
						break;
					}

					++keyFrame1;
				}

				// Calculate time gap between key frames
				timeGap = m_keyFrames[keyFrame1].m_time - m_keyFrames[keyFrame0].m_time;
			}

			float frame1Weight = (t - m_keyFrames[keyFrame0].m_time) / timeGap;
			float frame0Weight = 1.0f - frame1Weight;

			// Interpolate key frames
			keyFramesOut[curNodeIndex].m_time = t;

			if(weight < 1.0f)
			{
				float weight1 = 1.0f - weight;

				keyFramesOut[curNodeIndex].m_scale = (m_keyFrames[keyFrame0].m_scale * frame0Weight + m_keyFrames[keyFrame1].m_scale * frame1Weight) * weight + keyFramesOut[curNodeIndex].m_scale * weight1;
				keyFramesOut[curNodeIndex].m_rotation = QuaternionSlerp(QuaternionSlerp(m_keyFrames[keyFrame0].m_rotation, m_keyFrames[keyFrame1].m_rotation, frame1Weight), keyFramesOut[curNodeIndex].m_rotation, weight1);
				keyFramesOut[curNodeIndex].m_translation = (m_keyFrames[keyFrame0].m_translation * frame0Weight + m_keyFrames[keyFrame1].m_translation * frame1Weight) * weight + keyFramesOut[curNodeIndex].m_translation * weight1;
			}
			else
			{
				keyFramesOut[curNodeIndex].m_scale = m_keyFrames[keyFrame0].m_scale * frame0Weight + m_keyFrames[keyFrame1].m_scale * frame1Weight;
				keyFramesOut[curNodeIndex].m_rotation = QuaternionSlerp(m_keyFrames[keyFrame0].m_rotation, m_keyFrames[keyFrame1].m_rotation, frame1Weight);
				keyFramesOut[curNodeIndex].m_translation = m_keyFrames[keyFrame0].m_translation * frame0Weight + m_keyFrames[keyFrame1].m_translation * frame1Weight;
			}
		}

		// Advance to next node
		while(m_keyFrames[curKeyFrameIndex].m_time < m_duration)
		{
			++curKeyFrameIndex;
		}

		++curKeyFrameIndex;
		++curNodeIndex;
	}

	return TRUE;
}

float CRuAnimation::GetDuration()
{
	return m_duration;
}

INT32 CRuAnimation::GetNumKeyFrames()
{
	return m_numFrames;
}

CRuAnimKeyFrame *CRuAnimation::GetKeyFrames()
{
	return m_keyFrames;
}

INT32 CRuAnimation::GetNumNodes()
{
	INT32 curKeyFrameIndex = 0;
	INT32 numNodes = 0;

	while(curKeyFrameIndex < m_numFrames)
	{
		if(m_keyFrames[curKeyFrameIndex].m_time == m_duration)
		{
			++numNodes;
		}

		++curKeyFrameIndex;
	}

	return numNodes;
}

CRuAnimation *LoadRuAnimation(IRuStream *stream)
{
	// Read number of keyframes
	INT32 numKeyframes;
	stream->Read(&numKeyframes, sizeof(INT32));

	// Read duration
	float duration;
	stream->Read(&duration, sizeof(float));

	CRuAnimation *anim = ruNEW CRuAnimation(numKeyframes, duration);

	stream->Read(&anim->m_numUpdateNodes, sizeof(INT32));

	anim->m_updateNodes = ruNEW INT32 [anim->m_numUpdateNodes];
	for(INT32 i = 0; i < anim->m_numUpdateNodes; ++i)
	{
		stream->Read(&anim->m_updateNodes[i], sizeof(INT32));
	}
	
	CRuAnimKeyFrame *keyframes = anim->GetKeyFrames();
	for(INT32 i = 0; i < numKeyframes; ++i)
	{
		stream->Read(&keyframes[i].m_time, sizeof(float));

		stream->Read(&keyframes[i].m_scale.x, sizeof(float));
		stream->Read(&keyframes[i].m_scale.y, sizeof(float));
		stream->Read(&keyframes[i].m_scale.z, sizeof(float));

		stream->Read(&keyframes[i].m_rotation.x, sizeof(float));
		stream->Read(&keyframes[i].m_rotation.y, sizeof(float));
		stream->Read(&keyframes[i].m_rotation.z, sizeof(float));
		stream->Read(&keyframes[i].m_rotation.w, sizeof(float));

		stream->Read(&keyframes[i].m_translation.x, sizeof(float));
		stream->Read(&keyframes[i].m_translation.y, sizeof(float));
		stream->Read(&keyframes[i].m_translation.z, sizeof(float));
	}

	return anim;
}

BOOL SaveRuAnimation(IRuStream *stream, CRuAnimation* animation)
{
	// Write number of keyframes
	stream->Write(&animation->m_numFrames, sizeof(INT32));

	// Write duration
	stream->Write(&animation->m_duration, sizeof(float));

	// Write number of update nodes
	stream->Write(&animation->m_numUpdateNodes, sizeof(INT32));

	// Write update nodes
	for(INT32 i = 0; i < animation->m_numUpdateNodes; ++i)
	{
		stream->Write(&animation->m_updateNodes[i], sizeof(INT32));
	}

	// Write keyframes
	CRuAnimKeyFrame *keyframes = animation->GetKeyFrames();
	for(INT32 i = 0; i < animation->m_numFrames; ++i)
	{
		stream->Write(&keyframes[i].m_time, sizeof(float));

		stream->Write(&keyframes[i].m_scale.x, sizeof(float));
		stream->Write(&keyframes[i].m_scale.y, sizeof(float));
		stream->Write(&keyframes[i].m_scale.z, sizeof(float));

		stream->Write(&keyframes[i].m_rotation.x, sizeof(float));
		stream->Write(&keyframes[i].m_rotation.y, sizeof(float));
		stream->Write(&keyframes[i].m_rotation.z, sizeof(float));
		stream->Write(&keyframes[i].m_rotation.w, sizeof(float));

		stream->Write(&keyframes[i].m_translation.x, sizeof(float));
		stream->Write(&keyframes[i].m_translation.y, sizeof(float));
		stream->Write(&keyframes[i].m_translation.z, sizeof(float));
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuFrameAnimationInterpolator::CRuFrameAnimationInterpolator()
{
}

CRuFrameAnimationInterpolator::~CRuFrameAnimationInterpolator()
{
}

BOOL CRuFrameAnimationInterpolator::SerializeFrom(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuFrameAnimationInterpolator::SerializeTo(IRuStream *stream)
{
	return TRUE;
}

// ************************************************************************************************************************************************************

CRuAnimationInterpolator::CRuAnimationInterpolator(INT32 numNodes)
{
	m_time = 0.0f;
	m_numNodes = numNodes;
	m_groupIDs = ruNEW UINT32 [m_numNodes];
	m_keyFrames = ruNEW CRuAnimKeyFrame [m_numNodes];

	// Reset transition variables
	m_transitionStarted = FALSE;
	m_transitionTime = 0.0f;
	m_transitionDuration = 0.0f;
	m_transitionKeyFrames = ruNEW CRuAnimKeyFrame [m_numNodes];

	memset(m_groupIDs, 0xFFFFFFFF, sizeof(UINT32) * m_numNodes);

	// Reset key frames to default orientation
	for(INT32 i = 0; i < m_numNodes; ++i)
	{
		m_keyFrames[i].m_scale.Set(1.0f, 1.0f, 1.0f);
		m_keyFrames[i].m_rotation.Set(0.0f, 0.0f, 0.0f, 1.0f);
		m_keyFrames[i].m_translation.Set(0.0f, 0.0f, 0.0f);
	}

	m_channel = 0xFFFFFFFF;
}

CRuAnimationInterpolator::~CRuAnimationInterpolator()
{
	delete[] m_groupIDs;
	delete[] m_keyFrames;
	delete[] m_transitionKeyFrames;
}

CRuAnimationInterpolator *CRuAnimationInterpolator::Clone() const
{
	CRuAnimationInterpolator *clonedInterpolator = ruNEW CRuAnimationInterpolator(m_numNodes);

	// Copy group mask data
	clonedInterpolator->SetGroupIDs(m_groupIDs);

	// Copy animation data
	for(INT32 i = 0; i < m_animations.Count(); ++i)
	{
		clonedInterpolator->m_animations.Add(m_animations[i]);
	}

	// Copy time
	clonedInterpolator->m_time = m_time;

	// Copy current key frames
	for(INT32 i = 0; i < m_numNodes; ++i)
	{
		clonedInterpolator->m_keyFrames[i] = m_keyFrames[i];
		clonedInterpolator->m_transitionKeyFrames[i] = m_transitionKeyFrames[i];
	}

	// Copy transition info
	clonedInterpolator->m_transitionStarted = m_transitionStarted;
	clonedInterpolator->m_transitionTime = m_transitionTime;
	clonedInterpolator->m_transitionDuration = m_transitionDuration;

	return clonedInterpolator;
}

void CRuAnimationInterpolator::SetChannel(UINT32 channel)
{
	m_channel = channel;
}

void CRuAnimationInterpolator::SetInitialOrientation(INT32 keyFrameIndex, const CRuVector3 &scale, const CRuQuaternion &rotation, const CRuVector3 &translation)
{
	m_keyFrames[keyFrameIndex].m_scale = scale;
	m_keyFrames[keyFrameIndex].m_rotation = rotation;
	m_keyFrames[keyFrameIndex].m_translation = translation;

	m_transitionKeyFrames[keyFrameIndex].m_scale = scale;
	m_transitionKeyFrames[keyFrameIndex].m_rotation = rotation;
	m_transitionKeyFrames[keyFrameIndex].m_translation = translation;
}

void CRuAnimationInterpolator::SetGroupIDs(UINT32 *groupIDs)
{
	memcpy(m_groupIDs, groupIDs, sizeof(UINT32) * m_numNodes);
}

void CRuAnimationInterpolator::ClearAnimations(float transitionDuration)
{
	m_animations.Clear();
	m_time = 0.0f;

	if(transitionDuration > 0.0f)
	{
		m_transitionStarted = FALSE;
		m_transitionTime = 0.0f;
		m_transitionDuration = transitionDuration;

		memcpy(m_transitionKeyFrames, m_keyFrames, sizeof(CRuAnimKeyFrame) * m_numNodes);
	}
}

BOOL CRuAnimationInterpolator::PlayAnimation(CRuAnimation *animation, float startTime, float blendInTime, float blendOutTime, float loopBlendDuration)
{
	// Ensure the animation is compatible with the interpolator
//	if(animation->GetNumNodes() == m_numNodes)
	{
		AnimationInfo animInfo;

		animInfo.m_startTime = startTime;
		animInfo.m_blendInTime = blendInTime;
		animInfo.m_blendOutTime = blendOutTime;
		animInfo.m_localTime = 0.0f - startTime;

		animInfo.m_animation = animation;
		animInfo.m_loopBlendDuration = loopBlendDuration;
		animInfo.m_activeGroupIDs = m_channel;

		m_animations.Add(animInfo);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuAnimationInterpolator::SetAnimationTime(float animTime)
{
	m_time = animTime;

	// Reset transition mode status to off
	m_transitionTime = 0.0f;
	m_transitionDuration = 0.0f;

	// Reset local time for all playing animations
	for(INT32 i = 0; i < m_animations.Count(); ++i)
	{
		// Update animation time
		m_animations[i].m_localTime = animTime - m_animations[i].m_startTime;
	}

	// Update keyframes
	UpdateAnimationTime(0.0f);

	return TRUE;
}

BOOL CRuAnimationInterpolator::UpdateAnimationTime(float deltaTime, BOOL pauseTransition)
{
	float frame0Weight = 1.0f, frame1Weight = 0.0f, frame2Weight = 0.0f;

	// Update animation time
	m_time += deltaTime;

	// Blend animations one by one
	for(INT32 i = 0; i < m_animations.Count(); ++i)
	{
		float duration = m_animations[i].m_animation->GetDuration();
		float animWeight = 1.0f;

		// Update animation time
		m_animations[i].m_localTime += deltaTime;

		// Attempt to mark animation as active only if its local time >= 0
		if(m_animations[i].m_localTime >= 0.0f)
		{
			// Calculate the time at which the animation starts to blend out
			float blendOutT = duration - m_animations[i].m_blendOutTime;

			// If the animation is blending in, calculate its weight
			if(m_animations[i].m_localTime < m_animations[i].m_blendInTime)
			{
				animWeight = m_animations[i].m_localTime / m_animations[i].m_blendInTime;
			}

			// If the animation is blending out, calculate its weight
			if(m_animations[i].m_localTime >= blendOutT && m_animations[i].m_localTime < duration)
			{
				animWeight = min(1.0f - ((m_animations[i].m_localTime - blendOutT) / m_animations[i].m_blendOutTime), animWeight);
			}
		}

		m_animations[i].m_animation->InterpolateKeyFrames(m_numNodes, m_animations[i].m_localTime, m_animations[i].m_loopBlendDuration, animWeight, m_animations[i].m_activeGroupIDs, m_groupIDs, m_keyFrames);
	}

	if(m_transitionDuration > 0.0f)
	{
		m_transitionStarted = TRUE;

		if(pauseTransition == FALSE)
			m_transitionTime += deltaTime;

		if(m_transitionTime <= m_transitionDuration)
		{
			// Perform blend
			frame1Weight = m_transitionTime / m_transitionDuration;
			frame0Weight = 1.0f - frame1Weight;
			for(INT32 i = 0; i < m_numNodes; ++i)
			{
				m_keyFrames[i].m_scale = m_transitionKeyFrames[i].m_scale * frame0Weight + m_keyFrames[i].m_scale * frame1Weight;
				m_keyFrames[i].m_rotation = QuaternionSlerp(m_transitionKeyFrames[i].m_rotation, m_keyFrames[i].m_rotation, frame1Weight);
				m_keyFrames[i].m_translation = m_transitionKeyFrames[i].m_translation * frame0Weight + m_keyFrames[i].m_translation * frame1Weight;
			}

			return TRUE;
		}

		m_transitionStarted = FALSE;
		m_transitionTime = 0.0f;
		m_transitionDuration = 0.0f;
		deltaTime = m_transitionTime - m_transitionDuration;
	}

	return TRUE;
}

BOOL CRuAnimationInterpolator::BlendDeltaKey(INT32 nodeIndex, const CRuVector3 &scale, const CRuQuaternion &rotation, const CRuVector3 &position)
{
	m_keyFrames[nodeIndex].m_scale.Set(m_keyFrames[nodeIndex].m_scale.m_x * scale.m_x, m_keyFrames[nodeIndex].m_scale.m_y * scale.m_y, m_keyFrames[nodeIndex].m_scale.m_z * scale.m_z);
	m_keyFrames[nodeIndex].m_rotation = m_keyFrames[nodeIndex].m_rotation * rotation;
	m_keyFrames[nodeIndex].m_translation = m_keyFrames[nodeIndex].m_translation + position;

	return TRUE;
}

BOOL CRuAnimationInterpolator::Blend(const CRuAnimationInterpolator *interpolatorA, const CRuAnimationInterpolator *interpolatorB, float t)
{
	return TRUE;
}

/*
	in theory, this thing "mixes" one or more interpolators together. e.g. you have two interpolators, one for
	upper body and one for lower body. this function will allow you to take just the key frames from the upper body
	and the key frames from the lower body and combine them into a new interpolator. per call, this takes only those
	nodes which match the mixBoneIDs criteria, and multiplies the selected nodes with the mix weight.

	this will allow us to smoothly blend both upper and lower body animations very easily with minimal work.
*/
BOOL CRuAnimationInterpolator::Mix(const CRuAnimationInterpolator *interpolator, UINT32 mixGroupIDs, REAL mixWeight)
{
	REAL invMixWeight = 1.0f - mixWeight;

	for(INT32 i = 0; i < m_numNodes; ++i)
	{
		// Only mix if we are asked to mix all group IDs, or if the node matches the group ID mask
		if(mixGroupIDs == 0xFFFFFFFF || (mixGroupIDs & m_groupIDs[i]))
		{
			// Interpolate
			m_keyFrames[i].m_scale = m_keyFrames[i].m_scale * invMixWeight + interpolator->m_keyFrames[i].m_scale * mixWeight;
			m_keyFrames[i].m_rotation = QuaternionSlerp(m_keyFrames[i].m_rotation, interpolator->m_keyFrames[i].m_rotation, mixWeight);
			m_keyFrames[i].m_translation = m_keyFrames[i].m_translation * invMixWeight + interpolator->m_keyFrames[i].m_translation * mixWeight;
		}
	}

	return TRUE;
}

BOOL CRuAnimationInterpolator::OverrideKeyFrame_Rotation(INT32 index, CRuQuaternion rotQuat)
{
	m_keyFrames[index].m_rotation = rotQuat * m_keyFrames[index].m_rotation;
//	m_keyFrames[index].m_rotation = m_keyFrames[index].m_rotation * rotQuat;

	return TRUE;
}

BOOL  CRuAnimationInterpolator::OverrideKeyFrame(INT32 index, const CRuVector3 &scale, const CRuQuaternion &rotQuat, const CRuVector3 &translation)
{
	m_keyFrames[index].m_scale = scale;
	m_keyFrames[index].m_rotation = rotQuat;
//	m_keyFrames[index].m_translation = translation;

	return TRUE;
}

BOOL CRuAnimationInterpolator::Synchronize(CRuAnimationInterpolator *keyedInterpolator)
{
	if(m_numNodes == keyedInterpolator->m_numNodes && m_animations.Count() == keyedInterpolator->m_animations.Count())
	{
		for(INT32 i = 0; i < m_animations.Count(); ++i)
		{
			m_animations[i].m_localTime = keyedInterpolator->m_animations[i].m_localTime;
			m_animations[i].m_weight = keyedInterpolator->m_animations[i].m_weight;
			m_animations[i].m_weightFlux = keyedInterpolator->m_animations[i].m_weightFlux;
			m_animations[i].m_inFrame0 = keyedInterpolator->m_animations[i].m_inFrame0;
			m_animations[i].m_inFrame2 = keyedInterpolator->m_animations[i].m_inFrame2;
		}

		m_time = keyedInterpolator->m_time;

		// Force update
		UpdateAnimationTime(0.0f);

		return TRUE;
	}

	return FALSE;
}

REAL CRuAnimationInterpolator::GetTime() const
{
	return m_time;
}

INT32 CRuAnimationInterpolator::GetNumNodes() const
{
	return m_numNodes;
}

CRuAnimKeyFrame *CRuAnimationInterpolator::GetKeyFrames()
{
	return m_keyFrames;
}

BOOL CRuAnimationInterpolator::IsAnimationPlaying() const
{
	return m_animations.Count() > 0;
}

REAL CRuAnimationInterpolator::GetAnimationLength() const
{
	REAL longestAnimation = 0.0f;
	for(INT32 i = 0; i < m_animations.Count(); ++i)
	{
		REAL endTime = m_animations[i].m_startTime + m_animations[i].m_animation->GetDuration();
		if(longestAnimation < endTime)
		{
			longestAnimation = endTime;
		}
	}

	return longestAnimation;
}

/*
void CRuAnimationInterpolator::UpdateKeyFrames(CRuAnimKeyFrame *keyFrames)
{
	for(INT32 i = 0; i < m_animations.Count(); ++i)
	{
		if(m_animations[i].m_localTime >= 0.0f)
		{
			m_animations[i].m_animation->InterpolateKeyFrames(m_animations[i].m_localTime, m_animations[i].m_loopBlendDuration, 1.0f, keyFrames);
		}
	}
}
*/
// ************************************************************************************************************************************************************
/*
BOOL GR2Utility_FreeTrackGroup(granny_track_group *trackGroup)
{
	// Free vector tracks
	for(INT32 i = 0; i < trackGroup->VectorTrackCount; ++i)
	{
		delete[] trackGroup->VectorTracks[i].Name;
		GrannyFreeCurve(&trackGroup->VectorTracks[i].ValueCurve);
	}

	delete[] trackGroup->VectorTracks;

	// Free transform tracks
	for(INT32 i = 0; i < trackGroup->TransformTrackCount; ++i)
	{
		delete[] trackGroup->TransformTracks[i].Name;
		GrannyFreeCurve(&trackGroup->TransformTracks[i].PositionCurve);
		GrannyFreeCurve(&trackGroup->TransformTracks[i].OrientationCurve);
		GrannyFreeCurve(&trackGroup->TransformTracks[i].ScaleShearCurve);
	}

	delete[] trackGroup->TransformTracks;

	// Free transform LOD error array
	delete[] trackGroup->TransformLODErrors;

	// Free text tracks
	for(INT32 i = 0; i < trackGroup->TextTrackCount; ++i)
	{
		delete[] trackGroup->TextTracks[i].Name;

		for(INT32 j = 0; j < trackGroup->TextTracks[i].EntryCount; ++j)
		{
			delete[] trackGroup->TextTracks[i].Entries[j].Text;
		}

		delete[] trackGroup->TextTracks[i].Entries;
	}

	delete[] trackGroup->TextTracks;

	// Free periodic loop structure
	if(trackGroup->PeriodicLoop)
	{
		delete trackGroup->PeriodicLoop;
	}

	// Free root motion transform track
	if(trackGroup->RootMotion)
	{
		delete[] trackGroup->RootMotion->Name;
		GrannyFreeCurve(&trackGroup->RootMotion->PositionCurve);
		GrannyFreeCurve(&trackGroup->RootMotion->OrientationCurve);
		GrannyFreeCurve(&trackGroup->RootMotion->ScaleShearCurve);
	}

	// Free track group name array
	delete[] trackGroup->Name;

	// Free track group object
	delete trackGroup;

	return TRUE;
}

granny_track_group *GR2Utility_CloneTrackGroup(granny_track_group *srcTrackGroup)
{
	granny_track_group *dstTrackGroup = ruNEW granny_track_group;
	memset(dstTrackGroup, 0, sizeof(granny_track_group));

	// Copy track group
	dstTrackGroup->Name = ruNEW char [strlen(srcTrackGroup->Name) + 1];
	strcpy(dstTrackGroup->Name, srcTrackGroup->Name);

	// Copy transform tracks
	dstTrackGroup->TransformTrackCount = srcTrackGroup->TransformTrackCount;
	dstTrackGroup->TransformTracks = ruNEW granny_transform_track [dstTrackGroup->TransformTrackCount];
	memset(dstTrackGroup->TransformTracks, 0, sizeof(granny_transform_track) * dstTrackGroup->TransformTrackCount);

	for(INT32 i = 0; i < srcTrackGroup->TransformTrackCount; ++i)
	{
		// Copy track name
		dstTrackGroup->TransformTracks[i].Name = ruNEW char [strlen(srcTrackGroup->TransformTracks[i].Name) + 1];
		strcpy(dstTrackGroup->TransformTracks[i].Name, srcTrackGroup->TransformTracks[i].Name);

		// Copy position curve
		granny_curve_builder *posCurveBuilder = GrannyBeginCurveCopy(&srcTrackGroup->TransformTracks[i].PositionCurve);

		GrannyGetResultingCurveDataSize(posCurveBuilder)

		granny_curve2 *posCurve = GrannyEndCurve(posCurveBuilder);
		dstTrackGroup->TransformTracks[i].PositionCurve = *posCurve;
		GrannyFreeCurve(posCurve);

		dstTrackGroup->TransformTracks[i].PositionCurve = GrannyEndCurve(posCurveBuilder);

		// Copy orientation curve
		granny_curve_builder *rotCurveBuilder = GrannyBeginCurveCopy(&srcTrackGroup->TransformTracks[i].OrientationCurve);
		granny_curve2 *rotCurve = GrannyEndCurve(rotCurveBuilder);
		dstTrackGroup->TransformTracks[i].OrientationCurve = *rotCurve;
		GrannyFreeCurve(rotCurve);

		dstTrackGroup->TransformTracks[i].OrientationCurve = GrannyEndCurve(rotCurveBuilder);

		// Copy scale-shear curve
		granny_curve_builder *ssCurveBuilder = GrannyBeginCurveCopy(&srcTrackGroup->TransformTracks[i].ScaleShearCurve);
		granny_curve2 *ssCurve = GrannyEndCurve(ssCurveBuilder);
		dstTrackGroup->TransformTracks[i].ScaleShearCurve = *ssCurve;
		GrannyFreeCurve(ssCurve);

		dstTrackGroup->TransformTracks[i].ScaleShearCurve = GrannyEndCurve(ssCurveBuilder);
	}

	// Copy LOD error array
	dstTrackGroup->TransformLODErrorCount = srcTrackGroup->TransformLODErrorCount;
	dstTrackGroup->TransformLODErrors = ruNEW granny_real32 [dstTrackGroup->TransformLODErrorCount];
	memcpy(dstTrackGroup->TransformLODErrors, srcTrackGroup->TransformLODErrors, sizeof(granny_real32) * dstTrackGroup->TransformLODErrorCount);

	// Copy initial placement
	dstTrackGroup->InitialPlacement = srcTrackGroup->InitialPlacement;

	// Copy flags
	dstTrackGroup->Flags = srcTrackGroup->Flags;

	// Copy loop translation
	dstTrackGroup->LoopTranslation = srcTrackGroup->LoopTranslation;

	// Copy periodic loop
	dstTrackGroup->PeriodicLoop = ruNEW granny_periodic_loop;
	*dstTrackGroup->PeriodicLoop = *srcTrackGroup->PeriodicLoop;

	// Copy root motion
	if(srcTrackGroup->RootMotion)
	{
		dstTrackGroup->RootMotion = ruNEW granny_transform_track;

		// Copy position curve
		granny_curve_builder *posCurveBuilder = GrannyBeginCurveCopy(&srcTrackGroup->RootMotion->PositionCurve);
		granny_curve2 *posCurve = GrannyEndCurve(posCurveBuilder);
		dstTrackGroup->RootMotion->PositionCurve = *posCurve;
		GrannyFreeCurve(posCurve);

		// Copy orientation curve
		granny_curve_builder *rotCurveBuilder = GrannyBeginCurveCopy(&srcTrackGroup->RootMotion->OrientationCurve);
		granny_curve2 *rotCurve = GrannyEndCurve(rotCurveBuilder);
		dstTrackGroup->RootMotion->OrientationCurve = *rotCurve;
		GrannyFreeCurve(rotCurve);

		// Copy scale-shear curve
		granny_curve_builder *ssCurveBuilder = GrannyBeginCurveCopy(&srcTrackGroup->RootMotion->ScaleShearCurve);
		granny_curve2 *ssCurve = GrannyEndCurve(ssCurveBuilder);
		dstTrackGroup->RootMotion->ScaleShearCurve = *ssCurve;
		GrannyFreeCurve(ssCurve);
	}

	return dstTrackGroup;
}
*/
CRuAnimation_GR2::CRuAnimation_GR2()
:	m_animationStream(NULL),
	m_loopDelay(-1.0f),
	m_grannyFile(NULL),
	m_animation(NULL)
{
}

CRuAnimation_GR2::~CRuAnimation_GR2()
{
	ReleaseResources();
}

void CRuAnimation_GR2::SetAnimationStream(IRuStream *stream)
{
	// Free old stuff
	ReleaseResources();

	// Make a copy of the stream
	m_animationStream = ruNEW CRuMemoryStream();
	*m_animationStream = *stream;

	// Read granny file
	m_grannyFile = GrannyReadEntireFileFromMemory(m_animationStream->GetStreamSize(), m_animationStream->GetBuffer());

	// Enter critical section
	RuExtLink_Granny_GlobalCS()->Enter();

	// Get track group data
	granny_variant gVariant;
	GrannyGetDataTreeFromFile(m_grannyFile, &gVariant);
	m_animation = (granny_animation *) gVariant.Object;

	// Leave critical section
	RuExtLink_Granny_GlobalCS()->Leave();
}

IRuStream *CRuAnimation_GR2::GetAnimationStream()
{
	return m_animationStream;
}

REAL CRuAnimation_GR2::GetLoopDelay()
{
	return m_loopDelay;
}

void CRuAnimation_GR2::SetLoopDelay(REAL loopDelay)
{
	m_loopDelay = loopDelay;
}

granny_animation *CRuAnimation_GR2::GetGR2Animation()
{
	return m_animation;
}

void CRuAnimation_GR2::ReverseAnimation()
{
/*
	// Enter critical section
	RuExtLink_Granny_GlobalCS()->Enter();

	if(m_animation)
	{
		for(INT32 i = 0; i < m_animation->TrackGroupCount; ++i)
		{
			granny_track_group *trackGroup = m_animation->TrackGroups[i];

			if(trackGroup)
			{
				for(INT32 j = 0; j < trackGroup->TransformTrackCount; ++j)
				{
					granny_transform_track *transformTrack = trackGroup->TransformTracks[j];

					GrannyCurveExtractKnotValues(transformTrack->OrientationCurve
				}
			}
		}
	}

	// Leave critical section
	RuExtLink_Granny_GlobalCS()->Leave();
*/
}

REAL CRuAnimation_GR2::GetDuration()
{
	if(m_animation)
	{
		return m_animation->Duration;
	}

	return 0.0f;
}

BOOL CRuAnimation_GR2::SerializeFrom(IRuStream *stream)
{
	// Validate GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeFrom(stream);

	// Read loop delay
	stream->Read(&m_loopDelay, sizeof(REAL));

	// Read track group stream
	INT32 trackGroupStreamSize = 0;
	stream->Read(&trackGroupStreamSize, sizeof(INT32));

	m_animationStream = ruNEW CRuMemoryStream();
	m_animationStream->SetBufferSize(trackGroupStreamSize);
	m_animationStream->SetStreamSize(trackGroupStreamSize);
	stream->Read(m_animationStream->GetBuffer(), trackGroupStreamSize);

	// Read granny file
	m_grannyFile = GrannyReadEntireFileFromMemory(m_animationStream->GetStreamSize(), m_animationStream->GetBuffer());

	// Enter critical section
	RuExtLink_Granny_GlobalCS()->Enter();

	// Get track group data
	granny_variant gVariant;
	GrannyGetDataTreeFromFile(m_grannyFile, &gVariant);

	// Perform version check
	UINT32 fileTag = GrannyGetFileTypeTag(m_grannyFile);

	if(fileTag == GrannyCurrentGRNStandardTag)
	{
		// Same version, so we simply assign the pointer
		m_animation = (granny_animation *) gVariant.Object;
	}
	else
	{
		// Old version, perform data conversion
		m_animation = (granny_animation *) GrannyConvertTree(gVariant.Type, gVariant.Object, GrannyAnimationType);

		// Convert animation to stream
		CRuMemoryStream *memStream = GR2_ConvertVariantToStream(GrannyAnimationType, m_animation);

		// Free old animation data
		GrannyFreeBuilderResult(m_animation);
		m_animation = NULL;

		// Free old granny file and stream
		GrannyFreeFile(m_grannyFile);
		delete m_animationStream;

		// Replace with converted animation data
		m_animationStream = memStream;
		m_grannyFile = GrannyReadEntireFileFromMemory(m_animationStream->GetStreamSize(), m_animationStream->GetBuffer());
		GrannyGetDataTreeFromFile(m_grannyFile, &gVariant);
		m_animation = (granny_animation *) gVariant.Object;
	}

	// Leave critical section
	RuExtLink_Granny_GlobalCS()->Leave();

//	m_animation = (granny_animation *) gVariant.Object;

	return TRUE;
}

BOOL CRuAnimation_GR2::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeTo(stream);

	// Write loop delay
	stream->Write(&m_loopDelay, sizeof(REAL));

	// Write track group stream
	INT32 trackGroupStreamSize = m_animationStream->GetStreamSize();
	stream->Write(&trackGroupStreamSize, sizeof(INT32));
	stream->Write(m_animationStream->GetBuffer(), trackGroupStreamSize);

	return TRUE;
}

void CRuAnimation_GR2::ReleaseResources()
{
	if(m_grannyFile)
	{
		GrannyFreeFile(m_grannyFile);
		m_grannyFile = NULL;
	}

	if(m_animationStream)
	{
		delete m_animationStream;
		m_animationStream = NULL;
	}

	m_animation = NULL;
}

// ************************************************************************************************************************************************************

CRuAnimationInterpolator_GR2::CRuAnimationInterpolator_GR2()
:	m_animation(NULL),
	m_duration(1.0f),
	m_loopDelay(0.0f)
{
}

CRuAnimationInterpolator_GR2::~CRuAnimationInterpolator_GR2()
{
	ruSAFE_RELEASE(m_animation);
}

BOOL CRuAnimationInterpolator_GR2::SetAnimation(CRuAnimation_GR2 *animation)
{
	if(animation)
	{
		animation->AddRef();
		ruSAFE_RELEASE(m_animation);
		m_animation = animation;

		m_duration = m_animation->GetDuration();

		return TRUE;
	}

	return FALSE;
}

BOOL CRuAnimationInterpolator_GR2::SetLoopDelay(REAL loopDelay)
{
	m_loopDelay = loopDelay;

	return TRUE;
}

REAL CRuAnimationInterpolator_GR2::GetAnimationLength()
{
	return m_duration;
}

BOOL CRuAnimationInterpolator_GR2::Sample(REAL t, CRuVector3 &position, CRuQuaternion &rotation, CRuVector3 &scale)
{
	INT32 m_targetTrackGroupIndex = 0;
	INT32 m_targetTransformTrackIndex = 0;

	// Clamp t
	if(m_loopDelay >= 0.0f)
	{
		t = fmod(t, m_duration);
	}
	else
	{
		t = max(0.0f, min(m_duration, t));
	}

	granny_animation *gr2Ani = m_animation->GetGR2Animation();

	if(gr2Ani)
	{
		if(m_targetTrackGroupIndex >= 0 && m_targetTrackGroupIndex < gr2Ani->TrackGroupCount)
		{
			granny_track_group *trackGroup = gr2Ani->TrackGroups[m_targetTrackGroupIndex];

			if(m_targetTransformTrackIndex >= 0 && m_targetTransformTrackIndex < trackGroup->TransformTrackCount)
			{
				granny_transform_track &transformTrack = trackGroup->TransformTracks[m_targetTransformTrackIndex];

				// Evaluate curve at specified t
				REAL positionArray[3];
				REAL positionIdentity[3] = { 0.0f, 0.0f, 0.0f };

				REAL rotationArray[4];
				REAL rotationIdentity[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

				REAL scaleShearArray[9];
				REAL scaleShearIdentity[9] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

				GrannyEvaluateCurveAtT(3, false, false, &transformTrack.PositionCurve, false, gr2Ani->Duration, t, positionArray, positionIdentity);
				GrannyEvaluateCurveAtT(4, false, false, &transformTrack.OrientationCurve, false, gr2Ani->Duration, t, rotationArray, rotationIdentity);
				GrannyEvaluateCurveAtT(9, false, false, &transformTrack.ScaleShearCurve, false, gr2Ani->Duration, t, scaleShearArray, scaleShearIdentity);

				// Store evaluation result
				position[0] = positionArray[0];
				position[1] = positionArray[1];
				position[2] = positionArray[2];

				rotation.m_x = -rotationArray[0];
				rotation.m_y = -rotationArray[1];
				rotation.m_z = -rotationArray[2];
				rotation.m_w = rotationArray[3];

				scale[0] = scaleShearArray[0];
				scale[1] = scaleShearArray[3];
				scale[2] = scaleShearArray[6];

				return TRUE;
			}
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed
