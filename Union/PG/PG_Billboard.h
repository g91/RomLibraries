#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

//////////////////////////////////////////////////////////////////////////

//�浧�Ʀ���
struct BillBoardRoleInfoStruct
{
	int					Rank;			//�ƦW
	int					MainRankInfo;	//�D�n�ƦW��T
	bool				IsAnonymous;	//�ΦW

	StaticString< _MAX_NAMERECORD_SIZE_ > RoleName;
	int					DBID;			//��ƮwID

	Voc_ENUM		    Voc;			//¾�~
	Voc_ENUM		    Voc_Sub;		//��¾�~
	int					Lv;				//¾�~����
	int					Lv_Sub;			//��¾�~����
	int 				Money;
	int					TitleCount;		//�Y�μƶq
	int					PKScore;		//pk����

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
//�}�ұƦ�]
//Local -> Client 
struct PG_Billboard_LtoC_OpenBillboard
{
	GamePGCommandEnum	Command;
	int					TargetNpcID;

	PG_Billboard_LtoC_OpenBillboard( )
	{	Command = EM_PG_Billboard_LtoC_OpenBillboard;	}
};
//////////////////////////////////////////////////////////////////////////
//�����Ʀ�]
//Local -> Client 
struct PG_Billboard_LtoC_CloseBillboard
{
	GamePGCommandEnum	Command;

	PG_Billboard_LtoC_CloseBillboard( )
	{	Command = EM_PG_Billboard_LtoC_CloseBillboard;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�����Ʀ�]
//Client -> Local
struct PG_Billboard_CtoL_CloseBillboardRequest
{
	GamePGCommandEnum	Command;

	PG_Billboard_CtoL_CloseBillboardRequest( )
	{	Command = EM_PG_Billboard_CtoL_CloseBillboardRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�ƦW��T
//Client -> Local
struct PG_Billboard_CtoL_RankInfoRequest
{
	GamePGCommandEnum		Command;
	BillBoardMainTypeENUM	Type;
	int						RankBegin;		//-1 �N����ۤv���Ū���

	PG_Billboard_CtoL_RankInfoRequest( )
	{	Command = EM_PG_Billboard_CtoL_RankInfoRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�ƦW���
//Local -> DataCenter
struct PG_Billboard_LtoD_RankInfoRequest
{
	GamePGCommandEnum		Command;
	BillBoardMainTypeENUM	Type;
	int						RankBegin;		//-1 �N����ۤv���Ū���
	int						CliDBID;
	int						CliSockID;
	int						CliProxyID;

	PG_Billboard_LtoD_RankInfoRequest( )
	{	Command = EM_PG_Billboard_LtoD_RankInfoRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//�^���ƦW��T
//DataCenter -> Client
struct PG_Billboard_DtoC_RankInfoResult
{
	GamePGCommandEnum		Command;
	BillBoardMainTypeENUM	Type;
	int						RankBegin;		//�}�l������
	int						RankEnd;		//����������
	int						RankMax;		//�̤j��
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
//�]�w�O�_�n�ΰΦW
//Client -> Local
struct PG_Billboard_CtoL_SetAnonymous
{
	GamePGCommandEnum		Command;

	bool					IsAnonymous;	

	PG_Billboard_CtoL_SetAnonymous( )
	{	Command = EM_PG_Billboard_CtoL_SetAnonymous;	}
};
//////////////////////////////////////////////////////////////////////////
//�]�w�O�_�n�ΰΦW
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
//�s���Ʀ�]����
//////////////////////////////////////////////////////////////////////////
//Client -> Local �d�߱Ʀ�
struct PG_NewBillboard_CtoL_SearchRank
{
	GamePGCommandEnum		Command;
	int						SortType;		//�Ʀ�����
	int						MinRank;
	int						MaxRank;

	PG_NewBillboard_CtoL_SearchRank( )
	{	Command = EM_PG_NewBillboard_CtoL_SearchRank;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter �d�߱Ʀ�
struct PG_NewBillboard_LtoD_SearchRank
{
	GamePGCommandEnum		Command;
	int						PlayerDBID;
	int						SortType;		//�Ʀ�����
	int						MinRank;
	int						MaxRank;

	PG_NewBillboard_LtoD_SearchRank( )
	{	Command = EM_PG_NewBillboard_LtoD_SearchRank;	}
};
//////////////////////////////////////////////////////////////////////////
//�浧�Ʀ���
struct NewBillBoardRoleInfoStruct
{
	int					Rank;			//�ƦW
	int					LastRank;		//�W�����ƦW
	StaticString< _MAX_NAMERECORD_SIZE_ > RoleName;
	int					DBID;			//��ƮwID
	int					GuildID;
	Voc_ENUM		    Voc;			//¾�~
	Voc_ENUM		    Voc_Sub;		//��¾�~
	int					Lv;				//¾�~����
	int					Lv_Sub;			//��¾�~����

	int					SortValue;
	StaticString< 64 >  Note;			//����
};

//DataCenter -> Client
//��Ƭd�ߵ��G
struct PG_NewBillboard_DtoC_SearchRankResult
{
	GamePGCommandEnum		Command;
	int							SortType;		//�Ʀ�����
	int							TotalCount;
	int							ID;			
	NewBillBoardRoleInfoStruct	Info;

	PG_NewBillboard_DtoC_SearchRankResult( )
	{	Command = EM_PG_NewBillboard_DtoC_SearchRankResult;	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local �d�ߦۤv�Ʀ�
struct PG_NewBillboard_CtoL_SearchMyRank
{
	GamePGCommandEnum		Command;
	int						SortType;		//�Ʀ�����

	PG_NewBillboard_CtoL_SearchMyRank( )
	{	Command = EM_PG_NewBillboard_CtoL_SearchMyRank;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter �d�߱Ʀ�
struct PG_NewBillboard_LtoD_SearchMyRank
{
	GamePGCommandEnum		Command;
	int						PlayerDBID;
	int						SortType;		//�Ʀ�����

	PG_NewBillboard_LtoD_SearchMyRank( )
	{	Command = EM_PG_NewBillboard_LtoD_SearchMyRank;	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Client
//��Ƭd�ߵ��G
struct PG_NewBillboard_DtoC_SearchMyRankResult
{
	GamePGCommandEnum		Command;
	int				SortType;		//�Ʀ�����
	int				MyRank;
	int				SortValue;
	int				MyLastRank;
	StaticString< 64 >  MyNote;			//����

	PG_NewBillboard_DtoC_SearchMyRankResult( )
	{	Command = EM_PG_NewBillboard_DtoC_SearchMyRankResult;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter
//�ץ��Ʀ�]���
struct PG_NewBillboard_LtoD_Register
{
	GamePGCommandEnum		Command;
	int				PlayerDBID;
	int				SortType;		//�Ʀ�����
	int				SortValue;
	bool			IsAdd;			//�O�_�Υ[�k�٬O�]�w
	bool			IsTopUpdate;

	PG_NewBillboard_LtoD_Register( )
	{	Command = EM_PG_NewBillboard_LtoD_Register;	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//�]�w�Y�@�ӱƦW������
struct PG_NewBillboard_CtoL_RegisterNote
{
	GamePGCommandEnum		Command;
	int					SortType;		//�Ʀ�����
	StaticString< 64 >  Note;			//����

	PG_NewBillboard_CtoL_RegisterNote( )
	{	Command = EM_PG_NewBillboard_CtoL_RegisterNote;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter
//�]�w�Y�@�ӱƦW������
struct PG_NewBillboard_LtoD_RegisterNote
{
	GamePGCommandEnum		Command;
	int					PlayerDBID;
	int					SortType;		//�Ʀ�����
	StaticString< 64 >  Note;			//����

	PG_NewBillboard_LtoD_RegisterNote( )
	{	Command = EM_PG_NewBillboard_LtoD_RegisterNote;	}
};

//////////////////////////////////////////////////////////////////////////
#pragma managed

