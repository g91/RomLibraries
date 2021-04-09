#include "../../Core/Noise/RuNoise_Worley.h"

#include <math.h>
#include <float.h>

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuNoise_Worley, CRuNoise_Base)
ruHEAP_DEFINE_SUBCLASS(CRuNoise_Worley, "CRuNoise_Worley", "CRuNoise_Base")
ruCLASSGUID_DEFINE(CRuNoise_Worley, 0x8BF910DF, 0x83794422, 0x9A0C9DE1, 0x9EC6B422)

// ************************************************************************************************************************************************************

#define	CELLULAR_MULTIPLIER		(1.0f / 4294967296.0f)	// 1.0 / 2^32	(1.0 / MAX_UINT)

// ************************************************************************************************************************************************************

static const unsigned char poissonCount[256] =
{
	4, 3, 1, 1, 1, 2, 4, 2, 2, 2, 5, 1, 0, 2, 1, 2, 2, 0, 4, 3, 2, 1, 2, 1, 3, 2, 2, 4, 2, 2, 5, 1, 2, 3, 2, 2, 2, 2, 2, 3,
	2, 4, 2, 5, 3, 2, 2, 2, 5, 3, 3, 5, 2, 1, 3, 3, 4, 4, 2, 3, 0, 4, 2, 2, 2, 1, 3, 2, 2, 2, 3, 3, 3, 1, 2, 0, 2, 1, 1, 2,
	2, 2, 2, 5, 3, 2, 3, 2, 3, 2, 2, 1, 0, 2, 1, 1, 2, 1, 2, 2, 1, 3, 4, 2, 2, 2, 5, 4, 2, 4, 2, 2, 5, 4, 3, 2, 2, 5, 4, 3,
	3, 3, 5, 2, 2, 2, 2, 2, 3, 1, 1, 4, 2, 1, 3, 3, 4, 3, 2, 4, 3, 3, 3, 4, 5, 1, 4, 2, 4, 3, 1, 2, 3, 5, 3, 2, 1, 3, 1, 3,
	3, 3, 2, 3, 1, 5, 5, 4, 2, 2, 4, 1, 3, 4, 1, 5, 3, 3, 5, 3, 4, 3, 2, 2, 1, 1, 1, 1, 1, 2, 4, 5, 4, 5, 4, 2, 1, 5, 1, 1,
	2, 3, 3, 3, 2, 5, 2, 3, 3, 2, 0, 2, 1, 1, 4, 2, 1, 3, 2, 1, 2, 2, 3, 2, 5, 5, 3, 4, 5, 5, 2, 4, 4, 5, 3, 2, 2, 2, 1, 4,
	2, 3, 3, 4, 2, 5, 4, 2, 4, 2, 2, 2, 4, 5, 3, 2
};

// ************************************************************************************************************************************************************

CRuNoise_Worley::CRuNoise_Worley()
:	m_scale(1.0f),
	m_seed(0),
	m_frequency(32)
{
	DefaultPoissonTable();
}

CRuNoise_Worley::CRuNoise_Worley(UINT32 seed)
:	m_scale(1.0f),
	m_seed(0),
	m_frequency(1)
{
	UINT32 modseed = (1402024253 * seed + 586950981 * seed) >> 24;	// 0 ?255

	for(UINT32 i = 0; i < 256; ++i)
		m_poissonCount[i] = poissonCount[(modseed + i) & 255];
}

BOOL CRuNoise_Worley::SerializeFrom(IRuStream *stream)
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

	// Read scale
	stream->Read(&m_scale, sizeof(REAL));

	// Read seed
	stream->Read(&m_seed, sizeof(INT32));

	// Read frequency
	stream->Read(&m_frequency, sizeof(INT32));

	// Setup
	ReshufflePoissonTable(m_seed);

	return TRUE;
}

BOOL CRuNoise_Worley::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuNoise_Base::SerializeTo(stream);

	// Write scale
	stream->Write(&m_scale, sizeof(REAL));

	// Write seed
	stream->Write(&m_seed, sizeof(INT32));

	// Write frequency
	stream->Write(&m_frequency, sizeof(INT32));

	return TRUE;
}

REAL CRuNoise_Worley::Evaluate(REAL x, REAL y, REAL z)
{
	CELLULARDATA<2> cd;
	Worley(x * m_scale, y * m_scale, m_frequency, m_frequency, FALSE, FALSE, cd);

	REAL bias = 0.0f;
	REAL gain = 1.0f;

	return max(-1.0f, min(1.0f, (cd.dist[0] + bias) * gain));
}

void CRuNoise_Worley::DefaultPoissonTable()
{
	for(UINT32 i = 0; i < 256; ++i)
		m_poissonCount[i] = poissonCount[i];
}

void CRuNoise_Worley::ReshufflePoissonTable(UINT32 seed)
{
	UINT32 modseed = seed;
	for(UINT32 i = 0; i < 256; ++i)
	{
		modseed = 723748125 * seed + 2488143859;
		const UINT32 indx = (modseed >> 12) % 256;

		const BYTE tmp = m_poissonCount[i];
		m_poissonCount[i]    = m_poissonCount[indx];
		m_poissonCount[indx] = tmp;
	}
}
//************************************************************************************************
// The Poisson table, given in the book, is optimized for speed / isotropy in 3 dimensions.
// Using it for 2d and 1d Worley noise introduces discontinuous artifacts for higher Fn.
//
// In 2d we have a testing area of 3x3 quads (instead of 3x3x3 cubes as in 3d).
// In some situations, this isn't enough to find the 3 or 4 closest points.
//
// Solutions:
//		- recompute the new Poisson table, with higher mean density of feature points
//		- test a wider area, such as 5x5 quads, or 7x7 quads etc.
//		- bias the given Poisson table against the low populations of feature points
//
//
// The code for the first solution is given on the Worley's web site.
// The second solution requires modifications in the AddSamples() methods.
// The last solution is the simplest one, so it will be used here.
//
// Basically, for the 2d Worley noise, all populations of 1 point will be increased to 2 points,
// and all populations of 0 points will be increased to 1 point.
// After some testing, it appears that this deals with discontinuous artifacts for F3 and F4.
//
//		UPDATE:	Only in Euclidean metric. Manhattan F4 still features artifacts.
//
// It is even worse in 1d, where only three line segments are tested. |---|---|---|
// Because we are calculating up to F4, we need to clamp all populations to a minimum of 2,
// otherwise we could end up with less than 4 feature points on those line segments.
//
//
// Note however that this modification of the Poisson table destroys an ideal Poisson 
// distribution and may introduce visible artifacts in the form of repeatable patterns, similar
// to those when we skip the table and simply use some constant population of feature points.
//
// Luckely, this doesn't seem to be the case, and the noise still looks more or less random.
// Nevertheless, for perfect results, the first two solutions (perfect Poisson table with higher
// mean density and/or wider area testing) should be preferred.
//
void CRuNoise_Worley::ChangeCellDensity(BYTE from, BYTE to)
{
	for(UINT32 i = 0; i < 256; ++i)
	{
		if(m_poissonCount[i] == from)
			m_poissonCount[i] = to;
	}
}

REAL CRuNoise_Worley::GetScale()
{
	return m_scale;
}

void CRuNoise_Worley::SetScale(REAL scale)
{
	m_scale = scale;
}

INT32 CRuNoise_Worley::GetSeed()
{
	return m_seed;
}

void CRuNoise_Worley::SetSeed(INT32 seed)
{
	m_seed = seed;
	ReshufflePoissonTable(m_seed);
}

INT32 CRuNoise_Worley::GetFrequency()
{
	return m_frequency;
}

void CRuNoise_Worley::SetFrequency(INT32 frequency)
{
	m_frequency = frequency;
}

INT32 CRuNoise_Worley::TileMod(const INT32 index, const UINT32 frequency)
{
	return (index < 0) ? abs(index) * (frequency - 1) % frequency : index % frequency;
}

REAL CRuNoise_Worley::TileModP(const REAL value, const UINT32 frequency)
{
	INT32 iv = Floor(value);
	UINT32 index = UINT32(TileMod(iv, frequency));

	REAL frc = value - iv;
	REAL ret = index + frc;

	return ((frequency - 1) == index) ? (ret - frequency) : ret;
}

REAL CRuNoise_Worley::TileModM(const REAL value, const UINT32 frequency)
{
	const INT32 iv = Floor(value);
	const UINT32 index = UINT32(TileMod(iv, frequency));

	const REAL frc = value - iv;
	const REAL ret = index + frc;

	return (0 == index) ? (ret + frequency) : ret;
}

void CRuNoise_Worley::Worley(REAL fx, UINT32 frequency, BOOL tile, CELLULARDATA<1> &cd)
{
	cd.dist[0] = FLT_MAX;
	cd.dist[1] = FLT_MAX;
	cd.dist[2] = FLT_MAX;
	cd.dist[3] = FLT_MAX;

	// starting point of the line segment
	INT32 ix = Floor(fx);

	// central line segment test for closest point(s)
	AddSamples(ix, fx, cd);

	// distance between the sample point and the line's left point 
	// (result is in the 0.0 ?1.0 range)
	REAL cx1 = fx - ix;

	// distance between the sample point and the line's right point
	REAL cx2 = 1.0f - cx1;

	// squared distance to the left point
	cx1 *= cx1;

	// squared distance to the right point
	cx2 *= cx2;

	INT32 ixp = ix + 1;
	INT32 ixm = ix - 1;
	REAL fxp = fx;
	REAL fxm = fx;
	if(tile)
	{
		ixp = TileMod(ixp, frequency);
		ixm = TileMod(ixm, frequency);
		fxp = TileModP(fx, frequency);
		fxm = TileModM(fx, frequency);
	}

	// test 2 line neighbours with the most distant point
	if(cx1 < cd.dist[3])
		AddSamples(ixm, fxm, cd);
	if(cx2 < cd.dist[3])
		AddSamples(ixp, fxp, cd);

	// finally, convert the distance from squared to real distance
	cd.dist[0] = static_cast<REAL>(RuSqrt(cd.dist[0]));
	cd.dist[1] = static_cast<REAL>(RuSqrt(cd.dist[1]));
	cd.dist[2] = static_cast<REAL>(RuSqrt(cd.dist[2]));
	cd.dist[3] = static_cast<REAL>(RuSqrt(cd.dist[3]));
}

void CRuNoise_Worley::AddSamples(INT32 ix, REAL fx, CELLULARDATA<1> &cd)
{
	// RN seed for this line segment
	ULONG seed = 702395077 * ix;

	// the number of feature points in this line segment
	ULONG count = m_poissonCount[seed >> 24];	// index is from 0 to 255

	seed = 1402024253 * seed + 586950981;	// Knuth LCG

	for(ULONG j = 0; j < count; ++j)
	{
		const ULONG id = seed;

		// compute the coords of this feature point (in the 0.0 ?1.0 range)
		seed = 1402024253 * seed + 586950981;
		const REAL px = (seed + 0.5f) * CELLULAR_MULTIPLIER;
		seed = 1402024253 * seed + 586950981;

		// vector from sample location(fx) to feature point(ix+px)
		const REAL dx = (ix + px) - fx;

		// distance computation
		REAL dist2 = dx * dx;	// Euclidean distance, squared

		// is this point closer than the current most distant point?
		if(dist2 < cd.dist[3])
		{
			// insert the new point into the return data structure
			if(dist2 < cd.dist[0])
			{
				cd.delta[3][0] = cd.delta[2][0];
				cd.dist[3] = cd.dist[2];
				cd.ID[3] = cd.ID[2];

				cd.delta[2][0] = cd.delta[1][0];
				cd.dist[2] = cd.dist[1];
				cd.ID[2] = cd.ID[1];

				cd.delta[1][0] = cd.delta[0][0];
				cd.dist[1] = cd.dist[0];
				cd.ID[1] = cd.ID[0];

				cd.delta[0][0] = dx;
				cd.dist[0] = dist2;
				cd.ID[0] = id;
			}
			else if(dist2 < cd.dist[1])
			{
				cd.delta[3][0] = cd.delta[2][0];
				cd.dist[3] = cd.dist[2];
				cd.ID[3] = cd.ID[2];

				cd.delta[2][0] = cd.delta[1][0];
				cd.dist[2] = cd.dist[1];
				cd.ID[2] = cd.ID[1];

				cd.delta[1][0] = dx;
				cd.dist[1] = dist2;
				cd.ID[1] = id;
			}
			else if(dist2 < cd.dist[2])
			{
				cd.delta[3][0] = cd.delta[2][0];
				cd.dist[3] = cd.dist[2];
				cd.ID[3] = cd.ID[2];

				cd.delta[2][0] = dx;
				cd.dist[2] = dist2;
				cd.ID[2] = id;
			}
			else
			{
				cd.delta[3][0] = dx;
				cd.dist[3] = dist2;
				cd.ID[3] = id;
			}
		}
	}
}

void CRuNoise_Worley::Worley(REAL fx, REAL fy, UINT32 frequencyX, UINT32 frequencyY, BOOL tileX, BOOL tileY, CELLULARDATA<2> &cd)
{
	cd.dist[0] = FLT_MAX;
	cd.dist[1] = FLT_MAX;
	cd.dist[2] = FLT_MAX;
	cd.dist[3] = FLT_MAX;

	// starting corner of the integer bounding quad
	const INT32 ix = Floor(fx);
	const INT32 iy = Floor(fy);

	// central quad test for closest point(s)
	AddSamples(ix, iy, fx, fy, cd);

	// distance between the sample point and the quad's lower corner 
	// (result is in the 0.0 ?1.0 range)
	REAL cx1 = fx - ix;
	REAL cy1 = fy - iy;

	// distance between the sample point and the quad's upper corner
	REAL cx2 = 1.0f - cx1;
	REAL cy2 = 1.0f - cy1;

	// squared distance to the lower corner
	cx1 *= cx1;
	cy1 *= cy1;

	// squared distance to the upper corner
	cx2 *= cx2;
	cy2 *= cy2;

	INT32 ixp = ix + 1;
	INT32 ixm = ix - 1;
	REAL fxp = fx;
	REAL fxm = fx;
	if(tileX)
	{
		ixp = TileMod(ixp, frequencyX);
		ixm = TileMod(ixm, frequencyX);
		fxp = TileModP(fx, frequencyX);
		fxm = TileModM(fx, frequencyX);
	}

	INT32 iyp = iy + 1;
	INT32 iym = iy - 1;
	REAL fyp = fy;
	REAL fym = fy;
	if(tileY)
	{
		iyp = TileMod(iyp, frequencyY);
		iym = TileMod(iym, frequencyY);
		fyp = TileModP(fy, frequencyY);
		fym = TileModM(fy, frequencyY);
	}

	// test 4 'facing quad' neighbours
	if(cx1 < cd.dist[3]) AddSamples(ixm, iy,  fxm, fy,  cd);
	if(cy1 < cd.dist[3]) AddSamples(ix,  iym, fx,  fym, cd);
	if(cx2 < cd.dist[3]) AddSamples(ixp, iy,  fxp, fy , cd);
	if(cy2 < cd.dist[3]) AddSamples(ix,  iyp, fx,  fyp, cd);

	// test 4 'corner quad' neighbours
	if(cx1 + cy1 < cd.dist[3]) AddSamples(ixm, iym, fxm, fym, cd);
	if(cx2 + cy2 < cd.dist[3]) AddSamples(ixp, iyp, fxp, fyp, cd);
	if(cx1 + cy2 < cd.dist[3]) AddSamples(ixm, iyp, fxm, fyp, cd);
	if(cx2 + cy1 < cd.dist[3]) AddSamples(ixp, iym, fxp, fym, cd);

	// finally, convert the distances from squared to real distance
	cd.dist[0] = static_cast< REAL >(RuSqrt(cd.dist[0]));
	cd.dist[1] = static_cast< REAL >(RuSqrt(cd.dist[1]));
	cd.dist[2] = static_cast< REAL >(RuSqrt(cd.dist[2]));
	cd.dist[3] = static_cast< REAL >(RuSqrt(cd.dist[3]));
}

void CRuNoise_Worley::AddSamples(INT32 ix, INT32 iy, REAL fx, REAL fy, CELLULARDATA<2> &cd)
{
	// RN seed for this quad
	ULONG seed = 702395077 * ix + 915488749 * iy;

	// the number of feature points in this quad
	const ULONG count = m_poissonCount[ seed >> 24 ];	// index is from 0 to 255

	seed = 1402024253 * seed + 586950981;	// Knuth LCG

	for(ULONG j = 0; j < count; ++j)
	{
		const ULONG id = seed;

		// compute the coords of this feature point (in the 0.0 ?1.0 range)
		seed = 1402024253 * seed + 586950981;
		const REAL px = (seed + 0.5f) * CELLULAR_MULTIPLIER;
		seed = 1402024253 * seed + 586950981;
		const REAL py = (seed + 0.5f) * CELLULAR_MULTIPLIER;
		seed = 1402024253 * seed + 586950981;

		// vector from sample location(fx,fy) to feature point(ix+px, iy+py)
		const REAL dx = (ix + px) - fx;
		const REAL dy = (iy + py) - fy;

		// distance computation
		REAL dist2 = dx * dx + dy * dy;	// Euclidean distance, squared

		// is this point closer than the current most distant point?
		if(dist2 < cd.dist[3])
		{
			// insert the new point into the return data structure
			if(dist2 < cd.dist[0])
			{
				cd.delta[3][0] = cd.delta[2][0];
				cd.delta[3][1] = cd.delta[2][1];
				cd.dist[3] = cd.dist[2];
				cd.ID[3] = cd.ID[2];

				cd.delta[2][0] = cd.delta[1][0];
				cd.delta[2][1] = cd.delta[1][1];
				cd.dist[2] = cd.dist[1];
				cd.ID[2] = cd.ID[1];

				cd.delta[1][0] = cd.delta[0][0];
				cd.delta[1][1] = cd.delta[0][1];
				cd.dist[1] = cd.dist[0];
				cd.ID[1] = cd.ID[0];

				cd.delta[0][0] = dx;
				cd.delta[0][1] = dy;
				cd.dist[0] = dist2;
				cd.ID[0] = id;
			}
			else if(dist2 < cd.dist[1])
			{
				cd.delta[3][0] = cd.delta[2][0];
				cd.delta[3][1] = cd.delta[2][1];
				cd.dist[3] = cd.dist[2];
				cd.ID[3] = cd.ID[2];

				cd.delta[2][0] = cd.delta[1][0];
				cd.delta[2][1] = cd.delta[1][1];
				cd.dist[2] = cd.dist[1];
				cd.ID[2] = cd.ID[1];

				cd.delta[1][0] = dx;
				cd.delta[1][1] = dy;
				cd.dist[1] = dist2;
				cd.ID[1] = id;
			}
			else if(dist2 < cd.dist[2])
			{
				cd.delta[3][0] = cd.delta[2][0];
				cd.delta[3][1] = cd.delta[2][1];
				cd.dist[3] = cd.dist[2];
				cd.ID[3] = cd.ID[2];

				cd.delta[2][0] = dx;
				cd.delta[2][1] = dy;
				cd.dist[2] = dist2;
				cd.ID[2] = id;
			}
			else
			{
				cd.delta[3][0] = dx;
				cd.delta[3][1] = dy;
				cd.dist[3] = dist2;
				cd.ID[3] = id;
			}
		}
	}
}

void CRuNoise_Worley::Worley(REAL fx, REAL fy, REAL fz, UINT32 frequencyX, UINT32 frequencyY, UINT32 frequencyZ, BOOL tileX, BOOL tileY, BOOL tileZ,  CELLULARDATA<3> &cd)
{
	cd.dist[0] = FLT_MAX;
	cd.dist[1] = FLT_MAX;
	cd.dist[2] = FLT_MAX;
	cd.dist[3] = FLT_MAX;

	// starting corner of the INT32eger bounding cube
	const INT32 ix = Floor(fx);
	const INT32 iy = Floor(fy);
	const INT32 iz = Floor(fz);

	// central cube test for closest point(s)
	AddSamples(ix, iy, iz, fx, fy, fz, cd);

	// distance between the sample point and the cube's lower corner 
	// (result is in the 0.0 ?1.0 range)
	REAL cx1 = fx - ix;
	REAL cy1 = fy - iy;
	REAL cz1 = fz - iz;

	// distance between the sample point and the cube's upper corner
	REAL cx2 = 1.0f - cx1;
	REAL cy2 = 1.0f - cy1;
	REAL cz2 = 1.0f - cz1;

	// squared distance to the lower corner
	cx1 *= cx1;
	cy1 *= cy1;
	cz1 *= cz1;

	// squared distance to the upper corner
	cx2 *= cx2;
	cy2 *= cy2;
	cz2 *= cz2;

	//--------------------------------------------------------------------------------------------
	// Squared distance from the modified sample point to the closest point of the neighbouring 
	// cube is compared to the current most distant(also squared) feature point. 
	// If the neighbouring cube is closer than the most distant feature point, it will be 
	// searched for the possible new, closer feature point(s).
	//--------------------------------------------------------------------------------------------

	INT32 ixp = ix + 1;
	INT32 ixm = ix - 1;
	REAL fxp = fx;
	REAL fxm = fx;
	if(tileX)
	{
		ixp = TileMod(ixp, frequencyX);
		ixm = TileMod(ixm, frequencyX);
		fxp = TileModP(fx, frequencyX);
		fxm = TileModM(fx, frequencyX);
	}

	INT32 iyp = iy + 1;
	INT32 iym = iy - 1;
	REAL fyp = fy;
	REAL fym = fy;
	if(tileY)
	{
		iyp = TileMod(iyp, frequencyY);
		iym = TileMod(iym, frequencyY);
		fyp = TileModP(fy, frequencyY);
		fym = TileModM(fy, frequencyY);
	}

	INT32 izp = iz + 1;
	INT32 izm = iz - 1;
	REAL fzp = fz;
	REAL fzm = fz;
	if(tileZ)
	{
		izp = TileMod(izp, frequencyZ);
		izm = TileMod(izm, frequencyZ);
		fzp = TileModP(fz, frequencyZ);
		fzm = TileModM(fz, frequencyZ);
	}

	// test 6 'facing cube' neighbours
	if(cx1 < cd.dist[3])	AddSamples(ixm, iy,  iz,  fxm, fy,  fz,  cd);
	if(cy1 < cd.dist[3])	AddSamples(ix,  iym, iz,  fx,  fym, fz,  cd);
	if(cz1 < cd.dist[3])	AddSamples(ix,  iy,  izm, fx,  fy,  fzm, cd);
	if(cx2 < cd.dist[3])	AddSamples(ixp, iy,  iz,  fxp, fy,  fz,  cd);
	if(cy2 < cd.dist[3])	AddSamples(ix,  iyp, iz,  fx,  fyp, fz,  cd);
	if(cz2 < cd.dist[3])	AddSamples(ix,  iy,  izp, fx,  fy,  fzp, cd);

	// test 12 'edge cube' neighbours
	if(cx1 + cy1 < cd.dist[3])	AddSamples(ixm, iym, iz,  fxm, fym, fz,  cd);
	if(cx1 + cz1 < cd.dist[3])	AddSamples(ixm, iy,  izm, fxm, fy,  fzm, cd);
	if(cy1 + cz1 < cd.dist[3])	AddSamples(ix,  iym, izm, fx,  fym, fzm, cd);
	if(cx2 + cy2 < cd.dist[3])	AddSamples(ixp, iyp, iz,  fxp, fyp, fz,  cd);
	if(cx2 + cz2 < cd.dist[3])	AddSamples(ixp, iy,  izp, fxp, fy,  fzp, cd);
	if(cy2 + cz2 < cd.dist[3])	AddSamples(ix,  iyp, izp, fx,  fyp, fzp, cd);
	if(cx1 + cy2 < cd.dist[3])	AddSamples(ixm, iyp, iz,  fxm, fyp, fz,  cd);
	if(cx1 + cz2 < cd.dist[3])	AddSamples(ixm, iy,  izp, fxm, fy,  fzp, cd);
	if(cy1 + cz2 < cd.dist[3])	AddSamples(ix,  iym, izp, fx,  fym, fzp, cd);
	if(cx2 + cy1 < cd.dist[3])	AddSamples(ixp, iym, iz,  fxp, fym, fz,  cd);
	if(cx2 + cz1 < cd.dist[3])	AddSamples(ixp, iy,  izm, fxp, fy,  fzm, cd);
	if(cy2 + cz1 < cd.dist[3])	AddSamples(ix,  iyp, izm, fx,  fyp, fzm, cd);

	// test 8 'corner cube' neighbours
	if(cx1 + cy1 + cz1 < cd.dist[3])	AddSamples(ixm, iym, izm, fxm, fym, fzm, cd);
	if(cx1 + cy1 + cz2 < cd.dist[3])	AddSamples(ixm, iym, izp, fxm, fym, fzp, cd);
	if(cx1 + cy2 + cz1 < cd.dist[3])	AddSamples(ixm, iyp, izm, fxm, fyp, fzm, cd);
	if(cx2 + cy1 + cz1 < cd.dist[3])	AddSamples(ixp, iym, izm, fxp, fym, fzm, cd);
	if(cx1 + cy2 + cz2 < cd.dist[3])	AddSamples(ixm, iyp, izp, fxm, fyp, fzp, cd);
	if(cx2 + cy1 + cz2 < cd.dist[3])	AddSamples(ixp, iym, izp, fxp, fym, fzp, cd);
	if(cx2 + cy2 + cz1 < cd.dist[3])	AddSamples(ixp, iyp, izm, fxp, fyp, fzm, cd);
	if(cx2 + cy2 + cz2 < cd.dist[3])	AddSamples(ixp, iyp, izp, fxp, fyp, fzp, cd);

	//------------------------------------------------------------
	// finally, convert the distance from squared to real distance
	//------------------------------------------------------------

	cd.dist[0] = static_cast< REAL >(RuSqrt(cd.dist[0]));
	cd.dist[1] = static_cast< REAL >(RuSqrt(cd.dist[1]));
	cd.dist[2] = static_cast< REAL >(RuSqrt(cd.dist[2]));
	cd.dist[3] = static_cast< REAL >(RuSqrt(cd.dist[3]));
}
//************************************************************************************************
void CRuNoise_Worley::AddSamples(INT32 ix, INT32 iy, INT32 iz, REAL fx, REAL fy, REAL fz, CELLULARDATA<3> &cd)
{
	// RN seed for this cube
	ULONG seed = 702395077 * ix + 915488749 * iy + 2120969693 * iz;

	// the number of feature points in this cube
	const ULONG count = m_poissonCount[ seed >> 24 ];	// index is from 0 to 255

	seed = 1402024253 * seed + 586950981;	// Knuth LCG

	for(ULONG j = 0; j < count; ++j)
	{
		const ULONG id = seed;

		// compute the coords of this feature point (in the 0.0 ?1.0 range)
		seed = 1402024253 * seed + 586950981;
		const REAL px = (seed + 0.5f) * CELLULAR_MULTIPLIER;
		seed = 1402024253 * seed + 586950981;
		const REAL py = (seed + 0.5f) * CELLULAR_MULTIPLIER;
		seed = 1402024253 * seed + 586950981;
		const REAL pz = (seed + 0.5f) * CELLULAR_MULTIPLIER;
		seed = 1402024253 * seed + 586950981;

		// vector from sample location(fx,fy,fz) to feature point(ix+px, iy+py, iz+pz)
		const REAL dx = (ix + px) - fx;
		const REAL dy = (iy + py) - fy;
		const REAL dz = (iz + pz) - fz;

		// distance computation
		REAL dist2 = dx * dx + dy * dy + dz * dz;	// Euclidean distance, squared

		// is this point closer than the current most distant point?
		if(dist2 < cd.dist[3])
		{
			// insert the new point into the return data structure
			if(dist2 < cd.dist[0])
			{
				cd.delta[3][0] = cd.delta[2][0];
				cd.delta[3][1] = cd.delta[2][1];
				cd.delta[3][2] = cd.delta[2][2];
				cd.dist[3] = cd.dist[2];
				cd.ID[3] = cd.ID[2];

				cd.delta[2][0] = cd.delta[1][0];
				cd.delta[2][1] = cd.delta[1][1];
				cd.delta[2][2] = cd.delta[1][2];
				cd.dist[2] = cd.dist[1];
				cd.ID[2] = cd.ID[1];

				cd.delta[1][0] = cd.delta[0][0];
				cd.delta[1][1] = cd.delta[0][1];
				cd.delta[1][2] = cd.delta[0][2];
				cd.dist[1] = cd.dist[0];
				cd.ID[1] = cd.ID[0];

				cd.delta[0][0] = dx;
				cd.delta[0][1] = dy;
				cd.delta[0][2] = dz;
				cd.dist[0] = dist2;
				cd.ID[0] = id;
			}
			else if(dist2 < cd.dist[1])
			{
				cd.delta[3][0] = cd.delta[2][0];
				cd.delta[3][1] = cd.delta[2][1];
				cd.delta[3][2] = cd.delta[2][2];
				cd.dist[3] = cd.dist[2];
				cd.ID[3] = cd.ID[2];

				cd.delta[2][0] = cd.delta[1][0];
				cd.delta[2][1] = cd.delta[1][1];
				cd.delta[2][2] = cd.delta[1][2];
				cd.dist[2] = cd.dist[1];
				cd.ID[2] = cd.ID[1];

				cd.delta[1][0] = dx;
				cd.delta[1][1] = dy;
				cd.delta[1][2] = dz;
				cd.dist[1] = dist2;
				cd.ID[1] = id;
			}
			else if(dist2 < cd.dist[2])
			{
				cd.delta[3][0] = cd.delta[2][0];
				cd.delta[3][1] = cd.delta[2][1];
				cd.delta[3][2] = cd.delta[2][2];
				cd.dist[3] = cd.dist[2];
				cd.ID[3] = cd.ID[2];

				cd.delta[2][0] = dx;
				cd.delta[2][1] = dy;
				cd.delta[2][2] = dz;
				cd.dist[2] = dist2;
				cd.ID[2] = id;
			}
			else
			{
				cd.delta[3][0] = dx;
				cd.delta[3][1] = dy;
				cd.delta[3][2] = dz;
				cd.dist[3] = dist2;
				cd.ID[3] = id;
			}
		}
	}
}

void CRuNoise_Worley::Worley(REAL fx, REAL fy, REAL fz, REAL fw, UINT32 frequencyX, UINT32 frequencyY, UINT32 frequencyZ, UINT32 frequencyW, BOOL tileX, BOOL tileY, BOOL tileZ, BOOL tileW, CELLULARDATA<4> &cd)
{
	cd.dist[0] = FLT_MAX;
	cd.dist[1] = FLT_MAX;
	cd.dist[2] = FLT_MAX;
	cd.dist[3] = FLT_MAX;

	// starting corner of the INT32eger bounding cube
	const INT32 ix = Floor(fx);
	const INT32 iy = Floor(fy);
	const INT32 iz = Floor(fz);
	const INT32 iw = Floor(fw);

	// central cube test for closest point(s)
	AddSamples(ix, iy, iz, iw, fx, fy, fz, fw, cd);

	// distance between the sample point and the cube's lower corner 
	// (result is in the 0.0 ?1.0 range)
	REAL cx1 = fx - ix;
	REAL cy1 = fy - iy;
	REAL cz1 = fz - iz;
	REAL cw1 = fw - iw;

	// distance between the sample point and the cube's upper corner
	REAL cx2 = 1.0f - cx1;
	REAL cy2 = 1.0f - cy1;
	REAL cz2 = 1.0f - cz1;
	REAL cw2 = 1.0f - cw1;

	// squared distance to the lower corner
	cx1 *= cx1;
	cy1 *= cy1;
	cz1 *= cz1;
	cw1 *= cw1;

	// squared distance to the upper corner
	cx2 *= cx2;
	cy2 *= cy2;
	cz2 *= cz2;
	cw2 *= cw2;

	INT32 ixp = ix + 1;
	INT32 ixm = ix - 1;
	REAL fxp = fx;
	REAL fxm = fx;
	if(tileX)
	{
		ixp = TileMod(ixp, frequencyX);
		ixm = TileMod(ixm, frequencyX);
		fxp = TileModP(fx, frequencyX);
		fxm = TileModM(fx, frequencyX);
	}

	INT32 iyp = iy + 1;
	INT32 iym = iy - 1;
	REAL fyp = fy;
	REAL fym = fy;
	if(tileY)
	{
		iyp = TileMod(iyp, frequencyY);
		iym = TileMod(iym, frequencyY);
		fyp = TileModP(fy, frequencyY);
		fym = TileModM(fy, frequencyY);
	}

	INT32 izp = iz + 1;
	INT32 izm = iz - 1;
	REAL fzp = fz;
	REAL fzm = fz;
	if(tileZ)
	{
		izp = TileMod(izp, frequencyZ);
		izm = TileMod(izm, frequencyZ);
		fzp = TileModP(fz, frequencyZ);
		fzm = TileModM(fz, frequencyZ);
	}

	INT32 iwp = iw + 1;
	INT32 iwm = iw - 1;
	REAL fwp = fw;
	REAL fwm = fw;
	if(tileW)
	{
		iwp = TileMod(iwp, frequencyW);
		iwm = TileMod(iwm, frequencyW);
		fwp = TileModP(fw, frequencyW);
		fwm = TileModM(fw, frequencyW);
	}

	//--------------------------------------------------------------------------------------------
	// for 'fw' / 'iw'
	//--------------------------------------------------------------------------------------------

	// test 6 'facing cube' neighbours
	if(cx1 < cd.dist[3])	AddSamples(ixm, iy,  iz,  iw, fxm, fy,  fz,  fw, cd);
	if(cy1 < cd.dist[3])	AddSamples(ix,  iym, iz,  iw, fx,  fym, fz,  fw, cd);
	if(cz1 < cd.dist[3])	AddSamples(ix,  iy,  izm, iw, fx,  fy,  fzm, fw, cd);
	if(cx2 < cd.dist[3])	AddSamples(ixp, iy,  iz,  iw, fxp, fy,  fz,  fw, cd);
	if(cy2 < cd.dist[3])	AddSamples(ix,  iyp, iz,  iw, fx,  fyp, fz,  fw, cd);
	if(cz2 < cd.dist[3])	AddSamples(ix,  iy,  izp, iw, fx,  fy,  fzp, fw, cd);

	// test 12 'edge cube' neighbours
	if(cx1 + cy1 < cd.dist[3])	AddSamples(ixm, iym, iz,  iw, fxm, fym, fz,  fw, cd);
	if(cx1 + cz1 < cd.dist[3])	AddSamples(ixm, iy,  izm, iw, fxm, fy,  fzm, fw, cd);
	if(cy1 + cz1 < cd.dist[3])	AddSamples(ix,  iym, izm, iw, fx,  fym, fzm, fw, cd);
	if(cx2 + cy2 < cd.dist[3])	AddSamples(ixp, iyp, iz,  iw, fxp, fyp, fz,  fw, cd);
	if(cx2 + cz2 < cd.dist[3])	AddSamples(ixp, iy,  izp, iw, fxp, fy,  fzp, fw, cd);
	if(cy2 + cz2 < cd.dist[3])	AddSamples(ix,  iyp, izp, iw, fx,  fyp, fzp, fw, cd);
	if(cx1 + cy2 < cd.dist[3])	AddSamples(ixm, iyp, iz,  iw, fxm, fyp, fz,  fw, cd);
	if(cx1 + cz2 < cd.dist[3])	AddSamples(ixm, iy,  izp, iw, fxm, fy,  fzp, fw, cd);
	if(cy1 + cz2 < cd.dist[3])	AddSamples(ix,  iym, izp, iw, fx,  fym, fzp, fw, cd);
	if(cx2 + cy1 < cd.dist[3])	AddSamples(ixp, iym, iz,  iw, fxp, fym, fz,  fw, cd);
	if(cx2 + cz1 < cd.dist[3])	AddSamples(ixp, iy,  izm, iw, fxp, fy,  fzm, fw, cd);
	if(cy2 + cz1 < cd.dist[3])	AddSamples(ix,  iyp, izm, iw, fx,  fyp, fzm, fw, cd);

	// test 8 'corner cube' neighbours
	if(cx1 + cy1 + cz1 < cd.dist[3])	AddSamples(ixm, iym, izm, iw, fxm, fym, fzm, fw, cd);
	if(cx1 + cy1 + cz2 < cd.dist[3])	AddSamples(ixm, iym, izp, iw, fxm, fym, fzp, fw, cd);
	if(cx1 + cy2 + cz1 < cd.dist[3])	AddSamples(ixm, iyp, izm, iw, fxm, fyp, fzm, fw, cd);
	if(cx2 + cy1 + cz1 < cd.dist[3])	AddSamples(ixp, iym, izm, iw, fxp, fym, fzm, fw, cd);
	if(cx1 + cy2 + cz2 < cd.dist[3])	AddSamples(ixm, iyp, izp, iw, fxm, fyp, fzp, fw, cd);
	if(cx2 + cy1 + cz2 < cd.dist[3])	AddSamples(ixp, iym, izp, iw, fxp, fym, fzp, fw, cd);
	if(cx2 + cy2 + cz1 < cd.dist[3])	AddSamples(ixp, iyp, izm, iw, fxp, fyp, fzm, fw, cd);
	if(cx2 + cy2 + cz2 < cd.dist[3])	AddSamples(ixp, iyp, izp, iw, fxp, fyp, fzp, fw, cd);

	//--------------------------------------------------------------------------------------------
	// for 'fw - 1' / 'iw - 1'
	//--------------------------------------------------------------------------------------------

	// central cube from the past frame
	if(cw1 < cd.dist[3])			AddSamples(ix, iy, iz, iwm, fx, fy, fz, fwm, cd);

	// test 6 'facing cube' neighbours
	if(cx1 + cw1 < cd.dist[3])	AddSamples(ixm, iy,  iz,  iwm, fxm, fy,  fz,  fwm, cd);
	if(cy1 + cw1 < cd.dist[3])	AddSamples(ix,  iym, iz,  iwm, fx,  fym, fz,  fwm, cd);
	if(cz1 + cw1 < cd.dist[3])	AddSamples(ix,  iy,  izm, iwm, fx,  fy,  fzm, fwm, cd);
	if(cx2 + cw1 < cd.dist[3])	AddSamples(ixp, iy,  iz,  iwm, fxp, fy,  fz,  fwm, cd);
	if(cy2 + cw1 < cd.dist[3])	AddSamples(ix,  iyp, iz,  iwm, fx,  fyp, fz,  fwm, cd);
	if(cz2 + cw1 < cd.dist[3])	AddSamples(ix,  iy,  izp, iwm, fx,  fy,  fzp, fwm, cd);

	// test 12 'edge cube' neighbours
	if(cx1 + cy1 + cw1 < cd.dist[3])	AddSamples(ixm, iym, iz,  iwm, fxm, fym, fz,  fwm, cd);
	if(cx1 + cz1 + cw1 < cd.dist[3])	AddSamples(ixm, iy,  izm, iwm, fxm, fy,  fzm, fwm, cd);
	if(cy1 + cz1 + cw1 < cd.dist[3])	AddSamples(ix,  iym, izm, iwm, fx,  fym, fzm, fwm, cd);
	if(cx2 + cy2 + cw1 < cd.dist[3])	AddSamples(ixp, iyp, iz,  iwm, fxp, fyp, fz,  fwm, cd);
	if(cx2 + cz2 + cw1 < cd.dist[3])	AddSamples(ixp, iy,  izp, iwm, fxp, fy,  fzp, fwm, cd);
	if(cy2 + cz2 + cw1 < cd.dist[3])	AddSamples(ix,  iyp, izp, iwm, fx,  fyp, fzp, fwm, cd);
	if(cx1 + cy2 + cw1 < cd.dist[3])	AddSamples(ixm, iyp, iz,  iwm, fxm, fyp, fz,  fwm, cd);
	if(cx1 + cz2 + cw1 < cd.dist[3])	AddSamples(ixm, iy,  izp, iwm, fxm, fy,  fzp, fwm, cd);
	if(cy1 + cz2 + cw1 < cd.dist[3])	AddSamples(ix,  iym, izp, iwm, fx,  fym, fzp, fwm, cd);
	if(cx2 + cy1 + cw1 < cd.dist[3])	AddSamples(ixp, iym, iz,  iwm, fxp, fym, fz,  fwm, cd);
	if(cx2 + cz1 + cw1 < cd.dist[3])	AddSamples(ixp, iy,  izm, iwm, fxp, fy,  fzm, fwm, cd);
	if(cy2 + cz1 + cw1 < cd.dist[3])	AddSamples(ix,  iyp, izm, iwm, fx,  fyp, fzm, fwm, cd);

	// test 8 'corner cube' neighbours
	if(cx1 + cy1 + cz1 + cw1 < cd.dist[3])	AddSamples(ixm, iym, izm, iwm, fxm, fym, fzm, fwm, cd);
	if(cx1 + cy1 + cz2 + cw1 < cd.dist[3])	AddSamples(ixm, iym, izp, iwm, fxm, fym, fzp, fwm, cd);
	if(cx1 + cy2 + cz1 + cw1 < cd.dist[3])	AddSamples(ixm, iyp, izm, iwm, fxm, fyp, fzm, fwm, cd);
	if(cx2 + cy1 + cz1 + cw1 < cd.dist[3])	AddSamples(ixp, iym, izm, iwm, fxp, fym, fzm, fwm, cd);
	if(cx1 + cy2 + cz2 + cw1 < cd.dist[3])	AddSamples(ixm, iyp, izp, iwm, fxm, fyp, fzp, fwm, cd);
	if(cx2 + cy1 + cz2 + cw1 < cd.dist[3])	AddSamples(ixp, iym, izp, iwm, fxp, fym, fzp, fwm, cd);
	if(cx2 + cy2 + cz1 + cw1 < cd.dist[3])	AddSamples(ixp, iyp, izm, iwm, fxp, fyp, fzm, fwm, cd);
	if(cx2 + cy2 + cz2 + cw1 < cd.dist[3])	AddSamples(ixp, iyp, izp, iwm, fxp, fyp, fzp, fwm, cd);

	//--------------------------------------------------------------------------------------------
	// for 'fw + 1' / 'iw + 1'
	//--------------------------------------------------------------------------------------------

	// central cube from the future frame
	if(cw2 < cd.dist[3])			AddSamples(ix, iy, iz, iwp, fx, fy, fz, fwp, cd);

	// test 6 'facing cube' neighbours
	if(cx1 + cw2 < cd.dist[3])	AddSamples(ixm, iy,  iz,  iwp, fxm, fy,  fz,  fwp, cd);
	if(cy1 + cw2 < cd.dist[3])	AddSamples(ix,  iym, iz,  iwp, fx,  fym, fz,  fwp, cd);
	if(cz1 + cw2 < cd.dist[3])	AddSamples(ix,  iy,  izm, iwp, fx,  fy,  fzm, fwp, cd);
	if(cx2 + cw2 < cd.dist[3])	AddSamples(ixp, iy,  iz,  iwp, fxp, fy,  fz,  fwp, cd);
	if(cy2 + cw2 < cd.dist[3])	AddSamples(ix,  iyp, iz,  iwp, fx,  fyp, fz,  fwp, cd);
	if(cz2 + cw2 < cd.dist[3])	AddSamples(ix,  iy,  izp, iwp, fx,  fy,  fzp, fwp, cd);

	// test 12 'edge cube' neighbours
	if(cx1 + cy1 + cw2 < cd.dist[3])	AddSamples(ixm, iym, iz,  iwp, fxm, fym, fz,  fwp, cd);
	if(cx1 + cz1 + cw2 < cd.dist[3])	AddSamples(ixm, iy,  izm, iwp, fxm, fy,  fzm, fwp, cd);
	if(cy1 + cz1 + cw2 < cd.dist[3])	AddSamples(ix,  iym, izm, iwp, fx,  fym, fzm, fwp, cd);
	if(cx2 + cy2 + cw2 < cd.dist[3])	AddSamples(ixp, iyp, iz,  iwp, fxp, fyp, fz,  fwp, cd);
	if(cx2 + cz2 + cw2 < cd.dist[3])	AddSamples(ixp, iy,  izp, iwp, fxp, fy,  fzp, fwp, cd);
	if(cy2 + cz2 + cw2 < cd.dist[3])	AddSamples(ix,  iyp, izp, iwp, fx,  fyp, fzp, fwp, cd);
	if(cx1 + cy2 + cw2 < cd.dist[3])	AddSamples(ixm, iyp, iz,  iwp, fxm, fyp, fz,  fwp, cd);
	if(cx1 + cz2 + cw2 < cd.dist[3])	AddSamples(ixm, iy,  izp, iwp, fxm, fy,  fzp, fwp, cd);
	if(cy1 + cz2 + cw2 < cd.dist[3])	AddSamples(ix,  iym, izp, iwp, fx,  fym, fzp, fwp, cd);
	if(cx2 + cy1 + cw2 < cd.dist[3])	AddSamples(ixp, iym, iz,  iwp, fxp, fym, fz,  fwp, cd);
	if(cx2 + cz1 + cw2 < cd.dist[3])	AddSamples(ixp, iy,  izm, iwp, fxp, fy,  fzm, fwp, cd);
	if(cy2 + cz1 + cw2 < cd.dist[3])	AddSamples(ix,  iyp, izm, iwp, fx,  fyp, fzm, fwp, cd);

	// test 8 'corner cube' neighbours
	if(cx1 + cy1 + cz1 + cw2 < cd.dist[3])	AddSamples(ixm, iym, izm, iwp, fxm, fym, fzm, fwp, cd);
	if(cx1 + cy1 + cz2 + cw2 < cd.dist[3])	AddSamples(ixm, iym, izp, iwp, fxm, fym, fzp, fwp, cd);
	if(cx1 + cy2 + cz1 + cw2 < cd.dist[3])	AddSamples(ixm, iyp, izm, iwp, fxm, fyp, fzm, fwp, cd);
	if(cx2 + cy1 + cz1 + cw2 < cd.dist[3])	AddSamples(ixp, iym, izm, iwp, fxp, fym, fzm, fwp, cd);
	if(cx1 + cy2 + cz2 + cw2 < cd.dist[3])	AddSamples(ixm, iyp, izp, iwp, fxm, fyp, fzp, fwp, cd);
	if(cx2 + cy1 + cz2 + cw2 < cd.dist[3])	AddSamples(ixp, iym, izp, iwp, fxp, fym, fzp, fwp, cd);
	if(cx2 + cy2 + cz1 + cw2 < cd.dist[3])	AddSamples(ixp, iyp, izm, iwp, fxp, fyp, fzm, fwp, cd);
	if(cx2 + cy2 + cz2 + cw2 < cd.dist[3])	AddSamples(ixp, iyp, izp, iwp, fxp, fyp, fzp, fwp, cd);

	//------------------------------------------------------------
	// finally, convert the distance from squared to real distance
	//------------------------------------------------------------

	cd.dist[0] = static_cast< REAL >(RuSqrt(cd.dist[0]));
	cd.dist[1] = static_cast< REAL >(RuSqrt(cd.dist[1]));
	cd.dist[2] = static_cast< REAL >(RuSqrt(cd.dist[2]));
	cd.dist[3] = static_cast< REAL >(RuSqrt(cd.dist[3]));
}
//************************************************************************************************
void CRuNoise_Worley::AddSamples(INT32 ix, INT32 iy, INT32 iz, INT32 iw, REAL fx, REAL fy, REAL fz, REAL fw, CELLULARDATA<4> &cd)
{
	// RN seed for this cube
	ULONG seed = 702395077 * ix + 915488749 * iy + 2120969693 * iz + 1376312589 * iw;

	// the number of feature points in this cube
	const ULONG count = m_poissonCount[ seed >> 24 ];	// index is from 0 to 255

	seed = 1402024253 * seed + 586950981;	// Knuth LCG

	for(ULONG j = 0; j < count; ++j)
	{
		const ULONG id = seed;

		// compute the coords of this feature point (in the 0.0 ?1.0 range)
		seed = 1402024253 * seed + 586950981;
		const REAL px = (seed + 0.5f) * CELLULAR_MULTIPLIER;
		seed = 1402024253 * seed + 586950981;
		const REAL py = (seed + 0.5f) * CELLULAR_MULTIPLIER;
		seed = 1402024253 * seed + 586950981;
		const REAL pz = (seed + 0.5f) * CELLULAR_MULTIPLIER;
		seed = 1402024253 * seed + 586950981;
		const REAL pw = (seed + 0.5f) * CELLULAR_MULTIPLIER;
		seed = 1402024253 * seed + 586950981;

		// vector from sample location(fx,fy,fz,fw) to feature point(ix+px, iy+py, iz+pz, iw+pw)
		const REAL dx = (ix + px) - fx;
		const REAL dy = (iy + py) - fy;
		const REAL dz = (iz + pz) - fz;
		const REAL dw = (iw + pw) - fw;

		// distance computation
		REAL dist2 = dx * dx + dy * dy + dz * dz + dw * dw;	// Euclidean distance, squared

		// is this point closer than the current most distant point?
		if(dist2 < cd.dist[3])
		{
			// insert the new point into the return data structure
			if(dist2 < cd.dist[0])
			{
				cd.delta[3][0] = cd.delta[2][0];
				cd.delta[3][1] = cd.delta[2][1];
				cd.delta[3][2] = cd.delta[2][2];
				cd.delta[3][3] = cd.delta[2][3];
				cd.dist[3] = cd.dist[2];
				cd.ID[3] = cd.ID[2];

				cd.delta[2][0] = cd.delta[1][0];
				cd.delta[2][1] = cd.delta[1][1];
				cd.delta[2][2] = cd.delta[1][2];
				cd.delta[2][3] = cd.delta[1][3];
				cd.dist[2] = cd.dist[1];
				cd.ID[2] = cd.ID[1];

				cd.delta[1][0] = cd.delta[0][0];
				cd.delta[1][1] = cd.delta[0][1];
				cd.delta[1][2] = cd.delta[0][2];
				cd.delta[1][3] = cd.delta[0][3];
				cd.dist[1] = cd.dist[0];
				cd.ID[1] = cd.ID[0];

				cd.delta[0][0] = dx;
				cd.delta[0][1] = dy;
				cd.delta[0][2] = dz;
				cd.delta[0][3] = dw;
				cd.dist[0] = dist2;
				cd.ID[0] = id;
			}
			else if(dist2 < cd.dist[1])
			{
				cd.delta[3][0] = cd.delta[2][0];
				cd.delta[3][1] = cd.delta[2][1];
				cd.delta[3][2] = cd.delta[2][2];
				cd.delta[3][3] = cd.delta[2][3];
				cd.dist[3] = cd.dist[2];
				cd.ID[3] = cd.ID[2];

				cd.delta[2][0] = cd.delta[1][0];
				cd.delta[2][1] = cd.delta[1][1];
				cd.delta[2][2] = cd.delta[1][2];
				cd.delta[2][3] = cd.delta[1][3];
				cd.dist[2] = cd.dist[1];
				cd.ID[2] = cd.ID[1];

				cd.delta[1][0] = dx;
				cd.delta[1][1] = dy;
				cd.delta[1][2] = dz;
				cd.delta[1][3] = dw;
				cd.dist[1] = dist2;
				cd.ID[1] = id;
			}
			else if(dist2 < cd.dist[2])
			{
				cd.delta[3][0] = cd.delta[2][0];
				cd.delta[3][1] = cd.delta[2][1];
				cd.delta[3][2] = cd.delta[2][2];
				cd.delta[3][3] = cd.delta[2][3];
				cd.dist[3] = cd.dist[2];
				cd.ID[3] = cd.ID[2];

				cd.delta[2][0] = dx;
				cd.delta[2][1] = dy;
				cd.delta[2][2] = dz;
				cd.delta[2][3] = dw;
				cd.dist[2] = dist2;
				cd.ID[2] = id;
			}
			else
			{
				cd.delta[3][0] = dx;
				cd.delta[3][1] = dy;
				cd.delta[3][2] = dz;
				cd.delta[3][3] = dw;
				cd.dist[3] = dist2;
				cd.ID[3] = id;
			}
		}
	}
}

// ************************************************************************************************************************************************************

#pragma managed
