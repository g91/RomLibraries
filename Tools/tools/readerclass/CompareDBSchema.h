#pragma once
#include "ReaderClass/ReaderClass.h"
#include "readerclass/DbSqlExecClass.h"

// DB Schema比對回傳錯誤定義
enum DBCompareErrorEnum
{
	ENUM_DBCompareError_None		= 0	, // 正常
	ENUM_DBCompareError_Column			, // Server有的欄位DB中沒有
	ENUM_DBCompareError_Type			, // 欄位型態不同
	ENUM_DBCompareError_Size			, // Server欄位大小比DB中的小
};

struct DBResultData
{
	bool			bResult;
	char			sResultMsg[8192];
};

struct TransferEventData
{
	CreateDBCmdClass*	pCreateDBCmdClass;
	bool*				pCompareDBResult;
	int*				pCompareProcCounter;
};

// 與DB驗證Schema是否符合規則
class CompareDBSchema
{ 
private:
	static bool	_IsLoadOK;
protected:
	static bool _CompareDBSchemaEvent ( SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _CompareDBSchemaEventResult (void* UserObj , ArgTransferStruct& Arg , bool ResultOK );	

public:
	// 執行驗證
	static void CompareDBSchemaProc(CreateDBCmdClass* pCreateDBCmd, DBStructTransferClass* pDBStructTransfer, bool* pCompareDBResult, int* pCompareProcCounter);
	// 
	static void	BlockProLoadDBProc( DBStructTransferClass* db , bool& checkFlag , char* msg ); 
	
	//static bool* Get_IsLoadOK(){ return (bool*)_IsLoadOK; };
};
bool CompareDBSchema::_IsLoadOK = false;
//--------------------------------------------------------------------------------------------------------
// 執行驗證
void CompareDBSchema::CompareDBSchemaProc(CreateDBCmdClass* pCreateDBCmd, DBStructTransferClass* pDBStructTransfer, bool* pCompareDBResult, int* pCompareProcCounter)
{
	(*pCompareProcCounter)++;

	TransferEventData* pEventData = new TransferEventData;
	pEventData->pCreateDBCmdClass = pCreateDBCmd;
	pEventData->pCompareDBResult = pCompareDBResult;
	pEventData->pCompareProcCounter = pCompareProcCounter;

	pDBStructTransfer->SqlCmd(0, _CompareDBSchemaEvent, _CompareDBSchemaEventResult, pEventData, NULL);
	//BlockProLoadDBProc(pDBStructTransfer, *(Get_IsLoadOK()), "CompareDBSchemaProc");
	char cCompareMsg[128];
	sprintf(cCompareMsg, "CompareDBSchemaProc Table: %s ", pEventData->pCreateDBCmdClass->GetTableName().c_str());
	BlockProLoadDBProc(pDBStructTransfer, (CompareDBSchema::_IsLoadOK), cCompareMsg);
}

//--------------------------------------------------------------------------------------------------------
void CompareDBSchema::BlockProLoadDBProc( DBStructTransferClass* db , bool& checkFlag , char* msg )
{
	checkFlag = false; 
	printf( "\n");

	for( int i = 0 ; i < 2000 ; i++ )
	{
		Sleep( 1000 );
		db->Process( );
		printf( "\r%s" , msg );
		switch( i % 4 )
		{
		case 0:
			printf( "..... /        " );
			break;
		case 1:
			printf( "..... |        " );
			break;
		case 2:			
			printf( "..... \\        " );
			break;
		case 3:
			printf( "..... -        " );
			break;
		}

		if( checkFlag )
			break;
	}
	printf( "\n");
}

//--------------------------------------------------------------------------------------------------------
bool CompareDBSchema::_CompareDBSchemaEvent( SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	char	Buf[8192];
	char	sTmp[128];
	TransferEventData* pEventData = (TransferEventData*)UserObj;

	map<string,PointInfoStruct>&  ReadInfo = *( ((pEventData->pCreateDBCmdClass)->GetReader())->ReadInfo() );
	map<string,PointInfoStruct>::iterator Iter;

	string sColumnData = "";
	DBResultData DBResultTmp;
	DBResultTmp.bResult = true;
	memset(DBResultTmp.sResultMsg, '\0', sizeof(DBResultTmp.sResultMsg));

	int i = 0;

	// 取得Reader內欄位資訊
	for(Iter = ReadInfo.begin() ; Iter != ReadInfo.end() ; Iter++, i++)
	{
		PointInfoStruct *IDData = &(Iter->second);

		if(i > 0)
		{
			sColumnData += "|";
		}

		sprintf( sTmp , "%s,%d^%d" , IDData->ValueName.c_str(), (int)(IDData->Type), IDData->Size );
		sColumnData += sTmp;
	}

	MyDbSqlExecClass	MyDBProc( sqlBase );

	sprintf( Buf , "EXEC NSP_Server_S_CompareTableSchema @Table = '%s', @ColumnData = '%s'" , (pEventData->pCreateDBCmdClass)->GetTableName().c_str(), sColumnData.c_str() );
	MyDBProc.SqlCmd( Buf );

	//Print( Def_PrintLV2 , "SqlCmdPrint" , Buf );

	MyDBProc.Bind( 1, SQL_C_BIT, SQL_C_DEFAULT , (LPBYTE)&DBResultTmp.bResult );
	MyDBProc.Bind( 2, SQL_C_CHAR, sizeof(DBResultTmp.sResultMsg) , (LPBYTE)&DBResultTmp.sResultMsg );

	if( MyDBProc.Read() == false )
	{
		DBResultTmp.bResult = false;
		sprintf(DBResultTmp.sResultMsg, "NO DATA");
	}

	MyDBProc.Close();

	//char sResult[3072];
	//sprintf(sResult,"Table = %s, bResult = %d, sResultMsg = %s", (pEventData->pCreateDBCmdClass)->GetTableName().c_str(), (int)DBResultTmp.bResult, DBResultTmp.sResultMsg);
	//Print( Def_PrintLV6 , "SqlCmdResult" , sResult );

	// 取得結果，若有異常記錄至LOG
	if(DBResultTmp.bResult == false)
	{
		//Print( Def_PrintLV6 , "Error IN" , "===== Error IN =====" );

		// 拆解字串，加入換行符號，替換異常原因
		string sStrTmp(DBResultTmp.sResultMsg);
		string::size_type pos = 0;
		string sErrorDesc = "";
		int iErrorType = 0;

		// 替換異常原因
		// @ResultMsg中的段落分隔符號:
		//	'~': DB欄位名稱
		//	'!': DB欄位型態
		//	'$': SERVER欄位名稱
		//	';': SERVER欄位型態
		//	'^': 比對結果
		//	'|': 下筆結果分行符號

		//	'~': DB欄位名稱
		pos = 0;
		while( (pos = sStrTmp.find_first_of('~',pos)) != string::npos )
		{
			sErrorDesc = ", DB_Column_Name:";
			sStrTmp.replace(pos,1,sErrorDesc);
			sErrorDesc = "";
		}

		//	'!': DB欄位型態
		pos = 0;
		while( (pos = sStrTmp.find_first_of('!',pos)) != string::npos )
		{
			sErrorDesc = ", DB_Column_Type:";
			sStrTmp.replace(pos,1,sErrorDesc);
			sErrorDesc = "";
		}

		//	'$': SERVER欄位名稱
		pos = 0;
		while( (pos = sStrTmp.find_first_of('$',pos)) != string::npos )
		{
			sErrorDesc = ", SERVER_Column_Name:";
			sStrTmp.replace(pos,1,sErrorDesc);
			sErrorDesc = "";
		}

		//	';': SERVER欄位型態
		pos = 0;
		while( (pos = sStrTmp.find_first_of(';',pos)) != string::npos )
		{
			sErrorDesc = ", SERVER_Column_Type:";
			sStrTmp.replace(pos,1,sErrorDesc);
			sErrorDesc = "";
		}

		//	'^': 比對結果
		pos = 0;
		while( (pos = sStrTmp.find_first_of('^',pos)) != string::npos )
		{
			sStrTmp[pos] = ',';
			iErrorType = atoi(&(sStrTmp[pos+1]));
			switch(iErrorType)
			{
			case ENUM_DBCompareError_Column:
				sErrorDesc = "\n\t\tResult:Some column DB does not have";
				break;
			case ENUM_DBCompareError_Type:
				sErrorDesc = "\n\t\tResult:The column Type is different";
				break;
			case ENUM_DBCompareError_Size:
				sErrorDesc = "\n\t\tResult:Server column size bigger than DB column";
				break;
			}
			sStrTmp.replace(pos+1,1,sErrorDesc);
			sErrorDesc = "";
		}

		// 加入換行符號
		pos = 0;
		while( (pos = sStrTmp.find_first_of('|',pos)) != string::npos )
		{
			sStrTmp[pos] = '\n';
		}

		strcpy(DBResultTmp.sResultMsg, sStrTmp.c_str());
		*(pEventData->pCompareDBResult) = false;

		//Print( Def_PrintLV6 , "CompareResultError" , sStrTmp.c_str() );

		Print( Def_PrintLV2 , "_CompareDBSchemaEvent" , DBResultTmp.sResultMsg );
		PrintToController( false , "CriticalMsg {dc390dc9-c91d-46b6-80c4-a2ef3261994f},CompareDBSchemaEvent:%s\n" , DBResultTmp.sResultMsg );
	}

	(*(pEventData->pCompareProcCounter))--;

	return true;
}

//--------------------------------------------------------------------------------------------------------
void CompareDBSchema::_CompareDBSchemaEventResult( void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	//*(Get_IsLoadOK()) = true;
	(CompareDBSchema::_IsLoadOK) = true;
}