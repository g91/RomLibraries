#pragma once

// Base types
#include "../Collision/RuCollisionHierarchy_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuCollisionHierarchy_Simple : public IRuCollisionHierarchy
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuArrayList<IRuCollisionObject *>			m_colObjects;
	CRuAABB										m_bounds;

public:
												CRuCollisionHierarchy_Simple();
	virtual										~CRuCollisionHierarchy_Simple();

	// IRuObject interface
	virtual IRuObject*							Clone(IRuObject *clonedObject = NULL);
	virtual BOOL								SerializeFrom(IRuStream *stream);
	virtual BOOL								SerializeTo(IRuStream *stream);

	// Collision object query
	virtual const CRuAABB&						GetBounds() const;

	// Collision query interface
	virtual BOOL								QueryCollision(CRuCollisionQuery *colQuery);

	// Hierarchy management
	virtual BOOL								AddCollisionObject(IRuCollisionObject *colObject);
	virtual IRuCollisionObject*					GetCollisionObject(INT32 objKey);
	virtual BOOL								RemoveCollisionObject(INT32 objKey);

protected:
	void										CalculateBounds();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_CollisionHierarchy_Simple, CRuCollisionHierarchy_Simple)

// ************************************************************************************************************************************************************

#pragma managed(pop)
