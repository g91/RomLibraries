#include "../../Scene/Controller/RuController_Particle_Visualizer_Entity.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_Particle_Visualizer_Entity, IRuEntity_KeyableController)
ruHEAP_DEFINE_SUBCLASS(CRuController_Particle_Visualizer_Entity, "CRuController_Particle_Visualizer_Entity", "IRuEntity_KeyableController")
ruCLASSGUID_DEFINE(CRuController_Particle_Visualizer_Entity, 0x7EAB3980, 0xD0FC484c, 0xB63312DD, 0x28ABB07E)

// ************************************************************************************************************************************************************

CRuController_Particle_Visualizer_Entity::CRuController_Particle_Visualizer_Entity()
:	m_entityName(NULL),
	m_orientToDirectionOfTravel(FALSE),
	m_particleVisualizer(NULL)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Particle_Visualizer_Entity::HandleEvent_Dispose, this, _1), NULL);

	m_track_MotionName.InsertKey(0.0f, "");
	m_track_MotionName.InsertKey(1.0f, "");
	m_track_MotionName.SetTrackName("Motion Name");

	// Insert two dummy keys for start & end color
	CRuColor4 color(1.0f, 1.0f, 1.0f, 1.0f);
	m_track_Color.InsertKey(0.0f, color);
	m_track_Color.InsertKey(1.0f, color);
	m_track_Color.SetTrackName("Color");

	REAL alpha = 1.0f;
	m_track_Alpha.InsertKey(0.0f, alpha);
	m_track_Alpha.InsertKey(1.0f, alpha);
	m_track_Alpha.SetTrackName("Alpha");

	REAL scale = 1.0f;
	m_track_Scale.InsertKey(0.0f, scale);
	m_track_Scale.InsertKey(1.0f, scale);
	m_track_Scale.SetTrackName("Scale");

	// Set default duration to 1 second
	SetDuration(1.0f);
}

CRuController_Particle_Visualizer_Entity::~CRuController_Particle_Visualizer_Entity()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

IRuObject *CRuController_Particle_Visualizer_Entity::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_Particle_Visualizer_Entity();
	CRuController_Particle_Visualizer_Entity *controller = static_cast<CRuController_Particle_Visualizer_Entity *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_KeyableController::Clone(clonedObject);

	// Clone parameters
	controller->SetEntityName(m_entityName);
	controller->m_orientToDirectionOfTravel = m_orientToDirectionOfTravel;

	// Clone tracks
	CloneTracks(controller);

	return clonedObject;
}

BOOL CRuController_Particle_Visualizer_Entity::SerializeFrom(IRuStream *stream)
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
				// Read length of entity name including NULL, followed by the name
				INT32 len = 0;
				stream->Read(&len, sizeof(INT32));
				m_entityName = ruNEW char [len];
				stream->Read(m_entityName, len);

				// Serialize tracks
				m_track_MotionName.SerializeFrom(stream);
				m_track_Color.SerializeFrom(stream);
				m_track_Alpha.SerializeFrom(stream);
				m_track_Scale.SerializeFrom(stream);
			}

			break;

		case 2:
			{
				// Read length of entity name including NULL, followed by the name
				INT32 len = 0;
				stream->Read(&len, sizeof(INT32));
				m_entityName = ruNEW char [len];
				stream->Read(m_entityName, len);

				stream->Read(&m_orientToDirectionOfTravel, sizeof(BOOL));

				// Serialize tracks
				m_track_MotionName.SerializeFrom(stream);
				m_track_Color.SerializeFrom(stream);
				m_track_Alpha.SerializeFrom(stream);
				m_track_Scale.SerializeFrom(stream);
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_Particle_Visualizer_Entity::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 2;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_KeyableController::SerializeTo(stream);

	// Write length of entity name including NULL, followed by the name
	if(m_entityName)
	{
		INT32 len = (INT32)strlen(m_entityName) + 1;
		stream->Write(&len, sizeof(INT32));
		stream->Write(m_entityName, len);
	}
	else
	{
		INT32 len = 1;
		stream->Write(&len, sizeof(INT32));
		stream->Write("", len);
	}

	stream->Write(&m_orientToDirectionOfTravel, sizeof(BOOL));

	// Serialize tracks
	m_track_MotionName.SerializeTo(stream);
	m_track_Color.SerializeTo(stream);
	m_track_Alpha.SerializeTo(stream);
	m_track_Scale.SerializeTo(stream);

	return TRUE;
}

BOOL CRuController_Particle_Visualizer_Entity::Update(REAL elapsedTime)
{
	if(m_enabled)
	{
	}

	// Chain update call to base controller class
	return IRuEntity_KeyableController::Update(elapsedTime);
}

IRuObject *CRuController_Particle_Visualizer_Entity::GetTarget()
{
	return m_particleVisualizer;
}

BOOL CRuController_Particle_Visualizer_Entity::SetTarget(IRuObject *targetEntity)
{
	if(targetEntity->GetType() == CRuEntity_Particle_Visualizer_Entity::Type())
	{
		// If we had a previous target, unregister it
		if(m_particleVisualizer)
		{
			m_particleVisualizer->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		// Store target reference, setup event listener, and setup target parameter block
		m_particleVisualizer = static_cast<CRuEntity_Particle_Visualizer_Entity *>(targetEntity);
		m_particleVisualizer->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Particle_Visualizer_Entity::HandleEvent_Dispose, this, _1), NULL);
	}

	if(m_particleVisualizer)
	{
		m_particleVisualizer->SetEntityName(m_entityName);
		m_particleVisualizer->SetOrientToDirectionOfTravel(m_orientToDirectionOfTravel);
		m_track_MotionName.CopyTo(&m_particleVisualizer->GetTrack_MotionName());
		m_track_Color.CopyTo(&m_particleVisualizer->GetTrack_Color());
		m_track_Alpha.CopyTo(&m_particleVisualizer->GetTrack_Alpha());
		m_track_Scale.CopyTo(&m_particleVisualizer->GetTrack_Scale());
	}

	return TRUE;
}

INT32 CRuController_Particle_Visualizer_Entity::GetNumTracks() const
{
	return 4;
}

BOOL CRuController_Particle_Visualizer_Entity::GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut)
{
	switch(trackIndex)
	{
		case 0:
			trackOut = &m_track_MotionName;
			return TRUE;

		case 1:
			trackOut = &m_track_Color;
			return TRUE;

		case 2:
			trackOut = &m_track_Alpha;
			return TRUE;

		case 3:
			trackOut = &m_track_Scale;
			return TRUE;
	}

	return FALSE;
}

BOOL CRuController_Particle_Visualizer_Entity::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_particleVisualizer)
	{
		// Unregister from target entity
		m_particleVisualizer->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_particleVisualizer = NULL;
	}

	delete[] m_entityName;

	return TRUE;
}

const char *CRuController_Particle_Visualizer_Entity::GetEntityName() const
{
	if(m_entityName)
	{
		return m_entityName;
	}

	return "";
}

void CRuController_Particle_Visualizer_Entity::SetEntityName(const char *entityName)
{
	if(entityName)
	{
		if(m_entityName)
		{
			delete[] m_entityName;
		}

		m_entityName = ruNEW char [strlen(entityName) + 1];
		strcpy(m_entityName, entityName);
	}
}

BOOL CRuController_Particle_Visualizer_Entity::GetOrientToDirectionOfTravel() const
{
	return m_orientToDirectionOfTravel;
}

void CRuController_Particle_Visualizer_Entity::SetOrientToDirectionOfTravel(BOOL orientToDirectionOfTravel)
{
	m_orientToDirectionOfTravel = orientToDirectionOfTravel;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
