#pragma once
#include "../../Core/Collection/RuCollections.h"
//#include "../Collection/RuArrayList.h"
//#include "../Collection/RuStack.h"
//#include "../Exception/RuExceptions.h"
//#include "../Tool/RuHashGenerator.h"
//#include "../Type/RuString.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

typedef boost::function<INT32 (const char *, void *, void *)> CRuStringHashEnumerateCallbackFunctorNew;		// Parameters: (key, data, userData)

/*!
	CRuStringHash

	@version 2011.11.08
*/
template <class DataType>
class CRuStringHashNew
{
protected:
	struct EntryDesc
	{
		char*								m_key;
		DWORD								m_hash;
		DataType							m_data;
		EntryDesc*							m_next;
	};

protected:
	REAL									m_maxLoadFactor;

	DWORD									m_hashMask;
	INT32									m_tableSize;
	INT32									m_elementCount;

	EntryDesc**								m_table;
	EntryDesc*								m_unusedEntries;

	struct
	{
		UINT8								m_noInternalStore		: 1;								//!< No internal store of strings, rely on external storage
	};

public:
											CRuStringHashNew(INT32 tableSize = 16, INT32 externalStore = FALSE);
											~CRuStringHashNew();

	INT32									Add(const char *key, DataType data);
	INT32									Replace(const char *key, DataType data);
	INT32									Remove(const char *key);
	const char*								GetInternalKey(const char *key);
	DataType&								Get(const char *key);
	INT32									Get(const char *key, DataType &dataOut);
	INT32									Get(const char *key, DataType *&dataOut);
	INT32									Set(const char *key, DataType data);
	INT32									ContainsKey(const char *key) const;

	void									Clear(INT32 jettisonLists = FALSE);

	void									Rehash(INT32 newTableSize);

	void									Enumerate(CRuArrayList<DataType> &dataListOut);
	void									Enumerate(CRuArrayList<DataType *> &dataListOut);
	INT32									EnumerateKeys(CRuArrayList<const char *> &keysOut);
	INT32									ForAllElements(CRuStringHashEnumerateCallbackFunctorNew callback, void *userData);
	INT32									ForAllElements(CRuStringHashEnumerateCallbackFunctorNew callback, void *userData, INT32 startSeed);

	// Status reporting
	INT32									Count() const;
	INT32									GetTableSize() const;
	float									GetLoadFactor() const;

protected:
	void									AllocateTable(INT32 tableSize);

	inline EntryDesc*						AllocateEntry()
	{
		if(m_unusedEntries)
		{
			EntryDesc *newEntry = m_unusedEntries;
			m_unusedEntries = m_unusedEntries->m_next;
			return newEntry;
		}

		return ruNEW EntryDesc;
	}

	inline void								DeallocateEntry(EntryDesc *entry)
	{
		entry->m_next = m_unusedEntries;
		m_unusedEntries = entry;
	}
};

// ************************************************************************************************************************************************************

template <class DataType>
inline CRuStringHashNew<DataType>::CRuStringHashNew(INT32 tableSize, INT32 externalStore)
:	m_maxLoadFactor(0.75f),
	m_table(NULL),
	m_unusedEntries(NULL),
	m_noInternalStore(externalStore)
{
	AllocateTable(tableSize);
}

template <class DataType>
inline CRuStringHashNew<DataType>::~CRuStringHashNew()
{
	// Clear table
	Clear();

	// Delete table array
	delete[] m_table;

	// Free all unused entries
	while(m_unusedEntries)
	{
		EntryDesc *entry = m_unusedEntries;
		m_unusedEntries = m_unusedEntries->m_next;

		delete entry;
	}
}

template <class DataType>
inline INT32 CRuStringHashNew<DataType>::Add(const char *key, DataType data)
{
	// Generate key hash
	INT32 keyLength = static_cast<INT32>(strlen(key));
	DWORD keyHash = RuGlobalHashGen().GetHashCode((BYTE *) key, keyLength);

	// Calculate bucket index
	DWORD bucketIdx = keyHash & m_hashMask;

	// Ensure no duplicates exist
	EntryDesc *checkEntry = m_table[bucketIdx];

	while(checkEntry)
	{
		// Ensure no duplicates
		if(strcmp(checkEntry->m_key, key) == 0)
			return FALSE;

		checkEntry = checkEntry->m_next;
	}

	// Create new entry & insert entry into bucket
	EntryDesc *newEntry = AllocateEntry();

	if(m_noInternalStore)
	{
		newEntry->m_key = const_cast<char *>(key);
	}
	else
	{
		newEntry->m_key = ruNEW char [keyLength + 1];
		memcpy(newEntry->m_key, key, keyLength + 1);
	}

	newEntry->m_hash = keyHash;
	newEntry->m_data = data;
	newEntry->m_next = m_table[bucketIdx];

	m_table[bucketIdx] = newEntry;

	// Increment element count
	++m_elementCount;

	if(static_cast<REAL>(m_elementCount) / m_tableSize > m_maxLoadFactor)
	{
		Rehash(m_tableSize * 2);
	}

	return TRUE;
}

template <class DataType>
inline INT32 CRuStringHashNew<DataType>::Replace(const char *key, DataType data)
{
	// Generate key hash
	INT32 keyLength = static_cast<INT32>(strlen(key));
	DWORD keyHash = RuGlobalHashGen().GetHashCode((BYTE *) key, keyLength);

	// Calculate bucket index
	DWORD bucketIdx = keyHash & m_hashMask;

	// Find corresponding entry in current bucket
	EntryDesc *currentEntry = m_table[bucketIdx];

	while(currentEntry)
	{
		if(strcmp(currentEntry->m_key, key) == 0)
		{
			// Overwrite data
			currentEntry->m_data = data;

			return TRUE;
		}

		// Advance to next entry
		currentEntry = currentEntry->m_next;
	}

	return FALSE;
}

template <class DataType>
inline INT32 CRuStringHashNew<DataType>::Remove(const char *key)
{
	// Generate key hash
	INT32 keyLength = static_cast<INT32>(strlen(key));
	DWORD keyHash = RuGlobalHashGen().GetHashCode((BYTE *) key, keyLength);

	// Calculate bucket index
	DWORD bucketIdx = keyHash & m_hashMask;

	// Find corresponding entry in current bucket
	EntryDesc *currentEntry = m_table[bucketIdx];
	EntryDesc *previousEntry = NULL;

	while(currentEntry)
	{
		if(strcmp(currentEntry->m_key, key) == 0)
		{
			if(m_noInternalStore == FALSE)
			{
				delete[] currentEntry->m_key;
			}

			// Detach entry from table
			if(previousEntry)
			{
				previousEntry->m_next = currentEntry->m_next;
			}
			else
			{
				m_table[bucketIdx] = currentEntry->m_next;
			}

			// Deallocate entry
			DeallocateEntry(currentEntry);

			// Decrement element count
			--m_elementCount;

			return TRUE;
		}

		// Advance to next entry
		previousEntry = currentEntry;
		currentEntry = currentEntry->m_next;
	}

	return FALSE;
}

/*!
	const char *CRuStringHashNew::Get(const char *key)

	Retrieves the internal string corresponding to the given key string.

	@param key							The key string for which to retrieve the corresponding internal key string

	@return const char *				The corresponding internal string, or NULL if the key is not in the hash
*/
template <class DataType>
const char *CRuStringHashNew<DataType>::GetInternalKey(const char *key)
{
	// Generate key hash
	INT32 keyLength = static_cast<INT32>(strlen(key));
	DWORD keyHash = RuGlobalHashGen().GetHashCode((BYTE *) key, keyLength);

	// Calculate bucket index
	DWORD bucketIdx = keyHash & m_hashMask;

	// Find corresponding entry in current bucket
	EntryDesc *currentEntry = m_table[bucketIdx];

	while(currentEntry)
	{
		if(strcmp(currentEntry->m_key, key) == 0)
		{
			return currentEntry->m_key;
		}

		// Advance to next entry
		currentEntry = currentEntry->m_next;
	}

	return NULL;
}

template <class DataType>
DataType &CRuStringHashNew<DataType>::Get(const char *key)
{
	// Generate key hash
	INT32 keyLength = static_cast<INT32>(strlen(key));
	DWORD keyHash = RuGlobalHashGen().GetHashCode((BYTE *) key, keyLength);

	// Calculate bucket index
	DWORD bucketIdx = keyHash & m_hashMask;

	// Find corresponding entry in current bucket
	EntryDesc *currentEntry = m_table[bucketIdx];

	while(currentEntry)
	{
		if(strcmp(currentEntry->m_key, key) == 0)
		{
			return currentEntry->m_data;
		}

		// Advance to next entry
		currentEntry = currentEntry->m_next;
	}

	throw CRuArgumentOutOfRangeException();
}

template <class DataType>
inline INT32 CRuStringHashNew<DataType>::Get(const char *key, DataType &dataOut)
{
	// Generate key hash
	INT32 keyLength = static_cast<INT32>(strlen(key));
	DWORD keyHash = RuGlobalHashGen().GetHashCode((BYTE *) key, keyLength);

	// Calculate bucket index
	DWORD bucketIdx = keyHash & m_hashMask;

	// Find corresponding entry in current bucket
	EntryDesc *currentEntry = m_table[bucketIdx];

	while(currentEntry)
	{
		if(strcmp(currentEntry->m_key, key) == 0)
		{
			dataOut = currentEntry->m_data;
			return TRUE;
		}

		// Advance to next entry
		currentEntry = currentEntry->m_next;
	}

	return FALSE;
}

template <class DataType>
INT32 CRuStringHashNew<DataType>::Get(const char *key, DataType *&dataOut)
{
	// Generate key hash
	INT32 keyLength = static_cast<INT32>(strlen(key));
	DWORD keyHash = RuGlobalHashGen().GetHashCode((BYTE *) key, keyLength);

	// Calculate bucket index
	DWORD bucketIdx = keyHash & m_hashMask;

	// Find corresponding entry in current bucket
	EntryDesc *currentEntry = m_table[bucketIdx];

	while(currentEntry)
	{
		if(strcmp(currentEntry->m_key, key) == 0)
		{
			dataOut = &currentEntry->m_data;
			return TRUE;
		}

		// Advance to next entry
		currentEntry = currentEntry->m_next;
	}

	return FALSE;
}

template <class DataType>
inline INT32 CRuStringHashNew<DataType>::Set(const char *key, DataType data)
{
	if(Replace(key, data) == FALSE)
	{
		return Add(key, data);
	}

	return TRUE;
}

template <class DataType>
inline INT32 CRuStringHashNew<DataType>::ContainsKey(const char *key) const
{
	// Generate key hash
	INT32 keyLength = static_cast<INT32>(strlen(key));
	DWORD keyHash = RuGlobalHashGen().GetHashCode((BYTE *) key, keyLength);

	// Calculate bucket index
	DWORD bucketIdx = keyHash & m_hashMask;

	// Find corresponding entry in current bucket
	EntryDesc *currentEntry = m_table[bucketIdx];

	while(currentEntry)
	{
		if(strcmp(currentEntry->m_key, key) == 0)
		{
			return TRUE;
		}

		// Advance to next entry
		currentEntry = currentEntry->m_next;
	}

	return FALSE;
}

template <class DataType>
inline void CRuStringHashNew<DataType>::Clear(INT32 jettisonLists)
{
	for(INT32 i = 0; i < m_tableSize; ++i)
	{
		EntryDesc *currentEntry = m_table[i];

		while(currentEntry)
		{
			EntryDesc *deallocEntry = currentEntry;
			currentEntry = currentEntry->m_next;

			// Free string key
			if(m_noInternalStore == FALSE)
			{
				delete[] deallocEntry->m_key;
			}

			DeallocateEntry(deallocEntry);
		}
	}

	memset(m_table, 0, sizeof(EntryDesc *) * m_tableSize);

	m_elementCount = 0;

	if(jettisonLists)
	{
		// Free all unused entries
		while(m_unusedEntries)
		{
			EntryDesc *entry = m_unusedEntries;
			m_unusedEntries = m_unusedEntries->m_next;

			delete entry;
		}
	}
}

template <class DataType>
void CRuStringHashNew<DataType>::Rehash(INT32 newTableSize)
{
	// ...
	EntryDesc **oldTable = m_table;
	INT32 oldTableSize = m_tableSize;

	// Allocate new table
	AllocateTable(newTableSize);

	for(INT32 i = 0; i < oldTableSize; ++i)
	{
		EntryDesc *currentEntry = oldTable[i];

		while(currentEntry)
		{
			// Detach rehashed entry from old table
			EntryDesc *rehashedEntry = currentEntry;
			currentEntry = currentEntry->m_next;

			// Calculate target bucket
			DWORD bucketIdx = rehashedEntry->m_hash & m_hashMask;

			// Insert at head of bucket
			rehashedEntry->m_next = m_table[bucketIdx];
			m_table[bucketIdx] = rehashedEntry;

			// ...
			++m_elementCount;
		}
	}

	// Release old table
	delete[] oldTable;
}

template <class DataType>
inline void CRuStringHashNew<DataType>::Enumerate(CRuArrayList<DataType> &dataListOut)
{
	for(INT32 i = 0; i < m_tableSize; ++i)
	{
		EntryDesc *currentEntry = m_table[i];

		while(currentEntry)
		{
			dataListOut.Add(currentEntry->m_data);

			currentEntry = currentEntry->m_next;
		}
	}
}

template <class DataType>
inline void CRuStringHashNew<DataType>::Enumerate(CRuArrayList<DataType *> &dataListOut)
{
	for(INT32 i = 0; i < m_tableSize; ++i)
	{
		EntryDesc *currentEntry = m_table[i];

		while(currentEntry)
		{
			dataListOut.Add(&currentEntry->m_data);

			currentEntry = currentEntry->m_next;
		}
	}
}

template <class DataType>
INT32 CRuStringHashNew<DataType>::EnumerateKeys(CRuArrayList<const char *> &keysOut)
{
	for(INT32 i = 0; i < m_tableSize; ++i)
	{
		EntryDesc *currentEntry = m_table[i];

		while(currentEntry)
		{
			keysOut.Add(currentEntry->m_key);

			currentEntry = currentEntry->m_next;
		}
	}

	return TRUE;
}

template <class DataType>
inline INT32 CRuStringHashNew<DataType>::ForAllElements(CRuStringHashEnumerateCallbackFunctorNew callback, void *userData)
{
	for(INT32 i = 0; i < m_tableSize; ++i)
	{
		EntryDesc *currentEntry = m_table[i];

		while(currentEntry)
		{
			if(!callback(currentEntry->m_key, &currentEntry->m_data, userData))
				return FALSE;

			currentEntry = currentEntry->m_next;
		}
	}

	return TRUE;
}

template <class DataType>
inline INT32 CRuStringHashNew<DataType>::ForAllElements(CRuStringHashEnumerateCallbackFunctorNew callback, void *userData, INT32 startSeed)
{
	startSeed = startSeed % m_tableSize;

	for(INT32 i = startSeed; i < m_tableSize; ++i)
	{
		EntryDesc *currentEntry = m_table[i];

		while(currentEntry)
		{
			if(!callback(currentEntry->m_key, &currentEntry->m_data, userData))
				return i;

			currentEntry = currentEntry->m_next;
		}
	}

	for(INT32 i = 0; i < startSeed; ++i)
	{
		EntryDesc *currentEntry = m_table[i];

		while(currentEntry)
		{
			if(!callback(currentEntry->m_key, &currentEntry->m_data, userData))
				return i;

			currentEntry = currentEntry->m_next;
		}
	}

	return startSeed;
}

template <class DataType>
inline INT32 CRuStringHashNew<DataType>::Count() const
{
	return m_elementCount;
}

template <class DataType>
inline INT32 CRuStringHashNew<DataType>::GetTableSize() const
{
	return m_tableSize;
}

template <class DataType>
inline float CRuStringHashNew<DataType>::GetLoadFactor() const
{
	return (float) m_elementCount / (float) m_tableSize;
}

template <class DataType>
void CRuStringHashNew<DataType>::AllocateTable(INT32 tableSize)
{
	// Round size up to a power of 2
	m_tableSize = 0x00000001;
	int nonZeroBits = 0;
	while(tableSize != 0)
	{
		if(tableSize & 0x00000001)
			nonZeroBits++;

		tableSize = tableSize >> 1;
		m_tableSize = m_tableSize << 1;
	}
	m_tableSize = m_tableSize >> 1;

	if(nonZeroBits > 1)
		m_tableSize = m_tableSize << 1;

	// Store hash mask
	m_hashMask = m_tableSize - 1;

	// Allocate table
	m_table = ruNEW EntryDesc * [m_tableSize];
	memset(m_table, 0, sizeof(EntryDesc *) * m_tableSize);

	// Reset count
	m_elementCount = 0;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
