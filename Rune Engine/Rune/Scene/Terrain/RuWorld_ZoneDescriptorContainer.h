#ifndef _RUWORLD_ZONEDESCRIPTORCONTAINER_H_
#define _RUWORLD_ZONEDESCRIPTORCONTAINER_H_

// Base types
#include "../../Core/DataStore/RuDataStore_Base.h"
#include "../../Scene/Terrain/RuWorld_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	...
*/
class CRuWorld_ZoneDescriptorContainer : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct ZoneDescriptorInfo
	{
		CRuWorld_ZoneDescriptor*				m_zoneDescriptor;										//!< Zone descriptor
		BOOL									m_dirty;												//!< Is the zone descriptor dirty?
	};

	// Data client
	CRuDataStore_Base*							m_dataStore;

	// Zone descriptor table
	CRuArrayList<INT32>							m_deletedZoneIDs;										//!< Deleted zone ID array
	CRuArrayList<ZoneDescriptorInfo>			m_zoneDescriptors;										//!< Zone descriptor array

	// Table handles
	INT32										m_tableHandle_ZoneDescriptor;

	INT32										m_tableHandle_ZoneDescriptor_Old;

	// Workspace
	CRuMemoryStream								m_tempStream;

public:
												CRuWorld_ZoneDescriptorContainer();
	virtual										~CRuWorld_ZoneDescriptorContainer();

	BOOL										Attach(CRuDataStore_Base *dataStore);
	BOOL										CopyTo(CRuDataStore_Base *targetDataStore);
	virtual BOOL								Save();

	// Zone descriptor stuff
	virtual INT32								GetNumZoneDescriptors() const;

	virtual CRuWorld_ZoneDescriptor*			GetZoneDescriptor(INT32 id);
	virtual CRuWorld_ZoneDescriptor*			GetZoneDescriptorByIndex(INT32 index);

	virtual BOOL								SetZoneDescriptor(CRuWorld_ZoneDescriptor *zoneDesc);
	virtual BOOL								RemoveZoneDescriptor(CRuWorld_ZoneDescriptor *zoneDesc);
	virtual BOOL								RemoveZoneDescriptorAtIndex(INT32 index);
	virtual BOOL								MarkZoneDescriptorAsDirty(CRuWorld_ZoneDescriptor *zoneDesc);

	virtual BOOL								SetZoneName(INT32 id, const char *zoneName);
	virtual BOOL								SetZoneID(INT32 oldID, INT32 newID);

protected:
	void										ClearZoneDescriptors();
	void										ClearDeletedZoneDescriptors();

	void										SaveTimeGroups();
	void										LoadTimeGroups();
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
