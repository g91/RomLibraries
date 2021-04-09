#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

//////////////////////////////////////////////////////////////////////////

//單筆排行資料
struct BillBoardRoleInfoStruct
{
	int					Rank;			//排名
	int					MainRankInfo;	//主要排名資訊
	bool				IsAnonymous;	//匿名

	StaticString< _MAX_NAMERECORD_SIZE_ > RoleName;
	int					DBID;			//資料庫ID

	Voc_ENUM		    Voc;			//職業
	Voc_ENUM		    Voc_Sub;		//副職業
	int					Lv;				//職業等級
	int					Lv_Sub;			//副職業等級
	int 				Money;
	int					TitleCount;		//頭銜數量
	int					PKScore;		//pk次數

};

enum BillBoardMainTypeENUM
{
	EM_BillBoardMainType_Money				,
	EM_BillBoardMainType_Title				,
	EM_BillBoardMainType_PK					,

	EM_BillBoardMainType_Job_Warrior		= 10,
	EM_BillBoardMainType_Job_Ranger			,
	EM_BillBoardMainType_Job_Rogue			,
	EM_BillBoardMainType_Job_Wizard			,
	EM_BillBoardMainType_Job_Priest			,
	EM_BillBoardMainType_Job_Knight			,
	EM_BillBoardMainType_Job_RuneDancer		,
	EM_BillBoardMainType_Job_Druid			,

};

//////////////////////////////////////////////////////////////////////////
//開啟排行榜
//Local -> Client 
struct PG_Billboard_LtoC_OpenBillboard
{
	GamePGCommandEnum	Command;
	int					TargetNpcID;

	PG_Billboard_LtoC_OpenBillboard( )
	{	Command = EM_PG_Billboard_LtoC_OpenBillboard;	}
};
//////////////////////////////////////////////////////////////////////////
//關閉排行榜
//Local -> Client 
struct PG_Billboard_LtoC_CloseBillboard
{
	GamePGCommandEnum	Command;

	PG_Billboard_LtoC_CloseBillboard( )
	{	Command = EM_PG_Billboard_LtoC_CloseBillboard;	}
};
//////////////////////////////////////////////////////////////////////////
//要求關閉排行榜
//Client -> Local
struct PG_Billboard_CtoL_CloseBillboardRequest
{
	GamePGCommandEnum	Command;

	PG_Billboard_CtoL_CloseBillboardRequest( )
	{	Command = EM_PG_Billboard_CtoL_CloseBillboardRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//要求排名資訊
//Client -> Local
struct PG_Billboard_CtoL_RankInfoRequest
{
	GamePGCommandEnum		Command;
	BillBoardMainTypeENUM	Type;
	int						RankBegin;		//-1 代表取自己等級附近

	PG_Billboard_CtoL_RankInfoRequest( )
	{	Command = EM_PG_Billboard_CtoL_RankInfoRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//要求排名資料
//Local -> DataCenter
struct PG_Billboard_LtoD_RankInfoRequest
{
	GamePGCommandEnum		Command;
	BillBoardMainTypeENUM	Type;
	int						RankBegin;		//-1 代表取自己等級附近
	int						CliDBID;
	int						CliSockID;
	int						CliProxyID;

	PG_Billboard_LtoD_RankInfoRequest( )
	{	Command = EM_PG_Billboard_LtoD_RankInfoRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//回應排名資訊
//DataCenter -> Client
struct PG_Billboard_DtoC_RankInfoResult
{
	GamePGCommandEnum		Command;
	BillBoardMainTypeENUM	Type;
	int						RankBegin;		//開始的等級
	int						RankEnd;		//結束的等級
	int						RankMax;		//最大值
	bool					Result;

	PG_Billboard_DtoC_RankInfoResult( )
	{	Command = EM_PG_Billboard_DtoC_RankInfoResult;	}
};
//////////////////////////////////////////////////////////////////////////
struct PG_Billboard_DtoC_RankInfoList
{
	GamePGCommandEnum		Command;
	BillBoardRoleInfoStruct	Info;

	PG_Billboard_DtoC_RankInfoList( )
	{	Command = EM_PG_Billboard_DtoC_RankInfoList;	}
};
//////////////////////////////////////////////////////////////////////////
//設定是否要用匿名
//Client -> Local
struct PG_Billboard_CtoL_SetAnonymous
{
	GamePGCommandEnum		Command;

	bool					IsAnonymous;	

	PG_Billboard_CtoL_SetAnonymous( )
	{	Command = EM_PG_Billboard_CtoL_SetAnonymous;	}
};
//////////////////////////////////////////////////////////////////////////
//設定是否要用匿名
//Client -> Local
struct PG_Billboard_LtoD_SetAnonymous
{
	GamePGCommandEnum		Command;
	int						DBID;
	bool					IsAnonymous;	

	PG_Billboard_LtoD_SetAnonymous( )
	{	Command = EM_PG_Billboard_LtoD_SetAnonymous;	}
};
//////////////////////////////////////////////////////////////////////////
//新的排行榜版本
//////////////////////////////////////////////////////////////////////////
//Client -> Local 查詢排行
struct PG_NewBillboard_CtoL_SearchRank
{
	GamePGCommandEnum		Command;
	int						SortType;		//排行類型
	int						MinRank;
	int						MaxRank;

	PG_NewBillboard_CtoL_SearchRank( )
	{	Command = EM_PG_NewBillboard_CtoL_SearchRank;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter 查詢排行
struct PG_NewBillboard_LtoD_SearchRank
{
	GamePGCommandEnum		Command;
	int						PlayerDBID;
	int						SortType;		//排行類型
	int						MinRank;
	int						MaxRank;

	PG_NewBillboard_LtoD_SearchRank( )
	{	Command = EM_PG_NewBillboard_LtoD_SearchRank;	}
};
//////////////////////////////////////////////////////////////////////////
//單筆排行資料
struct NewBillBoardRoleInfoStruct
{
	int					Rank;			//排名
	int					LastRank;		//上次的排名
	StaticString< _MAX_NAMERECORD_SIZE_ > RoleName;
	int					DBID;			//資料庫ID
	int					GuildID;
	Voc_ENUM		    Voc;			//職業
	Voc_ENUM		    Voc_Sub;		//副職業
	int					Lv;				//職業等級
	int					Lv_Sub;			//副職業等級

	int					SortValue;
	StaticString< 64 >  Note;			//註解
};

//DataCenter -> Client
//資料查詢結果
struct PG_NewBillboard_DtoC_SearchRankResult
{
	GamePGCommandEnum		Command;
	int							SortType;		//排行類型
	int							TotalCount;
	int							ID;			
	NewBillBoardRoleInfoStruct	Info;

	PG_NewBillboard_DtoC_SearchRankResult( )
	{	Command = EM_PG_NewBillboard_DtoC_SearchRankResult;	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 查詢自己排行
struct PG_NewBillboard_CtoL_SearchMyRank
{
	GamePGCommandEnum		Command;
	int						SortType;		//排行類型

	PG_NewBillboard_CtoL_SearchMyRank( )
	{	Command = EM_PG_NewBillboard_CtoL_SearchMyRank;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter 查詢排行
struct PG_NewBillboard_LtoD_SearchMyRank
{
	GamePGCommandEnum		Command;
	int						PlayerDBID;
	int						SortType;		//排行類型

	PG_NewBillboard_LtoD_SearchMyRank( )
	{	Command = EM_PG_NewBillboard_LtoD_SearchMyRank;	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Client
//資料查詢結果
struct PG_NewBillboard_DtoC_SearchMyRankResult
{
	GamePGCommandEnum		Command;
	int				SortType;		//排行類型
	int				MyRank;
	int				SortValue;
	int				MyLastRank;
	StaticString< 64 >  MyNote;			//註解

	PG_NewBillboard_DtoC_SearchMyRankResult( )
	{	Command = EM_PG_NewBillboard_DtoC_SearchMyRankResult;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter
//修正排行榜資料
struct PG_NewBillboard_LtoD_Register
{
	GamePGCommandEnum		Command;
	int				PlayerDBID;
	int				SortType;		//排行類型
	int				SortValue;
	bool			IsAdd;			//是否用加法還是設定
	bool			IsTopUpdate;

	PG_NewBillboard_LtoD_Register( )
	{	Command = EM_PG_NewBillboard_LtoD_Register;	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//設定某一個排名的註解
struct PG_NewBillboard_CtoL_RegisterNote
{
	GamePGCommandEnum		Command;
	int					SortType;		//排行類型
	StaticString< 64 >  Note;			//註解

	PG_NewBillboard_CtoL_RegisterNote( )
	{	Command = EM_PG_NewBillboard_CtoL_RegisterNote;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter
//設定某一個排名的註解
struct PG_NewBillboard_LtoD_RegisterNote
{
	GamePGCommandEnum		Command;
	int					PlayerDBID;
	int					SortType;		//排行類型
	StaticString< 64 >  Note;			//註解

	PG_NewBillboard_LtoD_RegisterNote( )
	{	Command = EM_PG_NewBillboard_LtoD_RegisterNote;	}
};

//////////////////////////////////////////////////////////////////////////
#pragma managed

