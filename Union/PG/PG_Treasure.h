#pragma once
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

//////////////////////////////////////////////////////////////////////////
//Local -> Client �q�����a���_�c�O�_�i�H�}
struct PG_Treasure_LtoC_BaseInfo
{
	GamePGCommandEnum	Command;
	int					ItemGUID;
	bool				IsEmpty;

	PG_Treasure_LtoC_BaseInfo()
	{
		Command = EM_PG_Treasure_LtoC_BaseInfo;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local �}�_�c
struct PG_Treasure_CtoL_OpenRequest
{
	GamePGCommandEnum	Command;
	int					ItemGUID;

	PG_Treasure_CtoL_OpenRequest()
	{
		Command = EM_PG_Treasure_CtoL_OpenRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local �}�_�c
struct PG_Treasure_LtoC_OpenFailed
{
	GamePGCommandEnum	Command;
	int					ItemGUID;

	PG_Treasure_LtoC_OpenFailed()
	{
		Command = EM_PG_Treasure_LtoC_OpenFailed;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client �}���_�c&��T
struct PG_Treasure_LtoC_Open
{
	GamePGCommandEnum	Command;
	int					ItemGUID;
	int					MaxCount;
	ItemFieldStruct		Items[30];

	int	Size() 
	{
		return sizeof( int )* 3 + MaxCount * sizeof( ItemFieldStruct );
	}
	PG_Treasure_LtoC_Open()
	{
		Command = EM_PG_Treasure_LtoC_Open;
	}
};

//////////////////////////////////////////////////////////////////////////
//Client -> Local �����~
struct PG_Treasure_CtoL_GetItem
{
	GamePGCommandEnum	Command;
	int					ItemGUID;		//�_�cID
	int					ItemSerial;
	int					BodyPos;		//�n�񪺦�m

	PG_Treasure_CtoL_GetItem()
	{
		Command = EM_PG_Treasure_CtoL_GetItem;
	}
};
//////////////////////////////////////////////////////////////////////////
enum TreasureGetItemResultENUM
{
	EM_TreasureGetItemResult_Faild	,
	EM_TreasureGetItemResult_OK		,
	EM_TreasureGetItemResult_PartyQueue,
	EM_TreasureGetItemResult_ItemUnique,		
	EM_TreasureGetItemResult_MaxCountErr,	//�B���W�L�̤j�ƶq
};
//Local -> Client �����~���G
struct PG_Treasure_LtoC_GetItemResult
{
	GamePGCommandEnum	Command;
	int					ItemSerial;
	int					BodyPos;		//�n�񪺦�m
	TreasureGetItemResultENUM		Result;

	PG_Treasure_LtoC_GetItemResult()
	{
		Command = EM_PG_Treasure_LtoC_GetItemResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local ��������
struct PG_Treasure_CtoL_Close
{
	GamePGCommandEnum	Command;
	int					ItemGUID;

	PG_Treasure_CtoL_Close()
	{
		Command = EM_PG_Treasure_CtoL_Close;
	}
};

//////////////////////////////////////////////////////////////////////////
#pragma managed
