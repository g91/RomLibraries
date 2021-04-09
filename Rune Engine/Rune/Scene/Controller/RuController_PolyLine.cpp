#include "../../Scene/Controller/RuController_PolyLine.h"
#include "../../Scene/FX/RuEntity_PolyLine.h"
#include "../../Helper/RuHelper_Entity.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_PolyLine, IRuEntity_KeyableController)
ruHEAP_DEFINE_SUBCLASS(CRuController_PolyLine, "CRuController_PolyLine", "IRuEntity_KeyableController")
ruCLASSGUID_DEFINE(CRuController_PolyLine, 0xAFBA604E, 0x42B74e26, 0xB36BE543, 0x7B73B563)

// ************************************************************************************************************************************************************

CRuController_PolyLine::CRuController_PolyLine()
:	m_targetEntity(NULL),
	m_prevTargetName(NULL)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_PolyLine::HandleEvent_Dispose, this, _1), NULL);

	// Setup track names
	m_track_Target.SetTrackName("Target");
	m_track_SegmentLength.SetTrackName("Segment Length");
	m_track_SegmentWidth.SetTrackName("Segment Width");
	m_track_Deviation.SetTrackName("Deviation");
	m_track_UVRatio.SetTrackName("UV Ratio");
}

CRuController_PolyLine::~CRuController_PolyLine()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

IRuObject *CRuController_PolyLine::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_PolyLine();
	CRuController_PolyLine *controller = static_cast<CRuController_PolyLine *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_KeyableController::Clone(clonedObject);

	// Clone tracks
	CloneTracks(controller);

	return clonedObject;
}

BOOL CRuController_PolyLine::SerializeFrom(IRuStream *stream)
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
				// Serialize tracks
				m_track_Target.SerializeFrom(stream);
				m_track_SegmentLength.SerializeFrom(stream);
				m_track_SegmentWidth.SerializeFrom(stream);
				m_track_Deviation.SerializeFrom(stream);
				m_track_UVRatio.SerializeFrom(stream);
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_PolyLine::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_KeyableController::SerializeTo(stream);

	// Serialize tracks
	m_track_Target.SerializeTo(stream);
	m_track_SegmentLength.SerializeTo(stream);
	m_track_SegmentWidth.SerializeTo(stream);
	m_track_Deviation.SerializeTo(stream);
	m_track_UVRatio.SerializeTo(stream);

	return TRUE;
}

BOOL CRuController_PolyLine::Update(REAL elapsedTime)
{
	if(m_targetEntity && m_enabled)
	{
		REAL relativeT = m_age - m_startTime;

		if(relativeT >= 0.0f)
		{
			// If looping is enabled, clamp time range to the duration
			if(relativeT > m_duration && m_duration > 0.0f && m_loopTime >= 0.0f)
			{
				relativeT = fmod(relativeT, m_duration);
			}

			// Set basic poly line properties
			static_cast<CRuEntity_PolyLine *>(m_targetEntity)->SetSegmentLength(m_track_SegmentLength.GetInterpolatedKey(relativeT));
			static_cast<CRuEntity_PolyLine *>(m_targetEntity)->SetSegmentWidth(m_track_SegmentWidth.GetInterpolatedKey(relativeT));
			static_cast<CRuEntity_PolyLine *>(m_targetEntity)->SetDeviation(m_track_Deviation.GetInterpolatedKey(relativeT));
			static_cast<CRuEntity_PolyLine *>(m_targetEntity)->SetUVRatio(m_track_UVRatio.GetInterpolatedKey(relativeT));

			// Find the entity specified by target name
			char *targetName = m_track_Target.GetDiscreteKey(relativeT);
			if(targetName)
			{
				if(targetName != m_prevTargetName)
				{
					m_prevTargetName = targetName;

					if(strlen(targetName) > 0)
					{
						CRuEntity *closestTargetMatch = RuEntity_FindNearestEntityByName(m_targetEntity, targetName);

						if(closestTargetMatch)
						{
							static_cast<CRuEntity_PolyLine *>(m_targetEntity)->SetTarget(closestTargetMatch);
						}
					}
				}
			}
		}
	}

	// Chain update call to base controller class
	return IRuEntity_KeyableController::Update(elapsedTime);
}

IRuObject *CRuController_PolyLine::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_PolyLine::SetTarget(IRuObject *target)
{
	if(target->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		// If we had a previous target, unregister it
		if(m_targetEntity)
		{
			m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		m_targetEntity = static_cast<CRuEntity *>(target);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_PolyLine::HandleEvent_Dispose, this, _1), NULL);

		return TRUE;
	}

	return FALSE;
}

INT32 CRuController_PolyLine::GetNumTracks() const
{
	return 5;
}

BOOL CRuController_PolyLine::GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut)
{
	switch(trackIndex)
	{
		case 0:
			trackOut = &m_track_Target;
			return TRUE;

		case 1:
			trackOut = &m_track_SegmentLength;
			return TRUE;

		case 2:
			trackOut = &m_track_SegmentWidth;
			return TRUE;

		case 3:
			trackOut = &m_track_Deviation;
			return TRUE;

		case 4:
			trackOut = &m_track_UVRatio;
			return TRUE;
	}

	return FALSE;
}

BOOL CRuController_PolyLine::HandleEvent_Dispose(RuEventArgs *eventArgs)
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
