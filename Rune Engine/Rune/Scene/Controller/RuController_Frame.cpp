#include "../../Scene/Controller/RuController_Frame.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_Frame, IRuEntity_KeyableController)
ruHEAP_DEFINE_SUBCLASS(CRuController_Frame, "CRuController_Frame", "IRuEntity_KeyableController")
ruCLASSGUID_DEFINE(CRuController_Frame, 0xC2D32F63, 0x3AF24713, 0x80A7B1BF, 0x0A0442FA)

// ************************************************************************************************************************************************************

CRuController_Frame::CRuController_Frame()
:	m_targetEntity(NULL),
	m_animInterpolator0(NULL),
	m_animInterpolator1(NULL),
	m_currentAnimation(NULL),
	m_blendTimeRemaining(0.0f),
	m_loopBlendTime(0.0f),
	m_blendTime(0.5f)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Frame::HandleEvent_Dispose, this, _1), NULL);

	m_track_Animation.SetTrackName("Animation");
}

CRuController_Frame::~CRuController_Frame()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

REAL CRuController_Frame::GetLoopBlendTime()
{
	return m_loopBlendTime;
}

void CRuController_Frame::SetLoopBlendTime(REAL loopBlendTime)
{
	m_loopBlendTime = loopBlendTime;
}

REAL CRuController_Frame::GetBlendTime()
{
	return m_blendTime;
}

void CRuController_Frame::SetBlendTime(REAL blendTime)
{
	m_blendTime = blendTime;
}

IRuObject *CRuController_Frame::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_Frame();
	CRuController_Frame *controller = static_cast<CRuController_Frame *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_KeyableController::Clone(clonedObject);

	// Clone properties
	controller->m_loopBlendTime = m_loopBlendTime;
	controller->m_blendTime = m_blendTime;

	// Clone tracks
	CloneTracks(controller);

	return clonedObject;
}

BOOL CRuController_Frame::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_KeyableController::SerializeFrom(stream);

	switch(version)
	{
		case 1:
			{
				// Read properties
				stream->Read(&m_loopBlendTime, sizeof(REAL));
				stream->Read(&m_blendTime, sizeof(REAL));

				// Serialize tracks
				m_track_Animation.SerializeFrom(stream);
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_Frame::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_KeyableController::SerializeTo(stream);

	// Write properties
	stream->Write(&m_loopBlendTime, sizeof(REAL));
	stream->Write(&m_blendTime, sizeof(REAL));

	// Serialize tracks
	m_track_Animation.SerializeTo(stream);

	return TRUE;
}

BOOL CRuController_Frame::Update(REAL elapsedTime)
{
	if(m_targetEntity && m_enabled)
	{
		REAL relativeT = m_age - m_startTime;

		if(relativeT < 0.0f)
		{
		}
		else
		{
			// If looping is enabled, clamp time range to the duration
			if(relativeT > m_duration && m_duration > 0.0f && m_loopTime >= 0.0f)
			{
				relativeT = fmod(relativeT, m_duration);
			}

			const char *animationName = m_track_Animation.GetDiscreteKey(relativeT);
			if(animationName != m_currentAnimation)
			{
				m_currentAnimation = animationName;
				CRuAnimation *anim = g_ruResourceManager->LoadAnimation(m_currentAnimation);

				if(anim)
				{
					REAL blendInTime = 0.0f;
					REAL blendOutTime = 0.0f;
					CRuAnimationInterpolator *targetInterpolator = m_animInterpolator0;

					if(m_animInterpolator0->IsAnimationPlaying())
					{
						if(m_animInterpolator1->IsAnimationPlaying())
						{
							// Swap interpolators
							CRuAnimationInterpolator *temp = m_animInterpolator0;
							m_animInterpolator0 = m_animInterpolator1;
							m_animInterpolator1 = temp;

							// Play new animation on interpolator 1
							targetInterpolator = m_animInterpolator1;
						}
						else
						{
							// Play new animation on interpolator 1
							targetInterpolator = m_animInterpolator1;
						}
					}

					// Play animation on target interpolator
					targetInterpolator->ClearAnimations(0.0f);
					targetInterpolator->PlayAnimation(anim, targetInterpolator->GetTime(), blendInTime, blendOutTime, m_loopBlendTime);

					m_blendTimeRemaining = m_blendTime;
				}
			}

			m_blendTimeRemaining -= elapsedTime;

			m_animInterpolator0->UpdateAnimationTime(elapsedTime);

			if(m_animInterpolator1->IsAnimationPlaying())
			{
				m_animInterpolator1->UpdateAnimationTime(elapsedTime);

				if(m_blendTimeRemaining > 0.0f)
				{
					// Mix animation into interpolator 0
					REAL weight1 = min(1.0f, 1.0f - m_blendTimeRemaining / m_blendTime);
					m_animInterpolator0->Mix(m_animInterpolator1, 0xFFFFFFFF, weight1);
				}
				else
				{
					// Swap interpolators
					CRuAnimationInterpolator *temp = m_animInterpolator0;
					m_animInterpolator0 = m_animInterpolator1;
					m_animInterpolator1 = temp;

					// Clear second interpolator
					m_animInterpolator1->ClearAnimations(0.0f);

					// Reset remaining blend time to zero
					m_blendTimeRemaining = 0.0f;
				}
			}

			CRuAnimKeyFrame *keyFrames = m_animInterpolator0->GetKeyFrames();

			m_targetEntity->ResetLocalTransform();
			m_targetEntity->Scale(keyFrames[0].m_scale);
			m_targetEntity->Rotate(keyFrames[0].m_rotation);
			m_targetEntity->Translate(keyFrames[0].m_translation);
		}
	}

	// Chain update call to base controller class
	return IRuEntity_KeyableController::Update(elapsedTime);
}

IRuObject *CRuController_Frame::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_Frame::SetTarget(IRuObject *target)
{
	if(target->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		// If we had a previous target, unregister it
		if(m_targetEntity)
		{
			m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		m_targetEntity = static_cast<CRuEntity *>(target);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Frame::HandleEvent_Dispose, this, _1), NULL);

		// Initialize animation interpolators
		ruSAFE_RELEASE(m_animInterpolator0);
		ruSAFE_RELEASE(m_animInterpolator1);
		m_animInterpolator0 = ruNEW CRuAnimationInterpolator(1);
		m_animInterpolator1 = ruNEW CRuAnimationInterpolator(1);

		return TRUE;
	}

	return FALSE;
}

INT32 CRuController_Frame::GetNumTracks() const
{
	return 1;
}

BOOL CRuController_Frame::GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut)
{
	switch(trackIndex)
	{
		case 0:
			trackOut = &m_track_Animation;
			return TRUE;
	}

	return FALSE;
}

BOOL CRuController_Frame::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_targetEntity)
	{
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	// Release animation interpolators
	ruSAFE_RELEASE(m_animInterpolator0);
	ruSAFE_RELEASE(m_animInterpolator1);

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
