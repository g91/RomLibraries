#pragma once

#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma warning (disable:4949)
#pragma unmanaged

//-----------------------------------------------------------------------------------------
// Client -> Local
// 通知Server Client 目前攻擊狀態

struct PG_Attack_CtoL_AttackSignal
{
    GamePGCommandEnum	Command;
    myInt32				TargetID;		//攻擊目標
  

    PG_Attack_CtoL_AttackSignal()
    {	Command = EM_PG_Attack_CtoL_AttackSignal;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// 回應要求攻擊結果
enum AttackSignalResultENUM
{
    EM_AttackSignalResult_OK                    ,
    EM_AttackSignalResult_TargetDisAttackAbled  ,   //目標不可攻擊
    EM_AttackSignalResult_TargetNULL            ,   //無此目標

};

struct PG_Attack_LtoC_AttackSignalResult
{
    GamePGCommandEnum	    Command;
    myInt32				    TargetID;		//攻擊目標
    AttackSignalResultENUM  Result;

    PG_Attack_LtoC_AttackSignalResult()
    {	Command = EM_PG_Attack_LtoC_AttackSignalResult;	}
};
//-----------------------------------------------------------------------------------------
// Client -> Local
// 通知Server Client 結束戰鬥

struct PG_Attack_CtoL_EndAttackSignal
{
    GamePGCommandEnum	Command;

    PG_Attack_CtoL_EndAttackSignal()
    {	Command = EM_PG_Attack_CtoL_EndAttackSignal;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// 通知周圍的玩家攻擊命中或失敗的訊息

struct PG_Attack_LtoC_AttackResult
{
    GamePGCommandEnum	Command;

    myInt32		AttackID;		//攻擊者
    myInt32		UnderAttackID;	//被攻擊者
    myInt32		DamageHP;		//HP
	myInt32		OrgDamageHP;		//HP

    ATTACK_DMGTYPE_ENUM	Type;
	AttackHandTypeENUM	HandType;

    PG_Attack_LtoC_AttackResult()
    {	Command = EM_PG_Attack_LtoC_AttackResult;	}
};

//-----------------------------------------------------------------------------------------
// Local -> Client
// HP MP 數值變化時更新( 本身 隊友 範圍 )
struct PG_Attack_LtoC_HPMP
{
    GamePGCommandEnum	Command;

    myInt32		ItemID;		//區域ID
    myInt32		HP;
    myInt32		MP;
    myInt32       SP;
	myInt32		SP_Sub;

    PG_Attack_LtoC_HPMP()
    {	Command = EM_PG_Attack_LtoC_HPMPSP;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client 
// 通知 玩家某　MaxHP , MaxMP 的資訊
struct PG_Attack_LtoC_MaxHPMaxMP
{
    GamePGCommandEnum	Command;

    myInt32		ItemID;		//區域ID
    myInt32		MaxHP;
    myInt32		MaxMP;
    myInt32		MaxSP;
	myInt32		MaxSP_Sub;

    PG_Attack_LtoC_MaxHPMaxMP()
    {	Command = EM_PG_Attack_LtoC_MaxHPMaxMPMaxSP;	}
};

//-----------------------------------------------------------------------------------------
struct DeadOptionMode
{
	union		
	{
		int Mode;
		struct  
		{
			bool AutoRevive	: 1;	//自動重生
			bool RaiseMagic	: 1;	//重生法術
			bool Unknown	: 1;
		};
	};
};

// Local -> Client 
// 通知某物件死亡
struct PG_Attack_LtoC_Dead
{
    GamePGCommandEnum	Command;

    myInt32		DeadID;		//區域ID
    myInt32		KillerID;	//殺人者
    myInt32       Time;		
	RolePosStruct       Pos;
	myInt32		KillerOwnerID;	//殺人的擁有者
	myInt32		ReviveTime;		//重生的時間

	DeadOptionMode Mode;		//死亡模式

    PG_Attack_LtoC_Dead()
    {	Command = EM_PG_Attack_LtoC_Dead;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client 
// 通知某物件復活
struct PG_Attack_LtoC_Resurrection
{
    GamePGCommandEnum	Command;

    myInt32		ItemID;		//區域ID

    PG_Attack_LtoC_Resurrection()
    {	Command = EM_PG_Attack_LtoC_Resurrection;	}
};
//-----------------------------------------------------------------------------------------
//死亡回到重生點(放棄等待復活)
// Client -> Local
enum	RequestResurrection_ENUM
{
	EM_RequestResurrection_Normal		,	//一般復活
	EM_RequestResurrection_RaiseMagic	,	//重生法術
};

struct PG_Attack_CtoL_RequestResurrection
{
    GamePGCommandEnum			Command;
	RequestResurrection_ENUM	Type;

    PG_Attack_CtoL_RequestResurrection()
    {	Command = EM_PG_Attack_CtoL_RequestResurrection;	}
};
//-----------------------------------------------------------------------------------------
//設定某物件是否為公
struct PG_Attack_LtoC_SetAttackMode
{
    GamePGCommandEnum	Command;
	int					GItemID;
    bool                Flag;

    PG_Attack_LtoC_SetAttackMode()
    {	Command = EM_PG_Attack_LtoC_SetAttackMode;	}
};

//-----------------------------------------------------------------------------------------
// L -> C 通知Party成員的HPMPSP資訊
struct PG_Attack_LtoC_PartyMemberHPMPSP
{
	GamePGCommandEnum	Command;	

	int		DBID;
	int		HP,MP,SP,SP_Sub;		

	PG_Attack_LtoC_PartyMemberHPMPSP()
	{	Command = EM_PG_Attack_LtoC_PartyMemberHPMPSP;	};
};
// L -> C 通知Party成員的Max HPMPSP資訊
struct PG_Attack_LtoC_PartyMemberMaxHPMPSP
{
	GamePGCommandEnum	Command;	

	int		DBID;
	int		MaxHP,MaxMP,MaxSP,MaxSP_Sub;		

	PG_Attack_LtoC_PartyMemberMaxHPMPSP()
	{	Command = EM_PG_Attack_LtoC_PartyMemberMaxHPMPSP;	};
};
//-----------------------------------------------------------------------------------------
//設定NPC戰鬥鎖定某目標
//Local -> Client
struct PG_Attack_LtoC_NPCAttackTarget
{
	GamePGCommandEnum	Command;	

	int		NpcGUID;			//NPC 區域物件ID
	int		TargetGUID;			//攻擊目標
	
	PG_Attack_LtoC_NPCAttackTarget()
	{	Command = EM_PG_Attack_LtoC_NPCAttackTarget;	};
};
//-----------------------------------------------------------------------------------------
//通知區域PK 狀態
//Local -> Client
struct PG_Attack_LtoC_ZonePKFlag
{
	GamePGCommandEnum	Command;	

	PKTypeENUM	PKType;

	PG_Attack_LtoC_ZonePKFlag()
	{	Command = EM_PG_Attack_LtoC_ZonePKFlag;	};
};
//-----------------------------------------------------------------------------------------
#pragma managed
