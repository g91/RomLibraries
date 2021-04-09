#pragma once
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged
//////////////////////////////////////////////////////////////////////////
//��J�d�����~(�쥻���d���|�R��)
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
//�d�����~��J���G
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
//�άY���~���d���٭즨���~
//Client -> Local
struct PG_CultivatePet_CtoL_PopPet
{
	GamePGCommandEnum	Command;
	int					PetPos;
	int					ItemPos;			//-1 �S���]�w��ܧR���d��

	PG_CultivatePet_CtoL_PopPet()
	{
		Command = EM_PG_CultivatePet_CtoL_PopPet;
	}
};
//////////////////////////////////////////////////////////////////////////
//�d�����~���X���G
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
//�]�w�d���W�r
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
//�]�w�d���W�r���G
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
//�R�O�d���ƥ�
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
//�R�O�d���ƥ� �}�l�B�z
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
//�R�O�d���ƥ󵲪G
//Local -> Client 
struct PG_CultivatePet_LtoC_PetEventResult
{
	GamePGCommandEnum	Command;
	bool							Result;
	int								PetPos;
	CultivatePetCommandTypeENUM		OrgEvent;
	CultivatePetCommandTypeENUM		Event;
	int								EventTime;		//�ƥ�һݮɶ�

	PG_CultivatePet_LtoC_PetEventResult()
	{
		Command = EM_PG_CultivatePet_LtoC_PetEventResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�ǲ߷s�ޯ�
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
//�n�D�ǲ߷s�ޯ�
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
//�d���ͬ���
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
//�d���ͬ��޵��G
//Local -> Client 
struct PG_CultivatePet_LtoC_PetLifeResult
{
	GamePGCommandEnum	Command;
	CultivatePetLifeSkillTypeENUM	SkillType;
	int					EventTime;		//�ƥ�һݮɶ�
	int					PetPos;
	bool				Result;


	PG_CultivatePet_LtoC_PetLifeResult()
	{
		Command = EM_PG_CultivatePet_LtoC_PetLifeResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//�񪫫~���d�����
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
//�񪫫~���d�����
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
//�ץ��d�����~���
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
//�ץ��d�����~���
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
//�n�D���o�Ͳ��᪺���~
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
//�n�D���o�Ͳ��᪺���~
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
//�n�D�d���ĦX
struct PG_CultivatePet_CtoL_PetMarge
{
	GamePGCommandEnum	Command;
	int					PetCount;
	int					PetPos[ 3 ];	//0 �D�� 1,2 �ĦX���d��

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
	EM_PetMargeResult_MergeCountErr	,	//�ĦX���ƿ��~
	EM_PetMargeResult_MergeTicketErr,	//�ĦX�����~
	EM_PetMargeResult_EventTypeErr	,	//�d�� �ƥ���~
};

//Client -> Local
//�ĦX���G
struct PG_CultivatePet_LtoC_PetMargeResult
{
	GamePGCommandEnum	Command;
	int					PetCount;
	int					PetPos[ 3 ];	//0 �D�� 1,2 �ĦX���d��
	CultivatePetStruct	PetBase;
	PetMargeResultENUM	Result;

	PG_CultivatePet_LtoC_PetMargeResult()
	{
		Command = EM_PG_CultivatePet_LtoC_PetMargeResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//�n�J�d���d��
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
//�n�J�d���d��
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
