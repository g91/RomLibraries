#include "ReaderExClass.h"

using namespace DTF;

ReaderExClass::ReaderExClass( void* data , int size )
{
	_Size = size;
	_InitData = new char[ size ];
	memcpy( _InitData , data , size );
}
ReaderExClass::~ReaderExClass( )
{
	delete _InitData;
}
int		ReaderExClass::Size()
{
	return _Size;
}

void	ReaderExClass::SetReader( const char* name , void* dataPoint , ReaderExClass* reader , int group )
{
	ChildReaderStruct childReader;
	childReader.Address = (int)dataPoint;
	childReader.Reader = reader;
	childReader.Group = group;

	_ChildReader[name] = childReader;

	*(char**)(_InitData + childReader.Address ) = reader->StdData( );
}

//把所有指標清除為0
void	ReaderExClass::ClearAllPoint()
{
	ClearAllPointBase( StdData() );

	map< string , ChildReaderStruct >::iterator iter;
	for( iter = _ChildReader.begin() ; iter != _ChildReader.end() ; iter++ )
	{
		ChildReaderStruct& info = iter->second;
		info.Reader->ClearAllPoint();
		*(char**)( StdData() + info.Address ) = NULL;
		info.Reader->ClearAllPointBase( info.Reader->StdData() );
	}
}

void	ReaderExClass::AllocAllMem( void* obj , bool strNot )
{
	if( strNot == false )
	{
		AllocStringMem( obj );
	}

	map< string , ChildReaderStruct >::iterator iter;
	for( iter = _ChildReader.begin() ; iter != _ChildReader.end() ; iter++ )
	{
		ChildReaderStruct& info = iter->second;
		char** p = (char**)( (char*)obj + info.Address );
		*p = NEW char[ info.Reader->Size() ];
		//memset( *p , 0 , info.Reader->Size() );
		memcpy( *p , info.Reader->StdData() , info.Reader->Size() );
		info.Reader->AllocAllMem( *p , strNot );
	}
}

void	ReaderExClass::CopyAllMem( void* obj , void *obj1  , bool isnew )
{
	//重新產生物件，所以先釋放, 為了效能，確定沒new，可以略過
	if( isnew ) 
		FreeAllMem( obj );

	memcpy( obj , obj1 , Size() );
	CopyStringMem( obj , obj1 );
	map< string , ChildReaderStruct >::iterator iter;
	for( iter = _ChildReader.begin() ; iter != _ChildReader.end() ; iter++ )
	{
		ChildReaderStruct& info = iter->second;
		char** p = (char**)( (char*)obj + info.Address );
		char** p1 = (char**)( (char*)obj1 + info.Address );
		if( *p1 != NULL)
		{
			*p = NEW char[ info.Reader->Size() ];
			memcpy( *p , *p1 , info.Reader->Size() );
			info.Reader->CopyAllMem( *p , *p1 , false );
		}
	}
}
void	ReaderExClass::CopyAllMem( void* obj , void *obj1 , int startsize , int size , bool isnew )
{
	//重新產生物件，所以先釋放, 為了效能，確定沒new，可以略過
	if( isnew ) 
		FreeAllMem( obj );
	memcpy( (char*)obj + startsize , (char*)obj1 + startsize , size );
	CopyStringMem( obj , obj1 );
	map< string , ChildReaderStruct >::iterator iter;
	for( iter = _ChildReader.begin() ; iter != _ChildReader.end() ; iter++ )
	{
		ChildReaderStruct& info = iter->second;
		char** p = (char**)( (char*)obj + info.Address );
		char** p1 = (char**)( (char*)obj1 + info.Address );
		if( *p1 != NULL)
		{
			*p = NEW char[ info.Reader->Size() ];
			memcpy( *p , *p1 , info.Reader->Size() );
			info.Reader->CopyAllMem( *p , *p1 , false );
		}
	}
}
void	ReaderExClass::FreeAllMem( void* obj , bool ischild )
{
	FreeStringMem( obj );
	map< string , ChildReaderStruct >::iterator iter;
	for( iter = _ChildReader.begin() ; iter != _ChildReader.end() ; iter++ )
	{
		ChildReaderStruct& info = iter->second;
		char** p = (char**)( (char*)obj + info.Address );
		if( *p != NULL )
		{
			if( ischild )//ischild = false，有些時候child已經free掉了，所以可以忽略
				info.Reader->FreeAllMem( *p , ischild );
			delete *p;
			*(char**)( (char*)obj + info.Address ) = NULL;
		}		
	}

	if( ischild  == false )
	{
		delete obj;
		obj = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////
bool	ReaderExClass::SetString( void* obj ,const char*  valueName , const char* str , int size )
{
	if( ReaderClass::SetString( obj , valueName , str , size ) )
		return true;

	map< string , ChildReaderStruct >::iterator iter;
	for( iter = _ChildReader.begin() ; iter != _ChildReader.end() ; iter++ )
	{
		ChildReaderStruct& info = iter->second;
		if( info.Reader->SetString( info.GetAddress( obj ) , valueName , str , size) )
			return true;
	}

	return false;
}

bool	ReaderExClass::SetString( void* obj ,const char*  valueName , const wchar_t* wstr , int size )
{
	if( ReaderClass::SetString( obj , valueName , wstr) )
		return true;

	map< string , ChildReaderStruct >::iterator iter;
	for( iter = _ChildReader.begin() ; iter != _ChildReader.end() ; iter++ )
	{
		ChildReaderStruct& info = iter->second;
		if( info.Reader->SetString( info.GetAddress( obj ) , valueName , wstr ) )
			return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
bool	ReaderExClass::SetStringEx( const char**  valueMem , const char* str )
{
	if( valueMem == NULL )
		return false;
	if( *valueMem == NULL )
		return false;

	if( ReaderClass::SetStringEx( *valueMem , str) )
		return true;

	return false;
}
bool	ReaderExClass::SetStringEx( const char**  valueMem , const wchar_t* wstr )
{
	if( valueMem == NULL )
		return false;

	if( *valueMem == NULL )
		return false;

	if( ReaderClass::SetStringEx( *valueMem , wstr) )
		return true;

	return false;
}
//////////////////////////////////////////////////////////////////////////
bool	ReaderExClass::GetType( const char*  valueName , PointTypeENUM& type )
{
	if( ReaderClass::GetType( valueName , type ) )
		return true;

	map< string , ChildReaderStruct >::iterator iter;
	for( iter = _ChildReader.begin() ; iter != _ChildReader.end() ; iter++ )
	{
		ChildReaderStruct& info = iter->second;
		if( info.Reader->GetType( valueName , type ) )
			return true;
	}
	type = EM_PointType_Max;
	return false;
};
bool	ReaderExClass::GetSize(  const char* valueName , int& size )
{
	if( ReaderClass::GetSize( valueName , size ) )
		return true;

	map< string , ChildReaderStruct >::iterator iter;

	for( iter = _ChildReader.begin() ; iter != _ChildReader.end() ; iter++ )
	{
		ChildReaderStruct& info = iter->second;
		if( info.Reader->GetSize( valueName , size ) )
			return true;
	}

	return false;
}
void	ReaderExClass::TotalSize( short &size )
{
	size += _Size;
	map< string , ChildReaderStruct >::iterator iter;
	for( iter = _ChildReader.begin() ; iter != _ChildReader.end() ; iter++ )
	{
		ChildReaderStruct& info = iter->second;
		info.Reader->TotalSize( size );
	}
}


double	ReaderExClass::GetValue_Double( void* obj ,const char*  valueName  )
{
	double value = ReaderClass::GetValue_Double( obj , valueName );
	if( value )
		return value;

	map< string , ChildReaderStruct >::iterator iter;
	for( iter = _ChildReader.begin() ; iter != _ChildReader.end() ; iter++ )
	{
		ChildReaderStruct& info = iter->second;
		value = info.Reader->GetValue_Double( info.GetAddress( obj ) , valueName );
		if( value )
			return value;
	}

	return 0;
};