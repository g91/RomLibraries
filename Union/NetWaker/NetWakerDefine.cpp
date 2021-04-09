#pragma warning (disable:4949)
#include "MyTypeDef.h"
#include "NetWakerDefine.h"

#pragma unmanaged
#include <time.h>

using namespace NetWakerPackage;

/*
void	SwitchInfoStruct:Init()
{
	ID		= -1;			//連線號碼
	WorldID = -1;	
	Port	= 0;		//給Client聯結的Port
    Stat       = EM_NetStat_Unknow;
}
*/
void	ProxyInfoStruct::Init()
{
    Stat        = EM_NetStat_Unknow;

    ID          = -1;	  //連線號碼
//    IPnum       =  0;	  //ip number
	CliIPStr.clear();
	GSrvIPStr.clear();
    GSrvPort    =  0;     //給Server聯結的Port
    ClientPort  =  0;     //給Client聯結的Port

//    IsReady     = false;  
    GSrvConnect.clear();
	CliConnect.clear();
}

void	GSrvInfoStruct::Init()
{
    Stat        = EM_NetStat_Unknow;
    Type    = EM_GSrvType_Passive;
//    IsReady = false;  
    IPnum   = 0;
    ProxyConnect.clear();
	CliConnect.clear();
}

void    CliInfoStruct::Init()
{
    ID          = -1;			
    IPnum       = 0;			
    Stat        = EM_NetStat_Unknow;
    ProxyID     = -1;
    SwitchNetID = -1;
	TempScID	= -1;
	NetCloseType	= EM_ClientNetCloseType_None;


    LastCheckTime = clock();

    Account.clear();
    Password.clear();
    GSrvConnect.clear();
}

void	GSrv_CliInfoStruct::Init() 
{ 
    memset( this , 0 , sizeof(*this) );
}

bool    Map_NetIDtoIDClass::Set( int NetID , int ID )
{
	
    if( _Max_Number_ <= (unsigned)NetID )
    {
        TRACE0( "\nMap_NetIDtoIDClass::Set : Error!!-1-" );
		_LastErrID = 1;
        return false;
    }
	
    if( _Max_Number_ <= (unsigned)ID )
    {
        TRACE0( "\nMap_NetIDtoIDClass::Set : Error!!-2-" );
		_LastErrID = 2;
        return false;
    }

    while( ID >= (int)_IDToNetList.size() )
        _IDToNetList.push_back( -1 );

    while( NetID >= (int)_NetToIDList.size() )
        _NetToIDList.push_back( -1 );

    if( _NetToIDList[ NetID ] != -1 )
    {
        TRACE0( "\n Map_NetIDtoIDClass::Set : Error!! -3-" );
		_LastErrID = 3;
        return false;
    }
    if( _IDToNetList[ ID ] != -1 ) 
    {
        TRACE0( "\n Map_NetIDtoIDClass::Set : Error!! -4-" );
		_LastErrID = 4;
        return false;
    }

    _NetToIDList[ NetID ] = ID;
    _IDToNetList[ ID ]    = NetID;

    return true;
}

bool    Map_NetIDtoIDClass::Cls( int NetID , int ID )
{
    if( NetID == -1 )
    { 
        NetID = GetNetID( ID );
    }
    else if( ID == -1)
    {
        ID = GetID( NetID );
    }
    else
    {
        int TempNetID = GetNetID( ID );
        if( TempNetID != NetID )
        {
            TRACE2( "\n Map_NetIDtoIDClass::Cls Error !! -1- NETID=%d ID=%d" , NetID , ID );
            return false;
        }
    }

    if( unsigned( NetID ) < _NetToIDList.size() )
    {
        _NetToIDList[ NetID ] = -1;
    }
    else if( NetID != -1 )
    {
        TRACE2( "\n Map_NetIDtoIDClass::Cls Error !! -2- NETID=%d ID=%d" , NetID , ID );
    }

    if( unsigned( ID ) < _IDToNetList.size() )
    {
        _IDToNetList[ ID ] = -1;
    }
    else
    {
        TRACE2( "\n Map_NetIDtoIDClass::Cls Error !! -3- NETID=%d ID=%d" , NetID , ID );
    }

    return true;
}

int     Map_NetIDtoIDClass::GetID( int NetID )
{
    if( ( unsigned int )NetID >= _NetToIDList.size() )
        return -1;

    return _NetToIDList[ NetID ];
}
int     Map_NetIDtoIDClass::GetNetID( int ID )
{
    if( ( unsigned int )ID >= _IDToNetList.size() )
        return -1;

    return _IDToNetList[ ID ];
}

#pragma managed