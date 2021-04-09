#include "../../Scene/Controller/RuController_Motion.h"
#include "../../Helper/RuHelper_Entity.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_Motion, IRuEntity_KeyableController)
ruHEAP_DEFINE_SUBCLASS(CRuController_Motion, "CRuController_Motion", "IRuEntity_KeyableController")
ruCLASSGUID_DEFINE(CRuController_Motion, 0xB5ADCDA2, 0x0F514f54, 0xB11B6BE3, 0x3D017B42)

// ************************************************************************************************************************************************************

CRuController_Motion::CRuController_Motion()
:	m_targetEntity(NULL)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Motion::HandleEvent_Dispose, this, _1), NULL);

	// Setup tracks
	m_track_Motion.SetTrackName("Motion");
}

CRuController_Motion::~CRuController_Motion()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

IRuObject *CRuController_Motion::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_Motion();
	CRuController_Motion *controller = static_cast<CRuController_Motion *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_KeyableController::Clone(clonedObject);

	// Clone properties

	// Clone tracks
	CloneTracks(controller);

	return clonedObject;
}

BOOL CRuController_Motion::SerializeFrom(IRuStream *stream)
{
	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	CRuGUID classGUID;

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

				// Serialize tracks
				m_track_Motion.SerializeFrom(stream);
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_Motion::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_KeyableController::SerializeTo(stream);

	// Write properties

	// Serialize tracks
	m_track_Motion.SerializeTo(stream);

	return TRUE;
}

BOOL CRuController_Motion::Update(REAL elapsedTime)
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

			// Get motion name and play motion
			const char *motionName = m_track_Motion.GetDiscreteKey(relativeT);
			RuACT_PlayMotionEx(m_targetEntity, 0xFFFFFFFF, motionName, 0.0f, FALSE, 1.0f, 0.0f, 0.0f, 0.0f, NULL, TRUE);
		}
	}

	// Chain update call to base controller class
	return IRuEntity_KeyableController::Update(elapsedTime);
}

IRuObject *CRuController_Motion::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_Motion::SetTarget(IRuObject *target)
{
	if(target->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		// If we had a previous target, unregister it
		if(m_targetEntity)
		{
			m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		m_targetEntity = static_cast<CRuEntity *>(target);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Motion::HandleEvent_Dispose, this, _1), NULL);

		return TRUE;
	}

	return FALSE;
}

INT32 CRuController_Motion::GetNumTracks() const
{
	return 1;
}

BOOL CRuController_Motion::GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut)
{
	switch(trackIndex)
	{
		case 0:
			trackOut = &m_track_Motion;
			return TRUE;
	}

	return FALSE;
}

BOOL CRuController_Motion::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_targetEntity)
	{
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
