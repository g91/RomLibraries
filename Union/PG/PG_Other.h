#pragma once
#include "roledata/RoleData.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

//�q���Ҧ�Game Server �p�G���Y����^���n�D��Server
struct PG_Other_LtoL_RequestPlayerPos
{
	GamePGCommandEnum	Command;

	int									ZoneID;		//�ӷ��ϰ�
	int									DBID;		//�n�D�̪�DBID
	StaticString <_MAX_NAMESTRING_LEN_> Name;		//�n�䪺�H


	PG_Other_LtoL_RequestPlayerPos()
	{
		Command = EM_PG_Other_LtoL_RequestPlayerPos;
	}
};

//�^���������H����m
struct PG_Other_LtoL_PlayerPos
{
	GamePGCommandEnum	Command;

	int				DBID;		//�n�D�̪�DBID
	int				ZoneID;		//
	int				RoomID;
	RolePosStruct	Pos;

	PG_Other_LtoL_PlayerPos()
	{
		Command = EM_PG_Other_LtoL_PlayerPos;
	}
};

//�q���Ҧ�Game Server �p�G���Y���� �ǰe��Y��m
struct PG_Other_LtoL_CallPlayer
{
	GamePGCommandEnum	Command;

	int									ZoneID;		//�ӷ��ϰ�
	StaticString <_MAX_NAMESTRING_LEN_> Name;		//�n�䪺�H
	RolePosStruct						Pos;
	int									RoomID;


	PG_Other_LtoL_CallPlayer()
	{
		Command = EM_PG_Other_LtoL_CallPlayer;
	}
};

//------------------------------------------------------------------------------------------------------------
//�n�DClient �}��lua�@���ι�ܤ���
enum LuaDialogType_ENUM
{
	EM_LuaDialogType_YesNo		= 0,	//
	EM_LuaDialogType_Select		,
	EM_LuaDialogType_Input		,
	EM_LuaDialogType_Message	,
	EM_LuaDialogType_Pet		,
	EM_LuaDialogType_None		= 1000
};
struct PG_Other_LtoC_OpenDialog
{
	GamePGCommandEnum	Command;	

	LuaDialogType_ENUM Type;
	StaticString < 256 > Content;

	PG_Other_LtoC_OpenDialog()
	{
		Command = EM_PG_Other_LtoC_OpenDialog;
	}
};
//------------------------------------------------------------------------------------------------------------
//�]�wClient�����e���
//Server -> Client
struct PG_Other_LtoC_DialogSetContent
{
	GamePGCommandEnum	Command;	

	StaticString < 256 > Content;	

	PG_Other_LtoC_DialogSetContent()
	{
		Command = EM_PG_Other_LtoC_DialogSetContent;
	}
};


struct PG_Other_LtoC_DialogSetTitle
{
	GamePGCommandEnum	Command;	

	StaticString < 256 > Title;	

	PG_Other_LtoC_DialogSetTitle()
	{
		Command = EM_PG_Other_LtoC_DialogSetTitle;
	}
};

//------------------------------------------------------------------------------------------------------------
//�]�wClient���ﶵ���
//Server -> Client
struct PG_Other_LtoC_DialogSelectStr
{
	GamePGCommandEnum	Command;	

	StaticString < 64 > SelectStr;	

	PG_Other_LtoC_DialogSelectStr()
	{
		Command = EM_PG_Other_LtoC_DialogSelectStr;
	}
};
//------------------------------------------------------------------------------------------------------------
//�]�wClient���ﶵ���
//Server -> Client
struct PG_Other_LtoC_DialogSelectStrEnd
{
	GamePGCommandEnum	Command;	

	PG_Other_LtoC_DialogSelectStrEnd()
	{
		Command = EM_PG_Other_LtoC_DialogSelectStrEnd;
	}
};
//------------------------------------------------------------------------------------------------------------
//�n�DClient ����lua�@���ι�ܤ���
struct PG_Other_LtoC_CloseDialog
{
	GamePGCommandEnum	Command;	

	PG_Other_LtoC_CloseDialog()
	{
		Command = EM_PG_Other_LtoC_CloseDialog;
	}
};
//------------------------------------------------------------------------------------------------------------
//�n�DClient ����lua�@���ι�ܤ���
struct PG_Other_CtoL_DialogSelectID
{
	GamePGCommandEnum	Command;
	int					SelectID;		//-1 �N�����

	PG_Other_CtoL_DialogSelectID()
	{
		Command = EM_PG_Other_CtoL_DialogSelectID;
	}
};
//------------------------------------------------------------------------------------------------------------
//�󴫰}��
//Local -> Client
struct PG_Other_LtoC_CampID
{
	GamePGCommandEnum	Command;
	int				GItemID;
	CampID_ENUM 	CampID;

	PG_Other_LtoC_CampID()
	{
		Command = EM_PG_Other_LtoC_CampID;
	}
};
//------------------------------------------------------------------------------------------------------------
//�q���Ҧ��ϰ�Server SysKeyValue
//DataCenter -> Local
struct PG_Other_DtoL_SysKeyValue
{
	GamePGCommandEnum	Command;
	int					TotalCount;
	int					ID;
	char				KeyStr[32];
	int					Value;

	PG_Other_DtoL_SysKeyValue()
	{
		Command = EM_PG_Other_DtoL_SysKeyValue;
	}
};
//------------------------------------------------------------------------------------------------------------
//�ǰeClient �į��T
//Client -> Local
struct PG_Other_CtoL_Performance
{
	GamePGCommandEnum	Command;
	char cMac[16];
	float MaxCPUUsage;
	float MaxMemUsage;
	float MaxFPS;
	float AverageCPUUsage;
	float AverageMemUsage;
	float AverageFPS;
	float MinCPUUsage;
	float MinMemUsage;
	float MinFPS;
	float Ping;
	float PosX;
	float PosY;
	float PosZ;
	PG_Other_CtoL_Performance()
	{
		Command = EM_PG_Other_CtoL_Performance;
	}
};
//------------------------------------------------------------------------------------------------------------
//�ǰeClient �į��T
//Client -> Local
struct PG_Other_CtoL_TracertList
{
	GamePGCommandEnum	Command;
	int DBID;
	int Ping;
	TracertList List;
	PG_Other_CtoL_TracertList()
	{
		Command = EM_PG_Other_CtoL_TracertList;
	}
};
//------------------------------------------------------------------------------------------------------------
//�ǰeClient �į��T
// Local -> DataCenter
//struct PG_Other_LtoD_Performance
//{
//	GamePGCommandEnum	Command;
//	int CliDBID;
//	char cMac[16];
//	float MaxCPUUsage;
//	float MaxMemUsage;
//	float MaxFPS;
//	float AverageCPUUsage;
//	float AverageMemUsage;
//	float AverageFPS;
//	float MinCPUUsage;
//	float MinMemUsage;
//	float MinFPS;
//	float Ping;
//	float PosX;
//	float PosY;
//	float PosZ;
//
//	PG_Other_LtoD_Performance()
//	{
//		Command = EM_PG_Other_LtoD_Performance;
//	}
//};
//------------------------------------------------------------------------------------------------------------
/*
enum SystemMessageENUM
{
EM_SystemMessage_NULL				= -1,
EM_SystemMessage_TranStateNotCancelYet	,	//������A�S�Ѱ�
EM_SystemMessage_

};
//�n�DClient ����lua�@���ι�ܤ���
struct PG_Other_XtoC_SystemMessage
{
GamePGCommandEnum	Command;
SystemMessageENUM	Msg;

PG_Other_XtoC_SystemMessage()
{
Command = EM_PG_Other_XtoC_SystemMessage;
}
};
*/
//------------------------------------------------------------------------------------------------------------
/*
enum SystemMessageDataENUM
{
EM_SystemMessageData_NULL		,
};
//�T�����
struct PG_Other_XtoC_SystemMessageData
{
GamePGCommandEnum	Command;
SystemMessageDataENUM	Msg;
int					DataSize;
char				Data[1024];

int Size()
{
return sizeof(this) - sizeof(Data)+DataSize;
}
PG_Other_XtoC_SystemMessageData()
{
Command = EM_PG_Other_XtoC_SystemMessageData;
}
};
*/
//------------------------------------------------------------------------------------------------------------
//�}�lCasting Bar �n�D
//Client -> Local
struct PG_Other_CtoL_BeginCastingRequest
{
	GamePGCommandEnum	Command;
	CastingTypeENUM		Type;
	int					ItemID;
	int					TargetGUID;

	PG_Other_CtoL_BeginCastingRequest()
	{
		Command = EM_PG_Other_CtoL_BeginCastingRequest;
	}
};
//------------------------------------------------------------------------------------------------------------
//�}�lCasting Bar �^��
//Local -> Client
struct PG_Other_LtoC_BeginCastingResult
{
	GamePGCommandEnum	Command;
	CastingTypeENUM		Type;
	int					CastingTime;
	int					ItemID;
	bool				Result;

	PG_Other_LtoC_BeginCastingResult()
	{
		Command = EM_PG_Other_LtoC_BeginCastingResult;
	}
};
//------------------------------------------------------------------------------------------------------------
//�q�� �n���_
//Client -> Local
struct PG_Other_CtoL_InterruptCastingRequest
{
	GamePGCommandEnum	Command;

	PG_Other_CtoL_InterruptCastingRequest()
	{
		Command = EM_PG_Other_CtoL_InterruptCastingRequest;
	}
};
//------------------------------------------------------------------------------------------------------------
//�}�lCasting Bar �^��
//Local -> Client
struct PG_Other_LtoC_InterruptCasting
{
	GamePGCommandEnum	Command;
	CastingTypeENUM		Type;

	PG_Other_LtoC_InterruptCasting()
	{
		Command = EM_PG_Other_LtoC_InterruptCasting;
	}
};
//------------------------------------------------------------------------------------------------------------
//Casting OK
//Local ->Client
struct PG_Other_LtoC_CastingOK
{
	GamePGCommandEnum	Command;
	CastingTypeENUM		Type;

	PG_Other_LtoC_CastingOK()
	{
		Command = EM_PG_Other_LtoC_CastingOK;
	}
};
//------------------------------------------------------------------------------------------------------------
#define _MAX_CLIENT_TRANSFER_DATA_SIZE 128
//�n�DServer��e��Ƶ��P�򪺪��a
// Client -> Local
struct PG_Other_CtoL_RangeDataTransferRequest
{
	GamePGCommandEnum	Command;
	int					Range;
	int                 DataSize;
	char                Data[_MAX_CLIENT_TRANSFER_DATA_SIZE];

	PG_Other_CtoL_RangeDataTransferRequest()
	{
		Command = EM_PG_Other_CtoL_RangeDataTransferRequest;
	}   
	int Size()
	{
		return sizeof(*this) - sizeof(Data)  + DataSize;
	}
};
//------------------------------------------------------------------------------------------------------------
//�n�DServer��e��Ƶ��P�򪺪��a
// Local -> Client
struct PG_Other_LtoC_RangeDataTransfer
{
	GamePGCommandEnum	Command;
	int                 DataSize;
	char                Data[_MAX_CLIENT_TRANSFER_DATA_SIZE];

	PG_Other_LtoC_RangeDataTransfer()
	{
		Command = EM_PG_Other_LtoC_RangeDataTransfer;
	}   
	int Size()
	{
		return sizeof(*this) - sizeof(Data)  + DataSize;
	}
};
//------------------------------------------------------------------------------------------------------------
//�V��ө� �n�D�V��
//Client -> Local
struct ColoringBaseStruct
{
	bool	MainColorEnabled;
	bool	OffColorEnabled;
	int		MainColor;
	int		OffColor;
};

struct ColoringStruct
{
	union
	{
		ColoringBaseStruct	Items[ 8 ];
		struct  
		{
			ColoringBaseStruct Head;
			ColoringBaseStruct Gloves;
			ColoringBaseStruct Shoes;
			ColoringBaseStruct Clothes;
			ColoringBaseStruct Pants;
			ColoringBaseStruct Back;
			ColoringBaseStruct Belt;
			ColoringBaseStruct Shoulder;
		};
	};

	bool	HairColorEnabled;
	bool	BodyColorEnabled;
	int		HairColor;
	int		BodyColor;
	int		HairStyle;
	int		FaceStyle;
};

struct PG_Other_CtoL_ColoringShopRequest
{
	GamePGCommandEnum	Command;

	ColoringStruct		ColorInfo;
	StaticString <_MAX_NAMESTRING_LEN_>  Password;

	PG_Other_CtoL_ColoringShopRequest()
	{
		Command = EM_PG_Other_CtoL_ColoringShopRequest;
	}   
};
//------------------------------------------------------------------------------------------------------------
enum	ColoringShopResultENUM
{
	EM_ColoringShopResult_OK					,
	EM_ColoringShopResult_SecondPasswordError	,	//�G���K�X�����D
	EM_ColoringShopResult_DataError				,	//��Ʀ����D
	EM_ColoringShopResult_AccountMoneyError		,	//�b��������
};
//�V��ө� �n�D�V�⵲�G
//Local -> Client
struct PG_Other_LtoC_ColoringShopResult
{
	GamePGCommandEnum	Command;

	ColoringShopResultENUM	Result;
	PG_Other_LtoC_ColoringShopResult()
	{
		Command = EM_PG_Other_LtoC_ColoringShopResult;
	}   
};
//------------------------------------------------------------------------------------------------------------
//�n�D�ݬY���a�����
//Client -> Local
struct PG_Other_CtoL_PlayerInfoRequest
{
	GamePGCommandEnum	Command;
	int		GItemID;

	PG_Other_CtoL_PlayerInfoRequest()
	{
		Command = EM_PG_Other_CtoL_PlayerInfoRequest;
	}
};
//------------------------------------------------------------------------------------------------------------
//�n�D�ݬY���a����� ���G
//Local -> Client
struct PG_Other_LtoC_PlayerInfoResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	int					GItemID;
	EQStruct			EQ;
	StaticString< _MAX_FRIENDLIST_NOTE_ >   Note;

	PG_Other_LtoC_PlayerInfoResult()
	{
		Command = EM_PG_Other_LtoC_PlayerInfoResult;
	}
};
//------------------------------------------------------------------------------------------------------------
 
//Client -> Local
struct PG_Other_CtoL_SysKeyFunctionRequest
{
	GamePGCommandEnum	Command;
	int		KeyID;			//Keyid  0 �}�l
	int		TargetGUID;		//�ؼ�

	PG_Other_CtoL_SysKeyFunctionRequest()
	{
		Command = EM_PG_Other_CtoL_SysKeyFunctionRequest;
	}
};
//------------------------------------------------------------------------------------------------------------
 
//Local -> Client
struct PG_Other_LtoC_SysKeyFunctionResult
{
	GamePGCommandEnum	Command;
	int		KeyID;			//Keyid  0 �}�l
	int		TargetGUID;		//�ؼ�
	bool	Result;			//���G

	PG_Other_LtoC_SysKeyFunctionResult()
	{
		Command = EM_PG_Other_LtoC_SysKeyFunctionResult;
	}
};
//------------------------------------------------------------------------------------------------------------
//�n�D�Ҧ���npc��m��T
//Client -> Local 
struct PG_Other_CtoL_AllObjectPosRequest
{
	GamePGCommandEnum	Command;

	PG_Other_CtoL_AllObjectPosRequest()
	{
		Command = EM_PG_Other_CtoL_AllObjectPosRequest;
	}
};
//------------------------------------------------------------------------------------------------------------
struct ObjectPosInfoStruct
{
	char	Name[32];
	float	X , Y , Z;
	int		GUID;
	int		OrgObjID;
	int		Level;
};
//�^���Ҧ���NPC��m��T
//Local -> Client
struct PG_Other_LtoC_ObjectPosInfo
{
	GamePGCommandEnum	Command;
	int		Count;
	int		DataSize;
	char	Data[0x40000];
	//ObjectPosInfoStruct Info[10000];

	PG_Other_LtoC_ObjectPosInfo()
	{
		Command = EM_PG_Other_LtoC_ObjectPosInfo;
	}
	int Size()
	{
		return sizeof(*this) - sizeof( Data ) + DataSize;
	}
};
//------------------------------------------------------------------------------------------------------------
enum ClientMenuType_ENUM
{
	EM_ClientMenuType_Bag				,	//�I�]
	EM_ClientMenuType_Skill				,	//�ޯ��
	EM_ClientMenuType_EQ				,	//EQ����
	EM_ClientMenuType_Quest				,	//������x
	EM_ClientMenuType_FriendList		,	//�B�ͦC��
	EM_ClientMenuType_CardBook			,	//�d��
	EM_ClientMenuType_LifeSkill			,	//�ͬ��ޯ�
	EM_ClientMenuType_Guild				,	//���|����
	EM_ClientMenuType_GuildList			,	//���|�C��
	EM_ClientMenuType_GuildBillboard	,	//���|�d���O
	EM_ClientMenuType_Loading			,	//Loading�e��
	EM_ClientMenuType_SetGuildFlag		,	//�]�w���|�X����
	EM_ClientMenuType_Merchant			,	//�}�Ұӫ��ɭ�
	EM_ClientMenuType_GuildBank			,	//�}�Ҥ��|�Ȧ�

	EM_ClientMenuType_BlackSmith		,	// �}�ҥ��K
	EM_ClientMenuType_Carpenter			,	// �}�Ҥ�u
	EM_ClientMenuType_MakeArmor			,	// �}�һs��
	EM_ClientMenuType_Tailor			,	// �}�ҵ��_
	EM_ClientMenuType_Cook				,	// �}�Ҳi��
	EM_ClientMenuType_Alchemy			,	// �}�ҷҪ�

	EM_ClientMenuType_Mining			,	// �}�ҫ��q
	EM_ClientMenuType_Lumbering			,	// �}�Ҭ��
	EM_ClientMenuType_Herblism			,	// �}�ұ���
	EM_ClientMenuType_Fishing			,	// �}�ҳ���
	EM_ClientMenuType_Plant				,	// �}�Һش�
	EM_ClientMenuType_BillBoard	=10000  ,   // �}�ұƦ�]
	EM_ClientMenuType_Max		,
};
//�n�D�}��Client �ݪ�����
//Local -> Client
struct PG_Other_LtoC_OpenClientMenu
{
	GamePGCommandEnum	Command;

	ClientMenuType_ENUM	MenuType;

	PG_Other_LtoC_OpenClientMenu()
	{
		Command = EM_PG_Other_LtoC_OpenClientMenu;
	}
};
//------------------------------------------------------------------------------------------------------------
//�n�DServer �]�Y��������t�ΰT��
//Client -> Local
struct PG_Other_CtoL_RunGlobalPlotRequest
{
	GamePGCommandEnum	Command;
	int		PlotID;
	int		Value;


	PG_Other_CtoL_RunGlobalPlotRequest()
	{
		Command = EM_PG_Other_CtoL_RunGlobalPlotRequest;
	}
};
//------------------------------------------------------------------------------------------------------------
//�n�DServer �]�Y��������t�ΰT�����G
//Local -> Client
struct PG_Other_LtoC_RunGlobalPlotResult
{
	GamePGCommandEnum	Command;
	bool		Result;

	PG_Other_LtoC_RunGlobalPlotResult()
	{
		Command = EM_PG_Other_LtoC_RunGlobalPlotResult;
	}
};
//------------------------------------------------------------------------------------------------------------
//�n�D����C��
//Client -> Local
struct PG_Other_CtoL_HateListRequest
{
	GamePGCommandEnum	Command;
	int					NpcGUID;		//���󸹽X

	PG_Other_CtoL_HateListRequest()
	{
		Command = EM_PG_Other_CtoL_HateListRequest;
	}
};
//------------------------------------------------------------------------------------------------------------
//����C���G
//Local -> Client
struct PG_Other_LtoC_HateListResult
{
	GamePGCommandEnum	Command;
	int					NpcGUID;		//���󸹽X
	NPCHateList			NPCHate;

	PG_Other_LtoC_HateListResult()
	{
		Command = EM_PG_Other_LtoC_HateListResult;
	}
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//�n�DManageLv �b���E���X
struct PG_Other_CtoL_ManagePasswordKey
{
	GamePGCommandEnum	Command;


	PG_Other_CtoL_ManagePasswordKey()
	{
		Command = EM_PG_Other_CtoL_ManagePasswordKey;
	}
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//�n�DManageLv �b���K�X
struct PG_Other_CtoL_ManagePassword
{
	GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_>  MD5Pwd;

	PG_Other_CtoL_ManagePassword()
	{
		Command = EM_PG_Other_CtoL_ManagePassword;
	}
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//�n�D�}�� �����B�o�@���� ����
struct PG_Other_LtoC_OpenGiveItemPlot
{
	GamePGCommandEnum	Command;	
	int			Count;			//�ƶq
	int			TargetGUID;
	bool		IsSort;			//�ݭn�Ƨ�
	PG_Other_LtoC_OpenGiveItemPlot()
	{
		Command = EM_PG_Other_LtoC_OpenGiveItemPlot;
	}
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//�����B�o�@��
struct PG_Other_CtoL_GiveItemPlot
{
	GamePGCommandEnum	Command;
	int			ItemPos[4];
	bool		IsSort;			//�ݭn�Ƨ�
	PG_Other_CtoL_GiveItemPlot()
	{
		Command = EM_PG_Other_CtoL_GiveItemPlot;
	}
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//�ˬd�G���K�X
struct PG_Other_CtoL_CheckPassword
{
	GamePGCommandEnum	Command;
	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

	PG_Other_CtoL_CheckPassword()
	{
		Command = EM_PG_Other_CtoL_CheckPassword;
	}
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//�ˬd�G���K�X
struct PG_Other_LtoC_CheckPasswordResult
{
	GamePGCommandEnum	Command;
	StaticString< _MAX_NAMERECORD_SIZE_ >	Password;
	bool									Result;

	PG_Other_LtoC_CheckPasswordResult()
	{
		Command = EM_PG_Other_LtoC_CheckPasswordResult;
	}
};
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
//�d�M�Y���a��DBID or �W��
// Client -> Local
struct PG_Other_CtoL_Find_DBID_RoleName
{
	GamePGCommandEnum	Command;
	int										DBID;		//-1 ��ܥΦW�l��DBID �_�h��
	StaticString< _MAX_NAMERECORD_SIZE_ >   RoleName;


	PG_Other_CtoL_Find_DBID_RoleName()
	{
		Command = EM_PG_Other_CtoL_Find_DBID_RoleName;
	}
};
//------------------------------------------------------------------------------------------------------------
//�d�M�Y���a��DBID or �W��
// Local -> DataCenter
struct PG_Other_LtoD_Find_DBID_RoleName
{
	GamePGCommandEnum	Command;
	int										CliDBID;	//�d�M�̪��a��DBID
	int										DBID;		//-1 ��ܥΦW�l��DBID �_�h��
	StaticString< _MAX_NAMERECORD_SIZE_ >   RoleName;


	PG_Other_LtoD_Find_DBID_RoleName()
	{
		Command = EM_PG_Other_LtoD_Find_DBID_RoleName;
	}
};
//------------------------------------------------------------------------------------------------------------
//�d�M�Y���a��DBID or �W�� ���G
//DataCenter -> Client
struct PG_Other_DtoC_Find_DBID_RoleName
{
	GamePGCommandEnum	Command;
	int										DBID;		
	StaticString< _MAX_NAMERECORD_SIZE_ >   RoleName;
	bool									IsFind;

	PG_Other_DtoC_Find_DBID_RoleName()
	{
		Command = EM_PG_Other_DtoC_Find_DBID_RoleName;
	}
};
//------------------------------------------------------------------------------------------------------------
//�Y���⵽�c�ȧ�K
struct PG_Other_LtoC_GoodEvilValue
{
	GamePGCommandEnum	Command;
	int					PlayerGUID;
	float				GoodEvilValue;
	GoodEvilTypeENUM	GoodEvilType;

	PG_Other_LtoC_GoodEvilValue()
	{
		Command = EM_PG_Other_LtoC_GoodEvilValue;
	}
};
//------------------------------------------------------------------------------------------------------------
//�n�DClient ����lua�@���ι�ܤ���
struct PG_Other_CtoL_DialogInput
{
	GamePGCommandEnum	Command;
	int					SelectID;
	StaticString< 256 > pszString;
	int					iCheckResult;		// �o�e�ʥ]�e, �ϥ� Client ż���ˬd, �N���G�x�s


	PG_Other_CtoL_DialogInput()
	{
		Command = EM_PG_Other_CtoL_DialogInput;
	}
};
//------------------------------------------------------------------------------------------------------------
// DataCenter -> Local 
// �����I����Table
struct PG_Other_DtoL_ExchangeMoneyTable
{
	GamePGCommandEnum	Command;
	int		Count;			//��Ƶ���
	int		DataSize;
	char	Data[0x10000];

	int	Size()
	{
		return sizeof( int )*3+DataSize;
	}

	PG_Other_DtoL_ExchangeMoneyTable()
	{
		Command = EM_PG_Other_DtoL_ExchangeMoneyTable;
	}
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
// �����I����Table
struct PG_Other_LtoC_ExchangeMoneyTable
{
	GamePGCommandEnum	Command;
	int		Count;			//��Ƶ���
	int		DataSize;
	char	Data[0x10000];

	int	Size()
	{
		return sizeof( int )*3+DataSize;
	}

	PG_Other_LtoC_ExchangeMoneyTable()
	{
		Command = EM_PG_Other_LtoC_ExchangeMoneyTable;
	}
};
//------------------------------------------------------------------------------------------------------------
// DataCenter -> Local 
// db�t�Φr���
struct PG_Other_DtoL_DBStringTable
{
	GamePGCommandEnum	Command;
	int		Count;			//��Ƶ���
	int		DataSize;
	char	Data[0x10000];

	int	Size()
	{
		return sizeof( int )*3+DataSize;
	}

	PG_Other_DtoL_DBStringTable()
	{
		Command = EM_PG_Other_DtoL_DBStringTable;
	}
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
// db�t�Φr���
struct PG_Other_LtoC_DBStringTable
{
	GamePGCommandEnum	Command;
	int		Count;			//��Ƶ���
	int		DataSize;
	char	Data[0x10000];

	int	Size()
	{
		return sizeof( int )*3+DataSize;
	}

	PG_Other_LtoC_DBStringTable()
	{
		Command = EM_PG_Other_LtoC_DBStringTable;
	}
};
//------------------------------------------------------------------------------------------------------------
#define _MAX_GM_NOTIFICATION_SUBJECT_LEN 128
#define _MAX_GM_NOTIFICATION_CONTENT_LEN 512
#define _MAX_GM_NOTIFICATION_CLASS_LEN 128
//------------------------------------------------------------------------------------------------------------
//�^��Gm���e
//client -> Local
struct PG_Other_CtoL_GmNotification
{
	GamePGCommandEnum	Command;
	StaticString< _MAX_GM_NOTIFICATION_SUBJECT_LEN > Subject;
	StaticString< _MAX_GM_NOTIFICATION_CONTENT_LEN > Content;
	StaticString< _MAX_GM_NOTIFICATION_CLASS_LEN > Classification;

	PG_Other_CtoL_GmNotification()
	{
		Command = EM_PG_Other_CtoL_GmNotification;
	}
};
//------------------------------------------------------------------------------------------------------------
//�V��ө� �n�D�V��
//Client -> Local
struct PG_Other_CtoL_ColoringHorse
{
	GamePGCommandEnum	Command;

	int					Color[4];
	StaticString <_MAX_NAMESTRING_LEN_>  Password;
	int					ItemPos;	//�V�⪺���~��m

	PG_Other_CtoL_ColoringHorse()
	{
		Command = EM_PG_Other_CtoL_ColoringHorse;
	}   
};
//------------------------------------------------------------------------------------------------------------
//�V��ө� �n�D�V�⵲�G
//Local -> Client
struct PG_Other_LtoC_ColoringHorseResult
{
	GamePGCommandEnum	Command;
	ColoringShopResultENUM	Result;
	PG_Other_LtoC_ColoringHorseResult()
	{
		Command = EM_PG_Other_LtoC_ColoringHorseResult;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client�H���W�٭��_
//Client -> Local
struct PG_Other_CtoL_RoleNameDuplicateErr
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>  RoleName;
	int			PlayerDBID[2];

	PG_Other_CtoL_RoleNameDuplicateErr()
	{
		Command = EM_PG_Other_CtoL_RoleNameDuplicateErr;
	}   
};
//------------------------------------------------------------------------------------------------------------
//PK �z�X�����~
//Local -> Client
struct PG_Other_LtoC_PKDeadDropItem
{
	GamePGCommandEnum	Command;
	ItemFieldStruct		Item;
	PG_Other_LtoC_PKDeadDropItem()
	{
		Command = EM_PG_Other_LtoC_PKDeadDropItem;
	}   
};

//------------------------------------------------------------------------------------------------------------
enum	PlayerEventTypeENUM
{
	EM_PlayerEventType_WeakMagic	,	//��z�k�N
};

//Server �q��Client �ƥ�
//Local -> Client
struct PG_Other_LtoC_PlayerEvent
{
	GamePGCommandEnum	Command;
	PlayerEventTypeENUM Type;

	PG_Other_LtoC_PlayerEvent()
	{
		Command = EM_PG_Other_LtoC_PlayerEvent;
	}   
};
//------------------------------------------------------------------------------------------------------------
//���Client�ݮ���
//Local -> Client
struct PG_Other_LtoC_ClientClock
{
	GamePGCommandEnum	Command;
	int		BeginTime;
	int		NowTime;
	int		EndTime;
	int		Type;

	PG_Other_LtoC_ClientClock()
	{
		Command = EM_PG_Other_LtoC_ClientClock;
	}   
};
//------------------------------------------------------------------------------------------------------------
//�q���b��������ɶ�
//Local -> Client
struct PG_Other_LtoC_AccountMoneyLockInfo
{
	GamePGCommandEnum	Command;
	int					LockAccountMoney[ Def_AccountMoneyLockTime_Max_Count_ ];
	int					LockAccount_Forever;

	PG_Other_LtoC_AccountMoneyLockInfo()
	{
		Command = EM_PG_Other_LtoC_AccountMoneyLockInfo;
	}   
};
//------------------------------------------------------------------------------------------------------------
//�q���i���Ϊ��b��������
//Local -> Client
struct PG_Other_LtoC_AccountMoneyTranError
{
	GamePGCommandEnum	Command;

	PG_Other_LtoC_AccountMoneyTranError()
	{
		Command = EM_PG_Other_LtoC_AccountMoneyTranError;
	}   
};

//------------------------------------------------------------------------------------------------------------
//�ݪ��ؼ�
//Client -> Local
struct PG_Other_CtoL_LookTarget
{
	GamePGCommandEnum	Command;
	int					TargetGUID;

	PG_Other_CtoL_LookTarget()
	{
		Command = EM_PG_Other_CtoL_LookTarget;
	}   
};
//------------------------------------------------------------------------------------------------------------
//�n�D�]�w�`���k�N
//Client -> Local
struct PG_Other_CtoL_SetCycleMagic
{
	GamePGCommandEnum	Command;
	int					MagicID;
	int					MagicPos;		

	PG_Other_CtoL_SetCycleMagic()
	{
		Command = EM_PG_Other_CtoL_SetCycleMagic;
	}   
};
//------------------------------------------------------------------------------------------------------------
//�n�D�]�w�`���k�N
//Local -> Client
struct PG_Other_LtoC_SetCycleMagic
{
	GamePGCommandEnum	Command;
	int					MagicID;
	int					MagicPos;		


	PG_Other_LtoC_SetCycleMagic()
	{
		Command = EM_PG_Other_LtoC_SetCycleMagic;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client �n�D�� Master �n�J���� : �e�˴�Key
//Client -> Local
struct PG_Other_CtoL_CloseMaster
{
	GamePGCommandEnum	Command;
	char				Password[ 33 ];

	PG_Other_CtoL_CloseMaster()
	{
		Command = EM_PG_Other_CtoL_CloseMaster;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> DataCenter
//Ū�� DB ���e
struct PG_Other_LtoD_SelectDB
{
	GamePGCommandEnum	Command;

	int						PlayerDBID;
	StaticString< 20 >		DataType;
	StaticString< 2048 >	SelectCmd;

	PG_Other_LtoD_SelectDB()
	{
		Command = EM_PG_Other_LtoD_SelectDB;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//�M���Y�@��¾�~�����
struct PG_Other_LtoC_DelJob
{
	GamePGCommandEnum	Command;
	Voc_ENUM			Job; 

	PG_Other_LtoC_DelJob()
	{
		Command = EM_PG_Other_LtoC_DelJob;
	}   
};
//------------------------------------------------------------------------------------------------------------
enum SmallGameMenuTypeENUM
{
	EM_SmallGameMenu_Open	,
	EM_SmallGameMenu_Close	,
};
//Local -> Client
//�]�w�p�C������
struct PG_Other_LtoC_SetSmallGameMenuType
{
	GamePGCommandEnum	Command;
	SmallGameMenuTypeENUM	Type;
	int						MenuID;

	PG_Other_LtoC_SetSmallGameMenuType()
	{
		Command = EM_PG_Other_LtoC_SetSmallGameMenuType;
	}   
};

struct PG_Other_LtoC_SetSmallGameMenuValue
{
	GamePGCommandEnum	Command;
	int					MenuID;
	int					ID;
	int					Value;

	PG_Other_LtoC_SetSmallGameMenuValue()
	{
		Command = EM_PG_Other_LtoC_SetSmallGameMenuValue;
	}   
};

struct PG_Other_LtoC_SetSmallGameMenuStr
{
	GamePGCommandEnum	Command;
	int					MenuID;
	int					ID;
	int					ContentSize;
	StaticString <512>	Content;

	PG_Other_LtoC_SetSmallGameMenuStr()
	{
		Command = EM_PG_Other_LtoC_SetSmallGameMenuStr;
	}   
	int PGSize()
	{
		return sizeof(*this) -sizeof(Content)+ ContentSize;
	}
};
//------------------------------------------------------------------------------------------------------------
//�]�wClient �e�ʥ]���I��Ҧ�
struct PG_Other_CtoL_SetSendPacketMode
{
	GamePGCommandEnum	Command;

	SendPacketModeStruct	Mode;

	PG_Other_CtoL_SetSendPacketMode()
	{
		Command = EM_PG_Other_CtoL_SetSendPacketMode;
	}   
};
//------------------------------------------------------------------------------------------------------------
//�n�DClient �n�D���m�ƥ�(�u������)
//Client -> Local
struct PG_Other_CtoL_ResetInstanceRequest
{
	GamePGCommandEnum	Command;
	int	ZoneID;

	PG_Other_CtoL_ResetInstanceRequest()
	{
		Command = EM_PG_Other_CtoL_ResetInstanceRequest;
	}   
};
//------------------------------------------------------------------------------------------------------------
//�n�DClient �n�D���m�ƥ�
//Local -> Client
struct PG_Other_LtoC_ResetInstanceResult
{
	GamePGCommandEnum	Command;
	bool	Result;

	PG_Other_LtoC_ResetInstanceResult()
	{
		Command = EM_PG_Other_LtoC_ResetInstanceResult;
	}   
};
//------------------------------------------------------------------------------------------------------------
//�q���Ҧ�server �d�߬O�_���YParty���ƥ�
//Local -> Local
struct PG_Other_LtoL_ResetInstance
{
	GamePGCommandEnum	Command;
	int		PlayerDBID;
	int		PartyID;

	PG_Other_LtoL_ResetInstance()
	{
		Command = EM_PG_Other_LtoL_ResetInstance;
	}   
};
//------------------------------------------------------------------------------------------------------------
 
//Local -> Client
struct PG_Other_LtoC_ResetInstanceInfo
{
	GamePGCommandEnum	Command;
	int		ZoneID;
	bool	IsReset;

	PG_Other_LtoC_ResetInstanceInfo()
	{
		Command = EM_PG_Other_LtoC_ResetInstanceInfo;
	}   
};
//------------------------------------------------------------------------------------------------------------
//�n�D�����⪺���
struct PG_Other_CtoL_GmResetRole
{
	GamePGCommandEnum	Command;
	int		RoleBaseSize;
	int		PlayerRoleBaseSize;
	BaseRoleData			BaseData;
	PlayerBaseRoleData	 	PlayerBaseData;

	PG_Other_CtoL_GmResetRole()
	{
		Command = EM_PG_Other_CtoL_GmResetRole;
	}   
};
//------------------------------------------------------------------------------------------------------------
//party �Y��
struct PG_Other_CtoL_PartyDice
{
	GamePGCommandEnum	Command;
	int		Type;
	PG_Other_CtoL_PartyDice()
	{
		Command = EM_PG_Other_CtoL_PartyDice;
	}   
};
//------------------------------------------------------------------------------------------------------------
//party �Y��
struct PG_Other_LtoC_PartyDice
{
	GamePGCommandEnum	Command;
	int		Type;
	int		Rand;
	int		PlayerDBID;

	PG_Other_LtoC_PartyDice()
	{
		Command = EM_PG_Other_LtoC_PartyDice;
	}   
};
//------------------------------------------------------------------------------------------------------------
//�n�D�ק鶴�鰩�[
struct PG_Other_CtoL_BodyShop
{
	GamePGCommandEnum	Command;

	char	BoneScale[15];

	PG_Other_CtoL_BodyShop()
	{
		Command = EM_PG_Other_CtoL_BodyShop;
	}   
};
//------------------------------------------------------------------------------------------------------------
//�n�D�ק鶴�鰩�[
struct PG_Other_LtoC_BodyShopResult
{
	GamePGCommandEnum	Command;

	bool	Result;

	PG_Other_LtoC_BodyShopResult()
	{
		Command = EM_PG_Other_LtoC_BodyShopResult;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//�^�����H���
struct PG_Other_CtoL_MoneyVendorReport
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_> Name;
	StaticString <256> Content;

	PG_Other_CtoL_MoneyVendorReport()
	{
		Command = EM_PG_Other_CtoL_MoneyVendorReport;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//�q���Q�H���|���
struct PG_Other_LtoC_MoneyVendorReport
{
	GamePGCommandEnum	Command;
	int			Count;	//�Q�|������	

	PG_Other_LtoC_MoneyVendorReport()
	{
		Command = EM_PG_Other_LtoC_MoneyVendorReport;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//�^�����H���
struct PG_Other_CtoL_BotReport
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_> Name;
	StaticString <256> Content;

	PG_Other_CtoL_BotReport()
	{
		Command = EM_PG_Other_CtoL_BotReport;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//�q���Q�H���|���
struct PG_Other_LtoC_BotReport
{
	GamePGCommandEnum	Command;
	int			Count;	//�Q�|������	

	PG_Other_LtoC_BotReport()
	{
		Command = EM_PG_Other_LtoC_BotReport;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//�Ѱ������H���|
struct PG_Other_CtoL_UnlockBotReport
{
	GamePGCommandEnum	Command;

	PG_Other_CtoL_UnlockBotReport()
	{
		Command = EM_PG_Other_CtoL_UnlockBotReport;
	}   
};
//------------------------------------------------------------------------------------------------------------
enum UnlockBotTypeENUM
{
	EM_UnlockBotType_Unlock			,	//�|���ؼиѰ���w
	EM_UnlockBotType_OnProtect		,	//�|���ؼФ@�p�ɤ��Ѷ}��w�L
	EM_UnlockBotType_OnPrcess		,	//�|���ؼХ��b�B�z��w��
	EM_UnlockBotType_TargetNoMove	,	//�|���ؼФ@�q�ɶ��S����,�����B�z

};
//Local -> Client
//�q�� ���|�H�@�Y�H�Ѱ����|
struct PG_Other_LtoC_UnlockBotReport
{
	GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_> Name;	
	UnlockBotTypeENUM	Type;

	PG_Other_LtoC_UnlockBotReport()
	{
		Command = EM_PG_Other_LtoC_UnlockBotReport;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//�n�D�ƧǻȦ檫�~
struct PG_Other_CtoL_SortBankRequest
{
	GamePGCommandEnum	Command;

	PG_Other_CtoL_SortBankRequest()
	{
		Command = EM_PG_Other_CtoL_SortBankRequest;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//�n�D�ƧǻȦ檫�~ �����q��
struct PG_Other_LtoC_SortBankEnd
{
	GamePGCommandEnum	Command;

	PG_Other_LtoC_SortBankEnd()
	{
		Command = EM_PG_Other_LtoC_SortBankEnd;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//�n�D���
struct PG_Other_CtoL_LoadClientData
{
	GamePGCommandEnum	Command;
	int					KeyID;

	PG_Other_CtoL_LoadClientData()
	{
		Command = EM_PG_Other_CtoL_LoadClientData;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> DataCenter
//�n�D���
struct PG_Other_LtoD_LoadClientData
{
	GamePGCommandEnum	Command;
	int					KeyID;
	int					PlayerDBID;

	PG_Other_LtoD_LoadClientData()
	{
		Command = EM_PG_Other_LtoD_LoadClientData;
	}   
};
//------------------------------------------------------------------------------------------------------------
//DataCenter -> Client
//�q��Cliet���
struct PG_Other_DtoC_LoadClientDataResult
{
	GamePGCommandEnum	Command;
	int					KeyID;
	int					Size;
	char				Data[0x10000];

	PG_Other_DtoC_LoadClientDataResult()
	{
		Command = EM_PG_Other_DtoC_LoadClientDataResult;
	}   
	int PGSize()
	{
		return sizeof(*this) - sizeof(Data) + Size;
	}
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//�g�JClient���
struct PG_Other_CtoL_SaveClientData
{
	GamePGCommandEnum	Command;
	int					KeyID;
	int					PageID;
	int					Size;
	char				Data[1024];

	PG_Other_CtoL_SaveClientData()
	{
		Command = EM_PG_Other_CtoL_SaveClientData;
	}   
};

//------------------------------------------------------------------------------------------------------------
//Local -> DataCenter
//�g�JClient��Ƶ��G
struct PG_Other_LtoC_SaveClientDataResult
{
	GamePGCommandEnum	Command;
	int					KeyID;
	bool				Result;
	PG_Other_LtoC_SaveClientDataResult()
	{
		Command = EM_PG_Other_LtoC_SaveClientDataResult;
	}   
};
//------------------------------------------------------------------------------------------------------------
struct MapMarkInfoStruct
{
	int		MarkID;
	float	X , Y , Z;
	StaticString<256>	Tip;
	int		IconID;

};
//Local -> Client
//�]�w�j�a��Tip �P Icon
struct PG_Other_LtoC_SetMapMark
{
	GamePGCommandEnum	Command;
	MapMarkInfoStruct	Info;

	PG_Other_LtoC_SetMapMark()
	{
		Command = EM_PG_Other_LtoC_SetMapMark;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//�M���j�a��Tip �P Icon
struct PG_Other_LtoC_ClsMapMark
{
	GamePGCommandEnum	Command;
	int		MarkID;				//-1 ��ܥ��M

	PG_Other_LtoC_ClsMapMark()
	{
		Command = EM_PG_Other_LtoC_ClsMapMark;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//�۱�
struct PG_Other_CtoL_Suicide
{
	GamePGCommandEnum	Command;
	int					Type;

	PG_Other_CtoL_Suicide()
	{
		Command = EM_PG_Other_CtoL_Suicide;
	}   
};
//------------------------------------------------------------------------------------------------------------
//�]�w Room �������ܼ�
//Local -> Client
struct PG_Other_LtoC_SetRoomValue
{
	GamePGCommandEnum	Command;
	int					KeyID;
	int					Value;

	PG_Other_LtoC_SetRoomValue()
	{
		Command = EM_PG_Other_LtoC_SetRoomValue;
	} 
};
//------------------------------------------------------------------------------------------------------------
//�M�� Room �������ܼ�
//Local -> Client
struct PG_Other_LtoC_ClsRoomValue
{
	GamePGCommandEnum	Command;
	int					KeyID;	//-1��ܥ��M

	PG_Other_LtoC_ClsRoomValue()
	{
		Command = EM_PG_Other_LtoC_ClsRoomValue;
	} 
};
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//�n�D���
struct PG_Other_CtoM_LoadClientData_Account
{
	GamePGCommandEnum	Command;
	int					KeyID;

	PG_Other_CtoM_LoadClientData_Account()
	{
		Command = EM_PG_Other_CtoM_LoadClientData_Account;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> DataCenter
//�n�D���
struct PG_Other_MtoD_LoadClientData_Account
{
	GamePGCommandEnum	Command;
	int					SockID;
	int					ProxyID;

	int					KeyID;
	StaticString <_MAX_NAMESTRING_LEN_> Account;


	PG_Other_MtoD_LoadClientData_Account()
	{
		Command = EM_PG_Other_MtoD_LoadClientData_Account;
	}   
};
//------------------------------------------------------------------------------------------------------------
//DataCenter -> Client
//�q��Cliet���
struct PG_Other_DtoC_LoadClientDataResult_Account
{
	GamePGCommandEnum	Command;
	int					KeyID;
	int					Size;
	char				Data[0x10000];

	PG_Other_DtoC_LoadClientDataResult_Account()
	{
		Command = EM_PG_Other_DtoC_LoadClientDataResult_Account;
	}   
	int PGSize()
	{
		return sizeof(*this) - sizeof(Data) + Size;
	}
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//�g�JClient���
struct PG_Other_CtoM_SaveClientData_Account
{
	GamePGCommandEnum	Command;
	int					KeyID;
	int					PageID;
	int					Size;
	char				Data[1024];

	PG_Other_CtoM_SaveClientData_Account()
	{
		Command = EM_PG_Other_CtoM_SaveClientData_Account;
	}   
};

//------------------------------------------------------------------------------------------------------------
//Local -> DataCenter
//�g�JClient��Ƶ��G
struct PG_Other_MtoC_SaveClientDataResult_Account
{
	GamePGCommandEnum	Command;
	int					KeyID;
	bool				Result;
	PG_Other_MtoC_SaveClientDataResult_Account()
	{
		Command = EM_PG_Other_MtoC_SaveClientDataResult_Account;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//�s�W�@�Ӽ@���k�N
struct PG_Other_LtoC_AddExSkill
{
	GamePGCommandEnum	Command;
	ExSkillBaseStruct	Info;

	PG_Other_LtoC_AddExSkill()
	{
		Command = EM_PG_Other_LtoC_AddExSkill;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//�s�W�@�Ӽ@���k�N
struct PG_Other_LtoC_DelExSkill
{
	GamePGCommandEnum	Command;
	ExSkillBaseStruct	Info;

	PG_Other_LtoC_DelExSkill()
	{
		Command = EM_PG_Other_LtoC_DelExSkill;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//�n�D�I�k
struct PG_Other_CtoL_SpellExSkill
{
	GamePGCommandEnum	Command;
	int					TargetID;
	float				TargetX , TargetY, TargetZ;
	ExSkillBaseStruct	Info;

	int					ShootAngle;			//����
	int					ExplodeTime;	//�z���ɶ�

	PG_Other_CtoL_SpellExSkill()
	{
		Command = EM_PG_Other_CtoL_SpellExSkill;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//�n�D�I�k���G
struct PG_Other_LtoC_SpellExSkillResult
{
	GamePGCommandEnum	Command;
	bool				Result;
	RoleErrorMsgENUM	Msg;


	PG_Other_LtoC_SpellExSkillResult()
	{
		Command = EM_PG_Other_LtoC_SpellExSkillResult;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//�g�JClient �ƥ�log
struct PG_Other_CtoL_ClientEventLog
{
	GamePGCommandEnum	Command;
	int					EventType;

	PG_Other_CtoL_ClientEventLog()
	{
		Command = EM_PG_Other_CtoL_ClientEventLog;
	}   
};
//------------------------------------------------------------------------------------------------------------
enum ObjValueChangeENUM
{
	EM_ObjValueChange_Plot_Range		,	//�d��@��		( "�W��" , �d�� )
	EM_ObjValueChange_Plot_Touch		,	//Ĳ�I�@��		( "�W��" , �d�� )
	EM_ObjValueChange_Plot_ClientAuto	,	//Client�۰ʼ@��( "�W��" , 0 )
	EM_ObjValueChange_Plot_Collision	,	//�I���@��		( "�W��" , 0 )

	EM_ObjValueChange_CursorType		,	//���			( "" , ������� )

};
//Local -> Client
//�q���P�򦳬Y���� ���ƭȧ���
struct PG_Other_LtoC_BaseObjValueChange
{
	GamePGCommandEnum	Command;
	int					GItemID;
	ObjValueChangeENUM	Type;
	StaticString <_MAX_NAMESTRING_LEN_> ValueStr;		
	int					Value;

	PG_Other_LtoC_BaseObjValueChange()
	{
		Command = EM_PG_Other_LtoC_BaseObjValueChange;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//��e�P��Client(�p�ʥ]) 
struct PG_Other_CtoL_TransferData_Range
{
	GamePGCommandEnum	Command;
	char				Data[20];

	PG_Other_CtoL_TransferData_Range()
	{
		Command = EM_PG_Other_CtoL_TransferData_Range;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//��e�P��Client ( �p�ʥ] )
struct PG_Other_LtoC_TransferData_Range
{
	GamePGCommandEnum	Command;
	int					GItemID;
	char				Data[20];

	PG_Other_LtoC_TransferData_Range()
	{
		Command = EM_PG_Other_LtoC_TransferData_Range;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//�@��Ĳ�o�q��Client���ƥ�
struct PG_Other_LtoC_LuaClientEvent
{
	GamePGCommandEnum	Command;
	int					GItemID;
	int					EventType;
	int					Size;
	StaticString <256>  Content;

	PG_Other_LtoC_LuaClientEvent()
	{
		Command = EM_PG_Other_LtoC_LuaClientEvent;
	}   
	int PGSize()
	{
		return sizeof(*this) - sizeof( Content ) + Size;
	}
};
//------------------------------------------------------------------------------------------------------------
//�]�w����ƭ�
//Client -> Local
struct PG_Other_CtoL_SetRoleValue
{
	GamePGCommandEnum	Command;

	RoleValueName_ENUM	Type;
	double				Value;

	PG_Other_CtoL_SetRoleValue()
	{
		Command = EM_PG_Other_CtoL_SetRoleValue;
	}   
};
//------------------------------------------------------------------------------------------------------------
//�]�w����ƭ�
//Client -> Local
struct PG_Other_LtoC_SetRoleValueResult
{
	GamePGCommandEnum	Command;
	bool Result;

	PG_Other_LtoC_SetRoleValueResult()
	{
		Command = EM_PG_Other_LtoC_SetRoleValueResult;
	}   
};
//------------------------------------------------------------------------------------------------------------
//�q��Client �Y�Ӱϰ�}��
//Local -> Client
struct PG_Other_LtoC_ZoneOpen
{
	GamePGCommandEnum	Command;
	int ZoneID;

	PG_Other_LtoC_ZoneOpen()
	{
		Command = EM_PG_Other_LtoC_ZoneOpen;
	}   
};
//------------------------------------------------------------------------------------------------------------
struct PG_Other_CtoL_Client_Language
{
	GamePGCommandEnum	Command;
	int iLanguage;

	PG_Other_CtoL_Client_Language()
	{
		Command = EM_PG_Other_CtoL_Client_Language;
	}
};

//------------------------------------------------------------------------------------------------------------
struct PG_Other_DtoL_RoleRunPlot
{
	GamePGCommandEnum Command;
	StaticString<_MAX_LUA_FUNTIONNAMESIZE_>	LuaPlot;
	int DBID;

	PG_Other_DtoL_RoleRunPlot()
	{
		Command = EM_PG_Other_DtoL_RoleRunPlot;
	}
};
//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
enum NewTitleSysPosENUM
{
	EM_NewTitleSysPos_Quest			,
	EM_NewTitleSysPos_Challenge		,
	EM_NewTitleSysPos_Sys			,
	EM_NewTitleSysPos_Activitie		,
};
//�]�w[�s]�Y�Ψt�Ϊ��Y��
//Client -> Local
struct PG_Other_CtoL_SetNewTitleSys
{
	GamePGCommandEnum Command;
	NewTitleSysPosENUM	Pos;
	int					TitleID;

	PG_Other_CtoL_SetNewTitleSys()
	{
		Command = EM_PG_Other_CtoL_SetNewTitleSys;
	}
};

//�]�w[�s]�Y�Ψt�Ϊ��Y��
//Local -> Client
struct PG_Other_LtoC_SetNewTitleSysResult
{
	GamePGCommandEnum Command;
	bool Result;

	PG_Other_LtoC_SetNewTitleSysResult()
	{
		Command = EM_PG_Other_LtoC_SetNewTitleSysResult;
	}
};

//�ϥ�[�s]�Y�Ψt�Ϊ����~
//Client -> Local
struct PG_Other_CtoL_NewTitleSys_UseItem
{
	GamePGCommandEnum Command;
	int		PosType;		// 0 ���� 1 �Ȧ� 2 EQ
	int		Pos;	

	PG_Other_CtoL_NewTitleSys_UseItem()
	{
		Command = EM_PG_Other_CtoL_NewTitleSys_UseItem;
	}
};


enum NewTitleSysUseItemResultENUM
{
	EM_NewTitleSysUseItemResult_OK			,	
	EM_NewTitleSysUseItemResult_ItemNotFind	,	
	EM_NewTitleSysUseItemResult_NoEffect	,	
};
//�ϥ�[�s]�Y�Ψt�Ϊ����~result
//Local -> Client
struct PG_Other_LtoC_NewTitleSys_UseItemResult
{
	GamePGCommandEnum Command;
	NewTitleSysUseItemResultENUM	Result;

	PG_Other_LtoC_NewTitleSys_UseItemResult()
	{
		Command = EM_PG_Other_LtoC_NewTitleSys_UseItemResult;
	}
};


//�n�D�Y�ϰ����PCall
struct PG_Other_LtoL_PCall
{
	GamePGCommandEnum	Command;

	StaticString <256> PlotCmd;		

	PG_Other_LtoL_PCall()
	{
		Command = EM_PG_Other_LtoL_PCall;
	}
};
//------------------------------------------------------------------------------------------------------------
enum SessionTypeENUM
{
	EM_SessionType_None			= 0,
	EM_SessionType_OpenItemMall = 1,
	EM_SessionType_OpenZShop	= 2,
};
//------------------------------------------------------------------------------------------------------------
//�}��Session
//Client -> Local
struct PG_Other_CtoL_OpenSession
{
	GamePGCommandEnum Command;

	SessionTypeENUM	Type;
	int				CBID;

	PG_Other_CtoL_OpenSession()
	{
		Command = EM_PG_Other_CtoL_OpenSession;
	}
};
//------------------------------------------------------------------------------------------------------------
//����Session
//Client -> Local
struct PG_Other_CtoL_CloseSession
{
	GamePGCommandEnum Command;

	SessionTypeENUM	Type;
	int				SessionID;

	PG_Other_CtoL_CloseSession()
	{
		Command = EM_PG_Other_CtoL_CloseSession;
	}
};

//------------------------------------------------------------------------------------------------------------
//Session��T
//Local -> Client
struct PG_Other_LtoC_OpenSessionResult
{
	GamePGCommandEnum Command;

	SessionTypeENUM	Type;
	int				SessionID;
	int				CBID;

	PG_Other_LtoC_OpenSessionResult()
	{
		Command = EM_PG_Other_LtoC_OpenSessionResult;
	}
};
//------------------------------------------------------------------------------------------------------------
//�}��Session
//Local -> Client
struct PG_Other_LtoD_OpenSession
{
	GamePGCommandEnum Command;

	StaticString<64> Account;
	int				 RoleDBID;
	int				 ZoneID;
	SessionTypeENUM	 Type;
	int				 SessionID;
	int				 ValidTime; //Seconds

	PG_Other_LtoD_OpenSession()
	{
		Command = EM_PG_Other_LtoD_OpenSession;
	}
};
//------------------------------------------------------------------------------------------------------------
//����Session
//Local -> Client
struct PG_Other_LtoD_CloseSession
{
	GamePGCommandEnum Command;

	StaticString <64> Account;
	int				  RoleDBID;
	int				  ZoneID;
	SessionTypeENUM	  Type;	
	int				  SessionID;

	PG_Other_LtoD_CloseSession()
	{
		Command = EM_PG_Other_LtoD_CloseSession;
	}
};
//------------------------------------------------------------------------------------------------------------
//�ק�AcountFlag
//Local -> Client
struct PG_Other_LtoC_FixAccountFlag
{
	GamePGCommandEnum Command;

	StaticFlag< _ACCOUNT_FLAG_COUNT_ >	AccountFlag;		//�b����ƺX��

	PG_Other_LtoC_FixAccountFlag()
	{
		Command = EM_PG_Other_LtoC_FixAccountFlag;
	}
};
//------------------------------------------------------------------------------------------------------------
//client �۰ʦ^�������D�����a
struct PG_Other_CtoL_AutoReportPossibleCheater
{
	GamePGCommandEnum Command;
	AutoReportPossibleCheaterENUM	Type;
	AutoReportBaseInfoStruct		TargetInfo;	

	PG_Other_CtoL_AutoReportPossibleCheater()
	{
		Command = EM_PG_Other_CtoL_AutoReportPossibleCheater;
	}
};
//////////////////////////////////////////////////////////////////////////
//datacenter -> Local 
//���Ѫ��ƥ�
struct PG_Other_DtoL_WeekInstances
{
	GamePGCommandEnum Command;
	int				Count;
	WeekZoneStruct	Info[100];

	PG_Other_DtoL_WeekInstances()
	{
		Command = EM_PG_Other_DtoL_WeekInstances;
	}
};

//client -> local
//�n�D���Ѫ��ƥ�
struct PG_Other_CtoL_WeekInstancesRequest
{
	GamePGCommandEnum Command;

	PG_Other_CtoL_WeekInstancesRequest()
	{
		Command = EM_PG_Other_CtoL_WeekInstancesRequest;
	}
};

//Local -> Client
//���Ѫ��ƥ�
struct PG_Other_LtoC_WeekInstances
{
	GamePGCommandEnum Command;
	int				Day;
	int				Count;
	WeekZoneStruct	Info[100];

	PG_Other_LtoC_WeekInstances()
	{
		Command = EM_PG_Other_LtoC_WeekInstances;
	}
};

//Clent -> Local
//�n�D�ǰe��Y�ƥ�
struct PG_Other_CtoL_WeekInstancesTeleport
{
	GamePGCommandEnum Command;
	int ZoneID;

	PG_Other_CtoL_WeekInstancesTeleport()
	{
		Command = EM_PG_Other_CtoL_WeekInstancesTeleport;
	}
};

//Local -> Client
//�n�D���G
struct PG_Other_LtoC_WeekInstancesTeleportResult
{
	GamePGCommandEnum Command;
	bool	Result;

	PG_Other_LtoC_WeekInstancesTeleportResult()
	{
		Command = EM_PG_Other_LtoC_WeekInstancesTeleportResult;
	}
};

//Clent -> Local
//�n�D���m�Y�ƥ�
struct PG_Other_CtoL_WeekInstancesZoneReset
{
	GamePGCommandEnum Command;
	int ZoneID;

	PG_Other_CtoL_WeekInstancesZoneReset()
	{
		Command = EM_PG_Other_CtoL_WeekInstancesZoneReset;
	}
};


//Local -> Local
//�n�D���m�Y�ƥ�
struct PG_Other_LtoL_WeekInstancesZoneReset
{
	GamePGCommandEnum Command;
	int	DBID;
	int PartyID;

	PG_Other_LtoL_WeekInstancesZoneReset()
	{
		Command = EM_PG_Other_LtoL_WeekInstancesZoneReset;
	}
};

//Clent -> Local
//���m�ƥ��ǰe����
struct PG_Other_CtoL_WeekInstancesReset
{
	GamePGCommandEnum Command;

	PG_Other_CtoL_WeekInstancesReset()
	{
		Command = EM_PG_Other_CtoL_WeekInstancesReset;
	}
};

//Local -> Client
//���m�ƥ��ǰe���Ƶ��G
struct PG_Other_LtoC_WeekInstancesReset
{
	GamePGCommandEnum Command;
	bool	Result;

	PG_Other_LtoC_WeekInstancesReset()
	{
		Command = EM_PG_Other_LtoC_WeekInstancesReset;
	}
};

//Local -> Client
//�ƥ��ѤU���ͦs�ɶ�
struct PG_Other_LtoC_WeekInstancesLiveTime
{
	GamePGCommandEnum Command;
	int				Sec;

	PG_Other_LtoC_WeekInstancesLiveTime()
	{
		Command = EM_PG_Other_LtoC_WeekInstancesLiveTime;
	}
};
//////////////////////////////////////////////////////////////////
//Client->Local
//�]�F�ɶ�(�β���)
struct PG_Other_CtoL_PhantomRankUp
{
	GamePGCommandEnum Command;
	int		PhantomID;
	bool	Summon;

	PG_Other_CtoL_PhantomRankUp()
	{
		Command = EM_PG_Other_CtoL_PhantomRankUp;
	}
};

//Local -> Client
//�]�F�ɶ����G
struct PG_Other_LtoC_PhantomRankUpResult
{
	GamePGCommandEnum Command;
	bool	Summon;
	bool	Result;

	PG_Other_LtoC_PhantomRankUpResult()
	{
		Command = EM_PG_Other_LtoC_PhantomRankUpResult;
	}
};

//Client->Local
//�]�F�ɯ�
struct PG_Other_CtoL_PhantomLevelUp
{
	GamePGCommandEnum Command;
	int		PhantomID;	

	PG_Other_CtoL_PhantomLevelUp()
	{
		Command = EM_PG_Other_CtoL_PhantomLevelUp;
	}
};

//Local -> Client
//�]�F�ɯŵ��G
struct PG_Other_LtoC_PhantomLevelUpResult
{
	GamePGCommandEnum Command;
	bool	Result;

	PG_Other_LtoC_PhantomLevelUpResult()
	{
		Command = EM_PG_Other_LtoC_PhantomLevelUpResult;
	}
};

//Client -> Local
//�]�F�˳�
struct PG_Other_CtoL_PhantomEQ
{
	GamePGCommandEnum Command;
	int		EQ[2];	

	PG_Other_CtoL_PhantomEQ()
	{
		Command = EM_PG_Other_CtoL_PhantomEQ;
	}
};


//Client -> Local
//�]�F�˳Ƶ��G
struct PG_Other_LtoC_PhantomEQResult
{
	GamePGCommandEnum Command;
	bool	Result;

	PG_Other_LtoC_PhantomEQResult()
	{
		Command = EM_PG_Other_LtoC_PhantomEQResult;
	}
};

//Local -> Client
//�ץ��]�F�˳�
struct PG_Other_LtoC_FixPhantomEQ
{
	GamePGCommandEnum Command;
	int		EQ[2];	

	PG_Other_LtoC_FixPhantomEQ()
	{
		Command = EM_PG_Other_LtoC_FixPhantomEQ;
	}
};

//Local -> Client
//�ץ��]�F���
struct PG_Other_LtoC_FixPhantom
{
	GamePGCommandEnum Command;
	int				PhantomID;
	PhantomBase		Info;

	PG_Other_LtoC_FixPhantom()
	{
		Command = EM_PG_Other_LtoC_FixPhantom;
	}
};

//Local -> Client
//�}�Ҥ��F����
struct PG_Other_LtoC_OpenPhantomFrame
{
	GamePGCommandEnum Command;

	PG_Other_LtoC_OpenPhantomFrame()
	{
		Command = EM_PG_Other_LtoC_OpenPhantomFrame;
	}
};

//Local -> Client
//�}����@�ɰƥ�����
struct PG_Other_LtoC_OpenWeekInstancesFrame
{
	GamePGCommandEnum Command;

	PG_Other_LtoC_OpenWeekInstancesFrame()
	{
		Command = EM_PG_Other_LtoC_OpenWeekInstancesFrame;
	}
};

//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local
//��xBuff�]�w
struct PG_Other_DtoL_BuffSchedule
{
	GamePGCommandEnum	Command;
	int		BuffID;
	int		BuffLv;
	int		BuffTime;

	PG_Other_DtoL_BuffSchedule()
	{	
		Command		= EM_PG_Other_DtoL_BuffSchedule;	
	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local
//��x�]���_�����
struct PG_Other_DtoL_ItemCombinInfo
{
	GamePGCommandEnum	Command;
	int					GroupID;
	ItemCombineStruct	ItemCombine;

	PG_Other_DtoL_ItemCombinInfo()
	{	
		Command		= EM_PG_Other_DtoL_ItemCombinInfo;	
	}
};
//////////////////////////////////////////////////////////////////////////
//Local->Client
//��x�]���_������ID
struct PG_Other_LtoC_ItemCombinGroupID
{
	GamePGCommandEnum	Command;
	int					GroupID;

	PG_Other_LtoC_ItemCombinGroupID()
	{	
		Command		= EM_PG_Other_LtoC_ItemCombinGroupID;	
	}
};

//////////////////////////////////////////////////////////////////////////
//Local->Client
//��x�]���_�����
struct PG_Other_LtoC_ItemCombinInfo
{
	GamePGCommandEnum	Command;
	int					GroupID;
	ItemCombineStruct	ItemCombine;

	PG_Other_LtoC_ItemCombinInfo()
	{	
		Command		= EM_PG_Other_LtoC_ItemCombinInfo;	
	}
};


#pragma managed
