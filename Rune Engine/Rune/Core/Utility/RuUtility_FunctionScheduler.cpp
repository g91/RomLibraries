/*!
	@file RuUtility_FunctionScheduler.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../../Core/Utility/RuUtility_FunctionScheduler.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuUtility_FunctionScheduler, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuUtility_FunctionScheduler, "CRuUtility_FunctionScheduler", "IRuObject")
ruCLASSGUID_DEFINE(CRuUtility_FunctionScheduler, 0xF3613075, 0x39E84835, 0x8D65FF34, 0xFAED81F1)

// ************************************************************************************************************************************************************

CRuUtility_FunctionScheduler::CRuUtility_FunctionScheduler()
:	m_functionDescriptors(NULL)
{
	m_lastUpdateTick = GetTickCount();
}

CRuUtility_FunctionScheduler::~CRuUtility_FunctionScheduler()
{
	// Delete all function descriptors
	while(m_functionDescriptors)
	{
		DeleteFunctionDescriptor(m_functionDescriptors);
	}

	// Discard the unused stack
	while(m_unusedFunctionDescriptors.IsEmpty() == FALSE)
	{
		FunctionDesc *funcDesc = m_unusedFunctionDescriptors.Pop();
		delete funcDesc;
	}
}

BOOL CRuUtility_FunctionScheduler::ScheduleFunction(const CRuGUID &guid, INT32 priority, INT32 repetitionCount, DWORD invokePeriod, CRuUtility_FunctionScheduler::RuScheduledFunction func, PTRVALUE userData)
{
	// Determine if the registration GUID already exists
	FunctionDesc *curFuncDesc = m_functionDescriptors;
	while(curFuncDesc)
	{
		if(curFuncDesc->m_guid == guid)
		{
			// Update function (note that priority update is not allowed)
			curFuncDesc->m_function = func;
			curFuncDesc->m_userData = userData;

			return TRUE;
		}

		// Advance to next handler
		curFuncDesc = curFuncDesc->m_next;
	}

	// Create new handler descriptor
	FunctionDesc *funcDesc = CreateFunctionDescriptor(priority);
	funcDesc->m_guid = guid;
	funcDesc->m_priority = priority;
	funcDesc->m_repetitionCount = repetitionCount;
	funcDesc->m_invokePeriod = invokePeriod;
	funcDesc->m_timeToNextInvoke = invokePeriod;
	funcDesc->m_function = func;;
	funcDesc->m_userData = userData;

	return TRUE;
}

BOOL CRuUtility_FunctionScheduler::UnsheduleFunction(const CRuGUID &guid)
{
	// Find function with specified registration GUID
	FunctionDesc *curFuncDesc = m_functionDescriptors;
	while(curFuncDesc)
	{
		if(curFuncDesc->m_guid == guid)
		{
			// Delete the function and return
			DeleteFunctionDescriptor(curFuncDesc);
			return TRUE;
		}

		// Advance to next function
		curFuncDesc = curFuncDesc->m_next;
	}

	return FALSE;
}

void CRuUtility_FunctionScheduler::Update()
{
	// Update time
	DWORD currentTick = GetTickCount();
	DWORD elapsedTicks = currentTick - m_lastUpdateTick;
	m_lastUpdateTick = currentTick;

	FunctionDesc *curFuncDesc = m_functionDescriptors;
	while(curFuncDesc)
	{
		// Store pointer to next function
		FunctionDesc *nextFuncDesc = curFuncDesc->m_next;

		if(curFuncDesc->m_timeToNextInvoke <= elapsedTicks)
		{
			// Invoke handler
			curFuncDesc->m_function(curFuncDesc->m_userData);

			// Reset time to next invoke
			curFuncDesc->m_timeToNextInvoke = curFuncDesc->m_invokePeriod;

			if(curFuncDesc->m_repetitionCount == 1)
			{
				// If repetition count has reached one, remove the function
				DeleteFunctionDescriptor(curFuncDesc);
			}
			else if(curFuncDesc->m_repetitionCount > 1)
			{
				// If the repetition count is greater than one, deduct from the repetition count
				--curFuncDesc->m_repetitionCount;
			}
		}
		else
		{
			// Deduct elapsed ticks from the time to next invoke
			curFuncDesc->m_timeToNextInvoke -= elapsedTicks;
		}

		// Advance to next function
		curFuncDesc = nextFuncDesc;
	}
}

CRuUtility_FunctionScheduler::FunctionDesc *CRuUtility_FunctionScheduler::CreateFunctionDescriptor(INT32 priority)
{
	// Allocate function descriptor
	FunctionDesc *funcDesc = m_unusedFunctionDescriptors.IsEmpty() ? ruNEW FunctionDesc : m_unusedFunctionDescriptors.Pop();

	// Find proper location to insert function descriptor
	FunctionDesc *insertBeforeFunctionDesc = m_functionDescriptors;
	FunctionDesc *lastFunctionDesc = insertBeforeFunctionDesc;
	while(insertBeforeFunctionDesc)
	{
		// If the current function has a lower priority, this is where we insert
		if(insertBeforeFunctionDesc->m_priority < priority)
		{
			break;
		}

		// Advance to next function
		lastFunctionDesc = insertBeforeFunctionDesc;
		insertBeforeFunctionDesc = insertBeforeFunctionDesc->m_next;
	}

	if(insertBeforeFunctionDesc)
	{
		if(insertBeforeFunctionDesc->m_prev)
		{
			insertBeforeFunctionDesc->m_prev->m_next = funcDesc;

			funcDesc->m_prev = insertBeforeFunctionDesc->m_prev;
			funcDesc->m_next = insertBeforeFunctionDesc;

			insertBeforeFunctionDesc->m_prev = funcDesc;
		}
		else
		{
			m_functionDescriptors = funcDesc;

			funcDesc->m_prev = NULL;
			funcDesc->m_next = insertBeforeFunctionDesc;

			insertBeforeFunctionDesc->m_prev = funcDesc;
		}
	}
	else
	{
		if(lastFunctionDesc)
		{
			// Insert at the end of the function list
			lastFunctionDesc->m_next = funcDesc;

			funcDesc->m_prev = lastFunctionDesc;
			funcDesc->m_next = NULL;
		}
		else
		{
			// No functions exist, this is the head of the function list
			m_functionDescriptors = funcDesc;

			funcDesc->m_prev = NULL;
			funcDesc->m_next = NULL;
		}
	}

	return funcDesc;
}

void CRuUtility_FunctionScheduler::DeleteFunctionDescriptor(CRuUtility_FunctionScheduler::FunctionDesc *funcDesc)
{
	// Unlink the function descriptor
	if(funcDesc->m_prev)
	{
		funcDesc->m_prev->m_next = funcDesc->m_next;

		if(funcDesc->m_next)
		{
			funcDesc->m_next->m_prev = funcDesc->m_prev;
		}
	}
	else
	{
		m_functionDescriptors = funcDesc->m_next;

		if(funcDesc->m_next)
		{
			funcDesc->m_next->m_prev = NULL;
		}
	}

	// Push function descriptor onto unused stack
	m_unusedFunctionDescriptors.Push(funcDesc);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
