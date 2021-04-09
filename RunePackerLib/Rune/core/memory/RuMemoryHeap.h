#ifndef _RUMEMORYHEAP_H_
#define _RUMEMORYHEAP_H_

#include "../../Core/Type/RuType_Base.h"

#pragma unmanaged

struct CRuMemoryAllocHeader;

class CRuMemoryHeap
{
private:
    enum
	{
		NAMELENGTH = 128
	};

    BOOL						m_active;
    char						m_name[NAMELENGTH];
    size_t						m_bytesAllocated;
    size_t						m_bytesPeak;
    INT32						m_numInstances;
    CRuMemoryAllocHeader*		m_headAlloc;

    CRuMemoryHeap*				m_parent;
    CRuMemoryHeap*				m_firstChild;
    CRuMemoryHeap*				m_nextSibling;
    CRuMemoryHeap*				m_prevSibling;

	CRITICAL_SECTION			m_memoryHeapCS;

    static volatile LONG		s_nextAllocNum;

	static const char*			s_filename;
	static const char*			s_function;
	static UINT32				s_line;

public:
    void						Initialize();

    void*						Allocate(size_t bytes);
    static void					Deallocate(void * pMem);

    void						Activate(const char * name);
    void						Deactivate();

    void						AttachTo(CRuMemoryHeap *parent);

    BOOL						IsActive() const { return m_active; }
    const char*					GetName() const;

    void						PrintTreeInfo(INT32 indentLevel = 0) const;
    void						PrintInfo(INT32 indentLevel = 0) const;

    INT32						ReportMemoryLeaks(INT32 bookmark1, INT32 bookmark2);

    static INT32				GetMemoryBookmark();

	// Leak tracking
	static void					SetAllocationOwner(const char *filename, const char *function, const UINT32 line);

private:
    void						Deallocate(CRuMemoryAllocHeader *header);
    void						GetTreeStats(size_t &totalBytes, size_t &totalPeak, INT32 &totalInstances) const;
};

#pragma managed

#endif
