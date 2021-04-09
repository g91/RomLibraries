/*!
	@project Rune
	@file RuPathFinder_Base.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/12/21
*/

#ifndef _RUPATHFINDER_BASE_H_
#define _RUPATHFINDER_BASE_H_

#include "../PathEngine/RuPathEngine_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class IRuPathFinder_Base : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuPathFinder_Base();

	virtual BOOL							SetMaximumExpansions(INT32 maxExpansions) = 0;
	virtual BOOL							FindPath(CRuPathNode *startNode, CRuPathNode *endNode, CRuArrayList<CRuPathNode *> &pathOut) = 0;
};

// ************************************************************************************************************************************************************

inline IRuPathFinder_Base::~IRuPathFinder_Base()
{
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
