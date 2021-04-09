#include <assert.h>

#include "../../Core/Memory/RuMemoryAllocator.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRITICAL_SECTION CRuMemoryAllocator::s_allocatorCS;
INT32 CRuMemoryAllocator::s_numPools = 0;
CRuMemoryAllocator::RuPoolDescriptor *CRuMemoryAllocator::s_pools = NULL;

// ************************************************************************************************************************************************************

void CRuMemoryAllocator::Initialize()
{
	if(s_pools == NULL)
	{
		// Initialize global allocator critical section
		InitializeCriticalSection(&s_allocatorCS);

		INT32 poolAllocSizes[] = { 131072, 65536, 32768, 16384, 8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, -1 };

		INT32 i = 0;
		while(poolAllocSizes[i] > 0)
		{
			RuPoolDescriptor *poolDesc = static_cast<RuPoolDescriptor *>(malloc(sizeof(RuPoolDescriptor)));

			poolDesc->m_allocationSize = poolAllocSizes[i];

			poolDesc->m_allocationCount = 0;
			poolDesc->m_freeCount = 0;

			poolDesc->m_allocatedBlocks = NULL;
			poolDesc->m_freeBlocks = NULL;

			poolDesc->m_allocatedPages = NULL;

			poolDesc->m_prev = NULL;
			poolDesc->m_next = s_pools;

			if(s_pools)
			{
				s_pools->m_prev = poolDesc;
			}

			s_pools = poolDesc;

			++i;
		}
	}
}

void *CRuMemoryAllocator::Allocate(size_t bytes)
{
	RuAllocationHeader *allocHeader = NULL;

	RuPoolDescriptor *curPool = s_pools;

	while(curPool)
	{
		if(curPool->m_allocationSize >= bytes)
		{
			allocHeader = GetFreeBlockFromPool(curPool);
			++curPool->m_allocationCount;
			break;
		}

		// Advance to next pool
		curPool = curPool->m_next;
	}

	if(allocHeader == NULL)
	{
		allocHeader = reinterpret_cast<RuAllocationHeader *>(malloc(sizeof(RuAllocationHeader) + bytes));

		allocHeader->m_signature = RUMEMORYALLOCATOR_SIGNATURE;
		allocHeader->m_pool = NULL;
		allocHeader->m_prev = NULL;
		allocHeader->m_next = NULL;
	}

	void *memBlock = reinterpret_cast<void *>(reinterpret_cast<BYTE *>(allocHeader) + sizeof(RuAllocationHeader));

	return memBlock;
}

void CRuMemoryAllocator::Deallocate(void *ptr)
{
	RuAllocationHeader *allocHeader = reinterpret_cast<RuAllocationHeader *>(reinterpret_cast<BYTE *>(ptr) - sizeof(RuAllocationHeader));
	assert(allocHeader->m_signature == RUMEMORYALLOCATOR_SIGNATURE);

	if(allocHeader->m_pool)
	{
		// Detach deallocated block from allocation list
		if(allocHeader->m_next)
		{
			allocHeader->m_next->m_prev = allocHeader->m_prev;
		}

		if(allocHeader->m_prev)
		{
			allocHeader->m_prev->m_next = allocHeader->m_next;
		}

		if(allocHeader == allocHeader->m_pool->m_allocatedBlocks)
		{
			allocHeader->m_pool->m_allocatedBlocks = allocHeader->m_next;
		}

		if(	(allocHeader->m_pool->m_freeCount * allocHeader->m_pool->m_allocationSize) > 1048576 ||
			allocHeader->m_pool->m_freeCount > 1024)
		{
			--allocHeader->m_pool->m_allocationCount;
			free(allocHeader);
		}
		else
		{
			// Insert deallocated block onto free list
			allocHeader->m_prev = NULL;
			allocHeader->m_next = allocHeader->m_pool->m_freeBlocks;

			if(allocHeader->m_next)
			{
				allocHeader->m_next->m_prev = allocHeader;
			}

			allocHeader->m_pool->m_freeBlocks = allocHeader;

			--allocHeader->m_pool->m_allocationCount;
			++allocHeader->m_pool->m_freeCount;
		}
	}
	else
	{
		free(allocHeader);
	}
}

void CRuMemoryAllocator::AcquireLock()
{
	EnterCriticalSection(&s_allocatorCS);
}

void CRuMemoryAllocator::ReleaseLock()
{
	LeaveCriticalSection(&s_allocatorCS);
}

CRuMemoryAllocator::RuAllocationHeader *CRuMemoryAllocator::GetFreeBlockFromPool(CRuMemoryAllocator::RuPoolDescriptor *pool)
{
	RuAllocationHeader *allocHeader = NULL;

	if(pool->m_freeBlocks == NULL)
	{
		allocHeader = reinterpret_cast<RuAllocationHeader *>(malloc(sizeof(RuAllocationHeader) + pool->m_allocationSize));

		allocHeader->m_signature = RUMEMORYALLOCATOR_SIGNATURE;
		allocHeader->m_pool = pool;
	}
	else
	{
		// Get first free block
		allocHeader = pool->m_freeBlocks;

		// Detach block from free block list
		pool->m_freeBlocks = pool->m_freeBlocks->m_next;

		if(pool->m_freeBlocks)
		{
			pool->m_freeBlocks->m_prev = NULL;
		}

		// Update free count
		--pool->m_freeCount;
	}

	// 
	allocHeader->m_prev = NULL;
	allocHeader->m_next = pool->m_allocatedBlocks;

	if(allocHeader->m_next)
	{
		allocHeader->m_next->m_prev = allocHeader;
	}

	pool->m_allocatedBlocks = allocHeader;

	return allocHeader;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
