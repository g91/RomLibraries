#if !defined(GRANNY_PARAMETER_CHECKING_H)
#include "header_preamble.h"
// ========================================================================
// $File: //jeffr/granny/rt/granny_parameter_checking.h $
// $DateTime: 2007/01/12 10:25:10 $
// $Change: 14184 $
// $Revision: #11 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#if !defined(GRANNY_NAMESPACE_H)
#include "granny_namespace.h"
#endif

#if !defined(GRANNY_LOG_H)
#include "granny_log.h"
#endif

BEGIN_GRANNY_NAMESPACE;

// TODO: do {} while(false) all of these to get rid of multiple Value evaluations
#define CheckBoundedInt32(Min, Value, Max, Return)  \
if(((Value) < (Min)) || ((Value) > (Max))) { \
    Log3(ErrorLogMessage, SubsystemCode, \
        #Value " %d is out of range [%d, %d]", Value, Min, Max); \
        Return; }

#define CheckBoundedFloat32(Min, Value, Max, Return) \
if(((Value) < (Min)) || ((Value) > (Max))) { \
    Log3(ErrorLogMessage, SubsystemCode, \
        #Value " %f is out of range [%f, %f]", Value, Min, Max); \
        Return; }

#define CheckCountedInt32(Value, Count, Return) \
if(((Value) < 0) || ((Value) >= (Count))) { \
    Log2(ErrorLogMessage, SubsystemCode, \
        #Value " %d is out of range [0, %d)", Value, Count); \
        Return; }

#define CheckCountedUInt32(Value, Count, Return) \
    if ((Value) >= (Count)) {                                       \
        Log2(ErrorLogMessage, SubsystemCode,                        \
             #Value " %d is out of range [0, %d)", Value, Count);   \
        Return; }

#define CheckEqualInt32(Value, MustBe, Return) \
if((Value) != (MustBe)) { \
    Log2(ErrorLogMessage, SubsystemCode, \
        #Value " %d must be %d", Value, MustBe); \
        Return; }

#define CheckPointerNotNull(Pointer, Return) \
if(!(Pointer)) { \
    Log0(ErrorLogMessage, SubsystemCode, \
        #Pointer " is not allowed to be 0"); \
        Return; }

#define CheckPointersEqual(Pointer1, Pointer2, Return) \
if((Pointer1)!=(Pointer2)) { \
    Log0(ErrorLogMessage, SubsystemCode, \
        #Pointer1 " must equal " #Pointer2); \
        Return; }

#define CheckCondition(Condition, Return) \
if(!(Condition)) { \
    Log0(ErrorLogMessage, SubsystemCode, \
        #Condition " was not true"); \
        Return; }

#define CheckConvertToInt32(Target, Value, Return)                                      \
    do {                                                                                \
        int64 storValue = Value;                                                        \
        if (storValue < (int64)(Int32Minimum) || storValue > (int64)(Int32Maximum)) {   \
            Log1(ErrorLogMessage, SubsystemCode,                                        \
                 #Value " %I64d is out of range for an int32 " #Target, storValue);     \
            Return;                                                                     \
        } else {                                                                        \
            (Target) = (int32)storValue;                                                \
        }                                                                               \
    } while (false)

#define CheckConvertToInt16(Target, Value, Return)                                      \
    do {                                                                                \
        int64 storValue = Value;                                                        \
        if (storValue < (int64)(Int16Minimum) || storValue > (int64)(Int16Maximum)) {   \
            Log1(ErrorLogMessage, SubsystemCode,                                        \
                 #Value " %I64d is out of range for an int16 " #Target, storValue);     \
            Return;                                                                     \
        } else {                                                                        \
            (Target) = (int16)storValue;                                                \
        }                                                                               \
    } while (false)

#define CheckConvertToUInt32(Target, Value, Return)                                         \
    do {                                                                                    \
        uint64 storValue = Value;                                                           \
        if (storValue > (uint64)(UInt32Maximum)) {                                          \
            Log1(ErrorLogMessage, SubsystemCode,                                            \
                 #Value " %I64d is out of range for an uint32 " #Target, storValue);        \
            Return;                                                                         \
        } else {                                                                            \
            (Target) = (uint32)storValue;                                                   \
        }                                                                                   \
    } while (false)


END_GRANNY_NAMESPACE;

#include "header_postfix.h"
#define GRANNY_PARAMETER_CHECKING_H
#endif
