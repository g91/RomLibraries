#pragma once
#include "RoleData\RoleDataEx.h"
#include "NetWakerPGEnum.h"
//////////////////////////////////////////////////////////////////////////
//		建立連署公會
//////////////////////////////////////////////////////////////////////////
//要求開啟建立連署公會的介面
//Local -> Client
struct PG_Guild_LtoC_OpenCreateGuild
{
	GamePGCommandEnum	Command;
	int					TargetNPCID;

	PG_Guild_LtoC_OpenCreateGuild( )
	{	Command = EM_PG_Guild_LtoC_OpenCreateGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//Server通知關閉建立連署公會介面
//Client -> Server
struct PG_Guild_LtoC_CloseCreateGuild
{
	GamePGCommandEnum	Command;

	PG_Guild_LtoC_CloseCreateGuild( )
	{	Command = EM_PG_Guild_LtoC_CloseCreateGuild;	}
};

//////////////////////////////////////////////////////////////////////////
//通知關閉建立連署公會介面
//Client -> Server
struct PG_Guild_CtoL_CloseCreateGuild
{
	GamePGCommandEnum	Command;

	PG_Guild_CtoL_CloseCreateGuild( )
	{	Command = EM_PG_Guild_CtoL_CloseCreateGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//要求建立公會
//Client -> Local
struct PG_Guild_CtoL_CreateGuild
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>	GuildName;	//公會名稱

	PG_Guild_CtoL_CreateGuild( )
	{	Command = EM_PG_Guild_CtoL_CreateGuild;	}

};
//////////////////////////////////////////////////////////////////////////
//要求建立公會
//Local -> DataCenter
struct PG_Guild_LtoD_CreateGuild
{
	GamePGCommandEnum	Command;
	
	int					PlayerDBID;	//要求者DBID
	StaticString <_MAX_NAMESTRING_LEN_>	GuildName;	//公會名稱

	PG_Guild_LtoD_CreateGuild( )
	{	Command = EM_PG_Guild_LtoD_CreateGuild;	}

};
//////////////////////////////////////////////////////////////////////////
enum CreateGuildResultENUM
{
	EM_CreateGuildResult_OK				,	//成?
	EM_CreateGuildResult_Failed_Exist	,	//已有公會-
	EM_CreateGuildResult_Failed_Name	,	//公會名稱重復or 字串有問題
	EM_CreateGuildResult_Failed_Money	,	//金錢不足
	EM_CreateGuildResult_NameToShort	,		// 名稱太短,不合法
	EM_CreateGuildResult_NameToLong		,		// 名稱太長,不合法
	EM_CreateGuildResult_NameIsWrongful	,		// 有錯誤的符號或字元
	EM_CreateGuildResult_HasSpecialString,		// 特殊名稱不能用
	EM_CreateGuildResult_OnlyBasicLetters,		// 只能使用基本英文字母
	EM_CreateGuildResult_EndLetterError	,		// 結尾字元錯誤(',")
};

//回應建立結果
//Local -> DataCenter
struct PG_Guild_DtoL_CreateGuildResult
{
	GamePGCommandEnum	Command;

	int						PlayerDBID;	//要求者DBID
	int						GuildID;	//公會ID
	StaticString <_MAX_NAMESTRING_LEN_>		GuildName;	//公會名稱
	CreateGuildResultENUM	Result;

	PG_Guild_DtoL_CreateGuildResult( )
	{	Command = EM_PG_Guild_DtoL_CreateGuildResult;	}

};
//////////////////////////////////////////////////////////////////////////
//回應建立結果
//Local -> Client
struct PG_Guild_LtoC_CreateGuildResult
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>		GuildName;	//公會名稱
	CreateGuildResultENUM	Result;
	int						GuildID;

	PG_Guild_LtoC_CreateGuildResult( )
	{	Command = EM_PG_Guild_LtoC_CreateGuildResult;	}

};
//////////////////////////////////////////////////////////////////////////
//		邀請連署成員
//////////////////////////////////////////////////////////////////////////
//邀請連署成員
//Client -> Local
struct PG_Guild_CtoL_NewGuildInvite
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>		PlayerName;		//被邀請人名


	PG_Guild_CtoL_NewGuildInvite( )
	{	Command = EM_PG_Guild_CtoL_NewGuildInvite;	}
};
//////////////////////////////////////////////////////////////////////////
//通知被要請(連署成員)
//Local -> Client
struct PG_Guild_LtoC_NewGuildInvite
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>	LeaderName;			//邀請者人名
	int					LeaderDBID;
	int					GuildID;			//公會ID

	PG_Guild_LtoC_NewGuildInvite( )
	{	Command = EM_PG_Guild_LtoC_NewGuildInvite;	}
};
//////////////////////////////////////////////////////////////////////////
//回應邀請
//Client -> Local
struct PG_Guild_CtoL_NewGuildInviteResult
{
	GamePGCommandEnum	Command;

	int					LeaderDBID;
	int					GuildID;			//公會ID
	bool				Result;

	PG_Guild_CtoL_NewGuildInviteResult( )
	{	Command = EM_PG_Guild_CtoL_NewGuildInviteResult;	}
};
//////////////////////////////////////////////////////////////////////////
enum	NewGuildInviteResultENUM
{
	EM_NewGuildInviteResult_OK			,	//要請成?
	EM_NewGuildInviteResult_Failed		,	//要請失敗- 
	EM_NewGuildInviteResult_NotFind		,	//找不到 想邀請者
	EM_NewGuildInviteResult_HasGuild	,	//想邀請者 已經有公會
	EM_NewGuildInviteResult_Disagree	,	//想邀請者 不同意

};
//如果不同意邀請通知
//Local -> Client
struct PG_Guild_LtoC_NewGuildInviteResult
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>			PlayerName;			//被邀請者人名
	NewGuildInviteResultENUM	Result;				//結果

	PG_Guild_LtoC_NewGuildInviteResult( )
	{	Command = EM_PG_Guild_LtoC_NewGuildInviteResult;	}
};

//////////////////////////////////////////////////////////////////////////
//通知加入新成員
//Local -> DataCenter
struct PG_Guild_LtoD_AddNewGuildMember
{
	GamePGCommandEnum	Command;

	int					LeaderDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	LeaderName;			//邀請人名
	int					GuildID;			//公會ID
	int					NewMemberDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	NewMember;			//新加入者人名


	PG_Guild_LtoD_AddNewGuildMember( )
	{	Command = EM_PG_Guild_LtoD_AddNewGuildMember;	}
};
//////////////////////////////////////////////////////////////////////////
//回應加入結果
//DataCenter -> Local
struct PG_Guild_DtoL_AddNewGuildMemberResult
{
	GamePGCommandEnum	Command;

	int					LeaderDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	LeaderName;			//邀請人名
	int					GuildID;			//公會ID
	StaticString <_MAX_NAMESTRING_LEN_>	NewMember;			//新加入者人名
	bool				Result;

	PG_Guild_DtoL_AddNewGuildMemberResult( )
	{	Command = EM_PG_Guild_DtoL_AddNewGuildMemberResult;	}
};
//////////////////////////////////////////////////////////////////////////
//通知新加入連署者  加入成?
//Local -> Client
struct PG_Guild_LtoC_JoindNewGuild
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>	LeaderName;			//邀請人名
	int					GuildID;			//公會ID

	PG_Guild_LtoC_JoindNewGuild( )
	{	Command = EM_PG_Guild_LtoC_JoindNewGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//要求離開連署公會
//Client -> Local
struct PG_Guild_CtoL_LeaveNewGuild
{
	GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_>	PlayerName;		

	PG_Guild_CtoL_LeaveNewGuild( )
	{	Command = EM_PG_Guild_CtoL_LeaveNewGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//要求離開連署公會
//Local -> DataCenter
struct PG_Guild_LtoD_LeaveNewGuild
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	PlayerName;		


	PG_Guild_LtoD_LeaveNewGuild( )
	{	Command = EM_PG_Guild_LtoD_LeaveNewGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//回應離開連署公會結果
//DataCenter -> Local 
struct PG_Guild_DtoL_LeaveNewGuildResult
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	PlayerName;		

	bool				Result;

	PG_Guild_DtoL_LeaveNewGuildResult( )
	{	Command = EM_PG_Guild_DtoL_LeaveNewGuildResult;	}
};
//////////////////////////////////////////////////////////////////////////
//回應離開連署公會結果
//Local -> Client
struct PG_Guild_LtoC_LeaveNewGuildResult
{
	GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_>	KickPlayerName;		
	bool				Result;

	PG_Guild_LtoC_LeaveNewGuildResult( )
	{	Command = EM_PG_Guild_LtoC_LeaveNewGuildResult;	}
};
//////////////////////////////////////////////////////////////////////////
//要求解散連署公會
//Client -> Local
struct PG_Guild_CtoL_DelNewGuild
{
	GamePGCommandEnum	Command;

	PG_Guild_CtoL_DelNewGuild( )
	{	Command = EM_PG_Guild_CtoL_DelNewGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//要求解散連署公會
//Local -> DataCenter
struct PG_Guild_LtoD_DelNewGuild
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;

	PG_Guild_LtoD_DelNewGuild( )
	{	Command = EM_PG_Guild_LtoD_DelNewGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//回應解散連署結果
//DataCenter -> Local 
struct PG_Guild_DtoL_DelNewGuildResult
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;
	bool				Result;

	PG_Guild_DtoL_DelNewGuildResult( )
	{	Command = EM_PG_Guild_DtoL_DelNewGuildResult;	}
};
//////////////////////////////////////////////////////////////////////////
//回應解散連署結果
//Local -> Client
struct PG_Guild_LtoC_DelNewGuildResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Guild_LtoC_DelNewGuildResult( )
	{	Command = EM_PG_Guild_LtoC_DelNewGuildResult;	}
};


//////////////////////////////////////////////////////////////////////////
//	公會資料 通知 Local建立
//////////////////////////////////////////////////////////////////////////
//增加公會
// DataCenter -> Local
struct PG_Guild_DtoL_AddGuild
{
	GamePGCommandEnum	Command;

	GuildBaseStruct	Guild;

	PG_Guild_DtoL_AddGuild( )
	{	Command = EM_PG_Guild_DtoL_AddGuild;	}
};
//////////////////////////////////////////////////////////////////////////
// 移除公會
// DataCenter -> Local
struct PG_Guild_DtoL_DelGuild
{
	GamePGCommandEnum	Command;

	int		GuildID;

	PG_Guild_DtoL_DelGuild( )
	{	Command = EM_PG_Guild_DtoL_DelGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//增加公會成原
// DataCenter -> Local
struct PG_Guild_DtoL_AddGuildMember	
{
	GamePGCommandEnum	Command;

	GuildMemberStruct	Member;

	PG_Guild_DtoL_AddGuildMember( )
	{	Command = EM_PG_Guild_DtoL_AddGuildMember;	}
};
//////////////////////////////////////////////////////////////////////////
// 移除公會成員
// DataCenter -> Local
struct PG_Guild_DtoL_DelGuildMember
{
	GamePGCommandEnum	Command;

	int					GuildID;
	StaticString <_MAX_NAMESTRING_LEN_>	MemberName;			//移除者名稱

	PG_Guild_DtoL_DelGuildMember( )
	{	Command = EM_PG_Guild_DtoL_DelGuildMember;	}
};
//////////////////////////////////////////////////////////////////////////
//修改公會資料
struct PG_Guild_DtoL_ModifyGuild
{
	GamePGCommandEnum	Command;

	GuildBaseStruct	Guild;

	PG_Guild_DtoL_ModifyGuild( )
	{	Command = EM_PG_Guild_DtoL_ModifyGuild;	}
};

//修改常變的公會資料
//Datacenter -> Local
//DataCenter -> Client
struct PG_Guild_DtoX_ModifySimpleGuild
{
	GamePGCommandEnum	Command;

	SimpleGuildInfoStruct	SimpleGuildInfo;

	PG_Guild_DtoX_ModifySimpleGuild( )
	{	Command = EM_PG_Guild_DtoX_ModifySimpleGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//修改公會成員資料
struct PG_Guild_DtoL_ModifyGuildMember
{
	GamePGCommandEnum	Command;

	GuildMemberStruct	Member;

	PG_Guild_DtoL_ModifyGuildMember( )
	{	Command = EM_PG_Guild_DtoL_ModifyGuildMember;	}
};

//////////////////////////////////////////////////////////////////////////
//連署通過
//////////////////////////////////////////////////////////////////////////
//通知所有公會成員連暑成立
//DataCenter -> Client
struct PG_Guild_DtoC_GuildCreate
{
	GamePGCommandEnum	Command;
	int		GuildID;
	PG_Guild_DtoC_GuildCreate( )
	{	Command = EM_PG_Guild_DtoC_GuildCreate;	}
};
//////////////////////////////////////////////////////////////////////////
//要求公會基本資料
//Client -> Local
struct PG_Guild_CtoL_GuildInfoRequet
{
	GamePGCommandEnum	Command;
	int					GuildID;
	PG_Guild_CtoL_GuildInfoRequet( )
	{	Command = EM_PG_Guild_CtoL_GuildInfoRequet;	}
};
//////////////////////////////////////////////////////////////////////////
//通知公會基本資料
//Local -> Client
struct PG_Guild_LtoC_GuildInfo
{
	GamePGCommandEnum	Command;

	GuildBaseStruct		Guild;
	int					MemberCount;

	PG_Guild_LtoC_GuildInfo( )
	{	Command = EM_PG_Guild_LtoC_GuildInfo;	}
};
//////////////////////////////////////////////////////////////////////////
//要求公會成員列表資料
//Client -> Local
struct PG_Guild_CtoL_GuildMemberListRequet
{
	GamePGCommandEnum	Command;
	int					GuildID;
	PG_Guild_CtoL_GuildMemberListRequet( )
	{	Command = EM_PG_Guild_CtoL_GuildMemberListRequet;	}
};
//////////////////////////////////////////////////////////////////////////
//通知公會成員列表資料
//Local -> Client
struct PG_Guild_LtoC_GuildMemberListCount
{
	GamePGCommandEnum	Command;
	int		MemberCount;
	int		GuildID;

	PG_Guild_LtoC_GuildMemberListCount( )
	{	Command = EM_PG_Guild_LtoC_GuildMemberListCount;	}
};

struct GuildMemberBaseInfoStruct
{
	StaticString< _MAX_NAMERECORD_SIZE_ >   MemberName;
	int						MemberDBID;
	GuildRankENUM			Rank;				//角色公會等級
	Voc_ENUM				Voc;
	Voc_ENUM				Voc_Sub;
	Sex_ENUM				Sex;
	int						LV;
	int						LV_Sub;
	int						GroupID;
	bool					IsOnline;
	int						GUID;				//區域ID

};

//公會成員 列表須要資料
//Local -> Client
struct PG_Guild_LtoC_GuildMemberBaseInfo
{
	GamePGCommandEnum	Command;

	GuildMemberBaseInfoStruct Info;

	PG_Guild_LtoC_GuildMemberBaseInfo( )
	{	Command = EM_PG_Guild_LtoC_GuildMemberBaseInfo;	}
};

//////////////////////////////////////////////////////////////////////////
//要求公會成員基本資料
//Client -> Local
struct PG_Guild_CtoL_GuildMemberInfoRequet
{
	GamePGCommandEnum	Command;
	int					MemberDBID;
	PG_Guild_CtoL_GuildMemberInfoRequet( )
	{	Command = EM_PG_Guild_CtoL_GuildMemberInfoRequet;	}
};
//////////////////////////////////////////////////////////////////////////
//通知公會基本資料
//Local -> Client
struct PG_Guild_LtoC_GuildMemberInfo
{
	GamePGCommandEnum	Command;

	GuildMemberStruct	Member;

	PG_Guild_LtoC_GuildMemberInfo( )
	{	Command = EM_PG_Guild_LtoC_GuildMemberInfo;	}
};
//////////////////////////////////////////////////////////////////////////
//修改公會資料
//Client -> Local
struct PG_Guild_CtoL_ModifyGuildInfoRequest
{
	GamePGCommandEnum	Command;
	GuildBaseStruct		Guild;
	PG_Guild_CtoL_ModifyGuildInfoRequest( )
	{	Command = EM_PG_Guild_CtoL_ModifyGuildInfoRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//修改公會資料
//Local -> DataCenter
struct PG_Guild_LtoD_ModifyGuildInfoRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;			//要求修改者
	GuildBaseStruct		Guild;
	PG_Guild_LtoD_ModifyGuildInfoRequest( )
	{	Command = EM_PG_Guild_LtoD_ModifyGuildInfoRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//要求修改公會成員
//Client -> Local
struct PG_Guild_CtoL_ModifyGuildMemberInfoRequest
{
	GamePGCommandEnum	Command;
	GuildMemberStruct	Member;

	PG_Guild_CtoL_ModifyGuildMemberInfoRequest( )
	{	Command = EM_PG_Guild_CtoL_ModifyGuildMemberInfoRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//要求修改公會成員
//Local -> DataCenter
struct PG_Guild_LtoD_ModifyGuildMemberInfoRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;			//要求修改者
	GuildMemberStruct	Member;
	PG_Guild_LtoD_ModifyGuildMemberInfoRequest( )
	{	Command = EM_PG_Guild_LtoD_ModifyGuildMemberInfoRequest;	}
};

//////////////////////////////////////////////////////////////////////////
// 正式公會要請加入
//////////////////////////////////////////////////////////////////////////
//邀請加入公會
//Client -> Local
struct PG_Guild_CtoL_GuildInvite
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>	PlayerName;		//被邀請人名


	PG_Guild_CtoL_GuildInvite( )
	{	Command = EM_PG_Guild_CtoL_GuildInvite;	}
};
//////////////////////////////////////////////////////////////////////////
//通知被要請加入公會
//Local -> Client
struct PG_Guild_LtoC_GuildInvite
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>	LeaderName;			//邀請者人名
	int					LeaderDBID;
	int					GuildID;			//公會ID

	PG_Guild_LtoC_GuildInvite( )
	{	Command = EM_PG_Guild_LtoC_GuildInvite;	}
};
//////////////////////////////////////////////////////////////////////////
//回應邀請加入公會
//Client -> Local
struct PG_Guild_CtoL_GuildInviteResult
{
	GamePGCommandEnum	Command;

	int					LeaderDBID;
	int					GuildID;			//公會ID
	bool				Result;

	PG_Guild_CtoL_GuildInviteResult( )
	{	Command = EM_PG_Guild_CtoL_GuildInviteResult;	}
};
//////////////////////////////////////////////////////////////////////////
enum	GuildInviteResultENUM
{
	EM_GuildInviteResult_OK			,	//要請成?
	EM_GuildInviteResult_Failed		,	//要請失敗-
	EM_GuildInviteResult_NotFind	,	//找不到 想邀請者
	EM_GuildInviteResult_HasGuild	,	//想邀請者 已經有公會
	EM_GuildInviteResult_Disagree	,	//想邀請者 不同意

};
//如果不同意加入公會邀請通知
//Local -> Client
struct PG_Guild_LtoC_GuildInviteResult
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>		PlayerName;			//被邀請者人名
	GuildInviteResultENUM	Result;				//結果

	PG_Guild_LtoC_GuildInviteResult( )
	{	Command = EM_PG_Guild_LtoC_GuildInviteResult;	}
};

//////////////////////////////////////////////////////////////////////////
//通知加入新成員
//Local -> DataCenter
struct PG_Guild_LtoD_AddGuildMember
{
	GamePGCommandEnum	Command;

	int					MemberDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	MemberName;			//邀請人名
	int					GuildID;			//公會ID
	int					NewMemberDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	NewMember;			//新加入者人名


	PG_Guild_LtoD_AddGuildMember( )
	{	Command = EM_PG_Guild_LtoD_AddGuildMember;	}
};
//////////////////////////////////////////////////////////////////////////
//回應加入結果
//DataCenter -> Local
struct PG_Guild_DtoL_AddGuildMemberResult
{
	GamePGCommandEnum	Command;

	int					LeaderDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	LeaderName;			//邀請人名
	int					GuildID;			//公會ID
	StaticString <_MAX_NAMESTRING_LEN_>	NewMember;			//新加入者人名
	bool				Result;

	PG_Guild_DtoL_AddGuildMemberResult( )
	{	Command = EM_PG_Guild_DtoL_AddGuildMemberResult;	}
};
//////////////////////////////////////////////////////////////////////////
//通知新加入公會者  加入成
//Local -> Client
struct PG_Guild_LtoC_JoindGuild
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>	LeaderName;			//邀請人名
	int					GuildID;			//公會ID

	PG_Guild_LtoC_JoindGuild( )
	{	Command = EM_PG_Guild_LtoC_JoindGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//要求離開公會
//Client -> Local
struct PG_Guild_CtoL_LeaveGuild
{
	GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_>	PlayerName;		

	PG_Guild_CtoL_LeaveGuild( )
	{	Command = EM_PG_Guild_CtoL_LeaveGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//要求離開公會
//Local -> DataCenter
struct PG_Guild_LtoD_LeaveGuild
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	PlayerName;		


	PG_Guild_LtoD_LeaveGuild( )
	{	Command = EM_PG_Guild_LtoD_LeaveGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//回應離開公會結果
//DataCenter -> Local 
struct PG_Guild_DtoL_LeaveGuildResult
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	PlayerName;		

	bool				Result;

	PG_Guild_DtoL_LeaveGuildResult( )
	{	Command = EM_PG_Guild_DtoL_LeaveGuildResult;	}
};
//////////////////////////////////////////////////////////////////////////
//回應離開公會結果
//Local -> Client
struct PG_Guild_LtoC_LeaveGuildResult
{
	GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_>	KickPlayerName;		
	bool				Result;

	PG_Guild_LtoC_LeaveGuildResult( )
	{	Command = EM_PG_Guild_LtoC_LeaveGuildResult;	}
};
//////////////////////////////////////////////////////////////////////////
//要求解散公會
//Client -> Local
struct PG_Guild_CtoL_DelGuild
{
	GamePGCommandEnum	Command;

	PG_Guild_CtoL_DelGuild( )
	{	Command = EM_PG_Guild_CtoL_DelGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//要求解散公會
//Local -> DataCenter
struct PG_Guild_LtoD_DelGuild
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;

	PG_Guild_LtoD_DelGuild( )
	{	Command = EM_PG_Guild_LtoD_DelGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//回應解散公會結果
//DataCenter -> Local 
struct PG_Guild_DtoL_DelGuildResult
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;
	bool				Result;

	PG_Guild_DtoL_DelGuildResult( )
	{	Command = EM_PG_Guild_DtoL_DelGuildResult;	}
};
//////////////////////////////////////////////////////////////////////////
//回應解散公會結果
//Local -> Client
struct PG_Guild_LtoC_DelGuildResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Guild_LtoC_DelGuildResult( )
	{	Command = EM_PG_Guild_LtoC_DelGuildResult;	}
};
//////////////////////////////////////////////////////////////////////////
//要求把連署公會轉為正式公會
//Local -> DataCenter
struct PG_Guild_LtoD_SetGuildReady
{
	GamePGCommandEnum	Command;
	int					GuildID;
	PG_Guild_LtoD_SetGuildReady( )
	{	Command = EM_PG_Guild_LtoD_SetGuildReady;	}
};
//////////////////////////////////////////////////////////////////////////
//通知Client 處理結果
//Local -> DataCenter
struct PG_Guild_LtoC_SetGuildReady
{
	GamePGCommandEnum	Command;
	bool				Result;
	PG_Guild_LtoC_SetGuildReady( )
	{	Command = EM_PG_Guild_LtoC_SetGuildReady;	}
};

//////////////////////////////////////////////////////////////////////////
//	公會留言板
//////////////////////////////////////////////////////////////////////////
//要求留言
//Client -> Local
struct PG_Guild_CtoL_BoardPostRequest
{
	GamePGCommandEnum	Command;
	GuildBoardStruct	Message;

	PG_Guild_CtoL_BoardPostRequest( )
	{	Command = EM_PG_Guild_CtoL_BoardPostRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//要求留言
//Local -> DataCenter
struct PG_Guild_LtoD_BoardPostRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	GuildBoardStruct	Message;

	PG_Guild_LtoD_BoardPostRequest( )
	{	Command = EM_PG_Guild_LtoD_BoardPostRequest;	}
};
//////////////////////////////////////////////////////////////////////////

//DataCenter -> Client
struct PG_Guild_DtoC_BoardPostResult
{
	GamePGCommandEnum	Command;
	bool	Result;

	PG_Guild_DtoC_BoardPostResult( )
	{	Command = EM_PG_Guild_DtoC_BoardPostResult;	}
};
//////////////////////////////////////////////////////////////////////////
//要求留言列表
//Client -> Local
struct PG_Guild_CtoL_BoardListRequest
{
	GamePGCommandEnum	Command;
	int		PageID;					//那一頁

	PG_Guild_CtoL_BoardListRequest( )
	{	Command = EM_PG_Guild_CtoL_BoardListRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//要求留言列表
//Local -> DataCenter
struct PG_Guild_LtoD_BoardListRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					PageID;

	PG_Guild_LtoD_BoardListRequest( )
	{	Command = EM_PG_Guild_LtoD_BoardListRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//回應留言列表 數量
//DataCenter -> Client
struct PG_Guild_DtoC_BoardListCount
{
	GamePGCommandEnum	Command;
	int					MaxCount;		//最大筆數
	int					Count;			//送出幾比 (0表示找不到)
	int					PageID;

	PG_Guild_DtoC_BoardListCount( )
	{	Command = EM_PG_Guild_DtoC_BoardListCount;	}
};

//回應留言列表 單筆資料
struct PG_Guild_DtoC_BoardListData
{
	GamePGCommandEnum		Command;
	int						Index;		//第幾筆資料
	int						GUID;		//序號
	StaticString< _MAX_NAMERECORD_SIZE_ >   PlayerName;	    //發表人
	GuildBoardModeStruct	Type;		//狀態
	int						Time;		//發表時間
	StaticString< 128 >		Title;		//標題


	PG_Guild_DtoC_BoardListData( )
	{	Command = EM_PG_Guild_DtoC_BoardListData;	}
};
//////////////////////////////////////////////////////////////////////////
//要求留言內容
//Client -> Local
struct PG_Guild_CtoL_BoardMessageRequest
{
	GamePGCommandEnum	Command;
	int					GUID;		//訊息的編號

	PG_Guild_CtoL_BoardMessageRequest( )
	{	Command = EM_PG_Guild_CtoL_BoardMessageRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//要求留言內容
//Local -> DataCenter
struct PG_Guild_LtoD_BoardMessageRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					GUID;


	PG_Guild_LtoD_BoardMessageRequest( )
	{	Command = EM_PG_Guild_LtoD_BoardMessageRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//留言內容
//DataCenter -> Client
struct PG_Guild_DtoC_BoardMessageResult
{
	GamePGCommandEnum	Command;
	GuildBoardStruct	Message;

	PG_Guild_DtoC_BoardMessageResult( )
	{	Command = EM_PG_Guild_DtoC_BoardMessageResult;	}
};
//////////////////////////////////////////////////////////////////////////
//要求修改留言
//Client -> Local
struct PG_Guild_CtoL_BoardModifyMessageRequest
{
	GamePGCommandEnum		Command;
	int						MessageGUID;
	StaticString< 512 >		Message;		//新的留言

	PG_Guild_CtoL_BoardModifyMessageRequest( )
	{	Command = EM_PG_Guild_CtoL_BoardModifyMessageRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//要求修改留言
//Local -> DataCenter
struct PG_Guild_LtoD_BoardModifyMessageRequest
{
	GamePGCommandEnum		Command;
	int						PlayerDBID;
	int						MessageGUID;
	StaticString< 512 >		Message;		//新的留言

	PG_Guild_LtoD_BoardModifyMessageRequest( )
	{	Command = EM_PG_Guild_LtoD_BoardModifyMessageRequest;	}
};
//////////////////////////////////////////////////////////////////////////

//DataCenter -> Client
struct PG_Guild_DtoC_BoardModifyMessageResult
{
	GamePGCommandEnum	Command;
	bool	Result;

	PG_Guild_DtoC_BoardModifyMessageResult( )
	{	Command = EM_PG_Guild_DtoC_BoardModifyMessageResult;	}
};
//////////////////////////////////////////////////////////////////////////
//設定留言屬性
//Client -> Local
struct PG_Guild_CtoL_BoardModifyModeRequest
{
	GamePGCommandEnum		Command;
	int						MessageGUID;
	GuildBoardModeStruct	Mode;

	PG_Guild_CtoL_BoardModifyModeRequest( )
	{	Command = EM_PG_Guild_CtoL_BoardModifyModeRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//要求修改留言
//Local -> DataCenter
struct PG_Guild_LtoD_BoardModifyModeRequest
{
	GamePGCommandEnum		Command;
	int						PlayerDBID;
	int						MessageGUID;
	GuildBoardModeStruct	Mode;

	PG_Guild_LtoD_BoardModifyModeRequest( )
	{	Command = EM_PG_Guild_LtoD_BoardModifyModeRequest;	}
};
//////////////////////////////////////////////////////////////////////////

//DataCenter -> Client
struct PG_Guild_DtoC_BoardModifyModeResult
{
	GamePGCommandEnum	Command;
	bool	Result;

	PG_Guild_DtoC_BoardModifyModeResult( )
	{	Command = EM_PG_Guild_DtoC_BoardModifyModeResult;	}
};
//////////////////////////////////////////////////////////////////////////
//公會名稱與id資料
//Local -> Client
struct PG_Guild_LtoC_GuildBaseInfo
{
	GamePGCommandEnum	Command;
	
	int			GuildID;
	StaticString <_MAX_NAMESTRING_LEN_>	GuildName;	//公會名稱
	GuildFlagStruct		Flag;
	bool		IsReady;							//是否正式成立

	PG_Guild_LtoC_GuildBaseInfo( )
	{	Command = EM_PG_Guild_LtoC_GuildBaseInfo;	}

};
//////////////////////////////////////////////////////////////////////////
//公會刪除
//Local -> Client
struct PG_Guild_LtoC_GuildDestroy
{
	GamePGCommandEnum	Command;

	int			GuildID;

	PG_Guild_LtoC_GuildDestroy( )
	{	Command = EM_PG_Guild_LtoC_GuildDestroy;	}

};
//////////////////////////////////////////////////////////////////////////
//自己公會有加入新成員
//DataCenter -> Client
struct PG_Guild_DtoC_JoinGuildMember
{
	GamePGCommandEnum	Command;
	GuildMemberStruct	Member;

	PG_Guild_DtoC_JoinGuildMember( )
	{	Command = EM_PG_Guild_DtoC_JoinGuildMember;	}

};
//////////////////////////////////////////////////////////////////////////
//自己公會有人離開
//DataCenter -> Client
struct PG_Guild_DtoC_LeaveGuildMember
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;
	int					MemberDBID;

	PG_Guild_DtoC_LeaveGuildMember( )
	{	Command = EM_PG_Guild_DtoC_LeaveGuildMember;	}

};
//////////////////////////////////////////////////////////////////////////
//公會貢獻
//////////////////////////////////////////////////////////////////////////
//開啟公會貢獻選單
//Local -> Client
struct PG_Guild_LtoC_OpenGuildContribution
{
	GamePGCommandEnum	Command;
	int					TargetNPCID;

	PG_Guild_LtoC_OpenGuildContribution( )
	{	Command = EM_PG_Guild_LtoC_OpenGuildContribution;	}
};
//////////////////////////////////////////////////////////////////////////
//設定要放置的物品
//Client -> Server
struct PG_Guild_CtoL_GuildContributionItem
{
	GamePGCommandEnum	Command;
	
	int					Pos;			//交易欄位
	ItemFieldStruct		Item;
	bool				IsPut;			//true 放  false 取回

	PG_Guild_CtoL_GuildContributionItem()
	{
		Command = EM_PG_Guild_CtoL_GuildContributionItem;
	}
};
//////////////////////////////////////////////////////////////////////////
//設定放置貢獻物品的結果
//Server -> Client
struct PG_Guild_LtoC_GuildContributionItemResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Guild_LtoC_GuildContributionItemResult()
	{
		Command = EM_PG_Guild_LtoC_GuildContributionItemResult;
	}
};

//////////////////////////////////////////////////////////////////////////
//確定貢獻
//Client -> Server
struct PG_Guild_CtoL_GuildContributionOK
{
	GamePGCommandEnum	Command;
	int					Money;	//要共獻的錢
	int					Bonus_Money;

	PG_Guild_CtoL_GuildContributionOK( )
	{	Command = EM_PG_Guild_CtoL_GuildContributionOK;	}
};
//////////////////////////////////////////////////////////////////////////
//Server通知 關閉貢獻物品的介面
//Server -> Client
struct PG_Guild_LtoC_GuildContributionClose
{
	GamePGCommandEnum	Command;

	PG_Guild_LtoC_GuildContributionClose()
	{
		Command = EM_PG_Guild_LtoC_GuildContributionClose;
	}
};
//////////////////////////////////////////////////////////////////////////
//關閉貢獻物品的介面
//Client -> Server
struct PG_Guild_CtoL_GuildContributionClose
{
	GamePGCommandEnum	Command;

	PG_Guild_CtoL_GuildContributionClose()
	{
		Command = EM_PG_Guild_CtoL_GuildContributionClose;
	}
};
//////////////////////////////////////////////////////////////////////////
/*
 
//Local -> Datacenter
struct PG_Guild_LtoD_BuyFunction
{
	GamePGCommandEnum	Command;
	int					GuildID;
	int					PlayerDBID;
	int					Score;
	int					Type;			 
	int					Value;

	PG_Guild_LtoD_BuyFunction( )
	{	Command = EM_PG_Guild_LtoD_BuyFunction;	}
};
*/

//////////////////////////////////////////////////////////////////////////
//公會商店
//////////////////////////////////////////////////////////////////////////
/*
//Local -> Client 
//開啟公會商店
struct PG_Guild_LtoC_GuildShopOpen
{
	GamePGCommandEnum	Command;
	int				TargetNPCID;
	int				Score;
	StaticFlag<256>	Flag;

	PG_Guild_LtoC_GuildShopOpen( )
	{	Command = EM_PG_Guild_LtoC_GuildShopOpen;	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 
 
struct PG_Guild_CtoL_GuildShopBuy
{
	GamePGCommandEnum	Command;
	int					FlagID;

	PG_Guild_CtoL_GuildShopBuy( )
	{	Command = EM_PG_Guild_CtoL_GuildShopBuy;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DC 
 
struct PG_Guild_LtoD_GuildShopBuy
{
	GamePGCommandEnum	Command;

	int				PlayerDBID; 
	int				FlagID;

	PG_Guild_LtoD_GuildShopBuy( )
	{	Command = EM_PG_Guild_LtoD_GuildShopBuy;	}
};
//////////////////////////////////////////////////////////////////////////
//DC -> Client 
 
struct PG_Guild_DtoC_GuildShopBuyResult
{
	GamePGCommandEnum	Command;
	int					FlagID;
	bool				Result;

	PG_Guild_DtoC_GuildShopBuyResult( )
	{	Command = EM_PG_Guild_DtoC_GuildShopBuyResult;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client 
//關閉介面
struct PG_Guild_LtoC_GuildShopClose
{
	GamePGCommandEnum	Command;

	PG_Guild_LtoC_GuildShopClose( )
	{	Command = EM_PG_Guild_LtoC_GuildShopClose;	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 
//關閉介面		
struct PG_Guild_CtoL_GuildShopClose
{
	GamePGCommandEnum	Command;

	PG_Guild_CtoL_GuildShopClose( )
	{	Command = EM_PG_Guild_CtoL_GuildShopClose;	}
};
*/
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//要求所有公會人數資料
struct PG_Guild_CtoL_AllGuildMemberCountRequest
{
	GamePGCommandEnum	Command;

	PG_Guild_CtoL_AllGuildMemberCountRequest( )
	{	Command = EM_PG_Guild_CtoL_AllGuildMemberCountRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client
//通知每個公會的人數資料

#define DEF_MAX_PG_GUILD_MEMBER_COUNT_ 1000
struct GuildMemberCountStruct
{
	int GuildID;
	int Count;
};
struct PG_Guild_LtoC_AllGuildMemberCountResult
{
	GamePGCommandEnum	Command;
	int	Count;
	GuildMemberCountStruct MemberList[ DEF_MAX_PG_GUILD_MEMBER_COUNT_ ];

	PG_Guild_LtoC_AllGuildMemberCountResult( )
	{	Command = EM_PG_Guild_LtoC_AllGuildMemberCountResult;	}

	int Size()
	{
		return sizeof( *this ) - sizeof( int ) * (DEF_MAX_PG_GUILD_MEMBER_COUNT_ - Count );
	}
};

//////////////////////////////////////////////////////////////////////////
//公會戰
//////////////////////////////////////////////////////////////////////////
//Local -> Client 
//開啟競標介面
struct PG_Guild_LtoC_GuildFightAuctionOpen
{
	GamePGCommandEnum	Command;
	int					TargetNPCID;
	int					CrystalID;			//水晶號碼
	int 				OwnerGuildID;		//目前擁有的公會
	int 				BidGuildID;			//目前得標的公會
	int 				BidScore;			//目前競標的金額
	PG_Guild_LtoC_GuildFightAuctionOpen( )
	{	Command = EM_PG_Guild_LtoC_GuildFightAuctionOpen;	}
};
//////////////////////////////////////////////////////////////////////////
//Client - > Local
//關閉競標介面
struct PG_Guild_CtoL_GuildFightAuctionClose
{
	GamePGCommandEnum	Command;

	PG_Guild_CtoL_GuildFightAuctionClose( )
	{	Command = EM_PG_Guild_CtoL_GuildFightAuctionClose;	}
};
//////////////////////////////////////////////////////////////////////////
//Local - > Client
//關閉競標介面
struct PG_Guild_LtoC_GuildFightAuctionClose
{
	GamePGCommandEnum	Command;

	PG_Guild_LtoC_GuildFightAuctionClose( )
	{	Command = EM_PG_Guild_LtoC_GuildFightAuctionClose;	}
};
//////////////////////////////////////////////////////////////////////////
//Client - > Local
//要競標
struct PG_Guild_CtoL_GuildFightAuctionBid
{
	GamePGCommandEnum	Command;
	//int					iNPCID;
	int					CrystalID;
	int					BidScore;

	PG_Guild_CtoL_GuildFightAuctionBid( )
	{	Command = EM_PG_Guild_CtoL_GuildFightAuctionBid;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter 
//要競標
struct PG_Guild_LtoD_GuildFightAuctionBid
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;
	int					CrystalID;
	int					BidScore;

	PG_Guild_LtoD_GuildFightAuctionBid( )
	{	Command = EM_PG_Guild_LtoD_GuildFightAuctionBid;	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Client 
//競標結果
enum EM_GuildFightAuctionBidResult
{
	EM_GuildFightAuctionBidResult_Success		= 0,	 
	EM_GuildFightAuctionBidResult_Toolow		,		// 出價比目前價格還低
	EM_GuildFightAuctionBidResult_Timeout		,		// 已經超過可以出價的時間
	EM_GuildFightAuctionBidResult_LowGuildPoint	,		// 公會積分不夠標水晶
	EM_GuildFightAuctionBidResult_Bidding		,		// 要求者參與了其它水晶的競標
	EM_GuildFightAuctionBidResult_Exist			,		// 要求者擁有其它水晶的權力
};


struct PG_Guild_DtoC_GuildFightAuctionBidResult
{
	GamePGCommandEnum	Command;
	int					CrystalID;			//水晶號碼
	int 				OwnerGuildID;		//目前擁有的公會
	int 				BidGuildID;			//目前得標的公會
	int					iResult;
	int					iHighestAmount;		//目前最高金額

	PG_Guild_DtoC_GuildFightAuctionBidResult( )
	{	Command = EM_PG_Guild_DtoC_GuildFightAuctionBidResult;	}
};

//////////////////////////////////////////////////////////////////////////
//DataCenter -> Client 
//通知有人競標
struct PG_Guild_DtoC_GuildFightAuctionBidNewOne
{
	GamePGCommandEnum	Command;
	int					CrystalID;			//水晶號碼
	int 				OwnerGuildID;		//目前擁有的公會
	int 				BidGuildID;			//目前得標的公會
	int					iHighestAmount;		//目前最高金額
	int					iRemainTime;		//剩於的標價時間

	PG_Guild_DtoC_GuildFightAuctionBidNewOne( )
	{	Command = EM_PG_Guild_DtoC_GuildFightAuctionBidNewOne;	}
};

//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local
//通知所有區域競標修改的資訊
struct PG_Guild_DtoL_GuildFightAuctionInfo
{
	GamePGCommandEnum	Command;
	int					CrystalID;			//水晶號碼
	int 				OwnerGuildID;		//目前擁有的公會
	int 				BitGuildID;			//目前得標的公會

	PG_Guild_DtoL_GuildFightAuctionInfo( )
	{	Command = EM_PG_Guild_DtoL_GuildFightAuctionInfo;	}
};
//////////////////////////////////////////////////////////////////////////

struct PG_Guild_LtoD_GuildFightInfoRequest
{
	GamePGCommandEnum	Command;
	int					CrystalID;			//水晶號碼
	int 				RequestPlayerGUID;	//要求的玩家號碼

	PG_Guild_LtoD_GuildFightInfoRequest( )
	{	Command = EM_PG_Guild_LtoD_GuildFightInfoRequest;	}
};

enum EM_GuildFightInfo_Result
{
	EM_GuildFightInfo_Result_Okay		= 0,
	EM_GuildFightInfo_Result_Bidding	= 1,	// 要求者參與了其它水晶的競標
	EM_GuildFightInfo_Result_Exist		= 2,	// 要求者擁有其它水晶的權力
	EM_GuildFightInfo_Result_Timeout	= 3,	// 過了競標時間
};


struct PG_Guild_DtoC_GuildFightInfo
{
	GamePGCommandEnum			Command;
	EM_GuildFightInfo_Result	emResult;			//  查詢結果 0 正常, 1 查詢者本身已經下標其它水晶了, 2 查詢者本身已經擁有其它水晶了
		
	int							iCrystalID;			//水晶號碼
	//int					iHighestGUILDID;
	int							iHighestAmount;
	int							iDefenderGuildID;
	int							iAttackerGuildID;

	int							iOtherCrystalID;	// 要求者參與了其它水晶的競標, 要求者擁有其它水晶的權力 其水晶代號

	int							iGuildPoint;		// 公會績分
	int							iRemainTime;		//剩餘的時間
	int							iFightTime;

	PG_Guild_DtoC_GuildFightInfo( )
	{	Command = EM_PG_Guild_DtoC_GuildFightInfo;	}
};


//////////////////////////////////////////////////////////////////////////
//DataCenter -> Client 
//通知有人上下線
struct PG_Guild_DtoL_GuildPlayerOnline
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	bool				IsOnline;

	PG_Guild_DtoL_GuildPlayerOnline( )
	{	Command = EM_PG_Guild_DtoL_GuildPlayerOnline;	}
};
//////////////////////////////////////////////////////////////////////////
// DataCenter -> Client
struct PG_Guild_DtoC_GuildLeaderChange
{
	GamePGCommandEnum	Command;
	int					NewLeaderDBID;
	int					OldLeaderDBID;

	PG_Guild_DtoC_GuildLeaderChange( )
	{	Command = EM_PG_Guild_DtoC_GuildLeaderChange;	}
};
//////////////////////////////////////////////////////////////////////////
// DataCenter -> Client
struct PG_Guild_DtoC_GuildMemberRankChange
{
	GamePGCommandEnum	Command;
	int				MemberDBID;
	int				Rank;

	PG_Guild_DtoC_GuildMemberRankChange( )
	{	Command = EM_PG_Guild_DtoC_GuildMemberRankChange;	}
};
//////////////////////////////////////////////////////////////////////////
struct PG_Guild_DtoC_GuildFightAuctionEnd
{
	GamePGCommandEnum	Command;

	int					iCrystalID;
	int					iDefenderGuildID;
	int					iAttackerGuildID;
	int					iFightTime;
	int					iHighestAmount;

	PG_Guild_DtoC_GuildFightAuctionEnd( )
	{	Command = EM_PG_Guild_DtoC_GuildFightAuctionEnd;	}

};

struct PG_Guild_DtoC_GuildFightBeginNotify
{
	GamePGCommandEnum	Command;

	int					iCrystalID;
	int					iRemainTime;

	PG_Guild_DtoC_GuildFightBeginNotify( )
	{	Command = EM_PG_Guild_DtoC_GuildFightBeginNotify;	}

};

struct PG_Guild_DtoL_CreateGuildFightRoom
{
	GamePGCommandEnum	Command;

	int					iCrystalID;
	int					iDefenderGuildID;
	int					iAttackerGuildID;

	PG_Guild_DtoL_CreateGuildFightRoom( )
	{	Command = EM_PG_Guild_DtoL_CreateGuildFightRoom;	}

};

struct PG_Guild_LtoC_GuildFightLeaderInvite
{
	GamePGCommandEnum	Command;

	int					iCrystalID;
	int					iDefenderGuildID;
	int					iAttackerGuildID;
	int					iSrvID;					// SrvID
	int					iRoomID;				// 輔助驗證用

	PG_Guild_LtoC_GuildFightLeaderInvite( )
	{	Command = EM_PG_Guild_LtoC_GuildFightLeaderInvite;	}

};

struct PG_Guild_CtoL_GuildFightRequestInvite
{
	GamePGCommandEnum	Command;

	int					iRoomID;
	int					iCrystalID;
	int					iMemberDBID;
	int					iGuildID;

	PG_Guild_CtoL_GuildFightRequestInvite( )
	{	Command = EM_PG_Guild_CtoL_GuildFightRequestInvite;	}
};

struct PG_Guild_LtoC_GuildFightInviteNotify
{
	GamePGCommandEnum	Command;

	int					iSrvID;
	int					iRoomID;
	int					iCrystalID;
	

	PG_Guild_LtoC_GuildFightInviteNotify( )
	{	Command = EM_PG_Guild_LtoC_GuildFightInviteNotify;	}
};

struct PG_Guild_CtoL_GuildFightInviteRespond
{
	GamePGCommandEnum	Command;

	int					iRoomID;
	int					iCrystalID;
	bool				bJoin;
	int					iMemberDBID;
	int					iGuildID;

	PG_Guild_CtoL_GuildFightInviteRespond( )
	{	Command = EM_PG_Guild_CtoL_GuildFightInviteRespond;	}
};

//通知周圍玩家某人公會Id改變
//Local -> Client
struct PG_Guild_LtoC_GuildIDChange
{
	GamePGCommandEnum	Command;
	int		GItemID;
	int		GuildID;

	PG_Guild_LtoC_GuildIDChange( )
	{	Command = EM_PG_Guild_LtoC_GuildIDChange;	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local	
//公會會長要求與某 公會成敵對狀況
struct PG_Guild_CtoL_DeclareWar
{
	GamePGCommandEnum	Command;
	int		Type;				//宣戰類型
	int		TargetGuildID;

	PG_Guild_CtoL_DeclareWar( )
	{	Command = EM_PG_Guild_CtoL_DeclareWar;	}
};

//Local	-> DataCenter
//公會會長要求與某 公會成敵對狀況
struct PG_Guild_LtoD_DeclareWar
{
	GamePGCommandEnum	Command;
	int		Type;				//宣戰類型
	int		PlayerDBID;
	int		TargetGuildID;

	PG_Guild_LtoD_DeclareWar( )
	{	Command = EM_PG_Guild_LtoD_DeclareWar;	}
};

enum DeclareWarResultENUM
{
	EM_DeclareWarResult_OK				,
	EM_DeclareWarResult_DataErr			,	//資料錯誤
	EM_DeclareWarResult_TimeErr			,	//宣戰過快
	EM_DeclareWarResult_OnWarErr		,	//宣戰中
	EM_DeclareWarResult_OnWarPrepareErr	,	//宣戰準備中
	EM_DeclareWarResult_ScoreErr		,	//公會機分不足
	EM_DeclareWarResult_MonyeErr		,	//金錢不足
	EM_DeclareWarResult_LeaderErr		,	//不是會長
	EM_DeclareWarResult_GuildLvErr		,	//公會等級不足
	EM_DeclareWarResult_TargetGuildErr	,	//目標公會不存在
	EM_DeclareWarResult_TargetGuildLvErr,	//目標公會等級不足
};

//DataCenter -> Local
//宣戰結果
struct PG_Guild_DtoL_DeclareWarResult
{
	GamePGCommandEnum	Command;
	int		Type;				//宣戰類型
	int		PlayerDBID;
	int		GuildID;
	int		TargetGuildID;
	DeclareWarResultENUM	Result;

	PG_Guild_DtoL_DeclareWarResult( )
	{	Command = EM_PG_Guild_DtoL_DeclareWarResult;	}
};

//Local -> DataCenter
//宣戰結果
struct PG_Guild_LtoC_DeclareWarResult
{
	GamePGCommandEnum	Command;
	int		Type;				//宣戰類型
	int		TargetGuildID;
	DeclareWarResultENUM	Result;

	PG_Guild_LtoC_DeclareWarResult( )
	{	Command = EM_PG_Guild_LtoC_DeclareWarResult;	}
};

//DataCenter -> Client
//宣戰資訊 
struct PG_Guild_DtoC_PrepareWar
{
	GamePGCommandEnum	Command;
	int			GuildID;
	int			Time;			//宣戰時間(秒)

	PG_Guild_DtoC_PrepareWar( )
	{	Command = EM_PG_Guild_DtoC_PrepareWar;	}
};


//DataCenter -> Local
//宣戰資訊 
struct PG_Guild_DtoL_WarInfo
{
	GamePGCommandEnum	Command;
	bool		IsDeclareWar;
	int			GuildID[2];
	int			Time;			//宣戰時間
	
	PG_Guild_DtoL_WarInfo( )
	{	Command = EM_PG_Guild_DtoL_WarInfo;	}
};

//Local -> Client
//及時宣戰資訊 
struct PG_Guild_LtoC_WarInfo
{
	GamePGCommandEnum	Command;
	bool		IsDeclareWar;
	int			GuildID[2];
	int			Time;
	int			BeginTime;

	PG_Guild_LtoC_WarInfo( )
	{	Command = EM_PG_Guild_LtoC_WarInfo;	}
};

//Local -> Client
//宣戰資訊 
struct PG_Guild_LtoC_LoginWarInfo
{
	GamePGCommandEnum	Command;
	int			GuildID[2];
	int			Time;
	int			BeginTime;

	PG_Guild_LtoC_LoginWarInfo( )
	{	Command = EM_PG_Guild_LtoC_LoginWarInfo;	}
};

//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter
//公會一般宣戰 積分改變
struct PG_Guild_LtoD_DeclareWarScore
{
	GamePGCommandEnum	Command;
	int			GuildID[2];
	int			Score;
	int			KillCount;

	PG_Guild_LtoD_DeclareWarScore( )
	{	Command = EM_PG_Guild_LtoD_DeclareWarScore;	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local
//公會一般宣戰 戰鬥結果
struct PG_Guild_DtoC_DeclareWarFinalResult
{
	GamePGCommandEnum	Command;
	int			GuildID[2];
	int			Score[2];
	int			KillCount[2];

	PG_Guild_DtoC_DeclareWarFinalResult( )
	{	Command = EM_PG_Guild_DtoC_DeclareWarFinalResult;	}
};
//////////////////////////////////////////////////////////////////////////
/*
//Local -> Client 
//開啟公會旗設定
struct PG_Guild_LtoC_OpenSetGuildFlagInfo
{
	GamePGCommandEnum	Command;
	//GuildFlagStruct		Flag;
	PG_Guild_LtoC_OpenSetGuildFlagInfo( )
	{	Command = EM_PG_Guild_LtoC_OpenSetGuildFlagInfo;	}
};
*/
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//公會旗設定
struct PG_Guild_CtoL_SetGuildFlagInfoRequest
{
	GamePGCommandEnum	Command;
	GuildFlagStruct		Flag;
	PG_Guild_CtoL_SetGuildFlagInfoRequest( )
	{	Command = EM_PG_Guild_CtoL_SetGuildFlagInfoRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Datacenter
//公會旗設定
struct PG_Guild_LtoD_SetGuildFlagInfoRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	GuildFlagStruct		Flag;
	PG_Guild_LtoD_SetGuildFlagInfoRequest( )
	{	Command = EM_PG_Guild_LtoD_SetGuildFlagInfoRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Datacenter
//公會旗設定
enum	SetGuildFlagInfoResultENUM
{
	EM_SetGuildFlagInfoResult_OK			,	
	EM_SetGuildFlagInfoResult_GuildError	,	//公會錯誤
	EM_SetGuildFlagInfoResult_LeaderError	,	//不是會長
	EM_SetGuildFlagInfoResult_ScoreError	,	//積分不足
};

struct PG_Guild_XtoC_SetGuildFlagInfoResult
{
	GamePGCommandEnum	Command;
	SetGuildFlagInfoResultENUM		Result;

	PG_Guild_XtoC_SetGuildFlagInfoResult( )
	{	Command = EM_PG_Guild_XtoC_SetGuildFlagInfoResult;	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//更改公會名字
struct PG_Guild_CtoL_ChangeGuildNameRequest
{
	GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_>	GuildName;	//公會名稱

	PG_Guild_CtoL_ChangeGuildNameRequest( )
	{	Command = EM_PG_Guild_CtoL_ChangeGuildNameRequest;	}
};
//////////////////////////////////////////////////////////////////////////
// Local -> DataCenter
//更改公會名字
struct PG_Guild_LtoD_ChangeGuildNameRequest
{
	GamePGCommandEnum	Command;
	int					GuildID;
	int					PlayerDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	GuildName;	//公會名稱

	PG_Guild_LtoD_ChangeGuildNameRequest( )
	{	Command = EM_PG_Guild_LtoD_ChangeGuildNameRequest;	}
};
//////////////////////////////////////////////////////////////////////////
enum ChangeGuildNameResultENUM
{
	EM_ChangeGuildNameResult_OK				,
	EM_ChangeGuildNameResult_NameUsed		,		//名字重複
	EM_ChangeGuildNameResult_FlagError		,		//沒有開啟改名
	EM_ChangeGuildNameResult_NotLeader		,		//不是會長	
	EM_ChangeGuildNameResult_NameToShort	,		// 名稱太短,不合法
	EM_ChangeGuildNameResult_NameToLong		,		// 名稱太長,不合法
	EM_ChangeGuildNameResult_NameIsWrongful	,		// 有錯誤的符號或字元
	EM_ChangeGuildNameResult_HasSpecialString,		// 特殊名稱不能用
	EM_ChangeGuildNameResult_OnlyBasicLetters,		// 只能使用基本英文字母
	EM_ChangeGuildNameResult_EndLetterError	,		// 結尾字元錯誤(',")
	EM_ChangeGuildNameResult_UnknowErr		,		//未知錯誤
};
// DataCenter -> Client
//更改公會名字
struct PG_Guild_DtoC_ChangeGuildNameResult
{
	GamePGCommandEnum				Command;
	ChangeGuildNameResultENUM		Result;

	PG_Guild_DtoC_ChangeGuildNameResult( )
	{	Command = EM_PG_Guild_DtoC_ChangeGuildNameResult;	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Client
//通知所有玩家某公會改明
struct PG_Guild_DtoC_ChangeGuildName
{
	GamePGCommandEnum	Command;
	int					GuildID;
	StaticString <_MAX_NAMESTRING_LEN_>	GuildName;	//公會名稱
	

	PG_Guild_DtoC_ChangeGuildName( )
	{	Command = EM_PG_Guild_DtoC_ChangeGuildName;	}
};

//////////////////////////////////////////////////////////////////////////
//公會升級
//////////////////////////////////////////////////////////////////////////
//Client -> Local 
//要求公會升級
struct PG_Guild_CtoL_GuildUpgradeRequest
{
	GamePGCommandEnum	Command;
	int			GuildLv;

	PG_Guild_CtoL_GuildUpgradeRequest( )
	{	Command = EM_PG_Guild_CtoL_GuildUpgradeRequest;	}
};

//Local -> DataCenter
//要求公會升級
struct PG_Guild_LtoD_GuildUpgradeRequest
{
	GamePGCommandEnum	Command;
	int			GuildID;
	int			GuildLv;
	int			PlayerDBID;		//要求者

	PG_Guild_LtoD_GuildUpgradeRequest( )
	{	Command = EM_PG_Guild_LtoD_GuildUpgradeRequest;	}
};

//DataCenter->Client
//回應公會升級結果
struct PG_Guild_DtoC_GuildUpgradeResult
{
	GamePGCommandEnum	Command;
	int			GuildLv;
	bool		Result;

	PG_Guild_DtoC_GuildUpgradeResult( )
	{	Command = EM_PG_Guild_DtoC_GuildUpgradeResult;	}
};

//////////////////////////////////////////////////////////////////////////
enum AddGuildResourceTypeENUM
{
	EM_AddGuildResourceType_Contribution	,	//捐獻
	EM_AddGuildResourceType_Build			,	//公會建築
	EM_AddGuildResourceType_Upgrade			,	//公會建築升級
	EM_AddGuildResourceType_Plot			,	//劇情
	EM_AddGuildResourceType_Tax				,	//建築物維持費
	EM_AddGuildResourceType_Active			,	//建築物致能
	EM_AddGuildResourceType_Buy_ItemPage	,	//購買公會放物品的頁面
	EM_AddGuildResourceType_BuildDestroy	,	//摧毀建築物
	EM_AddGuildResourceType_Buy_GuildStyle	,	//購買公會樣式
};
//增加公會積分
//Local -> Datacenter
struct PG_Guild_LtoD_AddGuildResource
{
	GamePGCommandEnum	Command;
	int					GuildID;
	int					PlayerDBID;
	GuildResourceStruct	Resource;

	AddGuildResourceTypeENUM LogType;
	int					LogType_ObjID;
	

	StaticString <_MAX_NAMESTRING_LEN_>	Ret_Runplot;	//回傳後值行的劇情   bool Runplot( Result )

	PG_Guild_LtoD_AddGuildResource( )
	{	Command = EM_PG_Guild_LtoD_AddGuildResource;	}
};

//增加公會積分
//DataCenter -> Local
struct PG_Guild_DtoL_AddGuildResourceResult
{
	GamePGCommandEnum	Command;
	int					GuildID;
	int					PlayerDBID;
	GuildResourceStruct	Resource;

	StaticString <_MAX_NAMESTRING_LEN_>	Ret_Runplot;	//回傳後值行的劇情   bool Runplot( Result )
	bool				Result;

	PG_Guild_DtoL_AddGuildResourceResult( )
	{	Command = EM_PG_Guild_DtoL_AddGuildResourceResult;	}
};
//////////////////////////////////////////////////////////////////////////
//要求某天的公會捐獻資料
//Client -> Local
struct PG_Guild_CtoL_SelectContributionLog
{
	GamePGCommandEnum	Command;
	int					DayBefore;			//幾天前

	PG_Guild_CtoL_SelectContributionLog( )
	{	Command = EM_PG_Guild_CtoL_SelectContributionLog;	}
};

//要求某天的公會捐獻資料
//Local -> DataCenter
struct PG_Guild_LtoD_SelectContributionLog
{
	GamePGCommandEnum	Command;
	int					DayBefore;			//幾天前
	int					PlayerDBID;

	PG_Guild_LtoD_SelectContributionLog( )
	{	Command = EM_PG_Guild_LtoD_SelectContributionLog;	}
};

//要求某天的公會捐獻資料
//DataCenter -> Client
struct ContributionInfoLogStruct
{
	int		PlayerDBID;
	int		Time;
	GuildResourceStruct Resource;
};
struct PG_Guild_DtoC_SelectContributionLogResult
{
	GamePGCommandEnum	Command;
	int		Count;
	ContributionInfoLogStruct Log[500];

	int Size()
	{
		return sizeof(int)*2 + Count* sizeof(ContributionInfoLogStruct);
	}

	PG_Guild_DtoC_SelectContributionLogResult( )
	{	Command = EM_PG_Guild_DtoC_SelectContributionLogResult;	}
};

//公會資料有被更新
//DataCenter -> Client
struct PG_Guild_DtoC_GuildBaseInfoUpdate
{
	GamePGCommandEnum	Command;
	PG_Guild_DtoC_GuildBaseInfoUpdate( )
	{	Command = EM_PG_Guild_DtoC_GuildBaseInfoUpdate;	}
};

//////////////////////////////////////////////////////////////////////////
//通知公會升級
//DataCenter -> Client
struct PG_Guild_DtoC_Guild_LevelUp
{
	GamePGCommandEnum	Command;
	int					GuildID;
	int					Level;
	PG_Guild_DtoC_Guild_LevelUp( )
	{	Command = EM_PG_Guild_DtoC_Guild_LevelUp;	}
};

//////////////////////////////////////////////////////////////////////////
//設定公會需改名
//Local -> DataCenter
struct PG_Guild_LtoD_SetNeedChangeName
{
	GamePGCommandEnum	Command;
	int					GuildID;
	PG_Guild_LtoD_SetNeedChangeName( )
	{	Command = EM_PG_Guild_LtoD_SetNeedChangeName;	}
};
//////////////////////////////////////////////////////////////////////////

//通知公會成員有公會資料更新
//DataCenter -> Client
struct PG_Guild_DtoC_GuildInfoChange
{
	GamePGCommandEnum		Command;
	GuildInfoChangeStruct	ChangeMode;

	PG_Guild_DtoC_GuildInfoChange( )
	{	Command = EM_PG_Guild_DtoC_GuildInfoChange;	}
};
//////////////////////////////////////////////////////////////////////////
//通知DataCenter 公會戰積分改變
struct PG_Guild_LtoD_GuildWarEndResult
{
	GamePGCommandEnum	Command;
	int						GuildID;
	GameResultENUM			GameResult;
	int						GuildScore;		//公會積分修正
	GuildWarBaseInfoStruct	TargetGuild;
	PG_Guild_LtoD_GuildWarEndResult()
	{Command = EM_PG_Guild_LtoD_GuildWarEndResult;}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//要求公會戰歷史資料
struct PG_Guild_CtoL_GuildWarHistroy
{
	GamePGCommandEnum	Command;

	PG_Guild_CtoL_GuildWarHistroy( )
	{	Command = EM_PG_Guild_CtoL_GuildWarHistroy;	}
};

//Local -> DataCenter
//要求公會戰歷史資料
struct PG_Guild_LtoD_GuildWarHistroy
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;

	PG_Guild_LtoD_GuildWarHistroy( )
	{	Command = EM_PG_Guild_LtoD_GuildWarHistroy;	}
};

//DataCenter -> Client
//要求公會戰歷史資料結果
struct PG_Guild_DtoC_GuildWarHistroyResult
{
	GamePGCommandEnum	Command;
	int							Count;
	GuildHouseWarHistoryStruct	Log[50];

	PG_Guild_DtoC_GuildWarHistroyResult( )
	{	Command = EM_PG_Guild_DtoC_GuildWarHistroyResult;	}

	int PGSize()
	{
		return sizeof(int)*2 + Count* sizeof(GuildHouseWarHistoryStruct);
	}

};