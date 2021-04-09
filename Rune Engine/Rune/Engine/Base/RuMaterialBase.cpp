#include "../../Engine/Base/RuMaterialBase.h"
#include "../../Engine/Texture/RuTexture_Proxy.h"
#include "../../Engine/Texture/RuTextureUtility.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(IRuMaterial, IRuObject)
ruCLASSGUID_DEFINE(IRuMaterial, 0x8129E034, 0xBD474b3d, 0xBC2C5D54, 0x7F8852FB)

ruRTTI_DEFINE_SUBCLASS(CRuMaterial, IRuMaterial)
ruHEAP_DEFINE_SUBCLASS(CRuMaterial, "CRuMaterial", "IRuObject")
ruCLASSGUID_DEFINE(CRuMaterial, 0xCEE4F7EB, 0x85FA4127, 0xA4751BE8, 0x5BA6ADCC)

// ************************************************************************************************************************************************************

CRuMaterial::CRuMaterial()
:	m_shaderName(NULL),
	m_shader(NULL),
	m_shaderOverride(NULL),
	m_materialPriority(1000),
	m_isDeprecatedMaterial(FALSE),
	m_materialType(ruMATERIAL_TYPE_STANDARD),
	m_lightingType(ruMATERIAL_LIGHTINGTYPE_STANDARD),
	m_isSkinned(FALSE),
	m_updateRequired(TRUE)
{
	// Setup default material parameters
	m_parameterBlock.SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, 0);

	m_parameterBlock.SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_GREATER);

	m_parameterBlock.SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, ruALPHAMODE_NORMAL);
	m_parameterBlock.SetParameter(ruPARAMTYPE_MATERIAL_TRANSLUCENT, FALSE);

	m_parameterBlock.SetParameter(ruPARAMTYPE_MATERIAL_ZTESTMODE, ruZTESTMODE_DEFAULT);
	m_parameterBlock.SetParameter(ruPARAMTYPE_MATERIAL_ZWRITEMODE, ruZWRITEMODE_DEFAULT);

	m_parameterBlock.SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(1.0f));
	m_parameterBlock.SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODIFIER, RuCastFloatToINT64(1.0f));
}

CRuMaterial::~CRuMaterial()
{
	// Release overriding shader
	ruSAFE_RELEASE(m_shaderOverride);

	// Free all textures
	for(INT32 i = 0; i < m_textures.Count(); i++)
	{
		delete[] m_textures[i].m_textureBindingName;
		ruSAFE_RELEASE(m_textures[i].m_texture);
	}
	m_textures.Clear();

	// Free shader name string
	if(m_shaderName)
		delete[] m_shaderName;
}

IRuObject *CRuMaterial::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuMaterial();

	// Chain cloning call to parent class
	IRuMaterial::Clone(clonedObject);

	// Shortcut reference to the cloned entity
	CRuMaterial *clonedMaterial = static_cast<CRuMaterial *>(clonedObject);

	// Use assignment operator to copy parameters
	*clonedMaterial = *this;

	return clonedMaterial;
}

BOOL CRuMaterial::SerializeFrom(IRuStream *stream)
{
	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read signature and version
	INT32 signature;
	INT32 version = 1;
	stream->Read(&signature, sizeof(INT32));

	if(signature == 0xFFFFFFFF)
	{
		// Signature found, read version 
		stream->Read(&version, sizeof(INT32));
	}
	else
	{
		// No signature, this is an old model stream, so rewind
		stream->Seek(-4, ruSSM_Current);
	}

	switch(version)
	{
		case 1:
			{
				// Read shader name length & name (including NULL)
				INT32 shaderNameLength = 0;
				stream->Read(&shaderNameLength, sizeof(INT32));

				m_shaderName = ruNEW char [shaderNameLength];
				stream->Read(m_shaderName, shaderNameLength * sizeof(BYTE));

				// Set shader
				IRuShader *shader = NULL;
				if(m_shaderName && strlen(m_shaderName) > 0)
				{
					shader = g_ruShaderManager->GetShader(m_shaderName);
				}

				if(shader == NULL)
				{
					shader = g_ruShaderManager->GetShader("ruSHADER_STATIC_SH9");
				}

				SetShader(shader);

				m_isDeprecatedMaterial = TRUE;
			}

			break;

		case 2:
			{
				// Read material type
				stream->Read(&m_materialType, sizeof(INT32));

				// Read lighting type
				stream->Read(&m_lightingType, sizeof(INT32));

				m_isDeprecatedMaterial = FALSE;
			}

			break;

		case 3:
		case 4:
			{
				// Chain serialization to base class
				IRuMaterial::SerializeFrom(stream);

				// Read deprecated flag
				stream->Read(&m_isDeprecatedMaterial, sizeof(BOOL));

				// Read shader name length & name (including NULL)
				INT32 shaderNameLength = 0;
				stream->Read(&shaderNameLength, sizeof(INT32));

				m_shaderName = ruNEW char [shaderNameLength];
				stream->Read(m_shaderName, shaderNameLength * sizeof(BYTE));

				// Read material type
				stream->Read(&m_materialType, sizeof(INT32));

				// Read lighting type
				stream->Read(&m_lightingType, sizeof(INT32));

				if(version > 3)
				{
					// Read skinned toggle
					stream->Read(&m_isSkinned, sizeof(BOOL));
				}

				if(m_isDeprecatedMaterial)
				{
					// Set shader
					IRuShader *shader = NULL;
					if(m_shaderName && strlen(m_shaderName) > 0)
					{
						shader = g_ruShaderManager->GetShader(m_shaderName);
					}

					if(shader == NULL)
					{
						shader = g_ruShaderManager->GetShader("ruSHADER_STATIC_SH9");
					}

					SetShader(shader);
				}
			}

			break;
	}

	// Read number of texture channels
	INT32 numTexChannels;
	stream->Read(&numTexChannels, sizeof(INT32));

	if(numTexChannels > 0)
	{
		// Read each texture channel
		for(INT32 i = 0; i < numTexChannels; ++i)
		{
			// Read texture channel ID
			RuTextureChannel texChannel;
			stream->Read(&texChannel, sizeof(RuTextureChannel));

			// Read texture name length
			INT32 textureNameLength;
			stream->Read(&textureNameLength, sizeof(INT32));

			// Read texture name
			char textureName[2048];
			stream->Read(textureName, textureNameLength * sizeof(BYTE));

			// Bind texture name to channel
			this->SetTextureByName(texChannel, textureName);

			if(version >= 3)
			{
				// Read embedded texture stream size
				INT32 embeddedTextureStreamSize = 0;
				stream->Read(&embeddedTextureStreamSize, sizeof(INT32));

				if(embeddedTextureStreamSize > 0)
				{
					IRuObject *embeddedTexObj = RuObjectFactory().CreateObject(stream);

					if(embeddedTexObj)
					{
						if(embeddedTexObj->GetType().IsSubClassOf(IRuBaseTexture::Type()))
						{
							this->SetTexture(texChannel, static_cast<IRuBaseTexture *>(embeddedTexObj));
						}
						else
						{
							ruSAFE_RELEASE(embeddedTexObj);
						}
					}
				}
			}
		}
	}

	// Read number of parameters
	INT32 numParameters;
	stream->Read(&numParameters, sizeof(INT32));

	// Read each parameter
	for(INT32 i = 0; i < numParameters; ++i)
	{
		// Read parameter type
		RuParameterType paramType;
		stream->Read(&paramType, sizeof(RuParameterType));

		// Read parameter length
		INT32 paramLength = 0;
		stream->Read(&paramLength, sizeof(INT32));

		switch(paramType)
		{
			case ruPARAMTYPE_MATERIAL_2SIDED:
			case ruPARAMTYPE_MATERIAL_FACETED:
			case ruPARAMTYPE_MATERIAL_WIRE:
			case ruPARAMTYPE_MATERIAL_SMOOTHBLEND:
				{
					BOOL paramValue = FALSE;
					stream->Read(&paramValue, sizeof(BOOL));
					m_parameterBlock.SetParameter(paramType, paramValue);
				}

				break;

			case ruPARAMTYPE_MATERIAL_ALPHATESTMODE:
			case ruPARAMTYPE_MATERIAL_ALPHATESTVALUE:
			case ruPARAMTYPE_MATERIAL_ALPHAMODE:
			case ruPARAMTYPE_MATERIAL_TRANSLUCENT:
			case ruPARAMTYPE_MATERIAL_ZWRITEMODE:
				{
					UINT32 paramValue = 0;
					stream->Read(&paramValue, sizeof(UINT32));
					m_parameterBlock.SetParameter(paramType, paramValue);
				}

				break;

			case ruPARAMTYPE_MATERIAL_SELFILLUMINATION:
			case ruPARAMTYPE_MATERIAL_ALPHA:
				{
					REAL paramValue = 0.0f;
					stream->Read(&paramValue, sizeof(REAL));
					m_parameterBlock.SetParameter(paramType, RuCastFloatToINT64(paramValue));
				}

				break;

			default:
				{
					// Skip parameter
					stream->Seek(paramLength, ruSSM_Current);
				}

				break;
		}
	}

	// Determine shader to use
	SelectShader();

	return TRUE;
}

BOOL CRuMaterial::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write signature
	INT32 signature = 0xFFFFFFFF;
	stream->Write(&signature, sizeof(INT32));

	// Write version
	INT32 version = 4;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuMaterial::SerializeTo(stream);

	// Write deprecated flag
	stream->Write(&m_isDeprecatedMaterial, sizeof(BOOL));

	// Write shader name length & name (including NULL)
	INT32 shaderNameLength = 1;
	const char *shaderName = "";
	if(m_shaderName)
	{
		shaderNameLength = (INT32)strlen(m_shaderName) + 1;
		shaderName = m_shaderName;
	}

	stream->Write(&shaderNameLength, sizeof(INT32));
	stream->Write(shaderName, shaderNameLength * sizeof(BYTE));

	// Write material type
	stream->Write(&m_materialType, sizeof(INT32));

	// Write lighting type
	stream->Write(&m_lightingType, sizeof(INT32));

	// Write skinned toggle
	stream->Write(&m_isSkinned, sizeof(BOOL));

	// Write number of texture channels
	INT32 numTexChannels = m_textures.Count();
	stream->Write(&numTexChannels, sizeof(INT32));

	// Write each texture channel
	for(INT32 i = 0; i < numTexChannels; ++i)
	{
		// Write texture channel ID
		stream->Write(&m_textures[i].m_textureChannel, sizeof(RuTextureChannel));

		INT32 textureNameLength = 1;
		const char *textureName = "";

		if(m_textures[i].m_textureBindingName)
		{
			textureNameLength = (INT32)strlen(m_textures[i].m_textureBindingName) + 1;
			textureName = m_textures[i].m_textureBindingName;
		}

		// Write texture name length (including NULL)
		stream->Write(&textureNameLength, sizeof(INT32));

		// Write texture name
		stream->Write(textureName, textureNameLength * sizeof(BYTE));

		CRuMemoryStream *textureStream = ruNEW CRuMemoryStream();

		if(m_textures[i].m_textureIsEmbedded)
		{
			if(m_textures[i].m_texture)
			{
				m_textures[i].m_texture->SerializeTo(textureStream);
			}
		}

		// Serialize entire texture to stream
		INT32 embeddedTextureStreamSize = textureStream->GetStreamSize();
		stream->Write(&embeddedTextureStreamSize, sizeof(INT32));
		stream->Write(textureStream->GetBuffer(), textureStream->GetStreamSize());

		delete textureStream;
	}

	// Write number of parameters
	INT32 numParameters = m_parameterBlock.GetNumParameters();
	stream->Write(&numParameters, sizeof(INT32));

	// Write parameters
	for(INT32 i = 0; i < numParameters; ++i)
	{
		// Get parameter tuple
		RuParameterType paramType;
		UINT64 paramValue;
		m_parameterBlock.GetParameter(i, paramType, paramValue);

		// Write parameter type
		stream->Write(&paramType, sizeof(RuParameterType));

		switch(paramType)
		{
			case ruPARAMTYPE_MATERIAL_2SIDED:
			case ruPARAMTYPE_MATERIAL_FACETED:
			case ruPARAMTYPE_MATERIAL_WIRE:
			case ruPARAMTYPE_MATERIAL_SMOOTHBLEND:
				{
					// Write parameter length
					INT32 paramLength = sizeof(BOOL);
					stream->Write(&paramLength, sizeof(INT32));

					// Write parameter value
					BOOL realValue = (BOOL)paramValue;
					stream->Write(&realValue, sizeof(BOOL));
				}

				break;

			case ruPARAMTYPE_MATERIAL_ALPHATESTMODE:
			case ruPARAMTYPE_MATERIAL_ALPHATESTVALUE:
			case ruPARAMTYPE_MATERIAL_ALPHAMODE:
			case ruPARAMTYPE_MATERIAL_TRANSLUCENT:
			case ruPARAMTYPE_MATERIAL_ZWRITEMODE:
				{
					// Write parameter length
					INT32 paramLength = sizeof(UINT32);
					stream->Write(&paramLength, sizeof(INT32));

					// Write parameter value
					UINT32 realValue = (UINT32)paramValue;
					stream->Write(&realValue, sizeof(UINT32));
				}

				break;

			case ruPARAMTYPE_MATERIAL_SELFILLUMINATION:
			case ruPARAMTYPE_MATERIAL_ALPHA:
				{
					// Write parameter length
					INT32 paramLength = sizeof(REAL);
					stream->Write(&paramLength, sizeof(INT32));

					// Write parameter value
					REAL realValue = RuCastINT64ToFloat(paramValue);
					stream->Write(&realValue, sizeof(REAL));
				}

				break;

			default:
				{
					// Write parameter length (always zero for unrecognized types)
					INT32 paramLength = 0;
					stream->Write(&paramLength, sizeof(INT32));
				}

				break;
		}
	}

	return TRUE;
}

INT32 CRuMaterial::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuObject::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	footprint += sizeof(*this);

	footprint += m_textures.GetMemoryFootprint();
	for(INT32 i = 0; i < m_textures.Count(); ++i)
	{
		footprint += sizeof(m_textures[i].m_textureChannel);
		footprint += sizeof(m_textures[i].m_textureBindingName) + (m_textures[i].m_textureBindingName ? (INT32)strlen(m_textures[i].m_textureBindingName) + 1 : 0);
		footprint += sizeof(m_textures[i].m_textureIsEmbedded);
		footprint += sizeof(m_textures[i].m_texture) + (m_textures[i].m_texture ? RuTextureUtility_GetMemoryFootprint(m_textures[i].m_texture, enumHash) : 0);
	}
	footprint += (m_shaderName ? (INT32)strlen(m_shaderName) + 1 : 0);

	return footprint;
}

BOOL CRuMaterial::SetProperty(const char *propertyName, const RuPropertyType &propertyValue)
{
	m_updateRequired = TRUE;
	return IRuMaterial::SetProperty(propertyName, propertyValue);
}

DWORD CRuMaterial::GetDiffuse()
{
	UpdateMaterialState();

	return 0xFFFFFFFF;
}

DWORD CRuMaterial::GetSpecular()
{
	UpdateMaterialState();

	return 0xFFFFFFFF;
}

INT32 CRuMaterial::GetNumTextureChannels()
{
	UpdateMaterialState();

	return m_textures.Count();
}

RuTextureChannel CRuMaterial::GetTextureChannel(INT32 index)
{
	UpdateMaterialState();

	if(index >= 0 && index < (INT32) m_textures.Count())
		return m_textures[index].m_textureChannel;

	return ruTEXCHANNEL_INVALID;
}

BOOL CRuMaterial::ChannelExists(RuTextureChannel texChannel)
{
	for(INT32 i = 0; i < m_textures.Count(); ++i)
	{
		if(m_textures[i].m_textureChannel == texChannel)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuMaterial::SetTextureChannel(INT32 index, RuTextureChannel texChannel)
{
	if(index >= 0 && index < (INT32) m_textures.Count())
	{
		m_textures[index].m_textureChannel = texChannel;
		return TRUE;
	}

	return FALSE;
}

INT32 CRuMaterial::AddTextureChannel(RuTextureChannel texChannel)
{
	// Add new texture channel
	RuMatTexture matTexture;
	matTexture.m_textureChannel = texChannel;
	matTexture.m_textureBindingName = NULL;
	matTexture.m_textureIsEmbedded = FALSE;
	matTexture.m_texture = NULL;
	m_textures.Add(matTexture);

	return m_textures.Count() - 1;
}

BOOL CRuMaterial::DeleteTextureChannelAtIndex(INT32 index)
{
	if(index >= 0 && index < (INT32) m_textures.Count())
	{
		// Release the texture and remove the texture channel
		delete[] m_textures[index].m_textureBindingName;
		ruSAFE_RELEASE(m_textures[index].m_texture)
		m_textures.RemoveAt(index);

		return TRUE;
	}

	return FALSE;
}

IRuBaseTexture *CRuMaterial::GetTexture(RuTextureChannel texChannel)
{
	UpdateMaterialState();

	for(INT32 i = 0; i < m_textures.Count(); i++)
	{
		if(m_textures[i].m_textureChannel == texChannel)
		{
			return m_textures[i].m_texture;
		}
	}

	return NULL;
}

void CRuMaterial::SetTexture(RuTextureChannel texChannel, IRuBaseTexture *texture)
{
	for(INT32 i = 0; i < m_textures.Count(); i++)
	{
		if(m_textures[i].m_textureChannel == texChannel)
		{
			if(texture == NULL)
			{
				// Delete the texture binding name
				delete[] m_textures[i].m_textureBindingName;
				m_textures[i].m_textureBindingName = NULL;

				// Release old texture
				ruSAFE_RELEASE(m_textures[i].m_texture);

				// Delete texture channel entry
				m_textures.RemoveAt(i);

				return;
			}
			else
			{
				// Increment texture reference count
				texture->AddRef();

				// Delete the texture binding name
				delete[] m_textures[i].m_textureBindingName;
				m_textures[i].m_textureBindingName = NULL;

				// Release old texture
				ruSAFE_RELEASE(m_textures[i].m_texture);

				// Store bound texture name
				m_textures[i].m_textureBindingName = ruNEW char [strlen(texture->GetTextureName()) + 1];
				strcpy(m_textures[i].m_textureBindingName, texture->GetTextureName());

				// Set new texture into texture channel
				m_textures[i].m_texture = texture;

				return;
			}
		}
	}

	// Texture channel not found, so add the new channel if the specified texture is non-null
	if(texture == NULL)
		return;

	// Increment texture reference count
	texture->AddRef();

	// Add new texture channel with the specified texture
	RuMatTexture matTexture;
	matTexture.m_textureChannel = texChannel;
	matTexture.m_textureBindingName = ruNEW char [strlen(texture->GetTextureName()) + 1];
	strcpy(matTexture.m_textureBindingName, texture->GetTextureName());
	matTexture.m_textureBindingFormat = ruSURFACEFMT_DEFAULT;
	matTexture.m_textureBindingFailed = FALSE;
	matTexture.m_textureIsEmbedded = FALSE;
	matTexture.m_texture = texture;
	m_textures.Add(matTexture);
}

void CRuMaterial::SetTextureByName(RuTextureChannel texChannel, const char *textureName, RuSurfaceFormat textureFormat)
{
	for(INT32 i = 0; i < m_textures.Count(); i++)
	{
		if(m_textures[i].m_textureChannel == texChannel)
		{
			if(textureName == NULL)
			{
				// Delete the texture binding name
				delete[] m_textures[i].m_textureBindingName;
				m_textures[i].m_textureBindingName = NULL;

				// Release old texture
				ruSAFE_RELEASE(m_textures[i].m_texture);

				// Delete texture channel entry
				m_textures.RemoveAt(i);

				return;
			}
			else
			{
				// Delete the texture binding name
				delete[] m_textures[i].m_textureBindingName;
				m_textures[i].m_textureBindingName = NULL;

				// Release old texture
				ruSAFE_RELEASE(m_textures[i].m_texture);

				// Set new texture into texture channel
				m_textures[i].m_textureBindingName = ruNEW char [strlen(textureName) + 1];
				strcpy(m_textures[i].m_textureBindingName, textureName);
				m_textures[i].m_texture = NULL;

				m_textures[i].m_textureBindingFormat = textureFormat;
				m_textures[i].m_textureBindingFailed = FALSE;

				return;
			}
		}
	}

	// Texture channel not found, so add the new channel if the specified texture is non-null
	if(textureName == NULL)
		return;

	// Add new texture channel with the specified texture
	RuMatTexture matTexture;
	matTexture.m_textureChannel = texChannel;
	matTexture.m_textureBindingName = ruNEW char [strlen(textureName) + 1];
	strcpy(matTexture.m_textureBindingName, textureName);
	matTexture.m_textureBindingFormat = textureFormat;
	matTexture.m_textureBindingFailed = FALSE;
	matTexture.m_textureIsEmbedded = FALSE;
	matTexture.m_texture = NULL;
	m_textures.Add(matTexture);
}

void CRuMaterial::SetTextureIsEmbedded(RuTextureChannel texChannel, BOOL isEmbedded)
{
	for(INT32 i = 0; i < m_textures.Count(); i++)
	{
		if(m_textures[i].m_textureChannel == texChannel)
		{
			m_textures[i].m_textureIsEmbedded = isEmbedded;
			return;
		}
	}

	// Add new texture channel with the specified texture
	RuMatTexture matTexture;
	matTexture.m_textureChannel = texChannel;
	matTexture.m_textureBindingName = NULL;
	matTexture.m_textureBindingFormat = ruSURFACEFMT_DEFAULT;
	matTexture.m_textureBindingFailed = FALSE;
	matTexture.m_textureIsEmbedded = isEmbedded;
	matTexture.m_texture = NULL;
	m_textures.Add(matTexture);
}

const char *CRuMaterial::GetShaderName()
{
	UpdateMaterialState();

	return m_shaderName;
}

void CRuMaterial::SetShaderName(const char *shaderName)
{
	if(m_shaderName)
		delete[] m_shaderName;

	m_shaderName = ruNEW char [strlen(shaderName) + 1];
	strcpy(m_shaderName, shaderName);
}

IRuShader *CRuMaterial::GetShader()
{
	UpdateMaterialState();

	return m_shaderOverride ? m_shaderOverride : m_shader;
}

void CRuMaterial::SetShader(IRuShader *shader)
{
	if(shader)
	{
		m_shader = shader;
		this->SetShaderName(shader->GetShaderName());
		m_isDeprecatedMaterial = TRUE;
	}
}

IRuShader *CRuMaterial::GetShaderOverride()
{
	return m_shaderOverride;
}

void CRuMaterial::SetShaderOverride(IRuShader *shader)
{
	if(shader)
	{
		shader->AddRef();
		ruSAFE_RELEASE(m_shaderOverride);
		m_shaderOverride = shader;
	}
	else
	{
		ruSAFE_RELEASE(m_shaderOverride);
	}
}

CRuParameterBlock *CRuMaterial::GetParameterBlock()
{
	return &m_parameterBlock;
}

INT32 CRuMaterial::GetMaterialPriority()
{
	return m_materialPriority;
}

void CRuMaterial::SetMaterialPriority(INT32 priority)
{
	m_materialPriority = priority;
}

RuMaterial_Type CRuMaterial::GetMaterialType()
{
	return m_materialType;
}

void CRuMaterial::SetMaterialType(RuMaterial_Type materialType)
{
	m_materialType = materialType;
	m_updateRequired = TRUE;
}

RuMaterial_LightingType CRuMaterial::GetLightingType()
{
	return m_lightingType;
}

void CRuMaterial::SetLightingType(RuMaterial_LightingType lightingType)
{
	m_lightingType = lightingType;
	m_updateRequired = TRUE;
}

BOOL CRuMaterial::IsSkinned()
{
	return m_isSkinned;
}

void CRuMaterial::SetSkinned(BOOL isSkinned)
{
	if(m_isSkinned != isSkinned)
	{
		m_isSkinned = isSkinned;
		m_updateRequired = TRUE;

		// Re-select shader
		SelectShader();
	}
}

#ifdef KALYDO
BOOL CRuMaterial::GetTextures(std::vector<CRuString>& dependencies)
{
	for(INT32 i = 0; i < m_textures.Count(); ++i)
	{
		if(m_textures[i].m_textureBindingName)
			dependencies.push_back(m_textures[i].m_textureBindingName);
	}

	return TRUE;
}
#endif //KALYDO

BOOL CRuMaterial::BindTextures(BOOL enableAsyncBinding)
{
	BOOL allTexturesAreBound = TRUE;

	for(INT32 i = 0; i < m_textures.Count(); ++i)
	{
		allTexturesAreBound = allTexturesAreBound & BindTexture(i, enableAsyncBinding);
	}

	return allTexturesAreBound;
}

BOOL CRuMaterial::IsTextureBound(RuTextureChannel texChannel)
{
	for(INT32 i = 0; i < m_textures.Count(); ++i)
	{
		if(m_textures[i].m_textureChannel == texChannel)
		{
			if(m_textures[i].m_texture)
			{
				// If the texture is either not invalidated, or has been discarded, it is considered to be bound
				if(m_textures[i].m_texture->IsInvalidated() == FALSE || m_textures[i].m_texture->IsDiscarded())
				{
					return TRUE;
				}
			}
			else if(m_textures[i].m_textureBindingFailed)
			{
				// If binding failed, consider the bind complete
				return TRUE;
			}

			return FALSE;
		}
	}

	return FALSE;
}

CRuMaterial &CRuMaterial::operator=(CRuMaterial &material)
{
	// Copy property settings from source material
	CopyPropertiesFrom(&material);

	// Copy texture bindings
	for(INT32 i = 0; i < material.m_textures.Count(); ++i)
	{
		if(material.m_textures[i].m_textureBindingName)
		{
			SetTextureByName(material.m_textures[i].m_textureChannel, material.m_textures[i].m_textureBindingName);
		}

		if(material.m_textures[i].m_texture)
		{
			SetTexture(material.m_textures[i].m_textureChannel, material.m_textures[i].m_texture);
		}
	}

	// Copy shader name
	if(material.m_shaderName)
	{
		m_shaderName = ruNEW char [strlen(material.m_shaderName) + 1];
		strcpy(m_shaderName, material.m_shaderName);
	}

	// Copy other material settings
	m_shader = material.m_shader;
	m_parameterBlock = material.m_parameterBlock;
	m_materialPriority = material.m_materialPriority;

	m_isDeprecatedMaterial = material.m_isDeprecatedMaterial;
	m_materialType = material.m_materialType;
	m_lightingType = material.m_lightingType;
	m_isSkinned = material.m_isSkinned;

	m_updateRequired = material.m_updateRequired;

	return *this;
}

BOOL CRuMaterial::BindTexture(INT32 index, BOOL enableAsyncBinding)
{
	if(m_textures[index].m_texture == NULL && m_textures[index].m_textureBindingFailed == FALSE)
	{
		if(m_textures[index].m_textureBindingName)
		{
			// Retrieve texture loading preferences from the engine
			RuPropertyType propertyValue;
			INT32 texture_MaxDetailLevel = g_ruEngineSettings->GetSetting("texture:max detail level", propertyValue) ? boost::get<INT32>(propertyValue) : 4;
			INT32 texture_MinTextureSize = g_ruEngineSettings->GetSetting("texture:min texture size", propertyValue) ? boost::get<INT32>(propertyValue) : 256;
			INT32 maxMipLevel = max(0, 4 - texture_MaxDetailLevel);

			// Load texture
			m_textures[index].m_texture = g_ruResourceManager->LoadTexture(m_textures[index].m_textureBindingName, m_textures[index].m_textureBindingFormat, maxMipLevel, enableAsyncBinding);

			// Mark texture binding as failed if no texture was returned
			if(m_textures[index].m_texture == NULL)
			{
				m_textures[index].m_textureBindingFailed = TRUE;
			}
		}
	}

	if(m_textures[index].m_texture)
	{
		// The texture is considered to be bound when it is not in an invalidated state, or if it has been discarded by the resource manager
		return (m_textures[index].m_texture->IsInvalidated() == FALSE) || m_textures[index].m_texture->IsDiscarded();
	}

#ifdef KALYDO
	DownloadState ds = g_ruResourceManager->GetDownloadState(m_textures[index].m_textureBindingName);
	if (ds == PENDING || ds == PENDING_QUEUE)
		// wait for download
		return FALSE;
#endif // KALYDO
	return TRUE;
}

void CRuMaterial::SelectShader()
{
	RuPropertyType propertyValue;

	// Check material type
	const char *materialType = this->GetProperty("material:materialType", propertyValue) ? boost::get<const char *>(propertyValue) : "Default Material";
	if(stricmp(materialType, "Standard Material") == 0)
	{
		m_isDeprecatedMaterial = FALSE;
	}

	// Abort if shader manager is unavailable
	if(g_ruShaderManager == NULL)
	{
		return;
	}

	if(m_isDeprecatedMaterial)
	{
		if(	stricmp(m_shaderName, "ruSHADER_STATIC_FLAT") == 0)
		{
			GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATIONCOLOR, RUCOLOR_ARGB(0, 255, 255, 255));
			GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(1.0f));

			m_shader = g_ruShaderManager->GetShader("ruSHADER_STATIC");
		}

		if(	stricmp(m_shaderName, "ruSHADER_STATIC_Z") == 0)
		{
			GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_COLORWRITEENABLE, FALSE);

			m_shader = g_ruShaderManager->GetShader("ruSHADER_STATIC");
		}

		if(	stricmp(m_shaderName, "ruSHADER_STATIC_FX") == 0 ||
			stricmp(m_shaderName, "ruSHADER_STATIC_FX_DISTORT") == 0 ||
			stricmp(m_shaderName, "ruSHADER_STATIC_FX_DISTORT_PROJECTED") == 0 ||
			stricmp(m_shaderName, "ruSHADER_STATIC_PRELIT") == 0 ||
			stricmp(m_shaderName, "ruSHADER_STATIC_PRELIT_TRANSLUCENT") == 0 ||
			stricmp(m_shaderName, "ruSHADER_STATIC_SH9") == 0 ||
			stricmp(m_shaderName, "ruSHADER_STATIC_SH9_GLOSS") == 0 ||
			stricmp(m_shaderName, "ruSHADER_STATIC_SH9_COLORED") == 0 ||
			stricmp(m_shaderName, "ruSHADER_STATIC_SH9_COLORED_GLOSS") == 0 ||
			stricmp(m_shaderName, "ruSHADER_STATIC_SH9_TRANSLUCENT") == 0)
		{
			m_shader = g_ruShaderManager->GetShader("ruSHADER_STATIC");
		}
	}
	else
	{
		// Determine whether the material has a gloss map
		BOOL hasGlossMap = GetTexture(ruTEXCHANNEL_GLOSSMAP) ? TRUE : FALSE;

		// Enable distort FX?
		BOOL engine_DistortFXEnabled = g_ruEngineSettings->GetProperty("distortFX:enable", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;
		BOOL distortFXEnable = this->GetProperty("material:distortFX", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;

		// Enable dye FX?
		BOOL dyeFXEnable = this->GetProperty("material:dyeFX", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;

		// Enable gloss?
		BOOL glossEnable = this->GetProperty("material:gloss", propertyValue) ? boost::get<BOOL>(propertyValue) : TRUE;
		glossEnable = glossEnable && hasGlossMap;

		// Set parameters based on material type
		switch(m_materialType)
		{
			case ruMATERIAL_TYPE_STANDARD:
				{
				}
				break;

			case ruMATERIAL_TYPE_TRANSLUCENT:
				{
				}
				break;

			case ruMATERIAL_TYPE_FX:
				{
				}
				break;
		}

		// Set parameters based on lighting type
		switch(m_lightingType)
		{
			default:
			case ruMATERIAL_LIGHTINGTYPE_STANDARD:
			case ruMATERIAL_LIGHTINGTYPE_COLORED:
			case ruMATERIAL_LIGHTINGTYPE_RESERVED:
				{
				}
				break;

			case ruMATERIAL_LIGHTINGTYPE_PRELIT:
				{
					// Pre-lit material means full self-illumination and all color comes from vertex color/texture color
					GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATIONCOLOR, RUCOLOR_ARGB(0, 255, 255, 255));
					GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(1.0f));
				}
				break;
		}

		// Select shader
		if(m_isSkinned)
		{
			if(dyeFXEnable)
			{
				m_shader = g_ruShaderManager->GetShader("ruSHADER_PAPERDOLL");
			}
			else
			{
				m_shader = g_ruShaderManager->GetShader("ruSHADER_SKIN");
			}
		}
		else
		{
			m_shader = g_ruShaderManager->GetShader("ruSHADER_STATIC");
		}
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
