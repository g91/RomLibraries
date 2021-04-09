#ifndef __PG_CLICONNECT_H__
#define __PG_CLICONNECT_H__
#pragma warning (disable:4949)
#include "NetWakerPGEnum.h"
#include "roledata/RoleDataEx.h"
#pragma unmanaged
//----------------------------------------------------------------------------------------

//Local Server -> Client
//玩家角色區域ID
struct PG_CliConnect_LtoC_PlayerItemID
{
    GamePGCommandEnum	Command;
    myInt32	GItemID;
    myInt32 WorldGUID;
    myInt32 DBID;

    PG_CliConnect_LtoC_PlayerItemID()
    {
        Command = EM_PG_CliConnect_LtoC_PlayerItemID;
    }
};
//----------------------------------------------------------------------------------------
struct ZoneInfoModeStruct
{
	union
	{
		int Mode;
		struct  
		{
			bool				IsPvE							: 1;	//pk模式
			bool				IsBattleGround					: 1;	//強制背負旗子的圖形代碼, 若為 0 則不顯示
			bool				IsDisablePVPRule				: 1;	//仇人名單不記錄
			bool				IsDisableTrade					: 1;	//不可交易
			bool				IsDisableDual					: 1;	//不可比試
			bool				IsDisableSendGift				: 1;	//不可送禮			
			bool				AcAccountMoneyTrade				: 1;	//AC 不可交易帳號幣
			bool				AcItemAccountMoneyTrade			: 1;	//AC物品 不可交易帳號幣
			bool				IsZonePartyEnabled				: 1;	//用新的Party模式
			bool				IsDisableParty					: 1;	//不能邀請組隊
			bool				IsNoSwimming					: 1;	//此區域不可游泳					
			bool				IsCheckHeight					: 1;	//此區域要高度檢查
		};
	};
};
//Local Server -> Client
 
struct PG_CliConnect_LtoC_ZoneInfo
{
    GamePGCommandEnum	Command;
    myInt16				ZoneID;			//區域id
	myInt16				MapID;			//區域id
	int					CampObjID;		//陣營表
	LanguageTypeENUM	LanguageType;

	ZoneInfoModeStruct  ZoneMode;
	/*
	bool				IsPvE;			//pk模式
	bool				IsBattleGround;	// 強制背負旗子的圖形代碼, 若為 0 則不顯示
*/
	int					BG_iNumTeam;
	int					BG_iNumTeamPeople;

    PG_CliConnect_LtoC_ZoneInfo()
    {
        Command = EM_PG_CliConnect_LtoC_ZoneInfo;
    }
};
//----------------------------------------------------------------------------------------
//Local Server -> Client
//傳送角色基本資料
struct PG_CliConnect_LtoC_PlayerData
{
    GamePGCommandEnum	Command;

    //角色基本資料(Client 端需要用到的部分)
    BaseRoleData		BaseData;
	PlayerBaseRoleData	PlayerBaseData;
	SysBaseRoleData		Base;

//	int		LearnMagicID;
//	SystemVersionENUM	Version;

    PG_CliConnect_LtoC_PlayerData()
    {
        Command = EM_PG_CliConnect_LtoC_PlayerData;
    }
};
//----------------------------------------------------------------------------------------
//Local Server -> Client
struct PG_CliConnect_LtoC_SysVersionInfo
{
	GamePGCommandEnum	Command;

	int					LearnMagicID;
	SystemVersionENUM	Version;
	float				PK_DotRate;		//PK dot 的傷害修正百分比

	PG_CliConnect_LtoC_SysVersionInfo()
	{
		Command = EM_PG_CliConnect_LtoC_SysVersionInfo;
	}
};
//----------------------------------------------------------------------------------------
//Local Server -> Client 
//通知角色換區(鎖所有的角色公能)
struct PG_CliConnect_LtoC_PrepareChangeZone
{
    GamePGCommandEnum	Command;

    PG_CliConnect_LtoC_PrepareChangeZone()
    {
        Command = EM_PG_CliConnect_LtoC_PrepareChangeZone;
    }
};
//----------------------------------------------------------------------------------------
//Local Server -> Client 
//通知角色換區完成
struct PG_CliConnect_LtoC_ChangeZoneOK
{
    GamePGCommandEnum	Command;

    PG_CliConnect_LtoC_ChangeZoneOK()
    {
        Command = EM_PG_CliConnect_LtoC_ChangeZoneOK;
    }
};
//----------------------------------------------------------------------------------------
//Client -> Server
//要求登出
struct PG_CliConnect_CtoL_RequestLogout
{
    GamePGCommandEnum	Command;

    PG_CliConnect_CtoL_RequestLogout()
    {
        Command = EM_PG_CliConnect_CtoL_RequestLogout;
    }
};
//----------------------------------------------------------------------------------------
//Server -> Client
//要求登出
struct PG_CliConnect_LtoC_LogoutResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_CliConnect_LtoC_LogoutResult()
	{
		Command = EM_PG_CliConnect_LtoC_LogoutResult;
	}
};
//----------------------------------------------------------------------------------------
//換區通知
// Local -> Local
struct PG_CliConnect_LtoL_ChangeZoneNodify
{
    GamePGCommandEnum	Command;
	int					SrcZoneID;

    //RoleDataEx          Role;
	PlayerRoleData		Role;

    PG_CliConnect_LtoL_ChangeZoneNodify()
    {
        Command = EM_PG_CliConnect_LtoL_ChangeZoneNodify;
    }
};
//----------------------------------------------------------------------------------------
enum	ChangeZoneNodifyResult_ENUM
{
	EM_ChangeZoneNodifyResult_OK						,
	EM_ChangeZoneNodifyResult_PraviteZone_NoRoom		,
	EM_ChangeZoneNodifyResult_Failed					,
	EM_ChangeZoneNodifyResult_NoBattleGroundData		,
	EM_ChangeZoneNodifyResult_InstancePartyKeyErr		,
	EM_ChangeZoneNodifyResult_InstancePartyKey_Duplicate,
};

//換區通知
// Local -> Local
struct PG_CliConnect_LtoL_ChangeZoneNodifyResult
{
	GamePGCommandEnum	Command;

	int		GItemID;
	int		ZoneID;
	float	X , Y , Z;
	ChangeZoneNodifyResult_ENUM Result;

	PG_CliConnect_LtoL_ChangeZoneNodifyResult()
	{
		Command = EM_PG_CliConnect_LtoL_ChangeZoneNodifyResult;
	}
};
//----------------------------------------------------------------------------------------
// Server -> Client
//修改物件資訊

struct PG_CliConnect_LtoC_ModifyNPCInfo
{
    GamePGCommandEnum	Command;
    ModifyNPCInfo       Info;

    PG_CliConnect_LtoC_ModifyNPCInfo()
    {
        Command = EM_PG_CliConnect_LtoC_ModifyNPCInfo;
    }
};
//----------------------------------------------------------------------------------------
// Client -> Server
// 要求修改
struct PG_CliConnect_CtoL_ModifyNPCInfo
{
    GamePGCommandEnum	Command;
    ModifyNPCInfo       Info;

    PG_CliConnect_CtoL_ModifyNPCInfo()
    {
        Command = EM_PG_CliConnect_CtoL_ModifyNPCInfo;
    }
};
//----------------------------------------------------------------------------------------
// Client -> Local 
//要求取得某NPC的移動資訊
struct PG_CliConnect_CtoL_GetNPCMoveInfoRequest
{
	GamePGCommandEnum	Command;
	int					GUID;

	PG_CliConnect_CtoL_GetNPCMoveInfoRequest()
	{
		Command = EM_PG_CliConnect_CtoL_GetNPCMoveInfoRequest;
	}
};
//----------------------------------------------------------------------------------------
// Local -> Client 
// NPC的移動資訊
struct PG_CliConnect_LtoC_GetNPCMoveInfoResult
{
	GamePGCommandEnum	Command;
	int		TotalCount;
	int		GUID;			//NPCDBID
	int		IndexID;		//位置
	NPC_MoveBaseStruct Base;

	PG_CliConnect_LtoC_GetNPCMoveInfoResult()
	{
		Command = EM_PG_CliConnect_LtoC_GetNPCMoveInfoResult;
	}
};
//----------------------------------------------------------------------------------------
// Client -> Local
//----------------------------------------------------------------------------------------
struct PG_CliConnect_CtoL_SaveNPCMoveInfo
{
	GamePGCommandEnum	Command;
	int		TotalCount;
	int		GUID;			//NPCDBID
	int		IndexID;		//位置
	NPC_MoveBaseStruct Base;

	PG_CliConnect_CtoL_SaveNPCMoveInfo()
	{
		Command = EM_PG_CliConnect_CtoL_SaveNPCMoveInfo;
	}
};

//----------------------------------------------------------------------------------------
// Client -> Local 
//要求取得 Server Flag 清單
struct PG_CliConnect_CtoL_GetFlagList
{
	GamePGCommandEnum	Command;

	PG_CliConnect_CtoL_GetFlagList()
	{
		Command = EM_PG_CliConnect_CtoL_GetFlagList;
	}
};

//----------------------------------------------------------------------------------------
struct PG_CliConnect_LtoC_GetFlagListResult
{
	GamePGCommandEnum	Command;
	
	int					iTotalGroup;
	
	// 以下資料持續重覆, 並以 0 為一段資料重覆的 END

	int					iCount;

	int					ObjID;      //類別
	int					ID;         //編號
	float				X;
	float				Y;
	float				Z;    
	float				Dir;
	int					FlagDBID;	//旗子資料庫ID

	PG_CliConnect_LtoC_GetFlagListResult()
	{
		Command = EM_PG_CliConnect_LtoC_GetFlagListResult;
	}
};
//----------------------------------------------------------------------------------------
struct PG_CliConnect_CtoL_EditFlag
{
	GamePGCommandEnum	Command;

	int					ObjID;      //類別
	int					ID;         //編號
	float				X;
	float				Y;
	float				Z;    
	float				Dir;
	int					FlagDBID;	//旗子資料庫ID

	PG_CliConnect_CtoL_EditFlag()
	{
		Command = EM_PG_CliConnect_CtoL_EditFlag;
	}
};
//----------------------------------------------------------------------------------------
struct PG_CliConnect_LtoC_EditFlagResult
{
	GamePGCommandEnum	Command;
	int		iResult;

	PG_CliConnect_LtoC_EditFlagResult()
	{
		Command = EM_PG_CliConnect_LtoC_EditFlagResult;
	}
};
//----------------------------------------------------------------------------------------
struct PG_CliConnect_CtoL_DelFlag
{
	GamePGCommandEnum	Command;

	int					GroupID;      //類別
	int					FlagID;         //編號

	PG_CliConnect_CtoL_DelFlag()
	{
		Command = EM_PG_CliConnect_CtoL_DelFlag;
	}
};
//----------------------------------------------------------------------------------------
struct PG_CliConnect_LtoC_DelFlagResult
{
	GamePGCommandEnum	Command;
	int		iResult;

	PG_CliConnect_LtoC_DelFlagResult()
	{
		Command = EM_PG_CliConnect_LtoC_DelFlagResult;
	}
};
//----------------------------------------------------------------------------------------
struct PG_CliConnect_CtoL_DelFlagGroup
{
	GamePGCommandEnum	Command;

	int					iFlagGroupID;      //類別

	PG_CliConnect_CtoL_DelFlagGroup()
	{
		Command = EM_PG_CliConnect_CtoL_DelFlagGroup;
	}
};
//----------------------------------------------------------------------------------------
struct PG_CliConnect_LtoC_DelFlagGroupResult
{
	GamePGCommandEnum	Command;
	int		iResult;

	PG_CliConnect_LtoC_DelFlagGroupResult()
	{
		Command = EM_PG_CliConnect_LtoC_DelFlagGroupResult;
	}
};
//----------------------------------------------------------------------------------------
struct PG_CliConnect_LtoC_ParalledID
{
	GamePGCommandEnum	Command;
	int					ParalledID;

	PG_CliConnect_LtoC_ParalledID()
	{
		Command = EM_PG_CliConnect_LtoC_ParalledID;
	}
};
//----------------------------------------------------------------------------------------
struct PG_CliConnect_LtoC_PlayerData_Zip
{
	GamePGCommandEnum	Command;
	int					Size;
	char				Data[0x20000];

	int	PGSize()
	{
		return sizeof(int)*2+ Size;
	}
	PG_CliConnect_LtoC_PlayerData_Zip()
	{
		Command = EM_PG_CliConnect_LtoC_PlayerData_Zip;
	}
};

//----------------------------------------------------------------------------------------
//刪除墓碑
//Local -> Local
struct PG_CliConnect_LtoL_DelTomb
{
	GamePGCommandEnum	Command;
	int					TombGUID;
	int					OwnerDBID;

	PG_CliConnect_LtoL_DelTomb()
	{
		Command = EM_PG_CliConnect_LtoL_DelTomb;
	}
};

//////////////////////////////////////////////////////////////////////////
// 特殊的封包
//////////////////////////////////////////////////////////////////////////
//換到 另外一個 World 的戰場Server
//Local -> Battle Server
struct PG_CliConnect_LtoBL_ChangeZoneToOtherWorld
{
	GamePGCommandEnum	Command;
//	int					GSrvID;
	int					RoleSize;
	PlayerRoleData		Role;


	PG_CliConnect_LtoBL_ChangeZoneToOtherWorld()
	{
		Command = EM_PG_CliConnect_LtoBL_ChangeZoneToOtherWorld;
	}
};

//換到 另外一個 World 的戰場Server
//Local -> Battle Server
struct PG_CliConnect_LtoBL_ChangeZoneToOtherWorld_Zip
{
	GamePGCommandEnum	Command;
	int					RoleSize;
	int					Size;
	char				Data[0x20000];

	int	PGSize()
	{
		return sizeof(int)*3+ Size;
	}


	PG_CliConnect_LtoBL_ChangeZoneToOtherWorld_Zip()
	{
		Command = EM_PG_CliConnect_LtoBL_ChangeZoneToOtherWorld_Zip;
	}
};

//----------------------------------------------------------------------------------------
enum ChangeZoneToOtherWorldResultENUM
{
	EM_ChangeZoneToOtherWorldResult_OK		,
	EM_ChangeZoneToOtherWorldResult_Failed	,
	EM_ChangeZoneToOtherWorldResult_Full	,	//人數已滿
	
};
//要求換區OK 
//Battle Server -> Local
struct PG_CliConnect_BLtoL_ChangeZoneToOtherWorldResult
{
	GamePGCommandEnum	Command;
	int					RoleDBID;
	char				IPStr[64];
	int					Port;
	ChangeZoneToOtherWorldResultENUM				Result;

	PG_CliConnect_BLtoL_ChangeZoneToOtherWorldResult()
	{
		Command = EM_PG_CliConnect_BLtoL_ChangeZoneToOtherWorldResult;
	}
};
//----------------------------------------------------------------------------------------
//Local -> Client 連到 Battle World
struct PG_CliConnect_LtoC_ConnectToBattleWorld
{
	GamePGCommandEnum	Command;
	char				IPStr[64];
	int					Port;

	PG_CliConnect_LtoC_ConnectToBattleWorld()
	{
		Command = EM_PG_CliConnect_LtoC_ConnectToBattleWorld;
	}
};
//----------------------------------------------------------------------------------------
 
struct PG_CliConnect_CtoL_ConnectToBattleWorldOK
{
	GamePGCommandEnum	Command;
	int					CliNetID;

	PG_CliConnect_CtoL_ConnectToBattleWorldOK()
	{
		Command = EM_PG_CliConnect_CtoL_ConnectToBattleWorldOK;
	}
};
//----------------------------------------------------------------------------------------
//Local -> Battle Server 要求與Client 成立連線
struct PG_CliConnect_LtoBL_ConnectCliAndGSrv
{
	GamePGCommandEnum	Command;
//	int					GSrvID;
	int					CliNetID;

	PG_CliConnect_LtoBL_ConnectCliAndGSrv()
	{
		Command = EM_PG_CliConnect_LtoBL_ConnectCliAndGSrv;
	}
};
//----------------------------------------------------------------------------------------
//Battle Server -> Local 角色資料登出回原Server
struct PG_CliConnect_BLtoL_RetNomorlWorld
{
	GamePGCommandEnum	Command;
	int		PlayerDBID;
	PlayerRoleData	Role;

	PG_CliConnect_BLtoL_RetNomorlWorld()
	{
		Command = EM_PG_CliConnect_BLtoL_RetNomorlWorld;
	}
};

struct PG_CliConnect_BLtoL_RetNomorlWorld_Zip
{
	GamePGCommandEnum	Command;
	int			PlayerDBID;
	int			Size;
	char		Data[0x20000];

	int	PGSize()
	{
		return sizeof(int)*3+ Size;
	}

	PG_CliConnect_BLtoL_RetNomorlWorld_Zip()
	{
		Command = EM_PG_CliConnect_BLtoL_RetNomorlWorld_Zip;
	}
};
//----------------------------------------------------------------------------------------
//Battle Server -> Local 角色資料登出回原Server
struct PG_CliConnect_BLtoL_BattleSavePlayer
{
	GamePGCommandEnum	Command;
	int		PlayerDBID;
	PlayerRoleData	Role;

	PG_CliConnect_BLtoL_BattleSavePlayer()
	{
		Command = EM_PG_CliConnect_BLtoL_BattleSavePlayer;
	}
};

struct PG_CliConnect_BLtoL_BattleSavePlayer_Zip
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					Size;
	char				Data[0x20000];

	int	PGSize()
	{
		return sizeof(int)*3+ Size;
	}

	PG_CliConnect_BLtoL_BattleSavePlayer_Zip()
	{
		Command = EM_PG_CliConnect_BLtoL_BattleSavePlayer_Zip;
	}
};
//----------------------------------------------------------------------------------------
enum	ClientSkyProcTypeENUM
{
	EM_ClientSkyProcType_Normal		,	//一般
	EM_ClientSkyProcType_Day		,	//白天
	EM_ClientSkyProcType_NightFall	,	//黃昏
	EM_ClientSkyProcType_Night		,	//晚上
	EM_ClientSkyProcType_Dawn		,	//黎明
};

//Local -> Client 角色資料登出回原Server
struct PG_CliConnect_LtoC_SkyProcType
{
	GamePGCommandEnum	Command;
	ClientSkyProcTypeENUM Type;

	PG_CliConnect_LtoC_SkyProcType()
	{
		Command = EM_PG_CliConnect_LtoC_SkyProcType;
	}
};
//----------------------------------------------------------------------------------------
//Server -> Local 觸摸墓碑
struct PG_CliConnect_LtoC_TouchTomb
{
	GamePGCommandEnum	Command;

	PG_CliConnect_LtoC_TouchTomb()
	{
		Command = EM_PG_CliConnect_LtoC_TouchTomb;
	}
};
//----------------------------------------------------------------------------------------
//Server -> Local 墓碑資料
struct PG_CliConnect_LtoC_TombInfo
{
	GamePGCommandEnum	Command;
	int					CreateTime;
	int					ZoneID;
	float				X , Y , Z;
	int					DebtExp;
	int					DebtTp;
	int					Exp;


	PG_CliConnect_LtoC_TombInfo()
	{
		Command = EM_PG_CliConnect_LtoC_TombInfo;
	}
};
//----------------------------------------------------------------------------------------
//Server -> Client 與Battle Ground Server 終止封包 準備換回原Server
struct PG_CliConnect_LtoC_WorldReturnNotify
{
	GamePGCommandEnum	Command;
	int			ZoneID;				//檢查是否在此ZoneID , 否則不處理

	PG_CliConnect_LtoC_WorldReturnNotify()
	{
		Command = EM_PG_CliConnect_LtoC_WorldReturnNotify;
	}
};
//----------------------------------------------------------------------------------------
//Client -> Server 與Battle Ground Server 終止封包 OK
struct PG_CliConnect_CtoL_WorldReturnOK
{
	GamePGCommandEnum	Command;

	PG_CliConnect_CtoL_WorldReturnOK()
	{
		Command = EM_PG_CliConnect_CtoL_WorldReturnOK;
	}
};
//----------------------------------------------------------------------------------------
//跨Server轉送封包給某個玩家
//Local <-> Battle Server
struct PG_CliConnect_BLtoBL_SendToPlayerByDBID
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					DataSize;
	char				Data[0x10000];

	PG_CliConnect_BLtoBL_SendToPlayerByDBID()
	{
		Command = EM_PG_CliConnect_BLtoBL_SendToPlayerByDBID;
	}
	int PGSize()
	{
		return sizeof(*this)- sizeof(Data) + DataSize;
	}
};
//----------------------------------------------------------------------------------------
//Local -> Client 角色資料登出回原Server
struct PG_CliConnect_LtoC_ZoneOpenState
{
	GamePGCommandEnum	Command;
	int					ZoneID;
	bool				IsOpen;

	PG_CliConnect_LtoC_ZoneOpenState()
	{
		Command = EM_PG_CliConnect_LtoC_ZoneOpenState;
	}
};
//----------------------------------------------------------------------------------------/
//通知所在戰場
//Battle Server -> Local
struct PG_CliConnect_BLtoL_BattleZone
{
	GamePGCommandEnum	Command;
	int					ZoneID;
	int					PlayerDBID;
	PG_CliConnect_BLtoL_BattleZone()
	{
		Command = EM_PG_CliConnect_BLtoL_BattleZone;
	}
};

//要求把戰場角色刪除
//Local -> Battle Server
struct PG_CliConnect_LtoBL_ReturnWorldRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					TestCount;

	PG_CliConnect_LtoBL_ReturnWorldRequest()
	{
		Command = EM_PG_CliConnect_LtoBL_ReturnWorldRequest;
	}
};

//回應刪除結果
//Battle Server -> Local
struct PG_CliConnect_BLtoL_ReturnWorldResult
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					TestCount;
	bool				Result;

	PG_CliConnect_BLtoL_ReturnWorldResult()
	{
		Command = EM_PG_CliConnect_BLtoL_ReturnWorldResult;
	}
};


#pragma managed
#endif //__PG_CLICONNECT_H__