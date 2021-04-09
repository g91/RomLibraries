/*!
	@file RuEntity_CollectorFX.cpp

	Copyright (c) 2004-2006 All rights reserved
*/

#include "../../Scene/FX/RuEntity_CollectorFX.h"
#include "../../Helper/RuHelper_Entity.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_CollectorFX, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_CollectorFX, "CRuEntity_CollectorFX", "CRuEntity")
ruCLASSGUID_DEFINE(CRuEntity_CollectorFX, 0x7A46C2B1, 0x91E24ac6, 0xA54AE89E, 0x8309DCAC)

// ************************************************************************************************************************************************************

CRuEntity_CollectorFX::CRuEntity_CollectorFX()
:	m_effectEntity(NULL),
	m_effectStartMotion(NULL),
	m_effectFlightMotion(NULL),
	m_effectEndMotion(NULL),
	m_effectEntityGenerateCount(3),
	m_generatorPosition(0.0f, 0.0f, 0.0f),
	m_generatorStrength(0.0f),
	m_generatorVelocityDamping(20.0f),
	m_pathDeviation(10.0f),
	m_collectionSpeed(2.0f),
	m_collectorState(ruCOLLECTORSTATE_READY)
{
	this->SetOption(ruENTITYOPTION_TRANSFORMMODE, ruENTITYTRANSFORMMODE_NOROTATE);
}

CRuEntity_CollectorFX::~CRuEntity_CollectorFX()
{
	ruSAFE_RELEASE(m_effectEntity);
	delete[] m_effectStartMotion;
	delete[] m_effectFlightMotion;
	delete[] m_effectEndMotion;

	for(INT32 i = 0; i < m_effectEntities.Count(); ++i)
	{
		ruSAFE_RELEASE(m_effectEntities[i].m_entity);
	}
}

CRuEntity *CRuEntity_CollectorFX::GetEffectEntity()
{
	return m_effectEntity;
}

void CRuEntity_CollectorFX::SetEffectEntity(CRuEntity *entity)
{
	if(entity)
	{
		entity->AddRef();
		ruSAFE_RELEASE(m_effectEntity);
		m_effectEntity = entity;
	}
	else
	{
		ruSAFE_RELEASE(m_effectEntity);
	}
}

const char *CRuEntity_CollectorFX::GetEffectStartMotion()
{
	return m_effectStartMotion ? m_effectStartMotion : "";
}

void CRuEntity_CollectorFX::SetEffectStartMotion(const char *motion)
{
	delete[] m_effectStartMotion;
	m_effectStartMotion = NULL;

	if(motion)
	{
		INT32 strLength = (INT32)strlen(motion) + 1;
		m_effectStartMotion = ruNEW char [strLength];
		memcpy(m_effectStartMotion, motion, strLength);
	}
}

const char *CRuEntity_CollectorFX::GetEffectFlightMotion()
{
	return m_effectFlightMotion ? m_effectFlightMotion : "";
}

void CRuEntity_CollectorFX::SetEffectFlightMotion(const char *motion)
{
	delete[] m_effectFlightMotion;
	m_effectFlightMotion = NULL;

	if(motion)
	{
		INT32 strLength = (INT32)strlen(motion) + 1;
		m_effectFlightMotion = ruNEW char [strLength];
		memcpy(m_effectFlightMotion, motion, strLength);
	}
}

const char *CRuEntity_CollectorFX::GetEffectEndMotion()
{
	return m_effectEndMotion ? m_effectEndMotion : "";
}

void CRuEntity_CollectorFX::SetEffectEndMotion(const char *motion)
{
	delete[] m_effectEndMotion;
	m_effectEndMotion = NULL;

	if(motion)
	{
		INT32 strLength = (INT32)strlen(motion) + 1;
		m_effectEndMotion = ruNEW char [strLength];
		memcpy(m_effectEndMotion, motion, strLength);
	}
}

INT32 CRuEntity_CollectorFX::GetEffectEntityGenerateCount()
{
	return m_effectEntityGenerateCount;
}

void CRuEntity_CollectorFX::SetEffectEntityGenerateCount(INT32 numEntities)
{
	m_effectEntityGenerateCount = numEntities;
}

const CRuVector3 &CRuEntity_CollectorFX::GetGeneratorPosition()
{
	return m_generatorPosition;
}

void CRuEntity_CollectorFX::SetGeneratorPosition(const CRuVector3 &position)
{
	m_generatorPosition = position;
}

REAL CRuEntity_CollectorFX::GetGeneratorStrength()
{
	return m_generatorStrength;
}

void CRuEntity_CollectorFX::SetGeneratorStrength(REAL strength)
{
	m_generatorStrength = strength;
}

REAL CRuEntity_CollectorFX::GetGeneratorVelocityDamping()
{
	return m_generatorVelocityDamping;
}

void CRuEntity_CollectorFX::SetGeneratorVelocityDamping(REAL damping)
{
	m_generatorVelocityDamping = damping;
}

REAL CRuEntity_CollectorFX::GetPathDeviation()
{
	return m_pathDeviation;
}

void CRuEntity_CollectorFX::SetPathDeviation(REAL deviation)
{
	m_pathDeviation = deviation;
}

REAL CRuEntity_CollectorFX::GetCollectionSpeed()
{
	return m_collectionSpeed;
}

void CRuEntity_CollectorFX::SetCollectionSpeed(REAL collectionSpeed)
{
	m_collectionSpeed = collectionSpeed;
}

BOOL CRuEntity_CollectorFX::IsCollectorFXFinished()
{
	return m_collectorState == ruCOLLECTORSTATE_FINISHED;
}

IRuObject *CRuEntity_CollectorFX::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuEntity_CollectorFX();

	// Chain cloning call to parent class
	CRuEntity::Clone(clonedObject);

	return clonedObject;
}

BOOL CRuEntity_CollectorFX::SerializeFrom(IRuStream *stream)
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
	CRuEntity::SerializeFrom(stream);

	return TRUE;
}

BOOL CRuEntity_CollectorFX::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuEntity::SerializeTo(stream);

	return TRUE;
}

BOOL CRuEntity_CollectorFX::Update(REAL elapsedTime)
{
	// Chain update to base class
	CRuEntity::Update(elapsedTime);

	switch(m_collectorState)
	{
		case ruCOLLECTORSTATE_READY:
			{
				// Generate effect entities
				if(m_effectEntity)
				{
					// Calculate the generator axis to use
					CRuVector3 generatorAxis = m_generatorPosition - this->GetTranslation(ruFRAMETYPE_WORLD);
					generatorAxis.m_y = 0.0f;

					if(generatorAxis.Magnitude() < ruEPSILON)
					{
						CRuQuaternion rotationQuat;
						rotationQuat.FromAngleAxis(CRuVector3(0.0f, 1.0f, 0.0f), RuGlobalMTRand().GenerateFloat() * ruPI * 2.0f);

						generatorAxis.Set(1.0f, 0.0f, 0.0f);
						rotationQuat.TransformVector(generatorAxis);
					}

					generatorAxis.Normalize();

					for(INT32 i = 0; i < m_effectEntityGenerateCount; ++i)
					{
						// Clone the effect entity
						CRuEntity *fxEntity = static_cast<CRuEntity *>(m_effectEntity->Clone());

						if(fxEntity)
						{
							EffectEntityDesc fxEntityDesc;

							fxEntityDesc.m_entity = fxEntity;
							fxEntityDesc.m_velocity = generatorAxis * m_pathDeviation;
							fxEntityDesc.m_origin = this->GetTranslation(ruFRAMETYPE_WORLD);
							fxEntityDesc.m_deviationVectorLength = m_pathDeviation;
							fxEntityDesc.m_deviationVectorAngle = RuGlobalMTRand().GenerateFloat() * ruPI * 2.0f;
							fxEntityDesc.m_finished = FALSE;

							// Add fx entity to the list
							m_effectEntities.Add(fxEntityDesc);

							// Add fx entity to graph
							fxEntity->SetOption(ruENTITYOPTION_TRANSFORMMODE, ruENTITYTRANSFORMMODE_OVERRIDE);
							fxEntity->SetTranslation(fxEntityDesc.m_origin);
							this->AddChild(fxEntity);

							// Begin start motion
							RuACT_PlayMotion(fxEntity, this->GetEffectStartMotion());
						}
					}
				}

				// Switch state to active
				m_collectorState = ruCOLLECTORSTATE_ACTIVE;
			}

			break;

		case ruCOLLECTORSTATE_ACTIVE:
			{
				BOOL hasActiveFXEntities = FALSE;

				for(INT32 i = 0; i < m_effectEntities.Count(); ++i)
				{
					EffectEntityDesc &fxEntityDesc = m_effectEntities[i];

					// If the begin motion is finished, start playing the flight motion
					if(RuACT_IsMotionComplete(fxEntityDesc.m_entity, 0xFFFFFFFF, TRUE) && fxEntityDesc.m_finished == FALSE)
					{
						RuACT_PlayMotionEx(fxEntityDesc.m_entity, 0xFFFFFFFF, this->GetEffectFlightMotion());
					}

					if(fxEntityDesc.m_velocity.Magnitude() > ruEPSILON)
					{
						fxEntityDesc.m_entity->Translate(fxEntityDesc.m_velocity * elapsedTime);

						REAL speed = max(0.0f, fxEntityDesc.m_velocity.Magnitude() - m_generatorVelocityDamping * elapsedTime);
						fxEntityDesc.m_velocity.Normalize();
						fxEntityDesc.m_velocity = fxEntityDesc.m_velocity * speed;

						if(fxEntityDesc.m_velocity.Magnitude() <= ruEPSILON)
						{
							fxEntityDesc.m_origin = fxEntityDesc.m_entity->GetTranslation(ruFRAMETYPE_WORLD);
						}

						hasActiveFXEntities = TRUE;
					}
					else
					{
						if(fxEntityDesc.m_finished == FALSE)
						{
							// Calculate expected axis of travel for the fx entity
							CRuVector3 axisOfTravel = fxEntityDesc.m_entity->GetTranslation(ruFRAMETYPE_WORLD) - fxEntityDesc.m_origin;
							CRuVector3 expectedAxisOfTravel = this->GetTranslation(ruFRAMETYPE_WORLD) - fxEntityDesc.m_origin;

							// Project current axis of travel onto expected axis of travel
							CRuVector3 projAxisOfTravel = axisOfTravel;
							projAxisOfTravel.ProjectOnto(expectedAxisOfTravel);

							// t is the distance we have traveled along the expected axis of travel
							REAL t = projAxisOfTravel.Magnitude() / expectedAxisOfTravel.Magnitude();

							t += m_collectionSpeed * elapsedTime;

							if(t < 1.0f)
							{
								// Calculate expected fx entity position
								CRuVector3 expectedPos = expectedAxisOfTravel * t + fxEntityDesc.m_origin;

								// Calculate deviation vector which is orthogonal to the expected axis of travel
								CRuVector3 devVec = CrossProduct(CRuVector3(0.0f, 1.0f, 0.0f), expectedAxisOfTravel);
								if(devVec.Magnitude() < ruEPSILON)
								{
									devVec = CrossProduct(CRuVector3(1.0f, 0.0f, 0.0f), expectedAxisOfTravel);
								}

								// Rotate deviation vector by the deviation vector angle
								CRuQuaternion devRotQuat;
								devRotQuat.FromAngleAxis(expectedAxisOfTravel, fxEntityDesc.m_deviationVectorAngle);
								devRotQuat.TransformVector(devVec);

								// Calculate current deviation length
								REAL blargh = t * expectedAxisOfTravel.Magnitude();
								REAL curDeviationLength = fxEntityDesc.m_deviationVectorLength * sin(t * ruPI);

								devVec.Normalize();
								devVec = devVec * curDeviationLength;

								// Calculate expected position
								CRuVector3 expectedDeviatedPos = expectedPos + devVec;

								// Set new position
								fxEntityDesc.m_entity->SetTranslation(expectedDeviatedPos);
							}
							else
							{
								fxEntityDesc.m_finished = TRUE;
								RuACT_PlayMotionEx(fxEntityDesc.m_entity, 0xFFFFFFFF, this->GetEffectEndMotion());
							}

							hasActiveFXEntities = TRUE;
						}
						else
						{
							if(RuACT_IsMotionComplete(fxEntityDesc.m_entity, 0xFFFFFFFF, TRUE) == FALSE)
							{
								hasActiveFXEntities = TRUE;
							}
						}
					}
				}

				if(hasActiveFXEntities == FALSE)
				{
					// Switch state to finished
					m_collectorState = ruCOLLECTORSTATE_FINISHED;
				}
			}

			break;
		case ruCOLLECTORSTATE_FINISHED:
			{
			}

			break;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
