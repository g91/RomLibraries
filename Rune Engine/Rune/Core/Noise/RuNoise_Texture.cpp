#include "../../Core/Noise/RuNoise_Texture.h"

#include <math.h>

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuNoise_Texture, CRuNoise_Base)
ruHEAP_DEFINE_SUBCLASS(CRuNoise_Texture, "CRuNoise_Texture", "CRuNoise_Base")
ruCLASSGUID_DEFINE(CRuNoise_Texture, 0xF0EE5A93, 0x19774831, 0x88AA0C47, 0xF2E0772B)

// ************************************************************************************************************************************************************

CRuNoise_Texture::CRuNoise_Texture()
:	m_noiseTexture(NULL)
{
}

CRuNoise_Texture::~CRuNoise_Texture()
{
	delete[] m_noiseTexture;
}

void CRuNoise_Texture::SetNoiseTexture(INT32 width, INT32 height, INT32 stride, INT32 pitch, BYTE *texture)
{
	delete[] m_noiseTexture;

	m_noiseTextureWidth = width;
	m_noiseTextureHeight = height;
	m_noiseTexture = ruNEW BYTE [m_noiseTextureWidth * m_noiseTextureHeight];

	for(INT32 y = 0; y < height; ++y)
	{
		for(INT32 x = 0; x < width; ++x)
		{
			m_noiseTexture[x + y * width] = texture[x * stride];
		}

		texture = texture + pitch;
	}
}

BOOL CRuNoise_Texture::SerializeFrom(IRuStream *stream)
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

	// Read texture width
	stream->Read(&m_noiseTextureWidth, sizeof(INT32));

	// Read texture height
	stream->Read(&m_noiseTextureHeight, sizeof(INT32));

	// Read texture
	m_noiseTexture = ruNEW BYTE [m_noiseTextureWidth * m_noiseTextureHeight];
	stream->Read(m_noiseTexture, sizeof(BYTE) * m_noiseTextureWidth * m_noiseTextureHeight);

	return TRUE;
}

BOOL CRuNoise_Texture::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuNoise_Base::SerializeTo(stream);

	// Write texture width
	stream->Write(&m_noiseTextureWidth, sizeof(INT32));

	// Write texture height
	stream->Write(&m_noiseTextureHeight, sizeof(INT32));

	// Write texture
	stream->Write(m_noiseTexture, sizeof(BYTE) * m_noiseTextureWidth * m_noiseTextureHeight);

	return TRUE;
}

REAL CRuNoise_Texture::Evaluate(REAL x, REAL y, REAL z)
{
	if(m_noiseTexture)
	{
		INT32 iX = static_cast<INT32>(x) % m_noiseTextureWidth;
		INT32 iY = static_cast<INT32>(y) % m_noiseTextureHeight;

		if(iX < 0)
			iX += m_noiseTextureWidth;

		if(iY < 0)
			iY += m_noiseTextureWidth;

		return (m_noiseTexture[iX + iY * m_noiseTextureWidth] / 255.0f * 2.0f) - 1.0f;
	}

	return 0.0f;
}

// ************************************************************************************************************************************************************

#pragma managed
