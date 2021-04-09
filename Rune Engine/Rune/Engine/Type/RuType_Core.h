/*!
	@file RuTypeCore.h

	Copyright (c) 2004 All rights reserved
*/

#include "../../Core/Type/RuType_Base.h"
#include "../../Core/Type/RuType_Object.h"
#include "../../Core/Utility/RuUtility.h"

#ifndef _RUTYPECORE_H_
#define _RUTYPECORE_H_

#pragma unmanaged
#pragma pack(4)

// ************************************************************************************************************************************************************

#define RUCOLOR_ARGB(a, r, g, b)	((RUCOLOR) ((((a) & 0xFF) << 24) | (((r) & 0xFF) << 16) | (((g) & 0xFF) << 8)|((b) & 0xFF)))
#define RUCOLOR_RGBA(r, g, b, a)	RUCOLOR_ARGB(a, r, g, b)
#define RUCOLOR_XRGB(r, g, b)		RUCOLOR_ARGB(0xFF, r, g, b)

// ************************************************************************************************************************************************************

#define RUCLEAR_TARGET							0x00000001L						// Clear target surface
#define RUCLEAR_ZBUFFER							0x00000002L						// Clear target z buffer
#define RUCLEAR_STENCIL							0x00000004L						// Clear stencil planes

// ************************************************************************************************************************************************************

// This mimics exactly DirectX MAKEFOURCC macro
#define RUMAKEFOURCC(ch0, ch1, ch2, ch3) ((DWORD) (BYTE) (ch0) | ((DWORD) (BYTE) (ch1) << 8) | ((DWORD) (BYTE) (ch2) << 16) | ((DWORD) (BYTE) (ch3) << 24 ))

// ************************************************************************************************************************************************************

/*!
	Enumeration of primitive types.
*/
enum RuPrimitiveType
{
	ruPRIMTYPE_TRIANGLELIST						= 0,							//!< Triangle list
	ruPRIMTYPE_TRIANGLESTRIP					= 1,							//!< Triangle strip
	ruPRIMTYPE_TRIANGLEFAN						= 2,							//!< Triangle fan
	ruPRIMTYPE_LINELIST							= 3,							//!< Line list
	ruPRIMTYPE_POINTLIST						= 4,							//!< Point list (never indexed)
	ruPRIMTYPE_FORCE_DWORD						= 0x7FFFFFFF
};

inline INT32 RuIndexToPrimitiveCount(INT32 indexCount, RuPrimitiveType primitiveType)
{
	switch(primitiveType)
	{
		default:
		case ruPRIMTYPE_TRIANGLELIST:
			return indexCount / 3;

		case ruPRIMTYPE_TRIANGLESTRIP:
			return indexCount - 2;

		case ruPRIMTYPE_TRIANGLEFAN:
			return indexCount - 2;

		case ruPRIMTYPE_LINELIST:
			return indexCount / 2;

		case ruPRIMTYPE_POINTLIST:
			return indexCount;
	}
}

inline INT32 RuPrimitiveToIndexCount(INT32 primitiveCount, RuPrimitiveType primitiveType)
{
	switch(primitiveType)
	{
		default:
		case ruPRIMTYPE_TRIANGLELIST:
			return primitiveCount * 3;

		case ruPRIMTYPE_TRIANGLESTRIP:
			return primitiveCount + 2;

		case ruPRIMTYPE_TRIANGLEFAN:
			return primitiveCount + 2;

		case ruPRIMTYPE_LINELIST:
			return primitiveCount * 2;

		case ruPRIMTYPE_POINTLIST:
			return primitiveCount;
	}
}

// ************************************************************************************************************************************************************

// Note about texture channels - each channel can be used only once in a given render pass.
// e.g. if ruTEXCHANNEL_DIFFUSEMAP is used in sampler 0, it cannot be used in another sampler.
// Take notice that this has nothing to do with limiting use of actual textures!
// This merely means that each channel is guaranteed to be uniquely identified among all
// samplers used in a render pass. For all we know, the channels could be all using the same
// texture object. Primary reason for this is to properly link texture transforms to specific
// texture channels. Otherwise, if there were two diffuse channels, and one diffuse texture transform,
// which channel does the transform apply to? The shader would have no clue!
// Texture channels
enum RuTextureChannel
{
	ruTEXCHANNEL_INVALID						= 0,							//!< Invalid/No texture

	ruTEXCHANNEL_DIFFUSEMAP						= 1,							//!< Base diffuse texture map
	ruTEXCHANNEL_DETAILMAP						= 2,							//!< Detail texture map

	ruTEXCHANNEL_LIGHTMAP						= 3,							//!< Light map
	ruTEXCHANNEL_DIFFUSECUBEMAP					= 8,							//!< Diffuse cube map (diffuse environmental lighting)
	ruTEXCHANNEL_1DATTENUATIONMAP				= 4,							//!< 1D attenuation map (Usually used in combination with 2D attenuation map)
	ruTEXCHANNEL_2DATTENUATIONMAP				= 5,							//!< 2D attenuation map (Usually used in combination with 1D attenuation map)
	ruTEXCHANNEL_3DATTENUATIONMAP				= 6,							//!< 3D attenuation map

	ruTEXCHANNEL_ENVIRONMENTMAP					= 7,							//!< Environmental map

	ruTEXCHANNEL_REFLECTIONMAP					= 9,							//!< Reflection map
	ruTEXCHANNEL_REFRACTIONMAP					= 10,							//!< Refraction map

	ruTEXCHANNEL_CAUSTICMAP						= 11,							//!< Caustics map
	ruTEXCHANNEL_BUMPMAP						= 12,							//!< Bump map
	ruTEXCHANNEL_BUMPMAP2						= 13,							//!< Bump map 2
	ruTEXCHANNEL_NORMALMAP						= 14,							//!< Normal map
	ruTEXCHANNEL_NORMALMAP2						= 15,							//!< Normal map 2

	ruTEXCHANNEL_DISPLACEMENTMAP				= 16,							//!< Displacement map
	ruTEXCHANNEL_NORMALIZATIONCUBEMAP			= 17,							//!< Normalization cube map

	ruTEXCHANNEL_SHADOWMAP						= 18,							//!< Projected shadow map
	ruTEXCHANNEL_SHADOWDEPTHMAP					= 19,							//!< Shadow depth map
	ruTEXCHANNEL_SHADOWOVERLAY					= 20,							//!< Shadow overlay map

	ruTEXCHANNEL_GLOSSMAP						= 21,							//!< Gloss map (modulates specular)
	ruTEXCHANNEL_TOONGRADIENTMAP				= 22,							//!< 1-D toon shading gradient map

	ruTEXCHANNEL_DISTORTMAP						= 23,							//!< Distortion normal map
	ruTEXCHANNEL_BACKBUFFER						= 24,							//!< Backbuffer copy

	// Special maps
	ruTEXCHANNEL_RTLIGHTMAP						= 25,							//!< Real-time generated light map
	ruTEXCHANNEL_GLOWMAP						= 26,							//!< Glow map

	// Paperdoll channels
	ruTEXCHANNEL_PAPERDOLL_BASE					= 31,
	ruTEXCHANNEL_PAPERDOLL_LAYER_0				= 32,
	ruTEXCHANNEL_PAPERDOLL_LAYER_1				= 33,
	ruTEXCHANNEL_PAPERDOLL_LAYER_0_FIXED		= 34,
	ruTEXCHANNEL_PAPERDOLL_LAYER_1_FIXED		= 35,
	ruTEXCHANNEL_PAPERDOLL_MASK_0				= 36,
	ruTEXCHANNEL_PAPERDOLL_MASK_1				= 37,
	ruTEXCHANNEL_PAPERDOLL_MASK_2				= 38,
	ruTEXCHANNEL_PAPERDOLL_MASK_3				= 39,
	ruTEXCHANNEL_PAPERDOLL_MASK_COMPOSITE		= 40,							//!< Composited color change mask
	ruTEXCHANNEL_PAPERDOLL_COLORMAP				= 41,							//!< Composited color change mask

	// System maps
	ruTEXCHANNEL_SYSTEM_NULL					= 61,							//!< Null texture (all 0xFFFFFFFF)
	ruTEXCHANNEL_SYSTEM_CLAMP					= 62,							//!< Clamping texture

	// Terrain channels
	ruTEXCHANNEL_TERRAIN_ALPHA_0				= 101,							//!< Terrain alpha channel 0
	ruTEXCHANNEL_TERRAIN_ALPHA_1				= 102,							//!< Terrain alpha channel 1
	ruTEXCHANNEL_TERRAIN_ALPHA_2				= 103,							//!< Terrain alpha channel 2
	ruTEXCHANNEL_TERRAIN_ALPHA_3				= 104,							//!< Terrain alpha channel 3
	ruTEXCHANNEL_TERRAIN_LAYER_0				= 105,							//!< Terrain layer 0
	ruTEXCHANNEL_TERRAIN_LAYER_1				= 106,							//!< Terrain layer 1
	ruTEXCHANNEL_TERRAIN_LAYER_2				= 107,							//!< Terrain layer 2
	ruTEXCHANNEL_TERRAIN_LAYER_3				= 108,							//!< Terrain layer 3
	ruTEXCHANNEL_TERRAIN_LAYER_4				= 109,							//!< Terrain layer 4
	ruTEXCHANNEL_TERRAIN_LAYER_5				= 110,							//!< Terrain layer 5
	ruTEXCHANNEL_TERRAIN_LAYER_6				= 111,							//!< Terrain layer 6
	ruTEXCHANNEL_TERRAIN_LAYER_7				= 112,							//!< Terrain layer 7
	ruTEXCHANNEL_TERRAIN_LAYER_8				= 113,							//!< Terrain layer 8
	ruTEXCHANNEL_TERRAIN_LAYER_9				= 114,							//!< Terrain layer 9
	ruTEXCHANNEL_TERRAIN_LAYER_10				= 115,							//!< Terrain layer 10
	ruTEXCHANNEL_TERRAIN_LAYER_11				= 116,							//!< Terrain layer 11

	ruTEXCHANNEL_SKYDOME_GRADIENT_0				= 121,							//!< Sky dome gradient map 0
	ruTEXCHANNEL_SKYDOME_GRADIENT_1				= 122,							//!< Sky dome gradient map 1

	ruTEXCHANNEL_SKYDOME_CLOUD_0				= 123,							//!< Sky dome cloud map 0
	ruTEXCHANNEL_SKYDOME_CLOUD_1				= 124,							//!< Sky dome cloud map 1

	ruTEXCHANNEL_CUSTOM_0						= 125,
	ruTEXCHANNEL_CUSTOM_1						= 126,
	ruTEXCHANNEL_CUSTOM_2						= 127,
	ruTEXCHANNEL_CUSTOM_3						= 128,
	ruTEXCHANNEL_CUSTOM_4						= 129,
	ruTEXCHANNEL_CUSTOM_5						= 130,
	ruTEXCHANNEL_CUSTOM_6						= 131,
	ruTEXCHANNEL_CUSTOM_7						= 132,

	ruTEXCHANNEL_FORCE_DWORD					= 0x7FFFFFFF
};

inline INT32 RuTextureChannel_GetNumComponents(RuTextureChannel texChannel)
{
	switch(texChannel)
	{
		case ruTEXCHANNEL_1DATTENUATIONMAP:
			return 1;

		default:
			return 2;

		case ruTEXCHANNEL_DIFFUSECUBEMAP:
		case ruTEXCHANNEL_3DATTENUATIONMAP:
		case ruTEXCHANNEL_ENVIRONMENTMAP:
		case ruTEXCHANNEL_NORMALIZATIONCUBEMAP:
			return 3;
	}

	return 0;
}

// ************************************************************************************************************************************************************

enum RuLightType
{
	ruLIGHTTYPE_POINT							= 0,							// Point light
	ruLIGHTTYPE_SPOT							= 1,							// Spot light
	ruLIGHTTYPE_DIRECTIONAL						= 2,							// Directional light
	ruLIGHTTYPE_AMBIENT							= 3,

	ruLIGHTTYPE_FORCE_DWORD						= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuFogType
{
	ruFOGTYPE_NONE								= 0,							// No fog
	ruFOGTYPE_LINEAR							= 1,							// Linear fog
	ruFOGTYPE_EXPONENTIAL						= 2,							// Exponential fog
	ruFOGTYPE_EXPONENTIAL2						= 3,							// Squared exponential fog

	ruFOGTYPE_FORCE_DWORD						= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuAlphaMode
{
	ruALPHAMODE_NORMAL							= 0,							//!< SRCALPHA:INVSRCALPHA
	ruALPHAMODE_ADD								= 1,							//!< SRCALPHA:ONE
	ruALPHAMODE_SUBTRACT						= 2,							//!< ZERO:INVSRCALPHA
	ruALPHAMODE_PREMULTIPLIED_NORMAL			= 3,							//!< ONE:INVSRCALPHA
	ruALPHAMODE_PREMULTIPLIED_ADD				= 4,							//!< ONE:ONE
	ruALPHAMODE_DISABLE							= 5,							//!< ONE:ZERO

	ruALPHAMODE_FORCE_DWORD						= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuAlphaTestMode
{
	ruALPHATESTMODE_NONE						= 0,							//!< Alpha test is disabled
	ruALPHATESTMODE_GREATER						= 1,							//!< Default alpha test mode, greater
	ruALPHATESTMODE_GREATEREQUAL				= 2,							//!<
	ruALPHATESTMODE_EQUAL						= 3,							//!<
	ruALPHATESTMODE_LESSEQUAL					= 4,							//!<
	ruALPHATESTMODE_LESS						= 5,							//!<
	ruALPHATESTMODE_NOTEQUAL					= 6,							//!<

	ruALPHATESTMODE_FORCE_DWORD					= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuZTestMode
{
	ruZTESTMODE_DEFAULT							= 0,							//!< Default Z-test rule
	ruZTESTMODE_ALWAYS							= 1,							//!< 
	ruZTESTMODE_GREATER							= 2,							//!< 
	ruZTESTMODE_GREATEREQUAL					= 3,							//!<
	ruZTESTMODE_EQUAL							= 4,							//!<
	ruZTESTMODE_LESSEQUAL						= 5,							//!<
	ruZTESTMODE_LESS							= 6,							//!<
	ruZTESTMODE_NOTEQUAL						= 7,							//!<

	ruZTESTMODE_FORCE_DWORD						= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuZWriteMode
{
	ruZWRITEMODE_DEFAULT						= 0,							//!< Default Z-write rule
	ruZWRITEMODE_DISABLE						= 1,							//!< Force disable Z-write
	ruZWRITEMODE_ENABLE							= 2,							//!< Force enable Z-write

	ruZWRITEMODE_FORCE_DWORD					= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuBillboardType
{
	ruBILLBOARDTYPE_NONE						= 0,							//!< No billboarding
	ruBILLBOARDTYPE_UNCONSTRAINED				= 1,							//!< Unconstrained billboard
	ruBILLBOARDTYPE_UNCONSTRAINED_ALTERNATE		= 4,							//!< Unconstrained billboard (Alternate transformation)
	ruBILLBOARDTYPE_AXIAL						= 2,							//!< Axially-constrained billboard about Y axis
	ruBILLBOARDTYPE_AXIAL_ALTERNATE				= 5,							//!< Axially-constrained billboard about Y axis (Alternate transformation)
	ruBILLBOARDTYPE_AXIAL_CUSTOM				= 3,							//!< Axially-constrained billboard about custom axis

	ruBILLBOARDTYPE_FORCE_DWORD					= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuResourcePool
{
	ruRESPOOL_DEFAULT							= 0,							// Default resource pool - typically AGP or video memory
	ruRESPOOL_MANAGED							= 1,							// Managed resource pool
	ruRESPOOL_SYSTEMMEMORY						= 2,							// System memory pool
	ruRESPOOL_SCRATCH							= 3,							// Scratch memory pool

	ruRESPOOL_FORCE_DWORD						= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuSurfaceFormat
{
	ruSURFACEFMT_DEFAULT						= 0,

	// 8-bit unsigned formats
	ruSURFACEFMT_A8								= 28,
	ruSURFACEFMT_P8								= 41,
	ruSURFACEFMT_L8								= 50,
	ruSURFACEFMT_A4L4							= 52,

	// 16-bit unsigned formats
	ruSURFACEFMT_R5G6G5							= 23,
	ruSURFACEFMT_X1R5G5B5						= 24,
	ruSURFACEFMT_A1R5G5B5						= 25,
	ruSURFACEFMT_A4R4G4B4						= 26,
	ruSURFACEFMT_A8R3G3B2						= 29,
	ruSURFACEFMT_X4R4G4B4						= 30,
	ruSURFACEFMT_A8P8							= 40,
	ruSURFACEFMT_A8L8							= 51,
	ruSURFACEFMT_L16							= 81,

	// 24-bit unsigned formats
	ruSURFACEFMT_R8G8B8							= 20,
	ruSURFACEFMT_R3G3B2							= 27,

	// 32-bit unsigned formats
	ruSURFACEFMT_A8R8G8B8						= 21,
	ruSURFACEFMT_X8R8G8B8						= 22,
	ruSURFACEFMT_A8B8G8R8						= 32,
	ruSURFACEFMT_X8B8G8R8						= 33,
	ruSURFACEFMT_A2B10G10R10					= 31,
	ruSURFACEFMT_G16R16							= 34,
	ruSURFACEFMT_A2R10G10B10					= 35,

	// 64-bit unsigned formats
	ruSURFACEFMT_A16B16G16R16					= 36,

	// Depth-stencil formats
	ruSURFACEFMT_D16_LOCKABLE					= 70,
	ruSURFACEFMT_D32							= 71,
	ruSURFACEFMT_D15S1							= 73,
	ruSURFACEFMT_D24S8							= 75,
	ruSURFACEFMT_D24X8							= 77,
	ruSURFACEFMT_D24X4S4						= 79,
	ruSURFACEFMT_D16							= 80,

	ruSURFACEFMT_D32F_LOCKABLE					= 82,
	ruSURFACEFMT_D24FS8				            = 83,

	// 16-bit floating point formats
	ruSURFACEFMT_R16F							= 111,
	ruSURFACEFMT_G16R16F						= 112,
	ruSURFACEFMT_A16B16G16R16F					= 113,

	// 32-bit floating point formats
	ruSURFACEFMT_R32F							= 114,
	ruSURFACEFMT_G32R32F						= 115,
	ruSURFACEFMT_A32B32G32R32F					= 116,

	// Signed formats
	ruSURFACEFMT_V8U8							= 60,
	ruSURFACEFMT_Q8W8V8U8						= 63,
	ruSURFACEFMT_V16U16							= 64,
	ruSURFACEFMT_Q16W16V16U16					= 110,
	ruSURFACEFMT_CxV8U8							= 117,

	// Mixed formats
	ruSURFACEFMT_L6V5U5							= 61,
	ruSURFACEFMT_X8L8V8U8						= 62,
	ruSURFACEFMT_A2W10V10U10					= 67,

	// Compressed formats
	ruSURFACEFMT_DXT1							= RUMAKEFOURCC('D', 'X', 'T', '1'),
	ruSURFACEFMT_DXT2							= RUMAKEFOURCC('D', 'X', 'T', '2'),
	ruSURFACEFMT_DXT3							= RUMAKEFOURCC('D', 'X', 'T', '3'),
	ruSURFACEFMT_DXT4							= RUMAKEFOURCC('D', 'X', 'T', '4'),
	ruSURFACEFMT_DXT5							= RUMAKEFOURCC('D', 'X', 'T', '5'),

	ruSURFACEFMT_FORCE_DWORD					= 0x7FFFFFFF
};

inline INT32 RuSurfaceFormat_GetBitsPerTexel(RuSurfaceFormat surfaceFormat)
{
	switch(surfaceFormat)
	{
		// 8-bit unsigned formats
		case ruSURFACEFMT_A8:
		case ruSURFACEFMT_P8:
		case ruSURFACEFMT_L8:
		case ruSURFACEFMT_A4L4:
			return 8;

		// 16-bit unsigned formats
		case ruSURFACEFMT_R5G6G5:
		case ruSURFACEFMT_X1R5G5B5:
		case ruSURFACEFMT_A1R5G5B5:
		case ruSURFACEFMT_A4R4G4B4:
		case ruSURFACEFMT_A8R3G3B2:
		case ruSURFACEFMT_X4R4G4B4:
		case ruSURFACEFMT_A8P8:
		case ruSURFACEFMT_A8L8:
		case ruSURFACEFMT_L16:
			return 16;

		// 24-bit unsigned formats
		case ruSURFACEFMT_R8G8B8:
		case ruSURFACEFMT_R3G3B2:
			return 24;

		// 32-bit unsigned formats
		case ruSURFACEFMT_A8R8G8B8:
		case ruSURFACEFMT_X8R8G8B8:
		case ruSURFACEFMT_A8B8G8R8:
		case ruSURFACEFMT_X8B8G8R8:
		case ruSURFACEFMT_A2B10G10R10:
		case ruSURFACEFMT_G16R16:
		case ruSURFACEFMT_A2R10G10B10:
			return 32;

		// 64-bit unsigned formats
		case ruSURFACEFMT_A16B16G16R16:
			return 64;

		// Depth-stencil formats
		case ruSURFACEFMT_D16_LOCKABLE:
		case ruSURFACEFMT_D32:
		case ruSURFACEFMT_D15S1:
		case ruSURFACEFMT_D24S8:
		case ruSURFACEFMT_D24X8:
		case ruSURFACEFMT_D24X4S4:
		case ruSURFACEFMT_D16:

		case ruSURFACEFMT_D32F_LOCKABLE:
		case ruSURFACEFMT_D24FS8:
			return 0;

		// 16-bit floating point formats
		case ruSURFACEFMT_R16F:
		case ruSURFACEFMT_G16R16F:
		case ruSURFACEFMT_A16B16G16R16F:
			return 16;

		// 32-bit floating point formats
		case ruSURFACEFMT_R32F:
		case ruSURFACEFMT_G32R32F:
		case ruSURFACEFMT_A32B32G32R32F:
			return 32;

		// Signed formats
		case ruSURFACEFMT_V8U8:
		case ruSURFACEFMT_Q8W8V8U8:
		case ruSURFACEFMT_V16U16:
		case ruSURFACEFMT_Q16W16V16U16:
		case ruSURFACEFMT_CxV8U8:
			return 0;

		// Mixed formats
		case ruSURFACEFMT_L6V5U5:
		case ruSURFACEFMT_X8L8V8U8:
		case ruSURFACEFMT_A2W10V10U10:
			return 0;

		// Compressed formats
		case ruSURFACEFMT_DXT1:
			return 4;

		case ruSURFACEFMT_DXT2:
			return 8;

		case ruSURFACEFMT_DXT3:
			return 8;

		case ruSURFACEFMT_DXT4:
			return 8;

		case ruSURFACEFMT_DXT5:
			return 8;
	}

	return 0;
}

// ************************************************************************************************************************************************************

enum RuCubeTextureFace
{
	ruCUBEFACE_POSITIVEX						= 0,
	ruCUBEFACE_NEGATIVEX						= 1,
	ruCUBEFACE_POSITIVEY						= 2,
	ruCUBEFACE_NEGATIVEY						= 3,
	ruCUBEFACE_POSITIVEZ						= 4,
	ruCUBEFACE_NEGATIVEZ						= 5,

	ruCUBEFACE_FORCE_DWORD						= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuImageFileFormat
{
	ruIMAGEFILEFORMAT_BMP						= 0,
	ruIMAGEFILEFORMAT_JPG						= 1,
	ruIMAGEFILEFORMAT_TGA						= 2,
	ruIMAGEFILEFORMAT_PNG						= 3,
	ruIMAGEFILEFORMAT_DDS						= 4,
	ruIMAGEFILEFORMAT_PPM						= 5,
	ruIMAGEFILEFORMAT_DIB						= 6,
	ruIMAGEFILEFORMAT_HDR						= 7,
	ruIMAGEFILEFORMAT_PFM						= 8,

	ruIMAGEFILEFORMAT_FORCE_DWORD				= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

/*!
	Enumeration of possible shader parameters. When the parameters are set into the shader,
	the shader may not necessarily honor the parameter setting. See individual shader
	documentation for details. In general, transform and lighting parameters are always
	supported by shaders.

	@author John Tang
	@version 2004/12/21
*/
enum RuParameterType
{
	ruPARAMTYPE_INVALID							= 0,

	// Transforms
	ruPARAMTYPE_TRANSFORM_WORLD					= 1,				//!< World transform, parameter = CRuMatrix4x4*
	ruPARAMTYPE_TRANSFORM_VIEW					= 2,				//!< View transform, parameter = CRuMatrix4x4*
	ruPARAMTYPE_TRANSFORM_PROJECTION			= 3,				//!< Projection transform, parameter = CRuMatrix4x4*
	ruPARAMTYPE_TRANSFORM_BILLBOARD				= 4,				//!< Billboarding transform, parameter = CRuMatrix4x4*

	// Shadow
	ruPARAMTYPE_SHADOW_DIR						= 240,				//!< Shadow direction, parameter = 
	ruPARAMTYPE_SHADOW_FADE_START				= 241,				//!< Shadow distance, parameter = float
	ruPARAMTYPE_SHADOW_FADE_END					= 242,				//!< Shadow distance, parameter = float
	ruPARAMTYPE_SHADOW_STRENGTH					= 243,				//!< Shadow strength, parameter = float [0, 1]
	ruPARAMTYPE_TRANSFORM_SHADOW				= 244,				//!< Shadow transform, parameter = CRuMatrix4x4*
	ruPARAMTYPE_TRANSFORM_SHADOW2				= 245,				//!< Shadow transform, parameter = CRuMatrix4x4*
	ruPARAMTYPE_SHADOW_OBJECTID					= 246,				//!< Object ID for shadow map, parameter = UINT32

	// Lighting
	ruPARAMTYPE_LIGHT_AMBIENT					= 5,				//!< Ambient lighting term, parameter = CRuColor4*
	ruPARAMTYPE_LIGHT_OBJ_SH9					= 6,				//!< Object-level Spherical Harmonic terms (3rd order, 9 terms)
	ruPARAMTYPE_LIGHT_SPECULAR_DIR				= 7,
	ruPARAMTYPE_LIGHT_SPECULAR_COLOR			= 8,				//!< Parameter = RUCOLOR

	ruPARAMTYPE_CAMERA							= 9,				//!< Parameter = CRuCamera*

	// Fog
	ruPARAMTYPE_FOG_TYPE						= 11,
	ruPARAMTYPE_FOG_START						= 12,				//!< Parameter = float
	ruPARAMTYPE_FOG_END							= 13,				//!< Parameter = float
	ruPARAMTYPE_FOG_CONSTANT					= 14,				//!< Constant minimal fog level, Parameter = float [0, 1]
	ruPARAMTYPE_FOG_COLOR						= 15,				//!< Parameter = RUCOLOR

	// Tweening
	ruPARAMTYPE_TWEEN_FACTOR					= 16,				//!< Parameter = float* [0, 1]
	ruPARAMTYPE_TWEEN_MORPHTARGET_0				= 17,				//!< Parameter = INT32
	ruPARAMTYPE_TWEEN_MORPHTARGET_1				= 18,				//!< Parameter = INT32

	// Skinning
	ruPARAMTYPE_SKIN_NODECOUNT					= 19,				//!< Parameter = INT32 (Number of nodes in the skin)
	ruPARAMTYPE_SKIN_INDEXMAP					= 20,				//!< Parameter = Array of UINT16 (Index map - remaps bone indices for split meshes)
	ruPARAMTYPE_SKIN_TRANSFORMS					= 21,				//!< Parameter = Array of CRuMatrix4x4 (Node transforms)

	// Terrain-specific parameters
	ruPARAMTYPE_CHUNK_SPACING					= 25,				//!< Parameter = REAL (world-size spacing between vertices)
	ruPARAMTYPE_CHUNK_Y_BASE					= 26,				//!< Parameter = REAL (Minimum vertex Y)
	ruPARAMTYPE_CHUNK_Y_INCREMENT				= 27,				//!< Parameter = REAL (Vertex Y increment per unit)
	ruPARAMTYPE_SPLAT_ALPHAUVMODIFIER			= 28,
	ruPARAMTYPE_SPLAT_LAYERCOUNT				= 29,				//!< Parameter = UINT32 (Number of texture layers)

	// Water-specific parameters
	ruPARAMTYPE_WATER_SPLITPLANE				= 31,				//!< Parameter = CRuPlane* (Split plane of the water surface)
	ruPARAMTYPE_WATER_BUMPFACTOR				= 32,				//!< Parameter = float (Bump map strength factor)
	ruPARAMTYPE_WATER_WAVECOLOR					= 33,				//!< Parameter = RUCOLOR (Wave color)

	// Sky dome parameters
	ruPARAMTYPE_SKYDOME_GRADIENT_T				= 41,				//!< Parameter = float
	ruPARAMTYPE_SKYDOME_CLOUD_T					= 42,				//!< Parameter = float

	// Instancing?
	ruPARAMTYPE_GEOINSTANCE_S_COUNT				= 44,				//!< Parameter = UINT32 (Number of instances to render)
	ruPARAMTYPE_GEOINSTANCE_S_DATA				= 45,				//!< Parameter = float* (Array of float-4's with instancing data)

	// 2D toolkit parameters
	ruPARAMTYPE_2DFX_TEXTURE_WIDTH				= 51,				//!< Parameter = UINT32 (width of the texture)
	ruPARAMTYPE_2DFX_TEXTURE_HEIGHT				= 52,				//!< Parameter = UINT32 (height of the texture)

	// Material options (persisted)
	ruPARAMTYPE_MATERIAL_SELFILLUMINATIONCOLOR	= 79,				//!< Self-illumination color, parameter = RUCOLOR
	ruPARAMTYPE_MATERIAL_SELFILLUMINATION		= 60,				//!< Self-illumination term, parameter = float

	ruPARAMTYPE_MATERIAL_2SIDED					= 61,				//!< Is the geometry 2-sided? Parameter = BOOL
	ruPARAMTYPE_MATERIAL_FACETED				= 62,				//!< Is the geometry faceted? Parameter = BOOL
	ruPARAMTYPE_MATERIAL_WIRE					= 63,				//!< Is the geometry wireframe? Parameter = BOOL

	ruPARAMTYPE_MATERIAL_ALPHATESTMODE			= 64,				//!< Alpha test mode, parameter = RuAlphaTestMode
	ruPARAMTYPE_MATERIAL_ALPHATESTVALUE			= 65,				//!< Alpha test value, parameter = UINT32 [0, 255]

	ruPARAMTYPE_MATERIAL_ALPHAMODE				= 71,				//!< Material alpha setting, may be ignored by shader. Parameter = RuAlphaMode
	ruPARAMTYPE_MATERIAL_TRANSLUCENT			= 72,				//!< Is the material translucent? Parameter = BOOL

	ruPARAMTYPE_MATERIAL_ZTESTMODE				= 77,				//!< Z test mode, parameter = RuZTestMode
	ruPARAMTYPE_MATERIAL_ZWRITEMODE				= 73,				//!< Material Z Write mode, Parameter = RuZWriteMode

	ruPARAMTYPE_MATERIAL_ALPHA					= 74,				//!< Material alpha value, parameter = float [0, 1]
	ruPARAMTYPE_MATERIAL_ALPHAMODIFIER			= 78,				//!< Material alpha value modifier(actual alpha = alpha * modifier), parameter = float [0, 1]
	ruPARAMTYPE_MATERIAL_DIFFUSE				= 75,				//!< Parameter = RUCOLOR

	ruPARAMTYPE_MATERIAL_SMOOTHBLEND			= 76,				//!< Use smooth blending on alpha-tested material? Parameter = BOOL

	ruPARAMTYPE_MATERIAL_COLORWRITEENABLE		= 93,				//!< Enable color write? parameter = BOOL

	// Texture options
	ruPARAMTYPE_TEXTURE_USTEP					= 80,				//!< u step size
	ruPARAMTYPE_TEXTURE_VSTEP					= 81,				//!< v step size
	ruPARAMTYPE_TEXTURE_UINDEX					= 82,				//!< u index
	ruPARAMTYPE_TEXTURE_VINDEX					= 83,				//!< v index

	// Raw shader constant setting
	ruPARAMTYPE_RAW_OFFSET						= 84,				//!< Raw constant row offset
	ruPARAMTYPE_RAW_COUNT						= 85,				//!< Raw constant row count
	ruPARAMTYPE_RAW_DATA						= 86,				//!< Raw constant row data

	// Raw pixel shader constant setting
	ruPARAMTYPE_RAW_PS_OFFSET					= 87,				//!< Raw constant row offset
	ruPARAMTYPE_RAW_PS_COUNT					= 88,				//!< Raw constant row count
	ruPARAMTYPE_RAW_PS_DATA						= 89,				//!< Raw constant row data

	// Other FX settings
	ruPARAMTYPE_FX_BILLBOARD					= 90,				//!< Billboard setting
	ruPARAMTYPE_FX_REVERSECULL					= 91,				//!< Reverse culling winding order
	ruPARAMTYPE_FX_EXTRUDESCALE					= 92,				//!< Extrude scale along vertex normal, parameter = REAL

	// System stuff
	ruPARAMTYPE_SYSTEM_INVERTSORTORDER			= 230,				//!< Alpha sorting is inverted for the entity
	ruPARAMTYPE_SYSTEM_IGNOREALPHASORT			= 231,				//!< Alpha sorting is ignored for the entity
	ruPARAMTYPE_SYSTEM_ALPHAZPASS				= 234,				//!< Alpha sorting follows the special alpha Z pass rule
	ruPARAMTYPE_SYSTEM_STENCILENABLE			= 232,				//!< Enable stencil masking?
	ruPARAMTYPE_SYSTEM_STENCILID				= 233,				//!< Stencil ID to use, parameter = UINT8

	// ...
	ruPARAMTYPE_TEXTURE_TRANSFORM_0				= 100,				//!< UV 0 transform
	ruPARAMTYPE_TEXTURE_TRANSFORM_1				= 101,				//!< UV 1 transform
	ruPARAMTYPE_TEXTURE_TRANSFORM_2				= 102,				//!< UV 2 transform
	ruPARAMTYPE_TEXTURE_TRANSFORM_3				= 103,				//!< UV 3 transform
	ruPARAMTYPE_TEXTURE_TRANSFORM_4				= 104,				//!< UV 4 transform
	ruPARAMTYPE_TEXTURE_TRANSFORM_5				= 105,				//!< UV 5 transform
	ruPARAMTYPE_TEXTURE_TRANSFORM_6				= 106,				//!< UV 6 transform
	ruPARAMTYPE_TEXTURE_TRANSFORM_7				= 107,				//!< UV 7 transform

	ruPARAMTYPE_TRANSFORM_0						= 210,				//!< Transform 0
	ruPARAMTYPE_TRANSFORM_1						= 211,				//!< Transform 1
	ruPARAMTYPE_TRANSFORM_2						= 212,				//!< Transform 2
	ruPARAMTYPE_TRANSFORM_3						= 213,				//!< Transform 3
	ruPARAMTYPE_TRANSFORM_4						= 214,				//!< Transform 4
	ruPARAMTYPE_TRANSFORM_5						= 215,				//!< Transform 5
	ruPARAMTYPE_TRANSFORM_6						= 216,				//!< Transform 6
	ruPARAMTYPE_TRANSFORM_7						= 217,				//!< Transform 7

	// ...
	ruPARAMTYPE_CUSTOM_0						= 200,				//!< User-defined parameter 2
	ruPARAMTYPE_CUSTOM_1						= 201,				//!< User-defined parameter 3
	ruPARAMTYPE_CUSTOM_2						= 202,				//!< User-defined parameter 4
	ruPARAMTYPE_CUSTOM_3						= 203,				//!< User-defined parameter 5
	ruPARAMTYPE_CUSTOM_4						= 204,				//!< User-defined parameter 5
	ruPARAMTYPE_CUSTOM_5						= 205,				//!< User-defined parameter 5
	ruPARAMTYPE_CUSTOM_6						= 206,				//!< User-defined parameter 5
	ruPARAMTYPE_CUSTOM_7						= 207,				//!< User-defined parameter 5

	ruPARAMTYPE_DWORD_0							= 208,				//!< User-defined parameter 0
	ruPARAMTYPE_DWORD_1							= 209,				//!< User-defined parameter 1
	ruPARAMTYPE_DWORD_2							= 210,				//!< User-defined parameter 1
	ruPARAMTYPE_DWORD_3							= 211,				//!< User-defined parameter 1

	// DO NOT TOUCH ME
	ruPARAMTYPE_MAX								= 256,				//!< Largest parameter index

	ruPARAMTYPE_FORCE_DWORD						= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

/*!
	Specifies the usage for the vertex element.
	@author John Tang
	@version 2004/10/07
*/
enum RuVertexElementUsage
{
	ruELEMENTUSAGE_POSITION							= 0,
	ruELEMENTUSAGE_BLENDWEIGHT						= 1,
	ruELEMENTUSAGE_BLENDINDICES						= 2,
	ruELEMENTUSAGE_NORMAL							= 3,
	ruELEMENTUSAGE_PSIZE							= 4,
	ruELEMENTUSAGE_TEXCOORD							= 5,
	ruELEMENTUSAGE_TANGENT							= 6,
	ruELEMENTUSAGE_BINORMAL							= 7,
	ruELEMENTUSAGE_TESSFACTOR						= 8,
	ruELEMENTUSAGE_POSITIONT						= 9,
	ruELEMENTUSAGE_COLOR							= 10,
	ruELEMENTUSAGE_FOG								= 11,
	ruELEMENTUSAGE_DEPTH							= 12,
	ruELEMENTUSAGE_SAMPLE							= 13
};

// ************************************************************************************************************************************************************

/*!
	Specifies the method of data conversion used for the vertex element.
	@author John Tang
	@version 2004/10/07
*/
enum RuVertexElementMethod
{
	ruELEMENTMETHOD_DEFAULT							= 0,
	ruELEMENTMETHOD_PARTIALU						= 1,
	ruELEMENTMETHOD_PARTIALV						= 2,
	ruELEMENTMETHOD_CROSSUV							= 3,				// Normal
	ruELEMENTMETHOD_UV								= 4,
	ruELEMENTMETHOD_LOOKUP							= 5,				// Lookup a displacement map
	ruELEMENTMETHOD_LOOKUPPRESAMPLED				= 6					// Lookup a pre-sampled displacement map
};

// ************************************************************************************************************************************************************

/*!
	Specifies the type of data the vertex element holds.
	@author John Tang
	@version 2004/10/07
*/
enum RuVertexElementType
{
	ruELEMENTTYPE_FLOAT1							=  0,				// 1D float expanded to (value, 0., 0., 1.)
	ruELEMENTTYPE_FLOAT2							=  1,				// 2D float expanded to (value, value, 0., 1.)
	ruELEMENTTYPE_FLOAT3							=  2,				// 3D float expanded to (value, value, value, 1.)
	ruELEMENTTYPE_FLOAT4							=  3,				// 4D float
	ruELEMENTTYPE_RUCOLOR							=  4,				// 4D packed unsigned bytes mapped to 0. to 1. range
																		// Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
	ruELEMENTTYPE_UBYTE4							=  5,				// 4D unsigned byte
	ruELEMENTTYPE_SHORT2							=  6,				// 2D signed short expanded to (value, value, 0., 1.)
	ruELEMENTTYPE_SHORT4							=  7,				// 4D signed short

	// The following types are valid only with vertex shaders >= 2.0
	ruELEMENTTYPE_UBYTE4N							=  8,				// Each of 4 bytes is normalized by dividing to 255.0
	ruELEMENTTYPE_SHORT2N							=  9,				// 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
	ruELEMENTTYPE_SHORT4N							= 10,				// 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
	ruELEMENTTYPE_USHORT2N							= 11,				// 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
	ruELEMENTTYPE_USHORT4N							= 12,				// 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
	ruELEMENTTYPE_UDEC3								= 13,				// 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
	ruELEMENTTYPE_DEC3N								= 14,				// 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
	ruELEMENTTYPE_FLOAT16_2							= 15,				// Two 16-bit floating point values, expanded to (value, value, 0, 1)
	ruELEMENTTYPE_FLOAT16_4							= 16,				// Four 16-bit floating point values
	ruELEMENTTYPE_UNUSED							= 17				// When the type field in a decl is unused.
};

// ************************************************************************************************************************************************************

/*!
	Used to define the structure of a complete vertex stream. The structure and definitions
	are identical to those used by DX9.0c.
	@author John Tang
	@version 2004/10/07
*/
class CRuVertexElement
{
public:
	UINT16										m_stream;				// Stream index (0xFF if the element marks the end of the array)
	UINT16										m_offset;				// Offset in the stream in UINT8s
	UINT8										m_type;					// Data type
	UINT8										m_method;				// Processing method
	UINT8										m_usage;				// Semantics
	UINT8										m_usageIndex;			// Semantic index
};

// This is used to initialize the last vertex element in a vertex declaration array
#define RUVERTEXDECL_END()						{ 0xFF, 0, ruELEMENTTYPE_UNUSED, 0, 0, 0 }

// Helpers
inline UINT32									GetNumVertexElements(const CRuVertexElement *vertexElements)
												{
													const CRuVertexElement *curElement = vertexElements;

													UINT32 numElements = 0;
													while(curElement->m_stream != 0xFF)
													{
														++numElements;
														++curElement;
													}

													return numElements;
												}

inline INT32									GetNumVertexStreams(const CRuVertexElement *vertexElements)
												{
													const CRuVertexElement *curElement = vertexElements;

													INT32 maxStreamIdx = 0;
													while(curElement->m_stream != 0xFF)
													{
														maxStreamIdx = max(maxStreamIdx, curElement->m_stream);
														++curElement;
													}

													return maxStreamIdx + 1;
												}

inline INT32									StreamHasElements(const CRuVertexElement *vertexElements, INT32 streamIdx)
												{
													const CRuVertexElement *curElement = vertexElements;

													while(curElement->m_stream != 0xFF)
													{
														if(curElement->m_stream == streamIdx)
														{
															return TRUE;
														}

														++curElement;
													}

													return FALSE;
												}

inline DWORD									GetHashCode(const CRuVertexElement *vertexElements)
												{
													return RuGlobalHashGen().GetHashCode((BYTE *) vertexElements, GetNumVertexElements(vertexElements) * sizeof(CRuVertexElement));
												}

inline BOOL										IsEqual(const CRuVertexElement *vertexElementsA, const CRuVertexElement *vertexElementsB)
												{
													UINT32 numElementsA = GetNumVertexElements(vertexElementsA);
													UINT32 numElementsB = GetNumVertexElements(vertexElementsB);

													if(numElementsA == numElementsB)
													{
														for(UINT32 i = 0; i < numElementsA; ++i)
														{
															if(	vertexElementsA[i].m_stream != vertexElementsB[i].m_stream			||
																vertexElementsA[i].m_offset != vertexElementsB[i].m_offset			||
																vertexElementsA[i].m_type != vertexElementsB[i].m_type				||
																vertexElementsA[i].m_method != vertexElementsB[i].m_method			||
																vertexElementsA[i].m_usage != vertexElementsB[i].m_usage			||
																vertexElementsA[i].m_usageIndex != vertexElementsB[i].m_usageIndex)
																return FALSE;
														}

														return TRUE;
													}

													return FALSE;
												}

inline INT32									RuGetVertexElementSize(RuVertexElementType elementType)
												{
													switch(elementType)
													{
														case ruELEMENTTYPE_FLOAT1:
														case ruELEMENTTYPE_RUCOLOR:
														case ruELEMENTTYPE_UBYTE4:
														case ruELEMENTTYPE_SHORT2:
														case ruELEMENTTYPE_UBYTE4N:
														case ruELEMENTTYPE_SHORT2N:
														case ruELEMENTTYPE_USHORT2N:
														case ruELEMENTTYPE_FLOAT16_2:
															return 4;

														case ruELEMENTTYPE_FLOAT2:
														case ruELEMENTTYPE_SHORT4:
														case ruELEMENTTYPE_SHORT4N:
														case ruELEMENTTYPE_USHORT4N:
														case ruELEMENTTYPE_FLOAT16_4:
															return 8;

														case ruELEMENTTYPE_FLOAT3:
															return 12;

														case ruELEMENTTYPE_FLOAT4:
															return 16;

														case ruELEMENTTYPE_UDEC3:
														case ruELEMENTTYPE_DEC3N:
															return 0;
													}

													return 0;
												}

// ************************************************************************************************************************************************************

#pragma pack()
#pragma managed

#endif
