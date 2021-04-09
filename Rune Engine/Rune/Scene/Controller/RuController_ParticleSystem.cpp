#include "../../Scene/Controller/RuController_ParticleSystem.h"
#include "../../Scene/Particle/RuEntity_Particle_Emitter.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_ParticleSystem, IRuEntity_Controller)
ruHEAP_DEFINE_SUBCLASS(CRuController_ParticleSystem, "CRuController_ParticleSystem", "IRuEntity_Controller")
ruCLASSGUID_DEFINE(CRuController_ParticleSystem, 0x0B47136B, 0xC8F046e0, 0x9ED18FD5, 0x87FC14EA)

// ************************************************************************************************************************************************************

CRuController_ParticleSystem::CRuController_ParticleSystem()
:	m_targetEntity(NULL),
	m_systemAttached(FALSE),
	m_systemActive(FALSE),
	m_particleSystemFrame(NULL),
	m_particleSystem(NULL),
	m_particleSystemName(NULL)
{
	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_ParticleSystem::HandleEvent_Dispose, this, _1), NULL);
}

CRuController_ParticleSystem::~CRuController_ParticleSystem()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

IRuObject *CRuController_ParticleSystem::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_ParticleSystem();
	CRuController_ParticleSystem *controller = static_cast<CRuController_ParticleSystem *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_Controller::Clone(clonedObject);

	if(m_particleSystemName)
	{
		controller->SetParticleSystem(m_particleSystemName);
	}

	return clonedObject;
}

BOOL CRuController_ParticleSystem::SerializeFrom(IRuStream *stream)
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
				m_particleSystemName = ruNEW char [nameLength];
				stream->Read(m_particleSystemName, nameLength);
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_ParticleSystem::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Controller::SerializeTo(stream);

	if(m_particleSystemName)
	{
		UINT32 nameLength = (UINT32)strlen(m_particleSystemName) + 1;
		stream->Write(&nameLength, sizeof(nameLength));
		stream->Write(m_particleSystemName, nameLength);
	}
	else
	{
		UINT32 nameLength = (UINT32)strlen("") + 1;
		stream->Write(&nameLength, sizeof(nameLength));
		stream->Write("", nameLength);
	}

	return TRUE;
}

BOOL CRuController_ParticleSystem::Update(REAL elapsedTime)
{
	if(m_enabled)
	{
		if(m_targetEntity && m_particleSystem)
		{
			// Attach particle system to target entity if necessary
			if(m_systemAttached == FALSE)
			{
				m_systemAttached = TRUE;
				m_targetEntity->AddEntity(m_particleSystemFrame);
			}

			REAL t = m_age;
			REAL relativeT = t - m_startTime;
			REAL tMin = relativeT;
			REAL tMax = tMin + elapsedTime;

			if(m_loopTime >= 0.0f)
			{
				REAL loopPeriod = m_loopTime + m_duration;
				relativeT = fmod(relativeT, loopPeriod);
				tMin = fmod(tMin, loopPeriod);
				tMax = fmod(tMax, loopPeriod);

				if(tMax < tMin)
				{
					tMin -= loopPeriod;
				}
			}

			// Update time
			t += elapsedTime;

			if(tMin <= 0.0f && 0.0f <= tMax)
			{
				if(!m_systemActive && m_particleSystem)
				{
					// Process particle system
					if(m_particleSystem->GetType().IsSubClassOf(CRuEntity_Particle_Emitter::Type()))
					{
						static_cast<CRuEntity_Particle_Emitter *>(m_particleSystem)->SetEmissionRate(m_emissionRate);
					}
					else if(m_particleSystem->GetType().IsSubClassOf(CRuEntity_Particle_Sprite::Type()))
					{
						static_cast<CRuEntity_Particle_Sprite *>(m_particleSystem)->SetEmissionRate(m_emissionRate);
					}

					m_systemActive = TRUE;
				}
			}

			if(tMin <= m_duration && m_duration <= tMax)
			{
				if(m_systemActive && m_particleSystem)
				{
					// Process particle system
					if(m_particleSystem->GetType().IsSubClassOf(CRuEntity_Particle_Emitter::Type()))
					{
						static_cast<CRuEntity_Particle_Emitter *>(m_particleSystem)->SetEmissionRate(0.0f);
					}
					else if(m_particleSystem->GetType().IsSubClassOf(CRuEntity_Particle_Sprite::Type()))
					{
						static_cast<CRuEntity_Particle_Sprite *>(m_particleSystem)->SetEmissionRate(0.0f);
					}

					m_systemActive = FALSE;
				}
			}
		}
	}

	// Chain update call to base controller class
	return IRuEntity_Controller::Update(elapsedTime);
}

IRuObject *CRuController_ParticleSystem::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_ParticleSystem::SetTarget(IRuObject *targetEntity)
{
	if(targetEntity->GetType().IsSubClassOf(CRuEntity_Container_Impl::Type()))
	{
		// If we had a previous target, unregister it
		if(m_targetEntity)
		{
			m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		// Store target reference, setup event listener, and setup target parameter block
		m_targetEntity = static_cast<CRuEntity_Container_Impl *>(targetEntity);
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_ParticleSystem::HandleEvent_Dispose, this, _1), NULL);
	}

	return TRUE;
}

BOOL CRuController_ParticleSystem::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_targetEntity)
	{
		// Detach particle system from the target entity
		if(m_systemAttached)
		{
			for(INT32 i = 0; i < m_targetEntity->GetNumEntities(); ++i)
			{
				if(m_targetEntity->GetEntity(i) == m_particleSystemFrame)
				{
					m_targetEntity->RemoveEntityAt(i);
					break;
				}
			}

			m_systemAttached = FALSE;
		}

		// Unregister from target entity
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	// Release reference to the particle system
	ruSAFE_RELEASE(m_particleSystemFrame);

	return TRUE;
}

const char *CRuController_ParticleSystem::GetParticleSystemName()
{
	if(m_particleSystemName)
	{
		return m_particleSystemName;
	}

	return "";
}

void CRuController_ParticleSystem::SetParticleSystem(CRuEntity *particleSystem)
{
	// Detach particle system from the target entity
	if(m_systemAttached)
	{
		for(INT32 i = 0; i < m_targetEntity->GetNumEntities(); ++i)
		{
			if(m_targetEntity->GetEntity(i) == m_particleSystemFrame)
			{
				m_targetEntity->RemoveEntityAt(i);
				break;
			}
		}
	}

	// Release reference to the particle system
	ruSAFE_RELEASE(m_particleSystemFrame);
	m_systemAttached = FALSE;

	if(particleSystem->GetType().IsSubClassOf(IRuEntity_Container::Type()))
	{
		// Save reference to the new particle system
		m_particleSystemFrame = particleSystem;

		// Get first entity as particle system
		particleSystem = static_cast<IRuEntity_Container *>(particleSystem)->GetEntity(0);

		// Process particle system
		if(particleSystem->GetType().IsSubClassOf(IRuEntity_Particle_Visualizer::Type()))
		{
			// Store reference to the emitter as the particle system
			m_particleSystem = static_cast<IRuEntity_Particle_Visualizer *>(particleSystem)->GetEmitter();

			// Save emitter's current emission rate, and reset it to zero
			if(m_particleSystem && m_particleSystem->GetType().IsSubClassOf(CRuEntity_Particle_Emitter::Type()))
			{
				m_emissionRate = static_cast<CRuEntity_Particle_Emitter *>(m_particleSystem)->GetEmissionRate();
				static_cast<CRuEntity_Particle_Emitter *>(m_particleSystem)->SetEmissionRate(0.0f);
			}
		}
		else if(particleSystem->GetType().IsSubClassOf(CRuEntity_Particle_Sprite::Type()))
		{
			m_particleSystem = particleSystem;
			m_emissionRate = static_cast<CRuEntity_Particle_Sprite *>(m_particleSystem)->GetEmissionRate();
			static_cast<CRuEntity_Particle_Sprite *>(m_particleSystem)->SetEmissionRate(0.0f);
		}
	}
}

void CRuController_ParticleSystem::SetParticleSystem(const char *particleSystemName)
{
	if(particleSystemName)
	{
		delete[] m_particleSystemName;
		m_particleSystemName = ruNEW char [strlen(particleSystemName) + 1];
		memcpy(m_particleSystemName, particleSystemName, strlen(particleSystemName) + 1);

		// Load particle system
		CRuEntity *psEntity = g_ruResourceManager->LoadEntity(m_particleSystemName);

		if(psEntity)
		{
			SetParticleSystem(psEntity);
		}
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
