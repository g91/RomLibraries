#pragma once
#pragma warning (disable:4786)

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <functional>

#include "GSrvNetWakerClass.h"

//***********************************************************************************
// �w�q�^Call �禡
// �Ψӵ��U�禡������ �A �p�G���ƥ�o�ʹN�|�I�s�n�����禡
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
//CallBack class �]�w
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
	//��LWorld�e�Ӫ��ʥ]
	bool	OnOtherWorldPacket( int FromWorldID , int FromNetID , int size , void* data );	


    void	OnClientConnected	( int id , string accountname );
    void	OnClientDisconnected( int id );
    bool	OnCliPacket			( int id , int size , void* data );
    void	OnServerMsg			( char* Msg); 

	void	OnProxyPingLog		( int ProxyID , int Time );
};
//-----------------------------------------------------------------------------------
//Client �ݨӪ��ʥ]��ƺ޲z���c
struct CallBackStruct
{
    OnCliPacketFunctionBase		Fun;	//CallBack�禡
    int							Max;	//�̤j�ʥ]
    int							Min;	//�̤p�ʥ]

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
    //���檬�A
    //-----------------------------------------------------------------------------------
    int	    _ExitFlag;

    int     _ProcTime;
	
	bool	_IsEnableTryException;

	int		_GSrvType;			//�B�z�@����e�ʥ]���U
	int		_GSrvID;
    //------------------------------------------------------------------------------------------
    //�����n�J�ƥ�禡�����
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

    //GateWay ��ƥ洫�B�z
    bool	Process( );
    //�s�� Switch Server
    void    Connect( const char* SwitchIP , int SwitchPort , const char* SrvName );
    //�]�w�p�u�Ҧ�
    void    ConnectType( SYSGSrvTypeENUM Type );
    //-------------------------------------------------------------------------------------------
    //���Y cli �P �Y GSrv ���߷s���s�u
    bool	ConnectCliAndSrv( int SrvID , int CliID );	
    //�P �Y�� Client �_�u
    bool	CliDisconnect( int CliID );		
    //�n�D��Y��Client logout
    void	CliLogout( int CliID, int SrvType, int SrvID, const char* szAccount, const char* pszReason );
    //��ۤvLogout
    void	Logout( );
	//�n�D��Y��Server Logout
	void	SrvLogout( int SrvID );
    //���o�Y Cli ���s�u���
    GSrv_CliInfoStruct*    GetCliNetInfo( int CliID ) ;
	//�e��ƳB�z
	void	SendFlush();
	
	//���o�Ƶ{���c
	FunctionSchedularClass*  Schedular() { return _Net->Schedular(); };
    //-------------------------------------------------------------------------------------------
//    static char*	GetCliIPAddr	( int	CliId );			//���o�Y�@�� Client��IPAddr
    //-------------------------------------------------------------------------------------------
    ////�e�ʥ]��Server or Client
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
    //�w�n�J�L���ʥ]id���i���еn�J
    bool    RegOnCliPacketFunction	( int PGid , OnCliPacketFunctionBase , int MinPgSize = 0 , int MaxPgSize = 0x1000);

    bool    RegOnSrvMsgFunction		( OnSrvMsgFunctionBase );

	void	RegOnProxyPingLog		( OnProxyPingLogFunctionBase );

	//���UServer��loading ���p
	bool	RegServerLoading		( int MaxPlayerCount , int ParallelZoneCount , int PlayerCount[100] , float TotalPlayerCount );

	//�]�w�O�_�n�ϥ�Try Exception
	void	SetEnabledTryException	( bool Enabled );

	//���UGSrv NetID
	void	RegGSrvNetID( int GSrvType , int GSrvID = 0 );

	//�b���N��
	void	FreezeAccount( const char* Account , int FreezeType );

	void	ProxyPingLog( );

	int		NetID()	{ return _Net->NetID(); }
    //-------------------------------------------------------------------------------------------
	//���oIp Num
	unsigned int GetIpNum( int CliID );
	int		GetWorldID()			{ return _Net->GetWorldID(); };
	//-------------------------------------------------------------------------------------------
	CNetBridge*		NetBSwitch(){ return _Net->NetBSwitch() ; };			            //�D�n�O�γs��Switch
	CNetBridge*		NetBProxy(){ return _Net->NetBProxy();  };			                //�D�n�O�γs��Proxy
};

//-----------------------------------------------------------------------------------

