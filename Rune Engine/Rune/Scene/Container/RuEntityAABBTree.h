#pragma once

#include "../../Scene/Base/RuEntityContainer.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuAABBTree : public IRuEntity_Container
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct Node;

	struct EntityDescriptor
	{
		CRuEntity*							m_entity;
		CRuSphere							m_bounds;

		EntityDescriptor*					m_prevSibling;
		EntityDescriptor*					m_nextSibling;
		Node*								m_parentNode;

		void								Attach(Node *parentNode)
											{
												// Set entity info as the first entity of the parent node
												m_parentNode = parentNode;
												m_prevSibling = NULL;
												m_nextSibling = parentNode->m_firstEntity;

												parentNode->m_numEntities++;
												parentNode->m_firstEntity = this;

												// If there parent node already has entries, re-attach the list to the end of this entity info node
												if(m_nextSibling)
													m_nextSibling->m_prevSibling = this;
											}

		void								Detach()
											{
												// Make sure the entity info node is attached
												if(m_parentNode)
												{
													if(m_prevSibling == NULL)
													{
														if(m_nextSibling)
														{
															m_nextSibling->m_prevSibling = NULL;
														}

														m_parentNode->m_firstEntity = m_nextSibling;
													}
													else
													{
														m_prevSibling->m_nextSibling = m_nextSibling;

														if(m_nextSibling)
														{
															m_nextSibling->m_prevSibling = m_prevSibling;
														}
													}

													m_parentNode->m_numEntities--;

													m_prevSibling = NULL;
													m_nextSibling = NULL;
													m_parentNode = NULL;
												}
											}
	};

	struct Node
	{
		CRuAABB								m_bounds;										// Bounding box of the node

		INT32								m_numEntities;									// Number of entities at this node
		EntityDescriptor*							m_firstEntity;									// First entity at this node

		Node*								m_parent;										// Parent of the node
		Node*								m_prevSibling;									// Previous sibling
		Node*								m_nextSibling;									// Next sibling
		Node*								m_child;										// First child

											Node()
											:	m_numEntities(0), m_firstEntity(NULL), m_parent(NULL), m_prevSibling(NULL), m_nextSibling(NULL), m_child(NULL)
											{
											}
	};

	// Tree settings
	INT32									m_maxEntitiesPerNode;

	Node*									m_root;
	CRuHashMap<UINT64, EntityDescriptor *>*	m_entityHash;
	CRuStack<EntityDescriptor *>			m_recycledEntityDescriptors;
	CRuStack<Node *>						m_recycledNodes;

	BOOL									m_entityListDirty;
	CRuArrayList<EntityDescriptor *>		m_entityList;

	// Scratch
	CRuStack<Node *>						m_nodeStack;
	CRuArrayList<float>						m_floatArray;
	CRuRadixSort							m_radixSorter;

public:
											CRuAABBTree();
	virtual									~CRuAABBTree();

	// IRuObject interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							Update(float elapsedTime);

	BOOL									AddEntity(CRuEntity *entity);
	BOOL									RemoveEntity(CRuEntity *entity);
	void									Clear();

	INT32									GetDepth();

	INT32									GetEntities(const CRuAABB &bounds, CRuArrayList<CRuEntity *> &entitiesOut);
	INT32									GetEntities(const CRuSphere &sphere, CRuArrayList<CRuEntity *> &entitiesOut);
	void									DrawRepresentation();

protected:
	Node*									CreateNewNode();
	EntityDescriptor*						CreateNewEntityDescriptor();

	Node*									FindBestFitNode(CRuAABBTree::Node *node, const CRuSphere &bounds, float &volumeExpansionOut);
	void									SplitNode(CRuAABBTree::Node *node);
	void									CollapseNode(CRuAABBTree::Node *node);
	void									DiscardChildNodes(CRuAABBTree::Node *node);

	void									PropagateBounds(CRuAABBTree::Node *node);
	void									CalculateNodeBounds(CRuAABBTree::Node *node);

	void									BuildEntityList(CRuAABBTree::Node *node);

	INT32									GetDepth(CRuAABBTree::Node *node);
	INT32									GetEntities(CRuAABBTree::Node *node, const CRuAABB &bounds, CRuArrayList<CRuEntity *> &entitiesOut);
	INT32									GetEntities(CRuAABBTree::Node *node, const CRuSphere &sphere, CRuArrayList<CRuEntity *> &entitiesOut);

	void									DrawRepresentation(CRuAABBTree::Node *node, DWORD &color);

	BOOL									DisposeHandler(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_AABBTree, CRuAABBTree)

// ************************************************************************************************************************************************************

#pragma managed(pop)
