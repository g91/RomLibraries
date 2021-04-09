#if !defined(GRANNY_OODLE1_COMPRESSION_H)
#include "header_preamble.h"
// ========================================================================
// $File: //jeffr/granny/rt/granny_oodle1_compression.h $
// $DateTime: 2006/12/18 15:41:05 $
// $Change: 13934 $
// $Revision: #10 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#if !defined(GRANNY_NAMESPACE_H)
#include "granny_namespace.h"
#endif

#if !defined(GRANNY_TYPES_H)
#include "granny_types.h"
#endif

BEGIN_GRANNY_NAMESPACE EXPGROUP(FileCompressionGroup);

EXPTYPE struct oodle1_state;
struct aggr_allocator;

void AggrOodle1(aggr_allocator &Allocator,
                oodle1_state *&Oodle1State, uint32x ExpandedDataSize,
                int32x BufferCount);
void Oodle1Begin(oodle1_state &Oodle1State, int32x BufferCount);

EXPAPI GS_SAFE oodle1_state *Oodle1BeginSimple(uint32x ExpandedDataSize, int32x BufferCount);

// Buffer must be padded at the end by at least the number of bytes
// returned by GetOodle1CompressBufferPaddingSize(void) or the
// compressor will read-fault off the end.
EXPAPI GS_SAFE int32x GetOodle1CompressBufferPaddingSize(void);
EXPAPI GS_PARAM void Oodle1Compress(oodle1_state &Oodle1State,
                                    int32x BufferSize, void const *Buffer);

EXPAPI GS_PARAM int32x Oodle1End(oodle1_state &Oodle1State,
                                 void *&Buffer,
                                 bool WritingForReversedPlatform);

// Only use this if you called Oodle1BeginSimple.
EXPAPI GS_PARAM void Oodle1FreeSimple(oodle1_state &Oodle1State);


// CompressedBytes must be padded at the end by at least the number of
// bytes returned by GetOodle1DecompressBufferPaddingSize(void) or
// the compressor will read-fault off the end.
EXPAPI GS_SAFE int32x GetOodle1DecompressBufferPaddingSize(void);
void Oodle1Decompress(bool FileIsByteReversed,
                      int32x CompressedBytesSize,
                      void const *CompressedBytes,
                      int32x Stop0, int32x Stop1, int32x Stop2,
                      void *DecompressedBytes);
EXPAPI GS_PARAM void Oodle1Decompress(bool FileIsByteReversed,
                                      int32x CompressedBytesSize,
                                      void const *CompressedBytes,
                                      int32x DecompressedSize,
                                      void *DecompressedBytes);

END_GRANNY_NAMESPACE;

#include "header_postfix.h"
#define GRANNY_OODLE1_COMPRESSION_H
#endif
