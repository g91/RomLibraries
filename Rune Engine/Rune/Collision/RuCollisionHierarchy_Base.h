#ifndef _RUCOLLISIONHIERARCHY_BASE_H_
#define _RUCOLLISIONHIERARCHY_BASE_H_

// Base types
#include "../Collision/RuCollision_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class IRuCollisionHierarchy : public IRuCollisionObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuCollisionHierarchy();

	// Hierarchy management
	virtual BOOL							AddCollisionObject(IRuCollisionObject *colObject) = 0;		//!< Add a collision object to the hierarchy
	virtual IRuCollisionObject*				GetCollisionObject(INT32 objKey) = 0;						//!< Gets a collision object by its key
	virtual BOOL							RemoveCollisionObject(INT32 objKey) = 0;					//!< Remove a collision object with the given key from the hierarchy
};

// ************************************************************************************************************************************************************

inline IRuCollisionHierarchy::~IRuCollisionHierarchy()
{
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
