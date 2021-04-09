#ifndef _RUMEMORYMANAGER_H_
#define _RUMEMORYMANAGER_H_

#include "../../Core/Memory/RuMemoryHeapFactory.h"
#include "../../Core/Memory/RuMalloc.h"

#pragma managed(push, off)

//#define RU_USEDEFAULTALLOCATOR

//#define RUINCLUDE_MEMORYMANAGER

extern volatile LONG				g_ruMallocCount;

#ifndef _DEBUG
	#ifndef RU_USEDEFAULTALLOCATOR
		void*			operator new( size_t Size );
		void			operator delete( void* Ptr );
		void*			operator new[]( size_t Size );
		void			operator delete[]( void* Ptr );
	#endif
#endif

#ifdef RUINCLUDE_MEMORYMANAGER

	void*			operator new(size_t size);
	void*			operator new(size_t size, CRuMemoryHeap *heap);
	void			operator delete(void *mem);
	void*			operator new[](size_t size);
	void*			operator new[](size_t size, CRuMemoryHeap *heap);
	void			operator delete[](void *mem);

	#define ruHEAP_DECLARE																						\
		public:																									\
			static void*			operator new(size_t size);													\
			static void				operator delete(void *p, size_t size);										\
			static void*			operator new[](size_t size);												\
			static void				operator delete[](void *p, size_t size);									\
																												\
		private:																								\
			static CRuMemoryHeap*	s_heap;

	#define ruHEAP_DEFINE_ROOTCLASS(className, heapName)														\
		CRuMemoryHeap *className::s_heap = CRuMemoryHeapFactory::CreateHeap(heapName);							\
																												\
		void *className::operator new(size_t size)																\
		{																										\
			if(s_heap == NULL)																					\
				return ::operator new(size);																	\
			else																								\
				return ::operator new(size, s_heap);															\
		}																										\
																												\
		void className::operator delete(void * p, size_t size)													\
		{																										\
			::operator delete(p);																				\
		}																										\
																												\
		void *className::operator new[](size_t size)															\
		{																										\
			if(s_heap == NULL)																					\
				return ::operator new[](size);																	\
			else																								\
				return ::operator new[](size, s_heap);															\
		}																										\
																												\
		void className::operator delete[](void * p, size_t size)												\
		{																										\
			::operator delete[](p);																				\
		}

	#define ruHEAP_DEFINE_SUBCLASS(className, heapName, parentName)												\
		CRuMemoryHeap *className::s_heap = CRuMemoryHeapFactory::CreateHeap(heapName, parentName);				\
																												\
		void *className::operator new(size_t size)																\
		{																										\
			if(s_heap == NULL)																					\
				return ::operator new(size);																	\
			else																								\
				return ::operator new(size, s_heap);															\
		}																										\
																												\
		void className::operator delete(void *p, size_t size)													\
		{																										\
			::operator delete(p);																				\
		}																										\
																												\
		void *className::operator new[](size_t size)															\
		{																										\
			if(s_heap == NULL)																					\
				return ::operator new[](size);																	\
			else																								\
				return ::operator new[](size, s_heap);															\
		}																										\
																												\
		void className::operator delete[](void *p, size_t size)													\
		{																										\
			::operator delete[](p);																				\
		}

	#define ruNEW (CRuMemoryHeap::SetAllocationOwner(__FILE__, __FUNCTION__, __LINE__), false) ? NULL : new

#else

	#define ruHEAP_DECLARE
	#define ruHEAP_DEFINE_ROOTCLASS(className, heapName)
	#define ruHEAP_DEFINE_SUBCLASS(className, heapName, parentName)

	#define ruNEW new

#endif

/*
	#define ruHEAP_ALIGN16_DECLARE																				\
		public:																									\
			static void*			operator new(size_t size);													\
			static void				operator delete(void *p, size_t size);										\

	#define ruHEAP_ALIGN16_DEFINE(className)																	\
																												\
		static void *operator new(size_t size)																	\
		{																										\
			if(size != sizeof(className))																		\
				return ::operator new(size);																	\
																												\
			return _aligned_malloc(size, 16);																	\
		}																										\
																												\
		static void operator delete(void *p, size_t size)														\
		{																										\
			if(p == 0)																							\
				return;																							\
																												\
			if(size != sizeof(className))																		\
			{																									\
				::operator delete(p);																			\
				return;																							\
			}																									\
																												\
			_aligned_free(p);																					\
		}																										
*/
#pragma managed(pop)

#endif
