#include "../../Scene/Terrain/RuWorld_ZoneDescriptorContainer.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuWorld_ZoneDescriptorContainer, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_ZoneDescriptorContainer, "CRuWorld_ZoneDescriptorContainer", "IRuObject")
ruCLASSGUID_DEFINE(CRuWorld_ZoneDescriptorContainer, 0xDBE1291E, 0x3E2744de, 0x8E5C920E, 0x0F9CF3A9)

// ************************************************************************************************************************************************************

static const char *zoneDescContainerInternal_TableName_ZoneDescriptor = "world_zone_descriptors";
static const char *gaiaDSInternal_TableName_ZoneDescriptor = "gaia_ds_zone_descriptors";

// ************************************************************************************************************************************************************

CRuWorld_ZoneDescriptorContainer::CRuWorld_ZoneDescriptorContainer()
:	m_dataStore(NULL)
{
	// Pre-initialize temporary stream buffer
	m_tempStream.SetBufferSize(4096);
}

CRuWorld_ZoneDescriptorContainer::~CRuWorld_ZoneDescriptorContainer()
{
	// Release data store
	ruSAFE_RELEASE(m_dataStore);

	ClearZoneDescriptors();
	ClearDeletedZoneDescriptors();
}

BOOL CRuWorld_ZoneDescriptorContainer::Attach(CRuDataStore_Base *dataStore)
{
	// Store reference to data store
	m_dataStore = dataStore;
	m_dataStore->AddRef();

	// Get data tables
	m_tableHandle_ZoneDescriptor = m_dataStore->GetTableHandle(zoneDescContainerInternal_TableName_ZoneDescriptor);
	m_tableHandle_ZoneDescriptor_Old = m_dataStore->GetTableHandle(gaiaDSInternal_TableName_ZoneDescriptor);

	if(m_tableHandle_ZoneDescriptor < 0)
	{
		m_tableHandle_ZoneDescriptor = m_dataStore->CreateTable(zoneDescContainerInternal_TableName_ZoneDescriptor, 4);
	}

	// Load time groups
	LoadTimeGroups();

	return TRUE;
}

BOOL CRuWorld_ZoneDescriptorContainer::CopyTo(CRuDataStore_Base *targetDataStore)
{
	if(m_dataStore)
	{
		INT32 srcTableList[1];
		srcTableList[0] = m_tableHandle_ZoneDescriptor;

		// Create tables
		INT32 targetTableList[1];
		targetTableList[0] = targetDataStore->CreateTable(zoneDescContainerInternal_TableName_ZoneDescriptor, 4);

		// Copy data in each table
		for(INT32 i = 0; i < 1; ++i)
		{
			INT32 numEntries = m_dataStore->GetNumRecords(srcTableList[i]);
			INT32 keyLength = m_dataStore->GetTableKeyLength(srcTableList[i]);
			BYTE *key = ruNEW BYTE [keyLength];

			for(INT32 j = 0; j < numEntries; ++j)
			{
				// Retrieve key and data
				IRuStream *stream = m_dataStore->RetrieveByIndex(srcTableList[i], j);

				if(stream)
				{
					m_dataStore->RetrieveKeyByIndex(srcTableList[i], j, key);
					targetDataStore->Store(targetTableList[i], key, stream);
					delete stream;
				}
			}
			
			delete[] key;
		}

		return TRUE;
	}

	return TRUE;
}

BOOL CRuWorld_ZoneDescriptorContainer::Save()
{
	// Save time groups
	SaveTimeGroups();

	return FALSE;
}

INT32 CRuWorld_ZoneDescriptorContainer::GetNumZoneDescriptors() const
{
	return m_zoneDescriptors.Count();
}

CRuWorld_ZoneDescriptor *CRuWorld_ZoneDescriptorContainer::GetZoneDescriptor(INT32 id)
{
	// Search for a descriptor with matching time group, weather group, and zone ID
	for(INT32 i = 0; i < m_zoneDescriptors.Count(); ++i)
	{
		// TODO: Put in extra search criteria for when exact match cannot be found
		if(	m_zoneDescriptors[i].m_zoneDescriptor->GetZoneID() == id)
		{
			return m_zoneDescriptors[i].m_zoneDescriptor;
		}
	}

	return NULL;
}

CRuWorld_ZoneDescriptor *CRuWorld_ZoneDescriptorContainer::GetZoneDescriptorByIndex(INT32 index)
{
	return m_zoneDescriptors[index].m_zoneDescriptor;
}

BOOL CRuWorld_ZoneDescriptorContainer::SetZoneDescriptor(CRuWorld_ZoneDescriptor *zoneDesc)
{
	// Search for a descriptor with matching zone ID
	for(INT32 i = 0; i < m_zoneDescriptors.Count(); ++i)
	{
		if(	m_zoneDescriptors[i].m_zoneDescriptor->GetZoneID() == zoneDesc->GetZoneID())
		{
			// Copy members
			m_zoneDescriptors[i].m_dirty = TRUE;
			*(m_zoneDescriptors[i].m_zoneDescriptor) = *zoneDesc;

			return TRUE;
		}
	}

	// Make a copy of the descriptor and add it to the descriptor table
	INT32 index = m_zoneDescriptors.Count();
	m_zoneDescriptors.Add(ZoneDescriptorInfo());
	m_zoneDescriptors[index].m_zoneDescriptor = ruNEW CRuWorld_ZoneDescriptor();
	m_zoneDescriptors[index].m_dirty = TRUE;
	*(m_zoneDescriptors[index].m_zoneDescriptor) = *zoneDesc;

	return TRUE;
}

BOOL CRuWorld_ZoneDescriptorContainer::RemoveZoneDescriptor(CRuWorld_ZoneDescriptor *zoneDesc)
{
	// Search for a descriptor with matching time group, weather group, and zone ID
	for(INT32 i = 0; i < m_zoneDescriptors.Count(); ++i)
	{
		if(m_zoneDescriptors[i].m_zoneDescriptor->GetZoneID() == zoneDesc->GetZoneID())
		{
			// Add zone ID to the list of deleted zone IDs
			m_deletedZoneIDs.Add(zoneDesc->GetZoneID());

			// Release zone descriptor and remove it from list
			ruSAFE_RELEASE(m_zoneDescriptors[i].m_zoneDescriptor);
			m_zoneDescriptors.RemoveAt(i);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuWorld_ZoneDescriptorContainer::RemoveZoneDescriptorAtIndex(INT32 index)
{
	if(index >= 0 && index < m_zoneDescriptors.Count())
	{
		// Add zone ID to the list of deleted zone IDs
		m_deletedZoneIDs.Add(m_zoneDescriptors[index].m_zoneDescriptor->GetZoneID());

		// Release zone descriptor and remove it from list
		ruSAFE_RELEASE(m_zoneDescriptors[index].m_zoneDescriptor);
		m_zoneDescriptors.RemoveAt(index);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuWorld_ZoneDescriptorContainer::MarkZoneDescriptorAsDirty(CRuWorld_ZoneDescriptor *zoneDesc)
{
	// Search for a descriptor with matching time group, weather group, and zone ID
	for(INT32 i = 0; i < m_zoneDescriptors.Count(); ++i)
	{
		if(	m_zoneDescriptors[i].m_zoneDescriptor->GetZoneID() == zoneDesc->GetZoneID())
		{
			// Mark descriptor as dirty
			m_zoneDescriptors[i].m_dirty = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuWorld_ZoneDescriptorContainer::SetZoneName(INT32 id, const char *zoneName)
{
	for(INT32 i = 0; i < m_zoneDescriptors.Count(); ++i)
	{
		if(m_zoneDescriptors[i].m_zoneDescriptor->GetZoneID() == id)
		{
			m_zoneDescriptors[i].m_zoneDescriptor->SetName(zoneName);
			m_zoneDescriptors[i].m_dirty = TRUE;
		}
	}

	return TRUE;
}

BOOL CRuWorld_ZoneDescriptorContainer::SetZoneID(INT32 oldID, INT32 newID)
{
	if(oldID == newID)
	{
		return FALSE;
	}

	// Make sure newID does not exist
	for(INT32 i = 0; i < m_zoneDescriptors.Count(); ++i)
	{
		if(m_zoneDescriptors[i].m_zoneDescriptor->GetZoneID() == newID)
		{
			return FALSE;
		}
	}

	// Find descriptor and change zone ID
	for(INT32 i = 0; i < m_zoneDescriptors.Count(); ++i)
	{
		if(m_zoneDescriptors[i].m_zoneDescriptor->GetZoneID() == oldID)
		{
			m_zoneDescriptors[i].m_zoneDescriptor->SetZoneID(newID);
			m_zoneDescriptors[i].m_dirty = TRUE;
		}

		// If parent zone ID is to be changed, change it
		if(m_zoneDescriptors[i].m_zoneDescriptor->GetParentZoneID() == oldID)
		{
			m_zoneDescriptors[i].m_zoneDescriptor->SetParentZoneID(newID);
			m_zoneDescriptors[i].m_dirty = TRUE;
		}
	}

	// Add old ID to the list of deleted zone IDs
	m_deletedZoneIDs.Add(oldID);

	return TRUE;
}

void CRuWorld_ZoneDescriptorContainer::ClearZoneDescriptors()
{
	for(INT32 i = 0; i < m_zoneDescriptors.Count(); ++i)
	{
		ruSAFE_RELEASE(m_zoneDescriptors[i].m_zoneDescriptor);
	}

	m_zoneDescriptors.Clear();
}

void CRuWorld_ZoneDescriptorContainer::ClearDeletedZoneDescriptors()
{
	m_deletedZoneIDs.Clear();
}

void CRuWorld_ZoneDescriptorContainer::SaveTimeGroups()
{
	// Iterate through deleted zone descriptors
	for(INT32 i = 0; i < m_deletedZoneIDs.Count(); ++i)
	{
		// Delete zone descriptor
		INT32 zoneID = m_deletedZoneIDs[i];
		m_dataStore->Delete(m_tableHandle_ZoneDescriptor, (BYTE *) &zoneID);
	}

	// Iterate through each zone descriptor looking for dirty ones
	for(INT32 i = 0; i < m_zoneDescriptors.Count(); ++i)
	{
		// Skip if the descriptor is not dirty
		if(m_zoneDescriptors[i].m_dirty == FALSE)
			continue;

		// Reset dirty flag
		m_zoneDescriptors[i].m_dirty = FALSE;

		// Reset output stream size
		m_tempStream.SetStreamSize(0);

		// Serialize zone descriptor to stream
		m_zoneDescriptors[i].m_zoneDescriptor->SerializeTo(&m_tempStream);

		// Store zone descriptor
		INT32 zoneID = m_zoneDescriptors[i].m_zoneDescriptor->GetZoneID();
		m_dataStore->Store(m_tableHandle_ZoneDescriptor, (BYTE *) &zoneID, &m_tempStream);
	}

	// If an old version of the zone descriptor table exists, delete the table
	if(m_tableHandle_ZoneDescriptor_Old >= 0)
	{
		m_dataStore->DeleteTable(gaiaDSInternal_TableName_ZoneDescriptor);
	}

	// Flush deleted descriptors list
	ClearDeletedZoneDescriptors();
}

void CRuWorld_ZoneDescriptorContainer::LoadTimeGroups()
{
	// If there is an old version of the table, convert it
	if(m_tableHandle_ZoneDescriptor_Old >= 0)
	{
		// Get number of zone descriptors
		INT32 numZoneDescriptors = m_dataStore->GetNumRecords(m_tableHandle_ZoneDescriptor_Old);

		// Iterate through each zone descriptor
		for(INT32 i = 0; i < numZoneDescriptors; ++i)
		{
			// Retrieve zone descriptor record
			IRuStream *stream = m_dataStore->RetrieveByIndex(m_tableHandle_ZoneDescriptor_Old, i);

			if(stream == NULL)
				continue;

			// Create new zone descriptor
			INT32 j = m_zoneDescriptors.Count();
			m_zoneDescriptors.Add(ZoneDescriptorInfo());
			m_zoneDescriptors[j].m_zoneDescriptor = ruNEW CRuWorld_ZoneDescriptor();
			m_zoneDescriptors[j].m_dirty = TRUE;

			// Serialize zone descriptor from stream
			m_zoneDescriptors[j].m_zoneDescriptor->SerializeFromOldVersion(stream);

			delete stream;
		}

		// Combine zone descriptors
		for(INT32 i = 0; i < m_zoneDescriptors.Count(); ++i)
		{
			CRuWorld_ZoneDescriptor *zoneDescriptor = m_zoneDescriptors[i].m_zoneDescriptor;

			for(INT32 j = i - 1; j >= 0; --j)
			{
				if(zoneDescriptor->GetZoneID() == m_zoneDescriptors[j].m_zoneDescriptor->GetZoneID())
				{
					// Merge zone settings into zone descriptor
					m_zoneDescriptors[j].m_zoneDescriptor->InsertZoneSettings(zoneDescriptor->GetZoneSettingsByIndex(0));

					// Remove duplicate zone descriptor
					ruSAFE_RELEASE(m_zoneDescriptors[i].m_zoneDescriptor);
					m_zoneDescriptors.RemoveAt(i);

					--i;
					break;
				}
			}
		}
	}
	else
	{
		// Get number of zone descriptors
		INT32 numZoneDescriptors = m_dataStore->GetNumRecords(m_tableHandle_ZoneDescriptor);

		// Iterate through each zone descriptor
		for(INT32 i = 0; i < numZoneDescriptors; ++i)
		{
			// Retrieve zone descriptor record
			INT32 key;
			BOOL keyRetrievalOK = m_dataStore->RetrieveKeyByIndex(m_tableHandle_ZoneDescriptor, i, (BYTE *) &key);
			IRuStream *stream = m_dataStore->RetrieveByIndex(m_tableHandle_ZoneDescriptor, i);

			if(stream == NULL)
				continue;

			// Create new zone descriptor
			INT32 j = m_zoneDescriptors.Count();
			m_zoneDescriptors.Add(ZoneDescriptorInfo());
			m_zoneDescriptors[j].m_zoneDescriptor = ruNEW CRuWorld_ZoneDescriptor();
			m_zoneDescriptors[j].m_dirty = FALSE;

			// Serialize zone descriptor from stream
			m_zoneDescriptors[j].m_zoneDescriptor->SerializeFrom(stream);

			// Make sure key is equal to the zone ID... if not, fix the discrepancy (what can possibly cause this situation?)
			if(key != m_zoneDescriptors[j].m_zoneDescriptor->GetZoneID())
			{
				m_zoneDescriptors[j].m_dirty = TRUE;
				m_zoneDescriptors[j].m_zoneDescriptor->SetZoneID(key);
			}

			delete stream;
		}
	}
}

// ************************************************************************************************************************************************************

#pragma managed
