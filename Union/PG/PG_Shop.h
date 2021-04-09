#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

//------------------------------------------------------------------------------------------
//CServer -> Client
//�}�ҶR��
struct PG_Shop_LtoC_ShopOpen
{
    GamePGCommandEnum	Command;

    int					TraderID;       //�ӤH��ID
/*
    int                 BuyRate;        //�R��v(%) (���)
    int                 SellRate;       //���v(%) (���)
    bool                RepairWeapon;   //�Z���׸�

    int                 ItemID[30];
    */
    int                 ShopObjID;
    int                 ItemCount[30];

    PG_Shop_LtoC_ShopOpen()
    {
        Command = EM_PG_Shop_LtoC_ShopOpen;
    };
};
//------------------------------------------------------------------------------------------

//Server -> Client
//�����R��
struct PG_Shop_LtoC_ShopClose
{
    GamePGCommandEnum	Command;

    int					TraderID;       //�ӤH��ID

    PG_Shop_LtoC_ShopClose()
    {
        Command = EM_PG_Shop_LtoC_ShopClose;
    };
};

//------------------------------------------------------------------------------------------
//Client -> Server
//�����R��
struct PG_Shop_CtoL_ShopCloseNotify
{
    GamePGCommandEnum	Command;

//    int					TraderID;       //�ӤH��ID

    PG_Shop_CtoL_ShopCloseNotify()
    {
        Command = EM_PG_Shop_CtoL_ShopCloseNotify;
    };
};
//------------------------------------------------------------------------------------------
//Client -> Server
//�R���~
struct PG_Shop_CtoL_BuyItemRequest
{
    GamePGCommandEnum	Command;

//    int					TraderID;   //�ӤH��ID
    int                 OrgObjID;   //�R���F�誺�Ҫ�ID
    int					Count;
	int					Pos;

    PG_Shop_CtoL_BuyItemRequest( )
    {
        Command = EM_PG_Shop_CtoL_BuyItemRequest;
    };
};

//------------------------------------------------------------------------------------------
//Local -> Client
//�R���~���G
struct PG_Shop_LtoC_BuyItemResult
{
    GamePGCommandEnum	Command;

    int					TraderID;   //�ӤH��ID
    int                 OrgObjID;   //�R���F�誺�Ҫ�ID
    int					Count;
    bool                Result;

	PriceTypeENUM		PriceType[2];
	int					Money[2];

    PG_Shop_LtoC_BuyItemResult( )
    {
        Command = EM_PG_Shop_LtoC_BuyItemResult;
    };
};
//------------------------------------------------------------------------------------------
//Client -> Local ��h�檫�~
struct PG_Shop_CtoL_SelectSellItemRequest
{
    GamePGCommandEnum	Command;

//    int					TraderID; //�ӤH��ID
    int					FieldID;    //���~�쥻�Ҧb��m
    ItemFieldStruct		SellItem;	//�T�{�檺���~�S���~

    PG_Shop_CtoL_SelectSellItemRequest()
    {
        Command = EM_PG_Shop_CtoL_SelectSellItemRequest;
    };
};
//------------------------------------------------------------------------------------------
//Local -> Client ��h�檫�~���G
struct PG_Shop_LtoC_SelectSellItemResult
{
    GamePGCommandEnum	Command;

    bool                Result;
    int					TraderID; //�ӤH��ID
    int					FieldID;    //���~�쥻�Ҧb��m    


    PG_Shop_LtoC_SelectSellItemResult()
    {
        Command = EM_PG_Shop_LtoC_SelectSellItemResult;
    };
};
//------------------------------------------------------------------------------------------
/*
//Client -> Local
//�檫�~���G
struct PG_Shop_CtoL_SellA11ItemRequest
{

    GamePGCommandEnum	Command;
//    int     TraderID;


    PG_Shop_CtoL_SellA11ItemRequest()
    {
        Command = EM_PG_Shop_CtoL_SellA11ItemRequest;
    };
};
//------------------------------------------------------------------------------------------
//Local -> Client
//�檫�~���G
struct PG_Shop_LtoC_SellA11ItemResult
{

    GamePGCommandEnum	Command;
    bool                Result;
    int     TraderID;


    PG_Shop_LtoC_SellA11ItemResult()
    {
        Command = EM_PG_Shop_LtoC_SellA11ItemResult;
    };
};

//------------------------------------------------------------------------------------------
//Client -> Local
//�檫�~���G
struct PG_Shop_CtoL_ClsSellA11Item
{

    GamePGCommandEnum	Command;
//    int     TraderID;

    PG_Shop_CtoL_ClsSellA11Item()
    {
        Command = EM_PG_Shop_CtoL_ClsSellA11Item;
    };
};
*/
//------------------------------------------------------------------------------------------
//�ײz�Z��
//Client -> Local
struct PG_Shop_CtoL_RepairWeaponRequest
{
    GamePGCommandEnum	Command;

//    int					TraderID;   //�ӤH��ID
    int                 Pos;        //���@����m -1 �N�����
    bool                IsWeapon;

    PG_Shop_CtoL_RepairWeaponRequest()
    {
        Command = EM_PG_Shop_CtoL_RepairWeaponRequest;
    };
};
//------------------------------------------------------------------------------------------
//�ײz�Z��
//Local -> Client 
struct PG_Shop_LtoC_RepairWeaponResult
{
    GamePGCommandEnum	Command;

    bool                Result;
    int					TraderID;   //�ӤH��ID
    int                 Pos;        //���@����m -1 �N�����
    bool                IsWeapon;

    PG_Shop_LtoC_RepairWeaponResult()
    {
        Command = EM_PG_Shop_LtoC_RepairWeaponResult;
    };
};
//------------------------------------------------------------------------------------------
//�s�W�Q��X�����~
struct PG_Shop_LtoC_AddSellItemRecord
{
	GamePGCommandEnum	Command;
	SellItemFieldStruct	Info;

	PG_Shop_LtoC_AddSellItemRecord()
	{
		Command = EM_PG_Shop_LtoC_AddSellItemRecord;
	};
};
//�����Q��X�����~
struct PG_Shop_LtoC_DelSellItemRecord
{
	GamePGCommandEnum	Command;
	//SellItemFieldStruct	Info;
	int		EarsePos;

	PG_Shop_LtoC_DelSellItemRecord()
	{
		Command = EM_PG_Shop_LtoC_DelSellItemRecord;
	};
};

//�R�^��X�����~
struct PG_Shop_CtoL_BuySellItemRecord
{
	GamePGCommandEnum	Command;
	SellItemFieldStruct	Info;

	PG_Shop_CtoL_BuySellItemRecord()
	{
		Command = EM_PG_Shop_CtoL_BuySellItemRecord;
	};
};

//��s�Y��쪫�~���d��q
struct PG_Shop_LtoC_FixShopItemCount
{
	GamePGCommandEnum	Command;

	int					TraderID;       //�ӤH��ID
	int                 ShopObjID;
	int					Pos;			//���~��m
	int                 ItemCount;		//���~�ƶq

	PG_Shop_LtoC_FixShopItemCount()
	{
		Command = EM_PG_Shop_LtoC_FixShopItemCount;
	};
};
//------------------------------------------------------------------------------------------
//Client �ݭn�D�ʶR�I�ưӫ~
struct PG_Shop_CtoL_AccountMoneyShop
{
	GamePGCommandEnum	Command;

	int		OrgObjID;			//���󸹽X
	int		Money_Account;		//��O�I��

	PG_Shop_CtoL_AccountMoneyShop()
	{
		Command = EM_PG_Shop_CtoL_AccountMoneyShop;
	};
};

//------------------------------------------------------------------------------------------
enum AccountMoneyShopResultENUM
{
	EM_AccountMoneyShopResult_OK				,
	EM_AccountMoneyShopResult_MoneyNotEnough	,
	EM_AccountMoneyShopResult_PocketFull		,	//����]�q���F
	EM_AccountMoneyShopResult_PocketPosErr		,
	EM_AccountMoneyShopResult_Failed			,	//�n�D��Ʀ����D

};
//Client �ݭn�D�ʶR�I�ưӫ~
struct PG_Shop_LtoC_AccountMoneyShopResult
{
	GamePGCommandEnum	Command;

	int		OrgObjID;			//���󸹽X
	AccountMoneyShopResultENUM	Result;

	PG_Shop_LtoC_AccountMoneyShopResult()
	{
		Command = EM_PG_Shop_LtoC_AccountMoneyShopResult;
	};
};
//------------------------------------------------------------------------------------------
//Client �ݭn�D�ʶR�I�ưӫ~
// Client -> Local 
struct AccountMoneyBuyStruct
{
	int	SellID;		//-1�N��S�����ӫ~
	int Pos;
	int Count;
};
struct PG_Shop_CtoL_AccountMoneyShopBuyList
{
	GamePGCommandEnum	Command;

	AccountMoneyBuyStruct	BuyList[20];	//�ʶR�C��
	int	TotalCost;							//�`��

	PG_Shop_CtoL_AccountMoneyShopBuyList()
	{
		Command = EM_PG_Shop_CtoL_AccountMoneyShopBuyList;
	};
};
//------------------------------------------------------------------------------------------
struct PG_Shop_LtoC_AccountMoneyShopBuyListResult
{
	GamePGCommandEnum	Command;
	AccountMoneyShopResultENUM	Result;

	PG_Shop_LtoC_AccountMoneyShopBuyListResult()
	{
		Command = EM_PG_Shop_LtoC_AccountMoneyShopBuyListResult;
	};
};
//------------------------------------------------------------------------------------------
struct PG_Shop_LtoC_FixSellItemBackup
{
	GamePGCommandEnum	Command;
	StaticVector< SellItemFieldStruct , _MAX_SELL_ITEM_SAVE_COUNT_ >	SellItemBackup;

	PG_Shop_LtoC_FixSellItemBackup()
	{
		Command = EM_PG_Shop_LtoC_FixSellItemBackup;
	};
};
#pragma managed
