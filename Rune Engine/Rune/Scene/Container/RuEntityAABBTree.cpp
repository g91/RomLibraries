#include "../../Scene/Container/RuEntityAABBTree.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuAABBTree, IRuEntity_Container)
ruHEAP_DEFINE_SUBCLASS(CRuAABBTree, "CRuAABBTree", "IRuEntity_Container")
ruCLASSGUID_DEFINE(CRuAABBTree, 0x61930036, 0x77DC4b1b, 0x850488B9, 0x4796133A)

// ************************************************************************************************************************************************************

CRuAABBTree::CRuAABBTree()
:	m_maxEntitiesPerNode(64),
	m_root(NULL),
	m_entityListDirty(TRUE)
{
	// Initialize the root as a box at origin with no dimension
	m_root = ruNEW Node();
	m_root->m_bounds.SetBounds(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	m_entityHash = ruNEW CRuHashMap<UINT64, EntityDescriptor *>(8192);

	for(INT32 i = 0; i < 1000; ++i)
	{
		m_recycledEntityDescriptors.Push(ruNEW EntityDescriptor());
	}

	for(INT32 i = 0; i < 1000; ++i)
	{
		m_recycledNodes.Push(ruNEW Node());
	}

	// Register dispose event on self
	this->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuAABBTree::DisposeHandler, this, _1));
}

CRuAABBTree::~CRuAABBTree()
{
}

IRuObject *CRuAABBTree::Clone(IRuObject *clonedObject)
{
	return NULL;
}

BOOL CRuAABBTree::SerializeFrom(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuAABBTree::SerializeTo(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuAABBTree::Update(float elapsedTime)
{
	// Enumerate everything into entity list
	if(m_entityListDirty)
	{
		m_entityList.Clear();
		BuildEntityList(m_root);
		m_entityListDirty = FALSE;
	}

	for(INT32 i = 0; i < m_entityList.Count(); ++i)
	{
		m_entityList[i]->m_entity->Update(elapsedTime);
	}

	return TRUE;
}

BOOL CRuAABBTree::AddEntity(CRuEntity *entity)
{
	// Store reference to the entity's bounds
	const CRuSphere &entityBounds = entity->GetWorldBounds();

	// Find the AABB leaf that is the closest for this entity
	float volumeExpansion;
	Node *bestFitNode = FindBestFitNode(m_root, entityBounds, volumeExpansion);

	// Catastrophic failure
	assert(bestFitNode != NULL);

	// Increment reference count to the entity
	entity->AddRef();
	entity->SetParent(this);

	// Construct entity info struct
	EntityDescriptor *entityInfo = CreateNewEntityDescriptor();
	entityInfo->m_entity = entity;
	entityInfo->m_bounds = entityBounds;

	// Add entity info struct to the best fit node
	entityInfo->Attach(bestFitNode);
	bestFitNode->m_bounds.Merge(entityBounds);

	// Add entity info struct to the entity hash
	m_entityHash->Add((UINT64) entity, entityInfo);

	// Determine whether the best fit node needs to be split
	if(bestFitNode->m_numEntities > m_maxEntitiesPerNode)
	{
		// Split the node
		SplitNode(bestFitNode);
	}
	else
	{
		// Propagate new bounds up the tree
		PropagateBounds(bestFitNode);
	}

	// Mark entity list as dirty
	m_entityListDirty = TRUE;

	return TRUE;
}

BOOL CRuAABBTree::RemoveEntity(CRuEntity *entity)
{
	// Find matching entity info structure
	if(m_entityHash->ContainsKey((UINT64) entity))
	{
		EntityDescriptor *entityInfo = m_entityHash->Get((UINT64) entity);
		Node *oldParent = entityInfo->m_parentNode;

		// Detach entity info struct
		entityInfo->Detach();

		// Remove entity from list & hash
		m_entityHash->Remove((UINT64) entity);

		if(oldParent->m_numEntities == 0)
		{
			// Node is now degenerate... collapse it if it is not the root node
			if(oldParent->m_parent)
			{
				CollapseNode(oldParent->m_parent);
			}
		}
		else
		{
			// Recalculate the bounds of the entity info struct's old parent node
			CalculateNodeBounds(oldParent);
		}

		// Release reference to the entity
		ruENTITY_SAFE_RELEASE_CHILD(entityInfo->m_entity);

		// Push entity info struct into recycling stack
		m_recycledEntityDescriptors.Push(entityInfo);

		// Mark entity list as dirty
		m_entityListDirty = TRUE;

		return TRUE;
	}

	return FALSE;
}

void CRuAABBTree::Clear()
{
	// Discard entire tree
	DiscardChildNodes(m_root);

	// Reset bounds
	m_root->m_bounds.SetBounds(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

INT32 CRuAABBTree::GetDepth()
{
	return GetDepth(m_root);
}

INT32 CRuAABBTree::GetEntities(const CRuAABB &bounds, CRuArrayList<CRuEntity *> &entitiesOut)
{
	return GetEntities(m_root, bounds, entitiesOut);
}

INT32 CRuAABBTree::GetEntities(const CRuSphere &sphere, CRuArrayList<CRuEntity *> &entitiesOut)
{
	return GetEntities(m_root, sphere, entitiesOut);
}

void CRuAABBTree::DrawRepresentation()
{
	DWORD color = 0x000000FF;
	DrawRepresentation(m_root, color);
}

CRuAABBTree::Node *CRuAABBTree::CreateNewNode()
{
	Node *node = m_recycledNodes.IsEmpty() ? ruNEW Node() : m_recycledNodes.Pop();

	node->m_numEntities = 0;
	node->m_firstEntity = NULL;
	node->m_parent = NULL;
	node->m_prevSibling = NULL;
	node->m_nextSibling = NULL;
	node->m_child = NULL;

	return node;
}

CRuAABBTree::EntityDescriptor *CRuAABBTree::CreateNewEntityDescriptor()
{
	EntityDescriptor *entityInfo = m_recycledEntityDescriptors.IsEmpty() ? ruNEW EntityDescriptor() : m_recycledEntityDescriptors.Pop();

	entityInfo->m_entity = NULL;
	entityInfo->m_parentNode = NULL;
	entityInfo->m_prevSibling = NULL;
	entityInfo->m_nextSibling = NULL;

	return entityInfo;
}

CRuAABBTree::Node *CRuAABBTree::FindBestFitNode(CRuAABBTree::Node *node, const CRuSphere &bounds, float &volumeExpansionOut)
{
	// If the current node is a leaf, we can stop
	if(node->m_child == NULL)
	{
		// Determine how much volume the node needs to expand to enclose the sphere and return
		CRuAABB mergedBounds(node->m_bounds);
		mergedBounds.Merge(bounds);
		volumeExpansionOut = mergedBounds.GetVolume() - node->m_bounds.GetVolume();
		return node;
	}

	Node *bestFitNode = NULL;						// Best-fit node to the sphere
	float bestFitVolumeExpansion = FLT_MAX;			// Volume expansion required by the best-fit node to enclose the sphere

	// Iterate through every child
	Node *curChild = node->m_child;
	while(curChild)
	{
		// Determine how much volume the node needs to expand to enclose the sphere and return
		CRuAABB mergedBounds(curChild->m_bounds);
		mergedBounds.Merge(bounds);
		float childVolumeExpansion = mergedBounds.GetVolume() - curChild->m_bounds.GetVolume();

		if(childVolumeExpansion < bestFitVolumeExpansion)
		{
			bestFitNode = curChild;
			bestFitVolumeExpansion = childVolumeExpansion;
		}

		// Go to next child
		curChild = curChild->m_nextSibling;
	}

	return FindBestFitNode(bestFitNode, bounds, volumeExpansionOut);
}

void CRuAABBTree::SplitNode(CRuAABBTree::Node *node)
{
	EntityDescriptor *curEntity;

	// Find the longest orthogonal axis we can project the entities onto
	INT32 longestAxis = 0;
	float longestAxisLength = 0.0f;
	for(INT32 i = 0; i < 3; ++i)
	{
		// Determine the value range for the current axis
		float axisMin = FLT_MAX, axisMax = -FLT_MAX;
		curEntity = node->m_firstEntity;
		while(curEntity)
		{
			axisMin = min(axisMin, curEntity->m_entity->GetWorldBounds().Center()[i]);
			axisMax = max(axisMax, curEntity->m_entity->GetWorldBounds().Center()[i]);
			curEntity = curEntity->m_nextSibling;
		}

		float axisLength = axisMax - axisMin;
		if(axisLength > longestAxisLength)
		{
			longestAxis = i;
			longestAxisLength = axisLength;
		}
	}

	// Put all coordinates of the selected axis into the float array
	m_floatArray.Clear();
	curEntity = node->m_firstEntity;
	while(curEntity)
	{
		m_floatArray.Add(curEntity->m_entity->GetWorldBounds().Center()[longestAxis]);
		curEntity = curEntity->m_nextSibling;
	}

	// Sort the float array using radix sort
	m_radixSorter.Sort(m_floatArray.GetArray(), m_floatArray.Count());

	// Extract the median
	INT32 medianIndex = m_radixSorter.GetSortedIndices()[m_floatArray.Count() / 2];
	float medianValue = m_floatArray[medianIndex];

	// Create new child nodes
	Node *leftChild = CreateNewNode();
	Node *rightChild = CreateNewNode();

	leftChild->m_bounds.SetBounds(node->m_bounds);
	leftChild->m_bounds.Maxima()[longestAxis] = medianValue;

	rightChild->m_bounds.SetBounds(node->m_bounds);
	rightChild->m_bounds.Minima()[longestAxis] = medianValue;

	// Dump existing entities into children
	curEntity = node->m_firstEntity;
	while(curEntity)
	{
		EntityDescriptor *nextEntity = curEntity->m_nextSibling;

		// If the coordinate is less than or equal to median value, put in left child
		if(curEntity->m_entity->GetWorldBounds().Center()[longestAxis] <= medianValue)
		{
			curEntity->Detach();
			curEntity->Attach(leftChild);

			leftChild->m_bounds.Merge(curEntity->m_entity->GetWorldBounds());
		}
		else
		{
			curEntity->Detach();
			curEntity->Attach(rightChild);

			rightChild->m_bounds.Merge(curEntity->m_entity->GetWorldBounds());
		}

		curEntity = nextEntity;
	}

	// Watch out for degenerate case where all entities go to one child
	if(leftChild->m_numEntities == 0 || rightChild->m_numEntities == 0)
	{
		// Undo the split node operation
		if(leftChild->m_numEntities > 0)
			curEntity = leftChild->m_firstEntity;
		else
			curEntity = rightChild->m_firstEntity;

		while(curEntity)
		{
			EntityDescriptor *nextEntity = curEntity->m_nextSibling;

			curEntity->Detach();
			curEntity->Attach(node);

			curEntity = nextEntity;
		}

		// Re-calculate node bounds
		CalculateNodeBounds(node);

		// Free child nodes
		m_recycledNodes.Push(leftChild);
		m_recycledNodes.Push(rightChild);

		return;
	}

	// Clear entities at this node
	node->m_numEntities = 0;
	node->m_firstEntity = NULL;

	// Attach children
	leftChild->m_parent = node;
	leftChild->m_nextSibling = rightChild;
	rightChild->m_parent = node;
	rightChild->m_prevSibling = leftChild;

	node->m_child = leftChild;
}

void CRuAABBTree::CollapseNode(CRuAABBTree::Node *node)
{
	// Return if we are already at a leaf
	if(node->m_child == NULL)
	{
		return;
	}

	// Collapse all children
	Node *curChild = node->m_child;
	while(curChild)
	{
		Node *nextChild = curChild->m_nextSibling;

		CollapseNode(curChild);

		// Combine all entities from the child
		EntityDescriptor *curEntity = curChild->m_firstEntity;
		while(curEntity)
		{
			EntityDescriptor *nextEntity = curEntity->m_nextSibling;

			curEntity->Detach();
			curEntity->Attach(node);

			curEntity = nextEntity;
		}

		m_recycledNodes.Push(curChild);
		curChild = nextChild;
	}

	// Clear child pointer
	node->m_child = NULL;

	// Recalculate bounds
	CalculateNodeBounds(node);
}

void CRuAABBTree::DiscardChildNodes(CRuAABBTree::Node *node)
{
	// Discard all entities
	EntityDescriptor *curEntity = node->m_firstEntity;
	while(curEntity)
	{
		EntityDescriptor *nextEntity = curEntity->m_nextSibling;

		// Remove entity from list & hash
		m_entityHash->Remove((UINT64) curEntity->m_entity);

		// Detach and release entity
		curEntity->Detach();
		ruSAFE_RELEASE(curEntity->m_entity);

		// Recycle
		m_recycledEntityDescriptors.Push(curEntity);

		curEntity = nextEntity;
	}

	// Return if we are already at a leaf
	if(node->m_child == NULL)
	{
		return;
	}

	// Discard all children
	Node *curChild = node->m_child;
	while(curChild)
	{
		Node *nextChild = curChild->m_nextSibling;

		// Recycle child node
		DiscardChildNodes(curChild);
		m_recycledNodes.Push(curChild);

		curChild = nextChild;
	}

	// Clear child pointer
	node->m_child = NULL;
}

void CRuAABBTree::PropagateBounds(CRuAABBTree::Node *node)
{
	// Propagate new bounds up the tree
	Node *curNode = node;
	while(curNode->m_parent)
	{
		curNode->m_parent->m_bounds = curNode->m_parent->m_child->m_bounds;
		curNode->m_parent->m_bounds.Merge(curNode->m_parent->m_child->m_nextSibling->m_bounds);
		curNode = curNode->m_parent;
	}
}

void CRuAABBTree::CalculateNodeBounds(CRuAABBTree::Node *node)
{
	if(node->m_numEntities > 0)
	{
		// Recalculate bounds
		EntityDescriptor *curEntity = node->m_firstEntity;

		node->m_bounds.SetBounds(	curEntity->m_bounds.Center().x - curEntity->m_bounds.Radius(), curEntity->m_bounds.Center().y - curEntity->m_bounds.Radius(), curEntity->m_bounds.Center().z - curEntity->m_bounds.Radius(), 
									curEntity->m_bounds.Center().x + curEntity->m_bounds.Radius(), curEntity->m_bounds.Center().y + curEntity->m_bounds.Radius(), curEntity->m_bounds.Center().z + curEntity->m_bounds.Radius());

		curEntity = curEntity->m_nextSibling;
		while(curEntity)
		{
			EntityDescriptor *nextEntity = curEntity->m_nextSibling;
			node->m_bounds.Merge(curEntity->m_bounds);
			curEntity = nextEntity;
		}
	}
	else
	{
		// Set node bounds to its center point
		CRuVector3 nodeCenter = (node->m_bounds.Minima() + node->m_bounds.Maxima()) / 2.0f;
		node->m_bounds.SetBounds(nodeCenter, nodeCenter);
	}
}

void CRuAABBTree::BuildEntityList(CRuAABBTree::Node *node)
{
	// Add all entries at the node
	EntityDescriptor *curEntity = node->m_firstEntity;
	while(curEntity)
	{
		m_entityList.Add(curEntity);
		curEntity = curEntity->m_nextSibling;
	}

	// Recurse to all children
	Node *curChild = node->m_child;
	while(curChild)
	{
		BuildEntityList(curChild);

		curChild = curChild->m_nextSibling;
	}
}

INT32 CRuAABBTree::GetDepth(CRuAABBTree::Node *node)
{
	// If the current node is a leaf, return 1
	if(node->m_child == NULL)
	{
		return 1;
	}

	// Iterate through every child
	INT32 curMaxDepth = 0;
	Node *curChild = node->m_child;
	while(curChild)
	{
		curMaxDepth = max(curMaxDepth, GetDepth(curChild));
		curChild = curChild->m_nextSibling;
	}

	return curMaxDepth + 1;
}

INT32 CRuAABBTree::GetEntities(CRuAABBTree::Node *node, const CRuAABB &bounds, CRuArrayList<CRuEntity *> &entitiesOut)
{
	INT32 nodesTested = 0;

	m_nodeStack.Push(node);
	while(m_nodeStack.IsEmpty() == FALSE)
	{
		++nodesTested;
		node = m_nodeStack.Pop();

		if(Intersects(node->m_bounds, bounds))
		{
			EntityDescriptor *entityInfo = node->m_firstEntity;
			while(entityInfo)
			{
				if(Intersects(bounds, entityInfo->m_entity->GetWorldBounds()))
				{
					entitiesOut.Add(entityInfo->m_entity);
				}

				entityInfo = entityInfo->m_nextSibling;
			}

			if(node->m_child)
			{
				m_nodeStack.Push(node->m_child);
				m_nodeStack.Push(node->m_child->m_nextSibling);
			}
		}
	}

	return nodesTested;
}

INT32 CRuAABBTree::GetEntities(CRuAABBTree::Node *node, const CRuSphere &sphere, CRuArrayList<CRuEntity *> &entitiesOut)
{
	INT32 nodesTested = 0;

	m_nodeStack.Push(node);
	while(m_nodeStack.IsEmpty() == FALSE)
	{
		++nodesTested;
		node = m_nodeStack.Pop();

		if(Intersects(node->m_bounds, sphere))
		{
			EntityDescriptor *entityInfo = node->m_firstEntity;
			while(entityInfo)
			{
				if(Intersects(entityInfo->m_entity->GetWorldBounds(), sphere))
				{
					entitiesOut.Add(entityInfo->m_entity);
				}

				entityInfo = entityInfo->m_nextSibling;
			}

			if(node->m_child)
			{
				m_nodeStack.Push(node->m_child);
				m_nodeStack.Push(node->m_child->m_nextSibling);
			}
		}
	}

	return nodesTested;
}

void CRuAABBTree::DrawRepresentation(CRuAABBTree::Node *node, DWORD &color)
{
/*
	g_ruSceneRenderer->Ru3D_SetShaderParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, color);

	color = color << 8;
	if(color == 0xFF000000)
		color = 0x000000FF;

	// Draw bounding box of the current node
	g_ruSceneRenderer->Ru3D_Line_Begin();
	g_ruSceneRenderer->Ru3D_Line_Submit(node->m_bounds);
	g_ruSceneRenderer->Ru3D_Line_End();

	// Draw bounding boxes of contained entities
	EntityDescriptor *entityInfo = node->m_firstEntity;
	while(entityInfo)
	{
		CRuEntity *entity = entityInfo->m_entity;
		CRuAABB sphereBounds(	entity->GetWorldBounds().Center().x - entity->GetWorldBounds().Radius(), entity->GetWorldBounds().Center().y - entity->GetWorldBounds().Radius(), entity->GetWorldBounds().Center().z - entity->GetWorldBounds().Radius(), 
								entity->GetWorldBounds().Center().x + entity->GetWorldBounds().Radius(), entity->GetWorldBounds().Center().y + entity->GetWorldBounds().Radius(), entity->GetWorldBounds().Center().z + entity->GetWorldBounds().Radius());

		g_ruSceneRenderer->Ru3D_Line_Begin();
		g_ruSceneRenderer->Ru3D_Line_Submit(sphereBounds);
		g_ruSceneRenderer->Ru3D_Line_End();

		entityInfo = entityInfo->m_nextSibling;
	}

	Node *curChild = node->m_child;
	while(curChild)
	{
		DrawRepresentation(curChild, color);
		curChild = curChild->m_nextSibling;
	}
*/
}

BOOL CRuAABBTree::DisposeHandler(RuEventArgs *eventArgs)
{
	// Enumerate everything into entity list
	if(m_entityListDirty)
	{
		m_entityList.Clear();
		BuildEntityList(m_root);
	}

	// Remove all entities
	while(m_entityList.Count() > 0)
	{
		RemoveEntity(m_entityList[m_entityList.Count() - 1]->m_entity);
		m_entityList.RemoveAt(m_entityList.Count() - 1);
	}

	// Purge recycling bins
	while(!m_recycledEntityDescriptors.IsEmpty())
	{
		delete m_recycledEntityDescriptors.Pop();
	}

	while(!m_recycledNodes.IsEmpty())
	{
		delete m_recycledNodes.Pop();
	}

	// Remove root
	assert(m_root->m_child == NULL);
	delete m_root;

	// Destroy hash
	delete m_entityHash;

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
