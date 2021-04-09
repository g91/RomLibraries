#ifndef _RUTHREAD_WORKQUEUE_
#define _RUTHREAD_WORKQUEUE_

// Base types
#include "../../Core/Collection/RuCollections.h"
#include "../../Core/Collection/RuCollectionsHash.h"
#include "../../Core/Collection/RuCollection_TernaryStringTree.h"
#include "../../Core/Type/RuType_Base.h"
#include "../../Core/Thread/RuThread_CriticalSection.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

template <class KeyType, class ResultType>
class CRuThread_WorkQueue
{
protected:
	struct ResultTuple
	{
		KeyType										m_key;
		ResultType									m_result;
	};

	CRuThread_CriticalSection*						m_workQueue_CS;

	CRuQueue<KeyType>								m_workQueue;			//!< Queue of current workload
	CRuKeyHash<KeyType>*							m_workHash;				//!< Hash for current workload
	CRuArrayList<KeyType>							m_pendingList;			//!< List of pending workloads (dequeued and waiting for result)
	CRuQueue<ResultTuple>							m_resultQueue;			//!< Queue for completed results
	CRuKeyHash<KeyType>*							m_resultHash;			//!< Hash of keys for completed results
	CRuKeyHash<KeyType>*							m_failedResultHash;		//!< Hash of keys for failed results

public:
													CRuThread_WorkQueue(INT32 workHashSize, INT32 resultHashSize, INT32 failedResultHashSize);
													~CRuThread_WorkQueue();

	BOOL											QueueWork(const KeyType &key);
	BOOL											DequeueWork(KeyType &keyOut);
	BOOL											ClearWorkQueue();

	BOOL											QueueResult(const KeyType &key, ResultType result);
	BOOL											DequeueResult(KeyType &keyOut, ResultType &resultOut);

	BOOL											MarkWorkFailed(const KeyType &key);
	BOOL											WorkFailed(const KeyType &key, BOOL clearOnCheck);

	INT32											WorkCount();
	INT32											ResultCount();

protected:
	// Prevent copying by declaring copy constructors as protected
													CRuThread_WorkQueue(const CRuThread_WorkQueue &obj);
	CRuThread_WorkQueue&							operator=(const CRuThread_WorkQueue & obj);
};

// ************************************************************************************************************************************************************

template <class ResultType>
class CRuThread_WorkQueue<CRuGUID, ResultType>
{
protected:
	struct ResultTuple
	{
		CRuGUID										m_key;
		ResultType									m_result;
	};

	CRuThread_CriticalSection*						m_workQueue_CS;
	CRuThread_CriticalSection*						m_pendingList_CS;
	CRuThread_CriticalSection*						m_resultQueue_CS;

	CRuQueue<CRuGUID>								m_workQueue;			//!< Queue of current workload
	CRuKeyHash<CRuGUID>*							m_workHash;				//!< Hash for current workload
	CRuArrayList<CRuGUID>							m_pendingList;			//!< List of pending workloads (dequeued and waiting for result)
	CRuQueue<ResultTuple>							m_resultQueue;			//!< Queue for completed results
	CRuKeyHash<CRuGUID>*							m_resultHash;			//!< Hash of keys for completed results
	CRuKeyHash<CRuGUID>*							m_failedResultHash;		//!< Hash of keys for failed results

public:
													CRuThread_WorkQueue(INT32 workHashSize, INT32 resultHashSize, INT32 failedResultHashSize);
													~CRuThread_WorkQueue();

	BOOL											QueueWork(const CRuGUID &key);
	BOOL											DequeueWork(CRuGUID &keyOut);
	BOOL											ClearWorkQueue();

	BOOL											QueueResult(const CRuGUID &key, ResultType result);
	BOOL											DequeueResult(CRuGUID &keyOut, ResultType &resultOut);

	BOOL											MarkWorkFailed(const CRuGUID &key);
	BOOL											WorkFailed(const CRuGUID &key, BOOL clearOnCheck);

	INT32											WorkCount();
	INT32											ResultCount();

protected:
	// Prevent copying by declaring copy constructors as protected
													CRuThread_WorkQueue(const CRuThread_WorkQueue &obj);
	CRuThread_WorkQueue&							operator=(const CRuThread_WorkQueue & obj);
};

// ************************************************************************************************************************************************************

template <class ResultType>
class CRuThread_WorkQueue<char *, ResultType>
{
protected:
	struct ResultTuple
	{
		char*										m_key;
		ResultType									m_result;
	};

	CRuThread_CriticalSection*						m_workQueue_CS;

	CRuQueue<char *>								m_workQueue;			//!< Queue of current workload
	CRuTernaryStringTree<INT32>						m_workTree;				//!< String tree of current workload
	CRuTernaryStringTree<INT32>						m_pendingTree;			//!< String tree of pending workloads (dequeued and waiting for result)
	CRuQueue<ResultTuple>							m_resultQueue;			//!< Queue for completed results
	CRuTernaryStringTree<INT32>						m_resultTree;			//!< String tree of completed result keys
	CRuTernaryStringTree<INT32>						m_failedResultTree;		//!< String tree of failed results keys

public:
													CRuThread_WorkQueue();
													~CRuThread_WorkQueue();

	BOOL											QueueWork(const char *key);
	BOOL											DequeueWork(char **keyOut);
	BOOL											ClearWorkQueue();

	BOOL											QueueResult(char *key, ResultType result);
	BOOL											DequeueResult(char **keyOut, ResultType &resultOut);

	BOOL											MarkWorkFailed(const char *key);
	BOOL											WorkFailed(const char *key, BOOL clearOnCheck);

	INT32											WorkCount();
	INT32											ResultCount();

protected:
	// Prevent copying by declaring copy constructors as protected
													CRuThread_WorkQueue(const CRuThread_WorkQueue &obj);
	CRuThread_WorkQueue&							operator=(const CRuThread_WorkQueue & obj);
};

// ************************************************************************************************************************************************************

template <class KeyType, class ResultType>
CRuThread_WorkQueue<KeyType, ResultType>::CRuThread_WorkQueue(INT32 workHashSize, INT32 resultHashSize, INT32 failedResultHashSize)
{
	m_workQueue_CS = ruNEW CRuThread_CriticalSection();

	m_workHash = ruNEW CRuKeyHash<KeyType>(workHashSize);
	m_resultHash = ruNEW CRuKeyHash<KeyType>(resultHashSize);
	m_failedResultHash = ruNEW CRuKeyHash<KeyType>(workHashSize);
}

template <class KeyType, class ResultType>
CRuThread_WorkQueue<KeyType, ResultType>::~CRuThread_WorkQueue()
{
	delete m_workQueue_CS;

	delete m_workHash;
	delete m_resultHash;
	delete m_failedResultHash;
}

template <class KeyType, class ResultType>
BOOL CRuThread_WorkQueue<KeyType, ResultType>::QueueWork(const KeyType &key)
{
	// Return value
	BOOL retVal = FALSE;

	// Create hash for the work key
	DWORD keyHash = RuGlobalHashGen().GetHashCode(reinterpret_cast<const BYTE *>(&key), sizeof(KeyType));

	m_workQueue_CS->Enter();

	// Check whether the work key is already pending
	BOOL workAlreadyPending = m_pendingList.Contains(key);

	// Check whether the work key is already in the result queue
	BOOL workAlreadyCompleted = m_resultHash->ContainsKey(key, keyHash);

	if(m_workHash->ContainsKey(key, keyHash) == FALSE && !workAlreadyPending && !workAlreadyCompleted)
	{
		if(m_failedResultHash->ContainsKey(key, keyHash) == FALSE)
		{
			m_workQueue.Queue(key);
			m_workHash->Add(key, keyHash);

			retVal = TRUE;
		}
	}

	m_workQueue_CS->Leave();

	return retVal;
}

template <class KeyType, class ResultType>
BOOL CRuThread_WorkQueue<KeyType, ResultType>::DequeueWork(KeyType &keyOut)
{
	BOOL retVal = FALSE;

	m_workQueue_CS->Enter();

	if(m_workQueue.Count() > 0)
	{
		// Dequeue work key
		keyOut = m_workQueue.Dequeue();

		// Remove work key from work hash
		DWORD keyHash = RuGlobalHashGen().GetHashCode(reinterpret_cast<const BYTE *>(&keyOut), sizeof(KeyType));
		m_workHash->Remove(keyOut, keyHash);

		// Insert work key into pending list
		m_pendingList.Add(keyOut);

		retVal = TRUE;
	}

	m_workQueue_CS->Leave();

	return retVal;
}

template <class KeyType, class ResultType>
BOOL CRuThread_WorkQueue<KeyType, ResultType>::ClearWorkQueue()
{
	BOOL retVal = TRUE;

	m_workQueue_CS->Enter();

	m_workQueue.Clear();
	m_workHash->Clear();

	m_workQueue_CS->Leave();

	return retVal;
}

template <class KeyType, class ResultType>
BOOL CRuThread_WorkQueue<KeyType, ResultType>::QueueResult(const KeyType &key, ResultType result)
{
	BOOL retVal = TRUE;

	// Generate result tuple and key hash
	ResultTuple resultTuple = { key, result };
	DWORD keyHash = RuGlobalHashGen().GetHashCode(reinterpret_cast<const BYTE *>(&key), sizeof(KeyType));

	// Queue result
	m_workQueue_CS->Enter();

	m_resultQueue.Queue(resultTuple);
	m_resultHash->Add(key, keyHash);

	// Remove work key from pending list
	m_pendingList.Remove(key);

	m_workQueue_CS->Leave();

	return retVal;
}

template <class KeyType, class ResultType>
BOOL CRuThread_WorkQueue<KeyType, ResultType>::DequeueResult(KeyType &keyOut, ResultType &resultOut)
{
	BOOL retVal = FALSE;

	if(m_workQueue_CS->TryEnter())
	{
		if(m_resultQueue.Count() > 0)
		{
			// Dequeue result tuple
			ResultTuple resultTuple = m_resultQueue.Dequeue();
			DWORD keyHash = RuGlobalHashGen().GetHashCode(reinterpret_cast<const BYTE *>(&resultTuple.m_key), sizeof(KeyType));
			m_resultHash->Remove(resultTuple.m_key, keyHash);

			// Output result
			keyOut = resultTuple.m_key;
			resultOut = resultTuple.m_result;

			retVal = TRUE;
		}

		m_workQueue_CS->Leave();
	}

	return retVal;
}

template <class KeyType, class ResultType>
BOOL CRuThread_WorkQueue<KeyType, ResultType>::MarkWorkFailed(const KeyType &key)
{
	BOOL retVal = FALSE;

	DWORD keyHash = RuGlobalHashGen().GetHashCode(reinterpret_cast<const BYTE *>(&key), sizeof(KeyType));

	m_workQueue_CS->Enter();

	if(m_failedResultHash->ContainsKey(key, keyHash) == FALSE)
	{
		m_failedResultHash->Add(key, keyHash);

		retVal = TRUE;
	}

	// Remove work key from pending list
	m_pendingList.Remove(key);

	m_workQueue_CS->Leave();

	return retVal;
}

template <class KeyType, class ResultType>
BOOL CRuThread_WorkQueue<KeyType, ResultType>::WorkFailed(const KeyType &key, BOOL clearOnCheck)
{
	BOOL retVal = FALSE;

	DWORD keyHash = RuGlobalHashGen().GetHashCode(reinterpret_cast<const BYTE *>(&key), sizeof(KeyType));

	m_workQueue_CS->Enter();

	if(m_failedResultHash->ContainsKey(key, keyHash))
	{
		if(clearOnCheck)
		{
			m_failedResultHash->Remove(key, keyHash);
		}

		retVal = TRUE;
	}

	m_workQueue_CS->Leave();

	return retVal;
}

template <class KeyType, class ResultType>
INT32 CRuThread_WorkQueue<KeyType, ResultType>::WorkCount()
{
	m_workQueue_CS->Enter();

	INT32 workCount = m_workQueue.Count();

	m_workQueue_CS->Leave();

	return workCount;
}

template <class KeyType, class ResultType>
INT32 CRuThread_WorkQueue<KeyType, ResultType>::ResultCount()
{
	m_workQueue_CS->Enter();

	INT32 resultCount = m_resultQueue.Count();

	m_workQueue_CS->Leave();

	return resultCount;
}

// ************************************************************************************************************************************************************

template <class ResultType>
CRuThread_WorkQueue<CRuGUID, ResultType>::CRuThread_WorkQueue(INT32 workHashSize, INT32 resultHashSize, INT32 failedResultHashSize)
{
	m_workQueue_CS = ruNEW CRuThread_CriticalSection();

	m_workHash = ruNEW CRuKeyHash<CRuGUID>(workHashSize);
	m_resultHash = ruNEW CRuKeyHash<CRuGUID>(resultHashSize);
	m_failedResultHash = ruNEW CRuKeyHash<CRuGUID>(workHashSize);
}

template <class ResultType>
CRuThread_WorkQueue<CRuGUID, ResultType>::~CRuThread_WorkQueue()
{
	delete m_workQueue_CS;

	delete m_workHash;
	delete m_resultHash;
	delete m_failedResultHash;
}

template <class ResultType>
BOOL CRuThread_WorkQueue<CRuGUID, ResultType>::QueueWork(const CRuGUID &key)
{
	// Return value
	BOOL retVal = FALSE;

	m_workQueue_CS->Enter();

	// Check whether the work key is already pending
	BOOL workAlreadyPending = m_pendingList.Contains(key);

	// Check whether the work key is already in the result queue
	BOOL workAlreadyCompleted = m_resultHash->ContainsKey(key, key.HashCode());

	if(m_workHash->ContainsKey(key, key.HashCode()) == FALSE && !workAlreadyPending && !workAlreadyCompleted)
	{
		if(m_failedResultHash->ContainsKey(key, key.HashCode()) == FALSE)
		{	
			m_workQueue.Queue(key);
			m_workHash->Add(key, key.HashCode());
			retVal = TRUE;
		}
	}

	m_workQueue_CS->Leave();

	return retVal;
}

template <class ResultType>
BOOL CRuThread_WorkQueue<CRuGUID, ResultType>::DequeueWork(CRuGUID &keyOut)
{
	BOOL retVal = FALSE;

	m_workQueue_CS->Enter();

	if(m_workQueue.Count() > 0)
	{
		// Dequeue work key
		keyOut = m_workQueue.Dequeue();

		// Remove work key from work hash
		m_workHash->Remove(keyOut, keyOut.HashCode());

		// Insert work key into pending list
		m_pendingList.Add(keyOut);

		retVal = TRUE;
	}

	m_workQueue_CS->Leave();

	return retVal;
}

template <class ResultType>
BOOL CRuThread_WorkQueue<CRuGUID, ResultType>::ClearWorkQueue()
{
	BOOL retVal = TRUE;

	m_workQueue_CS->Enter();

	m_workQueue.Clear();
	m_workHash->Clear();

	m_workQueue_CS->Leave();

	return retVal;
}

template <class ResultType>
BOOL CRuThread_WorkQueue<CRuGUID, ResultType>::QueueResult(const CRuGUID &key, ResultType result)
{
	BOOL retVal = TRUE;

	// Generate result tuple
	ResultTuple resultTuple = { key, result };

	m_workQueue_CS->Enter();

	// Queue result
	m_resultQueue.Queue(resultTuple);
	m_resultHash->Add(key, key.HashCode());

	// Remove work key from pending list
	m_pendingList.Remove(key);

	m_workQueue_CS->Leave();

	return retVal;
}

template <class ResultType>
BOOL CRuThread_WorkQueue<CRuGUID, ResultType>::DequeueResult(CRuGUID &keyOut, ResultType &resultOut)
{
	BOOL retVal = FALSE;

	if(m_workQueue_CS->TryEnter())
	{
		if(m_resultQueue.Count() > 0)
		{
			// Dequeue result tuple
			ResultTuple resultTuple = m_resultQueue.Dequeue();
			m_resultHash->Remove(resultTuple.m_key, resultTuple.m_key.HashCode());

			// Output result
			keyOut = resultTuple.m_key;
			resultOut = resultTuple.m_result;

			retVal = TRUE;
		}

		m_workQueue_CS->Leave();
	}

	return retVal;
}

template <class ResultType>
BOOL CRuThread_WorkQueue<CRuGUID, ResultType>::MarkWorkFailed(const CRuGUID &key)
{
	BOOL retVal = FALSE;

	m_workQueue_CS->Enter();

	if(m_failedResultHash->ContainsKey(key, key.HashCode()) == FALSE)
	{
		m_failedResultHash->Add(key, key.HashCode());

		retVal = TRUE;
	}

	m_workQueue_CS->Leave();

	return retVal;
}

template <class ResultType>
BOOL CRuThread_WorkQueue<CRuGUID, ResultType>::WorkFailed(const CRuGUID &key, BOOL clearOnCheck)
{
	BOOL retVal = FALSE;

	m_workQueue_CS->Enter();

	if(m_failedResultHash->ContainsKey(key, key.HashCode()))
	{
		if(clearOnCheck)
		{
			m_failedResultHash->Remove(key, key.HashCode());
		}

		retVal = TRUE;
	}

	m_workQueue_CS->Leave();

	return retVal;
}

template <class ResultType>
INT32 CRuThread_WorkQueue<CRuGUID, ResultType>::WorkCount()
{
	m_workQueue_CS->Enter();

	INT32 workCount = m_workQueue.Count();

	m_workQueue_CS->Leave();

	return workCount;
}

template <class ResultType>
INT32 CRuThread_WorkQueue<CRuGUID, ResultType>::ResultCount()
{
	m_workQueue_CS->Enter();

	INT32 resultCount = m_resultQueue.Count();

	m_workQueue_CS->Leave();

	return resultCount;
}

// ************************************************************************************************************************************************************

template <class ResultType>
CRuThread_WorkQueue<char *, ResultType>::CRuThread_WorkQueue()
{
	m_workQueue_CS = ruNEW CRuThread_CriticalSection();
}

template <class ResultType>
CRuThread_WorkQueue<char *, ResultType>::~CRuThread_WorkQueue()
{
	while(m_workQueue.Count() > 0)
	{
		char *key = m_workQueue.Dequeue();
		delete[] key;
	}

	delete m_workQueue_CS;
}

template <class ResultType>
BOOL CRuThread_WorkQueue<char *, ResultType>::QueueWork(const char *key)
{
	// Return value
	BOOL retVal = FALSE;

	m_workQueue_CS->Enter();

	// Check whether the work key is already pending
	BOOL workAlreadyPending = m_pendingTree.ContainsKey(key);

	// Check whether the work key is already in the result queue
	BOOL workAlreadyCompleted = m_resultTree.ContainsKey(key);

	if(m_workTree.ContainsKey(key) == FALSE && !workAlreadyPending && !workAlreadyCompleted)
	{
		if(m_failedResultTree.ContainsKey(key) == FALSE)
		{
			INT32 nameLength = (INT32)strlen(key) + 1;
			char *nameCopy = ruNEW char [nameLength];
			memcpy(nameCopy, key, nameLength);

			m_workQueue.Queue(nameCopy);
			m_workTree.Add(key, 0);

			retVal = TRUE;
		}
	}

	m_workQueue_CS->Leave();

	return retVal;
}

template <class ResultType>
BOOL CRuThread_WorkQueue<char *, ResultType>::DequeueWork(char **keyOut)
{
	BOOL retVal = FALSE;

	m_workQueue_CS->Enter();

	if(m_workQueue.Count() > 0)
	{
		// Dequeue work key
		char *name = m_workQueue.Dequeue();

		// Remove work key from work tree
		m_workTree.Remove(name);

		// Insert work key into pending list
		m_pendingTree.Add(name, 0);

		// Set output parameter
		*keyOut = name;

		retVal = TRUE;
	}

	m_workQueue_CS->Leave();

	return retVal;
}

template <class ResultType>
BOOL CRuThread_WorkQueue<char *, ResultType>::ClearWorkQueue()
{
	BOOL retVal = TRUE;

	m_workQueue_CS->Enter();

	while(m_workQueue.Count() > 0)
	{
		char *key = m_workQueue.Dequeue();
		delete[] key;
	}

	m_workTree.Clear();

	m_workQueue_CS->Leave();

	return retVal;
}

template <class ResultType>
BOOL CRuThread_WorkQueue<char *, ResultType>::QueueResult(char *key, ResultType result)
{
	BOOL retVal = TRUE;

	// Generate result tuple
	ResultTuple resultTuple = { key, result };

	m_workQueue_CS->Enter();

	// Queue result
	m_resultQueue.Queue(resultTuple);
	m_resultTree.Add(key, 0);

	// Remove work key from pending list
	m_pendingTree.Remove(key);

	m_workQueue_CS->Leave();

	return retVal;
}

template <class ResultType>
BOOL CRuThread_WorkQueue<char *, ResultType>::DequeueResult(char **keyOut, ResultType &result)
{
	BOOL retVal = FALSE;

	if(m_workQueue_CS->TryEnter())
	{

		if(m_resultQueue.Count() > 0)
		{
			// Dequeue the result tuple
			ResultTuple resultTuple = m_resultQueue.Dequeue();
			m_resultTree.Remove(resultTuple.m_key);

			// Output result
			*keyOut = resultTuple.m_key;
			result = resultTuple.m_result;

			retVal = TRUE;
		}

		m_workQueue_CS->Leave();
	}

	return retVal;
}

template <class ResultType>
BOOL CRuThread_WorkQueue<char *, ResultType>::MarkWorkFailed(const char *key)
{
	BOOL retVal = FALSE;

	m_workQueue_CS->Enter();

	// Mark work as failed
	retVal = m_failedResultTree.Add(key, 0);

	// Remove work key from pending list
	m_pendingTree.Remove(key);

	m_workQueue_CS->Leave();

	return retVal;
}

template <class ResultType>
BOOL CRuThread_WorkQueue<char *, ResultType>::WorkFailed(const char *key, BOOL clearOnCheck)
{
	BOOL retVal = FALSE;

	m_workQueue_CS->Enter();

	if(m_failedResultTree.ContainsKey(key))
	{
		if(clearOnCheck)
		{
			m_failedResultTree.Remove(key);
		}

		retVal = TRUE;
	}

	m_workQueue_CS->Leave();

	return retVal;
}

template <class ResultType>
INT32 CRuThread_WorkQueue<char *, ResultType>::WorkCount()
{
	m_workQueue_CS->Enter();

	INT32 workCount = m_workQueue.Count();

	m_workQueue_CS->Leave();

	return workCount;
}

template <class ResultType>
INT32 CRuThread_WorkQueue<char *, ResultType>::ResultCount()
{
	m_workQueue_CS->Enter();

	INT32 resultCount = m_resultQueue.Count();

	m_workQueue_CS->Leave();

	return resultCount;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
