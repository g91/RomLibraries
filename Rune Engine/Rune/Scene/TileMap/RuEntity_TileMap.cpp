/*!
	@file RuEntity_TileMap.cpp

	Copyright (c) 2004-2007 All rights reserved
*/

#include "../../Scene/TileMap/RuEntity_TileMap.h"

#include "../../Scene/Base/RuEntityModel.h"

#include "../../Helper/RuHelper_Entity.h"
#include "../../Helper/RuHelper_IRuEntity_Renderable.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_TileMap, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_TileMap, "CRuEntity_TileMap", "CRuEntity")
ruCLASSGUID_DEFINE(CRuEntity_TileMap, 0x6B407F6F, 0x30BF4d84, 0xAEB8B742, 0x97652EBD)

// ************************************************************************************************************************************************************

CRuEntity_TileMap::TileDescriptor::TileDescriptor()
:	m_tileCoord(0),
	m_tileResourceName(NULL),
	m_volume_MinX(0),
	m_volume_MinY(0),
	m_volume_MinZ(0),
	m_volume_MaxX(-1),
	m_volume_MaxY(-1),
	m_volume_MaxZ(-1),
	m_tileEntity(NULL)
{
}

CRuEntity_TileMap::TileDescriptor::~TileDescriptor()
{
	delete[] m_tileResourceName;
	ruSAFE_RELEASE(m_tileEntity);
}

// ************************************************************************************************************************************************************

CRuEntity_TileMap::CRuEntity_TileMap()
:	m_mapSize(0),
	m_mapDepth(0),
	m_layerHeight(0.0f),
	m_tileSize(0.0f),
	m_origin(0.0f, 0.0f, 0.0f),
	m_rootPath(NULL),
	m_tilePath(NULL),
	m_tileHash(NULL),
	m_enableEditorMode(FALSE),
	m_tileStateMap(NULL),
	m_exclusionMap(NULL),
	m_currentDepth(0),
	m_tempExclusion_Entity(NULL),
	m_tempExclusion_MinX(0),
	m_tempExclusion_MinY(0),
	m_tempExclusion_MinZ(0),
	m_tempExclusion_MaxX(-1),
	m_tempExclusion_MaxY(-1),
	m_tempExclusion_MaxZ(-1),
	m_tileGrid(NULL),
	m_defaultTileID(0),
	m_tileSelectMode(ruTILESELECTMODE_SINGLE),
	m_tilePartition(NULL)
{
}

CRuEntity_TileMap::~CRuEntity_TileMap()
{
	// Free system paths
	delete[] m_rootPath;
	delete[] m_tilePath;

	// release tile hashes....
	for(INT32 i = 0; i < m_mapDepth; ++i)
	{
		m_tileHash[i]->ForAllElements(boost::bind(&CRuEntity_TileMap::ClearTileHashCallback, this, _1, _2));
		delete m_tileHash[i];
	}
	delete[] m_tileHash;

	// Release placed custom tiles
	for(INT32 i = 0; i < m_placedCustomTiles.Count(); ++i)
	{
		delete m_placedCustomTiles[i];
	}

	// Free tile state map
	if(m_tileStateMap)
	{
		delete[] m_tileStateMap;
	}

	// Free tile exclusion map
	if(m_exclusionMap)
	{
		delete[] m_exclusionMap;
	}

	// Free temp exclusion entity
	ruSAFE_RELEASE(m_tempExclusion_Entity);

	// Free system tiles
	for(INT32 i = 0; i < 15; ++i)
	{
		for(INT32 j = 0; j < m_systemTiles[i].Count(); ++j)
		{
			ruSAFE_RELEASE(m_systemTiles[i][j]);
		}
	}

	// Free custom tiles
	for(INT32 j = 0; j < m_customTiles.Count(); ++j)
	{
		ruSAFE_RELEASE(m_customTiles[j]);
	}

	// Free tile grid
	ruSAFE_RELEASE(m_tileGrid);

	// Free tile partition
	ruSAFE_RELEASE(m_tilePartition);
}

BOOL CRuEntity_TileMap::Initialize(INT32 mapSize, INT32 mapDepth, REAL layerHeight)
{
	m_mapSize = mapSize;
	m_mapDepth = mapDepth;
	m_layerHeight = layerHeight;

	// Initialize tile hash
	m_tileHash = ruNEW CRuHashMap<INT32, TileDescriptor *> * [m_mapDepth];
	for(INT32 i = 0; i < m_mapDepth; ++i)
	{
		m_tileHash[i] = ruNEW CRuHashMap<INT32, TileDescriptor *>(1024);
	}

	// Initialize tile state map
	m_tileStateMap = ruNEW CRuBitField [m_mapDepth];
	m_exclusionMap = ruNEW CRuBitField [m_mapDepth];
	for(INT32 i = 0; i < m_mapDepth; ++i)
	{
		m_tileStateMap[i].SetNumBits(m_mapSize * m_mapSize);
		m_tileStateMap[i].ClearAll();

		m_exclusionMap[i].SetNumBits(m_mapSize * m_mapSize);
		m_exclusionMap[i].ClearAll();
	}

	return FALSE;
}

void CRuEntity_TileMap::SetRootPath(const char *rootPath)
{
	if(rootPath)
	{
		delete[] m_rootPath;

		INT32 rootPathLen = (INT32)strlen(rootPath) + 1;
		m_rootPath = ruNEW char [rootPathLen];
		memcpy(m_rootPath, rootPath, rootPathLen);
	}
}

void CRuEntity_TileMap::SetTilePath(const char *tilePath)
{
	if(tilePath)
	{
		delete[] m_tilePath;

		INT32 tilePathLen = (INT32)strlen(tilePath) + 1;
		m_tilePath = ruNEW char [tilePathLen];
		memcpy(m_tilePath, tilePath, tilePathLen);
	}
}

BOOL CRuEntity_TileMap::ActivateEditorMode()
{
	if(m_rootPath && m_tilePath)
	{
		// Scan tile path for tiles and automatically determine the tile size
		CRuString searchPath(m_rootPath);
		CRuString tileSpecifierSearchPath;

		if(searchPath.LastIndexOf("\\") != (searchPath.GetLength() - 1))
			searchPath.Append("\\");

		searchPath.Append(m_tilePath);
		tileSpecifierSearchPath = searchPath;

		if(searchPath.LastIndexOf("\\") == (searchPath.GetLength() - 1))
		{
			searchPath.Append("*.ros");
			tileSpecifierSearchPath.Append("*.size");
		}
		else
		{
			searchPath.Append("\\*.ros");
			tileSpecifierSearchPath.Append("\\*.size");
		}

		WIN32_FIND_DATA findData;
		HANDLE findHandle = FindFirstFile((char *) searchPath, &findData);

		while(findHandle != INVALID_HANDLE_VALUE)
		{
			CRuString fileName(findData.cFileName);
			fileName.Replace(".ros", "");

			// Tokenize file name
			CRuString **tokens = fileName.Tokenize("_");

			// Look for the last two tokens (tile ID and tile variation)
			CRuString *tileClassToken = NULL;
			CRuString *tileIDToken = NULL;
			CRuString *tileVariationToken = NULL;

			for(INT32 i = 0; tokens[i] != NULL; ++i)
			{
				if(i == 0)
				{
					tileClassToken = tokens[i];
				}
				else
				{
					delete tileIDToken;
					tileIDToken = tileVariationToken;
					tileVariationToken = tokens[i];
				}
			}

			// Load entity
			CRuString tileEntityPath(m_tilePath);

			if(tileEntityPath.LastIndexOf("\\") != (tileEntityPath.GetLength() - 1))
				tileEntityPath.Append("\\");
			tileEntityPath.Append(fileName);
			tileEntityPath.Append(".ros");

			CRuEntity *tileEntity = g_ruResourceManager->LoadEntity(tileEntityPath);

			if(tileEntity)
			{
				// Process this entity if it has both ID and variation tokens
				if(tileIDToken && tileVariationToken && stricmp(*tileClassToken, "tile") == 0)
				{
					// Convert tile ID and variation to integer
					INT32 tileID = atoi(*tileIDToken);
					INT32 tileVariation = atoi(*tileVariationToken);

					assert(tileID >= 0 && tileID <= 14);

					// Add tile to system tile list
					m_systemTiles[tileID].Add(tileEntity);
				}
				else
				{
					m_customTiles.Add(tileEntity);
				}
			}

			// Cleanup
			delete tileClassToken;
			delete tileIDToken;
			delete tileVariationToken;

			delete[] tokens;

			// Find the next file
			if(FindNextFile(findHandle, &findData) == FALSE)
			{
				FindClose(findHandle);
				findHandle = INVALID_HANDLE_VALUE;
			}
		}

		// Try to find a file with .size as extension
		m_tileSize = 0;

		findHandle = FindFirstFile((char *) tileSpecifierSearchPath, &findData);

		if(findHandle != INVALID_HANDLE_VALUE)
		{
			// Remove extension and tokenize file name
			CRuString fileName(findData.cFileName);
			fileName.Replace(".size", "");

			CRuStringTokenizer tokenizer;
			tokenizer.Tokenize(fileName, "x");

			if(tokenizer.GetNumTokens() == 2)
			{
				m_tileSize = static_cast<REAL>(atoi(tokenizer.GetToken(0)));
				m_layerHeight = static_cast<REAL>(atoi(tokenizer.GetToken(1)));
			}
		}

		FindClose(findHandle);

		// Calculate actual tile size if necessary
		if(m_tileSize <= 0)
		{
			m_tileSize = CalculateTileSize();
		}

		// Initialize tile grid
		m_tileGrid = ruNEW CRuEntity_Widget_TileGrid();

		REAL halfTileMapExtents = m_mapSize * m_tileSize / 2.0f;
		CRuVector3 gridOrigin(-halfTileMapExtents, 0.0f, -halfTileMapExtents);
		m_tileGrid->Initialize(gridOrigin, m_mapSize, m_tileSize);

		m_origin = gridOrigin;

		this->AddChild(m_tileGrid);

		// Flag editor mode as enabled
		m_enableEditorMode = TRUE;

		// Release partitioning structure
		ruSAFE_RELEASE(m_tilePartition);

		return TRUE;
	}

	return FALSE;
}

CRuArrayList<CRuEntity *> &CRuEntity_TileMap::GetCustomTiles()
{
	return m_customTiles;
}

INT32 CRuEntity_TileMap::GetMapDepth()
{
	return m_mapDepth;
}

BOOL CRuEntity_TileMap::PickClosestPlacementPoint(const CRuSegment3 &segment, CRuVector3 &pointOut)
{
	if(m_tileGrid)
	{
		// Perform pick test
		INT32 x, z;

		if(m_tileGrid->PickTileCoordinatesAt(segment, x, z))
		{
			REAL halfTileSize = m_tileSize / 2.0f;

			pointOut.m_x = m_origin.m_x + x * m_tileSize + halfTileSize;
			pointOut.m_y = m_currentDepth * m_layerHeight;
			pointOut.m_z = m_origin.m_z + z * m_tileSize + halfTileSize;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuEntity_TileMap::ClearTempExclusion()
{
	// Mark previous exclusion zone dirty
	for(INT32 k = m_tempExclusion_MinY; k <= m_tempExclusion_MaxY; ++k)
	{
		for(INT32 j = m_tempExclusion_MinZ; j <= m_tempExclusion_MaxZ; ++j)
		{
			for(INT32 i = m_tempExclusion_MinX; i <= m_tempExclusion_MaxX; ++i)
			{
				if(i >= 0 && i < m_mapSize && j >= 0 && j < m_mapSize && k >= 0 && k < m_mapDepth)
					m_dirtyTiles.Push((k << 24) | (i << 12) | j);
			}
		}
	}

	// Clear exclusion parameters
	ruSAFE_RELEASE(m_tempExclusion_Entity);
	m_tempExclusion_MinX = 0;
	m_tempExclusion_MinY = 0;
	m_tempExclusion_MinZ = 0;
	m_tempExclusion_MaxX = -1;
	m_tempExclusion_MaxY = -1;
	m_tempExclusion_MaxZ = -1;

	return TRUE;
}

BOOL CRuEntity_TileMap::SetTempExclusionByObject(CRuEntity *entity, const CRuSegment3 &segment)
{
	if(entity)
	{
		// Mark previous exclusion zone dirty
		for(INT32 k = m_tempExclusion_MinY; k <= m_tempExclusion_MaxY; ++k)
		{
			for(INT32 j = m_tempExclusion_MinZ; j <= m_tempExclusion_MaxZ; ++j)
			{
				for(INT32 i = m_tempExclusion_MinX; i <= m_tempExclusion_MaxX; ++i)
				{
					if(i >= 0 && i < m_mapSize && j >= 0 && j < m_mapSize && k >= 0 && k < m_mapDepth)
						m_dirtyTiles.Push((k << 24) | (i << 12) | j);
				}
			}
		}

		if(m_tempExclusion_Entity == NULL || entity->GetGUID() != m_tempExclusion_Entity->GetGUID())
		{
			// Store reference to the exclusion entity
			ruSAFE_RELEASE(m_tempExclusion_Entity);
			m_tempExclusion_Entity = entity;
			m_tempExclusion_Entity->AddRef();

			// Attempt to parse tile bounds from the entity resource name
			m_tempExclusion_TileBounds = ParseTileBounds(m_tempExclusion_Entity);
		}

		CRuAABB gridVol = CalculateGridVolume(m_tempExclusion_TileBounds, segment);

		// Update exclusion bounds
		m_tempExclusion_MinX = static_cast<INT32>(gridVol.Minima().m_x);
		m_tempExclusion_MinY = static_cast<INT32>(gridVol.Minima().m_y);
		m_tempExclusion_MinZ = static_cast<INT32>(gridVol.Minima().m_z);
		m_tempExclusion_MaxX = static_cast<INT32>(gridVol.Maxima().m_x);
		m_tempExclusion_MaxY = static_cast<INT32>(gridVol.Maxima().m_y);
		m_tempExclusion_MaxZ = static_cast<INT32>(gridVol.Maxima().m_z);

		// Mark new exclusion zone dirty
		for(INT32 k = m_tempExclusion_MinY; k <= m_tempExclusion_MaxY; ++k)
		{
			for(INT32 j = m_tempExclusion_MinZ; j <= m_tempExclusion_MaxZ; ++j)
			{
				for(INT32 i = m_tempExclusion_MinX; i <= m_tempExclusion_MaxX; ++i)
				{
					if(i >= 0 && i < m_mapSize && j >= 0 && j < m_mapSize && k >= 0 && k < m_mapDepth)
						m_dirtyTiles.Push((k << 24) | (i << 12) | j);
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_TileMap::UpdateTempExclusionTileBounds()
{
	if(m_tempExclusion_Entity)
	{
		m_tempExclusion_TileBounds = ParseTileBounds(m_tempExclusion_Entity);
		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_TileMap::PlaceCustomTile()
{
	if(m_tempExclusion_Entity)
	{
		// Check exclusion zone to make sure it does not overlap any existing custom tiles
		for(INT32 k = m_tempExclusion_MinY; k <= m_tempExclusion_MaxY; ++k)
		{
			for(INT32 j = m_tempExclusion_MinZ; j <= m_tempExclusion_MaxZ; ++j)
			{
				for(INT32 i = m_tempExclusion_MinX; i <= m_tempExclusion_MaxX; ++i)
				{
					if(i >= 0 && i < m_mapSize && j >= 0 && j < m_mapSize && k >= 0 && k < m_mapDepth)
					{
						if(m_exclusionMap[k][i + j * m_mapSize])
						{
							return FALSE;
						}
					}
				}
			}
		}

		for(INT32 k = m_tempExclusion_MinY; k <= m_tempExclusion_MaxY; ++k)
		{
			for(INT32 j = m_tempExclusion_MinZ; j <= m_tempExclusion_MaxZ; ++j)
			{
				for(INT32 i = m_tempExclusion_MinX; i <= m_tempExclusion_MaxX; ++i)
				{
					if(i >= 0 && i < m_mapSize && j >= 0 && j < m_mapSize && k >= 0 && k < m_mapDepth)
					{
						// Mark spot on exclusion map
						m_exclusionMap[k].Set(i + j * m_mapSize);

						// Mark tile dirty
						m_dirtyTiles.Push((k << 24) | (i << 12) | j);
					}
				}
			}
		}

		// Store custom tile
		TileDescriptor *tileDesc = ruNEW TileDescriptor();

		INT32 resNameLen = (INT32)strlen(m_tempExclusion_Entity->GetResourceName()) + 1;
		tileDesc->m_tileResourceName = ruNEW char [resNameLen];
		memcpy(tileDesc->m_tileResourceName, m_tempExclusion_Entity->GetResourceName(), resNameLen);
		tileDesc->m_volume_MinX = m_tempExclusion_MinX;
		tileDesc->m_volume_MinY = m_tempExclusion_MinY;
		tileDesc->m_volume_MinZ = m_tempExclusion_MinZ;
		tileDesc->m_volume_MaxX = m_tempExclusion_MaxX;
		tileDesc->m_volume_MaxY = m_tempExclusion_MaxY;
		tileDesc->m_volume_MaxZ = m_tempExclusion_MaxZ;
		tileDesc->m_tileEntity = static_cast<CRuEntity *>(m_tempExclusion_Entity->Clone());

		m_placedCustomTiles.Add(tileDesc);

		// Add custom tile to list of children
		this->AddChild(tileDesc->m_tileEntity);

		// Disable cloning and serialization on the tile
		tileDesc->m_tileEntity->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
		tileDesc->m_tileEntity->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);
	}

	return FALSE;
}

BOOL CRuEntity_TileMap::RemoveCustomTile(const CRuSegment3 &segment)
{
	INT32 nearestCustomTileIdx = -1;
	REAL nearestCustomTileDist = FLT_MAX;

	for(INT32 i = 0; i < m_placedCustomTiles.Count(); ++i)
	{
		CRuCollisionQuery_Segment_Nearest segQuery;
		segQuery.m_colSegment = segment;

		m_placedCustomTiles[i]->m_tileEntity->QueryCollision(&segQuery);

		if(segQuery.m_positiveHit && segQuery.m_collisionT < nearestCustomTileDist)
		{
			nearestCustomTileIdx = i;
			nearestCustomTileDist = segQuery.m_collisionT;
		}
	}

	if(nearestCustomTileIdx >= 0)
	{
		TileDescriptor *tileDesc = m_placedCustomTiles[nearestCustomTileIdx];

		// Clear exclusion area occupied by the deleted custom tile
		for(INT32 k = tileDesc->m_volume_MinY; k <= tileDesc->m_volume_MaxY; ++k)
		{
			for(INT32 j = tileDesc->m_volume_MinZ; j <= tileDesc->m_volume_MaxZ; ++j)
			{
				for(INT32 i = tileDesc->m_volume_MinX; i <= tileDesc->m_volume_MaxX; ++i)
				{
					if(i >= 0 && i < m_mapSize && j >= 0 && j < m_mapSize && k >= 0 && k < m_mapDepth)
					{
						// CLear exclusion bit
						m_exclusionMap[k].Clear(i + j * m_mapSize);

						// Mark tile dirty
						m_dirtyTiles.Push((k << 24) | (i << 12) | j);
					}
				}
			}
		}

		// Remove custom tile from scene graph
		tileDesc->m_tileEntity->DetachFromParent();

		// Remove custom tile entry
		m_placedCustomTiles.RemoveAt(nearestCustomTileIdx);

		// Delete custom tile descriptor
		delete tileDesc;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_TileMap::SetCurrentDepth(INT32 depth)
{
	// Store current depth
	m_currentDepth = depth;

	// Reinitialize grid widget
	REAL halfTileMapExtents = m_mapSize * m_tileSize / 2.0f;
	CRuVector3 gridOrigin(-halfTileMapExtents, 0.0f, -halfTileMapExtents);
	m_tileGrid->Reinitialize(CRuVector3(gridOrigin.m_x, m_currentDepth * m_layerHeight, gridOrigin.m_z), m_tileStateMap[m_currentDepth]);

	return TRUE;
}

BOOL CRuEntity_TileMap::SetDefaultTileID(INT32 tileID)
{
	m_defaultTileID = tileID;
	return TRUE;
}

BOOL CRuEntity_TileMap::SetTileSelectMode(CRuEntity_TileMap::TileSelectMode tileSelectMode)
{
	m_tileSelectMode = tileSelectMode;
	return TRUE;
}

void CRuEntity_TileMap::ToggleTileAt(const CRuSegment3 &segment, BOOL toggleState)
{
	if(m_tileGrid)
	{
		// Perform pick test
		INT32 x, z;

		if(m_tileGrid->PickTileCoordinatesAt(segment, x, z))
		{
			// Determine new tile bit status
			BOOL newBitStatus = toggleState;

			// Update tile state map
			if(newBitStatus)
				m_tileStateMap[m_currentDepth].Set(x + z * m_mapSize);
			else
				m_tileStateMap[m_currentDepth].Clear(x + z * m_mapSize);

			// Update tile grid display
			m_tileGrid->SetTileBit(x, z, newBitStatus);

			// Push all surrounding tiles into dirty list
			INT32 x0 = max(0, x - 1);
			INT32 z0 = max(0, z - 1);
			INT32 x1 = min(m_mapSize - 1, x + 1);
			INT32 z1 = min(m_mapSize - 1, z + 1);

			for(INT32 i = x0; i <= x1; ++i)
			{
				for(INT32 j = z0; j <= z1; ++j)
				{
					m_dirtyTiles.Push((m_currentDepth << 24) | (i << 12) | j);
				}
			}
		}
	}
}

void CRuEntity_TileMap::ReselectTileAt(const CRuSegment3 &segment)
{
	if(m_tileGrid)
	{
		// Perform pick test
		INT32 x, z;

		if(m_tileGrid->PickTileCoordinatesAt(segment, x, z))
		{
			m_dirtyTiles.Push((m_currentDepth << 24) | (x << 12) | z);
		}
	}
}

IRuObject *CRuEntity_TileMap::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuEntity_TileMap();

	// Chain cloning call to parent class
	CRuEntity::Clone(clonedObject);

	// Shortcut
	CRuEntity_TileMap *clonedTileMap = static_cast<CRuEntity_TileMap *>(clonedObject);

	// Initialize cloned tile map
	clonedTileMap->Initialize(m_mapSize, m_mapDepth, m_layerHeight);

	clonedTileMap->m_tileSize = m_tileSize;

	clonedTileMap->m_origin = m_origin;

	clonedTileMap->SetRootPath(m_rootPath);
	clonedTileMap->SetTilePath(m_tilePath);

	// Clone tile hash
	for(INT32 i = 0; i < m_mapDepth; ++i)
	{
		m_tileHash[i]->ForAllElements(boost::bind(&CRuEntity_TileMap::CloneTileHashCallback, clonedTileMap, _1, _2));
	}

	// Clone custom tile list
	for(INT32 i = 0; i < m_placedCustomTiles.Count(); ++i)
	{
		// Copy custom tile descriptor
		TileDescriptor *tileDesc = ruNEW TileDescriptor();
		*tileDesc = *(m_placedCustomTiles[i]);

		// Add tile descriptor to list of custom tile descriptors
		clonedTileMap->m_placedCustomTiles.Add(tileDesc);

		// Add custom tile to list of children
		clonedTileMap->AddChild(tileDesc->m_tileEntity);

		// Disable cloning and serialization on the tile
		tileDesc->m_tileEntity->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
		tileDesc->m_tileEntity->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);
	}

	// Copy tile state and exclusion maps
	for(INT32 i = 0; i < m_mapDepth; ++i)
	{
		clonedTileMap->m_tileStateMap[i] = m_tileStateMap[i];
		clonedTileMap->m_exclusionMap[i] = m_exclusionMap[i];
	}

	// Initialize partitioning structure
	clonedTileMap->InitializePartitioningStructure();

	return clonedObject;
}

BOOL CRuEntity_TileMap::SerializeFrom(IRuStream *stream)
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

	// Chain serialization to base class
	CRuEntity::SerializeFrom(stream);

	// Read map properties
	stream->Read(&m_mapSize, sizeof(INT32));
	stream->Read(&m_mapDepth, sizeof(INT32));
	stream->Read(&m_layerHeight, sizeof(REAL));
	stream->Read(&m_tileSize, sizeof(REAL));
	::SerializeFrom(stream, &m_origin);
	::SerializeFrom(stream, &m_rootPath);
	::SerializeFrom(stream, &m_tilePath);

	// Initialize tile map storage
	Initialize(m_mapSize, m_mapDepth, m_layerHeight);

	// Read auto-tiles
	for(INT32 i = 0; i < m_mapDepth; ++i)
	{
		// Read number of auto-tiles
		INT32 numAutoTiles = 0;
		stream->Read(&numAutoTiles, sizeof(INT32));

		for(INT32 j = 0; j < numAutoTiles; ++j)
		{
			// Serialize tile descriptor from stream
			TileDescriptor *tileDesc = ruNEW TileDescriptor();
			tileDesc->SerializeFrom(stream);

			// Add tile descriptor to tile hash
			INT32 tileCoord = tileDesc->m_tileCoord;
			DWORD tileCoordHash = RuGlobalHashGen().GetHashCode(tileCoord);

			m_tileHash[i]->Add(tileCoord, tileCoordHash, tileDesc);

			if(tileDesc->m_tileEntity)
			{
				// Add auto-tile to list of children
				this->AddChild(tileDesc->m_tileEntity);

				// Disable cloning and serialization on the tile
				tileDesc->m_tileEntity->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
				tileDesc->m_tileEntity->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);
			}
		}
	}

	// Read custom tiles
	INT32 numCustomTiles = 0;
	stream->Read(&numCustomTiles, sizeof(INT32));

	for(INT32 i = 0; i < numCustomTiles; ++i)
	{
		// Serialize tile descriptor from stream
		TileDescriptor *tileDesc = ruNEW TileDescriptor();
		tileDesc->SerializeFrom(stream);

		// Add custom tile to custom tile list
		m_placedCustomTiles.Add(tileDesc);

		if(tileDesc->m_tileEntity)
		{
			// Add custom tile to list of children
			this->AddChild(tileDesc->m_tileEntity);

			// Disable cloning and serialization on the tile
			tileDesc->m_tileEntity->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
			tileDesc->m_tileEntity->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);
		}
	}

	// Read tile state and exclusion maps
	for(INT32 i = 0; i < m_mapDepth; ++i)
	{
		::SerializeFrom(stream, &m_tileStateMap[i]);
		::SerializeFrom(stream, &m_exclusionMap[i]);
	}

	// Initialize partitioning structure
	InitializePartitioningStructure();

	return TRUE;
}

BOOL CRuEntity_TileMap::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuEntity::SerializeTo(stream);

	// Write map properties
	stream->Write(&m_mapSize, sizeof(INT32));
	stream->Write(&m_mapDepth, sizeof(INT32));
	stream->Write(&m_layerHeight, sizeof(REAL));
	stream->Write(&m_tileSize, sizeof(REAL));
	::SerializeTo(stream, &m_origin);
	::SerializeTo(stream, &m_rootPath);
	::SerializeTo(stream, &m_tilePath);

	// Write tile hash
	m_outputStream = stream;
	for(INT32 i = 0; i < m_mapDepth; ++i)
	{
		INT32 numAutoTiles = m_tileHash[i]->Count();
		stream->Write(&numAutoTiles, sizeof(INT32));

		m_tileHash[i]->ForAllElements(boost::bind(&CRuEntity_TileMap::SerializeTileHashCallback, this, _1, _2));
	}

	// Write custom tiles
	INT32 numCustomTiles = m_placedCustomTiles.Count();
	stream->Write(&numCustomTiles, sizeof(INT32));

	for(INT32 i = 0; i < m_placedCustomTiles.Count(); ++i)
	{
		m_placedCustomTiles[i]->SerializeTo(stream);
	}

	// Write tile state and exclusion maps
	for(INT32 i = 0; i < m_mapDepth; ++i)
	{
		::SerializeTo(stream, &m_tileStateMap[i]);
		::SerializeTo(stream, &m_exclusionMap[i]);
	}

	return TRUE;
}

BOOL CRuEntity_TileMap::Update(REAL elapsedTime)
{
	// Chain update to base class
	CRuEntity::Update(elapsedTime);

	while(m_dirtyTiles.Count() > 0)
	{
		INT32 tileCoord = m_dirtyTiles.Pop();
		DWORD tileCoordHash = RuGlobalHashGen().GetHashCode(tileCoord);
		INT32 depth = (tileCoord >> 24) & 0xFF;
		INT32 x = (tileCoord >> 12) & 0xFFF;
		INT32 z = tileCoord & 0xFFF;

		// Determine the tile type
		INT32 tileID = 0;

		UINT8 borderBits = 0;

		if(!IsExcluded(x, depth, z))
		{
			if(!m_tileStateMap[depth][x + z * m_mapSize])
			{
				// Set bits
				for(INT32 j = z - 1, k = 0; j <= z + 1; ++j)
				{
					for(INT32 i = x - 1; i <= x + 1; ++i)
					{
						// Skip the center tile
						if(i == x && j == z)
							continue;

						if(i >= 0 && i < m_mapSize && j >= 0 && j < m_mapSize)
						{
							if(m_tileStateMap[depth][i + j * m_mapSize])
							{
								borderBits = borderBits | (0x01 << k);
							}
						}

						++k;
					}
				}

				// A bit of a hack... need a more elegant solution
				if((borderBits & 0x0A) == 0x0A)
				{
					tileID = 14;
				}
				else if((borderBits & 0x12) == 0x12)
				{
					tileID = 13;
				}
				else if((borderBits & 0x50) == 0x50)
				{
					tileID = 12;
				}
				else if((borderBits & 0x48) == 0x48)
				{
					tileID = 11;
				}
				else if((borderBits & 0x40) == 0x40)
				{
					tileID = 7;
				}
				else if((borderBits & 0x08) == 0x08)
				{
					tileID = 6;
				}
				else if((borderBits & 0x02) == 0x02)
				{
					tileID = 5;
				}
				else if((borderBits & 0x10) == 0x10)
				{
					tileID = 8;
				}
				else if((borderBits & 0x81) == 0x81)
				{
					tileID = 10;
				}
				else if((borderBits & 0x24) == 0x24)
				{
					tileID = 9;
				}
				else if((borderBits & 0x80) == 0x80)
				{
					tileID = 4;
				}
				else if((borderBits & 0x20) == 0x20)
				{
					tileID = 3;
				}
				else if((borderBits & 0x01) == 0x01)
				{
					tileID = 2;
				}
				else if((borderBits & 0x04) == 0x04)
				{
					tileID = 1;
				}
				else
				{
					tileID = -1;
				}
			}
		}
		else
		{
			tileID = -1;
		}

		TileDescriptor *tileDesc = NULL;

		// Check tile hash to see if a tile already exists at the current coordinates
		if(m_tileHash[depth]->ContainsKey(tileCoord, tileCoordHash))
		{
			tileDesc = m_tileHash[depth]->Get(tileCoord, tileCoordHash);;

			// Detach and destroy existing tile entity
			tileDesc->m_tileEntity->DetachFromParent();
			ruSAFE_RELEASE(tileDesc->m_tileEntity);

			if(tileID < 0)
			{
				// Destroy tile descriptor
				delete tileDesc;

				// Remove tile descriptor from hash
				m_tileHash[depth]->Remove(tileCoord, tileCoordHash);
			}
		}

		if(tileID >= 0)
		{
			// If there is no existing tile descriptor at this position, create one now
			if(tileDesc == NULL)
			{
				tileDesc = ruNEW TileDescriptor();
				tileDesc->m_tileCoord = tileCoord;

				m_tileHash[depth]->Add(tileCoord, tileCoordHash, tileDesc);
			}

			// Select index of tile to be used based on current selection mode
			INT32 variationIdx = 0;

			switch(m_tileSelectMode)
			{
				case ruTILESELECTMODE_SINGLE:
					variationIdx = m_defaultTileID;
					break;

				case ruTILESELECTMODE_SEQUENTIAL:
					{
						if(tileDesc->m_tileResourceName)
						{
							INT32 currentVariationIdx = -1;
							for(INT32 i = 0; i < m_systemTiles[tileID].Count(); ++i)
							{
								if(stricmp(m_systemTiles[tileID][i]->GetResourceName(), tileDesc->m_tileResourceName) == 0)
								{
									currentVariationIdx = i;
									break;
								}
							}

							variationIdx = currentVariationIdx + 1;
						}
					}
					break;

				case ruTILESELECTMODE_RANDOM:
					variationIdx = RuGlobalMTRand().GenerateINT32();
					break;
			}

			variationIdx = variationIdx  % m_systemTiles[tileID].Count();

			CRuEntity *sourceTile = m_systemTiles[tileID][variationIdx];

			// Save resource name of the newly selected tile
			delete[] tileDesc->m_tileResourceName;
			INT32 resNameLen = (INT32)strlen(sourceTile->GetResourceName()) + 1;
			tileDesc->m_tileResourceName = ruNEW char [resNameLen];
			memcpy(tileDesc->m_tileResourceName, sourceTile->GetResourceName(), resNameLen);

			// Create a clone of the template tile and position it
			tileDesc->m_tileEntity = static_cast<CRuEntity *>(sourceTile->Clone());

			REAL halfTileSize = m_tileSize / 2.0f;
			CRuVector3 offset(m_origin.m_x + x * m_tileSize + halfTileSize, depth * m_layerHeight, m_origin.m_z + z * m_tileSize + halfTileSize);

			tileDesc->m_tileEntity->SetTranslation(offset);

			// Add auto-tile to list of children
			this->AddChild(tileDesc->m_tileEntity);

			// Disable cloning and serialization on the tile
			tileDesc->m_tileEntity->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
			tileDesc->m_tileEntity->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);
		}
	}

	return TRUE;
}

BOOL CRuEntity_TileMap::QueryCollision(CRuCollisionQuery *colQuery)
{
	if(m_tilePartition)
	{
		// If collision query is disabled, return immediately
		if(colQuery->m_ignoreCollisionFlag == FALSE && GetOption(ruENTITYOPTION_COLLISION) == 0)
			return TRUE;

//		m_tilePartition->SetPosition(this->GetTranslation(ruFRAMETYPE_WORLD));

		// Save previous collision transform
		CRuMatrix4x4 *originalWorldTransform = colQuery->m_worldTransform;
		CRuMatrix4x4 *originalInvWorldTransform = colQuery->m_invWorldTransform;

		// Store the inverse of the current world transform into the collision query
		colQuery->m_worldTransform = const_cast<CRuMatrix4x4 *>(&this->GetWorldTransform());
		colQuery->m_invWorldTransform = const_cast<CRuMatrix4x4 *>(&this->GetInverseWorldTransform());

		BOOL retVal = TRUE;

		// Enumerate tiles using the partition structure
		m_temp_Objects.Clear();

		switch(colQuery->m_queryType)
		{
			case ruCOLQUERY_RAY:
			case ruCOLQUERY_RAY_HITTEST:
			case ruCOLQUERY_RAY_NEAREST:
			case ruCOLQUERY_RAY_PICK:
			case ruCOLQUERY_RAY_TRIPICK:
			case ruCOLQUERY_RAY_SUBOBJECT:
				{
					// Get the ray in local coordinates
					CRuCollisionQuery_Ray_Base *query = static_cast<CRuCollisionQuery_Ray_Base *>(colQuery);
					CRuRay3 oldRay(query->m_colRay);
					query->m_invWorldTransform->TransformPoint(query->m_colRay.Origin());
					query->m_invWorldTransform->TransformVector(query->m_colRay.Direction());

					// Perform enumeration
					m_tilePartition->EnumerateObjects(query->m_colRay, m_temp_Objects);

					// Restore old collision ray
					query->m_colRay = oldRay;
				}

				break;

			case ruCOLQUERY_SEGMENT:
			case ruCOLQUERY_SEGMENT_HITTEST:
			case ruCOLQUERY_SEGMENT_NEAREST:
			case ruCOLQUERY_SEGMENT_SUBOBJECT:
				{
					// Get the segment in local coordinates
					CRuCollisionQuery_Segment_Base *query = static_cast<CRuCollisionQuery_Segment_Base *>(colQuery);
					CRuSegment3 oldSegment(query->m_colSegment);
					query->m_invWorldTransform->TransformPoint(query->m_colSegment.Origin());
					query->m_invWorldTransform->TransformVector(query->m_colSegment.Direction());

					// Perform enumeration
					m_tilePartition->EnumerateObjects(query->m_colSegment, m_temp_Objects);

					// Restore old collision segment
					query->m_colSegment = oldSegment;
				}

				break;

			case ruCOLQUERY_OBB_HITTEST:
			case ruCOLQUERY_OBB_TRIENUM:
				{
					// Get the collision OBB in local coordinates
					CRuCollisionQuery_OBB_HitTest *query = static_cast<CRuCollisionQuery_OBB_HitTest *>(colQuery);
					CRuOBB oldBox(query->m_colOBB);
					query->m_colOBB.Transform(*query->m_invWorldTransform);

					// Perform enumeration
					m_tilePartition->EnumerateObjects(query->m_colOBB, m_temp_Objects);

					// Restore old box
					query->m_colOBB = oldBox;
				}

				break;

			case ruCOLQUERY_SPHERE_HITTEST:
			case ruCOLQUERY_SPHERE_TRIENUM:
				{
					// Get the sphere in local coordinates
					CRuCollisionQuery_Sphere_Base *query = static_cast<CRuCollisionQuery_Sphere_Base *>(colQuery);
					CRuSphere oldSphere(query->m_colSphere);
					query->m_invWorldTransform->TransformPoint(query->m_colSphere.Center());

					CRuVector3 extents(1.0f, 1.0f, 1.0f);
					query->m_invWorldTransform->TransformVector(extents);
					query->m_colSphere.Radius() = max(max(fabs(extents.x), fabs(extents.y)), fabs(extents.z)) * query->m_colSphere.Radius();

					// Perform enumeration
					m_tilePartition->EnumerateObjects(query->m_colSphere, m_temp_Objects);

					// Restore old OBB
					query->m_colSphere = oldSphere;
				}

				break;
		}

		for(INT32 i = 0; i < m_temp_Objects.Count(); ++i)
		{
			if(static_cast<CRuEntity *>(m_temp_Objects[i])->QueryCollision(colQuery) == FALSE)
			{
				retVal = FALSE;
				break;
			}
		}

		// Restore original collision transform
		colQuery->m_worldTransform = originalWorldTransform;
		colQuery->m_invWorldTransform = originalInvWorldTransform;

		return retVal;
	}

	return CRuEntity::QueryCollision(colQuery);
}

REAL CRuEntity_TileMap::PadValueToMultiple(REAL value, REAL divisor)
{
	REAL epsilon = 1.0f;

	if(value >= 0.0f)
	{
		value = ceil((value - epsilon) / divisor) * divisor;
	}
	else
	{
		value = floor((value + epsilon) / divisor) * divisor;
	}

	return value;
}

REAL CRuEntity_TileMap::CalculateTileSize()
{
	REAL tileSize = 0.0f;

	if(m_systemTiles[0].Count() > 0)
	{
		CRuEntity *tileEntity = m_systemTiles[0][0];
		CRuModel *model = RuEntity_FindFirstModel(tileEntity);

		if(model)
		{
			for(INT32 i = 0; i < model->GetNumMeshes(); ++i)
			{
				IRuMesh *mesh = NULL;
				IRuMaterial *material = NULL;

				if(model->GetMesh(i, &mesh, &material))
				{
					IRuMorphTarget *mt = mesh->GetMorphTarget(0);
					if(mt)
					{
						const CRuVector3 *position = mt->GetPosition();

						for(INT32 j = 0; j < mesh->GetNumVertices(); ++j)
						{
							tileSize = max(max(fabs(position[j].m_x), fabs(position[j].m_z)), tileSize);
						}
					}
				}
			}
		}
	}

	return tileSize * 2.0f;
}

CRuAABB CRuEntity_TileMap::CalculateTileBounds(CRuEntity *tileEntity)
{
	// Enumerate all meshes and their world transforms in the tile entity
	CRuArrayList<IRuMesh *> meshList;
	CRuArrayList<CRuMatrix4x4> worldTransformList;
	RuEntity_EnumerateMeshes(tileEntity, meshList, &worldTransformList);

	// Find min and max X/Z coordinates
	CRuVector3 minPoint(FLT_MAX, FLT_MAX, FLT_MAX), maxPoint(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for(INT32 i = 0; i < meshList.Count(); ++i)
	{
		IRuMorphTarget *mt = meshList[i]->GetMorphTarget(0);
		if(mt)
		{
			const CRuVector3 *position = mt->GetPosition();

			if(position)
			{
				for(INT32 j = 0; j < meshList[i]->GetNumVertices(); ++j)
				{
					// Perform rotation and scale on the position (throw away the translation)
					CRuVector3 worldPt;
					worldTransformList[i].TransformVector(position[j], worldPt);

					// Merge position into bounding box
					minPoint.m_x = min(worldPt.m_x, minPoint.m_x);
					minPoint.m_y = min(worldPt.m_y, minPoint.m_y);
					minPoint.m_z = min(worldPt.m_z, minPoint.m_z);

					maxPoint.m_x = max(worldPt.m_x, maxPoint.m_x);
					maxPoint.m_y = max(worldPt.m_y, maxPoint.m_y);
					maxPoint.m_z = max(worldPt.m_z, maxPoint.m_z);
				}
			}
		}
	}

	// Now shrink extents if necessary
	REAL halfTileSize = m_tileSize / 2.0f;

	for(INT32 i = 0; i < 3; ++i)
	{
		if(fabs(fmod(minPoint[i], halfTileSize)) < halfTileSize)
		{
			minPoint[i] += halfTileSize;
		}

		if(fabs(fmod(maxPoint[i], halfTileSize)) < halfTileSize)
		{
			maxPoint[i] -= halfTileSize;
		}
	}

	return CRuAABB(minPoint, maxPoint);
}

CRuAABB CRuEntity_TileMap::ParseTileBounds(CRuEntity *tileEntity)
{
	// Get bounds from mesh
	CRuAABB bounds = CalculateTileBounds(tileEntity);

	// Parse tile entity resource name and adjust bounds if necessary
	const char *resourceName = tileEntity->GetResourceName();

	if(resourceName)
	{
		// Tokenize the resource name into tokens separated by the underscore character
		CRuStringTokenizer tokenizer;
		tokenizer.Tokenize(resourceName, "_");

		// Find a string of the form [n]x[m]
		for(INT32 i = 0; i < tokenizer.GetNumTokens(); ++i)
		{
			CRuString &token = tokenizer.GetToken(i);

			BOOL isValidFormat = TRUE;
			BOOL onSecondNumber = FALSE;

			for(INT32 j = 0; j < token.GetLength(); ++j)
			{
				if(token[j] < 48 || token[j] > 57)
				{
					if(onSecondNumber == FALSE)
					{
						if(tolower(token[j]) == 'x')
						{
							onSecondNumber = TRUE;
						}
						else
						{
							isValidFormat = FALSE;
							break;
						}
					}
					else
					{
						isValidFormat = FALSE;
						break;
					}
				}
			}

			if(isValidFormat)
			{
				CRuStringTokenizer dimStrTokenizer;
				dimStrTokenizer.Tokenize(token, "xX");

				if(dimStrTokenizer.GetNumTokens() == 2)
				{
					INT32 xTiles = atoi(dimStrTokenizer.GetToken(0));
					INT32 zTiles = atoi(dimStrTokenizer.GetToken(1));

					REAL xSize = xTiles * m_tileSize;
					REAL zSize = zTiles * m_tileSize;

					bounds.Minima().m_x = -xSize * 0.5f;
					bounds.Minima().m_z = -zSize * 0.5f;

					bounds.Maxima().m_x = xSize * 0.5f;
					bounds.Maxima().m_z = zSize * 0.5f;
				}

				break;
			}
		}
	}

	return bounds;
}

CRuAABB CRuEntity_TileMap::CalculateGridVolume(const CRuAABB &bounds, const CRuSegment3 &segment)
{
	CRuAABB gridVol(0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f);

	INT32 x, z;
	if(m_tileGrid->PickTileCoordinatesAt(segment, x, z))
	{
		gridVol = bounds;

		REAL halfTileSize = m_tileSize / 2.0f;

		gridVol.Minima().m_x += x * m_tileSize + halfTileSize;
		gridVol.Minima().m_y += m_currentDepth * m_layerHeight;
		gridVol.Minima().m_z += z * m_tileSize + halfTileSize;

		gridVol.Maxima().m_x += x * m_tileSize + halfTileSize;
		gridVol.Maxima().m_y += m_currentDepth * m_layerHeight;
		gridVol.Maxima().m_z += z * m_tileSize + halfTileSize;

		// Subtract by a constant value to prevent over-exclusion
		gridVol.Maxima().m_x -= 1.0f;
		gridVol.Maxima().m_y -= 1.0f;
		gridVol.Maxima().m_z -= 1.0f;

		// Convert into grid coordinates
		gridVol.Minima().m_x /= m_tileSize;
		gridVol.Minima().m_y /= m_layerHeight;
		gridVol.Minima().m_z /= m_tileSize;

		gridVol.Maxima().m_x /= m_tileSize;
		gridVol.Maxima().m_y /= m_layerHeight;
		gridVol.Maxima().m_z /= m_tileSize;
	}

	return gridVol;
}

BOOL CRuEntity_TileMap::IsExcluded(INT32 x, INT32 y, INT32 z)
{
	if(x >= m_tempExclusion_MinX && x <= m_tempExclusion_MaxX && y >= m_tempExclusion_MinY && y <= m_tempExclusion_MaxY && z >= m_tempExclusion_MinZ && z <= m_tempExclusion_MaxZ)
		return TRUE;

	if(m_exclusionMap[y][x + z * m_mapSize])
		return TRUE;

	return FALSE;
}

BOOL CRuEntity_TileMap::ClearTileHashCallback(const void *key, void *data)
{
	INT32 tileCoord = *reinterpret_cast<const INT32 *>(key);
	DWORD tileCoordHash = RuGlobalHashGen().GetHashCode(tileCoord);
	TileDescriptor *tileDesc = *reinterpret_cast<TileDescriptor **>(data);

	delete tileDesc;

	return TRUE;
}

BOOL CRuEntity_TileMap::CloneTileHashCallback(const void *key, void *data)
{
	INT32 tileCoord = *reinterpret_cast<const INT32 *>(key);
	DWORD tileCoordHash = RuGlobalHashGen().GetHashCode(tileCoord);
	TileDescriptor *srcTileDesc = *reinterpret_cast<TileDescriptor **>(data);

	// Copy tile descriptor
	TileDescriptor *tileDesc = ruNEW TileDescriptor();
	*tileDesc = *srcTileDesc;

	// Add tile descriptor to tile hash
	INT32 depth = (tileCoord >> 24) & 0xFF;
	INT32 x = (tileCoord >> 12) & 0xFFF;
	INT32 z = tileCoord & 0xFFF;

	m_tileHash[depth]->Add(tileCoord, tileCoordHash, tileDesc);

	// Add auto-tile to list of children
	this->AddChild(tileDesc->m_tileEntity);

	// Disable cloning and serialization on the tile
	tileDesc->m_tileEntity->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
	tileDesc->m_tileEntity->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);

	return TRUE;
}

BOOL CRuEntity_TileMap::SerializeTileHashCallback(const void *key, void *data)
{
	INT32 tileCoord = *reinterpret_cast<const INT32 *>(key);
	DWORD tileCoordHash = RuGlobalHashGen().GetHashCode(tileCoord);
	TileDescriptor *tileDesc = *reinterpret_cast<TileDescriptor **>(data);

	tileDesc->SerializeTo(m_outputStream);

	return TRUE;
}

void CRuEntity_TileMap::InitializePartitioningStructure()
{
	// Initialize partition structure
	CRuVector3 positionOffset = this->GetTranslation(ruFRAMETYPE_WORLD);
	CRuSphere worldBounds = this->GetWorldBounds();
	worldBounds.Center().Set(0.0f, 0.0f, 0.0f);

	CRuAABB partitionBounds;
	partitionBounds.SetBounds(CRuVector3(-worldBounds.Radius(), -worldBounds.Radius(), -worldBounds.Radius()), CRuVector3(worldBounds.Radius(), worldBounds.Radius(), worldBounds.Radius()));

	ruSAFE_RELEASE(m_tilePartition);
	m_tilePartition = ruNEW CRuPartition_QuadTree();
	m_tilePartition->Initialize(partitionBounds, 32);

	// Insert all tiles into partition structure
	CRuEntity *curChild = this->GetFirstChild();
	while(curChild)
	{
		// Get child bounds and adjust it by the position offset
		CRuSphere childBounds = curChild->GetWorldBounds();
		childBounds.Center() = childBounds.Center() - positionOffset;

		// Insert into partition structure
		m_tilePartition->InsertObject(curChild, childBounds);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
