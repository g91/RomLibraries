#pragma     once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

//------------------------------------------------------------------------------------------
//Client -> Server
//準備施法
struct PG_Magic_CtoL_BeginSpell
{
    GamePGCommandEnum	Command;

    int                 OwnerID;
    int				    MagicID;		//所要施的法術
    int				    TargetID;		//目標

    int			    	MagicPos;		//法術資料位置

    PG_Magic_CtoL_BeginSpell()
    {
        Command = EM_PG_Magic_CtoL_BeginSpell;
    };
};
//------------------------------------------------------------------------------------------
//Server -> Client
//準備施法(通知週圍人)
struct PG_Magic_LtoC_BeginSpell
{
    GamePGCommandEnum	Command;

    int			    	OwnerID;		//施法者
    int			    	TargetID;		//施法目標
    int			    	Magic;			//所要施的法術
	float				TargetX;
	float				TargetY;
	float				TargetZ;

	int					SerialID;		//同一次施法的封包會有相同的號碼
	int					SpellTime;

    PG_Magic_LtoC_BeginSpell()
    {
        Command = EM_PG_Magic_LtoC_BeginSpell;
    };	
};
//------------------------------------------------------------------------------------------
//Client -> Server
//準備施法 指定位置
struct PG_Magic_CtoL_BeginSpell_Pos
{
	GamePGCommandEnum	Command;

	int                 OwnerID;
	int				    MagicID;		//所要施的法術
	int			    	MagicPos;		//法術資料位置
	float				X , Y , Z;		//位置

	PG_Magic_CtoL_BeginSpell_Pos()
	{
		Command = EM_PG_Magic_CtoL_BeginSpell_Pos;
	};
};
//------------------------------------------------------------------------------------------
//Server -> Client
//施法結束(通知週圍人)
struct PG_Magic_LtoC_EndSpell
{
    GamePGCommandEnum	Command;

	int					SerialID;		//同一次施法的封包會有相同的號碼

    PG_Magic_LtoC_EndSpell()
    {
        Command = EM_PG_Magic_LtoC_EndSpell;
    };	
};

//------------------------------------------------------------------------------------------
//Server -> Client
//準備施法結果
struct PG_Magic_LtoC_BeginSpellResult
{
    GamePGCommandEnum	Command;

    int				    MagicID;		//所要施的法術
	int					SerialID;		//同一次施法的封包會有相同的號碼
	int					SpellTime;		//施法時間( 千分之一秒 )
    bool        		Result;			
	int					ShootAngle;		//射角
	

    PG_Magic_LtoC_BeginSpellResult()
    {
        Command = EM_PG_Magic_LtoC_BeginSpellResult;
    };	
};
//------------------------------------------------------------------------------------------
//Server -> Client
//魔法攻擊封包
enum    MagicAtkResultENUM
{
    EM_MagicAtkResult_Miss			,		
    EM_MagicAtkResult_Normal		,
	EM_MagicAtkResult_Critial		,
	EM_MagicAtkResult_NoEffect		,		//無效
	EM_MagicAtkResult_Absorb		,		//吸收
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

	float					Value;		//變化值
	float					OrgValue;	//初使輸出
};

struct	PG_Magic_LtoC_MagicAttack
{
    GamePGCommandEnum	Command;

    int			    	MagicBaseID;
	MagicAttackTypeENUM	AtkType;
	int					SerialID;	//同一次施法的封包會有相同的號碼
    int                 Count;      //數量

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
//助法術的結果
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
	int							SerialID;		//同一次施法的封包會有相同的號碼
    int                         Count;
    MagicAssistEffectInfoStruct Atk[500];     //受影響的ID List

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
//修正某個輔助法術有效的時間
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
//取消某人施法
struct	PG_Magic_LtoC_CancelSpellMagic
{
    GamePGCommandEnum	Command;

    int			    	OwnerID;
	int					SerialID;		//同一次施法的封包會有相同的號碼

    PG_Magic_LtoC_CancelSpellMagic()
    {
        Command = EM_PG_Magic_LtoC_CancelSpellMagic;
    }
};
//------------------------------------------------------------------------------------------
//要求法術配點
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

//配點
struct	PG_Magic_CtoL_SetMagicPointEx
{
	GamePGCommandEnum	Command;
	int                 MagicID;
	int                 MagicPos;
	int					Times;		//配點次數

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

//要求法術配點結果
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
//增加一個Buff通知週為的人
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
//修改一個Buff通知週為的人
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
//移除一個Buff通知週為的人
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
//清除所有的Coldown
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
//取消某個輔助法術
struct	PG_Magic_LtoC_CancelAssistMagic
{
	GamePGCommandEnum	Command;
	int					MagicID;		//輔助法術資料庫ID

	PG_Magic_LtoC_CancelAssistMagic()
	{
		Command = EM_PG_Magic_LtoC_CancelAssistMagic;
	}
};
//-----------------------------------------------------------------------------------------------------
#define _DEF_MAX_MAGIC_TARGET_COUNT_	100
// Server -> Client
//通知法術射出
struct PG_Magic_LtoC_BeginShoot
{
	GamePGCommandEnum	Command;

	int					SerialID;		//同一次施法的封包會有相同的號碼
	int					TargetCount;
	int					TargetGUIDList[ _DEF_MAX_MAGIC_TARGET_COUNT_ ];

	PG_Magic_LtoC_BeginShoot()
	{
		Command = EM_PG_Magic_LtoC_BeginShoot;
	};	
};
//-----------------------------------------------------------------------------------------------------
// Server -> Client
//通知法術射出結束
struct PG_Magic_LtoC_EndShoot
{
	GamePGCommandEnum	Command;

	int					SerialID;		//同一次施法的封包會有相同的號碼

	PG_Magic_LtoC_EndShoot()
	{
		Command = EM_PG_Magic_LtoC_EndShoot;
	};	
};
//-----------------------------------------------------------------------------------------------------
// Server -> Client 
// 開始 系統觸發的額外法術(非玩家主動施法)
struct PG_Magic_LtoC_SysBeginShoot
{
	GamePGCommandEnum	Command;

	int			    	OwnerID;		//施法者
	int			    	TargetID;		//施法目標
	int			    	MagicColID;		//所要施的法術

	int					SerialID;		//同一次施法的封包會有相同的號碼
	float				TX , TY , TZ;

	PG_Magic_LtoC_SysBeginShoot()
	{
		Command = EM_PG_Magic_LtoC_SysBeginShoot;
	};	
};
//-----------------------------------------------------------------------------------------------------
// Server -> Client 
// 結束 系統觸發的額外法術(非玩家主動施法)
struct PG_Magic_LtoC_SysEndShoot
{
	GamePGCommandEnum	Command;

	int					SerialID;		//同一次施法的封包會有相同的號碼

	PG_Magic_LtoC_SysEndShoot()
	{
		Command = EM_PG_Magic_LtoC_SysEndShoot;
	};	
};
//-----------------------------------------------------------------------------------------------------
// Server -> Client 
//魔法盾 訊息資訊
struct PG_Magic_LtoC_MagicShieldInfo
{
	GamePGCommandEnum	Command;

	int		AttackMagicID;			//攻擊		基本法術
	int		ShieldMagicID;			//防預頓	基本法術
	int		AttackGUID;				//攻擊者
	int		UnAttackGUID;			//被攻擊者
	int		AbsobDamage;			//吸收傷害值

	PG_Magic_LtoC_MagicShieldInfo()
	{
		Command = EM_PG_Magic_LtoC_MagicShieldInfo;
	};	
};
//-----------------------------------------------------------------------------------------------------
// 通知魔法Coldown資料，並通知法術完全施展完成
// Server -> Client 
struct PG_Magic_LtoC_MagicEnd
{
	GamePGCommandEnum	Command;
	int		MagicID;
	int		Coldown_Normal;		//(秒)
	int		Coldown_All;		//(秒)

	PG_Magic_LtoC_MagicEnd()
	{
		Command = EM_PG_Magic_LtoC_MagicEnd;
	};	
};
//-----------------------------------------------------------------------------------------------------
//寵物處理
//-----------------------------------------------------------------------------------------------------
// 通知 Client 端產生寵物
// Server -> Client 
struct PG_Magic_LtoC_CreatePet
{
	GamePGCommandEnum	Command;
	int		PetGUID;			//區域的ID
	int		PetWorldGUID;		//世界為一的ID
	int		PetItemID;			//物件模板ID			
	int		MagicBaseID;

	SummonPetTypeENUM	PetType;

	PG_Magic_LtoC_CreatePet()
	{
		Command = EM_PG_Magic_LtoC_CreatePet;
	};	
};
//-----------------------------------------------------------------------------------------------------
//通知周圍,某個寵物新產生
// Server -> Client
struct PG_Magic_LtoC_CreatePet_Range
{
	GamePGCommandEnum	Command;
	int		PetGUID;			//區域的ID

	PG_Magic_LtoC_CreatePet_Range()
	{
		Command = EM_PG_Magic_LtoC_CreatePet_Range;
	};	
};
//-----------------------------------------------------------------------------------------------------
// 通知 Client 端刪除寵物
// Server -> Client 
struct PG_Magic_LtoC_DeletePet
{
	GamePGCommandEnum	Command;
	int					PetGUID;			//區域的ID
	SummonPetTypeENUM	PetType;

	PG_Magic_LtoC_DeletePet()
	{
		Command = EM_PG_Magic_LtoC_DeletePet;
	};	
};

//-----------------------------------------------------------------------------------------------------
// 命令寵物行為
// Client -> Server
struct	PG_Magic_CtoL_PetCommand
{
	GamePGCommandEnum	Command;

	PET_ACTION_ENUM		Action;
	int					TargetID;		//目標
	int					MagicID;

	int					PetGUID;

	PG_Magic_CtoL_PetCommand()
	{
		Command = EM_PG_Magic_CtoL_PetCommand;
	};	
};
//-----------------------------------------------------------------------------------------------------
//開關循環的法術
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
//開關循環的法術
struct	PG_Magic_LtoC_PetMagicSwitchResult
{
	GamePGCommandEnum	Command;
	int					PetGUID;
	SummonPetTypeENUM	PetType;
	int					ActiveMagicID;			// -1 代表關閉
	bool				Active;		

	PG_Magic_LtoC_PetMagicSwitchResult()
	{
		Command = EM_PG_Magic_LtoC_PetMagicSwitchResult;
	};	
};
//-----------------------------------------------------------------------------------------------------
//通知Client 有人對你施展復活術
//Local -> Client
struct	PG_Magic_LtoC_RaiseMagic
{
	GamePGCommandEnum	Command;

	int					MagicBaseID;		//復活法術ID
	int					SpellerID;			//施法者

	PG_Magic_LtoC_RaiseMagic()
	{
		Command = EM_PG_Magic_LtoC_RaiseMagic;
	};	
};
//-----------------------------------------------------------------------------------------------------
//取消施法
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
//要求施展Buff上的技能
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
//要求施展Buff上的技能
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
//通知Party成員法術Buff資料
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
