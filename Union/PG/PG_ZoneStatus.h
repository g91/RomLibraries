#pragma once

#include "NetWakerPGEnum.h"

//----------------------------------------------------------------------------------------
struct PG_ZoneStatus_L2M_RoleEnterZone
{
	PACKET_CONSTRUCT( PG_ZoneStatus_L2M_RoleEnterZone );

	int						iDBID;			// �H�� DBID
	int						iLocalID;		// �H���� LocalServer �W���N��
};
//----------------------------------------------------------------------------------------
struct PG_ZoneStatus_L2M_RoleLeaveZone
{
	PACKET_CONSTRUCT( PG_ZoneStatus_L2M_RoleLeaveZone );

	int						iDBID;			// �H�� DBID
	int						iLocalID;		// �H���� LocalServer �W���N��
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

	int						iDBID;			// �H�� DBID
	int						iLocalID;		// �H���� LocalServer �W���N��
	EM_RoleLeaveWorld		emEvent;
	int						iExpToNextLevel;	// ��U�@�ũһݪ� EXP
};
//----------------------------------------------------------------------------------------
// �q�� Zone Server ���U
struct PG_ZoneStatus_M2L_RequestZoneSrvReg
{
	PACKET_CONSTRUCT( PG_ZoneStatus_M2L_RequestZoneSrvReg );
};

//----------------------------------------------------------------------------------------
//	Local -> Master
#define     DF_NO_ZONEITEMID	-1
//Local Server �V Master �`�U
struct PG_ZoneStatus_L2M_ZoneSrvReg
{
	PACKET_CONSTRUCT( PG_ZoneStatus_L2M_ZoneSrvReg );

    int						iZoneID;		
    int                     iZoneItemID;
	int						iRemainCounter;		// �᭱�٦��h�� ID �n���U
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
//Local Server �V Master �`�U
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
    int                     iZoneItemID;        //�@�Ӥ����Ъ��y����(�Ψӥ�߰ߤ@ID�Ϊ�)
};
//----------------------------------------------------------------------------------------
// �q�� Master Local Server �ǳƦn�F
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



