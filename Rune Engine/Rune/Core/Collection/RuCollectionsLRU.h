#ifndef _RUCOLLECTIONSLRU_H_
#define _RUCOLLECTIONSLRU_H_

#include "../../Core/Collection/RuCollectionsHash.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	CRu2Queue
	@author Ceranos
	@version Initial
*/
typedef BOOL(* CRu2QueueEnumerateCallBack)(const void *key, const void *data, void *userData);
typedef boost::function<BOOL (const void *, void *)> CRu2QueueEnumerateCallbackFunctor;		// Parameters: (key, data)

template <class KeyType, class DataType>
class CRu2Queue
{
private:
	struct SQueueEntry
	{
		UINT32							Size;					// Relative size of the entry -- unitless
		void*							AddressReference;
		KeyType							Key;					// Key
		DWORD							KeyHash;				// Key hash value
		DataType						Data;					// Data
	};

	struct SOutQueueEntry
	{
		UINT32							Size;					// Relative size of the entry -- unitless
		KeyType							Key;
		DWORD							KeyHash;				// Key hash value
	};

	CRuLinkedList<SQueueEntry *>		m_A1in;
	CRuLinkedList<SOutQueueEntry>		m_A1out;
	CRuLinkedList<SQueueEntry *>		m_AM;					// Hot queue

	CRuHashMap<KeyType, SQueueEntry *>*	m_hashA1in;
	CRuKeyHash<KeyType>*					m_hashA1out;
	CRuHashMap<KeyType, SQueueEntry *>*	m_hashAM;

	UINT32								m_usableDataSize;		// How much space we have left
	UINT32								m_A1inSize;
	UINT32								m_A1outSize;
	UINT32								m_maximumDataSize;		// Maximum size of all data in the queue

	UINT32								m_a1inHashSize;
	UINT32								m_a1outHashSize;
	UINT32								m_amHashSize;

	UINT32								m_a1inQueueSize;
	UINT32								m_a1outQueueSize;
	UINT32								m_amQueueSize;

public:
										CRu2Queue(UINT32 maximumDataSize, UINT32 hashSize);
										~CRu2Queue();

	void								SetMaximumDataSize(UINT32 size);
	UINT32								GetDataSize() { return m_maximumDataSize - m_usableDataSize; }
	UINT32								Count() { return m_A1in.Count() + m_AM.Count(); }

	inline BOOL							Add(const KeyType &key, DataType data, UINT32 dataSize) { return Add(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key)), data, dataSize); }
	inline BOOL							Reclaim(const KeyType &key, UINT32 dataSize, DataType &reclaimedDataOut) { return Reclaim(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key)), dataSize, reclaimedDataOut); }
	inline DataType						Get(const KeyType &key) { return Get(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key))); }
	inline BOOL							ContainsKey(const KeyType &key) { return ContainsKey(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key))); }
	inline BOOL							Remove(const KeyType &key) { return Remove(key,  RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key))); }

	BOOL								Add(const KeyType &key, DWORD keyHash, DataType data, UINT32 dataSize);						// Store the specified key
	BOOL								Reclaim(const KeyType &key, DWORD keyHash, UINT32 dataSize, DataType &reclaimedDataOut);	// Reclaim space so we can store the specified key
	BOOL								ReclaimUnconditional(DataType &reclaimedDataOut);											// Unconditionally reclaim the next object in queue
	DataType							Get(const KeyType &key, DWORD keyHash);														// Get data associated with the specified key
	inline BOOL							Get(const KeyType &key, DWORD keyHash, DataType &dataOut);									// Get data associated with the specified key
	BOOL								ContainsKey(const KeyType &key, DWORD keyHash);												// Check whether the specified key exists
	BOOL								Remove(const KeyType &key, DWORD keyHash);

	void								Clear(void);														// Clear system

	BOOL								ForAllElements(CRu2QueueEnumerateCallBack callback, void *userData);
	BOOL								ForAllElements(CRu2QueueEnumerateCallbackFunctor callback);

	void								GetStatus(UINT32 &a, UINT32 &b, UINT32 &c)
	{
		a = m_A1in.Count();
		b = m_A1out.Count();
		c = m_AM.Count();
	}

//	UINT32								Count(void) { return m_elementCount; }
};

// ************************************************************************************************************************************************************

/*!
	CRuLRUQueue
	@author Ceranos
	@version Initial
*/
typedef BOOL(* CRuLRUQueueEnumerateCallBack)(const void *key, const void *data, void *userData);
typedef boost::function<BOOL (const void *, void *)> CRuLRUQueueEnumerateCallbackFunctor;		// Parameters: (key, data)

template <class KeyType, class DataType>
class CRuLRUQueue
{
private:
	struct SQueueEntry
	{
		UINT32							Size;					// Relative size of the entry -- unitless
		void*							AddressReference;		// Address reference back into the linked list for fast relocation
		KeyType							Key;					// Key
		DWORD							KeyHash;				// Key hash value
		DataType						Data;					// Data
	};

	CRuLinkedList<SQueueEntry *>		m_queue;
	CRuHashMap<KeyType, SQueueEntry *>*	m_hash;

	UINT32								m_usableDataSize;		// How much space we have left
	UINT32								m_maximumDataSize;		// Maximum size of all data in the queue

	UINT32								m_queueSize;

	UINT32								m_hashSize;

public:
										CRuLRUQueue(UINT32 maximumDataSize, UINT32 hashSize);
										~CRuLRUQueue();

	void								SetMaximumDataSize(UINT32 size);
	UINT32								GetDataSize() { return m_maximumDataSize - m_usableDataSize; }
	UINT32								Count() { return m_hash->Count(); }

	inline BOOL							Add(const KeyType &key, DataType data, UINT32 dataSize) { return Add(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key)), data, dataSize); }
	inline BOOL							Reclaim(const KeyType &key, UINT32 dataSize, DataType &reclaimedDataOut) { return Reclaim(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key)), dataSize, reclaimedDataOut); }
	inline DataType						Get(const KeyType &key) { return Get(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key))); }
	inline BOOL							ContainsKey(const KeyType &key) { return ContainsKey(key, RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key))); }
	inline BOOL							Remove(const KeyType &key) { return Remove(key,  RuGlobalHashGen().GetHashCode((BYTE *) &key, sizeof(key))); }

	BOOL								Add(const KeyType &key, DWORD keyHash, DataType data, UINT32 dataSize);						// Store the specified key
	BOOL								Reclaim(const KeyType &key, DWORD keyHash, UINT32 dataSize, DataType &reclaimedDataOut);	// Reclaim space so we can store the specified key
	BOOL								ReclaimUnconditional(DataType &reclaimedDataOut);											// Unconditionally reclaim the next object in queue
	BOOL								PeekNextReclaimTarget(DataType &reclaimedDataOut);											// Takes a peek at the next object to be reclaimed
	DataType							Get(const KeyType &key, DWORD keyHash);														// Get data associated with the specified key
	inline BOOL							Get(const KeyType &key, DWORD keyHash, DataType &dataOut);									// Get data associated with the specified key
	BOOL								Set(const KeyType &key, DWORD keyHash, const DataType &data);
	BOOL								Touch(const KeyType &key, DWORD keyHash);
	BOOL								ContainsKey(const KeyType &key, DWORD keyHash);												// Check whether the specified key exists
	BOOL								Remove(const KeyType &key, DWORD keyHash);

	void								Clear(void);														// Clear system

	BOOL								ForAllElements(CRuLRUQueueEnumerateCallBack callback, void *userData);
	BOOL								ForAllElements(CRuLRUQueueEnumerateCallbackFunctor callback);

	void								GetStatus(UINT32 &a, UINT32 &b, UINT32 &c)
										{
										}

//	UINT32								Count(void) { return m_elementCount; }
};

// ************************************************************************************************************************************************************

typedef boost::function<BOOL (const void *, const void *, void *)> CRuLRUQueueExEnumerateCallbackFunctor;		// Parameters: (key1, key2, data)

template <class KeyType1, class KeyType2, class DataType>
class CRuLRUQueueEx
{
private:
	struct QueueEntry
	{
		UINT32									m_size;					// Relative size of the entry -- unitless
		void*									m_addressReference;		// Address reference back into the linked list for fast relocation
		KeyType2								m_key;					// Key
		DWORD									m_keyHash;				// Key hash value
		DataType								m_data;					// Data
	};

	struct RootEntry
	{
		KeyType1								m_key;
		CRuHashMap<KeyType2, QueueEntry *>*		m_hash;
	};

	struct LRUEntry
	{
		KeyType1								m_key;
		DWORD									m_keyHash;
		QueueEntry*								m_queueEntry;
	};

	struct QueueForAllElementsCallbackData
	{
		CRuLRUQueueEnumerateCallBack			m_callback;
		void*									m_userData;
	};

	UINT32										m_usableDataSize;		// How much space we have left
	UINT32										m_maximumDataSize;		// Maximum size of all data in the queue

	UINT32										m_queueSize;
	UINT32										m_rootHashSize;
	UINT32										m_queueHashSize;

	CRuLinkedList<LRUEntry>						m_queue;
	CRuHashMap<KeyType1, RootEntry *>*			m_rootHash;

public:
												CRuLRUQueueEx(UINT32 maximumDataSize, UINT32 rootHashSize, UINT32 queueHashSize);
												~CRuLRUQueueEx();

	BOOL										Add(const KeyType1 &key1, DWORD key1Hash, const KeyType2 &key2, DWORD key2Hash, DataType data, UINT32 dataSize);	// Store the specified key
	BOOL										Reclaim(UINT32 dataSize, DataType &reclaimedDataOut);																// Reclaim space so we can store the specified key
	BOOL										ReclaimUnconditional(DataType &reclaimedDataOut);																	// Unconditionally reclaim the next object in queue
	BOOL										Get(const KeyType1 &key1, DWORD key1Hash, const KeyType2 &key2, DWORD key2Hash, DataType &dataOut);					// Get data associated with the specified key
	BOOL										Set(const KeyType1 &key1, DWORD key1Hash, const KeyType2 &key2, DWORD key2Hash, const DataType &data);
	BOOL										ContainsKey(const KeyType1 &key1, DWORD key1Hash, const KeyType2 &key2, DWORD key2Hash);							// Check whether the specified key exists
	BOOL										Remove(const KeyType1 &key1, DWORD key1Hash);
	BOOL										Remove(const KeyType1 &key1, DWORD key1Hash, const KeyType2 &key2, DWORD key2Hash);

	void										Clear();																											// Clear system

	BOOL										ForAllElements(CRuLRUQueueExEnumerateCallbackFunctor callback);

	BOOL										ForAllElements(CRuLRUQueueEnumerateCallBack callback, void *userData);
//	BOOL										ForAllElements(CRuLRUQueueEnumerateCallbackFunctor callback);
	BOOL										ForAllElements(const KeyType1 &key1, DWORD key1Hash, CRuLRUQueueEnumerateCallBack callback, void *userData);

private:
	static BOOL									QueueForAllElementsCallback(const void *key, const void *data, void *userData);
	static BOOL									ClearCallback(const void *key, const void *data, void *userData);
	static BOOL									RemoveCallback(const void *key, const void *data, void *userData);
};

// ************************************************************************************************************************************************************

/*
--------------------------------------------------------------------
CRu2Queue

Beginning of function definition for templatized CRu2Queue

--------------------------------------------------------------------
*/

template <class KeyType, class DataType>
CRu2Queue<KeyType, DataType>::CRu2Queue(UINT32 maximumDataSize, UINT32 hashSize)
{
	float A1inRatio = 0.25f, A1outRatio = 0.5f;
//	float A1inRatio = 0.95f, A1outRatio = 0.5f;	// this makes the queue behave a lot more like a FIFO LRU queue

	// Make sure specified hash size meets a certain minimum
	if(hashSize < 4)
		hashSize = 4;

	// Calculate hash sizes for each queue
	m_a1inHashSize = (UINT32) (hashSize * A1inRatio);
	m_a1outHashSize = (UINT32) (hashSize * A1outRatio);
	m_amHashSize = hashSize - m_a1inHashSize;

	// Allocate space for each queue's hash map
	m_hashA1in = ruNEW CRuHashMap<KeyType, SQueueEntry *>(m_a1inHashSize);
	m_hashA1out = ruNEW CRuKeyHash<KeyType>(m_a1outHashSize);
	m_hashAM = ruNEW CRuHashMap<KeyType, SQueueEntry *>(m_amHashSize);

	// Calculate allowable sizes for each queue
	m_a1inQueueSize = (UINT32) (maximumDataSize * A1inRatio);
	m_a1outQueueSize = (UINT32) (maximumDataSize * A1outRatio);
	m_amQueueSize = maximumDataSize - m_a1inQueueSize;

	// Set data sizes
	m_usableDataSize = maximumDataSize;
	m_A1inSize = 0;
	m_A1outSize = 0;
	m_maximumDataSize = maximumDataSize;
}

template <class KeyType, class DataType>
CRu2Queue<KeyType, DataType>::~CRu2Queue()
{
	Clear();

	delete m_hashA1in;
	delete m_hashA1out;
	delete m_hashAM;
}

template <class KeyType, class DataType>
void CRu2Queue<KeyType, DataType>::SetMaximumDataSize(UINT32 size)
{
	if(size > m_maximumDataSize)
	{
		m_usableDataSize += size - m_maximumDataSize;
		m_maximumDataSize = size;
	}
}

template <class KeyType, class DataType>
BOOL CRu2Queue<KeyType, DataType>::Add(const KeyType &key, DWORD keyHash, DataType data, UINT32 dataSize)
{
	// Abort if there is not enough space
	if(dataSize > m_usableDataSize)
		return FALSE;

	// If AM or A1in already contain the key, abort
	if(m_hashAM->ContainsKey(key, keyHash) || m_hashA1in->ContainsKey(key, keyHash))
		return FALSE;

	// Construct new queue entry
	SQueueEntry *newEntry = ruNEW SQueueEntry();
	newEntry->Size = dataSize;
	newEntry->Key = key;
	newEntry->KeyHash = keyHash;
	newEntry->Data = data;

	// If A1out contains the key, add the new entry directly to the LRU queue.
	// Otherwise, add the new entry to the FIFO queue.
	if(m_hashA1out->ContainsKey(key, keyHash))
	{
		m_AM.Add(newEntry, &newEntry->AddressReference);
		m_hashAM->Add(key, keyHash, newEntry);
		m_usableDataSize -= dataSize;
	}
	else
	{
		m_A1in.Add(newEntry, &newEntry->AddressReference);
		m_hashA1in->Add(key, keyHash, newEntry);
		m_A1inSize += dataSize;
		m_usableDataSize -= dataSize;
	}

	return TRUE;
}

template <class KeyType, class DataType>
BOOL CRu2Queue<KeyType, DataType>::Reclaim(const KeyType &key, DWORD keyHash, UINT32 dataSize, DataType &reclaimedDataOut)
{
	// NOTE: Assume user is not stupid enough to try reclaiming space that's larger than the capacity of the queue.

	SQueueEntry *reclaimedEntry;

	// If there is enough space to accomodate the new entry, return FALSE to discontinue reclaimation
	if(dataSize <= m_usableDataSize)
		return FALSE;

	// Abort if there is nothing more to reclaim
	if(m_AM.Count() == 0 && m_A1in.Count() == 0)
		return FALSE;

	// If size of A1in queue exceeds its maximum allowed size or if there is nothing in AM
	if(m_A1inSize > m_a1inQueueSize || m_AM.Count() == 0)
	{
		// Select the oldest entry in A1in
		reclaimedEntry = m_A1in.Get(0);
		reclaimedDataOut = reclaimedEntry->Data;

		// Page out the selected entry
		m_usableDataSize += reclaimedEntry->Size;
		m_A1inSize -= reclaimedEntry->Size;

		m_A1in.RemoveAt(0);
		m_hashA1in->Remove(reclaimedEntry->Key, reclaimedEntry->KeyHash);

		// Place reclaimed key into A1out
		SOutQueueEntry outEntry;
		outEntry.Size = reclaimedEntry->Size;
		outEntry.Key = reclaimedEntry->Key;
		outEntry.KeyHash = reclaimedEntry->KeyHash;

		m_A1out.Add(outEntry);
		m_hashA1out->Add(outEntry.Key, outEntry.KeyHash);
		m_A1outSize += outEntry.Size;

		delete reclaimedEntry;

		// Truncate the oldest entry from A1out if it exceeds the maximum size
		if(m_A1outSize > m_a1outQueueSize)
		{
			m_A1outSize -= m_A1out.Get(0).Size;
			m_hashA1out->Remove(m_A1out.Get(0).Key, m_A1out.Get(0).KeyHash);
			m_A1out.RemoveAt(0);
		}

		return TRUE;
	}

	// Select tail of AM
	reclaimedEntry = m_AM.Get(0);
	reclaimedDataOut = reclaimedEntry->Data;

	// Page out the selected entry
	m_usableDataSize += reclaimedEntry->Size;
	m_AM.RemoveAt(0);
	m_hashAM->Remove(reclaimedEntry->Key, reclaimedEntry->KeyHash);
	delete reclaimedEntry;

	return TRUE;
}

template <class KeyType, class DataType>
BOOL CRu2Queue<KeyType, DataType>::ReclaimUnconditional(DataType &reclaimedDataOut)
{
	// NOTE: Assume user is not stupid enough to try reclaiming space that's larger than the capacity of the queue.

	SQueueEntry *reclaimedEntry;

	// Abort if there is nothing more to reclaim
	if(m_AM.Count() == 0 && m_A1in.Count() == 0)
		return FALSE;

	// If size of A1in queue exceeds its maximum allowed size or if there is nothing in AM
	if(m_A1inSize > m_a1inQueueSize || m_AM.Count() == 0)
	{
		// Select the oldest entry in A1in
		reclaimedEntry = m_A1in.Get(0);
		reclaimedDataOut = reclaimedEntry->Data;

		// Page out the selected entry
		m_usableDataSize += reclaimedEntry->Size;
		m_A1inSize -= reclaimedEntry->Size;

		m_A1in.RemoveAt(0);
		m_hashA1in->Remove(reclaimedEntry->Key, reclaimedEntry->KeyHash);

		// Place reclaimed key into A1out
		SOutQueueEntry outEntry;
		outEntry.Size = reclaimedEntry->Size;
		outEntry.Key = reclaimedEntry->Key;
		outEntry.KeyHash = reclaimedEntry->KeyHash;

		m_A1out.Add(outEntry);
		m_hashA1out->Add(outEntry.Key, outEntry.KeyHash);
		m_A1outSize += outEntry.Size;

		delete reclaimedEntry;

		// Truncate the oldest entry from A1out if it exceeds the maximum size
		if(m_A1outSize > m_a1outQueueSize)
		{
			m_A1outSize -= m_A1out.Get(0).Size;
			m_hashA1out->Remove(m_A1out.Get(0).Key, m_A1out.Get(0).KeyHash);
			m_A1out.RemoveAt(0);
		}

		return TRUE;
	}

	// Select tail of AM
	reclaimedEntry = m_AM.Get(0);
	reclaimedDataOut = reclaimedEntry->Data;

	// Page out the selected entry
	m_usableDataSize += reclaimedEntry->Size;
	m_AM.RemoveAt(0);
	m_hashAM->Remove(reclaimedEntry->Key, reclaimedEntry->KeyHash);
	delete reclaimedEntry;

	return TRUE;
}

template <class KeyType, class DataType>
DataType CRu2Queue<KeyType, DataType>::Get(const KeyType &key, DWORD keyHash)
{
	SQueueEntry *entry;
	
	if(m_hashAM->Get(key, keyHash, entry))
	{
		m_AM.MoveToEndAtAddress(entry->AddressReference);

		return entry->Data;
	}

	if(m_hashA1in->Get(key, keyHash, entry))
	{
		m_A1in.MoveToEndAtAddress(entry->AddressReference);

		return entry->Data;
	}

	throw CRuArgumentOutOfRangeException();
}

template <class KeyType, class DataType>
inline BOOL CRu2Queue<KeyType, DataType>::Get(const KeyType &key, DWORD keyHash, DataType &dataOut)
{
	SQueueEntry *entry;
	
	if(m_hashAM->Get(key, keyHash, entry))
	{
		dataOut = entry->Data;

		m_AM.MoveToEndAtAddress(entry->AddressReference);

		return TRUE;
	}

	if(m_hashA1in->Get(key, keyHash, entry))
	{
		dataOut = entry->Data;

		m_A1in.MoveToEndAtAddress(entry->AddressReference);

		return TRUE;
	}

	return FALSE;
}

template <class KeyType, class DataType>
BOOL CRu2Queue<KeyType, DataType>::ContainsKey(const KeyType &key, DWORD keyHash)
{
	if(m_hashAM->ContainsKey(key, keyHash) || m_hashA1in->ContainsKey(key, keyHash))
		return TRUE;

	return FALSE;
}

template <class KeyType, class DataType>
BOOL CRu2Queue<KeyType, DataType>::Remove(const KeyType &key, DWORD keyHash)
{
	SQueueEntry *entry;

	if(m_hashAM->Get(key, keyHash, entry))
	{
		m_usableDataSize += entry->Size;
		m_AM.RemoveAtAddress(entry->AddressReference);
		m_hashAM->Remove(key, keyHash);
	}

	if(m_hashA1in->Get(key, keyHash, entry))
	{
		m_usableDataSize += entry->Size;
		m_A1inSize -= entry->Size;
		m_A1in.RemoveAtAddress(entry->AddressReference);
		m_hashA1in->Remove(key, keyHash);
	}

	return FALSE;
}

template <class KeyType, class DataType>
void CRu2Queue<KeyType, DataType>::Clear(void)
{
	INT32 i;
	SQueueEntry *curEntry;

	// Remove all entries in A1in
	for(i = 0; i < m_A1in.Count(); i++)
	{
		curEntry = m_A1in.Get(i);
		delete curEntry;
	}

	// Remove all entries in AM
	for(i = 0; i < m_AM.Count(); i++)
	{
		curEntry = m_AM.Get(i);
		delete curEntry;
	}

	m_A1in.Clear();
	m_A1out.Clear();
	m_AM.Clear();

	m_hashA1in->Clear();
	m_hashA1out->Clear();
	m_hashAM->Clear();

	m_usableDataSize = m_maximumDataSize;
	m_A1inSize = 0;
	m_A1outSize = 0;
}

template <class KeyType, class DataType>
BOOL CRu2Queue<KeyType, DataType>::ForAllElements(CRu2QueueEnumerateCallBack callback, void *userData)
{
	INT32 i;

	try
	{
		for(i = 0; i < m_A1in.Count(); i++)
		{
			SQueueEntry *curEntry = m_A1in.Get(i);
			if(!callback(&curEntry->Key, &curEntry->Data, userData))
				return FALSE;
		}
	}
	catch(CRuArgumentOutOfRangeException e)
	{
		char moo[200];
		sprintf(moo, "%u %u", i, m_A1in.Count());
		MessageBox(NULL, e.ToString(), moo, MB_OK);
	}


		for(i = 0; i < m_AM.Count(); i++)
		{
			SQueueEntry *curEntry = m_AM.Get(i);
			if(!callback(&curEntry->Key, &curEntry->Data, userData))
				return FALSE;
		}

	return TRUE;
}

template <class KeyType, class DataType>
BOOL CRu2Queue<KeyType, DataType>::ForAllElements(CRu2QueueEnumerateCallbackFunctor callback)
{
	INT32 i;

	try
	{
		for(i = 0; i < m_A1in.Count(); i++)
		{
			SQueueEntry *curEntry = m_A1in.Get(i);
			if(!callback(&curEntry->Key, &curEntry->Data))
				return FALSE;
		}
	}
	catch(CRuArgumentOutOfRangeException e)
	{
		char moo[200];
		sprintf(moo, "%u %u", i, m_A1in.Count());
		MessageBox(NULL, e.ToString(), moo, MB_OK);
	}


	for(i = 0; i < m_AM.Count(); i++)
	{
		SQueueEntry *curEntry = m_AM.Get(i);
		if(!callback(&curEntry->Key, &curEntry->Data))
			return FALSE;
	}

	return TRUE;
}


// ************************************************************************************************************************************************************

/*
--------------------------------------------------------------------
CRuLRUQueue

Beginning of function definition for templatized CRuLRUQueue

--------------------------------------------------------------------
*/

template <class KeyType, class DataType>
CRuLRUQueue<KeyType, DataType>::CRuLRUQueue(UINT32 maximumDataSize, UINT32 hashSize)
{
	// Make sure specified hash size meets a certain minimum
	if(hashSize < 4)
		hashSize = 4;

	// Calculate hash sizes for each queue
	m_hashSize = hashSize;
	m_queueSize = maximumDataSize;

	// Allocate space for the hash map
	m_hash = ruNEW CRuHashMap<KeyType, SQueueEntry *>(m_hashSize);

	// Set data sizes
	m_usableDataSize = maximumDataSize;
	m_maximumDataSize = maximumDataSize;
}

template <class KeyType, class DataType>
CRuLRUQueue<KeyType, DataType>::~CRuLRUQueue()
{
	Clear();

	delete m_hash;
}

template <class KeyType, class DataType>
void CRuLRUQueue<KeyType, DataType>::SetMaximumDataSize(UINT32 size)
{
	if(size > m_maximumDataSize)
	{
		m_usableDataSize += size - m_maximumDataSize;
		m_maximumDataSize = size;
	}
}

template <class KeyType, class DataType>
BOOL CRuLRUQueue<KeyType, DataType>::Add(const KeyType &key, DWORD keyHash, DataType data, UINT32 dataSize)
{
	// Abort if there is not enough space
	if(dataSize > m_usableDataSize)
		return FALSE;

	// If hash already contain the key, abort
	if(m_hash->ContainsKey(key, keyHash))
		return FALSE;

	// Construct new queue entry
	SQueueEntry *newEntry = ruNEW SQueueEntry();
	newEntry->Size = dataSize;
	newEntry->Key = key;
	newEntry->KeyHash = keyHash;
	newEntry->Data = data;

	// Add the new entry to the FIFO queue.
	m_queue.Add(newEntry, &newEntry->AddressReference);
	m_hash->Add(key, keyHash, newEntry);
	m_usableDataSize -= dataSize;

	return TRUE;
}

template <class KeyType, class DataType>
BOOL CRuLRUQueue<KeyType, DataType>::Reclaim(const KeyType &key, DWORD keyHash, UINT32 dataSize, DataType &reclaimedDataOut)
{
	SQueueEntry *reclaimedEntry;

	// If there is enough space to accomodate the new entry, return FALSE to discontinue reclaimation
	if(dataSize <= m_usableDataSize)
		return FALSE;

	// Abort if there is nothing more to reclaim
	if(m_queue.Count() == 0)
		return FALSE;

	// Select tail of the queue
	reclaimedEntry = m_queue.Get(0);
	reclaimedDataOut = reclaimedEntry->Data;

	// Page out the selected entry
	m_usableDataSize += reclaimedEntry->Size;
	m_queue.RemoveAt(0);
	m_hash->Remove(reclaimedEntry->Key, reclaimedEntry->KeyHash);
	delete reclaimedEntry;

	return TRUE;
}

template <class KeyType, class DataType>
BOOL CRuLRUQueue<KeyType, DataType>::ReclaimUnconditional(DataType &reclaimedDataOut)
{
	SQueueEntry *reclaimedEntry;

	// Abort if there is nothing more to reclaim
	if(m_queue.Count() == 0)
		return FALSE;

	// Select tail of the queue
	reclaimedEntry = m_queue.Get(0);
	reclaimedDataOut = reclaimedEntry->Data;

	// Page out the selected entry
	m_usableDataSize += reclaimedEntry->Size;
	m_queue.RemoveAt(0);
	m_hash->Remove(reclaimedEntry->Key, reclaimedEntry->KeyHash);
	delete reclaimedEntry;

	return TRUE;
}

template <class KeyType, class DataType>
BOOL CRuLRUQueue<KeyType, DataType>::PeekNextReclaimTarget(DataType &reclaimedDataOut)
{
	SQueueEntry *reclaimedEntry;

	// Abort if there is nothing more to reclaim
	if(m_queue.Count() == 0)
		return FALSE;

	// Select tail of the queue
	reclaimedEntry = m_queue.Get(0);
	reclaimedDataOut = reclaimedEntry->Data;

	return TRUE;
}

template <class KeyType, class DataType>
DataType CRuLRUQueue<KeyType, DataType>::Get(const KeyType &key, DWORD keyHash)
{
	SQueueEntry *entry;
	
	if(m_hash->Get(key, keyHash, entry))
	{
		m_queue.MoveToEndAtAddress(entry->AddressReference);

		return entry->Data;
	}

	throw CRuArgumentOutOfRangeException();
}

template <class KeyType, class DataType>
BOOL CRuLRUQueue<KeyType, DataType>::Get(const KeyType &key, DWORD keyHash, DataType &dataOut)
{
	SQueueEntry *entry;
	
	if(m_hash->Get(key, keyHash, entry))
	{
		dataOut = entry->Data;

		m_queue.MoveToEndAtAddress(entry->AddressReference);

		return TRUE;
	}

	return FALSE;
}

template <class KeyType, class DataType>
BOOL CRuLRUQueue<KeyType, DataType>::Set(const KeyType &key, DWORD keyHash, const DataType &data)
{
	SQueueEntry *entry;

	if(m_hash->Get(key, keyHash, entry))
	{
		entry->Data = data;

		return TRUE;
	}

	return FALSE;
}

template <class KeyType, class DataType>
BOOL CRuLRUQueue<KeyType, DataType>::Touch(const KeyType &key, DWORD keyHash)
{
	SQueueEntry *entry;

	if(m_hash->Get(key, keyHash, entry))
	{
		m_queue.MoveToEndAtAddress(entry->AddressReference);

		return TRUE;
	}

	return FALSE;
}

template <class KeyType, class DataType>
BOOL CRuLRUQueue<KeyType, DataType>::ContainsKey(const KeyType &key, DWORD keyHash)
{
	return m_hash->ContainsKey(key, keyHash);
}

template <class KeyType, class DataType>
BOOL CRuLRUQueue<KeyType, DataType>::Remove(const KeyType &key, DWORD keyHash)
{
	SQueueEntry *entry;

	if(m_hash->Get(key, keyHash, entry))
	{
		// Update the usable data size
		m_usableDataSize += entry->Size;

		// Remove the queue entry from the LRU queue
		m_queue.RemoveAtAddress(entry->AddressReference);

		// Remove the queue entry from the hash
		m_hash->Remove(key, keyHash);

		// Delete queue entry
		delete entry;
	}

	return FALSE;
}

template <class KeyType, class DataType>
void CRuLRUQueue<KeyType, DataType>::Clear(void)
{
	INT32 i;
	SQueueEntry *curEntry;

	// Remove all entries in queue
	for(i = 0; i < m_queue.Count(); i++)
	{
		curEntry = m_queue.Get(i);
		delete curEntry;
	}

	m_queue.Clear();
	m_hash->Clear();

	m_usableDataSize = m_maximumDataSize;
}

template <class KeyType, class DataType>
BOOL CRuLRUQueue<KeyType, DataType>::ForAllElements(CRuLRUQueueEnumerateCallBack callback, void *userData)
{
	INT32 i;

	for(i = 0; i < m_queue.Count(); i++)
	{
		SQueueEntry *curEntry = m_queue.Get(i);
		if(!callback(&curEntry->Key, &curEntry->Data, userData))
			return FALSE;
	}

	return TRUE;
}

template <class KeyType, class DataType>
BOOL CRuLRUQueue<KeyType, DataType>::ForAllElements(CRuLRUQueueEnumerateCallbackFunctor callback)
{
	INT32 i;

	for(i = 0; i < m_queue.Count(); i++)
	{
		SQueueEntry *curEntry = m_queue.Get(i);
		if(!callback(&curEntry->Key, &curEntry->Data))
			return FALSE;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

template <class KeyType1, class KeyType2, class DataType>
CRuLRUQueueEx<KeyType1, KeyType2, DataType>::CRuLRUQueueEx(UINT32 maximumDataSize, UINT32 rootHashSize, UINT32 queueHashSize)
{
	// Make sure specified hash size meets a certain minimum
	if(rootHashSize < 64)
		rootHashSize = 64;

	m_rootHashSize = rootHashSize;
	m_queueHashSize = queueHashSize;
	m_queueSize = maximumDataSize;

	// Allocate space for the hash map
	m_rootHash = ruNEW CRuHashMap<KeyType1, RootEntry *>(m_rootHashSize);

	// Set data sizes
	m_usableDataSize = maximumDataSize;
	m_maximumDataSize = maximumDataSize;
}

template <class KeyType1, class KeyType2, class DataType>
CRuLRUQueueEx<KeyType1, KeyType2, DataType>::~CRuLRUQueueEx()
{
	Clear();

	delete m_rootHash;
}

template <class KeyType1, class KeyType2, class DataType>
BOOL CRuLRUQueueEx<KeyType1, KeyType2, DataType>::Add(const KeyType1 &key1, DWORD key1Hash, const KeyType2 &key2, DWORD key2Hash, DataType data, UINT32 dataSize)
{
	// Abort if there is not enough space
	if(dataSize > m_usableDataSize)
		return FALSE;

	RootEntry *rootEntry;

	if(m_rootHash->Get(key1, key1Hash, rootEntry))
	{
		// If hash already contain the key, abort
		if(rootEntry->m_hash->ContainsKey(key2, key2Hash))
			return FALSE;
	}
	else
	{
		// Construct new root entry
		rootEntry = ruNEW RootEntry();
		rootEntry->m_key = key1;
		rootEntry->m_hash = ruNEW CRuHashMap<KeyType2, QueueEntry *>(m_queueHashSize);

		m_rootHash->Add(key1, key1Hash, rootEntry);
	}

	// Construct new entry
	QueueEntry *queueEntry = ruNEW QueueEntry();
	queueEntry->m_size = dataSize;
	queueEntry->m_key = key2;
	queueEntry->m_keyHash = key2Hash;
	queueEntry->m_data = data;

	// Add the new entry to the FIFO queue.
	LRUEntry lruEntry;
	lruEntry.m_key = key1;
	lruEntry.m_keyHash = key1Hash;
	lruEntry.m_queueEntry = queueEntry;

	// Add LRU entry and store its address reference
	m_queue.Add(lruEntry, &queueEntry->m_addressReference);

	// Add hash entry
	rootEntry->m_hash->Add(key2, key2Hash, queueEntry);

	// Update usable data size
	m_usableDataSize -= dataSize;

	return TRUE;
}

template <class KeyType1, class KeyType2, class DataType>
BOOL CRuLRUQueueEx<KeyType1, KeyType2, DataType>::Reclaim(UINT32 dataSize, DataType &reclaimedDataOut)
{
	RootEntry *rootEntry;
	QueueEntry *reclaimedEntry;

	// If there is enough space to accommodate the new entry, return FALSE to discontinue reclaimation
	if(dataSize <= m_usableDataSize)
		return FALSE;

	// Abort if there is nothing more to reclaim
	if(m_queue.Count() == 0)
		return FALSE;

	// Select the least recently used entry
	LRUEntry &lruEntry = m_queue.Get(0);
	reclaimedEntry = lruEntry.m_queueEntry;
	reclaimedDataOut = reclaimedEntry->m_data;

	if(m_rootHash->Get(lruEntry.m_key, lruEntry.m_keyHash, rootEntry))
	{
		// Page out the selected entry
		m_usableDataSize += reclaimedEntry->m_size;

		m_queue.RemoveAt(0);
		rootEntry->m_hash->Remove(reclaimedEntry->m_key, reclaimedEntry->m_keyHash);

		delete reclaimedEntry;

		return TRUE;
	}

	return FALSE;
}

template <class KeyType1, class KeyType2, class DataType>
BOOL CRuLRUQueueEx<KeyType1, KeyType2, DataType>::ReclaimUnconditional(DataType &reclaimedDataOut)
{
	RootEntry *rootEntry;
	QueueEntry *reclaimedEntry;

	// Abort if there is nothing more to reclaim
	if(m_queue.Count() == 0)
		return FALSE;

	// Select the least recently used entry
	LRUEntry &lruEntry = m_queue.Get(0);
	reclaimedEntry = lruEntry.m_queueEntry;
	reclaimedDataOut = reclaimedEntry->m_data;

	if(m_rootHash->Get(lruEntry.m_key, lruEntry.m_keyHash, rootEntry))
	{
		// Page out the selected entry
		m_usableDataSize += reclaimedEntry->m_size;

		m_queue.RemoveAt(0);
		rootEntry->m_hash->Remove(reclaimedEntry->m_key, reclaimedEntry->m_keyHash);

		delete reclaimedEntry;

		return TRUE;
	}

	return FALSE;
}

template <class KeyType1, class KeyType2, class DataType>
inline BOOL CRuLRUQueueEx<KeyType1, KeyType2, DataType>::Get(const KeyType1 &key1, DWORD key1Hash, const KeyType2 &key2, DWORD key2Hash, DataType &dataOut)
{
	RootEntry *rootEntry;

	if(m_rootHash->Get(key1, key1Hash, rootEntry))
	{
		QueueEntry *queueEntry;
		if(rootEntry->m_hash->Get(key2, key2Hash, queueEntry))
		{
			// Store data in dataOut
			dataOut = queueEntry->m_data;

			// Move entry to end of the LRU queue
			m_queue.MoveToEndAtAddress(queueEntry->m_addressReference);

			return TRUE;
		}
	}

	return FALSE;
}

template <class KeyType1, class KeyType2, class DataType>
inline BOOL CRuLRUQueueEx<KeyType1, KeyType2, DataType>::Set(const KeyType1 &key1, DWORD key1Hash, const KeyType2 &key2, DWORD key2Hash, const DataType &data)
{
	RootEntry *rootEntry;

	if(m_rootHash->Get(key1, key1Hash, rootEntry))
	{
		QueueEntry *queueEntry;
		if(rootEntry->m_hash->Get(key2, key2Hash, queueEntry))
		{
			queueEntry->m_data = data;

			return TRUE;
		}
	}

	return FALSE;
}

template <class KeyType1, class KeyType2, class DataType>
inline BOOL CRuLRUQueueEx<KeyType1, KeyType2, DataType>::ContainsKey(const KeyType1 &key1, DWORD key1Hash, const KeyType2 &key2, DWORD key2Hash)
{
	RootEntry *rootEntry;

	if(m_rootHash->Get(key1, key1Hash, rootEntry))
	{
		if(rootEntry->m_hash->ContainsKey(key2, key2Hash))
			return TRUE;
	}

	return FALSE;
}

template <class KeyType1, class KeyType2, class DataType>
BOOL CRuLRUQueueEx<KeyType1, KeyType2, DataType>::Remove(const KeyType1 &key1, DWORD key1Hash)
{
	RootEntry *rootEntry;

	if(m_rootHash->Get(key1, key1Hash, rootEntry))
	{
		// Clear all entries
		rootEntry->m_hash->ForAllElements(RemoveCallback, this);

		// Clear all entries at the key
		rootEntry->m_hash->Clear();

		return TRUE;
	}

	return FALSE;
}

template <class KeyType1, class KeyType2, class DataType>
BOOL CRuLRUQueueEx<KeyType1, KeyType2, DataType>::Remove(const KeyType1 &key1, DWORD key1Hash, const KeyType2 &key2, DWORD key2Hash)
{
	RootEntry *rootEntry;

	if(m_rootHash->Get(key1, key1Hash, rootEntry))
	{
		QueueEntry *queueEntry;
		if(rootEntry->m_hash->Get(key2, key2Hash, queueEntry))
		{
			m_usableDataSize += queueEntry->m_size;
			m_queue.RemoveAtAddress(queueEntry->m_addressReference);
			rootEntry->m_hash->Remove(key2, key2Hash);

			// Delete the queue entry
			delete queueEntry;

			return TRUE;
		}
	}

	return FALSE;
}

template <class KeyType1, class KeyType2, class DataType>
void CRuLRUQueueEx<KeyType1, KeyType2, DataType>::Clear()
{
	INT32 i;

	// Remove all entries in queue
	for(i = 0; i < m_queue.Count(); i++)
	{
		// Delete queue entry
		LRUEntry &lruEntry = m_queue.Get(i);
		delete lruEntry.m_queueEntry;
	}

	// Clear all root level entries
	m_rootHash->ForAllElements(ClearCallback, this);

	m_queue.Clear();
	m_rootHash->Clear();

	m_usableDataSize = m_maximumDataSize;
}

template <class KeyType1, class KeyType2, class DataType>
BOOL CRuLRUQueueEx<KeyType1, KeyType2, DataType>::ForAllElements(CRuLRUQueueExEnumerateCallbackFunctor callback)
{
	INT32 i;

	for(i = 0; i < m_queue.Count(); i++)
	{
		LRUEntry &lruEntry = m_queue.Get(i);
		if(!callback(&lruEntry.m_key, &lruEntry.m_queueEntry->m_key, &lruEntry.m_queueEntry->m_data))
			return FALSE;
	}

	return TRUE;
}

template <class KeyType1, class KeyType2, class DataType>
BOOL CRuLRUQueueEx<KeyType1, KeyType2, DataType>::ForAllElements(CRuLRUQueueEnumerateCallBack callback, void *userData)
{
	INT32 i;

	for(i = 0; i < m_queue.Count(); i++)
	{
		LRUEntry &lruEntry = m_queue.Get(i);
		if(!callback(&lruEntry.m_queueEntry->m_key, &lruEntry.m_queueEntry->m_data, userData))
			return FALSE;
	}

	return TRUE;
}
/*
template <class KeyType1, class KeyType2, class DataType>
BOOL CRuLRUQueueEx<KeyType1, KeyType2, DataType>::ForAllElements(CRuLRUQueueEnumerateCallbackFunctor callback)
{
	INT32 i;

	for(i = 0; i < m_queue.Count(); i++)
	{
		LRUEntry &lruEntry = m_queue.Get(i);
		if(!callback(&lruEntry.m_queueEntry->m_key, &lruEntry.m_queueEntry->m_data))
			return FALSE;
	}

	return TRUE;
}
*/
template <class KeyType1, class KeyType2, class DataType>
BOOL CRuLRUQueueEx<KeyType1, KeyType2, DataType>::ForAllElements(const KeyType1 &key1, DWORD key1Hash, CRuLRUQueueEnumerateCallBack callback, void *userData)
{
	RootEntry *rootEntry;

	if(m_rootHash->Get(key1, key1Hash, rootEntry))
	{
		// Invoke callback on all elements belonging to this key
		QueueForAllElementsCallbackData callbackData;
		callbackData.m_callback = callback;
		callbackData.m_userData = userData;

		return rootEntry->m_hash->ForAllElements(QueueForAllElementsCallback, &callbackData);
	}

	return FALSE;
}

template <class KeyType1, class KeyType2, class DataType>
BOOL CRuLRUQueueEx<KeyType1, KeyType2, DataType>::QueueForAllElementsCallback(const void *key, const void *data, void *userData)
{
	QueueEntry *queueEntry = *((QueueEntry **) data);
	QueueForAllElementsCallbackData *callbackData = (QueueForAllElementsCallbackData *) userData;

	callbackData->m_callback(&queueEntry->m_key, &queueEntry->m_data, callbackData->m_userData);

	return TRUE;
}

template <class KeyType1, class KeyType2, class DataType>
BOOL CRuLRUQueueEx<KeyType1, KeyType2, DataType>::ClearCallback(const void *key, const void *data, void *userData)
{
	RootEntry *rootEntry = *((RootEntry **) data);
	
	delete rootEntry->m_hash;
	delete rootEntry;

	return TRUE;
}

template <class KeyType1, class KeyType2, class DataType>
BOOL CRuLRUQueueEx<KeyType1, KeyType2, DataType>::RemoveCallback(const void *key, const void *data, void *userData)
{
	CRuLRUQueueEx<KeyType1, KeyType2, DataType> *lruQueue = (CRuLRUQueueEx<KeyType1, KeyType2, DataType> *) userData;

	QueueEntry *queueEntry = *((QueueEntry **) data);

	lruQueue->m_usableDataSize += queueEntry->m_size;

	// Remove queue entry from the LRU queue
	lruQueue->m_queue.RemoveAtAddress(queueEntry->m_addressReference);

	// Delete the queue entry
	delete queueEntry;

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
