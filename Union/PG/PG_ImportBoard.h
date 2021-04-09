#ifndef __PG_IMPORTBOARD_H__
#define __PG_IMPORTBOARD_H__
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

//-------------------------------------------------------------------------
//Client -> Local
//要求公告資訊
struct PG_ImportBoard_CtoL_ImportBoardRequest
{
	GamePGCommandEnum	Command;

	float				UpdateTime;	

	PG_ImportBoard_CtoL_ImportBoardRequest()
	{	Command = EM_PG_ImportBoard_CtoL_ImportBoardRequest;	};

};
//-------------------------------------------------------------------------
//Local -> Datacenter
//client 要求公告資訊
struct PG_ImportBoard_LtoD_ImportBoardRequest
{
	GamePGCommandEnum	Command;

	float				UpdateTime;	
	int					PlayerDBID;

	PG_ImportBoard_LtoD_ImportBoardRequest()
	{	Command = EM_PG_ImportBoard_LtoD_ImportBoardRequest;	};

};
//-------------------------------------------------------------------------
//DataCenter -> Client
//通知公告資訊
struct PG_ImportBoard_DtoC_ImportBoardInfo
{
	GamePGCommandEnum	Command;

	float				UpdateTime;	
	int					DataCount;		//幾筆公告
	int					DataSize;
	char				Data[0x20000];

	PG_ImportBoard_DtoC_ImportBoardInfo()
	{	Command = EM_PG_ImportBoard_DtoC_ImportBoardInfo;	};

	int	PGSize()
	{
		return sizeof(int)*3 + sizeof(float ) + DataSize;
	}
};
//-------------------------------------------------------------------------
//DataCenter -> Client
//通知有新的版本資訊
struct PG_ImportBoard_DtoC_ImportBoardNewVersion
{
	GamePGCommandEnum	Command;

	float				UpdateTime;	

	PG_ImportBoard_DtoC_ImportBoardNewVersion()
	{	Command = EM_PG_ImportBoard_DtoC_ImportBoardNewVersion;	};

};
//-------------------------------------------------------------------------
//Local -> DataCenter
//要求通知Client目前 版本
struct PG_ImportBoard_LtoD_NewVersionRequest
{
	GamePGCommandEnum	Command;
	int					SockID;
	int					ProxyID;
	

	PG_ImportBoard_LtoD_NewVersionRequest()
	{	Command = EM_PG_ImportBoard_LtoD_NewVersionRequest;	};

};
//-------------------------------------------------------------------------
#pragma managed
#endif 