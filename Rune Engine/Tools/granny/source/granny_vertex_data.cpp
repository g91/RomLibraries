// ========================================================================
// $File: //jeffr/granny/rt/granny_vertex_data.cpp $
// $DateTime: 2006/10/19 16:35:09 $
// $Change: 13632 $
// $Revision: #14 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#if !defined(GRANNY_VERTEX_DATA_H)
#include "granny_vertex_data.h"
#endif

#if !defined(GRANNY_MEMORY_H)
#include "granny_memory.h"
#endif

#if !defined(GRANNY_MATH_H)
#include "granny_math.h"
#endif

#if !defined(GRANNY_STRING_FORMATTING_H)
#include "granny_string_formatting.h"
#endif

#if !defined(GRANNY_LIMITS_H)
#include "granny_limits.h"
#endif

#if !defined(GRANNY_DATA_TYPE_CONVERSION_H)
#include "granny_data_type_conversion.h"
#endif

#if !defined(GRANNY_ASSERT_H)
#include "granny_assert.h"
#endif

#if !defined(GRANNY_LOG_H)
#include "granny_log.h"
#endif

#if !defined(GRANNY_STRING_H)
#include "granny_string.h"
#endif

#if !defined(GRANNY_PARAMETER_CHECKING_H)
#include "granny_parameter_checking.h"
#endif

#if !defined(GRANNY_CURVE_H)
#include "granny_curve.h"
#endif

#if !defined(GRANNY_CPP_SETTINGS_H)
// This should always be the last header included
#include "granny_cpp_settings.h"
#endif

#define SubsystemCode VertexLayoutLogMessage

USING_GRANNY_NAMESPACE;

BEGIN_GRANNY_NAMESPACE;

data_type_definition VertexAnnotationSetType[] =
{
    {StringMember, "Name"},
    {ReferenceToVariantArrayMember, "VertexAnnotations"},
    {Int32Member, "IndicesMapFromVertexToAnnotation"},
    {ReferenceToArrayMember, "VertexAnnotationIndices", Int32Type},

    {EndMember},
};

data_type_definition VertexDataType[] =
{
    {ReferenceToVariantArrayMember, "Vertices"},
    {ReferenceToArrayMember, "VertexComponentNames", StringType},
    {ReferenceToArrayMember, "VertexAnnotationSets", VertexAnnotationSetType},

    {EndMember},
};

data_type_definition P3VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},

    {EndMember},
};

data_type_definition PT32VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {Real32Member, VertexTextureCoordinatesName "0", 0, 2},

    {EndMember},
};

data_type_definition PN33VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {Real32Member, VertexNormalName, 0, 3},

    {EndMember},
};

data_type_definition PNG333VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},

    {EndMember},
};

data_type_definition PNGT3332VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},
    {Real32Member, VertexTextureCoordinatesName "0", 0, 2},

    {EndMember},
};

data_type_definition PNGB3333VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},
    {Real32Member, VertexBinormalName, 0, 3},

    {EndMember},
};

data_type_definition PNGBX33333VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},
    {Real32Member, VertexBinormalName, 0, 3},
    {Real32Member, VertexTangentBinormalCrossName, 0, 3},

    {EndMember},
};

data_type_definition PNT332VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTextureCoordinatesName "0", 0, 2},

    {EndMember},
};

data_type_definition PNGBT33332VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},
    {Real32Member, VertexBinormalName, 0, 3},
    {Real32Member, VertexTextureCoordinatesName "0", 0, 2},

    {EndMember},
};

data_type_definition PNT333VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTextureCoordinatesName "0", 0, 3},

    {EndMember},
};

data_type_definition PNGBT33333VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},
    {Real32Member, VertexBinormalName, 0, 3},
    {Real32Member, VertexTextureCoordinatesName "0", 0, 3},

    {EndMember},
};

data_type_definition PWN313VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {UInt32Member, VertexBoneIndicesName, 0, 1},
    {Real32Member, VertexNormalName, 0, 3},

    {EndMember},
};

data_type_definition PWNG3133VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {UInt32Member, VertexBoneIndicesName, 0, 1},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},

    {EndMember},
};

data_type_definition PWNGT31332VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {UInt32Member, VertexBoneIndicesName, 0, 1},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},
    {Real32Member, VertexTextureCoordinatesName "0", 0, 3},

    {EndMember},
};

data_type_definition PWNGB31333VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {UInt32Member, VertexBoneIndicesName, 0, 1},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},
    {Real32Member, VertexBinormalName, 0, 3},

    {EndMember},
};

data_type_definition PWNT3132VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {UInt32Member, VertexBoneIndicesName, 0, 1},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTextureCoordinatesName "0", 0, 2},

    {EndMember},
};

data_type_definition PWNGBT313332VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {UInt32Member, VertexBoneIndicesName, 0, 1},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},
    {Real32Member, VertexBinormalName, 0, 3},
    {Real32Member, VertexTextureCoordinatesName "0", 0, 2},

    {EndMember},
};

data_type_definition PWN323VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {NormalUInt8Member, VertexBoneWeightsName, 0, 2},
    {UInt8Member, VertexBoneIndicesName, 0, 2},
    {Real32Member, VertexNormalName, 0, 3},

    {EndMember},
};

data_type_definition PWNG3233VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {NormalUInt8Member, VertexBoneWeightsName, 0, 2},
    {UInt8Member, VertexBoneIndicesName, 0, 2},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},

    {EndMember},
};

data_type_definition PWNGT32332VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {NormalUInt8Member, VertexBoneWeightsName, 0, 2},
    {UInt8Member, VertexBoneIndicesName, 0, 2},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},
    {Real32Member, VertexTextureCoordinatesName "0", 0, 2},

    {EndMember},
};

data_type_definition PWNGB32333VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {NormalUInt8Member, VertexBoneWeightsName, 0, 2},
    {UInt8Member, VertexBoneIndicesName, 0, 2},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},
    {Real32Member, VertexBinormalName, 0, 3},

    {EndMember},
};

data_type_definition PWNT3232VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {NormalUInt8Member, VertexBoneWeightsName, 0, 2},
    {UInt8Member, VertexBoneIndicesName, 0, 2},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTextureCoordinatesName "0", 0, 2},

    {EndMember},
};

data_type_definition PWNGBT323332VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {NormalUInt8Member, VertexBoneWeightsName, 0, 2},
    {UInt8Member, VertexBoneIndicesName, 0, 2},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},
    {Real32Member, VertexBinormalName, 0, 3},
    {Real32Member, VertexTextureCoordinatesName "0", 0, 2},

    {EndMember},
};

data_type_definition PWN343VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {NormalUInt8Member, VertexBoneWeightsName, 0, 4},
    {UInt8Member, VertexBoneIndicesName, 0, 4},
    {Real32Member, VertexNormalName, 0, 3},

    {EndMember},
};

data_type_definition PWNG3433VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {NormalUInt8Member, VertexBoneWeightsName, 0, 4},
    {UInt8Member, VertexBoneIndicesName, 0, 4},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},

    {EndMember},
};

data_type_definition PWNGT34332VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {NormalUInt8Member, VertexBoneWeightsName, 0, 4},
    {UInt8Member, VertexBoneIndicesName, 0, 4},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},
    {Real32Member, VertexTextureCoordinatesName "0", 0, 2},

    {EndMember},
};

data_type_definition PWNGB34333VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {NormalUInt8Member, VertexBoneWeightsName, 0, 4},
    {UInt8Member, VertexBoneIndicesName, 0, 4},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},
    {Real32Member, VertexBinormalName, 0, 3},

    {EndMember},
};

data_type_definition PWNT3432VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {NormalUInt8Member, VertexBoneWeightsName, 0, 4},
    {UInt8Member, VertexBoneIndicesName, 0, 4},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTextureCoordinatesName "0", 0, 2},

    {EndMember},
};

data_type_definition PWNGBT343332VertexType[] =
{
    {Real32Member, VertexPositionName, 0, 3},
    {NormalUInt8Member, VertexBoneWeightsName, 0, 4},
    {UInt8Member, VertexBoneIndicesName, 0, 4},
    {Real32Member, VertexNormalName, 0, 3},
    {Real32Member, VertexTangentName, 0, 3},
    {Real32Member, VertexBinormalName, 0, 3},
    {Real32Member, VertexTextureCoordinatesName "0", 0, 2},

    {EndMember},
};

data_type_definition VertexWeightArraysType[] =
{
    {Real32Member, VertexBoneWeightsName, 0, MaximumWeightCount},
    {UInt32Member, VertexBoneIndicesName, 0, MaximumWeightCount},
    {EndMember},
};

END_GRANNY_NAMESPACE;

void GRANNY
ConvertVertexLayouts(int32x VertexCount,
                     data_type_definition const *SourceLayoutType,
                     void const *SourceVertices,
                     data_type_definition const *DestLayoutType,
                     void *DestVertices)
{
    int32x SourceStride = GetTotalObjectSize(SourceLayoutType);
    int32x DestStride = GetTotalObjectSize(DestLayoutType);

    uint8 const *Source = (uint8 const *)SourceVertices;
    uint8 *Dest = (uint8 *)DestVertices;
    while(VertexCount--)
    {
        ConvertSingleObject(SourceLayoutType, Source,
                            DestLayoutType, Dest);

        // So that's the generic stuff done. But we know these are
        // vertices, so there's some extra smart conversion
        // we can do for some common compressed vertex formats,
        // such as those generated the Granny Postprocessor.
        uint8 *DestMember = (uint8 *)Dest;
        data_type_definition const *DestComponentType = DestLayoutType;
        while(DestComponentType && (DestComponentType->Type != EndMember))
        {
            int32x const DestMemberSize = GetMemberTypeSize(*DestComponentType);

            if ( DestComponentType->Type == Real32Member )
            {
                // All the special conversions (so far) only happen when
                // the dest is a real32 (or an array of real32s)
                real32 *DestComponent = (real32 *)DestMember;

                int i;
                variant SourceComponent;
                FindMatchingMember(SourceLayoutType, Source, DestComponentType->Name, &SourceComponent);
                if(SourceComponent.Object)
                {
                    switch(SourceComponent.Type->Type)
                    {
                    case NormalUInt8Member:
                        if ( StringsAreEqual ( DestComponentType->Name, VertexNormalName ) ||
                             StringsAreEqual ( DestComponentType->Name, VertexTangentName ) ||
                             StringsAreEqual ( DestComponentType->Name, VertexBinormalName ) )
                        {
                            // If this was a BinormalInt8Member, we would have already converted it right.
                            // But it's marked as a NormalUInt8Member, which means it's in the rather curious
                            // D3D format of [0-255]->[-1,+1] (it's unsigned because only some cards
                            // can use signed bytes as inputs, so most people use unsigned bytes and
                            // bias them).
                            // (note that some people use [0-254]->[-1,+1] or [1-255]->[-1,+1], so
                            // that 0 has a precise representation, but there's no way to know, and
                            // they will probably all be close enough for government work)
                            for ( i = 0; i < DestComponentType->ArrayWidth; i++ )
                            {
                                // Convert from [0,1] to [-1,+1]
                                DestComponent[i] = ( DestComponent[i] * 2.0f ) - 1.0f;
                            }
                        }

                    case Int16Member:
                        if ( StringBeginsWith ( DestComponentType->Name, VertexTextureCoordinatesName ) )
                        {
                            // This might be a texture coordinate stored in 4.12 format.
                            // Actually, it could be stored in any fixed-point format - we
                            // have no possible way to know. However, leaving it as
                            // a dumb int->float conversion is almost guaranteed to be
                            // useless! And since 4.12 is a very common choice, it's
                            // better than a poke in the eye with a sharp stick.
                            for ( i = 0; i < DestComponentType->ArrayWidth; i++ )
                            {
                                // Convert from [-32768,32768) to [-8,+8)
                                DestComponent[i] *= ( 8.0f / 32768.0f );
                            }
                        }
                        break;

                    default:
                        // Will have been handled by the generic copier.
                        break;
                    }
                }
            }

            DestMember += DestMemberSize;
            ++DestComponentType;
        }


        Source += SourceStride;
        Dest += DestStride;
    }
}


#define ExactOneNormTemplate(Count, Values, OneValue)                   \
{                                                                       \
    int32x Total = 0;                                                   \
                                                                        \
    {for(int32x Index = 0;                                              \
         Index < Count;                                                 \
         ++Index)                                                       \
    {                                                                   \
        Total += Values[Index];                                         \
    }}                                                                  \
                                                                        \
    int32x Difference = (int32)OneValue - Total;                        \
    while(Difference > 0)                                               \
    {                                                                   \
        {for(int32x Index = 0;                                          \
                (Index < Count) && (Difference > 0);                    \
                ++Index)                                                \
        {                                                               \
            if(Values[Index] < OneValue)                                \
            {                                                           \
                ++Values[Index];                                        \
                --Difference;                                           \
            }                                                           \
        }}                                                              \
    }                                                                   \
    while(Difference < 0)                                               \
    {                                                                   \
        {for(int32x Index = Count - 1;                                  \
                (Index >= 0) && (Difference < 0);                       \
                --Index)                                                \
        {                                                               \
            if(Values[Index] > 0)                                       \
            {                                                           \
                --Values[Index];                                        \
                ++Difference;                                           \
            }                                                           \
        }}                                                              \
    }                                                                   \
}


#if DEBUG
#define ExactOneNormTemplateCheck(Count, Values, OneValue)              \
{                                                                       \
    int32x Total = 0;                                                   \
                                                                        \
    {for(int32x Index = 0;                                              \
         Index < Count;                                                 \
         ++Index)                                                       \
    {                                                                   \
        Total += Values[Index];                                         \
    }}                                                                  \
    Assert ( Total == (int32)OneValue );                                \
}

#else
#define ExactOneNormTemplateCheck(Count, Values, OneValue) {}
#endif

void GRANNY
EnsureExactOneNorm(data_type_definition const &WeightsType,
                   void *WeightsInit)
{
    int32x WeightsCount = WeightsType.ArrayWidth;
    switch(WeightsType.Type)
    {
        case BinormalInt8Member:
        {
            int8 *Weights = (int8 *)WeightsInit;
            ExactOneNormTemplate(WeightsCount, Weights, Int8Maximum);
            ExactOneNormTemplateCheck(WeightsCount, Weights, Int8Maximum);
        } break;

        case NormalUInt8Member:
        {
            uint8 *Weights = (uint8 *)WeightsInit;
            ExactOneNormTemplate(WeightsCount, Weights, UInt8Maximum);
            ExactOneNormTemplateCheck(WeightsCount, Weights, UInt8Maximum);
        } break;

        case BinormalInt16Member:
        {
            int16 *Weights = (int16 *)WeightsInit;
            ExactOneNormTemplate(WeightsCount, Weights, Int16Maximum);
            ExactOneNormTemplateCheck(WeightsCount, Weights, Int16Maximum);
        } break;

        case NormalUInt16Member:
        {
            uint16 *Weights = (uint16 *)WeightsInit;
            ExactOneNormTemplate(WeightsCount, Weights, UInt16Maximum);
            ExactOneNormTemplateCheck(WeightsCount, Weights, UInt16Maximum);
        } break;

        case Real32Member:
        {
            // Implicitly works - don't need to do anything
        } break;

        default:
        {
            Log1(ErrorLogMessage, MeshLogMessage,
                 "EnsureExactOneNorm called on unsupported "
                 "type %d\n", WeightsType.Type);
        } break;
    }
}

void GRANNY
OneNormalizeWeights(int32x VertexCount,
                    data_type_definition const *LayoutType,
                    void *Vertices)
{
    uint8 *VertexPointer = (uint8 *)Vertices;
    int32x VertexSize = GetTotalObjectSize(LayoutType);

    variant Weights;
    FindMatchingMember(LayoutType, 0,
                       VertexBoneWeightsName,
                       &Weights);
    if(Weights.Type)
    {
        int32x WeightCount = Weights.Type->ArrayWidth;

        vertex_weight_arrays WeightList;
        while(VertexCount--)
        {
            ConvertSingleObject(LayoutType, VertexPointer,
                                VertexWeightArraysType, &WeightList);

            real32 TotalWeight = 0.0f;
            {for(int32x WeightIndex = 0;
                 WeightIndex < WeightCount;
                 ++WeightIndex)
            {
                TotalWeight += WeightList.BoneWeights[WeightIndex];
            }}

            if(TotalWeight != 0.0f)
            {
                real32 const Normalization = 1.0f / TotalWeight;
                {for(int32x WeightIndex = 0;
                     WeightIndex < WeightCount;
                     ++WeightIndex)
                {
                    WeightList.BoneWeights[WeightIndex] *= Normalization;
                }}
            }

            MergeSingleObject(VertexWeightArraysType, &WeightList,
                              LayoutType, VertexPointer);
            EnsureExactOneNorm(*Weights.Type,
                               VertexPointer + (intaddrx)Weights.Object);

            VertexPointer += VertexSize;
        }
    }
}

void GRANNY
TransformVertices(int32x VertexCount,
                  data_type_definition const *LayoutType, void *Vertices,
                  real32 const *Affine3, real32 const *Linear3x3,
                  real32 const *InverseLinear3x3,
                  bool Renormalize)
{
    // Transform all the vertices
    uint8 *Vertex = (uint8 *)Vertices;
    int32x VertexSize = GetTotalObjectSize(LayoutType);

    pngbx33333_vertex Unpacked;
    {for(int32x VertexIndex = 0;
         VertexIndex < VertexCount;
         ++VertexIndex)
    {
        ConvertSingleObject(LayoutType, Vertex,
                            PNGBX33333VertexType, &Unpacked);

        // These are multiplied by A because they are regular vectors
        VectorTransform3(Unpacked.Position, Linear3x3);
        VectorAdd3(Unpacked.Position, Affine3);

        VectorTransform3(Unpacked.Tangent, Linear3x3);
        VectorTransform3(Unpacked.Binormal, Linear3x3);

        // These are mulitplied by A^-T because they are normals
        TransposeVectorTransform3(Unpacked.TangentBinormalCross,
                                  InverseLinear3x3);
        TransposeVectorTransform3(Unpacked.Normal,
                                  InverseLinear3x3);

        if(Renormalize)
        {
            NormalizeOrZero3(Unpacked.Tangent);
            NormalizeOrZero3(Unpacked.Binormal);
            NormalizeOrZero3(Unpacked.TangentBinormalCross);
            NormalizeOrZero3(Unpacked.Normal);
        }

        MergeSingleObject(PNGBX33333VertexType, &Unpacked,
                          LayoutType, Vertex);

        Vertex += VertexSize;
    }}
}

void GRANNY
NormalizeVertices(int32x VertexCount,
                  data_type_definition const *LayoutType, void *Vertices)
{
    // Transform all the vertices
    uint8 *Vertex = (uint8 *)Vertices;
    int32x VertexSize = GetTotalObjectSize(LayoutType);

    pngbx33333_vertex Unpacked;
    {for(int32x VertexIndex = 0;
         VertexIndex < VertexCount;
         ++VertexIndex)
    {
        ConvertSingleObject(LayoutType, Vertex,
                            PNGBX33333VertexType, &Unpacked);
        NormalizeOrZero3(Unpacked.Tangent);
        NormalizeOrZero3(Unpacked.Binormal);
        NormalizeOrZero3(Unpacked.TangentBinormalCross);
        NormalizeOrZero3(Unpacked.Normal);

        MergeSingleObject(PNGBX33333VertexType, &Unpacked,
                          LayoutType, Vertex);

        Vertex += VertexSize;
    }}
}

int32x GRANNY
GetVertexTextureCoordinatesName(int32x Index, char *Buffer)
{
    return(ConvertToStringVar(MaximumVertexNameLength, Buffer,
                              "%s%d", VertexTextureCoordinatesName, Index));
}

int32x GRANNY
GetVertexDiffuseColorName(int32x Index, char *Buffer)
{
    return(ConvertToStringVar(MaximumVertexNameLength, Buffer,
                              "%s%d", VertexDiffuseColorName, Index));
}

int32x GRANNY
GetVertexSpecularColorName(int32x Index, char *Buffer)
{
    return(ConvertToStringVar(MaximumVertexNameLength, Buffer,
                              "%s%d", VertexSpecularColorName, Index));
}

bool GRANNY
IsSpatialVertexMember(char const *Name)
{
    return(StringsAreEqual(Name, VertexPositionName) ||
           StringsAreEqual(Name, VertexNormalName) ||
           StringsAreEqual(Name, VertexTangentName) ||
           StringsAreEqual(Name, VertexBinormalName) ||
           StringsAreEqual(Name, VertexTangentBinormalCrossName) ||
           StringsAreEqual(Name, VertexBoneIndicesName) ||
           StringsAreEqual(Name, VertexBoneWeightsName));
}

int32x GRANNY
GetVertexBoneCount(data_type_definition const *VertexType)
{
    variant Weights;
    FindMatchingMember(VertexType, 0,
                       VertexBoneWeightsName,
                       &Weights);
    if(Weights.Type)
    {
        return(Weights.Type->ArrayWidth);
    }
    else
    {
        FindMatchingMember(VertexType, 0,
                           VertexBoneIndicesName,
                           &Weights);
        if(Weights.Type)
        {
            return(Weights.Type->ArrayWidth);
        }
    }

    return(0);
}

int32x GRANNY
GetVertexChannelCount(data_type_definition const *VertexType)
{
    int32x ChannelCount = 0;
    while(VertexType->Type != EndMember)
    {
        if(!IsSpatialVertexMember(VertexType->Name))
        {
            ++ChannelCount;
        }

        ++VertexType;
    }

    return(ChannelCount);
}

void GRANNY
GetSingleVertex(data_type_definition const *SourceType,
                void const *SourceVertices,
                int32x VertexIndex, data_type_definition const *As,
                void *Dest)
{
    int32x VertexSize = GetTotalObjectSize(SourceType);
    ConvertSingleObject(SourceType,
                        ((uint8 *)SourceVertices) + VertexIndex*VertexSize,
                        As, Dest);
}

void GRANNY
GetSingleVertex(vertex_data const &VertexData,
                int32x VertexIndex,
                data_type_definition const *As,
                void *Dest)
{
    CheckPointerNotNull(As, return);
    CheckPointerNotNull(Dest, return);
    CheckCountedInt32(VertexIndex, VertexData.VertexCount,
                      MakeEmptyDataTypeObject(As, Dest); return);

    GetSingleVertex(VertexData.VertexType, VertexData.Vertices,
                    VertexIndex, As, Dest);
}

void GRANNY
SetVertexPosition(data_type_definition const *VertexLayout,
                  void *VertexPointer, real32 const *Position)
{
    MergeSingleObject(P3VertexType, Position, VertexLayout, VertexPointer);
}

data_type_definition TempType[] =
{
    {Real32Member, 0, 0, 0},
    {EndMember},
};

void GRANNY
SetVertexNormal(data_type_definition const *VertexLayout,
                void *VertexPointer, real32 const *Normal)
{
    TempType[0].Name = VertexNormalName;
    TempType[0].ArrayWidth = 3;
    MergeSingleObject(TempType, Normal, VertexLayout, VertexPointer);
}

void GRANNY
SetVertexColor(data_type_definition const *VertexLayout,
               void *VertexPointer, int32x ColorIndex,
               real32 const *Color)
{
    char TempName[MaximumVertexNameLength];
    GetVertexDiffuseColorName(ColorIndex, TempName);
    TempType[0].Name = TempName;
    TempType[0].ArrayWidth = 3;
    MergeSingleObject(TempType, Color, VertexLayout, VertexPointer);
}

void GRANNY
SetVertexUVW(data_type_definition const *VertexLayout,
             void *VertexPointer, int32x UVWIndex,
             real32 const *UVW)
{
    char TempName[MaximumVertexNameLength];
    GetVertexTextureCoordinatesName(UVWIndex, TempName);
    TempType[0].Name = TempName;
    TempType[0].ArrayWidth = 3;
    MergeSingleObject(TempType, UVW, VertexLayout, VertexPointer);
}


int32x GRANNY
GetVertexComponentCount(data_type_definition const *VertexLayout)
{
    int32x MemberCount = 0;
    data_type_definition const *SourceType = VertexLayout;
    while(SourceType && (SourceType->Type != EndMember))
    {
        // We don't count bone weights because they are implicit from having bone indices. Kinda.
        if ( !StringsAreEqual(SourceType->Name, VertexBoneWeightsName) )
        {
            ++MemberCount;
        }
        ++SourceType;
    }
    return MemberCount;
}


int32x GRANNY
GetVertexComponentIndex(char const *ComponentName, data_type_definition const *VertexLayout)
{
    // So this will return the "item number" of a component of a vertex.
    // So for most formats, GetVertexTypeComponentIndex ( VertexPositionName, VertexLayout ) == 0
    // And then for PNGB3333VertexType, GetVertexTypeComponentIndex ( VertexTangentName, VertexLayout ) == 2
    // This allows people to look up the tool's component name in the VertexComponentNames array by, e.g.:
    // "uvMayaName" == VertexComponentNames[GetVertexTypeComponentIndex ( VertexTextureCoordinatesName "0", VertexLayout )]
    // (which is exactly what GetVertexComponentToolName does)

    int32x MemberNumber = 0;
    data_type_definition const *SourceType = VertexLayout;
    while(SourceType && (SourceType->Type != EndMember))
    {
        if(StringComparisonCallback)
        {
            if ( StringComparisonCallback ( SourceType->Name, ComponentName ) == 0 )
            {
                return MemberNumber;
            }
        }
        else
        {
            if(StringsAreEqualLowercase(SourceType->Name, ComponentName))
            {
                return MemberNumber;
            }
        }

        ++SourceType;
        ++MemberNumber;
    }

    // Not found.
    return -1;
}

char const * GRANNY
GetVertexComponentToolName(char const *ComponentName, vertex_data const *VertexData)
{
    int32x Index = GetVertexComponentIndex ( ComponentName, VertexData->VertexType );
    if ( Index < 0 )
    {
        // Doesn't exist.
        return NULL;
    }
    else if ( Index >= VertexData->VertexComponentNameCount )
    {
        // Probably means VertexComponentNameCount is zero, i.e. old format data.
        return NULL;
    }
    else
    {
        return VertexData->VertexComponentNames[Index];
    }
}



