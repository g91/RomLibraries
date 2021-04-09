#include "../../Core/Fractal/RuFractal_HeteroTerrain.h"

#include <math.h>

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFractal_HeteroTerrain, CRuFractal_Base)
ruHEAP_DEFINE_SUBCLASS(CRuFractal_HeteroTerrain, "CRuFractal_HeteroTerrain", "CRuFractal_Base")
ruCLASSGUID_DEFINE(CRuFractal_HeteroTerrain, 0xDAEE42C2, 0x55E2451e, 0x94DB157F, 0x9263B0BE)

// ************************************************************************************************************************************************************

CRuFractal_HeteroTerrain::CRuFractal_HeteroTerrain()
:	m_H(0.5f),
	m_lacunarity(2.0f),
	m_octaves(4.0f),
	m_offset(0.0f),
	m_exponentArray(NULL)
{
	m_scale = 0.01f;
}

CRuFractal_HeteroTerrain::~CRuFractal_HeteroTerrain()
{
	ruSAFE_RELEASE(m_noiseFunction);
	delete[] m_exponentArray;
}

REAL CRuFractal_HeteroTerrain::GetH() const
{
	return m_H;
}

void CRuFractal_HeteroTerrain::SetH(REAL H)
{
	m_H = H;

	// Compute and store spectral weights
	BuildExponentArray();
}

REAL CRuFractal_HeteroTerrain::GetLacunarity() const
{
	return m_lacunarity;
}

void CRuFractal_HeteroTerrain::SetLacunarity(REAL lacunarity)
{
	m_lacunarity = lacunarity;

	// Compute and store spectral weights
	BuildExponentArray();
}

REAL CRuFractal_HeteroTerrain::GetOctaves() const
{
	return m_octaves;
}

void CRuFractal_HeteroTerrain::SetOctaves(REAL octaves)
{
	m_octaves = octaves;

	// Compute and store spectral weights
	BuildExponentArray();
}

REAL CRuFractal_HeteroTerrain::GetOffset() const
{
	return m_offset;
}

void CRuFractal_HeteroTerrain::SetOffset(REAL offset)
{
	m_offset = offset;
}

BOOL CRuFractal_HeteroTerrain::SerializeFrom(IRuStream *stream)
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

BOOL CRuFractal_HeteroTerrain::SerializeTo(IRuStream *stream)
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

REAL CRuFractal_HeteroTerrain::Evaluate(REAL x, REAL y, REAL z)
{
	INT32 i;
	REAL value, increment, remainder;

	// Compute and store spectral weights
	BuildExponentArray();

	// Adjust input coordinates
	x = x * m_scale;
	y = y * m_scale;
	z = z * m_scale;

	// first unscaled octave of function; later octaves are scaled
	value = m_offset + m_noiseFunction->Evaluate(x, y, z);
	x *= m_lacunarity;
	y *= m_lacunarity;
	z *= m_lacunarity;

	// spectral construction inner loop, where the fractal is built
	for (i=1; i < m_octaves; i++)
	{
		// obtain displaced noise value
		increment = m_noiseFunction->Evaluate(x, y, z) + m_offset;

		// scale amplitude appropriately for this frequency
		increment *= m_exponentArray[i];

		// scale increment by current `altitude' of function
		increment *= value;

		// add increment to ``value'' 
		value += increment;

		// raise spatial frequency
		x *= m_lacunarity;
		y *= m_lacunarity;
		z *= m_lacunarity;
	}

	// take care of remainder in ``octaves'' 
	remainder = m_octaves - static_cast<INT32>(m_octaves);
	if(remainder != 0.0f)
	{
		// ``i''  and spatial freq. are preset in loop above
		// note that the main loop code is made shorter here
		// you may want to that loop more like this
		increment = (m_noiseFunction->Evaluate(x, y, z) + m_offset) * m_exponentArray[i];
		value += remainder * increment * value;
	}

//	return (((value + 1.0f) / 2.0f) + m_bias) * m_factor;
//	return ((value + m_bias) * m_factor + 1.0f) / 2.0f;
	return (value + m_bias) * m_factor;
//	return max(-1.0f, min(1.0f, (value + m_bias) * m_factor));
}

void CRuFractal_HeteroTerrain::BuildExponentArray()
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
