#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged


//-----------------------------------------------------------------------------------------

// Client -> Local
// 通知Server Client 要裝備物品
struct PG_EQ_CtoL_SetEQ
{
    GamePGCommandEnum	Command;

    ItemFieldStruct		Item;
    int					ItemPos;
    EQWearPosENUM		EqPos;

    short               PosType;

	bool				IsFromEq;		//來源為EQ

    PG_EQ_CtoL_SetEQ( )
    {	Command = EM_PG_EQ_CtoL_SetEQ;	}
};
//-----------------------------------------------------------------------------------------
//Client -> Local 交換備份裝備
struct PG_EQ_CtoL_SwapBackupEQ
{
	GamePGCommandEnum	Command;
	int					BackupPosID;

	PG_EQ_CtoL_SwapBackupEQ( )
	{	Command = EM_PG_EQ_CtoL_SwapBackupEQ;	}
};
//-----------------------------------------------------------------------------------------
//Local -> Client 交換備份裝備
struct PG_EQ_LtoC_SwapBackupEQ
{
	GamePGCommandEnum	Command;
	int					BackupPosID;
	bool				Result;
	PG_EQ_LtoC_SwapBackupEQ( )
	{	Command = EM_PG_EQ_LtoC_SwapBackupEQ;	}
};
//-----------------------------------------------------------------------------------------
enum EQResult_ENUM
{
    ENMU_EQ_SetEqOK			= 0     ,
    ENMU_EQ_SetEqFailed		    	,
};

// Local -> Client
 
struct PG_EQ_LtoC_SetEQResult
{
    GamePGCommandEnum	Command;

    EQResult_ENUM		Result;

    PG_EQ_LtoC_SetEQResult( )
    {	Command = EM_PG_EQ_LtoC_SetEQResult;	}
};
//-----------------------------------------------------------------------------------------
// Client -> Local
// 通知Server 是否要隱藏裝備
struct PG_EQ_CtoL_SetShowEQ
{
	GamePGCommandEnum	Command;
	ShowEQStruct		ShowEQ;

	PG_EQ_CtoL_SetShowEQ( )
	{	Command = EM_PG_EQ_CtoL_SetShowEQ;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// 通知Client 隱藏裝備裝備資料
struct PG_EQ_LtoC_SetShowEQ
{
	GamePGCommandEnum	Command;
	int					GItemID;
	ShowEQStruct		ShowEQ;

	PG_EQ_LtoC_SetShowEQ( )
	{	Command = EM_PG_EQ_LtoC_SetShowEQ;	}
};

//-----------------------------------------------------------------------------------------
// Local -> Client
// 通知Client 裝備爆了
struct PG_EQ_LtoC_EQBroken
{
	GamePGCommandEnum	Command;
	EQWearPosENUM		Pos;

	PG_EQ_LtoC_EQBroken( )
	{	Command = EM_PG_EQ_LtoC_EQBroken;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client
//開啟套裝學習介面
struct PG_EQ_LtoC_LearnSuitSkill_Open
{
	GamePGCommandEnum	Command;
	int			TargetID;

	PG_EQ_LtoC_LearnSuitSkill_Open( )
	{	Command = EM_PG_EQ_LtoC_LearnSuitSkill_Open;	}
};
//-----------------------------------------------------------------------------------------
//Client -> Local 
//關閉介面
struct PG_EQ_CtoL_LearnSuitSkill_Close
{
	GamePGCommandEnum	Command;

	PG_EQ_CtoL_LearnSuitSkill_Close( )
	{	Command = EM_PG_EQ_CtoL_LearnSuitSkill_Close;	}
};
//-----------------------------------------------------------------------------------------
//Client -> Local
//學習技能
struct PG_EQ_CtoL_LearnSuitSkillRequest
{
	GamePGCommandEnum	Command;
	int		ItemCount;		//套裝數量		
	int		ItemPos[10];	//身體物品位置 高為元 0 表示物品攔 1表示裝備攔
	int		LearnSkillID;	//學習的技能

	PG_EQ_CtoL_LearnSuitSkillRequest( )
	{	Command = EM_PG_EQ_CtoL_LearnSuitSkillRequest;	}
};
//-----------------------------------------------------------------------------------------
enum	LearnSuitSkillResultENUM
{
	EM_LearnSuitSkillResult_OK			,
	EM_LearnSuitSkillResult_NotOpen		,	//介面沒開啟
	EM_LearnSuitSkillResult_DataErr		,	//資料有問題
	EM_LearnSuitSkillResult_CountErr	,	//套裝數量有問題
	EM_LearnSuitSkillResult_AlwaysLearn	,	//已學會
	EM_LearnSuitSkillResult_SkillFull	,	//技能已滿
	EM_LearnSuitSkillResult_MoneyErr	,	//金錢不足
};
//Local -> Client
//學習結果
struct PG_EQ_LtoC_LearnSuitSkillResult
{
	GamePGCommandEnum	Command;
	int					LearnSkillID;	//學習的技能
	LearnSuitSkillResultENUM Result;

	PG_EQ_LtoC_LearnSuitSkillResult( )
	{	Command = EM_PG_EQ_LtoC_LearnSuitSkillResult;	}
};
//-----------------------------------------------------------------------------------------
//設定使用中的套裝技能
//Client -> Local
struct PG_EQ_CtoL_SetSuitSkillRequest
{
	GamePGCommandEnum	Command;
	int		SkillID;
	int		Pos;

	PG_EQ_CtoL_SetSuitSkillRequest( )
	{	Command = EM_PG_EQ_CtoL_SetSuitSkillRequest;	}
};
enum	SetSuitSkillResultENUM
{
	EM_SetSuitSkillResult_OK			,
	EM_SetSuitSkillResult_OnAttackMode	,	//戰鬥狀態不能使用
	EM_SetSuitSkillResult_SkillNotFind	,	//查無此技能
	EM_SetSuitSkillResult_PosErr		,	//位置錯誤
};
//Local -> Client
//設定結果
struct PG_EQ_LtoC_SetSuitSkillResult
{
	GamePGCommandEnum	Command;
	SetSuitSkillResultENUM Result;
	
	PG_EQ_LtoC_SetSuitSkillResult( )
	{	Command = EM_PG_EQ_LtoC_SetSuitSkillResult;	}
};
//-----------------------------------------------------------------------------------------
//Client -> Local
//要求使用套裝技能
struct PG_EQ_CtoL_UseSuitSkillRequest
{
	GamePGCommandEnum	Command;
	int		SkillID;
	int		TargetID;
	float	PosX , PosY , PosZ ;
	PG_EQ_CtoL_UseSuitSkillRequest( )
	{	Command = EM_PG_EQ_CtoL_UseSuitSkillRequest;	}
};
enum	UseSuitSkillResultENUM
{
	EM_UseSuitSkillResult_OK			,
	EM_UseSuitSkillResult_SkillNotFind	,	//找不到技能
};
//Local -> Client
//設定結果
struct PG_EQ_LtoC_UseSuitSkillResult
{
	GamePGCommandEnum	Command;
	UseSuitSkillResultENUM Result;

	PG_EQ_LtoC_UseSuitSkillResult( )
	{	Command = EM_PG_EQ_LtoC_UseSuitSkillResult;	}
};
//-----------------------------------------------------------------------------------------
#pragma managed
