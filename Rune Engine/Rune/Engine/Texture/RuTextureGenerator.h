/*!
	@file RuTextureGenerator.h

	Copyright (c) 2006-2007 All rights reserved
*/

#ifndef _RUTEXTUREGENERATOR_H_
#define _RUTEXTUREGENERATOR_H_

#include "../../Engine/Base/RuCore.h"
#include "../../Core/Fractal/RuFractal_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuTextureGenerator : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuFractal_Base*						m_fractalGenerator;			//!< Fractal generator
	REAL									m_fractalPeriod;			//!< Period over which the fractal repeats

	BOOL									m_enableLooping;			//!< Enable texture looping?

	INT32									m_textureWidth;				//!< Target texture width
	INT32									m_textureHeight;			//!< Target texture height
	BYTE*									m_textureBuffer;
	REAL									m_t;

	INT32									m_totalStepCount;
	INT32									m_curStepCount;
	INT32									m_curX;
	INT32									m_curY;

public:
											CRuTextureGenerator();
	virtual									~CRuTextureGenerator();

	void									SetFractalGenerator(CRuFractal_Base *fractalGenerator);
	void									SetFractalPeriod(REAL period);
	void									EnableLooping(BOOL enableLooping);

	INT32									Begin(INT32 width, INT32 height, REAL t);
	INT32									Generate(INT32 stepCount);

	BOOL									CopyTexture(IRuTexture *texture);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
