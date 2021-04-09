#pragma once
#include "RoleData\RoleDataEx.h"
#include "NetWakerPGEnum.h"
//////////////////////////////////////////////////////////////////////////
//公會屋建立
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter 要求建立公會屋
struct PG_GuildHouses_LtoD_BuildHouseRequest
{
	GamePGCommandEnum	Command;
	int		PlayerDBID;
	int		GuildID;

	PG_GuildHouses_LtoD_BuildHouseRequest()
	{
		Command = EM_PG_GuildHouses_LtoD_BuildHouseRequest;
	}
};
//-------------------------------------------------------------------------
//DataCenter -> Local 建立公會屋結果
struct PG_GuildHouses_DtoL_BuildHouseResult
{
	GamePGCommandEnum	Command;
	int		PlayerDBID;
	int		GuildID;
	bool	Result;

	PG_GuildHouses_DtoL_BuildHouseResult()
	{
		Command = EM_PG_GuildHouses_DtoL_BuildHouseResult;
	}
};
//-------------------------------------------------------------------------
//Local -> Client 建立公會屋結果
struct PG_GuildHouses_LtoC_BuildHouseResult
{
	GamePGCommandEnum	Command;
	int		GuildID;
	bool	Result;

	PG_GuildHouses_LtoC_BuildHouseResult()
	{
		Command = EM_PG_GuildHouses_LtoC_BuildHouseResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//設定倉庫權限
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local 
//公會倉庫權限設定
struct PG_GuildHouses_DtoL_StoreConfig
{
	GamePGCommandEnum	Command;

	GuildStorehouseRightStruct	Config;

	PG_GuildHouses_DtoL_StoreConfig()
	{
		Command = EM_PG_GuildHouses_DtoL_StoreConfig;
	}
};
//-------------------------------------------------------------------------
//Local -> Client
//公會倉庫權限設定
struct PG_GuildHouses_LtoC_StoreConfig
{
	GamePGCommandEnum	Command;

	GuildStorehouseRightStruct	Config;

	PG_GuildHouses_LtoC_StoreConfig()
	{
		Command = EM_PG_GuildHouses_LtoC_StoreConfig;
	}
};
//-------------------------------------------------------------------------
//Client -> Local
//公會倉庫權限設定
struct PG_GuildHouses_CtoL_SetStoreConfig
{
	GamePGCommandEnum	Command;

	GuildStorehouseRightStruct	Config;

	PG_GuildHouses_CtoL_SetStoreConfig()
	{
		Command = EM_PG_GuildHouses_CtoL_SetStoreConfig;
	}
};
//-------------------------------------------------------------------------
//Client -> Local
//公會倉庫權限設定
struct PG_GuildHouses_LtoC_SetStoreConfigResult
{
	GamePGCommandEnum	Command;
	bool	Result;

	PG_GuildHouses_LtoC_SetStoreConfigResult()
	{
		Command = EM_PG_GuildHouses_LtoC_SetStoreConfigResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//進入公會屋
//////////////////////////////////////////////////////////////////////////
//Local -> Client 要求開啟參觀公會屋的介面
struct PG_GuildHouses_LtoC_OpenVisitHouse
{
	GamePGCommandEnum	Command;

	int					TargetNpcID;
	bool				IsVisitMyGuildHouse;

	PG_GuildHouses_LtoC_OpenVisitHouse()
	{
		Command = EM_PG_GuildHouses_LtoC_OpenVisitHouse;
	}
};
//-------------------------------------------------------------------------
//Client -> Local 要求參觀某公會房子
struct PG_GuildHouses_CtoL_VisitHouseRequest
{
	GamePGCommandEnum	Command;
	int		GuildID;

	PG_GuildHouses_CtoL_VisitHouseRequest()
	{
		Command = EM_PG_GuildHouses_CtoL_VisitHouseRequest;
	}
};
//-------------------------------------------------------------------------
//Local -> DataCenter 查尋此公會屋是否存在
struct PG_GuildHouses_LtoD_FindHouseRequest
{
	GamePGCommandEnum	Command;
	int			PlayerDBID;		//要求的玩家
	int			GuildID;		//參觀房間號碼


	PG_GuildHouses_LtoD_FindHouseRequest()
	{
		Command = EM_PG_GuildHouses_LtoD_FindHouseRequest;
	}
};
//-------------------------------------------------------------------------
//DataCenter -> Local 查尋此公會屋是否存在 結果
struct PG_GuildHouses_DtoL_FindHouseResult
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;	//要求的玩家
	int					GuildID;
	bool				Result;

	PG_GuildHouses_DtoL_FindHouseResult()
	{
		Command = EM_PG_GuildHouses_DtoL_FindHouseResult;
	}
};
//-------------------------------------------------------------------------
//Local -> Client 查尋此公會屋是否存在 結果
struct PG_GuildHouses_LtoC_FindHouseResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_GuildHouses_LtoC_FindHouseResult()
	{
		Command = EM_PG_GuildHouses_LtoC_FindHouseResult;
	}
};
//-------------------------------------------------------------------------
//Client -> Local 關閉參觀公會屋的介面
struct PG_GuildHouses_CtoL_CloseVisitHouse
{
	GamePGCommandEnum	Command;
	PG_GuildHouses_CtoL_CloseVisitHouse()
	{
		Command = EM_PG_GuildHouses_CtoL_CloseVisitHouse;
	}
};
//-------------------------------------------------------------------------
//Local -> Client 關閉參觀公會屋的介面
struct PG_GuildHouses_LtoC_CloseVisitHouse
{
	GamePGCommandEnum	Command;
	PG_GuildHouses_LtoC_CloseVisitHouse()
	{
		Command = EM_PG_GuildHouses_LtoC_CloseVisitHouse;
	}
};
//-------------------------------------------------------------------------
//Local -> Client 等待載入公會屋資料
struct PG_GuildHouses_LtoC_HouseInfoLoading
{
	GamePGCommandEnum	Command;
	int					GuildID;
	PG_GuildHouses_LtoC_HouseInfoLoading()
	{
		Command = EM_PG_GuildHouses_LtoC_HouseInfoLoading;
	}
};
//-------------------------------------------------------------------------
//Local -> DataCenter 要求載入公會屋
struct PG_GuildHouses_LtoD_HouseInfoLoading
{
	GamePGCommandEnum	Command;
	int					GuildID;
	PG_GuildHouses_LtoD_HouseInfoLoading()
	{
		Command = EM_PG_GuildHouses_LtoD_HouseInfoLoading;
	}
};
//-------------------------------------------------------------------------
//DataCenter -> Local 公會屋資料資料
struct PG_GuildHouses_DtoL_HouseBaseInfo
{
	GamePGCommandEnum	Command;
	GuildHousesInfoStruct	HouseBaseInfo;

	PG_GuildHouses_DtoL_HouseBaseInfo()
	{
		Command = EM_PG_GuildHouses_DtoL_HouseBaseInfo;
	}
};
//-------------------------------------------------------------------------
//DataCenter -> Local 公會倉庫物品資料
struct PG_GuildHouses_DtoL_ItemInfo
{
	GamePGCommandEnum	Command;
	int		GuildID;
	int		PageID;
	int		ItemCount;
	GuildHouseItemStruct Item[ _DEF_GUIDHOUSEITEM_MAX_PAGEITEM_ ];		

	PG_GuildHouses_DtoL_ItemInfo()
	{
		Command = EM_PG_GuildHouses_DtoL_ItemInfo;
	}
};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//DataCenter -> Local 公會建築物資料
struct PG_GuildHouses_DtoL_BuildingInfo
{
	GamePGCommandEnum	Command;

	GuildHouseBuildingInfoStruct	Building;

	PG_GuildHouses_DtoL_BuildingInfo()
	{
		Command = EM_PG_GuildHouses_DtoL_BuildingInfo;
	}
};
//-------------------------------------------------------------------------
//DataCenter -> Local 通知公會屋讀取完畢			
struct PG_GuildHouses_DtoL_HouseInfoLoadOK
{
	GamePGCommandEnum	Command;
	int		GuildID;

	PG_GuildHouses_DtoL_HouseInfoLoadOK()
	{
		Command = EM_PG_GuildHouses_DtoL_HouseInfoLoadOK;
	}
};
//-------------------------------------------------------------------------
//Local -> Client	  公會屋載入完成
struct PG_GuildHouses_LtoC_HouseInfoLoadOK
{
	GamePGCommandEnum	Command;

	PG_GuildHouses_LtoC_HouseInfoLoadOK()
	{
		Command = EM_PG_GuildHouses_LtoC_HouseInfoLoadOK;
	}
};
//-------------------------------------------------------------------------
//Client -> Local	要求某人離開公會屋
struct PG_GuildHouses_CtoL_LeaveHouse
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;

	PG_GuildHouses_CtoL_LeaveHouse()
	{
		Command = EM_PG_GuildHouses_CtoL_LeaveHouse;
	}
};
//////////////////////////////////////////////////////////////////////////
//公會建設
//////////////////////////////////////////////////////////////////////////
//建立公會建築
//Client -> Local
struct PG_GuildHouses_CtoL_CreateBuildingRequest
{
	GamePGCommandEnum	Command;
	char				Point[32];	//參考點
	int					BuildingID;
	int					ParentBuildingDBID;
	float				Dir;		//方向

	PG_GuildHouses_CtoL_CreateBuildingRequest()
	{
		Command = EM_PG_GuildHouses_CtoL_CreateBuildingRequest;
	}
};
//-------------------------------------------------------------------------
//建立公會建築 (結果)
// Local -> Client 
struct PG_GuildHouses_LtoC_CreateBuildingResult
{
	GamePGCommandEnum	Command;
	char				Point[32];	//參考點
	int					BuildingID;
	int					ParentBuildingDBID;
	INT					BuildingDBID;		//	
	bool				Result;

	PG_GuildHouses_LtoC_CreateBuildingResult()
	{
		Command = EM_PG_GuildHouses_LtoC_CreateBuildingResult;
	}
};
//-------------------------------------------------------------------------
///建立公會建築 升級
//Client -> Local
struct PG_GuildHouses_CtoL_BuildingUpgradeRequest
{
	GamePGCommandEnum	Command;
	int					BuildingDBID;
	int					UpgradeBuildingID;

	PG_GuildHouses_CtoL_BuildingUpgradeRequest()
	{
		Command = EM_PG_GuildHouses_CtoL_BuildingUpgradeRequest;
	}
};
//-------------------------------------------------------------------------
//建立公會建築 升級(結果)
// Local -> Client 
struct PG_GuildHouses_LtoC_BuildingUpgradeResult
{
	GamePGCommandEnum	Command;
	int					GItemID;
	int					BuildingDBID;
	int					UpgradeBuildingID;
	bool				Result;

	PG_GuildHouses_LtoC_BuildingUpgradeResult()
	{
		Command = EM_PG_GuildHouses_LtoC_BuildingUpgradeResult;
	}
};
//-------------------------------------------------------------------------
///公會建設刪除
//Client -> Local
struct PG_GuildHouses_CtoL_DeleteBuildingRequest
{
	GamePGCommandEnum	Command;
	int					BuildingDBID;

	PG_GuildHouses_CtoL_DeleteBuildingRequest()
	{
		Command = EM_PG_GuildHouses_CtoL_DeleteBuildingRequest;
	}
};
//-------------------------------------------------------------------------
//建立公會建築 升級(結果)
// Local -> Client 
struct PG_GuildHouses_LtoC_DeleteBuildingResult
{
	GamePGCommandEnum	Command;
	int					BuildingDBID;
	bool				Result;

	PG_GuildHouses_LtoC_DeleteBuildingResult()
	{
		Command = EM_PG_GuildHouses_LtoC_DeleteBuildingResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//公會倉庫
//////////////////////////////////////////////////////////////////////////
//Local -> Client 要求公會倉庫資料
struct PG_GuildHouses_CtoL_HouseItemRequest
{
	GamePGCommandEnum	Command;
	int					PageID;

	PG_GuildHouses_CtoL_HouseItemRequest()
	{
		Command = EM_PG_GuildHouses_CtoL_HouseItemRequest;
	}
};
//------------------------------------------------------------------------
//Client -> Local 關閉公會倉庫
struct PG_GuildHouses_LtoC_HouseItemInfo
{
	GamePGCommandEnum		Command;
	int						PageID;
	int						MaxPage;
	int						GetCount;
	int						ItemCount;
	GuildHouseItemStruct	Item[ 100 ];

	PG_GuildHouses_LtoC_HouseItemInfo()
	{
		Command = EM_PG_GuildHouses_LtoC_HouseItemInfo;
	}

	int	Size()
	{
		return sizeof(*this) - sizeof(Item) + sizeof(GuildHouseItemStruct)* ItemCount;
	}
};

//------------------------------------------------------------------------
//Client -> Local 要求放置物品 or 取出物品
struct PG_GuildHouses_CtoL_SwapBodyHouseItem
{
	GamePGCommandEnum	Command;
	int		BodyPos;
	int		HouseItemPageID;;
	int		HouseItemPos;
	bool	IsFromBody;

	PG_GuildHouses_CtoL_SwapBodyHouseItem()
	{
		Command = EM_PG_GuildHouses_CtoL_SwapBodyHouseItem;
	}
};
//------------------------------------------------------------------------
//Local -> Client 通知放置結果
struct PG_GuildHouses_LtoC_SwapBodyHouseItemResult
{
	GamePGCommandEnum	Command;
	int		BodyPos;
	int		HouseItemPageID;
	int		HouseItemPos;
	int		GetCount;		//-1 代表未知
	bool	Result;

	PG_GuildHouses_LtoC_SwapBodyHouseItemResult()
	{
		Command = EM_PG_GuildHouses_LtoC_SwapBodyHouseItemResult;
	}
};
//------------------------------------------------------------------------
//Local -> Client 修正封包Client物品資料
struct PG_GuildHouses_LtoC_FixHouseItem
{
	GamePGCommandEnum		Command;
	GuildHouseItemStruct	Item;

	PG_GuildHouses_LtoC_FixHouseItem()
	{
		Command = EM_PG_GuildHouses_LtoC_FixHouseItem;
	}
};
//------------------------------------------------------------------------
//Client -> Local 要求物品搬移
struct PG_GuildHouses_CtoL_SwapHouseItemRequest
{
	GamePGCommandEnum	Command;
	int			HouseItemPos[2];		
	int			HouseItemPageID[2];

	PG_GuildHouses_CtoL_SwapHouseItemRequest()
	{
		Command = EM_PG_GuildHouses_CtoL_SwapHouseItemRequest;
	}
};
//------------------------------------------------------------------------
//Local -> Client 通知搬移結果
struct PG_GuildHouses_LtoC_SwapHouseItemResult
{
	GamePGCommandEnum	Command;	
	int			HouseItemPos[2];		
	int			HouseItemPageID[2];
	bool		Result;

	PG_GuildHouses_LtoC_SwapHouseItemResult()
	{
		Command = EM_PG_GuildHouses_LtoC_SwapHouseItemResult;
	}
};



//////////////////////////////////////////////////////////////////////////
//Client 要求公會相關資料
//////////////////////////////////////////////////////////////////////////
//Client -> Local 要求所有公會屋的建築資訊
struct PG_GuildHouses_CtoL_AllBuildingInfoRequest
{
	GamePGCommandEnum	Command;
	int					GuildID;

	PG_GuildHouses_CtoL_AllBuildingInfoRequest()
	{
		Command = EM_PG_GuildHouses_CtoL_AllBuildingInfoRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
#define _MAX_GUILD_BUILDING_COUNT_ 500
struct GuildBuildingObjectInfoStruct
{
	int					GItemID;
	int					BuildingParentDBID;
	int					BuildingDBID;
	int					BuildingID;
	int					Dir;
	char				PointStr[32];
};
//Local -> Client要求所有公會屋的建築資訊
struct PG_GuildHouses_LtoC_AllBuildingInfo
{
	GamePGCommandEnum	Command;
	int					GuildID;
	int					Count;
	GuildBuildingObjectInfoStruct Building[_MAX_GUILD_BUILDING_COUNT_];

	PG_GuildHouses_LtoC_AllBuildingInfo()
	{
		Command = EM_PG_GuildHouses_LtoC_AllBuildingInfo;
	}
	int PGSize()
	{
		return sizeof(*this)- sizeof(Building)+ sizeof(GuildBuildingObjectInfoStruct)*Count;
	}
};
//////////////////////////////////////////////////////////////////////////
//新增公會屋物件
// Local -> Client
struct PG_GuildHouses_LtoC_NewBuildingInfo
{
	GamePGCommandEnum	Command;
	GuildBuildingObjectInfoStruct Building;

	PG_GuildHouses_LtoC_NewBuildingInfo()
	{
		Command = EM_PG_GuildHouses_LtoC_NewBuildingInfo;
	}
};
//////////////////////////////////////////////////////////////////////////
//移除公會屋物件
// Local -> Client
struct PG_GuildHouses_LtoC_DelBuildingInfo
{
	GamePGCommandEnum	Command;
	int					BuildingDBID;

	PG_GuildHouses_LtoC_DelBuildingInfo()
	{
		Command = EM_PG_GuildHouses_LtoC_DelBuildingInfo;
	}
};
//////////////////////////////////////////////////////////////////////////
//公會裝飾品倉庫
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local 飾品倉庫物品資料
struct PG_GuildHouses_DtoL_FurnitureItemInfo
{
	GamePGCommandEnum	Command;

	GuildHouseFurnitureItemStruct Item;

	PG_GuildHouses_DtoL_FurnitureItemInfo()
	{
		Command = EM_PG_GuildHouses_DtoL_FurnitureItemInfo;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client 飾品倉庫物品資料
struct PG_GuildHouses_LtoC_FurnitureItemInfo
{
	GamePGCommandEnum	Command;

	GuildHouseFurnitureItemStruct Item;

	PG_GuildHouses_LtoC_FurnitureItemInfo()
	{
		Command = EM_PG_GuildHouses_LtoC_FurnitureItemInfo;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 要求放置物品 or 取出物品
struct PG_GuildHouses_CtoL_SwapBodyHouseFurniture
{
	GamePGCommandEnum	Command;
	int		BodyPos;
	int		HousePos;

	PG_GuildHouses_CtoL_SwapBodyHouseFurniture()
	{
		Command = EM_PG_GuildHouses_CtoL_SwapBodyHouseFurniture;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client 通知放置結果
struct PG_GuildHouses_LtoC_SwapBodyHouseFurnitureResult
{
	GamePGCommandEnum	Command;
	int		BodyPos;
	int		HousePos;			
	bool	Result;

	PG_GuildHouses_LtoC_SwapBodyHouseFurnitureResult()
	{
		Command = EM_PG_GuildHouses_LtoC_SwapBodyHouseFurnitureResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client 修正封包Client物品資料
struct PG_GuildHouses_LtoC_FixHouseFurniture
{
	GamePGCommandEnum	Command;
	GuildHouseFurnitureItemStruct	Item;

	PG_GuildHouses_LtoC_FixHouseFurniture()
	{
		Command = EM_PG_GuildHouses_LtoC_FixHouseFurniture;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 要求物品搬移
struct PG_GuildHouses_CtoL_SwapHouseFurnitureRequest
{
	GamePGCommandEnum	Command;
	int			Pos[2];		//高位元 母物件位置   低位元 目前位置

	PG_GuildHouses_CtoL_SwapHouseFurnitureRequest()
	{
		Command = EM_PG_GuildHouses_CtoL_SwapHouseFurnitureRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client 通知搬移結果
struct PG_GuildHouses_LtoC_SwapHouseFurnitureResult
{
	GamePGCommandEnum	Command;	
	int			Pos[2];		//高位元 母物件位置   低位元 目前位置	
	bool		Result;

	PG_GuildHouses_LtoC_SwapHouseFurnitureResult()
	{
		Command = EM_PG_GuildHouses_LtoC_SwapHouseFurnitureResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//物品擺設
//////////////////////////////////////////////////////////////////////////
//Client -> Local 擺設
struct PG_GuildHouses_CtoL_HouseItemLayoutRequest
{
	GamePGCommandEnum	Command;
	int		BuildingObjID;
	int		Pos;
	House3DLayoutStruct	Layout;

	PG_GuildHouses_CtoL_HouseItemLayoutRequest()
	{
		Command = EM_PG_GuildHouses_CtoL_HouseItemLayoutRequest;
	}
};
//------------------------------------------------------------------------
//Local -> Client 擺設結果
struct PG_GuildHouses_LtoC_HouseItemLayoutResult
{
	GamePGCommandEnum	Command;
	int		Pos;
	bool	Result;

	PG_GuildHouses_LtoC_HouseItemLayoutResult()
	{
		Command = EM_PG_GuildHouses_LtoC_HouseItemLayoutResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//公會物品Log
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local
//公會Log歷史資料
struct PG_GuildHouses_DtoL_ItemLog
{
	GamePGCommandEnum	Command;
	GuildItemStoreLogStruct	ItemLog;
	PG_GuildHouses_DtoL_ItemLog()
	{
		Command = EM_PG_GuildHouses_DtoL_ItemLog;
	}
};
//------------------------------------------------------------------------
//Client -> Local
//要求公會的歷史資料
struct PG_GuildHouses_CtoL_ItemLogRequest
{
	GamePGCommandEnum	Command;
	int		Day;
	int		PageID;
	PG_GuildHouses_CtoL_ItemLogRequest()
	{
		Command = EM_PG_GuildHouses_CtoL_ItemLogRequest;
	}
};
//------------------------------------------------------------------------
//Local -> Client
//公會的歷史資料
struct PG_GuildHouses_LtoC_ItemLogList
{
	GamePGCommandEnum	Command;
	int					Count;
	GuildItemStoreLogStruct	Log[1000];
	PG_GuildHouses_LtoC_ItemLogList()
	{
		Command = EM_PG_GuildHouses_LtoC_ItemLogList;
	}
	int Size()
	{
		return sizeof( int )*2 +sizeof(GuildItemStoreLogStruct)* Count;
	}
};
//------------------------------------------------------------------------
// Local -> Client
//公會Log歷史資料
struct PG_GuildHouses_LtoC_ItemLog
{
	GamePGCommandEnum	Command;
	GuildItemStoreLogStruct	ItemLog;
	PG_GuildHouses_LtoC_ItemLog()
	{
		Command = EM_PG_GuildHouses_LtoC_ItemLog;
	}
};

//------------------------------------------------------------------------
//Client -> Server 公會建築開啟or關閉使用
struct PG_GuildHouses_CtoL_ActiveBuildingRequest
{
	GamePGCommandEnum	Command;
	int					BuildDBID;
	bool				IsActive;

	PG_GuildHouses_CtoL_ActiveBuildingRequest()
	{
		Command = EM_PG_GuildHouses_CtoL_ActiveBuildingRequest;
	}
};
//------------------------------------------------------------------------
//Server -> Client 公會建築開啟or關閉使用
struct PG_GuildHouses_LtoC_ActiveBuildingResult
{
	GamePGCommandEnum	Command;
	int					BuildDBID;
	bool				IsActive;
	bool				Result;

	PG_GuildHouses_LtoC_ActiveBuildingResult()
	{
		Command = EM_PG_GuildHouses_LtoC_ActiveBuildingResult;
	}
};
//------------------------------------------------------------------------
//Server -> Client 公會建築開啟狀態
struct PG_GuildHouses_LtoC_BuildingActiveType
{
	GamePGCommandEnum	Command;
	int					BuildDBID;
	bool				IsActive;

	PG_GuildHouses_LtoC_BuildingActiveType()
	{
		Command = EM_PG_GuildHouses_LtoC_BuildingActiveType;
	}
};
//------------------------------------------------------------------------
//Client -> Server 要求購買公會物品放置頁
struct PG_GuildHouses_CtoL_BuyItemPageRequest
{
	GamePGCommandEnum	Command;
	int			PageID;

	PG_GuildHouses_CtoL_BuyItemPageRequest()
	{
		Command = EM_PG_GuildHouses_CtoL_BuyItemPageRequest;
	}
};
//------------------------------------------------------------------------
//Server -> Client 要求購買公會物品放置頁
struct PG_GuildHouses_LtoC_BuyItemPageResult
{
	GamePGCommandEnum	Command;
	int			PageID;
	bool		Result;

	PG_GuildHouses_LtoC_BuyItemPageResult()
	{
		Command = EM_PG_GuildHouses_LtoC_BuyItemPageResult;
	}
};
//------------------------------------------------------------------------
//Client -> Server 要求把某家俱寄回給擁有者
struct PG_GuildHouses_CtoL_FurnitureSendBack
{
	GamePGCommandEnum	Command;
	int					Pos;

	PG_GuildHouses_CtoL_FurnitureSendBack()
	{
		Command = EM_PG_GuildHouses_CtoL_FurnitureSendBack;
	}
};
//------------------------------------------------------------------------
//Server -> Client 要求把某家俱寄回給擁有者結果
struct PG_GuildHouses_LtoC_FurnitureSendBackResult
{
	GamePGCommandEnum	Command;
	int					Pos;
	bool				Result;

	PG_GuildHouses_LtoC_FurnitureSendBackResult()
	{
		Command = EM_PG_GuildHouses_LtoC_FurnitureSendBackResult;
	}
};
//------------------------------------------------------------------------
//Client -> Server 要求購買房屋樣式
struct PG_GuildHouses_CtoL_BuyHouseStyle
{
	GamePGCommandEnum	Command;
	int					Type;

	PG_GuildHouses_CtoL_BuyHouseStyle()
	{
		Command = EM_PG_GuildHouses_CtoL_BuyHouseStyle;
	}
};
//------------------------------------------------------------------------
//Server -> Client 購買結果
struct PG_GuildHouses_LtoC_BuyHouseStyleResult
{
	GamePGCommandEnum	Command;
	int					Type;
	bool				Result;

	PG_GuildHouses_LtoC_BuyHouseStyleResult()
	{
		Command = EM_PG_GuildHouses_LtoC_BuyHouseStyleResult;
	}
};
//------------------------------------------------------------------------
//Client -> Server 設定樣式
struct PG_GuildHouses_CtoL_SetHouseStyle
{
	GamePGCommandEnum	Command;
	int					Type;

	PG_GuildHouses_CtoL_SetHouseStyle()
	{
		Command = EM_PG_GuildHouses_CtoL_SetHouseStyle;
	}
};
//------------------------------------------------------------------------
//Server -> Client 購買結果
struct PG_GuildHouses_LtoC_SetHouseStyleResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_GuildHouses_LtoC_SetHouseStyleResult()
	{
		Command = EM_PG_GuildHouses_LtoC_SetHouseStyleResult;
	}
};
//------------------------------------------------------------------------
//Client -> Server 目前房屋樣式要求
struct PG_GuildHouses_CtoL_HouseStyleRequest
{
	GamePGCommandEnum	Command;

	PG_GuildHouses_CtoL_HouseStyleRequest()
	{
		Command = EM_PG_GuildHouses_CtoL_HouseStyleRequest;
	}
};
//------------------------------------------------------------------------
//Server -> Client 目前房屋樣式
struct PG_GuildHouses_LtoC_HouseStyle
{
	GamePGCommandEnum	Command;

	int					Type;	

	PG_GuildHouses_LtoC_HouseStyle()
	{
		Command = EM_PG_GuildHouses_LtoC_HouseStyle;
	}
};