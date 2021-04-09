/*!
	@project Rune
	@file RuEntity_ColVis.h

	Copyright (c) 2004 All rights reserved

	@author John Tang
	@date 2005/04/08
*/

#ifndef _RUENTITY_COLVIS_H_
#define _RUENTITY_COLVIS_H_

#include "../../Collision/RuCollisionHierarchy_Base.h"
#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	Collision hierarchy visualizer. Used for performing visual navigation through a collision
	data set.

	@author John Tang
	@date 2005/04/08
*/

class CRuEntity_ColVis : public IRuEntity_Container
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuCollisionHierarchy*						m_sourceColHierarchy;									//!< Source collision hierarchy
	CRuLRUQueue<CRuGUID, CRuModel *>*			m_modelQueue;											//!< LRU queue of instanced models
	CRuArrayList<CRuModel *>					m_visibleObjects;										//!< List of visible objects

	UINT32										m_highlightSurfaceID;									//!< Surface ID to highlight
	UINT32										m_highlightSurfaceFlag;									//!<
	UINT32										m_exclusionMask;										//!< Exclusion mask for visibile triangles

public:
												CRuEntity_ColVis();
	virtual 									~CRuEntity_ColVis();

	void										SetSourceCollisionHierarchy(IRuCollisionHierarchy *sourceColHierarchy);

	// IRuObject interface
	virtual IRuObject*							Clone(IRuObject *clonedObject = NULL);

	// Entity interface
	virtual BOOL								ResetStates();
	virtual BOOL								Update(REAL elapsedTime);
/*
	// Required subclass interface
	virtual void								UpdateEntityTransform();
	virtual void								UpdateEntityBounds();
*/
	// IRuEntity_Container interface
	virtual INT32								GetNumEntities();
	virtual CRuEntity*							GetEntity(INT32 index);

	// Collision query interface
	virtual BOOL								QueryCollision(CRuCollisionQuery *colQuery);

	// LOD selection and frustum culling
	void										SetCamera(CRuCamera *viewCamera);

	void										SetHighlightSurfaceID(UINT32 highlightSurfaceID, UINT32 highlightSurfaceFlag);
	void										SetExclusionMask(UINT32 mask);

protected:
	CRuModel*									InstanceModel(IRuCollisionObject *colObject);
	BOOL										FlushModelQueue();
	static BOOL									FlushModelQueueCallback(const void *key, const void *data, void *userData);
};

// ************************************************************************************************************************************************************

extern BOOL										BuildMeshRepresentation(IRuCollisionObject *collisionObj, UINT32 highlightSurfaceID, UINT32 highlightSurfaceFlag, UINT32 exclusionMask, CRuArrayList<IRuMesh *> &meshListOut);
extern IRuMesh*									BuildMeshRepresentation(IRuCollisionObject *collisionObj);

// ************************************************************************************************************************************************************

#pragma managed

#endif
