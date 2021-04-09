#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged
//-------------------------------------------------------------------------------
//Client -> Server 
//單挑要請
struct PG_PK_CtoL_PKInviteRequest
{
	GamePGCommandEnum	Command;
	int					TargetID;

	PG_PK_CtoL_PKInviteRequest( )
	{	Command = EM_PG_PK_CtoL_PKInviteRequest;	}
};
//-------------------------------------------------------------------------------
//Server -> Client
//單挑要請
struct PG_PK_LtoC_PKInviteNotify
{
	GamePGCommandEnum	Command;
	int					TargetID;
	int					TargetScore;

	PG_PK_LtoC_PKInviteNotify( )
	{	Command = EM_PG_PK_LtoC_PKInviteNotify;	}
};
//-------------------------------------------------------------------------------
//Client -> Server 
//單挑回應
struct PG_PK_CtoL_PKInviteResult
{
	GamePGCommandEnum	Command;
	int					TargetID;
	bool				Result;

	PG_PK_CtoL_PKInviteResult( )
	{	Command = EM_PG_PK_CtoL_PKInviteResult;	}
};
//-------------------------------------------------------------------------------
//Server -> Client
//單挑回應
struct PG_PK_LtoC_PKInviteResult
{
	GamePGCommandEnum	Command;
	int					TargetID;
	bool				Result;

	PG_PK_LtoC_PKInviteResult( )
	{	Command = EM_PG_PK_LtoC_PKInviteResult;	}
};
//-------------------------------------------------------------------------------
//Server -> Client
//單挑開始
struct PG_PK_LtoC_PKPrepare
{
	GamePGCommandEnum	Command;
	int					Player1ID;
	int					Player1Score;			//玩家PK基分
	int					Player2ID;
	int					Player2Score;			//玩家PK基分

	int					PosX , PosY , PosZ;		//位置
	int					Range;					//範圍
	int					Time;					//秒


	PG_PK_LtoC_PKPrepare( )
	{	Command = EM_PG_PK_LtoC_PKPrepare;	}
};
//-------------------------------------------------------------------------------
//Server -> Client
//單挑開始
struct PG_PK_LtoC_PKBegin
{
	GamePGCommandEnum	Command;
	int			Time;		//PK時間秒
	
	PG_PK_LtoC_PKBegin( )
	{	Command = EM_PG_PK_LtoC_PKBegin;	}
};
//-------------------------------------------------------------------------------
//Server -> Client
//單挑結束
struct PG_PK_LtoC_PKEnd
{
	GamePGCommandEnum	Command;
	int					WinnerID;
	int					LoserID;
	bool				IsEven;		//平手

	PG_PK_LtoC_PKEnd( )
	{	Command = EM_PG_PK_LtoC_PKEnd;	}
};
//-------------------------------------------------------------------------------
//Client -> Server
//取消單挑
struct PG_PK_CtoL_PKCancel
{
	GamePGCommandEnum	Command;

	PG_PK_CtoL_PKCancel( )
	{	Command = EM_PG_PK_CtoL_PKCancel;	}
};
//-------------------------------------------------------------------------------
//Client -> Server
//取消單挑
struct PG_PK_LtoC_PKCancel
{
	GamePGCommandEnum	Command;

	PG_PK_LtoC_PKCancel( )
	{	Command = EM_PG_PK_LtoC_PKCancel;	}
};
//-------------------------------------------------------------------------------
#pragma managed

