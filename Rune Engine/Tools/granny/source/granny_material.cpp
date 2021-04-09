// ========================================================================
// $File: //jeffr/granny/rt/granny_material.cpp $
// $DateTime: 2006/11/10 15:52:52 $
// $Change: 13754 $
// $Revision: #12 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#if !defined(GRANNY_MATERIAL_H)
#include "granny_material.h"
#endif

#if !defined(GRANNY_TEXTURE_H)
#include "granny_texture.h"
#endif

#if !defined(GRANNY_MEMORY_H)
#include "granny_memory.h"
#endif

#if !defined(GRANNY_STRING_H)
#include "granny_string.h"
#endif

#if !defined(GRANNY_CPP_SETTINGS_H)
// This should always be the last header included
#include "granny_cpp_settings.h"
#endif

USING_GRANNY_NAMESPACE;

BEGIN_GRANNY_NAMESPACE;

data_type_definition MaterialMapType[] =
{
    {StringMember, "Usage"},
    {ReferenceMember, "Map", MaterialType},         // Yes, this name should be "Material", but that would break existing files.

    {EndMember},
};

data_type_definition MaterialType[] =
{
    {StringMember, "Name"},

    {ReferenceToArrayMember, "Maps", MaterialMapType},

    {ReferenceMember, "Texture", TextureType},

    {VariantReferenceMember, "ExtendedData"},

    {EndMember},
};

struct usage_code
{
    material_texture_type Type;
    char const *Name;
};

usage_code const UsageCodes[] =
{
    // Maya texture codes
    {DiffuseColorTexture, "color"},
    {SpecularColorTexture, "specularColor"},
    {BumpHeightTexture, "normalCamera"},
    {SelfIlluminationTexture, "incandescence"},

    // MAX texture codes
    {AmbientColorTexture, "Ambient Color"},
    {DiffuseColorTexture, "Diffuse Color"},
    // Max has both specular colour and specular level, which are theoretically multiplied together,
    // but actually you usually only have one of the two present.
    {SpecularColorTexture, "Specular Color"},
    {SpecularColorTexture, "Specular Level"},
    {SelfIlluminationTexture, "Self-Illumination"},
    {OpacityTexture, "Opacity"},
    {BumpHeightTexture, "Bump"},
    {ReflectionTexture, "Reflection"},
    {RefractionTexture, "Refraction"},
    {DisplacementTexture, "Displacement"},

    // LightWave texture codes
    {AmbientColorTexture, "BaseColor"},
    {DiffuseColorTexture, "BaseColor"},
    {SpecularColorTexture, "Specularity"},
    {SelfIlluminationTexture, "Luminosity"},
    {OpacityTexture, "Transparency"},
    {BumpHeightTexture, "Bump"},
    {ReflectionTexture, "ReflectionImage"},
    {RefractionTexture, "RefractionImage"},

    // XSI texture codes
    {AmbientColorTexture, "ambience"},
    {AmbientColorTexture, "ambient"},
    {DiffuseColorTexture, "diffuse"},
    {SpecularColorTexture, "specular"},
    {SelfIlluminationTexture, "incandescence"},
    {SelfIlluminationTexture, "radiance"},
    {OpacityTexture, "transparency"},
    {BumpHeightTexture, "Bump"},
    {ReflectionTexture, "reflectivity"},
    {RefractionTexture, "index_of_refraction"},

};
int32x const UsageCodeCount = SizeOf(UsageCodes)/SizeOf(UsageCodes[0]);

END_GRANNY_NAMESPACE;

static bool
TextureTypeMatches(material_map &MaterialMap, material_texture_type Type)
{
    {for(int32x UsageCodeIndex = 0;
         UsageCodeIndex < UsageCodeCount;
         ++UsageCodeIndex)
    {
        usage_code const &UsageCode = UsageCodes[UsageCodeIndex];
        if(StringsAreEqualLowercase(UsageCode.Name, MaterialMap.Usage) &&
           (UsageCode.Type == Type))
        {
            return(true);
        }
    }}

    return(false);
}

static material *
FindTexturedMaterial(material *Material)
{
    material *Result = 0;

    if(Material)
    {
        if(Material->Texture)
        {
            Result = Material;
        }
        else
        {
            {for(int32x MapIndex = 0;
                 (MapIndex < Material->MapCount) && (!Result);
                 ++MapIndex)
            {
                Result = FindTexturedMaterial(
                    Material->Maps[MapIndex].Material);
            }}
        }
    }

    return(Result);
}

static texture *
FindTexture(material *Material)
{
    texture *Texture = 0;

    material *SubMaterial = FindTexturedMaterial(Material);
    if(SubMaterial)
    {
        Texture = SubMaterial->Texture;
    }

    return(Texture);
}

texture *GRANNY
GetMaterialTextureByType(material const *Material, material_texture_type Type)
{
    if(Material)
    {
        {for(int32x MapIndex = 0;
             MapIndex < Material->MapCount;
             ++MapIndex)
        {
            texture *SourceTexture = FindTexture(
                Material->Maps[MapIndex].Material);
            if(SourceTexture)
            {
                if(TextureTypeMatches(Material->Maps[MapIndex], Type))
                {
                    return(SourceTexture);
                }
            }
        }}
    }

    return(0);
}

material *GRANNY
GetTexturedMaterialByChannelName(material const *Material,
                                 char const *ChannelName)
{
    if(Material)
    {
        {for(int32x MapIndex = 0;
             MapIndex < Material->MapCount;
             ++MapIndex)
        {
            material *SourceMaterial = FindTexturedMaterial(
                Material->Maps[MapIndex].Material);
            if(SourceMaterial)
            {
                if(StringsAreEqualLowercase(
                       Material->Maps[MapIndex].Usage, ChannelName))
                {
                    return(SourceMaterial);
                }
            }
        }}
    }

    return(0);
}

texture *GRANNY
GetMaterialTextureByChannelName(material const *Material,
                                char const *ChannelName)
{
    material *Find = GetTexturedMaterialByChannelName(Material, ChannelName);
    if(Find)
    {
        return(Find->Texture);
    }

    return(0);
}
