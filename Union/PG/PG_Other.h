#pragma once
#include "roledata/RoleData.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

//通知所有Game Server 如果有某角色回應要求的Server
struct PG_Other_LtoL_RequestPlayerPos
{
	GamePGCommandEnum	Command;

	int									ZoneID;		//來源區域
	int									DBID;		//要求者的DBID
	StaticString <_MAX_NAMESTRING_LEN_> Name;		//要找的人


	PG_Other_LtoL_RequestPlayerPos()
	{
		Command = EM_PG_Other_LtoL_RequestPlayerPos;
	}
};

//回應收索的人物位置
struct PG_Other_LtoL_PlayerPos
{
	GamePGCommandEnum	Command;

	int				DBID;		//要求者的DBID
	int				ZoneID;		//
	int				RoomID;
	RolePosStruct	Pos;

	PG_Other_LtoL_PlayerPos()
	{
		Command = EM_PG_Other_LtoL_PlayerPos;
	}
};

//通知所有Game Server 如果有某角色 傳送到某位置
struct PG_Other_LtoL_CallPlayer
{
	GamePGCommandEnum	Command;

	int									ZoneID;		//來源區域
	StaticString <_MAX_NAMESTRING_LEN_> Name;		//要找的人
	RolePosStruct						Pos;
	int									RoomID;


	PG_Other_LtoL_CallPlayer()
	{
		Command = EM_PG_Other_LtoL_CallPlayer;
	}
};

//------------------------------------------------------------------------------------------------------------
//要求Client 開啟lua劇情用對話介面
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
//設定Client的內容資料
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
//設定Client的選項資料
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
//設定Client的選項資料
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
//要求Client 關閉lua劇情用對話介面
struct PG_Other_LtoC_CloseDialog
{
	GamePGCommandEnum	Command;	

	PG_Other_LtoC_CloseDialog()
	{
		Command = EM_PG_Other_LtoC_CloseDialog;
	}
};
//------------------------------------------------------------------------------------------------------------
//要求Client 關閉lua劇情用對話介面
struct PG_Other_CtoL_DialogSelectID
{
	GamePGCommandEnum	Command;
	int					SelectID;		//-1 代表取消

	PG_Other_CtoL_DialogSelectID()
	{
		Command = EM_PG_Other_CtoL_DialogSelectID;
	}
};
//------------------------------------------------------------------------------------------------------------
//更換陣營
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
//通知所有區域Server SysKeyValue
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
//傳送Client 效能資訊
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
//傳送Client 效能資訊
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
//傳送Client 效能資訊
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
EM_SystemMessage_TranStateNotCancelYet	,	//交易狀態沒解除
EM_SystemMessage_

};
//要求Client 關閉lua劇情用對話介面
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
//訊息資料
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
//開始Casting Bar 要求
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
//開始Casting Bar 回應
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
//通知 要中斷
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
//開始Casting Bar 回應
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
//要求Server轉送資料給周圍的玩家
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
//要求Server轉送資料給周圍的玩家
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
//染色商店 要求染色
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
	EM_ColoringShopResult_SecondPasswordError	,	//二次密碼有問題
	EM_ColoringShopResult_DataError				,	//資料有問題
	EM_ColoringShopResult_AccountMoneyError		,	//帳號幣不足
};
//染色商店 要求染色結果
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
//要求看某玩家的資料
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
//要求看某玩家的資料 結果
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
	int		KeyID;			//Keyid  0 開始
	int		TargetGUID;		//目標

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
	int		KeyID;			//Keyid  0 開始
	int		TargetGUID;		//目標
	bool	Result;			//結果

	PG_Other_LtoC_SysKeyFunctionResult()
	{
		Command = EM_PG_Other_LtoC_SysKeyFunctionResult;
	}
};
//------------------------------------------------------------------------------------------------------------
//要求所有的npc位置資訊
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
//回應所有的NPC位置資訊
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
	EM_ClientMenuType_Bag				,	//背包
	EM_ClientMenuType_Skill				,	//技能書
	EM_ClientMenuType_EQ				,	//EQ介面
	EM_ClientMenuType_Quest				,	//任物日誌
	EM_ClientMenuType_FriendList		,	//朋友列表
	EM_ClientMenuType_CardBook			,	//卡片
	EM_ClientMenuType_LifeSkill			,	//生活技能
	EM_ClientMenuType_Guild				,	//公會介面
	EM_ClientMenuType_GuildList			,	//公會列表
	EM_ClientMenuType_GuildBillboard	,	//公會留言板
	EM_ClientMenuType_Loading			,	//Loading畫面
	EM_ClientMenuType_SetGuildFlag		,	//設定公會旗介面
	EM_ClientMenuType_Merchant			,	//開啟商城界面
	EM_ClientMenuType_GuildBank			,	//開啟公會銀行

	EM_ClientMenuType_BlackSmith		,	// 開啟打鐵
	EM_ClientMenuType_Carpenter			,	// 開啟木工
	EM_ClientMenuType_MakeArmor			,	// 開啟製甲
	EM_ClientMenuType_Tailor			,	// 開啟裁縫
	EM_ClientMenuType_Cook				,	// 開啟烹飪
	EM_ClientMenuType_Alchemy			,	// 開啟煉金

	EM_ClientMenuType_Mining			,	// 開啟挖礦
	EM_ClientMenuType_Lumbering			,	// 開啟砍材
	EM_ClientMenuType_Herblism			,	// 開啟採藥
	EM_ClientMenuType_Fishing			,	// 開啟釣魚
	EM_ClientMenuType_Plant				,	// 開啟種植
	EM_ClientMenuType_BillBoard	=10000  ,   // 開啟排行榜
	EM_ClientMenuType_Max		,
};
//要求開啟Client 端的介面
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
//要求Server 跑某號的全域系統訊息
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
//要求Server 跑某號的全域系統訊息結果
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
//要求仇恨列表
//Client -> Local
struct PG_Other_CtoL_HateListRequest
{
	GamePGCommandEnum	Command;
	int					NpcGUID;		//物件號碼

	PG_Other_CtoL_HateListRequest()
	{
		Command = EM_PG_Other_CtoL_HateListRequest;
	}
};
//------------------------------------------------------------------------------------------------------------
//仇恨列表結果
//Local -> Client
struct PG_Other_LtoC_HateListResult
{
	GamePGCommandEnum	Command;
	int					NpcGUID;		//物件號碼
	NPCHateList			NPCHate;

	PG_Other_LtoC_HateListResult()
	{
		Command = EM_PG_Other_LtoC_HateListResult;
	}
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//要求ManageLv 帳號激活碼
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
//要求ManageLv 帳號密碼
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
//要求開啟 給予處發劇情的 介面
struct PG_Other_LtoC_OpenGiveItemPlot
{
	GamePGCommandEnum	Command;	
	int			Count;			//數量
	int			TargetGUID;
	bool		IsSort;			//需要排序
	PG_Other_LtoC_OpenGiveItemPlot()
	{
		Command = EM_PG_Other_LtoC_OpenGiveItemPlot;
	}
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//給予處發劇情
struct PG_Other_CtoL_GiveItemPlot
{
	GamePGCommandEnum	Command;
	int			ItemPos[4];
	bool		IsSort;			//需要排序
	PG_Other_CtoL_GiveItemPlot()
	{
		Command = EM_PG_Other_CtoL_GiveItemPlot;
	}
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//檢查二次密碼
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
//檢查二次密碼
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
//查尋某玩家的DBID or 名稱
// Client -> Local
struct PG_Other_CtoL_Find_DBID_RoleName
{
	GamePGCommandEnum	Command;
	int										DBID;		//-1 表示用名子找DBID 否則用
	StaticString< _MAX_NAMERECORD_SIZE_ >   RoleName;


	PG_Other_CtoL_Find_DBID_RoleName()
	{
		Command = EM_PG_Other_CtoL_Find_DBID_RoleName;
	}
};
//------------------------------------------------------------------------------------------------------------
//查尋某玩家的DBID or 名稱
// Local -> DataCenter
struct PG_Other_LtoD_Find_DBID_RoleName
{
	GamePGCommandEnum	Command;
	int										CliDBID;	//查尋者玩家的DBID
	int										DBID;		//-1 表示用名子找DBID 否則用
	StaticString< _MAX_NAMERECORD_SIZE_ >   RoleName;


	PG_Other_LtoD_Find_DBID_RoleName()
	{
		Command = EM_PG_Other_LtoD_Find_DBID_RoleName;
	}
};
//------------------------------------------------------------------------------------------------------------
//查尋某玩家的DBID or 名稱 結果
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
//某角色善惡值改便
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
//要求Client 關閉lua劇情用對話介面
struct PG_Other_CtoL_DialogInput
{
	GamePGCommandEnum	Command;
	int					SelectID;
	StaticString< 256 > pszString;
	int					iCheckResult;		// 發送封包前, 使用 Client 髒話檢查, 將結果儲存


	PG_Other_CtoL_DialogInput()
	{
		Command = EM_PG_Other_CtoL_DialogInput;
	}
};
//------------------------------------------------------------------------------------------------------------
// DataCenter -> Local 
// 金錢兌換的Table
struct PG_Other_DtoL_ExchangeMoneyTable
{
	GamePGCommandEnum	Command;
	int		Count;			//資料筆數
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
// 金錢兌換的Table
struct PG_Other_LtoC_ExchangeMoneyTable
{
	GamePGCommandEnum	Command;
	int		Count;			//資料筆數
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
// db系統字串表
struct PG_Other_DtoL_DBStringTable
{
	GamePGCommandEnum	Command;
	int		Count;			//資料筆數
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
// db系統字串表
struct PG_Other_LtoC_DBStringTable
{
	GamePGCommandEnum	Command;
	int		Count;			//資料筆數
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
//回報Gm內容
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
//染色商店 要求染色
//Client -> Local
struct PG_Other_CtoL_ColoringHorse
{
	GamePGCommandEnum	Command;

	int					Color[4];
	StaticString <_MAX_NAMESTRING_LEN_>  Password;
	int					ItemPos;	//染色的物品位置

	PG_Other_CtoL_ColoringHorse()
	{
		Command = EM_PG_Other_CtoL_ColoringHorse;
	}   
};
//------------------------------------------------------------------------------------------------------------
//染色商店 要求染色結果
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
//Client人物名稱重復
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
//PK 爆出的物品
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
	EM_PlayerEventType_WeakMagic	,	//虛弱法術
};

//Server 通知Client 事件
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
//顯示Client端時鐘
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
//通知帳號幣到期時間
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
//通知可活用的帳號幣不足
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
//看的目標
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
//要求設定循環法術
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
//要求設定循環法術
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
//Client 要求把 Master 登入關閉 : 送檢測Key
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
//讀取 DB 內容
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
//清除某一個職業的資料
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
//設定小遊戲介面
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
//設定Client 送封包的衰減模式
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
//要求Client 要求重置副本(只有隊長)
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
//要求Client 要求重置副本
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
//通知所有server 查詢是否有某Party的副本
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
//要求此角色的資料
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
//party 擲骰
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
//party 擲骰
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
//要求修改身體骨架
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
//要求修改身體骨架
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
//回報有人賣幣
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
//通知被人檢舉賣幣
struct PG_Other_LtoC_MoneyVendorReport
{
	GamePGCommandEnum	Command;
	int			Count;	//被舉報次數	

	PG_Other_LtoC_MoneyVendorReport()
	{
		Command = EM_PG_Other_LtoC_MoneyVendorReport;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//回報有人賣幣
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
//通知被人檢舉賣幣
struct PG_Other_LtoC_BotReport
{
	GamePGCommandEnum	Command;
	int			Count;	//被舉報次數	

	PG_Other_LtoC_BotReport()
	{
		Command = EM_PG_Other_LtoC_BotReport;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//解除機器人檢舉
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
	EM_UnlockBotType_Unlock			,	//舉報目標解除鎖定
	EM_UnlockBotType_OnProtect		,	//舉報目標一小時內解開鎖定過
	EM_UnlockBotType_OnPrcess		,	//舉報目標正在處理鎖定中
	EM_UnlockBotType_TargetNoMove	,	//舉報目標一段時間沒移動,忽略處理

};
//Local -> Client
//通知 檢舉人　某人解除檢舉
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
//要求排序銀行物品
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
//要求排序銀行物品 完成通知
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
//要求資料
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
//要求資料
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
//通知Cliet資料
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
//寫入Client資料
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
//寫入Client資料結果
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
//設定大地圖Tip 與 Icon
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
//清除大地圖Tip 與 Icon
struct PG_Other_LtoC_ClsMapMark
{
	GamePGCommandEnum	Command;
	int		MarkID;				//-1 表示全清

	PG_Other_LtoC_ClsMapMark()
	{
		Command = EM_PG_Other_LtoC_ClsMapMark;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//自殺
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
//設定 Room 的全域變數
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
//清除 Room 的全域變數
//Local -> Client
struct PG_Other_LtoC_ClsRoomValue
{
	GamePGCommandEnum	Command;
	int					KeyID;	//-1表示全清

	PG_Other_LtoC_ClsRoomValue()
	{
		Command = EM_PG_Other_LtoC_ClsRoomValue;
	} 
};
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
//Client -> Local
//要求資料
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
//要求資料
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
//通知Cliet資料
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
//寫入Client資料
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
//寫入Client資料結果
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
//新增一個劇情法術
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
//新增一個劇情法術
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
//要求施法
struct PG_Other_CtoL_SpellExSkill
{
	GamePGCommandEnum	Command;
	int					TargetID;
	float				TargetX , TargetY, TargetZ;
	ExSkillBaseStruct	Info;

	int					ShootAngle;			//角度
	int					ExplodeTime;	//爆炸時間

	PG_Other_CtoL_SpellExSkill()
	{
		Command = EM_PG_Other_CtoL_SpellExSkill;
	}   
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//要求施法結果
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
//寫入Client 事件log
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
	EM_ObjValueChange_Plot_Range		,	//範圍劇情		( "名稱" , 範圍 )
	EM_ObjValueChange_Plot_Touch		,	//觸碰劇情		( "名稱" , 範圍 )
	EM_ObjValueChange_Plot_ClientAuto	,	//Client自動劇情( "名稱" , 0 )
	EM_ObjValueChange_Plot_Collision	,	//碰撞劇情		( "名稱" , 0 )

	EM_ObjValueChange_CursorType		,	//游標			( "" , 游標類型 )

};
//Local -> Client
//通知周圍有某物件 的數值改變
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
//轉送周圍Client(小封包) 
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
//轉送周圍Client ( 小封包 )
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
//劇情觸發通知Client的事件
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
//設定角色數值
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
//設定角色數值
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
//通知Client 某個區域開啟
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
//設定[新]頭銜系統的頭銜
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

//設定[新]頭銜系統的頭銜
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

//使用[新]頭銜系統的物品
//Client -> Local
struct PG_Other_CtoL_NewTitleSys_UseItem
{
	GamePGCommandEnum Command;
	int		PosType;		// 0 身體 1 銀行 2 EQ
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
//使用[新]頭銜系統的物品result
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


//要求某區域執行PCall
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
//開啟Session
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
//關閉Session
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
//Session資訊
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
//開啟Session
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
//關閉Session
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
//修改AcountFlag
//Local -> Client
struct PG_Other_LtoC_FixAccountFlag
{
	GamePGCommandEnum Command;

	StaticFlag< _ACCOUNT_FLAG_COUNT_ >	AccountFlag;		//帳號資料旗標

	PG_Other_LtoC_FixAccountFlag()
	{
		Command = EM_PG_Other_LtoC_FixAccountFlag;
	}
};
//------------------------------------------------------------------------------------------------------------
//client 自動回報有問題的玩家
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
//今天的複本
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
//要求今天的複本
struct PG_Other_CtoL_WeekInstancesRequest
{
	GamePGCommandEnum Command;

	PG_Other_CtoL_WeekInstancesRequest()
	{
		Command = EM_PG_Other_CtoL_WeekInstancesRequest;
	}
};

//Local -> Client
//今天的複本
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
//要求傳送到某副本
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
//要求結果
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
//要求重置某副本
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
//要求重置某副本
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
//重置副本傳送次數
struct PG_Other_CtoL_WeekInstancesReset
{
	GamePGCommandEnum Command;

	PG_Other_CtoL_WeekInstancesReset()
	{
		Command = EM_PG_Other_CtoL_WeekInstancesReset;
	}
};

//Local -> Client
//重置副本傳送次數結果
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
//複本剩下的生存時間
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
//魔靈升階(或產生)
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
//魔靈升階結果
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
//魔靈升級
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
//魔靈升級結果
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
//魔靈裝備
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
//魔靈裝備結果
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
//修正魔靈裝備
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
//修正魔靈資料
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
//開啟幻靈介面
struct PG_Other_LtoC_OpenPhantomFrame
{
	GamePGCommandEnum Command;

	PG_Other_LtoC_OpenPhantomFrame()
	{
		Command = EM_PG_Other_LtoC_OpenPhantomFrame;
	}
};

//Local -> Client
//開啟鏡世界副本介面
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
//後台Buff設定
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
//後台魔幻寶盒資料
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
//後台魔幻寶盒索引ID
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
//後台魔幻寶盒資料
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
