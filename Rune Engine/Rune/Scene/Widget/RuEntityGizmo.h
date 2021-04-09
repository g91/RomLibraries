/*!
	@file RuEntityGizmo.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITYGIZMO_H_
#define _RUENTITYGIZMO_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	Base class for all gizmos. Gizmos are manipulator objects that act on CRuFrame entities to
	perform scaling, rotation, and translation. Once activated using the ActivateManipulator
	function, changes to the target CRuFrame entity are made by successive calls to the
	Manipulate function. All changes are made directly to the CRuFrame.

	Subclasses must implement the above two functions to provide proper manipulation capabilities.

	@version 2004.12.30
*/
class CRuEntity_Gizmo : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Gizmo on-screen size
	REAL									m_gizmoSize;									//!< On-screen size of the gizmo, in canonical units
	RuFrameType								m_frameType;									//!< Gizmo control frame time

	// Current gizmo status
	CRuCamera*								m_camera;										//!< Camera the gizmo is attached to
	CRuEntity*								m_gizmoTarget;									//!< Target that the gizmo manipulates

	REAL									m_gizmoScale;									//!< Current scale of the gizmo

public:
											CRuEntity_Gizmo();
	virtual									~CRuEntity_Gizmo();

	// Base interface
	virtual BOOL							Update(float elapsedTime);

	// IRuEntity_Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	// Gizmo property settings
	virtual void							SetGizmoSize(float gizmoSize);
	virtual void							SetGizmoTarget(CRuEntity *targetEntity);
	virtual void							SetCamera(CRuCamera *viewCamera);

	// Gizmo queries
	virtual BOOL							ActivateManipulator(const CRuRay3 &pickRay) = 0;
	virtual BOOL							Manipulate(const CRuRay3 &pickRay) = 0;
};

// ************************************************************************************************************************************************************

class CRuEntity_Gizmo_Translation : public CRuEntity_Gizmo
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	enum RuGizmoPart
	{
		ruGIZMOPART_X						= 0,
		ruGIZMOPART_Y						= 1,
		ruGIZMOPART_Z						= 2,
		ruGIZMOPART_XY						= 3,
		ruGIZMOPART_XZ						= 4,
		ruGIZMOPART_YZ						= 5,

		ruGIZMOPART_COUNT					= 6
	};

	// Translation gizmo representation settings
	float									m_arrowLength;									//!< Length of the gizmo arrow, must be in [0, 1]
	float									m_arrowRadius;									//!< Radius of the gizmo arrow
	float									m_shaftOffset;									//!< Offset of the gizmo shaft
	float									m_shaftLength;									//!< Length of the gizmo shaft, must be in [0, 1]
	float									m_shaftRadius;									//!< Radius of the gizmo shaft
	float									m_quadSize;										//!< Size of the gizmo plane quads

	CRuRay3									m_activeGizmoPickRay;
	RuGizmoPart								m_activeGizmoPart;

	// Translation gizmo bounding boxes
	CRuAABB									m_gizmoBounds[ruGIZMOPART_COUNT];				//!< Bounding boxes for each of the gizmo manipulators

	// Translation gizmo representation
	CRuMesh*								m_gizmoMesh;									//!< Mesh for the gizmo
	CRuMaterial*							m_gizmoMaterial;								//!< Material for the gizmo

public:
											CRuEntity_Gizmo_Translation();
	virtual									~CRuEntity_Gizmo_Translation();

	// Base interface
	virtual BOOL							Update(float elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// IRuEntity_Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	// Gizmo queries
	virtual BOOL							ActivateManipulator(const CRuRay3 &pickRay);
	virtual BOOL							Manipulate(const CRuRay3 &pickRay);

protected:
	BOOL									GetManipulationVector(const CRuRay3 &pickRay, CRuVector3 &vectorOut);
};

// ************************************************************************************************************************************************************

class CRuEntity_Gizmo_Rotation : public CRuEntity_Gizmo
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	enum RuRotationGizmoPart
	{
		ruROTGIZMOPART_X					= 0,
		ruROTGIZMOPART_Y					= 1,
		ruROTGIZMOPART_Z					= 2,

		ruROTGIZMOPART_COUNT				= 3
	};

	// Rotation gizmo representation settings
	float									m_torusRadius1;									//!< Radius of the torus ring
	float									m_torusRadius2;									//!< Radius of the torus tube
	INT32									m_torusSegments;
	INT32									m_torusSides;

	CRuRay3									m_activeGizmoPickRay;
	RuRotationGizmoPart						m_activeGizmoPart;

	CRuPlane4								m_gizmoPlanes[ruROTGIZMOPART_COUNT];

	// Translation gizmo representation
	CRuMesh*								m_gizmoMesh;									//!< Mesh for the gizmo
	CRuMaterial*							m_gizmoMaterial;								//!< Material for the gizmo

public:
											CRuEntity_Gizmo_Rotation();
	virtual									~CRuEntity_Gizmo_Rotation();

	// Base interface
	virtual BOOL							Update(float elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// IRuEntity_Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	// Gizmo queries
	virtual BOOL							ActivateManipulator(const CRuRay3 &pickRay);
	virtual BOOL							Manipulate(const CRuRay3 &pickRay);

protected:
	BOOL									GetManipulationRotation(const CRuRay3 &pickRay, CRuQuaternion &rotationOut);
};

// ************************************************************************************************************************************************************

class CRuEntity_Gizmo_Scaling : public CRuEntity_Gizmo
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	enum RuScalingGizmoPart
	{
		ruSCALEGIZMOPART_X					= 0,
		ruSCALEGIZMOPART_Y					= 1,
		ruSCALEGIZMOPART_Z					= 2,
		ruSCALEGIZMOPART_XY					= 3,
		ruSCALEGIZMOPART_XZ					= 4,
		ruSCALEGIZMOPART_YZ					= 5,

		ruSCALEGIZMOPART_COUNT				= 6
	};

	// Scaling gizmo representation settings
	float									m_arrowLength;									//!< Length of the gizmo arrow, must be in [0, 1]
	float									m_arrowRadius;									//!< Radius of the gizmo arrow
	float									m_shaftOffset;									//!< Offset of the gizmo shaft
	float									m_shaftLength;									//!< Length of the gizmo shaft, must be in [0, 1]
	float									m_shaftRadius;									//!< Radius of the gizmo shaft
	float									m_quadSize;										//!< Size of the gizmo plane quads

	CRuRay3									m_activeGizmoPickRay;
	RuScalingGizmoPart						m_activeGizmoPart;

	// Scaling gizmo bounding boxes
	CRuAABB									m_gizmoBounds[ruSCALEGIZMOPART_COUNT];			//!< Bounding boxes for each of the gizmo manipulators

	// Scaling gizmo representation
	CRuMesh*								m_gizmoMesh;									//!< Mesh for the gizmo
	CRuMaterial*							m_gizmoMaterial;								//!< Material for the gizmo

public:
											CRuEntity_Gizmo_Scaling();
	virtual									~CRuEntity_Gizmo_Scaling();

	// Base interface
	virtual BOOL							Update(float elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// IRuEntity_Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	// Gizmo queries
	virtual BOOL							ActivateManipulator(const CRuRay3 &pickRay);
	virtual BOOL							Manipulate(const CRuRay3 &pickRay);

protected:
	BOOL									GetManipulationScale(const CRuRay3 &pickRay, CRuVector3 &scaleOut);
};

// ************************************************************************************************************************************************************

/*!
	A gizmo used for height (screen y-axis) adjustment. This gizmo has no visual representation and so can be
	activated from anywhere on the screen. A typical use might be for adjusting a terrain height field. User
	clicks on a point on the terrain to activate the gizmo, then drags up or down to adjust the height. Since
	the gizmo manipulates a CRuFrame, the code would actually need to read back the y translation value from
	the frame in order to alter the height field.

	@version 2004.12.31
*/
class CRuEntity_Gizmo_Height : public CRuEntity_Gizmo
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuRay3									m_activeGizmoPickRay;
	float									m_heightDelta;

public:
											CRuEntity_Gizmo_Height();
	virtual									~CRuEntity_Gizmo_Height();

	// Gizmo queries
	virtual BOOL							ActivateManipulator(const CRuRay3 &pickRay);
	virtual BOOL							Manipulate(const CRuRay3 &pickRay);

	float									GetDelta();
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
