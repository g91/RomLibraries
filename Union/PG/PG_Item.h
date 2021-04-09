#pragma once

#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"

//--------------------------------------------------------------------------------------
//通知Server取地上某一個物品並且把此物品放置於欄位何處
// Client -> Local
struct PG_Item_CtoL_GetGroundItem
{
    GamePGCommandEnum	Command;
    int					ItemID;				//辨識用
//    short				Type;				//(0 Body )
    short				Pos;				//物品放置的位置

    PG_Item_CtoL_GetGroundItem()
    {
        Command = EM_PG_Item_CtoL_GetGroundItem;
    }
};
//--------------------------------------------------------------------------------------
//通知client 端某物品放置於某欄位是否成

enum ItemResult
{
	EM_Item_DelItemInBufferOK		= -2 ,
	EM_Item_GetItemInBufferOK	= -1   ,
    EM_Item_ItemOK				=	0,
    EM_Item_GetGroundItemFailed	     ,
    EM_Item_DeleteItemFailed         ,
    EM_Item_GetItemInBufferFailed   ,
    EM_Item_ExchangeFieldFailed     ,
    EM_Item_ExchangeMoneyFailed     ,


    EM_Item_FORCE_DWORD   = 0x7fffffff ,
};

// Local -> Client
struct PG_Item_LtoC_GetItemResult
{
    GamePGCommandEnum	Command;
    ItemResult			Result;				//辨識用
	int					Count;				//堆入量(只有buffer堆入有效)
	int					ItemID;				//物件號碼

    PG_Item_LtoC_GetItemResult()
    {
        Command = EM_PG_Item_LtoC_GetItemResult;
    }
};
//--------------------------------------------------------------------------------------
//通知Client 清除所有的ItemBuffer資料
// Local -> Client
struct PG_Item_LtoC_ClsItemInBuffer
{
    GamePGCommandEnum	Command;

    PG_Item_LtoC_ClsItemInBuffer()
    {
        Command = EM_PG_Item_LtoC_ClsItemInBuffer;
    }
};
//--------------------------------------------------------------------------------------
//通知Client 有新的物品可以取
// Local -> Client
struct PG_Item_LtoC_NewItemInBuffer
{
    GamePGCommandEnum	Command;
    ItemFieldStruct		Item;

    PG_Item_LtoC_NewItemInBuffer()
    {
        Command = EM_PG_Item_LtoC_NewItemInBuffer;
    }
};
//--------------------------------------------------------------------------------------
//要求取Buf內的資料
// Client -> Local
struct PG_Item_CtoL_GetBufferItem
{
    GamePGCommandEnum	Command;
    ItemFieldStruct		Item;
 //   short				Type;				//(0 Body)
    short				Pos;				//物品放置的位置
    short               Count;

    PG_Item_CtoL_GetBufferItem()
    {
        Command = EM_PG_Item_CtoL_GetBufferItem;
    }
};
//--------------------------------------------------------------------------------------
//通知刪除某欄位物品
//如果為金錢則位置忽略
// Client -> Local
struct PG_Item_CtoL_DeleteItem
{
    GamePGCommandEnum	Command;
    ItemFieldStruct		Item;
    short				Type;				//(0 Body 1 Bank 2 BodyMoney 3 BankMoney)
    short				Pos;				//物品放置的位置


    PG_Item_CtoL_DeleteItem()
    {
        Command = EM_PG_Item_CtoL_DeleteItem;
    }
};
//--------------------------------------------------------------------------------------
//通知刪除某欄位物品
//如果為金錢則位置忽略
// Local -> Client
struct PG_Item_LtoC_DeleteItem
{
    GamePGCommandEnum	Command;
    ItemFieldStruct		Item;
    short				Type;				//(0 Body 1 Bank 2 BodyMoney 3 BankMoney)
//    short				Pos;				//物品放置的位置
    ItemResult			Result;

    PG_Item_LtoC_DeleteItem()
    {
        Command = EM_PG_Item_LtoC_DeleteItem;
    }
};
//--------------------------------------------------------------------------------------
//要求兩物欄位品交換
// Client -> Local
struct	PG_Item_CtoL_ExchangeField
{
    GamePGCommandEnum	Command;
    ItemFieldStruct		Item[2];
    short				Type[2];			//(0 Body 1 Bank )
    short				Pos[2];				//物品放置的位置
	int					ClientID;			//Client 處理用ID

    PG_Item_CtoL_ExchangeField()
    {
        Command = EM_PG_Item_CtoL_ExchangeField;
    }
};
//--------------------------------------------------------------------------------------
//回應兩物品欄位交換結果
// Local -> Client
struct	PG_Item_LtoC_ExchangeField
{
    GamePGCommandEnum	Command;
    short				Type[2];			//(0 Body 1 Bank)
    ItemResult			Result;
	int					ClientID;			//Client 處理用ID

    PG_Item_LtoC_ExchangeField()
    {
        Command = EM_PG_Item_LtoC_ExchangeField;
    }
};
//--------------------------------------------------------------------------------------
//要求兩物品欄位交換結果
// Client -> Local
struct	PG_Item_CtoL_RequestBodyBankMoneyExchange
{
    GamePGCommandEnum	Command;

    int					Count;		//身體加 銀行減


    PG_Item_CtoL_RequestBodyBankMoneyExchange()
    {
        Command = EM_PG_Item_CtoL_RequestBodyBankMoneyExchange;
    }
};

//--------------------------------------------------------------------------------------
//回應身體與銀行金錢交換
// Local -> Client
struct	PG_Item_LtoC_BodyBankMoneyExchange
{
    GamePGCommandEnum	Command;
    ItemResult			Result;

    PG_Item_LtoC_BodyBankMoneyExchange()
    {
        Command = EM_PG_Item_LtoC_BodyBankMoneyExchange;
    }
};
//--------------------------------------------------------------------------------------
//要求重送Buffer內的資料
// Client -> Local
struct	PG_Item_CtoL_RequestBufferInfo
{
    GamePGCommandEnum	Command;

    PG_Item_CtoL_RequestBufferInfo()
    {
        Command = EM_PG_Item_CtoL_RequestBufferInfo;
    }
};
//--------------------------------------------------------------------------------------
//要求把身體某欄位分成兩半
// Client -> Server
struct	PG_Item_CtoL_ApartBodyItemRequest
{
    GamePGCommandEnum	Command;
    ItemFieldStruct		Item;
    short				PosSrc;
    short				CountDes; 
    short				PosDes;

    PG_Item_CtoL_ApartBodyItemRequest()
    {
        Command = EM_PG_Item_CtoL_ApartBodyItemRequest;
    }
};
//--------------------------------------------------------------------------------------
//要求把身體某欄位分成兩半 的結果
struct	PG_Item_LtoC_ApartBodyItemResult
{
	GamePGCommandEnum	Command;
	bool				Result;
	short				PosSrc;
	short				PosDes;

	PG_Item_LtoC_ApartBodyItemResult()
	{
		Command = EM_PG_Item_LtoC_ApartBodyItemResult;
	}
};
//--------------------------------------------------------------------------------------
//修正某一位置的資料
// Local -> Client
struct	PG_Item_LtoC_FixItemInfo
{
    GamePGCommandEnum	Command;
    ItemFieldStruct		Item;
    short				Type;				//(0 Body 1 Bank 2 身體 )
    short				Pos;				//物品放置的位置

    PG_Item_LtoC_FixItemInfo()
    {
        Command = EM_PG_Item_LtoC_FixItemInfo;
    }
};
//--------------------------------------------------------------------------------------
//要求修正所有物品更新
//Client -> Server
struct	PG_Item_CtoL_RequestFixAllItemInfo
{
    GamePGCommandEnum	Command;

    PG_Item_CtoL_RequestFixAllItemInfo()
    {
        Command = EM_PG_Item_CtoL_RequestFixAllItemInfo;
    }
};
//--------------------------------------------------------------------------------------
//修正所有物品更新
//Local -> Client
struct	PG_Item_LtoC_FixAllItemInfo
{
    GamePGCommandEnum	Command;

    BodyItemStruct		Body;
    BankItemStruct		Bank;
    EQStruct			EQ;

    PG_Item_LtoC_FixAllItemInfo()
    {
        Command = EM_PG_Item_LtoC_FixAllItemInfo;
    }
};
//--------------------------------------------------------------------------------------
//修正身體 銀行金錢
//Local -> Client
struct  PG_Item_LtoC_FixMoney
{
    GamePGCommandEnum	Command;

    int					BodyMoney;
    int					BankMoney;
	int					BodyMoney_Account;
	int					Money_Bonus;

    PG_Item_LtoC_FixMoney()
    {
        Command = EM_PG_Item_LtoC_FixMoney;
    }
};
//--------------------------------------------------------------------------------------

//通知目前包裹與銀行欄位的上線量
//Local -> Client
struct  PG_Item_LtoC_MaxItemCount
{
    GamePGCommandEnum	Command;

    int					MaxBody;		//身體欄位上限
    int					MaxBank;		//銀行欄位上限

    PG_Item_LtoC_MaxItemCount()
    {
        Command = EM_PG_Item_LtoC_MaxItemCount;
    }
};
//--------------------------------------------------------------------------------------
//通知開啟銀行
//Local  -> Client
struct  PG_Item_LtoC_OpenBank
{
    GamePGCommandEnum	Command;
	int					GItemID;	//銀行的物件id

    PG_Item_LtoC_OpenBank()
    {
        Command = EM_PG_Item_LtoC_OpenBank;
    }
};
//--------------------------------------------------------------------------------------
//通知關閉銀行
//Client -> Local
struct  PG_Item_CtoL_CloseBank
{
    GamePGCommandEnum	Command;

    PG_Item_CtoL_CloseBank()
    {
        Command = EM_PG_Item_CtoL_CloseBank;
    }
};
//--------------------------------------------------------------------------------------
//通知關閉銀行
//Client -> Local
struct  PG_Item_LtoC_CloseBank
{
	GamePGCommandEnum	Command;

	PG_Item_LtoC_CloseBank()
	{
		Command = EM_PG_Item_LtoC_CloseBank;
	}
};
//--------------------------------------------------------------------------------------
//要求使用物品
//Client -> Local
struct PG_Item_CtoL_UseItem
{
    GamePGCommandEnum	Command;
    int 				Pos;				//物品放置的位置
    int 				TargetID;
    ItemFieldStruct		Item;
    short				Type;				//(0 Body 1 Bank 2 EQ)
	float				TargetX , TargetY , TargetZ;

    PG_Item_CtoL_UseItem()
    {
        Command = EM_PG_Item_CtoL_UseItem;
    }
};
//--------------------------------------------------------------------------------------
//物品使用結果
//Local -> Client
struct PG_Item_LtoC_UseItemResult
{
    GamePGCommandEnum	Command;

    bool		Result;		// false 失敗 true 成?
    short		Pos;		//物品位置-
    short		Type;		//(0 Body 1 Bank 2 EQ)

    PG_Item_LtoC_UseItemResult()
    {
        Command = EM_PG_Item_LtoC_UseItemResult;
    }
};
//--------------------------------------------------------------------------------------
//修正重要物品的旗標
//Local -> Client
struct PG_Item_LtoC_SetKeyItemFlag
{
	GamePGCommandEnum	Command;
	
	int		KeyItemID;
	bool	Value;

	PG_Item_LtoC_SetKeyItemFlag()
	{
		Command = EM_PG_Item_LtoC_SetKeyItemFlag;
	}
};
//--------------------------------------------------------------------------------------
//修正頭銜的旗標
//Local -> Client
struct PG_Item_LtoC_SetTitleFlag
{
	GamePGCommandEnum	Command;

	int		TitleObjID;
	bool	Value;

	PG_Item_LtoC_SetTitleFlag()
	{
		Command = EM_PG_Item_LtoC_SetTitleFlag;
	}
};
//--------------------------------------------------------------------------------------
//修正卡片的旗標
//Local -> Client
struct PG_Item_LtoC_SetCardFlag
{
	GamePGCommandEnum	Command;

	int		CardObjID;
	bool	Value;

	PG_Item_LtoC_SetCardFlag()
	{
		Command = EM_PG_Item_LtoC_SetCardFlag;
	}
};
//--------------------------------------------------------------------------------------
//把某一張卡片取出來
//Client -> Local
struct PG_Item_CtoL_TakeOutCardRequest
{
	GamePGCommandEnum	Command;

	int		CardObjID;

	PG_Item_CtoL_TakeOutCardRequest()
	{
		Command = EM_PG_Item_CtoL_TakeOutCardRequest;
	}
};
//--------------------------------------------------------------------------------------
//把某一張卡片取出來
//Client -> Local
struct PG_Item_LtoC_TakeOutCardResult
{
	GamePGCommandEnum	Command;

	int		CardObjID;
	bool	Result;

	PG_Item_LtoC_TakeOutCardResult()
	{
		Command = EM_PG_Item_LtoC_TakeOutCardResult;
	}
};
//--------------------------------------------------------------------------------------
//對物品使用物品 要求
//Client -> Local
struct PG_Item_CtoL_UseItemToItem_Notify
{
	GamePGCommandEnum	Command;

	int					TargetPosType;		// 0 身體 1 銀行 2 裝備

	int					UseItemPos;
	ItemFieldStruct		UseItem;

	int					TargetItemPos;
	ItemFieldStruct		TargetItem;

	StaticString< 33 >  Password;

	PG_Item_CtoL_UseItemToItem_Notify()
	{
		Command = EM_PG_Item_CtoL_UseItemToItem_Notify;
	}
};


//對物品使用物品 結果
//Local -> Client
struct PG_Item_LtoC_UseItemToItem_Result
{
	GamePGCommandEnum	Command;

	bool		Result;

	PG_Item_LtoC_UseItemToItem_Result()
	{
		Command = EM_PG_Item_LtoC_UseItemToItem_Result;
	}
};

//--------------------------------------------------------------------------------------
//通知週圍人  某人取得效果光球
//Local -> Client
struct PG_Item_LtoC_UsePowerLight
{
	GamePGCommandEnum	Command;

	int		PowerLightGUID;
	int		PlayerGUID;

	PG_Item_LtoC_UsePowerLight()
	{
		Command = EM_PG_Item_LtoC_UsePowerLight;
	}
};
//--------------------------------------------------------------------------------------
enum EqRefineResultENUM
{
	EM_EqRefineResult_None		,		//沒效果
	EM_EqRefineResult_LevelUp	,		//等級上升
	EM_EqRefineResult_LevelDown	,		//等級下降
	EM_EqRefineResult_Broken	,		//物品壞掉
	EM_EqRefineResult_Protect	,		//裝備保護
};
//裝備衝等結果
//Local ->Client
struct PG_Item_LtoC_EqRefineResult
{
	GamePGCommandEnum	Command;

	ItemFieldStruct		Item;
	EqRefineResultENUM	Result;

	PG_Item_LtoC_EqRefineResult()
	{
		Command = EM_PG_Item_LtoC_EqRefineResult;
	}
};

//--------------------------------------------------------------------------------------
//要求分解物品
//Client -> Local
struct PG_Item_CtoL_ItemDissolutionRequest
{
	GamePGCommandEnum	Command;

	ItemFieldStruct		Item;
	int					Pos;				//物品放置的位置

	PG_Item_CtoL_ItemDissolutionRequest()
	{
		Command = EM_PG_Item_CtoL_ItemDissolutionRequest;
	}
};
//--------------------------------------------------------------------------------------
enum ItemDissolutionResultENUM
{
	EM_ItemDissolutionResult_OK			,
	EM_ItemDissolutionResult_OK_Empty	,	//沒猜猜猜拆解出東西
	EM_ItemDissolutionResult_Failed	,
};
//要求分解物品結果
//Local -> Client
struct PG_Item_LtoC_ItemDissolutionResult
{
	GamePGCommandEnum	Command;

	ItemFieldStruct		Item;
	ItemDissolutionResultENUM	Result;

	PG_Item_LtoC_ItemDissolutionResult()
	{
		Command = EM_PG_Item_LtoC_ItemDissolutionResult;
	}
};
//--------------------------------------------------------------------------------------
//通知打洞結果
//Local -> Client
struct PG_Item_LtoC_DigHoleResult
{
	GamePGCommandEnum	Command;
	bool	Result;

	PG_Item_LtoC_DigHoleResult()
	{
		Command = EM_PG_Item_LtoC_DigHoleResult;
	}
};
//--------------------------------------------------------------------------------------
//要求打開合成
//Local -> Client
struct PG_Item_LtoC_EQCombinOpen
{
	GamePGCommandEnum	Command;
	int					TargetNPC;
	int					ItemPos;
	int					Type;		//0有限制類型  1不限制類型

	PG_Item_LtoC_EQCombinOpen()
	{
		Command = EM_PG_Item_LtoC_EQCombinOpen;
	}
};
//--------------------------------------------------------------------------------------
//通知關閉
//Local -> Client
struct PG_Item_LtoC_EQCombinClose
{
	GamePGCommandEnum	Command;

	PG_Item_LtoC_EQCombinClose()
	{
		Command = EM_PG_Item_LtoC_EQCombinClose;
	}
};
//--------------------------------------------------------------------------------------
//通知關閉
// Client -> Local
struct PG_Item_CtoL_EQCombinClose
{
	GamePGCommandEnum	Command;

	PG_Item_CtoL_EQCombinClose()
	{
		Command = EM_PG_Item_CtoL_EQCombinClose;
	}
};
//--------------------------------------------------------------------------------------
//要求合成
//Client -> Local
struct PG_Item_CtoL_EQCombinRequest
{
	GamePGCommandEnum	Command;
	int					CombinItemPos[ 2 ];	//物品位置
	int					CheckItemPos;		//檢查合成物品

	PG_Item_CtoL_EQCombinRequest()
	{
		Command = EM_PG_Item_CtoL_EQCombinRequest;
	}
};
//--------------------------------------------------------------------------------------
//要求合成結果
//Local -> Client
struct PG_Item_LtoC_EQCombinResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Item_LtoC_EQCombinResult()
	{
		Command = EM_PG_Item_LtoC_EQCombinResult;
	}
};
//--------------------------------------------------------------------------------------
// accountbag 開啟
//--------------------------------------------------------------------------------------
// Local -> DataCenter 
// 要求AccountBag的資料
struct PG_Item_LtoD_AccountBagInfoRequest
{
	GamePGCommandEnum	Command;

	int					PlayerDBID;
	StaticString<_MAX_ACCOUNT_SIZE_>     Account;  //建立此物件的帳號

	PG_Item_LtoD_AccountBagInfoRequest()
	{
		Command = EM_PG_Item_LtoD_AccountBagInfoRequest;
	}
};
//--------------------------------------------------------------------------------------
// DataCenter -> Local
//AccountBag的資料
struct PG_Item_DtoL_AccountBagInfoResult
{
	GamePGCommandEnum	Command;

	int					PlayerDBID;
	int					MaxCount;
	int					ItemDBID[20];
	ItemFieldStruct		Item[20];


	PG_Item_DtoL_AccountBagInfoResult()
	{
		Command = EM_PG_Item_DtoL_AccountBagInfoResult;
	}
};
//--------------------------------------------------------------------------------------
// Local -> Client
// 開啟Account Bag
struct PG_Item_LtoC_OpenAccountBag
{
	GamePGCommandEnum	Command;

	int					TargetNPC;
	int					MaxCount;

	ItemFieldStruct		Item[20];
	int					ItemDBID[20];


	PG_Item_LtoC_OpenAccountBag()
	{
		Command = EM_PG_Item_LtoC_OpenAccountBag;
	}
};
//--------------------------------------------------------------------------------------
// Client -> Local
// 要求取Account Bag內的東西
struct PG_Item_CtoL_AccountBagGetItemRequest
{
	GamePGCommandEnum	Command;

	int		BodyPos;
	int		ItemDBID;

	PG_Item_CtoL_AccountBagGetItemRequest()
	{
		Command = EM_PG_Item_CtoL_AccountBagGetItemRequest;
	}
};
//--------------------------------------------------------------------------------------
enum	AccountBagGetItemResultENUM
{
	EM_AccountBagGetItemResult_OK			,
	EM_AccountBagGetItemResult_ObjDBErr		,
	EM_AccountBagGetItemResult_BodyPosErr	,
	EM_AccountBagGetItemResult_CountErr		,
	EM_AccountBagGetItemResult_ItemDBIDErr	,
};
// Local -> Client
// 要求取Account Bag內的東西
struct PG_Item_LtoC_AccountBagGetItemResult
{
	GamePGCommandEnum	Command;

	int		BodyPos;
	int		ItemDBID;
	AccountBagGetItemResultENUM	Result;

	PG_Item_LtoC_AccountBagGetItemResult()
	{
		Command = EM_PG_Item_LtoC_AccountBagGetItemResult;
	}
};
//--------------------------------------------------------------------------------------
//Client -> Local
//Close Account Bag
struct PG_Item_CtoL_AccountBagClose
{
	GamePGCommandEnum	Command;

	PG_Item_CtoL_AccountBagClose()
	{
		Command = EM_PG_Item_CtoL_AccountBagClose;
	}
};
//--------------------------------------------------------------------------------------
//Local -> Client 
//生存時間到 物品消失通知
struct PG_Item_LtoC_ItemTimeout
{
	GamePGCommandEnum	Command;

	ItemTimeoutPosENUM	Pos;
	ItemFieldStruct		Item;

	PG_Item_LtoC_ItemTimeout()
	{
		Command = EM_PG_Item_LtoC_ItemTimeout;
	}
};
//--------------------------------------------------------------------------------------
/*
//Client -> Local
//裝備容解 ==> 魔石
struct PG_Item_CtoL_SmeltEqtoMagicStoneRequest
{
	GamePGCommandEnum	Command;

	int		EQPos;
	int		SmeltItemPos;
	int		BoxPos;
	
	PG_Item_CtoL_SmeltEqtoMagicStoneRequest()
	{
		Command = EM_PG_Item_CtoL_SmeltEqtoMagicStoneRequest;
	}
};
//--------------------------------------------------------------------------------------
//Local -> Client
//裝備容解 ==> 魔石 結果
struct PG_Item_LtoC_SmeltEqtoMagicStoneResult
{
	GamePGCommandEnum	Command;

	int		EQPos;
	int		SmeltItemPos;
	int		BoxPos;
	bool	Result;

	PG_Item_LtoC_SmeltEqtoMagicStoneResult()
	{
		Command = EM_PG_Item_LtoC_SmeltEqtoMagicStoneResult;
	}
};
//--------------------------------------------------------------------------------------
//Client -> Local
//精煉 魔石(升等)
struct PG_Item_CtoL_RefineMagicStoneRequest
{
	GamePGCommandEnum	Command;

	int		MagicStonePos[3];
	int		BoxPos;

	PG_Item_CtoL_RefineMagicStoneRequest()
	{
		Command = EM_PG_Item_CtoL_RefineMagicStoneRequest;
	}
};
//--------------------------------------------------------------------------------------
//Local -> Client
//精煉 魔石(升等) 結果
struct PG_Item_LtoC_RefineMagicStoneResult
{
	GamePGCommandEnum	Command;

	int		MagicStonePos[3];		//-1 代表沒有
	int		BoxPos;
	bool	Result;

	PG_Item_LtoC_RefineMagicStoneResult()
	{
		Command = EM_PG_Item_LtoC_RefineMagicStoneResult;
	}
};
//--------------------------------------------------------------------------------------
//Client -> Local
//裝備 與魔石熔合
struct PG_Item_CtoL_CombinEQandMagicStoneRequest
{
	GamePGCommandEnum	Command;

	int		MagicStonePos[3];
	int		EQPos;
	int		BoxPos;

	PG_Item_CtoL_CombinEQandMagicStoneRequest()
	{
		Command = EM_PG_Item_CtoL_CombinEQandMagicStoneRequest;
	}
};

//--------------------------------------------------------------------------------------
//Local -> Client
//裝備 與魔石熔合 結果
struct PG_Item_LtoC_CombinEQandMagicStoneResult
{
	GamePGCommandEnum	Command;

	int		MagicStonePos[3];
	int		EQPos;
	int		BoxPos;
	bool	Result;

	PG_Item_LtoC_CombinEQandMagicStoneResult()
	{
		Command = EM_PG_Item_LtoC_CombinEQandMagicStoneResult;
	}
};
//--------------------------------------------------------------------------------------
//Client -> Local
//符文轉換
struct PG_Item_CtoL_RuneTransferRequest
{
	GamePGCommandEnum	Command;

	int		RunePos[2];
	int		BoxPos;

	PG_Item_CtoL_RuneTransferRequest()
	{
		Command = EM_PG_Item_CtoL_RuneTransferRequest;
	}
};
//--------------------------------------------------------------------------------------
//Local -> Client
//符文轉換 結果
struct PG_Item_LtoC_RuneTransferResult
{
	GamePGCommandEnum	Command;

	int		RunePos[2];
	int		BoxPos;
	bool	Result;

	PG_Item_LtoC_RuneTransferResult()
	{
		Command = EM_PG_Item_LtoC_RuneTransferResult;
	}
};
//--------------------------------------------------------------------------------------
//Client -> Local
//符文升等
struct PG_Item_CtoL_RefineRuneRequest
{
	GamePGCommandEnum	Command;

	int		RunePos[5];
	int		BoxPos;

	PG_Item_CtoL_RefineRuneRequest()
	{
		Command = EM_PG_Item_CtoL_RefineRuneRequest;
	}
};
//--------------------------------------------------------------------------------------
//Local -> Client
//符文升等 結果
struct PG_Item_LtoC_RefineRuneResult
{
	GamePGCommandEnum	Command;

	int		RunePos[5];
	int		BoxPos;
	bool	Result;

	PG_Item_LtoC_RefineRuneResult()
	{
		Command = EM_PG_Item_LtoC_RefineRuneResult;
	}
};
//--------------------------------------------------------------------------------------
*/
// Client -> Local 
//寶盒合成
struct PG_Item_CtoL_MagicBoxRequest
{
	GamePGCommandEnum	Command;

	PG_Item_CtoL_MagicBoxRequest()
	{
		Command = EM_PG_Item_CtoL_MagicBoxRequest;
	}
};

//寶盒合成 結果
//Local -> Client
struct PG_Item_LtoC_MagicBoxResult
{
	GamePGCommandEnum	Command;

	ItemFieldStruct		Item;
	bool				Result;

	PG_Item_LtoC_MagicBoxResult()
	{
		Command = EM_PG_Item_LtoC_MagicBoxResult;
	}
};
//--------------------------------------------------------------------------------------
//通知週圍玩家使用裝備
//Local -> Client
struct PG_Item_LtoC_UseEq
{
	GamePGCommandEnum	Command;
	
	int		PlayerGUID;
	int		EqID;

	PG_Item_LtoC_UseEq()
	{
		Command = EM_PG_Item_LtoC_UseEq;
	}
};
//------------------------------------------------------------------------------------------------------------
// Client -> Server
// 要求 賭屬性
struct PG_Item_CtoL_GamebleItemState
{
	GamePGCommandEnum	Command;

	int					TableID;
	int					ItemPos;
	ItemFieldStruct		BodyItem;
	int					LockFlag;		//鎖定不變的屬性

	PG_Item_CtoL_GamebleItemState()
	{
		Command = EM_PG_Item_CtoL_GamebleItemState;
	}   
};
//------------------------------------------------------------------------------------------------------------
enum GamebleItemStateResultENUM
{
	EM_GamebleItemStateResult_OK					,
	EM_GamebleItemStateResult_Failed				,	
	EM_GamebleItemStateResult_DataError				,
	EM_GamebleItemStateResult_LockCountError		,
	EM_GamebleItemStateResult_AttributeCountError	,
	EM_GamebleItemStateResult_TableIDError			,
	EM_GamebleItemStateResult_MoneyError			,
	EM_GamebleItemStateResult_TableDataError		,	//掉寶表有問題
	EM_GamebleItemStateResult_LockItemError			,
};
// Local -> Client
// 要求 賭屬性結果
struct PG_Item_LtoC_GamebleItemStateResult
{
	GamePGCommandEnum			Command;
	int							TableID;
	int							StatePos;
	int							StateID;
	GamebleItemStateResultENUM	Result;

	PG_Item_LtoC_GamebleItemStateResult()
	{
		Command = EM_PG_Item_LtoC_GamebleItemStateResult;
	}   
};
//------------------------------------------------------------------------------------------------------------
// Client -> Local
// 確定要此屬性
struct PG_Item_CtoL_GetGamebleItemState
{
	GamePGCommandEnum	Command;
	bool				GiveUp;		//是否放棄

	PG_Item_CtoL_GetGamebleItemState()
	{
		Command = EM_PG_Item_CtoL_GetGamebleItemState;
	}   
};
//------------------------------------------------------------------------------------------------------------
// Local -> Client
// 確定要此屬性
struct PG_Item_LtoC_GetGamebleItemStateResult
{
	GamePGCommandEnum	Command;
	bool				Result;		//是否放棄

	PG_Item_LtoC_GetGamebleItemStateResult()
	{
		Command = EM_PG_Item_LtoC_GetGamebleItemStateResult;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//開啟物品賭博介面
struct PG_Item_LtoC_OpenGamebleItemState
{
	GamePGCommandEnum	Command;
	int					TargetID;	//觸發物件

	PG_Item_LtoC_OpenGamebleItemState()
	{
		Command = EM_PG_Item_LtoC_OpenGamebleItemState;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//關閉賭博介面
struct PG_Item_CtoL_CloseGamebleItemState
{
	GamePGCommandEnum	Command;

	PG_Item_CtoL_CloseGamebleItemState()
	{
		Command = EM_PG_Item_CtoL_CloseGamebleItemState;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//關閉賭博介面
struct PG_Item_LtoC_CloseGamebleItemState
{
	GamePGCommandEnum	Command;

	PG_Item_LtoC_CloseGamebleItemState()
	{
		Command = EM_PG_Item_LtoC_CloseGamebleItemState;
	}   
};

//------------------------------------------------------------------------------------------------------------
//刪除某物品
//Local -> Client
struct PG_Item_LtoC_DelItemMsg
{
	GamePGCommandEnum	Command;
	ItemFieldStruct		Item;

	PG_Item_LtoC_DelItemMsg()
	{
		Command = EM_PG_Item_LtoC_DelItemMsg;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client 
//開啟兌換魔石的介面
struct PG_Item_LtoC_OpenMagicStoneExchange
{
	GamePGCommandEnum	Command;
	int					MagicStoneID;	//
	int					Lv;				//階層
	int					Money[2];			

	PG_Item_LtoC_OpenMagicStoneExchange()
	{
		Command = EM_PG_Item_LtoC_OpenMagicStoneExchange;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client -> Server
//確定兌換結果
struct PG_Item_CtoL_MagicStoneExchangeResult
{
	GamePGCommandEnum	Command;
	int		AbilityID[3];		//屬性ID
	bool	Result;

	PG_Item_CtoL_MagicStoneExchangeResult()
	{
		Command = EM_PG_Item_CtoL_MagicStoneExchangeResult;
	}   
};
//------------------------------------------------------------------------------------------------------------
enum MagicStoneExchangeResultENUM
{
	EM_MagicStoneExchangeResult_OK			,
	EM_MagicStoneExchangeResult_NotOpen		,	//介面沒開
	EM_MagicStoneExchangeResult_MoneyErr	,	
	EM_MagicStoneExchangeResult_ClientCancel,
	EM_MagicStoneExchangeResult_ClientDataErr,	//client 端送來的資料有問題
};
//Local -> Client 
//開啟兌換魔石的介面
struct PG_Item_LtoC_MagicStoneExchangeResult
{
	GamePGCommandEnum	Command;
	ItemFieldStruct		Item;
	MagicStoneExchangeResultENUM				Result;
	int					Money[2];
	PG_Item_LtoC_MagicStoneExchangeResult()
	{
		Command = EM_PG_Item_LtoC_MagicStoneExchangeResult;
	}   
};
//------------------------------------------------------------------------------------------------------------
//取出鑲入符文
//Client -> Local
struct PG_Item_CtoL_DrawOutRuneRequest
{
	GamePGCommandEnum	Command;
	int					BodyPos;
	int					RuneID;

	PG_Item_CtoL_DrawOutRuneRequest()
	{
		Command = EM_PG_Item_CtoL_DrawOutRuneRequest;
	}   
};

enum DrawOutRuneResultENUM
{
	EM_DrawOutRuneResult_OK				,
	EM_DrawOutRuneResult_DataErr		,
	EM_DrawOutRuneResult_MoneyErr		,
	EM_DrawOutRuneResult_RuneErr		,
	EM_DrawOutRuneResult_ItemQueueErr	,	//太多東西在 物品queue內
	EM_DrawOutRuneResult_ItemLock		,	//物品安全鎖
};
//取出鑲入符文
//Local -> Client
struct PG_Item_LtoC_DrawOutRuneResult
{
	GamePGCommandEnum	Command;
	DrawOutRuneResultENUM	Result;

	PG_Item_LtoC_DrawOutRuneResult()
	{
		Command = EM_PG_Item_LtoC_DrawOutRuneResult;
	}   
};

//////////////////////////////////////////////////////////////////////////
//酷裝衣櫥
//////////////////////////////////////////////////////////////////////////
//Client -> Local 要求把褲裝放入 衣櫥
struct PG_Item_CtoL_PutCoolClothRequest
{
	GamePGCommandEnum	Command;
	int					BodyPos;	
	int					CoolClothPos;

	PG_Item_CtoL_PutCoolClothRequest()
	{
		Command = EM_PG_Item_CtoL_PutCoolClothRequest;
	}   
};

enum PutCoolClothResultENUM
{
	EM_PutCoolClothResult_OK			,
	EM_PutCoolClothResult_NotFind		,
	EM_PutCoolClothResult_TypeErr		,
	EM_PutCoolClothResult_Exist			,	//物品已經存在
	EM_PutCoolClothResult_RefineErr		,	//物品有精煉過
	EM_PutCoolClothResult_FullErr		,	//欄位已滿
	EM_PutCoolClothResult_ClothPosErr	,
	EM_PutCoolClothResult_LimitErr		,	//物品限制有問題
	EM_PutCoolClothResult_KeyItemErr	,	//此頁沒有購買
};

//Local -> Client 回應放入褲裝的解果
struct PG_Item_LtoC_PutCoolClothResult
{
	GamePGCommandEnum	Command;
	PutCoolClothResultENUM	Result;

	PG_Item_LtoC_PutCoolClothResult()
	{
		Command = EM_PG_Item_LtoC_PutCoolClothResult;
	}   
};

//Client -> Local 交換酷裝位置
struct PG_Item_CtoL_SwapCoolClothRequest
{
	GamePGCommandEnum	Command;
	int					Pos[2];

	PG_Item_CtoL_SwapCoolClothRequest()
	{
		Command = EM_PG_Item_CtoL_SwapCoolClothRequest;
	}   
};

//Local -> Client 交換酷裝位置
struct PG_Item_LtoC_SwapCoolClothResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Item_LtoC_SwapCoolClothResult()
	{
		Command = EM_PG_Item_LtoC_SwapCoolClothResult;
	}   
};

//////////////////////////////////////////////////////////////////////////
//刪除某一件酷裝
//Client -> Local 
struct PG_Item_CtoL_DelCoolClothRequest
{
	GamePGCommandEnum	Command;
	int					Pos;
	int					ImageObjectID;	

	PG_Item_CtoL_DelCoolClothRequest()
	{
		Command = EM_PG_Item_CtoL_DelCoolClothRequest;
	}   
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client 回應刪除結果
struct PG_Item_LtoC_DelCoolClothResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Item_LtoC_DelCoolClothResult()
	{
		Command = EM_PG_Item_LtoC_DelCoolClothResult;
	}   
};
//////////////////////////////////////////////////////////////////////////

//Client -> Local 要求設定一套褲裝
struct PG_Item_CtoL_SetCoolSuitRequest
{
	GamePGCommandEnum	Command;

	int				SuitIndexID;
	CoolSuitBase	Info;

	PG_Item_CtoL_SetCoolSuitRequest()
	{
		Command = EM_PG_Item_CtoL_SetCoolSuitRequest;
	}   
};
//Local -> Client 回應結果 設定一套褲裝
struct PG_Item_LtoC_SetCoolSuitResult
{
	GamePGCommandEnum	Command;
	bool	Result;

	PG_Item_LtoC_SetCoolSuitResult()
	{
		Command = EM_PG_Item_LtoC_SetCoolSuitResult;
	}   
};

//Client -> Local 要求設定穿某套褲裝
struct PG_Item_CtoL_ShowCoolSuitIndexRequest
{
	GamePGCommandEnum	Command;
	int		CoolSuitIndexID;
	
	PG_Item_CtoL_ShowCoolSuitIndexRequest()
	{
		Command = EM_PG_Item_CtoL_ShowCoolSuitIndexRequest;
	}   
};

//Local -> Client 回應結果
struct PG_Item_LtoC_ShowCoolSuitIndexResult
{
	GamePGCommandEnum	Command;
	bool	Result;

	PG_Item_LtoC_ShowCoolSuitIndexResult()
	{
		Command = EM_PG_Item_LtoC_ShowCoolSuitIndexResult;
	}   
};
//////////////////////////////////////////////////////////////////////////
//特殊包
//////////////////////////////////////////////////////////////////////////
//Client -> Local 把物品放入特殊的背包( 可使用　不可堆疊　不消耗　)
struct PG_Item_CtoL_PutInExBagRequest
{
	GamePGCommandEnum	Command;
	ExBagTypeENUM		ExBagType;
	int					ItemID;
	int					BodyPos;

	PG_Item_CtoL_PutInExBagRequest()
	{
		Command = EM_PG_Item_CtoL_PutInExBagRequest;
	}   
};

enum PutInExBagResultENUM
{
	EM_PutInExBagResult_OK		,
	EM_PutInExBagResult_DataErr	,
	EM_PutInExBagResult_Exist	,
	EM_PutInExBagResult_Full	,
};
//Local -> Client 物品放入結果
struct PG_Item_LtoC_PutInExBagResult
{
	GamePGCommandEnum	Command;
	PutInExBagResultENUM	Result;
	PG_Item_LtoC_PutInExBagResult()
	{
		Command = EM_PG_Item_LtoC_PutInExBagResult;
	}   
};

struct PG_Item_CtoL_PopExBagRequest
{
	GamePGCommandEnum	Command;
	ExBagTypeENUM		ExBagType;
	int					ExBagPos;
	int					BodyPos;

	PG_Item_CtoL_PopExBagRequest()
	{
		Command = EM_PG_Item_CtoL_PopExBagRequest;
	}   
};

enum PopExBagResultENUM
{
	EM_PopExBagResult_OK		,
	EM_PopExBagResult_Failed	,
};
//Local -> Client 物品放入結果
struct PG_Item_LtoC_PopExBagResult
{
	GamePGCommandEnum	Command;
	PopExBagResultENUM	Result;
	PG_Item_LtoC_PopExBagResult()
	{
		Command = EM_PG_Item_LtoC_PopExBagResult;
	}   
};

//Client -> Local 要求使用
struct PG_Item_CtoL_UseExBagRequest
{
	GamePGCommandEnum	Command;
	ExBagTypeENUM		ExBagType;
	int					BagPos;

	PG_Item_CtoL_UseExBagRequest()
	{
		Command = EM_PG_Item_CtoL_UseExBagRequest;
	}   
};

//Local -> Client 使用結果
struct PG_Item_LtoC_UseExBagResult
{
	GamePGCommandEnum	Command;
	bool	Result;

	PG_Item_LtoC_UseExBagResult()
	{
		Command = EM_PG_Item_LtoC_UseExBagResult;
	}   
};
//Local -> Client
//開啟稀有度3 的裝備兌換物品介面
struct PG_Item_LtoC_OpenRare3EqExchangeItem
{
	GamePGCommandEnum	Command;
	int					TargetNPCID;

	PG_Item_LtoC_OpenRare3EqExchangeItem()
	{
		Command = EM_PG_Item_LtoC_OpenRare3EqExchangeItem;
	}
};

//Client -> Local
//兌換要求
struct PG_Item_CtoL_Rare3EqExchangeItemRequest
{
	GamePGCommandEnum	Command;
	int					Type;		//兌換類型
	int					ItemPos[6];	//身體欄位位置

	PG_Item_CtoL_Rare3EqExchangeItemRequest()
	{
		Command = EM_PG_Item_CtoL_Rare3EqExchangeItemRequest;
	}
};

enum Rare3EqExchangeItemRequestENUM
{
	EM_Rare3EqExchangeItem_OK			,
	EM_Rare3EqExchangeItem_MoneyError	,
	EM_Rare3EqExchangeItem_EQErr		,
	EM_Rare3EqExchangeItem_LuaErr		,
};

//Local -> Client 
//兌換結果
struct PG_Item_LtoC_Rare3EqExchangeItemResult
{
	GamePGCommandEnum	Command;
	Rare3EqExchangeItemRequestENUM	Result;
	
	PG_Item_LtoC_Rare3EqExchangeItemResult()
	{
		Command = EM_PG_Item_LtoC_Rare3EqExchangeItemResult;
	}
};

//Client -> Local
//要求關閉
struct PG_Item_CtoL_CloseRare3EqExchangeItem
{
	GamePGCommandEnum	Command;

	PG_Item_CtoL_CloseRare3EqExchangeItem()
	{
		Command = EM_PG_Item_CtoL_CloseRare3EqExchangeItem;
	}
};

//////////////////////////////////////////////////////////////////////////
//Client -> Local
//要求清除某一個屬性
struct PG_Item_CtoL_ClsAttribute
{
	GamePGCommandEnum	Command;
	int					Type;				//(0 Body 1 Bank )
	int					Pos;				//物品放置的位置
	int					AttributeID;

	PG_Item_CtoL_ClsAttribute()
	{
		Command = EM_PG_Item_CtoL_ClsAttribute;
	}
};

//Local -> Client
//要求清除某一個屬性
struct PG_Item_LtoC_ClsAttributeResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Item_LtoC_ClsAttributeResult()
	{
		Command = EM_PG_Item_LtoC_ClsAttributeResult;
	}
};

//Client -> Local 
//屬性萃取
struct PG_Item_CtoL_MagicBoxAttrDrawOff
{
	GamePGCommandEnum	Command;
	PG_Item_CtoL_MagicBoxAttrDrawOff()
	{
		Command = EM_PG_Item_CtoL_MagicBoxAttrDrawOff;
	}
};

//Local -> Client
//要求清除某一個屬性
struct PG_Item_LtoC_MagicBoxAttrDrawOffResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Item_LtoC_MagicBoxAttrDrawOffResult()
	{
		Command = EM_PG_Item_LtoC_MagicBoxAttrDrawOffResult;
	}
};

//////////////////////////////////////////////////////////////////////////
//雕文共鳴組
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//要求放入共鳴組物品
//Local -> Client
//要求放入共鳴組物品結果

//Client -> Local
//要求取出共鳴組物品
//Local -> Client
//要求取出共鳴組物品結果

//Client -> Local
//要求放入雕文物品
//Local -> Client
//要求放入雕文物品結果

//Client -> Local
//要求取出雕文物品
//Local -> Client
//要求取出雕文物品結果

//Client -> Local
//要求使用共鳴
//Local -> Client
//要求使用共鳴結果



