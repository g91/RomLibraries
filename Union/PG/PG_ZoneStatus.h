#pragma once

#include "NetWakerPGEnum.h"

//----------------------------------------------------------------------------------------
struct PG_ZoneStatus_L2M_RoleEnterZone
{
	PACKET_CONSTRUCT( PG_ZoneStatus_L2M_RoleEnterZone );

	int						iDBID;			// 人物 DBID
	int						iLocalID;		// 人物於 LocalServer 上的代號
};
//----------------------------------------------------------------------------------------
struct PG_ZoneStatus_L2M_RoleLeaveZone
{
	PACKET_CONSTRUCT( PG_ZoneStatus_L2M_RoleLeaveZone );

	int						iDBID;			// 人物 DBID
	int						iLocalID;		// 人物於 LocalServer 上的代號
};
//----------------------------------------------------------------------------------------
enum EM_RoleLeaveWorld
{
	EM_RoleLeaveWorld_Disconnect	= 0,
	EM_RoleLeaveWorld_Logout		= 1,
};

struct PG_ZoneStatus_L2M_RoleLeaveWorld
{
	PACKET_CONSTRUCT( PG_ZoneStatus_L2M_RoleLeaveWorld );

	int						iDBID;			// 人物 DBID
	int						iLocalID;		// 人物於 LocalServer 上的代號
	EM_RoleLeaveWorld		emEvent;
	int						iExpToNextLevel;	// 到下一級所需的 EXP
};
//----------------------------------------------------------------------------------------
// 通知 Zone Server 註冊
struct PG_ZoneStatus_M2L_RequestZoneSrvReg
{
	PACKET_CONSTRUCT( PG_ZoneStatus_M2L_RequestZoneSrvReg );
};

//----------------------------------------------------------------------------------------
//	Local -> Master
#define     DF_NO_ZONEITEMID	-1
//Local Server 向 Master 注冊
struct PG_ZoneStatus_L2M_ZoneSrvReg
{
	PACKET_CONSTRUCT( PG_ZoneStatus_L2M_ZoneSrvReg );

    int						iZoneID;		
    int                     iZoneItemID;
	int						iRemainCounter;		// 後面還有多少 ID 要註冊
};
//----------------------------------------------------------------------------------------
//	Local -> DataCenter
struct PG_ZoneStatus_L2D_RoleLeaveWorld
{
	PACKET_CONSTRUCT( PG_ZoneStatus_L2D_RoleLeaveWorld );

	StaticString< _MAX_ACCOUNT_SIZE_ >		szAccountName;
	StaticString< _MAX_NAMERECORD_SIZE_ >	szRoleName;
	int										iRoleDBID;
	int										iWorldID;
	int										iZoneID;
	int										iRace;
	int										iVoc;
	int										iVoc_Sub;
	int										iSex;
	int										iLV;
	int										iLV_Sub;
};

//----------------------------------------------------------------------------------------
//	Local -> DataCenter
struct PG_ZoneStatus_L2D_RoleEnterZone
{
	PACKET_CONSTRUCT( PG_ZoneStatus_L2D_RoleEnterZone );

	StaticString< _MAX_ACCOUNT_SIZE_ >	szAccount;
	int									iDBID;
	int									iZoneID;
	int									iIsLogin;
};

//----------------------------------------------------------------------------------------
//Local Server 向 Master 注冊
enum EM_ZoneSrvRegResult
{
    EM_ZoneSrvRegResult_OK					= 0,
	EM_ZoneSrvRegResult_OUT_ZONEITEMID		,
    EM_ZoneSrvRegResult_ZONEID_IN_USE		,
	EM_ZoneSrvRegResult_MasterSrvInitDone	,

    EM_ZoneSrvRegResult_MAX				= 0xffffffff,
};


struct PG_ZoneStatus_M2L_ZoneSrvRegResult
{
    PACKET_CONSTRUCT( PG_ZoneStatus_M2L_ZoneSrvRegResult );

	EM_ZoneSrvRegResult		emResult;
    int                     iZoneItemID;        //一個不重覆的流水號(用來件立唯一ID用的)
};
//----------------------------------------------------------------------------------------
// 通知 Master Local Server 準備好了
struct PG_ZoneStatus_L2M_ZoneSrvOkay
{
	PACKET_CONSTRUCT( PG_ZoneStatus_L2M_ZoneSrvOkay );
	
	int						iZoneID;
};
//----------------------------------------------------------------------------------------
struct PG_ZoneStatus_M2L_KickZonePlayer
{
	PACKET_CONSTRUCT( PG_ZoneStatus_M2L_KickZonePlayer );
	
	//char					szAccountName[ _MAX_ACCOUNT_SIZE_ ];
	StaticString< _MAX_ACCOUNT_SIZE_ >		szAccountName;
};
//----------------------------------------------------------------------------------------
struct PG_ZoneStatus_L2M_KickZonePlayerResult
{
	PACKET_CONSTRUCT( PG_ZoneStatus_L2M_KickZonePlayerResult );

	StaticString< _MAX_ACCOUNT_SIZE_ >		szAccountName;
	int										iResult;
	int										iZoneID;
};



