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
    SYSGSrvTypeENUM         _Type;                              //Áp½u¼Ò¦¡


    CNetBridge*			    _NetBSwitch;			            //¥D­n¬O¥Î³s±µSwitch
    CNetBridge*			    _NetBProxy;			                //¥D­n¬O¥Î³s±µProxy
    LBSwitch*               _SwitchBListener;
    LBProxy* 		        _ProxyBListener;                     //¸ê®Æ¦CÁ|Class

    int                     _NetSwitchID;

    string                  _SwitchAddrIP;
    int                     _SwitchAddrPort;

    set<int>			    _ProxyConnect;	                    //GSrv »P­þ¨ÇProxy¦³§@³sµ²
    Map_NetIDtoIDClass      _ProxyNetToIDMap;
	short					_ClientDataKey;						//¦s¨úClientºÝªºKey
    

    Map_StrAndID            _GSrvNameList;                      //Server ¦WºÙ»PIDÂà´«ªºMap

    bool                    _ExitFlag;                          //µ²§ôºX¼Ð
    bool                    _IsReady;                           //µn¤Jok

    vector<PGBaseInfo>	    _PGInfo;			                //«Ê¥]ºÞ²zªº¸ê°T

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
    // «Ê¥]Call Back ³B²z
    //-----------------------------------------------------------------------------------
    //(Switch)
    void _PGxUnKnoew( int NetID , PGSysNULL* PG , int Size );
    // ³sµ²¨ìSwitch ­n¨D³sµ²
    void _PGxSwitchConnectNodify( int NetID , PGSysNULL* PG , int Size );
    //³qª¾Proxy Addr¨Ã¥B­n¨D³sµ²
    void _PGxProxyAddr( int NetID , PGSysNULL* PG , int Size );
    //­n¨DGSrv»P¬YProxyÂ_½u
    void _PGxProxyDisconnectRequest( int NetID , PGSysNULL* PG , int Size );
    //Switch ³qª¾ GSrv¦³ ·sªºGSrv¦¨¥ß³s½u
    void _PGxOnNewGSrvConnect( int NetID , PGSysNULL* PG , int Size );
    //Switch ³qª¾ GSrv¦³ ·sªºGSrvÂ_½u
    void _PGxOnGSrvDisConnect( int NetID , PGSysNULL* PG , int Size );
    //GSrvÂà°e¹L¨Óªº¸ê®Æ
	void _PGxDataGSrvToGSrv( int NetID , PGSysNULL* PG , int Size );
	void _PGxDataGSrvToGSrv_Zip( int NetID , PGSysNULL* PG , int Size );
	//«Ê¥]Âà°e¨ì¥t¥~ªºServer
	void _PGDataTransmitToWorldGSrv( int NetID , PGSysNULL* PG , int Size );
	//«Ê¥]Âà°e¨ì¥t¥~ªºServer
	void _PGDataTransmitToWorldGSrv_GSrvID( int NetID , PGSysNULL* PG , int Size );
    
    //-----------------------------------------------------------------------------------
    //(Proxy)
    // ³sµ²¨ìProxy ¦¨¥
    void _PGxProxyConnectNotify( int NetID , PGSysNULL* PG , int Size );

    //»P GSrv¦¨¥ß³s½u
    void _PGxCliOnConnect	( int NetID , PGSysNULL* PG , int Size );
    //»P GSrvÂ_½u
    void _PGxCliOnDisonnect	( int NetID , PGSysNULL* PG , int Size );
    //Client Âà°e¹L¨Óªº¸ê®Æ
    void _PGxDataCliToGSrv	( int NetID , PGSysNULL* PG , int Size );
		
	void _PGxPingLog		( int NetID , PGSysNULL* PG , int Size );
    //-----------------------------------------------------------------------------------
    //­«·s³sµ² Switch Server
    static int  _ReConnectSwitch( SchedularInfo* Obj , void* InputClass );
    //-----------------------------------------------------------------------------------
    void _ConnectProxy( char* IpStr ,int Port );

public:

    GSrvNetWakerClass( );
    virtual ~GSrvNetWakerClass();

    void	ConnectSwitch( const char* IP,int Port );

    //³]©w GSrv ªº¦WºÙ
    void    SetGSrvName( const char* Name );

    //©w®É³B²z
    bool	Process( );

    //µn¥X
    bool    Logout( );

    void    SetListener( NewWakerEventListener* Listener );

    bool    SrvLogout( int SrvID );

	void	SendFlush();
    //-----------------------------------------------------------------------------------

    void    ConnectType( SYSGSrvTypeENUM Type );
    //Åý¬Y cli »P ¬Y GSrv ¦¨¥ß·sªº³s½u
    bool	ConnectCliAndSrv( int GSrvID , int CliID );	
    //»P ¬Y­Ó Client Â_½u
    bool	CliDisconnect( int CliID );		
    //­n¨D§â¬Y­ÓClient logout
    void	CliLogout( int CliID, const char* pszReason );
    //¨ú±o¬Y Cli ªº³s½u¸ê®Æ
    GSrv_CliInfoStruct*    GetCliNetInfo( int CliID ) ;

    //-----------------------------------------------------------------------------------
    //°e¸ê®Æ¨ì Switch
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

	//°eµ¹©Ò¦³³s¤W¦¹ServerªºClient
    void    SendToAllCli( int Size , const void* Data );
	//°eµ¹©Ò¦³ª±®a
	void    SendToAllPlayer( int Size , const void* Data );

	bool	SendToAccount( int Size , const void* Data );

	//°e¸ê®Æ¨ì©Ò¦³ªº¥@¬É
	void	SendToAllOtherWorldSrv( int WorldID , int Size , const void* Data );
	//°e¸ê®Æ¨ì©Ò¦³ªº¥@¬É
	void	SendToOtherWorldSrv( int ToWorldID , int ToNetID , int Size , const void* Data );
	//°e¸ê®Æ¨ì©Ò¦³ªº¥@¬É
	void	SendToOtherWorldSrv_GSrvID( int ToWorldID  , int ToGSrvType , int ToGSrvID, int Size , const void* Data );

	//µù¥UGSrv NetID
	void	RegGSrvNetID( int GSrvType , int GSrvID );

	void	ProxyPingLog(  );

	//§â¬Y±b¸¹§N­á
	void	FreezeAccount( const char* Account , int FreezeType );

	//µù¥UServerªºloading ª¬ªp
	bool	RegServerLoading		( int MaxPlayerCount , int	ParallelZoneCount , int PlayerCount[100] , float TotalPlayerCount );


	unsigned int GetIpNum( int SockID );

	//¨ú±o±Æµ{µ²ºc
	FunctionSchedularClass*  Schedular() { return _Schedular; };

	int		NetID()	{ return _GSrvID; }
    //-----------------------------------------------------------------------------------
    // ªì©l¤Æ Client »P Server ªº CallBack¨ç¦¡
    //-----------------------------------------------------------------------------------
    void	InitPGCallBack();

	//-----------------------------------------------------------------------------------
	//  ºô¸ô®Ö¤ßªº°T®§¿é¥X
	//-----------------------------------------------------------------------------------
	static void		Output_NetMsg			( int lv, const char* szOut );
	static int		Output_NetStatus		( SchedularInfo* Obj , void* InputClass );

	//¨ú±onetbirdge¸ê®Æ
	CNetBridge*			    NetBSwitch(){ return _NetBSwitch; };			            //¥D­n¬O¥Î³s±µSwitch
	CNetBridge*			    NetBProxy(){ return _NetBProxy;  };			                //¥D­n¬O¥Î³s±µProxy

	int		GetWorldID()					{ return _WorldID; }

};


#endif //__NETWAKERCLASS_H__