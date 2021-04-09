//////////////////////////////////////////////////////////////////////////
template<class Temp>
ReaderClass<Temp>::ReaderClass()
{ 
	_EncodeBuff = NULL;
	_EncodeBuffSize=0; 
}
//////////////////////////////////////////////////////////////////////////
template<class Temp>
ReaderClass<Temp>::~ReaderClass()
{
	if( _EncodeBuff != NULL )
		delete _EncodeBuff;
	_EncodeBuff = NULL;
	_EncodeBuffSize=0;
}
//////////////////////////////////////////////////////////////////////////
template<class Temp>
PointInfoStruct*	ReaderClass<Temp>::GetColumnInfo( string ValueName )
{
	map<string,PointInfoStruct>::iterator Iter;
	Iter = _ReadInfo.find( ValueName );

	if( Iter == _ReadInfo.end() )
		return NULL;

	return &(Iter->second);
}
//////////////////////////////////////////////////////////////////////////
template<class Temp>
template< class ValueTemp >
ValueTemp&	ReaderClass<Temp>::GetValue( Temp* Obj ,string ValueName )
{
	map<string,PointInfoStruct>::iterator Iter;
	Iter = _ReadInfo.find( ValueName );
	if( Iter == _ReadInfo.end() )
		return (*(ValueTemp*)NULL);

	return *(ValueTemp*)( Iter->second.Point + Obj );
}
//////////////////////////////////////////////////////////////////////////
//資料設定 如果以有資料則回傳失敗
//例 SetData ( 存取名稱 , 指標 , 指標型態(int short ...) , 大小 , )
template<class Temp>
bool	ReaderClass<Temp>::SetData( string ValueName , const void* Point , PointTypeENUM Type , int Size , int UserData )
{
	std::transform( ValueName.begin(), ValueName.end(), ValueName.begin(), ::tolower );

	PointInfoStruct NewData;

	NewData.Point		= (int)Point;
	NewData.Type		= Type;
	NewData.UserData    = UserData;
	NewData.Size        = Size;
	NewData.ValueName	= ValueName;
	NewData.ID			= (int)_ReadInfo.size();

	_ReadInfo[ ValueName ] = NewData;
	_ArgList.push_back( ValueName );
	_ListByType[ Type ].push_back( NewData );

	return true;
}
//////////////////////////////////////////////////////////////////////////
template<class Temp>
bool	ReaderClass<Temp>::DataTransfer_Decode	( void* _Obj , char* InBox )
{
	char* Obj = (char*)_Obj;

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
			continue;

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
				memcpy( Value , DataInfo->GetData() , DataInfo->DataSize );					
			}
			break;
		case EM_PointType_WCharString:		//WChar格式
			{
				wchar_t* Value = (wchar_t*)( Obj + IDData->Point );
				memcpy( Value , DataInfo->GetData() , DataInfo->DataSize );					
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
				MyMap<int>& Map = *(MyMap<int>*)( Obj + IDData->Point );
				Map.clear();
				for( int i = 0 ; i < DataInfo->DataCount ; i++ )
				{
					MapDataStruct* Data = (MapDataStruct*)(DataInfo->GetData(i));
					Map.insert( Data->ID , Data->Data , DataInfo->DataSize );
				}
			}
			break;				
		}

		//	InBox += DataInfo->TotalSize;
	}

	return (ProcDataSize==InSize);
}
//////////////////////////////////////////////////////////////////////////
template<class Temp>
int	ReaderClass<Temp>::_SizeofData( void* _Obj , PointInfoStruct& IDData )
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
			return myVec.size() * myVec.block_size();
		}
	case EM_PointType_MyMap:
		{
			MyMap<int>& myMap =  *( (MyMap<int>*)( IDData.Point + Obj ) );
			return myMap.size() * ( myMap.block_size() + sizeof(int) );
		}
	}
	return 0;
}


template<class Temp>
int	ReaderClass<Temp>::DataTransfer_Encode	( void* _Obj , char* &OutBox )
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
template<class Temp>
void	ReaderClass<Temp>::ClearEncodeBuff()
{
	if( _EncodeBuff != NULL )
		delete _EncodeBuff;

	_EncodeBuffSize = 0;
	_EncodeBuff = NULL;
}
//////////////////////////////////////////////////////////////////////////
