#pragma once

#include "../../Core/Type/RuType_Base.h"
#include "../../Core/Memory/RuMemUtil.h"
#include <map>
#include <vector>
#pragma managed(push, off)

// ************************************************************************************************************************************************************

template< class T > inline T Align( const T Ptr, INT Alignment )
{
	return (T)(((PTRVALUE)Ptr + Alignment - 1) & ~(Alignment-1));
}

// ************************************************************************************************************************************************************

class CRuMalloc_Windows
{
private:
	// Counts
	enum { POOL_COUNT = 42        };
	enum { POOL_MAX   = 32768 + 1 };

	// Forward declarations
	struct FreeMem;
	struct FPoolTable;

	// Memory pool info (32 bytes)
	struct PoolInfo
	{
		DWORD			m_bytes;			// Bytes allocated for pool
		DWORD			m_osBytes;			// Bytes aligned to page size
		DWORD			m_taken;			// Number of allocated elements in this pool, when counts down to zero can free the entire pool
		BYTE*			m_memBase;			// Memory base
		FPoolTable*		m_table;			// Index of pool
		FreeMem*		m_firstFreeMem;		// Pointer to first free memory in this pool
		PoolInfo*		m_next;
		PoolInfo**		m_prevLink;

		void Link(PoolInfo *&before)
		{
			if(before)
			{
				before->m_prevLink = &m_next;
			}

			m_next = before;
			m_prevLink = &before;
			before = this;
		}

		void Unlink()
		{
			if(m_next)
			{
				m_next->m_prevLink = m_prevLink;
			}

			*m_prevLink = m_next;
		}
	};

	// Information about a piece of free memory (8 bytes)
	struct FreeMem
	{
		FreeMem*		m_next;				// Next or MemLastPool[], always in order by pool
		DWORD			m_blocks;			// Number of consecutive free blocks here, at least 1

		PoolInfo*		GetPool()
		{
			return (PoolInfo *) ((PTRVALUE) this & 0xFFFFFFFFFFFF0000);
		}
	};

	// Pool table
	struct FPoolTable
	{
		PoolInfo*		m_firstPool;
		PoolInfo*		m_exaustedPool;
		DWORD			m_blockSize;
	};

	// Variables
	INT32				m_pageSize;
	FPoolTable			m_poolTable[POOL_COUNT];
	FPoolTable			m_osTable;
	PoolInfo*			m_poolIndirect[256];
	FPoolTable*			m_memSizeToPoolTable[POOL_MAX];
	INT32				m_memInit;
	INT32				m_osCurrent;
	INT32				m_osPeak;
	INT32				m_usedCurrent;
	INT32				m_usedPeak;
	INT32				m_currentAllocs;
	INT32				m_totalAllocs;
	DOUBLE				m_memTime;
public:
	CRuMalloc_Windows()
	:	m_memInit(0),
		m_osCurrent(0),
		m_osPeak(0),
		m_usedCurrent(0),
		m_usedPeak(0),
		m_currentAllocs(0),
		m_totalAllocs(0),
		m_memTime(0.0)
	{
	}

	void *Allocate(DWORD size)
	{
		return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
	}

	void Deallocate(void *Ptr)
	{
		if(!Ptr)
		{
			return;
		}

		HeapFree(GetProcessHeap(), 0, Ptr);
		return;
	}

	void Initialize()
	{
		m_memInit = 1;
		// Get OS page size
		SYSTEM_INFO SI;
		GetSystemInfo(&SI);
		m_pageSize = SI.dwPageSize;

		// Init tables
		m_osTable.m_firstPool = NULL;
		m_osTable.m_exaustedPool = NULL;
		m_osTable.m_blockSize = 0;

		m_poolTable[0].m_firstPool = NULL;
		m_poolTable[0].m_exaustedPool = NULL;
		m_poolTable[0].m_blockSize = 8;

		for(DWORD i = 1; i < 5; i++)
		{
			m_poolTable[i].m_firstPool = NULL;
			m_poolTable[i].m_exaustedPool = NULL;
			m_poolTable[i].m_blockSize = (8 << ((i + 1) >> 2)) + (2 << i);
		}

		for(DWORD i = 5; i < POOL_COUNT; i++)
		{
			m_poolTable[i].m_firstPool = NULL;
			m_poolTable[i].m_exaustedPool = NULL;
			m_poolTable[i].m_blockSize = (4 + ((i + 7) & 3)) << (1 + ((i + 7) >> 2));
		}

		for(DWORD i = 0; i < POOL_MAX; i++)
		{
			DWORD Index;
			for(Index = 0; m_poolTable[Index].m_blockSize < i; Index++);
			m_memSizeToPoolTable[i] = &m_poolTable[Index];
		}

		for(DWORD i = 0; i < 256; i++)
		{
			m_poolIndirect[i] = NULL;
		}
	}

protected:
	// Implementation
	void OutOfMemory()
	{
	}

	PoolInfo *CreateIndirect()
	{
		PoolInfo *indirect = (PoolInfo *) VirtualAlloc(NULL, 256 * sizeof(PoolInfo), MEM_COMMIT, PAGE_READWRITE);

		if(!indirect)
		{
			OutOfMemory();
		}

		return indirect;
	}
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
