#ifndef _RUCOLLECTIONSHASH_H_
#define _RUCOLLECTIONSHASH_H_

#include "../../Core/Collection/RuCollections.h"

#pragma unmanaged

/*!
	CRuHashMap
	@author Ceranos
	@version Initial
*/
typedef BOOL(* CRuHashMapEnumerateCallBack)(const void *key, const void *data, void *userData);
typedef boost::function<BOOL (const void *, void *)> CRuHashMapEnumerateCallbackFunctor;		// Parameters: (key, data)

template <class KeyType, class DataType>
class CRuHashMap
{
protected:
	struct STableEntry
	{
		KeyType								Key;
		DataType							Data;
	};

	DWORD									m_hashMask;
	INT32									m_tableSize;
	INT32									m_elementCount;
	CRuArrayList<STableEntry>**				m_table;
	CRuStack<CRuArrayList<STableEntry> *>	m_emptyLists;			// Stack of unused array lists

public:
							CRuHashMap(INT32 tableSize);
							~CRuHashMap();

	BOOL					Add(const KeyType &key, DataType data) { return Add(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key)), data); }
	void					Remove(const KeyType &key) { Remove(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key))); }
	DataType				Get(const KeyType &key) { return Get(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key))); }
	BOOL					ContainsKey(const KeyType &key) const { return ContainsKey(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key))); }

	BOOL					Add(const KeyType &key, DWORD keyHash, DataType data);
	BOOL					Replace(const KeyType &key, DWORD keyHash, DataType data);
	void					Remove(const KeyType &key, DWORD keyHash);
	DataType				Get(const KeyType &key, DWORD keyHash);
	BOOL					Get(const KeyType &key, DWORD keyHash, DataType &dataOut);
	BOOL					ContainsKey(const KeyType &key, DWORD keyHash) const;

	void					Clear();

	BOOL					ForAllElements(CRuHashMapEnumerateCallBack callback, void *userData);
	BOOL					ForAllElements(CRuHashMapEnumerateCallbackFunctor callback);

	// Status reporting
	INT32					Count() const { return m_elementCount; }
	INT32					GetTableSize() const { return m_tableSize; }
	float					GetLoadFactor() const { return (float) m_elementCount / (float) m_tableSize; }
};

// ************************************************************************************************************************************************************

/*!
	CRuHashMapEx
	@author Ceranos
	@version Initial
*/
template <class KeyType, class DataType>
class CRuHashMapEx
{
protected:
	struct STableEntry
	{
		BOOL								m_isOccupied;
		INT32								m_chainIndex;
		KeyType								m_key;
		DataType							m_data;
	};

	DWORD									m_hashMask;
	INT32									m_tableSize;
	INT32									m_elementCount;
	STableEntry*							m_table;

public:
							CRuHashMapEx(INT32 tableSize);
							~CRuHashMapEx();

	BOOL					Add(const KeyType &key, DataType data) { return Add(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key)), data); }
	void					Remove(const KeyType &key) { Remove(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key))); }
//	DataType				Get(const KeyType &key) { return Get(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key))); }
	BOOL					ContainsKey(const KeyType &key) const { return ContainsKey(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key))); }

	BOOL					Add(const KeyType &key, DWORD keyHash, DataType data);
//	BOOL					Replace(const KeyType &key, DWORD keyHash, DataType data);
	void					Remove(const KeyType &key, DWORD keyHash);
//	DataType				Get(const KeyType &key, DWORD keyHash);
	BOOL					Get(const KeyType &key, DWORD keyHash, DataType &dataOut);
	BOOL					ContainsKey(const KeyType &key, DWORD keyHash) const;

	void					Clear();

	BOOL					ForAllElements(CRuHashMapEnumerateCallBack callback, void *userData);
	BOOL					ForAllElements(CRuHashMapEnumerateCallbackFunctor callback);

	// Status reporting
	INT32					Count() const { return m_elementCount; }
	INT32					GetTableSize() const { return m_tableSize; }
	float					GetLoadFactor() const { return (float) m_elementCount / (float) m_tableSize; }
};

// ************************************************************************************************************************************************************

/*!
	CRuKeyHash
	@author Ceranos
	@version Initial
*/
template <class KeyType>
class CRuKeyHash
{
protected:
	DWORD							m_hashMask;
	INT32							m_tableSize;
	INT32							m_elementCount;
	CRuArrayList<KeyType>**			m_table;
	CRuStack<CRuArrayList<KeyType> *>	m_emptyLists;			// Stack of unused array lists

public:
							CRuKeyHash(INT32 tableSize);
							~CRuKeyHash();

	inline BOOL				Add(const KeyType &key) { return Add(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key))); }
	inline void				Remove(const KeyType &key) { Remove(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key))); }
	inline BOOL				ContainsKey(const KeyType &key) const { return ContainsKey(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key))); }

	BOOL					Add(const KeyType &key, DWORD keyHash);
	void					Remove(const KeyType &key, DWORD keyHash);
	BOOL					ContainsKey(const KeyType &key, DWORD keyHash) const;

	void					Clear();

	// Status reporting
	INT32					Count() const { return m_elementCount; }
	INT32					GetTableSize() const { return m_tableSize; }
	float					GetLoadFactor() const { return (float) m_elementCount / (float) m_tableSize; }
	float					GetCollisionFactor();
	float					GetAverageChainLength();
	INT32					GetMaximumChainLength();
};

// ************************************************************************************************************************************************************

/*!
	CRuStringHash
	@author John Tang
	@version 2004/07/17
*/
typedef BOOL(* CRuStringHashEnumerateCallBack)(const void *key, const void *data, void *userData);
typedef boost::function<BOOL (const void *, void *)> CRuStringHashEnumerateCallbackFunctor;		// Parameters: (key, data)

template <class DataType>
class CRuStringHash
{
protected:
	DWORD									m_hashMask;
	INT32									m_tableSize;
	INT32									m_elementCount;

	CRuArrayList<CRuString>**				m_keyTable;
	CRuArrayList<DataType>**				m_valueTable;

public:
							CRuStringHash(INT32 tableSize);
							~CRuStringHash();

	BOOL					Add(const char *key, DataType data);
	BOOL					Replace(const char *key, DataType data);
	BOOL					Remove(const char *key);
	BOOL					Get(const char *key, DataType &dataOut);
	BOOL					Set(const char *key, DataType data);
	BOOL					ContainsKey(const char *key) const;

	BOOL					Add(const CRuString &key, DataType data);
	BOOL					Replace(const CRuString &key, DataType data);
	BOOL					Remove(const CRuString &key);
	BOOL					Get(const CRuString &key, DataType &dataOut);
	BOOL					Set(const CRuString &key, DataType data);
	BOOL					ContainsKey(const CRuString &key) const;

	void					Clear();

	BOOL					ForAllElements(CRuStringHashEnumerateCallBack callback, void *userData);
	BOOL					ForAllElements(CRuStringHashEnumerateCallbackFunctor callback);

	// Status reporting
	INT32					Count() const;
	INT32					GetTableSize() const;
	float					GetLoadFactor() const;
};

// ************************************************************************************************************************************************************

/*
--------------------------------------------------------------------
CRuHashMap

Beginning of function definition for templatized

--------------------------------------------------------------------
*/

template <class KeyType, class DataType>
CRuHashMap<KeyType, DataType>::CRuHashMap(INT32 tableSize)
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

	m_hashMask = m_tableSize - 1;

	// Allocate table
	m_table = ruNEW CRuArrayList<STableEntry> * [m_tableSize];
	for(INT32 i = 0;i < m_tableSize;i++)
		m_table[i] = NULL;

	m_elementCount = 0;
}

template <class KeyType, class DataType>
CRuHashMap<KeyType, DataType>::~CRuHashMap()
{
	Clear();
	delete[] m_table;

	while(!m_emptyLists.IsEmpty())
	{
		delete m_emptyLists.Pop();
	}
}

template <class KeyType, class DataType>
BOOL CRuHashMap<KeyType, DataType>::Add(const KeyType &key, DWORD keyHash, DataType data)
{
	DWORD index = keyHash & m_hashMask;

	// Allocate an array list at the hash entry if there isn't one
	if(m_table[index] == NULL)
	{
		if(m_emptyLists.IsEmpty())
			m_table[index] = ruNEW CRuArrayList<STableEntry>(4);
		else
			m_table[index] = m_emptyLists.Pop();
	}

	// Search through the array list for a duplicate
	for(INT32 i = 0; i < m_table[index]->Count(); i++)
	{
		if(m_table[index]->Get(i).Key == key)
			return FALSE;
	}

	// Add key/data pair to the table
	STableEntry entry;
	entry.Key = key;
	entry.Data = data;
	m_table[index]->Add(entry);

	m_elementCount++;

	return TRUE;
}

template <class KeyType, class DataType>
BOOL CRuHashMap<KeyType, DataType>::Replace(const KeyType &key, DWORD keyHash, DataType data)
{
	DWORD index = keyHash & m_hashMask;

	// Return if the key does not exist in the table
	if(m_table[index] == NULL)
		return FALSE;

	// Search through the array list for the key
	for(INT32 i = 0; i < m_table[index]->Count(); i++)
	{
		if(m_table[index]->Get(i).Key == key)
		{
			m_table[index]->Get(i).Data = data;

			return TRUE;
		}
	}

	return FALSE;
}

template <class KeyType, class DataType>
void CRuHashMap<KeyType, DataType>::Remove(const KeyType &key, DWORD keyHash)
{
	DWORD index = keyHash & m_hashMask;

	// Return if the key does not exist in the table
	if(m_table[index] == NULL)
		throw CRuArgumentOutOfRangeException();

	// Search through the array list for the key
	for(INT32 i = 0; i < m_table[index]->Count(); i++)
	{
		if(m_table[index]->Get(i).Key == key)
		{
			m_table[index]->RemoveAt(i);
			m_elementCount--;

			if(m_table[index]->Count() == 0)
			{
				m_emptyLists.Push(m_table[index]);
				m_table[index] = NULL;
			}

			return;
		}
	}

#ifndef KALYDO
	// throw only in client version, kalydo version may throw this unneccesary.
	throw CRuArgumentOutOfRangeException();
#endif // KALYDO 
}

template <class KeyType, class DataType>
DataType CRuHashMap<KeyType, DataType>::Get(const KeyType &key, DWORD keyHash)
{
	DWORD index = keyHash & m_hashMask;

	// Return if the key does not exist in the table
	if(m_table[index] == NULL)
		throw CRuArgumentOutOfRangeException();

	// Search through the array list for the key
	for(INT32 i = 0; i < m_table[index]->Count(); i++)
	{
		if(m_table[index]->Get(i).Key == key)
			return m_table[index]->Get(i).Data;
	}

	throw CRuArgumentOutOfRangeException();
}

template <class KeyType, class DataType>
inline BOOL CRuHashMap<KeyType, DataType>::Get(const KeyType &key, DWORD keyHash, DataType &dataOut)
{
	DWORD index = keyHash & m_hashMask;

	if(m_table[index] == NULL)
		return FALSE;

	for(INT32 i = 0; i < m_table[index]->Count(); ++i)
	{
		if(m_table[index]->Get(i).Key == key)
		{
			dataOut = m_table[index]->Get(i).Data;
			return TRUE;
		}
	}

	return FALSE;
}

template <class KeyType, class DataType>
BOOL CRuHashMap<KeyType, DataType>::ContainsKey(const KeyType &key, DWORD keyHash) const
{
	DWORD index = keyHash & m_hashMask;

	// Return if the key does not exist in the table
	if(m_table[index] == NULL)
		return FALSE;

	// Search through the array list for the key	
	INT32 entries = m_table[index]->Count();
	for(INT32 i = 0; i < entries; i++)
	{
		if(m_table[index]->Get(i).Key == key)
			return TRUE;
	}

	return FALSE;
}

template <class KeyType, class DataType>
void CRuHashMap<KeyType, DataType>::Clear()
{
	for(INT32 i = 0; i < m_tableSize; i++)
	{
		delete m_table[i];
		m_table[i] = NULL;
	}

	m_elementCount = 0;
}

template <class KeyType, class DataType>
BOOL CRuHashMap<KeyType, DataType>::ForAllElements(CRuHashMapEnumerateCallBack callback, void *userData)
{
	for(INT32 i = 0; i < m_tableSize; i++)
	{
		if(m_table[i])
		{
			for(INT32 j = 0; j < m_table[i]->Count(); j++)
			{
				STableEntry &entry = m_table[i]->Get(j);

				if(!callback(&entry.Key, &entry.Data, userData))
					return FALSE;
			}
		}
	}

	return TRUE;
}

template <class KeyType, class DataType>
BOOL CRuHashMap<KeyType, DataType>::ForAllElements(CRuHashMapEnumerateCallbackFunctor callback)
{
	for(INT32 i = 0; i < m_tableSize; i++)
	{
		if(m_table[i])
		{
			for(INT32 j = 0; j < m_table[i]->Count(); j++)
			{
				STableEntry &entry = m_table[i]->Get(j);

				if(!callback(&entry.Key, &entry.Data))
					return FALSE;
			}
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

template <class KeyType, class DataType>
CRuHashMapEx<KeyType, DataType>::CRuHashMapEx(INT32 tableSize)
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

	m_hashMask = m_tableSize - 1;

	// Allocate table
	m_table = ruNEW STableEntry [m_tableSize];
	for(INT32 i = 0; i < m_tableSize; i++)
	{
		m_table[i].m_isOccupied = FALSE;
	}

	m_elementCount = 0;
}

template <class KeyType, class DataType>
CRuHashMapEx<KeyType, DataType>::~CRuHashMapEx()
{
	delete[] m_table;
}

template <class KeyType, class DataType>
BOOL CRuHashMapEx<KeyType, DataType>::Add(const KeyType &key, DWORD keyHash, DataType data)
{
	DWORD index = keyHash & m_hashMask;

	if(m_table[index].m_isOccupied)
	{
		// Go to the end of the chain
		while(m_table[index].m_chainIndex != index)
		{
			// Ensure the new entry is not a duplicate
			if(m_table[index].m_key == key)
				return FALSE;

			// Advance to next entry in the chain
			index = m_table[index].m_chainIndex;
		}

		DWORD lastIndex = index;

		// Ensure the new entry is not a duplicate of the last entry
		if(m_table[index].m_key == key)
			return FALSE;

		// Find the next available slot to chain
		while(m_table[index].m_isOccupied)
		{
			++index;
			
			index = index & m_hashMask;
		}

		m_table[lastIndex].m_chainIndex = index;

		// Insert into the table
		m_table[index].m_isOccupied = TRUE;
		m_table[index].m_chainIndex = index;
		m_table[index].m_key = key;
		m_table[index].m_data = data;
	}
	else
	{
		// Insert directly into the table
		m_table[index].m_isOccupied = TRUE;
		m_table[index].m_chainIndex = index;
		m_table[index].m_key = key;
		m_table[index].m_data = data;
	}

	++m_elementCount;

	return TRUE;
}

template <class KeyType, class DataType>
void CRuHashMapEx<KeyType, DataType>::Remove(const KeyType &key, DWORD keyHash)
{
	DWORD index = keyHash & m_hashMask;
	DWORD deletedIndex, nextToLastIndex, lastIndex;

	// PROBLEM: Say the chaining starts at entry 10, and what we are deleting is at entry 2.
	//          Entry 10 links directly to entry 2, and there are no entries linked to entry 2.
	//          In this case, entry 10 retains its link to entry 2 and everything gets fubar'ed.

	while(m_table[index].m_isOccupied)
	{
		if(m_table[index].m_key == key)
		{
			if(m_table[index].m_chainIndex == index)
			{
				m_table[index].m_isOccupied = FALSE;
				return;
			}

			// Find last and next to last entries in the chain
			deletedIndex = index;
			nextToLastIndex = index;
			lastIndex = m_table[nextToLastIndex].m_chainIndex;

			while(m_table[lastIndex].m_chainIndex != lastIndex)
			{
				nextToLastIndex = lastIndex;
				lastIndex = m_table[nextToLastIndex].m_chainIndex;
			}

			// Copy last entry into the deleted entry's slot
			m_table[deletedIndex].m_key = m_table[lastIndex].m_key;
			m_table[deletedIndex].m_data = m_table[lastIndex].m_data;

			// Mark next to last entry has not having a chain
			m_table[nextToLastIndex].m_chainIndex = nextToLastIndex;

			// Mark last entry as unoccupied
			m_table[lastIndex].m_isOccupied = FALSE;

			return;
		}

		if(m_table[index].m_chainIndex == index)
			return;

		index = m_table[index].m_chainIndex;
	}
}

template <class KeyType, class DataType>
inline BOOL CRuHashMapEx<KeyType, DataType>::Get(const KeyType &key, DWORD keyHash, DataType &dataOut)
{
	DWORD index = keyHash & m_hashMask;

	while(m_table[index].m_isOccupied)
	{
		if(m_table[index].m_key == key)
		{
			dataOut = m_table[index].m_data;
			return TRUE;
		}

		if(m_table[index].m_chainIndex == index)
			return FALSE;

		index = m_table[index].m_chainIndex;
	}

	return FALSE;
}

template <class KeyType, class DataType>
inline BOOL CRuHashMapEx<KeyType, DataType>::ContainsKey(const KeyType &key, DWORD keyHash) const
{
	DWORD index = keyHash & m_hashMask;

	while(m_table[index].m_isOccupied)
	{
		if(m_table[index].m_key == key)
			return TRUE;

		if(m_table[index].m_chainIndex == index)
			return FALSE;

		index = m_table[index].m_chainIndex;
	}

	return FALSE;
}

template <class KeyType, class DataType>
void CRuHashMapEx<KeyType, DataType>::Clear()
{
	for(INT32 i = 0; i < m_tableSize; i++)
	{
		m_table[i].m_isOccupied = FALSE;
	}
}

template <class KeyType, class DataType>
BOOL CRuHashMapEx<KeyType, DataType>::ForAllElements(CRuHashMapEnumerateCallBack callback, void *userData)
{
	for(INT32 i = 0; i < m_tableSize; i++)
	{
		if(m_table[i].m_isOccupied)
		{
			if(!callback(&m_table[i].m_key, &m_table[i].m_data, userData))
				return FALSE;
		}
	}

	return TRUE;
}

template <class KeyType, class DataType>
BOOL CRuHashMapEx<KeyType, DataType>::ForAllElements(CRuHashMapEnumerateCallbackFunctor callback)
{
	for(INT32 i = 0; i < m_tableSize; i++)
	{
		if(m_table[i].m_isOccupied)
		{
			if(!callback(&m_table[i].m_key, &m_table[i].m_data))
				return FALSE;
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

/*
--------------------------------------------------------------------
CRuKeyHash

Beginning of function definition for templatized CRuKeyHash

--------------------------------------------------------------------
*/

template <class KeyType>
CRuKeyHash<KeyType>::CRuKeyHash(INT32 tableSize)
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

	m_hashMask = m_tableSize - 1;

	// Allocate table
	m_table = ruNEW CRuArrayList<KeyType> * [m_tableSize];
	for(INT32 i = 0;i < m_tableSize;i++)
		m_table[i] = NULL;

	m_elementCount = 0;
}

template <class KeyType>
CRuKeyHash<KeyType>::~CRuKeyHash()
{
	Clear();
	delete[] m_table;

	while(m_emptyLists.IsEmpty() == FALSE)
	{
		delete m_emptyLists.Pop();
	}
}

template <class KeyType>
BOOL CRuKeyHash<KeyType>::Add(const KeyType &key, DWORD keyHash)
{
	DWORD index = keyHash & m_hashMask;

	// Allocate an array list at the hash entry if there isn't one
	if(m_table[index] == NULL)
	{
		if(m_emptyLists.IsEmpty())
			m_table[index] = ruNEW CRuArrayList<KeyType>(4);
		else
			m_table[index] = m_emptyLists.Pop();
	}

	// Search through the array list for a duplicate
	for(INT32 i = 0; i < m_table[index]->Count(); i++)
	{
		if(m_table[index]->Get(i) == key)
			return FALSE;
	}

	// Add key/data pair to the table
	m_table[index]->Add(key);

	m_elementCount++;

	return TRUE;
}

template <class KeyType>
void CRuKeyHash<KeyType>::Remove(const KeyType &key, DWORD keyHash)
{
	DWORD index = keyHash & m_hashMask;
	//TODO: Краш после фикса краша в 14 зоне
	// Return if the key does not exist in the table
	if(m_table[index] == NULL)
	{
		throw CRuArgumentOutOfRangeException();
	}
	// Search through the array list for the key
	for(INT32 i = 0; i < m_table[index]->Count(); i++)
	{
		if(m_table[index]->Get(i) == key)
		{
			m_table[index]->RemoveAt(i);
			m_elementCount--;

			if(m_table[index]->Count() == 0)
			{
				m_emptyLists.Push(m_table[index]);
				m_table[index] = NULL;
			}

			return;
		}
	}

#ifndef KALYDO
	throw CRuArgumentOutOfRangeException();
#endif
}

template <class KeyType>
BOOL CRuKeyHash<KeyType>::ContainsKey(const KeyType &key, DWORD keyHash) const
{
	DWORD index = keyHash & m_hashMask;

	// Return if the key does not exist in the table
	if(m_table[index] == NULL)
		return FALSE;

	// Search through the array list for the key
	for(INT32 i = 0; i < m_table[index]->Count(); i++)
	{
		if(m_table[index]->Get(i) == key)
		{
			return TRUE;
		}
	}

	return FALSE;
}

template <class KeyType>
void CRuKeyHash<KeyType>::Clear()
{
	for(INT32 i = 0; i < m_tableSize; i++)
	{
		if(m_table[i])
		{
			m_table[i]->Clear();
			m_emptyLists.Push(m_table[i]);
			m_table[i] = NULL;
		}
	}

	m_elementCount = 0;
}

template <class KeyType>
float CRuKeyHash<KeyType>::GetCollisionFactor()
{
	INT32 numCollisions = 0;

	for(INT32 i = 0; i < m_tableSize; i++)
	{
		if(m_table[i] && m_table[i]->Count() > 1)
			numCollisions += (m_table[i]->Count() - 1);
	}

	return (float) numCollisions / m_elementCount;
}

template <class KeyType>
float CRuKeyHash<KeyType>::GetAverageChainLength()
{
	INT32 numChains = 0, totalLength = 0;

	for(INT32 i = 0; i < m_tableSize; i++)
	{
		if(m_table[i] && m_table[i]->Count() > 0)
		{
			++numChains;
			totalLength += m_table[i]->Count();
		}
	}

	if(numChains > 0)
		return (float) totalLength / numChains;

	return 0.0f;
}

template <class KeyType>
INT32 CRuKeyHash<KeyType>::GetMaximumChainLength()
{
	INT32 maxLength = 0;

	for(INT32 i = 0; i < m_tableSize; i++)
	{
		if(m_table[i] && m_table[i]->Count() > maxLength)
			maxLength = m_table[i]->Count();
	}

	return maxLength;
}

// ************************************************************************************************************************************************************

template <class DataType>
inline CRuStringHash<DataType>::CRuStringHash(INT32 tableSize)
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

	m_hashMask = m_tableSize - 1;

	// Allocate tables
	m_keyTable = ruNEW CRuArrayList<CRuString> * [m_tableSize];
	m_valueTable = ruNEW CRuArrayList<DataType> * [m_tableSize];
	for(INT32 i = 0; i < m_tableSize; i++)
	{
		m_keyTable[i] = NULL;
		m_valueTable[i] = NULL;
	}

	m_elementCount = 0;
}

template <class DataType>
inline CRuStringHash<DataType>::~CRuStringHash()
{
	Clear();

	delete[] m_keyTable;
	delete[] m_valueTable;
}

template <class DataType>
inline BOOL CRuStringHash<DataType>::Add(const char *key, DataType data)
{
	return Add(CRuString(key), data);
}

template <class DataType>
inline BOOL CRuStringHash<DataType>::Replace(const char *key, DataType data)
{
	return Replace(CRuString(key), data);
}

template <class DataType>
inline BOOL CRuStringHash<DataType>::Remove(const char *key)
{
	return Remove(CRuString(key));
}

template <class DataType>
inline BOOL CRuStringHash<DataType>::Get(const char *key, DataType &dataOut)
{
	INT32 keyLength = (INT32)strlen(key);
	DWORD hashCode = RuGlobalHashGen().GetHashCode((BYTE *) key, keyLength);

	DWORD index = hashCode & m_hashMask;

	// Return if the key does not exist in the table
	if(m_keyTable[index] == NULL)
		return FALSE;

	for(INT32 i = 0; i < m_keyTable[index]->Count(); i++)
	{
		if(m_keyTable[index]->Get(i) == key)
		{
			dataOut = m_valueTable[index]->Get(i);
			return TRUE;
		}
	}

	return FALSE;

//	return Get(CRuString(key), dataOut);
}

template <class DataType>
inline BOOL CRuStringHash<DataType>::Set(const char *key, DataType data)
{
	INT32 keyLength = (INT32)strlen(key);
	DWORD hashCode = RuGlobalHashGen().GetHashCode((BYTE *) key, keyLength);

	DWORD index = hashCode & m_hashMask;

	// Allocate an array list at the hash entry if there isn't one
	if(m_keyTable[index] == NULL)
	{
		m_keyTable[index] = ruNEW CRuArrayList<CRuString>(4);
		m_valueTable[index] = ruNEW CRuArrayList<DataType>(4);
	}

	// Search through the array list for the specified key
	for(INT32 i = 0; i < m_keyTable[index]->Count(); i++)
	{
		if(m_keyTable[index]->Get(i) == key)
		{
			m_valueTable[index]->Get(i) = data;
			return TRUE;
		}
	}

	// Add key/data pair to the table
	m_keyTable[index]->Add(CRuString(key));
	m_valueTable[index]->Add(data);

	m_elementCount++;

	return TRUE;

//	return Set(CRuString(key), data);
}

template <class DataType>
inline BOOL CRuStringHash<DataType>::ContainsKey(const char *key) const
{
	return ContainsKey(CRuString(key));
}

template <class DataType>
inline BOOL CRuStringHash<DataType>::Add(const CRuString &key, DataType data)
{
	DWORD index = key.GetHashCode() & m_hashMask;

	// Allocate an array list at the hash entry if there isn't one
	if(m_keyTable[index] == NULL)
	{
		m_keyTable[index] = ruNEW CRuArrayList<CRuString>(4);
		m_valueTable[index] = ruNEW CRuArrayList<DataType>(4);
	}

	// Search through the array list for a duplicate
	for(INT32 i = 0; i < m_keyTable[index]->Count(); i++)
	{
		if(m_keyTable[index]->Get(i) == key)
			return FALSE;
	}

	// Add key/data pair to the table
	m_keyTable[index]->Add(key);
	m_valueTable[index]->Add(data);

	m_elementCount++;

	return TRUE;
}

template <class DataType>
inline BOOL CRuStringHash<DataType>::Replace(const CRuString &key, DataType data)
{
	DWORD index = key.GetHashCode() & m_hashMask;

	// Return if the key does not exist in the table
	if(m_keyTable[index] == NULL)
		return FALSE;

	// Search through the array list for the key
	for(INT32 i = 0; i < m_keyTable[index]->Count(); i++)
	{
		if(m_keyTable[index]->Get(i) == key)
		{
			m_valueTable[index]->Get(i) = data;

			return TRUE;
		}
	}

	return FALSE;
}

template <class DataType>
inline BOOL CRuStringHash<DataType>::Remove(const CRuString &key)
{
	DWORD index = key.GetHashCode() & m_hashMask;

	// Return if the key does not exist in the table
	if(m_keyTable[index] == NULL)
		return FALSE;

	// Search through the array list for the key
	for(INT32 i = 0; i < m_keyTable[index]->Count(); i++)
	{
		if(m_keyTable[index]->Get(i) == key)
		{
			m_keyTable[index]->RemoveAt(i);
			m_valueTable[index]->RemoveAt(i);
			m_elementCount--;

			if(m_keyTable[index]->Count() == 0)
			{
				delete m_keyTable[index];
				delete m_valueTable[index];

				m_keyTable[index] = NULL;
				m_valueTable[index] = NULL;
			}

			return TRUE;
		}
	}

	return FALSE;
}

template <class DataType>
inline BOOL CRuStringHash<DataType>::Get(const CRuString &key, DataType &dataOut)
{
	DWORD index = key.GetHashCode() & m_hashMask;

	// Return if the key does not exist in the table
	if(m_keyTable[index] == NULL)
		return FALSE;

	for(INT32 i = 0; i < m_keyTable[index]->Count(); i++)
	{
		if(m_keyTable[index]->Get(i) == key)
		{
			dataOut = m_valueTable[index]->Get(i);
			return TRUE;
		}
	}

	return FALSE;
}

template <class DataType>
inline BOOL CRuStringHash<DataType>::Set(const CRuString &key, DataType data)
{
	DWORD index = key.GetHashCode() & m_hashMask;

	// Allocate an array list at the hash entry if there isn't one
	if(m_keyTable[index] == NULL)
	{
		m_keyTable[index] = ruNEW CRuArrayList<CRuString>(4);
		m_valueTable[index] = ruNEW CRuArrayList<DataType>(4);
	}

	// Search through the array list for the specified key
	for(INT32 i = 0; i < m_keyTable[index]->Count(); i++)
	{
		if(m_keyTable[index]->Get(i) == key)
		{
			m_valueTable[index]->Get(i) = data;
			return TRUE;
		}
	}

	// Add key/data pair to the table
	m_keyTable[index]->Add(key);
	m_valueTable[index]->Add(data);

	m_elementCount++;

	return TRUE;
}

template <class DataType>
inline BOOL CRuStringHash<DataType>::ContainsKey(const CRuString &key) const
{
	DWORD index = key.GetHashCode() & m_hashMask;

	// Return if the key does not exist in the table
	if(m_keyTable[index] == NULL)
		return FALSE;
		
	// Search through the array list for the key	
	for(INT32 i = 0; i < m_keyTable[index]->Count(); i++)
	{
		if(m_keyTable[index]->Get(i) == key)
			return TRUE;
	}

	return FALSE;
}

template <class DataType>
inline void CRuStringHash<DataType>::Clear()
{
	for(INT32 i = 0; i < m_tableSize; i++)
	{
		delete m_keyTable[i];
		delete m_valueTable[i];

		m_keyTable[i] = NULL;
		m_valueTable[i] = NULL;
	}

	m_elementCount = 0;
}

template <class DataType>
inline BOOL CRuStringHash<DataType>::ForAllElements(CRuStringHashEnumerateCallBack callback, void *userData)
{
	for(INT32 i = 0; i < m_tableSize; i++)
	{
		if(m_keyTable[i])
		{
			for(INT32 j = 0; j < m_keyTable[i]->Count(); j++)
			{
				if(!callback(&m_keyTable[i]->Get(j), &m_valueTable[i]->Get(j), userData))
					return FALSE;
			}
		}
	}

	return TRUE;
}

template <class DataType>
inline BOOL CRuStringHash<DataType>::ForAllElements(CRuStringHashEnumerateCallbackFunctor callback)
{
	for(INT32 i = 0; i < m_tableSize; i++)
	{
		if(m_keyTable[i])
		{
			for(INT32 j = 0; j < m_keyTable[i]->Count(); j++)
			{
				if(!callback(&m_keyTable[i]->Get(j), &m_valueTable[i]->Get(j)))
					return FALSE;
			}
		}
	}

	return TRUE;
}

template <class DataType>
inline INT32 CRuStringHash<DataType>::Count() const
{
	return m_elementCount;
}

template <class DataType>
inline INT32 CRuStringHash<DataType>::GetTableSize() const
{
	return m_tableSize;
}

template <class DataType>
inline float CRuStringHash<DataType>::GetLoadFactor() const
{
	return (float) m_elementCount / (float) m_tableSize;
}

#pragma managed

#endif
