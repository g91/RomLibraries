#include "../../Scene/Controller/RuController_Camera.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_Camera, IRuEntity_Controller)
ruHEAP_DEFINE_SUBCLASS(CRuController_Camera, "CRuController_Camera", "IRuEntity_Controller")
ruCLASSGUID_DEFINE(CRuController_Camera, 0xC3AA17A3, 0x8CD943ba, 0x93B10784, 0x48C6F096)

// ************************************************************************************************************************************************************

CRuController_Camera::CRuController_Camera()
:	m_targetCamera(NULL),
	m_collisionObject(NULL),
	m_collisionExclusionMask(0x00000000),
	m_sceneTargetCamera(NULL),
	m_cameraMovementSpeed(10.0f)
{
}

CRuController_Camera::~CRuController_Camera()
{
	// Release target camera
	ruSAFE_RELEASE(m_targetCamera);

	// Release old collision object
	ruSAFE_RELEASE(m_collisionObject);

	// Release scene target camera
	ruSAFE_RELEASE(m_sceneTargetCamera);
}

BOOL CRuController_Camera::Update(REAL elapsedTime)
{
	if(m_targetCamera)
	{
		if(m_sceneTargetCamera)
		{
			// Set new camera parameters
			m_targetCamera->SetCameraPosition(m_sceneTargetCamera->GetCameraPosition());
			m_targetCamera->SetTargetPosition(m_sceneTargetCamera->GetTargetPosition());
			m_targetCamera->SetCameraUpVector(m_sceneTargetCamera->GetCameraUpVector());
		}
		else
		{
			// Detect possible collision with camera-to-target path
			REAL colDist = FindCollisionDistance(m_targetCamera->GetTargetPosition(), m_targetCamera->GetInverseCameraTransform(), m_targetCamera->GetProjectionTransform());
			REAL effectiveCameraDollySpeed = m_cameraMovementSpeed;

			if(colDist < FLT_MAX)
			{
				effectiveCameraDollySpeed = 2000.0f;
				m_targetCameraToTargetDistance = min(colDist, m_targetCameraToTargetDistance);
			}
			else
			{
				m_targetCameraToTargetDistance = m_presetCameraToTargetDistance;
			}

			PerformBufferedMovement(effectiveCameraDollySpeed, elapsedTime);

			// Detect possible collision with camera near plane bounding sphere
			colDist = FindNearPlaneCollisionDistance(m_targetCamera->GetTargetPosition(), m_targetCamera->GetInverseCameraTransform(), m_targetCamera->GetProjectionTransform());
			effectiveCameraDollySpeed = m_cameraMovementSpeed;

			if(colDist < FLT_MAX)
			{
				effectiveCameraDollySpeed = 2000.0f;
				m_targetCameraToTargetDistance = min(colDist, m_targetCameraToTargetDistance);
			}
			
			PerformBufferedMovement(effectiveCameraDollySpeed, elapsedTime);
		}
	}

	// Chain update call to base controller class
	return IRuEntity_Controller::Update(elapsedTime);
}

REAL CRuController_Camera::GetStartTime() const
{
	return 0.0f;
}

REAL CRuController_Camera::GetDuration() const
{
	return FLT_MAX;
}

BOOL CRuController_Camera::IsLooping() const
{
	return FALSE;
}

IRuObject *CRuController_Camera::GetTarget()
{
	return m_targetCamera;
}

BOOL CRuController_Camera::SetTarget(IRuObject *target)
{
	// Release any old target
	ruSAFE_RELEASE(m_targetCamera);

	// Track new target
	if(target->GetType().IsSubClassOf(CRuCamera::Type()))
	{
		m_targetCamera = static_cast<CRuCamera *>(target);
		m_targetCamera->AddRef();

		UpdatePresetCameraConfiguration();
	}

	return TRUE;
}

void CRuController_Camera::UpdatePresetCameraConfiguration()
{
	m_currentCameraPosition = m_targetCamera->GetTargetPosition();

	CRuVector3 cameraAxis = m_targetCamera->GetCameraPosition() - m_targetCamera->GetTargetPosition();
	m_presetCameraToTargetDistance = cameraAxis.Magnitude();
	m_targetCameraToTargetDistance = m_presetCameraToTargetDistance;
}

void CRuController_Camera::SetCollisionObject(IRuObject *colObject)
{
	if(colObject)
	{
		// Store reference to new collision object
		colObject->AddRef();
		ruSAFE_RELEASE(m_collisionObject);
		m_collisionObject = colObject;
	}
	else
	{
		// Release old collision object
		ruSAFE_RELEASE(m_collisionObject);
	}
}

void CRuController_Camera::SetCollisionExclusionMask(UINT32 mask)
{
	m_collisionExclusionMask = mask;
}

void CRuController_Camera::SetSceneTargetCamera(CRuEntity_Camera *sceneTargetCamera)
{
	if(sceneTargetCamera)
	{
		sceneTargetCamera->AddRef();
		ruSAFE_RELEASE(m_sceneTargetCamera);
		m_sceneTargetCamera = sceneTargetCamera;
	}
}

REAL CRuController_Camera::FindCollisionDistance(const CRuVector3 &targetPosition, const CRuMatrix4x4 &inverseCameraTransform, const CRuMatrix4x4 &projectionTransform)
{
	REAL nearestT = FLT_MAX;

	// Check for collision
	if(m_collisionObject)
	{
		// Make single line-of-sight query from target to camera
		CRuCollisionQuery_Segment_Nearest segQuery;
		segQuery.m_colSegment.SetFromPoints(targetPosition, m_targetCamera->GetCameraPosition());

		if(m_collisionObject->GetType().IsSubClassOf(CRuEntity::Type()))
		{
			static_cast<CRuEntity *>(m_collisionObject)->QueryCollision(&segQuery);
		}
		else if(m_collisionObject->GetType().IsSubClassOf(IRuCollisionHierarchy::Type()))
		{
			static_cast<IRuCollisionHierarchy *>(m_collisionObject)->QueryCollision(&segQuery);
		}

		if(segQuery.m_positiveHit)
		{
			CRuVector3 colVector = segQuery.m_colSegment.Direction() * segQuery.m_collisionT;

			REAL t = colVector.Magnitude();

			if(t < nearestT)
			{
				nearestT = t;
			}
		}
	}

	// Cap distance to some minimum value
	nearestT = max(5.0f, nearestT);

	return nearestT;
}

REAL CRuController_Camera::FindNearPlaneCollisionDistance(const CRuVector3 &targetPosition, const CRuMatrix4x4 &inverseCameraTransform, const CRuMatrix4x4 &projectionTransform)
{
	REAL nearestT = FLT_MAX;

	CRuVector3 curCamPos = m_targetCamera->GetCameraPosition();
	CRuVector3 camAxis = m_targetCamera->GetCameraPosition() - targetPosition;

	if(camAxis.Magnitude() > ruEPSILON)
	{
		camAxis.Normalize();
	}

	// Check for collision
	if(m_collisionObject)
	{
		for(INT32 i = 0; i < 8; ++i)
		{
			CRuCollisionQuery_Sphere_HitTest hitQuery;

			hitQuery.m_colSphere.Set(curCamPos, 2.0f);

			if(m_collisionObject->GetType().IsSubClassOf(CRuEntity::Type()))
			{
				static_cast<CRuEntity *>(m_collisionObject)->QueryCollision(&hitQuery);
			}
			else if(m_collisionObject->GetType().IsSubClassOf(IRuCollisionHierarchy::Type()))
			{
				static_cast<IRuCollisionHierarchy *>(m_collisionObject)->QueryCollision(&hitQuery);
			}

			if(hitQuery.m_positiveHit)
			{
				REAL t = (targetPosition - curCamPos).Magnitude() - 0.5f;
				CRuVector3 colVector = camAxis * t;
				curCamPos = targetPosition + colVector;

				if(t < nearestT)
				{
					nearestT = t;
				}
			}
			else
			{
				break;
			}
		}
	}

	// Cap distance to some minimum value
	nearestT = max(5.0f, nearestT);

	return nearestT;
}

void CRuController_Camera::PerformBufferedMovement(REAL effectiveCameraDollySpeed, REAL elapsedTime)
{
	// Perform buffered operations
	CRuVector3 cameraAxis = m_targetCamera->GetCameraPosition() - m_targetCamera->GetTargetPosition();
	REAL currentCameraToTargetDistance = cameraAxis.Magnitude();

	if(m_targetCameraToTargetDistance != currentCameraToTargetDistance)
	{
		if(currentCameraToTargetDistance < m_targetCameraToTargetDistance)
		{
			// Save original camera position
			CRuVector3 oldCamPos = m_targetCamera->GetCameraPosition();

			// Dolly camera away from target
			Dolly(max(-effectiveCameraDollySpeed * elapsedTime, currentCameraToTargetDistance - m_targetCameraToTargetDistance), FALSE);

			// Check possible collision
			REAL colDist0 = FindCollisionDistance(m_targetCamera->GetTargetPosition(), m_targetCamera->GetInverseCameraTransform(), m_targetCamera->GetProjectionTransform());
			REAL colDist1 = FindNearPlaneCollisionDistance(m_targetCamera->GetTargetPosition(), m_targetCamera->GetInverseCameraTransform(), m_targetCamera->GetProjectionTransform());

			// If there is a collision, undo the dolly
			if(colDist0 < FLT_MAX || colDist1 < FLT_MAX)
			{
				m_targetCamera->SetCameraPosition(oldCamPos);
			}
		}
		else
		{
			// Dolly camera closer to target
			Dolly(min(effectiveCameraDollySpeed * elapsedTime, currentCameraToTargetDistance - m_targetCameraToTargetDistance), FALSE);
		}
	}
}

void CRuController_Camera::Dolly(REAL offset, BOOL buffered)
{
	m_targetCamera->MoveCamera(CRuVector3(0.0f, 0.0f, offset), TRUE);
}

// ************************************************************************************************************************************************************

#pragma managed
