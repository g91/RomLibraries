/*!
	@file RuGizmo_Plane.cpp

	Copyright (c) 2004-2006 All rights reserved
*/

#include "../../Scene/Gizmo/RuGizmo_Plane.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuGizmo_Plane, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuGizmo_Plane, "CRuGizmo_Plane", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuGizmo_Plane, 0x1620DA48, 0x56714516, 0x9D25B534, 0xDD8E05E1)

// ************************************************************************************************************************************************************

CRuGizmo_Plane::CRuGizmo_Plane()
:	m_gizmoHandleSize(5.0f),
	m_planeBounds(-1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f)
{
	// Build new mesh for the gizmo
	m_mesh = ruNEW CRuMesh();
	m_mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, 4, 2, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);

	// Fill in vertex data
	CRuVector3 *position = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetDiffuse();

	position[0].Set(m_planeBounds.Minima().m_x, 0.0f, m_planeBounds.Minima().m_z);
	position[1].Set(m_planeBounds.Maxima().m_x, 0.0f, m_planeBounds.Minima().m_z);
	position[2].Set(m_planeBounds.Minima().m_x, 0.0f, m_planeBounds.Maxima().m_z);
	position[3].Set(m_planeBounds.Maxima().m_x, 0.0f, m_planeBounds.Maxima().m_z);

	diffuse[0] = 0x7FFFFFFF;
	diffuse[1] = 0x7FFFFFFF;
	diffuse[2] = 0x7FFFFFFF;
	diffuse[3] = 0x7FFFFFFF;

	// Fill in topology data
	UINT16 *indices = m_mesh->GetIndices();

	indices[0] = 0;
	indices[1] = 3;
	indices[2] = 1;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	// Build new material for the gizmo
	m_material = ruNEW CRuMaterial();

	m_material->SetProperty("material:materialType", "Standard Material");

	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_2SIDED, TRUE);
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_TRANSLUCENT, TRUE);

	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_NONE);
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 0);

	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATIONCOLOR, RUCOLOR_ARGB(0, 255, 255, 255));
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(1.0f));

	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ZTESTMODE, ruZTESTMODE_ALWAYS);
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ZWRITEMODE, ruZWRITEMODE_DISABLE);

	// Flag local entity bounds as dirty
	this->MarkLocalEntityBoundsDirty();

	// Update gizmo handles
	UpdateGizmoHandles();
}

CRuGizmo_Plane::~CRuGizmo_Plane()
{
	ruSAFE_RELEASE(m_mesh);
	ruSAFE_RELEASE(m_material);
}

BOOL CRuGizmo_Plane::SerializeFrom(IRuStream *stream)
{
	// Read class GUID
	CRuGUID classGUID;
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Renderable::SerializeFrom(stream);

	// Read plane bounds
	stream->Read(&m_planeBounds.Minima().m_x, sizeof(REAL));
	stream->Read(&m_planeBounds.Minima().m_y, sizeof(REAL));
	stream->Read(&m_planeBounds.Minima().m_z, sizeof(REAL));
	stream->Read(&m_planeBounds.Maxima().m_x, sizeof(REAL));
	stream->Read(&m_planeBounds.Maxima().m_y, sizeof(REAL));
	stream->Read(&m_planeBounds.Maxima().m_z, sizeof(REAL));

	// Update gizmo mesh
	UpdateGizmoMesh();

	return TRUE;
}

BOOL CRuGizmo_Plane::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Renderable::SerializeTo(stream);

	// Write plane bounds
	stream->Write(&m_planeBounds.Minima().m_x, sizeof(REAL));
	stream->Write(&m_planeBounds.Minima().m_y, sizeof(REAL));
	stream->Write(&m_planeBounds.Minima().m_z, sizeof(REAL));
	stream->Write(&m_planeBounds.Maxima().m_x, sizeof(REAL));
	stream->Write(&m_planeBounds.Maxima().m_y, sizeof(REAL));
	stream->Write(&m_planeBounds.Maxima().m_z, sizeof(REAL));

	return TRUE;
}

BOOL CRuGizmo_Plane::Update(float elapsedTime)
{
	return IRuEntity_Renderable::Update(elapsedTime);
}

BOOL CRuGizmo_Plane::QueryCollision(CRuCollisionQuery *colQuery)
{
	CRuVector3 *position = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
	UINT16 *indices = m_mesh->GetIndices();

	switch(colQuery->m_queryType)
	{
		case ruCOLQUERY_RAY_NEAREST:
			{
				CRuCollisionQuery_Ray_Nearest *rayQuery = static_cast<CRuCollisionQuery_Ray_Nearest *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 localRay(rayQuery->m_colRay, this->GetInverseWorldTransform());

				CRuVector3 intPt;
				REAL intT;

				for(INT32 i = 0, j = 0, numPrimitives = m_mesh->GetNumPrimitives(); i < numPrimitives; ++i)
				{
					INT32 faceIndices[3] = { indices[j++], indices[j++], indices[j++] };

					if(FindIntersection(localRay, position[faceIndices[0]], position[faceIndices[1]], position[faceIndices[2]], intPt, intT))
					{
						if(rayQuery->m_ignoreBackface)
						{
							// Perform a backface check
							CRuVector3 e0(position[faceIndices[1]] - position[faceIndices[0]]);
							CRuVector3 e1(position[faceIndices[2]] - position[faceIndices[0]]);
							CRuVector3 n = CrossProduct(e0, e1);

							// Normal and ray direction must be opposite
							if(DotProduct(localRay.Direction(), n) < 0.0f)
							{
								rayQuery->m_positiveHit = TRUE;
								rayQuery->m_collisionPoint = intPt * this->GetWorldTransform();
								rayQuery->m_collisionT = intT;
							}
						}
						else
						{
							rayQuery->m_positiveHit = TRUE;
							rayQuery->m_collisionPoint = intPt * this->GetWorldTransform();
							rayQuery->m_collisionT = intT;
						}
					}
				}

				return TRUE;
			}

			break;

		case ruCOLQUERY_SEGMENT_NEAREST:
			{
				CRuCollisionQuery_Segment_Nearest *segQuery = static_cast<CRuCollisionQuery_Segment_Nearest *>(colQuery);

				// Get the segment in local coordinates
				CRuSegment3 localSegment(segQuery->m_colSegment);
				this->GetInverseWorldTransform().TransformPoint(localSegment.Origin());
				this->GetInverseWorldTransform().TransformVector(localSegment.Direction());

				CRuVector3 intPt;
				REAL intT;

				for(INT32 i = 0, j = 0, numPrimitives = m_mesh->GetNumPrimitives(); i < numPrimitives; ++i)
				{
					INT32 faceIndices[3] = { indices[j++], indices[j++], indices[j++] };

					if(FindIntersection(localSegment, position[faceIndices[0]], position[faceIndices[1]], position[faceIndices[2]], intPt, intT))
					{
						if(segQuery->m_ignoreBackface)
						{
							// Perform a backface check
							CRuVector3 e0(position[faceIndices[1]] - position[faceIndices[0]]);
							CRuVector3 e1(position[faceIndices[2]] - position[faceIndices[0]]);
							CRuVector3 n = CrossProduct(e0, e1);

							// Normal and ray direction must be opposite
							if(DotProduct(localSegment.Direction(), n) < 0.0f)
							{
								segQuery->m_positiveHit = TRUE;
								segQuery->m_collisionPoint = intPt * this->GetWorldTransform();
								segQuery->m_collisionT = intT;
							}
						}
						else
						{
							segQuery->m_positiveHit = TRUE;
							segQuery->m_collisionPoint = intPt * this->GetWorldTransform();
							segQuery->m_collisionT = intT;
						}
					}
				}

				return TRUE;
			}

			break;
	}

	return TRUE;
}

void CRuGizmo_Plane::CalculateEntityBounds()
{
	// Force calculation of mesh bounds
	m_mesh->CalculateBounds();

	// Set mesh bounds as local entity bounds
	this->SetLocalEntityBounds(m_mesh->GetBounds());
}

INT32 CRuGizmo_Plane::GetNumMeshes() const
{
	return 1;
}

BOOL CRuGizmo_Plane::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex == 0)
	{
		*meshOut = m_mesh;
		*materialOut = m_material;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuGizmo_Plane::ActivateManipulator(const CRuRay3 &pickRay)
{
	const CRuMatrix4x4 &inverseWorldTransform = this->GetInverseWorldTransform();

	// Get the ray in local coordinates
	CRuRay3 localRay;
	inverseWorldTransform.TransformPoint(pickRay.Origin(), localRay.Origin());
	inverseWorldTransform.TransformVector(pickRay.Direction(), localRay.Direction());

	// Test collision with bounding boxes
	INT32 nearestPart = 0;
	float nearestT = FLT_MAX;
	for(INT32 i = 0; i < ruGIZMOHANDLE_COUNT; ++i)
	{
		CRuVector3 intPt;
		float intT;

		if(FindIntersection(localRay, m_gizmoHandles[i], intPt, intT))
		{
			if(intT < nearestT)
			{
				nearestPart = i;
				nearestT = intT;
			}
		}
	}

	// If we found a suitable gizmo manipulator to activate, save it and return TRUE
	if(nearestT < FLT_MAX)
	{
		m_activeGizmoPickRay = pickRay;
		m_activeGizmoHandle = static_cast<RuGizmoHandle>(nearestPart);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuGizmo_Plane::Manipulate(const CRuRay3 &pickRay)
{
	CRuVector3 movementVector;

	if(GetManipulationVector(pickRay, movementVector))
	{
		// Update pick ray
		m_activeGizmoPickRay = pickRay;

		switch(m_activeGizmoHandle)
		{
			case ruGIZMOHANDLE_X0Z0:
				m_planeBounds.Minima().m_x += movementVector.m_x;
				m_planeBounds.Minima().m_z += movementVector.m_z;
				break;

			case ruGIZMOHANDLE_X1Z0:
				m_planeBounds.Maxima().m_x += movementVector.m_x;
				m_planeBounds.Minima().m_z += movementVector.m_z;
				break;

			case ruGIZMOHANDLE_X0Z1:
				m_planeBounds.Minima().m_x += movementVector.m_x;
				m_planeBounds.Maxima().m_z += movementVector.m_z;
				break;

			case ruGIZMOHANDLE_X1Z1:
				m_planeBounds.Maxima().m_x += movementVector.m_x;
				m_planeBounds.Maxima().m_z += movementVector.m_z;
				break;

			case ruGIZMOHANDLE_X0:
				m_planeBounds.Minima().m_x += movementVector.m_x;
				break;

			case ruGIZMOHANDLE_X1:
				m_planeBounds.Maxima().m_x += movementVector.m_x;
				break;

			case ruGIZMOHANDLE_Z0:
				m_planeBounds.Minima().m_z += movementVector.m_z;
				break;

			case ruGIZMOHANDLE_Z1:
				m_planeBounds.Maxima().m_z += movementVector.m_z;
				break;
		}

		if(m_planeBounds.Minima().m_x > m_planeBounds.Maxima().m_x)
		{
			REAL tempValue = m_planeBounds.Minima().m_x;
			m_planeBounds.Minima().m_x = m_planeBounds.Maxima().m_x;
			m_planeBounds.Maxima().m_x = tempValue;
		}

		if(m_planeBounds.Minima().m_y > m_planeBounds.Maxima().m_y)
		{
			REAL tempValue = m_planeBounds.Minima().m_y;
			m_planeBounds.Minima().m_y = m_planeBounds.Maxima().m_y;
			m_planeBounds.Maxima().m_y = tempValue;
		}

		if(m_planeBounds.Minima().m_z > m_planeBounds.Maxima().m_z)
		{
			REAL tempValue = m_planeBounds.Minima().m_z;
			m_planeBounds.Minima().m_z = m_planeBounds.Maxima().m_z;
			m_planeBounds.Maxima().m_z = tempValue;
		}

		UpdateGizmoMesh();

		return TRUE;
	}

	return FALSE;
}

const CRuAABB &CRuGizmo_Plane::GetPlaneBounds()
{
	return m_planeBounds;
}

void CRuGizmo_Plane::UpdateGizmoMesh()
{
	// Fill in vertex data
	CRuVector3 *position = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();

	position[0].Set(m_planeBounds.Minima().m_x, 0.0f, m_planeBounds.Minima().m_z);
	position[1].Set(m_planeBounds.Maxima().m_x, 0.0f, m_planeBounds.Minima().m_z);
	position[2].Set(m_planeBounds.Minima().m_x, 0.0f, m_planeBounds.Maxima().m_z);
	position[3].Set(m_planeBounds.Maxima().m_x, 0.0f, m_planeBounds.Maxima().m_z);

	m_mesh->IncrementMeshIteration();

	UpdateGizmoHandles();

	// Flag local entity bounds as dirty
	this->MarkLocalEntityBoundsDirty();
}

void CRuGizmo_Plane::UpdateGizmoHandles()
{
	m_gizmoHandles[ruGIZMOHANDLE_X0Z0].SetBounds(m_planeBounds.Minima().m_x, 0.0f, m_planeBounds.Minima().m_z, m_planeBounds.Minima().m_x + m_gizmoHandleSize, 0.0f, m_planeBounds.Minima().m_z + m_gizmoHandleSize);
	m_gizmoHandles[ruGIZMOHANDLE_X1Z0].SetBounds(m_planeBounds.Maxima().m_x - m_gizmoHandleSize, 0.0f, m_planeBounds.Minima().m_z, m_planeBounds.Maxima().m_x, 0.0f, m_planeBounds.Minima().m_z + m_gizmoHandleSize);
	m_gizmoHandles[ruGIZMOHANDLE_X0Z1].SetBounds(m_planeBounds.Minima().m_x, 0.0f, m_planeBounds.Maxima().m_z - m_gizmoHandleSize, m_planeBounds.Minima().m_x + m_gizmoHandleSize, 0.0f, m_planeBounds.Maxima().m_z);
	m_gizmoHandles[ruGIZMOHANDLE_X1Z1].SetBounds(m_planeBounds.Maxima().m_x - m_gizmoHandleSize, 0.0f, m_planeBounds.Maxima().m_z - m_gizmoHandleSize, m_planeBounds.Maxima().m_x, 0.0f, m_planeBounds.Maxima().m_z);

	m_gizmoHandles[ruGIZMOHANDLE_X0].SetBounds(m_planeBounds.Minima().m_x, 0.0f, m_planeBounds.Minima().m_z, m_planeBounds.Minima().m_x + m_gizmoHandleSize, 0.0f, m_planeBounds.Maxima().m_z);
	m_gizmoHandles[ruGIZMOHANDLE_X1].SetBounds(m_planeBounds.Maxima().m_x - m_gizmoHandleSize, 0.0f, m_planeBounds.Minima().m_z, m_planeBounds.Maxima().m_x, 0.0f, m_planeBounds.Maxima().m_z);
	m_gizmoHandles[ruGIZMOHANDLE_Z0].SetBounds(m_planeBounds.Minima().m_x, 0.0f, m_planeBounds.Minima().m_z, m_planeBounds.Maxima().m_x, 0.0f, m_planeBounds.Minima().m_z + m_gizmoHandleSize);
	m_gizmoHandles[ruGIZMOHANDLE_Z1].SetBounds(m_planeBounds.Minima().m_x, 0.0f, m_planeBounds.Maxima().m_z - m_gizmoHandleSize, m_planeBounds.Maxima().m_x, 0.0f, m_planeBounds.Maxima().m_z);
}

BOOL CRuGizmo_Plane::GetManipulationVector(const CRuRay3 &pickRay, CRuVector3 &vectorOut)
{
	REAL stopThreshold = 0.05f;

	const CRuMatrix4x4 &inverseWorldTransform = this->GetInverseWorldTransform();

	// Get the ray in local coordinates
	CRuRay3 localRay0, localRay1;

	inverseWorldTransform.TransformPoint(m_activeGizmoPickRay.Origin(), localRay0.Origin());
	inverseWorldTransform.TransformVector(m_activeGizmoPickRay.Direction(), localRay0.Direction());
	localRay0.Direction().Normalize();

	inverseWorldTransform.TransformPoint(pickRay.Origin(), localRay1.Origin());
	inverseWorldTransform.TransformVector(pickRay.Direction(), localRay1.Direction());
	localRay1.Direction().Normalize();

	// Temp variables
	CRuVector3 oldPosition, newPosition;
	float oldT, newT;

	// Constrain ray intersection to the XZ plane
	CRuPlane4 movementPlane(CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 0.0f));

	// if the pick ray is almost perpendicular to the constraining plane, abort
	if(fabs(DotProduct(localRay1.Direction(), CRuVector3(0.0f, 1.0f, 0.0f))) <= stopThreshold)
		return FALSE;

	if(	FindIntersection(localRay0, movementPlane, oldPosition, oldT) &&
		FindIntersection(localRay1, movementPlane, newPosition, newT))
	{
		// Constrain movement vector to the XZ plane
		vectorOut.Set(newPosition.x - oldPosition.x, 0.0f, newPosition.z - oldPosition.z);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
