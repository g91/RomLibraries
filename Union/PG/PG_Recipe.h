#pragma once
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

#define DF_MAX_RECIPELIST	4096

//////////////////////////////////////////////////////////////////////////
// 鎖定所有配方單上的物品 並 要求合成
struct PG_Recipe_CtoL_LockResourceAndRefineRequest
{
	GamePGCommandEnum	Command;

	int		RecipeID;

	PG_Recipe_CtoL_LockResourceAndRefineRequest()
	{
		Command = EM_PG_Recipe_CtoL_LockResourceAndRefineRequest;
	};
};
//////////////////////////////////////////////////////////////////////////
//回應製造結果
enum RefineResultENUM
{
	EM_RefineResult_OK			,		//製造成 
	EM_RefineResult_Failed		,		//製造失敗
	EM_RefineResult_ResourceError,		//材料不足
	EM_RefineResult_RecipeError,		//沒配方單
	EM_RefineResult_SkillLvError,		//技能等級不足
	EM_RefineResult_ColdownError,		//冷卻中
	EM_RefineResult_NoMoney
};

enum EM_LearnRecipeResult
{
	EM_LearnRecipeResult_OK			= 0,	//學到了
	EM_LearnRecipeResult_Learned	,		//學過了
	EM_LearnRecipeResult_NoMoney	,		//錢不足
	EM_LearnRecipeResult_SkillTooLow,		//技能點數不夠
};

//物品資料

//失敗代表 消失的物品
struct PG_Recipe_LtoC_ProduceItemResult
{
	GamePGCommandEnum	Command;

	ItemFieldStruct		Item;

	PG_Recipe_LtoC_ProduceItemResult()
	{
		Command = EM_PG_Recipe_LtoC_ProduceItemResult;
	};
};

struct PG_Recipe_LtoC_LockResourceAndRefineResult
{
	GamePGCommandEnum	Command;

	int					RecipeID;
	RefineResultENUM	Result;

	PG_Recipe_LtoC_LockResourceAndRefineResult()
	{
		Command = EM_PG_Recipe_LtoC_LockResourceAndRefineResult;
	};
};
//-----------------------------------------------------------------
struct PG_Recipe_LtoC_RecipeList
{
	GamePGCommandEnum	Command;

	StaticFlag< DF_MAX_RECIPELIST >	RecipeList;

	PG_Recipe_LtoC_RecipeList()
	{
		Command = EM_PG_Recipe_LtoC_RecipeList;
	};
};
//-----------------------------------------------------------------
struct PG_Recipe_CtoL_LearnRecipe
{
	GamePGCommandEnum	Command;

	int					iRecipeID;

	PG_Recipe_CtoL_LearnRecipe()
	{
		Command = EM_PG_Recipe_CtoL_LearnRecipe;
	};
};
//-----------------------------------------------------------------
struct PG_Recipe_LtoC_LearnRecipeResult
{
	GamePGCommandEnum	Command;

	int					iRecipeID;
	int					iResult;

	PG_Recipe_LtoC_LearnRecipeResult()
	{
		Command = EM_PG_Recipe_LtoC_LearnRecipeResult;
	};
};
//-----------------------------------------------------------------
struct PG_Recipe_LtoC_RefineCooldown
{
	GamePGCommandEnum	Command;

	RefineBuffStruct	Cooldown;

	PG_Recipe_LtoC_RefineCooldown()
	{
		Command = EM_PG_Recipe_LtoC_RefineCooldown;
	};
};
//-----------------------------------------------------------------

/*
struct PG_Recipe_LtoC_StartRecipeList
{
	GamePGCommandEnum	Command;

	int					iSkillType;

	PG_Recipe_LtoC_StartRecipeList()
	{
		Command = EM_PG_Recipe_LtoC_StartRecipeList;
	};
};
//-----------------------------------------------------------------
struct PG_Recipe_LtoC_RecipeID
{
	GamePGCommandEnum	Command;

	int					iRecipeID;

	PG_Recipe_LtoC_StartRecipeList()
	{
		Command = EM_PG_Recipe_LtoC_RecipeID;
	};
};
//-----------------------------------------------------------------
struct PG_Recipe_LtoC_EndRecipeList
{
	GamePGCommandEnum	Command;

	PG_Recipe_LtoC_StartRecipeList()
	{
		Command = EM_PG_Recipe_LtoC_EndRecipeList;
	};
};
*/
//////////////////////////////////////////////////////////////////////////

#pragma managed
