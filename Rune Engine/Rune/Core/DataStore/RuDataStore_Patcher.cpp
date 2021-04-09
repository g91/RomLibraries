#include "../../Core/DataStore/RuDataStore_Patcher.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuDataStore_Patcher, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuDataStore_Patcher, "CRuDataStore_Patcher", "IRuObject")
ruCLASSGUID_DEFINE(CRuDataStore_Patcher, 0xF4AFFB60, 0x121D477e, 0xBFDABB5B, 0xFA422CBB)

// ************************************************************************************************************************************************************

CRuDataStore_Patcher_PatchState::CRuDataStore_Patcher_PatchState()
:	m_patchStream(NULL),
	m_dataStore(NULL),
	m_compacting(FALSE),
	m_done(FALSE),
	m_patchProgress(0.0f)
{
}

CRuDataStore_Patcher_PatchState::~CRuDataStore_Patcher_PatchState()
{
	delete m_patchStream;
	ruSAFE_RELEASE(m_dataStore);
}

BOOL CRuDataStore_Patcher_PatchState::IsDataStoreOpen()
{
	return m_dataStore != NULL;
}

BOOL CRuDataStore_Patcher_PatchState::IsPatchDone()
{
	return m_done;
}

REAL CRuDataStore_Patcher_PatchState::GetPatchProgress()
{
	return m_patchProgress;
}

// ************************************************************************************************************************************************************

CRuDataStore_Patcher_BuildState::CRuDataStore_Patcher_BuildState()
:	m_patchStream(NULL),
	m_srcDataStore(NULL),
	m_tgtDataStore(NULL),
	m_keyBuffer(NULL),
	m_done(FALSE),
	m_patchProgress(0.0f)
{
}

CRuDataStore_Patcher_BuildState::~CRuDataStore_Patcher_BuildState()
{
	delete m_patchStream;
	ruSAFE_RELEASE(m_srcDataStore);
	ruSAFE_RELEASE(m_tgtDataStore);

	delete[] m_keyBuffer;
}

BOOL CRuDataStore_Patcher_BuildState::IsPatchDone()
{
	return m_done;
}

REAL CRuDataStore_Patcher_BuildState::GetPatchProgress()
{
	return m_patchProgress;
}

// ************************************************************************************************************************************************************

CRuDataStore_Patcher::CRuDataStore_Patcher()
{
}

CRuDataStore_Patcher::~CRuDataStore_Patcher()
{
}

BOOL CRuDataStore_Patcher::ApplyPatch(const char *patchPath, INT32 patchOffset, BOOL compact)
{
	// Open patch stream
	CRuFileStream *patchStream = ruNEW CRuFileStream();
	if(patchStream->Open(patchPath) != ruSEC_OK)
	{
		delete patchStream;
		return FALSE;
	}

	// Seek to beginning of patch data
	if(patchStream->Seek(patchOffset, ruSSM_Begin) != ruSEC_OK)
	{
		delete patchStream;
		return FALSE;
	}

	// Validate class GUID
	if(RuReadAndValidateGUID(patchStream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		delete patchStream;
		return FALSE;
	}

	// Read relative data store path
	INT32 relativeDataStorePathLen = 0;
	patchStream->Read(&relativeDataStorePathLen, sizeof(INT32));

	char *relativeDataStorePathRaw = ruNEW char [relativeDataStorePathLen];
	patchStream->Read(relativeDataStorePathRaw, sizeof(char) * relativeDataStorePathLen);
	CRuString relativeDataStorePath = relativeDataStorePathRaw;
	delete[] relativeDataStorePathRaw;

	// Open data store for patching
	CRuDataStore_Disk *dataStore = ruNEW CRuDataStore_Disk();
	if(dataStore->Open(relativeDataStorePath, FALSE) == FALSE)
	{
		dataStore->Create(relativeDataStorePath);
	}

	// Go until no commands remaining in the patch stream
	BOOL done = FALSE;
	while(!patchStream->EndOfStream() && !done)
	{
		// Read patch action
		PatchAction patchAction = PATCHACTION_UPDATE;
		patchStream->Read(&patchAction, sizeof(PatchAction));

		// Read table name
		INT32 tableNameLen = 0;
		patchStream->Read(&tableNameLen, sizeof(INT32));

		char *tableName = ruNEW char [tableNameLen];
		patchStream->Read(tableName, tableNameLen);

		// Read key
		INT32 keyLen = 0;
		patchStream->Read(&keyLen, sizeof(INT32));

		BYTE *key = ruNEW BYTE [keyLen];
		patchStream->Read(key, keyLen);

		switch(patchAction)
		{
			case PATCHACTION_UPDATE:
				{
					// Read record stream
					DWORD streamSize = 0;
					patchStream->Read(&streamSize, sizeof(DWORD));

					CRuMemoryStream *recordStream = ruNEW CRuMemoryStream();
					recordStream->SetStreamSize(streamSize);
					patchStream->Read(recordStream->GetBuffer(), streamSize);

					// Patch into data store
					INT32 tableHandle = dataStore->CreateTable(tableName, keyLen);

					if(tableHandle >= 0)
					{
						dataStore->Store(tableHandle, key, recordStream);
					}
					else
					{
						OutputDebugString("Problems creating or opening target table\n");

						// Release record resources
						delete[] tableName;
						delete[] key;
						delete recordStream;

						// Release data store
						ruSAFE_RELEASE(dataStore);

						// Release patch stream
						delete patchStream;

						return FALSE;
					}

					delete recordStream;
				}

				break;

			case PATCHACTION_DELETE:
				{
					INT32 tableHandle = dataStore->CreateTable(tableName, keyLen);

					if(tableHandle >= 0)
					{
						// Delete entry
						dataStore->Delete(tableHandle, key);
					}
					else
					{
						// Return error
					}
				}

				break;

			case PATCHACTION_FINISH:
				{
					done = TRUE;
				}

				break;
		}

		// Release record resources
		delete[] tableName;
		delete[] key;
	}

	if(compact)
	{
		CRuString tempDSPath = relativeDataStorePath;
		tempDSPath.Append(".tmp");

		CRuDataStore_Disk *dstDataStore = ruNEW CRuDataStore_Disk();
		dstDataStore->Create(tempDSPath);
		dstDataStore->DisablePagePadding(TRUE);
		dataStore->CopyTo(dstDataStore);
		ruSAFE_RELEASE(dstDataStore);

		// Release data store
		ruSAFE_RELEASE(dataStore);

		DeleteFile(relativeDataStorePath);
		MoveFile(tempDSPath, relativeDataStorePath);
	}
	else
	{
		// Release data store
		ruSAFE_RELEASE(dataStore);
	}

	// Release patch stream
	delete patchStream;

	return TRUE;
}

CRuDataStore_Patcher_PatchState *CRuDataStore_Patcher::ApplyPatch_Begin(const char *rootPath, const char *patchPath, INT32 patchOffset, BOOL compact)
{
	CRuDataStore_Patcher_PatchState *patchState = ruNEW CRuDataStore_Patcher_PatchState();

	// Open patch stream
	patchState->m_patchStream = ruNEW CRuFileStream();
	if(patchState->m_patchStream->Open(patchPath) == ruSEC_OK)
	{
		// Retrieve patch stream size
		patchState->m_patchStreamSize = patchState->m_patchStream->GetStreamSize();

		// Seek to beginning of patch data
		if(patchState->m_patchStream->Seek(patchOffset, ruSSM_Begin) == ruSEC_OK)
		{
			// Validate class GUID
			if(RuReadAndValidateGUID(patchState->m_patchStream, ClassGUID(), this->GetClassGUID()))
			{
				// Read relative data store path
				INT32 relativeDataStorePathLen = 0;
				patchState->m_patchStream->Read(&relativeDataStorePathLen, sizeof(INT32));

				char *relativeDataStorePathRaw = ruNEW char [relativeDataStorePathLen];
				patchState->m_patchStream->Read(relativeDataStorePathRaw, sizeof(char) * relativeDataStorePathLen);
				patchState->m_relativeDataStorePath = relativeDataStorePathRaw;
				delete[] relativeDataStorePathRaw;

				// Try to get file attributes
				DWORD fileAttr = GetFileAttributesA(patchState->m_relativeDataStorePath);

				if(fileAttr != INVALID_FILE_ATTRIBUTES)
				{
					// Make sure data store is not read-only
					if(!SetFileAttributesA(patchState->m_relativeDataStorePath, FILE_ATTRIBUTE_NORMAL))
					{
						MessageBox(NULL, "Cannot set file attribute to read-write.", "Critical Error", MB_OK);
					}
				}

				// Open data store for patching
				patchState->m_dataStore = ruNEW CRuDataStore_Disk();
				if(patchState->m_dataStore->Open(patchState->m_relativeDataStorePath, FALSE) == FALSE)
				{
					// Open failed, create the data store for patching
					if(patchState->m_dataStore->Create(patchState->m_relativeDataStorePath) == FALSE)
					{
						// Release data store
						ruSAFE_RELEASE(patchState->m_dataStore);
					}
				}

/*
				// Open data store for patching
				patchState->m_dataStore = ruNEW CRuDataStore_Disk();
				if(patchState->m_dataStore->Open(patchState->m_relativeDataStorePath, FALSE) == FALSE)
				{
					// Build full path
					char fullDataStorePath[2048];
					sprintf(fullDataStorePath, "%s\\%s", rootPath, patchState->m_relativeDataStorePath);

					// Open failed, create the data store for patching
					patchState->m_dataStore->Create(fullDataStorePath);
				}
*/
				// Save options
				patchState->m_compact = compact;

				return patchState;
			}
		}
	}

	delete patchState;
	return NULL;
}

BOOL CRuDataStore_Patcher::ApplyPatch_Step(CRuDataStore_Patcher_PatchState *patchState)
{
	if(patchState->m_compacting == FALSE)
	{
		// Go until no commands remaining in the patch stream
		INT32 maxCmdCount = 10;
		INT32 cmdCount = 0;
		while(!patchState->m_patchStream->EndOfStream() && !patchState->m_done && cmdCount < maxCmdCount)
		{
			// Increment command count
			++cmdCount;

			// Update patch progress
			DWORD patchStreamPos = patchState->m_patchStream->GetPosition();
			patchState->m_patchProgress = (REAL) patchStreamPos / (REAL) patchState->m_patchStreamSize * 0.5f;

			// Read patch action
			PatchAction patchAction = PATCHACTION_UPDATE;
			patchState->m_patchStream->Read(&patchAction, sizeof(PatchAction));

			// Read table name
			INT32 tableNameLen = 0;
			patchState->m_patchStream->Read(&tableNameLen, sizeof(INT32));

			char *tableName = ruNEW char [tableNameLen];
			patchState->m_patchStream->Read(tableName, tableNameLen);

			// Read key
			INT32 keyLen = 0;
			patchState->m_patchStream->Read(&keyLen, sizeof(INT32));

			BYTE *key = ruNEW BYTE [keyLen];
			patchState->m_patchStream->Read(key, keyLen);

			switch(patchAction)
			{
				case PATCHACTION_UPDATE:
					{
						// Read record stream
						DWORD streamSize = 0;
						patchState->m_patchStream->Read(&streamSize, sizeof(DWORD));

						CRuMemoryStream *recordStream = ruNEW CRuMemoryStream();
						recordStream->SetStreamSize(streamSize);
						patchState->m_patchStream->Read(recordStream->GetBuffer(), streamSize);

						// Patch into data store
						INT32 tableHandle = patchState->m_dataStore->CreateTable(tableName, keyLen);

						if(tableHandle >= 0)
						{
							patchState->m_dataStore->Store(tableHandle, key, recordStream);
						}
						else
						{
							OutputDebugString("Problems creating or opening target table\n");

							// Release record resources
							delete[] tableName;
							delete[] key;
							delete recordStream;

							return FALSE;
						}

						delete recordStream;
					}

					break;

				case PATCHACTION_DELETE:
					{
						INT32 tableHandle = patchState->m_dataStore->CreateTable(tableName, keyLen);

						if(tableHandle >= 0)
						{
							// Delete entry
							patchState->m_dataStore->Delete(tableHandle, key);
						}
						else
						{
							// Return error
						}
					}

					break;

				case PATCHACTION_DELETETABLE:
					{
						INT32 tableHandle = patchState->m_dataStore->CreateTable(tableName, keyLen);

						if(tableHandle >= 0)
						{
							// Delete all entries
							patchState->m_dataStore->DeleteAllRecords(tableHandle);

							// Delete table
							patchState->m_dataStore->DeleteTable(tableName);
						}
						else
						{
							// Return error
						}
					}

					break;

				case PATCHACTION_CREATETABLE:
					{
						INT32 tableHandle = patchState->m_dataStore->CreateTable(tableName, keyLen);
					}

					break;

				case PATCHACTION_FINISH:
					{
						patchState->m_done = TRUE;
					}

					break;
			}

			// Release record resources
			delete[] tableName;
			delete[] key;
		}

		if(patchState->m_done != false)
		{
			if(patchState->m_compact)
			{
				patchState->m_compacting = TRUE;
				patchState->m_compactingStarted = FALSE;

				patchState->m_done = FALSE;
			}
		}
	}

	if(patchState->m_compacting)
	{
		if(patchState->m_compactingStarted == FALSE)
		{
			// Create the temporary datastore
			CRuString tempDSPath = patchState->m_relativeDataStorePath;
			tempDSPath.Append(".tmp");

			patchState->m_copyToDataStore = ruNEW CRuDataStore_Disk();
			patchState->m_copyToDataStore->Create(tempDSPath);
			patchState->m_copyToDataStore->DisablePagePadding(TRUE);

			// Start the step copy
			patchState->m_copyToState = patchState->m_dataStore->CopyTo_Begin(patchState->m_copyToDataStore);

			// Mark compacting as having started
			patchState->m_compactingStarted = TRUE;
		}

		// Do step copy
		patchState->m_dataStore->CopyTo_Step(patchState->m_copyToState);

		// Update progress meter
		patchState->m_patchProgress = patchState->m_copyToState->GetCopyProgress() * 0.5f + 0.5f;

		if(patchState->m_copyToState->IsCopyDone())
		{
			// Terminate copy to
			patchState->m_dataStore->CopyTo_End(patchState->m_copyToState);

			// Close the temporary data store
			ruSAFE_RELEASE(patchState->m_copyToDataStore);

			// Close the source data store
			ruSAFE_RELEASE(patchState->m_dataStore);

			// Delete original WDB and copy the WDB duplicate over
			CRuString tempDSPath = patchState->m_relativeDataStorePath;
			tempDSPath.Append(".tmp");

			// Wait before delete
			Sleep(2000);

			// Delete original WDB
			if(!DeleteFile(patchState->m_relativeDataStorePath))
			{
//				MessageBox(NULL, "Cannot delete original WDB file.", "Critical Error", MB_OK);
			}

			// Wait before rename
			Sleep(2000);

			// Rename WDB
			if(!MoveFile(tempDSPath, patchState->m_relativeDataStorePath))
			{
				DWORD errorCode = GetLastError();
				char errorStr[2048];
				sprintf(errorStr, "Cannot rename patched WDB file, error code %u.", errorCode);
				MessageBox(NULL, errorStr, "Critical Error", MB_OK);
			}

			// Mark patch as done
			patchState->m_done = TRUE;
		}
	}

	return TRUE;
}

BOOL CRuDataStore_Patcher::ApplyPatch_End(CRuDataStore_Patcher_PatchState *patchState)
{
	delete patchState;
	return TRUE;
}

void CRuDataStore_Patcher::BuildPatch(const char *patchPath, const char *srcDSPath, const char *tgtDSPath, const char *relativeDSPath)
{
	// Open data store
	CRuDataStore_Disk *srcDataStore = ruNEW CRuDataStore_Disk();
	srcDataStore->Open(srcDSPath, TRUE);

	CRuDataStore_Disk *tgtDataStore = ruNEW CRuDataStore_Disk();
	tgtDataStore->Open(tgtDSPath, TRUE);

	// Create patch file
	CRuFileStream *patchStream = ruNEW CRuFileStream();
	patchStream->Create(patchPath);

	// Write class GUID for identification
	patchStream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Store relative data store path
	INT32 relativeDSPathLen = (INT32)strlen(relativeDSPath) + 1;
	patchStream->Write(&relativeDSPathLen, sizeof(INT32));
	patchStream->Write(relativeDSPath, sizeof(char) * relativeDSPathLen);

	// Iterate through all tables in the target data store
	for(INT32 i = 0, numTables = tgtDataStore->GetNumTables(); i < numTables; ++i)
	{
		const char *tableName = tgtDataStore->GetTableNameByIndex(i);
		INT32 tableNameLen = (INT32)strlen(tableName) + 1;

		INT32 srcTableHandle = srcDataStore->GetTableHandle(tableName);
		INT32 tgtTableHandle = tgtDataStore->GetTableHandle(tableName);

		INT32 tableKeyLen = tgtDataStore->GetTableKeyLength(tgtTableHandle);
		INT32 numRecords = tgtDataStore->GetNumRecords(tgtTableHandle);

		BYTE *key = ruNEW BYTE [tableKeyLen];

		// Iterate through all records in the table
		for(INT32 j = 0; j < numRecords; ++j)
		{
			// Retrieve record key from data store by index
			if(tgtDataStore->RetrieveKeyByIndex(tgtTableHandle, j, key))
			{
				// By default assume record stream has changed
				BOOL recordStreamChanged = TRUE;

				// Retrieve record stream from target data store by index
				IRuStream *tgtRecordStream = tgtDataStore->RetrieveByIndex(tgtTableHandle, j);

				// Retrieve record stream from source data store using the record key
				IRuStream *srcRecordStream = srcDataStore->Retrieve(srcTableHandle, key);

				if(srcRecordStream)
				{
					if(tgtRecordStream->GetStreamSize() == srcRecordStream->GetStreamSize())
					{
						// Read both streams into memory so they can be compared
						DWORD streamSize = tgtRecordStream->GetStreamSize();
						BYTE *buffer0 = ruNEW BYTE [streamSize];
						BYTE *buffer1 = ruNEW BYTE [streamSize];

						tgtRecordStream->Seek(0, ruSSM_Begin);
						tgtRecordStream->Read(buffer0, streamSize);

						srcRecordStream->Seek(0, ruSSM_Begin);
						srcRecordStream->Read(buffer1, streamSize);

						// Compare the two streams
						if(memcmp(buffer0, buffer1, streamSize) == 0)
						{
							// If the two streams are identical, then mark the record as not changed
							recordStreamChanged = FALSE;
						}

						// Delete buffers
						delete[] buffer0;
						delete[] buffer1;
					}
				}

				if(recordStreamChanged)
				{
					// Copy target record stream into memory
					DWORD streamSize = tgtRecordStream->GetStreamSize();
					BYTE *buffer = ruNEW BYTE [streamSize];
					tgtRecordStream->Seek(0, ruSSM_Begin);
					tgtRecordStream->Read(buffer, streamSize);

					// Write patch action
					PatchAction patchAction = PATCHACTION_UPDATE;
					patchStream->Write(&patchAction, sizeof(PatchAction));

					// Write table name
					patchStream->Write(&tableNameLen, sizeof(INT32));
					patchStream->Write(tableName, tableNameLen);

					// Write key
					patchStream->Write(&tableKeyLen, sizeof(INT32));
					patchStream->Write(key, tableKeyLen);

					// Write record stream
					patchStream->Write(&streamSize, sizeof(DWORD));
					patchStream->Write(buffer, streamSize);

					// Delete buffer
					delete[] buffer;
				}

				// Delete record streams
				delete tgtRecordStream;
				delete srcRecordStream;
			}
		}

		delete[] key;
	}

	// Iterate through all records in the source data store to find all records which have been deleted
	for(INT32 i = 0, numTables = srcDataStore->GetNumTables(); i < numTables; ++i)
	{
		const char *tableName = srcDataStore->GetTableNameByIndex(i);
		INT32 tableNameLen = (INT32)strlen(tableName) + 1;

		INT32 srcTableHandle = srcDataStore->GetTableHandle(tableName);
		INT32 tgtTableHandle = tgtDataStore->GetTableHandle(tableName);

		INT32 tableKeyLen = srcDataStore->GetTableKeyLength(srcTableHandle);
		INT32 numRecords = srcDataStore->GetNumRecords(srcTableHandle);

		BYTE *key = ruNEW BYTE [tableKeyLen];

		if(tgtTableHandle < 0)
		{
			// Target data store has no such table, so let's delete the table

			// Write patch action
			PatchAction patchAction = PATCHACTION_DELETETABLE;
			patchStream->Write(&patchAction, sizeof(PatchAction));

			// Write table name
			patchStream->Write(&tableNameLen, sizeof(INT32));
			patchStream->Write(tableName, tableNameLen);

			// Write a dummy key
			memset(key, 0, sizeof(BYTE) * tableKeyLen);
			patchStream->Write(&tableKeyLen, sizeof(INT32));
			patchStream->Write(key, tableKeyLen);
		}
		else
		{
			// Iterate through all records in the table
			for(INT32 j = 0; j < numRecords; ++j)
			{
				// Retrieve record key from source data store by index
				if(srcDataStore->RetrieveKeyByIndex(srcTableHandle, j, key))
				{
					IRuStream *recordStream = tgtDataStore->Retrieve(tgtTableHandle, key);

					if(recordStream == NULL)
					{
						// Write patch action
						PatchAction patchAction = PATCHACTION_DELETE;
						patchStream->Write(&patchAction, sizeof(PatchAction));

						// Write table name
						patchStream->Write(&tableNameLen, sizeof(INT32));
						patchStream->Write(tableName, tableNameLen);

						// Write key
						patchStream->Write(&tableKeyLen, sizeof(INT32));
						patchStream->Write(key, tableKeyLen);
					}
					else
					{
						delete recordStream;
					}
				}
			}
		}

		delete[] key;
	}

	// Write final patch action
	PatchAction patchAction = PATCHACTION_FINISH;
	patchStream->Write(&patchAction, sizeof(PatchAction));

	// Release data store	
	ruSAFE_RELEASE(tgtDataStore);
	ruSAFE_RELEASE(srcDataStore);

	delete patchStream;
}

CRuDataStore_Patcher_BuildState *CRuDataStore_Patcher::BuildPatch_Begin(const char *patchPath, const char *srcDSPath, const char *tgtDSPath, const char *relativeDSPath)
{
	CRuDataStore_Patcher_BuildState *buildState = ruNEW CRuDataStore_Patcher_BuildState();

	// Open source data store
	buildState->m_srcDataStore = ruNEW CRuDataStore_Disk();
	if(buildState->m_srcDataStore->Open(srcDSPath, TRUE) == FALSE)
	{
		// Source data store open failed (this can happen if the WDB file is new)
		ruSAFE_RELEASE(buildState->m_srcDataStore);
	}

	// Open target data store
	buildState->m_tgtDataStore = ruNEW CRuDataStore_Disk();
	if(buildState->m_tgtDataStore->Open(tgtDSPath, TRUE) == FALSE)
	{
		// Target data store open failed (this should never happen)
		ruSAFE_RELEASE(buildState->m_srcDataStore);
		ruSAFE_RELEASE(buildState->m_tgtDataStore);

		// Delete build state
		delete buildState;
		buildState = NULL;
	}

	// If build state is normal, continue to create the patch stream
	if(buildState)
	{
		// Create patch file
		buildState->m_patchStream = ruNEW CRuFileStream();
		buildState->m_patchStream->Create(patchPath);

		// Write class GUID for identification into patch file
		buildState->m_patchStream->Write(ClassGUID().Data(), ClassGUID().Size());

		// Store relative data store path into patch file
		INT32 relativeDSPathLen = (INT32)strlen(relativeDSPath) + 1;
		buildState->m_patchStream->Write(&relativeDSPathLen, sizeof(INT32));
		buildState->m_patchStream->Write(relativeDSPath, sizeof(char) * relativeDSPathLen);

		// Begin at stage 0
		buildState->m_buildStage = 0;
	}

	return buildState;
}

BOOL CRuDataStore_Patcher::BuildPatch_Step(CRuDataStore_Patcher_BuildState *buildState)
{
	INT32 maxCmdCount = 10;
	INT32 cmdCount = 0;

	switch(buildState->m_buildStage)
	{
		case 0:
			{
				// Proceed directly to stage 1
				buildState->m_buildStage = 1;
				buildState->m_tabIndex = 0;
				buildState->m_recIndex = 0;
			}

			break;

		case 1:
			{
				// Iterate through all tables in the target data store
				for(INT32 i = buildState->m_tabIndex, numTables = buildState->m_tgtDataStore->GetNumTables(); i < numTables; ++i, ++buildState->m_tabIndex)
				{
					// If we are at the first record of the table, initialize table info first
					if(buildState->m_recIndex == 0)
					{
						buildState->m_tabName = buildState->m_tgtDataStore->GetTableNameByIndex(i);
						buildState->m_tabNameLen = (INT32)strlen(buildState->m_tabName) + 1;

						buildState->m_srcTabHandle = buildState->m_srcDataStore ? buildState->m_srcDataStore->GetTableHandle(buildState->m_tabName) : -1;
						buildState->m_tgtTabHandle = buildState->m_tgtDataStore->GetTableHandle(buildState->m_tabName);

						buildState->m_tabKeyLen = buildState->m_tgtDataStore->GetTableKeyLength(buildState->m_tgtTabHandle);
						buildState->m_tabNumRecords = buildState->m_tgtDataStore->GetNumRecords(buildState->m_tgtTabHandle);

						buildState->m_keyBuffer = ruNEW BYTE [buildState->m_tabKeyLen];

						// Write patch action
						PatchAction patchAction = PATCHACTION_CREATETABLE;
						buildState->m_patchStream->Write(&patchAction, sizeof(PatchAction));

						// Write table name
						buildState->m_patchStream->Write(&buildState->m_tabNameLen, sizeof(INT32));
						buildState->m_patchStream->Write(buildState->m_tabName, buildState->m_tabNameLen);

						// Write key
						buildState->m_patchStream->Write(&buildState->m_tabKeyLen, sizeof(INT32));
						buildState->m_patchStream->Write(buildState->m_keyBuffer, buildState->m_tabKeyLen);
					}

					// Iterate through all records in the table
					for(INT32 j = buildState->m_recIndex; j < buildState->m_tabNumRecords; )
					{
						// Retrieve record key from data store by index
						if(buildState->m_tgtDataStore->RetrieveKeyByIndex(buildState->m_tgtTabHandle, j, buildState->m_keyBuffer))
						{
							// By default assume record stream has changed
							BOOL recordStreamChanged = TRUE;

							// Retrieve record stream from target data store by index
							IRuStream *tgtRecordStream = buildState->m_tgtDataStore->RetrieveByIndex(buildState->m_tgtTabHandle, j);

							// Retrieve record stream from source data store using the record key
							IRuStream *srcRecordStream = buildState->m_srcDataStore ? buildState->m_srcDataStore->Retrieve(buildState->m_srcTabHandle, buildState->m_keyBuffer) : NULL;

							if(tgtRecordStream)
							{
								if(srcRecordStream)
								{
									if(tgtRecordStream->GetStreamSize() == srcRecordStream->GetStreamSize())
									{
										// Read both streams into memory so they can be compared
										DWORD streamSize = tgtRecordStream->GetStreamSize();
										BYTE *buffer0 = ruNEW BYTE [streamSize];
										BYTE *buffer1 = ruNEW BYTE [streamSize];

										tgtRecordStream->Seek(0, ruSSM_Begin);
										tgtRecordStream->Read(buffer0, streamSize);

										srcRecordStream->Seek(0, ruSSM_Begin);
										srcRecordStream->Read(buffer1, streamSize);

										// Compare the two streams
										if(memcmp(buffer0, buffer1, streamSize) == 0)
										{
											// If the two streams are identical, then mark the record as not changed
											recordStreamChanged = FALSE;
										}

										// Delete buffers
										delete[] buffer0;
										delete[] buffer1;
									}
								}

								if(recordStreamChanged)
								{
									// Copy target record stream into memory
									DWORD streamSize = tgtRecordStream->GetStreamSize();
									BYTE *buffer = ruNEW BYTE [streamSize];
									tgtRecordStream->Seek(0, ruSSM_Begin);
									tgtRecordStream->Read(buffer, streamSize);

									// Write patch action
									PatchAction patchAction = PATCHACTION_UPDATE;
									buildState->m_patchStream->Write(&patchAction, sizeof(PatchAction));

									// Write table name
									buildState->m_patchStream->Write(&buildState->m_tabNameLen, sizeof(INT32));
									buildState->m_patchStream->Write(buildState->m_tabName, buildState->m_tabNameLen);

									// Write key
									buildState->m_patchStream->Write(&buildState->m_tabKeyLen, sizeof(INT32));
									buildState->m_patchStream->Write(buildState->m_keyBuffer, buildState->m_tabKeyLen);

									// Write record stream
									buildState->m_patchStream->Write(&streamSize, sizeof(DWORD));
									buildState->m_patchStream->Write(buffer, streamSize);

									// Delete buffer
									delete[] buffer;
								}

								// Delete record streams
								delete tgtRecordStream;
								delete srcRecordStream;
							}
							else
							{
								// bad error
							}
						}

						// Increment counters
						++j;
						++buildState->m_recIndex;
						++cmdCount;

						// Update progress meter
						REAL tableFactor = 1.0f / numTables;
						buildState->m_patchProgress = ((static_cast<REAL>(j) / buildState->m_tabNumRecords * tableFactor) + (static_cast<REAL>(i) / numTables)) * 0.5f;

						// If we have reached the end of the current table, do some cleanup
						if(j == buildState->m_tabNumRecords)
						{
							// Delete key buffer
							delete[] buildState->m_keyBuffer;
							buildState->m_keyBuffer = NULL;

							// Reset record index to 0
							buildState->m_recIndex = 0;
						}
						// If we have exceed the command count, return
						else if(cmdCount > maxCmdCount)
						{
							return TRUE;
						}
					}
				}

				// Proceed directly to stage 2
				buildState->m_buildStage = 2;
				buildState->m_tabIndex = 0;
				buildState->m_recIndex = 0;
			}

			break;

		case 2:
			{
				if(buildState->m_srcDataStore)
				{
					// Iterate through all records in the source data store to find all records which have been deleted
					for(INT32 i = buildState->m_tabIndex, numTables = buildState->m_srcDataStore->GetNumTables(); i < numTables; ++i, ++buildState->m_tabIndex)
					{
						// If we are at the first record of the table, initialize table info first
						if(buildState->m_recIndex == 0)
						{
							buildState->m_tabName = buildState->m_srcDataStore->GetTableNameByIndex(i);
							buildState->m_tabNameLen = (INT32)strlen(buildState->m_tabName) + 1;

							buildState->m_srcTabHandle = buildState->m_srcDataStore ? buildState->m_srcDataStore->GetTableHandle(buildState->m_tabName) : -1;
							buildState->m_tgtTabHandle = buildState->m_tgtDataStore->GetTableHandle(buildState->m_tabName);

							buildState->m_tabKeyLen = buildState->m_srcDataStore->GetTableKeyLength(buildState->m_srcTabHandle);
							buildState->m_tabNumRecords = buildState->m_srcDataStore->GetNumRecords(buildState->m_srcTabHandle);

							buildState->m_keyBuffer = ruNEW BYTE [buildState->m_tabKeyLen];

							if(buildState->m_tgtTabHandle < 0)
							{
								// Target data store has no such table, so let's delete the table

								// Write patch action
								PatchAction patchAction = PATCHACTION_DELETETABLE;
								buildState->m_patchStream->Write(&patchAction, sizeof(PatchAction));

								// Write table name
								buildState->m_patchStream->Write(&buildState->m_tabNameLen, sizeof(INT32));
								buildState->m_patchStream->Write(buildState->m_tabName, buildState->m_tabNameLen);

								// Write a dummy key
								memset(buildState->m_keyBuffer, 0, sizeof(BYTE) * buildState->m_tabKeyLen);

								buildState->m_patchStream->Write(&buildState->m_tabKeyLen, sizeof(INT32));
								buildState->m_patchStream->Write(buildState->m_keyBuffer, buildState->m_tabKeyLen);

								delete[] buildState->m_keyBuffer;
								buildState->m_keyBuffer = NULL;

								// Continue to next table
								continue;
							}
						}

						// Iterate through all records in the table
						for(INT32 j = buildState->m_recIndex; j < buildState->m_tabNumRecords; )
						{
							// Retrieve record key from source data store by index
							if(buildState->m_srcDataStore->RetrieveKeyByIndex(buildState->m_srcTabHandle, j, buildState->m_keyBuffer))
							{
								IRuStream *recordStream = buildState->m_tgtDataStore->Retrieve(buildState->m_tgtTabHandle, buildState->m_keyBuffer);

								if(recordStream == NULL)
								{
									// Write patch action
									PatchAction patchAction = PATCHACTION_DELETE;
									buildState->m_patchStream->Write(&patchAction, sizeof(PatchAction));

									// Write table name
									buildState->m_patchStream->Write(&buildState->m_tabNameLen, sizeof(INT32));
									buildState->m_patchStream->Write(buildState->m_tabName, buildState->m_tabNameLen);

									// Write key
									buildState->m_patchStream->Write(&buildState->m_tabKeyLen, sizeof(INT32));
									buildState->m_patchStream->Write(buildState->m_keyBuffer, buildState->m_tabKeyLen);
								}
								else
								{
									delete recordStream;
								}
							}

							// Increment counters
							++j;
							++buildState->m_recIndex;
							++cmdCount;

							// Update progress meter
							REAL tableFactor = 1.0f / numTables;
							buildState->m_patchProgress = ((static_cast<REAL>(j) / buildState->m_tabNumRecords * tableFactor) + (static_cast<REAL>(i) / numTables)) * 0.5f + 0.5f;

							// If we have reached the end of the current table, do some cleanup
							if(j == buildState->m_tabNumRecords)
							{
								// Delete key buffer
								delete[] buildState->m_keyBuffer;
								buildState->m_keyBuffer = NULL;

								// Reset record index to 0
								buildState->m_recIndex = 0;
							}
							// If we have exceed the command count, return
							else if(cmdCount > maxCmdCount)
							{
								return TRUE;
							}
						}
					}
				}

				// Proceed directly to stage 3 (finished)
				buildState->m_buildStage = 3;
			}

			break;

		case 3:
			{
				// Write final patch action
				PatchAction patchAction = PATCHACTION_FINISH;
				buildState->m_patchStream->Write(&patchAction, sizeof(PatchAction));

				// Mark build as done
				buildState->m_done = TRUE;
			}

			break;
	}

	return TRUE;
}

BOOL CRuDataStore_Patcher::BuildPatch_End(CRuDataStore_Patcher_BuildState *buildState)
{
	delete buildState;
	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
