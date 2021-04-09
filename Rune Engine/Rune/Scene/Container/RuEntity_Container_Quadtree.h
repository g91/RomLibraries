#ifndef _RUENTITY_CONTAINER_QUADTREE_H_
#define _RUENTITY_CONTAINER_QUADTREE_H_

// Container base
#include "../../Scene/Base/RuEntityContainer.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuController_DistanceFader;

// ************************************************************************************************************************************************************

class CRuEntityContainer_Quadtree : public IRuEntity_Container
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	class EntityDesc
	{
	public:
		char*								m_resourceName;						//!< Resource name of the entity

		CRuVector3							m_scale;							//!< Scale of the entity
		CRuQuaternion						m_rotation;							//!< Rotation of the entity
		CRuVector3							m_translation;						//!< Translation of the entity

		CRuSphere							m_bounds;							//!< Bounds of the entity

		// Run-time information
		CRuEntity*							m_entity;							//!< Instanced entity

		REAL								m_distanceToCamera;					//!< Distance to camera
		BOOL								m_beingControlled;					//!< Is the entity being controlled by a fader?
		BOOL								m_fullyVisible;						//!< Is the entity fully visible?

	public:
											EntityDesc();
											~EntityDesc();
	};

	class TreeNode
	{
	public:
		INT32								m_iterationNumber;					//!< Node iteration number
		INT32								m_blockOffset;						//!< Node data block offset
		INT32								m_blockSize;						//!< Node data block size, denotes how much space is available in the block
		INT32								m_dataSize;							//!< Node data size (always <= m_blockSize)
		CRuAABB								m_bounds;							//!< Bounding box of the node

		// Run-time information
		BOOL								m_loaded;							//!< Has node data been loaded?
		CRuArrayList<EntityDesc *>			m_entities;							//!< Entities at the node

	public:
											TreeNode();
											~TreeNode();

		void								Initialize(const CRuAABB &bounds);

		BOOL								SerializeFrom(IRuStream *stream);
		BOOL								SerializeTo(IRuStream *stream);
	};

	class Tree
	{
	protected:
		struct InstanceDesc
		{
			INT32							m_nodeIndex;						//!< Node that contains the instanced entity
			EntityDesc*						m_entityDesc;						//!< Entity descriptor
			CRuEntity*						m_entity;							//!< Instanced entity
		};

		// Data source
		IRuStream*							m_dataStream;						//!< Stream which contains the container data
		INT32								m_headerOffset;						//!< Position at which the container header starts in the stream

		BOOL								m_readOnly;

		// Tree data
		INT32								m_depth;							//!< Depth of the tree
		INT32								m_numNodes;							//!< Number of nodes in the tree
		TreeNode*							m_treeArray;						//!< Tree stored as an array

		// Calculated values
		INT32								m_headerSize;						//!< Size of the tree stream header
		INT32								m_nodeSize;							//!< Size of each tree node

		// Locking data
		CRuArrayList<INT32>					m_lockedNodes;						//!< List of locked node indices

		// Instancing data
		CRuLRUQueue<UINT64, InstanceDesc>*	m_entityQueue;						//!< LRU queue of entities

		// Utility objects
		CRuStreamCompressor*				m_streamCompressor;					//!< Stream compression/decompression utility object
		CRuMemoryStream*					m_compressedChunkStream;			//!< Temporary buffer space for compressed streams
		CRuMemoryStream*					m_decompressedChunkStream;			//!< Temporary buffer space for decompressed streams

	public:
											Tree(IRuStream *dataStream, INT32 headerOffset, const CRuAABB &bounds, const CRuVector3 &maxLeafExtents);
											Tree(IRuStream *dataStream, INT32 headerOffset, BOOL readOnly);
											~Tree();

		BOOL								SerializeFromHeaderStream(IRuStream *stream);
		BOOL								SerializeToHeaderStream(IRuStream *stream, INT32 headerOffset = -1);

		BOOL								ExportTreeHeader(IRuStream *dataStream, INT32 headerOffset);
		BOOL								ExportTree(IRuStream *dataStream, INT32 headerOffset);

//		INT32								GetNodeIndex(const CRuAABB &bounds);
		BOOL								LockNode(INT32 nodeIndex);
		BOOL								UnlockNode(INT32 nodeIndex);
		BOOL								UnlockAllNodes();

		BOOL								LoadNode(INT32 nodeIndex);
		BOOL								SaveNode(INT32 nodeIndex);

		BOOL								FlushLockedNodes();

		BOOL								AddEntity(const char *resName, const CRuMatrix4x4 &transform, const CRuSphere &bounds);
		BOOL								RemoveEntity(EntityDesc *entityDescriptor);
		BOOL								RemoveEntity(CRuEntity *entity);
		BOOL								UpdateEntityDescriptor(CRuEntity *entity);
		BOOL								UpdateEntityDescriptor(CRuEntity *entity, const CRuMatrix4x4 &transform);

		BOOL								UpdateAllEntityBounds();

		BOOL								EnumerateEntities(const CRuAABB &bounds, CRuArrayList<EntityDesc *> &entityListOut, INT32 curNodeIndex = 0, INT32 curDepth = 0);
		BOOL								EnumerateEntities(const CRuPlane4 *frustumPlanes, CRuArrayList<EntityDesc *> &entityListOut, INT32 curNodeIndex = 0, INT32 curDepth = 0);

		BOOL								EnumerateEntityDescriptors(CRuArrayList<char *> &resNameList, CRuArrayList<CRuMatrix4x4> &transformList, CRuArrayList<CRuSphere> &boundsList);

	protected:
		BOOL								Initialize(INT32 curNodeIndex, INT32 curDepth, const CRuAABB &bounds);
		INT32								GetDepthNodeCount(INT32 depth);
		INT32								FindBestFitNode(INT32 curNodeIndex, INT32 curDepth, const CRuSphere &bounds);
		CRuEntity*							InstanceEntity(INT32 nodeIndex, INT32 entityIndex);

		BOOL								WriteNodeHeader(IRuStream *stream, INT32 headerOffset, TreeNode &node, INT32 nodeIndex);
		BOOL								WriteNodeData(IRuStream *stream, INT32 headerOffset, TreeNode &node, INT32 nodeIndex);

		BOOL								FlushEntityQueue();
		static BOOL							FlushEntityQueueCallback(const void *key, const void *data, void *userData);
	};

	class FadeDesc
	{
	public:
		EntityDesc*							m_targetEntity;						//!< Target entity descriptor
		CRuController_DistanceFader*		m_fadeController;					//!< Fade controller
		BOOL								m_isFadeOutController;				//!< Is this a fadeout controller?
	};

	// Entity tree
	Tree*									m_tree;								//!< The entity tree

	CRuArrayList<FadeDesc>					m_fadeControllers;					//!< List of active fade controllers

	// Visible entities
	CRuArrayList<EntityDesc *>				m_entityDescriptors;
	CRuArrayList<CRuEntity *>				m_entities;

	CRuInsertionSort						m_insertionSort;

	// Settings
	float									m_fadeInDistance;
	float									m_fadeOutDistance;

public:
											CRuEntityContainer_Quadtree();
	virtual									~CRuEntityContainer_Quadtree();

	BOOL									Initialize(IRuStream *dataStream, INT32 headerOffset, const CRuAABB &bounds, const CRuVector3 &maxLeafExtents);
	BOOL									Open(IRuStream *dataStream, INT32 headerOffset, BOOL readOnly = FALSE);

	BOOL									ExportEntityHeader(IRuStream *dataStream, INT32 headerOffset);
	BOOL									ExportTree(IRuStream *dataStream, INT32 headerOffset);

	BOOL									AddEntity(const char *resName, const CRuMatrix4x4 &transform, const CRuSphere &bounds);
	BOOL									RemoveEntity(CRuEntity *entityFrame);
	BOOL									UpdateEntity(CRuEntity *entityFrame);

	BOOL									EnumerateEntityDescriptors(CRuArrayList<char *> &resNameList, CRuArrayList<CRuMatrix4x4> &transformList, CRuArrayList<CRuSphere> &boundsList);

	BOOL									UpdateAllEntityBounds();

	BOOL									Save();

	// LOD selection and frustum culling
	void									SetCamera(CRuCamera *viewCamera);

	// IRuObject interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Entity interface
	virtual BOOL							ResetStates();
	virtual BOOL							Update(float elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// IRuEntity_Container interface
	virtual INT32							GetNumEntities();
	virtual CRuEntity*						GetEntity(INT32 index);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
