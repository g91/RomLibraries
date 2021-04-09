/*!
	@file RuTextureUtility.h

	Copyright (c) 2004-2006 All rights reserved
*/

#ifndef _RUTEXTUREUTILITY_H_
#define _RUTEXTUREUTILITY_H_

#include "../../Engine/Base/RuEngine_Base.h"
#include "../../Engine/Base/RuMaterialBase.h"

#include <dxtlib/dxtlib.h>

#pragma managed(push, off)

// ************************************************************************************************************************************************************

struct RuPixel_A8R8G8B8
{
	union
	{
		struct
		{
			BYTE				m_b;
			BYTE				m_g;
			BYTE				m_r;
			BYTE				m_a;
		};

		DWORD					m_color;
	};
};

// ************************************************************************************************************************************************************

struct RuColorBlock_DXT1
{
	UINT16						m_color0;				// Low color
	UINT16						m_color1;				// High color
	UINT32						m_index;				// Index bit field
};

// ************************************************************************************************************************************************************

extern INT32					RuTextureUtility_GetMemoryFootprint(IRuBaseTexture *texture, CRuKeyHash<PTRVALUE> *enumHash);
extern BOOL						RuTextureUtility_Copy(IRuBaseTexture *srcTexture, IRuBaseTexture *dstTexture);

extern INT32					RuTextureUtility_GetMemoryFootprint(IRuTexture *texture, CRuKeyHash<PTRVALUE> *enumHash);
extern IRuSurface*				RuTextureUtility_GetLevelSurfaceBySize(IRuTexture *texture, INT32 width, INT32 height);
extern BOOL						RuTextureUtility_Blit(IRuSurface *srcSurface, IRuSurface *dstSurface, RuAlphaMode alphaMode);
extern BOOL						RuTextureUtility_Copy_IRuTexture(IRuTexture *srcTexture, IRuTexture *dstTexture);
extern BOOL						RuTextureUtility_Clear(IRuTexture *dstTexture);
extern BOOL						RuTextureUtility_SerializeFrom(IRuTexture *texture, INT32 level, IRuStream *stream);
extern BOOL						RuTextureUtility_SerializeTo(IRuTexture *texture, INT32 level, IRuStream *stream);

extern BOOL						RuTextureUtility_Copy_IRuVolumeTexture(IRuVolumeTexture *srcTexture, IRuVolumeTexture *dstTexture);
extern BOOL						RuTextureUtility_SerializeFrom(IRuVolumeTexture *texture, INT32 level, IRuStream *stream);
extern BOOL						RuTextureUtility_SerializeTo(IRuVolumeTexture *texture, INT32 level, IRuStream *stream);

extern BOOL						RuSurfaceUtility_Convert(IRuSurface *srcSurface, IRuSurface *dstSurface);
extern BOOL						RuSurfaceUtility_SerializeFrom(IRuSurface *surface, IRuStream *stream);
extern BOOL						RuSurfaceUtility_SerializeTo(IRuSurface *surface, IRuStream *stream);

// ************************************************************************************************************************************************************

class CRuDXTCompressor
{
protected:
	struct DXTnWriteStruct
	{
//		BYTE*								m_dstImageBits;
//		INT32								m_dstImagePitch;
		IRuTexture*							m_dstTexture;
		INT32								m_colPosition;
	};

	static CRuFileStream*					m_outputStream;

public:
											CRuDXTCompressor();
											~CRuDXTCompressor();

	static BOOL								SaveToDXT(granny_texture *srcTexture, const char *filename, BOOL premultipliedAlpha);
	static BOOL								SaveToDXT(RUCOLOR *pixelArray, INT32 width, INT32 height, const char *filename, BOOL premultipliedAlpha, BOOL useDXT5);
	static BOOL								ConvertToDXT(IRuTexture *srcTexture, IRuTexture *dstTexture);

protected:
	static INT32							WriteDXTnFile(const void *buffer, size_t count, const MIPMapData *mipMapData, void *userData);
	static INT32							WriteDXTnMemory(const void *buffer, size_t count, const MIPMapData *mipMapData, void *userData);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
