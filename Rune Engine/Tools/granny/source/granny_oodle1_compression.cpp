// ========================================================================
// $File: //jeffr/granny/rt/granny_oodle1_compression.cpp $
// $DateTime: 2006/12/18 15:41:05 $
// $Change: 13934 $
// $Revision: #17 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#if !defined(GRANNY_OODLE1_COMPRESSION_H)
#include "granny_oodle1_compression.h"
#endif

// NOTE!  This include has to be here, to avoid the C/C++ include conflict
// that comes from including Jeff's C code.
#include <math.h>

extern "C"
{
#if !defined(VARBITS_H)
#include "varbits.h"
#endif

#if !defined(ARITHBIT_H)
#include "arithbit.h"
#endif

#if !defined(RADLZ_H)
#include "radlz.h"
#endif
};

#if !defined(GRANNY_MEMORY_H)
#include "granny_memory.h"
#endif

#if !defined(GRANNY_ASSERT_H)
#include "granny_assert.h"
#endif

#if !defined(GRANNY_STATISTICS_H)
#include "granny_statistics.h"
#endif

#define DUMP_DATA 0
#if DUMP_DATA
#if !defined(GRANNY_FILE_WRITER_H)
#include "granny_file_writer.h"
#endif

#if !defined(GRANNY_STRING_FORMATTING_H)
#include "granny_string_formatting.h"
#endif
#endif

#if !defined(GRANNY_CPP_SETTINGS_H)
// This should always be the last header included
#include "granny_cpp_settings.h"
#endif

#define GRANNY_OFFSET LARGEST_POSSIBLE_OFFSET

typedef struct LZ_HEADER
{
  U32 max_offset_and_byte;
  U32 uniq_offset_and_byte;
  U32 uniq_lens;
} LZ_HEADER;

USING_GRANNY_NAMESPACE;

BEGIN_GRANNY_NAMESPACE;

struct oodle1_state
{
    int32x ExpectedBufferCount;
    int32x BufferCount;

    LZ_HEADER *Headers;

    void *TempBuffer;
    void *CompressBuffer;
    void *GiantBuffer;

    ARITHBITS ArithBits;
};

END_GRANNY_NAMESPACE;

void GRANNY
AggrOodle1(aggr_allocator &Allocator,
           oodle1_state *&Oodle1State, uint32x ExpandedDataSize,
           int32x BufferCount)
{
    AggrAllocPtr(Allocator, Oodle1State);
    AggrAllocOffsetSizePtr(Allocator, Oodle1State,
                           LZ_compress_temp_size(
                               255, 256,
                               GRANNY_OFFSET),
                           TempBuffer);
    AggrAllocOffsetSizePtr(Allocator, Oodle1State,
                           LZ_compress_alloc_size(
                               255, 256,
                               GRANNY_OFFSET),
                           CompressBuffer);
    AggrAllocOffsetSizePtr(Allocator, Oodle1State,
                           BufferCount*SizeOf(LZ_HEADER) +
                           ((((ExpandedDataSize * 9) + 7) / 8) + 4 + 32),
                           GiantBuffer);
}

void GRANNY
Oodle1Begin(oodle1_state &Oodle1State,
            int32x BufferCount)
{
    Oodle1State.ExpectedBufferCount = BufferCount;
    Oodle1State.BufferCount = 0;
    Oodle1State.Headers = (LZ_HEADER *)Oodle1State.GiantBuffer;
    ZeroArray(Oodle1State.ExpectedBufferCount,
              Oodle1State.Headers);
    ArithBitsPutStart(&Oodle1State.ArithBits,
                      Oodle1State.Headers +
                      Oodle1State.ExpectedBufferCount);
}




oodle1_state *GRANNY
Oodle1BeginSimple(uint32x ExpandedDataSize, int32x BufferCount)
{
    aggr_allocator Allocator;
    InitializeAggrAlloc(Allocator);

    oodle1_state *Oodle1State;
    AggrOodle1(Allocator, Oodle1State, ExpandedDataSize, BufferCount);
    if(EndAggrAlloc(Allocator))
    {
        Oodle1Begin(*Oodle1State, BufferCount);
    }
    return Oodle1State;
}




int32x GRANNY
GetOodle1CompressBufferPaddingSize(void)
{
    return(8);
}

void GRANNY
Oodle1Compress(oodle1_state &Oodle1State,
                 int32x BufferSize, void const *Buffer)
{
    COUNT_BLOCK("Oodle1Compress");

#if DUMP_DATA
    static int32x FileIndex = 0;
    char FileName[256];
    ConvertToStringVar(SizeOf(FileName), FileName, "z:/temp/jeffcrashmyshitdotcom/Oodle1CompressIn %d.raw",
                       FileIndex++);
    file_writer *Writer = NewFileWriter(FileName, true);
    Write(*Writer, BufferSize, Buffer);
    DeleteFileWriter(Writer);
#endif

    Assert(Oodle1State.BufferCount < Oodle1State.ExpectedBufferCount);
    LZC lz = LZ_compress_open(Oodle1State.CompressBuffer,
                              Oodle1State.TempBuffer,
                              255, 256,
                              GRANNY_OFFSET);

    uint8 *i = (uint8 *)Buffer;
    int32x s = BufferSize;
#if 0
    int32x LastS = s;
#endif
    while ( s )
    {
        uint32x len = LZ_compress(lz, &Oodle1State.ArithBits,
                                  i, s );
        s -= len;
        i += len;

#if 0
        // This is for debugging only
        if((LastS - s) > 10000)
        {
            Log(NoteLogMessage, CompressorLogMessage,
                "%d of %d left", s, BufferSize);
            LastS = s;
        }
#endif
    }

    LZ_HEADER &Header = Oodle1State.Headers[Oodle1State.BufferCount++];
    LZ_compress_get_header(lz, &Header);
}

int32x GRANNY
Oodle1End(oodle1_state &Oodle1State, void *&Buffer,
          bool WritingForReversedPlatform)
{
    Assert(Oodle1State.BufferCount <= Oodle1State.ExpectedBufferCount);
    ArithBitsFlush(&Oodle1State.ArithBits);
    Buffer = Oodle1State.GiantBuffer;

    int32x Size = (ArithBitsSize(&Oodle1State.ArithBits) / 8) +
        SizeOf(LZ_HEADER)*Oodle1State.ExpectedBufferCount;

    if (WritingForReversedPlatform)
    {
        Reverse32(Oodle1State.ExpectedBufferCount * SizeOf(LZ_HEADER), Oodle1State.Headers);
    }

#if DUMP_DATA
    static int32x FileIndex = 0;
    char FileName[256];
    ConvertToStringVar(SizeOf(FileName), FileName, "z:/temp/jeffcrashmyshitdotcom/Oodle1CompressOut %d.raw",
                       FileIndex++);
    file_writer *Writer = NewFileWriter(FileName, true);
    Write(*Writer, Size, Buffer);
    DeleteFileWriter(Writer);
#endif

    return(Size);
}


void GRANNY
Oodle1FreeSimple(oodle1_state &Oodle1State)
{
    Deallocate ( &Oodle1State );
}


#if DEBUG_DECOMPRESSOR
static uint8 *DBuffer = 0;
#endif

int32x GRANNY
GetOodle1DecompressBufferPaddingSize(void)
{
    return(4);
}


void GRANNY
Oodle1Decompress(bool FileIsByteReversed,
                 int32x CompressedBytesSize,
                 void const *CompressedBytes,
                 int32x Stop0, int32x Stop1, int32x Stop2,
                 void *DecompressedBytes)
{
    COUNT_BLOCK("Oodle1Decompress w/ stops");

#if DUMP_DATA
    {
        static int32x FileIndex = 0;
        char FileName[256];
        ConvertToStringVar(SizeOf(FileName), FileName, "z:/temp/jeffcrashmyshitdotcom/Oodle1DecompressIn %d - %d %d %d.raw",
                           FileIndex++, Stop0, Stop1, Stop2);
        file_writer *Writer = NewFileWriter(FileName, true);
        Write(*Writer, CompressedBytesSize, CompressedBytes);
        DeleteFileWriter(Writer);
    }
#endif

#if 1
    // Note: this is a fix for when the compressor was not properly
    // flushing hanging bits, so that you need to make sure there are 0's
    // at the end.
    int32x Rounded = int32x(Align32(CompressedBytesSize) - CompressedBytesSize);
    while(Rounded--)
    {
        ((uint8 *)CompressedBytes)[CompressedBytesSize + Rounded] = 0;
    }
#endif

    LZ_HEADER *Headers = (LZ_HEADER *)CompressedBytes;
    if(FileIsByteReversed)
    {
        Reverse32(3*SizeOf(*Headers), Headers);
    }

    ARITHBITS ab;
    ArithBitsGetStart(&ab, (U8 *)(Headers + 3));
    void *Temp = AllocateSize(LZ_decompress_alloc_size(
        255, 256, GRANNY_OFFSET));

#if DEBUG_DECOMPRESSOR
    SetUInt8(Stop2, 0xAA, DecompressedBytes);
    DBuffer = (uint8 *)DecompressedBytes;
#endif

    int32x Size = 0;
    int32x Stops[] = {Stop0, Stop1, Stop2};
    uint8 *To = (uint8 *)DecompressedBytes;
    {for(int32x BlockIndex = 0;
         BlockIndex < 3;
         ++BlockIndex)
    {
        int32x Stop = Stops[BlockIndex];
#if 1
        LZD lz = LZ_decompress_open_from_header(Temp, &Headers[BlockIndex]);
#else
        LZD lz = LZ_decompress_open(Temp, 255, 256, GRANNY_OFFSET);
#endif

        while(Size < Stop)
        {
#if DEBUG_DECOMPRESSOR
            if(Size == (0x5c - 2))
            {
                _asm {int 3};
            }
#endif
            U32 len = LZ_decompress(lz, &ab, To);

            Size += len;
            To += len;
        }
        Assert(Size == Stop);
    }}

    Deallocate(Temp);

#if DUMP_DATA
    {
        static int32x FileIndex = 0;
        char FileName[256];
        ConvertToStringVar(SizeOf(FileName), FileName, "z:/temp/jeffcrashmyshitdotcom/Oodle1DecompressOut %d - %d %d %d.raw",
                           FileIndex++, Stop0, Stop1, Stop2);
        file_writer *Writer = NewFileWriter(FileName, true);
        Write(*Writer, Stop2, DecompressedBytes);
        DeleteFileWriter(Writer);
    }
#endif


#if DEBUG_DECOMPRESSOR
    file_writer *DebugWriter;

    DebugWriter = NewFileWriter("g:/debug/Oodle1Decompress.const", true);
    if(DebugWriter)
    {
        Write(*DebugWriter, SizeOf(VarBitsLens), VarBitsLens);
        Write(*DebugWriter, SizeOf(_bitlevels), _bitlevels);
        Write(*DebugWriter, SizeOf(_bitlevelsalign), &_bitlevelsalign);
        Write(*DebugWriter, SizeOf(bits_invert), bits_invert);
        Write(*DebugWriter, SizeOf(bits_invert_8), bits_invert_8);
        DeleteFileWriter(DebugWriter);
    }

    DebugWriter = NewFileWriter("g:/debug/Oodle1Decompress.in", true);
    if(DebugWriter)
    {
        Write(*DebugWriter, SizeOf(Stops), Stops);
        Write(*DebugWriter, CompressedBytesSize, Headers);
        DeleteFileWriter(DebugWriter);
    }

    DebugWriter = NewFileWriter("g:/debug/Oodle1Decompress.out", true);
    if(DebugWriter)
    {
        Write(*DebugWriter, Stop2, DecompressedBytes);
        DeleteFileWriter(DebugWriter);
    }
#endif
}

void GRANNY
Oodle1Decompress(bool FileIsByteReversed,
                 int32x CompressedBytesSize,
                 void const *CompressedBytes,
                 int32x Stop, void *DecompressedBytes)
{
    COUNT_BLOCK("Oodle1Decompress");

    LZ_HEADER *Header = (LZ_HEADER *)CompressedBytes;
    if(FileIsByteReversed)
    {
        Reverse32(SizeOf(*Header), Header);
    }

    ARITHBITS ab;
    ArithBitsGetStart(&ab, (U8 *)(Header + 1));
    void *Temp = AllocateSize(LZ_decompress_alloc_size(
        255, 256, GRANNY_OFFSET));

    int32x Size = 0;
    uint8 *To = (uint8 *)DecompressedBytes;
    LZD lz = LZ_decompress_open_from_header(Temp, Header);
    while(Size < Stop)
    {
        U32 len = LZ_decompress(lz, &ab, To);

        Size += len;
        To += len;
    }
    Assert(Size == Stop);

    Deallocate(Temp);
}


