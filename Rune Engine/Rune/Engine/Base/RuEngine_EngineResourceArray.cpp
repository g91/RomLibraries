/*!
	@file RuEngine_EngineResourceArray.cpp

	Copyright (c) 2004-2009 All rights reserved
*/

#include "../../Engine/Base/RuEngine_EngineResourceArray.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuEngine_EngineResourceArray::CRuEngine_EngineResourceArray()
:	m_engine(NULL),
	m_resources(16)
{
}

CRuEngine_EngineResourceArray::~CRuEngine_EngineResourceArray()
{
	HandleEvent_Shutdown(NULL);
}

BOOL CRuEngine_EngineResourceArray::Initialize(IRuEngine *engine)
{
	// Store pointer to engine
	m_engine = engine;

	// Install event handlers
	m_engine->Event_BeginScene().RegisterHandler(m_guid, boost::bind(&CRuEngine_EngineResourceArray::HandleEvent_BeginScene, this, _1));
	m_engine->Event_DeviceReset().RegisterHandler(m_guid, boost::bind(&CRuEngine_EngineResourceArray::HandleEvent_DeviceReset, this, _1));
	m_engine->Event_DeviceRestore().RegisterHandler(m_guid, boost::bind(&CRuEngine_EngineResourceArray::HandleEvent_DeviceRestore, this, _1));
	m_engine->Event_Shutdown().RegisterHandler(m_guid, boost::bind(&CRuEngine_EngineResourceArray::HandleEvent_Shutdown, this, _1));

	return TRUE;
}

INT32 CRuEngine_EngineResourceArray::Add(IRuEngineResource *resource)
{
	INT32 insertIdx = -1;

	if(m_resources.Contains(resource) == FALSE)
	{
		// Store insertion index
		insertIdx = m_resources.Count();

		// Install event handler on resource
		resource->Event_Dispose().RegisterHandler(m_guid, boost::bind(&CRuEngine_EngineResourceArray::HandleEvent_ResourceDispose, this, _1), reinterpret_cast<PTRVALUE>(resource));

		// Insert resource into resource array
		m_resources.Add(resource);
	}

	return insertIdx;
}

INT32 CRuEngine_EngineResourceArray::Remove(IRuEngineResource *resource)
{
	for(INT32 i = 0; i < m_resources.Count(); ++i)
	{
		if(m_resources[i] == resource)
		{
			// Unregister event handler from resource
			m_resources[i]->Event_Dispose().UnregisterHandler(m_guid);

			// Remove resource from resource array
			m_resources.RemoveAt(i);

			// Return removal index
			return i;
		}
	}

	return -1;
}

INT32 CRuEngine_EngineResourceArray::Count()
{
	return m_resources.Count();
}

INT32 CRuEngine_EngineResourceArray::IndexOf(IRuEngineResource *resource)
{
	return m_resources.IndexOf(resource);
}

BOOL CRuEngine_EngineResourceArray::HandleEvent_BeginScene(RuEventArgs *eventArgs)
{
	return TRUE;
}

BOOL CRuEngine_EngineResourceArray::HandleEvent_DeviceReset(RuEventArgs *eventArgs)
{
	// Invalidate all resources
	for(INT32 i = 0; i < m_resources.Count(); ++i)
	{
		m_resources[i]->Invalidate();
	}

	return TRUE;
}

BOOL CRuEngine_EngineResourceArray::HandleEvent_DeviceRestore(RuEventArgs *eventArgs)
{
	// Restore all resources
	for(INT32 i = 0; i < m_resources.Count(); ++i)
	{
		m_resources[i]->Restore();
	}

	return TRUE;
}

BOOL CRuEngine_EngineResourceArray::HandleEvent_Shutdown(RuEventArgs *eventArgs)
{
	// Disown all resources
	for(INT32 i = 0; i < m_resources.Count(); ++i)
	{
		// Invalidate the resource
		m_resources[i]->Invalidate();

		// Unregister event handler from resource
		m_resources[i]->Event_Dispose().UnregisterHandler(m_guid);
	}

	m_resources.Clear();

	// Unregister all event handlers
	if(m_engine)
	{
		m_engine->Event_BeginScene().UnregisterHandler(m_guid);
		m_engine->Event_DeviceReset().UnregisterHandler(m_guid);
		m_engine->Event_DeviceRestore().UnregisterHandler(m_guid);
		m_engine->Event_Shutdown().UnregisterHandler(m_guid);
		m_engine = NULL;
	}

	return TRUE;
}

BOOL CRuEngine_EngineResourceArray::HandleEvent_ResourceDispose(RuEventArgs *eventArgs)
{
	// Cast user data back into pointer to the disposed resource
	IRuEngineResource *disposedResource = reinterpret_cast<IRuEngineResource *>(eventArgs->m_userData);

	// Remove resource
	return this->Remove(disposedResource) >= 0;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
