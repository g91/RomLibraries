#include "../../Scene/Container/RuEntity_Container_Quadtree.h"

#include "../../Scene/Controller/RuController_DistanceFader.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntityContainer_Quadtree, IRuEntity_Container)
ruHEAP_DEFINE_SUBCLASS(CRuEntityContainer_Quadtree, "CRuEntityContainer_Quadtree", "IRuEntity_Container")
ruCLASSGUID_DEFINE(CRuEntityContainer_Quadtree, 0x0053632C, 0xBDEA469d, 0xAE85FD3A, 0x87D10C37)

// ************************************************************************************************************************************************************

/*!
	Standard constructor.
*/
CRuEntityContainer_Quadtree::EntityDesc::EntityDesc()
:	m_resourceName(NULL),
	m_entity(NULL),
	m_beingControlled(FALSE),
	m_fullyVisible(FALSE)
{
}

/*!
	Standard destructor. Frees all resources used by the descriptor.
*/
CRuEntityContainer_Quadtree::EntityDesc::~EntityDesc()
{
	delete[] m_resourceName;
	ruSAFE_RELEASE(m_entity);
}

/*!
	Standard constructor for TreeNode.
*/
CRuEntityContainer_Quadtree::TreeNode::TreeNode()
:	m_blockOffset(0),
	m_blockSize(0),
	m_dataSize(0),
	m_loaded(FALSE)
{
}

/*!
	Standard destructor for TreeNode. Frees all resources used by the node.
*/
CRuEntityContainer_Quadtree::TreeNode::~TreeNode()
{
	// Delete entity descriptors
	for(INT32 i = 0; i < m_entities.Count(); ++i)
	{
		delete m_entities[i];
	}
}

/*!
	Initializes the node. Should only be called at once per node.

	@param bounds Bounding box for the node.
*/
void CRuEntityContainer_Quadtree::TreeNode::Initialize(const CRuAABB &bounds)
{
	m_bounds = bounds;
}

BOOL CRuEntityContainer_Quadtree::TreeNode::SerializeFrom(IRuStream *stream)
{
	// Clear previously allocated entity descriptors

	// Read the number of entity descriptors
	INT32 numEntities = 0;
	stream->Read(&numEntities, sizeof(INT32));

	// Read each entity descriptor
	for(INT32 i = 0; i < numEntities; ++i)
	{
		EntityDesc *entityDesc = ruNEW EntityDesc();

		// Read length of the resource name string (including NULL)
		INT32 resNameLength = 0;
		stream->Read(&resNameLength, sizeof(INT32));

		// Read the resource name string
		entityDesc->m_resourceName = ruNEW char [resNameLength];
		stream->Read(entityDesc->m_resourceName, resNameLength);

		stream->Read(&entityDesc->m_scale.x, sizeof(float));
		stream->Read(&entityDesc->m_scale.y, sizeof(float));
		stream->Read(&entityDesc->m_scale.z, sizeof(float));

		stream->Read(&entityDesc->m_rotation.x, sizeof(float));
		stream->Read(&entityDesc->m_rotation.y, sizeof(float));
		stream->Read(&entityDesc->m_rotation.z, sizeof(float));
		stream->Read(&entityDesc->m_rotation.w, sizeof(float));

		stream->Read(&entityDesc->m_translation.x, sizeof(float));
		stream->Read(&entityDesc->m_translation.y, sizeof(float));
		stream->Read(&entityDesc->m_translation.z, sizeof(float));

		stream->Read(&entityDesc->m_bounds.Center().x, sizeof(float));
		stream->Read(&entityDesc->m_bounds.Center().y, sizeof(float));
		stream->Read(&entityDesc->m_bounds.Center().z, sizeof(float));
		stream->Read(&entityDesc->m_bounds.Radius(), sizeof(float));

		// Add descriptor to the descriptor list
		m_entities.Add(entityDesc);
	}

	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::TreeNode::SerializeTo(IRuStream *stream)
{
	// Write the number of entity descriptors
	INT32 numEntities = m_entities.Count();
	stream->Write(&numEntities, sizeof(INT32));

	// Write each entity descriptor
	for(INT32 i = 0; i < numEntities; ++i)
	{
		EntityDesc *entityDesc = m_entities[i];

		if(entityDesc->m_resourceName)
		{
			// Write length of the resource name string (including NULL)
			INT32 resNameLength = (INT32)strlen(entityDesc->m_resourceName) + 1;
			stream->Write(&resNameLength, sizeof(INT32));

			// Write the resource name string
			stream->Write(entityDesc->m_resourceName, resNameLength);
		}
		else
		{
			// Write length of the resource name string (including NULL)
			INT32 resNameLength = 1;
			stream->Write(&resNameLength, sizeof(INT32));

			// Write the resource name string
			stream->Write("", resNameLength);
		}

		stream->Write(&entityDesc->m_scale.x, sizeof(float));
		stream->Write(&entityDesc->m_scale.y, sizeof(float));
		stream->Write(&entityDesc->m_scale.z, sizeof(float));

		stream->Write(&entityDesc->m_rotation.x, sizeof(float));
		stream->Write(&entityDesc->m_rotation.y, sizeof(float));
		stream->Write(&entityDesc->m_rotation.z, sizeof(float));
		stream->Write(&entityDesc->m_rotation.w, sizeof(float));

		stream->Write(&entityDesc->m_translation.x, sizeof(float));
		stream->Write(&entityDesc->m_translation.y, sizeof(float));
		stream->Write(&entityDesc->m_translation.z, sizeof(float));

		stream->Write(&entityDesc->m_bounds.Center().x, sizeof(float));
		stream->Write(&entityDesc->m_bounds.Center().y, sizeof(float));
		stream->Write(&entityDesc->m_bounds.Center().z, sizeof(float));
		stream->Write(&entityDesc->m_bounds.Radius(), sizeof(float));
	}

	return TRUE;
}

/*!
	Standard constructor. The tree is subdivided until leaf nodes satisfy the maximum extents
	restriction. The Y extent is ignored, since this structure is a quadtree along the XZ plane.

	@param bounds Bounding box for the tree.
	@param maxLeafExtents Maximum extents of leaf nodes.
*/
CRuEntityContainer_Quadtree::Tree::Tree(IRuStream *dataStream, INT32 headerOffset, const CRuAABB &bounds, const CRuVector3 &maxLeafExtents)
:	m_dataStream(dataStream),
	m_headerOffset(headerOffset),
	m_readOnly(FALSE),
	m_depth(0),
	m_treeArray(NULL),
	m_headerSize(8),
	m_nodeSize(40)
{
	// Determine tree depth and array size
	float curXExtent = bounds.Maxima().x - bounds.Minima().x;
	float curZExtent = bounds.Maxima().z - bounds.Minima().z;
	while(curXExtent > maxLeafExtents.x || curZExtent > maxLeafExtents.z)
	{
		// Reduce extents for next tree level by a factor of 2
		curXExtent = curXExtent / 2.0f;
		curZExtent = curZExtent / 2.0f;

		// Increment tree depth
		++m_depth;
	}

	// Initialize tree array
	m_numNodes = GetDepthNodeCount(m_depth);
	m_treeArray = ruNEW TreeNode [m_numNodes];
	Initialize(0, 0, bounds);

	// Serialize headers to stream
	SerializeToHeaderStream(m_dataStream);

	// Initialize instancing queue
	m_entityQueue = ruNEW CRuLRUQueue<UINT64, InstanceDesc>(2048, 8192);

	// Initialize utility objects
	m_streamCompressor = ruNEW CRuStreamCompressor();
	m_compressedChunkStream = ruNEW CRuMemoryStream();
	m_decompressedChunkStream = ruNEW CRuMemoryStream();
}

CRuEntityContainer_Quadtree::Tree::Tree(IRuStream *dataStream, INT32 headerOffset, BOOL readOnly)
:	m_dataStream(dataStream),
	m_headerOffset(headerOffset),
	m_readOnly(readOnly),
	m_depth(0),
	m_treeArray(NULL),
	m_headerSize(8),
	m_nodeSize(40)
{
	// Serialize headers from stream
	SerializeFromHeaderStream(m_dataStream);

	// Initialize instancing queue
	m_entityQueue = ruNEW CRuLRUQueue<UINT64, InstanceDesc>(2048, 8192);

	// Initialize utility objects
	m_streamCompressor = ruNEW CRuStreamCompressor();
	m_compressedChunkStream = ruNEW CRuMemoryStream();
	m_decompressedChunkStream = ruNEW CRuMemoryStream();
}

/*!
	Standard destructor. Frees all resources used by the tree.
*/
CRuEntityContainer_Quadtree::Tree::~Tree()
{
	// Release lock on all nodes
	UnlockAllNodes();

	// Flush the entity queue
	FlushEntityQueue();

	delete[] m_treeArray;
	delete m_entityQueue;
	delete m_streamCompressor;
	delete m_compressedChunkStream;
	delete m_decompressedChunkStream;
}

BOOL CRuEntityContainer_Quadtree::Tree::SerializeFromHeaderStream(IRuStream *stream)
{
	// Seek to start of headers
	stream->Seek(m_headerOffset, ruSSM_Begin);

	// Read tree depth
	stream->Read(&m_depth, sizeof(INT32));

	// Read number of nodes
	stream->Read(&m_numNodes, sizeof(INT32));

	// Initialize tree array
	m_treeArray = ruNEW TreeNode [m_numNodes];

	// Read node data
	for(INT32 i = 0; i < m_numNodes; ++i)
	{
		m_treeArray[i].m_iterationNumber = 0;
		m_treeArray[i].m_blockOffset = 0;
		m_treeArray[i].m_blockSize = 0;
		m_treeArray[i].m_dataSize = 0;

		INT32 nodeOffset = m_headerOffset + m_headerSize + m_nodeSize * i;

		// Seek to the position of the node header
		stream->Seek(nodeOffset + sizeof(INT32) * 4, ruSSM_Begin);

		stream->Read(&m_treeArray[i].m_bounds.Minima().x, sizeof(float));
		stream->Read(&m_treeArray[i].m_bounds.Minima().y, sizeof(float));
		stream->Read(&m_treeArray[i].m_bounds.Minima().z, sizeof(float));
		stream->Read(&m_treeArray[i].m_bounds.Maxima().x, sizeof(float));
		stream->Read(&m_treeArray[i].m_bounds.Maxima().y, sizeof(float));
		stream->Read(&m_treeArray[i].m_bounds.Maxima().z, sizeof(float));
	}

	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::Tree::SerializeToHeaderStream(IRuStream *stream, INT32 headerOffset)
{
	if(headerOffset == -1)
	{
		// Seek to start of headers
		stream->Seek(m_headerOffset, ruSSM_Begin);
	}
	else
	{
		// Seek to start of headers
		stream->Seek(headerOffset, ruSSM_Begin);
	}

	// Write tree depth
	stream->Write(&m_depth, sizeof(INT32));

	// Write number of nodes
	stream->Write(&m_numNodes, sizeof(INT32));

	for(INT32 i = 0; i < m_numNodes; ++i)
	{
		stream->Write(&m_treeArray[i].m_iterationNumber, sizeof(INT32));
		stream->Write(&m_treeArray[i].m_blockOffset, sizeof(INT32));
		stream->Write(&m_treeArray[i].m_blockSize, sizeof(INT32));
		stream->Write(&m_treeArray[i].m_dataSize, sizeof(INT32));

		stream->Write(&m_treeArray[i].m_bounds.Minima().x, sizeof(float));
		stream->Write(&m_treeArray[i].m_bounds.Minima().y, sizeof(float));
		stream->Write(&m_treeArray[i].m_bounds.Minima().z, sizeof(float));
		stream->Write(&m_treeArray[i].m_bounds.Maxima().x, sizeof(float));
		stream->Write(&m_treeArray[i].m_bounds.Maxima().y, sizeof(float));
		stream->Write(&m_treeArray[i].m_bounds.Maxima().z, sizeof(float));
	}

	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::Tree::ExportTreeHeader(IRuStream *dataStream, INT32 headerOffset)
{
	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::Tree::ExportTree(IRuStream *dataStream, INT32 headerOffset)
{
	// Iterate and export each node
	for(INT32 i = 0; i < m_numNodes; ++i)
	{
		// Make sure the node is loaded
		LoadNode(i);

		// Save original node data values
		INT32 oldBlockOffset = m_treeArray[i].m_blockOffset;
		INT32 oldBlockSize = m_treeArray[i].m_blockSize;
		INT32 oldDataSize = m_treeArray[i].m_dataSize;

		// Reset node data values
		m_treeArray[i].m_blockOffset = 0;
		m_treeArray[i].m_blockSize = 0;
		m_treeArray[i].m_dataSize = 0;

		// Output node data
		WriteNodeData(dataStream, headerOffset, m_treeArray[i], i);

		// Output node header
		WriteNodeHeader(dataStream, headerOffset, m_treeArray[i], i);

		// Restore original node data values
		m_treeArray[i].m_blockOffset = oldBlockOffset;
		m_treeArray[i].m_blockSize = oldBlockSize;
		m_treeArray[i].m_dataSize = oldDataSize;
	}

	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::Tree::LockNode(INT32 nodeIndex)
{
	INT32 nodeOffset = m_headerOffset + m_headerSize + nodeIndex * m_nodeSize;

	// Check to see if the node is already locked
	if(m_lockedNodes.Contains(nodeIndex))
	{
		return TRUE;
	}

	// Make sure node is loaded
	if(LoadNode(nodeIndex) == FALSE)
	{
		return FALSE;
	}

	// Lock node header
	if(m_dataStream->LockStream(nodeOffset, 1, TRUE) == ruSEC_OK)
	{
		// Seek to node header position
		m_dataStream->Seek(nodeOffset, ruSSM_Begin);

		// Store node index in the list of locked nodes
		m_lockedNodes.Add(nodeIndex);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntityContainer_Quadtree::Tree::UnlockNode(INT32 nodeIndex)
{
	// Make sure the node is locked
	if(m_lockedNodes.Contains(nodeIndex))
	{
		INT32 nodeOffset = m_headerOffset + m_headerSize + nodeIndex * m_nodeSize;

		// Unlock node header
		if(m_dataStream->UnlockStream(nodeOffset, 1) == ruSEC_OK)
		{
			m_lockedNodes.Remove(nodeIndex);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuEntityContainer_Quadtree::Tree::UnlockAllNodes()
{
	while(m_lockedNodes.Count() > 0)
	{
		if(UnlockNode(m_lockedNodes[m_lockedNodes.Count() - 1]) == FALSE)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::Tree::LoadNode(INT32 nodeIndex)
{
	// Check the locked nodes list to see if the node is already locked
	if(m_lockedNodes.Contains(nodeIndex))
	{
		return TRUE;
	}

	// Check to see if the node is already loaded
	if(m_treeArray[nodeIndex].m_loaded)
	{
		return TRUE;
	}

	// Lock the node header for reading
	INT32 nodeOffset = m_headerOffset + m_headerSize + nodeIndex * m_nodeSize;

	// Lock node header for shared access
	if(m_readOnly || m_dataStream->LockStream(nodeOffset, 1, FALSE) == ruSEC_OK)
	{
		// Seek to the position of the node header
		m_dataStream->Seek(nodeOffset, ruSSM_Begin);
		
		m_dataStream->Read(&m_treeArray[nodeIndex].m_iterationNumber, sizeof(INT32));
		m_dataStream->Read(&m_treeArray[nodeIndex].m_blockOffset, sizeof(INT32));
		m_dataStream->Read(&m_treeArray[nodeIndex].m_blockSize, sizeof(INT32));
		m_dataStream->Read(&m_treeArray[nodeIndex].m_dataSize, sizeof(INT32));

		m_dataStream->Read(&m_treeArray[nodeIndex].m_bounds.Minima().x, sizeof(float));
		m_dataStream->Read(&m_treeArray[nodeIndex].m_bounds.Minima().y, sizeof(float));
		m_dataStream->Read(&m_treeArray[nodeIndex].m_bounds.Minima().z, sizeof(float));
		m_dataStream->Read(&m_treeArray[nodeIndex].m_bounds.Maxima().x, sizeof(float));
		m_dataStream->Read(&m_treeArray[nodeIndex].m_bounds.Maxima().y, sizeof(float));
		m_dataStream->Read(&m_treeArray[nodeIndex].m_bounds.Maxima().z, sizeof(float));

		// Load entities at the node if the node has data
		if(m_treeArray[nodeIndex].m_dataSize > 0)
		{
			// Seek to the position of the node data
			m_dataStream->Seek(m_treeArray[nodeIndex].m_blockOffset, ruSSM_Begin);

			// Read the compressed node entity stream into memory
			m_compressedChunkStream->SetStreamSize(m_treeArray[nodeIndex].m_dataSize);
			m_dataStream->Read(m_compressedChunkStream->GetBuffer(), m_treeArray[nodeIndex].m_dataSize);

			// Decompress the stream
			m_compressedChunkStream->Seek(0, ruSSM_Begin);
			m_decompressedChunkStream->SetStreamSize(0);
			m_streamCompressor->Decompress(m_compressedChunkStream, m_decompressedChunkStream);

			// Serialize from the decompressed stream
			m_decompressedChunkStream->Seek(0, ruSSM_Begin);
			m_treeArray[nodeIndex].SerializeFrom(m_decompressedChunkStream);
		}

		if(!m_readOnly)
		{
			// Unlock node header
			m_dataStream->UnlockStream(nodeOffset, 1);
		}

		// Set node status to loaded
		m_treeArray[nodeIndex].m_loaded = TRUE;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntityContainer_Quadtree::Tree::SaveNode(INT32 nodeIndex)
{
	// Check the locked nodes list to ensure the node is locked
	if(m_lockedNodes.Contains(nodeIndex) == FALSE)
	{
		return FALSE;
	}

	// Output node data
	WriteNodeData(m_dataStream, m_headerOffset, m_treeArray[nodeIndex], nodeIndex);

	// Output node header
	WriteNodeHeader(m_dataStream, m_headerOffset, m_treeArray[nodeIndex], nodeIndex);

	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::Tree::FlushLockedNodes()
{
	// Save locked nodes
	for(INT32 i = 0; i < m_lockedNodes.Count(); ++i)
	{
		if(SaveNode(m_lockedNodes[i]) == FALSE)
		{
			return FALSE;
		}
	}

	// Unlock all locked nodes
	UnlockAllNodes();

	return TRUE;
}

/*!
	Adds an entity descriptor to the tree. An attempt will be made to first add the entity
	descriptor to the tree's leaf nodes. An entity descriptor can be added to a tree if and
	only if the tree completely encloses the entity's bounding box.

	@param resName Resource name of the entity.
	@param scale Scale.
	@param rotation Rotation.
	@param translation Translation.
	@param bounds Bounds.

	@retval TRUE The operation succeeded.
	@retval FALSE The operation failed.
*/
BOOL CRuEntityContainer_Quadtree::Tree::AddEntity(const char *resName, const CRuMatrix4x4 &transform, const CRuSphere &bounds)
{
	INT32 bestFitNodeIndex = FindBestFitNode(0, 0, bounds);

	if(bestFitNodeIndex >= 0)
	{
		// Attempt to lock the node for writing
		if(LockNode(bestFitNodeIndex))
		{
			// Decompose orientation transform
			CRuVector3 scale, shear, rotation, translation;
			CRuQuaternion rotationQuat;

			transform.GetDecomposition(scale, shear, rotation, translation);
			rotationQuat.MakeEulerRotation(rotation);

			// Create new entity descriptor
			EntityDesc *desc = ruNEW EntityDesc();

			desc->m_resourceName = ruNEW char [strlen(resName) + 1];
			strcpy(desc->m_resourceName, resName);
			desc->m_scale = scale;
			desc->m_rotation = rotationQuat;
			desc->m_translation = translation;
			desc->m_bounds = bounds;

			// Add entity descriptor to the best fit node
			m_treeArray[bestFitNodeIndex].m_entities.Add(desc);

			return TRUE;
		}
	}
	
	return FALSE;
}

/*!
	Removes an entity descriptor from the tree.

	@param entityDescriptor Entity descriptor to remove.

	@retval TRUE The operation succeeded.
	@retval FALSE The operation failed.
*/
BOOL CRuEntityContainer_Quadtree::Tree::RemoveEntity(EntityDesc *entityDescriptor)
{
	INT32 bestFitNodeIndex = FindBestFitNode(0, 0, entityDescriptor->m_bounds);

	if(bestFitNodeIndex >= 0)
	{
		// Check to see if this node contains the entity
		for(INT32 i = 0; i < m_treeArray[bestFitNodeIndex].m_entities.Count(); ++i)
		{
			if(m_treeArray[bestFitNodeIndex].m_entities[i] == entityDescriptor)
			{
				// Delete the entity descriptor
				delete entityDescriptor;

				// Remove the entity descriptor from the entity list
				m_treeArray[bestFitNodeIndex].m_entities.RemoveAt(i);

				return TRUE;
			}
		}
	}

	return FALSE;
}

/*!
	Removes the entity descriptor for the given instanced entity.

	@param entity Instanced entity.

	@retval TRUE The operation succeeded.
	@retval FALSE The operation failed.
*/
BOOL CRuEntityContainer_Quadtree::Tree::RemoveEntity(CRuEntity *entity)
{
	UINT64 key = reinterpret_cast<UINT64>(entity);
	DWORD keyHash = RuGlobalHashGen().GetHashCode(key);

	// Attempt to locate the entity in the instanced entity queue
	InstanceDesc instanceDesc;
	if(m_entityQueue->Get(key, keyHash, instanceDesc))
	{
		// Attempt to lock the node for writing
		if(LockNode(instanceDesc.m_nodeIndex))
		{
			// Remove entity descriptor
			if(RemoveEntity(instanceDesc.m_entityDesc))
			{

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CRuEntityContainer_Quadtree::Tree::UpdateEntityDescriptor(CRuEntity *entity)
{
	UINT64 key = reinterpret_cast<UINT64>(entity);
	DWORD keyHash = RuGlobalHashGen().GetHashCode(key);

	// Attempt to locate the entity in the instanced entity queue
	InstanceDesc instanceDesc;
	if(m_entityQueue->Get(key, keyHash, instanceDesc))
	{
		// Attempt to lock the node for writing
		if(LockNode(instanceDesc.m_nodeIndex))
		{
			// Get the entity's bounds
			instanceDesc.m_entityDesc->m_bounds = entity->GetWorldBounds();

			// NOTES: The entity possibly needs to be moved to a different node
			// Re-insert the entity into the tree
			INT32 descriptorIndex = m_treeArray[instanceDesc.m_nodeIndex].m_entities.IndexOf(instanceDesc.m_entityDesc);

			if(descriptorIndex >= 0)
			{
				INT32 bestFitNodeIndex = FindBestFitNode(0, 0, instanceDesc.m_entityDesc->m_bounds);

				if(bestFitNodeIndex >= 0)
				{
					m_treeArray[instanceDesc.m_nodeIndex].m_entities.RemoveAt(descriptorIndex);

					// Attempt to lock the node for writing
					if(LockNode(bestFitNodeIndex))
					{
						// Add entity descriptor to the best fit node
						m_treeArray[bestFitNodeIndex].m_entities.Add(instanceDesc.m_entityDesc);

						// Alter instance descriptor
						instanceDesc.m_nodeIndex = bestFitNodeIndex;
						m_entityQueue->Set(key, keyHash, instanceDesc);

						return TRUE;
					}
				}
				else
				{
					// Cannot re-insert the entity, abort update
					MessageBox(NULL, "Unable to reinsert entity", "", MB_OK);
				}
			}

			return FALSE;
		}
	}

	return FALSE;
}

/*!
	Updates the entity descriptor for the given instanced entity with new orientation
	transforms.

	@param entity Instanced entity.
	@param scale Scale.
	@param rotation Rotation.
	@param translation Translation.

	@retval TRUE The operation succeeded.
	@retval FALSE The operation failed.
*/
BOOL CRuEntityContainer_Quadtree::Tree::UpdateEntityDescriptor(CRuEntity *entity, const CRuMatrix4x4 &transform)
{
	UINT64 key = reinterpret_cast<UINT64>(entity);
	DWORD keyHash = RuGlobalHashGen().GetHashCode(key);

	// Attempt to locate the entity in the instanced entity queue
	InstanceDesc instanceDesc;
	if(m_entityQueue->Get(key, keyHash, instanceDesc))
	{
		// Attempt to lock the node for writing
		if(LockNode(instanceDesc.m_nodeIndex))
		{
			CRuVector3 scale, shear, rot, translation;
			CRuQuaternion rotation;

			transform.GetDecomposition(scale, shear, rot, translation);
			rotation.MakeEulerRotation(rot);

			// Preserve old orientation
			CRuVector3 oldScale = instanceDesc.m_entityDesc->m_scale;
			CRuQuaternion oldRotation = instanceDesc.m_entityDesc->m_rotation;
			CRuVector3 oldTranslation = instanceDesc.m_entityDesc->m_translation;

			// Update orientation
			instanceDesc.m_entityDesc->m_scale = scale;
			instanceDesc.m_entityDesc->m_rotation = rotation;
			instanceDesc.m_entityDesc->m_translation = translation;

			// Build old orientation transform
			CRuMatrix4x4 oldOrientationTransform;
			oldOrientationTransform.Scale(oldScale);
			oldOrientationTransform.Rotate(oldRotation);
			oldOrientationTransform.Translate(oldTranslation);

			oldOrientationTransform = oldOrientationTransform.GetInverse();

			// Build orientation transform
			CRuMatrix4x4 orientationTransform;
			orientationTransform.Scale(scale);
			orientationTransform.Rotate(rotation);
			orientationTransform.Translate(translation);

			// Calculate new entity bounding sphere
			// Problem: the entity, at this point, is instanced somewhere in the world and has its own world coordinate.
			//          this means its bounding volume is also already in world coordinates.......
			// Solution: a big hack, just calculate the bounding box as a concatenation of the inverted old orientation and the new orientation..
			//           possible numerical stability implications!

			orientationTransform = oldOrientationTransform * orientationTransform;

			instanceDesc.m_entityDesc->m_bounds = entity->GetWorldBounds();
			orientationTransform.TransformPoint(instanceDesc.m_entityDesc->m_bounds.Center());

			// NOTES: The entity possibly needs to be moved to a different node

			// Re-insert the entity into the tree
			INT32 descriptorIndex = m_treeArray[instanceDesc.m_nodeIndex].m_entities.IndexOf(instanceDesc.m_entityDesc);

			if(descriptorIndex >= 0)
			{
				INT32 bestFitNodeIndex = FindBestFitNode(0, 0, instanceDesc.m_entityDesc->m_bounds);

				if(bestFitNodeIndex >= 0)
				{
					m_treeArray[instanceDesc.m_nodeIndex].m_entities.RemoveAt(descriptorIndex);

					// Attempt to lock the node for writing
					if(LockNode(bestFitNodeIndex))
					{
						// Add entity descriptor to the best fit node
						m_treeArray[bestFitNodeIndex].m_entities.Add(instanceDesc.m_entityDesc);

						// Alter instance descriptor
						instanceDesc.m_nodeIndex = bestFitNodeIndex;
						m_entityQueue->Set(key, keyHash, instanceDesc);

						return TRUE;
					}
				}
				else
				{
					instanceDesc.m_entityDesc->m_scale = oldScale;
					instanceDesc.m_entityDesc->m_rotation = oldRotation;
					instanceDesc.m_entityDesc->m_translation = oldTranslation;

					instanceDesc.m_entity->ResetLocalTransform();
					instanceDesc.m_entity->Scale(oldScale);
					instanceDesc.m_entity->Rotate(oldRotation);
					instanceDesc.m_entity->Translate(oldTranslation);
				}
			}

			// Cannot re-insert the entity, abort update
//			MessageBox(NULL, "Unable to reinsert entity", "", MB_OK);

			return FALSE;
		}
	}

	return FALSE;
}

BOOL CRuEntityContainer_Quadtree::Tree::UpdateAllEntityBounds()
{
	// Iterate through each node and update each contained entity's cached bounds
	for(INT32 i = 0; i < m_numNodes; ++i)
	{
		// Lock the node
		if(LockNode(i))
		{
			// Iterate through all entities at the node
			for(INT32 j = 0; j < m_treeArray[i].m_entities.Count(); ++j)
			{
				CRuEntity *entity = InstanceEntity(i, j);

				UpdateEntityDescriptor(entity);
			}
		}
	}

	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::Tree::EnumerateEntities(const CRuAABB &bounds, CRuArrayList<EntityDesc *> &entityListOut, INT32 curNodeIndex, INT32 curDepth)
{
	// Make sure the node at curNodeIndex intersects the bounding box
	if(Intersects(bounds, m_treeArray[curNodeIndex].m_bounds))
	{
		// If the current node is not a leaf
		if(curDepth < m_depth)
		{
			// Recursively invoke into children (NOTE: Child 0's index is calculated by nodeIndex * 4 + 1)
			for(INT32 i = 0, childIndex = curNodeIndex * 4 + 1, childDepth = curDepth + 1; i < 4; ++i, ++childIndex)
			{
				EnumerateEntities(bounds, entityListOut, childIndex, childDepth);
			}
		}

		// Make sure the node is fully loaded into memory
		if(LoadNode(curNodeIndex) == FALSE)
			return TRUE;

		for(INT32 i = 0; i < m_treeArray[curNodeIndex].m_entities.Count(); ++i)
		{
			// Make sure the entity actually intersects the bounding box
			if(Intersects(bounds, m_treeArray[curNodeIndex].m_entities[i]->m_bounds))
			{
				// Instance the entity
				CRuEntity *entity = InstanceEntity(curNodeIndex, i);
				entityListOut.Add(m_treeArray[curNodeIndex].m_entities[i]);
			}
		}
	}

	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::Tree::EnumerateEntities(const CRuPlane4 *frustumPlanes, CRuArrayList<EntityDesc *> &entityListOut, INT32 curNodeIndex, INT32 curDepth)
{
	// Make sure the node at curNodeIndex intersects the frustum
	if(IntersectsFrustum(frustumPlanes, m_treeArray[curNodeIndex].m_bounds))
	{
		// If the current node is not a leaf
		if(curDepth < m_depth)
		{
			// Recursively invoke into children (NOTE: Child 0's index is calculated by nodeIndex * 4 + 1)
			for(INT32 i = 0, childIndex = curNodeIndex * 4 + 1, childDepth = curDepth + 1; i < 4; ++i, ++childIndex)
			{
				EnumerateEntities(frustumPlanes, entityListOut, childIndex, childDepth);
			}
		}

		// Make sure the node is fully loaded into memory
		if(LoadNode(curNodeIndex) == FALSE)
			return TRUE;

		for(INT32 i = 0; i < m_treeArray[curNodeIndex].m_entities.Count(); ++i)
		{
			// Make sure the entity actually intersects the frustum
			if(IntersectsFrustum(frustumPlanes, m_treeArray[curNodeIndex].m_entities[i]->m_bounds))
			{
				// Instance the entity
				CRuEntity *entity = InstanceEntity(curNodeIndex, i);
				entityListOut.Add(m_treeArray[curNodeIndex].m_entities[i]);
			}
		}
	}

	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::Tree::EnumerateEntityDescriptors(CRuArrayList<char *> &resNameList, CRuArrayList<CRuMatrix4x4> &transformList, CRuArrayList<CRuSphere> &boundsList)
{
	for(INT32 curNodeIndex = 0; curNodeIndex < m_numNodes; ++curNodeIndex)
	{
		// Make sure the node is fully loaded into memory
		if(LoadNode(curNodeIndex) == FALSE)
			return TRUE;

		for(INT32 i = 0; i < m_treeArray[curNodeIndex].m_entities.Count(); ++i)
		{
			resNameList.Add(m_treeArray[curNodeIndex].m_entities[i]->m_resourceName);

			CRuMatrix4x4 transform;
			transform.SetToIdentity();
			transform.Scale(m_treeArray[curNodeIndex].m_entities[i]->m_scale);
			transform.Rotate(m_treeArray[curNodeIndex].m_entities[i]->m_rotation);
			transform.Translate(m_treeArray[curNodeIndex].m_entities[i]->m_translation);

			transformList.Add(transform);

			boundsList.Add(m_treeArray[curNodeIndex].m_entities[i]->m_bounds);
		}
	}

	return TRUE;
}

/*!
	Initializes the tree.

	@param curNodeIndex Current node index. Must be 0.
	@param curDepth Current depth. Must be 0.
	@param bounds Bounding box for the tree

	@retval TRUE The operation succeeded.
	@retval FALSE The operation failed.
*/
BOOL CRuEntityContainer_Quadtree::Tree::Initialize(INT32 curNodeIndex, INT32 curDepth, const CRuAABB &bounds)
{
	// Initialize current node
	m_treeArray[curNodeIndex].m_bounds = bounds;

	// If the current node is not a leaf
	if(curDepth < m_depth)
	{
		// Recursively invoke into children (NOTE: Child 0's index is calculated by nodeIndex * 4 + 1)
		CRuAABB childBounds;
		for(INT32 i = 0, childIndex = curNodeIndex * 4 + 1, childDepth = curDepth + 1; i < 4; ++i, ++childIndex)
		{
			// Set child bounds to the i-th quadrant
			childBounds.SetBoundsToQuadrantXZ(bounds, i);

			// Recursively invoke Initialize on child
			if(Initialize(childIndex, childDepth, childBounds) == FALSE)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

/*!
	Calculates and returns the total number of nodes in a tree with the specified depth.

	@param depth Depth of the tree. Depth of 0 specifies a tree with exactly 1 node.

	@return The number of nodes in the tree.
*/
INT32 CRuEntityContainer_Quadtree::Tree::GetDepthNodeCount(INT32 depth)
{
	INT32 depthNodes = 0;

	for(INT32 i = 0; i <= depth; ++i)
	{
		// Accumulate the number of nodes needed for the next tree level
		depthNodes += static_cast<INT32>(pow(4.0f, i));
	}

	return depthNodes;
}

INT32 CRuEntityContainer_Quadtree::Tree::FindBestFitNode(INT32 curNodeIndex, INT32 curDepth, const CRuSphere &bounds)
{
	// Make sure the node at curNodeIndex contains the specified bounding box
	if(m_treeArray[curNodeIndex].m_bounds.Contains(bounds))
	{
		// If the current node is not a leaf
		if(curDepth < m_depth)
		{
			// Recursively invoke into children (NOTE: Child 0's index is calculated by nodeIndex * 4 + 1)
			for(INT32 i = 0, childIndex = curNodeIndex * 4 + 1, childDepth = curDepth + 1; i < 4; ++i, ++childIndex)
			{
				// Check if child node is the best fit node for the bounding box
				INT32 childBestFitNode = FindBestFitNode(childIndex, childDepth, bounds);
				if(childBestFitNode >= 0)
				{
					return childBestFitNode;
				}
			}
		}

		// Current node is the best fit for the bounding box
		return curNodeIndex;
	}

	return -1;
}

CRuEntity *CRuEntityContainer_Quadtree::Tree::InstanceEntity(INT32 nodeIndex, INT32 entityIndex)
{
	// Load the entity or something
	if(m_treeArray[nodeIndex].m_entities[entityIndex]->m_entity == NULL)
	{
		CRuEntity *entity = g_ruResourceManager->LoadEntity(m_treeArray[nodeIndex].m_entities[entityIndex]->m_resourceName);

		if(entity)
		{
			m_treeArray[nodeIndex].m_entities[entityIndex]->m_entity = entity;

			// Setup frame orientation
			m_treeArray[nodeIndex].m_entities[entityIndex]->m_entity->ResetLocalTransform();
			m_treeArray[nodeIndex].m_entities[entityIndex]->m_entity->Scale(m_treeArray[nodeIndex].m_entities[entityIndex]->m_scale);
			m_treeArray[nodeIndex].m_entities[entityIndex]->m_entity->Rotate(m_treeArray[nodeIndex].m_entities[entityIndex]->m_rotation);
			m_treeArray[nodeIndex].m_entities[entityIndex]->m_entity->Translate(m_treeArray[nodeIndex].m_entities[entityIndex]->m_translation);

			// Store instance descriptor (Note that the entity queue itself keeps one reference to the entity's frame)
			InstanceDesc instanceDesc;

			instanceDesc.m_nodeIndex = nodeIndex;
			instanceDesc.m_entityDesc = m_treeArray[nodeIndex].m_entities[entityIndex];
			instanceDesc.m_entity = m_treeArray[nodeIndex].m_entities[entityIndex]->m_entity;
			instanceDesc.m_entity->AddRef();

			UINT64 key = reinterpret_cast<UINT64>(entity);
			DWORD keyHash = RuGlobalHashGen().GetHashCode(key);

			InstanceDesc reclaimedInstanceDesc;
			while(m_entityQueue->Reclaim(key, keyHash, 1, reclaimedInstanceDesc))
			{
				// Release instanced entity
				ruSAFE_RELEASE(reclaimedInstanceDesc.m_entity);
				ruSAFE_RELEASE(reclaimedInstanceDesc.m_entityDesc->m_entity);
			}

			m_entityQueue->Add(key, keyHash, instanceDesc, 1);

			// Force update of the entity (temporary)
			m_treeArray[nodeIndex].m_entities[entityIndex]->m_entity->Update(0.0f);
		}
	}

	return m_treeArray[nodeIndex].m_entities[entityIndex]->m_entity;
}

BOOL CRuEntityContainer_Quadtree::Tree::WriteNodeHeader(IRuStream *stream, INT32 headerOffset, TreeNode &node, INT32 nodeIndex)
{
	// Seek to the position of the node header
	INT32 nodeOffset = headerOffset + m_headerSize + nodeIndex * m_nodeSize;
	stream->Seek(nodeOffset, ruSSM_Begin);

	stream->Write(&node.m_iterationNumber, sizeof(INT32));
	stream->Write(&node.m_blockOffset, sizeof(INT32));
	stream->Write(&node.m_blockSize, sizeof(INT32));
	stream->Write(&node.m_dataSize, sizeof(INT32));

	stream->Write(&node.m_bounds.Minima().x, sizeof(float));
	stream->Write(&node.m_bounds.Minima().y, sizeof(float));
	stream->Write(&node.m_bounds.Minima().z, sizeof(float));
	stream->Write(&node.m_bounds.Maxima().x, sizeof(float));
	stream->Write(&node.m_bounds.Maxima().y, sizeof(float));
	stream->Write(&node.m_bounds.Maxima().z, sizeof(float));

	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::Tree::WriteNodeData(IRuStream *stream, INT32 headerOffset, TreeNode &node, INT32 nodeIndex)
{
	// Serialize node to memory stream
	m_decompressedChunkStream->SetStreamSize(0);
	node.SerializeTo(m_decompressedChunkStream);

	// Compress memory stream
	m_compressedChunkStream->SetStreamSize(0);
	m_streamCompressor->Compress(m_decompressedChunkStream, m_compressedChunkStream);

	// Store the compressed node stream size
	node.m_dataSize = m_compressedChunkStream->GetStreamSize();

	if(node.m_dataSize > node.m_blockSize)
	{
		// Lock the end of the data stream
		INT32 endOfStream = stream->GetStreamSize();
		while(stream->LockStream(endOfStream, 1024, TRUE) != ruSEC_OK)
		{
			// Update the end of stream marker
			endOfStream = stream->GetStreamSize();
		}

		node.m_blockOffset = endOfStream;
		node.m_blockSize = node.m_dataSize;

		// Seek to the position of the node data
		stream->Seek(node.m_blockOffset, ruSSM_Begin);

		// Write the compressed node stream
		stream->Write(m_compressedChunkStream->GetBuffer(), node.m_dataSize);

		// Unlock the end of the data stream
		stream->UnlockStream(endOfStream, 1024);
	}
	else
	{
		// Seek to the position of the node data
		stream->Seek(node.m_blockOffset, ruSSM_Begin);

		// Write the compressed node stream
		stream->Write(m_compressedChunkStream->GetBuffer(), node.m_dataSize);
	}

	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::Tree::FlushEntityQueue()
{
	if(m_entityQueue)
	{
		// Iterate through all instanced entities
		m_entityQueue->ForAllElements(FlushEntityQueueCallback, this);

		// Clear instanced entity queue
		m_entityQueue->Clear();
	}

	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::Tree::FlushEntityQueueCallback(const void *key, const void *data, void *userData)
{
	InstanceDesc &cd = *((InstanceDesc *) data);

	ruSAFE_RELEASE(cd.m_entity);

	return TRUE;
}

CRuEntityContainer_Quadtree::CRuEntityContainer_Quadtree()
:	m_tree(NULL)
{
}

CRuEntityContainer_Quadtree::~CRuEntityContainer_Quadtree()
{
	delete m_tree;
}

BOOL CRuEntityContainer_Quadtree::Initialize(IRuStream *dataStream, INT32 headerOffset, const CRuAABB &bounds, const CRuVector3 &maxLeafExtents)
{
	// Build initial tree
	m_tree = ruNEW CRuEntityContainer_Quadtree::Tree(dataStream, headerOffset, bounds, maxLeafExtents);

	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::Open(IRuStream *dataStream, INT32 headerOffset, BOOL readOnly)
{
	// Build tree from the data stream
	m_tree = ruNEW CRuEntityContainer_Quadtree::Tree(dataStream, headerOffset, readOnly);

	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::ExportEntityHeader(IRuStream *dataStream, INT32 headerOffset)
{
	return m_tree->SerializeToHeaderStream(dataStream, headerOffset);
}

BOOL CRuEntityContainer_Quadtree::ExportTree(IRuStream *dataStream, INT32 headerOffset)
{
	return m_tree->ExportTree(dataStream, headerOffset);
}

BOOL CRuEntityContainer_Quadtree::AddEntity(const char *resName, const CRuMatrix4x4 &transform, const CRuSphere &bounds)
{
	// Not done -- should perform locking
	return m_tree->AddEntity(resName, transform, bounds);
}

BOOL CRuEntityContainer_Quadtree::RemoveEntity(CRuEntity *entityFrame)
{
	return m_tree->RemoveEntity(entityFrame);
}

BOOL CRuEntityContainer_Quadtree::UpdateEntity(CRuEntity *entityFrame)
{
	return m_tree->UpdateEntityDescriptor(entityFrame, entityFrame->GetLocalTransform());
}

BOOL CRuEntityContainer_Quadtree::EnumerateEntityDescriptors(CRuArrayList<char *> &resNameList, CRuArrayList<CRuMatrix4x4> &transformList, CRuArrayList<CRuSphere> &boundsList)
{
	return m_tree->EnumerateEntityDescriptors(resNameList, transformList, boundsList);
}

BOOL CRuEntityContainer_Quadtree::UpdateAllEntityBounds()
{
	return m_tree->UpdateAllEntityBounds();
}

BOOL CRuEntityContainer_Quadtree::Save()
{
	return m_tree->FlushLockedNodes();
}

void CRuEntityContainer_Quadtree::SetCamera(CRuCamera *viewCamera)
{
	m_entityDescriptors.Clear();

	// Enumerate all entities within sweeping radius of the view camera
	m_tree->EnumerateEntities(viewCamera->GetSweepBounds(), m_entityDescriptors);

	CRuVector3 eyePosition = viewCamera->GetCameraPosition();

	for(INT32 i = 0; i < m_entityDescriptors.Count(); ++i)
	{
		if(m_entityDescriptors[i]->m_entity)
		{
			// Store distance to camera
			m_entityDescriptors[i]->m_distanceToCamera = SquaredDistance(eyePosition, m_entityDescriptors[i]->m_bounds);
		}
	}
/*
	float distanceClasses[16][3] =
	{
		{	2500.0f,	3000.0f,	4000.0f },
		{	2500.0f,	3000.0f,	4000.0f },
		{	2500.0f,	3000.0f,	4000.0f },
		{	2500.0f,	3000.0f,	4000.0f },
		{	 700.0f,	1050.0f,	1200.0f },
		{	 700.0f,	1050.0f,	1200.0f },
		{	 700.0f,	1050.0f,	1200.0f },
		{	 700.0f,	1050.0f,	1200.0f },
		{	 400.0f,	 600.0f,	 700.0f },
		{	 400.0f,	 600.0f,	 700.0f },
		{	 400.0f,	 600.0f,	 700.0f },
		{	 400.0f,	 600.0f,	 700.0f },
		{	 100.0f,	 300.0f,	 400.0f },
		{	 100.0f,	 300.0f,	 400.0f },
		{	 100.0f,	 300.0f,	 400.0f },
		{	 100.0f,	 300.0f,	 400.0f }
	};

	// Pre-square all of the distance class distances
	for(INT32 i = 0; i < 16; ++i)
	{
		for(INT32 j = 0; j < 3; ++j)
		{
			distanceClasses[i][j] *= distanceClasses[i][j];
		}
	}

	m_entities.Clear();
	for(INT32 i = 0; i < m_entityDescriptors.Count(); ++i)
	{
		if(m_entityDescriptors[i]->m_entity)
		{
			INT32 distanceClass = m_entityDescriptors[i]->m_entity->GetOption(ruENTITYOPTION_DISTANCECLASS);
			float distanceToEntity = SquaredDistance(eyePosition, m_entityDescriptors[i]->m_bounds);

			if(m_entityDescriptors[i]->m_beingControlled != false)
			{
				// If the entity is already being controlled by a controller, simply add it to the visible entity list
				m_entities.Add(m_entityDescriptors[i]->m_entity);
			}
			// If the entity was not visible before...
			else if(m_entityDescriptors[i]->m_fullyVisible == FALSE)
			{
				if(distanceToEntity <= distanceClasses[distanceClass][0])
				{
					// If the distance is less than the absolute visibility distance, do nothing and mark it visible
					m_entityDescriptors[i]->m_fullyVisible = TRUE;

					m_entities.Add(m_entityDescriptors[i]->m_entity);
				}
				else if(distanceToEntity <= distanceClasses[distanceClass][1])
				{
					m_entityDescriptors[i]->m_beingControlled = TRUE;

					// If the distance is less than the fade in distance, put a fadein controller on it
					FadeDesc fadeDesc;

					fadeDesc.m_targetEntity = m_entityDescriptors[i];
					fadeDesc.m_isFadeOutController = FALSE;

					fadeDesc.m_fadeController = ruNEW CRuController_DistanceFader();
					fadeDesc.m_fadeController->SetTarget(fadeDesc.m_targetEntity->m_entity);
					fadeDesc.m_fadeController->SetFadeType(fadeDesc.m_isFadeOutController);
					fadeDesc.m_fadeController->SetDuration(1.0f);
					fadeDesc.m_fadeController->Update(0.0f);

					m_fadeControllers.Add(fadeDesc);

					m_entities.Add(m_entityDescriptors[i]->m_entity);
				}
			}
			else
			{
				if(distanceToEntity >= distanceClasses[distanceClass][2])
				{
					m_entityDescriptors[i]->m_beingControlled = TRUE;

					// If the distance is greater than the fade out distance, put a fadeout controller on it
					FadeDesc fadeDesc;

					fadeDesc.m_targetEntity = m_entityDescriptors[i];
					fadeDesc.m_isFadeOutController = TRUE;

					fadeDesc.m_fadeController = ruNEW CRuController_DistanceFader();
					fadeDesc.m_fadeController->SetTarget(fadeDesc.m_targetEntity->m_entity);
					fadeDesc.m_fadeController->SetFadeType(fadeDesc.m_isFadeOutController);
					fadeDesc.m_fadeController->SetDuration(1.0f);
					fadeDesc.m_fadeController->Update(0.0f);

					m_fadeControllers.Add(fadeDesc);
				}

				m_entities.Add(m_entityDescriptors[i]->m_entity);
			}
		}
	}
*/
}

IRuObject *CRuEntityContainer_Quadtree::Clone(IRuObject *clonedObject)
{
	return NULL;
}

BOOL CRuEntityContainer_Quadtree::SerializeFrom(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::SerializeTo(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::ResetStates()
{
	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::Update(float elapsedTime)
{
	float distanceClasses[16][3] =
	{
		{	2500.0f,	3000.0f,	4000.0f },
		{	2500.0f,	3000.0f,	4000.0f },
		{	2500.0f,	3000.0f,	4000.0f },
		{	2500.0f,	3000.0f,	4000.0f },
		{	 700.0f,	1050.0f,	1200.0f },
		{	 700.0f,	1050.0f,	1200.0f },
		{	 700.0f,	1050.0f,	1200.0f },
		{	 700.0f,	1050.0f,	1200.0f },
		{	 400.0f,	 600.0f,	 700.0f },
		{	 400.0f,	 600.0f,	 700.0f },
		{	 400.0f,	 600.0f,	 700.0f },
		{	 400.0f,	 600.0f,	 700.0f },
		{	 100.0f,	 300.0f,	 400.0f },
		{	 100.0f,	 300.0f,	 400.0f },
		{	 100.0f,	 300.0f,	 400.0f },
		{	 100.0f,	 300.0f,	 400.0f }
	};

/*
	float distanceClasses[16][3] =
	{
		{	2500.0f,	3000.0f,	4000.0f },
		{	2500.0f,	3000.0f,	4000.0f },
		{	2500.0f,	3000.0f,	4000.0f },
		{	2500.0f,	3000.0f,	4000.0f },
		{	1200.0f,	1400.0f,	1600.0f },
		{	1200.0f,	1400.0f,	1600.0f },
		{	1200.0f,	1400.0f,	1600.0f },
		{	1200.0f,	1400.0f,	1600.0f },
		{	 600.0f,	 800.0f,	 950.0f },
		{	 600.0f,	 800.0f,	 950.0f },
		{	 600.0f,	 800.0f,	 950.0f },
		{	 600.0f,	 800.0f,	 950.0f },
		{	 100.0f,	 300.0f,	 400.0f },
		{	 100.0f,	 300.0f,	 400.0f },
		{	 100.0f,	 300.0f,	 400.0f },
		{	 100.0f,	 300.0f,	 400.0f }
	};
*/
	// Pre-square all of the distance class distances
	for(INT32 i = 0; i < 16; ++i)
	{
		for(INT32 j = 0; j < 3; ++j)
		{
			distanceClasses[i][j] *= distanceClasses[i][j];
		}
	}

	// Update fade controllers
	for(INT32 i = 0; i < m_fadeControllers.Count(); ++i)
	{
		// Update fade controller
		m_fadeControllers[i].m_fadeController->Update(elapsedTime);

		// Check fade controller status
		if(m_fadeControllers[i].m_fadeController->HasExpired())
		{
			if(m_fadeControllers[i].m_isFadeOutController)
			{
				m_fadeControllers[i].m_targetEntity->m_fullyVisible = FALSE;
			}
			else
			{
				m_fadeControllers[i].m_targetEntity->m_fullyVisible = TRUE;
			}

			m_fadeControllers[i].m_targetEntity->m_beingControlled = FALSE;

			// Release controller
			ruSAFE_RELEASE(m_fadeControllers[i].m_fadeController);

			// TODO: possibly release ref to entity

			// Remove controller
			m_fadeControllers.RemoveAt(i);
			--i;
		}
	}

	m_entities.Clear();
	for(INT32 i = 0; i < m_entityDescriptors.Count(); ++i)
	{
		if(m_entityDescriptors[i]->m_entity)
		{
			INT32 distanceClass = m_entityDescriptors[i]->m_entity->GetOption(ruENTITYOPTION_DISTANCECLASS);

			if(m_entityDescriptors[i]->m_beingControlled != false)
			{
				// If the entity is already being controlled by a controller, simply add it to the visible entity list
				m_entities.Add(m_entityDescriptors[i]->m_entity);
			}
			// If the entity was not visible before...
			else if(m_entityDescriptors[i]->m_fullyVisible == FALSE)
			{
				if(m_entityDescriptors[i]->m_distanceToCamera <= distanceClasses[distanceClass][0])
				{
					// If the distance is less than the absolute visibility distance, do nothing and mark it visible
					m_entityDescriptors[i]->m_fullyVisible = TRUE;

					m_entities.Add(m_entityDescriptors[i]->m_entity);
				}
				else if(m_entityDescriptors[i]->m_distanceToCamera <= distanceClasses[distanceClass][1])
				{
					m_entityDescriptors[i]->m_beingControlled = TRUE;

					// If the distance is less than the fade in distance, put a fadein controller on it
					FadeDesc fadeDesc;

					fadeDesc.m_targetEntity = m_entityDescriptors[i];
					fadeDesc.m_isFadeOutController = FALSE;

					fadeDesc.m_fadeController = ruNEW CRuController_DistanceFader();
					fadeDesc.m_fadeController->SetTarget(fadeDesc.m_targetEntity->m_entity);
					fadeDesc.m_fadeController->SetFadeType(fadeDesc.m_isFadeOutController);
					fadeDesc.m_fadeController->SetDuration(1.0f);
					fadeDesc.m_fadeController->Update(0.0f);

					m_fadeControllers.Add(fadeDesc);

					m_entities.Add(m_entityDescriptors[i]->m_entity);
				}
			}
			else
			{
				if(m_entityDescriptors[i]->m_distanceToCamera >= distanceClasses[distanceClass][2])
				{
					m_entityDescriptors[i]->m_beingControlled = TRUE;

					// If the distance is greater than the fade out distance, put a fadeout controller on it
					FadeDesc fadeDesc;

					fadeDesc.m_targetEntity = m_entityDescriptors[i];
					fadeDesc.m_isFadeOutController = TRUE;

					fadeDesc.m_fadeController = ruNEW CRuController_DistanceFader();
					fadeDesc.m_fadeController->SetTarget(fadeDesc.m_targetEntity->m_entity);
					fadeDesc.m_fadeController->SetFadeType(fadeDesc.m_isFadeOutController);
					fadeDesc.m_fadeController->SetDuration(1.0f);
					fadeDesc.m_fadeController->Update(0.0f);

					m_fadeControllers.Add(fadeDesc);
				}

				m_entities.Add(m_entityDescriptors[i]->m_entity);
			}
		}
	}

	// Update visible entities
	for(INT32 i = 0; i < m_entities.Count(); ++i)
	{
		m_entities[i]->Update(elapsedTime);
	}

	return TRUE;
}

BOOL CRuEntityContainer_Quadtree::QueryCollision(CRuCollisionQuery *colQuery)
{
	// If collision query is disabled, return immediately
	if(colQuery->m_ignoreCollisionFlag == FALSE && GetOption(ruENTITYOPTION_COLLISION) == 0)
		return TRUE;

	BOOL retVal = TRUE;

	switch(colQuery->m_queryType)
	{
		default:
			{
				CRuCollisionQuery_Ray_Base *rayQuery = static_cast<CRuCollisionQuery_Ray_Base *>(colQuery);

				for(INT32 i = 0; i < m_entities.Count(); ++i)
				{
					m_entities[i]->QueryCollision(colQuery);
				}
			}

			break;

		case ruCOLQUERY_SEGMENT:
		case ruCOLQUERY_SEGMENT_HITTEST:
		case ruCOLQUERY_SEGMENT_NEAREST:
			{
				CRuCollisionQuery_Segment_Base *segQuery = static_cast<CRuCollisionQuery_Segment_Base *>(colQuery);

				for(INT32 i = 0; i < m_entities.Count(); ++i)
				{
					m_entities[i]->QueryCollision(colQuery);
				}
			}

			break;

		case ruCOLQUERY_RAY_SUBOBJECT:
			{
				// Cast collision query to CRuCollisionQuery_Ray_SubObject
				CRuCollisionQuery_Ray_SubObject *subObjQuery = static_cast<CRuCollisionQuery_Ray_SubObject *>(colQuery);

				// Copy ray query parameters
				CRuCollisionQuery_Ray rayQuery;
				rayQuery.m_worldTransform = subObjQuery->m_worldTransform;
				rayQuery.m_invWorldTransform = subObjQuery->m_invWorldTransform;
				rayQuery.m_colRay = subObjQuery->m_colRay;

				for(INT32 i = 0; i < m_entities.Count(); ++i)
				{
					// Clear query results
					rayQuery.m_colPoints->Clear();
					rayQuery.m_colDistances->Clear();

					// Test collision with the current entity
					if(m_entities[i]->QueryCollision(&rayQuery))
					{
						if(rayQuery.m_colPoints->Count() > 0)
						{
							// Sort the results using insertion sort
							m_insertionSort.Sort(rayQuery.m_colDistances->GetArray(), rayQuery.m_colDistances->Count());
							UINT32 *sortedIndices = m_insertionSort.GetSortedIndices();

							// Insert sub-object entry into results
							subObjQuery->m_colObjects->Add(m_entities[i]);
							subObjQuery->m_colDistances->Add(rayQuery.m_colDistances->Get(sortedIndices[0]));
						}
					}
				}
			}

			break;
	}	

	return retVal;
}

INT32 CRuEntityContainer_Quadtree::GetNumEntities()
{
	return m_entities.Count();
}

CRuEntity *CRuEntityContainer_Quadtree::GetEntity(INT32 index)
{
	if(index >= 0 && index < m_entities.Count())
	{
		return m_entities[index];
	}

	return NULL;
}

// ************************************************************************************************************************************************************

#pragma managed
