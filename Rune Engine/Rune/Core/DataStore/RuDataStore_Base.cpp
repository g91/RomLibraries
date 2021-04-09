#include "../../Core/DataStore/RuDataStore_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuDataStore_Base, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuDataStore_Base, "CRuDataStore_Base", "IRuObject")
ruCLASSGUID_DEFINE(CRuDataStore_Base, 0xCAC22FD3, 0x66BB4363, 0x86021BBA, 0x088F1657)

// ************************************************************************************************************************************************************

CRuDataStore_CopyToState::CRuDataStore_CopyToState()
:	m_done(FALSE)
{
}

CRuDataStore_CopyToState::~CRuDataStore_CopyToState()
{
	delete[] m_keyBuffer;
}

BOOL CRuDataStore_CopyToState::IsCopyDone()
{
	return m_done;
}

REAL CRuDataStore_CopyToState::GetCopyProgress()
{
	return m_copyProgress;
}

// ************************************************************************************************************************************************************

CRuDataStore_Base::~CRuDataStore_Base()
{
}

BOOL CRuDataStore_Base::CopyTo(CRuDataStore_Base *dstDataStore)
{
	INT32 numTables = this->GetNumTables();

	INT32 keyBufferSize = 128;
	BYTE *key = ruNEW BYTE [keyBufferSize];

	for(INT32 i = 0; i < numTables; ++i)
	{
		const char *tableName = this->GetTableNameByIndex(i);
		INT32 tableKeyLength = this->GetTableKeyLengthByIndex(i);

		INT32 srcTableHandle = this->GetTableHandle(tableName);
		INT32 dstTableHandle = dstDataStore->CreateTable(tableName, tableKeyLength);

		INT32 numEntries = this->GetNumRecords(srcTableHandle);

		if(tableKeyLength > keyBufferSize)
		{
			keyBufferSize = tableKeyLength;
			delete[] key;
			key = ruNEW BYTE [keyBufferSize];
		}

		for(INT32 j = 0; j < numEntries; ++j)
		{
			// Retrieve key and data
			IRuStream *stream = this->RetrieveByIndex(srcTableHandle, j);

			if(stream)
			{
				this->RetrieveKeyByIndex(srcTableHandle, j, key);
				dstDataStore->Store(dstTableHandle, key, stream);
				delete stream;
			}
		}
	}

	delete[] key;

	return TRUE;
}

BOOL CRuDataStore_Base::CopyTo(CRuDataStore_Base *dstDataStore, const char *copiedTableName)
{
	INT32 numTables = this->GetNumTables();

	INT32 keyBufferSize = 128;
	BYTE *key = ruNEW BYTE [keyBufferSize];

	for(INT32 i = 0; i < numTables; ++i)
	{
		const char *tableName = this->GetTableNameByIndex(i);

		if(stricmp(copiedTableName, tableName) == 0)
		{
			INT32 tableKeyLength = this->GetTableKeyLengthByIndex(i);

			INT32 srcTableHandle = this->GetTableHandle(tableName);
			INT32 dstTableHandle = dstDataStore->CreateTable(tableName, tableKeyLength);

			INT32 numEntries = this->GetNumRecords(srcTableHandle);

			if(tableKeyLength > keyBufferSize)
			{
				keyBufferSize = tableKeyLength;
				delete[] key;
				key = ruNEW BYTE [keyBufferSize];
			}

			for(INT32 j = 0; j < numEntries; ++j)
			{
				// Retrieve key and data
				IRuStream *stream = this->RetrieveByIndex(srcTableHandle, j);

				if(stream)
				{
					this->RetrieveKeyByIndex(srcTableHandle, j, key);
					dstDataStore->Store(dstTableHandle, key, stream);
					delete stream;
				}
			}
		}
	}

	delete[] key;

	return TRUE;
}

CRuDataStore_CopyToState *CRuDataStore_Base::CopyTo_Begin(CRuDataStore_Base *dstDataStore)
{
	CRuDataStore_CopyToState *copyToState = ruNEW CRuDataStore_CopyToState();

	copyToState->m_dstDataStore = dstDataStore;
	copyToState->m_keyBufferSize = 128;
	copyToState->m_keyBuffer = ruNEW BYTE [copyToState->m_keyBufferSize];

	copyToState->m_numTables = this->GetNumTables();

	// Start with table at index zero
	copyToState->m_curTableIdx = 0;
	copyToState->m_curEntryIdx = 0;

	return copyToState;
}

BOOL CRuDataStore_Base::CopyTo_Step(CRuDataStore_CopyToState *copyToState)
{
	INT32 maxCmdCount = 25;
	INT32 cmdCount = 0;

	for( ; copyToState->m_curTableIdx < copyToState->m_numTables; ++copyToState->m_curTableIdx)
	{
		// Update the table info if we are at the first entry
		if(copyToState->m_curEntryIdx == 0)
		{
			CopyTo_UpdateCurrentTableInfo(copyToState);
		}

		// Iterate through all entries in the table
		for( ; copyToState->m_curEntryIdx < copyToState->m_numEntries && cmdCount < maxCmdCount; ++copyToState->m_curEntryIdx, ++cmdCount)
		{
			// Retrieve key and data
			IRuStream *stream = this->RetrieveByIndex(copyToState->m_srcTableHandle, copyToState->m_curEntryIdx);

			if(stream)
			{
				this->RetrieveKeyByIndex(copyToState->m_srcTableHandle, copyToState->m_curEntryIdx, copyToState->m_keyBuffer);
				copyToState->m_dstDataStore->Store(copyToState->m_dstTableHandle, copyToState->m_keyBuffer, stream);
				delete stream;
			}
		}

		// Update progress
		REAL sliceSize = 1.0f / copyToState->m_numTables;
		copyToState->m_copyProgress = ((REAL) copyToState->m_curTableIdx / copyToState->m_numTables) + ((REAL) copyToState->m_curEntryIdx / copyToState->m_numEntries * sliceSize);

		// We have not yet reached the end of the entries
		if(copyToState->m_curEntryIdx < copyToState->m_numEntries)
		{
			return TRUE;
		}

		// We have reached the end of the current table, reset
		copyToState->m_curEntryIdx = 0;
	}

	// Have we reached the end of all tables?
	if(copyToState->m_curTableIdx >= copyToState->m_numTables)
	{
		copyToState->m_done = TRUE;
	}

	return TRUE;
}

BOOL CRuDataStore_Base::CopyTo_End(CRuDataStore_CopyToState *copyToState)
{
	delete copyToState;
	return TRUE;
}

void CRuDataStore_Base::CopyTo_UpdateCurrentTableInfo(CRuDataStore_CopyToState *copyToState)
{
	const char *tableName = this->GetTableNameByIndex(copyToState->m_curTableIdx);

	copyToState->m_tableKeyLength = this->GetTableKeyLengthByIndex(copyToState->m_curTableIdx);
	copyToState->m_srcTableHandle = this->GetTableHandle(tableName);
	copyToState->m_dstTableHandle = copyToState->m_dstDataStore->CreateTable(tableName, copyToState->m_tableKeyLength);

	copyToState->m_numEntries = this->GetNumRecords(copyToState->m_srcTableHandle);

	if(copyToState->m_tableKeyLength > copyToState->m_keyBufferSize)
	{
		copyToState->m_keyBufferSize = copyToState->m_tableKeyLength;
		delete[] copyToState->m_keyBuffer;
		copyToState->m_keyBuffer = ruNEW BYTE [copyToState->m_keyBufferSize];
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
