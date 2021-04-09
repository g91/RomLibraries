/*!
	@file RuEntity_Particle_Visualizer_Entity.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Scene/Particle/RuEntity_Particle_Visualizer_Entity.h"

#include "../../Helper/RuHelper_Entity.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Particle_Visualizer_Entity, IRuEntity_Particle_Visualizer)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Particle_Visualizer_Entity, "CRuEntity_Particle_Visualizer_Entity", "IRuEntity_Particle_Visualizer")
ruCLASSGUID_DEFINE(CRuEntity_Particle_Visualizer_Entity, 0x25A2C9CB, 0x9DA5423c, 0xBEE852AF, 0x14878DF0)

// ************************************************************************************************************************************************************

CRuEntity_Particle_Visualizer_Entity::CRuEntity_Particle_Visualizer_Entity()
:	m_entityName(NULL),
	m_orientToDirectionOfTravel(FALSE),
	m_emitter(NULL),
	m_particleEntity(NULL),
	m_timeSinceLastUpdate(1000000.0f),
	m_updateInterval(0.010f),
	m_iterationCount(0)
{
	m_track_MotionName.InsertKey(0.0f, "");
	m_track_MotionName.InsertKey(1.0f, "");

	// Insert two dummy keys for start & end color
	CRuColor4 color(1.0f, 1.0f, 1.0f, 1.0f);
	m_track_Color.InsertKey(0.0f, color);
	m_track_Color.InsertKey(1.0f, color);

	REAL alpha = 1.0f;
	m_track_Alpha.InsertKey(0.0f, alpha);
	m_track_Alpha.InsertKey(1.0f, alpha);

	REAL scale = 1.0f;
	m_track_Scale.InsertKey(0.0f, scale);
	m_track_Scale.InsertKey(1.0f, scale);

	// Disable collision detection by default
	this->SetOption(ruENTITYOPTION_COLLISION, FALSE);
}

CRuEntity_Particle_Visualizer_Entity::~CRuEntity_Particle_Visualizer_Entity()
{
	// Release entity name string
	delete[] m_entityName;

	// Release member reference to the particle emitter
	ruSAFE_RELEASE(m_emitter);

	// Release member reference to the particle entity
	ruSAFE_RELEASE(m_particleEntity);

	// Release resources used by entity stack
	FlushParticleStack();
}

IRuObject *CRuEntity_Particle_Visualizer_Entity::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuEntity_Particle_Visualizer_Entity();

	// Chain cloning call to parent class
	IRuEntity_Particle_Visualizer::Clone(clonedObject);

	// Shortcut reference to the cloned entity
	CRuEntity_Particle_Visualizer_Entity *clonedPSys = static_cast<CRuEntity_Particle_Visualizer_Entity *>(clonedObject);

	// Copy visualizer settings
	if(m_entityName)
	{
		clonedPSys->m_entityName = ruNEW char [strlen(m_entityName) + 1];
		strcpy(clonedPSys->m_entityName, m_entityName);
	}

	m_track_MotionName.CopyTo(&clonedPSys->m_track_MotionName);
	m_track_Color.CopyTo(&clonedPSys->m_track_Color);
	m_track_Alpha.CopyTo(&clonedPSys->m_track_Alpha);
	m_track_Scale.CopyTo(&clonedPSys->m_track_Scale);

	// Clone the emitter
	if(m_emitter)
	{
		IRuObject *clonedEmitter = m_emitter->Clone();
		if(clonedEmitter && clonedEmitter->GetType().IsSubClassOf(IRuEntity_Particle_Emitter::Type()))
		{
			clonedPSys->m_emitter = static_cast<IRuEntity_Particle_Emitter *>(clonedEmitter);

			// Disable automatic cloning and serialization
			clonedPSys->m_emitter->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
			clonedPSys->m_emitter->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);
			clonedPSys->m_emitter->SetProperty("act:no enumerate", TRUE);

			clonedPSys->AddChild(clonedPSys->m_emitter);
		}
	}

	// Clone the particle entity
	if(m_particleEntity)
	{
		IRuObject *clonedParticleEntity = m_particleEntity->Clone();
		if(clonedParticleEntity && clonedParticleEntity->GetType().IsSubClassOf(CRuEntity::Type()))
		{
			clonedPSys->m_particleEntity = static_cast<CRuEntity *>(clonedParticleEntity);
		}
	}

	return clonedPSys;
}

BOOL CRuEntity_Particle_Visualizer_Entity::SerializeFrom(IRuStream *stream)
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

	if(version >= 3)
	{
		// Chain serialization to base class
		IRuEntity_Particle_Visualizer::SerializeFrom(stream);
	}

	switch(version)
	{
		case 1:
			{
				// Read length of entity name including NULL, followed by the name
				INT32 len = 0;
				stream->Read(&len, sizeof(INT32));
				m_entityName = ruNEW char [len];
				stream->Read(m_entityName, len);

				// Read length of motion name including NULL, followed by the name
				stream->Read(&len, sizeof(INT32));
				char *motionName = ruNEW char [len];
				stream->Read(motionName, len);

				// Read color settings
				DWORD startColor, endColor;
				REAL startScale, endScale;
				stream->Read(&startColor, sizeof(DWORD));
				stream->Read(&endColor, sizeof(DWORD));
				stream->Read(&startScale, sizeof(REAL));
				stream->Read(&endScale, sizeof(REAL));

				m_track_MotionName.SetKeyByIndex(0, motionName);
				m_track_MotionName.SetKeyByIndex(1, motionName);

				m_track_Color.SetKeyByIndex(0, CRuColor4(startColor));
				m_track_Color.SetKeyByIndex(1, CRuColor4(endColor));

				m_track_Alpha.SetKeyByIndex(0, (startColor >> 24) / 255.0f);
				m_track_Alpha.SetKeyByIndex(1, (endColor >> 24) / 255.0f);

				m_track_Scale.SetKeyByIndex(0, startScale);
				m_track_Scale.SetKeyByIndex(1, endScale);

				delete[] motionName;

				// If particle emitter is available, serialize particle emitter
				BOOL emitterAvailable = FALSE;
				stream->Read(&emitterAvailable, sizeof(BOOL));

				if(emitterAvailable)
				{
					IRuObject *object = RuObjectFactory().CreateObject(stream);
					if(object && object->GetType().IsSubClassOf(IRuEntity_Particle_Emitter::Type()))
					{
						m_emitter = static_cast<IRuEntity_Particle_Emitter *>(object);

						// Disable automatic cloning and serialization
						m_emitter->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
						m_emitter->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);
						m_emitter->SetProperty("act:no enumerate", TRUE);

						this->AddChild(m_emitter);
					}
					else
					{
						ruSAFE_RELEASE(object);
					}
				}
			}

			break;

		case 2:
		case 3:
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

				// If particle emitter is available, serialize particle emitter
				BOOL emitterAvailable = FALSE;
				stream->Read(&emitterAvailable, sizeof(BOOL));

				if(emitterAvailable)
				{
					IRuObject *object = RuObjectFactory().CreateObject(stream);
					if(object && object->GetType().IsSubClassOf(IRuEntity_Particle_Emitter::Type()))
					{
						m_emitter = static_cast<IRuEntity_Particle_Emitter *>(object);

						// Disable automatic cloning and serialization
						m_emitter->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
						m_emitter->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);
						m_emitter->SetProperty("act:no enumerate", TRUE);

						this->AddChild(m_emitter);
					}
					else
					{
						ruSAFE_RELEASE(object);
					}
				}
			}

			break;
	}

	// Load the particle entity
	ReloadParticleEntity();

	return TRUE;
}

BOOL CRuEntity_Particle_Visualizer_Entity::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 3;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Particle_Visualizer::SerializeTo(stream);

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

	// Serialize tracks
	m_track_MotionName.SerializeTo(stream);
	m_track_Color.SerializeTo(stream);
	m_track_Alpha.SerializeTo(stream);
	m_track_Scale.SerializeTo(stream);

	// If particle emitter is available, serialize particle emitter
	if(m_emitter)
	{
		BOOL emitterAvailable = TRUE;
		stream->Write(&emitterAvailable, sizeof(BOOL));
		m_emitter->SerializeTo(stream);
	}
	else
	{
		BOOL emitterAvailable = FALSE;
		stream->Write(&emitterAvailable, sizeof(BOOL));
	}

	return TRUE;
}

BOOL CRuEntity_Particle_Visualizer_Entity::ResetStates()
{
	for(INT32 i = 0; i < m_entities.Count(); ++i)
	{
		if(m_entities[i].m_entity)
		{
			m_entities[i].m_entity->SetOption(ruENTITYOPTION_HIDDEN, TRUE);

//			m_entities[i].m_entity->DetachFromParent();
//			ruSAFE_RELEASE(m_entities[i].m_entity);
		}
	}

	return IRuEntity_Particle_Visualizer::ResetStates();
}

BOOL CRuEntity_Particle_Visualizer_Entity::Update(REAL elapsedTime)
{
	// If the particle system was not visible during last render flush, or if it is marked invisible, simply return
	if(m_iterationCount && (this->GetStatusFlag(ruENTITYSTATUS_VISIBLE) == FALSE || this->GetOption(ruENTITYOPTION_HIDDEN) != false))
	{
		return TRUE;
	}

	// If the particle entity has not been loaded, load it now
	if(m_entityName && m_particleEntity == NULL)
	{
		g_ruResourceManager->LoadEntity(m_entityName, &m_particleEntity);
	}

//	BOOL retVal = IRuEntity_Particle_Visualizer::Update(elapsedTime);

	// Rebuild particle batches at specified intervals
	m_timeSinceLastUpdate += elapsedTime;
	if(m_timeSinceLastUpdate >= m_updateInterval)
	{
		// Reset update counter
		m_timeSinceLastUpdate = 0.0f;

		// Chain call to emitter
		if(m_emitter && m_particleEntity)
		{
			// Get particle list from the emitter
			CRuArrayList<CRuParticleDesc> *particles = m_emitter->GetParticleDescriptors();

			RuParticleFrameType frameType = m_emitter->GetFrameType();

			// Iterate through all particles and build batches
			for(INT32 i = 0; i < particles->Count(); ++i)
			{
				// Make sure entities list is at least as large as the particle list
				while(i >= m_entities.Count())
				{
					m_entities.Add(ParticleDesc());
				}

				// Store reference to the particle
				const CRuParticleDesc &particle = (*particles)[i];

				// Skip inactive particles
				if(particle.m_active == FALSE)
				{
	//				if(m_entities[i].m_entity)
	//					m_entities[i].m_entity->DetachFromParent();
	//				ruSAFE_RELEASE(m_entities[i].m_entity);

					if(m_entities[i].m_entity)
					{
						// Enable invisibility
						m_entities[i].m_entity->SetOption(ruENTITYOPTION_HIDDEN, TRUE);
					}

					continue;
				}

				// Get available particle entity
				CRuEntity *particleEntity = m_entities[i].m_entity;
				if(particleEntity == NULL)
				{
					IRuObject *clonedEntity = m_particleEntity->Clone();
					if(clonedEntity && clonedEntity->GetType().IsSubClassOf(CRuEntity::Type()))
					{
						particleEntity = static_cast<CRuEntity *>(clonedEntity);
						m_entities[i].m_entity = particleEntity;

						// Disable automatic cloning and serialization
						particleEntity->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
						particleEntity->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);
						particleEntity->SetProperty("act:no enumerate", TRUE);

						// Disable scene hierarchy role
						RuEntity_SetOption(particleEntity, ruENTITYOPTION_HRDISABLE, TRUE);

						// Attach child to tree
						this->AddChild(particleEntity);
					}
					else
					{
						continue;
					}
				}

				// Disable invisibility
				particleEntity->SetOption(ruENTITYOPTION_HIDDEN, FALSE);

				// Update particle
				REAL t = particle.m_age / particle.m_extinction;
				CRuColor4 color = m_track_Color.GetInterpolatedKey(t * m_track_Color.GetTrackLength());
				REAL alpha = m_track_Alpha.GetInterpolatedKey(t * m_track_Alpha.GetTrackLength());
				REAL scale = m_track_Scale.GetInterpolatedKey(t * m_track_Scale.GetTrackLength()) * particle.m_scale;

				particleEntity->ResetLocalTransform();
				particleEntity->Scale(CRuVector3(scale, scale, scale));

				if(m_orientToDirectionOfTravel)
				{
					CRuQuaternion quat;
					quat.RotateIntoVector(particle.m_direction, CRuVector3(0.0f, 1.0f, 0.0f));
					particleEntity->Transform(quat.GetMatrix());
				}

				particleEntity->Translate(particle.m_position);

				particleEntity->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(alpha));
				particleEntity->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, color.GetARGB());

				if(alpha < 1.0f)
				{
					particleEntity->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, FALSE);
				}
				else
				{
					particleEntity->GetParameterBlock()->ClearParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE);
				}

				const char *motionName = m_track_MotionName.GetDiscreteKey(t * m_track_Scale.GetTrackLength());

				if(motionName)
				{
					if(m_entities[i].m_particleIteration != particle.m_particleIteration)
					{
						m_entities[i].m_particleIteration = particle.m_particleIteration;
						particleEntity->ResetStates();
						RuACT_PlayMotionEx(particleEntity, 0xFFFFFFFF, motionName, 0.0f, TRUE);

						// Forced update for difference between elapsed time and actual particle age
						particleEntity->Update(particle.m_age - elapsedTime);
					}
					else
					{
						RuACT_PlayMotionEx(particleEntity, 0xFFFFFFFF, motionName, 0.0f, FALSE);
					}
				}

				if(m_emitter->GetFrameType() == ruPRTFRAMETYPE_WORLD)
				{
					particleEntity->SetOption(ruENTITYOPTION_TRANSFORMMODE, ruENTITYTRANSFORMMODE_OVERRIDE);
				}
				else
				{
					particleEntity->SetOption(ruENTITYOPTION_TRANSFORMMODE, ruENTITYTRANSFORMMODE_NORMAL);
				}

				// VizCon changes the alpha value.... can't exactly disable it, so i got to figure out how to make them share the alpha value..
				particleEntity->SetOption(ruENTITYOPTION_ENABLEDISTANCEFADE, FALSE);
			}
		}

		// Update visualizer container
		BOOL retVal = IRuEntity_Particle_Visualizer::Update(elapsedTime);

		// Increment iteration count
		++m_iterationCount;

		return retVal;
	}

	return TRUE;
}

BOOL CRuEntity_Particle_Visualizer_Entity::QueryCollision(CRuCollisionQuery *colQuery)
{
	// Chain collision query to the emitter
	if(m_emitter)
	{
		return m_emitter->QueryCollision(colQuery);
	}

	return TRUE;
}

IRuEntity_Particle_Emitter *CRuEntity_Particle_Visualizer_Entity::GetEmitter()
{
	return m_emitter;
}

BOOL CRuEntity_Particle_Visualizer_Entity::SetEmitter(IRuEntity_Particle_Emitter *emitter)
{
	if(emitter)
	{
		this->RemoveChild(m_emitter);

		// Release original emitter and store reference to new emitter
		ruSAFE_RELEASE(m_emitter);
		m_emitter = emitter;
		m_emitter->AddRef();

		// Disable automatic cloning and serialization
		m_emitter->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
		m_emitter->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);
		m_emitter->SetProperty("act:no enumerate", TRUE);

		// Insert emitter into tree
		this->AddChild(m_emitter);

		return TRUE;
	}

	return FALSE;
}

const char *CRuEntity_Particle_Visualizer_Entity::GetEntityName() const
{
	if(m_entityName)
	{
		return m_entityName;
	}

	return "";
}

void CRuEntity_Particle_Visualizer_Entity::SetEntityName(const char *entityName)
{
	if(entityName)
	{
		if(m_entityName)
		{
			// If the specified entity name is identical, abort
			if(strcmp(m_entityName, entityName) == 0)
			{
				return;
			}

			delete[] m_entityName;
		}

		m_entityName = ruNEW char [strlen(entityName) + 1];
		strcpy(m_entityName, entityName);

		// Reload entity
		ReloadParticleEntity();
	}
}

BOOL CRuEntity_Particle_Visualizer_Entity::GetOrientToDirectionOfTravel() const
{
	return m_orientToDirectionOfTravel;
}

void CRuEntity_Particle_Visualizer_Entity::SetOrientToDirectionOfTravel(BOOL orientToDirectionOfTravel)
{
	m_orientToDirectionOfTravel = orientToDirectionOfTravel;
}

CRuAnimTrack<char *> &CRuEntity_Particle_Visualizer_Entity::GetTrack_MotionName()
{
	return m_track_MotionName;
}

CRuAnimTrack<CRuColor4> &CRuEntity_Particle_Visualizer_Entity::GetTrack_Color()
{
	return m_track_Color;
}

CRuAnimTrack<REAL> &CRuEntity_Particle_Visualizer_Entity::GetTrack_Alpha()
{
	return m_track_Alpha;
}

CRuAnimTrack<REAL> &CRuEntity_Particle_Visualizer_Entity::GetTrack_Scale()
{
	return m_track_Scale;
}

const char *CRuEntity_Particle_Visualizer_Entity::GetMotionName() const
{
	char *motionName;

	if(m_track_MotionName.GetKeyByIndex(0, &motionName))
	{
		return motionName;
	}

	return "";
}

void CRuEntity_Particle_Visualizer_Entity::SetMotionName(const char *motionName)
{
	m_track_MotionName.SetKeyByIndex(0, motionName);
	m_track_MotionName.SetKeyByIndex(1, motionName);
}

DWORD CRuEntity_Particle_Visualizer_Entity::GetStartColor() const
{
	CRuColor4 color;
	m_track_Color.GetKeyByIndex(0, &color);
	return color.GetARGB();
}

void CRuEntity_Particle_Visualizer_Entity::SetStartColor(DWORD color)
{
	m_track_Color.SetKeyByIndex(0, CRuColor4(color));
}

DWORD CRuEntity_Particle_Visualizer_Entity::GetEndColor() const
{
	CRuColor4 color;
	m_track_Color.GetKeyByIndex(m_track_Color.GetNumKeys() - 1, &color);
	return color.GetARGB();
}

void CRuEntity_Particle_Visualizer_Entity::SetEndColor(DWORD color)
{
	m_track_Color.SetKeyByIndex(m_track_Color.GetNumKeys() - 1, CRuColor4(color));
}

REAL CRuEntity_Particle_Visualizer_Entity::GetStartAlpha() const
{
	REAL alpha;
	m_track_Alpha.GetKeyByIndex(0, &alpha);
	return alpha;
}

void CRuEntity_Particle_Visualizer_Entity::SetStartAlpha(REAL alpha)
{
	m_track_Alpha.SetKeyByIndex(0, alpha);
}

REAL CRuEntity_Particle_Visualizer_Entity::GetEndAlpha() const
{
	REAL alpha;
	m_track_Alpha.GetKeyByIndex(m_track_Alpha.GetNumKeys() - 1, &alpha);
	return alpha;
}

void CRuEntity_Particle_Visualizer_Entity::SetEndAlpha(REAL alpha)
{
	m_track_Alpha.SetKeyByIndex(m_track_Alpha.GetNumKeys() - 1, alpha);
}

REAL CRuEntity_Particle_Visualizer_Entity::GetStartScale() const
{
	REAL scale;
	m_track_Scale.GetKeyByIndex(0, &scale);
	return scale;
}

void CRuEntity_Particle_Visualizer_Entity::SetStartScale(REAL scale)
{
	m_track_Scale.SetKeyByIndex(0, scale);
}

REAL CRuEntity_Particle_Visualizer_Entity::GetEndScale() const
{
	REAL scale;
	m_track_Scale.GetKeyByIndex(m_track_Scale.GetNumKeys() - 1, &scale);
	return scale;
}

void CRuEntity_Particle_Visualizer_Entity::SetEndScale(REAL scale)
{
	m_track_Scale.SetKeyByIndex(m_track_Scale.GetNumKeys() - 1, scale);
}

void CRuEntity_Particle_Visualizer_Entity::FlushParticleStack()
{
	// Clear entities list
	for(INT32 i = 0; i < m_entities.Count(); ++i)
	{
		if(m_entities[i].m_entity)
		{
			// Remove entity from scene graph
			m_entities[i].m_entity->DetachFromParent();

			// Release entity
			ruSAFE_RELEASE(m_entities[i].m_entity);
		}
	}

	m_entities.Clear();
}

void CRuEntity_Particle_Visualizer_Entity::ReloadParticleEntity()
{
	// If a particle entity was already loaded, flush the entire entity stack
	if(m_particleEntity)
	{
		ruSAFE_RELEASE(m_particleEntity);
		FlushParticleStack();
	}
/*
	// Load the particle entity
	if(m_entityName)
	{
		m_particleEntity = g_ruResourceManager->LoadEntity(m_entityName);
	}
*/
}

// ************************************************************************************************************************************************************

#pragma managed
