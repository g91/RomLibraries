#ifndef _RUMEMORYHEAPFACTORY_H_
#define _RUMEMORYHEAPFACTORY_H_

#include "../../Core/Memory/RuMemoryHeap.h"

#pragma unmanaged

class CRuMemoryHeapFactory
{
private:
	enum
	{
		MAXHEAPS = 512
	};

	static CRITICAL_SECTION		s_heapFactoryCS;
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

    
private:
	static CRuMemoryHeap*		GetRootHeap();
    static CRuMemoryHeap*		FindHeap(const char *name);
    static CRuMemoryHeap*		CreateNewHeap(const char *name);
    static void					PrintHeapTree(CRuMemoryHeap *pHeap);

    static void					Initialize();
};

#pragma managed

#endif
