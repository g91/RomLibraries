#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

#define AC_BID_LIMIT_PERCENTAGE		1.05f
#define	_MAX_AC_SELL_ITEM_COUNT_	30	
#define _DEF_AC_BUY_TAXRATE_		0.94
//---------------------------------------------------------------------------------------------------------
//�j�M����		( -1 �N����)
struct	ACSearchConditionStruct
{
	StaticString <_MAX_NAMERECORD_SIZE_>	SearchName;
	int							Type[3];		//����		�D���� ������ �Ӷ�����
	EqWearSkillStruct			SearchType;		//�`������( )		
	int							LevelHi;		//������(���Žd��)	
	int							LevelLow;		//�C����(���Žd��)	
	int							Rare;			//�}����			
	int							RuneVolume;		//�Ť�e�q			
	SexFlagStruct				Sex;			//�ʧO				
	RaceFlagStruct				Race;			//�ر�			
	VocationFlagStruct			JobType;		//¾�~����
	PriceTypeENUM				PriceType;		//��������
	LanguageTypeENUM			Language;
};
//�j�M����
//Client -> Local
struct PG_AC_CtoL_SearchItem
{
	GamePGCommandEnum	Command;

	ACSearchConditionStruct	Data;

	PG_AC_CtoL_SearchItem( )
	{	Command = EM_PG_AC_CtoL_SearchItem;	}
};
//---------------------------------------------------------------------------------------------------------
//�j�M����
//Local -> DC
struct PG_AC_LtoDC_SearchItem
{
	GamePGCommandEnum	Command;

	int			DBID;			//���a�}��DBID
	ACSearchConditionStruct	Data;

	PG_AC_LtoDC_SearchItem( )
	{	Command = EM_PG_AC_LtoDC_SearchItem;	}
};
//---------------------------------------------------------------------------------------------------------
//�j�M���G�`�q
struct PG_AC_DCtoC_SearchItemCountResult
{
	GamePGCommandEnum	Command;

	int					MaxCount;	//�`�q
	int					Count;		//�n�e�X���q
	int					BeginIndex;	//�}�l��m

	PG_AC_DCtoC_SearchItemCountResult( )
	{	Command = EM_PG_AC_DCtoC_SearchItemCountResult;	}
};
//---------------------------------------------------------------------------------------------------------
//���ҷj�M�X�����~
struct ACItemInfoStruct
{
	int		DBID;
	int		NowPrices;
	int		BuyOutPrices;
	int		LiveTime;

	ItemFieldStruct		Item;	//�檺�F��
	PriceTypeENUM Type;
};

//�j�M���
//DC -> Client
struct PG_AC_DCtoC_SearchItemResult
{
	GamePGCommandEnum	Command;

	int					Index;
	ACItemInfoStruct	Data;
	StaticString <_MAX_NAMERECORD_SIZE_>	SellerName;		//�W�r
	int					BuyerDBID;

	PG_AC_DCtoC_SearchItemResult( )
	{	Command = EM_PG_AC_DCtoC_SearchItemResult;	}
};
//---------------------------------------------------------------------------------------------------------
//�q���쥻�̰������v�Ъ��Q�O�H�X�󰪻�
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
//�ҷj�M�X�Ӫ���Ʀ��ץ�
//DC -> Client
struct PG_AC_DCtoC_FixSearchItemResult
{
	GamePGCommandEnum	Command;

	int					ACItemDBID;		//AC ���~����ƮwID
	ACItemStateENUM		State;
	int					NewPrices;		//�ثe����
	int					BuyerDBID;
	PriceTypeENUM		Type;

	PG_AC_DCtoC_FixSearchItemResult( )
	{	Command = EM_PG_AC_DCtoC_FixSearchItemResult;	}
};
//---------------------------------------------------------------------------------------------------------
//�w���ƭץ�
//DC -> Client
struct PG_AC_DCtoC_FixBuyOrderItem
{
	GamePGCommandEnum	Command;

	int					ACItemDBID;		//AC ���~����ƮwID
	ACItemStateENUM		State;
	int					NewPrices;		//�ثe����
	int					BuyerDBID;
	PriceTypeENUM		Type;

	PG_AC_DCtoC_FixBuyOrderItem( )
	{	Command = EM_PG_AC_DCtoC_FixBuyOrderItem;	}
};
//---------------------------------------------------------------------------------------------------------
//�w���ƭץ�
//DC -> Client
struct PG_AC_DCtoC_FixSellOrderItem
{
	GamePGCommandEnum	Command;

	int					ACItemDBID;		//AC ���~����ƮwID
	ACItemStateENUM		State;
	int					NewPrices;		//�ثe����
	StaticString <_MAX_NAMERECORD_SIZE_>	BuyerName;		//�W�r
	PriceTypeENUM		Type;
	PG_AC_DCtoC_FixSellOrderItem( )
	{	Command = EM_PG_AC_DCtoC_FixSellOrderItem;	}
};

//---------------------------------------------------------------------------------------------------------
//���U 50�� �����
//Client -> Local
struct PG_AC_CtoL_GetNextSearchItem
{
	GamePGCommandEnum	Command;
	
	PG_AC_CtoL_GetNextSearchItem( )
	{	Command = EM_PG_AC_CtoL_GetNextSearchItem;	}
};
//---------------------------------------------------------------------------------------------------------
//���U 50�� �����
//Local -> DC
struct PG_AC_LtoDC_GetNextSearchItem
{
	GamePGCommandEnum	Command;
	int					DBID;

	PG_AC_LtoDC_GetNextSearchItem( )
	{	Command = EM_PG_AC_LtoDC_GetNextSearchItem;	}
};
//---------------------------------------------------------------------------------------------------------
//�n�D���o�ҽ檺�F��P�v�Ъ����~��T
//Client -> Local
struct PG_AC_CtoL_GetMyACItemInfo
{
	GamePGCommandEnum	Command;

	PG_AC_CtoL_GetMyACItemInfo( )
	{	Command = EM_PG_AC_CtoL_GetMyACItemInfo;	}
};
//---------------------------------------------------------------------------------------------------------
//�n�D���o�ҽ檺�F��P�v�Ъ����~��T
//Local -> DC
struct PG_AC_LtoDC_GetMyACItemInfo
{
	GamePGCommandEnum	Command;
	int					DBID;

	PG_AC_LtoDC_GetMyACItemInfo( )
	{	Command = EM_PG_AC_LtoDC_GetMyACItemInfo;	}
};
//---------------------------------------------------------------------------------------------------------
//�q���ثe�� �ۤv�� �U�Ъ����~
//DC -> Client
struct PG_AC_DCtoC_SelfCompetitive_BuyItem
{
	GamePGCommandEnum	Command;
	ACItemInfoStruct	Data;
	StaticString <_MAX_NAMERECORD_SIZE_>	SellerName;		//�W�r
	int					BuyerDBID;			//�̰����R�a��DBID

	PG_AC_DCtoC_SelfCompetitive_BuyItem( )
	{	Command = EM_PG_AC_DCtoC_SelfCompetitive_BuyItem;	}
};

//---------------------------------------------------------------------------------------------------------
//�q���ۤv���檺���~
//DC -> Client
struct PG_AC_DCtoC_SelfCompetitive_SellItem
{
	GamePGCommandEnum	Command;
	ACItemInfoStruct	Data;		
	StaticString <_MAX_NAMERECORD_SIZE_>	BuyerName;		//�W�r

	PG_AC_DCtoC_SelfCompetitive_SellItem( )
	{	Command = EM_PG_AC_DCtoC_SelfCompetitive_SellItem;	}
};
//---------------------------------------------------------------------------------------------------------
//
//�q���ۤv���檺���~
//DC -> Client
struct PG_AC_DCtoC_SelfCompetitive_ItemDataEnd
{
	GamePGCommandEnum	Command;

	PG_AC_DCtoC_SelfCompetitive_ItemDataEnd( )
	{	Command = EM_PG_AC_DCtoC_SelfCompetitive_ItemDataEnd;	}
};

//---------------------------------------------------------------------------------------------------------
//�v�Ъ��~ �� �����ʶR
//Client -> Local
struct PG_AC_CtoL_BuyItem
{
	GamePGCommandEnum	Command;
	int					ACItemDBID;		//�n�R�����~
	int					Prices;			//�ҥX������(�p�G ���󪽱��ʶR�� �N�����ʶR)
	PriceTypeENUM		Type;
	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

	PG_AC_CtoL_BuyItem( )
	{	Command = EM_PG_AC_CtoL_BuyItem;	}
};
//---------------------------------------------------------------------------------------------------------
//�v�Ъ��~ �� �����ʶR
//Local -> DC
struct PG_AC_LtoDC_BuyItem
{
	GamePGCommandEnum	Command;
	int					DBID;			//�R��
	int					ACItemDBID;		//�n�R�����~
	int					Prices;			//�ҥX������(�p�G ���󪽱��ʶR�� �N�����ʶR)
	PriceTypeENUM		Type;

	PG_AC_LtoDC_BuyItem( )
	{	Command = EM_PG_AC_LtoDC_BuyItem;	}
};
//---------------------------------------------------------------------------------------------------------
//�v�Ъ��~ �� �����ʶR ���G
struct PG_AC_DCtoL_BuyItemResult
{
	GamePGCommandEnum	Command;
	bool				Result;
	int					ACItemDBID;		//�n�R�����~
	int					Prices;			//�ҥX������(�p�G ���󪽱��ʶR�� �N�����ʶR)
	int					BuyerDBID;
	ACItemStateENUM		State;
	PriceTypeENUM		Type;

	PG_AC_DCtoL_BuyItemResult( )
	{	Command = EM_PG_AC_DCtoL_BuyItemResult;	}
};
//---------------------------------------------------------------------------------------------------------
//�v�Ъ��~ �� �����ʶR ���G
struct PG_AC_LtoC_BuyItemResult
{
	GamePGCommandEnum	Command;
	bool				Result;
	int					ACItemDBID;		//�n�R�����~
	int					Prices;			//�ҥX������(�p�G ���󪽱��ʶR�� �N�����ʶR)
	ACItemStateENUM		State;
	PriceTypeENUM		Type;

	PG_AC_LtoC_BuyItemResult( )
	{	Command = EM_PG_AC_LtoC_BuyItemResult;	}
};

//---------------------------------------------------------------------------------------------------------
//�檫�~
//Client -> Local
struct PG_AC_CtoL_SellItem
{
	GamePGCommandEnum	Command;
	int					BodyPos;
	ItemFieldStruct		Item;			//�檺�F��	
	int					Prices;			//�򥻻�
	int					BuyOutPrices;	//�����ʶR��
	int					LiveTime;		//(�����p��)
	PriceTypeENUM		Type;

	PG_AC_CtoL_SellItem( )
	{	Command = EM_PG_AC_CtoL_SellItem;	}
};
//---------------------------------------------------------------------------------------------------------
//���
//Client -> Local
struct PG_AC_CtoL_SellMoney
{
	GamePGCommandEnum	Command;
	PriceTypeENUM		SellType;
	int					SellMoney;

	int					Prices;			//�򥻻�
	int					BuyOutPrices;	//�����ʶR��
	int					LiveTime;		//(�����p��)
	PriceTypeENUM		Type;

	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

	PG_AC_CtoL_SellMoney( )
	{	Command = EM_PG_AC_CtoL_SellMoney;	}
};
//---------------------------------------------------------------------------------------------------------
enum SellMoneyResultENUM
{
	EM_SellMoneyResult_OK			,
	EM_SellMoneyResult_Err			,	//�@����� (���i������A)
	EM_SellMoneyResult_MoneyErr		,	//��������
	EM_SellMoneyResult_MoneyTypeErr	,	//�������������D( �ιC�������C���� , �Υαb�������b���� )
	EM_SellMoneyResult_PasswordErr	,
};
//��� ���G
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
//�檫�~
//Local -> DC
struct PG_AC_LtoDC_SellItem
{
	GamePGCommandEnum	Command;
	ItemFieldStruct		Item;			//�檺�F��	
	int					Prices;			//�򥻻�
	int					BuyOutPrices;	//�����ʶR��
	int					DBID;			//�֭n��
	int					LiveTime;		//�ͦs�ɶ�
	int					TaxMoney;		//�|��(���~�ɦ^�ǭץ���)
	PriceTypeENUM		Type;

	PG_AC_LtoDC_SellItem( )
	{	Command = EM_PG_AC_LtoDC_SellItem;	}
};
//---------------------------------------------------------------------------------------------------------
enum ACSellItemResultENUM
{
	EM_ACSellItemResult_OK				,	//���
	EM_ACSellItemResult_DataError		,	//��ƿ��~
	EM_ACSellItemResult_MaxCountError	,	//�ƶq��F�W��
	EM_ACSellItemResult_DisableTrade	,	//�T����
	EM_ACSellItemResult_MoneyError		,	//�������~
	EM_ACSellItemResult_EqFixError		,	//���~�ݭn�ײz
};
//�檫�~���G
//DC -> Local
struct PG_AC_DCtoL_SellItemResult
{
	GamePGCommandEnum	Command;

	//bool				Result;			//�O�_���
	ACSellItemResultENUM Result;

	ACItemInfoStruct	Item;			//�檺�F��	
	int					SellerDBID;
	int					TaxMoney;		//�|��(���~�ɦ^�ǭץ���)

	PG_AC_DCtoL_SellItemResult( )
	{	Command = EM_PG_AC_DCtoL_SellItemResult;	}
};
//---------------------------------------------------------------------------------------------------------
//�檫�~���G
//Local -> Client
struct PG_AC_LtoC_SellItemResult
{
	GamePGCommandEnum	Command;

	ACItemInfoStruct	Item;			//�檺�F��	
	//bool				Result;			//�O�_���
	ACSellItemResultENUM Result;

	PG_AC_LtoC_SellItemResult( )
	{	Command = EM_PG_AC_LtoC_SellItemResult;	}
};


//---------------------------------------------------------------------------------------------------------
//�������
//Client -> Local
struct PG_AC_CtoL_CancelSellItem
{
	GamePGCommandEnum	Command;
	int					ACItemDBID;			//AC ���~��DBID

	PG_AC_CtoL_CancelSellItem( )
	{	Command = EM_PG_AC_CtoL_CancelSellItem;	}
};
//---------------------------------------------------------------------------------------------------------
//�������
//Local -> DC
struct PG_AC_LtoDC_CancelSellItem
{
	GamePGCommandEnum	Command;
	int					DBID;				//�֦��� ( ��� )
	int					ACItemDBID;			//AC ���~��DBID

	PG_AC_LtoDC_CancelSellItem( )
	{	Command = EM_PG_AC_LtoDC_CancelSellItem;	}
};
//---------------------------------------------------------------------------------------------------------
//�^��������浲�G
// Datacenter -> Client
struct PG_AC_DCtoC_CancelSellItemResult
{
	GamePGCommandEnum	Command;
	int					ACItemDBID;			//AC ���~��DBID
	bool				Result;				

	PG_AC_DCtoC_CancelSellItemResult( )
	{	Command = EM_PG_AC_DCtoC_CancelSellItemResult;	}
};
//---------------------------------------------------------------------------------------------------------
//�}�ҩ���
//DataCenter -> Client
struct PG_AC_DCtoC_OpenAC
{
	GamePGCommandEnum	Command;
	bool		Result;
	int			NpcGUID;				//�B�z��NPC

	PG_AC_DCtoC_OpenAC( )
	{	Command = EM_PG_AC_DCtoC_OpenAC;	}
};
//---------------------------------------------------------------------------------------------------------
//�}�ҩ���
//Local -> DataCenter
struct PG_AC_LtoDC_OpenAC
{
	GamePGCommandEnum	Command;
	int					DBID;			//�n�D�����a
	int					NpcGUID;		//�B�z��NPC

	PG_AC_LtoDC_OpenAC( )
	{	Command = EM_PG_AC_LtoDC_OpenAC;	}
};
//---------------------------------------------------------------------------------------------------------
//��������
//Client -> Local
struct PG_AC_CtoL_CloseAC
{
	GamePGCommandEnum	Command;

	PG_AC_CtoL_CloseAC( )
	{	Command = EM_PG_AC_CtoL_CloseAC;	}
};
//---------------------------------------------------------------------------------------------------------
//��������
//DataCenter -> Client
struct PG_AC_DCtoC_CloseAC
{
	GamePGCommandEnum	Command;

	PG_AC_DCtoC_CloseAC( )
	{	Command = EM_PG_AC_DCtoC_CloseAC;	}
};
//---------------------------------------------------------------------------------------------------------
//��������
//Local -> DC
struct PG_AC_LtoDC_CloseAC
{
	GamePGCommandEnum	Command;
	int					DBID;

	PG_AC_LtoDC_CloseAC( )
	{	Command = EM_PG_AC_LtoDC_CloseAC;	}
};
//---------------------------------------------------------------------------------------------------------
//���o�Y���󪺾��v���
//Client -> Local
struct PG_AC_CtoL_ItemHistoryRequest
{
	GamePGCommandEnum	Command;
	int		OrgObjID;
	int		Inherent;		//���ϥ�

	PG_AC_CtoL_ItemHistoryRequest( )
	{	Command = EM_PG_AC_CtoL_ItemHistoryRequest;	}
};
//---------------------------------------------------------------------------------------------------------
//���o�Y���󪺾��v���
//Local -> DataCenter
struct PG_AC_LtoDC_ItemHistoryRequest
{
	GamePGCommandEnum	Command;
	int		DBID;			//���aDBID
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
//AC�w�b�}�Ҥ�
//Local -> Client
struct PG_AC_LtoC_AcAlreadyOpen
{
	GamePGCommandEnum	Command;
	int		TargtetNPC;

	PG_AC_LtoC_AcAlreadyOpen( )
	{	Command = EM_PG_AC_LtoC_AcAlreadyOpen;	}
};
//---------------------------------------------------------------------------------------------------------
//AC ��������q���R���
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

