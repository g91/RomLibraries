#include "../Collision/RuCollision_Simple.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuCollision_Simple, IRuCollisionObject)
ruHEAP_DEFINE_SUBCLASS(CRuCollision_Simple, "CRuCollision_Simple", "IRuCollisionObject")
ruCLASSGUID_DEFINE(CRuCollision_Simple, 0x8B46B7D7, 0x95B04fc1, 0xB9882510, 0x7CD1C99B)

// ************************************************************************************************************************************************************

CRuCollision_Simple::CRuCollision_Simple()
:	m_numColVertices(0),
	m_numColPrimitives(0),
	m_colVertices(NULL),
	m_colFlags(NULL),
	m_colIndices(NULL)
{
}

CRuCollision_Simple::~CRuCollision_Simple()
{
	delete[] m_colVertices;
	delete[] m_colFlags;
	delete[] m_colIndices;
}

BOOL CRuCollision_Simple::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	switch(version)
	{
		case 1:
			{
				// Read bounds
				stream->Read(&m_bounds.Minima().x, sizeof(REAL));
				stream->Read(&m_bounds.Minima().y, sizeof(REAL));
				stream->Read(&m_bounds.Minima().z, sizeof(REAL));
				stream->Read(&m_bounds.Maxima().x, sizeof(REAL));
				stream->Read(&m_bounds.Maxima().y, sizeof(REAL));
				stream->Read(&m_bounds.Maxima().z, sizeof(REAL));

				// Read number of vertices
				stream->Read(&m_numColVertices, sizeof(INT32));

				// Read number of primitives
				stream->Read(&m_numColPrimitives, sizeof(INT32));

				// Read vertices
				m_colVertices = ruNEW CRuVector3 [m_numColVertices];
				for(INT32 i = 0; i < m_numColVertices; ++i)
				{
					stream->Read(&m_colVertices[i].x, sizeof(REAL));
					stream->Read(&m_colVertices[i].y, sizeof(REAL));
					stream->Read(&m_colVertices[i].z, sizeof(REAL));
				}

				// Read collision flags
				m_colFlags = ruNEW UINT32 [m_numColPrimitives];
				stream->Read(m_colFlags, sizeof(UINT32) * m_numColPrimitives);

				// Read collision indices
				m_colIndices = ruNEW UINT16 [m_numColPrimitives * 3];
				stream->Read(m_colIndices, sizeof(UINT16) * m_numColPrimitives * 3);
			}

			break;
	}

	return TRUE;
}

BOOL CRuCollision_Simple::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Write bounds
	stream->Write(&m_bounds.Minima().x, sizeof(REAL));
	stream->Write(&m_bounds.Minima().y, sizeof(REAL));
	stream->Write(&m_bounds.Minima().z, sizeof(REAL));
	stream->Write(&m_bounds.Maxima().x, sizeof(REAL));
	stream->Write(&m_bounds.Maxima().y, sizeof(REAL));
	stream->Write(&m_bounds.Maxima().z, sizeof(REAL));

	// Write number of vertices
	stream->Write(&m_numColVertices, sizeof(INT32));

	// Write number of primitives
	stream->Write(&m_numColPrimitives, sizeof(INT32));

	// Write vertices
	for(INT32 i = 0; i < m_numColVertices; ++i)
	{
		stream->Write(&m_colVertices[i].x, sizeof(REAL));
		stream->Write(&m_colVertices[i].y, sizeof(REAL));
		stream->Write(&m_colVertices[i].z, sizeof(REAL));
	}

	// Write collision flags
	stream->Write(m_colFlags, sizeof(UINT32) * m_numColPrimitives);

	// Write collision indices
	stream->Write(m_colIndices, sizeof(UINT16) * m_numColPrimitives * 3);

	return TRUE;
}

const CRuAABB &CRuCollision_Simple::GetBounds() const
{
	return m_bounds;
}

BOOL CRuCollision_Simple::QueryCollision(CRuCollisionQuery *colQuery)
{
	switch(colQuery->m_queryType)
	{
		case ruCOLQUERY_RAY:
			{
				return QueryCollision(static_cast<CRuCollisionQuery_Ray *>(colQuery));
			}

			break;

		case ruCOLQUERY_RAY_PICK:
			{
				return QueryCollision(static_cast<CRuCollisionQuery_Ray_Pick *>(colQuery));
			}

			break;

		case ruCOLQUERY_RAY_TRIPICK:
			{
			}

			break;

		case ruCOLQUERY_SEGMENT:
			{
				return QueryCollision(static_cast<CRuCollisionQuery_Segment *>(colQuery));
			}

			break;

		case ruCOLQUERY_SEGMENT_HITTEST:
			{
				return QueryCollision(static_cast<CRuCollisionQuery_Segment_HitTest *>(colQuery));
			}

			break;

		case ruCOLQUERY_SEGMENT_NEAREST:
			{
				return QueryCollision(static_cast<CRuCollisionQuery_Segment_Nearest *>(colQuery));
			}

			break;

		case ruCOLQUERY_ENUMERATE:
			{
				return QueryCollision(static_cast<CRuCollisionQuery_Enumerate *>(colQuery));
			}

			break;

		case ruCOLQUERY_USER_FLAG_SET_RAY:
			{
				return QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Set_Ray *>(colQuery));
			}

			break;

		case ruCOLQUERY_USER_FLAG_SET_SPHERE:
			{
				return QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Set_Sphere *>(colQuery));
			}

			break;

		case ruCOLQUERY_USER_FLAG_GET_SEGMENT:
			{
				return QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Get_Segment *>(colQuery));
			}

			break;

		case ruCOLQUERY_USER_FLAG_ENUMERATE:
			{
				return QueryCollision(static_cast<CRuCollisionQuery_User_Flag_Enumerate *>(colQuery));
			}

			break;
	}

	return TRUE;
}

INT32 CRuCollision_Simple::GetNumColVertices() const
{
	return m_numColVertices;
}

INT32 CRuCollision_Simple::GetNumColPrimitives() const
{
	return m_numColPrimitives;
}

CRuVector3 *CRuCollision_Simple::GetColVertices()
{
	return m_colVertices;
}

UINT32 *CRuCollision_Simple::GetColFlags()
{
	return m_colFlags;
}

UINT16 *CRuCollision_Simple::GetColIndices()
{
	return m_colIndices;
}

BOOL CRuCollision_Simple::QueryCollision(CRuCollisionQuery_Ray *rayQuery)
{
	// Get the ray in local coordinates
	CRuRay3 localRay(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));

	// Test ray with bounding box
	if(Intersects(localRay, m_bounds))
	{
		// Test with every triangle
		CRuVector3 intPt;
		float intT;
		for(INT32 i = 0, j = 0; i < m_numColPrimitives; ++i)
		{
			INT32 faceIndices[3] = { m_colIndices[j++], m_colIndices[j++], m_colIndices[j++] };

			if(FindIntersection(localRay, m_colVertices[faceIndices[0]], m_colVertices[faceIndices[1]], m_colVertices[faceIndices[2]], intPt, intT))
			{
				if(rayQuery->m_ignoreBackface)
				{
					// Perform a backface check
					CRuVector3 e0(m_colVertices[faceIndices[1]] - m_colVertices[faceIndices[0]]);
					CRuVector3 e1(m_colVertices[faceIndices[2]] - m_colVertices[faceIndices[0]]);
					CRuVector3 n = CrossProduct(e0, e1);

					// Normal and ray direction must be opposite
					if(DotProduct(localRay.Direction(), n) < 0.0f)
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
	}

	return TRUE;
}

BOOL CRuCollision_Simple::QueryCollision(CRuCollisionQuery_Ray_Pick *rayQuery)
{
	// Get the ray in local coordinates
	CRuRay3 localRay(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));

	// Test ray with bounding box
	if(Intersects(localRay, m_bounds))
	{
		rayQuery->m_positiveHit = TRUE;

		// temp: return arbitrary distance
		CRuVector3 dummyVec;
		FindIntersection(localRay, m_bounds, dummyVec, rayQuery->m_colDistance);
	}

	return TRUE;
}

BOOL CRuCollision_Simple::QueryCollision(CRuCollisionQuery_Segment *segmentQuery)
{
	// Get the segment in local coordinates
	CRuSegment3 localSegment(segmentQuery->m_colSegment);
	segmentQuery->m_invWorldTransform->TransformPoint(localSegment.Origin());
	segmentQuery->m_invWorldTransform->TransformVector(localSegment.Direction());

	// Test segment with bounding box
	if(Intersects(localSegment, m_bounds))
	{
		// Test with every triangle
		CRuVector3 intPt;
		float intT;
		for(INT32 i = 0, j = 0; i < m_numColPrimitives; ++i)
		{
			INT32 faceIndices[3] = { m_colIndices[j++], m_colIndices[j++], m_colIndices[j++] };

			if(FindIntersection(localSegment, m_colVertices[faceIndices[0]], m_colVertices[faceIndices[1]], m_colVertices[faceIndices[2]], intPt, intT))
			{
				if(segmentQuery->m_ignoreBackface)
				{
					// Perform a backface check
					CRuVector3 e0(m_colVertices[faceIndices[1]] - m_colVertices[faceIndices[0]]);
					CRuVector3 e1(m_colVertices[faceIndices[2]] - m_colVertices[faceIndices[0]]);
					CRuVector3 n = CrossProduct(e0, e1);

					// Normal and ray direction must be opposite
					if(DotProduct(localSegment.Direction(), n) < 0.0f)
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
	}

	return TRUE;
}

BOOL CRuCollision_Simple::QueryCollision(CRuCollisionQuery_Segment_HitTest *segmentQuery)
{
	return TRUE;
}

BOOL CRuCollision_Simple::QueryCollision(CRuCollisionQuery_Segment_Nearest *segmentQuery)
{
	// Get the segment in local coordinates
	CRuSegment3 localSegment(segmentQuery->m_colSegment);
	segmentQuery->m_invWorldTransform->TransformPoint(localSegment.Origin());
	segmentQuery->m_invWorldTransform->TransformVector(localSegment.Direction());

	// Test segment with bounding box
	if(Intersects(localSegment, m_bounds))
	{
		// Test with every triangle
		CRuVector3 intPt;
		float intT;
		for(INT32 i = 0, j = 0; i < m_numColPrimitives; ++i)
		{
			INT32 faceIndices[3] = { m_colIndices[j++], m_colIndices[j++], m_colIndices[j++] };

			if(FindIntersection(localSegment, m_colVertices[faceIndices[0]], m_colVertices[faceIndices[1]], m_colVertices[faceIndices[2]], intPt, intT))
			{
				CRuVector3 e0(m_colVertices[faceIndices[1]] - m_colVertices[faceIndices[0]]);
				CRuVector3 e1(m_colVertices[faceIndices[2]] - m_colVertices[faceIndices[0]]);
				CRuVector3 n = CrossProduct(e0, e1);

				// Normalize
				n.Normalize();

				// Perform a backface check
				if(segmentQuery->m_ignoreBackface)
				{
					// Normal and ray direction must be opposite
					if(DotProduct(localSegment.Direction(), n) < 0.0f)
					{
						if(intT < segmentQuery->m_collisionT)
						{
							segmentQuery->m_positiveHit = TRUE;
							segmentQuery->m_collisionT = intT;
							segmentQuery->m_collisionPoint = intPt * *(segmentQuery->m_worldTransform);
							segmentQuery->m_collisionNormal = n;
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
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL CRuCollision_Simple::QueryCollision(CRuCollisionQuery_Enumerate *enumQuery)
{
	return TRUE;
}

BOOL CRuCollision_Simple::QueryCollision(CRuCollisionQuery_User_Flag_Set_Ray *setQuery)
{
	return TRUE;
}

BOOL CRuCollision_Simple::QueryCollision(CRuCollisionQuery_User_Flag_Set_Sphere *setQuery)
{
	return TRUE;
}

BOOL CRuCollision_Simple::QueryCollision(CRuCollisionQuery_User_Flag_Get_Segment *segmentQuery)
{
	return TRUE;
}

BOOL CRuCollision_Simple::QueryCollision(CRuCollisionQuery_User_Flag_Enumerate *enumQuery)
{
	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
