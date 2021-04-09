/*!
	@file RuGizmo_Box.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Scene/Gizmo/RuGizmo_Box.h"

#include "../../Helper/RuHelper_StreamIO.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuGizmo_Box, CRuGizmo_Base)
ruHEAP_DEFINE_SUBCLASS(CRuGizmo_Box, "CRuGizmo_Box", "CRuGizmo_Base")
ruCLASSGUID_DEFINE(CRuGizmo_Box, 0x6F33E772, 0xEE584b87, 0xA4EF36D5, 0x4138DB87)

// ************************************************************************************************************************************************************

CRuGizmo_Box::CRuGizmo_Box()
:	m_mesh(NULL),
	m_material(NULL),
	m_boxMesh(NULL),
	m_boxMaterial(NULL),
	m_gizmoConstraint(ruGIZMOCONSTRAINT_XZ),
	m_activeControlCylinder(-1),
	m_highlightControlCylinder(-1)
{
	m_boxColor = 0xFFFFFFFF;

	m_box.Center().Set(0.0f, 0.0f, 0.0f);
	m_box.Axis()[0].Set(1.0f, 0.0f, 0.0f);
	m_box.Axis()[1].Set(0.0f, 1.0f, 0.0f);
	m_box.Axis()[2].Set(0.0f, 0.0f, 1.0f);
	m_box.Extents()[0] = 1.0f;
	m_box.Extents()[1] = 1.0f;
	m_box.Extents()[2] = 1.0f;

	if(m_mesh == NULL)
	{
		m_mesh = ruNEW CRuDynamicMesh();
		m_mesh->Initialize(ruPRIMTYPE_LINELIST, 24, 12, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);

		m_mesh->SetNumPrimitives(12);
		m_mesh->SetNumVertices(24);
	}

	if(m_material == NULL)
	{
		m_material = ruNEW CRuMaterial();

		m_material->SetProperty("material:materialType", "Standard Material");

		m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_TRANSLUCENT, TRUE);

		m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_NONE);
		m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 0);

		m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATIONCOLOR, RUCOLOR_ARGB(0, 255, 255, 255));
		m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(1.0f));

		m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ZWRITEMODE, ruZWRITEMODE_DISABLE);
	}

	if(m_boxMesh == NULL)
	{
		m_boxMesh = ruNEW CRuDynamicMesh();
		m_boxMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, 8, 12, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);

		m_boxMesh->SetNumPrimitives(12);
		m_boxMesh->SetNumVertices(8);
	}

	if(m_boxMaterial == NULL)
	{
		m_boxMaterial = ruNEW CRuMaterial();

		m_boxMaterial->SetProperty("material:materialType", "Standard Material");

		m_boxMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_TRANSLUCENT, TRUE);

		m_boxMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_NONE);
		m_boxMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 0);

		m_boxMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATIONCOLOR, RUCOLOR_ARGB(0, 255, 255, 255));
		m_boxMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(1.0f));

		m_boxMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ZWRITEMODE, ruZWRITEMODE_DISABLE);
	}

	m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 0][0] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 0][1] = -1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 0][2] =  0.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 1][0] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 1][1] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 1][2] =  0.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 2][0] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 2][1] =  1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 2][2] =  0.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 3][0] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 3][1] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 3][2] =  0.0f;

	m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 4][0] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 4][1] = -1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 4][2] =  0.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 5][0] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 5][1] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 5][2] =  0.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 6][0] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 6][1] =  1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 6][2] =  0.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 7][0] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 7][1] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 7][2] =  0.0f;

	m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 8][0] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 8][1] = -1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 8][2] =  0.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 9][0] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 9][1] = -1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][ 9][2] =  0.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][10][0] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][10][1] =  1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][10][2] =  0.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][11][0] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][11][1] =  1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XY][11][2] =  0.0f;

	m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 0][0] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 0][1] = -1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 0][2] = -1.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 1][0] =  1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 1][1] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 1][2] = -1.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 2][0] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 2][1] =  1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 2][2] = -1.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 3][0] = -1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 3][1] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 3][2] = -1.0f;

	m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 4][0] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 4][1] = -1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 4][2] =  1.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 5][0] =  1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 5][1] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 5][2] =  1.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 6][0] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 6][1] =  1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 6][2] =  1.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 7][0] = -1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 7][1] =  0.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 7][2] =  1.0f;

	m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 8][0] = -1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 8][1] = -1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 8][2] =  0.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 9][0] =  1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 9][1] = -1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][ 9][2] =  0.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][10][0] =  1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][10][1] =  1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][10][2] =  0.0f;
	m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][11][0] = -1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][11][1] =  1.0f;		m_constraintMultipliers[ruGIZMOCONSTRAINT_XZ][11][2] =  0.0f;

	// Build the initial gizmo mesh
	UpdateGizmoMesh();

	// Setup event handlers
	Event_Camera().RegisterHandler(this->GetGUID(), boost::bind(&CRuGizmo_Box::HandleEvent_Camera, this, _1));
}

CRuGizmo_Box::~CRuGizmo_Box()
{
	ruSAFE_RELEASE(m_mesh);
	ruSAFE_RELEASE(m_material);

	ruSAFE_RELEASE(m_boxMesh);
	ruSAFE_RELEASE(m_boxMaterial);
}

const CRuOBB &CRuGizmo_Box::GetBox() const
{
	return m_box;
}

void CRuGizmo_Box::SetBox(const CRuOBB &box)
{
	m_box = box;

	// Normalize OBB
	NormalizeBox();

	// Update the gizmo mesh
	UpdateGizmoMesh();
}

CRuEntity *CRuGizmo_Box::Clone(CRuEntity *clonedEntity)
{
	return NULL;
}

BOOL CRuGizmo_Box::SerializeFrom(IRuStream *stream)
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

	// Read box parameters
	::SerializeFrom(stream, &m_box);

	NormalizeBox();

	// Update gizmo mesh
	UpdateGizmoMesh();

	return TRUE;
}

BOOL CRuGizmo_Box::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Renderable::SerializeTo(stream);

	// Write box parameters
	::SerializeTo(stream, &m_box);

	return TRUE;
}

BOOL CRuGizmo_Box::ResetStates()
{
	return TRUE;
}

BOOL CRuGizmo_Box::Update(float elapsedTime)
{
	// Chain update call to base class
	IRuEntity_Renderable::Update(elapsedTime);

	return TRUE;
}

BOOL CRuGizmo_Box::QueryCollision(CRuCollisionQuery *colQuery)
{
	CRuMatrix4x4 meshWorldTransform = this->GetWorldTransform();
	CRuMatrix4x4 inverseMeshWorldTransform = meshWorldTransform.GetInverse();

	switch(colQuery->m_queryType)
	{
		case ruCOLQUERY_RAY_NEAREST:
			{
				CRuCollisionQuery_Ray_Nearest *rayQuery = static_cast<CRuCollisionQuery_Ray_Nearest *>(colQuery);

				CRuRay3 localRay(rayQuery->m_colRay, inverseMeshWorldTransform);

				CRuAABB localBox(-m_box.Extents()[0], -m_box.Extents()[1], -m_box.Extents()[2], m_box.Extents()[0], m_box.Extents()[1], m_box.Extents()[2]);

				CRuVector3 intPt;
				REAL intT;

				if(FindIntersection(localRay, localBox, intPt, intT))
				{
					rayQuery->m_positiveHit = TRUE;
					rayQuery->m_collisionPoint = intPt * this->GetWorldTransform();
					rayQuery->m_collisionT = intT;
				}

				return TRUE;
			}

			break;

		case ruCOLQUERY_SEGMENT_HITTEST:
			{
				CRuCollisionQuery_Segment_HitTest *segQuery = static_cast<CRuCollisionQuery_Segment_HitTest *>(colQuery);

				// Get the segment in local coordinates
				CRuSegment3 localSegment(segQuery->m_colSegment);
				inverseMeshWorldTransform.TransformPoint(localSegment.Origin());
				inverseMeshWorldTransform.TransformVector(localSegment.Direction());

				CRuAABB localBox(-m_box.Extents()[0], -m_box.Extents()[1], -m_box.Extents()[2], m_box.Extents()[0], m_box.Extents()[1], m_box.Extents()[2]);

				CRuVector3 intPt;
				REAL intT;

				if(FindIntersection(localSegment, localBox, intPt, intT))
				{
					segQuery->m_positiveHit = TRUE;
				}

				return TRUE;
			}

			break;

		case ruCOLQUERY_SEGMENT_NEAREST:
			{
				CRuCollisionQuery_Segment_Nearest *segQuery = static_cast<CRuCollisionQuery_Segment_Nearest *>(colQuery);

				// Get the segment in local coordinates
				CRuSegment3 localSegment(segQuery->m_colSegment);
				inverseMeshWorldTransform.TransformPoint(localSegment.Origin());
				inverseMeshWorldTransform.TransformVector(localSegment.Direction());

				CRuAABB localBox(-m_box.Extents()[0], -m_box.Extents()[1], -m_box.Extents()[2], m_box.Extents()[0], m_box.Extents()[1], m_box.Extents()[2]);

				CRuVector3 intPt;
				REAL intT;

				if(FindIntersection(localSegment, localBox, intPt, intT))
				{
					segQuery->m_positiveHit = TRUE;
					segQuery->m_collisionPoint = intPt * this->GetWorldTransform();
					segQuery->m_collisionT = intT;
				}

				return TRUE;
			}

			break;
	}

	return TRUE;
}

INT32 CRuGizmo_Box::GetNumMeshes() const
{
	return 2;
}

BOOL CRuGizmo_Box::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex == 0)
	{
		*meshOut = m_mesh;
		*materialOut = m_material;

		return TRUE;
	}
	else if(meshIndex == 1)
	{
		*meshOut = m_boxMesh;
		*materialOut = m_boxMaterial;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuGizmo_Box::SetConstraint(RuGizmoConstraint gizmoConstraint)
{
	m_gizmoConstraint = gizmoConstraint;

	return TRUE;
}

BOOL CRuGizmo_Box::HighlightManipulator(const CRuRay3 &pickRay)
{
	INT32 oldHighlightControlCylinder = m_highlightControlCylinder;

	CRuMatrix4x4 meshWorldTransform = this->GetWorldTransform();
	CRuMatrix4x4 inverseMeshWorldTransform = meshWorldTransform.GetInverse();

	// Transform pick ray into local coordinate frame
	CRuRay3 localRay;
	inverseMeshWorldTransform.TransformPoint(pickRay.Origin(), localRay.Origin());
	inverseMeshWorldTransform.TransformVector(pickRay.Direction(), localRay.Direction());


	CRuVector3 *position = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();

	REAL cylRadius = 0.25f;

	m_controlCylinders[ 0].Set(CRuSegment3(position[ 0], position[ 1]), cylRadius * m_controlCylinderRadii[ 0]);
	m_controlCylinders[ 1].Set(CRuSegment3(position[ 2], position[ 3]), cylRadius * m_controlCylinderRadii[ 1]);
	m_controlCylinders[ 2].Set(CRuSegment3(position[ 4], position[ 5]), cylRadius * m_controlCylinderRadii[ 2]);
	m_controlCylinders[ 3].Set(CRuSegment3(position[ 6], position[ 7]), cylRadius * m_controlCylinderRadii[ 3]);

	m_controlCylinders[ 4].Set(CRuSegment3(position[ 8], position[ 9]), cylRadius * m_controlCylinderRadii[ 4]);
	m_controlCylinders[ 5].Set(CRuSegment3(position[10], position[11]), cylRadius * m_controlCylinderRadii[ 5]);
	m_controlCylinders[ 6].Set(CRuSegment3(position[12], position[13]), cylRadius * m_controlCylinderRadii[ 6]);
	m_controlCylinders[ 7].Set(CRuSegment3(position[14], position[15]), cylRadius * m_controlCylinderRadii[ 7]);

	m_controlCylinders[ 8].Set(CRuSegment3(position[16], position[17]), cylRadius * m_controlCylinderRadii[ 8]);
	m_controlCylinders[ 9].Set(CRuSegment3(position[18], position[19]), cylRadius * m_controlCylinderRadii[ 9]);
	m_controlCylinders[10].Set(CRuSegment3(position[20], position[21]), cylRadius * m_controlCylinderRadii[10]);
	m_controlCylinders[11].Set(CRuSegment3(position[22], position[23]), cylRadius * m_controlCylinderRadii[11]);

	// Test pick ray against each of the twelve control cylinders
	INT32 nearestIntersectionControlCylinder = -1;
	REAL nearestIntersectionT = FLT_MAX;

	for(INT32 i = 0; i < 12; ++i)
	{
		REAL intT;
		if(FindIntersection(localRay, m_controlCylinders[i], intT) && intT < nearestIntersectionT)
		{
			nearestIntersectionControlCylinder = i;
			nearestIntersectionT = intT;
		}
	}

	// Store active control box index
	m_highlightControlCylinder = nearestIntersectionControlCylinder;

	if(oldHighlightControlCylinder != m_highlightControlCylinder)
	{
		UpdateGizmoMesh();
	}

	return TRUE;
}

BOOL CRuGizmo_Box::ClearManipulatorHighlight()
{
	if(m_highlightControlCylinder != -1)
	{
		m_highlightControlCylinder = -1;
		UpdateGizmoMesh();
	}

	return TRUE;
}

BOOL CRuGizmo_Box::ActivateManipulator(const CRuRay3 &pickRay)
{
	CRuMatrix4x4 meshWorldTransform = this->GetWorldTransform();
	CRuMatrix4x4 inverseMeshWorldTransform = meshWorldTransform.GetInverse();

	// Transform pick ray into local coordinate frame
	CRuRay3 localRay;
	inverseMeshWorldTransform.TransformPoint(pickRay.Origin(), localRay.Origin());
	inverseMeshWorldTransform.TransformVector(pickRay.Direction(), localRay.Direction());

	CRuVector3 *position = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();

	REAL cylRadius = 0.25f;

	m_controlCylinders[ 0].Set(CRuSegment3(position[ 0], position[ 1]), cylRadius * m_controlCylinderRadii[ 0]);
	m_controlCylinders[ 1].Set(CRuSegment3(position[ 2], position[ 3]), cylRadius * m_controlCylinderRadii[ 1]);
	m_controlCylinders[ 2].Set(CRuSegment3(position[ 4], position[ 5]), cylRadius * m_controlCylinderRadii[ 2]);
	m_controlCylinders[ 3].Set(CRuSegment3(position[ 6], position[ 7]), cylRadius * m_controlCylinderRadii[ 3]);

	m_controlCylinders[ 4].Set(CRuSegment3(position[ 8], position[ 9]), cylRadius * m_controlCylinderRadii[ 4]);
	m_controlCylinders[ 5].Set(CRuSegment3(position[10], position[11]), cylRadius * m_controlCylinderRadii[ 5]);
	m_controlCylinders[ 6].Set(CRuSegment3(position[12], position[13]), cylRadius * m_controlCylinderRadii[ 6]);
	m_controlCylinders[ 7].Set(CRuSegment3(position[14], position[15]), cylRadius * m_controlCylinderRadii[ 7]);

	m_controlCylinders[ 8].Set(CRuSegment3(position[16], position[17]), cylRadius * m_controlCylinderRadii[ 8]);
	m_controlCylinders[ 9].Set(CRuSegment3(position[18], position[19]), cylRadius * m_controlCylinderRadii[ 9]);
	m_controlCylinders[10].Set(CRuSegment3(position[20], position[21]), cylRadius * m_controlCylinderRadii[10]);
	m_controlCylinders[11].Set(CRuSegment3(position[22], position[23]), cylRadius * m_controlCylinderRadii[11]);

	// Test pick ray against each of the twelve control cylinders
	INT32 nearestIntersectionControlCylinder = -1;
	REAL nearestIntersectionT = FLT_MAX;

	for(INT32 i = 0; i < 12; ++i)
	{
		REAL intT;
		CRuVector3 intPt;
		if(FindIntersection(localRay, m_controlCylinders[i], intT) && intT < nearestIntersectionT)
		{
			nearestIntersectionControlCylinder = i;
			nearestIntersectionT = intT;
		}
	}

	// Store active control box index
	m_activeControlCylinder = nearestIntersectionControlCylinder;

	// Store the current pick ray
	m_activeGizmoPickRay = pickRay;

	return m_activeControlCylinder >= 0;
}

BOOL CRuGizmo_Box::Manipulate(const CRuRay3 &pickRay)
{
	CRuVector3 movementVector;

	if(GetManipulationVector(pickRay, movementVector))
	{
		m_temp = movementVector;


		CRuVector3 halfMovementVector = movementVector * 0.5f;

		// Update pick ray
		m_activeGizmoPickRay = pickRay;

		CRuMatrix4x4 basisTransform;
		basisTransform.SetToBasis(m_box.Axis()[0], m_box.Axis()[1], m_box.Axis()[2]);

		CRuVector3 extentsMultiplier(-m_constraintMultipliers[m_gizmoConstraint][m_activeControlCylinder][0], -m_constraintMultipliers[m_gizmoConstraint][m_activeControlCylinder][1], -m_constraintMultipliers[m_gizmoConstraint][m_activeControlCylinder][2]);

		CRuVector3 oldExtents(m_box.Extents()[0] * extentsMultiplier[0], m_box.Extents()[1] * extentsMultiplier[1], m_box.Extents()[2] * extentsMultiplier[2]);
		CRuVector3 newExtents(oldExtents[0] + halfMovementVector[0] * extentsMultiplier[0], oldExtents[1] + halfMovementVector[1] * extentsMultiplier[1], oldExtents[2] + halfMovementVector[2] * extentsMultiplier[2]);

		basisTransform.TransformVector(oldExtents);
		basisTransform.TransformVector(newExtents);

		m_box.Center() = m_box.Center() - (newExtents - oldExtents);

		for(INT32 i = 0; i < 3; ++i)
			m_box.Extents()[i] = fabs(m_box.Extents()[i] + halfMovementVector[i]);

		// Normalize box such that its center is at (0, 0, 0)
		NormalizeBox();

		// Update the gizmo mesh
		UpdateGizmoMesh();

		return TRUE;
	}

	return FALSE;
}

BOOL CRuGizmo_Box::HandleEvent_Camera(RuEventArgs *eventArgs)
{
	RuEntityEvent_Camera_Args *cameraArgs = static_cast<RuEntityEvent_Camera_Args *>(eventArgs);

	const CRuMatrix4x4 &viewTransform = cameraArgs->m_camera->GetCameraTransform();
	const CRuMatrix4x4 &projTransform = cameraArgs->m_camera->GetProjectionTransform();

	for(INT32 i = 0; i < 12; ++i)
	{
		CRuVector3 pt0 = m_controlCylinders[i].Origin();
		CRuVector3 pt1 = m_controlCylinders[i].Origin() + m_controlCylinders[i].Direction();

		pt0 = pt0 + this->GetTranslation(ruFRAMETYPE_WORLD);
		pt1 = pt1 + this->GetTranslation(ruFRAMETYPE_WORLD);

		viewTransform.TransformPoint(pt0);
		viewTransform.TransformPoint(pt1);

		CRuVector3 pt0a = pt0 + CRuVector3(1.0f, 0.0f, 0.0);
		CRuVector3 pt1a = pt1 + CRuVector3(1.0f, 0.0f, 0.0);

		projTransform.TransformPointProjected(pt0);
		projTransform.TransformPointProjected(pt0a);

		projTransform.TransformPointProjected(pt1);
		projTransform.TransformPointProjected(pt1a);

		REAL len0 = (pt0a - pt0).Magnitude();
		REAL len1 = (pt1a - pt1).Magnitude();
		REAL invScale = min(len0, len1);

		if(invScale > ruEPSILON)
		{
			m_controlCylinderRadii[i] = 0.050f / invScale;
		}
		else
		{
			m_controlCylinderRadii[i] = 0;
		}
	}

	return TRUE;
}

CRuGizmo_Box &CRuGizmo_Box::operator=(const CRuGizmo_Box &src)
{
	m_boxColor = src.m_boxColor;
	this->SetBox(src.m_box);

	m_gizmoConstraint = src.m_gizmoConstraint;
	m_activeGizmoPickRay = src.m_activeGizmoPickRay;

	return *this;
}

BOOL CRuGizmo_Box::GetManipulationVector(const CRuRay3 &pickRay, CRuVector3 &vectorOut)
{
	REAL stopThreshold = 0.05f;
	CRuVector3 gizmoOrigin = CRuVector3(0.0f, 0.0f, 0.0f);

	// Get the ray in local coordinates
	CRuRay3 localRay0, localRay1;

	CRuMatrix4x4 meshWorldTransform = this->GetWorldTransform();
	CRuMatrix4x4 inverseMeshWorldTransform = meshWorldTransform.GetInverse();

	inverseMeshWorldTransform.TransformPoint(m_activeGizmoPickRay.Origin(), localRay0.Origin());
	inverseMeshWorldTransform.TransformVector(m_activeGizmoPickRay.Direction(), localRay0.Direction());
	localRay0.Direction().Normalize();

	inverseMeshWorldTransform.TransformPoint(pickRay.Origin(), localRay1.Origin());
	inverseMeshWorldTransform.TransformVector(pickRay.Direction(), localRay1.Direction());
	localRay1.Direction().Normalize();

	// Temp variables
	CRuVector3 oldPosition, newPosition;
	REAL oldT, newT;

	// Reset output vector
	vectorOut.Set(0.0f, 0.0f, 0.0f);

	CRuPlane4 movementPlane;

	switch(m_gizmoConstraint)
	{
		case ruGIZMOCONSTRAINT_XY:
			movementPlane.SetPlane(CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 0.0f, 0.0f));
			break;

		case ruGIZMOCONSTRAINT_XZ:
			movementPlane.SetPlane(CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, 0.0f, 0.0f));
			break;

		case ruGIZMOCONSTRAINT_YZ:
			movementPlane.SetPlane(CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 0.0f));
			break;
	}

	// if the pick ray is almost perpendicular to the constraining plane, abort
	if(fabs(DotProduct(localRay1.Direction(), CRuVector3(0.0f, 1.0f, 0.0f))) <= stopThreshold)
		return FALSE;

	if(FindIntersection(localRay0, movementPlane, oldPosition, oldT) && FindIntersection(localRay1, movementPlane, newPosition, newT))
	{
		vectorOut = newPosition - oldPosition;

		for(INT32 i = 0; i < 3; ++i)
		{
			vectorOut[i] *= m_constraintMultipliers[m_gizmoConstraint][m_activeControlCylinder][i];
		}

		return TRUE;
	}

	return FALSE;
}

void CRuGizmo_Box::UpdateGizmoMesh()
{
	// Initialize vertices
	CRuVector3 *position = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetDiffuse();

	/*
	          7
		  --------
		 /| 3    /|
		/-------/ |6
		|8|    |  |
	    | |----|--|
	   4| /  5 |2 /
		|/     | /
		/-------/
		    1
	*/

	REAL controlBoxSize = 0.25f;

	const REAL *extents = m_box.GetExtents();

	DWORD normalColor = RUCOLOR_ARGB(0xFF, 128, 128, 128);
	DWORD highlightColor = RUCOLOR_ARGB(0xFF, 0xFF, 0xFF, 0xFF);

	// Create box centered at (0, 0, 0) with the current box extents
	position[ 0].Set(-extents[0], -extents[1], -extents[2]);
	position[ 1].Set( extents[0], -extents[1], -extents[2]);
	diffuse [ 0] = m_highlightControlCylinder == 0 ? highlightColor : normalColor;
	diffuse [ 1] = m_highlightControlCylinder == 0 ? highlightColor : normalColor;

	position[ 2].Set( extents[0], -extents[1], -extents[2]);
	position[ 3].Set( extents[0],  extents[1], -extents[2]);
	diffuse [ 2] = m_highlightControlCylinder == 1 ? highlightColor : normalColor;
	diffuse [ 3] = m_highlightControlCylinder == 1 ? highlightColor : normalColor;

	position[ 4].Set(-extents[0],  extents[1], -extents[2]);
	position[ 5].Set( extents[0],  extents[1], -extents[2]);
	diffuse [ 4] = m_highlightControlCylinder == 2 ? highlightColor : normalColor;
	diffuse [ 5] = m_highlightControlCylinder == 2 ? highlightColor : normalColor;

	position[ 6].Set(-extents[0], -extents[1], -extents[2]);
	position[ 7].Set(-extents[0],  extents[1], -extents[2]);
	diffuse [ 6] = m_highlightControlCylinder == 3 ? highlightColor : normalColor;
	diffuse [ 7] = m_highlightControlCylinder == 3 ? highlightColor : normalColor;

	position[ 8].Set(-extents[0], -extents[1],  extents[2]);
	position[ 9].Set( extents[0], -extents[1],  extents[2]);
	diffuse [ 8] = m_highlightControlCylinder == 4 ? highlightColor : normalColor;
	diffuse [ 9] = m_highlightControlCylinder == 4 ? highlightColor : normalColor;

	position[10].Set( extents[0], -extents[1],  extents[2]);
	position[11].Set( extents[0],  extents[1],  extents[2]);
	diffuse [10] = m_highlightControlCylinder == 5 ? highlightColor : normalColor;
	diffuse [11] = m_highlightControlCylinder == 5 ? highlightColor : normalColor;

	position[12].Set(-extents[0],  extents[1],  extents[2]);
	position[13].Set( extents[0],  extents[1],  extents[2]);
	diffuse [12] = m_highlightControlCylinder == 6 ? highlightColor : normalColor;
	diffuse [13] = m_highlightControlCylinder == 6 ? highlightColor : normalColor;

	position[14].Set(-extents[0], -extents[1],  extents[2]);
	position[15].Set(-extents[0],  extents[1],  extents[2]);
	diffuse [14] = m_highlightControlCylinder == 7 ? highlightColor : normalColor;
	diffuse [15] = m_highlightControlCylinder == 7 ? highlightColor : normalColor;

	position[16].Set(-extents[0], -extents[1], -extents[2]);
	position[17].Set(-extents[0], -extents[1],  extents[2]);
	diffuse [16] = m_highlightControlCylinder == 8 ? highlightColor : normalColor;
	diffuse [17] = m_highlightControlCylinder == 8 ? highlightColor : normalColor;

	position[18].Set( extents[0], -extents[1], -extents[2]);
	position[19].Set( extents[0], -extents[1],  extents[2]);
	diffuse [18] = m_highlightControlCylinder == 9 ? highlightColor : normalColor;
	diffuse [19] = m_highlightControlCylinder == 9 ? highlightColor : normalColor;

	position[20].Set( extents[0],  extents[1], -extents[2]);
	position[21].Set( extents[0],  extents[1],  extents[2]);
	diffuse [20] = m_highlightControlCylinder == 10 ? highlightColor : normalColor;
	diffuse [21] = m_highlightControlCylinder == 10 ? highlightColor : normalColor;

	position[22].Set(-extents[0],  extents[1], -extents[2]);
	position[23].Set(-extents[0],  extents[1],  extents[2]);
	diffuse [22] = m_highlightControlCylinder == 11 ? highlightColor : normalColor;
	diffuse [23] = m_highlightControlCylinder == 11 ? highlightColor : normalColor;

	// Initialize indices
	INT32 j = 0;
	UINT16 *indices = m_mesh->GetIndices();

	for(INT32 i = 0; i < 24; ++i)
	{
		indices[i] = i;
	}

	// Update mesh iteration and bounds
	m_mesh->IncrementMeshIteration();
	m_mesh->CalculateBounds();

	// Update local entity bounds
	this->SetLocalEntityBounds(m_mesh->GetBounds());







	DWORD boxColor = RUCOLOR_ARGB(128, 128, 128, 128);

	position = static_cast<CRuMorphTarget *>(m_boxMesh->GetMorphTarget(0))->GetPosition();
	diffuse = static_cast<CRuMorphTarget *>(m_boxMesh->GetMorphTarget(0))->GetDiffuse();

	// Create box centered at (0, 0, 0) with the current box extents
	position[ 0].Set(-extents[0],  extents[1], -extents[2]);
	position[ 1].Set( extents[0],  extents[1], -extents[2]);
	position[ 2].Set( extents[0],  extents[1],  extents[2]);
	position[ 3].Set(-extents[0],  extents[1],  extents[2]);
	position[ 4].Set(-extents[0], -extents[1], -extents[2]);
	position[ 5].Set( extents[0], -extents[1], -extents[2]);
	position[ 6].Set( extents[0], -extents[1],  extents[2]);
	position[ 7].Set(-extents[0], -extents[1],  extents[2]);

	diffuse [ 0] = boxColor;
	diffuse [ 1] = boxColor;
	diffuse [ 2] = boxColor;
	diffuse [ 3] = boxColor;
	diffuse [ 4] = boxColor;
	diffuse [ 5] = boxColor;
	diffuse [ 6] = boxColor;
	diffuse [ 7] = boxColor;

	// Initialize indices
	indices = m_boxMesh->GetIndices();

	j = 0;

	// Top face
	indices[j++] = 0;
	indices[j++] = 2;
	indices[j++] = 1;

	indices[j++] = 0;
	indices[j++] = 3;
	indices[j++] = 2;

	// Bottom face
	indices[j++] = 4;
	indices[j++] = 5;
	indices[j++] = 6;

	indices[j++] = 4;
	indices[j++] = 6;
	indices[j++] = 7;

	// Forward face
	indices[j++] = 0;
	indices[j++] = 1;
	indices[j++] = 5;

	indices[j++] = 0;
	indices[j++] = 5;
	indices[j++] = 4;

	// Backward face
	indices[j++] = 2;
	indices[j++] = 3;
	indices[j++] = 7;

	indices[j++] = 2;
	indices[j++] = 7;
	indices[j++] = 6;

	// Left face
	indices[j++] = 3;
	indices[j++] = 0;
	indices[j++] = 4;

	indices[j++] = 3;
	indices[j++] = 4;
	indices[j++] = 7;

	// Right face
	indices[j++] = 1;
	indices[j++] = 2;
	indices[j++] = 6;

	indices[j++] = 1;
	indices[j++] = 6;
	indices[j++] = 5;

	// Update mesh iteration and bounds
	m_boxMesh->IncrementMeshIteration();
	m_boxMesh->CalculateBounds();
}

void CRuGizmo_Box::NormalizeBox()
{
	// Build transform from the OBB
	CRuMatrix4x4 test1;
	CRuMatrix4x4 test2;

	test1.SetToBasis(m_box.Axis()[0], m_box.Axis()[1], m_box.Axis()[2]);
	test2.SetToTranslation(m_box.Center());

	CRuMatrix4x4 boxTransform = test1 * test2;

	this->Transform(boxTransform, ruTRANSFORMOP_PRECONCAT);

	// Remove transform from the OBB
	m_box.Axis()[0].Set(1.0f, 0.0f, 0.0f);
	m_box.Axis()[1].Set(0.0f, 1.0f, 0.0f);
	m_box.Axis()[2].Set(0.0f, 0.0f, 1.0f);

	m_box.Center().Set(0.0f, 0.0f, 0.0f);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
