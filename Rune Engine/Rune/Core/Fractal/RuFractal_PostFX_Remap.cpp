#include "../../Core/Fractal/RuFractal_PostFX_Remap.h"

#include "../../Core/Math/RuMathBase.h"

#include <math.h>

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFractal_PostFX_Remap, CRuFractal_Base)
ruHEAP_DEFINE_SUBCLASS(CRuFractal_PostFX_Remap, "CRuFractal_PostFX_Remap", "CRuFractal_Base")
ruCLASSGUID_DEFINE(CRuFractal_PostFX_Remap, 0xC11259D0, 0x90B34f16, 0xB74ADF3C, 0x1FEAB03C)

// ************************************************************************************************************************************************************

REAL HermiteSpline_Evaluate(REAL t, CRuVector2 *knots, INT32 numKnots)
{
	// Clamp t to [0, 1]
	if (t < 0.0f)
		t = 0.0f;
	if (t > 1.0f)
		t = 1.0f;

	// Find the segment that t lies within
	int i0 = 0, i1 = 0;
	for (int i = 0; i < numKnots; ++i)
	{
		if (t < knots[i].x)
		{
			i0 = i - 1;
			i1 = i;
			break;
		}
	}

	if (i0 < 0 || i1 >= numKnots)
		return 0.0f;

	if (knots[i1].x > knots[i0].x)
	{
		float m0, m1;

		// Scale t to [0, 1] within the segment
		t = (t - knots[i0].x) / (knots[i1].x - knots[i0].x);

		if (i0 == 0)
		{
			m0 = ((knots[i0 + 1].y - knots[i0].y) * 0.5f);
			m1 = ((knots[i1].y - knots[i1 - 1].y) * 0.5f) + ((knots[i1 + 1].y - knots[i1].y) * 0.5f);
		}
		else if (i1 == numKnots - 1)
		{
			m0 = ((knots[i0].y - knots[i0 - 1].y) * 0.5f) + ((knots[i0 + 1].y - knots[i0].y) * 0.5f);
			m1 = ((knots[i1].y - knots[i1 - 1].y) * 0.5f);
		}
		else
		{
			m0 = ((knots[i0].y - knots[i0 - 1].y) * 0.5f) + ((knots[i0 + 1].y - knots[i0].y) * 0.5f);
			m1 = ((knots[i1].y - knots[i1 - 1].y) * 0.5f) + ((knots[i1 + 1].y - knots[i1].y) * 0.5f);
		}

		return HermiteSpline(knots[i0].y, knots[i1].y, m0, m1, t);
	}

	return knots[i0].y;
}

// ************************************************************************************************************************************************************

CRuFractal_PostFX_Remap::CRuFractal_PostFX_Remap()
:	m_rangeMin(-1.0f),
	m_rangeMax(1.0f)
{
	for(INT32 i = 0; i < 4; ++i)
	{
		m_curve.Add(CRuVector2(i * 0.33f, i * 0.33f));
	}
}

CRuFractal_PostFX_Remap::~CRuFractal_PostFX_Remap()
{
}

REAL CRuFractal_PostFX_Remap::GetRangeMin()
{
	return m_rangeMin;
}

void CRuFractal_PostFX_Remap::SetRangeMin(REAL rangeMin)
{
	m_rangeMin = rangeMin;
}

REAL CRuFractal_PostFX_Remap::GetRangeMax()
{
	return m_rangeMax;
}

void CRuFractal_PostFX_Remap::SetRangeMax(REAL rangeMax)
{
	m_rangeMax = rangeMax;
}

CRuArrayList<CRuVector2> &CRuFractal_PostFX_Remap::GetCurve()
{
	return m_curve;
}

void CRuFractal_PostFX_Remap::SetCurve(CRuArrayList<CRuVector2> &curve)
{
	m_curve.CloneFrom(curve);
}

BOOL CRuFractal_PostFX_Remap::SerializeFrom(IRuStream *stream)
{
	// Read class GUID
	CRuGUID classGUID;
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuFractal_Base::SerializeFrom(stream);

	// Read range min and max
	stream->Read(&m_rangeMin, sizeof(REAL));
	stream->Read(&m_rangeMax, sizeof(REAL));

	if(version >= 2)
	{
		// Read number of knots
		INT32 numKnots;
		stream->Read(&numKnots, sizeof(INT32));
		m_curve.SetArraySize(numKnots);
		m_curve.SetNumEntries(numKnots);

		// Read knots
		for(INT32 i = 0; i < numKnots; ++i)
		{
			stream->Read(&m_curve[i].x, sizeof(REAL));
			stream->Read(&m_curve[i].y, sizeof(REAL));
		}
	}

	return TRUE;
}

BOOL CRuFractal_PostFX_Remap::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 2;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuFractal_Base::SerializeTo(stream);

	// Write range min and max
	stream->Write(&m_rangeMin, sizeof(REAL));
	stream->Write(&m_rangeMax, sizeof(REAL));

	// Write number of knots
	INT32 numKnots = m_curve.Count();
	stream->Write(&numKnots, sizeof(INT32));

	// Write knots
	for(INT32 i = 0; i < numKnots; ++i)
	{
		stream->Write(&m_curve[i].x, sizeof(REAL));
		stream->Write(&m_curve[i].y, sizeof(REAL));
	}

	return TRUE;
}

REAL CRuFractal_PostFX_Remap::Evaluate(REAL x, REAL y, REAL z)
{
//	return ((max(min(x, m_rangeMax), m_rangeMin) - m_rangeMin) / (m_rangeMax - m_rangeMin));
//	return ((max(min(x, m_rangeMax), m_rangeMin) - m_rangeMin) / (m_rangeMax - m_rangeMin)) * 2.0f - 1.0f;

	x = ((max(min(x, m_rangeMax), m_rangeMin) - m_rangeMin) / (m_rangeMax - m_rangeMin));

	return HermiteSpline_Evaluate(x, m_curve.GetArray(), m_curve.Count()) * 2.0f - 1.0f;

//	return Spline_Evaluate(x, m_curve.GetArray(), m_curve.Count()) * 2.0f - 1.0f;
}

// ************************************************************************************************************************************************************

#pragma managed
