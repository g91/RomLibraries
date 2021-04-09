/*!
	@file RuEngine_EngineResourceArray.h

	Copyright (c) 2004-2009 All rights reserved
*/

#pragma once

#include "../../Engine/Base/RuEngine_Engine.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Engine resource array.

	@author John Tang
	@version 2008.11.08
*/
class CRuEngine_EngineResourceArray
{
protected:
	CRuGUID									m_guid;							//!< Run-time GUID for the resource array

	IRuEngine*								m_engine;						//!< Pointer to engine

	CRuArrayList<IRuEngineResource *>		m_resources;					//!< Array of managed engine resources

public:
											CRuEngine_EngineResourceArray();
	virtual									~CRuEngine_EngineResourceArray();

	BOOL									Initialize(IRuEngine *engine);

	INT32									Add(IRuEngineResource *resource);
	INT32									Remove(IRuEngineResource *resource);
	INT32									Count();
	INT32									IndexOf(IRuEngineResource *resource);

	inline IRuEngineResource*				operator[](INT32 index);

protected:
	BOOL									HandleEvent_BeginScene(RuEventArgs *eventArgs);
	BOOL									HandleEvent_DeviceReset(RuEventArgs *eventArgs);
	BOOL									HandleEvent_DeviceRestore(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Shutdown(RuEventArgs *eventArgs);
	BOOL									HandleEvent_ResourceDispose(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

IRuEngineResource *CRuEngine_EngineResourceArray::operator[](INT32 index)
{
	return m_resources[index];
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
