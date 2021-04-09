#pragma once
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged
//-----------------------------------------------------------------------

 
struct PG_DepartmentStore_CtoL_ShopFunctionRequest
{
	GamePGCommandEnum	Command;
	int					FunctionID;

	PG_DepartmentStore_CtoL_ShopFunctionRequest()
	{
		Command = EM_PG_DepartmentStore_CtoL_ShopFunctionRequest;
	}
};

//Local -> Client 回應租用結果
struct PG_DepartmentStore_LtoC_ShopFunctionResult
{
	GamePGCommandEnum	Command;
	int					FunctionID;
	bool				Result;

	PG_DepartmentStore_LtoC_ShopFunctionResult()
	{
		Command = EM_PG_DepartmentStore_LtoC_ShopFunctionResult;
	}
};
//-----------------------------------------------------------------------
//Client -> Local 要求商城販賣類型資訊
struct PG_DepartmentStore_CtoL_SellTypeRequest
{
	GamePGCommandEnum	Command;

	PG_DepartmentStore_CtoL_SellTypeRequest()
	{
		Command = EM_PG_DepartmentStore_CtoL_SellTypeRequest;
	}
};
//Local -> Datacenter 要求商城販賣類型資訊
struct PG_DepartmentStore_LtoD_SellTypeRequest
{
	GamePGCommandEnum	Command;
	int					CliSockID;
	int					CliProxyID;

	PG_DepartmentStore_LtoD_SellTypeRequest()
	{
		Command = EM_PG_DepartmentStore_LtoD_SellTypeRequest;
	}
};


//Datacenter -> Client 要求商城販賣類型資訊
struct PG_DepartmentStore_DtoC_SellType
{
	GamePGCommandEnum	Command;
	int					ItemCount;
	int					SellType[50];

	PG_DepartmentStore_DtoC_SellType()
	{
		Command = EM_PG_DepartmentStore_DtoC_SellType;
	}
	int Size()
	{
		return sizeof(int)*2+ItemCount*sizeof( int );
	}
};
//-----------------------------------------------------------------------
//Client -> Local 要求商城資訊
struct PG_DepartmentStore_CtoL_ShopInfoRequest
{
	GamePGCommandEnum	Command;
	int					SellType;

	PG_DepartmentStore_CtoL_ShopInfoRequest()
	{
		Command = EM_PG_DepartmentStore_CtoL_ShopInfoRequest;
	}
};
//Local -> Datacenter 要求商城資訊
struct PG_DepartmentStore_LtoD_ShopInfoRequest
{
	GamePGCommandEnum	Command;
	int					CliSockID;
	int					CliProxyID;
	int					SellType;

	PG_DepartmentStore_LtoD_ShopInfoRequest()
	{
		Command = EM_PG_DepartmentStore_LtoD_ShopInfoRequest;
	}
};
/*
struct DepartmentStoreBaseInfoStruct
{
	int					GUID;
	int					NeedMemberID;
	int					SellType;
	ItemFieldStruct		Item;
	struct  
	{
		int					BeginTime;
		int					EndTime;
		int					Count;
		int					MaxCount;
		int					Cost;
	}Sell;
	
};
*/
//Datacenter -> Client 商城資訊
struct PG_DepartmentStore_DtoC_ShopInfo
{
	GamePGCommandEnum	Command;
	int					ItemCount;
	DepartmentStoreBaseInfoStruct Item[1000];

	PG_DepartmentStore_DtoC_ShopInfo()
	{
		Command = EM_PG_DepartmentStore_DtoC_ShopInfo;
	}
	int Size()
	{
		return sizeof(int)*2+ItemCount*sizeof( DepartmentStoreBaseInfoStruct );
	}
};

struct PG_DepartmentStore_DtoC_ShopInfo_Zip
{
	GamePGCommandEnum	Command;
	int					ItemCount;
	int					DataSize;
	char				Data[0x40000];

	PG_DepartmentStore_DtoC_ShopInfo_Zip()
	{
		Command = EM_PG_DepartmentStore_DtoC_ShopInfo_Zip;
	}
	int Size()
	{
		return sizeof(int)*3+DataSize;
	}
};


//-----------------------------------------------------------------------
//Client -> Local 要求購買商城物品
struct PG_DepartmentStore_CtoL_BuyItem
{
	GamePGCommandEnum	Command;
	int					ItemGUID;		//物品索引號
	int					Count;			//購買數量
	int					Pos;			//放置位置
	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

	PG_DepartmentStore_CtoL_BuyItem()
	{
		Command = EM_PG_DepartmentStore_CtoL_BuyItem;
	}
};

//Local	-> DataCenter 要求購買商城物品
struct PG_DepartmentStore_LtoD_BuyItem
{
	GamePGCommandEnum	Command;
	int					ItemGUID;		//物品索引號
	int					PlayerDBID;		//玩家DBID
	int					PlayerAccountMoney;
	int					Count;			//購買數量
	int					Pos;			//放置位置

	PG_DepartmentStore_LtoD_BuyItem()
	{
		Command = EM_PG_DepartmentStore_LtoD_BuyItem;
	}
};

enum DepartmentStoreBuyItemResultENUM
{
	EM_DepartmentStoreBuyItemResult_OK	,			//成攻
	EM_DepartmentStoreBuyItemResult_SellOut	,		//賣完了
	EM_DepartmentStoreBuyItemResult_MoneyErr,		//金錢不足
	EM_DepartmentStoreBuyItemResult_PacketErr,		//包裹位置有問題
	EM_DepartmentStoreBuyItemResult_Err	,			//其它錯誤
	EM_DepartmentStoreBuyItemResult_PasswordErr	,	//密碼錯誤
	EM_DepartmentStoreBuyItemResult_TimeOut	,		//超過時間
	
};

//Datacenter -> Local 購買結果
struct PG_DepartmentStore_DtoL_BuyItemResult
{
	GamePGCommandEnum	Command;
	int										PlayerDBID;		//玩家DBID
	DepartmentStoreBuyItemResultENUM		Result;

	DepartmentStoreBaseInfoStruct	Item;
	int					Pos;			//放置位置
	int					Count;

	PG_DepartmentStore_DtoL_BuyItemResult()
	{
		Command = EM_PG_DepartmentStore_DtoL_BuyItemResult;
	}
};

//Local -> Client 購買結果
struct PG_DepartmentStore_LtoD_BuyItemResult
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;		//玩家DBID
	int					ItemGUID;
	bool				Result;
	int					Count;
	
	PG_DepartmentStore_LtoD_BuyItemResult()
	{
		Command = EM_PG_DepartmentStore_LtoD_BuyItemResult;
	}
};

//Local -> DataCenter 購買結果(計量處理)
struct PG_DepartmentStore_LtoC_BuyItemResult
{
	GamePGCommandEnum	Command;
	DepartmentStoreBuyItemResultENUM				Result;

	PG_DepartmentStore_LtoC_BuyItemResult()
	{
		Command = EM_PG_DepartmentStore_LtoC_BuyItemResult;
	}
};
//-----------------------------------------------------------------------
//Client -> Local 要求提領外部帳號幣
struct PG_DepartmentStore_CtoL_ImportAccountMoney
{
	GamePGCommandEnum	Command;

	PG_DepartmentStore_CtoL_ImportAccountMoney()
	{
		Command = EM_PG_DepartmentStore_CtoL_ImportAccountMoney;
	}
};
//-----------------------------------------------------------------------
//Local -> DataCenter 要求提領外部帳號幣
struct PG_DepartmentStore_LtoD_ImportAccountMoney
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	StaticString<_MAX_ACCOUNT_SIZE_>    Account_ID;

	PG_DepartmentStore_LtoD_ImportAccountMoney()
	{
		Command = EM_PG_DepartmentStore_LtoD_ImportAccountMoney;
	}
};
//-----------------------------------------------------------------------
//DataCenter -> Local 要求提領外部帳號幣
struct PG_DepartmentStore_DtoL_ImportAccountMoneyResult
{
	GamePGCommandEnum	Command;
	int									PlayerDBID;
	int									Money_Account;
	int									Money_Bonus;
	StaticString<_MAX_ACCOUNT_SIZE_>    Account_ID;
	int									LockDay;
	int									ImportGUID;

	PG_DepartmentStore_DtoL_ImportAccountMoneyResult()
	{
		Command = EM_PG_DepartmentStore_DtoL_ImportAccountMoneyResult;
	}
};
//-----------------------------------------------------------------------
//DataCenter -> Local 要求提領外部帳號幣
struct PG_DepartmentStore_LtoC_ImportAccountMoneyResult
{
	GamePGCommandEnum	Command;
	int									Money_Account;
	int									Money_Bonus;
	
	PG_DepartmentStore_LtoC_ImportAccountMoneyResult()
	{
		Command = EM_PG_DepartmentStore_LtoC_ImportAccountMoneyResult;
	}
};
//-----------------------------------------------------------------------
//Local ->DataCenter 確定帳號幣已提取
struct PG_DepartmentStore_LtoD_ImportAccountMoneyFailed
{
	GamePGCommandEnum	Command;
	int									PlayerDBID;
	StaticString<_MAX_ACCOUNT_SIZE_>    Account_ID;
	int									Money_Account;
	int									Money_Bonus;
	int									ImportGUID;

	PG_DepartmentStore_LtoD_ImportAccountMoneyFailed()
	{
		Command = EM_PG_DepartmentStore_LtoD_ImportAccountMoneyFailed;
	}
};
//-----------------------------------------------------------------------
//Client -> Local 要求兌換物品
struct PG_DepartmentStore_CtoL_ExchangeItemRequest
{
	GamePGCommandEnum	Command;
	char				Password[50];
	char				ItemSerial[50];
	

	PG_DepartmentStore_CtoL_ExchangeItemRequest()
	{
		Command = EM_PG_DepartmentStore_CtoL_ExchangeItemRequest;
	}
};
//-----------------------------------------------------------------------
//Local -> DataCenter 要求兌換物品
struct PG_DepartmentStore_LtoD_ExchangeItemRequest
{
	GamePGCommandEnum				 Command;
	StaticString<_MAX_ACCOUNT_SIZE_> Account;
	int								 PlayerDBID;
	char							 ItemSerial[50];
	char							 Password[50];
	int								 MapKey;

	PG_DepartmentStore_LtoD_ExchangeItemRequest()
	{
		Command = EM_PG_DepartmentStore_LtoD_ExchangeItemRequest;
	}
};
//-----------------------------------------------------------------------
enum ItemExchangeResultENUM
{
	EM_ItemExchangeResult_Depleted			= -8,	//兌換完畢
	EM_ItemExchangeResult_RoleLimited		= -7,	//角色兌換限制
	EM_ItemExchangeResult_AccountLimited	= -6,	//帳號兌換限制
	EM_ItemExchangeResult_DataError			= -5,	//兌換物品設定錯誤，請通知官方
	EM_ItemExchangeResult_ItemBufErr		= -4,	//物品欄位不足
	EM_ItemExchangeResult_WorldError		= -3,	//此世界不可兌換此物品
	EM_ItemExchangeResult_ItemUsed			= -2,	//物品已被使用
	EM_ItemExchangeResult_Failed			= -1,	//失敗，無此物件
	EM_ItemExchangeResult_OK				,
};

//DataCenter -> Local 要求兌換物品結果
struct PG_DepartmentStore_DtoL_ExchangeItemResult
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	ItemFieldStruct		Item;
	int					Money;
	int					MoneyAccount;

	int					ExchangeItemDBID;

	ItemExchangeResultENUM Result;

	int					LockDay;
	int					MapKey;

	PG_DepartmentStore_DtoL_ExchangeItemResult()
	{
		Command = EM_PG_DepartmentStore_DtoL_ExchangeItemResult;
	}
};
//-----------------------------------------------------------------------
//Local -> DataCenter 兌換物品確實送給玩家
struct PG_DepartmentStore_LtoD_ExchangeItemResultOK
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					ExchangeItemDBID;
	bool				Result;
	bool				IsDataError;

	PG_DepartmentStore_LtoD_ExchangeItemResultOK()
	{
		Command = EM_PG_DepartmentStore_LtoD_ExchangeItemResultOK;
	}
};
//-----------------------------------------------------------------------
//Local -> Client 通知Client 兌換物品結果
struct PG_DepartmentStore_LtoC_ExchangeItemResult
{
	GamePGCommandEnum	Command;
	ItemFieldStruct		Item;
	int					Money;
	int					MoneyAccount;

	ItemExchangeResultENUM Result;

	PG_DepartmentStore_LtoC_ExchangeItemResult()
	{
		Command = EM_PG_DepartmentStore_LtoC_ExchangeItemResult;
	}
};
//-----------------------------------------------------------------------
//Client -> Local 要求跑碼燈訊息
struct PG_DepartmentStore_CtoL_RunningMessageRequest
{
	GamePGCommandEnum	Command;
	PG_DepartmentStore_CtoL_RunningMessageRequest()
	{
		Command = EM_PG_DepartmentStore_CtoL_RunningMessageRequest;
	}
};
//-----------------------------------------------------------------------
//Local -> DataCenter 要求跑碼燈訊息
struct PG_DepartmentStore_LtoD_RunningMessageRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	PG_DepartmentStore_LtoD_RunningMessageRequest()
	{
		Command = EM_PG_DepartmentStore_LtoD_RunningMessageRequest;
	}
};
//-----------------------------------------------------------------------
//Local -> DataCenter 要求跑碼燈訊息
struct PG_DepartmentStore_DtoC_RunningMessage_Zip
{
	GamePGCommandEnum	Command;
	int		MsgCount;
	int		Size;
	char	Data[0x10000];
	
	PG_DepartmentStore_DtoC_RunningMessage_Zip()
	{
		Command = EM_PG_DepartmentStore_DtoC_RunningMessage_Zip;
	}
	int PGSize()
	{
		return sizeof(*this) - sizeof(Data) + Size;
	}
};
//-----------------------------------------------------------------------
//Client -> Local 要求送禮給其他人
struct PG_DepartmentStore_CtoL_MailGift
{
	GamePGCommandEnum	Command;

	int					ItemGUID;		//物品索引號
	int					Count;			//購買數量
	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

	//信件資訊
	MailBackGroundTypeENUM	BackGroundType;

	StaticString <_MAX_NAMERECORD_SIZE_>	TargetName;
	StaticString <_MAX_NAMERECORD_SIZE_>	Title;
	StaticString <2048>		Content;

	PG_DepartmentStore_CtoL_MailGift()
	{
		Command = EM_PG_DepartmentStore_CtoL_MailGift;
	}
};
//-----------------------------------------------------------------------
//Local -> DataCenter 檢查是否可以送給目標
struct PG_DepartmentStore_LtoD_CheckMailGift
{
	GamePGCommandEnum	Command;

	int					PlayerDBID;
	int					ItemGUID;		//物品索引號
	int					Count;			//購買數量
	StaticString <_MAX_NAMERECORD_SIZE_>	TargetName;
	int					MapKey;

	PG_DepartmentStore_LtoD_CheckMailGift()
	{
		Command = EM_PG_DepartmentStore_LtoD_CheckMailGift;
	}
};
//-----------------------------------------------------------------------
enum CheckMailGiftResultTypeENUM
{
	EM_CheckMailGiftResultType_OK				,
	EM_CheckMailGiftResultType_TargetNotFind	,
	EM_CheckMailGiftResultType_ItemError		,
	EM_CheckMailGiftResultType_MoneyError		,
	EM_CheckMailGiftResultType_PasswordError	,
	EM_CheckMailGiftResultType_DataError		,
	EM_CheckMailGiftResultType_TimeOut			,
	EM_CheckMailGiftResultType_SellOut			,
	EM_CheckMailGiftResultType_NotSendToSelf	,		
	EM_CheckMailGiftResultType_DisableSendGift	,		//送理攻能沒有開啟
};
//DataCenter -> Local 檢查是否可以送給目標
struct PG_DepartmentStore_DtoL_CheckMailGiftResult
{
	GamePGCommandEnum			Command;
	int							MapKey;
	int							TargetDBID;
	DepartmentStoreBaseInfoStruct Item;
	CheckMailGiftResultTypeENUM	Result;

	PG_DepartmentStore_DtoL_CheckMailGiftResult()
	{
		Command = EM_PG_DepartmentStore_DtoL_CheckMailGiftResult;
	}
};
//-----------------------------------------------------------------------
/*
//Local -> DataCenter 要求送禮給其他人
struct PG_DepartmentStore_LtoD_MailGift
{
	GamePGCommandEnum	Command;

	int					ItemGUID;		//物品索引號
	int					Count;			//購買數量
	int					Pos;			//放置位置

	PG_DepartmentStore_LtoD_MailGift()
	{
		Command = EM_PG_DepartmentStore_LtoD_MailGift;
	}
};
*/
//-----------------------------------------------------------------------
//Local -> Client 要求送禮給其他人結果
struct PG_DepartmentStore_LtoC_MailGiftResult
{
	GamePGCommandEnum	Command;
	CheckMailGiftResultTypeENUM Result;

	PG_DepartmentStore_LtoC_MailGiftResult()
	{
		Command = EM_PG_DepartmentStore_LtoC_MailGiftResult;
	}
};

//-----------------------------------------------------------------------
//Client -> Local 要求取出網頁商城的物品
struct PG_DepartmentStore_CtoL_WebBagRequest
{
	GamePGCommandEnum	Command;

	PG_DepartmentStore_CtoL_WebBagRequest()
	{
		Command = EM_PG_DepartmentStore_CtoL_WebBagRequest;
	}
};
//-----------------------------------------------------------------------
//Local -> DataCenter 要求取出網頁商城的物品
struct PG_DepartmentStore_LtoD_WebBagRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	StaticString<_MAX_ACCOUNT_SIZE_> Account;
	PG_DepartmentStore_LtoD_WebBagRequest()
	{
		Command = EM_PG_DepartmentStore_LtoD_WebBagRequest;
	}
};
//-----------------------------------------------------------------------
//Local -> Client 取得網頁商城物品通知
struct PG_DepartmentStore_LtoC_WebBagResult
{
	GamePGCommandEnum	Command;
	int					OrgObjID;
	int					Count;

	PG_DepartmentStore_LtoC_WebBagResult()
	{
		Command = EM_PG_DepartmentStore_LtoC_WebBagResult;
	}
};
//-----------------------------------------------------------------------
//DataCenter -> Local 商城物品List
struct PG_DepartmentStore_DtoL_WebBagResult
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					ItemGUID;
	ItemFieldStruct		Item;
	bool				IsEnd;
	char				Message[256];
	int					PriceType;
	int					Price;
	int					AddBonusMoney;

	PG_DepartmentStore_DtoL_WebBagResult()
	{
		Command = EM_PG_DepartmentStore_DtoL_WebBagResult;
	}
};
/*
//-----------------------------------------------------------------------
//Local -> Battle Local
//通知購買商城結果
struct PG_CliConnect_LtoBL_BuyItemResult
{
	GamePGCommandEnum	Command;
	int										PlayerDBID;		//玩家DBID
	DepartmentStoreBuyItemResultENUM		Result;

	DepartmentStoreBaseInfoStruct	Item;
	int					Pos;			//放置位置
	int					Count;

	PG_CliConnect_LtoBL_BuyItemResult()
	{
		Command = EM_PG_CliConnect_LtoBL_BuyItemResult;
	}
};
//-----------------------------------------------------------------------
//Battle Local -> Local
//購買結果
struct PG_DepartmentStore_BLtoL_BuyItemResult
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;		//玩家DBID
	int					ItemGUID;
	bool				Result;
	int					Count;

	PG_DepartmentStore_BLtoL_BuyItemResult()
	{
		Command = EM_PG_DepartmentStore_BLtoL_BuyItemResult;
	}
};
*/
//////////////////////////////////////////////////////////////////////////
//角子老虎
//////////////////////////////////////////////////////////////////////////

//轉蛋機獎項設定
struct SlotMachineBase
{
	int Type;	//類型
	int	Pos;	//位置
	int	Rate;	//機率
};
struct SlotMachineTypeBase
{
	int Type;		//類型
	int	Rate;		//機率
	int	ItemID;		//物品ID
	int	ItemCount;	//物品數量
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local 老虎機資料
struct PG_DepartmentStore_DtoL_SlotMachineInfo
{
	GamePGCommandEnum	Command;
	SlotMachineBase		Base[_MAX_SLOT_MACHINE_PRIZES_COUNT_];
	int					TypeInfoCount;
	SlotMachineTypeBase	TypeInfo[1000];

	PG_DepartmentStore_DtoL_SlotMachineInfo()
	{
		Command = EM_PG_DepartmentStore_DtoL_SlotMachineInfo;
	}
	int PGSize()
	{
		return sizeof(*this) - sizeof(SlotMachineTypeBase) * ( 1000 - TypeInfoCount );
	}
};
//Local -> Client 通知所有人 老虎機資料有改變
struct PG_DepartmentStore_LtoC_SlotMachineInfoChange
{
	GamePGCommandEnum	Command;

	PG_DepartmentStore_LtoC_SlotMachineInfoChange()
	{
		Command = EM_PG_DepartmentStore_LtoC_SlotMachineInfoChange;
	}
};
//Client -> Local 要求老虎機資料
struct PG_DepartmentStore_CtoL_SlotMachineInfoRequest
{
	GamePGCommandEnum	Command;

	PG_DepartmentStore_CtoL_SlotMachineInfoRequest()
	{
		Command = EM_PG_DepartmentStore_CtoL_SlotMachineInfoRequest;
	}
};
//Local -> Client 通知老虎機資料
struct PG_DepartmentStore_LtoC_SlotMachineInfo
{
	GamePGCommandEnum	Command;
	int		ItemID[_MAX_SLOT_MACHINE_PRIZES_COUNT_];
	int		ItemCount[_MAX_SLOT_MACHINE_PRIZES_COUNT_];

	PG_DepartmentStore_LtoC_SlotMachineInfo()
	{
		Command = EM_PG_DepartmentStore_LtoC_SlotMachineInfo;
	}
};
//Client -> Local 投幣
struct PG_DepartmentStore_CtoL_SlotMachinePlay
{
	GamePGCommandEnum	Command;

	PG_DepartmentStore_CtoL_SlotMachinePlay()
	{
		Command = EM_PG_DepartmentStore_CtoL_SlotMachinePlay;
	}
};
//Local -> Client 結果
enum SlotMachinePlayResultENUM
{
	EM_SlotMachinePlayResult_OK			,
	EM_SlotMachinePlayResult_Failed		,
	EM_SlotMachinePlayResult_NoMoney	,

};
struct PG_DepartmentStore_LtoC_SlotMachinePlayResult
{
	GamePGCommandEnum	Command;
	SlotMachinePlayResultENUM Result;
//	ItemFieldStruct		Item;
	int					ItemID;
	int					ItemCount;

	PG_DepartmentStore_LtoC_SlotMachinePlayResult()
	{
		Command = EM_PG_DepartmentStore_LtoC_SlotMachinePlayResult;
	}
};

//Client -> Local
struct PG_DepartmentStore_CtoL_GetSlotMachineItem
{
	GamePGCommandEnum	Command;

	PG_DepartmentStore_CtoL_GetSlotMachineItem()
	{
		Command = EM_PG_DepartmentStore_CtoL_GetSlotMachineItem;
	}
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#pragma managed