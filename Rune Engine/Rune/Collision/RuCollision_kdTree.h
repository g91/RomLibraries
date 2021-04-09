#ifndef _RUCOLLISION_KDTREE_H_
#define _RUCOLLISION_KDTREE_H_

#include "../Collision/RuCollision_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!

	NOTE: This class is here only as a template. It is not complete, nor is it really functional. Fix before use.

*/
class CRuCollision_kdTree : public IRuCollisionObject
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
		@version 2005/03/30
	*/
	union kdNode
	{
		REAL								m_splitVal;				//!< Non-Leaf, specifies splitting plane value
		INT32								m_leafIndex;			//!< Leaf, specifies leaf index
	};

	struct kdLeafData
	{
		INT32								m_numTriangles;			//!< Number of triangles in the leaf
		INT32*								m_triFaces;				//!< Indices into master triangle array to triangle faces for this leaf
	};

	INT32									m_numVertices;			//!< Total number of vertices in the tree
	CRuVector3*								m_triVertices;			//!< Master triangle vertex array
	INT32*									m_triFlags;				//!< Triangle flags
																	//!< | 3 3 2 2 2 2 2 2 2 2 2 2 | 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0 |
																	//!< | 1 0 9 8 7 6 5 4 3 2 1 0 | 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 |
																	//!< ---------------------------------------------------------------------
																	//!< |                         |                   ID                    |

	CRuAABB									m_bounds;
	INT32									m_numNodes;
	INT32									m_numLeaves;
	kdNode*									m_nodeArray;
	kdLeafData*								m_leafArray;

public:
	INT32									m_nodesChecked;
	INT32									m_trisChecked;

public:

											CRuCollision_kdTree();
	virtual 								~CRuCollision_kdTree();

	void									BuildTree(const CRuArrayList<CRuVector3> &triVertices, const CRuArrayList<UINT32> &triFlags, INT32 maxTriPerLeaf, INT32 maxDepth);

	// Serialization interface
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Collision query interface
	virtual const CRuAABB&					GetBounds() const;
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

protected:
	INT32									ClassifyCoord(REAL coord, REAL offset);

	INT32									ClassifyFace(const CRuVector3 *triVertices, const CRuArrayList<INT32> &triFaces, INT32 triIndex, INT32 axis, REAL offset);
	REAL									EvaluateSplit(const CRuVector3 *triVertices, const CRuArrayList<INT32> &triFaces, INT32 depth, const CRuAABB &bounds, INT32 axis, REAL splitValue);
	INT32									BuildTree(INT32 nodeIndex, const CRuVector3 *triVertices, const CRuArrayList<INT32> &triFaces, INT32 depth, const CRuAABB &bounds, INT32 maxTriPerLeaf, INT32 maxDepth, CRuArrayList<kdNode> &nodeListOut, CRuArrayList<kdLeafData> &leafDataListOut);

	BOOL									QueryCollision(CRuCollisionQuery_Ray *rayQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax);
	BOOL									QueryCollision(CRuCollisionQuery_Ray_Nearest *rayQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax);
	BOOL									QueryCollision(CRuCollisionQuery_Segment *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax);
	BOOL									QueryCollision(CRuCollisionQuery_Segment_HitTest *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax);
	BOOL									QueryCollision(CRuCollisionQuery_Segment_Nearest *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax);
	BOOL									QueryCollision(CRuCollisionQuery_Enumerate *enumQuery, INT32 nodeIndex);
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Set_Ray *setQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax);
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Set_Sphere *setQuery, INT32 nodeIndex, CRuVector3 &volNearPt);
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Get_Segment *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax);
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Enumerate *enumQuery, INT32 nodeIndex);
};

inline INT32 CRuCollision_kdTree::ClassifyCoord(REAL coord, REAL offset)
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
inline INT32 CRuCollision_kdTree::ClassifyFace(const CRuVector3 *triVertices, const CRuArrayList<INT32> &triFaces, INT32 triIndex, INT32 axis, REAL offset)
{
	assert(axis >= 0 && axis < 3);

	BOOL hasFrontVert = FALSE;
	BOOL hasBackVert = FALSE;

	for(INT32 i = 0; i < 3; ++i)
	{
		REAL coord = triVertices[triFaces[triIndex] * 3 + i][axis];
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
/*
	// NOTE: This is for when the kd-tree is converted to a packed form...

	INT32 PopCount8(INT32 n)
	{
		n = n - ((n & 0xAA) >> 1);					// Form partial sums of two bits each
		n = (n & 0x33) + ((n >> 2) & 0x33);			// Form partial sums of four bits each
		return (n + (n >> 4)) & 0x0F;				// Add four-bit sums together and mask result
	}

	BOOL ComputeChildIndices(kdNode *root, kdNode *node, kdNode **child0Out, kdNode **child1Out)
	{
		INT32 nodeAddress = reinterpret_cast<INT32>(node);
		INT32 nodeIndex = (nodeAddress & 0x3F) >> 2;				// Node index within the subtree
		INT32 child0Address, child1Address;

		if(nodeIndex < 7)
		{
			// If the node is not on the last level of the subtree, its children are at 2n + 1, 2n + 2
			child0Address = nodeAddress + (nodeAddress & 0x3F) + 4;
			child1Address = child0Address + 4;
		}
		else
		{
			INT32 rootAddress = reinterpret_cast<INT32>(root);
			INT32 bitIndex = nodeIndex - 7;

			// Last DWORD on cache line specifies linking of subtrees
			INT32 linkWord = *((INT32 *) (nodeAddress | 0x3C));

			// Assert if the node has no children
			assert(linkWord & (1 << bitIndex));

			// Calculate the number of subtrees to offset by
			INT32 offset = PopCount8(linkWord & ((1 << bitIndex) - 1));

			child0Address = rootAddress + ((linkWord >> 8) + offset * 2) * 64;
			child1Address = child0Address + 64;
		}

		*child0Out = (kdNode *) child0Address;
		*child1Out = (kdNode *) child1Address;
	}
*/

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Collision_kdTree, CRuCollision_kdTree)

// ************************************************************************************************************************************************************

#pragma managed

#endif
