#ifndef __PG_MOVE_H__
#define __PG_MOVE_H__
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged
//-----------------------------------------------------------------------------------------
// Local -> Client
// 通知Client物件產生

#define DF_TIME_PING	10					// 秒

struct PG_Move_LtoC_AddToPartition
{ 
	GamePGCommandEnum	Command;
	int					GItemID;			//物件號碼 (要求移動的物件)
	int					WorldGUID;
	//    myUInt32			LGItemID;           //上個區域的ID

	PlayerGraphStruct	GraphObj;			//顯像資料
	RolePosStruct       Pos;
	int					DBID;
	int					MasterGUID;			//主人的物件號碼
	CampID_ENUM			CampID;
	int					iWorldID;			// 物件所屬的世界代碼


	PG_Move_LtoC_AddToPartition()
	{ Command = EM_PG_Move_LtoC_AddToPartition; }
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// 通知Client物件消失
struct PG_Move_LtoC_DelFromPartition
{
	GamePGCommandEnum	Command;
	int					GItemID;				//物件號碼 (要求移動的物件)
	int					WorldGUID;

	PG_Move_LtoC_DelFromPartition()
	{ Command = EM_PG_Move_LtoC_DelFromPartition; }
};
//-----------------------------------------------------------------------------------------
struct LookEQListStruct
{
	union
	{
		int EQ[ 13 ];
		struct
		{
			int Head;        
			int Gloves;	   
			int Shoes;	
			int Clothes;	   
			int Pants;	
			int Back;		   
			int Belt;		   
			int Shoulder;	   
			int MainHand;	   
			int SecondHand;  
			int Bow;	
			int Manufacture;
			int	Ornament;
		};	
	};
};


struct LookEQInfoStruct
{
	LookEQListStruct	EQ;
	LookEQListStruct	MainColor;
	LookEQListStruct	OffColor;
	LookEQListStruct	Level;
};

struct PlayerEquipmentStruct
{
	LookEQInfoStruct	EQLook;
	ShowEQStruct		ShowEQ;		
};

//Local -> Client
struct PG_Move_LtoC_EQInfo
{
	GamePGCommandEnum	Command;
	int                 ItemID;
	LookEQInfoStruct    EQLook;
	ShowEQStruct		ShowEQ;

	PG_Move_LtoC_EQInfo()
	{ Command = EM_PG_Move_LtoC_EQInfo; }
};
//-----------------------------------------------------------------------------------------
struct PG_Move_LtoC_FixEQInfo
{
	GamePGCommandEnum	Command;
	int                 ItemID;
	EQWearPosENUM       Pos;
	int                 EQ;

	PG_Move_LtoC_FixEQInfo()
	{ Command = EM_PG_Move_LtoC_FixEQInfo; }
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// 通知Client物件重新定位
struct PG_Move_LtoC_SetPostion
{
	GamePGCommandEnum	Command;
	int					GItemID;				//物件號碼 (要求移動的物件)
	//    myUInt32				WorldGUID;
	RolePosStruct       Pos;
	//    float				X , Y , Z , Dir;

	PG_Move_LtoC_SetPostion()
	{ Command = EM_PG_Move_LtoC_SetPostion; }
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// 通知Client物件移動
struct PG_Move_LtoC_ObjectMove
{
	GamePGCommandEnum	Command;
	int					GItemID;				//物件號碼 (要求移動的物件)
	//    myUInt32				WorldGUID;
	RolePosStruct       Pos;
	float               vX;	//玩家為移動向量，npc 目前位置
	float				vY;
	float               vZ;

	ClientMoveTypeENUM  Type;					//

	PG_Move_LtoC_ObjectMove()
	{ Command = EM_PG_Move_LtoC_ObjectMove; }
};

//Local -> Client
//玩家移動封包(縮減板)
struct PG_Move_LtoC_PlayerMove
{
	GamePGCommandEnum	Command;
	int					GItemID;				//物件號碼 (要求移動的物件)

	float				X , Y , Z;

	int			Dir  : 10;
	int			vX	 : 9;
	int			vZ	 : 9;
	int			Type : 4;

	PG_Move_LtoC_PlayerMove()
	{ Command = EM_PG_Move_LtoC_PlayerMove; }
};


//-----------------------------------------------------------------------------------------
// Client -> Local 
// 玩家控制物件移動的資訊
struct PG_Move_CtoL_PlayerMoveObject
{
	GamePGCommandEnum	Command;
	int					GItemID;				//物件號碼 (要求移動的物件)    
	RolePosStruct       Pos;
	ClientMoveTypeENUM  Type;					//
	float               vX;
	float               vY;
	float               vZ;			
	
//	float				GroundHeight;
	int					AttachObjID;			//所站的物件


	PG_Move_CtoL_PlayerMoveObject()
	{ Command = EM_PG_Move_CtoL_PlayerMoveObject; }
};

//-----------------------------------------------------------------------------------------
// Local -> Client
//通知 Client 做動作
struct PG_Move_LtoC_DoAction
{
	GamePGCommandEnum	Command;
	int					GItemID;				//物件號碼 (要求做動作的物件)
	int					ActionID;
	int		            TempActionID;

	PG_Move_LtoC_DoAction()
	{ Command = EM_PG_Move_LtoC_DoAction; }
};
//-----------------------------------------------------------------------------------------
// Client -> Server
//通知 Server 轉發做動作
struct PG_Move_CtoL_DoAction
{
	GamePGCommandEnum	Command;
	int					GItemID;				//物件號碼 (要求做動作的物件)
	int					ActionID;
	int	                TempActionID;              //是否暫時動做

	PG_Move_CtoL_DoAction()
	{ Command = EM_PG_Move_CtoL_DoAction; }
};
//-----------------------------------------------------------------------------------------
// Local -> Client
//設定移動速度
struct PG_Move_LtoC_MoveSpeed
{
	GamePGCommandEnum	Command;
	int					GItemID;				//物件號碼 (要求做動作的物件)
	//    myUInt32				WorldGUID;
	float				Speed;

	PG_Move_LtoC_MoveSpeed()
	{ Command = EM_PG_Move_LtoC_MoveSpeed; }
};
//-----------------------------------------------------------------------------------------
// Local -> Client
//設定 移動資料 重力與跳躍力..
struct PG_Move_LtoC_MoveInfo
{
	GamePGCommandEnum	Command;
	int					GItemID;				//物件號碼 (要求做動作的物件)
	
	float				Gravity;
	float				JumpRate;

	PG_Move_LtoC_MoveInfo()
	{ Command = EM_PG_Move_LtoC_MoveInfo; }
};

//-----------------------------------------------------------------------------------------
// Local -> Client 
// 放大倍率
struct PG_Move_LtoC_ModelRate
{
	GamePGCommandEnum	Command;
	int					GItemID;				//物件號碼 (要求做動作的物件)
	float				ModelRate;

	PG_Move_LtoC_ModelRate()
	{ Command = EM_PG_Move_LtoC_ModelRate; }
};
//-----------------------------------------------------------------------------------------
// Local -> Client
//通知兩者互相的關係
struct PG_Move_LtoC_Relation
{
	GamePGCommandEnum	Command;
	int					GItemID;				//物件號碼 
	RoleRelationStruct  Relation;

	PG_Move_LtoC_Relation()
	{ Command = EM_PG_Move_LtoC_Relation; }
};
//-----------------------------------------------------------------------------------------
//角色容貌微調
struct PG_Move_LtoC_LookFace
{
	GamePGCommandEnum	Command;
	int                 GItemID;
	LookStruct          Look;

	PG_Move_LtoC_LookFace()
	{ Command = EM_PG_Move_LtoC_LookFace; }
};
//-----------------------------------------------------------------------------------------
struct PG_Move_LtoC_ClientAutoPlot
{
	GamePGCommandEnum	Command;

	int		                                    GItemID;				//物件的區域
	StaticString< _MAX_LUA_FUNTIONNAMESIZE_ >   AutoPlot;

	PG_Move_LtoC_ClientAutoPlot()
	{
		Command = EM_PG_Move_LtoC_ClientAutoPlot;
	}
};
//-----------------------------------------------------------------------------------------
struct PG_Move_LtoC_ActionType
{
	GamePGCommandEnum	Command;
	int                 GItemID;
	RoleActionStruct	Action;			                //目前的行動狀態

	PG_Move_LtoC_ActionType()
	{
		Command = EM_PG_Move_LtoC_ActionType;
	}
};
//-----------------------------------------------------------------------------------------
//Client 通知 Server要做跳越
struct PG_Move_CtoL_JumpBegin
{
	GamePGCommandEnum	Command;
	int                 GItemID;
	float               X;
	float               Y;
	float               Z;
	float               Dir;

	float               vX;
	float               vY;
	float               vZ;


	PG_Move_CtoL_JumpBegin()
	{
		Command = EM_PG_Move_CtoL_JumpBegin;
	}
};
//-----------------------------------------------------------------------------------------
//Server 通知 周圍玩家某角色要做跳越
struct PG_Move_LtoC_JumpBegin
{
	GamePGCommandEnum	Command;
	int                 GItemID;
	float               X;
	float               Y;
	float               Z;
	float               Dir;

	float               vX;
	float               vY;
	float               vZ;


	PG_Move_LtoC_JumpBegin()
	{
		Command = EM_PG_Move_LtoC_JumpBegin;
	}
};
//-----------------------------------------------------------------------------------------
//Client 通知 Server跳越結束
struct PG_Move_CtoL_JumpEnd
{
	GamePGCommandEnum	Command;
	int                 GItemID;
	float               X;
	float               Y;
	float               Z;
	float               Dir;
	int					AttackObjID;


	PG_Move_CtoL_JumpEnd()
	{
		Command = EM_PG_Move_CtoL_JumpEnd;
	}
};
//-----------------------------------------------------------------------------------------
//Server 通知 Client跳越結束
struct PG_Move_LtoC_JumpEnd
{
	GamePGCommandEnum	Command;
	int                 GItemID;
	float               X;
	float               Y;
	float               Z;
	float               Dir;


	PG_Move_LtoC_JumpEnd()
	{
		Command = EM_PG_Move_LtoC_JumpEnd;
	}
};
//-----------------------------------------------------------------------------------------
//特殊移動
//Local -> Client
struct PG_Move_LtoC_PartyMemberMove
{
	GamePGCommandEnum	Command;
	int					DBID;
	//int					ZoneID;
	int					GItemID;
	float               X;
	float               Y;
	float               Z;

	PG_Move_LtoC_PartyMemberMove()
	{
		Command = EM_PG_Move_LtoC_PartyMemberMove;
	}
};
//-----------------------------------------------------------------------------------------
struct PG_Move_LtoC_PlayMotion
{
	PACKET_CONSTRUCT( PG_Move_LtoC_PlayMotion );

	int						iObjID;
	int						iMotionID;
};
//-----------------------------------------------------------------------------------------
struct PG_Move_LtoC_SetIdleMotion
{
	PACKET_CONSTRUCT( PG_Move_LtoC_SetIdleMotion );

	int						iObjID;
	int						iMotionID;
};
//-----------------------------------------------------------------------------------------
//特殊移動行為列舉
enum	SpecialMoveTypeENUM
{
	EM_SpecialMoveType_Dash			,	//衝擊	
	EM_SpecialMoveType_StrikeBack	,	//震退
	EM_SpecialMoveType_StrikeAway	,	//震飛
	EM_SpecialMoveType_Bomb			,	//炸飛
	EM_SpecialMoveType_Catch		,	//抓取
};
//特殊移動
//Local -> Client
struct PG_Move_LtoC_SpecialMove
{
	GamePGCommandEnum	Command;

	SpecialMoveTypeENUM	MoveType;
	int					ItemGUID;
	int					TargetGUID;		//參考目標
	float				X;
	float				Y;
	float       		Z;
	float				Dir;
	int					MagicCollectID;

	PG_Move_LtoC_SpecialMove()
	{
		Command = EM_PG_Move_LtoC_SpecialMove;
	}
};
//-----------------------------------------------------------------------------------------
//測試連線速度
//Client -> Server
struct PG_Move_CtoL_Ping
{
	GamePGCommandEnum	Command;
	int					Time;

	PG_Move_CtoL_Ping()
	{
		Command = EM_PG_Move_CtoL_Ping;
	}
};
//-----------------------------------------------------------------------------------------
//測試連線速度
//Local -> Client
struct PG_Move_LtoC_Ping
{
	GamePGCommandEnum	Command;
	int					Time;

	PG_Move_LtoC_Ping()
	{
		Command = EM_PG_Move_LtoC_Ping;
	}
};
//-----------------------------------------------------------------------------------------
//測試連線速度Log
//Local -> Client
struct PG_Move_LtoC_PingLog
{
	GamePGCommandEnum	Command;
	int					ID;
	int					Time;

	PG_Move_LtoC_PingLog()
	{
		Command = EM_PG_Move_LtoC_PingLog;
	}
};
//-----------------------------------------------------------------------------------------
//測試連線速度
//Client -> Local
struct PG_Move_CtoL_PingLog
{
	GamePGCommandEnum	Command;
	int					ID;
	int					Time;
	int					ClietNetProcTime;

	PG_Move_CtoL_PingLog()
	{
		Command = EM_PG_Move_CtoL_PingLog;
	}
};
//----------------------------------------------------------------------------------------
// Client Loading 延遲加入分割區要求
// Local -> Client 
struct PG_Move_LtoC_ClientLoading
{
	GamePGCommandEnum	Command;

	float	CameraX;
	float	CameraY;
	float	CameraZ;

	PG_Move_LtoC_ClientLoading()
	{
		Command = EM_PG_Move_LtoC_ClientLoading;
	}
};
//----------------------------------------------------------------------------------------
// 載入完成，加入分割區
// Client -> Local
struct PG_Move_CtoL_ClientLoadingOK
{
	GamePGCommandEnum	Command;

	PG_Move_CtoL_ClientLoadingOK()
	{
		Command = EM_PG_Move_CtoL_ClientLoadingOK;
	}
};

struct PG_Move_LtoC_SetMinimapIcon
{
	GamePGCommandEnum	Command;

	int iObjID;
	int iIconID;

	PG_Move_LtoC_SetMinimapIcon()
	{
		Command = EM_PG_Move_LtoC_SetMinimapIcon;
	}
};
//----------------------------------------------------------------------------------------
//角色特效通知
//Local -> Client
struct PG_Move_LtoC_SpecialEfficacy
{
	GamePGCommandEnum		Command;
	int						ItemGUID;		//物件號碼
	SpecialEfficacyTypeENUM	EffectType;		//特效類型

	PG_Move_LtoC_SpecialEfficacy()
	{
		Command = EM_PG_Move_LtoC_SpecialEfficacy;
	}
};
//-----------------------------------------------------------------------------------------
//設定面向
//Local -> Client
struct PG_Move_LtoC_SetDir
{
	GamePGCommandEnum	Command;
	int					GItemID;
	float				Dir;

	PG_Move_LtoC_SetDir()
	{
		Command = EM_PG_Move_LtoC_SetDir;
	}
};
//-----------------------------------------------------------------------------------------
//重設某角色的公會ID
//Local -> Client
struct PG_Move_LtoC_SetGuildID
{
	GamePGCommandEnum	Command;
	int					GItemID;
	int					GuildID;

	PG_Move_LtoC_SetGuildID()
	{
		Command = EM_PG_Move_LtoC_SetGuildID;
	}
};
//-----------------------------------------------------------------------------------------
//重設某角色的公會ID
//Local -> Client
struct PG_Move_LtoC_SetTitleID
{
	GamePGCommandEnum	Command;
	int					GItemID;
	int					TitleID;
	bool				IsShowTitle;
	StaticString< _MAX_NAMERECORD_SIZE_ >   TitleStr;


	PG_Move_LtoC_SetTitleID()
	{
		Command = EM_PG_Move_LtoC_SetTitleID;
	}
};
//-----------------------------------------------------------------------------------------
//掉落受傷
//Local -> Client
struct PG_Move_LtoC_PlayerDropDownDamage
{
	GamePGCommandEnum	Command;
	int					Damage;		//扣血
	int					Type;		//保留


	PG_Move_LtoC_PlayerDropDownDamage()
	{
		Command = EM_PG_Move_LtoC_PlayerDropDownDamage;
	}
};
//-------------------------------------------------------------------------
//通知某物件為公會建築物
// Local -> Client
struct PG_Move_LtoC_ObjGuildBuildingInfo
{
	GamePGCommandEnum	Command;

	int					GItemID;
	int					BuildingParentDBID;
	int					BuildingDBID;
	int					BuildingID;
	int					Dir;
	char				Point[32];

	PG_Move_LtoC_ObjGuildBuildingInfo()
	{
		Command = EM_PG_Move_LtoC_ObjGuildBuildingInfo;
	}
};
//-------------------------------------------------------------------------
//通知某物件為公會家俱
// Local -> Client
struct PG_Move_LtoC_ObjGuildFurnitureInfo
{
	GamePGCommandEnum	Command;

	int					GItemID;
	int					FurnitureDBID;

	PG_Move_LtoC_ObjGuildFurnitureInfo()
	{
		Command = EM_PG_Move_LtoC_ObjGuildFurnitureInfo;
	}
};
//-----------------------------------------------------------------------------------------
//通知座騎顏色
struct PG_Move_LtoC_HorseColor
{
	GamePGCommandEnum	Command;

	int					GItemID;
	int					Color[4];

	PG_Move_LtoC_HorseColor()
	{
		Command = EM_PG_Move_LtoC_HorseColor;
	}
};
//-----------------------------------------------------------------------------------------
struct PG_Move_LtoC_RoomID
{
	GamePGCommandEnum	Command;

	int					RoomID;		

	PG_Move_LtoC_RoomID()
	{
		Command = EM_PG_Move_LtoC_RoomID;
	}
};
//-----------------------------------------------------------------------------------------
/*
enum DeadZoneTypeENUM
{
	EM_DeadZoneType_Normal		,
	EM_DeadZoneType_DeadZone	,
};*/
//進入死亡區
//Local -> Client
struct PG_Move_LtoC_DeadZone
{
	GamePGCommandEnum	Command;
	ZoneTypeENUM		Type;
	int					DeadCountDown;
	
	PG_Move_LtoC_DeadZone()
	{
		Command = EM_PG_Move_LtoC_DeadZone;
	}
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//通知Client 某物件連結在一起
struct PG_Move_LtoC_AttachObj
{
	GamePGCommandEnum	Command;
	int		GItemID;
	int		AttachItemID;
	int		AttachType;
	StaticString< 32 >   AttachPos;
	StaticString< 32 >   ItemPos;
	int		Pos;

	PG_Move_LtoC_AttachObj()
	{
		Command = EM_PG_Move_LtoC_AttachObj;
	}
};
//------------------------------------------------------------------------------------------------------------
//Local -> Client
//通知Client 某物件解開連結
struct PG_Move_LtoC_DetachObj
{
	GamePGCommandEnum	Command;
	int		GItemID;

	PG_Move_LtoC_DetachObj()
	{
		Command = EM_PG_Move_LtoC_DetachObj;
	}
};
//-----------------------------------------------------------------------------------------
//戰場額外的封包
//-----------------------------------------------------------------------------------------
struct AddPlayerStruct
{
	int			GItemID;
	int			DBID;
	int			WorldID;
	StaticString< _MAX_NAMERECORD_SIZE_ >   RoleName;
	float		X , Y , Z;
	float		HP , MP , SP , SP_Sub;
	int			MaxHP , MaxMP;
	int			Lv;
	int			Lv_Sub;
	Race_ENUM	Race;
	Voc_ENUM	Job;
	Voc_ENUM	Job_Sub;
	LookStruct	Look;

};
//Local -> Client 
//新建立的玩家角色
struct PG_Move_LtoC_BattleGround_AddPlayer
{
	GamePGCommandEnum	Command;

	AddPlayerStruct		Info;

	PG_Move_LtoC_BattleGround_AddPlayer()
	{
		Command = EM_PG_Move_LtoC_BattleGround_AddPlayer;
	}
};

//Local -> Client
//刪除的玩家角色
struct PG_Move_LtoC_BattleGround_DelPlayer
{
	GamePGCommandEnum	Command;

	int					GItemID;

	PG_Move_LtoC_BattleGround_DelPlayer()
	{
		Command = EM_PG_Move_LtoC_BattleGround_DelPlayer;
	}
};

//Local -> Client
//定時 位置的更新
struct PG_Move_LtoC_BattleGround_PlayerPos
{
	GamePGCommandEnum	Command;
	int					GItemID;
	float				X , Y , Z;

	PG_Move_LtoC_BattleGround_PlayerPos()
	{
		Command = EM_PG_Move_LtoC_BattleGround_PlayerPos;
	}
};

//Local -> Client
//定時 血量的更新
struct PG_Move_LtoC_BattleGround_PlayerHPMP
{
	GamePGCommandEnum	Command;
	int					GItemID;
	float				HP , MP , SP , SP_Sub;
	float				MaxHP , MaxMP;

	PG_Move_LtoC_BattleGround_PlayerHPMP()
	{
		Command = EM_PG_Move_LtoC_BattleGround_PlayerHPMP;
	}
};
//-----------------------------------------------------------------------------------------

struct BattleGroundPlayerHPMPBaseStruct
{
	int					GItemID;
	int					X , Y , Z;
	int					HP , MP , SP , SP_Sub;
	int					MaxHP , MaxMP;
};
//Local -> Client
//定時 血量位置的更新
struct PG_Move_LtoC_BattleGround_PlayerInfo_Zip
{
	GamePGCommandEnum	Command;
	int					Count;
	int					Size;
	char				Data[0x10000];

	PG_Move_LtoC_BattleGround_PlayerInfo_Zip()
	{
		Command = EM_PG_Move_LtoC_BattleGround_PlayerInfo_Zip;
	}
	int	PGSize()
	{
		return sizeof(int)*3+ Size;
	}
};
//-----------------------------------------------------------------------------------------
//Client -> Local
//解除卡點
struct PG_Move_CtoL_UnlockRolePosRequest
{
	GamePGCommandEnum	Command;
	float				X,Y,Z;
	bool				Cancel;		//是否取消

	PG_Move_CtoL_UnlockRolePosRequest()
	{
		Command = EM_PG_Move_CtoL_UnlockRolePosRequest;
	}
};
//-----------------------------------------------------------------------------------------
//Local -> Client
//解除卡點時間
struct PG_Move_LtoC_UnlockRolePosTime
{
	GamePGCommandEnum	Command;
	int					Time;	//秒

	PG_Move_LtoC_UnlockRolePosTime()
	{
		Command = EM_PG_Move_LtoC_UnlockRolePosTime;
	}
};
//-----------------------------------------------------------------------------------------
//Local -> Client
//解除卡點結果
struct PG_Move_LtoC_UnlockRolePosOK
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Move_LtoC_UnlockRolePosOK()
	{
		Command = EM_PG_Move_LtoC_UnlockRolePosOK;
	}
};
//------------------------------------------------------------------------------------------------------------
enum AttachObjRequestENUM
{
	EM_AttachObjRequest_Attach	,
	EM_AttachObjRequest_Detach	,
};
//Client -> Local
//通知Client 某物件連結在一起
struct PG_Move_CtoL_AttachObjRequest
{
	GamePGCommandEnum	Command;
	int		GItemID;
	int		WagonItemID;
	AttachObjRequestENUM		Type;		//0 Attach 1 Detach


	StaticString< 32 >   ItemPos;
	StaticString< 32 >   WagonPos;
	int		AttachType;						//Client attach 方式 server不處理

	PG_Move_CtoL_AttachObjRequest()
	{
		Command = EM_PG_Move_CtoL_AttachObjRequest;
	}
};
//-----------------------------------------------------------------------------------------
enum AttachObjResultENUM
{
	EN_AttachObjResult_OK		,
	EN_AttachObjResult_Failed	,
};
//Client -> Local
//通知Client 某物件連結在一起
struct PG_Move_LtoC_AttachObjResult
{
	GamePGCommandEnum	Command;
	AttachObjRequestENUM Type;
	AttachObjResultENUM	Result;
	int					Pos;		//連結位置

	PG_Move_LtoC_AttachObjResult()
	{
		Command = EM_PG_Move_LtoC_AttachObjResult;
	}
};
//-----------------------------------------------------------------------------------------
//通知Client 花盆的資料( 顯示需要ItemFiledStruct )
// Local -> Client
struct PG_Move_LtoC_FlowerPotInfo
{
	GamePGCommandEnum	Command;
	int					GItemID;
	ItemFieldStruct		Item;

	PG_Move_LtoC_FlowerPotInfo()
	{
		Command = EM_PG_Move_LtoC_FlowerPotInfo;
	}
};
//-----------------------------------------------------------------------------------------
//通知Client 某個物件的coldown 資料
// Local -> Client
struct PG_Move_LtoC_ColdownInfo
{
	GamePGCommandEnum	Command;
	int					GItemID;
	MagicColdownStruct	Coldown;

	PG_Move_LtoC_ColdownInfo()
	{
		Command = EM_PG_Move_LtoC_ColdownInfo;
	}
};
//-----------------------------------------------------------------------------------------
struct PlayerBaseInfoStruct
{
	int		DBID;	
	float	X , Y , Z;
	union
	{
		int Mode;
		struct  
		{
			bool IsAttack : 1;
		};
	};
};
//通知所有玩家的座標位置
// Local -> Client
struct PG_Move_LtoC_PlayerPos
{
	GamePGCommandEnum		Command;
	int						Size;
	PlayerBaseInfoStruct	Info[1000];

	PG_Move_LtoC_PlayerPos()
	{
		Command = EM_PG_Move_LtoC_PlayerPos;
	}
	int	PGSize()
	{
		return sizeof(*this) - sizeof( Info ) + Size * sizeof(PlayerBaseInfoStruct);
	}
};

//-----------------------------------------------------------------------------------------
//Client -> Local
//通知Client 某物件連結在一起
struct PG_Move_CtoL_SetPartition
{
	GamePGCommandEnum	Command;
	float X , Y , Z;

	PG_Move_CtoL_SetPartition()
	{
		Command = EM_PG_Move_CtoL_SetPartition;
	}
};

struct PlayerPosSimpleStruct
{
	int GUID;
	int	X , Y , Z;
	int Dir;
};
//Local -> Client 定位所有玩家移動的資訊
struct PG_Move_LtoC_AllPlayerPos
{
	GamePGCommandEnum	Command;
	int		PlayerCount;
	PlayerPosSimpleStruct	List[ 1000 ];

	PG_Move_LtoC_AllPlayerPos()
	{
		Command = EM_PG_Move_LtoC_AllPlayerPos;
	}
	int		PGSize()
	{
		return sizeof(*this) - sizeof(List) + PlayerCount*sizeof(PlayerPosSimpleStruct);		
	}
};

#pragma managed
#endif //__PG_MOVE_H__