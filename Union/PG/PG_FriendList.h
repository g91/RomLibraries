#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged
//------------------------------------------------------------------------------------------------------------
//Local -> Client 
//好有名單
struct PG_FriendList_LtoC_FriendListInfo_Zip
{
	GamePGCommandEnum	Command;
	int					Size;
	char				Data[0x10000];

	int	PGSize()
	{
		return sizeof(int)*2+ Size;
	}

	PG_FriendList_LtoC_FriendListInfo_Zip( )
	{	Command = EM_PG_FriendList_LtoC_FriendListInfo_Zip;	}
};

//------------------------------------------------------------------------------------------------------------
//設定插入好友名單要求
//Client -> Local
struct PG_FriendList_CtoL_InsertFriend
{
	GamePGCommandEnum	Command;
	
	FriendListTypeENUM	Type;
	int					Pos;

	BaseFriendStruct	Info;

	PG_FriendList_CtoL_InsertFriend( )
	{	Command = EM_PG_FriendList_CtoL_InsertFriend;	}
};
//------------------------------------------------------------------------------------------------------------
//設定插入好友名單結果
//Local -> Client
struct PG_FriendList_LtoC_InsertFriendResult
{
	GamePGCommandEnum	Command;

	FriendListTypeENUM		Type;
	int						Pos;
	bool					Result;

	PG_FriendList_LtoC_InsertFriendResult( )
	{	Command = EM_PG_FriendList_LtoC_InsertFriendResult;	}
};
//------------------------------------------------------------------------------------------------------------
//修正Chat好友名單上線通知資訊
//Local -> Chat
struct PG_FriendList_LtoChat_FriendListInfo
{
	GamePGCommandEnum	Command;
	int		PlayerDBID;
	int		FriendDBID;
	bool	IsInsert;			// true:Insert  false:Erase
	
	PG_FriendList_LtoChat_FriendListInfo( )
	{	Command = EM_PG_FriendList_LtoChat_FriendListInfo;	}
};
//------------------------------------------------------------------------------------------------------------
// 好友上線通知
// Chat -> Client
struct PG_FriendList_ChattoC_PlayerOnlineInfo
{
	GamePGCommandEnum	Command;
	int										PlayerDBID;
	StaticString< _MAX_NAMERECORD_SIZE_ >   RoleName;	    //角色名稱

	Voc_ENUM		Voc;
	Voc_ENUM		Voc_Sub;
	Sex_ENUM		Sex;
	Race_ENUM		Race;
	int				LV;
	int				LV_Sub;
	int				GuildID;	//公會 ID
	int				TitleID;


	PG_FriendList_ChattoC_PlayerOnlineInfo( )
	{	Command = EM_PG_FriendList_ChattoC_PlayerOnlineInfo;	}
};
//------------------------------------------------------------------------------------------------------------
// 好友上線通知
// Chat -> Client
struct PG_FriendList_ChattoC_PlayerOfflineInfo
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;

	PG_FriendList_ChattoC_PlayerOfflineInfo( )
	{	Command = EM_PG_FriendList_ChattoC_PlayerOfflineInfo;	}
};
//------------------------------------------------------------------------------------------------------------
//設定插入好友名單要求
//Client -> Local
struct PG_FriendList_CtoL_EraseFriend
{
	GamePGCommandEnum	Command;

	FriendListTypeENUM	Type;
	int					DBID;


	PG_FriendList_CtoL_EraseFriend( )
	{	Command = EM_PG_FriendList_CtoL_EraseFriend;	}
};

//------------------------------------------------------------------------------------------------------------
//設定插入好友名單結果
//Local -> Client
struct PG_FriendList_LtoC_EraseFriendResult
{
	GamePGCommandEnum	Command;

	FriendListTypeENUM		Type;
	int						DBID;
	bool					Result;

	PG_FriendList_LtoC_EraseFriendResult( )
	{	Command = EM_PG_FriendList_LtoC_EraseFriendResult;	}
};
//------------------------------------------------------------------------------------------------------------
//設定插入好友名單要求
//Client -> Local
struct PG_FriendList_CtoL_ModifyFriend
{
	GamePGCommandEnum	Command;

	FriendListTypeENUM	Type;
	BaseFriendStruct	Info;

	PG_FriendList_CtoL_ModifyFriend( )
	{	Command = EM_PG_FriendList_CtoL_ModifyFriend;	}
};
//------------------------------------------------------------------------------------------------------------
//設定插入好友名單結果
//Local -> Client
struct PG_FriendList_LtoC_ModifyFriendResult
{
	GamePGCommandEnum	Command;

	FriendListTypeENUM		Type;
	int						DBID;
	bool					Result;

	PG_FriendList_LtoC_ModifyFriendResult( )
	{	Command = EM_PG_FriendList_LtoC_ModifyFriendResult;	}
};
//------------------------------------------------------------------------------------------------------------
//設定自己的註解
//Client -> Local
struct PG_FriendList_CtoL_ModifyNote
{
	GamePGCommandEnum	Command;

	StaticString< _MAX_FRIENDLIST_NOTE_ >   Note;	    //自己註解

	PG_FriendList_CtoL_ModifyNote( )
	{	Command = EM_PG_FriendList_CtoL_ModifyNote;	}
};
//------------------------------------------------------------------------------------------------------------
//修正好友Client 端資料
//Client -> Local
struct PG_FriendList_CtoL_FixClientData
{
	GamePGCommandEnum	Command;

	int		Pos;
	char	CliData[ _MAX_FRIENDLIST_NOTE_ ];	    //自己註解

	PG_FriendList_CtoL_FixClientData( )
	{	Command = EM_PG_FriendList_CtoL_FixClientData;	}
};
//------------------------------------------------------------------------------------------------------------
//修正 好友目錄排序值
//Client -> Server
struct PG_FriendList_CtoL_FixGroupSortID
{
	GamePGCommandEnum	Command;
	char	GroupSortID[ _MAX_FRIENDLIST_GROUP_COUNT_  ];

	PG_FriendList_CtoL_FixGroupSortID( )
	{	Command = EM_PG_FriendList_CtoL_FixGroupSortID;	}		
};
//------------------------------------------------------------------------------------------------------------
//修正好友資料
//Local -> Client
struct PG_FriendList_LtoC_FixFriendInfo
{
	GamePGCommandEnum	Command;

	FriendListTypeENUM	Type;
	int					Pos;
	BaseFriendStruct	Info;

	PG_FriendList_LtoC_FixFriendInfo( )
	{	Command = EM_PG_FriendList_LtoC_FixFriendInfo;	}
};
//------------------------------------------------------------------------------------------------------------
//搜尋找隊伍的玩家
struct PG_Friend_CtoL_SearchFindPartyPlayerRequest
{
	GamePGCommandEnum	Command;

	Voc_ENUM		    MainJob;							//職業
	Voc_ENUM		    SubJob;								//副職業
	int					MinMainJobLv;
	int					MaxMainJobLv;
	int					MinSubJobLv;
	int					MaxSubJobLv;

	PG_Friend_CtoL_SearchFindPartyPlayerRequest()
	{
		Command = EM_PG_Friend_CtoL_SearchFindPartyPlayerRequest;
	}
};
//------------------------------------------------------------------------------------------------------------
struct SearchFindPartyPlayerStruct
{
	StaticString <_MAX_NAMESTRING_LEN_> Name;		//要找的人
	Voc_ENUM		    MainJob;					//職業
	Voc_ENUM		    SubJob;						//副職業
	int					MainJobLv;
	int					SubJobLv;

	int					TitleID;
	int					GuildID;
	int					ZoneID;

	StaticString< _MAX_NAMERECORD_SIZE_ >   TitleStr;	//自定稱號
};

//搜尋找隊伍的玩家
struct PG_Friend_LtoC_SearchFindPartyPlayerResult
{
	GamePGCommandEnum	Command;
	int			Count;
	int			Index;
	SearchFindPartyPlayerStruct Info;

	PG_Friend_LtoC_SearchFindPartyPlayerResult()
	{
		Command = EM_PG_Friend_LtoC_SearchFindPartyPlayerResult;
	}
};
