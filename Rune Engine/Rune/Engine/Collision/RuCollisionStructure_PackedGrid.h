/*!
	@file RuCollisionStructure_PackedGrid.h

	Copyright (c) 2004-2009 All rights reserved
*/

#pragma once

#include "../../Engine/Geometry/RuMeshCollision.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Implementation of IRuCollisionStructure using a quad-tree spatial subdivision strategy.
	Non-uniform subdivision is utilized to minimize tree size.

	@author John Tang
	@version 2004/11/02
*/
class CRuCollisionStructure_PackedGrid : public IRuCollisionStructure
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

	struct PackedVertex
	{
		UINT16								m_x;
		UINT16								m_z;
		UINT16								m_yH;
		UINT16								m_yL;
	};

	INT32									m_maxFacesPerLeaf;							//!< Maximum number of primitives allowed at a leaf
	IRuMesh*								m_mesh;										//!< Target mesh on which this collision structure operates
	REAL									m_gridSpacing;
	REAL									m_yBase;
	REAL									m_yIncrement;
	INT32									m_numTriangles;								//!< Total number of triangles in the tree
	INT32*									m_triFlags;									//!< Triangle flags
																						//!< | 3 3 2 2 2 2 2 2 2 2 2 2 | 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0 |
																						//!< | 1 0 9 8 7 6 5 4 3 2 1 0 | 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 |
																						//!< ---------------------------------------------------------------------
																						//!< |                         |                   ID                    |

	QuadtreeNode*							m_rootNode;									//!< Root node of the collision structure

public:
											CRuCollisionStructure_PackedGrid();
	virtual									~CRuCollisionStructure_PackedGrid();

	virtual BOOL							AttachToMesh(IRuMesh *mesh);
	BOOL									SetGridParameters(REAL gridSpacing, REAL yBase, REAL yIncrement);
	virtual BOOL							BuildCollisionStructure(IRuMesh *mesh, CRuArrayList<INT32> *batchIndices);
	BOOL									BuildCollisionStructureForRegularGrid(const CRuAABB &gridBounds, INT32 gridXTiles, INT32 gridZTiles);

	// IRuObject serialization
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Collision query interface
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

protected:
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

#pragma managed(pop)
