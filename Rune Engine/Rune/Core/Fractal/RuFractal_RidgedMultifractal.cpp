#include "../../Core/Fractal/RuFractal_RidgedMultifractal.h"

#include <math.h>

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFractal_RidgedMultifractal, CRuFractal_Base)
ruHEAP_DEFINE_SUBCLASS(CRuFractal_RidgedMultifractal, "CRuFractal_RidgedMultifractal", "CRuFractal_Base")
ruCLASSGUID_DEFINE(CRuFractal_RidgedMultifractal, 0x7B812DF8, 0x6231465d, 0x92892FF8, 0xC9E5BFCA)

// ************************************************************************************************************************************************************

CRuFractal_RidgedMultifractal::CRuFractal_RidgedMultifractal()
:	m_H(1.0f),
	m_lacunarity(2.0f),
	m_octaves(4.0f),
	m_offset(1.0f),
	m_gain(2.0f),
	m_exponentArray(NULL)
{
	m_scale = 0.005f;
}

CRuFractal_RidgedMultifractal::~CRuFractal_RidgedMultifractal()
{
	ruSAFE_RELEASE(m_noiseFunction);
	delete[] m_exponentArray;
}

REAL CRuFractal_RidgedMultifractal::GetH() const
{
	return m_H;
}

void CRuFractal_RidgedMultifractal::SetH(REAL H)
{
	m_H = H;

	// Compute and store spectral weights
	BuildExponentArray();
}

REAL CRuFractal_RidgedMultifractal::GetLacunarity() const
{
	return m_lacunarity;
}

void CRuFractal_RidgedMultifractal::SetLacunarity(REAL lacunarity)
{
	m_lacunarity = lacunarity;

	// Compute and store spectral weights
	BuildExponentArray();
}

REAL CRuFractal_RidgedMultifractal::GetOctaves() const
{
	return m_octaves;
}

void CRuFractal_RidgedMultifractal::SetOctaves(REAL octaves)
{
	m_octaves = octaves;

	// Compute and store spectral weights
	BuildExponentArray();
}

REAL CRuFractal_RidgedMultifractal::GetOffset() const
{
	return m_offset;
}

void CRuFractal_RidgedMultifractal::SetOffset(REAL offset)
{
	m_offset = offset;
}

REAL CRuFractal_RidgedMultifractal::GetGain() const
{
	return m_gain;
}

void CRuFractal_RidgedMultifractal::SetGain(REAL gain)
{
	m_gain = gain;
}

BOOL CRuFractal_RidgedMultifractal::SerializeFrom(IRuStream *stream)
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

	// Read gain
	stream->Read(&m_gain, sizeof(REAL));

	return TRUE;
}

BOOL CRuFractal_RidgedMultifractal::SerializeTo(IRuStream *stream)
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

	// Write gain
	stream->Write(&m_gain, sizeof(REAL));

	return TRUE;
}

REAL CRuFractal_RidgedMultifractal::Evaluate(REAL x, REAL y, REAL z)
{
	INT32 i;
	REAL result, signal, weight;

	// Compute and store spectral weights
	BuildExponentArray();

	// Adjust input coordinates
	x = x * m_scale;
	y = y * m_scale;
	z = z * m_scale;

	// get first octave
	signal = m_noiseFunction->Evaluate(x, y, z);

	// get absolute value of signal (this creates the ridges)
	if(signal < 0.0f)
		signal = -signal;

	// invert and translate (note that "offset" should be ~= 1.0)
	signal = m_offset - signal;

	// square the signal, to increase "sharpness" of ridges
	signal *= signal;

	// assign initial values
	result = signal;
	weight = 1.0f;

	for(i = 1; i < m_octaves; i++)
	{
		// increase the frequency
		x *= m_lacunarity;
		y *= m_lacunarity;
		z *= m_lacunarity;

		// weight successive contributions by previous signal
		weight = signal * m_gain;
		if(weight > 1.0f)
			weight = 1.0f;
		if(weight < 0.0f)
			weight = 0.0f;
		signal = m_noiseFunction->Evaluate(x, y, z);
		if(signal < 0.0f)
			signal = -signal;
		signal = m_offset - signal;
		signal *= signal;

		// weight the contribution
		signal *= weight;
		result += signal * m_exponentArray[i];
	}

	result = (result - 1.0f) / 2.0f;
//	return ((result + 1.0f) / 2.0f + m_bias) * m_factor;
//	return ((result + m_bias) * m_factor + 1.0f) / 2.0f;
	return (result + m_bias) * m_factor;
//	return max(-1.0f, min(1.0f, (result + m_bias) * m_factor));
}

void CRuFractal_RidgedMultifractal::BuildExponentArray()
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
