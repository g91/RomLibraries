#pragma once

#include "RoleData\RoleDataEx.h"
#include "NetWakerPGEnum.h"

//-------------------------------------------------------------------------------------------------------------------
//log種類
enum LogTypeEnum
{
	EM_LogType_None					= 0,
	EM_LogType_GMToolsGMCommand		,
	EM_LogType_GMCommand			,
	EM_LogType_ItemTrade			,
	EM_LogType_ItemDestroy			,
	EM_LogType_Money				,
	EM_LogType_Money_Account		,
	EM_LogType_Money_Bonus			,
	EM_LogType_PlayerActionLog		,
	EM_LogType_Level				,
	EM_LogType_Dead					,
	EM_LogType_Quest				,
	EM_LogType_ItemServerLog		,
	EM_LogType_EnterZone			,
	EM_LogType_LeaveZone			,
	EM_LogType_AddExp				,
	EM_LogType_ServerStatus			,
	EM_LogType_DepartmentStore		,
	EM_LogType_Design				,
	EM_LogType_SkillLv				,
	EM_LogType_ClientComputerInfo	,
	EM_LogType_Talk					,
	EM_LogType_ZoneMoneyLog			,
	EM_LogType_Warning_MoneyVendor	,
	EM_LogType_Warning_Bot			,
	EM_LogType_OfflineMsg			,
	EM_LogType_Verion				,
	EM_LogType_House
};

struct LOG_DATA_MONEY_ACCOUNT
{
	StaticString< _MAX_ACCOUNT_SIZE_ >   From_Account_ID;
	StaticString< _MAX_ACCOUNT_SIZE_ >   To_Account_ID;
	int WorldID;
	int TargetNPCDBID;
	int TargetPlayerDBID;
	int PlayerDBID;
	int ActionType;
	int ZoneID;
	int PosX;
	int PosZ;
	int NewAccountMoney;
	int OldAccountMoney;
	int Lv;
	int Cost_Lock;
	int Money_Lock_Remaining;
	int Time;

	LOG_DATA_MONEY_ACCOUNT()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct LOG_DATA_ITEM_TRADE
{
	StaticString< _MAX_ACCOUNT_SIZE_ >   From_Account_ID;
	StaticString< _MAX_ACCOUNT_SIZE_ >   To_Account_ID;
	int WorldID;
	int TargetNPCDBID;
	int TargetPlayerDBID;
	int PlayerDBID;
	int ActionType;
	int ZoneID;
	int PosX;
	int PosZ;
	int ItemID;
	int ItemSerial;
	int ItemCreateTime;
	int ItemCount;
	int ItemExValue;
	int ItemDurable;
	int ItemMode;
	int ItemImageObjectID;
	char ItemAbility[32];

	LOG_DATA_ITEM_TRADE()
	{
		memset(this, 0, sizeof(*this));
	}
};

//對應departmentstore log
struct LOG_DATA_DEPARTMENT_STORE
{
	StaticString< _MAX_ACCOUNT_SIZE_ >   Account_ID;	
	StaticString< 64 >   ItemName;	
	int Money_Account_Remaining;
	int Money_Bonus_Remaining;
	int Money_Free_Remaining;	//必爾汀代幣
	int Money_Lock_Remaining;
	int Type;
	int ItemID;
	int ItemVesion;				//序號
	int ItemCreateTime;
	int ItemCount;
	int Cost;
	int Cost_Bonus;
	int Cost_Free;
	int Cost_Lock;
	int WorldID;
	int PlayerDBID;
	int Client_Language;

	//for KR billing log
	int CurrencyType;
	int Discounted;
	char PromotionalEvent[32];

	LOG_DATA_DEPARTMENT_STORE()
	{
		memset(this, 0, sizeof(*this));
	}
};

//-------------------------------------------------------------------------------------------------------------------
//儲存角色資料
struct PG_DCBase_XtoRD_SavePlayer
{
    GamePGCommandEnum	Command;
	int					SaveTime;							//儲存時間
    int					DBID;								//資料庫編號		
    int					Start;								//開始位置
    int					Size;								//數量
    PlayerRoleData		Player;

    PG_DCBase_XtoRD_SavePlayer()
    {
        Command	= EM_PG_DCBase_XtoRD_SavePlayer;
    };
};
//-------------------------------------------------------------------------------------------------------------------
//儲存角色資料有問題
enum SavePlayerResultEnum
{
    EM_SavePlayer_PlayerNotFind = -1   ,
    EM_SavePlayer_OK            = 0    ,
};

struct PG_DBBase_RDtoX_SavePlayerError
{
    GamePGCommandEnum	    Command;
    int                     DBID;
    SavePlayerResultEnum    Result;

    PG_DBBase_RDtoX_SavePlayerError()
    {
        Command	= EM_PG_DBBase_RDtoX_SavePlayerError;
    };    
};
//-------------------------------------------------------------------------------------------------------------------
//儲存新的NPC資料

struct PG_DCBase_LtoD_SaveNPCRequest
{
    GamePGCommandEnum	Command;
    int					LocalObjID;
    RoleData			NPC;	

    PG_DCBase_LtoD_SaveNPCRequest()
    {
        Command	= EM_PG_DCBase_LtoD_SaveNPCRequest;
    }
};
//-------------------------------------------------------------------------------------------------------------------
enum SaveNewNPCResultEnum
{
    EM_SaveNPC_Failed = -1   ,
    EM_SaveNPC_OK     = 0    ,
};

//回應儲存後的NPC的DBID
struct PG_DCBase_DtoL_SaveNPCResult
{
    GamePGCommandEnum	    Command;
    int					    LocalObjID;
    int                     NPCDBID;
    SaveNewNPCResultEnum    Result;

    PG_DCBase_DtoL_SaveNPCResult()
    {
        Command	= EM_PG_DCBase_DtoL_SaveNPCResult;
    }
};
//-------------------------------------------------------------------------------------------------------------------
//刪除NPC資料
struct PG_DBBase_LtoD_DropNPCRequest
{
    GamePGCommandEnum	Command;
    int					LocalObjID;
    int					DBID;
	bool				IsDelFlag;
	StaticString< _MAX_NAMERECORD_SIZE_ >   DestroyAccount;	    
    PG_DBBase_LtoD_DropNPCRequest()
    {
        Command	= EM_PG_DBBase_LtoD_DropNPCRequest;
    }
};
//----------------------------------------------------------------------------------------
//刪除NPC資料
enum DropNPCResultEnum
{
    EM_DropNPC_NotFind      = -1   ,
    EM_DropNPC_OK           = 0    ,
};

struct PG_DBBase_DtoL_DropNPCResult
{
    GamePGCommandEnum	Command;
    int					LocalObjID;
    int					DBID;
    DropNPCResultEnum   Result;


    PG_DBBase_DtoL_DropNPCResult()
    {
        Command	= EM_PG_DBBase_DtoL_DropNPCResult;
    }
};
//-------------------------------------------------------------------------------------------------------------------
//載入NPC資料
struct PG_DBBase_LtoD_LoadAllNPC
{
    GamePGCommandEnum	Command;
    int					ZoneID;
    PG_DBBase_LtoD_LoadAllNPC()
    {
        Command	= EM_PG_DBBase_LtoD_LoadAllNPC;
    }
};
//----------------------------------------------------------------------------------------
//載入NPC資料
struct PG_DBBase_DtoL_LoadAllNPCResult
{
    GamePGCommandEnum	Command;
    int                 TotalCount; //全部有多少筆
    int                 ID;         //目前為第幾筆
    NPCData				NPC;
    PG_DBBase_DtoL_LoadAllNPCResult()
    {
        Command	= EM_PG_DBBase_DtoL_LoadAllNPCResult;
    };
};

//-------------------------------------------------------------------------------------------------------------------
//設定某一個角色在DB的生存期
struct PG_DBBase_XtoRD_SetPlayerLiveTime
{
    GamePGCommandEnum	Command;

    int         DBID;
    int         LiveTime;           //(1/1000 秒)為單位
    PG_DBBase_XtoRD_SetPlayerLiveTime()
    {
        Command	= EM_PG_DBBase_XtoRD_SetPlayerLiveTime;
    };
};
//-------------------------------------------------------------------------------------------------------------------
//生存期設定失敗回應
enum SetPlayerLiveTimeResultEnum
{
    EM_SetPlayerLiveTime_PlayerNotFind = -1   ,
    EM_SetPlayerLiveTime_OK            = 0    ,
};

struct PG_DBBase_RDtoX_SetPlayerLiveTimeError
{
    GamePGCommandEnum	Command;    
    int                 DBID;
    SetPlayerLiveTimeResultEnum Result;

    PG_DBBase_RDtoX_SetPlayerLiveTimeError()
    {
        Command	= EM_PG_DBBase_RDtoX_SetPlayerLiveTimeError;
    };
};
//-------------------------------------------------------------------------------------------------------------------
//設定某一個角色在DB的生存期
struct PG_DBBase_XtoRD_SetPlayerLiveTime_ByAccount
{
    GamePGCommandEnum	Command;

    char        Account[_MAX_ACCOUNT_SIZE_];
    int         LiveTime;           //(1/1000 秒)為單位
    PG_DBBase_XtoRD_SetPlayerLiveTime_ByAccount()
    {
        Command	= EM_PG_DBBase_XtoRD_SetPlayerLiveTime_ByAccount;
    };
};
//-------------------------------------------------------------------------------------------------------------------
//設定某一個角色在DB的生存期失敗
struct PG_DBBase_RDtoX_SetPlayerLiveTimeError_ByAccount
{
    GamePGCommandEnum	Command;    
    char        Account[_MAX_ACCOUNT_SIZE_];
    SetPlayerLiveTimeResultEnum Result;

    PG_DBBase_RDtoX_SetPlayerLiveTimeError_ByAccount()
    {
        Command	= EM_PG_DBBase_RDtoX_SetPlayerLiveTimeError_ByAccount;
    };
};
//-------------------------------------------------------------------------------------------------------------------
//要求載入所有npc移動位置的資訊
//Local -> DataCenter
struct PG_DBBase_LtoD_LoadAllNPCMoveInfo
{
	GamePGCommandEnum	Command;
	int ZoneID;
	PG_DBBase_LtoD_LoadAllNPCMoveInfo()
	{
		Command	= EM_PG_DBBase_LtoD_LoadAllNPCMoveInfo;
	}
};
//-------------------------------------------------------------------------------------------------------------------
//回應npc移動位置的資訊
//DataCenter -> Local
struct PG_DBBase_DtoL_NPCMoveInfoResult
{
	GamePGCommandEnum	Command;
	int		ZoneID;		//區域ID
	int		NPCDBID;	//NPCDBID
	int		TotalCount;	//總共有多少個
	int		IndexID;	//位置
	NPC_MoveBaseStruct Base;

	PG_DBBase_DtoL_NPCMoveInfoResult()
	{
		Command	= EM_PG_DBBase_DtoL_NPCMoveInfoResult;
	}
};
//-------------------------------------------------------------------------------------------------------------------
//要求儲存npc移動位置
//Local -> DataCenter
struct PG_DBBase_LtoD_SaveNPCMoveInfo
{
	GamePGCommandEnum	Command;

	int		TotalCount;
	int		ZoneID;		//區域ID
	int		NPCDBID;	//NPCDBID
	int		IndexID;	//位置
	NPC_MoveBaseStruct Base;

	PG_DBBase_LtoD_SaveNPCMoveInfo()
	{
		Command	= EM_PG_DBBase_LtoD_SaveNPCMoveInfo;
	}
};
//-------------------------------------------------------------------------------------------------------------------
enum SqlCommandTypeENUM
{
	EM_SqlCommandType_GlobalDB	,
	EM_SqlCommandType_ImportDB	,
	EM_SqlCommandType_WorldDB	,
	EM_SqlCommandType_WorldLogDB,	

};
//執行 SQL 
//Local -> DataCenter
struct PG_DBBase_LtoD_SqlCommand
{
	GamePGCommandEnum	Command;
	SqlCommandTypeENUM Type;
	int		ThreadID;
	int		Size;
	StaticString< 0x10000 >   Data;

	PG_DBBase_LtoD_SqlCommand()
	{
		Command	= EM_PG_DBBase_LtoD_SqlCommand;
	}

	int PGSize()
	{
		return Size + sizeof(*this) - sizeof( Data );
	}
};
/*
//-------------------------------------------------------------------------------------------------------------------
//執行 SQL 
//Local -> DataCenter
struct PG_DBBase_LtoD_LogSqlCommand
{
	GamePGCommandEnum	Command;
	int		Size;
	StaticString< 2048 >   Data;

	PG_DBBase_LtoD_LogSqlCommand()
	{
		Command	= EM_PG_DBBase_LtoD_LogSqlCommand;
	}

	int PGSize()
	{
		return Size + sizeof(this) - sizeof( Data );
	}
};
*/
//-------------------------------------------------------------------------------------------------------------------
//確認與DataCenter的角色資料大小是否相同
// Master or Local -> DataCenter
struct PG_DBBase_XtoRD_CheckRoleDataSize
{
	GamePGCommandEnum	Command;
	int		Size;

	PG_DBBase_XtoRD_CheckRoleDataSize()
	{
		Size = sizeof(PlayerRoleData);
		Command	= EM_PG_DBBase_XtoRD_CheckRoleDataSize;
	}

};
//-------------------------------------------------------------------------------------------------------------------
//確認與DataCenter的角色資料大小是否相同
//  DataCenter -> Master or Local
struct PG_DBBase_RDtoX_CheckRoleDataSizeResult
{
	GamePGCommandEnum	Command;
	bool	Result;

	PG_DBBase_RDtoX_CheckRoleDataSizeResult()
	{
		Command	= EM_PG_DBBase_RDtoX_CheckRoleDataSizeResult;
	}
};
//-------------------------------------------------------------------------------------------------------------------
// 通知區域共用資料
// Local -> Datacenter
struct PG_DBBase_LtoD_ZoneInfoRequest
{
	GamePGCommandEnum	Command;
	int	ZoneID;

	PG_DBBase_LtoD_ZoneInfoRequest()
	{
		Command	= EM_PG_DBBase_LtoD_ZoneInfoRequest;
	}
};
//-------------------------------------------------------------------------------------------------------------------
// 通知區域共用資料
// Datacenter -> Local
struct PG_DBBase_DtoL_ZoneInfoResult
{
	GamePGCommandEnum	Command;

	LocalServerBaseDBInfoStruct Info;

	PG_DBBase_DtoL_ZoneInfoResult()
	{
		Command	= EM_PG_DBBase_DtoL_ZoneInfoResult;
	}
};
//-------------------------------------------------------------------------------------------------------------------
// 通知全區域共用資料
// Datacenter -> Local
struct PG_DBBase_DtoL_GlobalZoneInfoResult
{
	GamePGCommandEnum	Command;

	GlobalServerDBInfoStruct Info;

	PG_DBBase_DtoL_GlobalZoneInfoResult()
	{
		Command	= EM_PG_DBBase_DtoL_GlobalZoneInfoResult;
	}
};
//-------------------------------------------------------------------------------------------------------------------
//區域共用資料
struct ZoneConfigBaseStruct
{
	int ZoneID;
	StaticString< 64 >   CmdStr;
	int	Value;
	StaticString< 256 >   Str;
};

// Datacenter -> Local
//通知 設定資料
struct PG_DBBase_DtoL_GlobalZoneInfoList
{
	GamePGCommandEnum	Command;

	int						Count;
	ZoneConfigBaseStruct	Info[1000];

	PG_DBBase_DtoL_GlobalZoneInfoList()
	{
		Command	= EM_PG_DBBase_DtoL_GlobalZoneInfoList;
	}
	int	PGSize()
	{
		return sizeof(*this)-sizeof(Info)+Count*sizeof(ZoneConfigBaseStruct);
	}
};
//-------------------------------------------------------------------------------------------------------------------
struct GmCommandStruct
{
	int		GUID;
	int		ManageLv;
	int		PlayerDBID;
	bool	IsSendOffline;
	char	Command[512];
};

//通知對某角色執行GM 命令
//DataCenter -> Local
struct PG_DBBase_DtoL_GmCommandRequest
{
	GamePGCommandEnum	Command;

	GmCommandStruct		Info;

	PG_DBBase_DtoL_GmCommandRequest()
	{
		Command	= EM_PG_DBBase_DtoL_GmCommandRequest;
	}
};
//-------------------------------------------------------------------------------------------------------------------
enum GmCommandResultENUM
{
	EM_GmCommandResult_Failed,
	EM_GmCommandResult_OK	,
	EM_GmCommandResult_OfflineMsg,
};
//通知對某角色執行GM 命令 結果
//Local -> DataCenter
struct PG_DBBase_LtoD_GmCommandResult
{
	GamePGCommandEnum	Command;

	int						GUID;		//命令ID
	GmCommandResultENUM		Result;		//
	char					ResultStr[512];

	PG_DBBase_LtoD_GmCommandResult()
	{
		Command	= EM_PG_DBBase_LtoD_GmCommandResult;
	}
};
//-------------------------------------------------------------------------------------------------------------------
//確認要刪的問品是否存在
//DataCenter -> Local
struct PG_DBBase_DtoL_CheckRoleLiveTime
{
	GamePGCommandEnum	Command;
	int					DBID;
	
	PG_DBBase_DtoL_CheckRoleLiveTime()
	{
		Command	= EM_PG_DBBase_DtoL_CheckRoleLiveTime;
	}
};
//-------------------------------------------------------------------------------------------------------------------
//角色儲存結構比對
struct PG_DBBase_RDtoX_CheckRoleStruct
{
	GamePGCommandEnum	Command;
	int					Count;

	CheckKeyAddressStruct	Info[1000];

	PG_DBBase_RDtoX_CheckRoleStruct()
	{
		Command	= EM_PG_DBBase_RDtoX_CheckRoleStruct;
	}

	int Size()
	{
		return sizeof(int)*2+Count*( 64 + sizeof(int ) );
	}
};
//-------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------
//log data
struct PG_DBBase_LtoD_LogData
{
	GamePGCommandEnum	Command;
	int					LogType;
	unsigned long		DataSize;

	PG_DBBase_LtoD_LogData()
	{
		memset(this, 0, sizeof(*this));
		Command	= EM_PG_DBBase_LtoD_LogData;		
	}

	//------------------------
	//packet format
	//------------------------
	//PG_DBBase_LtoD_LogData
	//Log data struct, like LOG_DB_STRUCT_ITEM_TRADE... etc.
};
//-------------------------------------------------------------------------------------------------------------------
