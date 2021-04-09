/*!
	@file RuSortedList.h

	Copyright (c) 2004-2012 Runewaker Entertainment Ltd. All rights reserved.
*/

#pragma once


#include "../../Core/Type/RuType_Base.h"
#include "../../Core/Memory/RuMemoryManager.h"
#include "../../Core/Exception/RuExceptions.h"
#include "../../Core/Utility/RuUtility.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	CRuSortedList
	@version 2012.04.27

	Sorted list implementation.
*/
template <class KeyType, class DataType>
class CRuSortedList
{
public:
	enum FindMode
	{
		FIND_EXACT				= 0,
		FIND_INSERTPOSITION		= 1,
		FIND_INSERTUNIQUE		= 2,
		FIND_NEAREST			= 3
	};

protected:
	struct DataTuple
	{
		KeyType					m_key;
		DataType				m_data;
	};

	DataTuple*					m_data;
	INT32						m_numEntries;
	INT32						m_numEntriesAllocated;

public:
								CRuSortedList(INT32 allocationSize = 16);
								~CRuSortedList();

	INT32						Add(const KeyType &key, const DataType &data, INT32 unique = FALSE);
	inline INT32				Count() const { return m_numEntries; }
	INT32						Find(const KeyType &key, FindMode findMode) const;
	INT32						Remove(const KeyType &key);
	INT32						RemoveAt(INT32 index);
	DataType&					Get(const KeyType &key);
	DataType&					GetByIndex(INT32 index);
	KeyType&					GetKeyByIndex(INT32 index);
	INT32						Contains(const KeyType &key) const;

	inline void					Clear() { m_numEntries = 0; }

	inline DataType&			operator[](const KeyType &key);
	inline const DataType&		operator[](const KeyType &key) const;
};

// ************************************************************************************************************************************************************

template <class KeyType, class DataType>
CRuSortedList<KeyType, DataType>::CRuSortedList(INT32 allocationSize)
{
	if(allocationSize == 0)
		m_data = NULL;
	else
		m_data = static_cast<DataTuple *>(::operator new(sizeof(DataTuple) * allocationSize));
//		m_data = ruNEW DataTuple [allocationSize];

	m_numEntries = 0;
	m_numEntriesAllocated = allocationSize;
}

template <class KeyType, class DataType>
CRuSortedList<KeyType, DataType>::~CRuSortedList()
{
	if(m_data)
		::operator delete(m_data);

//	ruSAFE_DELETE_ARRAY(m_data);

	m_numEntries = 0;
	m_numEntriesAllocated = 0;
}

template <class KeyType, class DataType>
INT32 CRuSortedList<KeyType, DataType>::Add(const KeyType &key, const DataType &data, INT32 unique)
{
	INT32 index = Find(key, unique ? FIND_INSERTUNIQUE : FIND_INSERTPOSITION);

	// Unique insertion is requested, and an entry with the same key was found
	if(index < 0)
	{
		return -1 - index;
	}

	// Determine whether data array resize is required
	if(m_numEntries >= m_numEntriesAllocated)
	{
		// Determine new data array size (this heuristic needs some adjustment)
		INT32 numEntriesAllocatedNew = m_numEntriesAllocated == 0 ? 1 : (m_numEntriesAllocated < 16 ? m_numEntriesAllocated * 2 : m_numEntriesAllocated + 8);

		// Allocate new data array
		DataTuple *dataNew = static_cast<DataTuple *>(::operator new(sizeof(DataTuple) * numEntriesAllocatedNew));
//		DataTuple *dataNew = ruNEW DataTuple [numEntriesAllocatedNew];
		if(dataNew == NULL)
			return FALSE;

		// Copy old data if necessary
		if(m_data != NULL)
		{
			memcpy(dataNew, m_data, m_numEntries * sizeof(DataTuple));
			::operator delete(m_data);

//			memcpy(dataNew, m_data, m_numEntries * sizeof(DataTuple));
//			ruSAFE_DELETE_ARRAY(m_data);
		}

		// Assign data array and allocation count
		m_data = dataNew;
		m_numEntriesAllocated = numEntriesAllocatedNew;
	}

	// Clamp insertion index such that it is within valid array range
	index = min(max(0, index), m_numEntries);

	// Move old data
	INT32 entriesToMove = m_numEntries - index;
	if(entriesToMove > 0)
	{
		memmove(&m_data[index + 1], &m_data[index], sizeof(DataTuple) * entriesToMove);
	}

	// Store data into array
	new (&m_data[index].m_key) KeyType(key);
	new (&m_data[index].m_data) DataType(data);

//	m_data[index].m_key = key;
//	m_data[index].m_data = data;

	// Update item count
	++m_numEntries;

	return index;
}

template <class KeyType, class DataType>
INT32 CRuSortedList<KeyType, DataType>::Find(const KeyType &key, FindMode findMode) const
{
	INT32 iMin = 0;
	INT32 iMax = m_numEntries;
	INT32 entryIdx = 0;

	while(iMax > iMin)
	{
		// Take median index
		entryIdx = (iMax + iMin) / 2;

		if(m_data[entryIdx].m_key < key)
		{
			// Narrow search range to [insertIdx, iMax]
			iMin = ++entryIdx;
		}
		else if(m_data[entryIdx].m_key > key)
		{
			// Narrow search range to [iMin, insertIdx]
			iMax = entryIdx;
		}
		else
		{
			switch(findMode)
			{
				case FIND_EXACT:
				case FIND_NEAREST:
					// Adjust index such that the first index with the same key is returned
					while(entryIdx > 0 && m_data[entryIdx - 1].m_key == key)
					{
						--entryIdx;
					}
					break;

				case FIND_INSERTUNIQUE:
					// No adjustments needed, but return inverted index
					entryIdx = -1 - entryIdx;
					break;

				case FIND_INSERTPOSITION:
					// Adjust index such that the proper insertion position is returned
					do 
					{
						++entryIdx;
					} while (entryIdx < m_numEntries && m_data[entryIdx].m_key == key);
					break;
			}

			return entryIdx;
		}
	}

	switch(findMode)
	{
		case FIND_EXACT:
			break;

		case FIND_NEAREST:
			// If no exact match was found and we want to find the nearest item, entryIdx is always one larger than the nearest index
			// NOTE: Nearest is defined as the smaller of the two values nearest the search value
			if(entryIdx > 0)
			{
				--entryIdx;
				return entryIdx;
			}
			break;

		case FIND_INSERTUNIQUE:
		case FIND_INSERTPOSITION:
			return entryIdx;
	}

	return -1;
}

template <class KeyType, class DataType>
INT32 CRuSortedList<KeyType, DataType>::Remove(const KeyType &key)
{
	INT32 index = Find(key, FIND_EXACT);

	if(index > -1)
	{
		// Decrement count
		--m_numEntries;

		// Collapse the array
		INT32 entriesToMove = m_numEntries - index;
		if(entriesToMove > 0)
		{
			memmove(&m_data[index], &m_data[index + 1], sizeof(DataTuple) * entriesToMove);
		}

		return TRUE;
	}

	return FALSE;
}

template <class KeyType, class DataType>
INT32 CRuSortedList<KeyType, DataType>::RemoveAt(INT32 index)
{
	// Decrement count
	--m_numEntries;

	// Collapse the array
	INT32 entriesToMove = m_numEntries - index;
	if(entriesToMove > 0)
	{
		memmove(&m_data[index], &m_data[index + 1], sizeof(DataTuple) * entriesToMove);
	}

	return TRUE;
}

template <class KeyType, class DataType>
DataType &CRuSortedList<KeyType, DataType>::Get(const KeyType &key)
{
	INT32 index = Find(key, FIND_EXACT);

	if(index > -1)
	{
		return m_data[index].m_data;
	}

	throw CRuArgumentOutOfRangeException();
}

template <class KeyType, class DataType>
DataType &CRuSortedList<KeyType, DataType>::GetByIndex(INT32 index)
{
	return m_data[index].m_data;
}

template <class KeyType, class DataType>
KeyType &CRuSortedList<KeyType, DataType>::GetKeyByIndex(INT32 index)
{
	return m_data[index].m_key;
}

template <class KeyType, class DataType>
INT32 CRuSortedList<KeyType, DataType>::Contains(const KeyType &key) const
{
	INT32 index = Find(key, FIND_EXACT);

	if(index > -1)
	{
		return TRUE;
	}

	return FALSE;
}

template <class KeyType, class DataType>
DataType &CRuSortedList<KeyType, DataType>::operator[](const KeyType &key)
{
	INT32 index = Find(key, FIND_EXACT);

	if(index > -1)
	{
		return m_data[index].m_data;
	}

	throw CRuArgumentOutOfRangeException();
}

template <class KeyType, class DataType>
const DataType &CRuSortedList<KeyType, DataType>::operator[](const KeyType &key) const
{
	INT32 index = Find(key, FIND_EXACT);

	if(index > -1)
	{
		return m_data[index].m_data;
	}

	throw CRuArgumentOutOfRangeException();
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
