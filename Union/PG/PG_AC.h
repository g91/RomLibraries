#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

#define AC_BID_LIMIT_PERCENTAGE		1.05f
#define	_MAX_AC_SELL_ITEM_COUNT_	30	
#define _DEF_AC_BUY_TAXRATE_		0.94
//---------------------------------------------------------------------------------------------------------
//·j´M±ø¥ó		( -1 ¥Nªí©¿²¤)
struct	ACSearchConditionStruct
{
	StaticString <_MAX_NAMERECORD_SIZE_>	SearchName;
	int							Type[3];		//Ãþ«¬		¥DÃþ«¬ ¦¸Ãþ«¬ ²Ó¶µÃþ«¬
	EqWearSkillStruct			SearchType;		//»`¯ÁÃþ«¬( )		
	int							LevelHi;		//°ªµ¥¯Å(µ¥¯Å½d³ò)	
	int							LevelLow;		//§Cµ¥¯Å(µ¥¯Å½d³ò)	
	int							Rare;			//µ}¦³«×			
	int							RuneVolume;		//²Å¤å®e¶q			
	SexFlagStruct				Sex;			//©Ê§O				
	RaceFlagStruct				Race;			//ºØ±Ú			
	VocationFlagStruct			JobType;		//Â¾·~Ãþ«¬
	PriceTypeENUM				PriceType;		//ª÷¿úÃþ«¬
	LanguageTypeENUM			Language;
};
//·j´Mª«¥ó
//Client -> Local
struct PG_AC_CtoL_SearchItem
{
	GamePGCommandEnum	Command;

	ACSearchConditionStruct	Data;

	PG_AC_CtoL_SearchItem( )
	{	Command = EM_PG_AC_CtoL_SearchItem;	}
};
//---------------------------------------------------------------------------------------------------------
//·j´Mª«¥ó
//Local -> DC
struct PG_AC_LtoDC_SearchItem
{
	GamePGCommandEnum	Command;

	int			DBID;			//ª±®a¸}¦âDBID
	ACSearchConditionStruct	Data;

	PG_AC_LtoDC_SearchItem( )
	{	Command = EM_PG_AC_LtoDC_SearchItem;	}
};
//---------------------------------------------------------------------------------------------------------
//·j´Mµ²ªGÁ`¶q
struct PG_AC_DCtoC_SearchItemCountResult
{
	GamePGCommandEnum	Command;

	int					MaxCount;	//Á`¶q
	int					Count;		//­n°e¥Xªº¶q
	int					BeginIndex;	//¶}©l¦ì¸m

	PG_AC_DCtoC_SearchItemCountResult( )
	{	Command = EM_PG_AC_DCtoC_SearchItemCountResult;	}
};
//---------------------------------------------------------------------------------------------------------
//©ç½æ©Ò·j´M¥Xªºª««~
struct ACItemInfoStruct
{
	int		DBID;
	int		NowPrices;
	int		BuyOutPrices;
	int		LiveTime;

	ItemFieldStruct		Item;	//½æªºªF¦è
	PriceTypeENUM Type;
};

//·j´M¸ê®Æ
//DC -> Client
struct PG_AC_DCtoC_SearchItemResult
{
	GamePGCommandEnum	Command;

	int					Index;
	ACItemInfoStruct	Data;
	StaticString <_MAX_NAMERECORD_SIZE_>	SellerName;		//¦W¦r
	int					BuyerDBID;

	PG_AC_DCtoC_SearchItemResult( )
	{	Command = EM_PG_AC_DCtoC_SearchItemResult;	}
};
//---------------------------------------------------------------------------------------------------------
//³qª¾­ì¥»³Ì°ª»ùªÌÄv¼Ðª«³Q§O¤H¥X§ó°ª»ù
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
//©Ò·j´M¥X¨Óªº¸ê®Æ¦³­×¥¿
//DC -> Client
struct PG_AC_DCtoC_FixSearchItemResult
{
	GamePGCommandEnum	Command;

	int					ACItemDBID;		//AC ª««~ªº¸ê®Æ®wID
	ACItemStateENUM		State;
	int					NewPrices;		//¥Ø«e»ù¿ú
	int					BuyerDBID;
	PriceTypeENUM		Type;

	PG_AC_DCtoC_FixSearchItemResult( )
	{	Command = EM_PG_AC_DCtoC_FixSearchItemResult;	}
};
//---------------------------------------------------------------------------------------------------------
//©w³æ¸ê®Æ­×¥¿
//DC -> Client
struct PG_AC_DCtoC_FixBuyOrderItem
{
	GamePGCommandEnum	Command;

	int					ACItemDBID;		//AC ª««~ªº¸ê®Æ®wID
	ACItemStateENUM		State;
	int					NewPrices;		//¥Ø«e»ù¿ú
	int					BuyerDBID;
	PriceTypeENUM		Type;

	PG_AC_DCtoC_FixBuyOrderItem( )
	{	Command = EM_PG_AC_DCtoC_FixBuyOrderItem;	}
};
//---------------------------------------------------------------------------------------------------------
//©w³æ¸ê®Æ­×¥¿
//DC -> Client
struct PG_AC_DCtoC_FixSellOrderItem
{
	GamePGCommandEnum	Command;

	int					ACItemDBID;		//AC ª««~ªº¸ê®Æ®wID
	ACItemStateENUM		State;
	int					NewPrices;		//¥Ø«e»ù¿ú
	StaticString <_MAX_NAMERECORD_SIZE_>	BuyerName;		//¦W¦r
	PriceTypeENUM		Type;
	PG_AC_DCtoC_FixSellOrderItem( )
	{	Command = EM_PG_AC_DCtoC_FixSellOrderItem;	}
};

//---------------------------------------------------------------------------------------------------------
//¨ú¤U 50¤ñ ªº¸ê®Æ
//Client -> Local
struct PG_AC_CtoL_GetNextSearchItem
{
	GamePGCommandEnum	Command;
	
	PG_AC_CtoL_GetNextSearchItem( )
	{	Command = EM_PG_AC_CtoL_GetNextSearchItem;	}
};
//---------------------------------------------------------------------------------------------------------
//¨ú¤U 50¤ñ ªº¸ê®Æ
//Local -> DC
struct PG_AC_LtoDC_GetNextSearchItem
{
	GamePGCommandEnum	Command;
	int					DBID;

	PG_AC_LtoDC_GetNextSearchItem( )
	{	Command = EM_PG_AC_LtoDC_GetNextSearchItem;	}
};
//---------------------------------------------------------------------------------------------------------
//­n¨D¨ú±o©Ò½æªºªF¦è»PÄv¼Ðªºª««~¸ê°T
//Client -> Local
struct PG_AC_CtoL_GetMyACItemInfo
{
	GamePGCommandEnum	Command;

	PG_AC_CtoL_GetMyACItemInfo( )
	{	Command = EM_PG_AC_CtoL_GetMyACItemInfo;	}
};
//---------------------------------------------------------------------------------------------------------
//­n¨D¨ú±o©Ò½æªºªF¦è»PÄv¼Ðªºª««~¸ê°T
//Local -> DC
struct PG_AC_LtoDC_GetMyACItemInfo
{
	GamePGCommandEnum	Command;
	int					DBID;

	PG_AC_LtoDC_GetMyACItemInfo( )
	{	Command = EM_PG_AC_LtoDC_GetMyACItemInfo;	}
};
//---------------------------------------------------------------------------------------------------------
//³qª¾¥Ø«e¦³ ¦Û¤v¦³ ¤U¼Ðªºª««~
//DC -> Client
struct PG_AC_DCtoC_SelfCompetitive_BuyItem
{
	GamePGCommandEnum	Command;
	ACItemInfoStruct	Data;
	StaticString <_MAX_NAMERECORD_SIZE_>	SellerName;		//¦W¦r
	int					BuyerDBID;			//³Ì°ª»ù¶R®aªºDBID

	PG_AC_DCtoC_SelfCompetitive_BuyItem( )
	{	Command = EM_PG_AC_DCtoC_SelfCompetitive_BuyItem;	}
};

//---------------------------------------------------------------------------------------------------------
//³qª¾¦Û¤v¦³½æªºª««~
//DC -> Client
struct PG_AC_DCtoC_SelfCompetitive_SellItem
{
	GamePGCommandEnum	Command;
	ACItemInfoStruct	Data;		
	StaticString <_MAX_NAMERECORD_SIZE_>	BuyerName;		//¦W¦r

	PG_AC_DCtoC_SelfCompetitive_SellItem( )
	{	Command = EM_PG_AC_DCtoC_SelfCompetitive_SellItem;	}
};
//---------------------------------------------------------------------------------------------------------
//
//³qª¾¦Û¤v¦³½æªºª««~
//DC -> Client
struct PG_AC_DCtoC_SelfCompetitive_ItemDataEnd
{
	GamePGCommandEnum	Command;

	PG_AC_DCtoC_SelfCompetitive_ItemDataEnd( )
	{	Command = EM_PG_AC_DCtoC_SelfCompetitive_ItemDataEnd;	}
};

//---------------------------------------------------------------------------------------------------------
//Äv¼Ðª««~ ©Î ª½±µÁÊ¶R
//Client -> Local
struct PG_AC_CtoL_BuyItem
{
	GamePGCommandEnum	Command;
	int					ACItemDBID;		//­n¶Rªºª««~
	int					Prices;			//©Ò¥Xªº»ù¿ú(¦pªG µ¥©óª½±µÁÊ¶R»ù ¥Nªíª½±µÁÊ¶R)
	PriceTypeENUM		Type;
	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

	PG_AC_CtoL_BuyItem( )
	{	Command = EM_PG_AC_CtoL_BuyItem;	}
};
//---------------------------------------------------------------------------------------------------------
//Äv¼Ðª««~ ©Î ª½±µÁÊ¶R
//Local -> DC
struct PG_AC_LtoDC_BuyItem
{
	GamePGCommandEnum	Command;
	int					DBID;			//¶RªÌ
	int					ACItemDBID;		//­n¶Rªºª««~
	int					Prices;			//©Ò¥Xªº»ù¿ú(¦pªG µ¥©óª½±µÁÊ¶R»ù ¥Nªíª½±µÁÊ¶R)
	PriceTypeENUM		Type;

	PG_AC_LtoDC_BuyItem( )
	{	Command = EM_PG_AC_LtoDC_BuyItem;	}
};
//---------------------------------------------------------------------------------------------------------
//Äv¼Ðª««~ ©Î ª½±µÁÊ¶R µ²ªG
struct PG_AC_DCtoL_BuyItemResult
{
	GamePGCommandEnum	Command;
	bool				Result;
	int					ACItemDBID;		//­n¶Rªºª««~
	int					Prices;			//©Ò¥Xªº»ù¿ú(¦pªG µ¥©óª½±µÁÊ¶R»ù ¥Nªíª½±µÁÊ¶R)
	int					BuyerDBID;
	ACItemStateENUM		State;
	PriceTypeENUM		Type;

	PG_AC_DCtoL_BuyItemResult( )
	{	Command = EM_PG_AC_DCtoL_BuyItemResult;	}
};
//---------------------------------------------------------------------------------------------------------
//Äv¼Ðª««~ ©Î ª½±µÁÊ¶R µ²ªG
struct PG_AC_LtoC_BuyItemResult
{
	GamePGCommandEnum	Command;
	bool				Result;
	int					ACItemDBID;		//­n¶Rªºª««~
	int					Prices;			//©Ò¥Xªº»ù¿ú(¦pªG µ¥©óª½±µÁÊ¶R»ù ¥Nªíª½±µÁÊ¶R)
	ACItemStateENUM		State;
	PriceTypeENUM		Type;

	PG_AC_LtoC_BuyItemResult( )
	{	Command = EM_PG_AC_LtoC_BuyItemResult;	}
};

//---------------------------------------------------------------------------------------------------------
//½æª««~
//Client -> Local
struct PG_AC_CtoL_SellItem
{
	GamePGCommandEnum	Command;
	int					BodyPos;
	ItemFieldStruct		Item;			//½æªºªF¦è	
	int					Prices;			//°ò¥»»ù
	int					BuyOutPrices;	//ª½±µÁÊ¶R»ù
	int					LiveTime;		//(¤ÀÄÁ­pºâ)
	PriceTypeENUM		Type;

	PG_AC_CtoL_SellItem( )
	{	Command = EM_PG_AC_CtoL_SellItem;	}
};
//---------------------------------------------------------------------------------------------------------
//½æ¿ú
//Client -> Local
struct PG_AC_CtoL_SellMoney
{
	GamePGCommandEnum	Command;
	PriceTypeENUM		SellType;
	int					SellMoney;

	int					Prices;			//°ò¥»»ù
	int					BuyOutPrices;	//ª½±µÁÊ¶R»ù
	int					LiveTime;		//(¤ÀÄÁ­pºâ)
	PriceTypeENUM		Type;

	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

	PG_AC_CtoL_SellMoney( )
	{	Command = EM_PG_AC_CtoL_SellMoney;	}
};
//---------------------------------------------------------------------------------------------------------
enum SellMoneyResultENUM
{
	EM_SellMoneyResult_OK			,
	EM_SellMoneyResult_Err			,	//¤@¯ë¿ùª« (¤£¥i¥æ©öª¬ºA)
	EM_SellMoneyResult_MoneyErr		,	//ª÷¿ú¤£¨¬
	EM_SellMoneyResult_MoneyTypeErr	,	//ª÷¿úÃþ«¬¦³°ÝÃD( ¥Î¹CÀ¸¹ô´«¹CÀ¸¹ô , ©Î¥Î±b¸¹¹ô´«±b¸¹¹ô )
	EM_SellMoneyResult_PasswordErr	,
};
//½æ¿ú µ²ªG
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
//½æª««~
//Local -> DC
struct PG_AC_LtoDC_SellItem
{
	GamePGCommandEnum	Command;
	ItemFieldStruct		Item;			//½æªºªF¦è	
	int					Prices;			//°ò¥»»ù
	int					BuyOutPrices;	//ª½±µÁÊ¶R»ù
	int					DBID;			//½Ö­n½æ
	int					LiveTime;		//¥Í¦s®É¶¡
	int					TaxMoney;		//µ|ª÷(¿ù»~®É¦^¶Ç­×¥¿¥Î)
	PriceTypeENUM		Type;

	PG_AC_LtoDC_SellItem( )
	{	Command = EM_PG_AC_LtoDC_SellItem;	}
};
//---------------------------------------------------------------------------------------------------------
enum ACSellItemResultENUM
{
	EM_ACSellItemResult_OK				,	//¦¨¥
	EM_ACSellItemResult_DataError		,	//¸ê®Æ¿ù»~
	EM_ACSellItemResult_MaxCountError	,	//¼Æ¶q¨ì¹F¤W­­
	EM_ACSellItemResult_DisableTrade	,	//¸T¤î¥æ©ö
	EM_ACSellItemResult_MoneyError		,	//ª÷¿ú¿ù»~
	EM_ACSellItemResult_EqFixError		,	//ª««~»Ý­n­×²z
};
//½æª««~µ²ªG
//DC -> Local
struct PG_AC_DCtoL_SellItemResult
{
	GamePGCommandEnum	Command;

	//bool				Result;			//¬O§_¦¨¥
	ACSellItemResultENUM Result;

	ACItemInfoStruct	Item;			//½æªºªF¦è	
	int					SellerDBID;
	int					TaxMoney;		//µ|ª÷(¿ù»~®É¦^¶Ç­×¥¿¥Î)

	PG_AC_DCtoL_SellItemResult( )
	{	Command = EM_PG_AC_DCtoL_SellItemResult;	}
};
//---------------------------------------------------------------------------------------------------------
//½æª««~µ²ªG
//Local -> Client
struct PG_AC_LtoC_SellItemResult
{
	GamePGCommandEnum	Command;

	ACItemInfoStruct	Item;			//½æªºªF¦è	
	//bool				Result;			//¬O§_¦¨¥
	ACSellItemResultENUM Result;

	PG_AC_LtoC_SellItemResult( )
	{	Command = EM_PG_AC_LtoC_SellItemResult;	}
};


//---------------------------------------------------------------------------------------------------------
//¨ú®ø©ç½æ
//Client -> Local
struct PG_AC_CtoL_CancelSellItem
{
	GamePGCommandEnum	Command;
	int					ACItemDBID;			//AC ª««~ªºDBID

	PG_AC_CtoL_CancelSellItem( )
	{	Command = EM_PG_AC_CtoL_CancelSellItem;	}
};
//---------------------------------------------------------------------------------------------------------
//¨ú®ø©ç½æ
//Local -> DC
struct PG_AC_LtoDC_CancelSellItem
{
	GamePGCommandEnum	Command;
	int					DBID;				//¾Ö¦³ªÌ ( ½æªÌ )
	int					ACItemDBID;			//AC ª««~ªºDBID

	PG_AC_LtoDC_CancelSellItem( )
	{	Command = EM_PG_AC_LtoDC_CancelSellItem;	}
};
//---------------------------------------------------------------------------------------------------------
//¦^À³¨ú®ø©ç½æµ²ªG
// Datacenter -> Client
struct PG_AC_DCtoC_CancelSellItemResult
{
	GamePGCommandEnum	Command;
	int					ACItemDBID;			//AC ª««~ªºDBID
	bool				Result;				

	PG_AC_DCtoC_CancelSellItemResult( )
	{	Command = EM_PG_AC_DCtoC_CancelSellItemResult;	}
};
//---------------------------------------------------------------------------------------------------------
//¶}±Ò©ç½æ©Ò
//DataCenter -> Client
struct PG_AC_DCtoC_OpenAC
{
	GamePGCommandEnum	Command;
	bool		Result;
	int			NpcGUID;				//³B²zªºNPC

	PG_AC_DCtoC_OpenAC( )
	{	Command = EM_PG_AC_DCtoC_OpenAC;	}
};
//---------------------------------------------------------------------------------------------------------
//¶}±Ò©ç½æ©Ò
//Local -> DataCenter
struct PG_AC_LtoDC_OpenAC
{
	GamePGCommandEnum	Command;
	int					DBID;			//­n¨Dªºª±®a
	int					NpcGUID;		//³B²zªºNPC

	PG_AC_LtoDC_OpenAC( )
	{	Command = EM_PG_AC_LtoDC_OpenAC;	}
};
//---------------------------------------------------------------------------------------------------------
//Ãö³¬©ç½æ©Ò
//Client -> Local
struct PG_AC_CtoL_CloseAC
{
	GamePGCommandEnum	Command;

	PG_AC_CtoL_CloseAC( )
	{	Command = EM_PG_AC_CtoL_CloseAC;	}
};
//---------------------------------------------------------------------------------------------------------
//Ãö³¬©ç½æ©Ò
//DataCenter -> Client
struct PG_AC_DCtoC_CloseAC
{
	GamePGCommandEnum	Command;

	PG_AC_DCtoC_CloseAC( )
	{	Command = EM_PG_AC_DCtoC_CloseAC;	}
};
//---------------------------------------------------------------------------------------------------------
//Ãö³¬©ç½æ©Ò
//Local -> DC
struct PG_AC_LtoDC_CloseAC
{
	GamePGCommandEnum	Command;
	int					DBID;

	PG_AC_LtoDC_CloseAC( )
	{	Command = EM_PG_AC_LtoDC_CloseAC;	}
};
//---------------------------------------------------------------------------------------------------------
//¨ú±o¬Yª«¥óªº¾ú¥v¸ê®Æ
//Client -> Local
struct PG_AC_CtoL_ItemHistoryRequest
{
	GamePGCommandEnum	Command;
	int		OrgObjID;
	int		Inherent;		//¤£¨Ï¥Î

	PG_AC_CtoL_ItemHistoryRequest( )
	{	Command = EM_PG_AC_CtoL_ItemHistoryRequest;	}
};
//---------------------------------------------------------------------------------------------------------
//¨ú±o¬Yª«¥óªº¾ú¥v¸ê®Æ
//Local -> DataCenter
struct PG_AC_LtoDC_ItemHistoryRequest
{
	GamePGCommandEnum	Command;
	int		DBID;			//ª±®aDBID
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
//AC¤w¦b¶}±Ò¤¤
//Local -> Client
struct PG_AC_LtoC_AcAlreadyOpen
{
	GamePGCommandEnum	Command;
	int		TargtetNPC;

	PG_AC_LtoC_AcAlreadyOpen( )
	{	Command = EM_PG_AC_LtoC_AcAlreadyOpen;	}
};
//---------------------------------------------------------------------------------------------------------
//AC ¥æ©ö§¹¦¨³qª¾¶R½æ¤è
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

