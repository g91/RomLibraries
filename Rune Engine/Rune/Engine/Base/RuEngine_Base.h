/*!
	@project Rune
	@file RuEngineBase.h

	Copyright (c) 2004 All rights reserved

	@author John Tang
	@date 2004/07/08
*/

#pragma once

#include "../../Core/Type/RuType_Base.h"
#include "../../Core/Math/RuMathVector.h"
#include "../../Core/Memory/RuMemoryManager.h"
#include "../../Core/Thread/RuThread_Worker.h"
#include "../../Core/Utility/RuUtility_Profile.h"
#include "../../Core/Utility/RuUtility_FunctionScheduler.h"

#include "../../Engine/Base/RuEngine_Settings.h"

#include "../../Engine/Type/RuType_Core.h"
#include "../../Engine/Type/RuType_Flags.h"
#include "../../Engine/Type/RuType_FX.h"
#include "../../Engine/Type/RuType_Vertex.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

// Profiler macros
#define RUPROFILE_ENABLE

#ifdef RUPROFILE_ENABLE

	#define RUPROFILE_INITIALIZE

	#define RUPROFILE_SAMPLE_BEGIN(probeIndex, profileLevel)							\
		if(g_ruProfilingLevel >= profileLevel) g_ruProfiler->BeginSample(probeIndex);

	#define RUPROFILE_SAMPLE_END(probeIndex, profileLevel)								\
		if(g_ruProfilingLevel >= profileLevel) g_ruProfiler->EndSample(probeIndex);

	#define RUPROFILE_SAMPLE_ACCUMULATE(probeIndex, profileLevel, probeValue)			\
		if(g_ruProfilingLevel >= profileLevel) g_ruProfiler->AccumulateProbeValue(probeIndex, probeValue);
		
	#define RUPROFILE_SAMPLE_CLEAR()													\
		g_ruProfiler->ClearProbeValues();	

#else

	#define RUPROFILE_INITIALIZE
	#define RUPROFILE_SAMPLE_BEGIN(probeIndex, profileLevel)
	#define RUPROFILE_SAMPLE_END(probeIndex, profileLevel)
	#define RUPROFILE_SAMPLE_ACCUMULATE(probeIndex, profileLevel, probeValue)
	#define RUPROFILE_SAMPLE_CLEAR()

#endif

// ************************************************************************************************************************************************************

enum RuAsyncGetDataFlag
{
	ruASYNCGETDATAFLAG_DONOTFLUSH				= 1
};

// ************************************************************************************************************************************************************

enum RuGPUQueryType
{
	ruGPUQUERYTYPE_EVENT						= 0,
	ruGPUQUERYTYPE_OCCLUSION					= 1,

	ruGPUQUERYTYPE_FORCE_DWORD					= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

struct RuDisplayMode
{
	INT32				m_width;
	INT32				m_height;
	INT32				m_refreshRate;
	RuSurfaceFormat		m_format;
};

// ************************************************************************************************************************************************************

// Global function schedule
extern CRuUtility_FunctionScheduler*			g_ruFunctionSchedule;

// Global worker thread
extern CRuThread_Worker*						g_ruGlobalWorkerThread;

// ************************************************************************************************************************************************************

#pragma managed(pop)
