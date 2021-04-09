#include "../../Scene/ColVis/RuEntity_ColVis.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_ColVis, IRuEntity_Container)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_ColVis, "CRuEntity_ColVis", "IRuEntity_Container")
ruCLASSGUID_DEFINE(CRuEntity_ColVis, 0xE806D612, 0xEC244402, 0x9E96A8D7, 0x8333C789)

// ************************************************************************************************************************************************************

CRuEntity_ColVis::CRuEntity_ColVis()
{
	m_sourceColHierarchy = NULL;

	// Initialize model queue
	m_modelQueue = ruNEW CRuLRUQueue<CRuGUID, CRuModel *>(128, 1024);

	m_highlightSurfaceID = 0;
	m_exclusionMask = 0x00000000;
}

CRuEntity_ColVis::~CRuEntity_ColVis()
{
	// Flush collision object queue
	FlushModelQueue();

	// Destroy collision object queue
	delete m_modelQueue;

	ruSAFE_RELEASE(m_sourceColHierarchy);
}

void CRuEntity_ColVis::SetSourceCollisionHierarchy(IRuCollisionHierarchy *sourceColHierarchy)
{
	// Reset source collision hierarchy
	sourceColHierarchy->AddRef();
	ruSAFE_RELEASE(m_sourceColHierarchy);
	m_sourceColHierarchy = sourceColHierarchy;

	// Clear all existing cache
	FlushModelQueue();
}

IRuObject *CRuEntity_ColVis::Clone(IRuObject *clonedObject)
{
	// This object cannot be cloned
	return NULL;
}

BOOL CRuEntity_ColVis::ResetStates()
{
	return TRUE;
}

BOOL CRuEntity_ColVis::Update(REAL elapsedTime)
{
	return IRuEntity_Container::Update(elapsedTime);
}
/*
void CRuEntity_ColVis::UpdateEntityTransform()
{
	// Chain transform update call to base class
	IRuEntity_Container::UpdateEntityTransform();

	// Invoke entity transform update on list of visible objects
	for(INT32 i = 0; i < m_visibleObjects.Count(); ++i)
	{
		if(m_visibleObjects[i])
		{
			m_visibleObjects[i]->UpdateEntityTransform();
		}
	}
}

void CRuEntity_ColVis::UpdateEntityBounds()
{
	// Chain bounds update call to base class
	IRuEntity_Container::UpdateEntityBounds();

	// Invoke entity bounds update on visible objects
	for(INT32 i = 0; i < m_visibleObjects.Count(); ++i)
	{
		if(m_visibleObjects[i])
		{
			m_visibleObjects[i]->UpdateEntityBounds();
		}
	}
}
*/
INT32 CRuEntity_ColVis::GetNumEntities()
{
	return m_visibleObjects.Count();
}

CRuEntity *CRuEntity_ColVis::GetEntity(INT32 index)
{
	return m_visibleObjects[index];
}

BOOL CRuEntity_ColVis::QueryCollision(CRuCollisionQuery *colQuery)
{
	// If collision query is disabled, return immediately
	if(colQuery->m_ignoreCollisionFlag == FALSE && GetOption(ruENTITYOPTION_COLLISION) == 0)
		return TRUE;

	switch(colQuery->m_queryType)
	{
		case ruCOLQUERY_USER_FLAG_SET_RAY:
			{
				m_sourceColHierarchy->QueryCollision(colQuery);

				CRuCollisionQuery_User_Flag_Set_Ray *setQuery = static_cast<CRuCollisionQuery_User_Flag_Set_Ray *>(colQuery);
				for(INT32 i = 0; i < setQuery->m_affectedObjects->Count(); ++i)
				{
					CRuGUID objKey = static_cast<IRuObject *>(setQuery->m_affectedObjects->Get(i))->GetGUID();
					DWORD objKeyHash = objKey.HashCode();

					CRuModel *affectedModel = NULL;
					if(m_modelQueue->Get(objKey, objKeyHash, affectedModel))
					{
						ruSAFE_RELEASE(affectedModel);
						m_modelQueue->Remove(objKey, objKeyHash);
					}
				}
			}

			return TRUE;

		case ruCOLQUERY_USER_FLAG_SET_SPHERE:
			{
				m_sourceColHierarchy->QueryCollision(colQuery);

				CRuCollisionQuery_User_Flag_Set_Sphere *setQuery = static_cast<CRuCollisionQuery_User_Flag_Set_Sphere *>(colQuery);
				for(INT32 i = 0; i < setQuery->m_affectedObjects->Count(); ++i)
				{
					CRuGUID objKey = static_cast<IRuObject *>(setQuery->m_affectedObjects->Get(i))->GetGUID();
					DWORD objKeyHash = objKey.HashCode();

					CRuModel *affectedModel = NULL;
					if(m_modelQueue->Get(objKey, objKeyHash, affectedModel))
					{
						ruSAFE_RELEASE(affectedModel);
						m_modelQueue->Remove(objKey, objKeyHash);
					}
				}
			}

			return TRUE;
	}

	return FALSE;
}

void CRuEntity_ColVis::SetCamera(CRuCamera *viewCamera)
{
	// Clear existing visible list
	m_visibleObjects.Clear();

	// Enumerate visible collision objects
	CRuCollisionQuery_Frustum_SubObject subObjQuery(viewCamera->GetFrustumPlanes());
	m_sourceColHierarchy->QueryCollision(&subObjQuery);

	const CRuPlane4 *frustumPlanes = viewCamera->GetFrustumPlanes();

	for(int i = 0; i < subObjQuery.m_colObjects->Count(); ++i)
	{
		// Get collision object bounds
		const CRuAABB &subObjBounds = static_cast<IRuCollisionObject *>(subObjQuery.m_colObjects->Get(i))->GetBounds();

		// Test visibility against camera
		if(IntersectsFrustum(frustumPlanes, subObjBounds))
		{
			// Test distance criteria
			REAL camDistance = GetDistance(subObjBounds, viewCamera->GetCameraPosition());

			if(camDistance < 600.0f)
			{
				// Instance the collision object model
				CRuModel *objModel = InstanceModel(static_cast<IRuCollisionObject *>(subObjQuery.m_colObjects->Get(i)));

				// Append to visible list
				if(objModel)
				{
					m_visibleObjects.Add(objModel);
				}
			}
		}
	}

}

void CRuEntity_ColVis::SetHighlightSurfaceID(UINT32 highlightSurfaceID, UINT32 highlightSurfaceFlag)
{
	if(m_highlightSurfaceID != highlightSurfaceID || m_highlightSurfaceFlag != highlightSurfaceFlag)
	{
		m_highlightSurfaceID = highlightSurfaceID;
		m_highlightSurfaceFlag = highlightSurfaceFlag;

		// Flush all models and force re-generation
		FlushModelQueue();
	}
}

void CRuEntity_ColVis::SetExclusionMask(UINT32 mask)
{
	if(m_exclusionMask != mask)
	{
		m_exclusionMask = mask;

		// Flush all models and force re-generation
		FlushModelQueue();
	}
}

CRuModel *CRuEntity_ColVis::InstanceModel(IRuCollisionObject *colObject)
{
	CRuGUID objKey = colObject->GetGUID();
	DWORD objKeyHash = objKey.HashCode();

	CRuModel *objModel = NULL;

	if(m_modelQueue->Get(objKey, objKeyHash, objModel) == FALSE)
	{
		// Reclaim space
		CRuModel *reclaimedObjModel;
		while(m_modelQueue->Reclaim(objKey, objKeyHash, 1, reclaimedObjModel))
		{
			ruSAFE_RELEASE(reclaimedObjModel);
		}

		CRuArrayList<IRuMesh *> meshList;

		BuildMeshRepresentation(colObject, m_highlightSurfaceID, m_highlightSurfaceFlag, m_exclusionMask, meshList);

		objModel = ruNEW CRuModel();

		for(INT32 i = 0; i < meshList.Count(); ++i)
		{
			CRuMaterial *colMaterial = ruNEW CRuMaterial();
			colMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_EDITOR_COLVIS"));
			colMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, 0x40FFFFFF);
			colMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(0.10f));

			objModel->AddSubModel(meshList[i], colMaterial);

			ruSAFE_RELEASE(meshList[i]);
			ruSAFE_RELEASE(colMaterial);
		}

		// Add the model to the queue
		m_modelQueue->Add(objKey, objKeyHash, objModel, 1);
	}

	return objModel;
}

BOOL CRuEntity_ColVis::FlushModelQueue()
{
	if(m_modelQueue)
	{
		// Iterate through all instanced entities
		m_modelQueue->ForAllElements(FlushModelQueueCallback, this);

		// Clear instanced entity queue
		m_modelQueue->Clear();
	}

	return TRUE;
}

BOOL CRuEntity_ColVis::FlushModelQueueCallback(const void *key, const void *data, void *userData)
{
	CRuModel *colObj = *((CRuModel **) data);

	ruSAFE_RELEASE(colObj);

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL BuildMeshRepresentation(IRuCollisionObject *collisionObj, UINT32 highlightSurfaceID, UINT32 highlightSurfaceFlag, UINT32 exclusionMask, CRuArrayList<IRuMesh *> &meshListOut)
{
	// Query collision object for all of its triangles
	CRuCollisionQuery_User_Flag_Enumerate enumQuery;
	enumQuery.m_exclusionMask = exclusionMask;
	collisionObj->QueryCollision(&enumQuery);

	// Construct mesh
	if(enumQuery.m_colTriVerts->Count() > 0)
	{
		UINT32 maskedHighlighSurfaceID = highlightSurfaceID & highlightSurfaceFlag;
		INT32 maxTrisPerMesh = 20000;
		INT32 numTris = enumQuery.m_colTriVerts->Count() / 3;
		INT32 numMeshes = numTris / maxTrisPerMesh + ((numTris % maxTrisPerMesh) ? 1 : 0);

		for(INT32 i = 0, vertIndex = 0, triIndex = 0, trisLeft = numTris; i < numMeshes; ++i)
		{
			INT32 trisThisBatch = min(maxTrisPerMesh, trisLeft);

			CRuMesh *mesh = ruNEW CRuMesh();
			mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, trisThisBatch * 3, trisThisBatch, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_NORMAL | ruVERTEXFORMAT_DIFFUSE);

			CRuVector3 *position = static_cast<CRuMorphTarget *>(mesh->GetMorphTarget(0))->GetPosition();
			CRuVector3 *normal = static_cast<CRuMorphTarget *>(mesh->GetMorphTarget(0))->GetNormal();
			DWORD *diffuse = static_cast<CRuMorphTarget *>(mesh->GetMorphTarget(0))->GetDiffuse();
			UINT16 *indices = mesh->GetIndices();

			for(INT32 k = 0, l = 0; k < trisThisBatch; ++k, vertIndex += 3, ++triIndex, --trisLeft)
			{
				const CRuVector3 &v0 = enumQuery.m_colTriVerts->Get(vertIndex);
				const CRuVector3 &v1 = enumQuery.m_colTriVerts->Get(vertIndex + 1);
				const CRuVector3 &v2 = enumQuery.m_colTriVerts->Get(vertIndex + 2);

				INT32 surfaceID = enumQuery.m_colTriFlags->Get(triIndex) & highlightSurfaceFlag;

				DWORD diffuseColor = 0xFFFFFFFF;
				if(surfaceID == maskedHighlighSurfaceID)
				{
					diffuseColor = 0xFFFFFF00;
				}

				CRuVector3 e0 = v0 - v2;
				CRuVector3 e1 = v1 - v2;
				CRuVector3 n = CrossProduct(e0, e1);

				indices[l] = l;
				position[l] = v0;
				normal[l] = n;
				diffuse[l] = diffuseColor;
				l++;

				indices[l] = l;
				position[l] = v1;
				normal[l] = n;
				diffuse[l] = diffuseColor;
				l++;

				indices[l] = l;
				position[l] = v2;
				normal[l] = n;
				diffuse[l] = diffuseColor;
				l++;
			}

			mesh->CalculateBounds();
			meshListOut.Add(mesh);
		}

		return TRUE;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

IRuMesh *BuildMeshRepresentation(IRuCollisionObject *collisionObj)
{
	// Query collision object for all of its triangles
	CRuCollisionQuery_Enumerate enumQuery;
	collisionObj->QueryCollision(&enumQuery);

	// Construct mesh
	if(enumQuery.m_colTriVerts->Count() > 0)
	{
		CRuMesh *mesh = ruNEW CRuMesh();

		mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, enumQuery.m_colTriVerts->Count(), enumQuery.m_colTriVerts->Count() / 3, ruVERTEXFORMAT_POSITION);

		CRuVector3 *position = static_cast<CRuMorphTarget *>(mesh->GetMorphTarget(0))->GetPosition();
		UINT16 *indices = mesh->GetIndices();

		for(INT32 i = 0; i < enumQuery.m_colTriVerts->Count(); ++i)
		{
			indices[i] = i;
			position[i] = enumQuery.m_colTriVerts->Get(i);
		}

		return mesh;
	}

	return NULL;
}

// ************************************************************************************************************************************************************

#pragma managed
