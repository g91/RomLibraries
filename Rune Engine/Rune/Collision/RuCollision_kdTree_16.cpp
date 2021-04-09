#include "../Collision/RuCollision_kdTree_16.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuCollision_kdTree_16, IRuCollisionObject)
ruHEAP_DEFINE_SUBCLASS(CRuCollision_kdTree_16, "CRuCollision_kdTree_16", "IRuCollisionObject")
ruCLASSGUID_DEFINE(CRuCollision_kdTree_16, 0x44F3BCFB, 0xCB734de5, 0x9C5BF635, 0x654AA07B)

// ************************************************************************************************************************************************************

CRuCollision_kdTree_16::CRuCollision_kdTree_16()
:	m_numVertices(0),
	m_numTriangles(0),
	m_triVertices(NULL),
	m_triFlags(NULL),
	m_triIndices(NULL),
	m_numNodes(0),
	m_numLeaves(0),
	m_nodeArray(NULL),
	m_leafArray(NULL)
{
}

CRuCollision_kdTree_16::~CRuCollision_kdTree_16()
{
	// Clear shit in arrays
	for(INT32 i = 0; i < m_numLeaves; ++i)
	{
		delete[] m_leafArray[i].m_triFaces;
	}

	// Delete arrays
	delete[] m_triVertices;
	delete[] m_triFlags;
	delete[] m_triIndices;
	delete[] m_nodeArray;
	delete[] m_leafArray;
}

void CRuCollision_kdTree_16::BuildTree(const CRuVector3 *triVertices, const UINT16 *triIndices, const UINT32 *triFlags, INT32 numVertices, INT32 numTriangles, INT32 maxTriPerLeaf, INT32 maxDepth)
{
	// Determine maximum number of nodes
	INT32 maxNodes = 0;
	for(INT32 i = 0; i < maxDepth; ++i)
	{
		maxNodes += (INT32) pow(2.0f, i);
	}

	CRuArrayList<kdNode> nodes(maxNodes);
	CRuArrayList<kdLeafData> leaves;

	kdNode node;
	for(INT32 i = 0; i < maxNodes; ++i)
	{
		nodes.Add(node);
	}

	// Allocate array to store triangle vertices
	m_numVertices = numVertices;
	m_numTriangles = numTriangles;
	m_triVertices = ruNEW CRuVector3 [m_numVertices];
	m_triFlags = ruNEW UINT32 [m_numTriangles];
	m_triIndices = ruNEW UINT16 [m_numTriangles * 3];

	// Compute bounds while copying the entire vertex array
	m_bounds.Set(triVertices[0].x, triVertices[0].y, triVertices[0].z, triVertices[0].x, triVertices[0].y, triVertices[0].z);
	for(INT32 i = 0; i < m_numVertices; ++i)
	{
		m_triVertices[i] = triVertices[i];
		m_bounds.Merge(m_triVertices[i]);
	}

	// Copy the flags array
	if(triFlags)
	{
		memcpy(m_triFlags, triFlags, sizeof(UINT32) * m_numTriangles);
	}
	else
	{
		memset(m_triFlags, 0, sizeof(UINT32) * m_numTriangles);
	}

	// Copy the index array
	memcpy(m_triIndices, triIndices, sizeof(UINT16) * m_numTriangles * 3);

	// Build initial face list
	CRuArrayList<UINT16> triFaces(m_numTriangles);
	for(INT32 i = 0; i < m_numTriangles; ++i)
	{
		triFaces.Add(i);
	}

	// Drop degenerate triangles

	// Build tree
	INT32 nodesUsed = BuildTree(0, triFaces, 1, m_bounds, maxTriPerLeaf, maxDepth, nodes, leaves);

	// Copy crap
	m_numNodes = nodesUsed;
	m_numLeaves = leaves.Count();
	m_nodeArray = ruNEW kdNode [m_numNodes];
	m_leafArray = ruNEW kdLeafData [m_numLeaves];

	memcpy(m_nodeArray, nodes.GetArray(), sizeof(kdNode) * m_numNodes);
	memcpy(m_leafArray, leaves.GetArray(), sizeof(kdLeafData) * m_numLeaves);
}

BOOL CRuCollision_kdTree_16::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;
	INT32 version = 1;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	stream->Read(&version, sizeof(INT32));

	// Read bounds
	stream->Read(&m_bounds.Minima().x, sizeof(float));
	stream->Read(&m_bounds.Minima().y, sizeof(float));
	stream->Read(&m_bounds.Minima().z, sizeof(float));
	stream->Read(&m_bounds.Maxima().x, sizeof(float));
	stream->Read(&m_bounds.Maxima().y, sizeof(float));
	stream->Read(&m_bounds.Maxima().z, sizeof(float));

	// Read number of vertices
	stream->Read(&m_numVertices, sizeof(INT32));

	// Read number of triangles
	stream->Read(&m_numTriangles, sizeof(INT32));

	// Read number of nodes
	stream->Read(&m_numNodes, sizeof(INT32));

	// Read number of leaves
	stream->Read(&m_numLeaves, sizeof(INT32));

	// Read vertex array
	m_triVertices = ruNEW CRuVector3 [m_numVertices];
	for(INT32 i = 0; i < m_numVertices; ++i)
	{
		stream->Read(&m_triVertices[i].x, sizeof(float));
		stream->Read(&m_triVertices[i].y, sizeof(float));
		stream->Read(&m_triVertices[i].z, sizeof(float));
	}

	// Read flags array
	m_triFlags = ruNEW UINT32 [m_numTriangles];
	for(INT32 i = 0; i < m_numTriangles; ++i)
	{
		stream->Read(&m_triFlags[i], sizeof(UINT32));
	}

	// Read index array
	m_triIndices = ruNEW UINT16 [m_numTriangles * 3];
	stream->Read(m_triIndices, sizeof(UINT16) * m_numTriangles * 3);

	// Allocate nodes and read from stream
	m_nodeArray = ruNEW kdNode [m_numNodes];
	stream->Read(m_nodeArray, sizeof(kdNode) * m_numNodes);

	// Allocate leaves and read from stream
	m_leafArray = ruNEW kdLeafData [m_numLeaves];
	for(INT32 i = 0; i < m_numLeaves; ++i)
	{
		// Read number of triangles
		stream->Read(&m_leafArray[i].m_numTriangles, sizeof(INT32));

		// Read triangles
		m_leafArray[i].m_triFaces = ruNEW UINT16 [m_leafArray[i].m_numTriangles];
		for(INT32 j = 0; j < m_leafArray[i].m_numTriangles; ++j)
		{
			stream->Read(&m_leafArray[i].m_triFaces[j], sizeof(UINT16));
		}
	}


	return TRUE;
}

BOOL CRuCollision_kdTree_16::SerializeTo(IRuStream *stream)
{
	const INT32 version = 1;

	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	stream->Write(&version, sizeof(INT32));

	// Write bounds
	stream->Write(&m_bounds.Minima().x, sizeof(float));
	stream->Write(&m_bounds.Minima().y, sizeof(float));
	stream->Write(&m_bounds.Minima().z, sizeof(float));
	stream->Write(&m_bounds.Maxima().x, sizeof(float));
	stream->Write(&m_bounds.Maxima().y, sizeof(float));
	stream->Write(&m_bounds.Maxima().z, sizeof(float));

	// Write number of vertices
	stream->Write(&m_numVertices, sizeof(INT32));

	// Write number of triangles
	stream->Write(&m_numTriangles, sizeof(INT32));

	// Write number of nodes
	stream->Write(&m_numNodes, sizeof(INT32));

	// Write number of leaves
	stream->Write(&m_numLeaves, sizeof(INT32));

	// Write vertex array
	for(INT32 i = 0; i < m_numVertices; ++i)
	{
		stream->Write(&m_triVertices[i].x, sizeof(float));
		stream->Write(&m_triVertices[i].y, sizeof(float));
		stream->Write(&m_triVertices[i].z, sizeof(float));
	}

	// Write flags array
	stream->Write(m_triFlags, sizeof(UINT32) * m_numTriangles);

	// Write index array
	stream->Write(m_triIndices, sizeof(UINT16) * m_numTriangles * 3);

	// Write nodes
	stream->Write(m_nodeArray, sizeof(kdNode) * m_numNodes);

	// Write leaves
	for(INT32 i = 0; i < m_numLeaves; ++i)
	{
		// Write number of triangles
		stream->Write(&m_leafArray[i].m_numTriangles, sizeof(INT32));

		// Write triangle face indices
		stream->Write(m_leafArray[i].m_triFaces, sizeof(UINT16) * m_leafArray[i].m_numTriangles);
	}

	return TRUE;
}

INT32 CRuCollision_kdTree_16::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuCollisionObject::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	footprint += sizeof(*this);

	footprint += (m_triVertices ? sizeof(CRuVector3) * m_numVertices : 0);
	footprint += (m_triIndices ? sizeof(UINT16) * m_numTriangles * 3 : 0);
	footprint += (m_triFlags ? sizeof(UINT32) * m_numTriangles : 0);

	footprint += (m_nodeArray ? sizeof(kdNode) * m_numNodes : 0);
	footprint += (m_leafArray ? sizeof(kdLeafData) * m_numLeaves : 0);

	for(INT32 i = 0; i < m_numLeaves; ++i)
	{
		footprint += sizeof(m_leafArray[i].m_numTriangles);
		footprint += sizeof(m_leafArray[i].m_triFaces) + (m_leafArray[i].m_triFaces ? sizeof(UINT16) * m_leafArray[i].m_numTriangles : 0);
	}

	return footprint;
}

const CRuAABB &CRuCollision_kdTree_16::GetBounds() const
{
	return m_bounds;
}

BOOL CRuCollision_kdTree_16::QueryCollision(CRuCollisionQuery *colQuery)
{
	BOOL retVal = TRUE;

	switch(colQuery->m_queryType)
	{
		case ruCOLQUERY_RAY:
			{
				CRuCollisionQuery_Ray_Base *rayQuery = static_cast<CRuCollisionQuery_Ray_Base *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 oldRay = rayQuery->m_colRay;
				rayQuery->m_colRay = CRuRay3(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));

				// Make collision query
				if(Intersects(rayQuery->m_colRay, m_bounds))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					retVal = QueryCollision(static_cast<CRuCollisionQuery_Ray *>(colQuery), 0, 0.0f, FLT_MAX);
				}

				// Restore old collision ray
				rayQuery->m_colRay = oldRay;
			}

			break;

		case ruCOLQUERY_RAY_NEAREST:
			{
				CRuCollisionQuery_Ray_Base *rayQuery = static_cast<CRuCollisionQuery_Ray_Base *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 oldRay = rayQuery->m_colRay;
				rayQuery->m_colRay = CRuRay3(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));

				// Make collision query
				if(Intersects(rayQuery->m_colRay, m_bounds))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					retVal = QueryCollision(static_cast<CRuCollisionQuery_Ray_Nearest *>(colQuery), 0, 0.0f, FLT_MAX);
				}

				// Restore old collision ray
				rayQuery->m_colRay = oldRay;
			}

			break;

		case ruCOLQUERY_RAY_PICK:
			{
				CRuCollisionQuery_Ray_Pick *rayQuery = static_cast<CRuCollisionQuery_Ray_Pick *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 oldRay = rayQuery->m_colRay;
				rayQuery->m_colRay = CRuRay3(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));

				// Make collision query
				if(Intersects(rayQuery->m_colRay, m_bounds))
				{
					rayQuery->m_positiveHit = TRUE;

					// temp: return arbitrary distance
					CRuVector3 dummyPt;
					FindIntersection(rayQuery->m_colRay, m_bounds, dummyPt, rayQuery->m_colDistance);

					retVal = TRUE;
				}

				// Restore old collision ray
				rayQuery->m_colRay = oldRay;
			}

			break;

		case ruCOLQUERY_RAY_TRIPICK:
			{
			}

			break;

		case ruCOLQUERY_SEGMENT:
			{
				CRuCollisionQuery_Segment_Base *segQuery = static_cast<CRuCollisionQuery_Segment_Base *>(colQuery);

				// Get the segment in local coordinates
				CRuSegment3 oldSegment(segQuery->m_colSegment);
				segQuery->m_invWorldTransform->TransformPoint(segQuery->m_colSegment.Origin());
				segQuery->m_invWorldTransform->TransformVector(segQuery->m_colSegment.Direction());

				// Make collision query
				if(Intersects(segQuery->m_colSegment, m_bounds))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					retVal = QueryCollision(static_cast<CRuCollisionQuery_Segment *>(colQuery), 0, 0.0f, 1.0f);
				}

				// Restore old collision segment
				segQuery->m_colSegment = oldSegment;
			}

			break;

		case ruCOLQUERY_SEGMENT_HITTEST:
			{
				CRuCollisionQuery_Segment_Base *segQuery = static_cast<CRuCollisionQuery_Segment_Base *>(colQuery);

				// Get the segment in local coordinates
				CRuSegment3 oldSegment(segQuery->m_colSegment);
				segQuery->m_invWorldTransform->TransformPoint(segQuery->m_colSegment.Origin());
				segQuery->m_invWorldTransform->TransformVector(segQuery->m_colSegment.Direction());

				// Make collision query
				if(Intersects(segQuery->m_colSegment, m_bounds))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					retVal = QueryCollision(static_cast<CRuCollisionQuery_Segment_HitTest *>(colQuery), 0, 0.0f, 1.0f);
				}

				// Restore old collision segment
				segQuery->m_colSegment = oldSegment;
			}

			break;

		case ruCOLQUERY_SEGMENT_NEAREST:
			{
				CRuCollisionQuery_Segment_Base *segQuery = static_cast<CRuCollisionQuery_Segment_Base *>(colQuery);

				// Get the segment in local coordinates
				CRuSegment3 oldSegment(segQuery->m_colSegment);
				segQuery->m_invWorldTransform->TransformPoint(segQuery->m_colSegment.Origin());
				segQuery->m_invWorldTransform->TransformVector(segQuery->m_colSegment.Direction());

				// Make collision query
				if(Intersects(segQuery->m_colSegment, m_bounds))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					retVal = QueryCollision(static_cast<CRuCollisionQuery_Segment_Nearest *>(colQuery), 0, 0.0f, 1.0f);
				}

				// Restore old collision segment
				segQuery->m_colSegment = oldSegment;
			}

			break;

		case ruCOLQUERY_OBB_HITTEST:
			{
				CRuCollisionQuery_OBB_HitTest *boxQuery = static_cast<CRuCollisionQuery_OBB_HitTest *>(colQuery);

				// Get the collision OBB in local coordinates
				CRuOBB oldBox(boxQuery->m_colOBB);
				boxQuery->m_colOBB.Transform(*boxQuery->m_invWorldTransform);

				if(Intersects(m_bounds, boxQuery->m_colOBB))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					retVal = QueryCollision(boxQuery, 0, m_bounds);
				}

				// Restore old box
				boxQuery->m_colOBB = oldBox;
			}

			break;

		case ruCOLQUERY_OBB_TRIENUM:
			{
				CRuCollisionQuery_OBB_TriEnum *boxQuery = static_cast<CRuCollisionQuery_OBB_TriEnum *>(colQuery);

				// Get the collision OBB in local coordinates
				CRuOBB oldBox(boxQuery->m_colOBB);
				boxQuery->m_colOBB.Transform(*boxQuery->m_invWorldTransform);

				if(Intersects(m_bounds, boxQuery->m_colOBB))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					retVal = QueryCollision(boxQuery, 0, m_bounds);
				}

				// Restore old box
				boxQuery->m_colOBB = oldBox;
			}

			break;

		case ruCOLQUERY_SPHERE_HITTEST:
			{
				CRuCollisionQuery_Sphere_HitTest *sphereQuery = static_cast<CRuCollisionQuery_Sphere_HitTest *>(colQuery);

				// Get the sphere in local coordinates
				CRuSphere oldSphere(sphereQuery->m_colSphere);
				sphereQuery->m_invWorldTransform->TransformPoint(sphereQuery->m_colSphere.Center());

				CRuVector3 extents(1.0f, 1.0f, 1.0f);
				sphereQuery->m_invWorldTransform->TransformVector(extents);
				sphereQuery->m_colSphere.Radius() = max(max(fabs(extents.x), fabs(extents.y)), fabs(extents.z)) * sphereQuery->m_colSphere.Radius();

				// Make collision query
				if(Intersects(m_bounds, sphereQuery->m_colSphere))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					CRuVector3 volNearPt = sphereQuery->m_colSphere.Center();
					retVal = QueryCollision(sphereQuery, 0, volNearPt);
				}

				// Restore old sphere
				sphereQuery->m_colSphere = oldSphere;
			}

			break;

		case ruCOLQUERY_SPHERE_TRIENUM:
			{
				CRuCollisionQuery_Sphere_TriEnum *sphereQuery = static_cast<CRuCollisionQuery_Sphere_TriEnum *>(colQuery);

				// Get the sphere in local coordinates
				CRuSphere oldSphere(sphereQuery->m_colSphere);
				sphereQuery->m_invWorldTransform->TransformPoint(sphereQuery->m_colSphere.Center());

				CRuVector3 extents(1.0f, 1.0f, 1.0f);
				sphereQuery->m_invWorldTransform->TransformVector(extents);
				sphereQuery->m_colSphere.Radius() = max(max(fabs(extents.x), fabs(extents.y)), fabs(extents.z)) * sphereQuery->m_colSphere.Radius();

				// Make collision query
				if(Intersects(m_bounds, sphereQuery->m_colSphere))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					CRuVector3 volNearPt = sphereQuery->m_colSphere.Center();
					retVal = QueryCollision(static_cast<CRuCollisionQuery_Sphere_TriEnum *>(colQuery), 0, volNearPt);
				}

				// Restore old sphere
				sphereQuery->m_colSphere = oldSphere;
			}

			break;

		case ruCOLQUERY_ENUMERATE:
			// TODO: Not verified
			return QueryCollision(static_cast<CRuCollisionQuery_Enumerate *>(colQuery), 0);

		case ruCOLQUERY_USER_FLAG_SET_RAY:
			{
				CRuCollisionQuery_User_Flag_Set_Ray *rayQuery = static_cast<CRuCollisionQuery_User_Flag_Set_Ray *>(colQuery);

				// Get the ray in local coordinates
				CRuRay3 oldRay = rayQuery->m_markingRay;
				rayQuery->m_markingRay = CRuRay3(rayQuery->m_markingRay, *(rayQuery->m_invWorldTransform));

				// Make collision query
				if(Intersects(rayQuery->m_markingRay, m_bounds))
				{
					retVal = QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Set_Ray *>(colQuery), 0, 0.0f, FLT_MAX);
				}

				// Restore old collision ray
				rayQuery->m_markingRay = oldRay;
			}

			break;

		case ruCOLQUERY_USER_FLAG_SET_SPHERE:
			{
				CRuCollisionQuery_User_Flag_Set_Sphere *sphereQuery = static_cast<CRuCollisionQuery_User_Flag_Set_Sphere *>(colQuery);

				// Get the sphere in local coordinates
				CRuSphere oldSphere(sphereQuery->m_markingSphere);
				sphereQuery->m_invWorldTransform->TransformPoint(sphereQuery->m_markingSphere.Center());

				CRuVector3 extents(1.0f, 1.0f, 1.0f);
				sphereQuery->m_invWorldTransform->TransformVector(extents);
				sphereQuery->m_markingSphere.Radius() = max(max(fabs(extents.x), fabs(extents.y)), fabs(extents.z)) * sphereQuery->m_markingSphere.Radius();

				// Make collision query
				if(Intersects(m_bounds, sphereQuery->m_markingSphere))
				{
					// Use bitfield to ensure we do not test duplicate triangles
					m_bitField.SetNumBits(m_numTriangles);
					if(colQuery->m_resetColStruct)
					{
						m_bitField.ClearAll();
					}

					CRuVector3 volNearPt = sphereQuery->m_markingSphere.Center();
					retVal = QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Set_Sphere *>(colQuery), 0, volNearPt);
				}

				// Restore old sphere
				sphereQuery->m_markingSphere = oldSphere;
			}

			break;

		case ruCOLQUERY_USER_FLAG_GET_SEGMENT:
			{
				CRuCollisionQuery_User_Flag_Get_Segment *segQuery = static_cast<CRuCollisionQuery_User_Flag_Get_Segment *>(colQuery);

				// Get the segment in local coordinates
				CRuSegment3 oldSegment(segQuery->m_colSegment);
				segQuery->m_invWorldTransform->TransformPoint(segQuery->m_colSegment.Origin());
				segQuery->m_invWorldTransform->TransformVector(segQuery->m_colSegment.Direction());

				// Make collision query
				if(Intersects(segQuery->m_colSegment, m_bounds))
				{
					retVal = QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Get_Segment *>(colQuery), 0, 0.0f, 1.0f);
				}

				// Restore old collision segment
				segQuery->m_colSegment = oldSegment;
			}

			break;

		case ruCOLQUERY_USER_FLAG_ENUMERATE:
			// TODO: Not verified
			return QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Enumerate *>(colQuery), 0);
	}

	return retVal;
}

INT32 CRuCollision_kdTree_16::GetNumColVertices() const
{
	return m_numVertices;
}

INT32 CRuCollision_kdTree_16::GetNumColPrimitives() const
{
	return m_numTriangles;
}

CRuVector3 *CRuCollision_kdTree_16::GetColVertices()
{
	return m_triVertices;
}

UINT32 *CRuCollision_kdTree_16::GetColFlags()
{
	return m_triFlags;
}

UINT16 *CRuCollision_kdTree_16::GetColIndices()
{
	return m_triIndices;
}


// Compute the "value" of splitting the given set of faces, bounded by
// the given box, along the plane [axis]=offset.  A value of 0 means
// that a split is possible, but has no value.  A negative value means
// that the split is not valid at all.  Positive values indicate
// increasing goodness.
//
// *posOffset is computed based on the minimum coord of the faces
// that don't fit behind the negOffset.  Could be greater or less
// than negOffset.
//
// This is kinda heuristicy -- it's where the "special sauce" comes
// in.
REAL CRuCollision_kdTree_16::EvaluateSplit(const CRuArrayList<UINT16> &triFaces, INT32 depth, const CRuAABB &bounds, INT32 axis, REAL splitValue)
{
	// Count the faces that will end up in the groups
	// back,front.
	INT32 backCount = 0;
	INT32 frontCount = 0;

	INT32 numTriangles = triFaces.Count();

	for(INT32 i = 0; i < numTriangles; ++i)
	{
		INT32 result = ClassifyFace(triFaces, i, axis, splitValue);
		if(result == -1)
		{
			// Neg.
			backCount++;
		}
		else if(result == 0)
		{
			// Neg.
			backCount++;

			// Pos.
			frontCount++;
		}
		else
		{
			// Pos.
			frontCount++;
		}
	}

	// No faces are separated by this split; this split is entirely useless (not so! check to see if we are cutting off empty space!)
	if((backCount == 0 && splitValue <= bounds.Minima()[axis]) || (frontCount == 0 && splitValue >= bounds.Maxima()[axis]))
	{
		return -1;
	}

	REAL center = ((bounds.Minima() + bounds.Maxima()) / 2.0f)[axis];
	REAL extent = bounds.Maxima()[axis] - bounds.Minima()[axis];

	CRuAABB backBounds(bounds);
	backBounds.Maxima()[axis] = splitValue;

	CRuAABB	frontBounds(bounds);
	frontBounds.Minima()[axis] = splitValue;



	// MacDonald and Booth's metric, as quoted by Havran, endorsed by Ville Miettinen and Atman Binstock:

	REAL costBack = backBounds.GetSurfaceArea() * backCount;
	REAL costFront = frontBounds.GetSurfaceArea() * frontCount;

	REAL havranCost = costBack + costFront;

	REAL parentCost = bounds.GetSurfaceArea() * numTriangles;

	// We need to turn the cost into a quality, so subtract it from a
	// big number.
	return parentCost - havranCost;
}

INT32 CRuCollision_kdTree_16::BuildTree(INT32 nodeIndex, const CRuArrayList<UINT16> &triFaces, INT32 depth, const CRuAABB &bounds, INT32 maxTriPerLeaf, INT32 maxDepth, CRuArrayList<kdNode> &nodeListOut, CRuArrayList<kdLeafData> &leafDataListOut)
{
	INT32 treeSize = nodeIndex + 1;
	const INT32 maxSplitPlanesTested = 100;

	// If stop conditions are not met, subdivide node
	INT32 numTriangles = triFaces.Count();
	if(numTriangles > maxTriPerLeaf && depth < maxDepth)
	{
		// Find a good splitting plane
		REAL bestSplitQuality = 0.0f;
		INT32 bestSplitAxis = -1;
		REAL bestSplitVal = 0.0f;

		for(INT32 axis = 0; axis < 3; ++axis)
		{
			// Don't try to divide if the extent in current direction is degenerate
			if(bounds.Maxima()[axis] - bounds.Minima()[axis] < ruEPSILON)
			{
				continue;
			}

			// Try offsets that correspond to existing face boundaries
			INT32 stepSize = 1;
			if(numTriangles > maxSplitPlanesTested)
			{
				// For the sake of speed & sanity, only try the bounds of every N faces
				stepSize = numTriangles / maxSplitPlanesTested;
			}
			assert(stepSize > 0);

			REAL lastSplitValueTried = -FLT_MAX;
			REAL posOffset = 0;
			for(INT32 i = 0; i < numTriangles; i += stepSize)
			{
				REAL splitValue = max(max(m_triVertices[m_triIndices[triFaces[i] * 3]][axis], m_triVertices[m_triIndices[triFaces[i] * 3 + 1]][axis]), m_triVertices[m_triIndices[triFaces[i] * 3 + 2]][axis]);

				// Already tried this offset, skip
				if(fabsf(splitValue - lastSplitValueTried) < ruEPSILON)
				{
					continue;
				}

				lastSplitValueTried = splitValue;

				// How good is this split?
				REAL quality = EvaluateSplit(triFaces, depth, bounds, axis, splitValue);
				if(quality > bestSplitQuality)
				{
					// Best so far
					bestSplitQuality = quality;
					bestSplitAxis = axis;
					bestSplitVal = RuCastINT64ToFloat((RuCastFloatToINT64(splitValue) & 0xFFFFFFFC) | axis);
				}
			}
		}

		if(bestSplitAxis >= 0)
		{
			// Distribute triangles to each child
			INT32 childNodeIndex0 = nodeIndex * 2 + 1;
			INT32 childNodeIndex1 = nodeIndex * 2 + 2;
			CRuArrayList<UINT16> childTriFaces0;			// Negative half-space
			CRuArrayList<UINT16> childTriFaces1;			// Positive half-space

			for(INT32 i = 0; i < triFaces.Count(); ++i)
			{
				REAL d0 = m_triVertices[m_triIndices[triFaces[i] * 3]][bestSplitAxis] - bestSplitVal;
				REAL d1 = m_triVertices[m_triIndices[triFaces[i] * 3 + 1]][bestSplitAxis] - bestSplitVal;
				REAL d2 = m_triVertices[m_triIndices[triFaces[i] * 3 + 2]][bestSplitAxis] - bestSplitVal;

				// If the triangle straddles the negative half space
				if(d0 < 0.0f || d1 < 0.0f || d2 < 0.0f)
				{
					childTriFaces0.Add(triFaces[i]);
				}

				// If the triangle straddles the positive half space
				if(d0 >= 0.0f || d1 >= 0.0f || d2 >= 0.0f)
				{
					childTriFaces1.Add(triFaces[i]);
				}
			}

			CRuAABB childBounds0(bounds);
			childBounds0.Maxima()[bestSplitAxis] = bestSplitVal;

			CRuAABB	childBounds1(bounds);
			childBounds1.Minima()[bestSplitAxis] = bestSplitVal;

			INT32 treeSize0 = BuildTree(childNodeIndex0, childTriFaces0, depth + 1, childBounds0, maxTriPerLeaf, maxDepth, nodeListOut, leafDataListOut);
			INT32 treeSize1 = BuildTree(childNodeIndex1, childTriFaces1, depth + 1, childBounds1, maxTriPerLeaf, maxDepth, nodeListOut, leafDataListOut);

			// Setup kdNode data
			nodeListOut[nodeIndex].m_splitVal = bestSplitVal;

			return max(treeSize0, treeSize1);
		}
	}

	// Fill node data
	kdLeafData leafData;
	leafData.m_numTriangles = numTriangles;
	leafData.m_triFaces = ruNEW UINT16 [triFaces.Count()];
	for(INT32 i = 0; i < triFaces.Count(); ++i)
	{
		leafData.m_triFaces[i] = triFaces[i];
	}

	// Append new leaf data to leaf data list
	INT32 leafIndex = leafDataListOut.Count();
	leafDataListOut.Add(leafData);

	// Setup kdNode data
	nodeListOut[nodeIndex].m_leafIndex = (leafIndex << 2) | 0x00000003;

	return treeSize;
}

BOOL CRuCollision_kdTree_16::QueryCollision(CRuCollisionQuery_Ray *rayQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

	if(axis >= 3)
	{
		CRuVector3 intPt;
		REAL intT;

		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

			const CRuVector3 &v0 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & rayQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(rayQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & rayQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(FindIntersection(rayQuery->m_colRay, v0, v1, v2, intPt, intT))
			{
				if(rayQuery->m_ignoreBackface)
				{
					// Perform a backface check
					CRuVector3 e0(v1 - v0);
					CRuVector3 e1(v2 - v0);
					CRuVector3 n = CrossProduct(e0, e1);

					// Normal and ray direction must be opposite
					if(DotProduct(rayQuery->m_colRay.Direction(), n) < 0.0f)
					{
						rayQuery->m_colPoints->Add(intPt * *(rayQuery->m_worldTransform));
						rayQuery->m_colDistances->Add(intT);
					}
				}
				else
				{
					rayQuery->m_colPoints->Add(intPt * *(rayQuery->m_worldTransform));
					rayQuery->m_colDistances->Add(intT);
				}
			}
		}

		return TRUE;
	}

	INT32 nearNodeOffset = rayQuery->m_colRay.Origin()[axis] >= m_nodeArray[nodeIndex].m_splitVal;

	if(rayQuery->m_colRay.Direction()[axis] == 0.0f)
	{
		INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

		// Ray parallel to splitting plane, visit near side only
		QueryCollision(rayQuery, nearNodeIndex, rayMin, rayMax);
	}
	else
	{
		static const REAL m_epsilon_DropBranch = 1e-4f;

		// Find t value for intersection between ray and split plane
		REAL t = (m_nodeArray[nodeIndex].m_splitVal - rayQuery->m_colRay.Origin()[axis]) / rayQuery->m_colRay.Direction()[axis];

		// Test if ray straddles splitting plane
		if(t < 0.0f || t > rayMax + m_epsilon_DropBranch)
		{
			// Traverse near side
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

			QueryCollision(rayQuery, nearNodeIndex, rayMin, rayMax);
		}
		else if(t < rayMin - m_epsilon_DropBranch)
		{
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			QueryCollision(rayQuery, farNodeIndex, rayMin, rayMax);
		}
		else
		{
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			// Traverse near side first, then far side
			QueryCollision(rayQuery, nearNodeIndex, rayMin, t);
			QueryCollision(rayQuery, farNodeIndex, t, rayMax);
		}
	}

	return TRUE;
}

BOOL CRuCollision_kdTree_16::QueryCollision(CRuCollisionQuery_Ray_Nearest *rayQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

	if(axis >= 3)
	{
		CRuVector3 intPt;
		REAL intT;

		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

			const CRuVector3 &v0 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & rayQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(rayQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & rayQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(FindIntersection(rayQuery->m_colRay, v0, v1, v2, intPt, intT))
			{
				if(rayQuery->m_ignoreBackface)
				{
					// Perform a backface check
					CRuVector3 e0(v1 - v0);
					CRuVector3 e1(v2 - v0);
					CRuVector3 n = CrossProduct(e0, e1);

					// Normal and ray direction must be opposite
					if(DotProduct(rayQuery->m_colRay.Direction(), n) < 0.0f)
					{
						if(intT < rayQuery->m_collisionT)
						{
							rayQuery->m_positiveHit = TRUE;
							rayQuery->m_collisionT = intT;
							rayQuery->m_collisionPoint = intPt * *(rayQuery->m_worldTransform);
							rayQuery->m_collisionFlag = m_triFlags[m_leafArray[leafIndex].m_triFaces[i]];
						}
					}
				}
				else
				{
					if(intT < rayQuery->m_collisionT)
					{
						rayQuery->m_positiveHit = TRUE;
						rayQuery->m_collisionT = intT;
						rayQuery->m_collisionPoint = intPt * *(rayQuery->m_worldTransform);
						rayQuery->m_collisionFlag = m_triFlags[m_leafArray[leafIndex].m_triFaces[i]];
					}
				}
			}
		}

		return TRUE;
	}

	INT32 nearNodeOffset = rayQuery->m_colRay.Origin()[axis] >= m_nodeArray[nodeIndex].m_splitVal;

	if(rayQuery->m_colRay.Direction()[axis] == 0.0f)
	{
		INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

		// Ray parallel to splitting plane, visit near side only
		QueryCollision(rayQuery, nearNodeIndex, rayMin, rayMax);
	}
	else
	{
		static const REAL m_epsilon_DropBranch = 1e-4f;

		// Find t value for intersection between ray and split plane
		REAL t = (m_nodeArray[nodeIndex].m_splitVal - rayQuery->m_colRay.Origin()[axis]) / rayQuery->m_colRay.Direction()[axis];

		// Test if ray straddles splitting plane
		if(t < 0.0f || t > rayMax + m_epsilon_DropBranch)
		{
			// Traverse near side
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

			QueryCollision(rayQuery, nearNodeIndex, rayMin, rayMax);
		}
		else if(t < rayMin - m_epsilon_DropBranch)
		{
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			QueryCollision(rayQuery, farNodeIndex, rayMin, rayMax);
		}
		else
		{
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			// Traverse near side first, then far side
			QueryCollision(rayQuery, nearNodeIndex, rayMin, t);
			QueryCollision(rayQuery, farNodeIndex, t, rayMax);
		}
	}

	return TRUE;
}

BOOL CRuCollision_kdTree_16::QueryCollision(CRuCollisionQuery_Segment *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

	if(axis >= 3)
	{
		CRuVector3 intPt;
		REAL intT;

		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

			const CRuVector3 &v0 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & segmentQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(segmentQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & segmentQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(FindIntersection(segmentQuery->m_colSegment, v0, v1, v2, intPt, intT))
			{
				if(segmentQuery->m_ignoreBackface)
				{
					// Perform a backface check
					CRuVector3 e0(v1 - v0);
					CRuVector3 e1(v2 - v0);
					CRuVector3 n = CrossProduct(e0, e1);

					// Normal and ray direction must be opposite
					if(DotProduct(segmentQuery->m_colSegment.Direction(), n) < 0.0f)
					{
						segmentQuery->m_colPoints->Add(intPt * *(segmentQuery->m_worldTransform));
						segmentQuery->m_colDistances->Add(intT);
					}
				}
				else
				{
					segmentQuery->m_colPoints->Add(intPt * *(segmentQuery->m_worldTransform));
					segmentQuery->m_colDistances->Add(intT);
				}
			}
		}

		return TRUE;
	}

	INT32 nearNodeOffset = segmentQuery->m_colSegment.Origin()[axis] >= m_nodeArray[nodeIndex].m_splitVal;

	if(segmentQuery->m_colSegment.Direction()[axis] == 0.0f)
	{
		INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

		// Ray parallel to splitting plane, visit near side only
		QueryCollision(segmentQuery, nearNodeIndex, rayMin, rayMax);
	}
	else
	{
		static const REAL m_epsilon_DropBranch = 1e-4f;

		// Find t value for intersection between ray and split plane
		REAL t = (m_nodeArray[nodeIndex].m_splitVal - segmentQuery->m_colSegment.Origin()[axis]) / segmentQuery->m_colSegment.Direction()[axis];

		// Test if ray straddles splitting plane
		if(t < 0.0f || t > rayMax + m_epsilon_DropBranch)
		{
			// Traverse near side
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

			QueryCollision(segmentQuery, nearNodeIndex, rayMin, rayMax);
		}
		else if(t < rayMin - m_epsilon_DropBranch)
		{
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			QueryCollision(segmentQuery, farNodeIndex, rayMin, rayMax);
		}
		else
		{
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			// Traverse near side first, then far side
			QueryCollision(segmentQuery, nearNodeIndex, rayMin, t);
			QueryCollision(segmentQuery, farNodeIndex, t, rayMax);
		}
	}

	return TRUE;
}

BOOL CRuCollision_kdTree_16::QueryCollision(CRuCollisionQuery_Segment_HitTest *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

	if(axis >= 3)
	{
		CRuVector3 intPt;
		REAL intT;

		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

			const CRuVector3 &v0 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & segmentQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(segmentQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & segmentQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(FindIntersection(segmentQuery->m_colSegment, v0, v1, v2, intPt, intT))
			{
				if(segmentQuery->m_ignoreBackface)
				{
					// Perform a backface check
					CRuVector3 e0(v1 - v0);
					CRuVector3 e1(v2 - v0);
					CRuVector3 n = CrossProduct(e0, e1);

					// Normal and ray direction must be opposite
					if(DotProduct(segmentQuery->m_colSegment.Direction(), n) < 0.0f)
					{
						segmentQuery->m_positiveHit = TRUE;
						return TRUE;
					}
				}
				else
				{
					segmentQuery->m_positiveHit = TRUE;
					return TRUE;
				}
			}
		}

		return TRUE;
	}

	INT32 nearNodeOffset = segmentQuery->m_colSegment.Origin()[axis] >= m_nodeArray[nodeIndex].m_splitVal;

	if(segmentQuery->m_colSegment.Direction()[axis] == 0.0f)
	{
		INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

		// Ray parallel to splitting plane, visit near side only
		QueryCollision(segmentQuery, nearNodeIndex, rayMin, rayMax);
	}
	else
	{
		static const REAL m_epsilon_DropBranch = 1e-4f;

		// Find t value for intersection between ray and split plane
		REAL t = (m_nodeArray[nodeIndex].m_splitVal - segmentQuery->m_colSegment.Origin()[axis]) / segmentQuery->m_colSegment.Direction()[axis];

		// Test if ray straddles splitting plane
		if(t < 0.0f || t > rayMax + m_epsilon_DropBranch)
		{
			// Traverse near side
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

			QueryCollision(segmentQuery, nearNodeIndex, rayMin, rayMax);
		}
		else if(t < rayMin - m_epsilon_DropBranch)
		{
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			QueryCollision(segmentQuery, farNodeIndex, rayMin, rayMax);
		}
		else
		{
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			// Traverse near side first, then far side
			QueryCollision(segmentQuery, nearNodeIndex, rayMin, t);

			if(segmentQuery->m_positiveHit == FALSE)
			{
				QueryCollision(segmentQuery, farNodeIndex, t, rayMax);
			}
		}
	}

	return TRUE;
}

BOOL CRuCollision_kdTree_16::QueryCollision(CRuCollisionQuery_Segment_Nearest *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

	if(axis >= 3)
	{
		CRuVector3 intPt;
		REAL intT;

		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

			const CRuVector3 &v0 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & segmentQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(segmentQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & segmentQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(FindIntersection(segmentQuery->m_colSegment, v0, v1, v2, intPt, intT))
			{
				CRuVector3 e0(v1 - v0);
				CRuVector3 e1(v2 - v0);
				CRuVector3 n = CrossProduct(e0, e1);

				// Normalize
				n.Normalize();

				// Perform a backface check
				if(segmentQuery->m_ignoreBackface)
				{
					// Normal and ray direction must be opposite
					if(DotProduct(segmentQuery->m_colSegment.Direction(), n) < 0.0f)
					{
						if(intT < segmentQuery->m_collisionT)
						{
							segmentQuery->m_positiveHit = TRUE;
							segmentQuery->m_collisionT = intT;
							segmentQuery->m_collisionPoint = intPt * *(segmentQuery->m_worldTransform);
							segmentQuery->m_collisionNormal = n;
							segmentQuery->m_collisionFlag = m_triFlags[m_leafArray[leafIndex].m_triFaces[i]];
						}
					}
				}
				else
				{
					if(intT < segmentQuery->m_collisionT)
					{
						segmentQuery->m_positiveHit = TRUE;
						segmentQuery->m_collisionT = intT;
						segmentQuery->m_collisionPoint = intPt * *(segmentQuery->m_worldTransform);
						segmentQuery->m_collisionNormal = n;
						segmentQuery->m_collisionFlag = m_triFlags[m_leafArray[leafIndex].m_triFaces[i]];
					}
				}
			}
		}

		return TRUE;
	}

	INT32 nearNodeOffset = segmentQuery->m_colSegment.Origin()[axis] >= m_nodeArray[nodeIndex].m_splitVal;

	if(segmentQuery->m_colSegment.Direction()[axis] == 0.0f)
	{
		INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

		// Ray parallel to splitting plane, visit near side only
		QueryCollision(segmentQuery, nearNodeIndex, rayMin, rayMax);
	}
	else
	{
		static const REAL m_epsilon_DropBranch = 1e-4f;

		// Find t value for intersection between ray and split plane
		REAL t = (m_nodeArray[nodeIndex].m_splitVal - segmentQuery->m_colSegment.Origin()[axis]) / segmentQuery->m_colSegment.Direction()[axis];

		// Test if ray straddles splitting plane
		if(t < 0.0f || t > rayMax + m_epsilon_DropBranch)
		{
			// Traverse near side
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

			QueryCollision(segmentQuery, nearNodeIndex, rayMin, rayMax);
		}
		else if(t < rayMin - m_epsilon_DropBranch)
		{
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			QueryCollision(segmentQuery, farNodeIndex, rayMin, rayMax);
		}
		else
		{
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			// Traverse near side first, then far side
			QueryCollision(segmentQuery, nearNodeIndex, rayMin, t);
			QueryCollision(segmentQuery, farNodeIndex, t, rayMax);
		}
	}

	return TRUE;
}

BOOL CRuCollision_kdTree_16::QueryCollision(CRuCollisionQuery_OBB_HitTest *boxQuery, INT32 nodeIndex, CRuAABB &volBounds)
{
	BOOL flagSet = FALSE;
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

	if(axis >= 3)
	{
		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

			const CRuVector3 &v0 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & boxQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(boxQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & boxQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			// If an intersection occurs, mark as positive hit and return immediately
			if(IntersectsTriangle(boxQuery->m_colOBB, v0, v1, v2))
			{
				boxQuery->m_positiveHit = TRUE;

				return TRUE;
			}
		}

		return TRUE;
	}

	// near node is one that is on the side of the box minima
	INT32 nearNodeIndex = nodeIndex * 2 + 1;
	INT32 farNodeIndex = nearNodeIndex + 1;

	CRuAABB nearBounds(volBounds);
	CRuAABB farBounds(volBounds);

	nearBounds.Maxima()[axis] = m_nodeArray[nodeIndex].m_splitVal;
	farBounds.Minima()[axis] = m_nodeArray[nodeIndex].m_splitVal;

	// Query on near side
	if(boxQuery->m_positiveHit == FALSE && Intersects(nearBounds, boxQuery->m_colOBB))
	{
		QueryCollision(boxQuery, nearNodeIndex, nearBounds);
	}

	// Query on far side
	if(boxQuery->m_positiveHit == FALSE && Intersects(farBounds, boxQuery->m_colOBB))
	{
		QueryCollision(boxQuery, farNodeIndex, farBounds);
	}

	return TRUE;
}

BOOL CRuCollision_kdTree_16::QueryCollision(CRuCollisionQuery_OBB_TriEnum *boxQuery, INT32 nodeIndex, CRuAABB &volBounds)
{
	BOOL flagSet = FALSE;
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

	if(axis >= 3)
	{
		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

			const CRuVector3 &v0 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & boxQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(boxQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & boxQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			// If an intersection occurs, mark as positive hit and return immediately
			if(IntersectsTriangle(boxQuery->m_colOBB, v0, v1, v2))
			{
				// Calculate hit normal
				CRuVector3 e0(v1 - v0);
				CRuVector3 e1(v2 - v0);
				CRuVector3 n = CrossProduct(e0, e1);

				if(n.SquaredMagnitude() > ruEPSILON)
				{
					CRuVector3 p0 = v0;
					CRuVector3 p1 = v1;
					CRuVector3 p2 = v2;
					boxQuery->m_worldTransform->TransformPoint(p0);
					boxQuery->m_worldTransform->TransformPoint(p1);
					boxQuery->m_worldTransform->TransformPoint(p2);

					n.Normalize();
					boxQuery->m_worldTransform->TransformVector(n);

					boxQuery->m_colTriVerts->Add(p0);
					boxQuery->m_colTriVerts->Add(p1);
					boxQuery->m_colTriVerts->Add(p2);

					boxQuery->m_colTriNormals->Add(n);
					boxQuery->m_colTriFlags->Add(m_triFlags[m_leafArray[leafIndex].m_triFaces[i]]);
				}
			}
		}

		return TRUE;
	}

	// near node is one that is on the side of the box minima
	INT32 nearNodeIndex = nodeIndex * 2 + 1;
	INT32 farNodeIndex = nearNodeIndex + 1;

	CRuAABB nearBounds(volBounds);
	CRuAABB farBounds(volBounds);

	nearBounds.Maxima()[axis] = m_nodeArray[nodeIndex].m_splitVal;
	farBounds.Minima()[axis] = m_nodeArray[nodeIndex].m_splitVal;

	// Query on near side
	if(Intersects(nearBounds, boxQuery->m_colOBB))
	{
		QueryCollision(boxQuery, nearNodeIndex, nearBounds);
	}

	// Query on far side
	if(Intersects(farBounds, boxQuery->m_colOBB))
	{
		QueryCollision(boxQuery, farNodeIndex, farBounds);
	}

	return TRUE;
}

BOOL CRuCollision_kdTree_16::QueryCollision(CRuCollisionQuery_Sphere_HitTest *sphereQuery, INT32 nodeIndex, CRuVector3 &volNearPt)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

	if(axis >= 3)
	{
		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

			const CRuVector3 &v0 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & sphereQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(sphereQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & sphereQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			// Return immediately on collision
			if(IntersectsTriangle(sphereQuery->m_colSphere, v0, v1, v2))
			{
				sphereQuery->m_positiveHit = TRUE;
				return TRUE;
			}
		}

		return TRUE;
	}

	INT32 nearNodeOffset = sphereQuery->m_colSphere.Center()[axis] >= m_nodeArray[nodeIndex].m_splitVal;
	INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

	// Always visit near side child
	QueryCollision(sphereQuery, nearNodeIndex, volNearPt);

	// Update nearest point on volume when traversing far side.
	float oldValue = volNearPt[axis];
	volNearPt[axis] = m_nodeArray[nodeIndex].m_splitVal;

	// If the sphere overlaps the volume of the far node, recurse into it
	CRuVector3 diff = volNearPt - sphereQuery->m_colSphere.Center();
	if(diff.SquaredMagnitude() < sphereQuery->m_colSphere.Radius() * sphereQuery->m_colSphere.Radius())
	{
		INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);
		QueryCollision(sphereQuery, farNodeIndex, volNearPt);
	}

	// Restore component of nearest point on volume
	volNearPt[axis] = oldValue;

	return TRUE;
}

BOOL CRuCollision_kdTree_16::QueryCollision(CRuCollisionQuery_Sphere_TriEnum *sphereQuery, INT32 nodeIndex, CRuVector3 &volNearPt)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

	if(axis >= 3)
	{
		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

			const CRuVector3 &v0 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & sphereQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(sphereQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & sphereQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(IntersectsTriangle(sphereQuery->m_colSphere, v0, v1, v2))
			{
				REAL colDepth = sphereQuery->m_colSphere.Radius() - RuSqrt(SquaredDistance(sphereQuery->m_colSphere.Center(), v0, v1, v2));

				// Calculate hit normal
				CRuVector3 e0(v1 - v0);
				CRuVector3 e1(v2 - v0);
				CRuVector3 n = CrossProduct(e0, e1);

				if(n.SquaredMagnitude() > ruEPSILON)
				{
					CRuVector3 p0 = v0;
					CRuVector3 p1 = v1;
					CRuVector3 p2 = v2;
					sphereQuery->m_worldTransform->TransformPoint(p0);
					sphereQuery->m_worldTransform->TransformPoint(p1);
					sphereQuery->m_worldTransform->TransformPoint(p2);

					n.Normalize();
					sphereQuery->m_worldTransform->TransformVector(n);

					sphereQuery->m_colTriVerts->Add(p0);
					sphereQuery->m_colTriVerts->Add(p1);
					sphereQuery->m_colTriVerts->Add(p2);

					sphereQuery->m_colTriNormals->Add(n);
					sphereQuery->m_colTriFlags->Add(m_triFlags[m_leafArray[leafIndex].m_triFaces[i]]);
				}
			}
		}

		return TRUE;
	}

	INT32 nearNodeOffset = sphereQuery->m_colSphere.Center()[axis] >= m_nodeArray[nodeIndex].m_splitVal;
	INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

	// Always visit near side child
	QueryCollision(sphereQuery, nearNodeIndex, volNearPt);

	// Update nearest point on volume when traversing far side.
	float oldValue = volNearPt[axis];
	volNearPt[axis] = m_nodeArray[nodeIndex].m_splitVal;

	// If the sphere overlaps the volume of the far node, recurse into it
	CRuVector3 diff = volNearPt - sphereQuery->m_colSphere.Center();
	if(diff.SquaredMagnitude() < sphereQuery->m_colSphere.Radius() * sphereQuery->m_colSphere.Radius())
	{
		INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);
		QueryCollision(sphereQuery, farNodeIndex, volNearPt);
	}

	// Restore component of nearest point on volume
	volNearPt[axis] = oldValue;

	return TRUE;
}

BOOL CRuCollision_kdTree_16::QueryCollision(CRuCollisionQuery_Enumerate *enumQuery, INT32 nodeIndex)
{
	for(INT32 i = 0, j = 0; j < m_numTriangles; i += 3, ++j)
	{
		// Check triangle's flags against the exclusion mask
		if((m_triFlags[j] & enumQuery->m_exclusionMask) != 0)
		{
			continue;
		}

		// Check triangle's flags against the inclusion mask
		if(enumQuery->m_inclusionMask && (m_triFlags[j] & enumQuery->m_inclusionMask) == 0)
		{
			continue;
		}

		enumQuery->m_colTriVerts->Add(m_triVertices[i]);
		enumQuery->m_colTriVerts->Add(m_triVertices[i + 1]);
		enumQuery->m_colTriVerts->Add(m_triVertices[i + 2]);
	}


	return TRUE;
}

BOOL CRuCollision_kdTree_16::QueryCollision(CRuCollisionQuery_User_Flag_Set_Ray *setQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	BOOL flagSet = FALSE;
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

	if(axis >= 3)
	{
		CRuVector3 intPt;
		REAL intT;

		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			const CRuVector3 &v0 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & setQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(setQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & setQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(FindIntersection(setQuery->m_markingRay, v0, v1, v2, intPt, intT))
			{
				// Set flag...
				flagSet = TRUE;
				m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] = (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & ~setQuery->m_flagMask) | (setQuery->m_markingFlag & setQuery->m_flagMask);
			}
		}

		if(flagSet)
		{
			for(INT32 i = 0; i < setQuery->m_affectedObjects->Count(); ++i)
			{
				if(setQuery->m_affectedObjects->Get(i) == reinterpret_cast<void *>(this))
				{
					return TRUE;
				}
			}

			setQuery->m_affectedObjects->Add(reinterpret_cast<void *>(this));
		}

		return TRUE;
	}

	INT32 nearNodeOffset = setQuery->m_markingRay.Origin()[axis] >= m_nodeArray[nodeIndex].m_splitVal;

	if(setQuery->m_markingRay.Direction()[axis] == 0.0f)
	{
		INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

		// Ray parallel to splitting plane, visit near side only
		QueryCollision(setQuery, nearNodeIndex, rayMin, rayMax);
	}
	else
	{
		static const REAL m_epsilon_DropBranch = 1e-4f;

		// Find t value for intersection between ray and split plane
		REAL t = (m_nodeArray[nodeIndex].m_splitVal - setQuery->m_markingRay.Origin()[axis]) / setQuery->m_markingRay.Direction()[axis];

		// Test if ray straddles splitting plane
		if(t < 0.0f || t > rayMax + m_epsilon_DropBranch)
		{
			// Traverse near side
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

			QueryCollision(setQuery, nearNodeIndex, rayMin, rayMax);
		}
		else if(t < rayMin - m_epsilon_DropBranch)
		{
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			QueryCollision(setQuery, farNodeIndex, rayMin, rayMax);
		}
		else
		{
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			// Traverse near side first, then far side
			QueryCollision(setQuery, nearNodeIndex, rayMin, t);
			QueryCollision(setQuery, farNodeIndex, t, rayMax);
		}
	}

	return TRUE;
}

BOOL CRuCollision_kdTree_16::QueryCollision(CRuCollisionQuery_User_Flag_Set_Sphere *setQuery, INT32 nodeIndex, CRuVector3 &volNearPt)
{
	BOOL flagSet = FALSE;
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

	if(axis >= 3)
	{
		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			// Skip triangle if it has already been checked
			if(m_bitField[m_leafArray[leafIndex].m_triFaces[i]])
				continue;
			m_bitField.Set(m_leafArray[leafIndex].m_triFaces[i]);

			const CRuVector3 &v0 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			// Check triangle's flags against the exclusion mask
			if((m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & setQuery->m_exclusionMask) != 0)
			{
				continue;
			}

			// Check triangle's flags against the inclusion mask
			if(setQuery->m_inclusionMask && (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & setQuery->m_inclusionMask) == 0)
			{
				continue;
			}

			if(IntersectsTriangle(setQuery->m_markingSphere, v0, v1, v2))
			{
				// Set flag...
				flagSet = TRUE;
				m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] = (m_triFlags[m_leafArray[leafIndex].m_triFaces[i]] & ~setQuery->m_flagMask) | (setQuery->m_markingFlag & setQuery->m_flagMask);
			}
		}

		if(flagSet)
		{
			for(INT32 i = 0; i < setQuery->m_affectedObjects->Count(); ++i)
			{
				if(setQuery->m_affectedObjects->Get(i) == reinterpret_cast<void *>(this))
				{
					return TRUE;
				}
			}

			setQuery->m_affectedObjects->Add(reinterpret_cast<void *>(this));
		}

		return TRUE;
	}

	INT32 nearNodeOffset = setQuery->m_markingSphere.Center()[axis] >= m_nodeArray[nodeIndex].m_splitVal;
	INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

	// Always visit near side child
	QueryCollision(setQuery, nearNodeIndex, volNearPt);

	// Update nearest point on volume when traversing far side.
	float oldValue = volNearPt[axis];
	volNearPt[axis] = m_nodeArray[nodeIndex].m_splitVal;

	// If the sphere overlaps the volume of the far node, recurse into it
	CRuVector3 diff = volNearPt - setQuery->m_markingSphere.Center();
	if(diff.SquaredMagnitude() < setQuery->m_markingSphere.Radius() * setQuery->m_markingSphere.Radius())
	{
		INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);
		QueryCollision(setQuery, farNodeIndex, volNearPt);
	}

	// Restore component of nearest point on volume
	volNearPt[axis] = oldValue;

	return TRUE;
}

BOOL CRuCollision_kdTree_16::QueryCollision(CRuCollisionQuery_User_Flag_Get_Segment *segmentQuery, INT32 nodeIndex, REAL rayMin, REAL rayMax)
{
	INT32 axis = m_nodeArray[nodeIndex].m_leafIndex & 0x00000003;

	if(axis >= 3)
	{
		CRuVector3 intPt;
		REAL intT;

		// Leaf node
		INT32 leafIndex = m_nodeArray[nodeIndex].m_leafIndex >> 2;

		// Test with every triangle at the leaf
		for(INT32 i = 0; i < m_leafArray[leafIndex].m_numTriangles; ++i)
		{
			const CRuVector3 &v0 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3]];
			const CRuVector3 &v1 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 1]];
			const CRuVector3 &v2 = m_triVertices[m_triIndices[m_leafArray[leafIndex].m_triFaces[i] * 3 + 2]];

			if(FindIntersection(segmentQuery->m_colSegment, v0, v1, v2, intPt, intT))
			{
				if(segmentQuery->m_ignoreBackface)
				{
					// Perform a backface check
					CRuVector3 e0(v1 - v0);
					CRuVector3 e1(v2 - v0);
					CRuVector3 n = CrossProduct(e0, e1);

					// Normal and ray direction must be opposite
					if(DotProduct(segmentQuery->m_colSegment.Direction(), n) < 0.0f)
					{
						if(intT < segmentQuery->m_collisionT)
						{
							segmentQuery->m_positiveHit = TRUE;
							segmentQuery->m_collisionT = intT;
							segmentQuery->m_collisionPoint = intPt * *(segmentQuery->m_worldTransform);
							segmentQuery->m_flag = m_triFlags[m_leafArray[leafIndex].m_triFaces[i]];
						}
					}
				}
				else
				{
					if(intT < segmentQuery->m_collisionT)
					{
						segmentQuery->m_positiveHit = TRUE;
						segmentQuery->m_collisionT = intT;
						segmentQuery->m_collisionPoint = intPt * *(segmentQuery->m_worldTransform);
						segmentQuery->m_flag = m_triFlags[m_leafArray[leafIndex].m_triFaces[i]];
					}
				}
			}
		}

		return TRUE;
	}

	INT32 nearNodeOffset = segmentQuery->m_colSegment.Origin()[axis] >= m_nodeArray[nodeIndex].m_splitVal;

	if(segmentQuery->m_colSegment.Direction()[axis] == 0.0f)
	{
		INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

		// Ray parallel to splitting plane, visit near side only
		QueryCollision(segmentQuery, nearNodeIndex, rayMin, rayMax);
	}
	else
	{
		static const REAL m_epsilon_DropBranch = 1e-4f;

		// Find t value for intersection between ray and split plane
		REAL t = (m_nodeArray[nodeIndex].m_splitVal - segmentQuery->m_colSegment.Origin()[axis]) / segmentQuery->m_colSegment.Direction()[axis];

		// Test if ray straddles splitting plane
		if(t < 0.0f || t > rayMax + m_epsilon_DropBranch)
		{
			// Traverse near side
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;

			QueryCollision(segmentQuery, nearNodeIndex, rayMin, rayMax);
		}
		else if(t < rayMin - m_epsilon_DropBranch)
		{
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			QueryCollision(segmentQuery, farNodeIndex, rayMin, rayMax);
		}
		else
		{
			INT32 nearNodeIndex = nodeIndex * 2 + 1 + nearNodeOffset;
			INT32 farNodeIndex = nodeIndex * 2 + 1 + (nearNodeOffset ^ 1);

			// Traverse near side first, then far side
			QueryCollision(segmentQuery, nearNodeIndex, rayMin, t);

			if(segmentQuery->m_positiveHit == FALSE)
			{
				QueryCollision(segmentQuery, farNodeIndex, t, rayMax);
			}
		}
	}

	return TRUE;
}

BOOL CRuCollision_kdTree_16::QueryCollision(CRuCollisionQuery_User_Flag_Enumerate *enumQuery, INT32 nodeIndex)
{
	// Iterate through all triangles
	for(INT32 i = 0, j = 0; i < m_numTriangles; ++i)
	{
		// If the triangle matches the exclusion mask, skip it
		if((m_triFlags[i] & enumQuery->m_exclusionMask) != 0x00000000)
		{
			j += 3;
			continue;
		}

		// Add vertices of the triangle to output list
		for(INT32 k = 0; k < 3; ++k, ++j)
		{
			enumQuery->m_colTriVerts->Add(m_triVertices[m_triIndices[j]]);
		}

		// Add triangle's flag to output list
		enumQuery->m_colTriFlags->Add(m_triFlags[i]);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
