#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged
//---------------------------------------------------------------------------
// Client -> Server
//新增傳送位置要求
struct PG_Teleport_CtoL_AddTeleportRequest
{
	GamePGCommandEnum	Command;
	int		ItemPos;			//身体物品位置
	int		TeleportPos;		//儲存位置
	LocationSaveStruct Info;		

	PG_Teleport_CtoL_AddTeleportRequest()
	{
		Command = EM_PG_Teleport_CtoL_AddTeleportRequest;
	}
};
//---------------------------------------------------------------------------
// Server -> Client
//新增傳送位置結果
struct PG_Teleport_LtoC_AddTeleportResult
{
	GamePGCommandEnum	Command;	
	int					TeleportPos;		//儲存位置
	LocationSaveStruct	Info;	
	bool				Result;

	PG_Teleport_LtoC_AddTeleportResult()
	{
		Command = EM_PG_Teleport_LtoC_AddTeleportResult;
	}
};
//---------------------------------------------------------------------------
// Client -> Server
//移除傳送位置要求
struct PG_Teleport_CtoL_DelTeleportRequest
{
	GamePGCommandEnum	Command;
	int		TeleportPos;		//儲存位置

	PG_Teleport_CtoL_DelTeleportRequest()
	{
		Command = EM_PG_Teleport_CtoL_DelTeleportRequest;
	}
};
//---------------------------------------------------------------------------
// Server -> Client
//移除傳送位置結果
struct PG_Teleport_LtoC_DelTeleportResult
{
	GamePGCommandEnum	Command;	
	int					TeleportPos;		//儲存位置
	bool				Result;

	PG_Teleport_LtoC_DelTeleportResult()
	{
		Command = EM_PG_Teleport_LtoC_DelTeleportResult;
	}
};
//---------------------------------------------------------------------------
// Client -> Server
//交換傳送位置要求
struct PG_Teleport_CtoL_SwapTeleportRequest
{
	GamePGCommandEnum	Command;
	int		TeleportPos[2];		//儲存位置

	PG_Teleport_CtoL_SwapTeleportRequest()
	{
		Command = EM_PG_Teleport_CtoL_SwapTeleportRequest;
	}
};
//---------------------------------------------------------------------------
// Server -> Client
//交換傳送位置結果
struct PG_Teleport_LtoC_SwapTeleportResult
{
	GamePGCommandEnum	Command;	
	int					TeleportPos[2];		//儲存位置
	bool				Result;

	PG_Teleport_LtoC_SwapTeleportResult()
	{
		Command = EM_PG_Teleport_LtoC_SwapTeleportResult;
	}
};
//---------------------------------------------------------------------------
// Client -> Server
//修改傳送位置要求
struct PG_Teleport_CtoL_ModifyTeleportRequest
{
	GamePGCommandEnum	Command;
	int		TeleportPos;		//儲存位置
	LocationSaveStruct Info;		

	PG_Teleport_CtoL_ModifyTeleportRequest()
	{
		Command = EM_PG_Teleport_CtoL_ModifyTeleportRequest;
	}
};
//---------------------------------------------------------------------------
// Server -> Client
//修改傳送位置結果
struct PG_Teleport_LtoC_ModifyTeleportResult
{
	GamePGCommandEnum	Command;	
	int					TeleportPos;		//儲存位置
	LocationSaveStruct	Info;	
	bool				Result;

	PG_Teleport_LtoC_ModifyTeleportResult()
	{
		Command = EM_PG_Teleport_LtoC_ModifyTeleportResult;
	}
};
//---------------------------------------------------------------------------
enum UseTeleportTypeENUM
{
	EM_UseTeleportType_Normal		,	//一般傳送
	EM_UseTeleportType_OneWayHole	,	//單向傳送
	EM_UseTeleportType_TwoWayHole	,	//雙向傳送
};
// Client -> Server
//使用傳送位置要求
struct PG_Teleport_CtoL_UseTeleportRequest
{
	GamePGCommandEnum	Command;
	int		ItemPos;			//身体物品位置
	int		TeleportPos;		//儲存位置
	UseTeleportTypeENUM		Type;
	
	PG_Teleport_CtoL_UseTeleportRequest()
	{
		Command = EM_PG_Teleport_CtoL_UseTeleportRequest;
	}
};
//---------------------------------------------------------------------------
// Server -> Client
//使用傳送位置結果
struct PG_Teleport_LtoC_UseTeleportResult
{
	GamePGCommandEnum	Command;
	int		ItemPos;			//身体物品位置
	int		TeleportPos;		//儲存位置
	UseTeleportTypeENUM		Type;
	bool	Result;

	PG_Teleport_LtoC_UseTeleportResult()
	{
		Command = EM_PG_Teleport_LtoC_UseTeleportResult;
	}
};
//---------------------------------------------------------------------------
// Server -> Server
//要求另外的區域產生傳送門
struct PG_Teleport_LtoL_CreateTeleport
{
	GamePGCommandEnum	Command;
	int			FromZone;
	float		FromX , FromY , FromZ;


	int			ToZone;
	float		ToX , ToY , ToZ;
	StaticString< _MAX_NAMERECORD_SIZE_ >   Name;


	PG_Teleport_LtoL_CreateTeleport()
	{
		Command = EM_PG_Teleport_LtoL_CreateTeleport;
	}
};

#pragma managed
