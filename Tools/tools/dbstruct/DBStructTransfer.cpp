#include "DBStructTransfer.h"

bool DBStructTransferClass::ReadDBDataByReader(void* OutData, HSTMT dbhstmt, ReaderClass* Reader)
{
	int		i;
	map<string, PointInfoStruct>* ReadInfo;
	map<string, PointInfoStruct>::iterator    Iter;
	ReadInfo = Reader->ReadInfo();

	for (Iter = ReadInfo->begin(), i = 1;
		Iter != ReadInfo->end();
		Iter++, i++)
	{

		PointInfoStruct* IDData = &(Iter->second);

		if (IDData->UserData & Def_Disabled_Select_Flag)
			continue;


		bool bindResult = false;
		switch (IDData->Type)
		{
		case EM_PointType_Float:
			bindResult = Proxy_SQLBindCol(dbhstmt, i, SQL_C_FLOAT, (SQLPOINTER)(IDData->Point + int(OutData)), SQL_C_DEFAULT, NULL);
			break;

		case EM_PointType_Int:
			bindResult = Proxy_SQLBindCol(dbhstmt, i, SQL_C_LONG, (SQLPOINTER)(IDData->Point + int(OutData)), SQL_C_DEFAULT, NULL);
			break;
		case EM_PointType_Int64:
			bindResult = Proxy_SQLBindCol(dbhstmt, i, SQL_C_SBIGINT, (SQLPOINTER)(IDData->Point + int(OutData)), SQL_C_DEFAULT, NULL);
			break;

		case EM_PointType_Short:
			bindResult = Proxy_SQLBindCol(dbhstmt, i, SQL_C_SHORT, (SQLPOINTER)(IDData->Point + int(OutData)), SQL_C_DEFAULT, NULL);
			break;

		case EM_PointType_CharString:
			bindResult = Proxy_SQLBindCol(dbhstmt, i, SQL_C_CHAR, (SQLPOINTER)(IDData->Point + int(OutData)), IDData->Size, NULL);
			break;
		case EM_PointType_WCharString:
			bindResult = Proxy_SQLBindCol(dbhstmt, i, SQL_C_WCHAR, (SQLPOINTER)(IDData->Point + int(OutData)), IDData->Size, NULL);
			break;

		case  EM_PointType_BinaryData:
			bindResult = Proxy_SQLBindCol(dbhstmt, i, SQL_C_BINARY, (SQLPOINTER)(IDData->Point + int(OutData)), IDData->Size, NULL);
			break;
		case EM_PointType_Bit:
			bindResult = Proxy_SQLBindCol(dbhstmt, i, SQL_C_BIT, (SQLPOINTER)(IDData->Point + int(OutData)), SQL_C_DEFAULT, NULL);
			break;

		case EM_PointType_SmallDateTime:
			bindResult = Proxy_SQLBindCol(dbhstmt, i, SQL_C_FLOAT, (SQLPOINTER)(IDData->Point + int(OutData)), SQL_C_DEFAULT, NULL);
			break;
		}

		if (!bindResult)
			return false;

	}

	return true;
};

void DBStructTransferClass::NullEventCB(SqlBaseClass* sqlBase, void* UserObj, ArgTransferStruct& Arg)
{

}

//--------------------------------------------------------------------------------------
void DBStructTransferClass::WriteEventCB(SqlBaseClass* sqlBase, void* UserObj, ArgTransferStruct& Arg)
{
	SQLCloseCursor(sqlBase->Gethstmt());

	char* SQLCMD = (char*)Arg.GetString("SqlCmd");
#ifdef _DEBUG
	PrintToFile(LV5, "SQLCMD", SQLCMD);
#endif

	DBWriteInfoStruct* WriteObj = (DBWriteInfoStruct*)UserObj;
	DBStructTransferClass* This = WriteObj->This;

	if (sqlBase->TrySQLExecDirect(5, SQLCMD))
		WriteObj->ResultOK = true;
	else
		WriteObj->ResultOK = false;

	{
		This->_Thread_CritSect.Enter();
		This->_WriteEventResult.push_back(WriteObj);
		This->_Thread_CritSect.Leave();
	}

	SQLCloseCursor(sqlBase->Gethstmt());
}
//--------------------------------------------------------------------------------------
DBStructTransferClass::DBStructTransferClass(DBTransferClass* DBConnect, ReaderClass* Reader, char* TableName)
{
	string  Str;
	bool    IsBegin;

	_Reader = Reader;
	_DBConnect = DBConnect;
	if (Reader == NULL)
		return;
	map<string, PointInfoStruct>* ReadInfo = Reader->ReadInfo();
	map<string, PointInfoStruct>::iterator Iter;

	_SQLSelectCmd = "Select ";
	_SQLInsertCmd = "Insert ";
	;
	_SQLInsertCmd = _SQLInsertCmd + TableName + "( ";

	IsBegin = true;
	Str = "";
	for (Iter = ReadInfo->begin(); Iter != ReadInfo->end(); Iter++)
	{
		PointInfoStruct* Info = &(Iter->second);

		if (Info->UserData & Def_Disabled_Select_Flag)
			continue;

		if (IsBegin == false)
		{
			Str = Str + ",";
		}
		IsBegin = false;

		if (EM_PointType_SmallDateTime == Info->Type)
		{
			Str = Str + "CAST(";
			Str = Str + Iter->first;
			Str = Str + " AS float )";
		}
		else
			Str = Str + Iter->first;
	}

	_SQLSelectCmd = _SQLSelectCmd + Str + " From " + TableName + " ";
	_SQLUpDateCmd = "UPDATE ";
	_SQLUpDateCmd = _SQLUpDateCmd + TableName + " SET ";

	IsBegin = true;
	Str = "";
	for (Iter = ReadInfo->begin(); Iter != ReadInfo->end(); Iter++)
	{
		PointInfoStruct* Info = &(Iter->second);
		if (Info->UserData & Def_Disabled_Insert_Flag)
			continue;

		if (IsBegin == false)
		{
			Str = Str + ",";
		}
		IsBegin = false;

		Str = Str + Iter->first;

	}
	_SQLInsertCmd = _SQLInsertCmd + Str + ") values( ";
}

DBStructTransferClass::~DBStructTransferClass()
{
}

void DBStructTransferClass::Write(int ID, void* Data, DBWriteEventCB CBFunc, void* UserObj, const char* WhereCmd)
{
	DBWriteInfoStruct* WriteInfo = _AllocWriteEvent.NewObj();
	WriteInfo->Init();
	WriteInfo->UserObj = UserObj;
	WriteInfo->EventFunc = CBFunc;
	WriteInfo->This = this;
	WriteInfo->WhereCmd = WhereCmd;

	char    Buf[256];
	string  SQLCmd;
	bool    BeginFlag = true;

	map<string, PointInfoStruct>* ReadInfo;
	map<string, PointInfoStruct>::iterator    Iter;

	{
		_Thread_CritSect.Enter();
		ReadInfo = _Reader->ReadInfo();
		_Thread_CritSect.Leave();
	}

	if (strlen(WhereCmd) == 0)
	{
		SQLCmd = _SQLInsertCmd;
		for (Iter = ReadInfo->begin();
			Iter != ReadInfo->end();
			Iter++)
		{
			PointInfoStruct* IDData = &(Iter->second);

			if (IDData->UserData & Def_Disabled_Insert_Flag)
				continue;


			switch (IDData->Type)
			{
			case EM_PointType_Float:
			{
				float Value = *(float*)(IDData->Point + int(Data));
				if (CheckFloatAvlid(Value) == false)
					Value = 0;
				sprintf(Buf, "%e", Value);
			}break;

			case EM_PointType_Int:
			{
				int Value = *((int*)(IDData->Point + int(Data)));
				sprintf(Buf, "%d", Value);
			}break;
			case EM_PointType_Int64:
			{
				INT64 Value = *((INT64*)(IDData->Point + int(Data)));
				sprintf(Buf, "%I64d", Value);
			}break;
			case EM_PointType_Short:
			{
				short Value = *((short*)(IDData->Point + int(Data)));
				sprintf(Buf, "%d", Value);
			}
			break;
			case EM_PointType_Bit:
			{
				bool	Value = *((bool*)(IDData->Point + int(Data)));
				sprintf(Buf, "%d", Value);
			}
			break;
			case EM_PointType_CharString:
			{
				char* Str = ((char*)(IDData->Point + int(Data)));
				sprintf(Buf, "'%s'", Str);
			}
			break;
			case EM_PointType_BinaryData:
			{
				char* Str = ((char*)(IDData->Point + int(Data)));
				sprintf(Buf, " CAST( %s AS binary(%d) ) ", StringToSqlX(Str, IDData->Size, false).c_str(), IDData->Size);
			}
			break;
			case EM_PointType_SmallDateTime:
			{
				float Value = *((float*)(IDData->Point + int(Data)));
				sprintf(Buf, "%f", Value);
			}
			break;


			default:
				continue;
			}

			if (BeginFlag == false)
			{
				SQLCmd = SQLCmd + ",";
			}
			else
				BeginFlag = false;

			SQLCmd = SQLCmd + Buf;

		}
		SQLCmd = SQLCmd + ")";
	}
	else
	{
		SQLCmd = _SQLUpDateCmd;

		for (Iter = ReadInfo->begin();
			Iter != ReadInfo->end();
			Iter++)
		{
			PointInfoStruct* IDData = &(Iter->second);

			if (IDData->UserData & Def_Disabled_Update_Flag)
				continue;


			switch (IDData->Type)
			{
			case EM_PointType_Float:
			{
				float Value = *(float*)(IDData->Point);
				if (CheckFloatAvlid(Value) == false)
					Value = 0;
				sprintf(Buf, " %s = %e ", Iter->first.c_str(), Value);
			}break;

			case EM_PointType_Int:
			{
				int Value = *((int*)(IDData->Point));
				sprintf(Buf, " %s = %d ", Iter->first.c_str(), Value);
			}break;
			case EM_PointType_Int64:
			{
				INT64 Value = *((INT64*)(IDData->Point));
				sprintf(Buf, " %s = %I64d ", Iter->first.c_str(), Value);
			}break;

			case EM_PointType_Short:
			{
				short Value = *((short*)(IDData->Point));
				sprintf(Buf, " %s = %d ", Iter->first.c_str(), Value);
			}
			break;
			case EM_PointType_Bit:
			{
				bool	Value = *((bool*)(IDData->Point));
				sprintf(Buf, " %s = %d ", Iter->first.c_str(), Value);
			}
			break;
			case EM_PointType_CharString:
			{
				char* Str = ((char*)(IDData->Point));
				sprintf(Buf, " %s = '%s' ", Iter->first.c_str(), Str);
			}
			break;
			case EM_PointType_BinaryData:
			{
				char* Str = ((char*)(IDData->Point));
				sprintf(Buf, " %s = CAST( %s AS binary(%d) ) ", Iter->first.c_str(), StringToSqlX(Str, IDData->Size, false).c_str(), IDData->Size);
			}
			break;
			case EM_PointType_SmallDateTime:
			{
				float Value = *((float*)(IDData->Point + int(Data)));
				sprintf(Buf, "%s = %f ", Iter->first.c_str(), Value);
			}
			break;
			default:
				continue;
			}
			if (BeginFlag == false)
			{
				SQLCmd = SQLCmd + ",";
			}
			else
				BeginFlag = false;

			SQLCmd = SQLCmd + Buf;

		}
		SQLCmd = SQLCmd + WhereCmd;
	}

	_DBConnect->Write(ID, WriteEventCB, WriteInfo, "SqlCmd", EM_ValueType_String, (char*)SQLCmd.c_str(), NULL);
}

void DBStructTransferClass::Process()
{
	while (1)
	{
		DBSqlCmdInfoStruct* DBResult;
		{
			_Thread_CritSect.Enter();
			if (_DBSqlCmdEventResult.size() == 0)
			{
				_Thread_CritSect.Leave();
				break;
			}

			DBResult = _DBSqlCmdEventResult.front();
			_DBSqlCmdEventResult.pop_front();
			_Thread_CritSect.Leave();
		}
		if (DBResult->ResultFunc)
			DBResult->ResultFunc(DBResult->UserObj, DBResult->Arg, DBResult->ResultOK);

		_AllocSqlCmdEvent.DeleteObj(DBResult);
	}

	while (1)
	{
		DBWriteInfoStruct* DBResult;
		{
			_Thread_CritSect.Enter();
			if (_WriteEventResult.size() == 0)
			{
				_Thread_CritSect.Leave();
				break;
			}

			DBResult = _WriteEventResult.front();
			_WriteEventResult.pop_front();
			_Thread_CritSect.Leave();
		}
		if (DBResult->EventFunc)
			DBResult->EventFunc(DBResult->UserObj, (char*)DBResult->WhereCmd.c_str(), DBResult->ResultOK);

		_AllocWriteEvent.DeleteObj(DBResult);
	}
}
//--------------------------------------------------------------------------------------
void DBStructTransferClass::BeginTransaction()
{
	_DBConnect->Write(0, NullEventCB, NULL, "BEGIN TRANSACTION");
}
//--------------------------------------------------------------------------------------
void DBStructTransferClass::CommitTransaction()
{
	_DBConnect->Write(0, NullEventCB, NULL, "COMMIT TRANSACTION");
}
//--------------------------------------------------------------------------------------
void    DBStructTransferClass::SqlCmd_Transcation(int ID, SQLCmdRequestEvent RequestFunc, SQLCmdReusltEventCB ResultFunc, void* UserObj, ...)
{
	DBSqlCmdInfoStruct* CmdInfo = _AllocSqlCmdEvent.NewObj();
	if (CmdInfo == NULL)
	{
		printf("\n SqlCmd 記憶體要求有問題??????? ");
		return;
	}
	CmdInfo->Init();

	CmdInfo->This = this;
	CmdInfo->UserObj = UserObj;
	CmdInfo->RequestFunc = RequestFunc;
	CmdInfo->ResultFunc = ResultFunc;
	CmdInfo->IsTransation = true;

	va_list		    	marker;
	ArgValueTypeENUM    type;
	char* name;
	char                Buf[256];
	char* value;

	va_start(marker, UserObj);
	while (1)
	{
		name = va_arg(marker, char*);
		if (name == NULL)
			break;
		type = (ArgValueTypeENUM)va_arg(marker, int);
		switch (type)
		{
		case EM_ValueType_Double:
			sprintf(Buf, "%e", va_arg(marker, double));
			value = Buf;
			break;
		case EM_ValueType_Int:
			sprintf(Buf, "%d", va_arg(marker, int));
			value = Buf;
			break;
		case EM_ValueType_String:
			value = va_arg(marker, char*);
			break;
		}
		CmdInfo->Arg.PushValue(name, value);
	}

	_DBConnect->SqlCmd(ID, SqlCmdEventCB, CmdInfo);
}

void    DBStructTransferClass::SqlCmd(int ID, SQLCmdRequestEvent RequestFunc, SQLCmdReusltEventCB ResultFunc, void* UserObj, ...)
{
	DBSqlCmdInfoStruct* CmdInfo = _AllocSqlCmdEvent.NewObj();
	if (CmdInfo == NULL)
	{
		printf("\n SqlCmd 記憶體要求有問題??????? ");
		return;
	}
	CmdInfo->Init();

	CmdInfo->This = this;
	CmdInfo->UserObj = UserObj;
	CmdInfo->RequestFunc = RequestFunc;
	CmdInfo->ResultFunc = ResultFunc;

	va_list		    	marker;
	ArgValueTypeENUM    type;
	char* name;
	char                Buf[256];
	char* value;

	va_start(marker, UserObj);
	while (1)
	{
		name = va_arg(marker, char*);
		if (name == NULL)
			break;
		type = (ArgValueTypeENUM)va_arg(marker, int);
		switch (type)
		{
		case EM_ValueType_Double:
			sprintf(Buf, "%e", va_arg(marker, double));
			value = Buf;
			break;
		case EM_ValueType_Int:
			sprintf(Buf, "%d", va_arg(marker, int));
			value = Buf;
			break;
		case EM_ValueType_String:
			value = va_arg(marker, char*);
			break;
		}
		CmdInfo->Arg.PushValue(name, value);
	}

	_DBConnect->SqlCmd(ID, SqlCmdEventCB, CmdInfo);
}

static bool	ST_SqlCmd_WriteOneLine(SqlBaseClass* sqlBase, const char* sqlcmd)
{

#ifdef _DEBUG
	PrintToFile(LV5, "SQLCMD", sqlcmd);
#endif
	if (sqlBase->TrySQLExecDirect(5, sqlcmd) == false)
	{
		return false;
	}

	__try
	{
		SQLCloseCursor(sqlBase->Gethstmt());
	}
	__except (GetExceptionCode() == STATUS_ACCESS_VIOLATION)
	{
		PrintToFile(LV5, "ODBC ERROR", "CRITICAL ODBC DRIVER ERROR! FUCK ME ALREADY! FUCK YOU TOO!");
		printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\nCRITICAL ODBC DRIVER ERROR! FUCK ME ALREADY! FUCK YOU TOO!\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	}

	return true;
}

void    DBStructTransferClass::SqlCmdEventCB(SqlBaseClass* sqlBase, void* UserObj, ArgTransferStruct& Arg)
{
	DBSqlCmdInfoStruct* CmdInfo = (DBSqlCmdInfoStruct*)UserObj;

	if (CmdInfo->IsTransation)
	{
		g_CritSect()->Enter();
		ST_SqlCmd_WriteOneLine(sqlBase, "BEGIN TRANSACTION");
	}

	CmdInfo->ResultOK = CmdInfo->RequestFunc(sqlBase, CmdInfo->UserObj, CmdInfo->Arg);

	if (CmdInfo->IsTransation)
	{
		ST_SqlCmd_WriteOneLine(sqlBase, "COMMIT TRANSACTION");
		g_CritSect()->Leave();
	}

	DBStructTransferClass* This = CmdInfo->This;

	{
		This->_Thread_CritSect.Enter();
		This->_DBSqlCmdEventResult.push_back(CmdInfo);
		This->_Thread_CritSect.Leave();
	}

	__try
	{
		SQLCloseCursor(sqlBase->Gethstmt());
	}
	__except (GetExceptionCode() == STATUS_ACCESS_VIOLATION)
	{
		PrintToFile(LV5, "ODBC ERROR", "CRITICAL ODBC DRIVER ERROR! FUCK ME ALREADY! FUCK YOU TOO!-1");
		printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\nCRITICAL ODBC DRIVER ERROR! FUCK ME ALREADY! FUCK YOU TOO!-1\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	}

}

void DBStructTransferClass::SQLCMDWrite(int ID, DBWriteEventCB CBFunc, void* UserObj, const char* SQL)
{
	DBWriteInfoStruct* WriteInfo = _AllocWriteEvent.NewObj();
	WriteInfo->Init();
	WriteInfo->UserObj = UserObj;
	WriteInfo->EventFunc = CBFunc;
	WriteInfo->This = this;

	WriteInfo->WhereCmd = SQL;

	_DBConnect->Write(ID, WriteEventCB, WriteInfo, "SqlCmd", EM_ValueType_String, SQL, NULL);
}
