#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged
//#define MAX_TALK_CONTENT				512
//-------------------------------------------------------------------
// �n��
//-------------------------------------------------------------------
// C1 -> Chat C1 �n�� C2 
// Chat -> C2 C1 �n�� C2
// C2 -> Chat C2 �n�[�J�ݩ� C1 �� Party
	//1 �p�G C1 �S Party�۰ʲ���
	//2 �[�J C1 �� Party
// Chat -> Local ��s Party ���c��T
// Chat -> Local �[�JParty����
// Chat -> Local ����Party����

// Chat -> C ��s Party ���c��T
// Chat -> C �[�JParty����
// Chat -> C ����Party����

//-------------------------------------------------------------------
// ��L
//-------------------------------------------------------------------
// C -> Chat �� �����
// C -> Chat �]�w����W�h
// C -> Chat �]�w����
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// �ܽ�
//-------------------------------------------------------------------
// C1 -> Chat C1 �ܽ� C2 
struct PG_Party_CtoChat_Invite
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_> InviteName;
	int					ClientInfo;	

	PG_Party_CtoChat_Invite()
	{	Command = EM_PG_Party_CtoChat_Invite;	};
};
//-------------------------------------------------------------------
// Chat -> C2 C1 �ܽ� C2
struct PG_Party_ChattoC_Invite
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_> LeaderName;
	int					ClientInfo;	

	PG_Party_ChattoC_Invite()
	{	Command = EM_PG_Party_ChattoC_Invite;	};
};

// ¾�~��T
struct SJobInfo
{
	SJobInfo()
	{
		Voc			= EM_Vocation_None;
		VocLV		= 0;
		Voc_Sub		= EM_Vocation_None;
		Voc_SubLV	= 0;
	}

	Voc_ENUM	Voc;
	int			VocLV;
	Voc_ENUM	Voc_Sub;
	int			Voc_SubLV;
};

//-------------------------------------------------------------------
// C1 -> C2 --- C1 �ӽХ[�J C2 ������
struct PG_Party_CtoC_AskJoinToParty
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> AskerName; // �֥ӽЪ�
	SJobInfo sJobInfo;

	PG_Party_CtoC_AskJoinToParty()
	{
		Command = EM_PG_Party_CtoC_AskJoinToParty;
		AskerName.Clear();
	}
};

//-------------------------------------------------------------------
// C2 -> C1 --- C2 �� C1 �ӽХ[�J����^��
struct PG_Party_CtoC_AskJoinToPartyResult
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> LeaderName; // �֦^����
	bool bYesNo;

	PG_Party_CtoC_AskJoinToPartyResult()
	{
		Command = EM_PG_Party_CtoC_AskJoinToPartyResult;
		bYesNo = false;
	}
};

//-------------------------------------------------------------------
// C1 -> C2 --- C1 �n�D C2 �R�� C2 �b�ն��۶үd���O�W���d��
struct PG_Party_CtoC_DeletePBPost
{
	CliToCliPGCommandEnum Command;

	PG_Party_CtoC_DeletePBPost()
	{
		Command = EM_PG_Party_CtoC_DeletePBPost;
	}
};

//-------------------------------------------------------------------
// C2 -> Chat C2 �n�[�J�ݩ� C1 �� Party
	//1 �p�G C1 �S Party�۰ʲ���
	//2 �[�J C1 �� Party
struct PG_Party_CtoChat_Join
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_> LeaderName;

	PG_Party_CtoChat_Join()
	{	Command = EM_PG_Party_CtoChat_Join;	};
};
//-------------------------------------------------------------------
// Local -> Chat �n�D�Y�H�[�J��Y��Party,(���s�n�J)
struct PG_Party_LtoChat_PartyRelogin
{
	GamePGCommandEnum	Command;
	int	PartyID;
	int	PlayerDBID;

	PG_Party_LtoChat_PartyRelogin()
	{	Command = EM_PG_Party_LtoChat_PartyRelogin;	};
};
//-------------------------------------------------------------------
// Chat -> Local 
struct PG_Party_ChattoL_PartyReloginResult
{
	GamePGCommandEnum	Command;
	int		PartyID;
	int		PlayerDBID;
	bool	Result;

	PG_Party_ChattoL_PartyReloginResult()
	{	Command = EM_PG_Party_ChattoL_PartyReloginResult;	};
};

//-------------------------------------------------------------------
// Chat -> Local ��s Party ���c��T ( �e���Ҧ�Server )
struct PG_Party_ChattoL_PartyBaseInfo
{
	GamePGCommandEnum	Command;
	PartyBaseInfoStruct Info;
	

	PG_Party_ChattoL_PartyBaseInfo()
	{	Command = EM_PG_Party_ChattoL_PartyBaseInfo;	};
};
//-------------------------------------------------------------------
// Chat -> Local �[�JParty����( �e���Ҧ�Server )
struct PG_Party_ChattoL_AddMember
{
	GamePGCommandEnum	Command;
	int					PartyID;
	PartyMemberInfoStruct NewMember;

	PG_Party_ChattoL_AddMember()
	{	Command = EM_PG_Party_ChattoL_AddMember;	};
};
//-------------------------------------------------------------------
// Chat -> Local ����Party����( �e���Ҧ�Server )
struct PG_Party_ChattoL_DelMember
{
	GamePGCommandEnum		Command;
	int						PartyID;
	PartyMemberInfoStruct	Member;
	bool					IsOffline;
//	StaticString <_MAX_NAMESTRING_LEN_> DelMemberName;

	PG_Party_ChattoL_DelMember()
	{	Command = EM_PG_Party_ChattoL_DelMember;	};
};
//-------------------------------------------------------------------
// Chat -> L ��s������T
struct PG_Party_ChattoL_FixMember
{
	GamePGCommandEnum	Command;
	int					PartyID;
	PartyMemberInfoStruct Member;

	PG_Party_ChattoL_FixMember()
	{	Command = EM_PG_Party_ChattoL_FixMember;	};
};
//-------------------------------------------------------------------
// Chat -> C ��s Party ���c��T
struct PG_Party_ChattoC_PartyBaseInfo
{
	GamePGCommandEnum	Command;
	PartyBaseInfoStruct Info;


	PG_Party_ChattoC_PartyBaseInfo()
	{	Command = EM_PG_Party_ChattoC_PartyBaseInfo;	};
};
//-------------------------------------------------------------------
// Chat -> C �[�JParty����
struct PG_Party_ChattoC_AddMember
{
	GamePGCommandEnum	Command;
	PartyMemberInfoStruct NewMember;

	PG_Party_ChattoC_AddMember()
	{	Command = EM_PG_Party_ChattoC_AddMember;	};
};
//-------------------------------------------------------------------
// Chat -> C ����Party����
struct PG_Party_ChattoC_DelMember
{
	GamePGCommandEnum	Command;
	PartyMemberInfoStruct Member;
	//StaticString <_MAX_NAMESTRING_LEN_> DelMemberName;

	PG_Party_ChattoC_DelMember()
	{	Command = EM_PG_Party_ChattoC_DelMember;	};
};
//-------------------------------------------------------------------
// Chat -> C ��s������T
struct PG_Party_ChattoC_FixMember
{
	GamePGCommandEnum	Command;
	int					PartyID;
	PartyMemberInfoStruct Member;

	PG_Party_ChattoC_FixMember()
	{	Command = EM_PG_Party_ChattoC_FixMember;	};
};
//-------------------------------------------------------------------
// ��L
//-------------------------------------------------------------------
// C -> Chat �� �����
struct PG_Party_CtoChat_KickMember
{
	GamePGCommandEnum	Command;	
	StaticString <_MAX_NAMESTRING_LEN_> MemberName;
	int					PartyID;

	PG_Party_CtoChat_KickMember()
	{	Command = EM_PG_Party_CtoChat_KickMember;	};
};
//-------------------------------------------------------------------
// C -> Chat �]�w����W�h
struct PG_Party_CtoChat_PartyRule
{
	GamePGCommandEnum	Command;	
	PartyBaseInfoStruct PartyInfo;

	PG_Party_CtoChat_PartyRule()
	{	Command = EM_PG_Party_CtoChat_PartyRule;	};
};
//-------------------------------------------------------------------
// Party����������T
//-------------------------------------------------------------------
/*
struct PartyMemberBaseInfo
{
//	StaticString <_MAX_NAMESTRING_LEN_> Name;
	int		DBID;
	int		HP,MP,SP,SP_Sub;
	int		MaxHP,MaxMP,MaxSP,MaxSP_Sub;
};
struct PG_Party_LtoC_PartyMemberInfo
{
	GamePGCommandEnum	Command;	
	
	PartyMemberBaseInfo Info;

	PG_Party_LtoC_PartyMemberInfo()
	{	Command = EM_PG_Party_LtoC_PartyMemberInfo;	};
};
*/
//-------------------------------------------------------------------
//����ԧQ�~�����ʥ] ( ���� )
//-------------------------------------------------------------------
// L -> Chat Party����o�ԧQ�~
struct PG_Party_LtoChat_PartyTreasure
{
	GamePGCommandEnum	Command;

	int					PartyID;
	ItemFieldStruct		Item;
	StaticVector< int , _MAX_PARTY_COUNT_  > LootDBID;

	PG_Party_LtoChat_PartyTreasure()
	{	Command = EM_PG_Party_LtoChat_PartyTreasure;	};
};
//-------------------------------------------------------------------
// Chat -> C Party����o�ԧQ�~
struct PG_Party_ChattoC_PartyTreasure
{
	GamePGCommandEnum	Command;	
	int					ItemVersion;
	ItemFieldStruct		Item;
	StaticVector< int , _MAX_PARTY_COUNT_  > LootDBID;
	PartyItemShareENUM	LootType;	//�Ҧ�

	PG_Party_ChattoC_PartyTreasure()
	{	Command = EM_PG_Party_ChattoC_PartyTreasure;	};
};
//-------------------------------------------------------------------
// Chat -> C Party����o�ԧQ�~
struct PG_Party_ChattoC_PartyTreasureLoot
{
	GamePGCommandEnum	Command;	
	int					ItemVersion;
	int					DBID;		//���Ӫ��a
	int					LootValue;	//0   �N����

	PG_Party_ChattoC_PartyTreasureLoot()
	{	Command = EM_PG_Party_ChattoC_PartyTreasureLoot;	};
};
//-------------------------------------------------------------------
enum  PartyTreasureLootTypeENUM
{
	EM_PartyTreasureLootType_Loot	,
	EM_PartyTreasureLootType_Pass	,	

	EM_PartyTreasureLootType_Assist	,	//���w
	EM_PartyTreasureLootType_Loot_NotNeed	,	//�D���n
};
// C -> Chat	�]�w��l
struct PG_Party_CtoChat_PartyTreasureLoot
{
	GamePGCommandEnum	Command;	
	int					ItemVersion;
	PartyTreasureLootTypeENUM Type;
	int					DBID;			//���w���Ӫ��a(EM_PartyTreasureLootType_Assist �~���� )

	PG_Party_CtoChat_PartyTreasureLoot()
	{	Command = EM_PG_Party_CtoChat_PartyTreasureLoot;	};
};


//-------------------------------------------------------------------
// Chat -> L �Y�H�o�쪫�~
struct PG_Party_ChattoL_GiveItem
{
	GamePGCommandEnum	Command;	

	int					DBID;
	int					GItemID;
	ItemFieldStruct		Item;	

	PG_Party_ChattoL_GiveItem()
	{	Command = EM_PG_Party_ChattoL_GiveItem;	};
};
//-------------------------------------------------------------------
// Chat -> L �Y�H�o�쪫�~
struct PG_Party_ChattoC_GiveItemInfo
{
	GamePGCommandEnum	Command;	

	int					DBID;
	ItemFieldStruct		Item;	
	int					Version;

	PG_Party_ChattoC_GiveItemInfo()
	{	Command = EM_PG_Party_ChattoC_GiveItemInfo;	};
};
//-------------------------------------------------------------------
// Local -> Client
//�q��Party�������H���u
struct PG_Party_LtoC_MemberLogout
{
	GamePGCommandEnum	Command;	
	int					MemberDBID;

	PG_Party_LtoC_MemberLogout()
	{	Command = EM_PG_Party_LtoC_MemberLogout;	};
};
//-------------------------------------------------------------------
// C -> Chat Party �����m�h��
struct PG_Party_CtoChat_SetMemberPosRequest
{
	GamePGCommandEnum	Command;	
	int		MemberDBID[2];
	int		Pos[2];

	PG_Party_CtoChat_SetMemberPosRequest()
	{	Command = EM_PG_Party_CtoChat_SetMemberPosRequest;	};
};
//-------------------------------------------------------------------
// Chat -> C Party �����m�h�����G
struct PG_Party_ChattoC_SetMemberPosResult
{
	GamePGCommandEnum	Command;	
	bool				Result;

	PG_Party_ChattoC_SetMemberPosResult()
	{	Command = EM_PG_Party_ChattoC_SetMemberPosResult;	};
};
//-------------------------------------------------------------------
struct PG_Party_CtoChat_SetMemberMode
{
	GamePGCommandEnum	Command;	

	int						MemberDBID;
	PartyMemberModeStruct	Mode;

	PG_Party_CtoChat_SetMemberMode()
	{	Command = EM_PG_Party_CtoChat_SetMemberMode;	};
};

//-------------------------------------------------------------------
// ZoneParty
//-------------------------------------------------------------------
//C -> L �ܽХ[�JZoneParty
struct PG_ZoneParty_CtoL_Invite
{
	GamePGCommandEnum	Command;

	int					InviteDBID;
	int					ClientInfo;	

	PG_ZoneParty_CtoL_Invite()
	{	Command = EM_PG_ZoneParty_CtoL_Invite;	};
};
//-------------------------------------------------------------------
//L -> C �q���ܽХ[�JZoneParty
struct PG_ZoneParty_LtoC_Invite
{
	GamePGCommandEnum	Command;

	int					LeaderDBID;
	int					ClientInfo;	

	PG_ZoneParty_LtoC_Invite()
	{	Command = EM_PG_ZoneParty_LtoC_Invite;	};
};
//-------------------------------------------------------------------
//C -> L �[�JZoneParty
struct PG_ZoneParty_CtoL_Join
{
	GamePGCommandEnum	Command;

	int					LeaderDBID;

	PG_ZoneParty_CtoL_Join()
	{	Command = EM_PG_ZoneParty_CtoL_Join;	};
};
//-------------------------------------------------------------------
//L -> C �s�W����
struct PG_ZoneParty_LtoC_AddMember
{
	GamePGCommandEnum	Command;

	int					PartyID;
	int					MemberDBID;
	int					PartyNo;
	int					MemberID;

	PG_ZoneParty_LtoC_AddMember()
	{	Command = EM_PG_ZoneParty_LtoC_AddMember;	};
};
//-------------------------------------------------------------------
//L -> C ��������
struct PG_ZoneParty_LtoC_DelMember
{
	GamePGCommandEnum	Command;

	int					MemberDBID;

	PG_ZoneParty_LtoC_DelMember()
	{	Command = EM_PG_ZoneParty_LtoC_DelMember;	};
};
//-------------------------------------------------------------------
//L -> C �ץ��������
struct PG_ZoneParty_LtoC_FixMember
{
	GamePGCommandEnum	Command;
	int					PartyID;
	int					MemberDBID;
	int					PartyNo;
	int					MemberID;


	PG_ZoneParty_LtoC_FixMember()
	{	Command = EM_PG_ZoneParty_LtoC_FixMember;	};
};
//-------------------------------------------------------------------
//C -> L �n�D�𰣦���
struct PG_ZoneParty_CtoL_KickMember
{
	GamePGCommandEnum	Command;
	int					MemberDBID;

	PG_ZoneParty_CtoL_KickMember()
	{	Command = EM_PG_ZoneParty_CtoL_KickMember;	};
};
//-------------------------------------------------------------------
//C -> L ��缾�a�Ҧb��m
struct PG_ZoneParty_CtoL_SetMemberPosRequest
{
	GamePGCommandEnum	Command;	
	int		MemberDBID[2];
	int		Pos[2];

	PG_ZoneParty_CtoL_SetMemberPosRequest()
	{	Command = EM_PG_ZoneParty_CtoL_SetMemberPosRequest;	};
};
//-------------------------------------------------------------------
//L -> C ��缾�a�Ҧb��m���G
struct PG_ZoneParty_LtoC_SetMemberPosResult
{
	GamePGCommandEnum	Command;	
	bool				Result;

	PG_ZoneParty_LtoC_SetMemberPosResult()
	{	Command = EM_PG_ZoneParty_LtoC_SetMemberPosResult;	};
};
//-------------------------------------------------------------------
//L -> C Party���򥻸��
struct PG_ZoneParty_LtoC_PartyBase
{
	GamePGCommandEnum	Command;
	int					PartyID;
	int					LeaderDBID;	

	PG_ZoneParty_LtoC_PartyBase()
	{	Command = EM_PG_ZoneParty_LtoC_PartyBase;	};
};
//-------------------------------------------------------------------
//C -> L �n�D������
struct PG_ZoneParty_CtoL_ChangeLeader
{
	GamePGCommandEnum	Command;
	int					NewLeaderDBID;	

	PG_ZoneParty_CtoL_ChangeLeader()
	{	Command = EM_PG_ZoneParty_CtoL_ChangeLeader;	};
};
//-------------------------------------------------------------------
//L -> C �n�D���������G
struct PG_ZoneParty_LtoC_ChangeLeaderResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_ZoneParty_LtoC_ChangeLeaderResult()
	{	Command = EM_PG_ZoneParty_LtoC_ChangeLeaderResult;	};
};
//-------------------------------------------------------------------
//C -> L ���ܫʥ]
struct PG_ZoneParty_CtoL_Talk
{
	GamePGCommandEnum	Command;

	StaticString <MAX_TALK_CONTENT>     Content;

	PG_ZoneParty_CtoL_Talk()
	{	Command = EM_PG_ZoneParty_CtoL_Talk;	};
};
//-------------------------------------------------------------------
//L -> C ���ܫʥ]
struct PG_ZoneParty_LtoC_Talk
{
	GamePGCommandEnum	Command;

	int									GItemID;
	int									ContentSize;
	StaticString <MAX_TALK_CONTENT>     Content;


	PG_ZoneParty_LtoC_Talk()
	{	Command = EM_PG_ZoneParty_LtoC_Talk;	};

	int	Size()
	{
		return sizeof(*this)-sizeof(Content)+ContentSize;
	}
};
//////////////////////////////////////////////////////////////////////////
//�d�߬Y�H�������
//Client -> Local
struct PG_Party_CtoL_GetMemberInfo
{
	GamePGCommandEnum	Command;
	
	int		LeaderDBID;

	PG_Party_CtoL_GetMemberInfo()
	{	Command = EM_PG_Party_CtoL_GetMemberInfo;	};
};
//-------------------------------------------------------------------
//�d�߬Y�H�������
//Local -> Local
struct PG_Party_LtoL_GetMemberInfo
{
	GamePGCommandEnum	Command;

	int		RequestPlayerDBID;
	int		LeaderDBID;

	PG_Party_LtoL_GetMemberInfo()
	{	Command = EM_PG_Party_LtoL_GetMemberInfo;	};
};


//-------------------------------------------------------------------
//�d��party�������G
//Local -> Local
struct PG_Party_LtoC_GetMemberInfoResult
{
	GamePGCommandEnum	Command;

	int		LeaderDBID;
	int		TotalCount;
	int		ID;

	PartyMemberInfoStruct Member;

	PG_Party_LtoC_GetMemberInfoResult()
	{	Command = EM_PG_Party_LtoC_GetMemberInfoResult;	};
};

//-------------------------------------------------------------------
#pragma managed
