/*!
	@file RuGizmo_Plane.h

	Copyright (c) 2004-2006 All rights reserved
*/

#ifndef _RUGIZMO_PLANE_H_
#define _RUGIZMO_PLANE_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	Plane gizmo.

	@version 2006.07.07
*/
class CRuGizmo_Plane : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	enum RuGizmoHandle
	{
		ruGIZMOHANDLE_X0Z0						= 0,
		ruGIZMOHANDLE_X1Z0						= 1,
		ruGIZMOHANDLE_X0Z1						= 2,
		ruGIZMOHANDLE_X1Z1						= 3,
		ruGIZMOHANDLE_X0						= 4,
		ruGIZMOHANDLE_X1						= 5,
		ruGIZMOHANDLE_Z0						= 6,
		ruGIZMOHANDLE_Z1						= 7,

		ruGIZMOHANDLE_COUNT						= 8
	};

	REAL									m_gizmoHandleSize;								//!< Size of the gizmo handle
	CRuAABB									m_planeBounds;

	// Gizmo handle bounding boxes
	CRuAABB									m_gizmoHandles[ruGIZMOHANDLE_COUNT];			//!< Bounding boxes for each of the gizmo handles

	CRuRay3									m_activeGizmoPickRay;
	RuGizmoHandle							m_activeGizmoHandle;

	CRuMesh*								m_mesh;
	CRuMaterial*							m_material;

public:
											CRuGizmo_Plane();
	virtual									~CRuGizmo_Plane();

	// IRuObject interface
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Base interface
	virtual BOOL							Update(float elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// Local entity bounds callback
	virtual void							CalculateEntityBounds();

	// Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	// Gizmo queries
	virtual BOOL							ActivateManipulator(const CRuRay3 &pickRay);
	virtual BOOL							Manipulate(const CRuRay3 &pickRay);

	// Plane queries
	const CRuAABB&							GetPlaneBounds();

protected:
	void									UpdateGizmoMesh();
	void									UpdateGizmoHandles();
	BOOL									GetManipulationVector(const CRuRay3 &pickRay, CRuVector3 &vectorOut);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
