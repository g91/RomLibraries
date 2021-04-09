#pragma once
#include "RoleData\RoleDataEx.h"
#include "NetWakerPGEnum.h"

#define DEF_LOTTERY_PRIZE1_RATE	15
#define DEF_LOTTERY_PRIZE2_RATE	45
#define DEF_LOTTERY_PRIZE3_RATE	15
#define DEF_LOTTERY_BONUS_RATE	20
#define DEF_LOTTERY_COST		500
//////////////////////////////////////////////////////////////////////////
//Local -> Client 開啟樂透介面兌換
struct PG_Lottery_LtoC_OpenExchangePrize
{
	GamePGCommandEnum	Command;
	int					TargetNPCID;	

	PG_Lottery_LtoC_OpenExchangePrize()
	{
		Command = EM_PG_Lottery_LtoC_OpenExchangePrize;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 兌換彩金
struct PG_Lottery_CtoL_ExchangePrize
{
	GamePGCommandEnum	Command;
	ItemFieldStruct		Item;
	int					Pos;

	PG_Lottery_CtoL_ExchangePrize()
	{
		Command = EM_PG_Lottery_CtoL_ExchangePrize;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local - > DataCenter 兌換彩金
struct PG_Lottery_LtoD_ExchangePrize
{
	GamePGCommandEnum	Command;
	ItemFieldStruct		Item;
	int					Pos;
	int					PlayerDBID;

	PG_Lottery_LtoD_ExchangePrize()
	{
		Command = EM_PG_Lottery_LtoD_ExchangePrize;
	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local 兌換結果
struct PG_Lottery_DtoL_ExchangePrizeResult
{
	GamePGCommandEnum	Command;
	ItemFieldStruct		Item;
	int					Pos;
	int					PlayerDBID;
	int					PrizeCount[3];			//中各獎項的數量
	int					TotalMoney;				//總彩金
	bool				Result;

	PG_Lottery_DtoL_ExchangePrizeResult()
	{
		Command = EM_PG_Lottery_DtoL_ExchangePrizeResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client 兌換結果
struct PG_Lottery_LtoC_ExchangePrizeResult
{
	GamePGCommandEnum	Command;

	int		PrizeCount[3];			//中各獎項的數量
	int		Money;					//獎金
	bool	Result;

	PG_Lottery_LtoC_ExchangePrizeResult()
	{
		Command = EM_PG_Lottery_LtoC_ExchangePrizeResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 關閉介面
struct PG_Lottery_CtoL_CloseExchangePrize
{
	GamePGCommandEnum	Command;

	PG_Lottery_CtoL_CloseExchangePrize()
	{
		Command = EM_PG_Lottery_CtoL_CloseExchangePrize;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client 關閉介面
struct PG_Lottery_LtoC_CloseExchangePrize
{
	GamePGCommandEnum	Command;

	PG_Lottery_LtoC_CloseExchangePrize()
	{
		Command = EM_PG_Lottery_LtoC_CloseExchangePrize;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client 開啟購買樂透
struct PG_Lottery_LtoC_OpenBuyLottery
{
	GamePGCommandEnum	Command;
	int					TargetNPCID;

	PG_Lottery_LtoC_OpenBuyLottery()
	{
		Command = EM_PG_Lottery_LtoC_OpenBuyLottery;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 購買彩卷
struct PG_Lottery_CtoL_BuyLottery
{
	GamePGCommandEnum	Command;
	int					Money;
	int					Count;
	int					Number[20];

	PG_Lottery_CtoL_BuyLottery()
	{
		Command = EM_PG_Lottery_CtoL_BuyLottery;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter 購買彩卷
struct PG_Lottery_LtoD_BuyLottery
{
	GamePGCommandEnum	Command;
	int					Money;	
	int					PlayerDBID;
	int					Count;
	int					Number[20];

	PG_Lottery_LtoD_BuyLottery()
	{
		Command = EM_PG_Lottery_LtoD_BuyLottery;
	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local 購買結果
struct PG_Lottery_DtoL_BuyLotteryResult
{
	GamePGCommandEnum	Command;
	int					Version;
	int					Money;
	int					PlayerDBID;
	int					Count;
	int					Number[20];
	bool				Result;

	PG_Lottery_DtoL_BuyLotteryResult()
	{
		Command = EM_PG_Lottery_DtoL_BuyLotteryResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client  購買結果
struct PG_Lottery_LtoC_BuyLotteryResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Lottery_LtoC_BuyLotteryResult()
	{
		Command = EM_PG_Lottery_LtoC_BuyLotteryResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 關閉介面
struct PG_Lottery_CtoL_CloseBuyLottery
{
	GamePGCommandEnum	Command;

	PG_Lottery_CtoL_CloseBuyLottery()
	{
		Command = EM_PG_Lottery_CtoL_CloseBuyLottery;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client 關閉介面
struct PG_Lottery_LtoC_CloseBuyLottery
{
	GamePGCommandEnum	Command;

	PG_Lottery_LtoC_CloseBuyLottery()
	{
		Command = EM_PG_Lottery_LtoC_CloseBuyLottery;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 要求樂透目前資料
struct PG_Lottery_CtoL_LotteryInfoRequest
{
	GamePGCommandEnum	Command;

	PG_Lottery_CtoL_LotteryInfoRequest()
	{
		Command = EM_PG_Lottery_CtoL_LotteryInfoRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter 要求樂透目前資料
struct PG_Lottery_LtoD_LotteryInfoRequest
{
	GamePGCommandEnum	Command;
	int				PlayerDBID;

	PG_Lottery_LtoD_LotteryInfoRequest()
	{
		Command = EM_PG_Lottery_LtoD_LotteryInfoRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Client 樂透目前資料
struct PG_Lottery_DtoC_LotteryInfo
{
	GamePGCommandEnum	Command;
	int					MaxPrizeMoney;	//最大獎目前累計金額
	int					Version;		//第幾期

	//LotteryInfoStruct	LastInfo[5];

	PG_Lottery_DtoC_LotteryInfo()
	{
		Command = EM_PG_Lottery_DtoC_LotteryInfo;
	}
};
//////////////////////////////////////////////////////////////////////////
struct LotteryInfoStruct
{
	int Version;				//第幾期 ( -1 代表沒有 )
	int	Num[5];
	int PrizeMoney[3];			//中獎總彩金
	int PrizePlayerCount[3];	//中獎人數
	int	RunLotteryTime;			//開獎日
	int TotalMoney;
};
//DataCenter -> Client 開獎資料
struct PG_Lottery_DtoC_RunLottery
{
	GamePGCommandEnum	Command;

	LotteryInfoStruct	Info;

	PG_Lottery_DtoC_RunLottery()
	{
		Command = EM_PG_Lottery_DtoC_RunLottery;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 要求樂透目前資料
struct PG_Lottery_CtoL_LotteryHistoryInfoRequest
{
	GamePGCommandEnum	Command;

	PG_Lottery_CtoL_LotteryHistoryInfoRequest()
	{
		Command = EM_PG_Lottery_CtoL_LotteryHistoryInfoRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter 要求樂透目前資料
struct PG_Lottery_LtoD_LotteryHistoryInfoRequest
{
	GamePGCommandEnum	Command;
	int				PlayerDBID;

	PG_Lottery_LtoD_LotteryHistoryInfoRequest()
	{
		Command = EM_PG_Lottery_LtoD_LotteryHistoryInfoRequest;
	}
};
//////////////////////////////////////////////////////////////////////////

//DataCenter -> Client 樂透目前資料
struct PG_Lottery_DtoC_LotteryHistoryInfo
{
	GamePGCommandEnum	Command;

	LotteryInfoStruct	LastInfo[5];

	PG_Lottery_DtoC_LotteryHistoryInfo()
	{
		Command = EM_PG_Lottery_DtoC_LotteryHistoryInfo;
	}
};
//////////////////////////////////////////////////////////////////////////