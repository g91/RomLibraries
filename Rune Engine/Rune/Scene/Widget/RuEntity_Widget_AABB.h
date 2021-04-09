/*!
	@file RuEntity_Widget_AABB.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITY_WIDGET_AABB_H_
#define _RUENTITY_WIDGET_AABB_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#include "../../Scene/Base/RuEntityContainer.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	Axis-aligned bounding box visualizing widget.

	@version 2005.09.25
*/
class CRuEntity_Widget_AABB : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuDynamicMesh*							m_mesh;
	CRuMaterial*							m_material;

public:
											CRuEntity_Widget_AABB();
	virtual									~CRuEntity_Widget_AABB();

	void									SetAABB(const CRuAABB &box, RUCOLOR color);

	// Base entity interface
	virtual CRuEntity*						Clone(CRuEntity *clonedEntity = NULL);
	virtual BOOL							ResetStates();

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	// Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
