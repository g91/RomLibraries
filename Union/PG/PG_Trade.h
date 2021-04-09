#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged
//---------------------------------------------------------------------------
//A Client -> Local 
//要求交易
struct PG_Trade_CtoL_RequestTrade
{
    GamePGCommandEnum	Command;
    int					TargetID;				

    PG_Trade_CtoL_RequestTrade()
    {
        Command = EM_PG_Trade_CtoL_RequestTrade;
    }
};
//---------------------------------------------------------------------------
//Local -> A Client
// 要求交易目標有問題
struct PG_Trade_LtoC_RequestTradeError
{
	GamePGCommandEnum	Command;

	PG_Trade_LtoC_RequestTradeError()
	{
		Command = EM_PG_Trade_LtoC_RequestTradeError;
	}
};
//---------------------------------------------------------------------------
//Local -> B Client 
//交易申請
struct PG_Trade_LtoC_RequestTrade
{
    GamePGCommandEnum	Command;
    int					TargetID;				

    PG_Trade_LtoC_RequestTrade()
    {
        Command = EM_PG_Trade_LtoC_RequestTrade;
    }
};
//---------------------------------------------------------------------------
//B Client -> Local 
//同意交易( 或不同意 )
struct PG_Trade_CtoL_AgreeTrade
{
    GamePGCommandEnum	Command;
    int					TargetID;
	bool				Result;
	int					ClientResult;		//Client轉送給要請者

    PG_Trade_CtoL_AgreeTrade()
    {
        Command = EM_PG_Trade_CtoL_AgreeTrade;
    }
};
//---------------------------------------------------------------------------
// Local -> A Client
//對方不同意交易
struct PG_Trade_LtoC_DisagreeTrade
{
	GamePGCommandEnum	Command;
	int					ClientResult;		//Client轉送給要請者

	PG_Trade_LtoC_DisagreeTrade()
	{
		Command = EM_PG_Trade_LtoC_DisagreeTrade;
	}
};

//---------------------------------------------------------------------------
//Local -> Client 
//開啟交易
struct PG_Trade_LtoC_OpenTrade
{
    GamePGCommandEnum	Command;
    int					TargetID;				
    char				Name[32];

    PG_Trade_LtoC_OpenTrade()
    {
        Command = EM_PG_Trade_LtoC_OpenTrade;
    }
};
//---------------------------------------------------------------------------
//Client -> Local 
//放物品		
struct PG_Trade_CtoL_PutItem
{
    GamePGCommandEnum	Command;

    int					Pos;
    int					ItemPos;		//物品欄位
    ItemFieldStruct		Item;

    PG_Trade_CtoL_PutItem()
    {
        Command = EM_PG_Trade_CtoL_PutItem;
    }
};
//---------------------------------------------------------------------------
//Local -> Client 
//對方放物品

struct PG_Trade_LtoC_PutItemResult
{
    GamePGCommandEnum	Command;
    bool                Result;
    
    PG_Trade_LtoC_PutItemResult()
    {
        Command = EM_PG_Trade_LtoC_PutItemResult;
    }
};
//---------------------------------------------------------------------------
//Local -> Client 
//對方放物品

struct PG_Trade_LtoC_TargetPutItem
{
    GamePGCommandEnum	Command;

    int					Pos;			//交易欄位
    ItemFieldStruct		Item;

    PG_Trade_LtoC_TargetPutItem()
    {
        Command = EM_PG_Trade_LtoC_TargetPutItem;
    }
};
//---------------------------------------------------------------------------
//Client -> Local 
//放金錢
struct PG_Trade_CtoL_PutMoney
{
    GamePGCommandEnum	Command;

    int					Money;
	int					Money_Account;

    PG_Trade_CtoL_PutMoney()
    {
        Command = EM_PG_Trade_CtoL_PutMoney;
    }
};
//---------------------------------------------------------------------------
//Local -> Client 
//對方放錢
struct PG_Trade_LtoC_TargetPutMoney
{
    GamePGCommandEnum	Command;

    int					Money;
	int					Money_Account;

    PG_Trade_LtoC_TargetPutMoney()
    {
        Command = EM_PG_Trade_LtoC_TargetPutMoney;
    }
};
//---------------------------------------------------------------------------
//Local -> Client 
//自己放錢
struct PG_Trade_LtoC_OwnerPutMoney
{
    GamePGCommandEnum	Command;

    int					Money;
	int					Money_Account;

    PG_Trade_LtoC_OwnerPutMoney()
    {
        Command = EM_PG_Trade_LtoC_OwnerPutMoney;
    }
};
//---------------------------------------------------------------------------
//Client -> Local 
//確定
struct PG_Trade_CtoL_ItemOK
{
    GamePGCommandEnum	Command;

    PG_Trade_CtoL_ItemOK()
    {
        Command = EM_PG_Trade_CtoL_ItemOK;
    }
};
//---------------------------------------------------------------------------
//Local -> Client 
//對方確定 
struct PG_Trade_LtoC_TargetItemOK
{
    GamePGCommandEnum	Command;

    PG_Trade_LtoC_TargetItemOK()
    {
        Command = EM_PG_Trade_LtoC_TargetItemOK;
    }
};
//---------------------------------------------------------------------------
//Local -> Client 
//取消確定
struct PG_Trade_LtoC_CancelItemOK
{
    GamePGCommandEnum	Command;

    PG_Trade_LtoC_CancelItemOK()
    {
        Command = EM_PG_Trade_LtoC_CancelItemOK;
    }
};
//---------------------------------------------------------------------------
//Local -> Client 
//可決定
struct PG_Trade_LtoC_FinalOKEnable
{
    GamePGCommandEnum	Command;

    PG_Trade_LtoC_FinalOKEnable()
    {
        Command = EM_PG_Trade_LtoC_FinalOKEnable;
    }
};
//---------------------------------------------------------------------------
//Client -> Local 
//決定OK
struct PG_Trade_CtoL_FinalOK
{
    GamePGCommandEnum	Command;
	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

    PG_Trade_CtoL_FinalOK()
    {
        Command = EM_PG_Trade_CtoL_FinalOK;
    }
};
//---------------------------------------------------------------------------
enum	FinalOKFailedENUM
{
	EM_FinalOKFailed_PasswordErr	,
	EM_FinalOKFailed_Unknow			,
};
//Local  -> Client 
//決定OK
struct PG_Trade_LtoC_FinalOKFailed
{
	GamePGCommandEnum	Command;
	FinalOKFailedENUM	Result;

	PG_Trade_LtoC_FinalOKFailed()
	{
		Command = EM_PG_Trade_LtoC_FinalOKFailed;
	}
};
//---------------------------------------------------------------------------
//Local -> Client 
//對方決定OK
struct PG_Trade_LtoC_TargetFinalOK
{
    GamePGCommandEnum	Command;

    PG_Trade_LtoC_TargetFinalOK()
    {
        Command = EM_PG_Trade_LtoC_TargetFinalOK;
    }
};
//---------------------------------------------------------------------------
//Local -> Client 
//交易全清
struct PG_Trade_LtoC_ClearTradeItem
{
    GamePGCommandEnum	Command;

    PG_Trade_LtoC_ClearTradeItem()
    {
        Command = EM_PG_Trade_LtoC_ClearTradeItem;
    }
};
//---------------------------------------------------------------------------
//Local -> Client 
//交易終止
//交易成立

struct PG_Trade_LtoC_TradeItemResult
{
    GamePGCommandEnum	Command;

    bool                Result;

    PG_Trade_LtoC_TradeItemResult()
    {
        Command = EM_PG_Trade_LtoC_TradeItemResult;
    }
};
//---------------------------------------------------------------------------
//Client -> Local 
//要求終止交易
struct PG_Trade_CtoL_StopTrade
{
    GamePGCommandEnum	Command;
    int					TargetID;

    PG_Trade_CtoL_StopTrade()
    {
        Command = EM_PG_Trade_CtoL_StopTrade;
    }
};
//---------------------------------------------------------------------------
//Local -> Client
//要求終止交易
struct PG_Trade_LtoC_StopTrade
{
	GamePGCommandEnum	Command;
	int					TargetID;

	PG_Trade_LtoC_StopTrade()
	{
		Command = EM_PG_Trade_LtoC_StopTrade;
	}
};
//---------------------------------------------------------------------------
//Local -> Client
//交易狀態通知
struct PG_Trade_LtoC_TradeState
{
	GamePGCommandEnum	Command;
	bool	TargetTradeOK;
	bool	TradeOK;
	bool	TargetFinalOK;
	bool	FinalOK;

	PG_Trade_LtoC_TradeState()
	{
		Command = EM_PG_Trade_LtoC_TradeState;
	}
};
//---------------------------------------------------------------------------
//Local -> Client
//取消交易請求
struct PG_Trade_LtoC_CancelTradeRequest
{
	GamePGCommandEnum	Command;
	int					TargetID;

	PG_Trade_LtoC_CancelTradeRequest()
	{
		Command = EM_PG_Trade_LtoC_CancelTradeRequest;
	}
};
//---------------------------------------------------------------------------
//Client -> Local
//取消交易請求
struct PG_Trade_CtoL_CancelTradeRequest
{
	GamePGCommandEnum	Command;
	int					TargetID;

	PG_Trade_CtoL_CancelTradeRequest()
	{
		Command = EM_PG_Trade_CtoL_CancelTradeRequest;
	}
};
//---------------------------------------------------------------------------
#pragma managed
