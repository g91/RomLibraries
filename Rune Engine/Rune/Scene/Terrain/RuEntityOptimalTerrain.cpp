#include "../../Scene/Terrain/RuEntityOptimalTerrain.h"

#include "../../Helper/RuHelper_Entity.h"

#ifdef KALYDO
#include "../../core/datastore/RuDataStore_Disk.h"
#endif // KALYDO

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuOptimalTerrain_Chunk, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuOptimalTerrain_Chunk, "CRuOptimalTerrain_Chunk", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuOptimalTerrain_Chunk, 0xB998108E, 0x642643e0, 0x9C2AA68C, 0x5B9AEF03)

ruRTTI_DEFINE_SUBCLASS(IRuOptimalTerrain_DataSource, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuOptimalTerrain_DataSource, "IRuOptimalTerrain_DataSource", "IRuObject")
ruCLASSGUID_DEFINE(IRuOptimalTerrain_DataSource, 0xBD950906, 0xC73D402f, 0x97E1DBAB, 0x2EE3B408)

ruRTTI_DEFINE_SUBCLASS(CRuOptimalTerrain_DataSource_DS, IRuOptimalTerrain_DataSource)
ruHEAP_DEFINE_SUBCLASS(CRuOptimalTerrain_DataSource_DS, "CRuOptimalTerrain_DataSource_DS", "IRuOptimalTerrain_DataSource")
ruCLASSGUID_DEFINE(CRuOptimalTerrain_DataSource_DS, 0x27269EB9, 0x3D544baf, 0x9EB00671, 0xDB75F4AA)

ruRTTI_DEFINE_SUBCLASS(CRuOptimalTerrain, IRuEntity_Container)
ruHEAP_DEFINE_SUBCLASS(CRuOptimalTerrain, "CRuOptimalTerrain", "IRuEntity_Container")
ruCLASSGUID_DEFINE(CRuOptimalTerrain, 0xC3F9DBB3, 0x21C445c2, 0x9686D314, 0x22D1AE0B)

// ************************************************************************************************************************************************************

CRuOptimalTerrain_Chunk::CRuOptimalTerrain_Chunk()
:	m_chunkKey(0x00000000),
	m_dataValidated(FALSE),
	m_isMaskedChunk(FALSE),
	m_dimension(64),
	m_gridSize(10.0f),
	m_lodLevels(1),
	m_numVertices(0),
	m_numIndices(0),
	m_vertexFlags(NULL),
	m_mesh(NULL),
	m_colStructure(NULL),
	m_currentLOD(0),
	m_currentTileLink(0),
	m_splatIDMap(NULL),
	m_alphaTextureLOD(0)
{
	m_material = ruNEW CRuMaterial();
	m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_TERRAIN_PACKED"));
}

CRuOptimalTerrain_Chunk::~CRuOptimalTerrain_Chunk()
{
	delete[] m_vertexFlags;
	ruSAFE_RELEASE(m_mesh);
	ruSAFE_RELEASE(m_colStructure);
	ruSAFE_RELEASE(m_material);
	ruSAFE_RELEASE(m_splatIDMap);

	for(INT32 i = 0; i < m_lodMeshes.Count(); ++i)
	{
		ruSAFE_RELEASE(m_lodMeshes[i])
	}

	for(INT32 i = 0; i < m_tileLinkMeshes.Count(); ++i)
	{
		for(INT32 j = 0; j < 16; ++j)
		{
			ruSAFE_RELEASE(m_tileLinkMeshes[i][j]);
		}

		delete[] m_tileLinkMeshes[i];
	}

	for(INT32 i = 0; i < m_doodadLists.Count(); ++i)
	{
		delete m_doodadLists[i];
	}

	for(INT32 i = 0; i < m_waterLayers.Count(); ++i)
	{
		ruSAFE_RELEASE(m_waterLayers[i]);
	}
}

INT32 CRuOptimalTerrain_Chunk::GetChunkKey()
{
	return m_chunkKey;
}

void CRuOptimalTerrain_Chunk::SetChunkKey(INT32 chunkKey)
{
	m_chunkKey = chunkKey;
}

INT32 CRuOptimalTerrain_Chunk::GetUseIteration() const
{
	return m_useIteration;
}

INT32 CRuOptimalTerrain_Chunk::SetUseIteration(INT32 useIteration)
{
	m_useIteration = useIteration;
	return m_useIteration;
}

BOOL CRuOptimalTerrain_Chunk::SerializeFrom(IRuStream *stream, RuOptimalTerrainChunkLoadMode loadMode, IRuMeshTopology *meshTopology)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read chunk iteration
	stream->Read(&m_chunkIteration, sizeof(INT32));

	// Read chunk dimension
	stream->Read(&m_dimension, sizeof(INT32));

	// Read grid size
	stream->Read(&m_gridSize, sizeof(float));

	// Read default number of lod levels
	stream->Read(&m_lodLevels, sizeof(INT32));

	m_lodLevels = 5;

	// Read chunk bounds
	stream->Read(&m_bounds_AABB.Minima().x, sizeof(float));
	stream->Read(&m_bounds_AABB.Minima().y, sizeof(float));
	stream->Read(&m_bounds_AABB.Minima().z, sizeof(float));
	stream->Read(&m_bounds_AABB.Maxima().x, sizeof(float));
	stream->Read(&m_bounds_AABB.Maxima().y, sizeof(float));
	stream->Read(&m_bounds_AABB.Maxima().z, sizeof(float));

	// Read coordinate offset values
	stream->Read(&m_origin.m_x, sizeof(float));
	stream->Read(&m_origin.m_z, sizeof(float));

	// Set location
	this->ResetLocalTransform();
	this->Translate(m_origin);

	// Number of vertices = (m_dimension + 1)^2 + (m_dimension + 1) * 4
	m_numVertices = (m_dimension + 1) * (m_dimension + 1) + (m_dimension + 1) * 4;

	// Calculate number of indices
	m_numIndices = ((m_dimension * m_dimension * 2) * 3); // this is for the main body used for collision only
	for(INT32 lod = 0, curDimension = m_dimension; lod < m_lodLevels; ++lod, curDimension /= 2)
	{
		// Main body
		INT32 mainBodyDimension = curDimension - 2;
		m_numIndices += ((mainBodyDimension * mainBodyDimension * 2) * 3);

		// 16 tile link sets
		INT32 facesPerStrip = ((curDimension - 1) * 2) + (curDimension * 2);	// Each strip: strip surface + skirt
		m_numIndices += ((facesPerStrip * 4) * 3) * 16;
	}

	// Construct mesh object
	m_mesh = ruNEW CRuMesh();
	m_mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, m_numVertices, m_numIndices / 3, ruVERTEXFORMAT_DIFFUSE | ruVERTEXFORMAT_BLENDINDICES);
	memset(m_mesh->GetIndices(), 0, sizeof(UINT16) * m_mesh->GetNumIndices());

	// Allocate temporary buffer for uncompressed vertex data
	CRuVector3 *vertices = ruNEW CRuVector3 [m_numVertices];

	// Allocate space for vertex flags
	m_vertexFlags = ruNEW BYTE [m_numVertices];

	// Read height values and normal/vertex data
	INT32 numHeightValues = (m_dimension + 1) * (m_dimension + 1) + (m_dimension + 1) * 4;
	REAL *heightField = ruNEW REAL [numHeightValues];
	DWORD *vertexData = ruNEW DWORD [numHeightValues];
	stream->Read(heightField, sizeof(REAL) * numHeightValues);
	stream->Read(vertexData, sizeof(DWORD) * numHeightValues);

	// Setup XYZ, diffuse, and vertex flags
	DWORD *diffuse = m_mesh->GetMorphTarget(0)->GetDiffuse();
	m_isMaskedChunk = FALSE;

	for(INT32 z = 0, vertexIndex = 0, i = 0; z <= m_dimension; ++z)
	{
		REAL xF = 0.0f;
		REAL zF = m_gridSize * z;
		for(INT32 x = 0; x <= m_dimension; ++x, ++vertexIndex, xF += m_gridSize)
		{
			// Set XZ position
			vertices[vertexIndex].m_x = xF;
			vertices[vertexIndex].m_y = heightField[x + z * (m_dimension + 1)];
			vertices[vertexIndex].m_z = zF;
			diffuse[vertexIndex] = vertexData[x + z * (m_dimension + 1)];

			m_vertexFlags[vertexIndex] = (BYTE) ((diffuse[vertexIndex] & 0xFF000000) >> 24);

			if(m_vertexFlags[vertexIndex] == 0)
				m_isMaskedChunk = TRUE;
		}
	}

	// If the chunk is a masked chunk, re-construct the entire mesh and force LOD level count to one
	if(m_isMaskedChunk)
	{
		m_lodLevels = 1;

		// Calculate number of indices
		m_numIndices = ((m_dimension * m_dimension * 2) * 3) + (m_dimension * 2) * 4 * 3;;

		// Construct new mesh object
		ruSAFE_RELEASE(m_mesh);
		m_mesh = ruNEW CRuMesh();
		m_mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, m_numVertices, m_numIndices / 3, ruVERTEXFORMAT_DIFFUSE | ruVERTEXFORMAT_BLENDINDICES);
		memset(m_mesh->GetIndices(), 0, sizeof(UINT16) * m_mesh->GetNumIndices());

		// Setup XYZ, diffuse, and vertex flags
		diffuse = m_mesh->GetMorphTarget(0)->GetDiffuse();

		for(INT32 z = 0, vertexIndex = 0, i = 0; z <= m_dimension; ++z)
		{
			float xF = 0.0f;
			float zF = m_gridSize * z;
			for(INT32 x = 0; x <= m_dimension; ++x, ++vertexIndex, xF += m_gridSize)
			{
				// Set XZ position
				vertices[vertexIndex].m_x = xF;
				vertices[vertexIndex].m_y = heightField[x + z * (m_dimension + 1)];
				vertices[vertexIndex].m_z = zF;
				diffuse[vertexIndex] = vertexData[x + z * (m_dimension + 1)];

				m_vertexFlags[vertexIndex] = (BYTE) ((diffuse[vertexIndex] & 0xFF000000) >> 24);
			}
		}
	}

	// Update skirt vertices
	UpdateSkirtVertices(vertices, diffuse);

	// Rebuild indices
	BuildChunkTopology();

	if(m_isMaskedChunk == FALSE && meshTopology)
	{
		m_mesh->OverrideMeshTopology(meshTopology);
	}

	// Generate compressed vertices
	UINT16 *compressedVertices = m_mesh->GetMorphTarget(0)->GetBlendIndex();

	m_chunk_Y_Base = m_bounds_AABB.Minima().m_y;
	m_chunk_Y_Increment = (m_bounds_AABB.Maxima().m_y - m_bounds_AABB.Minima().m_y) / 65025;

	for(INT32 vertexIndex = 0; vertexIndex < m_numVertices; ++vertexIndex, compressedVertices += 4)
	{
		UINT16 discreteY = (UINT16) ((vertices[vertexIndex].m_y - m_chunk_Y_Base) / m_chunk_Y_Increment);

		compressedVertices[0] = (BYTE) (vertices[vertexIndex].m_x / m_gridSize);
		compressedVertices[1] = (BYTE) (vertices[vertexIndex].m_z / m_gridSize);
		compressedVertices[2] = discreteY / 255;
		compressedVertices[3] = discreteY % 255;
	}

	// Delete temporaries
	ruSAFE_DELETE_ARRAY(heightField);
	ruSAFE_DELETE_ARRAY(vertexData);

	// Delete temporary vertex array
	ruSAFE_DELETE_ARRAY(vertices);

	// Delete vertex flag array (it is not needed after this point)
	ruSAFE_DELETE_ARRAY(m_vertexFlags);

	// TEMPORARY
	ruSAFE_RELEASE(m_splatIDMap);

	// If the splat map is not loaded but is required, load it now
	if(loadMode == ruOPTIMALTERRAINCHUNKLOADMODE_FULL)
	{
		// Read splat resolution
		stream->Read(&m_splatResolution, sizeof(INT32));

		// Read splat
		IRuObject *idMapObj = RuObjectFactory().CreateObject(stream);

		if(idMapObj && idMapObj->GetType() == CRuSplatIDMap::Type())
		{
			// Store pointer to splat map
			m_splatIDMap = static_cast<CRuSplatIDMap *>(idMapObj);
		}
		else
		{
			// Display error here
			m_splatIDMap = NULL;
			ruSAFE_RELEASE(idMapObj);
		}
	}
	else
	{
		// Skip splat resolution
		stream->Seek(sizeof(INT32), ruSSM_Current);

		// Skip loading the ID map entirely
		if(CRuSplatIDMap::SkipStream(stream) == FALSE)
		{
			return FALSE;
		}
	}

	// Read number of doodad clusters
	INT32 numDoodadClusters = 0;
	stream->Read(&numDoodadClusters, sizeof(INT32));

	if(numDoodadClusters > 0)
	{
		if(loadMode == ruOPTIMALTERRAINCHUNKLOADMODE_COLLISION)
		{
			CRuGUID dummyGUID;
			CRuDoodad_Descriptor doodadDescriptor;

			// Read each doodad cluster
			for(INT32 i = 0; i < numDoodadClusters; ++i)
			{
				// Read template GUID
				RuReadGUID(stream, dummyGUID);

				// Read number of doodads
				INT32 numDoodads = 0;
				stream->Read(&numDoodads, sizeof(INT32));

				// Read each doodad
				for(INT32 j = 0; j < numDoodads; ++j)
				{
					stream->Read(&doodadDescriptor.m_textureIndex, sizeof(INT32));
					stream->Read(&doodadDescriptor.m_scale, sizeof(REAL));
					stream->Read(&doodadDescriptor.m_rotation.x, sizeof(REAL));
					stream->Read(&doodadDescriptor.m_rotation.y, sizeof(REAL));
					stream->Read(&doodadDescriptor.m_rotation.z, sizeof(REAL));
					stream->Read(&doodadDescriptor.m_rotation.w, sizeof(REAL));
					stream->Read(&doodadDescriptor.m_translation.x, sizeof(REAL));
					stream->Read(&doodadDescriptor.m_translation.y, sizeof(REAL));
					stream->Read(&doodadDescriptor.m_translation.z, sizeof(REAL));
					stream->Read(&doodadDescriptor.m_swayMagnitude, sizeof(REAL));
				}
			}
		}
		else
		{
			m_doodadLists.SetNumEntries(numDoodadClusters);

			// Read each doodad cluster
			for(INT32 i = 0; i < numDoodadClusters; ++i)
			{
				// Create new doodad cluster
				m_doodadLists[i] = ruNEW CRuWorld_DoodadCluster();

				// Read template GUID
				RuReadGUID(stream, m_doodadLists[i]->m_guid);

				// Read number of doodads
				INT32 numDoodads = 0;
				stream->Read(&numDoodads, sizeof(INT32));

				// Allocate space for doodads
				m_doodadLists[i]->m_doodads.SetNumEntries(numDoodads);

				// Read each doodad
				for(INT32 j = 0; j < numDoodads; ++j)
				{
					CRuDoodad_Descriptor &doodadDescriptor = m_doodadLists[i]->m_doodads[j];

					stream->Read(&doodadDescriptor.m_textureIndex, sizeof(INT32));
					stream->Read(&doodadDescriptor.m_scale, sizeof(REAL));
					stream->Read(&doodadDescriptor.m_rotation.x, sizeof(REAL));
					stream->Read(&doodadDescriptor.m_rotation.y, sizeof(REAL));
					stream->Read(&doodadDescriptor.m_rotation.z, sizeof(REAL));
					stream->Read(&doodadDescriptor.m_rotation.w, sizeof(REAL));
					stream->Read(&doodadDescriptor.m_translation.x, sizeof(REAL));
					stream->Read(&doodadDescriptor.m_translation.y, sizeof(REAL));
					stream->Read(&doodadDescriptor.m_translation.z, sizeof(REAL));
					stream->Read(&doodadDescriptor.m_swayMagnitude, sizeof(REAL));
				}
			}
		}
	}

	// Read number of water layers
	INT32 numWaterLayers;
	stream->Read(&numWaterLayers, sizeof(INT32));

	// Read each water layer
	for(INT32 i = 0; i < numWaterLayers; ++i)
	{
		// Serialize water layer from store
		m_waterLayers.Add(ruNEW CRuWorld_Water());
		m_waterLayers[i]->SerializeFrom(stream);

		// Build collision structure for the water layer
		m_waterLayers[i]->BuildCollisionStructure();
	}

	// Set entity bounds to enclose the chunk's AABB (Offset back to origin, since this is the local entity bounds)
	CRuVector3 center = (m_bounds_AABB.Minima() + m_bounds_AABB.Maxima()) / 2.0f - CRuVector3(m_bounds_AABB.Minima().m_x, 0.0f, m_bounds_AABB.Minima().m_z);
	REAL radius = (m_bounds_AABB.Maxima() - m_bounds_AABB.Minima()).Magnitude() / 2.0f;
	this->SetLocalEntityBounds(CRuSphere(center, radius));

	// Setup the alpha texture UV modifier
	m_alphaUVModifier[0] = m_bounds_AABB.Minima().x;
	m_alphaUVModifier[1] = m_bounds_AABB.Minima().z;
	m_alphaUVModifier[2] = 1.0f / (m_bounds_AABB.Maxima().x - m_bounds_AABB.Minima().x);
	m_alphaUVModifier[3] = 1.0f / (m_bounds_AABB.Maxima().z - m_bounds_AABB.Minima().z);

	for(INT32 i = 0; i < m_lodMeshes.Count(); ++i)
	{
		m_lodMeshes[i]->GetBatchParameterBlock(0)->SetParameter(ruPARAMTYPE_SPLAT_ALPHAUVMODIFIER, (UINT64) m_alphaUVModifier);
	}

	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_CHUNK_SPACING, RuCastFloatToINT64(m_gridSize));
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_CHUNK_Y_BASE, RuCastFloatToINT64(m_chunk_Y_Base));
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_CHUNK_Y_INCREMENT, RuCastFloatToINT64(m_chunk_Y_Increment));

	// Set default values for UV stretch scales
	for(INT32 i = 0; i < 6; ++i)
	{
		m_textureUVScale[i][0] = 24.0f;
		m_textureUVScale[i][1] = -24.0f;
		m_textureUVScale[i][2] = 1.0f;
		m_textureUVScale[i][3] = 1.0f;
	}

	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_RAW_OFFSET, 0);
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_RAW_COUNT, 6);
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_RAW_DATA, reinterpret_cast<PTRVALUE>(m_textureUVScale));

	return TRUE;
}

BOOL CRuOptimalTerrain_Chunk::SerializeFrom(IRuStream *stream)
{
	return SerializeFrom(stream, ruOPTIMALTERRAINCHUNKLOADMODE_FULL, NULL);
}

BOOL CRuOptimalTerrain_Chunk::SerializeTo(IRuStream *stream)
{
	return FALSE;
}

INT32 CRuOptimalTerrain_Chunk::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuEntity_Renderable::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	// Accumulate base class size
	footprint += sizeof(m_chunkKey);

	footprint += sizeof(m_chunkIteration);
	footprint += sizeof(m_dataValidated);
	footprint += sizeof(m_isMaskedChunk);

	footprint += sizeof(m_dimension);
	footprint += sizeof(m_gridSize);
	footprint += sizeof(m_bounds_AABB);
	footprint += sizeof(m_origin);

	footprint += sizeof(m_lodLevels);
	footprint += sizeof(m_numVertices);
	footprint += sizeof(m_numIndices);

	footprint += sizeof(m_vertexFlags) + (m_vertexFlags ? sizeof(BYTE) * m_numVertices : 0);
	footprint += sizeof(m_mesh) + (m_mesh ? m_mesh->GetMemoryFootprint(enumHash) : 0);
	footprint += sizeof(m_colStructure) + (m_colStructure ? m_colStructure->GetMemoryFootprint(enumHash) : 0);

	footprint += m_doodadLists.GetMemoryFootprint();
	for(INT32 i = 0; i < m_doodadLists.Count(); ++i)
	{
		if(m_doodadLists[i])
		{
			footprint += sizeof(m_doodadLists[i]->m_guid);
			footprint += m_doodadLists[i]->m_doodads.GetMemoryFootprint();
		}
	}
	footprint += m_waterLayers.GetMemoryFootprint();
	for(INT32 i = 0; i < m_waterLayers.Count(); ++i)
	{
		if(m_waterLayers[i])
		{
			footprint += m_waterLayers[i]->GetMemoryFootprint(enumHash);
		}
	}

	footprint += sizeof(m_material) + (m_material ? m_material->GetMemoryFootprint(enumHash) : 0);
	footprint += m_lodMeshes.GetMemoryFootprint();
	for(INT32 i = 0; i < m_lodMeshes.Count(); ++i)
	{
		if(m_lodMeshes[i])
		{
			footprint += m_lodMeshes[i]->GetMemoryFootprint(enumHash);
		}
	}
	footprint += m_tileLinkMeshes.GetMemoryFootprint();
	for(INT32 i = 0; i < m_tileLinkMeshes.Count(); ++i)
	{
		if(m_tileLinkMeshes[i])
		{
			for(INT32 j = 0; j < 16; ++j)
			{
				footprint += m_tileLinkMeshes[i][j]->GetMemoryFootprint(enumHash);
			}
		}
	}

	footprint += sizeof(m_currentLOD);
	footprint += sizeof(m_currentTileLink);

	footprint += sizeof(m_splatResolution);
	footprint += sizeof(m_splatIDMap) + (m_splatIDMap ? m_splatIDMap->GetMemoryFootprint(enumHash) : 0);

	footprint += sizeof(m_alphaTextureLOD);

	footprint += sizeof(m_alphaUVModifier);

	return footprint;
}

BOOL CRuOptimalTerrain_Chunk::AttachCollisionStructure(CRuCollisionStructure_PackedGrid *colStructure)
{
	if(colStructure)
	{
		colStructure->AddRef();
		ruSAFE_RELEASE(m_colStructure);
		m_colStructure = colStructure;
	}
	else
	{
		ruSAFE_RELEASE(m_colStructure);
	}

	return TRUE;
}

INT32 CRuOptimalTerrain_Chunk::GetNumMeshes() const
{
	if(m_isMaskedChunk)
		return 1;
	else
		return 2;
}

BOOL CRuOptimalTerrain_Chunk::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(m_isMaskedChunk)
	{
		*meshOut = m_lodMeshes[0];
		*materialOut = m_material;
	}
	else
	{
		if(meshIndex == 0)
		{
#ifdef KALYDO
			if (m_currentLOD > m_lodMeshes.Count())
				return FALSE;
#endif
			*meshOut = m_lodMeshes[m_currentLOD];
			*materialOut = m_material;
		}
		else
		{
#ifdef KALYDO
			if (m_currentLOD > m_tileLinkMeshes.Count())
				return FALSE;
#endif
			*meshOut = m_tileLinkMeshes[m_currentLOD][m_currentTileLink];
			*materialOut = m_material;
		}
	}

	return TRUE;
}

BOOL CRuOptimalTerrain_Chunk::QueryCollision(CRuCollisionQuery *colQuery)
{
	// Save previous collision transform
	CRuMatrix4x4 *originalWorldTransform = colQuery->m_worldTransform;
	CRuMatrix4x4 *originalInvWorldTransform = colQuery->m_invWorldTransform;

	// Store the inverse of the current world transform into the collision query
	colQuery->m_worldTransform = const_cast<CRuMatrix4x4 *>(&this->GetWorldTransform());
	colQuery->m_invWorldTransform = const_cast<CRuMatrix4x4 *>(&this->GetInverseWorldTransform());

	if(m_colStructure)
	{
		// Attach common collision structure to the chunk mesh
		m_colStructure->AttachToMesh(m_mesh);

		// Set grid parameters
		m_colStructure->SetGridParameters(m_gridSize, m_chunk_Y_Base, m_chunk_Y_Increment);

		// Perform collision query
		m_colStructure->QueryCollision(colQuery);
	}

	// Restore original collision transform
	colQuery->m_worldTransform = originalWorldTransform;
	colQuery->m_invWorldTransform = originalInvWorldTransform;

	// Water is in world space.. yeah yea... so this isn't exactly ideal
	for(INT32 i = 0; i < m_waterLayers.Count(); ++i)
	{
		m_waterLayers[i]->QueryCollision(colQuery);
	}

	return TRUE;
}

void CRuOptimalTerrain_Chunk::SetCamera(CRuCamera *viewCamera)
{
	float sqrFarPlaneDistance = viewCamera->GetFarPlaneDistance();
	float sqrDistanceToChunk = SquaredDistance(viewCamera->GetCameraPosition(), m_bounds_AABB);
	sqrFarPlaneDistance *= sqrFarPlaneDistance;

	// Calculate LOD to use based on a linear scale
	float t = sqrDistanceToChunk / sqrFarPlaneDistance;
	m_currentLOD = min((INT32) (t * m_lodLevels), m_lodLevels - 1);
}

INT32 CRuOptimalTerrain_Chunk::GetLOD()
{
	if(m_isMaskedChunk)
	{
		m_currentLOD = -1;
	}

	return m_currentLOD;
}

void CRuOptimalTerrain_Chunk::SetLOD(INT32 lod)
{
	if(m_isMaskedChunk)
	{
		m_currentLOD = -1;
	}
	else
	{
		m_currentLOD = lod;
	}
}

void CRuOptimalTerrain_Chunk::SetTileLink(INT32 tileLink)
{
	m_currentTileLink = tileLink;
}

void CRuOptimalTerrain_Chunk::ReleaseSplatIDMap()
{
	ruSAFE_RELEASE(m_splatIDMap);
}

void CRuOptimalTerrain_Chunk::SetTexture(INT32 layer, IRuBaseTexture *texture, REAL tileScale)
{
	if(layer >= 0 && layer <= 5)
	{
		m_material->SetTexture(static_cast<RuTextureChannel>(ruTEXCHANNEL_TERRAIN_LAYER_0 + layer), texture);
		m_textureUVScale[layer][0] = tileScale;
		m_textureUVScale[layer][1] = -tileScale;
		m_textureUVScale[layer][2] = 0.0f;
		m_textureUVScale[layer][3] = 0.0f;
	}
}

INT32 CRuOptimalTerrain_Chunk::GetChunkIteration() const
{
	return m_chunkIteration;
}

BOOL CRuOptimalTerrain_Chunk::IsMaskedChunk() const
{
	return m_isMaskedChunk;
}

const CRuAABB &CRuOptimalTerrain_Chunk::GetBoundingBox() const
{
	return m_bounds_AABB;
}

CRuSplatIDMap *CRuOptimalTerrain_Chunk::GetSplatIDMap()
{
	return m_splatIDMap;
}

INT32 CRuOptimalTerrain_Chunk::GetNumDoodadClusters() const
{
	return m_doodadLists.Count();
}

CRuWorld_DoodadCluster *CRuOptimalTerrain_Chunk::GetDoodadCluster(INT32 index)
{
	return m_doodadLists[index];
}

INT32 CRuOptimalTerrain_Chunk::GetNumWaterLayers() const
{
	return m_waterLayers.Count();
}

CRuWorld_Water *CRuOptimalTerrain_Chunk::GetWaterLayerByIndex(INT32 index)
{
	return m_waterLayers[index];
}

INT32 CRuOptimalTerrain_Chunk::GetMeshDataSize() const
{
	return (m_numVertices * 16) + (m_numIndices * 2);
}

INT32 CRuOptimalTerrain_Chunk::GetSplatDataSize() const
{
	if(m_splatIDMap)
	{
		return m_splatIDMap->GetMemoryFootprint();
	}

	return 0;
}

void CRuOptimalTerrain_Chunk::SetAlphaTextureLOD(INT32 alphaTextureLOD)
{
	if(m_alphaTextureLOD != alphaTextureLOD)
	{
		// Store current alpha texture LOD
		m_alphaTextureLOD = alphaTextureLOD;

		// If the splat map exists, set the new LOD into the textures
		if(m_splatIDMap)
		{
			for(INT32 i = 0; i < m_splatIDMap->GetNumAlphaMaps(); ++i)
			{
				m_splatIDMap->GetAlphaMap(i)->SetLOD(m_alphaTextureLOD);
			}
		}
	}
}

void CRuOptimalTerrain_Chunk::BuildChunkTopology()
{
	// Clear all batches in the mesh (we will rebuild all of the batches)
	m_mesh->ClearBatches();

	if(m_isMaskedChunk)
	{
		// Start building the LOD mesh at the base level
		UINT16 *indices = m_mesh->GetIndices();
		INT32 indexCount = 0;

		// Build one base level with original geometry for collision
		INT32 batchOffset = indexCount;
		INT32 batchNumIndices = 0;

		// Build skirt indices
		batchNumIndices = RuTerrainUtility_BuildGridIndicesEx(m_dimension, 1, m_dimension + 1, 0, m_vertexFlags, &indices[batchNumIndices]);
		batchNumIndices += RuTerrainUtility_BuildIndices_Skirt(m_dimension, 1, m_dimension + 1, m_dimension + 1, m_vertexFlags, &indices[batchNumIndices]);

		m_mesh->AddBatch(0, batchNumIndices / 3);

		CRuProxyMesh *proxyMesh = ruNEW CRuProxyMesh();
		proxyMesh->SetProxiedMesh(m_mesh, 0);

		m_lodMeshes.Add(proxyMesh);
	}
	else
	{
		// Start building the LOD mesh at the base level
		UINT16 *indices = m_mesh->GetIndices();
		INT32 indexCount = 0;

		// Build one base level with original geometry for collision
		INT32 batchOffset = indexCount;
		INT32 batchNumIndices = RuTerrainUtility_BuildGridIndicesEx(m_dimension, 1, m_dimension + 1, 0, m_vertexFlags, &indices[indexCount]);
		INT32 batchIndex = m_mesh->AddBatch(0, batchNumIndices / 3);
		indexCount += batchNumIndices;

		for(INT32 lod = 0, gridDimension = m_dimension, stride = 1, pitch = m_dimension + 1, offset = pitch + 1; lod < m_lodLevels; ++lod, gridDimension /= 2, stride *= 2, pitch *= 2, offset *= 2)
		{
			INT32 choppedDimension = gridDimension - 2;

			// Allocate space for the LOD proxy mesh
			if(lod >= m_lodMeshes.Count())
			{
				m_lodMeshes.Add(ruNEW CRuProxyMesh());
				m_tileLinkMeshes.Add(ruNEW CRuProxyMesh * [16]);
			}

			// Build main body face indices
			batchOffset = indexCount;
			batchNumIndices = RuTerrainUtility_BuildGridIndicesEx(choppedDimension, stride, pitch, offset, m_vertexFlags, &indices[indexCount]);
			batchIndex = m_mesh->AddBatch(batchOffset / 3, batchNumIndices / 3);
			indexCount += batchNumIndices;

			// Setup the LOD proxy mesh for main body
			m_lodMeshes[lod]->SetProxiedMesh(m_mesh, batchIndex);

			// Build tile linking face indices
			for(INT32 i = 0; i < 16; ++i)
			{
				batchOffset = indexCount;
				batchNumIndices = RuTerrainUtility_BuildIndices_TileLink(i, gridDimension, stride, pitch, m_dimension + 1, m_vertexFlags, &indices[indexCount]);
				batchIndex = m_mesh->AddBatch(batchOffset / 3, batchNumIndices / 3);
				indexCount += batchNumIndices;

				m_tileLinkMeshes[lod][i] = ruNEW CRuProxyMesh();
				m_tileLinkMeshes[lod][i]->SetProxiedMesh(m_mesh, batchIndex);
			}
		}
	}
}

void CRuOptimalTerrain_Chunk::UpdateSkirtVertices(CRuVector3 *vertices, DWORD *diffuse)
{
	INT32 topSkirtVertexArrayOffset = (m_dimension + 1) * (m_dimension + 1);
	INT32 bottomSkirtVertexArrayOffset = topSkirtVertexArrayOffset + (m_dimension + 1);
	INT32 leftSkirtVertexArrayOffset = bottomSkirtVertexArrayOffset + (m_dimension + 1);
	INT32 rightSkirtVertexArrayOffset = leftSkirtVertexArrayOffset + (m_dimension + 1);

	// Duplicate skirt vertices (top row)
	for(INT32 i = 0, destOffset = topSkirtVertexArrayOffset, srcOffset = 0; i <= m_dimension; ++i, ++destOffset, ++srcOffset)
	{
		vertices[destOffset].Set(vertices[srcOffset].x, vertices[srcOffset].y - 1.0f, vertices[srcOffset].z);
		diffuse[destOffset] = diffuse[srcOffset];
		m_vertexFlags[destOffset] = m_vertexFlags[srcOffset];
	}

	// Duplicate skirt vertices (bottom row)
	for(INT32 i = 0, destOffset = bottomSkirtVertexArrayOffset, srcOffset = m_dimension * (m_dimension + 1); i <= m_dimension; ++i, ++destOffset, ++srcOffset)
	{
		vertices[destOffset].Set(vertices[srcOffset].x, vertices[srcOffset].y - 1.0f, vertices[srcOffset].z);
		diffuse[destOffset] = diffuse[srcOffset];
		m_vertexFlags[destOffset] = m_vertexFlags[srcOffset];
	}

	// Duplicate skirt vertices (left row)
	for(INT32 i = 0, destOffset = leftSkirtVertexArrayOffset, srcOffset = 0; i <= m_dimension; ++i, ++destOffset, srcOffset += (m_dimension + 1))
	{
		vertices[destOffset].Set(vertices[srcOffset].x, vertices[srcOffset].y - 1.0f, vertices[srcOffset].z);
		diffuse[destOffset] = diffuse[srcOffset];
		m_vertexFlags[destOffset] = m_vertexFlags[srcOffset];
	}

	// Duplicate skirt vertices (right row)
	for(INT32 i = 0, destOffset = rightSkirtVertexArrayOffset, srcOffset = m_dimension; i <= m_dimension; ++i, ++destOffset, srcOffset += (m_dimension + 1))
	{
		vertices[destOffset].Set(vertices[srcOffset].x, vertices[srcOffset].y - 1.0f, vertices[srcOffset].z);
		diffuse[destOffset] = diffuse[srcOffset];
		m_vertexFlags[destOffset] = m_vertexFlags[srcOffset];
	}
}

// ************************************************************************************************************************************************************

CRuOptimalTerrain_DataSource_DS::CRuOptimalTerrain_DataSource_DS()
:	m_dataStore(NULL),
	m_commonCollisionStructure(NULL),
	m_commonMeshTopology(NULL),
	m_commonBackdropMeshTopology(NULL),
	m_collisionOnlyMode(FALSE),
	m_chunkHash(NULL),
	m_textureList(NULL)
{
	// Create chunk hash
	m_chunkHash = ruNEW CRuHashMap<DWORD, CRuOptimalTerrain_Chunk *>(1024);

	// Create back drop queue
	m_backdropQueue = ruNEW CRuLRUQueue<INT32, CRuWorld_Backdrop *>(128, 256);

	// Create work queues
	m_chunkWorkQueue = ruNEW CRuThread_WorkQueue<INT32, CRuOptimalTerrain_Chunk *>(256, 256, 128);
	m_backdropWorkQueue = ruNEW CRuThread_WorkQueue<INT32, CRuWorld_Backdrop *>(64, 64, 64);

	// Create object-wide critical section
	m_commonCollisionStructure_CS = ruNEW CRuThread_CriticalSection();

	// Allocate resources for worker
	m_worker_StreamCompressor = ruNEW CRuStreamCompressor();
	m_worker_CompressedChunkStream = ruNEW CRuMemoryStream();
	m_worker_DecompressedChunkStream = ruNEW CRuMemoryStream();

	// Register worker function with worker thread
	g_ruGlobalWorkerThread->RegisterWorkerFunction(this->GetGUID(), boost::bind(&CRuOptimalTerrain_DataSource_DS::WorkerFunction, this));
}

CRuOptimalTerrain_DataSource_DS::~CRuOptimalTerrain_DataSource_DS()
{
	// Unregister worker function
	g_ruGlobalWorkerThread->UnregisterWorkerFunction(this->GetGUID());

	// Clear chunk hash and delete it
	ClearChunkQueue();
	ruSAFE_DELETE(m_chunkHash);

	// Purge chunk LRU queues
	ClearBackdropQueue();
	ruSAFE_DELETE(m_backdropQueue);

	// Release texture list
	ruSAFE_RELEASE(m_textureList);

	// Release common objects
	ruSAFE_RELEASE(m_commonCollisionStructure);
	ruSAFE_RELEASE(m_commonMeshTopology);
	ruSAFE_RELEASE(m_commonBackdropMeshTopology);

	// Release data store
	ruSAFE_RELEASE(m_dataStore);

	// Clean up work queues
	INT32 orphanedChunkKey;
	CRuOptimalTerrain_Chunk *orphanedChunk = NULL;

	while(m_chunkWorkQueue->DequeueResult(orphanedChunkKey, orphanedChunk))
	{
		ruSAFE_RELEASE(orphanedChunk);
	}

	delete m_chunkWorkQueue;

	// Clean up backdrop work queue
	INT32 orphanedBackdropKey;
	CRuWorld_Backdrop *orphanedBackdrop = NULL;

	while(m_backdropWorkQueue->DequeueResult(orphanedBackdropKey, orphanedBackdrop))
	{
		ruSAFE_RELEASE(orphanedBackdrop);
	}

	delete m_backdropWorkQueue;

	// Destroy critical sections
	delete m_commonCollisionStructure_CS;

	// Release resources for worker
	delete m_worker_StreamCompressor;
	delete m_worker_CompressedChunkStream;
	delete m_worker_DecompressedChunkStream;
}

INT32 CRuOptimalTerrain_DataSource_DS::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = 0;

	m_temp_Footprint = 0;
	m_temp_EnumHash = enumHash;
	
	m_chunkHash->ForAllElements(boost::bind(&CRuOptimalTerrain_DataSource_DS::ChunkQueueMemoryFootprintCallback, this, _1, _2));
	m_backdropQueue->ForAllElements(boost::bind(&CRuOptimalTerrain_DataSource_DS::BackdropQueueMemoryFootprintCallback, this, _1, _2));
	footprint += m_temp_Footprint;

	return footprint;
}

BOOL CRuOptimalTerrain_DataSource_DS::ChunkQueueMemoryFootprintCallback(const void *key, void *data)
{
	CRuOptimalTerrain_Chunk *chunk = *reinterpret_cast<CRuOptimalTerrain_Chunk **>(data);

	m_temp_Footprint += chunk->GetMemoryFootprint(m_temp_EnumHash);

	return TRUE;
}

BOOL CRuOptimalTerrain_DataSource_DS::BackdropQueueMemoryFootprintCallback(const void *key, void *data)
{
	CRuWorld_Backdrop *backdrop = *reinterpret_cast<CRuWorld_Backdrop **>(data);

	m_temp_Footprint += backdrop->GetMemoryFootprint(m_temp_EnumHash);

	return TRUE;
}

INT32 CRuOptimalTerrain_DataSource_DS::IncrementIteration()
{
	++m_iteration;
	return m_iteration;
}

BOOL CRuOptimalTerrain_DataSource_DS::Update(REAL deltaT)
{
	INT32 newChunkKey;
	CRuOptimalTerrain_Chunk *newChunk = NULL;

	while(m_chunkWorkQueue->DequeueResult(newChunkKey, newChunk))
	{
		DWORD newChunkKey = newChunk->GetChunkKey();
		DWORD newChunkKeyHash = RuGlobalHashGen().GetHashCode(newChunkKey);

		// If the chunk hash already contains the loaded chunk, release the loaded chunk
		if(m_chunkHash->ContainsKey(newChunkKey, newChunkKeyHash))
		{
			// Detach common collision structure from the duplicate chunk
			this->DetachCollisionStructure(newChunk);

			// Release the duplicate chunk
			ruSAFE_RELEASE(newChunk);
		}
		else
		{
			// Set chunk use iteration
			newChunk->SetUseIteration(m_iteration);

			// Put chunk into the chunk hash
			m_chunkHash->Add(newChunkKey, newChunkKeyHash, newChunk);
		}
	}

	INT32 newBackdropKey;
	CRuWorld_Backdrop *newBackdrop = NULL;

	while(m_backdropWorkQueue->DequeueResult(newBackdropKey, newBackdrop))
	{
		DWORD newBackdropKeyHash = RuGlobalHashGen().GetHashCode(newBackdropKey);

		if(m_backdropQueue->ContainsKey(newBackdropKey, newBackdropKeyHash))
		{
			// Release the duplicate backdrop
			ruSAFE_RELEASE(newBackdrop);
		}
		else
		{
			// Try to free up space in the LRU queue for the new backdrop, if necessary
			CRuWorld_Backdrop *reclaimedBackdrop;
			while(m_backdropQueue->Reclaim(newBackdropKey, newBackdropKeyHash, 1, reclaimedBackdrop))
			{
				ruSAFE_RELEASE(reclaimedBackdrop);
			}

			// Put backdrop into the backdrop queue
			m_backdropQueue->Add(newBackdropKey, newBackdropKeyHash, newBackdrop, 1);
		}
	}

	// Garbage collect the chunk hash
	GCChunkQueue();

	return TRUE;
}

BOOL CRuOptimalTerrain_DataSource_DS::SetCollisionOnlyMode(BOOL collisionOnlyMode)
{
	m_collisionOnlyMode = collisionOnlyMode;

	return TRUE;
}

#ifdef KALYDO

void CRuOptimalTerrain_DataSource_DS::KalydoBackdropDownloaded(const BYTE* key, void* userData)
{
	CRuOptimalTerrain_DataSource_DS* ds = reinterpret_cast<CRuOptimalTerrain_DataSource_DS*>(userData);

	ds->m_backdropWorkQueue->QueueWork(*reinterpret_cast<const DWORD*>(key));
	
	delete [] key;
}

void CRuOptimalTerrain_DataSource_DS::KalydoChunkDownloaded(const BYTE* key, void* userData)
{
	CRuOptimalTerrain_DataSource_DS* ds = reinterpret_cast<CRuOptimalTerrain_DataSource_DS*>(userData);

	ds->m_chunkWorkQueue->QueueWork(*reinterpret_cast<const DWORD*>(key));
	
	delete [] key;
}

void CRuOptimalTerrain_DataSource_DS::KalydoQueue(DWORD key, BOOL isBackdrop)
{
	BYTE* bKey = new BYTE[sizeof(DWORD)];
	memcpy(bKey, &key, sizeof(DWORD));
	if (isBackdrop)
	{
		if (m_dataStore->KalydoDownloadData(m_tableHandle_Backdrop, bKey, this, &CRuOptimalTerrain_DataSource_DS::KalydoBackdropDownloaded) == TRUE)
			return;

		// if all else fails ... queue, and queue again!
		m_backdropWorkQueue->QueueWork(key);
	}
	else
	{
		if (m_dataStore->KalydoDownloadData(m_tableHandle_Chunk, bKey, this, &CRuOptimalTerrain_DataSource_DS::KalydoChunkDownloaded) == TRUE)
			return;

		m_chunkWorkQueue->QueueWork(key);
	}

	delete [] bKey;
}

#endif // KALYDO
CRuOptimalTerrain_Chunk *CRuOptimalTerrain_DataSource_DS::LoadChunk(INT32 x, INT32 z, RuOptimalTerrainChunkLoadMode loadMode)
{
	// Build chunk key and its hash
	DWORD chunkKey = (x << 16) | z;
	DWORD chunkKeyHash = RuGlobalHashGen().GetHashCode(chunkKey);
	CRuOptimalTerrain_Chunk *chunk = NULL;

	// Attempt to locate chunk in the chunk hash
	if(m_chunkHash->Get(chunkKey, chunkKeyHash, chunk) == FALSE && m_chunkWorkQueue->WorkFailed(chunkKey, FALSE) == FALSE)
	{
		// If the chunk is not already loaded, queue it for loading
#ifdef KALYDO
		KalydoQueue(chunkKey, FALSE);
#else
		m_chunkWorkQueue->QueueWork(chunkKey);
#endif // KALYDO
	}

	// Update chunk's use iteration
	if(chunk)
	{
		chunk->SetUseIteration(m_iteration);
	}

	return chunk;
}

CRuWorld_Backdrop *CRuOptimalTerrain_DataSource_DS::LoadBackdrop(INT32 x, INT32 z)
{
	// Build backdrop key and its hash
	DWORD backdropKey = (x << 16) | z;
	DWORD backdropKeyHash = RuGlobalHashGen().GetHashCode(backdropKey);
	CRuWorld_Backdrop *backdrop = NULL;

	// Attempt to locate chunk in the chunk queue
	if(m_backdropQueue->Get(backdropKey, backdropKeyHash, backdrop) == FALSE && m_backdropWorkQueue->WorkFailed(backdropKey, FALSE) == FALSE)
	{
		// If the backdrop is not already loaded, queue it for loading
#ifdef KALYDO
		KalydoQueue(backdropKey, TRUE);
#else
		m_backdropWorkQueue->QueueWork(backdropKey);
#endif // KALYDO
	}

	return backdrop;
}

BOOL CRuOptimalTerrain_DataSource_DS::ChunkLoadFailed(UINT32 chunkKey)
{
	return m_chunkWorkQueue->WorkFailed(chunkKey, FALSE);
}

BOOL CRuOptimalTerrain_DataSource_DS::BackdropLoadFailed(UINT32 chunkKey)
{
	return m_backdropWorkQueue->WorkFailed(chunkKey, FALSE);
}

CRuTextureList *CRuOptimalTerrain_DataSource_DS::GetTextureList()
{
	return m_textureList;
}

INT32 CRuOptimalTerrain_DataSource_DS::GetNumWaterBodyDescriptors()
{
	return m_dataStore->GetNumRecords(m_tableHandle_WaterDescriptor);
}

CRuWorld_WaterBodyDescriptor *CRuOptimalTerrain_DataSource_DS::LoadWaterBodyDescriptorByIndex(INT32 index)
{
	CRuWorld_WaterBodyDescriptor *waterBodyDescriptor = NULL;
	IRuStream *stream = m_dataStore->RetrieveByIndex(m_tableHandle_WaterDescriptor, index);

	if(stream)
	{
		waterBodyDescriptor = ruNEW CRuWorld_WaterBodyDescriptor();

		if(waterBodyDescriptor->SerializeFrom(stream) == FALSE)
		{
			delete waterBodyDescriptor;
			waterBodyDescriptor = NULL;
		}

		delete stream;
	}

	return waterBodyDescriptor;
}

INT32 CRuOptimalTerrain_DataSource_DS::GetChunkDimension() const
{
	return m_chunkDimension;
}

INT32 CRuOptimalTerrain_DataSource_DS::GetNumXChunks() const
{
	return m_numXChunks;
}

INT32 CRuOptimalTerrain_DataSource_DS::GetNumZChunks() const
{
	return m_numZChunks;
}

INT32 CRuOptimalTerrain_DataSource_DS::GetSplatResolution() const
{
	return m_splatResolution;
}

REAL CRuOptimalTerrain_DataSource_DS::GetGridSize() const
{
	return m_gridSize;
}

CRuAABB CRuOptimalTerrain_DataSource_DS::GetBounds(INT32 x, INT32 z) const
{
	CRuAABB chunkBounds;
	REAL verticalLimit = 20000.0f;
	REAL chunkWorldSpaceSize = m_chunkDimension * m_gridSize;

	// Calculate chunk bounds based on integer (X, Z) coordinates
	chunkBounds.Minima().Set(x * chunkWorldSpaceSize + m_origin.m_x, -verticalLimit, z * chunkWorldSpaceSize + m_origin.m_z);
	chunkBounds.Maxima().Set(chunkBounds.Minima().m_x + chunkWorldSpaceSize, verticalLimit, chunkBounds.Minima().m_z + chunkWorldSpaceSize);

	return chunkBounds;
}

CRuAABB CRuOptimalTerrain_DataSource_DS::GetAllBackdropBounds()
{
	CRuAABB allBackdropBounds;
	REAL verticalLimit = 20000.0f;
	REAL backdropWorldSpaceSize = m_backdropDimension * m_backdropGridSpacing;

	allBackdropBounds.Minima().Set(m_backdropOrigin.m_x, -verticalLimit, m_backdropOrigin.m_z);
	allBackdropBounds.Maxima().Set(allBackdropBounds.Minima().m_x + backdropWorldSpaceSize * m_backdropXChunks, verticalLimit, allBackdropBounds.Minima().m_z + backdropWorldSpaceSize * m_backdropZChunks);

	return allBackdropBounds;
}

CRuAABB CRuOptimalTerrain_DataSource_DS::GetBackdropBounds(INT32 x, INT32 z) const
{
	CRuAABB backdropBounds;
	REAL verticalLimit = 20000.0f;
	REAL backdropWorldSpaceSize = m_backdropDimension * m_backdropGridSpacing;

	// Calculate backdrop bounds based on integer (X, Z) coordinates
	backdropBounds.Minima().Set(x * backdropWorldSpaceSize + m_backdropOrigin.m_x, -verticalLimit, z * backdropWorldSpaceSize + m_backdropOrigin.m_z);
	backdropBounds.Maxima().Set(backdropBounds.Minima().m_x + backdropWorldSpaceSize, verticalLimit, backdropBounds.Minima().m_z + backdropWorldSpaceSize);

	return backdropBounds;
}

INT32 CRuOptimalTerrain_DataSource_DS::GetNumBackdropXChunks() const
{
	return m_backdropXChunks;
}

INT32 CRuOptimalTerrain_DataSource_DS::GetNumBackdropZChunks() const
{
	return m_backdropZChunks;
}

void CRuOptimalTerrain_DataSource_DS::ClearWorkQueues()
{
	m_chunkWorkQueue->ClearWorkQueue();
	m_backdropWorkQueue->ClearWorkQueue();
}

CRuDataStore_Base *CRuOptimalTerrain_DataSource_DS::GetDataStore()
{
	return m_dataStore;
}

BOOL CRuOptimalTerrain_DataSource_DS::Open(CRuDataStore_Base *dataStore)
{
	// Store reference to data store
	ruSAFE_RELEASE(m_dataStore);
	m_dataStore = dataStore;
	m_dataStore->AddRef();

	// Get table handles
	m_tableHandle_Header = m_dataStore->GetTableHandle("header");
	m_tableHandle_Chunk = m_dataStore->GetTableHandle("chunks");
	m_tableHandle_ChunkIteration = dataStore->GetTableHandle("chunk_iterations");
	m_tableHandle_BackdropHeader = dataStore->GetTableHandle("backdrop_header");
	m_tableHandle_Backdrop = dataStore->GetTableHandle("backdrop");
	m_tableHandle_TextureList = m_dataStore->GetTableHandle("texture_list");
	m_tableHandle_WaterDescriptor = m_dataStore->GetTableHandle("water_descriptor");

	// Load header information
	LoadHeader();

	// Load backdrop header information
	LoadBackdropHeader();

	// Construct texture list
	m_textureList = ruNEW CRuTextureList();
	m_textureList->EnableDelayedBinding(TRUE);
	m_textureList->EnableGarbageCollection(TRUE);

	// Prepend 2048 NULL textures into the texture list
	for(INT32 i = 0; i < 2048; ++i)
	{
		m_textureList->AddTexture(NULL);
	}

	// Load texture list
	LoadTextureList(m_textureList);

	return TRUE;
}

BOOL CRuOptimalTerrain_DataSource_DS::LoadHeader()
{
	INT32 dummyKey = 0;
	IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_Header, reinterpret_cast<BYTE *>(&dummyKey));

	if(stream)
	{
		// Read header information
		stream->Read(&m_origin.m_x, sizeof(REAL));
		stream->Read(&m_origin.m_y, sizeof(REAL));
		stream->Read(&m_origin.m_z, sizeof(REAL));
		stream->Read(&m_chunkDimension, sizeof(INT32));
		stream->Read(&m_numXChunks, sizeof(INT32));
		stream->Read(&m_numZChunks, sizeof(INT32));
		stream->Read(&m_splatResolution, sizeof(INT32));
		stream->Read(&m_gridSize, sizeof(REAL));
		stream->Read(&m_lodLevels, sizeof(INT32));

		delete stream;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuOptimalTerrain_DataSource_DS::LoadBackdropHeader()
{
	INT32 dummyKey = 0;
	IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_BackdropHeader, reinterpret_cast<BYTE *>(&dummyKey));

	if(stream)
	{
		// Read header information
		stream->Read(&m_backdropOrigin.m_x, sizeof(REAL));
		stream->Read(&m_backdropOrigin.m_y, sizeof(REAL));
		stream->Read(&m_backdropOrigin.m_z, sizeof(REAL));
		stream->Read(&m_backdropDimension, sizeof(INT32));
		stream->Read(&m_backdropGridSpacing, sizeof(REAL));
		stream->Read(&m_backdropXChunks, sizeof(INT32));
		stream->Read(&m_backdropZChunks, sizeof(INT32));

		delete stream;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuOptimalTerrain_DataSource_DS::LoadTextureList(CRuTextureList *textureList)
{
	if(textureList)
	{
		if(m_collisionOnlyMode)
		{
			for(INT32 i = 0; i < 2048; ++i)
			{
				textureList->SetTexture(i, NULL);
			}
		}
		else
		{
			textureList->Load(m_dataStore);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuOptimalTerrain_DataSource_DS::GCChunkQueue()
{
	m_gc_Chunks.Clear();

	// 
	m_chunkHash->ForAllElements(boost::bind(&CRuOptimalTerrain_DataSource_DS::GCChunkQueueCallback, this, _1, _2));

	for(INT32 i = 0; i < m_gc_Chunks.Count(); ++i)
	{
		DWORD chunkKey = m_gc_Chunks[i];
		DWORD chunkKeyHash = RuGlobalHashGen().GetHashCode(chunkKey);
		CRuOptimalTerrain_Chunk *chunk = NULL;

		if(m_chunkHash->Get(chunkKey, chunkKeyHash, chunk))
		{
			// Release chunk
			ruSAFE_RELEASE(chunk);

			// Remove chunk from hash
			m_chunkHash->Remove(chunkKey, chunkKeyHash);
		}
	}

	return TRUE;
}

BOOL CRuOptimalTerrain_DataSource_DS::GCChunkQueueCallback(const void *key, void *data)
{
	CRuOptimalTerrain_Chunk *chunk = *reinterpret_cast<CRuOptimalTerrain_Chunk **>(data);

	INT32 iterationSinceLastUse = m_iteration - chunk->GetUseIteration();

	if(iterationSinceLastUse > 1)
	{
		m_gc_Chunks.Add(chunk->GetChunkKey());
	}

	return TRUE;
}

BOOL CRuOptimalTerrain_DataSource_DS::ClearChunkQueue()
{
	if(m_chunkHash)
	{
		m_chunkHash->ForAllElements(ClearChunkQueueCallback, this);
		m_chunkHash->Clear();
	}

	return TRUE;
}

BOOL CRuOptimalTerrain_DataSource_DS::ClearChunkQueueCallback(const void *key, const void *data, void *userData)
{
	CRuOptimalTerrain_Chunk *chunk = *((CRuOptimalTerrain_Chunk **) data);

	ruSAFE_RELEASE(chunk);

	return TRUE;
}

BOOL CRuOptimalTerrain_DataSource_DS::ClearBackdropQueue()
{
	if(m_backdropQueue)
	{
		m_backdropQueue->ForAllElements(ClearBackdropQueueCallback, this);
		m_backdropQueue->Clear();
	}

	return TRUE;
}

BOOL CRuOptimalTerrain_DataSource_DS::ClearBackdropQueueCallback(const void *key, const void *data, void *userData)
{
	CRuWorld_Backdrop *backdrop = *((CRuWorld_Backdrop **) data);

	ruSAFE_RELEASE(backdrop);

	return TRUE;
}

BOOL CRuOptimalTerrain_DataSource_DS::AttachCollisionStructure(CRuOptimalTerrain_Chunk *chunk)
{
	m_commonCollisionStructure_CS->Enter();

	// Create the common collision structure if it does not already exist
	if(m_commonCollisionStructure == NULL)
	{
		CRuAABB gridBounds = chunk->GetBoundingBox();
		gridBounds.Maxima() = gridBounds.Maxima() - gridBounds.Minima();
		gridBounds.Minima().Set(0.0f, 0.0f, 0.0f);
		gridBounds.Minima().m_y = -ruMAXFLOATRANGE;
		gridBounds.Maxima().m_y = ruMAXFLOATRANGE;

		m_commonCollisionStructure = ruNEW CRuCollisionStructure_PackedGrid();
		m_commonCollisionStructure->BuildCollisionStructureForRegularGrid(gridBounds, GetChunkDimension(), GetChunkDimension());
	}

	// Attach common collision structure to the chunk
	chunk->AttachCollisionStructure(m_commonCollisionStructure);

	m_commonCollisionStructure_CS->Leave();

	return TRUE;
}

BOOL CRuOptimalTerrain_DataSource_DS::DetachCollisionStructure(CRuOptimalTerrain_Chunk *chunk)
{
	m_commonCollisionStructure_CS->Enter();

	chunk->AttachCollisionStructure(NULL);

	m_commonCollisionStructure_CS->Leave();

	return TRUE;
}

CRuOptimalTerrain_Chunk *CRuOptimalTerrain_DataSource_DS::CreateChunk(BOOL minimalChunk)
{
	CRuOptimalTerrain_Chunk *retVal = NULL;

	retVal = ruNEW CRuOptimalTerrain_Chunk();

	return retVal;
}

BOOL CRuOptimalTerrain_DataSource_DS::DestroyChunk(CRuOptimalTerrain_Chunk *chunk, BOOL minimalChunk)
{
	ruSAFE_RELEASE(chunk);

	return TRUE;
}

BOOL CRuOptimalTerrain_DataSource_DS::WorkerFunction()
{
	INT32 chunkKey;
	if(m_chunkWorkQueue->DequeueWork(chunkKey))
	{
		// Build chunk key hash
		DWORD chunkKeyHash = RuGlobalHashGen().GetHashCode(chunkKey);

		// Retrieve the compressed chunk stream from data store
		IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_Chunk, reinterpret_cast<BYTE *>(&chunkKey));

		if(stream)
		{
			// Decompress chunk stream
			m_worker_StreamCompressor->Decompress(stream, 0, stream->GetStreamSize(), m_worker_DecompressedChunkStream);
			delete stream;

			// Construct new chunk from decompressed chunk stream
			m_worker_DecompressedChunkStream->Seek(0, ruSSM_Begin);
			CRuOptimalTerrain_Chunk *chunk = CreateChunk(FALSE);

			if(m_collisionOnlyMode)
				chunk->SerializeFrom(m_worker_DecompressedChunkStream, ruOPTIMALTERRAINCHUNKLOADMODE_COLLISION, m_commonMeshTopology);
			else
				chunk->SerializeFrom(m_worker_DecompressedChunkStream, ruOPTIMALTERRAINCHUNKLOADMODE_FULL, m_commonMeshTopology);

			// Store a reference to the common mesh topology object
			if(m_commonMeshTopology == NULL && chunk->IsMaskedChunk() == FALSE)
			{
				IRuMesh *mesh;
				IRuMaterial *material;
				if(chunk->GetMesh(0, &mesh, &material))
				{
					m_commonMeshTopology = mesh->GetMeshTopology();
					m_commonMeshTopology->AddRef();
				}
			}

			// Set the chunk's chunk key
			chunk->SetChunkKey(chunkKey);

			// Forcibly set the chunk's reflect class
			chunk->SetOption(ruENTITYOPTION_REFLECTCLASS, 3);
			
			// Attach common collision structure to the chunk
			AttachCollisionStructure(chunk);

			// Insert preloaded chunk into preloaded chunks list
			m_chunkWorkQueue->QueueResult(chunkKey, chunk);
		}
		else
		{
			m_chunkWorkQueue->MarkWorkFailed(chunkKey);
		}
	}

	INT32 backdropKey;
	if(m_backdropWorkQueue->DequeueWork(backdropKey))
	{
		// Build backdrop key hash
		DWORD backdropKeyHash = RuGlobalHashGen().GetHashCode(backdropKey);

		// Retrieve the compressed backdrop stream from data store
		IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_Backdrop, reinterpret_cast<BYTE *>(&backdropKey));

		if(stream)
		{
			// Decompress backdrop stream
			m_worker_StreamCompressor->Decompress(stream, 0, stream->GetStreamSize(), m_worker_DecompressedChunkStream);
			delete stream;

			// Construct new backdrop from decompressed backdrop stream
			m_worker_DecompressedChunkStream->Seek(0, ruSSM_Begin);
			CRuWorld_Backdrop *backdrop = ruNEW CRuWorld_Backdrop();
			backdrop->SerializeFrom(m_worker_DecompressedChunkStream, m_commonBackdropMeshTopology);

			// Store a reference to the common backdrop mesh topology object
			if(m_commonBackdropMeshTopology == NULL)
			{
				IRuMesh *mesh;
				IRuMaterial *material;
				if(backdrop->GetMesh(0, &mesh, &material))
				{
					m_commonBackdropMeshTopology = mesh->GetMeshTopology();
					m_commonBackdropMeshTopology->AddRef();
				}
			}

			// Insert preloaded backdrop into preloaded backdrops list
			m_backdropWorkQueue->QueueResult(backdropKey, backdrop);
		}
		else
		{
			m_backdropWorkQueue->MarkWorkFailed(backdropKey);
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuOptimalTerrain::CRuOptimalTerrain()
:	m_enableWaterReflection(TRUE),
	m_enableWaterRefraction(TRUE),
	m_dataSource(NULL),
	m_chunkTree(NULL),
	m_objectContainer(NULL),
	m_objectManager(NULL),
	m_doodadContainer(NULL),
	m_doodadManager(NULL),
	m_zoneDescriptorContainer(NULL),
	m_pathHierarchy(NULL),
	m_enumeratedChunkHash(NULL),
	m_textureList(NULL),
	m_terrainSplatDetailLevel(2),
	m_detailClassLevel(4),
	m_renderTerrain(TRUE),
	m_renderTerrainTexture(TRUE),
	m_renderBackdropTerrain(TRUE),
	m_renderObjects(TRUE),
	m_renderGrass(TRUE),
	m_renderWater(TRUE),
	m_metric_NumCacheMisses(0),
	m_metric_NumBackdropCacheMisses(0),
	m_metric_NumPreloadedChunks(0),
	m_metric_NumPreloadedBackdropChunks(0),
	m_metric_NumEnumeratedEntities(0),
	m_metric_NumChunksLastEnumeration(0)
{
	m_configuredViewDistance = 2000.0f;
	m_viewVolumeMarginSize = 120.0f;
	m_viewVolume.Set(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	m_enumeratedChunkHash = ruNEW CRuHashMap<INT32, CRuOptimalTerrain_Chunk *>(256);

	m_chunkContainer = ruNEW CRuEntity();
	this->AddChild(m_chunkContainer);

	m_backdropChunkContainer = ruNEW CRuEntity();
	this->AddChild(m_backdropChunkContainer);

	m_waterContainer = ruNEW CRuEntity();
	this->AddChild(m_waterContainer);

	// Create a hash map for water body descriptors
	m_waterBodyDescriptorMap = ruNEW CRuHashMap<CRuGUID, CRuWorld_WaterBodyDescriptor *>(1024);

	// Set self as decal target
	this->SetOption(ruENTITYOPTION_DECALTARGET, TRUE);

	// Register listener on engine settings
	g_ruEngineSettings->RegisterSettingsChangeHandler(this->GetGUID(), boost::bind(&CRuOptimalTerrain::EngineSettingsChangedHandler, this, _1, _2));

	// Set up profiler probes
	m_probe_NumCacheMisses = g_ruProfiler->CreateProbe("Chunk Cache Misses", "Terrain");
	m_probe_NumBackdropCacheMisses = g_ruProfiler->CreateProbe("Backdrop Cache Misses", "Terrain");
	m_probe_NumPreloadedChunks = g_ruProfiler->CreateProbe("Chunk Preloads", "Terrain");
	m_probe_NumPreloadedBackdropChunks = g_ruProfiler->CreateProbe("Backdrop Preloads", "Terrain");
	m_probe_CollisionQuery = g_ruProfiler->CreateProbe("Collision Query", "Terrain");

	// Apply settings from the engine
	ApplyEngineSettings();
}

CRuOptimalTerrain::~CRuOptimalTerrain()
{
	// Unregister listener on engine settings
	g_ruEngineSettings->UnregisterSettingsChangeHandler(this->GetGUID());

	delete m_enumeratedChunkHash;

	ruSAFE_RELEASE(m_chunkContainer);
	ruSAFE_RELEASE(m_backdropChunkContainer);
	ruSAFE_RELEASE(m_waterContainer);

	// Release entity container
	ruSAFE_RELEASE(m_objectContainer);
	ruSAFE_RELEASE(m_objectManager);

	// Release doodad container and manager
	ruSAFE_RELEASE(m_doodadContainer);
	ruSAFE_RELEASE(m_doodadManager);

	// Release zone descriptor container
	ruSAFE_RELEASE(m_zoneDescriptorContainer);

	// Release path hierarchy
	ruSAFE_RELEASE(m_pathHierarchy);

	// Release data source
	ruSAFE_RELEASE(m_dataSource);

	// Release chunk tree
	ruSAFE_RELEASE(m_chunkTree);

	// Release backdrop tree
	ruSAFE_RELEASE(m_backdropTree);

	for(INT32 i = 0; i < m_waterBodyDescriptors.Count(); ++i)
	{
		ruSAFE_RELEASE(m_waterBodyDescriptors[i]);
	}

	delete m_waterBodyDescriptorMap;
}

BOOL CRuOptimalTerrain::SerializeFrom(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuOptimalTerrain::SerializeTo(IRuStream *stream)
{
	return TRUE;
}

INT32 CRuOptimalTerrain::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuEntity_Container::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	footprint += sizeof(m_dataSource) + (m_dataSource ? m_dataSource->GetMemoryFootprint(enumHash) : 0);

	footprint += sizeof(m_chunkTree) + (m_chunkTree ? m_chunkTree->GetMemoryFootprint(enumHash) : 0);
	footprint += sizeof(m_backdropTree) + (m_backdropTree ? m_backdropTree->GetMemoryFootprint(enumHash) : 0);

	footprint += sizeof(m_objectContainer) + (m_objectContainer ? m_objectContainer->GetMemoryFootprint(enumHash) : 0);

	return footprint;
}

BOOL CRuOptimalTerrain::ResetStates()
{
	return TRUE;
}

BOOL CRuOptimalTerrain::Update(float elapsedTime)
{
	// Update active water descriptors
	for(INT32 i = 0; i < m_waterBodyDescriptors.Count(); ++i)
	{
		m_waterBodyDescriptors[i]->Update(elapsedTime);
	}

	// Run update on the data source
	m_dataSource->Update(elapsedTime);

	// Calculate view volume center, and flatten it for LOD calculation
	CRuVector3 center = (m_viewVolume.Maxima() + m_viewVolume.Minima()) / 2.0f;
	center.m_y = 0.0f;

	// Perform initial LOD selection
	for(INT32 i = 0; i < m_enumeratedChunks.Count(); ++i)
	{
		if(m_enumeratedChunks[i]->IsMaskedChunk())
		{
			m_enumeratedChunks[i]->SetLOD(0);
		}
		else
		{
			// Flatten the bounding box used for LOD calculation
			CRuAABB boundingBox = m_enumeratedChunks[i]->GetBoundingBox();
			boundingBox.Minima().m_y = 0.0f;
			boundingBox.Maxima().m_y = 0.0f;

			REAL lowLODDistance = 960.0f;
			REAL distanceToChunk = RuSqrt(SquaredDistance(center, boundingBox));

			// Calculate LOD to use based on a linear scale
			REAL t = distanceToChunk / lowLODDistance;
			INT32 LOD = min((INT32) ceil((t * 4) - 0.5f), 4);

			m_enumeratedChunks[i]->SetLOD(LOD);
		}
	}

	// Perform LOD fixup - make sure no chunk has a neighbor whose LOD is more than one level away
	for(INT32 i = 0; i < m_enumeratedChunks.Count(); ++i)
	{
		INT32 defaultLOD = m_enumeratedChunks[i]->GetLOD();

		INT32 chunkKey = m_enumeratedChunks[i]->GetChunkKey();
		DWORD chunkKeyHash = RuGlobalHashGen().GetHashCode(chunkKey);
		INT32 x = (chunkKey >> 16) & 0x0000FFFF;
		INT32 z = chunkKey & 0x0000FFFF;

		INT32 neighborKey_0 = ((x & 0x0000FFFF) << 16) | ((z - 1) & 0x0000FFFF);
		DWORD neighborKeyHash_0 = RuGlobalHashGen().GetHashCode(neighborKey_0);

		INT32 neighborKey_1 = (((x + 1) & 0x0000FFFF) << 16) | (z & 0x0000FFFF);
		DWORD neighborKeyHash_1 = RuGlobalHashGen().GetHashCode(neighborKey_1);

		INT32 neighborKey_2 = ((x & 0x0000FFFF) << 16) | ((z + 1) & 0x0000FFFF);
		DWORD neighborKeyHash_2 = RuGlobalHashGen().GetHashCode(neighborKey_2);

		INT32 neighborKey_3 = (((x - 1) & 0x0000FFFF) << 16) | (z & 0x0000FFFF);
		DWORD neighborKeyHash_3 = RuGlobalHashGen().GetHashCode(neighborKey_3);

		CRuOptimalTerrain_Chunk *neighborChunk = NULL;

		if(m_enumeratedChunkHash->Get(neighborKey_0, neighborKeyHash_0, neighborChunk))
		{
			if(neighborChunk->GetLOD() - defaultLOD > 1)
			{
				neighborChunk->SetLOD(defaultLOD + 1);
				i = -1;
				continue;
			}
		}

		if(m_enumeratedChunkHash->Get(neighborKey_1, neighborKeyHash_1, neighborChunk))
		{
			if(neighborChunk->GetLOD() - defaultLOD > 1)
			{
				neighborChunk->SetLOD(defaultLOD + 1);
				i = -1;
				continue;
			}
		}

		if(m_enumeratedChunkHash->Get(neighborKey_2, neighborKeyHash_2, neighborChunk))
		{
			if(neighborChunk->GetLOD() - defaultLOD > 1)
			{
				neighborChunk->SetLOD(defaultLOD + 1);
				i = -1;
				continue;
			}
		}

		if(m_enumeratedChunkHash->Get(neighborKey_3, neighborKeyHash_3, neighborChunk))
		{
			if(neighborChunk->GetLOD() - defaultLOD > 1)
			{
				neighborChunk->SetLOD(defaultLOD + 1);
				i = -1;
				continue;
			}
		}
	}

	// Select tile links
	for(INT32 i = 0; i < m_enumeratedChunks.Count(); ++i)
	{
		INT32 defaultLOD = m_enumeratedChunks[i]->GetLOD();

		INT32 chunkKey = m_enumeratedChunks[i]->GetChunkKey();
		DWORD chunkKeyHash = RuGlobalHashGen().GetHashCode(chunkKey);
		INT32 x = (chunkKey >> 16) & 0x0000FFFF;
		INT32 z = chunkKey & 0x0000FFFF;

		INT32 neighborKey_0 = ((x & 0x0000FFFF) << 16) | ((z - 1) & 0x0000FFFF);
		DWORD neighborKeyHash_0 = RuGlobalHashGen().GetHashCode(neighborKey_0);

		INT32 neighborKey_1 = (((x + 1) & 0x0000FFFF) << 16) | (z & 0x0000FFFF);
		DWORD neighborKeyHash_1 = RuGlobalHashGen().GetHashCode(neighborKey_1);

		INT32 neighborKey_2 = ((x & 0x0000FFFF) << 16) | ((z + 1) & 0x0000FFFF);
		DWORD neighborKeyHash_2 = RuGlobalHashGen().GetHashCode(neighborKey_2);

		INT32 neighborKey_3 = (((x - 1) & 0x0000FFFF) << 16) | (z & 0x0000FFFF);
		DWORD neighborKeyHash_3 = RuGlobalHashGen().GetHashCode(neighborKey_3);

		CRuOptimalTerrain_Chunk *neighborChunk = NULL;

		INT32 neighborLOD_0 = m_enumeratedChunkHash->Get(neighborKey_0, neighborKeyHash_0, neighborChunk) ? neighborChunk->GetLOD() : defaultLOD;
		INT32 neighborLOD_1 = m_enumeratedChunkHash->Get(neighborKey_1, neighborKeyHash_1, neighborChunk) ? neighborChunk->GetLOD() : defaultLOD;
		INT32 neighborLOD_2 = m_enumeratedChunkHash->Get(neighborKey_2, neighborKeyHash_2, neighborChunk) ? neighborChunk->GetLOD() : defaultLOD;
		INT32 neighborLOD_3 = m_enumeratedChunkHash->Get(neighborKey_3, neighborKeyHash_3, neighborChunk) ? neighborChunk->GetLOD() : defaultLOD;

		INT32 tileMask = (defaultLOD < neighborLOD_0) | ((defaultLOD < neighborLOD_1) << 1) | ((defaultLOD < neighborLOD_2) << 2) | ((defaultLOD < neighborLOD_3) << 3);

		m_enumeratedChunks[i]->SetTileLink(tileMask);
	}

	// Garbage collect the texture list
	m_textureList->GarbageCollect();

	return IRuEntity_Container::Update(elapsedTime);
}

BOOL CRuOptimalTerrain::QueryCollision(CRuCollisionQuery *colQuery)
{
	RUPROFILE_SAMPLE_BEGIN(m_probe_CollisionQuery, 3)

	BOOL retVal = TRUE;

	// Store the inverse of the current world transform into the collision query
	colQuery->m_worldTransform = const_cast<CRuMatrix4x4 *>(&this->GetWorldTransform());
	colQuery->m_invWorldTransform = const_cast<CRuMatrix4x4 *>(&this->GetInverseWorldTransform());

	if(m_chunkTree)
	{
		switch(colQuery->m_queryType)
		{
			case ruCOLQUERY_RAY:
			case ruCOLQUERY_RAY_HITTEST:
			case ruCOLQUERY_RAY_NEAREST:
			case ruCOLQUERY_RAY_PICK:
			case ruCOLQUERY_RAY_TRIPICK:
			case ruCOLQUERY_RAY_SUBOBJECT:
				{
					CRuCollisionQuery_Ray_Base *rayQuery = static_cast<CRuCollisionQuery_Ray_Base *>(colQuery);

					// Get a list intersecting chunks
					m_ws_chunkKeyList.Clear();
					m_ws_chunkBoundsList.Clear();
					m_chunkTree->EnumerateChunks(rayQuery->m_colRay, m_ws_chunkKeyList, &m_ws_chunkBoundsList);

					for(INT32 i = 0; i < m_ws_chunkKeyList.Count(); ++i)
					{
						// Make sure chunk bounds actually intersects the current enumeration bounds
						if(m_standardViewVolume.Intersects(m_ws_chunkBoundsList[i]) == FALSE)
						{
							//OutputDebugString("[TERRAIN] Warning: collision query is outside of enumeration bounds!\n");
							continue;
						}

						// Decode the chunk key
						INT32 x = (m_ws_chunkKeyList[i] >> 16) & 0x0000FFFF;
						INT32 z = m_ws_chunkKeyList[i] & 0x0000FFFF;

						// Load the chunk
						CRuOptimalTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z, ruOPTIMALTERRAINCHUNKLOADMODE_ANY);
						if(chunk)
						{
							// Query ray collision with the chunk
							chunk->QueryCollision(rayQuery);
						}
					}

					// Query collision against object manager
					m_objectManager->QueryCollision(rayQuery);
				}

				break;

			case ruCOLQUERY_SEGMENT:
			case ruCOLQUERY_SEGMENT_HITTEST:
			case ruCOLQUERY_SEGMENT_NEAREST:
				{
					CRuCollisionQuery_Segment_Base *segQuery = static_cast<CRuCollisionQuery_Segment_Base *>(colQuery);

					// Get a list intersecting chunks
					m_ws_chunkKeyList.Clear();
					m_ws_chunkBoundsList.Clear();
					m_chunkTree->EnumerateChunks(segQuery->m_colSegment, m_ws_chunkKeyList, &m_ws_chunkBoundsList);

					for(INT32 i = 0; i < m_ws_chunkKeyList.Count(); ++i)
					{
						// Make sure chunk bounds actually intersects the current enumeration bounds
						if(m_standardViewVolume.Intersects(m_ws_chunkBoundsList[i]) == FALSE)
						{
							//OutputDebugString("[TERRAIN] Warning: collision query is outside of enumeration bounds!\n");
							continue;
						}

						// Decode the chunk key
						INT32 x = (m_ws_chunkKeyList[i] >> 16) & 0x0000FFFF;
						INT32 z = m_ws_chunkKeyList[i] & 0x0000FFFF;

						// Load the chunk
						CRuOptimalTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z, ruOPTIMALTERRAINCHUNKLOADMODE_ANY);
						if(chunk)
						{
							// Query ray collision with the chunk
							chunk->QueryCollision(segQuery);
						}
					}

					// Query collision against object manager
					m_objectManager->QueryCollision(segQuery);
				}

				break;

			case ruCOLQUERY_OBB_TRIENUM:
				{
					CRuCollisionQuery_OBB_Base *obbQuery = static_cast<CRuCollisionQuery_OBB_Base *>(colQuery);

					// Get a list intersecting chunks
					m_ws_chunkKeyList.Clear();
					m_ws_chunkBoundsList.Clear();
					m_chunkTree->EnumerateChunks(obbQuery->m_colOBB, m_ws_chunkKeyList, &m_ws_chunkBoundsList);

					for(INT32 i = 0; i < m_ws_chunkKeyList.Count(); ++i)
					{
						// Make sure chunk bounds actually intersects the current enumeration bounds
						if(m_standardViewVolume.Intersects(m_ws_chunkBoundsList[i]) == FALSE)
						{
							//OutputDebugString("[TERRAIN] Warning: collision query is outside of enumeration bounds!\n");
							continue;
						}

						// Decode the chunk key
						INT32 x = (m_ws_chunkKeyList[i] >> 16) & 0x0000FFFF;
						INT32 z = m_ws_chunkKeyList[i] & 0x0000FFFF;

						// Load the chunk
						CRuOptimalTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z, ruOPTIMALTERRAINCHUNKLOADMODE_ANY);
						if(chunk)
						{
							// Query ray collision with the chunk
							chunk->QueryCollision(obbQuery);
						}
					}

					// Query collision against object manager
					m_objectManager->QueryCollision(obbQuery);
				}

				break;

			case ruCOLQUERY_SPHERE_HITTEST:
			case ruCOLQUERY_SPHERE_TRIENUM:
				{
					CRuCollisionQuery_Sphere_Base *sphereQuery = static_cast<CRuCollisionQuery_Sphere_Base *>(colQuery);

					// Get a list intersecting chunks
					m_ws_chunkKeyList.Clear();
					m_ws_chunkBoundsList.Clear();
					m_chunkTree->EnumerateChunks(sphereQuery->m_colSphere, m_ws_chunkKeyList, &m_ws_chunkBoundsList);

					for(INT32 i = 0; i < m_ws_chunkKeyList.Count(); ++i)
					{
						// Make sure chunk bounds actually intersects the current enumeration bounds
						if(m_standardViewVolume.Intersects(m_ws_chunkBoundsList[i]) == FALSE)
						{
							//OutputDebugString("[TERRAIN] Warning: collision query is outside of enumeration bounds!\n");
							continue;
						}

						// Decode the chunk key
						INT32 x = (m_ws_chunkKeyList[i] >> 16) & 0x0000FFFF;
						INT32 z = m_ws_chunkKeyList[i] & 0x0000FFFF;

						// Load the chunk
						CRuOptimalTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z, ruOPTIMALTERRAINCHUNKLOADMODE_ANY);
						if(chunk)
						{
							// Query ray collision with the chunk
							chunk->QueryCollision(sphereQuery);
						}
					}

					// Query collision against object manager
					m_objectManager->QueryCollision(sphereQuery);
				}

				break;

			case ruCOLQUERY_USER_FLAG_GET_SEGMENT:
				{
					CRuCollisionQuery_User_Flag_Get_Segment *flagQuery = static_cast<CRuCollisionQuery_User_Flag_Get_Segment *>(colQuery);

					// Get a list intersecting chunks
					m_ws_chunkKeyList.Clear();
					m_ws_chunkBoundsList.Clear();
					m_chunkTree->EnumerateChunks(flagQuery->m_colSegment, m_ws_chunkKeyList, &m_ws_chunkBoundsList);

					for(INT32 i = 0; i < m_ws_chunkKeyList.Count(); ++i)
					{
						// Make sure chunk bounds actually intersects the current enumeration bounds
						if(m_standardViewVolume.Intersects(m_ws_chunkBoundsList[i]) == FALSE)
						{
							//OutputDebugString("[TERRAIN] Warning: collision query is outside of enumeration bounds!\n");
							continue;
						}

						// Decode the chunk key
						INT32 x = (m_ws_chunkKeyList[i] >> 16) & 0x0000FFFF;
						INT32 z = m_ws_chunkKeyList[i] & 0x0000FFFF;

						// Load the chunk
						CRuOptimalTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z, ruOPTIMALTERRAINCHUNKLOADMODE_ANY);
						if(chunk)
						{
							// Query ray collision with the chunk
							chunk->QueryCollision(flagQuery);
						}
					}

					// Query collision against instanced entities
//					m_dataSource->GetEntityContainer()->QueryCollision(flagQuery);
				}

				break;
		}
	}

	// Query collision against other child entities
	CRuEntity *curChild = this->GetFirstChild();
	while(curChild)
	{

		if(	curChild != m_chunkContainer &&
			curChild != m_backdropChunkContainer &&
			curChild != m_objectManager &&
			curChild != m_doodadManager &&
			curChild != m_waterContainer)
		{
			curChild->QueryCollision(colQuery);
		}

		curChild = curChild->GetNextSibling();
	}

	RUPROFILE_SAMPLE_END(m_probe_CollisionQuery, 3)

	return retVal;
}

BOOL CRuOptimalTerrain::Initialize()
{
	return TRUE;
}

BOOL CRuOptimalTerrain::AttachDataSource(IRuOptimalTerrain_DataSource *dataSource, BOOL enableCollisionData)
{
	// Release previous data source
	ruSAFE_RELEASE(m_dataSource);

	// Release previously allocated data



	// Save options
	m_enableCollisionData = enableCollisionData;

	// Attach to new data source
	m_dataSource = dataSource;
	m_dataSource->AddRef();

	// Get terrain dimensions
	INT32 numXChunks = m_dataSource->GetNumXChunks();
	INT32 numZChunks = m_dataSource->GetNumZChunks();
	INT32 totalChunks = numXChunks * numZChunks;

	// Build the maximal bounding box
	CRuAABB spatialBounds(m_dataSource->GetBounds(0, 0));
	for(INT32 z = 0; z < numZChunks; ++z)
	{
		for(INT32 x = 0; x < numXChunks; ++x)
		{
			spatialBounds.Merge(m_dataSource->GetBounds(x, z));
		}
	}

	m_terrainBounds = spatialBounds;

	// Force total chunks to be a power of 2
	totalChunks = RoundUpToPow2(totalChunks);

	// Build chunk partitioning structure
	m_chunkTree = ruNEW CRuWorld_ChunkTree();
	m_chunkTree->Initialize(spatialBounds, 32);

	// Insert chunk descriptors into partitioning structure
	for(INT32 z = 0; z < numZChunks; ++z)
	{
		for(INT32 x = 0; x < numXChunks; ++x)
		{
			m_chunkTree->InsertChunk(x, z, m_dataSource->GetBounds(x, z));
		}
	}

	// Build backdrop partitioning tree
	INT32 numXBackdrops = m_dataSource->GetNumBackdropXChunks();
	INT32 numZBackdrops = m_dataSource->GetNumBackdropZChunks();

	m_backdropTree = ruNEW CRuWorld_ChunkTree();
	m_backdropTree->Initialize(m_dataSource->GetAllBackdropBounds(), 16);

	for(INT32 z = 0; z < numZBackdrops; ++z)
	{
		for(INT32 x = 0; x < numXBackdrops; ++x)
		{
			m_backdropTree->InsertChunk(x, z, m_dataSource->GetBackdropBounds(x, z));
		}
	}

	// Get texture list from data source
	m_textureList = dataSource->GetTextureList();

	// Load water body descriptors
	INT32 numWaterBodyDescriptors = m_dataSource->GetNumWaterBodyDescriptors();
	for(INT32 i = 0; i < numWaterBodyDescriptors; ++i)
	{
		CRuWorld_WaterBodyDescriptor *waterBodyDescriptor = m_dataSource->LoadWaterBodyDescriptorByIndex(i);

		if(waterBodyDescriptor)
		{
			// Append descriptor to list
			m_waterBodyDescriptors.Add(waterBodyDescriptor);

			// Map water body descriptor
			m_waterBodyDescriptorMap->Add(waterBodyDescriptor->m_guid, waterBodyDescriptor->m_guid.HashCode(), waterBodyDescriptor);
		}
	}

	// Construct entity container
	m_objectContainer = ruNEW CRuWorld_ObjectContainer(m_terrainBounds, m_dataSource->GetGridSize() * m_dataSource->GetChunkDimension() * ruWORLD_OBJECTCELLMULTIPLIER);
	m_objectContainer->Attach(static_cast<CRuOptimalTerrain_DataSource_DS *>(m_dataSource)->GetDataStore());

	// Construct entity manager
	m_objectManager = ruNEW CRuWorld_ObjectManager();
	m_objectManager->Attach(m_objectContainer);

	// Construct doodad container
	m_doodadContainer = ruNEW CRuWorld_DoodadContainer();
	m_doodadContainer->Attach(static_cast<CRuOptimalTerrain_DataSource_DS *>(m_dataSource)->GetDataStore(), FALSE);

	// Construct doodad manager
	m_doodadManager = ruNEW CRuWorld_DoodadManager();
	m_doodadManager->Attach(m_doodadContainer);
	m_doodadManager->SetEnumerator(boost::bind(&CRuOptimalTerrain::GetDoodadEnumeration, this, _1, _2));

	// Construct zone descriptor container
	m_zoneDescriptorContainer = ruNEW CRuWorld_ZoneDescriptorContainer();
	m_zoneDescriptorContainer->Attach(static_cast<CRuOptimalTerrain_DataSource_DS *>(m_dataSource)->GetDataStore());

	// Construct path hierarchy
	m_pathHierarchy = ruNEW CRuPathHierarchy_DS();
	m_pathHierarchy->Bind(static_cast<CRuOptimalTerrain_DataSource_DS *>(m_dataSource)->GetDataStore(), m_terrainBounds);

	// Apply relevant engine settings
	ApplyEngineSettings();

	return TRUE;
}

BOOL CRuOptimalTerrain::PreLoadChunks(const CRuAABB &bounds, RuOptimalTerrainChunkLoadMode chunkLoadMode)
{
	if(m_chunkTree)
	{
		// Give global worker thread maximum allocation
//		g_ruGlobalWorkerThread->SetSleepPeriod(0);

		// Remove all children
		m_chunkContainer->RemoveAllChildren();

		// Clear enumeration cache
		this->ClearEnumerationCache();

		// Store bounds as the enumeration bounds
		m_standardViewVolume = bounds;

		// Enumerate keys of chunks that intersect the specified bounds
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(bounds, chunkKeyList);

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Load chunk with specified chunk load method
			CRuOptimalTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z, chunkLoadMode);

			// Load forever...
			while(chunk == NULL)
			{
				Sleep(5);
				m_dataSource->Update(0.0f);
				chunk = m_dataSource->LoadChunk(x, z, chunkLoadMode);
			}

			if(chunk)
			{
				m_chunkContainer->AddChild(chunk);

				// Add to chunk list
				m_enumeratedChunks.Add(chunk);

				// Iterate through doodad lists
				for(INT32 j = 0; j < chunk->GetNumDoodadClusters(); ++j)
				{
					m_enumeratedDoodadKeys.Add(chunkKeyList[i]);
					m_enumeratedDoodadClusters.Add(chunk->GetDoodadCluster(j));
				}

				// Iterate through water layers
				for(INT32 j = 0; j < chunk->GetNumWaterLayers(); ++j)
				{
					CRuWorld_Water *waterLayer = chunk->GetWaterLayerByIndex(j);

					// Update collision flags on water layer
					CRuWorld_WaterBodyDescriptor *descriptor = NULL;
					if(m_waterBodyDescriptorMap->Get(waterLayer->GetWaterBodyGUID(), waterLayer->GetWaterBodyGUID().HashCode(), descriptor))
					{
						waterLayer->UpdateCollisionFlags(descriptor->m_bodyType);
					}

					// Add to water container
					if(descriptor && descriptor->m_bodyType != RuWorld::ruWATERBODYTYPE_EXTENTMARKER)
					{
						m_waterContainer->AddChild(waterLayer);
					}
				}
			}
		}

		// Update entity enumeration
		m_objectManager->UpdateEnumeration(bounds);

		// Update object manager until all objects are loaded
		while(m_objectManager->GetLoadingStatus() < 1.0f)
		{
			// Update global function schedule
			g_ruFunctionSchedule->Update();

			m_objectManager->Update(0.0f);
			Sleep(10);
		}

		// Restore global worker thread default allocation
//		g_ruGlobalWorkerThread->SetSleepPeriod(25);
	}

	return TRUE;
}

REAL CRuOptimalTerrain::GetChunkLoadingStatus()
{
	return m_metric_NumChunksLastEnumeration ? 1.0f - ((REAL) m_chunkWorkQueue.Count() / m_metric_NumChunksLastEnumeration) : 1.0f;
}

REAL CRuOptimalTerrain::GetLoadingStatus()
{
	INT32 loadingStageCount = 4;
	REAL loadingStageStatus[4];

	loadingStageStatus[0] = m_metric_NumChunksLastEnumeration ? 1.0f - ((REAL) m_chunkWorkQueue.Count() / m_metric_NumChunksLastEnumeration) : 1.0f;
	loadingStageStatus[1] = m_objectManager->GetLoadingStatus();
	loadingStageStatus[2] = g_ruResourceManager->IsObjectWorkQueueEmpty() ? 1.0f : 0.0f;
	loadingStageStatus[3] = g_ruResourceManager->IsTextureWorkQueueEmpty() ? 1.0f : 0.0f;
/*
	char moo[200];
	sprintf(moo, "%d %f %f %f %f\n", m_metric_NumChunksLastEnumeration, loadingStageStatus[0], loadingStageStatus[1], loadingStageStatus[2], loadingStageStatus[3]);
	OutputDebugString(moo);
*/
	REAL loadingStageFactor = 1.0f / loadingStageCount;
	REAL baseLoadingStageFactor = 0.0f;

	for(INT32 i = 0; i < loadingStageCount; ++i)
	{
		if(loadingStageStatus[i] < 1.0f)
		{
			return baseLoadingStageFactor + (loadingStageFactor * loadingStageStatus[i]);
		}

		baseLoadingStageFactor += loadingStageFactor;
	}

	return 1.0f;
}

BOOL CRuOptimalTerrain::ModifyViewDistanceConfiguration(REAL viewDistance)
{
	// Check the current enumeration configuration
	if(fabs(viewDistance - m_configuredViewDistance) > 1.0f)
	{
		// Clear the enumeration cache
		ClearEnumerationCache();

		// Store new configured view distance
		m_configuredViewDistance = viewDistance;
	}

	return TRUE;
}

void CRuOptimalTerrain::SetMovementDirection(const CRuVector3 &direction)
{
	m_movementDirection = direction;
}

void CRuOptimalTerrain::SetCamera(CRuCamera *viewCamera)
{
	if(m_chunkTree == NULL)
		return;

	BOOL enumerationCacheCleared = FALSE;
	BOOL doodadEnumerationChanged = FALSE;
	BOOL chunkEnumerationUpdated = FALSE;

	CRuAABB normalEnumerationBounds = viewCamera->GetSweepBounds();
	normalEnumerationBounds.Minima().m_y = -100000.0f;
	normalEnumerationBounds.Maxima().m_y = +100000.0f;

	m_standardViewVolume = normalEnumerationBounds;

	CRuAABB backdropEnumerationBounds;
	if(viewCamera->IsBackdropEnabled())
	{
		backdropEnumerationBounds = viewCamera->GetBackdropSweepBounds();
	}
	else
	{
		backdropEnumerationBounds = viewCamera->GetSweepBounds();
	}
	backdropEnumerationBounds.Minima().m_y = -100000.0f;
	backdropEnumerationBounds.Maxima().m_y = +100000.0f;

	REAL grassThreshold = 300.0f * 300.0f;

	INT32 chunkDimension = m_dataSource->GetChunkDimension();

/*
	DWORD t0, t1, et;
	t0 = GetTickCount();
*/

	// Enumerate only if the view volume has changed significantly
	if(m_viewVolume.Contains(backdropEnumerationBounds) == FALSE)
	{
/*
		char moo[2000];
		sprintf(moo, "Camera pos: %f, %f, %f\n", viewCamera->GetCameraPosition().m_x, viewCamera->GetCameraPosition().m_y, viewCamera->GetCameraPosition().m_z);
		OutputDebugString(moo);
*/
		enumerationCacheCleared = TRUE;
		doodadEnumerationChanged = TRUE;

		// Clear enumeration cache
		this->ClearEnumerationCache();

		// Clear work queue and increment iteration
		m_dataSource->ClearWorkQueues();
		m_dataSource->IncrementIteration();

		// Set view volume to the camera sweep bounds plus margin
		m_viewVolume = backdropEnumerationBounds;
		m_viewVolume.Minima().m_x -= m_viewVolumeMarginSize;
		m_viewVolume.Minima().m_y -= m_viewVolumeMarginSize;
		m_viewVolume.Minima().m_z -= m_viewVolumeMarginSize;
		m_viewVolume.Maxima().m_x += m_viewVolumeMarginSize;
		m_viewVolume.Maxima().m_y += m_viewVolumeMarginSize;
		m_viewVolume.Maxima().m_z += m_viewVolumeMarginSize;

		normalEnumerationBounds.Minima().m_x -= m_viewVolumeMarginSize;
		normalEnumerationBounds.Minima().m_y -= m_viewVolumeMarginSize;
		normalEnumerationBounds.Minima().m_z -= m_viewVolumeMarginSize;
		normalEnumerationBounds.Maxima().m_x += m_viewVolumeMarginSize;
		normalEnumerationBounds.Maxima().m_y += m_viewVolumeMarginSize;
		normalEnumerationBounds.Maxima().m_z += m_viewVolumeMarginSize;

		m_movementDirection.m_y = 0.0f;
		if(m_movementDirection.Magnitude() > ruEPSILON)
			m_movementDirection.Normalize();

		CRuAABB xPaddingVolume(normalEnumerationBounds);
		CRuAABB zPaddingVolume(normalEnumerationBounds);

		CRuAABB xBackdropPaddingVolume(m_viewVolume);
		CRuAABB zBackdropPaddingVolume(m_viewVolume);

		if(m_movementDirection.m_x < 0.0f)
		{
			xPaddingVolume.Maxima().m_x = xPaddingVolume.Minima().m_x;
			xPaddingVolume.Minima().m_x -= 960.0f;

			zPaddingVolume.Minima().m_x -= 960.0f;

			xBackdropPaddingVolume.Maxima().m_x = xBackdropPaddingVolume.Minima().m_x;
			xBackdropPaddingVolume.Minima().m_x -= 960.0f;

			zBackdropPaddingVolume.Minima().m_x -= 960.0f;
		}
		else
		{
			xPaddingVolume.Minima().m_x = xPaddingVolume.Maxima().m_x;
			xPaddingVolume.Maxima().m_x += 960.0f;

			zPaddingVolume.Minima().m_x += 960.0f;

			xBackdropPaddingVolume.Minima().m_x = xBackdropPaddingVolume.Maxima().m_x;
			xBackdropPaddingVolume.Maxima().m_x += 960.0f;

			zBackdropPaddingVolume.Minima().m_x += 960.0f;
		}

		if(m_movementDirection.m_z < 0.0f)
		{
			xPaddingVolume.Minima().m_z -= 960.0f;

			zPaddingVolume.Maxima().m_z = zPaddingVolume.Minima().m_z;
			zPaddingVolume.Minima().m_z -= 960.0f;

			xBackdropPaddingVolume.Minima().m_z -= 960.0f;

			zBackdropPaddingVolume.Maxima().m_z = zBackdropPaddingVolume.Minima().m_z;
			zBackdropPaddingVolume.Minima().m_z -= 960.0f;
		}
		else
		{
			xPaddingVolume.Minima().m_z += 960.0f;

			zPaddingVolume.Minima().m_z = zPaddingVolume.Maxima().m_z;
			zPaddingVolume.Maxima().m_z += 960.0f;

			xBackdropPaddingVolume.Minima().m_z += 960.0f;

			zBackdropPaddingVolume.Minima().m_z = zBackdropPaddingVolume.Maxima().m_z;
			zBackdropPaddingVolume.Maxima().m_z += 960.0f;
		}

		{
			// Get a list of keys to visible chunks
			CRuArrayList<DWORD> chunkKeyList;
			m_chunkTree->EnumerateChunks(xPaddingVolume, chunkKeyList);
			m_chunkTree->EnumerateChunks(zPaddingVolume, chunkKeyList);

			// Load the chunks (data source takes care of background loading)
			for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
			{
				INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
				INT32 z = chunkKeyList[i] & 0x0000FFFF;

				m_dataSource->LoadChunk(x, z, ruOPTIMALTERRAINCHUNKLOADMODE_FULL);
			}
		}

		{
			// Get a list of keys to visible chunks
			CRuArrayList<DWORD> backdropKeyList;
			m_backdropTree->EnumerateChunks(xPaddingVolume, backdropKeyList);
			m_backdropTree->EnumerateChunks(zPaddingVolume, backdropKeyList);

			// Load the backdrops (data source takes care of background loading)
			for(INT32 i = 0; i < backdropKeyList.Count(); ++i)
			{
				INT32 x = (backdropKeyList[i] >> 16) & 0x0000FFFF;
				INT32 z = backdropKeyList[i] & 0x0000FFFF;

				m_dataSource->LoadBackdrop(x, z);
			}
		}

		// Remove all children
		m_chunkContainer->RemoveAllChildren();
		m_backdropChunkContainer->RemoveAllChildren();

		// Get a list of keys to visible chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(m_viewVolume, chunkKeyList);

		// Queue visible chunks
		m_chunkWorkQueue.Clear();

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			if(!m_dataSource->ChunkLoadFailed(chunkKeyList[i]))
			{
				m_chunkWorkQueue.Queue(chunkKeyList[i]);
			}
		}

		// Get a list of keys to visible backdrops
		CRuArrayList<DWORD> backdropKeyList;
		m_backdropTree->EnumerateChunks(backdropEnumerationBounds, backdropKeyList);

		// Queue visible backdrops
		m_backdropWorkQueue.Clear();

		for(INT32 i = 0; i < backdropKeyList.Count(); ++i)
		{
			if(!m_dataSource->BackdropLoadFailed(backdropKeyList[i]))
			{
				m_backdropWorkQueue.Queue(backdropKeyList[i]);
			}
		}
	}

	// Work through the chunk work queue
	for(INT32 i = 0, queueItemCount = m_chunkWorkQueue.Count(); i < queueItemCount; ++i)
	{
		// Decode the chunk key
		DWORD chunkKey = m_chunkWorkQueue.Dequeue();
		DWORD chunkKeyHash = RuGlobalHashGen().GetHashCode(chunkKey);
		INT32 x = (chunkKey >> 16) & 0x0000FFFF;
		INT32 z = chunkKey & 0x0000FFFF;

		// Get chunk bounds
		const CRuAABB &chunkBounds = m_dataSource->GetBounds(x, z);

		BOOL isNormalChunk = Intersects(normalEnumerationBounds, chunkBounds);

		// If the chunk intersects the normal view sweep bounds, process it as a normal chunk
		if(isNormalChunk)
		{
			CRuOptimalTerrain_Chunk *chunk = NULL;
			IRuMesh *mesh = NULL;
			IRuMaterial *material = NULL;

			// Load chunk with full splat data
			chunk = m_dataSource->LoadChunk(x, z, ruOPTIMALTERRAINCHUNKLOADMODE_FULL);

			if(chunk)
			{
				if(chunk->GetParent() == NULL)
				{
					// Set detail level
					switch(m_terrainSplatDetailLevel)
					{
						case 0:
							chunk->SetAlphaTextureLOD(3);
							break;

						case 1:
							chunk->SetAlphaTextureLOD(2);
							break;

						case 2:
							chunk->SetAlphaTextureLOD(1);
							break;

						case 3:
						default:
							chunk->SetAlphaTextureLOD(0);
							break;
					}

					chunk->GetMesh(0, &mesh, &material);

					// Build alpha maps
					CRuSplatIDMap *idMap = chunk->GetSplatIDMap();

					if(idMap)
					{
						idMap->FinishAlphaMapLoad();

						// Setup data references
						INT32 numAlphaMaps = idMap->GetNumAlphaMaps();
						INT32 numLayers = idMap->GetNumLayers();

						// Set number of layers
						material->GetParameterBlock()->SetParameter(ruPARAMTYPE_SPLAT_LAYERCOUNT, numLayers);

						// Set alpha maps
						IRuTexture *blackTexture = g_ruResourceManager->LoadTexture("RUSYSTEM_BLACK");

						for(INT32 j = 0; j < 2; ++j)
						{
							IRuTexture *alphaTexture = idMap->GetAlphaMap(j);
							material->SetTexture((RuTextureChannel) (ruTEXCHANNEL_TERRAIN_ALPHA_0 + j), alphaTexture ? alphaTexture : blackTexture);
						}

						ruSAFE_RELEASE(blackTexture);

						// Set layer textures
						for(INT32 j = 0; j < numLayers; ++j)
						{
							INT32 textureID = idMap->GetLayerID(j);
							chunk->SetTexture(j, m_textureList->GetTexture(textureID), m_textureList->GetTileScale(textureID));
						}
					}
					else
					{
						// Set number of layers
						material->GetParameterBlock()->SetParameter(ruPARAMTYPE_SPLAT_LAYERCOUNT, 0);
					}

					if(m_renderTerrainTexture == FALSE)
					{
						material->GetParameterBlock()->SetParameter(ruPARAMTYPE_SPLAT_LAYERCOUNT, 0);
					}

					// Add chunk to the chunk container
					m_chunkContainer->AddChild(chunk);

					// Add to chunk list
					m_enumeratedChunkHash->Add(chunkKey, chunkKeyHash, chunk);
					m_enumeratedChunks.Add(chunk);

					// If the chunk has doodad lists, work through them
					if(chunk->GetNumDoodadClusters() > 0)
					{
						// Mark doodad enumeration as having changed
						doodadEnumerationChanged = TRUE;

						// Iterate through doodad lists and add them to the list of enumerated doodad lists
						for(INT32 j = 0; j < chunk->GetNumDoodadClusters(); ++j)
						{
							m_enumeratedDoodadKeys.Add(chunkKey);
							m_enumeratedDoodadClusters.Add(chunk->GetDoodadCluster(j));
						}
					}

					chunkEnumerationUpdated = TRUE;
				}
			}
			else
			{
				if(!m_dataSource->ChunkLoadFailed(chunkKey))
				{
					m_chunkWorkQueue.Queue(chunkKey);
				}
			}
		}
	}

	// Go through the backdrop work queue
	for(INT32 i = 0, queueItemCount = m_backdropWorkQueue.Count(); i < queueItemCount; ++i)
	{
		// Decode the backdrop key
		DWORD backdropKey = m_backdropWorkQueue.Dequeue();
		INT32 x = (backdropKey >> 16) & 0x0000FFFF;
		INT32 z = backdropKey & 0x0000FFFF;

		CRuWorld_Backdrop *backdrop = m_dataSource->LoadBackdrop(x, z);

		if(backdrop)
		{
			backdrop->SetLOD(1);
			RuEntity_SetOption(backdrop, ruENTITYOPTION_BACKDROP, TRUE);
			m_backdropChunkContainer->AddChild(backdrop);
		}
		else
		{
			if(!m_dataSource->BackdropLoadFailed(backdropKey))
			{
				m_backdropWorkQueue.Queue(backdropKey);
			}
		}
	}

	if(enumerationCacheCleared)
	{
		// Store the number of chunks enumerated during this enumeration
		m_metric_NumChunksLastEnumeration = m_chunkWorkQueue.Count();
	}

	// Clear grass and water containers
//	m_grassContainer->RemoveAllChildren();
	m_waterContainer->RemoveAllChildren();

	for(INT32 i = 0; i < m_enumeratedChunks.Count(); ++i)
	{
		CRuOptimalTerrain_Chunk *chunk = m_enumeratedChunks[i];
		if(chunk)
		{
/*
			REAL sqrDistanceToChunk = SquaredDistance(viewCamera->GetCameraPosition(), chunk->GetBoundingBox());

			// Display grass system
			if(sqrDistanceToChunk < grassThreshold && chunk->GetGrassParticles().Count() > 0)
			{
				DWORD key = (DWORD) chunk;
				DWORD keyHash = RuGlobalHashGen().GetHashCode(key);
				if(m_grassQueue->ContainsKey(key, keyHash) == FALSE)
				{
					// Create new grass particle system
					CRuEntity_Particle_Grass_Optimized *grassSystem = ruNEW CRuEntity_Particle_Grass_Optimized();
					grassSystem->SetGrassDatabase(m_grassDatabase);
					grassSystem->SetParticleList(chunk->GetGrassParticles());
					grassSystem->SetLocalEntityBounds(chunk->GetLocalEntityBounds());

					CRuEntity_Particle_Grass_Optimized *reclaimedGrassSystem;
					while(m_grassQueue->Reclaim(key, keyHash, 1, reclaimedGrassSystem))
					{
						// Release the chunk
						ruSAFE_RELEASE(reclaimedGrassSystem);
					}

					m_grassQueue->Add(key, keyHash, grassSystem, 1);
				}

				m_grassContainer->AddChild(m_grassQueue->Get(key, keyHash));
			}
*/
		}

		// Display water layers
		for(INT32 j = 0; j < chunk->GetNumWaterLayers(); ++j)
		{
			CRuWorld_Water *waterLayer = chunk->GetWaterLayerByIndex(j);

			// Override material
			CRuWorld_WaterBodyDescriptor *descriptor = NULL;
			if(m_waterBodyDescriptorMap->Get(waterLayer->GetWaterBodyGUID(), waterLayer->GetWaterBodyGUID().HashCode(), descriptor))
			{
				waterLayer->SetWaterLayerMaterial(descriptor->GetWaterBodyMaterial());

				waterLayer->UpdateCollisionFlags(descriptor->m_bodyType);
			}

			// Show water body only if it has a descriptor and is of a normal body type
			if(descriptor && descriptor->m_bodyType != RuWorld::ruWATERBODYTYPE_EXTENTMARKER)
			{
				m_waterContainer->AddChild(waterLayer);
			}
		}
	}

	// Update entity enumeration
/*	if(chunkEnumerationUpdated)
	{
		m_objectManager->ClearEnumerationCache();
	}*/
	m_objectManager->SetDetailClass(m_detailClassLevel);
	m_objectManager->UpdateEnumeration(viewCamera->GetSweepBounds());

	// Update doodad enumeration
	if(doodadEnumerationChanged)
	{
		m_doodadManager->ClearEnumerationCache();
	}
	m_doodadManager->UpdateEnumeration(viewCamera->GetSweepBounds());

	// Attach/detach sub entities based on visibility options
	this->AddChild(m_chunkContainer);
	this->AddChild(m_backdropChunkContainer);
	this->AddChild(m_objectManager);
	this->AddChild(m_doodadManager);
	this->AddChild(m_waterContainer);

	BOOL posIsIndoor = m_objectManager->CameraPositionIsIndoor();

	(m_renderTerrain && !posIsIndoor) ? m_chunkContainer->SetOption(ruENTITYOPTION_HIDDEN, FALSE) : m_chunkContainer->SetOption(ruENTITYOPTION_HIDDEN, TRUE);
	(m_renderBackdropTerrain && !posIsIndoor) ? m_backdropChunkContainer->SetOption(ruENTITYOPTION_HIDDEN, FALSE) : m_backdropChunkContainer->SetOption(ruENTITYOPTION_HIDDEN, TRUE);
	m_renderObjects ? m_objectManager->SetOption(ruENTITYOPTION_HIDDEN, FALSE) : m_objectManager->SetOption(ruENTITYOPTION_HIDDEN, TRUE);
	m_renderGrass ? m_doodadManager->SetOption(ruENTITYOPTION_HIDDEN, FALSE) : m_doodadManager->SetOption(ruENTITYOPTION_HIDDEN, TRUE);
	m_renderWater ? m_waterContainer->SetOption(ruENTITYOPTION_HIDDEN, FALSE) : m_waterContainer->SetOption(ruENTITYOPTION_HIDDEN, TRUE);

	// Store probe values
	g_ruProfiler->SetProbeValue(m_probe_NumCacheMisses, static_cast<REAL>(m_metric_NumCacheMisses));
	g_ruProfiler->SetProbeValue(m_probe_NumBackdropCacheMisses, static_cast<REAL>(m_metric_NumBackdropCacheMisses));
	g_ruProfiler->SetProbeValue(m_probe_NumPreloadedChunks, static_cast<REAL>(m_metric_NumPreloadedChunks));
	g_ruProfiler->SetProbeValue(m_probe_NumPreloadedBackdropChunks, static_cast<REAL>(m_metric_NumPreloadedBackdropChunks));

/*
	t1 = GetTickCount();
//	if(enumerationCacheCleared)
	{
		et = t1 - t0;
		char moo[200];
		sprintf(moo, "Terrain Enum: %d\n", et);
		OutputDebugString(moo);
	}
*/
}

const CRuAABB &CRuOptimalTerrain::GetTerrainBounds() const
{
	return m_terrainBounds;
}

const CRuAABB &CRuOptimalTerrain::GetEnumerationBounds() const
{
	return m_standardViewVolume;
}

INT32 CRuOptimalTerrain::GetNumXChunks() const
{
	return m_dataSource->GetNumXChunks();
}

INT32 CRuOptimalTerrain::GetNumZChunks() const
{
	return m_dataSource->GetNumZChunks();
}

BOOL CRuOptimalTerrain::ClearEnumerationCache()
{
	// Reset view volume
	m_viewVolume.Set(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	// Clear internal enumerations
	m_enumeratedChunkHash->Clear();
	m_enumeratedChunks.Clear();
	m_enumeratedDoodadKeys.Clear();
	m_enumeratedDoodadClusters.Clear();

	m_chunkWorkQueue.Clear();
	m_backdropWorkQueue.Clear();

	// Clear external enumerations
	m_objectManager->ClearEnumerationCache();
	m_doodadManager->ClearEnumerationCache();

	return TRUE;
}

INT32 CRuOptimalTerrain::IsChunkLoadedAt(const CRuVector3 &point)
{
	// Build unit sphere at test point
	CRuSphere testSphere(point, 1.0f);

	// Get a list intersecting chunks
	CRuArrayList<DWORD> chunkKeyList;
	m_chunkTree->EnumerateChunks(testSphere, chunkKeyList);

	for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
	{
		// Decode the chunk key
		INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
		INT32 z = chunkKeyList[i] & 0x0000FFFF;

		// Load the chunk
		CRuOptimalTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z, ruOPTIMALTERRAINCHUNKLOADMODE_ANY);
		if(chunk == NULL)
		{
			return FALSE;
		}
	}

	return TRUE;
}

DWORD CRuOptimalTerrain::GetChunkKeyAt(const CRuVector3 &point)
{
	// Build unit sphere at test point
	CRuSphere testSphere(point, 1.0f);

	// Get a list intersecting chunks
	CRuArrayList<DWORD> chunkKeyList;
	m_chunkTree->EnumerateChunks(testSphere, chunkKeyList);

	for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
	{
		return chunkKeyList[i];
	}

	return 0xFFFFFFFF;
}

BOOL CRuOptimalTerrain::EnumerateTriggers(RuWorld::TriggerType triggerType, CRuArrayList<CRuWorld_Trigger *> &triggersOut)
{
	// Get all enumerated triggers
	CRuArrayList<IRuObject *> triggers;
	m_objectManager->EnumerateVisibleObjectsByType(triggers, CRuWorld_Trigger::Type());

	for(INT32 i = 0; i < triggers.Count(); ++i)
	{
		CRuWorld_Trigger *trigger = static_cast<CRuWorld_Trigger *>(triggers[i]);

		// Process only the specified trigger type
		if(trigger->GetTriggerType() == triggerType)
		{
			triggersOut.Add(trigger);
		}
	}

	return TRUE;
}

BOOL CRuOptimalTerrain::GetTriggers(const CRuVector3 &point, CRuArrayList<CRuWorld_Trigger *> &triggersOut)
{
	REAL nearestDistance = FLT_MAX;
	CRuVector3 nearestPoint;
	CRuWorld_Trigger *nearestTrigger = NULL;

	// Build common collision query
	CRuCollisionQuery_Segment_Nearest segQuery;
	CRuMatrix4x4 identityMatrix;
	segQuery.m_worldTransform = &identityMatrix;
	segQuery.m_invWorldTransform = &identityMatrix;
	segQuery.m_ignoreCollisionFlag = TRUE;

	// Build query segment
	CRuSegment3 colSeg(CRuVector3(point.m_x, -10000.0f, point.m_z), CRuVector3(point.m_x, 10000.0f, point.m_z));
	segQuery.m_colSegment = colSeg;

	// Get all enumerated triggers
	CRuArrayList<IRuObject *> triggers;
	m_objectContainer->EnumerateVisibleObjectsByType(triggers, CRuWorld_Trigger::Type());

	for(INT32 i = 0; i < triggers.Count(); ++i)
	{
		CRuWorld_Trigger *trigger = static_cast<CRuWorld_Trigger *>(triggers[i]);

		// Make query
		segQuery.m_positiveHit = FALSE;
		segQuery.m_collisionT = FLT_MAX;

		trigger->QueryCollision(&segQuery);

		if(segQuery.m_positiveHit)
		{
			triggersOut.Add(trigger);
		}
	}

	return TRUE;
}

CRuWorld_Trigger *CRuOptimalTerrain::GetApplicableTrigger(RuWorld::TriggerType triggerType, const CRuVector3 &point, BOOL limitRange)
{
	REAL nearestDistance = FLT_MAX;
	CRuVector3 nearestPoint;
	CRuWorld_Trigger *nearestTrigger = NULL;
	INT32 nearestTriggerPriority = 0xFFFFFFFF;

	// Build common collision query
	CRuCollisionQuery_Segment_Nearest segQuery;
	CRuMatrix4x4 identityMatrix;
	segQuery.m_worldTransform = &identityMatrix;
	segQuery.m_invWorldTransform = &identityMatrix;
	segQuery.m_ignoreCollisionFlag = TRUE;

	// Build query segments
	CRuSegment3 downwardSegment(point, point + CRuVector3(0.0f, limitRange ? -5.0f : -10000.0f, 0.0f));
	CRuSegment3 upSegment(point, point + CRuVector3(0.0f, limitRange ? 5.0f : 10000.0f, 0.0f));

	// Get all enumerated triggers
	CRuArrayList<IRuObject *> triggers;
	m_objectContainer->EnumerateVisibleObjectsByType(triggers, CRuWorld_Trigger::Type());

	for(INT32 i = 0; i < triggers.Count(); ++i)
	{
		CRuWorld_Trigger *trigger = static_cast<CRuWorld_Trigger *>(triggers[i]);

		// Process only the specified trigger type
		if(trigger->GetTriggerType() == triggerType)
		{
			// Get trigger priority
			RuPropertyType propertyValue;
			INT32 priority  = trigger->GetProperty("CRuWorld_Trigger:Priority", propertyValue) ? boost::get<INT32>(propertyValue) : 0;

			// Make query downwards
			segQuery.m_positiveHit = FALSE;
			segQuery.m_collisionT = FLT_MAX;
			segQuery.m_colSegment = downwardSegment;

			trigger->QueryCollision(&segQuery);

			if(segQuery.m_positiveHit)
			{
				if(	priority > nearestTriggerPriority ||
					(priority == nearestTriggerPriority && (segQuery.m_collisionT < nearestDistance || (segQuery.m_collisionT == nearestDistance && IsSubZoneOf(trigger->GetZoneID(), nearestTrigger->GetZoneID())))))
				{
					nearestDistance = segQuery.m_collisionT;
					nearestPoint = segQuery.m_collisionPoint;
					nearestTrigger = trigger;
					nearestTriggerPriority = priority;
				}
			}

			// Make query upwards
			segQuery.m_positiveHit = FALSE;
			segQuery.m_collisionT = FLT_MAX;
			segQuery.m_colSegment = upSegment;

			trigger->QueryCollision(&segQuery);

			if(segQuery.m_positiveHit)
			{
				if(	priority > nearestTriggerPriority ||
					(priority == nearestTriggerPriority && (segQuery.m_collisionT < nearestDistance || (segQuery.m_collisionT == nearestDistance && IsSubZoneOf(trigger->GetZoneID(), nearestTrigger->GetZoneID())))))
				{
					nearestDistance = segQuery.m_collisionT;
					nearestPoint = segQuery.m_collisionPoint;
					nearestTrigger = trigger;
					nearestTriggerPriority = priority;
				}
			}
		}
	}

	return nearestTrigger;
}

CRuWorld_Trigger *CRuOptimalTerrain::GetApplicableTrigger(RuWorld::TriggerType *triggerTypes, const CRuVector3 &point, BOOL limitRange)
{
	REAL nearestDistance = FLT_MAX;
	CRuVector3 nearestPoint;
	CRuWorld_Trigger *nearestTrigger = NULL;
	INT32 nearestTriggerPriority = 0xFFFFFFFF;

	// Build common collision query
	CRuCollisionQuery_Segment_Nearest segQuery;
	CRuMatrix4x4 identityMatrix;
	segQuery.m_worldTransform = &identityMatrix;
	segQuery.m_invWorldTransform = &identityMatrix;
	segQuery.m_ignoreCollisionFlag = TRUE;

	// Build query segments
	CRuSegment3 downwardSegment(point, point + CRuVector3(0.0f, limitRange ? -5.0f : -10000.0f, 0.0f));
	CRuSegment3 upSegment(point, point + CRuVector3(0.0f, limitRange ? 5.0f : 10000.0f, 0.0f));

	// Get all enumerated triggers
	CRuArrayList<IRuObject *> triggers;
	m_objectContainer->EnumerateVisibleObjectsByType(triggers, CRuWorld_Trigger::Type());

	for(INT32 i = 0; i < triggers.Count(); ++i)
	{
		CRuWorld_Trigger *trigger = static_cast<CRuWorld_Trigger *>(triggers[i]);

		for(INT32 j = 0; triggerTypes[j] != RuWorld::ruTRIGGERTYPE_FORCE_DWORD; ++j)
		{
			if(trigger->GetTriggerType() == triggerTypes[j])
			{
				// Get trigger priority
				RuPropertyType propertyValue;
				INT32 priority  = trigger->GetProperty("CRuWorld_Trigger:Priority", propertyValue) ? boost::get<INT32>(propertyValue) : 0;

				// Make query downwards
				segQuery.m_positiveHit = FALSE;
				segQuery.m_collisionT = FLT_MAX;
				segQuery.m_colSegment = downwardSegment;

				trigger->QueryCollision(&segQuery);

				if(segQuery.m_positiveHit)
				{
					if(	priority > nearestTriggerPriority ||
						(priority == nearestTriggerPriority && (segQuery.m_collisionT < nearestDistance || (segQuery.m_collisionT == nearestDistance && IsSubZoneOf(trigger->GetZoneID(), nearestTrigger->GetZoneID())))))
					{
						nearestDistance = segQuery.m_collisionT;
						nearestPoint = segQuery.m_collisionPoint;
						nearestTrigger = trigger;
						nearestTriggerPriority = priority;
					}
				}

				// Make query upwards
				segQuery.m_positiveHit = FALSE;
				segQuery.m_collisionT = FLT_MAX;
				segQuery.m_colSegment = upSegment;

				trigger->QueryCollision(&segQuery);

				if(segQuery.m_positiveHit)
				{
					if(	priority > nearestTriggerPriority ||
						(priority == nearestTriggerPriority && (segQuery.m_collisionT < nearestDistance || (segQuery.m_collisionT == nearestDistance && IsSubZoneOf(trigger->GetZoneID(), nearestTrigger->GetZoneID())))))
					{
						nearestDistance = segQuery.m_collisionT;
						nearestPoint = segQuery.m_collisionPoint;
						nearestTrigger = trigger;
						nearestTriggerPriority = priority;
					}
				}

				break;
			}
		}
	}

	return nearestTrigger;
}

INT32 CRuOptimalTerrain::GetZoneID(const CRuVector3 &point)
{
	CRuWorld_Trigger *trigger = GetApplicableTrigger(RuWorld::ruTRIGGERTYPE_ZONE, point, FALSE);

	if(trigger)
	{
		return trigger->GetZoneID();
	}

	return 0;
}

RuWorld::ZoneType CRuOptimalTerrain::GetZoneType(const CRuVector3 &point)
{
	CRuWorld_Trigger *trigger = GetApplicableTrigger(RuWorld::ruTRIGGERTYPE_ZONETYPE, point, TRUE);

	if(trigger)
	{
		return trigger->GetZoneType();
	}

	return RuWorld::ruZONETYPE_OUTDOOR;
}

CRuWorld_ZoneDescriptor *CRuOptimalTerrain::GetZoneDescriptor(const CRuVector3 &point)
{
	INT32 zoneID = GetZoneID(point);

	return m_zoneDescriptorContainer->GetZoneDescriptor(zoneID);
}

CRuWorld_ZoneDescriptor *CRuOptimalTerrain::GetZoneDescriptorByID(INT32 zoneID)
{
	return m_zoneDescriptorContainer->GetZoneDescriptor(zoneID);
}

BOOL CRuOptimalTerrain::IsSubZoneOf(INT32 subZoneID, INT32 parentZoneID)
{
	CRuWorld_ZoneDescriptor *zoneDesc = m_zoneDescriptorContainer->GetZoneDescriptor(subZoneID);

	while(zoneDesc)
	{
		if(zoneDesc->GetZoneID() == parentZoneID)
		{
			return TRUE;
		}

		zoneDesc = m_zoneDescriptorContainer->GetZoneDescriptor(zoneDesc->GetParentZoneID());
	}

	return FALSE;
}

CRuWorld_WaterBodyDescriptor *CRuOptimalTerrain::GetWaterBodyDescriptorAtPoint(const CRuVector3 &point)
{
	CRuEntity *waterEntity = m_waterContainer->GetFirstChild();

	CRuWorld_WaterBodyDescriptor *nearestWaterBodyDescriptor = NULL;
	REAL nearestT = FLT_MAX;

	CRuCollisionQuery_Segment_Nearest colQuery;
	colQuery.m_colSegment = CRuSegment3(point - CRuVector3(0.0f, 1.0f, 0.0f), point + CRuVector3(0.0f, 10000.0f, 0.0f));
	colQuery.m_ignoreBackface = FALSE;

	while(waterEntity)
	{
		if(waterEntity->GetType().IsSubClassOf(CRuWorld_Water::Type()))
		{
			CRuWorld_Water *water = static_cast<CRuWorld_Water *>(waterEntity);
			CRuWorld_WaterBodyDescriptor *waterBodyDescriptor = NULL;
			if(m_waterBodyDescriptorMap->Get(water->GetWaterBodyGUID(), water->GetWaterBodyGUID().HashCode(), waterBodyDescriptor))
			{
				// Reset collision query and make query against water
				colQuery.m_positiveHit = FALSE;
				colQuery.m_collisionT = FLT_MAX;
				water->QueryCollision(&colQuery);

				if(colQuery.m_positiveHit && colQuery.m_collisionT < nearestT)
				{
					nearestWaterBodyDescriptor = waterBodyDescriptor;
					nearestT = colQuery.m_collisionT;
				}
			}
		}

		waterEntity = waterEntity->GetNextSibling();
	}

	return nearestWaterBodyDescriptor;
}

BOOL CRuOptimalTerrain::EnumerateAllTriggersByZoneID(INT32 zoneID, CRuArrayList<CRuWorld_Trigger *> &triggersOut)
{
	// Enumerate all objects of the type trigger
	CRuArrayList<IRuObject *> objects;
	m_objectContainer->EnumerateAllObjectsByType(CRuWorld_Trigger::Type(), objects);

	for(INT32 i = 0; i < objects.Count(); ++i)
	{
		CRuWorld_Trigger *trigger = static_cast<CRuWorld_Trigger *>(objects[i]);

		// If the trigger is a zone trigger, and its zone ID matches the specified zone ID, add it to output list. Otherwise release.
		if(trigger->GetTriggerType() == RuWorld::ruTRIGGERTYPE_ZONE && trigger->GetZoneID() == zoneID)
		{
			triggersOut.Add(trigger);
		}
		else
		{
			ruSAFE_RELEASE(trigger);
		}
	}

	return TRUE;
}

BOOL CRuOptimalTerrain::EnumerateAllTriggersByZoneID(CRuArrayList<INT32> zoneIDList, CRuArrayList<CRuWorld_Trigger *> &triggersOut)
{
	// Enumerate all objects of the type trigger
	CRuArrayList<IRuObject *> objects;
	m_objectContainer->EnumerateAllObjectsByType(CRuWorld_Trigger::Type(), objects);

	for(INT32 i = 0; i < objects.Count(); ++i)
	{
		CRuWorld_Trigger *trigger = static_cast<CRuWorld_Trigger *>(objects[i]);

		// If the trigger is a zone trigger, and its zone ID matches the specified zone ID, add it to output list. Otherwise release.
		if(trigger->GetTriggerType() == RuWorld::ruTRIGGERTYPE_ZONE && zoneIDList.Contains(trigger->GetZoneID()))
		{
			triggersOut.Add(trigger);
		}
		else
		{
			ruSAFE_RELEASE(trigger);
		}
	}

	return TRUE;
}

IRuOptimalTerrain_DataSource *CRuOptimalTerrain::GetDataSource()
{
	return m_dataSource;
}

CRuWorld_ObjectContainer *CRuOptimalTerrain::GetObjectContainer()
{
	return m_objectContainer;
}

CRuWorld_ObjectManager *CRuOptimalTerrain::GetObjectManager()
{
	return m_objectManager;
}

CRuWorld_DoodadContainer *CRuOptimalTerrain::GetDoodadContainer()
{
	return m_doodadContainer;
}

CRuWorld_ZoneDescriptorContainer *CRuOptimalTerrain::GetZoneDescriptorContainer()
{
	return m_zoneDescriptorContainer;
}

IRuPathHierarchy_Base *CRuOptimalTerrain::GetPathHierarchy()
{
	return m_pathHierarchy;
}

void CRuOptimalTerrain::SetTerrainSplatDetailLevel(INT32 detailLevel)
{
	m_terrainSplatDetailLevel = detailLevel;
}

void CRuOptimalTerrain::ToggleOption_RenderTerrain()
{
	m_renderTerrain = !m_renderTerrain;
}

void CRuOptimalTerrain::ToggleOption_RenderTerrainTexture()
{
	m_renderTerrainTexture = !m_renderTerrainTexture;
}

void CRuOptimalTerrain::ToggleOption_RenderBackdropTerrain()
{
	m_renderBackdropTerrain = !m_renderBackdropTerrain;
}

void CRuOptimalTerrain::ToggleOption_RenderObjects()
{
	m_renderObjects = !m_renderObjects;
}

void CRuOptimalTerrain::ToggleOption_RenderGrass()
{
	m_renderGrass = !m_renderGrass;
}

void CRuOptimalTerrain::ToggleOption_RenderWater()
{
	m_renderWater = !m_renderWater;
}

BOOL CRuOptimalTerrain::EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting)
{
	return ApplyEngineSettings();
}

BOOL CRuOptimalTerrain::ApplyEngineSettings()
{
	RuPropertyType propertyValue;
	BOOL rebuildWaterMaterials = FALSE;

	if(g_ruEngineSettings->GetSetting("world:view distance", propertyValue))
	{
		// Handled externally
	}

	if(g_ruEngineSettings->GetSetting("terrain:splat detail", propertyValue))
	{
		this->SetTerrainSplatDetailLevel(boost::get<INT32>(propertyValue));
	}

	if(g_ruEngineSettings->GetSetting("terrain:detail class", propertyValue))
	{
		m_detailClassLevel = boost::get<INT32>(propertyValue);
	}

	if(g_ruEngineSettings->GetSetting("gfx:water reflection", propertyValue))
	{
		BOOL reflectionEnabled = boost::get<BOOL>(propertyValue);

		if(m_enableWaterReflection != reflectionEnabled)
		{
			m_enableWaterReflection = reflectionEnabled;
			rebuildWaterMaterials = TRUE;
		}
	}

	if(g_ruEngineSettings->GetSetting("gfx:water refraction", propertyValue))
	{
		BOOL refractionEnabled = boost::get<BOOL>(propertyValue);

		if(m_enableWaterRefraction != refractionEnabled)
		{
			m_enableWaterRefraction = refractionEnabled;
			rebuildWaterMaterials = TRUE;
		}
	}

	if(rebuildWaterMaterials)
	{
		for(INT32 i = 0; i < m_waterBodyDescriptors.Count(); ++i)
		{
			m_waterBodyDescriptors[i]->BuildWaterBodyMaterial();
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
