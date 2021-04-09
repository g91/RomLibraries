#include "AllOnlinePlayerInfo.h"
#include "MainProc/Global.h"
//////////////////////////////////////////////////////////////////////////
AllOnlinePlayerInfoClass *AllOnlinePlayerInfoClass::_This = NULL;

bool AllOnlinePlayerInfoClass::Init( )
{
	if( _This != NULL )
		return false;
	_This = NEW( AllOnlinePlayerInfoClass );
	return true;
}

bool AllOnlinePlayerInfoClass::Release( )
{
	if( _This == NULL )
		return false;

	delete _This;
	_This = NULL;

	return true;
}
//////////////////////////////////////////////////////////////////////////
AllOnlinePlayerInfoClass::AllOnlinePlayerInfoClass()
{

}
AllOnlinePlayerInfoClass::~AllOnlinePlayerInfoClass()
{
	Clear();
}	

bool	AllOnlinePlayerInfoClass::AddorSetPlayer( OnlinePlayerInfoStruct& Info )
{
//	strlwr( (char*)Info.Name.Begin() );
	OnlinePlayerInfoStruct* OldInfo = GetInfo_ByDBID( Info.DBID );
	if( OldInfo != NULL )
	{	
		//資料比較
		if( OldInfo->GUID != Info.GUID )
		{
			_GUIDMap.erase( OldInfo->GUID );
			_GUIDMap[ Info.GUID ] = OldInfo;
		}

		if( strcmp( OldInfo->Name.Begin() , Info.Name.Begin() ) != 0 )
		{
			_NameMap.erase( OldInfo->Name.Begin() );
			_NameMap[ Info.Name.Begin() ]  = OldInfo;
		}

		*OldInfo = Info;
		return false;
	}
	else
	{
		if( _UnUsedData.size() == 0 )
		{
			OldInfo = NEW(OnlinePlayerInfoStruct);
		}
		else
		{
			OldInfo = _UnUsedData.back();
			_UnUsedData.pop_back();
		}
		
		*OldInfo = Info;

		_GUIDMap[ Info.GUID ] = OldInfo;
		_DBIDMap[ Info.DBID ]  = OldInfo;
		_SockIDMap[ Info.SockID ]  = OldInfo;
		_NameMap[ Info.Name.Begin() ]  = OldInfo;
		_AccountMap[ Info.Account.Begin() ]  = OldInfo;
	}

	return true;
}
bool	AllOnlinePlayerInfoClass::DelPlayer( int DBID )
{
	OnlinePlayerInfoStruct* OldInfo = GetInfo_ByDBID( DBID );
	if( OldInfo == NULL )
		return false;

	_GUIDMap.erase		( OldInfo->GUID );
	_DBIDMap.erase		( OldInfo->DBID );
	_SockIDMap.erase	( OldInfo->SockID );
	_NameMap.erase		( OldInfo->Name.Begin() );
	_AccountMap.erase	( OldInfo->Account.Begin() );
	

	_UnUsedData.push_back( OldInfo );

	return true;
}


OnlinePlayerInfoStruct*	AllOnlinePlayerInfoClass::GetInfo_ByName( char* Name )
{
//	string Name = _Name;
//	strlwr( (char*)Name.c_str() );
	map< string , OnlinePlayerInfoStruct* , StringICmp >::iterator Iter;
	Iter = _NameMap.find( Name );
	if( Iter == _NameMap.end() )
		return NULL;

	return Iter->second;
}
OnlinePlayerInfoStruct*	AllOnlinePlayerInfoClass::GetInfo_ByAccount( char* Account )
{
	map< string , OnlinePlayerInfoStruct* , StringICmp >::iterator Iter;
	Iter = _AccountMap.find( Account );
	if( Iter == _AccountMap.end() )
		return NULL;

	return Iter->second;
}
OnlinePlayerInfoStruct*	AllOnlinePlayerInfoClass::GetInfo_ByDBID( int DBID )
{
	map< int , OnlinePlayerInfoStruct* >::iterator Iter;
	Iter = _DBIDMap.find( DBID );
	if( Iter == _DBIDMap.end() )
		return NULL;

	return Iter->second;

}
OnlinePlayerInfoStruct*	AllOnlinePlayerInfoClass::GetInfo_ByGUID( int GUID )
{
	map< int , OnlinePlayerInfoStruct* >::iterator Iter;
	Iter = _GUIDMap.find( GUID );
	if( Iter == _GUIDMap.end() )
		return NULL;
	return Iter->second;
}
OnlinePlayerInfoStruct*	AllOnlinePlayerInfoClass::GetInfo_BySockID( int SockID )
{
	map< int , OnlinePlayerInfoStruct* >::iterator Iter;
	Iter = _SockIDMap.find( SockID );
	if( Iter == _GUIDMap.end() )
		return NULL;
	return Iter->second;
}

map< int	, OnlinePlayerInfoStruct* >* AllOnlinePlayerInfoClass::DBIDMap()
{
	return &_DBIDMap;
}

map< int	, OnlinePlayerInfoStruct* >* AllOnlinePlayerInfoClass::GUIDMap()
{
	return &_GUIDMap;
}

void AllOnlinePlayerInfoClass::Clear()
{
	for( unsigned i = 0 ; i < _UnUsedData.size() ; i++ )
	{
		delete _UnUsedData[i];
	}
	_UnUsedData.clear();

	map< int , OnlinePlayerInfoStruct* >::iterator Iter;

	for( Iter = _DBIDMap.begin() ; Iter != _DBIDMap.end() ; Iter++ )
	{
		delete Iter->second;
	}

	_NameMap.clear();
	_DBIDMap.clear();
	_GUIDMap.clear();
}