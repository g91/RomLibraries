#include "../../Scene/Paperdoll/RuEntityCompositeModel.h"

#include "../../Engine/Texture/RuTexture_Proxy.h"
#include "../../Engine/Texture/RuTextureUtility.h"
#include "../../ImageFX/RuImageFX_Blitter.h"
#include "../../../Tools/dxtLib/dxtutility.h"

#include "../../Helper/RuHelper_IRuEntity_Renderable.h"

//#include "../../Direct3D/Engine/RuEngineD3D.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuCompositeModelTemplate, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuCompositeModelTemplate, "CRuCompositeModelTemplate", "IRuObject")
ruCLASSGUID_DEFINE(CRuCompositeModelTemplate, 0x2BBCDBB6, 0xAA3A481a, 0x9ADA9A1B, 0x6ABC1F31)

ruRTTI_DEFINE_SUBCLASS(CRuCompositeModel, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuCompositeModel, "CRuCompositeModel", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuCompositeModel, 0xDDE985A6, 0x1B694655, 0x963D291A, 0xB0E22E38)

// ************************************************************************************************************************************************************

CRuCompositeModelTemplate::CRuCompositeModelTemplate()
:	m_assemblyRule(NULL)
{
}

CRuCompositeModelTemplate::~CRuCompositeModelTemplate()
{
	// Delete all part descriptors
	for(INT32 i = 0; i < m_descriptors.Count(); ++i)
	{
		delete m_descriptors[i];
	}

	// Delete assembly rule string
	delete[] m_assemblyRule;
}

BOOL CRuCompositeModelTemplate::SerializeFrom(IRuStream *stream)
{
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
				// Read length of descriptor string including NULL
				INT32 descriptorLength;
				stream->Read(&descriptorLength, sizeof(descriptorLength));

				// Read descriptor string including NULL
				char *descriptorString = ruNEW char [descriptorLength];
				stream->Read(descriptorString, descriptorLength);

				// Set descriptor string
				SetDescriptorString(descriptorString);
				delete[] descriptorString;
			}

			break;

		case 2:
			{
				// Chain serialization to parent class
				IRuObject::SerializeFrom(stream);

				// Read length of descriptor string including NULL
				INT32 descriptorLength;
				stream->Read(&descriptorLength, sizeof(descriptorLength));

				// Read descriptor string including NULL
				char *descriptorString = ruNEW char [descriptorLength];
				stream->Read(descriptorString, descriptorLength);

				// Set descriptor string
				SetDescriptorString(descriptorString);
				delete[] descriptorString;

				// Read length of assembly rule string including NULL
				INT32 assemblyRuleLength;
				stream->Read(&assemblyRuleLength, sizeof(assemblyRuleLength));

				if(assemblyRuleLength > 0)
				{
					// Read assembly rule string including NULL
					m_assemblyRule = ruNEW char [assemblyRuleLength];
					stream->Read(m_assemblyRule, assemblyRuleLength);
				}
			}

			break;
	}

	return TRUE;
}

BOOL CRuCompositeModelTemplate::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write signature
	INT32 signature = 0xFFFFFFFF;
	stream->Write(&signature, sizeof(INT32));

	// Write version
	INT32 version = 2;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to parent class
	IRuObject::SerializeTo(stream);

	// Write length of descriptor string including NULL
	INT32 descriptorLength = m_descriptorString.GetLength() + 1;
	stream->Write(&descriptorLength, sizeof(descriptorLength));

	// Write descriptor string including NULL
	stream->Write((char *) m_descriptorString, descriptorLength);

	// Write length of the assembly rule string including NULL
	INT32 assemblyRuleLength = m_assemblyRule ? ((INT32)strlen(m_assemblyRule) + 1) : 0;
	stream->Write(&assemblyRuleLength, sizeof(assemblyRuleLength));

	// Write assembly rule string including NULL
	if(m_assemblyRule)
	{
		stream->Write(m_assemblyRule, assemblyRuleLength);
	}

	return TRUE;
}

void CRuCompositeModelTemplate::SetDescriptorString(const char *descriptorString)
{
	// Delete all existing part descriptors
	for(INT32 i = 0; i < m_descriptors.Count(); ++i)
	{
		delete m_descriptors[i];
	}
	m_descriptors.Clear();

	// Copy descriptor string
	m_descriptorString = descriptorString;

	// Parse descriptor
	CRuStringTokenizer strTokenizer;
	strTokenizer.Tokenize(m_descriptorString, CRuString("\n\r"));

	// Parse each line in the descriptor
	for(INT32 i = 0; i < strTokenizer.GetNumTokens(); ++i)
	{
		CRuStringTokenizer lineTokenizer;
		lineTokenizer.Tokenize(strTokenizer.GetToken(i), CRuString(" "));

		// Descriptor format: [Part ID] [Part name] [X] [Y] [Width] [Height]
		if(lineTokenizer.GetNumTokens() == 6)
		{
			// Create new part descriptor
			PartDescriptor *partDescriptor = ruNEW PartDescriptor();

			// Parse part descriptor information
			partDescriptor->m_partID = lineTokenizer.GetTokenAsInteger(0);
			partDescriptor->m_partName = lineTokenizer.GetToken(1);
			partDescriptor->m_partTexureX = lineTokenizer.GetTokenAsInteger(2);
			partDescriptor->m_partTexureY = lineTokenizer.GetTokenAsInteger(3);
			partDescriptor->m_partTexureWidth = lineTokenizer.GetTokenAsInteger(4);
			partDescriptor->m_partTexureHeight = lineTokenizer.GetTokenAsInteger(5);

			// Append new part descriptor to the descriptor list
			m_descriptors.Add(partDescriptor);
		}
	}
}

const char *CRuCompositeModelTemplate::GetDescriptorString() const
{
	return (char *) m_descriptorString;
}

INT32 CRuCompositeModelTemplate::GetNumParts() const
{
	return m_descriptors.Count();
}

INT32 CRuCompositeModelTemplate::GetPartIndex(INT32 partID) const
{
	// Find a descriptor with part ID matching the specified part ID
	for(INT32 i = 0; i < m_descriptors.Count(); ++i)
	{
		if(m_descriptors[i]->m_partID == partID)
		{
			return i;
		}
	}

	// Return -1 if the specified part ID was not found
	return -1;
}

INT32 CRuCompositeModelTemplate::GetPartID(INT32 partIndex) const
{
	// Ensure the part index is within range
	if(partIndex >= 0 && partIndex < m_descriptors.Count())
	{
		// Return part ID
		return m_descriptors[partIndex]->m_partID;
	}

	return -1;
}

const char *CRuCompositeModelTemplate::GetPartName(INT32 partIndex) const
{
	// Ensure the part index is within range
	if(partIndex >= 0 && partIndex < m_descriptors.Count())
	{
		// Return part name
		return (char *) m_descriptors[partIndex]->m_partName;
	}

	return "";
}

BOOL CRuCompositeModelTemplate::GetPartTextureQuad(INT32 partIndex, INT32 &xOut, INT32 &yOut, INT32 &widthOut, INT32 &heightOut) const
{
	// Ensure the part index is within range
	if(partIndex >= 0 && partIndex < m_descriptors.Count())
	{
		// Return the texture quad parameters
		xOut = m_descriptors[partIndex]->m_partTexureX;
		yOut = m_descriptors[partIndex]->m_partTexureY;
		widthOut = m_descriptors[partIndex]->m_partTexureWidth;
		heightOut = m_descriptors[partIndex]->m_partTexureHeight;

		return TRUE;
	}

	return FALSE;
}

const char *CRuCompositeModelTemplate::GetAssemblyRule()
{
	return m_assemblyRule ? m_assemblyRule : "";
}

BOOL CRuCompositeModelTemplate::SetAssemblyRule(const char *assemblyRule)
{
	if(assemblyRule)
	{
		delete[] m_assemblyRule;
		m_assemblyRule = ruNEW char [strlen(assemblyRule) + 1];
		strcpy(m_assemblyRule, assemblyRule);
	}

	return FALSE;
}

CRuCompositeModel *CRuCompositeModelTemplate::Construct()
{
	CRuCompositeModel *compositeModel = ruNEW CRuCompositeModel();

	// Set template into the composite model and increment the template's reference count
	compositeModel->m_modelTemplate = this;
	this->AddRef();

	return compositeModel;
}

// ************************************************************************************************************************************************************

CRuCompositeModel::CRuCompositeModel()
:	m_maxMipLevel(0x7FFFFFFF),
	m_modelTemplate(NULL),
	m_enablePartCompositing(FALSE),
	m_modelIsBuilt(FALSE),
	m_textureRebuildRequired(FALSE),
	m_compositeMesh(NULL),
	m_compositeMaterial(NULL)
{
	// Read settings from engine settings
	RuPropertyType propertyValue;
	m_enablePartCompositing = g_ruEngineSettings->GetSetting("composite model:enable part compositing", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;

	// Set default max mip level to zero
	SetMaxMipLevel(0);

	// Create a model object
	m_compositeModel = ruNEW CRuModel();
	m_compositeModel->SetOption(ruENTITYOPTION_SKINNED, TRUE);

	// Attempt to load a system black texture
	m_sys_BlackTexture = g_ruResourceManager->LoadTexture("RUSYSTEM_BLACK");
}

CRuCompositeModel::~CRuCompositeModel()
{
	ruSAFE_RELEASE(m_compositeModel);
	ruSAFE_RELEASE(m_sys_BlackTexture);

	// Release references to submodels
	for(INT32 i = 0; i < m_modelParts.Count(); ++i)
	{
		// Decrement reference count on the old model
		ruSAFE_RELEASE(m_modelParts[i].m_model);
	}

	// Release reference to model template
	if(m_modelTemplate)
	{
		m_modelTemplate->Release();
	}

	// Release reference to composite mesh
	if(m_compositeMesh)
	{
		m_compositeMesh->Release();
	}

	// Release reference to composite material
	if(m_compositeMaterial)
	{
		m_compositeMaterial->Release();
	}
}

IRuObject *CRuCompositeModel::Clone(IRuObject *clonedObject)
{
	return NULL;
}

BOOL CRuCompositeModel::SerializeFrom(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuCompositeModel::SerializeTo(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuCompositeModel::Update(REAL elapsedTime)
{
	if(m_textureRebuildRequired && m_modelIsBuilt)
	{
		// Reset rebuild flag
		m_textureRebuildRequired = FALSE;

		// Re-initialize composite material textures
		InitializeTextures();

		// Rebuild model
		BuildModel();
	}

	// Chain update call to parent class
	return IRuEntity_Renderable::Update(elapsedTime);
}

BOOL CRuCompositeModel::QueryCollision(CRuCollisionQuery *colQuery)
{
	return IRuEntity_Renderable::QueryCollision(colQuery);
/*
	// If collision query is disabled, return immediately
	if(colQuery->m_ignoreCollisionFlag == FALSE && GetOption(ruENTITYOPTION_COLLISION) == 0)
		return TRUE;

	if(m_enablePartCompositing)
	{
		if(m_compositeMesh)
		{
			// Save previous collision transform
			CRuMatrix4x4 *originalWorldTransform = colQuery->m_worldTransform;
			CRuMatrix4x4 *originalInvWorldTransform = colQuery->m_invWorldTransform;

			// Store the inverse of the current world transform into the collision query
			colQuery->m_worldTransform = const_cast<CRuMatrix4x4 *>(&this->GetWorldTransform());
			colQuery->m_invWorldTransform = const_cast<CRuMatrix4x4 *>(&this->GetInverseWorldTransform());

			if(m_compositeMesh->QueryCollision(colQuery) == FALSE)
				return FALSE;

			// Restore original collision transform
			colQuery->m_worldTransform = originalWorldTransform;
			colQuery->m_invWorldTransform = originalInvWorldTransform;
		}
	}
	else
	{
		return m_compositeModel->QueryCollision(colQuery);
	}

	return TRUE;
*/
}

BOOL CRuCompositeModel::SetMaxMipLevel(INT32 maxMipLevel)
{
	if(m_maxMipLevel != maxMipLevel)
	{
		m_maxMipLevel = maxMipLevel;

		// Gotta move this into an option or something
		m_fullCompositionSize = 2048;			// This is the size of the original intended, completely composited texture size
		m_compositeTextureSize = 2048;			// This is the actual requested size of the composited diffuse texture size
		m_maskTextureSize = 1024;				// This is the actual requested size of the composited color mask texture size
		m_colorMapTextureSize = 32;				// This is the color map texture size
		m_glossTextureSize = 1024;				// This is the actual requested size of the composited gloss texture size

		// Adjust texture sizes based on specified max mip level
		for(INT32 i = 0; i < m_maxMipLevel; ++i)
		{
			m_compositeTextureSize = m_compositeTextureSize >> 1;
			m_maskTextureSize = m_maskTextureSize >> 1;
			m_glossTextureSize = m_glossTextureSize >> 1;
		}

		// Mark for a texture rebuild
		m_textureRebuildRequired = TRUE;
	}

	return TRUE;
}

BOOL CRuCompositeModel::EnablePartCompositing(BOOL enable)
{
	// Save new setting
	m_enablePartCompositing = enable;

	// Mark for a texture rebuild
	m_textureRebuildRequired = TRUE;

	return TRUE;
}

BOOL CRuCompositeModel::SetPart(INT32 partID, CRuEntity *partModel)
{
	// Setup the model parts array
	while(m_modelParts.Count() < m_modelTemplate->GetNumParts())
	{
		m_modelParts.Add(ModelPart());
	}

	// Search for part ID
	INT32 partIndex = m_modelTemplate->GetPartIndex(partID);

	if(partIndex >= 0)
	{
		if(partModel == NULL)
		{
			// Decrement reference count on the old model
			ruSAFE_RELEASE(m_modelParts[partIndex].m_model);
		}
		else
		{
			// Decrement reference count on the old model
			ruSAFE_RELEASE(m_modelParts[partIndex].m_model);

			// Store reference to part model
			partModel->AddRef();
			m_modelParts[partIndex].m_model = partModel;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuCompositeModel::SetPartColors(INT32 partID, const BOOL *layerActivated, const RUCOLOR *layerColor)
{
	// Setup the model parts array
	while(m_modelParts.Count() < m_modelTemplate->GetNumParts())
	{
		m_modelParts.Add(ModelPart());
	}

	// Search for part ID
	INT32 partIndex = m_modelTemplate->GetPartIndex(partID);

	if(partIndex >= 0)
	{
		for(INT32 i = 0; i < 4; ++i)
		{
			m_modelParts[partIndex].m_layerActivated[i] = layerActivated[i];
			m_modelParts[partIndex].m_layerColor[i] = layerColor[i] | 0XFF000000;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuCompositeModel::BuildModel()
{
	DWORD t0 = timeGetTime();

	if(m_textureRebuildRequired)
	{
		// Reset rebuild flag
		m_textureRebuildRequired = FALSE;

		// Re-initialize composite material textures
		InitializeTextures();
	}

	// Build the color map
	BuildColorMap();

	// Make sure the model parts array has been properly initialized
	if(m_modelParts.Count() == m_modelTemplate->GetNumParts())
	{
		// Release old composite mesh
		ruSAFE_RELEASE(m_compositeMesh);

		// Remove all children
		this->RemoveAllChildren();

		for(INT32 i = 0; i < m_modelParts.Count(); ++i)
		{
			if(m_modelParts[i].m_model)
			{
				// Set material properties
				CRuArrayList<IRuMaterial *> materials;
				RuEntity_EnumerateMaterials(m_modelParts[i].m_model, materials);

				for(INT32 j = 0; j < materials.Count(); ++j)
				{
					materials[j]->SetShader(g_ruShaderManager->GetShader("ruSHADER_PAPERDOLL"));
					materials[j]->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(0.1f));

					// Set gloss map to black texture if the part does not have one
					if(materials[j]->GetTexture(ruTEXCHANNEL_GLOSSMAP) == NULL)
					{
						materials[j]->SetTexture(ruTEXCHANNEL_GLOSSMAP, m_sys_BlackTexture);
					}
				}

				// Set model culling mode
				m_modelParts[i].m_model->SetOption(ruENTITYOPTION_CULLMODE, ruENTITYCULLMODE_ATTACHED, TRUE);

				// Add model to hierarchy
				this->AddChild(m_modelParts[i].m_model);
			}
		}
	}

	return FALSE;
}

BOOL CRuCompositeModel::ResetModel()
{
	// Reset model built flag
	m_modelIsBuilt = FALSE;

	// Mark texture rebuild as required
	m_textureRebuildRequired = TRUE;

	// Reset existing texture channels
	if(m_compositeMaterial)
	{
		m_compositeMaterial->SetTexture(ruTEXCHANNEL_DIFFUSEMAP, NULL);
		m_compositeMaterial->SetTexture(ruTEXCHANNEL_PAPERDOLL_MASK_COMPOSITE, NULL);
		m_compositeMaterial->SetTexture(ruTEXCHANNEL_PAPERDOLL_COLORMAP, NULL);
		m_compositeMaterial->SetTexture(ruTEXCHANNEL_GLOSSMAP, NULL);
	}

	// Release composite mesh and material
	ruSAFE_RELEASE(m_compositeMesh);
	ruSAFE_RELEASE(m_compositeMaterial);

	return TRUE;
}

CRuModel *CRuCompositeModel::ConvertToStandardModel()
{
	CRuModel *outputModel = ruNEW CRuModel();

	CRuMesh *clonedMesh = static_cast<CRuMesh *>(m_compositeMesh->Clone());
	CRuMaterial *clonedMaterial = static_cast<CRuMaterial *>(m_compositeMaterial->Clone());

	outputModel->AddSubModel(clonedMesh, clonedMaterial);
	outputModel->SetOption(ruENTITYOPTION_SKINNED, 1);

	return outputModel;
}

INT32 CRuCompositeModel::GetNumMeshes() const
{
	if(m_compositeMesh)
	{
		return 1;
	}

	return 0;
}

BOOL CRuCompositeModel::GetMesh(INT32 subModelIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(subModelIndex == 0 && m_compositeMesh && m_compositeMaterial)
	{
		*meshOut = m_compositeMesh;
		*materialOut = m_compositeMaterial;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuCompositeModel::InitializeTextures()
{
	// Construct the composite material if necessary
	if(m_compositeMaterial == NULL)
	{
		m_compositeMaterial = ruNEW CRuMaterial();
	}

	// Set basic composite material parameters
	m_compositeMaterial->SetSkinned(TRUE);
	m_compositeMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_PAPERDOLL"));
	m_compositeMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(0.1f));

	if(m_enablePartCompositing)
	{
		// Create diffuse texture
		CRuTexture_Proxy *texture = ruNEW CRuTexture_Proxy();
		texture->Allocate(m_compositeTextureSize, m_compositeTextureSize, 0, ruSURFACEFMT_DXT1);
		m_compositeMaterial->SetTexture(ruTEXCHANNEL_DIFFUSEMAP, texture);
		ruSAFE_RELEASE(texture);

		// Create color masking texture
		CRuTexture_Proxy *maskTexture = ruNEW CRuTexture_Proxy();
		maskTexture->Allocate(m_maskTextureSize, m_maskTextureSize, 0, ruSURFACEFMT_A4R4G4B4);
		m_compositeMaterial->SetTexture(ruTEXCHANNEL_PAPERDOLL_MASK_COMPOSITE, maskTexture);
		ruSAFE_RELEASE(maskTexture);

		// Create gloss map texture
		CRuTexture_Proxy *glossTexture = ruNEW CRuTexture_Proxy();
		glossTexture->Allocate(m_glossTextureSize, m_glossTextureSize, 0, ruSURFACEFMT_DXT1);
		m_compositeMaterial->SetTexture(ruTEXCHANNEL_GLOSSMAP, glossTexture);
		ruSAFE_RELEASE(glossTexture);

		// Create color map volume texture
		CRuVolumeTexture_Proxy *colorMapTexture = ruNEW CRuVolumeTexture_Proxy(m_colorMapTextureSize, m_colorMapTextureSize, 4, 1, ruSURFACEFMT_A8R8G8B8);
		m_compositeMaterial->SetTexture(ruTEXCHANNEL_PAPERDOLL_COLORMAP, colorMapTexture);
		ruSAFE_RELEASE(colorMapTexture);
	}
	else
	{
		m_compositeMaterial->SetTexture(ruTEXCHANNEL_DIFFUSEMAP, NULL);
		m_compositeMaterial->SetTexture(ruTEXCHANNEL_PAPERDOLL_MASK_COMPOSITE, NULL);
		m_compositeMaterial->SetTexture(ruTEXCHANNEL_GLOSSMAP, NULL);
		m_compositeMaterial->SetTexture(ruTEXCHANNEL_PAPERDOLL_COLORMAP, NULL);
	}

	return TRUE;
}

void CRuCompositeModel::BuildColorMap()
{
	for(INT32 i = 0; i < m_modelParts.Count(); ++i)
	{
		if(m_modelParts[i].m_model)
		{
			// Set material properties
			CRuArrayList<IRuMaterial *> materials;
			RuEntity_EnumerateMaterials(m_modelParts[i].m_model, materials);

			for(INT32 j = 0; j < materials.Count(); ++j)
			{
				IRuVolumeTexture *destColorMapTexture = (IRuVolumeTexture *) materials[j]->GetTexture(ruTEXCHANNEL_PAPERDOLL_COLORMAP);

				if(destColorMapTexture == NULL)
				{
					IRuBaseTexture *colorMapTexture = ruNEW CRuVolumeTexture_Proxy(1, 1, 4, 1, ruSURFACEFMT_A8R8G8B8);
					materials[j]->SetTexture(ruTEXCHANNEL_PAPERDOLL_COLORMAP, colorMapTexture);
					destColorMapTexture = (IRuVolumeTexture *) colorMapTexture;
					ruSAFE_RELEASE(colorMapTexture);
				}

				UINT32 *destBits;
				INT32 destRowPitch, destSlicePitch;

				// Lock destination and source
				destColorMapTexture->Lock(0, (void **) &destBits, &destRowPitch, &destSlicePitch);

				for(INT32 k = 0; k < 4; ++k)
				{
					if(m_modelParts[i].m_layerActivated[k])
					{
						RuImageFX_Clear_32(static_cast<UINT32 *>(destBits + k * (destSlicePitch / 4)), 0, 0, destColorMapTexture->GetLevelWidth(0), destColorMapTexture->GetLevelHeight(0), destRowPitch, m_modelParts[i].m_layerColor[k] | 0xFF000000);
					}
					else
					{
						RuImageFX_Clear_32(static_cast<UINT32 *>(destBits + k * (destSlicePitch / 4)), 0, 0, destColorMapTexture->GetLevelWidth(0), destColorMapTexture->GetLevelHeight(0), destRowPitch, 0x00000000);
					}
				}

				// Unlock destination and source
				destColorMapTexture->Unlock(0);
			}
		}
	}

//	D3DXSaveTextureToFile("D:\\output_colormap.dds", D3DXIFF_DDS, ((CRuVolumeTextureD3D *) destColorMapTexture)->GetIDirect3DVolumeTexture(), NULL);
}

// ************************************************************************************************************************************************************

#pragma managed
