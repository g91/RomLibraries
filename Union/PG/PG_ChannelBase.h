#pragma once
#pragma warning (disable:4949)
#include "NetWakerPGEnum.h"
#include "roledata/RoleDataEx.h"
#pragma unmanaged

//-----------------------------------------------------------------------------------------------------
//資料轉送 到某一群組Srv 
// 所有 Srv -> Chat
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
//資料轉送 到某一群組Cli
// 所有 Srv -> Chat
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
//資料轉送 到某人的Srv
// 所有 Srv -> Chat
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
//資料轉送 到某人的Cli
// 所有 Srv -> Chat
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
//資料註冊 或 產生
//-----------------------------------------------------------------------------------------------------
//要求產生一個 Channel
struct PG_ChannelBase_LtoChat_CreateChannel
{
    GamePGCommandEnum	Command;
    GroupObjectTypeEnum Type;
    int                 UserDBID;       //命令要求者
	StaticString <_MAX_NAMESTRING_LEN_> Name;    
	StaticString <_MAX_NAMESTRING_LEN_> Password;
    PG_ChannelBase_LtoChat_CreateChannel()
    {
        Command = EM_PG_ChannelBase_LtoChat_CreateChannel;
    }
};
//-----------------------------------------------------------------------------------------------------
//產生 Channel 結果
struct PG_ChannelBase_ChattoL_CreateChannelResult
{
    GamePGCommandEnum	Command;
    bool                Result;          
    GroupObjectTypeEnum Type;
    int                 ChannelID;
	StaticString<_MAX_NAMESTRING_LEN_> Name;    
    int                 UserDBID;       //命令要求者
    PG_ChannelBase_ChattoL_CreateChannelResult()
    {
        Command = EM_PG_ChannelBase_ChattoL_CreateChannelResult;
    }
};
//-----------------------------------------------------------------------------------------------------
//要求加入某角色到某一頻道
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
//要求加入某角色到某一頻道 結果
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
//通知所有channel內的玩家有人加入
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
//要求加入某角色到某一頻道
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
//要求加入某角色到某一頻道 結果
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
//把角色設定成某Chennel的擁有者
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
//通知所有channel內的玩家有人加入
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
//當玩家進入時通知Chat 的資料
struct PG_ChannelBase_LtoChat_ChangeRoleInfo
{
    GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_> Account;
	OnlinePlayerInfoStruct Info;
	/*
    //角色資訊
    int		DBID;								    // 該角色的 DBID (應該是唯一)
	StaticString <_MAX_NAMESTRING_LEN_> RoleName;
    //區域資訊
    int		ZoneID;								// Local Server 本身的 ZoneID
    int		LocalID;							// 該角色在 Local 中的區域ID
*/
    PG_ChannelBase_LtoChat_ChangeRoleInfo()
    {
        Command = EM_PG_ChannelBase_LtoChat_ChangeRoleInfo;
    };
};
//-----------------------------------------------------------------------------------------------------
// Client 到 Local 產生 Channel 的封包
//-----------------------------------------------------------------------------------------------------
//要求產生一個 Channel
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
//產生 Channel 結果
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
//要求 加入到某一頻道
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
//要求加入某角色到某一頻道 結果
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
//要求加入某角色到某一頻道
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
//要求加入某角色到某一頻道 結果
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
// 資料轉送
//-----------------------------------------------------------------------------------------------------
//資料轉送 到某一群組 Cli
// 所有 C -> chat
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
//  Cli -> Cli 轉送透過 Chat
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
//資料轉送 到某一個 Cli
// 所有 C -> chat
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
//資料轉送 到某一個 Cli
// 所有 Chat -> C
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
//通知Cli聯結到Chat
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
//新增玩家
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
//玩家離線
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
//某頻道的玩家資訊(自己加入的)
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
//某頻道的玩家資訊(自己有加入的頻道)
//Chat -> C
struct PG_ChannelBase_ChattoC_ChannelPlayerListResult
{
	GamePGCommandEnum		Command;
	GroupObjectTypeEnum		Type;
	int						ChannelID;
	int						Count;		//人數
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
//允許玩家加入與否的開關(管理者)
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
//允許玩家加入與否的開關 
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
//踢出頻道(管理者)
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
//設定管理者(管理者)
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
//Chat -> Cli  設定頻道擁有者結果
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
//Chat -> Cli  通知所有Channel中的玩家知道新的擁有者
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
//通知有那些已存在的系統頻道
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
//通知Chat現在 Client 所在的區域頻道
//C -> Chat
struct PG_ChannelBase_CtoChat_ZoneChannelID
{
	GamePGCommandEnum		Command;
	int						MapZoneID;		//地圖區域id
	PG_ChannelBase_CtoChat_ZoneChannelID()
	{
		Command = EM_PG_ChannelBase_CtoChat_ZoneChannelID;
	}
};
//-----------------------------------------------------------------------------------------------------
//通知 Server 統一系統時間
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
//通知 Chat 微調系統時間
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
//Cli to Cli轉送測試
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
//Cli to Cli轉送點擊地圖的訊息
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
// Cli to Cli 傳送回應訊息
struct PG_CtoC_Message
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> SenderName;			//誰要求的
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
// 訊問該玩家是否有此物品
struct PG_CtoC_AskHasItem
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> SenderName;			//誰要求的
	int itemID;
	int itemVersion;
	PG_CtoC_AskHasItem()
	{
		Command = EM_PG_CtoC_AskHasItem;
		SenderName.Clear();
	}
};
//-----------------------------------------------------------------------------------------------------
// 訊問該玩家是否有此物品
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
// 訊問該玩家是否願意搭乘坐騎
struct PG_CtoC_AskRideMount
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> SenderName;			//誰要求的
	int GItemID;											//邀請人的ID
	PG_CtoC_AskRideMount()
	{
		Command = EM_PG_CtoC_AskRideMount;
		GItemID = 0;
		SenderName.Clear();
	}
};
//-----------------------------------------------------------------------------------------------------
// 回應玩家的答案(同意或不同意)
struct PG_CtoC_RideMountResult
{
	CliToCliPGCommandEnum Command;
	int answer;												//0同意 1拒絕 2忙錄中(受到邀請)
	StaticString <_MAX_NAMESTRING_LEN_> name;				//誰回應的
	PG_CtoC_RideMountResult()
	{
		Command = EM_PG_CtoC_RideMountResult;		
		answer = 0;
		name.Clear();
	}
};
//-----------------------------------------------------------------------------------------------------
// Cli to Cli 設定團隊目標
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
// Cli to Cli 追蹤玩家
struct PG_CtoC_TrackPlayer
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> SenderName; //誰要求的
	PG_CtoC_TrackPlayer()
	{
		Command = EM_PG_CtoC_TrackPlayer;
		SenderName.Clear();
	}
};
//-----------------------------------------------------------------------------------------------------
// Cli to Cli 追蹤玩家 回報玩家座標
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
//Cli to Cli 要求取得ModleInfo
struct PG_CtoC_GetModleInfoRequest
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> RequesterName; //誰要求的
	PG_CtoC_GetModleInfoRequest()
	{
		Command = EM_PG_CtoC_GetModleInfoRequest;
	}
};
//-----------------------------------------------------------------------------------------------------
//Cli to Cli 傳回取得ModleInfo結果
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
// Cli to Cli 要求取得好友資訊
struct PG_CtoC_GiveMeYourFriendInfo
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> SenderName; //誰要求的
	PG_CtoC_GiveMeYourFriendInfo()
	{
		Command = EM_PG_CtoC_GiveMeYourFriendInfo;
		SenderName.Clear();
	}
};
//-----------------------------------------------------------------------------------------------------
// Cli to Cli 回報好友資訊
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
//詢問是否是對方的朋友
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
//詢問是否是對方的朋友結果
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
// Cli to Cli 查詢玩家資訊  shift+左鍵點名子 查詢玩家資料
struct PG_CtoC_AskPlayerInfo
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> SenderName; //誰要求的
	PG_CtoC_AskPlayerInfo()
	{
		Command = EM_PG_CtoC_AskPlayerInfo;
		SenderName.Clear();
	}
};
// Cli to Cli 查詢玩家資訊
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
// Cli to Cli 查詢玩家資訊結果
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
// Cli to Cli 訊問玩家是否要當會長
struct PG_CtoC_LeaderChange
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> SenderName;			//誰要求的
	PG_CtoC_LeaderChange()
	{
		Command = EM_PG_CtoC_LeaderChange;
	}
};
//-----------------------------------------------------------------------------------------------------
// 玩家是否要當會長
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
// Cli to Cli 邀請加入頻道
struct PG_CtoC_ChannelInvite
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> SenderName; //誰要求的
	StaticString <_MAX_NAMESTRING_LEN_> ChannelName; //頻道名稱
	StaticString <_MAX_NAMESTRING_LEN_> ChannelPass; //頻道密碼
	PG_CtoC_ChannelInvite()
	{
		Command = EM_PG_CtoC_ChannelInvite;
		SenderName.Clear();
	}
};

//-----------------------------------------------------------------------------------------------------
#pragma managed