#include "../../Core/Fractal/RuFractal_Multifractal.h"

#include <math.h>

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFractal_Multifractal, CRuFractal_Base)
ruHEAP_DEFINE_SUBCLASS(CRuFractal_Multifractal, "CRuFractal_Multifractal", "CRuFractal_Base")
ruCLASSGUID_DEFINE(CRuFractal_Multifractal, 0x0D5A3293, 0x0B094df4, 0x8AB119B6, 0x2C4C342D)

// ************************************************************************************************************************************************************

CRuFractal_Multifractal::CRuFractal_Multifractal()
:	m_H(1.0f),
	m_lacunarity(2.0f),
	m_octaves(4.0f),
	m_offset(1.0f),
	m_exponentArray(NULL)
{
}

CRuFractal_Multifractal::~CRuFractal_Multifractal()
{
	ruSAFE_RELEASE(m_noiseFunction);
	delete[] m_exponentArray;
}

REAL CRuFractal_Multifractal::GetH() const
{
	return m_H;
}

void CRuFractal_Multifractal::SetH(REAL H)
{
	m_H = H;

	// Compute and store spectral weights
	BuildExponentArray();
}

REAL CRuFractal_Multifractal::GetLacunarity() const
{
	return m_lacunarity;
}

void CRuFractal_Multifractal::SetLacunarity(REAL lacunarity)
{
	m_lacunarity = lacunarity;

	// Compute and store spectral weights
	BuildExponentArray();
}

REAL CRuFractal_Multifractal::GetOctaves() const
{
	return m_octaves;
}

void CRuFractal_Multifractal::SetOctaves(REAL octaves)
{
	m_octaves = octaves;

	// Compute and store spectral weights
	BuildExponentArray();
}

REAL CRuFractal_Multifractal::GetOffset() const
{
	return m_offset;
}

void CRuFractal_Multifractal::SetOffset(REAL offset)
{
	m_offset = offset;
}

BOOL CRuFractal_Multifractal::SerializeFrom(IRuStream *stream)
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

BOOL CRuFractal_Multifractal::SerializeTo(IRuStream *stream)
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

REAL CRuFractal_Multifractal::Evaluate(REAL x, REAL y, REAL z)
{
	INT32 i;
	REAL value, frequency, remainder;

	// Compute and store spectral weights
	BuildExponentArray();

	// Adjust input coordinates
	x = x * m_scale;
	y = y * m_scale;
	z = z * m_scale;

	value = 1.0f;            /* initialize vars to proper values */
	frequency = 1.0f;

	/* inner loop of multifractal construction */
	for(i = 0; i < m_octaves; i++)
	{
		value *= m_offset * frequency * m_noiseFunction->Evaluate(x, y, z);
		x *= m_lacunarity;
		y *= m_lacunarity;
		z *= m_lacunarity;
	}

	remainder = m_octaves - static_cast<INT32>(m_octaves);
	if(remainder != 0.0f)
	{
		/* add in ``octaves''  remainder */
		/* ``i''  and spatial freq. are preset in loop above */
		value += remainder * m_noiseFunction->Evaluate(x, y, z) * m_exponentArray[i];
	}

//	return (((value + 1.0f) / 2.0f) + m_bias) * m_factor;
//	return ((value + m_bias) * m_factor + 1.0f) / 2.0f;
	return (value + m_bias) * m_factor;
//	return max(-1.0f, min(1.0f, (value + m_bias) * m_factor));
}

void CRuFractal_Multifractal::BuildExponentArray()
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
