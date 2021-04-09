/*!
	@file RuPartition_QuadTree.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../Partition/RuPartition_QuadTree.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuPartition_QuadTree, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuPartition_QuadTree, "CRuPartition_QuadTree", "IRuObject")
ruCLASSGUID_DEFINE(CRuPartition_QuadTree, 0xB58DBAD0, 0xCDF94616, 0xA40724E3, 0xBDB75282)

// ************************************************************************************************************************************************************

CRuPartition_QuadTree::QuadTreeNode::QuadTreeNode(const CRuAABB &bounds, INT32 objectsPerNode)
:	m_bounds(bounds),
	m_objectsPerNode(objectsPerNode)
{
	// Clear pointers to children
	memset(m_children, 0, sizeof(QuadTreeNode *) * 4);

	// Pre-subdivide the tree
	Subdivide();
}

CRuPartition_QuadTree::QuadTreeNode::~QuadTreeNode()
{
	// Delete all children
	for(INT32 i = 0; i < 4; ++i)
	{
		delete m_children[i];
	}
}

const CRuAABB &CRuPartition_QuadTree::QuadTreeNode::GetBounds()
{
	return m_bounds;
}

BOOL CRuPartition_QuadTree::QuadTreeNode::Clear()
{
	// Clear all objects at this node
	m_objects.Clear();

	// Recursively call on children
	if(!IsLeaf())
	{
		for(INT32 i = 0; i < 4; ++i)
		{
			m_children[i]->Clear();
		}
	}

	return TRUE;
}

BOOL CRuPartition_QuadTree::QuadTreeNode::InsertObject(IRuObject *object, const CRuSphere &bounds, BOOL root)
{
	if(Intersects(m_bounds, bounds))
	{
		// Attempt to insert into children first
		if(!IsLeaf())
		{
			for(INT32 i = 0; i < 4; ++i)
			{
				m_children[i]->InsertObject(object, bounds, FALSE);
			}
		}
		else
		{
			// Insert at this node
			ObjectDescriptor objDesc = { bounds, object };
			m_objects.Add(objDesc);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuPartition_QuadTree::QuadTreeNode::SetPosition(const CRuVector3 &position)
{
	// Offset all objects (NOT DONE)

	// Calculate position offset
	CRuVector3 oldPosition = (m_bounds.Minima() + m_bounds.Maxima()) * 0.5f;
	CRuVector3 offset = position - oldPosition;

	// Offset bounding box
	m_bounds.Minima() = m_bounds.Minima() + offset;
	m_bounds.Maxima() = m_bounds.Maxima() + offset;

	// Propagate change to children if necessary
	if(!IsLeaf())
	{
		for(INT32 i = 0; i < 4; ++i)
		{
			// Calculate child bounds
			CRuAABB childBounds;
			childBounds.SetBoundsToQuadrantXZ(m_bounds, i);

			// Calculate and set child position
			CRuVector3 childPosition = (childBounds.Minima() + childBounds.Maxima()) * 0.5f;
			m_children[i]->SetPosition(childPosition);
		}
	}

	return TRUE;
}

BOOL CRuPartition_QuadTree::QuadTreeNode::Subdivide()
{
	if(!IsLeaf())
	{
		return FALSE;
	}

	// If the node is already below the maximum node dimension threshold, do nothing
	CRuVector3 extents = m_bounds.Maxima() - m_bounds.Minima();
	REAL size = max(extents.m_x, extents.m_z);
	if(size < 480.0f)
		return TRUE;

	// Initialize children and distribute objects to children
	for(INT32 i = 0; i < 4; ++i)
	{
		CRuAABB childBounds;
		childBounds.SetBoundsToQuadrantXZ(m_bounds, i);
		m_children[i] = ruNEW QuadTreeNode(childBounds, m_objectsPerNode);

		for(INT32 j = 0; j < m_objects.Count(); ++j)
		{
			m_children[i]->InsertObject(m_objects[j].m_object, m_objects[j].m_bounds, FALSE);
		}

		m_children[i]->Subdivide();
	}

	// Clear all objects at this node
	m_objects.Clear();

	return TRUE;
}

BOOL CRuPartition_QuadTree::QuadTreeNode::IsLeaf()
{
	// If any child is NULL, this node is a leaf
	return m_children[0] == NULL;
}

void CRuPartition_QuadTree::QuadTreeNode::EnumerateObjects(const CRuPlane4 *frustumPlanes, CRuArrayList<IRuObject *> &objectsOut, CRuKeyHash<CRuGUID> *enumHash)
{
	// Ensure the node intersects the frustum
	if(IntersectsFrustum(frustumPlanes, m_bounds))
	{
		// Enumerate visible chunks
		for(INT32 i = 0; i < m_objects.Count(); ++i)
		{
//			if(IntersectsFrustum(frustumPlanes, m_objects[i].m_bounds))
			{
				// Make sure the object is not already on the output list
				const CRuGUID &guid = m_objects[i].m_object->GetGUID();
				if(enumHash->ContainsKey(guid, guid.HashCode()) == FALSE)
				{
					// Add object to output list
					objectsOut.Add(m_objects[i].m_object);

					// Add object's GUID to hash
					enumHash->Add(guid, guid.HashCode());
				}
			}
		}

		if(!IsLeaf())
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				m_children[i]->EnumerateObjects(frustumPlanes, objectsOut, enumHash);
			}
		}
	}
}

void CRuPartition_QuadTree::QuadTreeNode::EnumerateObjects(const CRuAABB &box, CRuArrayList<IRuObject *> &objectsOut, CRuKeyHash<CRuGUID> *enumHash)
{
	// Ensure the node intersects the frustum
	if(Intersects(box, m_bounds))
	{
		// Enumerate visible chunks
		for(INT32 i = 0; i < m_objects.Count(); ++i)
		{
//			if(Intersects(box, m_objects[i].m_bounds))
			{
				// Make sure the object is not already on the output list
				const CRuGUID &guid = m_objects[i].m_object->GetGUID();
				if(enumHash->ContainsKey(guid, guid.HashCode()) == FALSE)
				{
					// Add object to output list
					objectsOut.Add(m_objects[i].m_object);

					// Add object's GUID to hash
					enumHash->Add(guid, guid.HashCode());
				}
			}
		}

		if(!IsLeaf())
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				m_children[i]->EnumerateObjects(box, objectsOut, enumHash);
			}
		}
	}
}

void CRuPartition_QuadTree::QuadTreeNode::EnumerateObjects(const CRuOBB &box, CRuArrayList<IRuObject *> &objectsOut, CRuKeyHash<CRuGUID> *enumHash)
{
	// Ensure the node intersects the frustum
	if(Intersects(box, m_bounds))
	{
		// Enumerate visible chunks
		for(INT32 i = 0; i < m_objects.Count(); ++i)
		{
//			if(Intersects(box, m_objects[i].m_bounds))
			{
				// Make sure the object is not already on the output list
				const CRuGUID &guid = m_objects[i].m_object->GetGUID();
				if(enumHash->ContainsKey(guid, guid.HashCode()) == FALSE)
				{
					// Add object to output list
					objectsOut.Add(m_objects[i].m_object);

					// Add object's GUID to hash
					enumHash->Add(guid, guid.HashCode());
				}
			}
		}

		if(!IsLeaf())
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				m_children[i]->EnumerateObjects(box, objectsOut, enumHash);
			}
		}
	}
}

void CRuPartition_QuadTree::QuadTreeNode::EnumerateObjects(const CRuSphere &sphere, CRuArrayList<IRuObject *> &objectsOut, CRuKeyHash<CRuGUID> *enumHash)
{
	// Ensure the node intersects the frustum
	if(Intersects(m_bounds, sphere))
	{
		// Enumerate visible chunks
		for(INT32 i = 0; i < m_objects.Count(); ++i)
		{
//			if(Intersects(m_objects[i].m_bounds, sphere))
			{
				// Make sure the object is not already on the output list
				const CRuGUID &guid = m_objects[i].m_object->GetGUID();
				if(enumHash->ContainsKey(guid, guid.HashCode()) == FALSE)
				{
					// Add object to output list
					objectsOut.Add(m_objects[i].m_object);

					// Add object's GUID to hash
					enumHash->Add(guid, guid.HashCode());
				}
			}
		}

		if(!IsLeaf())
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				m_children[i]->EnumerateObjects(sphere, objectsOut, enumHash);
			}
		}
	}
}

void CRuPartition_QuadTree::QuadTreeNode::EnumerateObjects(const CRuRay3 &ray, CRuArrayList<IRuObject *> &objectsOut, CRuKeyHash<CRuGUID> *enumHash)
{
	// Ensure the node intersects the frustum
	if(Intersects(ray, m_bounds))
	{
		// Enumerate visible chunks
		for(INT32 i = 0; i < m_objects.Count(); ++i)
		{
//			if(Intersects(ray, m_objects[i].m_bounds))
			{
				// Make sure the object is not already on the output list
				const CRuGUID &guid = m_objects[i].m_object->GetGUID();
				if(enumHash->ContainsKey(guid, guid.HashCode()) == FALSE)
				{
					// Add object to output list
					objectsOut.Add(m_objects[i].m_object);

					// Add object's GUID to hash
					enumHash->Add(guid, guid.HashCode());
				}
			}
		}

		if(!IsLeaf())
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				m_children[i]->EnumerateObjects(ray, objectsOut, enumHash);
			}
		}
	}
}

void CRuPartition_QuadTree::QuadTreeNode::EnumerateObjects(const CRuSegment3 &segment, CRuArrayList<IRuObject *> &objectsOut, CRuKeyHash<CRuGUID> *enumHash)
{
	// Ensure the node intersects the frustum
	if(Intersects(segment, m_bounds))
	{
		// Enumerate visible chunks
		for(INT32 i = 0; i < m_objects.Count(); ++i)
		{
//			if(Intersects(segment, m_objects[i].m_bounds))
			{
				// Make sure the object is not already on the output list
				const CRuGUID &guid = m_objects[i].m_object->GetGUID();
				if(enumHash->ContainsKey(guid, guid.HashCode()) == FALSE)
				{
					// Add object to output list
					objectsOut.Add(m_objects[i].m_object);

					// Add object's GUID to hash
					enumHash->Add(guid, guid.HashCode());
				}
			}
		}

		if(!IsLeaf())
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				m_children[i]->EnumerateObjects(segment, objectsOut, enumHash);
			}
		}
	}
}

// ************************************************************************************************************************************************************

CRuPartition_QuadTree::CRuPartition_QuadTree()
:	m_root(NULL)
{
	m_enumeratedObjectHash = ruNEW CRuKeyHash<CRuGUID>(2048);
}

CRuPartition_QuadTree::~CRuPartition_QuadTree()
{
	delete m_root;
	delete m_enumeratedObjectHash;
}

BOOL CRuPartition_QuadTree::Initialize(const CRuAABB &treeBounds, INT32 objectsPerNode)
{
	m_root = ruNEW QuadTreeNode(treeBounds, objectsPerNode);

	return TRUE;
}

const CRuAABB &CRuPartition_QuadTree::GetBounds()
{
	return m_root->GetBounds();
}

BOOL CRuPartition_QuadTree::SetPosition(const CRuVector3 &position)
{
	if(m_root)
	{
		m_root->SetPosition(position);
		return TRUE;
	}

	return FALSE;
}

BOOL CRuPartition_QuadTree::Clear()
{
	if(m_root)
	{
		m_root->Clear();
	}

	return TRUE;
}

BOOL CRuPartition_QuadTree::InsertObject(IRuObject *object, const CRuSphere &bounds)
{
	if(m_root)
	{
		return m_root->InsertObject(object, bounds, TRUE);
	}

	return FALSE;
}

BOOL CRuPartition_QuadTree::DeleteObject(IRuObject *object)
{
	// NO IMPLEMENTATION
	return FALSE;
}

void CRuPartition_QuadTree::EnumerateObjects(const CRuPlane4 *frustumPlanes, CRuArrayList<IRuObject *> &objectsOut)
{
	if(m_root)
	{
		m_enumeratedObjectHash->Clear();
		m_root->EnumerateObjects(frustumPlanes, objectsOut, m_enumeratedObjectHash);
	}
}

void CRuPartition_QuadTree::EnumerateObjects(const CRuAABB &box, CRuArrayList<IRuObject *> &objectsOut)
{
	if(m_root)
	{
		m_enumeratedObjectHash->Clear();
		m_root->EnumerateObjects(box, objectsOut, m_enumeratedObjectHash);
	}
}

void CRuPartition_QuadTree::EnumerateObjects(const CRuOBB &box, CRuArrayList<IRuObject *> &objectsOut)
{
	if(m_root)
	{
		m_enumeratedObjectHash->Clear();
		m_root->EnumerateObjects(box, objectsOut, m_enumeratedObjectHash);
	}
}

void CRuPartition_QuadTree::EnumerateObjects(const CRuSphere &sphere, CRuArrayList<IRuObject *> &objectsOut)
{
	if(m_root)
	{
		m_enumeratedObjectHash->Clear();
		m_root->EnumerateObjects(sphere, objectsOut, m_enumeratedObjectHash);
	}
}

void CRuPartition_QuadTree::EnumerateObjects(const CRuRay3 &ray, CRuArrayList<IRuObject *> &objectsOut)
{
	if(m_root)
	{
		m_enumeratedObjectHash->Clear();
		m_root->EnumerateObjects(ray, objectsOut, m_enumeratedObjectHash);
	}
}

void CRuPartition_QuadTree::EnumerateObjects(const CRuSegment3 &segment, CRuArrayList<IRuObject *> &objectsOut)
{
	if(m_root)
	{
		m_enumeratedObjectHash->Clear();
		m_root->EnumerateObjects(segment, objectsOut, m_enumeratedObjectHash);
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
