/*!
	@file RuContainer_Collision.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../../Scene/Container/RuContainer_Collision.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuContainer_Collision, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(CRuContainer_Collision, "CRuContainer_Collision", "CRuEntity")
ruCLASSGUID_DEFINE(CRuContainer_Collision, 0x62D26E8A, 0x2E7B47ed, 0x9651017A, 0xD5407DEA)

// ************************************************************************************************************************************************************

CRuContainer_Collision::CRuContainer_Collision()
:	m_containerIteration(0)
{
}

CRuContainer_Collision::~CRuContainer_Collision()
{
	ClearCollisionObjects();
}

CRuEvent &CRuContainer_Collision::Event_Changed()
{
	return m_event_Changed;
}

BOOL CRuContainer_Collision::InsertCollisionObject(IRuObject *obj, const CRuSphere &objBounds, BOOL transient)
{
	if(obj)
	{
		// Retrieve existing collision object descriptor if available
		ColObjDesc *oldColObjDesc = GetColObjDesc(obj);

		if(oldColObjDesc)
		{
			// Detect whether the object has moved
			if((oldColObjDesc->m_objectBounds.Center() - objBounds.Center()).Magnitude() > ruEPSILON || oldColObjDesc->m_objectBounds.Radius() != objBounds.Radius())
			{
				// Save new object bounds
				oldColObjDesc->m_objectBounds = objBounds;

				// Increment container iteration
				++m_containerIteration;

				// Trigger container updated event
				RuColContainerEvent_Changed_Args changedArgs(objBounds);
				m_event_Changed.Trigger(&changedArgs);
			}

			// Update TLL
			oldColObjDesc->m_ttl = (transient ? 2 : 0);

			return TRUE;
		}
		else
		{
			// NOTE: Transient objects live for 2 cycles
			ColObjDesc colObjDesc = { obj, objBounds, (transient ? 2 : 0) };

			colObjDesc.m_object->AddRef();
			m_collisionObjects.Add(colObjDesc);

			// Increment container iteration
			++m_containerIteration;

			// Trigger container updated event
			RuColContainerEvent_Changed_Args changedArgs(objBounds);
			m_event_Changed.Trigger(&changedArgs);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuContainer_Collision::RemoveCollisionObject(IRuObject *obj)
{
	if(obj)
	{
		for(INT32 i = 0; i < m_collisionObjects.Count(); ++i)
		{
			if(m_collisionObjects[i].m_object == obj)
			{
				// Trigger container updated event
				RuColContainerEvent_Changed_Args changedArgs(m_collisionObjects[i].m_objectBounds);
				m_event_Changed.Trigger(&changedArgs);

				// Release local reference to object
				ruSAFE_RELEASE(m_collisionObjects[i].m_object);

				// Remove object from list
				m_collisionObjects.RemoveAt(i);

				// Increment container iteration
				++m_containerIteration;

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CRuContainer_Collision::ClearCollisionObjects()
{
	for(INT32 i = 0; i < m_collisionObjects.Count(); ++i)
	{
		ruSAFE_RELEASE(m_collisionObjects[i].m_object);
	}

	m_collisionObjects.Clear();

	// Increment container iteration
	++m_containerIteration;

	return TRUE;
}

INT32 CRuContainer_Collision::GetContainerIteration()
{
	return m_containerIteration;
}

BOOL CRuContainer_Collision::Update(float elapsedTime)
{
	for(INT32 i = 0; i < m_collisionObjects.Count(); ++i)
	{
		// If the collision object is transient, remove it
		if(m_collisionObjects[i].m_ttl > 0)
		{
			// Decrement TTL
			--m_collisionObjects[i].m_ttl;

			// If TTL is up, remove the object
			if(m_collisionObjects[i].m_ttl == 0)
			{
				// Trigger container updated event
				RuColContainerEvent_Changed_Args changedArgs(m_collisionObjects[i].m_objectBounds);
				m_event_Changed.Trigger(&changedArgs);

				// Release local reference to object
				ruSAFE_RELEASE(m_collisionObjects[i].m_object);

				// Remove object from list
				m_collisionObjects.RemoveAt(i);
				--i;

				// Increment container iteration
				++m_containerIteration;
			}
		}
	}

	return TRUE;
}

BOOL CRuContainer_Collision::QueryCollision(CRuCollisionQuery *colQuery)
{
	for(INT32 i = 0; i < m_collisionObjects.Count(); ++i)
	{
		// Perform collision query against current object
		IRuObject *curObj = m_collisionObjects[i].m_object;
		BOOL retVal = TRUE;

		if(curObj->GetType().IsSubClassOf(CRuEntity::Type()))
		{
			retVal = static_cast<CRuEntity *>(curObj)->QueryCollision(colQuery);
		}
		else if(curObj->GetType().IsSubClassOf(IRuCollisionObject::Type()))
		{
			retVal = static_cast<IRuCollisionObject *>(curObj)->QueryCollision(colQuery);
		}

		// If return value is false, abort and return
		if(retVal == FALSE)
		{
			return FALSE;
		}
	}

	return TRUE;
}

CRuContainer_Collision::ColObjDesc *CRuContainer_Collision::GetColObjDesc(IRuObject *obj)
{
	for(INT32 i = 0; i < m_collisionObjects.Count(); ++i)
	{
		if(m_collisionObjects[i].m_object == obj)
		{
			return &m_collisionObjects[i];
		}
	}

	return NULL;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
