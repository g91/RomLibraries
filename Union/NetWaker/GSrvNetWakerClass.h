#ifndef __GSRVNETWAKERMAINCLASS_H__
#define __GSRVNETWAKERMAINCLASS_H__

#include "NetBridge/NetBridge.h"

#include "NetWakerPackage.h"
#include "NetWakerDefine.h"
#include "functionschedular/functionschedular.h"

#include "RecycleBin/RecycleBin.h"
#include "SmallObj/SmallObj.h"

#include "DebugLog/errormessage.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <functional>

class GSrvNetWakerClass;
using namespace std;
using namespace NetWakerPackage;
//---------------------------------------------------------------------------------
typedef void  (GSrvNetWakerClass::*NetPGCallBack) ( int	NetID , PGSysNULL* PG , int Size );

class LBSwitch: public CEventObj
{
    GSrvNetWakerClass	*_Parent;
public:
    LBSwitch( GSrvNetWakerClass* value ) { _Parent = value; };
    virtual bool		OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData );
    virtual void		OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode );
    virtual void		OnConnect		( DWORD dwNetID );
    virtual	void		OnDisconnect	( DWORD dwNetID );
    virtual CEventObj*	OnAccept		( DWORD dwNetID );
};
//---------------------------------------------------------------------------------

class LBProxy: public CEventObj
{
    GSrvNetWakerClass	*_Parent;
public:
    LBProxy( GSrvNetWakerClass* value ) { _Parent = value; };
    virtual bool		OnRecv			( DWORD dwNetID, DWORD dwPackSize, PVOID pPacketData );
    virtual void		OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode );
    virtual void		OnConnect		( DWORD dwNetID );
    virtual	void		OnDisconnect	( DWORD dwNetID );
    virtual CEventObj*	OnAccept		( DWORD dwNetID );
};
//---------------------------------------------------------------------------------
/*
struct GameItemExchangeInfoStruct
{
	int Type;
	int ItemID;
	int Count;
	int ExValue;
	int Inherent;
	int Rune[4];
	int MainColor;
	int OffColor;
	int Mode;
	int ImageObjID;
	int Money;
	int Money_Account;
};
*/
//#define ItemExchangeResultFunctionBase         boost::function<void ( char* UserAccount , int RoleGUID  , GameItemExchangeInfoStruct& Info ) >
//---------------------------------------------------------------------------------
class GSrvNetWakerClass
{
public:
	static bool		G_SwitchPacketTimeProcLog;
protected:
    //friend	LSwitch;	
    friend	LBSwitch;	
    friend	LBProxy;

//    static char _TempSendData[ 0x40000 ];

    NewWakerEventListener*              _Listener;
    FunctionSchedularClass*             _Schedular;


    MyVector<GSrv_CliInfoStruct >       _CliList;

	int						_WorldID;
    int                     _GSrvID;
    char                    _GSrvName[ _Max_GSrv_Name_Size_ ];
    SYSGSrvTypeENUM         _Type;                              //聯線模式


    CNetBridge*			    _NetBSwitch;			            //主要是用連接Switch
    CNetBridge*			    _NetBProxy;			                //主要是用連接Proxy
    LBSwitch*               _SwitchBListener;
    LBProxy* 		        _ProxyBListener;                     //資料列舉Class

    int                     _NetSwitchID;

    string                  _SwitchAddrIP;
    int                     _SwitchAddrPort;

    set<int>			    _ProxyConnect;	                    //GSrv 與哪些Proxy有作連結
    Map_NetIDtoIDClass      _ProxyNetToIDMap;
	short					_ClientDataKey;						//存取Client端的Key
    

    Map_StrAndID            _GSrvNameList;                      //Server 名稱與ID轉換的Map

    bool                    _ExitFlag;                          //結束旗標
    bool                    _IsReady;                           //登入ok

    vector<PGBaseInfo>	    _PGInfo;			                //封包管理的資訊

    NetPGCallBack		    _PGCallBack[ EM_SysNet_Packet_Count ];

    void	_OnSwitchConnected       ( int NetID );
    void	_OnSwitchDisconnected    ( int NetID );
    bool	_OnSwitchPacket          ( DWORD dwNetID, DWORD Size , PVOID Data );
    void	_OnSwitchConnectFailed   ( int NetID );

    void	_OnProxyConnected       ( int NetID );
    void	_OnProxyDisconnected    ( int NetID );
    bool	_OnProxyPacket          ( DWORD NetID , DWORD dwPackSize, PVOID pPacketData );
    void	_OnProxyConnectFailed   ( int NetID );

    //-----------------------------------------------------------------------------------
    // 封包Call Back 處理
    //-----------------------------------------------------------------------------------
    //(Switch)
    void _PGxUnKnoew( int NetID , PGSysNULL* PG , int Size );
    // 連結到Switch 要求連結
    void _PGxSwitchConnectNodify( int NetID , PGSysNULL* PG , int Size );
    //通知Proxy Addr並且要求連結
    void _PGxProxyAddr( int NetID , PGSysNULL* PG , int Size );
    //要求GSrv與某Proxy斷線
    void _PGxProxyDisconnectRequest( int NetID , PGSysNULL* PG , int Size );
    //Switch 通知 GSrv有 新的GSrv成立連線
    void _PGxOnNewGSrvConnect( int NetID , PGSysNULL* PG , int Size );
    //Switch 通知 GSrv有 新的GSrv斷線
    void _PGxOnGSrvDisConnect( int NetID , PGSysNULL* PG , int Size );
    //GSrv轉送過來的資料
	void _PGxDataGSrvToGSrv( int NetID , PGSysNULL* PG , int Size );
	void _PGxDataGSrvToGSrv_Zip( int NetID , PGSysNULL* PG , int Size );
	//封包轉送到另外的Server
	void _PGDataTransmitToWorldGSrv( int NetID , PGSysNULL* PG , int Size );
	//封包轉送到另外的Server
	void _PGDataTransmitToWorldGSrv_GSrvID( int NetID , PGSysNULL* PG , int Size );
    
    //-----------------------------------------------------------------------------------
    //(Proxy)
    // 連結到Proxy 成�
    void _PGxProxyConnectNotify( int NetID , PGSysNULL* PG , int Size );

    //與 GSrv成立連線
    void _PGxCliOnConnect	( int NetID , PGSysNULL* PG , int Size );
    //與 GSrv斷線
    void _PGxCliOnDisonnect	( int NetID , PGSysNULL* PG , int Size );
    //Client 轉送過來的資料
    void _PGxDataCliToGSrv	( int NetID , PGSysNULL* PG , int Size );
		
	void _PGxPingLog		( int NetID , PGSysNULL* PG , int Size );
    //-----------------------------------------------------------------------------------
    //重新連結 Switch Server
    static int  _ReConnectSwitch( SchedularInfo* Obj , void* InputClass );
    //-----------------------------------------------------------------------------------
    void _ConnectProxy( char* IpStr ,int Port );

public:

    GSrvNetWakerClass( );
    virtual ~GSrvNetWakerClass();

    void	ConnectSwitch( const char* IP,int Port );

    //設定 GSrv 的名稱
    void    SetGSrvName( const char* Name );

    //定時處理
    bool	Process( );

    //登出
    bool    Logout( );

    void    SetListener( NewWakerEventListener* Listener );

    bool    SrvLogout( int SrvID );

	void	SendFlush();
    //-----------------------------------------------------------------------------------

    void    ConnectType( SYSGSrvTypeENUM Type );
    //讓某 cli 與 某 GSrv 成立新的連線
    bool	ConnectCliAndSrv( int GSrvID , int CliID );	
    //與 某個 Client 斷線
    bool	CliDisconnect( int CliID );		
    //要求把某個Client logout
    void	CliLogout( int CliID, const char* pszReason );
    //取得某 Cli 的連線資料
    GSrv_CliInfoStruct*    GetCliNetInfo( int CliID ) ;

    //-----------------------------------------------------------------------------------
    //送資料到 Switch
    bool    SendToSwitch( int Size , const void* Data );
	bool    SendToSwitch( int Size1 , const void* Data1 , int Size2 , const void* Data2 );

    bool    SendToProxy( int ProxyID , int Size , const void* Data );
    bool    SendToProxy( int ProxyID , int Size1 , const void* Data1 , int Size2 , const void* Data2 );

    bool    SendToProxy_NETID( int NetID , int Size , const void* Data );
	void	SendToAllProxy( int Size , const void* Data );

    bool    SendToGSrv( int GSrvID , int Size , const void* Data );
    bool    SendToAllGSrv( int Size , const void* Data );

    bool    SendToCli( int SockID , int Size , const void* Data );
    bool    SendToCli( int SockID , int ProxyID , int Size , const void* Data );
	bool	SendToCli_ClientKey( int SockID , short CliKeyID );

	//送給所有連上此Server的Client
    void    SendToAllCli( int Size , const void* Data );
	//送給所有玩家
	void    SendToAllPlayer( int Size , const void* Data );

	bool	SendToAccount( int Size , const void* Data );

	//送資料到所有的世界
	void	SendToAllOtherWorldSrv( int WorldID , int Size , const void* Data );
	//送資料到所有的世界
	void	SendToOtherWorldSrv( int ToWorldID , int ToNetID , int Size , const void* Data );
	//送資料到所有的世界
	void	SendToOtherWorldSrv_GSrvID( int ToWorldID  , int ToGSrvType , int ToGSrvID, int Size , const void* Data );

	//註冊GSrv NetID
	void	RegGSrvNetID( int GSrvType , int GSrvID );

	void	ProxyPingLog(  );

	//把某帳號冷凍
	void	FreezeAccount( const char* Account , int FreezeType );

	//註冊Server的loading 狀況
	bool	RegServerLoading		( int MaxPlayerCount , int	ParallelZoneCount , int PlayerCount[100] , float TotalPlayerCount );


	unsigned int GetIpNum( int SockID );

	//取得排程結構
	FunctionSchedularClass*  Schedular() { return _Schedular; };

	int		NetID()	{ return _GSrvID; }
    //-----------------------------------------------------------------------------------
    // 初始化 Client 與 Server 的 CallBack函式
    //-----------------------------------------------------------------------------------
    void	InitPGCallBack();

	//-----------------------------------------------------------------------------------
	//  網路核心的訊息輸出
	//-----------------------------------------------------------------------------------
	static void		Output_NetMsg			( int lv, const char* szOut );
	static int		Output_NetStatus		( SchedularInfo* Obj , void* InputClass );

	//取得netbirdge資料
	CNetBridge*			    NetBSwitch(){ return _NetBSwitch; };			            //主要是用連接Switch
	CNetBridge*			    NetBProxy(){ return _NetBProxy;  };			                //主要是用連接Proxy

	int		GetWorldID()					{ return _WorldID; }

};


#endif //__NETWAKERCLASS_H__