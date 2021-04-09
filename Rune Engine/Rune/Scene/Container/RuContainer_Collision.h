/*!
	@file RuContainer_Collision.h

	Copyright (c) 2004-2008 All rights reserved
*/

#pragma once

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class RuColContainerEvent_Changed_Args : public RuEventArgs
{
public:
	CRuSphere								m_changeBounds;					//!< Bounding radius of change

	RuColContainerEvent_Changed_Args(const CRuSphere &changeBounds)
	:	m_changeBounds(changeBounds)
	{
	}
};


// ************************************************************************************************************************************************************

/*!
	Collision object container. Used to store a collection of objects or
	entities which are collision targets.

	@version 2008.09.17
*/
class CRuContainer_Collision : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct ColObjDesc
	{
		IRuObject*							m_object;
		CRuSphere							m_objectBounds;
		INT32								m_ttl;
	};

	CRuEvent								m_event_Changed;

	CRuArrayList<ColObjDesc>				m_collisionObjects;

	INT32									m_containerIteration;

public:
											CRuContainer_Collision();
	virtual									~CRuContainer_Collision();

	CRuEvent&								Event_Changed();

	BOOL									InsertCollisionObject(IRuObject *obj, const CRuSphere &objBounds, BOOL transient = FALSE);
	BOOL									RemoveCollisionObject(IRuObject *obj);
	BOOL									ClearCollisionObjects();

	INT32									GetContainerIteration();

	// Base interface
	virtual BOOL							Update(float elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

protected:
	ColObjDesc*								GetColObjDesc(IRuObject *obj);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
