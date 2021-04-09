/*!
	@file RuEntity_Widget_FrustumVis.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITY_WIDGET_FRUSTUMVIS_H_
#define _RUENTITY_WIDGET_FRUSTUMVIS_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#include "../../Scene/Base/RuEntityContainer.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	Bone visualizer widget

	@version 2005.04.09
*/
class CRuEntity_Widget_FrustumVis : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuCamera*								m_camera;
	CRuCamera*								m_tsmCamera;

	CRuDynamicMesh*							m_frustumMesh;
	CRuMaterial*							m_frustumMaterial;

public:
											CRuEntity_Widget_FrustumVis();
	virtual									~CRuEntity_Widget_FrustumVis();

	void									SetTarget(IRuObject *target);
	void									SetTSMTarget(IRuObject *target);

	// Base entity interface
	virtual CRuEntity*						Clone(CRuEntity *clonedEntity = NULL);
	virtual BOOL							ResetStates();

	// Base interface
	virtual BOOL							Update(float elapsedTime);
/*
	// Required subclass interface
	virtual void							UpdateEntityTransform();
	virtual void							UpdateEntityBounds();
*/
	// Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
