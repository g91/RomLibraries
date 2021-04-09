#pragma once

#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma warning (disable:4949)
#pragma unmanaged

//-----------------------------------------------------------------------------------------
// Client -> Local
// �q��Server Client �ثe�������A

struct PG_Attack_CtoL_AttackSignal
{
    GamePGCommandEnum	Command;
    myInt32				TargetID;		//�����ؼ�
  

    PG_Attack_CtoL_AttackSignal()
    {	Command = EM_PG_Attack_CtoL_AttackSignal;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// �^���n�D�������G
enum AttackSignalResultENUM
{
    EM_AttackSignalResult_OK                    ,
    EM_AttackSignalResult_TargetDisAttackAbled  ,   //�ؼФ��i����
    EM_AttackSignalResult_TargetNULL            ,   //�L���ؼ�

};

struct PG_Attack_LtoC_AttackSignalResult
{
    GamePGCommandEnum	    Command;
    myInt32				    TargetID;		//�����ؼ�
    AttackSignalResultENUM  Result;

    PG_Attack_LtoC_AttackSignalResult()
    {	Command = EM_PG_Attack_LtoC_AttackSignalResult;	}
};
//-----------------------------------------------------------------------------------------
// Client -> Local
// �q��Server Client �����԰�

struct PG_Attack_CtoL_EndAttackSignal
{
    GamePGCommandEnum	Command;

    PG_Attack_CtoL_EndAttackSignal()
    {	Command = EM_PG_Attack_CtoL_EndAttackSignal;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client
// �q���P�򪺪��a�����R���Υ��Ѫ��T��

struct PG_Attack_LtoC_AttackResult
{
    GamePGCommandEnum	Command;

    myInt32		AttackID;		//������
    myInt32		UnderAttackID;	//�Q������
    myInt32		DamageHP;		//HP
	myInt32		OrgDamageHP;		//HP

    ATTACK_DMGTYPE_ENUM	Type;
	AttackHandTypeENUM	HandType;

    PG_Attack_LtoC_AttackResult()
    {	Command = EM_PG_Attack_LtoC_AttackResult;	}
};

//-----------------------------------------------------------------------------------------
// Local -> Client
// HP MP �ƭ��ܤƮɧ�s( ���� ���� �d�� )
struct PG_Attack_LtoC_HPMP
{
    GamePGCommandEnum	Command;

    myInt32		ItemID;		//�ϰ�ID
    myInt32		HP;
    myInt32		MP;
    myInt32       SP;
	myInt32		SP_Sub;

    PG_Attack_LtoC_HPMP()
    {	Command = EM_PG_Attack_LtoC_HPMPSP;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client 
// �q�� ���a�Y�@MaxHP , MaxMP ����T
struct PG_Attack_LtoC_MaxHPMaxMP
{
    GamePGCommandEnum	Command;

    myInt32		ItemID;		//�ϰ�ID
    myInt32		MaxHP;
    myInt32		MaxMP;
    myInt32		MaxSP;
	myInt32		MaxSP_Sub;

    PG_Attack_LtoC_MaxHPMaxMP()
    {	Command = EM_PG_Attack_LtoC_MaxHPMaxMPMaxSP;	}
};

//-----------------------------------------------------------------------------------------
struct DeadOptionMode
{
	union		
	{
		int Mode;
		struct  
		{
			bool AutoRevive	: 1;	//�۰ʭ���
			bool RaiseMagic	: 1;	//���ͪk�N
			bool Unknown	: 1;
		};
	};
};

// Local -> Client 
// �q���Y���󦺤`
struct PG_Attack_LtoC_Dead
{
    GamePGCommandEnum	Command;

    myInt32		DeadID;		//�ϰ�ID
    myInt32		KillerID;	//���H��
    myInt32       Time;		
	RolePosStruct       Pos;
	myInt32		KillerOwnerID;	//���H���֦���
	myInt32		ReviveTime;		//���ͪ��ɶ�

	DeadOptionMode Mode;		//���`�Ҧ�

    PG_Attack_LtoC_Dead()
    {	Command = EM_PG_Attack_LtoC_Dead;	}
};
//-----------------------------------------------------------------------------------------
// Local -> Client 
// �q���Y����_��
struct PG_Attack_LtoC_Resurrection
{
    GamePGCommandEnum	Command;

    myInt32		ItemID;		//�ϰ�ID

    PG_Attack_LtoC_Resurrection()
    {	Command = EM_PG_Attack_LtoC_Resurrection;	}
};
//-----------------------------------------------------------------------------------------
//���`�^�쭫���I(��󵥫ݴ_��)
// Client -> Local
enum	RequestResurrection_ENUM
{
	EM_RequestResurrection_Normal		,	//�@��_��
	EM_RequestResurrection_RaiseMagic	,	//���ͪk�N
};

struct PG_Attack_CtoL_RequestResurrection
{
    GamePGCommandEnum			Command;
	RequestResurrection_ENUM	Type;

    PG_Attack_CtoL_RequestResurrection()
    {	Command = EM_PG_Attack_CtoL_RequestResurrection;	}
};
//-----------------------------------------------------------------------------------------
//�]�w�Y����O�_����
struct PG_Attack_LtoC_SetAttackMode
{
    GamePGCommandEnum	Command;
	int					GItemID;
    bool                Flag;

    PG_Attack_LtoC_SetAttackMode()
    {	Command = EM_PG_Attack_LtoC_SetAttackMode;	}
};

//-----------------------------------------------------------------------------------------
// L -> C �q��Party������HPMPSP��T
struct PG_Attack_LtoC_PartyMemberHPMPSP
{
	GamePGCommandEnum	Command;	

	int		DBID;
	int		HP,MP,SP,SP_Sub;		

	PG_Attack_LtoC_PartyMemberHPMPSP()
	{	Command = EM_PG_Attack_LtoC_PartyMemberHPMPSP;	};
};
// L -> C �q��Party������Max HPMPSP��T
struct PG_Attack_LtoC_PartyMemberMaxHPMPSP
{
	GamePGCommandEnum	Command;	

	int		DBID;
	int		MaxHP,MaxMP,MaxSP,MaxSP_Sub;		

	PG_Attack_LtoC_PartyMemberMaxHPMPSP()
	{	Command = EM_PG_Attack_LtoC_PartyMemberMaxHPMPSP;	};
};
//-----------------------------------------------------------------------------------------
//�]�wNPC�԰���w�Y�ؼ�
//Local -> Client
struct PG_Attack_LtoC_NPCAttackTarget
{
	GamePGCommandEnum	Command;	

	int		NpcGUID;			//NPC �ϰ쪫��ID
	int		TargetGUID;			//�����ؼ�
	
	PG_Attack_LtoC_NPCAttackTarget()
	{	Command = EM_PG_Attack_LtoC_NPCAttackTarget;	};
};
//-----------------------------------------------------------------------------------------
//�q���ϰ�PK ���A
//Local -> Client
struct PG_Attack_LtoC_ZonePKFlag
{
	GamePGCommandEnum	Command;	

	PKTypeENUM	PKType;

	PG_Attack_LtoC_ZonePKFlag()
	{	Command = EM_PG_Attack_LtoC_ZonePKFlag;	};
};
//-----------------------------------------------------------------------------------------
#pragma managed
