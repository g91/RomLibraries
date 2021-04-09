#include "../../Core/Fractal/RuFractal_fBm.h"

#include <math.h>

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFractal_fBm, CRuFractal_Base)
ruHEAP_DEFINE_SUBCLASS(CRuFractal_fBm, "CRuFractal_fBm", "CRuFractal_Base")
ruCLASSGUID_DEFINE(CRuFractal_fBm, 0xC6A63F6A, 0x8B534e7a, 0x85648C31, 0x8FA60CE6)

// ************************************************************************************************************************************************************

CRuFractal_fBm::CRuFractal_fBm()
:	m_H(1.0f),
	m_lacunarity(2.0f),
	m_octaves(4.0f),
	m_exponentArray(NULL)
{
	m_scale = 0.01f;
}

CRuFractal_fBm::~CRuFractal_fBm()
{
	ruSAFE_RELEASE(m_noiseFunction);
	delete[] m_exponentArray;
}

REAL CRuFractal_fBm::GetH() const
{
	return m_H;
}

void CRuFractal_fBm::SetH(REAL H)
{
	m_H = H;

	// Compute and store spectral weights
	BuildExponentArray();
}

REAL CRuFractal_fBm::GetLacunarity() const
{
	return m_lacunarity;
}

void CRuFractal_fBm::SetLacunarity(REAL lacunarity)
{
	m_lacunarity = lacunarity;

	// Compute and store spectral weights
	BuildExponentArray();
}

REAL CRuFractal_fBm::GetOctaves() const
{
	return m_octaves;
}

void CRuFractal_fBm::SetOctaves(REAL octaves)
{
	m_octaves = octaves;

	// Compute and store spectral weights
	BuildExponentArray();
}

BOOL CRuFractal_fBm::SerializeFrom(IRuStream *stream)
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

	return TRUE;
}

BOOL CRuFractal_fBm::SerializeTo(IRuStream *stream)
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

	return TRUE;
}

REAL CRuFractal_fBm::Evaluate(REAL x, REAL y, REAL z)
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
		value += m_noiseFunction->Evaluate(x, y, z) * m_exponentArray[i];

		x *= m_lacunarity;
		y *= m_lacunarity;
		z *= m_lacunarity;
	}

	// Add in ``m_octaves''  remainder
	// ``i''  and spatial freq. are preset in loop above
	remainder = m_octaves - static_cast<INT32>(m_octaves);
	if(remainder != 0.0f)
	{
		value += remainder * m_noiseFunction->Evaluate(x, y, z) * m_exponentArray[i];
	}

//	return (((value + 1.0f) / 2.0f) + m_bias) * m_factor;
//	return ((value + m_bias) * m_factor + 1.0f) / 2.0f;
	return (value + m_bias) * m_factor;
//	return max(-1.0f, min(1.0f, (value + m_bias) * m_factor));
}

void CRuFractal_fBm::BuildExponentArray()
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
