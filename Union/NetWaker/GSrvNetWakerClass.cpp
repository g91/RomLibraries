#include "GSrvNetWakerClass.h"
#include <string.h>
#include "ControllerClient\ControllerClient.h"
#pragma warning (disable:4949)
#pragma unmanaged
//---------------------------------------------------------------------------------
//char	GSrvNetWakerClass::_TempSendData[ 0x40000 ];
bool	GSrvNetWakerClass::G_SwitchPacketTimeProcLog = false;
//---------------------------------------------------------------------------------
bool		LBSwitch::OnRecv			( DWORD NetID, DWORD dwPackSize, PVOID pPacketData )
{
    _Parent->_OnSwitchPacket(NetID,dwPackSize,pPacketData);
    return true;   
}
void		LBSwitch::OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode )
{
    Print( Def_PrintLV3 , "OnConnectFailed" , "(Switch)dwFailedCode" , dwFailedCode );
    _Parent->_OnSwitchConnectFailed(dwFailedCode);
}
void		LBSwitch::OnConnect		( DWORD dwNetID )
{
    Print( Def_PrintLV3 , "OnConnect" , "(Switch:%d)_OnSwitchConnected" , dwNetID );
    _Parent->_OnSwitchConnected(dwNetID);
}
void		LBSwitch::OnDisconnect	( DWORD dwNetID )
{
    Print( Def_PrintLV3 , "OnDisconnect" , "(Switch:%d)_OnSwitchDisconnected" , dwNetID );
	PrintToController(true, "On Switch Disconnected");
    _Parent->_OnSwitchDisconnected(dwNetID);
}
CEventObj*	LBSwitch::OnAccept		( DWORD dwNetID )
{
    Print( Def_PrintLV3 , "OnAccept" , "(Switch:%d)_OnSwitchConnected" , dwNetID );
    //_Parent->_OnSwitchConnected(dwNetID); 
    return this;
}
//---------------------------------------------------------------------------------
bool		LBProxy::OnRecv			( DWORD NetID, DWORD dwPackSize, PVOID pPacketData )
{
    _Parent->_OnProxyPacket(NetID,dwPackSize,pPacketData);
    return true;   
}
void		LBProxy::OnConnectFailed	( DWORD dwNetID, DWORD dwFailedCode )
{
    Print( Def_PrintLV3 , "dwFailedCode" ,"(Proxy)" , dwFailedCode );
    _Parent->_OnProxyConnectFailed(dwFailedCode);
}
void		LBProxy::OnConnect		( DWORD dwNetID )
{
    Print( Def_PrintLV3 , "_OnProxyConnected" , "(Proxy:%d)" , dwNetID );
    _Parent->_OnProxyConnected(dwNetID);
}
void		LBProxy::OnDisconnect	( DWORD dwNetID )
{
    Print( Def_PrintLV3 , "_OnProxyDisconnected" , "(Proxy:%d)" , dwNetID );
	PrintToController(true, "On Proxy Disconnected");
    _Parent->_OnProxyDisconnected(dwNetID);
}
CEventObj*	LBProxy::OnAccept		( DWORD dwNetID )
{
    Print( Def_PrintLV3 , "_OnProxyConnected" , "(Proxy:%d)" , dwNetID );
    //_Parent->_OnProxyConnected(dwNetID);
    return this;
}
//---------------------------------------------------------------------------------
void	GSrvNetWakerClass::InitPGCallBack()
{
    int		i;
    PGBaseInfo	PGBase;
    //callback 函式陣列出始化
    for( i = 0 ; i <EM_SysNet_Packet_Count ; i++ )
    {
        _PGCallBack[i] = &GSrvNetWakerClass::_PGxUnKnoew;
        _PGInfo.push_back(PGBase);
    }

    _PGInfo[ EM_SysNet_Switch_GSrv_NotifyConnect ].Type.Switch = true;
	_PGCallBack[ EM_SysNet_Switch_GSrv_NotifyConnect ]         = &GSrvNetWakerClass::_PGxSwitchConnectNodify;

    _PGInfo[ EM_SysNet_Switch_GSrv_ProxyAddr ].Type.Switch = true;
    _PGCallBack[ EM_SysNet_Switch_GSrv_ProxyAddr ]         = &GSrvNetWakerClass::_PGxProxyAddr;

    _PGInfo[ EM_SysNet_Switch_GSrv_ProxyDisconnectRequest ].Type.Switch = true;
    _PGCallBack[ EM_SysNet_Switch_GSrv_ProxyDisconnectRequest ]         = &GSrvNetWakerClass::_PGxProxyDisconnectRequest;

    _PGInfo[ EM_SysNet_Switch_GSrv_OnGSrvConnect ].Type.Switch = true;
    _PGCallBack[ EM_SysNet_Switch_GSrv_OnGSrvConnect ]         = &GSrvNetWakerClass::_PGxOnNewGSrvConnect;

    _PGInfo[ EM_SysNet_Switch_GSrv_OnGSrvDisConnect ].Type.Switch = true;
    _PGCallBack[ EM_SysNet_Switch_GSrv_OnGSrvDisConnect ]         = &GSrvNetWakerClass::_PGxOnGSrvDisConnect;

    _PGInfo[ EM_SysNet_Switch_GSrv_DataGSrvToGSrv ].Type.Switch = true;
    _PGCallBack[ EM_SysNet_Switch_GSrv_DataGSrvToGSrv ]        = &GSrvNetWakerClass::_PGxDataGSrvToGSrv;

	_PGInfo[ EM_SysNet_Switch_GSrv_DataGSrvToGSrv_Zip ].Type.Switch = true;
	_PGCallBack[ EM_SysNet_Switch_GSrv_DataGSrvToGSrv_Zip ]        = &GSrvNetWakerClass::_PGxDataGSrvToGSrv_Zip;

//	_PGInfo[ EM_SysNet_Switch_GSrv_WorldID ].Type.Switch = true;
//	_PGCallBack[ EM_SysNet_Switch_GSrv_WorldID ]         = &GSrvNetWakerClass::_PGxOnWorldID;

//	_PGInfo[ EM_SysNet_Switch_GSrv_ExchangeItemResult ].Type.Switch = true;
//	_PGCallBack[ EM_SysNet_Switch_GSrv_ExchangeItemResult ]        = &GSrvNetWakerClass::_PGxExchangeItemResult;

	_PGInfo[ EM_SysNet_X_DataTransmitToWorldGSrv ].Type.Switch = true;
	_PGCallBack[ EM_SysNet_X_DataTransmitToWorldGSrv ]        = &GSrvNetWakerClass::_PGDataTransmitToWorldGSrv;

	_PGInfo[ EM_SysNet_X_DataTransmitToWorldGSrv_GSrvID ].Type.Switch = true;
	_PGCallBack[ EM_SysNet_X_DataTransmitToWorldGSrv_GSrvID ]         = &GSrvNetWakerClass::_PGDataTransmitToWorldGSrv_GSrvID;


    _PGInfo[ EM_SysNet_Proxy_GSrv_ConnectNotify ].Type.Proxy = true;
    _PGCallBack[ EM_SysNet_Proxy_GSrv_ConnectNotify ]         = &GSrvNetWakerClass::_PGxProxyConnectNotify;

    _PGInfo[ EM_SysNet_Proxy_GSrv_CliOnDisConnect ].Type.Proxy = true;
    _PGCallBack[ EM_SysNet_Proxy_GSrv_CliOnDisConnect ]         = &GSrvNetWakerClass::_PGxCliOnDisonnect;

    _PGInfo[ EM_SysNet_Proxy_GSrv_CliOnConnect ].Type.Proxy = true;
    _PGCallBack[ EM_SysNet_Proxy_GSrv_CliOnConnect ]         = &GSrvNetWakerClass::_PGxCliOnConnect;

    _PGInfo[ EM_SysNet_Proxy_GSrv_DataCliToGSrv ].Type.Proxy = true;
    _PGCallBack[ EM_SysNet_Proxy_GSrv_DataCliToGSrv ]        = &GSrvNetWakerClass::_PGxDataCliToGSrv;

	_PGInfo[ EM_SysNet_Proxy_GSrv_PingLog ].Type.Proxy = true;
	_PGCallBack[ EM_SysNet_Proxy_GSrv_PingLog ]        = &GSrvNetWakerClass::_PGxPingLog;
 

}

//---------------------------------------------------------------------------------

GSrvNetWakerClass::GSrvNetWakerClass( )
{
 

    InitPGCallBack();


    _ExitFlag       = false;
    _NetSwitchID    = -1;                   //設定為尚未連線狀態
    _GSrvID         = -1;
	_WorldID		= -1;
    _GSrvName[ 0 ]  = 0; 
    _IsReady        = false;
    _Type           = EM_GSrvType_Passive;

    _NetBSwitch     = CreateNetBridge( true, 3 );
	_NetBSwitch->SetOutPutCB( &GSrvNetWakerClass::Output_NetMsg );

    _SwitchBListener = NEW LBSwitch( this );

    _NetBProxy     = CreateNetBridge( true, 3 );
	_NetBProxy->SetOutPutCB( Output_NetMsg );
    _ProxyBListener  = NEW LBProxy( this );	//資料列舉Class

    _Schedular      = NEW( FunctionSchedularClass );

//	ItemExchangeResult = EmptyItemExchangeResultFunction;
	_Schedular->Push( Output_NetStatus, 10000, this, NULL );
}

GSrvNetWakerClass::~GSrvNetWakerClass()
{
    DeleteNetBridge( _NetBSwitch );
    DeleteNetBridge( _NetBProxy);
   delete _SwitchBListener;
   delete _ProxyBListener;

   delete _Schedular;
}
//---------------------------------------------------------------------------------
void    GSrvNetWakerClass::SetListener( NewWakerEventListener* Listener )
{
    _Listener       = Listener;
}
//---------------------------------------------------------------------------------
void	GSrvNetWakerClass::ConnectSwitch( const char* IP,int Port )
{
    _SwitchAddrIP = IP;
    _SwitchAddrPort = Port;
    int IPnum = ConvertIP( IP );
    _NetBSwitch->Connect( IPnum , Port , _SwitchBListener );	
}
void	GSrvNetWakerClass::_ConnectProxy( char* IPStr ,int Port )
{
	int IP = ConvertIP( IPStr );
    _NetBProxy->Connect( IP , Port , _ProxyBListener );
}

//---------------------------------------------------------------------------------
//定時處理
bool	GSrvNetWakerClass::Process( )
{
	static ProcTimeLogClass	ProcLog;

    timeval t1 = {0,0};
	
	CNetBridge::WaitAll( 10 );
	Sleep(1);

	ProcLog.Enter();
    _Schedular->Process();
	ProcLog.Leave();

	if( ProcLog.LogTime() > 5000 )
	{
		PrintToFile( Def_PrintLV2 , "_Schedular ProcLog" , " Min =%d Max=%d Avg=%d Count=%d" 
			, ProcLog.Base().MinProcTime 
			, ProcLog.Base().MaxProcTime 
			, ProcLog.Base().AvgProcTime() 
			, ProcLog.Base().ProcCount  );	
		ProcLog.Clear();
	}


    return !_ExitFlag;	
}
//---------------------------------------------------------------------------------
//送資料到 Switch
bool    GSrvNetWakerClass::SendToSwitch( int Size , const void* Data )
{
    if( _NetSwitchID == -1 )
        return false;

    _NetBSwitch->Send( _NetSwitchID , Size , (void*)Data  );
    return true;
}

bool    GSrvNetWakerClass::SendToSwitch( int Size1 , const void* Data1 , int Size2 , const void* Data2 )
{
	if( _NetSwitchID == -1 )
		return false;

	_NetBSwitch->SendWithBuff( _NetSwitchID , Size1 , (void*)Data1 , Size2 , (void*)Data2 );
	return true;
}

/*
bool	GSrvNetWakerClass::SendToAccount( int Size , const void* Data )
{
	if( _NetSwitchID == -1 )
		return false;
	SysNet_GSrv_Switch_TransmitToAccount Send;
	Send.DataSize = Size;
	memcpy( Send.Data , Data , Size );

	_NetBSwitch->Send( _NetSwitchID , Send.Size() , (void*)&Send , 0 , 0 );
	return true;
}
*/
//---------------------------------------------------------------------------------
void	GSrvNetWakerClass::SendToAllProxy( int Size , const void* Data )
{
	_NetBProxy->SendToAll( Size , (void*)Data );
}
bool    GSrvNetWakerClass::SendToProxy_NETID( int ProxyNetID , int Size , const void* Data )
{
    //    int ProxyNetID = _ProxyNetToIDMap.GetNetID( ProxyID );
//    if( _NetProxy->IsValidLNETID( ProxyNetID ) == false )
//    {
//        Print( Def_PrintLV1 , "SendToProxy_NETID : ProxyID(%d) is Valid NetID !! " , ProxyNetID );
//        return false;
//    }

    _NetBProxy->Send( ProxyNetID , Size , (void*)Data  );
    return true;
}
//送資料到 Switch
bool    GSrvNetWakerClass::SendToProxy( int ProxyID , int Size , const void* Data )
{
    int ProxyNetID = _ProxyNetToIDMap.GetNetID( ProxyID );
    _NetBProxy->Send( ProxyNetID , Size , (void*)Data  );
    return true;
}

bool    GSrvNetWakerClass::SendToProxy( int ProxyID , int Size1 , const void* Data1 , int Size2 , const void* Data2 )
{
	int ProxyNetID = _ProxyNetToIDMap.GetNetID( ProxyID );
	_NetBProxy->SendWithBuff( ProxyNetID , Size1 , (void*)Data1 , Size2 , (void*)Data2 );
	return true;
}

//-----------------------------------------------------------------------------------
//設定 GSrv 的名稱
void     GSrvNetWakerClass::SetGSrvName( const char* Name )
{
    //strncpy_s( _GSrvName, sizeof( _GSrvName ) , Name , sizeof(_GSrvName) );
	strncpy( _GSrvName, Name , sizeof(_GSrvName) );
	_GSrvName[sizeof(_GSrvName)-1] = 0;
}
//-----------------------------------------------------------------------------------
void     GSrvNetWakerClass::ConnectType( SYSGSrvTypeENUM Type )
{
    if( Type == _Type )
        return;

    if( _IsReady != false )
	{
        SysNet_GSrv_Switch_SetActive    SetActiveType;
        SetActiveType.Type = Type;

        SendToSwitch( sizeof( SetActiveType ) , &SetActiveType );
    }

    _Type = Type;
}
//-----------------------------------------------------------------------------------
bool     GSrvNetWakerClass::Logout( )
{
//    _NetSwitch->Close( _NetSwitchID );
    _NetBSwitch->Close( _NetSwitchID );
    return true;
}
//-----------------------------------------------------------------------------------
bool    GSrvNetWakerClass::SendToGSrv( int GSrvID , int Size , const void* Data )
{
/*
    int   PGSize = Size + sizeof( SysNet_GSrv_Switch_DataGSrvToGSrv);
    SysNet_GSrv_Switch_DataGSrvToGSrv* DataGSrvToGSrv = (SysNet_GSrv_Switch_DataGSrvToGSrv*)_TempSendData;
    DataGSrvToGSrv->Command = EM_SysNet_GSrv_Switch_DataGSrvToGSrv;
    DataGSrvToGSrv->GSrvID = GSrvID;
    DataGSrvToGSrv->Size = Size; 
    memcpy( DataGSrvToGSrv->Data , Data , Size );


    SendToSwitch( PGSize , DataGSrvToGSrv );
*/

	if( Size < 5000 )
	{
		SysNet_GSrv_Switch_DataGSrvToGSrv DataGSrvToGSrv;
		DataGSrvToGSrv.GSrvID = GSrvID;
		DataGSrvToGSrv.Size = Size; 

		SendToSwitch( sizeof(DataGSrvToGSrv) , &DataGSrvToGSrv , Size , Data );

	}
	else
	{
		SysNet_GSrv_Switch_DataGSrvToGSrv_Zip Send;
		Send.GSrvID = GSrvID;
		Send.Size = Size; 

		MyLzoClass myZip;
		Send.ZipSize = myZip.Encode( (char*)(Data) , Size , (char*)&Send.Data );
		SendToSwitch( Send.PGSize()  , &Send );
	}

    return true;
}
//-----------------------------------------------------------------------------------
bool    GSrvNetWakerClass::SendToAllGSrv( int Size , const void* Data )
{
    return SendToGSrv( -1 , Size , Data );
}
//-----------------------------------------------------------------------------------
bool	GSrvNetWakerClass::SendToCli_ClientKey( int CliID , short ClientDataKeyID )
{
	//尋找此Client 所聯結的 Proxy
	if( CliID < 0  )
		return false;

	GSrv_CliInfoStruct* CliInfo = _CliList[ CliID ];
	if( CliInfo == NULL )
		return false;

	SysNet_GSrv_Proxy_DataGSrvToCli Send;
	Send.CliID = CliID;
	Send.Size = 0;
	Send.ClientDataKey = ClientDataKeyID;
	SendToProxy( CliInfo->ProxyID , sizeof(Send) , &Send );
	return true;
}
bool    GSrvNetWakerClass::SendToCli( int CliID , int ProxyID , int Size , const void* Data )
{
    //尋找此Client 所聯結的 Proxy
    if( CliID < 0  )
        return false;

	/*
    int   PGSize = Size + sizeof( SysNet_GSrv_Switch_DataGSrvToGSrv);

    SysNet_GSrv_Proxy_DataGSrvToCli* DataGSrvToCli = (SysNet_GSrv_Proxy_DataGSrvToCli*)_TempSendData;

	if( Size > sizeof(_TempSendData ) )
	{
		DataGSrvToCli = (SysNet_GSrv_Proxy_DataGSrvToCli*) NEW( char[ PGSize ] );
	}
    DataGSrvToCli->Command = EM_SysNet_GSrv_Proxy_DataGSrvToCli;

    DataGSrvToCli->CliID = CliID;
    DataGSrvToCli->Size = Size;
    memcpy( DataGSrvToCli->Data , Data , Size );

    SendToProxy( ProxyID , PGSize , DataGSrvToCli );

	if( Size > sizeof(_TempSendData ) )
		delete DataGSrvToCli;
		*/


	SysNet_GSrv_Proxy_DataGSrvToCli DataGSrvToCli;

	DataGSrvToCli.CliID = CliID;
	DataGSrvToCli.Size = Size;

	SendToProxy( ProxyID , sizeof(DataGSrvToCli) , &DataGSrvToCli , Size , Data );

//	if( Size > sizeof(_TempSendData ) )
//		delete DataGSrvToCli;
    return true;

}
//-----------------------------------------------------------------------------------
bool    GSrvNetWakerClass::SendToCli( int CliID , int Size , const void* Data )
{
    //尋找此Client 所聯結的 Proxy
    GSrv_CliInfoStruct* CliInfo = _CliList[ CliID ];
    if( CliInfo == NULL )
         return false;
/* 
    int   PGSize = Size + sizeof( SysNet_GSrv_Switch_DataGSrvToGSrv);

    SysNet_GSrv_Proxy_DataGSrvToCli* DataGSrvToCli = (SysNet_GSrv_Proxy_DataGSrvToCli*)_TempSendData;
    DataGSrvToCli->Command = EM_SysNet_GSrv_Proxy_DataGSrvToCli;

    DataGSrvToCli->CliID = CliID;
    DataGSrvToCli->Size = Size;
    memcpy( DataGSrvToCli->Data , Data , Size );

    SendToProxy( CliInfo->ProxyID , PGSize , DataGSrvToCli );
*/


	SysNet_GSrv_Proxy_DataGSrvToCli DataGSrvToCli;
	DataGSrvToCli.CliID = CliID;
	DataGSrvToCli.Size = Size;
//	memcpy( DataGSrvToCli->Data , Data , Size );

	SendToProxy( CliInfo->ProxyID , sizeof(DataGSrvToCli) , &DataGSrvToCli , Size , Data );
    return true;

}
//-----------------------------------------------------------------------------------
void    GSrvNetWakerClass::SendToAllPlayer( int Size , const void* Data )
{ 
/*
	set<int>::iterator Iter;
	int   PGSize = Size + sizeof( SysNet_GSrv_Switch_DataGSrvToGSrv);

	SysNet_GSrv_Proxy_DataGSrvToCli* DataGSrvToCli = (SysNet_GSrv_Proxy_DataGSrvToCli*)_TempSendData;
	DataGSrvToCli->Size = Size;
	DataGSrvToCli->CliID = -1;
	DataGSrvToCli->Command = EM_SysNet_GSrv_Proxy_DataGSrvToCli;
	DataGSrvToCli->ClientDataKey = -1;

	memcpy( DataGSrvToCli->Data , Data , Size );

	_NetBProxy->SendToAll( PGSize ,  DataGSrvToCli );
*/
	set<int>::iterator Iter;

	SysNet_GSrv_Proxy_DataGSrvToCli DataGSrvToCli;
	DataGSrvToCli.Size = Size;
	DataGSrvToCli.CliID = -1;
	DataGSrvToCli.ClientDataKey = -1;

//	memcpy( DataGSrvToCli->Data , Data , Size );

	_NetBProxy->SendToAllWithBuff( sizeof(DataGSrvToCli) , &DataGSrvToCli , Size , (void*)Data );
}
//-----------------------------------------------------------------------------------
void    GSrvNetWakerClass::SendToAllCli( int Size , const void* Data )
{
/*
    set<int>::iterator Iter;
    int   PGSize = Size + sizeof( SysNet_GSrv_Switch_DataGSrvToGSrv);

    SysNet_GSrv_Proxy_DataGSrvToCli* DataGSrvToCli = (SysNet_GSrv_Proxy_DataGSrvToCli*)_TempSendData;
    DataGSrvToCli->Size = Size;
    DataGSrvToCli->Command = EM_SysNet_GSrv_Proxy_DataGSrvToCli;
	DataGSrvToCli->ClientDataKey = -1;

    memcpy( DataGSrvToCli->Data , Data , Size );

    GSrv_CliInfoStruct* CliInfo;

    _CliList.GetData( true );
    for( int i = 0 ; i < _CliList.size() ; i++ )
    {
        CliInfo = _CliList.GetData( );

        DataGSrvToCli->CliID = CliInfo->ID;
        SendToProxy( CliInfo->ProxyID , PGSize , DataGSrvToCli );        
    }
*/
	set<int>::iterator Iter;

	SysNet_GSrv_Proxy_DataGSrvToCli DataGSrvToCli;
	DataGSrvToCli.Size = Size;
	DataGSrvToCli.ClientDataKey = -1;

//	memcpy( DataGSrvToCli->Data , Data , Size );

	GSrv_CliInfoStruct* CliInfo;

	_CliList.GetData( true );
	for( int i = 0 ; i < _CliList.size() ; i++ )
	{
		CliInfo = _CliList.GetData( );

		DataGSrvToCli.CliID = CliInfo->ID;
		SendToProxy( CliInfo->ProxyID , sizeof(DataGSrvToCli) , &DataGSrvToCli , Size , Data );
	}
}
//-----------------------------------------------------------------------------------
//讓某 cli 與 某 GSrv 成立新的連線
bool	GSrvNetWakerClass::ConnectCliAndSrv( int GSrvID , int CliID )
{
    Print( Def_PrintLV1 , "ConnectCliAndSrv" , "GSrvID = %d CliID=%d" , GSrvID , CliID );
    SysNet_GSrv_Switch_Proxy_ConnectCliAndGSrv ConnectCliAndGSrv; 
    ConnectCliAndGSrv.CliID  = CliID;
    ConnectCliAndGSrv.GSrvID = GSrvID;

    bool Ret = SendToSwitch( sizeof(ConnectCliAndGSrv) , &ConnectCliAndGSrv  );
    return Ret;
}
//-----------------------------------------------------------------------------------
//與 某個 Client 斷線
bool	GSrvNetWakerClass::CliDisconnect( int CliID )		
{
	if( CliID == -1 )
		return false;

    Print( Def_PrintLV1 , "CliDisconnect" , "~~~" );
    SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv DisconnectCliAndGSrv; 
    DisconnectCliAndGSrv.CliID  = CliID;
    DisconnectCliAndGSrv.GSrvID = _GSrvID;

    bool Ret = SendToSwitch( sizeof(DisconnectCliAndGSrv) , &DisconnectCliAndGSrv );
    return Ret;
}
//-----------------------------------------------------------------------------------
//要求把某個Client logout
void	GSrvNetWakerClass::CliLogout( int CliID, const char* pszReason )
{
    Print( Def_PrintLV2 , "CliLogout" , "Srv CliLogout = %s", pszReason );
    SysNet_Proxy_CliLogoutRequest CliLogout; 

	CliLogout.CliID  = CliID;		
	sprintf( CliLogout.Reason, "%s", pszReason );

    GSrv_CliInfoStruct* CliInfo = _CliList[ CliID ];
    if( CliInfo == NULL )
    { 
        Print( Def_PrintLV1 , "CliLogout" , "CliInfo == NULL" );
        return;
    }

    SendToProxy( CliInfo->ProxyID  , sizeof(CliLogout) , &CliLogout );
}
//-----------------------------------------------------------------------------------
//取得某 Cli 的連線資料
GSrv_CliInfoStruct*    GSrvNetWakerClass::GetCliNetInfo( int CliID )
{
    return  _CliList[ CliID ];
}
//-----------------------------------------------------------------------------------
void	GSrvNetWakerClass::SendFlush()
{
	_NetBSwitch->SendFlush();			            //主要是用連接Switch
    _NetBProxy->SendFlush();			                //主要是用連接Proxy
}

bool    GSrvNetWakerClass::SrvLogout( int SrvID )
{
    SysNet_GSrv_Switch_SrvLogoutRequest Send;
    Send.SrvID = SrvID;

    bool Ret = SendToSwitch( sizeof(Send) , &Send );
    return Ret;
}
//-----------------------------------------------------------------------------------
void GSrvNetWakerClass::Output_NetMsg			( int lv, const char* szOut )
{
	Print( lv , "NetBridge" , szOut );
}
//-----------------------------------------------------------------------------------
int  GSrvNetWakerClass::Output_NetStatus		( SchedularInfo* Obj , void* InputClass )
{
	char szOut[4096];
	GSrvNetWakerClass* pThis = (GSrvNetWakerClass*)InputClass;

	pThis->_NetBSwitch->GetStatusInfo( szOut );
	PrintToFile( Def_PrintLV2, "Output_NetStatus" , "Switch:\n%s", szOut );

	pThis->_NetBProxy->GetStatusInfo( szOut );
	PrintToFile( Def_PrintLV2, "Output_NetStatus" ,"Proxy:\n%s", szOut );

	pThis->_Schedular->Push( Output_NetStatus, 10000, pThis, NULL );

	return 0;
}

unsigned int GSrvNetWakerClass::GetIpNum( int SockID )
{
	GSrv_CliInfoStruct*    CliInfo = GetCliNetInfo( SockID );
	if( CliInfo == NULL )
		return 0;

	return CliInfo->IPnum;
}

//註冊Server的loading 狀況
bool	GSrvNetWakerClass::RegServerLoading( int MaxPlayerCount , int ParallelZoneCount , int PlayerCount[100] , float TotalPlayerRate )
{	
	SysNet_GSrv_Switch_ServerListInfo Send;
	Send.MaxPlayerCount = MaxPlayerCount;
	Send.ParallelZoneCount = ParallelZoneCount;
	memcpy( Send.PlayerCount , PlayerCount , sizeof(Send.PlayerCount) );
	Send.TotalPlayerRate = TotalPlayerRate;

	bool Ret = SendToSwitch( sizeof(Send) , &Send );
	return Ret;
}

//送資料到所有的世界
void	GSrvNetWakerClass::SendToAllOtherWorldSrv( int ToWorldID , int Size , const void* Data )
{
	SysNet_X_DataTransmitToWorldGSrv Send;
	Send.FromSrvType = EM_FromServerType_GSrv;
	Send.DataSize = Size;
	Send.ToWorldID = ToWorldID;
	Send.ToNetID = -1;
	_NetBSwitch->SendWithBuff( _NetSwitchID , sizeof(Send) , &Send , Size , (PVOID)Data );
}
//送資料到所有的世界
void	GSrvNetWakerClass::SendToOtherWorldSrv( int ToWorldID , int ToNetID , int Size , const void* Data )
{
	SysNet_X_DataTransmitToWorldGSrv Send;
	Send.FromSrvType = EM_FromServerType_GSrv;
	Send.DataSize = Size;
	Send.ToWorldID = ToWorldID;
	Send.ToNetID = ToNetID;
	_NetBSwitch->SendWithBuff( _NetSwitchID , sizeof(Send) , &Send , Size , (PVOID)Data );
}

//送資料到所有的世界
void	GSrvNetWakerClass::SendToOtherWorldSrv_GSrvID( int ToWorldID , int ToGSrvType , int ToGSrvID , int Size , const void* Data )
{
	SysNet_X_DataTransmitToWorldGSrv_GSrvID Send;
	Send.FromSrvType = EM_FromServerType_GSrv;
	Send.DataSize = Size;
	Send.ToWorldID = ToWorldID;
	Send.ToGSrvID = ToGSrvID;
	Send.ToGSrvType = ToGSrvType;
	_NetBSwitch->SendWithBuff( _NetSwitchID , sizeof(Send) , &Send , Size , (PVOID)Data );
}

//註冊GSrv NetID
void	GSrvNetWakerClass::RegGSrvNetID( int GSrvType , int GSrvID )
{
	SysNet_GSrv_Switch_RegisterGSrvID Send;
	Send.SrvID = GSrvID;
	Send.SrvType = GSrvType;
	SendToSwitch( sizeof(Send) , &Send );
}

//把某帳號冷凍
void	GSrvNetWakerClass::FreezeAccount( const char* Account , int FreezeType )
{
	SysNet_GSrv_Switch_FreezeAccount Send;
	MyStrcpy( Send.Account , Account , sizeof(Send.Account) );
	Send.FreezeType = FreezeType;
	SendToSwitch( sizeof(Send) , &Send );
}

//PingLog
void	GSrvNetWakerClass::ProxyPingLog(  )
{
	SysNet_GSrv_Proxy_PingLog Send;
	Send.Time = timeGetTime();
	//SendToProxy( ProxyID , sizeof(Send) , &Send );
	SendToAllProxy( sizeof(Send) , &Send );
}

#pragma managed