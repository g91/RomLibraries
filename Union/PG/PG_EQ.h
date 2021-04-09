#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged


//-----------------------------------------------------------------------------------------

// Client -> Local
// �q��Server Client �n�˳ƪ��~
struct PG_EQ_CtoL_SetEQ
{
    GamePGCommandEnum	Command;

    ItemFieldStruct		Item;
    int					ItemPos;
    EQWearPosENUM		EqPos;

    short               PosType;

	bool				IsFromEq;		//�ӷ���EQ

    PG_EQ_CtoL_SetEQ( )
    {	Command = EM_PG_EQ_CtoL_SetEQ;	}
};
//-----------------------------------------------------------------------------------------
//Client -> Local �洫�ƥ��˳�
struct PG_EQ_CtoL_SwapBackupEQ
{
	GamePGCommandEnum	Command;
	int					BackupPosID;

	PG_EQ_CtoL_SwapBackupEQ( )
	{	Command = EM_PG_EQ_CtoL_SwapBackupEQ;	}
};
//-----------------------------------------------------------------------------------------
//Local -> Client �洫�ƥ��˳�
struct PG_EQ_LtoC_SwapBackupEQ
{
	GamePGCommandEnum	Command;
	int					BackupPosID;
	bool				Result;
	PG_EQ_LtoC_SwapBackupEQ( )
	{	Command = EM_PG_EQ_LtoC_SwapBackupEQ;	}
};
//-----------------------------------------------------------------------------------------
enum EQResult_ENUM
{
    ENMU_EQ_SetEqOK			= 0     ,
    ENMU_EQ_SetEqFailed		    	,
};

// Local -> Client
 
struct PG_EQ_LtoC_SetEQResult
{
    GamePGCommandEnum	Command;

    EQResult_ENUM		Result;

    PG_EQ_LtoC_SetEQResult( )
    {	Command = EM_PG_EQ_LtoC_SetEQResult;	}
};
//-----------------------------------------------------------------------------------------
// Client -> Local
// �q��Server �O�_�n���ø˳�
struct PG_EQ_CtoL_SetShowEQ
{
	GamePGCommandEnum	Command;
	ShowEQStruct		ShowEQ;

	PG_EQ_CtoL_SetShowEQ( )
	{	Command = EM_PG_EQ_CtoL_SetShowEQ;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// �q��Client ���ø˳Ƹ˳Ƹ��
struct PG_EQ_LtoC_SetShowEQ
{
	GamePGCommandEnum	Command;
	int					GItemID;
	ShowEQStruct		ShowEQ;

	PG_EQ_LtoC_SetShowEQ( )
	{	Command = EM_PG_EQ_LtoC_SetShowEQ;	}
};

//-----------------------------------------------------------------------------------------
// Local -> Client
// �q��Client �˳��z�F
struct PG_EQ_LtoC_EQBroken
{
	GamePGCommandEnum	Command;
	EQWearPosENUM		Pos;

	PG_EQ_LtoC_EQBroken( )
	{	Command = EM_PG_EQ_LtoC_EQBroken;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client
//�}�ҮM�˾ǲߤ���
struct PG_EQ_LtoC_LearnSuitSkill_Open
{
	GamePGCommandEnum	Command;
	int			TargetID;

	PG_EQ_LtoC_LearnSuitSkill_Open( )
	{	Command = EM_PG_EQ_LtoC_LearnSuitSkill_Open;	}
};
//-----------------------------------------------------------------------------------------
//Client -> Local 
//��������
struct PG_EQ_CtoL_LearnSuitSkill_Close
{
	GamePGCommandEnum	Command;

	PG_EQ_CtoL_LearnSuitSkill_Close( )
	{	Command = EM_PG_EQ_CtoL_LearnSuitSkill_Close;	}
};
//-----------------------------------------------------------------------------------------
//Client -> Local
//�ǲߧޯ�
struct PG_EQ_CtoL_LearnSuitSkillRequest
{
	GamePGCommandEnum	Command;
	int		ItemCount;		//�M�˼ƶq		
	int		ItemPos[10];	//���骫�~��m ������ 0 ��ܪ��~�d 1��ܸ˳��d
	int		LearnSkillID;	//�ǲߪ��ޯ�

	PG_EQ_CtoL_LearnSuitSkillRequest( )
	{	Command = EM_PG_EQ_CtoL_LearnSuitSkillRequest;	}
};
//-----------------------------------------------------------------------------------------
enum	LearnSuitSkillResultENUM
{
	EM_LearnSuitSkillResult_OK			,
	EM_LearnSuitSkillResult_NotOpen		,	//�����S�}��
	EM_LearnSuitSkillResult_DataErr		,	//��Ʀ����D
	EM_LearnSuitSkillResult_CountErr	,	//�M�˼ƶq�����D
	EM_LearnSuitSkillResult_AlwaysLearn	,	//�w�Ƿ|
	EM_LearnSuitSkillResult_SkillFull	,	//�ޯ�w��
	EM_LearnSuitSkillResult_MoneyErr	,	//��������
};
//Local -> Client
//�ǲߵ��G
struct PG_EQ_LtoC_LearnSuitSkillResult
{
	GamePGCommandEnum	Command;
	int					LearnSkillID;	//�ǲߪ��ޯ�
	LearnSuitSkillResultENUM Result;

	PG_EQ_LtoC_LearnSuitSkillResult( )
	{	Command = EM_PG_EQ_LtoC_LearnSuitSkillResult;	}
};
//-----------------------------------------------------------------------------------------
//�]�w�ϥΤ����M�˧ޯ�
//Client -> Local
struct PG_EQ_CtoL_SetSuitSkillRequest
{
	GamePGCommandEnum	Command;
	int		SkillID;
	int		Pos;

	PG_EQ_CtoL_SetSuitSkillRequest( )
	{	Command = EM_PG_EQ_CtoL_SetSuitSkillRequest;	}
};
enum	SetSuitSkillResultENUM
{
	EM_SetSuitSkillResult_OK			,
	EM_SetSuitSkillResult_OnAttackMode	,	//�԰����A����ϥ�
	EM_SetSuitSkillResult_SkillNotFind	,	//�d�L���ޯ�
	EM_SetSuitSkillResult_PosErr		,	//��m���~
};
//Local -> Client
//�]�w���G
struct PG_EQ_LtoC_SetSuitSkillResult
{
	GamePGCommandEnum	Command;
	SetSuitSkillResultENUM Result;
	
	PG_EQ_LtoC_SetSuitSkillResult( )
	{	Command = EM_PG_EQ_LtoC_SetSuitSkillResult;	}
};
//-----------------------------------------------------------------------------------------
//Client -> Local
//�n�D�ϥήM�˧ޯ�
struct PG_EQ_CtoL_UseSuitSkillRequest
{
	GamePGCommandEnum	Command;
	int		SkillID;
	int		TargetID;
	float	PosX , PosY , PosZ ;
	PG_EQ_CtoL_UseSuitSkillRequest( )
	{	Command = EM_PG_EQ_CtoL_UseSuitSkillRequest;	}
};
enum	UseSuitSkillResultENUM
{
	EM_UseSuitSkillResult_OK			,
	EM_UseSuitSkillResult_SkillNotFind	,	//�䤣��ޯ�
};
//Local -> Client
//�]�w���G
struct PG_EQ_LtoC_UseSuitSkillResult
{
	GamePGCommandEnum	Command;
	UseSuitSkillResultENUM Result;

	PG_EQ_LtoC_UseSuitSkillResult( )
	{	Command = EM_PG_EQ_LtoC_UseSuitSkillResult;	}
};
//-----------------------------------------------------------------------------------------
#pragma managed
