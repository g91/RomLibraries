#ifndef _RUCOLLECTIONS_H_
#define _RUCOLLECTIONS_H_

#include "../../Core/Type/RuType_Base.h"
#include "../../Core/Memory/RuMemoryManager.h"
#include "../../Core/Exception/RuExceptions.h"
#include "../../Core/Utility/RuUtility.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

// Abstract interface used exclusively for enumeration purposes - usually subclasses implement some state storage
// for the specific type of enumerating class. Basically an empty interface.
template<class DataType>
class IRuEnumeration
{
public:
	virtual					~IRuEnumeration() { }

	virtual BOOL			Enumerate(const DataType &enumerationData) = 0;
};

// ************************************************************************************************************************************************************

template<class T>
class CRuStack
{
private:
	T*						m_stack;
	INT32					m_stackSize;
	INT32					m_stackTail;

public:
	CRuStack(int stackSize = 1);
	~CRuStack();

	BOOL					Push(T src);
	T						Pop();
	T						Peek();
	inline BOOL				IsEmpty() const { return (m_stackTail <= -1); }
	inline INT32			Count() const { return m_stackTail + 1; }
	BOOL					Contains(const T &entryData) const;

	inline void				Clear() { m_stackTail = -1; }
};

// ************************************************************************************************************************************************************

template <class DataType>
class CRuQueue
{
private:
	DataType*				m_queue;								// Queue
	INT32					m_queueSize;							// Actual size of the allocated queue
	INT32					m_itemCount;							// Number of entries on the queue
	INT32					m_queueHead;							// Index of the first item on the queue
	INT32					m_queueTail;							// Index of the last item on the queue

public:
	CRuQueue(INT32 initialQueueSize = 100);
	~CRuQueue();

	// Queue operations
	DataType				Peek();
	BOOL					Queue(DataType entry);
	BOOL					QueueAtHead(DataType entry);
	DataType				Dequeue();								// Dequeues the head, treating the queue as a FIFO queue
	DataType				DequeueTail();							// Dequeues the tail, treating the queue as a LIFO queue
	BOOL					RequeueHead();							// Re-queues the head of the queue to the tail

	BOOL					MergeAtEnd(CRuQueue<DataType> &queue);	// Merges the specified queue into this one at the end

	inline BOOL				IsEmpty() const { return (m_itemCount == 0) ? TRUE : FALSE; }
	inline INT32			Count() const { return m_itemCount; }

	void					Clear();

private:
	void					ResizeQueue(INT32 newQueueSize = 0);
};

// ************************************************************************************************************************************************************

template <class DataType>
class CRuCircularBuffer
{
private:
	DataType*				m_queue;								// Queue
	INT32					m_queueSize;							// Actual size of the allocated queue
	INT32					m_itemCount;							// Number of entries on the queue
	INT32					m_queueHead;							// Index of the first item on the queue
	INT32					m_queueTail;							// Index of the last item on the queue

public:
	CRuCircularBuffer(INT32 initialQueueSize = 100);
	~CRuCircularBuffer();

	// Queue operations
	BOOL					Queue(DataType entry);
	DataType				Dequeue();

	DataType&				Peek(INT32 index);

	inline BOOL				IsEmpty() const { return (m_itemCount == 0) ? TRUE : FALSE; }
	inline INT32			Count() const { return m_itemCount; }

	void					Clear();
};

// ************************************************************************************************************************************************************

template <class DataType>
class CRuArrayList
{
protected:
	DataType*					m_data;
	INT32						m_numEntries;
	INT32						m_numEntriesAllocated;
	INT32						m_lastIndex;

public:
	CRuArrayList(INT32 allocationSize = 1);
	CRuArrayList(const CRuArrayList<DataType> &srcList);
	~CRuArrayList();

	void						Release();

	INT32						GetMemoryFootprint();
	INT32						GetArraySize();
	BOOL						SetArraySize(INT32 arraySize);
	BOOL						SetNumEntries(INT32 numEntries);

	BOOL						Add(const DataType &entry);
	BOOL						Insert(INT32 index, DataType entry);
	BOOL						ReplaceAt(INT32 index, DataType entry);
	void						Remove(const DataType &entryData);
	void						RemoveAt(INT32 entry);
	DataType&					Get(INT32 entry);
	inline INT32				Count() const { return m_numEntries; }
	BOOL						Contains(const DataType &entryData) const;

	INT32						GetIndex(const DataType &entryData) const;
	INT32						IndexOf(const DataType &entryData) const;

	inline void					Clear() { m_numEntries = 0; }
	inline DataType*			GetArray() { return m_data; }
	inline const DataType*		GetArray() const { return m_data; }
	void						CloneFrom(const CRuArrayList<DataType> &srcList);

	inline DataType&			operator[](INT32 index);
	inline const DataType&		operator[](INT32 index) const;
};

// ************************************************************************************************************************************************************

// NOTE: One word about random access using CRuLinkedList - DON'T. Sequential access works extremely fast either forward or backward.
template <class DataType>
class CRuLinkedList
{
private:
	struct SListNode
	{
		DataType					Data;				// Actual data
		SListNode*					Prev;				// Pervious node
		SListNode*					Next;				// Next node
	};

	INT32							m_numNodes;			// Number of nodes in list
	SListNode*						m_head;				// Start of the list
	SListNode*						m_tail;				// End of the list

	// Cache data
	INT32							m_lastQueryNodeIndex;
	SListNode*						m_lastQueryNode;

	// More cache data
	CRuStack<SListNode *>			m_emptyNodes;

public:
	CRuLinkedList() : m_numNodes(0), m_head(NULL), m_tail(NULL), m_lastQueryNodeIndex(-1), m_lastQueryNode(NULL) { }
	~CRuLinkedList();

	void							Release();

	BOOL							Add(const DataType &newData);
	BOOL							Add(const DataType &newData, void **addressOut);
	inline void						RemoveAt(INT32 index) { RemoveAtAddress(FindNodeByIndex(index)); }
	void							RemoveAtAddress(void *nodeAddress);
	void							MoveToEndAtAddress(void *nodeAddress);

	inline DataType&				Get(INT32 index) { return FindNodeByIndex(index)->Data; }
	inline const DataType&			Get(INT32 index) const { return FindNodeByIndex(index)->Data; }

	inline void						Clear() { Release(); }

	// Queries
	inline INT32					Count() const { return m_numNodes; }

	// Operators
	inline DataType&				operator[](INT32 index) { return FindNodeByIndex(index)->Data; }
	inline const DataType&			operator[](INT32 index) const { return FindNodeByIndex(index)->Data; }

private:
	inline SListNode*				FindNodeByIndex(INT32 index)
	{
		// If we want the head, just return it (could be NULL)
		if(index == 0)
			return m_head;

		// Use the cache if possible
		if(m_lastQueryNodeIndex == index)
			return m_lastQueryNode;

		// Make sure the list actually has something
		if(m_head == NULL || m_tail == NULL)
			return NULL;

		INT32 curIndex = 0;
		SListNode *curNode = m_head;
		if(m_lastQueryNodeIndex >= 0 && (index > m_lastQueryNodeIndex || (m_lastQueryNodeIndex - index < index)))
		{
			curIndex = m_lastQueryNodeIndex;
			curNode = m_lastQueryNode;
		}

		// Step through to the requested index
		while(curIndex > index && curNode != NULL)
		{
			curNode = curNode->Prev;
			curIndex--;
		}

		while(curIndex < index && curNode != NULL)
		{
			curNode = curNode->Next;
			curIndex++;
		}

		if(curNode)
		{
			m_lastQueryNodeIndex = curIndex;
			m_lastQueryNode = curNode;
			return curNode;
		}

#ifdef _DEBUG
		throw CRuArgumentOutOfRangeException();
#else
		return NULL;
#endif
	}
};

// ************************************************************************************************************************************************************

/*!
CRuBinaryHeap
@author Ceranos
@version Initial
*/
template <class KeyType, class DataType>
class CRuBinaryHeap
{
private:
	INT32								m_numElements;				// Number of elements in the array
	INT32								m_heapArraySize;			// Array size
	KeyType*							m_heapKeyArray;				// Key array - the heap is sorted on this key
	DataType*							m_heapDataArray;			// Data array

public:
	CRuBinaryHeap();
	~CRuBinaryHeap();

	void								Push(const KeyType &key, const DataType &data);
	DataType							Pop();
	INT32								Count() const { return m_numElements; }

	inline DataType&					operator[](INT32 index) { return m_heapDataArray[index]; }
	inline const DataType&				operator[](INT32 index) const { return m_heapDataArray[index]; }

	BOOL								IncreasePriority(const DataType &data, const KeyType &key);

	inline BOOL							IsEmpty() const { return m_numElements == 0; }

	inline void							Clear() { m_numElements = 0; }

private:
	INT32								PercolateUp(INT32 elementIndex);
	INT32								PercolateDown(INT32 elementIndex);
};

// ************************************************************************************************************************************************************

/*
--------------------------------------------------------------------
CRuStack

Beginning of function definition for templatized CRuStack

--------------------------------------------------------------------
*/

template <class T>
CRuStack<T>::CRuStack(int stackSize)
:	m_stackSize(stackSize),
m_stackTail(-1),
m_stack(NULL)
{
	if(stackSize < 10)
		stackSize = 10;
	m_stackSize = stackSize;
	m_stack = ruNEW T [stackSize];
};

template<class T>
CRuStack<T>::~CRuStack()
{
	delete[] m_stack;
}

template<class T>
BOOL CRuStack<T>::Push(T src)
{
	if(m_stackTail >= (m_stackSize - 1))
	{
		// Reallocate a larger stack
		T *oldStack = m_stack;
		m_stack = ruNEW T [m_stackSize * 2];
		memcpy(m_stack, oldStack, m_stackSize * sizeof(T));
		m_stackSize *= 2;

		// Delete old stack
		delete[] oldStack;
	}

	m_stack[++m_stackTail] = src;

	return TRUE;
}

template<class T>
T CRuStack<T>::Pop()
{
	if(m_stackTail > -1)
		return m_stack[m_stackTail--];

#ifdef _DEBUG
	throw CRuStackEmptyException();
#endif

	return m_stack[0];
}

template<class T>
T CRuStack<T>::Peek()
{
	if(m_stackTail > -1)
		return m_stack[m_stackTail];

#ifdef _DEBUG
	throw CRuStackEmptyException();
#endif

	return m_stack[0];
}

template<class T>
BOOL CRuStack<T>::Contains(const T &entryData) const
{
	INT32 stackPos = m_stackTail;

	while(stackPos > -1)
	{
		if(m_stack[stackPos] == entryData)
		{
			return TRUE;
		}

		--stackPos;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

/*
--------------------------------------------------------------------
CRuQueue

Beginning of function definition for templatized CRuQueue

--------------------------------------------------------------------
*/

template <class DataType>
CRuQueue<DataType>::CRuQueue(INT32 initialQueueSize)
: m_queue(NULL), m_queueSize(initialQueueSize), m_itemCount(0), m_queueHead(0), m_queueTail(0)
{
	// Prevent idiotic usages
	if(m_queueSize <= 1)
		m_queueSize = 2;

	m_queue = ruNEW DataType [m_queueSize];
}

template <class DataType>
CRuQueue<DataType>::~CRuQueue()
{
	delete[] m_queue;
}

template <class DataType>
DataType CRuQueue<DataType>::Peek()
{
#ifdef _DEBUG
	if(m_itemCount == 0)
		throw CRuQueueEmptyException();
#endif

	DataType &dequeuedEntry = m_queue[m_queueHead];

	return dequeuedEntry;
}

template <class DataType>
BOOL CRuQueue<DataType>::Queue(DataType entry)
{
	if(m_itemCount == m_queueSize)
		ResizeQueue();

	// Insert item at tail
	m_queue[m_queueTail] = entry;
	m_itemCount++;

	// Advance tail
	m_queueTail++;
	if(m_queueTail == m_queueSize)
		m_queueTail = 0;

	return TRUE;
}

template <class DataType>
BOOL CRuQueue<DataType>::QueueAtHead(DataType entry)
{
	if(m_itemCount == m_queueSize)
		ResizeQueue();

	// Insert item at head
	--m_queueHead;
	if(m_queueHead < 0)
	{
		m_queueHead = m_queueSize - 1;
	}

	m_queue[m_queueHead] = entry;
	m_itemCount++;

	return TRUE;
}

template <class DataType>
BOOL CRuQueue<DataType>::MergeAtEnd(CRuQueue<DataType> &srcQueue)
{
	INT32 mergedQueueSize = m_itemCount + srcQueue.Count();

	if(mergedQueueSize >= m_queueSize)
		ResizeQueue(mergedQueueSize * 2);

	if(srcQueue.m_itemCount > 0)
	{
		INT32 copyCount = 0;

		INT32 dstBlock0Head = m_queueTail;
		INT32 dstBlock0Size = (m_queueTail > m_queueHead || m_itemCount == 0) ? m_queueSize - m_queueTail : m_queueHead - m_queueTail;
		INT32 dstBlock1Head = 0;
		INT32 dstBlock1Size = (m_queueTail > m_queueHead || m_itemCount == 0) ? m_queueHead : 0;

		INT32 srcBlock0Head = srcQueue.m_queueHead;
		INT32 srcBlock0Size = (srcQueue.m_queueTail > srcQueue.m_queueHead || srcQueue.m_itemCount == 0) ? (srcQueue.m_queueTail - srcQueue.m_queueHead) : (srcQueue.m_queueSize - srcQueue.m_queueHead);
		INT32 srcBlock1Head = 0;
		INT32 srcBlock1Size = (srcQueue.m_queueTail > srcQueue.m_queueHead || srcQueue.m_itemCount == 0) ? 0 : srcQueue.m_queueTail;

		// Copy as much of source block 0 into destination block 0 as possible
		copyCount = min(dstBlock0Size, srcBlock0Size);
		memcpy(&m_queue[dstBlock0Head], &srcQueue.m_queue[srcBlock0Head], sizeof(DataType) * copyCount);

		dstBlock0Head += copyCount;
		dstBlock0Size -= copyCount;

		srcBlock0Head += copyCount;
		srcBlock0Size -= copyCount;

		// More of source block 0 remain -- destination block 1 must be able to fit all of the remaining data
		if(srcBlock0Size > 0)
		{
			// Copy remainder of source block 0 into destination block 1
			copyCount = srcBlock0Size;
			memcpy(&m_queue[dstBlock1Head], &srcQueue.m_queue[srcBlock0Head], sizeof(DataType) * copyCount);

			dstBlock1Head += copyCount;
			dstBlock1Size -= copyCount;

			srcBlock0Head += copyCount;
			srcBlock0Size -= copyCount;

			// Copy all of source block 1 into destination block 1
			memcpy(&m_queue[dstBlock1Head], &srcQueue.m_queue[srcBlock1Head], sizeof(DataType) * srcBlock1Size);
		}
		// No more of source block 0 remain
		else
		{
			// Copy as much of source block 1 into destination block 0 as possible
			copyCount = min(dstBlock0Size, srcBlock1Size);
			memcpy(&m_queue[dstBlock0Head], &srcQueue.m_queue[srcBlock1Head], sizeof(DataType) * copyCount);

			dstBlock0Head += copyCount;
			dstBlock0Size -= copyCount;

			srcBlock1Head += copyCount;
			srcBlock1Size -= copyCount;

			if(srcBlock1Size > 0)
			{
				// Copy remainder of source block 1 into destination block 1
				memcpy(&m_queue[dstBlock1Head], &srcQueue.m_queue[srcBlock1Head], sizeof(DataType) * srcBlock1Size);
			}
		}

		// Store new item count
		m_itemCount = mergedQueueSize;

		// Update queue tail
		m_queueTail = (m_queueTail + srcQueue.Count()) % m_queueSize;
	}

	return TRUE;
}

template <class DataType>
DataType CRuQueue<DataType>::Dequeue()
{
#ifdef _DEBUG
	if(m_itemCount == 0)
		throw CRuQueueEmptyException();
#endif

	DataType &dequeuedEntry = m_queue[m_queueHead];
	m_itemCount--;
	m_queueHead++;
	if(m_queueHead == m_queueSize)
		m_queueHead = 0;

	return dequeuedEntry;
}

template <class DataType>
DataType CRuQueue<DataType>::DequeueTail()
{
#ifdef _DEBUG
	if(m_itemCount == 0)
		throw CRuQueueEmptyException();
#endif

	// Determine the index to dequeue (subtract 1 from the queue tail, wrap around if necessary)
	INT32 dequeuedIndex = m_queueTail - 1;
	if(dequeuedIndex < 0)
		dequeuedIndex = m_queueSize - 1;

	DataType &dequeuedEntry = m_queue[dequeuedIndex];
	m_itemCount--;

	m_queueTail = dequeuedIndex;

	return dequeuedEntry;
}

template <class DataType>
BOOL CRuQueue<DataType>::RequeueHead()
{
	if(m_itemCount > 1)
	{
		DataType &dequeuedEntry = m_queue[m_queueHead];

		// Advance queue head
		m_queueHead++;
		if(m_queueHead == m_queueSize)
			m_queueHead = 0;

		// Insert item at tail
		m_queue[m_queueTail] = dequeuedEntry;

		// Advance queue tail
		m_queueTail++;
		if(m_queueTail == m_queueSize)
			m_queueTail = 0;

		return TRUE;
	}

	return FALSE;
}

template <class DataType>
void CRuQueue<DataType>::Clear()
{
	m_itemCount = 0;
	m_queueHead = 0;
	m_queueTail = 0;
}

template <class DataType>
void CRuQueue<DataType>::ResizeQueue(INT32 newQueueSize)
{
	// If specified queue size is zero, simply double the current queue size
	newQueueSize = (newQueueSize <= 0) ? m_queueSize * 2 : newQueueSize;

	// Allocate new array for the queue
	DataType *newQueue = ruNEW DataType [newQueueSize];

	// Copy old data over to the new array
	if(m_itemCount > 0)
	{
		if(m_queueHead < m_queueTail)
		{
			// Queue is in one segment, so perform a copy directly
			memcpy(newQueue, &m_queue[m_queueHead], sizeof(DataType) * m_itemCount);
		}
		else
		{
			// Copy from queue head to end of queue array
			memcpy(newQueue, &m_queue[m_queueHead], sizeof(DataType) * (m_queueSize - m_queueHead));

			// Copy from beginning of queue array to queue tail - 1
			if(m_queueTail > 0)
				memcpy(&newQueue[m_queueSize - m_queueHead], m_queue, sizeof(DataType) * m_queueTail);
		}

		m_queueHead = 0;
		m_queueTail = m_itemCount;
	}

	// Delete old queue array and assign new queue array to it
	delete[] m_queue;
	m_queue = newQueue;
	m_queueSize = newQueueSize;
}

// ************************************************************************************************************************************************************

/*
--------------------------------------------------------------------
CRuCircularBuffer

Beginning of function definition for templatized CRuCircularBuffer

--------------------------------------------------------------------
*/

template <class DataType>
CRuCircularBuffer<DataType>::CRuCircularBuffer(INT32 initialQueueSize)
: m_queue(NULL), m_queueSize(initialQueueSize), m_itemCount(0), m_queueHead(0), m_queueTail(0)
{
	// Prevent idiotic usages
	if(m_queueSize <= 1)
		m_queueSize = 2;

	m_queue = ruNEW DataType [m_queueSize];
}

template <class DataType>
CRuCircularBuffer<DataType>::~CRuCircularBuffer()
{
	delete[] m_queue;
}

template <class DataType>
BOOL CRuCircularBuffer<DataType>::Queue(DataType entry)
{
	// Insert item at tail
	m_queue[m_queueTail] = entry;
	m_itemCount++;

	// Advance tail
	m_queueTail++;
	if(m_queueTail == m_queueSize)
		m_queueTail = 0;

	// Advance queue head if we have overrun the buffer
	if(m_queueHead == m_queueTail)
	{
		--m_itemCount;
		++m_queueHead;

		if(m_queueHead == m_queueSize)
			m_queueHead = 0;
	}

	return TRUE;
}

template <class DataType>
DataType CRuCircularBuffer<DataType>::Dequeue()
{
#ifdef _DEBUG
	if(m_itemCount == 0)
		throw CRuQueueEmptyException();
#endif

	DataType &dequeuedEntry = m_queue[m_queueHead];
	m_itemCount--;
	m_queueHead++;
	if(m_queueHead == m_queueSize)
		m_queueHead = 0;

	return dequeuedEntry;
}

template <class DataType>
DataType &CRuCircularBuffer<DataType>::Peek(INT32 index)
{
#ifdef _DEBUG
	if(m_itemCount == 0 || index >= m_itemCount)
		throw CRuQueueEmptyException();
#endif

	INT32 peekIndex = m_queueHead + index;

	while(peekIndex >= m_queueSize)
	{
		peekIndex -= m_queueSize;
	}

	return m_queue[peekIndex];
}

template <class DataType>
void CRuCircularBuffer<DataType>::Clear()
{
	m_itemCount = 0;
	m_queueHead = 0;
	m_queueTail = 0;
}

// ************************************************************************************************************************************************************

/*
--------------------------------------------------------------------
CRuArrayList

Beginning of function definition for templatized CRuArrayList

--------------------------------------------------------------------
*/

template <class DataType>
CRuArrayList<DataType>::CRuArrayList(INT32 allocationSize)
{
	if(allocationSize == 0)
		m_data = NULL;
	else
		m_data = ruNEW DataType [allocationSize];
	m_numEntries = 0;
	m_numEntriesAllocated = allocationSize;
	m_lastIndex = m_numEntriesAllocated - 1;
}

template <class DataType>
CRuArrayList<DataType>::CRuArrayList(const CRuArrayList<DataType> &srcList)
{
	m_data = NULL;
	m_numEntries = 0;
	m_numEntriesAllocated = 0;
	m_lastIndex = -1;

	CloneFrom(srcList);
}

template <class DataType>
CRuArrayList<DataType>::~CRuArrayList()
{
	if(m_data != NULL)
		delete[] m_data;
}

template <class DataType>
void CRuArrayList<DataType>::Release()
{
	if(m_data != NULL)
		delete[] m_data;
	m_data = NULL;
	m_numEntries = 0;
	m_numEntriesAllocated = 0;
}

template <class DataType>
INT32 CRuArrayList<DataType>::GetMemoryFootprint()
{
	INT32 footprint = 0;

	footprint += sizeof(m_data) + sizeof(DataType) * m_numEntriesAllocated;
	footprint += sizeof(m_numEntries);
	footprint += sizeof(m_numEntriesAllocated);
	footprint += sizeof(m_lastIndex);

	return footprint;
}

template <class DataType>
INT32 CRuArrayList<DataType>::GetArraySize()
{
	return m_numEntriesAllocated;
}

template <class DataType>
BOOL CRuArrayList<DataType>::SetArraySize(INT32 arraySize)
{
	// If array size is zero, force delete of the data array
	if(arraySize == 0)
	{
		delete[] m_data;

		m_data = NULL;
		m_numEntries = 0;
		m_numEntriesAllocated = 0;
		m_lastIndex = -1;

		return TRUE;
	}

	// If array size is smaller than or equal to the number of entries, abort
	if(arraySize <= m_numEntries)
	{
		return FALSE;
	}

	DataType *dataNew;
	dataNew = ruNEW DataType [arraySize];

	if(dataNew == NULL)
		return FALSE;

	if(m_data != NULL)
	{
		memcpy(dataNew, m_data, m_numEntries * sizeof(DataType));
		delete[] m_data;
	}

	m_data = dataNew;
	m_numEntriesAllocated = arraySize;
	m_lastIndex = m_numEntriesAllocated - 1;

	return TRUE;
}

template <class DataType>
BOOL CRuArrayList<DataType>::SetNumEntries(INT32 numEntries)
{
	if(m_numEntriesAllocated < numEntries && SetArraySize(numEntries) == FALSE)
	{
		return FALSE;
	}

	m_numEntries = numEntries;

	return TRUE;
}

template <class DataType>
BOOL CRuArrayList<DataType>::Add(const DataType &entry)
{
	if(m_data == NULL || m_numEntries >= m_lastIndex)
	{
		DataType *dataNew;
		INT32 numEntriesAllocatedNew;
		if(m_numEntriesAllocated == 0)
			numEntriesAllocatedNew = 16;
		else
			numEntriesAllocatedNew = m_numEntriesAllocated * 2;
		dataNew = ruNEW DataType [numEntriesAllocatedNew];
		if(dataNew == NULL)
			return FALSE;
		if(m_data != NULL)
		{
			memcpy(dataNew, m_data, m_numEntries * sizeof(DataType));
			delete[] m_data;
		}
		m_data = dataNew;
		m_numEntriesAllocated = numEntriesAllocatedNew;
		m_lastIndex = m_numEntriesAllocated - 1;
	}

	m_data[m_numEntries] = entry;
	++m_numEntries;

	return TRUE;
}

template <class DataType>
BOOL CRuArrayList<DataType>::Insert(INT32 index, DataType entry)
{
	if(m_data == NULL || m_numEntries + 1 > m_numEntriesAllocated)
	{
		DataType *dataNew;
		INT32 numEntriesAllocatedNew;
		if(m_numEntriesAllocated == 0)
			numEntriesAllocatedNew = 16;
		else
			numEntriesAllocatedNew = m_numEntriesAllocated * 2;
		dataNew = ruNEW DataType [numEntriesAllocatedNew];
		if(dataNew == NULL)
			return FALSE;
		if(m_data != NULL)
		{
			memcpy(dataNew, m_data, m_numEntries * sizeof(DataType));
			delete[] m_data;
		}
		m_data = dataNew;
		m_numEntriesAllocated = numEntriesAllocatedNew;
	}

	if(index > m_numEntries)
		index = m_numEntries;

	int entriesToMove = m_numEntries - index;
	if(entriesToMove > 0)
		memmove(&m_data[index + 1], &m_data[index], entriesToMove * sizeof(DataType));

	m_data[index] = entry;
	m_numEntries++;

	return TRUE;
}

template <class DataType>
BOOL CRuArrayList<DataType>::ReplaceAt(INT32 index, DataType entry)
{
	if(index < 0 || index >= m_numEntries)
		return FALSE;

	m_data[index] = entry;

	return TRUE;
}

template <class DataType>
void CRuArrayList<DataType>::Remove(const DataType &entryData)
{
	for(INT32 index = 0; index < m_numEntries; index++)
	{
		if(m_data[index] == entryData)
		{
			RemoveAt(index);
			return;
		}
	}
}

template <class DataType>
void CRuArrayList<DataType>::RemoveAt(INT32 entry)
{
	// Decrement count
	m_numEntries--;

	// Collapse the array
	if(m_numEntries - entry > 0)
	{
		memmove(&m_data[entry], &m_data[entry + 1], (m_numEntries - entry) * sizeof(DataType));
	}
}

template <class DataType>
DataType &CRuArrayList<DataType>::Get(INT32 entry)
{
#ifdef _DEBUG
	if(entry < 0 || entry >= m_numEntries)
		throw CRuArgumentOutOfRangeException();
#endif

	return m_data[entry];
}

template <class DataType>
BOOL CRuArrayList<DataType>::Contains(const DataType &entryData) const
{
	for(INT32 entry = 0; entry < m_numEntries; entry++)
	{
		if(m_data[entry] == entryData)
			return TRUE;
	}

	return FALSE;
}

template <class DataType>
INT32 CRuArrayList<DataType>::GetIndex(const DataType &entryData) const
{
	for(INT32 entry = 0; entry < m_numEntries; entry++)
	{
		if(m_data[entry] == entryData)
			return entry;
	}

	return 0xFFFFFFFF;
}

template <class DataType>
INT32 CRuArrayList<DataType>::IndexOf(const DataType &entryData) const
{
	for(INT32 entry = 0; entry < m_numEntries; entry++)
	{
		if(m_data[entry] == entryData)
			return entry;
	}

	return 0xFFFFFFFF;
}

template <class DataType>
void CRuArrayList<DataType>::CloneFrom(const CRuArrayList<DataType> &srcList)
{
	if(m_numEntriesAllocated >= srcList.m_numEntriesAllocated)
	{
		m_numEntries = srcList.m_numEntries;
		memcpy(m_data, srcList.m_data, sizeof(DataType) * srcList.m_numEntries);
	}
	else
	{
		Release();
		m_data = ruNEW DataType [srcList.m_numEntries];
		m_numEntries = srcList.m_numEntries;
		m_numEntriesAllocated = srcList.m_numEntries;
		memcpy(m_data, srcList.m_data, sizeof(DataType) * srcList.m_numEntries);
	}
}

template <class DataType>
DataType &CRuArrayList<DataType>::operator[](INT32 index)
{
#ifdef _DEBUG
	if(index < 0 || index >= (int) m_numEntries)
		throw CRuArgumentOutOfRangeException();
#endif

	return m_data[index];
}

template <class DataType>
const DataType &CRuArrayList<DataType>::operator[](INT32 index) const
{
#ifdef _DEBUG
	if(index < 0 || index >= (int) m_numEntries)
		throw CRuArgumentOutOfRangeException();
#endif

	return m_data[index];
}

// ************************************************************************************************************************************************************

/*
--------------------------------------------------------------------
CRuLinkedList

Beginning of function definition for templatized CRuLinkedList

--------------------------------------------------------------------
*/

template <class DataType>
CRuLinkedList<DataType>::~CRuLinkedList()
{
	Release();
}

template <class DataType>
void CRuLinkedList<DataType>::Release()
{
	SListNode *curNode, *nextNode;

	if(m_head != NULL)
	{
		curNode = m_head;
		nextNode = curNode->Next;
		while(curNode != NULL)
		{
			delete curNode;
			curNode = nextNode;
			if(curNode != NULL)
				nextNode = curNode->Next;
		}
	}

	while(!m_emptyNodes.IsEmpty())
		delete m_emptyNodes.Pop();

	m_numNodes = 0;
	m_head = NULL;
	m_tail = NULL;
	m_lastQueryNodeIndex = -1;
	m_lastQueryNode = NULL;
}

template <class DataType>
BOOL CRuLinkedList<DataType>::Add(const DataType &newData)
{
	SListNode *newNode;

	// Create a new node and fill with data
	if(m_emptyNodes.IsEmpty())
		newNode = ruNEW SListNode;
	else
		newNode = m_emptyNodes.Pop();
	newNode->Data = newData;

	// Is the list empty?
	if(m_head == NULL)
	{
		newNode->Prev = NULL;
		newNode->Next = NULL;
		m_head = newNode;
		m_tail = newNode;
		m_numNodes++;
		return TRUE;
	}

	// Add to end of list
	m_tail->Next = newNode;
	newNode->Prev = m_tail;
	newNode->Next = NULL;
	m_tail = newNode;
	m_numNodes++;

	return TRUE;
}

template <class DataType>
BOOL CRuLinkedList<DataType>::Add(const DataType &newData, void **addressOut)
{
	SListNode *newNode;

	// Create a new node and fill with data
	if(m_emptyNodes.IsEmpty())
		newNode = ruNEW SListNode;
	else
		newNode = m_emptyNodes.Pop();
	newNode->Data = newData;

	// Is the list empty?
	if(m_head == NULL)
	{
		newNode->Prev = NULL;
		newNode->Next = NULL;
		m_head = newNode;
		m_tail = newNode;
		m_numNodes++;

		*addressOut = newNode;

		return TRUE;
	}

	// Add to end of list
	m_tail->Next = newNode;
	newNode->Prev = m_tail;
	newNode->Next = NULL;
	m_tail = newNode;
	m_numNodes++;

	*addressOut = newNode;

	return TRUE;
}

template <class DataType>
inline void CRuLinkedList<DataType>::RemoveAtAddress(void *nodeAddress)
{
	SListNode *node = (SListNode *) nodeAddress;

	if(node->Prev && node->Next)
	{
		// Update cache node
		//		m_lastQueryNode = node->Next;

		// Upate next & prev pointers & delete
		node->Prev->Next = node->Next;
		node->Next->Prev = node->Prev;
		m_numNodes--;

		m_emptyNodes.Push(node);
	}
	else if(node->Prev)
	{
		// Update cache node
		//		m_lastQueryNodeIndex--;
		//		m_lastQueryNode = node->Prev;

		// Upate next & prev pointers & delete
		node->Prev->Next = NULL;
		m_tail = node->Prev;
		m_numNodes--;

		m_emptyNodes.Push(node);
	}
	else if(node->Next)
	{
		// Update cache node
		//		m_lastQueryNode = node->Next;

		// Upate next & prev pointers & delete
		node->Next->Prev = NULL;
		m_head = node->Next;
		m_numNodes--;

		m_emptyNodes.Push(node);
	}
	else
	{
		// Update cache node
		//		m_lastQueryNodeIndex = -1;
		//		m_lastQueryNode = NULL;

		m_head = 0;
		m_tail = 0;
		m_numNodes--;

		m_emptyNodes.Push(node);
	}

	// Reset cache
	m_lastQueryNodeIndex = -1;
	m_lastQueryNode = NULL;
}

template <class DataType>
inline void CRuLinkedList<DataType>::MoveToEndAtAddress(void *nodeAddress)
{
	SListNode *node = (SListNode *) nodeAddress;

	if(node->Prev && node->Next)
	{
		// Update cache node
		//		m_lastQueryNode = node->Next;

		// Upate next & prev pointers
		node->Prev->Next = node->Next;
		node->Next->Prev = node->Prev;

		// Reattach node at end of list
		m_tail->Next = node;
		node->Prev = m_tail;
		node->Next = NULL;
		m_tail = node;
	}
	else if(node->Next)
	{
		// Update cache node
		//		m_lastQueryNode = node->Next;

		// Upate next & prev pointers
		node->Next->Prev = NULL;
		m_head = node->Next;

		// Reattach node at end of list
		m_tail->Next = node;
		node->Prev = m_tail;
		node->Next = NULL;
		m_tail = node;
	}

	// Reset cache
	m_lastQueryNodeIndex = -1;
	m_lastQueryNode = NULL;
}

// ************************************************************************************************************************************************************

/*
--------------------------------------------------------------------
CRuBinaryHeap

Beginning of function definition for templatized CRuBinaryHeap

--------------------------------------------------------------------
*/

template <class KeyType, class DataType>
CRuBinaryHeap<KeyType, DataType>::CRuBinaryHeap()
{
	m_numElements = 0;
	m_heapArraySize = 32;

	m_heapKeyArray = ruNEW KeyType [m_heapArraySize];
	m_heapDataArray = ruNEW DataType [m_heapArraySize];
}

template <class KeyType, class DataType>
CRuBinaryHeap<KeyType, DataType>::~CRuBinaryHeap()
{
	delete[] m_heapKeyArray;
	delete[] m_heapDataArray;
}

template <class KeyType, class DataType>
void CRuBinaryHeap<KeyType, DataType>::Push(const KeyType &key, const DataType &data)
{
	// Resize heap array if necessary
	if(m_numElements == m_heapArraySize)
	{
		m_heapArraySize *= 2;
		KeyType *newKeyArray = ruNEW KeyType [m_heapArraySize];
		DataType *newDataArray = ruNEW DataType [m_heapArraySize];

		memcpy(newKeyArray, m_heapKeyArray, sizeof(KeyType) * m_numElements);
		memcpy(newDataArray, m_heapDataArray, sizeof(DataType) * m_numElements);

		delete[] m_heapKeyArray;
		delete[] m_heapDataArray;
		m_heapKeyArray = newKeyArray;
		m_heapDataArray = newDataArray;
	}

	// Insert element at end of heap
	m_heapKeyArray[m_numElements] = key;
	m_heapDataArray[m_numElements] = data;
	++m_numElements;

	// Percolate element up the heap
	PercolateUp(m_numElements - 1);
}

template <class KeyType, class DataType>
DataType CRuBinaryHeap<KeyType, DataType>::Pop()
{
#ifdef _DEBUG
	if(m_numElements == 0)
		throw CRuHeapEmptyException();
#endif

	DataType poppedElement = m_heapDataArray[0];

	// Move last element to the root
	--m_numElements;
	m_heapKeyArray[0] = m_heapKeyArray[m_numElements];
	m_heapDataArray[0] = m_heapDataArray[m_numElements];

	// Percolate element down the heap
	PercolateDown(0);

	return poppedElement;
}

template <class KeyType, class DataType>
BOOL CRuBinaryHeap<KeyType, DataType>::IncreasePriority(const DataType &data, const KeyType &key)
{
	INT32 i;

	for(i = 0; i < m_numElements; i++)
	{
		if(m_heapDataArray[i] == data)
		{
			if(m_heapKeyArray[i] > key)
			{
				m_heapKeyArray[i] = key;
				PercolateUp(PercolateDown(i));
				return TRUE;
			}

			return FALSE;
		}
	}

	return FALSE;
}

template <class KeyType, class DataType>
INT32 CRuBinaryHeap<KeyType, DataType>::PercolateUp(INT32 elementIndex)
{
	INT32 parentIndex = elementIndex / 2;

	// While the element is smaller than its parent and isn't the root element
	while(elementIndex > 0 && m_heapKeyArray[parentIndex] > m_heapKeyArray[elementIndex])
	{
		// Swap the element with its parent
		KeyType tempKey = m_heapKeyArray[parentIndex];
		DataType tempData = m_heapDataArray[parentIndex];

		m_heapKeyArray[parentIndex] = m_heapKeyArray[elementIndex];
		m_heapDataArray[parentIndex] = m_heapDataArray[elementIndex];

		m_heapKeyArray[elementIndex] = tempKey;
		m_heapDataArray[elementIndex] = tempData;

		// Move up the heap
		elementIndex = parentIndex;
		parentIndex = elementIndex / 2;
	}

	return elementIndex;
}

template <class KeyType, class DataType>
INT32 CRuBinaryHeap<KeyType, DataType>::PercolateDown(INT32 elementIndex)
{
	INT32 swapIndex;
	INT32 childIndex0 = elementIndex * 2;
	INT32 childIndex1 = childIndex0 + 1;

	// While the element isn't a leaf element
	while(childIndex0 < m_numElements)
	{
		if(childIndex1 < m_numElements)
		{
			if(m_heapKeyArray[childIndex0] <= m_heapKeyArray[childIndex1])
			{
				// If the element is smaller than both its children, return
				if(m_heapKeyArray[elementIndex] <= m_heapKeyArray[childIndex0])
				{
					return elementIndex;
				}

				// Swap element with the smaller child
				swapIndex = childIndex0;
			}
			else
			{
				// If the element is smaller than both its children, return
				if(m_heapKeyArray[elementIndex] <= m_heapKeyArray[childIndex1])
				{
					return elementIndex;
				}

				// Swap element with the smaller child
				swapIndex = childIndex1;
			}
		}
		else
		{
			// If the element is smaller than its lone child, return
			if(m_heapKeyArray[elementIndex] <= m_heapKeyArray[childIndex0])
			{
				return elementIndex;
			}

			// Swap element with the child
			swapIndex = childIndex0;
		}

		// Swap the element with its child
		KeyType tempKey = m_heapKeyArray[swapIndex];
		DataType tempData = m_heapDataArray[swapIndex];

		m_heapKeyArray[swapIndex] = m_heapKeyArray[elementIndex];
		m_heapDataArray[swapIndex] = m_heapDataArray[elementIndex];

		m_heapKeyArray[elementIndex] = tempKey;
		m_heapDataArray[elementIndex] = tempData;

		elementIndex = swapIndex;
		childIndex0 = elementIndex * 2;
		childIndex1 = childIndex0 + 1;
	}

	return elementIndex;
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
