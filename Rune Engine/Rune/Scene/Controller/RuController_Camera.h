#ifndef _RUCONTROLLER_CAMERA_H_
#define _RUCONTROLLER_CAMERA_H_

#include "../../Scene/Base/RuEntity_Camera.h"
#include "../../Scene/Base/RuEntityController.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuController_Camera : public IRuEntity_Controller
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Settings
	CRuCamera*								m_targetCamera;									//!< Camera that the controller affects
	IRuObject*								m_collisionObject;								//!< Collision object to detect camera collisions with
	UINT32									m_collisionExclusionMask;						//!< Geometry collision exclusion mask
	CRuEntity_Camera*						m_sceneTargetCamera;							//!< Scene camera settings

	// Runtime
	CRuVector3								m_currentCameraPosition;

	REAL									m_cameraMovementSpeed;
	REAL									m_presetCameraToTargetDistance;
	REAL									m_targetCameraToTargetDistance;

public:
											CRuController_Camera();
	virtual									~CRuController_Camera();

	virtual BOOL							Update(REAL elapsedTime);

	virtual REAL							GetStartTime() const;
	virtual REAL							GetDuration() const;

	virtual BOOL							IsLooping() const;

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *target);

	void									UpdatePresetCameraConfiguration();

	void									SetCollisionObject(IRuObject *colObject);
	void									SetCollisionExclusionMask(UINT32 mask);

	void									SetSceneTargetCamera(CRuEntity_Camera *sceneTargetCamera);

protected:
	REAL									FindCollisionDistance(const CRuVector3 &targetPosition, const CRuMatrix4x4 &inverseCameraTransform, const CRuMatrix4x4 &projectionTransform);
	REAL									FindNearPlaneCollisionDistance(const CRuVector3 &targetPosition, const CRuMatrix4x4 &inverseCameraTransform, const CRuMatrix4x4 &projectionTransform);

	void									PerformBufferedMovement(REAL effectiveCameraDollySpeed, REAL elapsedTime);
	void									Dolly(REAL offset, BOOL buffered);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
