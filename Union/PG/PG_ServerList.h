#pragma once
#pragma warning (disable:4949)
#include <windows.h>
#include "NetWakerPGEnum.h"
#pragma unmanaged
//----------------------------------------------------------------------------------------
// ServerList -> Server
// �n�D Server �Ǧ^�������

struct PG_SL_SL2SL_RequestServerData
{
	PACKET_CONSTRUCT( PG_SL_SL2SL_RequestServerData );
};

//----------------------------------------------------------------------------------------
// Server -> Master's ServerList
// Server ���� Master's ServerList EM_PG_SL_SL2S_RequestServerData �n�D��, �Ǧ^�������


struct PG_SL_SL2SL_RegsiterServerData
{
	PACKET_CONSTRUCT( PG_SL_SL2SL_RegsiterServerData );

	EM_SERVER_TYPE			m_ServerType;						// Server �ҥN������
	DWORD					m_dwServerLocalID;					// �~�� Server �u����O�N��
	bool					m_bReg;								// �O�_���g�M Master ���U�L
};

//----------------------------------------------------------------------------------------
// Master's ServerList -> Server
// Master's ServerList ���� PG_ServerList_S2SL_ServerData �n�D��, �{�i��n��. �N������e���䥦 Server

struct PG_SL_SL2SL_TransmitServerData
{
	PACKET_CONSTRUCT( PG_SL_SL2SL_TransmitServerData );

	EM_SERVER_TYPE			m_ServerType;						// Server �ҥN������
	DWORD					m_dwServerLocalID;					// �~�� Server �u����O�N��
	DWORD					m_dwServerID;						// �~�� Server �u������N��
};

//----------------------------------------------------------------------------------------
// ServerList -> Server
// ServerList ���� PG_ServerList_S2SL_ServerData �n�D��, �o�{���ƭ��ЩΦ����D. �����\�n�J
enum EM_REG_SRV_FAILED
{
	EM_REG_SRV_FAILED_LOCALIDINUSE		= 1,
	EM_REG_SRV_FAILED_SERVERTYPEINUSE	,
};

struct PG_SL_SL2SL_RegSrvDataFailed
{
	PACKET_CONSTRUCT( PG_SL_SL2SL_RegSrvDataFailed );

	EM_REG_SRV_FAILED	emRegSrvDataFailed;
};

struct PG_SL_SL2SL_LocalID
{
	PACKET_CONSTRUCT( PG_SL_SL2SL_LocalID );

	DWORD					m_dwLocalID;
};

struct PG_SL_SL2SL_ServerReady
{
	PACKET_CONSTRUCT( PG_SL_SL2SL_ServerReady );

	EM_SERVER_TYPE			m_ServerType;			// Server �ҥN������
	DWORD					m_dwServerLocalID;		// �~�� Server �u����O�N��
};

struct PG_SL_SL2SL_Shutdown
{
	PACKET_CONSTRUCT( PG_SL_SL2SL_Shutdown );

	int						iCloseTime;				// �H 5min �����, ����ȫ�, �U Srv �U�۳B�z, ��軡.  ZoneSrv ���� 1, ��̫� 5min. �ݭn�C 1min �����i
};

struct PG_SL_S2M_RegisterAccountEvent
{
	PACKET_CONSTRUCT( PG_SL_S2M_RegisterAccountEvent );
};

struct PG_SL_M2S_AccountEnterWorld
{
	PACKET_CONSTRUCT( PG_SL_M2S_AccountEnterWorld );
	StaticString<32>		Account;
	int						iClientID;
};

struct PG_SL_M2S_AccountLeaveWorld
{
	PACKET_CONSTRUCT( PG_SL_M2S_AccountLeaveWorld );
	StaticString<32>		Account;
	int						iClientID;
};

#pragma managed