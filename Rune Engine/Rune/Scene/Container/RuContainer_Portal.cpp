#include "../../Scene/Container/RuContainer_Portal.h"

#include "../../Scene/Base/RuEntityACT.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuContainer_Portal, IRuEntity_Container)
ruHEAP_DEFINE_SUBCLASS(CRuContainer_Portal, "CRuContainer_Portal", "IRuEntity_Container")
ruCLASSGUID_DEFINE(CRuContainer_Portal, 0x0609E0D9, 0x9FD44208, 0x9F2EF97D, 0x40EC2AC7)

// ************************************************************************************************************************************************************

CRuContainer_Portal::CRuContainer_Portal()
:	m_disablePortalFunction(FALSE),
	m_positionType(ruPORTAL_ROOMTYPE_OUTDOOR),
	m_hideOutdoor(FALSE),
	m_lastCameraPos(0.0f, FLT_MAX, 0.0f)
{
	// Register listener on self
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuContainer_Portal::HandleEvent_Dispose, this, _1), NULL);

	if(g_ruEngineSettings)
	{
		// Register listener on engine settings
		g_ruEngineSettings->RegisterSettingsChangeHandler(this->GetGUID(), boost::bind(&CRuContainer_Portal::EngineSettingsChangedHandler, this, _1, _2));
	}
}

CRuContainer_Portal::~CRuContainer_Portal()
{
	if(g_ruEngineSettings)
	{
		// Unregister listener on engine settings
		g_ruEngineSettings->UnregisterSettingsChangeHandler(this->GetGUID());
	}

	for(INT32 i = 0; i < m_rooms.Count(); ++i)
	{
		ruSAFE_RELEASE(m_rooms[i].m_roomEntity);
		delete[] m_rooms[i].m_portals;
	}
}

void CRuContainer_Portal::SetCamera(CRuCamera *viewCamera)
{
	if(m_disablePortalFunction)
	{
		// Populate list of visible entities
		for(INT32 i = 0; i < m_rooms.Count(); ++i)
		{
			m_rooms[i].m_roomEntity->SetOption(ruENTITYOPTION_HIDDEN, FALSE);
		}

		// Update visibility chain
		for(INT32 i = 0; i < m_vizChainSlaves.Count(); ++i)
		{
			m_vizChainSlaves[i].m_entity->SetOption(ruENTITYOPTION_HIDDEN, FALSE);
		}
	}
	else
	{
		const CRuVector3 &cameraPosition = viewCamera->GetCameraPosition();
		
		if((m_lastCameraPos - cameraPosition).Magnitude() < 5.0f)
		{
			return;
		}

		m_lastCameraPos = cameraPosition;

		// Reset status data
		m_positionType = ruPORTAL_ROOMTYPE_OUTDOOR;
		m_hideOutdoor = FALSE;
		m_intersectingRooms.Clear();

		CRuCollisionQuery_Segment_HitTest colQuery;
		colQuery.m_colSegment = CRuSegment3(cameraPosition - CRuVector3(0.0f, 5000.0f, 0.0f), cameraPosition + CRuVector3(0.0f, 5000.0f, 0.0f));
		colQuery.m_ignoreBackface = FALSE;

		// First determine which indoor sectors the camera lies within, and the indoor-outdoor status of the camera
		for(INT32 i = 0; i < m_rooms.Count(); ++i)
		{
			// Reset visible and traversed flags
			m_rooms[i].m_roomVisible = FALSE;
			m_rooms[i].m_roomTraversed = FALSE;

			// Make collision query against room
			colQuery.m_positiveHit = FALSE;
			m_rooms[i].m_roomEntity->QueryCollision(&colQuery);

			if(colQuery.m_positiveHit)
			{
				// If the containing room is an indoor room, set the position type to indoor and insert it into the list of intersecting rooms
				if(m_rooms[i].m_roomType == ruPORTAL_ROOMTYPE_INDOOR)
				{
					m_positionType = ruPORTAL_ROOMTYPE_INDOOR;

					if(m_rooms[i].m_hideOutdoor)
					{
						m_hideOutdoor = TRUE;
					}

					m_rooms[i].m_roomVisible = TRUE;
					m_intersectingRooms.Add(i);
				}
			}
		}

		// If the camera lies outdoors, add all outdoor sectors to the list of intersecting rooms
		if(m_positionType == ruPORTAL_ROOMTYPE_OUTDOOR || m_intersectingRooms.Count() == 0)
		{
			for(INT32 i = 0; i < m_rooms.Count(); ++i)
			{
				if(m_rooms[i].m_roomType == ruPORTAL_ROOMTYPE_OUTDOOR)
				{
					m_rooms[i].m_roomVisible = TRUE;
					m_intersectingRooms.Add(i);
				}
			}
		}

		// Add all persistent rooms to the list of intersecting rooms
		for(INT32 i = 0; i < m_rooms.Count(); ++i)
		{
			if(m_rooms[i].m_roomType == ruPORTAL_ROOMTYPE_PERSISTENT && m_rooms[i].m_roomVisible == FALSE)
			{
				m_rooms[i].m_roomVisible = TRUE;
				m_intersectingRooms.Add(i);
			}
		}

		// Recursively enumerate all visible rooms
		for(INT32 i = 0; i < m_intersectingRooms.Count(); ++i)
		{
			EnumerateVisibleRooms(m_intersectingRooms[i], viewCamera, CRuVector2(-1.0f, -1.0f), CRuVector2(1.0f, 1.0f));
		}

		// Populate list of visible entities
		for(INT32 i = 0; i < m_rooms.Count(); ++i)
		{
			if(m_rooms[i].m_roomVisible)
			{
				m_rooms[i].m_roomEntity->SetOption(ruENTITYOPTION_HIDDEN, FALSE);
			}
			else
			{
				m_rooms[i].m_roomEntity->SetOption(ruENTITYOPTION_HIDDEN, TRUE);
			}
		}

		// Update visibility chain
		for(INT32 i = 0; i < m_vizChainSlaves.Count(); ++i)
		{
			if(m_rooms[m_vizChainSlaves[i].m_index].m_roomVisible)
			{
				m_vizChainSlaves[i].m_entity->SetOption(ruENTITYOPTION_HIDDEN, FALSE);
			}
			else
			{
				m_vizChainSlaves[i].m_entity->SetOption(ruENTITYOPTION_HIDDEN, TRUE);
			}
		}
	}
}

void CRuContainer_Portal::DisablePortalFunction(BOOL disablePortalFunction)
{
	m_disablePortalFunction = disablePortalFunction;
}

IRuObject *CRuContainer_Portal::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuContainer_Portal();

	// Chain cloning call to parent class
	IRuEntity_Container::Clone(clonedObject);

	// Shortcut reference to the cloned entity
	CRuContainer_Portal *clonedContainer = static_cast<CRuContainer_Portal *>(clonedObject);

	// Clone all rooms
	for(INT32 i = 0; i < m_rooms.Count(); ++i)
	{
		// Append new room structure
		clonedContainer->m_rooms.Add(Room());
		Room &room = clonedContainer->m_rooms[clonedContainer->m_rooms.Count() - 1];

		IRuObject *clonedEntity = m_rooms[i].m_roomEntity->Clone();

		// Alternate processing for ACT rooms
		if(m_rooms[i].m_roomEntity->GetType().IsSubClassOf(CRuACTEntity::Type()))
		{
			clonedEntity = static_cast<CRuACTEntity *>(m_rooms[i].m_roomEntity)->GetTemplate()->Construct();
		}

		if(clonedEntity && clonedEntity->GetType().IsSubClassOf(CRuEntity::Type()))
		{
			room.m_roomGUID = m_rooms[i].m_roomGUID;
			room.m_roomType = m_rooms[i].m_roomType;
			room.m_hideOutdoor = m_rooms[i].m_hideOutdoor;
			room.m_roomEntity = static_cast<CRuEntity *>(clonedEntity);
//			room.m_roomIteration = room.m_roomEntity->GetIteration();
			room.m_numPortals = m_rooms[i].m_numPortals;
			room.m_portals = ruNEW Portal [room.m_numPortals];

			for(INT32 j = 0; j < room.m_numPortals; ++j)
			{
				room.m_portals[j] = m_rooms[i].m_portals[j];
			}

			// Disable cloning and serialization on the room entity
			room.m_roomEntity->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
			room.m_roomEntity->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);

			// Attach child to portal container
			clonedContainer->AddChild(room.m_roomEntity);
		}
		else
		{
			ruSAFE_RELEASE(clonedContainer);
			return NULL;
		}
	}

	return clonedContainer;
}

BOOL CRuContainer_Portal::SerializeFrom(IRuStream *stream)
{
	// Validate GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version 
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Read number of rooms
	INT32 numRooms = 0;
	stream->Read(&numRooms, sizeof(INT32));

	// Read data for each room
	for(INT32 i = 0; i < numRooms; ++i)
	{
		// Append new room structure
		m_rooms.Add(Room());
		Room &room = m_rooms[m_rooms.Count() - 1];

		// Read room GUID
		if(version > 1)
		{
			RuReadGUID(stream, room.m_roomGUID);
		}

		// Read room type
		stream->Read(&room.m_roomType, sizeof(RuPortal_RoomType));

		// Read hide setting
		if(version > 2)
		{
			stream->Read(&room.m_hideOutdoor, sizeof(BOOL));
		}
		else
		{
			room.m_hideOutdoor = FALSE;
		}

		// Read room entity using the object factory
		IRuObject *entityObject = RuObjectFactory().CreateObject(stream);

		if(entityObject == NULL)
		{
		}
		else
		{
			if(entityObject->GetType().IsSubClassOf(CRuACTTemplate::Type()))
			{
				// Construct an entity from the ACT template
				room.m_roomEntity = static_cast<CRuACTTemplate *>(entityObject)->Construct();
				ruSAFE_RELEASE(entityObject);
			}
			else if(entityObject->GetType().IsSubClassOf(CRuEntity::Type()))
			{
				room.m_roomEntity = static_cast<CRuEntity *>(entityObject);

				// Disable cloning and serialization on the room entity
				room.m_roomEntity->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
				room.m_roomEntity->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);

				// Attach child to portal container
				this->AddChild(room.m_roomEntity);
			}
		}

		// Read number of portals
		stream->Read(&room.m_numPortals, sizeof(INT32));

		// Read data for each portal
		room.m_portals = ruNEW Portal [room.m_numPortals];
		for(INT32 j = 0; j < room.m_numPortals; ++j)
		{
			// Read normal
			stream->Read(&room.m_portals[j].m_normal.x, sizeof(REAL));
			stream->Read(&room.m_portals[j].m_normal.y, sizeof(REAL));
			stream->Read(&room.m_portals[j].m_normal.z, sizeof(REAL));

			// Read portal vertices
			for(INT32 k = 0; k < 4; ++k)
			{
				stream->Read(&room.m_portals[j].m_vertices[k].x, sizeof(REAL));
				stream->Read(&room.m_portals[j].m_vertices[k].y, sizeof(REAL));
				stream->Read(&room.m_portals[j].m_vertices[k].z, sizeof(REAL));
			}

			// Read destination room index
			stream->Read(&room.m_portals[j].m_desination, sizeof(INT32));
		}
	}

	return TRUE;
}

BOOL CRuContainer_Portal::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 3;
	stream->Write(&version, sizeof(INT32));

	// Write number of rooms
	INT32 numRooms = m_rooms.Count();
	stream->Write(&numRooms, sizeof(INT32));

	// Write data for each room
	for(INT32 i = 0; i < numRooms; ++i)
	{
		Room &room = m_rooms[i];

		// Write room GUID
		stream->Write(room.m_roomGUID.Data(), room.m_roomGUID.Size());

		// Write room type
		stream->Write(&room.m_roomType, sizeof(RuPortal_RoomType));

		// Write hide setting
		stream->Write(&room.m_hideOutdoor, sizeof(BOOL));

		// Write room entity as embedded data stream
		if(room.m_roomEntity->GetType().IsSubClassOf(CRuACTEntity::Type()))
		{
			static_cast<CRuACTEntity *>(room.m_roomEntity)->GetTemplate()->SerializeTo(stream);
		}
		else if(room.m_roomEntity->GetType().IsSubClassOf(CRuEntity::Type()))
		{
			room.m_roomEntity->SerializeTo(stream);
		}

		// Write number of portals
		stream->Write(&room.m_numPortals, sizeof(INT32));

		// Write data for each portal
		for(INT32 j = 0; j < room.m_numPortals; ++j)
		{
			// Write normal
			stream->Write(&room.m_portals[j].m_normal.x, sizeof(REAL));
			stream->Write(&room.m_portals[j].m_normal.y, sizeof(REAL));
			stream->Write(&room.m_portals[j].m_normal.z, sizeof(REAL));

			// Write portal vertices
			for(INT32 k = 0; k < 4; ++k)
			{
				stream->Write(&room.m_portals[j].m_vertices[k].x, sizeof(REAL));
				stream->Write(&room.m_portals[j].m_vertices[k].y, sizeof(REAL));
				stream->Write(&room.m_portals[j].m_vertices[k].z, sizeof(REAL));
			}

			// Write destination room index
			stream->Write(&room.m_portals[j].m_desination, sizeof(INT32));
		}
	}

	return TRUE;
}

BOOL CRuContainer_Portal::ResetStates()
{
	return TRUE;
}

BOOL CRuContainer_Portal::Update(float elapsedTime)
{
	// Force an update of bounding transforms
	this->GetWorldBounds();

	// Chain update call to parent class
	return IRuEntity_Container::Update(elapsedTime);
}

BOOL CRuContainer_Portal::QueryCollision(CRuCollisionQuery *colQuery)
{
	// If collision query is disabled, return immediately
	if(colQuery->m_ignoreCollisionFlag == FALSE && GetOption(ruENTITYOPTION_COLLISION) == 0)
		return TRUE;

	// Save previous collision transform
	CRuMatrix4x4 *originalWorldTransform = colQuery->m_worldTransform;
	CRuMatrix4x4 *originalInvWorldTransform = colQuery->m_invWorldTransform;

	// Store the inverse of the current world transform into the collision query
	colQuery->m_worldTransform = const_cast<CRuMatrix4x4 *>(&this->GetWorldTransform());
	colQuery->m_invWorldTransform = const_cast<CRuMatrix4x4 *>(&this->GetInverseWorldTransform());

	// Return value
	BOOL retVal = TRUE;

	if(m_collisionObject && !colQuery->m_queryOnOriginalGeometry)
	{
		// Query attached collision object
		m_collisionObject->QueryCollision(colQuery);
	}
	else
	{
		// Query collision on all of the children
		for(INT32 i = 0; i < m_rooms.Count(); ++i)
		{
			if(m_rooms[i].m_roomEntity->QueryCollision(colQuery) == FALSE)
			{
				retVal = FALSE;
				break;
			}
		}
	}

	// Restore original collision transform
	colQuery->m_worldTransform = originalWorldTransform;
	colQuery->m_invWorldTransform = originalInvWorldTransform;

	return retVal;
}

BOOL CRuContainer_Portal::AddRoom(RuPortal_RoomType roomType, BOOL hideOutdoor, CRuEntity *roomEntity)
{
	if(roomEntity)
	{
		// Increment reference on room entity
		roomEntity->AddRef();

		// Create new room descriptor
		Room room;
		room.m_roomType = roomType;
		room.m_hideOutdoor = hideOutdoor;
		room.m_roomEntity = roomEntity;
		room.m_roomIteration = 0;
		room.m_numPortals = 0;
		room.m_portals = NULL;
		room.m_roomVisible = TRUE;
		room.m_roomTraversed = FALSE;

		// Insert room descriptor into room list
		m_rooms.Add(room);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuContainer_Portal::AddPortal(INT32 srcIndex, INT32 dstIndex, CRuVector3 vertices[4], CRuVector3 &normal)
{
	if(srcIndex >= 0 && srcIndex < m_rooms.Count() && dstIndex >= 0 && dstIndex < m_rooms.Count())
	{
		Room &srcRoom = m_rooms[srcIndex];
		Room &dstRoom = m_rooms[dstIndex];

		Portal *newSrcPortalList = ruNEW Portal [srcRoom.m_numPortals + 1];
		memcpy(newSrcPortalList, srcRoom.m_portals, sizeof(Portal) * srcRoom.m_numPortals);
		newSrcPortalList[srcRoom.m_numPortals].m_desination = dstIndex;
		newSrcPortalList[srcRoom.m_numPortals].m_vertices[0] = vertices[0];
		newSrcPortalList[srcRoom.m_numPortals].m_vertices[1] = vertices[1];
		newSrcPortalList[srcRoom.m_numPortals].m_vertices[2] = vertices[2];
		newSrcPortalList[srcRoom.m_numPortals].m_vertices[3] = vertices[3];
		newSrcPortalList[srcRoom.m_numPortals].m_normal = normal;

		delete[] srcRoom.m_portals;
		srcRoom.m_numPortals = srcRoom.m_numPortals + 1;
		srcRoom.m_portals = newSrcPortalList;

		return TRUE;
	}

	return FALSE;
}

INT32 CRuContainer_Portal::GetNumRooms() const
{
	return m_rooms.Count();
}

const CRuGUID &CRuContainer_Portal::GetRoomGUID(INT32 index) const
{
	return m_rooms[index].m_roomGUID;
}

CRuEntity *CRuContainer_Portal::GetRoomEntity(INT32 index)
{
	return m_rooms[index].m_roomEntity;
}

BOOL CRuContainer_Portal::AddToVisibilityChain(const CRuGUID &roomGUID, CRuEntity *slaveEntity)
{
	// Make sure the slave entity is not already in the visibility chain
	for(INT32 i = 0; i < m_vizChainSlaves.Count(); ++i)
	{
		if(m_vizChainSlaves[i].m_entity == slaveEntity)
		{
			return FALSE;
		}
	}

	// Scan through all rooms to find appropriate room to attach to
	for(INT32 i = 0; i < m_rooms.Count(); ++i)
	{
		if(m_rooms[i].m_roomGUID == roomGUID)
		{
			VizSlave slave = { i, slaveEntity };

			// Register event handler on slave
			slaveEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuContainer_Portal::HandleEvent_Dispose, this, _1), NULL);

			// Add slave to list of slaves
			m_vizChainSlaves.Add(slave);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuContainer_Portal::CameraPositionIsIndoor()
{
	return m_positionType == ruPORTAL_ROOMTYPE_INDOOR;
}

BOOL CRuContainer_Portal::HideOutdoor()
{
	return m_hideOutdoor;
}

BOOL CRuContainer_Portal::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	RuObjectEvent_Dispose_Args *disposeArgs = static_cast<RuObjectEvent_Dispose_Args *>(eventArgs);

	if(disposeArgs->m_object == this)
	{
		// Unregister from all slaves in the visibility chain
		for(INT32 i = 0; i < m_vizChainSlaves.Count(); ++i)
		{
			m_vizChainSlaves[i].m_entity->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		m_vizChainSlaves.Clear();
	}
	else
	{
		// Slave is being destroyed, remove slave from the visibility chain
		for(INT32 i = 0; i < m_vizChainSlaves.Count(); ++i)
		{
			if(m_vizChainSlaves[i].m_entity == disposeArgs->m_object)
			{
				m_vizChainSlaves.RemoveAt(i);
				break;
			}
		}
	}

	return TRUE;
}

BOOL CRuContainer_Portal::EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting)
{
	GetSettingsFromEngine();
	return TRUE;
}

void CRuContainer_Portal::EnumerateVisibleRooms(INT32 roomIndex, CRuCamera *camera, CRuVector2 &viewportMinima, CRuVector2 &viewportMaxima)
{
	const CRuVector3 &camPos = camera->GetCameraPosition();

	CRuVector2 curMinima, curMaxima;
	CRuVector3 wsEyeDir, ssVertices[4];
	CRuVector4 ssVertices4D[4];

	Room &room = m_rooms[roomIndex];

	// If the room has already been traversed, return immediately
	if(room.m_roomTraversed != false)
	{
		return;
	}

	// Mark room as traversed
	room.m_roomTraversed = TRUE;

	for(INT32 j = 0; j < room.m_numPortals; ++j)
	{
		BOOL portalIsVisible = FALSE;

		// Transform portal vertices into screen space
		for(INT32 k = 0; k < 4; ++k)
		{
			ssVertices[k] = room.m_portals[j].m_vertices[k];

			// Transform portal vertex into world space
			room.m_roomEntity->GetWorldTransform().TransformPoint(ssVertices[k]);

			// Transform portal vertex into projected space
			camera->GetFrustumTransform().TransformPoint(ssVertices[k], ssVertices4D[k]);

			// If w is negative, the portal is behind the camera
			if(ssVertices4D[k].m_w >= 0.0f)
			{
				portalIsVisible = TRUE;
			}

			// Divide by absolute value of W
			// Using absolute value of W for this division prevents the portal rectangle from skewing out of shape as the portal straddles the view plane
			for(INT32 l = 0; l < 3; l++)
			{
				ssVertices4D[k].m_elements[l] /= fabs(ssVertices4D[k].m_w);
			}
		}

		REAL dist = min(SquaredDistance(camPos, ssVertices[0], ssVertices[1], ssVertices[2]), SquaredDistance(camPos, ssVertices[2], ssVertices[3], ssVertices[0]));

		if(dist > 25.0f)
		{
			CRuVector3 portalCenter = (room.m_portals[j].m_vertices[0] + room.m_portals[j].m_vertices[1] + room.m_portals[j].m_vertices[2] + room.m_portals[j].m_vertices[3]) * 0.25f;
			room.m_roomEntity->GetWorldTransform().TransformPoint(portalCenter);

			CRuVector3 portalWorldNormal(room.m_portals[j].m_normal);
			room.m_roomEntity->GetWorldTransform().TransformVector(portalWorldNormal);

			CRuVector3 vecToPortalCenter = camPos - portalCenter;

			// Test portal normal against view vector
			if(DotProduct(portalWorldNormal, vecToPortalCenter) < 0.0f)
			{
				continue;
			}
		}

		BOOL portalStraddlesNearPlane = FALSE;
		for(INT32 k = 0; k < 3; ++k)
		{
			if((ssVertices4D[k].m_w * ssVertices4D[k + 1].m_w) < 0.0f)
			{
				portalStraddlesNearPlane = TRUE;
			}
		}

		if(portalIsVisible == FALSE)
		{
			continue;
		}

		// Get bounding box of portal on the view plane
		curMinima.x = max(min(min(min(ssVertices4D[0].x, ssVertices4D[1].x), ssVertices4D[2].x), ssVertices4D[3].x), viewportMinima.x);
		curMinima.y = max(min(min(min(ssVertices4D[0].y, ssVertices4D[1].y), ssVertices4D[2].y), ssVertices4D[3].y), viewportMinima.y);
		curMaxima.x = min(max(max(max(ssVertices4D[0].x, ssVertices4D[1].x), ssVertices4D[2].x), ssVertices4D[3].x), viewportMaxima.x);
		curMaxima.y = min(max(max(max(ssVertices4D[0].y, ssVertices4D[1].y), ssVertices4D[2].y), ssVertices4D[3].y), viewportMaxima.y);

		// If portal projection does not lie on the current view area, it is not visible
		if(curMinima.x >= curMaxima.x || curMinima.y >= curMaxima.y)
		{
			continue;
		}

		// Mark destination room as visible
		m_rooms[room.m_portals[j].m_desination].m_roomVisible = TRUE;

		// Traverse the destination room
		EnumerateVisibleRooms(room.m_portals[j].m_desination, camera, curMinima, curMaxima);
	}
}

void CRuContainer_Portal::GetSettingsFromEngine()
{
	RuPropertyType propertyValue;
	m_disablePortalFunction = !(g_ruEngineSettings->GetSetting("portal:enable", propertyValue) ? boost::get<BOOL>(propertyValue) : TRUE);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
