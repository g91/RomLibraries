#ifndef _RUNOISE_WORLEY_
#define _RUNOISE_WORLEY_

// Base types
#include "../../Core/Noise/RuNoise_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

// D - dimensionality (1D, 2D, 3D or 4D)
//
template <unsigned int D>
struct CELLULARDATA
{
	REAL delta[4][D];	// vector from the sample point to the 'Nth' feature point
	REAL dist[4];		// magnitude of the above vector
	unsigned int ID[4];	// 32bit ID number for every feature point
};

// ************************************************************************************************************************************************************

class CRuNoise_Worley : public CRuNoise_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	BYTE											m_poissonCount[256];

	REAL											m_scale;
	INT32											m_seed;
	INT32											m_frequency;

public:
													CRuNoise_Worley();
													CRuNoise_Worley(UINT32 seed);

	virtual BOOL									SerializeFrom(IRuStream *stream);
	virtual BOOL									SerializeTo(IRuStream *stream);

	virtual REAL									Evaluate(REAL x, REAL y, REAL z);

	void											DefaultPoissonTable();
	void											ReshufflePoissonTable(UINT32 seed);

	void											ChangeCellDensity(BYTE from, BYTE to);

	REAL											GetScale();
	void											SetScale(REAL scale);

	INT32											GetSeed();
	void											SetSeed(INT32 seed);

	INT32											GetFrequency();
	void											SetFrequency(INT32 frequency);

private:
	INT32											Floor(REAL x) { return ((INT32)(x) - ((x) < 0 && (x) != (INT32)(x)));  }

	INT32											TileMod(INT32 index,	UINT32 frequency);
	REAL											TileModP(REAL value, UINT32 frequency);
	REAL											TileModM(REAL value, UINT32 frequency);

	void											Worley(REAL fx, UINT32 frequency, BOOL tile, CELLULARDATA<1> &cd);
	void											AddSamples(INT32 ix, REAL fx, CELLULARDATA<1> &cd);

	void											Worley(REAL fx, REAL fy, UINT32 frequencyX, UINT32 frequencyY, BOOL tileX, BOOL tileY, CELLULARDATA<2> &cd);
	void											AddSamples(INT32 ix, INT32 iy, REAL fx, REAL fy, CELLULARDATA<2> &cd);

	void											Worley(REAL fx, REAL fy, REAL fz, UINT32 frequencyX, UINT32 frequencyY, UINT32 frequencyZ, BOOL tileX, BOOL tileY, BOOL tileZ,  CELLULARDATA<3> &cd);
	void											AddSamples(INT32 ix, INT32 iy, INT32 iz, REAL fx, REAL fy, REAL fz, CELLULARDATA<3> &cd);

	void											Worley(REAL fx, REAL fy, REAL fz, REAL fw, UINT32 frequencyX, UINT32 frequencyY, UINT32 frequencyZ, UINT32 frequencyW, BOOL tileX, BOOL tileY, BOOL tileZ, BOOL tileW, CELLULARDATA<4> &cd);
	void											AddSamples(INT32 ix, INT32 iy, INT32 iz, INT32 iw, REAL fx, REAL fy, REAL fz, REAL fw, CELLULARDATA<4> &cd);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Noise_Worley, CRuNoise_Worley)

// ************************************************************************************************************************************************************

#pragma managed

#endif
