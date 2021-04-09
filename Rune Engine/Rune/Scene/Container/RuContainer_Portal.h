#ifndef _RUCONTAINER_PORTAL_H_
#define _RUCONTAINER_PORTAL_H_

// Container base
#include "../../Scene/Base/RuEntityContainer.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

enum RuPortal_RoomType
{
	ruPORTAL_ROOMTYPE_PERSISTENT			= 0,
	ruPORTAL_ROOMTYPE_INDOOR				= 1,
	ruPORTAL_ROOMTYPE_OUTDOOR				= 2,

	ruPORTAL_ROOMTYPE_FORCE_DWORD			= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

class CRuContainer_Portal : public IRuEntity_Container
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct Portal
	{
		CRuVector3							m_normal;						//!< Normal of the portal, always points towards room 0
		CRuVector3							m_vertices[4];					//!< Portal vertices in local space
		INT32								m_desination;					//!< Destination room index
	};

	struct Room
	{
		CRuGUID								m_roomGUID;						//!< Room unique identifier
		RuPortal_RoomType					m_roomType;						//!< Room type - persistent, indoor, outdoor
		BOOL								m_hideOutdoor;					//!< When inside, hide outdoor rooms?
		CRuEntity*							m_roomEntity;					//!< Room entity
		INT32								m_roomIteration;				//!< Last known iteration number
		INT32								m_numPortals;					//!< Number of portals owned by this room
		Portal*								m_portals;						//!< Array of portals owned by this room

		// Runtime
		BOOL								m_roomVisible;					//!< Whether the room is visible
		BOOL								m_roomTraversed;				//!< Has the room been visited by the enumerator?
	};

	struct VizSlave
	{
		INT32								m_index;						//!< Index of room to which the slave is chained
		CRuEntity*							m_entity;						//!< Slave entity object
	};

	CRuArrayList<Room>						m_rooms;						//!< Array of rooms owned by this portal container
	CRuArrayList<CRuEntity *>				m_visibleEntities;				//!< Array of visible entities

	CRuArrayList<VizSlave>					m_vizChainSlaves;				//!< Visibility chain slaves

	// Run-time status
	BOOL									m_disablePortalFunction;
	RuPortal_RoomType						m_positionType;
	BOOL									m_hideOutdoor;
	CRuArrayList<INT32>						m_intersectingRooms;

	CRuVector3								m_lastCameraPos;

public:
											CRuContainer_Portal();
	virtual									~CRuContainer_Portal();

	// LOD selection and frustum culling
	void									SetCamera(CRuCamera *viewCamera);
	void									DisablePortalFunction(BOOL disablePortalFunction);

	// IRuObject interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Entity interface
	virtual BOOL							ResetStates();
	virtual BOOL							Update(float elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// Portal container construction
	BOOL									AddRoom(RuPortal_RoomType roomType, BOOL hideOutdoor, CRuEntity *roomEntity);
	BOOL									AddPortal(INT32 srcIndex, INT32 dstIndex, CRuVector3 vertices[4], CRuVector3 &normal);

	// Direct portal child entity access
	INT32									GetNumRooms() const;
	const CRuGUID&							GetRoomGUID(INT32 index) const;
	CRuEntity*								GetRoomEntity(INT32 index);

	// Visibility chaining
	BOOL									AddToVisibilityChain(const CRuGUID &roomGUID, CRuEntity *slaveEntity);

	// State queries
	BOOL									CameraPositionIsIndoor();
	BOOL									HideOutdoor();

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);
	BOOL									EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting);

protected:
	void									EnumerateVisibleRooms(INT32 roomIndex, CRuCamera *camera, CRuVector2 &viewportMinima, CRuVector2 &viewportMaxima);
	void									GetSettingsFromEngine();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Container_Portal, CRuContainer_Portal)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
