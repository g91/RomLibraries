#pragma once
#include "roledata/RoleBaseDef.h"
//////////////////////////////////////////////////////////////////////////
//所有線上玩家 基本資料
//////////////////////////////////////////////////////////////////////////

struct StringICmp : public binary_function< const string, const string, bool>
{	
	bool operator()(const string& _Left, const string& _Right) const
	{	// apply operator> to operands
		return stricmp(_Left.c_str() , _Right.c_str() ) > 0 ;
	}
};

class AllOnlinePlayerInfoClass
{
protected:
	static AllOnlinePlayerInfoClass *_This;
public:

	static bool Init( );
	static bool Release( );
	static AllOnlinePlayerInfoClass* This(){ return _This; };

protected:
	map< string , OnlinePlayerInfoStruct* , StringICmp >	_NameMap;			
	map< string , OnlinePlayerInfoStruct* , StringICmp >	_AccountMap;
	map< int	, OnlinePlayerInfoStruct* >	_DBIDMap;
	map< int	, OnlinePlayerInfoStruct* >	_GUIDMap;
	map< int	, OnlinePlayerInfoStruct* >	_SockIDMap;

	vector< OnlinePlayerInfoStruct* >	_UnUsedData;

public:
	AllOnlinePlayerInfoClass( );
	~AllOnlinePlayerInfoClass( );

	//重新設定某一個角的的全域資料
	bool	AddorSetPlayer( OnlinePlayerInfoStruct& Info );	
	//刪除某個角色的資料
	bool	DelPlayer( int DBID );


	OnlinePlayerInfoStruct*	GetInfo_ByName		( char* Name );
	OnlinePlayerInfoStruct*	GetInfo_ByAccount	( char* Account );
	OnlinePlayerInfoStruct*	GetInfo_ByDBID		( int DBID );
	OnlinePlayerInfoStruct*	GetInfo_ByGUID		( int GUID );
	OnlinePlayerInfoStruct*	GetInfo_BySockID	( int SockID );

	map< int	, OnlinePlayerInfoStruct* >* DBIDMap();
	map< int	, OnlinePlayerInfoStruct* >* GUIDMap();

	void Clear();

};