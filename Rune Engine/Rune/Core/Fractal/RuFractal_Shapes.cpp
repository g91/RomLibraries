#include "../../Core/Fractal/RuFractal_Shapes.h"

#include <math.h>

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFractal_Shapes, CRuFractal_Base)
ruHEAP_DEFINE_SUBCLASS(CRuFractal_Shapes, "CRuFractal_Shapes", "CRuFractal_Base")
ruCLASSGUID_DEFINE(CRuFractal_Shapes, 0xD893CA84, 0xE80E4de6, 0x90F3D2DD, 0x8995F965)

// ************************************************************************************************************************************************************

CRuFractal_Shapes::CRuFractal_Shapes()
:	m_H(1.0f),
	m_lacunarity(2.0f),
	m_octaves(1.0f),
	m_exponentArray(NULL)
{
	m_scale = 0.01f;
}

CRuFractal_Shapes::~CRuFractal_Shapes()
{
	ruSAFE_RELEASE(m_noiseFunction);
	delete[] m_exponentArray;
}

RuFractalShapeFunction CRuFractal_Shapes::GetXShapeFunction()
{
	return m_xShapeFunction;
}

void CRuFractal_Shapes::SetXShapeFunction(RuFractalShapeFunction fn)
{
	m_xShapeFunction = fn;
}

RuFractalShapeFunction CRuFractal_Shapes::GetYShapeFunction()
{
	return m_yShapeFunction;
}

void CRuFractal_Shapes::SetYShapeFunction(RuFractalShapeFunction fn)
{
	m_yShapeFunction = fn;
}

REAL CRuFractal_Shapes::GetH() const
{
	return m_H;
}

void CRuFractal_Shapes::SetH(REAL H)
{
	m_H = H;

	// Compute and store spectral weights
	BuildExponentArray();
}

REAL CRuFractal_Shapes::GetLacunarity() const
{
	return m_lacunarity;
}

void CRuFractal_Shapes::SetLacunarity(REAL lacunarity)
{
	m_lacunarity = lacunarity;

	// Compute and store spectral weights
	BuildExponentArray();
}

REAL CRuFractal_Shapes::GetOctaves() const
{
	return m_octaves;
}

void CRuFractal_Shapes::SetOctaves(REAL octaves)
{
	m_octaves = octaves;

	// Compute and store spectral weights
	BuildExponentArray();
}

BOOL CRuFractal_Shapes::SerializeFrom(IRuStream *stream)
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

	// Read x function
	stream->Read(&m_xShapeFunction, sizeof(RuFractalShapeFunction));

	// Read y function
	stream->Read(&m_yShapeFunction, sizeof(RuFractalShapeFunction));

	// Read H
	stream->Read(&m_H, sizeof(REAL));

	// Read lacunarity
	stream->Read(&m_lacunarity, sizeof(REAL));

	// Read octaves
	stream->Read(&m_octaves, sizeof(REAL));

	return TRUE;
}

BOOL CRuFractal_Shapes::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuFractal_Base::SerializeTo(stream);

	// Write x function
	stream->Write(&m_xShapeFunction, sizeof(RuFractalShapeFunction));

	// Write y function
	stream->Write(&m_yShapeFunction, sizeof(RuFractalShapeFunction));

	// Write H
	stream->Write(&m_H, sizeof(REAL));

	// Write lacunarity
	stream->Write(&m_lacunarity, sizeof(REAL));

	// Write octaves
	stream->Write(&m_octaves, sizeof(REAL));

	return TRUE;
}

REAL CRuFractal_Shapes::Evaluate(REAL x, REAL y, REAL z)
{
	INT32 i;
	REAL value, frequency, remainder;

	// Compute and store spectral weights
	BuildExponentArray();

	// Adjust input coordinates
	x = x * m_scale;
	y = y * m_scale;
	z = z * m_scale;

	// Initialize vars to proper values
	value = 0.0f;
	frequency = 1.0f;

	// Inner loop of spectral construction
	for(i = 0; i < m_octaves; ++i)
	{
		value += (EvaluateFunction(x, m_xShapeFunction) + EvaluateFunction(y, m_yShapeFunction)) * m_exponentArray[i];

		x *= m_lacunarity;
		y *= m_lacunarity;
		z *= m_lacunarity;
	}

	// Add in ``m_octaves''  remainder
	// ``i''  and spatial freq. are preset in loop above
	remainder = m_octaves - static_cast<INT32>(m_octaves);
	if(remainder != 0.0f)
	{
		value += remainder * (EvaluateFunction(x, m_xShapeFunction) + EvaluateFunction(y, m_yShapeFunction)) * m_exponentArray[i];
	}

	return (value + m_bias) * m_factor;
}

void CRuFractal_Shapes::BuildExponentArray()
{
	REAL frequency;

	if(m_exponentArray == NULL)
	{
		m_exponentArray = ruNEW REAL [32];
	}

	// Compute and store spectral weights
	frequency = 1.0f;
	for(INT32 i = 0; i <= m_octaves; ++i)
	{
		// Compute weight for each frequency
		m_exponentArray[i] = pow(frequency, -m_H);
		frequency *= m_lacunarity;
	}
}

REAL CRuFractal_Shapes::EvaluateFunction(REAL t, RuFractalShapeFunction fn)
{
	switch(fn)
	{
		case ruFRACTALSHAPEFN_SINE:
			return sin(t);
		case ruFRACTALSHAPEFN_COSINE:
			return  cos(t);
		case ruFRACTALSHAPEFN_SAWTOOTH:
			break;
	}

	return 0.0f;
}

// ************************************************************************************************************************************************************

#pragma managed
