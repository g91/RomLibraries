#pragma once

#include "RoleData\RoleDataEx.h"
#include "NetWakerPGEnum.h"

//-------------------------------------------------------------------------------------------------------------------
//log����
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

//����departmentstore log
struct LOG_DATA_DEPARTMENT_STORE
{
	StaticString< _MAX_ACCOUNT_SIZE_ >   Account_ID;	
	StaticString< 64 >   ItemName;	
	int Money_Account_Remaining;
	int Money_Bonus_Remaining;
	int Money_Free_Remaining;	//�����ťN��
	int Money_Lock_Remaining;
	int Type;
	int ItemID;
	int ItemVesion;				//�Ǹ�
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
//�x�s������
struct PG_DCBase_XtoRD_SavePlayer
{
    GamePGCommandEnum	Command;
	int					SaveTime;							//�x�s�ɶ�
    int					DBID;								//��Ʈw�s��		
    int					Start;								//�}�l��m
    int					Size;								//�ƶq
    PlayerRoleData		Player;

    PG_DCBase_XtoRD_SavePlayer()
    {
        Command	= EM_PG_DCBase_XtoRD_SavePlayer;
    };
};
//-------------------------------------------------------------------------------------------------------------------
//�x�s�����Ʀ����D
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
//�x�s�s��NPC���

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

//�^���x�s�᪺NPC��DBID
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
//�R��NPC���
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
//�R��NPC���
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
//���JNPC���
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
//���JNPC���
struct PG_DBBase_DtoL_LoadAllNPCResult
{
    GamePGCommandEnum	Command;
    int                 TotalCount; //�������h�ֵ�
    int                 ID;         //�ثe���ĴX��
    NPCData				NPC;
    PG_DBBase_DtoL_LoadAllNPCResult()
    {
        Command	= EM_PG_DBBase_DtoL_LoadAllNPCResult;
    };
};

//-------------------------------------------------------------------------------------------------------------------
//�]�w�Y�@�Ө���bDB���ͦs��
struct PG_DBBase_XtoRD_SetPlayerLiveTime
{
    GamePGCommandEnum	Command;

    int         DBID;
    int         LiveTime;           //(1/1000 ��)�����
    PG_DBBase_XtoRD_SetPlayerLiveTime()
    {
        Command	= EM_PG_DBBase_XtoRD_SetPlayerLiveTime;
    };
};
//-------------------------------------------------------------------------------------------------------------------
//�ͦs���]�w���Ѧ^��
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
//�]�w�Y�@�Ө���bDB���ͦs��
struct PG_DBBase_XtoRD_SetPlayerLiveTime_ByAccount
{
    GamePGCommandEnum	Command;

    char        Account[_MAX_ACCOUNT_SIZE_];
    int         LiveTime;           //(1/1000 ��)�����
    PG_DBBase_XtoRD_SetPlayerLiveTime_ByAccount()
    {
        Command	= EM_PG_DBBase_XtoRD_SetPlayerLiveTime_ByAccount;
    };
};
//-------------------------------------------------------------------------------------------------------------------
//�]�w�Y�@�Ө���bDB���ͦs������
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
//�n�D���J�Ҧ�npc���ʦ�m����T
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
//�^��npc���ʦ�m����T
//DataCenter -> Local
struct PG_DBBase_DtoL_NPCMoveInfoResult
{
	GamePGCommandEnum	Command;
	int		ZoneID;		//�ϰ�ID
	int		NPCDBID;	//NPCDBID
	int		TotalCount;	//�`�@���h�֭�
	int		IndexID;	//��m
	NPC_MoveBaseStruct Base;

	PG_DBBase_DtoL_NPCMoveInfoResult()
	{
		Command	= EM_PG_DBBase_DtoL_NPCMoveInfoResult;
	}
};
//-------------------------------------------------------------------------------------------------------------------
//�n�D�x�snpc���ʦ�m
//Local -> DataCenter
struct PG_DBBase_LtoD_SaveNPCMoveInfo
{
	GamePGCommandEnum	Command;

	int		TotalCount;
	int		ZoneID;		//�ϰ�ID
	int		NPCDBID;	//NPCDBID
	int		IndexID;	//��m
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
//���� SQL 
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
//���� SQL 
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
//�T�{�PDataCenter�������Ƥj�p�O�_�ۦP
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
//�T�{�PDataCenter�������Ƥj�p�O�_�ۦP
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
// �q���ϰ�@�θ��
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
// �q���ϰ�@�θ��
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
// �q�����ϰ�@�θ��
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
//�ϰ�@�θ��
struct ZoneConfigBaseStruct
{
	int ZoneID;
	StaticString< 64 >   CmdStr;
	int	Value;
	StaticString< 256 >   Str;
};

// Datacenter -> Local
//�q�� �]�w���
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

//�q����Y�������GM �R�O
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
//�q����Y�������GM �R�O ���G
//Local -> DataCenter
struct PG_DBBase_LtoD_GmCommandResult
{
	GamePGCommandEnum	Command;

	int						GUID;		//�R�OID
	GmCommandResultENUM		Result;		//
	char					ResultStr[512];

	PG_DBBase_LtoD_GmCommandResult()
	{
		Command	= EM_PG_DBBase_LtoD_GmCommandResult;
	}
};
//-------------------------------------------------------------------------------------------------------------------
//�T�{�n�R���ݫ~�O�_�s�b
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
//�����x�s���c���
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
