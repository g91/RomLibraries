#pragma once
#include "RoleData\RoleDataEx.h"
#include "NetWakerPGEnum.h"

//////////////////////////////////////////////////////////////////////////
//買房子
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter 要求建立房屋
struct PG_Houses_LtoD_BuildHouseRequest
{
	GamePGCommandEnum	Command;
	int		PlayerDBID;
	int		HouseType;
	StaticString<_MAX_ACCOUNT_SIZE_>    Account_ID;

	PG_Houses_LtoD_BuildHouseRequest()
	{
		Command = EM_PG_Houses_LtoD_BuildHouseRequest;
	}
};
//------------------------------------------------------------------------
//DataCenter -> Local 建立房屋結果
struct PG_Houses_DtoL_BuildHouseResult
{
	GamePGCommandEnum	Command;
	int		PlayerDBID;
	int		HouseDBID;
	bool	Result;

	PG_Houses_DtoL_BuildHouseResult()
	{
		Command = EM_PG_Houses_DtoL_BuildHouseResult;
	}
};
//------------------------------------------------------------------------
//Local -> Client 建立房屋結果
struct PG_Houses_LtoC_BuildHouseResult
{
	GamePGCommandEnum	Command;
	int		HouseDBID;
	bool	Result;

	PG_Houses_LtoC_BuildHouseResult()
	{
		Command = EM_PG_Houses_LtoC_BuildHouseResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//進入屋子
//////////////////////////////////////////////////////////////////////////
//Local -> Client 要求開啟參觀房子的介面
struct PG_Houses_LtoC_OpenVisitHouse
{
	GamePGCommandEnum	Command;

	int					TargetNpcID;
	bool				IsVisitMyHouse;

	PG_Houses_LtoC_OpenVisitHouse()
	{
		Command = EM_PG_Houses_LtoC_OpenVisitHouse;
	}
};
//------------------------------------------------------------------------
//Client -> Local 要求參觀某房子與(id,password)
struct PG_Houses_CtoL_VisitHouseRequest
{
	GamePGCommandEnum	Command;
	int			HouseDBID;		//參觀房間號碼
	StaticString< _MAX_NAMERECORD_SIZE_ >   Password;
	

	PG_Houses_CtoL_VisitHouseRequest()
	{
		Command = EM_PG_Houses_CtoL_VisitHouseRequest;
	}
};
//------------------------------------------------------------------------
//Local -> DataCenter 查尋此房間號碼是否存在 密碼是否正確
struct PG_Houses_LtoD_FindHouseRequest
{
	GamePGCommandEnum	Command;
	int			PlayerDBID;	//要求的玩家
	int			HouseDBID;		//參觀房間號碼
	StaticString< _MAX_NAMERECORD_SIZE_ >   Password;
	int			ManageLv;

	PG_Houses_LtoD_FindHouseRequest()
	{
		Command = EM_PG_Houses_LtoD_FindHouseRequest;
	}
};
//------------------------------------------------------------------------
//Client -> Local 要求參觀某房子與(id,password)
struct PG_Houses_CtoL_VisitHouseRequest_ByName
{
	GamePGCommandEnum	Command;
	//int			HouseDBID;		//參觀房間號碼
	StaticString< _MAX_NAMERECORD_SIZE_ >   RoleName;
	StaticString< _MAX_NAMERECORD_SIZE_ >   Password;


	PG_Houses_CtoL_VisitHouseRequest_ByName()
	{
		Command = EM_PG_Houses_CtoL_VisitHouseRequest_ByName;
	}
};
//------------------------------------------------------------------------
//Local -> DataCenter 查尋此房間號碼是否存在 密碼是否正確
struct PG_Houses_LtoD_FindHouseRequest_ByName
{
	GamePGCommandEnum	Command;
	int			PlayerDBID;	//要求的玩家
	StaticString< _MAX_NAMERECORD_SIZE_ >   RoleName;
	StaticString< _MAX_NAMERECORD_SIZE_ >   Password;
	int			ManageLv;

	PG_Houses_LtoD_FindHouseRequest_ByName()
	{
		Command = EM_PG_Houses_LtoD_FindHouseRequest_ByName;
	}
};
//------------------------------------------------------------------------
enum FindHouseResultENUM
{
	EM_FindHouseResult_OK				,
	EM_FindHouseResult_PasswordError	,
	EM_FindHouseResult_NotFind			,
	EM_FindHouseResult_Unknow			,
};
//DataCenter -> Local 查尋房間號碼是否存在 密碼是否正確 結果
struct PG_Houses_DtoL_FindHouseResult
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;	//要求的玩家
	int					HouseDBID;
	FindHouseResultENUM	Result;

	PG_Houses_DtoL_FindHouseResult()
	{
		Command = EM_PG_Houses_DtoL_FindHouseResult;
	}
};

//Local -> Client 查尋房間號碼是否存在 密碼是否正確 結果
struct PG_Houses_LtoC_FindHouseResult
{
	GamePGCommandEnum	Command;
	FindHouseResultENUM	Result;

	PG_Houses_LtoC_FindHouseResult()
	{
		Command = EM_PG_Houses_LtoC_FindHouseResult;
	}
};
//------------------------------------------------------------------------
//Local -> Client 關閉參觀房子的介面
struct PG_Houses_LtoC_CloseVisitHouse
{
	GamePGCommandEnum	Command;
	PG_Houses_LtoC_CloseVisitHouse()
	{
		Command = EM_PG_Houses_LtoC_CloseVisitHouse;
	}
};
//------------------------------------------------------------------------
//Local -> Client 關閉參觀房子的介面
struct PG_Houses_CtoL_CloseVisitHouse
{
	GamePGCommandEnum	Command;
	PG_Houses_CtoL_CloseVisitHouse()
	{
		Command = EM_PG_Houses_CtoL_CloseVisitHouse;
	}
};
//------------------------------------------------------------------------
//Local -> Client 等待載入房間資料
struct PG_Houses_LtoC_HouseInfoLoading
{
	GamePGCommandEnum	Command;
	PG_Houses_LtoC_HouseInfoLoading()
	{
		Command = EM_PG_Houses_LtoC_HouseInfoLoading;
	}
};
//------------------------------------------------------------------------
//Local -> DataCenter 要求建立房間
struct PG_Houses_LtoD_HouseInfoLoading
{
	GamePGCommandEnum	Command;
	int					HouseDBID;
	PG_Houses_LtoD_HouseInfoLoading()
	{
		Command = EM_PG_Houses_LtoD_HouseInfoLoading;
	}
};
//------------------------------------------------------------------------
//DataCenter -> Local 房屋資料
struct PG_Houses_DtoL_HouseBaseInfo
{
	GamePGCommandEnum	Command;
	HouseBaseDBStruct	HouseBaseInfo;

	PG_Houses_DtoL_HouseBaseInfo()
	{
		Command = EM_PG_Houses_DtoL_HouseBaseInfo;
	}
};
//------------------------------------------------------------------------
//DataCenter -> Local 表面物品資料
struct PG_Houses_DtoL_ItemInfo
{
	GamePGCommandEnum	Command;
	int		HouseDBID;
	int		ItemCount;
	int		ParentItemDBID;
	HouseItemDBStruct Item[ _DEF_MAX_HOUSE_FURNITURE + 10 ];		//最多一次送100筆 + 10個女僕

	PG_Houses_DtoL_ItemInfo()
	{
		Command = EM_PG_Houses_DtoL_ItemInfo;
	}
};
//------------------------------------------------------------------------
//Local -> Client 房屋資料
struct PG_Houses_LtoC_HouseBaseInfo
{
	GamePGCommandEnum	Command;
	HouseBaseDBStruct	HouseBaseInfo;

	PG_Houses_LtoC_HouseBaseInfo()
	{
		Command = EM_PG_Houses_LtoC_HouseBaseInfo;
	}
};
//------------------------------------------------------------------------
//Local -> Client 物品資料	
struct PG_Houses_LtoC_ItemInfo
{
	GamePGCommandEnum	Command;
	int		ItemDBID;
	int		ItemCount;
	HouseItemDBStruct Item[_DEF_MAX_HOUSE_FURNITURE+10];		//最多一次送100筆

	PG_Houses_LtoC_ItemInfo()
	{
		Command = EM_PG_Houses_LtoC_ItemInfo;
	}
	int Size()
	{
		return sizeof(*this)-sizeof(Item)+ItemCount*sizeof(HouseItemDBStruct);
	}
};
//------------------------------------------------------------------------
//DataCenter -> Local 通知讀取完畢							
struct PG_Houses_DtoL_HouseInfoLoadOK
{
	GamePGCommandEnum	Command;
	int		HouseDBID;

	PG_Houses_DtoL_HouseInfoLoadOK()
	{
		Command = EM_PG_Houses_DtoL_HouseInfoLoadOK;
	}
};
//------------------------------------------------------------------------
//Local -> Client 載入完成
struct PG_Houses_LtoC_HouseInfoLoadOK
{
	GamePGCommandEnum	Command;

	PG_Houses_LtoC_HouseInfoLoadOK()
	{
		Command = EM_PG_Houses_LtoC_HouseInfoLoadOK;
	}
};
//////////////////////////////////////////////////////////////////////////
//放置物品
//////////////////////////////////////////////////////////////////////////
//Client -> Local 要求放置物品 or 取出物品
struct PG_Houses_CtoL_SwapBodyHouseItem
{
	GamePGCommandEnum	Command;
	int		BodyPos;
	int		HouseParentItemDBID;
	int		HousePos;
	bool	IsFromBody;

	PG_Houses_CtoL_SwapBodyHouseItem()
	{
		Command = EM_PG_Houses_CtoL_SwapBodyHouseItem;
	}
};
//------------------------------------------------------------------------
/*
//Local -> DataCenter 立即儲存放置物品
struct PG_Houses_LtoD_SaveSwapBodyHouseItem
{
	GamePGCommandEnum	Command;
	int		PlayerDBID;
	int		BodyPos;
	ItemFieldStruct BodyItem;
	HouseItemDBStruct HouseItem;

	PG_Houses_LtoD_SaveSwapBodyHouseItem()
	{
		Command = EM_PG_Houses_LtoD_SaveSwapBodyHouseItem;
	}
};
*/
//------------------------------------------------------------------------
//Local -> Client 通知放置結果
struct PG_Houses_LtoC_SwapBodyHouseItemResult
{
	GamePGCommandEnum	Command;
	int		BodyPos;
	int		HousePos;			
	int		HouseParentItemDBID;
	bool	Result;

	PG_Houses_LtoC_SwapBodyHouseItemResult()
	{
		Command = EM_PG_Houses_LtoC_SwapBodyHouseItemResult;
	}
};
//------------------------------------------------------------------------
//Local -> Client 修正封包Client物品資料
struct PG_Houses_LtoC_FixHouseItem
{
	GamePGCommandEnum	Command;
	HouseItemDBStruct	HouseItem;

	PG_Houses_LtoC_FixHouseItem()
	{
		Command = EM_PG_Houses_LtoC_FixHouseItem;
	}
};
//------------------------------------------------------------------------

//Client -> Local 要求物品搬移
struct PG_Houses_CtoL_SwapHouseItemRequest
{
	GamePGCommandEnum	Command;
	int			HousePos[2];		//高位元 母物件位置   低位元 目前位置
	int			HouseParentItemDBID[2];

	PG_Houses_CtoL_SwapHouseItemRequest()
	{
		Command = EM_PG_Houses_CtoL_SwapHouseItemRequest;
	}
};
//------------------------------------------------------------------------
//Local -> Client 通知搬移結果
struct PG_Houses_LtoC_SwapHouseItemResult
{
	GamePGCommandEnum	Command;	
	int			HousePos[2];		//高位元 母物件位置   低位元 目前位置
	int			ParentItemDBID[2];
	bool		Result;

	PG_Houses_LtoC_SwapHouseItemResult()
	{
		Command = EM_PG_Houses_LtoC_SwapHouseItemResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//物品擺設
//////////////////////////////////////////////////////////////////////////
//Client -> Local 擺設
struct PG_Houses_CtoL_HouseItemLayoutRequest
{
	GamePGCommandEnum	Command;
	int		ItemDBID;
	House3DLayoutStruct	Layout;

	PG_Houses_CtoL_HouseItemLayoutRequest()
	{
		Command = EM_PG_Houses_CtoL_HouseItemLayoutRequest;
	}
};
//------------------------------------------------------------------------
//Local -> Client 擺設結果
struct PG_Houses_LtoC_HouseItemLayoutResult
{
	GamePGCommandEnum	Command;
	int		HouseItemDBID;
	bool	Result;

	PG_Houses_LtoC_HouseItemLayoutResult()
	{
		Command = EM_PG_Houses_LtoC_HouseItemLayoutResult;
	}
};
//////////////////////////////////////////////////////////////////////////
 
//////////////////////////////////////////////////////////////////////////
//Client -> Local 要求升級房子
struct PG_Houses_CtoL_ChangeHouseRequest
{
	GamePGCommandEnum	Command;
	int	HouseType;

	PG_Houses_CtoL_ChangeHouseRequest()
	{
		Command = EM_PG_Houses_CtoL_ChangeHouseRequest;
	}
};
//------------------------------------------------------------------------
//Local -> Client 回應升級房子結果
struct PG_Houses_LtoC_ChangeHouseResult
{
	GamePGCommandEnum	Command;
	int		HouseType;
	bool	Result;

	PG_Houses_LtoC_ChangeHouseResult()
	{
		Command = EM_PG_Houses_LtoC_ChangeHouseResult;
	}
};
//------------------------------------------------------------------------
 
struct PG_Houses_CtoL_BuyFunctionRequest
{
	GamePGCommandEnum	Command;
	int		FunctionID;

	PG_Houses_CtoL_BuyFunctionRequest()
	{
		Command = EM_PG_Houses_CtoL_BuyFunctionRequest;
	}
};
//------------------------------------------------------------------------
//Local -> Client 回應購買結果
struct PG_Houses_LtoC_BuyFunctionResult
{
	GamePGCommandEnum	Command;
	int		FunctionID;
	bool	Result;

	PG_Houses_LtoC_BuyFunctionResult()
	{
		Command = EM_PG_Houses_LtoC_BuyFunctionResult;
	}
};
//------------------------------------------------------------------------
//Client -> Local 購買公能設定
struct PG_Houses_CtoL_FunctionSettingRequest
{
	GamePGCommandEnum	Command;
	int		FunctionID;
	int		Mode;		//0 解約

	PG_Houses_CtoL_FunctionSettingRequest()
	{
		Command = EM_PG_Houses_CtoL_FunctionSettingRequest;
	}
};
//------------------------------------------------------------------------
//Local -> Client 回應購買設定結果
struct PG_Houses_LtoC_FunctionSettingResult
{
	GamePGCommandEnum	Command;
	int		FunctionID;
	int		Mode;
	bool	Result;

	PG_Houses_LtoC_FunctionSettingResult()
	{
		Command = EM_PG_Houses_LtoC_FunctionSettingResult;
	}
};
//------------------------------------------------------------------------
//Client -> Local 購買能量點數
struct PG_Houses_CtoL_BuyEnergyRequest
{
	GamePGCommandEnum	Command;
	int		Money_Account;		
	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;


	PG_Houses_CtoL_BuyEnergyRequest()
	{
		Command = EM_PG_Houses_CtoL_BuyEnergyRequest;
	}
};
//------------------------------------------------------------------------
//Local -> Client 回應購買能量點數結果
struct PG_Houses_LtoC_BuyEnergyResult
{
	GamePGCommandEnum	Command;
	int		Money_Account;		
	int		Energy;
	bool	Result;

	PG_Houses_LtoC_BuyEnergyResult()
	{
		Command = EM_PG_Houses_LtoC_BuyEnergyResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//其它
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter 儲存房屋基本資料
struct PG_Houses_LtoD_SaveHouseBase
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	HouseBaseDBStruct	HouseBaseInfo;

	PG_Houses_LtoD_SaveHouseBase()
	{
		Command = EM_PG_Houses_LtoD_SaveHouseBase;
	}
};
//------------------------------------------------------------------------
//儲存房間物品
//Local -> DataCenter
struct PG_Houses_LtoD_SaveItem
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	HouseItemDBStruct	Item;

	PG_Houses_LtoD_SaveItem()
	{
		Command = EM_PG_Houses_LtoD_SaveItem;
	}
};
//------------------------------------------------------------------------
//要求某人離開房間
//Client -> Local
struct PG_Houses_CtoL_LeaveHouse
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;

	PG_Houses_CtoL_LeaveHouse()
	{
		Command = EM_PG_Houses_CtoL_LeaveHouse;
	}
};
//------------------------------------------------------------------------
//要求某寶箱資料
//Client -> Local
struct PG_Houses_CtoL_BoxInfoRequset
{
	GamePGCommandEnum	Command;
	int					BoxItemDBID;

	PG_Houses_CtoL_BoxInfoRequset()
	{
		Command = EM_PG_Houses_CtoL_BoxInfoRequset;
	}
};
//------------------------------------------------------------------------
//要求某寶箱資料
//Client -> Local
struct PG_Houses_LtoC_BoxInfoResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Houses_LtoC_BoxInfoResult()
	{
		Command = EM_PG_Houses_LtoC_BoxInfoResult;
	}
};
//------------------------------------------------------------------------
//要求與某個寶箱交換裝備
//Client -> Local
struct PG_Houses_CtoL_ExchangeEq
{
	GamePGCommandEnum	Command;
	int					BoxItemDBID;
	int					BoxPos;
	EQWearPosENUM		EqPos;

	PG_Houses_CtoL_ExchangeEq()
	{
		Command = EM_PG_Houses_CtoL_ExchangeEq;
	}
};
//------------------------------------------------------------------------
//要求與某個寶箱交換裝備OK
//Local -> Client
struct PG_Houses_LtoC_ExchangeEqResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Houses_LtoC_ExchangeEqResult()
	{
		Command = EM_PG_Houses_LtoC_ExchangeEqResult;
	}
};
//------------------------------------------------------------------------
//設定房屋密碼
// Client -> Local
struct PG_Houses_CtoL_SetPassword
{
	GamePGCommandEnum	Command;
	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

	PG_Houses_CtoL_SetPassword()
	{
		Command = EM_PG_Houses_CtoL_SetPassword;
	}
};
//------------------------------------------------------------------------
//設定房屋密碼結果
// Local -> Client
struct PG_Houses_LtoC_SetPasswordResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Houses_LtoC_SetPasswordResult()
	{
		Command = EM_PG_Houses_LtoC_SetPasswordResult;
	}
};
//------------------------------------------------------------------------
//設定房屋名稱
// Client -> Local
struct PG_Houses_CtoL_SetHouseName
{
	GamePGCommandEnum	Command;
	StaticString< _MAX_NAMERECORD_SIZE_ >		HouseName;

	PG_Houses_CtoL_SetHouseName()
	{
		Command = EM_PG_Houses_CtoL_SetHouseName;
	}
};
//------------------------------------------------------------------------
//設定房屋Client 端資料
// Client -> Local
struct PG_Houses_CtoL_SetClientData
{
	GamePGCommandEnum	Command;
	char	ClientData[ MAX_HOUSE_CLIENT_DATA_SIZE_ ];

	PG_Houses_CtoL_SetClientData()
	{
		Command = EM_PG_Houses_CtoL_SetClientData;
	}
};
//------------------------------------------------------------------------
enum SetHouseNameResultENUM
{
	EM_SetHouseNameResult_OK		,
	EM_SetHouseNameResult_NameErr	,
	EM_SetHouseNameResult_EnergyErr	,
	EM_SetHouseNameResult_DataErr	,
};
//設定房屋名稱結果
// Local -> Client
struct PG_Houses_LtoC_SetHouseNameResult
{
	GamePGCommandEnum	Command;
	SetHouseNameResultENUM	Result;

	PG_Houses_LtoC_SetHouseNameResult()
	{
		Command = EM_PG_Houses_LtoC_SetHouseNameResult;
	}
};
//------------------------------------------------------------------------
//刪除房屋內的物品
// Client -> Local
struct PG_Houses_CtoL_DestroyItemRequest
{
	GamePGCommandEnum	Command;
	int					ItemDBID;

	PG_Houses_CtoL_DestroyItemRequest()
	{
		Command = EM_PG_Houses_CtoL_DestroyItemRequest;
	}
};
//------------------------------------------------------------------------
//刪除房屋內的物品
// Local -> Client 
struct PG_Houses_LtoC_DestroyItemResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Houses_LtoC_DestroyItemResult()
	{
		Command = EM_PG_Houses_LtoC_DestroyItemResult;
	}
};
//------------------------------------------------------------------------
// DataCenter -> Local
// 房屋好友
struct PG_Houses_DtoL_FriendDBID
{
	GamePGCommandEnum	Command;
	int				HouseDBID;
	int				FriendDBID;

	PG_Houses_DtoL_FriendDBID()
	{
		Command = EM_PG_Houses_DtoL_FriendDBID;
	}
};
//------------------------------------------------------------------------
//Local -> Client
// 房屋好友
struct PG_Houses_LtoC_FriendDBID
{
	GamePGCommandEnum	Command;
	int				HouseDBID;
	int				FriendDBID;

	PG_Houses_LtoC_FriendDBID()
	{
		Command = EM_PG_Houses_LtoC_FriendDBID;
	}
};
//------------------------------------------------------------------------
struct PG_Houses_CtoL_ModifyFriendDBID
{
	GamePGCommandEnum	Command;
	bool			IsDelete;		//0 新增　1刪除	
	int				FriendDBID;

	PG_Houses_CtoL_ModifyFriendDBID()
	{
		Command = EM_PG_Houses_CtoL_ModifyFriendDBID;
	}
};
//------------------------------------------------------------------------
struct PG_Houses_LtoC_ModifyFriendDBID
{
	GamePGCommandEnum	Command;
	bool			IsDelete;		//0 新增　1刪除	
	int				FriendDBID;

	PG_Houses_LtoC_ModifyFriendDBID()
	{
		Command = EM_PG_Houses_LtoC_ModifyFriendDBID;
	}
};


//////////////////////////////////////////////////////////////////////////
//種植
//////////////////////////////////////////////////////////////////////////
//清空盆栽
//Client -> Local
struct PG_Houses_CtoL_PlantClearRequest
{
	GamePGCommandEnum	Command;
	int					HousePos;		

	PG_Houses_CtoL_PlantClearRequest()
	{
		Command = EM_PG_Houses_CtoL_PlantClearRequest;
	}
};
//------------------------------------------------------------------------
//清空盆栽結果
//Local -> Client 
enum PlantClearResultTypeENUM
{
	EM_PlantClearResultType_OK		,
	EM_PlantClearResultType_Failed	,
};
struct PG_Houses_LtoC_PlantClearResult
{
	GamePGCommandEnum	Command;
	int							HousePos;
	PlantClearResultTypeENUM	Result;

	PG_Houses_LtoC_PlantClearResult()
	{
		Command = EM_PG_Houses_LtoC_PlantClearResult;
	}
};
//------------------------------------------------------------------------
enum PlantItemTypeENUM
{
	EM_PlantItem_Seed		,
	EM_PlantItem_Water		,
	EM_PlantItem_Topdress	,
	EM_PlantItem_MagicWater	,
};
//把東西放到盆子裡(種子 肥料 水 )
//Client -> Local
struct PG_Houses_CtoL_PlantItemRequest
{
	GamePGCommandEnum	Command;
	PlantItemTypeENUM	Type;
	int					PotHousePos;		//盆子位置	
	int					ItemBodyPos;		//身體位置

	PG_Houses_CtoL_PlantItemRequest()
	{
		Command = EM_PG_Houses_CtoL_PlantItemRequest;
	}
};
//------------------------------------------------------------------------
//把東西放到盆子裡結果
//Local -> Client 
enum PlantItemResultTypeENUM
{
	EM_PlantItemResultType_OK						,
	EM_PlantItemResultType_Failed					,
	EM_PlantItemResultType_Failed_HouseNotFind		,
	EM_PlantItemResultType_Failed_PotErr			,
	EM_PlantItemResultType_Failed_HouseItemNotFind	,
	EM_PlantItemResultType_Failed_BodyItemErr		,
	EM_PlantItemResultType_Failed_SkillLvErr		,
	EM_PlantItemResultType_Failed_GrowRateErr		,	//已經成長完成 不可灌溉
	EM_PlantItemResultType_Failed_PlantCount		,	//種植超過上限量
	EM_PlantItemResultType_Failed_ItemNoUse			,	//澆水施肥或魔法灌溉已滿
};
struct PG_Houses_LtoC_PlantItemResult
{
	GamePGCommandEnum	Command;
	PlantItemTypeENUM	Type;
	int					PotHousePos;		//盆子位置	
	int					ItemBodyPos;		//身體位置

	PlantItemResultTypeENUM	Result;

	PG_Houses_LtoC_PlantItemResult()
	{
		Command = EM_PG_Houses_LtoC_PlantItemResult;
	}
};
//------------------------------------------------------------------------
//把盆哉狀態鎖定
//Client -> Local
struct PG_Houses_CtoL_PlantLockRequest
{
	GamePGCommandEnum	Command;
	int					PotHousePos;		//盆子位置	
	PG_Houses_CtoL_PlantLockRequest()
	{
		Command = EM_PG_Houses_CtoL_PlantLockRequest;
	}
};
//------------------------------------------------------------------------
//把東西放到盆子裡結果
//Local -> Client 
enum PlantLockResultTypeENUM
{
	EM_PlantLockResultType_OK		,
	EM_PlantLockResultType_Failed	,
};
struct PG_Houses_LtoC_PlantLockResult
{
	GamePGCommandEnum	Command;
	int					PotHousePos;		//盆子位置	
	PlantLockResultTypeENUM	Result;

	PG_Houses_LtoC_PlantLockResult()
	{
		Command = EM_PG_Houses_LtoC_PlantLockResult;
	}
};
//------------------------------------------------------------------------
//把盆哉收成
//Client -> Local
struct PG_Houses_CtoL_PlantGetProductRequest
{
	GamePGCommandEnum	Command;
	int					PotHousePos;		//盆子位置	
	PG_Houses_CtoL_PlantGetProductRequest()
	{
		Command = EM_PG_Houses_CtoL_PlantGetProductRequest;
	}
};
//------------------------------------------------------------------------
//把東西放到盆子裡結果
//Local -> Client 
enum PlantGetProductResultTypeENUM
{
	EM_PlantGetProductResultType_OK			,
	EM_PlantGetProductResultType_Failed		,
	EM_PlantGetProductResultType_NoFruits	,	//沒收成
};
struct PG_Houses_LtoC_PlantGetProductResult
{
	GamePGCommandEnum	Command;
	int					PotHousePos;		//盆子位置	
	PlantGetProductResultTypeENUM	Result;

	PG_Houses_LtoC_PlantGetProductResult()
	{
		Command = EM_PG_Houses_LtoC_PlantGetProductResult;
	}
};
//------------------------------------------------------------------------
//Local -> Client
//好友存取物品的log
struct PG_Houses_LtoC_FriendItemLog
{
	GamePGCommandEnum	Command;

	HouseFriendItemLogStruct Log;

	PG_Houses_LtoC_FriendItemLog()
	{
		Command = EM_PG_Houses_LtoC_FriendItemLog;
	}
};
//------------------------------------------------------------------------
//Client -> Local
//要求清除Log
struct PG_Houses_CtoL_ClearItemLogRequest
{
	GamePGCommandEnum	Command;

	PG_Houses_CtoL_ClearItemLogRequest()
	{
		Command = EM_PG_Houses_CtoL_ClearItemLogRequest;
	}
};
//------------------------------------------------------------------------
//Local -> Client
struct PG_Houses_LtoC_ClearItemLogResult
{
	GamePGCommandEnum	Command;
	bool				Result;
	PG_Houses_LtoC_ClearItemLogResult()
	{
		Command = EM_PG_Houses_LtoC_ClearItemLogResult;
	}
};
//////////////////////////////////////////////////////////////////////////
// 女僕封包
//////////////////////////////////////////////////////////////////////////
//要求招募列表
//Client->Local
struct PG_Houses_CtoL_ServantHireListRequest
{
	GamePGCommandEnum	Command;


	PG_Houses_CtoL_ServantHireListRequest()
	{Command = EM_PG_Houses_CtoL_ServantHireListRequest;}
};
//////////////////////////////////////////////////////////////////////////
//招募列表回應
//Local->Client
struct PG_Houses_LtoC_ServantHireList
{
	GamePGCommandEnum	Command;
	int		ID;
	bool	End;
	HouseServantStruct Servant;	

	PG_Houses_LtoC_ServantHireList()
	{Command = EM_PG_Houses_LtoC_ServantHireList;}
};
//////////////////////////////////////////////////////////////////////////
//招募女僕
//Client->Local
struct PG_Houses_CtoL_ServantHireRequest
{
	GamePGCommandEnum	Command;
	int ID;
	int	Pos;	//女僕要放的位置

	PG_Houses_CtoL_ServantHireRequest()
	{Command = EM_PG_Houses_CtoL_ServantHireRequest;}
};
//////////////////////////////////////////////////////////////////////////
//招募女僕結果
//Local->Client
struct PG_Houses_LtoC_ServantHireResult
{
	GamePGCommandEnum	Command;
	int	Pos;
	int ID;
	HouseServantStruct Servant;	
	bool Result;

	PG_Houses_LtoC_ServantHireResult()
	{Command = EM_PG_Houses_LtoC_ServantHireResult;}
};
//////////////////////////////////////////////////////////////////////////
//要求刪除女僕
//Client->Local
struct PG_Houses_CtoL_ServantDelRequest
{
	GamePGCommandEnum	Command;
	int ServantID;

	PG_Houses_CtoL_ServantDelRequest()
	{Command = EM_PG_Houses_CtoL_ServantDelRequest;}
};
//////////////////////////////////////////////////////////////////////////
//女僕刪除結果
//Local->Client
struct PG_Houses_LtoC_ServantDelResult
{
	GamePGCommandEnum	Command;
	int		ServantID;
	bool	Result;	

	PG_Houses_LtoC_ServantDelResult()
	{Command = EM_PG_Houses_LtoC_ServantDelResult;}
};
//////////////////////////////////////////////////////////////////////////
//女僕互動事件
//Client->Local
struct PG_Houses_CtoL_ServantEventRequest
{
	GamePGCommandEnum	Command;
	int ServantID;
	int	EventType1;
	int EventType2;

	PG_Houses_CtoL_ServantEventRequest()
	{Command = EM_PG_Houses_CtoL_ServantEventRequest;}
};
//////////////////////////////////////////////////////////////////////////
//女僕互動事件通知
//Local->Client
struct PG_Houses_LtoC_ServantEvent
{
	GamePGCommandEnum	Command;
	int		ServantID;
	int		EventID;

	PG_Houses_LtoC_ServantEvent()
	{Command = EM_PG_Houses_LtoC_ServantEvent;}
};
//////////////////////////////////////////////////////////////////////////
//女僕擺設
//Client->Local
struct PG_Houses_CtoL_ServantLayout
{
	GamePGCommandEnum	Command;
	int		ServantID;
	House3DLayoutStruct	Layout;

	PG_Houses_CtoL_ServantLayout()
	{Command = EM_PG_Houses_CtoL_ServantLayout;}
};
//////////////////////////////////////////////////////////////////////////
//女僕命名
//Client->Local
struct PG_Houses_CtoL_ServantRenameRequest
{
	GamePGCommandEnum	Command;
	StaticString< _MAX_NAMERECORD_SIZE_ >	Name;
	int	ServantID;

	PG_Houses_CtoL_ServantRenameRequest()
	{Command = EM_PG_Houses_CtoL_ServantRenameRequest;}
};
//////////////////////////////////////////////////////////////////////////
//女僕命名結果
//Local->Client
struct PG_Houses_LtoC_ServantRenameResult
{
	GamePGCommandEnum	Command;
	StaticString< _MAX_NAMERECORD_SIZE_ >	Name;
	bool Result;

	PG_Houses_LtoC_ServantRenameResult()
	{Command = EM_PG_Houses_LtoC_ServantRenameResult;}
};
//////////////////////////////////////////////////////////////////////////
//修正女僕數值
//Local -> Client
struct PG_Houses_LtoC_FixServantValue
{
	GamePGCommandEnum	Command;
	int							ServantID;
	HouseServantValueTypeENUM	Type;
	int							Value;
	PG_Houses_LtoC_FixServantValue()
	{Command = EM_PG_Houses_LtoC_FixServantValue;}
};

//////////////////////////////////////////////////////////////////////////