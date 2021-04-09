#ifndef _RUD3DTEXTUREUTILITY_H_
#define _RUD3DTEXTUREUTILITY_H_

// DirectX 9 includes
#include <d3d9.h>
#include <d3dx9.h>

// Aurora base types
#include "../../Core/Type/RuType_Base.h"
#include "../../Core/Math/RuVectorMath.h"

extern BOOL CopyLuminanceMapToTextureAlpha(UINT8 *lumMap, int width, int height, IDirect3DTexture9 *texture);

extern BOOL TextureCopyDXT1(IDirect3DTexture9 *srcTexture, IDirect3DTexture9 *destTexture, INT32 x, INT32 y, INT32 xEnd, INT32 yEnd);

extern BOOL TextureCopyDXT1(IDirect3DTexture9 *srcTexture, IDirect3DTexture9 *destTexture, INT32 x, INT32 y, INT32 xEnd, INT32 yEnd, DWORD modulationColor);
extern BOOL TextureCopyA8R8G8B8(IDirect3DTexture9 *srcTexture, IDirect3DTexture9 *destTexture, INT32 x, INT32 y, INT32 xEnd, INT32 yEnd, DWORD modulationColor);
extern BOOL TextureCopyA1R5G5B5(IDirect3DTexture9 *srcTexture, IDirect3DTexture9 *destTexture, INT32 x, INT32 y, INT32 xEnd, INT32 yEnd, DWORD modulationColor);
extern BOOL TextureCopyR5G6B5(IDirect3DTexture9 *srcTexture, IDirect3DTexture9 *destTexture, INT32 x, INT32 y, INT32 xEnd, INT32 yEnd, DWORD modulationColor);

extern BOOL TextureBlendDXT1(IDirect3DTexture9 *srcTexture, IDirect3DTexture9 *destTexture, INT32 x, INT32 y, INT32 xEnd, INT32 yEnd);
//extern BOOL TextureBlendDXT1(IDirect3DTexture9 *srcTexture0, IDirect3DTexture9 *srcTexture1, IDirect3DTexture9 *destTexture, INT32 x, INT32 y, INT32 xEnd, INT32 yEnd);

#endif
