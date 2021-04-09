#include "../../Scene/Terrain/RuWorld_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuWorld_ChunkTree, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_ChunkTree, "CRuWorld_ChunkTree", "IRuObject")
ruCLASSGUID_DEFINE(CRuWorld_ChunkTree, 0xDFD72A85, 0x40394181, 0xBE44CECD, 0x8D2B0527)

ruRTTI_DEFINE_SUBCLASS(CRuWorld_EntityDescriptor, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_EntityDescriptor, "CRuWorld_EntityDescriptor", "IRuObject")
ruCLASSGUID_DEFINE(CRuWorld_EntityDescriptor, 0x829C671F, 0xF6B44e14, 0xA1320ECF, 0x93885693)

ruRTTI_DEFINE_SUBCLASS(CRuWorld_Trigger, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_Trigger, "CRuWorld_Trigger", "CRuEntity")
ruCLASSGUID_DEFINE(CRuWorld_Trigger, 0x83E36878, 0x3C324a32, 0x97AD4DD7, 0x6A7E7350)

ruRTTI_DEFINE_SUBCLASS(CRuWorld_ZoneSettings, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_ZoneSettings, "CRuWorld_ZoneSettings", "IRuObject")
ruCLASSGUID_DEFINE(CRuWorld_ZoneSettings, 0x4F7B3D5E, 0x605748cd, 0x83444D10, 0xC6FC5225)

ruRTTI_DEFINE_SUBCLASS(CRuWorld_ZoneDescriptor, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_ZoneDescriptor, "CRuWorld_ZoneDescriptor", "IRuObject")
ruCLASSGUID_DEFINE(CRuWorld_ZoneDescriptor, 0x9FFD2585, 0x6DF34e3c, 0x9A1FE244, 0x85B9AD8A)

// ************************************************************************************************************************************************************

CRuWorld_ChunkTree::ChunkTreeNode::ChunkTreeNode()
:	m_chunkArraySize(0),
	m_numChunks(0),
	m_chunks(NULL)
{
	// Initialize children array to NULL
	for(INT32 i = 0; i < 4; ++i)
	{
		m_children[i] = NULL;
	}
}

CRuWorld_ChunkTree::ChunkTreeNode::~ChunkTreeNode()
{
	// Destroy chunk descriptor array
	delete[] m_chunks;

	// Destroy children
	for(INT32 i = 0; i < 4; ++i)
	{
		delete m_children[i];
	}
}

BOOL CRuWorld_ChunkTree::ChunkTreeNode::IsLeaf()
{
	if(m_children[0] == NULL)
		return TRUE;

	return FALSE;
}

// ************************************************************************************************************************************************************

CRuWorld_ChunkTree::CRuWorld_ChunkTree()
:	m_root(NULL)
{
}

CRuWorld_ChunkTree::~CRuWorld_ChunkTree()
{
	delete m_root;
}

BOOL CRuWorld_ChunkTree::Initialize(const CRuAABB &treeBounds, INT32 chunksPerNode)
{
	m_root = ruNEW ChunkTreeNode();
	m_root->m_bounds = treeBounds;

	m_chunksPerNode = chunksPerNode;

	return TRUE;
}

BOOL CRuWorld_ChunkTree::InsertChunk(INT32 x, INT32 z, const CRuAABB &bounds)
{
	return InsertChunk(m_root, x, z, bounds);
}

void CRuWorld_ChunkTree::EnumerateChunks(const CRuPlane4 *frustumPlanes, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut)
{
	EnumerateChunks(m_root, frustumPlanes, chunkKeyOut, chunkBoundsOut);
}

void CRuWorld_ChunkTree::EnumerateChunks(const CRuAABB &box, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut)
{
	EnumerateChunks(m_root, box, chunkKeyOut, chunkBoundsOut);
}

void CRuWorld_ChunkTree::EnumerateChunks(const CRuOBB &box, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut)
{
	EnumerateChunks(m_root, box, chunkKeyOut, chunkBoundsOut);
}

void CRuWorld_ChunkTree::EnumerateChunks(const CRuSphere &sphere, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut)
{
	EnumerateChunks(m_root, sphere, chunkKeyOut, chunkBoundsOut);
}

void CRuWorld_ChunkTree::EnumerateChunks(const CRuRay3 &ray, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut)
{
	EnumerateChunks(m_root, ray, chunkKeyOut, chunkBoundsOut);
}

void CRuWorld_ChunkTree::EnumerateChunks(const CRuSegment3 &segment, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut)
{
	EnumerateChunks(m_root, segment, chunkKeyOut, chunkBoundsOut);
}

BOOL CRuWorld_ChunkTree::PickNearestChunk(const CRuSegment3 &segment, DWORD &chunkKeyOut, CRuAABB &chunkBoundsOut)
{
	REAL distance = FLT_MAX;

	PickNearestChunk(m_root, segment, chunkKeyOut, chunkBoundsOut, distance);

	return distance < FLT_MAX;
}

INT32 CRuWorld_ChunkTree::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuObject::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	footprint += sizeof(m_root) + (m_root ? GetNodeMemoryFootprint(m_root) : 0);
	footprint += sizeof(m_chunksPerNode);

	return footprint;
}

BOOL CRuWorld_ChunkTree::InsertChunk(ChunkTreeNode *node, INT32 x, INT32 z, const CRuAABB &bounds)
{
	CRuVector3 centroid = (bounds.Minima() + bounds.Maxima()) * 0.5f;

	if(node->m_bounds.Contains(centroid))
	{
		if(node->IsLeaf())
		{
			if(node->m_chunkArraySize == 0)
			{
				node->m_chunkArraySize = m_chunksPerNode;
				node->m_numChunks = 0;
				node->m_chunks = ruNEW ChunkDescriptor [node->m_chunkArraySize];
			}

			if(node->m_numChunks >= node->m_chunkArraySize)
			{
				// Subdivide and store into children
				if(Subdivide(node))
				{
					return InsertChunk(node, x, z, bounds);
				}

				return FALSE;
			}
			else
			{
				node->m_chunks[node->m_numChunks].m_x = x;
				node->m_chunks[node->m_numChunks].m_z = z;
				node->m_chunks[node->m_numChunks].m_bounds = bounds;

				++node->m_numChunks;

				return TRUE;
			}
		}
		else
		{
			// Insert the chunk into the first acceptable child
			for(INT32 i = 0; i < 4; ++i)
			{
				if(InsertChunk(node->m_children[i], x, z, bounds) != false)
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL CRuWorld_ChunkTree::Subdivide(ChunkTreeNode *node)
{
	// Create children
	for(INT32 i = 0; i < 4; ++i)
	{
		node->m_children[i] = ruNEW ChunkTreeNode();
		node->m_children[i]->m_bounds.SetBoundsToQuadrantXZ(node->m_bounds, i);
	}

	// Distribute chunks to children
	for(INT32 i = 0; i < node->m_numChunks; ++i)
	{
		if(InsertChunk(node, node->m_chunks[i].m_x, node->m_chunks[i].m_z, node->m_chunks[i].m_bounds) == FALSE)
		{
			return FALSE;
		}
	}

	// Destroy chunk information at node
	delete[] node->m_chunks;

	node->m_chunkArraySize = 0;
	node->m_numChunks = 0;
	node->m_chunks = NULL;

	return TRUE;
}

void CRuWorld_ChunkTree::EnumerateChunks(ChunkTreeNode *node, const CRuPlane4 *frustumPlanes, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut)
{
	// Ensure the node intersects the frustum
	if(IntersectsFrustum(frustumPlanes, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			// Enumerate visible chunks
			for(INT32 i = 0; i < node->m_numChunks; ++i)
			{
				if(IntersectsFrustum(frustumPlanes, node->m_chunks[i].m_bounds))
				{
					// Add chunk key to output list
					chunkKeyOut.Add((node->m_chunks[i].m_x << 16) | node->m_chunks[i].m_z);

					// Add chunk bounds to output list
					if(chunkBoundsOut)
					{
						chunkBoundsOut->Add(node->m_chunks[i].m_bounds);
					}
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				EnumerateChunks(node->m_children[i], frustumPlanes, chunkKeyOut, chunkBoundsOut);
			}
		}
	}
}

void CRuWorld_ChunkTree::EnumerateChunks(ChunkTreeNode *node, const CRuAABB &box, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut)
{
	// Ensure the node intersects the box
	if(Intersects(box, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			// Enumerate visible chunks
			for(INT32 i = 0; i < node->m_numChunks; ++i)
			{
				if(Intersects(box, node->m_chunks[i].m_bounds))
				{
					// Add chunk key to output list
					chunkKeyOut.Add((node->m_chunks[i].m_x << 16) | node->m_chunks[i].m_z);

					// Add chunk bounds to output list
					if(chunkBoundsOut)
					{
						chunkBoundsOut->Add(node->m_chunks[i].m_bounds);
					}
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				EnumerateChunks(node->m_children[i], box, chunkKeyOut, chunkBoundsOut);
			}
		}
	}
}

void CRuWorld_ChunkTree::EnumerateChunks(ChunkTreeNode *node, const CRuOBB &box, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut)
{
	// Ensure the node intersects the box
	if(Intersects(box, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			// Enumerate visible chunks
			for(INT32 i = 0; i < node->m_numChunks; ++i)
			{
				if(Intersects(box, node->m_chunks[i].m_bounds))
				{
					// Add chunk key to output list
					chunkKeyOut.Add((node->m_chunks[i].m_x << 16) | node->m_chunks[i].m_z);

					// Add chunk bounds to output list
					if(chunkBoundsOut)
					{
						chunkBoundsOut->Add(node->m_chunks[i].m_bounds);
					}
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				EnumerateChunks(node->m_children[i], box, chunkKeyOut, chunkBoundsOut);
			}
		}
	}
}

void CRuWorld_ChunkTree::EnumerateChunks(ChunkTreeNode *node, const CRuSphere &sphere, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut)
{
	// Ensure the node intersects the sphere
	if(Intersects(node->m_bounds, sphere))
	{
		if(node->IsLeaf())
		{
			// Enumerate visible chunks
			for(INT32 i = 0; i < node->m_numChunks; ++i)
			{
				if(Intersects(node->m_chunks[i].m_bounds, sphere))
				{
					// Add chunk key to output list
					chunkKeyOut.Add((node->m_chunks[i].m_x << 16) | node->m_chunks[i].m_z);

					// Add chunk bounds to output list
					if(chunkBoundsOut)
					{
						chunkBoundsOut->Add(node->m_chunks[i].m_bounds);
					}
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				EnumerateChunks(node->m_children[i], sphere, chunkKeyOut, chunkBoundsOut);
			}
		}
	}
}

void CRuWorld_ChunkTree::EnumerateChunks(ChunkTreeNode *node, const CRuRay3 &ray, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut)
{
	// Ensure the node intersects the ray
	if(Intersects(ray, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			// Enumerate visible chunks
			for(INT32 i = 0; i < node->m_numChunks; ++i)
			{
				if(Intersects(ray, node->m_chunks[i].m_bounds))
				{
					// Add chunk key to output list
					chunkKeyOut.Add((node->m_chunks[i].m_x << 16) | node->m_chunks[i].m_z);

					// Add chunk bounds to output list
					if(chunkBoundsOut)
					{
						chunkBoundsOut->Add(node->m_chunks[i].m_bounds);
					}
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				EnumerateChunks(node->m_children[i], ray, chunkKeyOut, chunkBoundsOut);
			}
		}
	}
}

void CRuWorld_ChunkTree::EnumerateChunks(ChunkTreeNode *node, const CRuSegment3 &segment, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut)
{
	// Ensure the node intersects the segment
	if(Intersects(segment, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			// Enumerate visible chunks
			for(INT32 i = 0; i < node->m_numChunks; ++i)
			{
				if(Intersects(segment, node->m_chunks[i].m_bounds))
				{
					// Add chunk key to output list
					chunkKeyOut.Add((node->m_chunks[i].m_x << 16) | node->m_chunks[i].m_z);

					// Add chunk bounds to output list
					if(chunkBoundsOut)
					{
						chunkBoundsOut->Add(node->m_chunks[i].m_bounds);
					}
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				EnumerateChunks(node->m_children[i], segment, chunkKeyOut, chunkBoundsOut);
			}
		}
	}
}

void CRuWorld_ChunkTree::PickNearestChunk(ChunkTreeNode *node, const CRuSegment3 &segment, DWORD &chunkKeyOut, CRuAABB &chunkBoundsOut, REAL &distanceOut)
{
	// Ensure the node intersects the segment
	if(Intersects(segment, node->m_bounds))
	{
		if(node->IsLeaf())
		{
			// Enumerate visible chunks
			for(INT32 i = 0; i < node->m_numChunks; ++i)
			{
				CRuVector3 intPt;
				REAL intT;

				if(FindIntersection(segment, node->m_chunks[i].m_bounds, intPt, intT))
				{
					// If the pick distance is currently the shortest, store as output
					if(intT < distanceOut)
					{
						chunkKeyOut = (node->m_chunks[i].m_x << 16) | node->m_chunks[i].m_z;
						chunkBoundsOut = node->m_chunks[i].m_bounds;
						distanceOut = intT;
					}
				}
			}
		}
		else
		{
			// Recurse to children
			for(INT32 i = 0; i < 4; ++i)
			{
				PickNearestChunk(node->m_children[i], segment, chunkKeyOut, chunkBoundsOut, distanceOut);
			}
		}
	}
}

INT32 CRuWorld_ChunkTree::GetNodeMemoryFootprint(ChunkTreeNode *node)
{
	INT32 footprint = 0;

	footprint += sizeof(node->m_bounds);
	footprint += sizeof(node->m_chunkArraySize);
	footprint += sizeof(node->m_numChunks);
	footprint += sizeof(node->m_chunks) + (node->m_chunks ? sizeof(ChunkDescriptor) * node->m_chunkArraySize : 0);

	if(!node->IsLeaf())
	{
		// Recurse to children
		for(INT32 i = 0; i < 4; ++i)
		{
			footprint += GetNodeMemoryFootprint(node->m_children[i]);
		}
	}

	return footprint;
}

// ************************************************************************************************************************************************************

CRuWorld_EntityDescriptor::CRuWorld_EntityDescriptor()
:	m_resourceName(NULL),
	m_zoneFlags(0x00000000),
	m_detailClass(RuWorld::ruDETAILCLASS_NORMAL),
	m_zoneType(RuWorld::ruZONETYPE_OUTDOOR),
	m_visibilityChainSource(CRuGUID::Invalid),
	m_visibilityChainSubPath(CRuGUID::Invalid),
	m_scale(1.0f, 1.0f, 1.0f),
	m_rotation(0.0f, 1.0f, 0.0f, 0.0f),
	m_translation(0.0f, 0.0f, 0.0f),
	m_worldBoundsDirty(FALSE)
{
}

CRuWorld_EntityDescriptor::~CRuWorld_EntityDescriptor()
{
	delete[] m_resourceName;
}

IRuObject *CRuWorld_EntityDescriptor::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuWorld_EntityDescriptor();
	CRuWorld_EntityDescriptor *clonedDescriptor = static_cast<CRuWorld_EntityDescriptor *>(clonedObject);

	// Chain cloning call to parent class
	IRuObject::Clone(clonedObject);

	clonedDescriptor->m_guid = this->GetGUID();
	clonedDescriptor->SetResourceName(this->GetResourceName());
	clonedDescriptor->SetZoneFlags(this->GetZoneFlags());
	clonedDescriptor->SetDetailClass(this->GetDetailClass());
	clonedDescriptor->SetZoneType(this->GetZoneType());
	clonedDescriptor->SetVisibilityChainSource(this->GetVisibilityChainSource());
	clonedDescriptor->SetVisibilityChainSubPath(this->GetVisibilityChainSubPath());
	clonedDescriptor->SetScale(this->GetScale());
	clonedDescriptor->SetRotation(this->GetRotation());
	clonedDescriptor->SetTranslation(this->GetTranslation());
	clonedDescriptor->SetLocalBounds(this->GetLocalBounds());

	return clonedDescriptor;
}

BOOL CRuWorld_EntityDescriptor::SerializeFrom(IRuStream *stream)
{
	// Read class GUID
	CRuGUID classGUID;
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeFrom(stream);

	// Read entity guid
	RuReadGUID(stream, m_guid);

	// Read length of the resource name string (including NULL)
	INT32 resNameLength = 0;
	stream->Read(&resNameLength, sizeof(INT32));

	// Read the resource name string
	m_resourceName = ruNEW char [resNameLength];
	stream->Read(m_resourceName, resNameLength);

	// Read zone flags
	stream->Read(&m_zoneFlags, sizeof(UINT32));

	if(version > 2)
	{
		// Read detail class
		stream->Read(&m_detailClass, sizeof(RuWorld::DetailClass));

		// Read zone type
		stream->Read(&m_zoneType, sizeof(RuWorld::ZoneType));
	}

	if(version > 1)
	{
		// Read visibility chain information
		RuReadGUID(stream, m_visibilityChainSource);
		RuReadGUID(stream, m_visibilityChainSubPath);
	}

	// Read scale
	stream->Read(&m_scale.x, sizeof(REAL));
	stream->Read(&m_scale.y, sizeof(REAL));
	stream->Read(&m_scale.z, sizeof(REAL));

	// Read rotation
	stream->Read(&m_rotation.x, sizeof(REAL));
	stream->Read(&m_rotation.y, sizeof(REAL));
	stream->Read(&m_rotation.z, sizeof(REAL));
	stream->Read(&m_rotation.w, sizeof(REAL));

	// Read translation
	stream->Read(&m_translation.x, sizeof(REAL));
	stream->Read(&m_translation.y, sizeof(REAL));
	stream->Read(&m_translation.z, sizeof(REAL));

	stream->Read(&m_localBounds.Center().x, sizeof(REAL));
	stream->Read(&m_localBounds.Center().y, sizeof(REAL));
	stream->Read(&m_localBounds.Center().z, sizeof(REAL));
	stream->Read(&m_localBounds.Radius(), sizeof(REAL));

	// Read number of linked chunks
	INT32 numLinkedChunks = m_linkedChunks.Count();
	stream->Read(&numLinkedChunks, sizeof(INT32));

	// Read all linked chunks
	m_linkedChunks.SetArraySize(numLinkedChunks);
	m_linkedChunks.SetNumEntries(numLinkedChunks);
	for(INT32 i = 0; i < numLinkedChunks; ++i)
	{
		stream->Read(&m_linkedChunks[i], sizeof(INT32));
	}

	// Set world bounds dirty to TRUE
	m_worldBoundsDirty = TRUE;

	return TRUE;
}

BOOL CRuWorld_EntityDescriptor::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 3;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeTo(stream);

	// Write entity GUID
	stream->Write(m_guid.Data(), m_guid.Size());

	if(m_resourceName)
	{
		// Write length of the resource name string (including NULL)
		INT32 resNameLength = (INT32)strlen(m_resourceName) + 1;
		stream->Write(&resNameLength, sizeof(INT32));

		// Write the resource name string
		stream->Write(m_resourceName, resNameLength);
	}
	else
	{
		// Write length of the resource name string (including NULL)
		INT32 resNameLength = 1;
		stream->Write(&resNameLength, sizeof(INT32));

		// Write the resource name string
		stream->Write("", resNameLength);
	}

	// Write zone flags
	stream->Write(&m_zoneFlags, sizeof(UINT32));

	// Write detail class
	stream->Write(&m_detailClass, sizeof(RuWorld::DetailClass));

	// Write zone type
	stream->Write(&m_zoneType, sizeof(RuWorld::ZoneType));

	// Write visibility chain information
	stream->Write(m_visibilityChainSource.Data(), m_visibilityChainSource.Size());
	stream->Write(m_visibilityChainSubPath.Data(), m_visibilityChainSubPath.Size());

	// Write entity scale
	stream->Write(&m_scale.x, sizeof(REAL));
	stream->Write(&m_scale.y, sizeof(REAL));
	stream->Write(&m_scale.z, sizeof(REAL));

	// Write entity rotation
	stream->Write(&m_rotation.x, sizeof(REAL));
	stream->Write(&m_rotation.y, sizeof(REAL));
	stream->Write(&m_rotation.z, sizeof(REAL));
	stream->Write(&m_rotation.w, sizeof(REAL));

	// Write entity translation
	stream->Write(&m_translation.x, sizeof(REAL));
	stream->Write(&m_translation.y, sizeof(REAL));
	stream->Write(&m_translation.z, sizeof(REAL));

	// Write entity local bounds
	stream->Write(&m_localBounds.Center().x, sizeof(REAL));
	stream->Write(&m_localBounds.Center().y, sizeof(REAL));
	stream->Write(&m_localBounds.Center().z, sizeof(REAL));
	stream->Write(&m_localBounds.Radius(), sizeof(REAL));

	// Write number of linked chunks
	INT32 numLinkedChunks = m_linkedChunks.Count();
	stream->Write(&numLinkedChunks, sizeof(INT32));

	// Write all linked chunks
	for(INT32 i = 0; i < numLinkedChunks; ++i)
	{
		stream->Write(&m_linkedChunks[i], sizeof(INT32));
	}

	return TRUE;
}

INT32 CRuWorld_EntityDescriptor::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuObject::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	footprint += sizeof(m_resourceName) + (m_resourceName ? (INT32)strlen(m_resourceName) + 1 : 0);
	footprint += sizeof(m_zoneFlags);

	footprint += sizeof(m_scale);
	footprint += sizeof(m_rotation);
	footprint += sizeof(m_translation);

	footprint += sizeof(m_worldBoundsDirty);
	footprint += sizeof(m_localBounds);
	footprint += sizeof(m_worldBounds);

	footprint += m_linkedChunks.GetMemoryFootprint();

	return footprint;
}

const char *CRuWorld_EntityDescriptor::GetResourceName() const
{
	return m_resourceName ? m_resourceName : "";
}

void CRuWorld_EntityDescriptor::SetResourceName(const char *resName)
{
	delete[] m_resourceName;
	m_resourceName = ruNEW char [strlen(resName) + 1];
	strcpy(m_resourceName, resName);
}

UINT32 CRuWorld_EntityDescriptor::GetZoneFlags() const
{
	return m_zoneFlags;
}

void CRuWorld_EntityDescriptor::SetZoneFlags(UINT32 zoneFlags)
{
	m_zoneFlags = zoneFlags;
}

RuWorld::DetailClass CRuWorld_EntityDescriptor::GetDetailClass() const
{
	return m_detailClass;
}

void CRuWorld_EntityDescriptor::SetDetailClass(RuWorld::DetailClass detailClass)
{
	m_detailClass = detailClass;
}

RuWorld::ZoneType CRuWorld_EntityDescriptor::GetZoneType() const
{
	return m_zoneType;
}

void CRuWorld_EntityDescriptor::SetZoneType(RuWorld::ZoneType zoneType)
{
	m_zoneType = zoneType;
}

const CRuGUID &CRuWorld_EntityDescriptor::GetVisibilityChainSource() const
{
	return m_visibilityChainSource;
}

void CRuWorld_EntityDescriptor::SetVisibilityChainSource(const CRuGUID &guid)
{
	m_visibilityChainSource = guid;
}

const CRuGUID &CRuWorld_EntityDescriptor::GetVisibilityChainSubPath() const
{
	return m_visibilityChainSubPath;
}

void CRuWorld_EntityDescriptor::SetVisibilityChainSubPath(const CRuGUID &guid)
{
	m_visibilityChainSubPath = guid;
}

const CRuVector3 &CRuWorld_EntityDescriptor::GetScale() const
{
	return m_scale;
}

void CRuWorld_EntityDescriptor::SetScale(const CRuVector3 &scale)
{
	m_scale = scale;
	m_worldBoundsDirty = TRUE;
}

const CRuQuaternion &CRuWorld_EntityDescriptor::GetRotation() const
{
	return m_rotation;
}

void CRuWorld_EntityDescriptor::SetRotation(const CRuQuaternion &rotation)
{
	m_rotation = rotation;
	m_worldBoundsDirty = TRUE;
}

const CRuVector3 &CRuWorld_EntityDescriptor::GetTranslation() const
{
	return m_translation;
}

void CRuWorld_EntityDescriptor::SetTranslation(const CRuVector3 &translation)
{
	m_translation = translation;
	m_worldBoundsDirty = TRUE;
}

const CRuSphere &CRuWorld_EntityDescriptor::GetLocalBounds() const
{
	return m_localBounds;
}

void CRuWorld_EntityDescriptor::SetLocalBounds(const CRuSphere &bounds)
{
	m_localBounds = bounds;
	m_worldBoundsDirty = TRUE;
}

const CRuSphere &CRuWorld_EntityDescriptor::GetWorldBounds()
{
	if(m_worldBoundsDirty)
	{
		CRuMatrix4x4 worldTransform;
		worldTransform.SetToIdentity();
		worldTransform.Scale(m_scale);
		worldTransform.Rotate(m_rotation);
		worldTransform.Translate(m_translation);

		worldTransform.TransformPoint(m_localBounds.Center(), m_worldBounds.Center());
		m_worldBounds.Radius() = m_localBounds.Radius() * max(m_scale.m_x, max(m_scale.m_y, m_scale.m_z));
	}

	return m_worldBounds;
}

INT32 CRuWorld_EntityDescriptor::GetNumLinkedChunks() const
{
	return m_linkedChunks.Count();
}

INT32 CRuWorld_EntityDescriptor::GetLinkedChunk(INT32 index) const
{
	return m_linkedChunks[index];
}

BOOL CRuWorld_EntityDescriptor::InsertLinkedChunk(INT32 chunkID)
{
	if(m_linkedChunks.Contains(chunkID) == FALSE)
	{
		m_linkedChunks.Add(chunkID);
	}

	return TRUE;
}

BOOL CRuWorld_EntityDescriptor::DeleteLinkedChunk(INT32 chunkID)
{
	m_linkedChunks.Remove(chunkID);

	return TRUE;
}

void CRuWorld_EntityDescriptor::ClearLinkedChunks()
{
	m_linkedChunks.Clear();
}

// ************************************************************************************************************************************************************

CRuWorld_Trigger::CRuWorld_Trigger()
:	m_boxGizmo(NULL),
	m_triggerType(RuWorld::ruTRIGGERTYPE_ZONE),
	m_zoneID(0),
	m_zoneType(RuWorld::ruZONETYPE_OUTDOOR),
	m_commandString(NULL)
{
	// Set default options
	this->SetOption(ruENTITYOPTION_EXCLUDEBOUNDS, TRUE);

	// Create box gizmo
	m_boxGizmo = ruNEW CRuGizmo_Box();
	this->AddChild(m_boxGizmo);
}

CRuWorld_Trigger::~CRuWorld_Trigger()
{
	ruSAFE_RELEASE(m_boxGizmo);
	delete[] m_commandString;
}

IRuObject *CRuWorld_Trigger::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuWorld_Trigger();
	CRuWorld_Trigger *clonedTrigger = static_cast<CRuWorld_Trigger *>(clonedObject);

	// Chain cloning call to parent class
	CRuEntity::Clone(clonedObject);

	// Set bounds into box gizmo
	clonedTrigger->m_boxGizmo->SetBox(m_boxGizmo->GetBox());
	clonedTrigger->m_boxGizmo->Transform(m_boxGizmo->GetLocalTransform());

	// Copy trigger properties except command string (it is deprecated)
	clonedTrigger->m_triggerType = m_triggerType;
	clonedTrigger->m_zoneID = m_zoneID;
	clonedTrigger->m_zoneType = m_zoneType;

	return clonedObject;
}

BOOL CRuWorld_Trigger::SerializeFrom(IRuStream *stream)
{
	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuEntity::SerializeFrom(stream);

	// Read entity guid
	RuReadGUID(stream, m_guid);

	switch(version)
	{
		case 1:
			{
				// Serialize plane gizmo
				CRuGizmo_Plane *planeGizmo = ruNEW CRuGizmo_Plane();
				planeGizmo->SerializeFrom(stream);

				// Convert plane gizmo to box gizmo
				CRuOBB box(planeGizmo->GetPlaneBounds(), CRuMatrix4x4());
				box.Extents()[1] = 5.0f;
				m_boxGizmo->SetBox(box);

				ruSAFE_RELEASE(planeGizmo);

				// Read trigger type
				stream->Read(&m_triggerType, sizeof(RuWorld::TriggerType));

				// Read zone ID
				stream->Read(&m_zoneID, sizeof(INT32));
			}

			break;

		case 2:
			{
				// Serialize box gizmo
				m_boxGizmo->SerializeFrom(stream);

				// Read trigger type
				stream->Read(&m_triggerType, sizeof(RuWorld::TriggerType));

				// Read zone ID
				stream->Read(&m_zoneID, sizeof(INT32));
			}

			break;

		case 3:
		case 4:
			{
				// Serialize box gizmo
				m_boxGizmo->SerializeFrom(stream);

				// Read trigger type
				stream->Read(&m_triggerType, sizeof(RuWorld::TriggerType));

				// Read zone ID
				stream->Read(&m_zoneID, sizeof(INT32));

				if(version > 3)
				{
					// Read zone type
					stream->Read(&m_zoneType, sizeof(RuWorld::ZoneType));
				}

				// Read command string
				::SerializeFrom(stream, &m_commandString);
			}

			break;
	}

	return TRUE;
}

BOOL CRuWorld_Trigger::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 4;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuEntity::SerializeTo(stream);

	// Write entity GUID
	stream->Write(m_guid.Data(), m_guid.Size());

	// Serialize plane gizmo
	m_boxGizmo->SerializeTo(stream);

	// Write trigger type
	stream->Write(&m_triggerType, sizeof(RuWorld::TriggerType));

	// Write zone ID
	stream->Write(&m_zoneID, sizeof(INT32));

	// Write zone type
	stream->Write(&m_zoneType, sizeof(RuWorld::ZoneType));

	// Write command string
	::SerializeTo(stream, &m_commandString);

	return TRUE;
}

INT32 CRuWorld_Trigger::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = CRuEntity::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	footprint += sizeof(m_boxGizmo) + (m_boxGizmo ? m_boxGizmo->GetMemoryFootprint(enumHash) : 0);
	footprint += sizeof(m_triggerType);
	footprint += sizeof(m_zoneID);
	footprint += sizeof(m_commandString) + (m_commandString ? sizeof(m_commandString) + 1 : 0);

	return footprint;
}

void CRuWorld_Trigger::Initialize(const CRuVector3 &origin)
{
	// Set translation
	this->ResetAllTransforms();
	this->Translate(origin);

	// Reset gizmo parameters
}

CRuGizmo_Box *CRuWorld_Trigger::GetGizmo()
{
	return m_boxGizmo;
}

CRuOBB CRuWorld_Trigger::GetTriggerBounds()
{
	// Get bounds from box gizmo and transform by the world transform
	CRuOBB bounds = m_boxGizmo->GetBox();
	bounds.Transform(m_boxGizmo->GetWorldTransform());

	return bounds;
}

void CRuWorld_Trigger::SetTriggerBounds(const CRuOBB &bounds)
{
	// Set bounds into box gizmo
	m_boxGizmo->SetBox(bounds);

	// Move transform to self
	CRuMatrix4x4 boxGizmoTransform = m_boxGizmo->GetLocalTransform();

	this->Transform(boxGizmoTransform, ruTRANSFORMOP_SET);
	m_boxGizmo->ResetAllTransforms();
}

RuWorld::TriggerType CRuWorld_Trigger::GetTriggerType()
{
	return m_triggerType;
}

void CRuWorld_Trigger::SetTriggerType(RuWorld::TriggerType triggerType)
{
	m_triggerType = triggerType;
}

INT32 CRuWorld_Trigger::GetZoneID()
{
	return m_zoneID;
}

void CRuWorld_Trigger::SetZoneID(INT32 zoneID)
{
	m_zoneID = zoneID;
}

RuWorld::ZoneType CRuWorld_Trigger::GetZoneType() const
{
	return m_zoneType;
}

void CRuWorld_Trigger::SetZoneType(RuWorld::ZoneType zoneType)
{
	m_zoneType = zoneType;
}

const char *CRuWorld_Trigger::GetCommandString()
{
	if(m_commandString)
	{
		return m_commandString;
	}

	return "";
}

void CRuWorld_Trigger::SetCommandString(const char *commandString)
{
	if(commandString)
	{
		delete[] m_commandString;
		m_commandString = ruNEW char [strlen(commandString) + 1];
		strcpy(m_commandString, commandString);
	}
}

// ************************************************************************************************************************************************************

BOOL RuTrigger_GetWarpParameters(CRuWorld_Trigger *trigger, char *targetMapOut, INT32 targetMapBufLen, CRuVector3 *targetPositionOut)
{
	if(trigger && trigger->GetTriggerType() == RuWorld::ruTRIGGERTYPE_WARP)
	{
		CRuStringTokenizer strTokenizer;
		strTokenizer.Tokenize(trigger->GetCommandString(), CRuString(" "));

		if(strTokenizer.GetNumTokens() == 4)
		{
			const CRuString &targetMap = strTokenizer.GetToken(0);
			const CRuString &targetX = strTokenizer.GetToken(1);
			const CRuString &targetY = strTokenizer.GetToken(2);
			const CRuString &targetZ = strTokenizer.GetToken(3);

			// Make sure the target map buffer is large enough
			if(targetMap.GetLength() + 1 > targetMapBufLen)
			{
				return FALSE;
			}

			// Copy target map
			strcpy(targetMapOut, targetMap);

			// Copy target position
			targetPositionOut->Set(static_cast<REAL>(atof(targetX)), static_cast<REAL>(atof(targetY)), static_cast<REAL>(atof(targetZ)));

			return TRUE;
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

CRuWorld_ZoneSettings::CRuWorld_ZoneSettings()
{
	m_color_Primary = 0xFF777777;
	m_color_Secondary = 0xFF202020;
	m_color_Ambient = 0xFF101010;
	m_color_Specular = 0xFF303030;
	m_color_Fog = 0xFF707070;

	m_fog_Start = 400.0f;
	m_fog_End = 1200.0f;
	m_fog_Constant = 0.0f;

	m_theta_Standard = 15.0f * ruPI / 180.0f;
	m_phi_Standard = 35.0f * ruPI / 180.0f;
	m_theta_Specular = 15.0f * ruPI / 180.0f;
	m_phi_Specular = 15.0f * ruPI / 180.0f;

	m_weatherSystem = NULL;

	m_skyGradientTextureName = NULL;

	INT32 octaveSize[8] = { 1024, 512, 128, 128, 128, 128, 128, 128 };
	REAL octaveScale[8] = { 0.010f, 0.020f, 0.040f, 0.080f, 0.160f, 0.320f, 0.640f, 0.960f };
	REAL octaveWeight[8] = { 0.50000000f, 0.25000000f, 0.12500000f, 0.06250000f, 0.03125000f, 0.01562500f, 0.00781250f, 0.00390625f };
	REAL octaveScrollSpeed[8] = { 0.0020f, 0.0020f, 0.0020f, 0.0020f, 0.0025f, 0.0025f, 0.0025f, 0.0025f };

	m_cloudCover = 0.45f;
	m_cloudSharpness = 0.96f;
	m_cloudColor0 = 0xFF000000;
	m_cloudColor1 = 0xFFFFFFFF;

	m_baseOctaveSize = 1024;

	for(INT32 i = 0; i < 8; ++i)
	{
		m_octaveSize[i] = octaveSize[i];
		m_octaveScale[i] = octaveScale[i];
		m_octaveWeight[i] = octaveWeight[i];
		m_octaveScrollSpeed[i] = octaveScrollSpeed[i];
	}

	m_customSkyBoxName = NULL;
	m_frameRateMultiplier = 1.0f;

	m_ambientSound = NULL;

	m_themeMusic = NULL;
	m_themeMusicOccurrenceRate = 1.0f;
}

CRuWorld_ZoneSettings::~CRuWorld_ZoneSettings()
{
	delete[] m_weatherSystem;

	delete[] m_skyGradientTextureName;

	delete[] m_customSkyBoxName;

	delete[] m_ambientSound;

	delete[] m_themeMusic;
}

BOOL CRuWorld_ZoneSettings::SerializeFrom(IRuStream *stream)
{
	DWORD dummyDWORD;
	INT32 dummyINT32;

	// Read class GUID
	CRuGUID classGUID;
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeFrom(stream);

	switch(version)
	{
		case 1:
			{
				// Read time group
				stream->Read(&m_timeGroup, sizeof(INT32));

				// Read weather group
				stream->Read(&m_weatherGroup, sizeof(INT32));

				// Read primary color
				stream->Read(&m_color_Primary, sizeof(DWORD));

				// Read secondary color
				stream->Read(&m_color_Secondary, sizeof(DWORD));

				// Read ambient color
				stream->Read(&m_color_Ambient, sizeof(DWORD));

				// Read specular color
				stream->Read(&m_color_Specular, sizeof(DWORD));

				// Read fog color
				stream->Read(&m_color_Fog, sizeof(DWORD));

				// Read fog start
				stream->Read(&m_fog_Start, sizeof(float));

				// Read fog end
				stream->Read(&m_fog_End, sizeof(float));

				// Read theta
				stream->Read(&m_theta_Standard, sizeof(float));

				// Read phi
				stream->Read(&m_phi_Standard, sizeof(float));

				// Read theta
				stream->Read(&m_theta_Specular, sizeof(float));

				// Read phi
				stream->Read(&m_phi_Specular, sizeof(float));

				stream->Read(&dummyDWORD, sizeof(DWORD));
				stream->Read(&dummyDWORD, sizeof(DWORD));
				stream->Read(&dummyINT32, sizeof(INT32));
				stream->Read(&dummyINT32, sizeof(INT32));

				// Read length of sky gradient texture name including NULL
				INT32 skyGradientTextureNameLength;
				stream->Read(&skyGradientTextureNameLength, sizeof(INT32));

				// Read sky gradient texture name
				if(skyGradientTextureNameLength > 0)
				{
					m_skyGradientTextureName = ruNEW char [skyGradientTextureNameLength];
					stream->Read(m_skyGradientTextureName, sizeof(char) * skyGradientTextureNameLength);
				}

				// Read length of horizon cloud texture name including NULL
				INT32 horizonCloudTextureNameLength;
				stream->Read(&horizonCloudTextureNameLength, sizeof(INT32));

				// Skip horizon cloud texture name
				if(horizonCloudTextureNameLength > 0)
				{
					stream->Seek(horizonCloudTextureNameLength, ruSSM_Current);
				}

				// Read length of overhead cloud texture name including NULL
				INT32 overheadCloudTextureNameLength;
				stream->Read(&overheadCloudTextureNameLength, sizeof(INT32));

				// Skip overhead cloud texture name
				if(overheadCloudTextureNameLength > 0)
				{
					stream->Seek(overheadCloudTextureNameLength, ruSSM_Current);
				}

				// Read length of custom sky box name including NULL
				INT32 customSkyBoxNameLength;
				stream->Read(&customSkyBoxNameLength, sizeof(INT32));

				// Read custom sky box name
				if(customSkyBoxNameLength > 0)
				{
					m_customSkyBoxName = ruNEW char [customSkyBoxNameLength];
					stream->Read(m_customSkyBoxName, sizeof(char) * customSkyBoxNameLength);
				}

				// Write frame rate multiplier
				stream->Read(&m_frameRateMultiplier, sizeof(REAL));

				// Read cloud mode
				stream->Read(&dummyDWORD, sizeof(DWORD));

				// Read ambient sound
				INT32 ambientSoundLength;
				stream->Read(&ambientSoundLength, sizeof(INT32));
				m_ambientSound = ruNEW char [ambientSoundLength];
				stream->Read(m_ambientSound, ambientSoundLength);

				// Read theme music
				INT32 themeMusicLength;
				stream->Read(&themeMusicLength, sizeof(INT32));
				m_themeMusic = ruNEW char [themeMusicLength];
				stream->Read(m_themeMusic, themeMusicLength);
			}

			break;

		case 2:
		case 3:
		case 4:
			{
				// Read time group
				stream->Read(&m_timeGroup, sizeof(INT32));

				// Read weather group
				stream->Read(&m_weatherGroup, sizeof(INT32));

				// Read primary color
				stream->Read(&m_color_Primary, sizeof(DWORD));

				// Read secondary color
				stream->Read(&m_color_Secondary, sizeof(DWORD));

				// Read ambient color
				stream->Read(&m_color_Ambient, sizeof(DWORD));

				// Read specular color
				stream->Read(&m_color_Specular, sizeof(DWORD));

				// Read fog color
				stream->Read(&m_color_Fog, sizeof(DWORD));

				// Read fog start
				stream->Read(&m_fog_Start, sizeof(float));

				// Read fog end
				stream->Read(&m_fog_End, sizeof(float));

				// Read theta
				stream->Read(&m_theta_Standard, sizeof(float));

				// Read phi
				stream->Read(&m_phi_Standard, sizeof(float));

				// Read theta
				stream->Read(&m_theta_Specular, sizeof(float));

				// Read phi
				stream->Read(&m_phi_Specular, sizeof(float));

				if(version > 3)
				{
					// Read length of weather system including NULL
					INT32 weatherSystemLength;
					stream->Read(&weatherSystemLength, sizeof(INT32));

					// Read weather system
					if(weatherSystemLength > 0)
					{
						m_weatherSystem = ruNEW char [weatherSystemLength];
						stream->Read(m_weatherSystem, sizeof(char) * weatherSystemLength);
					}
				}

				// Read length of sky gradient texture name including NULL
				INT32 skyGradientTextureNameLength;
				stream->Read(&skyGradientTextureNameLength, sizeof(INT32));

				// Read sky gradient texture name
				if(skyGradientTextureNameLength > 0)
				{
					m_skyGradientTextureName = ruNEW char [skyGradientTextureNameLength];
					stream->Read(m_skyGradientTextureName, sizeof(char) * skyGradientTextureNameLength);
				}

				// Read cloud settings
				stream->Read(&m_cloudCover, sizeof(REAL));
				stream->Read(&m_cloudSharpness, sizeof(REAL));
				stream->Read(&m_cloudColor0, sizeof(RUCOLOR));
				stream->Read(&m_cloudColor1, sizeof(RUCOLOR));

				stream->Read(&m_baseOctaveSize, sizeof(INT32));

				stream->Read(m_octaveSize, sizeof(INT32) * 8);
				stream->Read(m_octaveScale, sizeof(REAL) * 8);
				stream->Read(m_octaveWeight, sizeof(REAL) * 8);
				stream->Read(m_octaveScrollSpeed, sizeof(REAL) * 8);

				// Read length of custom sky box name including NULL
				INT32 customSkyBoxNameLength;
				stream->Read(&customSkyBoxNameLength, sizeof(INT32));

				// Read custom sky box name
				if(customSkyBoxNameLength > 0)
				{
					m_customSkyBoxName = ruNEW char [customSkyBoxNameLength];
					stream->Read(m_customSkyBoxName, sizeof(char) * customSkyBoxNameLength);
				}

				// Write frame rate multiplier
				stream->Read(&m_frameRateMultiplier, sizeof(REAL));

				// Read ambient sound
				INT32 ambientSoundLength;
				stream->Read(&ambientSoundLength, sizeof(INT32));
				m_ambientSound = ruNEW char [ambientSoundLength];
				stream->Read(m_ambientSound, ambientSoundLength);

				// Read theme music
				INT32 themeMusicLength;
				stream->Read(&themeMusicLength, sizeof(INT32));
				m_themeMusic = ruNEW char [themeMusicLength];
				stream->Read(m_themeMusic, themeMusicLength);

				if(version > 2)
				{
					// Read theme music occurrence rate
					stream->Read(&m_themeMusicOccurrenceRate, sizeof(REAL));
				}
			}

			break;
	}

	// Error trapping
	if(m_timeGroup < 0 || m_weatherGroup < 0)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CRuWorld_ZoneSettings::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 4;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeTo(stream);

	// Stream identification key
	stream->Write(&m_timeGroup, sizeof(INT32));
	stream->Write(&m_weatherGroup, sizeof(INT32));

	// Colors
	stream->Write(&m_color_Primary, sizeof(DWORD));
	stream->Write(&m_color_Secondary, sizeof(DWORD));
	stream->Write(&m_color_Ambient, sizeof(DWORD));
	stream->Write(&m_color_Specular, sizeof(DWORD));
	stream->Write(&m_color_Fog, sizeof(DWORD));

	// Fog
	stream->Write(&m_fog_Start, sizeof(float));
	stream->Write(&m_fog_End, sizeof(float));

	// Light setup
	stream->Write(&m_theta_Standard, sizeof(float));
	stream->Write(&m_phi_Standard, sizeof(float));
	stream->Write(&m_theta_Specular, sizeof(float));
	stream->Write(&m_phi_Specular, sizeof(float));

	// Write length of weather system including NULL
	INT32 weatherSystemLength = m_weatherSystem ? (INT32)strlen(m_weatherSystem) + 1 : 0;
	stream->Write(&weatherSystemLength, sizeof(INT32));

	// Write weather system
	if(weatherSystemLength > 0)
	{
		stream->Write(m_weatherSystem, sizeof(char) * weatherSystemLength);
	}

	// Write length of sky gradient texture name including NULL
	INT32 skyGradientTextureNameLength = m_skyGradientTextureName ? (INT32)strlen(m_skyGradientTextureName) + 1 : 0;
	stream->Write(&skyGradientTextureNameLength, sizeof(INT32));

	// Write sky gradient texture name
	if(skyGradientTextureNameLength > 0)
	{
		stream->Write(m_skyGradientTextureName, sizeof(char) * skyGradientTextureNameLength);
	}

	// Write cloud settings
	stream->Write(&m_cloudCover, sizeof(REAL));
	stream->Write(&m_cloudSharpness, sizeof(REAL));
	stream->Write(&m_cloudColor0, sizeof(RUCOLOR));
	stream->Write(&m_cloudColor1, sizeof(RUCOLOR));

	stream->Write(&m_baseOctaveSize, sizeof(INT32));

	stream->Write(m_octaveSize, sizeof(INT32) * 8);
	stream->Write(m_octaveScale, sizeof(REAL) * 8);
	stream->Write(m_octaveWeight, sizeof(REAL) * 8);
	stream->Write(m_octaveScrollSpeed, sizeof(REAL) * 8);

	// Write length of custom sky box name including NULL
	INT32 customSkyBoxNameLength = m_customSkyBoxName ? (INT32)strlen(m_customSkyBoxName) + 1 : 0;
	stream->Write(&customSkyBoxNameLength, sizeof(INT32));

	// Write custom sky box name
	if(customSkyBoxNameLength > 0)
	{
		stream->Write(m_customSkyBoxName, sizeof(char) * customSkyBoxNameLength);
	}

	// Write frame rate multiplier
	stream->Write(&m_frameRateMultiplier, sizeof(REAL));

	// Write ambient sound
	if(m_ambientSound)
	{
		INT32 ambientSoundLength = (INT32)strlen(m_ambientSound) + 1;
		stream->Write(&ambientSoundLength, sizeof(INT32));
		stream->Write(m_ambientSound, ambientSoundLength);
	}
	else
	{
		INT32 ambientSoundLength = 1;
		stream->Write(&ambientSoundLength, sizeof(INT32));
		stream->Write("", ambientSoundLength);
	}

	// Write theme music
	if(m_themeMusic)
	{
		INT32 themeMusicLength = (INT32)strlen(m_themeMusic) + 1;
		stream->Write(&themeMusicLength, sizeof(INT32));
		stream->Write(m_themeMusic, themeMusicLength);
	}
	else
	{
		INT32 themeMusicLength = 1;
		stream->Write(&themeMusicLength, sizeof(INT32));
		stream->Write("", themeMusicLength);
	}

	stream->Write(&m_themeMusicOccurrenceRate, sizeof(REAL));

	return TRUE;
}

const char *CRuWorld_ZoneSettings::GetWeatherSystem() const
{
	return m_weatherSystem ? m_weatherSystem : "";
}

void CRuWorld_ZoneSettings::SetWeatherSystem(const char *weatherSystem)
{
	delete[] m_weatherSystem;

	if(weatherSystem)
	{
		m_weatherSystem = ruNEW char [strlen(weatherSystem) + 1];
		strcpy(m_weatherSystem, weatherSystem);
	}
	else
	{
		m_weatherSystem = NULL;
	}
}

void CRuWorld_ZoneSettings::SetSkyGradientTextureName(const char *skyGradientTextureName)
{
	delete[] m_skyGradientTextureName;

	if(skyGradientTextureName)
	{
		m_skyGradientTextureName = ruNEW char [strlen(skyGradientTextureName) + 1];
		strcpy(m_skyGradientTextureName, skyGradientTextureName);
	}
	else
	{
		m_skyGradientTextureName = NULL;
	}
}

void CRuWorld_ZoneSettings::SetCustomSkyBoxName(const char *customSkyBoxName)
{
	delete[] m_customSkyBoxName;

	if(customSkyBoxName)
	{
		m_customSkyBoxName = ruNEW char [strlen(customSkyBoxName) + 1];
		strcpy(m_customSkyBoxName, customSkyBoxName);
	}
	else
	{
		m_customSkyBoxName = NULL;
	}
}

const char *CRuWorld_ZoneSettings::GetAmbientSound() const
{
	if(m_ambientSound)
	{
		return m_ambientSound;
	}

	return "";
}

void CRuWorld_ZoneSettings::SetAmbientSound(const char *ambientSound)
{
	if(ambientSound)
	{
		delete[] m_ambientSound;
		m_ambientSound = ruNEW char [strlen(ambientSound) + 1];
		strcpy(m_ambientSound, ambientSound);
	}
}

const char *CRuWorld_ZoneSettings::GetThemeMusic() const
{
	if(m_themeMusic)
	{
		return m_themeMusic;
	}

	return "";
}

void CRuWorld_ZoneSettings::SetThemeMusic(const char *themeMusic)
{
	if(themeMusic)
	{
		delete[] m_themeMusic;
		m_themeMusic = ruNEW char [strlen(themeMusic) + 1];
		strcpy(m_themeMusic, themeMusic);
	}
}

REAL CRuWorld_ZoneSettings::GetThemeMusicOccurrenceRate()
{
	return m_themeMusicOccurrenceRate;
}

void CRuWorld_ZoneSettings::SetThemeMusicOccurrenceRate(REAL occurrenceRate)
{
	m_themeMusicOccurrenceRate = occurrenceRate;
}

void CRuWorld_ZoneSettings::CopySettings(const CRuWorld_ZoneSettings &zoneSettings)
{
	delete[] m_skyGradientTextureName;
	m_skyGradientTextureName = NULL;

	delete[] m_customSkyBoxName;
	m_customSkyBoxName = NULL;

	m_color_Primary = zoneSettings.m_color_Primary;
	m_color_Secondary = zoneSettings.m_color_Secondary;
	m_color_Ambient = zoneSettings.m_color_Ambient;
	m_color_Specular = zoneSettings.m_color_Specular;
	m_color_Fog = zoneSettings.m_color_Fog;

	m_fog_Start = zoneSettings.m_fog_Start;
	m_fog_End = zoneSettings.m_fog_End;

	m_theta_Standard = zoneSettings.m_theta_Standard;
	m_phi_Standard = zoneSettings.m_phi_Standard;

	m_theta_Specular = zoneSettings.m_theta_Specular;
	m_phi_Specular = zoneSettings.m_phi_Specular;

	if(zoneSettings.m_skyGradientTextureName)
	{
		INT32 len = (INT32)strlen(zoneSettings.m_skyGradientTextureName);
		m_skyGradientTextureName = ruNEW char [len + 1];
		memcpy(m_skyGradientTextureName, zoneSettings.m_skyGradientTextureName, sizeof(char) * (len + 1));
	}

	m_cloudCover = zoneSettings.m_cloudCover;
	m_cloudSharpness = zoneSettings.m_cloudSharpness;
	m_cloudColor0 = zoneSettings.m_cloudColor0;
	m_cloudColor1 = zoneSettings.m_cloudColor1;

	m_baseOctaveSize = zoneSettings.m_baseOctaveSize;
	memcpy(m_octaveSize, zoneSettings.m_octaveSize, sizeof(INT32) * 8);
	memcpy(m_octaveScale, zoneSettings.m_octaveScale, sizeof(REAL) * 8);
	memcpy(m_octaveWeight, zoneSettings.m_octaveWeight, sizeof(REAL) * 8);
	memcpy(m_octaveScrollSpeed, zoneSettings.m_octaveScrollSpeed, sizeof(REAL) * 8);

	if(zoneSettings.m_customSkyBoxName)
	{
		INT32 len = (INT32)strlen(zoneSettings.m_customSkyBoxName);
		m_customSkyBoxName = ruNEW char [len + 1];
		memcpy(m_customSkyBoxName, zoneSettings.m_customSkyBoxName, sizeof(char) * (len + 1));
	}

	m_frameRateMultiplier = zoneSettings.m_frameRateMultiplier;

	if(zoneSettings.m_ambientSound)
	{
		delete[] m_ambientSound;
		m_ambientSound = ruNEW char [strlen(zoneSettings.m_ambientSound) + 1];
		strcpy(m_ambientSound, zoneSettings.m_ambientSound);
	}

	if(zoneSettings.m_themeMusic)
	{
		delete[] m_themeMusic;
		m_themeMusic = ruNEW char [strlen(zoneSettings.m_themeMusic) + 1];
		strcpy(m_themeMusic, zoneSettings.m_themeMusic);
	}

	m_themeMusicOccurrenceRate = zoneSettings.m_themeMusicOccurrenceRate;
}

CRuWorld_ZoneSettings &CRuWorld_ZoneSettings::operator=(const CRuWorld_ZoneSettings &zoneSettings)
{
	m_timeGroup = zoneSettings.m_timeGroup;
	m_weatherGroup = zoneSettings.m_weatherGroup;

	CopySettings(zoneSettings);

	return *this;
}

// ************************************************************************************************************************************************************

CRuWorld_ZoneDescriptor::CRuWorld_ZoneDescriptor()
{
	m_parentID = -1;
	m_id = 0;
	m_name = NULL;
	m_zoneType = RuWorld::ruZONETYPE_OUTDOOR;
}

CRuWorld_ZoneDescriptor::~CRuWorld_ZoneDescriptor()
{
	delete[] m_name;

	for(INT32 i = 0; i < m_zoneSettings.Count(); ++i)
	{
		ruSAFE_RELEASE(m_zoneSettings[i]);
	}
}

BOOL CRuWorld_ZoneDescriptor::SerializeFrom(IRuStream *stream)
{
	// Read class GUID
	CRuGUID classGUID;
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeFrom(stream);

	switch(version)
	{
		case 1:
		case 2:
			{
				// Read zone ID
				stream->Read(&m_id, sizeof(INT32));

				// Read length of zone name including NULL
				INT32 zoneNameLength;
				stream->Read(&zoneNameLength, sizeof(INT32));

				// Read zone name
				if(zoneNameLength > 0)
				{
					m_name = ruNEW char [zoneNameLength];
					stream->Read(m_name, sizeof(char) * zoneNameLength);
				}

				if(version >= 2)
				{
					// Write respawn point
					::SerializeFrom(stream, &m_respawnPoint);
				}

				// Read number of zone settings
				INT32 numZoneSettings = 0;
				stream->Read(&numZoneSettings, sizeof(INT32));

				// Read zone settings
				for(INT32 i = 0; i < numZoneSettings; ++i)
				{
					CRuWorld_ZoneSettings *zoneSettings = ruNEW CRuWorld_ZoneSettings();
					zoneSettings->SerializeFrom(stream);

					m_zoneSettings.Add(zoneSettings);
				}
			}

			break;

		case 3:
		case 4:
			{
				// Read parent zone ID
				stream->Read(&m_parentID, sizeof(INT32));

				// Read zone ID
				stream->Read(&m_id, sizeof(INT32));

				// Read length of zone name including NULL
				INT32 zoneNameLength;
				stream->Read(&zoneNameLength, sizeof(INT32));

				// Read zone name
				if(zoneNameLength > 0)
				{
					m_name = ruNEW char [zoneNameLength];
					stream->Read(m_name, sizeof(char) * zoneNameLength);
				}

				if(version > 3)
				{
					// Write zone type
					stream->Write(&m_zoneType, sizeof(RuWorld::ZoneType));
				}

				// Write respawn point
				::SerializeFrom(stream, &m_respawnPoint);

				// Read number of zone settings
				INT32 numZoneSettings = 0;
				stream->Read(&numZoneSettings, sizeof(INT32));

				// Read zone settings
				for(INT32 i = 0; i < numZoneSettings; ++i)
				{
					CRuWorld_ZoneSettings *zoneSettings = ruNEW CRuWorld_ZoneSettings();
					zoneSettings->SerializeFrom(stream);

					m_zoneSettings.Add(zoneSettings);
				}
			}

			break;
	}


	return TRUE;
}

BOOL CRuWorld_ZoneDescriptor::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 4;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeTo(stream);

	// Write parent zone ID
	stream->Write(&m_parentID, sizeof(INT32));

	// Write zone ID
	stream->Write(&m_id, sizeof(INT32));

	// Write length of zone name including NULL
	INT32 zoneNameLength = m_name ? (INT32)strlen(m_name) + 1 : 0;
	stream->Write(&zoneNameLength, sizeof(INT32));

	// Write zone name
	if(zoneNameLength > 0)
	{
		stream->Write(m_name, sizeof(char) * zoneNameLength);
	}

	// Write zone type
	stream->Write(&m_zoneType, sizeof(RuWorld::ZoneType));

	// Write respawn point
	::SerializeTo(stream, &m_respawnPoint);

	// Write number of zone settings
	INT32 numZoneSettings = m_zoneSettings.Count();
	stream->Write(&numZoneSettings, sizeof(INT32));

	// Write zone settings
	for(INT32 i = 0; i < numZoneSettings; ++i)
	{
		m_zoneSettings[i]->SerializeTo(stream);
	}

	return TRUE;
}

INT32 CRuWorld_ZoneDescriptor::GetParentZoneID()
{
	return m_parentID;
}

void CRuWorld_ZoneDescriptor::SetParentZoneID(INT32 parentZoneID)
{
	m_parentID = parentZoneID;
}

INT32 CRuWorld_ZoneDescriptor::GetZoneID()
{
	return m_id;
}

void CRuWorld_ZoneDescriptor::SetZoneID(INT32 zoneID)
{
	m_id = zoneID;
}

const char *CRuWorld_ZoneDescriptor::GetName() const
{
	if(m_name)
	{
		return m_name;
	}

	return "";
}

void CRuWorld_ZoneDescriptor::SetName(const char *name)
{
	delete[] m_name;

	m_name = ruNEW char [strlen(name) + 1];
	strcpy(m_name, name);
}

RuWorld::ZoneType CRuWorld_ZoneDescriptor::GetZoneType() const
{
	return m_zoneType;
}

void CRuWorld_ZoneDescriptor::SetZoneType(RuWorld::ZoneType zoneType)
{
	m_zoneType = zoneType;
}

const CRuVector3 &CRuWorld_ZoneDescriptor::GetRespawnPoint() const
{
	return m_respawnPoint;
}

void CRuWorld_ZoneDescriptor::SetRespawnPoint(const CRuVector3 &respawnPoint)
{
	m_respawnPoint = respawnPoint;
}

INT32 CRuWorld_ZoneDescriptor::GetNumZoneSettings()
{
	return m_zoneSettings.Count();
}

CRuWorld_ZoneSettings *CRuWorld_ZoneDescriptor::GetZoneSettings(INT32 timeGroup, INT32 weatherGroup, BOOL absoluteMatch)
{
	// Return the preferred zone settings
	for(INT32 i = 0; i < m_zoneSettings.Count(); ++i)
	{
		if(m_zoneSettings[i]->m_timeGroup == timeGroup && m_zoneSettings[i]->m_weatherGroup == weatherGroup)
		{
			return m_zoneSettings[i];
		}
	}

	// If we do not need an absolute match, perform a fuzzy match
	if(!absoluteMatch)
	{
		// Return the preferred zone settings without regard to weather
		for(INT32 i = 0; i < m_zoneSettings.Count(); ++i)
		{
			if(m_zoneSettings[i]->m_timeGroup == timeGroup)
			{
				return m_zoneSettings[i];
			}
		}

		// Return any available
		if(m_zoneSettings.Count() > 0)
		{
			return m_zoneSettings[0];
		}
	}

	return NULL;
}

CRuWorld_ZoneSettings *CRuWorld_ZoneDescriptor::GetZoneSettingsByIndex(INT32 index)
{
	return m_zoneSettings[index];
}

BOOL CRuWorld_ZoneDescriptor::InsertZoneSettings(const CRuWorld_ZoneSettings *zoneSettings)
{
	// Ensure that the zone settings with the specified time and weather groups does not exist
	for(INT32 i = 0; i < m_zoneSettings.Count(); ++i)
	{
		if(m_zoneSettings[i]->m_timeGroup == zoneSettings->m_timeGroup && m_zoneSettings[i]->m_weatherGroup == zoneSettings->m_weatherGroup)
		{
			return FALSE;
		}
	}

	// Make a copy of the zone settings
	CRuWorld_ZoneSettings *newZoneSettings = ruNEW CRuWorld_ZoneSettings();
	newZoneSettings->m_timeGroup = zoneSettings->m_timeGroup;
	newZoneSettings->m_weatherGroup = zoneSettings->m_weatherGroup;
	newZoneSettings->CopySettings(*zoneSettings);

	// Insert new zone settings into zone settings list
	for(INT32 i = 0; i < m_zoneSettings.Count(); ++i)
	{
		if(newZoneSettings->m_timeGroup < m_zoneSettings[i]->m_timeGroup)
		{
			m_zoneSettings.Insert(i, newZoneSettings);

			return TRUE;
		}
		else if(newZoneSettings->m_timeGroup == m_zoneSettings[i]->m_timeGroup && newZoneSettings->m_weatherGroup < m_zoneSettings[i]->m_weatherGroup)
		{
			m_zoneSettings.Insert(i, newZoneSettings);

			return TRUE;
		}
	}

	m_zoneSettings.Add(newZoneSettings);

	return TRUE;
}

BOOL CRuWorld_ZoneDescriptor::DeleteZoneSettings(INT32 timeGroup, INT32 weatherGroup)
{
	// If specified weather group is -1, then delete all matching time groups. Otherwise, delete exact match only.
	if(weatherGroup == -1)
	{
		for(INT32 i = 0; i < m_zoneSettings.Count(); ++i)
		{
			if(m_zoneSettings[i]->m_timeGroup == timeGroup)
			{
				// Release zone settings
				ruSAFE_RELEASE(m_zoneSettings[i]);

				// Remove from list
				m_zoneSettings.RemoveAt(i);
				--i;
			}
		}

		return TRUE;
	}
	else
	{
		for(INT32 i = 0; i < m_zoneSettings.Count(); ++i)
		{
			if(m_zoneSettings[i]->m_timeGroup == timeGroup && m_zoneSettings[i]->m_weatherGroup == weatherGroup)
			{
				// Release zone settings
				ruSAFE_RELEASE(m_zoneSettings[i]);

				// Remove from list
				m_zoneSettings.RemoveAt(i);

				return TRUE;
			}
		}
	}

	return FALSE;
}

void CRuWorld_ZoneDescriptor::CopySettings(const CRuWorld_ZoneDescriptor &zoneDesc)
{
/*
	for(INT32 i = 0; i < m_zoneSettings.Count(); ++i)
	{
	}
*/
}

CRuWorld_ZoneDescriptor &CRuWorld_ZoneDescriptor::operator=(const CRuWorld_ZoneDescriptor &zoneDesc)
{
	// If the source zone descriptor is the same as this object, there is no point in copying
	if(this == &zoneDesc)
	{
		return *this;
	}

	delete[] m_name;
	m_name = NULL;

	m_parentID = zoneDesc.m_parentID;
	m_id = zoneDesc.m_id;
	
	if(zoneDesc.m_name)
	{
		INT32 len = (INT32)strlen(zoneDesc.m_name);
		m_name = ruNEW char [len + 1];
		memcpy(m_name, zoneDesc.m_name, sizeof(char) * (len + 1));
	}

	m_respawnPoint = zoneDesc.m_respawnPoint;

	CopySettings(zoneDesc);

	return *this;
}

BOOL CRuWorld_ZoneDescriptor::SerializeFromOldVersion(IRuStream *stream)
{
	DWORD dummyDWORD;
	INT32 dummyINT32;

	// Create new zone setting
	CRuWorld_ZoneSettings *zoneSettings = ruNEW CRuWorld_ZoneSettings();
	m_zoneSettings.Add(zoneSettings);

	// Read signature and version
	INT32 signature;
	INT32 version = 1;
	stream->Read(&signature, sizeof(INT32));

	if(signature == 0xFFFFFFFF)
	{
		// Signature found, read version 
		stream->Read(&version, sizeof(INT32));
	}
	else
	{
		// No signature, this is an old model stream, so rewind
		stream->Seek(-4, ruSSM_Current);
	}

	switch(version)
	{
		case 1:
			{
				// Read zone ID
				stream->Read(&m_id, sizeof(INT32));

				// Read time group
				stream->Read(&zoneSettings->m_timeGroup, sizeof(INT32));

				// Read weather group
				stream->Read(&zoneSettings->m_weatherGroup, sizeof(INT32));

				// Read length of zone name including NULL
				INT32 zoneNameLength;
				stream->Read(&zoneNameLength, sizeof(INT32));

				// Read zone name
				if(zoneNameLength > 0)
				{
					m_name = ruNEW char [zoneNameLength];
					stream->Read(m_name, sizeof(char) * zoneNameLength);
				}

				// Read primary color
				stream->Read(&zoneSettings->m_color_Primary, sizeof(DWORD));

				// Read secondary color
				stream->Read(&zoneSettings->m_color_Secondary, sizeof(DWORD));

				// Read ambient color
				stream->Read(&zoneSettings->m_color_Ambient, sizeof(DWORD));

				// Read specular color
				stream->Read(&zoneSettings->m_color_Specular, sizeof(DWORD));

				// Read fog color
				stream->Read(&zoneSettings->m_color_Fog, sizeof(DWORD));

				// Read fog start
				stream->Read(&zoneSettings->m_fog_Start, sizeof(float));

				// Read fog end
				stream->Read(&zoneSettings->m_fog_End, sizeof(float));

				// Read theta
				stream->Read(&zoneSettings->m_theta_Standard, sizeof(float));

				// Read phi
				stream->Read(&zoneSettings->m_phi_Standard, sizeof(float));

				// Read theta
				stream->Read(&zoneSettings->m_theta_Specular, sizeof(float));

				// Read phi
				stream->Read(&zoneSettings->m_phi_Specular, sizeof(float));

				stream->Read(&dummyDWORD, sizeof(DWORD));
				stream->Read(&dummyDWORD, sizeof(DWORD));
				stream->Read(&dummyINT32, sizeof(INT32));
				stream->Read(&dummyINT32, sizeof(INT32));

				// Read length of sky gradient texture name including NULL
				INT32 skyGradientTextureNameLength;
				stream->Read(&skyGradientTextureNameLength, sizeof(INT32));

				// Read sky gradient texture name
				if(skyGradientTextureNameLength > 0)
				{
					zoneSettings->m_skyGradientTextureName = ruNEW char [skyGradientTextureNameLength];
					stream->Read(zoneSettings->m_skyGradientTextureName, sizeof(char) * skyGradientTextureNameLength);
				}

				// Read length of horizon cloud texture name including NULL
				INT32 horizonCloudTextureNameLength;
				stream->Read(&horizonCloudTextureNameLength, sizeof(INT32));

				// Read horizon cloud texture name
				if(horizonCloudTextureNameLength > 0)
				{
					stream->Seek(horizonCloudTextureNameLength, ruSSM_Current);
				}

				// Read length of overhead cloud texture name including NULL
				INT32 overheadCloudTextureNameLength;
				stream->Read(&overheadCloudTextureNameLength, sizeof(INT32));

				// Read overhead cloud texture name
				if(overheadCloudTextureNameLength > 0)
				{
					stream->Seek(overheadCloudTextureNameLength, ruSSM_Current);
				}

				// Read cloud mode
				stream->Read(&dummyDWORD, sizeof(DWORD));
			}

			break;

		case 2:
			{
				// Read zone ID
				stream->Read(&m_id, sizeof(INT32));

				// Read time group
				stream->Read(&zoneSettings->m_timeGroup, sizeof(INT32));

				// Read weather group
				stream->Read(&zoneSettings->m_weatherGroup, sizeof(INT32));

				// Read length of zone name including NULL
				INT32 zoneNameLength;
				stream->Read(&zoneNameLength, sizeof(INT32));

				// Read zone name
				if(zoneNameLength > 0)
				{
					m_name = ruNEW char [zoneNameLength];
					stream->Read(m_name, sizeof(char) * zoneNameLength);
				}

				// Read primary color
				stream->Read(&zoneSettings->m_color_Primary, sizeof(DWORD));

				// Read secondary color
				stream->Read(&zoneSettings->m_color_Secondary, sizeof(DWORD));

				// Read ambient color
				stream->Read(&zoneSettings->m_color_Ambient, sizeof(DWORD));

				// Read specular color
				stream->Read(&zoneSettings->m_color_Specular, sizeof(DWORD));

				// Read fog color
				stream->Read(&zoneSettings->m_color_Fog, sizeof(DWORD));

				// Read fog start
				stream->Read(&zoneSettings->m_fog_Start, sizeof(float));

				// Read fog end
				stream->Read(&zoneSettings->m_fog_End, sizeof(float));

				// Read theta
				stream->Read(&zoneSettings->m_theta_Standard, sizeof(float));

				// Read phi
				stream->Read(&zoneSettings->m_phi_Standard, sizeof(float));

				// Read theta
				stream->Read(&zoneSettings->m_theta_Specular, sizeof(float));

				// Read phi
				stream->Read(&zoneSettings->m_phi_Specular, sizeof(float));

				stream->Read(&dummyDWORD, sizeof(DWORD));
				stream->Read(&dummyDWORD, sizeof(DWORD));
				stream->Read(&dummyINT32, sizeof(INT32));
				stream->Read(&dummyINT32, sizeof(INT32));

				// Read length of sky gradient texture name including NULL
				INT32 skyGradientTextureNameLength;
				stream->Read(&skyGradientTextureNameLength, sizeof(INT32));

				// Read sky gradient texture name
				if(skyGradientTextureNameLength > 0)
				{
					zoneSettings->m_skyGradientTextureName = ruNEW char [skyGradientTextureNameLength];
					stream->Read(zoneSettings->m_skyGradientTextureName, sizeof(char) * skyGradientTextureNameLength);
				}

				// Read length of horizon cloud texture name including NULL
				INT32 horizonCloudTextureNameLength;
				stream->Read(&horizonCloudTextureNameLength, sizeof(INT32));

				// Read horizon cloud texture name
				if(horizonCloudTextureNameLength > 0)
				{
					stream->Seek(horizonCloudTextureNameLength, ruSSM_Current);
				}

				// Read length of overhead cloud texture name including NULL
				INT32 overheadCloudTextureNameLength;
				stream->Read(&overheadCloudTextureNameLength, sizeof(INT32));

				// Read overhead cloud texture name
				if(overheadCloudTextureNameLength > 0)
				{
					stream->Seek(overheadCloudTextureNameLength, ruSSM_Current);
				}

				// Read length of custom sky box name including NULL
				INT32 customSkyBoxNameLength;
				stream->Read(&customSkyBoxNameLength, sizeof(INT32));

				// Read custom sky box name
				if(customSkyBoxNameLength > 0)
				{
					zoneSettings->m_customSkyBoxName = ruNEW char [customSkyBoxNameLength];
					stream->Read(zoneSettings->m_customSkyBoxName, sizeof(char) * customSkyBoxNameLength);
				}

				// Write frame rate multiplier
				stream->Read(&zoneSettings->m_frameRateMultiplier, sizeof(REAL));

				// Read cloud mode
				stream->Read(&dummyDWORD, sizeof(DWORD));
			}

			break;

		case 3:
			{
				// Read zone ID
				stream->Read(&m_id, sizeof(INT32));

				// Read time group
				stream->Read(&zoneSettings->m_timeGroup, sizeof(INT32));

				// Read weather group
				stream->Read(&zoneSettings->m_weatherGroup, sizeof(INT32));

				// Read length of zone name including NULL
				INT32 zoneNameLength;
				stream->Read(&zoneNameLength, sizeof(INT32));

				// Read zone name
				if(zoneNameLength > 0)
				{
					m_name = ruNEW char [zoneNameLength];
					stream->Read(m_name, sizeof(char) * zoneNameLength);
				}

				// Read primary color
				stream->Read(&zoneSettings->m_color_Primary, sizeof(DWORD));

				// Read secondary color
				stream->Read(&zoneSettings->m_color_Secondary, sizeof(DWORD));

				// Read ambient color
				stream->Read(&zoneSettings->m_color_Ambient, sizeof(DWORD));

				// Read specular color
				stream->Read(&zoneSettings->m_color_Specular, sizeof(DWORD));

				// Read fog color
				stream->Read(&zoneSettings->m_color_Fog, sizeof(DWORD));

				// Read fog start
				stream->Read(&zoneSettings->m_fog_Start, sizeof(float));

				// Read fog end
				stream->Read(&zoneSettings->m_fog_End, sizeof(float));

				// Read theta
				stream->Read(&zoneSettings->m_theta_Standard, sizeof(float));

				// Read phi
				stream->Read(&zoneSettings->m_phi_Standard, sizeof(float));

				// Read theta
				stream->Read(&zoneSettings->m_theta_Specular, sizeof(float));

				// Read phi
				stream->Read(&zoneSettings->m_phi_Specular, sizeof(float));

				stream->Read(&dummyDWORD, sizeof(DWORD));
				stream->Read(&dummyDWORD, sizeof(DWORD));
				stream->Read(&dummyINT32, sizeof(INT32));
				stream->Read(&dummyINT32, sizeof(INT32));

				// Read length of sky gradient texture name including NULL
				INT32 skyGradientTextureNameLength;
				stream->Read(&skyGradientTextureNameLength, sizeof(INT32));

				// Read sky gradient texture name
				if(skyGradientTextureNameLength > 0)
				{
					zoneSettings->m_skyGradientTextureName = ruNEW char [skyGradientTextureNameLength];
					stream->Read(zoneSettings->m_skyGradientTextureName, sizeof(char) * skyGradientTextureNameLength);
				}

				// Read length of horizon cloud texture name including NULL
				INT32 horizonCloudTextureNameLength;
				stream->Read(&horizonCloudTextureNameLength, sizeof(INT32));

				// Read horizon cloud texture name
				if(horizonCloudTextureNameLength > 0)
				{
					stream->Seek(horizonCloudTextureNameLength, ruSSM_Current);
				}

				// Read length of overhead cloud texture name including NULL
				INT32 overheadCloudTextureNameLength;
				stream->Read(&overheadCloudTextureNameLength, sizeof(INT32));

				// Read overhead cloud texture name
				if(overheadCloudTextureNameLength > 0)
				{
					stream->Seek(overheadCloudTextureNameLength, ruSSM_Current);
				}

				// Read length of custom sky box name including NULL
				INT32 customSkyBoxNameLength;
				stream->Read(&customSkyBoxNameLength, sizeof(INT32));

				// Read custom sky box name
				if(customSkyBoxNameLength > 0)
				{
					zoneSettings->m_customSkyBoxName = ruNEW char [customSkyBoxNameLength];
					stream->Read(zoneSettings->m_customSkyBoxName, sizeof(char) * customSkyBoxNameLength);
				}

				// Write frame rate multiplier
				stream->Read(&zoneSettings->m_frameRateMultiplier, sizeof(REAL));

				// Read cloud mode
				stream->Read(&dummyDWORD, sizeof(DWORD));

				// Read ambient sound
				INT32 ambientSoundLength;
				stream->Read(&ambientSoundLength, sizeof(INT32));
				zoneSettings->m_ambientSound = ruNEW char [ambientSoundLength];
				stream->Read(zoneSettings->m_ambientSound, ambientSoundLength);

				// Read theme music
				INT32 themeMusicLength;
				stream->Read(&themeMusicLength, sizeof(INT32));
				zoneSettings->m_themeMusic = ruNEW char [themeMusicLength];
				stream->Read(zoneSettings->m_themeMusic, themeMusicLength);
			}

			break;
	}
/*
	// Error trapping
	if(m_id < 0 || m_timeGroup < 0 || m_weatherGroup < 0)
	{
		return FALSE;
	}
*/
	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
