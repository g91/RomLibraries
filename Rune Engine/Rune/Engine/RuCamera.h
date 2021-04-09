#ifndef _RUCAMERA_H_
#define _RUCAMERA_H_

// Rune system core
#include "../Engine/Base/RuCore.h"
#include "../Core/Math/RuODESolver_RK4.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuCamera : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
public:
	// Frustum setting
	BOOL								m_parallelProjection;			//!< Is the projection a parallel projection?
	REAL								m_nearPlaneDistance;
	REAL								m_farPlaneDistance;
	REAL								m_viewportWidth;
	REAL								m_viewportHeight;
	REAL								m_aspectRatio;					// Width-to-height aspect ratio
	REAL								m_orientation;					//!< Orientation (used for oblique projections)

	// Backdrop setting
	BOOL								m_backdropEnabled;				//!< Backdrop frustum enabled?
	REAL								m_backdropNearDistance;			//!< Backdrop near plane distance
	REAL								m_backdropFarDistance;			//!< Backdrop far plane distance

	// Movement setting
	BOOL								m_ySmoothEnabled;
	REAL								m_targetY;

	// Derived frustum properties
	REAL								m_sweepRadius;					// Radius of the circle which encloses the entire frustum as it sweeps
	CRuAABB								m_sweepBounds;					// Bounding box of the circle which encloses the entire frustum as it sweeps

	REAL								m_backdropSweepRadius;
	REAL								m_backdropExclusionRadius;
	CRuAABB								m_backdropSweepBounds;
	CRuAABB								m_backdropExclusionBounds;

	// Camera orientation
	REAL								m_transitionDuration;
	REAL								m_transitionT;
	CRuVector3							m_cameraPosOld;
	CRuVector3							m_cameraPosNew;

	CRuVector3							m_cameraPos;					// Camera position
	CRuVector3							m_targetPos;					// Viewing target of the camera
	CRuVector3							m_cameraRight;					// Camera right-direction vector
	CRuVector3							m_cameraUp;						// Camera up-direction vector
	CRuVector3							m_cameraLookAt;					// Camera look-at vector

	BOOL								m_reflectCamera;				// Reflect the camera about the reflection plane?
	CRuPlane4							m_reflectPlane;					// Reflection plane about which the camera reflects the scene

	// Jiggle parameters
	CRuODESolver_RK4*					m_rk4Solver;
	CRuVector3							m_jiggle_Direction;
	CRuVector3							m_jiggle_Offset;
	REAL								m_jiggle_m;
	REAL								m_jiggle_R;
	REAL								m_jiggle_k;
	REAL								m_jiggle_b;
	REAL								m_jiggle_tDeltaLimit;

	// Camera transforms
	BOOL								m_transformsDirty;				//!< Are the transforms dirty?

	CRuMatrix4x4						m_projectionTransform;			//!< Projection transform
	CRuMatrix4x4						m_inverseProjectionTransform;	//!< Inverse projection transform
	CRuMatrix4x4						m_cameraTransform;				//!< Camera(view) transform
	CRuMatrix4x4						m_inverseCameraTransform;		//!< Inverse camera transform
	CRuMatrix4x4						m_frustumTransform;				//!< Frustum(projection * view) transform
	CRuMatrix4x4						m_inverseFrustumTransform;		//!< Inverse frustum transform
	CRuMatrix4x4						m_backdropProjectionTransform;	//!<
	CRuMatrix4x4						m_backdropFrustumTransform;		//!<
	CRuMatrix4x4						m_extendedProjectionTransform;	//!<
	CRuMatrix4x4						m_extendedFrustumTransform;		//!<

	// Frustum planes
	CRuPlane4							m_frustumPlanes[6];
	CRuPlane4							m_backdropFrustumPlanes[6];
	CRuPlane4							m_extendedFrustumPlanes[6];

public:
										CRuCamera();
	virtual								~CRuCamera();

	virtual void						Update(REAL deltaT);
	virtual void						SetCameraPositionSoft(const CRuVector3 &position, REAL duration);

	virtual const CRuVector3&			GetCameraPosition();
	virtual const CRuVector3&			GetTargetPosition();

	virtual const CRuVector3&			GetCameraRightVector();
	virtual const CRuVector3&			GetCameraUpVector();
	virtual const CRuVector3&			GetCameraLookAtVector();

	virtual REAL						GetFarPlaneDistance();
	virtual REAL						GetViewportWidth();
	virtual REAL						GetViewportHeight();

	// Frustum shape
	virtual void						SetObliqueProjection(REAL nearPlane, REAL farPlane, REAL viewportWidth, REAL viewportHeight, REAL orientation);
	virtual void						SetOrthographicProjection(REAL nearPlane, REAL farPlane, REAL viewportWidth, REAL viewportHeight);
	virtual void						SetPerspectiveProjection(REAL nearPlane, REAL farPlane, REAL viewportWidth, REAL viewportHeight);
	virtual void						SetPerspectiveProjectionFOV(REAL nearPlane, REAL farPlane, REAL FOV, REAL aspectRatio);
	virtual void						SetViewDistance(REAL farPlane);

	// Backdrop
	virtual BOOL						IsBackdropEnabled() const;
	virtual void						SetBackdropDistance(REAL nearPlane, REAL farPlane);

	// settings
	BOOL								EnableYSmoothing(BOOL enable);

	// Shadow stuff
	void								LinCombVector3(const CRuVector3 &pos, const CRuVector3 &dir, REAL t, CRuVector3 &result);
	INT32								ClipTest(REAL p, REAL q, REAL *u1, REAL *u2);
	INT32								IntersectionLineAABox(const CRuVector3 &p, const CRuVector3 &dir, const CRuAABB &b, CRuVector3 &vOut);

	void								CalculateViewFrustumPoints(CRuCamera *viewCamera, CRuVector3 *pointsOut);
	void								CalculateViewFrustumGeometry(CRuVector3 *points, CRuArrayList<CRuArrayList<CRuVector3> *> &frustumGeometryOut);	
	void								IncludeObjectLightVolume(CRuCamera *viewCamera, CRuArrayList<CRuArrayList<CRuVector3> *> &frustumGeometry, CRuArrayList<CRuVector3> &pointsOut);
	void								CalculateFocusedLightVolumePoints(CRuCamera *viewCamera, CRuArrayList<CRuVector3> &pointsOut);

	void								AppendToCubicHull(const CRuVector3 &v, CRuVector3 &minima, CRuVector3 &maxima);
	void								CalculateCubicHull(CRuArrayList<CRuVector3> &points, CRuVector3 &minima, CRuVector3 &maxima);
	void								ScaleTranslateToFit(const CRuVector3 &vMin, const CRuVector3 &vMax, CRuMatrix4x4 &transformOut);

	virtual void						SetUniformSMProjection(CRuCamera *viewCamera, const CRuVector3 &lightVec);
	virtual void						SetLiSPSMProjection(CRuCamera *viewCamera, const CRuVector3 &lightVec);

	virtual void						CalculateTrapezoid(CRuCamera *viewCamera, CRuVector3 &t0, CRuVector3 &t1, CRuVector3 &t2, CRuVector3 &t3, CRuVector3 &apexOut, BOOL worldSpace = FALSE);
	virtual void						SetTSMProjection(CRuCamera *viewCamera);

	// Camera frustum/volume
	virtual const CRuPlane4*			GetFrustumPlanes();
	virtual const CRuPlane4*			GetBackdropFrustumPlanes();
	virtual const CRuPlane4*			GetExtendedFrustumPlanes();

	virtual REAL						GetSweepRadius();
	virtual REAL						GetBackdropSweepRadius();
	virtual REAL						GetBackdropExclusionRadius();

	virtual REAL						GetBackdropNearDistance();
	virtual REAL						GetBackdropFarDistance();

	virtual const CRuAABB&				GetSweepBounds();
	virtual const CRuAABB&				GetBackdropSweepBounds();
	virtual const CRuAABB&				GetBackdropExclusionBounds();

	// Orientation
	virtual void						SetCameraPosition(const CRuVector3 &position);
	virtual void						SetTargetPosition(const CRuVector3 &position);
	virtual void						SetCameraUpVector(const CRuVector3 &upVector);
	virtual void						Move(const CRuVector3 &movementVector, BOOL relativeMove);
	virtual void						MoveCamera(const CRuVector3 &movementVector, BOOL relativeMove);
	virtual void						Cycle(REAL cycleAngle, const CRuVector3 &cycleAxis);
	virtual void						Rotate(REAL rotationAngle, const CRuVector3 &rotationAxis);
	virtual void						Reflect(BOOL reflectOn, const CRuPlane4 &reflectPlane);

	// Special effects
	void								Jiggle(REAL strength, REAL springConstant, REAL dampingFactor);

	// Pick ray
	virtual BOOL						Coord_ScreenToHomogeneous(INT32 sX, INT32 sY, INT32 viewportWidth, INT32 viewportHeight, CRuVector3 &ptOut);
	virtual BOOL						Coord_HomogeneousToView(CRuVector3 &pt, CRuVector3 &ptOut);
	virtual BOOL						Coord_ViewToWorld(CRuVector3 &pt, CRuVector3 &ptOut);

	// Utility
	virtual REAL						Utility_GetScreenToWorldScale(const CRuVector3 &origin);
	virtual void						Utility_GetBillboardTransform(const CRuVector3 &geometryForward, const CRuVector3 &geometryUp, CRuMatrix4x4 &matOut);

	// Transform retrieval functions
	virtual const CRuMatrix4x4&			GetProjectionTransform();			// Gets the projection transform
	virtual const CRuMatrix4x4&			GetInverseProjectionTransform();	// Gets the inverse projection transform
	virtual const CRuMatrix4x4&			GetCameraTransform();				// Gets the view transform
	virtual const CRuMatrix4x4&			GetInverseCameraTransform();		// Gets the inverse view transform
	virtual const CRuMatrix4x4&			GetFrustumTransform();				// Gets the frustum(projection * view) transform
	virtual const CRuMatrix4x4&			GetInverseFrustumTransform();		// Gets the inverse frustum transform
	virtual const CRuMatrix4x4&			GetBackdropProjectionTransform();
	virtual const CRuMatrix4x4&			GetBackdropFrustumTransform();
	virtual const CRuMatrix4x4&			GetExtendedProjectionTransform();
	virtual const CRuMatrix4x4&			GetExtendedFrustumTransform();

	virtual BOOL						IsCameraReflected();

	virtual CRuCamera&					operator=(const CRuCamera &camera);

protected:
	BOOL								UpdateBasis();
	BOOL								UpdateTransforms();
	BOOL								GetFrustumPlanes(const CRuMatrix4x4 &frustumMatrix, CRuPlane4 *planesOut);


	REAL JiggleDiffEq0(REAL t, REAL *x)
	{
		return x[1];
	}

	REAL JiggleDiffEq1(REAL t, REAL *x)
	{
		// v' = -(k / m) (x - R) - (b / m) v
		REAL r = -m_jiggle_k * (x[0] - 0.0f - m_jiggle_R) - m_jiggle_b * x[1];
		return r / m_jiggle_m;
	}
};

// ************************************************************************************************************************************************************

inline CRuCamera::CRuCamera()
:	m_nearPlaneDistance(1.0f),
	m_farPlaneDistance(1000.0f),
	m_viewportWidth(2.0f),
	m_viewportHeight(2.0f),
	m_aspectRatio(1.0f),
	m_backdropEnabled(FALSE),
	m_cameraPos(0.0f, 0.0f, 0.0f),
	m_targetPos(0.0f, 0.0f, 1.0f),
	m_cameraUp(0.0f, 1.0f, 0.0f),
	m_reflectCamera(FALSE),
	m_transformsDirty(TRUE)
{
	m_ySmoothEnabled = FALSE;
	m_targetY = 0.0f;

	m_transitionT = -1.0f;

	// Setup RK4 solver for spring motion
	m_rk4Solver = ruNEW CRuODESolver_RK4();
	m_rk4Solver->SetNumValues(2);
	m_rk4Solver->SetNumDiffEqs(2);
	m_rk4Solver->SetDiffEq(0, boost::bind(&CRuCamera::JiggleDiffEq0, this, _1, _2));
	m_rk4Solver->SetDiffEq(1, boost::bind(&CRuCamera::JiggleDiffEq1, this, _1, _2));

	// Initialize solver values to zero
	REAL solverValues[2] = { 0.0f, 0.0f };
	m_rk4Solver->SetValues(solverValues);

	// Preset spring constants
	m_jiggle_m = 1.0f;
	m_jiggle_R = 0.0f;
	m_jiggle_k = 300.0f;
	m_jiggle_b = 10.0f;
	m_jiggle_tDeltaLimit = 0.10f;
}

inline CRuCamera::~CRuCamera()
{
	delete m_rk4Solver;
}

inline const CRuVector3 &CRuCamera::GetCameraPosition()
{
	return m_cameraPos;
}

inline const CRuVector3 &CRuCamera::GetTargetPosition()
{
	return m_targetPos;
}

inline const CRuVector3 &CRuCamera::GetCameraRightVector()
{
	return m_cameraRight;
}

inline const CRuVector3 &CRuCamera::GetCameraUpVector()
{
	return m_cameraUp;
}

inline const CRuVector3 &CRuCamera::GetCameraLookAtVector()
{
	return m_cameraLookAt;
}

inline REAL CRuCamera::GetFarPlaneDistance()
{
	return m_farPlaneDistance;
}

inline REAL CRuCamera::GetViewportWidth()
{
	return m_viewportWidth;
}

inline REAL CRuCamera::GetViewportHeight()
{
	return m_viewportHeight;
}

inline const CRuMatrix4x4 &CRuCamera::GetProjectionTransform()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_projectionTransform;
}

inline const CRuMatrix4x4 &CRuCamera::GetInverseProjectionTransform()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_inverseProjectionTransform;
}

inline const CRuMatrix4x4 &CRuCamera::GetCameraTransform()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_cameraTransform;
}

inline const CRuMatrix4x4 &CRuCamera::GetInverseCameraTransform()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_inverseCameraTransform;
}

inline const CRuMatrix4x4 &CRuCamera::GetFrustumTransform()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_frustumTransform;
}

inline const CRuMatrix4x4 &CRuCamera::GetInverseFrustumTransform()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_inverseFrustumTransform;
}

inline BOOL CRuCamera::IsCameraReflected()
{
	return m_reflectCamera;
}

inline const CRuMatrix4x4 &CRuCamera::GetBackdropProjectionTransform()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_backdropProjectionTransform;
}

inline const CRuMatrix4x4 &CRuCamera::GetBackdropFrustumTransform()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_backdropFrustumTransform;
}

inline const CRuMatrix4x4 &CRuCamera::GetExtendedProjectionTransform()
{
	if(m_transformsDirty)
		UpdateTransforms();

	if(m_backdropEnabled)
	{
		return m_extendedProjectionTransform;
	}
	else
	{
		return m_projectionTransform;;
	}
}

inline const CRuMatrix4x4 &CRuCamera::GetExtendedFrustumTransform()
{
	if(m_transformsDirty)
		UpdateTransforms();

	if(m_backdropEnabled)
	{
		return m_extendedFrustumTransform;
	}
	else
	{
		return m_frustumTransform;
	}
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
