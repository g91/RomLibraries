#ifndef _RUCOLLISION_SIMPLE_H_
#define _RUCOLLISION_SIMPLE_H_

#include "../Collision/RuCollision_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!

*/
class CRuCollision_Simple : public IRuCollisionObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuAABB									m_bounds;

	INT32									m_numColVertices;							//!< Number of collision vertices
	INT32									m_numColPrimitives;							//!< Number of collision primitives

	CRuVector3*								m_colVertices;								//!< Collision vertices
	UINT32*									m_colFlags;									//!< Collision primitive flags
	UINT16*									m_colIndices;								//!< Collision primitive indices

public:
											CRuCollision_Simple();
	virtual 								~CRuCollision_Simple();

	// Serialization interface
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Collision query interface
	virtual const CRuAABB&					GetBounds() const;
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// Collision data query
	INT32									GetNumColVertices() const;
	INT32									GetNumColPrimitives() const;
	CRuVector3*								GetColVertices();
	UINT32*									GetColFlags();
	UINT16*									GetColIndices();

protected:
	BOOL									QueryCollision(CRuCollisionQuery_Ray *rayQuery);
	BOOL									QueryCollision(CRuCollisionQuery_Ray_Pick *rayQuery);
	BOOL									QueryCollision(CRuCollisionQuery_Segment *segmentQuery);
	BOOL									QueryCollision(CRuCollisionQuery_Segment_HitTest *segmentQuery);
	BOOL									QueryCollision(CRuCollisionQuery_Segment_Nearest *segmentQuery);
	BOOL									QueryCollision(CRuCollisionQuery_Enumerate *enumQuery);
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Set_Ray *setQuery);
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Set_Sphere *setQuery);
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Get_Segment *segmentQuery);
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Enumerate *enumQuery);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Collision_Simple, CRuCollision_Simple)

// ************************************************************************************************************************************************************

#pragma managed

#endif
