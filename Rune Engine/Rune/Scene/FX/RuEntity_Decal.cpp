/*!
	@file RuEntity_Decal.cpp

	Copyright (c) 2004-2006 All rights reserved
*/

#include "../../Scene/FX/RuEntity_Decal.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Decal, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Decal, "CRuEntity_Decal", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuEntity_Decal, 0x7D3E8CA6, 0x94BE4aa3, 0x8CD5E352, 0x19E86E59)

// ************************************************************************************************************************************************************

CRuEntity_Decal::CRuEntity_Decal()
:	m_maximumDecalRadius(240),
	m_maximumDecalFaces(4000),
	m_uvBillboardType(ruBILLBOARDTYPE_NONE),
	m_decalRadius(1.0f),
	m_decalVerticalLimit(10.0f),
	m_enumerationPadding(20.0f),
	m_enumerationBounds(CRuVector3(0.0f, 0.0f, 0.0f), 0.0f),
	m_mesh(NULL),
	m_material(NULL)
{
	m_decalTransform.SetToIdentity();

	m_mesh = ruNEW CRuDynamicMesh();
	m_mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, m_maximumDecalFaces * 3, m_maximumDecalFaces, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);
	m_mesh->AddTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
	m_mesh->SetNumPrimitives(0);
	m_mesh->SetNumVertices(0);

	m_material = ruNEW CRuMaterial();
	m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_FX_DECAL"));

	// By default, set material as translucent
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_TRANSLUCENT, TRUE);
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_GREATER);
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 0);

	// Register event handlers
	Event_Camera().RegisterHandler(this->GetGUID(), boost::bind(&CRuEntity_Decal::HandleEvent_Camera, this, _1));
	Event_DecalTarget().RegisterHandler(this->GetGUID(), boost::bind(&CRuEntity_Decal::HandleEvent_DecalTarget, this, _1));
}

CRuEntity_Decal::~CRuEntity_Decal()
{
	ruSAFE_RELEASE(m_mesh);
	ruSAFE_RELEASE(m_material);
}

void CRuEntity_Decal::SetDecalRadius(REAL decalRadius)
{
	m_decalRadius = decalRadius;
}

void CRuEntity_Decal::SetDecalTexture(IRuTexture *texture)
{
	m_material->SetTexture(ruTEXCHANNEL_DIFFUSEMAP, texture);
}

void CRuEntity_Decal::SetDecalTransform(const CRuMatrix4x4 &decalTransform)
{
	m_decalTransform = decalTransform;
}

void CRuEntity_Decal::SetDecalUVBasis(const CRuVector3 basisVectors[3], REAL uOffset, REAL vOffset)
{
	m_decalUVBasis[0] = basisVectors[0];
	m_decalUVBasis[1] = basisVectors[1];
	m_decalUVBasis[2] = basisVectors[2];

	m_decalUOffset = uOffset;
	m_decalVOffset = vOffset;
}

void CRuEntity_Decal::BuildDecalMesh(const CRuArrayList<CRuEntity *> &decalTargets)
{
	// Setup decal bounds
	REAL scale = this->GetScale(ruFRAMETYPE_WORLD);
	CRuVector3 translation = this->GetTranslation(ruFRAMETYPE_WORLD);

	m_decalBounds.Set(translation, scale * m_decalRadius);

	// Cap decal radius to the maximum decal radius
	m_decalBounds.Radius() = min(m_maximumDecalRadius, m_decalBounds.Radius());

	if(m_enumerationBounds.Contains(m_decalBounds) == FALSE)
	{
		m_enumerationBounds.Set(m_decalBounds.Center(), m_decalBounds.Radius() + m_enumerationPadding);
		m_decalBounds = m_enumerationBounds;

//		const CRuSphere &decalBounds = m_decalBounds;
		CRuOBB decalBounds(m_decalBounds);

		CRuMorphTarget *morphTarget = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0));
		CRuVector3 *position = morphTarget->GetPosition();
		DWORD *diffuse = morphTarget->GetDiffuse();
		//	REAL *uv = morphTarget->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
		UINT16 *indices = m_mesh->GetIndices();

		INT32 vertexIndex = 0;
		INT32 iIndex = 0;

		CRuVector3 refVec(0.0f, 1.0f, 0.0f);

		for(INT32 i = 0; i < decalTargets.Count(); ++i)
		{

			// Build triangle enumeration query
			CRuCollisionQuery_OBB_TriEnum triEnumQuery;
			triEnumQuery.m_exclusionMask = 0x00A00000;		// Exclude water and foliage surfaces
			triEnumQuery.m_queryOnOriginalGeometry = TRUE;
			triEnumQuery.m_colOBB = decalBounds;
/*
			// Build triangle enumeration query
			CRuCollisionQuery_Sphere_TriEnum triEnumQuery;
			triEnumQuery.m_exclusionMask = 0x00A00000;		// Exclude water and foliage surfaces
			triEnumQuery.m_queryOnOriginalGeometry = TRUE;
			triEnumQuery.m_colSphere = decalBounds;
*/
			// Perform collision query on the current decal target
			decalTargets[i]->QueryCollision(&triEnumQuery);

			INT32 maxVertsToProcess = min(m_maximumDecalFaces * 3 - vertexIndex, triEnumQuery.m_colTriVerts->Count());
			if(maxVertsToProcess == 0)
				continue;

			// Iterate through colliding triangles and add them to the decal mesh
			for(INT32 j = 0, k = 0; j < maxVertsToProcess; j += 3, ++k)
			{
				CRuVector3 &triNormal = triEnumQuery.m_colTriNormals->Get(k);
				CRuVector3 centroid = (triEnumQuery.m_colTriVerts->Get(j) + triEnumQuery.m_colTriVerts->Get(j + 1) + triEnumQuery.m_colTriVerts->Get(j + 2)) / 3.0f;

				if(DotProduct(triNormal, refVec) > 0.2f && fabs(centroid.m_y - decalBounds.Center().m_y) <= m_decalVerticalLimit)
				{
					position[vertexIndex] = triEnumQuery.m_colTriVerts->Get(j);
					position[vertexIndex + 1] = triEnumQuery.m_colTriVerts->Get(j + 1);
					position[vertexIndex + 2] = triEnumQuery.m_colTriVerts->Get(j + 2);

					diffuse[vertexIndex] = 0xFFFFFFFF;
					diffuse[vertexIndex + 1] = 0xFFFFFFFF;
					diffuse[vertexIndex + 2] = 0xFFFFFFFF;

					indices[iIndex++] = vertexIndex;
					indices[iIndex++] = vertexIndex + 1;
					indices[iIndex++] = vertexIndex + 2;

					vertexIndex += 3;
				}
			}
		}

		m_mesh->SetNumPrimitives(iIndex / 3);
		m_mesh->SetNumVertices(vertexIndex);

		// Increment dynamic mesh iteration
		m_mesh->IncrementMeshIteration();
		m_mesh->IncrementTopologyIteration();

		this->SetLocalEntityBounds(CRuSphere(CRuVector3(0.0f, 0.0f, 0.0f), m_decalRadius));
	}
}

RuBillboardType CRuEntity_Decal::GetBillboardType()
{
	return m_uvBillboardType;
}

void CRuEntity_Decal::SetBillboardType(RuBillboardType billboardType)
{
	m_uvBillboardType = billboardType;
}

IRuObject *CRuEntity_Decal::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuEntity_Decal();

	// Chain cloning call to parent class
	IRuEntity_Renderable::Clone(clonedObject);

	return clonedObject;
}

BOOL CRuEntity_Decal::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Renderable::SerializeFrom(stream);

	return TRUE;
}

BOOL CRuEntity_Decal::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Renderable::SerializeTo(stream);

	return TRUE;
}

BOOL CRuEntity_Decal::Update(REAL elapsedTime)
{
	// Chain update to base class
	IRuEntity_Renderable::Update(elapsedTime);

	// Update UV transform
	switch(m_uvBillboardType)
	{
		default:
		case ruBILLBOARDTYPE_NONE:
			{
				REAL scale = this->GetScale(ruFRAMETYPE_WORLD);
				CRuVector3 translation = this->GetTranslation(ruFRAMETYPE_WORLD);

				m_uvTransform = RuUVTransform_Projection(translation, m_decalTransform, m_decalUVBasis, m_decalRadius * scale * 2.0f, m_decalUOffset, m_decalVOffset);
			}

			break;

		case ruBILLBOARDTYPE_AXIAL:
			{
				CRuMatrix4x4 aggregateMatrix;
				aggregateMatrix.SetToAxialBillboard(m_aggregateMatrix, CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 1.0f, 0.0f));

				// NOTE: I am not sure why, but the axial billboard rotates points in the OPPOSITE direction. Is the UV transform interfering
				//       with the billboard in this case? Need to investigate. For now, it works.
//				aggregateMatrix = m_decalTransform * aggregateMatrix.GetInverse();
				aggregateMatrix = aggregateMatrix.GetInverse();

				REAL scale = this->GetScale(ruFRAMETYPE_WORLD);
				CRuVector3 translation = this->GetTranslation(ruFRAMETYPE_WORLD);

				m_uvTransform = RuUVTransform_Projection(translation, aggregateMatrix, m_decalUVBasis, m_decalRadius * scale * 2.0f, m_decalUOffset, m_decalVOffset);
			}

			break;
	}

	// Set UV transform into material
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_TEXTURE_TRANSFORM_0, reinterpret_cast<UINT64>(&m_uvTransform));

	return TRUE;
}

INT32 CRuEntity_Decal::GetNumMeshes() const
{
	if(m_material->BindTextures(TRUE))
	{
		return 1;
	}

	return 0;
}

BOOL CRuEntity_Decal::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex == 0)
	{
		*meshOut = m_mesh;
		*materialOut = m_material;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_Decal::HandleEvent_Camera(RuEventArgs *eventArgs)
{
	RuEntityEvent_Camera_Args *cameraArgs = static_cast<RuEntityEvent_Camera_Args *>(eventArgs); 
	CRuCamera *camera = cameraArgs->m_camera;

	// Store aggregated transform matrix
	m_aggregateMatrix = GetWorldTransform() * camera->GetCameraTransform();

	return TRUE;
}

BOOL CRuEntity_Decal::HandleEvent_DecalTarget(RuEventArgs *eventArgs)
{
	// Cast parameter 0 as the decal target list
	RuEntityEvent_DecalTarget_Args *decalTargetEventArgs = static_cast<RuEntityEvent_DecalTarget_Args *>(eventArgs);

	// Build decal mesh
	BuildDecalMesh(*(decalTargetEventArgs->m_validDecalTargets));

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
