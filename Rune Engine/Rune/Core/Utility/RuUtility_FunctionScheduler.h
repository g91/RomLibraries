/*!
	@file RuUtility_FunctionScheduler.h

	Copyright (c) 2004-2008 All rights reserved
*/

#pragma once

// Base Includes
#include "../../Core/Type/RuType_Object.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	An utility class for scheduling function calls

	@version 2008.07.28
*/
class CRuUtility_FunctionScheduler : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	typedef boost::function<BOOL (PTRVALUE)> RuScheduledFunction;				//!< Parameters: BOOL RuScheduledFunction(PTRVALUE userData);

protected:
	struct FunctionDesc
	{
		CRuGUID								m_guid;							//!< Registration GUID
		INT32								m_priority;						//!< Handler priority
		INT32								m_repetitionCount;				//!< Number of repetitions remaining, 0 = no repetition limit
		DWORD								m_invokePeriod;					//!< Time (in milliseconds) between each invocation
		DWORD								m_timeToNextInvoke;				//!< Time (in milliseconds) until next invocation
		RuScheduledFunction					m_function;						//!< Scheduled function
		PTRVALUE							m_userData;						//!< User data

		FunctionDesc*						m_prev;
		FunctionDesc*						m_next;
	};

	FunctionDesc*							m_functionDescriptors;

	DWORD									m_lastUpdateTick;

	CRuStack<FunctionDesc *>				m_unusedFunctionDescriptors;

public:
											CRuUtility_FunctionScheduler();
	virtual									~CRuUtility_FunctionScheduler();

	BOOL									ScheduleFunction(const CRuGUID &guid, INT32 priority, INT32 repetitionCount, DWORD invokePeriod, RuScheduledFunction func, PTRVALUE userData);
	BOOL									UnsheduleFunction(const CRuGUID &guid);

	void									Update();

protected:
	FunctionDesc*							CreateFunctionDescriptor(INT32 priority);
	void									DeleteFunctionDescriptor(FunctionDesc *funcDesc);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
