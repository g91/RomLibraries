#pragma once
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged
//////////////////////////////////////////////////////////////////////////
//放入寵物物品(原本的寵物會刪除)
//Client -> Local
struct PG_CultivatePet_CtoL_PushPet
{
	GamePGCommandEnum	Command;
	int					PetPos;
	int					ItemPos;

	PG_CultivatePet_CtoL_PushPet()
	{
		Command = EM_PG_CultivatePet_CtoL_PushPet;
	}
};
//////////////////////////////////////////////////////////////////////////
//寵物物品放入結果
//Local -> Client 
struct PG_CultivatePet_LtoC_PushPetResult
{
	GamePGCommandEnum	Command;
	int					PetPos;
	CultivatePetStruct	PetInfo;
	bool				Result;

	PG_CultivatePet_LtoC_PushPetResult()
	{
		Command = EM_PG_CultivatePet_LtoC_PushPetResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//用某物品把寵物還原成物品
//Client -> Local
struct PG_CultivatePet_CtoL_PopPet
{
	GamePGCommandEnum	Command;
	int					PetPos;
	int					ItemPos;			//-1 沒有設定表示刪除寵物

	PG_CultivatePet_CtoL_PopPet()
	{
		Command = EM_PG_CultivatePet_CtoL_PopPet;
	}
};
//////////////////////////////////////////////////////////////////////////
//寵物物品取出結果
//Local -> Client 
struct PG_CultivatePet_LtoC_PopPetResult
{
	GamePGCommandEnum	Command;
	int					ItemPos;
	bool				Result;

	PG_CultivatePet_LtoC_PopPetResult()
	{
		Command = EM_PG_CultivatePet_LtoC_PopPetResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//設定寵物名字
//Client -> Local
struct PG_CultivatePet_CtoL_PetName
{
	GamePGCommandEnum	Command;
	int					PetPos;
	StaticString< _MAX_NAMERECORD_SIZE_ >   Name;

	PG_CultivatePet_CtoL_PetName()
	{
		Command = EM_PG_CultivatePet_CtoL_PetName;
	}
};
//////////////////////////////////////////////////////////////////////////
//設定寵物名字結果
//Local -> Client 
struct PG_CultivatePet_LtoC_PetNameResult
{
	GamePGCommandEnum	Command;
	StaticString< _MAX_NAMERECORD_SIZE_ >   Name;
	bool				Result;

	PG_CultivatePet_LtoC_PetNameResult()
	{
		Command = EM_PG_CultivatePet_LtoC_PetNameResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//命令寵物事件
//Client -> Local
struct PG_CultivatePet_CtoL_PetEvent
{
	GamePGCommandEnum	Command;
	int								PetPos;
	int								ItemPos;
	CultivatePetCommandTypeENUM		Event;


	PG_CultivatePet_CtoL_PetEvent()
	{
		Command = EM_PG_CultivatePet_CtoL_PetEvent;
	}
};
//////////////////////////////////////////////////////////////////////////
//命令寵物事件 開始處理
//Local -> Client 
struct PG_CultivatePet_LtoC_PetEventProcess
{
	GamePGCommandEnum	Command;
	int								PetPos;
	CultivatePetCommandTypeENUM		Event;

	PG_CultivatePet_LtoC_PetEventProcess()
	{
		Command = EM_PG_CultivatePet_LtoC_PetEventProcess;
	}
};
//////////////////////////////////////////////////////////////////////////
//命令寵物事件結果
//Local -> Client 
struct PG_CultivatePet_LtoC_PetEventResult
{
	GamePGCommandEnum	Command;
	bool							Result;
	int								PetPos;
	CultivatePetCommandTypeENUM		OrgEvent;
	CultivatePetCommandTypeENUM		Event;
	int								EventTime;		//事件所需時間

	PG_CultivatePet_LtoC_PetEventResult()
	{
		Command = EM_PG_CultivatePet_LtoC_PetEventResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//要求學習新技能
//Client -> Local
struct PG_CultivatePet_CtoL_LearnSkill
{
	GamePGCommandEnum	Command;
	int			PetPos;		
	int			MagicID;
	int			MagicLv;

	PG_CultivatePet_CtoL_LearnSkill()
	{
		Command = EM_PG_CultivatePet_CtoL_LearnSkill;
	}
};
//////////////////////////////////////////////////////////////////////////
//要求學習新技能
//Local -> Client
struct PG_CultivatePet_LtoC_LearnSkillResult
{
	GamePGCommandEnum	Command;
	int			PetPos;		
	int			MagicID;
	int			MagicLv;
	bool		Result;

	PG_CultivatePet_LtoC_LearnSkillResult()
	{
		Command = EM_PG_CultivatePet_LtoC_LearnSkillResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//寵物生活技
//Client -> Local
struct PG_CultivatePet_CtoL_PetLifeSkill
{
	GamePGCommandEnum	Command;
	int					PetPos;
	CultivatePetLifeSkillTypeENUM	SkillType;
	int					Lv;
	int					TablePos;

	PG_CultivatePet_CtoL_PetLifeSkill()
	{
		Command = EM_PG_CultivatePet_CtoL_PetLifeSkill;
	}
};
//////////////////////////////////////////////////////////////////////////
//寵物生活技結果
//Local -> Client 
struct PG_CultivatePet_LtoC_PetLifeResult
{
	GamePGCommandEnum	Command;
	CultivatePetLifeSkillTypeENUM	SkillType;
	int					EventTime;		//事件所需時間
	int					PetPos;
	bool				Result;


	PG_CultivatePet_LtoC_PetLifeResult()
	{
		Command = EM_PG_CultivatePet_LtoC_PetLifeResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//放物品到寵物欄位
//Client -> Local
struct PG_CultivatePet_CtoL_SwapItem
{
	GamePGCommandEnum	Command;
	int						PetPos;
	int						BodyPos;
	CultivatePetEQTypeENUM	PetItemPos;
	bool					IsFromBody;

	PG_CultivatePet_CtoL_SwapItem()
	{
		Command = EM_PG_CultivatePet_CtoL_SwapItem;
	}
};
//////////////////////////////////////////////////////////////////////////
//放物品到寵物欄位
//Local -> Client
struct PG_CultivatePet_LtoC_SwapItemResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_CultivatePet_LtoC_SwapItemResult()
	{
		Command = EM_PG_CultivatePet_LtoC_SwapItemResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//修正寵物物品資料
//Local -> Client 
struct PG_CultivatePet_LtoC_FixItem
{
	GamePGCommandEnum	Command;
	int					PetPos;
	int					PetItemPos;
	ItemFieldStruct		Item;


	PG_CultivatePet_LtoC_FixItem()
	{
		Command = EM_PG_CultivatePet_LtoC_FixItem;
	}
};
//////////////////////////////////////////////////////////////////////////
//修正寵物物品資料
//Local -> Client 
struct PG_CultivatePet_LtoC_FixPetBase
{
	GamePGCommandEnum	Command;
	int					PetPos;
	CultivatePetStruct	PetBase;

	PG_CultivatePet_LtoC_FixPetBase()
	{
		Command = EM_PG_CultivatePet_LtoC_FixPetBase;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//要求取得生產後的物品
struct PG_CultivatePet_CtoL_GetProductRequest
{
	GamePGCommandEnum	Command;
	int					PetPos;	

	PG_CultivatePet_CtoL_GetProductRequest()
	{
		Command = EM_PG_CultivatePet_CtoL_GetProductRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client
//要求取得生產後的物品
struct PG_CultivatePet_LtoC_GetProductResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_CultivatePet_LtoC_GetProductResult()
	{
		Command = EM_PG_CultivatePet_LtoC_GetProductResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client
//要求寵物融合
struct PG_CultivatePet_CtoL_PetMarge
{
	GamePGCommandEnum	Command;
	int					PetCount;
	int					PetPos[ 3 ];	//0 主體 1,2 融合的寵物

	PG_CultivatePet_CtoL_PetMarge()
	{
		Command = EM_PG_CultivatePet_CtoL_PetMarge;
	}
};
//////////////////////////////////////////////////////////////////////////
enum PetMargeResultENUM
{
	EM_PetMargeResult_OK			,
	EM_PetMargeResult_DataErr		,
	EM_PetMargeResult_LevelErr		,
	EM_PetMargeResult_MergeCountErr	,	//融合次數錯誤
	EM_PetMargeResult_MergeTicketErr,	//融合卷錯誤
	EM_PetMargeResult_EventTypeErr	,	//寵物 事件錯誤
};

//Client -> Local
//融合結果
struct PG_CultivatePet_LtoC_PetMargeResult
{
	GamePGCommandEnum	Command;
	int					PetCount;
	int					PetPos[ 3 ];	//0 主體 1,2 融合的寵物
	CultivatePetStruct	PetBase;
	PetMargeResultENUM	Result;

	PG_CultivatePet_LtoC_PetMargeResult()
	{
		Command = EM_PG_CultivatePet_LtoC_PetMargeResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//登入寵物卡片
struct PG_CultivatePet_CtoL_RegPetCard
{
	GamePGCommandEnum	Command;
	int					PetPos;

	PG_CultivatePet_CtoL_RegPetCard()
	{
		Command = EM_PG_CultivatePet_CtoL_RegPetCard;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//登入寵物卡片
struct PG_CultivatePet_LtoC_RegPetCardResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_CultivatePet_LtoC_RegPetCardResult()
	{
		Command = EM_PG_CultivatePet_LtoC_RegPetCardResult;
	}
};
//////////////////////////////////////////////////////////////////////////
#pragma managed
