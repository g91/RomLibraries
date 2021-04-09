/*!
	@project Rune
	@file RuPathRover_Base.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/12/27
*/

#ifndef _RUPATHROVER_BASE_H_
#define _RUPATHROVER_BASE_H_

#include "../Collision/RuCollision_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class IRuPathRover_Base : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuPathRover_Base();

	virtual BOOL							SetCollisionObject(IRuObject *colObject) = 0;
	virtual BOOL							ReAnchorPoint(CRuVector3 &point) = 0;
	virtual BOOL							IsPathValid(const CRuVector3 &startPt, const CRuVector3 &endPt) = 0;
};

// ************************************************************************************************************************************************************

inline IRuPathRover_Base::~IRuPathRover_Base()
{
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
