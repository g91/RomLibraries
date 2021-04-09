/*!
	@file RuEntity_Camera.h

	Copyright (c) 2004-2007 All rights reserved
*/

#ifndef _RUENTITY_CAMERA_H_
#define _RUENTITY_CAMERA_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuEntity_Camera : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	BOOL									m_orthographic;
	REAL									m_fov;
	REAL									m_nearClipPlane;
	REAL									m_farClipPlane;

	CRuVector3								m_cameraUpVector;
	CRuVector3								m_cameraPosition;
	CRuVector3								m_targetPosition;

public:
											CRuEntity_Camera();
	virtual									~CRuEntity_Camera();

	BOOL									GetOrthographic();
	void									SetOrthographic(BOOL orthographic);

	REAL									GetFOV();
	void									SetFOV(REAL fov);

	REAL									GetNearClipPlane();
	void									SetNearClipPlane(REAL nearClipPlane);

	REAL									GetFarClipPlane();
	void									SetFarClipPlane(REAL farClipPlane);

	const CRuVector3&						GetCameraUpVector();
	void									SetCameraUpVector(const CRuVector3 &cameraUpVector);

	const CRuVector3&						GetCameraPosition();
	void									SetCameraPosition(const CRuVector3 &cameraPosition);

	const CRuVector3&						GetTargetPosition();
	void									SetTargetPosition(const CRuVector3 &targetPosition);

	// IRuObject interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_Camera, CRuEntity_Camera)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
