/*!
	@file RuEntity_Widget_BoneVis.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITY_WIDGET_BONEVIS_H_
#define _RUENTITY_WIDGET_BONEVIS_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#include "../../Scene/Base/RuEntityContainer.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	Bone visualizer widget

	@version 2005.04.09
*/
class CRuEntity_Widget_BoneVis : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuFrameHierarchy*						m_targetHierarchy;

	CRuDynamicMesh*							m_boneMesh;
	CRuMaterial*							m_boneMaterial;
	CRuModel*								m_boneModel;

public:
											CRuEntity_Widget_BoneVis();
	virtual									~CRuEntity_Widget_BoneVis();

	void									SetTarget(CRuEntity *target);

	// Base entity interface
	virtual CRuEntity*						Clone(CRuEntity *clonedEntity = NULL);
	virtual BOOL							ResetStates();

	// Base interface
	virtual BOOL							Update(float elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
