/*!
	@project Rune
	@file RuImageFX_Resample.cpp

	Copyright (c) 2004-2006 All rights reserved

	@author John Tang
	@date 2005/12/08
*/

#include "../ImageFX/RuImageFX_Resample.h"
#include "../Core/Math/RuMathBase.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

CRuImageFX_Filter_Cubic::CRuImageFX_Filter_Cubic()
{
	m_support = 2.0f;
	m_blur = 0.6f;
	m_windowed = FALSE;
	m_cardinal = FALSE;
	m_unitRange = TRUE;
}

CRuImageFX_Filter_Cubic::~CRuImageFX_Filter_Cubic()
{
}

REAL CRuImageFX_Filter_Cubic::Sample(REAL x, REAL support)
{
	REAL t;

	if(x < -2.0f) 
		return 0.0f;

	if(x < -1.0f)
	{
		t = 2.0f + x;
		return t * t * t / 6.0f;
	}

	if(x < 0.0f)
		return (4.0f + x * x * (-6.0f + x * -3.0f)) / 6.0f;

	if(x < 1.0f) 
		return (4.0f + x * x * (-6.0f + x * 3.0f)) / 6.0f;

	if(x < 2.0f)
	{
		t = 2.0f - x;
		return t * t * t / 6.0f;
	}

	return 0.0f;
}

// ************************************************************************************************************************************************************

REAL sinc(REAL x, REAL support)
{
	if(x == 0.0f)
		return 1.0f;

	return(sin(ruPI * x) / (ruPI * x));
}

CRuImageFX_Filter_Lanczos::CRuImageFX_Filter_Lanczos()
{
	m_support = 3.0f;
	m_blur = 1.0f;
	m_windowed = FALSE;
	m_cardinal = FALSE;
	m_unitRange = TRUE;
}

CRuImageFX_Filter_Lanczos::~CRuImageFX_Filter_Lanczos()
{
}

REAL CRuImageFX_Filter_Lanczos::Sample(REAL x, REAL support)
{
/*
#define B   (1.0/3.0)
#define C   (1.0/3.0)
#define P0  ((  6.0- 2.0*B       )/6.0)
#define P2  ((-18.0+12.0*B+ 6.0*C)/6.0)
#define P3  (( 12.0- 9.0*B- 6.0*C)/6.0)
#define Q0  ((       8.0*B+24.0*C)/6.0)
#define Q1  ((     -12.0*B-48.0*C)/6.0)
#define Q2  ((       6.0*B+30.0*C)/6.0)
#define Q3  ((     - 1.0*B- 6.0*C)/6.0)

	if (x < -2.0)
		return(0.0);
	if (x < -1.0)
		return(Q0-x*(Q1-x*(Q2-x*Q3)));
	if (x < 0.0)
		return(P0+x*x*(P2-x*P3));
	if (x < 1.0)
		return(P0+x*x*(P2+x*P3));
	if (x < 2.0)
		return(Q0+x*(Q1+x*(Q2+x*Q3)));
	return(0.0);
*/
/*
	if(x < -3.0f)
		return 0.0f;

	if(x < 0.0f)
		return sinc(-x,support) * sinc(-x/3.0f,support);

	if(x < 3.0f)
		return sinc(x,support) * sinc(x/3.0f,support);

	return(0.0);
*/

	if(fabs(x) >= 3.0f)
		return 0.0f;

	return sinc(x / 3.0f, support) / (x / 3.0f);
}

// ************************************************************************************************************************************************************

CRuImageFX_Resampler_8::CRuImageFX_Resampler_8()
:	m_workspaceSize(0),
	m_workspace(NULL)
{
}

CRuImageFX_Resampler_8::~CRuImageFX_Resampler_8()
{
	delete[] m_workspace;
}

BOOL CRuImageFX_Resampler_8::ResampleImage(BYTE *srcBits, INT32 srcWidth, INT32 srcHeight, INT32 srcPitch, BYTE *dstBits, INT32 dstWidth, INT32 dstHeight, INT32 dstPitch)
{
	// Determine the workspace size necessary for processing and adjust workspace size if necessary
	INT32 requiredWorkspaceSize = dstWidth * srcHeight;
	if(requiredWorkspaceSize > m_workspaceSize)
	{
		delete[] m_workspace;

		m_workspaceSize = requiredWorkspaceSize;
		m_workspace = ruNEW BYTE [m_workspaceSize];
	}

	// Resample image along the x direction
	for(INT32 y = 0; y < srcHeight; ++y)
	{
		Resample_Inverse(&srcBits[y * srcPitch], srcWidth, 1, &m_workspace[y * dstWidth], dstWidth, 1);
	}

	// Resample image along the y direction
	for(INT32 x = 0; x < dstWidth; ++x)
	{
		Resample_Inverse(&m_workspace[x], srcHeight, dstWidth, &dstBits[x], dstHeight, dstPitch);
	}

	return TRUE;
}

CRuImageFX_Resampler_8::WeightTable *CRuImageFX_Resampler_8::BuildWeightTable(REAL center, INT32 srcLength, INT32 dstLength, CRuImageFX_Filter_Base *filter)
{
	WeightTable *weightTable = ruNEW WeightTable();

	REAL dstSrcFactor = ((REAL) dstLength / srcLength);
	REAL blur = filter->Blur();
	REAL scale = blur * max(1.0f / dstSrcFactor, 1.0f);
	REAL support = scale * filter->Support();
	scale = 1.0f / scale;

	REAL width;
	REAL fscale;

	if(dstSrcFactor < 1.0f)
	{
		width = filter->Support() / dstSrcFactor;
		fscale = 1.0f / scale;
	}
	else
	{
		width = filter->Support();
		fscale = 1.0f;
	}

	// Find source coordinate range of this positioned kernel
	weightTable->m_i0 = static_cast<INT32>(floor(center - width));
	weightTable->m_i1 = static_cast<INT32>(ceil(center + width)) + 1;

	// Allocate table
	weightTable->m_indices = ruNEW INT32 [weightTable->m_i1 - weightTable->m_i0];
	weightTable->m_weights = ruNEW REAL [weightTable->m_i1 - weightTable->m_i0];

	// Compute discrete sampled kernel coefficients
	for(INT32 i = weightTable->m_i0; i < weightTable->m_i1; ++i)
	{
		weightTable->m_indices[i - weightTable->m_i0] = min(srcLength, max(0, i));

		// Evaluate kernel at position
		weightTable->m_weights[i - weightTable->m_i0] = filter->Sample((center - i) / fscale, support) / fscale;
	}

	return weightTable;
}

BOOL CRuImageFX_Resampler_8::Resample_Inverse(BYTE *srcBits, INT32 srcLen, INT32 srcStride, BYTE *dstBits, INT32 dstLen, INT32 dstStride)
{
	CRuImageFX_Filter_Cubic *filter = ruNEW CRuImageFX_Filter_Cubic;

	REAL dstSrcFactor = ((REAL) dstLen / srcLen);

	for(INT32 x = 0, i = 0; x < dstLen * dstStride; x += dstStride, ++i)
	{
		// Convert x into src image coordinates
		REAL srcX = i / dstSrcFactor;

		WeightTable *wt = BuildWeightTable(srcX, srcLen, dstLen, filter);

		// Accumulate or something
		REAL colorSum = 0.0f;
		REAL sizeFactor = 0.0f;

		for(INT32 j = wt->m_i0; j < wt->m_i1; ++j)
		{
			REAL color0 = srcBits[wt->m_indices[j - wt->m_i0] * srcStride] / 255.0f;
			colorSum += (color0 * wt->m_weights[j - wt->m_i0]);
			colorSum += (color0 * wt->m_weights[j - wt->m_i0]);
		}

		// Init output with normalized accumulator
		colorSum = colorSum * (1.0f / sizeFactor);
		colorSum = min(255.0f, colorSum * 255.0f);
		dstBits[x] = static_cast<BYTE>(colorSum);

		// Free weight table
		delete[] wt->m_indices;
		delete[] wt->m_weights;
		delete wt;
	}

	delete filter;

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuImageFX_Resampler_32::CRuImageFX_Resampler_32()
:	m_workspaceSize(0),
	m_workspace(NULL)
{
	m_filter = ruNEW CRuImageFX_Filter_Cubic();

	m_currentWeightTable = ruNEW WeightTable();
	m_currentWeightTable->m_indices = ruNEW INT32 [256];
	m_currentWeightTable->m_weights = ruNEW REAL [256];
}

CRuImageFX_Resampler_32::~CRuImageFX_Resampler_32()
{
	delete m_filter;
	delete[] m_workspace;
	delete[] m_currentWeightTable->m_indices;
	delete[] m_currentWeightTable->m_weights;
	delete m_currentWeightTable;
}

BOOL CRuImageFX_Resampler_32::ResampleImage(DWORD *srcBits, INT32 srcWidth, INT32 srcHeight, INT32 srcPitch, DWORD *dstBits, INT32 dstWidth, INT32 dstHeight, INT32 dstPitch)
{
	// Determine the workspace size necessary for processing and adjust workspace size if necessary
	INT32 requiredWorkspaceSize = dstWidth * srcHeight;
	if(requiredWorkspaceSize > m_workspaceSize)
	{
		delete[] m_workspace;

		m_workspaceSize = requiredWorkspaceSize;
		m_workspace = ruNEW DWORD [m_workspaceSize];
	}

	// Resample image along the x direction
	for(INT32 y = 0; y < srcHeight; ++y)
	{
//		Resample_Fants_Linear(&srcBits[y * srcPitch], srcWidth, 1, &m_workspace[y * dstWidth], dstWidth, 1);
		Resample_Inverse(&srcBits[y * srcPitch], srcWidth, 1, &m_workspace[y * dstWidth], dstWidth, 1);
	}

	// Resample image along the y direction
	for(INT32 x = 0; x < dstWidth; ++x)
	{
//		Resample_Fants_Linear(&m_workspace[x], srcHeight, dstWidth, &dstBits[x], dstHeight, dstPitch);
		Resample_Inverse(&m_workspace[x], srcHeight, dstWidth, &dstBits[x], dstHeight, dstPitch);
	}

	return TRUE;
}

BOOL CRuImageFX_Resampler_32::Resample_Fants_Linear(DWORD *srcBits, INT32 srcLen, INT32 srcStride, DWORD *dstBits, INT32 dstLen, INT32 dstStride)
{
	REAL sizeFactor = (REAL) dstLen / (REAL) srcLen;
	REAL inverseScaleFactor = 1.0f / sizeFactor;
	REAL inSeg = 1.0f;
	REAL outSeg = inverseScaleFactor;

	CRuColor4 colorSum(0.0f, 0.0f, 0.0f, 0.0f);

	for(INT32 x = 0, u = 0; x < dstLen * dstStride; )
	{
		// Linear interpolation
		CRuColor4 color0(srcBits[u]);
		CRuColor4 color1(srcBits[u + 1]);
		CRuColor4 intensity = color0 * inSeg + color1 * (1.0f - inSeg);

		// inSeg < outSeg: Input pixel is entirely consumed before output pixel
		if(inSeg < outSeg)
		{
			// Accumulate weighted contribution
			colorSum += (intensity * inSeg);

			// inSeg portion has been filled
			outSeg -= inSeg;

			// New input pixel will be available
			inSeg = 1.0f;

			// Index into next input pixel
			u += srcStride;
		}
		// inSeg >= outSeg: Input pixel is not entirely consumed before output pixel
		else
		{
			// Accumulate weighted contribution
			colorSum += (intensity * outSeg);

			// Init output with normalized accumulator
			colorSum = colorSum * sizeFactor;
			colorSum.Clamp();
			dstBits[x] = colorSum.GetARGB();

			// Reset accumulator for next output pixel
			colorSum.Set(0.0f, 0.0f, 0.0f, 0.0f);

			// outSeg portion of input has been used
			inSeg -= outSeg;

			// Restore outSeg
			outSeg = inverseScaleFactor;

			// Index into next output pixel
			x += dstStride;
		}
	}

	return TRUE;
}

CRuImageFX_Resampler_32::WeightTable *CRuImageFX_Resampler_32::BuildWeightTable(REAL center, INT32 srcLength, INT32 dstLength, CRuImageFX_Filter_Base *filter)
{
//	WeightTable *weightTable = ruNEW WeightTable();

	REAL dstSrcFactor = ((REAL) dstLength / srcLength);
	REAL blur = filter->Blur();
	REAL scale = blur * max(1.0f / dstSrcFactor, 1.0f);
	REAL support = scale * filter->Support();
	scale = 1.0f / scale;

	REAL width;
	REAL fscale;

	if(dstSrcFactor < 1.0f)
	{
		width = filter->Support() / dstSrcFactor;
		fscale = 1.0f / scale;
	}
	else
	{
		width = filter->Support();
		fscale = 1.0f;
	}

	// Find source coordinate range of this positioned kernel
	m_currentWeightTable->m_i0 = static_cast<INT32>(floor(center - width));
	m_currentWeightTable->m_i1 = static_cast<INT32>(ceil(center + width)) + 1;

	// Allocate table
//	weightTable->m_indices = ruNEW INT32 [weightTable->m_i1 - weightTable->m_i0];
//	weightTable->m_weights = ruNEW REAL [weightTable->m_i1 - weightTable->m_i0];

//	assert(m_currentWeightTable->m_i1 - m_currentWeightTable->m_i0 < 64);

	// Compute discrete sampled kernel coefficients
	for(INT32 i = m_currentWeightTable->m_i0; i < m_currentWeightTable->m_i1; ++i)
	{
		// Store relative index
		m_currentWeightTable->m_indices[i - m_currentWeightTable->m_i0] = min(srcLength - 1, max(0, i));

		// Evaluate kernel at position
		m_currentWeightTable->m_weights[i - m_currentWeightTable->m_i0] = filter->Sample((center - i) / fscale, support) / fscale;
	}

	return m_currentWeightTable;
}

BOOL CRuImageFX_Resampler_32::Resample_Inverse(DWORD *srcBits, INT32 srcLen, INT32 srcStride, DWORD *dstBits, INT32 dstLen, INT32 dstStride)
{
	for(INT32 x = 0, i = 0; x < dstLen * dstStride; x += dstStride, ++i)
	{
		REAL dstSrcFactor = ((REAL) dstLen / srcLen);

		// Convert x into src image coordinates
		REAL srcX = i / dstSrcFactor;

		BuildWeightTable(srcX, srcLen, dstLen, m_filter);


		// Accumulate or something
		REAL colorSum = 0.0f;
		REAL sizeFactor = 0.0f;

		for(INT32 j = m_currentWeightTable->m_i0; j < m_currentWeightTable->m_i1; ++j)
		{
//			REAL color0 = ((srcBits[m_currentWeightTable->m_indices[j - m_currentWeightTable->m_i0] * srcStride] & 0xFF000000) >> 24);
			REAL color0 = (REAL) (srcBits[m_currentWeightTable->m_indices[j - m_currentWeightTable->m_i0] * srcStride] & 0x000000FF);
			colorSum += (color0 * m_currentWeightTable->m_weights[j - m_currentWeightTable->m_i0]);
			sizeFactor += m_currentWeightTable->m_weights[j - m_currentWeightTable->m_i0];
		}

		// Init output with normalized accumulator
		colorSum = colorSum * (1.0f / sizeFactor);
		colorSum = max(0.0f, min(255.0f, colorSum));
		dstBits[x] = static_cast<BYTE>(colorSum) | 0xFFFFFF00;

/*
		// Accumulate or something
		CRuColor4 colorSum(0.0f, 0.0f, 0.0f, 0.0f);
		REAL sizeFactor = 0.0f;

		for(INT32 j = m_currentWeightTable->m_i0; j < m_currentWeightTable->m_i1; ++j)
		{
			CRuColor4 color0(srcBits[m_currentWeightTable->m_indices[j - m_currentWeightTable->m_i0] * srcStride]);
			colorSum += (color0 * m_currentWeightTable->m_weights[j - m_currentWeightTable->m_i0]);
			sizeFactor += m_currentWeightTable->m_weights[j - m_currentWeightTable->m_i0];
		}

		// Init output with normalized accumulator
		colorSum = colorSum * (1.0f / sizeFactor);
		colorSum.Clamp();
		dstBits[x] = colorSum.GetARGB();
*/
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
