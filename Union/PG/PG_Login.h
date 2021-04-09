#pragma once
#ifndef __PG_LOGIN_H__
#define __PG_LOGIN_H__

#include "RoleData\RoleData.h"
#include "NetWakerPGEnum.h"

//EM_PG_NEWCONNECT_M2D_XXX

//
//	Client Logout 通知 DB CENTER
//	Request Account's ROLE 資料, 
//

//----------------------------------------------------------------------------------------
enum LBRequestResultENUM
{
	EM_LBRequestResult_Unknow		= -1,
	EM_LBRequestResult_OK			= 0,
	EM_LBRequestResult_NotExist		,	//物品不存在
	EM_LBRequestResult_Requested	,	//已經被領取過
	EM_LBRequestResult_ItemDataError,	//物品資料有誤
	EM_LBRequestResult_BagFull		,	//物品暫存空間已滿
	EM_LBRequestResult_Expired		,	//過期
	EM_LBRequestResult_Constrained	,	//不符合限制條件
};

//----------------------------------------------------------------------------------------
enum LBWhoENUM
{
	EM_LB_Who_Role	 ,
	EM_LB_Who_Account,	
};

//----------------------------------------------------------------------------------------
//儲存的內容物的類型
enum LBGoodsTypeENUM
{
	EM_LBGoodsType_None	 = 0,	//無
	EM_LBGoodsType_Item  = 1,	//標準物品
	EM_LBGoodsType_Boost = 2,	//Buff
};

//---------------------------------------------------------------------------------------------------------
enum LBConstraintTypeENUM
{
	EM_LBConstraintType_None		,
	EM_LBConstraintType_OnlineTime	,
	EM_LBConstraintType_LoginDays	,
};

//----------------------------------------------------------------------------------------
enum LBStatusENUM
{
	EM_LBStatus_Normal	  = 0,	//正常
	EM_LBStatus_DataError = 1,	//資料錯誤
};

//----------------------------------------------------------------------------------------
//領取狀態
enum LBBoxStatusENUM
{
	EM_LBBoxStatus_Avaliable	,	//可以領取
	EM_LBBoxStatus_Unavaliable	,	//不可領取
	EM_LBBoxStatus_Requested	,	//已領取
};

//----------------------------------------------------------------------------------------
struct LBGoods
{
	LBGoodsTypeENUM Type;

	union
	{
		int _Data[4];

		struct
		{			
			int OrgObjID;
			int Count;
		}Item;

		struct
		{
			int Count;
		}Money;

		struct
		{
			int MagicID;
			int MagicLv;
			int EffectTime;			//seconds
		}Boost;
	};

	LBGoods()
	{
		Init();
	}

	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};

//----------------------------------------------------------------------------------------
struct LBConstraint
{
	LBConstraintTypeENUM Type;

	union
	{
		int Value[4];

		struct
		{
			int Who;
			int	Time;
		}OnlineTime;

		struct
		{
			int Who;
			int Day;
		}LoginDays;
	};

	int	Remaining;

	LBConstraint()
	{
		Init();
	}

	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};

//----------------------------------------------------------------------------------------
struct LBBox
{
	int						  ID;
	LBWhoENUM				  ToWho;
	int						  StartDate;
	float					  EndDate;
	std::vector<LBGoods>	  Goods;
	std::vector<LBConstraint> Constraints;
	int						  Money;
	LBBoxStatusENUM			  Status;

	LBBox()
	{
		Init();
	}

	void Init()
	{
		ID = 0;
		ToWho = EM_LB_Who_Role;
		StartDate = 0;
		EndDate = 0;
		Goods.clear();
		Constraints.clear();
		Money = 0;
		Status = EM_LBBoxStatus_Avaliable;
	}
};

//---------------------------------------------------------------------------------------------------------
struct PG_LBBox
{
	int				ID;
	LBWhoENUM		ToWho;
	int				StartDate;
	int				EndDate;
	int				GoodsCount;
	int				ConstraintCount;
	int				Money;
	LBBoxStatusENUM	Status;
	//LBGoods		Goods[];
	//LBConstraint	Constraints[];

	PG_LBBox()
	{
		Init();
	}

	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};

//----------------------------------------------------------------------------------------
struct PG_Login_M2C_IdentifyMaster
{
	PACKET_CONSTRUCT( PG_Login_M2C_IdentifyMaster );
};
//----------------------------------------------------------------------------------------
// Master Server -> DataCenter
// 要求玩家帳號角色資料

struct PG_Login_M2D_LoadAccountData
{	
	PACKET_CONSTRUCT( PG_Login_M2D_LoadAccountData );

	int						iClientID;		// MasterServer 本地端所連接的 ClientID
	char					szAccountName[ _MAX_ACCOUNT_SIZE_ ];
};
//----------------------------------------------------------------------------------------
struct PG_Login_M2D_SetCheckSecondPasswordResult
{	
	PACKET_CONSTRUCT( PG_Login_M2D_SetCheckSecondPasswordResult );

	int						iClientID;		// MasterServer 本地端所連接的 ClientID
	char					szAccountName[ _MAX_ACCOUNT_SIZE_ ];
	int						iResult;
};
//----------------------------------------------------------------------------------------
// DateCenter -> Master Server
// 資料庫在處理角色帳號時發生錯誤

enum EM_LoadAccountFailed
{
	EM_LoadAccountFailed_Err		= 0,
	EM_LoadAccountFailed_Exist		= -1,
};

struct PG_Login_D2M_LoadAccountFailed
{
	GamePGCommandEnum		Command;

	EM_LoadAccountFailed	emErrID;
	int						iClientID;

	PG_Login_D2M_LoadAccountFailed()
	{
		Command = EM_PG_Login_D2M_LoadAccountFailed;
	}
};

//----------------------------------------------------------------------------------------
// DateCenter -> Master Server
// 傳送玩家角色資料，一次一個角色資料，最後一筆Character.Type == ENUM_ObjectType_NULL
//----------------------------------------------------------------------------------------
struct PG_Login_D2M_RoleData
{
	GamePGCommandEnum					Command;

	int									iClientID;
	int									iDBID;								// 角色在資料庫中的索引號

	//RoleData				Role;
	SysBaseRoleData 					Base;
	SelfRoleData						SelfData;							//LocalServer 專用
	BaseRoleData						BaseData;
	PlayerSelfRoleData					PlayerSelfData;
	PlayerBaseRoleData	 				PlayerBaseData;
	
	PG_Login_D2M_RoleData()			{ Command = EM_PG_Login_D2M_RoleData; }
};


struct PG_Login_D2M_RoleDataCount
{
	GamePGCommandEnum					Command;
	int									iClientID;
	int									iRoleCount;

	StaticString<_MAX_ACCOUNT_SIZE_>    Account_ID;

	PlayerAccountBaseInfoStruct AccountBaseInfo;

	PG_Login_D2M_RoleDataCount()		{ Command = EM_PG_Login_D2M_RoleDataCount; }
};

enum EM_LoginResult
{
	EM_LoginResult_OK							= 0,
	EM_LoginResult_AccountExist					= -1,       // 帳號使用中
	EM_LoginResult_Error						= -2,		// 不知名的錯誤
	EM_LoginResult_AccountError					= -3,		// 帳號比對不一致
};

struct PG_Login_M2C_LoginResult
{
	GamePGCommandEnum		Command;
	EM_LoginResult			emResult;

	PG_Login_M2C_LoginResult()
	{
		Command = EM_PG_Login_M2C_LoginResult;
	}
};

struct PG_Login_C2M_LoadRoleData
{
	GamePGCommandEnum		Command;
	char					szAccountName[ _MAX_ACCOUNT_SIZE_ ];
	int						PlayTimeQuota;

	PG_Login_C2M_LoadRoleData()
	{
		Command = EM_PG_Login_C2M_LoadRoleData;
	}
};

//----------------------------------------------------------------------------------------
// Master Server -> Client
// 傳送玩家角色資料，一次一個角色資料，最後一筆Character.Type == ENUM_ObjectType_NULL
//----------------------------------------------------------------------------------------

struct PG_Login_M2C_SampleRoleData
{
	GamePGCommandEnum		Command;
	int						iDBID;
	SampleRoleData			SampleRole;

	PG_Login_M2C_SampleRoleData()
	{
		Command = EM_PG_Login_M2C_SampleRoleData;
	}
};

struct PG_Login_M2C_SampleRoleDataEnd
{
	GamePGCommandEnum		Command;
	int						ParallelZoneCount;	//平行的Zone數量

	PG_Login_M2C_SampleRoleDataEnd()
	{
		Command = EM_PG_Login_M2C_SampleRoleDataEnd;
	}
};



//----------------------------------------------------------------------------------------
// Client -> MasterServer
// Client 創角

struct CreateRoleData
{
	SampleRoleData		SampleRole;
};

struct PG_Login_C2M_CreateRole
{
	GamePGCommandEnum	Command;

	int					iFieldIndex;	// 第幾個欄位
	//RoleData			Role;			// 角色資料

	CreateRoleData		CreateRole;


	char				szAccountName[_MAX_ACCOUNT_SIZE_];
	//char				RoleName[_MAX_NAMERECORD_SIZE_];

	PG_Login_C2M_CreateRole()
	{
		Command = EM_PG_Login_C2M_CreateRole;
	}
};


struct PG_Login_M2D_CreateRole
{
	GamePGCommandEnum	Command;

	int					iClientID;		// 幫助 MasterServer 處理流程所留下的資料

	PlayerRoleData		Role;			// 角色資料
	char				szAccountName[_MAX_ACCOUNT_SIZE_];

	PG_Login_M2D_CreateRole()
	{
		Command = EM_PG_Login_M2D_CreateRole;
	}
};

//----------------------------------------------------------------------------------------
// Client -> MasterServer
// Client 創角結果

enum EM_CreateRoleResult
{
	EM_CreateRoleResult_OK						= 0,
	EM_CreateRoleResult_RoleNameExist			= -1,       //角色名稱重覆
    EM_CreateRoleResult_SystemError             = -2,
	EM_CreateRoleResult_EmptyRoleName			= -3,
	EM_CreateRoleResult_RoleNameSyntaxError		= -4,		//使用不可使用的語句	
	EM_CreateRoleResult_NameToShort				= -5,		// 名稱太短,不合法
	EM_CreateRoleResult_NameToLong				= -6,		// 名稱太長,不合法
	EM_CreateRoleResult_NameIsWrongful			= -7,		// 有錯誤的符號或字元
	EM_CreateRoleResult_HasSpecialString		= -8,		// 特殊名稱不能用
	EM_CreateRoleResult_OnlyBasicLetters		= -9,		// 只能使用基本英文字母
	EM_CreateRoleResult_EndLetterError			= -10,		// 結尾字元錯誤(',")
	EM_CreateRoleResult_NewHandDisableCreateRole = -11,		// 此區域新手不能創角
};

struct PG_Login_M2C_CreateRoleResult
{
	GamePGCommandEnum	Command;

	int					iDBID;
	EM_CreateRoleResult emResult;
	SampleRoleData		SampleRole;
	int					GiftItemID[5];
	int					GiftItemCount[5];

	PG_Login_M2C_CreateRoleResult()	{ Command = EM_PG_Login_M2C_CreateRoleResult; }
};

struct PG_Login_D2M_CreateRoleResult
{
	GamePGCommandEnum	Command;


	char				RoleName[_MAX_NAMERECORD_SIZE_];
	int					iClientID;
	int					iDBID;
	EM_CreateRoleResult emResult;
	int					GiftItemID[5];
	int					GiftItemCount[5];
	int					Job;
	int					Job_Sub;
	int					Lv;
	int					Lv_Sub;

	PG_Login_D2M_CreateRoleResult() { Command = EM_PG_Login_D2M_CreateRoleResult; }
};

//----------------------------------------------------------------------------------------
// Client -> MasterServer
// Client 刪角

struct PG_Login_C2M_DeleteRole
{
	GamePGCommandEnum	Command;
	char				szAccountName[_MAX_ACCOUNT_SIZE_];
	int					iDBID;	// 刪除的角色資料 DBID
	bool				IsImmediately;	//立即刪除
	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

	PG_Login_C2M_DeleteRole()
	{
		Command = EM_PG_Login_C2M_DeleteRole;
	}
};

struct PG_Login_M2D_DeleteRole
{
	GamePGCommandEnum	Command;
	//int					iFieldIndex;	// 第幾個欄位
	//RoleData			Role;			// 角色資料
	int					iClientID;
	char				szAccountName[_MAX_ACCOUNT_SIZE_];
	char				szRoleName[_MAX_NAMERECORD_SIZE_];
	int					iDBID;
	bool				IsImmediately;

	PG_Login_M2D_DeleteRole()
	{
		Command = EM_PG_Login_M2D_DeleteRole;
	}
};

//----------------------------------------------------------------------------------------
// Client -> MasterServer
// Client 刪角結果

enum EM_DeleteRoleResult
{
	EM_DeleteRole_OK						= 0,
	EM_RoleNotExist							= -1,
    EM_DeleteSysErr                         = -2,
	EM_DeletePasswordErr                    = -3,
};

struct PG_Login_M2C_DeleteRoleResult
{
	GamePGCommandEnum	Command;
	EM_DeleteRoleResult emResult;		// 刪除角色的結果
	int					iDBID;			// 角色所在的欄位
	//char				szRoleName[_MAX_NAMERECORD_SIZE_];

	PG_Login_M2C_DeleteRoleResult()
	{
		Command = EM_PG_Login_M2C_DeleteRoleResult;
	}
};

struct PG_Login_D2M_DeleteRoleResult
{
	GamePGCommandEnum	Command;
	EM_DeleteRoleResult emResult;
	int					iDBID;
	//int				iFieldIndex;
	//char				szRoleName[_MAX_NAMERECORD_SIZE_];
	//char				szAccountName[_MAX_ACCOUNT_SIZE_];
	int					iClientID;

	PG_Login_D2M_DeleteRoleResult()
	{
		Command = EM_PG_Login_D2M_DeleteRoleResult;
	}
};



//----------------------------------------------------------------------------------------
// 將角色註冊進入世界

struct PG_Login_M2L_SetRoleToWorld
{
	GamePGCommandEnum		Command;
	int						iClientID;
	PlayerRoleData			Role;

	PG_Login_M2L_SetRoleToWorld()
	{
		Command = EM_PG_Login_M2L_SetRoleToWorld;
	}
};

//----------------------------------------------------------------------------------------
// 將角色註冊進入世界

enum EM_SelectRoleResult
{
	EM_SelectRole_OK					= 0,
	EM_SelectRole_No_This				= -1,
	EM_SelectRole_ZoneNotExist			= -2,
	EM_SelectRole_Parallel_Zone_Full_	= -3,		//此平行空間已滿
	EM_SelectRole_NoSecondPassword_		= -4,		//沒設定2次密碼
	EM_SelectRole_ServerShutdown_		= -5,		//Server 正在關機
	EM_SelectRole_NeedChangeRoleName_	= -6,		//需修改角色名字
	EM_SelectRole_SecondPasswordErr_	= -7,		//二次密碼錯誤
};

struct PG_Login_L2M_SetRoleToWorldResult
{
	GamePGCommandEnum			Command;
	int							iClientID;
	int							iDBID;	

	EM_SelectRoleResult			emResult;

	PG_Login_L2M_SetRoleToWorldResult()
	{
		Command = EM_PG_Login_L2M_SetRoleToWorldResult;
	}
};
//----------------------------------------------------------------------------------------
// Client -> MasterServer
// Client 選擇登入遊戲的角色

struct PG_Login_C2M_SelectRole
{
	GamePGCommandEnum	Command;
	int					iDBID;	// 第幾個欄位
	char				szAccountName[_MAX_ACCOUNT_SIZE_];
	int					ParallelZoneID;

	PG_Login_C2M_SelectRole()
	{
		Command = EM_PG_Login_C2M_SelectRole;
	}
};
//----------------------------------------------------------------------------------------
// Client -> MasterServer
// Client 選擇登入遊戲的角色

struct PG_Login_C2M_SelectRole_EnterZone1
{
	GamePGCommandEnum	Command;
	int					iDBID;	// 第幾個欄位
	char				szAccountName[_MAX_ACCOUNT_SIZE_];
	int					ParallelZoneID;

	PG_Login_C2M_SelectRole_EnterZone1()
	{
		Command = EM_PG_Login_C2M_SelectRole_EnterZone1;
	}
};

//----------------------------------------------------------------------------------------
// MasterServer -> Client
// 回傳登入遊戲的角色結果

struct PG_Login_M2C_SelectRoleResult
{
	GamePGCommandEnum	Command;
	EM_SelectRoleResult	emResult;

	int					iDBID;	
	

	PG_Login_M2C_SelectRoleResult()
	{
		Command = EM_PG_Login_M2C_SelectRoleResult;
	}
};

//----------------------------------------------------------------------------------------
// Client -> Master 要求改名
struct PG_Login_C2M_ChangeNameRequest
{
	GamePGCommandEnum	Command;
	
	int					DBID;	
	char				RoleName[_MAX_NAMERECORD_SIZE_];	

	PG_Login_C2M_ChangeNameRequest()
	{
		Command = EM_PG_Login_C2M_ChangeNameRequest;
	}
};
//----------------------------------------------------------------------------------------
// Master -> PlayerCenter 要求改名
struct PG_Login_M2D_ChangeNameRequest
{
	GamePGCommandEnum	Command;

	int					CliNetID;
	int					DBID;	
	char				RoleName[_MAX_NAMERECORD_SIZE_];	

	PG_Login_M2D_ChangeNameRequest()
	{
		Command = EM_PG_Login_M2D_ChangeNameRequest;
	}
};
//----------------------------------------------------------------------------------------
enum ChangeNameResultENUM
{
	EM_ChangeNameResult_OK			,
	EM_ChangeNameResult_Failed		,	//不可修改
	EM_ChangeNameResult_NameErr		,	//名稱錯誤
	EM_ChangeNameResult_NameUsed	,	//名稱重復
};
// PlayerCenter -> Master 要求改名結果
struct PG_Login_D2M_ChangeNameResult
{
	GamePGCommandEnum	Command;

	int						CliNetID;
	int						DBID;	
	char					RoleName[_MAX_NAMERECORD_SIZE_];	
	ChangeNameResultENUM	Result;

	PG_Login_D2M_ChangeNameResult()
	{
		Command = EM_PG_Login_D2M_ChangeNameResult;
	}
};
//----------------------------------------------------------------------------------------
// Master -> Client 要求改名結果
struct PG_Login_M2C_ChangeNameResult
{
	GamePGCommandEnum	Command;

	int						DBID;	
	char					RoleName[_MAX_NAMERECORD_SIZE_];	
	ChangeNameResultENUM	Result;

	PG_Login_M2C_ChangeNameResult()
	{
		Command = EM_PG_Login_M2C_ChangeNameResult;
	}
};

//----------------------------------------------------------------------------------------
// Master Server -> Client
// 角色進入及離開了世界

struct PG_Login_M2C_EnterWorld
{	
	PACKET_CONSTRUCT( PG_Login_M2C_EnterWorld );
};

struct PG_Login_M2C_LeaveWorld
{	
	PACKET_CONSTRUCT( PG_Login_M2C_LeaveWorld );

	int					iDBID;
};
//----------------------------------------------------------------------------------------
enum EM_LoadDataEvent
{
	EM_LoadDataEvent_StartLoad					= 0,
	EM_LoadDataEvent_WaitingQueue				= 1,		// 排隊中
	EM_LoadDataEvent_RoleExist					= 2,       // 帳號中有角色仍未存檔
	EM_LoadDataEvent_Error						= 3,
	EM_LoadDataEvent_Lock						= 4,		// 鎖定一定時間不能登辱
	EM_LoadDataEvent_SPWCheckFailed				= 5,		// 二次密碼錯誤
};

struct PG_Login_M2C_LoadDataEvent
{
	PACKET_CONSTRUCT( PG_Login_M2C_LoadDataEvent );

	EM_LoadDataEvent		emEvent;
	int						iValue;

};
//----------------------------------------------------------------------------------------
//解除刪角
//Client - > Master
struct PG_Login_C2M_DelRoleRecoverRequest
{
	PACKET_CONSTRUCT( PG_Login_C2M_DelRoleRecoverRequest );

	int		DBID;
};
//----------------------------------------------------------------------------------------
//解除刪角
//Master - > DataCenter
struct PG_Login_M2D_DelRoleRecoverRequest
{
	PACKET_CONSTRUCT( PG_Login_M2D_DelRoleRecoverRequest );
	int		DBID;
	int		NetID;
	int		ProxyID;
};
//----------------------------------------------------------------------------------------
//解除刪角結果
//DataCenter -> Client
struct PG_Login_D2C_DelRoleRecoverResult
{
	PACKET_CONSTRUCT( PG_Login_D2C_DelRoleRecoverResult );
	int		DBID;
	bool	Result;
};
//----------------------------------------------------------------------------------------
//要求設定2次密碼
//Master -> Client
struct PG_Login_M2C_SetSecondPasswordRequest
{
	PACKET_CONSTRUCT( PG_Login_M2C_SetSecondPasswordRequest );
};
//----------------------------------------------------------------------------------------
//要求設定2次密碼
//Client -> Master
struct PG_Login_C2M_SetSecondPassword
{
	PACKET_CONSTRUCT( PG_Login_C2M_SetSecondPassword );
	char		Password[33];
};
//----------------------------------------------------------------------------------------
//要求設定2次密碼
//Client -> Master
struct PG_Login_M2D_SetSecondPassword
{
	PACKET_CONSTRUCT( PG_Login_M2D_SetSecondPassword );
	char		Account[_DEF_MAX_ACCOUNTNAME_SIZE_];
	char		Password[33];
};
//----------------------------------------------------------------------------------------
//取得某區域各平行空間的Loading
//Client -> Master
struct PG_Login_C2M_ZoneLoadingRequest
{
	PACKET_CONSTRUCT( PG_Login_C2M_ZoneLoadingRequest );
	int		ZoneID;	
};
//----------------------------------------------------------------------------------------
//取得某區域各平行空間的Loading
// Master -> Client
struct PG_Login_M2C_ZoneLoading
{
	PACKET_CONSTRUCT( PG_Login_M2C_ZoneLoading );
	int		ZoneID;	
	int		MaxZoneCount;
	int		ParallelCount;
	int		Count[ 100 ];
};
//----------------------------------------------------------------------------------------
/*
struct PG_Login_M2L_BTIPlayTime
{
	//PACKET_CONSTRUCT( PG_Login_M2L_BTIPlayTime );
	
	int		iRoleDBID;		// 登入的角色
	int		iHours;			// 玩的時間
};
*/
struct PG_Nonage_M2L_HealthTime
{
	PACKET_CONSTRUCT( PG_Nonage_M2L_HealthTime );

	int		iRoleDBID;		// 登入的角色
	int		iHealthTime;			// 玩的時間
};
//----------------------------------------------------------------------------------------
struct PG_Login_M2C_APEXTOCLIENT
{
	PACKET_CONSTRUCT( PG_Login_M2C_APEXTOCLIENT );

	int		iSize;
	BYTE*	pData;
};
//----------------------------------------------------------------------------------------
struct PG_Login_C2M_APEXTOSERVER
{
	PACKET_CONSTRUCT( PG_Login_C2M_APEXTOSERVER );

	int		iSize;
	BYTE*	pData;
};
//----------------------------------------------------------------------------------------
struct PG_Login_APEX2C_APEXSERVERID
{
	PACKET_CONSTRUCT( PG_Login_APEX2C_APEXSERVERID );
};
//----------------------------------------------------------------------------------------
struct PG_Login_C2APEX_APEXSTARTVALUE
{
	PACKET_CONSTRUCT( PG_Login_C2APEX_APEXSTARTVALUE );
	
	int		iReturn;
};

//------------------------------------------------------------------------------------------------------------
//關閉MasterServer登入机制
//Local -> Master
struct PG_Login_LtoM_CloseMaster
{
	GamePGCommandEnum	Command;

	PG_Login_LtoM_CloseMaster()
	{
		Command = EM_PG_Login_LtoM_CloseMaster;
	}   
};
//------------------------------------------------------------------------------------------------------------
//通知最多可以建立幾個角色
//Master -> Client
struct PG_Login_MtoC_MaxRoleCount
{
	GamePGCommandEnum	Command;
	int		Count;

	PG_Login_MtoC_MaxRoleCount()
	{
		Command = EM_PG_Login_MtoC_MaxRoleCount;
	}   
};
//------------------------------------------------------------------------------------------------------------
//通知Client 保留角色資料
//Master -> PlayerCenter
struct PG_Login_MtoD_ReserveRoleRequest
{
	GamePGCommandEnum	Command;
	StaticString< _MAX_ACCOUNT_SIZE_ > Account_ID;
	int					CliNetID;
	int					CliProxyID;

	PG_Login_MtoD_ReserveRoleRequest()
	{
		Command = EM_PG_Login_MtoD_ReserveRoleRequest;
	}   
};
//------------------------------------------------------------------------------------------------------------
//通知Client 保留角色資料
struct PG_Login_DtoC_ReserveRole
{
	GamePGCommandEnum	Command;
	ReserverRoleStruct	Info;

	PG_Login_DtoC_ReserveRole()
	{
		Command = EM_PG_Login_DtoC_ReserveRole;
	}   
};
//------------------------------------------------------------------------------------------------------------
//要求2次密碼
//Master -> Client
struct PG_Login_MtoC_SecondPasswordRequest
{
	GamePGCommandEnum	Command;

	PG_Login_MtoC_SecondPasswordRequest()
	{
		Command = EM_PG_Login_MtoC_SecondPasswordRequest;
	}   
};
//------------------------------------------------------------------------------------------------------------
//要求2次密碼
//Master -> Client
struct PG_Login_CtoM_SecondPassword
{
	GamePGCommandEnum	Command;
	char				Password[33];

	PG_Login_CtoM_SecondPassword()
	{
		Command = EM_PG_Login_CtoM_SecondPassword;
	}   
};
//------------------------------------------------------------------------------------------------------------
//要求2次密碼
//Master -> Client
struct PG_Login_MtoC_SecondPasswordResult
{
	GamePGCommandEnum	Command;
	bool Result;

	PG_Login_MtoC_SecondPasswordResult()
	{
		Command = EM_PG_Login_MtoC_SecondPasswordResult;
	}
};

//------------------------------------------------------------------------------------------------------------
//要求2次密碼 version 2
//Master -> Client
struct PG_Login_MtoC_SecondPasswordRequest2
{
	GamePGCommandEnum	Command;

	PG_Login_MtoC_SecondPasswordRequest2()
	{
		Command = EM_PG_Login_MtoC_SecondPasswordRequest2;
	}
};

//------------------------------------------------------------------------------------------------------------
//2次密碼回覆 version 2
//Client -> Master
struct PG_Login_CtoM_SecondPassword2
{
	GamePGCommandEnum	Command;
	char				Password[33];

	PG_Login_CtoM_SecondPassword2()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_Login_CtoM_SecondPassword2;
	}   
};

//------------------------------------------------------------------------------------------------------------
//2次密碼檢查結果 version 2
//Master -> Client
struct PG_Login_MtoC_SecondPasswordResult2
{
	GamePGCommandEnum	Command;
	bool Result;

	PG_Login_MtoC_SecondPasswordResult2()
	{
		Command = EM_PG_Login_MtoC_SecondPasswordResult2;
	}
};

//------------------------------------------------------------------------------------------------------------
//通知Client有圖形驗證碼
//Master -> Client
struct PG_Login_MtoC_CaptchaCheck
{
	GamePGCommandEnum	Command;
	int CaptchaSize;

	PG_Login_MtoC_CaptchaCheck()
	{
		Command = EM_PG_Login_MtoC_CaptchaCheck;
		CaptchaSize = 0;
	}
};

//------------------------------------------------------------------------------------------------------------
//更新圖形驗證碼
//Client -> Master
struct PG_Login_CtoM_CaptchaRefresh
{
	GamePGCommandEnum	Command;

	PG_Login_CtoM_CaptchaRefresh()
	{
		Command = EM_PG_Login_CtoM_CaptchaRefresh;
	}
};

//------------------------------------------------------------------------------------------------------------
//回覆圖形驗證碼
//Client -> Master
struct PG_Login_CtoM_CaptchaReply
{
	GamePGCommandEnum	Command;
	char				Captcha[9];

	PG_Login_CtoM_CaptchaReply()
	{
		Command = EM_PG_Login_CtoM_CaptchaReply;
	}
};


//------------------------------------------------------------------------------------------------------------
//圖形驗證碼檢查結果
//Master -> Client
enum EM_CaptchaCheckResult
{
	EM_CaptchaCheckResult_OK			= 0,
	EM_CaptchaCheckResult_Failed		= 1,
	EM_CaptchaCheckResult_TimeExpired	= 2
};

struct PG_Login_MtoC_CaptchaCheckResult
{
	GamePGCommandEnum	Command;
	int Result;

	PG_Login_MtoC_CaptchaCheckResult()
	{
		Command = EM_PG_Login_MtoC_CaptchaCheckResult;
	}
};

//------------------------------------------------------------------------------------------------------------
//通知Client 保留角色資料
struct PG_Login_MtoP_GetObjCount
{
	GamePGCommandEnum	Command;

	PG_Login_MtoP_GetObjCount()
	{
		Command = EM_PG_Login_MtoP_GetObjCount;
	}   
};
//------------------------------------------------------------------------------------------------------------
struct PG_Login_PtoM_GetObjCountResult
{
	GamePGCommandEnum	Command;
	int					iCount;

	PG_Login_PtoM_GetObjCountResult()
	{
		Command = EM_PG_Login_PtoM_GetObjCountResult;
	}   
};
//------------------------------------------------------------------------------------------------------------
struct PG_Login_MtoC_SetGameGuard
{
	GamePGCommandEnum	Command;
	int					iEnable;

	PG_Login_MtoC_SetGameGuard()
	{
		Command = EM_PG_Login_MtoC_SetGameGuard;
	}   
};
//------------------------------------------------------------------------------------------------------------
struct PG_Login_CtoM_GameGuardReport
{
	GamePGCommandEnum		Command;
	int						iType;
	int						iSize;
	StaticString< 4000 >	Report;


	PG_Login_CtoM_GameGuardReport()
	{
		Command = EM_PG_Login_CtoM_GameGuardReport;
	}   
};
//------------------------------------------------------------------------------------------------------------
struct PG_Login_MtoC_VivoxAccount
{
	GamePGCommandEnum					Command;

	StaticString< _MAX_ACCOUNT_SIZE_ >	Account;
	StaticString< 33 >					Password;
	bool								bCreateResult;

	PG_Login_MtoC_VivoxAccount()
	{
		Command = EM_PG_Login_MtoC_VivoxAccount;
	}   
};
//------------------------------------------------------------------------------------------------------------
struct PG_Login_MtoC_VivoxInfo
{
	GamePGCommandEnum					Command;

	StaticString< 256 >					Url;
	StaticString< 33 >					CountryCode;


	PG_Login_MtoC_VivoxInfo()
	{
		Command = EM_PG_Login_MtoC_VivoxInfo;
	}   
};
//------------------------------------------------------------------------------------------------------------
struct PG_Login_CtoM_CreateVivoxAccount
{
	GamePGCommandEnum					Command;

	StaticString< _MAX_ACCOUNT_SIZE_ >	Account;

	PG_Login_CtoM_CreateVivoxAccount()
	{
		Command = EM_PG_Login_CtoM_CreateVivoxAccount;
	}   
};

//----------------------------------------------------------------------------------------
//Client -> Local 要求獎勵列表
struct PG_Login_CtoL_RequestBonusList
{
	GamePGCommandEnum Command;

	int Page;

	PG_Login_CtoL_RequestBonusList()
	{
		Command = EM_PG_Login_CtoL_RequestBonusList;
	}
};

//----------------------------------------------------------------------------------------
//Client -> Local 領取獎勵
struct PG_Login_CtoL_RequestBonus
{
	GamePGCommandEnum Command;

	int	ID;

	PG_Login_CtoL_RequestBonus()
	{
		Command = EM_PG_Login_CtoL_RequestBonus;
	}
};

//----------------------------------------------------------------------------------------
//Local -> DataCenter 要求獎勵列表
struct PG_Login_LtoD_RequestBonusList
{
	GamePGCommandEnum Command;

	StaticString<_DEF_MAX_ACCOUNTNAME_SIZE_> Account;
	int										 PlayerDBID;
	int										 Page;

	PG_Login_LtoD_RequestBonusList()
	{
		Command = EM_PG_Login_LtoD_RequestBonusList;
	}
};

//----------------------------------------------------------------------------------------
//Local -> DataCenter 領取獎勵
struct PG_Login_LtoD_RequestBonus
{
	GamePGCommandEnum Command;

	StaticString<_DEF_MAX_ACCOUNTNAME_SIZE_> Account;
	int										 PlayerDBID;
	int										 ID;

	PG_Login_LtoD_RequestBonus()
	{
		Command = EM_PG_Login_LtoD_RequestBonus;
	}
};

//----------------------------------------------------------------------------------------
//Local -> DataCenter 領取獎勵完成
struct PG_Login_LtoD_RequestBonusComplete
{
	GamePGCommandEnum Command;

	StaticString<_DEF_MAX_ACCOUNTNAME_SIZE_> Account;
	int										 PlayerDBID;
	int										 ID;
	LBRequestResultENUM						 Result;

	PG_Login_LtoD_RequestBonusComplete()
	{
		Command = EM_PG_Login_LtoD_RequestBonusComplete;
	}
};

//----------------------------------------------------------------------------------------
//Local -> DataCenter client connect
struct PG_Login_LtoD_CliConnect
{
	GamePGCommandEnum Command;

	StaticString<_DEF_MAX_ACCOUNTNAME_SIZE_> Account;
	int										 PlayerDBID;

	PG_Login_LtoD_CliConnect()
	{
		Command = EM_PG_Login_LtoD_CliConnect;
	}
};

//----------------------------------------------------------------------------------------
//Local -> DataCenter client disconnect
struct PG_Login_LtoD_CliDisconnect
{
	GamePGCommandEnum Command;

	StaticString<_DEF_MAX_ACCOUNTNAME_SIZE_> Account;
	int										 PlayerDBID;

	PG_Login_LtoD_CliDisconnect()
	{
		Command = EM_PG_Login_LtoD_CliDisconnect;
	}
};

//----------------------------------------------------------------------------------------
//DataCenter -> Local 獎勵列表
struct PG_Login_DtoL_BonusList
{
	GamePGCommandEnum Command;

	int		   PlayerDBID;
	int		   Year;
	int		   Month;
	int		   MDay;
	int		   Page;
	int		   DescriptionSize;
	int		   BoxCount;
	//wchar_t  Description[]
	//PG_LBBox Boxes[]

	PG_Login_DtoL_BonusList()
	{
		Command = EM_PG_Login_DtoL_BonusList;
	}
};

//----------------------------------------------------------------------------------------
//DataCenter -> Local 領取獎勵結果
struct PG_Login_DtoL_RequestBonusResult
{
	GamePGCommandEnum Command;

	StaticString<_DEF_MAX_ACCOUNTNAME_SIZE_> Account;
	int										 PlayerDBID;
	int										 ID;
	int										 Money;
	int										 Count;
	LBRequestResultENUM						 Result;
	//LBGoods								 Goods[];

	PG_Login_DtoL_RequestBonusResult()
	{
		Command = EM_PG_Login_DtoL_RequestBonusResult;
	}
};

//----------------------------------------------------------------------------------------
//DataCenter -> Local 獎勵時間
struct PG_Login_DtoL_BonusPeriod
{
	GamePGCommandEnum Command;

	int	PlayerDBID;
	int MaxPage;
	int MinPage;

	PG_Login_DtoL_BonusPeriod()
	{
		Command = EM_PG_Login_DtoL_BonusPeriod;
	}
};

//----------------------------------------------------------------------------------------
//Local -> Client 獎勵列表
struct PG_Login_LtoC_BonusList
{
	GamePGCommandEnum Command;

	int		   Year;
	int		   Month;
	int		   MDay;
	int		   Page;
	int		   DescriptionSize;
	int		   BoxCount;
	//wchar_t  Description[]
	//PG_LBBox Boxes[]

	PG_Login_LtoC_BonusList()
	{
		Command = EM_PG_Login_LtoC_BonusList;
	}
};

//----------------------------------------------------------------------------------------
//Local -> Client 領取獎勵結果
struct PG_Login_LtoC_RequestBonusResult
{
	GamePGCommandEnum Command;

	int					ID;
	LBRequestResultENUM	Result;

	PG_Login_LtoC_RequestBonusResult()
	{
		Command = EM_PG_Login_LtoC_RequestBonusResult;
	}
};

//----------------------------------------------------------------------------------------
//Local -> Client 獎勵時間
struct PG_Login_LtoC_BonusPeriod
{
	GamePGCommandEnum Command;

	int MaxPage;
	int MinPage;

	PG_Login_LtoC_BonusPeriod()
	{
		Command = EM_PG_Login_LtoC_BonusPeriod;
	}
};

#endif