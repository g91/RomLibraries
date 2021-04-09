/*!
	@project Rune
	@file RuSQL_Base.cpp

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/07/13
*/

#include "../SQL/RuSQL_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuSQLSession, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuSQLSession, "CRuSQLSession", "IRuObject")
ruCLASSGUID_DEFINE(CRuSQLSession, 0x3CDA62BB, 0x16194048, 0x9B74357F, 0x18742C81)

// ************************************************************************************************************************************************************

/*!
	Generates a lighting vector according to the specified theta and phi values.
	An undisturbed lighting vector is one that points along the negative y axis.
	Theta determines the longitudinal angle, and phi determines the latitudinal
	angle.

	@param theta Angle along XZ plane.
	@param phi Angle along XY plane.

	@return A vector value denoting the light direction.
*/

CRuSQLSession::CRuSQLSession()
:	m_sqlEnvironmentHandle(SQL_NULL_HENV),
	m_sqlConnectionHandle(SQL_NULL_HDBC)
{
	// Allocate environment handle
	if(!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_sqlEnvironmentHandle)))
	{
		OutputDebugString("[Rune-SQL] Failed to allocate SQL environment handle.\n");
	}

	// Set ODBC version
	if(!SQL_SUCCEEDED(SQLSetEnvAttr(m_sqlEnvironmentHandle, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0)))
	{
		OutputDebugString("[Rune-SQL] Failed to set ODBC version.");
	}

	// Allocate connection handle
	if(!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_DBC, m_sqlEnvironmentHandle, &m_sqlConnectionHandle)))
	{
		SQLGetDiagRec(SQL_HANDLE_ENV, m_sqlEnvironmentHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);
		OutputDebugString("[Rune-SQL] Failed to allocate SQL connection handle.\n");
	}
}

CRuSQLSession::~CRuSQLSession()
{
	// Disconnect from driver
	Disconnect();

	// Free connection handle
	if(m_sqlConnectionHandle != SQL_NULL_HDBC)
	{
		if(!SQL_SUCCEEDED(SQLFreeHandle(SQL_HANDLE_DBC, m_sqlConnectionHandle)))
		{
			SQLGetDiagRec(SQL_HANDLE_ENV, m_sqlEnvironmentHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);
			OutputDebugString("[Rune-SQL] Failed to free SQL connection handle.\n");
		}
	}

	// Free environment handle
	if(m_sqlEnvironmentHandle != SQL_NULL_HENV)
	{
		if(!SQL_SUCCEEDED(SQLFreeHandle(SQL_HANDLE_ENV, m_sqlEnvironmentHandle)))
		{
			OutputDebugString("[Rune-SQL] Failed to free SQL environment handle.\n");
		}
	}
}

BOOL CRuSQLSession::Connect(const char *serverName, const char *databaseName)
{
	BOOL connectOK = TRUE;

	// Build connection string
	char *connectionString = ruNEW char [strlen(serverName) + strlen(databaseName) + 256];
	sprintf(connectionString, "DRIVER={SQL Server};SERVER=%s;DATABASE=%s;Trusted_Connection=yes", serverName, databaseName);

	// Connect to driver
	INT32 connectionStringLength = (INT32)strlen(connectionString);
	SQLCHAR *completedConnectionString = ruNEW SQLCHAR [2048];
	SQLSMALLINT completedConnectionStringLength;

	if(!SQL_SUCCEEDED(SQLDriverConnect(m_sqlConnectionHandle, NULL, (SQLCHAR *) connectionString, connectionStringLength, completedConnectionString, 2048, &completedConnectionStringLength, SQL_DRIVER_NOPROMPT)))
	{
		connectOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_DBC, m_sqlConnectionHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);
		OutputDebugString("[Rune-SQL] Failed to connect to database.\n");
	}

	// Allocate statement handle
	if(!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, m_sqlConnectionHandle, &m_sqlStatementHandle)))
	{
		SQLGetDiagRec(SQL_HANDLE_DBC, m_sqlConnectionHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);
		OutputDebugString("[Rune-SQL] Failed to allocate SQL statement handle.\n");
	}

	// Free memory
	delete[] connectionString;
	delete[] completedConnectionString;

	return connectOK;
}

BOOL CRuSQLSession::Disconnect()
{
	BOOL disconnectOK = TRUE;

	// Free statement handle
	if(m_sqlStatementHandle != SQL_NULL_HSTMT)
	{
		if(!SQL_SUCCEEDED(SQLFreeHandle(SQL_HANDLE_STMT, m_sqlStatementHandle)))
		{
			disconnectOK = FALSE;

			OutputDebugString("[Rune-SQL] Failed to free SQL statement handle.\n");
		}
	}

	// Disconnect from driver
	if(!SQL_SUCCEEDED(SQLDisconnect(m_sqlConnectionHandle)))
	{
		disconnectOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_DBC, m_sqlConnectionHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);
		OutputDebugString("[Rune-SQL] Failed to disconnect from database.\n");
	}

	return disconnectOK;
}

BOOL CRuSQLSession::EnableAutoCommit(BOOL enable)
{
	BOOL changeOK = TRUE;
	SQLRETURN retVal;
	
	if(enable)
	{
		retVal = SQLSetConnectAttr(m_sqlConnectionHandle, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_ON, 0);
	}
	else
	{
		retVal = SQLSetConnectAttr(m_sqlConnectionHandle, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_OFF, 0);
	}

	if(!SQL_SUCCEEDED(retVal))
	{
		changeOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_DBC, m_sqlConnectionHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);
		OutputDebugString("[Rune-SQL] Failed to change commit mode.\n");
	}

	return changeOK;
}

BOOL CRuSQLSession::ResetStatement()
{
	BOOL resetOK = TRUE;

	if(!SQL_SUCCEEDED(SQLFreeStmt(m_sqlStatementHandle, SQL_CLOSE)) || !SQL_SUCCEEDED(SQLFreeStmt(m_sqlStatementHandle, SQL_UNBIND)) || !SQL_SUCCEEDED(SQLFreeStmt(m_sqlStatementHandle, SQL_RESET_PARAMS)))
	{
		resetOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_STMT, m_sqlStatementHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);
		OutputDebugString("[Rune-SQL] Failed to reset statement.\n");
	}

	return resetOK;
}

BOOL CRuSQLSession::Execute()
{
	BOOL executeOK = TRUE;

	SQLRETURN retVal = SQLExecute(m_sqlStatementHandle);

	if(!SQL_SUCCEEDED(retVal) && retVal != SQL_NO_DATA)
	{
		executeOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_STMT, m_sqlStatementHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);

		char barf[2000];
		sprintf(barf, "%c%c%c%c%c / %s\n", m_errorState_SQLState[0], m_errorState_SQLState[1], m_errorState_SQLState[2], m_errorState_SQLState[3], m_errorState_SQLState[4], m_errorState_SQLMessage);
		OutputDebugString(barf);

		OutputDebugString("[Rune-SQL] Failed to execute statement.\n");
	}

	return executeOK;
}

BOOL CRuSQLSession::Execute(SQLCHAR *statementStr)
{
	BOOL executeOK = TRUE;

	SQLRETURN retVal = SQLExecDirect(m_sqlStatementHandle, statementStr, SQL_NTS);

	if(!SQL_SUCCEEDED(retVal) && retVal != SQL_NO_DATA)
	{
		executeOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_STMT, m_sqlStatementHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);

		char barf[2000];
		sprintf(barf, "%c%c%c%c%c / %s\n", m_errorState_SQLState[0], m_errorState_SQLState[1], m_errorState_SQLState[2], m_errorState_SQLState[3], m_errorState_SQLState[4], m_errorState_SQLMessage);
		OutputDebugString(barf);

		OutputDebugString("[Rune-SQL] Failed to execute statement.\n");
	}

	return executeOK;
}

BOOL CRuSQLSession::Prepare(SQLCHAR *statementStr)
{
	BOOL prepareOK = TRUE;

	if(!SQL_SUCCEEDED(SQLPrepare(m_sqlStatementHandle, statementStr, SQL_NTS)))
	{
		prepareOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_STMT, m_sqlStatementHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);

		char barf[2000];
		sprintf(barf, "%c%c%c%c%c / %s\n", m_errorState_SQLState[0], m_errorState_SQLState[1], m_errorState_SQLState[2], m_errorState_SQLState[3], m_errorState_SQLState[4], m_errorState_SQLMessage);
		OutputDebugString(barf);

		OutputDebugString("[Rune-SQL] Failed to execute statement.\n");
	}

	return prepareOK;
}

BOOL CRuSQLSession::Commit()
{
	BOOL commitOK = TRUE;

	if(!SQL_SUCCEEDED(SQLEndTran(SQL_HANDLE_DBC, m_sqlConnectionHandle, SQL_COMMIT)))
	{
		commitOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_DBC, m_sqlConnectionHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);
		OutputDebugString("[Rune-SQL] Failed to commit transaction.\n");
	}

	return commitOK;
}

BOOL CRuSQLSession::Rollback()
{
	BOOL rollbackOK = TRUE;

	if(!SQL_SUCCEEDED(SQLEndTran(SQL_HANDLE_DBC, m_sqlConnectionHandle, SQL_ROLLBACK)))
	{
		rollbackOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_DBC, m_sqlConnectionHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);
		OutputDebugString("[Rune-SQL] Failed to rollback transaction.\n");
	}

	return rollbackOK;
}

BOOL CRuSQLSession::GetParamData(SQLPOINTER *paramPtr)
{
	BOOL getParamDataOK = TRUE;

	if(!SQL_SUCCEEDED(SQLParamData(m_sqlStatementHandle, paramPtr)))
	{
		getParamDataOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_DBC, m_sqlConnectionHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);
		OutputDebugString("[Rune-SQL] Failed to rollback transaction.\n");
	}

	return getParamDataOK;
}

BOOL CRuSQLSession::BindParameterArray(SQLINTEGER arraySize, SQLUSMALLINT *paramStatusArray, SQLUSMALLINT *paramsProcessed)
{
	// Set the SQL_ATTR_PARAM_BIND_TYPE statement attribute to use column-wise binding.
	SQLSetStmtAttr(m_sqlStatementHandle, SQL_ATTR_PARAM_BIND_TYPE, SQL_PARAM_BIND_BY_COLUMN, 0);

	// Specify the number of elements in each parameter array.
	SQLSetStmtAttr(m_sqlStatementHandle, SQL_ATTR_PARAMSET_SIZE, (SQLPOINTER) arraySize, 0);

	// Specify an array in which to return the status of each set of parameters.
	SQLSetStmtAttr(m_sqlStatementHandle, SQL_ATTR_PARAM_STATUS_PTR, paramStatusArray, 0);

	// Specify an SQLUINTEGER value in which to return the number of sets of parameters processed.
	SQLSetStmtAttr(m_sqlStatementHandle, SQL_ATTR_PARAMS_PROCESSED_PTR, paramsProcessed, 0);

	return TRUE;
}

BOOL CRuSQLSession::BindParameter_VarImage(SQLUSMALLINT paramNumber, BYTE *dataPtr, SQLLEN *indicatorPtr)
{
	BOOL bindOK = TRUE;

	SQLRETURN retVal = SQLBindParameter(	
											m_sqlStatementHandle,
											paramNumber,
											SQL_PARAM_INPUT,
											SQL_C_BINARY,
											SQL_LONGVARBINARY,
											*indicatorPtr,
											0,
											reinterpret_cast<SQLPOINTER *>(dataPtr),
											*indicatorPtr,
											indicatorPtr
										);

	if(!SQL_SUCCEEDED(retVal))
	{
		bindOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_STMT, m_sqlStatementHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);

		char barf[2000];
		sprintf(barf, "%c%c%c%c%c / %s\n", m_errorState_SQLState[0], m_errorState_SQLState[1], m_errorState_SQLState[2], m_errorState_SQLState[3], m_errorState_SQLState[4], m_errorState_SQLMessage);
		OutputDebugString(barf);

		OutputDebugString("[Rune-SQL] Failed to bind parameter.\n");
	}

	return bindOK;
}

BOOL CRuSQLSession::BindParameter_VarString(SQLUSMALLINT paramNumber, char *paramStr, SQLLEN bufferLength)
{
	BOOL bindOK = TRUE;

	INT32 paramStrLength = (INT32)strlen(paramStr) + 1;

	if(bufferLength > 0)
		paramStrLength = (INT32)bufferLength;

	SQLRETURN retVal = SQLBindParameter(	
											m_sqlStatementHandle,
											paramNumber,
											SQL_PARAM_INPUT,
											SQL_C_CHAR,
											SQL_VARCHAR,
											paramStrLength - 1,
											0,
											reinterpret_cast<SQLPOINTER *>(paramStr),
											paramStrLength,
											NULL
										);

	if(!SQL_SUCCEEDED(retVal))
	{
		bindOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_STMT, m_sqlStatementHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);

		char barf[2000];
		sprintf(barf, "%c%c%c%c%c / %s\n", m_errorState_SQLState[0], m_errorState_SQLState[1], m_errorState_SQLState[2], m_errorState_SQLState[3], m_errorState_SQLState[4], m_errorState_SQLMessage);
		OutputDebugString(barf);

		OutputDebugString("[Rune-SQL] Failed to bind parameter.\n");
	}

	return bindOK;
}

BOOL CRuSQLSession::BindParameter_Float(SQLUSMALLINT paramNumber, SQLREAL *floatPtr)
{
	BOOL bindOK = TRUE;

	SQLRETURN retVal = SQLBindParameter(	
											m_sqlStatementHandle,
											paramNumber,
											SQL_PARAM_INPUT,
											SQL_C_FLOAT,
											SQL_REAL,
											24,
											0,
											reinterpret_cast<SQLPOINTER *>(floatPtr),
											0,
											NULL
										);

	if(!SQL_SUCCEEDED(retVal))
	{
		bindOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_STMT, m_sqlStatementHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);
		OutputDebugString("[Rune-SQL] Failed to bind parameter.\n");
	}

	return bindOK;
}

BOOL CRuSQLSession::BindParameter_Integer(SQLUSMALLINT paramNumber, SQLINTEGER *integerPtr)
{
	BOOL bindOK = TRUE;

	SQLRETURN retVal = SQLBindParameter(	
											m_sqlStatementHandle,
											paramNumber,
											SQL_PARAM_INPUT,
											SQL_C_SLONG,
											SQL_INTEGER,
											24,
											0,
											reinterpret_cast<SQLPOINTER *>(integerPtr),
											0,
											NULL
										);

	if(!SQL_SUCCEEDED(retVal))
	{
		bindOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_STMT, m_sqlStatementHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);
		OutputDebugString("[Rune-SQL] Failed to bind parameter.\n");
	}

	return bindOK;
}

BOOL CRuSQLSession::BindColumn_VarString(SQLUSMALLINT colNumber, char *stringPtr, SQLLEN bufferLength, SQLLEN *indicatorPtr)
{
	BOOL bindOK = TRUE;

	if(!SQL_SUCCEEDED(SQLBindCol(m_sqlStatementHandle, colNumber, SQL_C_CHAR, stringPtr, bufferLength, indicatorPtr)))
	{
		bindOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_STMT, m_sqlStatementHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);
		OutputDebugString("[Rune-SQL] Failed to bind column.\n");
	}

	return bindOK;
}

BOOL CRuSQLSession::BindColumn_Float(SQLUSMALLINT colNumber, SQLREAL *floatPtr, SQLLEN *indicatorPtr)
{
	BOOL bindOK = TRUE;

	if(!SQL_SUCCEEDED(SQLBindCol(m_sqlStatementHandle, colNumber, SQL_C_FLOAT, floatPtr, 0, indicatorPtr)))
	{
		bindOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_STMT, m_sqlStatementHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);
		OutputDebugString("[Rune-SQL] Failed to bind column.\n");
	}

	return bindOK;
}

BOOL CRuSQLSession::BindColumn_Integer(SQLUSMALLINT colNumber, SQLINTEGER *integerPtr, SQLLEN *indicatorPtr)
{
	BOOL bindOK = TRUE;

	if(!SQL_SUCCEEDED(SQLBindCol(m_sqlStatementHandle, colNumber, SQL_C_SLONG, integerPtr, 0, indicatorPtr)))
	{
		bindOK = FALSE;

		SQLGetDiagRec(SQL_HANDLE_STMT, m_sqlStatementHandle, 1, m_errorState_SQLState, &m_errorState_NativeErrorPtr, m_errorState_SQLMessage, 2048, &m_errorState_SQLMessageLength);
		OutputDebugString("[Rune-SQL] Failed to bind column.\n");
	}

	return bindOK;
}

BOOL CRuSQLSession::Fetch()
{
	return (SQLFetch(m_sqlStatementHandle) != SQL_NO_DATA);
}

// ************************************************************************************************************************************************************

#pragma managed
