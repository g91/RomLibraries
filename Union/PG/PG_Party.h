#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged
//#define MAX_TALK_CONTENT				512
//-------------------------------------------------------------------
// 要請
//-------------------------------------------------------------------
// C1 -> Chat C1 要請 C2 
// Chat -> C2 C1 要請 C2
// C2 -> Chat C2 要加入屬於 C1 的 Party
	//1 如果 C1 沒 Party自動產生
	//2 加入 C1 的 Party
// Chat -> Local 更新 Party 結構資訊
// Chat -> Local 加入Party成員
// Chat -> Local 移除Party成員

// Chat -> C 更新 Party 結構資訊
// Chat -> C 加入Party成員
// Chat -> C 移除Party成員

//-------------------------------------------------------------------
// 其他
//-------------------------------------------------------------------
// C -> Chat 踢除 隊伍成員
// C -> Chat 設定隊伍規則
// C -> Chat 設定隊長
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// 邀請
//-------------------------------------------------------------------
// C1 -> Chat C1 邀請 C2 
struct PG_Party_CtoChat_Invite
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_> InviteName;
	int					ClientInfo;	

	PG_Party_CtoChat_Invite()
	{	Command = EM_PG_Party_CtoChat_Invite;	};
};
//-------------------------------------------------------------------
// Chat -> C2 C1 邀請 C2
struct PG_Party_ChattoC_Invite
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_> LeaderName;
	int					ClientInfo;	

	PG_Party_ChattoC_Invite()
	{	Command = EM_PG_Party_ChattoC_Invite;	};
};

// 職業資訊
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
// C1 -> C2 --- C1 申請加入 C2 的隊伍
struct PG_Party_CtoC_AskJoinToParty
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> AskerName; // 誰申請的
	SJobInfo sJobInfo;

	PG_Party_CtoC_AskJoinToParty()
	{
		Command = EM_PG_Party_CtoC_AskJoinToParty;
		AskerName.Clear();
	}
};

//-------------------------------------------------------------------
// C2 -> C1 --- C2 對 C1 申請加入隊伍的回應
struct PG_Party_CtoC_AskJoinToPartyResult
{
	CliToCliPGCommandEnum Command;
	StaticString <_MAX_NAMESTRING_LEN_> LeaderName; // 誰回應的
	bool bYesNo;

	PG_Party_CtoC_AskJoinToPartyResult()
	{
		Command = EM_PG_Party_CtoC_AskJoinToPartyResult;
		bYesNo = false;
	}
};

//-------------------------------------------------------------------
// C1 -> C2 --- C1 要求 C2 刪除 C2 在組隊招募留言板上的留言
struct PG_Party_CtoC_DeletePBPost
{
	CliToCliPGCommandEnum Command;

	PG_Party_CtoC_DeletePBPost()
	{
		Command = EM_PG_Party_CtoC_DeletePBPost;
	}
};

//-------------------------------------------------------------------
// C2 -> Chat C2 要加入屬於 C1 的 Party
	//1 如果 C1 沒 Party自動產生
	//2 加入 C1 的 Party
struct PG_Party_CtoChat_Join
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_> LeaderName;

	PG_Party_CtoChat_Join()
	{	Command = EM_PG_Party_CtoChat_Join;	};
};
//-------------------------------------------------------------------
// Local -> Chat 要求某人加入到某個Party,(重新登入)
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
// Chat -> Local 更新 Party 結構資訊 ( 送給所有Server )
struct PG_Party_ChattoL_PartyBaseInfo
{
	GamePGCommandEnum	Command;
	PartyBaseInfoStruct Info;
	

	PG_Party_ChattoL_PartyBaseInfo()
	{	Command = EM_PG_Party_ChattoL_PartyBaseInfo;	};
};
//-------------------------------------------------------------------
// Chat -> Local 加入Party成員( 送給所有Server )
struct PG_Party_ChattoL_AddMember
{
	GamePGCommandEnum	Command;
	int					PartyID;
	PartyMemberInfoStruct NewMember;

	PG_Party_ChattoL_AddMember()
	{	Command = EM_PG_Party_ChattoL_AddMember;	};
};
//-------------------------------------------------------------------
// Chat -> Local 移除Party成員( 送給所有Server )
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
// Chat -> L 更新成員資訊
struct PG_Party_ChattoL_FixMember
{
	GamePGCommandEnum	Command;
	int					PartyID;
	PartyMemberInfoStruct Member;

	PG_Party_ChattoL_FixMember()
	{	Command = EM_PG_Party_ChattoL_FixMember;	};
};
//-------------------------------------------------------------------
// Chat -> C 更新 Party 結構資訊
struct PG_Party_ChattoC_PartyBaseInfo
{
	GamePGCommandEnum	Command;
	PartyBaseInfoStruct Info;


	PG_Party_ChattoC_PartyBaseInfo()
	{	Command = EM_PG_Party_ChattoC_PartyBaseInfo;	};
};
//-------------------------------------------------------------------
// Chat -> C 加入Party成員
struct PG_Party_ChattoC_AddMember
{
	GamePGCommandEnum	Command;
	PartyMemberInfoStruct NewMember;

	PG_Party_ChattoC_AddMember()
	{	Command = EM_PG_Party_ChattoC_AddMember;	};
};
//-------------------------------------------------------------------
// Chat -> C 移除Party成員
struct PG_Party_ChattoC_DelMember
{
	GamePGCommandEnum	Command;
	PartyMemberInfoStruct Member;
	//StaticString <_MAX_NAMESTRING_LEN_> DelMemberName;

	PG_Party_ChattoC_DelMember()
	{	Command = EM_PG_Party_ChattoC_DelMember;	};
};
//-------------------------------------------------------------------
// Chat -> C 更新成員資訊
struct PG_Party_ChattoC_FixMember
{
	GamePGCommandEnum	Command;
	int					PartyID;
	PartyMemberInfoStruct Member;

	PG_Party_ChattoC_FixMember()
	{	Command = EM_PG_Party_ChattoC_FixMember;	};
};
//-------------------------------------------------------------------
// 其他
//-------------------------------------------------------------------
// C -> Chat 踢除 隊伍成員
struct PG_Party_CtoChat_KickMember
{
	GamePGCommandEnum	Command;	
	StaticString <_MAX_NAMESTRING_LEN_> MemberName;
	int					PartyID;

	PG_Party_CtoChat_KickMember()
	{	Command = EM_PG_Party_CtoChat_KickMember;	};
};
//-------------------------------------------------------------------
// C -> Chat 設定隊伍規則
struct PG_Party_CtoChat_PartyRule
{
	GamePGCommandEnum	Command;	
	PartyBaseInfoStruct PartyInfo;

	PG_Party_CtoChat_PartyRule()
	{	Command = EM_PG_Party_CtoChat_PartyRule;	};
};
//-------------------------------------------------------------------
// Party成員相關資訊
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
//隊伍戰利品相關封包 ( 測試 )
//-------------------------------------------------------------------
// L -> Chat Party內獲得戰利品
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
// Chat -> C Party內獲得戰利品
struct PG_Party_ChattoC_PartyTreasure
{
	GamePGCommandEnum	Command;	
	int					ItemVersion;
	ItemFieldStruct		Item;
	StaticVector< int , _MAX_PARTY_COUNT_  > LootDBID;
	PartyItemShareENUM	LootType;	//模式

	PG_Party_ChattoC_PartyTreasure()
	{	Command = EM_PG_Party_ChattoC_PartyTreasure;	};
};
//-------------------------------------------------------------------
// Chat -> C Party內獲得戰利品
struct PG_Party_ChattoC_PartyTreasureLoot
{
	GamePGCommandEnum	Command;	
	int					ItemVersion;
	int					DBID;		//那個玩家
	int					LootValue;	//0   代表放棄

	PG_Party_ChattoC_PartyTreasureLoot()
	{	Command = EM_PG_Party_ChattoC_PartyTreasureLoot;	};
};
//-------------------------------------------------------------------
enum  PartyTreasureLootTypeENUM
{
	EM_PartyTreasureLootType_Loot	,
	EM_PartyTreasureLootType_Pass	,	

	EM_PartyTreasureLootType_Assist	,	//指定
	EM_PartyTreasureLootType_Loot_NotNeed	,	//非必要
};
// C -> Chat	設定骰子
struct PG_Party_CtoChat_PartyTreasureLoot
{
	GamePGCommandEnum	Command;	
	int					ItemVersion;
	PartyTreasureLootTypeENUM Type;
	int					DBID;			//指定那個玩家(EM_PartyTreasureLootType_Assist 才有效 )

	PG_Party_CtoChat_PartyTreasureLoot()
	{	Command = EM_PG_Party_CtoChat_PartyTreasureLoot;	};
};


//-------------------------------------------------------------------
// Chat -> L 某人得到物品
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
// Chat -> L 某人得到物品
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
//通知Party成員有人離線
struct PG_Party_LtoC_MemberLogout
{
	GamePGCommandEnum	Command;	
	int					MemberDBID;

	PG_Party_LtoC_MemberLogout()
	{	Command = EM_PG_Party_LtoC_MemberLogout;	};
};
//-------------------------------------------------------------------
// C -> Chat Party 角色位置搬移
struct PG_Party_CtoChat_SetMemberPosRequest
{
	GamePGCommandEnum	Command;	
	int		MemberDBID[2];
	int		Pos[2];

	PG_Party_CtoChat_SetMemberPosRequest()
	{	Command = EM_PG_Party_CtoChat_SetMemberPosRequest;	};
};
//-------------------------------------------------------------------
// Chat -> C Party 角色位置搬移結果
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
//C -> L 邀請加入ZoneParty
struct PG_ZoneParty_CtoL_Invite
{
	GamePGCommandEnum	Command;

	int					InviteDBID;
	int					ClientInfo;	

	PG_ZoneParty_CtoL_Invite()
	{	Command = EM_PG_ZoneParty_CtoL_Invite;	};
};
//-------------------------------------------------------------------
//L -> C 通知邀請加入ZoneParty
struct PG_ZoneParty_LtoC_Invite
{
	GamePGCommandEnum	Command;

	int					LeaderDBID;
	int					ClientInfo;	

	PG_ZoneParty_LtoC_Invite()
	{	Command = EM_PG_ZoneParty_LtoC_Invite;	};
};
//-------------------------------------------------------------------
//C -> L 加入ZoneParty
struct PG_ZoneParty_CtoL_Join
{
	GamePGCommandEnum	Command;

	int					LeaderDBID;

	PG_ZoneParty_CtoL_Join()
	{	Command = EM_PG_ZoneParty_CtoL_Join;	};
};
//-------------------------------------------------------------------
//L -> C 新增成員
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
//L -> C 移除成員
struct PG_ZoneParty_LtoC_DelMember
{
	GamePGCommandEnum	Command;

	int					MemberDBID;

	PG_ZoneParty_LtoC_DelMember()
	{	Command = EM_PG_ZoneParty_LtoC_DelMember;	};
};
//-------------------------------------------------------------------
//L -> C 修正成員資料
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
//C -> L 要求踢除成員
struct PG_ZoneParty_CtoL_KickMember
{
	GamePGCommandEnum	Command;
	int					MemberDBID;

	PG_ZoneParty_CtoL_KickMember()
	{	Command = EM_PG_ZoneParty_CtoL_KickMember;	};
};
//-------------------------------------------------------------------
//C -> L 更改玩家所在位置
struct PG_ZoneParty_CtoL_SetMemberPosRequest
{
	GamePGCommandEnum	Command;	
	int		MemberDBID[2];
	int		Pos[2];

	PG_ZoneParty_CtoL_SetMemberPosRequest()
	{	Command = EM_PG_ZoneParty_CtoL_SetMemberPosRequest;	};
};
//-------------------------------------------------------------------
//L -> C 更改玩家所在位置結果
struct PG_ZoneParty_LtoC_SetMemberPosResult
{
	GamePGCommandEnum	Command;	
	bool				Result;

	PG_ZoneParty_LtoC_SetMemberPosResult()
	{	Command = EM_PG_ZoneParty_LtoC_SetMemberPosResult;	};
};
//-------------------------------------------------------------------
//L -> C Party的基本資料
struct PG_ZoneParty_LtoC_PartyBase
{
	GamePGCommandEnum	Command;
	int					PartyID;
	int					LeaderDBID;	

	PG_ZoneParty_LtoC_PartyBase()
	{	Command = EM_PG_ZoneParty_LtoC_PartyBase;	};
};
//-------------------------------------------------------------------
//C -> L 要求換隊長
struct PG_ZoneParty_CtoL_ChangeLeader
{
	GamePGCommandEnum	Command;
	int					NewLeaderDBID;	

	PG_ZoneParty_CtoL_ChangeLeader()
	{	Command = EM_PG_ZoneParty_CtoL_ChangeLeader;	};
};
//-------------------------------------------------------------------
//L -> C 要求換隊長結果
struct PG_ZoneParty_LtoC_ChangeLeaderResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_ZoneParty_LtoC_ChangeLeaderResult()
	{	Command = EM_PG_ZoneParty_LtoC_ChangeLeaderResult;	};
};
//-------------------------------------------------------------------
//C -> L 說話封包
struct PG_ZoneParty_CtoL_Talk
{
	GamePGCommandEnum	Command;

	StaticString <MAX_TALK_CONTENT>     Content;

	PG_ZoneParty_CtoL_Talk()
	{	Command = EM_PG_ZoneParty_CtoL_Talk;	};
};
//-------------------------------------------------------------------
//L -> C 說話封包
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
//查詢某人的隊伍成員
//Client -> Local
struct PG_Party_CtoL_GetMemberInfo
{
	GamePGCommandEnum	Command;
	
	int		LeaderDBID;

	PG_Party_CtoL_GetMemberInfo()
	{	Command = EM_PG_Party_CtoL_GetMemberInfo;	};
};
//-------------------------------------------------------------------
//查詢某人的隊伍成員
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
//查詢party成員結果
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
