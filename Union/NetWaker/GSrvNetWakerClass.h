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
    SYSGSrvTypeENUM         _Type;                              //�p�u�Ҧ�


    CNetBridge*			    _NetBSwitch;			            //�D�n�O�γs��Switch
    CNetBridge*			    _NetBProxy;			                //�D�n�O�γs��Proxy
    LBSwitch*               _SwitchBListener;
    LBProxy* 		        _ProxyBListener;                     //��ƦC�|Class

    int                     _NetSwitchID;

    string                  _SwitchAddrIP;
    int                     _SwitchAddrPort;

    set<int>			    _ProxyConnect;	                    //GSrv �P����Proxy���@�s��
    Map_NetIDtoIDClass      _ProxyNetToIDMap;
	short					_ClientDataKey;						//�s��Client�ݪ�Key
    

    Map_StrAndID            _GSrvNameList;                      //Server �W�ٻPID�ഫ��Map

    bool                    _ExitFlag;                          //�����X��
    bool                    _IsReady;                           //�n�Jok

    vector<PGBaseInfo>	    _PGInfo;			                //�ʥ]�޲z����T

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
    // �ʥ]Call Back �B�z
    //-----------------------------------------------------------------------------------
    //(Switch)
    void _PGxUnKnoew( int NetID , PGSysNULL* PG , int Size );
    // �s����Switch �n�D�s��
    void _PGxSwitchConnectNodify( int NetID , PGSysNULL* PG , int Size );
    //�q��Proxy Addr�åB�n�D�s��
    void _PGxProxyAddr( int NetID , PGSysNULL* PG , int Size );
    //�n�DGSrv�P�YProxy�_�u
    void _PGxProxyDisconnectRequest( int NetID , PGSysNULL* PG , int Size );
    //Switch �q�� GSrv�� �s��GSrv���߳s�u
    void _PGxOnNewGSrvConnect( int NetID , PGSysNULL* PG , int Size );
    //Switch �q�� GSrv�� �s��GSrv�_�u
    void _PGxOnGSrvDisConnect( int NetID , PGSysNULL* PG , int Size );
    //GSrv��e�L�Ӫ����
	void _PGxDataGSrvToGSrv( int NetID , PGSysNULL* PG , int Size );
	void _PGxDataGSrvToGSrv_Zip( int NetID , PGSysNULL* PG , int Size );
	//�ʥ]��e��t�~��Server
	void _PGDataTransmitToWorldGSrv( int NetID , PGSysNULL* PG , int Size );
	//�ʥ]��e��t�~��Server
	void _PGDataTransmitToWorldGSrv_GSrvID( int NetID , PGSysNULL* PG , int Size );
    
    //-----------------------------------------------------------------------------------
    //(Proxy)
    // �s����Proxy ���
    void _PGxProxyConnectNotify( int NetID , PGSysNULL* PG , int Size );

    //�P GSrv���߳s�u
    void _PGxCliOnConnect	( int NetID , PGSysNULL* PG , int Size );
    //�P GSrv�_�u
    void _PGxCliOnDisonnect	( int NetID , PGSysNULL* PG , int Size );
    //Client ��e�L�Ӫ����
    void _PGxDataCliToGSrv	( int NetID , PGSysNULL* PG , int Size );
		
	void _PGxPingLog		( int NetID , PGSysNULL* PG , int Size );
    //-----------------------------------------------------------------------------------
    //���s�s�� Switch Server
    static int  _ReConnectSwitch( SchedularInfo* Obj , void* InputClass );
    //-----------------------------------------------------------------------------------
    void _ConnectProxy( char* IpStr ,int Port );

public:

    GSrvNetWakerClass( );
    virtual ~GSrvNetWakerClass();

    void	ConnectSwitch( const char* IP,int Port );

    //�]�w GSrv ���W��
    void    SetGSrvName( const char* Name );

    //�w�ɳB�z
    bool	Process( );

    //�n�X
    bool    Logout( );

    void    SetListener( NewWakerEventListener* Listener );

    bool    SrvLogout( int SrvID );

	void	SendFlush();
    //-----------------------------------------------------------------------------------

    void    ConnectType( SYSGSrvTypeENUM Type );
    //���Y cli �P �Y GSrv ���߷s���s�u
    bool	ConnectCliAndSrv( int GSrvID , int CliID );	
    //�P �Y�� Client �_�u
    bool	CliDisconnect( int CliID );		
    //�n�D��Y��Client logout
    void	CliLogout( int CliID, const char* pszReason );
    //���o�Y Cli ���s�u���
    GSrv_CliInfoStruct*    GetCliNetInfo( int CliID ) ;

    //-----------------------------------------------------------------------------------
    //�e��ƨ� Switch
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

	//�e���Ҧ��s�W��Server��Client
    void    SendToAllCli( int Size , const void* Data );
	//�e���Ҧ����a
	void    SendToAllPlayer( int Size , const void* Data );

	bool	SendToAccount( int Size , const void* Data );

	//�e��ƨ�Ҧ����@��
	void	SendToAllOtherWorldSrv( int WorldID , int Size , const void* Data );
	//�e��ƨ�Ҧ����@��
	void	SendToOtherWorldSrv( int ToWorldID , int ToNetID , int Size , const void* Data );
	//�e��ƨ�Ҧ����@��
	void	SendToOtherWorldSrv_GSrvID( int ToWorldID  , int ToGSrvType , int ToGSrvID, int Size , const void* Data );

	//���UGSrv NetID
	void	RegGSrvNetID( int GSrvType , int GSrvID );

	void	ProxyPingLog(  );

	//��Y�b���N��
	void	FreezeAccount( const char* Account , int FreezeType );

	//���UServer��loading ���p
	bool	RegServerLoading		( int MaxPlayerCount , int	ParallelZoneCount , int PlayerCount[100] , float TotalPlayerCount );


	unsigned int GetIpNum( int SockID );

	//���o�Ƶ{���c
	FunctionSchedularClass*  Schedular() { return _Schedular; };

	int		NetID()	{ return _GSrvID; }
    //-----------------------------------------------------------------------------------
    // ��l�� Client �P Server �� CallBack�禡
    //-----------------------------------------------------------------------------------
    void	InitPGCallBack();

	//-----------------------------------------------------------------------------------
	//  �����֤ߪ��T����X
	//-----------------------------------------------------------------------------------
	static void		Output_NetMsg			( int lv, const char* szOut );
	static int		Output_NetStatus		( SchedularInfo* Obj , void* InputClass );

	//���onetbirdge���
	CNetBridge*			    NetBSwitch(){ return _NetBSwitch; };			            //�D�n�O�γs��Switch
	CNetBridge*			    NetBProxy(){ return _NetBProxy;  };			                //�D�n�O�γs��Proxy

	int		GetWorldID()					{ return _WorldID; }

};


#endif //__NETWAKERCLASS_H__