#ifndef _RUDATASTORE_BASE_
#define _RUDATASTORE_BASE_

// Base types
#include "../../Core/Type/RuType_Object.h"
#include "../../Core/Stream/RuStreamIO.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuDataStore_Base;

// ************************************************************************************************************************************************************

class CRuDataStore_CopyToState
{
protected:
	CRuDataStore_Base*							m_dstDataStore;

	INT32										m_keyBufferSize;
	BYTE*										m_keyBuffer;

	INT32										m_numTables;
	INT32										m_curTableIdx;
	INT32										m_tableKeyLength;
	INT32										m_srcTableHandle;
	INT32										m_dstTableHandle;

	INT32										m_numEntries;
	INT32										m_curEntryIdx;

	BOOL										m_done;

	REAL										m_copyProgress;

public:
												CRuDataStore_CopyToState();
												~CRuDataStore_CopyToState();

	BOOL										IsCopyDone();
	REAL										GetCopyProgress();

	friend CRuDataStore_Base;
};

// ************************************************************************************************************************************************************

#ifdef KALYDO

// A callback for the successful download of a chunk of data.
typedef void(DataDownloadCallback)(const BYTE*, void*);

#endif // KALYDO

class CRuDataStore_Base : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual										~CRuDataStore_Base();

	virtual INT32								CreateTable(const char *tableName, INT32 keyLength) = 0;
	virtual BOOL								DeleteTable(const char *tableName) = 0;
	virtual INT32								GetTableHandle(const char *tableName) = 0;
	virtual INT32								GetTableKeyLength(INT32 tableHandle) = 0;
	virtual INT32								GetNumTables() = 0;
	virtual const char*							GetTableNameByIndex(INT32 index) = 0;
	virtual INT32								GetTableKeyLengthByIndex(INT32 index) = 0;

	virtual INT32								GetNumRecords(INT32 tableHandle) = 0;
	virtual IRuStream*							Retrieve(INT32 tableHandle, const BYTE *key) = 0;
	virtual IRuStream*							RetrieveByIndex(INT32 tableHandle, INT32 index) = 0;
	virtual BOOL								RetrieveKeyByIndex(INT32 tableHandle, INT32 index, BYTE *keyOut) = 0;
	virtual BOOL								Store(INT32 tableHandle, const BYTE *key, IRuStream *stream) = 0;
	virtual BOOL								Delete(INT32 tableHandle, const BYTE *key) = 0;
	virtual BOOL								DeleteAllRecords(INT32 tableHandle) = 0;

	BOOL										CopyTo(CRuDataStore_Base *dstDataStore);
	BOOL										CopyTo(CRuDataStore_Base *dstDataStore, const char *copiedTableName);
	CRuDataStore_CopyToState*					CopyTo_Begin(CRuDataStore_Base *dstDataStore);
	BOOL										CopyTo_Step(CRuDataStore_CopyToState *copyToState);
	BOOL										CopyTo_End(CRuDataStore_CopyToState *copyToState);

#ifdef KALYDO
	// Function for downloading parts of data 
	virtual BOOL								KalydoDownloadData(INT32 table, const BYTE* key, void* userData, DataDownloadCallback* callback) { return FALSE; }

#endif // KALYDO
protected:
	void										CopyTo_UpdateCurrentTableInfo(CRuDataStore_CopyToState *copyToState);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
