/*!
	@project Rune
	@file RuFusion_Actor.h

	Copyright (c) 2004-2006 All rights reserved

	@author John Tang
	@date 2006/07/28
*/

#ifndef _RUFUSION_ACTOR_H_
#define _RUFUSION_ACTOR_H_

#include "../Fusion/RuFusion_Mime.h"
#include "../Fusion/RuFusion_Sojourner.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

enum CRuFusion_ActorState
{
	// "Background" States
	ruFUSION_ACTORSTATE_NORMAL					= 0,
	ruFUSION_ACTORSTATE_JUMP					= 1,
	ruFUSION_ACTORSTATE_FALLING					= 2,
	ruFUSION_ACTORSTATE_SWIM_SURFACE			= 3,
	ruFUSION_ACTORSTATE_SWIM_DIVE				= 4,

	ruFUSION_ACTORSTATE_SIT_BEGIN				= 6,
	ruFUSION_ACTORSTATE_SIT_LOOP				= 7,
	ruFUSION_ACTORSTATE_SIT_END					= 8,

	ruFUSION_ACTORSTATE_SLEEP_BEGIN				= 9,
	ruFUSION_ACTORSTATE_SLEEP_LOOP				= 10,
	ruFUSION_ACTORSTATE_SLEEP_END				= 11,

	ruFUSION_ACTORSTATE_DYING					= 12,
	ruFUSION_ACTORSTATE_DEAD					= 13,
	ruFUSION_ACTORSTATE_REVIVING				= 14,

	ruFUSION_ACTORSTATE_SPAWN_BEGIN				= 15,
	ruFUSION_ACTORSTATE_SPAWN_END				= 16,

	ruFUSION_ACTORSTATE_DEAD_WATER				= 17,

	ruFUSION_ACTORSTATE_CROUCH_BEGIN			= 148,
	ruFUSION_ACTORSTATE_CROUCH_LOOP				= 149,
	ruFUSION_ACTORSTATE_CROUCH_END				= 150,

	ruFUSION_ACTORSTATE_CRAFTING_BEGIN			= 131,
	ruFUSION_ACTORSTATE_CRAFTING_LOOP			= 132,
	ruFUSION_ACTORSTATE_CRAFTING_END			= 133,

	ruFUSION_ACTORSTATE_MINING_BEGIN			= 134,
	ruFUSION_ACTORSTATE_MINING_LOOP				= 135,
	ruFUSION_ACTORSTATE_MINING_END				= 136,
	ruFUSION_ACTORSTATE_MINING_END_HOLD			= 137,

	ruFUSION_ACTORSTATE_LUMBERJACK_BEGIN		= 201,
	ruFUSION_ACTORSTATE_LUMBERJACK_LOOP			= 202,
	ruFUSION_ACTORSTATE_LUMBERJACK_END			= 203,

	ruFUSION_ACTORSTATE_GATHER_BEGIN			= 204,
	ruFUSION_ACTORSTATE_GATHER_LOOP				= 205,
	ruFUSION_ACTORSTATE_GATHER_END				= 206,

	ruFUSION_ACTORSTATE_LOOT_BEGIN				= 207,
	ruFUSION_ACTORSTATE_LOOT_LOOP				= 208,
	ruFUSION_ACTORSTATE_LOOT_END				= 209,

	ruFUSION_ACTORSTATE_CARRY_BEGIN				= 216,
	ruFUSION_ACTORSTATE_CARRY_LOOP				= 217,
	ruFUSION_ACTORSTATE_CARRY_END				= 218,	

	ruFUSION_ACTORSTATE_DAZED_BEGIN				= 138,
	ruFUSION_ACTORSTATE_DAZED_LOOP				= 139,
	ruFUSION_ACTORSTATE_DAZED_END				= 140,

	ruFUSION_ACTORSTATE_SLUMBER_BEGIN			= 141,
	ruFUSION_ACTORSTATE_SLUMBER_LOOP			= 142,
	ruFUSION_ACTORSTATE_SLUMBER_END				= 143,

	ruFUSION_ACTORSTATE_ACTIVATE_BEGIN			= 145,
	ruFUSION_ACTORSTATE_ACTIVATE_LOOP			= 146,
	ruFUSION_ACTORSTATE_ACTIVATE_END			= 147,

	ruFUSION_ACTORSTATE_READING_BEGIN			= 155,
	ruFUSION_ACTORSTATE_READING_LOOP			= 156,
	ruFUSION_ACTORSTATE_READING_END				= 157,

	ruFUSION_ACTORSTATE_SIT_CHAIR_BEGIN			= 158,
	ruFUSION_ACTORSTATE_SIT_CHAIR_LOOP			= 159,
	ruFUSION_ACTORSTATE_SIT_CHAIR_END			= 160,

	ruFUSION_ACTORSTATE_KNOCKDOWN_BEGIN			= 181,
	ruFUSION_ACTORSTATE_KNOCKDOWN_LOOP			= 182,
	ruFUSION_ACTORSTATE_KNOCKDOWN_END			= 183,
	ruFUSION_ACTORSTATE_KNOCKDOWN_RECOVER		= 184,

	ruFUSION_ACTORSTATE_EMOTE_HANDSTAND			= 185,
	ruFUSION_ACTORSTATE_EMOTE_ROFL				= 186,
	ruFUSION_ACTORSTATE_EMOTE_SWEEP1			= 187,
	ruFUSION_ACTORSTATE_EMOTE_SWEEP2			= 188,
	ruFUSION_ACTORSTATE_EMOTE_SWEEP3			= 189,

	ruFUSION_ACTORSTATE_HOLDING					= 144,

	ruFUSION_ACTORSTATE_SPIRAL_RIGHT			= 210,
	ruFUSION_ACTORSTATE_SPIRAL_LEFT				= 211,
	ruFUSION_ACTORSTATE_SPIRAL_LOOP				= 212,
	ruFUSION_ACTORSTATE_FLY_BEGIN				= 213,
	ruFUSION_ACTORSTATE_FLY_LOOP				= 214,
	ruFUSION_ACTORSTATE_FLY_END					= 215,

	// "Auxiliary" States
	ruFUSION_ACTORSTATE_HURT_NORMAL				= 18,
	ruFUSION_ACTORSTATE_HURT_CRITICAL			= 19,
	ruFUSION_ACTORSTATE_DODGE					= 20,

	// "Foreground" States
	ruFUSION_ACTORSTATE_COMBAT_1H				= 21,							//!< Idle 1H combat state
	ruFUSION_ACTORSTATE_COMBAT_2H				= 22,							//!< Idle 2H combat state
	ruFUSION_ACTORSTATE_COMBAT_POLEARM			= 23,							//!< Idle polearm combat state
	ruFUSION_ACTORSTATE_COMBAT_UNARMED			= 24,							//!< Idle unarmed combat state
	ruFUSION_ACTORSTATE_COMBAT_BOW				= 25,							//!< Idle bow combat state
	ruFUSION_ACTORSTATE_COMBAT_GUN				= 26,							//!< Idle gun combat state

	// "Transient" States
	ruFUSION_ACTORSTATE_ATTACK_1H				= 31,							//!< 1-handed strike
	ruFUSION_ACTORSTATE_ATTACK_1H_SP			= 32,							//!< 1-handed strike (SP)
	ruFUSION_ACTORSTATE_ATTACK_1H_SP02			= 57,							//!< 1-handed strike (SP)
	ruFUSION_ACTORSTATE_ATTACK_1H_SLOW01		= 58,							//!< 1-handed strike (SP)
	ruFUSION_ACTORSTATE_ATTACK_1H_PIERCE		= 47,							//!< 1-handed strike (pierce)
	ruFUSION_ACTORSTATE_ATTACK_2H				= 33,							//!< 2-handed strike
	ruFUSION_ACTORSTATE_ATTACK_2H_SP			= 34,							//!< 2-handed strike (SP)
	ruFUSION_ACTORSTATE_ATTACK_POLEARM			= 35,							//!< Polearm strike
	ruFUSION_ACTORSTATE_ATTACK_POLEARM_SP		= 36,							//!< Polearm strike (SP)
	ruFUSION_ACTORSTATE_ATTACK_UNARMED			= 37,							//!< Unarmed strike
	ruFUSION_ACTORSTATE_ATTACK_UNARMED_SP		= 38,							//!< Unarmed strike (SP)
	ruFUSION_ACTORSTATE_ATTACK_BOW_BEGIN		= 39,
	ruFUSION_ACTORSTATE_ATTACK_BOW_LOOP			= 40,
	ruFUSION_ACTORSTATE_ATTACK_BOW_END			= 41,
	ruFUSION_ACTORSTATE_ATTACK_BOW_SP_BEGIN		= 48,
	ruFUSION_ACTORSTATE_ATTACK_BOW_SP_LOOP		= 49,
	ruFUSION_ACTORSTATE_ATTACK_BOW_SP_END		= 50,
	ruFUSION_ACTORSTATE_ATTACK_GUN_BEGIN		= 42,
	ruFUSION_ACTORSTATE_ATTACK_GUN_LOOP			= 43,
	ruFUSION_ACTORSTATE_ATTACK_GUN_END			= 44,
	ruFUSION_ACTORSTATE_ATTACK_OH				= 45,							//!< Offhand strike
	ruFUSION_ACTORSTATE_ATTACK_OH_PIERCE		= 46,							//!< Offhand piercing strike

	ruFUSION_ACTORSTATE_PARRY_1H				= 51,							//!< Standard 1H parry
	ruFUSION_ACTORSTATE_PARRY_2H				= 52,							//!< Standard 2H parry
	ruFUSION_ACTORSTATE_PARRY_BOW				= 53,
	ruFUSION_ACTORSTATE_PARRY_GUN				= 54,
	ruFUSION_ACTORSTATE_PARRY_POLEARM			= 55,
	ruFUSION_ACTORSTATE_PARRY_UNARMED			= 56,

	ruFUSION_ACTORSTATE_SHIELD_BASH				= 71,
	ruFUSION_ACTORSTATE_SHIELD_BLOCK			= 72,

	ruFUSION_ACTORSTATE_USE						= 75,
	ruFUSION_ACTORSTATE_THROW					= 76,

	ruFUSION_ACTORSTATE_BUFF_BEGIN				= 81,
	ruFUSION_ACTORSTATE_BUFF_LOOP				= 82,
	ruFUSION_ACTORSTATE_BUFF_END				= 83,
	ruFUSION_ACTORSTATE_BUFF_INSTANT			= 84,

	ruFUSION_ACTORSTATE_BUFF_SP01				= 161,
	ruFUSION_ACTORSTATE_BUFF_SP02				= 162,
	ruFUSION_ACTORSTATE_BUFF_SP03				= 163,

	ruFUSION_ACTORSTATE_CAST_BEGIN				= 85,
	ruFUSION_ACTORSTATE_CAST_LOOP				= 86,
	ruFUSION_ACTORSTATE_CAST_END				= 87,
	ruFUSION_ACTORSTATE_CAST_INSTANT			= 88,

	ruFUSION_ACTORSTATE_CAST_SP01				= 164,
	ruFUSION_ACTORSTATE_CAST_SP02				= 165,
	ruFUSION_ACTORSTATE_CAST_SP03				= 166,
	ruFUSION_ACTORSTATE_CAST_SP04				= 171,

	ruFUSION_ACTORSTATE_DEBUFF_INSTANT			= 167,

	ruFUSION_ACTORSTATE_FISHING_CAST			= 168,
	ruFUSION_ACTORSTATE_FISHING_IDLE			= 169,
	ruFUSION_ACTORSTATE_FISHING_REEL			= 170,

	ruFUSION_ACTORSTATE_CHANNEL_BEGIN			= 89,
	ruFUSION_ACTORSTATE_CHANNEL_LOOP			= 90,
	ruFUSION_ACTORSTATE_CHANNEL_END				= 91,
	ruFUSION_ACTORSTATE_CHANNEL_INSTANT			= 92,

	ruFUSION_ACTORSTATE_SP_SPINKICK				= 95,
	ruFUSION_ACTORSTATE_SP_SPINSLASH			= 96,

	ruFUSION_ACTORSTATE_EMOTE_ANGRY				= 101,
	ruFUSION_ACTORSTATE_EMOTE_APPLAUSE			= 102,
	ruFUSION_ACTORSTATE_EMOTE_APPROVAL			= 103,
	ruFUSION_ACTORSTATE_EMOTE_BEG				= 104,
	ruFUSION_ACTORSTATE_EMOTE_CRY				= 105,
	ruFUSION_ACTORSTATE_EMOTE_HEAD_NOD			= 106,
	ruFUSION_ACTORSTATE_EMOTE_HEAD_SHAKE		= 107,
	ruFUSION_ACTORSTATE_EMOTE_LAUGH				= 108,
	ruFUSION_ACTORSTATE_EMOTE_POINT				= 109,
	ruFUSION_ACTORSTATE_EMOTE_PROVOKE			= 110,
	ruFUSION_ACTORSTATE_EMOTE_SALUTE			= 111,
	ruFUSION_ACTORSTATE_EMOTE_SPEAK				= 112,
	ruFUSION_ACTORSTATE_EMOTE_SURRENDER			= 113,
	ruFUSION_ACTORSTATE_EMOTE_THREATEN			= 114,
	ruFUSION_ACTORSTATE_EMOTE_VICTORY			= 115,
	ruFUSION_ACTORSTATE_EMOTE_WAVE				= 116,
	ruFUSION_ACTORSTATE_EMOTE_EAT				= 117,
	ruFUSION_ACTORSTATE_EMOTE_DRINK				= 118,
	ruFUSION_ACTORSTATE_EMOTE_IDLE				= 119,
	ruFUSION_ACTORSTATE_EMOTE_IDLE0				= 120,
	ruFUSION_ACTORSTATE_EMOTE_IDLE1				= 121,
	ruFUSION_ACTORSTATE_EMOTE_IDLE2				= 122,
	ruFUSION_ACTORSTATE_EMOTE_DANCE				= 123,
	ruFUSION_ACTORSTATE_EMOTE_PLAY_SAXOPHONE	= 124,
	ruFUSION_ACTORSTATE_EMOTE_PLAY_TAMBOURINE	= 125,
	ruFUSION_ACTORSTATE_EMOTE_PLAY_GUITAR		= 126,
	ruFUSION_ACTORSTATE_EMOTE_JUMP_INPLACE		= 127,
	ruFUSION_ACTORSTATE_EMOTE_BELLY				= 128,
	ruFUSION_ACTORSTATE_EMOTE_EMBRAASSED		= 129,
	ruFUSION_ACTORSTATE_EMOTE_EMBRACE			= 130,
	ruFUSION_ACTORSTATE_EMOTE_KISS				= 250,
	ruFUSION_ACTORSTATE_EMOTE_SALUTE2			= 251,
	ruFUSION_ACTORSTATE_EMOTE_CUTE1				= 252,
	ruFUSION_ACTORSTATE_EMOTE_CUTE2				= 253,
	ruFUSION_ACTORSTATE_EMOTE_STRETCH			= 254,
	ruFUSION_ACTORSTATE_EMOTE_ROCK				= 255,
	ruFUSION_ACTORSTATE_EMOTE_KAMEHAMEHA		= 256,
	ruFUSION_ACTORSTATE_EMOTE_KICK				= 257,	
	ruFUSION_ACTORSTATE_EMOTE_BREAKDANCE		= 260,
	ruFUSION_ACTORSTATE_EMOTE_TAPDANCE			= 261,
	ruFUSION_ACTORSTATE_EMOTE_TIRED				= 262,
	ruFUSION_ACTORSTATE_EMOTE_FEAR				= 263,
	ruFUSION_ACTORSTATE_EMOTE_AMAZED			= 264,
	ruFUSION_ACTORSTATE_EMOTE_THINK				= 265,
	ruFUSION_ACTORSTATE_EMOTE_CUTE3				= 266,	
	ruFUSION_ACTORSTATE_EMOTE_HANGOVER			= 267,

	ruFUSION_ACTORSTATE_HOLSTER					= 151,
	ruFUSION_ACTORSTATE_UNHOLSTER				= 152,
	ruFUSION_ACTORSTATE_HOLSTER_2H				= 153,
	ruFUSION_ACTORSTATE_UNHOLSTER_2H			= 154,

	ruFUSION_ACTORSTATE_FORCE_DWORD				= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

class CRuFusion_Actor : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	enum OrientMode
	{
		ruORIENTMODE_UNCONSTRAINED,
		ruORIENTMODE_CONSTRAINED
	};

protected:
	REAL								m_latentPeriod;

	CRuEntity*							m_targetEntity;					//!< Target entity
	CRuFusion_Mime*						m_targetMime;					//!< Target mime object

	BOOL								m_enableSojourner;
	CRuFusion_Sojourner*				m_sojourner;

	CRuMatrix4x4						m_refFrameBasis;

	// Current actor state
	CRuFusion_ActorState				m_actorState;					//!< Current state of the actor
	BOOL								m_isAboveWater;
	BOOL								m_isInWater;
	BOOL								m_isSubmerged;					//!< Actor is submerged?

	CRuFusion_ActorState				m_markedForegroundState;
	CRuFusion_ActorState				m_foregroundState;
	CRuFusion_Mime_Action				m_foregroundMimeAction;

	CRuFusion_ActorState				m_transientState;
	CRuFusion_Mime_Action				m_transientMimeAction;

	BOOL								m_enableTorsoTurn;
	BOOL								m_enableRebasingOnUpVector;
	CRuVector3							m_upVector;
	CRuVector3							m_currentForwardVector;
	CRuVector3							m_targetForwardVector;			//!< Forward direction vector
	REAL								m_tiltMagnitude;
	CRuQuaternion						m_tiltQuaternion;

	REAL								m_upRotAngleRate;				//!< Up vector rotation rate
	REAL								m_curUpRotAngle;				//!< Current up vector rotation angle relative to side vector

	CRuMTRandom							m_randGen;
	REAL								m_nextIdlingPeriod;
	REAL								m_idlingTime;
	BOOL								m_lastUpdateIsIdle;

	// Settings
	OrientMode							m_orientMode;

	CRuSojournerMovementType			m_movementType;

	CRuFusion_ActorState				m_disableMovementOnAction;		//!< Disable movement next time this actor state is encountered
	BOOL								m_disableMovement;				//!< Movement is disabled
	BOOL								m_disableInPlaceJump;			//!< Is in-place jumping disabled? (For mounts only)

	REAL								m_slopeSlideThreshold;			//!< Angle beyond which sliding will cease to go upwards (in degrees)

	REAL								m_gravityAcceleration;

	REAL								m_rotationSpeed;				//!< hmmm....
	REAL								m_runThreshold;					//!< Threshold above which to begin using running animation
	REAL								m_scale;						//!< Actor scale

	CRuSegment3							m_swim_Surface_EntrySegment;	//!< Segment used for detecting entry into water
	CRuSegment3							m_swim_Surface_ExitSegment;		//!< Segment used for detecting exit from water
	CRuSegment3							m_swim_Dive_EntrySegment;		//!< Segment used for detecting entry into diving
	CRuSegment3							m_swim_Dive_ExitSegment;		//!< Segment used for detecting entry into diving

	CRuFusion_Mime_Action				m_defaultIdleAction;
	CRuFusion_Mime_Action				m_defaultWalkingAction;
	CRuFusion_Mime_Action				m_defaultRunningAction;
	CRuFusion_Mime_Action				m_defaultBackAction;

	// Movement data
	CRuVector3							m_inputVelocity;				//!< Current velocity due to user input (in canonical frame)
	CRuVector3							m_latentVelocity;				//!< Latent velocity (in local frame)
	REAL								m_gravitySpeed;					//!< Current speed contribution due to gravity
	BOOL								m_onFirmGround;					//!< Current position is on firm ground?
	REAL								m_timeSinceLastJump;
	REAL								m_stopPoint_OffsetFromBottom;
	REAL								m_stopPoint_OffsetFromTop;

	REAL								m_targetBodyHeadingOffset;
	REAL								m_targetTorsoHeadingOffset;
	REAL								m_targetYawOffset;
	REAL								m_currentBodyHeadingOffset;
	REAL								m_currentTorsoHeadingOffset;
	REAL								m_currentYawOffset;
	REAL								m_shufflingTime;

	// Actor links
	CRuFusion_Actor*					m_linkParent;				// Parent actor to which this actor is linked
	CRuArrayList<CRuFusion_Actor *>		m_linkedActors;				// List of actors linked to this actor

	// Profiling probes
	INT32								m_probe_ActorUpdate;

	REAL								m_deadTime;

public:
										CRuFusion_Actor();
	virtual								~CRuFusion_Actor();

	CRuEntity*							GetTargetEntity();
	CRuFusion_Mime*						GetTargetMime();
	BOOL								SetTargetMime(CRuFusion_Mime *targetMime);
	BOOL								SetOrientMode(CRuFusion_Actor::OrientMode orientMode);
	BOOL								SetMovementType(CRuSojournerMovementType movementType);

	BOOL								EnableSojourner(BOOL enable);
	CRuFusion_Sojourner*				GetSojourner();
	BOOL								SetCollisionObject(IRuObject *collisionObject);

	BOOL								IsAboveWater();
	BOOL								IsInWater();
	BOOL								IsSubmerged();

	BOOL								EnableTorsoTurn(BOOL enableTorsoTurn);
	void								SetEnableRebasingOnUpVector(BOOL enable);
	void								SetUpVector(const CRuVector3 &upVector);
	const CRuVector3&					GetTargetForwardVector();
	const CRuVector3&					GetCurrentForwardVector();
	void								SetForwardVector(CRuVector3 forwardVector, BOOL immediate = FALSE);
	void								SetGravityAcceleration(REAL gravityAcceleration);
	void								SetRunThreshold(REAL runThreshold);
	void								SetScale(REAL scale);
	void								SetDefaultIdleAction(CRuFusion_Mime_Action defaultIdleAction);
	void								SetDefaultWalkingAction(CRuFusion_Mime_Action defaultWalkingAction);
	void								SetDefaultRunningAction(CRuFusion_Mime_Action defaultRunningAction, CRuFusion_Mime_Action defaultBackAction = ruFUSION_MIME_NONE);
	void								SetDefaultBackAction(CRuFusion_Mime_Action defaultBackAction);

	CRuFusion_Mime_Action				GetDefaultIdleAction();
	CRuFusion_Mime_Action				GetDefaultWalkingAction();
	CRuFusion_Mime_Action				GetDefaultRunningAction();
	CRuFusion_Mime_Action				GetDefaultBackAction();

	void								Move(const CRuVector3 &impulse);

	BOOL								Jump(REAL risingImpulse);
	BOOL								ForcedJump(REAL risingImpulse, const CRuVector3 &latentVelocity);
	BOOL								DisableMovement(BOOL disableMovement);
	BOOL								DisableInPlaceJump(BOOL disableInPlaceJump);

	BOOL								Knockdown(const CRuVector3 &knockdownDirection);

	BOOL								NotifyPositionReset();

	REAL								GetGravitySpeed() const;
	const CRuVector3&					GetLatentVelocity() const;

	CRuFusion_ActorState				GetBackgroundState();
	BOOL								SetBackgroundState(CRuFusion_ActorState actorState, BOOL force = FALSE);
	CRuFusion_ActorState				GetForegroundState();
	BOOL								SetForegroundState(CRuFusion_ActorState actorState);
	CRuFusion_ActorState				GetTransientState();
	BOOL								SetTransientState(CRuFusion_ActorState actorState, BOOL disableMovement = FALSE, CRuACT_ActionEvents *actionEventsOut = NULL);

	BOOL								SetAuxiliaryState(CRuFusion_ActorState actorState);

	BOOL								ClearForegroundQueue();

	// Links
	void								LinkActor(CRuFusion_Actor *linkedActor);
	void								UnlinkActor(CRuFusion_Actor *linkedActor);
	void								SeverActorLinks();
	CRuFusion_Actor*					GetLinkParent();

	// Updates
	void								Update(REAL elapsedTime);

protected:
	// Internal state checks
	BOOL								CanTransitionToFalling();

	CRuSegment3							BuildMaximalTestSegment(const CRuSegment3 &refSegment, const CRuVector3 &pt0, const CRuVector3 &pt1);
	void								MimeActionChangedHandler(CRuFusion_Mime_Channel channel, CRuFusion_Mime_Action action, PTRVALUE eventData);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
