#pragma once

#include "RoleData\RoleData.h"
#include "NetWakerPGEnum.h"

#define DF_MAX_BG_LANDMARK	10
#define DF_MAX_BG_VAR		20
#define DF_ARENA_MAXTYPE	3		// 1 = 1v1, 2 = 2vs2, 3 = 6vs6, 4 = 2vs2vs2vs2
#define DF_BG_MAXTEAM		8
#define DF_MAX_BG_ROLEVAR	3
#define DF_BG_MAXLVGROUP	10

#define DF_MAX_TEAMDBID		36


enum EM_BG_JOINQUEUE_RESULT
{
	EM_BG_JOINQUEUE_RESULT_OK					= 0,	 
	EM_BG_JOINQUEUE_RESULT_INBG					= 1,	 
	EM_BG_JOINQUEUE_RESULT_NOPARTY				= 2,	// 隊伍不存在
	EM_BG_JOINQUEUE_RESULT_INQUEUE				= 3,	// 人物已經在佇列中, 重新加入 QUEUE
	EM_BG_JOINQUEUE_RESULT_WAITENTERBG			= 4,	// 已經擁有進入戰場權力

	EM_BG_JOINQUEUE_RESULT_TEAM_WAITENTERBG		= 5,	// 某個隊員已經在佇列當中	( LEADER )
	EM_BG_JOINQUEUE_RESULT_TEAM_INBG			= 6,	// 某個隊員身在戰場中		( LEADER )

	EM_BG_JOINQUEUE_RESULT_TEAM_MEMBER_WAITBG	= 7,	// 你的隊伍因為你擁有進入戰捵的權力, 無法排隊
	EM_BG_JOINQUEUE_RESULT_TEAM_MEMBER_INBG		= 8,	// 你的隊伍因為你在戰場之中, 無法排隊

	EM_BG_JOINQUEUE_RESULT_LVERROR				= 9,	 
	EM_BG_JOINQUEUE_RESULT_NOSLOT				= 10,	// NO ROOM
};


struct StructLandMark
{
	StaticString< 32 >	Name;
	float				fX;
	float				fY;
	float				fZ;
	int					iIconID;
};

struct StructRoomLandMark
{
	StructLandMark		LandMark[DF_MAX_BG_LANDMARK];
};


struct StructBattleGroundPlayerPos
{
	StaticString< _MAX_NAMERECORD_SIZE_ >   RoleName;	    //角色名稱
	int										ZoneID;				//目前區域編號
	RolePosStruct       Pos;
};

struct StructBattleGroundValue
{
	int		iRoomID;
	int		iVar[DF_MAX_BG_VAR];
	int		iLandMark[DF_MAX_BG_LANDMARK];
	int		iTeamScore[DF_BG_MAXTEAM];
	int		iBattleGroundID;
};


//-----------------------------------------------------------------------------
/*
struct PG_Script_CtoL_QuestAccept
{	
	PACKET_CONSTRUCT( PG_Script_CtoL_QuestAccept );
	
	int iQuestNpcID; 
	int iQuestID;
};
*/
//----------------------------------------------------------
struct PG_BattleGround_CtoL_JoinBattleGround
{	
	PACKET_CONSTRUCT( PG_BattleGround_CtoL_JoinBattleGround );
	int		iObjID;
	int		iBattleGroundID;
	int		iParty;
	int		iSrvID;

	int		iWorldGroupID;
	int		iTeamDBID[ DF_MAX_TEAMDBID ];
};
//----------------------------------------------------------
struct PG_BattleGround_WLtoWL_JoinBattleGround
{	
	PACKET_CONSTRUCT( PG_BattleGround_WLtoWL_JoinBattleGround );

	int		iLV;
	int		iDBID;
	int		iBattleGroundID;
	int		iParty;
	int		iZoneID;

	int		iWorldGroupID;
	//int		iRoomID;
	//float	fX, fY, fZ, fDir;

	int		iGroupID;
	int		iTeamDBID[ DF_MAX_TEAMDBID ];
	int		iTeamMemberLV[ DF_MAX_TEAMDBID ];
};
//----------------------------------------------------------
struct PG_BattleGround_LtoL_JoinBattleGround
{	
	PACKET_CONSTRUCT( PG_BattleGround_LtoL_JoinBattleGround );

	int		iLV;
	int		iDBID;
	int		iBattleGroundID;
	int		iParty;
	int		iZoneID;

	int		iWorldGroupID;
	//int		iRoomID;
	//float	fX, fY, fZ, fDir;
	int		iGroupID;
	int		iTeamDBID[ DF_MAX_TEAMDBID ];
	int		iTeamMemberLV[ DF_MAX_TEAMDBID ];
};
//----------------------------------------------------------
struct PG_BattleGround_CtoL_GetBattleGround
{	
	PACKET_CONSTRUCT( PG_BattleGround_CtoL_GetBattleGround );
	int		iObjID;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoL_GetBattleGround
{	
	PACKET_CONSTRUCT( PG_BattleGround_LtoL_GetBattleGround );
	int		iDBID;
	int		iBattleGroundID;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_EnterBattleGroundRight
{	
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_EnterBattleGroundRight );

	int					iType;
	int					iRoomID;
	int					iTeamID;
	int					iWaitTime;
	StaticString< 32 >	sName;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_NotifyEnterBattleGroundRight
{	
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_NotifyEnterBattleGroundRight );

	int					iType;
	int					iRoomID;
	int					iWaitTime;
	StaticString< 32 >	sName;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_LostEnterBattleGroundRight
{	
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_LostEnterBattleGroundRight );

	int					iType;
	int					iRoomID;
	int					iWaitTime;
	StaticString< 32 >	sName;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_EmptyBattleGround
{	
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_EmptyBattleGround );

	int					iType;
	int					iRoomID;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_GetBattleGroundResult
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_GetBattleGroundResult );

	int					iBattleGroundID;

	bool				IsLimitJoinLevel;				// 是否限定進入等級
	int					iMinJoinLevel;					// 最低登入等級
	int					iMaxJoinLevel;					// 最高登入等級

	bool				IsForceGroup;					// 是否強制組隊

	int					iNumTeamPeople;					// 每個隊伍的人數限制
	int					iNumTeam;						// 隊伍數量
};
//----------------------------------------------------------
struct PG_BattleGround_CtoL_EnterBattleGround
{
	PACKET_CONSTRUCT( PG_BattleGround_CtoL_EnterBattleGround );

	int					iBattleGroundType;
	int					iRoomID;
	int					iTeamID;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoL_EnterBattleGround
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoL_EnterBattleGround );

	int					iClientDBID;
	int					iType;
	int					iRoomID;
	int					iTeamID;

	int					iRoleSourceZoneID;
	int					iRoleSourceRoomID;
	float				fRoleSourceX, fRoleSourceY, fRoleSourceZ, fRoleSourceDir;

	float				fRoleSourceHP;
	float				fRoleSourceMP;
};
//----------------------------------------------------------
struct PG_BattleGround_WLtoWL_EnterBattleGround
{
	PACKET_CONSTRUCT( PG_BattleGround_WLtoWL_EnterBattleGround );

	int					iClientDBID;
	int					iType;
	int					iRoomID;
	int					iTeamID;

	int					iRoleSourceZoneID;
	int					iRoleSourceRoomID;
	float				fRoleSourceX, fRoleSourceY, fRoleSourceZ, fRoleSourceDir;

	float				fRoleSourceHP;
	float				fRoleSourceMP;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoL_ChangeZoneToBattleGround
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoL_ChangeZoneToBattleGround );

	int					iClientDBID;
	int					iZoneID;
	int					iRoomID;
	float				X,Y,Z,Dir;
};

//----------------------------------------------------------
struct PG_BattleGround_WLtoWL_ChangeZoneToBattleGround
{
	PACKET_CONSTRUCT( PG_BattleGround_WLtoWL_ChangeZoneToBattleGround );

	int					iClientDBID;
	int					iWorldID;
	int					iZoneID;
	int					iRoomID;
	float				X,Y,Z,Dir;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_JoinQueueResult
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_JoinQueueResult );

	int					iType;
	int					iRoomID;
	int					iTeamID;
	int					iWaitTime;
	StaticString< 32 >	sName;
	int					iResult;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_BattleGroundClose
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_BattleGroundClose );
	int					iWinTeamID;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_BattleGroundStatus
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_BattleGroundStatus );
	StructBattleGroundValue	Data;
};
//----------------------------------------------------------
struct PG_BattleGround_CtoL_LeaveBattleGround
{
	PACKET_CONSTRUCT( PG_BattleGround_CtoL_LeaveBattleGround );
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_BattleGroundLandMark
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_BattleGroundLandMark );

	int					iID;
	StructLandMark		LandMark;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoChat_AskGroupListToJoin
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoChat_AskGroupListToJoin );

	int					iID;
	//StructLandMark		LandMark;
	int					iGroupID;
	int					iClientDBID;
};
//----------------------------------------------------------
struct PG_BattleGround_ChattoL_AskGroupListToJoinResult
{
	PACKET_CONSTRUCT( PG_BattleGround_ChattoL_AskGroupListToJoinResult );

	int					iResult;
	int					iClientDBID;
	int					iGroupList[40];
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_SetBattleGroundData
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_SetBattleGroundData );

	int					iForceFlagID;
	int					iCampID;
	int					iRoomID;
};
//----------------------------------------------------------

struct StructBattleGroundScore
{
	int										iTeamID;
	int										iDBID;
	int										iRoomID;
	int										iVocID;
	int										iVocSubID;
	int										iLV;
	int										iLVSub;
	StaticString< 64 >						Name;
	BattleGroundInfoStruct					Score;
	int										iStatus;
	int										iRoleVar[DF_MAX_BG_ROLEVAR];
};
//----------------------------------------------------------
struct PG_BattleGround_CtoL_GetBattleGroundScoreInfo
{
	PACKET_CONSTRUCT( PG_BattleGround_CtoL_GetBattleGroundScoreInfo );

	int						iClientDBID;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_BattleGroundScoreInfo
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_BattleGroundScoreInfo );

	int							iTotalRole;
	StructBattleGroundScore		pScoreArray[1];
	//StructBattleGroundScore	RoleScore; 內容為指標, 使用記憶體構成
};
//----------------------------------------------------------
struct PG_BattleGround_CtoL_LeaveBattleGroundWaitQueue
{
	PACKET_CONSTRUCT( PG_BattleGround_CtoL_LeaveBattleGroundWaitQueue );

	int		iDBID;
	int		iBattleGroundType;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoL_LeaveBattleGroundWaitQueue
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoL_LeaveBattleGroundWaitQueue );

	int		iDBID;
	int		iBattleGroundType;
};
//----------------------------------------------------------
struct PG_BattleGround_CtoL_LeaveBattleGroundRemote
{
	PACKET_CONSTRUCT( PG_BattleGround_CtoL_LeaveBattleGroundRemote );

	int		iDBID;
	int		iBattleGroundType;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoL_LeaveBattleGroundRemote
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoL_LeaveBattleGroundRemote );

	int		iDBID;
	int		iBattleGroundType;
};
//----------------------------------------------------------
struct PG_BattleGround_WLtoWL_LeaveBattleGroundWaitQueue
{
	PACKET_CONSTRUCT( PG_BattleGround_WLtoWL_LeaveBattleGroundWaitQueue );

	int		iDBID;
	int		iBattleGroundType;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_BattleGroundWaitQueueStatus
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_BattleGroundWaitQueueStatus );

	int		iBattleGroundType;
	int		iStatus;
	int		iWaitID;
	int		iSrvCheckTime;
};
//----------------------------------------------------------
struct PG_BattleGround_CtoL_BattleGroundWaitQueueStatusRespond
{
	PACKET_CONSTRUCT( PG_BattleGround_CtoL_BattleGroundWaitQueueStatusRespond );
	int		iSrvID;
	int		iDBID;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoL_BattleGroundWaitQueueStatusRespond
{	
	PACKET_CONSTRUCT( PG_BattleGround_LtoL_BattleGroundWaitQueueStatusRespond );
	int		iSrvID;
	int		iDBID;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_AllLandMarkInfo
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_AllLandMarkInfo );

	StructRoomLandMark		Info;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_AllBattleGroundPlayerPos
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_AllBattleGroundPlayerPos );

	int								iTotalPlayer;
	StructBattleGroundPlayerPos		PlayerPos[1];	// Server 組合封包
};
//----------------------------------------------------------
struct PG_BattleGround_LtoD_GetArenaData
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoD_GetArenaData );

	int								iArenaType;		// 1 = 1v1, 2 = 2vs2, 3 = 6vs6, 4 = 2vs2vs2vs2
	int								iTeamGUID;		 
};
//----------------------------------------------------------
struct PG_BattleGround_DtoL_UpdateArenaData
{
	PACKET_CONSTRUCT( PG_BattleGround_DtoL_UpdateArenaData );

	int								iArenaType;		// 1 = 1v1, 2 = 2vs2, 3 = 6vs6, 4 = 2vs2vs2vs2
	int								iTeamGUID;		 
	StructArenaInfo					ArenaInfo;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_UpdateArenaData
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_UpdateArenaData );

	int								iArenaType;		// 1 = 1v1, 2 = 2vs2, 3 = 6vs6, 4 = 2vs2vs2vs2
	int								iTeamGUID;		 
	StructArenaInfo					ArenaInfo;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoD_SetArenaData
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoD_SetArenaData );

	int								iArenaType;		// 1 = 1v1, 2 = 2vs2, 3 = 6vs6, 4 = 2vs2vs2vs2
	int								iTeamGUID;		 
	StructArenaInfo					ArenaInfo;
};
//----------------------------------------------------------
struct PG_BattleGround_CtoL_GetBattleGroundList
{
	PACKET_CONSTRUCT( PG_BattleGround_CtoL_GetBattleGroundList );

	int								iDBID;
};
//----------------------------------------------------------
struct PG_BattleGround_LtoL_GetBattleGroundList
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoL_GetBattleGroundList );

	int								iDBID;
};
//----------------------------------------------------------
struct PG_BattleGround_WLtoWL_GetBattleGroundList
{
	PACKET_CONSTRUCT( PG_BattleGround_WLtoWL_GetBattleGroundList );

	int								iDBID;
};
//----------------------------------------------------------
struct PG_BattleGround_WLtoWD_AddRankPoint
{
	PACKET_CONSTRUCT( PG_BattleGround_WLtoWD_AddRankPoint );

	int								iWorldDBID;
	int								iAddRankPoint;			// Value system want to add
	int								iCurrentRankPoint;		// for check
};
//----------------------------------------------------------
struct PG_BattleGround_WDtoWL_AddRankPointResult
{
	PACKET_CONSTRUCT( PG_BattleGround_WDtoWL_AddRankPointResult );

	int								iWorldDBID;
	int								iCurrentRankPoint;
};
//----------------------------------------------------------
/*
struct PG_BattleGround_WDtoWL_GetRankPointResult
{
	PACKET_CONSTRUCT( PG_BattleGround_WDtoWL_GetRankPointResult );

	int								iWorldDBID;
	int								iWorldID;
};
//----------------------------------------------------------
struct PG_BattleGround_WLtoWD_GetRankPointResult
{
	PACKET_CONSTRUCT( PG_BattleGround_WDtoWL_GetRankPointResult );

	int								iWorldDBID;
	int								iWorldID;
};
*/
//----------------------------------------------------------
/*
struct PG_BattleGround_WLtoWD_SetRankPersonalInfo
{
	PACKET_CONSTRUCT( PG_BattleGround_WDtoWL_GetRankPointResult );

	int								iWorldDBID;
	int								iWorldID;
};
*/
//----------------------------------------------------------
struct PG_BattleGround_WLtoWD_GetRankPersonalInfo
{
	PACKET_CONSTRUCT( PG_BattleGround_WLtoWD_GetRankPersonalInfo );

	int								iWorldDBID;
	int								iWorldID;

	int								iCliSockID;
	int								iCliProxyID;
};
//----------------------------------------------------------
struct PG_BattleGround_WDtoWL_GetRankPersonalInfoResult
{
	PACKET_CONSTRUCT( PG_BattleGround_WDtoWL_GetRankPersonalInfoResult );

	int								iWorldDBID;
	int								iWorldID;

	int								iRankPoint;
	int								iLastRoundRank;
	int								iRank;
	
	StaticString<128>				sNote;
};
//----------------------------------------------------------
struct PG_BattleGround_CtoL_GetRankPersonalInfo
{
	PACKET_CONSTRUCT( PG_BattleGround_CtoL_GetRankPersonalInfo );
	int								iMode;		// 0 = Personal, 1 = Top 100
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_GetRankPersonalInfoResult
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_GetRankPersonalInfoResult );

	int								iDBID;
	

	int								iRankPoint;
	int								iLastRoundRank;
	int								iRank;

	StaticString<128>				sNote;

};
//----------------------------------------------------------
/*
struct PG_BattleGround_WDtoWL_GetRankList
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_GetRankListCount );

	int								iWorldDBID;
	int								iCount;
//	StructRankInfo					Info[100];
};
//----------------------------------------------------------
struct PG_BattleGround_LtoC_GetRankList
{
	PACKET_CONSTRUCT( PG_BattleGround_LtoC_GetRankList );

	int								iWorldDBID;
	int								iCount;
//	StructRankInfo					Info[100];
};
*/

