#pragma once
#include "RoleData\RoleDataEx.h"
#include "NetWakerPGEnum.h"
//////////////////////////////////////////////////////////////////////////
//���|�Ϋإ�
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter �n�D�إߤ��|��
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
//DataCenter -> Local �إߤ��|�ε��G
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
//Local -> Client �إߤ��|�ε��G
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
//�]�w�ܮw�v��
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local 
//���|�ܮw�v���]�w
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
//���|�ܮw�v���]�w
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
//���|�ܮw�v���]�w
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
//���|�ܮw�v���]�w
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
//�i�J���|��
//////////////////////////////////////////////////////////////////////////
//Local -> Client �n�D�}�Ұ��[���|�Ϊ�����
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
//Client -> Local �n�D���[�Y���|�Фl
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
//Local -> DataCenter �d�M�����|�άO�_�s�b
struct PG_GuildHouses_LtoD_FindHouseRequest
{
	GamePGCommandEnum	Command;
	int			PlayerDBID;		//�n�D�����a
	int			GuildID;		//���[�ж����X


	PG_GuildHouses_LtoD_FindHouseRequest()
	{
		Command = EM_PG_GuildHouses_LtoD_FindHouseRequest;
	}
};
//-------------------------------------------------------------------------
//DataCenter -> Local �d�M�����|�άO�_�s�b ���G
struct PG_GuildHouses_DtoL_FindHouseResult
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;	//�n�D�����a
	int					GuildID;
	bool				Result;

	PG_GuildHouses_DtoL_FindHouseResult()
	{
		Command = EM_PG_GuildHouses_DtoL_FindHouseResult;
	}
};
//-------------------------------------------------------------------------
//Local -> Client �d�M�����|�άO�_�s�b ���G
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
//Client -> Local �������[���|�Ϊ�����
struct PG_GuildHouses_CtoL_CloseVisitHouse
{
	GamePGCommandEnum	Command;
	PG_GuildHouses_CtoL_CloseVisitHouse()
	{
		Command = EM_PG_GuildHouses_CtoL_CloseVisitHouse;
	}
};
//-------------------------------------------------------------------------
//Local -> Client �������[���|�Ϊ�����
struct PG_GuildHouses_LtoC_CloseVisitHouse
{
	GamePGCommandEnum	Command;
	PG_GuildHouses_LtoC_CloseVisitHouse()
	{
		Command = EM_PG_GuildHouses_LtoC_CloseVisitHouse;
	}
};
//-------------------------------------------------------------------------
//Local -> Client ���ݸ��J���|�θ��
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
//Local -> DataCenter �n�D���J���|��
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
//DataCenter -> Local ���|�θ�Ƹ��
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
//DataCenter -> Local ���|�ܮw���~���
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
//DataCenter -> Local ���|�ؿv�����
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
//DataCenter -> Local �q�����|��Ū������			
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
//Local -> Client	  ���|�θ��J����
struct PG_GuildHouses_LtoC_HouseInfoLoadOK
{
	GamePGCommandEnum	Command;

	PG_GuildHouses_LtoC_HouseInfoLoadOK()
	{
		Command = EM_PG_GuildHouses_LtoC_HouseInfoLoadOK;
	}
};
//-------------------------------------------------------------------------
//Client -> Local	�n�D�Y�H���}���|��
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
//���|�س]
//////////////////////////////////////////////////////////////////////////
//�إߤ��|�ؿv
//Client -> Local
struct PG_GuildHouses_CtoL_CreateBuildingRequest
{
	GamePGCommandEnum	Command;
	char				Point[32];	//�Ѧ��I
	int					BuildingID;
	int					ParentBuildingDBID;
	float				Dir;		//��V

	PG_GuildHouses_CtoL_CreateBuildingRequest()
	{
		Command = EM_PG_GuildHouses_CtoL_CreateBuildingRequest;
	}
};
//-------------------------------------------------------------------------
//�إߤ��|�ؿv (���G)
// Local -> Client 
struct PG_GuildHouses_LtoC_CreateBuildingResult
{
	GamePGCommandEnum	Command;
	char				Point[32];	//�Ѧ��I
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
///�إߤ��|�ؿv �ɯ�
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
//�إߤ��|�ؿv �ɯ�(���G)
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
///���|�س]�R��
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
//�إߤ��|�ؿv �ɯ�(���G)
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
//���|�ܮw
//////////////////////////////////////////////////////////////////////////
//Local -> Client �n�D���|�ܮw���
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
//Client -> Local �������|�ܮw
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
//Client -> Local �n�D��m���~ or ���X���~
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
//Local -> Client �q����m���G
struct PG_GuildHouses_LtoC_SwapBodyHouseItemResult
{
	GamePGCommandEnum	Command;
	int		BodyPos;
	int		HouseItemPageID;
	int		HouseItemPos;
	int		GetCount;		//-1 �N����
	bool	Result;

	PG_GuildHouses_LtoC_SwapBodyHouseItemResult()
	{
		Command = EM_PG_GuildHouses_LtoC_SwapBodyHouseItemResult;
	}
};
//------------------------------------------------------------------------
//Local -> Client �ץ��ʥ]Client���~���
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
//Client -> Local �n�D���~�h��
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
//Local -> Client �q���h�����G
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
//Client �n�D���|�������
//////////////////////////////////////////////////////////////////////////
//Client -> Local �n�D�Ҧ����|�Ϊ��ؿv��T
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
//Local -> Client�n�D�Ҧ����|�Ϊ��ؿv��T
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
//�s�W���|�Ϊ���
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
//�������|�Ϊ���
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
//���|�˹��~�ܮw
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local ���~�ܮw���~���
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
//Local -> Client ���~�ܮw���~���
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
//Client -> Local �n�D��m���~ or ���X���~
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
//Local -> Client �q����m���G
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
//Local -> Client �ץ��ʥ]Client���~���
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
//Client -> Local �n�D���~�h��
struct PG_GuildHouses_CtoL_SwapHouseFurnitureRequest
{
	GamePGCommandEnum	Command;
	int			Pos[2];		//���줸 �������m   �C�줸 �ثe��m

	PG_GuildHouses_CtoL_SwapHouseFurnitureRequest()
	{
		Command = EM_PG_GuildHouses_CtoL_SwapHouseFurnitureRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client �q���h�����G
struct PG_GuildHouses_LtoC_SwapHouseFurnitureResult
{
	GamePGCommandEnum	Command;	
	int			Pos[2];		//���줸 �������m   �C�줸 �ثe��m	
	bool		Result;

	PG_GuildHouses_LtoC_SwapHouseFurnitureResult()
	{
		Command = EM_PG_GuildHouses_LtoC_SwapHouseFurnitureResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//���~�\�]
//////////////////////////////////////////////////////////////////////////
//Client -> Local �\�]
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
//Local -> Client �\�]���G
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
//���|���~Log
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local
//���|Log���v���
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
//�n�D���|�����v���
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
//���|�����v���
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
//���|Log���v���
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
//Client -> Server ���|�ؿv�}��or�����ϥ�
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
//Server -> Client ���|�ؿv�}��or�����ϥ�
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
//Server -> Client ���|�ؿv�}�Ҫ��A
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
//Client -> Server �n�D�ʶR���|���~��m��
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
//Server -> Client �n�D�ʶR���|���~��m��
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
//Client -> Server �n�D��Y�a�ѱH�^���֦���
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
//Server -> Client �n�D��Y�a�ѱH�^���֦��̵��G
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
//Client -> Server �n�D�ʶR�Ыμ˦�
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
//Server -> Client �ʶR���G
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
//Client -> Server �]�w�˦�
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
//Server -> Client �ʶR���G
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
//Client -> Server �ثe�Ыμ˦��n�D
struct PG_GuildHouses_CtoL_HouseStyleRequest
{
	GamePGCommandEnum	Command;

	PG_GuildHouses_CtoL_HouseStyleRequest()
	{
		Command = EM_PG_GuildHouses_CtoL_HouseStyleRequest;
	}
};
//------------------------------------------------------------------------
//Server -> Client �ثe�Ыμ˦�
struct PG_GuildHouses_LtoC_HouseStyle
{
	GamePGCommandEnum	Command;

	int					Type;	

	PG_GuildHouses_LtoC_HouseStyle()
	{
		Command = EM_PG_GuildHouses_LtoC_HouseStyle;
	}
};