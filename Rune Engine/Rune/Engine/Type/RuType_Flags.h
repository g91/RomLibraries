#ifndef _RUTYPEFLAGS_H_
#define _RUTYPEFLAGS_H_

#pragma unmanaged

// Device buffer lock flags (D3D9-equivalent)
#define ruLOCK_DISCARD					0x00002000UL
#define ruLOCK_READONLY					0x00000010UL
#define ruLOCK_WRITEONLY				0x00000001UL
#define ruLOCK_NOOVERWRITE				0x00001000UL

// Stream format flags
#define ruVERTEXFORMAT_POSITION			0x00000001
#define ruVERTEXFORMAT_BLENDWEIGHT		0x00000002
#define ruVERTEXFORMAT_BLENDINDICES		0x00000004
#define ruVERTEXFORMAT_NORMAL			0x00000008
#define ruVERTEXFORMAT_POINTSIZE		0x00000010
#define ruVERTEXFORMAT_DIFFUSE			0x00000020
#define ruVERTEXFORMAT_SPECULAR			0x00000040
#define ruVERTEXFORMAT_INVALID			0xFFFFFFFF								// This combination is invalid

// Shader cap flags
#define ruSHADERCAP_TWEENING			0x00000001								// Shader supports tweening
#define ruSHADERCAP_SKINNING			0x00000002								// Shader supports skinning
#define ruSHADERCAP_LIGHT_POINT			0x00000004								// Shader supports point lights
#define ruSHADERCAP_LIGHT_SPOT			0x00000008								// Shader supports spot lights
#define ruSHADERCAP_LIGHT_DIRECTIONAL	0x00000010								// Shader supports directional lights

#define ruSHADERCAP_LIGHTS				0x0000001C								// Combination of all light flags

#pragma managed

#endif
