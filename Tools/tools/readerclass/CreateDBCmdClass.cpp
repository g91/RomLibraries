#pragma once
#include "CreateDBCmdClass.h"

//-----------------------------------------------------------------------------------------------------------------
CreateDBCmdClass::CreateDBCmdClass( ReaderClass* Reader , char* TableName , bool IsNoString )
{ 
	_Reader = Reader; 
	_NoString = IsNoString;
	_SQLTableName = TableName;

	string  Str;
	bool    IsBegin;

	_Reader     = Reader;   

	//----------------SQL 字串產生 ---------------------------
	map<string,PointInfoStruct>* ReadInfo = Reader->ReadInfo();
	map<string,PointInfoStruct>::iterator Iter;

	_SQLSelectCmd = "";
	_SQLInsertCmd = "Insert ";
	_SQLInsertCmd = _SQLInsertCmd + TableName + "( ";

	IsBegin = true ;
	Str     = "";
	for( Iter = ReadInfo->begin() ; Iter != ReadInfo->end() ; Iter++ )
	{
		PointInfoStruct* Info = &(Iter->second);
		if( _NoString )
		{

			switch( Info->Type )
			{
			case EM_PointType_CharStringPoint:
			case EM_PointType_WCharStringPoint:
			case EM_PointType_TextDataPoint:
			case EM_PointType_BinaryCharPoint:
				continue;
			}
		}
		if( Info->UserData & Def_Disabled_Select_Flag )
			continue;

		if( IsBegin == false )
		{
			Str = Str + ",";            
		}
		IsBegin = false ;

		if( EM_PointType_SmallDateTime == Info->Type )
		{
			Str = Str + "CAST(";
			Str = Str + Iter->first;
			Str = Str + " AS float )";
		}
		else
			Str = Str + Iter->first;
	}

	_SQLSelectCmd = _SQLSelectCmd + Str + " From " + TableName + " ";
	//-----------------------------------------------------------------------------------------------------------------
	_SQLUpDateCmd = "UPDATE "; 
	_SQLUpDateCmd = _SQLUpDateCmd + TableName + " SET ";

	//-----------------------------------------------------------------------------------------------------------------
	IsBegin = true ;
	Str     = "";
	for( Iter = ReadInfo->begin() ; Iter != ReadInfo->end() ; Iter++ )
	{
		PointInfoStruct* Info = &(Iter->second);
		if( Info->UserData & Def_Disabled_Insert_Flag )
			continue;

		if( _NoString )
		{
			switch( Info->Type )
			{
			case EM_PointType_CharStringPoint:
			case EM_PointType_WCharStringPoint:
			case EM_PointType_TextDataPoint:
			case EM_PointType_BinaryCharPoint:
				continue;
			}
		}

		if( IsBegin == false )
		{
			Str = Str + ",";            
		}
		IsBegin = false ;

		Str = Str + Iter->first;

	}
	_SQLInsertCmd = _SQLInsertCmd + Str + ") values( ";


}
//--------------------------------------------------------------------------------------------------------
string CreateDBCmdClass::GetInsertStr( void* Data )
{
	char    Buf[0x20000];
	//	_CritSect.Enter();
	map<string,PointInfoStruct>*             ReadInfo ;
	map<string,PointInfoStruct>::iterator    Iter;
	string SQLCmd = _SQLInsertCmd;
	ReadInfo = _Reader->ReadInfo();
	bool        BeginFlag = true;

	for(    Iter  = ReadInfo->begin() ; 
		Iter != ReadInfo->end() ; 
		Iter++  )
	{
		PointInfoStruct *IDData = &(Iter->second);

		if( IDData->UserData & Def_Disabled_Insert_Flag )
			continue;

		if( _NoString )
		{
			switch( IDData->Type )
			{
			case EM_PointType_CharStringPoint:
			case EM_PointType_WCharStringPoint:
			case EM_PointType_TextDataPoint:
			case EM_PointType_BinaryCharPoint:
				continue;
			}
		}


		switch(IDData->Type)
		{
		case EM_PointType_Float:
			{               
				float Value = *(float*)(IDData->Point + int(Data));
				if( CheckFloatAvlid( Value ) == false )
					Value = 0;
				sprintf( Buf ,"%e" , Value  );
			}break;

		case EM_PointType_Int:
			{
				int Value = *((int*)(IDData->Point + int(Data)));
				sprintf( Buf ,"%d" , Value  );
			}break;
		case EM_PointType_Int64:
			{
				INT64 Value = *((INT64*)(IDData->Point + int(Data)));
				sprintf( Buf ,"%I64d" , Value  );
			}break;
		case EM_PointType_Short:
			{                    
				short Value = *((short*)(IDData->Point + int(Data)));
				sprintf( Buf ,"%d" , Value  );
			}                
			break;
		case EM_PointType_BinaryData:
			{
				char* Str = ((char*)(IDData->Point + int(Data)));
				sprintf( Buf , " CAST( %s AS binary(%d) ) ", StringToSqlX( Str, IDData->Size , false ).c_str() , IDData->Size );
			}
			break;
		case EM_PointType_CharString:
		case EM_PointType_TextData:
			{
				char* Str = ((char*)(IDData->Point + int(Data)));
				sprintf( Buf , " CAST( %s AS varchar(8000) ) ", StringToSqlX( Str , IDData->Size ).c_str() );
			}
			break;	
		case EM_PointType_WCharString:
			{					
				wchar_t* Str = ((wchar_t*)(IDData->Point + int(Data)));
				sprintf( Buf , " CAST( %s AS nvarchar(4000) ) ", WStringToSqlX( Str , IDData->Size ).c_str() );
			}
			break;

		case EM_PointType_CharStringPoint:
		case EM_PointType_TextDataPoint:
		case EM_PointType_BinaryCharPoint:
			{
				char* Str = *((char**)(IDData->Point + int(Data)));
				sprintf( Buf , " CAST( %s As varchar(8000) ) ", StringToSqlX( Str , IDData->Size ).c_str() );
			}
			break;
		case EM_PointType_Bit:
			{
				bool Value = *((bool*)(IDData->Point + int(Data)));
				sprintf( Buf ,"%d" , Value  );

			}
			break;
		case EM_PointType_SmallDateTime:
			{
				float Value = *((float*)(IDData->Point + int(Data)));
				sprintf( Buf ,"%f" , Value  );
			}break;   
		default:
			continue;
		}

		if( BeginFlag == false )
			SQLCmd = SQLCmd + ",";
		else 
			BeginFlag = false;

		SQLCmd = SQLCmd + Buf;            

	}
	SQLCmd = SQLCmd + ")";
	//	_SQLCmd = SQLCmd;
	//	_CritSect.Leave();
	return SQLCmd;
}
//--------------------------------------------------------------------------------------------------------
string CreateDBCmdClass::GetUpDateStr( void* Data , const char* WhereCmd )
{
	//	_CritSect.Enter();
	char    Buf[0x20000];
	bool    Beginflag = true;

	map<string,PointInfoStruct>*             ReadInfo ;
	map<string,PointInfoStruct>::iterator    Iter;
	ReadInfo = _Reader->ReadInfo();
	string SQLCmd = _SQLUpDateCmd;

	for(    Iter  = ReadInfo->begin() ; 
		Iter != ReadInfo->end() ; 
		Iter++  )
	{
		PointInfoStruct *IDData = &(Iter->second);

		string TempStr = Iter->first;

		if( IDData->UserData & Def_Disabled_Update_Flag )
			continue;

		if( _NoString )
		{
			switch( IDData->Type )
			{
			case EM_PointType_CharStringPoint:
			case EM_PointType_WCharStringPoint:
			case EM_PointType_TextDataPoint:
			case EM_PointType_BinaryCharPoint:
				continue;
			}
		}


		switch(IDData->Type)
		{
		case EM_PointType_Float:
			{               
				float Value = *(float*)(IDData->Point + int(Data));
				bool Ret = CheckFloatAvlid( Value );
				if( CheckFloatAvlid( Value ) == false )
					Value = 0;
				sprintf( Buf ," %s = %e " , Iter->first.c_str() , Value  );
			}break;

		case EM_PointType_Int:
			{
				int Value = *((int*)(IDData->Point + int(Data)));
				sprintf( Buf ," %s = %d " , Iter->first.c_str() , Value  );
			}break;             
		case EM_PointType_Int64:
			{
				INT64 Value = *((INT64*)(IDData->Point + int(Data)));
				sprintf( Buf ," %s = %I64d " , Iter->first.c_str() , Value  );
			}break;              
		case EM_PointType_Short:
			{                    
				short Value = *((short*)(IDData->Point + int(Data)));
				sprintf( Buf ," %s = %d " , Iter->first.c_str() , Value  );
			}                
			break;

		case EM_PointType_BinaryData:
			{
				char* Str = ((char*)(IDData->Point + int(Data)));
				sprintf( Buf , " %s = CAST( %s AS binary(%d) ) ", Iter->first.c_str() , StringToSqlX( Str, IDData->Size , false ).c_str() , IDData->Size );
			}
			break;
		case EM_PointType_TextData:
			{
				char* Str = ((char*)(IDData->Point + int(Data)));
				sprintf( Buf , " %s ='%s' ", Iter->first.c_str() , StringToSqlStr( Str , IDData->Size ).c_str()  );
			}
			break;

		case EM_PointType_CharString:
			{
				char* Str = ((char*)(IDData->Point + int(Data)));
				sprintf( Buf , " %s = CAST( %s AS varchar(8000) ) ", Iter->first.c_str() , StringToSqlX( Str , IDData->Size ).c_str()  );
			}
			break;
		case EM_PointType_WCharString:
			{
				char* Str = ((char*)(IDData->Point + int(Data)));
				int strLen = __min( (int)wcslen( (wchar_t*)Str ) * 2  , IDData->Size );
				sprintf( Buf , " %s = CAST( %s AS nvarchar(4000) ) ", Iter->first.c_str() , StringToSqlX( Str , strLen , false ).c_str()  );
			}
			break;

		case EM_PointType_CharStringPoint:
		case EM_PointType_TextDataPoint:
		case EM_PointType_BinaryCharPoint:
			{
				char* Str = *((char**)(IDData->Point + int(Data)));

				sprintf( Buf , " %s = CAST( %s AS varchar(8000) ) ", Iter->first.c_str() , StringToSqlX( Str , IDData->Size ).c_str()  );
			}
			break;
		case EM_PointType_Bit:
			{
				int Value = *((bool*)(IDData->Point + int(Data)));
				sprintf( Buf ," %s = %d " , Iter->first.c_str() , Value  );
			}
			break;
		case EM_PointType_SmallDateTime:
			{
				float Value = *((float*)(IDData->Point + int(Data)));
				sprintf( Buf ," %s = %f " , Iter->first.c_str() , Value  );
			}break;   
		default:
			continue;
		}
		if( !Beginflag  )
		{                
			SQLCmd = SQLCmd + ",";
		}
		else
			Beginflag = false;

		SQLCmd = SQLCmd + Buf;            

	}

	SQLCmd = SQLCmd + WhereCmd ;
	//	_SQLCmd = SQLCmd;
	//	_CritSect.Leave();
	return SQLCmd;
}
//--------------------------------------------------------------------------------------------------------
string CreateDBCmdClass::GetSelectStr( const char* WhereCmd , int TopCount )
{
	string SQLCmd;
	//	_CritSect.Enter();
	//產生 sql 命令( 利用 Reader產生)
	if( TopCount == 0 )
	{
		SQLCmd = "Select ";
	}
	else
	{
		char	Buf[256];
		sprintf( Buf , "Select Top %d " , TopCount );
		SQLCmd = Buf;
	}

	SQLCmd = SQLCmd+_SQLSelectCmd + WhereCmd;
	//_SQLCmd = SQLCmd;

	//	_CritSect.Leave();
	return SQLCmd;
}
//--------------------------------------------------------------------------------------------------------
