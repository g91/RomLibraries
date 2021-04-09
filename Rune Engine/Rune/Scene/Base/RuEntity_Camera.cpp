/*!
	@file RuEntity_Camera.cpp

	Copyright (c) 2004-2007 All rights reserved
*/

#include "../../Scene/Base/RuEntity_Camera.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Camera, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Camera, "CRuEntity_Camera", "CRuEntity")
ruCLASSGUID_DEFINE(CRuEntity_Camera, 0x5A4BE25A, 0x96BD4700, 0xA6E2580F, 0x328B767B)

// ************************************************************************************************************************************************************

CRuEntity_Camera::CRuEntity_Camera()
:	m_orthographic(FALSE),
	m_fov(1.0f),
	m_nearClipPlane(1.0f),
	m_farClipPlane(1000.0f),
	m_cameraPosition(0.0f, 0.0f, 0.0f),
	m_targetPosition(0.0f, 0.0f, 1.0f)
{
}

CRuEntity_Camera::~CRuEntity_Camera()
{
}

BOOL CRuEntity_Camera::GetOrthographic()
{
	return m_orthographic;
}

void CRuEntity_Camera::SetOrthographic(BOOL orthographic)
{
	m_orthographic = orthographic;
}

REAL CRuEntity_Camera::GetFOV()
{
	return m_fov;
}

void CRuEntity_Camera::SetFOV(REAL fov)
{
	m_fov = fov;
}

REAL CRuEntity_Camera::GetNearClipPlane()
{
	return m_nearClipPlane;
}

void CRuEntity_Camera::SetNearClipPlane(REAL nearClipPlane)
{
	m_nearClipPlane = nearClipPlane;
}

REAL CRuEntity_Camera::GetFarClipPlane()
{
	return m_farClipPlane;
}

void CRuEntity_Camera::SetFarClipPlane(REAL farClipPlane)
{
	m_farClipPlane = farClipPlane;
}

const CRuVector3 &CRuEntity_Camera::GetCameraUpVector()
{
	return m_cameraUpVector;
}

void CRuEntity_Camera::SetCameraUpVector(const CRuVector3 &cameraUpVector)
{
	m_cameraUpVector = cameraUpVector;
}

const CRuVector3 &CRuEntity_Camera::GetCameraPosition()
{
	return m_cameraPosition;
}

void CRuEntity_Camera::SetCameraPosition(const CRuVector3 &cameraPosition)
{
	m_cameraPosition = cameraPosition;
}

const CRuVector3 &CRuEntity_Camera::GetTargetPosition()
{
	return m_targetPosition;
}

void CRuEntity_Camera::SetTargetPosition(const CRuVector3 &targetPosition)
{
	m_targetPosition = targetPosition;
}

IRuObject *CRuEntity_Camera::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuEntity_Camera();

	// Chain cloning call to parent class
	CRuEntity::Clone(clonedObject);

	// Shortcut reference to the cloned entity
	CRuEntity_Camera *clonedCamera = static_cast<CRuEntity_Camera *>(clonedObject);

	clonedCamera->m_orthographic = this->m_orthographic;
	clonedCamera->m_fov = this->m_fov;
	clonedCamera->m_nearClipPlane = this->m_nearClipPlane;
	clonedCamera->m_farClipPlane = this->m_farClipPlane;

	clonedCamera->m_cameraUpVector = this->m_cameraUpVector;
	clonedCamera->m_cameraPosition = this->m_cameraPosition;
	clonedCamera->m_targetPosition = this->m_targetPosition;

	return clonedCamera;
}

BOOL CRuEntity_Camera::SerializeFrom(IRuStream *stream)
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
	CRuEntity::SerializeFrom(stream);

	// Read camera parameters from stream
	stream->Read(&m_orthographic, sizeof(BOOL));
	stream->Read(&m_fov, sizeof(REAL));
	stream->Read(&m_nearClipPlane, sizeof(REAL));
	stream->Read(&m_farClipPlane, sizeof(REAL));

	return TRUE;
}

BOOL CRuEntity_Camera::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuEntity::SerializeTo(stream);

	// Write camera parameters to stream
	stream->Write(&m_orthographic, sizeof(BOOL));
	stream->Write(&m_fov, sizeof(REAL));
	stream->Write(&m_nearClipPlane, sizeof(REAL));
	stream->Write(&m_farClipPlane, sizeof(REAL));

	return TRUE;
}

BOOL CRuEntity_Camera::Update(REAL elapsedTime)
{
	const CRuMatrix4x4 &worldTransform = this->GetWorldTransform();

	CRuVector3 cameraUpVector(0.0f, 0.0f, 1.0f);
	CRuVector3 cameraAtVector(0.0f, -1.0f, 0.0f);
	CRuVector3 cameraPosition(0.0f, 0.0f, 0.0f);

	worldTransform.TransformVector(cameraUpVector);
	worldTransform.TransformVector(cameraAtVector);
	worldTransform.TransformPoint(cameraPosition);

	CRuVector3 targetPosition = cameraPosition + cameraAtVector;

	m_cameraUpVector = cameraUpVector;
	m_cameraPosition = cameraPosition;
	m_targetPosition = targetPosition;

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
