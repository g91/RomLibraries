#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged
//---------------------------------------------------------------------------
//A Client -> Local 
//�n�D���
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
// �n�D����ؼЦ����D
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
//����ӽ�
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
//�P�N���( �Τ��P�N )
struct PG_Trade_CtoL_AgreeTrade
{
    GamePGCommandEnum	Command;
    int					TargetID;
	bool				Result;
	int					ClientResult;		//Client��e���n�Ъ�

    PG_Trade_CtoL_AgreeTrade()
    {
        Command = EM_PG_Trade_CtoL_AgreeTrade;
    }
};
//---------------------------------------------------------------------------
// Local -> A Client
//��褣�P�N���
struct PG_Trade_LtoC_DisagreeTrade
{
	GamePGCommandEnum	Command;
	int					ClientResult;		//Client��e���n�Ъ�

	PG_Trade_LtoC_DisagreeTrade()
	{
		Command = EM_PG_Trade_LtoC_DisagreeTrade;
	}
};

//---------------------------------------------------------------------------
//Local -> Client 
//�}�ҥ��
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
//�񪫫~		
struct PG_Trade_CtoL_PutItem
{
    GamePGCommandEnum	Command;

    int					Pos;
    int					ItemPos;		//���~���
    ItemFieldStruct		Item;

    PG_Trade_CtoL_PutItem()
    {
        Command = EM_PG_Trade_CtoL_PutItem;
    }
};
//---------------------------------------------------------------------------
//Local -> Client 
//���񪫫~

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
//���񪫫~

struct PG_Trade_LtoC_TargetPutItem
{
    GamePGCommandEnum	Command;

    int					Pos;			//������
    ItemFieldStruct		Item;

    PG_Trade_LtoC_TargetPutItem()
    {
        Command = EM_PG_Trade_LtoC_TargetPutItem;
    }
};
//---------------------------------------------------------------------------
//Client -> Local 
//�����
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
//�����
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
//�ۤv���
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
//�T�w
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
//���T�w 
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
//�����T�w
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
//�i�M�w
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
//�M�wOK
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
//�M�wOK
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
//���M�wOK
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
//������M
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
//����פ�
//�������

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
//�n�D�פ���
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
//�n�D�פ���
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
//������A�q��
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
//��������ШD
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
//��������ШD
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
