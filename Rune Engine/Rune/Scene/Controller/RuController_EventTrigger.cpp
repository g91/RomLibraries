#include "../../Scene/Controller/RuController_EventTrigger.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_EventTrigger, IRuEntity_Controller)
ruHEAP_DEFINE_SUBCLASS(CRuController_EventTrigger, "CRuController_EventTrigger", "IRuEntity_Controller")
ruCLASSGUID_DEFINE(CRuController_EventTrigger, 0x02C2410D, 0x745B4e57, 0x967E7096, 0x5DB772FA)

// ************************************************************************************************************************************************************

CRuController_EventTrigger::CRuController_EventTrigger()
:	m_targetEntity(NULL),
	m_accumulatedT(0.0f),
	m_eventID(0),
	m_eventParameter0(0),
	m_eventParameter1(0),
	m_eventString(NULL)
{
	m_overrideClockUpdate = TRUE;

	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_EventTrigger::HandleEvent_Dispose, this, _1), NULL);
}

CRuController_EventTrigger::~CRuController_EventTrigger()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

IRuObject *CRuController_EventTrigger::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_EventTrigger();
	CRuController_EventTrigger *controller = static_cast<CRuController_EventTrigger *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_Controller::Clone(clonedObject);

	controller->m_eventID = m_eventID;
	controller->m_eventParameter0 = m_eventParameter0;
	controller->m_eventParameter1 = m_eventParameter1;

	if(m_eventString)
	{
		controller->SetEventString(m_eventString);
	}
	else
	{
		controller->m_eventString = NULL;
	}

	controller->m_eventDuration = m_eventDuration;

	return clonedObject;
}

BOOL CRuController_EventTrigger::SerializeFrom(IRuStream *stream)
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
				// Read event ID
				stream->Read(&m_eventID, sizeof(m_eventID));

				// Read event parameter 0
				stream->Read(&m_eventParameter0, sizeof(m_eventParameter0));

				// Read event parameter 1
				stream->Read(&m_eventParameter1, sizeof(m_eventParameter1));

				// Read event duration
				stream->Read(&m_eventDuration, sizeof(m_eventDuration));
			}

			break;

		case 2:
			{
				// Read event ID
				stream->Read(&m_eventID, sizeof(m_eventID));

				// Read event parameter 0
				stream->Read(&m_eventParameter0, sizeof(m_eventParameter0));

				// Read event parameter 1
				stream->Read(&m_eventParameter1, sizeof(m_eventParameter1));

				// Read event string
				INT32 eventStringLength;
				stream->Read(&eventStringLength, sizeof(INT32));
				m_eventString = ruNEW char [eventStringLength];
				stream->Read(m_eventString, eventStringLength);

#ifdef KALYDO
				// predownload all sounds
				switch(m_eventID)
				{
				case ruENTITYEVENT_AUDIO_DIRECT:
				case ruENTITYEVENT_AUDIO_3D_DIRECT:
				//case ruENTITYEVENT_AUDIO_TAGGED:
				//case ruENTITYEVENT_AUDIO_3D_TAGGED:
					{
						g_ruResourceManager->BackgroundDownload(m_eventString);
						break;
					}
				default:
					break;
				}

#endif // KALYDO
				// Read event duration
				stream->Read(&m_eventDuration, sizeof(m_eventDuration));
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_EventTrigger::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 2;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Controller::SerializeTo(stream);

	// Write event ID
	stream->Write(&m_eventID, sizeof(m_eventID));

	// Write event parameter 0
	stream->Write(&m_eventParameter0, sizeof(m_eventParameter0));

	// Write event parameter 1
	stream->Write(&m_eventParameter1, sizeof(m_eventParameter1));

	// Write event string
	if(m_eventString)
	{
		INT32 eventStringLength = (INT32)strlen(m_eventString) + 1;
		stream->Write(&eventStringLength, sizeof(INT32));
		stream->Write(m_eventString, eventStringLength);
	}
	else
	{
		INT32 eventStringLength = (INT32)strlen("") + 1;
		stream->Write(&eventStringLength, sizeof(INT32));
		stream->Write("", eventStringLength);
	}

	// Write event duration
	stream->Write(&m_eventDuration, sizeof(m_eventDuration));

	return TRUE;
}

BOOL CRuController_EventTrigger::Update(REAL elapsedTime)
{
	if(m_enabled)
	{
		// Accumulate elapsed time and maintain update at 30 Hz
		m_accumulatedT += elapsedTime;

		if(m_accumulatedT >= 0.033f)
		{
			// Trigger check
			if(m_age <= m_startTime)
			{
				// Update age
				m_age += m_accumulatedT;
				m_accumulatedT = 0.0f;

				// If we have hit the trigger time, raise the event
				if(m_age > m_startTime)
				{
					if(m_targetEntity)
					{
						RuEntityEvent_Trigger_Args triggerArgs(m_targetEntity, this, m_eventID, m_eventParameter0, m_eventParameter1, m_eventString, m_eventDuration);
						m_targetEntity->Event_Trigger().Trigger(&triggerArgs);
					}

					// If this is a looping event, rewind time
					if(m_loopTime >= 0.0f)
					{
						m_age = m_age - (m_duration + m_loopTime);
					}
				}
			}
			else
			{
				// Update age
				m_age += m_accumulatedT;
				m_accumulatedT = 0.0f;
			}
		}
	}

	// Chain update call to base controller class
	return IRuEntity_Controller::Update(elapsedTime);
}

REAL CRuController_EventTrigger::SetDuration(REAL duration)
{
	m_eventDuration = duration;
	return IRuEntity_Controller::SetDuration(duration);
}

IRuObject *CRuController_EventTrigger::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_EventTrigger::SetTarget(IRuObject *targetEntity)
{
	if(targetEntity->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		// If we had a previous target, unregister it
		if(m_targetEntity)
		{
			m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		// Store target reference and setup event listener
		m_targetEntity = static_cast<CRuEntity *>(targetEntity);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_EventTrigger::HandleEvent_Dispose, this, _1), NULL);
	}

	return TRUE;
}

BOOL CRuController_EventTrigger::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_targetEntity)
	{
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	delete[] m_eventString;

	return TRUE;
}

INT32 CRuController_EventTrigger::GetEventID() const
{
	return m_eventID;
}

INT32 CRuController_EventTrigger::GetEventParameter0() const
{
	return m_eventParameter0;
}

INT32 CRuController_EventTrigger::GetEventParameter1() const
{
	return m_eventParameter1;
}

const char *CRuController_EventTrigger::GetEventString() const
{
	if(m_eventString)
	{
		return m_eventString;
	}

	return "";
}

REAL CRuController_EventTrigger::GetEventDuration() const
{
	return m_eventDuration;
}

void CRuController_EventTrigger::SetEventID(INT32 eventID)
{
	m_eventID = eventID;
}

void CRuController_EventTrigger::SetEventParameter0(INT32 eventParameter0)
{
	m_eventParameter0 = eventParameter0;
}

void CRuController_EventTrigger::SetEventParameter1(INT32 eventParameter1)
{
	m_eventParameter1 = eventParameter1;
}

void CRuController_EventTrigger::SetEventString(const char *eventString)
{
	delete[] m_eventString;

	m_eventString = ruNEW char [strlen(eventString) + 1];
	strcpy(m_eventString, eventString);
}

void CRuController_EventTrigger::SetEventDuration(REAL eventDuration)
{
	m_eventDuration = eventDuration;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
