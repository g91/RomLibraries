/*!
	@file RuGizmo_Mover.h

	Copyright (c) 2004-2007 All rights reserved
*/

#ifndef _RUGIZMO_MOVER_H_
#define _RUGIZMO_MOVER_H_

#include "../../Scene/Gizmo/RuGizmo_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	Mover gizmo.

	@version 2006.07.07
*/
class CRuGizmo_Mover : public CRuGizmo_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	enum RuMoverModes
	{
		ruMOVERMODE_MOVE_XZ					= 0,
		ruMOVERMODE_MOVE_Y					= 1,
		ruMOVERMODE_ROTATE_Y				= 2,
		ruMOVERMODE_SCALE					= 3,
		ruMOVERMODE_COUNT					= 4
	};

	IRuObject*								m_collisionObject;

	CRuEntity*								m_modeSelectorEntity;
	CRuEntity*								m_moverEntities[ruMOVERMODE_COUNT];
	RuMoverModes							m_nextMoverMode[ruMOVERMODE_COUNT];

	RuMoverModes							m_moverMode;
	BOOL									m_snapToCollisionObject;

	CRuRay3									m_activePickRay;

public:
											CRuGizmo_Mover();
	virtual									~CRuGizmo_Mover();

	void									SetCollisionObject(IRuObject *colObject);
	void									SetSnapToCollisionObject(BOOL snapToCollisionObject);

	// Base interface
	virtual BOOL							Update(float elapsedTime);

	// Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	// Gizmo queries
	virtual BOOL							SetConstraint(RuGizmoConstraint gizmoConstraint);
	virtual BOOL							HighlightManipulator(const CRuRay3 &pickRay);
	virtual BOOL							ActivateManipulator(const CRuRay3 &pickRay);
	virtual BOOL							Manipulate(const CRuRay3 &pickRay);

protected:
	CRuEntity*								LoadEntityFromBlock(const BYTE *block, INT32 blockLength, const char *identifier);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
