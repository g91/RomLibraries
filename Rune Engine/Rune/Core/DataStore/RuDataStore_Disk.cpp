#include "../../Core/DataStore/RuDataStore_Disk.h"
#ifdef KALYDO
// include exception so we can raise the Kalydo Error Reporter.
#include "SocialIntegration/Exception.h"
#include "../../Engine/Manager/RuEngine_ResourceManager.h"
#endif // KALYDO
#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuDataStore_Disk, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuDataStore_Disk, "CRuDataStore_Disk", "CRuDataStore_Base")
ruCLASSGUID_DEFINE(CRuDataStore_Disk, 0x319CB9B4, 0x4CB94600, 0xA36D2022, 0xD059DF1D)

// ************************************************************************************************************************************************************

BOOL CRuDataStore_Disk::KeyStruct::operator==(const KeyStruct &key) const
{
	return (memcmp(m_key, key.m_key, m_keyLength) == 0);
}

BOOL CRuDataStore_Disk::KeyStruct::operator<(const KeyStruct &key) const
{
	return (memcmp(m_key, key.m_key, m_keyLength) < 0);
}

BOOL CRuDataStore_Disk::KeyStruct::operator>(const KeyStruct &key) const
{
	return (memcmp(m_key, key.m_key, m_keyLength) > 0);
}

// ************************************************************************************************************************************************************

CRuDataStore_Disk::CRuDataStore_Disk()
:	m_ioBufferSize(4096),
	m_tableListPageSize(32),
	m_tablePageSize(64),
	m_disablePagePadding(FALSE),
	m_stream(NULL),
	m_ioBuffer(NULL),
	m_journalTarget(NULL)
{
	m_ioBuffer = ruNEW BYTE [m_ioBufferSize];

	// Create object-level critical section
	m_objectCS = ruNEW CRuThread_CriticalSection();
}

CRuDataStore_Disk::~CRuDataStore_Disk()
{
	// End journal
	EndJournal();

	// Flush any changes to disk
	Flush();

	for(INT32 i = 0; i < m_tableHeaders.Count(); ++i)
	{
		// Iterate over all stream headers and delete each
		m_tableHeaders[i]->m_streamHeaders.ForAllElements(boost::bind(&CRuDataStore_Disk::DeleteStreamHeadersCallback, this, _1, _2));

		// Clear binary tree
		m_tableHeaders[i]->m_streamHeaders.Clear();

		// Delete the table header
		delete m_tableHeaders[i];
	}

	// Free resources
	delete m_stream;
	delete[] m_ioBuffer;

	// Destroy object-level critical section
	delete m_objectCS;
}

BOOL CRuDataStore_Disk::Create(const char *dataStoreName)
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(m_objectCS);

	m_readOnly = FALSE;

	// Create data store stream
	m_stream = ruNEW CRuFileStream();
	if(m_stream->Create(dataStoreName, FALSE) == ruSEC_OK)
	{

		m_dataStoreHeader.m_tableListPosition = 0;
		m_dataStoreHeader.m_tableListSize = 0;
		m_dataStoreHeader.m_tableListAllocationSize = 0;

		// Reserve space for data store header
		SaveDataStoreHeader(&m_dataStoreHeader);

		// Create initial table list
		SaveTableHeaders(m_dataStoreHeader, m_tableHeaders);

		// Update data store header
		SaveDataStoreHeader(&m_dataStoreHeader);

		return TRUE;
	}
	else
	{
		ruSAFE_DELETE(m_stream);
		return FALSE;
	}
}

BOOL CRuDataStore_Disk::Open(const char *dataStoreName, BOOL readOnly)
{
	BOOL retVal = FALSE;

	// Acquire critical section
	m_objectCS->Enter();

	m_readOnly = readOnly;

	RuStreamOpenMethod openMethod = ruSOM_OpenReadWrite;
	if(m_readOnly)
	{
		openMethod = ruSOM_OpenRead;
	}

	// Open data store stream
	m_stream = ruNEW CRuFileStream();

	if(m_stream->Open(dataStoreName, openMethod) == ruSEC_OK)
	{
		// Load data store header
		LoadDataStoreHeader(&m_dataStoreHeader);

		// Load table list
		LoadTableHeaders(m_dataStoreHeader, m_tableHeaders);

		// Load stream lists
		for(INT32 i = 0; i < m_tableHeaders.Count(); ++i)
		{
			LoadStreamHeaders(m_tableHeaders, i);

			// Mark the header cache as dirty
			m_tableHeaders[i]->m_streamHeaderIndexedCacheDirty = TRUE;
		}

		retVal = TRUE;
	}
	else
	{
		delete m_stream;
		m_stream = NULL;
	}

	// Release critical section
	m_objectCS->Leave();

	return retVal;
}

CRuEvent &CRuDataStore_Disk::Event_Retrieved()
{
	return m_event_Retrieved;
}

void CRuDataStore_Disk::BeginJournal(CRuDataStore_Disk *srcStore)
{
	if(m_journalTarget)
	{
		EndJournal();
	}

	// Store reference to target and register event handler
	m_journalTarget = srcStore;
	m_journalTarget->Event_Retrieved().RegisterHandler(this->GetGUID(), boost::bind(&CRuDataStore_Disk::RetrievedHandler, this, _1));

	// Copy table structure
	INT32 numTables = m_journalTarget->GetNumTables();
	for(INT32 i = 0; i < numTables; ++i)
	{
		// Get source table properties
		const char *srcTableName = m_journalTarget->GetTableNameByIndex(i);
		INT32 srcTableKeyLength = m_journalTarget->GetTableKeyLengthByIndex(i);

		// Create the table
		this->CreateTable(srcTableName, srcTableKeyLength);
	}
}

void CRuDataStore_Disk::EndJournal()
{
	if(m_journalTarget)
	{
		// Unregister retrieved handler from target
		m_journalTarget->Event_Retrieved().UnregisterHandler(this->GetGUID());

		// Clear target
		m_journalTarget = NULL;
	}
}

INT32 CRuDataStore_Disk::CreateTable(const char *tableName, INT32 keyLength)
{
	INT32 retVal = -2;

	// Acquire critical section
	m_objectCS->Enter();

	// Check whether the specified table name is already in the list
	for(INT32 i = 0; i < m_tableHeaders.Count(); ++i)
	{
		if(strcmpi(tableName, m_tableHeaders[i]->m_name) == 0)
		{
			// If the table already exists with the correct key length, return its handle
			if(m_tableHeaders[i]->m_keyLength == keyLength)
			{
				retVal = i;
				break;
			}
			else
			{
				// Otherwise, return an error
				retVal = -2;
				break;
			}
		}
	}

	if(retVal < 0)
	{
		if(m_readOnly)
		{
			retVal = -2;
		}
		else
		{
			// If the table is to be created, append it to the table list
			if(retVal < 0)
			{
				INT32 tableHandle = m_tableHeaders.Count();
				m_tableHeaders.Add(ruNEW TableHeader());

				memset(m_tableHeaders[tableHandle]->m_name, 0, sizeof(char) * 256);
				strcpy(m_tableHeaders[tableHandle]->m_name, tableName);
				m_tableHeaders[tableHandle]->m_keyLength = keyLength;
				m_tableHeaders[tableHandle]->m_tablePosition = 0;
				m_tableHeaders[tableHandle]->m_tableSize = 0;
				m_tableHeaders[tableHandle]->m_allocationSize = 0;
				m_tableHeaders[tableHandle]->m_streamHeaderIndexedCacheDirty = TRUE;

				++m_dataStoreHeader.m_tableListSize;

				// Write table list
				SaveTableHeaders(m_dataStoreHeader, m_tableHeaders);

				// Reserve space for data store header
				SaveDataStoreHeader(&m_dataStoreHeader);

				retVal = tableHandle;
			}
		}
	}

	// Release critical section
	m_objectCS->Leave();

	// Return table handle
	return retVal;
}

BOOL CRuDataStore_Disk::DeleteTable(const char *tableName)
{
	BOOL retVal = FALSE;

	// Acquire critical section
	m_objectCS->Enter();

	if(m_readOnly == FALSE)
	{
		// Find the table to be deleted
		for(INT32 i = 0; i < m_tableHeaders.Count(); ++i)
		{
			if(strcmpi(tableName, m_tableHeaders[i]->m_name) == 0)
			{
				// Delete table structure
				delete m_tableHeaders[i];
				m_tableHeaders.RemoveAt(i);

				--m_dataStoreHeader.m_tableListSize;

				// Write table list
				SaveTableHeaders(m_dataStoreHeader, m_tableHeaders);

				// Save data store header
				SaveDataStoreHeader(&m_dataStoreHeader);

				retVal = TRUE;
				break;
			}
		}
	}

	// Release critical section
	m_objectCS->Leave();

	return retVal;
}

INT32 CRuDataStore_Disk::GetTableHandle(const char *tableName)
{
	INT32 retVal = -1;

	// Acquire critical section
	m_objectCS->Enter();

	for(INT32 i = 0; i < m_tableHeaders.Count(); ++i)
	{
		if(strcmpi(tableName, m_tableHeaders[i]->m_name) == 0)
		{
			retVal = i;
			break;
		}
	}

	// Release critical section
	m_objectCS->Leave();

	return retVal;
}

INT32 CRuDataStore_Disk::GetNumTables()
{
	return m_tableHeaders.Count();
}

const char *CRuDataStore_Disk::GetTableNameByIndex(INT32 index)
{
	if(index >= 0 && index < m_tableHeaders.Count())
	{
		return m_tableHeaders[index]->m_name;
	}

	return NULL;
}

INT32 CRuDataStore_Disk::GetTableKeyLengthByIndex(INT32 index)
{
	if(index >= 0 && index < m_tableHeaders.Count())
	{
		return m_tableHeaders[index]->m_keyLength;
	}

	return 0;
}

INT32 CRuDataStore_Disk::GetTableKeyLength(INT32 tableHandle)
{
	// Acquire critical section
	m_objectCS->Enter();

	// Ensure validity of handle
	assert(tableHandle >= 0 && tableHandle < m_tableHeaders.Count());
	INT32 retVal = m_tableHeaders[tableHandle]->m_keyLength;

	// Release critical section
	m_objectCS->Leave();

	return retVal;
}

INT32 CRuDataStore_Disk::GetNumRecords(INT32 tableHandle)
{
	// Acquire critical section
	m_objectCS->Enter();

	// Ensure validity of handle
	assert(tableHandle >= 0 && tableHandle < m_tableHeaders.Count());
	INT32 retVal = m_tableHeaders[tableHandle]->m_streamHeaders.Count();

	// Release critical section
	m_objectCS->Leave();

	return retVal;
}

IRuStream *CRuDataStore_Disk::Retrieve(INT32 tableHandle, const BYTE *key)
{
	IRuStream *retVal = NULL;

	// Acquire critical section
	m_objectCS->Enter();

	// Ensure validity of handle
	assert(tableHandle >= 0 && tableHandle < m_tableHeaders.Count());

	// Build a key structure
	KeyStruct keyStruct;
	keyStruct.m_key = key;
	keyStruct.m_keyLength = m_tableHeaders[tableHandle]->m_keyLength;

	if(m_tableHeaders[tableHandle]->m_streamHeaders.ContainsKey(keyStruct))
	{
		// Locate the stream header record
		StreamHeader &header = m_tableHeaders[tableHandle]->m_streamHeaders.Get(keyStruct);

		// Seek to position of the stream
		m_stream->Seek(header.m_streamPosition, ruSSM_Begin);

		// Read stream into memory
		CRuMemoryStream *memStream;
		memStream = ruNEW CRuMemoryStream();
		memStream->SetBufferSize(header.m_streamSize);
		memStream->SetStreamSize(header.m_streamSize);
		m_stream->Read(memStream->GetBuffer(), header.m_streamSize);

		retVal = memStream;
	}

	// Release critical section
	m_objectCS->Leave();

	// Trigger retrieved event
	RuDataStoreEvent_Retrieved retrievedArgs(m_tableHeaders[tableHandle]->m_name, key, -1, retVal);
	m_event_Retrieved.Trigger(&retrievedArgs);

	return retVal;
}

IRuStream *CRuDataStore_Disk::RetrieveByIndex(INT32 tableHandle, INT32 index)
{
	IRuStream *retVal = NULL;

	// Acquire critical section
	m_objectCS->Enter();

	// Ensure validity of handle
	assert(tableHandle >= 0 && tableHandle < m_tableHeaders.Count());

	if(index >= 0 && index < m_tableHeaders[tableHandle]->m_streamHeaders.Count())
	{
/*
		// Locate the stream header record
		StreamHeader &header = m_tableHeaders[tableHandle]->m_streamHeaders.GetAtIndex(index);
*/
		// Update the stream header cache
		UpdateTableStreamHeaderCache(tableHandle);

		// Locate the stream header record
		StreamHeader &header = *(m_tableHeaders[tableHandle]->m_streamHeaderIndexedCache[index]);

		// Seek to position of the stream
		m_stream->Seek(header.m_streamPosition, ruSSM_Begin);

		if(header.m_streamSize > 1024 * 1024)
		{
			printf("\nWARNING: Ignored abnormally large stream chunk at %d, size %d\n", index, header.m_streamSize);

			retVal = NULL;
		}
		else if(header.m_streamSize < 0)
		{
			printf("\nWARNING: Ignored negative-sized stream chunk at %d, size %d\n", index, header.m_streamSize);

			retVal = NULL;
		}
		else
		{
			// Read stream into memory
			CRuMemoryStream *memStream;
			memStream = ruNEW CRuMemoryStream();
			memStream->SetBufferSize(header.m_streamSize);
			memStream->SetStreamSize(header.m_streamSize);
			m_stream->Read(memStream->GetBuffer(), header.m_streamSize);

			retVal = memStream;
		}
	}

	// Release critical section
	m_objectCS->Leave();

	// Trigger retrieved event
	RuDataStoreEvent_Retrieved retrievedArgs(m_tableHeaders[tableHandle]->m_name, NULL, index, retVal);
	m_event_Retrieved.Trigger(&retrievedArgs);

	return retVal;
}

BOOL CRuDataStore_Disk::RetrieveKeyByIndex(INT32 tableHandle, INT32 index, BYTE *keyOut)
{
	BOOL retVal = FALSE;

	// Acquire critical section
	m_objectCS->Enter();

	// Ensure validity of handle
	assert(tableHandle >= 0 && tableHandle < m_tableHeaders.Count());

	if(index >= 0 && index < m_tableHeaders[tableHandle]->m_streamHeaders.Count())
	{
/*
		// Locate the stream header record
		StreamHeader &header = m_tableHeaders[tableHandle]->m_streamHeaders.GetAtIndex(index);
*/
		// Update the stream header cache
		UpdateTableStreamHeaderCache(tableHandle);

		// Locate the stream header record
		StreamHeader &header = *(m_tableHeaders[tableHandle]->m_streamHeaderIndexedCache[index]);

		// Copy key to output
		memcpy(keyOut, header.m_key, m_tableHeaders[tableHandle]->m_keyLength);

		retVal = TRUE;
	}

	// Release critical section
	m_objectCS->Leave();

	return retVal;
}

BOOL CRuDataStore_Disk::Store(INT32 tableHandle, const BYTE *key, IRuStream *stream)
{
	BOOL retVal = FALSE;
	StreamHeader streamHeader;

	// Acquire critical section
	m_objectCS->Enter();

	// Ensure validity of handle
	assert(tableHandle >= 0 && tableHandle < m_tableHeaders.Count());

	if(m_readOnly == FALSE)
	{
		// Build a key structure
		KeyStruct keyStruct;
		keyStruct.m_key = key;
		keyStruct.m_keyLength = m_tableHeaders[tableHandle]->m_keyLength;

		if(m_tableHeaders[tableHandle]->m_streamHeaders.ContainsKey(keyStruct))
		{
			// Locate the stream header record
			streamHeader = m_tableHeaders[tableHandle]->m_streamHeaders.Get(keyStruct);

			// Update stream size
			streamHeader.m_streamSize = stream->GetStreamSize();

			if(streamHeader.m_allocationSize < streamHeader.m_streamSize)
			{
				m_stream->Seek(0, ruSSM_End);
				streamHeader.m_streamPosition = m_stream->GetPosition();
				streamHeader.m_allocationSize = 0;
			}

			// Update key struct with the correct key pointer
			keyStruct.m_key = streamHeader.m_key;
		}
		else
		{
			// Mark the header cache as dirty
			m_tableHeaders[tableHandle]->m_streamHeaderIndexedCacheDirty = TRUE;

			// Set up stream header
			streamHeader.m_key = ruNEW BYTE [m_tableHeaders[tableHandle]->m_keyLength];
			memcpy(streamHeader.m_key, key, m_tableHeaders[tableHandle]->m_keyLength);

			m_stream->Seek(0, ruSSM_End);
			streamHeader.m_streamPosition = m_stream->GetPosition();
			streamHeader.m_allocationSize = 0;

			// Update stream size
			streamHeader.m_streamSize = stream->GetStreamSize();

			// Update key struct with the correct key pointer
			keyStruct.m_key = streamHeader.m_key;

			// Append stream header to table
			++m_tableHeaders[tableHandle]->m_tableSize;
			m_tableHeaders[tableHandle]->m_streamHeaders.Add(keyStruct, streamHeader);
		}

		if(streamHeader.m_allocationSize == 0)
		{
			if(m_disablePagePadding)
			{
				// Stream allocation size is same as stream size
				streamHeader.m_allocationSize = streamHeader.m_streamSize;
			}
			else
			{
				// Stream allocation size is a multiple of m_ioBufferSize
				streamHeader.m_allocationSize = ((streamHeader.m_streamSize / m_ioBufferSize) * m_ioBufferSize) + ((streamHeader.m_streamSize % m_ioBufferSize) ? m_ioBufferSize : 0);
			}
		}

		// Fill allocated area with 0xCD
		m_stream->Seek(streamHeader.m_streamPosition, ruSSM_Begin);
		INT32 bytesToFill = streamHeader.m_allocationSize;
		while(bytesToFill > 0)
		{
			INT32 bytesThisIteration = bytesToFill > m_ioBufferSize ? m_ioBufferSize : bytesToFill;

			memset(m_ioBuffer, 0xCD, m_ioBufferSize);
			m_stream->Write(m_ioBuffer, bytesThisIteration);

			bytesToFill -= bytesThisIteration;
		}

		// Output stream data
		stream->Seek(0, ruSSM_Begin);
		m_stream->Seek(streamHeader.m_streamPosition, ruSSM_Begin);
		do
		{
			memset(m_ioBuffer, 0, m_ioBufferSize);
			INT32 bytesRead = stream->Read(m_ioBuffer, m_ioBufferSize);

			if(bytesRead > 0)
			{
				// Write input buffer
				m_stream->Write(m_ioBuffer, bytesRead);
			}
			else
			{
				break;
			}
		} while(TRUE);

		// Update stream header
		m_tableHeaders[tableHandle]->m_streamHeaders.Set(keyStruct, streamHeader);

		// Update stream list
	//	SaveStreamHeaders(m_tableHeaders, tableHandle);

		// Update table list
	//	SaveTableHeaders(m_dataStoreHeader, m_tableHeaders);

		// Update data store header
	//	SaveDataStoreHeader(&m_dataStoreHeader);

		retVal = TRUE;
	}

	// Release critical section
	m_objectCS->Leave();

	return retVal;
}

BOOL CRuDataStore_Disk::Delete(INT32 tableHandle, const BYTE *key)
{
	BOOL retVal = FALSE;

	// Acquire critical section
	m_objectCS->Enter();

	// Ensure validity of handle
	assert(tableHandle >= 0 && tableHandle < m_tableHeaders.Count());

	if(m_readOnly == FALSE)
	{
		// Build a key structure
		KeyStruct keyStruct;
		keyStruct.m_key = key;
		keyStruct.m_keyLength = m_tableHeaders[tableHandle]->m_keyLength;

		if(m_tableHeaders[tableHandle]->m_streamHeaders.ContainsKey(keyStruct))
		{
			// Mark the header cache as dirty
			m_tableHeaders[tableHandle]->m_streamHeaderIndexedCacheDirty = TRUE;

			// Remove the stream header record
			--m_tableHeaders[tableHandle]->m_tableSize;
			m_tableHeaders[tableHandle]->m_streamHeaders.Remove(keyStruct);

			// Update stream list
	//		SaveStreamHeaders(m_tableHeaders, tableHandle);

			// Update table list
	//		SaveTableHeaders(m_dataStoreHeader, m_tableHeaders);

			// Update data store header
	//		SaveDataStoreHeader(&m_dataStoreHeader);

			retVal = TRUE;
		}
	}

	// Release critical section
	m_objectCS->Leave();

	return retVal;
}

BOOL CRuDataStore_Disk::DeleteAllRecords(INT32 tableHandle)
{
	BOOL retVal = FALSE;

	// Acquire critical section
	m_objectCS->Enter();

	// Ensure validity of handle
	assert(tableHandle >= 0 && tableHandle < m_tableHeaders.Count());

	if(m_readOnly == FALSE)
	{
		// Clear table
		m_tableHeaders[tableHandle]->m_tableSize = 0;
		m_tableHeaders[tableHandle]->m_streamHeaders.Clear();
		m_tableHeaders[tableHandle]->m_streamHeaderIndexedCacheDirty = TRUE;
		m_tableHeaders[tableHandle]->m_streamHeaderIndexedCache.Clear();

		retVal = TRUE;
	}

	// Release critical section
	m_objectCS->Leave();

	return retVal;
}

void CRuDataStore_Disk::DisablePagePadding(BOOL disablePagePadding)
{
	m_disablePagePadding = disablePagePadding;
}

BOOL CRuDataStore_Disk::Flush()
{
	BOOL retVal = FALSE;

	// Acquire critical section
	m_objectCS->Enter();

	if(m_stream)
	{
		if(m_readOnly == FALSE)
		{
			for(INT32 i = 0; i < m_tableHeaders.Count(); ++i)
			{
				// Update stream list
				SaveStreamHeaders(m_tableHeaders, i);
			}

			// Update table list
			SaveTableHeaders(m_dataStoreHeader, m_tableHeaders);

			// Update data store header
			SaveDataStoreHeader(&m_dataStoreHeader);

			retVal = TRUE;
		}
	}

	// Release critical section
	m_objectCS->Leave();

	return retVal;
}

void CRuDataStore_Disk::UpdateTableStreamHeaderCache(INT32 tableHandle)
{
	// Acquire critical section
	m_objectCS->Enter();

	if(m_tableHeaders[tableHandle]->m_streamHeaderIndexedCacheDirty)
	{
		// Reset temps
		m_temp_ActiveTableHeader = tableHandle;
		m_temp_Counter = 0;

		// Set the header cache size
		m_tableHeaders[tableHandle]->m_streamHeaderIndexedCache.SetNumEntries(m_tableHeaders[tableHandle]->m_streamHeaders.Count());

		// Fill out the header cache
		m_tableHeaders[tableHandle]->m_streamHeaders.ForAllElements(boost::bind(&CRuDataStore_Disk::UpdateTableStreamHeaderCacheCallback, this, _1, _2));

		// Mark the header cache as not dirty
		m_tableHeaders[tableHandle]->m_streamHeaderIndexedCacheDirty = FALSE;
	}

	// Release critical section
	m_objectCS->Leave();
}

BOOL CRuDataStore_Disk::UpdateTableStreamHeaderCacheCallback(const void *key, void *data)
{
	StreamHeader *header = reinterpret_cast<StreamHeader *>(data);

	m_tableHeaders[m_temp_ActiveTableHeader]->m_streamHeaderIndexedCache[m_temp_Counter] = header;

	++m_temp_Counter;

	return TRUE;
}

BOOL CRuDataStore_Disk::DeleteStreamHeadersCallback(const void *key, void *data)
{
	BYTE *headerKey = *((BYTE **) key);

	delete[] headerKey;

	return TRUE;
}

BOOL CRuDataStore_Disk::LoadDataStoreHeader(DataStoreHeader *headerOut)
{
	// Seek to beginning of data store stream
	m_stream->Seek(0, ruSSM_Begin);

	// Read & confirm GUID correctness
	CRuGUID classGUID;
	if(RuReadGUID(m_stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read data store header position
	m_stream->Read(&headerOut->m_tableListPosition, sizeof(INT32));
	m_stream->Read(&headerOut->m_tableListSize, sizeof(INT32));
	m_stream->Read(&headerOut->m_tableListAllocationSize, sizeof(INT32));

	return TRUE;
}

BOOL CRuDataStore_Disk::SaveDataStoreHeader(DataStoreHeader *header)
{
	// Seek to beginning of data store stream
	m_stream->Seek(0, ruSSM_Begin);

	// Write & confirm GUID correctness
	m_stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write data store header position
	m_stream->Write(&header->m_tableListPosition, sizeof(INT32));
	m_stream->Write(&header->m_tableListSize, sizeof(INT32));
	m_stream->Write(&header->m_tableListAllocationSize, sizeof(INT32));

	return TRUE;
}

BOOL CRuDataStore_Disk::LoadTableHeaders(DataStoreHeader &dsHeader, CRuArrayList<TableHeader *> &tableHeaders)
{
	// Seek to beginning of table headers
	m_stream->Seek(dsHeader.m_tableListPosition, ruSSM_Begin);

	// Clear table headers array
	tableHeaders.Clear();

	for(INT32 i = 0; i < dsHeader.m_tableListSize; ++i)
	{
		// Append blank header to table list
		tableHeaders.Add(ruNEW TableHeader());

		// Read table properties
		m_stream->Read(tableHeaders[i]->m_name, sizeof(char) * 256);
		m_stream->Read(&tableHeaders[i]->m_keyLength, sizeof(INT32));
		m_stream->Read(&tableHeaders[i]->m_tablePosition, sizeof(INT32));
		m_stream->Read(&tableHeaders[i]->m_tableSize, sizeof(INT32));
		m_stream->Read(&tableHeaders[i]->m_allocationSize, sizeof(INT32));
	}

	return TRUE;
}

BOOL CRuDataStore_Disk::SaveTableHeaders(DataStoreHeader &dsHeader, CRuArrayList<TableHeader *> &tableHeaders)
{
	// If the current allocation size is too small, relocate the table list
	if(dsHeader.m_tableListAllocationSize < dsHeader.m_tableListSize || dsHeader.m_tableListAllocationSize == 0)
	{
		// Set table list position to the end of the stream
		dsHeader.m_tableListPosition = m_stream->GetStreamSize();

		// Round allocation size up to the nearest multiple of m_tableListPageSize
		while(dsHeader.m_tableListAllocationSize < dsHeader.m_tableListSize || dsHeader.m_tableListAllocationSize == 0)
		{
			dsHeader.m_tableListAllocationSize += m_tableListPageSize;
		}
	}

	// Seek to beginning of table headers
	m_stream->Seek(dsHeader.m_tableListPosition, ruSSM_Begin);

	// Write blank entries for the entire table list allocation section
	TableHeader dummyHeader;
	memset(dummyHeader.m_name, 0, sizeof(char) * 256);
	dummyHeader.m_keyLength = 0;
	dummyHeader.m_tablePosition = 0;
	dummyHeader.m_tableSize = 0;
	dummyHeader.m_allocationSize = 0;

	for(INT32 i = 0; i < dsHeader.m_tableListAllocationSize; ++i)
	{
		m_stream->Write(dummyHeader.m_name, sizeof(char) * 256);
		m_stream->Write(&dummyHeader.m_keyLength, sizeof(INT32));
		m_stream->Write(&dummyHeader.m_tablePosition, sizeof(INT32));
		m_stream->Write(&dummyHeader.m_tableSize, sizeof(INT32));
		m_stream->Write(&dummyHeader.m_allocationSize, sizeof(INT32));
	}

	// Seek to beginning of table headers
	m_stream->Seek(dsHeader.m_tableListPosition, ruSSM_Begin);

	// Write occupied table headers
	for(INT32 i = 0; i < tableHeaders.Count(); ++i)
	{
		// Write table properties
		m_stream->Write(tableHeaders[i]->m_name, sizeof(char) * 256);
		m_stream->Write(&tableHeaders[i]->m_keyLength, sizeof(INT32));
		m_stream->Write(&tableHeaders[i]->m_tablePosition, sizeof(INT32));
		m_stream->Write(&tableHeaders[i]->m_tableSize, sizeof(INT32));
		m_stream->Write(&tableHeaders[i]->m_allocationSize, sizeof(INT32));
	}

	return TRUE;
}

BOOL CRuDataStore_Disk::LoadStreamHeaders(CRuArrayList<TableHeader *> &tableHeaders, INT32 tableHandle)
{
	// Seek to beginning of stream headers
	m_stream->Seek(tableHeaders[tableHandle]->m_tablePosition, ruSSM_Begin);

	for(INT32 i = 0; i < tableHeaders[tableHandle]->m_tableSize; ++i)
	{
		// Read stream header
		StreamHeader header;
		header.m_key = ruNEW BYTE [tableHeaders[tableHandle]->m_keyLength];
		m_stream->Read(header.m_key, tableHeaders[tableHandle]->m_keyLength);
		m_stream->Read(&header.m_streamPosition, sizeof(INT32));
		m_stream->Read(&header.m_streamSize, sizeof(INT32));
		m_stream->Read(&header.m_allocationSize, sizeof(INT32));

		// Build a key structure
		KeyStruct keyStruct;
		keyStruct.m_key = header.m_key;
		keyStruct.m_keyLength = tableHeaders[tableHandle]->m_keyLength;

		tableHeaders[tableHandle]->m_streamHeaders.Add(keyStruct, header);
	}

	// Balance the stream header tree
	tableHeaders[tableHandle]->m_streamHeaders.BalanceTree();

	return TRUE;
}

BOOL CRuDataStore_Disk::SaveStreamHeaders(CRuArrayList<TableHeader *> &tableHeaders, INT32 tableHandle)
{
	// If the current allocation size is too small, relocate the table
	if(tableHeaders[tableHandle]->m_allocationSize < tableHeaders[tableHandle]->m_tableSize)
	{
		// Set table list position to the end of the stream
		tableHeaders[tableHandle]->m_tablePosition = m_stream->GetStreamSize();

		// Round allocation size up to the nearest multiple of m_tablePageSize
		while(tableHeaders[tableHandle]->m_allocationSize < tableHeaders[tableHandle]->m_tableSize)
		{
			tableHeaders[tableHandle]->m_allocationSize += m_tablePageSize;
		}
	}

	// Reserve space for the entire table allocation section
	INT32 streamHeaderSize = sizeof(INT32) * 3 + tableHeaders[tableHandle]->m_keyLength;
	INT32 allocatedBlockSize = streamHeaderSize * tableHeaders[tableHandle]->m_allocationSize;

	// Store temporaries for the save stream headers callback
	m_callbackKeyLength = tableHeaders[tableHandle]->m_keyLength;

	m_tempStream.Seek(0, ruSSM_Begin);
	m_tempStream.SetStreamSize(allocatedBlockSize);

	// Balance the stream header tree
	tableHeaders[tableHandle]->m_streamHeaders.BalanceTree();

	// Iterate over all stream headers
	tableHeaders[tableHandle]->m_streamHeaders.ForAllElements(boost::bind(&CRuDataStore_Disk::SaveStreamHeadersCallback, this, _1, _2));

	// Seek to beginning of stream headers
	m_stream->Seek(tableHeaders[tableHandle]->m_tablePosition, ruSSM_Begin);

	// Write entire stream header block
	m_stream->Write(m_tempStream.GetBuffer(), m_tempStream.GetStreamSize());

	return TRUE;
}

BOOL CRuDataStore_Disk::SaveStreamHeadersCallback(const void *key, void *data)
{
	StreamHeader *header = (StreamHeader *) data;

	m_tempStream.Write(header->m_key, m_callbackKeyLength);
	m_tempStream.Write(&header->m_streamPosition, sizeof(INT32));
	m_tempStream.Write(&header->m_streamSize, sizeof(INT32));
	m_tempStream.Write(&header->m_allocationSize, sizeof(INT32));

	return TRUE;
}

BOOL CRuDataStore_Disk::RetrievedHandler(RuEventArgs *eventArgs)
{
	RuDataStoreEvent_Retrieved *retrievedArgs = static_cast<RuDataStoreEvent_Retrieved *>(eventArgs);

	if(retrievedArgs->m_stream)
	{
		// Get table handle
		INT32 tableHandle = this->GetTableHandle(retrievedArgs->m_table);

		if(retrievedArgs->m_key)
		{
			// Store record
			this->Store(tableHandle, retrievedArgs->m_key, retrievedArgs->m_stream);
		}
		else
		{
			// If there is no key, try to get it
			BYTE key[2048];
			m_journalTarget->RetrieveKeyByIndex(tableHandle, retrievedArgs->m_keyIndex, key);

			// Store record
			this->Store(tableHandle, key, retrievedArgs->m_stream);
		}

		// Restore stream position
		retrievedArgs->m_stream->Seek(0, ruSSM_Begin);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************
#ifdef KALYDO

CRuDataStore_MultiFile::CRuDataStore_MultiFile()
{
	m_callback = NULL;
}

CRuDataStore_MultiFile::~CRuDataStore_MultiFile()
{
	// make sure we do not get a callback after this object is gone
	for(std::vector<ChunkDownloadStruct*>::iterator i = m_downloadStructs.begin(); i != m_downloadStructs.end(); i++)
	{
		(*i)->multiFile = NULL;
	}
}

BOOL CRuDataStore_MultiFile::Open(const char *dataStoreName, BOOL readOnly)
{
	BOOL retVal = FALSE;

	// Acquire critical section
	m_objectCS->Enter();

	m_readOnly = readOnly;

	RuStreamOpenMethod openMethod = ruSOM_OpenReadWrite;
	if(m_readOnly)
	{
		openMethod = ruSOM_OpenRead;
	}

	// Open data store stream
	m_stream = ruNEW CRuMultiFileStream();

	if(m_stream->Open(dataStoreName, openMethod) == ruSEC_OK)
	{
		retVal = TRUE;

		// Load data store header
		if (LoadDataStoreHeader(&m_dataStoreHeader) == FALSE)
			retVal = FALSE;

		// Load table list
		if (retVal && LoadTableHeaders(m_dataStoreHeader, m_tableHeaders) == FALSE)
			retVal = FALSE;

		// Load stream lists
		for(INT32 i = 0; retVal && i < m_tableHeaders.Count(); ++i)
		{
			if (LoadStreamHeaders(m_tableHeaders, i) == FALSE)
				retVal = FALSE;

			// Mark the header cache as dirty
			m_tableHeaders[i]->m_streamHeaderIndexedCacheDirty = TRUE;
		}
	}
	else
	{
		delete m_stream;
		m_stream = NULL;
	}

	// Release critical section
	m_objectCS->Leave();

	return retVal;
}

BOOL CRuDataStore_MultiFile::FilesContainingChunk(INT32 tableHandle, const BYTE *key, DWORD& startFile, DWORD& endFile)
{
	BOOL retVal = FALSE;

	// Acquire critical section
	m_objectCS->Enter();

	// Ensure validity of handle
	assert(tableHandle >= 0 && tableHandle < m_tableHeaders.Count());

	// Build a key structure
	KeyStruct keyStruct;
	keyStruct.m_key = key;
	keyStruct.m_keyLength = m_tableHeaders[tableHandle]->m_keyLength;

	if(m_tableHeaders[tableHandle]->m_streamHeaders.ContainsKey(keyStruct))
	{
		// Locate the stream header record
		StreamHeader &header = m_tableHeaders[tableHandle]->m_streamHeaders.Get(keyStruct);

		CRuMultiFileStream* mfs = static_cast<CRuMultiFileStream*>(m_stream);

		retVal = mfs->GetFilesContaining(header.m_streamPosition, header.m_streamSize, startFile, endFile);
	}

	// Release critical section
	m_objectCS->Leave();

	return retVal;
}

std::string CRuDataStore_MultiFile::GetFileName(DWORD file)
{
	return static_cast<CRuMultiFileStream*>(m_stream)->GetFileName(file);
}

BOOL CRuDataStore_MultiFile::KalydoDownloadData(INT32 table, const BYTE* key, void* userData, DataDownloadCallback* callback)
{
	DWORD start;
	DWORD end;
	if (FilesContainingChunk(table, key, start, end) == TRUE)
	{
		ChunkDownloadStruct* cds = new ChunkDownloadStruct;
		cds->key = key;
		cds->userData = userData;
		cds->callback = callback;
	
		return KalydoDownloadFiles(cds, start, end);
	}

	return FALSE;
}

BOOL CRuDataStore_MultiFile::KalydoDownloadFiles(ChunkDownloadStruct* cds, DWORD start, DWORD end)
{
	if (cds == NULL)
	{
		DownloadState maxDownloadState = YES;
		for (DWORD i = start; i <= end; ++i)
		{
			switch (m_downloadedFiles[i])
			{
			case YES:
				break;
			case NO:
				maxDownloadState = NO;
				break;
			case PENDING:
				if (maxDownloadState == YES)
					maxDownloadState = PENDING;
			}
		}

		if (maxDownloadState != NO)
		{
			// all is coming ...
			return TRUE;
		}
		else
		{
			cds = new ChunkDownloadStruct;
			memset(cds, 0, sizeof(ChunkDownloadStruct));
		}
	}
	
	cds->multiFile = this;

	// count the number of files that need to be downloaded
	cds->count = 0;
	for (DWORD i = start; i <= end; ++i)
		if (m_downloadedFiles[i] != YES)
			++cds->count;

	if (cds->count == 0)
	{
		// nothing needs to be downloaded; just call all callbacks.
		if (cds->callback)
			cds->callback(cds->key, cds->userData);

		if (m_callback && m_downloadStructs.empty())
			m_callback(this);

		delete cds;
		return TRUE;
	}
	TKResult res = KR_OK;
	m_downloadStructs.push_back(cds);
	for (DWORD i = start; KRF_SUCCESS(res) && i <= end; ++i)
	{
		if (m_downloadedFiles[i] == YES)
			continue;

		m_downloadedFiles[i] = PENDING;
		res = krfRequestKCPFile(GetFileName(i).c_str(), &CRuDataStore_MultiFile::ChunkDownloadCallback, cds);
		switch (res)
		{
		case KR_IO_PENDING:
#ifdef _DEBUG
			OutputDebugStringA(std::string(std::string("Requested file") + GetFileName(i) + "\n").c_str());
#endif
			// fall through
		case KR_OK:
			g_ruResourceManager->AddPending();
			break;
		default:
			--cds->count;
			break;
		}
	}

	if (KRF_SUCCESS(res))
		// don't queue work until it is downloaded, so return.
		return TRUE;

	if (cds->count == 0)
	{
		for (std::vector<ChunkDownloadStruct*>::iterator i = cds->multiFile->m_downloadStructs.begin(); i != cds->multiFile->m_downloadStructs.end(); i++)
		{
			if (*i == cds)
			{
				cds->multiFile->m_downloadStructs.erase(i);
				break;
			}
		}

		// failure, cleanup.
		delete cds;
	}

	return FALSE;
}

void CRuDataStore_MultiFile::ChunkDownloadCallback(const char* fileName, TKResult result, void* id)
{
	if (KRF_FAILURE(result))
	{
		krfRequestKCPFile(fileName, &CRuDataStore_MultiFile::ChunkDownloadCallback, id);
		return;
	}
	else
	{
		g_ruResourceManager->RemovePending();

		ChunkDownloadStruct* cds = reinterpret_cast<ChunkDownloadStruct*>(id);

		if(cds->multiFile)
		{
			size_t i = strlen(fileName);
			// retrieve the number (e.g. wdb/world/world_2.wdb128 will result in 128)
			while(isdigit(fileName[i - 1]))
				--i;

			cds->multiFile->m_downloadedFiles[atoi(fileName + i)] = YES;

			if (--cds->count == 0)
			{
				if (cds->callback)
					cds->callback(cds->key, cds->userData);
				for (std::vector<ChunkDownloadStruct*>::iterator i = cds->multiFile->m_downloadStructs.begin(); i != cds->multiFile->m_downloadStructs.end(); i++)
				{
					if (*i == cds)
					{
						cds->multiFile->m_downloadStructs.erase(i);
						break;
					}
				}

				if (cds->multiFile->m_callback && cds->multiFile->m_downloadStructs.empty())
					cds->multiFile->m_callback(cds->multiFile);

				delete cds;
			}
		}
	}
}


IRuStream *CRuDataStore_MultiFile::Retrieve(INT32 tableHandle, const BYTE *key)
{
	IRuStream *retVal = NULL;

	// Acquire critical section
	m_objectCS->Enter();

	// Ensure validity of handle
	assert(tableHandle >= 0 && tableHandle < m_tableHeaders.Count());

	// Build a key structure
	KeyStruct keyStruct;
	keyStruct.m_key = key;
	keyStruct.m_keyLength = m_tableHeaders[tableHandle]->m_keyLength;

	if(m_tableHeaders[tableHandle]->m_streamHeaders.ContainsKey(keyStruct))
	{
		// Locate the stream header record
		StreamHeader &header = m_tableHeaders[tableHandle]->m_streamHeaders.Get(keyStruct);

		DWORD start;
		DWORD end;

		FilesContainingChunk(tableHandle, key, start, end);

		bool allFilesDownloaded = true;
		for (DWORD i = start; i <= end; ++i)
		{
			switch (m_downloadedFiles[i])
			{
			case YES:
				break;
			case NO:
#ifdef _DEBUG
				char buf[128];
				itoa(i, buf, 10);
				OutputDebugStringA((std::string("Didn't download file ") + buf +"\n").c_str());
#endif
				KalydoDownloadData(tableHandle, key, NULL, NULL);
				if (m_downloadedFiles[i] != YES)
					allFilesDownloaded = false;
				break;
			case PENDING:
				allFilesDownloaded = false;
				break;
			}
		}

		if (allFilesDownloaded)
		{
			// Seek to position of the stream
			m_stream->Seek(header.m_streamPosition, ruSSM_Begin);

			// Read stream into memory
			CRuMemoryStream *memStream;
			memStream = ruNEW CRuMemoryStream();
			memStream->SetBufferSize(header.m_streamSize);
			memStream->SetStreamSize(header.m_streamSize);
			m_stream->Read(memStream->GetBuffer(), header.m_streamSize);

			retVal = memStream;
		}
	}

	// Release critical section
	m_objectCS->Leave();

	// Trigger retrieved event
	RuDataStoreEvent_Retrieved retrievedArgs(m_tableHeaders[tableHandle]->m_name, key, -1, retVal);
	m_event_Retrieved.Trigger(&retrievedArgs);

	return retVal;
}

IRuStream *CRuDataStore_MultiFile::RetrieveByIndex(INT32 tableHandle, INT32 index)
{
	IRuStream *retVal = NULL;

	// Acquire critical section
	m_objectCS->Enter();

	// Ensure validity of handle
	assert(tableHandle >= 0 && tableHandle < m_tableHeaders.Count());

	if(index >= 0 && index < m_tableHeaders[tableHandle]->m_streamHeaders.Count())
	{
/*
		// Locate the stream header record
		StreamHeader &header = m_tableHeaders[tableHandle]->m_streamHeaders.GetAtIndex(index);
*/
		// Update the stream header cache
		UpdateTableStreamHeaderCache(tableHandle);

		// Locate the stream header record
		StreamHeader &header = *(m_tableHeaders[tableHandle]->m_streamHeaderIndexedCache[index]);

		DWORD start;
		DWORD end;

		BYTE* key = new BYTE[m_tableHeaders[tableHandle]->m_keyLength];
		RetrieveKeyByIndex(tableHandle, index, key);
		FilesContainingChunk(tableHandle, key, start, end);
		bool allFilesDownloaded = true;
		for (DWORD i = start; i <= end; ++i)
		{
			switch (m_downloadedFiles[i])
			{
			case YES:
				break;
			case NO:
#ifdef _DEBUG
				char buf[128];
				itoa(i, buf, 10);
				OutputDebugStringA((std::string("Didn't download file ") + buf +"\n").c_str());
#endif
				KalydoDownloadData(tableHandle, key, NULL, NULL);
				if (m_downloadedFiles[i] != YES)
					allFilesDownloaded = false;
				break;
			case PENDING:
				allFilesDownloaded = false;
				break;
			}
		}
		delete [] key;

		if (allFilesDownloaded)
		{
			// Seek to position of the stream
			m_stream->Seek(header.m_streamPosition, ruSSM_Begin);

			if(header.m_streamSize > 1024 * 1024)
			{
				printf("\nWARNING: Ignored abnormally large stream chunk at %d, size %d\n", index, header.m_streamSize);

				retVal = NULL;
			}
			else if(header.m_streamSize < 0)
			{
				printf("\nWARNING: Ignored negative-sized stream chunk at %d, size %d\n", index, header.m_streamSize);

				retVal = NULL;
			}
			else
			{
				// Read stream into memory
				CRuMemoryStream *memStream;
				memStream = ruNEW CRuMemoryStream();
				memStream->SetBufferSize(header.m_streamSize);
				memStream->SetStreamSize(header.m_streamSize);
				m_stream->Read(memStream->GetBuffer(), header.m_streamSize);

				retVal = memStream;
			}
		}
	}

	// Release critical section
	m_objectCS->Leave();

	// Trigger retrieved event
	RuDataStoreEvent_Retrieved retrievedArgs(m_tableHeaders[tableHandle]->m_name, NULL, index, retVal);
	m_event_Retrieved.Trigger(&retrievedArgs);

	return retVal;
}

void CRuDataStore_MultiFile::SetDownloadCallback(CMultiFileCallback* function)
{
	m_callback = function;
}

BOOL CRuDataStore_MultiFile::LoadDataStoreHeader(DataStoreHeader *headerOut)
{
	// GUID + pos + size + allocSize
	INT32 headerSize = sizeof(UINT32) * 4 + sizeof(INT32) + sizeof(INT32) + sizeof(INT32);

	if (!PartAvailable(0, headerSize))
		return FALSE;

	// Seek to beginning of data store stream
	if (m_stream->Seek(0, ruSSM_Begin) != ruSEC_OK)
		return FALSE;

	// Read & confirm GUID correctness
	CRuGUID classGUID;
	if(RuReadGUID(m_stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read data store header position
	if (m_stream->Read(&headerOut->m_tableListPosition, sizeof(INT32)) != sizeof(INT32))
		return FALSE;
	if (m_stream->Read(&headerOut->m_tableListSize, sizeof(INT32)) != sizeof(INT32))
		return FALSE;
	if (m_stream->Read(&headerOut->m_tableListAllocationSize, sizeof(INT32)) != sizeof(INT32))
		return FALSE;

	return TRUE;
}

BOOL CRuDataStore_MultiFile::LoadTableHeaders(DataStoreHeader &dsHeader, CRuArrayList<TableHeader *> &tableHeaders)
{
	// size of one complete table header
	INT32 tableHeaderSize = sizeof(char) * 256 + sizeof(INT32) + sizeof(INT32) + sizeof(INT32) + sizeof(INT32);
	// total table size
	INT32 tableSize = dsHeader.m_tableListSize * tableHeaderSize;

	if (!PartAvailable(dsHeader.m_tableListPosition, tableSize))
		return FALSE;

	// Seek to beginning of table headers
	if (m_stream->Seek(dsHeader.m_tableListPosition, ruSSM_Begin) != ruSEC_OK)
		return FALSE;

	// Clear table headers array
	tableHeaders.Clear();

	for(INT32 i = 0; i < dsHeader.m_tableListSize; ++i)
	{
		// Append blank header to table list
		tableHeaders.Add(ruNEW TableHeader());

		// Read table properties
		if (m_stream->Read(tableHeaders[i]->m_name, sizeof(char) * 256) != sizeof(char) * 256)
			return FALSE;
		if (m_stream->Read(&tableHeaders[i]->m_keyLength, sizeof(INT32)) != sizeof(INT32))
			return FALSE;
		if (m_stream->Read(&tableHeaders[i]->m_tablePosition, sizeof(INT32)) != sizeof(INT32))
			return FALSE;
		if (m_stream->Read(&tableHeaders[i]->m_tableSize, sizeof(INT32)) != sizeof(INT32))
			return FALSE;
		if (m_stream->Read(&tableHeaders[i]->m_allocationSize, sizeof(INT32)) != sizeof(INT32))
			return FALSE;
	}

	return TRUE;
}

BOOL CRuDataStore_MultiFile::LoadStreamHeaders(CRuArrayList<TableHeader *> &tableHeaders, INT32 tableHandle)
{
	// size of one complete table header
	INT32 tableHeaderSize = tableHeaders[tableHandle]->m_keyLength + sizeof(INT32) + sizeof(INT32) + sizeof(INT32);
	// total table size
	INT32 tableSize = tableHeaders[tableHandle]->m_tableSize * tableHeaderSize;

	if (!PartAvailable(tableHeaders[tableHandle]->m_tablePosition, tableSize))
		return FALSE;

	// Seek to beginning of stream headers
	if (m_stream->Seek(tableHeaders[tableHandle]->m_tablePosition, ruSSM_Begin) != ruSEC_OK)
		return FALSE;

	for(INT32 i = 0; i < tableHeaders[tableHandle]->m_tableSize; ++i)
	{
		// Read stream header
		StreamHeader header;
		header.m_key = ruNEW BYTE [tableHeaders[tableHandle]->m_keyLength];
		if (m_stream->Read(header.m_key, tableHeaders[tableHandle]->m_keyLength) != tableHeaders[tableHandle]->m_keyLength)
			return FALSE;
		if (m_stream->Read(&header.m_streamPosition, sizeof(INT32)) != sizeof(INT32))
			return FALSE;
		if (m_stream->Read(&header.m_streamSize, sizeof(INT32)) != sizeof(INT32))
			return FALSE;
		if (m_stream->Read(&header.m_allocationSize, sizeof(INT32)) != sizeof(INT32))
			return FALSE;

		// Build a key structure
		KeyStruct keyStruct;
		keyStruct.m_key = header.m_key;
		keyStruct.m_keyLength = tableHeaders[tableHandle]->m_keyLength;

		tableHeaders[tableHandle]->m_streamHeaders.Add(keyStruct, header);
	}

	// Balance the stream header tree
	tableHeaders[tableHandle]->m_streamHeaders.BalanceTree();

	return TRUE;
}

BOOL CRuDataStore_MultiFile::PartAvailable(INT32 startPos, INT32 size)
{
	DWORD startFile, endFile;

	CRuMultiFileStream* mfs = static_cast<CRuMultiFileStream*>(m_stream);

	if (!mfs->GetFilesContaining(startPos, size, startFile, endFile))
		return FALSE;

	BOOL retVal = TRUE;
	for (DWORD i = startFile; i <= endFile; ++i)
	{
		switch (m_downloadedFiles[i])
		{
		case YES:
			break;
		case NO:
		{

			KalydoDownloadFiles(NULL, startFile, endFile);
			// check if the files are downloaded.
			if (m_downloadedFiles[i] != YES)
				retVal = FALSE;

			break;
		}
		case PENDING:
			retVal = FALSE;
			break;
		}
	}

	return retVal;
}
#endif


#pragma managed
