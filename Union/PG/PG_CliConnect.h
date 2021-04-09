#ifndef __PG_CLICONNECT_H__
#define __PG_CLICONNECT_H__
#pragma warning (disable:4949)
#include "NetWakerPGEnum.h"
#include "roledata/RoleDataEx.h"
#pragma unmanaged
//----------------------------------------------------------------------------------------

//Local Server -> Client
//���a����ϰ�ID
struct PG_CliConnect_LtoC_PlayerItemID
{
    GamePGCommandEnum	Command;
    myInt32	GItemID;
    myInt32 WorldGUID;
    myInt32 DBID;

    PG_CliConnect_LtoC_PlayerItemID()
    {
        Command = EM_PG_CliConnect_LtoC_PlayerItemID;
    }
};
//----------------------------------------------------------------------------------------
struct ZoneInfoModeStruct
{
	union
	{
		int Mode;
		struct  
		{
			bool				IsPvE							: 1;	//pk�Ҧ�
			bool				IsBattleGround					: 1;	//�j��I�t�X�l���ϧΥN�X, �Y�� 0 �h�����
			bool				IsDisablePVPRule				: 1;	//���H�W�椣�O��
			bool				IsDisableTrade					: 1;	//���i���
			bool				IsDisableDual					: 1;	//���i���
			bool				IsDisableSendGift				: 1;	//���i�e§			
			bool				AcAccountMoneyTrade				: 1;	//AC ���i����b����
			bool				AcItemAccountMoneyTrade			: 1;	//AC���~ ���i����b����
			bool				IsZonePartyEnabled				: 1;	//�ηs��Party�Ҧ�
			bool				IsDisableParty					: 1;	//�����ܽвն�
			bool				IsNoSwimming					: 1;	//���ϰ줣�i��a					
			bool				IsCheckHeight					: 1;	//���ϰ�n�����ˬd
		};
	};
};
//Local Server -> Client
 
struct PG_CliConnect_LtoC_ZoneInfo
{
    GamePGCommandEnum	Command;
    myInt16				ZoneID;			//�ϰ�id
	myInt16				MapID;			//�ϰ�id
	int					CampObjID;		//�}���
	LanguageTypeENUM	LanguageType;

	ZoneInfoModeStruct  ZoneMode;
	/*
	bool				IsPvE;			//pk�Ҧ�
	bool				IsBattleGround;	// �j��I�t�X�l���ϧΥN�X, �Y�� 0 �h�����
*/
	int					BG_iNumTeam;
	int					BG_iNumTeamPeople;

    PG_CliConnect_LtoC_ZoneInfo()
    {
        Command = EM_PG_CliConnect_LtoC_ZoneInfo;
    }
};
//----------------------------------------------------------------------------------------
//Local Server -> Client
//�ǰe����򥻸��
struct PG_CliConnect_LtoC_PlayerData
{
    GamePGCommandEnum	Command;

    //����򥻸��(Client �ݻݭn�Ψ쪺����)
    BaseRoleData		BaseData;
	PlayerBaseRoleData	PlayerBaseData;
	SysBaseRoleData		Base;

//	int		LearnMagicID;
//	SystemVersionENUM	Version;

    PG_CliConnect_LtoC_PlayerData()
    {
        Command = EM_PG_CliConnect_LtoC_PlayerData;
    }
};
//----------------------------------------------------------------------------------------
//Local Server -> Client
struct PG_CliConnect_LtoC_SysVersionInfo
{
	GamePGCommandEnum	Command;

	int					LearnMagicID;
	SystemVersionENUM	Version;
	float				PK_DotRate;		//PK dot ���ˮ`�ץ��ʤ���

	PG_CliConnect_LtoC_SysVersionInfo()
	{
		Command = EM_PG_CliConnect_LtoC_SysVersionInfo;
	}
};
//----------------------------------------------------------------------------------------
//Local Server -> Client 
//�q�����⴫��(��Ҧ������⤽��)
struct PG_CliConnect_LtoC_PrepareChangeZone
{
    GamePGCommandEnum	Command;

    PG_CliConnect_LtoC_PrepareChangeZone()
    {
        Command = EM_PG_CliConnect_LtoC_PrepareChangeZone;
    }
};
//----------------------------------------------------------------------------------------
//Local Server -> Client 
//�q�����⴫�ϧ���
struct PG_CliConnect_LtoC_ChangeZoneOK
{
    GamePGCommandEnum	Command;

    PG_CliConnect_LtoC_ChangeZoneOK()
    {
        Command = EM_PG_CliConnect_LtoC_ChangeZoneOK;
    }
};
//----------------------------------------------------------------------------------------
//Client -> Server
//�n�D�n�X
struct PG_CliConnect_CtoL_RequestLogout
{
    GamePGCommandEnum	Command;

    PG_CliConnect_CtoL_RequestLogout()
    {
        Command = EM_PG_CliConnect_CtoL_RequestLogout;
    }
};
//----------------------------------------------------------------------------------------
//Server -> Client
//�n�D�n�X
struct PG_CliConnect_LtoC_LogoutResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_CliConnect_LtoC_LogoutResult()
	{
		Command = EM_PG_CliConnect_LtoC_LogoutResult;
	}
};
//----------------------------------------------------------------------------------------
//���ϳq��
// Local -> Local
struct PG_CliConnect_LtoL_ChangeZoneNodify
{
    GamePGCommandEnum	Command;
	int					SrcZoneID;

    //RoleDataEx          Role;
	PlayerRoleData		Role;

    PG_CliConnect_LtoL_ChangeZoneNodify()
    {
        Command = EM_PG_CliConnect_LtoL_ChangeZoneNodify;
    }
};
//----------------------------------------------------------------------------------------
enum	ChangeZoneNodifyResult_ENUM
{
	EM_ChangeZoneNodifyResult_OK						,
	EM_ChangeZoneNodifyResult_PraviteZone_NoRoom		,
	EM_ChangeZoneNodifyResult_Failed					,
	EM_ChangeZoneNodifyResult_NoBattleGroundData		,
	EM_ChangeZoneNodifyResult_InstancePartyKeyErr		,
	EM_ChangeZoneNodifyResult_InstancePartyKey_Duplicate,
};

//���ϳq��
// Local -> Local
struct PG_CliConnect_LtoL_ChangeZoneNodifyResult
{
	GamePGCommandEnum	Command;

	int		GItemID;
	int		ZoneID;
	float	X , Y , Z;
	ChangeZoneNodifyResult_ENUM Result;

	PG_CliConnect_LtoL_ChangeZoneNodifyResult()
	{
		Command = EM_PG_CliConnect_LtoL_ChangeZoneNodifyResult;
	}
};
//----------------------------------------------------------------------------------------
// Server -> Client
//�ק磌���T

struct PG_CliConnect_LtoC_ModifyNPCInfo
{
    GamePGCommandEnum	Command;
    ModifyNPCInfo       Info;

    PG_CliConnect_LtoC_ModifyNPCInfo()
    {
        Command = EM_PG_CliConnect_LtoC_ModifyNPCInfo;
    }
};
//----------------------------------------------------------------------------------------
// Client -> Server
// �n�D�ק�
struct PG_CliConnect_CtoL_ModifyNPCInfo
{
    GamePGCommandEnum	Command;
    ModifyNPCInfo       Info;

    PG_CliConnect_CtoL_ModifyNPCInfo()
    {
        Command = EM_PG_CliConnect_CtoL_ModifyNPCInfo;
    }
};
//----------------------------------------------------------------------------------------
// Client -> Local 
//�n�D���o�YNPC�����ʸ�T
struct PG_CliConnect_CtoL_GetNPCMoveInfoRequest
{
	GamePGCommandEnum	Command;
	int					GUID;

	PG_CliConnect_CtoL_GetNPCMoveInfoRequest()
	{
		Command = EM_PG_CliConnect_CtoL_GetNPCMoveInfoRequest;
	}
};
//----------------------------------------------------------------------------------------
// Local -> Client 
// NPC�����ʸ�T
struct PG_CliConnect_LtoC_GetNPCMoveInfoResult
{
	GamePGCommandEnum	Command;
	int		TotalCount;
	int		GUID;			//NPCDBID
	int		IndexID;		//��m
	NPC_MoveBaseStruct Base;

	PG_CliConnect_LtoC_GetNPCMoveInfoResult()
	{
		Command = EM_PG_CliConnect_LtoC_GetNPCMoveInfoResult;
	}
};
//----------------------------------------------------------------------------------------
// Client -> Local
//----------------------------------------------------------------------------------------
struct PG_CliConnect_CtoL_SaveNPCMoveInfo
{
	GamePGCommandEnum	Command;
	int		TotalCount;
	int		GUID;			//NPCDBID
	int		IndexID;		//��m
	NPC_MoveBaseStruct Base;

	PG_CliConnect_CtoL_SaveNPCMoveInfo()
	{
		Command = EM_PG_CliConnect_CtoL_SaveNPCMoveInfo;
	}
};

//----------------------------------------------------------------------------------------
// Client -> Local 
//�n�D���o Server Flag �M��
struct PG_CliConnect_CtoL_GetFlagList
{
	GamePGCommandEnum	Command;

	PG_CliConnect_CtoL_GetFlagList()
	{
		Command = EM_PG_CliConnect_CtoL_GetFlagList;
	}
};

//----------------------------------------------------------------------------------------
struct PG_CliConnect_LtoC_GetFlagListResult
{
	GamePGCommandEnum	Command;
	
	int					iTotalGroup;
	
	// �H�U��ƫ�����, �åH 0 ���@�q��ƭ��Ъ� END

	int					iCount;

	int					ObjID;      //���O
	int					ID;         //�s��
	float				X;
	float				Y;
	float				Z;    
	float				Dir;
	int					FlagDBID;	//�X�l��ƮwID

	PG_CliConnect_LtoC_GetFlagListResult()
	{
		Command = EM_PG_CliConnect_LtoC_GetFlagListResult;
	}
};
//----------------------------------------------------------------------------------------
struct PG_CliConnect_CtoL_EditFlag
{
	GamePGCommandEnum	Command;

	int					ObjID;      //���O
	int					ID;         //�s��
	float				X;
	float				Y;
	float				Z;    
	float				Dir;
	int					FlagDBID;	//�X�l��ƮwID

	PG_CliConnect_CtoL_EditFlag()
	{
		Command = EM_PG_CliConnect_CtoL_EditFlag;
	}
};
//----------------------------------------------------------------------------------------
struct PG_CliConnect_LtoC_EditFlagResult
{
	GamePGCommandEnum	Command;
	int		iResult;

	PG_CliConnect_LtoC_EditFlagResult()
	{
		Command = EM_PG_CliConnect_LtoC_EditFlagResult;
	}
};
//----------------------------------------------------------------------------------------
struct PG_CliConnect_CtoL_DelFlag
{
	GamePGCommandEnum	Command;

	int					GroupID;      //���O
	int					FlagID;         //�s��

	PG_CliConnect_CtoL_DelFlag()
	{
		Command = EM_PG_CliConnect_CtoL_DelFlag;
	}
};
//----------------------------------------------------------------------------------------
struct PG_CliConnect_LtoC_DelFlagResult
{
	GamePGCommandEnum	Command;
	int		iResult;

	PG_CliConnect_LtoC_DelFlagResult()
	{
		Command = EM_PG_CliConnect_LtoC_DelFlagResult;
	}
};
//----------------------------------------------------------------------------------------
struct PG_CliConnect_CtoL_DelFlagGroup
{
	GamePGCommandEnum	Command;

	int					iFlagGroupID;      //���O

	PG_CliConnect_CtoL_DelFlagGroup()
	{
		Command = EM_PG_CliConnect_CtoL_DelFlagGroup;
	}
};
//----------------------------------------------------------------------------------------
struct PG_CliConnect_LtoC_DelFlagGroupResult
{
	GamePGCommandEnum	Command;
	int		iResult;

	PG_CliConnect_LtoC_DelFlagGroupResult()
	{
		Command = EM_PG_CliConnect_LtoC_DelFlagGroupResult;
	}
};
//----------------------------------------------------------------------------------------
struct PG_CliConnect_LtoC_ParalledID
{
	GamePGCommandEnum	Command;
	int					ParalledID;

	PG_CliConnect_LtoC_ParalledID()
	{
		Command = EM_PG_CliConnect_LtoC_ParalledID;
	}
};
//----------------------------------------------------------------------------------------
struct PG_CliConnect_LtoC_PlayerData_Zip
{
	GamePGCommandEnum	Command;
	int					Size;
	char				Data[0x20000];

	int	PGSize()
	{
		return sizeof(int)*2+ Size;
	}
	PG_CliConnect_LtoC_PlayerData_Zip()
	{
		Command = EM_PG_CliConnect_LtoC_PlayerData_Zip;
	}
};

//----------------------------------------------------------------------------------------
//�R���ӸO
//Local -> Local
struct PG_CliConnect_LtoL_DelTomb
{
	GamePGCommandEnum	Command;
	int					TombGUID;
	int					OwnerDBID;

	PG_CliConnect_LtoL_DelTomb()
	{
		Command = EM_PG_CliConnect_LtoL_DelTomb;
	}
};

//////////////////////////////////////////////////////////////////////////
// �S���ʥ]
//////////////////////////////////////////////////////////////////////////
//���� �t�~�@�� World ���Գ�Server
//Local -> Battle Server
struct PG_CliConnect_LtoBL_ChangeZoneToOtherWorld
{
	GamePGCommandEnum	Command;
//	int					GSrvID;
	int					RoleSize;
	PlayerRoleData		Role;


	PG_CliConnect_LtoBL_ChangeZoneToOtherWorld()
	{
		Command = EM_PG_CliConnect_LtoBL_ChangeZoneToOtherWorld;
	}
};

//���� �t�~�@�� World ���Գ�Server
//Local -> Battle Server
struct PG_CliConnect_LtoBL_ChangeZoneToOtherWorld_Zip
{
	GamePGCommandEnum	Command;
	int					RoleSize;
	int					Size;
	char				Data[0x20000];

	int	PGSize()
	{
		return sizeof(int)*3+ Size;
	}


	PG_CliConnect_LtoBL_ChangeZoneToOtherWorld_Zip()
	{
		Command = EM_PG_CliConnect_LtoBL_ChangeZoneToOtherWorld_Zip;
	}
};

//----------------------------------------------------------------------------------------
enum ChangeZoneToOtherWorldResultENUM
{
	EM_ChangeZoneToOtherWorldResult_OK		,
	EM_ChangeZoneToOtherWorldResult_Failed	,
	EM_ChangeZoneToOtherWorldResult_Full	,	//�H�Ƥw��
	
};
//�n�D����OK 
//Battle Server -> Local
struct PG_CliConnect_BLtoL_ChangeZoneToOtherWorldResult
{
	GamePGCommandEnum	Command;
	int					RoleDBID;
	char				IPStr[64];
	int					Port;
	ChangeZoneToOtherWorldResultENUM				Result;

	PG_CliConnect_BLtoL_ChangeZoneToOtherWorldResult()
	{
		Command = EM_PG_CliConnect_BLtoL_ChangeZoneToOtherWorldResult;
	}
};
//----------------------------------------------------------------------------------------
//Local -> Client �s�� Battle World
struct PG_CliConnect_LtoC_ConnectToBattleWorld
{
	GamePGCommandEnum	Command;
	char				IPStr[64];
	int					Port;

	PG_CliConnect_LtoC_ConnectToBattleWorld()
	{
		Command = EM_PG_CliConnect_LtoC_ConnectToBattleWorld;
	}
};
//----------------------------------------------------------------------------------------
 
struct PG_CliConnect_CtoL_ConnectToBattleWorldOK
{
	GamePGCommandEnum	Command;
	int					CliNetID;

	PG_CliConnect_CtoL_ConnectToBattleWorldOK()
	{
		Command = EM_PG_CliConnect_CtoL_ConnectToBattleWorldOK;
	}
};
//----------------------------------------------------------------------------------------
//Local -> Battle Server �n�D�PClient ���߳s�u
struct PG_CliConnect_LtoBL_ConnectCliAndGSrv
{
	GamePGCommandEnum	Command;
//	int					GSrvID;
	int					CliNetID;

	PG_CliConnect_LtoBL_ConnectCliAndGSrv()
	{
		Command = EM_PG_CliConnect_LtoBL_ConnectCliAndGSrv;
	}
};
//----------------------------------------------------------------------------------------
//Battle Server -> Local �����Ƶn�X�^��Server
struct PG_CliConnect_BLtoL_RetNomorlWorld
{
	GamePGCommandEnum	Command;
	int		PlayerDBID;
	PlayerRoleData	Role;

	PG_CliConnect_BLtoL_RetNomorlWorld()
	{
		Command = EM_PG_CliConnect_BLtoL_RetNomorlWorld;
	}
};

struct PG_CliConnect_BLtoL_RetNomorlWorld_Zip
{
	GamePGCommandEnum	Command;
	int			PlayerDBID;
	int			Size;
	char		Data[0x20000];

	int	PGSize()
	{
		return sizeof(int)*3+ Size;
	}

	PG_CliConnect_BLtoL_RetNomorlWorld_Zip()
	{
		Command = EM_PG_CliConnect_BLtoL_RetNomorlWorld_Zip;
	}
};
//----------------------------------------------------------------------------------------
//Battle Server -> Local �����Ƶn�X�^��Server
struct PG_CliConnect_BLtoL_BattleSavePlayer
{
	GamePGCommandEnum	Command;
	int		PlayerDBID;
	PlayerRoleData	Role;

	PG_CliConnect_BLtoL_BattleSavePlayer()
	{
		Command = EM_PG_CliConnect_BLtoL_BattleSavePlayer;
	}
};

struct PG_CliConnect_BLtoL_BattleSavePlayer_Zip
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					Size;
	char				Data[0x20000];

	int	PGSize()
	{
		return sizeof(int)*3+ Size;
	}

	PG_CliConnect_BLtoL_BattleSavePlayer_Zip()
	{
		Command = EM_PG_CliConnect_BLtoL_BattleSavePlayer_Zip;
	}
};
//----------------------------------------------------------------------------------------
enum	ClientSkyProcTypeENUM
{
	EM_ClientSkyProcType_Normal		,	//�@��
	EM_ClientSkyProcType_Day		,	//�դ�
	EM_ClientSkyProcType_NightFall	,	//����
	EM_ClientSkyProcType_Night		,	//�ߤW
	EM_ClientSkyProcType_Dawn		,	//����
};

//Local -> Client �����Ƶn�X�^��Server
struct PG_CliConnect_LtoC_SkyProcType
{
	GamePGCommandEnum	Command;
	ClientSkyProcTypeENUM Type;

	PG_CliConnect_LtoC_SkyProcType()
	{
		Command = EM_PG_CliConnect_LtoC_SkyProcType;
	}
};
//----------------------------------------------------------------------------------------
//Server -> Local Ĳ�N�ӸO
struct PG_CliConnect_LtoC_TouchTomb
{
	GamePGCommandEnum	Command;

	PG_CliConnect_LtoC_TouchTomb()
	{
		Command = EM_PG_CliConnect_LtoC_TouchTomb;
	}
};
//----------------------------------------------------------------------------------------
//Server -> Local �ӸO���
struct PG_CliConnect_LtoC_TombInfo
{
	GamePGCommandEnum	Command;
	int					CreateTime;
	int					ZoneID;
	float				X , Y , Z;
	int					DebtExp;
	int					DebtTp;
	int					Exp;


	PG_CliConnect_LtoC_TombInfo()
	{
		Command = EM_PG_CliConnect_LtoC_TombInfo;
	}
};
//----------------------------------------------------------------------------------------
//Server -> Client �PBattle Ground Server �פ�ʥ] �ǳƴ��^��Server
struct PG_CliConnect_LtoC_WorldReturnNotify
{
	GamePGCommandEnum	Command;
	int			ZoneID;				//�ˬd�O�_�b��ZoneID , �_�h���B�z

	PG_CliConnect_LtoC_WorldReturnNotify()
	{
		Command = EM_PG_CliConnect_LtoC_WorldReturnNotify;
	}
};
//----------------------------------------------------------------------------------------
//Client -> Server �PBattle Ground Server �פ�ʥ] OK
struct PG_CliConnect_CtoL_WorldReturnOK
{
	GamePGCommandEnum	Command;

	PG_CliConnect_CtoL_WorldReturnOK()
	{
		Command = EM_PG_CliConnect_CtoL_WorldReturnOK;
	}
};
//----------------------------------------------------------------------------------------
//��Server��e�ʥ]���Y�Ӫ��a
//Local <-> Battle Server
struct PG_CliConnect_BLtoBL_SendToPlayerByDBID
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					DataSize;
	char				Data[0x10000];

	PG_CliConnect_BLtoBL_SendToPlayerByDBID()
	{
		Command = EM_PG_CliConnect_BLtoBL_SendToPlayerByDBID;
	}
	int PGSize()
	{
		return sizeof(*this)- sizeof(Data) + DataSize;
	}
};
//----------------------------------------------------------------------------------------
//Local -> Client �����Ƶn�X�^��Server
struct PG_CliConnect_LtoC_ZoneOpenState
{
	GamePGCommandEnum	Command;
	int					ZoneID;
	bool				IsOpen;

	PG_CliConnect_LtoC_ZoneOpenState()
	{
		Command = EM_PG_CliConnect_LtoC_ZoneOpenState;
	}
};
//----------------------------------------------------------------------------------------/
//�q���Ҧb�Գ�
//Battle Server -> Local
struct PG_CliConnect_BLtoL_BattleZone
{
	GamePGCommandEnum	Command;
	int					ZoneID;
	int					PlayerDBID;
	PG_CliConnect_BLtoL_BattleZone()
	{
		Command = EM_PG_CliConnect_BLtoL_BattleZone;
	}
};

//�n�D��Գ�����R��
//Local -> Battle Server
struct PG_CliConnect_LtoBL_ReturnWorldRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					TestCount;

	PG_CliConnect_LtoBL_ReturnWorldRequest()
	{
		Command = EM_PG_CliConnect_LtoBL_ReturnWorldRequest;
	}
};

//�^���R�����G
//Battle Server -> Local
struct PG_CliConnect_BLtoL_ReturnWorldResult
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					TestCount;
	bool				Result;

	PG_CliConnect_BLtoL_ReturnWorldResult()
	{
		Command = EM_PG_CliConnect_BLtoL_ReturnWorldResult;
	}
};


#pragma managed
#endif //__PG_CLICONNECT_H__