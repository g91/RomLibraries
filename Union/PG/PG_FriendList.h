#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged
//------------------------------------------------------------------------------------------------------------
//Local -> Client 
//�n���W��
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
//�]�w���J�n�ͦW��n�D
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
//�]�w���J�n�ͦW�浲�G
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
//�ץ�Chat�n�ͦW��W�u�q����T
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
// �n�ͤW�u�q��
// Chat -> Client
struct PG_FriendList_ChattoC_PlayerOnlineInfo
{
	GamePGCommandEnum	Command;
	int										PlayerDBID;
	StaticString< _MAX_NAMERECORD_SIZE_ >   RoleName;	    //����W��

	Voc_ENUM		Voc;
	Voc_ENUM		Voc_Sub;
	Sex_ENUM		Sex;
	Race_ENUM		Race;
	int				LV;
	int				LV_Sub;
	int				GuildID;	//���| ID
	int				TitleID;


	PG_FriendList_ChattoC_PlayerOnlineInfo( )
	{	Command = EM_PG_FriendList_ChattoC_PlayerOnlineInfo;	}
};
//------------------------------------------------------------------------------------------------------------
// �n�ͤW�u�q��
// Chat -> Client
struct PG_FriendList_ChattoC_PlayerOfflineInfo
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;

	PG_FriendList_ChattoC_PlayerOfflineInfo( )
	{	Command = EM_PG_FriendList_ChattoC_PlayerOfflineInfo;	}
};
//------------------------------------------------------------------------------------------------------------
//�]�w���J�n�ͦW��n�D
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
//�]�w���J�n�ͦW�浲�G
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
//�]�w���J�n�ͦW��n�D
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
//�]�w���J�n�ͦW�浲�G
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
//�]�w�ۤv������
//Client -> Local
struct PG_FriendList_CtoL_ModifyNote
{
	GamePGCommandEnum	Command;

	StaticString< _MAX_FRIENDLIST_NOTE_ >   Note;	    //�ۤv����

	PG_FriendList_CtoL_ModifyNote( )
	{	Command = EM_PG_FriendList_CtoL_ModifyNote;	}
};
//------------------------------------------------------------------------------------------------------------
//�ץ��n��Client �ݸ��
//Client -> Local
struct PG_FriendList_CtoL_FixClientData
{
	GamePGCommandEnum	Command;

	int		Pos;
	char	CliData[ _MAX_FRIENDLIST_NOTE_ ];	    //�ۤv����

	PG_FriendList_CtoL_FixClientData( )
	{	Command = EM_PG_FriendList_CtoL_FixClientData;	}
};
//------------------------------------------------------------------------------------------------------------
//�ץ� �n�ͥؿ��Ƨǭ�
//Client -> Server
struct PG_FriendList_CtoL_FixGroupSortID
{
	GamePGCommandEnum	Command;
	char	GroupSortID[ _MAX_FRIENDLIST_GROUP_COUNT_  ];

	PG_FriendList_CtoL_FixGroupSortID( )
	{	Command = EM_PG_FriendList_CtoL_FixGroupSortID;	}		
};
//------------------------------------------------------------------------------------------------------------
//�ץ��n�͸��
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
//�j�M�䶤����a
struct PG_Friend_CtoL_SearchFindPartyPlayerRequest
{
	GamePGCommandEnum	Command;

	Voc_ENUM		    MainJob;							//¾�~
	Voc_ENUM		    SubJob;								//��¾�~
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
	StaticString <_MAX_NAMESTRING_LEN_> Name;		//�n�䪺�H
	Voc_ENUM		    MainJob;					//¾�~
	Voc_ENUM		    SubJob;						//��¾�~
	int					MainJobLv;
	int					SubJobLv;

	int					TitleID;
	int					GuildID;
	int					ZoneID;

	StaticString< _MAX_NAMERECORD_SIZE_ >   TitleStr;	//�۩w�ٸ�
};

//�j�M�䶤����a
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
