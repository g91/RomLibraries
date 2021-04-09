/*!
	@file RuRenderer_Type.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../../Renderer/Type/RuRenderer_Type.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuClipPlaneSet, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuClipPlaneSet, "CRuClipPlaneSet", "IRuObject")
ruCLASSGUID_DEFINE(CRuClipPlaneSet, 0x0954828B, 0x791B41c6, 0xBFE2DD81, 0x87FA3B31)

ruRTTI_DEFINE_SUBCLASS(CRuRendererNode, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuRendererNode, "CRuRendererNode", "IRuObject")
ruCLASSGUID_DEFINE(CRuRendererNode, 0x25EE7DBE, 0x640C4b77, 0x9177ADA1, 0xCB3C7F69)

// ************************************************************************************************************************************************************

CRuClipPlaneSet::CRuClipPlaneSet()
{
}

CRuClipPlaneSet::~CRuClipPlaneSet()
{
}

const CRuPlane4 &CRuClipPlaneSet::GetClipPlane(INT32 index)
{
	return m_clipPlanes[index];
}

void CRuClipPlaneSet::SetClipPlane(INT32 index, const CRuPlane4 &clipPlane)
{
	m_clipPlanes[index] = clipPlane;
}

// ************************************************************************************************************************************************************

CRuRendererNode::CRuRendererNode()
:	m_persistAcrossFrames(FALSE),
	m_priority(0),
	m_parent(NULL),
	m_firstChild(NULL),
	m_prevSibling(NULL),
	m_nextSibling(NULL)
{
}

CRuRendererNode::~CRuRendererNode()
{
	// Detach all child nodes
	CRuRendererNode *curChild = m_firstChild;
	while(curChild)
	{
		CRuRendererNode *nextChild = curChild->m_nextSibling;

		// Reset pointers
		curChild->m_parent = NULL;
		curChild->m_prevSibling = NULL;
		curChild->m_nextSibling = NULL;

		// Release
		ruSAFE_RELEASE(curChild);

		// Advance to next sibling
		curChild = nextChild;
	}

	// If the entity is an interior node or leaf, detach it
	if(m_parent)
	{
		this->DetachFromParent();
		m_parent = NULL;
	}

	// Clear children
	m_firstChild = NULL;
}

BOOL CRuRendererNode::PersistAcrossFrames(BOOL enable)
{
	m_persistAcrossFrames = enable;

	return m_persistAcrossFrames;
}

BOOL CRuRendererNode::IsPersistentAcrossFrames()
{
	return m_persistAcrossFrames;
}

INT32 CRuRendererNode::GetPriority()
{
	return m_priority;
}

INT32 CRuRendererNode::SetPriority(INT32 priority)
{
	m_priority = priority;
	return m_priority;
}

BOOL CRuRendererNode::AddChild(CRuRendererNode *node)
{
	// Detach node from its parent
	node->DetachFromParent();

	// Increment reference count on new child
	node->AddRef();

	if(m_firstChild)
	{
		// Go to the first child with the same or lower priority number
		CRuRendererNode *lastNode = NULL;
		CRuRendererNode *insertionNode = m_firstChild;

		while(insertionNode && node->m_priority <= insertionNode->m_priority)
		{
			lastNode = insertionNode;
			insertionNode = insertionNode->m_nextSibling;
		}

		if(insertionNode)
		{
			// Attach  node in front of the insertion node
			node->m_parent = this;
			node->m_prevSibling = insertionNode->m_prevSibling;
			node->m_nextSibling = insertionNode;

			insertionNode->m_prevSibling = node;

			if(m_firstChild == insertionNode)
			{
				m_firstChild = node;
			}
			else
			{
				node->m_prevSibling->m_nextSibling = node;
			}
		}
		else
		{
			// Attach node as the last child
			node->m_parent = this;
			node->m_prevSibling = lastNode;
			node->m_nextSibling = NULL;

			lastNode->m_nextSibling = node;
		}
	}
	else
	{
		// Attach node as the first child
		m_firstChild = node;

		node->m_parent = this;
		node->m_prevSibling = NULL;
		node->m_nextSibling = NULL;
	}

	return TRUE;
}

BOOL CRuRendererNode::RemoveChild(CRuRendererNode *node)
{
	if(m_firstChild)
	{
		CRuRendererNode *curChild = m_firstChild;

		// Find the child
		while(curChild && curChild != node)
		{
			curChild = curChild->m_nextSibling;
		}

		// Child node is found
		if(curChild)
		{
			// Detach child entity from the tree
			if(curChild->m_prevSibling)
			{
				curChild->m_prevSibling->m_nextSibling = curChild->m_nextSibling;

				if(curChild->m_nextSibling)
				{
					curChild->m_nextSibling->m_prevSibling = curChild->m_prevSibling;
				}
			}
			else if(curChild->m_nextSibling)
			{
				curChild->m_nextSibling->m_prevSibling = NULL;

				m_firstChild = curChild->m_nextSibling;
			}
			else
			{
				m_firstChild = NULL;
			}

			// Clear child's structure info
			curChild->m_parent = NULL;
			curChild->m_prevSibling = NULL;
			curChild->m_nextSibling = NULL;

			// Release reference to child entity
			ruSAFE_RELEASE(curChild);

			return TRUE;
		}

		// Child node is not found, recursively call on all children
		curChild = m_firstChild;
		while(curChild)
		{
			if(curChild->RemoveChild(node))
			{
				return TRUE;
			}

			curChild = curChild->m_nextSibling;
		}
	}

	return FALSE;
}

BOOL CRuRendererNode::RemoveAllChildren()
{
	CRuRendererNode *curChild = m_firstChild;

	while(curChild)
	{
		CRuRendererNode *nextChild = curChild->m_nextSibling;

		// Detach child node
		curChild->m_parent = NULL;
		curChild->m_prevSibling = NULL;
		curChild->m_nextSibling = NULL;

		// Release reference to child
		ruSAFE_RELEASE(curChild);

		// Advance to next sibling
		curChild = nextChild;
	}

	m_firstChild = NULL;

	return TRUE;
}

BOOL CRuRendererNode::DetachFromParent()
{
	if(m_parent)
	{
		// Reassign links
		if(m_prevSibling)
		{
			if(m_nextSibling)
			{
				m_prevSibling->m_nextSibling = m_nextSibling;
				m_nextSibling->m_prevSibling = m_prevSibling;

				// Clear next sibling pointer
				m_nextSibling = NULL;
			}
			else
			{
				m_prevSibling->m_nextSibling = NULL;
			}

			// Clear previous sibling pointer
			m_prevSibling = NULL;
		}
		else
		{
			if(m_nextSibling)
			{
				m_nextSibling->m_prevSibling = NULL;
				m_parent->m_firstChild = m_nextSibling;

				// Clear next sibling pointer
				m_nextSibling = NULL;
			}
			else
			{
				m_parent->m_firstChild = NULL;
			}
		}

		// Clear parent pointer
		m_parent = NULL;

		// Release parent's reference count
		this->Release();

		return TRUE;
	}

	return FALSE;
}

BOOL CRuRendererNode::DetachNonPersistentChildren()
{
	CRuRendererNode *curChild = m_firstChild;

	while(curChild)
	{
		CRuRendererNode *nextChild = curChild->m_nextSibling;

		// Recursively invoke on children
		curChild->DetachNonPersistentChildren();

		// If child is not persistent across frames, detach it
		if(curChild->IsPersistentAcrossFrames() == FALSE)
		{
			curChild->DetachFromParent();
		}

		curChild = nextChild;
	}

	return TRUE;
}

BOOL CRuRendererNode::AddDependency(CRuRendererNode *node, BOOL transient)
{
	if(transient)
	{
		if(m_transientDependencies.Contains(node) == FALSE)
		{
			m_transientDependencies.Add(node);

			return TRUE;
		}
	}
	else
	{
		if(m_dependencies.Contains(node) == FALSE)
		{
			m_dependencies.Add(node);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuRendererNode::RemoveDependency(CRuRendererNode *node)
{
	m_dependencies.Remove(node);

	return TRUE;
}

BOOL CRuRendererNode::ReadyToExecute(CRuKeyHash<PTRVALUE> &executionMap)
{
	// Check dependencies at this node against the execution map
	// A node is ready to execute when all of its dependencies have been executed.
	// Dependencies are only valid on nodes of the same tree level. Dependencies against
	// ancestors are invalid because a parent node is always dependent on its children.
	for(INT32 i = 0; i < m_dependencies.Count(); ++i)
	{
		// If the execution map does not contain the current dependency, we are not ready to execute
		if(!executionMap.ContainsKey(reinterpret_cast<PTRVALUE>(m_dependencies[i])))
		{
			// Go to the first sibling
			CRuRendererNode *curSibling = this;
			while(curSibling->m_prevSibling)
			{
				curSibling = curSibling->m_prevSibling;
			}

			// Verify that the dependency is one of the node's siblings
			// NOTE: A node must never have a dependency on its ancestors, a node is always dependent on its descendants
			while(curSibling)
			{
				if(curSibling == m_dependencies[i])
				{
					return FALSE;
				}

				curSibling = curSibling->m_nextSibling;
			}
		}
	}

	for(INT32 i = 0; i < m_transientDependencies.Count(); ++i)
	{
		// If the execution map does not contain the current dependency, we are not ready to execute
		if(!executionMap.ContainsKey(reinterpret_cast<PTRVALUE>(m_transientDependencies[i])))
		{
			// Go to the first sibling
			CRuRendererNode *curSibling = this;
			while(curSibling->m_prevSibling)
			{
				curSibling = curSibling->m_prevSibling;
			}

			// Verify that the dependency is one of the node's siblings
			// NOTE: A node must never have a dependency on its ancestors, a node is always dependent on its descendants
			while(curSibling)
			{
				if(curSibling == m_transientDependencies[i])
				{
					return FALSE;
				}

				curSibling = curSibling->m_nextSibling;
			}
		}
	}

	return TRUE;
}

CRuEvent &CRuRendererNode::Event_Begin()
{
	return m_event_Begin;
}

CRuEvent &CRuRendererNode::Event_Execute()
{
	return m_event_Execute;
}

void CRuRendererNode::SetParentRenderer(IRuObject *renderer)
{
}

void CRuRendererNode::Begin()
{
	// Trigger begin event
	CRuRendererEvent_Begin_Args beginArgs(this);
	m_event_Begin.Trigger(&beginArgs);

	CRuRendererNode *curChild = m_firstChild;

	while(curChild)
	{
		// Call begin on child
		curChild->Begin();

		// Advance to next child
		curChild = curChild->m_nextSibling;
	}
}

void CRuRendererNode::Execute()
{
	// Trigger execute event on all descendants
	CRuKeyHash<CRuGUID> triggerHash(128);
	this->RunExecuteTrigger(triggerHash, 0);

	// Perform actual execution
	CRuKeyHash<PTRVALUE> executionMap(128);
	this->Execute(executionMap);
}

void CRuRendererNode::End()
{
	CRuRendererNode *curChild = m_firstChild;

	while(curChild)
	{
		// Call end on child
		curChild->End();

		// Advance to next child
		curChild = curChild->m_nextSibling;
	}

	// Clear transient dependencies
	m_transientDependencies.Clear();
}

void CRuRendererNode::Execute(CRuKeyHash<PTRVALUE> &executionMap)
{
	// Build an execution array
	CRuRendererNode *curChild = m_firstChild;
	m_runtime_ExecutionArray.Clear();

	while(curChild)
	{
		// Add child to execution array
		m_runtime_ExecutionArray.Add(curChild);

		// Advance to next child
		curChild = curChild->m_nextSibling;
	}

	// Execute all nodes in the execution array
	INT32 executionCount = 0;
	INT32 nodesExecutedThisIteration = 1;

	while(executionCount < m_runtime_ExecutionArray.Count() && nodesExecutedThisIteration > 0)
	{
		// Reset node execution count
		nodesExecutedThisIteration = 0;

		for(INT32 i = 0; i < m_runtime_ExecutionArray.Count(); ++i)
		{
			if(m_runtime_ExecutionArray[i])
			{
				// Is the node ready to execute?
				if(m_runtime_ExecutionArray[i]->ReadyToExecute(executionMap))
				{
					// Execute node, add it to execution map, and remove it from the execution array
					m_runtime_ExecutionArray[i]->Execute(executionMap);
					executionMap.Add(reinterpret_cast<PTRVALUE>(m_runtime_ExecutionArray[i]));
					m_runtime_ExecutionArray[i] = NULL;

					// Increment nodes executed this iteration
					++nodesExecutedThisIteration;
				}
			}
		}
	}
}

void CRuRendererNode::RunExecuteTrigger(CRuKeyHash<CRuGUID> &triggerHash, INT32 lastTriggerHashCount)
{
	// If the execute event has not been triggered, do it now
	const CRuGUID &nodeKey = this->GetGUID();

	if(triggerHash.ContainsKey(nodeKey, nodeKey.HashCode()) == FALSE)
	{
		// Trigger execute event
		CRuRendererEvent_Execute_Args executeArgs(this);
		m_event_Execute.Trigger(&executeArgs);

		// Insert key into trigger hash
		triggerHash.Add(nodeKey, nodeKey.HashCode());
	}

	// Recursively call on children
	CRuRendererNode *curChild = m_firstChild;

	while(curChild)
	{
		// Invoke recursively on child
		curChild->RunExecuteTrigger(triggerHash, triggerHash.Count());

		// Advance to next child
		curChild = curChild->m_nextSibling;
	}

	if(lastTriggerHashCount != triggerHash.Count())
	{
		this->RunExecuteTrigger(triggerHash, triggerHash.Count());
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
