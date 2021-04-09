/*!
	@file RuPartition_QuadTree.h

	Copyright (c) 2004-2008 All rights reserved
*/

#ifndef _RUPARTITION_QUADTREE_H_
#define _RUPARTITION_QUADTREE_H_

#include "../Core/Type/RuType_Object.h"
#include "../Core/Math/RuMathCollision.h"
#include "../Core/Math/RuMathGeometry.h"
#include "../Core/Math/RuMathVector.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuPartition_QuadTree : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct ObjectDescriptor
	{
	public:
		CRuSphere							m_bounds;						//!< Object bounds
		IRuObject*							m_object;						//!< Object
	};

	struct QuadTreeNode
	{
	protected:
		CRuAABB								m_bounds;						//!< Bounds of the node
		INT32								m_objectsPerNode;				//!< Maximum number of objects per node
		CRuArrayList<ObjectDescriptor>		m_objects;						//!< Array of objects at this node
		QuadTreeNode*						m_children[4];					//!< Children of the node

	public:
											QuadTreeNode(const CRuAABB &bounds, INT32 objectsPerNode);
											~QuadTreeNode();

		const CRuAABB&						GetBounds();
		BOOL								Clear();
	
		BOOL								InsertObject(IRuObject *object, const CRuSphere &bounds, BOOL root);

		BOOL								SetPosition(const CRuVector3 &position);
		BOOL								Subdivide();

		BOOL								IsLeaf();

		void								EnumerateObjects(const CRuPlane4 *frustumPlanes, CRuArrayList<IRuObject *> &objectsOut, CRuKeyHash<CRuGUID> *enumHash);
		void								EnumerateObjects(const CRuAABB &box, CRuArrayList<IRuObject *> &objectsOut, CRuKeyHash<CRuGUID> *enumHash);
		void								EnumerateObjects(const CRuOBB &box, CRuArrayList<IRuObject *> &objectsOut, CRuKeyHash<CRuGUID> *enumHash);
		void								EnumerateObjects(const CRuSphere &sphere, CRuArrayList<IRuObject *> &objectsOut, CRuKeyHash<CRuGUID> *enumHash);
		void								EnumerateObjects(const CRuRay3 &ray, CRuArrayList<IRuObject *> &objectsOut, CRuKeyHash<CRuGUID> *enumHash);
		void								EnumerateObjects(const CRuSegment3 &segment, CRuArrayList<IRuObject *> &objectsOut, CRuKeyHash<CRuGUID> *enumHash);
	};

	// Tree
	QuadTreeNode*							m_root;							//!< Root of the tree

	// Workspace
	CRuKeyHash<CRuGUID>*					m_enumeratedObjectHash;

public:
											CRuPartition_QuadTree();
	virtual									~CRuPartition_QuadTree();

	BOOL									Initialize(const CRuAABB &treeBounds, INT32 objectsPerNode);
	const CRuAABB&							GetBounds();
	BOOL									SetPosition(const CRuVector3 &position);

	BOOL									Clear();

	BOOL									InsertObject(IRuObject *object, const CRuSphere &bounds);
	BOOL									DeleteObject(IRuObject *object);

	void									EnumerateObjects(const CRuPlane4 *frustumPlanes, CRuArrayList<IRuObject *> &objectsOut);
	void									EnumerateObjects(const CRuAABB &box, CRuArrayList<IRuObject *> &objectsOut);
	void									EnumerateObjects(const CRuOBB &box, CRuArrayList<IRuObject *> &objectsOut);
	void									EnumerateObjects(const CRuSphere &sphere, CRuArrayList<IRuObject *> &objectsOut);
	void									EnumerateObjects(const CRuRay3 &ray, CRuArrayList<IRuObject *> &objectsOut);
	void									EnumerateObjects(const CRuSegment3 &segment, CRuArrayList<IRuObject *> &objectsOut);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
