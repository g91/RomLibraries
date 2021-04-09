#if !defined(GRANNY_TRACK_MASK_H)
#include "header_preamble.h"
// ========================================================================
// $File: //jeffr/granny/rt/granny_track_mask.h $
// $DateTime: 2006/10/19 16:35:09 $
// $Change: 13632 $
// $Revision: #8 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#if !defined(GRANNY_NAMESPACE_H)
#include "granny_namespace.h"
#endif

#if !defined(GRANNY_TYPES_H)
#include "granny_types.h"
#endif

BEGIN_GRANNY_NAMESPACE EXPGROUP(TrackMaskGroup);

struct track_group;
struct skeleton;

EXPTYPE struct track_mask;
struct track_mask
{
    real32 DefaultWeight;

    int32x BoneCount;
    real32 *BoneWeights;
};

EXPTYPE enum extract_track_mask_result
{
    ExtractTrackMaskResult_AllDataPresent,
    ExtractTrackMaskResult_PartialDataPresent,
    ExtractTrackMaskResult_NoDataPresent
};


EXPAPI GS_SAFE track_mask *NewTrackMask(real32 DefaultWeight, int32x BoneCount);
EXPAPI GS_PARAM extract_track_mask_result ExtractTrackMask(track_mask *TrackMask,
                                                           int32x BoneCount,
                                                           skeleton const &Skeleton,
                                                           char *MaskName,
                                                           real32 DefaultWeight,
                                                           bool UseParentForDefault);
EXPAPI GS_READ real32 GetTrackMaskBoneWeight(track_mask const &Mask,
                                             int32x BoneIndex);
EXPAPI GS_PARAM void SetTrackMaskBoneWeight(track_mask &Mask, int32x BoneIndex,
                                            real32 Weight);
EXPAPI GS_PARAM void FreeTrackMask(track_mask *Mask);

EXPAPI GS_PARAM track_mask *CopyTrackMask(track_mask const &Mask);
EXPAPI GS_PARAM void InvertTrackMask(track_mask &Mask);

EXPAPI GS_PARAM void SetSkeletonTrackMaskFromTrackGroup(track_mask &Mask,
                                                        skeleton const &Skeleton,
                                                        track_group const &TrackGroup,
                                                        real32 IdentityValue,
                                                        real32 ConstantValue,
                                                        real32 AnimatedValue);

EXPAPI GS_PARAM void SetSkeletonTrackMaskChainUpwards(track_mask &Mask,
                                                      skeleton const &Skeleton,
                                                      int32x ChainLeafBoneIndex,
                                                      real32 Weight);
EXPAPI GS_PARAM void SetSkeletonTrackMaskChainDownwards(track_mask &Mask,
                                                        skeleton const &Skeleton,
                                                        int32x ChainRootBoneIndex,
                                                        real32 Weight);

EXPCONST extern track_mask IdentityTrackMask;
EXPCONST extern track_mask NullTrackMask;

END_GRANNY_NAMESPACE;

#include "header_postfix.h"
#define GRANNY_TRACK_MASK_H
#endif
