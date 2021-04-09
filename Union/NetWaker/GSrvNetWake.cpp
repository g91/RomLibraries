#include	"GSrvNetWaker.h"
#include	"ControllerClient/ControllerClient.h"
//#include	"../mainproc/Global.h"
#pragma warning (disable:4949)
#pragma unmanaged
/*
NetListener         GSrvNetWaker::_Listener;
GSrvNetWakerClass* 	GSrvNetWaker::_Net          = NULL;
int	                GSrvNetWaker::_ExitFlag     = false;
int                 GSrvNetWaker::_ProcTime     = 10;
*/
//------------------------------------------------------------------------------------------
//紀錄登入事件函式的資料
/*
vector< OnLoginFunctionBase >			GSrvNetWaker::_LoginFunctionList;
vector< OnLogoutFunctionBase >			GSrvNetWaker::_LogoutFunctionList;
vector< OnLoginFailFunctionBase >		GSrvNetWaker::_LoginFailFunctionList;

vector< OnSrvConnectFunctionBase >		GSrvNetWaker::_SrvConnectFunctionList;
vector< OnSrvDisconnectFunctionBase >	GSrvNetWaker::_SrvDisconnectFunctionList;
vector< OnSrvPacketFunctionBase >		GSrvNetWaker::_SrvPacketFunctionList;

vector< OnCliConnectFunctionBase >		GSrvNetWaker::_CliConnectFunctionList;
vector< OnCliDisconnectFunctionBase >	GSrvNetWaker::_CliDisconnectFunctionList;
vector< CallBackStruct > 				GSrvNetWaker::_CliPacketFunctionList;
vector< OnSrvMsgFunctionBase >			GSrvNetWaker::_SrvMsgFunctionList;
*/
//------------------------------------------------------------------------------------------

void	NetListener::OnLogin				( string info )
{
    _Parent->_OnLogin( info );
}
void	NetListener::OnLogout			    ( )
{
    _Parent->_OnLogout( );
}
void	NetListener::OnLoginFailed		    ( SYSLogFailedENUM type )
{
    _Parent->_OnLoginFailed( type );
}

void    NetListener::OnGSrvConnected        ( int id , string GSrvName)
{
    _Parent->_OnGSrvConnected( id , GSrvName );
}
void    NetListener::OnGSrvDisconnected     ( int id )
{
    _Parent->_OnGSrvDisconnected( id );
}
bool    NetListener::OnGSrvPacket           ( int id, int size , void* _data )	
{
	PacketBase* data = (PacketBase*)_data;

    bool Ret;
    int  Time = GetTickCount();
    Ret = _Parent->_OnGSrvPacket( id , size , data );

	//if( Global::_DebugPacket == 1 )
	//{
	//	char		szBuff[65535];
	//	string		buff;
	//	byte*		ptr = (byte*)_data;

	//	for( int i=0; i<size; i++ )
	//	{
	//		sprintf( szBuff, "%d ", *ptr );
	//		buff += szBuff;
	//		ptr++;
	//	}

	//	Print( LV2, "DebugPacket", "PacketID = %d, size = %d, data = %s", data->Command, size, buff.c_str() );
	//	g_pPrint->OutputBuffToFile();
	//}


    Time = GetTickCount() - Time;
	if( Time > 200 )
		Print( LV5 , "OnGSrvPacket" , "OnGSrvPacket %d Proc Time = %d PGID = %d Packet Delay" , id , Time , data->Command );
	else
		Print( LV1 , "OnGSrvPacket" , "OnGSrvPacket %d Proc Time = %d PGID = %d" , id , Time , data->Command );

	


    return Ret;
}
bool    NetListener::OnOtherWorldPacket( int FromWorldID , int FromNetID , int size , void* data )
{
	bool Ret;
	Ret = _Parent->_OnOtherWorldPacket( FromWorldID , FromNetID , size , (PacketBase*)data );
	return Ret;
}

void	NetListener::OnProxyPingLog		( int ProxyID , int Time )
{
	_Parent->_OnProxyPingLog( ProxyID , Time );
}

void	NetListener::OnClientConnected	    ( int id , string accountname  )
{
    _Parent->_OnClientConnected( id , accountname );
}
void	NetListener::OnClientDisconnected   ( int id )
{
    _Parent->_OnClientDisconnected( id );
}
bool	NetListener::OnCliPacket			( int id , int size , void* data )
{
    bool Ret;
    int  Time = timeGetTime();
    Ret = _Parent->_OnCliPacket( id , size , (PacketBase*)data );

    Time = timeGetTime() - Time;
    Print( LV1 , "OnCliPacket" , "OnGSrvPacket %d Proc Time = %d" , id , Time );

    return Ret;
}
void	NetListener::OnServerMsg			( char* Msg)
{
    _Parent->_OnServerMsg( Msg );
}

//------------------------------------------------------------------------------------------
void	GSrvNetWaker::_OnLogin				( string info )
{
    Print( Def_PrintLV1 , "_OnLogin" , "info=%s" , info.c_str() );
    for( myUInt32 i = 0 ; i < _LoginFunctionList.size() ; i++ )
    {
        _LoginFunctionList[i]( info );
    }

	_Net->RegGSrvNetID( _GSrvType , _GSrvID );
}
void	GSrvNetWaker::_OnLogout				( )
{
    Print( Def_PrintLV1 , "_OnLogout" ,"" );
    for( myUInt32 i = 0 ; i < _LogoutFunctionList.size() ; i++ )
    {
        _LogoutFunctionList[i]( );
    }
}
void	GSrvNetWaker::_OnLoginFailed		( SYSLogFailedENUM type )
{
    Print( Def_PrintLV1 , "_OnLoginFailed","" );
    for( myUInt32 i = 0 ; i < _LoginFailFunctionList.size() ; i++ )
    {
        _LoginFailFunctionList[i]( type );
    }
}

void	GSrvNetWaker::_OnGSrvConnected	    ( int id , string GSrvName )
{
    _SrvName[id] = GSrvName;
    Print( Def_PrintLV1 , "_OnGSrvConnected" , "id=%d GSrvName=%s" , id , GSrvName.c_str() );
    for( myUInt32 i = 0 ; i < _SrvConnectFunctionList.size() ; i++ )
    {
        _SrvConnectFunctionList[i]( id , GSrvName );
    }
}
void	GSrvNetWaker::_OnGSrvDisconnected	( int id )
{
    string Str = _SrvName[id];
    if( Str.empty() )
        Str = " ";

    Print( Def_PrintLV1 , "_OnGSrvDisconnected" , "id=%d Name=%s" , id , Str.c_str() );
    for( myUInt32 i = 0 ; i < _SrvDisconnectFunctionList.size() ; i++ )
    {
        _SrvDisconnectFunctionList[i]( id );
    }
    _SrvName.erase( id );
}
bool	GSrvNetWaker::_OnGSrvPacket			( int id , int size , PacketBase* data )
{
    if( (unsigned)data->Command >= _SrvPacketFunctionList.size() )
    {
        Print( Def_PrintLV4 , "_OnSrvPacket" , "( %d ) Packet command too big??" , (unsigned)data->Command );
        return false;
    }
    OnSrvPacketFunctionBase* PGFunc;

    PGFunc = &_SrvPacketFunctionList[ data->Command ];

    if( *PGFunc == NULL )
    {
        Print( Def_PrintLV4 , "_OnSrvPacket" , "( %d ) Packet command no register??" , (unsigned)data->Command );
        return false;
    }

	if( _IsEnableTryException != false )
	{
		__try
		{
			(*PGFunc)( id , size , data );
		}
		__except( ExceptionFilter(GetExceptionCode(), GetExceptionInformation()) )
		{
			Print( LV5 , "_OnGSrvPacket" , "Crash Size=%d Cmd=%d" , size , data->Command );	
		}
	}
	else
	{
	    (*PGFunc)( id , size , data );
	}

    return true;
}
//bool	GSrvNetWaker::_OnGSrvPacket			( int id , int size , PacketBase* data )
bool	GSrvNetWaker::_OnOtherWorldPacket		( int worldid , int netid , int size, PacketBase* data )
{
	if( (unsigned)data->Command >= _OtherWorldPacketFunctionList.size() )
	{
		Print( Def_PrintLV4 , "_OnOtherWorldPacket" , "( %d ) Packet command too big??" , (unsigned)data->Command );
		return false;
	}
	OnOtherWorldPacketFunctionBase* PGFunc;

	PGFunc = &_OtherWorldPacketFunctionList[ data->Command ];

	if( *PGFunc == NULL )
	{
		Print( Def_PrintLV4 , "_OnOtherWorldPacket" , "( %d ) Packet command no register??" , (unsigned)data->Command );
		return false;
	}

	if( _IsEnableTryException != false )
	{
		__try
		{
			(*PGFunc)( worldid , netid , size , data );
		}
		__except( ExceptionFilter(GetExceptionCode(), GetExceptionInformation()) )
		{
			Print( LV5 , "_OnOtherWorldPacket" , "Crash Size=%d Cmd=%d" , size , data->Command );	
		}
	}
	else
	{
		(*PGFunc)( worldid , netid , size , data );
	}

	return true;
}
void	GSrvNetWaker::_OnClientConnected	( int id , string accountname )
{   
    Print( Def_PrintLV1 , "_OnClientConnected" , "id = %d , accountname = %s " , id , accountname.c_str() );
    for( myUInt32 i = 0 ; i < _CliConnectFunctionList.size() ; i++ )
    {
        _CliConnectFunctionList[i]( id , accountname );
    }
}
void	GSrvNetWaker::_OnClientDisconnected	( int id )
{
    Print( Def_PrintLV1 , "_OnClientDisconnected" , "id = %d" , id );
    for( myUInt32 i = 0 ; i < _CliDisconnectFunctionList.size() ; i++ )
    {
        _CliDisconnectFunctionList[i]( id );
    }
}
bool	GSrvNetWaker::_OnCliPacket			( int id , int size , PacketBase* data )
{

    if( (unsigned)data->Command >= _CliPacketFunctionList.size() )
    {
        Print( Def_PrintLV4 , "_OnCliPacket" , "( %d ) Packet command too big??",(unsigned)data->Command );
        return false;
    }
    CallBackStruct *PGFunc;

    PGFunc = &(_CliPacketFunctionList[ data->Command ]);

    if( PGFunc->Fun == NULL )
    {
        Print( Def_PrintLV4 , "_OnCliPacket" , "( %d ) Packet command no register??",(unsigned)data->Command);
        return false;
    }
    //檢查封包Size(*暫時*不檢查)
    if( PGFunc->Max < size || PGFunc->Min > size )
    {
        Print( Def_PrintLV4 , "_OnCliPacket" , "NetID=%d PGCmd=%d Packet size Error?? MaxSize=%d MinSize=%d PGSize=%d" , id , data->Command , PGFunc->Max , PGFunc->Min , size );
        return false;
    }

	if( _IsEnableTryException != false )
	{
		__try
		{
			PGFunc->Fun( id , size , data );
		}
		__except( ExceptionFilter(GetExceptionCode(), GetExceptionInformation()) )
		{
			Print( LV5 , "_OnCliPacket" , "Crash" );
			for( unsigned i = 0 ; i < _OnClientPacketCrashOutput.size() ; i++ )
			{
				_OnClientPacketCrashOutput[i]( id , size , data );
			}		
		}
	}
	else
	{
		PGFunc->Fun( id , size , data );
	}


    return true;
}
void	GSrvNetWaker::_OnServerMsg			( char*	Msg )
{
    Print( Def_PrintLV1 , "_OnServerMsg" , "Msg = %s " , Msg );
}

void	GSrvNetWaker::_OnProxyPingLog			( int ProxyID,int Time )
{

	if( _ProxyPingLogFunc != NULL )
		_ProxyPingLogFunc( ProxyID,Time);
}

//------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------
//GateWay 資料交換處理
bool	GSrvNetWaker::Process( )
{
    _Net->Process( );
    return !_ExitFlag;
}
//------------------------------------------------------------------------------------------
//連到 Switch Server
void GSrvNetWaker::Connect( const char* IP , int Port , const char* SrvName )
{
    //設定 GSrv 的名稱
    _Net->SetGSrvName( SrvName );
    _Net->ConnectSwitch( IP , Port );

}
//------------------------------------------------------------------------------------------
//設定是否要使用Try Exception
void	GSrvNetWaker::SetEnabledTryException	( bool Enabled )
{
	_IsEnableTryException = Enabled;
}
//------------------------------------------------------------------------------------------
GSrvNetWaker::GSrvNetWaker( )
{
    _ExitFlag     = false;
    _ProcTime     = 10;
	_IsEnableTryException = false;

    _Listener     = NEW NetListener( this );

    _Net = NEW (GSrvNetWakerClass);
    _Net->SetListener( _Listener );

	_ProxyPingLogFunc = NULL;
}
//------------------------------------------------------------------------------------------
GSrvNetWaker::~GSrvNetWaker( )
{
    if( _Net != NULL )
        delete _Net;

    if( _Listener != NULL )
        delete _Listener;

}
//------------------------------------------------------------------------------------------
//設定聯線模式
void GSrvNetWaker::ConnectType( SYSGSrvTypeENUM Type )
{
    _Net->ConnectType( Type );
}
//------------------------------------------------------------------------------------------
//把自己Logout
void GSrvNetWaker::Logout( )
{
    _Net->Logout();
}
//------------------------------------------------------------------------------------------
//讓某 cli 與 某 GSrv 成立新的連線
bool    GSrvNetWaker::ConnectCliAndSrv( int SrvID , int CliID )
{
    return _Net->ConnectCliAndSrv( SrvID , CliID );
}
//------------------------------------------------------------------------------------------
//與 某個 Client 斷線
bool	GSrvNetWaker::CliDisconnect( int CliID )
{
    return _Net->CliDisconnect( CliID );
}
//------------------------------------------------------------------------------------------
//要求把某個Client logout
void	GSrvNetWaker::CliLogout( int CliID, int SrvType, int SrvID, const char* _Account, const char* _Reason )
{
	if( CliID == -1 )
		return;

	char szReason[1024];
	char szServer[256];

	vector< string > vecSrvType;

	vecSrvType.push_back( "MASTER" );
	vecSrvType.push_back( "LOCAL" );
	vecSrvType.push_back( "CHAT" );
	vecSrvType.push_back( "DATACENTER" );
	vecSrvType.push_back( "PARTITION" );
	vecSrvType.push_back( "GMTOOLS" );
	vecSrvType.push_back( "PLAYERCENTER" );
	vecSrvType.push_back( "UNKNOWTYPE 1" );
	vecSrvType.push_back( "UNKNOWTYPE 2" );
	vecSrvType.push_back( "UNKNOWTYPE 3" );
	vecSrvType.push_back( "UNKNOWTYPE 4" );
	vecSrvType.push_back( "UNKNOWTYPE 5" );

	if( SrvType < (int)vecSrvType.size() && SrvType >= 0 )
	{
		sprintf( szServer, "%s", vecSrvType[SrvType].c_str() );
	}
	else
	{
		sprintf( szServer, "ERROR");
	}
	
	sprintf( szReason, "%s %d kick [ %s ][%d] - [ %s ]", szServer, SrvID, _Account, CliID, _Reason );

    _Net->CliLogout( CliID, szReason );
}
//------------------------------------------------------------------------------------------
////送封包給Server or Client
bool	GSrvNetWaker::SendToSrv(int SrvID , int Size , const void* Buf)	
{
    return _Net->SendToGSrv( SrvID , Size , Buf );
}
bool	GSrvNetWaker::SendToCli(int CliID , int ProxyID , int Size , const void* Buf)	
{
    return _Net->SendToCli( CliID , ProxyID , Size , (void*)Buf );
}
bool	GSrvNetWaker::SendToCli(int CliID , int Size , const void* Buf)	
{
    return _Net->SendToCli( CliID , Size , (void*)Buf );
}
void	GSrvNetWaker::SendToAllSrv( int Size , const void*	Buf)
{
    _Net->SendToAllGSrv( Size , (void*)Buf );
}
void	GSrvNetWaker::SendToAllCli( int Size , const void*	Buf)
{
    _Net->SendToAllCli( Size , (void*)Buf );
}
void	GSrvNetWaker::SendToAllPlayer( int Size , const void*	Buf)
{
	_Net->SendToAllPlayer( Size , (void*)Buf );
}

void	GSrvNetWaker::SendToOtherWorld_NetID( int WorldID , int NetID , int Size , const void* Buf )
{
	_Net->SendToOtherWorldSrv( WorldID , NetID , Size , Buf );
}
void	GSrvNetWaker::SendToOtherWorld_GSrvType( int WorldID , int GSrvType , int GSrvID , int Size , const void* Buf )
{
	_Net->SendToOtherWorldSrv_GSrvID( WorldID , GSrvType , GSrvID , Size , Buf );
}
/*
bool	GSrvNetWaker::SendToAccount( int Size , const void* Buf )
{
	return _Net->SendToAccount( Size , Buf );
}*/
//------------------------------------------------------------------------------------------
//***********************************************************************************
//
//***********************************************************************************
 
bool    GSrvNetWaker::RegOnLoginFunction			( OnLoginFunctionBase Func )
{
    _LoginFunctionList.push_back( Func );
    return true;
}
bool    GSrvNetWaker::RegOnLogoutFunction			( OnLogoutFunctionBase Func )
{
    _LogoutFunctionList.push_back( Func );
    return true;
}
bool    GSrvNetWaker::RegOnLoginFailFunction		( OnLoginFailFunctionBase Func ) 
{
    _LoginFailFunctionList.push_back( Func );
    return true;
}
bool    GSrvNetWaker::RegOnSrvConnectFunction		( OnSrvConnectFunctionBase Func )
{
    _SrvConnectFunctionList.push_back( Func );
    return true;
}
bool    GSrvNetWaker::RegOnSrvDisConnectFunction	( OnSrvDisconnectFunctionBase Func )
{
    _SrvDisconnectFunctionList.push_back( Func );
    return true;
}

bool    GSrvNetWaker::RegOnCliConnectFunction		( OnCliConnectFunctionBase Func )
{
    _CliConnectFunctionList.push_back( Func );
    return true;
}

bool    GSrvNetWaker::RegOnCliDisconnectFunction	( OnCliDisconnectFunctionBase Func )
{
    _CliDisconnectFunctionList.push_back( Func );
    return true;
}

bool	GSrvNetWaker::RegOnClientPacketCrashOutput( OnClientPacketCrashOutputBase Func )
{
    _OnClientPacketCrashOutput.push_back( Func );
    return true;
}

bool    GSrvNetWaker::RegOnSrvMsgFunction		(  OnSrvMsgFunctionBase Func)
{
    _SrvMsgFunctionList.push_back( Func );
    return true;
}

void	GSrvNetWaker::RegOnProxyPingLog		( OnProxyPingLogFunctionBase Func )
{
	_ProxyPingLogFunc = Func;
}

//已登入過的封包id不可重覆登入
bool    GSrvNetWaker::RegOnCliPacketFunction		( int PGid , OnCliPacketFunctionBase Func , int MinSize , int MaxSize )
{
    CallBackStruct	Temp;

    if( PGid < 0  )
        return false;

    while(  (int)_CliPacketFunctionList.size() <= PGid )
        _CliPacketFunctionList.push_back( Temp );

	if( _CliPacketFunctionList[ PGid ].Fun != NULL )
	{
		char Buf[512];
		sprintf( Buf , "RegOnCliPacketFunction PGId=%d Error", PGid  );
		PrintToController(true, Buf );
	}
    Temp.Fun = Func;
    Temp.Max = MaxSize+2;
    Temp.Min = MinSize;

    _CliPacketFunctionList[ PGid ] = Temp;

    return true;
}
//------------------------------------------------------------------------------------------
bool    GSrvNetWaker::RegOnSrvPacketFunction	    ( int PGid , OnCliPacketFunctionBase  Func )
{
    if( PGid < 0  )
        return false;

    while(  (int)_SrvPacketFunctionList.size() <= PGid )
        _SrvPacketFunctionList.push_back( NULL );

	if( _SrvPacketFunctionList[ PGid ] != NULL )
	{
		char Buf[512];
		sprintf( Buf , "RegOnSrvPacketFunction PGId=%d Error", PGid  );
		PrintToController(true, Buf );
	}


    _SrvPacketFunctionList[ PGid ] = Func;

    return true;
}

bool    GSrvNetWaker::RegOnOtherWorldPacketFunction	    ( int PGid , OnOtherWorldPacketFunctionBase  Func )
{
	if( PGid < 0  )
		return false;

	while(  (int)_OtherWorldPacketFunctionList.size() <= PGid )
		_OtherWorldPacketFunctionList.push_back( NULL );

	if( _OtherWorldPacketFunctionList[ PGid ] != NULL )
	{
		char Buf[512];
		sprintf( Buf , "RegOnOtherWorldPacketFunction PGId=%d Error", PGid  );
		PrintToController(true, Buf );
	}

	_OtherWorldPacketFunctionList[ PGid ] = Func;

	return true;
}
//------------------------------------------------------------------------------------------
GSrv_CliInfoStruct*    GSrvNetWaker::GetCliNetInfo( int CliID ) 
{
    return _Net->GetCliNetInfo( CliID );
}
//送資料處理
void	GSrvNetWaker::SendFlush( )
{
	_Net->SendFlush();
}
//------------------------------------------------------------------------------------------
void	GSrvNetWaker::SrvLogout( int SrvID )
{
    _Net->SrvLogout( SrvID );
}
//------------------------------------------------------------------------------------------
//註冊Server的loading 狀況
bool	GSrvNetWaker::RegServerLoading		( int MaxPlayerCount , int ParallelZoneCount , int PlayerCount[100] , float TotalPlayerCount )
{
	return _Net->RegServerLoading( MaxPlayerCount , ParallelZoneCount , PlayerCount , TotalPlayerCount );
}

//取得Ip Num
unsigned int GSrvNetWaker::GetIpNum( int CliID )
{
	return _Net->GetIpNum( CliID );
}

//註冊GSrv NetID
void	GSrvNetWaker::RegGSrvNetID( int GSrvType , int GSrvID )
{
	_Net->RegGSrvNetID( GSrvType , GSrvID );
	_GSrvType = GSrvType;			//處理世界轉送封包註冊
	_GSrvID = GSrvID;
}


//帳號冷凍
void	GSrvNetWaker::FreezeAccount( const char* Account , int FreezeType )
{
	_Net->FreezeAccount( Account , FreezeType );
}

void	GSrvNetWaker::ProxyPingLog(  )
{
	_Net->ProxyPingLog( );
}
#pragma managed