/*!
	@file RuEntity_TileMap.h

	Copyright (c) 2004-2007 All rights reserved
*/

#ifndef _RUENTITY_TILEMAP_H_
#define _RUENTITY_TILEMAP_H_

#include "../../Partition/RuPartition_QuadTree.h"
#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Widget/RuEntity_Widget_TileGrid.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Tile map entity.

	@version 2007.07.26
*/
class CRuEntity_TileMap : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	enum TileSelectMode
	{
		ruTILESELECTMODE_SINGLE,
		ruTILESELECTMODE_SEQUENTIAL,
		ruTILESELECTMODE_RANDOM
	};

protected:
	struct TileDescriptor
	{
	public:
		// Tile settings
		INT32								m_tileCoord;								//!< Tile coordinates (used only for auto-tiles)
		char*								m_tileResourceName;							//!< Tile resource name

		INT32								m_volume_MinX;
		INT32								m_volume_MinY;
		INT32								m_volume_MinZ;
		INT32								m_volume_MaxX;
		INT32								m_volume_MaxY;
		INT32								m_volume_MaxZ;

		// Runtime
		CRuEntity*							m_tileEntity;								//!< Runtime tile entity

	public:
											TileDescriptor();
											~TileDescriptor();

		inline BOOL							SerializeFrom(IRuStream *stream)
											{
												// Read tile key
												stream->Read(&m_tileCoord, sizeof(INT32));

												// Read tile resource name
												::SerializeFrom(stream, &m_tileResourceName);

												// Read occupied volume
												stream->Read(&m_volume_MinX, sizeof(INT32));
												stream->Read(&m_volume_MinY, sizeof(INT32));
												stream->Read(&m_volume_MinZ, sizeof(INT32));
												stream->Read(&m_volume_MaxX, sizeof(INT32));
												stream->Read(&m_volume_MaxY, sizeof(INT32));
												stream->Read(&m_volume_MaxZ, sizeof(INT32));

												// Read entity transform
												REAL scale = 1.0f;
												CRuQuaternion rotation(0.0f, 0.0f, 0.0f, 1.0f);
												CRuVector3 position(0.0f, 0.0f, 0.0f);

												stream->Read(&scale, sizeof(REAL));
												::SerializeFrom(stream, &rotation);
												::SerializeFrom(stream, &position);

												// Instantiate entity
												m_tileEntity = g_ruResourceManager->LoadEntity(m_tileResourceName);

												if(m_tileEntity)
												{
													// Set entity transform
													m_tileEntity->Scale(scale);
													m_tileEntity->Rotate(rotation);
													m_tileEntity->Translate(position);
												}

												return TRUE;
											}

		inline BOOL							SerializeTo(IRuStream *stream)
											{
												// Write tile key
												stream->Write(&m_tileCoord, sizeof(INT32));

												// Write tile resource name
												::SerializeTo(stream, &m_tileResourceName);

												// Write occupied volume
												stream->Write(&m_volume_MinX, sizeof(INT32));
												stream->Write(&m_volume_MinY, sizeof(INT32));
												stream->Write(&m_volume_MinZ, sizeof(INT32));
												stream->Write(&m_volume_MaxX, sizeof(INT32));
												stream->Write(&m_volume_MaxY, sizeof(INT32));
												stream->Write(&m_volume_MaxZ, sizeof(INT32));

												// Write entity transform
												REAL scale = 1.0f;
												CRuQuaternion rotation(0.0f, 0.0f, 0.0f, 1.0f);
												CRuVector3 position(0.0f, 0.0f, 0.0f);

												if(m_tileEntity)
												{
													scale = m_tileEntity->GetScale(ruFRAMETYPE_LOCAL);
													rotation = m_tileEntity->GetRotation(ruFRAMETYPE_LOCAL);
													position = m_tileEntity->GetTranslation(ruFRAMETYPE_LOCAL);
												}

												stream->Write(&scale, sizeof(REAL));
												::SerializeTo(stream, &rotation);
												::SerializeTo(stream, &position);

												return TRUE;
											}

		inline TileDescriptor&				operator=(TileDescriptor &srcDesc)
											{
												m_tileCoord = srcDesc.m_tileCoord;

												INT32 resNameLen = (INT32)strlen(srcDesc.m_tileResourceName) + 1;
												m_tileResourceName = ruNEW char [resNameLen];
												memcpy(m_tileResourceName, srcDesc.m_tileResourceName, resNameLen);

												m_volume_MinX = srcDesc.m_volume_MinX;
												m_volume_MinY = srcDesc.m_volume_MinY;
												m_volume_MinZ = srcDesc.m_volume_MinZ;
												m_volume_MaxX = srcDesc.m_volume_MaxX;
												m_volume_MaxY = srcDesc.m_volume_MaxY;
												m_volume_MaxZ = srcDesc.m_volume_MaxZ;

												m_tileEntity = static_cast<CRuEntity *>(srcDesc.m_tileEntity->Clone());

												return *this;
											}
	};

	// Settings
	INT32									m_mapSize;									//!< Size of the map (always square)
	INT32									m_mapDepth;									//!< Depth of the map (number of levels)
	REAL									m_layerHeight;								//!< Height of each layer
	REAL									m_tileSize;									//!< Size of each tile (always square)

	CRuVector3								m_origin;

	// Editor mode settings
	char*									m_rootPath;
	char*									m_tilePath;									//!< Path to tiles (relative to root path)

	// Data
	CRuHashMap<INT32, TileDescriptor *>**	m_tileHash;
	CRuArrayList<TileDescriptor *>			m_placedCustomTiles;

	CRuBitField*							m_tileStateMap;
	CRuBitField*							m_exclusionMap;

	// Runtime toggles
	BOOL									m_enableEditorMode;

	// Runtime data
	INT32									m_currentDepth;
	CRuStack<INT32>							m_dirtyTiles;

	CRuEntity*								m_tempExclusion_Entity;
	CRuAABB									m_tempExclusion_TileBounds;
	INT32									m_tempExclusion_MinX;
	INT32									m_tempExclusion_MinY;
	INT32									m_tempExclusion_MinZ;
	INT32									m_tempExclusion_MaxX;
	INT32									m_tempExclusion_MaxY;
	INT32									m_tempExclusion_MaxZ;

	CRuArrayList<CRuEntity *>				m_systemTiles[15];							//!< The fifteen system-only tile bins
	CRuArrayList<CRuEntity *>				m_customTiles;								//!< List of custom tiles

	CRuEntity_Widget_TileGrid*				m_tileGrid;

	INT32									m_defaultTileID;
	TileSelectMode							m_tileSelectMode;

	CRuPartition_QuadTree*					m_tilePartition;							//!< Partitioning structure for tiles
	CRuArrayList<IRuObject *>				m_temp_Objects;

	// Runtime callback temp variables
	IRuStream*								m_outputStream;

public:
											CRuEntity_TileMap();
	virtual									~CRuEntity_TileMap();

	// Base functions
	BOOL									Initialize(INT32 mapSize, INT32 mapDepth, REAL layerHeight);

	void									SetRootPath(const char *rootPath);
	void									SetTilePath(const char *tilePath);
	BOOL									ActivateEditorMode();
	CRuArrayList<CRuEntity *>&				GetCustomTiles();
	INT32									GetMapDepth();

	// Manipulation
	BOOL									PickClosestPlacementPoint(const CRuSegment3 &segment, CRuVector3 &pointOut);

	// Custom tile placement
	BOOL									ClearTempExclusion();
	BOOL									SetTempExclusionByObject(CRuEntity *entity, const CRuSegment3 &segment);
	BOOL									UpdateTempExclusionTileBounds();
	BOOL									PlaceCustomTile();
	BOOL									RemoveCustomTile(const CRuSegment3 &segment);

	// Standard tile editing
	BOOL									SetCurrentDepth(INT32 depth);
	BOOL									SetDefaultTileID(INT32 tileID);
	BOOL									SetTileSelectMode(CRuEntity_TileMap::TileSelectMode tileSelectMode);
	void									ToggleTileAt(const CRuSegment3 &segment, BOOL toggleState);
	void									ReselectTileAt(const CRuSegment3 &segment);

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

protected:
	REAL									PadValueToMultiple(REAL value, REAL divisor);
	REAL									CalculateTileSize();
	CRuAABB									CalculateTileBounds(CRuEntity *tileEntity);
	CRuAABB									ParseTileBounds(CRuEntity *tileEntity);
	CRuAABB									CalculateGridVolume(const CRuAABB &bounds, const CRuSegment3 &segment);
	BOOL									IsExcluded(INT32 x, INT32 y, INT32 z);

	BOOL									ClearTileHashCallback(const void *key, void *data);
	BOOL									CloneTileHashCallback(const void *key, void *data);
	BOOL									SerializeTileHashCallback(const void *key, void *data);

	void									InitializePartitioningStructure();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_TileMap, CRuEntity_TileMap)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
