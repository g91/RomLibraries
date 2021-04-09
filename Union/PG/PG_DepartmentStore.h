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

//Local -> Client �^�����ε��G
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
//Client -> Local �n�D�ӫ��c��������T
struct PG_DepartmentStore_CtoL_SellTypeRequest
{
	GamePGCommandEnum	Command;

	PG_DepartmentStore_CtoL_SellTypeRequest()
	{
		Command = EM_PG_DepartmentStore_CtoL_SellTypeRequest;
	}
};
//Local -> Datacenter �n�D�ӫ��c��������T
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


//Datacenter -> Client �n�D�ӫ��c��������T
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
//Client -> Local �n�D�ӫ���T
struct PG_DepartmentStore_CtoL_ShopInfoRequest
{
	GamePGCommandEnum	Command;
	int					SellType;

	PG_DepartmentStore_CtoL_ShopInfoRequest()
	{
		Command = EM_PG_DepartmentStore_CtoL_ShopInfoRequest;
	}
};
//Local -> Datacenter �n�D�ӫ���T
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
//Datacenter -> Client �ӫ���T
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
//Client -> Local �n�D�ʶR�ӫ����~
struct PG_DepartmentStore_CtoL_BuyItem
{
	GamePGCommandEnum	Command;
	int					ItemGUID;		//���~���޸�
	int					Count;			//�ʶR�ƶq
	int					Pos;			//��m��m
	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

	PG_DepartmentStore_CtoL_BuyItem()
	{
		Command = EM_PG_DepartmentStore_CtoL_BuyItem;
	}
};

//Local	-> DataCenter �n�D�ʶR�ӫ����~
struct PG_DepartmentStore_LtoD_BuyItem
{
	GamePGCommandEnum	Command;
	int					ItemGUID;		//���~���޸�
	int					PlayerDBID;		//���aDBID
	int					PlayerAccountMoney;
	int					Count;			//�ʶR�ƶq
	int					Pos;			//��m��m

	PG_DepartmentStore_LtoD_BuyItem()
	{
		Command = EM_PG_DepartmentStore_LtoD_BuyItem;
	}
};

enum DepartmentStoreBuyItemResultENUM
{
	EM_DepartmentStoreBuyItemResult_OK	,			//����
	EM_DepartmentStoreBuyItemResult_SellOut	,		//�槹�F
	EM_DepartmentStoreBuyItemResult_MoneyErr,		//��������
	EM_DepartmentStoreBuyItemResult_PacketErr,		//�]�q��m�����D
	EM_DepartmentStoreBuyItemResult_Err	,			//�䥦���~
	EM_DepartmentStoreBuyItemResult_PasswordErr	,	//�K�X���~
	EM_DepartmentStoreBuyItemResult_TimeOut	,		//�W�L�ɶ�
	
};

//Datacenter -> Local �ʶR���G
struct PG_DepartmentStore_DtoL_BuyItemResult
{
	GamePGCommandEnum	Command;
	int										PlayerDBID;		//���aDBID
	DepartmentStoreBuyItemResultENUM		Result;

	DepartmentStoreBaseInfoStruct	Item;
	int					Pos;			//��m��m
	int					Count;

	PG_DepartmentStore_DtoL_BuyItemResult()
	{
		Command = EM_PG_DepartmentStore_DtoL_BuyItemResult;
	}
};

//Local -> Client �ʶR���G
struct PG_DepartmentStore_LtoD_BuyItemResult
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;		//���aDBID
	int					ItemGUID;
	bool				Result;
	int					Count;
	
	PG_DepartmentStore_LtoD_BuyItemResult()
	{
		Command = EM_PG_DepartmentStore_LtoD_BuyItemResult;
	}
};

//Local -> DataCenter �ʶR���G(�p�q�B�z)
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
//Client -> Local �n�D����~���b����
struct PG_DepartmentStore_CtoL_ImportAccountMoney
{
	GamePGCommandEnum	Command;

	PG_DepartmentStore_CtoL_ImportAccountMoney()
	{
		Command = EM_PG_DepartmentStore_CtoL_ImportAccountMoney;
	}
};
//-----------------------------------------------------------------------
//Local -> DataCenter �n�D����~���b����
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
//DataCenter -> Local �n�D����~���b����
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
//DataCenter -> Local �n�D����~���b����
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
//Local ->DataCenter �T�w�b�����w����
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
//Client -> Local �n�D�I�����~
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
//Local -> DataCenter �n�D�I�����~
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
	EM_ItemExchangeResult_Depleted			= -8,	//�I������
	EM_ItemExchangeResult_RoleLimited		= -7,	//����I������
	EM_ItemExchangeResult_AccountLimited	= -6,	//�b���I������
	EM_ItemExchangeResult_DataError			= -5,	//�I�����~�]�w���~�A�гq���x��
	EM_ItemExchangeResult_ItemBufErr		= -4,	//���~��줣��
	EM_ItemExchangeResult_WorldError		= -3,	//���@�ɤ��i�I�������~
	EM_ItemExchangeResult_ItemUsed			= -2,	//���~�w�Q�ϥ�
	EM_ItemExchangeResult_Failed			= -1,	//���ѡA�L������
	EM_ItemExchangeResult_OK				,
};

//DataCenter -> Local �n�D�I�����~���G
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
//Local -> DataCenter �I�����~�T��e�����a
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
//Local -> Client �q��Client �I�����~���G
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
//Client -> Local �n�D�]�X�O�T��
struct PG_DepartmentStore_CtoL_RunningMessageRequest
{
	GamePGCommandEnum	Command;
	PG_DepartmentStore_CtoL_RunningMessageRequest()
	{
		Command = EM_PG_DepartmentStore_CtoL_RunningMessageRequest;
	}
};
//-----------------------------------------------------------------------
//Local -> DataCenter �n�D�]�X�O�T��
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
//Local -> DataCenter �n�D�]�X�O�T��
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
//Client -> Local �n�D�e§����L�H
struct PG_DepartmentStore_CtoL_MailGift
{
	GamePGCommandEnum	Command;

	int					ItemGUID;		//���~���޸�
	int					Count;			//�ʶR�ƶq
	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

	//�H���T
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
//Local -> DataCenter �ˬd�O�_�i�H�e���ؼ�
struct PG_DepartmentStore_LtoD_CheckMailGift
{
	GamePGCommandEnum	Command;

	int					PlayerDBID;
	int					ItemGUID;		//���~���޸�
	int					Count;			//�ʶR�ƶq
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
	EM_CheckMailGiftResultType_DisableSendGift	,		//�e�z���S���}��
};
//DataCenter -> Local �ˬd�O�_�i�H�e���ؼ�
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
//Local -> DataCenter �n�D�e§����L�H
struct PG_DepartmentStore_LtoD_MailGift
{
	GamePGCommandEnum	Command;

	int					ItemGUID;		//���~���޸�
	int					Count;			//�ʶR�ƶq
	int					Pos;			//��m��m

	PG_DepartmentStore_LtoD_MailGift()
	{
		Command = EM_PG_DepartmentStore_LtoD_MailGift;
	}
};
*/
//-----------------------------------------------------------------------
//Local -> Client �n�D�e§����L�H���G
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
//Client -> Local �n�D���X�����ӫ������~
struct PG_DepartmentStore_CtoL_WebBagRequest
{
	GamePGCommandEnum	Command;

	PG_DepartmentStore_CtoL_WebBagRequest()
	{
		Command = EM_PG_DepartmentStore_CtoL_WebBagRequest;
	}
};
//-----------------------------------------------------------------------
//Local -> DataCenter �n�D���X�����ӫ������~
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
//Local -> Client ���o�����ӫ����~�q��
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
//DataCenter -> Local �ӫ����~List
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
//�q���ʶR�ӫ����G
struct PG_CliConnect_LtoBL_BuyItemResult
{
	GamePGCommandEnum	Command;
	int										PlayerDBID;		//���aDBID
	DepartmentStoreBuyItemResultENUM		Result;

	DepartmentStoreBaseInfoStruct	Item;
	int					Pos;			//��m��m
	int					Count;

	PG_CliConnect_LtoBL_BuyItemResult()
	{
		Command = EM_PG_CliConnect_LtoBL_BuyItemResult;
	}
};
//-----------------------------------------------------------------------
//Battle Local -> Local
//�ʶR���G
struct PG_DepartmentStore_BLtoL_BuyItemResult
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;		//���aDBID
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
//���l�Ѫ�
//////////////////////////////////////////////////////////////////////////

//��J�������]�w
struct SlotMachineBase
{
	int Type;	//����
	int	Pos;	//��m
	int	Rate;	//���v
};
struct SlotMachineTypeBase
{
	int Type;		//����
	int	Rate;		//���v
	int	ItemID;		//���~ID
	int	ItemCount;	//���~�ƶq
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local �Ѫ�����
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
//Local -> Client �q���Ҧ��H �Ѫ����Ʀ�����
struct PG_DepartmentStore_LtoC_SlotMachineInfoChange
{
	GamePGCommandEnum	Command;

	PG_DepartmentStore_LtoC_SlotMachineInfoChange()
	{
		Command = EM_PG_DepartmentStore_LtoC_SlotMachineInfoChange;
	}
};
//Client -> Local �n�D�Ѫ�����
struct PG_DepartmentStore_CtoL_SlotMachineInfoRequest
{
	GamePGCommandEnum	Command;

	PG_DepartmentStore_CtoL_SlotMachineInfoRequest()
	{
		Command = EM_PG_DepartmentStore_CtoL_SlotMachineInfoRequest;
	}
};
//Local -> Client �q���Ѫ�����
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
//Client -> Local ���
struct PG_DepartmentStore_CtoL_SlotMachinePlay
{
	GamePGCommandEnum	Command;

	PG_DepartmentStore_CtoL_SlotMachinePlay()
	{
		Command = EM_PG_DepartmentStore_CtoL_SlotMachinePlay;
	}
};
//Local -> Client ���G
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