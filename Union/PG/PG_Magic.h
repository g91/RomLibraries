#pragma     once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

//------------------------------------------------------------------------------------------
//Client -> Server
//�ǳƬI�k
struct PG_Magic_CtoL_BeginSpell
{
    GamePGCommandEnum	Command;

    int                 OwnerID;
    int				    MagicID;		//�ҭn�I���k�N
    int				    TargetID;		//�ؼ�

    int			    	MagicPos;		//�k�N��Ʀ�m

    PG_Magic_CtoL_BeginSpell()
    {
        Command = EM_PG_Magic_CtoL_BeginSpell;
    };
};
//------------------------------------------------------------------------------------------
//Server -> Client
//�ǳƬI�k(�q���g��H)
struct PG_Magic_LtoC_BeginSpell
{
    GamePGCommandEnum	Command;

    int			    	OwnerID;		//�I�k��
    int			    	TargetID;		//�I�k�ؼ�
    int			    	Magic;			//�ҭn�I���k�N
	float				TargetX;
	float				TargetY;
	float				TargetZ;

	int					SerialID;		//�P�@���I�k���ʥ]�|���ۦP�����X
	int					SpellTime;

    PG_Magic_LtoC_BeginSpell()
    {
        Command = EM_PG_Magic_LtoC_BeginSpell;
    };	
};
//------------------------------------------------------------------------------------------
//Client -> Server
//�ǳƬI�k ���w��m
struct PG_Magic_CtoL_BeginSpell_Pos
{
	GamePGCommandEnum	Command;

	int                 OwnerID;
	int				    MagicID;		//�ҭn�I���k�N
	int			    	MagicPos;		//�k�N��Ʀ�m
	float				X , Y , Z;		//��m

	PG_Magic_CtoL_BeginSpell_Pos()
	{
		Command = EM_PG_Magic_CtoL_BeginSpell_Pos;
	};
};
//------------------------------------------------------------------------------------------
//Server -> Client
//�I�k����(�q���g��H)
struct PG_Magic_LtoC_EndSpell
{
    GamePGCommandEnum	Command;

	int					SerialID;		//�P�@���I�k���ʥ]�|���ۦP�����X

    PG_Magic_LtoC_EndSpell()
    {
        Command = EM_PG_Magic_LtoC_EndSpell;
    };	
};

//------------------------------------------------------------------------------------------
//Server -> Client
//�ǳƬI�k���G
struct PG_Magic_LtoC_BeginSpellResult
{
    GamePGCommandEnum	Command;

    int				    MagicID;		//�ҭn�I���k�N
	int					SerialID;		//�P�@���I�k���ʥ]�|���ۦP�����X
	int					SpellTime;		//�I�k�ɶ�( �d�����@�� )
    bool        		Result;			
	int					ShootAngle;		//�g��
	

    PG_Magic_LtoC_BeginSpellResult()
    {
        Command = EM_PG_Magic_LtoC_BeginSpellResult;
    };	
};
//------------------------------------------------------------------------------------------
//Server -> Client
//�]�k�����ʥ]
enum    MagicAtkResultENUM
{
    EM_MagicAtkResult_Miss			,		
    EM_MagicAtkResult_Normal		,
	EM_MagicAtkResult_Critial		,
	EM_MagicAtkResult_NoEffect		,		//�L��
	EM_MagicAtkResult_Absorb		,		//�l��
	EM_MagicAtkResult_NoEffect_Dead ,
	EM_MagicAtkResult_Cancel		,
	EM_MagicAtkResult_SpecialAction	,
	EM_MagicAtkResult_Parry			,
	EM_MagicAtkResult_ShieldBlock	,
	EM_MagicAtkResult_Dodge			,

};
struct  MagicAtkEffectInfoStruct
{
    int                     GItemID;
    MagicAtkResultENUM      Result;

	float					Value;		//�ܤƭ�
	float					OrgValue;	//��Ͽ�X
};

struct	PG_Magic_LtoC_MagicAttack
{
    GamePGCommandEnum	Command;

    int			    	MagicBaseID;
	MagicAttackTypeENUM	AtkType;
	int					SerialID;	//�P�@���I�k���ʥ]�|���ۦP�����X
    int                 Count;      //�ƶq

    MagicAtkEffectInfoStruct Atk[500];   

    PG_Magic_LtoC_MagicAttack( )
    {
        Command = EM_PG_Magic_LtoC_MagicAttack;
    }
    
    int Size()
    {
        return ( sizeof(*this) - sizeof( Atk ) + Count * sizeof( MagicAtkEffectInfoStruct ) );
    }
};

//------------------------------------------------------------------------------------------
//Server -> Client
//�U�k�N�����G
struct  MagicAssistEffectInfoStruct
{
    int                     GItemID;
    int                     EffectTime;
	int						MagicLv;
    MagicAtkResultENUM      Result;
	MagicAssistEffectInfoStruct()
	{
		GItemID			= 0;
		EffectTime		= 0;
		MagicLv			= 0;
		Result			= EM_MagicAtkResult_Normal;
	};
};
struct	PG_Magic_LtoC_MagicAssistResult
{
    GamePGCommandEnum	        Command;

    int			    	        MagicBaseID;
//	int							MagicLv;
	int							SerialID;		//�P�@���I�k���ʥ]�|���ۦP�����X
    int                         Count;
    MagicAssistEffectInfoStruct Atk[500];     //���v�T��ID List

    PG_Magic_LtoC_MagicAssistResult()
    {
        Command = EM_PG_Magic_LtoC_MagicAssistResult;
    }

    int Size()
    {
        return ( sizeof(*this) - sizeof( Atk ) + Count * sizeof( MagicAssistEffectInfoStruct ) );
    }
};

//------------------------------------------------------------------------------------------
//Server -> Client
//�ץ��Y�ӻ��U�k�N���Ī��ɶ�
struct	PG_Magic_LtoC_FixAssistMagicTime
{
    GamePGCommandEnum	Command;
    int		    		MagicID;
    int		    		EffectTime;
	int					BuffOwnerID;
    PG_Magic_LtoC_FixAssistMagicTime()
    {
        Command = EM_PG_Magic_LtoC_FixAssistMagicTime;
    }
};
//------------------------------------------------------------------------------------------
// Server -> Client 
//�����Y�H�I�k
struct	PG_Magic_LtoC_CancelSpellMagic
{
    GamePGCommandEnum	Command;

    int			    	OwnerID;
	int					SerialID;		//�P�@���I�k���ʥ]�|���ۦP�����X

    PG_Magic_LtoC_CancelSpellMagic()
    {
        Command = EM_PG_Magic_LtoC_CancelSpellMagic;
    }
};
//------------------------------------------------------------------------------------------
//�n�D�k�N�t�I
struct	PG_Magic_CtoL_SetMagicPoint
{
    GamePGCommandEnum	Command;
    int                 MagicID;
    int                 MagicPos;

    PG_Magic_CtoL_SetMagicPoint()
    {
        Command = EM_PG_Magic_CtoL_SetMagicPoint;
    }
};

//�t�I
struct	PG_Magic_CtoL_SetMagicPointEx
{
	GamePGCommandEnum	Command;
	int                 MagicID;
	int                 MagicPos;
	int					Times;		//�t�I����

	PG_Magic_CtoL_SetMagicPointEx()
	{
		Command = EM_PG_Magic_CtoL_SetMagicPointEx;
	}
};
//-----------------------------------------------------------------------------------------------------
enum SetMagicPointResultCodeENUM
{
    EM_MagicPointResult_OK                 ,
    EM_MagicPointResult_MagicPosTooLarge     ,
    EM_MagicPointResult_IDandPosError        ,
	EM_MagicPointResult_MaxLvError           ,
    EM_MagicPointResult_OtherError           ,
};

//�n�D�k�N�t�I���G
struct	PG_Magic_LtoC_SetMagicPointResult
{
    GamePGCommandEnum				Command;
    int								MagicID;
    int								MagicPos;
    SetMagicPointResultCodeENUM     Result;

    PG_Magic_LtoC_SetMagicPointResult()
    {
        Command = EM_PG_Magic_LtoC_SetMagicPointResult;
    }
};

//-----------------------------------------------------------------------------------------------------
//�W�[�@��Buff�q���g�����H
struct	PG_Magic_LtoC_AddBuffInfo
{
	GamePGCommandEnum	Command;
	int					GItemID;
	int					TargetGItemID;
	int                 MagicID;
	int					EffectTime;
	int					MagicLv;


	PG_Magic_LtoC_AddBuffInfo()
	{
		Command = EM_PG_Magic_LtoC_AddBuffInfo;
	}
};
//-----------------------------------------------------------------------------------------------------
//�ק�@��Buff�q���g�����H
struct	PG_Magic_LtoC_ModifyBuffInfo
{
	GamePGCommandEnum	Command;
	int					GItemID;
	int					TargetGItemID;
	int                 MagicID;
	int					EffectTime;
	int					MagicLv;


	PG_Magic_LtoC_ModifyBuffInfo()
	{
		Command = EM_PG_Magic_LtoC_ModifyBuffInfo;
	}
};


//-----------------------------------------------------------------------------------------------------
//�����@��Buff�q���g�����H
struct	PG_Magic_LtoC_DelBuffInfo
{
	GamePGCommandEnum	Command;
	int					GItemID;
	int                 MagicID;
	int					BuffOwnerID;

	PG_Magic_LtoC_DelBuffInfo()
	{
		Command = EM_PG_Magic_LtoC_DelBuffInfo;
	}
};
//-----------------------------------------------------------------------------------------------------
enum ResetColdownTypeENUM
{
	EM_ResetColdownType_All			,
	EM_ResetColdownType_NewAll			,
	EM_ResetColdownType_Job_Normal	,
	EM_ResetColdownType_Job			,
	EM_ResetColdownType_EQ			,
	EM_ResetColdownType_Item		,

};
//�M���Ҧ���Coldown
struct	PG_Magic_LtoC_ResetColdown
{
	GamePGCommandEnum	Command;
	ResetColdownTypeENUM	Type;

	PG_Magic_LtoC_ResetColdown()
	{
		Command = EM_PG_Magic_LtoC_ResetColdown;
	}
};
//------------------------------------------------------------------------------------------
// Client -> Server
//�����Y�ӻ��U�k�N
struct	PG_Magic_LtoC_CancelAssistMagic
{
	GamePGCommandEnum	Command;
	int					MagicID;		//���U�k�N��ƮwID

	PG_Magic_LtoC_CancelAssistMagic()
	{
		Command = EM_PG_Magic_LtoC_CancelAssistMagic;
	}
};
//-----------------------------------------------------------------------------------------------------
#define _DEF_MAX_MAGIC_TARGET_COUNT_	100
// Server -> Client
//�q���k�N�g�X
struct PG_Magic_LtoC_BeginShoot
{
	GamePGCommandEnum	Command;

	int					SerialID;		//�P�@���I�k���ʥ]�|���ۦP�����X
	int					TargetCount;
	int					TargetGUIDList[ _DEF_MAX_MAGIC_TARGET_COUNT_ ];

	PG_Magic_LtoC_BeginShoot()
	{
		Command = EM_PG_Magic_LtoC_BeginShoot;
	};	
};
//-----------------------------------------------------------------------------------------------------
// Server -> Client
//�q���k�N�g�X����
struct PG_Magic_LtoC_EndShoot
{
	GamePGCommandEnum	Command;

	int					SerialID;		//�P�@���I�k���ʥ]�|���ۦP�����X

	PG_Magic_LtoC_EndShoot()
	{
		Command = EM_PG_Magic_LtoC_EndShoot;
	};	
};
//-----------------------------------------------------------------------------------------------------
// Server -> Client 
// �}�l �t��Ĳ�o���B�~�k�N(�D���a�D�ʬI�k)
struct PG_Magic_LtoC_SysBeginShoot
{
	GamePGCommandEnum	Command;

	int			    	OwnerID;		//�I�k��
	int			    	TargetID;		//�I�k�ؼ�
	int			    	MagicColID;		//�ҭn�I���k�N

	int					SerialID;		//�P�@���I�k���ʥ]�|���ۦP�����X
	float				TX , TY , TZ;

	PG_Magic_LtoC_SysBeginShoot()
	{
		Command = EM_PG_Magic_LtoC_SysBeginShoot;
	};	
};
//-----------------------------------------------------------------------------------------------------
// Server -> Client 
// ���� �t��Ĳ�o���B�~�k�N(�D���a�D�ʬI�k)
struct PG_Magic_LtoC_SysEndShoot
{
	GamePGCommandEnum	Command;

	int					SerialID;		//�P�@���I�k���ʥ]�|���ۦP�����X

	PG_Magic_LtoC_SysEndShoot()
	{
		Command = EM_PG_Magic_LtoC_SysEndShoot;
	};	
};
//-----------------------------------------------------------------------------------------------------
// Server -> Client 
//�]�k�� �T����T
struct PG_Magic_LtoC_MagicShieldInfo
{
	GamePGCommandEnum	Command;

	int		AttackMagicID;			//����		�򥻪k�N
	int		ShieldMagicID;			//���w�y	�򥻪k�N
	int		AttackGUID;				//������
	int		UnAttackGUID;			//�Q������
	int		AbsobDamage;			//�l���ˮ`��

	PG_Magic_LtoC_MagicShieldInfo()
	{
		Command = EM_PG_Magic_LtoC_MagicShieldInfo;
	};	
};
//-----------------------------------------------------------------------------------------------------
// �q���]�kColdown��ơA�óq���k�N�����I�i����
// Server -> Client 
struct PG_Magic_LtoC_MagicEnd
{
	GamePGCommandEnum	Command;
	int		MagicID;
	int		Coldown_Normal;		//(��)
	int		Coldown_All;		//(��)

	PG_Magic_LtoC_MagicEnd()
	{
		Command = EM_PG_Magic_LtoC_MagicEnd;
	};	
};
//-----------------------------------------------------------------------------------------------------
//�d���B�z
//-----------------------------------------------------------------------------------------------------
// �q�� Client �ݲ����d��
// Server -> Client 
struct PG_Magic_LtoC_CreatePet
{
	GamePGCommandEnum	Command;
	int		PetGUID;			//�ϰ쪺ID
	int		PetWorldGUID;		//�@�ɬ��@��ID
	int		PetItemID;			//����ҪOID			
	int		MagicBaseID;

	SummonPetTypeENUM	PetType;

	PG_Magic_LtoC_CreatePet()
	{
		Command = EM_PG_Magic_LtoC_CreatePet;
	};	
};
//-----------------------------------------------------------------------------------------------------
//�q���P��,�Y���d���s����
// Server -> Client
struct PG_Magic_LtoC_CreatePet_Range
{
	GamePGCommandEnum	Command;
	int		PetGUID;			//�ϰ쪺ID

	PG_Magic_LtoC_CreatePet_Range()
	{
		Command = EM_PG_Magic_LtoC_CreatePet_Range;
	};	
};
//-----------------------------------------------------------------------------------------------------
// �q�� Client �ݧR���d��
// Server -> Client 
struct PG_Magic_LtoC_DeletePet
{
	GamePGCommandEnum	Command;
	int					PetGUID;			//�ϰ쪺ID
	SummonPetTypeENUM	PetType;

	PG_Magic_LtoC_DeletePet()
	{
		Command = EM_PG_Magic_LtoC_DeletePet;
	};	
};

//-----------------------------------------------------------------------------------------------------
// �R�O�d���欰
// Client -> Server
struct	PG_Magic_CtoL_PetCommand
{
	GamePGCommandEnum	Command;

	PET_ACTION_ENUM		Action;
	int					TargetID;		//�ؼ�
	int					MagicID;

	int					PetGUID;

	PG_Magic_CtoL_PetCommand()
	{
		Command = EM_PG_Magic_CtoL_PetCommand;
	};	
};
//-----------------------------------------------------------------------------------------------------
//�}���`�����k�N
// Client -> Server
struct	PG_Magic_CtoL_PetMagicSwitchRequest
{
	GamePGCommandEnum	Command;

	int		PetGUID;

	int		ActiveMagicID;			
	bool	Active;		


	PG_Magic_CtoL_PetMagicSwitchRequest()
	{
		Command = EM_PG_Magic_CtoL_PetMagicSwitchRequest;
	};	
};
//-----------------------------------------------------------------------------------------------------
//�}���`�����k�N
struct	PG_Magic_LtoC_PetMagicSwitchResult
{
	GamePGCommandEnum	Command;
	int					PetGUID;
	SummonPetTypeENUM	PetType;
	int					ActiveMagicID;			// -1 �N������
	bool				Active;		

	PG_Magic_LtoC_PetMagicSwitchResult()
	{
		Command = EM_PG_Magic_LtoC_PetMagicSwitchResult;
	};	
};
//-----------------------------------------------------------------------------------------------------
//�q��Client ���H��A�I�i�_���N
//Local -> Client
struct	PG_Magic_LtoC_RaiseMagic
{
	GamePGCommandEnum	Command;

	int					MagicBaseID;		//�_���k�NID
	int					SpellerID;			//�I�k��

	PG_Magic_LtoC_RaiseMagic()
	{
		Command = EM_PG_Magic_LtoC_RaiseMagic;
	};	
};
//-----------------------------------------------------------------------------------------------------
//�����I�k
//Client -> Local
struct	PG_Magic_CtoL_CancelMagicRequest
{
	GamePGCommandEnum	Command;

	PG_Magic_CtoL_CancelMagicRequest()
	{
		Command = EM_PG_Magic_CtoL_CancelMagicRequest;
	};	
};
//-----------------------------------------------------------------------------------------------------
//�n�D�I�iBuff�W���ޯ�
//Client -> Local
struct	PG_Magic_CtoL_BuffSkillRequest
{
	GamePGCommandEnum	Command;
	int		TargetID;
	float	PosX , PosY , PosZ;
	int		BuffID;
	int		BuffSkillID;

	PG_Magic_CtoL_BuffSkillRequest()
	{
		Command = EM_PG_Magic_CtoL_BuffSkillRequest;
	};	
};
//-----------------------------------------------------------------------------------------------------
//�n�D�I�iBuff�W���ޯ�
//Local -> Client
struct	PG_Magic_LtoC_BuffSkillResult
{
	GamePGCommandEnum	Command;
	bool	Result;

	PG_Magic_LtoC_BuffSkillResult()
	{
		Command = EM_PG_Magic_LtoC_BuffSkillResult;
	};	
};
//-----------------------------------------------------------------------------------------------------
//�q��Party�����k�NBuff���
//Local -> Client
struct MemberBuffInfoStruct
{
	int	BuffID;
	int	PowerLv;
	int	Time;	
};
struct	PG_Magic_LtoC_PartyMemberBuff
{
	GamePGCommandEnum	Command;
	int MemberDBID;
	int Count;
	MemberBuffInfoStruct Buff[_MAX_BUFF_COUNT_];

	PG_Magic_LtoC_PartyMemberBuff()
	{
		Command = EM_PG_Magic_LtoC_PartyMemberBuff;
	};

	int PGSize() 
	{
		if( Count > _MAX_BUFF_COUNT_ )
			Count = _MAX_BUFF_COUNT_;
		return sizeof(*this) - sizeof(Buff) + sizeof(MemberBuffInfoStruct)*Count;
	}
};
//-----------------------------------------------------------------------------------------------------
#pragma managed
