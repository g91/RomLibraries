#if !defined(GRANNY_FLOATS_H)
#include "header_preamble.h"
// ========================================================================
// $File: //jeffr/granny/rt/granny_floats.h $
// $DateTime: 2006/10/16 14:57:23 $
// $Change: 13583 $
// $Revision: #6 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#if !defined(GRANNY_TYPES_H)
#include "granny_types.h"
#endif

BEGIN_GRANNY_NAMESPACE;

uint32 const system_types_CastToInfinity32 = 0x7f800000;
uint32 const system_types_CastToQuietNaN32 = 0xffc00000;
uint32 const system_types_CastToSignalingNaN32 = 0xff800000;
uint32 const system_types_CastToInfinity64[] = {0x00000000, 0x7ff00000};
uint32 const system_types_CastToQuietNaN64[] = {0x00000000, 0xfff80000};
uint32 const system_types_CastToSignalingNaN64[] = {0x00000000, 0xfff00000};


uint32 const system_types_AlmostInfinity32 = 0x7f0fffff;

#if 0
// This doesn't work on VC7 (at least) - the linker gets it all horribly wrong.

real32 const Real32Infinity = *(real32 *)&system_types_CastToInfinity32;
real32 const Real32QuietNaN = *(real32 *)&system_types_CastToQuietNaN32;
real32 const Real32SignalingNaN = *(real32 *)&system_types_CastToSignalingNaN32;
real32 const Real32AlmostInfinity = *(real32 *)&system_types_AlmostInfinity32;
real64x const Real64Infinity = *(real64x *)&system_types_CastToInfinity64;
real64x const Real64QuietNaN = *(real64x *)&system_types_CastToQuietNaN64;
real64x const Real64SignalingNaN = *(real64x *)&system_types_CastToSignalingNaN64;

#else

// Remember! These don't exist on PS2, and it's unknown if they work properly on SSE
// (and also remember that with certain compiler options, on the PC/XBox, _all_ floating-point ops use SSE)
#define Real32Infinity (*(real32 *)&system_types_CastToInfinity32)
#define Real32QuietNaN (*(real32 *)&system_types_CastToQuietNaN32)
#define Real32SignalingNaN (*(real32 *)&system_types_CastToSignalingNaN32)

// In practice it's probably better to use these.
#define Real32AlmostInfinity (*(real32 *)&system_types_AlmostInfinity32)

// Again, remember that 64-bit floats on PS2 are evil.
#define Real64Infinity (*(real64x *)&system_types_CastToInfinity64)
#define Real64QuietNaN (*(real64x *)&system_types_CastToQuietNaN64)
#define Real64SignalingNaN (*(real64x *)&system_types_CastToSignalingNaN64)

#endif


END_GRANNY_NAMESPACE;

#include "header_postfix.h"
#define GRANNY_FLOATS_H
#endif
