#include <assert.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>

#include "../../Core/Memory/RuMemoryHeap.h"
#include "../../Core/Memory/RuMemoryHeapFactory.h"

#define MEMSYSTEM_SIGNATURE  0xDEADC0DE

#pragma managed(push, off)

volatile LONG				CRuMemoryHeap::s_nextAllocNum = 1;

const char*					CRuMemoryHeap::s_filename = "Unknown";
const char*					CRuMemoryHeap::s_function = "Unknown";
UINT32						CRuMemoryHeap::s_line = 0;

struct CRuMemoryAllocHeader
{
    INT32						m_signature;
    INT32						m_allocNum;
    INT32						m_size;
    CRuMemoryHeap*				m_heap;
    CRuMemoryAllocHeader*		m_next;
    CRuMemoryAllocHeader*		m_prev;

#ifdef RUMEMORYMANAGER_DEBUG
	const char*					m_filename;
	const char*					m_function;
	UINT32						m_line;
#endif
};

void CRuMemoryHeap::Initialize()
{
    m_active = FALSE;
    m_bytesAllocated = 0;
    m_bytesPeak = 0;
    m_numInstances = 0;
    m_headAlloc = NULL;

    m_parent = NULL;
    m_firstChild = NULL;
    m_nextSibling = NULL;
    m_prevSibling = NULL;
}

void CRuMemoryHeap::Activate(const char* name)
{
    assert (name != NULL);
    assert (strlen(name) < NAMELENGTH);

    strcpy (m_name, name);
    m_bytesAllocated = 0;
    m_bytesPeak = 0;
    m_numInstances = 0;
    m_active = TRUE;
}

void CRuMemoryHeap::Deactivate()
{
    strcpy (m_name, "");

    // Destroy chain
    m_bytesAllocated = 0;
    m_bytesPeak = 0;
    m_numInstances = 0;
    m_active = FALSE;
}

const char *CRuMemoryHeap::GetName() const
{
	return m_name;
}

void CRuMemoryHeap::PrintTreeInfo(INT32 indentLevel) const
{
	PrintInfo(indentLevel);

	CRuMemoryHeap *pChild = m_firstChild;
	while(pChild != NULL)
	{
		pChild->PrintTreeInfo(indentLevel + 1);
		pChild = pChild->m_nextSibling;
	}
}

void CRuMemoryHeap::PrintInfo(INT32 indentLevel) const
{
	for(INT32 i = 0; i < indentLevel; ++i)
		OutputDebugString("  ");

	size_t totalBytes = 0;
	size_t totalPeakBytes = 0;
	INT32 totalInstances = 0;
	GetTreeStats(totalBytes, totalPeakBytes, totalInstances);

	INT32 spacing = 60 - indentLevel * 2;
	char outputString[2048];
	sprintf(outputString, "%-*s %10d %10d %10d | %10d %10d %10d\n", spacing, m_name, m_bytesAllocated, m_bytesPeak, m_numInstances,totalBytes, totalPeakBytes, totalInstances);
	OutputDebugString(outputString);
}

void CRuMemoryHeap::GetTreeStats(size_t &totalBytes, size_t &totalPeak, INT32 &totalInstances) const
{
	totalBytes += m_bytesAllocated;
	totalPeak += m_bytesPeak;
	totalInstances += m_numInstances;

	CRuMemoryHeap *pChild = m_firstChild;
	while(pChild != NULL)
	{
		pChild->GetTreeStats(totalBytes, totalPeak, totalInstances);
		pChild = pChild->m_nextSibling;
	}
}

void *CRuMemoryHeap::Allocate(size_t bytes)
{
	// Acquire global critical section lock
	CRuMemoryHeapFactory::AcquireLock();

	size_t nRequestedBytes = bytes + sizeof(CRuMemoryAllocHeader);
//	char *pMem = (char *) malloc(nRequestedBytes);
	char *pMem = (char *) CRuMemoryAllocator::Allocate(nRequestedBytes);
	CRuMemoryAllocHeader *pHeader = (CRuMemoryAllocHeader *) pMem;

	#ifdef RUMEMORYMANAGER_DEBUG
		pHeader->m_filename = s_filename;
		pHeader->m_function = s_function;
		pHeader->m_line = s_line;

		s_filename = "Unknown";
		s_function = "Unknown";
		s_line = 0;
	#endif

	pHeader->m_signature = MEMSYSTEM_SIGNATURE;
	pHeader->m_heap = this;
	pHeader->m_size = (INT32)bytes;
	pHeader->m_next = m_headAlloc;
	pHeader->m_prev = NULL;
	pHeader->m_allocNum = InterlockedIncrement(&s_nextAllocNum) - 1;

	if (m_headAlloc != NULL)
		m_headAlloc->m_prev = pHeader;
	m_headAlloc = pHeader;

	m_bytesAllocated += bytes;
	if (m_bytesAllocated > m_bytesPeak)
		m_bytesPeak = m_bytesAllocated;
	m_numInstances++;

	void *pStartMemBlock = pMem + sizeof(CRuMemoryAllocHeader);

	// Release global critical section lock
	CRuMemoryHeapFactory::ReleaseLock();

	return pStartMemBlock; 
}

void CRuMemoryHeap::Deallocate(void *pMem)
{
	CRuMemoryAllocHeader *pHeader = (CRuMemoryAllocHeader *) ((char *) pMem - sizeof(CRuMemoryAllocHeader));
	assert(pHeader->m_signature == MEMSYSTEM_SIGNATURE);
	pHeader->m_heap->Deallocate(pHeader);
}

void CRuMemoryHeap::Deallocate(CRuMemoryAllocHeader *pHeader)
{
	// Acquire global critical section lock
	CRuMemoryHeapFactory::AcquireLock();

	if(pHeader->m_prev == NULL)
	{
		assert(pHeader == m_headAlloc);
		m_headAlloc = pHeader->m_next;
	}        
	else
	{
		pHeader->m_prev->m_next = pHeader->m_next;
	}

	if(pHeader->m_next != NULL)
		pHeader->m_next->m_prev = pHeader->m_prev;   

	m_bytesAllocated -= pHeader->m_size;
	m_numInstances--;

//	free (pHeader);
	CRuMemoryAllocator::Deallocate(pHeader);

	// Release global critical section lock
	CRuMemoryHeapFactory::ReleaseLock();
}

INT32 CRuMemoryHeap::ReportMemoryLeaks(INT32 nBookmark1, INT32 nBookmark2)
{
	INT32 nLeaks = 0;

	CRuMemoryAllocHeader* pHeader = m_headAlloc;
	while(pHeader != NULL) 
	{
		if(pHeader->m_allocNum >= nBookmark1 && pHeader->m_allocNum < nBookmark2)
		{
			char outputString[2048];

			#ifdef RUMEMORYMANAGER_DEBUG
				sprintf(outputString, "Leak in %s. Size: %d, address: 0x%0Xd file:%s function:%s line:%u\n", m_name, pHeader->m_size, (char *) pHeader + sizeof(CRuMemoryAllocHeader), pHeader->m_filename, pHeader->m_function, pHeader->m_line);
			#else
				sprintf(outputString, "Leak in %s. Size: %d, address: 0x%0Xd\n", m_name, pHeader->m_size, (char *) pHeader + sizeof(CRuMemoryAllocHeader));
			#endif

			OutputDebugString(outputString);

			nLeaks++;
		}

		pHeader = pHeader->m_next;
	}

	return nLeaks;
}

void CRuMemoryHeap::SetAllocationOwner(const char *filename, const char *function, const UINT32 line)
{
	// Acquire global critical section lock
//	CRuMemoryHeapFactory::AcquireLock();

	// NOTE: Without the critical section... it is possible that these following
	//       variables will get changed before new is invoked... it will not crash,
	//       however the resulting debug output will be incorrect.

	s_filename = filename;
	s_function = function;
	s_line = line;
}

void CRuMemoryHeap::ResetAllocationOwner()
{
	// Release global critical section lock
//	CRuMemoryHeapFactory::ReleaseLock();
}

INT32 CRuMemoryHeap::GetMemoryBookmark()
{
	return s_nextAllocNum;
}

void CRuMemoryHeap::AttachTo(CRuMemoryHeap *pParent)
{
	assert(pParent != NULL);

	if(pParent == m_parent)
		return;

	// First detach itself from its current parent
	if(m_prevSibling != NULL)
		m_prevSibling->m_nextSibling = m_nextSibling;

	if(m_nextSibling != NULL)
		m_nextSibling->m_prevSibling = m_prevSibling;

	if(m_parent != NULL)
	{
		if(m_parent->m_firstChild == this)
			m_parent->m_firstChild = m_nextSibling;
	}

	// Now attach itself to the new parent
	if(pParent->m_firstChild)
	{
		pParent->m_firstChild->m_prevSibling = this;
	}

	m_nextSibling = pParent->m_firstChild;
	m_prevSibling = NULL;
	m_parent = pParent;
	pParent->m_firstChild = this;
}

#pragma managed(pop)
