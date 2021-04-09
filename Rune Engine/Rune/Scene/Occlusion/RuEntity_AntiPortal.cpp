/*!
	@file RuEntity_AntiPortal.cpp

	Copyright (c) 2004-2007 All rights reserved
*/

#include "../../Scene/Occlusion/RuEntity_AntiPortal.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_AntiPortal, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_AntiPortal, "CRuEntity_AntiPortal", "CRuEntity")
ruCLASSGUID_DEFINE(CRuEntity_AntiPortal, 0x42F51121, 0x6A0345d5, 0x8D614A2B, 0xD3CC42D2)

// ************************************************************************************************************************************************************

CRuEntity_AntiPortal::CRuEntity_AntiPortal()
{
	// Register event handlers
	Event_Camera().RegisterHandler(this->GetGUID(), boost::bind(&CRuEntity_AntiPortal::HandleEvent_Camera, this, _1));
}

CRuEntity_AntiPortal::~CRuEntity_AntiPortal()
{
}

BOOL CRuEntity_AntiPortal::SetPortalVertices(const CRuVector3 *vertices, INT32 vertexCount)
{
	if(vertexCount != 4)
	{
		return FALSE;
	}

	for(INT32 i = 0; i < 4; ++i)
	{
		m_vertices[i] = vertices[i];
	}

	return TRUE;
}

BOOL CRuEntity_AntiPortal::IsOccluded(const CRuSphere &sphere)
{
	BOOL side[5];

//	for(INT32 i = 0; i < 5; ++i)
	for(INT32 i = 0; i < 4; ++i)
	{
		REAL dist = m_portalPlanes[i].GetDistance(sphere.Center());

		// The sphere straddles plane
		if(dist < sphere.Radius() && dist > -sphere.Radius())
		{
			return FALSE;
		}

		// Determine sign
		if(dist > 0.0f)
		{
			side[i] = TRUE;
		}
		else
		{
			side[i] = FALSE;
		}
	}

	// If sphere is on the same side of all planes, it is occluded
//	if(side[0] == side[1] && side[0] == side[2] && side[0] == side[3] && side[0] == side[4])
	if(side[0] == side[1] && side[0] == side[2] && side[0] == side[3])
	{
		return TRUE;
	}

	return FALSE;
}

IRuObject *CRuEntity_AntiPortal::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuEntity_AntiPortal();

	// Chain cloning call to parent class
	CRuEntity::Clone(clonedObject);

	// Shortcut
	CRuEntity_AntiPortal *clonedAntiPortal = static_cast<CRuEntity_AntiPortal *>(clonedObject);

	// Copy portal parameters
	for(INT32 i = 0; i < 4; ++i)
	{
		clonedAntiPortal->m_vertices[i] = m_vertices[i];
	}

	return clonedObject;
}

BOOL CRuEntity_AntiPortal::SerializeFrom(IRuStream *stream)
{
	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuEntity::SerializeFrom(stream);

	for(INT32 i = 0; i < 4; ++i)
	{
		::SerializeFrom(stream, &m_vertices[i]);
	}

	return TRUE;
}

BOOL CRuEntity_AntiPortal::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuEntity::SerializeTo(stream);

	for(INT32 i = 0; i < 4; ++i)
	{
		::SerializeTo(stream, &m_vertices[i]);
	}

	return TRUE;
}

BOOL CRuEntity_AntiPortal::Update(REAL elapsedTime)
{
	// Chain update to base class
	CRuEntity::Update(elapsedTime);

	return TRUE;
}

BOOL CRuEntity_AntiPortal::HandleEvent_Camera(RuEventArgs *eventArgs)
{
	RuEntityEvent_Camera_Args *cameraArgs = static_cast<RuEntityEvent_Camera_Args *>(eventArgs); 
	CRuCamera *camera = cameraArgs->m_camera;

	// Get camera world position
	CRuVector3 camPos = camera->GetCameraPosition();

	// Transform portal vertices into world coordinate frame
	CRuMatrix4x4 worldTransform = this->GetWorldTransform();
	for(INT32 i = 0; i < 4; ++i)
	{
		worldTransform.TransformPoint(m_vertices[i], m_vertices_World[i]);
	}

	// Update portal volume bounding planes
	for(INT32 i = 0, j = 1; i < 4; ++i, j = (j + 1 == 4) ? 0 : j + 1)
	{
		// Assuming portal vertices are in a clock-wise direction, then n will be pointing outwards
		// Otherwise, n will be pointing inwards
		CRuVector3 e0 = m_vertices_World[i] - camPos;
		CRuVector3 e1 = camPos - m_vertices_World[j];
		CRuVector3 n = CrossProduct(e0, e1).Normalize();

		m_portalPlanes[i].SetPlane(n, camPos);
	}

	CRuVector3 ne0 = m_vertices_World[1] - m_vertices_World[0];
	CRuVector3 ne1 = m_vertices_World[2] - m_vertices_World[1];
	CRuVector3 nn = CrossProduct(ne0, ne1).Normalize();

	m_portalPlanes[4].SetPlane(nn, m_vertices_World[0]);

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
