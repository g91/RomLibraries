/*!
	@file RuExtLink_Granny.h

	Copyright (c) 2004-2008 All rights reserved
*/

#ifndef _RUEXTLINK_GRANNY_H_
#define _RUEXTLINK_GRANNY_H_

#include "../Core/Type/RuType_Object.h"
#include "../Core/Thread/RuThread_CriticalSection.h"

#include "../../Tools/granny/source/granny.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

extern CRuThread_CriticalSection*	RuExtLink_Granny_GlobalCS();

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
