#ifndef _RUPOLYSOUP_H_
#define _RUPOLYSOUP_H_

// Rune system core
#include "../../Engine/Base/RuCore.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuPolySoup_Quadtree : public IRuObject
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
		INT32*								m_faceIndices;								//!< Array of indices to faces belonging to this node

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

											~QuadtreeNode()
											{
												// Destroy face indices
												delete[] m_faceIndices;

												// Destroy children
												for(INT32 i = 0; i < 4; ++i)
												{
													delete m_children[i];
												}
											}

		BOOL								IsLeaf()
											{
												return m_children[0] == NULL;
											}

		BOOL								SerializeFrom(IRuStream *stream);
		BOOL								SerializeTo(IRuStream *stream);

		void								AddTriangle(const CRuVector3 *v, INT32 maxFacesPerLeaf, CRuArrayList<CRuVector3> &vertices, CRuArrayList<INT32> &indices);
		void								AddTriangle(INT32 triangleIndex, INT32 maxFacesPerLeaf, CRuArrayList<CRuVector3> &vertices, CRuArrayList<INT32> &indices);
		void								RemoveTriangle(const CRuRay3 &removalRay, CRuArrayList<CRuVector3> &vertices, CRuArrayList<INT32> &indices);
		void								RemoveTriangle(const CRuAABB &removalAABB, CRuArrayList<CRuVector3> &vertices, CRuArrayList<INT32> &indices);
		INT32								GetNumTriangles();

		void								TranslatePolySoup(const CRuVector3 &translation);

		void								GetUniquePartitions(INT32 trianglesPerPartition, BOOL divideMarker, CRuArrayList<INT32> &faceCountList, CRuArrayList<INT32 *> &faceIndexList);

		BOOL								QueryCollision(CRuCollisionQuery *colQuery, CRuArrayList<CRuVector3> &vertices, CRuArrayList<INT32> &indices);

		void								SplitNode(INT32 maxFacesPerLeaf, CRuArrayList<CRuVector3> &vertices, CRuArrayList<INT32> &indices);
	};

public:
	// Polygon soup settings
	INT32									m_maxFacesPerLeaf;							//!< Maximum number of primitives allowed at a leaf

	// Polygon soup data
	CRuArrayList<CRuVector3>				m_vertices;									//!< List of triangle vertices
	CRuArrayList<INT32>						m_indices;									//!< List of triangle indices
	QuadtreeNode*							m_rootNode;									//!< Root node of the partition structure

public:
											CRuPolySoup_Quadtree();
	virtual									~CRuPolySoup_Quadtree();

	// IRuObject serialization
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	BOOL									AddTriangle(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2);
	BOOL									RemoveTriangle(const CRuRay3 &removalRay);
	BOOL									RemoveTriangle(const CRuAABB &removalAABB);

	BOOL									TranslatePolySoup(const CRuVector3 &translation);

	const CRuAABB&							GetBounds() const;
	INT32									GetNumTriangles() const;
	void									GetUniquePartitions(INT32 trianglesPerPartition, CRuArrayList<INT32> &faceCountList, CRuArrayList<INT32 *> &faceIndexList);

	// TODO: Add function to perform a "compact" operation -- remove unused triangles

	// Collision
	BOOL									QueryCollision(CRuCollisionQuery *colQuery);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_PolySoup_Quadtree, CRuPolySoup_Quadtree)

// ************************************************************************************************************************************************************

#pragma managed

#endif
