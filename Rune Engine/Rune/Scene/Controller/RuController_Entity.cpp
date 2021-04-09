#include "../../Scene/Controller/RuController_Entity.h"

#include "../../Helper/RuHelper_Entity.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_Entity, IRuEntity_Controller)
ruHEAP_DEFINE_SUBCLASS(CRuController_Entity, "CRuController_Entity", "IRuEntity_Controller")
ruCLASSGUID_DEFINE(CRuController_Entity, 0x026F1E8E, 0x033343a2, 0x9045E0FA, 0x572A984B)

// ************************************************************************************************************************************************************

CRuController_Entity::CRuController_Entity()
:	m_targetEntity(NULL),
	m_accumulatedT(0.0f),
	m_entityName(NULL),
	m_motionName(NULL),
	m_fadeInTime(0.0f),
	m_fadeOutTime(0.0f),
	m_attached(FALSE),
	m_active(FALSE),
	m_childEntity(NULL)
{
	// Override clock update
	m_overrideClockUpdate = TRUE;

	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Entity::HandleEvent_Dispose, this, _1), NULL);
}

CRuController_Entity::~CRuController_Entity()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

IRuObject *CRuController_Entity::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_Entity();
	CRuController_Entity *controller = static_cast<CRuController_Entity *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_Controller::Clone(clonedObject);

	controller->SetSpawnedEntityName(m_entityName);
	controller->SetSpawnedMotionName(m_motionName);
	controller->SetFadeInTime(m_fadeInTime);
	controller->SetFadeOutTime(m_fadeOutTime);

	return clonedObject;
}

BOOL CRuController_Entity::SerializeFrom(IRuStream *stream)
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
				UINT32 nameLength;

				stream->Read(&nameLength, sizeof(nameLength));
				m_entityName = ruNEW char [nameLength];
				stream->Read(m_entityName, nameLength);

				stream->Read(&nameLength, sizeof(nameLength));
				m_motionName = ruNEW char [nameLength];
				stream->Read(m_motionName, nameLength);

				stream->Read(&m_fadeInTime, sizeof(REAL));
				stream->Read(&m_fadeOutTime, sizeof(REAL));
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_Entity::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Controller::SerializeTo(stream);

	if(m_entityName)
	{
		UINT32 nameLength = (UINT32)strlen(m_entityName) + 1;
		stream->Write(&nameLength, sizeof(nameLength));
		stream->Write(m_entityName, nameLength);
	}
	else
	{
		UINT32 nameLength = (UINT32)strlen("") + 1;
		stream->Write(&nameLength, sizeof(nameLength));
		stream->Write("", nameLength);
	}

	if(m_motionName)
	{
		UINT32 nameLength = (UINT32)strlen(m_motionName) + 1;
		stream->Write(&nameLength, sizeof(nameLength));
		stream->Write(m_motionName, nameLength);
	}
	else
	{
		UINT32 nameLength = (UINT32)strlen("") + 1;
		stream->Write(&nameLength, sizeof(nameLength));
		stream->Write("", nameLength);
	}

	stream->Write(&m_fadeInTime, sizeof(REAL));
	stream->Write(&m_fadeOutTime, sizeof(REAL));

	return TRUE;
}

BOOL CRuController_Entity::Update(REAL elapsedTime)
{
	if(m_enabled)
	{
		// Accumulate elapsed time and maintain update at 30 Hz
		m_accumulatedT += elapsedTime;

		if(m_accumulatedT >= 0.033f)
		{
			// Update age
			m_age += m_accumulatedT;
			m_accumulatedT = 0.0f;

			// Trigger check
			if(m_age > (m_startTime + m_duration))
			{
				// Detach the spawned entity
				if(m_attached && m_childEntity)
				{
					m_targetEntity->RemoveEntity(m_childEntity);
					m_attached = FALSE;
				}

				// If this is a looping event, rewind time
				if(m_loopTime >= 0.0f)
				{
					m_age = m_age - (m_duration + m_loopTime);
				}
			}
			else if(m_age > m_startTime)
			{
				if(m_targetEntity)
				{
					if(m_childEntity == NULL)
					{
						m_childEntity = g_ruResourceManager->LoadEntity(m_entityName);
					}

					if(!m_attached && m_childEntity)
					{
						// Attach the spawned entity
						m_targetEntity->AddEntity(m_childEntity);
						m_attached = TRUE;

						// Reset motion on the spawned entity
						RuACT_PlayMotionEx(m_childEntity, 0xFFFFFFFF, m_motionName, 0.0f, TRUE);
					}
				}
			}
		}
	}

	// Chain update call to base controller class
	return IRuEntity_Controller::Update(elapsedTime);
}

IRuObject *CRuController_Entity::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_Entity::SetTarget(IRuObject *targetEntity)
{
	if(targetEntity->GetType().IsSubClassOf(IRuEntity_Container_Mutable::Type()))
	{
		// If we had a previous target, unregister it
		if(m_targetEntity)
		{
			m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		m_targetEntity = static_cast<IRuEntity_Container_Mutable *>(targetEntity);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Entity::HandleEvent_Dispose, this, _1), NULL);
	}

	return TRUE;
}

BOOL CRuController_Entity::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	delete[] m_entityName;
	delete[] m_motionName;

	m_entityName = NULL;
	m_motionName = NULL;

	// Detach from the target entity
	if(m_attached)
	{
		m_targetEntity->RemoveEntity(m_childEntity);
	}

	if(m_targetEntity)
	{
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	// Release reference to the spawned entity
	ruSAFE_RELEASE(m_childEntity);

	return TRUE;
}

const char *CRuController_Entity::GetSpawnedEntityName()
{
	return m_entityName;
}

const char *CRuController_Entity::GetSpawnedMotionName()
{
	return m_motionName;
}

REAL CRuController_Entity::GetFadeInTime()
{
	return m_fadeInTime;
}

REAL CRuController_Entity::GetFadeOutTime()
{
	return m_fadeOutTime;
}

void CRuController_Entity::SetSpawnedEntityName(const char *entityName)
{
	delete[] m_entityName;

	if(entityName)
	{
		m_entityName = ruNEW char [strlen(entityName) + 1];
		memcpy(m_entityName, entityName, strlen(entityName) + 1);
	}
	else
	{
		m_entityName = NULL;
	}
}

void CRuController_Entity::SetSpawnedMotionName(const char *motionName)
{
	delete[] m_motionName;

	if(motionName)
	{
		m_motionName = ruNEW char [strlen(motionName) + 1];
		memcpy(m_motionName, motionName, strlen(motionName) + 1);
	}
	else
	{
		m_motionName = NULL;
	}
}

void CRuController_Entity::SetFadeInTime(REAL fadeInTime)
{
	m_fadeInTime = fadeInTime;
}

void CRuController_Entity::SetFadeOutTime(REAL fadeOutTime)
{
	m_fadeOutTime = fadeOutTime;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
