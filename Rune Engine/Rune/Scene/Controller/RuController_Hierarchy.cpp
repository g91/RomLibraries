#include "../../Scene/Controller/RuController_Hierarchy.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_Hierarchy, IRuEntity_Controller)
ruHEAP_DEFINE_SUBCLASS(CRuController_Hierarchy, "CRuController_Hierarchy", "IRuEntity_Controller")
ruCLASSGUID_DEFINE(CRuController_Hierarchy, 0x69F1B94E, 0x486B4bf4, 0x8D0BF5FD, 0xC28A6791)

// ************************************************************************************************************************************************************

CRuController_Hierarchy::CRuController_Hierarchy()
{
	m_targetEntity = NULL;
	m_targetInterpolator = NULL;

	m_applied = FALSE;
	m_animation = NULL;

	m_animation_Control = NULL;
	m_animation_GR2 = NULL;

	m_embedAnimation = FALSE;
	m_animationName = NULL;
	m_blendInTime = 0.0f;
	m_blendOutTime = 0.0f;
	m_baseTimeMultiplier = 1.0f;

	m_motionChannel = 0xFFFFFFFF;
	m_timeMultiplier = 1.0f;

	m_loopTime = -1.0f;

	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Hierarchy::HandleEvent_Dispose, this, _1), NULL);
}

CRuController_Hierarchy::~CRuController_Hierarchy()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

IRuObject *CRuController_Hierarchy::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_Hierarchy();
	CRuController_Hierarchy *controller = static_cast<CRuController_Hierarchy *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_Controller::Clone(clonedObject);

	controller->m_embedAnimation = m_embedAnimation;

	if(m_animationName)
	{
		controller->m_animationName = ruNEW char [strlen(m_animationName) + 1];
		memcpy(controller->m_animationName, m_animationName, strlen(m_animationName) + 1);
	}

	if(m_animation)
	{
		controller->m_animation = m_animation;
		controller->m_animation->AddRef();
	}

	if(m_animation_GR2)
	{
		controller->m_animation_GR2 = m_animation_GR2;
		controller->m_animation_GR2->AddRef();
	}

	controller->m_blendInTime = m_blendInTime;
	controller->m_blendOutTime = m_blendOutTime;
	controller->m_baseTimeMultiplier = m_baseTimeMultiplier;

	return clonedObject;
}

BOOL CRuController_Hierarchy::SerializeFrom(IRuStream *stream)
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
	IRuEntity_Controller::SerializeFrom(stream);

	switch(version)
	{
		case 1:
			{
				// Read embedding setting
				stream->Read(&m_embedAnimation, sizeof(BOOL));

				if(m_embedAnimation)
				{
					// Attempt to read GR2 animation
					m_animation_GR2 = ruNEW CRuAnimation_GR2();
					if(m_animation_GR2->SerializeFrom(stream) == FALSE)
					{
						// GR2 load failed, load RuAnimation
						ruSAFE_RELEASE(m_animation_GR2);
						m_animation = LoadRuAnimation(stream);
					}

					// Read blend in time
					stream->Read(&m_blendInTime, sizeof(m_blendInTime));

					// Read blend out time
					stream->Read(&m_blendOutTime, sizeof(m_blendOutTime));
				}
				else
				{
					UINT32 nameLength;
					stream->Read(&nameLength, sizeof(nameLength));
					m_animationName = ruNEW char [nameLength];
					stream->Read(m_animationName, nameLength);

					// Read blend in time
					stream->Read(&m_blendInTime, sizeof(m_blendInTime));

					// Read blend out time
					stream->Read(&m_blendOutTime, sizeof(m_blendOutTime));
				}
			}

			break;

		case 2:
			{
				// Read embedding setting
				stream->Read(&m_embedAnimation, sizeof(BOOL));

				if(m_embedAnimation)
				{
					// Attempt to read GR2 animation
					m_animation_GR2 = ruNEW CRuAnimation_GR2();
					if(m_animation_GR2->SerializeFrom(stream) == FALSE)
					{
						// GR2 load failed, load RuAnimation
						ruSAFE_RELEASE(m_animation_GR2);
						m_animation = LoadRuAnimation(stream);
					}

					// Read blend in time
					stream->Read(&m_blendInTime, sizeof(m_blendInTime));

					// Read blend out time
					stream->Read(&m_blendOutTime, sizeof(m_blendOutTime));
				}
				else
				{
					UINT32 nameLength;
					stream->Read(&nameLength, sizeof(nameLength));
					m_animationName = ruNEW char [nameLength];
					stream->Read(m_animationName, nameLength);
#ifdef KALYDO
					g_ruResourceManager->BackgroundDownload(m_animationName);
#endif // KALYDO
					// Read blend in time
					stream->Read(&m_blendInTime, sizeof(m_blendInTime));

					// Read blend out time
					stream->Read(&m_blendOutTime, sizeof(m_blendOutTime));
				}

				// Read base time multiplier
				stream->Read(&m_baseTimeMultiplier, sizeof(REAL));
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_Hierarchy::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 2;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Controller::SerializeTo(stream);

	// Write embedding setting
	stream->Write(&m_embedAnimation, sizeof(BOOL));

	if(m_embedAnimation && (m_animation || m_animation_GR2))
	{
		if(m_animation_GR2)
		{
			// Write GR2 animation
			m_animation_GR2->SerializeTo(stream);
		}
		else
		{
			// Write animation
			SaveRuAnimation(stream, m_animation);
		}

		// Write blend in time
		stream->Write(&m_blendInTime, sizeof(m_blendInTime));

		// Write blend out time
		stream->Write(&m_blendOutTime, sizeof(m_blendOutTime));
	}
	else
	{
		// Write animation name length including NULL followed by the name
		if(m_animationName)
		{
			UINT32 nameLength = (UINT32)strlen(m_animationName) + 1;
			stream->Write(&nameLength, sizeof(nameLength));
			stream->Write(m_animationName, nameLength);
		}
		else
		{
			UINT32 nameLength = (UINT32)strlen("") + 1;
			stream->Write(&nameLength, sizeof(nameLength));
			stream->Write("", nameLength);
		}

		// Write blend in time
		stream->Write(&m_blendInTime, sizeof(m_blendInTime));

		// Write blend out time
		stream->Write(&m_blendOutTime, sizeof(m_blendOutTime));
	}

	// Write base time multiplier
	stream->Write(&m_baseTimeMultiplier, sizeof(REAL));

	return TRUE;
}

BOOL CRuController_Hierarchy::Update(REAL elapsedTime)
{
	if(InstanceAnimation())
	{
		if(m_animation)
		{
			if(m_enabled && m_targetInterpolator)
			{
				if(!m_applied)
				{
					m_targetInterpolator->PlayAnimation(m_animation, m_startTime, m_blendInTime, m_blendOutTime, m_loopTime);
					m_applied = TRUE;
				}
			}
		}
		else if(m_animation_GR2)
		{
			if(m_enabled && m_targetEntity)
			{
				if(!m_applied)
				{
/*
					CRuHierarchy_GR2 *gr2Hierarchy = static_cast<CRuHierarchy_GR2 *>(m_targetEntity);

					// not quite done, need to adjust start time
					granny_controlled_animation_builder *builder = GrannyBeginControlledAnimation(0.0f, m_animation_GR2->GetGR2Animation());

					GrannySetTrackGroupTarget(builder, 0, gr2Hierarchy->GetGR2ModelInstance());

					m_animation_Control = GrannyEndControlledAnimation(builder);

//					m_animation_Control = GrannyPlayControlledAnimation(0.0f, m_animation_GR2->GetGR2Animation(), gr2Hierarchy->GetGR2ModelInstance());

					if(m_loopTime >= 0)
						GrannySetControlLoopCount(m_animation_Control, 0);
					else
						GrannySetControlLoopCount(m_animation_Control, 1);
*/
					if(static_cast<CRuHierarchy_GR2 *>(m_targetEntity)->PlayAnimation(m_motionChannel, m_animation_GR2, m_loopTime / m_baseTimeMultiplier, m_baseTimeMultiplier * m_timeMultiplier))
					{
						m_applied = TRUE;
					}
				}
			}
		}
	}
/*
	// Advance GR2 animation control clock
	if(m_animation_Control)
	{
		REAL controlClock = GrannyGetControlClock(m_animation_Control);
		GrannySetControlClock(m_animation_Control, controlClock + elapsedTime);
	}
*/
	// Chain update call to base controller class
	return IRuEntity_Controller::Update(elapsedTime);
}

REAL CRuController_Hierarchy::GetDuration()
{
	// If animation is not instanced, instance it
	if(m_animation == NULL)
	{
		InstanceAnimation();
	}

	return IRuEntity_Controller::GetDuration();
}

IRuObject *CRuController_Hierarchy::GetTarget()
{
	if(m_targetInterpolator)
	{
		return m_targetInterpolator;
	}

	return m_targetEntity;
}

BOOL CRuController_Hierarchy::SetTarget(IRuObject *target)
{
	if(target->GetType() == CRuFrameHierarchy::Type())
	{
		// If we had a previous target, unregister it
		if(m_targetEntity)
		{
			m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		// Store target reference and setup event listener
		m_targetEntity = static_cast<CRuEntity *>(target);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Hierarchy::HandleEvent_Dispose, this, _1), NULL);

		// Store target interpolator reference
		m_targetInterpolator = static_cast<CRuFrameHierarchy *>(target)->GetInterpolator();

		return TRUE;
	}

	if(target->GetType() == CRuAnimationInterpolator::Type())
	{
		m_targetInterpolator = static_cast<CRuAnimationInterpolator *>(target);

		return TRUE;
	}

	if(target->GetType() == CRuHierarchy_GR2::Type())
	{
		m_targetEntity = static_cast<CRuEntity *>(target);

		return TRUE;
	}

	return FALSE;
}

const char *CRuController_Hierarchy::GetAnimationName()
{
	if(m_animationName == NULL)
	{
		return "";
	}

	return m_animationName;
}

REAL CRuController_Hierarchy::GetBlendInTime()
{
	return m_blendInTime;
}

REAL CRuController_Hierarchy::GetBlendOutTime()
{
	return m_blendOutTime;
}

UINT32 CRuController_Hierarchy::GetMotionChannel()
{
	return m_motionChannel;
}

void CRuController_Hierarchy::SetAnimation(const char *animationName)
{
	if(animationName)
	{
		// Release reference to existing animation
		ruSAFE_RELEASE(m_animation);

		// Release animation name
		delete[] m_animationName;
		m_animationName = NULL;

		// Copy animation name
		m_animationName = ruNEW char [strlen(animationName) + 1];
		memcpy(m_animationName, animationName, strlen(animationName) + 1);

		// Reset duration
		IRuEntity_Controller::SetDuration(0.0f);
	}
}

void CRuController_Hierarchy::SetAnimation(CRuAnimation *animation)
{
	if(animation)
	{
		// Release reference to existing animation
		ruSAFE_RELEASE(m_animation);

		// Release animation name
		delete[] m_animationName;
		m_animationName = NULL;

		// Force animation embedding
		m_embedAnimation = TRUE;
		m_animation = animation;
		m_animation->AddRef();

		// Update animation duration
		IRuEntity_Controller::SetDuration(m_animation->GetDuration());
	}
}

void CRuController_Hierarchy::SetAnimation(CRuAnimation_GR2 *animation)
{
	if(animation)
	{
		// Release reference to existing animation
		ruSAFE_RELEASE(m_animation_GR2);

		// Release animation name
		delete[] m_animationName;
		m_animationName = NULL;

		// Force animation embedding
		m_embedAnimation = TRUE;
		m_animation_GR2 = animation;
		m_animation_GR2->AddRef();

		// Update animation duration
		IRuEntity_Controller::SetDuration(m_animation_GR2->GetDuration());
	}
}

void CRuController_Hierarchy::SetBlendInTime(REAL blendInTime)
{
	m_blendInTime = blendInTime;
}

void CRuController_Hierarchy::SetBlendOutTime(REAL blendOutTime)
{
	m_blendOutTime = blendOutTime;
}

REAL CRuController_Hierarchy::GetBaseTimeMultiplier()
{
	return m_baseTimeMultiplier;
}

void CRuController_Hierarchy::SetBaseTimeMultiplier(REAL baseTimeMultiplier)
{
	m_baseTimeMultiplier = baseTimeMultiplier;
}

void CRuController_Hierarchy::SetMotionChannel(UINT32 motionChannel)
{
	m_motionChannel = motionChannel;
}

void CRuController_Hierarchy::SetTimeMultiplier(REAL timeMultiplier)
{
	m_timeMultiplier = timeMultiplier;

	// If controller is already active, apply the time multiplier change
	if(m_applied)
	{
		if(m_targetEntity && m_targetEntity->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
		{
			static_cast<CRuHierarchy_GR2 *>(m_targetEntity)->UpdateAnimationTimeMultiplier(m_motionChannel, m_baseTimeMultiplier * m_timeMultiplier);
		}
	}
}

BOOL CRuController_Hierarchy::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_targetEntity)
	{
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	// Reset target interpolator to null
	m_targetInterpolator = NULL;

	// Release reference to existing animation
	ruSAFE_RELEASE(m_animation);

	// Free the animation control
	if(m_animation_Control)
	{
		GrannyFreeControl(m_animation_Control);
		m_animation_Control = NULL;
	}

	// Free GR2 animation
	ruSAFE_RELEASE(m_animation_GR2);

	// Release animation name string
	delete[] m_animationName;

	return TRUE;
}

BOOL CRuController_Hierarchy::InstanceAnimation()
{
	if(m_animation || m_animation_GR2)
	{
		return TRUE;
	}

	if(m_animationName)
	{
		ruSAFE_RELEASE(m_animation_GR2);
		ruSAFE_RELEASE(m_animation);

		m_animation_GR2 = g_ruResourceManager->LoadAnimation_GR2(m_animationName);

		if(m_animation_GR2)
		{
			IRuEntity_Controller::SetDuration(m_animation_GR2->GetDuration());
			return TRUE;
		}

		// Load animation using resource manager
		m_animation = g_ruResourceManager->LoadAnimation(m_animationName);

		if(m_animation)
		{
			IRuEntity_Controller::SetDuration(m_animation->GetDuration());
			return TRUE;
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
