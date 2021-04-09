#include <assert.h>

#include "../../Core/Memory/RuMemoryManager.h"

#ifdef new
	#undef new
#endif

#pragma managed(push, off)

volatile LONG						g_ruMallocCount = 0;

static CRITICAL_SECTION				g_ruMalloc_CS;
static CRuMalloc_Windows*			g_ruMalloc = NULL;

#ifndef _DEBUG
	#ifndef RU_USEDEFAULTALLOCATOR

		static void RuCreateMalloc()
		{
			InitializeCriticalSection(&g_ruMalloc_CS);

			// Allocate space for the allocator
			void *buf = malloc(sizeof(CRuMalloc_Windows));

			// Perform a placement new
			g_ruMalloc = new (buf) CRuMalloc_Windows();

			// Initialize the allocator
			g_ruMalloc->Initialize();
		}

		void* operator new(size_t Size)
		{
			InterlockedIncrement(&g_ruMallocCount);

			if(!g_ruMalloc)
			{
				RuCreateMalloc();
			}

			EnterCriticalSection(&g_ruMalloc_CS);

			void *memPtr = g_ruMalloc->Allocate((DWORD)Size);

			LeaveCriticalSection(&g_ruMalloc_CS);

			return memPtr;
		}

		void operator delete(void *Ptr)
		{
			EnterCriticalSection(&g_ruMalloc_CS);

			g_ruMalloc->Deallocate(Ptr);

			LeaveCriticalSection(&g_ruMalloc_CS);
		}

		void* operator new[](size_t Size)
		{
			InterlockedIncrement(&g_ruMallocCount);

			if(!g_ruMalloc)
			{
				RuCreateMalloc();
			}

			EnterCriticalSection(&g_ruMalloc_CS);

			void *memPtr = g_ruMalloc->Allocate((DWORD)Size);

			LeaveCriticalSection(&g_ruMalloc_CS);

			return memPtr;
		}

		void operator delete[](void *Ptr)
		{
			EnterCriticalSection(&g_ruMalloc_CS);

			g_ruMalloc->Deallocate(Ptr);

			LeaveCriticalSection(&g_ruMalloc_CS);
		}

	#endif
#endif

#ifdef RUINCLUDE_MEMORYMANAGER

	void *operator new(size_t size)
	{
		return operator new(size, CRuMemoryHeapFactory::GetDefaultHeap());
	}

	void *operator new(size_t size, CRuMemoryHeap *heap)
	{
		assert(heap != NULL);

		void *ret = heap->Allocate(size);

		CRuMemoryHeap::ResetAllocationOwner();

		return ret;
	}

	void operator delete(void *mem)
	{
		if(mem != NULL)	
			CRuMemoryHeap::Deallocate(mem);
	}

	void *operator new[](size_t size)
	{
		return operator new[](size, CRuMemoryHeapFactory::GetDefaultHeap());
	}

	void *operator new[](size_t size, CRuMemoryHeap *heap)
	{
		assert(heap != NULL);

		void *ret = heap->Allocate(size);

		CRuMemoryHeap::ResetAllocationOwner();

		return ret;
	}

	void operator delete[](void *mem)
	{
		if(mem != NULL)	
			CRuMemoryHeap::Deallocate(mem);
	}

#else

#endif

#pragma managed(pop)
