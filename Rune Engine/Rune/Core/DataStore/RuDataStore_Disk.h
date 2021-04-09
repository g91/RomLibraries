#ifndef _RUDATASTORE_DISK_
#define _RUDATASTORE_DISK_

// Base types
#include "../../Core/Collection/RuCollectionsTree.h"
#include "../../Core/DataStore/RuDataStore_Base.h"
#include "../../Core/Stream/RuStreamIO.h"
#include "../../Core/Thread/RuThread_CriticalSection.h"
#ifdef KALYDO
// for downloading DLC
#include "CompactIntegration/Include/KRFReadLib.h"
// for the reporting of non-downloaded parts
#include <map>

// A download state, which states whether an object or texture is being downloaded.
enum DownloadState
{
	NO,				// not yet downloaded
	PENDING,		// pending for download
	PENDING_QUEUE,	// pending for download and queue
	YES,			// downloaded
	FAILED			// could not download
};
#endif // KALYDO
#pragma unmanaged

// ************************************************************************************************************************************************************

class RuDataStoreEvent_Retrieved : public RuEventArgs
{
public:
	char*							m_table;											//!< Table
	const BYTE*						m_key;												//!< Key, or NULL for invalid key
	INT32							m_keyIndex;											//!< Key index, or -1 for invalid index
	IRuStream*						m_stream;											//!< Stream being retrieved

	RuDataStoreEvent_Retrieved(char *table, const BYTE *key, INT32 keyIndex, IRuStream *stream)
	:	m_table(table),
		m_key(key),
		m_keyIndex(keyIndex),
		m_stream(stream)
	{
	}
};

// ************************************************************************************************************************************************************

class CRuDataStore_Disk : public CRuDataStore_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct DataStoreHeader
	{
		INT32									m_tableListPosition;
		INT32									m_tableListSize;
		INT32									m_tableListAllocationSize;
	};

	struct KeyStruct
	{
		const BYTE*								m_key;										//!< Stream key
		INT32									m_keyLength;

		BOOL									operator==(const KeyStruct &key) const;
		BOOL									operator<(const KeyStruct &key) const;
		BOOL									operator>(const KeyStruct &key) const;
	};

	struct StreamHeader
	{
		BYTE*									m_key;										//!< Stream key
		INT32									m_streamPosition;							//!< Stream position
		INT32									m_streamSize;								//!< Used stream size
		INT32									m_allocationSize;							//!< Allocation stream size
	};

	struct TableHeader
	{
		char									m_name[256];								//!< Name of the table
		INT32									m_keyLength;								//!< Length of keys
		INT32									m_tablePosition;							//!< Position of first stream descriptor
		INT32									m_tableSize;								//!< Number of used stream descriptors
		INT32									m_allocationSize;							//!< Number of allocated stream descriptors

		CRuBinaryTree<KeyStruct, StreamHeader>	m_streamHeaders;

		BOOL									m_streamHeaderIndexedCacheDirty;
		CRuArrayList<StreamHeader *>			m_streamHeaderIndexedCache;
	};

	// Events
	CRuEvent									m_event_Retrieved;

	// Settings
	INT32										m_ioBufferSize;
	INT32										m_tableListPageSize;
	INT32										m_tablePageSize;
	BOOL										m_disablePagePadding;

	// Data source
	BOOL										m_readOnly;
	CRuFileStream*								m_stream;

	// Run-time structure
	DataStoreHeader								m_dataStoreHeader;
	CRuArrayList<TableHeader *>					m_tableHeaders;

	// Workspace
	INT32										m_callbackKeyLength;
	BYTE*										m_ioBuffer;
	CRuMemoryStream								m_tempStream;

	INT32										m_temp_ActiveTableHeader;
	INT32										m_temp_Counter;

	CRuDataStore_Disk*							m_journalTarget;

	// Threading
	CRuThread_CriticalSection*					m_objectCS;									//!< Object-level critical section

public:
												CRuDataStore_Disk();
	virtual										~CRuDataStore_Disk();

	BOOL										Create(const char *dataStoreName);
#ifdef KALYDO
	// make "Open" virtual so we can overload it with our multi-file version.
	virtual BOOL								Open(const char *dataStoreName, BOOL readOnly = FALSE);
#else
	BOOL										Open(const char *dataStoreName, BOOL readOnly = FALSE);
#endif // KALYDO

	CRuEvent&									Event_Retrieved();

	void										BeginJournal(CRuDataStore_Disk *srcStore);
	void										EndJournal();

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

	void										DisablePagePadding(BOOL disablePagePadding);
	BOOL										Flush();

protected:
	void										UpdateTableStreamHeaderCache(INT32 tableHandle);
	BOOL										UpdateTableStreamHeaderCacheCallback(const void *key, void *data);

	BOOL										DeleteStreamHeadersCallback(const void *key, void *data);

	BOOL										LoadDataStoreHeader(DataStoreHeader *headerOut);
	BOOL										SaveDataStoreHeader(DataStoreHeader *header);

	BOOL										LoadTableHeaders(DataStoreHeader &dsHeader, CRuArrayList<TableHeader *> &tableHeaders);
	BOOL										SaveTableHeaders(DataStoreHeader &dsHeader, CRuArrayList<TableHeader *> &tableHeaders);

	BOOL										LoadStreamHeaders(CRuArrayList<TableHeader *> &tableHeaders, INT32 tableHandle);
	BOOL										SaveStreamHeaders(CRuArrayList<TableHeader *> &tableHeaders, INT32 tableHandle);
	BOOL										SaveStreamHeadersCallback(const void *key, void *data);

	BOOL										RetrievedHandler(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

#ifdef KALYDO

class CRuDataStore_MultiFile;

// a struct for the handling of downloading chunks.
struct ChunkDownloadStruct
{
	const BYTE* key;
	void* userData;
	DWORD fileNum;
	DataDownloadCallback* callback;
	INT32 count;	
	CRuDataStore_MultiFile*	multiFile;
};

// typedef for download callback

typedef void (CMultiFileCallback)(CRuDataStore_MultiFile*);
// A class for handling multi file datastores.
class CRuDataStore_MultiFile : public CRuDataStore_Disk
{
public:
												CRuDataStore_MultiFile();
	virtual										~CRuDataStore_MultiFile();

	virtual BOOL								Open(const char *dataStoreName, BOOL readOnly = FALSE);
	BOOL										FilesContainingChunk(INT32 tableHandle, const BYTE *key, DWORD& startFile, DWORD& endFile);
	std::string									GetFileName(DWORD file);

	// download a part of the data.
	BOOL										KalydoDownloadData(INT32 table, const BYTE* key, void* userData, DataDownloadCallback* callback);
	BOOL										KalydoDownloadFiles(ChunkDownloadStruct* cds, DWORD start, DWORD end);

	// callback for the download of data.
	static void									ChunkDownloadCallback(const char* fileName, TKResult result, void* id);

	virtual IRuStream*							Retrieve(INT32 tableHandle, const BYTE *key);
	virtual IRuStream*							RetrieveByIndex(INT32 tableHandle, INT32 index);

	int											GetFilesPending() { return m_downloadStructs.size(); }
	void										SetDownloadCallback(CMultiFileCallback* function);
protected:
	std::vector<ChunkDownloadStruct*>			m_downloadStructs;
	std::map<DWORD, DownloadState>				m_downloadedFiles;

	CMultiFileCallback*							m_callback;

	BOOL										LoadDataStoreHeader(DataStoreHeader *headerOut);
	BOOL										LoadTableHeaders(DataStoreHeader &dsHeader, CRuArrayList<TableHeader *> &tableHeaders);
	BOOL										LoadStreamHeaders(CRuArrayList<TableHeader *> &tableHeaders, INT32 tableHandle);

	BOOL										PartAvailable(INT32 startPos, INT32 size);

};
#endif // KALYDO
#pragma managed

#endif
