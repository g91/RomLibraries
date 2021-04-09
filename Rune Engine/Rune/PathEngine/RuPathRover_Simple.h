/*!
	@project Rune
	@file RuPathRover_Simple.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2006/02/09
*/

#ifndef _RUPATHROVER_SIMPLE_H_
#define _RUPATHROVER_SIMPLE_H_

#include "../PathEngine/RuPathRover_Base.h"

#include "../Fusion/RuFusion_Sojourner.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuPathRover_Simple : public IRuPathRover_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuObject*								m_colObject;
	BOOL									m_simpleRoving;

	CRuFusion_Sojourner*					m_sojourner;					//!< Used for testing path validity

public:
											CRuPathRover_Simple();
	virtual									~CRuPathRover_Simple();

	void									EnableSimpleRoving(BOOL enable);

	virtual BOOL							SetCollisionObject(IRuObject *colObject);
	virtual BOOL							ReAnchorPoint(CRuVector3 &point);
	virtual BOOL							IsPathValid(const CRuVector3 &startPt, const CRuVector3 &endPt);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
