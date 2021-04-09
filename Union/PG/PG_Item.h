#pragma once

#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"

//--------------------------------------------------------------------------------------
//�q��Server���a�W�Y�@�Ӫ��~�åB�⦹���~��m������B
// Client -> Local
struct PG_Item_CtoL_GetGroundItem
{
    GamePGCommandEnum	Command;
    int					ItemID;				//���ѥ�
//    short				Type;				//(0 Body )
    short				Pos;				//���~��m����m

    PG_Item_CtoL_GetGroundItem()
    {
        Command = EM_PG_Item_CtoL_GetGroundItem;
    }
};
//--------------------------------------------------------------------------------------
//�q��client �ݬY���~��m��Y���O�_��

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
    ItemResult			Result;				//���ѥ�
	int					Count;				//��J�q(�u��buffer��J����)
	int					ItemID;				//���󸹽X

    PG_Item_LtoC_GetItemResult()
    {
        Command = EM_PG_Item_LtoC_GetItemResult;
    }
};
//--------------------------------------------------------------------------------------
//�q��Client �M���Ҧ���ItemBuffer���
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
//�q��Client ���s�����~�i�H��
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
//�n�D��Buf�������
// Client -> Local
struct PG_Item_CtoL_GetBufferItem
{
    GamePGCommandEnum	Command;
    ItemFieldStruct		Item;
 //   short				Type;				//(0 Body)
    short				Pos;				//���~��m����m
    short               Count;

    PG_Item_CtoL_GetBufferItem()
    {
        Command = EM_PG_Item_CtoL_GetBufferItem;
    }
};
//--------------------------------------------------------------------------------------
//�q���R���Y��쪫�~
//�p�G�������h��m����
// Client -> Local
struct PG_Item_CtoL_DeleteItem
{
    GamePGCommandEnum	Command;
    ItemFieldStruct		Item;
    short				Type;				//(0 Body 1 Bank 2 BodyMoney 3 BankMoney)
    short				Pos;				//���~��m����m


    PG_Item_CtoL_DeleteItem()
    {
        Command = EM_PG_Item_CtoL_DeleteItem;
    }
};
//--------------------------------------------------------------------------------------
//�q���R���Y��쪫�~
//�p�G�������h��m����
// Local -> Client
struct PG_Item_LtoC_DeleteItem
{
    GamePGCommandEnum	Command;
    ItemFieldStruct		Item;
    short				Type;				//(0 Body 1 Bank 2 BodyMoney 3 BankMoney)
//    short				Pos;				//���~��m����m
    ItemResult			Result;

    PG_Item_LtoC_DeleteItem()
    {
        Command = EM_PG_Item_LtoC_DeleteItem;
    }
};
//--------------------------------------------------------------------------------------
//�n�D�⪫���~�洫
// Client -> Local
struct	PG_Item_CtoL_ExchangeField
{
    GamePGCommandEnum	Command;
    ItemFieldStruct		Item[2];
    short				Type[2];			//(0 Body 1 Bank )
    short				Pos[2];				//���~��m����m
	int					ClientID;			//Client �B�z��ID

    PG_Item_CtoL_ExchangeField()
    {
        Command = EM_PG_Item_CtoL_ExchangeField;
    }
};
//--------------------------------------------------------------------------------------
//�^���⪫�~���洫���G
// Local -> Client
struct	PG_Item_LtoC_ExchangeField
{
    GamePGCommandEnum	Command;
    short				Type[2];			//(0 Body 1 Bank)
    ItemResult			Result;
	int					ClientID;			//Client �B�z��ID

    PG_Item_LtoC_ExchangeField()
    {
        Command = EM_PG_Item_LtoC_ExchangeField;
    }
};
//--------------------------------------------------------------------------------------
//�n�D�⪫�~���洫���G
// Client -> Local
struct	PG_Item_CtoL_RequestBodyBankMoneyExchange
{
    GamePGCommandEnum	Command;

    int					Count;		//����[ �Ȧ��


    PG_Item_CtoL_RequestBodyBankMoneyExchange()
    {
        Command = EM_PG_Item_CtoL_RequestBodyBankMoneyExchange;
    }
};

//--------------------------------------------------------------------------------------
//�^������P�Ȧ�����洫
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
//�n�D���eBuffer�������
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
//�n�D�⨭��Y��������b
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
//�n�D�⨭��Y��������b �����G
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
//�ץ��Y�@��m�����
// Local -> Client
struct	PG_Item_LtoC_FixItemInfo
{
    GamePGCommandEnum	Command;
    ItemFieldStruct		Item;
    short				Type;				//(0 Body 1 Bank 2 ���� )
    short				Pos;				//���~��m����m

    PG_Item_LtoC_FixItemInfo()
    {
        Command = EM_PG_Item_LtoC_FixItemInfo;
    }
};
//--------------------------------------------------------------------------------------
//�n�D�ץ��Ҧ����~��s
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
//�ץ��Ҧ����~��s
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
//�ץ����� �Ȧ����
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

//�q���ثe�]�q�P�Ȧ���쪺�W�u�q
//Local -> Client
struct  PG_Item_LtoC_MaxItemCount
{
    GamePGCommandEnum	Command;

    int					MaxBody;		//�������W��
    int					MaxBank;		//�Ȧ����W��

    PG_Item_LtoC_MaxItemCount()
    {
        Command = EM_PG_Item_LtoC_MaxItemCount;
    }
};
//--------------------------------------------------------------------------------------
//�q���}�һȦ�
//Local  -> Client
struct  PG_Item_LtoC_OpenBank
{
    GamePGCommandEnum	Command;
	int					GItemID;	//�Ȧ檺����id

    PG_Item_LtoC_OpenBank()
    {
        Command = EM_PG_Item_LtoC_OpenBank;
    }
};
//--------------------------------------------------------------------------------------
//�q�������Ȧ�
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
//�q�������Ȧ�
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
//�n�D�ϥΪ��~
//Client -> Local
struct PG_Item_CtoL_UseItem
{
    GamePGCommandEnum	Command;
    int 				Pos;				//���~��m����m
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
//���~�ϥε��G
//Local -> Client
struct PG_Item_LtoC_UseItemResult
{
    GamePGCommandEnum	Command;

    bool		Result;		// false ���� true ��?
    short		Pos;		//���~��m-
    short		Type;		//(0 Body 1 Bank 2 EQ)

    PG_Item_LtoC_UseItemResult()
    {
        Command = EM_PG_Item_LtoC_UseItemResult;
    }
};
//--------------------------------------------------------------------------------------
//�ץ����n���~���X��
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
//�ץ��Y�Ϊ��X��
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
//�ץ��d�����X��
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
//��Y�@�i�d�����X��
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
//��Y�@�i�d�����X��
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
//�磌�~�ϥΪ��~ �n�D
//Client -> Local
struct PG_Item_CtoL_UseItemToItem_Notify
{
	GamePGCommandEnum	Command;

	int					TargetPosType;		// 0 ���� 1 �Ȧ� 2 �˳�

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


//�磌�~�ϥΪ��~ ���G
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
//�q���g��H  �Y�H���o�ĪG���y
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
	EM_EqRefineResult_None		,		//�S�ĪG
	EM_EqRefineResult_LevelUp	,		//���ŤW��
	EM_EqRefineResult_LevelDown	,		//���ŤU��
	EM_EqRefineResult_Broken	,		//���~�a��
	EM_EqRefineResult_Protect	,		//�˳ƫO�@
};
//�˳ƽĵ����G
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
//�n�D���Ѫ��~
//Client -> Local
struct PG_Item_CtoL_ItemDissolutionRequest
{
	GamePGCommandEnum	Command;

	ItemFieldStruct		Item;
	int					Pos;				//���~��m����m

	PG_Item_CtoL_ItemDissolutionRequest()
	{
		Command = EM_PG_Item_CtoL_ItemDissolutionRequest;
	}
};
//--------------------------------------------------------------------------------------
enum ItemDissolutionResultENUM
{
	EM_ItemDissolutionResult_OK			,
	EM_ItemDissolutionResult_OK_Empty	,	//�S�q�q�q��ѥX�F��
	EM_ItemDissolutionResult_Failed	,
};
//�n�D���Ѫ��~���G
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
//�q�����}���G
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
//�n�D���}�X��
//Local -> Client
struct PG_Item_LtoC_EQCombinOpen
{
	GamePGCommandEnum	Command;
	int					TargetNPC;
	int					ItemPos;
	int					Type;		//0����������  1����������

	PG_Item_LtoC_EQCombinOpen()
	{
		Command = EM_PG_Item_LtoC_EQCombinOpen;
	}
};
//--------------------------------------------------------------------------------------
//�q������
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
//�q������
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
//�n�D�X��
//Client -> Local
struct PG_Item_CtoL_EQCombinRequest
{
	GamePGCommandEnum	Command;
	int					CombinItemPos[ 2 ];	//���~��m
	int					CheckItemPos;		//�ˬd�X�����~

	PG_Item_CtoL_EQCombinRequest()
	{
		Command = EM_PG_Item_CtoL_EQCombinRequest;
	}
};
//--------------------------------------------------------------------------------------
//�n�D�X�����G
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
// accountbag �}��
//--------------------------------------------------------------------------------------
// Local -> DataCenter 
// �n�DAccountBag�����
struct PG_Item_LtoD_AccountBagInfoRequest
{
	GamePGCommandEnum	Command;

	int					PlayerDBID;
	StaticString<_MAX_ACCOUNT_SIZE_>     Account;  //�إߦ����󪺱b��

	PG_Item_LtoD_AccountBagInfoRequest()
	{
		Command = EM_PG_Item_LtoD_AccountBagInfoRequest;
	}
};
//--------------------------------------------------------------------------------------
// DataCenter -> Local
//AccountBag�����
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
// �}��Account Bag
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
// �n�D��Account Bag�����F��
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
// �n�D��Account Bag�����F��
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
//�ͦs�ɶ��� ���~�����q��
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
//�˳Ʈe�� ==> �]��
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
//�˳Ʈe�� ==> �]�� ���G
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
//��� �]��(�ɵ�)
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
//��� �]��(�ɵ�) ���G
struct PG_Item_LtoC_RefineMagicStoneResult
{
	GamePGCommandEnum	Command;

	int		MagicStonePos[3];		//-1 �N��S��
	int		BoxPos;
	bool	Result;

	PG_Item_LtoC_RefineMagicStoneResult()
	{
		Command = EM_PG_Item_LtoC_RefineMagicStoneResult;
	}
};
//--------------------------------------------------------------------------------------
//Client -> Local
//�˳� �P�]�ۺ��X
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
//�˳� �P�]�ۺ��X ���G
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
//�Ť��ഫ
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
//�Ť��ഫ ���G
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
//�Ť�ɵ�
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
//�Ť�ɵ� ���G
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
//�_���X��
struct PG_Item_CtoL_MagicBoxRequest
{
	GamePGCommandEnum	Command;

	PG_Item_CtoL_MagicBoxRequest()
	{
		Command = EM_PG_Item_CtoL_MagicBoxRequest;
	}
};

//�_���X�� ���G
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
//�q���g�򪱮a�ϥθ˳�
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
// �n�D ���ݩ�
struct PG_Item_CtoL_GamebleItemState
{
	GamePGCommandEnum	Command;

	int					TableID;
	int					ItemPos;
	ItemFieldStruct		BodyItem;
	int					LockFlag;		//��w���ܪ��ݩ�

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
	EM_GamebleItemStateResult_TableDataError		,	//���_�����D
	EM_GamebleItemStateResult_LockItemError			,
};
// Local -> Client
// �n�D ���ݩʵ��G
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
// �T�w�n���ݩ�
struct PG_Item_CtoL_GetGamebleItemState
{
	GamePGCommandEnum	Command;
	bool				GiveUp;		//�O�_���

	PG_Item_CtoL_GetGamebleItemState()
	{
		Command = EM_PG_Item_CtoL_GetGamebleItemState;
	}   
};
//------------------------------------------------------------------------------------------------------------
// Local -> Client
// �T�w�n���ݩ�
struct PG_Item_LtoC_GetGamebleItemStateResult
{
	GamePGCommandEnum	Command;
	bool				Result;		//�O�_���

	PG_Item_LtoC_GetGamebleItemStateResult()
	{
		Command = EM_PG_Item_LtoC_GetGamebleItemStateResult;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//�}�Ҫ��~��դ���
struct PG_Item_LtoC_OpenGamebleItemState
{
	GamePGCommandEnum	Command;
	int					TargetID;	//Ĳ�o����

	PG_Item_LtoC_OpenGamebleItemState()
	{
		Command = EM_PG_Item_LtoC_OpenGamebleItemState;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//������դ���
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
//������դ���
struct PG_Item_LtoC_CloseGamebleItemState
{
	GamePGCommandEnum	Command;

	PG_Item_LtoC_CloseGamebleItemState()
	{
		Command = EM_PG_Item_LtoC_CloseGamebleItemState;
	}   
};

//------------------------------------------------------------------------------------------------------------
//�R���Y���~
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
//�}�ҧI���]�۪�����
struct PG_Item_LtoC_OpenMagicStoneExchange
{
	GamePGCommandEnum	Command;
	int					MagicStoneID;	//
	int					Lv;				//���h
	int					Money[2];			

	PG_Item_LtoC_OpenMagicStoneExchange()
	{
		Command = EM_PG_Item_LtoC_OpenMagicStoneExchange;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client -> Server
//�T�w�I�����G
struct PG_Item_CtoL_MagicStoneExchangeResult
{
	GamePGCommandEnum	Command;
	int		AbilityID[3];		//�ݩ�ID
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
	EM_MagicStoneExchangeResult_NotOpen		,	//�����S�}
	EM_MagicStoneExchangeResult_MoneyErr	,	
	EM_MagicStoneExchangeResult_ClientCancel,
	EM_MagicStoneExchangeResult_ClientDataErr,	//client �ݰe�Ӫ���Ʀ����D
};
//Local -> Client 
//�}�ҧI���]�۪�����
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
//���X�^�J�Ť�
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
	EM_DrawOutRuneResult_ItemQueueErr	,	//�Ӧh�F��b ���~queue��
	EM_DrawOutRuneResult_ItemLock		,	//���~�w����
};
//���X�^�J�Ť�
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
//�Ÿ˦��o
//////////////////////////////////////////////////////////////////////////
//Client -> Local �n�D��Ǹ˩�J ���o
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
	EM_PutCoolClothResult_Exist			,	//���~�w�g�s�b
	EM_PutCoolClothResult_RefineErr		,	//���~����ҹL
	EM_PutCoolClothResult_FullErr		,	//���w��
	EM_PutCoolClothResult_ClothPosErr	,
	EM_PutCoolClothResult_LimitErr		,	//���~������D
	EM_PutCoolClothResult_KeyItemErr	,	//�����S���ʶR
};

//Local -> Client �^����J�Ǹ˪��ѪG
struct PG_Item_LtoC_PutCoolClothResult
{
	GamePGCommandEnum	Command;
	PutCoolClothResultENUM	Result;

	PG_Item_LtoC_PutCoolClothResult()
	{
		Command = EM_PG_Item_LtoC_PutCoolClothResult;
	}   
};

//Client -> Local �洫�Ÿ˦�m
struct PG_Item_CtoL_SwapCoolClothRequest
{
	GamePGCommandEnum	Command;
	int					Pos[2];

	PG_Item_CtoL_SwapCoolClothRequest()
	{
		Command = EM_PG_Item_CtoL_SwapCoolClothRequest;
	}   
};

//Local -> Client �洫�Ÿ˦�m
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
//�R���Y�@��Ÿ�
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
//Local -> Client �^���R�����G
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

//Client -> Local �n�D�]�w�@�M�Ǹ�
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
//Local -> Client �^�����G �]�w�@�M�Ǹ�
struct PG_Item_LtoC_SetCoolSuitResult
{
	GamePGCommandEnum	Command;
	bool	Result;

	PG_Item_LtoC_SetCoolSuitResult()
	{
		Command = EM_PG_Item_LtoC_SetCoolSuitResult;
	}   
};

//Client -> Local �n�D�]�w��Y�M�Ǹ�
struct PG_Item_CtoL_ShowCoolSuitIndexRequest
{
	GamePGCommandEnum	Command;
	int		CoolSuitIndexID;
	
	PG_Item_CtoL_ShowCoolSuitIndexRequest()
	{
		Command = EM_PG_Item_CtoL_ShowCoolSuitIndexRequest;
	}   
};

//Local -> Client �^�����G
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
//�S��]
//////////////////////////////////////////////////////////////////////////
//Client -> Local �⪫�~��J�S���I�]( �i�ϥΡ@���i���|�@�����ӡ@)
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
//Local -> Client ���~��J���G
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
//Local -> Client ���~��J���G
struct PG_Item_LtoC_PopExBagResult
{
	GamePGCommandEnum	Command;
	PopExBagResultENUM	Result;
	PG_Item_LtoC_PopExBagResult()
	{
		Command = EM_PG_Item_LtoC_PopExBagResult;
	}   
};

//Client -> Local �n�D�ϥ�
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

//Local -> Client �ϥε��G
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
//�}�ҵ}����3 ���˳ƧI�����~����
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
//�I���n�D
struct PG_Item_CtoL_Rare3EqExchangeItemRequest
{
	GamePGCommandEnum	Command;
	int					Type;		//�I������
	int					ItemPos[6];	//��������m

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
//�I�����G
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
//�n�D����
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
//�n�D�M���Y�@���ݩ�
struct PG_Item_CtoL_ClsAttribute
{
	GamePGCommandEnum	Command;
	int					Type;				//(0 Body 1 Bank )
	int					Pos;				//���~��m����m
	int					AttributeID;

	PG_Item_CtoL_ClsAttribute()
	{
		Command = EM_PG_Item_CtoL_ClsAttribute;
	}
};

//Local -> Client
//�n�D�M���Y�@���ݩ�
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
//�ݩʵѨ�
struct PG_Item_CtoL_MagicBoxAttrDrawOff
{
	GamePGCommandEnum	Command;
	PG_Item_CtoL_MagicBoxAttrDrawOff()
	{
		Command = EM_PG_Item_CtoL_MagicBoxAttrDrawOff;
	}
};

//Local -> Client
//�n�D�M���Y�@���ݩ�
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
//�J��@���
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//�n�D��J�@��ժ��~
//Local -> Client
//�n�D��J�@��ժ��~���G

//Client -> Local
//�n�D���X�@��ժ��~
//Local -> Client
//�n�D���X�@��ժ��~���G

//Client -> Local
//�n�D��J�J�媫�~
//Local -> Client
//�n�D��J�J�媫�~���G

//Client -> Local
//�n�D���X�J�媫�~
//Local -> Client
//�n�D���X�J�媫�~���G

//Client -> Local
//�n�D�ϥΦ@��
//Local -> Client
//�n�D�ϥΦ@�ﵲ�G



