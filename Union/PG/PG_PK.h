#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged
//-------------------------------------------------------------------------------
//Client -> Server 
//��D�n��
struct PG_PK_CtoL_PKInviteRequest
{
	GamePGCommandEnum	Command;
	int					TargetID;

	PG_PK_CtoL_PKInviteRequest( )
	{	Command = EM_PG_PK_CtoL_PKInviteRequest;	}
};
//-------------------------------------------------------------------------------
//Server -> Client
//��D�n��
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
//��D�^��
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
//��D�^��
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
//��D�}�l
struct PG_PK_LtoC_PKPrepare
{
	GamePGCommandEnum	Command;
	int					Player1ID;
	int					Player1Score;			//���aPK���
	int					Player2ID;
	int					Player2Score;			//���aPK���

	int					PosX , PosY , PosZ;		//��m
	int					Range;					//�d��
	int					Time;					//��


	PG_PK_LtoC_PKPrepare( )
	{	Command = EM_PG_PK_LtoC_PKPrepare;	}
};
//-------------------------------------------------------------------------------
//Server -> Client
//��D�}�l
struct PG_PK_LtoC_PKBegin
{
	GamePGCommandEnum	Command;
	int			Time;		//PK�ɶ���
	
	PG_PK_LtoC_PKBegin( )
	{	Command = EM_PG_PK_LtoC_PKBegin;	}
};
//-------------------------------------------------------------------------------
//Server -> Client
//��D����
struct PG_PK_LtoC_PKEnd
{
	GamePGCommandEnum	Command;
	int					WinnerID;
	int					LoserID;
	bool				IsEven;		//����

	PG_PK_LtoC_PKEnd( )
	{	Command = EM_PG_PK_LtoC_PKEnd;	}
};
//-------------------------------------------------------------------------------
//Client -> Server
//������D
struct PG_PK_CtoL_PKCancel
{
	GamePGCommandEnum	Command;

	PG_PK_CtoL_PKCancel( )
	{	Command = EM_PG_PK_CtoL_PKCancel;	}
};
//-------------------------------------------------------------------------------
//Client -> Server
//������D
struct PG_PK_LtoC_PKCancel
{
	GamePGCommandEnum	Command;

	PG_PK_LtoC_PKCancel( )
	{	Command = EM_PG_PK_LtoC_PKCancel;	}
};
//-------------------------------------------------------------------------------
#pragma managed

