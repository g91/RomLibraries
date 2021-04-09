/*!
	@file RuEntityGizmo.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Scene/Widget/RuEntityGizmo.h"

#include "../../Engine/Geometry/RuPrimitiveToolkit.h"
#include "../../Scene/Base/RuEntityContainer.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Gizmo, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Gizmo, "CRuEntity_Gizmo", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuEntity_Gizmo, 0x0369E091, 0x53784c7b, 0x8B127052, 0xB4C1A6FB)

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Gizmo_Translation, CRuEntity_Gizmo)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Gizmo_Translation, "CRuEntity_Gizmo_Translation", "CRuEntity_Gizmo")
ruCLASSGUID_DEFINE(CRuEntity_Gizmo_Translation, 0x95CF1802, 0xCBC14b66, 0x84E5BBB0, 0xFCAB05D8)

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Gizmo_Rotation, CRuEntity_Gizmo)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Gizmo_Rotation, "CRuEntity_Gizmo_Rotation", "CRuEntity_Gizmo")
ruCLASSGUID_DEFINE(CRuEntity_Gizmo_Rotation, 0xA947F658, 0xD65E4a5d, 0x85F645AE, 0x43580A91)

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Gizmo_Scaling, CRuEntity_Gizmo)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Gizmo_Scaling, "CRuEntity_Gizmo_Scaling", "CRuEntity_Gizmo")
ruCLASSGUID_DEFINE(CRuEntity_Gizmo_Scaling, 0xD807AC6E, 0x4FF94817, 0x92261F3E, 0x288EAB12)

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Gizmo_Height, CRuEntity_Gizmo)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Gizmo_Height, "CRuEntity_Gizmo_Height", "CRuEntity_Gizmo")
ruCLASSGUID_DEFINE(CRuEntity_Gizmo_Height, 0x66C96916, 0x86644f3e, 0x85440A81, 0x5D8AE499)

// ************************************************************************************************************************************************************

CRuEntity_Gizmo::CRuEntity_Gizmo()
:	m_gizmoSize(1.0f),
	m_frameType(ruFRAMETYPE_LOCAL),
	m_camera(NULL)
{
}

CRuEntity_Gizmo::~CRuEntity_Gizmo()
{
	// Decrement reference count of the camera
	ruSAFE_RELEASE(m_camera);
}

BOOL CRuEntity_Gizmo::Update(float elapsedTime)
{
	// Disable distance fade
	SetOption(ruENTITYOPTION_ENABLEDISTANCEFADE, FALSE);
	SetOption(ruENTITYOPTION_HIDDENBYDISTANCE, FALSE);

	if(m_camera)
	{
		// If the control frame is in world space, counter rotation of the parent frame
		if(m_frameType == ruFRAMETYPE_WORLD && this->GetParent())
		{
			CRuQuaternion invRotQuat = this->GetParent()->GetRotation(ruFRAMETYPE_WORLD);
			invRotQuat.m_x *= -1.0f;
			invRotQuat.m_y *= -1.0f;
			invRotQuat.m_z *= -1.0f;
			this->Rotate(invRotQuat, ruTRANSFORMOP_SET);
		}

		// Calculate scale factor to use on the gizmo based on its world position and the requested size
		CRuVector3 gizmoPosition = this->GetTranslation(ruFRAMETYPE_WORLD);
		REAL scale = m_camera->Utility_GetScreenToWorldScale(gizmoPosition);

		// Scale gizmo
		m_gizmoScale = m_gizmoSize / scale;
		m_gizmoScale /= this->GetParent()->GetScale(ruFRAMETYPE_WORLD);
		this->Scale(m_gizmoScale, ruTRANSFORMOP_SET);
	}

	// Set local entity bounds
	this->SetLocalEntityBounds(CRuSphere(CRuVector3(0.0f, 0.0f, 0.0f), 0.1f));

	return CRuEntity::Update(elapsedTime);
}

INT32 CRuEntity_Gizmo::GetNumMeshes() const
{
	return 0;
}

BOOL CRuEntity_Gizmo::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	return FALSE;
}

void CRuEntity_Gizmo::SetGizmoSize(float gizmoSize)
{
	// Store gizmo size
	m_gizmoSize = gizmoSize;
}

void CRuEntity_Gizmo::SetGizmoTarget(CRuEntity *targetEntity)
{
	// Detach gizmo from previous target entity
	this->DetachFromParent();

	// Attach gizmo to target entity
	targetEntity->AddChild(this);
}

void CRuEntity_Gizmo::SetCamera(CRuCamera *viewCamera)
{
	if(viewCamera)
	{
		// Store reference to new camera
		viewCamera->AddRef();
		ruSAFE_RELEASE(m_camera);
		m_camera = viewCamera;
	}
}

// ************************************************************************************************************************************************************

CRuEntity_Gizmo_Translation::CRuEntity_Gizmo_Translation()
:	m_arrowLength(0.3f),
	m_arrowRadius(0.075f),
	m_shaftOffset(0.4f),
	m_shaftLength(0.4f),
	m_shaftRadius(0.02f),
	m_quadSize(0.3f)
{
	// Default frame type is world frame
	m_frameType = ruFRAMETYPE_WORLD;

	INT32 totalVertexCount = CRuPrimitiveToolkit::Cone_VertexCount(16) * 3 + CRuPrimitiveToolkit::Cylinder_VertexCount(16) * 3 + CRuPrimitiveToolkit::Quad_VertexCount() * 3;
	INT32 totalPrimitiveCount = CRuPrimitiveToolkit::Cone_PrimitiveCount(16) * 3 + CRuPrimitiveToolkit::Cylinder_PrimitiveCount(16) * 3 + CRuPrimitiveToolkit::Quad_PrimitiveCount() * 3;

	// Build new mesh for the gizmo
	m_gizmoMesh = ruNEW CRuMesh();
	m_gizmoMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, totalVertexCount, totalPrimitiveCount, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);

	CRuVector3 *position = static_cast<CRuMorphTarget *>(m_gizmoMesh->GetMorphTarget(0))->GetPosition();
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_gizmoMesh->GetMorphTarget(0))->GetDiffuse();
	UINT16 *indices = m_gizmoMesh->GetIndices();

	INT32 firstVertex = 0;
	INT32 vertexOffset = 0;
	INT32 indexOffset = 0;

	CRuMatrix4x4 transformMatrix;

	// X-axis arrow head
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Cone_Build(16, vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_arrowRadius, m_arrowLength, m_arrowRadius);
	transformMatrix.ChangeBasis(CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));
	transformMatrix.Translate(CRuVector3(1.0f - m_arrowLength, 0.0f, 0.0f));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0xFFFF0000;
	}

	// Y-axis arrow head
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Cone_Build(16, vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_arrowRadius, m_arrowLength, m_arrowRadius);
	transformMatrix.ChangeBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));
	transformMatrix.Translate(CRuVector3(0.0f, 1.0f - m_arrowLength, 0.0f));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0xFF00FF00;
	}

	// Z-axis arrow head
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Cone_Build(16, vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_arrowRadius, m_arrowLength, m_arrowRadius);
	transformMatrix.ChangeBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 1.0f, 0.0f));
	transformMatrix.Translate(CRuVector3(0.0f, 0.0f, 1.0f - m_arrowLength));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0xFF0000FF;
	}

	// X-axis arrow shaft
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Cylinder_Build(16, vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_shaftRadius, m_shaftLength, m_shaftRadius);
	transformMatrix.ChangeBasis(CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));
	transformMatrix.Translate(CRuVector3(m_shaftOffset, 0.0f, 0.0f));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0xFFFF0000;
	}

	// Y-axis arrow shaft
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Cylinder_Build(16, vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_shaftRadius, m_shaftLength, m_shaftRadius);
	transformMatrix.ChangeBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));
	transformMatrix.Translate(CRuVector3(0.0f, m_shaftOffset, 0.0f));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0xFF00FF00;
	}

	// Z-axis arrow shaft
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Cylinder_Build(16, vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_shaftRadius, m_shaftLength, m_shaftRadius);
	transformMatrix.ChangeBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 1.0f, 0.0f));
	transformMatrix.Translate(CRuVector3(0.0f, 0.0f, m_shaftOffset));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0xFF0000FF;
	}

	// XY-plane quad
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Quad_Build(vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_quadSize, 1.0f, m_quadSize);
	transformMatrix.ChangeBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 1.0f, 0.0f));
	transformMatrix.Translate(CRuVector3(m_quadSize / 2.0f, m_quadSize / 2.0f, 0.0f));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0x70FFFF00;
	}

	// XZ-plane quad
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Quad_Build(vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_quadSize, 1.0f, m_quadSize);
	transformMatrix.ChangeBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));
	transformMatrix.Translate(CRuVector3(m_quadSize / 2.0f, 0.0f, m_quadSize / 2.0f));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0x70FF00FF;
	}

	// YZ-plane quad
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Quad_Build(vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_quadSize, 1.0f, m_quadSize);
	transformMatrix.ChangeBasis(CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));
	transformMatrix.Translate(CRuVector3(0.0f, m_quadSize / 2.0f, m_quadSize / 2.0f));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0x7000FFFF;
	}

	// Build new material for the gizmo
	m_gizmoMaterial = ruNEW CRuMaterial();

	m_gizmoMaterial->SetProperty("material:materialType", "Standard Material");

	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_2SIDED, TRUE);
	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_TRANSLUCENT, TRUE);

	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_NONE);
	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 0);

	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATIONCOLOR, RUCOLOR_ARGB(0, 255, 255, 255));
	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(1.0f));

	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ZTESTMODE, ruZTESTMODE_ALWAYS);
	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ZWRITEMODE, ruZWRITEMODE_DISABLE);
}

CRuEntity_Gizmo_Translation::~CRuEntity_Gizmo_Translation()
{
	ruSAFE_RELEASE(m_gizmoMesh);
	ruSAFE_RELEASE(m_gizmoMaterial);
}

BOOL CRuEntity_Gizmo_Translation::Update(float elapsedTime)
{
	// Call base class
	CRuEntity_Gizmo::Update(elapsedTime);

	// Bounding box for X, Y, and Z axis
	float bbShaftSize = max(m_shaftRadius, m_arrowRadius);
	m_gizmoBounds[ruGIZMOPART_X].SetBounds(m_shaftOffset, -bbShaftSize, -bbShaftSize, 1.0f, bbShaftSize, bbShaftSize);
	m_gizmoBounds[ruGIZMOPART_Y].SetBounds(-bbShaftSize, m_shaftOffset, -bbShaftSize, bbShaftSize, 1.0f, bbShaftSize);
	m_gizmoBounds[ruGIZMOPART_Z].SetBounds(-bbShaftSize, -bbShaftSize, m_shaftOffset, bbShaftSize, bbShaftSize, 1.0f);

	// Bounding box for XY, XZ, and YZ planes
	m_gizmoBounds[ruGIZMOPART_XY].SetBounds(0.0f, 0.0f, 0.0f, m_quadSize, m_quadSize, 0.0f);
	m_gizmoBounds[ruGIZMOPART_XZ].SetBounds(0.0f, 0.0f, 0.0f, m_quadSize, 0.0f, m_quadSize);
	m_gizmoBounds[ruGIZMOPART_YZ].SetBounds(0.0f, 0.0f, 0.0f, 0.0f, m_quadSize, m_quadSize);

	return TRUE;
}

BOOL CRuEntity_Gizmo_Translation::QueryCollision(CRuCollisionQuery *colQuery)
{
	switch(colQuery->m_queryType)
	{
		case ruCOLQUERY_RAY:
			{
				CRuCollisionQuery_Ray *rayQuery = static_cast<CRuCollisionQuery_Ray *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 localRay(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));
				
				// Test collision with bounding boxes
				for(INT32 i = 0; i < ruGIZMOPART_COUNT; ++i)
				{
					CRuVector3 intPt;
					float intT;

					if(FindIntersection(localRay, m_gizmoBounds[i], intPt, intT))
					{
						rayQuery->m_colPoints->Add(intPt);
						rayQuery->m_colDistances->Add(intT);
					}
				}

				return TRUE;
			}

			break;
	}

	return TRUE;
}

INT32 CRuEntity_Gizmo_Translation::GetNumMeshes() const
{
	return 1;
}

BOOL CRuEntity_Gizmo_Translation::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex == 0)
	{
		*meshOut = m_gizmoMesh;
		*materialOut = m_gizmoMaterial;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_Gizmo_Translation::ActivateManipulator(const CRuRay3 &pickRay)
{
	const CRuMatrix4x4 &inverseWorldTransform = this->GetInverseWorldTransform();

	// Get the ray in local coordinates
	CRuRay3 localRay;
	inverseWorldTransform.TransformPoint(pickRay.Origin(), localRay.Origin());
	inverseWorldTransform.TransformVector(pickRay.Direction(), localRay.Direction());

	// Test collision with bounding boxes
	INT32 nearestPart = 0;
	float nearestT = FLT_MAX;
	for(INT32 i = 0; i < ruGIZMOPART_COUNT; ++i)
	{
		CRuVector3 intPt;
		float intT;

		if(FindIntersection(localRay, m_gizmoBounds[i], intPt, intT))
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
		m_activeGizmoPart = static_cast<RuGizmoPart>(nearestPart);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_Gizmo_Translation::Manipulate(const CRuRay3 &pickRay)
{
	CRuVector3 movementVector;

	if(GetManipulationVector(pickRay, movementVector))
	{
		// Update pick ray
		m_activeGizmoPickRay = pickRay;

		// Update target position
		this->GetParent()->Translate(movementVector);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_Gizmo_Translation::GetManipulationVector(const CRuRay3 &pickRay, CRuVector3 &vectorOut)
{
	float stopThreshold = 0.05f;

	const CRuMatrix4x4 &inverseWorldTransform = this->GetInverseWorldTransform();

	// Get the ray in local coordinates
	CRuRay3 localRay0, localRay1;

	inverseWorldTransform.TransformPoint(m_activeGizmoPickRay.Origin(), localRay0.Origin());
	inverseWorldTransform.TransformVector(m_activeGizmoPickRay.Direction(), localRay0.Direction());
	localRay0.Direction().Normalize();

	inverseWorldTransform.TransformPoint(pickRay.Origin(), localRay1.Origin());
	inverseWorldTransform.TransformVector(pickRay.Direction(), localRay1.Direction());
	localRay1.Direction().Normalize();

	CRuVector3 gizmoPosition = CRuVector3(0.0f, 0.0f, 0.0f);

	// Temp variables
	CRuVector3 oldPosition, newPosition;
	float oldT, newT;

	// Reset output vector
	vectorOut.Set(0.0f, 0.0f, 0.0f);

	switch(m_activeGizmoPart)
	{
		case ruGIZMOPART_X:
			{
				// Constrain ray intersection to the XZ plane
				CRuPlane4 movementPlane(CRuVector3(0.0f, 1.0f, 0.0f), gizmoPosition);

				// if the pick ray is almost perpendicular to the constraining plane, use alternate plane
				if(fabs(DotProduct(localRay1.Direction(), CRuVector3(0.0f, 1.0f, 0.0f))) <= stopThreshold)
					movementPlane = CRuPlane4(CRuVector3(0.0f, 0.0f, 1.0f), gizmoPosition);

				if(	FindIntersection(localRay0, movementPlane, oldPosition, oldT) &&
					FindIntersection(localRay1, movementPlane, newPosition, newT))
				{
					// Constrain movement vector to the X axis
					vectorOut.Set(newPosition.x - oldPosition.x, 0.0f, 0.0f);
				}
			}

			break;

		case ruGIZMOPART_Y:
			{
				// Constrain ray intersection to the YZ plane
				CRuPlane4 movementPlane(CRuVector3(1.0f, 0.0f, 0.0f), gizmoPosition);

				// if the pick ray is almost perpendicular to the constraining plane, use alternate plane
				if(fabs(DotProduct(localRay1.Direction(), CRuVector3(1.0f, 0.0f, 0.0f))) <= stopThreshold)
					movementPlane = CRuPlane4(CRuVector3(0.0f, 0.0f, 1.0f), gizmoPosition);

				if(	FindIntersection(localRay0, movementPlane, oldPosition, oldT) &&
					FindIntersection(localRay1, movementPlane, newPosition, newT))
				{
					// Constrain movement vector to the Y axis
					vectorOut.Set(0.0f, newPosition.y - oldPosition.y, 0.0f);
				}
			}

			break;

		case ruGIZMOPART_Z:
			{
				// Constrain ray intersection to the XZ plane
				CRuPlane4 movementPlane(CRuVector3(0.0f, 1.0f, 0.0f), gizmoPosition);

				// if the pick ray is almost perpendicular to the constraining plane, use alternate plane
				if(fabs(DotProduct(localRay1.Direction(), CRuVector3(0.0f, 1.0f, 0.0f))) <= stopThreshold)
					movementPlane = CRuPlane4(CRuVector3(1.0f, 0.0f, 0.0f), gizmoPosition);

				if(	FindIntersection(localRay0, movementPlane, oldPosition, oldT) &&
					FindIntersection(localRay1, movementPlane, newPosition, newT))
				{
					// Constrain movement vector to the Z axis
					vectorOut.Set(0.0f, 0.0f, newPosition.z - oldPosition.z);
				}
			}

			break;

		case ruGIZMOPART_XY:
			{
				// Constrain ray intersection to the XY plane
				CRuPlane4 movementPlane(CRuVector3(0.0f, 0.0f, 1.0f), gizmoPosition);

				// if the pick ray is almost perpendicular to the constraining plane, abort
				if(fabs(DotProduct(localRay1.Direction(), CRuVector3(0.0f, 0.0f, 1.0f))) <= stopThreshold)
					return FALSE;

				if(	FindIntersection(localRay0, movementPlane, oldPosition, oldT) &&
					FindIntersection(localRay1, movementPlane, newPosition, newT))
				{
					// Constrain movement vector to the XY plane
					vectorOut.Set(newPosition.x - oldPosition.x, newPosition.y - oldPosition.y, 0.0f);
				}
			}

			break;

		case ruGIZMOPART_XZ:
			{
				// Constrain ray intersection to the XZ plane
				CRuPlane4 movementPlane(CRuVector3(0.0f, 1.0f, 0.0f), gizmoPosition);

				// if the pick ray is almost perpendicular to the constraining plane, abort
				if(fabs(DotProduct(localRay1.Direction(), CRuVector3(0.0f, 1.0f, 0.0f))) <= stopThreshold)
					return FALSE;

				if(	FindIntersection(localRay0, movementPlane, oldPosition, oldT) &&
					FindIntersection(localRay1, movementPlane, newPosition, newT))
				{
					// Constrain movement vector to the XZ plane
					vectorOut.Set(newPosition.x - oldPosition.x, 0.0f, newPosition.z - oldPosition.z);
				}
			}

			break;

		case ruGIZMOPART_YZ:
			{
				// Constrain ray intersection to the YZ plane
				CRuPlane4 movementPlane(CRuVector3(1.0f, 0.0f, 0.0f), gizmoPosition);

				// if the pick ray is almost perpendicular to the constraining plane, abort
				if(fabs(DotProduct(localRay1.Direction(), CRuVector3(1.0f, 0.0f, 0.0f))) <= stopThreshold)
					return FALSE;

				if(	FindIntersection(localRay0, movementPlane, oldPosition, oldT) &&
					FindIntersection(localRay1, movementPlane, newPosition, newT))
				{
					// Constrain movement vector to the YZ plane
					vectorOut.Set(0.0f, newPosition.y - oldPosition.y, newPosition.z - oldPosition.z);
				}
			}

			break;
	}

	// Rescale by the scale factor
	this->GetLocalTransform().TransformVector(vectorOut);

	// Transform translation into target local space
	this->GetParent()->GetLocalTransform().TransformVector(vectorOut);

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuEntity_Gizmo_Rotation::CRuEntity_Gizmo_Rotation()
:	m_torusRadius1(1.0f),
	m_torusRadius2(0.015f),
	m_torusSegments(16),
	m_torusSides(8)
{
	INT32 totalVertexCount = CRuPrimitiveToolkit::Torus_VertexCount(m_torusSegments, m_torusSides) * 3 + CRuPrimitiveToolkit::Cylinder_VertexCount(16) * 3;
	INT32 totalPrimitiveCount = CRuPrimitiveToolkit::Torus_PrimitiveCount(m_torusSegments, m_torusSides) * 3 + CRuPrimitiveToolkit::Cylinder_PrimitiveCount(16) * 3;

	// Build new mesh for the gizmo
	m_gizmoMesh = ruNEW CRuMesh();
	m_gizmoMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, totalVertexCount, totalPrimitiveCount, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);

	CRuVector3 *position = static_cast<CRuMorphTarget *>(m_gizmoMesh->GetMorphTarget(0))->GetPosition();
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_gizmoMesh->GetMorphTarget(0))->GetDiffuse();
	UINT16 *indices = m_gizmoMesh->GetIndices();

	INT32 firstVertex = 0;
	INT32 vertexOffset = 0;
	INT32 indexOffset = 0;

	CRuMatrix4x4 transformMatrix;

	// X-axis torus
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Torus_Build(m_torusSegments, m_torusSides, m_torusRadius2, vertexOffset, indexOffset, position, indices);

	transformMatrix.SetToScaling(m_torusRadius1, m_torusRadius1, m_torusRadius1);
	transformMatrix.ChangeBasis(CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));

	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0xFFFF0000;
	}

	// Y-axis torus
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Torus_Build(m_torusSegments, m_torusSides, m_torusRadius2, vertexOffset, indexOffset, position, indices);

	transformMatrix.SetToScaling(m_torusRadius1, m_torusRadius1, m_torusRadius1);
	transformMatrix.ChangeBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));

	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0xFF00FF00;
	}

	// Z-axis torus
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Torus_Build(m_torusSegments, m_torusSides, m_torusRadius2, vertexOffset, indexOffset, position, indices);

	transformMatrix.SetToScaling(m_torusRadius1, m_torusRadius1, m_torusRadius1);
	transformMatrix.ChangeBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 1.0f, 0.0f));

	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0xFF0000FF;
	}

	// X-axis shaft
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Cylinder_Build(16, vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_torusRadius2, m_torusRadius1 * 2, m_torusRadius2);
	transformMatrix.ChangeBasis(CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));
	transformMatrix.Translate(CRuVector3(-m_torusRadius1, 0.0f, 0.0f));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0x70FFFFFF;
	}

	// Y-axis shaft
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Cylinder_Build(16, vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_torusRadius2, m_torusRadius1 * 2, m_torusRadius2);
	transformMatrix.ChangeBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));
	transformMatrix.Translate(CRuVector3(0.0f, -m_torusRadius1, 0.0f));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0x70FFFFFF;
	}

	// Z-axis shaft
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Cylinder_Build(16, vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_torusRadius2, m_torusRadius1 * 2, m_torusRadius2);
	transformMatrix.ChangeBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 1.0f, 0.0f));
	transformMatrix.Translate(CRuVector3(0.0f, 0.0f, -m_torusRadius1));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0x70FFFFFF;
	}

	// Build new material for the gizmo
	m_gizmoMaterial = ruNEW CRuMaterial();

	m_gizmoMaterial->SetProperty("material:materialType", "Standard Material");

	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_2SIDED, TRUE);
	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_TRANSLUCENT, TRUE);

	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_NONE);
	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 0);

	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATIONCOLOR, RUCOLOR_ARGB(0, 255, 255, 255));
	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(1.0f));

	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ZTESTMODE, ruZTESTMODE_ALWAYS);
	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ZWRITEMODE, ruZWRITEMODE_DISABLE);
}

CRuEntity_Gizmo_Rotation::~CRuEntity_Gizmo_Rotation()
{
	ruSAFE_RELEASE(m_gizmoMesh);
	ruSAFE_RELEASE(m_gizmoMaterial);
}

BOOL CRuEntity_Gizmo_Rotation::Update(float elapsedTime)
{
	// Call base class
	CRuEntity_Gizmo::Update(elapsedTime);

	m_gizmoPlanes[ruROTGIZMOPART_X].SetPlane(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 0.0f));
	m_gizmoPlanes[ruROTGIZMOPART_Y].SetPlane(CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 0.0f));
	m_gizmoPlanes[ruROTGIZMOPART_Z].SetPlane(CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 0.0f, 0.0f));

	return TRUE;
}

BOOL CRuEntity_Gizmo_Rotation::QueryCollision(CRuCollisionQuery *colQuery)
{
	return TRUE;
}

INT32 CRuEntity_Gizmo_Rotation::GetNumMeshes() const
{
	return 1;
}

BOOL CRuEntity_Gizmo_Rotation::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex == 0)
	{
		*meshOut = m_gizmoMesh;
		*materialOut = m_gizmoMaterial;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_Gizmo_Rotation::ActivateManipulator(const CRuRay3 &pickRay)
{
	REAL errorThreshold = 0.2f;
	REAL innerRadius = m_torusRadius1 - m_torusRadius2 - errorThreshold;
	REAL outerRadius = m_torusRadius1;

	const CRuMatrix4x4 &inverseWorldTransform = this->GetInverseWorldTransform();

	// Get the ray in local coordinates
	CRuRay3 localRay;
	inverseWorldTransform.TransformPoint(pickRay.Origin(), localRay.Origin());
	inverseWorldTransform.TransformVector(pickRay.Direction(), localRay.Direction());
	localRay.Direction().Normalize();

	// Test collision with manipulator planes
	INT32 nearestPart = 0;
	REAL nearestT = FLT_MAX;
	for(INT32 i = 0; i < ruROTGIZMOPART_COUNT; ++i)
	{
		CRuVector3 intPt;
		float intT;

		if(FindIntersection(localRay, m_gizmoPlanes[i], intPt, intT))
		{
			REAL intRadius = intPt .Magnitude();

			// Make sure we're in the circle of the disc
			if(intRadius >= innerRadius && intRadius <= outerRadius)
			{
				if(intT < nearestT)
				{
					nearestPart = i;
					nearestT = intT;
				}
			}
		}
	}

	// If we found a suitable gizmo manipulator to activate, save it and return TRUE
	if(nearestT < FLT_MAX)
	{
		m_activeGizmoPickRay = pickRay;
		m_activeGizmoPart = static_cast<RuRotationGizmoPart>(nearestPart);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_Gizmo_Rotation::Manipulate(const CRuRay3 &pickRay)
{
	CRuQuaternion rotQuat;

	if(GetManipulationRotation(pickRay, rotQuat))
	{
		// Update pick ray
		m_activeGizmoPickRay = pickRay;

		// Decompose orientation transform
		CRuVector3 scale, shear, rotation, translation;
		CRuQuaternion rotationQuat;

		CRuVector3 oldTranslation = this->GetParent()->GetTranslation(ruFRAMETYPE_LOCAL);

		this->GetParent()->Translate(CRuVector3(0.0f, 0.0f, 0.0f), ruTRANSFORMOP_SET);
		this->GetParent()->Rotate(rotQuat);
		this->GetParent()->Translate(oldTranslation, ruTRANSFORMOP_SET);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_Gizmo_Rotation::GetManipulationRotation(const CRuRay3 &pickRay, CRuQuaternion &rotationOut)
{
	// Temp variables
	CRuVector3 oldPosition, newPosition;
	float oldT, newT;

	const CRuMatrix4x4 &inverseWorldTransform = this->GetInverseWorldTransform();

	// Get the ray in local coordinates
	CRuRay3 localRay0, localRay1;

	inverseWorldTransform.TransformPoint(m_activeGizmoPickRay.Origin(), localRay0.Origin());
	inverseWorldTransform.TransformVector(m_activeGizmoPickRay.Direction(), localRay0.Direction());
	localRay0.Direction().Normalize();

	inverseWorldTransform.TransformPoint(pickRay.Origin(), localRay1.Origin());
	inverseWorldTransform.TransformVector(pickRay.Direction(), localRay1.Direction());
	localRay1.Direction().Normalize();

	if(	FindIntersection(localRay0, m_gizmoPlanes[m_activeGizmoPart], oldPosition, oldT) &&
		FindIntersection(localRay1, m_gizmoPlanes[m_activeGizmoPart], newPosition, newT))
	{
		CRuVector3 oldVec = oldPosition.Normalize();
		CRuVector3 newVec = newPosition.Normalize();

		rotationOut.RotateIntoVector(newVec, oldVec);

		// Convert quaternion into angle-axis form
		CRuVector3 axis;
		REAL angle;
		rotationOut.ToAngleAxis(axis, angle);

		// Transform axis into target local space
		this->GetParent()->GetLocalTransform().TransformVector(axis);
		axis.Normalize();

		// Convert back to quaternion form
		rotationOut.FromAngleAxis(axis, angle);

		return TRUE;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

CRuEntity_Gizmo_Scaling::CRuEntity_Gizmo_Scaling()
:	m_arrowLength(0.3f),
	m_arrowRadius(0.075f),
	m_shaftOffset(0.4f),
	m_shaftLength(0.4f),
	m_shaftRadius(0.02f),
	m_quadSize(0.3f)
{
	INT32 totalVertexCount = CRuPrimitiveToolkit::Cone_VertexCount(16) * 3 + CRuPrimitiveToolkit::Cylinder_VertexCount(16) * 3 + CRuPrimitiveToolkit::Quad_VertexCount() * 3;
	INT32 totalPrimitiveCount = CRuPrimitiveToolkit::Cone_PrimitiveCount(16) * 3 + CRuPrimitiveToolkit::Cylinder_PrimitiveCount(16) * 3 + CRuPrimitiveToolkit::Quad_PrimitiveCount() * 3;

	// Build new mesh for the gizmo
	m_gizmoMesh = ruNEW CRuMesh();
	m_gizmoMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, totalVertexCount, totalPrimitiveCount, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);

	CRuVector3 *position = static_cast<CRuMorphTarget *>(m_gizmoMesh->GetMorphTarget(0))->GetPosition();
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_gizmoMesh->GetMorphTarget(0))->GetDiffuse();
	UINT16 *indices = m_gizmoMesh->GetIndices();

	INT32 firstVertex = 0;
	INT32 vertexOffset = 0;
	INT32 indexOffset = 0;

	CRuMatrix4x4 transformMatrix;

	// X-axis arrow head
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Cone_Build(16, vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_arrowRadius, m_arrowLength, m_arrowRadius);
	transformMatrix.ChangeBasis(CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));
	transformMatrix.Translate(CRuVector3(1.0f - m_arrowLength, 0.0f, 0.0f));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0xFFFF0000;
	}

	// Y-axis arrow head
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Cone_Build(16, vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_arrowRadius, m_arrowLength, m_arrowRadius);
	transformMatrix.ChangeBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));
	transformMatrix.Translate(CRuVector3(0.0f, 1.0f - m_arrowLength, 0.0f));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0xFF00FF00;
	}

	// Z-axis arrow head
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Cone_Build(16, vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_arrowRadius, m_arrowLength, m_arrowRadius);
	transformMatrix.ChangeBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 1.0f, 0.0f));
	transformMatrix.Translate(CRuVector3(0.0f, 0.0f, 1.0f - m_arrowLength));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0xFF0000FF;
	}

	// X-axis arrow shaft
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Cylinder_Build(16, vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_shaftRadius, m_shaftLength, m_shaftRadius);
	transformMatrix.ChangeBasis(CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));
	transformMatrix.Translate(CRuVector3(m_shaftOffset, 0.0f, 0.0f));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0xFFFF0000;
	}

	// Y-axis arrow shaft
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Cylinder_Build(16, vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_shaftRadius, m_shaftLength, m_shaftRadius);
	transformMatrix.ChangeBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));
	transformMatrix.Translate(CRuVector3(0.0f, m_shaftOffset, 0.0f));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0xFF00FF00;
	}

	// Z-axis arrow shaft
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Cylinder_Build(16, vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_shaftRadius, m_shaftLength, m_shaftRadius);
	transformMatrix.ChangeBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 1.0f, 0.0f));
	transformMatrix.Translate(CRuVector3(0.0f, 0.0f, m_shaftOffset));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0xFF0000FF;
	}

	// XY-plane quad
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Quad_Build(vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_quadSize, 1.0f, m_quadSize);
	transformMatrix.ChangeBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 1.0f, 0.0f));
	transformMatrix.Translate(CRuVector3(m_quadSize / 2.0f, m_quadSize / 2.0f, 0.0f));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0x70FFFF00;
	}

	// XZ-plane quad
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Quad_Build(vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_quadSize, 1.0f, m_quadSize);
	transformMatrix.ChangeBasis(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));
	transformMatrix.Translate(CRuVector3(m_quadSize / 2.0f, 0.0f, m_quadSize / 2.0f));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0x70FF00FF;
	}

	// YZ-plane quad
	firstVertex = vertexOffset;
	CRuPrimitiveToolkit::Quad_Build(vertexOffset, indexOffset, position, indices);
	transformMatrix.SetToScaling(m_quadSize, 1.0f, m_quadSize);
	transformMatrix.ChangeBasis(CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));
	transformMatrix.Translate(CRuVector3(0.0f, m_quadSize / 2.0f, m_quadSize / 2.0f));
	for(INT32 i = firstVertex; i < vertexOffset; ++i)
	{
		transformMatrix.TransformPoint(position[i]);
		diffuse[i] = 0x7000FFFF;
	}

	// Build new material for the gizmo
	m_gizmoMaterial = ruNEW CRuMaterial();

	m_gizmoMaterial->SetProperty("material:materialType", "Standard Material");

	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_2SIDED, TRUE);
	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_TRANSLUCENT, TRUE);

	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_NONE);
	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 0);

	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATIONCOLOR, RUCOLOR_ARGB(0, 255, 255, 255));
	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(1.0f));

	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ZTESTMODE, ruZTESTMODE_ALWAYS);
	m_gizmoMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ZWRITEMODE, ruZWRITEMODE_DISABLE);
}

CRuEntity_Gizmo_Scaling::~CRuEntity_Gizmo_Scaling()
{
	ruSAFE_RELEASE(m_gizmoMesh);
	ruSAFE_RELEASE(m_gizmoMaterial);
}

BOOL CRuEntity_Gizmo_Scaling::Update(float elapsedTime)
{
	// Call base class
	CRuEntity_Gizmo::Update(elapsedTime);

	// Bounding box for X, Y, and Z axis
	float bbShaftSize = max(m_shaftRadius, m_arrowRadius);
	m_gizmoBounds[ruSCALEGIZMOPART_X].SetBounds(m_shaftOffset, -bbShaftSize, -bbShaftSize, 1.0f, bbShaftSize, bbShaftSize);
	m_gizmoBounds[ruSCALEGIZMOPART_Y].SetBounds(-bbShaftSize, m_shaftOffset, -bbShaftSize, bbShaftSize, 1.0f, bbShaftSize);
	m_gizmoBounds[ruSCALEGIZMOPART_Z].SetBounds(-bbShaftSize, -bbShaftSize, m_shaftOffset, bbShaftSize, bbShaftSize, 1.0f);

	// Bounding box for XY, XZ, and YZ planes
	m_gizmoBounds[ruSCALEGIZMOPART_XY].SetBounds(0.0f, 0.0f, 0.0f, m_quadSize, m_quadSize, 0.0f);
	m_gizmoBounds[ruSCALEGIZMOPART_XZ].SetBounds(0.0f, 0.0f, 0.0f, m_quadSize, 0.0f, m_quadSize);
	m_gizmoBounds[ruSCALEGIZMOPART_YZ].SetBounds(0.0f, 0.0f, 0.0f, 0.0f, m_quadSize, m_quadSize);

	return TRUE;
}

BOOL CRuEntity_Gizmo_Scaling::QueryCollision(CRuCollisionQuery *colQuery)
{
	switch(colQuery->m_queryType)
	{
		case ruCOLQUERY_RAY:
			{
				CRuCollisionQuery_Ray *rayQuery = static_cast<CRuCollisionQuery_Ray *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 localRay(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));
				
				// Test collision with bounding boxes
				for(INT32 i = 0; i < ruSCALEGIZMOPART_COUNT; ++i)
				{
					CRuVector3 intPt;
					float intT;

					if(FindIntersection(localRay, m_gizmoBounds[i], intPt, intT))
					{
						rayQuery->m_colPoints->Add(intPt);
						rayQuery->m_colDistances->Add(intT);
					}
				}

				return TRUE;
			}

			break;
	}

	return TRUE;
}

INT32 CRuEntity_Gizmo_Scaling::GetNumMeshes() const
{
	return 1;
}

BOOL CRuEntity_Gizmo_Scaling::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex == 0)
	{
		*meshOut = m_gizmoMesh;
		*materialOut = m_gizmoMaterial;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_Gizmo_Scaling::ActivateManipulator(const CRuRay3 &pickRay)
{
	CRuMatrix4x4 inverseParentWorldTransform;

	const CRuMatrix4x4 &inverseWorldTransform = this->GetInverseWorldTransform();

	// Get the ray in local coordinates
	CRuRay3 localRay;
	inverseWorldTransform.TransformPoint(pickRay.Origin(), localRay.Origin());
	inverseWorldTransform.TransformVector(pickRay.Direction(), localRay.Direction());

	// Test collision with bounding boxes
	INT32 nearestPart = 0;
	float nearestT = FLT_MAX;
	for(INT32 i = 0; i < ruSCALEGIZMOPART_COUNT; ++i)
	{
		CRuVector3 intPt;
		float intT;

		if(FindIntersection(localRay, m_gizmoBounds[i], intPt, intT))
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
		m_activeGizmoPart = static_cast<RuScalingGizmoPart>(nearestPart);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_Gizmo_Scaling::Manipulate(const CRuRay3 &pickRay)
{
	CRuVector3 scaleVector;

	if(GetManipulationScale(pickRay, scaleVector))
	{
		// Update pick ray
		m_activeGizmoPickRay = pickRay;

		// Scale target entity
		REAL currentScale = this->GetParent()->GetScale(ruFRAMETYPE_LOCAL);
		REAL newScale = currentScale * scaleVector.m_x;

		// Limit scale factor
		newScale = max(0.01f, newScale);

		// Perform scale
		this->GetParent()->Scale(newScale, ruTRANSFORMOP_SET);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_Gizmo_Scaling::GetManipulationScale(const CRuRay3 &pickRay, CRuVector3 &scaleOut)
{
	REAL uniScale = 0.0f;
	float scaleMultiplier = 0.01f;
	float stopThreshold = 0.05f;

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

	CRuVector3 gizmoPosition = CRuVector3(0.0f, 0.0f, 0.0f);

	// Reset output vector
	scaleOut.Set(1.0f, 1.0f, 1.0f);

	switch(m_activeGizmoPart)
	{
		case ruSCALEGIZMOPART_X:
			{
				// Constrain ray intersection to the XZ plane
				CRuPlane4 movementPlane(CRuVector3(0.0f, 1.0f, 0.0f), gizmoPosition);

				// if the pick ray is almost perpendicular to the constraining plane, use alternate plane
				if(fabs(DotProduct(localRay1.Direction(), CRuVector3(0.0f, 1.0f, 0.0f))) <= stopThreshold)
					movementPlane = CRuPlane4(CRuVector3(0.0f, 0.0f, 1.0f), gizmoPosition);

				if(	FindIntersection(localRay0, movementPlane, oldPosition, oldT) &&
					FindIntersection(localRay1, movementPlane, newPosition, newT))
				{
					// Constrain movement vector to the X axis
					scaleOut.Set((newPosition.x - oldPosition.x) * scaleMultiplier + 1.0f, 1.0f, 1.0f);

					// Set uniform scale value
					uniScale = (newPosition.x - oldPosition.x) * scaleMultiplier;
				}
			}

			break;

		case ruSCALEGIZMOPART_Y:
			{
				// Constrain ray intersection to the YZ plane
				CRuPlane4 movementPlane(CRuVector3(1.0f, 0.0f, 0.0f), gizmoPosition);

				// if the pick ray is almost perpendicular to the constraining plane, use alternate plane
				if(fabs(DotProduct(localRay1.Direction(), CRuVector3(1.0f, 0.0f, 0.0f))) <= stopThreshold)
					movementPlane = CRuPlane4(CRuVector3(0.0f, 0.0f, 1.0f), gizmoPosition);

				if(	FindIntersection(localRay0, movementPlane, oldPosition, oldT) &&
					FindIntersection(localRay1, movementPlane, newPosition, newT))
				{
					// Constrain movement vector to the Y axis
					scaleOut.Set(1.0f, (newPosition.y - oldPosition.y) * scaleMultiplier + 1.0f, 1.0f);

					// Set uniform scale value
					uniScale = (newPosition.y - oldPosition.y) * scaleMultiplier;
				}
			}

			break;

		case ruSCALEGIZMOPART_Z:
			{
				// Constrain ray intersection to the XZ plane
				CRuPlane4 movementPlane(CRuVector3(0.0f, 1.0f, 0.0f), gizmoPosition);

				// if the pick ray is almost perpendicular to the constraining plane, use alternate plane
				if(fabs(DotProduct(localRay1.Direction(), CRuVector3(0.0f, 1.0f, 0.0f))) <= stopThreshold)
					movementPlane = CRuPlane4(CRuVector3(1.0f, 0.0f, 0.0f), gizmoPosition);

				if(	FindIntersection(localRay0, movementPlane, oldPosition, oldT) &&
					FindIntersection(localRay1, movementPlane, newPosition, newT))
				{
					// Constrain movement vector to the Z axis
					scaleOut.Set(1.0f, 1.0f, (newPosition.z - oldPosition.z) * scaleMultiplier + 1.0f);

					// Set uniform scale value
					uniScale = (newPosition.z - oldPosition.z) * scaleMultiplier;
				}
			}

			break;

		case ruSCALEGIZMOPART_XY:
			{
				// Constrain ray intersection to the XY plane
				CRuPlane4 movementPlane(CRuVector3(0.0f, 0.0f, 1.0f), gizmoPosition);

				// if the pick ray is almost perpendicular to the constraining plane, abort
				if(fabs(DotProduct(localRay1.Direction(), CRuVector3(0.0f, 0.0f, 1.0f))) <= stopThreshold)
					return FALSE;

				if(	FindIntersection(localRay0, movementPlane, oldPosition, oldT) &&
					FindIntersection(localRay1, movementPlane, newPosition, newT))
				{
					// Constrain movement vector to the XY plane
					scaleOut.Set((newPosition.x - oldPosition.x) * scaleMultiplier + 1.0f, (newPosition.y - oldPosition.y) * scaleMultiplier + 1.0f, 1.0f);

					// Set uniform scale value
					uniScale = (newPosition.x - oldPosition.x) * scaleMultiplier;
				}
			}

			break;

		case ruSCALEGIZMOPART_XZ:
			{
				// Constrain ray intersection to the XZ plane
				CRuPlane4 movementPlane(CRuVector3(0.0f, 1.0f, 0.0f), gizmoPosition);

				// if the pick ray is almost perpendicular to the constraining plane, abort
				if(fabs(DotProduct(localRay1.Direction(), CRuVector3(0.0f, 1.0f, 0.0f))) <= stopThreshold)
					return FALSE;

				if(	FindIntersection(localRay0, movementPlane, oldPosition, oldT) &&
					FindIntersection(localRay1, movementPlane, newPosition, newT))
				{
					// Constrain movement vector to the XZ plane
					scaleOut.Set((newPosition.x - oldPosition.x) * scaleMultiplier + 1.0f, 1.0f, (newPosition.z - oldPosition.z) * scaleMultiplier + 1.0f);

					// Set uniform scale value
					uniScale = (newPosition.x - oldPosition.x) * scaleMultiplier;
				}
			}

			break;

		case ruSCALEGIZMOPART_YZ:
			{
				// Constrain ray intersection to the YZ plane
				CRuPlane4 movementPlane(CRuVector3(1.0f, 0.0f, 0.0f), gizmoPosition);

				// if the pick ray is almost perpendicular to the constraining plane, abort
				if(fabs(DotProduct(localRay1.Direction(), CRuVector3(1.0f, 0.0f, 0.0f))) <= stopThreshold)
					return FALSE;

				if(	FindIntersection(localRay0, movementPlane, oldPosition, oldT) &&
					FindIntersection(localRay1, movementPlane, newPosition, newT))
				{
					// Constrain movement vector to the YZ plane
					scaleOut.Set(1.0f, (newPosition.y - oldPosition.y) * scaleMultiplier + 1.0f, (newPosition.z - oldPosition.z) * scaleMultiplier + 1.0f);

					// Set uniform scale value
					uniScale = (newPosition.y - oldPosition.y) * scaleMultiplier;
				}
			}

			break;
	}

	// Force uniform scale
	uniScale *= this->GetScale(ruFRAMETYPE_LOCAL);
	uniScale += 1.0f;
	scaleOut.Set(uniScale, uniScale, uniScale);

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuEntity_Gizmo_Height::CRuEntity_Gizmo_Height()
:	m_heightDelta(0.0f)
{
}

CRuEntity_Gizmo_Height::~CRuEntity_Gizmo_Height()
{
}

BOOL CRuEntity_Gizmo_Height::ActivateManipulator(const CRuRay3 &pickRay)
{
	m_activeGizmoPickRay = pickRay;

	return TRUE;
}

BOOL CRuEntity_Gizmo_Height::Manipulate(const CRuRay3 &pickRay)
{
	float stopThreshold = 0.05f;

	// Temp variables
	CRuVector3 oldPosition, newPosition;
	float oldT, newT;

	const CRuMatrix4x4 &inverseWorldTransform = this->GetInverseWorldTransform();

	// Get the ray in local coordinates
	CRuRay3 localRay0, localRay1;

	inverseWorldTransform.TransformPoint(m_activeGizmoPickRay.Origin(), localRay0.Origin());
	inverseWorldTransform.TransformVector(m_activeGizmoPickRay.Direction(), localRay0.Direction());
	localRay0.Direction().Normalize();

	inverseWorldTransform.TransformPoint(pickRay.Origin(), localRay1.Origin());
	inverseWorldTransform.TransformVector(pickRay.Direction(), localRay1.Direction());
	localRay1.Direction().Normalize();

	// Gizmo pos in local space at 0, 0, 0
	CRuVector3 gizmoPosition = CRuVector3(0.0f, 0.0f, 0.0f);

	// Constrain ray intersection to the YZ plane
	CRuPlane4 movementPlane(CRuVector3(1.0f, 0.0f, 0.0f), gizmoPosition);

	// if the pick ray is almost perpendicular to the constraining plane, use alternate plane (XY plane)
	if(fabs(DotProduct(pickRay.Direction(), CRuVector3(1.0f, 0.0f, 0.0f))) <= stopThreshold)
		movementPlane = CRuPlane4(CRuVector3(0.0f, 0.0f, 1.0f), gizmoPosition);

	if(	FindIntersection(localRay0, movementPlane, oldPosition, oldT) &&
		FindIntersection(localRay1, movementPlane, newPosition, newT))
	{
		// Constrain movement vector to the Y axis
		CRuVector3 movementVector(0.0f, newPosition.y - oldPosition.y, 0.0f);

		// Update pick ray
		m_activeGizmoPickRay = pickRay;

		m_heightDelta += movementVector.m_y;

		this->GetParent()->Translate(movementVector);

		return TRUE;
	}

	return FALSE;
}

float CRuEntity_Gizmo_Height::GetDelta()
{
	float retVal = m_heightDelta;

	m_heightDelta = 0.0f;

	return retVal;
}

// ************************************************************************************************************************************************************

#pragma managed
