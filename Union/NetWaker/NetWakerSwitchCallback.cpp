#include "smallobj/SmallObj.h"
#include "GSrvNetWakerClass.h"
#include "RemotetableInput/RemotableInput.h"
#pragma warning (disable:4949)
#pragma unmanaged

//----------------------------------------------------------------------------------
void     GSrvNetWakerClass::_PGxUnKnoew( int NetID,PGSysNULL* PG,int Size)
{
    Print( Def_PrintLV1 , "_PGxUnKnoew" , "(Switch:%d) : Command = %d" , NetID ,  PG->Command );
}
//---------------------------------------------------------------------------------
//重新連結 Switch Server
int    GSrvNetWakerClass::_ReConnectSwitch( SchedularInfo* Obj , void* InputClass )
{
    Print( Def_PrintLV1 , "_ReConnectSwitch" , "Try to connect to Switch Server ....." );
    GSrvNetWakerClass* Class = (GSrvNetWakerClass*)InputClass;

	int IPnum = ConvertIP( (char*)Class->_SwitchAddrIP.c_str() );

    Class->_NetBSwitch->Connect( IPnum  , Class->_SwitchAddrPort , Class->_SwitchBListener );
    return 0;
}
//---------------------------------------------------------------------------------
void	GSrvNetWakerClass::_OnSwitchConnected       ( int NetID )
{
    _NetSwitchID = NetID;


	char Buf[256];
	char OrgTitleStr[256];
	GetConsoleTitleA( OrgTitleStr ,sizeof( OrgTitleStr ) );
	sprintf(Buf,"[OK]%s", OrgTitleStr );
	SetConsoleTitleA(Buf);

	//g_remotableInput.OutputStartupMessage();


	// Add command

		// 30 min
		/*
		printf( "Add Command,{79CC1671-A46D-478a-9497-4933581B5B03},Stop all after 30min,STOP_ALL_30" );

		// 15 min
		printf( "Add Command,{79CC1671-A46D-478a-9497-4933581B5B03},Stop all after 15min,STOP_ALL_15" );

		// 5 min
		printf( "Add Command,{79CC1671-A46D-478a-9497-4933581B5B03},Stop all after 5min,STOP_ALL_5" );

		// 1 min
		printf( "Add Command,{79CC1671-A46D-478a-9497-4933581B5B03},Stop all after 1min,STOP_ALL_1" );
		*/

		// BoradCast

}
void	GSrvNetWakerClass::_OnSwitchDisconnected    ( int NetID )
{
    _NetSwitchID = -1;
    if( _IsReady != false )
    {
        _Listener->OnLogout( );
    }
    else
    {
        _Listener->OnLoginFailed( EM_NetLogFailed_UnknowError );
    }
    
    _IsReady = false;
}
bool	GSrvNetWakerClass::_OnSwitchPacket          ( DWORD NetID, DWORD Size , PVOID Data )
{
	static PacketProcTimeLogClass TimeLog;

    PGSysNULL* PG = ( PGSysNULL *)Data;
    //------------------------------------------------------------------------------
    if(_PGInfo.size() <= (UINT32)PG->Command )
    {
        Print( Def_PrintLV4 , "_OnSwitchPacket" , "(Switch:%d) : md=%d Packet command too big" , NetID , PG->Command );
        return false;
    }

    //權限檢查
    PGBaseInfo *PGinfo = &( _PGInfo[ PG->Command ] );

	/*
    //封包大小檢查
    if(PGinfo->PGSize <= (int)Size)
    {
        Print( Def_PrintLV4 , "_OnSwitchPacket" , "(Switch:%d) : Size=%d cmd=%d Packet size too big" , NetID , Size , PG->Command );
        return false;
    }
	*/

    //來源檢查
    if( PGinfo->Type.Switch == false )
    {
        Print( Def_PrintLV4 , "_OnSwitchPacket" , "(Switch:%d) : Packet source error (PGinfo->Type.Switch == false ??) cmd=%d" , NetID , PG->Command );
        return false;
    }

	if( G_SwitchPacketTimeProcLog != false )
	{
		TimeLog.Enter( );
		//呼叫Callback 處理
		(this->*_PGCallBack[ PG->Command ])( NetID , PG , Size );
		TimeLog.Leave( PG->Command );		

		if( TimeLog.LogTime() >= 60000 )
		{
			//////////////////////////////////////////////////////////////////////////
			vector< TimeLogBaseStruct >& PGLog = TimeLog.PGLog();
			for( unsigned i = 0 ; i < PGLog.size() ; i++ )
			{
				if(		PGLog[i].MaxProcTime > 10 
					||	PGLog[i].ProcCount > 50 )
				{
					Print( Def_PrintLV2 , "Packet ProcTime" , "( Switch -> GSrv Packet ENUM) PGID=%d Min =%d Max=%d Avg=%d Count=%d" 
						, i
						, PGLog[i].MinProcTime 
						, PGLog[i].MaxProcTime 
						, PGLog[i].AvgProcTime() 
						, PGLog[i].ProcCount  );	
				}
			}
			TimeLog.Clear();
		}    
	}
	else
	{
		//呼叫Callback 處理
		(this->*_PGCallBack[ PG->Command ])( NetID , PG , Size );
	}

    return true;
}
void	GSrvNetWakerClass::_OnSwitchConnectFailed   ( int NetID )
{
    //重新連線
    Schedular()->Push( _ReConnectSwitch , 5000 , this , NULL );
}
//---------------------------------------------------------------------------------
// 連結到Switch 要求連結
void    GSrvNetWakerClass::_PGxSwitchConnectNodify( int NetID , PGSysNULL* PG , int Size )
{
    Print( Def_PrintLV1 , "_PGxSwitchConnectNodify" , "(Switch:%d)" , NetID );

    SysNet_Switch_GSrv_NotifyConnect*    NotifyConnect = (SysNet_Switch_GSrv_NotifyConnect*)PG;
    _GSrvID = NotifyConnect->GSrvID;
	_WorldID = NotifyConnect->WorldID;

    SysNet_GSrv_Switch_Connect          Connect;
    memcpy( Connect.GSrvName , _GSrvName , sizeof(Connect.GSrvName) );
    Print( Def_PrintLV1 , "_PGxSwitchConnectNodify" , "GSrv -> Switch  Connect to Switch..... GSrvName = %s" , _GSrvName );

    SendToSwitch( sizeof( Connect ) , &Connect );

    _Listener->OnLogin( "None" );
    _IsReady = true;


    //設定GSrv的模式
    if( _Type != EM_GSrvType_Passive )
    {
        SysNet_GSrv_Switch_SetActive    SetActiveType;
        SetActiveType.Type = _Type;

        SendToSwitch( sizeof( SetActiveType ) , &SetActiveType );
    }
}
//---------------------------------------------------------------------------------
//通知Proxy Addr並且要求連結
void    GSrvNetWakerClass::_PGxProxyAddr( int NetID , PGSysNULL* PG , int Size )
{
    Print( Def_PrintLV1 , "_PGxProxyAddr" , "(Switch:%d) receive Proxy Host . Connect to Proxy!" , NetID );

    SysNet_Switch_GSrv_ProxyAddr*    ProxyAddr = (SysNet_Switch_GSrv_ProxyAddr*)PG;

    //連結Proxy
    if( _ProxyConnect.find( ProxyAddr->ProxyID ) != _ProxyConnect.end() )
    {
        //要求連結 Proxy 有問題
        Print( Def_PrintLV4 , "_OnProxyConnected" , "Proxy nConnect Request Error!! Proxy(%d) already Exist!!" , ProxyAddr->ProxyID );
        return;
    }
    
    _ConnectProxy( ProxyAddr->GSrvIPStr , ProxyAddr->Port );
//    Obj->_ProxyConnect.insert( ProxyAddr->ProxyID );
}
//---------------------------------------------------------------------------------
//要求GSrv與某Proxy斷線
void    GSrvNetWakerClass::_PGxProxyDisconnectRequest( int NetID , PGSysNULL* PG , int Size )
{
    SysNet_Switch_GSrv_ProxyDisconnectRequest*  ProxyDisconnectRequet = ( SysNet_Switch_GSrv_ProxyDisconnectRequest* ) PG;

    Print( Def_PrintLV1 , "_PGxProxyDisconnectRequest" , "(Switch:%d) ProxyID = %d" , NetID , ProxyDisconnectRequet->ProxyID ); 

    int ProxyNetID = _ProxyNetToIDMap.GetNetID( ProxyDisconnectRequet->ProxyID );
    if( ProxyNetID == -1 )
    {
		//尚未註冊完成 or 已先斷線了
        Print( Def_PrintLV4 , "_PGxProxyDisconnectRequest" , "Proxy NetID not find , ProxyID = %d Not registers completes!!!!!!! or Proxy already disconnect " , ProxyDisconnectRequet->ProxyID );
        return;
    }
    _NetBProxy->Close( ProxyNetID );
}
//---------------------------------------------------------------------------------
//Switch 通知 GSrv有 新的GSrv成立連線
void    GSrvNetWakerClass::_PGxOnNewGSrvConnect( int NetID , PGSysNULL* PG , int Size )
{
    Print( Def_PrintLV1 , "_PGxOnNewGSrvConnect" , "_PGxOnNewGSrvConnect " );
    SysNet_Switch_GSrv_OnGSrvConnect* OnGSrvConnect = (SysNet_Switch_GSrv_OnGSrvConnect*)PG;
    _GSrvNameList.Add( OnGSrvConnect->GSrvName , OnGSrvConnect->GSrvID );
    _Listener->OnGSrvConnected( OnGSrvConnect->GSrvID , OnGSrvConnect->GSrvName );
/*
    if(     _GSrvID == NetID 
        &&  _Type != EM_GSrvType_Passive)
    {
        SysNet_GSrv_Switch_SetActive    SetActiveType;
        SetActiveType.Type = _Type;

        SendToSwitch( sizeof( SetActiveType ) , &SetActiveType );
    }
*/
}
//---------------------------------------------------------------------------------
//Switch 通知 GSrv有 新的GSrv斷線
void    GSrvNetWakerClass::_PGxOnGSrvDisConnect( int NetID , PGSysNULL* PG , int Size )
{
    Print( Def_PrintLV1 , "_PGxOnGSrvDisConnect" , "_PGxOnGSrvDisConnect " );
    SysNet_Switch_GSrv_OnGSrvDisConnect* OnGSrvDisConnect = (SysNet_Switch_GSrv_OnGSrvDisConnect*)PG;
    _GSrvNameList.Del( OnGSrvDisConnect->GSrvID );
    _Listener->OnGSrvDisconnected( OnGSrvDisConnect->GSrvID  );    
}
//---------------------------------------------------------------------------------
//GSrv轉送過來的資料
void    GSrvNetWakerClass::_PGxDataGSrvToGSrv( int NetID , PGSysNULL* PG , int Size )
{
    SysNet_Switch_GSrv_DataGSrvToGSrv* DataGSrvToGSrv = (SysNet_Switch_GSrv_DataGSrvToGSrv*)PG;
    _Listener->OnGSrvPacket( DataGSrvToGSrv->GSrvID , DataGSrvToGSrv->Size ,  DataGSrvToGSrv->Data );
}

void    GSrvNetWakerClass::_PGxDataGSrvToGSrv_Zip( int NetID , PGSysNULL* PG , int Size )
{
	char	TempData[0x40000];
	SysNet_Switch_GSrv_DataGSrvToGSrv_Zip* DataGSrvToGSrv = (SysNet_Switch_GSrv_DataGSrvToGSrv_Zip*)PG;
	if( DataGSrvToGSrv->Size >= 0x40000 )
	{
		Print( LV5 , "_PGxDataGSrvToGSrv_Zip" ,  "DataGSrvToGSrv->Size(%d) >= 0x20000 " , DataGSrvToGSrv->Size );
		return;
	}

	MyLzoClass myZip;

	int RetSize = myZip.Decode( (const char*)DataGSrvToGSrv->Data , DataGSrvToGSrv->ZipSize , sizeof(TempData) , (char*)&TempData );


	_Listener->OnGSrvPacket( DataGSrvToGSrv->GSrvID , DataGSrvToGSrv->Size ,  &TempData );
}
//---------------------------------------------------------------------------------
void GSrvNetWakerClass::_PGDataTransmitToWorldGSrv( int NetID , PGSysNULL* Data , int Size )
{
	SysNet_X_DataTransmitToWorldGSrv* PG = (SysNet_X_DataTransmitToWorldGSrv*)Data;
	_Listener->OnOtherWorldPacket( PG->FromWorldID , PG->FromNetID , PG->DataSize , PG->Data );
}
//---------------------------------------------------------------------------------
void GSrvNetWakerClass::_PGDataTransmitToWorldGSrv_GSrvID( int NetID , PGSysNULL* Data , int Size )
{
	SysNet_X_DataTransmitToWorldGSrv_GSrvID* PG = (SysNet_X_DataTransmitToWorldGSrv_GSrvID*)Data;
	_Listener->OnOtherWorldPacket( PG->FromWorldID , PG->FromNetID , PG->DataSize , PG->Data );
}
//---------------------------------------------------------------------------------
#pragma managed