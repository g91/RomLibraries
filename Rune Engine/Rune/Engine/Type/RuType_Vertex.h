#ifndef _RUTYPEVERTEX_H_
#define _RUTYPEVERTEX_H_

#include "../../Core/Type/RuType_Base.h"

#pragma unmanaged

struct CRuVertex_Bare
{
	float x, y, z;
};

struct CRuVertex_Bare_Colored
{
	float x, y, z;
	DWORD diffuse;
};

struct CRuVertex_Static
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
};

struct CRuVertex_Static_Colored
{
	REAL x, y, z;
	BYTE nx, ny, nz, nw;
	DWORD color0;
	REAL u, v;
	UINT16 u1, v1;				// Half
};

struct CRuVertex_Static_ColoredOld
{
	float x, y, z;
	float nx, ny, nz;
	DWORD diffuse;
	float u, v;
};

struct CRuVertex_Static_Prelit
{
	float x, y, z;
	DWORD diffuse;
	float u, v;
};

struct CRuVertex_Static_NoTexture
{
	float x, y, z;
	float nx, ny, nz;
	DWORD diffuse;
	DWORD specular;
};

struct CRuVertex_Particle_Simple
{
	float x, y, z;
	DWORD diffuse;
};

struct CRuVertex_Particle_Grass_Optimized
{
	float x, y, z;
	DWORD diffuse;
	float w0, w1, w2, w3;
};

struct CRuVertex_Terrain_Packed
{
	BYTE x, z, yH, yL;
	DWORD diffuse;
};

struct CRuVertex_Skin
{
	REAL x, y, z;
	BYTE nx, ny, nz, nw;
	DWORD diffuse;
	BYTE i0, i1, i2, i3;
	BYTE w0, w1, w2, w3;
	INT16 u, v;
};

// *** DEPRECATED SHIT ***

struct SBareVertex
{
	float x, y, z;
	DWORD diffuse;
};

struct SLitSingleTextureVertex
{
	float x, y, z;
	DWORD diffuse;
	float u, v;
};

struct SUnlitSingleTextureVertex
{
	float x, y, z;
	float nx, ny, nz;
	DWORD diffuse;
	float u, v;
};

struct SUnlitDualTextureVertex
{
	float x, y, z;
	float nx, ny, nz;
	DWORD diffuse;
	float u, v;
	float u2, v2;
};

struct SUnlitSingle3DTextureVertex
{
	float x, y, z;
	float nx, ny, nz;
	DWORD diffuse;
	float u, v, w;
};

struct SVertexTerrain
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
};

struct SDeviceVertex
{
	float x, y, z, rhw;
	float u, v;
};

struct SDualTextureDeviceVertex
{
	float x, y, z, rhw;
	float u, v;
	float u2, v2;
};

struct SUnlitSingleTextureSkinnedVertex
{
	float x, y, z;
	float w0, w1, w2;
//	UINT8 i0, i1, i2, i3;
	float i0, i1, i2, i3;
	float nx, ny, nz;
	DWORD diffuse;
	float u, v;
};

struct SLitSingleTextureSkinnedVertex
{
	float x, y, z;
	float i0, i1, i2, i3;
	float w0, w1, w2, w3;
	DWORD diffuse;
	float u, v;
};

#pragma managed

#endif
