/*	待做

1. 連線中斷線處置
2. 認證中斷線處置
3. 斷線發生中, Post_Send 錯誤及如何利用該項錯誤檢測斷線發生.

MapSocket 可以考慮用 vector 來實作, 加強效能, 因為使用 map 來查詢很沒效率

*/
#ifndef __NETBRIDGE_BASE_H__
#define __NETBRIDGE_BASE_H__

#if _DEBUG
#define NET_BRIDGE_BASE_BUFFER_CREATE_AND_DESTORY_DEBUG_PRINT 1
#define NET_BRIDGE_BASE_BUFFER_KEEP_POINTER 1
#define NET_BRIDGE_BASE_EVENT_CREATE_AND_DESTORY_DEBUG_PRINT 1
#else
#define NET_BRIDGE_BASE_BUFFER_CREATE_AND_DESTORY_DEBUG_PRINT 0
#define NET_BRIDGE_BASE_BUFFER_KEEP_POINTER 0
#define NET_BRIDGE_BASE_EVENT_CREATE_AND_DESTORY_DEBUG_PRINT 0
#endif

#include <map>
#include <vector>
#include <queue>
using namespace std;
#include "NetBridge_Struct.h"
#include "NetBridge_EventObj.h"
#include "BufferManger.h"
#include "NetBridge.h"

struct	SockSendRevInfoStruct
{
	LONG BytesSend;
	LONG BytesRevc;
	LONG PecketSend;
	LONG PecketRevc;
};

class CNetBridge_Base : public CNetBridge
{
public:
	CNetBridge_Base(void);
	virtual ~CNetBridge_Base(void);
	virtual int			Init(int iNumThread);
	virtual void		Release();
	virtual int			Connect(DWORD dwIP, WORD wPort, CEventObj* pEventObj, PVOID pContext);
	virtual int			Telnet(DWORD dwIP, WORD wPort, CEventObj* pEventObj, PVOID pContext) = 0;
	virtual void		Flush();
	virtual DWORD		Wait(DWORD dwTime);
	virtual void		GetConnectInfo(DWORD dwNetID, DWORD* pdwIP, WORD* pwPort);
	virtual void		SetOutPutCB(pfOutPutCB pFun) { m_pfOutPut = pFun; }
	virtual void		GetStatusInfo(char* pszOut);
	virtual void		GetRespondInfo(char* pszOut);
	virtual	bool		Shutdown();
	virtual HANDLE		GetMsgEvent() { return this->m_hMsgEvent; }
	virtual void		SetCheckRespond(bool bEnable);
	virtual void		SetKeepAlive(bool bEnable, DWORD dwTime);
	virtual void		SetSendBufferLimit(bool bEnable, DWORD dwLimit);
	void				MsgOutPut(int lv, char* format, ...);
	LONG				GetBytesSend();
	LONG				GetBytesRecv();
	LONG				GetPacketSend() { return m_PacketSend; }
	LONG				GetPacketRecv() { return m_PacketRecv; }
	DWORD				GetTotalSocket();
	void				ReSetSendCount();
	void				ReSetRecvCount();
	DWORD				GetAvgNewEvent() { return m_dwAvgNewEvent; }
	DWORD				GetAvgProcEvent() { return DWORD(((float)m_dwAvgProcEvent) * (((float)1000) / DF_NB_AVG_EVENT_MS_TIME)); }
	DWORD				GetQueueEvent() { return m_MsgQueue.Count(); }
	void				ReSetPacketSendCount() { InterlockedExchange(&m_PacketSend, 0); }
	void				ReSetPacketRecvCount() { InterlockedExchange(&m_PacketRecv, 0); }
	virtual	DWORD		GetMsgCount() { return (DWORD)m_vecMsg.size(); }
	void				CalSendRecvBytes();
	virtual LONG		GetBytesSend(int Second);	//Second 最多 3600 ==> 一小時
	virtual LONG		GetBytesRecv(int Second);	//Second 最多 3600 ==> 一小時
	virtual LONG		GetPacketsSend(int Second);	//Second 最多 3600 ==> 一小時
	virtual LONG		GetPacketsRecv(int Second);	//Second 最多 3600 ==> 一小時
	virtual	DWORD		GetClientRespond(DWORD dwNetID);
	virtual void		CheckMsg(vector< CEventMsg* >* pQueue);

	virtual void		CheckSocket();
	virtual void		CheckPacket();
	virtual void		SendFlush();

	virtual void		Authorize(CNetSocket* pSocket, CNetSocket* pAcceptSocket);

	virtual	bool		RemoveHost(DWORD dwNetID) { return false; }
	virtual CEventObj* ResetClientEvent(DWORD dwNetID, CEventObj* pEventObj) { return 0; }

	virtual CBuffer* CreatePacketBuffer(DWORD dwSize, PVOID pData, BYTE** pOffset);
	virtual bool		CheckCompress(CBuffer* pBuffer);

	virtual void		Post_Compress(CNetSocket* pSocket, CBuffer* pBuffer) = 0;
	virtual void		Post_Send(CNetSocket* pSocket) = 0;


	virtual void		SendConnect(CNetSocket* pSocket) = 0;
	virtual CBuffer* On_Compress(CBuffer* pBuffer) { return NULL; }
	virtual CBuffer* On_Uncompress(CBuffer* pBuffer) { return NULL; }
	virtual CBuffer* On_Encrypt(CBuffer* pBuffer) { return NULL; }
	virtual CBuffer* On_Decrypte(CBuffer* pBuffer) { return NULL; }
	static  DWORD		ConvertIP(const char* pszIP);

	CNetSocket* CreateSocket();
	void				DeleteSocket(DWORD dwNetID);
	inline CNetSocket* GetSocket(DWORD dwNetID);

	CEventMsg* CreateMsg(EM_MSG_TYPE emMsgType, CNetSocket* pSocket);
	void				DeleteMsg(CEventMsg* pMsg);

	void				MsgAccept(CNetSocket* pSocket);
	void				MsgConnect(CNetSocket* pSocket);
	void				MsgDisconnect(CNetSocket* pSocket);
	void				MsgRecv(CNetSocket* pSocket, DWORD dwSize, BYTE* pData, BYTE* pPacketData);
	void				MsgError(CNetSocket* pSocket, DWORD		dwErrorID);
	void				MsgAuthorize(CNetSocket* pSocket, CNetSocket* pAcceptSocket);
	void				MsgConnectFailed(CNetSocket* pSocket);
	void				MsgSystem(CNetSocket* pSocket, Struct_PacketHeader* pHeader, BYTE* pData, DWORD dwDataSize);

	void				PostMsg(CEventMsg* pMsg);

	CBuffer* CreateBuffer(DWORD dwSize);
	void				DeleteBuffer(CBuffer* lpBuffer);

	bool				GetPacketInfo(PVOID pData, DWORD pDataSize, DWORD* pFlag, DWORD* pSize);
	inline byte			GetCheckSum(BYTE* pSource, int iSize, byte Code, byte SourceCheckSum);

	virtual void		On_Error(char* format, ...);

	virtual void		SendAliveAnswer(CNetSocket* pSocket, DWORD dwSrvTick) = 0;
	virtual void		SendAliveRequest(CNetSocket* pSocket) = 0;
	volatile LONG							m_bytesSend;
	volatile LONG							m_bytesRecv;

	volatile LONG							m_PacketSend;
	volatile LONG							m_PacketRecv;

	int										m_vecSocketSize;
	CNetSocket** m_vecSocket;
	CNetQueue<DWORD>						m_unusedNetIDs;

	bool									m_bShutdown;
	multimap< DWORD, CNetSocket* >			m_mapClosingSocket;

	vector< CNetSocket* >					m_vecWaitCloseSocket;		// 等待所有待送訊息送出後 Close 的 Socket queue
	vector< CNetSocket* >					m_vecWaitDeleteSocket;		// 等待所有接收訊息處理後 Delete 的 Socket queue
	CCritcalsection							m_cs_Socket;		// 新增或刪除 Socket 用
	CCritcalsection							m_cs_MessageAndEvent;	// 新增或取出 Message / Event
	CCritcalsection							m_cs_buffer;
	CCritcalsection							m_cs_vecWaitCloseSocket;
	CCritcalsection							m_cs_EventMsgPool;
	DWORD									m_dwHost;			// 用來計數有多少的 Socket 在 Host 狀態, 也是用來計錄有多少 Event Waiting
	//WSAEVENT								m_EventHostArray[ DF_MAX_HOST_SOCKET ];
	HANDLE									m_hMsgEvent;
	bool									m_bEvent;
	CNetQueue< CEventMsg* >					m_vecMsg;
	CNetQueue< CEventMsg* >					m_MsgQueue;
	vector< CEventMsg* >					m_vecAuthorize;
	vector< CEventMsg* >					m_vecHoldMsg;			// 用來存放暫時不能處理的 Msg, 如還沒有設 Event 但已收到封包的 Msg
	vector< DWORD >							m_vecWaitSendSocket;	// 用來計錄那些 Socket 有資料待送
	CBufferManger							m_bufferManager;
	PFERRMSG								m_pfError;		// 用來輸出錯誤訊息的函式指標
	pfOutPutCB								m_pfOutPut;
	CNetQueue<CEventMsg*>					m_unusedEventMsgs;
	deque< SockSendRevInfoStruct >			m_SockSendRevInfo;
	LONG									m_LastCalSendRevTime;
	DWORD									m_dwAvgProcEvent;			// 平均每 200ms 處理的事件量
	DWORD									m_dwAvgNewEvent;		// 每秒新事件量
	DWORD									m_dwTimeLastGetEvent;
	bool									m_bCheckRespondTime;			// 是否開啟檢查反應時間機制
	DWORD									m_dwCheckRespondTime;			// 檢查反應時間的間隔時間, Invete for check respond time
	DWORD									m_dwKeepAliveTime;				// 反應時間低於一定值, 便斷線, 若設 0 則表示不斷線

	bool									m_bSendBufferLimit;				// flag of Check transfer, default is off
	DWORD									m_dwSendBuffLimit;				// number of  transfer limit, default is 4096k and respond time bigger than 3000ms
	DWORD									m_LastFlushTime;
	DWORD									m_minFlushTime;
	DWORD									m_maxFlushTime;

	DWORD									m_minEvent;
	DWORD									m_maxEvent;

	volatile LONG							m_SendCount;
	DWORD									m_minSendCountPerSec;
	DWORD									m_maxSendCountPerSec;

	volatile LONG							m_SendBytes;
	DWORD									m_minSendBytesPerSec;
	DWORD									m_maxSendBytesPerSec;

	volatile LONG							m_RecvCount;
	DWORD									m_minRecvCountPerSec;
	DWORD									m_maxRecvCountPerSec;

	volatile LONG							m_RecvBytes;
	DWORD									m_minRecvBytesPerSec;
	DWORD									m_maxRecvBytesPerSec;

	volatile LONG							m_Respond[10];				// 以 100 Ping 為單位的差別

	DWORD									m_minSecSendThread;
	DWORD									m_maxSecSendThread;


	bool									m_CheckRecvEvent;

#if NET_BRIDGE_BASE_BUFFER_KEEP_POINTER
	std::vector<CNetSocket*>    m_vecNetSocket;
	std::vector<std::string>    m_vecNetSocketName;
	std::vector<CEventMsg*>     m_vecEventMsg;
	std::vector<std::string>    m_vecEventMsgName;
#endif

};

inline CNetSocket* CNetBridge_Base::GetSocket(DWORD dwNetID)
{
	CNetSocket* pSocket = NULL;

	m_cs_Socket.Enter("CNetBridge_Base::GetSocket");
	{
		if (dwNetID < static_cast<DWORD>(m_vecSocketSize))
		{
			pSocket = m_vecSocket[dwNetID];
		}
	}
	m_cs_Socket.Leave("CNetBridge_Base::GetSocket");

	return pSocket;
}


#endif //__NETBRIDGE_BASE_H__
