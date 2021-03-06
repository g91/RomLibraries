#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleData.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged


//-----------------------------------------------------------------------------------------
// Client -> Local
// 要求修正所有基本數值
struct PG_RoleValue_CtoL_RequestFixRoleValue
{
    GamePGCommandEnum	Command;

    PG_RoleValue_CtoL_RequestFixRoleValue( )
    {	Command = EM_PG_RoleValue_CtoL_RequestFixRoleValue;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// 修正所有基本數值
struct PG_RoleValue_LtoC_FixAllRoleValue
{
    GamePGCommandEnum	Command;

    BaseAbilityStruct	Ability;
    //SkillStruct			Skill;

    PG_RoleValue_LtoC_FixAllRoleValue( )
    {	Command = EM_PG_RoleValue_LtoC_FixAllRoleValue;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// 修正基本數值
struct PG_RoleValue_LtoC_FixRoleValue
{
    GamePGCommandEnum	Command;

    RoleValueName_ENUM	ValueName;
    double				Value;

    PG_RoleValue_LtoC_FixRoleValue( )
    {	Command = EM_PG_RoleValue_LtoC_FixRoleValue;	}
};

struct PG_RoleValue_LtoC_GetTargetRoleValue
{
	GamePGCommandEnum	Command;

	int					GUID;
	RoleValueName_ENUM	ValueName;
	double				Value;

	PG_RoleValue_LtoC_GetTargetRoleValue( )
	{	Command = EM_PG_RoleValue_LtoC_GetTargetRoleValue;	}
};

//-----------------------------------------------------------------------------------------
// Client -> Local
// 要求配點 
struct PG_RoleValue_CtoL_RequestSetPoint
{
    GamePGCommandEnum	Command;

//    RoleValueName_ENUM	ValueName;
//    int					Value;
	int STR;
	int	STA;
	int INT;
	int MND;
	int	AGI;

    PG_RoleValue_CtoL_RequestSetPoint( )
    {	Command = EM_PG_RoleValue_CtoL_RequestSetPoint;	}
};
//-----------------------------------------------------------------------------------------
// Server -> Client
// 配點回應
struct PG_RoleValue_LtoC_ResultSetPoint
{
    GamePGCommandEnum	Command;

    bool                Result;


    PG_RoleValue_LtoC_ResultSetPoint( )
    {	Command = EM_PG_RoleValue_LtoC_ResultSetPoint;	} 
};
//-----------------------------------------------------------------------------------------
// Server -> Client
// 通知某角色升級
struct PG_RoleValue_LtoC_NotifyLvUp
{
    GamePGCommandEnum	Command;

    int					ItemID;
    int					LV;			//目前等級

    PG_RoleValue_LtoC_NotifyLvUp( )
    {	Command = EM_PG_RoleValue_LtoC_NotifyLvUp;	} 
};
//-----------------------------------------------------------------------------------------
// Server -> Client
//設定頭銜旗標
struct PG_RoleValue_LtoC_SetTitleFlag
{
    GamePGCommandEnum	Command;

    int					TitleID;
    bool				Flag;			

    PG_RoleValue_LtoC_SetTitleFlag( )
    {	Command = EM_PG_RoleValue_LtoC_SetTitleFlag;	} 
};
//-----------------------------------------------------------------------------------------
// Server -> Client
//設定旗標
struct PG_RoleValue_LtoC_SetFlag
{
    GamePGCommandEnum	Command;

    int					ID;
    bool				Flag;			

    PG_RoleValue_LtoC_SetFlag( )
    {	Command = EM_PG_RoleValue_LtoC_SetFlag;	} 
};
//-----------------------------------------------------------------------------------------
// Client -> Server
//設定頭銜資料
struct PG_RoleValue_CtoL_SetTitlRequest
{
    GamePGCommandEnum	Command;
    int					TitleID;
	bool				IsShowTitle;		//是否顯示

    PG_RoleValue_CtoL_SetTitlRequest( )
    {	Command = EM_PG_RoleValue_CtoL_SetTitlRequest;	} 
};
//-----------------------------------------------------------------------------------------
// Server -> Client
//回應頭銜設定結果
struct PG_RoleValue_LtoC_SetTitleResult
{
    GamePGCommandEnum	Command;
    int					TitleID;
    bool				Result;

    PG_RoleValue_LtoC_SetTitleResult( )
    {	Command = EM_PG_RoleValue_LtoC_SetTitleResult;	} 
};
//-----------------------------------------------------------------------------------------
/*
// Server -> Client
//通知周圍玩家頭銜設定
struct PG_RoleValue_LtoC_Title
{
    GamePGCommandEnum	Command;
    int					ItemID;
    int					TitleID;
	bool				IsShowTitle;
	StaticString< _MAX_NAMERECORD_SIZE_ >   TitleStr;

    PG_RoleValue_LtoC_Title( )
    {	Command = EM_PG_RoleValue_LtoC_Title;	} 
};
*/
//-----------------------------------------------------------------------------------------
//通知某玩家升級
struct PG_RoleValue_LtoC_RoleInfoChange
{
    GamePGCommandEnum	Command;
    int					ItemID;
    int					Level;
	int					Level_Sub;
	Voc_ENUM			Voc;
	Voc_ENUM			Voc_Sub;

    PG_RoleValue_LtoC_RoleInfoChange( )
    {	Command = EM_PG_RoleValue_LtoC_RoleInfoChange;	} 
};
//-----------------------------------------------------------------------------------------
//  更換職業處理
//-----------------------------------------------------------------------------------------
//Local -> Client 
//通知開啟換職業
struct PG_RoleValue_LtoC_OpenChangeJob
{
	GamePGCommandEnum	Command;
	int					TargetID;		//觸發劇情的目標

	PG_RoleValue_LtoC_OpenChangeJob( )
	{	Command = EM_PG_RoleValue_LtoC_OpenChangeJob;	} 
};
//-----------------------------------------------------------------------------------------
//Client -> Local 
//通知關畢換職業
struct PG_RoleValue_CtoL_CloseChangeJobRequest
{
	GamePGCommandEnum	Command;

	PG_RoleValue_CtoL_CloseChangeJobRequest( )
	{	Command = EM_PG_RoleValue_CtoL_CloseChangeJobRequest;	} 
};
//-----------------------------------------------------------------------------------------
//Local -> Client
//通知關畢換職業
struct PG_RoleValue_LtoC_CloseChangeJob
{
	GamePGCommandEnum	Command;

	PG_RoleValue_LtoC_CloseChangeJob( )
	{	Command = EM_PG_RoleValue_LtoC_CloseChangeJob;	} 
};

//-----------------------------------------------------------------------------------------
//Client -> Local 
//通知要換的主副職
struct PG_RoleValue_CtoL_ChangeJobRequest
{
	GamePGCommandEnum	Command;
	int			Job;			//主職
	int			Job_Sub;		//副職

	PG_RoleValue_CtoL_ChangeJobRequest( )
	{	Command = EM_PG_RoleValue_CtoL_ChangeJobRequest;	} 
};
//-----------------------------------------------------------------------------------------
enum ChangeJobResult_ENUM
{
	EM_ChangeJobResult_OK				,		//成?
	EM_ChangeJobResult_Failed			,		//失敗		
};
//Local -> Client
//換角結果
struct PG_RoleValue_LtoC_ChangeJobResult
{
	GamePGCommandEnum		Command;
	ChangeJobResult_ENUM	Result;

	PG_RoleValue_LtoC_ChangeJobResult( )
	{	Command = EM_PG_RoleValue_LtoC_ChangeJobResult;	} 
};
//-----------------------------------------------------------------------------------------
//Client -> Local
//通知 Logout
struct PG_RoleValue_CtoL_ClientLogout
{
	GamePGCommandEnum		Command;

	PG_RoleValue_CtoL_ClientLogout( )
	{	Command = EM_PG_RoleValue_CtoL_ClientLogout;	} 
};
//-----------------------------------------------------------------------------------------
//Local -> Client
//通知 Logout
struct PG_RoleValue_LtoC_ClientLogoutResult
{
	GamePGCommandEnum		Command;
	int						Second;

	PG_RoleValue_LtoC_ClientLogoutResult( )
	{	Command = EM_PG_RoleValue_LtoC_ClientLogoutResult;	} 
};
//-----------------------------------------------------------------------------------------
// Client -> Local
// 取消 Logout
struct PG_RoleValue_CtoL_CancelClientLogout
{
	GamePGCommandEnum		Command;

	PG_RoleValue_CtoL_CancelClientLogout( )
	{	Command = EM_PG_RoleValue_CtoL_CancelClientLogout;	} 
};
//-----------------------------------------------------------------------------------------
//Local -> Client
//通知 Logout
struct PG_RoleValue_LtoC_CancelClientLogout
{
	GamePGCommandEnum		Command;

	PG_RoleValue_LtoC_CancelClientLogout( )
	{	Command = EM_PG_RoleValue_LtoC_CancelClientLogout;	} 
};
//-----------------------------------------------------------------------------------------
//Client -> Local
//設定角色全區狀態
struct PG_RoleValue_CtoL_SetPlayerAllZoneState
{
	GamePGCommandEnum		Command;
	PlayerStateStruct		State;

	PG_RoleValue_CtoL_SetPlayerAllZoneState( )
	{	Command = EM_PG_RoleValue_CtoL_SetPlayerAllZoneState;	} 
};
//-----------------------------------------------------------------------------------------
//Local -> Client
//設定角色全區狀態
struct PG_RoleValue_LtoC_SetPlayerAllZoneState
{
	GamePGCommandEnum		Command;
	int						GItemID;
	PlayerStateStruct		State;

	PG_RoleValue_LtoC_SetPlayerAllZoneState( )
	{	Command = EM_PG_RoleValue_LtoC_SetPlayerAllZoneState;	} 
};
//-----------------------------------------------------------------------------------------
//Client -> Local
//要求存Client端的Flag
struct PG_RoleValue_CtoL_SetClientFlag
{
	GamePGCommandEnum		Command;
	StaticFlag<_MAX_CLIENT_FLAG_COUNT_>		ClientFlag;

	PG_RoleValue_CtoL_SetClientFlag( )
	{	Command = EM_PG_RoleValue_CtoL_SetClientFlag;	} 
};
//-----------------------------------------------------------------------------------------
//Client -> Local
//要求存Client端的Flag
struct PG_RoleValue_CtoL_SetClientData
{
	GamePGCommandEnum		Command;
	char					ClientData[_MAX_CLIENT_DATA_SIZE_];

	PG_RoleValue_CtoL_SetClientData( )
	{	Command = EM_PG_RoleValue_CtoL_SetClientData;	} 
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// 基本數值改變差值
struct PG_RoleValue_LtoC_DeltaRoleValue
{
	GamePGCommandEnum	Command;

	RoleValueName_ENUM	ValueName;
	double				Value;

	PG_RoleValue_LtoC_DeltaRoleValue( )
	{	Command = EM_PG_RoleValue_LtoC_DeltaRoleValue;	}
};
//-----------------------------------------------------------------------------------------
//Client -> Local
//Client端要求 換平形空間
struct PG_RoleValue_CtoL_ChangeParallel
{
	GamePGCommandEnum		Command;
	int						ParallelID;

	PG_RoleValue_CtoL_ChangeParallel( )
	{	Command = EM_PG_RoleValue_CtoL_ChangeParallel;	} 
};
//-----------------------------------------------------------------------------------------
//Client -> Local
//取消換平形空間
struct PG_RoleValue_CtoL_CancelChangeParallel
{
	GamePGCommandEnum		Command;

	PG_RoleValue_CtoL_CancelChangeParallel( )
	{	Command = EM_PG_RoleValue_CtoL_CancelChangeParallel;	} 
};
//-----------------------------------------------------------------------------------------
//Local -> Client
//通知等待換平行空間
struct PG_RoleValue_LtoC_WaitChangeParallel
{
	GamePGCommandEnum	Command;

	int					Second;
	bool				Result;

	PG_RoleValue_LtoC_WaitChangeParallel( )
	{	Command = EM_PG_RoleValue_LtoC_WaitChangeParallel;	}
};
//-----------------------------------------------------------------------------------------
//Local -> Client 
//通知要求Client 的環境資料
struct PG_RoleValue_LtoC_ClientComputerInfoRequest
{
	GamePGCommandEnum	Command;
	

	PG_RoleValue_LtoC_ClientComputerInfoRequest( )
	{	Command = EM_PG_RoleValue_LtoC_ClientComputerInfoRequest;	}
};
//-----------------------------------------------------------------------------------------
// Client -> Local
//通知Client 的環境資料
struct PG_RoleValue_CtoL_ClientComputerInfo
{
	GamePGCommandEnum	Command;
	ClientComputerInfoStruct Info;

	PG_RoleValue_CtoL_ClientComputerInfo( )
	{	Command = EM_PG_RoleValue_CtoL_ClientComputerInfo;	}
};
//-----------------------------------------------------------------------------------------
//	通知Client 端某物件的Mode
// Local -> Client
struct PG_RoleValue_LtoC_ObjMode
{
	GamePGCommandEnum	Command;
	int					ItemGUID;
	ObjectModeStruct	Mode;	

	PG_RoleValue_LtoC_ObjMode( )
	{	Command = EM_PG_RoleValue_LtoC_ObjMode;	}
};
//-----------------------------------------------------------------------------------------
// Client -> Local
//Client的狀態資料
struct PG_RoleValue_CtoL_ClientEnvironmentState
{
	GamePGCommandEnum	Command;
	ClientStateStruct	State;

	PG_RoleValue_CtoL_ClientEnvironmentState( )
	{	Command = EM_PG_RoleValue_CtoL_ClientEnvironmentState;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// 通知某一個物件的角色資料
struct PG_RoleValue_LtoC_ObjRoleInfo
{
	GamePGCommandEnum	Command;
	PlayerRoleData		Role;

	PG_RoleValue_LtoC_ObjRoleInfo( )
	{	Command = EM_PG_RoleValue_LtoC_ObjRoleInfo;	}
};

#pragma managed
