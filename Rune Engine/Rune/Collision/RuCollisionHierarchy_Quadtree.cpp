#include "../Collision/RuCollisionHierarchy_Quadtree.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuCollisionHierarchy_Quadtree, IRuCollisionHierarchy)
ruHEAP_DEFINE_SUBCLASS(CRuCollisionHierarchy_Quadtree, "CRuCollisionHierarchy_Quadtree", "IRuCollisionHierarchy")
ruCLASSGUID_DEFINE(CRuCollisionHierarchy_Quadtree, 0x648B4F78, 0x784A4e75, 0x9C9D9056, 0x018BF3AD)

// ************************************************************************************************************************************************************

// ************************************************************************************************************************************************************

CRuCollisionHierarchy_Quadtree::Node::Node()
{
	m_colObjects = NULL;

	for(INT32 i = 0; i < 4; ++i)
	{
		m_children[i] = NULL;
	}
}

CRuCollisionHierarchy_Quadtree::Node::Node(const CRuAABB &bounds, float maxExtents)
{
	m_center = (bounds.Maxima() + bounds.Minima()) / 2.0f;
	m_center.y = 0.0f;
	m_bounds = bounds;

	m_colObjects = NULL;

	// Must we initialize child nodes?
	if(bounds.Maxima().x - bounds.Minima().x > maxExtents || bounds.Maxima().z - bounds.Minima().z > maxExtents)
	{
		CRuAABB childBounds;

		for(INT32 i = 0; i < 4; ++i)
		{
			childBounds.SetBoundsToQuadrantXZ(bounds, i);
			m_children[i] = ruNEW Node(childBounds, maxExtents);
		}
	}
	else
	{
		for(INT32 i = 0; i < 4; ++i)
		{
			m_children[i] = NULL;
		}
	}
}

CRuCollisionHierarchy_Quadtree::Node::~Node()
{
	if(m_colObjects)
	{
		for(INT32 i = 0; i < m_colObjects->Count(); ++i)
		{
			delete m_colObjects->Get(i);
		}

		// Delete collision object descriptor list
		delete m_colObjects;
	}

	// Delete child nodes
	for(INT32 i = 0; i < 4; ++i)
	{
		delete m_children[i];
	}
}

BOOL CRuCollisionHierarchy_Quadtree::Node::AddCollisionObject(IRuCollisionObject *colObject, INT32 objOffset)
{
	// Is this node a leaf?
	if(IsLeaf())
	{
		if(m_colObjects == NULL)
		{
			m_colObjects = ruNEW CRuArrayList<ColObjectDesc *>();
		}

		// Create new object descriptor
		ColObjectDesc *objDesc = ruNEW ColObjectDesc();

		objDesc->m_key = colObject->GetCollisionObjectKey();
		objDesc->m_offset = objOffset;
		objDesc->m_bounds = colObject->GetBounds();

		// Add object descriptor to list
		m_colObjects->Add(objDesc);

		// Update bounds
		m_bounds.Merge(colObject->GetBounds());

		return TRUE;
	}
	else
	{
		CRuVector3 colObjCenter = (colObject->GetBounds().Maxima() + colObject->GetBounds().Minima()) / 2.0f;
		colObjCenter.y = 0.0f;

		// Find nearest child
		INT32 nearestChild = 0;
		float nearestDistance = FLT_MAX;

		for(INT32 i = 0; i < 4; ++i)
		{
			float distance = (colObjCenter - m_children[i]->m_center).Magnitude();

			if(distance < nearestDistance)
			{
				nearestChild = i;
				nearestDistance = distance;
			}
		}

		// Add collision object to nearest child
		m_children[nearestChild]->AddCollisionObject(colObject, objOffset);
	}

	return FALSE;
}

BOOL CRuCollisionHierarchy_Quadtree::Node::RemoveCollisionObject(INT32 objKey)
{
	BOOL childRemoved = FALSE;

	if(m_colObjects)
	{
		// Search for a collision object with the matching key
		for(INT32 i = 0; i < m_colObjects->Count(); ++i)
		{
			if(m_colObjects->Get(i)->m_key == objKey)
			{
				// Remove object
				m_colObjects->RemoveAt(i);
				i--;

				// Set child removed to true
				childRemoved |= TRUE;
			}
		}
	}

	// Recurse to child nodes
	if(IsLeaf() == FALSE)
	{
		for(INT32 i = 0; i < 4; ++i)
		{
			childRemoved |= m_children[i]->RemoveCollisionObject(objKey);
		}
	}

	return childRemoved;
}

BOOL CRuCollisionHierarchy_Quadtree::Node::SerializeFrom(IRuStream *stream)
{
	// Read center
	stream->Read(&m_center.x, sizeof(float));
	stream->Read(&m_center.y, sizeof(float));
	stream->Read(&m_center.z, sizeof(float));

	// Read bounds
	stream->Read(&m_bounds.Minima().x, sizeof(float));
	stream->Read(&m_bounds.Minima().y, sizeof(float));
	stream->Read(&m_bounds.Minima().z, sizeof(float));
	stream->Read(&m_bounds.Maxima().x, sizeof(float));
	stream->Read(&m_bounds.Maxima().y, sizeof(float));
	stream->Read(&m_bounds.Maxima().z, sizeof(float));

	// Read number of collision object descriptors
	INT32 numColObj = 0;
	stream->Read(&numColObj, sizeof(INT32));

	// Read collision object descriptors
	if(numColObj > 0)
	{
		m_colObjects = ruNEW CRuArrayList<ColObjectDesc *>();

		for(INT32 i = 0; i < numColObj; ++i)
		{
			ColObjectDesc *colObjDesc = ruNEW ColObjectDesc();

			stream->Read(&colObjDesc->m_key, sizeof(INT32));
			stream->Read(&colObjDesc->m_offset, sizeof(INT32));
			stream->Read(&colObjDesc->m_bounds.Minima().x, sizeof(float));
			stream->Read(&colObjDesc->m_bounds.Minima().y, sizeof(float));
			stream->Read(&colObjDesc->m_bounds.Minima().z, sizeof(float));
			stream->Read(&colObjDesc->m_bounds.Maxima().x, sizeof(float));
			stream->Read(&colObjDesc->m_bounds.Maxima().y, sizeof(float));
			stream->Read(&colObjDesc->m_bounds.Maxima().z, sizeof(float));

			m_colObjects->Add(colObjDesc);
		}
	}

	// Read children
	INT32 hasChildren = 0;
	stream->Read(&hasChildren, sizeof(INT32));

	if(hasChildren)
	{
		for(INT32 i = 0; i < 4; ++i)
		{
			m_children[i] = ruNEW Node();
			m_children[i]->SerializeFrom(stream);
		}
	}

	return TRUE;
}

BOOL CRuCollisionHierarchy_Quadtree::Node::SerializeTo(IRuStream *stream)
{
	// Write center
	stream->Write(&m_center.x, sizeof(float));
	stream->Write(&m_center.y, sizeof(float));
	stream->Write(&m_center.z, sizeof(float));

	// Write bounds
	stream->Write(&m_bounds.Minima().x, sizeof(float));
	stream->Write(&m_bounds.Minima().y, sizeof(float));
	stream->Write(&m_bounds.Minima().z, sizeof(float));
	stream->Write(&m_bounds.Maxima().x, sizeof(float));
	stream->Write(&m_bounds.Maxima().y, sizeof(float));
	stream->Write(&m_bounds.Maxima().z, sizeof(float));

	// Write number of collision object descriptors
	INT32 numColObj = m_colObjects ? m_colObjects->Count() : 0;
	stream->Write(&numColObj, sizeof(INT32));

	// Write collision object descriptors
	for(INT32 i = 0; i < numColObj; ++i)
	{
		stream->Write(&m_colObjects->Get(i)->m_key, sizeof(INT32));
		stream->Write(&m_colObjects->Get(i)->m_offset, sizeof(INT32));
		stream->Write(&m_colObjects->Get(i)->m_bounds.Minima().x, sizeof(float));
		stream->Write(&m_colObjects->Get(i)->m_bounds.Minima().y, sizeof(float));
		stream->Write(&m_colObjects->Get(i)->m_bounds.Minima().z, sizeof(float));
		stream->Write(&m_colObjects->Get(i)->m_bounds.Maxima().x, sizeof(float));
		stream->Write(&m_colObjects->Get(i)->m_bounds.Maxima().y, sizeof(float));
		stream->Write(&m_colObjects->Get(i)->m_bounds.Maxima().z, sizeof(float));
	}

	// Write children
	INT32 hasChildren = !IsLeaf();
	stream->Write(&hasChildren, sizeof(INT32));

	if(hasChildren)
	{
		for(INT32 i = 0; i < 4; ++i)
		{
			m_children[i]->SerializeTo(stream);
		}
	}

	return TRUE;
}

void CRuCollisionHierarchy_Quadtree::Node::Enumerate(CRuCollisionQuery *colQuery, CRuArrayList<ColObjectDesc *> &objListOut)
{
	BOOL recurseToChildren = FALSE;

	switch(colQuery->m_queryType)
	{
		case ruCOLQUERY_RAY:
			{
				CRuCollisionQuery_Ray *rayQuery = static_cast<CRuCollisionQuery_Ray *>(colQuery);

				if(Intersects(rayQuery->m_colRay, m_bounds))
				{
					if(IsLeaf())
					{
						if(m_colObjects)
						{
							for(INT32 i = 0, numObjs = m_colObjects->Count(); i < numObjs; ++i)
							{
								if(Intersects(rayQuery->m_colRay, m_colObjects->Get(i)->m_bounds))
								{
									objListOut.Add(m_colObjects->Get(i));
								}
							}
						}
					}
					else
					{
						recurseToChildren = TRUE;
					}
				}
			}

			break;

		case ruCOLQUERY_SEGMENT:
			{
				CRuCollisionQuery_Segment *segQuery = static_cast<CRuCollisionQuery_Segment *>(colQuery);

				if(Intersects(segQuery->m_colSegment, m_bounds))
				{
					if(IsLeaf())
					{
						if(m_colObjects)
						{
							for(INT32 i = 0, numObjs = m_colObjects->Count(); i < numObjs; ++i)
							{
								if(Intersects(segQuery->m_colSegment, m_colObjects->Get(i)->m_bounds))
								{
									objListOut.Add(m_colObjects->Get(i));
								}
							}
						}
					}
					else
					{
						recurseToChildren = TRUE;
					}
				}
			}

			break;

		case ruCOLQUERY_SEGMENT_HITTEST:
			{
				CRuCollisionQuery_Segment_HitTest *segQuery = static_cast<CRuCollisionQuery_Segment_HitTest *>(colQuery);

				if(Intersects(segQuery->m_colSegment, m_bounds))
				{
					if(IsLeaf())
					{
						if(m_colObjects)
						{
							for(INT32 i = 0, numObjs = m_colObjects->Count(); i < numObjs; ++i)
							{
								if(Intersects(segQuery->m_colSegment, m_colObjects->Get(i)->m_bounds))
								{
									objListOut.Add(m_colObjects->Get(i));
								}
							}
						}
					}
					else
					{
						recurseToChildren = TRUE;
					}
				}
			}

			break;

		case ruCOLQUERY_SEGMENT_NEAREST:
			{
				CRuCollisionQuery_Segment_Nearest *segQuery = static_cast<CRuCollisionQuery_Segment_Nearest *>(colQuery);

				if(Intersects(segQuery->m_colSegment, m_bounds))
				{
					if(IsLeaf())
					{
						if(m_colObjects)
						{
							for(INT32 i = 0, numObjs = m_colObjects->Count(); i < numObjs; ++i)
							{
								if(Intersects(segQuery->m_colSegment, m_colObjects->Get(i)->m_bounds))
								{
									objListOut.Add(m_colObjects->Get(i));
								}
							}
						}
					}
					else
					{
						recurseToChildren = TRUE;
					}
				}
			}

			break;

		case ruCOLQUERY_RAY_TRIPICK:
			{
			}

			break;

		case ruCOLQUERY_ENUMERATE:
			{
			}

			break;

		case ruCOLQUERY_USER_FLAG_SET_RAY:
			{
				CRuCollisionQuery_User_Flag_Set_Ray *rayQuery = static_cast<CRuCollisionQuery_User_Flag_Set_Ray *>(colQuery);

				if(Intersects(rayQuery->m_markingRay, m_bounds))
				{
					if(IsLeaf())
					{
						if(m_colObjects)
						{
							for(INT32 i = 0, numObjs = m_colObjects->Count(); i < numObjs; ++i)
							{
								if(Intersects(rayQuery->m_markingRay, m_colObjects->Get(i)->m_bounds))
								{
									objListOut.Add(m_colObjects->Get(i));
								}
							}
						}
					}
					else
					{
						recurseToChildren = TRUE;
					}
				}
			}

			break;

		case ruCOLQUERY_USER_FLAG_SET_SPHERE:
			{
				CRuCollisionQuery_User_Flag_Set_Sphere *setQuery = static_cast<CRuCollisionQuery_User_Flag_Set_Sphere *>(colQuery);

				if(Intersects(m_bounds, setQuery->m_markingSphere))
				{
					if(IsLeaf())
					{
						if(m_colObjects)
						{
							for(INT32 i = 0, numObjs = m_colObjects->Count(); i < numObjs; ++i)
							{
								if(Intersects(m_colObjects->Get(i)->m_bounds, setQuery->m_markingSphere))
								{
									objListOut.Add(m_colObjects->Get(i));
								}
							}
						}
					}
					else
					{
						recurseToChildren = TRUE;
					}
				}
			}

			break;

		case ruCOLQUERY_USER_FLAG_GET_SEGMENT:
			{
				CRuCollisionQuery_User_Flag_Get_Segment *segQuery = static_cast<CRuCollisionQuery_User_Flag_Get_Segment *>(colQuery);

				if(Intersects(segQuery->m_colSegment, m_bounds))
				{
					if(IsLeaf())
					{
						if(m_colObjects)
						{
							for(INT32 i = 0, numObjs = m_colObjects->Count(); i < numObjs; ++i)
							{
								if(Intersects(segQuery->m_colSegment, m_colObjects->Get(i)->m_bounds))
								{
									objListOut.Add(m_colObjects->Get(i));
								}
							}
						}
					}
					else
					{
						recurseToChildren = TRUE;
					}
				}
			}

			break;
	}

	// Recurse to children
	if(recurseToChildren)
	{
		for(INT32 i = 0; i < 4; ++i)
		{
			m_children[i]->Enumerate(colQuery, objListOut);
		}
	}
}

CRuCollisionHierarchy_Quadtree::CRuCollisionHierarchy_Quadtree()
:	m_stream(NULL),
	m_root(NULL)
{
	m_tableAddressOffset = 20;
	m_zoneTableAddressOffset = 24;

	// Initialize collision object queue
	m_colObjQueue = ruNEW CRuLRUQueue<INT32, IRuCollisionObject *>(256, 2048);
}

CRuCollisionHierarchy_Quadtree::~CRuCollisionHierarchy_Quadtree()
{
	// Flush collision object queue
	FlushColObjQueue();

	// Destroy collision object queue
	delete m_colObjQueue;

	delete m_stream;
	delete m_root;
}

BOOL CRuCollisionHierarchy_Quadtree::Create(IRuStream *stream, const CRuAABB &bounds, float maxExtents, float gridSpacing, const char *srcResourceName)
{
	const INT32 version = 1;
	INT32 tableOffset = 0;
	INT32 zoneDescriptorTableOffset = 0;

	m_stream = stream;

	m_bounds = bounds;
	m_gridSpacing = gridSpacing;

	INT32 srcResNameLength = (INT32)strlen(srcResourceName) + 1;
	m_srcResourceName = ruNEW char [srcResNameLength];
	memcpy(m_srcResourceName, srcResourceName, srcResNameLength);

	// Initialize empty tree
	m_root = ruNEW Node(bounds, maxExtents);

	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	stream->Write(&version, sizeof(INT32));

	// Write table address
	stream->Write(&tableOffset, sizeof(INT32));

	// Write zone descriptor table address
	stream->Write(&zoneDescriptorTableOffset, sizeof(INT32));

	// Write maximal bounds
	stream->Write(&m_bounds.Minima().x, sizeof(float));
	stream->Write(&m_bounds.Minima().y, sizeof(float));
	stream->Write(&m_bounds.Minima().z, sizeof(float));
	stream->Write(&m_bounds.Maxima().x, sizeof(float));
	stream->Write(&m_bounds.Maxima().y, sizeof(float));
	stream->Write(&m_bounds.Maxima().z, sizeof(float));

	// Write grid spacing
	stream->Write(&m_gridSpacing, sizeof(float));

	// Write resource name length, including NULL
	stream->Write(&srcResNameLength, sizeof(INT32));

	// Write resource name, including NULL
	stream->Write(m_srcResourceName, srcResNameLength);

	// Write table
	tableOffset = stream->GetPosition();
	m_root->SerializeTo(stream);

	return TRUE;
}

BOOL CRuCollisionHierarchy_Quadtree::Open(IRuStream *stream)
{
	// Read four UINT32 values from the stream to determine class GUID
	UINT32 guidValues[4];

	for(INT32 i = 0; i < 4; ++i)
	{
		if(stream->Read(&guidValues[i], sizeof(UINT32)) != sizeof(UINT32))
		{
			return NULL;
		}
	}

	// Construct class GUID
	CRuGUID classGUID(guidValues[0], guidValues[1], guidValues[2], guidValues[3]);

	if(classGUID == ClassGUID())
	{
		INT32 version;
		INT32 tableOffset;
		INT32 zoneDescriptorTableOffset;

		// Read version
		stream->Read(&version, sizeof(INT32));

		// Read table address
		stream->Read(&tableOffset, sizeof(INT32));

		// Read zone descriptor table address
		stream->Read(&zoneDescriptorTableOffset, sizeof(INT32));

		// Read maximal bounds
		stream->Read(&m_bounds.Minima().x, sizeof(float));
		stream->Read(&m_bounds.Minima().y, sizeof(float));
		stream->Read(&m_bounds.Minima().z, sizeof(float));
		stream->Read(&m_bounds.Maxima().x, sizeof(float));
		stream->Read(&m_bounds.Maxima().y, sizeof(float));
		stream->Read(&m_bounds.Maxima().z, sizeof(float));

		// Read grid spacing
		stream->Read(&m_gridSpacing, sizeof(float));

		// Read resource name length, including NULL
		INT32 srcResNameLength = 0;
		stream->Read(&srcResNameLength, sizeof(INT32));

		// Read resource name, including NULL
		if(srcResNameLength > 0)
		{
			m_srcResourceName = ruNEW char [srcResNameLength];
			stream->Read(m_srcResourceName, srcResNameLength);
		}
		else
		{
			m_srcResourceName = ruNEW char [1];
			m_srcResourceName[0] = NULL;
		}

		// Read table
		stream->Seek(tableOffset, ruSSM_Begin);
		m_root = ruNEW Node();
		m_root->SerializeFrom(stream);

		m_stream = stream;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuCollisionHierarchy_Quadtree::Save()
{
	if(m_stream && m_root)
	{
		// Seek to end of file
		m_stream->Seek(0, ruSSM_End);

		// Write table
		INT32 tableOffset = m_stream->GetPosition();
		m_root->SerializeTo(m_stream);

		return TRUE;
	}

	return FALSE;
}

const CRuAABB &CRuCollisionHierarchy_Quadtree::GetBounds() const
{
	return m_bounds;
}

float CRuCollisionHierarchy_Quadtree::GetGridSpacing() const
{
	return m_gridSpacing;
}

const char *CRuCollisionHierarchy_Quadtree::GetSourceResourceName() const
{
	return m_srcResourceName;
}

BOOL CRuCollisionHierarchy_Quadtree::AddCollisionObject(IRuCollisionObject *colObject)
{
	DWORD colObjectOffset = 0;

	// Write collision object to end of the stream
	// TODO: Compression!
	m_stream->Seek(0, ruSSM_End);
	colObjectOffset = m_stream->GetPosition();
	colObject->SerializeTo(m_stream);

	// Add collision object data to the tree
	m_root->AddCollisionObject(colObject, colObjectOffset);

	return TRUE;
}

BOOL CRuCollisionHierarchy_Quadtree::RemoveCollisionObject(INT32 objKey)
{
	return m_root->RemoveCollisionObject(objKey);
}

BOOL CRuCollisionHierarchy_Quadtree::QueryCollision(CRuCollisionQuery *colQuery)
{
	if(m_root)
	{
		m_objList.Clear();

		// Enumerate all collision objects involved in this query
		m_root->Enumerate(colQuery, m_objList);

		// Perform collision query on all of the enumerated collision objects
		for(INT32 i = 0; i < m_objList.Count(); ++i)
		{
			// Instance the collision object
			IRuCollisionObject *colObject = InstanceCollisionObject(m_objList[i]);

			// Perform collision query
			if(colObject)
			{
				colObject->QueryCollision(colQuery);
			}
		}

		return TRUE;
	}

	return FALSE;
}

IRuCollisionObject *CRuCollisionHierarchy_Quadtree::InstanceCollisionObject(ColObjectDesc *objDesc)
{
	DWORD keyHash = RuGlobalHashGen().GetHashCode(static_cast<UINT32>(objDesc->m_key));
	IRuCollisionObject *colObject = NULL;

	// Locate the collision key
	if(m_colObjQueue->Get(objDesc->m_key, keyHash, colObject) == FALSE)
	{
		// Reclaim space
		IRuCollisionObject *reclaimedColObject;
		while(m_colObjQueue->Reclaim(objDesc->m_key, keyHash, 1, reclaimedColObject))
		{
			// Release
			if(reclaimedColObject)
			{
				reclaimedColObject->Release();
			}
		}

		// Instance the collision object
		m_stream->Seek(objDesc->m_offset, ruSSM_Begin);
		IRuObject *obj = RuObjectFactory().CreateObject(m_stream);
		if(obj && obj->GetType().IsSubClassOf(IRuCollisionObject::Type()))
		{
			colObject = static_cast<IRuCollisionObject *>(obj);
		}

		// Add the collision object to the queue
		m_colObjQueue->Add(objDesc->m_key, keyHash, colObject, 1);
	}

	return colObject;
}

BOOL CRuCollisionHierarchy_Quadtree::FlushColObjQueue()
{
	if(m_colObjQueue)
	{
		// Iterate through all instanced entities
		m_colObjQueue->ForAllElements(FlushColObjQueueCallback, this);

		// Clear instanced entity queue
		m_colObjQueue->Clear();
	}

	return TRUE;
}

BOOL CRuCollisionHierarchy_Quadtree::FlushColObjQueueCallback(const void *key, const void *data, void *userData)
{
	IRuCollisionObject *colObj = *((IRuCollisionObject **) data);

	if(colObj)
	{
		colObj->Release();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
