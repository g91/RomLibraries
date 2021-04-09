#include "../../Core/Fractal/RuFractal_HybridMultifractal.h"

#include <math.h>

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFractal_HybridMultifractal, CRuFractal_Base)
ruHEAP_DEFINE_SUBCLASS(CRuFractal_HybridMultifractal, "CRuFractal_HybridMultifractal", "CRuFractal_Base")
ruCLASSGUID_DEFINE(CRuFractal_HybridMultifractal, 0x5B62A959, 0x69544a23, 0x81F4076F, 0x8642A23D)

// ************************************************************************************************************************************************************

CRuFractal_HybridMultifractal::CRuFractal_HybridMultifractal()
:	m_H(1.0f),
	m_lacunarity(2.0f),
	m_octaves(4.0f),
	m_offset(1.0f),
	m_exponentArray(NULL)
{
	m_offset = 0.01f;
}

CRuFractal_HybridMultifractal::~CRuFractal_HybridMultifractal()
{
	ruSAFE_RELEASE(m_noiseFunction);
	delete[] m_exponentArray;
}

REAL CRuFractal_HybridMultifractal::GetH() const
{
	return m_H;
}

void CRuFractal_HybridMultifractal::SetH(REAL H)
{
	m_H = H;

	// Compute and store spectral weights
	BuildExponentArray();
}

REAL CRuFractal_HybridMultifractal::GetLacunarity() const
{
	return m_lacunarity;
}

void CRuFractal_HybridMultifractal::SetLacunarity(REAL lacunarity)
{
	m_lacunarity = lacunarity;

	// Compute and store spectral weights
	BuildExponentArray();
}

REAL CRuFractal_HybridMultifractal::GetOctaves() const
{
	return m_octaves;
}

void CRuFractal_HybridMultifractal::SetOctaves(REAL octaves)
{
	m_octaves = octaves;

	// Compute and store spectral weights
	BuildExponentArray();
}

REAL CRuFractal_HybridMultifractal::GetOffset() const
{
	return m_offset;
}

void CRuFractal_HybridMultifractal::SetOffset(REAL offset)
{
	m_offset = offset;
}

BOOL CRuFractal_HybridMultifractal::SerializeFrom(IRuStream *stream)
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

	// Read H
	stream->Read(&m_H, sizeof(REAL));

	// Read lacunarity
	stream->Read(&m_lacunarity, sizeof(REAL));

	// Read octaves
	stream->Read(&m_octaves, sizeof(REAL));

	// Read offset
	stream->Read(&m_offset, sizeof(REAL));

	return TRUE;
}

BOOL CRuFractal_HybridMultifractal::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuFractal_Base::SerializeTo(stream);

	// Write H
	stream->Write(&m_H, sizeof(REAL));

	// Write lacunarity
	stream->Write(&m_lacunarity, sizeof(REAL));

	// Write octaves
	stream->Write(&m_octaves, sizeof(REAL));

	// Write offset
	stream->Write(&m_offset, sizeof(REAL));

	return TRUE;
}

REAL CRuFractal_HybridMultifractal::Evaluate(REAL x, REAL y, REAL z)
{
	INT32 i;
	REAL result, signal, weight, remainder;

	// Compute and store spectral weights
	BuildExponentArray();

	// Adjust input coordinates
	x = x * m_scale;
	y = y * m_scale;
	z = z * m_scale;

	// get first octave of function
	result = (m_noiseFunction->Evaluate(x, y, z) + m_offset) * m_exponentArray[0];
	weight = result;
	
	// increase frequency
	x *= m_lacunarity;
	y *= m_lacunarity;
	z *= m_lacunarity;

	// spectral construction inner loop, where the fractal is built
	for(i = 1; i < m_octaves; i++)
	{
		// prevent divergence
		if(weight > 1.0f)
			weight = 1.0f;

		// get next higher frequency
		signal = (m_noiseFunction->Evaluate(x, y, z) + m_offset) * m_exponentArray[i];

		// add it in, weighted by previous freq's local value
		result += weight * signal;

		// update the (monotonically decreasing) weighting value
		// (this is why H must specify a high fractal dimension)
		weight *= signal;

		// increase frequency
		x *= m_lacunarity;
		y *= m_lacunarity;
		z *= m_lacunarity;
	}

	// take care of remainder in ``octaves'' 
	remainder = m_octaves - static_cast<INT32>(m_octaves);
	if(remainder != 0.0f)
	{
		// ``i''  and spatial freq. are preset in loop above
		result += remainder * m_noiseFunction->Evaluate(x, y, z) * m_exponentArray[i];
	}

	result = result / 2.0f - 1.0f;
//	return (((result + 1.0f) / 2.0f) + m_bias) * m_factor;
//	return ((result + m_bias) * m_factor + 1.0f) / 2.0f;
	return (result + m_bias) * m_factor;
//	return max(-1.0f, min(1.0f, (result + m_bias) * m_factor));
}

void CRuFractal_HybridMultifractal::BuildExponentArray()
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

// ************************************************************************************************************************************************************

#pragma managed
