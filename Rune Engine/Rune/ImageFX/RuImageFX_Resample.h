/*!
	@project Rune
	@file RuImageFX_Resample.h

	Copyright (c) 2004-2006 All rights reserved

	@author John Tang
	@date 2005/12/08
*/

#ifndef _RUIMAGEFX_RESAMPLE_H_
#define _RUIMAGEFX_RESAMPLE_H_

#include "../Core/Type/RuType_Object.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuImageFX_Filter_Base
{
protected:
	REAL							m_support;					//!< Radius of non-zero portion
	REAL							m_blur;						//!< Blur factor (1 = normal)
	BOOL							m_windowed;					//!< Should the filter be windowed?
	BOOL							m_cardinal;					//!< Is this filter cardinal? e.g. does func(x) = (x==0) for integer x?
	BOOL							m_unitRange;				//!< Does the filter stay within unit range [0, 1]?

public:
									CRuImageFX_Filter_Base();
	virtual							~CRuImageFX_Filter_Base();

	REAL							Support() const;
	REAL							Blur() const;
	BOOL							Windowed() const;
	BOOL							Cardinal() const;
	BOOL							UnitRange() const;

	virtual REAL					Sample(REAL x, REAL support) = 0;
};

// ************************************************************************************************************************************************************

class CRuImageFX_Filter_Cubic : public CRuImageFX_Filter_Base
{
public:
									CRuImageFX_Filter_Cubic();
	virtual							~CRuImageFX_Filter_Cubic();

	virtual REAL					Sample(REAL x, REAL support);
};

// ************************************************************************************************************************************************************

class CRuImageFX_Filter_Lanczos : public CRuImageFX_Filter_Base
{
public:
									CRuImageFX_Filter_Lanczos();
	virtual							~CRuImageFX_Filter_Lanczos();

	virtual REAL					Sample(REAL x, REAL support);
};

// ************************************************************************************************************************************************************

class CRuImageFX_Resampler_8
{
protected:
	struct WeightTable
	{
		INT32						m_i0;
		INT32						m_i1;
		INT32*						m_indices;
		REAL*						m_weights;
	};

	INT32							m_workspaceSize;
	BYTE*							m_workspace;

public:
									CRuImageFX_Resampler_8();
	virtual							~CRuImageFX_Resampler_8();

	BOOL							ResampleImage(BYTE *srcBits, INT32 srcWidth, INT32 srcHeight, INT32 srcPitch, BYTE *dstBits, INT32 dstWidth, INT32 dstHeight, INT32 dstPitch);

protected:
	WeightTable*					BuildWeightTable(REAL center, INT32 srcLength, INT32 dstLength, CRuImageFX_Filter_Base *filter);
	BOOL							Resample_Inverse(BYTE *srcBits, INT32 srcLen, INT32 srcStride, BYTE *dstBits, INT32 dstLen, INT32 dstStride);
};

// ************************************************************************************************************************************************************

class CRuImageFX_Resampler_32
{
protected:
	struct WeightTable
	{
		INT32						m_i0;
		INT32						m_i1;
		INT32*						m_indices;
		REAL*						m_weights;
	};

	CRuImageFX_Filter_Base*			m_filter;

	INT32							m_workspaceSize;
	DWORD*							m_workspace;
	WeightTable*					m_currentWeightTable;

public:
									CRuImageFX_Resampler_32();
	virtual							~CRuImageFX_Resampler_32();

	BOOL							ResampleImage(DWORD *srcBits, INT32 srcWidth, INT32 srcHeight, INT32 srcPitch, DWORD *dstBits, INT32 dstWidth, INT32 dstHeight, INT32 dstPitch);

protected:
	BOOL							Resample_Fants_Linear(DWORD *srcBits, INT32 srcLen, INT32 srcStride, DWORD *dstBits, INT32 dstLen, INT32 dstStride);

	WeightTable*					BuildWeightTable(REAL center, INT32 srcLength, INT32 dstLength, CRuImageFX_Filter_Base *filter);

	BOOL							Resample_Inverse(DWORD *srcBits, INT32 srcLen, INT32 srcStride, DWORD *dstBits, INT32 dstLen, INT32 dstStride);
};

// ************************************************************************************************************************************************************

inline CRuImageFX_Filter_Base::CRuImageFX_Filter_Base()
:	m_support(0.0f),
	m_blur(1.0f),
	m_windowed(FALSE),
	m_cardinal(FALSE),
	m_unitRange(TRUE)
{
}

inline CRuImageFX_Filter_Base::~CRuImageFX_Filter_Base()
{
}

inline REAL CRuImageFX_Filter_Base::Support() const
{
	return m_support;
}

inline REAL CRuImageFX_Filter_Base::Blur() const
{
	return m_blur;
}

inline BOOL CRuImageFX_Filter_Base::Windowed() const
{
	return m_windowed;
}

inline BOOL CRuImageFX_Filter_Base::Cardinal() const
{
	return m_cardinal;
}

inline BOOL CRuImageFX_Filter_Base::UnitRange() const
{
	return m_unitRange;
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
