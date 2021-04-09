#include "../../Core/Noise/RuNoise_Perlin.h"

#include <math.h>

#pragma unmanaged

/*
float bias(float a, float b)
{
	return pow(a, log(b) / log(0.5));
}

float gain(float a, float b)
{
	float p = log(1. - b) / log(0.5);

	if (a < .001)
		return 0.;
	else if (a > .999)
		return 1.;
	if (a < 0.5)
		return pow(2 * a, p) / 2;
	else
		return 1. - pow(2 * (1. - a), p) / 2;
}
*/

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuNoise_Perlin, CRuNoise_Base)
ruHEAP_DEFINE_SUBCLASS(CRuNoise_Perlin, "CRuNoise_Perlin", "CRuNoise_Base")
ruCLASSGUID_DEFINE(CRuNoise_Perlin, 0x4415471D,0x9FA64fdd, 0xAEAA2C00, 0x5F188975)

// ************************************************************************************************************************************************************

CRuNoise_Perlin::CRuNoise_Perlin()
:	m_B(0x100),
	m_BMask(0xFF),
	m_N(0x1000),
	m_NMask(0xFFF),
	m_NP(12),
	m_permutationTable(NULL),
	m_g1(NULL),
	m_g2(NULL),
	m_g3(NULL)
{
	Initialize(0);
}

CRuNoise_Perlin::~CRuNoise_Perlin()
{
	Reset();
}

INT32 CRuNoise_Perlin::GetSeed()
{
	return m_seed;
}

void CRuNoise_Perlin::SetSeed(INT32 seed)
{
	m_seed = seed;

	Initialize(m_seed);
}

BOOL CRuNoise_Perlin::SerializeFrom(IRuStream *stream)
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
	CRuNoise_Base::SerializeFrom(stream);

	// Read seed
	stream->Read(&m_seed, sizeof(INT32));

	return TRUE;
}

BOOL CRuNoise_Perlin::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuNoise_Base::SerializeTo(stream);

	// Write seed
	stream->Write(&m_seed, sizeof(INT32));

	return TRUE;
}

REAL CRuNoise_Perlin::Evaluate(REAL x, REAL y, REAL z)
{
	REAL v[3] = { x, y, z };
	return Noise3(v);
}

void CRuNoise_Perlin::Normalize2(REAL *v)
{
	REAL s;

	s = RuSqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

void CRuNoise_Perlin::Normalize3(REAL *v)
{
	REAL s;

	s = RuSqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}

void CRuNoise_Perlin::Initialize(INT32 seed)
{
	// Initialize random number generator
	m_mtRand.SetSeed(seed);

	Reset();

	INT32 arrayLength = m_B + m_B + 2;
	m_permutationTable = ruNEW INT32 [arrayLength];
	m_g1 = ruNEW REAL [arrayLength];
	
	m_g2 = ruNEW REAL * [arrayLength];
	for(INT32 i = 0; i < arrayLength; ++i)
	{
		m_g2[i] = ruNEW REAL [2];
	}

	m_g3 = ruNEW REAL * [arrayLength];
	for(INT32 i = 0; i < arrayLength; ++i)
	{
		m_g3[i] = ruNEW REAL [3];
	}

	INT32 i, j, k;

	for(i = 0; i < m_B; i++)
	{
		m_permutationTable[i] = i;

		m_g1[i] = (REAL) ((m_mtRand.GenerateINT32() % (m_B + m_B)) - m_B) / m_B;

		for(j = 0; j < 2; j++)
		{
			m_g2[i][j] = (REAL) ((m_mtRand.GenerateINT32() % (m_B + m_B)) - m_B) / m_B;
		}

		Normalize2(m_g2[i]);

		for(j = 0; j < 3; j++)
		{
			m_g3[i][j] = (REAL) ((m_mtRand.GenerateINT32() % (m_B + m_B)) - m_B) / m_B;
		}

		Normalize3(m_g3[i]);
	}

	// Generate the permutation table
	while(--i)
	{
		j = m_mtRand.GenerateINT32() % m_B;
		k = m_permutationTable[i];
		m_permutationTable[i] = m_permutationTable[j];
		m_permutationTable[j] = k;
	}

	for(i = 0; i < m_B + 2; i++)
	{
		m_permutationTable[m_B + i] = m_permutationTable[i];
		m_g1[m_B + i] = m_g1[i];

		for(j = 0; j < 2; j++)
		{
			m_g2[m_B + i][j] = m_g2[i][j];
		}

		for(j = 0; j < 3; j++)
		{
			m_g3[m_B + i][j] = m_g3[i][j];
		}
	}
}

void CRuNoise_Perlin::Reset()
{
	INT32 arrayLength = m_B + m_B + 2;

	delete[] m_permutationTable;
	delete[] m_g1;

	if(m_g2)
	{
		for(INT32 i = 0; i < arrayLength; ++i)
		{
			delete[] m_g2[i];
		}

		delete[] m_g2;
	}

	if(m_g3)
	{
		for(INT32 i = 0; i < arrayLength; ++i)
		{
			delete[] m_g3[i];
		}

		delete[] m_g3;
	}

	m_permutationTable = NULL;
	m_g1 = NULL;
	m_g2 = NULL;
	m_g3 = NULL;
}

REAL CRuNoise_Perlin::Noise3(REAL *vec)
{
	INT32 bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	REAL rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	INT32 i, j;

	/* setup(0, bx0,bx1, rx0,rx1) */
	t = vec[0] + m_N;
	bx0 = ((int)t) & m_BMask;
	bx1 = (bx0+1) & m_BMask;
	rx0 = t - (int)t;
	rx1 = rx0 - 1.0f;
	/***/
	/* setup(1, by0,by1, ry0,ry1) */
	t = vec[1] + m_N;
	by0 = ((int)t) & m_BMask;
	by1 = (by0+1) & m_BMask;
	ry0 = t - (int)t;
	ry1 = ry0 - 1.0f;
	/***/
	/* setup(2, bz0,bz1, rz0,rz1) */
	t = vec[2] + m_N;
	bz0 = ((int)t) & m_BMask;
	bz1 = (bz0+1) & m_BMask;
	rz0 = t - (int)t;
	rz1 = rz0 - 1.0f;
	/***/

	i = m_permutationTable[ bx0 ];
	j = m_permutationTable[ bx1 ];

	b00 = m_permutationTable[ i + by0 ];
	b10 = m_permutationTable[ j + by0 ];
	b01 = m_permutationTable[ i + by1 ];
	b11 = m_permutationTable[ j + by1 ];

	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);

	q = m_g3[ b00 + bz0 ] ; u = ( rx0 * q[0] + ry0 * q[1] + rz0 * q[2] );
	q = m_g3[ b10 + bz0 ] ; v = ( rx1 * q[0] + ry0 * q[1] + rz0 * q[2] );
	a = lerp(t, u, v);

	q = m_g3[ b01 + bz0 ] ; u = ( rx0 * q[0] + ry1 * q[1] + rz0 * q[2] );
	q = m_g3[ b11 + bz0 ] ; v = ( rx1 * q[0] + ry1 * q[1] + rz0 * q[2] );
	b = lerp(t, u, v);

	c = lerp(sy, a, b);

	q = m_g3[ b00 + bz1 ] ; u = ( rx0 * q[0] + ry0 * q[1] + rz1 * q[2] );
	q = m_g3[ b10 + bz1 ] ; v = ( rx1 * q[0] + ry0 * q[1] + rz1 * q[2] );
	a = lerp(t, u, v);

	q = m_g3[ b01 + bz1 ] ; u = ( rx0 * q[0] + ry1 * q[1] + rz1 * q[2] );
	q = m_g3[ b11 + bz1 ] ; v = ( rx1 * q[0] + ry1 * q[1] + rz1 * q[2] );
	b = lerp(t, u, v);

	d = lerp(sy, a, b);

	return lerp(sz, c, d);
}

REAL CRuNoise_Perlin::s_curve(REAL t)
{
	return t * t * (3.0f - 2.0f * t);
}

REAL CRuNoise_Perlin::lerp(REAL t, REAL a, REAL b)
{
	return a + t * (b - a);
}


// ************************************************************************************************************************************************************

#pragma managed
