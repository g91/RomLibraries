/*!
	@file RuExtLink_Granny.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../ExtLink/RuExtLink_Granny.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuThread_CriticalSection *g_ruExLink_Granny_GlobalCS = NULL;

// ************************************************************************************************************************************************************

CRuThread_CriticalSection *RuExtLink_Granny_GlobalCS()
{
	if(g_ruExLink_Granny_GlobalCS == NULL)
	{
		g_ruExLink_Granny_GlobalCS = ruNEW CRuThread_CriticalSection();
	}

	return g_ruExLink_Granny_GlobalCS;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
