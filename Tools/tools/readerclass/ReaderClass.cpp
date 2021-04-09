#include "ReaderClass.h"
#include <algorithm>
#include <BaseTsd.h>
#include "malloc/MyAlloc.h"
#include <Windows.h>

#define _MAX_CHAR_SIZE 2048
//////////////////////////////////////////////////////////////////////////
using namespace DTF;
using namespace std;
//////////////////////////////////////////////////////////////////////////
ReaderClass::ReaderClass()
{ 
	_EncodeBuff = NULL;
	_EncodeBuffSize=0; 
}
//////////////////////////////////////////////////////////////////////////

ReaderClass::~ReaderClass()
{
	if( _EncodeBuff != NULL )
		delete _EncodeBuff;
	_EncodeBuff = NULL;
	_EncodeBuffSize=0;
}
//////////////////////////////////////////////////////////////////////////

PointInfoStruct*	ReaderClass::GetColumnInfo( const char* ValueName )
{
	map<string,PointInfoStruct>::iterator Iter;
	Iter = _ReadInfo.find( ValueName );

	if( Iter == _ReadInfo.end() )
		return NULL;

	return &(Iter->second);
}
//////////////////////////////////////////////////////////////////////////
//資料設定 如果以有資料則回傳失敗
//例 SetData ( 存取名稱 , 指標 , 指標型態(int short ...) , 大小 , )
bool	ReaderClass::SetData( const char* ValueName , const void* Point , PointTypeENUM Type , int Size , int UserData )
{
	//	std::transform( ValueName.begin(), ValueName.end(), ValueName.begin(), ::tolower );

	PointInfoStruct NewData;

	NewData.Point		= (int)Point;
	NewData.Type		= Type;
	NewData.UserData    = UserData;
	NewData.Size        = Size;
	switch( Type )
	{
	case EM_PointType_Float:
		NewData.Size = sizeof( float );
		break;
	case EM_PointType_Int:
		NewData.Size = sizeof( int );
		break;
	case EM_PointType_SmallDateTime:
		NewData.Size = sizeof( float );
		break;
	case EM_PointType_Int64:
		NewData.Size = sizeof( INT64 );
		break;
	case EM_PointType_Short:
		NewData.Size = sizeof( short );
		break;
	case EM_PointType_Char:
		NewData.Size = sizeof( char );
		break;
	case EM_PointType_Bit:
		NewData.Size = sizeof( bool );
		break;
	}

	NewData.ValueName	= ValueName;
	NewData.ID			= (int)_ReadInfo.size();

	_ReadInfo[ ValueName ] = NewData;
	_ArgList.push_back( ValueName );
	_ListByType[ Type ].push_back( NewData );

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool	ReaderClass::DataTransfer_Decode	( void* _Obj , char* InBox  )
{
	char* Obj = (char*)_Obj;
	bool  Result = true;	

	int InSize = *((int*)InBox);
	//	InBox += sizeof( int );
	int	ProcDataSize = sizeof(int);

	while( ProcDataSize < InSize  )
	{
		DataInfoStruct* DataInfo = (DataInfoStruct*)( InBox + ProcDataSize );

		ProcDataSize += DataInfo->TotalSize;
		if( ProcDataSize > InSize )
			return false;

		PointInfoStruct* IDData = GetColumnInfo( DataInfo->ValueName() );
		if( IDData == NULL )
		{
			Result = false;
			continue;
		}

		switch( IDData->Type )
		{
		case EM_PointType_Float:
			{
				float& Value = *(float*)( Obj + IDData->Point );
				Value = *(float*)(DataInfo->GetData());
			}
			break;
		case EM_PointType_Int:
			{
				int& Value = *(int*)( Obj + IDData->Point );
				Value = *(int*)(DataInfo->GetData());
			}
			break;
		case EM_PointType_Int64:
			{
				INT64& Value = *(INT64*)( Obj + IDData->Point );
				Value = *(INT64*)(DataInfo->GetData());
			}
			break;

		case EM_PointType_Short:
			{
				short& Value = *(short*)( Obj + IDData->Point );
				Value = *(short*)(DataInfo->GetData());
			}
			break;
		case EM_PointType_Char:
			{
				char& Value = *(char*)( Obj + IDData->Point );
				Value = *(char*)(DataInfo->GetData());
			}
			break;
		case EM_PointType_Bit:
			{
				bool& Value = *(bool*)( Obj + IDData->Point );
				Value = *(bool*)(DataInfo->GetData());
			}
			break;
		case EM_PointType_SmallDateTime:
			{
				float& Value = *(float*)( Obj + IDData->Point );
				Value = *(float*)(DataInfo->GetData());
			}
			break;
		case EM_PointType_CharString:
		case EM_PointType_BinaryData:       //主要給資料庫用的要設定Size
		case EM_PointType_TextData:
			{
				char* Value = (char*)( Obj + IDData->Point );
				int dataSize = __min( IDData->Size , DataInfo->DataSize );
				memcpy( Value , DataInfo->GetData() , dataSize );	
				if( IDData->Size != DataInfo->DataSize )
				{
					Result = false;
				}
			}
			break;
		case EM_PointType_WCharString:		//WChar格式
			{
				wchar_t* Value = (wchar_t*)( Obj + IDData->Point );
				int dataSize = __min( IDData->Size , DataInfo->DataSize );
				memcpy( Value , DataInfo->GetData() , dataSize );					
			}
			break;
		case EM_PointType_StdString:		//std String 指標
			{
				string& Value = *(string*)( Obj + IDData->Point );
				Value.clear();
				Value = DataInfo->GetData();
			}
			break;
		case EM_PointType_StdWString:		//std String 指標
			{
				wstring& Value = *(wstring*)( Obj + IDData->Point );
				Value.clear();
				Value = (wchar_t*)(DataInfo->GetData());
			}
			break;
		case EM_PointType_MyVector:		
			{
				MyVector<int>& Vec = *(MyVector<int>*)( Obj + IDData->Point );
				Vec.clear();
				for( int i = 0 ; i < DataInfo->DataCount ; i++ )
				{
					Vec.push_back( DataInfo->GetData(i) , DataInfo->DataSize );
				}
			}
			break;
		case EM_PointType_MyMap:
			{
				MyMap< int >& Map = *(MyMap<int>*)( Obj + IDData->Point );
				Map.clear();
				for( int i = 0 ; i < DataInfo->DataCount ; i++ )
				{
					MapDataStruct* Data = (MapDataStruct*)(DataInfo->GetData(i));
					Map.insert( Data->ID , Data->Data , DataInfo->DataSize );
				}
			}
			break;				
		}

	/*	if( orgCheckValue != *checkValue )
		{
			orgCheckValue = *checkValue;
			//( LV3 , "DataTransfer_Decode" , "ValueName=%s" , DataInfo->ValueName() );
			//TRACE1( "DataTransfer_Decode ValueName=%s" , DataInfo->ValueName() );
			{ char  szTemp[256];sprintf( szTemp , "DataTransfer_Decode ValueName=%s" , DataInfo->ValueName() );		OutputDebugStringA( szTemp ); }
		}
		*/
		//	InBox += DataInfo->TotalSize;
	}

	return (ProcDataSize==InSize) && Result;
}
//////////////////////////////////////////////////////////////////////////
int	ReaderClass::_SizeofData( void* _Obj , PointInfoStruct& IDData )
{
	char* Obj = (char*)_Obj;
	switch( IDData.Type )
	{
	case EM_PointType_Float:
		return sizeof(float);
	case EM_PointType_Int:
		return sizeof(int);
	case EM_PointType_Int64:
		return sizeof(INT64);
	case EM_PointType_Short:
		return sizeof(short);
	case EM_PointType_Char:
		return sizeof(char);
	case EM_PointType_Bit:
		return sizeof(bool);
	case EM_PointType_SmallDateTime:
		return sizeof(float);
	case EM_PointType_CharString:
	case EM_PointType_WCharString:
	case EM_PointType_BinaryData:       //主要給資料庫用的要設定Size
	case EM_PointType_TextData:
		return IDData.Size;

	case EM_PointType_StdString:		//std String 指標
		{
			string* tmpStr =  ( (string*)( IDData.Point + Obj ) );
			return tmpStr->length() + 1;
		}
	case EM_PointType_StdWString:		//std String 指標
		{
			wstring& tmpStr =  *( (wstring*)( IDData.Point + Obj ) );
			return ( tmpStr.length() + 1 ) * 2;			
		}
	case EM_PointType_MyVector:
		{
			MyVector<int>& myVec = *( (MyVector<int>*)( IDData.Point + Obj ) );
			//return myVec.size() * myVec.block_size();
			return myVec.size() * ( myVec.block_size() + sizeof(int) );
		}
	case EM_PointType_MyMap:
		{
			MyMap<int>& myMap =  *( (MyMap<int>*)( IDData.Point + Obj ) );
			return myMap.size() * ( myMap.block_size() + sizeof(int) );
		}
	}
	return 0;
}


int	ReaderClass::DataTransfer_Encode	( void* _Obj , char* &OutBox )
{
	char* Obj = (char*)_Obj;
	int DataSize = sizeof(int);
	//sizeof(int);
	//計算資料大小
	map<string,PointInfoStruct>::iterator Iter;
	for( Iter = _ReadInfo.begin() ; Iter != _ReadInfo.end() ; Iter++ )
	{
		PointInfoStruct& IDData = Iter->second;
		DataSize +=  sizeof( DataInfoStruct ) + IDData.ValueName.length() + 1 + _SizeofData( Obj , IDData );
	}

	if( _EncodeBuff == NULL )
	{
		_EncodeBuff = new char[ DataSize ];
		_EncodeBuffSize = DataSize;
	}
	else if( _EncodeBuffSize < DataSize )
	{
		delete _EncodeBuff;
		_EncodeBuff = new char[ DataSize ];
		_EncodeBuffSize = DataSize;
	}

	DataInfoStruct* DataInfo = (DataInfoStruct*)(_EncodeBuff + sizeof(int));
	for( Iter = _ReadInfo.begin() ; Iter != _ReadInfo.end() ; Iter++ )
	{
		PointInfoStruct& IDData = Iter->second;
		strcpy( DataInfo->Data , IDData.ValueName.c_str() );
		DataInfo->DataCount = 1;
		DataInfo->ValueNameSize = strlen( DataInfo->Data ) + 1;
		DataInfo->DataSize = IDData.Size;
		//DataInfo->TotalSize = DataInfo->ValueNameSize + IDData.Size;
		DataInfo->TotalSize = DataInfo->ValueNameSize + _SizeofData( Obj , IDData ) + sizeof( DataInfoStruct );

		switch( IDData.Type )
		{
		case EM_PointType_Float:
		case EM_PointType_Int:
		case EM_PointType_Int64:
		case EM_PointType_Short:
		case EM_PointType_Char:
		case EM_PointType_Bit:
		case EM_PointType_SmallDateTime:
		case EM_PointType_CharString:
		case EM_PointType_WCharString:
		case EM_PointType_BinaryData:       //主要給資料庫用的要設定Size
		case EM_PointType_TextData:
			memcpy( DataInfo->Data + DataInfo->ValueNameSize , (char*)( IDData.Point + Obj ) , _SizeofData(Obj , IDData ) );
			break;

		case EM_PointType_StdString:		//std String 指標
			{
				string& tmpStr =  *( (string*)( IDData.Point + Obj ) );
				strcpy( DataInfo->Data + DataInfo->ValueNameSize , tmpStr.c_str() );
				DataInfo->DataSize = tmpStr.length() + 1;
				//				DataInfo->TotalSize = DataInfo->ValueNameSize + tmpStr.length() + 1;				
			}
			break;
		case EM_PointType_StdWString:		//std String 指標
			{
				wstring& tmpStr =  *( (wstring*)( IDData.Point + Obj ) );


				wcscpy( (wchar_t*)(DataInfo->Data + DataInfo->ValueNameSize) , tmpStr.c_str() );
				DataInfo->DataSize = ( tmpStr.length() + 1 ) * 2;
				//				DataInfo->TotalSize = DataInfo->ValueNameSize + DataInfo->DataSize;
			}
			break;
		case EM_PointType_MyVector:
			{
				MyVector<int>& tmpMyVec = *( (MyVector<int>*)( IDData.Point + Obj ) );

				DataInfo->DataCount = (int)tmpMyVec.size();
				DataInfo->DataSize = (int)tmpMyVec.block_size();
				//				DataInfo->TotalSize = DataInfo->DataCount * DataInfo->DataSize + DataInfo->ValueNameSize;

				for( unsigned i = 0 ; i < tmpMyVec.size() ; i++ )
				{
					memcpy( DataInfo->GetData( i ) , &tmpMyVec[i] , tmpMyVec.block_size() );
				}
			}
			break;
		case EM_PointType_MyMap:
			{
				MyMap<int>& tmpMyMap = *( (MyMap<int>*)( IDData.Point + Obj ) );

				DataInfo->DataCount = (int)tmpMyMap.size();
				DataInfo->DataSize = (int)tmpMyMap.block_size();
				//				DataInfo->TotalSize = DataInfo->ValueNameSize + _SizeofData( Obj , IDData );

				MyMap<int>::iterator Iter;
				int i = 0;
				for( Iter = tmpMyMap.begin() ; Iter != tmpMyMap.end() ; Iter++ , i++ )
				{
					memcpy( DataInfo->GetData( i ) , (char*)(&Iter->first) , sizeof(int) );
					memcpy( DataInfo->GetData( i )+sizeof(int) , (char*)Iter->second , tmpMyMap.block_size() );
				}
			}
			break;
		}
		DataInfo = ( DataInfoStruct*)( (char*)DataInfo + DataInfo->TotalSize );

	}
	OutBox = _EncodeBuff;
	*(int*)OutBox = DataSize;

	return DataSize;
}
//////////////////////////////////////////////////////////////////////////
void	ReaderClass::ClearEncodeBuff()
{
	if( _EncodeBuff != NULL )
		delete _EncodeBuff;

	_EncodeBuffSize = 0;
	_EncodeBuff = NULL;
}
//////////////////////////////////////////////////////////////////////////
void	ReaderClass::AllocStringMem( void* obj )
{
	vector< PointInfoStruct >& wcharInfoList = _ListByType[ EM_PointType_WCharStringPoint ];
	for( unsigned i = 0 ; i < wcharInfoList.size() ; i++ )
	{
		PointInfoStruct& info = wcharInfoList[i];
		wchar_t**p = (wchar_t**)((char*)obj + info.Point);
		*p = NEW wchar_t[_MAX_CHAR_SIZE];
		(*p)[0] = 0;
	}

	vector< PointInfoStruct >& charInfoList = _ListByType[ EM_PointType_CharStringPoint ];
	for( unsigned i = 0 ; i < charInfoList.size() ; i++ )
	{
		PointInfoStruct& info = charInfoList[i];
		char**p = (char**)((char*)obj + info.Point);
		*p = NEW char[_MAX_CHAR_SIZE];
		(*p)[0] = 0;
	}

	vector< PointInfoStruct >& bicharInfoList = _ListByType[ EM_PointType_BinaryCharPoint ];
	for( unsigned i = 0 ; i < bicharInfoList.size() ; i++ )
	{
		PointInfoStruct& info = bicharInfoList[i];
		char**p = (char**)((char*)obj + info.Point);
		*p = NEW char[_MAX_CHAR_SIZE];
		(*p)[0] = 0;
	}
}
void	ReaderClass::CopyStringMem( void* obj , void *obj1 )
{
	vector< PointInfoStruct >& wcharInfoList = _ListByType[ EM_PointType_WCharStringPoint ];
	for( unsigned i = 0 ; i < wcharInfoList.size() ; i++ )
	{
		PointInfoStruct& info = wcharInfoList[i];
		wchar_t**p = (wchar_t**)((char*)obj + info.Point);
		wchar_t**p1 = (wchar_t**)((char*)obj1 + info.Point);
		
		if( *p == *p1 || *p == NULL )
			*p = NEW wchar_t[_MAX_CHAR_SIZE];

		if( *p1 != NULL)	
			memcpy( *p , *p1 , ( wcslen( *p1 ) + 1 ) * 2 ); 
	}

	vector< PointInfoStruct >& charInfoList = _ListByType[ EM_PointType_CharStringPoint ];
	for( unsigned i = 0 ; i < charInfoList.size() ; i++ )
	{
		PointInfoStruct& info = charInfoList[i];
		char**p = (char**)((char*)obj + info.Point);
		char**p1 = (char**)((char*)obj1 + info.Point);
		
		if( *p == *p1 || *p == NULL )
			*p = NEW char[_MAX_CHAR_SIZE];

		if( *p1 != NULL)
			memcpy( *p , *p1 , strlen( *p1 ) + 1 ); 
	}

	vector< PointInfoStruct >& bicharInfoList = _ListByType[ EM_PointType_BinaryCharPoint ];
	for( unsigned i = 0 ; i < bicharInfoList.size() ; i++ )
	{
		PointInfoStruct& info = bicharInfoList[i];
		char**p = (char**)((char*)obj + info.Point);
		char**p1 = (char**)((char*)obj1 + info.Point);

		if( *p == *p1 || *p == NULL )
			*p = NEW char[_MAX_CHAR_SIZE];
		
		if( *p1 != NULL)
			memcpy( *p , *p1 , _MAX_CHAR_SIZE ); 
	}
}
void	ReaderClass::ClearAllPointBase( void* obj )
{
	vector< PointInfoStruct >& wcharInfoList = _ListByType[ EM_PointType_WCharStringPoint ];
	for( unsigned i = 0 ; i < wcharInfoList.size() ; i++ )
	{
		PointInfoStruct& info = wcharInfoList[i];
		*(char**)((char*)obj + info.Point) = NULL;
	}

	vector< PointInfoStruct >& charInfoList = _ListByType[ EM_PointType_CharStringPoint ];
	for( unsigned i = 0 ; i < charInfoList.size() ; i++ )
	{
		PointInfoStruct& info = charInfoList[i];
		*(char**)((char*)obj + info.Point) = NULL;
	}

	vector< PointInfoStruct >& bicharInfoList = _ListByType[ EM_PointType_BinaryCharPoint ];
	for( unsigned i = 0 ; i < bicharInfoList.size() ; i++ )
	{
		PointInfoStruct& info = bicharInfoList[i];
		*(char**)((char*)obj + info.Point) = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////
void	ReaderClass::FreeStringMem( void* obj  , bool isAllfree , const char* name )
{
	vector< PointInfoStruct >& wcharInfoList = _ListByType[ EM_PointType_WCharStringPoint ];
	for( unsigned i = 0 ; i < wcharInfoList.size() ; i++ )
	{
		PointInfoStruct& info = wcharInfoList[i];
		if( !isAllfree && info.ValueName != name )
			continue;

		wchar_t**p = (wchar_t**)((char*)obj + info.Point);
		if( *p != NULL)
		{
			delete [] *p;
			*(wchar_t**)((char*)obj + info.Point) = NULL;
		}

	}

	vector< PointInfoStruct >& charInfoList = _ListByType[ EM_PointType_CharStringPoint ];
	for( unsigned i = 0 ; i < charInfoList.size() ; i++ )
	{
		PointInfoStruct& info = charInfoList[i];
		if( !isAllfree && info.ValueName != name ) continue;

		char**p = (char**)((char*)obj + info.Point);
		if( *p != NULL )
		{
			delete [] *p;
			*(char**)((char*)obj + info.Point) = NULL;
		}
	}

	vector< PointInfoStruct >& bicharInfoList = _ListByType[ EM_PointType_BinaryCharPoint ];
	for( unsigned i = 0 ; i < bicharInfoList.size() ; i++ )
	{
		PointInfoStruct& info = bicharInfoList[i];
		if( !isAllfree && info.ValueName != name ) continue;

		char**p = (char**)((char*)obj + info.Point);
		if( *p != NULL )
		{
			delete [] *p;
			*(char**)((char*)obj + info.Point) = NULL;
		}
	}

}

bool	ReaderClass::SetString( void* obj ,const char* valueName , const char* str , int size )
{
	map<string,PointInfoStruct>::iterator Iter;
	Iter = _ReadInfo.find( valueName );
	if( Iter == _ReadInfo.end() )
		return false;
	if( size == -1 )
	{
		assert ( strlen( str ) <= _MAX_CHAR_SIZE);
		strcpy( *(char**)( Iter->second.Point + (char*)obj ) , str );
	}
	else
	{
		assert ( size <= _MAX_CHAR_SIZE);
		memcpy( *(char**)( Iter->second.Point + (char*)obj ) , str , size );
	}
	
	return true;
}

bool	ReaderClass::SetString( void* obj ,const char* valueName , const wchar_t* wstr )
{
	map<string,PointInfoStruct>::iterator Iter;
	Iter = _ReadInfo.find( valueName );
	if( Iter == _ReadInfo.end() )
		return false;
	assert ( wcslen( wstr ) <= _MAX_CHAR_SIZE * 2 );
	wcscpy( *(wchar_t**)( Iter->second.Point + (char*)obj ) , wstr );
	return true;
}

bool	ReaderClass::SetStringEx( const char* valueMem , const char* str )
{
	if( valueMem == NULL )
		return false;

	assert ( strlen( str ) <= _MAX_CHAR_SIZE );
	strcpy( (char*)valueMem , str );
	return true;
}

bool	ReaderClass::SetStringEx( const char* valueMem , const wchar_t* wstr )
{
	if( valueMem == NULL )
		return false;

	assert ( wcslen( wstr ) <= _MAX_CHAR_SIZE * 2 );
	wcscpy( (wchar_t*)valueMem , wstr );
	return true;
}

bool	ReaderClass::GetType( const char* ValueName , PointTypeENUM& type )
{
	map<string,PointInfoStruct>::iterator Iter;
	Iter = _ReadInfo.find( ValueName );
	type = EM_PointType_Max;
	if( Iter == _ReadInfo.end() )
		return false;

	type = Iter->second.Type;
	return true;
}
bool	ReaderClass::GetSize( const char* ValueName , int& size )
{
	map<string,PointInfoStruct>::iterator Iter;
	Iter = _ReadInfo.find( ValueName );
	if( Iter == _ReadInfo.end() )
		return false;

	size = Iter->second.Size;
	return true;
}
double ReaderClass::GetValue_Double( void* Obj , const char* ValueName )
{
	double ret = 0;
	map<string,PointInfoStruct>::iterator Iter;
	Iter = _ReadInfo.find( ValueName );
	if( Iter == _ReadInfo.end() )
		return 0;

	char* point = ( Iter->second.Point + (char*)Obj );
	switch( Iter->second.Type )
	{
	case EM_PointType_Float:
		ret = *(float*)point;
		break;
	case EM_PointType_Int:
		ret = *(int*)point;
		break;
	case EM_PointType_Int64:
		ret = *(int*)point;
		break;

	case EM_PointType_Short:
		ret = *(short*)point;
		break;
	case EM_PointType_Char:
		ret = *(char*)point;
		break;
	case EM_PointType_Bit:
		ret = *(bool*)point;
		break;
	default:			
		break;
	}
	//ret = *(ValueTemp*)( Iter->second.Point + (char*)Obj );

	return ret;
}

//////////////////////////////////////////////////////////////////////////
//binary轉換成Char
//////////////////////////////////////////////////////////////////////////
bool CBinaryChar::BinaryToChar( short length , char* content , vector<char>& result )
{
	if( length < 0 || length > BINARY_MAX_SIZE || content == NULL )
		return false;

	int				code;
	int				pos = 0;

	result.clear();
	result.resize( sizeof( length ) + sizeof( code ) + length );
	code = BINARY_CHAR_CODE;
	memcpy( &result[pos] , &code , sizeof( code ) );
	pos += sizeof( code );
	memcpy( &result[pos] , &length , sizeof( length ) );
	pos += sizeof( length );

	if( length > 0 )
		memcpy( &result[pos] , content , length );

	return true;
}
//將資料庫的資料，轉出成不定長binary檔
bool CBinaryChar::CharToBinary( char* content , vector<char>& result )
{
	if( CheckIsBinaryChar(content) == false )
		return false;

	short length;

	memcpy( &length , content + sizeof( int ) , sizeof( length ) );
	result.clear();
	result.resize( length );
	if( length > 0) 
		memcpy( &result[0] , content + sizeof( int ) + sizeof( short ) , length );

	return true;
}

//檢查是一般資料才是binary
bool CBinaryChar::CheckIsBinaryChar( char* content )
{
	if( content == NULL )
		return false;
	//size小於code就是一般char
	if( strlen( content ) < sizeof( int ) )
		return false;

	int code;
	int pos = 0;

	memcpy( &code , content + pos , sizeof( code ) );
	pos += sizeof( code );

	if( code != BINARY_CHAR_CODE )
		return false;

	short length;

	memcpy( &length , content + pos , sizeof( length ) );
	pos += sizeof( length );

	if( length < 0 )
		return false;

	return true;
}
//取得轉成資料庫資料時的長度
short CBinaryChar::GetBinaryCharLength( char* content )
{
	if( CheckIsBinaryChar( content ) == false )
		return 0;

	short length;

	memcpy( &length , content + sizeof( int ) , sizeof( length ) );

	return sizeof(int) + sizeof( length ) + length;

}