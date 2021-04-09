// ========================================================================
// $File: //jeffr/granny/rt/granny_track_group_sampler.cpp $
// $DateTime: 2006/10/16 14:57:23 $
// $Change: 13583 $
// $Revision: #4 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#if !defined(GRANNY_TRACK_GROUP_SAMPLER_H)
#include "granny_track_group_sampler.h"
#endif

#if !defined(GRANNY_MEMORY_H)
#include "granny_memory.h"
#endif

#if !defined(GRANNY_ASSERT_H)
#include "granny_assert.h"
#endif

#if !defined(GRANNY_LOG_H)
#include "granny_log.h"
#endif

#if !defined(GRANNY_CPP_SETTINGS_H)
// This should always be the last header included
#include "granny_cpp_settings.h"
#endif

USING_GRANNY_NAMESPACE;

BEGIN_GRANNY_NAMESPACE;

struct track_group_sampler
{
    int32x TransformCurveCount;

    int32x MaxSampleCount;
    int32x SampleCount;

    int32x PositionSampleCount;
    real32 *PositionSamples;

    int32x OrientationSampleCount;
    real32 *OrientationSamples;

    int32x ScaleShearSampleCount;
    real32 *ScaleShearSamples;
};

END_GRANNY_NAMESPACE;

track_group_sampler *GRANNY
BeginSampledAnimation(int32x TransformCurveCount,
                      int32x SampleCount)
{
    aggr_allocator Allocator;
    InitializeAggrAlloc(Allocator);

    track_group_sampler *Sampler;
    AggrAllocPtr(Allocator, Sampler);
    AggrAllocOffsetArrayPtr(Allocator, Sampler, SampleCount * TransformCurveCount * 3,
                            PositionSampleCount, PositionSamples);
    AggrAllocOffsetArrayPtr(Allocator, Sampler, SampleCount * TransformCurveCount * 4,
                            OrientationSampleCount, OrientationSamples);
    AggrAllocOffsetArrayPtr(Allocator, Sampler, SampleCount * TransformCurveCount * 9,
                            ScaleShearSampleCount, ScaleShearSamples);

    if(EndAggrAlloc(Allocator))
    {
        Sampler->TransformCurveCount = TransformCurveCount;
        Sampler->MaxSampleCount = SampleCount;
        Sampler->SampleCount = 0;
    }

    return(Sampler);
}

void GRANNY
EndSampledAnimation(track_group_sampler *Sampler)
{
    Deallocate(Sampler);
}

real32 *GRANNY
GetPositionSamples(track_group_sampler &Sampler, int32x TrackIndex)
{
    return(Sampler.PositionSamples +
           TrackIndex * (Sampler.MaxSampleCount * 3));
}

real32 *GRANNY
GetOrientationSamples(track_group_sampler &Sampler, int32x TrackIndex)
{
    return(Sampler.OrientationSamples +
           TrackIndex * (Sampler.MaxSampleCount * 4));
}

real32 *GRANNY
GetScaleShearSamples(track_group_sampler &Sampler, int32x TrackIndex)
{
    return(Sampler.ScaleShearSamples +
           TrackIndex * (Sampler.MaxSampleCount * 9));
}

void GRANNY
SetTransformSample(track_group_sampler &Sampler,
                   int32x TrackIndex,
                   real32 const *Position3,
                   real32 const *Orientation4,
                   real32 const *ScaleShear3x3)
{
    Assert(TrackIndex < Sampler.TransformCurveCount);
    Assert(Sampler.SampleCount < Sampler.MaxSampleCount);

    real32 *PositionSamples = GetPositionSamples(Sampler, TrackIndex);
    real32 *OrientationSamples = GetOrientationSamples(Sampler, TrackIndex);
    real32 *ScaleShearSamples = GetScaleShearSamples(Sampler, TrackIndex);

    Copy32(3, Position3, &PositionSamples[Sampler.SampleCount * 3]);
    Copy32(4, Orientation4, &OrientationSamples[Sampler.SampleCount * 4]);
    Copy32(9, ScaleShear3x3, &ScaleShearSamples[Sampler.SampleCount * 9]);
}

void GRANNY
PushSampledFrame(track_group_sampler &Sampler)
{
    Assert(Sampler.SampleCount < Sampler.MaxSampleCount);
    ++Sampler.SampleCount;
}
