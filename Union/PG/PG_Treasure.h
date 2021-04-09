#pragma once
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

//////////////////////////////////////////////////////////////////////////
//Local -> Client 通知玩家此寶箱是否可以開
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
//Client -> Local 開寶箱
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
//Client -> Local 開寶箱
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
//Local -> Client 開啟寶箱&資訊
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
//Client -> Local 取物品
struct PG_Treasure_CtoL_GetItem
{
	GamePGCommandEnum	Command;
	int					ItemGUID;		//寶箱ID
	int					ItemSerial;
	int					BodyPos;		//要放的位置

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
	EM_TreasureGetItemResult_MaxCountErr,	//拾取超過最大數量
};
//Local -> Client 取物品結果
struct PG_Treasure_LtoC_GetItemResult
{
	GamePGCommandEnum	Command;
	int					ItemSerial;
	int					BodyPos;		//要放的位置
	TreasureGetItemResultENUM		Result;

	PG_Treasure_LtoC_GetItemResult()
	{
		Command = EM_PG_Treasure_LtoC_GetItemResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 關閉介面
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
