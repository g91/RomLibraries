#pragma once
#pragma warning (disable:4949)
#include <windows.h>
#include "NetWakerPGEnum.h"
#pragma unmanaged
//----------------------------------------------------------------------------------------
// ServerList -> Server
// 要求 Server 傳回其辨視資料

struct PG_SL_SL2SL_RequestServerData
{
	PACKET_CONSTRUCT( PG_SL_SL2SL_RequestServerData );
};

//----------------------------------------------------------------------------------------
// Server -> Master's ServerList
// Server 收到 Master's ServerList EM_PG_SL_SL2S_RequestServerData 要求後, 傳回本身資料


struct PG_SL_SL2SL_RegsiterServerData
{
	PACKET_CONSTRUCT( PG_SL_SL2SL_RegsiterServerData );

	EM_SERVER_TYPE			m_ServerType;						// Server 所代表的類型
	DWORD					m_dwServerLocalID;					// 外部 Server 真實視別代號
	bool					m_bReg;								// 是否曾經和 Master 註冊過
};

//----------------------------------------------------------------------------------------
// Master's ServerList -> Server
// Master's ServerList 收到 PG_ServerList_S2SL_ServerData 要求後, 認可其登錄. 將其資料轉送給其它 Server

struct PG_SL_SL2SL_TransmitServerData
{
	PACKET_CONSTRUCT( PG_SL_SL2SL_TransmitServerData );

	EM_SERVER_TYPE			m_ServerType;						// Server 所代表的類型
	DWORD					m_dwServerLocalID;					// 外部 Server 真實視別代號
	DWORD					m_dwServerID;						// 外部 Server 真實網路代號
};

//----------------------------------------------------------------------------------------
// ServerList -> Server
// ServerList 收到 PG_ServerList_S2SL_ServerData 要求後, 發現其資料重覆或有問題. 不允許登入
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

	EM_SERVER_TYPE			m_ServerType;			// Server 所代表的類型
	DWORD					m_dwServerLocalID;		// 外部 Server 真實視別代號
};

struct PG_SL_SL2SL_Shutdown
{
	PACKET_CONSTRUCT( PG_SL_SL2SL_Shutdown );

	int						iCloseTime;				// 以 5min 為單位, 收到值後, 各 Srv 各自處理, 比方說.  ZoneSrv 收到 1, 表最後 5min. 需要每 1min 都公告
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