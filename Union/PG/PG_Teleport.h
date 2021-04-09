#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged
//---------------------------------------------------------------------------
// Client -> Server
//�s�W�ǰe��m�n�D
struct PG_Teleport_CtoL_AddTeleportRequest
{
	GamePGCommandEnum	Command;
	int		ItemPos;			//���^���~��m
	int		TeleportPos;		//�x�s��m
	LocationSaveStruct Info;		

	PG_Teleport_CtoL_AddTeleportRequest()
	{
		Command = EM_PG_Teleport_CtoL_AddTeleportRequest;
	}
};
//---------------------------------------------------------------------------
// Server -> Client
//�s�W�ǰe��m���G
struct PG_Teleport_LtoC_AddTeleportResult
{
	GamePGCommandEnum	Command;	
	int					TeleportPos;		//�x�s��m
	LocationSaveStruct	Info;	
	bool				Result;

	PG_Teleport_LtoC_AddTeleportResult()
	{
		Command = EM_PG_Teleport_LtoC_AddTeleportResult;
	}
};
//---------------------------------------------------------------------------
// Client -> Server
//�����ǰe��m�n�D
struct PG_Teleport_CtoL_DelTeleportRequest
{
	GamePGCommandEnum	Command;
	int		TeleportPos;		//�x�s��m

	PG_Teleport_CtoL_DelTeleportRequest()
	{
		Command = EM_PG_Teleport_CtoL_DelTeleportRequest;
	}
};
//---------------------------------------------------------------------------
// Server -> Client
//�����ǰe��m���G
struct PG_Teleport_LtoC_DelTeleportResult
{
	GamePGCommandEnum	Command;	
	int					TeleportPos;		//�x�s��m
	bool				Result;

	PG_Teleport_LtoC_DelTeleportResult()
	{
		Command = EM_PG_Teleport_LtoC_DelTeleportResult;
	}
};
//---------------------------------------------------------------------------
// Client -> Server
//�洫�ǰe��m�n�D
struct PG_Teleport_CtoL_SwapTeleportRequest
{
	GamePGCommandEnum	Command;
	int		TeleportPos[2];		//�x�s��m

	PG_Teleport_CtoL_SwapTeleportRequest()
	{
		Command = EM_PG_Teleport_CtoL_SwapTeleportRequest;
	}
};
//---------------------------------------------------------------------------
// Server -> Client
//�洫�ǰe��m���G
struct PG_Teleport_LtoC_SwapTeleportResult
{
	GamePGCommandEnum	Command;	
	int					TeleportPos[2];		//�x�s��m
	bool				Result;

	PG_Teleport_LtoC_SwapTeleportResult()
	{
		Command = EM_PG_Teleport_LtoC_SwapTeleportResult;
	}
};
//---------------------------------------------------------------------------
// Client -> Server
//�ק�ǰe��m�n�D
struct PG_Teleport_CtoL_ModifyTeleportRequest
{
	GamePGCommandEnum	Command;
	int		TeleportPos;		//�x�s��m
	LocationSaveStruct Info;		

	PG_Teleport_CtoL_ModifyTeleportRequest()
	{
		Command = EM_PG_Teleport_CtoL_ModifyTeleportRequest;
	}
};
//---------------------------------------------------------------------------
// Server -> Client
//�ק�ǰe��m���G
struct PG_Teleport_LtoC_ModifyTeleportResult
{
	GamePGCommandEnum	Command;	
	int					TeleportPos;		//�x�s��m
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
	EM_UseTeleportType_Normal		,	//�@��ǰe
	EM_UseTeleportType_OneWayHole	,	//��V�ǰe
	EM_UseTeleportType_TwoWayHole	,	//���V�ǰe
};
// Client -> Server
//�ϥζǰe��m�n�D
struct PG_Teleport_CtoL_UseTeleportRequest
{
	GamePGCommandEnum	Command;
	int		ItemPos;			//���^���~��m
	int		TeleportPos;		//�x�s��m
	UseTeleportTypeENUM		Type;
	
	PG_Teleport_CtoL_UseTeleportRequest()
	{
		Command = EM_PG_Teleport_CtoL_UseTeleportRequest;
	}
};
//---------------------------------------------------------------------------
// Server -> Client
//�ϥζǰe��m���G
struct PG_Teleport_LtoC_UseTeleportResult
{
	GamePGCommandEnum	Command;
	int		ItemPos;			//���^���~��m
	int		TeleportPos;		//�x�s��m
	UseTeleportTypeENUM		Type;
	bool	Result;

	PG_Teleport_LtoC_UseTeleportResult()
	{
		Command = EM_PG_Teleport_LtoC_UseTeleportResult;
	}
};
//---------------------------------------------------------------------------
// Server -> Server
//�n�D�t�~���ϰ첣�Ͷǰe��
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
