#pragma once
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

#define DF_MAX_RECIPELIST	4096

//////////////////////////////////////////////////////////////////////////
// ��w�Ҧ��t���W�����~ �� �n�D�X��
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
//�^���s�y���G
enum RefineResultENUM
{
	EM_RefineResult_OK			,		//�s�y�� 
	EM_RefineResult_Failed		,		//�s�y����
	EM_RefineResult_ResourceError,		//���Ƥ���
	EM_RefineResult_RecipeError,		//�S�t���
	EM_RefineResult_SkillLvError,		//�ޯ൥�Ť���
	EM_RefineResult_ColdownError,		//�N�o��
	EM_RefineResult_NoMoney
};

enum EM_LearnRecipeResult
{
	EM_LearnRecipeResult_OK			= 0,	//�Ǩ�F
	EM_LearnRecipeResult_Learned	,		//�ǹL�F
	EM_LearnRecipeResult_NoMoney	,		//������
	EM_LearnRecipeResult_SkillTooLow,		//�ޯ��I�Ƥ���
};

//���~���

//���ѥN�� ���������~
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
