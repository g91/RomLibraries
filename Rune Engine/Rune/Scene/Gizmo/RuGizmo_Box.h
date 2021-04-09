/*!
	@file RuGizmo_Box.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITY_GIZMO_BOX_H_
#define _RUENTITY_GIZMO_BOX_H_

#include "../../Scene/Gizmo/RuGizmo_Base.h"
#include "../../Scene/Base/RuEntityModel.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Axis-aligned bounding box visualizing widget.

	@version 2005.09.25
*/
class CRuGizmo_Box : public CRuGizmo_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	RUCOLOR									m_boxColor;										//!< Box display color
	CRuOBB									m_box;											//!< Box parameters

	CRuDynamicMesh*							m_mesh;
	CRuMaterial*							m_material;

	CRuDynamicMesh*							m_boxMesh;
	CRuMaterial*							m_boxMaterial;

	RuGizmoConstraint						m_gizmoConstraint;
	CRuRay3									m_activeGizmoPickRay;

	INT32									m_activeControlCylinder;
	INT32									m_highlightControlCylinder;
	CRuCylinder								m_controlCylinders[12];
	REAL									m_controlCylinderRadii[12];

	REAL									m_constraintMultipliers[ruGIZMOCONSTRAINT_COUNT][12][3];

public:
	CRuVector3								m_temp;

public:
											CRuGizmo_Box();
	virtual									~CRuGizmo_Box();

	void									SetBoxColor(RUCOLOR boxColor);

	const CRuOBB&							GetBox() const;
	void									SetBox(const CRuOBB &box);

	// Base entity interface
	virtual CRuEntity*						Clone(CRuEntity *clonedEntity = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							ResetStates();

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	// Gizmo queries
	virtual BOOL							SetConstraint(RuGizmoConstraint gizmoConstraint);
	virtual BOOL							HighlightManipulator(const CRuRay3 &pickRay);
	virtual BOOL							ClearManipulatorHighlight();
	virtual BOOL							ActivateManipulator(const CRuRay3 &pickRay);
	virtual BOOL							Manipulate(const CRuRay3 &pickRay);

	// Event handlers
	BOOL									HandleEvent_Camera(RuEventArgs *eventArgs);

	// Operator overloads
	CRuGizmo_Box&							operator=(const CRuGizmo_Box &src);

protected:
	BOOL									GetManipulationVector(const CRuRay3 &pickRay, CRuVector3 &vectorOut);
	void									UpdateGizmoMesh();

	void									NormalizeBox();
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
