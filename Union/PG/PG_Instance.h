#pragma once
#include "roledata/RoleData.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

//////////////////////////////////////////////////////////////////////////
struct	WeekDayStruct
{
	union
	{
		bool Day[7];
		struct  
		{
			bool	Sunday		;
			bool	Monday		;
			bool	Tuesday		;
			bool	Wednesday	;
			bool	Thursday	;
			bool	Firday		;
			bool	Saturday	;
		};
	};
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local
//�ƥ����m�ɶ���
struct PG_Instance_DtoL_ResetTime
{
	GamePGCommandEnum		Command;

	WeekDayStruct		ResetDay;

	PG_Instance_DtoL_ResetTime()
	{
		Command = EM_PG_Instance_DtoL_ResetTime;
	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local
//���a�T��
struct  PG_Instance_DtoL_PlayInfo
{
	GamePGCommandEnum		Command;

	int	PartyKey;
	int	KeyID;
	int	KeyValue;

	PG_Instance_DtoL_PlayInfo()
	{
		Command = EM_PG_Instance_DtoL_PlayInfo;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client
//�ƥ��h�[�᭫�m
struct  PG_Instance_LtoC_ResetTime
{
	GamePGCommandEnum		Command;
	int		Time;
	int		InstanceSaveID;
	int		ValueKeyID;
	bool	IsLogin;

	PG_Instance_LtoC_ResetTime()
	{
		Command = EM_PG_Instance_LtoC_ResetTime;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client
//�߰ݬO�_�n�[�J�����ƥ��i��
struct  PG_Instance_LtoC_JoinInstanceRequest
{
	GamePGCommandEnum		Command;

	PG_Instance_LtoC_JoinInstanceRequest()
	{
		Command = EM_PG_Instance_LtoC_JoinInstanceRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
// Client -> Local
//�߰ݬO�_�n�[�J�����ƥ��i��
struct  PG_Instance_CtoL_JoinInstanceResult
{
	GamePGCommandEnum		Command;
	bool	Result;

	PG_Instance_CtoL_JoinInstanceResult()
	{
		Command = EM_PG_Instance_CtoL_JoinInstanceResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client
//�q���Y�ƥ����m
struct  PG_Instance_LtoC_InstanceReset
{
	GamePGCommandEnum		Command;
	int		InstanceKey;

	PG_Instance_LtoC_InstanceReset()
	{
		Command = EM_PG_Instance_LtoC_InstanceReset;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client
//�q���Ҧ������Y�Ӱƥ��i�ױĥάY������
struct  PG_Instance_LtoC_InstanceCreate
{
	GamePGCommandEnum		Command;
	int			PlayerDBID;
	int			ZoneID;
	int			Key;

	PG_Instance_LtoC_InstanceCreate()
	{
		Command = EM_PG_Instance_LtoC_InstanceCreate;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local �n�D(����)�����ƥ��i��
struct  PG_Instance_CtoL_InstanceExtend
{
	GamePGCommandEnum		Command;
	int			ZoneID;
	int			ExtendTime;
	int			SavePos;

	PG_Instance_CtoL_InstanceExtend()
	{
		Command = EM_PG_Instance_CtoL_InstanceExtend;
	}
};
//Local -> Local �n�D(����)�����ƥ��i��
struct  PG_Instance_LtoL_InstanceExtend
{
	GamePGCommandEnum		Command;
	int			PlayerDBID;
	int			ZoneID;
	int			LiveTime;
	int			ExtendTime;
	int			KeyID;
	int			SavePos;

	PG_Instance_LtoL_InstanceExtend()
	{
		Command = EM_PG_Instance_LtoL_InstanceExtend;
	}
};
//Local -> Local �n�D(����)�����ƥ��i�׵��G
struct  PG_Instance_LtoL_InstanceExtendResult
{
	GamePGCommandEnum		Command;
	int			PlayerDBID;
	int			ZoneID;
	int			ExtendTime;
	int			LiveTime;
	int			SavePos;
	int			KeyID;

	PG_Instance_LtoL_InstanceExtendResult()
	{
		Command = EM_PG_Instance_LtoL_InstanceExtendResult;
	}
};
//Local -> Client �ץ��ƥ��i�׮ɶ�
struct  PG_Instance_LtoC_FixInstanceExtend
{
	GamePGCommandEnum		Command;
	int			ZoneID;
	int			ExtendTime;
	int			LiveTime;
	int			KeyID;

	PG_Instance_LtoC_FixInstanceExtend()
	{
		Command = EM_PG_Instance_LtoC_FixInstanceExtend;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local �d�߰ƥ��i�׭��m�ɶ��PKeyID
struct  PG_Instance_CtoL_InstanceRestTimeRequest
{
	GamePGCommandEnum		Command;
	int			ZoneID;
	int			SavePos;

	PG_Instance_CtoL_InstanceRestTimeRequest()
	{
		Command = EM_PG_Instance_CtoL_InstanceRestTimeRequest;
	}
};
//Local -> Local �d�߰ƥ��i�׭��m�ɶ�
struct  PG_Instance_LtoL_InstanceRestTimeRequest
{
	GamePGCommandEnum		Command;
	int	PlayerDBID;
	int KeyID;

	PG_Instance_LtoL_InstanceRestTimeRequest()
	{
		Command = EM_PG_Instance_LtoL_InstanceRestTimeRequest;
	}
};
//Local -> Client �d�߰ƥ��i�׭��m�ɶ����G
struct  PG_Instance_LtoC_InstanceRestTime
{
	GamePGCommandEnum		Command;
	int	ZoneID;
	int KeyID;
	int LiveTime;
	int ExtendTime;

	PG_Instance_LtoC_InstanceRestTime()
	{
		Command = EM_PG_Instance_LtoC_InstanceRestTime;
	}
};

// Zone server �V datacenter ���U�Χ�s�ܼ�
struct PG_Instance_LtoD_SetWorldVar
{
	GamePGCommandEnum	Command;

	StaticString<32>	VarName;
	int					VarValue;

	PG_Instance_LtoD_SetWorldVar()
	{
		Command	= EM_PG_Instance_LtoD_SetWorldVar;
	}
};

struct PG_Instance_DtoL_SetWorldVar
{
	GamePGCommandEnum	Command;

	StaticString<32>	VarName;
	int					VarValue;

	PG_Instance_DtoL_SetWorldVar()
	{
		Command	= EM_PG_Instance_DtoL_SetWorldVar;
	}
};

struct PG_Instance_CtoL_GetWorldVar
{
	GamePGCommandEnum	Command;

	StaticString<32>	VarName;

	PG_Instance_CtoL_GetWorldVar()
	{
		Command	= EM_PG_Instance_CtoL_GetWorldVar;
	}
};


struct PG_Instance_LtoC_GetWorldVarResult
{
	GamePGCommandEnum	Command;

	StaticString<32>	VarName;
	int					VarValue;

	PG_Instance_LtoC_GetWorldVarResult()
	{
		Command	= EM_PG_Instance_LtoC_GetWorldVarResult;
	}
};

//////////////////////////////////////////////////////////////////////////
#pragma managed
