#pragma once
#include "NetWakerPGEnum.h"
#include "roledata/RoleDataEx.h"
#include "PG/PG_BG_GuildWarDef.h"
//////////////////////////////////////////////////////////////////////////
//要求跨伺服器公會名稱要求
//Client->Battle Field Local
struct PG_BG_GuildWar_CtoBL_GuildNameRequest
{
	GamePGCommandEnum	Command;
	int		WorldGuildID;

	PG_BG_GuildWar_CtoBL_GuildNameRequest()
	{Command = EM_PG_BG_GuildWar_CtoBL_GuildNameRequest;}
};
//////////////////////////////////////////////////////////////////////////
//要求跨伺服器公會名稱要求
//Battle Field Local->Local
struct PG_BG_GuildWar_BLtoL_GuildNameRequest
{
	GamePGCommandEnum	Command;
	int	PlayerDBID;
	int	WorldGuildID;

	PG_BG_GuildWar_BLtoL_GuildNameRequest()
	{Command = EM_PG_BG_GuildWar_BLtoL_GuildNameRequest;}
};
//////////////////////////////////////////////////////////////////////////
//要求跨伺服器公會名稱結果
//Local->Battle Field Local
struct PG_BG_GuildWar_LtoBL_GuildNameResult
{
	GamePGCommandEnum	Command;
	int	PlayerDBID;
	int	WorldGuildID;
	StaticString<_MAX_NAMERECORD_SIZE_> GuildName;	

	PG_BG_GuildWar_LtoBL_GuildNameResult()
	{Command = EM_PG_BG_GuildWar_LtoBL_GuildNameResult;}
};
//////////////////////////////////////////////////////////////////////////
//要求跨伺服器公會名稱結果
//Battle Field Local->Client
struct PG_BG_GuildWar_BLtoC_GuildNameResult
{
	GamePGCommandEnum	Command;
	int	WorldGuildID;
	StaticString<_MAX_NAMERECORD_SIZE_> GuildName;	

	PG_BG_GuildWar_BLtoC_GuildNameResult()
	{Command = EM_PG_BG_GuildWar_BLtoC_GuildNameResult;}
};
//////////////////////////////////////////////////////////////////////////
//開啟註冊公會戰介面
//Client->Local
struct PG_BG_GuildWar_CtoL_OpenMenuRequest
{
	GamePGCommandEnum	Command;


	PG_BG_GuildWar_CtoL_OpenMenuRequest()
	{Command = EM_PG_BG_GuildWar_CtoL_OpenMenuRequest;}
};
//////////////////////////////////////////////////////////////////////////
//要求公會戰資訊
//Local->Battle Field Local
struct PG_BG_GuildWar_LtoBL_OpenMenuRequest
{
	GamePGCommandEnum	Command;
	int PlayerDBID;

	PG_BG_GuildWar_LtoBL_OpenMenuRequest()
	{Command = EM_PG_BG_GuildWar_LtoBL_OpenMenuRequest;}
};
//////////////////////////////////////////////////////////////////////////
//公會戰開啟狀況
//Battle Field Local->Local
struct PG_BG_GuildWar_BLtoL_OpenMenuResult
{
	GamePGCommandEnum	Command;
	int PlayerDBID;	
	int	ZoneGroupID;
	GuildHouseWarStateENUM	State;
	int	NextTime;
	int						Count;
	GuildHouseWarInfoStruct List[1000];

	PG_BG_GuildWar_BLtoL_OpenMenuResult()
	{Command = EM_PG_BG_GuildWar_BLtoL_OpenMenuResult;}
	int PGSize()
	{
		return sizeof(*this) - sizeof(List) + Count * sizeof( GuildHouseWarInfoStruct );
	}
};
//////////////////////////////////////////////////////////////////////////
//開啟註冊公會戰介面
//Local->Client
struct PG_BG_GuildWar_LtoC_OpenMenuResult
{
	GamePGCommandEnum	Command;
	int PlayerDBID;	
	int ZoneGroupID;
	GuildHouseWarStateENUM	State;
	int	NextTime;
	int						Count;
	GuildHouseWarInfoStruct List[1000];

	PG_BG_GuildWar_LtoC_OpenMenuResult()
	{Command = EM_PG_BG_GuildWar_LtoC_OpenMenuResult;}
	int PGSize()
	{
		return sizeof(*this) - sizeof(List) + Count * sizeof( GuildHouseWarInfoStruct );
	}
};
//////////////////////////////////////////////////////////////////////////
//註冊公會戰( or 取消 )
//Client->Local
struct PG_BG_GuildWar_CtoL_RegisterRequest
{
	GamePGCommandEnum	Command;
	GuildWarRegisterTypeENUM Type;
	int					ZoneGroupID;
	int					GuildCount;			//目前公會數量
	bool				IsAcceptAssignment;	//接受指定對戰
	int					TargetGuild;		//指定對戰公會

	PG_BG_GuildWar_CtoL_RegisterRequest()
	{Command = EM_PG_BG_GuildWar_CtoL_RegisterRequest;}
};
//////////////////////////////////////////////////////////////////////////
//註冊公會戰( or 取消 )
//Local->Battle Field Local
struct GuildWarRegisterInfoStruct
{
	int					GuildID;
	int					GuildCount;
	int					GuildLv;
	int 				Score;
	int					PlayerDBID;			//要求者
	bool				IsAcceptAssignment;	//接受指定對戰
	int					TargetGuild;		//指定對戰公會
	StaticString< _MAX_NAMERECORD_SIZE_ > GuildName;
	GuildWarRegisterTypeENUM Type;
};
struct PG_BG_GuildWar_LtoBL_RegisterRequest
{
	GamePGCommandEnum	Command;

	GuildWarRegisterInfoStruct Info;
	/*
	int					GuildID;
	int					GuildCount;
	int 				Score;
	int					PlayerDBID;			//要求者
	bool				IsAcceptAssignment;	//接受指定對戰
	int					TargetGuild;		//指定對戰公會

	StaticString< _MAX_NAMERECORD_SIZE_ > GuildName;
	GuildWarRegisterTypeENUM Type;
*/

	PG_BG_GuildWar_LtoBL_RegisterRequest()
	{Command = EM_PG_BG_GuildWar_LtoBL_RegisterRequest;}
};
//////////////////////////////////////////////////////////////////////////
enum GuildWarRegisterResultENUM
{
	EM_GuildWarRegisterResult_OK			,
	EM_GuildWarRegisterResult_UnknowErr		,
	EM_GuildWarRegisterResult_Full			,
	EM_GuildWarRegisterResult_GuildCountErr	,
	EM_GuildWarRegisterResult_NoGuildHouse	,	//沒有公會屋
	EM_GuildWarRegisterResult_NotLeader		,	//不是會長
	EM_GuildWarRegisterResult_OnceaDay		,	//一天一次公會戰
	EM_GuildWarRegisterResult_GuildWarClose	,	//非開放時間
};
//註冊公會戰( or 取消 )結果
//Battle Field Local->Local
struct PG_BG_GuildWar_BLtoL_RegisterResult
{
	GamePGCommandEnum	Command;
	int		GuildID;
	GuildWarRegisterTypeENUM Type;
	GuildHouseWarInfoStruct Info;
	GuildWarRegisterResultENUM	Result;
	int		PlayerDBID;			//要求者

	PG_BG_GuildWar_BLtoL_RegisterResult()
	{Command = EM_PG_BG_GuildWar_BLtoL_RegisterResult;}
};
//////////////////////////////////////////////////////////////////////////
//註冊公會戰( or 取消 )結果
//Local->Client
struct PG_BG_GuildWar_LtoC_RegisterResult
{
	GamePGCommandEnum	Command;
	GuildWarRegisterTypeENUM Type;
	GuildHouseWarInfoStruct Info;
	GuildWarRegisterResultENUM	Result;

	PG_BG_GuildWar_LtoC_RegisterResult()
	{Command = EM_PG_BG_GuildWar_LtoC_RegisterResult;}
};
//////////////////////////////////////////////////////////////////////////
//公會戰狀況( 開始 , 準備 , 結束 )
//Battle Field Local->Local
struct PG_BG_GuildWar_BLtoL_Status
{
	GamePGCommandEnum	Command;
	GuildWarStatusTypeENUM Status;
	int					NextTime;

	PG_BG_GuildWar_BLtoL_Status()
	{Command = EM_PG_BG_GuildWar_BLtoL_Status;}
};
//////////////////////////////////////////////////////////////////////////
//公會戰狀況( 開始 , 準備 , 結束 )
//Local->Client
struct PG_BG_GuildWar_LtoC_Status
{
	GamePGCommandEnum	Command;
	GuildWarStatusTypeENUM Status;
	int					NextTime;

	PG_BG_GuildWar_LtoC_Status()
	{Command = EM_PG_BG_GuildWar_LtoC_Status;}
};
//////////////////////////////////////////////////////////////////////////
//要求公會屋資料(取zone401的資料)
//Battle Field Local->Local
struct PG_BG_GuildWar_BLtoL_HouseLoadRequest
{
	GamePGCommandEnum	Command;
	int		GuildID;

	PG_BG_GuildWar_BLtoL_HouseLoadRequest()
	{Command = EM_PG_BG_GuildWar_BLtoL_HouseLoadRequest;}
};
//////////////////////////////////////////////////////////////////////////
//公會屋基本資料
//Local->Battle Field Local
struct PG_BG_GuildWar_LtoBL_HouseBaseInfo
{
	GamePGCommandEnum	Command;
	GuildHousesInfoStruct HouseBaseInfo;

	PG_BG_GuildWar_LtoBL_HouseBaseInfo()
	{Command = EM_PG_BG_GuildWar_LtoBL_HouseBaseInfo;}
};
//////////////////////////////////////////////////////////////////////////
//公會屋建築資料
//Local->Battle Field Local
struct PG_BG_GuildWar_LtoBL_HouseBuildingInfo
{
	GamePGCommandEnum	Command;
	GuildHouseBuildingInfoStruct Building;

	PG_BG_GuildWar_LtoBL_HouseBuildingInfo()
	{Command = EM_PG_BG_GuildWar_LtoBL_HouseBuildingInfo;}
};
//////////////////////////////////////////////////////////////////////////
//公會屋家具資料
//Local->Battle Field Local
struct PG_BG_GuildWar_LtoBL_HouseFurnitureInfo
{
	GamePGCommandEnum	Command;
	GuildHouseFurnitureItemStruct Item;

	PG_BG_GuildWar_LtoBL_HouseFurnitureInfo()
	{Command = EM_PG_BG_GuildWar_LtoBL_HouseFurnitureInfo;}
};
//////////////////////////////////////////////////////////////////////////
//公會屋資料讀取完畢
//Local->Battle Field Local
struct PG_BG_GuildWar_LtoBL_HouseLoadOK
{
	GamePGCommandEnum	Command;
	int	GuildID;

	PG_BG_GuildWar_LtoBL_HouseLoadOK()
	{Command = EM_PG_BG_GuildWar_LtoBL_HouseLoadOK;}
};
//////////////////////////////////////////////////////////////////////////
//要求進入公會戰場
//Client->Local
struct PG_BG_GuildWar_CtoL_EnterRequest
{
	GamePGCommandEnum	Command;
	int					ZoneGroupID;


	PG_BG_GuildWar_CtoL_EnterRequest()
	{Command = EM_PG_BG_GuildWar_CtoL_EnterRequest;}
};
//////////////////////////////////////////////////////////////////////////
//要求進入公會戰場
//Local->Battle Field Local
struct PG_BG_GuildWar_LtoBL_EnterRequest
{
	GamePGCommandEnum	Command;
	int PlayerDBID;
	int GuildID;

	PG_BG_GuildWar_LtoBL_EnterRequest()
	{Command = EM_PG_BG_GuildWar_LtoBL_EnterRequest;}
};
//////////////////////////////////////////////////////////////////////////
//要求進入結果
//Battle Field Local->Local
struct PG_BG_GuildWar_BLtoL_EnterResult
{
	GamePGCommandEnum	Command;
	int ZoneGroupID;
	int PlayerDBID;
	int GuildID;
	bool Result;

	PG_BG_GuildWar_BLtoL_EnterResult()
	{Command = EM_PG_BG_GuildWar_BLtoL_EnterResult;}
};
//////////////////////////////////////////////////////////////////////////
//要求進入結果
//Local->Client
struct PG_BG_GuildWar_LtoC_EnterResult
{
	GamePGCommandEnum	Command;
	bool Result;

	PG_BG_GuildWar_LtoC_EnterResult()
	{Command = EM_PG_BG_GuildWar_LtoC_EnterResult;}
};
//////////////////////////////////////////////////////////////////////////
//要求離開公會戰場
//Client->Battle Field Local
struct PG_BG_GuildWar_CtoBL_LeaveRequest
{
	GamePGCommandEnum	Command;


	PG_BG_GuildWar_CtoBL_LeaveRequest()
	{Command = EM_PG_BG_GuildWar_CtoBL_LeaveRequest;}
};
//////////////////////////////////////////////////////////////////////////
//要求離開公會戰場結果
//Battle Field Local->Client
struct PG_BG_GuildWar_BLtoC_LeaveResult
{
	GamePGCommandEnum	Command;
	bool Result;

	PG_BG_GuildWar_BLtoC_LeaveResult()
	{Command = EM_PG_BG_GuildWar_BLtoC_LeaveResult;}
};
//////////////////////////////////////////////////////////////////////////
//公會戰績分資訊
//Battle Field Local->Client
struct PG_BG_GuildWar_BLtoC_GuildScore
{
	GamePGCommandEnum	Command;
	int					Time;
	GuildHouseWarFightStruct Info;

	PG_BG_GuildWar_BLtoC_GuildScore()
	{Command = EM_PG_BG_GuildWar_BLtoC_GuildScore;}
};
//////////////////////////////////////////////////////////////////////////
//公會戰開始
//Battle Field Local->Client
struct PG_BG_GuildWar_BLtoC_BeginWar
{
	GamePGCommandEnum	Command;


	PG_BG_GuildWar_BLtoC_BeginWar()
	{Command = EM_PG_BG_GuildWar_BLtoC_BeginWar;}
};
//////////////////////////////////////////////////////////////////////////
//公會戰結束
//Battle Field Local->Client
struct PG_BG_GuildWar_BLtoC_EndWar
{
	GamePGCommandEnum	Command;

	PG_BG_GuildWar_BLtoC_EndWar()
	{Command = EM_PG_BG_GuildWar_BLtoC_EndWar;}
};
//////////////////////////////////////////////////////////////////////////
//除錯時間設定
//Ｌocal -> Battle Field Local
struct PG_BG_GuildWar_LtoBL_DebugTime
{
	GamePGCommandEnum	Command;
	int		Time_Hour;
	int		Time_Day;
	
	PG_BG_GuildWar_LtoBL_DebugTime()
	{Command = EM_PG_BG_GuildWar_LtoBL_DebugTime;}
};
//////////////////////////////////////////////////////////////////////////
//要求戰場玩家狀態資料
//Client -> Battle Field Local
struct PG_BG_GuildWar_CtoBL_PlayerInfoRequest
{
	GamePGCommandEnum			Command;

	PG_BG_GuildWar_CtoBL_PlayerInfoRequest()
	{Command = EM_PG_BG_GuildWar_CtoBL_PlayerInfoRequest;}
};
//////////////////////////////////////////////////////////////////////////
//要求戰場玩家狀態資料
//Battle Field Local -> Client
struct PG_BG_GuildWar_BLtoC_PlayerInfoResult
{
	GamePGCommandEnum			Command;
	int							TotalCount;
	int							ID;
	GuildWarPlayerInfoStruct	Info;

	PG_BG_GuildWar_BLtoC_PlayerInfoResult()
	{Command = EM_PG_BG_GuildWar_BLtoC_PlayerInfoResult;}
};
//////////////////////////////////////////////////////////////////////////
struct PlayerListInfoBase
{
	int	Count;
	GuildWarPlayerInfoStruct	Info[500];
};
//打包送出玩家分數資訊
struct PG_BG_GuildWar_BLtoC_PlayerListInfo_Zip
{
	GamePGCommandEnum		Command;
	int						DataSize;
	char*					Data[0x10000];

	PG_BG_GuildWar_BLtoC_PlayerListInfo_Zip()
	{Command = EM_PG_BG_GuildWar_BLtoC_PlayerListInfo_Zip;}

	int PGSize()
	{
		return sizeof(*this) - sizeof(Data) + DataSize;
	}
};

//////////////////////////////////////////////////////////////////////////
//通知最後戰績資料
//Battle Field Local -> Client
struct PG_BG_GuildWar_BLtoC_PlayerFinalInfo
{
	GamePGCommandEnum			Command;
	
	int							PlayerDBID;
	int							PrizeType;
	int							Score;
	int							OrderID;

	Voc_ENUM					Voc;								//職業
	Voc_ENUM					Voc_Sub;							//副職業
	int							Level;
	int							Level_Sub;	


	PG_BG_GuildWar_BLtoC_PlayerFinalInfo()
	{Command = EM_PG_BG_GuildWar_BLtoC_PlayerFinalInfo;}
};

//////////////////////////////////////////////////////////////////////////
//公會戰結束通知最後輸贏( 跟原本Server 401 公會屋說)
//Battle Field Local->Local
struct PG_BG_GuildWar_BLtoL_EndWarResult
{
	GamePGCommandEnum	Command;
	int					GuildID;
	GameResultENUM		GameResult;
	int					GuildScore;		//公會積分修正
	GuildWarBaseInfoStruct TargetGuild;

	PG_BG_GuildWar_BLtoL_EndWarResult()
	{Command = EM_PG_BG_GuildWar_BLtoL_EndWarResult;}
};

//////////////////////////////////////////////////////////////////////////
//要求載入公會戰資料
//BL -> DataCenter
struct PG_BG_GuildWar_BLtoD_LoadInfo
{
	GamePGCommandEnum	Command;
	int					ZoneGroupID;

	PG_BG_GuildWar_BLtoD_LoadInfo()
	{Command = EM_PG_BG_GuildWar_BLtoD_LoadInfo;}
};


struct PG_BG_GuildWar_DtoBL_OrderInfo
{
	GamePGCommandEnum	Command;
	int						Count;
	GuildHouseWarInfoStruct List[500];

	PG_BG_GuildWar_DtoBL_OrderInfo()
	{Command = EM_PG_BG_GuildWar_DtoBL_OrderInfo;}
};
//////////////////////////////////////////////////////////////////////////
// 公會排行
//////////////////////////////////////////////////////////////////////////
struct GuildWarRankInfoStruct
{
	StaticString<_MAX_NAMERECORD_SIZE_> GuildName;	
	int					WorldGuildID;
	int					Score;
	int					WinCount;			//勝場
	int					EvenCount;			//平手
	int					LostCount;			//敗場

};
//Datacenter -> Battle Local
//定時回報公會排行資料
struct PG_BG_GuildWar_DtoBL_RegGuildWarRankInfo
{
	GamePGCommandEnum			Command;
	GuildWarRankInfoStruct		Info;
	GuildHouseWarHistoryStruct	Hisotry[50];

	PG_BG_GuildWar_DtoBL_RegGuildWarRankInfo()
	{Command = EM_PG_BG_GuildWar_DtoBL_RegGuildWarRankInfo;}
};
//////////////////////////////////////////////////////////////////////////
enum GuildWarRankInfoTypeENUM
{
	EM_GuildWarRankInfoType_RankList	,
	EM_GuildWarRankInfoType_History		,
};
//Client -> Local 
//要求所有工會排行的資料
struct PG_BG_GuildWar_CtoL_GuildWarRankInfoRequest
{
	GamePGCommandEnum			Command;
	GuildWarRankInfoTypeENUM	Type;
	int							WorldGuildID;

	PG_BG_GuildWar_CtoL_GuildWarRankInfoRequest()
	{Command = EM_PG_BG_GuildWar_CtoL_GuildWarRankInfoRequest;}
};
//////////////////////////////////////////////////////////////////////////
//Local -> 戰場401
//要求所有工會排行的資料
struct PG_BG_GuildWar_LtoBL_GuildWarRankInfoRequest
{
	GamePGCommandEnum			Command;
	int							PlayerDBID;
	int							WorldGuildID;
	GuildWarRankInfoTypeENUM	Type;

	PG_BG_GuildWar_LtoBL_GuildWarRankInfoRequest()
	{Command = EM_PG_BG_GuildWar_LtoBL_GuildWarRankInfoRequest;}
};
//////////////////////////////////////////////////////////////////////////
// 戰場401 -> Local
//公會排行資料
struct PG_BG_GuildWar_BLtoL_GuildWarRankInfo
{
	GamePGCommandEnum			Command;
	int							PlayerDBID;
	GuildWarRankInfoStruct		Info;

	PG_BG_GuildWar_BLtoL_GuildWarRankInfo()
	{Command = EM_PG_BG_GuildWar_BLtoL_GuildWarRankInfo;}
};

// 戰場401 -> Local
//公會排行資料
struct PG_BG_GuildWar_LtoC_GuildWarRankInfo
{
	GamePGCommandEnum			Command;
	GuildWarRankInfoStruct		Info;

	PG_BG_GuildWar_LtoC_GuildWarRankInfo()
	{Command = EM_PG_BG_GuildWar_LtoC_GuildWarRankInfo;}
};
//////////////////////////////////////////////////////////////////////////
// 戰場401 -> Local
//公會排行資料
struct PG_BG_GuildWar_BLtoL_GuildWarHisotry
{
	GamePGCommandEnum			Command;
	int							PlayerDBID;
	GuildHouseWarHistoryStruct	Hisotry[50];

	PG_BG_GuildWar_BLtoL_GuildWarHisotry()
	{Command = EM_PG_BG_GuildWar_BLtoL_GuildWarHisotry;}
};

// 戰場401 -> Local
//公會排行資料
struct PG_BG_GuildWar_LtoC_GuildWarHisotry
{
	GamePGCommandEnum			Command;
	GuildHouseWarHistoryStruct	Hisotry[50];

	PG_BG_GuildWar_LtoC_GuildWarHisotry()
	{Command = EM_PG_BG_GuildWar_LtoC_GuildWarHisotry;}
};
//列出所有列舉
//EM_PG_BG_GuildWar_CtoBL_GuildNameRequest,
//EM_PG_BG_GuildWar_BLtoL_GuildNameRequest,
//EM_PG_BG_GuildWar_LtoBL_GuildNameResult,
//EM_PG_BG_GuildWar_BLtoC_GuildNameResult,
//EM_PG_BG_GuildWar_CtoL_OpenMenuRequest,
//EM_PG_BG_GuildWar_LtoBL_OpenMenuRequest,
//EM_PG_BG_GuildWar_BLtoL_OpenMenuResult,
//EM_PG_BG_GuildWar_LtoC_OpenMenuResult,
//EM_PG_BG_GuildWar_CtoL_RegisterRequest,
//EM_PG_BG_GuildWar_LtoBL_RegisterRequest,
//EM_PG_BG_GuildWar_BLtoL_RegisterResult,
//EM_PG_BG_GuildWar_LtoC_RegisterResult,
//EM_PG_BG_GuildWar_BLtoL_Status,
//EM_PG_BG_GuildWar_LtoC_Status,
//EM_PG_BG_GuildWar_BLtoL_HouseLoadRequest,
//EM_PG_BG_GuildWar_LtoBL_HouseBaseInfo,
//EM_PG_BG_GuildWar_LtoBL_HouseBuildingInfo,
//EM_PG_BG_GuildWar_LtoBL_HouseFurnitureInfo,
//EM_PG_BG_GuildWar_LtoBL_HouseLoadOK,
//EM_PG_BG_GuildWar_CtoL_EnterRequest,
//EM_PG_BG_GuildWar_LtoBL_EnterRequest,
//EM_PG_BG_GuildWar_BLtoL_EnterResult,
//EM_PG_BG_GuildWar_LtoC_EnterResult,
//EM_PG_BG_GuildWar_CtoBL_LeaveRequest,
//EM_PG_BG_GuildWar_BLtoC_LeaveResult,
//EM_PG_BG_GuildWar_BLtoC_GuildScore,
//EM_PG_BG_GuildWar_BLtoC_BeginWar,
//EM_PG_BG_GuildWar_BLtoC_EndWar,
//////////////////////////////////////////////////////////////////////////
