// ========================================================================
// $File: //jeffr/granny/rt/granny_platform_convert.cpp $
// $DateTime: 2006/12/18 13:25:32 $
// $Change: 13931 $
// $Revision: #11 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================
// =================
//  Todo: Save string to last?
// =================

#if !defined(GRANNY_PLATFORM_CONVERT_H)
#include "granny_platform_convert.h"
#endif

#if !defined(GRANNY_FILE_H)
#include "granny_file.h"
#endif

#if !defined(GRANNY_ASSERT_H)
#include "granny_assert.h"
#endif

#if !defined(GRANNY_MEMORY_H)
#include "granny_memory.h"
#endif

#if !defined(GRANNY_MEMORY_ARENA_H)
#include "granny_memory_arena.h"
#endif

#if !defined(GRANNY_TRANSFORM_H)
#include "granny_transform.h"
#endif

#if !defined(GRANNY_STACK_ALLOCATOR_H)
#include "granny_stack_allocator.h"
#endif

#if !defined(GRANNY_OODLE1_COMPRESSION_H)
#include "granny_oodle1_compression.h"
#endif

#if !defined(GRANNY_PARAMETER_CHECKING_H)
#include "granny_parameter_checking.h"
#endif

#if !defined(GRANNY_CPP_SETTINGS_H)
// This should always be the last header included
#include "granny_cpp_settings.h"
#endif

#define SubsystemCode DataTypeLogMessage

USING_GRANNY_NAMESPACE;

inline int32x
ReferenceDifference(grn_reference &A,
                    grn_reference &B)
{
    // Be paranoid.  These are uint32s, so naive subtraction might not
    // give the right results in extreme cases.
    if (A.SectionIndex < B.SectionIndex)
    {
        return -1;
    }
    else if (A.SectionIndex > B.SectionIndex)
    {
        return 1;
    }
    else
    {
        if (A.Offset < B.Offset)
            return -1;
        else if (A.Offset > B.Offset)
            return 1;
        else
            return 0;
    }
}



#define CONTAINER_NAME fixup_remap_container
#define CONTAINER_ITEM_TYPE fixup_remap
#define CONTAINER_ADD_FIELDS grn_reference const &Key, temp_grn_reference const &Data
#define CONTAINER_ADD_ASSIGN(Item) (Item)->OldLocation = Key; (Item)->NewLocation = Data;
#define CONTAINER_COMPARE_ITEMS(Item1, Item2) ReferenceDifference((Item1)->OldLocation, (Item2)->OldLocation)
#define CONTAINER_FIND_FIELDS grn_reference Ref
#define CONTAINER_COMPARE_FIND_FIELDS(Item) ReferenceDifference(Ref, (Item)->OldLocation)
#define CONTAINER_SORTED 1
#define CONTAINER_KEEP_LINKED_LIST 0
#define CONTAINER_SUPPORT_DUPES 0
#include "granny_contain.inl"



BEGIN_GRANNY_NAMESPACE;

// We need to be able to measure the size of a datatype definition on
// either platform.  We check the size with the compile assert below
// for the current platform.
int32x const DataTypeDefBaseSize = (SizeOfMember(data_type_definition, Type) +
                                    SizeOfMember(data_type_definition, ArrayWidth) +
                                    SizeOfMember(data_type_definition, Extra));

// Account for the two pointers and the uintaddr field
int32x const DataTypeDefSize32 = DataTypeDefBaseSize + 2 * SizeOf(uint32) + SizeOf(uint32);
int32x const DataTypeDefSize64 = DataTypeDefBaseSize + 2 * SizeOf(uint64) + SizeOf(uint64);

int32x const DataTypeNameOffset32          = 4;
int32x const DataTypeReferenceTypeOffset32 = 8;
int32x const DataTypeNameOffset64          = 4;
int32x const DataTypeReferenceTypeOffset64 = 12;

// Do as much checking as we can...
CompileAssert(SizeOf(uintaddrx) == SizeOf(void*));
CompileAssert(SizeOf(data_type_definition) == (DataTypeDefBaseSize + 3 * SizeOf(void*)));
CompileAssert(OffsetFromType(data_type_definition, Name) == DataTypeNameOffset32 ||
              OffsetFromType(data_type_definition, Name) == DataTypeNameOffset64);
CompileAssert(OffsetFromType(data_type_definition, ReferenceType) == DataTypeReferenceTypeOffset32 ||
              OffsetFromType(data_type_definition, ReferenceType) == DataTypeReferenceTypeOffset64);

// ---------------------------------------------------------------------
// Types and enums.  Note that the convention here is that any
// structure prefixed with temp_ is a reference to the unflattened
// sections that are constructed by traversing the tree.

enum sub_section_index
{
    Index32  = 0,
    Index16  = 1,
    IndexAny = 2,

    NumSubsections = 3
};

struct temp_section
{
    uint32x Size[NumSubsections];
    uint32x Offsets[NumSubsections];
    uint8*  Pointer[NumSubsections];

    uint8* Flattened;
    int32x TotalSize;

    uint32x NumFixups;
    uint32x NumMarshalls;
};

// Handy utility structure to pass around to conversion functions.
// Basically holds everything that we need or have to alloc during the
// conversion
struct conversion_state
{
    bool CopyRequiresSwap;
    file*  File;
    grn_pointer_fixup **SectionFixups;

    memory_arena   *Arena;

    fixup_remap_container* FixupRemaps;
    stack_allocator PointerFixups;
    stack_allocator MixedMarshalling;
    int32x NumFileSections;
    temp_section* NewSections;

    int32x SourcePointerSize;
    int32x SourcePointerSizeInBytes;
    bool   SourceIsLittleEndian;
    int32x DestPointerSize;
    int32x DestPointerSizeInBytes;
    bool   DestIsLittleEndian;

    int32x SourceDataTypeDefSize;
    int32x SourceDataTypeNameOffset;
    int32x SourceDataTypeReferenceTypeOffset;

    int32x DestDataTypeDefSize;
    int32x DestDataTypeNameOffset;
    int32x DestDataTypeReferenceTypeOffset;

    bool ExcludeTypeTree;
};



bool TraverseTree(grn_reference const& ObjReference,
                  grn_reference const& TypeReference,
                  conversion_state& ConversionState,
                  temp_grn_reference* NewObjectReference,
                  temp_grn_reference* NewTypeReference);

END_GRANNY_NAMESPACE;

static uint32x
AlignSize(uint32x Size, uint32x Alignment)
{
    // General alignment, not just pow2
    Assert(Alignment > 0);

    const int32x ModAlign = Size % Alignment;
    if (ModAlign)
        return Size + (Alignment - ModAlign);

    return Size;
}

static void
InitializeConversionState(int32x SourcePointerSize,
                          bool   SourceIsLittleEndian,
                          int32x DestPointerSize,
                          bool   DestIsLittleEndian,
                          bool   ExcludeTypeTree,
                          file*  ConvertFile,
                          grn_pointer_fixup **SectionFixups,
                          conversion_state* Result)
{
    Assert(SourcePointerSize == 32 || SourcePointerSize == 64);
    Assert(DestPointerSize == 32 || DestPointerSize == 64);

    Result->CopyRequiresSwap = (SourceIsLittleEndian != DestIsLittleEndian);
    Result->File = ConvertFile;
    Result->SectionFixups = SectionFixups;

    // Create an arena for our temp vars
    Result->Arena = NewMemoryArena();

    // And a container to hold fixup fixups.  No, that's not a typo
    Result->FixupRemaps = Allocate(fixup_remap_container);
    Initialize(Result->FixupRemaps, 0);

    // And a stack to hold actual fixups in the final file
    StackInitialize(Result->PointerFixups, SizeOf(temp_pointer_fixup), 4096);

    // And a stack to hold mixed marshalling entries
    StackInitialize(Result->MixedMarshalling, SizeOf(temp_mixed_marshall), 4096);

    // Array for section pointers and sizes
    Result->NumFileSections = ConvertFile->Header->SectionArrayCount;
    Result->NewSections = (temp_section*)
        MemoryArenaPush(*Result->Arena, Result->NumFileSections * sizeof(temp_section));
    {for(int32x SecIndex = 0; SecIndex < Result->NumFileSections; ++SecIndex)
    {
        // Zero everything out
        SetUInt8(SizeOf(Result->NewSections[SecIndex]), 0, &Result->NewSections[SecIndex]);
    }}


    // Size variables...
    Result->SourcePointerSize = SourcePointerSize;
    Result->SourcePointerSizeInBytes = SourcePointerSize / 8;
    Result->SourceIsLittleEndian = SourceIsLittleEndian;
    Result->DestPointerSize = DestPointerSize;
    Result->DestPointerSizeInBytes = DestPointerSize / 8;
    Result->DestIsLittleEndian = DestIsLittleEndian;
    if (SourcePointerSize == 32)
    {
        Result->SourceDataTypeDefSize = DataTypeDefSize32;
        Result->SourceDataTypeNameOffset = DataTypeNameOffset32;
        Result->SourceDataTypeReferenceTypeOffset = DataTypeReferenceTypeOffset32;
    }
    else
    {
        Result->SourceDataTypeDefSize = DataTypeDefSize64;
        Result->SourceDataTypeNameOffset = DataTypeNameOffset64;
        Result->SourceDataTypeReferenceTypeOffset = DataTypeReferenceTypeOffset64;
    }

    if (DestPointerSize == 32)
    {
        Result->DestDataTypeDefSize = DataTypeDefSize32;
        Result->DestDataTypeNameOffset = DataTypeNameOffset32;
        Result->DestDataTypeReferenceTypeOffset = DataTypeReferenceTypeOffset32;
    }
    else
    {
        Result->DestDataTypeDefSize = DataTypeDefSize64;
        Result->DestDataTypeNameOffset = DataTypeNameOffset64;
        Result->DestDataTypeReferenceTypeOffset = DataTypeReferenceTypeOffset64;
    }

    Result->ExcludeTypeTree = ExcludeTypeTree;
}


static void
FreeConversionState(conversion_state &OldState)
{
    FreeMemory(OldState.FixupRemaps);
    Deallocate(OldState.FixupRemaps);
    OldState.FixupRemaps = NULL;

    StackCleanUp(OldState.PointerFixups);
    StackCleanUp(OldState.MixedMarshalling);

    if (OldState.NewSections)
    {
        {for(int32x Index = 0; Index < OldState.NumFileSections; Index++)
        {
            {for(int32x Sub = 0; Sub < NumSubsections; Sub++)
            {
                if (OldState.NewSections[Index].Pointer[Sub])
                {
                    PlatformDeallocate(OldState.NewSections[Index].Pointer[Sub]);
                    OldState.NewSections[Index].Pointer[Sub] = 0;
                }

            }}
            if (OldState.NewSections[Index].Flattened)
            {
                PlatformDeallocate(OldState.NewSections[Index].Flattened);
                OldState.NewSections[Index].Flattened = 0;
            }
        }}
    }

    // Free this last, since we have arrays that depend on it
    if (OldState.Arena)
    {
        FreeMemoryArena(OldState.Arena);
        OldState.Arena = 0;
    }
}


static int32
GetInt32Native(void const* Memory,
               conversion_state& ConversionState)
{
    int32 const* AsInts = (int32 const*)Memory;

#if PROCESSOR_LITTLE_ENDIAN
    if (ConversionState.SourceIsLittleEndian)
        return *AsInts;
    else
        return Reverse32(*AsInts);
#else
    if (!ConversionState.SourceIsLittleEndian)
        return *AsInts;
    else
        return Reverse32(*AsInts);
#endif
}


static int32
GetInt32FromPointerNative(void const* Memory,
                          conversion_state& ConversionState)
{
    int32* AsInts = (int32*)Memory;

    bool const BytesReversed = ((PROCESSOR_LITTLE_ENDIAN != 0) !=
                                ConversionState.SourceIsLittleEndian);

    if (ConversionState.DestPointerSize == 64)
    {
        if (ConversionState.DestIsLittleEndian)
        {
            return BytesReversed ? Reverse32(AsInts[1]) : AsInts[1];
        }
        else
        {
            return BytesReversed ? Reverse32(AsInts[0]) : AsInts[0];
        }
    }
    else
    {
        return BytesReversed ? Reverse32(AsInts[0]) : AsInts[0];
    }
}


static void
PutInt32(void const* Memory,
         int32 const Value,
         conversion_state& ConversionState)
{
    int32 *AsInts = (int32*)Memory;

#if PROCESSOR_LITTLE_ENDIAN
    if (ConversionState.DestIsLittleEndian)
        *AsInts = Value;
    else
        *AsInts = Reverse32(Value);
#else
    if (ConversionState.DestIsLittleEndian)
        *AsInts = Reverse32(Value);
    else
        *AsInts = Value;
#endif
}


static void
PutInt32AsPointer(uint8* Memory,
                  int32 Value,
                  conversion_state& ConversionState)
{
    int32* AsInts = (int32*)Memory;

    bool const BytesReversed = ((PROCESSOR_LITTLE_ENDIAN != 0) !=
                                ConversionState.DestIsLittleEndian);
    int32 InjectedValue = BytesReversed ? Reverse32(Value) : Value;

    if (ConversionState.DestPointerSize == 64)
    {
        if (ConversionState.DestIsLittleEndian)
        {
            AsInts[0] = 0;
            AsInts[1] = Value;
        }
        else
        {
            AsInts[0] = Value;
            AsInts[1] = 0;
        }
    }
    else
    {
        *AsInts = InjectedValue;
    }
}

static void
CopyStringHash(uint8 const* Source,
               uint8 *Destination,
               conversion_state &ConversionState)
{
    // Getting the correct bytes is a little tricky here if the source
    // is 64bit.
    int32 HashVal;
    if (ConversionState.SourcePointerSize == 32)
    {
        HashVal = GetInt32Native(Source, ConversionState);
    }
    else
    {
        if (ConversionState.SourceIsLittleEndian)
        {
            // Value is in the LSBs at the top
            HashVal = GetInt32Native(Source, ConversionState);
        }
        else
        {
            // Value is in the LSBs at the bottom
            HashVal = GetInt32Native(Source + 4, ConversionState);
        }
    }

    // Same deal.  Make sure to zero the other bits in the 64byte case
    if (ConversionState.DestPointerSize == 32)
    {
        PutInt32(Destination, HashVal, ConversionState);
    }
    else
    {
        if (ConversionState.SourceIsLittleEndian)
        {
            // Value is in the LSBs at the top
            PutInt32(Destination, HashVal, ConversionState);
            SetUInt8(4, 0, Destination + 4);
        }
        else
        {
            // Value is in the LSBs at the bottom
            SetUInt8(4, 0, Destination);
            PutInt32(Destination + 4, HashVal, ConversionState);
        }
    }
}


static bool
ValidFileReference(grn_reference const& Reference, file* File)
{
    if (Reference.SectionIndex >= File->Header->SectionArrayCount)
    {
        return false;
    }

    grn_section *SectionArray = GetGRNSectionArray(*File->Header);
    if (Reference.Offset >= SectionArray[Reference.SectionIndex].ExpandedDataSize)
    {
        return false;
    }

    return true;
}

static uint8*
GetFilePointer(uint32x SectionIndex, uint32x Offset, file* File)
{
    // We're assuming the parameters are ok...

    Assert(File->Sections[SectionIndex] != NULL);
    return (((uint8*)File->Sections[SectionIndex]) + Offset);
}

static uint8*
GetFilePointer(grn_reference const& Reference, file* File)
{
    Assert(ValidFileReference(Reference, File));

    return GetFilePointer(Reference.SectionIndex, Reference.Offset, File);
}

static bool
GetPointerReferenceAt(uint32x const PtrSection,
                      uint32x const PtrOffset,
                      conversion_state& ConversionState,
                      grn_reference* Fixup)
{
    Assert(Fixup);
    file* File = ConversionState.File;

    grn_reference PointerRef;
    PointerRef.SectionIndex = PtrSection;
    PointerRef.Offset       = PtrOffset;
    if (!ValidFileReference(PointerRef, ConversionState.File))
    {
        InvalidCodePath("Bad file reference found!");
        return false;
    }

    Assert(File->Sections[PtrSection] != NULL);
    grn_section *SectionArray = GetGRNSectionArray(*File->Header);
    grn_pointer_fixup *FixupArray = ConversionState.SectionFixups[PtrSection];

    //--- todo: can we assume this is sorted and do a bin search?
    {for(uint32x FixIndex = 0;
         FixIndex < SectionArray[PtrSection].PointerFixupArrayCount;
         ++FixIndex)
    {
        if (PtrOffset == FixupArray[FixIndex].FromOffset)
        {
            Fixup->SectionIndex = FixupArray[FixIndex].To.SectionIndex;
            Fixup->Offset = FixupArray[FixIndex].To.Offset;
            return true;
        }
    }}

    return false;
}

static void
ExtractDataType(uint8 const* BasePointer,
                int32x TypeOffset,
                conversion_state& ConversionState,
                data_type_definition* DataType)
{
    // Check some platform independant truths...
    CompileAssert(OffsetFromType(data_type_definition, Type) == 0);
    CompileAssert(SizeOfMember(data_type_definition, Type) == 4);

    DataType->Type = (member_type)GetInt32Native(BasePointer + TypeOffset, ConversionState);

    // We actually want to pull the name field, since it might be a
    // hash.  Note that we convert it to a /native/ type, not the
    // dest type, since we're working with this directly
    DataType->Name = (char*)(intaddrx)
        GetInt32FromPointerNative(BasePointer + TypeOffset + ConversionState.SourceDataTypeNameOffset,
                                  ConversionState);

    int32x const TailOffset =
        TypeOffset + (ConversionState.SourceDataTypeReferenceTypeOffset +
                      ConversionState.SourcePointerSizeInBytes);

    // Only the array width is really critical here, but let's be safe.
    DataType->ArrayWidth  = GetInt32Native(BasePointer + TailOffset,      ConversionState);
    DataType->Extra[0]    = GetInt32Native(BasePointer + TailOffset +  4, ConversionState);
    DataType->Extra[1]    = GetInt32Native(BasePointer + TailOffset +  8, ConversionState);
    DataType->Extra[2]    = GetInt32Native(BasePointer + TailOffset + 12, ConversionState);
    //DataType->Ignored__Ignored
}


// Copies just the data members.  We'll fixup the pointers separately
static void
InjectDataType(data_type_definition const &DataType,
               uint8 *Destination,
               conversion_state& ConversionState)
{
    // Clear everything to zero by default
    SetUInt8(ConversionState.DestDataTypeDefSize, 0, Destination);

    PutInt32(Destination, DataType.Type, ConversionState);

    // We do want to set the name pointer, in case it's a hash...
    PutInt32AsPointer(Destination + ConversionState.DestDataTypeNameOffset,
                      (int32)(intaddrx)DataType.Name, ConversionState);

    int32x const TailOffset = (ConversionState.DestDataTypeReferenceTypeOffset +
                               ConversionState.DestPointerSizeInBytes);

    // Only the array width is critical in the rest of the structure
    PutInt32(Destination + TailOffset +  0, DataType.ArrayWidth,  ConversionState);
}


static bool
IsLocationEmptyType(grn_reference const& Reference,
                    conversion_state& ConversionState)
{
    // unable to determine type because of error = EmptyType
    if (!ValidFileReference(Reference, ConversionState.File))
    {
        InvalidCodePath("Bad file reference found!");
        return true;
    }

    data_type_definition DataType;
    uint8* TypeLocation = GetFilePointer(Reference, ConversionState.File);
    ExtractDataType(TypeLocation, 0,
                    ConversionState,
                    &DataType);

    return DataType.Type == EndMember;
}


static uint32x
ExtractDataTypeMarshalling(grn_reference const& TypeReference,
                           conversion_state& ConversionState)
{
    file *File = ConversionState.File;
    uint8* BaseTypePointer = GetFilePointer(TypeReference, File);

    uint32 Marshall = 0;
    int32x TypeOffset = 0;
    while (true)
    {
        // Note that the Name and ReferenceType pointers are not to be
        // touched!
        data_type_definition DataType;
        ExtractDataType(BaseTypePointer, TypeOffset,
                        ConversionState,
                        &DataType);
        if (DataType.Type == EndMember)
            break;

        // We have to handle the inline case ourselves, since it tries
        // to dereference the ReferencedType field
        if (DataType.Type == InlineMember)
        {
            grn_reference FixedUp;
            if (GetPointerReferenceAt(TypeReference.SectionIndex,
                                      TypeReference.Offset + TypeOffset + ConversionState.SourceDataTypeReferenceTypeOffset,
                                      ConversionState,
                                    &FixedUp))
            {
                // Recurse to the inline type
                Marshall |= ExtractDataTypeMarshalling(FixedUp, ConversionState);
            }
            else
            {
                // This is really bad
                InvalidCodePath("Should never occur, inline members must always have valid type reference");
            }
        }
        else
        {
            Marshall |= GetMemberMarshalling(DataType);
        }

        // Advance to the next type field...
        TypeOffset += ConversionState.SourceDataTypeDefSize;
    }

    return Marshall;
}


static int32x
ExtractDataTypeSizePlatform(grn_reference const& TypeReference,
                            int32x const PointerSizeInBits,
                            conversion_state& ConversionState)
{
    file *File = ConversionState.File;
    uint8* BaseTypePointer = GetFilePointer(TypeReference, File);

    int32x FinalSize = 0;
    int32x TypeOffset = 0;
    while (true)
    {
        // Note that the Name and ReferenceType pointers are not to be
        // touched!
        data_type_definition DataType;
        ExtractDataType(BaseTypePointer, TypeOffset,
                        ConversionState,
                        &DataType);
        if (DataType.Type == EndMember)
            break;

        // We have to handle the inline case ourselves, since it tries
        // to dereference the ReferencedType field
        if (DataType.Type == InlineMember)
        {
            grn_reference FixedUp;
            if (GetPointerReferenceAt(TypeReference.SectionIndex,
                                      TypeReference.Offset + TypeOffset + ConversionState.SourceDataTypeReferenceTypeOffset,
                                      ConversionState,
                                      &FixedUp))
            {
                // Recurse to the inline type
                FinalSize += ExtractDataTypeSizePlatform(FixedUp, PointerSizeInBits, ConversionState);
            }
            else
            {
                // This is really bad
                InvalidCodePath("Should never occur, inline members must always have valid type reference");
            }
        }
        else
        {
            int32x RealArraySize = DataType.ArrayWidth ? DataType.ArrayWidth : 1;
            FinalSize += RealArraySize * GetMemberUnitSizePlatform(DataType.Type, PointerSizeInBits);
        }

        // Advance to the next type field...
        TypeOffset += ConversionState.SourceDataTypeDefSize;
    }

    return FinalSize;
}


static int32x
GetSubsectionIndex(uint32x Marshalling)
{
    Assert((Marshalling & !MarshallingMask) == 0);

    switch (Marshalling)
    {
        case Int32Marshalling:
            return Index32;

        case Int16Marshalling:
            return Index16;

        default:
            return IndexAny;
    }
}

static bool
ObjectAlreadyCopied(conversion_state &ConversionState,
                    grn_reference const &Reference,
                    temp_grn_reference *FixedUpLocation)
{
    fixup_remap *RemapEntry = Find(ConversionState.FixupRemaps, Reference);
    if (RemapEntry != NULL)
    {
        Assert(RemapEntry->OldLocation.SectionIndex == Reference.SectionIndex);
        Assert(RemapEntry->OldLocation.Offset == Reference.Offset);
        *FixedUpLocation = RemapEntry->NewLocation;
        return true;
    }

    return false;
}


static void
InsertFixupRemap(grn_reference const &OldLocation,
                 temp_grn_reference const &NewLocation,
                 conversion_state &ConversionState)
{
    Add(ConversionState.FixupRemaps, OldLocation, NewLocation);
}


static void
InsertPointerFixup(temp_grn_reference const &PointerLocation,
                   temp_grn_reference const &ObjectLocation,
                   conversion_state &ConversionState)
{
    Assert((int32x)PointerLocation.SectionIndex < ConversionState.NumFileSections);
    Assert(PointerLocation.SubsectionIndex < NumSubsections);
    Assert(PointerLocation.Offset < ConversionState.NewSections[PointerLocation.SectionIndex].Size[PointerLocation.SubsectionIndex]);

    int32x Index;
    if (!NewStackUnit(ConversionState.PointerFixups, Index))
    {
        InvalidCodePath("Failed to allocate pointer fixup");
        return;
    }

    temp_pointer_fixup *NewRemap = (temp_pointer_fixup*)GetStackUnit(ConversionState.PointerFixups, Index);
    NewRemap->PointerLocation = PointerLocation;
    NewRemap->ObjectLocation = ObjectLocation;
}


static void
InsertMixedMarshall(temp_grn_reference const &ObjectLocation,
                    temp_grn_reference const &TypeLocation,
                    int32x const ArrayCount,
                    conversion_state &ConversionState)
{
    int32x Index;
    if (!NewStackUnit(ConversionState.MixedMarshalling, Index))
    {
        InvalidCodePath("Failed to allocate marshalling");
        return;
    }

    temp_mixed_marshall *NewMarshalling =
        (temp_mixed_marshall*)GetStackUnit(ConversionState.MixedMarshalling, Index);

    NewMarshalling->ObjectLocation = ObjectLocation;
    NewMarshalling->TypeLocation   = TypeLocation;
    NewMarshalling->ArrayCount     = ArrayCount;
}



static bool
CopyObjectData(grn_reference const& ObjReference,
               grn_reference const& TypeReference,
               uint32x Marshalling,
               bool AlignObject,
               conversion_state &ConversionState,
               temp_grn_reference *NewObjectReference)
{
    file* File = ConversionState.File;
    temp_section* NewSections = ConversionState.NewSections;

    // First of all, let's find the section that we'll be copying the object into,
    //  this is marshalling dependant
    int32x const SubsectionIndex = GetSubsectionIndex(Marshalling);

    // Handy to have these as references
    uint32x& SectionSizeRef = NewSections[ObjReference.SectionIndex].Size[SubsectionIndex];
    uint8*   SectionPointer = NewSections[ObjReference.SectionIndex].Pointer[SubsectionIndex];

    // Require that objects be aligned within the section to the
    // alignment of the section
    if (AlignObject)
    {
        grn_section* SectionArray = GetGRNSectionArray(*ConversionState.File->Header);
        SectionSizeRef =
            AlignSize(SectionSizeRef,
                      SectionArray[ObjReference.SectionIndex].InternalAlignment);
    }


    // We actually know exactly where the object will be placed now
    NewObjectReference->SectionIndex = ObjReference.SectionIndex;
    NewObjectReference->SubsectionIndex = SubsectionIndex;
    NewObjectReference->Offset = SectionSizeRef;

    // Loop through the types.  We only copy data for inline members, but we
    //  do account for the space taken up by pointer types
    uint8* ObjPointer = GetFilePointer(ObjReference, File);
    uint8* BaseTypePointer = GetFilePointer(TypeReference, File);

    int32x SrcCopyOffset = 0;
    int32x DstCopyOffset = NewObjectReference->Offset;
    int32x TypeOffset = 0;
    while (true)
    {
        // Note that the Name and ReferenceType pointers are not to be
        // touched!
        data_type_definition DataType;
        ExtractDataType(BaseTypePointer, TypeOffset,
                        ConversionState,
                        &DataType);

        // Have we hit the end?
        if (DataType.Type == EndMember)
            break;

        // Note that Destination might not be valid...
        uint8* Source      = ObjPointer + SrcCopyOffset;
        uint8* Destination = SectionPointer + DstCopyOffset;
        switch (DataType.Type)
        {
            // Inline members are the worst of the lot.  We have to
            // recurse, making sure to preserve the marshalling, etc,
            // and then fixup our offsets once we find out how big the
            // object acutally is/was.
            case InlineMember:
            {
                // Get the reference to the inline members datatype
                grn_reference FixedUpType;
                if (GetPointerReferenceAt(TypeReference.SectionIndex,
                                          TypeReference.Offset + TypeOffset + ConversionState.SourceDataTypeReferenceTypeOffset,
                                          ConversionState, &FixedUpType))
                {
                    uint32x const OldSectionSize = SectionSizeRef;

                    // Ok, now we can recurse, and afterwards, find
                    // the size of the object.
                    grn_reference InlineObjRef = ObjReference;
                    InlineObjRef.Offset += SrcCopyOffset;

                    temp_grn_reference Ignored;
                    if (CopyObjectData(InlineObjRef, FixedUpType, Marshalling,
                                       false, // do NOT align inline members
                                       ConversionState, &Ignored))
                    {
                        // Advance source and dest pointers...
                        int32x SourceSize = ExtractDataTypeSizePlatform(FixedUpType, ConversionState.SourcePointerSize,
                                                                        ConversionState);
                        uint32x DestSize  = ExtractDataTypeSizePlatform(FixedUpType, ConversionState.DestPointerSize,
                                                                        ConversionState);


                        SrcCopyOffset  += SourceSize;
                        DstCopyOffset  += DestSize;

                        // NOTE!  I know it looks like this is
                        // missing, but it's not.  The section size
                        // field will be updated by the recursed
                        // function.  Check the new offset though, to
                        // make sure that it's correct.
                        //SectionSizeRef += ConversionState.DestPointerSizeInBytes;
                        Assert(SectionSizeRef == OldSectionSize + DestSize);
                    }
                    else { InvalidCodePath("Inline member copy recursion failed"); }
                } else { InvalidCodePath("Should never occur, inline members must always have valid type reference"); }
            } break;


            // Strings are interesting.  If the file was written out
            // with a string callback registered, then the values are
            // hashes, rather than pointers.  We can't actually tell
            // which is the case at this point, but we can deduce it
            // from the presense or absence of a pointer fixup for the
            // string.  (Hashes don't get fixed up, obviously).
            case StringMember:
            {
                grn_reference Fixup;
                if (GetPointerReferenceAt(ObjReference.SectionIndex,
                                          ObjReference.Offset + SrcCopyOffset,
                                          ConversionState,
                                          &Fixup))
                {
                    // It's a pointer.  Don't copy anything
                }
                else
                {
                    // It's a hash, copy and marshall.  This is
                    // complicated enough that we call a routine for
                    // it
                    if (SectionPointer != 0)
                    {
                        CopyStringHash(Source, Destination, ConversionState);
                    }
                }
                SrcCopyOffset  += ConversionState.SourcePointerSizeInBytes;
                DstCopyOffset  += ConversionState.DestPointerSizeInBytes;
                SectionSizeRef += ConversionState.DestPointerSizeInBytes;
            } break;


            case ReferenceMember:
            {
                // This type is just a void*.  We'll issue a fixup for
                // it in the subobject copy.  For now, just step the
                // offsets
                SrcCopyOffset  += ConversionState.SourcePointerSizeInBytes;
                DstCopyOffset  += ConversionState.DestPointerSizeInBytes;
                SectionSizeRef += ConversionState.DestPointerSizeInBytes;
            } break;

            case VariantReferenceMember:
            {
                // This type is just a void* + a type reference.
                // We'll issue fixups for it in the subobject copy.
                // For now, just step the offsets
                SrcCopyOffset  += 2 * ConversionState.SourcePointerSizeInBytes;
                DstCopyOffset  += 2 * ConversionState.DestPointerSizeInBytes;
                SectionSizeRef += 2 * ConversionState.DestPointerSizeInBytes;
            } break;

            case ReferenceToArrayMember:
            case ArrayOfReferencesMember:
            {
                // This is an int32 count + a pointer.  We'll fixup
                // the pointer later, but we need to copy and marshall
                // the count now.  It's at the head of the pair.
                if (SectionPointer != 0)
                {
                    Copy32(1, Source, Destination);
                    if (ConversionState.CopyRequiresSwap)
                        Reverse32(4, Destination);
                }

                SrcCopyOffset  += SizeOf(int32) + ConversionState.SourcePointerSizeInBytes;
                DstCopyOffset  += SizeOf(int32) + ConversionState.DestPointerSizeInBytes;
                SectionSizeRef += SizeOf(int32) + ConversionState.DestPointerSizeInBytes;
            } break;

            case ReferenceToVariantArrayMember:
            {
                // This is (ptr + int32 + ptr).  We'll fixup the
                // pointers later, but we need to copy and marshall
                // the count now.  It's in the middle of the triple.
                // Dammit.
                uint8* SourceCountPtr = Source      + ConversionState.SourcePointerSizeInBytes;
                uint8* DestCountPtr   = Destination + ConversionState.DestPointerSizeInBytes;

                if (SectionPointer != 0)
                {
                    Copy32(1, SourceCountPtr, DestCountPtr);
                    if (ConversionState.CopyRequiresSwap)
                        Reverse32(4, DestCountPtr);
                }

                SrcCopyOffset  += SizeOf(int32) + 2 * ConversionState.SourcePointerSizeInBytes;
                DstCopyOffset  += SizeOf(int32) + 2 * ConversionState.DestPointerSizeInBytes;
                SectionSizeRef += SizeOf(int32) + 2 * ConversionState.DestPointerSizeInBytes;
            } break;

            // Simple type: Copy ArrayWidth bytes
            case Int8Member:
            case UInt8Member:
            case BinormalInt8Member:
            case NormalUInt8Member:
            {
                int32x RealArraySize = DataType.ArrayWidth ? DataType.ArrayWidth : 1;
                int32x CopySize = RealArraySize * SizeOf(int8);

                if (SectionPointer != 0)
                {
                    Copy(CopySize, Source, Destination);
                }
                SrcCopyOffset  += CopySize;
                DstCopyOffset  += CopySize;
                SectionSizeRef += CopySize;
            } break;


            // Simple type: Copy 2 * ArrayWidth bytes and marshall16
            case Int16Member:
            case UInt16Member:
            case BinormalInt16Member:
            case NormalUInt16Member:
            {
                int32x RealArraySize = DataType.ArrayWidth ? DataType.ArrayWidth : 1;
                int32x CopySize = RealArraySize * SizeOf(int16);
                if (SectionPointer != 0)
                {
                    Copy(CopySize, Source, Destination);
                    if (ConversionState.CopyRequiresSwap)
                        Reverse16(CopySize, Destination);
                }
                SrcCopyOffset  += CopySize;
                DstCopyOffset  += CopySize;
                SectionSizeRef += CopySize;
            } break;


            // Simple type: Copy and marshall32
            case TransformMember:
            {
                int32x RealArraySize = DataType.ArrayWidth ? DataType.ArrayWidth : 1;
                int32x CopySize = RealArraySize * SizeOf(transform);
                if (SectionPointer != 0)
                {
                    Copy(CopySize, Source, Destination);
                    if (ConversionState.CopyRequiresSwap)
                        Reverse32(CopySize, Destination);
                }
                SrcCopyOffset  += CopySize;
                DstCopyOffset  += CopySize;
                SectionSizeRef += CopySize;
            } break;


            // Simple type: Copy and marshall32
            case Real32Member:
            case Int32Member:
            case UInt32Member:
            {
                int32x RealArraySize = DataType.ArrayWidth ? DataType.ArrayWidth : 1;
                int32x CopySize = RealArraySize * SizeOf(int32);
                if (SectionPointer != 0)
                {
                    Copy(CopySize, Source, Destination);
                    if (ConversionState.CopyRequiresSwap)
                        Reverse32(CopySize, Destination);
                }
                SrcCopyOffset  += CopySize;
                DstCopyOffset  += CopySize;
                SectionSizeRef += CopySize;
            } break;


            // Should never reach these
            case UnsupportedMemberType_Remove:
                InvalidCodePath("Switchable types no longer supported in Granny 2.7+");
            default:
                InvalidCodePath("Bad member type found!");
                break;
        }

        // Advance to the next type field...
        TypeOffset += ConversionState.SourceDataTypeDefSize;
    }

    return true;
}

static bool HandleStringCopy(grn_reference &StringPointerRef,
                             temp_grn_reference &NewStringPointerRef,
                             conversion_state &ConversionState)
{
    temp_section* NewSections = ConversionState.NewSections;

    // Ok, we have to copy this if we have a fixup for it.  Otherwise,
    // we can bail now, it's a hash, which is copied by Other People.
    grn_reference Fixup;
    if (!GetPointerReferenceAt(StringPointerRef.SectionIndex, StringPointerRef.Offset,
                                ConversionState, &Fixup))
    {
        return true;
    }

    // First, let's see if we already have a fixup for this...
    temp_grn_reference CopiedLocation;
    if (!ObjectAlreadyCopied(ConversionState, Fixup, &CopiedLocation))
    {
        CopiedLocation.SectionIndex = Fixup.SectionIndex;
        CopiedLocation.SubsectionIndex = IndexAny;
        CopiedLocation.Offset = NewSections[Fixup.SectionIndex].Size[IndexAny];

        uint32x &CurrentOffset = NewSections[Fixup.SectionIndex].Size[IndexAny];
        uint8*   BasePointer   = NewSections[Fixup.SectionIndex].Pointer[IndexAny];

        // Slow copy
        uint8 const* SourceString = GetFilePointer(Fixup, ConversionState.File);
        Assert(SourceString);

        do
        {
            if (BasePointer != NULL)
                BasePointer[CurrentOffset] = *SourceString;

            CurrentOffset++;
        } while (*(SourceString++) != '\0');

        // Mark the copy
        InsertFixupRemap(Fixup, CopiedLocation, ConversionState);
    }

    // Issue the fixup, CopiedLocation is valid for both branches of
    // the above test
    InsertPointerFixup(NewStringPointerRef, CopiedLocation, ConversionState);

    return true;
}


static bool
CopyType(grn_reference const &TypeReference,
         temp_grn_reference *NewTypeReference,
         conversion_state &ConversionState)
{
    temp_section* NewSections = ConversionState.NewSections;

    Assert(NewTypeReference);
    // TraverseTree can call this multiple times.  Easier to check here.
    if (ObjectAlreadyCopied(ConversionState, TypeReference, NewTypeReference))
    {
        return true;
    }

    // Types always write into the 32-bit marshalling section of the
    //  file
    uint32x& SectionSizeRef = NewSections[TypeReference.SectionIndex].Size[Index32];
    uint8*   SectionPtr     = NewSections[TypeReference.SectionIndex].Pointer[Index32];

    // We can fill in the new type reference at this point...
    NewTypeReference->SectionIndex    = TypeReference.SectionIndex;
    NewTypeReference->SubsectionIndex = Index32;
    NewTypeReference->Offset          = SectionSizeRef;

    // Here's the trick.  We have to copy all of the fields from this
    // data type, then recurse on any subtypes.
    uint8* BasePointer = GetFilePointer(TypeReference, ConversionState.File);
    int32x TypeOffset = 0;

    // In this case, we still need to do something to the end
    // member...
    data_type_definition DataType;
    do
    {
        ExtractDataType(BasePointer, TypeOffset,
                        ConversionState, &DataType);
        if (SectionPtr != NULL)
        {
            InjectDataType(DataType,
                           SectionPtr + SectionSizeRef,
                           ConversionState);
        }

        SectionSizeRef += ConversionState.DestDataTypeDefSize;
        TypeOffset     += ConversionState.SourceDataTypeDefSize;
    } while (DataType.Type != EndMember);

    // Make sure we have a fixup registered for this type, since we
    //  might refer to it circularly
    InsertFixupRemap(TypeReference, *NewTypeReference, ConversionState);


    // This is a little tricky, but in essence: we've moved the
    // sectionsize forward already so that the recursion won't mess up
    // our memory.  We need to run over the types again to issue the
    // fixups, so bring the offset back...
    int32x DestOffset = NewTypeReference->Offset;

    // Now recurse to the sub-types, and issue the fixups.
    TypeOffset = 0;
    do
    {
        ExtractDataType(BasePointer, TypeOffset,
                        ConversionState, &DataType);

        if (DataType.Type != EndMember)
        {
            // Copy and issue a fixup for the name, but only if it's
            // not a hash.  As in other spots, we detect this by
            // checking to see if we have a fixup for it.

            grn_reference StringPointerRef = TypeReference;
            StringPointerRef.Offset += TypeOffset + ConversionState.SourceDataTypeNameOffset;

            temp_grn_reference NewStringPointerRef = *NewTypeReference;
            NewStringPointerRef.Offset = DestOffset + ConversionState.DestDataTypeNameOffset;

            HandleStringCopy(StringPointerRef,
                             NewStringPointerRef,
                             ConversionState);
        }

        switch (DataType.Type)
        {
            case ArrayOfReferencesMember:
            case InlineMember:
            case ReferenceMember:
            case ReferenceToArrayMember:
            {
                grn_reference RefTypeLocation = TypeReference;
                RefTypeLocation.Offset += (TypeOffset + ConversionState.SourceDataTypeReferenceTypeOffset);

                grn_reference RefTypeReference;
                if (GetPointerReferenceAt(RefTypeLocation.SectionIndex, RefTypeLocation.Offset,
                                          ConversionState,
                                          &RefTypeReference))
                {
                    // Check that the type hasn't been copied before
                    temp_grn_reference SubType;
                    if (!ObjectAlreadyCopied(ConversionState, RefTypeReference, &SubType))
                    {
                        if (!CopyType(RefTypeReference, &SubType, ConversionState))
                        {
                            // exceptionally bad
                            InvalidCodePath("Failed to copy type.");
                            return false;
                        }

                        InsertFixupRemap(RefTypeReference, SubType, ConversionState);
                    }

                    // Issue a fixup for the reference type pointer in
                    // the destination type
                    temp_grn_reference PointerLocation = *NewTypeReference;
                    PointerLocation.Offset = DestOffset + ConversionState.DestDataTypeReferenceTypeOffset;
                    InsertPointerFixup(PointerLocation, SubType, ConversionState);
                }
                else
                {
                    InvalidCodePath("Member type that requires a type reference missing one!");
                }
            } break;

            // Threre is a referenced type, but we can't access it
            // here, it's data dependant.  Copy it later.
            case VariantReferenceMember:
            case ReferenceToVariantArrayMember:
                break;

                // No referenced type.
            case StringMember:
            case TransformMember:
            case Real32Member:
            case Int8Member:
            case UInt8Member:
            case BinormalInt8Member:
            case NormalUInt8Member:
            case Int16Member:
            case UInt16Member:
            case BinormalInt16Member:
            case NormalUInt16Member:
            case Int32Member:
            case UInt32Member:
            case EndMember:
                break;

            case UnsupportedMemberType_Remove:
                InvalidCodePath("Switchable types no longer supported in Granny 2.7+");
            default:
                InvalidCodePath("Bad member type!");
                break;
        }

        DestOffset += ConversionState.DestDataTypeDefSize;
        TypeOffset += ConversionState.SourceDataTypeDefSize;
    } while (DataType.Type != EndMember);

    return true;
}


// This is going to look very familiar, if you've just read through
// CopyObject.  The procedure is the same, but we won't actually be
// writing any of this object's data to the new file.
static bool
CopySubObjects(grn_reference const &ObjReference,
               grn_reference const &TypeReference,
               temp_grn_reference const &NewObjectReference,
               temp_grn_reference const &NewTypeReference,
               conversion_state &ConversionState)
{
    file* File = ConversionState.File;
    temp_section* NewSections = ConversionState.NewSections;

    uint8* BaseTypePointer = GetFilePointer(TypeReference, File);

    int32x ObjOffset = 0;
    int32x DestObjOffset = 0;
    int32x TypeOffset = 0;
    int32x DestTypeOffset = 0;
    while (true)
    {
        data_type_definition DataType;
        ExtractDataType(BaseTypePointer, TypeOffset,
                        ConversionState, &DataType);

        // Have we hit the end?
        if (DataType.Type == EndMember)
            break;

        // This is used by several of the cases, so pull it out
        grn_reference ReferencedTypePtr = TypeReference;
        ReferencedTypePtr.Offset += (TypeOffset + ConversionState.SourceDataTypeReferenceTypeOffset);

        switch (DataType.Type)
        {
            case InlineMember:
            {
                grn_reference ReferencedType;
                if (!GetPointerReferenceAt(ReferencedTypePtr.SectionIndex,
                                           ReferencedTypePtr.Offset,
                                           ConversionState,
                                           &ReferencedType))
                {
                    InvalidCodePath("Inline members must always have a type");
                    return false;
                }

                temp_grn_reference NewTypeReference;
                if (!ObjectAlreadyCopied(ConversionState, ReferencedType, &NewTypeReference))
                {
                    InvalidCodePath("Type should have been copied alread by CopyType");
                    return false;
                }

                // Copy the subobjects of this inline
                temp_grn_reference NewInlineReference = NewObjectReference;
                NewInlineReference.Offset += DestObjOffset;

                grn_reference OldInlineReference = ObjReference;
                OldInlineReference.Offset += ObjOffset;

                if (!CopySubObjects(OldInlineReference,
                                    ReferencedType,
                                    NewInlineReference,
                                    NewTypeReference,
                                    ConversionState))
                {
                    InvalidCodePath("Failed to CopySubObjects of an inline type");
                    return false;
                }

                ObjOffset     += ExtractDataTypeSizePlatform(ReferencedType, ConversionState.SourcePointerSize, ConversionState);
                DestObjOffset += ExtractDataTypeSizePlatform(ReferencedType, ConversionState.DestPointerSize, ConversionState);
            } break;

            // Strings are interesting.  If the file was written out
            // with a string callback registered, then the values are
            // hashes, rather than pointers.  We can't actually tell
            // which is the case at this point, but we can deduce it
            // from the presense or absence of a pointer fixup for the
            // string.  (Hashes don't get fixed up, obviously).
            case StringMember:
            {
                grn_reference StringReference = ObjReference;
                StringReference.Offset += ObjOffset;

                temp_grn_reference NewStringReference = NewObjectReference;
                NewStringReference.Offset += DestObjOffset;

                if (!HandleStringCopy(StringReference, NewStringReference, ConversionState))
                {
                    InvalidCodePath("Failed to copy string?");
                }
            } break;


            case ReferenceMember:
            {
                grn_reference ReferencedType;
                if (!GetPointerReferenceAt(ReferencedTypePtr.SectionIndex,
                                           ReferencedTypePtr.Offset,
                                           ConversionState,
                                           &ReferencedType))
                {
                    InvalidCodePath("Reference members must always have a type");
                    return false;
                }

                grn_reference ReferencedObject;
                if (GetPointerReferenceAt(ObjReference.SectionIndex,
                                          ObjReference.Offset + ObjOffset,
                                          ConversionState,
                                          &ReferencedObject))
                {
                    // copy object if it's not already copied
                    temp_grn_reference CopiedObjectRef;
                    temp_grn_reference CopiedTypeRef;
                    if (!TraverseTree(ReferencedObject, ReferencedType,
                                      ConversionState,
                                      &CopiedObjectRef,
                                      &CopiedTypeRef))
                    {
                        InvalidCodePath("Failed to copy referenced object");
                        return false;
                    }

                    // issue a fixup for object
                    temp_grn_reference ObjPointerReference = NewObjectReference;
                    ObjPointerReference.Offset += DestObjOffset;
                    InsertPointerFixup(ObjPointerReference, CopiedObjectRef, ConversionState);

                    // type is already copied, and the fixup is issued
                }
                else
                {
                    // Null object, type is already copied, and the fixup is issued
                }
            } break;

            case VariantReferenceMember:
            {
                // A variant is a pointer to the type followed by a
                // pointer to the object.  Note that it's ok for
                // either of these to be null, in which case we don't
                // fix either of them up...
                grn_reference ReferencedType;
                if (GetPointerReferenceAt(ObjReference.SectionIndex,
                                          ObjReference.Offset + ObjOffset,
                                          ConversionState,
                                          &ReferencedType))
                {
                    grn_reference ReferencedObject;
                    if (GetPointerReferenceAt(ObjReference.SectionIndex,
                                              (ObjReference.Offset + ObjOffset +
                                               ConversionState.SourcePointerSizeInBytes),
                                              ConversionState,
                                              &ReferencedObject))
                    {
                        // copy object and type
                        temp_grn_reference CopiedObjectRef;
                        temp_grn_reference CopiedTypeRef;
                        if (TraverseTree(ReferencedObject, ReferencedType,
                                         ConversionState,
                                         &CopiedObjectRef,
                                         &CopiedTypeRef))
                        {
                            // issue a fixup for the pointer in the datatype
                            temp_grn_reference PointerLocation = NewObjectReference;
                            PointerLocation.Offset += DestObjOffset;
                            InsertPointerFixup(PointerLocation, CopiedTypeRef, ConversionState);

                            // issue a fixup for object
                            temp_grn_reference ObjPointerReference = NewObjectReference;
                            ObjPointerReference.Offset += DestObjOffset + ConversionState.DestPointerSizeInBytes;
                            InsertPointerFixup(ObjPointerReference, CopiedObjectRef, ConversionState);
                        }
                        else
                        {
                            InvalidCodePath("Failed to copy variant object");
                            return false;
                        }
                    }
                }
            } break;

            case ReferenceToArrayMember:
            {
                grn_reference ReferencedType;
                temp_grn_reference CopiedTypeRef;
                if (!GetPointerReferenceAt(ReferencedTypePtr.SectionIndex,
                                           ReferencedTypePtr.Offset,
                                           ConversionState,
                                           &ReferencedType) ||
                    !ObjectAlreadyCopied(ConversionState, ReferencedType,
                                         &CopiedTypeRef))
                {
                    InvalidCodePath("ReferenceToArray members must always have a type which was already copied");
                    return false;
                }

                int32x const CountOffset   = ObjReference.Offset + ObjOffset;
                int32x const PointerOffset = ObjReference.Offset + ObjOffset + SizeOf(int32);
                uint8* CountMemory = GetFilePointer(ObjReference.SectionIndex,
                                                    CountOffset,
                                                    ConversionState.File);
                int32x const SourceCount = GetInt32Native(CountMemory, ConversionState);

                grn_reference ReferencedArray;
                if (GetPointerReferenceAt(ObjReference.SectionIndex,
                                          PointerOffset,
                                          ConversionState,
                                          &ReferencedArray))
                {
                    Assert(SourceCount != 0);

                    // Check for array already copied
                    temp_grn_reference ArrayStartRef;
                    if (!ObjectAlreadyCopied(ConversionState, ReferencedArray,
                                             &ArrayStartRef))
                    {
                        // Marshall the objects in the same way everytime.
                        // This is key to making sure that the objects wind
                        // up continuous!
                        int32x Marshalling = ExtractDataTypeMarshalling(ReferencedType, ConversionState);

                        // Align the destination location, since it
                        // will be done in CopyObjectData anyways...
                        uint32x StartOffset;
                        {
                            grn_section* SectionArray = GetGRNSectionArray(*ConversionState.File->Header);
                            StartOffset =
                                AlignSize(NewSections[ReferencedArray.SectionIndex].Size[GetSubsectionIndex(Marshalling)],
                                          SectionArray[ReferencedArray.SectionIndex].InternalAlignment);
                            NewSections[ReferencedArray.SectionIndex].Size[GetSubsectionIndex(Marshalling)] = StartOffset;
                        }

                        // First, we need to make space for this in the
                        // destination file.
                        uint32x const SourceObjSize = ExtractDataTypeSizePlatform(ReferencedType,
                                                                                  ConversionState.SourcePointerSize,
                                                                                  ConversionState);
                        uint32x const DestObjSize = ExtractDataTypeSizePlatform(ReferencedType,
                                                                                ConversionState.DestPointerSize,
                                                                                ConversionState);
                        // Ok, now let's copy subobjects...
                        {for(int32x Index = 0; Index < SourceCount; ++Index)
                        {
                            grn_reference SourceObjReference = ReferencedArray;
                            SourceObjReference.Offset += Index * SourceObjSize;

                            temp_grn_reference CopiedObjRef;
                            if (CopyObjectData(SourceObjReference, ReferencedType,
                                               Marshalling,
                                               false,  // we've already aligned this object
                                               ConversionState,
                                               &CopiedObjRef) == false)
                            {
                                Log0(ErrorLogMessage, DataTypeLogMessage,
                                     "PlatformConversion::CopySubObjects: Failed to copy ReferenceToArrayMember object data");
                                return false;
                            }

                            // Get a reference to the start of the array for fixups...
                            if (Index == 0)
                                ArrayStartRef = CopiedObjRef;

                            Assert(CopiedObjRef.SectionIndex == SourceObjReference.SectionIndex);
                            Assert(CopiedObjRef.SubsectionIndex == GetSubsectionIndex(Marshalling));
                            Assert(CopiedObjRef.Offset == StartOffset + (Index * DestObjSize));
                        }}

                        int32x const EndOffset = NewSections[ReferencedArray.SectionIndex].Size[GetSubsectionIndex(Marshalling)];
                        Assert(EndOffset - StartOffset == (DestObjSize * SourceCount));

                        // Allow pointers to the array to be fixed up
                        InsertFixupRemap(ReferencedArray, ArrayStartRef, ConversionState);

                        // If these are mixed marshalling objects, then issue a marshalling fixup
                        if (IsMixedMarshalling(Marshalling))
                        {
                            InsertMixedMarshall(ArrayStartRef, CopiedTypeRef, SourceCount,
                                                ConversionState);
                        }

                        // Ok, now let's traverse subobjects...
                        {for(int32x Index = 0; Index < SourceCount; ++Index)
                        {
                            grn_reference SourceObjReference = ReferencedArray;
                            SourceObjReference.Offset += Index * SourceObjSize;

                            temp_grn_reference DestObjRef = ArrayStartRef;
                            DestObjRef.Offset += (Index * DestObjSize);

                            if (CopySubObjects(SourceObjReference, ReferencedType,
                                               DestObjRef, CopiedTypeRef,
                                               ConversionState) == false)
                            {
                                Log0(ErrorLogMessage, DataTypeLogMessage,
                                     "PlatformConversion::CopySubObjects: Failed to copy ReferenceToArrayMember SubObjects");
                                return false;
                            }
                        }}
                    }

                    // Issue a fixup for the array pointer in the object
                    temp_grn_reference ObjPointerReference = NewObjectReference;
                    ObjPointerReference.Offset += DestObjOffset + SizeOf(int32);
                    InsertPointerFixup(ObjPointerReference, ArrayStartRef, ConversionState);
                }
                else
                {
                    // Empty array.  Size should be zero.
                    Assert(SourceCount == 0);
                }
            } break;

            case ArrayOfReferencesMember:
            {
                // An ArrayOfReferencesMember is an int32 count
                // followed by a pointer to an array of "count"
                // pointers.  This should be fun!
                int32x const CountOffset   = ObjReference.Offset + ObjOffset;
                int32x const PointerOffset = ObjReference.Offset + ObjOffset + SizeOf(int32);
                uint8* CountMemory = GetFilePointer(ObjReference.SectionIndex,
                                                    CountOffset,
                                                    ConversionState.File);
                int32x const SourceCount = GetInt32Native(CountMemory, ConversionState);

                grn_reference ReferencedType;
                if (!GetPointerReferenceAt(ReferencedTypePtr.SectionIndex,
                                           ReferencedTypePtr.Offset,
                                           ConversionState,
                                           &ReferencedType))
                {
                    InvalidCodePath("ArrayOfReferencesMember must always have a type which was already copied");
                    return false;
                }

                grn_reference ReferencedArray;
                if (GetPointerReferenceAt(ObjReference.SectionIndex,
                                          PointerOffset,
                                          ConversionState,
                                          &ReferencedArray))
                {
                    Assert(SourceCount != 0);

                    // Subtlety: the granny file writing routines do NOT check to
                    //  see if the array of references is the same in the same
                    //  way that they check normal arrays.  We duplicate that behavior
                    //  here, even though we can easily check ObjectAlreadyCopied and
                    //  skip it.

                    // First, we need to make space for this in the
                    // destination file.  Since the array consists of
                    // only pointers, it's put into the "any"
                    // marshalling section.  We also never need to
                    // copy the pointer values, since they're always
                    // fixed up.
                    temp_section &ArraySection = NewSections[ReferencedArray.SectionIndex];
                    int32x const DestArrayBaseOffset = ArraySection.Size[IndexAny];
                    ArraySection.Size[IndexAny] += SourceCount * ConversionState.DestPointerSizeInBytes;

                    // Note the location of the array start
                    temp_grn_reference ArrayStartRef;
                    ArrayStartRef.SectionIndex    = ReferencedArray.SectionIndex;
                    ArrayStartRef.SubsectionIndex = IndexAny;
                    ArrayStartRef.Offset          = DestArrayBaseOffset;

                    // Ok, now let's copy subobjects...
                    {for(int32x Index = 0; Index < SourceCount; ++Index)
                    {
                        int32x const SrcArrayOffset = Index * ConversionState.SourcePointerSizeInBytes;
                        int32x const DstArrayOffset = Index * ConversionState.DestPointerSizeInBytes;

                        grn_reference Referenced;
                        if (GetPointerReferenceAt(ReferencedArray.SectionIndex,
                                                  ReferencedArray.Offset + SrcArrayOffset,
                                                  ConversionState,
                                                  &Referenced))
                        {
                            // copy object and type
                            temp_grn_reference CopiedObjectRef;
                            temp_grn_reference CopiedTypeRef;
                            if (TraverseTree(Referenced, ReferencedType,
                                             ConversionState,
                                             &CopiedObjectRef,
                                             &CopiedTypeRef))
                            {
                                // issue a fixup for object
                                temp_grn_reference ObjPointerReference;
                                ObjPointerReference.SectionIndex = ReferencedArray.SectionIndex;
                                ObjPointerReference.SubsectionIndex = IndexAny;
                                ObjPointerReference.Offset = DestArrayBaseOffset + DstArrayOffset;

                                InsertPointerFixup(ObjPointerReference, CopiedObjectRef, ConversionState);
                            }
                            else
                            {
                                InvalidCodePath("Failed to copy variant object");
                                return false;
                            }
                        }
                        else
                        {
                            // Null pointer?  Leave it alone.
                        }
                    }}

                    // Issue a fixup for the array pointer in the object
                    temp_grn_reference ObjPointerReference = NewObjectReference;
                    ObjPointerReference.Offset += DestObjOffset + SizeOf(int32);
                    InsertPointerFixup(ObjPointerReference, ArrayStartRef, ConversionState);
                }
                else
                {
                    // Empty array.  Size should be zero.
                    Assert(SourceCount == 0);
                }
            } break;

            case ReferenceToVariantArrayMember:
            {
                // The bad boy.  This is essentially the same as
                // ReferenceToArray, but the type works like a
                // variant, and must be copied here.
                int32x const TypeOffset    = ObjReference.Offset + ObjOffset;
                int32x const CountOffset   = TypeOffset  + ConversionState.SourcePointerSizeInBytes;
                int32x const PointerOffset = CountOffset + SizeOf(int32);

                grn_reference ReferencedType;
                if (!GetPointerReferenceAt(ObjReference.SectionIndex,
                                           TypeOffset,
                                           ConversionState,
                                           &ReferencedType))
                {
                    // Null variant, that's ok.  Everything else is treated as null as well...
                    break;
                }

                temp_grn_reference CopiedTypeRef;
                if (!ObjectAlreadyCopied(ConversionState, ReferencedType, &CopiedTypeRef))
                {
                    // Copy the type...
                    if (!CopyType(ReferencedType, &CopiedTypeRef, ConversionState))
                    {
                        InvalidCodePath("ReferenceToVariantArrayMember failed to copy type");
                        return false;
                    }
                }

                uint8* CountMemory = GetFilePointer(ObjReference.SectionIndex,
                                                    CountOffset,
                                                    ConversionState.File);
                int32x const SourceCount = GetInt32Native(CountMemory, ConversionState);

                grn_reference ReferencedArray;
                if (GetPointerReferenceAt(ObjReference.SectionIndex,
                                          PointerOffset,
                                          ConversionState,
                                          &ReferencedArray))
                {
                    Assert(SourceCount != 0);

                    // Check for array already copied
                    temp_grn_reference ArrayStartRef;
                    if (!ObjectAlreadyCopied(ConversionState, ReferencedArray,
                                             &ArrayStartRef))
                    {
                        // Marshall the objects in the same way everytime.
                        // This is key to making sure that the objects wind
                        // up continuous!
                        int32x Marshalling = ExtractDataTypeMarshalling(ReferencedType, ConversionState);

                        // Align the destination location, since it
                        // will be done in CopyObjectData anyways...
                        uint32x StartOffset;
                        {
                            grn_section* SectionArray = GetGRNSectionArray(*ConversionState.File->Header);
                            StartOffset =
                                AlignSize(NewSections[ReferencedArray.SectionIndex].Size[GetSubsectionIndex(Marshalling)],
                                          SectionArray[ReferencedArray.SectionIndex].InternalAlignment);
                            NewSections[ReferencedArray.SectionIndex].Size[GetSubsectionIndex(Marshalling)] = StartOffset;
                        }

                        // First, we need to make space for this in the
                        // destination file.
                        uint32x const SourceObjSize = ExtractDataTypeSizePlatform(ReferencedType,
                                                                                  ConversionState.SourcePointerSize,
                                                                                  ConversionState);
                        uint32x const DestObjSize = ExtractDataTypeSizePlatform(ReferencedType,
                                                                                ConversionState.DestPointerSize,
                                                                                ConversionState);
                        // Ok, now let's copy subobjects...
                        {for(int32x Index = 0; Index < SourceCount; ++Index)
                        {
                            grn_reference SourceObjReference = ReferencedArray;
                            SourceObjReference.Offset += Index * SourceObjSize;

                            temp_grn_reference CopiedObjRef;
                            if (CopyObjectData(SourceObjReference, ReferencedType,
                                               Marshalling,
                                               false, // we've already aligned...
                                               ConversionState,
                                               &CopiedObjRef) == false)
                            {
                                Log0(ErrorLogMessage, DataTypeLogMessage,
                                     "PlatformConversion::CopySubObjects: Failed to copy ReferenceToVariantArrayMember object data");
                                return false;
                            }

                            // Get a reference to the start of the array for fixups...
                            if (Index == 0)
                                ArrayStartRef = CopiedObjRef;

                            Assert(CopiedObjRef.SectionIndex == SourceObjReference.SectionIndex);
                            Assert(CopiedObjRef.SubsectionIndex == GetSubsectionIndex(Marshalling));
                            Assert(CopiedObjRef.Offset == StartOffset + (Index * DestObjSize));
                        }}

                        int32x const EndOffset = NewSections[ReferencedArray.SectionIndex].Size[GetSubsectionIndex(Marshalling)];
                        Assert(EndOffset - StartOffset == (DestObjSize * SourceCount));

                        // Allow pointers to the array to be fixed up
                        InsertFixupRemap(ReferencedArray, ArrayStartRef, ConversionState);

                        // If these are mixed marshalling objects, then issue a marshalling fixup
                        if (IsMixedMarshalling(Marshalling))
                        {
                            InsertMixedMarshall(ArrayStartRef, CopiedTypeRef, SourceCount,
                                                ConversionState);
                        }

                        // Ok, now let's traverse subobjects...
                        {for(int32x Index = 0; Index < SourceCount; ++Index)
                        {
                            grn_reference SourceObjReference = ReferencedArray;
                            SourceObjReference.Offset += Index * SourceObjSize;

                            temp_grn_reference DestObjRef = ArrayStartRef;
                            DestObjRef.Offset += (Index * DestObjSize);

                            if (CopySubObjects(SourceObjReference, ReferencedType,
                                               DestObjRef, CopiedTypeRef,
                                               ConversionState) == false)
                            {
                                Log0(ErrorLogMessage, DataTypeLogMessage,
                                     "PlatformConversion::CopySubObjects: Failed to copy ReferenceToVariantArrayMember SubObjects");
                                return false;
                            }
                        }}
                    }

                    // Issue a fixup for the type pointer in the object
                    temp_grn_reference ObjTypeReference = NewObjectReference;
                    ObjTypeReference.Offset += DestObjOffset;
                    InsertPointerFixup(ObjTypeReference, CopiedTypeRef, ConversionState);

                    // Issue a fixup for the array pointer in the object
                    temp_grn_reference ObjPointerReference = NewObjectReference;
                    ObjPointerReference.Offset += DestObjOffset + SizeOf(int32) + ConversionState.DestPointerSizeInBytes;
                    InsertPointerFixup(ObjPointerReference, ArrayStartRef, ConversionState);
                }
                else
                {
                    // Empty array.  Size should be zero.
                    Assert(SourceCount == 0);
                }
            } break;

            case Int8Member:
            case UInt8Member:
            case BinormalInt8Member:
            case NormalUInt8Member:
            case Int16Member:
            case UInt16Member:
            case BinormalInt16Member:
            case NormalUInt16Member:
            case TransformMember:
            case Real32Member:
            case Int32Member:
            case UInt32Member:
            {
                // Do nothing for these types...
            } break;

            // Should never reach these
            case UnsupportedMemberType_Remove:
                InvalidCodePath("Switchable types no longer supported in Granny 2.7+");
            default:
                InvalidCodePath("Bad member type found!");
                break;
        }


        // Step the ObjOffset:
        // Note!  We know we can get away with this, since the inline
        // member type returns 0 for it's size.  Any adjustments
        // should be done in that case branch.
        int32x RealArraySize = DataType.ArrayWidth ? DataType.ArrayWidth : 1;
        ObjOffset     += RealArraySize * GetMemberUnitSizePlatform(DataType.Type, ConversionState.SourcePointerSize);
        DestObjOffset += RealArraySize * GetMemberUnitSizePlatform(DataType.Type, ConversionState.DestPointerSize);

        // Advance to the next type field...
        TypeOffset += ConversionState.SourceDataTypeDefSize;
        DestTypeOffset += ConversionState.DestDataTypeDefSize;
    }

    return true;
}


bool GRANNY
TraverseTree(grn_reference const& ObjReference,
             grn_reference const& TypeReference,
             conversion_state& ConversionState,
             temp_grn_reference* NewObjectReference,
             temp_grn_reference* NewTypeReference)
{
    file *File = ConversionState.File;
    if (!ValidFileReference(ObjReference, File) ||
        !ValidFileReference(TypeReference, File))
    {
        InvalidCodePath("Bad file reference found!");
        return false;
    }

    // Copy the type information
    if (CopyType(TypeReference, NewTypeReference, ConversionState) == false)
    {
        Log0(ErrorLogMessage, DataTypeLogMessage,
             "PlatformConversion::TraverseTree: Failed to copy type");
        return false;
    }

    // If we've already copied this, then skip.  ObjectAC will set
    // NewObjectReference for us.
    if (!ObjectAlreadyCopied(ConversionState, ObjReference, NewObjectReference))
    {
        // We do two passes over the object.  First, make space in the
        // destination for the object itself.  Then return, and copy
        // subobjects that haven't yet been copied.  We have to extract
        // the marshalling here, since all of the inline members must copy
        // into the same subsection.
        uint32x const Marshalling = ExtractDataTypeMarshalling(TypeReference, ConversionState);
        if (CopyObjectData(ObjReference, TypeReference,
                           Marshalling,
                           true, // align the object
                           ConversionState,
                           NewObjectReference) == false)
        {
            Log0(ErrorLogMessage, DataTypeLogMessage,
                 "PlatformConversion::TraverseTree: Failed to ObjectData");
            return false;
        }

        // Allow pointers to the object to be fixed up
        InsertFixupRemap(ObjReference, *NewObjectReference, ConversionState);

        // If this is a mixed marshalling object, then issue a marshalling fixup
        if (IsMixedMarshalling(Marshalling))
        {
            InsertMixedMarshall(*NewObjectReference, *NewTypeReference, 1,
                                ConversionState);
        }

        if (CopySubObjects(ObjReference, TypeReference,
                           *NewObjectReference,
                           *NewTypeReference,
                           ConversionState) == false)
        {
            Log0(ErrorLogMessage, DataTypeLogMessage,
                 "PlatformConversion::TraverseTree: Failed to CopySubObjects");
            return false;
        }
    }

    return true;
}

static void
GetFlattenedReference(temp_grn_reference const &Unflattened,
                      grn_reference *Flattened,
                      conversion_state &ConversionState)
{
    Flattened->SectionIndex = Unflattened.SectionIndex;

    temp_section &Section = ConversionState.NewSections[Flattened->SectionIndex];
    Flattened->Offset = Section.Offsets[Unflattened.SubsectionIndex] + Unflattened.Offset;
}


static bool
WriteFixupsAndFlattenSections(conversion_state &ConversionState,
                              grn_pointer_fixup **SectionFixups,
                              grn_mixed_marshalling_fixup **SectionMarshalls)
{
    uint32x const NumSections = ConversionState.File->Header->SectionArrayCount;
    grn_section* SectionArray = GetGRNSectionArray(*ConversionState.File->Header);
    temp_section* NewSections = ConversionState.NewSections;

    int32x NumPointerFixups  = GetStackUnitCount(ConversionState.PointerFixups);
    int32x NumMixedMarshalls = GetStackUnitCount(ConversionState.MixedMarshalling);

    // First, let's count fixups/marshalling and make sure that we hve
    //  the correct number
    // accounted for.
    {
        {for(int32x Index = 0; Index < NumPointerFixups; Index++)
        {
            temp_pointer_fixup *Fixup =
                (temp_pointer_fixup*)GetStackUnit(ConversionState.PointerFixups, Index);

            int32x SectionIndex = Fixup->PointerLocation.SectionIndex;
            NewSections[SectionIndex].NumFixups++;
        }}

        {for(int32x Index = 0; Index < NumMixedMarshalls; Index++)
        {
            temp_mixed_marshall *Marshall =
                (temp_mixed_marshall*)GetStackUnit(ConversionState.MixedMarshalling, Index);

            int32x SectionIndex = Marshall->ObjectLocation.SectionIndex;
            NewSections[SectionIndex].NumMarshalls++;
        }}

        {for(uint32x Index = 0; Index < NumSections; Index++)
        {
            Assert(SectionArray[Index].PointerFixupArrayCount == NewSections[Index].NumFixups);
            Assert(SectionArray[Index].MixedMarshallingFixupArrayCount == NewSections[Index].NumMarshalls);
        }}
    }

    // Ok, time to allocate
    {for(uint32x Index = 0; Index < NumSections; Index++)
    {
        temp_section &Section = NewSections[Index];

        // Align the sizes to the correct internal alignment and compute the
        //  total size
        int32x TotalSize = 0;
        {for(int32x SubSection = 0; SubSection < 3; SubSection++)
        {
            Assert(SectionArray[Index].InternalAlignment > 0);
            uint32x Pad = (SectionArray[Index].InternalAlignment -
                           (Section.Size[SubSection] % SectionArray[Index].InternalAlignment));
            if (Pad == SectionArray[Index].InternalAlignment)
                Pad = 0;

            Section.Offsets[SubSection] = TotalSize;
            TotalSize += Section.Size[SubSection] + Pad;
        }}

        // Use the aggr system to match granny_file.cpp:LoadSection
        // Subtlety!  We're adding a chunk of memory onto the end
        // of the allocation to allow the buffer to be passed into
        // the Oodle compressors, which require a bit of padding.
        const int32x OodlePadding = GetOodle1CompressBufferPaddingSize() * 2;
        Section.TotalSize = TotalSize;

        aggr_allocator Allocator;
        InitializeAggrAlloc(Allocator);

        SetAggrAlignment(Allocator, SectionArray[Index].InternalAlignment);
        AggrAllocSizePtr(Allocator, TotalSize + OodlePadding, Section.Flattened);
        EndAggrAlloc(Allocator);

        if (!Section.Flattened)
        {
            Log1(ErrorLogMessage, DataTypeLogMessage,
                 "WriteFixupsAndFlattenSections: Failed to create flattened array (attempting: %d",
                 TotalSize+OodlePadding);
            return false;
        }

        // And copy...
        {for(int32x SubSection = 0; SubSection < 3; SubSection++)
        {
            Copy(Section.Size[SubSection],
                 Section.Pointer[SubSection],
                 Section.Flattened + Section.Offsets[SubSection]);
        }}
    }}


    // We now have all the info we need to write the fixups.
    {for(uint32x Index = 0; Index < NumSections; Index++)
    {
        // Fixups
        uint32x DestIndex = 0;
        {for(int32x FixIndex = 0; FixIndex < NumPointerFixups; FixIndex++)
        {
            temp_pointer_fixup *Fixup =
                (temp_pointer_fixup*)GetStackUnit(ConversionState.PointerFixups, FixIndex);

            // Not for this section?
            if (Fixup->PointerLocation.SectionIndex != Index)
                continue;

            grn_reference PointerReference;
            grn_reference ObjReference;
            GetFlattenedReference(Fixup->PointerLocation,
                                  &PointerReference,
                                  ConversionState);
            GetFlattenedReference(Fixup->ObjectLocation,
                                  &ObjReference,
                                  ConversionState);

            // Ensure that the grn_pointer_fixup structure hasn't changed on us...
            CompileAssert(OffsetFromType(grn_pointer_fixup, FromOffset) == 0);
            CompileAssert(OffsetFromType(grn_pointer_fixup, To.SectionIndex) == 4);
            CompileAssert(OffsetFromType(grn_pointer_fixup, To.Offset) == 8);
            CompileAssert(SizeOf(grn_pointer_fixup) == 12);

            // Write out the fixup
            grn_pointer_fixup &FinalFixup = SectionFixups[Index][DestIndex];
            FinalFixup.FromOffset = PointerReference.Offset;
            FinalFixup.To         = ObjReference;

            // Advance the destination
            DestIndex++;
        }}
        Assert(DestIndex == SectionArray[Index].PointerFixupArrayCount);


        // MixedMarshalling
        DestIndex = 0;
        {for(int32x FixIndex = 0; FixIndex < NumMixedMarshalls; FixIndex++)
        {
            temp_mixed_marshall *Fixup =
                (temp_mixed_marshall*)GetStackUnit(ConversionState.MixedMarshalling, FixIndex);

            // Not for this section?
            if (Fixup->ObjectLocation.SectionIndex != Index)
                continue;

            grn_reference ObjectReference;
            grn_reference TypeReference;
            GetFlattenedReference(Fixup->ObjectLocation,
                                  &ObjectReference,
                                  ConversionState);
            GetFlattenedReference(Fixup->TypeLocation,
                                  &TypeReference,
                                  ConversionState);

            // Ensure that the grn_pointer_fixup structure hasn't changed on us...
            CompileAssert(OffsetFromType(grn_mixed_marshalling_fixup, Count) == 0);
            CompileAssert(OffsetFromType(grn_mixed_marshalling_fixup, Offset) == 4);
            CompileAssert(OffsetFromType(grn_mixed_marshalling_fixup, Type.SectionIndex) == 8);
            CompileAssert(OffsetFromType(grn_mixed_marshalling_fixup, Type.Offset) == 12);
            CompileAssert(SizeOf(grn_mixed_marshalling_fixup) == 16);

            // Write out the fixup
            grn_mixed_marshalling_fixup &FinalFixup = SectionMarshalls[Index][DestIndex];
            FinalFixup.Count  = Fixup->ArrayCount;
            FinalFixup.Offset = ObjectReference.Offset;
            FinalFixup.Type   = TypeReference;

            // Advance the destination index
            DestIndex++;
        }}
    }}

    // Huzzah!  One flattened file structure
    return true;
}


bool GRANNY
PlatformConversion(int32x SourcePointerSize,
                   bool   SourceIsLittleEndian,
                   int32x DestPointerSize,
                   bool   DestIsLittleEndian,
                   bool   ExcludeTypeTree,
                   file*  ConvertFile,
                   grn_pointer_fixup **SectionFixups,
                   grn_mixed_marshalling_fixup **SectionMarshalls)
{
    Assert(SourcePointerSize == 32 || SourcePointerSize == 64);
    Assert(DestPointerSize == 32 || DestPointerSize == 64);
    Assert(ConvertFile != NULL);
    Assert(SectionFixups);

    conversion_state ConversionState;
    InitializeConversionState(SourcePointerSize,
                              SourceIsLittleEndian,
                              DestPointerSize,
                              DestIsLittleEndian,
                              ExcludeTypeTree,
                              ConvertFile,
                              SectionFixups,
                              &ConversionState);

    // Note that if the root type definition is the empty type, then
    // this is a raw file, and we can bail.  We don't support
    // converting raw files yet.
    if (IsLocationEmptyType(ConvertFile->Header->RootObjectTypeDefinition,
                            ConversionState))
    {
        Log0(ErrorLogMessage, DataTypeLogMessage,
             "PlatformConversion: Cannot convert Raw File types (empty type as root object type detected)");
        return false;
    }

    // The first thing we need to do is to traverse the typetree, and
    // find out the new section sizes so we can allocate the new
    // buffers.
    temp_grn_reference NewObjReference;
    temp_grn_reference NewObjTypeReference;
    if (!TraverseTree(ConvertFile->Header->RootObject,
                      ConvertFile->Header->RootObjectTypeDefinition,
                      ConversionState,
                      &NewObjReference,
                      &NewObjTypeReference))
    {
        Log0(ErrorLogMessage, DataTypeLogMessage,
             "PlatformConversion: Initial tree traversal failed");

        FreeConversionState(ConversionState);
        return false;
    }

    // Clear the fixup stacks
    {
        FreeMemory(ConversionState.FixupRemaps);
        Initialize(ConversionState.FixupRemaps, 0);

        PopStackUnits(ConversionState.PointerFixups,
                      GetStackUnitCount(ConversionState.PointerFixups));
        PopStackUnits(ConversionState.MixedMarshalling,
                      GetStackUnitCount(ConversionState.MixedMarshalling));
    }

    // Allocate the buffers
    {for(int32x SecIndex = 0; SecIndex < ConversionState.NumFileSections; ++SecIndex)
    {
        temp_section &Section = ConversionState.NewSections[SecIndex];
        {for(int32x Sub = 0; Sub < NumSubsections; ++Sub)
        {
            if (Section.Size[Sub])
            {
                Section.Pointer[Sub] = (uint8*)PlatformAllocate(Section.Size[Sub]);
                SetUInt8(Section.Size[Sub], 0, Section.Pointer[Sub]);
            }
            Section.Size[Sub] = 0;
        }}
    }}

    // Retraverse
    if (!TraverseTree(ConvertFile->Header->RootObject,
                      ConvertFile->Header->RootObjectTypeDefinition,
                      ConversionState,
                      &NewObjReference,
                      &NewObjTypeReference))
    {
        Log0(ErrorLogMessage, DataTypeLogMessage,
             "PlatformConversion: Secondary tree traversal failed");

        FreeConversionState(ConversionState);
        return false;
    }

    // Ok, we now need to flatten the arrays, and write the fixups.
    // Note that the fixups obviously have subsection dependencies
    // that can't be properly resolved until the arrays are flattened.
    if (!WriteFixupsAndFlattenSections(ConversionState,
                                       SectionFixups,
                                       SectionMarshalls))
    {
        Log0(ErrorLogMessage, DataTypeLogMessage,
             "PlatformConversion: Failed to write flattened sections and fixups");

        FreeConversionState(ConversionState);
        return false;
    }

    // Fixup the section offsets (marshalling indices, etc.)  Some of
    // these get modified by the writing interface, so it's sorta
    // magic which fields we touch here...
    grn_section* SectionArray = GetGRNSectionArray(*ConversionState.File->Header);
    {for(int32x SecIndex = 0; SecIndex < ConversionState.NumFileSections; ++SecIndex)
    {
        SectionArray[SecIndex].ExpandedDataSize = ConversionState.NewSections[SecIndex].TotalSize;
        SectionArray[SecIndex].First16Bit = ConversionState.NewSections[SecIndex].Offsets[Index16];
        SectionArray[SecIndex].First8Bit  = ConversionState.NewSections[SecIndex].Offsets[IndexAny];
    }}

    // Fixup header reference to the root object and type
    {
        GetFlattenedReference(NewObjTypeReference,
                              &ConvertFile->Header->RootObjectTypeDefinition,
                              ConversionState);
        GetFlattenedReference(NewObjReference,
                              &ConvertFile->Header->RootObject,
                              ConversionState);
    }

    // Set the byte reversed flag.  Note that the meaning of this
    // is: ByteReversed w.r.t. the platform of the file
    ConvertFile->IsByteReversed = (PROCESSOR_LITTLE_ENDIAN != 0) != ConversionState.DestIsLittleEndian;

    // Replace grn_file pointers
    {for(int32x SecIndex = 0; SecIndex < ConversionState.NumFileSections; ++SecIndex)
    {
        if (ConvertFile->IsUserMemory[SecIndex] == false)
        {
            Deallocate(ConvertFile->Sections[SecIndex]);
        }

        ConvertFile->Sections[SecIndex] = ConversionState.NewSections[SecIndex].Flattened;
        ConvertFile->IsUserMemory[SecIndex] = false;
        ConversionState.NewSections[SecIndex].Flattened = 0;

        // Note that we've already marshalled the sections in the
        // conversion process, so we can just mark this as done.
        ConvertFile->Marshalled[SecIndex] = ConvertFile->IsByteReversed;
    }}

    FreeConversionState(ConversionState);

    return true;
}
