#if !defined(GRANNY_MEMORY_ARENA_H)
#include "header_preamble.h"
// ========================================================================
// $File: //jeffr/granny/rt/granny_memory_arena.h $
// $DateTime: 2006/10/16 14:57:23 $
// $Change: 13583 $
// $Revision: #4 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#if !defined(GRANNY_NAMESPACE_H)
#include "granny_namespace.h"
#endif

#if !defined(GRANNY_TYPES_H)
#include "granny_types.h"
#endif

#if !defined(GRANNY_MEMORY_H)
#include "granny_memory.h"
#endif

BEGIN_GRANNY_NAMESPACE EXPGROUP(MemoryArenaGroup);

#define ArenaPush(Arena, type) (type *)MemoryArenaPush(Arena, SizeOf(type))
#define ArenaPushSize(Arena, Size) MemoryArenaPush(Arena, Size)
#define ArenaPushArray(Arena, Count, type) (type *)MemoryArenaPush(Arena, (Count) * SizeOf(type))

EXPTYPE struct memory_arena;

EXPAPI GS_SAFE memory_arena *NewMemoryArena(void);
EXPAPI GS_PARAM void ClearArena(memory_arena *Arena);
EXPAPI GS_PARAM void FreeMemoryArena(memory_arena *Arena);

EXPAPI GS_PARAM void *MemoryArenaPush(memory_arena &Arena, int32x Size);

char *ArenaPushString(memory_arena &Arena, char const *String);
void *ArenaPushBinary(memory_arena &Arena, int32x Size, void const *Data);

END_GRANNY_NAMESPACE;

#include "header_postfix.h"
#define GRANNY_MEMORY_ARENA_H
#endif
