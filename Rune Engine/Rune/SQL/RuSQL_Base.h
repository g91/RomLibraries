/*!
	@project Rune
	@file RuSQL_Base.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/08/03
*/

#ifndef _RUSQL_BASE_H_
#define _RUSQL_BASE_H_

#include "../Core/Type/RuType_Object.h"

#include <sql.h>
#include <sqlext.h>

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuSQLSession : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	SQLHENV										m_sqlEnvironmentHandle;
	SQLHDBC										m_sqlConnectionHandle;
	SQLHSTMT									m_sqlStatementHandle;

	// SQL error state information
	SQLCHAR										m_errorState_SQLState[5];
	SQLINTEGER									m_errorState_NativeErrorPtr;
	SQLCHAR										m_errorState_SQLMessage[2048];
	SQLSMALLINT									m_errorState_SQLMessageLength;

public:
												CRuSQLSession();
	virtual										~CRuSQLSession();

	// Connection management
	BOOL										Connect(const char *serverName, const char *databaseName);
	BOOL										Disconnect();
	BOOL										EnableAutoCommit(BOOL enable);

	// Statement management
	BOOL										ResetStatement();
	BOOL										Execute();
	BOOL										Execute(SQLCHAR *statementStr);
	BOOL										Prepare(SQLCHAR *statementStr);
	BOOL										Commit();
	BOOL										Rollback();

	// Data putback
	BOOL										GetParamData(SQLPOINTER *paramPtr);

	// Parameter binding
	BOOL										BindParameterArray(SQLINTEGER arraySize, SQLUSMALLINT *paramStatusArray, SQLUSMALLINT *paramsProcessed);
	BOOL										BindParameter_VarImage(SQLUSMALLINT paramNumber, BYTE *dataPtr, SQLLEN *indicatorPtr);
	BOOL										BindParameter_VarString(SQLUSMALLINT paramNumber, char *paramStr, SQLLEN bufferLength = SQL_NTS);
	BOOL										BindParameter_Float(SQLUSMALLINT paramNumber, SQLREAL *floatPtr);
	BOOL										BindParameter_Integer(SQLUSMALLINT paramNumber, SQLINTEGER *integerPtr);

	// Column binding
	BOOL										BindColumn_VarString(SQLUSMALLINT colNumber, char *stringPtr, SQLLEN bufferLength, SQLLEN *indicatorPtr);
	BOOL										BindColumn_Float(SQLUSMALLINT colNumber, SQLREAL *floatPtr, SQLLEN *indicatorPtr);
	BOOL										BindColumn_Integer(SQLUSMALLINT colNumber, SQLINTEGER *integerPtr, SQLLEN *indicatorPtr);

	// Resultset
	BOOL										Fetch();
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
