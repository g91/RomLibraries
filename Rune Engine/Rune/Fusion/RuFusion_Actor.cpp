/*!
	@project Rune
	@file RuFusion_Actor.cpp

	Copyright (c) 2004-2006 All rights reserved

	@author John Tang
	@date 2006/07/28
*/

#include "../Fusion/RuFusion_Actor.h"
#include "../Helper/RuHelper_Entity.h"
#include "../Helper/RuHelper_Math.h"

//#include "../../../Interface/debug/DebugMessage.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFusion_Actor, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuFusion_Actor, "CRuFusion_Actor", "IRuObject")
ruCLASSGUID_DEFINE(CRuFusion_Actor, 0xE5AD9C6E, 0x57F347d0, 0x93F12EE0, 0xDB36D2C8)

// ************************************************************************************************************************************************************

CRuFusion_Actor::CRuFusion_Actor()
:	m_targetEntity(NULL),
	m_targetMime(NULL),
	m_enableSojourner(TRUE),
	m_sojourner(NULL),
	m_actorState(ruFUSION_ACTORSTATE_NORMAL),
	m_isInWater(FALSE),
	m_isSubmerged(FALSE),
	m_foregroundState(ruFUSION_ACTORSTATE_NORMAL),
	m_foregroundMimeAction(ruFUSION_MIME_NONE),
	m_transientState(ruFUSION_ACTORSTATE_NORMAL),
	m_transientMimeAction(ruFUSION_MIME_NONE),
	m_enableTorsoTurn(TRUE),
	m_enableRebasingOnUpVector(FALSE),
	m_upVector(0.0f, 1.0f, 0.0f),
	m_currentForwardVector(1.0f, 0.0f, 0.0f),
	m_targetForwardVector(1.0f, 0.0f, 0.0f),
	m_tiltMagnitude(0.0f),
	m_upRotAngleRate(7.5f),
	m_curUpRotAngle(0.0f),
	m_idlingTime(0.0f),
	m_lastUpdateIsIdle(TRUE),
	m_orientMode(ruORIENTMODE_CONSTRAINED),
	m_movementType(ruSOJMOVETYPE_FULL),
	m_disableMovementOnAction(ruFUSION_ACTORSTATE_NORMAL),
	m_disableMovement(FALSE),
	m_disableInPlaceJump(FALSE),
	m_slopeSlideThreshold(45.0f),
	m_gravityAcceleration(-98.0f),
	m_rotationSpeed(0.0f),
	m_runThreshold(20.0f),
	m_scale(1.0f),
	m_defaultIdleAction(ruFUSION_MIME_IDLE_STAND),
	m_defaultWalkingAction(ruFUSION_MIME_WALK_FORWARD),
	m_defaultRunningAction(ruFUSION_MIME_RUN_FORWARD),
	m_defaultBackAction(ruFUSION_MIME_RUN_BACKWARD),
	m_gravitySpeed(0.0f),
	m_deadTime(0.0f),
	m_onFirmGround(FALSE),
	m_timeSinceLastJump(10.0f),
	m_targetBodyHeadingOffset(0.0f),
	m_targetTorsoHeadingOffset(0.0f),
	m_targetYawOffset(0.0f),
	m_currentBodyHeadingOffset(0.0f),
	m_currentTorsoHeadingOffset(0.0f),
	m_currentYawOffset(0.0f),
	m_shufflingTime(FLT_MAX),
	m_linkParent(NULL)
{
	// Build transform to rotate from impulse/velocity space (where +Z is forward and +X is right) into entity reference space
	m_refFrameBasis.SetToBasis(CRuVector3(-1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, -1.0f));

	m_latentPeriod = 0;

	m_swim_Surface_EntrySegment.Origin().Set(0.0f, 15.0f, 0.0f);
	m_swim_Surface_EntrySegment.Direction().Set(0.0f, -2.0f, 0.0f);

	m_swim_Surface_ExitSegment.Origin().Set(0.0f, 13.0f, 0.0f);
	m_swim_Surface_ExitSegment.Direction().Set(0.0f, -5.0f, 0.0f);

	m_swim_Dive_EntrySegment.Origin().Set(0.0f, 20.0f, 0.0f);
	m_swim_Dive_EntrySegment.Direction().Set(0.0f, -3.0f, 0.0f);

	m_swim_Dive_ExitSegment.Origin().Set(0.0f, 16.0f, 0.0f);
	m_swim_Dive_ExitSegment.Direction().Set(0.0f, -5.0f, 0.0f);

	m_probe_ActorUpdate = g_ruProfiler->CreateProbe("Actor Update");

	m_nextIdlingPeriod = m_randGen.GenerateFloat() * 10.0f + 10.0f;
}

CRuFusion_Actor::~CRuFusion_Actor()
{
	SeverActorLinks();

	ruSAFE_RELEASE(m_targetEntity);
	ruSAFE_RELEASE(m_targetMime);
	ruSAFE_RELEASE(m_sojourner);
}

CRuEntity *CRuFusion_Actor::GetTargetEntity()
{
	return m_targetEntity;
}

CRuFusion_Mime *CRuFusion_Actor::GetTargetMime()
{
	return m_targetMime;
}

BOOL CRuFusion_Actor::SetTargetMime(CRuFusion_Mime *targetMime)
{
	if(targetMime)
	{
		// Store reference to new mime
		targetMime->AddRef();
		ruSAFE_RELEASE(m_targetMime);
		m_targetMime = targetMime;

		// Store reference to new target entity
		CRuEntity *targetEntity = targetMime->GetTarget();
		if(targetEntity)
		{
			targetEntity->AddRef();
			ruSAFE_RELEASE(m_targetEntity);
			m_targetEntity = targetEntity;
		}

		// Setup callback
		m_targetMime->SetActionChangedCallback(boost::bind(&CRuFusion_Actor::MimeActionChangedHandler, this, _1, _2, _3));

		// Set a default foreground state
		m_targetMime->SetDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_NONE, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_NORMAL));

		return TRUE;
	}

	return FALSE;
}

BOOL CRuFusion_Actor::SetOrientMode(CRuFusion_Actor::OrientMode orientMode)
{
	m_orientMode = orientMode;
	return TRUE;
}

BOOL CRuFusion_Actor::SetMovementType(CRuSojournerMovementType movementType)
{
	// Reset all latent velocities
	m_latentVelocity.Set(0.0f, 0.0f, 0.0f);
	m_gravitySpeed = 0.0f;
	m_onFirmGround = TRUE;

	// Store new movement type
	m_movementType = movementType;

	if(m_sojourner)
	{
		m_sojourner->SetMovementType(movementType);
		return TRUE;
	}

	return FALSE;
}

BOOL CRuFusion_Actor::EnableSojourner(BOOL enable)
{
	m_enableSojourner = enable;
	return TRUE;
}

CRuFusion_Sojourner *CRuFusion_Actor::GetSojourner()
{
	return m_sojourner;
}

BOOL CRuFusion_Actor::SetCollisionObject(IRuObject *collisionObject)
{
	ruSAFE_RELEASE(m_sojourner);
	m_sojourner = ruNEW CRuFusion_Sojourner(collisionObject);

	return TRUE;
}

BOOL CRuFusion_Actor::IsAboveWater()
{
	return m_isAboveWater;
}

BOOL CRuFusion_Actor::IsInWater()
{
	return m_isInWater;
}

BOOL CRuFusion_Actor::IsSubmerged()
{
	return m_isSubmerged;
}

BOOL CRuFusion_Actor::EnableTorsoTurn(BOOL enableTorsoTurn)
{
	m_enableTorsoTurn = enableTorsoTurn;
	return m_enableTorsoTurn;
}

void CRuFusion_Actor::SetEnableRebasingOnUpVector(BOOL enable)
{
	m_enableRebasingOnUpVector = enable;
}

void CRuFusion_Actor::SetUpVector(const CRuVector3 &upVector)
{
	m_upVector = upVector;
/*
	REAL upVecLen = m_upVector.Magnitude();

	if((upVecLen > 1 + ruEPSILON) || (upVecLen < 1 - ruEPSILON))
	{
		MessageBoxA(NULL, "Fuck", "", MB_OK);
	}
*/
}

const CRuVector3 &CRuFusion_Actor::GetTargetForwardVector()
{
	return m_targetForwardVector;
}

const CRuVector3 &CRuFusion_Actor::GetCurrentForwardVector()
{
	return m_currentForwardVector;
}

void CRuFusion_Actor::SetForwardVector(CRuVector3 forwardVector, BOOL immediate)
{
	CRuVector3 entityReferenceForward(0.0f, 0.0f, -1.0f);
	CRuVector3 forwardVectorHaveY = forwardVector;

	// Pre-process the specified forward vector
	if(forwardVector.Magnitude() > ruEPSILON)
	{
		// Store Y length as tilt magnitude
		m_tiltMagnitude = forwardVector.m_y;

		// Flatten vertical magnitude - up/down movement is controlled separately by the tilt magnitude
		forwardVector.m_y = 0.0f;

		// Normalize the target forward vector
		if(forwardVector.Magnitude() < ruEPSILON)
		{
			forwardVector.Set(1.0f, 0.0f, 0.0f);
		}
		else
		{
			forwardVector.Normalize();
		}
	}
	else
	{
		// Reset tilt magnitude
		m_tiltMagnitude = 0.0f;

		// Use an arbitrary vector as forward vector
		forwardVector.Set(1.0f, 0.0f, 0.0f);
	}

	// Process the new forward vector based on movement type and actor state
	switch(m_movementType)
	{
		case ruSOJMOVETYPE_FULL:
			{
				switch(m_actorState)
				{
					case ruFUSION_ACTORSTATE_NORMAL:
					case ruFUSION_ACTORSTATE_JUMP:
					case ruFUSION_ACTORSTATE_FALLING:
					case ruFUSION_ACTORSTATE_SWIM_SURFACE:
					case ruFUSION_ACTORSTATE_SWIM_DIVE:
						{
							// Store new forward vector as the target forward vector
							m_targetForwardVector = forwardVector;

							// If the setting is to take effect immediately, apply target forward vector to current forward vector now
							if(immediate)
							{
								m_currentForwardVector = m_targetForwardVector;
							}

							// Orient entity toward forward current vector
							switch(m_orientMode)
							{
								case ruORIENTMODE_UNCONSTRAINED:
									RuEntity_Orient(m_targetEntity, entityReferenceForward, m_currentForwardVector, m_upVector, m_scale);
									break;

								case ruORIENTMODE_CONSTRAINED:
									RuEntity_OrientConstrained(m_targetEntity, entityReferenceForward, m_currentForwardVector, m_upVector, m_scale, 0.0f, m_curUpRotAngle);
									break;
							}

							// Tilt the entity toward the effective forward vector
							m_targetEntity->Rotate(m_tiltQuaternion);
						}

						break;

					default:
						{
							// Store new forward vector as the target forward vector
							m_targetForwardVector = forwardVector;

							// If the setting is to take effect immediately, apply target forward vector to current forward vector now
							if(immediate)
							{
								m_currentForwardVector = m_targetForwardVector;
							}

							// Orient entity toward forward current vector
							switch(m_orientMode)
							{
								case ruORIENTMODE_UNCONSTRAINED:
									RuEntity_Orient(m_targetEntity, entityReferenceForward, m_currentForwardVector, m_upVector, m_scale);
									break;

								case ruORIENTMODE_CONSTRAINED:
									RuEntity_OrientConstrained(m_targetEntity, entityReferenceForward, m_currentForwardVector, m_upVector, m_scale, 0.0f, m_curUpRotAngle);
									break;
							}

							// Tilt the entity toward the effective forward vector
							m_targetEntity->Rotate(m_tiltQuaternion);
						}

						break;
				}
			}

			break;

		case ruSOJMOVETYPE_NPC:
			{
				switch(m_actorState)
				{
					case ruFUSION_ACTORSTATE_NORMAL:
					case ruFUSION_ACTORSTATE_JUMP:
					case ruFUSION_ACTORSTATE_FALLING:
					case ruFUSION_ACTORSTATE_SWIM_SURFACE:
					case ruFUSION_ACTORSTATE_SWIM_DIVE:					
						{
							// Store new forward vector as the target forward vector
							m_targetForwardVector = forwardVector;

							// If the setting is to take effect immediately, apply target forward vector to current forward vector now
							if(immediate)
							{
								m_currentForwardVector = m_targetForwardVector;
							}

							// Orient entity toward forward current vector
							switch(m_orientMode)
							{
								case ruORIENTMODE_UNCONSTRAINED:
									RuEntity_Orient(m_targetEntity, entityReferenceForward, m_currentForwardVector, m_upVector, m_scale);
									break;

								case ruORIENTMODE_CONSTRAINED:
									RuEntity_OrientConstrained(m_targetEntity, entityReferenceForward, m_currentForwardVector, m_upVector, m_scale, 0.0f, m_curUpRotAngle);
									break;
							}

							// Tilt the entity toward the effective forward vector
							m_targetEntity->Rotate(m_tiltQuaternion);
						}
						break;

					case ruFUSION_ACTORSTATE_SPIRAL_RIGHT:
					case ruFUSION_ACTORSTATE_SPIRAL_LEFT:
					case ruFUSION_ACTORSTATE_SPIRAL_LOOP:
					case ruFUSION_ACTORSTATE_FLY_BEGIN:
					case ruFUSION_ACTORSTATE_FLY_LOOP:
					case ruFUSION_ACTORSTATE_FLY_END:
						{
							// Store new forward vector as the target forward vector
							m_targetForwardVector = forwardVectorHaveY;

							// If the setting is to take effect immediately, apply target forward vector to current forward vector now
							if(immediate)
							{
								m_currentForwardVector = m_targetForwardVector;
							}

							CRuQuaternion quaternion[3];
							quaternion[0].RotateIntoVector(m_upVector, CRuVector3(0.0f, 1.0f, 0.0f));

							CRuVector3 defaultForward(0.0f, 0.0f, -1.0f);
							quaternion[0].TransformVector(defaultForward, defaultForward);
							quaternion[1].RotateIntoVector(m_currentForwardVector, defaultForward, m_upVector);
							quaternion[2] = quaternion[0] * quaternion[1];
							quaternion[2] = quaternion[2] * m_tiltQuaternion;

							m_targetEntity->Rotate(quaternion[2], ruTRANSFORMOP_SET);
						}
						break;

					default:
						break;
				}
			}

			break;
	}
}

void CRuFusion_Actor::SetGravityAcceleration(REAL gravityAcceleration)
{
	m_gravityAcceleration = gravityAcceleration;

	if(fabs(m_gravityAcceleration) < ruEPSILON)
	{
		m_sojourner->SetWalkingSurfaceSnapping(FALSE);
	}
	else
	{
		m_sojourner->SetWalkingSurfaceSnapping(TRUE);
	}

	m_gravitySpeed = 0.0f;
}

void CRuFusion_Actor::SetRunThreshold(REAL runThreshold)
{
	m_runThreshold = runThreshold;
}

void CRuFusion_Actor::SetScale(REAL scale)
{
	m_scale = scale;
}

void CRuFusion_Actor::SetDefaultIdleAction(CRuFusion_Mime_Action defaultIdleAction)
{
	m_defaultIdleAction = defaultIdleAction;
}

void CRuFusion_Actor::SetDefaultWalkingAction(CRuFusion_Mime_Action defaultWalkingAction)
{
	m_defaultWalkingAction = defaultWalkingAction;
}

void CRuFusion_Actor::SetDefaultRunningAction(CRuFusion_Mime_Action defaultRunningAction, CRuFusion_Mime_Action defaultBackAction)
{
	m_defaultRunningAction = defaultRunningAction;

	if(defaultBackAction == ruFUSION_MIME_NONE)
	{
		//m_defaultBackAction = ruFUSION_MIME_RUN_BACKWARD;
	}
	else
	{
		m_defaultBackAction = defaultBackAction;
	}
}

void CRuFusion_Actor::SetDefaultBackAction(CRuFusion_Mime_Action defaultBackAction)
{
	m_defaultBackAction = defaultBackAction;
}

CRuFusion_Mime_Action CRuFusion_Actor::GetDefaultIdleAction()
{
	return m_defaultIdleAction;
}

CRuFusion_Mime_Action CRuFusion_Actor::GetDefaultWalkingAction()
{
	return m_defaultWalkingAction;
}

CRuFusion_Mime_Action CRuFusion_Actor::GetDefaultRunningAction()
{
	return m_defaultRunningAction;
}

CRuFusion_Mime_Action CRuFusion_Actor::GetDefaultBackAction()
{
	return m_defaultBackAction;
}

void CRuFusion_Actor::Move(const CRuVector3 &impulse)
{
	m_inputVelocity.m_x = impulse.m_x;
	m_inputVelocity.m_z = impulse.m_z;
}

BOOL CRuFusion_Actor::Jump(REAL risingImpulse)
{
	if(m_movementType == ruSOJMOVETYPE_FULL)
	{
		switch(m_actorState)
		{
			case ruFUSION_ACTORSTATE_NORMAL:
				{
					if(!m_disableInPlaceJump || m_inputVelocity.Magnitude() > ruEPSILON)
					{
						// Make sure the actor is in a position to jump
						if(m_stopPoint_OffsetFromBottom > 1.0f)
						{
							return FALSE;
						}

						// Cap minimum jumping period
						if(m_timeSinceLastJump < 0.50f)
						{
							return FALSE;
						}

						m_timeSinceLastJump = 0.0f;

						// Update mime state
						m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_JUMP_UP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_FALLING, NULL, FALSE);
						m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_JUMP_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_FALLING, NULL, FALSE);

						// Store current input velocity as latent velocity, and apply the rising impulse to gravity
						m_latentVelocity = m_inputVelocity;
						m_gravitySpeed = risingImpulse;

						// Transform latent velocity into reference frame
						m_refFrameBasis.TransformVector(m_latentVelocity);

						// Transform latent velocity into entity local frame
						CRuQuaternion forwardQuat;
						forwardQuat.RotateIntoVector(m_targetForwardVector, CRuVector3(0.0f, 0.0f, -1.0f));
						forwardQuat.TransformVector(m_latentVelocity);
					}
					else
					{
						m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_JUMP_INPLACE, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
					}
				}

				return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuFusion_Actor::ForcedJump(REAL risingImpulse, const CRuVector3 &latentVelocity)
{
	// Update mime state
	m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_JUMP_UP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_FALLING, NULL, FALSE);
	m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_JUMP_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_FALLING, NULL, FALSE);

	// Store current input velocity as latent velocity, and apply the rising impulse to gravity
	m_latentVelocity = latentVelocity;
	m_gravitySpeed = risingImpulse;

	return TRUE;
}

BOOL CRuFusion_Actor::DisableMovement(BOOL disableMovement)
{
	m_disableMovement = disableMovement;

	return TRUE;
}

BOOL CRuFusion_Actor::DisableInPlaceJump(BOOL disableInPlaceJump)
{
	m_disableInPlaceJump = disableInPlaceJump;

	return TRUE;
}

BOOL CRuFusion_Actor::Knockdown(const CRuVector3 &knockdownDirection)
{
	// Reset related parameters
	m_gravitySpeed = 0.0f;

	m_actorState = ruFUSION_ACTORSTATE_KNOCKDOWN_BEGIN;


	return TRUE;
}

BOOL CRuFusion_Actor::NotifyPositionReset()
{
	if(m_movementType == ruSOJMOVETYPE_NPC)
	{
		// Perform state-dependent actions
		switch(m_actorState)
		{
			default:
				break;

			case ruFUSION_ACTORSTATE_FALLING:
				m_actorState = ruFUSION_ACTORSTATE_NORMAL;
				break;
		}

		// Reset all latent velocities
		m_latentVelocity.Set(0.0f, 0.0f, 0.0f);
		m_gravitySpeed = 0.0f;
		m_onFirmGround = TRUE;
	}

	return TRUE;
}

REAL CRuFusion_Actor::GetGravitySpeed() const
{
	return m_gravitySpeed;
}

const CRuVector3 &CRuFusion_Actor::GetLatentVelocity() const
{
	return m_latentVelocity;
}

CRuFusion_ActorState CRuFusion_Actor::GetBackgroundState()
{
	return m_actorState;
}

BOOL CRuFusion_Actor::SetBackgroundState(CRuFusion_ActorState actorState, BOOL force)
{
/*
	if(RuEntity_FindFirstPaperdoll(m_targetEntity))
	{
		char moo[200];
		sprintf(moo, "[ACTOR] Background state set to %d.\n", actorState);
		OutputDebugString(moo);
	}
*/
	if(m_actorState == ruFUSION_ACTORSTATE_JUMP || m_actorState == ruFUSION_ACTORSTATE_FALLING)
	{
		if(!force)
		{
			return FALSE;
		}
	}

	BOOL retVal = FALSE;

	switch(actorState)
	{
		case ruFUSION_ACTORSTATE_SIT_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SIT_DOWN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_SIT_BEGIN, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SIT_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_SIT_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_SIT_LOOP:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SIT_LOOP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_SIT_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_SIT_END:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SIT_UP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_SIT_END, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_SLEEP_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SLEEP_DOWN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_SLEEP_BEGIN, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SLEEP_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_SLEEP_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_SLEEP_LOOP:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SLEEP_LOOP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_SLEEP_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_SLEEP_END:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SLEEP_UP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_SLEEP_END, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_DYING:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_DEATH_DOWN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_DYING, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_DEATH_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_DEAD, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_DEAD:
			{
				m_deadTime = 0.0f;
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_DEATH_LOOP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_DEAD, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_REVIVING:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_DEATH_UP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_REVIVING, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_SPAWN_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SPAWN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_SPAWN_END, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_SPAWN_END:
			{

			}
			break;

		case ruFUSION_ACTORSTATE_DEAD_WATER:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_DEAD_WATER, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_DEAD_WATER, NULL, FALSE);
			}
			break;

		case ruFUSION_ACTORSTATE_CROUCH_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_CROUCH_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_CROUCH_BEGIN, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_CROUCH_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_CROUCH_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_CROUCH_LOOP:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_CROUCH_LOOP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_CROUCH_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_CROUCH_END:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_CROUCH_END, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_CROUCH_END, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_CRAFTING_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_CRAFTING_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_CRAFTING_BEGIN, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_CRAFTING_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_CRAFTING_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_CRAFTING_LOOP:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_CRAFTING_LOOP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_CRAFTING_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_CRAFTING_END:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_CRAFTING_END, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_CRAFTING_END, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_MINING_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_MINING_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_MINING_BEGIN, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_MINING_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_MINING_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_MINING_LOOP:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_MINING_LOOP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_MINING_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_MINING_END:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_MINING_END, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_MINING_END, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_MINING_END_HOLD:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_MINING_END, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_MINING_END, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_CAST_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_HOLDING, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_LUMBERJACK_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_LUMBERJACK_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_LUMBERJACK_BEGIN, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_LUMBERJACK_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_LUMBERJACK_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_LUMBERJACK_LOOP:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_LUMBERJACK_LOOP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_LUMBERJACK_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_LUMBERJACK_END:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_LUMBERJACK_END, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_LUMBERJACK_END, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_GATHER_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_GATHER_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_GATHER_BEGIN, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_GATHER_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_GATHER_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_GATHER_LOOP:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_GATHER_LOOP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_GATHER_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_GATHER_END:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_GATHER_END, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_GATHER_END, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_LOOT_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_LOOT_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_LOOT_BEGIN, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_LOOT_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_LOOT_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_LOOT_LOOP:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_LOOT_LOOP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_LOOT_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_LOOT_END:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_LOOT_END, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_LOOT_END, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_CARRY_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_CARRY_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_CARRY_BEGIN, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_CARRY_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_CARRY_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_CARRY_LOOP:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_CARRY_LOOP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_CARRY_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_CARRY_END:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_CARRY_END, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_CARRY_END, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_DAZED_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_DAZED, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_DAZED_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_DAZED_END:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_SLUMBER_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SLUMBER, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_SLUMBER_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_SLUMBER_END:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_ACTIVATE_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_ACTIVATE_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_ACTIVATE_BEGIN, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_ACTIVATE_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_ACTIVATE_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_ACTIVATE_LOOP:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_ACTIVATE_LOOP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_ACTIVATE_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_ACTIVATE_END:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_ACTIVATE_END, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_ACTIVATE_END, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_READING_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_READING_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_READING_BEGIN, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_READING_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_READING_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_READING_LOOP:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_READING_LOOP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_READING_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_READING_END:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_READING_END, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_READING_END, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_SIT_CHAIR_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SIT_CHAIR_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_SIT_CHAIR_BEGIN, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SIT_CHAIR_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_SIT_CHAIR_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_SIT_CHAIR_LOOP:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SIT_CHAIR_LOOP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_SIT_CHAIR_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_SIT_CHAIR_END:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SIT_CHAIR_END, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_SIT_CHAIR_END, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_KNOCKDOWN_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_KNOCKDOWN_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_KNOCKDOWN_BEGIN, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_KNOCKDOWN_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_KNOCKDOWN_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_KNOCKDOWN_LOOP:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_KNOCKDOWN_LOOP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_KNOCKDOWN_LOOP, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_KNOCKDOWN_END:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_KNOCKDOWN_END, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_KNOCKDOWN_END, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_KNOCKDOWN_RECOVER, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_KNOCKDOWN_RECOVER, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_KNOCKDOWN_RECOVER:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_KNOCKDOWN_RECOVER, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_KNOCKDOWN_RECOVER, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_EMOTE_HANDSTAND:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_EMOTE_HANDSTAND, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_EMOTE_HANDSTAND, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}
			break;
		case ruFUSION_ACTORSTATE_EMOTE_ROFL:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_EMOTE_ROFL, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_EMOTE_ROFL, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}
			break;

		case ruFUSION_ACTORSTATE_EMOTE_SWEEP1:
			{				
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_EMOTE_SWEEP1, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_EMOTE_SWEEP1, NULL, FALSE);
			}
			break;
		case ruFUSION_ACTORSTATE_EMOTE_SWEEP2:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_EMOTE_SWEEP2, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_EMOTE_SWEEP2, NULL, FALSE);
			}
			break;
		case ruFUSION_ACTORSTATE_EMOTE_SWEEP3:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_EMOTE_SWEEP3, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_EMOTE_SWEEP3, NULL, FALSE);
			}
			break;

		case ruFUSION_ACTORSTATE_SPIRAL_RIGHT:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SPIRAL_RIGHT, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_SPIRAL_RIGHT, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_FLY_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_FLY_LOOP, NULL, FALSE);
			}
			break;

		case ruFUSION_ACTORSTATE_SPIRAL_LEFT:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SPIRAL_LEFT, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_SPIRAL_LEFT, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_FLY_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_FLY_LOOP, NULL, FALSE);
			}
			break;

		case ruFUSION_ACTORSTATE_SPIRAL_LOOP:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SPIRAL_LOOP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_SPIRAL_LOOP, NULL, FALSE);
			}
			break;

		case ruFUSION_ACTORSTATE_FLY_LOOP:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_FLY_LOOP, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_FLY_LOOP, NULL, FALSE);
			}
			break;
		
		case ruFUSION_ACTORSTATE_FLY_BEGIN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_FLY_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_FLY_BEGIN, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_FLY_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_SPIRAL_LOOP, NULL, FALSE);
			}
			break;

		case ruFUSION_ACTORSTATE_FLY_END:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_FLY_END, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, ruFUSION_ACTORSTATE_FLY_END, NULL, FALSE);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_IDLE_STAND, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
			}
			break;

		default:
			m_actorState = actorState;
			retVal = TRUE;
	}


	return retVal;
}

CRuFusion_ActorState CRuFusion_Actor::GetForegroundState()
{
	return m_foregroundState;
}

BOOL CRuFusion_Actor::SetForegroundState(CRuFusion_ActorState actorState)
{
/*
	if(m_movementType == ruSOJMOVETYPE_FULL)
	{
		char moo[200];
		sprintf(moo, "Set Foreground State: %d\n", actorState);
		OutputDebugString(moo);
	}
*/
	switch(actorState)
	{
		case ruFUSION_ACTORSTATE_NORMAL:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_NONE, ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), NULL, TRUE);
			}

			break;

		case ruFUSION_ACTORSTATE_COMBAT_1H:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_IDLE_COMBAT_1H, ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), NULL, TRUE);
			}

			break;

		case ruFUSION_ACTORSTATE_COMBAT_2H:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_IDLE_COMBAT_2H, ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), NULL, TRUE);
			}

			break;

		case ruFUSION_ACTORSTATE_COMBAT_BOW:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_IDLE_COMBAT_BOW, ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), NULL, TRUE);
			}

			break;

		case ruFUSION_ACTORSTATE_COMBAT_GUN:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_IDLE_COMBAT_GUN, ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), NULL, TRUE);
			}

			break;

		case ruFUSION_ACTORSTATE_COMBAT_POLEARM:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_IDLE_COMBAT_POLEARM, ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), NULL, TRUE);
			}

			break;

		case ruFUSION_ACTORSTATE_COMBAT_UNARMED:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_IDLE_COMBAT_UNARMED, ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), NULL, TRUE);
			}

			break;
	}

	if(actorState != ruFUSION_ACTORSTATE_NORMAL)
	{
		// Reset idling time
		m_idlingTime = 0.0f;
	}

	m_markedForegroundState = actorState;

	return TRUE;
}

CRuFusion_ActorState CRuFusion_Actor::GetTransientState()
{
	return m_transientState;
}

BOOL CRuFusion_Actor::SetTransientState(CRuFusion_ActorState actorState, BOOL disableMovement, CRuACT_ActionEvents *actionEventsOut)
{
/*
	if(m_movementType == ruSOJMOVETYPE_FULL)
	{
		char moo[200];
		sprintf(moo, "Set Transient State: %d\n", actorState);
		OutputDebugString(moo);
	}
*/
	if(disableMovement)
	{
		m_disableMovementOnAction = actorState;
	}

	switch(actorState)
	{
		case ruFUSION_ACTORSTATE_NORMAL:
			{
				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_ATTACK_1H:
		case ruFUSION_ACTORSTATE_ATTACK_1H_SP:
		case ruFUSION_ACTORSTATE_ATTACK_1H_SP02:
		case ruFUSION_ACTORSTATE_ATTACK_1H_SLOW01:
		case ruFUSION_ACTORSTATE_ATTACK_1H_PIERCE:
		case ruFUSION_ACTORSTATE_ATTACK_2H:
		case ruFUSION_ACTORSTATE_ATTACK_2H_SP:
		case ruFUSION_ACTORSTATE_ATTACK_POLEARM:
		case ruFUSION_ACTORSTATE_ATTACK_POLEARM_SP:
		case ruFUSION_ACTORSTATE_ATTACK_UNARMED:
		case ruFUSION_ACTORSTATE_ATTACK_UNARMED_SP:
			{
				// Execute attack motion
				INT32 actionOffset = actorState - ruFUSION_ACTORSTATE_ATTACK_1H;
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, static_cast<CRuFusion_Mime_Action>(ruFUSION_MIME_ATTACK_1H + actionOffset), ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_ATTACK_BOW_BEGIN:
			{
				// Execute attack motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_ATTACK_BOW_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_ATTACK_BOW_LOOP), actionEventsOut, FALSE);

				// Enter next motion stage
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_ATTACK_BOW_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_ATTACK_BOW_LOOP), NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_ATTACK_BOW_END:
			{
				// Execute end motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_ATTACK_BOW_END, ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_ATTACK_BOW_SP_BEGIN:
			{
				// Execute attack motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_ATTACK_BOW_SP_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_ATTACK_BOW_SP_LOOP), actionEventsOut, FALSE);

				// Enter next motion stage
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_ATTACK_BOW_SP_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_ATTACK_BOW_SP_LOOP), NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_ATTACK_BOW_SP_END:
			{
				// Execute end motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_ATTACK_BOW_SP_END, ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_ATTACK_GUN_BEGIN:
			{
				// Execute attack motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_ATTACK_GUN_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_ATTACK_GUN_LOOP), actionEventsOut, FALSE);

				// Enter next motion stage
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_ATTACK_GUN_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_ATTACK_GUN_LOOP), NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_ATTACK_GUN_END:
			{
				// Execute end motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_ATTACK_GUN_END, ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_ATTACK_OH:
			{
				// Execute attack motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_ATTACK_OH, ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_ATTACK_OH_PIERCE:
			{
				// Execute attack motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_ATTACK_OH_PIERCE, ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_PARRY_1H:
		case ruFUSION_ACTORSTATE_PARRY_2H:
		case ruFUSION_ACTORSTATE_PARRY_BOW:
		case ruFUSION_ACTORSTATE_PARRY_GUN:
		case ruFUSION_ACTORSTATE_PARRY_POLEARM:
		case ruFUSION_ACTORSTATE_PARRY_UNARMED:
			{
				// Execute PARRY motion
				INT32 actionOffset = actorState - ruFUSION_ACTORSTATE_PARRY_1H;
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, static_cast<CRuFusion_Mime_Action>(ruFUSION_MIME_PARRY_1H + actionOffset), ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_SHIELD_BASH:
			{
				// Execute PARRY motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_SHIELD_BASH, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_SHIELD_BLOCK:
			{
				// Execute PARRY motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_SHIELD_BLOCK, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_USE:
			{
				// Execute PARRY motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_USE, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_THROW:
			{
				// Execute PARRY motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_THROW, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_BUFF_BEGIN:
			{
				// Execute attack motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_BUFF_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_BUFF_LOOP), actionEventsOut, FALSE);

				// Enter next motion stage
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_BUFF_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_BUFF_LOOP), NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_BUFF_END:
			{
				// Execute end motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_BUFF_END, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_BUFF_INSTANT:
			{
				// Execute attack motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_BUFF_INSTANT, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_BUFF_SP01:
		case ruFUSION_ACTORSTATE_BUFF_SP02:
		case ruFUSION_ACTORSTATE_BUFF_SP03:
		case ruFUSION_ACTORSTATE_CAST_SP01:
		case ruFUSION_ACTORSTATE_CAST_SP02:
		case ruFUSION_ACTORSTATE_CAST_SP03:
		case ruFUSION_ACTORSTATE_CAST_SP04:
			{
				// Execute motion
				INT32 actionOffset = actorState - ruFUSION_ACTORSTATE_BUFF_SP01;
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, static_cast<CRuFusion_Mime_Action>(ruFUSION_ACTORSTATE_BUFF_SP01 + actionOffset), ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_CAST_BEGIN:
			{
				// Execute attack motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_CAST_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_CAST_LOOP), actionEventsOut, FALSE);
//				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_CAST_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_CAST_LOOP), actionEventsOut);

				// Enter next motion stage
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_CAST_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_CAST_LOOP), NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_CAST_END:
			{
				// Execute end motion
//				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_CAST_END, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut);
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_CAST_END, ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_CAST_INSTANT:
			{
				// Execute attack motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_CAST_INSTANT, ruFUSION_MIME_SETACTIONMETHOD_QUEUED, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);
//				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_CAST_INSTANT, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_DEBUFF_INSTANT:
			{
				// Execute attack motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_DEBUFF_INSTANT, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_CHANNEL_BEGIN:
			{
				// Execute attack motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_CHANNEL_BEGIN, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_CHANNEL_LOOP), actionEventsOut, FALSE);

				// Enter next motion stage
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_CHANNEL_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_CHANNEL_LOOP), NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_CHANNEL_END:
			{
				// Execute end motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_CHANNEL_END, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_CHANNEL_INSTANT:
			{
				// Execute end motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_CHANNEL_INSTANT, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_SP_SPINKICK:
			{
				// Execute end motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_SP_SPINKICK, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_SP_SPINSLASH:
			{
				// Execute end motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_SP_SPINSLASH, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_FISHING_CAST:
			{
				// Execute attack motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_FISHING_CAST, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_CHANNEL_LOOP), actionEventsOut, FALSE);

				// Enter next motion stage
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_FISHING_IDLE, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_CHANNEL_LOOP), NULL, FALSE);
			}

			break;

		case ruFUSION_ACTORSTATE_FISHING_IDLE:
			{
				// Execute end motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_FISHING_IDLE, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_FISHING_REEL:
			{
				// Execute end motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_FISHING_REEL, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_EMOTE_ANGRY:
		case ruFUSION_ACTORSTATE_EMOTE_APPLAUSE:
		case ruFUSION_ACTORSTATE_EMOTE_APPROVAL:
		case ruFUSION_ACTORSTATE_EMOTE_BEG:
		case ruFUSION_ACTORSTATE_EMOTE_CRY:
		case ruFUSION_ACTORSTATE_EMOTE_HEAD_NOD:
		case ruFUSION_ACTORSTATE_EMOTE_HEAD_SHAKE:
		case ruFUSION_ACTORSTATE_EMOTE_LAUGH:
		case ruFUSION_ACTORSTATE_EMOTE_POINT:
		case ruFUSION_ACTORSTATE_EMOTE_PROVOKE:
		case ruFUSION_ACTORSTATE_EMOTE_SALUTE:
		case ruFUSION_ACTORSTATE_EMOTE_SPEAK:
		case ruFUSION_ACTORSTATE_EMOTE_SURRENDER:
		case ruFUSION_ACTORSTATE_EMOTE_THREATEN:
		case ruFUSION_ACTORSTATE_EMOTE_VICTORY:
		case ruFUSION_ACTORSTATE_EMOTE_WAVE:
		case ruFUSION_ACTORSTATE_EMOTE_EAT:
		case ruFUSION_ACTORSTATE_EMOTE_DRINK:
		case ruFUSION_ACTORSTATE_EMOTE_IDLE:
		case ruFUSION_ACTORSTATE_EMOTE_IDLE0:
		case ruFUSION_ACTORSTATE_EMOTE_IDLE1:
		case ruFUSION_ACTORSTATE_EMOTE_IDLE2:
		case ruFUSION_ACTORSTATE_EMOTE_DANCE:
		case ruFUSION_ACTORSTATE_EMOTE_PLAY_SAXOPHONE:
		case ruFUSION_ACTORSTATE_EMOTE_PLAY_TAMBOURINE:
		case ruFUSION_ACTORSTATE_EMOTE_PLAY_GUITAR:
		case ruFUSION_ACTORSTATE_EMOTE_JUMP_INPLACE:
		case ruFUSION_ACTORSTATE_EMOTE_BELLY:
		case ruFUSION_ACTORSTATE_EMOTE_EMBRAASSED:
		case ruFUSION_ACTORSTATE_EMOTE_EMBRACE:
		case ruFUSION_ACTORSTATE_EMOTE_KISS:
		case ruFUSION_ACTORSTATE_EMOTE_SALUTE2:
		case ruFUSION_ACTORSTATE_EMOTE_CUTE1:
		case ruFUSION_ACTORSTATE_EMOTE_CUTE2:
		case ruFUSION_ACTORSTATE_EMOTE_STRETCH:
		case ruFUSION_ACTORSTATE_EMOTE_ROCK:
		case ruFUSION_ACTORSTATE_EMOTE_KAMEHAMEHA:
		case ruFUSION_ACTORSTATE_EMOTE_KICK:		
		case ruFUSION_ACTORSTATE_EMOTE_BREAKDANCE:
		case ruFUSION_ACTORSTATE_EMOTE_TAPDANCE:
		case ruFUSION_ACTORSTATE_EMOTE_TIRED:
		case ruFUSION_ACTORSTATE_EMOTE_FEAR:
		case ruFUSION_ACTORSTATE_EMOTE_AMAZED:
		case ruFUSION_ACTORSTATE_EMOTE_THINK:
		case ruFUSION_ACTORSTATE_EMOTE_CUTE3:
		case ruFUSION_ACTORSTATE_EMOTE_HANGOVER:
			{
				CRuFusion_Mime_Action mimeAction = (CRuFusion_Mime_Action) (ruFUSION_MIME_EMOTE_ANGRY + (actorState - ruFUSION_ACTORSTATE_EMOTE_ANGRY));

				// Execute end motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, mimeAction, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_HOLSTER:
		case ruFUSION_ACTORSTATE_UNHOLSTER:
		case ruFUSION_ACTORSTATE_HOLSTER_2H:
		case ruFUSION_ACTORSTATE_UNHOLSTER_2H:
			{
				CRuFusion_Mime_Action mimeAction = (CRuFusion_Mime_Action) (ruFUSION_MINE_HOLSTER + (actorState - ruFUSION_ACTORSTATE_HOLSTER));

				// Execute end motion
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, mimeAction, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), actionEventsOut, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;
	}

	// Reset idling time
	m_idlingTime = 0.0f;

	return TRUE;
}

BOOL CRuFusion_Actor::SetAuxiliaryState(CRuFusion_ActorState actorState)
{
	CRuFusion_ActorState				oldForegroundState = m_foregroundState;
	CRuFusion_Mime_Action				oldForegroundMimeAction = m_foregroundMimeAction;

	CRuFusion_ActorState				oldTransientState = m_transientState;
	CRuFusion_Mime_Action				oldTransientMimeAction = m_transientMimeAction;

	// If current foreground state is not one of the allowed ones, ignore action
	switch(m_transientState)
	{
		case ruFUSION_ACTORSTATE_NORMAL:
		case ruFUSION_ACTORSTATE_COMBAT_1H:
		case ruFUSION_ACTORSTATE_COMBAT_2H:
		case ruFUSION_ACTORSTATE_COMBAT_BOW:
		case ruFUSION_ACTORSTATE_COMBAT_GUN:
		case ruFUSION_ACTORSTATE_COMBAT_POLEARM:
		case ruFUSION_ACTORSTATE_COMBAT_UNARMED:
			break;

		default:
			return TRUE;
	}

	switch(actorState)
	{
		case ruFUSION_ACTORSTATE_HURT_NORMAL:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_HURT_NORMAL, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), NULL, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_HURT_CRITICAL:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_HURT_CRITICAL, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), NULL, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;

		case ruFUSION_ACTORSTATE_DODGE:
			{
				m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_DODGE, ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, static_cast<PTRVALUE>(actorState), NULL, FALSE);

				// Restore original action and motion
				m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
			}

			break;
	}

	return TRUE;
}

BOOL CRuFusion_Actor::ClearForegroundQueue()
{
	return m_targetMime->ClearQueuedActions(ruFUSION_MIME_CHANNEL_FOREGROUND);
}

void CRuFusion_Actor::LinkActor(CRuFusion_Actor *linkedActor)
{
	if ( linkedActor == NULL )
		return;

	// Unlink any previous links
	if(linkedActor->m_linkParent)
	{
		linkedActor->m_linkParent->UnlinkActor(linkedActor);
	}

	// Establish Actor link
	linkedActor->m_linkParent = this;
	m_linkedActors.Add(linkedActor);
}

void CRuFusion_Actor::UnlinkActor(CRuFusion_Actor *linkedActor)
{
	if ( linkedActor == NULL )
		return;

	// Remove Actor from linked Actor list
	for(INT32 i = 0; i < m_linkedActors.Count(); ++i)
	{
		if(m_linkedActors[i] == linkedActor)
		{
			m_linkedActors.RemoveAt(i);
			break;
		}
	}

	// Reset link parent
	linkedActor->m_linkParent = NULL;
}

void CRuFusion_Actor::SeverActorLinks()
{
	// Unlink from parent
	if(m_linkParent)
	{
		m_linkParent->UnlinkActor(this);
	}

	// Unlink children
	for(INT32 i = 0; i < m_linkedActors.Count(); ++i)
	{
		m_linkedActors[i]->m_linkParent = NULL;
	}

	m_linkedActors.Clear();
}

CRuFusion_Actor *CRuFusion_Actor::GetLinkParent()
{
	return m_linkParent;
}

void CRuFusion_Actor::Update(REAL elapsedTime)
{
	BOOL doNotInterpolateHeadingOffsets = FALSE;
	CRuVector3 staticTurnAxis(0.0f, 1.0f, 0.0f);
	REAL staticTurnAngle = 0.0f;
	INT32 shufflingDirection = 0;
	REAL m_frictionSurfaceThreshold = 0.95f;
	REAL m_frictionConstant = 1.0f;

	CRuVector3 entityReferenceForward(0.0f, 0.0f, -1.0f);

	// Accumulate update time
	m_latentPeriod += elapsedTime;

	// If the update period is too short, return
	if(m_latentPeriod < 0.01000f)
		return;

	m_timeSinceLastJump += m_latentPeriod;

	RUPROFILE_SAMPLE_BEGIN(m_probe_ActorUpdate, 3)

	// Update current forward vector
	switch(m_movementType)
	{
		case ruSOJMOVETYPE_FULL:
			{
				switch(m_actorState)
				{
					case ruFUSION_ACTORSTATE_NORMAL:
					case ruFUSION_ACTORSTATE_JUMP:
					case ruFUSION_ACTORSTATE_FALLING:
					case ruFUSION_ACTORSTATE_SWIM_SURFACE:
					case ruFUSION_ACTORSTATE_SWIM_DIVE:
						{
							// Calculate torso turn angle
							CalculateAngleDifferential(m_currentForwardVector, m_targetForwardVector, staticTurnAxis, staticTurnAngle);

							// If the actor is moving, or we have stopped turning for a while, or if torso turning is disabled, eliminate torso turn
							if(!m_enableTorsoTurn)
							{
								m_currentForwardVector = m_targetForwardVector;
								staticTurnAngle = 0.0f;
							}
							else if(m_inputVelocity.Magnitude() > ruEPSILON)
							{
								REAL angle = RuMathHelper_StepInterpolate(0.0f, staticTurnAngle, 20.00f, 10.00f, elapsedTime);

								if(angle < 0.05f)
								{
									m_currentForwardVector = m_targetForwardVector;
								}
								else
								{
									// Turn current forward vector
									CRuQuaternion excessRotQuat;
									excessRotQuat.FromAngleAxis(staticTurnAxis, angle);
									excessRotQuat.TransformVector(m_currentForwardVector);
								}

								staticTurnAngle = 0.0f;
							}
							else
							{
								REAL torsoTurnLimit = ruPI / 4.0f;

								if(staticTurnAngle > torsoTurnLimit)
								{
									// Cap static turn angle and calculate the excess turn angle
									REAL excessTurnAngle = staticTurnAngle - torsoTurnLimit;
									staticTurnAngle = torsoTurnLimit;

									shufflingDirection = -1;
									m_shufflingTime = 0.0f;

									// Turn current forward vector
									CRuQuaternion excessRotQuat;
									excessRotQuat.FromAngleAxis(staticTurnAxis, excessTurnAngle);
									excessRotQuat.TransformVector(m_currentForwardVector);
								}
								else if(staticTurnAngle < -torsoTurnLimit)
								{
									// Cap static turn angle and calculate the excess turn angle
									REAL excessTurnAngle = staticTurnAngle + torsoTurnLimit;
									staticTurnAngle = -torsoTurnLimit;

									shufflingDirection = 1;
									m_shufflingTime = 0.0f;

									// Turn current forward vector
									CRuQuaternion excessRotQuat;
									excessRotQuat.FromAngleAxis(staticTurnAxis, excessTurnAngle);
									excessRotQuat.TransformVector(m_currentForwardVector);
								}
							}
						}

						break;

					default:
						break;
				}
			}

			break;

		case ruSOJMOVETYPE_NPC:
			{
				CRuQuaternion curQuat, dstQuat, finalQuat;

				curQuat.RotateIntoVector(m_currentForwardVector, CRuVector3(1.0f, 0.0f, 0.0f));
				dstQuat.RotateIntoVector(m_targetForwardVector, CRuVector3(1.0f, 0.0f, 0.0f));

				REAL factor = 10.0f;

				REAL relDiff = DotProduct(m_currentForwardVector, m_targetForwardVector);
				if(relDiff > 0.975f)
				{
					m_currentForwardVector = m_targetForwardVector;
				}
				else
				{
					if(TripleProduct_Scalar(m_currentForwardVector, m_targetForwardVector, CRuVector3(0.0f, 1.0f, 0.0f)) > 0.0f)
						shufflingDirection = -1;
					else
						shufflingDirection = 1;

					finalQuat = QuaternionLerp(curQuat, dstQuat, factor * elapsedTime);
					finalQuat.TransformVector(CRuVector3(1.0f, 0.0f, 0.0f), m_currentForwardVector);
					m_currentForwardVector.Normalize();
				}
			}

			break;
	}

	// Build rotation for current forward direction of movement
	CRuQuaternion forwardQuat;
	forwardQuat.RotateIntoVector(m_targetForwardVector, CRuVector3(0.0f, 0.0f, -1.0f));

	CRuVector3 actorOffset(0.0f, 9.0f, 0.0f);
	CRuVector3 actorEllipsoid(4.0f, 9.0f, 4.0f);
	CRuVector3 origin = m_targetEntity->GetTranslation() + actorOffset;
	CRuVector3 destination0 = origin;
	CRuVector3 destination1 = origin;

	REAL speed = 0.0f;
	CRuVector3 instantaneousVelocityGradient;
	CRuVector3 instantaneousGravityGradient;

	BOOL tiltEnabled = FALSE;
	CRuVector3 effectiveForwardVector(m_currentForwardVector);

	switch(m_actorState)
	{
		case ruFUSION_ACTORSTATE_NORMAL:
			{
				// Transform input velocity into entity local frame
				CRuVector3 localVelocity;
				m_refFrameBasis.TransformVector(m_inputVelocity, localVelocity);
				forwardQuat.TransformVector(localVelocity);

				// Calculate instantaneous velocity
				speed = localVelocity.Magnitude();
				instantaneousVelocityGradient = localVelocity * m_latentPeriod;
				instantaneousGravityGradient.Set(0.0f, m_gravitySpeed * m_latentPeriod, 0.0f);
			}

			break;

		case ruFUSION_ACTORSTATE_JUMP:
		case ruFUSION_ACTORSTATE_FALLING:
		case ruFUSION_ACTORSTATE_KNOCKDOWN_BEGIN:
		case ruFUSION_ACTORSTATE_KNOCKDOWN_LOOP:
		case ruFUSION_ACTORSTATE_KNOCKDOWN_END:
		case ruFUSION_ACTORSTATE_KNOCKDOWN_RECOVER:
			{
				// NOTE: Maybe time to separate NPC and Full modes into entirely different functions so I can actually read this code!
				if(m_actorState == ruSOJMOVETYPE_NPC)
				{
					// Transform input velocity into entity local frame
					CRuVector3 localVelocity;
					m_refFrameBasis.TransformVector(m_inputVelocity, localVelocity);
					forwardQuat.TransformVector(localVelocity);
				}

				// Calculate instantaneous velocity
				speed = m_latentVelocity.Magnitude();
				instantaneousVelocityGradient = m_latentVelocity * m_latentPeriod;
				instantaneousGravityGradient.Set(0.0f, m_gravitySpeed * m_latentPeriod, 0.0f);
			}

			break;

		case ruFUSION_ACTORSTATE_SWIM_SURFACE:
			{
				// Transform input velocity into entity local frame
				CRuVector3 localVelocity;
				m_refFrameBasis.TransformVector(m_inputVelocity, localVelocity);

				if(fabs(m_tiltMagnitude) > 0.5f)
				{
					if(fabs(m_inputVelocity.m_z) > ruEPSILON)
					{
						tiltEnabled = TRUE;
					}

					// Separate input velocity into forward and side components
					CRuVector3 localForwardVelocity;
					localForwardVelocity.m_z = localVelocity.m_z;
					localVelocity.m_z = 0.0f;

					// Transform side component of input velocity by the normal forward rotation
					forwardQuat.TransformVector(localVelocity);

					// Modify forward quat by taking into account the tilt magnitude
					effectiveForwardVector.m_y = m_tiltMagnitude;
					effectiveForwardVector.Normalize();
					forwardQuat.RotateIntoVector(effectiveForwardVector, CRuVector3(0.0f, 0.0f, -1.0f));

					// Transform the z-axis movement
					forwardQuat.TransformVector(localForwardVelocity);

					// Re-combine forward and side components
					localVelocity = localForwardVelocity + localVelocity;
				}
				else
				{
					forwardQuat.TransformVector(localVelocity);
				}

				// Calculate instantaneous velocity
				speed = localVelocity.Magnitude();
				instantaneousVelocityGradient = localVelocity * m_latentPeriod;
				instantaneousGravityGradient.Set(0.0f, 0.0f, 0.0f);

				// No gravity while in water
				m_gravitySpeed = 0.0f;
			}

			break;

		case ruFUSION_ACTORSTATE_SWIM_DIVE:
			{
				// Transform input velocity into entity local frame
				CRuVector3 localVelocity;
				m_refFrameBasis.TransformVector(m_inputVelocity, localVelocity);

				if(fabs(m_inputVelocity.m_z) > ruEPSILON)
				{
					tiltEnabled = TRUE;
				}

				// Separate input velocity into forward and side components
				CRuVector3 localForwardVelocity;
				localForwardVelocity.m_z = localVelocity.m_z;
				localVelocity.m_z = 0.0f;

				// Transform side component of input velocity by the normal forward rotation
				forwardQuat.TransformVector(localVelocity);

				// Modify forward quat by taking into account the tilt magnitude
				effectiveForwardVector.m_y = m_tiltMagnitude;
				effectiveForwardVector.Normalize();
				forwardQuat.RotateIntoVector(effectiveForwardVector, CRuVector3(0.0f, 0.0f, -1.0f));

				// Transform the z-axis movement
				forwardQuat.TransformVector(localForwardVelocity);

				// Re-combine forward and side components
				localVelocity = localForwardVelocity + localVelocity;

				// Calculate instantaneous velocity
				speed = localVelocity.Magnitude();
				instantaneousVelocityGradient = localVelocity * m_latentPeriod;
				instantaneousGravityGradient.Set(0.0f, 0.0f, 0.0f);

				// No gravity while in water
				m_gravitySpeed = 0.0f;
			}

			break;

		case ruFUSION_ACTORSTATE_SIT_LOOP:
			{
				// Transform input velocity into entity local frame
				CRuVector3 localVelocity;
				m_refFrameBasis.TransformVector(m_inputVelocity, localVelocity);
				forwardQuat.TransformVector(localVelocity);

				// Calculate instantaneous velocity
				speed = localVelocity.Magnitude();

				if(speed > ruEPSILON)
				{
					this->SetBackgroundState(ruFUSION_ACTORSTATE_SIT_END);
				}

				speed = 0.0f;
				instantaneousVelocityGradient.Set(0.0f, 0.0f, 0.0f);
				instantaneousGravityGradient.Set(0.0f, m_gravitySpeed * m_latentPeriod, 0.0f);
			}

			break;

		case ruFUSION_ACTORSTATE_SLEEP_LOOP:
			{
				// Transform input velocity into entity local frame
				CRuVector3 localVelocity;
				m_refFrameBasis.TransformVector(m_inputVelocity, localVelocity);
				forwardQuat.TransformVector(localVelocity);

				// Calculate instantaneous velocity
				speed = localVelocity.Magnitude();

				if(speed > ruEPSILON)
				{
					this->SetBackgroundState(ruFUSION_ACTORSTATE_SLEEP_END);
				}

				speed = 0.0f;
				instantaneousVelocityGradient.Set(0.0f, 0.0f, 0.0f);
				instantaneousGravityGradient.Set(0.0f, m_gravitySpeed * m_latentPeriod, 0.0f);
			}

			break;

		case ruFUSION_ACTORSTATE_CROUCH_LOOP:
			{
				// Transform input velocity into entity local frame
				CRuVector3 localVelocity;
				m_refFrameBasis.TransformVector(m_inputVelocity, localVelocity);
				forwardQuat.TransformVector(localVelocity);

				// Calculate instantaneous velocity
				speed = localVelocity.Magnitude();

				if(speed > ruEPSILON)
				{
					this->SetBackgroundState(ruFUSION_ACTORSTATE_CROUCH_END);
				}

				speed = 0.0f;
				instantaneousVelocityGradient.Set(0.0f, 0.0f, 0.0f);
				instantaneousGravityGradient.Set(0.0f, m_gravitySpeed * m_latentPeriod, 0.0f);
			}

			break;

		case ruFUSION_ACTORSTATE_SLUMBER_LOOP:
			{
				// Transform input velocity into entity local frame
				CRuVector3 localVelocity;
				m_refFrameBasis.TransformVector(m_inputVelocity, localVelocity);
				forwardQuat.TransformVector(localVelocity);

				// Calculate instantaneous velocity
				speed = localVelocity.Magnitude();

				if(speed > ruEPSILON)
				{
					this->SetBackgroundState(ruFUSION_ACTORSTATE_SLUMBER_END);
				}

				speed = 0.0f;
				instantaneousVelocityGradient.Set(0.0f, 0.0f, 0.0f);
				instantaneousGravityGradient.Set(0.0f, m_gravitySpeed * m_latentPeriod, 0.0f);
			}

			break;

		case ruFUSION_ACTORSTATE_CRAFTING_BEGIN:
		case ruFUSION_ACTORSTATE_CRAFTING_LOOP:
		case ruFUSION_ACTORSTATE_CRAFTING_END:
		case ruFUSION_ACTORSTATE_MINING_BEGIN:
		case ruFUSION_ACTORSTATE_MINING_LOOP:
		case ruFUSION_ACTORSTATE_MINING_END:
		case ruFUSION_ACTORSTATE_MINING_END_HOLD:
		case ruFUSION_ACTORSTATE_ACTIVATE_BEGIN:
		case ruFUSION_ACTORSTATE_ACTIVATE_LOOP:
		case ruFUSION_ACTORSTATE_ACTIVATE_END:
		case ruFUSION_ACTORSTATE_READING_BEGIN:
		case ruFUSION_ACTORSTATE_READING_LOOP:
		case ruFUSION_ACTORSTATE_READING_END:
		case ruFUSION_ACTORSTATE_HOLDING:
			{
				// Transform input velocity into entity local frame
				CRuVector3 localVelocity;
				m_refFrameBasis.TransformVector(m_inputVelocity, localVelocity);
				forwardQuat.TransformVector(localVelocity);

				// Calculate instantaneous velocity
				speed = localVelocity.Magnitude();

				if(speed > ruEPSILON)
				{
					m_actorState = ruFUSION_ACTORSTATE_NORMAL;
				}

				speed = 0.0f;
				instantaneousVelocityGradient.Set(0.0f, 0.0f, 0.0f);
				instantaneousGravityGradient.Set(0.0f, m_gravitySpeed * m_latentPeriod, 0.0f);
			}

			break;

		case ruFUSION_ACTORSTATE_DEAD:
			{
				m_deadTime += elapsedTime;
				if ( m_deadTime > 20.0f )
				{
					m_gravitySpeed = 0.0f;
					m_deadTime = 0.0f;
				}

				speed = 0.0f;
				instantaneousVelocityGradient.Set(0.0f, 0.0f, 0.0f);
				instantaneousGravityGradient.Set(0.0f, 0.0f, 0.0f);
			}

			break;

		case ruFUSION_ACTORSTATE_SIT_BEGIN:
		case ruFUSION_ACTORSTATE_SIT_END:
		default:
			{
				speed = 0.0f;
				instantaneousVelocityGradient.Set(0.0f, 0.0f, 0.0f);
				instantaneousGravityGradient.Set(0.0f, m_gravitySpeed * m_latentPeriod, 0.0f);
			}

			break;
	}

	// Calculate entity tilt quaternion
	if(tiltEnabled && (DotProduct(m_currentForwardVector, effectiveForwardVector) < (1.0f - ruEPSILON)))
	{
		m_tiltQuaternion.RotateIntoVector(effectiveForwardVector, m_currentForwardVector);
	}
	else
	{
		m_tiltQuaternion.Set(0.0f, 0.0f, 0.0f, 1.0f);
	}

	// Orient entity toward current forward vector
	switch(m_orientMode)
	{
		case ruORIENTMODE_UNCONSTRAINED:
			RuEntity_Orient(m_targetEntity, entityReferenceForward, m_currentForwardVector, m_upVector, m_scale);
			break;

		case ruORIENTMODE_CONSTRAINED:
			RuEntity_OrientConstrained(m_targetEntity, entityReferenceForward, m_currentForwardVector, m_upVector, m_scale, m_latentPeriod * 5.0f, m_curUpRotAngle);
			break;
	}

	// Tilt the entity toward the effective forward vector
	m_targetEntity->Rotate(m_tiltQuaternion);

	// Set expected motion scale for the mime
	m_targetMime->SetExpectedMotionScale(speed / m_scale);

	// Calculate expected destination based on the instantaneous velocity
	CRuVector3 expectedDestination = origin + instantaneousVelocityGradient;

	// Transitions
	BOOL lastMoveSuccessful = TRUE;
	BOOL lastIterationCollided = FALSE;
	CRuVector3 lastIterationCollisionNormal;
	CRuVector3 lastIterationCollisionPoint;
	CRuVector3 lastIterationRestingPoint;
	CRuVector3 lastIterationRestingPointNormal;
	BOOL transitionToFalling = FALSE;
	BOOL transitionToStationary = FALSE;

	CRuVector3 stepFactor(0.0f, 10.0f, 0.0f);

	CRuVector3 constantGravitySpeed(0.0f, 0.0f, 0.0f);

	if(m_sojourner)
	{
		// Reset variables
		m_onFirmGround = FALSE;

		// Set friction parameters
		if(m_gravitySpeed < -40.0f)
		{
			m_sojourner->SetFrictionParameters(CRuVector3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
		}
		else
		{
			m_sojourner->SetFrictionParameters(CRuVector3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
		}

		// Apply step factor to help us move up stairs
//		instantaneousVelocityGradient = instantaneousVelocityGradient + instantaneousGravityGradient + constantGravitySpeed * m_latentPeriod;
		instantaneousVelocityGradient = instantaneousVelocityGradient + instantaneousGravityGradient + constantGravitySpeed;

//		m_sojourner->Move(origin + CRuVector3(0.0f, 2.5f, 0.0f), instantaneousVelocityGradient, actorEllipsoid, destination0);
		if(m_enableSojourner)
		{
			// Perform move
			lastMoveSuccessful = m_sojourner->MoveWithClimbAssist(origin, instantaneousVelocityGradient, actorEllipsoid, destination0);

			// Offset movement end points such that the end points are at the feet of the actor
			CRuArrayList<CRuVector3> &movementEndPoints = m_sojourner->GetMovementEndPoints();
			for(INT32 i = 0; i < movementEndPoints.Count(); ++i)
			{
				movementEndPoints[i] = movementEndPoints[i] - actorOffset;
			}

			// Get sojourner's last status
			lastIterationCollided = m_sojourner->LastIterationCollided();
			lastIterationCollisionNormal = m_sojourner->GetLastCollisionNormal();
			lastIterationCollisionPoint = m_sojourner->GetLastCollisionPoint();
			lastIterationRestingPoint = m_sojourner->GetLastRestingPoint();
			lastIterationRestingPointNormal = m_sojourner->GetLastRestingPointNormal();

			// If resting point is about the same as the current actor position, then consider us to have collided with the ground
			if(lastIterationCollided == FALSE)
			{
				REAL restPtHtDiff = fabs(lastIterationRestingPoint.m_y - (destination0.m_y - actorOffset.m_y));
				if(restPtHtDiff < 0.1f)
				{
					lastIterationCollided = TRUE;
					lastIterationCollisionNormal = lastIterationRestingPointNormal;
					lastIterationCollisionPoint = lastIterationRestingPoint;
				}
			}
		}
		else
		{
			destination0 = origin;

			// Get sojourner's last status
			lastIterationCollided = FALSE;
			lastIterationCollisionNormal.Set(0.0f, 1.0f, 0.0f);
			lastIterationCollisionPoint = destination0;
			lastIterationRestingPoint = destination0;
			lastIterationRestingPointNormal.Set(0.0f, 1.0f, 0.0f);
		}

		// Legacy stuff... we no longer use destination1... since we combined gravity case with the lateral movement case
		destination1 = destination0;

		if(lastMoveSuccessful)
		{
			if(lastIterationCollided)
			{
				// Catch-all case to guard against infinite falling
				if(m_actorState == ruFUSION_ACTORSTATE_FALLING)
				{
					if(m_gravitySpeed < ruEPSILON)
					{
						REAL instantaneousTravelDueToGravity = m_gravitySpeed * m_latentPeriod;
						if(instantaneousTravelDueToGravity < -ruEPSILON)
						{
							REAL verticalTravel = destination0.m_y - origin.m_y;
							REAL gravityTravelRatio = verticalTravel / instantaneousTravelDueToGravity;

							if(gravityTravelRatio < 0.10f)
							{
								m_gravitySpeed = 0.0f;
								transitionToStationary = TRUE;
/*
								char moo[200];
								sprintf(moo, "Infinite fall protection triggered at %u.\n", timeGetTime());
								OutputDebugString(moo);
*/
							}
						}
					}
				}

				// Slope prohibition test code
				REAL restingAngle = 0.0f;
				CRuVector3 restingNormal = m_sojourner->GetLastRestingPointNormal();
				CRuVector3 canonicalNormal(0.0f, 1.0f, 0.0f);

				if(fabs(DotProduct(restingNormal, canonicalNormal)) < 0.999f)
				{
					CRuVector3 orthoVec = CrossProduct(restingNormal, canonicalNormal).Normalize();
					CalculateAngleDifferential(canonicalNormal, restingNormal, orthoVec, restingAngle);
					restingAngle = restingAngle * (180.0f / ruPI);
				}

				// Calculate collision height difference
				REAL colDiff = lastIterationCollisionPoint.m_y - destination0.m_y;

				// Calculate collision offset from the top & bottom
				REAL offsetFromBottom = colDiff + actorOffset.m_y;
				REAL offsetFromTop = colDiff - actorOffset.m_y;

				m_stopPoint_OffsetFromBottom = offsetFromBottom;
				m_stopPoint_OffsetFromTop = offsetFromTop;

//				if(offsetFromTop > -14.0f)
				if(offsetFromBottom > 2.50f)
				{
					if(m_gravitySpeed > 0.0f)
					{
						// We're going up and we collided with some top portion of the collision ellipsoid, so stop going up
						m_gravitySpeed = 0.0f;
					}

					m_latentVelocity.Set(0.0f, 0.0f, 0.0f);
				}

				if(m_actorState == ruFUSION_ACTORSTATE_FALLING)
				{
					if(offsetFromBottom > 1.00f)
					{
						// Collision occurs outside of the defined "base plate", so we're falling
						m_gravitySpeed += m_gravityAcceleration * m_latentPeriod;
					}
					else
					{
						if(m_gravitySpeed < -ruEPSILON)
						{
	/*
							if(m_actorState != ruFUSION_ACTORSTATE_NORMAL)
							{
								char moo[200];
								sprintf(moo, "Standard falling to stationary triggered at %u, speed %f, offset %d.\n", timeGetTime(), m_gravitySpeed, offsetFromBottom);
								OutputDebugString(moo);
							}
	*/
							if(m_gravitySpeed < ruEPSILON && offsetFromBottom < 1.00f)
							{
								// Collision occurs inside the defined "base plate", so we are standing on firm ground
								m_gravitySpeed = 0.0f;
							}
							else
							{
								// Continue to accumulate gravity speed
								m_gravitySpeed += m_gravityAcceleration * m_latentPeriod;
							}

							// Confirm that the actor is standing on firm ground
							m_onFirmGround = TRUE;

							// Transition movement to stationary mode
							transitionToStationary = TRUE;
						}
						else
						{
							// Continue to accumulate gravity speed
							m_gravitySpeed += m_gravityAcceleration * m_latentPeriod;
						}
					}
				}
				else
				{
					if(offsetFromBottom > 1.00f || restingAngle > m_slopeSlideThreshold)
					{
						// Continue to accumulate gravity speed
						m_gravitySpeed += m_gravityAcceleration * m_latentPeriod;
					}
					else
					{
						m_gravitySpeed -= m_gravityAcceleration * m_latentPeriod;
						m_gravitySpeed = min(0.0f, m_gravitySpeed);
					}
				}

			}
			else
			{
				// No collision, this means we're falling
				m_gravitySpeed += m_gravityAcceleration * m_latentPeriod;
			}

			if(m_gravitySpeed < m_gravityAcceleration * 1.25f)
			{
				transitionToFalling = TRUE;
			}
		}
		else
		{
			transitionToStationary = TRUE;
			m_gravitySpeed = 0.0f;
		}
	}
/*
	if(RuEntity_FindFirstPaperdoll(m_targetEntity))
	{
		if(fabs(m_gravitySpeed) > ruEPSILON)
		{
			char moo[200];
			sprintf(moo, "Gravity Speed: %f\n", m_gravitySpeed);
			OutputDebugString(moo);
		}
	}
*/
	// Calculate difference between actual destination and expected destination
	REAL destDiff = (expectedDestination - destination0).Magnitude();

	// Restore destinations to entity coordinate frame
	destination0 = destination0 - actorOffset;
	destination1 = destination1 - actorOffset;

	// Move target entity to final destination
	m_targetEntity->Translate(destination1, ruTRANSFORMOP_SET);

	// Process torso and body headings only if this actor is not linked to another actor
	if(m_linkParent)
	{
		m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, m_defaultIdleAction, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
	}
	else
	{
		if(m_inputVelocity.m_x > ruEPSILON)
		{
			if(m_inputVelocity.m_z > ruEPSILON)
			{
				m_targetBodyHeadingOffset = ruPI / 4.0f;
			}
			else if (m_inputVelocity.m_z < -ruEPSILON)
			{
				m_targetBodyHeadingOffset = -ruPI / 4.0f;
			}
			else
			{
				m_targetBodyHeadingOffset = ruPI / 2.0f;
			}

			m_targetTorsoHeadingOffset = -m_targetBodyHeadingOffset;
			m_targetYawOffset = 0.0f;
		}
		else if(m_inputVelocity.m_x < -ruEPSILON)
		{
			if(m_inputVelocity.m_z > ruEPSILON)
			{
				m_targetBodyHeadingOffset = -ruPI / 4.0f;
			}
			else if (m_inputVelocity.m_z < -ruEPSILON)
			{
				m_targetBodyHeadingOffset = ruPI / 4.0f;
			}
			else
			{
				m_targetBodyHeadingOffset = -ruPI / 2.0f;
			}

			m_targetTorsoHeadingOffset = -m_targetBodyHeadingOffset;
			m_targetYawOffset = 0.0f;
		}
		else
		{
			m_targetBodyHeadingOffset = 0.0f;
			m_targetTorsoHeadingOffset = -staticTurnAngle;
			m_targetYawOffset = 0.0f;
		}

		// To be safe, set in-water and submerged to false here
		if(m_actorState != ruFUSION_ACTORSTATE_DYING && m_actorState != ruFUSION_ACTORSTATE_DEAD)
		{
			m_isInWater = FALSE;
			m_isSubmerged = FALSE;
		}

		// Handle transition to and from swimming/diving
		switch(m_actorState)
		{
			case ruFUSION_ACTORSTATE_NORMAL:
			case ruFUSION_ACTORSTATE_JUMP:
			case ruFUSION_ACTORSTATE_FALLING:
				{
					if(m_sojourner->IsUnderWaterSurface(m_swim_Surface_EntrySegment.Origin() + m_targetEntity->GetTranslation(ruFRAMETYPE_WORLD)))
					{
						m_actorState = ruFUSION_ACTORSTATE_SWIM_SURFACE;
						m_isInWater = TRUE;
						m_isSubmerged = FALSE;
					}
				}

				break;

			case ruFUSION_ACTORSTATE_SWIM_SURFACE:
				{
					if(m_sojourner->IsUnderWaterSurface(m_swim_Surface_ExitSegment.Origin() + m_targetEntity->GetTranslation(ruFRAMETYPE_WORLD)) == FALSE)
					{
						m_actorState = ruFUSION_ACTORSTATE_NORMAL;
						m_isInWater = FALSE;
						m_isSubmerged = FALSE;
					}
					else if(m_sojourner->IsUnderWaterSurface(m_swim_Dive_EntrySegment.Origin() + m_targetEntity->GetTranslation(ruFRAMETYPE_WORLD)))
					{
						m_actorState = ruFUSION_ACTORSTATE_SWIM_DIVE;
						m_isInWater = TRUE;
						m_isSubmerged = TRUE;
					}
				}

				break;

			case ruFUSION_ACTORSTATE_SWIM_DIVE:
				{
					if(m_sojourner->IsUnderWaterSurface(m_swim_Dive_ExitSegment.Origin() + m_targetEntity->GetTranslation(ruFRAMETYPE_WORLD)) == FALSE)
					{
						m_actorState = ruFUSION_ACTORSTATE_SWIM_SURFACE;
						m_isInWater = TRUE;
						m_isSubmerged = FALSE;
					}
					else
					{
						m_isInWater = TRUE;
						m_isSubmerged = TRUE;
					}
				}

				break;

			default:
				break;
		}

		if(m_sojourner->IsAboveWaterSurface(m_targetEntity->GetTranslation(ruFRAMETYPE_WORLD) + CRuVector3(0.0f, 5.0f, 0.0f)))
		{
			m_isAboveWater = TRUE;
		}
		else
		{
			m_isAboveWater = FALSE;
		}

		// State transitions and motion
		switch(m_actorState)
		{
			case ruFUSION_ACTORSTATE_NORMAL:
				{
					BOOL resetForegroundAction = TRUE;
					REAL inputSpeed = m_inputVelocity.Magnitude();
					CRuFusion_Mime_Action curMimeAction = m_targetMime->GetAction(ruFUSION_MIME_CHANNEL_BACKGROUND);
					CRuFusion_Mime_Action curMimeForegroundAction = m_targetMime->GetAction(ruFUSION_MIME_CHANNEL_FOREGROUND);

					if(m_inputVelocity.m_z < -ruEPSILON)
					{
						m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, m_defaultBackAction, (curMimeAction == ruFUSION_MIME_JUMP_DOWN) ? ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE : ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);

						// If current foreground action is the idle emote, force a dequeue from the action queue to terminate it
						if(curMimeForegroundAction == ruFUSION_MIME_EMOTE_IDLE)
						{
							m_targetMime->ForceActionDequeue(ruFUSION_MIME_CHANNEL_FOREGROUND);
						}

						m_lastUpdateIsIdle = FALSE;
					}
					else if(m_inputVelocity.m_z > ruEPSILON || fabs(m_inputVelocity.m_x) > ruEPSILON)
					{
						m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, inputSpeed > m_runThreshold ? m_defaultRunningAction : m_defaultWalkingAction, (curMimeAction == ruFUSION_MIME_JUMP_DOWN) ? ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE : ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);

						// If current foreground action is the idle emote, force a dequeue from the action queue to terminate it
						if(curMimeForegroundAction == ruFUSION_MIME_EMOTE_IDLE)
						{
							m_targetMime->ForceActionDequeue(ruFUSION_MIME_CHANNEL_FOREGROUND);
						}

						m_lastUpdateIsIdle = FALSE;
					}
					else
					{
						if(shufflingDirection == -1)
						{
							m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SHUFFLE_LEFT, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);

							m_shufflingTime += elapsedTime;
						}
						else if(shufflingDirection == 1)
						{
							m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SHUFFLE_RIGHT, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);

							m_shufflingTime += elapsedTime;
						}
						else
						{
							if(m_lastUpdateIsIdle)
							{
								if(curMimeAction == ruFUSION_MIME_SHUFFLE_LEFT || curMimeAction == ruFUSION_MIME_SHUFFLE_LEFT)
								{
									if(m_shufflingTime > 0.20f)
									{
										m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, m_defaultIdleAction, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
									}
									else
									{
										m_shufflingTime += elapsedTime;
									}
								}
								else
								{
									resetForegroundAction = FALSE;

									// Doing nothing, check last idle time
									if(m_idlingTime > m_nextIdlingPeriod)
									{
										m_nextIdlingPeriod = m_randGen.GenerateFloat() * 10.0f + 10.0f;
										m_idlingTime = 0.0f;

										m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, m_defaultIdleAction, (curMimeAction == ruFUSION_MIME_JUMP_DOWNANDRUN || curMimeAction == ruFUSION_MIME_JUMP_DOWNANDBACK) ? ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE : ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);

										// Only do idle action if we are in a normal standing state
										if(m_actorState == ruFUSION_ACTORSTATE_NORMAL && m_foregroundState == ruFUSION_ACTORSTATE_NORMAL && m_transientState == ruFUSION_ACTORSTATE_NORMAL && m_defaultIdleAction == ruFUSION_MIME_IDLE_STAND)
										{
											m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_FOREGROUND, ruFUSION_MIME_EMOTE_IDLE, (curMimeAction == ruFUSION_MIME_JUMP_DOWNANDRUN || curMimeAction == ruFUSION_MIME_JUMP_DOWNANDBACK) ? ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE : ruFUSION_MIME_SETACTIONMETHOD_NORMAL, static_cast<PTRVALUE>(ruFUSION_ACTORSTATE_EMOTE_IDLE), NULL, FALSE);

											// Restore original action and motion
											m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
										}
									}
									else
									{
										m_idlingTime += m_latentPeriod;
										m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, m_defaultIdleAction, (curMimeAction == ruFUSION_MIME_JUMP_DOWNANDRUN || curMimeAction == ruFUSION_MIME_JUMP_DOWNANDBACK) ? ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE : ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
									}
								}
							}
						}

						m_lastUpdateIsIdle = TRUE;
					}

					if(transitionToFalling && CanTransitionToFalling())
					{
						m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_JUMP_LOOP, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_FALLING, NULL, FALSE);
					}

					if(resetForegroundAction)
					{
						if(m_targetMime->GetAction(ruFUSION_MIME_CHANNEL_FOREGROUND) == ruFUSION_MIME_EMOTE_IDLE)
						{
							m_targetMime->RestoreDefaultAction(ruFUSION_MIME_CHANNEL_FOREGROUND);
						}
					}
				}

				break;

			case ruFUSION_ACTORSTATE_FALLING:
				{
					if(transitionToStationary)
					{
						if(m_latentVelocity.Magnitude() > ruEPSILON)
						{
							if(fabs(m_inputVelocity.m_x) > ruEPSILON || m_inputVelocity.m_z > ruEPSILON)
							{
								m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_JUMP_DOWNANDRUN, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
							}
							else if(m_inputVelocity.m_z < -ruEPSILON)
							{
								m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_JUMP_DOWNANDBACK, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
							}
							else
							{
								m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_JUMP_DOWN, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
							}
						}
						else
						{
							if(fabs(m_inputVelocity.m_x) > ruEPSILON || m_inputVelocity.m_z > ruEPSILON)
							{
								m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_JUMP_DOWNANDRUN_2, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
							}
							else if(m_inputVelocity.m_z < -ruEPSILON)
							{
								m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_JUMP_DOWNANDBACK_2, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
							}
							else
							{
								m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_JUMP_DOWN, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_NORMAL, NULL, FALSE);
							}
						}


						m_latentVelocity.Set(0.0f, 0.0f, 0.0f);
					}
				}

				break;
	/*
			case ruFUSION_ACTORSTATE_LANDING:
				{
				}

				break;
	*/
			case ruFUSION_ACTORSTATE_SWIM_SURFACE:
				{
					REAL inputSpeed = m_inputVelocity.Magnitude();

					if(m_inputVelocity.m_z > ruEPSILON)
					{
						m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SWIM_FORWARD, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_SWIM_SURFACE, NULL, FALSE);

						m_targetTorsoHeadingOffset = 0.0f;
						m_targetYawOffset = 0.0f;
					}
					else if(m_inputVelocity.m_z < -ruEPSILON)
					{
						m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SWIM_BACKWARD, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_SWIM_SURFACE, NULL, FALSE);

						m_targetTorsoHeadingOffset = 0.0f;
						m_targetYawOffset = 0.0f;
					}
					else if(m_inputVelocity.m_x > ruEPSILON)
					{
						m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SWIM_RIGHT, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_SWIM_SURFACE, NULL, FALSE);

						m_targetBodyHeadingOffset = 0.0f;
						m_targetTorsoHeadingOffset = 0.0f;
						m_targetYawOffset = 0.0f;
					}
					else if(m_inputVelocity.m_x < -ruEPSILON)
					{
						m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SWIM_LEFT, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_SWIM_SURFACE, NULL, FALSE);

						m_targetBodyHeadingOffset = 0.0f;
						m_targetTorsoHeadingOffset = 0.0f;
						m_targetYawOffset = 0.0f;
					}
					else
					{
						m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SWIM_IDLE, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_SWIM_SURFACE, NULL, FALSE);

						m_targetBodyHeadingOffset = 0.0f;
						m_targetTorsoHeadingOffset = 0.0f;
						m_targetYawOffset = 0.0f;
					}
				}

				break;

			case ruFUSION_ACTORSTATE_SWIM_DIVE:
				{
					REAL inputSpeed = m_inputVelocity.Magnitude();

					REAL yawOffset = 0.0f;
					CRuVector3 orthoVec(m_targetForwardVector.m_x, 0.0f, m_targetForwardVector.m_z);
					if(orthoVec.SquaredMagnitude() > ruEPSILON)
					{
						orthoVec.Normalize();

						if(m_targetForwardVector.m_y >= 0.0f)
						{
							yawOffset = (REAL)acos(DotProduct(m_targetForwardVector, orthoVec));
						}
						else
						{
							yawOffset = (REAL)-acos(DotProduct(m_targetForwardVector, orthoVec));
						}
					}

					if(m_inputVelocity.m_z > ruEPSILON)
					{
						m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SWIM_FORWARD, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_SWIM_DIVE, NULL, FALSE);

						m_targetTorsoHeadingOffset = 0.0f;
						m_targetYawOffset = yawOffset;
					}
					else if(m_inputVelocity.m_z < -ruEPSILON)
					{
						m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SWIM_BACKWARD, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_SWIM_DIVE, NULL, FALSE);

						m_targetTorsoHeadingOffset = 0.0f;
						m_targetYawOffset = 0.0f;
					}
					else if(m_inputVelocity.m_x > ruEPSILON)
					{
						m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SWIM_RIGHT, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_SWIM_DIVE, NULL, FALSE);

						m_targetBodyHeadingOffset = 0.0f;
						m_targetTorsoHeadingOffset = 0.0f;
						m_targetYawOffset = 0.0f;
					}
					else if(m_inputVelocity.m_x < -ruEPSILON)
					{
						m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SWIM_LEFT, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_SWIM_DIVE, NULL, FALSE);

						m_targetBodyHeadingOffset = 0.0f;
						m_targetTorsoHeadingOffset = 0.0f;
						m_targetYawOffset = 0.0f;
					}
					else
					{
						m_targetMime->SetAction(ruFUSION_MIME_CHANNEL_BACKGROUND, ruFUSION_MIME_SWIM_IDLE, ruFUSION_MIME_SETACTIONMETHOD_NORMAL, ruFUSION_ACTORSTATE_SWIM_DIVE, NULL, FALSE);

						m_targetBodyHeadingOffset = 0.0f;
						m_targetTorsoHeadingOffset = 0.0f;
						m_targetYawOffset = 0.0f;
					}
				}

				break;
		}

		if(doNotInterpolateHeadingOffsets)
		{
			m_currentBodyHeadingOffset = -m_targetBodyHeadingOffset;
			m_currentTorsoHeadingOffset = -m_targetTorsoHeadingOffset;
			m_currentYawOffset = -m_targetYawOffset;
		}
		else
		{
			// Interpolate avatar heading and rotation
			m_currentBodyHeadingOffset = RuMathHelper_StepInterpolate(m_currentBodyHeadingOffset, -m_targetBodyHeadingOffset, 20.00f, 10.00f, elapsedTime);
			m_currentTorsoHeadingOffset = RuMathHelper_StepInterpolate(m_currentTorsoHeadingOffset, -m_targetTorsoHeadingOffset, 20.00f, 10.00f, elapsedTime);
			m_currentYawOffset = RuMathHelper_StepInterpolate(m_currentYawOffset, -m_targetYawOffset, 20.00f, 10.00f, elapsedTime);
		}

		// Check linked actors and forcibly change their heading offsets
		for(INT32 i = 0; i < m_linkedActors.Count(); ++i)
		{
			m_linkedActors[i]->m_targetBodyHeadingOffset = m_targetBodyHeadingOffset;
			m_linkedActors[i]->m_targetTorsoHeadingOffset = m_targetTorsoHeadingOffset;
			m_linkedActors[i]->m_targetYawOffset = m_targetYawOffset;

			m_linkedActors[i]->m_currentBodyHeadingOffset = m_currentBodyHeadingOffset;
			m_linkedActors[i]->m_currentTorsoHeadingOffset = m_currentTorsoHeadingOffset;
			m_linkedActors[i]->m_currentYawOffset = m_currentYawOffset;
		}
	}

//	g_pDebugMessage->DebugOutput(0, "[ACTOR] %f %f %f", m_currentBodyHeadingOffset , m_currentTorsoHeadingOffset , m_currentYawOffset);

	// Set offsets
	m_targetMime->SetBodyHeadingOffset(m_currentBodyHeadingOffset);
	m_targetMime->SetTorsoHeadingOffset(m_currentTorsoHeadingOffset);
	m_targetMime->SetBodyYawOffset(m_currentYawOffset);

	// Reset latent period
	m_latentPeriod = 0.0f;

	RUPROFILE_SAMPLE_END(m_probe_ActorUpdate, 3)
}

BOOL CRuFusion_Actor::CanTransitionToFalling()
{
	switch(m_actorState)
	{
		default:
			return TRUE;

		case ruFUSION_ACTORSTATE_DYING:
		case ruFUSION_ACTORSTATE_DEAD:
		case ruFUSION_ACTORSTATE_REVIVING:
			return FALSE;
	}
	return TRUE;
}

CRuSegment3 CRuFusion_Actor::BuildMaximalTestSegment(const CRuSegment3 &refSegment, const CRuVector3 &pt0, const CRuVector3 &pt1)
{
	CRuVector3 entryPoint0_Origin = refSegment.Origin() + pt0;
	CRuVector3 entryPoint1_Origin = entryPoint0_Origin + refSegment.Direction();

	CRuVector3 entryPoint0_Dest = refSegment.Origin() + pt1;
	CRuVector3 entryPoint1_Dest = entryPoint0_Dest + refSegment.Direction();

	CRuVector3 p0 = entryPoint0_Dest;
	CRuVector3 p1 = entryPoint1_Dest;

	p0.m_y = max(entryPoint0_Origin.m_y, entryPoint0_Dest.m_y);
	p1.m_y = min(entryPoint1_Origin.m_y, entryPoint1_Dest.m_y);

	CRuSegment3 testSegment(p0, p1);

	return testSegment;
}

void CRuFusion_Actor::MimeActionChangedHandler(CRuFusion_Mime_Channel channel, CRuFusion_Mime_Action action, PTRVALUE eventData)
{
	switch(channel)
	{
		case ruFUSION_MIME_CHANNEL_BACKGROUND:
			{
				CRuFusion_ActorState newActorState = static_cast<CRuFusion_ActorState>(eventData & 0x0000FFFF);

				m_actorState = newActorState;
			}

			break;

		case ruFUSION_MIME_CHANNEL_FOREGROUND:
			{
				CRuFusion_ActorState newActorState = static_cast<CRuFusion_ActorState>(eventData & 0x0000FFFF);

				// Determine what to do depending on actor state
				switch(newActorState)
				{
					// FOREGROUND states
					case ruFUSION_ACTORSTATE_NORMAL:
					case ruFUSION_ACTORSTATE_COMBAT_1H:
					case ruFUSION_ACTORSTATE_COMBAT_2H:
					case ruFUSION_ACTORSTATE_COMBAT_BOW:
					case ruFUSION_ACTORSTATE_COMBAT_GUN:
					case ruFUSION_ACTORSTATE_COMBAT_POLEARM:
					case ruFUSION_ACTORSTATE_COMBAT_UNARMED:
						{
							m_foregroundState = newActorState;

							m_foregroundMimeAction = action;
							m_transientState = ruFUSION_ACTORSTATE_NORMAL;

							m_disableMovement = FALSE;
						}

						break;

					// AUXILIARY states
					case ruFUSION_ACTORSTATE_HURT_NORMAL:
					case ruFUSION_ACTORSTATE_HURT_CRITICAL:
					case ruFUSION_ACTORSTATE_DODGE:
						{
							m_transientState = newActorState;
							m_transientMimeAction = action;

							if(m_disableMovementOnAction != ruFUSION_ACTORSTATE_NORMAL && m_transientState == m_disableMovementOnAction)
							{
								m_disableMovementOnAction = ruFUSION_ACTORSTATE_NORMAL;
								m_disableMovement = TRUE;
							}
							else
							{
								m_disableMovement = FALSE;
							}
						}

						break;

					// TRANSIENT states
					default:
						{
							m_transientState = newActorState;
							m_transientMimeAction = action;

							if(m_disableMovementOnAction != ruFUSION_ACTORSTATE_NORMAL && m_transientState == m_disableMovementOnAction)
							{
								m_disableMovementOnAction = ruFUSION_ACTORSTATE_NORMAL;
								m_disableMovement = TRUE;
							}
							else
							{
								m_disableMovement = FALSE;
							}
						}

						break;
				}
			}

			break;
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
