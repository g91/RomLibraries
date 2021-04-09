/*!
	@project Rune
	@file RuPathHierarchy_Base.cpp

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/12/27
*/

#include "../PathEngine/RuPathHierarchy_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(IRuPathHierarchy_Base, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuPathHierarchy_Base, "IRuPathHierarchy_Base", "IRuObject")
ruCLASSGUID_DEFINE(IRuPathHierarchy_Base, 0x9F4EFE66, 0xB0FA4856, 0xAE108C94, 0xF8F482DC)

// ************************************************************************************************************************************************************

INT32 CRuPathGraphKeyPartition::s_partitionDensity = 25;

CRuPathGraphKeyPartition::CRuPathGraphKeyPartition(const CRuAABB &bounds)
:	m_partitionBounds(bounds)
{
	// Ignore Y parameter, since this is a quadtree
	m_partitionBounds.Minima().m_y = -100000.0f;
	m_partitionBounds.Maxima().m_y = 100000.0f;

	// Initialize children
	for(INT32 i = 0; i < 4; ++i)
	{
		m_children[i] = NULL;
	}
}

CRuPathGraphKeyPartition::~CRuPathGraphKeyPartition()
{
	// Destroy children
	for(INT32 i = 0; i < 4; ++i)
	{
		delete m_children[i];
	}
}

BOOL CRuPathGraphKeyPartition::InsertPathGraphKey(INT32 graphKey, const CRuAABB &graphBounds)
{
	// If the partition cannot contain the path graph, abort
	if(!m_partitionBounds.Contains(graphBounds))
	{
		return FALSE;
	}

	if(IsLeaf())
	{
		// Insert path graph key into the list
		m_pathGraphKeys.Add(GraphKeyDesc(graphKey, graphBounds));

		// Subdivide
		Subdivide(s_partitionDensity);

		return TRUE;
	}
	else
	{
		// Try adding path graph key into one of the child partitions
		for(INT32 i = 0; i < 4; ++i)
		{
			if(m_children[i]->InsertPathGraphKey(graphKey, graphBounds))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CRuPathGraphKeyPartition::DeletePathGraphKey(INT32 graphKey)
{
	if(IsLeaf())
	{
		for(INT32 i = 0; i < m_pathGraphKeys.Count(); ++i)
		{
			if(m_pathGraphKeys[i].m_graphKey == graphKey)
			{
				// Remove path graph key from array
				m_pathGraphKeys.RemoveAt(i);

				return TRUE;
			}
		}
	}
	else
	{
		// Try removing path graph key from one of the child partitions
		for(INT32 i = 0; i < 4; ++i)
		{
			if(m_children[i]->DeletePathGraphKey(graphKey))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CRuPathGraphKeyPartition::Enumerate(const CRuSegment3 &segment, CRuArrayList<INT32> &pathGraphKeysOut)
{
	if(Intersects(segment, m_partitionBounds))
	{
		if(IsLeaf())
		{
			for(INT32 i = 0; i < m_pathGraphKeys.Count(); ++i)
			{
				if(Intersects(segment, m_pathGraphKeys[i].m_graphBounds))
				{
					pathGraphKeysOut.Add(m_pathGraphKeys[i].m_graphKey);
				}
			}
		}
		else
		{
			for(INT32 i = 0; i < 4; ++i)
			{
				m_children[i]->Enumerate(segment, pathGraphKeysOut);
			}
		}
	}
}

void CRuPathGraphKeyPartition::Enumerate(const CRuVector3 &point, CRuArrayList<INT32> &pathGraphKeysOut)
{
	if(m_partitionBounds.Contains(point))
	{
		if(IsLeaf())
		{
			for(INT32 i = 0; i < m_pathGraphKeys.Count(); ++i)
			{
				if(m_pathGraphKeys[i].m_graphBounds.Contains(point))
				{
					pathGraphKeysOut.Add(m_pathGraphKeys[i].m_graphKey);
				}
			}
		}
		else
		{
			for(INT32 i = 0; i < 4; ++i)
			{
				m_children[i]->Enumerate(point, pathGraphKeysOut);
			}
		}
	}
}

void CRuPathGraphKeyPartition::Enumerate(const CRuAABB &bounds, CRuArrayList<INT32> &pathGraphKeysOut)
{
	if(Intersects(m_partitionBounds, bounds))
	{
		if(IsLeaf())
		{
			for(INT32 i = 0; i < m_pathGraphKeys.Count(); ++i)
			{
				if(Intersects(m_pathGraphKeys[i].m_graphBounds, bounds))
				{
					pathGraphKeysOut.Add(m_pathGraphKeys[i].m_graphKey);
				}
			}
		}
		else
		{
			for(INT32 i = 0; i < 4; ++i)
			{
				m_children[i]->Enumerate(bounds, pathGraphKeysOut);
			}
		}
	}
}

void CRuPathGraphKeyPartition::Enumerate(const CRuSphere &bounds, CRuArrayList<INT32> &pathGraphKeysOut)
{
	if(Intersects(m_partitionBounds, bounds))
	{
		if(IsLeaf())
		{
			for(INT32 i = 0; i < m_pathGraphKeys.Count(); ++i)
			{
				if(Intersects(m_pathGraphKeys[i].m_graphBounds, bounds))
				{
					pathGraphKeysOut.Add(m_pathGraphKeys[i].m_graphKey);
				}
			}
		}
		else
		{
			for(INT32 i = 0; i < 4; ++i)
			{
				m_children[i]->Enumerate(bounds, pathGraphKeysOut);
			}
		}
	}
}

void CRuPathGraphKeyPartition::ForAllElements(ForAllElementsCallback callback)
{
	CRuStack<CRuPathGraphKeyPartition *> workStack;
	workStack.Push(this);

	while(workStack.IsEmpty() == FALSE)
	{
		// Pop partition off work stack
		CRuPathGraphKeyPartition *partition = workStack.Pop();

		if(partition->IsLeaf())
		{
			for(INT32 i = 0; i < partition->m_pathGraphKeys.Count(); ++i)
			{
				callback(partition->m_pathGraphKeys[i].m_graphKey);
			}
		}
		else
		{
			// Push all children onto work stack
			for(INT32 i = 0; i < 4; ++i)
			{
				workStack.Push(partition->m_children[i]);
			}
		}
	}
}

void CRuPathGraphKeyPartition::Subdivide(INT32 targetDensity)
{
	if(targetDensity < m_pathGraphKeys.Count())
	{
		// Initialize child partitions
		for(INT32 i = 0; i < 4; ++i)
		{
			CRuAABB childBounds;
			childBounds.SetBoundsToQuadrantXZ(m_partitionBounds, i);
			m_children[i] = ruNEW CRuPathGraphKeyPartition(childBounds);
		}

		// Distribute path nodes to children
		for(INT32 i = 0; i < m_pathGraphKeys.Count(); ++i)
		{
			for(INT32 j = 0; j < 4; ++j)
			{
				m_children[j]->InsertPathGraphKey(m_pathGraphKeys[i].m_graphKey, m_pathGraphKeys[i].m_graphBounds);
			}
		}

		// Clear path nodes list
		m_pathGraphKeys.Clear();
		m_pathGraphKeys.SetArraySize(0);
	}
}

// ************************************************************************************************************************************************************

#pragma managed
