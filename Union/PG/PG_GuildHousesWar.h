#pragma once
/*
#include "RoleData\RoleDataEx.h"
#include "NetWakerPGEnum.h"

//公會屋戰

//1 註冊公會戰
//2 看目前公會戰註冊結果
//3 通知公會戰開打
//4 傳入公會戰場(只有一般野外能傳)

//////////////////////////////////////////////////////////////////////////
//Client ->Local 開啟註冊公會戰介面	
struct PG_GuildHousesWar_CtoL_OpenMenu
{
	GamePGCommandEnum	Command;

	PG_GuildHousesWar_CtoL_OpenMenu()
	{
		Command = EM_PG_GuildHousesWar_CtoL_OpenMenu;
	}
};

//////////////////////////////////////////////////////////////////////////
//Local->DataCenter 開啟註冊公會戰介面	
struct PG_GuildHousesWar_LtoD_OpenMenu
{
	GamePGCommandEnum	Command;
	int		PlayerDBID;

	PG_GuildHousesWar_LtoD_OpenMenu()
	{
		Command = EM_PG_GuildHousesWar_LtoD_OpenMenu;
	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Client 開啟註冊公會戰介面
struct PG_GuildHousesWar_DtoC_OpenMenu
{
	GamePGCommandEnum	Command;
	GuildHouseWarStateENUM	State;
	int						Size;
	GuildHouseWarInfoStruct List[1000];

	PG_GuildHousesWar_DtoC_OpenMenu()
	{
		Command = EM_PG_GuildHousesWar_DtoC_OpenMenu;
	}
	int PGSize()
	{
		return sizeof(*this) - sizeof(List) + Size * sizeof(GuildHouseWarInfoStruct);
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 	註冊公會戰
enum GuildHouseWarRegisterTypeENUM
{
	EM_GuildHouseWarRegisterType_Register	,
	EM_GuildHouseWarRegisterType_Cancel		,
};
struct PG_GuildHousesWar_CtoL_WarRegister
{
	GamePGCommandEnum	Command;
	GuildHouseWarRegisterTypeENUM Type;

	PG_GuildHousesWar_CtoL_WarRegister()
	{
		Command = EM_PG_GuildHousesWar_CtoL_WarRegister;
	}
};

//Local -> Datacenter 註冊公會戰
struct PG_GuildHousesWar_LtoD_WarRegister
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	GuildHouseWarRegisterTypeENUM Type;

	PG_GuildHousesWar_LtoD_WarRegister()
	{
		Command = EM_PG_GuildHousesWar_LtoD_WarRegister;
	}
};

//DataCenter -> Client 註冊公會戰結果
struct PG_GuildHousesWar_DtoC_WarRegisterResult
{
	GamePGCommandEnum	Command;
	GuildHouseWarRegisterTypeENUM Type;
	bool Result;

	PG_GuildHousesWar_DtoC_WarRegisterResult()
	{
		Command = EM_PG_GuildHousesWar_DtoC_WarRegisterResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local 公會戰開始
struct PG_GuildHousesWar_DtoL_WarBegin
{
	GamePGCommandEnum	Command;
	int					GuildID[2];

	PG_GuildHousesWar_DtoL_WarBegin()
	{
		Command = EM_PG_GuildHousesWar_DtoL_WarBegin;
	}
};
//Local -> Client 公會戰開始
struct PG_GuildHousesWar_LtoC_WarBegin
{
	GamePGCommandEnum	Command;
	int					GuildID[2];

	PG_GuildHousesWar_LtoC_WarBegin()
	{
		Command = EM_PG_GuildHousesWar_LtoC_WarBegin;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 傳入公會戰場
struct PG_GuildHousesWar_CtoL_EnterWar
{
	GamePGCommandEnum	Command;

	PG_GuildHousesWar_CtoL_EnterWar()
	{
		Command = EM_PG_GuildHousesWar_CtoL_EnterWar;
	}
};
//Local -> Client 傳入公會戰場結果
struct PG_GuildHousesWar_LtoC_EnterWarResult
{
	GamePGCommandEnum	Command;
	bool Result;

	PG_GuildHousesWar_LtoC_EnterWarResult()
	{
		Command = EM_PG_GuildHousesWar_LtoC_EnterWarResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 傳入出會戰場
struct PG_GuildHousesWar_CtoL_LeaveWar
{
	GamePGCommandEnum	Command;

	PG_GuildHousesWar_CtoL_LeaveWar()
	{
		Command = EM_PG_GuildHousesWar_CtoL_LeaveWar;
	}
};
//Local -> Client 傳入出會戰場結果
struct PG_GuildHousesWar_LtoC_LeaveWarResult
{
	GamePGCommandEnum	Command;
	bool Result;

	PG_GuildHousesWar_LtoC_LeaveWarResult()
	{
		Command = EM_PG_GuildHousesWar_LtoC_LeaveWarResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local 戰爭結束
struct PG_GuildHousesWar_DtoL_WarEnd
{
	GamePGCommandEnum	Command;

	PG_GuildHousesWar_DtoL_WarEnd()
	{
		Command = EM_PG_GuildHousesWar_DtoL_WarEnd;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter 戰爭結束
struct PG_GuildHousesWar_LtoD_WarEndOK
{
	GamePGCommandEnum	Command;

	PG_GuildHousesWar_LtoD_WarEndOK()
	{
		Command = EM_PG_GuildHousesWar_LtoD_WarEndOK;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client 戰爭結果
struct PG_GuildHousesWar_LtoC_EndWar
{
	GamePGCommandEnum	Command;

	int					GuildID[2];
	int					Score[2];

	PG_GuildHousesWar_LtoC_EndWar()
	{
		Command = EM_PG_GuildHousesWar_LtoC_EndWar;
	}
};

//Local -> Client 戰爭結果
struct PG_GuildHousesWar_LtoC_EndWarPlayerInfo
{
	GamePGCommandEnum	Command;
	
	int							Count;
	GuildWarPlayerInfoStruct	PlayerList[ 1000 ];

	PG_GuildHousesWar_LtoC_EndWarPlayerInfo()
	{
		Command = EM_PG_GuildHousesWar_LtoC_EndWarPlayerInfo;
	}
	int Size( )
	{
		return sizeof(*this)- sizeof(PlayerList) + Count * sizeof( PlayerList );
	}
};
//////////////////////////////////////////////////////////////////////////
//建立 戰場 公會屋
//////////////////////////////////////////////////////////////////////////
// Local -> DataCenter 要求公會屋資料資料
struct PG_GuildHousesWar_LtoD_LoadHouseBaseInfo
{
	GamePGCommandEnum	Command;
	PG_GuildHousesWar_LtoD_LoadHouseBaseInfo()
	{
		Command = EM_PG_GuildHousesWar_LtoD_LoadHouseBaseInfo;
	}
};

//DataCenter -> Local 公會屋資料資料
struct PG_GuildHousesWar_DtoL_HouseBaseInfo
{
	GamePGCommandEnum	Command;
	GuildHousesInfoStruct	HouseBaseInfo;

	PG_GuildHousesWar_DtoL_HouseBaseInfo()
	{
		Command = EM_PG_GuildHousesWar_DtoL_HouseBaseInfo;
	}
};
//-------------------------------------------------------------------------
//DataCenter -> Local 公會建築物資料
struct PG_GuildHousesWar_DtoL_BuildingInfo
{
	GamePGCommandEnum	Command;

	GuildHouseBuildingInfoStruct	Building;

	PG_GuildHousesWar_DtoL_BuildingInfo()
	{
		Command = EM_PG_GuildHousesWar_DtoL_BuildingInfo;
	}
};
//-------------------------------------------------------------------------
//DataCenter -> Local 通知公會屋讀取完畢			
struct PG_GuildHousesWar_DtoL_HouseInfoLoadOK
{
	GamePGCommandEnum	Command;
	int					GuildID;

	PG_GuildHousesWar_DtoL_HouseInfoLoadOK()
	{
		Command = EM_PG_GuildHousesWar_DtoL_HouseInfoLoadOK;
	}
};
//-------------------------------------------------------------------------
//DataCenter -> Local 飾品倉庫物品資料
struct PG_GuildHousesWar_DtoL_FurnitureItemInfo
{
	GamePGCommandEnum	Command;

	GuildHouseFurnitureItemStruct Item;

	PG_GuildHousesWar_DtoL_FurnitureItemInfo()
	{
		Command = EM_PG_GuildHousesWar_DtoL_FurnitureItemInfo;
	}
};
//------------------------------------------------------------------------
//Local -> Client 戰爭結果
struct PG_GuildHousesWar_LtoD_DebugTime
{
	GamePGCommandEnum	Command;

	int		Time_Hour;
	int		Time_Day;

	PG_GuildHousesWar_LtoD_DebugTime()
	{
		Command = EM_PG_GuildHousesWar_LtoD_DebugTime;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//要求領獎賞
struct PG_GuildHousesWar_CtoL_PricesRequst
{
	GamePGCommandEnum	Command;

	PG_GuildHousesWar_CtoL_PricesRequst()
	{
		Command = EM_PG_GuildHousesWar_CtoL_PricesRequst;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client
//要求領獎賞
struct PG_GuildHousesWar_LtoC_PricesResult
{
	GamePGCommandEnum	Command;
	bool				Result;
	
	PG_GuildHousesWar_LtoC_PricesResult()
	{
		Command = EM_PG_GuildHousesWar_LtoC_PricesResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client 要求歷史資料
//Client -> Local
struct PG_GuildHousesWar_CtoL_HistoryRequest
{
	GamePGCommandEnum	Command;
	int					DayBefore;

	PG_GuildHousesWar_CtoL_HistoryRequest()
	{
		Command = EM_PG_GuildHousesWar_CtoL_HistoryRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client 要求歷史資料
//Local -> DataCenter
struct PG_GuildHousesWar_LtoD_HistoryRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					DayBefore;

	PG_GuildHousesWar_LtoD_HistoryRequest()
	{
		Command = EM_PG_GuildHousesWar_LtoD_HistoryRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client 要求歷史資料回應
// DataCenter -> Client
struct PG_GuildHousesWar_DtoC_HistoryResult
{
	GamePGCommandEnum	Command;
	int					DayBefore;
	int					Count;
	GuildHouseWarHistoryStruct List[1000];

	PG_GuildHousesWar_DtoC_HistoryResult()
	{
		Command = EM_PG_GuildHousesWar_DtoC_HistoryResult;
	}
	int Size()
	{
		return sizeof(*this) - sizeof(List) + Count * sizeof( GuildHouseWarHistoryStruct );
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client
//公會戰績分資訊
struct PG_GuildHousesWar_LtoC_GuildScore
{
	GamePGCommandEnum	Command;
	GuildHouseWarFightStruct	Info;

	PG_GuildHousesWar_LtoC_GuildScore()
	{
		Command = EM_PG_GuildHousesWar_LtoC_GuildScore;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//要求公會戰個人績分資訊
struct PG_GuildHousesWar_CtoL_PlayerScoreRequest
{
	GamePGCommandEnum	Command;
//	int		PlayerDBID;

	PG_GuildHousesWar_CtoL_PlayerScoreRequest()
	{
		Command = EM_PG_GuildHousesWar_CtoL_PlayerScoreRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client
//公會戰個人績分資訊
struct PG_GuildHousesWar_LtoC_PlayerScore
{
	GamePGCommandEnum	Command;

	int							Count;
	GuildWarPlayerInfoStruct	PlayerList[ 1000 ];

	PG_GuildHousesWar_LtoC_PlayerScore()
	{
		Command = EM_PG_GuildHousesWar_LtoC_PlayerScore;
	}
	int Size( )
	{
		return sizeof(*this)- sizeof(PlayerList) + Count * sizeof( PlayerList );
	}

};
//////////////////////////////////////////////////////////////////////////
*/