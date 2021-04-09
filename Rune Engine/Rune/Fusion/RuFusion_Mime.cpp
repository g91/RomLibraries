/*!
	@project Rune
	@file RuFusion_Mime.cpp

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/07/22
*/

#include "../Fusion/RuFusion_Mime.h"
#include "../Helper/RuHelper_Entity.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFusion_Mime_Template, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuFusion_Mime_Template, "CRuFusion_Mime_Template", "IRuObject")
ruCLASSGUID_DEFINE(CRuFusion_Mime_Template, 0x2DBA8869, 0xE7874df9, 0x987EE60C, 0xFC1E05EC)

ruRTTI_DEFINE_SUBCLASS(CRuFusion_Mime, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuFusion_Mime, "CRuFusion_Mime", "IRuObject")
ruCLASSGUID_DEFINE(CRuFusion_Mime, 0xF7962690, 0x962F4eb9, 0x985BA20D, 0x8680734D)

// ************************************************************************************************************************************************************

CRuFusion_Mime_BoneScaler::CRuFusion_Mime_BoneScaler()
:	m_boneScalerName(NULL),
	m_targetBoneName(NULL),
	m_heightCompensationTargetBoneName(NULL)
{
}

CRuFusion_Mime_BoneScaler::~CRuFusion_Mime_BoneScaler()
{
	delete m_boneScalerName;
	delete m_targetBoneName;

	for(INT32 j = 0; j < m_terminatorBoneNames.Count(); ++j)
	{
		delete m_terminatorBoneNames[j];
	}

	delete m_heightCompensationTargetBoneName;
}

// ************************************************************************************************************************************************************

CRuFusion_Mime_Template::CRuFusion_Mime_ActionDescriptor::CRuFusion_Mime_ActionDescriptor()
:	m_actionPriority(0),
	m_actionLayer(0),
	m_actionType(ruFUSION_MIME_ACTIONTYPE_NORMAL),
	m_actionOverrides(FALSE),
	m_actionMaxBlendDelay(FLT_MAX)
{
}

CRuFusion_Mime_Template::CRuFusion_Mime_ActionDescriptor::~CRuFusion_Mime_ActionDescriptor()
{
	for(INT32 i = 0; i < m_motionNames.Count(); ++i)
	{
		delete[] m_motionNames[i];
	}

	m_motionNames.Clear();
}

void CRuFusion_Mime_Template::CRuFusion_Mime_ActionDescriptor::SetActionType(CRuFusion_Mime_ActionType actionType)
{
	m_actionType = actionType;
}

INT32 CRuFusion_Mime_Template::CRuFusion_Mime_ActionDescriptor::AddMotion(const char *motionName, REAL motionProbability)
{
	char *newMotionName = ruNEW char [strlen(motionName) + 1];
	memcpy(newMotionName, motionName, sizeof(char) * (strlen(motionName) + 1));

	m_motionNames.Add(newMotionName);
	m_motionProbabilities.Add(motionProbability);

	return m_motionNames.Count() - 1;
}

INT32 CRuFusion_Mime_Template::CRuFusion_Mime_ActionDescriptor::GetNumMotions() const
{
	return m_motionNames.Count();
}

const char *CRuFusion_Mime_Template::CRuFusion_Mime_ActionDescriptor::GetMotion(INT32 motionIndex) const
{
	return m_motionNames[motionIndex];
}

REAL CRuFusion_Mime_Template::CRuFusion_Mime_ActionDescriptor::GetMotionProbability(INT32 motionIndex) const
{
	return m_motionProbabilities[motionIndex];
}

// ************************************************************************************************************************************************************

CRuFusion_Mime_Template::CRuFusion_Mime_Template()
{
	m_actionDescriptors = ruNEW CRuFusion_Mime_ActionDescriptor [ruFUSION_MIME_ACTION_COUNT];

	m_actionDescriptors[ruFUSION_MIME_HURT_NORMAL].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_HURT_NORMAL].AddMotion("hurt");

	m_actionDescriptors[ruFUSION_MIME_HURT_CRITICAL].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_HURT_CRITICAL].AddMotion("hurt");

	m_actionDescriptors[ruFUSION_MIME_DODGE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_DODGE].AddMotion("dodge");

	m_actionDescriptors[ruFUSION_MIME_SPAWN].m_actionPriority = 40;
	m_actionDescriptors[ruFUSION_MIME_SPAWN].AddMotion("spawn");

	m_actionDescriptors[ruFUSION_MIME_IDLE_STAND].m_actionPriority = 0;
	m_actionDescriptors[ruFUSION_MIME_IDLE_STAND].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_IDLE_STAND].m_actionOverrides = TRUE;
	m_actionDescriptors[ruFUSION_MIME_IDLE_STAND].AddMotion("stand_idle01", 10.0f);

	m_actionDescriptors[ruFUSION_MIME_IDLE_STAND_02].m_actionPriority = 0;
	m_actionDescriptors[ruFUSION_MIME_IDLE_STAND_02].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_IDLE_STAND_02].AddMotion("stand_idle02", 10.0f);

	m_actionDescriptors[ruFUSION_MIME_IDLE_STAND_03].m_actionPriority = 0;
	m_actionDescriptors[ruFUSION_MIME_IDLE_STAND_03].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_IDLE_STAND_03].AddMotion("stand_idle03", 10.0f);

	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_1H].m_actionPriority = 0;
	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_1H].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_1H].AddMotion("1h_idle");

	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_2H].m_actionPriority = 0;
	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_2H].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_2H].AddMotion("2h_idle");

	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_BOW].m_actionPriority = 0;
	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_BOW].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_BOW].AddMotion("bow_idle");

	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_GUN].m_actionPriority = 0;
	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_GUN].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_GUN].AddMotion("gun_idle");

	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_POLEARM].m_actionPriority = 0;
	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_POLEARM].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_POLEARM].AddMotion("polearm_idle");

	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_UNARMED].m_actionPriority = 0;
	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_UNARMED].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_IDLE_COMBAT_UNARMED].AddMotion("unarmed_idle");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H].AddMotion("1h_attack01");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H].AddMotion("1h_attack02");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H].AddMotion("1h_attack03");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H].AddMotion("1h_attack04");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H].AddMotion("1h_attack05");
	//m_actionDescriptors[ruFUSION_MIME_ATTACK_1H].AddMotion("1h_slow_attack01");	

	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_SP].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_SP].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_SP].AddMotion("1h_attack01");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_SP].AddMotion("1h_attack02");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_SP].AddMotion("1h_attack03");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_SP].AddMotion("1h_attack04");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_SP].AddMotion("1h_attack05");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_SP02].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_SP02].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_SP02].AddMotion("1h_sp02");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_SLOW01].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_SLOW01].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_SLOW01].AddMotion("1h_slow_attack01");

//	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_SP].AddMotion("1h_slow_attack01");
//	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_SP].AddMotion("1h_sp01");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_PIERCE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_PIERCE].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_1H_PIERCE].AddMotion("1h_pierce01");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_2H].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_2H].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_2H].AddMotion("2h_attack01");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_2H].AddMotion("2h_attack02");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_2H].AddMotion("2h_attack03");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_2H].AddMotion("2h_attack04");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_2H].AddMotion("2h_attack05");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_2H].AddMotion("2h_slow_attack01");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_2H_SP].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_2H_SP].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_2H_SP].AddMotion("2h_attack01");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_2H_SP].AddMotion("2h_attack02");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_2H_SP].AddMotion("2h_attack03");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_2H_SP].AddMotion("2h_attack04");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_2H_SP].AddMotion("2h_attack05");
//	m_actionDescriptors[ruFUSION_MIME_ATTACK_2H_SP].AddMotion("2h_sp01");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_BEGIN].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_BEGIN].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_BEGIN].AddMotion("bow_ready");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_LOOP].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_LOOP].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_LOOP].AddMotion("bow_channel");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_END].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_END].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_END].AddMotion("bow_release");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_SP_BEGIN].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_SP_BEGIN].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_SP_BEGIN].AddMotion("bow_ready_sp");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_SP_LOOP].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_SP_LOOP].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_SP_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_SP_LOOP].AddMotion("bow_channel_sp");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_SP_END].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_SP_END].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_BOW_SP_END].AddMotion("bow_release_sp");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_GUN_BEGIN].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_GUN_BEGIN].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_GUN_BEGIN].AddMotion("gun_ready");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_GUN_LOOP].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_GUN_LOOP].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_GUN_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_GUN_LOOP].AddMotion("gun_channel");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_GUN_END].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_GUN_END].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_GUN_END].AddMotion("gun_release");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_OH].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_OH].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_OH].AddMotion("oh_attack01");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_OH_PIERCE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_OH_PIERCE].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_OH_PIERCE].AddMotion("oh_pierce01");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_POLEARM].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_POLEARM].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_POLEARM].AddMotion("polearm_attack01");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_POLEARM].AddMotion("polearm_attack02");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_POLEARM].AddMotion("polearm_attack03");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_POLEARM].AddMotion("polearm_attack04");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_POLEARM].AddMotion("polearm_attack05");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_POLEARM].AddMotion("polearm_slow_attack01");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_POLEARM_SP].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_POLEARM_SP].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_POLEARM_SP].AddMotion("polearm_attack01");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_POLEARM_SP].AddMotion("polearm_attack02");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_POLEARM_SP].AddMotion("polearm_attack03");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_POLEARM_SP].AddMotion("polearm_attack04");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_POLEARM_SP].AddMotion("polearm_attack05");
//	m_actionDescriptors[ruFUSION_MIME_ATTACK_POLEARM_SP].AddMotion("polearm_sp01");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_UNARMED].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_UNARMED].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_UNARMED].AddMotion("unarmed_attack01");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_UNARMED].AddMotion("unarmed_attack02");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_UNARMED].AddMotion("unarmed_attack03");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_UNARMED].AddMotion("unarmed_attack04");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_UNARMED].AddMotion("unarmed_attack05");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_UNARMED].AddMotion("unarmed_slow_attack01");

	m_actionDescriptors[ruFUSION_MIME_ATTACK_UNARMED_SP].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_UNARMED_SP].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_ATTACK_UNARMED_SP].AddMotion("unarmed_attack01");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_UNARMED_SP].AddMotion("unarmed_attack02");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_UNARMED_SP].AddMotion("unarmed_attack03");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_UNARMED_SP].AddMotion("unarmed_attack04");
	m_actionDescriptors[ruFUSION_MIME_ATTACK_UNARMED_SP].AddMotion("unarmed_attack05");
//	m_actionDescriptors[ruFUSION_MIME_ATTACK_UNARMED_SP].AddMotion("unarmed_sp01");

	m_actionDescriptors[ruFUSION_MIME_PARRY_1H].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_PARRY_1H].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_PARRY_1H].AddMotion("1h_parry");

	m_actionDescriptors[ruFUSION_MIME_PARRY_2H].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_PARRY_2H].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_PARRY_2H].AddMotion("2h_parry");

	m_actionDescriptors[ruFUSION_MIME_PARRY_BOW].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_PARRY_BOW].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_PARRY_BOW].AddMotion("bow_parry");

	m_actionDescriptors[ruFUSION_MIME_PARRY_GUN].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_PARRY_GUN].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_PARRY_GUN].AddMotion("gun_parry");

	m_actionDescriptors[ruFUSION_MIME_PARRY_POLEARM].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_PARRY_POLEARM].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_PARRY_POLEARM].AddMotion("polearm_parry");

	m_actionDescriptors[ruFUSION_MIME_PARRY_UNARMED].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_PARRY_UNARMED].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_PARRY_UNARMED].AddMotion("unarmed_parry");

	m_actionDescriptors[ruFUSION_MIME_SHIELD_BASH].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_SHIELD_BASH].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_SHIELD_BASH].AddMotion("shield_bash");

	m_actionDescriptors[ruFUSION_MIME_SHIELD_BLOCK].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_SHIELD_BLOCK].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_SHIELD_BLOCK].AddMotion("shield_block");

	m_actionDescriptors[ruFUSION_MIME_USE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_USE].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_USE].AddMotion("use_item");

	m_actionDescriptors[ruFUSION_MIME_THROW].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_THROW].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_THROW].AddMotion("throw");

	m_actionDescriptors[ruFUSION_MIME_BUFF_BEGIN].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_BUFF_BEGIN].m_actionLayer = 1;

	m_actionDescriptors[ruFUSION_MIME_BUFF_LOOP].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_BUFF_LOOP].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_BUFF_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_BUFF_LOOP].AddMotion("continuous_buff");

	m_actionDescriptors[ruFUSION_MIME_BUFF_END].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_BUFF_END].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_BUFF_END].AddMotion("buff01");

	m_actionDescriptors[ruFUSION_MIME_BUFF_INSTANT].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_BUFF_INSTANT].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_BUFF_INSTANT].AddMotion("buff01");

	m_actionDescriptors[ruFUSION_MIME_BUFF_SP01].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_BUFF_SP01].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_BUFF_SP01].AddMotion("buff_sp01");

	m_actionDescriptors[ruFUSION_MIME_BUFF_SP02].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_BUFF_SP02].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_BUFF_SP02].AddMotion("buff_sp02");

	m_actionDescriptors[ruFUSION_MIME_BUFF_SP03].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_BUFF_SP03].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_BUFF_SP03].AddMotion("buff_sp03");

	m_actionDescriptors[ruFUSION_MIME_CAST_BEGIN].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_CAST_BEGIN].m_actionLayer = 1;

	m_actionDescriptors[ruFUSION_MIME_CAST_LOOP].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_CAST_LOOP].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_CAST_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_CAST_LOOP].AddMotion("casting_delay");

	m_actionDescriptors[ruFUSION_MIME_CAST_END].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_CAST_END].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_CAST_END].AddMotion("cast01");

	m_actionDescriptors[ruFUSION_MIME_CAST_INSTANT].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_CAST_INSTANT].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_CAST_INSTANT].AddMotion("cast01");

	m_actionDescriptors[ruFUSION_MIME_CAST_SP01].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_CAST_SP01].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_CAST_SP01].AddMotion("cast_sp01");

	m_actionDescriptors[ruFUSION_MIME_CAST_SP02].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_CAST_SP02].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_CAST_SP02].AddMotion("cast_sp02");

	m_actionDescriptors[ruFUSION_MIME_CAST_SP03].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_CAST_SP03].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_CAST_SP03].AddMotion("cast_sp03");

	m_actionDescriptors[ruFUSION_MIME_CAST_SP04].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_CAST_SP04].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_CAST_SP04].AddMotion("cast_sp04");

	m_actionDescriptors[ruFUSION_MIME_DEBUFF_INSTANT].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_DEBUFF_INSTANT].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_DEBUFF_INSTANT].AddMotion("debuff01");

	m_actionDescriptors[ruFUSION_MIME_CHANNEL_BEGIN].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_CHANNEL_BEGIN].m_actionLayer = 1;

	m_actionDescriptors[ruFUSION_MIME_CHANNEL_LOOP].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_CHANNEL_LOOP].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_CHANNEL_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_CHANNEL_LOOP].AddMotion("continuous_cast");

	m_actionDescriptors[ruFUSION_MIME_CHANNEL_END].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_CHANNEL_END].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_CHANNEL_END].AddMotion("cast01");

	m_actionDescriptors[ruFUSION_MIME_CHANNEL_INSTANT].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_CHANNEL_INSTANT].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_CHANNEL_INSTANT].AddMotion("cast01");

	m_actionDescriptors[ruFUSION_MIME_FISHING_CAST].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_FISHING_CAST].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_FISHING_CAST].AddMotion("fishing_cast");

	m_actionDescriptors[ruFUSION_MIME_FISHING_REEL].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_FISHING_REEL].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_FISHING_REEL].AddMotion("fishing_reel");

	m_actionDescriptors[ruFUSION_MIME_FISHING_IDLE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_FISHING_IDLE].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_FISHING_IDLE].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_FISHING_IDLE].AddMotion("fishing_idle");

	m_actionDescriptors[ruFUSION_MIME_SP_SPINKICK].m_actionPriority = 40;
	m_actionDescriptors[ruFUSION_MIME_SP_SPINKICK].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_SP_SPINKICK].AddMotion("spinkick");

	m_actionDescriptors[ruFUSION_MIME_SP_SPINSLASH].m_actionPriority = 40;
	m_actionDescriptors[ruFUSION_MIME_SP_SPINSLASH].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_SP_SPINSLASH].AddMotion("spinslash");

	m_actionDescriptors[ruFUSION_MIME_RUN_FORWARD].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_RUN_FORWARD].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_RUN_FORWARD].AddMotion("run_forward");

	m_actionDescriptors[ruFUSION_MIME_RUN_CHARGE].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_RUN_CHARGE].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_RUN_CHARGE].AddMotion("charge");

	m_actionDescriptors[ruFUSION_MIME_RUN_CHARGE2].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_RUN_CHARGE2].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_RUN_CHARGE2].AddMotion("charge02");

	m_actionDescriptors[ruFUSION_MIME_RUN_BACKWARD].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_RUN_BACKWARD].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_RUN_BACKWARD].AddMotion("run_back");

	m_actionDescriptors[ruFUSION_MIME_WALK_FORWARD].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_WALK_FORWARD].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_WALK_FORWARD].AddMotion("walk_forward");

	m_actionDescriptors[ruFUSION_MIME_SNEAK_FORWARD].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_SNEAK_FORWARD].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_SNEAK_FORWARD].AddMotion("sneak_forward");

	m_actionDescriptors[ruFUSION_MIME_SNEAK_BACKWARD].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_SNEAK_BACKWARD].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_SNEAK_BACKWARD].AddMotion("sneak_back");

	m_actionDescriptors[ruFUSION_MIME_WALKSP_FORWARD].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_WALKSP_FORWARD].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_WALKSP_FORWARD].AddMotion("walksp_forward");

	m_actionDescriptors[ruFUSION_MIME_RUNSP_FORWARD].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_RUNSP_FORWARD].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_RUNSP_FORWARD].AddMotion("runsp_forward");

	m_actionDescriptors[ruFUSION_MIME_RUNSP_BACKWARD].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_RUNSP_BACKWARD].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_RUNSP_BACKWARD].AddMotion("runsp_backward");

	m_actionDescriptors[ruFUSION_MIME_SWIM_FORWARD].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_SWIM_FORWARD].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_SWIM_FORWARD].AddMotion("swim_forward");

	m_actionDescriptors[ruFUSION_MIME_SWIM_BACKWARD].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_SWIM_BACKWARD].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_SWIM_BACKWARD].AddMotion("swim_back");

	m_actionDescriptors[ruFUSION_MIME_SWIM_LEFT].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_SWIM_LEFT].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_SWIM_LEFT].AddMotion("swim_left");

	m_actionDescriptors[ruFUSION_MIME_SWIM_RIGHT].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_SWIM_RIGHT].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_SWIM_RIGHT].AddMotion("swim_right");

	m_actionDescriptors[ruFUSION_MIME_SWIM_IDLE].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_SWIM_IDLE].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_SWIM_IDLE].AddMotion("swim_idle");

	m_actionDescriptors[ruFUSION_MINE_HOLSTER].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MINE_HOLSTER].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MINE_HOLSTER].AddMotion("1h_weapon_holster");

	m_actionDescriptors[ruFUSION_MIME_UNHOLSTER].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_UNHOLSTER].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_UNHOLSTER].AddMotion("1h_weapon_unholster");

	m_actionDescriptors[ruFUSION_MINE_HOLSTER_2H].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MINE_HOLSTER_2H].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MINE_HOLSTER_2H].AddMotion("2h_weapon_holster");

	m_actionDescriptors[ruFUSION_MIME_UNHOLSTER_2H].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_UNHOLSTER_2H].m_actionLayer = 1;
	m_actionDescriptors[ruFUSION_MIME_UNHOLSTER_2H].AddMotion("2h_weapon_unholster");

	m_actionDescriptors[ruFUSION_MIME_SHUFFLE_LEFT].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_SHUFFLE_LEFT].AddMotion("shuffle_left");

	m_actionDescriptors[ruFUSION_MIME_SHUFFLE_RIGHT].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_SHUFFLE_RIGHT].AddMotion("shuffle_right");

	m_actionDescriptors[ruFUSION_MIME_RIDE_IDLE].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_RIDE_IDLE].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_RIDE_IDLE].AddMotion("ride_idle01");

	m_actionDescriptors[ruFUSION_MIME_RIDE_IDLE0].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_RIDE_IDLE0].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_RIDE_IDLE0].AddMotion("ride_idle02");

	m_actionDescriptors[ruFUSION_MIME_RIDE_IDLE1].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_RIDE_IDLE1].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_RIDE_IDLE1].AddMotion("ride_idle03");

	m_actionDescriptors[ruFUSION_MIME_RIDE_IDLE2].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_RIDE_IDLE2].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_RIDE_IDLE2].AddMotion("ride_idle04");

	m_actionDescriptors[ruFUSION_MIME_RIDE_IDLE3].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_RIDE_IDLE3].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_RIDE_IDLE3].AddMotion("ride_idle05");

	m_actionDescriptors[ruFUSION_MIME_KNOCKDOWN_BEGIN].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_KNOCKDOWN_BEGIN].AddMotion("knockdown_start");

	m_actionDescriptors[ruFUSION_MIME_KNOCKDOWN_LOOP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_KNOCKDOWN_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_KNOCKDOWN_LOOP].AddMotion("knockdown_idle");

	m_actionDescriptors[ruFUSION_MIME_KNOCKDOWN_END].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_KNOCKDOWN_END].AddMotion("knockdown_end");

	m_actionDescriptors[ruFUSION_MIME_KNOCKDOWN_RECOVER].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_KNOCKDOWN_RECOVER].AddMotion("knockdown_recover");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_HANDSTAND].m_actionPriority = 30;
	//m_actionDescriptors[ruFUSION_MIME_EMOTE_HANDSTAND].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_HANDSTAND].AddMotion("emote_handstand");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_ROFL].m_actionPriority = 30;
	//m_actionDescriptors[ruFUSION_MIME_EMOTE_ROFL].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_ROFL].AddMotion("emote_rofl");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_SWEEP1].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_SWEEP1].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_SWEEP1].AddMotion("emote_sweep01");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_SWEEP2].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_SWEEP2].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_SWEEP2].AddMotion("emote_sweep02");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_SWEEP3].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_SWEEP3].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_SWEEP3].AddMotion("emote_sweep03");

	/************************************************************************/
	/* EMOTES                                                               */
	/************************************************************************/

	m_actionDescriptors[ruFUSION_MIME_EMOTE_ANGRY].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_ANGRY].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_ANGRY].AddMotion("emote_angry");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_APPLAUSE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_APPLAUSE].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_APPLAUSE].AddMotion("emote_applause");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_APPROVAL].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_APPROVAL].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_APPROVAL].AddMotion("emote_approval");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_BEG].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_BEG].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_BEG].AddMotion("emote_beg");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_CRY].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_CRY].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_CRY].AddMotion("emote_cry");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_HEAD_NOD].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_HEAD_NOD].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_HEAD_NOD].AddMotion("emote_head_nod");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_HEAD_SHAKE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_HEAD_SHAKE].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_HEAD_SHAKE].AddMotion("emote_head_shake");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_LAUGH].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_LAUGH].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_LAUGH].AddMotion("emote_laugh");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_POINT].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_POINT].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_POINT].AddMotion("emote_point");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_PROVOKE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_PROVOKE].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_PROVOKE].AddMotion("emote_provoke");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_SALUTE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_SALUTE].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_SALUTE].AddMotion("emote_salute");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_SPEAK].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_SPEAK].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_SPEAK].AddMotion("emote_speak01");
	m_actionDescriptors[ruFUSION_MIME_EMOTE_SPEAK].AddMotion("emote_speak02");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_SURRENDER].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_SURRENDER].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_SURRENDER].AddMotion("emote_surrender");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_THREATEN].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_THREATEN].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_THREATEN].AddMotion("emote_threaten");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_VICTORY].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_VICTORY].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_VICTORY].AddMotion("emote_victory");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_WAVE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_WAVE].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_WAVE].AddMotion("emote_wave");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_EAT].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_EAT].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_EAT].AddMotion("eat");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_DRINK].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_DRINK].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_DRINK].AddMotion("drink");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_IDLE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_IDLE].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_IDLE].AddMotion("idle02");
	m_actionDescriptors[ruFUSION_MIME_EMOTE_IDLE].AddMotion("idle03");
	m_actionDescriptors[ruFUSION_MIME_EMOTE_IDLE].AddMotion("idle04");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_IDLE0].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_IDLE0].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_IDLE0].AddMotion("idle02");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_IDLE1].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_IDLE1].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_IDLE1].AddMotion("idle03");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_IDLE2].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_IDLE2].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_IDLE2].AddMotion("idle04");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_DANCE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_DANCE].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
//	m_actionDescriptors[ruFUSION_MIME_EMOTE_DANCE].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_DANCE].AddMotion("emote_dance01");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_PLAY_SAXOPHONE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_PLAY_SAXOPHONE].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_PLAY_SAXOPHONE].AddMotion("emote_play_saxophone");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_PLAY_TAMBOURINE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_PLAY_TAMBOURINE].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_PLAY_TAMBOURINE].AddMotion("emote_play_tambourine");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_PLAY_GUITAR].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_PLAY_GUITAR].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_PLAY_GUITAR].AddMotion("emote_play_guitar");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_JUMP_INPLACE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_JUMP_INPLACE].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_JUMP_INPLACE].AddMotion("jump_inplace");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_BELLY].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_BELLY].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_BELLY].AddMotion("emote_belly");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_EMBRAASSED].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_EMBRAASSED].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_EMBRAASSED].AddMotion("emote_embraassed");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_EMBRACE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_EMBRACE].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_EMBRACE].AddMotion("emote_embrace");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_KISS].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_KISS].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_KISS].AddMotion("emote_kiss");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_SALUTE2].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_SALUTE2].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_SALUTE2].AddMotion("emote_salute02");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_CUTE1].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_CUTE1].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_CUTE1].AddMotion("emote_cute01");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_CUTE2].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_CUTE2].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_CUTE2].AddMotion("emote_cute02");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_CUTE3].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_CUTE3].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_CUTE3].AddMotion("emote_cute03");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_HANGOVER].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_HANGOVER].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_HANGOVER].AddMotion("hangover");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_STRETCH].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_STRETCH].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_STRETCH].AddMotion("emote_stretch");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_ROCK].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_ROCK].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_ROCK].AddMotion("emote_rock");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_KAMEHAMEHA].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_KAMEHAMEHA].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_KAMEHAMEHA].AddMotion("emote_kamehameha");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_KICK].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_KICK].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_KICK].AddMotion("emote_kick");	

	m_actionDescriptors[ruFUSION_MIME_EMOTE_BREAKDANCE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_BREAKDANCE].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_BREAKDANCE].AddMotion("emote_breakdance");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_TAPDANCE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_TAPDANCE].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_TAPDANCE].AddMotion("emote_tapdance");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_TIRED].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_TIRED].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_TIRED].AddMotion("emote_tired");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_FEAR].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_FEAR].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_FEAR].AddMotion("emote_fear");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_AMAZED].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_AMAZED].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_AMAZED].AddMotion("emote_amazed");

	m_actionDescriptors[ruFUSION_MIME_EMOTE_THINK].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_THINK].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_EMOTE_THINK].AddMotion("emote_think");

	/************************************************************************/
	/* PERSISTENT STATES                                                    */
	/************************************************************************/

	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWN].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWN].m_actionOverrides = TRUE;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWN].AddMotion("jump_down");

	m_actionDescriptors[ruFUSION_MIME_JUMP_LOOP].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_JUMP_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_JUMP_LOOP].AddMotion("jump_loop");

	m_actionDescriptors[ruFUSION_MIME_JUMP_UP].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_JUMP_UP].m_actionType = ruFUSION_MIME_ACTIONTYPE_LOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_JUMP_UP].AddMotion("jump_up");

	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDRUN].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDRUN].m_actionType = ruFUSION_MIME_ACTIONTYPE_LOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDRUN].m_actionOverrides = TRUE;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDRUN].m_actionMaxBlendDelay = 0.0f;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDRUN].m_chainableAction = ruFUSION_MIME_RUN_FORWARD;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDRUN].AddMotion("jump_end_run");

	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDRUN_2].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDRUN_2].m_actionType = ruFUSION_MIME_ACTIONTYPE_LOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDRUN_2].m_actionOverrides = FALSE;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDRUN_2].m_actionMaxBlendDelay = 0.0f;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDRUN_2].m_chainableAction = ruFUSION_MIME_RUN_FORWARD;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDRUN_2].AddMotion("jump_end_run");

	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDBACK].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDBACK].m_actionType = ruFUSION_MIME_ACTIONTYPE_LOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDBACK].m_actionOverrides = TRUE;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDBACK].m_actionMaxBlendDelay = 0.0f;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDBACK].m_chainableAction = ruFUSION_MIME_RUN_BACKWARD;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDBACK].AddMotion("jump_end_back");

	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDBACK_2].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDBACK_2].m_actionType = ruFUSION_MIME_ACTIONTYPE_LOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDBACK_2].m_actionOverrides = FALSE;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDBACK_2].m_actionMaxBlendDelay = 0.0f;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDBACK_2].m_chainableAction = ruFUSION_MIME_RUN_BACKWARD;
	m_actionDescriptors[ruFUSION_MIME_JUMP_DOWNANDBACK_2].AddMotion("jump_end_back");

	m_actionDescriptors[ruFUSION_MIME_JUMP_INPLACE].m_actionPriority = 10;
	m_actionDescriptors[ruFUSION_MIME_JUMP_INPLACE].m_actionType = ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY;
	m_actionDescriptors[ruFUSION_MIME_JUMP_INPLACE].m_actionOverrides = TRUE;
	m_actionDescriptors[ruFUSION_MIME_JUMP_INPLACE].AddMotion("jump_inplace");

	m_actionDescriptors[ruFUSION_MIME_SLEEP_DOWN].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_SLEEP_DOWN].AddMotion("emote_sleep_down");

	m_actionDescriptors[ruFUSION_MIME_SLEEP_LOOP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_SLEEP_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_SLEEP_LOOP].AddMotion("emote_sleep_idle");

	m_actionDescriptors[ruFUSION_MIME_SLEEP_UP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_SLEEP_UP].AddMotion("emote_sleep_up");

	m_actionDescriptors[ruFUSION_MIME_SIT_DOWN].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_SIT_DOWN].AddMotion("sit");

	m_actionDescriptors[ruFUSION_MIME_SIT_LOOP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_SIT_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_SIT_LOOP].AddMotion("sit_idle");

	m_actionDescriptors[ruFUSION_MIME_SIT_UP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_SIT_UP].AddMotion("stand_up");

	m_actionDescriptors[ruFUSION_MIME_DEATH_DOWN].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_DEATH_DOWN].AddMotion("death");

	m_actionDescriptors[ruFUSION_MIME_DEATH_LOOP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_DEATH_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_DEATH_LOOP].AddMotion("dead");

	m_actionDescriptors[ruFUSION_MIME_DEATH_UP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_DEATH_UP].AddMotion("revive");

	m_actionDescriptors[ruFUSION_MIME_CROUCH_BEGIN].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_CROUCH_BEGIN].AddMotion("crouch_down");

	m_actionDescriptors[ruFUSION_MIME_CROUCH_LOOP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_CROUCH_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_CROUCH_LOOP].AddMotion("crouch_idle");

	m_actionDescriptors[ruFUSION_MIME_CROUCH_END].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_CROUCH_END].AddMotion("crouch_up");

	m_actionDescriptors[ruFUSION_MIME_CRAFTING_BEGIN].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_CRAFTING_BEGIN].AddMotion("crafting_start");

	m_actionDescriptors[ruFUSION_MIME_CRAFTING_LOOP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_CRAFTING_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_CRAFTING_LOOP].AddMotion("crafting_idle");

	m_actionDescriptors[ruFUSION_MIME_CRAFTING_END].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_CRAFTING_END].AddMotion("crafting_end");

	m_actionDescriptors[ruFUSION_MIME_MINING_BEGIN].m_actionPriority = 30;
//	m_actionDescriptors[ruFUSION_MIME_MINING_BEGIN].AddMotion("crafting_start");

	m_actionDescriptors[ruFUSION_MIME_MINING_LOOP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_MINING_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_MINING_LOOP].AddMotion("mining_idle");

	m_actionDescriptors[ruFUSION_MIME_MINING_END].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_MINING_END].AddMotion("crafting_end");

	m_actionDescriptors[ruFUSION_MIME_LUMBERJACK_BEGIN].m_actionPriority = 30;

	m_actionDescriptors[ruFUSION_MIME_LUMBERJACK_LOOP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_LUMBERJACK_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_LUMBERJACK_LOOP].AddMotion("lumberjack");

	m_actionDescriptors[ruFUSION_MIME_LUMBERJACK_END].m_actionPriority = 30;

	m_actionDescriptors[ruFUSION_MIME_GATHER_BEGIN].m_actionPriority = 30;

	m_actionDescriptors[ruFUSION_MIME_GATHER_LOOP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_GATHER_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_GATHER_LOOP].AddMotion("gather");

	m_actionDescriptors[ruFUSION_MIME_GATHER_END].m_actionPriority = 30;

	m_actionDescriptors[ruFUSION_MIME_LOOT_BEGIN].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_LOOT_BEGIN].AddMotion("loot_start");

	m_actionDescriptors[ruFUSION_MIME_LOOT_LOOP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_LOOT_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_LOOT_LOOP].AddMotion("loot_idle");

	m_actionDescriptors[ruFUSION_MIME_LOOT_END].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_LOOT_END].AddMotion("loot_end");

	m_actionDescriptors[ruFUSION_MIME_CARRY_BEGIN].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_CARRY_BEGIN].AddMotion("carry_begin");

	m_actionDescriptors[ruFUSION_MIME_CARRY_LOOP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_CARRY_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_CARRY_LOOP].AddMotion("carry_loop");

	m_actionDescriptors[ruFUSION_MIME_CARRY_END].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_CARRY_END].AddMotion("carry_end");

	m_actionDescriptors[ruFUSION_MIME_DAZED].m_actionPriority = 0;
	m_actionDescriptors[ruFUSION_MIME_DAZED].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_DAZED].AddMotion("dazed");

	m_actionDescriptors[ruFUSION_MIME_SLUMBER].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_SLUMBER].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_SLUMBER].AddMotion("slumber");

	m_actionDescriptors[ruFUSION_MIME_ACTIVATE_BEGIN].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_ACTIVATE_BEGIN].AddMotion("activate_begin");

	m_actionDescriptors[ruFUSION_MIME_ACTIVATE_LOOP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_ACTIVATE_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_ACTIVATE_LOOP].AddMotion("activate_loop");

	m_actionDescriptors[ruFUSION_MIME_ACTIVATE_END].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_ACTIVATE_END].AddMotion("activate_end");

	m_actionDescriptors[ruFUSION_MIME_READING_BEGIN].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_READING_BEGIN].AddMotion("reading_start");

	m_actionDescriptors[ruFUSION_MIME_READING_LOOP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_READING_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_READING_LOOP].AddMotion("reading_idle");

	m_actionDescriptors[ruFUSION_MIME_READING_END].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_READING_END].AddMotion("reading_end");

	m_actionDescriptors[ruFUSION_MIME_SIT_CHAIR_BEGIN].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_SIT_CHAIR_BEGIN].AddMotion("sit_chair_begin"); 

	m_actionDescriptors[ruFUSION_MIME_SIT_CHAIR_LOOP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_SIT_CHAIR_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_SIT_CHAIR_LOOP].AddMotion("sit_chair_idle01");

	m_actionDescriptors[ruFUSION_MIME_SIT_CHAIR_END].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_SIT_CHAIR_BEGIN].AddMotion("sit_chair_end");

	m_actionDescriptors[ruFUSION_MIME_IDLE_CROUCH].m_actionPriority = 0;
	m_actionDescriptors[ruFUSION_MIME_IDLE_CROUCH].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_IDLE_CROUCH].m_actionOverrides = TRUE;
	m_actionDescriptors[ruFUSION_MIME_IDLE_CROUCH].AddMotion("crouch_idle", 10.0f);

	m_actionDescriptors[ruFUSION_MIME_IDLE_DEAD].m_actionPriority = 0;
	m_actionDescriptors[ruFUSION_MIME_IDLE_DEAD].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_IDLE_DEAD].m_actionOverrides = TRUE;
	m_actionDescriptors[ruFUSION_MIME_IDLE_DEAD].AddMotion("dead", 10.0f);

	m_actionDescriptors[ruFUSION_MIME_IDLE_SIT].m_actionPriority = 0;
	m_actionDescriptors[ruFUSION_MIME_IDLE_SIT].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_IDLE_SIT].m_actionOverrides = TRUE;
	m_actionDescriptors[ruFUSION_MIME_IDLE_SIT].AddMotion("sit_idle", 10.0f);

	m_actionDescriptors[ruFUSION_MIME_IDLE_SLEEP].m_actionPriority = 0;
	m_actionDescriptors[ruFUSION_MIME_IDLE_SLEEP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_IDLE_SLEEP].m_actionOverrides = TRUE;
	m_actionDescriptors[ruFUSION_MIME_IDLE_SLEEP].AddMotion("emote_sleep_idle", 10.0f);

	m_actionDescriptors[ruFUSION_MIME_SPIRAL_RIGHT].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_SPIRAL_RIGHT].AddMotion("spiral_right");

	m_actionDescriptors[ruFUSION_MIME_SPIRAL_LEFT].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_SPIRAL_LEFT].AddMotion("spiral_left");

	m_actionDescriptors[ruFUSION_MIME_SPIRAL_LOOP].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_SPIRAL_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_SPIRAL_LOOP].AddMotion("spiral_loop");

	m_actionDescriptors[ruFUSION_MIME_FLY_BEGIN].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_FLY_BEGIN].AddMotion("fly_begin");
	m_actionDescriptors[ruFUSION_MIME_FLY_END].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_FLY_END].AddMotion("fly_end");
	m_actionDescriptors[ruFUSION_MIME_FLY_LOOP].m_actionPriority = 20;
	m_actionDescriptors[ruFUSION_MIME_FLY_LOOP].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_FLY_LOOP].AddMotion("fly_loop");

	m_actionDescriptors[ruFUSION_MIME_DEAD_WATER].m_actionPriority = 30;
	m_actionDescriptors[ruFUSION_MIME_DEAD_WATER].m_actionType = ruFUSION_MIME_ACTIONTYPE_IDLELOOP;
	m_actionDescriptors[ruFUSION_MIME_DEAD_WATER].AddMotion("dead_water");

	// Bone rotator setup
	InitializeStandardBoneRotators();

	// Bone scaler setup
	InitializeStandardBoneScalers();
}

CRuFusion_Mime_Template::~CRuFusion_Mime_Template()
{
	delete[] m_actionDescriptors;

	for(INT32 i = 0; i < m_bodyBones.Count(); ++i)
	{
		ruSAFE_DELETE_ARRAY(m_bodyBones[i].m_boneName);
	}

	for(INT32 i = 0; i < m_torsoBones.Count(); ++i)
	{
		ruSAFE_DELETE_ARRAY(m_torsoBones[i].m_boneName);
	}

	for(INT32 i = 0; i < m_boneScalers.Count(); ++i)
	{
		delete m_boneScalers[i];
	}
}

BOOL CRuFusion_Mime_Template::LoadSettings(IRuStream *settingsStream)
{
	// Allocate buffer large enough to contain the entire settings stream
	DWORD bufferSize = settingsStream->GetStreamSize() - settingsStream->GetPosition();
	char *settingsBuffer = ruNEW char [bufferSize];

	// Read entire settings stream into memory
	settingsStream->Read(settingsBuffer, bufferSize);

	// Tokenize based on carriage-return and line-feed
	CRuStringTokenizer strTokenizer;
	strTokenizer.Tokenize(settingsBuffer, "\n\r");

	// Process each token
	for(INT32 i = 0; i < strTokenizer.GetNumTokens(); ++i)
	{
		// Get token and process
		CRuString token = strTokenizer.GetToken(i);
		if(token.GetLength() > 0)
		{
			// Tokenize based on space and tab
			CRuStringTokenizer lineTokenizer;
			lineTokenizer.Tokenize(token, " \t");

			// Read action string

			// Read action priority

			// Read motion string until no tokens remain
		}
	}

	// Release resources
	delete[] settingsBuffer;

	return TRUE;
}

BOOL CRuFusion_Mime_Template::SaveSettings(IRuStream *settingsStream)
{
	return TRUE;
}

INT32 CRuFusion_Mime_Template::GetActionPriority(CRuFusion_Mime_Action action)
{
	return m_actionDescriptors[action].m_actionPriority;
}

INT32 CRuFusion_Mime_Template::GetActionLayer(CRuFusion_Mime_Action action)
{
	return m_actionDescriptors[action].m_actionLayer;
}

CRuFusion_Mime_ActionType CRuFusion_Mime_Template::GetActionType(CRuFusion_Mime_Action action)
{
	return m_actionDescriptors[action].m_actionType;
}

BOOL CRuFusion_Mime_Template::GetActionOverrides(CRuFusion_Mime_Action action)
{
	return m_actionDescriptors[action].m_actionOverrides;
}

REAL CRuFusion_Mime_Template::GetActionMaxBlendDelay(CRuFusion_Mime_Action action)
{
	return m_actionDescriptors[action].m_actionMaxBlendDelay;
}

CRuFusion_Mime_Action CRuFusion_Mime_Template::GetChainableAction(CRuFusion_Mime_Action action)
{
	return m_actionDescriptors[action].m_chainableAction;
}

const char *CRuFusion_Mime_Template::GetMotion(CRuFusion_Mime_Action action)
{
	if(m_actionDescriptors[action].GetNumMotions() > 0)
	{
		// Generate the motion index to use
		INT32 motionIndex = m_randGen.GenerateUINT32() % m_actionDescriptors[action].GetNumMotions();

		// Return the motion name
		return m_actionDescriptors[action].GetMotion(motionIndex);
	}

	return "";
}

INT32 CRuFusion_Mime_Template::GetNumBodyBones() const
{
	return m_bodyBones.Count();
}

const char *CRuFusion_Mime_Template::GetBodyBoneName(INT32 index) const
{
	if(m_bodyBones[index].m_boneName)
	{
		return m_bodyBones[index].m_boneName;
	}

	return "";
}

REAL CRuFusion_Mime_Template::GetBodyBoneWeight(INT32 index) const
{
	return m_bodyBones[index].m_weight;
}

INT32 CRuFusion_Mime_Template::GetNumTorsoBones() const
{
	return m_torsoBones.Count();
}

const char *CRuFusion_Mime_Template::GetTorsoBoneName(INT32 index) const
{
	if(m_torsoBones[index].m_boneName)
	{
		return m_torsoBones[index].m_boneName;
	}

	return "";
}

REAL CRuFusion_Mime_Template::GetTorsoBoneWeight(INT32 index) const
{
	return m_torsoBones[index].m_weight;
}

INT32 CRuFusion_Mime_Template::GetNumBoneScalers() const
{
	return m_boneScalers.Count();
}

CRuFusion_Mime_BoneScaler *CRuFusion_Mime_Template::GetBoneScaler(INT32 index)
{
	return m_boneScalers[index];
}

void CRuFusion_Mime_Template::InitializeStandardBoneRotators()
{
	m_bodyBones.Add(CRuFusion_Mime_Bone_R("Bip01", 1.0f));

	m_torsoBones.Add(CRuFusion_Mime_Bone_R("Bip01 Spine1", 0.25f));
	m_torsoBones.Add(CRuFusion_Mime_Bone_R("Bip01 Spine2", 0.25f));
	m_torsoBones.Add(CRuFusion_Mime_Bone_R("Bip01 Neck", 0.25f));
	m_torsoBones.Add(CRuFusion_Mime_Bone_R("Bip01 Head", 0.25f));
}

void CRuFusion_Mime_Template::InitializeDeprecatedBoneRotators()
{
	m_bodyBones.Add(CRuFusion_Mime_Bone_R("Bip01", 1.0f));

	m_torsoBones.Add(CRuFusion_Mime_Bone_R("Bip01 Spine1", 0.25f));
	m_torsoBones.Add(CRuFusion_Mime_Bone_R("Bip01 Neck", 0.30f));
	m_torsoBones.Add(CRuFusion_Mime_Bone_R("Bip01 Head", 0.30f));
}

void CRuFusion_Mime_Template::InitializeStandardBoneScalers()
{
	for(INT32 i = 0; i < m_boneScalers.Count(); ++i)
	{
		delete m_boneScalers[i];
	}
	m_boneScalers.Clear();

	CRuFusion_Mime_BoneScaler *boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Head Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 Head");
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Chest Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 Spine2");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 Neck"));
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("p_back_shield"));
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("p_back_staff"));
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("p_back_sword"));
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("p_back_gun"));
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("p_back_bow"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Butt Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 Pelvis");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 Spine2"));
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 L Thigh"));
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 R Thigh"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Right Arm Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 R UpperArm");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 R Forearm"));
//	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 R Hand"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Left Arm Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 L UpperArm");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 L Forearm"));
//	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 L Hand"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Right Forearm Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 R Forearm");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 R Hand"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Left Forearm Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 L Forearm");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 L Hand"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Right Hand Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 R Hand");
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Left Hand Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 L Hand");
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Right Leg Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 R Thigh");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 R Calf"));
//	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 R Foot"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Left Leg Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 L Thigh");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 L Calf"));
//	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 L Foot"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Right Calf Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 R Calf");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 R Foot"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Left Calf Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 L Calf");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 L Foot"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Right Foot Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 R Foot");
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Left Foot Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 L Foot");
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Right Breast Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("bone_breast_r");
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Left Breast Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("bone_breast_l");
	m_boneScalers.Add(boneScaler);
}

void CRuFusion_Mime_Template::InitializeDeprecatedBoneScalers()
{
	for(INT32 i = 0; i < m_boneScalers.Count(); ++i)
	{
		delete m_boneScalers[i];
	}
	m_boneScalers.Clear();

	CRuFusion_Mime_BoneScaler *boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Head Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 Head");
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Chest Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 Spine1");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 Neck"));
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("p_back_shield"));
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("p_back_staff"));
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("p_back_sword"));
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("p_back_gun"));
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("p_back_bow"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Butt Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 Pelvis");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 Spine1"));
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 L Thigh"));
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 R Thigh"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Right Arm Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 R UpperArm");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 R Forearm"));
//	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 R Hand"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Left Arm Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 L UpperArm");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 L Forearm"));
//	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 L Hand"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Right Forearm Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 R Forearm");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 R Hand"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Left Forearm Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 L Forearm");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 L Hand"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Right Hand Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 R Hand");
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Left Hand Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 L Hand");
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Right Leg Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 R Thigh");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 R Calf"));
//	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 R Foot"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Left Leg Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 L Thigh");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 L Calf"));
//	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 L Foot"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Right Calf Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 R Calf");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 R Foot"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Left Calf Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 L Calf");
	boneScaler->m_terminatorBoneNames.Add(ruNEW CRuString("Bip01 L Foot"));
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Right Foot Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 R Foot");
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Left Foot Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("Bip01 L Foot");
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Right Breast Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("bone_breast_r");
	m_boneScalers.Add(boneScaler);

	boneScaler = ruNEW CRuFusion_Mime_BoneScaler();
	boneScaler->m_boneScalerName = ruNEW CRuString("Left Breast Scale");
	boneScaler->m_targetBoneName = ruNEW CRuString("bone_breast_l");
	m_boneScalers.Add(boneScaler);
}

// ************************************************************************************************************************************************************

CRuFusion_Mime::CRuFusion_Mime()
:	m_mimeTemplate(NULL),
	m_defaultChannel(ruACTMOTIONCHANNEL_DEFAULT),
	m_auxiliaryChannel(ruACTMOTIONCHANNEL_BODY_FULL),
	m_backgroundChannel(ruACTMOTIONCHANNEL_BODY_LOWER),
	m_foregroundChannel(ruACTMOTIONCHANNEL_BODY_UPPER),
	m_userChannel(ruACTMOTIONCHANNEL_BODY_AUXILIARY),
	m_blendDelay(0.20f),
	m_targetEntity(NULL),
	m_targetACT(NULL),
	m_targetHierarchy(NULL),
	m_targetEntitySupportsChannels(TRUE),
	m_channels(NULL),
	m_queuedActions(NULL),
	m_body_YawOffset(0.0f),
	m_body_HeadingOffset(0.0f),
	m_body_BoneController(NULL),
	m_torso_HeadingOffset(0.0f),
	m_torso_BoneController(NULL),
	m_bodyScale(1.0f),
	m_defaultForwardSpeed(61.0f),
	m_forwardSpeed(33.0f),
	m_defaultBackwardSpeed(61.0f),
	m_backwardSpeed(33.0f)
{
	// Setup action channels
	m_channels = ruNEW ChannelDesc [ruFUSION_MIME_CHANNEL_COUNT];

	m_channels[ruFUSION_MIME_CHANNEL_FOREGROUND].m_motionChannel = ruACTMOTIONCHANNEL_BODY_UPPER;
	m_channels[ruFUSION_MIME_CHANNEL_FOREGROUND].m_activeAction = ruFUSION_MIME_NONE;
	m_channels[ruFUSION_MIME_CHANNEL_FOREGROUND].m_exclusiveAction = FALSE;
	m_channels[ruFUSION_MIME_CHANNEL_FOREGROUND].m_motionName = NULL;
	m_channels[ruFUSION_MIME_CHANNEL_FOREGROUND].m_motionIsInterruptible = FALSE;
	m_channels[ruFUSION_MIME_CHANNEL_FOREGROUND].m_motionJustDequeued = FALSE;
	m_channels[ruFUSION_MIME_CHANNEL_FOREGROUND].m_actionEvents = NULL;

	m_channels[ruFUSION_MIME_CHANNEL_BACKGROUND].m_motionChannel = ruACTMOTIONCHANNEL_BODY_LOWER;
	m_channels[ruFUSION_MIME_CHANNEL_BACKGROUND].m_activeAction = ruFUSION_MIME_NONE;
	m_channels[ruFUSION_MIME_CHANNEL_BACKGROUND].m_exclusiveAction = FALSE;
	m_channels[ruFUSION_MIME_CHANNEL_BACKGROUND].m_motionName = NULL;
	m_channels[ruFUSION_MIME_CHANNEL_BACKGROUND].m_motionIsInterruptible = FALSE;
	m_channels[ruFUSION_MIME_CHANNEL_BACKGROUND].m_motionJustDequeued = FALSE;
	m_channels[ruFUSION_MIME_CHANNEL_BACKGROUND].m_actionEvents = NULL;

	// Create action queues
	m_queuedActions = ruNEW CRuArrayList<ActionDesc> [ruFUSION_MIME_CHANNEL_COUNT];
}

CRuFusion_Mime::~CRuFusion_Mime()
{
	ruSAFE_RELEASE(m_mimeTemplate);
	ruSAFE_RELEASE(m_targetEntity);
	ruSAFE_RELEASE(m_body_BoneController);
	ruSAFE_RELEASE(m_torso_BoneController);

	for(INT32 i = 0; i < m_boneScalerDescriptors.Count(); ++i)
	{
		ruSAFE_RELEASE(m_boneScalerDescriptors[i].m_boneScalerController);
	}

	delete[] m_channels;
	delete[] m_queuedActions;
}

CRuEvent &CRuFusion_Mime::Event_DequeueAction()
{
	return m_event_DequeueAction;
}

CRuEntity *CRuFusion_Mime::GetTarget() const
{
	return m_targetEntity;
}

BOOL CRuFusion_Mime::SetTarget(CRuEntity *targetEntity, BOOL disableCompensators)
{
	if(targetEntity)
	{
		// Reset pointers
		m_targetACT = NULL;
		m_targetHierarchy = NULL;

		// Destroy old controllers
		ruSAFE_RELEASE(m_body_BoneController);
		ruSAFE_RELEASE(m_torso_BoneController);

		// Store reference to target entity
		targetEntity->AddRef();
		ruSAFE_RELEASE(m_targetEntity);
		m_targetEntity = targetEntity;

		// Store reference to target ACT
		m_targetACT = RuEntity_FindFirstACTEntity(m_targetEntity);

		if(m_targetACT)
		{
			SetupMotionChannels();

			CRuFrameHierarchy *targetHierarchy = RuEntity_FindFirstFrameHierarchy(m_targetEntity);
			CRuHierarchy_GR2 *targetHierarchy_GR2 = RuEntity_FindFirstHierarchy_GR2(m_targetEntity);

			if(targetHierarchy)
			{
				m_targetHierarchy = targetHierarchy;

				// Create animation compensators
				m_targetACT->CreateAnimationCompensator(2, 4, 2002, 0.4f);
				m_targetACT->CreateAnimationCompensator(2, 4, 2003, 0.3f);
			}
			else if (targetHierarchy_GR2)
			{
				m_targetHierarchy = targetHierarchy_GR2;

				if(disableCompensators == FALSE)
				{
					// Create motion compensators on the hierarchy
					// NOTE: Compensators are used for compensating any deviations in motion bone orientation when 2 or more motions are playing!
					INT32 torsoBone0 = targetHierarchy_GR2->GetSubNodeIndexByName("Bip01 Spine1");
					INT32 torsoBone1 = targetHierarchy_GR2->GetSubNodeIndexByName("Bip01 Spine2");
					INT32 torsoBone2 = targetHierarchy_GR2->GetSubNodeIndexByName("Bip01 Neck");
					INT32 headBone = targetHierarchy_GR2->GetSubNodeIndexByName("Bip01 Head");

					if(torsoBone0 >= 0 && torsoBone1 >= 0 && torsoBone2 >= 0)
					{
						targetHierarchy_GR2->AddMotionCompensator(0x00000004, torsoBone0, 0.15f);
						targetHierarchy_GR2->AddMotionCompensator(0x00000004, torsoBone1, 0.15f);
						targetHierarchy_GR2->AddMotionCompensator(0x00000004, torsoBone2, 0.20f);
					}
					else if(torsoBone0 >= 0 && torsoBone1 >= 0)
					{
						targetHierarchy_GR2->AddMotionCompensator(0x00000004, torsoBone0, 0.25f);
						targetHierarchy_GR2->AddMotionCompensator(0x00000004, torsoBone1, 0.15f);
					}

					if(headBone >= 0)
					{
						targetHierarchy_GR2->AddMotionCompensator(0x00000004, headBone, 0.9f);
					}
				}
			}

			// Initialize bone controllers
			InitializeBoneControllers(m_targetHierarchy);

			// Determine whether the entity supports independent motion channels
			if(RuACT_SupportsMotionChannel(m_targetACT, m_backgroundChannel) == FALSE)
			{
				m_targetEntitySupportsChannels = FALSE;
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuFusion_Mime::SetupMotionChannels()
{
	CRuFrameHierarchy *targetHierarchy = RuEntity_FindFirstFrameHierarchy(m_targetEntity);
	CRuHierarchy_GR2 *targetHierarchy_GR2 = RuEntity_FindFirstHierarchy_GR2(m_targetEntity);

	if(targetHierarchy)
	{
	}
	else if (targetHierarchy_GR2)
	{
		// test code : this sets channel masks programmatically... not quite finished
		targetHierarchy_GR2->SetSubNodeChannelMaskFromBone("bone_Root", ruACTMOTIONCHANNEL_BODY_LOWER, TRUE);
		targetHierarchy_GR2->SetSubNodeChannelMaskFromBone("root_Bone", ruACTMOTIONCHANNEL_BODY_LOWER, TRUE);
		targetHierarchy_GR2->SetSubNodeChannelMaskFromBone("root_Bone01", ruACTMOTIONCHANNEL_BODY_LOWER, TRUE);
		targetHierarchy_GR2->SetSubNodeChannelMaskFromBone("Bip01", ruACTMOTIONCHANNEL_BODY_LOWER, TRUE);
		targetHierarchy_GR2->SetSubNodeChannelMaskFromBone("Bip01 Spine1", ruACTMOTIONCHANNEL_BODY_UPPER, TRUE);
		targetHierarchy_GR2->SetSubNodeChannelMaskFromBone("Bip01 Head", ruACTMOTIONCHANNEL_BODY_HEAD, TRUE);

		targetHierarchy_GR2->SetSubNodeChannelMaskFromBone("bone_Root", ruACTMOTIONCHANNEL_BODY_FULL, FALSE);
		targetHierarchy_GR2->SetSubNodeChannelMaskFromBone("root_Bone", ruACTMOTIONCHANNEL_BODY_FULL, FALSE);
		targetHierarchy_GR2->SetSubNodeChannelMaskFromBone("root_Bone01", ruACTMOTIONCHANNEL_BODY_FULL, FALSE);
		targetHierarchy_GR2->SetSubNodeChannelMaskFromBone("Bip01", ruACTMOTIONCHANNEL_BODY_FULL, FALSE);
	}

	return TRUE;
}

void CRuFusion_Mime::InitializeBoneControllers(IRuObject *hierarchyObj)
{
	// Destroy existing bone controlelrs
	if(m_body_BoneController)
	{
		m_body_BoneController->DetachFromParent();
		ruSAFE_RELEASE(m_body_BoneController);
	}

	if(m_torso_BoneController)
	{
		m_torso_BoneController->DetachFromParent();
		ruSAFE_RELEASE(m_torso_BoneController);
	}

	if(hierarchyObj)
	{
		// Create new bone controllers
		if(hierarchyObj->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
		{
			CRuHierarchy_GR2 *hierarchy = static_cast<CRuHierarchy_GR2 *>(hierarchyObj);

			// Create bone controller
			m_body_BoneController = ruNEW CRuController_Bone();
			m_body_BoneController->SetTarget(hierarchy);
			m_body_BoneController->SetBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));

			// Create bone controller
			m_torso_BoneController = ruNEW CRuController_Bone();
			m_torso_BoneController->SetTarget(hierarchy);
			m_torso_BoneController->SetBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));

			// Attach bone controllers to the ACT
			m_targetACT->AddEntity(m_body_BoneController);
			m_targetACT->AddEntity(m_torso_BoneController);
		}
		else if(hierarchyObj->GetType().IsSubClassOf(CRuFrameHierarchy::Type()))
		{
			CRuFrameHierarchy *hierarchy = static_cast<CRuFrameHierarchy *>(hierarchyObj);

			// Create bone controller
			m_body_BoneController = ruNEW CRuController_Bone();
			m_body_BoneController->SetTarget(hierarchy);
			m_body_BoneController->SetBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));

			// Create bone controller
			m_torso_BoneController = ruNEW CRuController_Bone();
			m_torso_BoneController->SetTarget(hierarchy);

			// Attach bone controllers to the ACT
			m_targetACT->AddEntity(m_body_BoneController);
			m_targetACT->AddEntity(m_torso_BoneController);
		}
	}
}

BOOL CRuFusion_Mime::SetMimeTemplate(CRuFusion_Mime_Template *mimeTemplate)
{
	if(mimeTemplate)
	{
		// Store reference to new template
		mimeTemplate->AddRef();
		ruSAFE_RELEASE(m_mimeTemplate);
		m_mimeTemplate = mimeTemplate;

		// Re-initialize bone controllers
		InitializeBoneControllers(m_targetHierarchy);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuFusion_Mime::SetActionChangedCallback(ActionChangedCallback actionChangedCallback)
{
	m_actionChangedCallback = actionChangedCallback;

	return TRUE;
}

CRuFusion_Mime_Action CRuFusion_Mime::GetAction(CRuFusion_Mime_Channel channel)
{
	return m_channels[channel].m_activeAction;
}

BOOL CRuFusion_Mime::SetAction(CRuFusion_Mime_Channel channel, CRuFusion_Mime_Action action, CRuFusion_Mime_SetActionMethod setActionMethod, PTRVALUE eventData, CRuACT_ActionEvents *actionEventsOut, BOOL setAsDefaultAction)
{
	if (action == -1)
	{
		//char buf[4096];
		//sprintf(buf, "CRuFusion_Mime::SetAction action:%d\n", action);
		//OutputDebugString(buf);
		// MessageBox( NULL, "ruFUSION_MIME_FORCE_DWORD", "ruFUSION_MIME_FORCE_DWORD", MB_OK );
		//return FALSE;
		action = ruFUSION_MIME_NONE;
	}
	// Setup shortcut references to specified channel
	ChannelDesc &chanDesc = m_channels[channel];
	CRuArrayList<ActionDesc> &actionQueue = m_queuedActions[channel];

	if	(	
			(chanDesc.m_activeAction != action && action != ruFUSION_MIME_ACTION_COUNT) ||
			(action == ruFUSION_MIME_ACTION_COUNT && chanDesc.m_activeAction != chanDesc.m_defaultAction.m_action) ||
			(setActionMethod == ruFUSION_MIME_SETACTIONMETHOD_QUEUED) ||
			(setActionMethod == ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE)
		)
	{
		// Initialize action descriptor
		ActionDesc actionDesc;

		if(action == ruFUSION_MIME_ACTION_COUNT)
		{
			// Action is specified as a default action. Use dummy data.
			actionDesc = ActionDesc(action, "", eventData, setActionMethod == ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, actionEventsOut);
		}
		else
		{
			//char buf[4096];
			//sprintf(buf, "CRuFusion_Mime::SetAction action:%d", action);
			//sprintf(buf, "%s motionName:%s\n", buf, m_mimeTemplate->GetMotion(action));
			//sprintf(buf, "CRuFusion_Mime::SetAction motionName:%s action:%d\n", m_mimeTemplate->GetMotion(action), action);
			//OutputDebugString(buf);
			// Action is a normal action, fill in data.
			actionDesc = ActionDesc(action, m_mimeTemplate->GetMotion(action), eventData, setActionMethod == ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE, actionEventsOut);
		}

		// Time-stamp the action
		actionDesc.m_actionTimeStamp = GetTickCount();

		// Extract event information if there is a target ACT
		if(m_targetACT && actionEventsOut)
		{
			actionEventsOut->InitializeActionEventsFromMotion(m_targetACT, actionDesc.m_motionName);
		}

		// Store current sequence number into action events, if available
		if(actionEventsOut)
		{
			// Store and increment sequence number
			actionEventsOut->SetProperty("mimesys:seqnumber", m_nextSeqNumber);
			m_nextSeqNumber++;
		}

		// Set as default action
		if(setAsDefaultAction)
		{
			chanDesc.m_defaultAction = actionDesc;
/*
			if(RuEntity_FindFirstPaperdoll(m_targetEntity) && channel == ruFUSION_MIME_CHANNEL_FOREGROUND)
			{
				char moo[200];
				sprintf(moo, "Default action now %d\n", actionDesc.m_action);
				OutputDebugString(moo);
			}
*/
		}

		switch(setActionMethod)
		{
			case ruFUSION_MIME_SETACTIONMETHOD_NORMAL:
				{
					for(INT32 i = 0; i < actionQueue.Count(); ++i)
					{
						if(actionQueue[i].m_action == actionDesc.m_action)
						{
							actionQueue[i].m_eventData = actionDesc.m_eventData;
							return TRUE;
						}
					}

					// Queue new action
					return QueueAction(channel, actionDesc);
				}

				break;

			case ruFUSION_MIME_SETACTIONMETHOD_QUEUED:
				{
					// Queue new action
					return QueueAction(channel, actionDesc);
				}

				break;

			case ruFUSION_MIME_SETACTIONMETHOD_EXCLUSIVE:
				{
					// Scan action queue for exclusive actions which are of higher priority than the one to be inserted
					for(INT32 i = 0; i < actionQueue.Count(); ++i)
					{
						// If the exclusive action has higher priority than the new exclusive action, the existing exclusive action wins
						if(actionQueue[i].m_exclusiveAction && m_mimeTemplate->GetActionPriority(actionQueue[i].m_action) > m_mimeTemplate->GetActionPriority(action))
						{
							return FALSE;
						}
					}

					// If channel is active and the existing exclusive action has higher priority than the new exclusive action, queue the exclusive action
					if(chanDesc.m_activeAction != ruFUSION_MIME_NONE && chanDesc.m_exclusiveAction && m_mimeTemplate->GetActionPriority(chanDesc.m_activeAction) > m_mimeTemplate->GetActionPriority(action))
					{
						// Queue new exclusive action
						return QueueAction(channel, actionDesc);
					}
					else
					{
						// Clear active action and any queued actions
						chanDesc.m_activeAction = ruFUSION_MIME_NONE;
						chanDesc.m_exclusiveAction = FALSE;
						actionQueue.Clear();

						// Queue new action
						if(QueueAction(channel, actionDesc))
						{
							// Dequeue new action immediately
							return DequeueAction(channel, FALSE);
						}
					}
				}

				break;
		}
	}
/*
	if(RuEntity_FindFirstPaperdoll(m_targetEntity) && channel == ruFUSION_MIME_CHANNEL_FOREGROUND)
	{
		char moo[200];
		sprintf(moo, "Attempted action queue %d failed.\n", action);
		OutputDebugString(moo);
	}
*/
	return FALSE;
}

BOOL CRuFusion_Mime::SetDefaultAction(CRuFusion_Mime_Channel channel, CRuFusion_Mime_Action action, PTRVALUE eventData)
{
	// Setup shortcut references to specified channel
	ChannelDesc &chanDesc = m_channels[channel];
	CRuArrayList<ActionDesc> &actionQueue = m_queuedActions[channel];

	ActionDesc actionDesc;
	actionDesc = ActionDesc(action, m_mimeTemplate->GetMotion(action), eventData, FALSE, NULL);

	chanDesc.m_defaultAction = actionDesc;

	return TRUE;
}

BOOL CRuFusion_Mime::RestoreDefaultAction(CRuFusion_Mime_Channel channel)
{
	RemoveQueuedAction(channel, ruFUSION_MIME_ACTION_COUNT);
	return this->SetAction(channel, ruFUSION_MIME_ACTION_COUNT, ruFUSION_MIME_SETACTIONMETHOD_QUEUED, 0, NULL, FALSE);
}

INT32 CRuFusion_Mime::GetQueuedActionCount(CRuFusion_Mime_Channel channel)
{
	// Setup shortcut references to specified channel
	ChannelDesc &chanDesc = m_channels[channel];
	CRuArrayList<ActionDesc> &actionQueue = m_queuedActions[channel];

	return actionQueue.Count();
}

BOOL CRuFusion_Mime::HasQueuedAction(CRuFusion_Mime_Channel channel, CRuFusion_Mime_Action action)
{
	// Setup shortcut references to specified channel
	ChannelDesc &chanDesc = m_channels[channel];
	CRuArrayList<ActionDesc> &actionQueue = m_queuedActions[channel];

	for(INT32 i = 0; i < actionQueue.Count(); ++i)
	{
		if(actionQueue[i].m_action == action)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuFusion_Mime::RemoveQueuedAction(CRuFusion_Mime_Channel channel, CRuFusion_Mime_Action action)
{
	// Setup shortcut references to specified channel
	ChannelDesc &chanDesc = m_channels[channel];
	CRuArrayList<ActionDesc> &actionQueue = m_queuedActions[channel];

	for(INT32 i = 0; i < actionQueue.Count(); ++i)
	{
		if(actionQueue[i].m_action == action)
		{
			actionQueue.RemoveAt(i);
			--i;
		}
	}

	return TRUE;
}

BOOL CRuFusion_Mime::ForceActionDequeue(CRuFusion_Mime_Channel channel)
{
	return DequeueAction(channel, FALSE);
}

BOOL CRuFusion_Mime::ClearQueuedActions(CRuFusion_Mime_Channel channel)
{
	while(DequeueAction(channel, TRUE))
	{
	}

	return TRUE;
}

BOOL CRuFusion_Mime::SetBodyYawOffset(REAL yawOffset)
{
	m_body_YawOffset = yawOffset;

	return TRUE;
}

BOOL CRuFusion_Mime::SetBodyHeadingOffset(REAL headingOffset)
{
	m_body_HeadingOffset = headingOffset;

	return TRUE;
}

BOOL CRuFusion_Mime::SetTorsoHeadingOffset(REAL headingOffset)
{
	m_torso_HeadingOffset = headingOffset;

	return TRUE;
}

BOOL CRuFusion_Mime::SetExpectedMotionScale(REAL expectedMotionScale)
{
	m_expectedMotionScale = expectedMotionScale;

	return TRUE;
}

BOOL CRuFusion_Mime::SetDefaultForwardSpeed(REAL defaultForwardSpeed)
{
	m_defaultForwardSpeed = defaultForwardSpeed;

	return TRUE;
}

BOOL CRuFusion_Mime::SetForwardSpeed(REAL forwardSpeed)
{
	m_forwardSpeed = forwardSpeed;

	return TRUE;
}

BOOL CRuFusion_Mime::SetDefaultBackwardSpeed(REAL defaultBackwardSpeed)
{
	m_defaultBackwardSpeed = defaultBackwardSpeed;

	return TRUE;
}

BOOL CRuFusion_Mime::SetBackwardSpeed(REAL backwardSpeed)
{
	m_backwardSpeed = backwardSpeed;

	return TRUE;
}

INT32 CRuFusion_Mime::GetNumBoneScalers()
{
	return m_mimeTemplate->GetNumBoneScalers() + 1;
}

const char *CRuFusion_Mime::GetBoneScalerNameByIndex(INT32 index)
{
	// Index is for the body scale?
	if(index == 0)
	{
		return "Body Scale";
	}

	// Adjust index so that it is zero-based
	--index;

	// Get bone scaler from template and return its name
	CRuFusion_Mime_BoneScaler *boneScaler = m_mimeTemplate->GetBoneScaler(index);

	if(boneScaler)
	{
		return (char *) (*(boneScaler->m_boneScalerName));
	}

	return "";
}

BOOL CRuFusion_Mime::GetBoneScalerParametersByIndex(INT32 index, CRuVector3 &scaleOut)
{
	// Index is for the body scale?
	if(index == 0)
	{
		scaleOut.Set(m_bodyScale, m_bodyScale, m_bodyScale);
		return TRUE;
	}

	// Adjust index so that it is zero-based
	--index;

	// Get bone scaler from template
	CRuFusion_Mime_BoneScaler *boneScaler = m_mimeTemplate->GetBoneScaler(index);

	// Search bone scaler descriptors for a matching descriptor
	for(INT32 i = 0; i < m_boneScalerDescriptors.Count(); ++i)
	{
		if(m_boneScalerDescriptors[i].m_boneScaler == boneScaler)
		{
			// Return scale
			scaleOut = m_boneScalerDescriptors[i].m_boneScale;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuFusion_Mime::SetBoneScalerParameters(const char *boneScalerName, const CRuVector3 &scale)
{
	// Check against built-in scalers
	if(stricmp("Body Scale", boneScalerName) == 0)
	{
		m_bodyScale = max(max(scale.m_x, scale.m_y), scale.m_z);
	}

	// Attempt to find the specified bone scaler among the list of active ones
	INT32 boneScalerDescIndex = -1;
	for(INT32 i = 0; i < m_boneScalerDescriptors.Count(); ++i)
	{
		if(stricmp(*m_boneScalerDescriptors[i].m_boneScaler->m_boneScalerName, boneScalerName) == 0)
		{
			boneScalerDescIndex = i;
			break;
		}
	}

	// If there is no active bone scaler that matches the specified name, create a new one
	if(boneScalerDescIndex == -1 && m_targetHierarchy)
	{
		for(INT32 i = 0; i < m_mimeTemplate->GetNumBoneScalers(); ++i)
		{
			CRuFusion_Mime_BoneScaler *boneScaler = m_mimeTemplate->GetBoneScaler(i);

			if(stricmp(*boneScaler->m_boneScalerName, boneScalerName) == 0)
			{
				// Store bone scaler descriptor index
				boneScalerDescIndex = m_boneScalerDescriptors.Count();

				// Create new bone scaler descriptor
				BoneScalerDesc boneScalerDesc;
				boneScalerDesc.m_boneScaler = boneScaler;
				boneScalerDesc.m_boneScale = scale;
				boneScalerDesc.m_boneScalerController = NULL;
//				boneScalerDesc.m_boneScalerController = ruNEW CRuController_Bone();
//				boneScalerDesc.m_boneScalerController->SetTarget(m_targetHierarchy);
//				boneScalerDesc.m_boneScalerController->SetBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));

				m_boneScalerDescriptors.Add(boneScalerDesc);
			}
		}
	}

	if(boneScalerDescIndex >= 0)
	{
		BoneScalerDesc &boneScalerDesc = m_boneScalerDescriptors[boneScalerDescIndex];
		CRuFusion_Mime_BoneScaler *boneScaler = boneScalerDesc.m_boneScaler;

		// Update bone scale
		boneScalerDesc.m_boneScale = scale;

		REAL heightOffset = 0.0f;
/*
		// Note -- this is the old version which relies on dynamic matrix manipulation

		// If height compensation is enabled, check bone height now
		if(m_targetHierarchy->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
		{
			INT32 targetBoneIndex = static_cast<CRuHierarchy_GR2 *>(m_targetHierarchy)->GetSubNodeIndexByName(*boneScaler->m_targetBoneName);
			CRuMatrix4x4 *restPoseTransforms = static_cast<CRuHierarchy_GR2 *>(m_targetHierarchy)->GetSubNodeRestPoseTransforms();

			if(targetBoneIndex >= 0 && restPoseTransforms)
			{
				CRuVector3 originalPosition(restPoseTransforms[targetBoneIndex]._41, restPoseTransforms[targetBoneIndex]._42, restPoseTransforms[targetBoneIndex]._43);
				CRuVector3 relativePosition(originalPosition.m_x * scale.m_x, originalPosition.m_y * scale.m_y, originalPosition.m_z * scale.m_z);

				heightOffset = relativePosition.m_y - originalPosition.m_y;
			}
		}

		// Set transform target for the target bone
		boneScalerDesc.m_boneScalerController->SetTransformTarget(*boneScaler->m_targetBoneName, scale, CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 0.0f), 1.0f);

		CRuVector3 inverseScale(1.0f / scale.m_x, 1.0f / scale.m_y, 1.0f / scale.m_z);
		for(INT32 i = 0; i < boneScaler->m_terminatorBoneNames.Count(); ++i)
		{
			boneScalerDesc.m_boneScalerController->AddTransformStopTarget(*boneScaler->m_targetBoneName, *boneScaler->m_terminatorBoneNames[i]);
		}
*/
/*
		if(m_targetHierarchy->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
		{
			static_cast<CRuHierarchy_GR2 *>(m_targetHierarchy)->SetBoneModifier(*boneScaler->m_targetBoneName, scale);

			CRuVector3 inverseScale(1.0f / scale.m_x, 1.0f / scale.m_y, 1.0f / scale.m_z);
			for(INT32 i = 0; i < boneScaler->m_terminatorBoneNames.Count(); ++i)
			{
				static_cast<CRuHierarchy_GR2 *>(m_targetHierarchy)->SetBoneModifier(*boneScaler->m_terminatorBoneNames[i], inverseScale);
			}
		}
*/
	}

	if(m_targetHierarchy && m_targetHierarchy->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
	{
		// Reset all bone modifiers
		static_cast<CRuHierarchy_GR2 *>(m_targetHierarchy)->ResetBoneModifiers();

		// Set bone modifiers into hierarchy
		for(INT32 i = 0; i < m_boneScalerDescriptors.Count(); ++i)
		{
			BoneScalerDesc &boneScalerDesc = m_boneScalerDescriptors[i];
			CRuFusion_Mime_BoneScaler *boneScaler = boneScalerDesc.m_boneScaler;

			CRuVector3 boneScale = m_boneScalerDescriptors[i].m_boneScale;
			CRuVector3 inverseScale(1.0f / boneScale.m_x, 1.0f / boneScale.m_y, 1.0f / boneScale.m_z);

			// Construct temporary terminator bone name array for passing into SetBoneModifier(...)
			if(boneScaler->m_terminatorBoneNames.Count() > 0)
			{
				char **terminatorBoneNames = ruNEW char * [boneScaler->m_terminatorBoneNames.Count() + 1];			
				for(INT32 i = 0; i < boneScaler->m_terminatorBoneNames.Count(); ++i)
				{
					INT32 terminatorBoneNameLen = (INT32)strlen(*boneScaler->m_terminatorBoneNames[i]);
					terminatorBoneNames[i] = ruNEW char [terminatorBoneNameLen + 1];
					strcpy(terminatorBoneNames[i], *boneScaler->m_terminatorBoneNames[i]);
					terminatorBoneNames[i + 1] = NULL;
				}

				// Set bone modifier
				static_cast<CRuHierarchy_GR2 *>(m_targetHierarchy)->SetBoneModifier(*boneScaler->m_targetBoneName, boneScale, terminatorBoneNames);

				// Free temporary terminator bone name array
				for(INT32 i = 0; i < boneScaler->m_terminatorBoneNames.Count(); ++i)
				{
					ruSAFE_DELETE_ARRAY(terminatorBoneNames[i]);
				}
				ruSAFE_DELETE_ARRAY(terminatorBoneNames);
			}
			else
			{
				// Set bone modifier
				static_cast<CRuHierarchy_GR2 *>(m_targetHierarchy)->SetBoneModifier(*boneScaler->m_targetBoneName, boneScale, NULL);
			}


/*
			static_cast<CRuHierarchy_GR2 *>(m_targetHierarchy)->SetBoneModifier(*boneScaler->m_targetBoneName, boneScale);

			for(INT32 i = 0; i < boneScaler->m_terminatorBoneNames.Count(); ++i)
			{
				static_cast<CRuHierarchy_GR2 *>(m_targetHierarchy)->SetBoneModifier(*boneScaler->m_terminatorBoneNames[i], inverseScale);
			}
*/
		}
	}

	return TRUE;
}

void CRuFusion_Mime::Update(REAL elapsedTime)
{
	if(m_targetEntity == NULL)
		return;

	Update_WithChannel(elapsedTime);

/*
	if(m_targetEntitySupportsChannels)
	{
		Update_WithChannel(elapsedTime);
	}
	else
	{
		Update_WithoutChannel(elapsedTime);
	}
*/

	// Update body twist
	if(m_body_BoneController)
	{
		CRuQuaternion xRot, yRot;
		xRot.FromAngleAxis(CRuVector3(1.0f, 0.0f, 0.0f), m_body_YawOffset);
		yRot.FromAngleAxis(CRuVector3(0.0f, 1.0f, 0.0f), m_body_HeadingOffset);

		CRuVector3 originHeading = CRuVector3(0.0f, 0.0f, 1.0f);
		CRuVector3 targetHeading = originHeading * yRot.GetMatrix() * xRot.GetMatrix();

		for(INT32 i = 0; i < m_mimeTemplate->GetNumBodyBones(); ++i)
		{
			m_body_BoneController->SetTransformTarget(m_mimeTemplate->GetBodyBoneName(i), CRuVector3(1.0f, 1.0f, 1.0f), originHeading, targetHeading, CRuVector3(0.0f, 0.0f, 0.0f), m_mimeTemplate->GetBodyBoneWeight(i));
		}
	}

	// Update torso twist
	if(m_torso_BoneController)
	{
		CRuQuaternion yRot;
		yRot.FromAngleAxis(CRuVector3(0.0f, 1.0f, 0.0f), m_torso_HeadingOffset);

		CRuVector3 originHeading = CRuVector3(0.0f, 0.0f, 1.0f);
		CRuVector3 targetHeading = originHeading * yRot.GetMatrix();

		for(INT32 i = 0; i < m_mimeTemplate->GetNumTorsoBones(); ++i)
		{
			m_torso_BoneController->SetTransformTarget(m_mimeTemplate->GetTorsoBoneName(i), CRuVector3(1.0f, 1.0f, 1.0f), originHeading, targetHeading, CRuVector3(0.0f, 0.0f, 0.0f), m_mimeTemplate->GetTorsoBoneWeight(i));
		}
	}

	// Apply body scale
	if(m_targetHierarchy)
	{
		m_targetHierarchy->Scale(m_bodyScale, ruTRANSFORMOP_SET);
	}
}

void CRuFusion_Mime::Update_WithChannel(REAL elapsedTime)
{
	REAL effectiveExpectedMotionScale = m_expectedMotionScale / m_bodyScale;

	// Get current time stamp
	DWORD curTimeStamp = GetTickCount();

	// Shortcuts
	ChannelDesc &c0 = m_channels[ruFUSION_MIME_CHANNEL_BACKGROUND];		// Master channel
	ChannelDesc &c1 = m_channels[ruFUSION_MIME_CHANNEL_FOREGROUND];		// Slave channel

	CRuArrayList<ActionDesc> &actionQueue1 = m_queuedActions[ruFUSION_MIME_CHANNEL_FOREGROUND];

	// Calculate current action delay time due to queueing
	REAL actionDelayTime = 0.0f;
	if(actionQueue1.Count() > 1)
	{
		actionDelayTime = (curTimeStamp - actionQueue1[0].m_actionTimeStamp) * 0.001f;
/*
		char woo[2000];
		sprintf(woo, "%f ", actionDelayTime);

		for(INT32 i = 0; i < actionQueue1.Count(); ++i)
		{
			char woo2[200];
			sprintf(woo2, "%d ", actionQueue1[i].m_actionTimeStamp);
			strcat(woo, woo2);
		}

		strcat(woo, "\n");

		OutputDebugString(woo);
*/
	}

	// Foreground & Background channel processing
	if(c0.m_activeAction != ruFUSION_MIME_NONE || c1.m_activeAction != ruFUSION_MIME_NONE)
	{
		REAL backgroundMotionScale = GetMotionScale(c0.m_activeAction);
		REAL foregroundMotionScale = GetMotionScale(c1.m_activeAction);

		INT32 backgroundPriority = m_mimeTemplate->GetActionPriority(c0.m_activeAction);
		INT32 foregroundPriority = m_mimeTemplate->GetActionPriority(c1.m_activeAction);

		REAL c1Scale = effectiveExpectedMotionScale;

		if(actionDelayTime > 0.0f)
		{
			c1Scale = -(actionDelayTime + 1.0f);
		}

/*
		if(RuEntity_FindFirstPaperdoll(m_targetEntity))
		{
			char moo[200];
			sprintf(moo, "Queue length: %d, Queue time length: %f, motion scale: %f\n", actionQueue1.Count(), c1QueueLength, c1Scale);
			OutputDebugString(moo);
		}
*/
		// Only c0 is active
		if(c1.m_activeAction == c0.m_activeAction || c1.m_activeAction == ruFUSION_MIME_NONE)
		{
			// Play motion from c0 on both channels
			RuACT_PlayMotionEx(m_targetEntity, c0.m_motionChannel, c0.m_motionName, c0.m_blendDelay, FALSE, 1.0f, 0.0f, 0.0f, effectiveExpectedMotionScale);
			RuACT_PlayMotionEx(m_targetEntity, c1.m_motionChannel, c0.m_motionName, c0.m_blendDelay, FALSE, 1.0f, 0.0f, 0.0f, effectiveExpectedMotionScale);

			if(!m_targetEntitySupportsChannels)
			{
				RuACT_PlayMotionEx(m_targetEntity, m_defaultChannel, c0.m_motionName, c0.m_blendDelay, FALSE, 1.0f, 0.0f, 0.0f, effectiveExpectedMotionScale);
			}
		}
		else
		{
			// Check foreground priority versus background priority
			if(foregroundPriority >= backgroundPriority)
			{
				// c1 has higher priority, so have it override c0
				RuACT_PlayMotionEx(m_targetEntity, c0.m_motionChannel, c1.m_motionName, c1.m_blendDelay, c1.m_motionJustDequeued, 1.0f, 0.0f, 0.0f, c1Scale);
				RuACT_PlayMotionEx(m_targetEntity, c1.m_motionChannel, c1.m_motionName, c1.m_blendDelay, c1.m_motionJustDequeued, 1.0f, 0.0f, 0.0f, c1Scale);

				if(!m_targetEntitySupportsChannels)
				{
					RuACT_PlayMotionEx(m_targetEntity, m_defaultChannel, c1.m_motionName, c1.m_blendDelay, c1.m_motionJustDequeued, 1.0f, 0.0f, 0.0f, effectiveExpectedMotionScale);
				}
			}
			else
			{
				INT32 priorityDiff = backgroundPriority - foregroundPriority;

				if(priorityDiff > 10)
				{
					// c0 has higher priority, priority difference exceed threshold, so c1 motion does not play at all
					RuACT_PlayMotionEx(m_targetEntity, c0.m_motionChannel, c0.m_motionName, c0.m_blendDelay, c0.m_motionJustDequeued, 1.0f, 0.0f, 0.0f, effectiveExpectedMotionScale);
					RuACT_PlayMotionEx(m_targetEntity, c1.m_motionChannel, c0.m_motionName, c0.m_blendDelay, c0.m_motionJustDequeued, 1.0f, 0.0f, 0.0f, effectiveExpectedMotionScale);

					if(!m_targetEntitySupportsChannels)
					{
						RuACT_PlayMotionEx(m_targetEntity, m_defaultChannel, c0.m_motionName, c0.m_blendDelay, c0.m_motionJustDequeued, 1.0f, 0.0f, 0.0f, effectiveExpectedMotionScale);
					}
				}
				else
				{
					// Background has higher priority, foreground motion plays in foreground channel only
					RuACT_PlayMotionEx(m_targetEntity, c0.m_motionChannel, c0.m_motionName, c0.m_blendDelay, c0.m_motionJustDequeued, 1.0f, 0.0f, 0.0f, effectiveExpectedMotionScale);
					RuACT_PlayMotionEx(m_targetEntity, c1.m_motionChannel, c1.m_motionName, c1.m_blendDelay, c1.m_motionJustDequeued, 1.0f, 0.0f, 0.0f, c1Scale);

					if(!m_targetEntitySupportsChannels)
					{
						RuACT_PlayMotionEx(m_targetEntity, m_defaultChannel, c0.m_motionName, c0.m_blendDelay, c0.m_motionJustDequeued, 1.0f, 0.0f, 0.0f, effectiveExpectedMotionScale);
					}
				}
			}
		}
	}

	if(c1.m_motionJustDequeued && c1.m_actionEvents)
	{
		CRuACTEntity *actEntity = c1.m_actionEvents->GetACTEntity();

		if(actEntity)
		{
			CRuArrayList<IRuEntity_Controller *> *controllers = actEntity->GetActiveMotionControllers(c1.m_motionChannel);

			if(controllers)
			{
				// Get sequence number from the action events object
				RuPropertyType propertyValue;
				INT32 seqNumber = c1.m_actionEvents->GetProperty("mimesys:seqnumber", propertyValue) ? boost::get<INT32>(propertyValue) : 0;

				// Set sequence number into all of the controllers
				for(INT32 i = 0; i < controllers->Count(); ++i)
				{
					controllers->Get(i)->GetTarget()->SetProperty("mimesys:seqnumber", seqNumber);
				}
			}
		}
	}

	// Reset motion flags
	c0.m_motionJustDequeued = FALSE;
	c1.m_motionJustDequeued = FALSE;

	// Dequeue actions for channels which are not active
	for(INT32 i = 0; i < ruFUSION_MIME_CHANNEL_COUNT; ++i)
	{
		if(m_channels[i].m_activeAction == ruFUSION_MIME_NONE)
		{
			DequeueAction(static_cast<CRuFusion_Mime_Channel>(i), FALSE);
		}
		else
		{
			ChannelDesc &channel = m_channels[i];
			CRuArrayList<ActionDesc> &actionQueue = m_queuedActions[i];

			CRuFusion_Mime_ActionType actionType = m_mimeTemplate->GetActionType(channel.m_activeAction);
			BOOL motionIsComplete = RuACT_IsMotionComplete(m_targetEntity, channel.m_motionChannel, TRUE) || channel.m_motionIsInterruptible;
			BOOL dequeueMotion = FALSE;
/*
			if(RuEntity_FindFirstPaperdoll(m_targetEntity) && i == ruFUSION_MIME_CHANNEL_FOREGROUND && motionIsComplete)
			{
				char moo[200];
				sprintf(moo, "Current action %d is complete @ %u (m_motionIsInterruptible = %d)\n", m_channels[i].m_activeAction, timeGetTime(), channel.m_motionIsInterruptible);
				OutputDebugString(moo);
			}
*/
			switch(actionType)
			{
				case ruFUSION_MIME_ACTIONTYPE_NORMAL:
					{
						if(motionIsComplete)
						{
							dequeueMotion = TRUE;
						}
					}

					break;

				case ruFUSION_MIME_ACTIONTYPE_IDLELOOP:
					{
						// Special handling rule for foreground channel
						if(i == ruFUSION_MIME_CHANNEL_FOREGROUND)
						{
							// Dequeue any idle loop motions following the current action
							for(INT32 j = 0; j < actionQueue.Count(); ++j)
							{
								// Break if the action is not an idle loop
								if(m_mimeTemplate->GetActionType(actionQueue[j].m_action) != ruFUSION_MIME_ACTIONTYPE_IDLELOOP)
									break;

								// Break if the idle loop action has higher priority than the current action
								if(m_mimeTemplate->GetActionPriority(actionQueue[j].m_action) > m_mimeTemplate->GetActionPriority(channel.m_activeAction))
									break;

								// Remove the motion
								DequeueAction(static_cast<CRuFusion_Mime_Channel>(i), TRUE);
							}
						}

						if(actionQueue.Count() > 0)
						{
							// Request motion dequeue
							dequeueMotion = TRUE;
						}
					}

					break;

				case ruFUSION_MIME_ACTIONTYPE_LOWPRIORITY:
					{
						if(motionIsComplete)
						{
							dequeueMotion = TRUE;
							break;
						}

						for(INT32 j = 0; j < actionQueue.Count(); ++j)
						{
							//
							if(m_mimeTemplate->GetActionOverrides(actionQueue[j].m_action))
							{
								dequeueMotion = TRUE;

								// Dequeue all excess motions
								for(INT32 k = 0; k < j; ++k)
								{
									DequeueAction(static_cast<CRuFusion_Mime_Channel>(i), FALSE);
								}

								break;
							}

							if(m_mimeTemplate->GetActionType(actionQueue[j].m_action) != ruFUSION_MIME_ACTIONTYPE_IDLELOOP)
							{
								break;
							}
						}
					}

					break;

				case ruFUSION_MIME_ACTIONTYPE_ULTRALOWPRIORITY:
					{
						if(motionIsComplete)
						{
							dequeueMotion = TRUE;
						}

						for(INT32 j = 0; j < actionQueue.Count(); ++j)
						{
							if(m_mimeTemplate->GetActionType(actionQueue[j].m_action) != ruFUSION_MIME_ACTIONTYPE_IDLELOOP && actionQueue[j].m_action != ruFUSION_MIME_ACTION_COUNT)
							{
								dequeueMotion = TRUE;
								break;
							}
						}
					}

					break;
			}

			if(dequeueMotion)
			{
				DequeueAction(static_cast<CRuFusion_Mime_Channel>(i), FALSE);
			}
		}
	}
}

void CRuFusion_Mime::Update_WithoutChannel(REAL elapsedTime)
{
/*
	if(m_backgroundAction != ruFUSION_MIME_NONE || m_foregroundAction != ruFUSION_MIME_NONE)
	{
		REAL backgroundMotionScale = 0.0f;
		REAL foregroundMotionScale = 0.0f;

		INT32 backgroundPriority = m_mimeTemplate->GetActionPriority(m_backgroundAction);
		INT32 foregroundPriority = m_mimeTemplate->GetActionPriority(m_foregroundAction);

		// Determine motion scale to use for background action
		switch(m_backgroundAction)
		{
			case ruFUSION_MIME_RUN_FORWARD:
			case ruFUSION_MIME_SWIM_FORWARD:
				backgroundMotionScale = m_forwardSpeed / m_defaultForwardSpeed;
				break;

			case ruFUSION_MIME_RUN_BACKWARD:
			case ruFUSION_MIME_SWIM_BACKWARD:
				backgroundMotionScale = m_backwardSpeed / m_defaultBackwardSpeed;
				break;

			default:
				backgroundMotionScale = 0.0f;
				break;
		}

		// Determine motion scale to use for foreground action
		switch(m_foregroundAction)
		{
			case ruFUSION_MIME_RUN_FORWARD:
			case ruFUSION_MIME_SWIM_FORWARD:
				foregroundMotionScale = m_forwardSpeed / m_defaultForwardSpeed;
				break;

			case ruFUSION_MIME_RUN_BACKWARD:
			case ruFUSION_MIME_SWIM_BACKWARD:
				foregroundMotionScale = m_backwardSpeed / m_defaultBackwardSpeed;
				break;

			default:
				foregroundMotionScale = 0.0f;
				break;
		}

		if(m_foregroundAction == m_backgroundAction || m_foregroundAction == ruFUSION_MIME_NONE)
		{
			RuACT_PlayMotionEx(m_targetEntity, m_defaultChannel, m_backgroundMotion, m_blendDelay, FALSE, 1.0f, 0.0f, 0.0f, foregroundMotionScale);
			m_backgroundActive = TRUE;
		}
		else
		{
			// Check foreground priority versus background priority
			if(foregroundPriority >= backgroundPriority)
			{
				// Foreground has higher priority, so have it override the background channel
				RuACT_PlayMotionEx(m_targetEntity, m_defaultChannel, m_foregroundMotion, m_blendDelay, FALSE, 1.0f, 0.0f, 0.0f, foregroundMotionScale);
				m_backgroundActive = FALSE;
			}
			else
			{
				RuACT_PlayMotionEx(m_targetEntity, m_defaultChannel, m_backgroundMotion, m_blendDelay, FALSE, 1.0f, 0.0f, 0.0f, backgroundMotionScale);
				m_backgroundActive = TRUE;
			}
		}

		if(m_backgroundActive)
		{
			// Check background channel to see whether the motion has stopped and a new motion needs to be picked.
			if(RuACT_IsMotionComplete(m_targetEntity, m_defaultChannel) != false || strlen(m_backgroundMotion) == 0)
			{
				// If there is a schedule background action, apply it now
				if(m_scheduledBackgroundAction != ruFUSION_MIME_NONE)
				{
					m_backgroundAction = m_scheduledBackgroundAction;
					m_backgroundMotionType = m_scheduledBackgroundMotionType;
					m_backgroundMotion = m_mimeTemplate->GetMotion(m_backgroundAction);

					// Clear background action schedule
					m_scheduledBackgroundAction = ruFUSION_MIME_NONE;
					m_scheduledBackgroundMotionType = ruFUSION_MIME_MOTIONTYPE_NORMAL;
				}
				else
				{
					m_backgroundMotion = m_mimeTemplate->GetMotion(m_backgroundAction);
				}
			}
		}
		else
		{
			// If the foreground channel has finished, reset its status
			if(RuACT_IsMotionComplete(m_targetEntity, m_defaultChannel) != false || strlen(m_foregroundMotion) == 0)
			{
				// If there is a schedule foreground action, apply it now
				if(m_scheduledForegroundAction != ruFUSION_MIME_NONE)
				{
					m_foregroundAction = m_scheduledForegroundAction;
					m_foregroundMotionType = m_scheduledForegroundMotionType;
					m_foregroundMotion = m_mimeTemplate->GetMotion(m_foregroundAction);

					// Clear Foreground action schedule
					m_scheduledForegroundAction = ruFUSION_MIME_NONE;
					m_scheduledForegroundMotionType = ruFUSION_MIME_MOTIONTYPE_NORMAL;
				}
				else
				{
					m_foregroundAction = ruFUSION_MIME_NONE;
					m_foregroundMotion = "";
				}
			}
		}
	}
*/
}

REAL CRuFusion_Mime::GetMotionScale(CRuFusion_Mime_Action action)
{
	REAL motionScale = 0.0f;

	switch(action)
	{
		case ruFUSION_MIME_RUN_FORWARD:
		case ruFUSION_MIME_SWIM_FORWARD:
			motionScale = m_forwardSpeed / m_defaultForwardSpeed;
			break;

		case ruFUSION_MIME_RUN_BACKWARD:
		case ruFUSION_MIME_SWIM_BACKWARD:
			motionScale = m_backwardSpeed / m_defaultBackwardSpeed;
			break;
	}

	return motionScale;
}

BOOL CRuFusion_Mime::QueueAction(CRuFusion_Mime_Channel channel, const ActionDesc &actionDesc)
{
	// Setup shortcut references to specified channel
	ChannelDesc &chanDesc = m_channels[channel];
	CRuArrayList<ActionDesc> &actionQueue = m_queuedActions[channel];

	// Insert action at the end of the queue
	INT32 actionIdx = actionQueue.Count();
	actionQueue.Add(actionDesc);

	// Store length of the specified motion
	actionQueue[actionIdx].m_motionLength = RuACT_GetMotionLength(m_targetEntity, actionDesc.m_motionName);
/*
	if(RuEntity_FindFirstPaperdoll(m_targetEntity) && channel == ruFUSION_MIME_CHANNEL_FOREGROUND)
	{
		char moo[200];
		sprintf(moo, "Queued action %d at end, queue length %d, %s @ t = %u\n", actionDesc.m_action, actionQueue.Count(), actionDesc.m_exclusiveAction ? "exclusive" : "not exclusive", timeGetTime());
		OutputDebugString(moo);
	}
*/
	return TRUE;
}

BOOL CRuFusion_Mime::DequeueAction(CRuFusion_Mime_Channel channel, BOOL removeOnly)
{
/*
	if(RuEntity_FindFirstPaperdoll(m_targetEntity) && channel == ruFUSION_MIME_CHANNEL_FOREGROUND)
	{
		char moo[200];
		sprintf(moo, "Restored action");
	}
*/
	// Setup shortcut references to specified channel
	ChannelDesc &chanDesc = m_channels[channel];
	CRuArrayList<ActionDesc> &actionQueue = m_queuedActions[channel];

	if(removeOnly)
	{
		if(actionQueue.Count() > 0)
		{
/*
			if(RuEntity_FindFirstPaperdoll(m_targetEntity) && channel == ruFUSION_MIME_CHANNEL_FOREGROUND)
			{
				char moo[200];
				sprintf(moo, "Removed action %d, queue length %d @ t = %u\n", actionQueue[0].m_action, actionQueue.Count() - 1, timeGetTime());
				OutputDebugString(moo);
			}
*/
			actionQueue.RemoveAt(0);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	if(actionQueue.Count() > 0)
	{
		if(actionQueue[0].m_action == ruFUSION_MIME_ACTION_COUNT)
		{
			// Retrieve the chainable action
			CRuFusion_Mime_Action chainableAction = m_mimeTemplate->GetChainableAction(chanDesc.m_activeAction);

			// Restore default action
			chanDesc.m_activeAction = chanDesc.m_defaultAction.m_action;
			chanDesc.m_exclusiveAction = chanDesc.m_defaultAction.m_exclusiveAction;
			chanDesc.m_blendDelay = (chanDesc.m_activeAction == chainableAction) ? 0.0f : min(m_blendDelay, m_mimeTemplate->GetActionMaxBlendDelay(chanDesc.m_defaultAction.m_action));
			chanDesc.m_motionName = chanDesc.m_defaultAction.m_motionName;
			chanDesc.m_motionIsInterruptible = strlen(chanDesc.m_motionName) > 0 ? FALSE : TRUE;
			chanDesc.m_motionJustDequeued = TRUE;
			chanDesc.m_actionEvents = actionQueue[0].m_actionEvents;
/*
			if(RuEntity_FindFirstPaperdoll(m_targetEntity) && channel == ruFUSION_MIME_CHANNEL_FOREGROUND)
			{
				char moo[200];
				sprintf(moo, "Restored action %d (%s), queue length %d @ t = %u\n", chanDesc.m_activeAction, chanDesc.m_motionName, actionQueue.Count() - 1, timeGetTime());
				OutputDebugString(moo);
			}
*/
			// Invoke action changed callback
			if(m_actionChangedCallback)
			{
				m_actionChangedCallback(channel, chanDesc.m_activeAction, chanDesc.m_defaultAction.m_eventData);
			}

			// Remove new action from queue
			actionQueue.RemoveAt(0);
		}
		else
		{
			// Retrieve the chainable action
			CRuFusion_Mime_Action chainableAction = m_mimeTemplate->GetChainableAction(chanDesc.m_activeAction);

			// Apply new action
			chanDesc.m_activeAction = actionQueue[0].m_action;
			chanDesc.m_exclusiveAction = actionQueue[0].m_exclusiveAction;
			chanDesc.m_blendDelay = (chanDesc.m_activeAction == chainableAction) ? 0.0f : min(m_blendDelay, m_mimeTemplate->GetActionMaxBlendDelay(actionQueue[0].m_action));
			chanDesc.m_motionName = actionQueue[0].m_motionName;
			chanDesc.m_motionIsInterruptible = strlen(chanDesc.m_motionName) > 0 ? FALSE : TRUE;
			chanDesc.m_motionJustDequeued = TRUE;
			chanDesc.m_actionEvents = actionQueue[0].m_actionEvents;
/*
			if(RuEntity_FindFirstPaperdoll(m_targetEntity) && channel == ruFUSION_MIME_CHANNEL_FOREGROUND)
			{
				char moo[200];
				sprintf(moo, "Dequeued action %d (%s), queue length %d @ t = %u\n", chanDesc.m_activeAction, chanDesc.m_motionName, actionQueue.Count() - 1, timeGetTime());
				OutputDebugString(moo);
			}
*/
			// Invoke action changed callback
			if(m_actionChangedCallback)
			{
				m_actionChangedCallback(channel, actionQueue[0].m_action, actionQueue[0].m_eventData);
			}

			// Remove new action from queue
			actionQueue.RemoveAt(0);
		}
	}
	else
	{
		chanDesc.m_activeAction = ruFUSION_MIME_NONE;
		chanDesc.m_exclusiveAction = FALSE;
		chanDesc.m_motionName = "";
		chanDesc.m_actionEvents = NULL;
	}

	// Trigger dequeue action event
	RuMimeEvent_DequeueAction_Args eventArgs(this, channel, chanDesc.m_activeAction);
	m_event_DequeueAction.Trigger(&eventArgs);

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
