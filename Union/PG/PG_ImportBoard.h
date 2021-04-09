#ifndef __PG_IMPORTBOARD_H__
#define __PG_IMPORTBOARD_H__
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

//-------------------------------------------------------------------------
//Client -> Local
//�n�D���i��T
struct PG_ImportBoard_CtoL_ImportBoardRequest
{
	GamePGCommandEnum	Command;

	float				UpdateTime;	

	PG_ImportBoard_CtoL_ImportBoardRequest()
	{	Command = EM_PG_ImportBoard_CtoL_ImportBoardRequest;	};

};
//-------------------------------------------------------------------------
//Local -> Datacenter
//client �n�D���i��T
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
//�q�����i��T
struct PG_ImportBoard_DtoC_ImportBoardInfo
{
	GamePGCommandEnum	Command;

	float				UpdateTime;	
	int					DataCount;		//�X�����i
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
//�q�����s��������T
struct PG_ImportBoard_DtoC_ImportBoardNewVersion
{
	GamePGCommandEnum	Command;

	float				UpdateTime;	

	PG_ImportBoard_DtoC_ImportBoardNewVersion()
	{	Command = EM_PG_ImportBoard_DtoC_ImportBoardNewVersion;	};

};
//-------------------------------------------------------------------------
//Local -> DataCenter
//�n�D�q��Client�ثe ����
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