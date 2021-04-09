/*!
	@project Rune
	@file RuFusion_Mime.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/07/22
*/

#ifndef _RUFUSION_MIME_H_
#define _RUFUSION_MIME_H_

#include "../Scene/Base/RuEntityACT.h"
#include "../Scene/Controller/RuController_Bone.h"
#include "../Scene/Controller/RuController_Bone.h"
#include "../Scene/Controller/RuController_EventTrigger.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

enum CRuFusion_Mime_Action
{
	ruFUSION_MIME_NONE								= 0,				//!< No motion

	ruFUSION_MIME_HURT_NORMAL						= 1,
	ruFUSION_MIME_HURT_CRITICAL						= 2,
	ruFUSION_MIME_DODGE								= 3,
	ruFUSION_MIME_SPAWN								= 4,				//!< Special spawning motion

	ruFUSION_MIME_IDLE_STAND						= 11,				//!< Temporary idle motion
	ruFUSION_MIME_IDLE_STAND_02						= 12,				//!< Alternate stand-idle motion #02
	ruFUSION_MIME_IDLE_STAND_03						= 19,				//!< Alternate stand-idle motion #03
	ruFUSION_MIME_IDLE_COMBAT_1H					= 13,
	ruFUSION_MIME_IDLE_COMBAT_2H					= 14,
	ruFUSION_MIME_IDLE_COMBAT_BOW					= 15,
	ruFUSION_MIME_IDLE_COMBAT_GUN					= 16,
	ruFUSION_MIME_IDLE_COMBAT_POLEARM				= 17,
	ruFUSION_MIME_IDLE_COMBAT_UNARMED				= 18,	

	ruFUSION_MIME_ATTACK_1H							= 21,				//!< Standard 1H attack
	ruFUSION_MIME_ATTACK_1H_SP						= 22,				//!< Special 1H attack
	ruFUSION_MIME_ATTACK_1H_SP02					= 47,				//!< Special 1H attack
	ruFUSION_MIME_ATTACK_1H_SLOW01					= 48,				//!< Special 1H attack
	ruFUSION_MIME_ATTACK_1H_PIERCE					= 37,
	ruFUSION_MIME_ATTACK_2H							= 23,				//!< Standard 2H attack
	ruFUSION_MIME_ATTACK_2H_SP						= 24,				//!< Special 2H attack	
	ruFUSION_MIME_ATTACK_POLEARM					= 25,
	ruFUSION_MIME_ATTACK_POLEARM_SP					= 26,	
	ruFUSION_MIME_ATTACK_UNARMED					= 27,				//!< Standard unarmed attack
	ruFUSION_MIME_ATTACK_UNARMED_SP					= 28,				//!< Special unarmed attack	
	ruFUSION_MIME_ATTACK_BOW_BEGIN					= 29,
	ruFUSION_MIME_ATTACK_BOW_LOOP					= 30,
	ruFUSION_MIME_ATTACK_BOW_END					= 31,
	ruFUSION_MIME_ATTACK_BOW_SP_BEGIN				= 38,
	ruFUSION_MIME_ATTACK_BOW_SP_LOOP				= 39,
	ruFUSION_MIME_ATTACK_BOW_SP_END					= 40,
	ruFUSION_MIME_ATTACK_GUN_BEGIN					= 32,
	ruFUSION_MIME_ATTACK_GUN_LOOP					= 33,
	ruFUSION_MIME_ATTACK_GUN_END					= 34,
	ruFUSION_MIME_ATTACK_OH							= 35,
	ruFUSION_MIME_ATTACK_OH_PIERCE					= 36,

	ruFUSION_MIME_PARRY_1H							= 41,				//!< Standard 1H parry
	ruFUSION_MIME_PARRY_2H							= 42,				//!< Standard 2H parry
	ruFUSION_MIME_PARRY_BOW							= 43,
	ruFUSION_MIME_PARRY_GUN							= 44,
	ruFUSION_MIME_PARRY_POLEARM						= 45,
	ruFUSION_MIME_PARRY_UNARMED						= 46,

	ruFUSION_MIME_SHIELD_BASH						= 61,
	ruFUSION_MIME_SHIELD_BLOCK						= 62,

	ruFUSION_MIME_USE								= 65,
	ruFUSION_MIME_THROW								= 66,

	ruFUSION_MIME_BUFF_BEGIN						= 71,
	ruFUSION_MIME_BUFF_LOOP							= 72,
	ruFUSION_MIME_BUFF_END							= 73,
	ruFUSION_MIME_BUFF_INSTANT						= 74,

	ruFUSION_MIME_BUFF_SP01							= 161,
	ruFUSION_MIME_BUFF_SP02							= 162,
	ruFUSION_MIME_BUFF_SP03							= 163,

	ruFUSION_MIME_CAST_BEGIN						= 75,
	ruFUSION_MIME_CAST_LOOP							= 76,
	ruFUSION_MIME_CAST_END							= 77,
	ruFUSION_MIME_CAST_INSTANT						= 78,

	ruFUSION_MIME_CAST_SP01							= 164,
	ruFUSION_MIME_CAST_SP02							= 165,
	ruFUSION_MIME_CAST_SP03							= 166,
	ruFUSION_MIME_CAST_SP04							= 171,

	ruFUSION_MIME_DEBUFF_INSTANT					= 167,	

	ruFUSION_MIME_CHANNEL_BEGIN						= 79,
	ruFUSION_MIME_CHANNEL_LOOP						= 80,
	ruFUSION_MIME_CHANNEL_END						= 81,
	ruFUSION_MIME_CHANNEL_INSTANT					= 82,

	ruFUSION_MIME_FISHING_CAST						= 83,
	ruFUSION_MIME_FISHING_IDLE						= 84,
	ruFUSION_MIME_FISHING_REEL						= 85,	

	ruFUSION_MIME_SP_SPINKICK						= 91,
	ruFUSION_MIME_SP_SPINSLASH						= 92,

	ruFUSION_MIME_RUN_FORWARD						= 101,				//!<
	ruFUSION_MIME_RUN_CHARGE						= 102,				//!<
	ruFUSION_MIME_RUN_CHARGE2						= 110,				//!<
	ruFUSION_MIME_RUN_BACKWARD						= 103,				//!<
	ruFUSION_MIME_WALK_FORWARD						= 104,				//!<

	ruFUSION_MIME_SNEAK_FORWARD						= 118,				//!<
	ruFUSION_MIME_SNEAK_BACKWARD					= 119,				//!<

	ruFUSION_MIME_WALKSP_FORWARD					= 168,
	ruFUSION_MIME_RUNSP_FORWARD						= 169,
	ruFUSION_MIME_RUNSP_BACKWARD					= 170,

	ruFUSION_MIME_SWIM_FORWARD						= 105,				//!<
	ruFUSION_MIME_SWIM_BACKWARD						= 106,				//!<
	ruFUSION_MIME_SWIM_LEFT							= 107,				//!<
	ruFUSION_MIME_SWIM_RIGHT						= 108,				//!<
	ruFUSION_MIME_SWIM_IDLE							= 109,				//!< Temporary idle motion

	ruFUSION_MINE_HOLSTER							= 111,
	ruFUSION_MIME_UNHOLSTER							= 112,

	ruFUSION_MINE_HOLSTER_2H						= 113,
	ruFUSION_MIME_UNHOLSTER_2H						= 114,

	ruFUSION_MIME_SHUFFLE_LEFT						= 115,
	ruFUSION_MIME_SHUFFLE_RIGHT						= 116,

	ruFUSION_MIME_RIDE_IDLE							= 117,
	ruFUSION_MIME_RIDE_IDLE0						= 120,
	ruFUSION_MIME_RIDE_IDLE1						= 158,
	ruFUSION_MIME_RIDE_IDLE2						= 159,
	ruFUSION_MIME_RIDE_IDLE3						= 160,

	ruFUSION_MIME_KNOCKDOWN_BEGIN					= 181,				//!< Knockdown start
	ruFUSION_MIME_KNOCKDOWN_LOOP					= 182,				//!< Knockdown fly loop
	ruFUSION_MIME_KNOCKDOWN_END						= 183,				//!< Knockdown end (on ground)
	ruFUSION_MIME_KNOCKDOWN_RECOVER					= 184,				//!< Knockdown recover (from ground to standing)

	ruFUSION_MIME_EMOTE_HANDSTAND					= 185,
	ruFUSION_MIME_EMOTE_ROFL						= 186,
	ruFUSION_MIME_EMOTE_SWEEP1						= 187,
	ruFUSION_MIME_EMOTE_SWEEP2						= 188,
	ruFUSION_MIME_EMOTE_SWEEP3						= 189,

	// States
	ruFUSION_MIME_JUMP_DOWN							= 121,
	ruFUSION_MIME_JUMP_LOOP							= 122,
	ruFUSION_MIME_JUMP_UP							= 123,
	ruFUSION_MIME_JUMP_DOWNANDRUN					= 147,
	ruFUSION_MIME_JUMP_DOWNANDRUN_2					= 148,
	ruFUSION_MIME_JUMP_DOWNANDBACK					= 150,
	ruFUSION_MIME_JUMP_DOWNANDBACK_2				= 151,
	ruFUSION_MIME_JUMP_INPLACE						= 149,

	ruFUSION_MIME_SLEEP_DOWN						= 124,
	ruFUSION_MIME_SLEEP_LOOP						= 125,
	ruFUSION_MIME_SLEEP_UP							= 126,

	ruFUSION_MIME_SIT_DOWN							= 127,
	ruFUSION_MIME_SIT_LOOP							= 128,
	ruFUSION_MIME_SIT_UP							= 129,

	ruFUSION_MIME_DEATH_DOWN						= 130,
	ruFUSION_MIME_DEATH_LOOP						= 131,
	ruFUSION_MIME_DEATH_UP							= 132,

	ruFUSION_MIME_CROUCH_BEGIN						= 133,
	ruFUSION_MIME_CROUCH_LOOP						= 134,
	ruFUSION_MIME_CROUCH_END						= 135,	

	ruFUSION_MIME_CRAFTING_BEGIN					= 136,
	ruFUSION_MIME_CRAFTING_LOOP						= 137,
	ruFUSION_MIME_CRAFTING_END						= 138,

	ruFUSION_MIME_DEAD_WATER						= 20,

	// Fly
	ruFUSION_MIME_SPIRAL_RIGHT						= 190,
	ruFUSION_MIME_SPIRAL_LEFT						= 191,
	ruFUSION_MIME_SPIRAL_LOOP						= 192,
	ruFUSION_MIME_FLY_BEGIN							= 193,
	ruFUSION_MIME_FLY_LOOP							= 194,
	ruFUSION_MIME_FLY_END							= 195,
	
	ruFUSION_MIME_MINING_BEGIN						= 139,
	ruFUSION_MIME_MINING_LOOP						= 140,
	ruFUSION_MIME_MINING_END						= 141,

	ruFUSION_MIME_LUMBERJACK_BEGIN					= 251,
	ruFUSION_MIME_LUMBERJACK_LOOP					= 252,
	ruFUSION_MIME_LUMBERJACK_END					= 253,

	ruFUSION_MIME_GATHER_BEGIN						= 254,
	ruFUSION_MIME_GATHER_LOOP						= 255,
	ruFUSION_MIME_GATHER_END						= 256,

	ruFUSION_MIME_LOOT_BEGIN						= 257,
	ruFUSION_MIME_LOOT_LOOP							= 258,
	ruFUSION_MIME_LOOT_END							= 259,

	ruFUSION_MIME_CARRY_BEGIN						= 260,
	ruFUSION_MIME_CARRY_LOOP						= 261,
	ruFUSION_MIME_CARRY_END							= 262,

	ruFUSION_MIME_DAZED								= 142,

	ruFUSION_MIME_SLUMBER							= 143,

	ruFUSION_MIME_ACTIVATE_BEGIN					= 144,
	ruFUSION_MIME_ACTIVATE_LOOP						= 145,
	ruFUSION_MIME_ACTIVATE_END						= 146,

	ruFUSION_MIME_READING_BEGIN						= 152,
	ruFUSION_MIME_READING_LOOP						= 153,
	ruFUSION_MIME_READING_END						= 154,

	ruFUSION_MIME_SIT_CHAIR_BEGIN					= 155,
	ruFUSION_MIME_SIT_CHAIR_LOOP					= 156,
	ruFUSION_MIME_SIT_CHAIR_END						= 157,

	// Emotes
	ruFUSION_MIME_EMOTE_ANGRY						= 201,
	ruFUSION_MIME_EMOTE_APPLAUSE					= 202,
	ruFUSION_MIME_EMOTE_APPROVAL					= 203,
	ruFUSION_MIME_EMOTE_BEG							= 204,
	ruFUSION_MIME_EMOTE_CRY							= 205,
	ruFUSION_MIME_EMOTE_HEAD_NOD					= 206,
	ruFUSION_MIME_EMOTE_HEAD_SHAKE					= 207,
	ruFUSION_MIME_EMOTE_LAUGH						= 208,
	ruFUSION_MIME_EMOTE_POINT						= 209,
	ruFUSION_MIME_EMOTE_PROVOKE						= 210,
	ruFUSION_MIME_EMOTE_SALUTE						= 211,
	ruFUSION_MIME_EMOTE_SPEAK						= 212,
	ruFUSION_MIME_EMOTE_SURRENDER					= 213,
	ruFUSION_MIME_EMOTE_THREATEN					= 214,
	ruFUSION_MIME_EMOTE_VICTORY						= 215,
	ruFUSION_MIME_EMOTE_WAVE						= 216,
	ruFUSION_MIME_EMOTE_EAT							= 217,
	ruFUSION_MIME_EMOTE_DRINK						= 218,
	ruFUSION_MIME_EMOTE_IDLE						= 219,
	ruFUSION_MIME_EMOTE_IDLE0						= 220,
	ruFUSION_MIME_EMOTE_IDLE1						= 221,
	ruFUSION_MIME_EMOTE_IDLE2						= 222,
	ruFUSION_MIME_EMOTE_DANCE						= 223,
	ruFUSION_MIME_EMOTE_PLAY_SAXOPHONE				= 224,
	ruFUSION_MIME_EMOTE_PLAY_TAMBOURINE				= 225,
	ruFUSION_MIME_EMOTE_PLAY_GUITAR					= 226,
	ruFUSION_MIME_EMOTE_JUMP_INPLACE				= 227,
	ruFUSION_MIME_EMOTE_BELLY						= 228,
	ruFUSION_MIME_EMOTE_EMBRAASSED					= 229,
	ruFUSION_MIME_EMOTE_EMBRACE						= 230,
	ruFUSION_MIME_EMOTE_KISS						= 350,
	ruFUSION_MIME_EMOTE_SALUTE2						= 351,
	ruFUSION_MIME_EMOTE_CUTE1						= 352,
	ruFUSION_MIME_EMOTE_CUTE2						= 353,
	ruFUSION_MIME_EMOTE_STRETCH						= 354,
	ruFUSION_MIME_EMOTE_ROCK						= 355,
	ruFUSION_MIME_EMOTE_KAMEHAMEHA					= 356,
	ruFUSION_MIME_EMOTE_KICK						= 357,	
	ruFUSION_MIME_EMOTE_BREAKDANCE					= 360,
	ruFUSION_MIME_EMOTE_TAPDANCE					= 361,
	ruFUSION_MIME_EMOTE_TIRED						= 362,
	ruFUSION_MIME_EMOTE_FEAR						= 363,
	ruFUSION_MIME_EMOTE_AMAZED						= 364,
	ruFUSION_MIME_EMOTE_THINK						= 365,
	ruFUSION_MIME_EMOTE_CUTE3						= 366,
	ruFUSION_MIME_EMOTE_HANGOVER					= 367,

	ruFUSION_MIME_IDLE_CROUCH						= 241,
	ruFUSION_MIME_IDLE_DEAD							= 242,
	ruFUSION_MIME_IDLE_SIT							= 243,
	ruFUSION_MIME_IDLE_SLEEP						= 244,

	ruFUSION_MIME_ACTION_COUNT						= 500,
	ruFUSION_MIME_FORCE_DWORD						= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum CRuFusion_Mime_Channel
{
	ruFUSION_MIME_CHANNEL_FOREGROUND				= 0,
	ruFUSION_MIME_CHANNEL_BACKGROUND				= 1,

	ruFUSION_MIME_CHANNEL_COUNT						= 2,

	ruFUSION_MIME_CHANNEL_FORCE_DWORD				= 0x7FFFFFFF
};

enum CRuFusion_Mime_ActionType
{
	ruFUSION_MIME_ACTIONTYPE_NORMAL					= 0,				//!< Normal action
	ruFUSION_MIME_ACTIONTYPE_IDLELOOP				= 1,				//!< Action is an idle loop
	ruFUSION_MIME_ACTIONTYPE_LOWPRIORITY			= 2,				//!< Action is low-priority
	ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY		= 3,				//!< Action is ultra low-priority

	ruFUSION_MIME_ACTIONTYPE_FORCE_DWORD			= 0x7FFFFFFF
};

enum CRuFusion_Mime_SetActionMethod
{
	ruFUSION_MIME_SETACTIONMETHOD_NORMAL			= 0,				//!< Normal action
	ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE			= 1,				//!< Exclusive action
	ruFUSION_MIME_SETACTIONMETHOD_QUEUED			= 2,				//!< Queued action

	ruFUSION_MIME_SETACTIONMETHOD_FORCE_DWORD		= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

class CRuFusion_Mime;

// ************************************************************************************************************************************************************

class RuMimeEvent_DequeueAction_Args : public RuEventArgs
{
public:
	CRuFusion_Mime*						m_mime;							//!< Event source
	CRuFusion_Mime_Channel				m_channel;						//!< Mime channel being dequeued
	CRuFusion_Mime_Action				m_action;						//!< Action being dequeued

	RuMimeEvent_DequeueAction_Args(CRuFusion_Mime *mime, CRuFusion_Mime_Channel channel, CRuFusion_Mime_Action action)
	:	m_mime(mime),
		m_channel(channel),
		m_action(action)
	{
	}
};

// ************************************************************************************************************************************************************

struct CRuFusion_Mime_BoneScaler
{
	CRuString*							m_boneScalerName;
	CRuString*							m_targetBoneName;

	CRuArrayList<CRuString *>			m_terminatorBoneNames;						//!< List of "terminators" at which this bone scaler should stop affecting the skeleton

	CRuString*							m_heightCompensationTargetBoneName;
//	char*							

//	BOOL								m_enableHeightCompensation;					//!< Compensate for height expansion resulting from scaling on this bone?

										CRuFusion_Mime_BoneScaler();
										~CRuFusion_Mime_BoneScaler();
};

// ************************************************************************************************************************************************************

class CRuFusion_Mime_Template : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Action descriptor
	struct CRuFusion_Mime_ActionDescriptor
	{
		INT32										m_actionPriority;							//!< Action priority
		INT32										m_actionLayer;								//!< Action layer (0 = default, 1 = transient)
		CRuFusion_Mime_ActionType					m_actionType;								//!< Action type
		BOOL										m_actionOverrides;							//!< Action overrides low-priority actions?
		REAL										m_actionMaxBlendDelay;						//!< Action's maximum blending delay
		CRuFusion_Mime_Action						m_chainableAction;							//!< Action which can be chained to this action with no blend delay
		CRuArrayList<const char *>					m_motionNames;								//!< Array of motion names
		CRuArrayList<REAL>							m_motionProbabilities;						//!< Array of motion probabilities

										CRuFusion_Mime_ActionDescriptor();
										~CRuFusion_Mime_ActionDescriptor();

		void							SetActionType(CRuFusion_Mime_ActionType actionType);
		INT32							AddMotion(const char *motionName, REAL motionProbability = 1.0f);

		INT32							GetNumMotions() const;
		const char*						GetMotion(INT32 motionIndex) const;
		REAL							GetMotionProbability(INT32 motionIndex) const;
	};

	// Rotational bone descriptor
	struct CRuFusion_Mime_Bone_R
	{
		char*							m_boneName;									//!< Name of bone
		REAL							m_weight;									//!< Weight

										CRuFusion_Mime_Bone_R()
										:	m_boneName(NULL),
											m_weight(1.0f)
										{
										}

										CRuFusion_Mime_Bone_R(const char *boneName, REAL weight)
										:	m_boneName(NULL),
											m_weight(weight)
										{
											if(boneName)
											{
												m_boneName = ruNEW char [strlen(boneName) + 1];
												strcpy(m_boneName, boneName);
											}
										}
	};

	// Members
	CRuMTRandom							m_randGen;									//!< Build-in random number generator
	CRuFusion_Mime_ActionDescriptor*	m_actionDescriptors;						//!< List of action descriptors
	CRuArrayList<CRuFusion_Mime_Bone_R>	m_bodyBones;								//!< List of main body bones, used for body rotation
	CRuArrayList<CRuFusion_Mime_Bone_R>	m_torsoBones;								//!< List of torso bones, used for torso twisting

	CRuArrayList<CRuFusion_Mime_BoneScaler *>	m_boneScalers;

public:
										CRuFusion_Mime_Template();
	virtual								~CRuFusion_Mime_Template();

	BOOL								LoadSettings(IRuStream *settingsStream);
	BOOL								SaveSettings(IRuStream *settingsStream);

	INT32								GetActionPriority(CRuFusion_Mime_Action action);
	INT32								GetActionLayer(CRuFusion_Mime_Action action);
	CRuFusion_Mime_ActionType			GetActionType(CRuFusion_Mime_Action action);
	BOOL								GetActionOverrides(CRuFusion_Mime_Action action);
	REAL								GetActionMaxBlendDelay(CRuFusion_Mime_Action action);
	CRuFusion_Mime_Action				GetChainableAction(CRuFusion_Mime_Action action);
	const char*							GetMotion(CRuFusion_Mime_Action action);

	INT32								GetNumBodyBones() const;
	const char*							GetBodyBoneName(INT32 index) const;
	REAL								GetBodyBoneWeight(INT32 index) const;

	INT32								GetNumTorsoBones() const;
	const char*							GetTorsoBoneName(INT32 index) const;
	REAL								GetTorsoBoneWeight(INT32 index) const;

	INT32								GetNumBoneScalers() const;
	CRuFusion_Mime_BoneScaler*			GetBoneScaler(INT32 index);

	// Helpers
	void								InitializeStandardBoneRotators();
	void								InitializeDeprecatedBoneRotators();

	void								InitializeStandardBoneScalers();
	void								InitializeDeprecatedBoneScalers();
};

// ************************************************************************************************************************************************************

class CRuFusion_Mime : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	//!< Defined: void ActionChangedCallback(CRuFusion_Mime_Channel channel, CRuFusion_Mime_Action action, PTRVALUE eventData);
	typedef boost::function<void (CRuFusion_Mime_Channel channel, CRuFusion_Mime_Action action, PTRVALUE eventData)> ActionChangedCallback;

protected:
	struct BoneScalerDesc
	{
		// Template data
		CRuFusion_Mime_BoneScaler*		m_boneScaler;								//!< Bone scaler template

		// Runtime
		CRuVector3						m_boneScale;								//!< Current bone scale

		// Deprecated
		CRuController_Bone*				m_boneScalerController;						//!< Master bone controller
	};

	struct ActionDesc
	{
		CRuFusion_Mime_Action			m_action;
		const char*						m_motionName;
		REAL							m_motionLength;
		PTRVALUE						m_eventData;
		BOOL							m_exclusiveAction;
		CRuACT_ActionEvents*			m_actionEvents;

		DWORD							m_actionTimeStamp;							//!< Time at which action was invoked

										ActionDesc()
										{
											m_action = ruFUSION_MIME_NONE;
											m_motionName = "";
											m_motionLength = 0.0f;
											m_eventData = 0;
											m_exclusiveAction = FALSE;
											m_actionEvents = NULL;
											m_actionTimeStamp = 0;
										}

										ActionDesc(CRuFusion_Mime_Action action, const char *motionName, PTRVALUE eventData, BOOL exclusiveAction, CRuACT_ActionEvents *actionEvents)
										{
											m_action = action;
											m_motionName = motionName;
											m_motionLength = 0.0f;
											m_eventData = eventData;
											m_exclusiveAction = exclusiveAction;
											m_actionEvents = actionEvents;
											m_actionTimeStamp = 0;
										}
	};

	struct ChannelDesc
	{
		UINT32							m_motionChannel;
		CRuFusion_Mime_Action			m_activeAction;
		BOOL							m_exclusiveAction;
		REAL							m_blendDelay;
		ActionDesc						m_defaultAction;
		const char*						m_motionName;
		BOOL							m_motionIsInterruptible;
		BOOL							m_motionJustDequeued;
		CRuACT_ActionEvents*			m_actionEvents;
	};

	// Events
	CRuEvent							m_event_DequeueAction;

	// Mime definition
	CRuFusion_Mime_Template*			m_mimeTemplate;
	ActionChangedCallback				m_actionChangedCallback;
	UINT32								m_defaultChannel;

	UINT32								m_auxiliaryChannel;
	UINT32								m_backgroundChannel;
	UINT32								m_foregroundChannel;
	UINT32								m_userChannel;

	REAL								m_blendDelay;

	// Target and its status
	CRuEntity*							m_targetEntity;										//!< Target entity
	CRuACTEntity*						m_targetACT;
	CRuEntity*							m_targetHierarchy;
	BOOL								m_targetEntitySupportsChannels;						//!< Does the target entity support channels?
	UINT32								m_backgroundActive;

	// Action channel data
	ChannelDesc*						m_channels;											//!<
	CRuArrayList<ActionDesc>*			m_queuedActions;

	// Skeletal rotation
	REAL								m_body_YawOffset;
	REAL								m_body_HeadingOffset;
	CRuController_Bone*					m_body_BoneController;

	REAL								m_torso_HeadingOffset;
	CRuController_Bone*					m_torso_BoneController;								//!< Bone controller that acts on this bone group

	// Skeletal scaling
	REAL								m_bodyScale;
	CRuArrayList<BoneScalerDesc>		m_boneScalerDescriptors;

	// Other stuff
	REAL								m_expectedMotionScale;

	REAL								m_defaultForwardSpeed;
	REAL								m_forwardSpeed;

	REAL								m_defaultBackwardSpeed;
	REAL								m_backwardSpeed;

	INT32								m_nextSeqNumber;

public:
										CRuFusion_Mime();
	virtual								~CRuFusion_Mime();

	CRuEvent&							Event_DequeueAction();

	CRuEntity*							GetTarget() const;
	BOOL								SetTarget(CRuEntity *targetEntity, BOOL disableCompensators = FALSE);
	BOOL								SetupMotionChannels();

	BOOL								SetMimeTemplate(CRuFusion_Mime_Template *mimeTemplate);
	BOOL								SetActionChangedCallback(ActionChangedCallback actionChangedCallback);

	CRuFusion_Mime_Action				GetAction(CRuFusion_Mime_Channel channel);
	BOOL								SetAction(CRuFusion_Mime_Channel channel, CRuFusion_Mime_Action action, CRuFusion_Mime_SetActionMethod setActionMethod, PTRVALUE eventData, CRuACT_ActionEvents *actionEventsOut, BOOL setAsDefaultAction);
	BOOL								SetDefaultAction(CRuFusion_Mime_Channel channel, CRuFusion_Mime_Action action, PTRVALUE eventData);
	BOOL								RestoreDefaultAction(CRuFusion_Mime_Channel channel);
	INT32								GetQueuedActionCount(CRuFusion_Mime_Channel channel);
	BOOL								HasQueuedAction(CRuFusion_Mime_Channel channel, CRuFusion_Mime_Action action);
	BOOL								RemoveQueuedAction(CRuFusion_Mime_Channel channel, CRuFusion_Mime_Action action);
	BOOL								ForceActionDequeue(CRuFusion_Mime_Channel channel);
	BOOL								ClearQueuedActions(CRuFusion_Mime_Channel channel);

	BOOL								SetBodyYawOffset(REAL yawOffset);
	BOOL								SetBodyHeadingOffset(REAL headingOffset);
	BOOL								SetTorsoHeadingOffset(REAL headingOffset);

	BOOL								SetExpectedMotionScale(REAL expectedMotionScale);

	BOOL								SetDefaultForwardSpeed(REAL defaultForwardSpeed);
	BOOL								SetForwardSpeed(REAL forwardSpeed);

	BOOL								SetDefaultBackwardSpeed(REAL defaultBackwardSpeed);
	BOOL								SetBackwardSpeed(REAL backwardSpeed);

	INT32								GetNumBoneScalers();
	const char*							GetBoneScalerNameByIndex(INT32 index);
	BOOL								GetBoneScalerParametersByIndex(INT32 index, CRuVector3 &scaleOut);
	BOOL								SetBoneScalerParameters(const char *boneScalerName, const CRuVector3 &scale);

	void								Update(REAL elapsedTime);
	void								Update_WithChannel(REAL elapsedTime);
	void								Update_WithoutChannel(REAL elapsedTime);

protected:
	void								InitializeBoneControllers(IRuObject *hierarchyObj);

	REAL								GetMotionScale(CRuFusion_Mime_Action action);
	BOOL								QueueAction(CRuFusion_Mime_Channel channel, const ActionDesc &actionDesc);
	BOOL								DequeueAction(CRuFusion_Mime_Channel channel, BOOL removeOnly);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
