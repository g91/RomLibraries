/*!
	@file RuEngine_DeviceBufferPool.cpp

	Copyright (c) 2004-2009 All rights reserved
*/

#include "../../Engine/Base/RuEngine_DeviceBufferPool.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuEngine_DeviceBufferPool::CRuEngine_DeviceBufferPool()
:	m_numSubPools(0),
	m_subPoolSizes(NULL),
	m_subPools(NULL),
	m_engine(NULL)
{
}

CRuEngine_DeviceBufferPool::~CRuEngine_DeviceBufferPool()
{
	// Manually handle shutdown
	HandleEvent_Shutdown(NULL);
}

BOOL CRuEngine_DeviceBufferPool::Initialize(INT32 *subPoolSizes, IRuEngine *engine, CreateBufferCallback createBufferFn)
{
	// Determine the number of sub-pools
	for(m_numSubPools = 0; subPoolSizes[m_numSubPools] > 0; ++m_numSubPools);

	// Copy sub-pool sizes
	m_subPoolSizes = ruNEW INT32 [m_numSubPools];
	RuMemcpy(m_subPoolSizes, subPoolSizes, m_numSubPools * sizeof(INT32));

	// Create stacks
	m_subPools = ruNEW CRuQueue<BufferGCTuple> [m_numSubPools];

	// Store pointer to engine
	m_engine = engine;

	// Store pointer to create buffer callback
	m_createBufferFn = createBufferFn;

	// Install event handlers
	m_engine->Event_BeginScene().RegisterHandler(m_guid, boost::bind(&CRuEngine_DeviceBufferPool::HandleEvent_BeginScene, this, _1));
	m_engine->Event_DeviceReset().RegisterHandler(m_guid, boost::bind(&CRuEngine_DeviceBufferPool::HandleEvent_DeviceReset, this, _1));
	m_engine->Event_Shutdown().RegisterHandler(m_guid, boost::bind(&CRuEngine_DeviceBufferPool::HandleEvent_Shutdown, this, _1));

	return TRUE;
}

IRuDeviceBuffer *CRuEngine_DeviceBufferPool::CreateBuffer(INT32 size)
{
	IRuDeviceBuffer *buffer = NULL;

	// Search in pools for compatible buffer
	for(INT32 i = 0; i < m_numSubPools; ++i)
	{
		if(size <= m_subPoolSizes[i])
		{
			if(m_subPools[i].IsEmpty() == FALSE)
			{
				buffer = m_subPools[i].DequeueTail().m_buffer;
				break;
			}

			// Adjust size to match the pool size
			size = m_subPoolSizes[i];
			break;
		}
	}

	// No compatible pool was found, create new buffer as-is
	if(buffer == NULL)
	{
		buffer = m_createBufferFn(size);
	}

	if(buffer)
	{
		// Store local reference to the buffer
		buffer->AddRef();
		m_allocatedBuffers.Add(buffer);
	}

	return buffer;
}

BOOL CRuEngine_DeviceBufferPool::Invalidate()
{
	// Iterate through all allocated buffers and release them
	for(INT32 i = 0; i < m_allocatedBuffers.Count(); ++i)
	{
		IRuDeviceBuffer *buffer = m_allocatedBuffers[i];

		// Invalidate the buffer
		buffer->Invalidate();

		// Release the buffer
		ruSAFE_RELEASE(buffer);
	}

	m_allocatedBuffers.Clear();

	if(m_subPools)
	{
		// Release all buffers in sub pools
		for(INT32 i = 0; i < m_numSubPools; ++i)
		{
			while(m_subPools[i].IsEmpty() == FALSE)
			{
				// Dequeue buffer off pool queue
				IRuDeviceBuffer *buffer = m_subPools[i].Dequeue().m_buffer;

				// Invalidate the buffer
				buffer->Invalidate();

				// Release the buffer
				ruSAFE_RELEASE(buffer);
			}
		}
	}

	return TRUE;
}

BOOL CRuEngine_DeviceBufferPool::GarbageCollect()
{
	// Iterate through all allocated buffers
	for(INT32 i = 0; i < m_allocatedBuffers.Count(); ++i)
	{
		// If the allocated buffer has a ref count of 1, it is no longer being used
		if(m_allocatedBuffers[i]->GetRefCount() == 1)
		{
			// Try to find a pool to push the buffer into
			INT32 j = 0;
			INT32 size = m_allocatedBuffers[i]->GetSize();
			for( ; j < m_numSubPools; ++j)
			{
				if(size == m_subPoolSizes[j])
				{
					// Push buffer into pool
					BufferGCTuple gcTuple = { m_allocatedBuffers[i], 0 };
					m_subPools[j].Queue(gcTuple);
					break;
				}
			}

			// No fitting pool was found, simply release the buffer
			if(j >= m_numSubPools)
			{
				ruSAFE_RELEASE(m_allocatedBuffers[i]);
			}

			// Remove from allocated buffers list
			m_allocatedBuffers.RemoveAt(i);
			--i;
		}
	}

	// Iterate through all buffer pools
	for(INT32 i = 0; i < m_numSubPools; ++i)
	{
		CRuQueue<BufferGCTuple> &curPool = m_subPools[i];

		for(INT32 j = 0, numBuffers = curPool.Count(); j < numBuffers; ++j)
		{
			// Dequeue buffer off pool queue
			BufferGCTuple gcTuple = curPool.Dequeue();

			// Increment GC age
			++gcTuple.m_gcAge;

			if(gcTuple.m_gcAge > 30)
			{
				// Invalidate the buffer
				gcTuple.m_buffer->Invalidate();

				// Release the buffer
				ruSAFE_RELEASE(gcTuple.m_buffer);
			}
			else
			{
				// Queue buffer tuple back onto pool queue
				curPool.Queue(gcTuple);
			}
		}
	}

	return TRUE;
}

INT32 CRuEngine_DeviceBufferPool::GetBufferPoolFootprint()
{
	INT32 footprint = 0;

	// Iterate through all allocated buffers
	for(INT32 i = 0; i < m_allocatedBuffers.Count(); ++i)
	{
		footprint += m_allocatedBuffers[i]->GetSize();
	}

	// Iterate through all pools
	if(m_subPools)
	{
		for(INT32 i = 0; i < m_numSubPools; ++i)
		{
			if(m_subPools[i].IsEmpty() == FALSE)
			{
				footprint += m_subPools[i].Peek().m_buffer->GetSize() * m_subPools[i].Count();
			}
		}
	}

	return footprint;
}

BOOL CRuEngine_DeviceBufferPool::HandleEvent_BeginScene(RuEventArgs *eventArgs)
{
	// Perform automatic garbage collection on managed buffers
	return this->GarbageCollect();
}

BOOL CRuEngine_DeviceBufferPool::HandleEvent_DeviceReset(RuEventArgs *eventArgs)
{
	// Perform automatic invalidation on managed buffers
	return this->Invalidate();
}

BOOL CRuEngine_DeviceBufferPool::HandleEvent_Shutdown(RuEventArgs *eventArgs)
{
	// Invalidate all managed buffers
	this->Invalidate();

	// Delete dynamic arrays
	ruSAFE_DELETE_ARRAY(m_subPoolSizes);
	ruSAFE_DELETE_ARRAY(m_subPools);

	// Unregister all event handlers
	if(m_engine)
	{
		m_engine->Event_BeginScene().UnregisterHandler(m_guid);
		m_engine->Event_DeviceReset().UnregisterHandler(m_guid);
		m_engine->Event_Shutdown().UnregisterHandler(m_guid);
		m_engine = NULL;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
