/*!
	@file RuTextureGenerator.cpp

	Copyright (c) 2006-2007 All rights reserved
*/

#include "../../Engine/Texture/RuTextureGenerator.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuTextureGenerator, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuTextureGenerator, "CRuTextureGenerator", "IRuObject")
ruCLASSGUID_DEFINE(CRuTextureGenerator, 0xB694A50F, 0x03DF4f1a, 0x9FCFED33, 0x1CE23358)

// ************************************************************************************************************************************************************

CRuTextureGenerator::CRuTextureGenerator()
:	m_fractalGenerator(NULL),
	m_fractalPeriod(1.0f),
	m_enableLooping(FALSE),
	m_textureWidth(0),
	m_textureHeight(0),
	m_textureBuffer(NULL)
{
}

CRuTextureGenerator::~CRuTextureGenerator()
{
	ruSAFE_RELEASE(m_fractalGenerator);
}

void CRuTextureGenerator::SetFractalGenerator(CRuFractal_Base *fractalGenerator)
{
	if(fractalGenerator)
	{
		fractalGenerator->AddRef();
		ruSAFE_RELEASE(m_fractalGenerator);
		m_fractalGenerator = fractalGenerator;
	}
}

void CRuTextureGenerator::SetFractalPeriod(REAL period)
{
	m_fractalPeriod = period;
}

void CRuTextureGenerator::EnableLooping(BOOL enableLooping)
{
	m_enableLooping = enableLooping;
}

INT32 CRuTextureGenerator::Begin(INT32 width, INT32 height, REAL t)
{
	if(m_fractalGenerator)
	{
		// Re-initialize the texture buffer if necessary
		if(m_textureWidth != width || m_textureHeight != height)
		{
			delete[] m_textureBuffer;

			m_textureWidth = width;
			m_textureHeight = height;
			m_textureBuffer = ruNEW BYTE [m_textureWidth * m_textureHeight];
		}

		m_t = t;

		// Calculate total number of steps required to generate the texture buffer
		m_totalStepCount = m_textureWidth * m_textureHeight;

		// Reset progress counters
		m_curStepCount = 0;
		m_curX = 0;
		m_curY = 0;

		return m_totalStepCount;
	}

	return 0;
}

INT32 CRuTextureGenerator::Generate(INT32 stepCount)
{
	INT32 stepsToExecute = min(stepCount, m_totalStepCount - m_curStepCount);

	for(INT32 i = 0; i < stepsToExecute; ++i, ++m_curStepCount)
	{
		REAL v0 = m_fractalGenerator->Evaluate(static_cast<REAL>(m_curX), static_cast<REAL>(m_curY), m_t) * (m_textureWidth - m_curX) * (m_textureHeight - m_curY);
		REAL v1 = m_fractalGenerator->Evaluate(static_cast<REAL>(m_curX - m_textureWidth), static_cast<REAL>(m_curY), m_t) * (m_curX) * (m_textureHeight - m_curY);
		REAL v2 = m_fractalGenerator->Evaluate(static_cast<REAL>(m_curX - m_textureWidth), static_cast<REAL>(m_curY - m_textureHeight), m_t) * (m_curX) * (m_curY);
		REAL v3 = m_fractalGenerator->Evaluate(static_cast<REAL>(m_curX), static_cast<REAL>(m_curY - m_textureHeight), m_t) * (m_textureWidth - m_curX) * (m_curY);

		REAL value = ((v0 + v1 + v2 + v3) / (m_textureWidth * m_textureHeight) + 1.0f) * 0.5f;

		// Convert to 8-bit gray-scale
		m_textureBuffer[m_curStepCount] = static_cast<BYTE>(value * 255.0f);

/*
		// NON-TILING VERSION

		// Evaluate fractal
		REAL value = (m_fractalGenerator->Evaluate(static_cast<REAL>(m_curX), static_cast<REAL>(m_curY), m_t) + 1.0f) * 0.5f;

		// Convert to 8-bit gray-scale
		m_textureBuffer[m_curStepCount] = static_cast<BYTE>(value * 255.0f);
*/
		// Increment X, Y counters
		++m_curX;
		if(m_curX >= m_textureWidth)
		{
			m_curX = 0;
			++m_curY;
		}
	}

	return stepsToExecute;
}

BOOL CRuTextureGenerator::CopyTexture(IRuTexture *texture)
{
	if(texture->GetLevelWidth(0) != m_textureWidth || texture->GetLevelHeight(0) != m_textureHeight)
	{
		return FALSE;
	}

	switch(texture->GetSurfaceFormat())
	{
		case ruSURFACEFMT_A8:
		case ruSURFACEFMT_L8:
			{
				BYTE *srcBits = m_textureBuffer;
				BYTE *dstBits;
				INT32 dstPitch;

				texture->Lock(0, reinterpret_cast<void **>(&dstBits), &dstPitch);

				for(INT32 i = 0; i < m_textureHeight; ++i)
				{
					memcpy(dstBits, srcBits, sizeof(BYTE) * m_textureWidth);

					srcBits += m_textureWidth;
					dstBits += dstPitch;
				}

				texture->Unlock(0);
			}

			return TRUE;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed
