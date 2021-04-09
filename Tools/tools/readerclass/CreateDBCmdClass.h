#pragma once
#include <string>
#include "ReaderClass/ReaderClass.h"
#include "BaseFunction/BaseFunction.h"
#include "smallobj/SmallObj.h"

#define Def_Disabled_Update_Flag 0x01
#define Def_Disabled_Insert_Flag 0x02

#pragma unmanaged
//-----------------------------------------------------------------------------------------------------------------
// Critical Safe class
//-----------------------------------------------------------------------------------------------------------------
template <class Temp>
class CreateDBCmdClass
{
	ReaderClass<Temp> *_Reader;

	string _SQLSelectCmd;
	string _SQLInsertCmd;
	string _SQLUpDateCmd;
	string _TableName;

	bool _NoString; //��ۦ��X

  public:
	CreateDBCmdClass(ReaderClass<Temp> *Reader, char *TableName, bool IsNoString = true);
	string GetInsertStr(Temp *Date);
	string GetUpDateStr(Temp *Date, char *WhereCmd);
	string GetSelectStr(char *WhereCmd, int TopCount = 0);
	string GetCreateStr(char *useDataBase);
	//����X����r�ꪺ���� (�q�`���F��r���X��t�~����)
	//	void  NoString( bool Flag ){ _NoString = Flag; }
};
//-----------------------------------------------------------------------------------------------------------------
template <class Temp>
CreateDBCmdClass<Temp>::CreateDBCmdClass(ReaderClass<Temp> *Reader, char *TableName, bool IsNoString)
{
	_Reader = Reader;
	_NoString = IsNoString;

	string Str;
	bool IsBegin;

	//----------------SQL �r�겣�� ---------------------------
	map<string, ReadInfoStruct> *ReadInfo = Reader->ReadInfo();
	map<string, ReadInfoStruct>::iterator Iter;

	_SQLSelectCmd = "";
	_SQLInsertCmd = "Insert ";
	_SQLInsertCmd = _SQLInsertCmd + TableName + "( ";
	_TableName = TableName;
	IsBegin = true;
	Str = "";
	for (Iter = ReadInfo->begin(); Iter != ReadInfo->end(); Iter++)
	{
		ReadInfoStruct *Info = &(Iter->second);
		if (_NoString != false)
		{

			switch (Info->Type)
			{
			case ENUM_Type_CharStringPoint:
			case ENUM_Type_TextDataPoint:
				continue;
			}
		}
		if (Info->UserData & Def_Disabled_Select_Flag)
			continue;

		if (IsBegin == false)
		{
			Str = Str + ",";
		}
		IsBegin = false;

		if (ENUM_Type_SmallDateTime == Info->Type)
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
	IsBegin = true;
	Str = "";
	for (Iter = ReadInfo->begin(); Iter != ReadInfo->end(); Iter++)
	{
		ReadInfoStruct *Info = &(Iter->second);
		if (Info->UserData & Def_Disabled_Insert_Flag)
			continue;

		if (_NoString != false)
		{
			switch (Info->Type)
			{
			case ENUM_Type_CharStringPoint:
			case ENUM_Type_TextDataPoint:
				continue;
			}
		}

		if (IsBegin == false)
		{
			Str = Str + ",";
		}
		IsBegin = false;

		Str = Str + Iter->first;
	}
	_SQLInsertCmd = _SQLInsertCmd + Str + ") values( ";
}
//--------------------------------------------------------------------------------------------------------
template <class Temp>
string CreateDBCmdClass<Temp>::GetInsertStr(Temp *Data)
{
	char Buf[0x20000];
	//	_CritSect.Enter();
	map<string, ReadInfoStruct> *ReadInfo;
	map<string, ReadInfoStruct>::iterator Iter;
	string SQLCmd = _SQLInsertCmd;
	ReadInfo = _Reader->ReadInfo();
	bool BeginFlag = true;

	for (Iter = ReadInfo->begin();
		 Iter != ReadInfo->end();
		 Iter++)
	{
		ReadInfoStruct *IDData = &(Iter->second);

		if (IDData->UserData & Def_Disabled_Insert_Flag)
			continue;

		if (_NoString != false)
		{
			switch (IDData->Type)
			{
			case ENUM_Type_CharStringPoint:
			case ENUM_Type_TextDataPoint:
				continue;
			}
		}

		if (IDData->AddressFlag == ENUM_Indirect_Address)
		{
			switch (IDData->Type)
			{
			case ENUM_Type_FloatPoint:
			{
				float Value = *(float *)(IDData->Point + UINT64(Data));
				if (CheckFloatAvlid(Value) == false)
					Value = 0;
				sprintf(Buf, "%e", Value);
			}
			break;

			case ENUM_Type_IntPoint:
			{
				int Value = *((int *)(IDData->Point + UINT64(Data)));
				sprintf(Buf, "%d", Value);
			}
			break;
			case ENUM_Type_ShortPoint:
			{
				short Value = *((short *)(IDData->Point + UINT64(Data)));
				sprintf(Buf, "%d", Value);
			}
			break;
			case ENUM_Type_TimeCharString:
			{
				char *Str = ((char *)(IDData->Point + UINT64(Data)));
				replace(Str, Str + strlen(Str), '\'', '\"');
				sprintf(Buf, "'%s'", Str);
			}
			break;
			case ENUM_Type_BinaryData:
			{
				char *Str = ((char *)(IDData->Point + UINT64(Data)));
				sprintf(Buf, " CAST( %s AS binary(%d) ) ", StringToSqlX(Str, IDData->Size, false).c_str(), IDData->Size);
			}
			break;

			case ENUM_Type_CharString:
			case ENUM_Type_TextData:
			{
				char *Str = ((char *)(IDData->Point + UINT64(Data)));
				sprintf(Buf, " CAST( %s AS varchar(8000) ) ", StringToSqlX(Str, IDData->Size).c_str());
			}
			break;
			case ENUM_Type_WCharString:
			{
				char *Str = ((char *)(IDData->Point + UINT64(Data)));
				wchar_t wcBuf[8096];
				int Strlen = MultiByteToWideChar(CP_UTF8, 0, Str, -1, wcBuf, sizeof(wcBuf)) * 2;
				sprintf(Buf, " CAST( %s AS nvarchar(4000) ) ", StringToSqlX((char *)wcBuf, Strlen, false).c_str());
			}
			break;
			case ENUM_Type_WCharStringBase:
			{
				char *Str = ((char *)(IDData->Point + UINT64(Data)));
				sprintf(Buf, " CAST( %s AS nvarchar(4000) ) ", StringToSqlX(Str, IDData->Size, false).c_str());
			}
			break;

			case ENUM_Type_CharStringPoint:
			{
				char *Str = *((char **)(IDData->Point + UINT64(Data)));
				sprintf(Buf, " CAST( %s As varchar(8000) ) ", StringToSqlX(Str, IDData->Size).c_str());
			}
			break;

			case ENUM_Type_TextDataPoint:
			{
				char *Str = *((char **)(IDData->Point + UINT64(Data)));
				sprintf(Buf, " CAST( %s As Text ) ", StringToSqlX(Str, IDData->Size).c_str());
			}
			break;

			case ENUM_Type_BitPoint:
			{
				bool Value = *((bool *)(IDData->Point + UINT64(Data)));
				sprintf(Buf, "%d", Value);
			}
			break;
			case ENUM_Type_SmallDateTime:
			{
				float Value = *((float *)(IDData->Point + UINT64(Data)));
				sprintf(Buf, "%f", Value);
			}
			break;
			default:
				continue;
			}

			if (BeginFlag == false)
				SQLCmd = SQLCmd + ",";
			else
				BeginFlag = false;

			SQLCmd = SQLCmd + Buf;
		}
	}
	SQLCmd = SQLCmd + ")";
	//	_SQLCmd = SQLCmd;
	//	_CritSect.Leave();
	return SQLCmd;
}
//--------------------------------------------------------------------------------------------------------
template <class Temp>
string CreateDBCmdClass<Temp>::GetUpDateStr(Temp *Data, char *WhereCmd)
{
	//	_CritSect.Enter();
	char Buf[0x20000];
	bool Beginflag = true;

	map<string, ReadInfoStruct> *ReadInfo;
	map<string, ReadInfoStruct>::iterator Iter;
	ReadInfo = _Reader->ReadInfo();
	string SQLCmd = _SQLUpDateCmd;

	for (Iter = ReadInfo->begin(); Iter != ReadInfo->end(); Iter++)
	{
		ReadInfoStruct *IDData = &(Iter->second);

		string TempStr = Iter->first;

		if (IDData->UserData & Def_Disabled_Update_Flag)
			continue;

		if (_NoString != false)
		{
			switch (IDData->Type)
			{
			case ENUM_Type_CharStringPoint:
			case ENUM_Type_TextDataPoint:
				continue;
			}
		}

		if (IDData->AddressFlag == ENUM_Indirect_Address)
		{
			switch (IDData->Type)
			{
			case ENUM_Type_FloatPoint:
			{
				float Value = *(float *)(IDData->Point + UINT64(Data));
				bool Ret = CheckFloatAvlid(Value);
				if (CheckFloatAvlid(Value) == false)
					Value = 0;
				sprintf(Buf, " %s = %e ", Iter->first.c_str(), Value);
			}
			break;

			case ENUM_Type_IntPoint:
			{
				int Value = *((int *)(IDData->Point + UINT64(Data)));
				sprintf(Buf, " %s = %d ", Iter->first.c_str(), Value);
			}
			break;
			case ENUM_Type_ShortPoint:
			{
				short Value = *((short *)(IDData->Point + UINT64(Data)));
				sprintf(Buf, " %s = %d ", Iter->first.c_str(), Value);
			}
			break;

			case ENUM_Type_TimeCharString:
			{
				char *Str = ((char *)(IDData->Point + UINT64(Data)));
				replace(Str, Str + strlen(Str), '\'', '\"');
				sprintf(Buf, " %s = '%s' ", Iter->first.c_str(), Str);
			}
			break;
			case ENUM_Type_BinaryData:
			{
				char *Str = ((char *)(IDData->Point + UINT64(Data)));
				sprintf(Buf, " %s = CAST( %s AS binary(%d) ) ", Iter->first.c_str(), StringToSqlX(Str, IDData->Size, false).c_str(), IDData->Size);
			}
			break;
			case ENUM_Type_TextData:
			{
				char *Str = ((char *)(IDData->Point + UINT64(Data)));
				sprintf(Buf, " %s ='%s' ", Iter->first.c_str(), StringToSqlStr(Str, IDData->Size).c_str());
			}
			break;

			case ENUM_Type_CharString:
			{
				char *Str = ((char *)(IDData->Point + UINT64(Data)));
				sprintf(Buf, " %s = CAST( %s AS varchar(8000) ) ", Iter->first.c_str(), StringToSqlX(Str, IDData->Size).c_str());
			}
			break;
			case ENUM_Type_WCharString:
			{
				char *Str = ((char *)(IDData->Point + UINT64(Data)));

				wchar_t wcBuf[8096];
				int Strlen = MultiByteToWideChar(CP_UTF8, 0, Str, -1, wcBuf, sizeof(wcBuf)) * 2;

				sprintf(Buf, " %s = CAST( %s AS nvarchar(4000) ) ", Iter->first.c_str(), StringToSqlX((char *)wcBuf, Strlen, false).c_str());
			}
			break;
			case ENUM_Type_WCharStringBase:
			{
				char *Str = ((char *)(IDData->Point + UINT64(Data)));
				sprintf(Buf, " %s = CAST( %s AS nvarchar(4000) ) ", Iter->first.c_str(), StringToSqlX(Str, IDData->Size, false).c_str());
			}
			break;

			case ENUM_Type_CharStringPoint:
			{
				char *Str = *((char **)(IDData->Point + UINT64(Data)));

				sprintf(Buf, " %s = CAST( %s AS varchar(8000) ) ", Iter->first.c_str(), StringToSqlX(Str, IDData->Size).c_str());
			}
			break;
			case ENUM_Type_TextDataPoint:
			{
				char *Str = *((char **)(IDData->Point + UINT64(Data)));

				sprintf(Buf, " %s = CAST( %s AS Text ) ", Iter->first.c_str(), StringToSqlX(Str, IDData->Size).c_str());
			}
			break;
			case ENUM_Type_BitPoint:
			{
				int Value = *((bool *)(IDData->Point + UINT64(Data)));
				sprintf(Buf, " %s = %d ", Iter->first.c_str(), Value);
			}
			break;
			case ENUM_Type_SmallDateTime:
			{
				float Value = *((float *)(IDData->Point + UINT64(Data)));
				sprintf(Buf, " %s = %f ", Iter->first.c_str(), Value);
			}
			break;
			default:
				continue;
			}
			if (!Beginflag)
			{
				SQLCmd = SQLCmd + ",";
			}
			else
				Beginflag = false;

			SQLCmd = SQLCmd + Buf;
		}
	}

	SQLCmd = SQLCmd + WhereCmd;
	//	_SQLCmd = SQLCmd;
	//	_CritSect.Leave();
	return SQLCmd;
}
//--------------------------------------------------------------------------------------------------------
template <class Temp>
string CreateDBCmdClass<Temp>::GetCreateStr(char *useDataBase)
{
	char Buf[0x20000];
	char BufDef[0x20000];
	bool Beginflag = true;

	map<string, ReadInfoStruct> *ReadInfo;
	map<string, ReadInfoStruct>::iterator Iter;
	ReadInfo = _Reader->ReadInfo();
	string SQLDefaultCmd = "";
	string SQLCmd = "USE [" + std::string(useDataBase) + "]" + "\n" + "GO" + "\n";
	SQLCmd = SQLCmd + "SET ANSI_NULLS ON" + "\n" + "GO" + "\n";
	SQLCmd = SQLCmd + "SET QUOTED_IDENTIFIER ON" + "\n" + "GO" + "\n";
	SQLCmd = SQLCmd + "SET ANSI_PADDING ON" + "\n" + "GO" + "\n";

	SQLCmd = SQLCmd + "CREATE TABLE [dbo].[" + _TableName + "](" + "\n";

	for (Iter = ReadInfo->begin(); Iter != ReadInfo->end(); Iter++)
	{
		ReadInfoStruct *IDData = &(Iter->second);

		string TempStr = Iter->first;

		// if (IDData->UserData & Def_Disabled_Update_Flag)
		// 	continue;

		if (_NoString != false)
		{
			switch (IDData->Type)
			{
			case ENUM_Type_CharStringPoint:
			case ENUM_Type_TextDataPoint:
				continue;
			}
		}

		if (IDData->AddressFlag == ENUM_Indirect_Address)
		{
			switch (IDData->Type)
			{
			case ENUM_Type_FloatPoint:
			{
				sprintf(Buf, "[%s] [real] NOT NULL,\n", Iter->first.c_str());
				sprintf(BufDef, "ALTER TABLE [dbo].[%s] ADD CONSTRAINT [DF_%s_%s] DEFAULT ((0)) FOR [%s]\nGO\n", _TableName.c_str(), _TableName.c_str(), Iter->first.c_str(), Iter->first.c_str());
			}
			break;
			case ENUM_Type_IntPoint:
			{
				std::cout << "first " << Iter->first.c_str() << std::endl;
				sprintf(Buf, "[%s] [int] NOT NULL,\n", Iter->first.c_str());
				std::cout << "two" << std::endl;
				sprintf(BufDef, "ALTER TABLE [dbo].[%s] ADD CONSTRAINT [DF_%s_%s] DEFAULT ((0)) FOR [%s]\nGO\n", _TableName.c_str(), _TableName.c_str(), Iter->first.c_str(), Iter->first.c_str());
			}
			break;
			case ENUM_Type_ShortPoint:
			{
				sprintf(Buf, "[%s] [smallint] NOT NULL,\n", Iter->first.c_str());
				sprintf(BufDef, "ALTER TABLE [dbo].[%s] ADD CONSTRAINT [DF_%s_%s] DEFAULT ((0)) FOR [%s]\nGO\n", _TableName.c_str(), _TableName.c_str(), Iter->first.c_str(), Iter->first.c_str());
			}
			break;
			case ENUM_Type_TimeCharString:
			{
				sprintf(Buf, "[%s] [datetime] NOT NULL,\n", Iter->first.c_str());
				sprintf(BufDef, "ALTER TABLE [dbo].[%s] ADD CONSTRAINT [DF_%s_%s] DEFAULT (('')) FOR [%s]\nGO\n", _TableName.c_str(), _TableName.c_str(), Iter->first.c_str(), Iter->first.c_str());
			}
			break;
			case ENUM_Type_BinaryData:
			{
				sprintf(Buf, "[%s] [varbinary](%d) NOT NULL,\n", Iter->first.c_str(), IDData->Size);
				sprintf(BufDef, "ALTER TABLE [dbo].[%s] ADD CONSTRAINT [DF_%s_%s] DEFAULT ((0)) FOR [%s]\nGO\n", _TableName.c_str(), _TableName.c_str(), Iter->first.c_str(), Iter->first.c_str());
			}
			break;
			case ENUM_Type_TextData:
			{
				sprintf(Buf, "[%s] [varchar](%d) NOT NULL,\n", Iter->first.c_str(), IDData->Size);
				sprintf(BufDef, "ALTER TABLE [dbo].[%s] ADD CONSTRAINT [DF_%s_%s] DEFAULT (('')) FOR [%s]\nGO\n", _TableName.c_str(), _TableName.c_str(), Iter->first.c_str(), Iter->first.c_str());
			}
			break;
			case ENUM_Type_CharString:
			{
				sprintf(Buf, "[%s] [varchar](8000) NOT NULL,\n", Iter->first.c_str());
				sprintf(BufDef, "ALTER TABLE [dbo].[%s] ADD CONSTRAINT [DF_%s_%s] DEFAULT (('')) FOR [%s]\nGO\n", _TableName.c_str(), _TableName.c_str(), Iter->first.c_str(), Iter->first.c_str());
			}
			break;
			case ENUM_Type_WCharString:
			{
				sprintf(Buf, "[%s] [nvarchar](4000) NOT NULL,\n", Iter->first.c_str());
				sprintf(BufDef, "ALTER TABLE [dbo].[%s] ADD CONSTRAINT [DF_%s_%s] DEFAULT (('')) FOR [%s]\nGO\n", _TableName.c_str(), _TableName.c_str(), Iter->first.c_str(), Iter->first.c_str());
			}
			break;
			case ENUM_Type_WCharStringBase:
			{
				sprintf(Buf, "[%s] [nvarchar](4000) NOT NULL,\n", Iter->first.c_str());
				sprintf(BufDef, "ALTER TABLE [dbo].[%s] ADD CONSTRAINT [DF_%s_%s] DEFAULT (('')) FOR [%s]\nGO\n", _TableName.c_str(), _TableName.c_str(), Iter->first.c_str(), Iter->first.c_str());
			}
			break;
			case ENUM_Type_CharStringPoint:
			{
				sprintf(Buf, "[%s] [varchar](8000) NOT NULL,\n", Iter->first.c_str());
				sprintf(BufDef, "ALTER TABLE [dbo].[%s] ADD CONSTRAINT [DF_%s_%s] DEFAULT (('')) FOR [%s]\nGO\n", _TableName.c_str(), _TableName.c_str(), Iter->first.c_str(), Iter->first.c_str());
			}
			break;
			case ENUM_Type_TextDataPoint:
			{
				continue;
			}
			break;
			case ENUM_Type_BitPoint:
			{
				sprintf(Buf, "[%s] [bit] NOT NULL,\n", Iter->first.c_str());
				sprintf(BufDef, "ALTER TABLE [dbo].[%s] ADD CONSTRAINT [DF_%s_%s] DEFAULT ((0)) FOR [%s]\nGO\n", _TableName.c_str(), _TableName.c_str(), Iter->first.c_str(), Iter->first.c_str());
			}
			break;
			case ENUM_Type_SmallDateTime:
			{
				sprintf(Buf, "[%s] [smalldatetime] NOT NULL,\n", Iter->first.c_str());
				sprintf(BufDef, "ALTER TABLE [dbo].[%s] ADD CONSTRAINT [DF_%s_%s] DEFAULT ((0)) FOR [%s]\nGO\n", _TableName.c_str(), _TableName.c_str(), Iter->first.c_str(), Iter->first.c_str());
			}
			break;
			default:
				continue;
			}
			// if (!Beginflag)
			// {
			// 	SQLCmd = SQLCmd + ",";
			// }
			// else
			// 	Beginflag = false;

			SQLCmd = SQLCmd + Buf;
			SQLDefaultCmd = SQLDefaultCmd + BufDef;
		}
	}
	std::cout << "return 1" << std::endl;
	SQLCmd = SQLCmd + ") ON [PRIMARY]\nGO\n";
	std::cout << "return 2" << std::endl;
	SQLCmd = SQLCmd + "SET ANSI_PADDING OFF" + "\n" + "GO" + "\n";
	std::cout << "return 3" << std::endl;
	SQLCmd = SQLCmd + SQLDefaultCmd;
	std::cout << "return 4" << std::endl;
	return SQLCmd;
}

template <class Temp>
string CreateDBCmdClass<Temp>::GetSelectStr(char *WhereCmd, int TopCount)
{
	string SQLCmd;
	//	_CritSect.Enter();
	//���� sql �R�O( �Q�� Reader����)
	if (TopCount == 0)
	{
		SQLCmd = "Select ";
	}
	else
	{
		char Buf[256];
		sprintf(Buf, "Select Top %d ", TopCount);
		SQLCmd = Buf;
	}

	SQLCmd = SQLCmd + _SQLSelectCmd + WhereCmd;
	//_SQLCmd = SQLCmd;

	//	_CritSect.Leave();
	return SQLCmd;
}
//--------------------------------------------------------------------------------------------------------
#pragma managed
