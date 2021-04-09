#ifndef _RUCOLLISION_KDTREE_16_H_
#define _RUCOLLISION_KDTREE_16_H_

#include "../Collision/RuCollision_Base.h"
#include "../Core/Type/RuType_BitField.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!

*/
class CRuCollision_kdTree_16 : public IRuCollisionObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	/*!
		Internal union specifying data for a kd-tree node.
		
		The lower two bits of the union specifies the node type.

			00 = Internal node split along x
			01 = Internal node split along y
			10 = Internal node split along z
			11 = Leaf node

		If the node is an internal node, m_splitVal is used as its node data and specifies the
		distance of the splitting plane. If the node is a leaf node, the upper 30 bits specify
		the leaf node index.

		@author John Tang
		@version 2006/01/09
	*/
	union kdNode
	{
		REAL								m_splitVal;				//!< Non-Leaf, specifies splitting plane value
		INT32								m_leafIndex;			//!< Leaf, specifies leaf index
	};

	struct kdLeafData
	{
		INT32								m_numTriangles;			//!< Number of triangles in the leaf
		UINT16*								m_triFaces;				//!< Indices into master triangle array to triangle faces for this leaf
	};

	INT32									m_numVertices;			//!< Total number of vertices in the tree
	INT32									m_numTriangles;			//!< Total number of triangles in the tree
	CRuVector3*								m_triVertices;			//!< Master triangle vertex array
	UINT16*									m_triIndices;
	UINT32*									m_triFlags;				//!< Triangle flags
																	//!< | 3 3 2 2 2 2 2 2 2 2 2 2 | 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0 |
																	//!< | 1 0 9 8 7 6 5 4 3 2 1 0 | 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 |
																	//!< ---------------------------------------------------------------------
																	//!< |                         |                   ID                    |

	CRuAABB									m_bounds;
	INT32									m_numNodes;
	INT32									m_numLeaves;
	kdNode*									m_nodeArray;
	kdLeafData*								m_leafArray;

	CRuBitField								m_bitField;

public:

											CRuCollision_kdTree_16();
	virtual 								~CRuCollision_kdTree_16();

	void									BuildTree(const CRuVector3 *triVertices, const UINT16 *triIndices, const UINT32 *triFlags, INT32 numVertices, INT32 numTriangles, INT32 maxTriPerLeaf, INT32 maxDepth);

	// Serialization interface
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Memory reporting
	virtual INT32							GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

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
	INT32									ClassifyCoord(REAL coord, REAL offset);

	INT32									ClassifyFace(const CRuArrayList<UINT16> &triFaces, INT32 triIndex, INT32 axis, REAL offset);
	REAL									EvaluateSplit(const CRuArrayList<UINT16> &triFaces, INT32 depth, const CRuAABB &bounds, INT32 axis, REAL splitValue);
	INT32									BuildTree(INT32 nodeIndex, const CRuArrayList<UINT16> &triFaces, INT32 depth, const CRuAABB &bounds, INT32 maxTriPerLeaf, INT32 maxDepth, CRuArrayList<kdNode> &nodeListOut, CRuArrayList<kdLeafData> &leafDataListOut);

	BOOL									QueryCollision(CRuCollisionQuery_Ray *rayQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax);
	BOOL									QueryCollision(CRuCollisionQuery_Ray_Nearest *rayQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax);
	BOOL									QueryCollision(CRuCollisionQuery_Segment *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax);
	BOOL									QueryCollision(CRuCollisionQuery_Segment_HitTest *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax);
	BOOL									QueryCollision(CRuCollisionQuery_Segment_Nearest *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax);
	BOOL									QueryCollision(CRuCollisionQuery_OBB_HitTest *boxQuery, INT32 nodeIndex, CRuAABB &volBounds);
	BOOL									QueryCollision(CRuCollisionQuery_OBB_TriEnum *boxQuery, INT32 nodeIndex, CRuAABB &volBounds);
	BOOL									QueryCollision(CRuCollisionQuery_Sphere_HitTest *sphereQuery, INT32 nodeIndex, CRuVector3 &volNearPt);
	BOOL									QueryCollision(CRuCollisionQuery_Sphere_TriEnum *sphereQuery, INT32 nodeIndex, CRuVector3 &volNearPt);
	BOOL									QueryCollision(CRuCollisionQuery_Enumerate *enumQuery, INT32 nodeIndex);
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Set_Ray *setQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax);
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Set_Sphere *setQuery, INT32 nodeIndex, CRuVector3 &volNearPt);
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Get_Segment *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax);
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Enumerate *enumQuery, INT32 nodeIndex);
};

inline INT32 CRuCollision_kdTree_16::ClassifyCoord(REAL coord, REAL offset)
{
	if(coord < offset /* - EPSILON */)
	{
		return -1;
	}
	else if(coord > offset /* + EPSILON */)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// Return -1 if the face is entirely behind the plane [axis]=offset
// Return 0 if the face spans the plane.
// Return 1 if the face is entirely in front of the plane.
//
// "behind" means on the negative side, "in front" means on the
// positive side.
inline INT32 CRuCollision_kdTree_16::ClassifyFace(const CRuArrayList<UINT16> &triFaces, INT32 triIndex, INT32 axis, REAL offset)
{
	assert(axis >= 0 && axis < 3);

	BOOL hasFrontVert = FALSE;
	BOOL hasBackVert = FALSE;

	for(INT32 i = 0; i < 3; ++i)
	{
		REAL coord = m_triVertices[m_triIndices[triFaces[triIndex] * 3 + i]][axis];
		INT32 cr = ClassifyCoord(coord, offset);

		if(cr == -1)
		{
			hasBackVert = TRUE;
		}
		else if(cr == 1)
		{
			hasFrontVert = TRUE;
		}
	}

	if(hasFrontVert && hasBackVert)
	{
		return 0;	// crossing.
	}
	else if(hasFrontVert)
	{
		return 1;	// all verts in front.
	}
	else if(hasBackVert)
	{
		return -1;	// all verts in back.
	}
	else
	{
		// Face is ON the plane.
		return 0;	// call it "crossing".
	}
}

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Collision_kdTree_16, CRuCollision_kdTree_16)

// ************************************************************************************************************************************************************

#pragma managed

#endif
