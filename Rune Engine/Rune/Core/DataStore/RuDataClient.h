#ifndef _RUDATACLIENT_
#define _RUDATACLIENT_

// Base types
#include <winsock2.h>
#include "../../Core/DataStore/RuDataStore_Base.h"
#include "../../Core/Thread/RuThread_CriticalSection.h"

// Library includes
#pragma comment(lib, "ws2_32.lib")

#pragma unmanaged

// ************************************************************************************************************************************************************

enum RuDataClient_CommandCode
{
	ruDATACLIENTCMD_CREATEDATASTORE				= 1,
	ruDATACLIENTCMD_OPENDATASTORE				= 2,
	ruDATACLIENTCMD_DELETEDATASTORE				= 3,
	ruDATACLIENTCMD_ENUMERATEDATASTORES			= 4,

	ruDATACLIENTCMD_CREATETABLE					= 5,
	ruDATACLIENTCMD_DELETETABLE					= 6,
	ruDATACLIENTCMD_GETTABLEHANDLE				= 7,
	ruDATACLIENTCMD_GETTABLEKEYLENGTH			= 8,
	ruDATACLIENTCMD_GETNUMTABLES				= 16,
	ruDATACLIENTCMD_GETTABLENAMEBYINDEX			= 17,
	ruDATACLIENTCMD_GETTABLEKEYLENGTHBYINDEX	= 18,

	ruDATACLIENTCMD_GETNUMRECORDS				= 9,
	ruDATACLIENTCMD_RETRIEVE					= 10,
	ruDATACLIENTCMD_RETRIEVEBYINDEX				= 11,
	ruDATACLIENTCMD_RETRIEVEKEYBYINDEX			= 14,
	ruDATACLIENTCMD_STORE						= 12,
	ruDATACLIENTCMD_DELETE						= 13,
	ruDATACLIENTCMD_DELETEALL					= 15
};

// ************************************************************************************************************************************************************

class CRuDataClient : public CRuDataStore_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	SOCKET										m_socket;

	CRuArrayList<INT32>							m_tableKeyLengths;

	BYTE										m_tempBufferSpace[2048];

	// Workspace
	INT32										m_ioBufferSize;
	BYTE*										m_ioBuffer;
	CRuMemoryStream								m_packetStream;

	CRuThread_CriticalSection					m_cs;

public:
												CRuDataClient();
	virtual										~CRuDataClient();

	BOOL										Connect(const char *serverName, INT32 port);

	BOOL										CreateDataStore(const char *dataStoreName);
	BOOL										OpenDataStore(const char *dataStoreName);
	BOOL										DeleteDataStore(const char *dataStoreName);

	BOOL										EnumerateDataStores(CRuArrayList<char *> &dataStoresOut);

	virtual INT32								CreateTable(const char *tableName, INT32 keyLength);
	virtual BOOL								DeleteTable(const char *tableName);
	virtual INT32								GetTableHandle(const char *tableName);
	virtual INT32								GetTableKeyLength(INT32 tableHandle);
	virtual INT32								GetNumTables();
	virtual const char*							GetTableNameByIndex(INT32 index);
	virtual INT32								GetTableKeyLengthByIndex(INT32 index);

	virtual INT32								GetNumRecords(INT32 tableHandle);
	virtual IRuStream*							Retrieve(INT32 tableHandle, const BYTE *key);
	virtual IRuStream*							RetrieveByIndex(INT32 tableHandle, INT32 index);
	virtual BOOL								RetrieveKeyByIndex(INT32 tableHandle, INT32 index, BYTE *keyOut);
	virtual BOOL								Store(INT32 tableHandle, const BYTE *key, IRuStream *stream);
	virtual BOOL								Delete(INT32 tableHandle, const BYTE *key);
	virtual BOOL								DeleteAllRecords(INT32 tableHandle);

protected:
	BOOL										ReceivePacket(CRuMemoryStream *packetStream);

	BOOL										GetReturnPacket_Integer(INT32 *returnValueOut);
	IRuStream*									GetReturnPacket_Stream();
	BOOL										GetReturnPacket_ByteStream(BYTE *byteStreamOut);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
