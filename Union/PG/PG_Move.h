#ifndef __PG_MOVE_H__
#define __PG_MOVE_H__
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged
//-----------------------------------------------------------------------------------------
// Local -> Client
// �q��Client���󲣥�

#define DF_TIME_PING	10					// ��

struct PG_Move_LtoC_AddToPartition
{ 
	GamePGCommandEnum	Command;
	int					GItemID;			//���󸹽X (�n�D���ʪ�����)
	int					WorldGUID;
	//    myUInt32			LGItemID;           //�W�Ӱϰ쪺ID

	PlayerGraphStruct	GraphObj;			//�㹳���
	RolePosStruct       Pos;
	int					DBID;
	int					MasterGUID;			//�D�H�����󸹽X
	CampID_ENUM			CampID;
	int					iWorldID;			// ������ݪ��@�ɥN�X


	PG_Move_LtoC_AddToPartition()
	{ Command = EM_PG_Move_LtoC_AddToPartition; }
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// �q��Client�������
struct PG_Move_LtoC_DelFromPartition
{
	GamePGCommandEnum	Command;
	int					GItemID;				//���󸹽X (�n�D���ʪ�����)
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
// �q��Client���󭫷s�w��
struct PG_Move_LtoC_SetPostion
{
	GamePGCommandEnum	Command;
	int					GItemID;				//���󸹽X (�n�D���ʪ�����)
	//    myUInt32				WorldGUID;
	RolePosStruct       Pos;
	//    float				X , Y , Z , Dir;

	PG_Move_LtoC_SetPostion()
	{ Command = EM_PG_Move_LtoC_SetPostion; }
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// �q��Client���󲾰�
struct PG_Move_LtoC_ObjectMove
{
	GamePGCommandEnum	Command;
	int					GItemID;				//���󸹽X (�n�D���ʪ�����)
	//    myUInt32				WorldGUID;
	RolePosStruct       Pos;
	float               vX;	//���a�����ʦV�q�Anpc �ثe��m
	float				vY;
	float               vZ;

	ClientMoveTypeENUM  Type;					//

	PG_Move_LtoC_ObjectMove()
	{ Command = EM_PG_Move_LtoC_ObjectMove; }
};

//Local -> Client
//���a���ʫʥ](�Y��O)
struct PG_Move_LtoC_PlayerMove
{
	GamePGCommandEnum	Command;
	int					GItemID;				//���󸹽X (�n�D���ʪ�����)

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
// ���a����󲾰ʪ���T
struct PG_Move_CtoL_PlayerMoveObject
{
	GamePGCommandEnum	Command;
	int					GItemID;				//���󸹽X (�n�D���ʪ�����)    
	RolePosStruct       Pos;
	ClientMoveTypeENUM  Type;					//
	float               vX;
	float               vY;
	float               vZ;			
	
//	float				GroundHeight;
	int					AttachObjID;			//�ү�������


	PG_Move_CtoL_PlayerMoveObject()
	{ Command = EM_PG_Move_CtoL_PlayerMoveObject; }
};

//-----------------------------------------------------------------------------------------
// Local -> Client
//�q�� Client ���ʧ@
struct PG_Move_LtoC_DoAction
{
	GamePGCommandEnum	Command;
	int					GItemID;				//���󸹽X (�n�D���ʧ@������)
	int					ActionID;
	int		            TempActionID;

	PG_Move_LtoC_DoAction()
	{ Command = EM_PG_Move_LtoC_DoAction; }
};
//-----------------------------------------------------------------------------------------
// Client -> Server
//�q�� Server ��o���ʧ@
struct PG_Move_CtoL_DoAction
{
	GamePGCommandEnum	Command;
	int					GItemID;				//���󸹽X (�n�D���ʧ@������)
	int					ActionID;
	int	                TempActionID;              //�O�_�Ȯɰʰ�

	PG_Move_CtoL_DoAction()
	{ Command = EM_PG_Move_CtoL_DoAction; }
};
//-----------------------------------------------------------------------------------------
// Local -> Client
//�]�w���ʳt��
struct PG_Move_LtoC_MoveSpeed
{
	GamePGCommandEnum	Command;
	int					GItemID;				//���󸹽X (�n�D���ʧ@������)
	//    myUInt32				WorldGUID;
	float				Speed;

	PG_Move_LtoC_MoveSpeed()
	{ Command = EM_PG_Move_LtoC_MoveSpeed; }
};
//-----------------------------------------------------------------------------------------
// Local -> Client
//�]�w ���ʸ�� ���O�P���D�O..
struct PG_Move_LtoC_MoveInfo
{
	GamePGCommandEnum	Command;
	int					GItemID;				//���󸹽X (�n�D���ʧ@������)
	
	float				Gravity;
	float				JumpRate;

	PG_Move_LtoC_MoveInfo()
	{ Command = EM_PG_Move_LtoC_MoveInfo; }
};

//-----------------------------------------------------------------------------------------
// Local -> Client 
// ��j���v
struct PG_Move_LtoC_ModelRate
{
	GamePGCommandEnum	Command;
	int					GItemID;				//���󸹽X (�n�D���ʧ@������)
	float				ModelRate;

	PG_Move_LtoC_ModelRate()
	{ Command = EM_PG_Move_LtoC_ModelRate; }
};
//-----------------------------------------------------------------------------------------
// Local -> Client
//�q����̤��۪����Y
struct PG_Move_LtoC_Relation
{
	GamePGCommandEnum	Command;
	int					GItemID;				//���󸹽X 
	RoleRelationStruct  Relation;

	PG_Move_LtoC_Relation()
	{ Command = EM_PG_Move_LtoC_Relation; }
};
//-----------------------------------------------------------------------------------------
//����e���L��
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

	int		                                    GItemID;				//���󪺰ϰ�
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
	RoleActionStruct	Action;			                //�ثe����ʪ��A

	PG_Move_LtoC_ActionType()
	{
		Command = EM_PG_Move_LtoC_ActionType;
	}
};
//-----------------------------------------------------------------------------------------
//Client �q�� Server�n�����V
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
//Server �q�� �P�򪱮a�Y����n�����V
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
//Client �q�� Server���V����
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
//Server �q�� Client���V����
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
//�S����
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
//�S���ʦ欰�C�|
enum	SpecialMoveTypeENUM
{
	EM_SpecialMoveType_Dash			,	//����	
	EM_SpecialMoveType_StrikeBack	,	//�_�h
	EM_SpecialMoveType_StrikeAway	,	//�_��
	EM_SpecialMoveType_Bomb			,	//����
	EM_SpecialMoveType_Catch		,	//���
};
//�S����
//Local -> Client
struct PG_Move_LtoC_SpecialMove
{
	GamePGCommandEnum	Command;

	SpecialMoveTypeENUM	MoveType;
	int					ItemGUID;
	int					TargetGUID;		//�Ѧҥؼ�
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
//���ճs�u�t��
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
//���ճs�u�t��
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
//���ճs�u�t��Log
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
//���ճs�u�t��
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
// Client Loading ����[�J���ΰϭn�D
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
// ���J�����A�[�J���ΰ�
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
//����S�ĳq��
//Local -> Client
struct PG_Move_LtoC_SpecialEfficacy
{
	GamePGCommandEnum		Command;
	int						ItemGUID;		//���󸹽X
	SpecialEfficacyTypeENUM	EffectType;		//�S������

	PG_Move_LtoC_SpecialEfficacy()
	{
		Command = EM_PG_Move_LtoC_SpecialEfficacy;
	}
};
//-----------------------------------------------------------------------------------------
//�]�w���V
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
//���]�Y���⪺���|ID
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
//���]�Y���⪺���|ID
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
//��������
//Local -> Client
struct PG_Move_LtoC_PlayerDropDownDamage
{
	GamePGCommandEnum	Command;
	int					Damage;		//����
	int					Type;		//�O�d


	PG_Move_LtoC_PlayerDropDownDamage()
	{
		Command = EM_PG_Move_LtoC_PlayerDropDownDamage;
	}
};
//-------------------------------------------------------------------------
//�q���Y���󬰤��|�ؿv��
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
//�q���Y���󬰤��|�a��
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
//�q���y�M�C��
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
//�i�J���`��
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
//�q��Client �Y����s���b�@�_
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
//�q��Client �Y����Ѷ}�s��
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
//�Գ��B�~���ʥ]
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
//�s�إߪ����a����
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
//�R�������a����
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
//�w�� ��m����s
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
//�w�� ��q����s
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
//�w�� ��q��m����s
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
//�Ѱ��d�I
struct PG_Move_CtoL_UnlockRolePosRequest
{
	GamePGCommandEnum	Command;
	float				X,Y,Z;
	bool				Cancel;		//�O�_����

	PG_Move_CtoL_UnlockRolePosRequest()
	{
		Command = EM_PG_Move_CtoL_UnlockRolePosRequest;
	}
};
//-----------------------------------------------------------------------------------------
//Local -> Client
//�Ѱ��d�I�ɶ�
struct PG_Move_LtoC_UnlockRolePosTime
{
	GamePGCommandEnum	Command;
	int					Time;	//��

	PG_Move_LtoC_UnlockRolePosTime()
	{
		Command = EM_PG_Move_LtoC_UnlockRolePosTime;
	}
};
//-----------------------------------------------------------------------------------------
//Local -> Client
//�Ѱ��d�I���G
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
//�q��Client �Y����s���b�@�_
struct PG_Move_CtoL_AttachObjRequest
{
	GamePGCommandEnum	Command;
	int		GItemID;
	int		WagonItemID;
	AttachObjRequestENUM		Type;		//0 Attach 1 Detach


	StaticString< 32 >   ItemPos;
	StaticString< 32 >   WagonPos;
	int		AttachType;						//Client attach �覡 server���B�z

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
//�q��Client �Y����s���b�@�_
struct PG_Move_LtoC_AttachObjResult
{
	GamePGCommandEnum	Command;
	AttachObjRequestENUM Type;
	AttachObjResultENUM	Result;
	int					Pos;		//�s����m

	PG_Move_LtoC_AttachObjResult()
	{
		Command = EM_PG_Move_LtoC_AttachObjResult;
	}
};
//-----------------------------------------------------------------------------------------
//�q��Client ��֪����( ��ܻݭnItemFiledStruct )
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
//�q��Client �Y�Ӫ���coldown ���
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
//�q���Ҧ����a���y�Ц�m
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
//�q��Client �Y����s���b�@�_
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
//Local -> Client �w��Ҧ����a���ʪ���T
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