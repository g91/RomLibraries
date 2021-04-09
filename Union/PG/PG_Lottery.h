#pragma once
#include "RoleData\RoleDataEx.h"
#include "NetWakerPGEnum.h"

#define DEF_LOTTERY_PRIZE1_RATE	15
#define DEF_LOTTERY_PRIZE2_RATE	45
#define DEF_LOTTERY_PRIZE3_RATE	15
#define DEF_LOTTERY_BONUS_RATE	20
#define DEF_LOTTERY_COST		500
//////////////////////////////////////////////////////////////////////////
//Local -> Client �}�Ҽֳz�����I��
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
//Client -> Local �I���m��
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
//Local - > DataCenter �I���m��
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
//DataCenter -> Local �I�����G
struct PG_Lottery_DtoL_ExchangePrizeResult
{
	GamePGCommandEnum	Command;
	ItemFieldStruct		Item;
	int					Pos;
	int					PlayerDBID;
	int					PrizeCount[3];			//���U�������ƶq
	int					TotalMoney;				//�`�m��
	bool				Result;

	PG_Lottery_DtoL_ExchangePrizeResult()
	{
		Command = EM_PG_Lottery_DtoL_ExchangePrizeResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client �I�����G
struct PG_Lottery_LtoC_ExchangePrizeResult
{
	GamePGCommandEnum	Command;

	int		PrizeCount[3];			//���U�������ƶq
	int		Money;					//����
	bool	Result;

	PG_Lottery_LtoC_ExchangePrizeResult()
	{
		Command = EM_PG_Lottery_LtoC_ExchangePrizeResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local ��������
struct PG_Lottery_CtoL_CloseExchangePrize
{
	GamePGCommandEnum	Command;

	PG_Lottery_CtoL_CloseExchangePrize()
	{
		Command = EM_PG_Lottery_CtoL_CloseExchangePrize;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client ��������
struct PG_Lottery_LtoC_CloseExchangePrize
{
	GamePGCommandEnum	Command;

	PG_Lottery_LtoC_CloseExchangePrize()
	{
		Command = EM_PG_Lottery_LtoC_CloseExchangePrize;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client �}���ʶR�ֳz
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
//Client -> Local �ʶR�m��
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
//Local -> DataCenter �ʶR�m��
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
//DataCenter -> Local �ʶR���G
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
//Local -> Client  �ʶR���G
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
//Client -> Local ��������
struct PG_Lottery_CtoL_CloseBuyLottery
{
	GamePGCommandEnum	Command;

	PG_Lottery_CtoL_CloseBuyLottery()
	{
		Command = EM_PG_Lottery_CtoL_CloseBuyLottery;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client ��������
struct PG_Lottery_LtoC_CloseBuyLottery
{
	GamePGCommandEnum	Command;

	PG_Lottery_LtoC_CloseBuyLottery()
	{
		Command = EM_PG_Lottery_LtoC_CloseBuyLottery;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local �n�D�ֳz�ثe���
struct PG_Lottery_CtoL_LotteryInfoRequest
{
	GamePGCommandEnum	Command;

	PG_Lottery_CtoL_LotteryInfoRequest()
	{
		Command = EM_PG_Lottery_CtoL_LotteryInfoRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter �n�D�ֳz�ثe���
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
//DataCenter -> Client �ֳz�ثe���
struct PG_Lottery_DtoC_LotteryInfo
{
	GamePGCommandEnum	Command;
	int					MaxPrizeMoney;	//�̤j���ثe�֭p���B
	int					Version;		//�ĴX��

	//LotteryInfoStruct	LastInfo[5];

	PG_Lottery_DtoC_LotteryInfo()
	{
		Command = EM_PG_Lottery_DtoC_LotteryInfo;
	}
};
//////////////////////////////////////////////////////////////////////////
struct LotteryInfoStruct
{
	int Version;				//�ĴX�� ( -1 �N��S�� )
	int	Num[5];
	int PrizeMoney[3];			//�����`�m��
	int PrizePlayerCount[3];	//�����H��
	int	RunLotteryTime;			//�}����
	int TotalMoney;
};
//DataCenter -> Client �}�����
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
//Client -> Local �n�D�ֳz�ثe���
struct PG_Lottery_CtoL_LotteryHistoryInfoRequest
{
	GamePGCommandEnum	Command;

	PG_Lottery_CtoL_LotteryHistoryInfoRequest()
	{
		Command = EM_PG_Lottery_CtoL_LotteryHistoryInfoRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter �n�D�ֳz�ثe���
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

//DataCenter -> Client �ֳz�ثe���
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