// ========================================================================
// $File: //jeffr/granny/rt/x86/x86_granny_accelerated_deformers.cpp $
// $DateTime: 2006/10/16 14:57:23 $
// $Change: 13583 $
// $Revision: #10 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#include <xmmintrin.h>

#if !defined(GRANNY_ACCELERATED_DEFORMERS_H)
#include "granny_accelerated_deformers.h"
#endif

#if !defined(GRANNY_VERTEX_DATA_H)
#include "granny_vertex_data.h"
#endif

#if !defined(GRANNY_DEFORMERS_H)
#include "granny_deformers.h"
#endif

#if !defined(GRANNY_GENERIC_DEFORMERS_H)
#include "granny_generic_deformers.h"
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

#if !defined(X86_GRANNY_CPU_QUERIES_H)
#include "x86_granny_cpu_queries.h"
#endif

#if !defined(GRANNY_STATISTICS_H)
#include "granny_statistics.h"
#endif

#if !defined(GRANNY_CPP_SETTINGS_H)
// This should always be the last header included
#include "granny_cpp_settings.h"
#endif

USING_GRANNY_NAMESPACE;

#define DEFINE_CODE 1

#define _Stringize(Name) #Name
#define Stringize(Name) _Stringize(Name)

/*
 * Note: the matrix_4x4 Transforms are assumed to be 16 byte aligned for all the
 *  routines below. You'll crash rather quickly if they aren't.
 */

/*
 * 16 byte aligned structure to help hold intermediate results and
 *  our transform pointers.
 */
typedef struct _SSEABUF
{
    __m128 DestPosXYZ;
    __m128 DestNormXYZ;

    matrix_4x4 const *Transforms;
    int32x const *TransformTable;
} SSEABUF;

// Weight conversion multiplier.
static const __m128 OneOver255 = { 1.0f/255.0f, 1.0f/255.0f, 1.0f/255.0f, 1.0f/255.0f };

/*
 * Include the direct versions of our sse helper routines.
 */
#define DIRECT_ROUTINES
#include "x86_granny_accelerated_deformers_custom.inl"

/*
 * Include the indirect versions of our sse helper routines.
 */
#undef DIRECT_ROUTINES
#include "x86_granny_accelerated_deformers_custom.inl"

#define LogSSEPunt(T, S, D)                                         \
{                                                                   \
    static bool Once = false;                                       \
    if(Once)                                                        \
    {                                                               \
        Log3(ErrorLogMessage, DeformerLogMessage,                   \
             "SSE routine bypassed because the following pointers " \
             "were not aligned: %s%s%s  (this message will only "   \
             "appear once)",                                        \
             IS_ALIGNED_16(T) ? "" : "MatrixBuffer4x4 ",            \
             IS_ALIGNED_16(S) ? "" : "SourceVertices ",             \
             IS_ALIGNED_16(D) ? "" : "DestVertices ");              \
                                                                    \
        Once = false;                                               \
    }                                                               \
}

#define BONE_COUNT 0
#include "x86_granny_accelerated_deformer_wrapper.inl"

#define BONE_COUNT 1
#include "x86_granny_accelerated_deformer_wrapper.inl"

#define BONE_COUNT 2
#include "x86_granny_accelerated_deformer_wrapper.inl"

#define BONE_COUNT 4
#include "x86_granny_accelerated_deformer_wrapper.inl"

void GRANNY
AddAcceleratedDeformers(void)
{
}
