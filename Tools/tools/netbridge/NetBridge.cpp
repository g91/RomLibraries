#pragma warning (disable:4949)
#pragma unmanaged

#ifndef _NB_USE_OLD_ENGINE
#include "NetBridge_IOCP.h"
#include "NetBridge_ASIO.h"
#else
#include "NetBridge_OLDENGINE.h"
#endif

HANDLE CNetBridge::m_pNetEventHandle[MAXIMUM_WAIT_OBJECTS];
DWORD CNetBridge::m_dwNetHandleIndex = 0;
CNetBridge* CNetBridge::m_pNetPoint[MAXIMUM_WAIT_OBJECTS];
HANDLE CNetBridge::m_pNetMainEvent = NULL;
vector< CNetBridge* >* CNetBridge::m_pvecCore = NULL;
vector< CNetBridge* >* CNetBridge::m_pvecNewCore = NULL;
DWORD CNetBridge::m_refCount = 0;

CNetBridge* g_pNetEngineCore = NULL;
CNetBridge* g_pNetEngineAsioCore = NULL;
DWORD g_dwNetEngineRefCount = 0;

#ifdef DF_PACKET_LOG
DWORD CNetBridge::m_dwPacketLog = 0;
#endif

CNetBridge::CNetBridge()
{
	m_dwEnterTime = GetTickCount();

	if (m_pNetMainEvent == NULL)
		m_pNetMainEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CNetBridge::~CNetBridge()
{
	if (m_pNetMainEvent != NULL)
	{
		CloseHandle(m_pNetMainEvent);
		m_pNetMainEvent = NULL;
	}
}

CNetBridge* CreateNetBridge(bool bManage, int iNumThread)
{
	CNetBridge* pBridge = (CNetBridge*)new CNetBridge_IOCP;

	if (CNetBridge::m_pvecCore == NULL)
	{
		CNetBridge::m_pvecCore = new vector< CNetBridge* >;

	}

	if (CNetBridge::m_pvecNewCore == NULL)
	{
		CNetBridge::m_pvecNewCore = new vector< CNetBridge* >;

	}

	{
		CNetBridge::m_pvecNewCore->push_back(pBridge);
	}

	pBridge->Init(1);
	return pBridge;
}

CNetBridge* CreateAsio(bool bManage)
{
	{
		g_dwNetEngineRefCount++;

		if (g_pNetEngineAsioCore == NULL)
		{
			g_pNetEngineAsioCore = (CNetBridge*)new CNetBridge_ASIO;
			g_pNetEngineAsioCore->Init(2);
			g_pNetEngineAsioCore->m_refCount++;
		}
	}

	return g_pNetEngineAsioCore;
}

void DeleteNetBridge(CNetBridge* pBridge)
{
	if (pBridge == NULL)
		return;

	CNetBridge::WaitAll(10);

	if (CNetBridge::m_pvecCore != NULL)
	{
		for (vector< CNetBridge* >::iterator it = CNetBridge::m_pvecCore->begin(); it != CNetBridge::m_pvecCore->end(); it++)
		{
			if ((*it) == pBridge)
			{
				CNetBridge::m_pvecCore->erase(it);

				if (CNetBridge::m_pvecCore->size() == 0)
				{
					delete CNetBridge::m_pvecCore;
					CNetBridge::m_pvecCore = NULL;
				}

				break;
			}
		}
	}

	if (CNetBridge::m_pvecNewCore != NULL)
	{
		delete CNetBridge::m_pvecNewCore;
		CNetBridge::m_pvecNewCore = NULL;
	}

	pBridge->Shutdown();
	delete pBridge;
}

DWORD CNetBridge::WaitAll(DWORD dwTime)
{
	DWORD dwTimeNow = GetTickCount();

	if (m_pvecNewCore != NULL)
	{
		for (vector< CNetBridge* >::iterator it = CNetBridge::m_pvecNewCore->begin(); it != CNetBridge::m_pvecNewCore->end(); it++)
		{
			CNetBridge::m_pvecCore->push_back(*it);
		}
		CNetBridge::m_pvecNewCore->clear();
	}

	DWORD dwReturn = WaitForSingleObject(CNetBridge::m_pNetMainEvent, dwTime);

	if (dwReturn != WAIT_TIMEOUT)
	{
		for (vector< CNetBridge* >::iterator it = CNetBridge::m_pvecCore->begin(); it != CNetBridge::m_pvecCore->end(); it++)
		{
			CNetBridge_IOCP* pNetBridge = (CNetBridge_IOCP*)*it;

			if (pNetBridge->m_bEvent == true)
			{
				pNetBridge->m_bEvent = false;
				pNetBridge->m_dwEnterTime = dwTimeNow;
				pNetBridge->Flush();

			}
		}
	}

	for (vector< CNetBridge* >::iterator it = CNetBridge::m_pvecCore->begin(); it != CNetBridge::m_pvecCore->end(); it++)
	{
		CNetBridge_IOCP* pNetBridge = (CNetBridge_IOCP*)*it;

		if ((dwTimeNow - pNetBridge->m_dwEnterTime) >= 20)
		{
			pNetBridge->m_dwEnterTime = dwTimeNow;
			pNetBridge->Flush();
		}
	}

	return dwReturn;
}

DWORD	ConvertIP(const char* pszIP)
{
	u_long IP_ADDRESS;

	if (pszIP != NULL)
	{
		if (strlen(pszIP) != 0)
		{

			//測試輸入的IP是IP Address 或是 DNS

			//轉換 IP
			IP_ADDRESS = inet_addr(pszIP);

			//測試轉出結果, 若傳回 INADDR_NONE 的話, 表該 IP 可能是 DNS
			if (IP_ADDRESS == INADDR_NONE)
			{
				// Host isn't an IP address, try using DNS
				hostent* HE = gethostbyname(pszIP);

				if (HE == 0)
				{
					//Error can't check DNS
					return 0;
				}

				IP_ADDRESS = *((u_long*)HE->h_addr_list[0]);
			}
		}
	}

	return IP_ADDRESS;
}

void ConvertIPtoString(DWORD dwIP, char* pszIP)
{
	BYTE* pIP = (BYTE*)&dwIP;
	if (pszIP != NULL)
	{
		sprintf(pszIP, "%d.%d.%d.%d", *pIP, *(pIP + 1), *(pIP + 2), *(pIP + 3));
	}
}

#pragma managed