#pragma once

// Base types
#include "../../Core/Type/RuType_Base.h"

// Base collections
#include "../../Core/Collection/RuCollections.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class RuEventArgs
{
public:
	PTRVALUE								m_userData;						//!< User data
};

// ************************************************************************************************************************************************************

typedef boost::function<BOOL (RuEventArgs *eventArgs)> RuEventHandler;		// Parameters: BOOL RuEventHandler(RuEventArgs *eventArgs);

// ************************************************************************************************************************************************************

class CRuEvent
{
protected:
	struct EventHandlerDesc
	{
		CRuGUID								m_guid;							//!< Registration GUID
		INT32								m_priority;						//!< Handler priority
		RuEventHandler						m_handler;						//!< Event handler
		PTRVALUE							m_userData;						//!< User data

		EventHandlerDesc*					m_prev;
		EventHandlerDesc*					m_next;
	};

	EventHandlerDesc*						m_eventHandlers;

public:
											CRuEvent();
											~CRuEvent();

	// Handler registration
	BOOL									RegisterHandler(const CRuGUID &guid, RuEventHandler eventHandler);
	BOOL									RegisterHandler(const CRuGUID &guid, RuEventHandler eventHandler, PTRVALUE userData);
	BOOL									RegisterHandler(const CRuGUID &guid, INT32 priority, RuEventHandler eventHandler, PTRVALUE userData);
	BOOL									UnregisterHandler(const CRuGUID &guid);
	BOOL									UnregisterAllHandlers();

	// Event trigger
	BOOL									Trigger(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
