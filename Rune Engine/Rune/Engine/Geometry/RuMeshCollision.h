/*!
	@file RuMeshCollision.h

	Copyright (c) 2004 All rights reserved
*/

#ifndef _RUMESHCOLLISION_H_
#define _RUMESHCOLLISION_H_

// Rune system core
#include "../../Engine/Base/RuCore.h"
#include "../../Core/Type/RuType_BitField.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class IRuMesh;

// ************************************************************************************************************************************************************

/*!
	Base class specifying interface required for IRuMesh collision query structures.
	Collision structures form the basis for collision testing against a IRuMesh object,
	and queries take the form of a CRuCollisionQuery object. Refer to its documentation
	for usage details.

	Implementations of this interface must supply all three function calls.
	
	AttachToMesh should attach the collision structure to a mesh, and should return FALSE in
	the event that the specified mesh cannot be attached. For instance, if the collision
	structure was constructed for mesh A, the collision structure wouldn't be usable on mesh B.
	It is not strictly necessary to implement a rigorous error check here as long as attaching
	an incompatible collision structure to a mesh does not result in a program fault.
	In addition, IRuCollisionStructure should be treated as a module of IRuMesh, and thus, upon
	AttachToMesh, the collision structure implementation must not increment the IRuMesh reference
	count. Since the target IRuMesh will keep a reference to the collision structure, this would
	result in a circular reference.

	BuildCollisionStructure should initialize the collision structure for the specified mesh,
	and only on the specified batches of the mesh.

	QueryCollision should perform collision testing against the collision structure. See
	documentation for CRuCollisionQuery for rules regarding collision testing primitives.
	The function call should always return TRUE, and only return FALSE in the event of a
	catastrophic error.

	@author John Tang
	@version 2004/11/02
*/
class IRuCollisionStructure : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual 								~IRuCollisionStructure();

	// Collision structure construction
	virtual BOOL							AttachToMesh(IRuMesh *mesh) = 0;
	virtual BOOL							BuildCollisionStructure(IRuMesh *mesh, CRuArrayList<INT32> *batchIndices) = 0;

	// Collision query interface
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery) = 0;
};

// ************************************************************************************************************************************************************

/*!
	Implementation of IRuCollisionStructure using brute-force testing. No partitioning is
	performed. Only simple collision testing data is stored, and collision tests require
	O(N) time. Should be used only on very small meshes.

	@author John Tang
	@version 2004/11/24
*/
class CRuCollisionStructure_Simple : public IRuCollisionStructure
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuMesh*								m_mesh;										//!< Target mesh on which this collision structure operates

	CRuSphere								m_bounds;									//!< Bounds for the collision structure
	INT32									m_numColPrimitives;							//!< Number of collision primitives
	UINT32*									m_colFlags;									//!< Collision primitive flags
	UINT16*									m_colIndices;								//!< Collision primitive indices

public:
											CRuCollisionStructure_Simple();
	virtual									~CRuCollisionStructure_Simple();

	virtual BOOL							AttachToMesh(IRuMesh *mesh);
	virtual BOOL							BuildCollisionStructure(IRuMesh *mesh, CRuArrayList<INT32> *batchIndices);

	// IRuObject serialization
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Collision query interface
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);
};

// ************************************************************************************************************************************************************

/*!
	Implementation of IRuCollisionStructure using a quad-tree spatial subdivision strategy.
	Non-uniform subdivision is utilized to minimize tree size.

	@author John Tang
	@version 2004/11/02
*/
class CRuCollisionStructure_Quadtree : public IRuCollisionStructure
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct QuadtreeNode
	{
	public:
		CRuAABB								m_bounds;									//!< Bounding box for the node

		INT32								m_numFaces;									//!< Number of faces belonging to this node
		UINT16*								m_faceIndices;								//!< Indices into master triangle array to triangle faces for this leaf

		QuadtreeNode*						m_children[4];								//!< Pointer to the node's children

	public:
											QuadtreeNode()
											:	m_numFaces(0),
												m_faceIndices(NULL)
											{
												m_children[0] = NULL;
												m_children[1] = NULL;
												m_children[2] = NULL;
												m_children[3] = NULL;
											}

		BOOL								IsLeaf()
											{
												return m_children[0] == NULL;
											}
	};

	INT32									m_maxFacesPerLeaf;							//!< Maximum number of primitives allowed at a leaf
	IRuMesh*								m_mesh;										//!< Target mesh on which this collision structure operates
	INT32									m_numTriangles;								//!< Total number of triangles in the tree
	INT32*									m_triFlags;									//!< Triangle flags
																						//!< | 3 3 2 2 2 2 2 2 2 2 2 2 | 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0 |
																						//!< | 1 0 9 8 7 6 5 4 3 2 1 0 | 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 |
																						//!< ---------------------------------------------------------------------
																						//!< |                         |                   ID                    |

	QuadtreeNode*							m_rootNode;									//!< Root node of the collision structure

public:
											CRuCollisionStructure_Quadtree();
	virtual									~CRuCollisionStructure_Quadtree();

	virtual BOOL							AttachToMesh(IRuMesh *mesh);
	virtual BOOL							BuildCollisionStructure(IRuMesh *mesh, CRuArrayList<INT32> *batchIndices);
	BOOL									BuildCollisionStructureForRegularGrid(const CRuAABB &gridBounds, INT32 gridXTiles, INT32 gridZTiles);

	// IRuObject serialization
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Collision query interface
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	void									RelaxTree();

protected:
	BOOL									AddTriangle(QuadtreeNode *node, UINT16 triIndex, const CRuVector3 &centroid, INT32 depth);
	void									SubdivideNode(QuadtreeNode *node, INT32 depth);
	void									RelaxTree(QuadtreeNode *node);
	void									DestroyCollisionStructure(QuadtreeNode *node);

	BOOL									QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Ray *rayQuery);
	BOOL									QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Ray_Nearest *rayQuery);
	BOOL									QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Segment *segQuery);
	BOOL									QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Segment_HitTest *segQuery);
	BOOL									QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Segment_Nearest *segQuery);
	BOOL									QueryCollision(QuadtreeNode *node, CRuCollisionQuery_OBB_HitTest *enumQuery);
	BOOL									QueryCollision(QuadtreeNode *node, CRuCollisionQuery_OBB_TriEnum *boxQuery);
	BOOL									QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Sphere_HitTest *sphereQuery);
	BOOL									QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Sphere_TriEnum *sphereQuery);
	BOOL									QueryCollision(QuadtreeNode *node, CRuCollisionQuery_Enumerate *enumQuery);
	BOOL									QueryCollision(QuadtreeNode *node, CRuCollisionQuery_User_Flag_Set *setQuery);
	BOOL									QueryCollision(QuadtreeNode *node, CRuCollisionQuery_User_Flag_Get_Segment *segQuery);
};

// ************************************************************************************************************************************************************

class CRuCollisionStructure_kdTree : public IRuCollisionStructure
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
		UINT16*								m_triFaces;				//!< Indices into master triangle array to triangle faces for this leaf
	};

	IRuMesh*								m_mesh;					//!< Target mesh on which this collision structure operates
	INT32									m_numTriangles;			//!< Total number of triangles in the tree
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

	INT32									m_triTested;

	CRuBitField								m_bitField;

public:

											CRuCollisionStructure_kdTree();
	virtual 								~CRuCollisionStructure_kdTree();

	virtual BOOL							AttachToMesh(IRuMesh *mesh);
	virtual BOOL							BuildCollisionStructure(IRuMesh *mesh, CRuArrayList<INT32> *batchIndices);

//	void									BuildTree(const CRuArrayList<CRuVector3> &triVertices, const CRuArrayList<UINT32> &triFlags, INT32 maxTriPerLeaf, INT32 maxDepth);

	// Serialization interface
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Memory reporting
	virtual INT32							GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	// Collision query interface
	virtual const CRuAABB&					GetBounds() const;
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

protected:
	INT32									ClassifyCoord(REAL coord, REAL offset);

	INT32									ClassifyFace(const CRuVector3 *triVertices, const UINT16 *triIndices, const CRuArrayList<UINT16> &triFaces, INT32 triIndex, INT32 axis, REAL offset);
	REAL									EvaluateSplit(const CRuVector3 *triVertices, const UINT16 *triIndices, const CRuArrayList<UINT16> &triFaces, INT32 depth, const CRuAABB &bounds, INT32 axis, REAL splitValue);
	INT32									BuildTree(INT32 nodeIndex, const CRuVector3 *triVertices, const UINT16 *triIndices, const CRuArrayList<UINT16> &triFaces, INT32 depth, const CRuAABB &bounds, INT32 maxTriPerLeaf, INT32 maxDepth, CRuArrayList<kdNode> &nodeListOut, CRuArrayList<kdLeafData> &leafDataListOut);

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
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Set *setQuery, INT32 nodeIndex);
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Set_Ray *setQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax);
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Set_Sphere *setQuery, INT32 nodeIndex, CRuVector3 &volNearPt);
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Get_Segment *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax);
	BOOL									QueryCollision(CRuCollisionQuery_User_Flag_Enumerate *enumQuery, INT32 nodeIndex);
};

// ************************************************************************************************************************************************************

inline IRuCollisionStructure::~IRuCollisionStructure()
{
}

// ************************************************************************************************************************************************************

inline INT32 CRuCollisionStructure_kdTree::ClassifyCoord(REAL coord, REAL offset)
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
inline INT32 CRuCollisionStructure_kdTree::ClassifyFace(const CRuVector3 *triVertices, const UINT16 *triIndices, const CRuArrayList<UINT16> &triFaces, INT32 triIndex, INT32 axis, REAL offset)
{
	assert(axis >= 0 && axis < 3);

	BOOL hasFrontVert = FALSE;
	BOOL hasBackVert = FALSE;

	for(INT32 i = 0; i < 3; ++i)
	{
		REAL coord = triVertices[triIndices[triFaces[triIndex] * 3 + i]][axis];
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

ruOBJECTMODULE_DEFINE(CRuObjectModule_MeshCollision_Simple, CRuCollisionStructure_Simple)
ruOBJECTMODULE_DEFINE(CRuObjectModule_MeshCollision_Quadtree, CRuCollisionStructure_Quadtree)
ruOBJECTMODULE_DEFINE(CRuObjectModule_MeshCollision_kdTree, CRuCollisionStructure_kdTree)

// ************************************************************************************************************************************************************

#pragma managed

#endif
