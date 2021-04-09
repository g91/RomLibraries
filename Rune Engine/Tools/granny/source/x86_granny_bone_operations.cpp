// ========================================================================
// $File: //jeffr/granny/rt/x86/x86_granny_bone_operations.cpp $
// $DateTime: 2006/10/16 14:57:23 $
// $Change: 13583 $
// $Revision: #7 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#if !defined(GRANNY_BONE_OPERATIONS_H)
#include "granny_bone_operations.h"
#endif

#if !defined(GRANNY_TRANSFORM_H)
#include "granny_transform.h"
#endif

#if !defined(GRANNY_MATH_H)
#include "granny_math.h"
#endif

#if !defined(X86_GRANNY_CPU_QUERIES_H)
#include "x86_granny_cpu_queries.h"
#endif

#if !defined(GRANNY_CPP_SETTINGS_H)
// This should always be the last header included
#include "granny_cpp_settings.h"
#endif

USING_GRANNY_NAMESPACE;

#if !FORCE_SSE_VERSION

static void
SetPointers(void)
{
    BuildIdentityWorldPoseComposite = BuildIdentityWorldPoseComposite_Generic;
    BuildPositionWorldPoseComposite = BuildPositionWorldPoseComposite_Generic;
    BuildPositionOrientationWorldPoseComposite = BuildPositionOrientationWorldPoseComposite_Generic;
    BuildFullWorldPoseComposite = BuildFullWorldPoseComposite_Generic;

    BuildIdentityWorldPoseOnly = BuildIdentityWorldPoseOnly_Generic;
    BuildPositionWorldPoseOnly = BuildPositionWorldPoseOnly_Generic;
    BuildPositionOrientationWorldPoseOnly = BuildPositionOrientationWorldPoseOnly_Generic;
    BuildFullWorldPoseOnly = BuildFullWorldPoseOnly_Generic;

    BuildSingleCompositeFromWorldPose = BuildSingleCompositeFromWorldPose_Generic;
    BuildSingleCompositeFromWorldPoseTranspose = BuildSingleCompositeFromWorldPoseTranspose_Generic;
}

static void
BuildIdentityWorldPoseComposite_Stub(real32 const *ParentMatrix,
                                  real32 const *InverseWorld4x4,
                                  real32 *Result,
                                  real32 *ResultWorldMatrix)
{
    SetPointers();
    BuildIdentityWorldPoseComposite(ParentMatrix,
                                 InverseWorld4x4,
                                 Result,
                                 ResultWorldMatrix);
}

static void
BuildPositionWorldPoseComposite_Stub(real32 const *Position,
                                  real32 const *ParentMatrix,
                                  real32 const *InverseWorld4x4,
                                  real32 *Result, real32 *ResultWorldMatrix)
{
    SetPointers();
    BuildPositionWorldPoseComposite(Position, ParentMatrix,
                                 InverseWorld4x4, Result,
                                 ResultWorldMatrix);
}

static void
BuildPositionOrientationWorldPoseComposite_Stub(real32 const *Position,
                                             real32 const *Orientation,
                                             real32 const *ParentMatrix,
                                             real32 const *InverseWorld4x4,
                                             real32 *Result,
                                             real32 * ResultWorldMatrix)
{
    SetPointers();
    BuildPositionOrientationWorldPoseComposite(Position,
                                            Orientation,
                                            ParentMatrix,
                                            InverseWorld4x4,
                                            Result,
                                            ResultWorldMatrix);
}

static void
BuildFullWorldPoseComposite_Stub(transform const &Transform,
                              real32 const *ParentMatrix,
                              real32 const *InverseWorld4x4,
                              real32 *Result,
                              real32 * ResultWorldMatrix)
{
    SetPointers();
    BuildFullWorldPoseComposite(Transform, ParentMatrix,
                             InverseWorld4x4, Result, ResultWorldMatrix);
}

static void
BuildIdentityWorldPoseOnly_Stub(real32 const *ParentMatrix,
                                real32 *ResultWorldMatrix)
{
    SetPointers();
    BuildIdentityWorldPoseOnly(ParentMatrix,
                               ResultWorldMatrix);
}

static void
BuildPositionWorldPoseOnly_Stub(real32 const *Position,
                                real32 const *ParentMatrix,
                                real32 *ResultWorldMatrix)
{
    SetPointers();
    BuildPositionWorldPoseOnly(Position, ParentMatrix,
                               ResultWorldMatrix);
}

static void
BuildPositionOrientationWorldPoseOnly_Stub(real32 const *Position,
                                           real32 const *Orientation,
                                           real32 const *ParentMatrix,
                                           real32 * ResultWorldMatrix)
{
    SetPointers();
    BuildPositionOrientationWorldPoseOnly(Position,
                                          Orientation,
                                          ParentMatrix,
                                          ResultWorldMatrix);
}

static void
BuildFullWorldPoseOnly_Stub(transform const &Transform,
                            real32 const *ParentMatrix,
                            real32 * ResultWorldMatrix)
{
    SetPointers();
    BuildFullWorldPoseOnly(Transform, ParentMatrix, ResultWorldMatrix);
}

static void
BuildSingleCompositeFromWorldPose_Stub(real32 const *InverseWorld4x4,
                                       real32 const *WorldMatrix,
                                       real32 *ResultComposite)
{
    SetPointers();
    BuildSingleCompositeFromWorldPose(InverseWorld4x4,
                                      WorldMatrix,
                                      ResultComposite);
}

static void
BuildSingleCompositeFromWorldPoseTranspose_Stub(real32 const *InverseWorld4x4,
                                                real32 const *WorldMatrix,
                                                real32 *ResultComposite)
{
    SetPointers();
    BuildSingleCompositeFromWorldPoseTranspose(InverseWorld4x4,
                                               WorldMatrix,
                                               ResultComposite);
}


#define STUB_POINTER(Name) \
OPTIMIZED_DISPATCH(Name) = (OPTIMIZED_DISPATCH_TYPE(Name) *)Name##_Stub

BEGIN_GRANNY_NAMESPACE;

STUB_POINTER(BuildIdentityWorldPoseComposite);
STUB_POINTER(BuildPositionWorldPoseComposite);
STUB_POINTER(BuildPositionOrientationWorldPoseComposite);
STUB_POINTER(BuildFullWorldPoseComposite);

STUB_POINTER(BuildIdentityWorldPoseOnly);
STUB_POINTER(BuildPositionWorldPoseOnly);
STUB_POINTER(BuildPositionOrientationWorldPoseOnly);
STUB_POINTER(BuildFullWorldPoseOnly);

STUB_POINTER(BuildSingleCompositeFromWorldPose);
STUB_POINTER(BuildSingleCompositeFromWorldPoseTranspose);

END_GRANNY_NAMESPACE;
#endif
