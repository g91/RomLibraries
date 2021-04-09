#pragma once
#pragma warning (disable:4949)
#include "NetWakerPGEnum.h"
#include "roledata/RoleDataEx.h"
#pragma unmanaged

//-----------------------------------------------------------------------------------------------------
//�����e ��Y�@�s��Srv 
// �Ҧ� Srv -> Chat
struct PG_ChannelBase_toChat_DataTransmitToGSrv_Group
{
    GamePGCommandEnum	Command;
    int                 SenderDBID;
    int                 DataSize;
    int                 ChannelID;
    GroupObjectTypeEnum Type;
    PacketBaseMaxStruct Data;
    PG_ChannelBase_toChat_DataTransmitToGSrv_Group()
    {
        Command = EM_PG_ChannelBase_toChat_DataTransmitToGSrv_Group;
    }
    int     Size( )
    {
        return (sizeof(*this) + DataSize - sizeof(Data));
    }
};
//-----------------------------------------------------------------------------------------------------
//�����e ��Y�@�s��Cli
// �Ҧ� Srv -> Chat
struct PG_ChannelBase_toChat_DataTransmitToCli_Group
{
    GamePGCommandEnum	Command;
    int                 SenderDBID;
    int                 DataSize;
    int                 ChannelID;
    GroupObjectTypeEnum Type;
    PacketBaseMaxStruct Data;
    PG_ChannelBase_toChat_DataTransmitToCli_Group()
    {
        Command = EM_PG_ChannelBase_toChat_DataTransmitToCli_Group;
    }
    int     Size( )
    {
        return (sizeof(*this) + DataSize - sizeof(Data));
    }
};
//-----------------------------------------------------------------------------------------------------
//�����e ��Y�H��Srv
// �Ҧ� Srv -> Chat
struct PG_ChannelBase_toChat_DataTransmitToSrv
{
    GamePGCommandEnum	Command;
    int                 SenderDBID;
    int                 DataSize;
    int                 DBID;
	StaticString <_MAX_NAMESTRING_LEN_> Name;
    PacketBaseMaxStruct Data;
    PG_ChannelBase_toChat_DataTransmitToSrv()
    {
        Command = EM_PG_ChannelBase_toChat_DataTransmitToSrv;
    }
    int     Size( )
    {
        return (sizeof(*this) + DataSize - sizeof(Data));
    }
};
//-----------------------------------------------------------------------------------------------------
//�����e ��Y�H��Cli
// �Ҧ� Srv -> Chat
struct PG_ChannelBase_toChat_DataTransmitToCli
{
    GamePGCommandEnum	Command;
    int                 SenderDBID;
    int                 DataSize;
    int                 DBID;
    StaticString <_MAX_NAMESTRING_LEN_> Name;    
    PacketBaseMaxStruct Data;
    PG_ChannelBase_toChat_DataTransmitToCli()
    {
        Command = EM_PG_ChannelBase_toChat_DataTransmitToCli;
    }
    int     Size( )
    {
        return (sizeof(*this) + DataSize - sizeof(Data));
    }
};
//-----------------------------------------------------------------------------------------------------
//��Ƶ��U �� ����
//-----------------------------------------------------------------------------------------------------
//�n�D���ͤ@�� Channel
struct PG_ChannelBase_LtoChat_CreateChannel
{
    GamePGCommandEnum	Command;
    GroupObjectTypeEnum Type;
    int                 UserDBID;       //�R�O�n�D��
	StaticString <_MAX_NAMESTRING_LEN_> Name;    
	StaticString <_MAX_NAMESTRING_LEN_> Password;
    PG_ChannelBase_LtoChat_CreateChannel()
    {
        Command = EM_PG_ChannelBase_LtoChat_CreateChannel;
    }
};
//-----------------------------------------------------------------------------------------------------
//���� Channel ���G
struct PG_ChannelBase_ChattoL_CreateChannelResult
{
    GamePGCommandEnum	Command;
    bool                Result;          
    GroupObjectTypeEnum Type;
    int                 ChannelID;
	StaticString<_MAX_NAMESTRING_LEN_> Name;    
    int                 UserDBID;       //�R�O�n�D��
    PG_ChannelBase_ChattoL_CreateChannelResult()
    {
        Command = EM_PG_ChannelBase_ChattoL_CreateChannelResult;
    }
};
//-----------------------------------------------------------------------------------------------------
//�n�D�[�J�Y�����Y�@�W�D
struct PG_ChannelBase_LtoChat_AddUser
{
    GamePGCommandEnum	Command;
    GroupObjectTypeEnum Type;
    int                 ChannelID;
    int                 UserDBID;
    int                 SenderDBID;
	StaticString <_MAX_NAMESTRING_LEN_> Password;    
	StaticString <_MAX_NAMESTRING_LEN_> ChannelName;    
	bool				IsForceJoin;
    PG_ChannelBase_LtoChat_AddUser()
    {
        Command = EM_PG_ChannelBase_LtoChat_AddUser;
    }
};
//-----------------------------------------------------------------------------------------------------
//�n�D�[�J�Y�����Y�@�W�D ���G
struct PG_ChannelBase_ChattoL_AddUserResult
{
    GamePGCommandEnum	Command;
    bool                Result;          
    GroupObjectTypeEnum Type;
    int                 ChannelID;
    int                 UserDBID;
    int                 SenderDBID;
	int					ChannelOwnerDBID;
	StaticString <_MAX_NAMESTRING_LEN_> ChannelName;    
	StaticString <_MAX_NAMESTRING_LEN_> ChannelOwnerName;    
    PG_ChannelBase_ChattoL_AddUserResult()
    {
        Command = EM_PG_ChannelBase_ChattoL_AddUserResult;
    }
};
//-----------------------------------------------------------------------------------------------------
//�q���Ҧ�channel�������a���H�[�J
//Chat -> C
struct PG_ChannelBase_ChattoC_AddUserNotify
{
	GamePGCommandEnum	Command;
	GroupObjectTypeEnum Type;
	int                 ChannelID;
	StaticString <_MAX_NAMESTRING_LEN_> Name;    
	PG_ChannelBase_ChattoC_AddUserNotify()
	{
		Command = EM_PG_ChannelBase_ChattoC_AddUserNotify;
	}
};
//-----------------------------------------------------------------------------------------------------
//�n�D�[�J�Y�����Y�@�W�D
struct PG_ChannelBase_LtoChat_DelUser
{
    GamePGCommandEnum	Command;
    GroupObjectTypeEnum Type;
    int                 ChannelID;
    int                 UserDBID;
    PG_ChannelBase_LtoChat_DelUser()
    {
        Command = EM_PG_ChannelBase_LtoChat_DelUser;
    }
};
//-----------------------------------------------------------------------------------------------------
//�n�D�[�J�Y�����Y�@�W�D ���G
struct PG_ChannelBase_ChattoL_DelUserResult
{
    GamePGCommandEnum	Command;
    bool                Result;          
    GroupObjectTypeEnum Type;
    int                 ChannelID;
    int                 UserDBID;
    PG_ChannelBase_ChattoL_DelUserResult()
    {
        Command = EM_PG_ChannelBase_ChattoL_DelUserResult;
    }
};
//-----------------------------------------------------------------------------------------------------
//�⨤��]�w���YChennel���֦���
struct PG_ChannelBase_ChattoL_SetChannelOwner
{
	GamePGCommandEnum	Command;
	GroupObjectTypeEnum Type;
	int                 ChannelID;
	int                 PlayerDBID;
	PG_ChannelBase_ChattoL_SetChannelOwner()
	{
		Command = EM_PG_ChannelBase_ChattoL_SetChannelOwner;
	}
};
//-----------------------------------------------------------------------------------------------------
//�q���Ҧ�channel�������a���H�[�J
//Chat -> C
struct PG_ChannelBase_ChattoC_DelUserNotify
{
	GamePGCommandEnum	Command;
	GroupObjectTypeEnum Type;
	int                 ChannelID;
	StaticString <_MAX_NAMESTRING_LEN_> Name;    
	PG_ChannelBase_ChattoC_DelUserNotify()
	{
		Command = EM_PG_ChannelBase_ChattoC_DelUserNotify;
	}
};
//-----------------------------------------------------------------------------------------------------
//Local -> Chat
//���a�i�J�ɳq��Chat �����
struct PG_ChannelBase_LtoChat_ChangeRoleInfo
{
    GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_> Account;
	OnlinePlayerInfoStruct Info;
	/*
    //�����T
    int		DBID;								    // �Ө��⪺ DBID (���ӬO�ߤ@)
	StaticString <_MAX_NAMESTRING_LEN_> RoleName;
    //�ϰ��T
    int		ZoneID;								// Local Server ������ ZoneID
    int		LocalID;							// �Ө���b Local �����ϰ�ID
*/
    PG_ChannelBase_LtoChat_ChangeRoleInfo()
    {
        Command = EM_PG_ChannelBase_LtoChat_ChangeRoleInfo;
    };
};
//-----------------------------------------------------------------------------------------------------
// Client �� Local ���� Channel ���ʥ]
//-----------------------------------------------------------------------------------------------------
//�n�D���ͤ@�� Channel
struct PG_ChannelBase_CtoL_CreateChannel
{
	GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_> ChannelName;
	StaticString <_MAX_NAMESTRING_LEN_> Password;
	PG_ChannelBase_CtoL_CreateChannel()
	{
		Command = EM_PG_ChannelBase_CtoL_CreateChannel;
	}
};
//-----------------------------------------------------------------------------------------------------
//���� Channel ���G
struct PG_ChannelBase_LtoC_CreateChannelResult
{
	GamePGCommandEnum	Command;
	bool                Result;          
	GroupObjectTypeEnum Type;
	int                 ChannelID;
	StaticString <_MAX_NAMESTRING_LEN_> ChannelName;
	PG_ChannelBase_LtoC_CreateChannelResult()
	{
		Command = EM_PG_ChannelBase_LtoC_CreateChannelResult;
	}
};
//-----------------------------------------------------------------------------------------------------
//�n�D �[�J��Y�@�W�D
struct PG_ChannelBase_CtoL_JoinChannel
{
	GamePGCommandEnum	Command;
	//int                 ChannelID;
	StaticString <_MAX_NAMESTRING_LEN_> ChannelName;
	StaticString <_MAX_NAMESTRING_LEN_> Password;
	GroupObjectTypeEnum Type;
	bool				IsForceJoin;
	PG_ChannelBase_CtoL_JoinChannel()
	{
		Command = EM_PG_ChannelBase_CtoL_JoinChannel;
	}
};
//-----------------------------------------------------------------------------------------------------
//�n�D�[�J�Y�����Y�@�W�D ���G
struct PG_ChannelBase_LtoC_JoinChannelResult
{
	GamePGCommandEnum	Command;
	bool                Result;          
	int                 ChannelID;
	GroupObjectTypeEnum Type;
	StaticString <_MAX_NAMESTRING_LEN_> ChannelName;
	int					ChannelOwnerDBID;
	StaticString <_MAX_NAMESTRING_LEN_> ChannelOwnerName;
	PG_ChannelBase_LtoC_JoinChannelResult()
	{
		Command = EM_PG_ChannelBase_LtoC_JoinChannelResult;
	}
};
//-----------------------------------------------------------------------------------------------------
//�n�D�[�J�Y�����Y�@�W�D
struct PG_ChannelBase_CtoL_LeaveChannel
{
	GamePGCommandEnum	Command;
	int                 ChannelID;
	GroupObjectTypeEnum Type;
	PG_ChannelBase_CtoL_LeaveChannel()
	{
		Command = EM_PG_ChannelBase_CtoL_LeaveChannel;
	}
};
//-----------------------------------------------------------------------------------------------------
//�n�D�[�J�Y�����Y�@�W�D ���G
struct PG_ChannelBase_LtoC_LeaveChannelResult
{
	GamePGCommandEnum	Command;
	bool                Result;          
	int                 ChannelID;
	GroupObjectTypeEnum Type;
	PG_ChannelBase_LtoC_LeaveChannelResult()
	{
		Command = EM_PG_ChannelBase_LtoC_LeaveChannelResult;
	}
};
//-----------------------------------------------------------------------------------------------------
// �����e
//-----------------------------------------------------------------------------------------------------
//�����e ��Y�@�s�� Cli
// �Ҧ� C -> chat
struct PG_ChannelBase_CtoChat_DataTransmitToCli_Group
{
	GamePGCommandEnum	Command;
	int                 ChannelID;
	GroupObjectTypeEnum Type;
	short	DataSize;
	char	Data[ 0x200 ];
	PG_ChannelBase_CtoChat_DataTransmitToCli_Group()
	{
		Command = EM_PG_ChannelBase_CtoChat_DataTransmitToCli_Group;
	}
	int     Size( )
	{
		return (sizeof(*this) + DataSize - sizeof(Data));
	}
};
//-----------------------------------------------------------------------------------------------------
//  Cli -> Cli ��e�z�L Chat
//  Chat -> C
struct PG_ChannelBase_ChattoC_DataTransmitCliToCli
{
	GamePGCommandEnum	Command;
	short	DataSize;
	char	Data[ 0x200 ];
	PG_ChannelBase_ChattoC_DataTransmitCliToCli()
	{
		Command = EM_PG_ChannelBase_ChattoC_DataTransmitCliToCli;
	}
	int     Size( )
	{
		return (sizeof(*this) + DataSize - sizeof(Data));
	}
};
//-----------------------------------------------------------------------------------------------------
//�����e ��Y�@�� Cli
// �Ҧ� C -> chat
struct PG_ChannelBase_CtoChat_DataTransmitToCli_Name
{
	GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_> Name;
	short	DataSize;
	char	Data[ 0x200 ];
	PG_ChannelBase_CtoChat_DataTransmitToCli_Name()
	{
		Command = EM_PG_ChannelBase_CtoChat_DataTransmitToCli_Name;
	}
	int     Size( )
	{
		return (sizeof(*this) + DataSize - sizeof(Data));
	}
};
//-----------------------------------------------------------------------------------------------------
//�����e ��Y�@�� Cli
// �Ҧ� Chat -> C
struct PG_ChannelBase_ChattoC_PlayerNotFind
{
	GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_> Name;
	PG_ChannelBase_ChattoC_PlayerNotFind()
	{
		Command = EM_PG_ChannelBase_ChattoC_PlayerNotFind;
	}
};
//-----------------------------------------------------------------------------------------------------
//�q��Cli�p����Chat
// Chat -> C
struct PG_ChannelBase_ChattoC_ClientConnectToChat
{
	GamePGCommandEnum	Command;
	PG_ChannelBase_ChattoC_ClientConnectToChat()
	{
		Command = EM_PG_ChannelBase_ChattoC_ClientConnectToChat;
	}
};
//-----------------------------------------------------------------------------------------------------
//�s�W���a
// Chat -> L
struct PG_ChannelBase_ChattoL_AddorSetOnlinePlayer
{
	GamePGCommandEnum		Command;
	OnlinePlayerInfoStruct	Info;
	PG_ChannelBase_ChattoL_AddorSetOnlinePlayer()
	{
		Command = EM_PG_ChannelBase_ChattoL_AddorSetOnlinePlayer;
	}
};
//-----------------------------------------------------------------------------------------------------
//���a���u
// Chat -> L
struct PG_ChannelBase_ChattoL_DelOnlinePlayer
{
	GamePGCommandEnum		Command;
	StaticString< _MAX_NAMERECORD_SIZE_ >   Name;
	int						DBID;
	PG_ChannelBase_ChattoL_DelOnlinePlayer()
	{
		Command = EM_PG_ChannelBase_ChattoL_DelOnlinePlayer;
	}
};
//-----------------------------------------------------------------------------------------------------
//�Y�W�D�����a��T(�ۤv�[�J��)
//C -> Chat
struct PG_ChannelBase_CtoChat_ChannelPlayerList
{
	GamePGCommandEnum		Command;
	GroupObjectTypeEnum		Type;
	int						ChannelID;
	PG_ChannelBase_CtoChat_ChannelPlayerList()
	{
		Command = EM_PG_ChannelBase_CtoChat_ChannelPlayerList;
	}
};
//-----------------------------------------------------------------------------------------------------
//�Y�W�D�����a��T(�ۤv���[�J���W�D)
//Chat -> C
struct PG_ChannelBase_ChattoC_ChannelPlayerListResult
{
	GamePGCommandEnum		Command;
	GroupObjectTypeEnum		Type;
	int						ChannelID;
	int						Count;		//�H��
	StaticString <_MAX_NAMESTRING_LEN_> PlayerNameList[ 0x1000 ];
	PG_ChannelBase_ChattoC_ChannelPlayerListResult()
	{
		Command = EM_PG_ChannelBase_ChattoC_ChannelPlayerListResult;
	}
	int PGSize()
	{
		int Size = sizeof(*this) - sizeof( PlayerNameList ) + Count * sizeof(PlayerNameList[0]);
		return Size;
	}
};
//-----------------------------------------------------------------------------------------------------
//���\���a�[�J�P�_���}��(�޲z��)
//C -> Chat
struct PG_ChannelBase_CtoChat_JoinPlayerDisabled
{
	GamePGCommandEnum		Command;
	GroupObjectTypeEnum		Type;
	int						ChannelID;	
	bool					JoinEnabled;
	PG_ChannelBase_CtoChat_JoinPlayerDisabled()
	{
		Command = EM_PG_ChannelBase_CtoChat_JoinPlayerDisabled;
	}
};
//-----------------------------------------------------------------------------------------------------
//���\���a�[�J�P�_���}�� 
//Chat -> C
struct PG_ChannelBase_ChattoC_JoinPlayerDisabledResult
{
	GamePGCommandEnum		Command;
	int						ChannelID;
	GroupObjectTypeEnum		Type;
	bool					JoinEnabled;
	PG_ChannelBase_ChattoC_JoinPlayerDisabledResult()
	{
		Command = EM_PG_ChannelBase_ChattoC_JoinPlayerDisabledResult;
	}
};
//-----------------------------------------------------------------------------------------------------
//��X�W�D(�޲z��)
//C -> Chat
struct PG_ChannelBase_CtoChat_KickPlayer
{
	GamePGCommandEnum		Command;
	GroupObjectTypeEnum		Type;
	int						ChannelID;
	StaticString <_MAX_NAMESTRING_LEN_> PlayerName;
	PG_ChannelBase_CtoChat_KickPlayer()
	{
		Command = EM_PG_ChannelBase_CtoChat_KickPlayer;
	}
};
//-----------------------------------------------------------------------------------------------------
//�]�w�޲z��(�޲z��)
//C -> Chat
struct PG_ChannelBase_CtoChat_SetManager
{
	GamePGCommandEnum		Command;
	GroupObjectTypeEnum		Type;
	int						ChannelID;
	StaticString <_MAX_NAMESTRING_LEN_> PlayerName;
	PG_ChannelBase_CtoChat_SetManager()
	{
		Command = EM_PG_ChannelBase_CtoChat_SetManager;
	}
};
//-----------------------------------------------------------------------------------------------------
//Chat -> Cli  �]�w�W�D�֦��̵��G
struct PG_ChannelBase_ChattoC_SetManagerResult
{
	GamePGCommandEnum		Command;
	GroupObjectTypeEnum		Type;
	int						ChannelID;
	bool					Result;
	PG_ChannelBase_ChattoC_SetManagerResult()
	{
		Command = EM_PG_ChannelBase_ChattoC_SetManagerResult;
	}
};
//-----------------------------------------------------------------------------------------------------
//Chat -> Cli  �q���Ҧ�Channel�������a���D�s���֦���
struct PG_ChannelBase_ChattoC_ChannelOwner
{
	GamePGCommandEnum		Command;
	GroupObjectTypeEnum		Type;
	int						ChannelID;
	StaticString <_MAX_NAMESTRING_LEN_> OwnerName;
	int						OwnerDBID;
	PG_ChannelBase_ChattoC_ChannelOwner()
	{
		Command = EM_PG_ChannelBase_ChattoC_ChannelOwner;
	}
};
//-----------------------------------------------------------------------------------------------------
//�q�������Ǥw�s�b���t���W�D
//Chat -> C
struct PG_ChannelBase_ChattoC_SysChannel
{
	GamePGCommandEnum		Command;
	int						ChannelID;
	GroupObjectTypeEnum		Type;
	StaticString <_MAX_NAMESTRING_LEN_> Name;
	PG_ChannelBase_ChattoC_SysChannel()
	{
		Command = EM_PG_ChannelBase_ChattoC_SysChannel;
	}
};
//-----------------------------------------------------------------------------------------------------
//�q��Chat�{�b Client �Ҧb���ϰ��W�D
//C -> Chat
struct PG_ChannelBase_CtoChat_ZoneChannelID
{
	GamePGCommandEnum		Command;
	int						MapZoneID;		//�a�ϰϰ�id
	PG_ChannelBase_CtoChat_ZoneChannelID()
	{
		Command = EM_PG_ChannelBase_CtoChat_ZoneChannelID;
	}
};
//-----------------------------------------------------------------------------------------------------
//�q�� Server �Τ@�t�ήɶ�
// Chat -> L
struct PG_ChannelBase_ChattoAll_SystemTime
{
	GamePGCommandEnum		Command;
	int			Time;
	int			TimeZone;
	PG_ChannelBase_ChattoAll_SystemTime()
	{
		Command = EM_PG_ChannelBase_ChattoAll_SystemTime;
	}
};
//-----------------------------------------------------------------------------------------------------
//�q�� Chat �L�ըt�ήɶ�
// L -> Chat
struct PG_ChannelBase_LtoChat_SetDSysTime
{
	GamePGCommandEnum		Command;
	int			Time;
	PG_ChannelBase_LtoChat_SetDSysTime()
	{
		Command = EM_PG_ChannelBase_LtoChat_SetDSysTime;
	}
};
//-----------------------------------------------------------------------------------------------------
//Cli to Cli��e����
struct PG_CtoC_Test
{
	CliToCliPGCommandEnum Command;
	char	TestData[128];
	PG_CtoC_Test()
	{
		Command = EM_PG_CtoC_Test;
		strcpy( TestData , "TEST" );
	}
};
//-----------------------------------------------------------------------------------------------------
//Cli to Cli��e�I���a�Ϫ��T��
struct PG_CtoC_MapPing
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> PingerName;
	int ZoneID;
	float fX;
	float fZ;
	PG_CtoC_MapPing()
	{
		Command = EM_PG_CtoC_MapPing;
	}
};
//-----------------------------------------------------------------------------------------------------
// Cli to Cli �ǰe�^���T��
struct PG_CtoC_Message
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> SenderName;			//�֭n�D��
	int messageType;
	char Content[256];
	PG_CtoC_Message()
	{
		Command = EM_PG_CtoC_Message;
		SenderName.Clear();
		memset(Content, 0, sizeof(Content));
	}
};
//-----------------------------------------------------------------------------------------------------
// �T�ݸӪ��a�O�_�������~
struct PG_CtoC_AskHasItem
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> SenderName;			//�֭n�D��
	int itemID;
	int itemVersion;
	PG_CtoC_AskHasItem()
	{
		Command = EM_PG_CtoC_AskHasItem;
		SenderName.Clear();
	}
};
//-----------------------------------------------------------------------------------------------------
// �T�ݸӪ��a�O�_�������~
struct PG_CtoC_HasItemResult
{
	CliToCliPGCommandEnum Command;
	int itemVersion;
	int itemID;
	bool hasItem;
	PG_CtoC_HasItemResult()
	{
		Command = EM_PG_CtoC_HasItemResult;		
		hasItem = false;
		itemVersion = 0;
		itemID = 0;
	}
};
//-----------------------------------------------------------------------------------------------------
// �T�ݸӪ��a�O�_�@�N�f�����M
struct PG_CtoC_AskRideMount
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> SenderName;			//�֭n�D��
	int GItemID;											//�ܽФH��ID
	PG_CtoC_AskRideMount()
	{
		Command = EM_PG_CtoC_AskRideMount;
		GItemID = 0;
		SenderName.Clear();
	}
};
//-----------------------------------------------------------------------------------------------------
// �^�����a������(�P�N�Τ��P�N)
struct PG_CtoC_RideMountResult
{
	CliToCliPGCommandEnum Command;
	int answer;												//0�P�N 1�ڵ� 2������(�����ܽ�)
	StaticString <_MAX_NAMESTRING_LEN_> name;				//�֦^����
	PG_CtoC_RideMountResult()
	{
		Command = EM_PG_CtoC_RideMountResult;		
		answer = 0;
		name.Clear();
	}
};
//-----------------------------------------------------------------------------------------------------
// Cli to Cli �]�w�ζ��ؼ�
struct PG_CtoC_SetRaidTarget
{
	CliToCliPGCommandEnum Command;
	int TargetID;
	int IconID;
	PG_CtoC_SetRaidTarget()
	{
		Command = EM_PG_CtoC_SetRaidTarget;
		TargetID = 0;
		IconID = 0;
	}
};
//-----------------------------------------------------------------------------------------------------
// Cli to Cli �l�ܪ��a
struct PG_CtoC_TrackPlayer
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> SenderName; //�֭n�D��
	PG_CtoC_TrackPlayer()
	{
		Command = EM_PG_CtoC_TrackPlayer;
		SenderName.Clear();
	}
};
//-----------------------------------------------------------------------------------------------------
// Cli to Cli �l�ܪ��a �^�����a�y��
struct PG_CtoC_TrackPlayerResult
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> TargetName; //
	int WorldMapID;
	float fX;
	float fY;
	float fZ;
	PG_CtoC_TrackPlayerResult()
	{
		Command = EM_PG_CtoC_TrackPlayerResult;
		TargetName.Clear();
	}
};
//-----------------------------------------------------------------------------------------------------
//Cli to Cli �n�D���oModleInfo
struct PG_CtoC_GetModleInfoRequest
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> RequesterName; //�֭n�D��
	PG_CtoC_GetModleInfoRequest()
	{
		Command = EM_PG_CtoC_GetModleInfoRequest;
	}
};
//-----------------------------------------------------------------------------------------------------
//Cli to Cli �Ǧ^���oModleInfo���G
#define DF_ModelInfoPart_Max 10
#define DF_ModelInfoBoneScale_Max 11
struct SModelInfoPart
{
	char name[32];
	int color[2];
};
struct SModelInfo 
{
	int actID;
	SModelInfoPart Parts[DF_ModelInfoPart_Max];
	float BoneScales[DF_ModelInfoBoneScale_Max];
};
struct PG_CtoC_GetModleInfoResult
{
	CliToCliPGCommandEnum Command;
	//StaticString <_MAX_NAMESTRING_LEN_> Name;
	int DBID;
	SModelInfo ModelInfo;
	PG_CtoC_GetModleInfoResult()
	{
		Command = EM_PG_CtoC_GetModleInfoResult;
	}
};
//-----------------------------------------------------------------------------------------------------
// Cli to Cli �n�D���o�n�͸�T
struct PG_CtoC_GiveMeYourFriendInfo
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> SenderName; //�֭n�D��
	PG_CtoC_GiveMeYourFriendInfo()
	{
		Command = EM_PG_CtoC_GiveMeYourFriendInfo;
		SenderName.Clear();
	}
};
//-----------------------------------------------------------------------------------------------------
// Cli to Cli �^���n�͸�T
struct SFriendInfo
{
	SFriendInfo()
	{
		TitleStr.Clear();
	}
	int			MapID;
	int         ZoneID;
 	int			TitleID;
  	StaticString< _MAX_NAMERECORD_SIZE_ > TitleStr;
	int			GuildID;
	Voc_ENUM	Voc;
	int			VocLV;
	Voc_ENUM	Voc_Sub;
	int			Voc_SubLV;
	Sex_ENUM	Sex;
  	StaticString< _MAX_FRIENDLIST_NOTE_ > Introduection;
};
struct PG_CtoC_GiveYouMyFriendInfo
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> Name; //
	SFriendInfo FriendInfo;
	PG_CtoC_GiveYouMyFriendInfo()
	{
		Command = EM_PG_CtoC_GiveYouMyFriendInfo;
	}
};
//�߰ݬO�_�O��誺�B��
struct PG_CtoC_AmIYourFriend 
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> AskerName;
	StaticString <_MAX_NAMESTRING_LEN_> AnswererName;
	FriendListTypeENUM FriendListType; 
	PG_CtoC_AmIYourFriend()
	{
		Command = EM_PG_CtoC_AmIYourFriend;
		FriendListType = EM_FriendListType_Friend;
	}
};
//�߰ݬO�_�O��誺�B�͵��G
struct PG_CtoC_AmIYourFriendResult 
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> AskerName;
	StaticString <_MAX_NAMESTRING_LEN_> AnswererName;
	FriendListTypeENUM FriendListType; 
	bool Result;
	PG_CtoC_AmIYourFriendResult()
	{
		Command = EM_PG_CtoC_AmIYourFriendResult;
		FriendListType = EM_FriendListType_Friend;
	}
};
//-----------------------------------------------------------------------------------------------------
// Cli to Cli �d�ߪ��a��T  shift+�����I�W�l �d�ߪ��a���
struct PG_CtoC_AskPlayerInfo
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> SenderName; //�֭n�D��
	PG_CtoC_AskPlayerInfo()
	{
		Command = EM_PG_CtoC_AskPlayerInfo;
		SenderName.Clear();
	}
};
// Cli to Cli �d�ߪ��a��T
struct SAskPlayerInfo
{
	SAskPlayerInfo()
	{
	}
	StaticString <_MAX_NAMESTRING_LEN_> Name; //
	int			MapID;
	int			GuildID;
	Voc_ENUM	Voc;
	int			VocLV;
	Voc_ENUM	Voc_Sub;
	int			Voc_SubLV;
	Sex_ENUM	Sex;
	Race_ENUM	Race;
};
// Cli to Cli �d�ߪ��a��T���G
struct PG_CtoC_AskPlayerInfoResult
{
	CliToCliPGCommandEnum Command;
	SAskPlayerInfo PlayerInfo;
	PG_CtoC_AskPlayerInfoResult()
	{
		Command = EM_PG_CtoC_AskPlayerInfoResult;
	}
};
//-----------------------------------------------------------------------------------------------------
// Cli to Cli �T�ݪ��a�O�_�n��|��
struct PG_CtoC_LeaderChange
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> SenderName;			//�֭n�D��
	PG_CtoC_LeaderChange()
	{
		Command = EM_PG_CtoC_LeaderChange;
	}
};
//-----------------------------------------------------------------------------------------------------
// ���a�O�_�n��|��
struct PG_CtoC_LeaderChangeResult
{
	CliToCliPGCommandEnum Command;
	int DBID;
	bool YesNo;
	PG_CtoC_LeaderChangeResult()
	{
		Command = EM_PG_CtoC_LeaderChangeResult;	
		DBID=0;
		YesNo = false;
	}
};
//-----------------------------------------------------------------------------------------------------
// Cli to Cli �ܽХ[�J�W�D
struct PG_CtoC_ChannelInvite
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> SenderName; //�֭n�D��
	StaticString <_MAX_NAMESTRING_LEN_> ChannelName; //�W�D�W��
	StaticString <_MAX_NAMESTRING_LEN_> ChannelPass; //�W�D�K�X
	PG_CtoC_ChannelInvite()
	{
		Command = EM_PG_CtoC_ChannelInvite;
		SenderName.Clear();
	}
};

//-----------------------------------------------------------------------------------------------------
#pragma managed