#if !defined(GRANNY_BSPLINE_BUFFERS_H)
#include "header_preamble.h"
// ========================================================================
// $File: //jeffr/granny/rt/granny_bspline_buffers.h $
// $DateTime: 2006/10/16 14:57:23 $
// $Change: 13583 $
// $Revision: #11 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#if !defined(GRANNY_NAMESPACE_H)
#include "granny_namespace.h"
#endif

#if !defined(GRANNY_TYPES_H)
#include "granny_types.h"
#endif

BEGIN_GRANNY_NAMESPACE EXPGROUP(BSplineBufferGroup);

struct curve2;

EXPAPI GS_READ int32x FindKnot(int32x KnotCount, real32 const *Knots, real32 t);
EXPAPI GS_READ int32x FindCloseKnot(int32x KnotCount, real32 const *Knots, real32 t, int32x StartingIndex);

// And the 8 and 16-bit versions.
int32x FindKnotUint16(int32x KnotCount, uint16 const *Knots, uint16 t);
int32x FindKnotUint8 (int32x KnotCount, uint8  const *Knots, uint8  t);
int32x FindCloseKnotUint16(int32x KnotCount, uint16 const *Knots, uint16 t, int32x StartingIndex);
int32x FindCloseKnotUint8 (int32x KnotCount, uint8  const *Knots, uint8  t, int32x StartingIndex);

EXPAPI GS_READ bool ConstructBSplineBuffers(int32x Dimension,
                                            curve2 const *PrevCurve,
                                            curve2 const &Curve,
                                            curve2 const *NextCurve,
                                            real32 PrevCurveDuration,
                                            real32 CurveDuration,
                                            real32 NextCurveDuration,
                                            int32x KnotIndex,
                                            real32 *ti, real32 *pi,
                                            real32 *&tiPtr, real32 *&piPtr,
                                            real32 const *IdentityVector);

EXPAPI GS_READ void EvaluateCurveAtT(int32x Dimension, bool Normalize,
                                     bool BackwardsLoop,
                                     curve2 const &Curve,
                                     bool ForwardsLoop,
                                     real32 CurveDuration,
                                     real32 t, real32 *Result,
                                     real32 const *IdentityVector);

EXPAPI GS_READ void EvaluateCurveAtKnotIndex(int32x Dimension, bool Normalize,
                                             bool BackwardsLoop,
                                             curve2 const &Curve,
                                             bool ForwardsLoop,
                                             real32 CurveDuration,
                                             int32x KnotIndex, real32 t,
                                             real32 *Result,
                                             real32 const *IdentityVector);

END_GRANNY_NAMESPACE;

#include "header_postfix.h"
#define GRANNY_BSPLINE_BUFFERS_H
#endif
