#pragma once
#pragma warning (disable:4786)

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <functional>

#include "GSrvNetWakerClass.h"

//***********************************************************************************
// 定義回Call 函式
// 用來註冊函式的指標 ， 如果有事件發生就會呼叫登錄的函式
/*
typedef void (*OnLoginFunctionBase)			( void* , string info );
typedef void (*OnLogoutFunctionBase)		( void* );
typedef void (*OnLoginFailFunctionBase)		( void* , SYSLogFailedENUM );	

typedef void (*OnSrvConnectFunctionBase)	( void* , int id , string GSrvName );	
typedef void (*OnSrvDisconnectFunctionBase)	( void* , int id );	
typedef void (*OnSrvPacketFunctionBase)		( void* , int id , int size , void* data);	

typedef void (*OnCliConnectFunctionBase)	( void* , int id , string accountname );	
typedef void (*OnCliDisconnectFunctionBase)	( void* , int id );	
typedef void (*OnCliPacketFunctionBase)		( void* , int id , int size , void* data);	
typedef void (*OnSrvMsgFunctionBase)		( void* , char* msg);	
*/
#define OnLoginFunctionBase         boost::function<void ( string info ) >
#define OnLogoutFunctionBase        boost::function<void ( ) >
#define OnLoginFailFunctionBase     boost::function<void ( SYSLogFailedENUM ) >

#define OnSrvConnectFunctionBase    boost::function<void ( int id , string GSrvName ) >
#define OnSrvDisconnectFunctionBase boost::function<void ( int id ) >
#define OnSrvPacketFunctionBase     boost::function<void ( int id , int size , void* data) >
#define OnOtherWorldPacketFunctionBase     boost::function<void ( int worldid , int netid , int size , void* data) >

#define OnCliConnectFunctionBase    boost::function<void ( int id , string accountname ) >
#define OnCliDisconnectFunctionBase boost::function<void ( int id ) >
#define OnCliPacketFunctionBase     boost::function<void ( int id , int size , void* data ) >
#define OnSrvMsgFunctionBase			boost::function<void ( char* msg) >
#define OnClientPacketCrashOutputBase	boost::function<void ( int id , int size , PacketBase* data) >
#define OnProxyPingLogFunctionBase		boost::function<void ( int , int ) >
//-----------------------------------------------------------------------------------
class	GSrvNetWaker;
//CallBack class 設定
class NetListener: public NewWakerEventListener
{
    GSrvNetWaker* _Parent;
public:
    NetListener( GSrvNetWaker* value ){ _Parent = value; };
    
    void	OnLogin				( string info );
    void	OnLogout			( );
    void	OnLoginFailed		( SYSLogFailedENUM type );
    

    void    OnGSrvConnected     ( int id , string GSrvName);
    void    OnGSrvDisconnected  ( int id );
    bool    OnGSrvPacket        ( int id, int size , void* data );	
	//其他World送來的封包
	bool	OnOtherWorldPacket( int FromWorldID , int FromNetID , int size , void* data );	


    void	OnClientConnected	( int id , string accountname );
    void	OnClientDisconnected( int id );
    bool	OnCliPacket			( int id , int size , void* data );
    void	OnServerMsg			( char* Msg); 

	void	OnProxyPingLog		( int ProxyID , int Time );
};
//-----------------------------------------------------------------------------------
//Client 端來的封包資料管理結構
struct CallBackStruct
{
    OnCliPacketFunctionBase		Fun;	//CallBack函式
    int							Max;	//最大封包
    int							Min;	//最小封包

    CallBackStruct()
    {
        Fun = NULL;
        Max = 0xffff;
        Min = 0;			 
    };
};
//-----------------------------------------------------------------------------------
class	GSrvNetWaker
{
    friend  NetListener;

    GSrvNetWakerClass*  	    _Net;
    NetListener*                _Listener;
    std::map< int , string >    _SrvName;
   //-----------------------------------------------------------------------------------
    //執行狀態
    //-----------------------------------------------------------------------------------
    int	    _ExitFlag;

    int     _ProcTime;
	
	bool	_IsEnableTryException;

	int		_GSrvType;			//處理世界轉送封包註冊
	int		_GSrvID;
    //------------------------------------------------------------------------------------------
    //紀錄登入事件函式的資料
    vector< OnLoginFunctionBase >			    _LoginFunctionList;
    vector< OnLogoutFunctionBase >			    _LogoutFunctionList;
    vector< OnLoginFailFunctionBase >		    _LoginFailFunctionList;

    vector< OnSrvConnectFunctionBase >		    _SrvConnectFunctionList;
    vector< OnSrvDisconnectFunctionBase >	    _SrvDisconnectFunctionList;
    vector< OnSrvPacketFunctionBase >		    _SrvPacketFunctionList;
	vector< OnOtherWorldPacketFunctionBase >	_OtherWorldPacketFunctionList;

    vector< OnCliConnectFunctionBase >		    _CliConnectFunctionList;
    vector< OnCliDisconnectFunctionBase >	    _CliDisconnectFunctionList;
    vector< CallBackStruct > 				    _CliPacketFunctionList;
    vector< OnSrvMsgFunctionBase >			    _SrvMsgFunctionList;
	vector< OnClientPacketCrashOutputBase >		_OnClientPacketCrashOutput;

	OnProxyPingLogFunctionBase					_ProxyPingLogFunc;

    void	_OnLogin				( string info );
    void	_OnLogout				( );
    void	_OnLoginFailed			( SYSLogFailedENUM type );

    void	_OnGSrvConnected		( int id , string GSrvName );
    void	_OnGSrvDisconnected	    ( int id );
    bool	_OnGSrvPacket			( int id , int size , PacketBase* data );
	bool	_OnOtherWorldPacket		( int worldid , int netid , int size, PacketBase* data );

    void	_OnClientConnected		( int id , string accountname );
    void	_OnClientDisconnected	( int id );
    bool	_OnCliPacket			( int id , int size , PacketBase* data );
    void	_OnServerMsg			( char*	Msg );
	void	_OnProxyPingLog			( int ProxyID , int Time );
public:
    GSrvNetWaker( );
    ~GSrvNetWaker( );

    //GateWay 資料交換處理
    bool	Process( );
    //連到 Switch Server
    void    Connect( const char* SwitchIP , int SwitchPort , const char* SrvName );
    //設定聯線模式
    void    ConnectType( SYSGSrvTypeENUM Type );
    //-------------------------------------------------------------------------------------------
    //讓某 cli 與 某 GSrv 成立新的連線
    bool	ConnectCliAndSrv( int SrvID , int CliID );	
    //與 某個 Client 斷線
    bool	CliDisconnect( int CliID );		
    //要求把某個Client logout
    void	CliLogout( int CliID, int SrvType, int SrvID, const char* szAccount, const char* pszReason );
    //把自己Logout
    void	Logout( );
	//要求把某部Server Logout
	void	SrvLogout( int SrvID );
    //取得某 Cli 的連線資料
    GSrv_CliInfoStruct*    GetCliNetInfo( int CliID ) ;
	//送資料處理
	void	SendFlush();
	
	//取得排程結構
	FunctionSchedularClass*  Schedular() { return _Net->Schedular(); };
    //-------------------------------------------------------------------------------------------
//    static char*	GetCliIPAddr	( int	CliId );			//取得某一個 Client的IPAddr
    //-------------------------------------------------------------------------------------------
    ////送封包給Server or Client
    bool	SendToSrv( int SrvID , int Size , const void*	Buf );	
    bool	SendToCli( int CliID , int Size , const void*	Buf );	
    bool	SendToCli( int CliID , int ProxyID , int Size , const void*	Buf );	
    void	SendToAllSrv( int Size , const void* Buf );	
    void	SendToAllCli( int Size , const void* Buf );	
	void	SendToAllPlayer( int Size , const void* Buf );	
	//bool	SendToAccount( int Size , const void* Buf );

	void	SendToOtherWorld_NetID( int WorldID , int NetID , int Size , const void* Buf );
	void	SendToOtherWorld_GSrvType( int WorldID , int GSrvType, int GSrvID , int Size , const void* Buf );

    //-------------------------------------------------------------------------------------------
     
    bool    RegOnLoginFunction		( OnLoginFunctionBase );
    bool    RegOnLogoutFunction		( OnLogoutFunctionBase );
    bool    RegOnLoginFailFunction	( OnLoginFailFunctionBase ); 

    bool    RegOnSrvConnectFunction	( OnSrvConnectFunctionBase );
    bool    RegOnSrvDisConnectFunction( OnSrvDisconnectFunctionBase );
    bool    RegOnSrvPacketFunction	( int PGid , OnCliPacketFunctionBase );
	bool    RegOnOtherWorldPacketFunction( int PGid , OnOtherWorldPacketFunctionBase  Func );

    bool    RegOnCliConnectFunction	( OnCliConnectFunctionBase );
    bool    RegOnCliDisconnectFunction( OnCliDisconnectFunctionBase );

	bool	RegOnClientPacketCrashOutput( OnClientPacketCrashOutputBase );
    //已登入過的封包id不可重覆登入
    bool    RegOnCliPacketFunction	( int PGid , OnCliPacketFunctionBase , int MinPgSize = 0 , int MaxPgSize = 0x1000);

    bool    RegOnSrvMsgFunction		( OnSrvMsgFunctionBase );

	void	RegOnProxyPingLog		( OnProxyPingLogFunctionBase );

	//註冊Server的loading 狀況
	bool	RegServerLoading		( int MaxPlayerCount , int ParallelZoneCount , int PlayerCount[100] , float TotalPlayerCount );

	//設定是否要使用Try Exception
	void	SetEnabledTryException	( bool Enabled );

	//註冊GSrv NetID
	void	RegGSrvNetID( int GSrvType , int GSrvID = 0 );

	//帳號冷凍
	void	FreezeAccount( const char* Account , int FreezeType );

	void	ProxyPingLog( );

	int		NetID()	{ return _Net->NetID(); }
    //-------------------------------------------------------------------------------------------
	//取得Ip Num
	unsigned int GetIpNum( int CliID );
	int		GetWorldID()			{ return _Net->GetWorldID(); };
	//-------------------------------------------------------------------------------------------
	CNetBridge*		NetBSwitch(){ return _Net->NetBSwitch() ; };			            //主要是用連接Switch
	CNetBridge*		NetBProxy(){ return _Net->NetBProxy();  };			                //主要是用連接Proxy
};

//-----------------------------------------------------------------------------------

