// ========================================================================
// $File: //jeffr/granny/rt/x86/x86_granny_cpu_queries.cpp $
// $DateTime: 2006/10/16 14:57:23 $
// $Change: 13583 $
// $Revision: #3 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#if !defined(X86_GRANNY_CPU_QUERIES_H)
#include "x86_granny_cpu_queries.h"
#endif

#if !defined(GRANNY_CPP_SETTINGS_H)
// This should always be the last header included
#include "granny_cpp_settings.h"
#endif

bool GRANNY
SSEIsAvailable(void)
{
	return false;
}
