#pragma once

#include "RoleData\RoleData.h"
#include "NetWakerPGEnum.h"

//-----------------------------------------------------------------------------
struct PG_Script_CtoL_QuestAccept
{	
	PACKET_CONSTRUCT( PG_Script_CtoL_QuestAccept );
	
	int iQuestNpcID; 
	int iQuestID;
};
//-----------------------------------------------------------------------------
struct PG_Script_CtoL_QuestComplete
{	
	PACKET_CONSTRUCT( PG_Script_CtoL_QuestComplete );
	
	int iQuestNpcID; 
	int iQuestID;
	int iChoiceID;
};
//-----------------------------------------------------------------------------
struct PG_Script_CtoL_QuestDelete
{	
	PACKET_CONSTRUCT( PG_Script_CtoL_QuestDelete );

	int iQuestField;
	int iQuestID;
};
//-----------------------------------------------------------------------------
struct PG_Script_CtoL_TouchEvent
{
	PACKET_CONSTRUCT( PG_Script_CtoL_TouchEvent );

	int iEventObjID;
};
//-----------------------------------------------------------------------------
struct PG_Script_CtoL_RangeEvent
{
	PACKET_CONSTRUCT( PG_Script_CtoL_RangeEvent );

	int iEventObjID;
};
//-----------------------------------------------------------------------------
struct PG_Script_CtoL_CollisionEvent
{
	PACKET_CONSTRUCT( PG_Script_CtoL_CollisionEvent );

	int iEventObjID;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_QuestAccept_Result
{
	PACKET_CONSTRUCT( PG_Script_LtoC_QuestAccept_Result );

	int						iQuestID;
	int						iQuestField;
	EM_QUESTCHECK_RESULT	emResult;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_QuestComplete_Result
{
	PACKET_CONSTRUCT( PG_Script_LtoC_QuestComplete_Result );

	int						iQuestID;
	int						iQuestField;

	EM_QUESTCHECK_RESULT	emResult;
	int						iQuestNPCID;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_QuestDelete_Result
{
	PACKET_CONSTRUCT( PG_Script_LtoC_QuestDelete_Result );

	int						iQuestID;
	int						iQuestField;
	EM_QUESTCHECK_RESULT	emResult;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_QuestFlag
{
	PACKET_CONSTRUCT( PG_Script_LtoC_QuestFlag );

	int						iQuestFlagID;
	int						iVal;
};
//-----------------------------------------------------------------------------
struct PG_Script_CtoL_ClickQuestObj
{
	PACKET_CONSTRUCT( PG_Script_CtoL_ClickQuestObj );
	
	int						iQuestField;
	int						iQuestID;
	int						iClickObj;
	int						iClickObjID;
};
//-----------------------------------------------------------------------------
struct PG_Script_CtoL_ClickQuestObjFailed
{
	PACKET_CONSTRUCT( PG_Script_CtoL_ClickQuestObjFailed );
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_ClickQuestObjResult
{
	PACKET_CONSTRUCT( PG_Script_LtoC_ClickQuestObjResult );

	int						iResult;
	int						iItemID;
	int						iItemVal;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_KillQuestObjNotify
{
	PACKET_CONSTRUCT( PG_Script_LtoC_KillQuestObjNotify );

	int						iQuestField;
	int						iQuestID;
	int						iDeadObjID;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_GetQuestObjNotify
{
	PACKET_CONSTRUCT( PG_Script_LtoC_GetQuestObjNotify );

	int						iQuestField;
	int						iQuestID;
	int						iObjGUID;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_PlayMotion
{
	PACKET_CONSTRUCT( PG_Script_LtoC_PlayMotion );

	int						iObjID;
	//int						iMotionID;
	char					szMotion[64];
};
//-----------------------------------------------------------------------------
struct PG_Script_CtoL_LoadQuestSpeakOption
{
	PACKET_CONSTRUCT( PG_Script_CtoL_LoadQuestSpeakOption );

	int						iNPCID;
};
//-----------------------------------------------------------------------------
struct PG_Script_CtoL_ChoiceOption
{
	PACKET_CONSTRUCT( PG_Script_CtoL_ChoiceOption );

	int						iOptionID;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_ChoiceOptionResult
{
	PACKET_CONSTRUCT( PG_Script_LtoC_ChoiceOptionResult );
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_CloseSpeak
{
	PACKET_CONSTRUCT( PG_Script_LtoC_CloseSpeak );
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_Play3DSound
{
	PACKET_CONSTRUCT( PG_Script_LtoC_Play3DSound );

	int						iObjID;
	StaticString< 256 >		sSoundPath;
	bool					bLoop;
	int						iSoundID;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_PlaySound
{
	PACKET_CONSTRUCT( PG_Script_LtoC_PlaySound );

	int						iObjID;
	StaticString< 256 >		sSoundPath;
	bool					bLoop;
	int						iSoundID;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_PlayMusic
{
	PACKET_CONSTRUCT( PG_Script_LtoC_PlayMusic );

	int						iObjID;
	StaticString< 256 >		sSoundPath;
	bool					bLoop;
	int						iSoundID;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_StopSound
{
	PACKET_CONSTRUCT( PG_Script_LtoC_StopSound );

	int						iObjID;
	int						iSoundID;
};

//-----------------------------------------------------------------------------
struct PG_Script_CtoL_DelayPatrol
{
	PACKET_CONSTRUCT( PG_Script_CtoL_DelayPatrol );

	int						iObjID;
	int						iTime;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_SetQuestState
{
	PACKET_CONSTRUCT( PG_Script_LtoC_SetQuestState );

	int						iQuestID;
	int						iState;
};

//-----------------------------------------------------------------------------
struct PG_Script_LtoC_ShowBorder
{
	PACKET_CONSTRUCT( PG_Script_LtoC_ShowBorder );

	int											iQuestID;

	StaticString<_MAX_LUA_FUNTIONNAMESIZE_>		szTitle;
	StaticString<_MAX_LUA_FUNTIONNAMESIZE_>		szTexture;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_ClearBorder
{
	PACKET_CONSTRUCT( PG_Script_LtoC_ClearBorder );
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_AddBorderPage
{
	PACKET_CONSTRUCT( PG_Script_LtoC_AddBorderPage );
	// 長度不定, 要看函式內容
};
//-----------------------------------------------------------------------------
struct PG_Script_CtoL_AcceptBorderQuest
{
	PACKET_CONSTRUCT( PG_Script_CtoL_AcceptBorderQuest );	
};
//-----------------------------------------------------------------------------
struct PG_Script_CtoL_CloseBorder
{
	PACKET_CONSTRUCT( PG_Script_CtoL_CloseBorder );	
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_ScriptCast
{
	PACKET_CONSTRUCT( PG_Script_LtoC_ScriptCast );	
	
	StaticString<_MAX_LUA_FUNTIONNAMESIZE_>		szString;

	int			iTime;
	int			iMotionID;
	int			iMotionEndID;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_ScriptCastResult
{
	PACKET_CONSTRUCT( PG_Script_LtoC_ScriptCastResult );	

	int			iResult;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_ResetDailyQuest
{
	PACKET_CONSTRUCT( PG_Script_LtoC_ResetDailyQuest );	

	int			iType;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_AttachCastMotionToolID
{
	PACKET_CONSTRUCT( PG_Script_LtoC_AttachCastMotionToolID );	
	int			iToolID;
	int			iObjID;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_AttachTool
{
	PACKET_CONSTRUCT( PG_Script_LtoC_AttachTool );	
	
	int					iObjID;
	int					iToolID;
	StaticString<64>	LinkID;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_StartClientCountDown
{
	PACKET_CONSTRUCT( PG_Script_LtoC_StartClientCountDown );	
	int			iStartNumber;
	int			iEndNumber;
	int			iOffset;
	int         iMode;
	int			iShowStatusBar;
	StaticString<64> szText;
};
//-----------------------------------------------------------------------------
struct PG_Script_LtoC_SetCameraFollowUnit
{
	PACKET_CONSTRUCT( PG_Script_LtoC_SetCameraFollowUnit );	

	int iObjID;
	int offsetX;
	int offsetY;
	int offsetZ;
};