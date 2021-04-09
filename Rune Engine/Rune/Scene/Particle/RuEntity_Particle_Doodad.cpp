/*!
	@file RuEntity_Particle_Doodad.cpp

	Copyright (c) 2004-2006 All rights reserved
*/

#include "../../Scene/Particle/RuEntity_Particle_Doodad.h"

#include "../../Helper/RuHelper_Entity.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuDoodad_Template, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuDoodad_Template, "CRuDoodad_Template", "IRuObject")
ruCLASSGUID_DEFINE(CRuDoodad_Template, 0x4364739B, 0xC48347a8, 0x97576289, 0xB3C38299)

ruRTTI_DEFINE_SUBCLASS(CRuDoodad_Cluster, IRuEntity_Container)
ruHEAP_DEFINE_SUBCLASS(CRuDoodad_Cluster, "CRuDoodad_Cluster", "IRuEntity_Container")
ruCLASSGUID_DEFINE(CRuDoodad_Cluster, 0x7A408F32, 0xF95B46da, 0x8592E86E, 0x4F182ABE)

ruRTTI_DEFINE_SUBCLASS(CRuDoodad_ClusterManager, CRuEntity_Container_Impl)
ruHEAP_DEFINE_SUBCLASS(CRuDoodad_ClusterManager, "CRuDoodad_ClusterManager", "CRuEntity_Container_Impl")
ruCLASSGUID_DEFINE(CRuDoodad_ClusterManager, 0x09501908, 0x5DEB4d61, 0x9F67ADC2, 0xF655B772)

// ************************************************************************************************************************************************************

CRuDoodad_Template::CRuDoodad_Template()
:	m_doodadName(NULL),
	m_modelName(NULL),
	m_material(NULL),
	m_textureName(NULL),
	m_uSubdivision(1),
	m_vSubdivision(1),
	m_mesh(NULL),
	m_texture(NULL)
{
}

CRuDoodad_Template::~CRuDoodad_Template()
{
	delete[] m_doodadName;
	delete[] m_modelName;
	delete[] m_textureName;
	ruSAFE_RELEASE(m_mesh);
	ruSAFE_RELEASE(m_material);
	ruSAFE_RELEASE(m_texture);
}

IRuObject *CRuDoodad_Template::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuDoodad_Template();
	CRuDoodad_Template *clonedTemplate = static_cast<CRuDoodad_Template *>(clonedObject);

	// Chain cloning call to parent class
	IRuObject::Clone(clonedObject);

	clonedTemplate->m_guid = this->GetGUID();
	clonedTemplate->SetDoodadName(this->GetDoodadName());
	clonedTemplate->SetModelName(this->GetModelName());
	clonedTemplate->SetTextureName(this->GetTextureName());
	clonedTemplate->SetUSubdivision(this->GetUSubdivision());
	clonedTemplate->SetVSubdivision(this->GetVSubdivision());

	return clonedTemplate;
}

BOOL CRuDoodad_Template::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeFrom(stream);

	// Read entity guid
	RuReadGUID(stream, m_guid);

	// Read length of doodad name including NULL
	INT32 doodadNameLength;
	stream->Read(&doodadNameLength, sizeof(INT32));

	// Read doodad name including NULL
	m_doodadName = ruNEW char [doodadNameLength];
	stream->Read(m_doodadName, sizeof(char) * doodadNameLength);

	// Read length of model name including NULL
	INT32 modelNameLength;
	stream->Read(&modelNameLength, sizeof(INT32));

	// Read model name including NULL
	m_modelName = ruNEW char [modelNameLength];
	stream->Read(m_modelName, sizeof(char) * modelNameLength);

	// Read length of texture name including NULL
	INT32 textureNameLength;
	stream->Read(&textureNameLength, sizeof(INT32));

	// Read texture name including NULL
	m_textureName = ruNEW char [textureNameLength];
	stream->Read(m_textureName, sizeof(char) * textureNameLength);

	// Read u subdivision
	stream->Read(&m_uSubdivision, sizeof(INT32));

	// Read v subdivision
	stream->Read(&m_vSubdivision, sizeof(INT32));

	return TRUE;
}

BOOL CRuDoodad_Template::SerializeTo(IRuStream *stream)
{
	INT32 version = 1;

	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeTo(stream);

	// Write entity GUID
	stream->Write(m_guid.Data(), m_guid.Size());

	// Write length of doodad name including NULL
	INT32 doodadNameLength = m_doodadName ? (INT32)strlen(m_doodadName) + 1 : 1;
	stream->Write(&doodadNameLength, sizeof(INT32));

	// Write doodad name including NULL
	m_doodadName ? stream->Write(m_doodadName, sizeof(char) * doodadNameLength) : stream->Write("", 1);

	// Write length of model name including NULL
	INT32 modelNameLength = m_modelName ? (INT32)strlen(m_modelName) + 1 : 1;
	stream->Write(&modelNameLength, sizeof(INT32));

	// Write model name including NULL
	m_modelName ? stream->Write(m_modelName, sizeof(char) * modelNameLength) : stream->Write("", 1);

	// Write length of texture name including NULL
	INT32 textureNameLength = m_textureName ? (INT32)strlen(m_textureName) + 1 : 1;
	stream->Write(&textureNameLength, sizeof(INT32));

	// Write texture name including NULL
	m_textureName ? stream->Write(m_textureName, sizeof(char) * textureNameLength) : stream->Write("", 1);

	// Write u subdivision
	stream->Write(&m_uSubdivision, sizeof(INT32));

	// Write v subdivision
	stream->Write(&m_vSubdivision, sizeof(INT32));

	return TRUE;
}

const char *CRuDoodad_Template::GetDoodadName() const
{
	if(m_doodadName)
		return m_doodadName;

	return "";
}

void CRuDoodad_Template::SetDoodadName(const char *doodadName)
{
	delete[] m_doodadName;
	m_doodadName = ruNEW char [strlen(doodadName) + 1];
	strcpy(m_doodadName, doodadName);
}

const char *CRuDoodad_Template::GetModelName() const
{
	if(m_modelName)
		return m_modelName;

	return "";
}

void CRuDoodad_Template::SetModelName(const char *modelName)
{
	delete[] m_modelName;
	m_modelName = ruNEW char [strlen(modelName) + 1];
	strcpy(m_modelName, modelName);

	ReloadModel();
}

const char *CRuDoodad_Template::GetTextureName()
{
	if(m_textureName)
		return m_textureName;

	return "";
}

void CRuDoodad_Template::SetTextureName(const char *textureName)
{
	delete[] m_textureName;
	m_textureName = ruNEW char [strlen(textureName) + 1];
	strcpy(m_textureName, textureName);

	ReloadTexture();
}

INT32 CRuDoodad_Template::GetUSubdivision() const
{
	return m_uSubdivision;
}

void CRuDoodad_Template::SetUSubdivision(INT32 uSubdivision)
{
	m_uSubdivision = uSubdivision;
}

INT32 CRuDoodad_Template::GetVSubdivision() const
{
	return m_vSubdivision;
}

void CRuDoodad_Template::SetVSubdivision(INT32 vSubdivision)
{
	m_vSubdivision = vSubdivision;
}

IRuMesh *CRuDoodad_Template::GetMesh()
{
	if(m_mesh == NULL)
	{
		ReloadModel();
	}

	return m_mesh;
}

IRuMaterial *CRuDoodad_Template::GetMaterial()
{
	if(m_material == NULL)
	{
		ReloadModel();
	}

	return m_material;
}

IRuTexture *CRuDoodad_Template::GetTexture()
{
	if(m_texture == NULL)
	{
		ReloadTexture();
	}

	return m_texture;
}

void CRuDoodad_Template::ReloadModel()
{
	ruSAFE_RELEASE(m_mesh);
	ruSAFE_RELEASE(m_material);

	if(m_modelName)
	{
		CRuEntity *entity = g_ruResourceManager->LoadEntity(m_modelName);

		if(entity)
		{
			CRuModel *model = RuEntity_FindFirstModel(entity);

			if(model)
			{
				if(model->GetNumMeshes() >= 1)
				{
					model->GetMesh(0, &m_mesh, &m_material);

					m_mesh->AddRef();
					m_material->AddRef();
				}
			}

			ruSAFE_RELEASE(entity);
		}
	}
}

void CRuDoodad_Template::ReloadTexture()
{
	ruSAFE_RELEASE(m_texture);

	if(m_textureName)
	{
		m_texture = g_ruResourceManager->LoadTexture(m_textureName, ruSURFACEFMT_DEFAULT, 0, FALSE);
	}
}

// ************************************************************************************************************************************************************

CRuDoodad_Cluster::CRuDoodad_Cluster()
:	m_maxDoodadsPerBatch(70),
	m_periodInterval(1.0f),
	m_t(0.0f),
	m_doodadTemplate(NULL),
	m_batchesDirty(TRUE),
	m_mesh(NULL)
{
	// Ensure that the cluster object is not included in bounding volume
	this->SetOption(ruENTITYOPTION_EXCLUDEBOUNDS, TRUE);
}

CRuDoodad_Cluster::~CRuDoodad_Cluster()
{
	// Release doodad template
	ruSAFE_RELEASE(m_doodadTemplate);

	// Delete all doodad list descriptors in the active list
	for(INT32 i = 0; i < m_doodadLists.Count(); ++i)
	{
		delete m_doodadLists[i].m_doodadList;
	}

	// Delete all doodad list descriptors in the free stack
	while(m_freeDoodadList.IsEmpty() == FALSE)
	{
		delete m_freeDoodadList.Pop().m_doodadList;
	}

	// Clear all batches
	ClearBatches();

	// Release resources used by batch structures
	while(m_freeBatchDescStack.IsEmpty() == FALSE)
	{
		delete m_freeBatchDescStack.Pop();
	}

	while(m_freeModels.IsEmpty() == FALSE)
	{
		CRuModel *model = m_freeModels.Pop();
		ruSAFE_RELEASE(model);
	}

	// Release mesh
	ruSAFE_RELEASE(m_mesh);
}

BOOL CRuDoodad_Cluster::Update(REAL elapsedTime)
{
	// Create batches if necessary
	if(m_batchesDirty)
	{
		CreateBatches();
	}

	// Update t
	m_t += (elapsedTime / m_periodInterval) * ruPI;

	// Cycle t in [0, 2PI]
	if(m_t > ruPI * 2.0f)
	{
		m_t = fmod(m_t, ruPI * 2.0f);
	}

	// Update t in all batches
	for(INT32 i = 0; i < m_batches.Count(); ++i)
	{
		m_batches[i]->m_vsConstants[2] = m_t;
	}

	return IRuEntity_Container::Update(elapsedTime);
}
/*
INT32 CRuDoodad_Cluster::GetNumMeshes() const
{
	return m_batches.Count();
}

BOOL CRuDoodad_Cluster::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex >= 0 && meshIndex < m_batches.Count())
	{
		*meshOut = m_batches[meshIndex]->m_mesh;
		*materialOut = m_batches[meshIndex]->m_material;

		return TRUE;
	}

	return FALSE;
}
*/
CRuDoodad_Template *CRuDoodad_Cluster::GetDoodadTemplate()
{
	return m_doodadTemplate;
}

void CRuDoodad_Cluster::SetDoodadTemplate(CRuDoodad_Template *doodadTemplate)
{
	if(doodadTemplate)
	{
		// Store reference to the template
		doodadTemplate->AddRef();
		ruSAFE_RELEASE(m_doodadTemplate);
		m_doodadTemplate = doodadTemplate;

		// Replicate mesh data from the template into the rendering mesh
		IRuMesh *srcMesh = m_doodadTemplate->GetMesh();
		IRuMaterial *srcMaterial = m_doodadTemplate->GetMaterial();
		if(srcMesh && srcMaterial)
		{
			INT32 numVertices = srcMesh->GetNumVertices();
			INT32 numIndices = srcMesh->GetNumIndices();
			INT32 numPrimitives = srcMesh->GetNumPrimitives();
			DWORD vertexFormat = srcMesh->GetVertexFormat();

			// Construct mesh
			ruSAFE_RELEASE(m_mesh);
			m_mesh = ruNEW CRuMesh();
			m_mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, numVertices * m_maxDoodadsPerBatch, numPrimitives * m_maxDoodadsPerBatch, vertexFormat | ruVERTEXFORMAT_DIFFUSE, 1);

			for(INT32 i = 0, vOffset = 0; i < m_maxDoodadsPerBatch; ++i, vOffset += numVertices)
			{
				// Copy vertex data from source mesh
				if(vertexFormat & ruVERTEXFORMAT_POSITION)
				{
					const CRuVector3 *srcPosition = srcMesh->GetMorphTarget(0)->GetPosition();
					CRuVector3 *dstPosition = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();

					for(INT32 j = 0, k = vOffset; j < numVertices; ++j, ++k)
					{
						dstPosition[k] = srcPosition[j];
					}
				}

				if(vertexFormat & ruVERTEXFORMAT_NORMAL)
				{
					const CRuVector3 *srcNormal = srcMesh->GetMorphTarget(0)->GetNormal();
					CRuVector3 *dstNormal = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetNormal();

					for(INT32 j = 0, k = vOffset; j < numVertices; ++j, ++k)
					{
						dstNormal[k] = srcNormal[j];
					}
				}

				if(vertexFormat & ruVERTEXFORMAT_DIFFUSE)
				{
					const DWORD *srcDiffuse = srcMesh->GetMorphTarget(0)->GetDiffuse();
					DWORD *dstDiffuse = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetDiffuse();

					for(INT32 j = 0, k = vOffset; j < numVertices; ++j, ++k)
					{
						dstDiffuse[k] = (srcDiffuse[j] & 0x00FFFFFF) | ((i << 24) & 0xFF000000);
					}
				}
				else
				{
					DWORD *dstDiffuse = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetDiffuse();

					for(INT32 j = 0, k = vOffset; j < numVertices; ++j, ++k)
					{
						dstDiffuse[k] = (0x00FFFFFF) | ((i << 24) & 0xFF000000);
					}
				}

				// Copy texture coordinate data from source mesh
				for(INT32 j = 0; j < srcMaterial->GetNumTextureChannels(); ++j)
				{
					RuTextureChannel texChan = srcMaterial->GetTextureChannel(j);
					INT32 numComponents = RuTextureChannel_GetNumComponents(texChan);

					m_mesh->AddTextureCoordinate(texChan);

					const REAL *srcUVW = srcMesh->GetMorphTarget(0)->GetTextureCoordinate(texChan);
					REAL *dstUVW = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetTextureCoordinate(texChan);

					memcpy(&dstUVW[vOffset * numComponents], srcUVW, sizeof(REAL) * numComponents * numVertices);
				}
			}

			// Replicate index data n times from source mesh
			const UINT16 *srcIndices = srcMesh->GetIndices();
			UINT16 *dstIndices = m_mesh->GetIndices();
			for(INT32 i = 0, k = 0, vOffset = 0; i < m_maxDoodadsPerBatch; ++i, vOffset += numVertices)
			{
				for(INT32 j = 0; j < numIndices; ++j, ++k)
				{
					dstIndices[k] = srcIndices[j] + vOffset;
				}
			}
		}
	}
}

void CRuDoodad_Cluster::InsertDoodadList(INT32 key, const CRuArrayList<CRuDoodad_Descriptor> &doodadList)
{
	if(m_freeDoodadList.IsEmpty())
	{
		// Create doodad list descriptor
		DoodadList dl;
		dl.m_key = key;
		dl.m_doodadList = ruNEW CRuArrayList<CRuDoodad_Descriptor>(doodadList);

		// Add new doodad list to the master list
		m_doodadLists.Add(dl);
	}
	else
	{
		DoodadList dl = m_freeDoodadList.Pop();
		dl.m_key = key;
		dl.m_doodadList->CloneFrom(doodadList);

		// Add new doodad list to the master list
		m_doodadLists.Add(dl);
	}

	m_batchesDirty = TRUE;
}

void CRuDoodad_Cluster::RemoveDoodadLists(INT32 key)
{
	for(INT32 i = 0; i < m_doodadLists.Count(); ++i)
	{
		if(m_doodadLists[i].m_key == key)
		{
			// Push doodad list descriptor onto free stack and remove it from active list
			m_freeDoodadList.Push(m_doodadLists[i]);
			m_doodadLists.RemoveAt(i);

			// Decrement i so we continue at the next doodad list descriptor
			--i;
		}
	}

	m_batchesDirty = TRUE;
}

void CRuDoodad_Cluster::RemoveAllDoodadLists()
{
	for(INT32 i = 0; i < m_doodadLists.Count(); ++i)
	{
		// Push doodad list descriptor onto free stack
		m_freeDoodadList.Push(m_doodadLists[i]);
	}

	// Clear active list
	m_doodadLists.Clear();

	m_batchesDirty = TRUE;
}

void CRuDoodad_Cluster::CreateBatches()
{
	// Declare minimal sphere builder
	CRuMinSphereBuilder minSphereBuilder;

	// Clear existing batches
	ClearBatches();

	IRuMesh *mesh = m_doodadTemplate->GetMesh();
	IRuTexture *texture = m_doodadTemplate->GetTexture();
	INT32 textureWidth = 1;
	INT32 textureHeight = 1;

	if(mesh == NULL || m_mesh == NULL)
	{
		return;
	}

	if(texture)
	{
		textureWidth = texture->GetLevelWidth(0);
		textureHeight = texture->GetLevelHeight(0);
	}

	INT32 facesPerDoodad = mesh->GetNumPrimitives();

	BatchDesc *batch = NULL;
	INT32 doodadsThisBatch = 0;

	CRuSphere localBounds;

	CRuArrayList<CRuVector3> doodadPos;

	// Iterate through all doodad lists
	for(INT32 i = 0, k = 24; i < m_doodadLists.Count(); ++i)
	{
		CRuArrayList<CRuDoodad_Descriptor> &doodadList = *m_doodadLists[i].m_doodadList;

		CRuArrayList<INT32> batchIndices;

		// Iterate through all doodads
		for(INT32 j = 0; j < doodadList.Count(); ++j)
		{
			// Store reference to the doodad
			const CRuDoodad_Descriptor &doodad = doodadList[j];

			// Detect if there is a need for a new batch
			if(doodadsThisBatch >= m_maxDoodadsPerBatch || batch == NULL)
			{
				// Set number of particles for the previous batch if applicable
				if(batch)
				{
					minSphereBuilder.BuildSphere(doodadPos.GetArray(), doodadPos.Count(), localBounds);

					batch->m_mesh->SetProxiedMesh(m_mesh, m_mesh->AddBatch(0, doodadsThisBatch * facesPerDoodad));
					batch->m_mesh->SetBounds(localBounds);
					batch->m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, doodadsThisBatch);
				}

				// Get a free batch descriptor
				if(m_freeBatchDescStack.IsEmpty())
				{
					batch = ruNEW BatchDesc(m_maxDoodadsPerBatch);
				}
				else
				{
					batch = m_freeBatchDescStack.Pop();
				}

				// Select texture
				batch->m_material->SetTexture(ruTEXCHANNEL_DIFFUSEMAP, texture);

				// Setup header constants
				batch->m_vsConstants[0] = (REAL) 1.0f / m_doodadTemplate->GetUSubdivision();
				batch->m_vsConstants[1] = (REAL) 1.0f / m_doodadTemplate->GetVSubdivision();
				batch->m_vsConstants[2] = m_t;
				batch->m_vsConstants[3] = 0.0f;

				batch->m_vsConstants[4] = 256.0f;						// diffuse color scale factor - so we can get a proper integer index
				batch->m_vsConstants[5] = 3.0f;							// 3 constant rows per instance dataset
				batch->m_vsConstants[6] = 33.0f;						// Instance dataset begins at c33
				batch->m_vsConstants[7] = 1.0f / 2400.0f;				// reciprocal fade distance

				batch->m_vsConstants[8] = 0.0f;							// normal x
				batch->m_vsConstants[9] = 1.0f;							// normal y
				batch->m_vsConstants[10] = 0.0f;						// normal z
				batch->m_vsConstants[11] = 0.0f;						// unused

				batch->m_vsConstants[12] =  3.14159265f;				// sin/cos range
				batch->m_vsConstants[13] =  0.50000000f;
				batch->m_vsConstants[14] =  6.28318531f;
				batch->m_vsConstants[15] =  0.15915494f;

				batch->m_vsConstants[16] =  1.00000000f;
				batch->m_vsConstants[17] = -0.50000000f;
				batch->m_vsConstants[18] =  0.04166667f;
				batch->m_vsConstants[19] = -0.00138889f;

				batch->m_vsConstants[20] =  1.00000000f;
				batch->m_vsConstants[21] = -0.16666667f;
				batch->m_vsConstants[22] =  0.00833333f;
				batch->m_vsConstants[23] = -0.00019841f;

				// Reset constant index
				doodadsThisBatch = 0;
				k = 24;

				// Add batch to batch list
				m_batches.Add(batch);

				// Add batch index to list
				batchIndices.Add(m_batches.Count() - 1);

				doodadPos.Clear();
			}

			INT32 subTexPerRow = max(1, m_doodadTemplate->GetUSubdivision());

			// Setup constant data for this particle
			batch->m_vsConstants[k++] = doodad.m_translation.x;												// position x
			batch->m_vsConstants[k++] = doodad.m_translation.y;												// position y
			batch->m_vsConstants[k++] = doodad.m_translation.z;												// position z
			batch->m_vsConstants[k++] = doodad.m_scale;														// scale

			batch->m_vsConstants[k++] = doodad.m_rotation.x;												// rotation x
			batch->m_vsConstants[k++] = doodad.m_rotation.y;												// rotation y
			batch->m_vsConstants[k++] = doodad.m_rotation.z;												// rotation z
			batch->m_vsConstants[k++] = doodad.m_rotation.w;												// rotation w

			batch->m_vsConstants[k++] = static_cast<REAL>(doodad.m_textureIndex % subTexPerRow);			// u index
			batch->m_vsConstants[k++] = static_cast<REAL>(doodad.m_textureIndex / subTexPerRow);			// v index
			batch->m_vsConstants[k++] = (RuGlobalMTRand().GenerateFloat() - 0.5f) * doodad.m_swayMagnitude;			// x max displacement
			batch->m_vsConstants[k++] = (RuGlobalMTRand().GenerateFloat() - 0.5f) * doodad.m_swayMagnitude;			// z max displacement

			doodadPos.Add(doodad.m_translation);

			++doodadsThisBatch;
		}

		// Set number of doodads for the previous batch if applicable
		if(batch)
		{
			minSphereBuilder.BuildSphere(doodadPos.GetArray(), doodadPos.Count(), localBounds);

			batch->m_mesh->SetProxiedMesh(m_mesh, m_mesh->AddBatch(0, doodadsThisBatch * facesPerDoodad));
			batch->m_mesh->SetBounds(localBounds);
			batch->m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, doodadsThisBatch);

			batch = NULL;
		}

		CRuModel *model = m_freeModels.IsEmpty() ? ruNEW CRuModel() : m_freeModels.Pop();

		for(INT32 m = 0; m < batchIndices.Count(); ++m)
		{
			model->AddSubModel(m_batches[batchIndices[m]]->m_mesh, m_batches[batchIndices[m]]->m_material);
		}

		// Add to list of models
		m_models.Add(model);

		if(model->GetNumMeshes() > 0)
		{
			// Re-calculate entity bounds
			model->MarkLocalEntityBoundsDirty();

			// Add to entity tree
			this->AddChild(model);
		}
	}

	// Reset batch dirty flag
	m_batchesDirty = FALSE;
}

void CRuDoodad_Cluster::ClearBatches()
{
	// Push all used batches onto stack
	for(INT32 i = 0; i < m_batches.Count(); ++i)
	{
		m_freeBatchDescStack.Push(m_batches[i]);
	}

	// Clear batches
	m_batches.Clear();

	// Clear batches
	if(m_mesh)
	{
		m_mesh->ClearBatches();
	}

	// Remove all children
	this->RemoveAllChildren();

	// 
	for(INT32 i = 0; i < m_models.Count(); ++i)
	{
		m_models[i]->RemoveAllSubModels();
		m_freeModels.Push(m_models[i]);
	}

	m_models.Clear();
}

// ************************************************************************************************************************************************************

CRuDoodad_ClusterManager::CRuDoodad_ClusterManager()
{
	// Ensure that the cluster manager is not included in bounding volume
	this->SetOption(ruENTITYOPTION_EXCLUDEBOUNDS, TRUE);
}

CRuDoodad_ClusterManager::~CRuDoodad_ClusterManager()
{
	// Release all cluster objects
	for(INT32 i = 0; i < m_clusters.Count(); ++i)
	{
		ruSAFE_RELEASE(m_clusters[i].m_cluster);
	}
}

BOOL CRuDoodad_ClusterManager::Update(REAL elapsedTime)
{
	return CRuEntity_Container_Impl::Update(elapsedTime);
}

void CRuDoodad_ClusterManager::SetDoodadTemplate(const CRuGUID &clusterGUID, CRuDoodad_Template *doodadTemplate)
{
	for(INT32 i = 0; i < m_clusters.Count(); ++i)
	{
		if(m_clusters[i].m_clusterGUID == clusterGUID)
		{
			if(m_clusters[i].m_cluster)
			{
				m_clusters[i].m_cluster->SetDoodadTemplate(doodadTemplate);
			}

			return;
		}
	}

	// No cluster with matching GUID exists, create a new descriptor for it
	ClusterDesc clusterDesc;
	clusterDesc.m_clusterGUID = clusterGUID;
	clusterDesc.m_cluster = ruNEW CRuDoodad_Cluster();
	clusterDesc.m_cluster->SetDoodadTemplate(doodadTemplate);

	m_clusters.Add(clusterDesc);
	this->AddEntity(clusterDesc.m_cluster);
}

void CRuDoodad_ClusterManager::InsertDoodadList(INT32 key, const CRuGUID &clusterGUID, const CRuArrayList<CRuDoodad_Descriptor> &doodadList)
{
	for(INT32 i = 0; i < m_clusters.Count(); ++i)
	{
		if(m_clusters[i].m_clusterGUID == clusterGUID)
		{
			if(m_clusters[i].m_cluster)
			{
				m_clusters[i].m_cluster->InsertDoodadList(key, doodadList);
			}

			return;
		}
	}

	// No cluster with matching GUID exists, create a new descriptor for it
	ClusterDesc clusterDesc;
	clusterDesc.m_clusterGUID = clusterGUID;
	clusterDesc.m_cluster = ruNEW CRuDoodad_Cluster();
	clusterDesc.m_cluster->InsertDoodadList(key, doodadList);

	m_clusters.Add(clusterDesc);
	this->AddEntity(clusterDesc.m_cluster);
}

void CRuDoodad_ClusterManager::RemoveDoodadLists(INT32 key)
{
	for(INT32 i = 0; i < m_clusters.Count(); ++i)
	{
		if(m_clusters[i].m_cluster)
		{
			m_clusters[i].m_cluster->RemoveDoodadLists(key);
		}
	}
}

// ************************************************************************************************************************************************************

#pragma managed
