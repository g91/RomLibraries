#include "../../Core/Noise/RuNoise_Value.h"

#include <math.h>

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuNoise_Value, CRuNoise_Base)
ruHEAP_DEFINE_SUBCLASS(CRuNoise_Value, "CRuNoise_Value", "CRuNoise_Base")
ruCLASSGUID_DEFINE(CRuNoise_Value, 0xAA0AC2E5, 0xA4614483, 0xA3D274E3, 0xBCB3CA78)

// ************************************************************************************************************************************************************

CRuNoise_Value::CRuNoise_Value()
:	m_valueTable(NULL),
	m_permutationTable(NULL)
{
	m_splineBasis.m_elements[0][0] = -0.5f;
	m_splineBasis.m_elements[0][1] =  1.5f;
	m_splineBasis.m_elements[0][2] = -1.5f;
	m_splineBasis.m_elements[0][3] =  0.5f;
	m_splineBasis.m_elements[1][0] =  1.0f;
	m_splineBasis.m_elements[1][1] = -2.5f;
	m_splineBasis.m_elements[1][2] =  2.0f;
	m_splineBasis.m_elements[1][3] = -0.5f;
	m_splineBasis.m_elements[2][0] = -0.5f;
	m_splineBasis.m_elements[2][1] =  0.0f;
	m_splineBasis.m_elements[2][2] =  0.5f;
	m_splineBasis.m_elements[2][3] =  0.0f;
	m_splineBasis.m_elements[3][0] =  0.0f;
	m_splineBasis.m_elements[3][1] =  1.0f;
	m_splineBasis.m_elements[3][2] =  0.0f;
	m_splineBasis.m_elements[3][3] =  0.0f;

	InitializeValueTable(256, 762541);
}

CRuNoise_Value::~CRuNoise_Value()
{
	delete[] m_valueTable;
	delete[] m_permutationTable;
}

BOOL CRuNoise_Value::SerializeFrom(IRuStream *stream)
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

	return TRUE;
}

BOOL CRuNoise_Value::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuNoise_Base::SerializeTo(stream);

	return TRUE;
}

REAL CRuNoise_Value::Evaluate(REAL x, REAL y, REAL z)
{
	INT32 ix, iy, iz;
	INT32 i, j, k;
	REAL fx, fy, fz;
	REAL xknots[4], yknots[4], zknots[4];

	ix = static_cast<INT32>(floor(x));
	fx = x - ix;

	iy = static_cast<INT32>(floor(y));
	fy = y - iy;

	iz = static_cast<INT32>(floor(z));
	fz = z - iz;

	for(k = -1; k <= 2; k++)
	{
		for(j = -1; j <= 2; j++)
		{
			for(i = -1; i <= 2; i++)
			{
				xknots[i + 1] = SampleLattice(ix + i, iy + j,iz + k);
			}

			yknots[j + 1] = Spline(fx, 4, xknots);
		}

		zknots[k + 1] = Spline(fy, 4, yknots);
	}

	return Spline(fz, 4, zknots);
}

void CRuNoise_Value::InitializeValueTable(INT32 valueTableSize, INT32 seed)
{
	// Allocate value table
	m_valueTableSize = valueTableSize;
	m_valueTable = ruNEW REAL [m_valueTableSize];
	m_permutationTable = ruNEW INT32 [m_valueTableSize];

	// Set generator seed
	m_mtRand.SetSeed(seed);

	// Populate value table
	REAL *table =m_valueTable;
	for(INT32 i = 0; i < m_valueTableSize; ++i)
	{
		*table++ = 1.0f - 2.0f * m_mtRand.GenerateFloat();
	}

	// Create identity permutation (0, 1, ..., m_valueTableSize - 1)
	INT32 i;
	for(i = 0; i < m_valueTableSize; i++)
	{
		m_permutationTable[i] = i;
	}

	// Generate random permutation
	for(INT32 i = m_valueTableSize - 1; i > 0; --i)
	{
		int j = (m_mtRand.GenerateINT32() & 0x7FFFFFFF) % (i + 1);
		if(j != i)
		{
			INT32 temp = m_permutationTable[i];
			m_permutationTable[i] = m_permutationTable[j];
			m_permutationTable[j] = temp;
		}
	}
}

INT32 CRuNoise_Value::Permute(INT32 x)
{
	return m_permutationTable[x & (m_valueTableSize - 1)];
}

INT32 CRuNoise_Value::Index(INT32 x, INT32 y, INT32 z)
{
	return Permute(x + Permute(y + Permute(z)));
}

REAL CRuNoise_Value::SampleLattice(INT32 x, INT32 y, INT32 z)
{
	return m_valueTable[Index(x, y, z)];
}

REAL CRuNoise_Value::Clamp(REAL x, REAL a, REAL b)
{
	return (x < a ? a : (x > b ? b : x));
}

REAL CRuNoise_Value::Spline(REAL x, INT32 nKnots, REAL *knot)
{
	INT32 span;
	INT32 nspans = nKnots - 3;
	REAL c0, c1, c2, c3;	/* coefficients of the cubic.*/

	if(nspans < 1)
	{  /* illegal */
//		fprintf(stderr, "Spline has too few knots.\n");
		return 0.0f;
	}

	// Find the appropriate 4-point span of the spline
	x = Clamp(x, 0.0f, 1.0f) * nspans;

	span = static_cast<INT32>(x);
	if(span >= nKnots - 3)
	{
		span = nKnots - 3;
	}

	x -= span;
	knot += span;

	// Evaluate the span cubic at x using Horner's rule
	c3 = m_splineBasis.m_elements[0][0] * knot[0] + m_splineBasis.m_elements[0][1] * knot[1] + m_splineBasis.m_elements[0][2] * knot[2] + m_splineBasis.m_elements[0][3] * knot[3];
	c2 = m_splineBasis.m_elements[1][0] * knot[0] + m_splineBasis.m_elements[1][1] * knot[1] + m_splineBasis.m_elements[1][2] * knot[2] + m_splineBasis.m_elements[1][3] * knot[3];
	c1 = m_splineBasis.m_elements[2][0] * knot[0] + m_splineBasis.m_elements[2][1] * knot[1] + m_splineBasis.m_elements[2][2] * knot[2] + m_splineBasis.m_elements[2][3] * knot[3];
	c0 = m_splineBasis.m_elements[3][0] * knot[0] + m_splineBasis.m_elements[3][1] * knot[1] + m_splineBasis.m_elements[3][2] * knot[2] + m_splineBasis.m_elements[3][3] * knot[3];

	return ((c3 * x + c2) * x + c1) * x + c0;
}

// ************************************************************************************************************************************************************

#pragma managed
