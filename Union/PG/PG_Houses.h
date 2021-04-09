#pragma once
#include "RoleData\RoleDataEx.h"
#include "NetWakerPGEnum.h"

//////////////////////////////////////////////////////////////////////////
//�R�Фl
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter �n�D�إߩЫ�
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
//DataCenter -> Local �إߩЫε��G
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
//Local -> Client �إߩЫε��G
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
//�i�J�Τl
//////////////////////////////////////////////////////////////////////////
//Local -> Client �n�D�}�Ұ��[�Фl������
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
//Client -> Local �n�D���[�Y�Фl�P(id,password)
struct PG_Houses_CtoL_VisitHouseRequest
{
	GamePGCommandEnum	Command;
	int			HouseDBID;		//���[�ж����X
	StaticString< _MAX_NAMERECORD_SIZE_ >   Password;
	

	PG_Houses_CtoL_VisitHouseRequest()
	{
		Command = EM_PG_Houses_CtoL_VisitHouseRequest;
	}
};
//------------------------------------------------------------------------
//Local -> DataCenter �d�M���ж����X�O�_�s�b �K�X�O�_���T
struct PG_Houses_LtoD_FindHouseRequest
{
	GamePGCommandEnum	Command;
	int			PlayerDBID;	//�n�D�����a
	int			HouseDBID;		//���[�ж����X
	StaticString< _MAX_NAMERECORD_SIZE_ >   Password;
	int			ManageLv;

	PG_Houses_LtoD_FindHouseRequest()
	{
		Command = EM_PG_Houses_LtoD_FindHouseRequest;
	}
};
//------------------------------------------------------------------------
//Client -> Local �n�D���[�Y�Фl�P(id,password)
struct PG_Houses_CtoL_VisitHouseRequest_ByName
{
	GamePGCommandEnum	Command;
	//int			HouseDBID;		//���[�ж����X
	StaticString< _MAX_NAMERECORD_SIZE_ >   RoleName;
	StaticString< _MAX_NAMERECORD_SIZE_ >   Password;


	PG_Houses_CtoL_VisitHouseRequest_ByName()
	{
		Command = EM_PG_Houses_CtoL_VisitHouseRequest_ByName;
	}
};
//------------------------------------------------------------------------
//Local -> DataCenter �d�M���ж����X�O�_�s�b �K�X�O�_���T
struct PG_Houses_LtoD_FindHouseRequest_ByName
{
	GamePGCommandEnum	Command;
	int			PlayerDBID;	//�n�D�����a
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
//DataCenter -> Local �d�M�ж����X�O�_�s�b �K�X�O�_���T ���G
struct PG_Houses_DtoL_FindHouseResult
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;	//�n�D�����a
	int					HouseDBID;
	FindHouseResultENUM	Result;

	PG_Houses_DtoL_FindHouseResult()
	{
		Command = EM_PG_Houses_DtoL_FindHouseResult;
	}
};

//Local -> Client �d�M�ж����X�O�_�s�b �K�X�O�_���T ���G
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
//Local -> Client �������[�Фl������
struct PG_Houses_LtoC_CloseVisitHouse
{
	GamePGCommandEnum	Command;
	PG_Houses_LtoC_CloseVisitHouse()
	{
		Command = EM_PG_Houses_LtoC_CloseVisitHouse;
	}
};
//------------------------------------------------------------------------
//Local -> Client �������[�Фl������
struct PG_Houses_CtoL_CloseVisitHouse
{
	GamePGCommandEnum	Command;
	PG_Houses_CtoL_CloseVisitHouse()
	{
		Command = EM_PG_Houses_CtoL_CloseVisitHouse;
	}
};
//------------------------------------------------------------------------
//Local -> Client ���ݸ��J�ж����
struct PG_Houses_LtoC_HouseInfoLoading
{
	GamePGCommandEnum	Command;
	PG_Houses_LtoC_HouseInfoLoading()
	{
		Command = EM_PG_Houses_LtoC_HouseInfoLoading;
	}
};
//------------------------------------------------------------------------
//Local -> DataCenter �n�D�إߩж�
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
//DataCenter -> Local �Ыθ��
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
//DataCenter -> Local �����~���
struct PG_Houses_DtoL_ItemInfo
{
	GamePGCommandEnum	Command;
	int		HouseDBID;
	int		ItemCount;
	int		ParentItemDBID;
	HouseItemDBStruct Item[ _DEF_MAX_HOUSE_FURNITURE + 10 ];		//�̦h�@���e100�� + 10�Ӥk��

	PG_Houses_DtoL_ItemInfo()
	{
		Command = EM_PG_Houses_DtoL_ItemInfo;
	}
};
//------------------------------------------------------------------------
//Local -> Client �Ыθ��
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
//Local -> Client ���~���	
struct PG_Houses_LtoC_ItemInfo
{
	GamePGCommandEnum	Command;
	int		ItemDBID;
	int		ItemCount;
	HouseItemDBStruct Item[_DEF_MAX_HOUSE_FURNITURE+10];		//�̦h�@���e100��

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
//DataCenter -> Local �q��Ū������							
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
//Local -> Client ���J����
struct PG_Houses_LtoC_HouseInfoLoadOK
{
	GamePGCommandEnum	Command;

	PG_Houses_LtoC_HouseInfoLoadOK()
	{
		Command = EM_PG_Houses_LtoC_HouseInfoLoadOK;
	}
};
//////////////////////////////////////////////////////////////////////////
//��m���~
//////////////////////////////////////////////////////////////////////////
//Client -> Local �n�D��m���~ or ���X���~
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
//Local -> DataCenter �ߧY�x�s��m���~
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
//Local -> Client �q����m���G
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
//Local -> Client �ץ��ʥ]Client���~���
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

//Client -> Local �n�D���~�h��
struct PG_Houses_CtoL_SwapHouseItemRequest
{
	GamePGCommandEnum	Command;
	int			HousePos[2];		//���줸 �������m   �C�줸 �ثe��m
	int			HouseParentItemDBID[2];

	PG_Houses_CtoL_SwapHouseItemRequest()
	{
		Command = EM_PG_Houses_CtoL_SwapHouseItemRequest;
	}
};
//------------------------------------------------------------------------
//Local -> Client �q���h�����G
struct PG_Houses_LtoC_SwapHouseItemResult
{
	GamePGCommandEnum	Command;	
	int			HousePos[2];		//���줸 �������m   �C�줸 �ثe��m
	int			ParentItemDBID[2];
	bool		Result;

	PG_Houses_LtoC_SwapHouseItemResult()
	{
		Command = EM_PG_Houses_LtoC_SwapHouseItemResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//���~�\�]
//////////////////////////////////////////////////////////////////////////
//Client -> Local �\�]
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
//Local -> Client �\�]���G
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
//Client -> Local �n�D�ɯũФl
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
//Local -> Client �^���ɯũФl���G
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
//Local -> Client �^���ʶR���G
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
//Client -> Local �ʶR����]�w
struct PG_Houses_CtoL_FunctionSettingRequest
{
	GamePGCommandEnum	Command;
	int		FunctionID;
	int		Mode;		//0 �Ѭ�

	PG_Houses_CtoL_FunctionSettingRequest()
	{
		Command = EM_PG_Houses_CtoL_FunctionSettingRequest;
	}
};
//------------------------------------------------------------------------
//Local -> Client �^���ʶR�]�w���G
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
//Client -> Local �ʶR��q�I��
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
//Local -> Client �^���ʶR��q�I�Ƶ��G
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
//�䥦
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter �x�s�Ыΰ򥻸��
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
//�x�s�ж����~
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
//�n�D�Y�H���}�ж�
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
//�n�D�Y�_�c���
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
//�n�D�Y�_�c���
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
//�n�D�P�Y���_�c�洫�˳�
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
//�n�D�P�Y���_�c�洫�˳�OK
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
//�]�w�ЫαK�X
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
//�]�w�ЫαK�X���G
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
//�]�w�ЫΦW��
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
//�]�w�Ы�Client �ݸ��
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
//�]�w�ЫΦW�ٵ��G
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
//�R���ЫΤ������~
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
//�R���ЫΤ������~
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
// �ЫΦn��
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
// �ЫΦn��
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
	bool			IsDelete;		//0 �s�W�@1�R��	
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
	bool			IsDelete;		//0 �s�W�@1�R��	
	int				FriendDBID;

	PG_Houses_LtoC_ModifyFriendDBID()
	{
		Command = EM_PG_Houses_LtoC_ModifyFriendDBID;
	}
};


//////////////////////////////////////////////////////////////////////////
//�ش�
//////////////////////////////////////////////////////////////////////////
//�M�Ŭ֮�
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
//�M�Ŭ֮⵲�G
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
//��F����֤l��(�ؤl �ή� �� )
//Client -> Local
struct PG_Houses_CtoL_PlantItemRequest
{
	GamePGCommandEnum	Command;
	PlantItemTypeENUM	Type;
	int					PotHousePos;		//�֤l��m	
	int					ItemBodyPos;		//�����m

	PG_Houses_CtoL_PlantItemRequest()
	{
		Command = EM_PG_Houses_CtoL_PlantItemRequest;
	}
};
//------------------------------------------------------------------------
//��F����֤l�̵��G
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
	EM_PlantItemResultType_Failed_GrowRateErr		,	//�w�g�������� ���i��@
	EM_PlantItemResultType_Failed_PlantCount		,	//�شӶW�L�W���q
	EM_PlantItemResultType_Failed_ItemNoUse			,	//����I�Ω��]�k��@�w��
};
struct PG_Houses_LtoC_PlantItemResult
{
	GamePGCommandEnum	Command;
	PlantItemTypeENUM	Type;
	int					PotHousePos;		//�֤l��m	
	int					ItemBodyPos;		//�����m

	PlantItemResultTypeENUM	Result;

	PG_Houses_LtoC_PlantItemResult()
	{
		Command = EM_PG_Houses_LtoC_PlantItemResult;
	}
};
//------------------------------------------------------------------------
//��֫v���A��w
//Client -> Local
struct PG_Houses_CtoL_PlantLockRequest
{
	GamePGCommandEnum	Command;
	int					PotHousePos;		//�֤l��m	
	PG_Houses_CtoL_PlantLockRequest()
	{
		Command = EM_PG_Houses_CtoL_PlantLockRequest;
	}
};
//------------------------------------------------------------------------
//��F����֤l�̵��G
//Local -> Client 
enum PlantLockResultTypeENUM
{
	EM_PlantLockResultType_OK		,
	EM_PlantLockResultType_Failed	,
};
struct PG_Houses_LtoC_PlantLockResult
{
	GamePGCommandEnum	Command;
	int					PotHousePos;		//�֤l��m	
	PlantLockResultTypeENUM	Result;

	PG_Houses_LtoC_PlantLockResult()
	{
		Command = EM_PG_Houses_LtoC_PlantLockResult;
	}
};
//------------------------------------------------------------------------
//��֫v����
//Client -> Local
struct PG_Houses_CtoL_PlantGetProductRequest
{
	GamePGCommandEnum	Command;
	int					PotHousePos;		//�֤l��m	
	PG_Houses_CtoL_PlantGetProductRequest()
	{
		Command = EM_PG_Houses_CtoL_PlantGetProductRequest;
	}
};
//------------------------------------------------------------------------
//��F����֤l�̵��G
//Local -> Client 
enum PlantGetProductResultTypeENUM
{
	EM_PlantGetProductResultType_OK			,
	EM_PlantGetProductResultType_Failed		,
	EM_PlantGetProductResultType_NoFruits	,	//�S����
};
struct PG_Houses_LtoC_PlantGetProductResult
{
	GamePGCommandEnum	Command;
	int					PotHousePos;		//�֤l��m	
	PlantGetProductResultTypeENUM	Result;

	PG_Houses_LtoC_PlantGetProductResult()
	{
		Command = EM_PG_Houses_LtoC_PlantGetProductResult;
	}
};
//------------------------------------------------------------------------
//Local -> Client
//�n�ͦs�����~��log
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
//�n�D�M��Log
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
// �k���ʥ]
//////////////////////////////////////////////////////////////////////////
//�n�D�۶ҦC��
//Client->Local
struct PG_Houses_CtoL_ServantHireListRequest
{
	GamePGCommandEnum	Command;


	PG_Houses_CtoL_ServantHireListRequest()
	{Command = EM_PG_Houses_CtoL_ServantHireListRequest;}
};
//////////////////////////////////////////////////////////////////////////
//�۶ҦC��^��
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
//�۶Ҥk��
//Client->Local
struct PG_Houses_CtoL_ServantHireRequest
{
	GamePGCommandEnum	Command;
	int ID;
	int	Pos;	//�k���n�񪺦�m

	PG_Houses_CtoL_ServantHireRequest()
	{Command = EM_PG_Houses_CtoL_ServantHireRequest;}
};
//////////////////////////////////////////////////////////////////////////
//�۶Ҥk�����G
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
//�n�D�R���k��
//Client->Local
struct PG_Houses_CtoL_ServantDelRequest
{
	GamePGCommandEnum	Command;
	int ServantID;

	PG_Houses_CtoL_ServantDelRequest()
	{Command = EM_PG_Houses_CtoL_ServantDelRequest;}
};
//////////////////////////////////////////////////////////////////////////
//�k���R�����G
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
//�k�����ʨƥ�
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
//�k�����ʨƥ�q��
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
//�k���\�]
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
//�k���R�W
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
//�k���R�W���G
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
//�ץ��k���ƭ�
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