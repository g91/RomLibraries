#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

#define AC_BID_LIMIT_PERCENTAGE		1.05f
#define	_MAX_AC_SELL_ITEM_COUNT_	30	
#define _DEF_AC_BUY_TAXRATE_		0.94
//---------------------------------------------------------------------------------------------------------
//搜尋條件		( -1 代表忽略)
struct	ACSearchConditionStruct
{
	StaticString <_MAX_NAMERECORD_SIZE_>	SearchName;
	int							Type[3];		//類型		主類型 次類型 細項類型
	EqWearSkillStruct			SearchType;		//蒐索類型( )		
	int							LevelHi;		//高等級(等級範圍)	
	int							LevelLow;		//低等級(等級範圍)	
	int							Rare;			//稀有度			
	int							RuneVolume;		//符文容量			
	SexFlagStruct				Sex;			//性別				
	RaceFlagStruct				Race;			//種族			
	VocationFlagStruct			JobType;		//職業類型
	PriceTypeENUM				PriceType;		//金錢類型
	LanguageTypeENUM			Language;
};
//搜尋物件
//Client -> Local
struct PG_AC_CtoL_SearchItem
{
	GamePGCommandEnum	Command;

	ACSearchConditionStruct	Data;

	PG_AC_CtoL_SearchItem( )
	{	Command = EM_PG_AC_CtoL_SearchItem;	}
};
//---------------------------------------------------------------------------------------------------------
//搜尋物件
//Local -> DC
struct PG_AC_LtoDC_SearchItem
{
	GamePGCommandEnum	Command;

	int			DBID;			//玩家腳色DBID
	ACSearchConditionStruct	Data;

	PG_AC_LtoDC_SearchItem( )
	{	Command = EM_PG_AC_LtoDC_SearchItem;	}
};
//---------------------------------------------------------------------------------------------------------
//搜尋結果總量
struct PG_AC_DCtoC_SearchItemCountResult
{
	GamePGCommandEnum	Command;

	int					MaxCount;	//總量
	int					Count;		//要送出的量
	int					BeginIndex;	//開始位置

	PG_AC_DCtoC_SearchItemCountResult( )
	{	Command = EM_PG_AC_DCtoC_SearchItemCountResult;	}
};
//---------------------------------------------------------------------------------------------------------
//拍賣所搜尋出的物品
struct ACItemInfoStruct
{
	int		DBID;
	int		NowPrices;
	int		BuyOutPrices;
	int		LiveTime;

	ItemFieldStruct		Item;	//賣的東西
	PriceTypeENUM Type;
};

//搜尋資料
//DC -> Client
struct PG_AC_DCtoC_SearchItemResult
{
	GamePGCommandEnum	Command;

	int					Index;
	ACItemInfoStruct	Data;
	StaticString <_MAX_NAMERECORD_SIZE_>	SellerName;		//名字
	int					BuyerDBID;

	PG_AC_DCtoC_SearchItemResult( )
	{	Command = EM_PG_AC_DCtoC_SearchItemResult;	}
};
//---------------------------------------------------------------------------------------------------------
//通知原本最高價者競標物被別人出更高價
//DC -> Client
struct PG_AC_DCtoC_LostCompetitiveItem
{
	GamePGCommandEnum	Command;
	
	int					ACItemDBID;
	ItemFieldStruct		Item;

	PG_AC_DCtoC_LostCompetitiveItem( )
	{	Command = EM_PG_AC_DCtoC_LostCompetitiveItem;	}
};
//---------------------------------------------------------------------------------------------------------
//所搜尋出來的資料有修正
//DC -> Client
struct PG_AC_DCtoC_FixSearchItemResult
{
	GamePGCommandEnum	Command;

	int					ACItemDBID;		//AC 物品的資料庫ID
	ACItemStateENUM		State;
	int					NewPrices;		//目前價錢
	int					BuyerDBID;
	PriceTypeENUM		Type;

	PG_AC_DCtoC_FixSearchItemResult( )
	{	Command = EM_PG_AC_DCtoC_FixSearchItemResult;	}
};
//---------------------------------------------------------------------------------------------------------
//定單資料修正
//DC -> Client
struct PG_AC_DCtoC_FixBuyOrderItem
{
	GamePGCommandEnum	Command;

	int					ACItemDBID;		//AC 物品的資料庫ID
	ACItemStateENUM		State;
	int					NewPrices;		//目前價錢
	int					BuyerDBID;
	PriceTypeENUM		Type;

	PG_AC_DCtoC_FixBuyOrderItem( )
	{	Command = EM_PG_AC_DCtoC_FixBuyOrderItem;	}
};
//---------------------------------------------------------------------------------------------------------
//定單資料修正
//DC -> Client
struct PG_AC_DCtoC_FixSellOrderItem
{
	GamePGCommandEnum	Command;

	int					ACItemDBID;		//AC 物品的資料庫ID
	ACItemStateENUM		State;
	int					NewPrices;		//目前價錢
	StaticString <_MAX_NAMERECORD_SIZE_>	BuyerName;		//名字
	PriceTypeENUM		Type;
	PG_AC_DCtoC_FixSellOrderItem( )
	{	Command = EM_PG_AC_DCtoC_FixSellOrderItem;	}
};

//---------------------------------------------------------------------------------------------------------
//取下 50比 的資料
//Client -> Local
struct PG_AC_CtoL_GetNextSearchItem
{
	GamePGCommandEnum	Command;
	
	PG_AC_CtoL_GetNextSearchItem( )
	{	Command = EM_PG_AC_CtoL_GetNextSearchItem;	}
};
//---------------------------------------------------------------------------------------------------------
//取下 50比 的資料
//Local -> DC
struct PG_AC_LtoDC_GetNextSearchItem
{
	GamePGCommandEnum	Command;
	int					DBID;

	PG_AC_LtoDC_GetNextSearchItem( )
	{	Command = EM_PG_AC_LtoDC_GetNextSearchItem;	}
};
//---------------------------------------------------------------------------------------------------------
//要求取得所賣的東西與競標的物品資訊
//Client -> Local
struct PG_AC_CtoL_GetMyACItemInfo
{
	GamePGCommandEnum	Command;

	PG_AC_CtoL_GetMyACItemInfo( )
	{	Command = EM_PG_AC_CtoL_GetMyACItemInfo;	}
};
//---------------------------------------------------------------------------------------------------------
//要求取得所賣的東西與競標的物品資訊
//Local -> DC
struct PG_AC_LtoDC_GetMyACItemInfo
{
	GamePGCommandEnum	Command;
	int					DBID;

	PG_AC_LtoDC_GetMyACItemInfo( )
	{	Command = EM_PG_AC_LtoDC_GetMyACItemInfo;	}
};
//---------------------------------------------------------------------------------------------------------
//通知目前有 自己有 下標的物品
//DC -> Client
struct PG_AC_DCtoC_SelfCompetitive_BuyItem
{
	GamePGCommandEnum	Command;
	ACItemInfoStruct	Data;
	StaticString <_MAX_NAMERECORD_SIZE_>	SellerName;		//名字
	int					BuyerDBID;			//最高價買家的DBID

	PG_AC_DCtoC_SelfCompetitive_BuyItem( )
	{	Command = EM_PG_AC_DCtoC_SelfCompetitive_BuyItem;	}
};

//---------------------------------------------------------------------------------------------------------
//通知自己有賣的物品
//DC -> Client
struct PG_AC_DCtoC_SelfCompetitive_SellItem
{
	GamePGCommandEnum	Command;
	ACItemInfoStruct	Data;		
	StaticString <_MAX_NAMERECORD_SIZE_>	BuyerName;		//名字

	PG_AC_DCtoC_SelfCompetitive_SellItem( )
	{	Command = EM_PG_AC_DCtoC_SelfCompetitive_SellItem;	}
};
//---------------------------------------------------------------------------------------------------------
//
//通知自己有賣的物品
//DC -> Client
struct PG_AC_DCtoC_SelfCompetitive_ItemDataEnd
{
	GamePGCommandEnum	Command;

	PG_AC_DCtoC_SelfCompetitive_ItemDataEnd( )
	{	Command = EM_PG_AC_DCtoC_SelfCompetitive_ItemDataEnd;	}
};

//---------------------------------------------------------------------------------------------------------
//競標物品 或 直接購買
//Client -> Local
struct PG_AC_CtoL_BuyItem
{
	GamePGCommandEnum	Command;
	int					ACItemDBID;		//要買的物品
	int					Prices;			//所出的價錢(如果 等於直接購買價 代表直接購買)
	PriceTypeENUM		Type;
	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

	PG_AC_CtoL_BuyItem( )
	{	Command = EM_PG_AC_CtoL_BuyItem;	}
};
//---------------------------------------------------------------------------------------------------------
//競標物品 或 直接購買
//Local -> DC
struct PG_AC_LtoDC_BuyItem
{
	GamePGCommandEnum	Command;
	int					DBID;			//買者
	int					ACItemDBID;		//要買的物品
	int					Prices;			//所出的價錢(如果 等於直接購買價 代表直接購買)
	PriceTypeENUM		Type;

	PG_AC_LtoDC_BuyItem( )
	{	Command = EM_PG_AC_LtoDC_BuyItem;	}
};
//---------------------------------------------------------------------------------------------------------
//競標物品 或 直接購買 結果
struct PG_AC_DCtoL_BuyItemResult
{
	GamePGCommandEnum	Command;
	bool				Result;
	int					ACItemDBID;		//要買的物品
	int					Prices;			//所出的價錢(如果 等於直接購買價 代表直接購買)
	int					BuyerDBID;
	ACItemStateENUM		State;
	PriceTypeENUM		Type;

	PG_AC_DCtoL_BuyItemResult( )
	{	Command = EM_PG_AC_DCtoL_BuyItemResult;	}
};
//---------------------------------------------------------------------------------------------------------
//競標物品 或 直接購買 結果
struct PG_AC_LtoC_BuyItemResult
{
	GamePGCommandEnum	Command;
	bool				Result;
	int					ACItemDBID;		//要買的物品
	int					Prices;			//所出的價錢(如果 等於直接購買價 代表直接購買)
	ACItemStateENUM		State;
	PriceTypeENUM		Type;

	PG_AC_LtoC_BuyItemResult( )
	{	Command = EM_PG_AC_LtoC_BuyItemResult;	}
};

//---------------------------------------------------------------------------------------------------------
//賣物品
//Client -> Local
struct PG_AC_CtoL_SellItem
{
	GamePGCommandEnum	Command;
	int					BodyPos;
	ItemFieldStruct		Item;			//賣的東西	
	int					Prices;			//基本價
	int					BuyOutPrices;	//直接購買價
	int					LiveTime;		//(分鐘計算)
	PriceTypeENUM		Type;

	PG_AC_CtoL_SellItem( )
	{	Command = EM_PG_AC_CtoL_SellItem;	}
};
//---------------------------------------------------------------------------------------------------------
//賣錢
//Client -> Local
struct PG_AC_CtoL_SellMoney
{
	GamePGCommandEnum	Command;
	PriceTypeENUM		SellType;
	int					SellMoney;

	int					Prices;			//基本價
	int					BuyOutPrices;	//直接購買價
	int					LiveTime;		//(分鐘計算)
	PriceTypeENUM		Type;

	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

	PG_AC_CtoL_SellMoney( )
	{	Command = EM_PG_AC_CtoL_SellMoney;	}
};
//---------------------------------------------------------------------------------------------------------
enum SellMoneyResultENUM
{
	EM_SellMoneyResult_OK			,
	EM_SellMoneyResult_Err			,	//一般錯物 (不可交易狀態)
	EM_SellMoneyResult_MoneyErr		,	//金錢不足
	EM_SellMoneyResult_MoneyTypeErr	,	//金錢類型有問題( 用遊戲幣換遊戲幣 , 或用帳號幣換帳號幣 )
	EM_SellMoneyResult_PasswordErr	,
};
//賣錢 結果
//Local -> Client
struct PG_AC_LtoC_SellMoneyResult
{
	GamePGCommandEnum	Command;
	SellMoneyResultENUM	Result;
	ACItemInfoStruct	Item;
	
	PG_AC_LtoC_SellMoneyResult( )
	{	Command = EM_PG_AC_LtoC_SellMoneyResult;	}
};

//---------------------------------------------------------------------------------------------------------
//賣物品
//Local -> DC
struct PG_AC_LtoDC_SellItem
{
	GamePGCommandEnum	Command;
	ItemFieldStruct		Item;			//賣的東西	
	int					Prices;			//基本價
	int					BuyOutPrices;	//直接購買價
	int					DBID;			//誰要賣
	int					LiveTime;		//生存時間
	int					TaxMoney;		//稅金(錯誤時回傳修正用)
	PriceTypeENUM		Type;

	PG_AC_LtoDC_SellItem( )
	{	Command = EM_PG_AC_LtoDC_SellItem;	}
};
//---------------------------------------------------------------------------------------------------------
enum ACSellItemResultENUM
{
	EM_ACSellItemResult_OK				,	//成?
	EM_ACSellItemResult_DataError		,	//資料錯誤
	EM_ACSellItemResult_MaxCountError	,	//數量到達上限
	EM_ACSellItemResult_DisableTrade	,	//禁止交易
	EM_ACSellItemResult_MoneyError		,	//金錢錯誤
	EM_ACSellItemResult_EqFixError		,	//物品需要修理
};
//賣物品結果
//DC -> Local
struct PG_AC_DCtoL_SellItemResult
{
	GamePGCommandEnum	Command;

	//bool				Result;			//是否成?
	ACSellItemResultENUM Result;

	ACItemInfoStruct	Item;			//賣的東西	
	int					SellerDBID;
	int					TaxMoney;		//稅金(錯誤時回傳修正用)

	PG_AC_DCtoL_SellItemResult( )
	{	Command = EM_PG_AC_DCtoL_SellItemResult;	}
};
//---------------------------------------------------------------------------------------------------------
//賣物品結果
//Local -> Client
struct PG_AC_LtoC_SellItemResult
{
	GamePGCommandEnum	Command;

	ACItemInfoStruct	Item;			//賣的東西	
	//bool				Result;			//是否成?
	ACSellItemResultENUM Result;

	PG_AC_LtoC_SellItemResult( )
	{	Command = EM_PG_AC_LtoC_SellItemResult;	}
};


//---------------------------------------------------------------------------------------------------------
//取消拍賣
//Client -> Local
struct PG_AC_CtoL_CancelSellItem
{
	GamePGCommandEnum	Command;
	int					ACItemDBID;			//AC 物品的DBID

	PG_AC_CtoL_CancelSellItem( )
	{	Command = EM_PG_AC_CtoL_CancelSellItem;	}
};
//---------------------------------------------------------------------------------------------------------
//取消拍賣
//Local -> DC
struct PG_AC_LtoDC_CancelSellItem
{
	GamePGCommandEnum	Command;
	int					DBID;				//擁有者 ( 賣者 )
	int					ACItemDBID;			//AC 物品的DBID

	PG_AC_LtoDC_CancelSellItem( )
	{	Command = EM_PG_AC_LtoDC_CancelSellItem;	}
};
//---------------------------------------------------------------------------------------------------------
//回應取消拍賣結果
// Datacenter -> Client
struct PG_AC_DCtoC_CancelSellItemResult
{
	GamePGCommandEnum	Command;
	int					ACItemDBID;			//AC 物品的DBID
	bool				Result;				

	PG_AC_DCtoC_CancelSellItemResult( )
	{	Command = EM_PG_AC_DCtoC_CancelSellItemResult;	}
};
//---------------------------------------------------------------------------------------------------------
//開啟拍賣所
//DataCenter -> Client
struct PG_AC_DCtoC_OpenAC
{
	GamePGCommandEnum	Command;
	bool		Result;
	int			NpcGUID;				//處理的NPC

	PG_AC_DCtoC_OpenAC( )
	{	Command = EM_PG_AC_DCtoC_OpenAC;	}
};
//---------------------------------------------------------------------------------------------------------
//開啟拍賣所
//Local -> DataCenter
struct PG_AC_LtoDC_OpenAC
{
	GamePGCommandEnum	Command;
	int					DBID;			//要求的玩家
	int					NpcGUID;		//處理的NPC

	PG_AC_LtoDC_OpenAC( )
	{	Command = EM_PG_AC_LtoDC_OpenAC;	}
};
//---------------------------------------------------------------------------------------------------------
//關閉拍賣所
//Client -> Local
struct PG_AC_CtoL_CloseAC
{
	GamePGCommandEnum	Command;

	PG_AC_CtoL_CloseAC( )
	{	Command = EM_PG_AC_CtoL_CloseAC;	}
};
//---------------------------------------------------------------------------------------------------------
//關閉拍賣所
//DataCenter -> Client
struct PG_AC_DCtoC_CloseAC
{
	GamePGCommandEnum	Command;

	PG_AC_DCtoC_CloseAC( )
	{	Command = EM_PG_AC_DCtoC_CloseAC;	}
};
//---------------------------------------------------------------------------------------------------------
//關閉拍賣所
//Local -> DC
struct PG_AC_LtoDC_CloseAC
{
	GamePGCommandEnum	Command;
	int					DBID;

	PG_AC_LtoDC_CloseAC( )
	{	Command = EM_PG_AC_LtoDC_CloseAC;	}
};
//---------------------------------------------------------------------------------------------------------
//取得某物件的歷史資料
//Client -> Local
struct PG_AC_CtoL_ItemHistoryRequest
{
	GamePGCommandEnum	Command;
	int		OrgObjID;
	int		Inherent;		//不使用

	PG_AC_CtoL_ItemHistoryRequest( )
	{	Command = EM_PG_AC_CtoL_ItemHistoryRequest;	}
};
//---------------------------------------------------------------------------------------------------------
//取得某物件的歷史資料
//Local -> DataCenter
struct PG_AC_LtoDC_ItemHistoryRequest
{
	GamePGCommandEnum	Command;
	int		DBID;			//玩家DBID
	int		OrgObjID;
	int		Inherent;

	PG_AC_LtoDC_ItemHistoryRequest( )
	{	Command = EM_PG_AC_LtoDC_ItemHistoryRequest;	}
};
//---------------------------------------------------------------------------------------------------------
#define	_MAX_HISTORY_ITEM_COUNT_ 10
struct PG_AC_DCtoC_ItemHistoryListResult
{
	GamePGCommandEnum	Command;
	int		Count;
	DB_ACItemHistoryStruct ItemHistory[_MAX_HISTORY_ITEM_COUNT_ ];

	PG_AC_DCtoC_ItemHistoryListResult( )
	{	Command = EM_PG_AC_DCtoC_ItemHistoryListResult;	}

};
//---------------------------------------------------------------------------------------------------------
//AC已在開啟中
//Local -> Client
struct PG_AC_LtoC_AcAlreadyOpen
{
	GamePGCommandEnum	Command;
	int		TargtetNPC;

	PG_AC_LtoC_AcAlreadyOpen( )
	{	Command = EM_PG_AC_LtoC_AcAlreadyOpen;	}
};
//---------------------------------------------------------------------------------------------------------
//AC 交易完成通知買賣方
struct PG_AC_DtoC_AcTradeOk
{
	GamePGCommandEnum	Command;
	StaticString <_MAX_NAMERECORD_SIZE_>	BuyerName;
	StaticString <_MAX_NAMERECORD_SIZE_>	SellerName;
	PriceTypeENUM		PriceType;
	int					Money;
	int					Tax;
	ItemFieldStruct		Item;

	PG_AC_DtoC_AcTradeOk( )
	{	Command = EM_PG_AC_DtoC_AcTradeOk;	}
};
//---------------------------------------------------------------------------------------------------------

#pragma managed

