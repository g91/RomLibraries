#ifndef _RUMEMORYALLOCATOR_H_
#define _RUMEMORYALLOCATOR_H_

#include "../../Core/Type/RuType_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

#define RUMEMORYALLOCATOR_SIGNATURE 0xDEADC0DE

// ************************************************************************************************************************************************************

class CRuMemoryAllocator
{
protected:
	struct RuAllocationHeader;
	struct RuPageAllocationHeader;
	struct RuPoolDescriptor;

	struct RuAllocationHeader
	{
		INT32							m_signature;						//!< Signature - 0xDEADC0DE

		RuPoolDescriptor*				m_pool;								//!< Memory pool descriptor, NULL = non-pooled direct allocation

		RuAllocationHeader*				m_prev;
		RuAllocationHeader*				m_next;
	};

	struct RuPageAllocationHeader
	{
		void*							m_memPtr;

		RuPageAllocationHeader*			m_prev;
		RuPageAllocationHeader*			m_next;
	};

	struct RuPoolDescriptor
	{
		size_t							m_allocationSize;

		INT32							m_allocationCount;
		INT32							m_freeCount;

		RuAllocationHeader*				m_allocatedBlocks;
		RuAllocationHeader*				m_freeBlocks;

		RuPageAllocationHeader*			m_allocatedPages;

		RuPoolDescriptor*				m_prev;
		RuPoolDescriptor*				m_next;
	};

	static CRITICAL_SECTION				s_allocatorCS;

	static INT32						s_numPools;
	static RuPoolDescriptor*			s_pools;

public:
	static void							Initialize();

	static void*						Allocate(size_t bytes);
	static void							Deallocate(void *ptr);

	static void							AcquireLock();
	static void							ReleaseLock();

protected:
	static RuAllocationHeader*			GetFreeBlockFromPool(RuPoolDescriptor *pool);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
