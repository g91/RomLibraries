#ifndef _RUCOLLISION_BASE_H_
#define _RUCOLLISION_BASE_H_

// Object and type system
#include "../Core/Type/RuType_Object.h"

// Math system
#include "../Core/Math/RuMath.h"
#include "../Core/Math/RuMathBase.h"
#include "../Core/Math/RuMathVector.h"
#include "../Core/Math/RuMathGeometry.h"
#include "../Core/Math/RuMathCollision.h"
#include "../Core/Math/RuMathSSE.h"

// Base types
#include "../Collision/RuType_Collision.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class IRuCollisionObject : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32									m_collisionObjectKey;										//!< The collision object's search key
	INT32									m_collisionObjectIteration;									//!< The collision object's iteration

public:
	virtual 								~IRuCollisionObject();

	// Collision object key setting
	INT32									GetCollisionObjectKey() const;
	void									SetCollisionObjectKey(INT32 objKey);

	// Collision object iteration setting
	INT32									GetCollisionObjectIteration() const;
	void									SetCollisionObjectIteration(INT32 iteration);

	// Collision object query
	virtual const CRuAABB&					GetBounds() const = 0;										// Query collision object for its bounding box

	// Collision query interface
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery) = 0;			// Perform collision query
};

// ************************************************************************************************************************************************************

inline IRuCollisionObject::~IRuCollisionObject()
{
}

inline INT32 IRuCollisionObject::GetCollisionObjectKey() const
{
	return m_collisionObjectKey;
}

inline void IRuCollisionObject::SetCollisionObjectKey(INT32 objKey)
{
	m_collisionObjectKey = objKey;
}

inline INT32 IRuCollisionObject::GetCollisionObjectIteration() const
{
	return m_collisionObjectIteration;
}

inline void IRuCollisionObject::SetCollisionObjectIteration(INT32 iteration)
{
	m_collisionObjectIteration = iteration;
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
