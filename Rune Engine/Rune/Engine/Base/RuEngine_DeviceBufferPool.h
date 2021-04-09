/*!
	@file RuEngine_DeviceBufferPool.h

	Copyright (c) 2004-2009 All rights reserved
*/

#pragma once

// Rune Engine Base
#include "../../Engine/Base/RuEngine_Engine.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Device buffer pool

	@author John Tang
	@version 2008.11.07
*/
class CRuEngine_DeviceBufferPool
{
protected:
	// Definition: IRuDeviceBuffer *CreateBufferCallback(INT32 size);
	typedef boost::function<IRuDeviceBuffer * (INT32 size)> CreateBufferCallback;

	struct BufferGCTuple
	{
		IRuDeviceBuffer*					m_buffer;
		INT32								m_gcAge;
	};

	CRuGUID									m_guid;

	CRuArrayList<IRuDeviceBuffer *>			m_allocatedBuffers;

	INT32									m_numSubPools;
	INT32*									m_subPoolSizes;
	CRuQueue<BufferGCTuple>*				m_subPools;

	IRuEngine*								m_engine;
	CreateBufferCallback					m_createBufferFn;

public:
											CRuEngine_DeviceBufferPool();
	virtual									~CRuEngine_DeviceBufferPool();

	BOOL									Initialize(INT32 *subPoolSizes, IRuEngine *engine, CreateBufferCallback createBufferFn);
	IRuDeviceBuffer*						CreateBuffer(INT32 size);
	BOOL									Invalidate();
	BOOL									GarbageCollect();

	INT32									GetBufferPoolFootprint();

protected:
	BOOL									HandleEvent_BeginScene(RuEventArgs *eventArgs);
	BOOL									HandleEvent_DeviceReset(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Shutdown(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
