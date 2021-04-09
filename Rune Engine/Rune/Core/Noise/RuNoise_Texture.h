#ifndef _RUNOISE_TEXTURE_
#define _RUNOISE_TEXTURE_

// Base types
#include "../../Core/Noise/RuNoise_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuNoise_Texture : public CRuNoise_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32											m_noiseTextureWidth;
	INT32											m_noiseTextureHeight;
	BYTE*											m_noiseTexture;

public:
													CRuNoise_Texture();
	virtual											~CRuNoise_Texture();

	void											SetNoiseTexture(INT32 width, INT32 height, INT32 stride, INT32 pitch, BYTE *texture);

	virtual BOOL									SerializeFrom(IRuStream *stream);
	virtual BOOL									SerializeTo(IRuStream *stream);

	virtual REAL									Evaluate(REAL x, REAL y, REAL z);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Noise_Texture, CRuNoise_Texture)

// ************************************************************************************************************************************************************

#pragma managed

#endif
