#ifndef _RUMATERIALBASE_H_
#define _RUMATERIALBASE_H_

// Rune Engine system core
#include "../../Engine/Base/RuCore.h"

// Rune Engine base renderer core
#include "../../Engine/Base/RuShaderBase.h"

// Rune Engine Manager
#include "../../Engine/Manager/RuEngine_ResourceManager_Base.h"
#include "../../Engine/Manager/RuEngine_ShaderManager.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

enum RuMaterial_Type
{
	ruMATERIAL_TYPE_STANDARD					= 0,							// Standard material
	ruMATERIAL_TYPE_TRANSLUCENT					= 1,							// Translucent material (Deprecated)
	ruMATERIAL_TYPE_FX							= 2,							// FX material (Deprecated)

	ruMATERIAL_TYPE_FORCE_DWORD					= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuMaterial_LightingType
{
	ruMATERIAL_LIGHTINGTYPE_STANDARD			= 0,							// Standard lighting
	ruMATERIAL_LIGHTINGTYPE_PRELIT				= 1,							// Pre-lit
	ruMATERIAL_LIGHTINGTYPE_RESERVED			= 2,							// OLD: Self-illuminated
	ruMATERIAL_LIGHTINGTYPE_COLORED				= 3,							// Colored vertex + standard lighting

	ruMATERIAL_LIGHTINGTYPE_FORCE_DWORD			= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

/*
	NOTES
	-----
	The material interface directly uses a pointer to a shader interface.
	This is because of speed concerns - we don't want to be looking up shaders in
	a string hash at runtime whenever we render! Materials should presumably be read
	from an entity datafile which will specify the shader by name. So the shader
	is looked up during load-time and construction of the material.
*/
class IRuMaterial : public IRuObject
{
	ruRTTI_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuMaterial();

	virtual DWORD							GetDiffuse() = 0;														// ??? Not sure.... materials have a reflectance strength..
	virtual DWORD							GetSpecular() = 0;

	virtual INT32							GetNumTextureChannels() = 0;											// Gets the number of texture channels the material holds
	virtual RuTextureChannel				GetTextureChannel(INT32 index) = 0;										// Gets the texture channel at the specified index
	virtual BOOL							ChannelExists(RuTextureChannel texChannel) = 0;

	virtual IRuBaseTexture*					GetTexture(RuTextureChannel texChannel) = 0;							// Gets the texture specified by the texture channel
	virtual void							SetTexture(RuTextureChannel texChannel, IRuBaseTexture *texture) = 0;	// Sets the texture specified by the texture channel
	virtual void							SetTextureByName(RuTextureChannel texChannel, const char *textureName, RuSurfaceFormat textureFormat = ruSURFACEFMT_DEFAULT) = 0;
	virtual void							SetTextureIsEmbedded(RuTextureChannel texChannel, BOOL isEmbedded) = 0;

	virtual const char*						GetShaderName() = 0;													// Gets the material's preferred shader name
	virtual IRuShader*						GetShader() = 0;														// Gets the interface to the shader used by the material
	virtual void							SetShader(IRuShader *shader) = 0;										// Sets the shader used by the material

	virtual IRuShader*						GetShaderOverride() = 0;
	virtual void							SetShaderOverride(IRuShader *shader) = 0;

	virtual CRuParameterBlock*				GetParameterBlock() = 0;

	virtual INT32							GetMaterialPriority() = 0;
	virtual void							SetMaterialPriority(INT32 priority) = 0;

	virtual RuMaterial_Type					GetMaterialType() = 0;
	virtual void							SetMaterialType(RuMaterial_Type materialType) = 0;

	virtual RuMaterial_LightingType			GetLightingType() = 0;
	virtual void							SetLightingType(RuMaterial_LightingType lightingType) = 0;

	virtual BOOL							IsSkinned() = 0;
	virtual void							SetSkinned(BOOL isSkinned) = 0;

	virtual BOOL							BindTextures(BOOL enableAsyncBinding) = 0;
	virtual BOOL							IsTextureBound(RuTextureChannel texChannel) = 0;

#ifdef KALYDO
	virtual BOOL							GetTextures(std::vector<CRuString>& dependencies) = 0;
#endif // KALYDO
};

// ************************************************************************************************************************************************************

class CRuMaterial : public IRuMaterial
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct RuMatTexture
	{
		RuTextureChannel					m_textureChannel;											//!< Texture channel associated with the texture
		char*								m_textureBindingName;										//!< Texture binding name
		RuSurfaceFormat						m_textureBindingFormat;										//!< Texture format to use when binding
		BOOL								m_textureBindingFailed;										//!< Texture binding failed?
		BOOL								m_textureIsEmbedded;										//!< Is the texture an embedded texture?
		IRuBaseTexture*						m_texture;													//!< The texture
	};

	CRuArrayList<RuMatTexture>				m_textures;													//!< List of textures used by this material
	char*									m_shaderName;												//!< Name of shader used by this material
	IRuShader*								m_shader;													//!< Shader used by this material
	IRuShader*								m_shaderOverride;											//!< Overriding shader
	CRuParameterBlock						m_parameterBlock;											//!< Parameter block
	INT32									m_materialPriority;

	BOOL									m_isDeprecatedMaterial;
	RuMaterial_Type							m_materialType;												//!< Material type
	RuMaterial_LightingType					m_lightingType;												//!< Lighting type
//	BOOL									m_enableGloss;												//!< Enable gloss(specular)?
	BOOL									m_isSkinned;												//!< Enable skinning?

	BOOL									m_updateRequired;

public:
											CRuMaterial();
	virtual									~CRuMaterial();

	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);

	// IRuObject serialization
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Memory reporting
	virtual INT32							GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	virtual BOOL							SetProperty(const char *propertyName, const RuPropertyType &propertyValue);

	virtual DWORD							GetDiffuse();
	virtual DWORD							GetSpecular();

	virtual INT32							GetNumTextureChannels();
	virtual RuTextureChannel				GetTextureChannel(INT32 index);
	virtual BOOL							ChannelExists(RuTextureChannel texChannel);
	BOOL									SetTextureChannel(INT32 index, RuTextureChannel texChannel);

	INT32									AddTextureChannel(RuTextureChannel texChannel);
	BOOL									DeleteTextureChannelAtIndex(INT32 index);

	virtual IRuBaseTexture*					GetTexture(RuTextureChannel texChannel);
	virtual void							SetTexture(RuTextureChannel texChannel, IRuBaseTexture *texture);
	virtual void							SetTextureByName(RuTextureChannel texChannel, const char *textureName, RuSurfaceFormat textureFormat = ruSURFACEFMT_DEFAULT);
	virtual void							SetTextureIsEmbedded(RuTextureChannel texChannel, BOOL isEmbedded);

	virtual const char*						GetShaderName();
	void									SetShaderName(const char *shaderName);
	virtual IRuShader*						GetShader();
	virtual void							SetShader(IRuShader *shader);

	virtual IRuShader*						GetShaderOverride();
	virtual void							SetShaderOverride(IRuShader *shader);

	virtual CRuParameterBlock*				GetParameterBlock();

	virtual INT32							GetMaterialPriority();
	virtual void							SetMaterialPriority(INT32 priority);

	virtual RuMaterial_Type					GetMaterialType();
	virtual void							SetMaterialType(RuMaterial_Type materialType);

	virtual RuMaterial_LightingType			GetLightingType();
	virtual void							SetLightingType(RuMaterial_LightingType lightingType);

	virtual BOOL							IsSkinned();
	virtual void							SetSkinned(BOOL isSkinned);

	virtual BOOL							BindTextures(BOOL enableAsyncBinding);
	virtual BOOL							IsTextureBound(RuTextureChannel texChannel);

	CRuMaterial&							operator=(CRuMaterial &material);

#ifdef KALYDO
	virtual BOOL							GetTextures(std::vector<CRuString>& dependencies);
#endif

protected:
	void									UpdateMaterialState();
	BOOL									BindTexture(INT32 index, BOOL enableAsyncBinding);
	void									SelectShader();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Material, CRuMaterial)

// ************************************************************************************************************************************************************

inline IRuMaterial::~IRuMaterial()
{
}

// ************************************************************************************************************************************************************

inline void CRuMaterial::UpdateMaterialState()
{
	if(m_updateRequired)
	{
		m_updateRequired = FALSE;

		SelectShader();
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
