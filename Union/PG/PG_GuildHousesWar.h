#pragma once
/*
#include "RoleData\RoleDataEx.h"
#include "NetWakerPGEnum.h"

//���|�ξ�

//1 ���U���|��
//2 �ݥثe���|�Ե��U���G
//3 �q�����|�Զ}��
//4 �ǤJ���|�Գ�(�u���@�볥�~���)

//////////////////////////////////////////////////////////////////////////
//Client ->Local �}�ҵ��U���|�Ԥ���	
struct PG_GuildHousesWar_CtoL_OpenMenu
{
	GamePGCommandEnum	Command;

	PG_GuildHousesWar_CtoL_OpenMenu()
	{
		Command = EM_PG_GuildHousesWar_CtoL_OpenMenu;
	}
};

//////////////////////////////////////////////////////////////////////////
//Local->DataCenter �}�ҵ��U���|�Ԥ���	
struct PG_GuildHousesWar_LtoD_OpenMenu
{
	GamePGCommandEnum	Command;
	int		PlayerDBID;

	PG_GuildHousesWar_LtoD_OpenMenu()
	{
		Command = EM_PG_GuildHousesWar_LtoD_OpenMenu;
	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Client �}�ҵ��U���|�Ԥ���
struct PG_GuildHousesWar_DtoC_OpenMenu
{
	GamePGCommandEnum	Command;
	GuildHouseWarStateENUM	State;
	int						Size;
	GuildHouseWarInfoStruct List[1000];

	PG_GuildHousesWar_DtoC_OpenMenu()
	{
		Command = EM_PG_GuildHousesWar_DtoC_OpenMenu;
	}
	int PGSize()
	{
		return sizeof(*this) - sizeof(List) + Size * sizeof(GuildHouseWarInfoStruct);
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 	���U���|��
enum GuildHouseWarRegisterTypeENUM
{
	EM_GuildHouseWarRegisterType_Register	,
	EM_GuildHouseWarRegisterType_Cancel		,
};
struct PG_GuildHousesWar_CtoL_WarRegister
{
	GamePGCommandEnum	Command;
	GuildHouseWarRegisterTypeENUM Type;

	PG_GuildHousesWar_CtoL_WarRegister()
	{
		Command = EM_PG_GuildHousesWar_CtoL_WarRegister;
	}
};

//Local -> Datacenter ���U���|��
struct PG_GuildHousesWar_LtoD_WarRegister
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	GuildHouseWarRegisterTypeENUM Type;

	PG_GuildHousesWar_LtoD_WarRegister()
	{
		Command = EM_PG_GuildHousesWar_LtoD_WarRegister;
	}
};

//DataCenter -> Client ���U���|�Ե��G
struct PG_GuildHousesWar_DtoC_WarRegisterResult
{
	GamePGCommandEnum	Command;
	GuildHouseWarRegisterTypeENUM Type;
	bool Result;

	PG_GuildHousesWar_DtoC_WarRegisterResult()
	{
		Command = EM_PG_GuildHousesWar_DtoC_WarRegisterResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local ���|�Զ}�l
struct PG_GuildHousesWar_DtoL_WarBegin
{
	GamePGCommandEnum	Command;
	int					GuildID[2];

	PG_GuildHousesWar_DtoL_WarBegin()
	{
		Command = EM_PG_GuildHousesWar_DtoL_WarBegin;
	}
};
//Local -> Client ���|�Զ}�l
struct PG_GuildHousesWar_LtoC_WarBegin
{
	GamePGCommandEnum	Command;
	int					GuildID[2];

	PG_GuildHousesWar_LtoC_WarBegin()
	{
		Command = EM_PG_GuildHousesWar_LtoC_WarBegin;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local �ǤJ���|�Գ�
struct PG_GuildHousesWar_CtoL_EnterWar
{
	GamePGCommandEnum	Command;

	PG_GuildHousesWar_CtoL_EnterWar()
	{
		Command = EM_PG_GuildHousesWar_CtoL_EnterWar;
	}
};
//Local -> Client �ǤJ���|�Գ����G
struct PG_GuildHousesWar_LtoC_EnterWarResult
{
	GamePGCommandEnum	Command;
	bool Result;

	PG_GuildHousesWar_LtoC_EnterWarResult()
	{
		Command = EM_PG_GuildHousesWar_LtoC_EnterWarResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local �ǤJ�X�|�Գ�
struct PG_GuildHousesWar_CtoL_LeaveWar
{
	GamePGCommandEnum	Command;

	PG_GuildHousesWar_CtoL_LeaveWar()
	{
		Command = EM_PG_GuildHousesWar_CtoL_LeaveWar;
	}
};
//Local -> Client �ǤJ�X�|�Գ����G
struct PG_GuildHousesWar_LtoC_LeaveWarResult
{
	GamePGCommandEnum	Command;
	bool Result;

	PG_GuildHousesWar_LtoC_LeaveWarResult()
	{
		Command = EM_PG_GuildHousesWar_LtoC_LeaveWarResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local �Ԫ�����
struct PG_GuildHousesWar_DtoL_WarEnd
{
	GamePGCommandEnum	Command;

	PG_GuildHousesWar_DtoL_WarEnd()
	{
		Command = EM_PG_GuildHousesWar_DtoL_WarEnd;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter �Ԫ�����
struct PG_GuildHousesWar_LtoD_WarEndOK
{
	GamePGCommandEnum	Command;

	PG_GuildHousesWar_LtoD_WarEndOK()
	{
		Command = EM_PG_GuildHousesWar_LtoD_WarEndOK;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client �Ԫ����G
struct PG_GuildHousesWar_LtoC_EndWar
{
	GamePGCommandEnum	Command;

	int					GuildID[2];
	int					Score[2];

	PG_GuildHousesWar_LtoC_EndWar()
	{
		Command = EM_PG_GuildHousesWar_LtoC_EndWar;
	}
};

//Local -> Client �Ԫ����G
struct PG_GuildHousesWar_LtoC_EndWarPlayerInfo
{
	GamePGCommandEnum	Command;
	
	int							Count;
	GuildWarPlayerInfoStruct	PlayerList[ 1000 ];

	PG_GuildHousesWar_LtoC_EndWarPlayerInfo()
	{
		Command = EM_PG_GuildHousesWar_LtoC_EndWarPlayerInfo;
	}
	int Size( )
	{
		return sizeof(*this)- sizeof(PlayerList) + Count * sizeof( PlayerList );
	}
};
//////////////////////////////////////////////////////////////////////////
//�إ� �Գ� ���|��
//////////////////////////////////////////////////////////////////////////
// Local -> DataCenter �n�D���|�θ�Ƹ��
struct PG_GuildHousesWar_LtoD_LoadHouseBaseInfo
{
	GamePGCommandEnum	Command;
	PG_GuildHousesWar_LtoD_LoadHouseBaseInfo()
	{
		Command = EM_PG_GuildHousesWar_LtoD_LoadHouseBaseInfo;
	}
};

//DataCenter -> Local ���|�θ�Ƹ��
struct PG_GuildHousesWar_DtoL_HouseBaseInfo
{
	GamePGCommandEnum	Command;
	GuildHousesInfoStruct	HouseBaseInfo;

	PG_GuildHousesWar_DtoL_HouseBaseInfo()
	{
		Command = EM_PG_GuildHousesWar_DtoL_HouseBaseInfo;
	}
};
//-------------------------------------------------------------------------
//DataCenter -> Local ���|�ؿv�����
struct PG_GuildHousesWar_DtoL_BuildingInfo
{
	GamePGCommandEnum	Command;

	GuildHouseBuildingInfoStruct	Building;

	PG_GuildHousesWar_DtoL_BuildingInfo()
	{
		Command = EM_PG_GuildHousesWar_DtoL_BuildingInfo;
	}
};
//-------------------------------------------------------------------------
//DataCenter -> Local �q�����|��Ū������			
struct PG_GuildHousesWar_DtoL_HouseInfoLoadOK
{
	GamePGCommandEnum	Command;
	int					GuildID;

	PG_GuildHousesWar_DtoL_HouseInfoLoadOK()
	{
		Command = EM_PG_GuildHousesWar_DtoL_HouseInfoLoadOK;
	}
};
//-------------------------------------------------------------------------
//DataCenter -> Local ���~�ܮw���~���
struct PG_GuildHousesWar_DtoL_FurnitureItemInfo
{
	GamePGCommandEnum	Command;

	GuildHouseFurnitureItemStruct Item;

	PG_GuildHousesWar_DtoL_FurnitureItemInfo()
	{
		Command = EM_PG_GuildHousesWar_DtoL_FurnitureItemInfo;
	}
};
//------------------------------------------------------------------------
//Local -> Client �Ԫ����G
struct PG_GuildHousesWar_LtoD_DebugTime
{
	GamePGCommandEnum	Command;

	int		Time_Hour;
	int		Time_Day;

	PG_GuildHousesWar_LtoD_DebugTime()
	{
		Command = EM_PG_GuildHousesWar_LtoD_DebugTime;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//�n�D�����
struct PG_GuildHousesWar_CtoL_PricesRequst
{
	GamePGCommandEnum	Command;

	PG_GuildHousesWar_CtoL_PricesRequst()
	{
		Command = EM_PG_GuildHousesWar_CtoL_PricesRequst;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client
//�n�D�����
struct PG_GuildHousesWar_LtoC_PricesResult
{
	GamePGCommandEnum	Command;
	bool				Result;
	
	PG_GuildHousesWar_LtoC_PricesResult()
	{
		Command = EM_PG_GuildHousesWar_LtoC_PricesResult;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client �n�D���v���
//Client -> Local
struct PG_GuildHousesWar_CtoL_HistoryRequest
{
	GamePGCommandEnum	Command;
	int					DayBefore;

	PG_GuildHousesWar_CtoL_HistoryRequest()
	{
		Command = EM_PG_GuildHousesWar_CtoL_HistoryRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client �n�D���v���
//Local -> DataCenter
struct PG_GuildHousesWar_LtoD_HistoryRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					DayBefore;

	PG_GuildHousesWar_LtoD_HistoryRequest()
	{
		Command = EM_PG_GuildHousesWar_LtoD_HistoryRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client �n�D���v��Ʀ^��
// DataCenter -> Client
struct PG_GuildHousesWar_DtoC_HistoryResult
{
	GamePGCommandEnum	Command;
	int					DayBefore;
	int					Count;
	GuildHouseWarHistoryStruct List[1000];

	PG_GuildHousesWar_DtoC_HistoryResult()
	{
		Command = EM_PG_GuildHousesWar_DtoC_HistoryResult;
	}
	int Size()
	{
		return sizeof(*this) - sizeof(List) + Count * sizeof( GuildHouseWarHistoryStruct );
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client
//���|���Z����T
struct PG_GuildHousesWar_LtoC_GuildScore
{
	GamePGCommandEnum	Command;
	GuildHouseWarFightStruct	Info;

	PG_GuildHousesWar_LtoC_GuildScore()
	{
		Command = EM_PG_GuildHousesWar_LtoC_GuildScore;
	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//�n�D���|�ԭӤH�Z����T
struct PG_GuildHousesWar_CtoL_PlayerScoreRequest
{
	GamePGCommandEnum	Command;
//	int		PlayerDBID;

	PG_GuildHousesWar_CtoL_PlayerScoreRequest()
	{
		Command = EM_PG_GuildHousesWar_CtoL_PlayerScoreRequest;
	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client
//���|�ԭӤH�Z����T
struct PG_GuildHousesWar_LtoC_PlayerScore
{
	GamePGCommandEnum	Command;

	int							Count;
	GuildWarPlayerInfoStruct	PlayerList[ 1000 ];

	PG_GuildHousesWar_LtoC_PlayerScore()
	{
		Command = EM_PG_GuildHousesWar_LtoC_PlayerScore;
	}
	int Size( )
	{
		return sizeof(*this)- sizeof(PlayerList) + Count * sizeof( PlayerList );
	}

};
//////////////////////////////////////////////////////////////////////////
*/