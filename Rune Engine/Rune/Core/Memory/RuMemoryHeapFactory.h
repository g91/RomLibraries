#ifndef _RUMEMORYHEAPFACTORY_H_
#define _RUMEMORYHEAPFACTORY_H_

#include "../../Core/Memory/RuMemoryHeap.h"
#include "../../Core/Memory/RuMemoryAllocator.h"

#pragma managed(push, off)

class CRuMemoryHeapFactory
{
private:
	enum
	{
		MAXHEAPS = 1024
	};

	static CRuMemoryHeap*		s_rootHeap;   
	static CRuMemoryHeap*		s_defaultHeap;   
	static CRuMemoryHeap		s_heaps[MAXHEAPS];

public:
    static CRuMemoryHeap*		CreateHeap(const char *name);
    static CRuMemoryHeap*		CreateHeap(const char *name, const char *parent);
	static void					DestroyHeap(CRuMemoryHeap* pHeap);

	static CRuMemoryHeap*		GetDefaultHeap();
    static void					PrintInfo();

    static INT32				GetMemoryBookmark();
    static void					ReportMemoryLeaks(INT32 bookmark);
    static void					ReportMemoryLeaks(INT32 bookmark1, INT32 bookmark2);

protected:
	static void					AcquireLock();
	static void					ReleaseLock();
    
private:
	static CRuMemoryHeap*		GetRootHeap();
    static CRuMemoryHeap*		FindHeap(const char *name);
    static CRuMemoryHeap*		CreateNewHeap(const char *name);
    static void					PrintHeapTree(CRuMemoryHeap *pHeap);

public:
    static void					Initialize();

	friend						CRuMemoryHeap;
};

#pragma managed(pop)

#endif
