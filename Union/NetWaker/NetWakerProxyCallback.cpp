#include "GSrvNetWakerClass.h"
#pragma warning (disable:4949)
#pragma unmanaged
void	GSrvNetWakerClass::_OnProxyConnected       ( int NetID )
{
 //   SysNet_GSrv_Proxy_ConnectRequest    ConnectRequest;
 //   ConnectRequest.GSrvID = _GSrvID;

 ///   SendToProxy( NetID , sizeof( ConnectRequest ) , &ConnectRequest );
}
void	GSrvNetWakerClass::_OnProxyDisconnected    ( int NetID )
{
    bool    CheckFlag;
    //清除此Proxy的相關資料
    int ProxyID = _ProxyNetToIDMap.GetID( NetID );

    CheckFlag = (_ProxyConnect.find( ProxyID ) != _ProxyConnect.end() );
    if(     ProxyID == -1 
        ||  CheckFlag != true )
    {
		//尚未註冊完成就斷線
        Print( Def_PrintLV5 , "_OnProxyDisconnected" , "Proxy NetID = %d Not yet registers completes!!!!" , NetID );
        _ProxyNetToIDMap.Cls( NetID , -1 );
        if( CheckFlag != false )
            _ProxyConnect.erase( ProxyID );
        return;
    }

    _ProxyNetToIDMap.Cls( NetID , -1 );
    _ProxyConnect.erase( ProxyID );
    
    //通知 Switch  Proxy 斷線
    Print( Def_PrintLV5 , "_OnProxyDisconnected" , "ProxyID = %d NetID = %d" , ProxyID , NetID );
    SysNet_GSrv_Switch_ProxyDisconnect ProxyDisconnect;
    ProxyDisconnect.ProxyID = ProxyID;
    SendToSwitch( sizeof(ProxyDisconnect) , &ProxyDisconnect );

	_CliList.GetData( true );
	while( 1 )
	{
		GSrv_CliInfoStruct* Data = _CliList.GetData();

		if( Data == NULL )
			break;

		if( Data->ProxyID == ProxyID )
		{
			_Listener->OnClientDisconnected( Data->ID );
			_CliList.Del( Data->ID );			
		}		
	}
}
bool	GSrvNetWakerClass::_OnProxyPacket          ( DWORD NetID, DWORD Size, PVOID Data)
{
    //取得封包資料
   // ObjNetGetBuffer(hIncomingData,Size,Data);
    PGSysNULL* PG = ( PGSysNULL *)Data;

    //------------------------------------------------------------------------------
    {
        if(_PGInfo.size() <= (UINT32)PG->Command )
        {
            Print( Def_PrintLV4 , "_OnProxyPacket" , "(Proxy:%d): cmd=%d , Packet command too big" , NetID , PG->Command);
            return false;
        }

        //權限檢查
        PGBaseInfo *PGinfo = &( _PGInfo[ PG->Command ] );

/*        //封包大小檢查
        if(PGinfo->PGSize <= Size)
        {
            Print( Def_PrintLV4 , "_OnProxyPacket" , "(Proxy:%d) : Size=%d , cmd = %d , Packet size too big" , NetID , Size , PG->Command );
            return false;
        }
		*/

        //來源檢查
        if( PGinfo->Type.Proxy == false )
        {
            Print( Def_PrintLV4 , "_OnProxyPacket" , "(Proxy:%d): Packet source error , (PGinfo->Type.Proxy == false ??) cmd=%d" , NetID , PG->Command);
            return false;
        }

        //呼叫Callback 處理
        (this->*_PGCallBack[ PG->Command ])( NetID , PG , Size );
    }

    return true;
    
}
//-----------------------------------------------------------------------------------
void	GSrvNetWakerClass::_OnProxyConnectFailed   ( int NetID )
{
	Print( Def_PrintLV4 , "_OnProxyConnectFailed" ," NetID =%d WSAGetLastError( %d )", NetID , WSAGetLastError() );
}
//-----------------------------------------------------------------------------------
// 連結到Proxy 成?
void	GSrvNetWakerClass::_PGxProxyConnectNotify( int NetID , PGSysNULL* PG , int Size )
{
    SysNet_Proxy_GSrv_ConnectNotify* ConnectNotify = (SysNet_Proxy_GSrv_ConnectNotify*)PG;

    Print( Def_PrintLV1 , "_PGxProxyConnectNotify" , "(Proxy:%d)_PGxProxyConnectNotify" , NetID );
    SysNet_GSrv_Proxy_Connect   Connect;
    Connect.GSrvID = _GSrvID;
    memcpy( Connect.GSrvName , _GSrvName , sizeof(Connect.GSrvName) );

    Print( Def_PrintLV1 , "_PGxProxyConnectNotify" , "GSrv -> Proxy  Connect to Proxy....." );
    SendToProxy_NETID( NetID , sizeof( Connect ) , &Connect );

    //建立Proxy的相關資料
    _ProxyConnect.insert( ConnectNotify->ProxyID );
    _ProxyNetToIDMap.Set( NetID , ConnectNotify->ProxyID );

    //通知proxy 此 GSrv的模式
//    SysNet_GSrv_Proxy_SetActive SetActive;
//    SetActive.GSrvID = Obj->_GSrvID;
//    SetActive.Type   = Obj->_Type;
//    Obj->SendToProxy( NetID , sizeof(SetActive) , &SetActive );
}
//-----------------------------------------------------------------------------------
//與 Cli 成立連線
void	GSrvNetWakerClass::_PGxCliOnConnect( int NetID , PGSysNULL* PG , int Size )
{
    Print( Def_PrintLV1 , "_PGxCliOnConnect" , "~~~" );
    SysNet_Proxy_GSrv_CliOnConnect* CliOnConnect = (SysNet_Proxy_GSrv_CliOnConnect*)PG;

    GSrv_CliInfoStruct* CliInfo = _CliList.New(CliOnConnect->CliID );

	if( CliInfo == NULL )
		return;

    CliInfo->Account = CliOnConnect->Account;
    CliInfo->ProxyID = _ProxyNetToIDMap.GetID( NetID );
	CliInfo->IPnum	 = CliOnConnect->IpNun;
    
    _Listener->OnClientConnected( CliOnConnect->CliID , CliOnConnect->Account );
}
//-----------------------------------------------------------------------------------
//與 Cli 斷線
void	GSrvNetWakerClass::_PGxCliOnDisonnect( int NetID , PGSysNULL* PG , int Size )
{
    Print( Def_PrintLV1 , "_PGxCliOnDisConnect" , "~~~" );
    SysNet_Proxy_GSrv_CliOnDisConnect* CliOnDisConnect = (SysNet_Proxy_GSrv_CliOnDisConnect*)PG;

    //obj->_CliConnect.erase( CliOnDisConnect->CliID );
    _CliList.Del( CliOnDisConnect->CliID );
    _Listener->OnClientDisconnected( CliOnDisConnect->CliID );

}
//-----------------------------------------------------------------------------------
//Client 轉送過來的資料
void    GSrvNetWakerClass::_PGxDataCliToGSrv( int NetID , PGSysNULL* PG , int Size )
{
    SysNet_Proxy_GSrv_DataCliToGSrv* DataCliToGSrv = (SysNet_Proxy_GSrv_DataCliToGSrv*)PG;
	_ClientDataKey = DataCliToGSrv->ClientDataKey;

	if( _ClientDataKey != -1 )
		SendToCli_ClientKey( DataCliToGSrv->CliID , _ClientDataKey );

    _Listener->OnCliPacket( DataCliToGSrv->CliID , DataCliToGSrv->Size ,  DataCliToGSrv->Data );

	if( _ClientDataKey != -1 )
		SendToCli_ClientKey( DataCliToGSrv->CliID , -1 );

	_ClientDataKey = -1;
}

void    GSrvNetWakerClass::_PGxPingLog( int NetID , PGSysNULL* Data , int Size )
{
	SysNet_Proxy_GSrv_PingLog* PG = (SysNet_Proxy_GSrv_PingLog*)Data;
	_Listener->OnProxyPingLog( PG->ProxyID , timeGetTime() - PG->Time );
}

#pragma managed