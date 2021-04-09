#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

//------------------------------------------------------------------------------------------
//CServer -> Client
//開啟買賣
struct PG_Shop_LtoC_ShopOpen
{
    GamePGCommandEnum	Command;

    int					TraderID;       //商人的ID
/*
    int                 BuyRate;        //買比率(%) (原價)
    int                 SellRate;       //賣比率(%) (原價)
    bool                RepairWeapon;   //武器修裡

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
//關閉買賣
struct PG_Shop_LtoC_ShopClose
{
    GamePGCommandEnum	Command;

    int					TraderID;       //商人的ID

    PG_Shop_LtoC_ShopClose()
    {
        Command = EM_PG_Shop_LtoC_ShopClose;
    };
};

//------------------------------------------------------------------------------------------
//Client -> Server
//關閉買賣
struct PG_Shop_CtoL_ShopCloseNotify
{
    GamePGCommandEnum	Command;

//    int					TraderID;       //商人的ID

    PG_Shop_CtoL_ShopCloseNotify()
    {
        Command = EM_PG_Shop_CtoL_ShopCloseNotify;
    };
};
//------------------------------------------------------------------------------------------
//Client -> Server
//買物品
struct PG_Shop_CtoL_BuyItemRequest
{
    GamePGCommandEnum	Command;

//    int					TraderID;   //商人的ID
    int                 OrgObjID;   //買的東西的模版ID
    int					Count;
	int					Pos;

    PG_Shop_CtoL_BuyItemRequest( )
    {
        Command = EM_PG_Shop_CtoL_BuyItemRequest;
    };
};

//------------------------------------------------------------------------------------------
//Local -> Client
//買物品結果
struct PG_Shop_LtoC_BuyItemResult
{
    GamePGCommandEnum	Command;

    int					TraderID;   //商人的ID
    int                 OrgObjID;   //買的東西的模版ID
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
//Client -> Local 選則賣物品
struct PG_Shop_CtoL_SelectSellItemRequest
{
    GamePGCommandEnum	Command;

//    int					TraderID; //商人的ID
    int					FieldID;    //物品原本所在位置
    ItemFieldStruct		SellItem;	//確認賣的物品沒錯誤

    PG_Shop_CtoL_SelectSellItemRequest()
    {
        Command = EM_PG_Shop_CtoL_SelectSellItemRequest;
    };
};
//------------------------------------------------------------------------------------------
//Local -> Client 選則賣物品結果
struct PG_Shop_LtoC_SelectSellItemResult
{
    GamePGCommandEnum	Command;

    bool                Result;
    int					TraderID; //商人的ID
    int					FieldID;    //物品原本所在位置    


    PG_Shop_LtoC_SelectSellItemResult()
    {
        Command = EM_PG_Shop_LtoC_SelectSellItemResult;
    };
};
//------------------------------------------------------------------------------------------
/*
//Client -> Local
//賣物品結果
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
//賣物品結果
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
//賣物品結果
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
//修理武器
//Client -> Local
struct PG_Shop_CtoL_RepairWeaponRequest
{
    GamePGCommandEnum	Command;

//    int					TraderID;   //商人的ID
    int                 Pos;        //修護的位置 -1 代表全部
    bool                IsWeapon;

    PG_Shop_CtoL_RepairWeaponRequest()
    {
        Command = EM_PG_Shop_CtoL_RepairWeaponRequest;
    };
};
//------------------------------------------------------------------------------------------
//修理武器
//Local -> Client 
struct PG_Shop_LtoC_RepairWeaponResult
{
    GamePGCommandEnum	Command;

    bool                Result;
    int					TraderID;   //商人的ID
    int                 Pos;        //修護的位置 -1 代表全部
    bool                IsWeapon;

    PG_Shop_LtoC_RepairWeaponResult()
    {
        Command = EM_PG_Shop_LtoC_RepairWeaponResult;
    };
};
//------------------------------------------------------------------------------------------
//新增被賣出的物品
struct PG_Shop_LtoC_AddSellItemRecord
{
	GamePGCommandEnum	Command;
	SellItemFieldStruct	Info;

	PG_Shop_LtoC_AddSellItemRecord()
	{
		Command = EM_PG_Shop_LtoC_AddSellItemRecord;
	};
};
//移除被賣出的物品
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

//買回賣出的物品
struct PG_Shop_CtoL_BuySellItemRecord
{
	GamePGCommandEnum	Command;
	SellItemFieldStruct	Info;

	PG_Shop_CtoL_BuySellItemRecord()
	{
		Command = EM_PG_Shop_CtoL_BuySellItemRecord;
	};
};

//更新某欄位物品的範賣量
struct PG_Shop_LtoC_FixShopItemCount
{
	GamePGCommandEnum	Command;

	int					TraderID;       //商人的ID
	int                 ShopObjID;
	int					Pos;			//物品位置
	int                 ItemCount;		//物品數量

	PG_Shop_LtoC_FixShopItemCount()
	{
		Command = EM_PG_Shop_LtoC_FixShopItemCount;
	};
};
//------------------------------------------------------------------------------------------
//Client 端要求購買點數商品
struct PG_Shop_CtoL_AccountMoneyShop
{
	GamePGCommandEnum	Command;

	int		OrgObjID;			//物件號碼
	int		Money_Account;		//花費點數

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
	EM_AccountMoneyShopResult_PocketFull		,	//身體包裹滿了
	EM_AccountMoneyShopResult_PocketPosErr		,
	EM_AccountMoneyShopResult_Failed			,	//要求資料有問題

};
//Client 端要求購買點數商品
struct PG_Shop_LtoC_AccountMoneyShopResult
{
	GamePGCommandEnum	Command;

	int		OrgObjID;			//物件號碼
	AccountMoneyShopResultENUM	Result;

	PG_Shop_LtoC_AccountMoneyShopResult()
	{
		Command = EM_PG_Shop_LtoC_AccountMoneyShopResult;
	};
};
//------------------------------------------------------------------------------------------
//Client 端要求購買點數商品
// Client -> Local 
struct AccountMoneyBuyStruct
{
	int	SellID;		//-1代表沒有此商品
	int Pos;
	int Count;
};
struct PG_Shop_CtoL_AccountMoneyShopBuyList
{
	GamePGCommandEnum	Command;

	AccountMoneyBuyStruct	BuyList[20];	//購買列表
	int	TotalCost;							//總價

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
