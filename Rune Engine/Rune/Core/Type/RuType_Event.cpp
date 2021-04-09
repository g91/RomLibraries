#include "../../Core/Type/RuType_Event.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuEvent::CRuEvent()
:	m_eventHandlers(NULL)
{
}

CRuEvent::~CRuEvent()
{
	EventHandlerDesc *curHandlerDesc = m_eventHandlers;
	while(curHandlerDesc)
	{
		// Store pointer to next handler
		EventHandlerDesc *nextHandlerDesc = curHandlerDesc->m_next;

		// Delete the handler
		delete curHandlerDesc;

		// Advance to next handler
		curHandlerDesc = nextHandlerDesc;
	}
}

BOOL CRuEvent::RegisterHandler(const CRuGUID &guid, RuEventHandler eventHandler)
{
	return RegisterHandler(guid, 0, eventHandler, 0);
}

BOOL CRuEvent::RegisterHandler(const CRuGUID &guid, RuEventHandler eventHandler, PTRVALUE userData)
{
	return RegisterHandler(guid, 0, eventHandler, userData);
}

BOOL CRuEvent::RegisterHandler(const CRuGUID &guid, INT32 priority, RuEventHandler eventHandler, PTRVALUE userData)
{
	// Determine if the handler's registration GUID already exists
	EventHandlerDesc *curHandlerDesc = m_eventHandlers;
	while(curHandlerDesc)
	{
		if(curHandlerDesc->m_guid == guid)
		{
			// Update handler (note that priority update is not allowed)
			curHandlerDesc->m_handler = eventHandler;
			curHandlerDesc->m_userData = userData;

			return TRUE;
		}

		// Advance to next handler
		curHandlerDesc = curHandlerDesc->m_next;
	}

	// Create new handler descriptor
	EventHandlerDesc *handlerDesc = ruNEW EventHandlerDesc();
	handlerDesc->m_guid = guid;
	handlerDesc->m_priority = priority;
	handlerDesc->m_handler = eventHandler;
	handlerDesc->m_userData = userData;

	// Find proper location to insert handler
	EventHandlerDesc *insertBeforeHandlerDesc = m_eventHandlers;
	EventHandlerDesc *lastHandlerDesc = insertBeforeHandlerDesc;
	while(insertBeforeHandlerDesc)
	{
		// If the current handler has a lower priority, this is where we insert
		if(insertBeforeHandlerDesc->m_priority < priority)
		{
			break;
		}

		// Advance to next handler
		lastHandlerDesc = insertBeforeHandlerDesc;
		insertBeforeHandlerDesc = insertBeforeHandlerDesc->m_next;
	}

	if(insertBeforeHandlerDesc)
	{
		if(insertBeforeHandlerDesc->m_prev)
		{
			insertBeforeHandlerDesc->m_prev->m_next = handlerDesc;

			handlerDesc->m_prev = insertBeforeHandlerDesc->m_prev;
			handlerDesc->m_next = insertBeforeHandlerDesc;

			insertBeforeHandlerDesc->m_prev = handlerDesc;
		}
		else
		{
			m_eventHandlers = handlerDesc;

			handlerDesc->m_prev = NULL;
			handlerDesc->m_next = insertBeforeHandlerDesc;

			insertBeforeHandlerDesc->m_prev = handlerDesc;
		}
	}
	else
	{
		if(lastHandlerDesc)
		{
			// Insert at the end of the handler list
			lastHandlerDesc->m_next = handlerDesc;

			handlerDesc->m_prev = lastHandlerDesc;
			handlerDesc->m_next = NULL;
		}
		else
		{
			// No handlers exist, this is the head of the handler list
			m_eventHandlers = handlerDesc;

			handlerDesc->m_prev = NULL;
			handlerDesc->m_next = NULL;
		}
	}

	return TRUE;
}

BOOL CRuEvent::UnregisterHandler(const CRuGUID &guid)
{
	// Find handler with specified GUID
	EventHandlerDesc *curHandlerDesc = m_eventHandlers;
	while(curHandlerDesc)
	{
		if(curHandlerDesc->m_guid == guid)
		{
			if(curHandlerDesc->m_prev)
			{
				curHandlerDesc->m_prev->m_next = curHandlerDesc->m_next;

				if(curHandlerDesc->m_next)
				{
					curHandlerDesc->m_next->m_prev = curHandlerDesc->m_prev;
				}
			}
			else
			{
				m_eventHandlers = curHandlerDesc->m_next;

				if(curHandlerDesc->m_next)
				{
					curHandlerDesc->m_next->m_prev = NULL;
				}
			}

			// Delete the handler descriptor
			delete curHandlerDesc;

			return TRUE;
		}

		// Advance to next handler
		curHandlerDesc = curHandlerDesc->m_next;
	}

	return FALSE;
}

BOOL CRuEvent::UnregisterAllHandlers()
{
	EventHandlerDesc *curHandlerDesc = m_eventHandlers;
	while(curHandlerDesc)
	{
		EventHandlerDesc *nextHandler = curHandlerDesc->m_next;
		delete curHandlerDesc;
		curHandlerDesc = nextHandler;
	}

	// Reset event handlers pointer
	m_eventHandlers = NULL;

	return TRUE;
}

BOOL CRuEvent::Trigger(RuEventArgs *eventArgs)
{
	EventHandlerDesc *curHandlerDesc = m_eventHandlers;
	while(curHandlerDesc)
	{
		// Store pointer to next handler
		EventHandlerDesc *nextHandlerDesc = curHandlerDesc->m_next;

		// Store user data into event args
		eventArgs->m_userData = curHandlerDesc->m_userData;

		// Invoke handler
		BOOL continueInvocation = curHandlerDesc->m_handler(eventArgs);

		// Abort invocation if specified to do so
		if(!continueInvocation)
			break;

		// Advance to next handler
		curHandlerDesc = nextHandlerDesc;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
