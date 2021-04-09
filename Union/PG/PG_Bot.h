#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged
//---------------------------------------------------------------------------
//Client -> Local 
//要求某點的路徑
struct PG_Bot_CtoL_RequestMovePath
{
	GamePGCommandEnum	Command;
	float	X , Y , Z;
	float	Range;

	PG_Bot_CtoL_RequestMovePath()
	{
		Command = EM_PG_Bot_CtoL_RequestMovePath;
	}
};
//---------------------------------------------------------------------------
//Local -> Client
//回傳Bot要求的移動路徑點給Client
struct PG_Bot_CtoL_MovePoint
{
	GamePGCommandEnum	Command;
	int		Count;
	int		Index;
	float	X , Y , Z;

	PG_Bot_CtoL_MovePoint()
	{
		Command = EM_PG_Bot_CtoL_MovePoint;
	}
};
//---------------------------------------------------------------------------