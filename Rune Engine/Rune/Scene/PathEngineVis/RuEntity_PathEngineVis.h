/*!
	@project Rune
	@file RuEntity_PathEngineVis.h

	Copyright (c) 2004 All rights reserved

	@author John Tang
	@date 2006/01/03
*/

#ifndef _RUENTITY_PATHENGINEVIS_H_
#define _RUENTITY_PATHENGINEVIS_H_

#include "../../PathEngine/RuPathHierarchy_Base.h"
#include "../../Scene/PathEngineVis/RuEntity_PathGraphVis.h"

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityContainer.h"
#include "../../Scene/Base/RuEntityModel.h"
#include "../../UI/RuUI_Font.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Collision hierarchy visualizer. Used for performing visual navigation through a collision
	data set.

	@author John Tang
	@date 2005/04/08
*/

class CRuEntity_PathEngineVis : public IRuEntity_Container
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuFontType*								m_fontType;

	IRuPathHierarchy_Base*						m_sourcePathHierarchy;									//!< Source path hierarchy
	CRuLRUQueue<CRuGUID, CRuEntity *>*			m_modelQueue;											//!< LRU queue of instanced models
	CRuArrayList<CRuEntity *>					m_visibleObjects;										//!< List of visible objects

	CRuRay3										m_highlightRay;
	INT32										m_selectedNodeKey;
	CRuFrame*									m_pathDisplayEntity;

public:
												CRuEntity_PathEngineVis();
	virtual 									~CRuEntity_PathEngineVis();

	void										SetFontType(CRuFontType *fontType);
	void										SetSourcePathHierarchy(IRuPathHierarchy_Base *sourcePathHierarchy);

	void										SetHighlightRay(const CRuRay3 &highlightRay);
	INT32										GetNodeKeyAtCursor(const CRuRay3 &ray);

	// IRuObject interface
	virtual IRuObject*							Clone(IRuObject *clonedObject = NULL);

	// Entity interface
	virtual BOOL								ResetStates();
	virtual BOOL								Update(REAL elapsedTime);

	// IRuEntity_Container interface
	virtual INT32								GetNumEntities();
	virtual CRuEntity*							GetEntity(INT32 index);

	// Collision query interface
	virtual BOOL								QueryCollision(CRuCollisionQuery *colQuery);

	// LOD selection and frustum culling
	void										SetCamera(CRuCamera *viewCamera);

	BOOL										FlushModelQueue();

protected:
	CRuEntity*									InstanceModel(IRuPathGraph_Base *pathGraph);
	static BOOL									FlushModelQueueCallback(const void *key, const void *data, void *userData);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
