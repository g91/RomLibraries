#include "../../Scene/Base/RuEntityModel.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuModel, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuModel, "CRuModel", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuModel, 0xC786345E, 0x95164d99, 0xA210080D, 0x59C9AB48)

// ************************************************************************************************************************************************************

CRuModel::CRuModel()
{
}

CRuModel::~CRuModel()
{
	for(INT32 i = 0; i < m_subModels.Count(); ++i)
	{
		m_subModels[i].m_mesh->Release();
		m_subModels[i].m_material->Release();
	}
}

IRuObject *CRuModel::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuModel();

	// Chain cloning call to parent class
	IRuEntity_Renderable::Clone(clonedObject);

	// Shortcut reference to the cloned entity
	CRuModel *clonedModel = static_cast<CRuModel *>(clonedObject);

	// Iterate through all submodels of the original and add them to the cloned model
	for(INT32 i = 0; i < m_subModels.Count(); ++i)
	{
		SubModel clonedSubModel;

		// Clone the submodel
		clonedSubModel.m_mesh = m_subModels[i].m_mesh;
		clonedSubModel.m_mesh->AddRef();
		clonedSubModel.m_material = static_cast<IRuMaterial *>(m_subModels[i].m_material->Clone());

		clonedModel->m_subModels.Add(clonedSubModel);
	}

	return clonedModel;
}

BOOL CRuModel::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Release old submeshes
	for(INT32 i = 0; i < m_subModels.Count(); ++i)
	{
		m_subModels[i].m_mesh->Release();
		m_subModels[i].m_material->Release();
	}
	m_subModels.Clear();

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
				// Enable collision
				SetOption(ruENTITYOPTION_COLLISION, 1);

				// Read number of submeshes
				INT32 subMeshCount;
				stream->Read(&subMeshCount, sizeof(INT32));

				for(INT32 i = 0; i < subMeshCount; ++i)
				{
					IRuObject *meshObject = RuObjectFactory().CreateObject(stream);
					IRuObject *materialObject = RuObjectFactory().CreateObject(stream);

					if(meshObject && materialObject)
					{
						if(meshObject->GetType().IsSubClassOf(IRuMesh::Type()) && materialObject->GetType().IsSubClassOf(IRuMaterial::Type()))
						{
							// Insert mesh+material into model
							AddSubModel((IRuMesh *) meshObject, (IRuMaterial *) materialObject);

							// Verify whether the mesh has skinning weights and set the skinning option accordingly
							if(static_cast<IRuMesh *>(meshObject)->GetMorphTarget(0)->GetBlendWeight())
							{
//								SetOption(ruENTITYOPTION_SKINNED, 1);
							}
						}
					}

					// Release local copies
					if(meshObject)
					{
						meshObject->Release();
					}

					if(materialObject)
					{
						materialObject->Release();
					}
				}
			}

			break;

		case 2:
			{
				// Read options
				stream->Read(&m_optionFlags0, sizeof(UINT32));

				// Read number of submeshes
				INT32 subMeshCount;
				stream->Read(&subMeshCount, sizeof(INT32));

				for(INT32 i = 0; i < subMeshCount; ++i)
				{
					IRuObject *meshObject = RuObjectFactory().CreateObject(stream);
					IRuObject *materialObject = RuObjectFactory().CreateObject(stream);

					if(meshObject && materialObject)
					{
						if(meshObject->GetType().IsSubClassOf(IRuMesh::Type()) && materialObject->GetType().IsSubClassOf(IRuMaterial::Type()))
						{
							// Insert mesh+material into model
							AddSubModel((IRuMesh *) meshObject, (IRuMaterial *) materialObject);
						}
					}

					// Release local copies
					if(meshObject)
					{
						meshObject->Release();
					}

					if(materialObject)
					{
						materialObject->Release();
					}
				}
			}

			break;

		case 3:
			{
				// Chain serialization to base class
				CRuEntity::SerializeFrom(stream);

				// Read number of submeshes
				INT32 subMeshCount;
				stream->Read(&subMeshCount, sizeof(INT32));

				for(INT32 i = 0; i < subMeshCount; ++i)
				{
					IRuObject *meshObject = RuObjectFactory().CreateObject(stream);
					IRuObject *materialObject = RuObjectFactory().CreateObject(stream);

					if(meshObject && materialObject)
					{
						if(meshObject->GetType().IsSubClassOf(IRuMesh::Type()) && materialObject->GetType().IsSubClassOf(IRuMaterial::Type()))
						{
							// Insert mesh+material into model
							AddSubModel((IRuMesh *) meshObject, (IRuMaterial *) materialObject);
						}
					}

					// Release local copies
					if(meshObject)
					{
						meshObject->Release();
					}

					if(materialObject)
					{
						materialObject->Release();
					}
				}
			}

			break;
	}

	return TRUE;
}

BOOL CRuModel::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write signature
	INT32 signature = 0xFFFFFFFF;
	stream->Write(&signature, sizeof(INT32));

	// Write version
	INT32 version = 3;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuEntity::SerializeTo(stream);

	// Write number of sub models
	INT32 subModelCount = m_subModels.Count();
	stream->Write(&subModelCount, sizeof(INT32));

	// Serialize each individual sub model
	for(INT32 i = 0; i < m_subModels.Count(); ++i)
	{
		// Serialize the mesh
		m_subModels[i].m_mesh->SerializeTo(stream);

		// Serialize the material
		m_subModels[i].m_material->SerializeTo(stream);
	}

	return TRUE;
}

INT32 CRuModel::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuEntity_Renderable::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	footprint += m_subModels.GetMemoryFootprint();
	for(INT32 i = 0; i < m_subModels.Count(); ++i)
	{
		footprint += m_subModels[i].m_mesh->GetMemoryFootprint(enumHash);
		footprint += m_subModels[i].m_material->GetMemoryFootprint(enumHash);
	}

	return footprint;
}

BOOL CRuModel::Update(REAL elapsedTime)
{
	// Chain update call to parent class
	return IRuEntity_Renderable::Update(elapsedTime);
}

BOOL CRuModel::QueryCollision(CRuCollisionQuery *colQuery)
{
	// If collision query is disabled, return immediately
	if(colQuery->m_ignoreCollisionFlag == FALSE && GetOption(ruENTITYOPTION_COLLISION) == 0)
		return TRUE;

	// Save previous collision transform
	CRuMatrix4x4 *originalWorldTransform = colQuery->m_worldTransform;
	CRuMatrix4x4 *originalInvWorldTransform = colQuery->m_invWorldTransform;

	// Store the inverse of the current world transform into the collision query
	colQuery->m_worldTransform = const_cast<CRuMatrix4x4 *>(&this->GetWorldTransform());
	colQuery->m_invWorldTransform = const_cast<CRuMatrix4x4 *>(&this->GetInverseWorldTransform());

	if(m_collisionObject)
	{
		// Query attached collision object
		m_collisionObject->QueryCollision(colQuery);
	}
	else
	{
		// Query collision on all sub models
		for(INT32 i = 0; i < m_subModels.Count(); ++i)
		{
			if(m_subModels[i].m_mesh->QueryCollision(colQuery) == FALSE)
				return FALSE;
		}
	}

	// Restore original collision transform
	colQuery->m_worldTransform = originalWorldTransform;
	colQuery->m_invWorldTransform = originalInvWorldTransform;

	return TRUE;
}

BOOL CRuModel::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex < m_subModels.Count())
	{
		// Store pointer to the submodel mesh & material into parameters
		*meshOut = m_subModels[meshIndex].m_mesh;
		*materialOut = m_subModels[meshIndex].m_material;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuModel::SetMesh(INT32 meshIndex, IRuMesh *mesh)
{
	if(meshIndex < m_subModels.Count())
	{
		// Store pointer to the old mesh
		IRuMesh *oldMesh = m_subModels[meshIndex].m_mesh;

		// Store the new mesh and increment its ref count
		m_subModels[meshIndex].m_mesh = mesh;
		m_subModels[meshIndex].m_mesh->AddRef();

		// Decrement reference count for the old mesh
		oldMesh->Release();

		return TRUE;
	}

	return FALSE;
}

INT32 CRuModel::GetMeshIndex(IRuMesh *mesh)
{
	for(INT32 i = 0; i < m_subModels.Count(); ++i)
	{
		if(m_subModels[i].m_mesh == mesh)
		{
			return i;
		}
	}

	return -1;
}

BOOL CRuModel::AddSubModel(IRuMesh *mesh, IRuMaterial *material)
{
	SubModel subModel;

	// Setup submodel structure
	subModel.m_meshIterationNumber = mesh->GetMeshIteration();
	subModel.m_mesh = mesh;
	subModel.m_material = material;

	// Increment reference counts on mesh and material
	subModel.m_mesh->AddRef();
	subModel.m_material->AddRef();

	// Add submodel to list of submodels
	m_subModels.Add(subModel);

	// Mark local entity bounds as dirty
	this->MarkLocalEntityBoundsDirty();

	return TRUE;
}

BOOL CRuModel::RemoveSubModel(INT32 meshIndex)
{
	if(meshIndex < m_subModels.Count())
	{
		// Release submodel resources
		m_subModels[meshIndex].m_mesh->Release();
		m_subModels[meshIndex].m_material->Release();

		// Remove submodel from list
		m_subModels.RemoveAt(meshIndex);

		// Mark local entity bounds as dirty
		this->MarkLocalEntityBoundsDirty();

		return TRUE;
	}

	return FALSE;
}

BOOL CRuModel::RemoveAllSubModels()
{
	// Release submodel resources
	for(INT32 i = 0; i < m_subModels.Count(); ++i)
	{
		m_subModels[i].m_mesh->Release();
		m_subModels[i].m_material->Release();
	}

	m_subModels.Clear();

	// Mark local entity bounds as dirty
	this->MarkLocalEntityBoundsDirty();

	return TRUE;
}

void CRuModel::CalculateEntityBounds()
{
	CRuSphere localBounds;
	const CRuMatrix4x4 &worldTransform = this->GetWorldTransform();

	if(m_subModels.Count() > 0)
	{
		localBounds = m_subModels[0].m_mesh->GetBounds();
		m_subModels[0].m_meshIterationNumber = m_subModels[0].m_mesh->GetMeshIteration();

		for(INT32 i = 1; i < m_subModels.Count(); ++i)
		{
			localBounds.Merge(m_subModels[i].m_mesh->GetBounds());
			m_subModels[i].m_meshIterationNumber = m_subModels[0].m_mesh->GetMeshIteration();
		}
	}
	else
	{
		// No meshes, so set the mesh's bounds to be at the origin of local space
		localBounds.Set(CRuVector3(0.0f, 0.0f, 0.0f), 0.0f);
	}

	// Set local entity bounds
	this->SetLocalEntityBounds(localBounds);
}

// ************************************************************************************************************************************************************

#pragma managed
