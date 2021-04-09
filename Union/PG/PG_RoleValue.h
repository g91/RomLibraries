#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleData.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged


//-----------------------------------------------------------------------------------------
// Client -> Local
// ­n¨D­×¥¿©Ò¦³°ò¥»¼Æ­È
struct PG_RoleValue_CtoL_RequestFixRoleValue
{
    GamePGCommandEnum	Command;

    PG_RoleValue_CtoL_RequestFixRoleValue( )
    {	Command = EM_PG_RoleValue_CtoL_RequestFixRoleValue;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// ­×¥¿©Ò¦³°ò¥»¼Æ­È
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
// ­×¥¿°ò¥»¼Æ­È
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
// ­n¨D°tÂI 
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
// °tÂI¦^À³
struct PG_RoleValue_LtoC_ResultSetPoint
{
    GamePGCommandEnum	Command;

    bool                Result;


    PG_RoleValue_LtoC_ResultSetPoint( )
    {	Command = EM_PG_RoleValue_LtoC_ResultSetPoint;	} 
};
//-----------------------------------------------------------------------------------------
// Server -> Client
// ³qª¾¬Y¨¤¦â¤É¯Å
struct PG_RoleValue_LtoC_NotifyLvUp
{
    GamePGCommandEnum	Command;

    int					ItemID;
    int					LV;			//¥Ø«eµ¥¯Å

    PG_RoleValue_LtoC_NotifyLvUp( )
    {	Command = EM_PG_RoleValue_LtoC_NotifyLvUp;	} 
};
//-----------------------------------------------------------------------------------------
// Server -> Client
//³]©wÀY»ÎºX¼Ð
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
//³]©wºX¼Ð
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
//³]©wÀY»Î¸ê®Æ
struct PG_RoleValue_CtoL_SetTitlRequest
{
    GamePGCommandEnum	Command;
    int					TitleID;
	bool				IsShowTitle;		//¬O§_Åã¥Ü

    PG_RoleValue_CtoL_SetTitlRequest( )
    {	Command = EM_PG_RoleValue_CtoL_SetTitlRequest;	} 
};
//-----------------------------------------------------------------------------------------
// Server -> Client
//¦^À³ÀY»Î³]©wµ²ªG
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
//³qª¾©P³òª±®aÀY»Î³]©w
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
//³qª¾¬Yª±®a¤É¯Å
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
//  §ó´«Â¾·~³B²z
//-----------------------------------------------------------------------------------------
//Local -> Client 
//³qª¾¶}±Ò´«Â¾·~
struct PG_RoleValue_LtoC_OpenChangeJob
{
	GamePGCommandEnum	Command;
	int					TargetID;		//Ä²µo¼@±¡ªº¥Ø¼Ð

	PG_RoleValue_LtoC_OpenChangeJob( )
	{	Command = EM_PG_RoleValue_LtoC_OpenChangeJob;	} 
};
//-----------------------------------------------------------------------------------------
//Client -> Local 
//³qª¾Ãö²¦´«Â¾·~
struct PG_RoleValue_CtoL_CloseChangeJobRequest
{
	GamePGCommandEnum	Command;

	PG_RoleValue_CtoL_CloseChangeJobRequest( )
	{	Command = EM_PG_RoleValue_CtoL_CloseChangeJobRequest;	} 
};
//-----------------------------------------------------------------------------------------
//Local -> Client
//³qª¾Ãö²¦´«Â¾·~
struct PG_RoleValue_LtoC_CloseChangeJob
{
	GamePGCommandEnum	Command;

	PG_RoleValue_LtoC_CloseChangeJob( )
	{	Command = EM_PG_RoleValue_LtoC_CloseChangeJob;	} 
};

//-----------------------------------------------------------------------------------------
//Client -> Local 
//³qª¾­n´«ªº¥D°ÆÂ¾
struct PG_RoleValue_CtoL_ChangeJobRequest
{
	GamePGCommandEnum	Command;
	int			Job;			//¥DÂ¾
	int			Job_Sub;		//°ÆÂ¾

	PG_RoleValue_CtoL_ChangeJobRequest( )
	{	Command = EM_PG_RoleValue_CtoL_ChangeJobRequest;	} 
};
//-----------------------------------------------------------------------------------------
enum ChangeJobResult_ENUM
{
	EM_ChangeJobResult_OK				,		//¦¨¥
	EM_ChangeJobResult_Failed			,		//¥¢±Ñ		
};
//Local -> Client
//´«¨¤µ²ªG
struct PG_RoleValue_LtoC_ChangeJobResult
{
	GamePGCommandEnum		Command;
	ChangeJobResult_ENUM	Result;

	PG_RoleValue_LtoC_ChangeJobResult( )
	{	Command = EM_PG_RoleValue_LtoC_ChangeJobResult;	} 
};
//-----------------------------------------------------------------------------------------
//Client -> Local
//³qª¾ Logout
struct PG_RoleValue_CtoL_ClientLogout
{
	GamePGCommandEnum		Command;

	PG_RoleValue_CtoL_ClientLogout( )
	{	Command = EM_PG_RoleValue_CtoL_ClientLogout;	} 
};
//-----------------------------------------------------------------------------------------
//Local -> Client
//³qª¾ Logout
struct PG_RoleValue_LtoC_ClientLogoutResult
{
	GamePGCommandEnum		Command;
	int						Second;

	PG_RoleValue_LtoC_ClientLogoutResult( )
	{	Command = EM_PG_RoleValue_LtoC_ClientLogoutResult;	} 
};
//-----------------------------------------------------------------------------------------
// Client -> Local
// ¨ú®ø Logout
struct PG_RoleValue_CtoL_CancelClientLogout
{
	GamePGCommandEnum		Command;

	PG_RoleValue_CtoL_CancelClientLogout( )
	{	Command = EM_PG_RoleValue_CtoL_CancelClientLogout;	} 
};
//-----------------------------------------------------------------------------------------
//Local -> Client
//³qª¾ Logout
struct PG_RoleValue_LtoC_CancelClientLogout
{
	GamePGCommandEnum		Command;

	PG_RoleValue_LtoC_CancelClientLogout( )
	{	Command = EM_PG_RoleValue_LtoC_CancelClientLogout;	} 
};
//-----------------------------------------------------------------------------------------
//Client -> Local
//³]©w¨¤¦â¥þ°Ïª¬ºA
struct PG_RoleValue_CtoL_SetPlayerAllZoneState
{
	GamePGCommandEnum		Command;
	PlayerStateStruct		State;

	PG_RoleValue_CtoL_SetPlayerAllZoneState( )
	{	Command = EM_PG_RoleValue_CtoL_SetPlayerAllZoneState;	} 
};
//-----------------------------------------------------------------------------------------
//Local -> Client
//³]©w¨¤¦â¥þ°Ïª¬ºA
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
//­n¨D¦sClientºÝªºFlag
struct PG_RoleValue_CtoL_SetClientFlag
{
	GamePGCommandEnum		Command;
	StaticFlag<_MAX_CLIENT_FLAG_COUNT_>		ClientFlag;

	PG_RoleValue_CtoL_SetClientFlag( )
	{	Command = EM_PG_RoleValue_CtoL_SetClientFlag;	} 
};
//-----------------------------------------------------------------------------------------
//Client -> Local
//­n¨D¦sClientºÝªºFlag
struct PG_RoleValue_CtoL_SetClientData
{
	GamePGCommandEnum		Command;
	char					ClientData[_MAX_CLIENT_DATA_SIZE_];

	PG_RoleValue_CtoL_SetClientData( )
	{	Command = EM_PG_RoleValue_CtoL_SetClientData;	} 
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// °ò¥»¼Æ­È§ïÅÜ®t­È
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
//ClientºÝ­n¨D ´«¥­§ÎªÅ¶¡
struct PG_RoleValue_CtoL_ChangeParallel
{
	GamePGCommandEnum		Command;
	int						ParallelID;

	PG_RoleValue_CtoL_ChangeParallel( )
	{	Command = EM_PG_RoleValue_CtoL_ChangeParallel;	} 
};
//-----------------------------------------------------------------------------------------
//Client -> Local
//¨ú®ø´«¥­§ÎªÅ¶¡
struct PG_RoleValue_CtoL_CancelChangeParallel
{
	GamePGCommandEnum		Command;

	PG_RoleValue_CtoL_CancelChangeParallel( )
	{	Command = EM_PG_RoleValue_CtoL_CancelChangeParallel;	} 
};
//-----------------------------------------------------------------------------------------
//Local -> Client
//³qª¾µ¥«Ý´«¥­¦æªÅ¶¡
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
//³qª¾­n¨DClient ªºÀô¹Ò¸ê®Æ
struct PG_RoleValue_LtoC_ClientComputerInfoRequest
{
	GamePGCommandEnum	Command;
	

	PG_RoleValue_LtoC_ClientComputerInfoRequest( )
	{	Command = EM_PG_RoleValue_LtoC_ClientComputerInfoRequest;	}
};
//-----------------------------------------------------------------------------------------
// Client -> Local
//³qª¾Client ªºÀô¹Ò¸ê®Æ
struct PG_RoleValue_CtoL_ClientComputerInfo
{
	GamePGCommandEnum	Command;
	ClientComputerInfoStruct Info;

	PG_RoleValue_CtoL_ClientComputerInfo( )
	{	Command = EM_PG_RoleValue_CtoL_ClientComputerInfo;	}
};
//-----------------------------------------------------------------------------------------
//	³qª¾Client ºÝ¬Yª«¥óªºMode
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
//Clientªºª¬ºA¸ê®Æ
struct PG_RoleValue_CtoL_ClientEnvironmentState
{
	GamePGCommandEnum	Command;
	ClientStateStruct	State;

	PG_RoleValue_CtoL_ClientEnvironmentState( )
	{	Command = EM_PG_RoleValue_CtoL_ClientEnvironmentState;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// ³qª¾¬Y¤@­Óª«¥óªº¨¤¦â¸ê®Æ
struct PG_RoleValue_LtoC_ObjRoleInfo
{
	GamePGCommandEnum	Command;
	PlayerRoleData		Role;

	PG_RoleValue_LtoC_ObjRoleInfo( )
	{	Command = EM_PG_RoleValue_LtoC_ObjRoleInfo;	}
};

#pragma managed
