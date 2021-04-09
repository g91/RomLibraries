#include <cstddef>
#include <stdio.h>
#include <string.h>

#include "../../Core/Memory/RuMemoryHeapFactory.h"

#pragma managed(push, off)

CRuMemoryHeap*			CRuMemoryHeapFactory::s_rootHeap = NULL;
CRuMemoryHeap*			CRuMemoryHeapFactory::s_defaultHeap = NULL;
CRuMemoryHeap			CRuMemoryHeapFactory::s_heaps[];

void CRuMemoryHeapFactory::Initialize()
{   
	// Initialize allocator
	CRuMemoryAllocator::Initialize();

	// Initialize heaps
    for(INT32 i = 0; i < MAXHEAPS; ++i)
        s_heaps[i].Initialize();

	// Setup root and default heaps
    s_rootHeap = CreateNewHeap("Root");
    s_defaultHeap = CreateHeap("Default");
}

CRuMemoryHeap *CRuMemoryHeapFactory::GetRootHeap()
{
    if(s_rootHeap == NULL)
        Initialize();

    return s_rootHeap;
}

CRuMemoryHeap *CRuMemoryHeapFactory::GetDefaultHeap()
{
    if(s_defaultHeap == NULL)
        Initialize();

    return s_defaultHeap;
}

CRuMemoryHeap *CRuMemoryHeapFactory::CreateHeap(const char *name, const char *parent)
{
    if(s_rootHeap == NULL)
        Initialize();

	// Enter global heap factory critical section
	CRuMemoryAllocator::AcquireLock();

	// Try to find the parent heap
    CRuMemoryHeap *pParent = FindHeap(parent);
    if(pParent == NULL)
    {
		// If the parent heap does not exist, create it
        pParent = CreateNewHeap(parent);
        pParent->AttachTo(s_rootHeap);
    }

	// Find the specified heap, and create it if it does not exist
    CRuMemoryHeap *pHeap = FindHeap(name);
    if(pHeap == NULL)
        pHeap = CreateNewHeap(name);

	// Attach heap to the parent
    pHeap->AttachTo(pParent);

	// Exit global heap factory critical section
	CRuMemoryAllocator::ReleaseLock();

    return pHeap;
}

CRuMemoryHeap *CRuMemoryHeapFactory::CreateHeap(const char *name)
{
    return CreateHeap(name, "Root");
}

CRuMemoryHeap *CRuMemoryHeapFactory::FindHeap(const char *name)
{
	CRuMemoryHeap *retVal = NULL;

	// Enter global heap factory critical section
	CRuMemoryAllocator::AcquireLock();

    for(INT32 i = 0; i < MAXHEAPS; ++i)
    {
        CRuMemoryHeap *pHeap = &s_heaps[i];
        if(pHeap->IsActive() && !stricmp(name, pHeap->GetName()))
		{
            retVal = pHeap;
			break;
		}
    }

	// Exit global heap factory critical section
	CRuMemoryAllocator::ReleaseLock();

    return retVal;
}

CRuMemoryHeap *CRuMemoryHeapFactory::CreateNewHeap(const char *name)
{
	CRuMemoryHeap *retVal = NULL;

	// Enter global heap factory critical section
	CRuMemoryAllocator::AcquireLock();

    for(INT32 i = 0; i < MAXHEAPS; ++i)
    {
        CRuMemoryHeap *pHeap = &s_heaps[i];
        if(!pHeap->IsActive())
        {
            pHeap->Activate(name);
            retVal = pHeap;
			break;
        }
    }

	// Exit global heap factory critical section
	CRuMemoryAllocator::ReleaseLock();

    return retVal;
}

void CRuMemoryHeapFactory::PrintInfo()
{
	#ifdef RUMEMORYMANAGER_DEBUG

		// Enter global heap factory critical section
		CRuMemoryAllocator::AcquireLock();

		OutputDebugString("--------------------------------------------------------------------------------------------------------------------------------\n");
		OutputDebugString("MEMORY INFORMATION\n");
		OutputDebugString("                                                                             Local                              Total\n");
		OutputDebugString("Name                                                             Memory       Peak       Inst |     Memory       Peak       Inst\n");
		GetRootHeap()->PrintTreeInfo(0);
		OutputDebugString("--------------------------------------------------------------------------------------------------------------------------------\n");

		// Exit global heap factory critical section
		CRuMemoryAllocator::ReleaseLock();

	#endif
}

INT32 CRuMemoryHeapFactory::GetMemoryBookmark() 
{
    return CRuMemoryHeap::GetMemoryBookmark();
}

void CRuMemoryHeapFactory::ReportMemoryLeaks(INT32 bookmark) 
{
    ReportMemoryLeaks(bookmark, GetMemoryBookmark());
}

void CRuMemoryHeapFactory::ReportMemoryLeaks(INT32 bookmark1, INT32 bookmark2) 
{
	#ifdef RUMEMORYMANAGER_DEBUG

		// Enter global heap factory critical section
		CRuMemoryAllocator::AcquireLock();

		INT32 numLeaks = 0;
		for(INT32 i = 0; i < MAXHEAPS; ++i)
		{
			if(s_heaps[i].IsActive())
				numLeaks += s_heaps[i].ReportMemoryLeaks(bookmark1, bookmark2);
		}

		char outputString[2048];
		if(numLeaks > 0)
			sprintf(outputString, "%d memory leaks found\n", numLeaks);
		else
			sprintf(outputString, "No memory leaks detected.\n");

		OutputDebugString(outputString);

		// Exit global heap factory critical section
		CRuMemoryAllocator::ReleaseLock();

	#endif
}

void CRuMemoryHeapFactory::AcquireLock()
{
	CRuMemoryAllocator::AcquireLock();
}

void CRuMemoryHeapFactory::ReleaseLock()
{
	CRuMemoryAllocator::ReleaseLock();
}

#pragma managed(pop)
