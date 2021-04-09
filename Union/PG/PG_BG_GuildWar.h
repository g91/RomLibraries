#pragma once
#include "NetWakerPGEnum.h"
#include "roledata/RoleDataEx.h"
#include "PG/PG_BG_GuildWarDef.h"
//////////////////////////////////////////////////////////////////////////
//�n�D����A�����|�W�٭n�D
//Client->Battle Field Local
struct PG_BG_GuildWar_CtoBL_GuildNameRequest
{
	GamePGCommandEnum	Command;
	int		WorldGuildID;

	PG_BG_GuildWar_CtoBL_GuildNameRequest()
	{Command = EM_PG_BG_GuildWar_CtoBL_GuildNameRequest;}
};
//////////////////////////////////////////////////////////////////////////
//�n�D����A�����|�W�٭n�D
//Battle Field Local->Local
struct PG_BG_GuildWar_BLtoL_GuildNameRequest
{
	GamePGCommandEnum	Command;
	int	PlayerDBID;
	int	WorldGuildID;

	PG_BG_GuildWar_BLtoL_GuildNameRequest()
	{Command = EM_PG_BG_GuildWar_BLtoL_GuildNameRequest;}
};
//////////////////////////////////////////////////////////////////////////
//�n�D����A�����|�W�ٵ��G
//Local->Battle Field Local
struct PG_BG_GuildWar_LtoBL_GuildNameResult
{
	GamePGCommandEnum	Command;
	int	PlayerDBID;
	int	WorldGuildID;
	StaticString<_MAX_NAMERECORD_SIZE_> GuildName;	

	PG_BG_GuildWar_LtoBL_GuildNameResult()
	{Command = EM_PG_BG_GuildWar_LtoBL_GuildNameResult;}
};
//////////////////////////////////////////////////////////////////////////
//�n�D����A�����|�W�ٵ��G
//Battle Field Local->Client
struct PG_BG_GuildWar_BLtoC_GuildNameResult
{
	GamePGCommandEnum	Command;
	int	WorldGuildID;
	StaticString<_MAX_NAMERECORD_SIZE_> GuildName;	

	PG_BG_GuildWar_BLtoC_GuildNameResult()
	{Command = EM_PG_BG_GuildWar_BLtoC_GuildNameResult;}
};
//////////////////////////////////////////////////////////////////////////
//�}�ҵ��U���|�Ԥ���
//Client->Local
struct PG_BG_GuildWar_CtoL_OpenMenuRequest
{
	GamePGCommandEnum	Command;


	PG_BG_GuildWar_CtoL_OpenMenuRequest()
	{Command = EM_PG_BG_GuildWar_CtoL_OpenMenuRequest;}
};
//////////////////////////////////////////////////////////////////////////
//�n�D���|�Ը�T
//Local->Battle Field Local
struct PG_BG_GuildWar_LtoBL_OpenMenuRequest
{
	GamePGCommandEnum	Command;
	int PlayerDBID;

	PG_BG_GuildWar_LtoBL_OpenMenuRequest()
	{Command = EM_PG_BG_GuildWar_LtoBL_OpenMenuRequest;}
};
//////////////////////////////////////////////////////////////////////////
//���|�Զ}�Ҫ��p
//Battle Field Local->Local
struct PG_BG_GuildWar_BLtoL_OpenMenuResult
{
	GamePGCommandEnum	Command;
	int PlayerDBID;	
	int	ZoneGroupID;
	GuildHouseWarStateENUM	State;
	int	NextTime;
	int						Count;
	GuildHouseWarInfoStruct List[1000];

	PG_BG_GuildWar_BLtoL_OpenMenuResult()
	{Command = EM_PG_BG_GuildWar_BLtoL_OpenMenuResult;}
	int PGSize()
	{
		return sizeof(*this) - sizeof(List) + Count * sizeof( GuildHouseWarInfoStruct );
	}
};
//////////////////////////////////////////////////////////////////////////
//�}�ҵ��U���|�Ԥ���
//Local->Client
struct PG_BG_GuildWar_LtoC_OpenMenuResult
{
	GamePGCommandEnum	Command;
	int PlayerDBID;	
	int ZoneGroupID;
	GuildHouseWarStateENUM	State;
	int	NextTime;
	int						Count;
	GuildHouseWarInfoStruct List[1000];

	PG_BG_GuildWar_LtoC_OpenMenuResult()
	{Command = EM_PG_BG_GuildWar_LtoC_OpenMenuResult;}
	int PGSize()
	{
		return sizeof(*this) - sizeof(List) + Count * sizeof( GuildHouseWarInfoStruct );
	}
};
//////////////////////////////////////////////////////////////////////////
//���U���|��( or ���� )
//Client->Local
struct PG_BG_GuildWar_CtoL_RegisterRequest
{
	GamePGCommandEnum	Command;
	GuildWarRegisterTypeENUM Type;
	int					ZoneGroupID;
	int					GuildCount;			//�ثe���|�ƶq
	bool				IsAcceptAssignment;	//�������w���
	int					TargetGuild;		//���w��Ԥ��|

	PG_BG_GuildWar_CtoL_RegisterRequest()
	{Command = EM_PG_BG_GuildWar_CtoL_RegisterRequest;}
};
//////////////////////////////////////////////////////////////////////////
//���U���|��( or ���� )
//Local->Battle Field Local
struct GuildWarRegisterInfoStruct
{
	int					GuildID;
	int					GuildCount;
	int					GuildLv;
	int 				Score;
	int					PlayerDBID;			//�n�D��
	bool				IsAcceptAssignment;	//�������w���
	int					TargetGuild;		//���w��Ԥ��|
	StaticString< _MAX_NAMERECORD_SIZE_ > GuildName;
	GuildWarRegisterTypeENUM Type;
};
struct PG_BG_GuildWar_LtoBL_RegisterRequest
{
	GamePGCommandEnum	Command;

	GuildWarRegisterInfoStruct Info;
	/*
	int					GuildID;
	int					GuildCount;
	int 				Score;
	int					PlayerDBID;			//�n�D��
	bool				IsAcceptAssignment;	//�������w���
	int					TargetGuild;		//���w��Ԥ��|

	StaticString< _MAX_NAMERECORD_SIZE_ > GuildName;
	GuildWarRegisterTypeENUM Type;
*/

	PG_BG_GuildWar_LtoBL_RegisterRequest()
	{Command = EM_PG_BG_GuildWar_LtoBL_RegisterRequest;}
};
//////////////////////////////////////////////////////////////////////////
enum GuildWarRegisterResultENUM
{
	EM_GuildWarRegisterResult_OK			,
	EM_GuildWarRegisterResult_UnknowErr		,
	EM_GuildWarRegisterResult_Full			,
	EM_GuildWarRegisterResult_GuildCountErr	,
	EM_GuildWarRegisterResult_NoGuildHouse	,	//�S�����|��
	EM_GuildWarRegisterResult_NotLeader		,	//���O�|��
	EM_GuildWarRegisterResult_OnceaDay		,	//�@�Ѥ@�����|��
	EM_GuildWarRegisterResult_GuildWarClose	,	//�D�}��ɶ�
};
//���U���|��( or ���� )���G
//Battle Field Local->Local
struct PG_BG_GuildWar_BLtoL_RegisterResult
{
	GamePGCommandEnum	Command;
	int		GuildID;
	GuildWarRegisterTypeENUM Type;
	GuildHouseWarInfoStruct Info;
	GuildWarRegisterResultENUM	Result;
	int		PlayerDBID;			//�n�D��

	PG_BG_GuildWar_BLtoL_RegisterResult()
	{Command = EM_PG_BG_GuildWar_BLtoL_RegisterResult;}
};
//////////////////////////////////////////////////////////////////////////
//���U���|��( or ���� )���G
//Local->Client
struct PG_BG_GuildWar_LtoC_RegisterResult
{
	GamePGCommandEnum	Command;
	GuildWarRegisterTypeENUM Type;
	GuildHouseWarInfoStruct Info;
	GuildWarRegisterResultENUM	Result;

	PG_BG_GuildWar_LtoC_RegisterResult()
	{Command = EM_PG_BG_GuildWar_LtoC_RegisterResult;}
};
//////////////////////////////////////////////////////////////////////////
//���|�Ԫ��p( �}�l , �ǳ� , ���� )
//Battle Field Local->Local
struct PG_BG_GuildWar_BLtoL_Status
{
	GamePGCommandEnum	Command;
	GuildWarStatusTypeENUM Status;
	int					NextTime;

	PG_BG_GuildWar_BLtoL_Status()
	{Command = EM_PG_BG_GuildWar_BLtoL_Status;}
};
//////////////////////////////////////////////////////////////////////////
//���|�Ԫ��p( �}�l , �ǳ� , ���� )
//Local->Client
struct PG_BG_GuildWar_LtoC_Status
{
	GamePGCommandEnum	Command;
	GuildWarStatusTypeENUM Status;
	int					NextTime;

	PG_BG_GuildWar_LtoC_Status()
	{Command = EM_PG_BG_GuildWar_LtoC_Status;}
};
//////////////////////////////////////////////////////////////////////////
//�n�D���|�θ��(��zone401�����)
//Battle Field Local->Local
struct PG_BG_GuildWar_BLtoL_HouseLoadRequest
{
	GamePGCommandEnum	Command;
	int		GuildID;

	PG_BG_GuildWar_BLtoL_HouseLoadRequest()
	{Command = EM_PG_BG_GuildWar_BLtoL_HouseLoadRequest;}
};
//////////////////////////////////////////////////////////////////////////
//���|�ΰ򥻸��
//Local->Battle Field Local
struct PG_BG_GuildWar_LtoBL_HouseBaseInfo
{
	GamePGCommandEnum	Command;
	GuildHousesInfoStruct HouseBaseInfo;

	PG_BG_GuildWar_LtoBL_HouseBaseInfo()
	{Command = EM_PG_BG_GuildWar_LtoBL_HouseBaseInfo;}
};
//////////////////////////////////////////////////////////////////////////
//���|�Ϋؿv���
//Local->Battle Field Local
struct PG_BG_GuildWar_LtoBL_HouseBuildingInfo
{
	GamePGCommandEnum	Command;
	GuildHouseBuildingInfoStruct Building;

	PG_BG_GuildWar_LtoBL_HouseBuildingInfo()
	{Command = EM_PG_BG_GuildWar_LtoBL_HouseBuildingInfo;}
};
//////////////////////////////////////////////////////////////////////////
//���|�ήa����
//Local->Battle Field Local
struct PG_BG_GuildWar_LtoBL_HouseFurnitureInfo
{
	GamePGCommandEnum	Command;
	GuildHouseFurnitureItemStruct Item;

	PG_BG_GuildWar_LtoBL_HouseFurnitureInfo()
	{Command = EM_PG_BG_GuildWar_LtoBL_HouseFurnitureInfo;}
};
//////////////////////////////////////////////////////////////////////////
//���|�θ��Ū������
//Local->Battle Field Local
struct PG_BG_GuildWar_LtoBL_HouseLoadOK
{
	GamePGCommandEnum	Command;
	int	GuildID;

	PG_BG_GuildWar_LtoBL_HouseLoadOK()
	{Command = EM_PG_BG_GuildWar_LtoBL_HouseLoadOK;}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�i�J���|�Գ�
//Client->Local
struct PG_BG_GuildWar_CtoL_EnterRequest
{
	GamePGCommandEnum	Command;
	int					ZoneGroupID;


	PG_BG_GuildWar_CtoL_EnterRequest()
	{Command = EM_PG_BG_GuildWar_CtoL_EnterRequest;}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�i�J���|�Գ�
//Local->Battle Field Local
struct PG_BG_GuildWar_LtoBL_EnterRequest
{
	GamePGCommandEnum	Command;
	int PlayerDBID;
	int GuildID;

	PG_BG_GuildWar_LtoBL_EnterRequest()
	{Command = EM_PG_BG_GuildWar_LtoBL_EnterRequest;}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�i�J���G
//Battle Field Local->Local
struct PG_BG_GuildWar_BLtoL_EnterResult
{
	GamePGCommandEnum	Command;
	int ZoneGroupID;
	int PlayerDBID;
	int GuildID;
	bool Result;

	PG_BG_GuildWar_BLtoL_EnterResult()
	{Command = EM_PG_BG_GuildWar_BLtoL_EnterResult;}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�i�J���G
//Local->Client
struct PG_BG_GuildWar_LtoC_EnterResult
{
	GamePGCommandEnum	Command;
	bool Result;

	PG_BG_GuildWar_LtoC_EnterResult()
	{Command = EM_PG_BG_GuildWar_LtoC_EnterResult;}
};
//////////////////////////////////////////////////////////////////////////
//�n�D���}���|�Գ�
//Client->Battle Field Local
struct PG_BG_GuildWar_CtoBL_LeaveRequest
{
	GamePGCommandEnum	Command;


	PG_BG_GuildWar_CtoBL_LeaveRequest()
	{Command = EM_PG_BG_GuildWar_CtoBL_LeaveRequest;}
};
//////////////////////////////////////////////////////////////////////////
//�n�D���}���|�Գ����G
//Battle Field Local->Client
struct PG_BG_GuildWar_BLtoC_LeaveResult
{
	GamePGCommandEnum	Command;
	bool Result;

	PG_BG_GuildWar_BLtoC_LeaveResult()
	{Command = EM_PG_BG_GuildWar_BLtoC_LeaveResult;}
};
//////////////////////////////////////////////////////////////////////////
//���|���Z����T
//Battle Field Local->Client
struct PG_BG_GuildWar_BLtoC_GuildScore
{
	GamePGCommandEnum	Command;
	int					Time;
	GuildHouseWarFightStruct Info;

	PG_BG_GuildWar_BLtoC_GuildScore()
	{Command = EM_PG_BG_GuildWar_BLtoC_GuildScore;}
};
//////////////////////////////////////////////////////////////////////////
//���|�Զ}�l
//Battle Field Local->Client
struct PG_BG_GuildWar_BLtoC_BeginWar
{
	GamePGCommandEnum	Command;


	PG_BG_GuildWar_BLtoC_BeginWar()
	{Command = EM_PG_BG_GuildWar_BLtoC_BeginWar;}
};
//////////////////////////////////////////////////////////////////////////
//���|�Ե���
//Battle Field Local->Client
struct PG_BG_GuildWar_BLtoC_EndWar
{
	GamePGCommandEnum	Command;

	PG_BG_GuildWar_BLtoC_EndWar()
	{Command = EM_PG_BG_GuildWar_BLtoC_EndWar;}
};
//////////////////////////////////////////////////////////////////////////
//�����ɶ��]�w
//��ocal -> Battle Field Local
struct PG_BG_GuildWar_LtoBL_DebugTime
{
	GamePGCommandEnum	Command;
	int		Time_Hour;
	int		Time_Day;
	
	PG_BG_GuildWar_LtoBL_DebugTime()
	{Command = EM_PG_BG_GuildWar_LtoBL_DebugTime;}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�Գ����a���A���
//Client -> Battle Field Local
struct PG_BG_GuildWar_CtoBL_PlayerInfoRequest
{
	GamePGCommandEnum			Command;

	PG_BG_GuildWar_CtoBL_PlayerInfoRequest()
	{Command = EM_PG_BG_GuildWar_CtoBL_PlayerInfoRequest;}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�Գ����a���A���
//Battle Field Local -> Client
struct PG_BG_GuildWar_BLtoC_PlayerInfoResult
{
	GamePGCommandEnum			Command;
	int							TotalCount;
	int							ID;
	GuildWarPlayerInfoStruct	Info;

	PG_BG_GuildWar_BLtoC_PlayerInfoResult()
	{Command = EM_PG_BG_GuildWar_BLtoC_PlayerInfoResult;}
};
//////////////////////////////////////////////////////////////////////////
struct PlayerListInfoBase
{
	int	Count;
	GuildWarPlayerInfoStruct	Info[500];
};
//���]�e�X���a���Ƹ�T
struct PG_BG_GuildWar_BLtoC_PlayerListInfo_Zip
{
	GamePGCommandEnum		Command;
	int						DataSize;
	char*					Data[0x10000];

	PG_BG_GuildWar_BLtoC_PlayerListInfo_Zip()
	{Command = EM_PG_BG_GuildWar_BLtoC_PlayerListInfo_Zip;}

	int PGSize()
	{
		return sizeof(*this) - sizeof(Data) + DataSize;
	}
};

//////////////////////////////////////////////////////////////////////////
//�q���̫���Z���
//Battle Field Local -> Client
struct PG_BG_GuildWar_BLtoC_PlayerFinalInfo
{
	GamePGCommandEnum			Command;
	
	int							PlayerDBID;
	int							PrizeType;
	int							Score;
	int							OrderID;

	Voc_ENUM					Voc;								//¾�~
	Voc_ENUM					Voc_Sub;							//��¾�~
	int							Level;
	int							Level_Sub;	


	PG_BG_GuildWar_BLtoC_PlayerFinalInfo()
	{Command = EM_PG_BG_GuildWar_BLtoC_PlayerFinalInfo;}
};

//////////////////////////////////////////////////////////////////////////
//���|�Ե����q���̫��Ĺ( ��쥻Server 401 ���|�λ�)
//Battle Field Local->Local
struct PG_BG_GuildWar_BLtoL_EndWarResult
{
	GamePGCommandEnum	Command;
	int					GuildID;
	GameResultENUM		GameResult;
	int					GuildScore;		//���|�n���ץ�
	GuildWarBaseInfoStruct TargetGuild;

	PG_BG_GuildWar_BLtoL_EndWarResult()
	{Command = EM_PG_BG_GuildWar_BLtoL_EndWarResult;}
};

//////////////////////////////////////////////////////////////////////////
//�n�D���J���|�Ը��
//BL -> DataCenter
struct PG_BG_GuildWar_BLtoD_LoadInfo
{
	GamePGCommandEnum	Command;
	int					ZoneGroupID;

	PG_BG_GuildWar_BLtoD_LoadInfo()
	{Command = EM_PG_BG_GuildWar_BLtoD_LoadInfo;}
};


struct PG_BG_GuildWar_DtoBL_OrderInfo
{
	GamePGCommandEnum	Command;
	int						Count;
	GuildHouseWarInfoStruct List[500];

	PG_BG_GuildWar_DtoBL_OrderInfo()
	{Command = EM_PG_BG_GuildWar_DtoBL_OrderInfo;}
};
//////////////////////////////////////////////////////////////////////////
// ���|�Ʀ�
//////////////////////////////////////////////////////////////////////////
struct GuildWarRankInfoStruct
{
	StaticString<_MAX_NAMERECORD_SIZE_> GuildName;	
	int					WorldGuildID;
	int					Score;
	int					WinCount;			//�ӳ�
	int					EvenCount;			//����
	int					LostCount;			//�ѳ�

};
//Datacenter -> Battle Local
//�w�ɦ^�����|�Ʀ���
struct PG_BG_GuildWar_DtoBL_RegGuildWarRankInfo
{
	GamePGCommandEnum			Command;
	GuildWarRankInfoStruct		Info;
	GuildHouseWarHistoryStruct	Hisotry[50];

	PG_BG_GuildWar_DtoBL_RegGuildWarRankInfo()
	{Command = EM_PG_BG_GuildWar_DtoBL_RegGuildWarRankInfo;}
};
//////////////////////////////////////////////////////////////////////////
enum GuildWarRankInfoTypeENUM
{
	EM_GuildWarRankInfoType_RankList	,
	EM_GuildWarRankInfoType_History		,
};
//Client -> Local 
//�n�D�Ҧ��u�|�Ʀ檺���
struct PG_BG_GuildWar_CtoL_GuildWarRankInfoRequest
{
	GamePGCommandEnum			Command;
	GuildWarRankInfoTypeENUM	Type;
	int							WorldGuildID;

	PG_BG_GuildWar_CtoL_GuildWarRankInfoRequest()
	{Command = EM_PG_BG_GuildWar_CtoL_GuildWarRankInfoRequest;}
};
//////////////////////////////////////////////////////////////////////////
//Local -> �Գ�401
//�n�D�Ҧ��u�|�Ʀ檺���
struct PG_BG_GuildWar_LtoBL_GuildWarRankInfoRequest
{
	GamePGCommandEnum			Command;
	int							PlayerDBID;
	int							WorldGuildID;
	GuildWarRankInfoTypeENUM	Type;

	PG_BG_GuildWar_LtoBL_GuildWarRankInfoRequest()
	{Command = EM_PG_BG_GuildWar_LtoBL_GuildWarRankInfoRequest;}
};
//////////////////////////////////////////////////////////////////////////
// �Գ�401 -> Local
//���|�Ʀ���
struct PG_BG_GuildWar_BLtoL_GuildWarRankInfo
{
	GamePGCommandEnum			Command;
	int							PlayerDBID;
	GuildWarRankInfoStruct		Info;

	PG_BG_GuildWar_BLtoL_GuildWarRankInfo()
	{Command = EM_PG_BG_GuildWar_BLtoL_GuildWarRankInfo;}
};

// �Գ�401 -> Local
//���|�Ʀ���
struct PG_BG_GuildWar_LtoC_GuildWarRankInfo
{
	GamePGCommandEnum			Command;
	GuildWarRankInfoStruct		Info;

	PG_BG_GuildWar_LtoC_GuildWarRankInfo()
	{Command = EM_PG_BG_GuildWar_LtoC_GuildWarRankInfo;}
};
//////////////////////////////////////////////////////////////////////////
// �Գ�401 -> Local
//���|�Ʀ���
struct PG_BG_GuildWar_BLtoL_GuildWarHisotry
{
	GamePGCommandEnum			Command;
	int							PlayerDBID;
	GuildHouseWarHistoryStruct	Hisotry[50];

	PG_BG_GuildWar_BLtoL_GuildWarHisotry()
	{Command = EM_PG_BG_GuildWar_BLtoL_GuildWarHisotry;}
};

// �Գ�401 -> Local
//���|�Ʀ���
struct PG_BG_GuildWar_LtoC_GuildWarHisotry
{
	GamePGCommandEnum			Command;
	GuildHouseWarHistoryStruct	Hisotry[50];

	PG_BG_GuildWar_LtoC_GuildWarHisotry()
	{Command = EM_PG_BG_GuildWar_LtoC_GuildWarHisotry;}
};
//�C�X�Ҧ��C�|
//EM_PG_BG_GuildWar_CtoBL_GuildNameRequest,
//EM_PG_BG_GuildWar_BLtoL_GuildNameRequest,
//EM_PG_BG_GuildWar_LtoBL_GuildNameResult,
//EM_PG_BG_GuildWar_BLtoC_GuildNameResult,
//EM_PG_BG_GuildWar_CtoL_OpenMenuRequest,
//EM_PG_BG_GuildWar_LtoBL_OpenMenuRequest,
//EM_PG_BG_GuildWar_BLtoL_OpenMenuResult,
//EM_PG_BG_GuildWar_LtoC_OpenMenuResult,
//EM_PG_BG_GuildWar_CtoL_RegisterRequest,
//EM_PG_BG_GuildWar_LtoBL_RegisterRequest,
//EM_PG_BG_GuildWar_BLtoL_RegisterResult,
//EM_PG_BG_GuildWar_LtoC_RegisterResult,
//EM_PG_BG_GuildWar_BLtoL_Status,
//EM_PG_BG_GuildWar_LtoC_Status,
//EM_PG_BG_GuildWar_BLtoL_HouseLoadRequest,
//EM_PG_BG_GuildWar_LtoBL_HouseBaseInfo,
//EM_PG_BG_GuildWar_LtoBL_HouseBuildingInfo,
//EM_PG_BG_GuildWar_LtoBL_HouseFurnitureInfo,
//EM_PG_BG_GuildWar_LtoBL_HouseLoadOK,
//EM_PG_BG_GuildWar_CtoL_EnterRequest,
//EM_PG_BG_GuildWar_LtoBL_EnterRequest,
//EM_PG_BG_GuildWar_BLtoL_EnterResult,
//EM_PG_BG_GuildWar_LtoC_EnterResult,
//EM_PG_BG_GuildWar_CtoBL_LeaveRequest,
//EM_PG_BG_GuildWar_BLtoC_LeaveResult,
//EM_PG_BG_GuildWar_BLtoC_GuildScore,
//EM_PG_BG_GuildWar_BLtoC_BeginWar,
//EM_PG_BG_GuildWar_BLtoC_EndWar,
//////////////////////////////////////////////////////////////////////////
