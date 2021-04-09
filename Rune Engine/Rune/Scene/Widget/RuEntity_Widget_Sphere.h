/*!
	@file RuEntity_Widget_Sphere.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITY_WIDGET_SPHERE_H_
#define _RUENTITY_WIDGET_SPHERE_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#include "../../Scene/Base/RuEntityContainer.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	Sphere visualizer widget.

	@version 2005.08.31
*/
class CRuEntity_Widget_Sphere : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32									m_numSegments;

	CRuDynamicMesh*							m_sphereMesh;
	CRuMaterial*							m_sphereMaterial;

public:
											CRuEntity_Widget_Sphere();
	virtual									~CRuEntity_Widget_Sphere();

	void									SetSegments(INT32 segments);
	void									SetSphere(const CRuSphere &sphere, RUCOLOR color);

	// Base interface
	virtual BOOL							Update(float elapsedTime);

	// Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
