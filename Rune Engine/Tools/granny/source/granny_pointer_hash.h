#if !defined(GRANNY_POINTER_HASH_H)
#include "header_preamble.h"
// ========================================================================
// $File: //jeffr/granny/rt/granny_pointer_hash.h $
// $DateTime: 2006/12/21 17:45:34 $
// $Change: 14013 $
// $Revision: #5 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#if !defined(GRANNY_NAMESPACE_H)
#include "granny_namespace.h"
#endif

#if !defined(GRANNY_TYPES_H)
#include "granny_types.h"
#endif

BEGIN_GRANNY_NAMESPACE;

EXPTYPE struct pointer_hash;

EXPAPI pointer_hash *NewPointerHash(void);
EXPAPI void DeletePointerHash(pointer_hash *Hash);

EXPAPI void AddPointerToHash(pointer_hash &Hash, void const *Key, void *Data);
EXPAPI bool SetHashedPointerData(pointer_hash &Hash, void const *Key, void *Data);
EXPAPI bool GetHashedPointerData(pointer_hash &Hash, void const *Key, void *&Data);
EXPAPI bool HashedPointerKeyExists(pointer_hash &Hash, void const *Key);

END_GRANNY_NAMESPACE;

#include "header_postfix.h"
#define GRANNY_POINTER_HASH_H
#endif
