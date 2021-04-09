#include "../../Scene/Terrain/RuEntityTerrain.h"
#include "../../Scene/Terrain/RuEntityOptimalTerrain.h"

#include "../../../Tools/zLib/zlib.h"
#include "../../../Tools/dxtLib/dxtutility.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuTerrain_Chunk, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuTerrain_Chunk, "CRuTerrain_Chunk", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuTerrain_Chunk, 0x9306948C, 0x7938455d, 0xB287CEE6, 0x73BC4EF6)

ruRTTI_DEFINE_SUBCLASS(IRuTerrain_DataSource, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuTerrain_DataSource, "IRuTerrain_DataSource", "IRuObject")
ruCLASSGUID_DEFINE(IRuTerrain_DataSource, 0xED509656, 0xE1834463, 0x923257DE, 0x0F209B24)

ruRTTI_DEFINE_SUBCLASS(CRuTerrain_DataSource_Remote, IRuTerrain_DataSource)
ruHEAP_DEFINE_SUBCLASS(CRuTerrain_DataSource_Remote, "CRuTerrain_DataSource_Remote", "IRuTerrain_DataSource")
ruCLASSGUID_DEFINE(CRuTerrain_DataSource_Remote, 0x6BCC6615, 0xD76D44dd, 0x970EFC06, 0x730B80CD)

ruRTTI_DEFINE_SUBCLASS(CRuTerrain, IRuEntity_Container)
ruHEAP_DEFINE_SUBCLASS(CRuTerrain, "CRuTerrain", "IRuEntity_Container")
ruCLASSGUID_DEFINE(CRuTerrain, 0x72E195BC, 0x5E0E4ec6, 0x9C0E144E, 0xFEAD9181)

// ************************************************************************************************************************************************************

CRuTerrain_Chunk::CRuTerrain_Chunk()
:	m_chunkIteration(0),
	m_dimension(64),
	m_heightFieldDimension(67),
	m_gridSize(10.0f),
	m_lodLevels(1),
	m_numVertices(0),
	m_numIndices(0),
	m_chunkFlags(0),
	m_hfObject(NULL),
	m_heightField(NULL),
	m_heightAutoOffset(NULL),
	m_heightAutoPainted(NULL),
	m_vertexFlags(NULL),
	m_mesh(NULL),
	m_colStructure(NULL),
	m_outlineMesh(NULL),
	m_outlineMaterial(NULL),
	m_material(NULL),
	m_highlightController(NULL),
	m_lodMeshes(NULL),
	m_currentLOD(0),
	m_highlightOn(FALSE),
	m_outlineOn(FALSE),
	m_splatIDMap(NULL),
	m_offsetPainted(FALSE)
{
	// Create highlight controller and add it as child
	m_highlightController = ruNEW CRuController_Highlighter();
	this->AddChild(m_highlightController);
}

CRuTerrain_Chunk::~CRuTerrain_Chunk()
{
	// Release all held resources
	ruSAFE_RELEASE(m_hfObject);
	delete[] m_heightAutoOffset;
	delete[] m_heightAutoPainted;
	delete[] m_vertexFlags;

	ruSAFE_RELEASE(m_mesh);
	ruSAFE_RELEASE(m_colStructure);

	ruSAFE_RELEASE(m_outlineMesh);
	ruSAFE_RELEASE(m_outlineMaterial);

	ruSAFE_RELEASE(m_material);
	ruSAFE_RELEASE(m_highlightController);

	if(m_lodMeshes)
	{
		for(INT32 i = 0; i < m_lodLevels; ++i)
		{
			ruSAFE_RELEASE(m_lodMeshes[i])
		}

		delete[] m_lodMeshes;
	}

	// Release splat ID maps
	ruSAFE_RELEASE(m_splatIDMap);

	// Release doodad lists
	for(INT32 i = 0; i < m_doodadLists.Count(); ++i)
	{
		delete m_doodadLists[i];
	}
	m_doodadLists.Clear();

	// Release water layers
	for(INT32 i = 0; i < m_waterLayers.Count(); ++i)
	{
		ruSAFE_RELEASE(m_waterLayers[i]);
	}
	m_waterLayers.Clear();

	// Reset members
	m_numVertices = 0;
	m_numIndices = 0;

	m_heightField = NULL;
	m_heightAutoOffset = NULL;
	m_heightAutoPainted = NULL;
	m_mesh = NULL;
	m_colStructure = NULL;
	m_lodMeshes = NULL;
}

BOOL CRuTerrain_Chunk::Allocate(INT32 dimension, float gridSize, const CRuVector3 &chunkOrigin, INT32 lodLevels, INT32 splatResolution)
{
	// Setup translation
	this->SetTranslation(chunkOrigin);

	// Force dimension to be a power of two
	m_dimension = dimension;

	// Store grid size
	m_gridSize = gridSize;

	// Set tessellation level to 16x16 grids
	m_lodLevels = lodLevels;

	// Setup other parameters based on initial settings
	CalculateChunkParameters();

	// Initialize mesh vertices
	InitializeMeshVertices(chunkOrigin.x, chunkOrigin.z);

	// Calculate bounding sphere for the chunk mesh
	CalculateBounds();

	// Store splat resolution and allocate splat ID maps
	InitializeSplatIDMap(splatResolution);

	return TRUE;
}

IRuObject *CRuTerrain_Chunk::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuTerrain_Chunk();

	// Chain cloning call to parent class
	IRuEntity_Renderable::Clone(clonedObject);

	// Allocate space for the cloned chunk
	CRuTerrain_Chunk *terrainChunk = static_cast<CRuTerrain_Chunk *>(clonedObject);
	terrainChunk->Allocate(m_dimension, m_gridSize, m_bounds.Minima(), m_lodLevels, m_splatResolution);

	// Copy chunk iteration
	terrainChunk->m_chunkIteration = m_chunkIteration;

	// Copy chunk key
	terrainChunk->m_chunkKey = m_chunkKey;

	// Copy chunk flags
	terrainChunk->m_chunkFlags = m_chunkFlags;

	// Copy height field
	ruSAFE_RELEASE(terrainChunk->m_hfObject);
	terrainChunk->m_hfObject = static_cast<CRuMojo_AttributeParcel<REAL> *>(m_hfObject->Clone());
	terrainChunk->m_heightField = terrainChunk->m_hfObject->GetParcel();

	// Copy height auto-offset
	memcpy(terrainChunk->m_heightAutoOffset, m_heightAutoOffset, sizeof(REAL) * m_heightFieldDimension * m_heightFieldDimension);

	memcpy(terrainChunk->m_heightAutoPainted, m_heightAutoPainted, sizeof(BYTE) * m_heightFieldDimension * m_heightFieldDimension);
	
	// Propagate height field to mesh vertices
	CRuVector3 *vertices = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
	for(INT32 z = 0, i = 0, srcIdx = m_heightFieldDimension + 1, destIdx = 0; z <= m_dimension; ++z, srcIdx += 2)
	{
		for(INT32 x = 0; x <= m_dimension; ++x, ++destIdx, ++srcIdx)
		{
			// Copy vertex data
			vertices[destIdx].m_y = m_heightField[srcIdx];
		}
	}

	// Copy color values
	memcpy(static_cast<CRuMorphTarget *>(terrainChunk->m_mesh->GetMorphTarget(0))->GetDiffuse(), static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetDiffuse(), sizeof(DWORD) * (m_dimension + 1) * (m_dimension + 1));

	// Copy doodad clusters
	for(INT32 i = 0; i < m_doodadLists.Count(); ++i)
	{
		CRuWorld_DoodadCluster *doodadCluster = ruNEW CRuWorld_DoodadCluster();
		doodadCluster->m_guid = m_doodadLists[i]->m_guid;
		doodadCluster->m_doodads.CloneFrom(m_doodadLists[i]->m_doodads);
		terrainChunk->m_doodadLists.Add(doodadCluster);
	}

	// Copy water layers
	for(INT32 i = 0; i < m_waterLayers.Count(); ++i)
	{
		terrainChunk->m_waterLayers.Add(static_cast<CRuWorld_Water *>(m_waterLayers[i]->Clone()));
	}

	// Copy the translation
	terrainChunk->SetTranslation(this->GetTranslation());

	// Build the mesh topology
	terrainChunk->BuildChunkTopology();

	// Update main body vertices
	terrainChunk->UpdateVertices();

	// Update skirt vertices
	terrainChunk->UpdateSkirtVertices();

	// Calculate normals(not quite complete)
	terrainChunk->CalculateNormals();

	// Calculate bounding sphere for the chunk mesh
	terrainChunk->CalculateBounds();

	// Copy splat map
	terrainChunk->m_splatIDMap->Release();
	terrainChunk->m_splatIDMap = static_cast<CRuSplatIDMap *>(m_splatIDMap->Clone());

	// Set the collision object
	terrainChunk->AttachCollisionStructure(this->m_colStructure);

	return clonedObject;
}

BOOL CRuTerrain_Chunk::SerializeFrom(IRuStream *stream, RuTerrainChunkLoadMode loadMode)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version;
	stream->Read(&version, sizeof(INT32));

	// Read chunk iteration
	stream->Read(&m_chunkIteration, sizeof(INT32));

	// Read chunk dimension
	stream->Read(&m_dimension, sizeof(INT32));

	// Read grid size
	stream->Read(&m_gridSize, sizeof(float));

	// Read chunk tessellation
	stream->Read(&m_lodLevels, sizeof(INT32));

	// Read chunk flags
	stream->Read(&m_chunkFlags, sizeof(UINT32));

	// Read coordinate offset values
	float xOrigin, zOrigin;
	stream->Read(&xOrigin, sizeof(float));
	stream->Read(&zOrigin, sizeof(float));

	// Setup translation
	this->SetTranslation(CRuVector3(xOrigin, 0.0f, zOrigin));

	// Calculate related parameters
	CalculateChunkParameters();

	switch(loadMode)
	{
		case ruTERRAINCHUNKLOADMODE_MINIMAL:
			{
			}

			break;

		default:
			{
				// Initialize mesh vertices
				InitializeMeshVertices(xOrigin, zOrigin);

				ruSAFE_RELEASE(m_hfObject);

				Mojo_SerializeFrom(&m_hfObject, stream);

				// Correct parcel origin... this is because we changed from absolute to relative coordinates
				m_hfObject->SetParcelOrigin(CRuVector3(-m_gridSize, 0.0f, -m_gridSize));

				// Correct parcel gap.... this is our previous stupid mistake
				m_hfObject->SetParcelOverlapGap(1);

				// Set height field pointer
				m_heightField = m_hfObject->GetParcel();

				// Read field flags
				stream->Read(m_vertexFlags, sizeof(BYTE) * (m_dimension + 1) * (m_dimension + 1));

				// Set color values
				DWORD *diffuse = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetDiffuse();
				for(INT32 i = 0; i < m_numVertices; ++i)
				{
					diffuse[i] = 0xFFFFFFFF;
				}

				// Update main body vertices
				UpdateVertices();

				// Update skirt vertices
				UpdateSkirtVertices();

				// Build the mesh topology
				BuildChunkTopology();

				// Calculate normals(not quite complete)
				CalculateNormals();

				// Calculate bounding sphere for the chunk mesh
				CalculateBounds();

				// Read splat resolution
				stream->Read(&m_splatResolution, sizeof(INT32));

				// Use object factory to serialize the splat map object
				IRuObject *idMapObj = RuObjectFactory().CreateObject(stream);

				if(idMapObj && idMapObj->GetType() == CRuSplatIDMap::Type())
				{
					m_splatIDMap = static_cast<CRuSplatIDMap *>(idMapObj);

					// FIXME: Remove this later on, this is here to fix the stupid mistake of using world coordinates...
					FixSplatIDMapBounds(m_splatResolution);

					// FIXME: Reduce the splat ID map to power of two
					m_splatIDMap->ReduceSizeToPowerOfTwo();
				}
				else
				{
					// Display error here
					m_splatIDMap = NULL;
					idMapObj->Release();
				}

				if(version <= 2)
				{
					// Read number of grass particles
					INT32 numGrassParticles;
					stream->Read(&numGrassParticles, sizeof(INT32));

					// Skip the grass particles
					INT32 skipGrassBytes = 24 * numGrassParticles;
					stream->Seek(skipGrassBytes, ruSSM_Current);
/*
					// Read all particles
					for(INT32 i = 0; i < numGrassParticles; ++i)
					{
						// Create new particle
						m_grassParticles.Add(CRuParticle_Grass());

						stream->Read(&m_grassParticles[i].m_position.x, sizeof(REAL));
						stream->Read(&m_grassParticles[i].m_position.y, sizeof(REAL));
						stream->Read(&m_grassParticles[i].m_position.z, sizeof(REAL));
						stream->Read(&m_grassParticles[i].m_textureIndex, sizeof(UINT16));
						stream->Read(&m_grassParticles[i].m_subTextureIndex, sizeof(UINT8));
						stream->Read(&m_grassParticles[i].m_modelStyle, sizeof(UINT8));
						stream->Read(&m_grassParticles[i].m_scale, sizeof(REAL));
						stream->Read(&m_grassParticles[i].m_swayMagnitude, sizeof(REAL));
					}
*/
					// Read number of entity descriptor GUIDs
					INT32 numEntityDescriptorGUIDs;
					stream->Read(&numEntityDescriptorGUIDs, sizeof(INT32));

					// Skip the entity descriptor GUIDs
					INT32 skipEntityGUIDBytes = numEntityDescriptorGUIDs * 16;
					stream->Seek(skipEntityGUIDBytes, ruSSM_Current);
/*
					// Read all entity descriptor GUIDs
					m_entityDescriptorGUIDs.SetArraySize(numEntityDescriptorGUIDs);
					m_entityDescriptorGUIDs.SetNumEntries(numEntityDescriptorGUIDs);
					for(INT32 i = 0; i < numEntityDescriptorGUIDs; ++i)
					{
						RuReadGUID(stream, m_entityDescriptorGUIDs[i]);
					}
*/
				}

				if(version >= 2)
				{
					// Read number of doodad clusters
					INT32 numDoodadClusters = 0;
					stream->Read(&numDoodadClusters, sizeof(INT32));

					if(numDoodadClusters > 0)
					{
						m_doodadLists.SetArraySize(numDoodadClusters);
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
							m_doodadLists[i]->m_doodads.SetArraySize(numDoodads);
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
					// Serialize water layer from stream
					m_waterLayers.Add(ruNEW CRuWorld_Water());
					m_waterLayers[i]->SerializeFrom(stream);

					// Build collision structure for the water layer
					m_waterLayers[i]->BuildCollisionStructure();
				}
			}

			break;
	}

	return TRUE;
}

BOOL CRuTerrain_Chunk::SerializeFrom(IRuStream *stream)
{
	return SerializeFrom(stream, ruTERRAINCHUNKLOADMODE_FULL);
}

BOOL CRuTerrain_Chunk::SerializeTo(IRuStream *stream)
{
	INT32 version = 3;

	// Increment chunk iteration
	++m_chunkIteration;

	// Get pointers to data
	CRuVector3 *vertices = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetDiffuse();

	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	stream->Write(&version, sizeof(INT32));

	// Write chunk iteration
	stream->Write(&m_chunkIteration, sizeof(INT32));

	// Write chunk dimension
	stream->Write(&m_dimension, sizeof(INT32));

	// Write grid size
	stream->Write(&m_gridSize, sizeof(float));

	// Write tessellation level
	stream->Write(&m_lodLevels, sizeof(INT32));

	// Write chunk flags
	stream->Write(&m_chunkFlags, sizeof(UINT32));

	// Write coordinate offset values
	CRuVector3 origin = this->GetTranslation();
	stream->Write(&origin.m_x, sizeof(float));
	stream->Write(&origin.m_z, sizeof(float));

	// Serialize height field object
	Mojo_SerializeTo(m_hfObject, stream);

	// Write field flags
	stream->Write(m_vertexFlags, sizeof(BYTE) * (m_dimension + 1) * (m_dimension + 1));

	// Write splat resolution
	stream->Write(&m_splatResolution, sizeof(INT32));

	// Use object factory to serialize the splat map object
	m_splatIDMap->SerializeTo(stream);

	// Write number of doodad clusters
	INT32 numDoodadClusters = m_doodadLists.Count();
	stream->Write(&numDoodadClusters, sizeof(INT32));

	// Write each doodad cluster
	for(INT32 i = 0; i < numDoodadClusters; ++i)
	{
		// Write template GUID
		stream->Write(m_doodadLists[i]->m_guid.Data(), m_doodadLists[i]->m_guid.Size());

		// Write number of doodads
		INT32 numDoodads = m_doodadLists[i]->m_doodads.Count();
		stream->Write(&numDoodads, sizeof(INT32));

		// Write each doodad
		for(INT32 j = 0; j < numDoodads; ++j)
		{
			CRuDoodad_Descriptor &doodadDescriptor = m_doodadLists[i]->m_doodads[j];

			stream->Write(&doodadDescriptor.m_textureIndex, sizeof(INT32));
			stream->Write(&doodadDescriptor.m_scale, sizeof(REAL));
			stream->Write(&doodadDescriptor.m_rotation.x, sizeof(REAL));
			stream->Write(&doodadDescriptor.m_rotation.y, sizeof(REAL));
			stream->Write(&doodadDescriptor.m_rotation.z, sizeof(REAL));
			stream->Write(&doodadDescriptor.m_rotation.w, sizeof(REAL));
			stream->Write(&doodadDescriptor.m_translation.x, sizeof(REAL));
			stream->Write(&doodadDescriptor.m_translation.y, sizeof(REAL));
			stream->Write(&doodadDescriptor.m_translation.z, sizeof(REAL));
			stream->Write(&doodadDescriptor.m_swayMagnitude, sizeof(REAL));
		}
	}

	// Write number of water layers
	INT32 numWaterLayers = m_waterLayers.Count();
	stream->Write(&numWaterLayers, sizeof(INT32));

	// Write each water layer
	for(INT32 i = 0; i < numWaterLayers; ++i)
	{
		m_waterLayers[i]->SerializeTo(stream);
	}

	return TRUE;
}

BOOL CRuTerrain_Chunk::SerializeToOptimizedChunk(IRuStream *stream, CRuSplatMapBuilder *splatMapBuilder)
{
	// Get pointers to data
	CRuVector3 *vertices = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
	CRuVector3 *normals = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetNormal();
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetDiffuse();
	UINT16 *indices = m_mesh->GetIndices();

	// Write optimized chunk class GUID
	stream->Write(CRuOptimalTerrain_Chunk::ClassGUID().Data(), CRuOptimalTerrain_Chunk::ClassGUID().Size());

	// Read chunk iteration
	stream->Write(&m_chunkIteration, sizeof(INT32));

	// Write chunk dimension
	stream->Write(&m_dimension, sizeof(INT32));

	// Write grid size
	stream->Write(&m_gridSize, sizeof(REAL));

	// Write default number of lod levels
	stream->Write(&m_lodLevels, sizeof(INT32));

	// Write chunk bounds
	stream->Write(&m_bounds.Minima().x, sizeof(REAL));
	stream->Write(&m_bounds.Minima().y, sizeof(REAL));
	stream->Write(&m_bounds.Minima().z, sizeof(REAL));
	stream->Write(&m_bounds.Maxima().x, sizeof(REAL));
	stream->Write(&m_bounds.Maxima().y, sizeof(REAL));
	stream->Write(&m_bounds.Maxima().z, sizeof(REAL));

	// Write coordinate offset values
	CRuVector3 origin = this->GetTranslation();
	stream->Write(&origin.m_x, sizeof(float));
	stream->Write(&origin.m_z, sizeof(float));

	// Write height values (without padding)
	float *heightBuffer = ruNEW REAL [m_numVertices];
	for(INT32 i = 0; i < m_numVertices; ++i)
	{
		heightBuffer[i] = vertices[i].m_y;
	}

	stream->Write(heightBuffer, sizeof(REAL) * m_numVertices);
	delete[] heightBuffer;

	// Write normals and alpha in packed DWORD format
	DWORD *packedDataArray = ruNEW DWORD [m_numVertices];

	// Process through non-skirt vertices
	INT32 numMainBodyVertices = (m_dimension + 1) * (m_dimension + 1);
	for(INT32 i = 0; i < m_numVertices; ++i)
	{
		// Pack vertex flags into 0xAA000000
		DWORD packedData;

		// Only process vertex flags for main body vertices
		if(i < numMainBodyVertices)
		{
			packedData = (DWORD) m_vertexFlags[i] << 24;
		}
		else
		{
			packedData = (DWORD) 0x00000000;
		}

		// Pack normal into 0x00XXYYZZ
		BYTE nx = (BYTE) ((normals[i].m_x * 127.0f) + 127.0f);
		BYTE ny = (BYTE) ((normals[i].m_y * 127.0f) + 127.0f);
		BYTE nz = (BYTE) ((normals[i].m_z * 127.0f) + 127.0f);

		packedData = packedData | ((nx << 16) & 0x00FF0000);
		packedData = packedData | ((ny << 8) & 0x0000FF00);
		packedData = packedData | (nz & 0x000000FF);

		packedDataArray[i] = packedData;
	}

	stream->Write(packedDataArray, sizeof(DWORD) * m_numVertices);
	delete[] packedDataArray;

	// Write splat resolution
	stream->Write(&m_splatResolution, sizeof(INT32));

	// Write splat alpha maps
	m_splatIDMap->MarkAsDirty();
	splatMapBuilder->BuildAlphaMaps(m_splatIDMap, TRUE, ruSURFACEFMT_A8R8G8B8, TRUE);
	m_splatIDMap->SerializeToAlphaMaps(stream);
	m_splatIDMap->FreeAlphaMaps();

	// Write number of doodad clusters
	INT32 numDoodadClusters = m_doodadLists.Count();
	stream->Write(&numDoodadClusters, sizeof(INT32));

	// Write each doodad cluster
	for(INT32 i = 0; i < numDoodadClusters; ++i)
	{
		// Write template GUID
		stream->Write(m_doodadLists[i]->m_guid.Data(), m_doodadLists[i]->m_guid.Size());

		// Write number of doodads
		INT32 numDoodads = m_doodadLists[i]->m_doodads.Count();
		stream->Write(&numDoodads, sizeof(INT32));

		// Write each doodad
		for(INT32 j = 0; j < numDoodads; ++j)
		{
			CRuDoodad_Descriptor &doodadDescriptor = m_doodadLists[i]->m_doodads[j];

			stream->Write(&doodadDescriptor.m_textureIndex, sizeof(INT32));
			stream->Write(&doodadDescriptor.m_scale, sizeof(REAL));
			stream->Write(&doodadDescriptor.m_rotation.x, sizeof(REAL));
			stream->Write(&doodadDescriptor.m_rotation.y, sizeof(REAL));
			stream->Write(&doodadDescriptor.m_rotation.z, sizeof(REAL));
			stream->Write(&doodadDescriptor.m_rotation.w, sizeof(REAL));
			stream->Write(&doodadDescriptor.m_translation.x, sizeof(REAL));
			stream->Write(&doodadDescriptor.m_translation.y, sizeof(REAL));
			stream->Write(&doodadDescriptor.m_translation.z, sizeof(REAL));
			stream->Write(&doodadDescriptor.m_swayMagnitude, sizeof(REAL));
		}
	}

	// Write number of water layers
	INT32 numWaterLayers = m_waterLayers.Count();
	stream->Write(&numWaterLayers, sizeof(INT32));

	// Write each water layer
	for(INT32 i = 0; i < numWaterLayers; ++i)
	{
		m_waterLayers[i]->SerializeTo(stream);
	}

	return TRUE;
}

BOOL CRuTerrain_Chunk::AttachCollisionStructure(IRuCollisionStructure *colStructure)
{
	if(colStructure)
	{
		colStructure->AddRef();
		ruSAFE_RELEASE(m_colStructure);
		m_colStructure = colStructure;

		return TRUE;
	}

	return FALSE;
}

INT32 CRuTerrain_Chunk::GetNumMeshes() const
{
	if(m_outlineOn)
	{
		return 2;
	}

	return 1;
}

BOOL CRuTerrain_Chunk::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(m_outlineOn)
	{
		if(meshIndex == 0)
		{
			*meshOut = m_lodMeshes[m_currentLOD];
			*materialOut = m_material;
		}
		else
		{
			*meshOut = m_outlineMesh;
			*materialOut = m_outlineMaterial;
		}
	}
	else
	{
		*meshOut = m_lodMeshes[m_currentLOD];
		*materialOut = m_material;
	}

	return TRUE;
}

void CRuTerrain_Chunk::CalculateEntityBounds()
{
	CalculateBounds();
}

BOOL CRuTerrain_Chunk::QueryCollision(CRuCollisionQuery *colQuery)
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

DWORD CRuTerrain_Chunk::GetChunkKey() const
{
	return m_chunkKey;
}

void CRuTerrain_Chunk::SetChunkKey(DWORD chunkKey)
{
	m_chunkKey = chunkKey;
}

UINT32 CRuTerrain_Chunk::GetChunkFlags() const
{
	return m_chunkFlags;
}

void CRuTerrain_Chunk::SetChunkFlags(UINT32 chunkFlags)
{
	m_chunkFlags = chunkFlags;
}

void CRuTerrain_Chunk::SetCamera(CRuCamera *viewCamera)
{
	float farPlaneDistance = viewCamera->GetFarPlaneDistance();
	float distanceToChunk = RuSqrt(SquaredDistance(viewCamera->GetCameraPosition(), m_bounds));

	// Calculate LOD to use based on a linear scale
	float t = distanceToChunk / farPlaneDistance;
	m_currentLOD = min((INT32) (t * m_lodLevels), m_lodLevels - 1);
}

void CRuTerrain_Chunk::SetHighlight(BOOL highlight, RUCOLOR color, REAL alpha)
{
	m_highlightOn = highlight;

	if(m_highlightOn)
	{
		m_highlightController->SetTarget(this);
		m_highlightController->SetPulsePeriod(2.0f);
		m_highlightController->SetHighlightColor0(CRuColor4(color));
		m_highlightController->SetHighlightColor1(CRuColor4(color));
	}
	else
	{
		m_highlightController->SetTarget(NULL);
	}
}

void CRuTerrain_Chunk::SetOutline(BOOL outline)
{
	m_outlineOn = outline;
}

INT32 CRuTerrain_Chunk::GetChunkIteration() const
{
	return m_chunkIteration;
}

REAL CRuTerrain_Chunk::GetGridSize() const
{
	return m_gridSize;
}

INT32 CRuTerrain_Chunk::GetLODLevels() const
{
	return m_lodLevels;
}

const CRuAABB &CRuTerrain_Chunk::GetBoundingBox() const
{
	return m_bounds;
}

CRuSplatIDMap *CRuTerrain_Chunk::GetSplatIDMap()
{
	return m_splatIDMap;
}

CRuMojo_AttributeParcel<REAL> *CRuTerrain_Chunk::GetHeightField()
{
	return m_hfObject;
}

BOOL CRuTerrain_Chunk::PaintFlags(const CRuVector3 &center, float radius, UINT32 flags, UINT32 flagMask)
{
	m_chunkFlags = (m_chunkFlags & ~flagMask) | (flags & flagMask);

	return TRUE;
}

BOOL CRuTerrain_Chunk::PaintHeight(const CRuVector3 &center, float radius, float delta, float refHeight, BOOL adaptivePainting, RuFalloffFunction falloffFunction)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	// Perform painting
	Mojo_PaintHeight(m_hfObject, localCenter, radius, delta, refHeight, adaptivePainting, falloffFunction);

	// Update main body vertices
	UpdateVertices();

	// Update skirt vertices
	UpdateSkirtVertices();

	// Change mesh iteration
	m_mesh->IncrementMeshIteration();
	m_outlineMesh->IncrementMeshIteration();

	// Recalculate bounds and normals
	CalculateBounds();
	CalculateNormals();

	// temp
//	m_colStructure->RelaxTree();

	return TRUE;
}

BOOL CRuTerrain_Chunk::NoiseHeight(const CRuVector3 &center, float radius, float variation)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	// Generate noise
	Mojo_NoiseHeight(m_hfObject, localCenter, radius, variation);

	// Update main body vertices
	UpdateVertices();

	// Update skirt vertices
	UpdateSkirtVertices();

	// Change mesh iteration
	m_mesh->IncrementMeshIteration();
	m_outlineMesh->IncrementMeshIteration();

	// Recalculate bounds and normals
	CalculateBounds();
	CalculateNormals();

	// temp
//	m_colStructure->RelaxTree();

	return TRUE;
}

BOOL CRuTerrain_Chunk::SetHeight(const CRuVector3 &center, float radius, float height, float brushStrength, RuFalloffFunction falloffFunction)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	// Set height
	Mojo_SetHeight(m_hfObject, localCenter, radius, height, brushStrength, falloffFunction);

	// Update main body vertices
	UpdateVertices();

	// Update skirt vertices
	UpdateSkirtVertices();

	// Change mesh iteration
	m_mesh->IncrementMeshIteration();
	m_outlineMesh->IncrementMeshIteration();

	// Recalculate bounds and normals
	CalculateBounds();
	CalculateNormals();

	// temp
//	m_colStructure->RelaxTree();

	return TRUE;
}

BOOL CRuTerrain_Chunk::SmoothHeight(const CRuVector3 &center, float radius, float delta)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	// Smooth height
	Mojo_SmoothHeight(m_hfObject, localCenter, radius, delta);

	// Update main body vertices
	UpdateVertices();

	// Update skirt vertices
	UpdateSkirtVertices();

	// Change mesh iteration
	m_mesh->IncrementMeshIteration();
	m_outlineMesh->IncrementMeshIteration();

	// Recalculate bounds and normals
	CalculateBounds();
	CalculateNormals();

	// temp
//	m_colStructure->RelaxTree();

	return TRUE;
}

BOOL CRuTerrain_Chunk::Stitch(INT32 neighborID, CRuTerrain_Chunk *chunk)
{
	if(chunk == NULL)
		return FALSE;

	// Stitch height field
	m_hfObject->Stitch(neighborID, chunk->m_hfObject);

	// Propagate height field to mesh
	CRuVector3 *vertices = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
	for(INT32 z = 0, i = 0, srcIdx = m_heightFieldDimension + 1, destIdx = 0; z <= m_dimension; ++z, srcIdx += 2)
	{
		for(INT32 x = 0; x <= m_dimension; ++x, ++destIdx, ++srcIdx)
		{
			// Copy vertex data
			vertices[destIdx].m_y = m_heightField[srcIdx];
		}
	}

	return TRUE;
}

BOOL CRuTerrain_Chunk::ExportHeight(float *destHeightMap, INT32 destOffset, INT32 destPitch)
{
	CRuVector3 *vertices = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();

	for(INT32 z = 0, vertexOffset = 0; z <= m_dimension; ++z)
	{
		float *destPtr = &destHeightMap[destOffset + z * destPitch];

		for(INT32 x = 0; x <= m_dimension; ++x, ++vertexOffset)
		{
			destPtr[x] = vertices[vertexOffset].y;
		}
	}

	return TRUE;
}

BOOL CRuTerrain_Chunk::ImportHeight(float *srcHeightMap, INT32 srcOffset, INT32 srcPitch)
{
	CRuVector3 *vertices = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();

	// Import heights into the main mesh
	for(INT32 z = 0; z <= m_dimension; ++z)
	{
		float *srcPtr = &srcHeightMap[srcOffset + z * srcPitch];

		for(INT32 x = 0, vertexOffset = 1 + m_heightFieldDimension * (z + 1); x <= m_dimension; ++x, ++vertexOffset)
		{
			m_heightField[vertexOffset] = srcPtr[x];
		}
	}

	// Update main body vertices
	UpdateVertices();

	// Update skirt vertices
	UpdateSkirtVertices();

	// Change mesh iteration
	m_mesh->IncrementMeshIteration();
	m_outlineMesh->IncrementMeshIteration();

	// Recalculate bounds and normals
	CalculateBounds();
	CalculateNormals();

	// temp
//	m_colStructure->RelaxTree();

	return TRUE;
}

BOOL CRuTerrain_Chunk::AutoPaintHeight(const CRuVector3 &center, float radius, CRuFractal_Base *srcFractal, const CRuSplatIDMap_PaintSetting &paintSetting)
{
	// Figure out the area to alter
	CRuVector3 relativeCenter = center - m_heightFieldBounds.Minima();

	INT32 sX = max(0, (INT32) ((relativeCenter.x - radius) / m_gridSize));
	INT32 sZ = max(0, (INT32) ((relativeCenter.z - radius) / m_gridSize));
	INT32 eX = min(m_heightFieldDimension, (INT32) ((relativeCenter.x + radius) / m_gridSize + 1));
	INT32 eZ = min(m_heightFieldDimension, (INT32) ((relativeCenter.z + radius) / m_gridSize + 1));

	REAL coordScale = paintSetting.m_brushLimit / radius;

	// Reset auto-paint field
	memset(m_heightAutoPainted, 0, sizeof(BYTE) * m_heightFieldDimension * m_heightFieldDimension);

	// Check brush against height field
	for(INT32 z = sZ; z < eZ; ++z)
	{
		for(INT32 x = sX; x < eX; ++x)
		{
			CRuVector3 pointPos(m_heightFieldBounds.Minima().x + x * m_gridSize, center.y, m_heightFieldBounds.Minima().z + z * m_gridSize);
			CRuVector3 relativePos = pointPos - center;
			float distance = relativePos.Magnitude();

			if(distance <= radius)
			{
				REAL noiseLevel = 0.0f;

				// Get basic noise level
				if(paintSetting.m_brushLimit > 0.0f)
				{
					// Offset relative position by painting clamp center
					relativePos.x *= coordScale;
					relativePos.z *= coordScale;

					relativePos.x += paintSetting.m_brushCenterX;
					relativePos.z += paintSetting.m_brushCenterY;

					noiseLevel = max(0.0f, min(1.0f, (srcFractal->Evaluate(relativePos.x, relativePos.z, 0.0f) + 1.0f) / 2.0f));
				}
				else
				{
					noiseLevel = max(0.0f, min(1.0f, (srcFractal->Evaluate(pointPos.x, pointPos.z, 0.0f) + 1.0f) / 2.0f));
				}

				// Noise falloff
				noiseLevel *= paintSetting.EvaluateFalloff(paintSetting.m_noiseFalloffType, distance, radius);

				// Tamper noise level by the height threshold
				if(paintSetting.m_heightThreshold < 1.0f)
				{
					noiseLevel = max(0.0f, (noiseLevel - paintSetting.m_heightThreshold)) / (1.0f - paintSetting.m_heightThreshold);
				}

				// Tamper noise level by the height offset
				noiseLevel = noiseLevel + paintSetting.m_heightOffset;

				// Calculate new height level based on noise level
				REAL newHeight = 0.0f;
				if(paintSetting.m_heightAutoLevelOn)
				{
					newHeight = paintSetting.m_heightAutoLevel - m_heightAutoOffset[x + z * m_heightFieldDimension] + (noiseLevel * paintSetting.m_heightMultiplier);
					m_heightAutoOffset[x + z * m_heightFieldDimension] = 0.0f;
				}
				else
				{
					newHeight = m_heightField[x + z * m_heightFieldDimension] - m_heightAutoOffset[x + z * m_heightFieldDimension] + (noiseLevel * paintSetting.m_heightMultiplier);
					m_heightAutoOffset[x + z * m_heightFieldDimension] = noiseLevel * paintSetting.m_heightMultiplier;
				}

				// Modify new height level by the falloff function
				REAL heightFalloff = paintSetting.EvaluateFalloff(paintSetting.m_heightFalloffType, distance, radius);
				newHeight = newHeight * heightFalloff + m_heightField[x + z * m_heightFieldDimension] * (1.0f - heightFalloff); 

				// Modify terrain based on boolean mode
				switch(paintSetting.m_heightBooleanType)
				{
					case ruTUBOOLEANTYPE_OVERWRITE:
						{
							m_heightField[x + z * m_heightFieldDimension] = newHeight;
							m_heightAutoPainted[x + z * m_heightFieldDimension] = 1;
						}

						break;
					case ruTUBOOLEANTYPE_UNION:
						{
							if(paintSetting.m_heightMultiplier >= 0.0f)
							{
								if(newHeight > m_heightField[x + z * m_heightFieldDimension])
								{
									m_heightField[x + z * m_heightFieldDimension] = newHeight;
									m_heightAutoPainted[x + z * m_heightFieldDimension] = 1;
								}
							}
							else
							{
								if(newHeight < m_heightField[x + z * m_heightFieldDimension])
								{
									m_heightField[x + z * m_heightFieldDimension] = newHeight;
									m_heightAutoPainted[x + z * m_heightFieldDimension] = 1;
								}
							}
						}

						break;
					case ruTUBOOLEANTYPE_INTERSECTION:
						{
							if(newHeight < m_heightField[x + z * m_heightFieldDimension])
							{
								m_heightField[x + z * m_heightFieldDimension] = newHeight;
								m_heightAutoPainted[x + z * m_heightFieldDimension] = 1;
							}
						}

						break;
				}
			}
		}
	}

	// Update main body vertices
	UpdateVertices();

	// Update skirt vertices
	UpdateSkirtVertices();

	// Change mesh iteration
	m_mesh->IncrementMeshIteration();
	m_outlineMesh->IncrementMeshIteration();

	// Recalculate bounds and normals
	CalculateBounds();
	CalculateNormals();

	// temp
//	m_colStructure->RelaxTree();

	return TRUE;
}

BOOL CRuTerrain_Chunk::PaintMask(const CRuVector3 &center, float radius, BOOL maskOn)
{
	// Figure out the area to alter
	CRuVector3 relativeCenter = center - m_bounds.Minima();

	INT32 sX = max(0, (INT32) ((relativeCenter.x - radius) / m_gridSize));
	INT32 sZ = max(0, (INT32) ((relativeCenter.z - radius) / m_gridSize));
	INT32 eX = min(m_dimension, (INT32) ((relativeCenter.x + radius) / m_gridSize + 1));
	INT32 eZ = min(m_dimension, (INT32) ((relativeCenter.z + radius) / m_gridSize + 1));

	for(INT32 z = sZ; z <= eZ; ++z)
	{
		for(INT32 x = sX; x <= eX; ++x)
		{
			CRuVector3 pointPos(x * m_gridSize, relativeCenter.m_y, z * m_gridSize);
			REAL distance = (relativeCenter - pointPos).Magnitude();

			if(distance <= radius)
			{
				// Alter alpha
				if(maskOn)
				{
					m_vertexFlags[x + z * (m_dimension + 1)] = 0x00;
				}
				else
				{
					m_vertexFlags[x + z * (m_dimension + 1)] = 0xFF;
				}
			}
		}
	}

	BuildChunkTopology();

	// Change mesh iteration
	m_mesh->IncrementTopologyIteration();

	return TRUE;
}

BOOL CRuTerrain_Chunk::PaintOffset(const CRuVector3 &center, float radius, REAL offset)
{
	if(m_offsetPainted == FALSE)
	{
		m_offsetPainted = TRUE;

		// Offset height
		Mojo_OffsetHeight(m_hfObject, offset);
	}

	// Update main body vertices
	UpdateVertices();

	// Update skirt vertices
	UpdateSkirtVertices();

	// Change mesh iteration
	m_mesh->IncrementMeshIteration();
	m_outlineMesh->IncrementMeshIteration();

	// Recalculate bounds and normals
	CalculateBounds();
	CalculateNormals();

	return TRUE;
}

BOOL CRuTerrain_Chunk::PaintSplat(const CRuVector3 &center, float radius, REAL power, RuFalloffFunction falloffFunc, INT32 iteration, UINT16 splatID)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	BOOL splatChanged = FALSE;

	m_splatIDMap->ClearTexelAlterationMap(iteration);
	if(m_splatIDMap->PaintSplat(localCenter, radius, power, falloffFunc, splatID))
	{
		splatChanged = TRUE;
	}

	return splatChanged;
}

BOOL CRuTerrain_Chunk::SprayPaintSplat(const CRuVector3 &center, float radius, REAL power, RuFalloffFunction falloffFunc, INT32 iteration, UINT16 splatID, float sprayProbability)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	BOOL splatChanged = FALSE;

	m_splatIDMap->ClearTexelAlterationMap(iteration);
	if(m_splatIDMap->SprayPaintSplat(localCenter, radius, power, falloffFunc, splatID, sprayProbability))
	{
		splatChanged = TRUE;
	}

	return splatChanged;
}

BOOL CRuTerrain_Chunk::BlockFillSplat(const CRuVector3 &center, float radius, UINT16 splatID)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	BOOL splatChanged = FALSE;

	if(m_splatIDMap->BlockFillSplat(localCenter, radius, splatID))
	{
		splatChanged = TRUE;
	}

	return splatChanged;
}

BOOL CRuTerrain_Chunk::FloodFillSplat(const CRuVector3 &center, float radius, UINT16 splatID)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	BOOL splatChanged = FALSE;
/*
	if(m_splatIDMap->BlockFillSplat(center, radius, splatID))
	{
		splatChanged = TRUE;
	}
*/
	return splatChanged;
}

BOOL CRuTerrain_Chunk::SmudgeSplat(const CRuVector3 &center, float radius, float smudgeProbability)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	BOOL splatChanged = FALSE;

	if(m_splatIDMap->SmudgeSplat(localCenter, radius, smudgeProbability))
	{
		splatChanged = TRUE;
	}

	return splatChanged;
}

BOOL CRuTerrain_Chunk::ReplaceSplat(const CRuVector3 &center, float radius, UINT16 splatID)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	BOOL splatChanged = FALSE;

	if(m_splatIDMap->ReplaceSplat(localCenter, radius, splatID))
	{
		splatChanged = TRUE;
	}

	return splatChanged;
}

BOOL CRuTerrain_Chunk::AttractSplat(const CRuVector3 &center, float radius, UINT16 attractedSplatID, float attractProbability)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	BOOL splatChanged = FALSE;

	if(m_splatIDMap->AttractSplat(localCenter, radius, attractedSplatID, attractProbability))
	{
		splatChanged = TRUE;
	}

	return splatChanged;
}

UINT16 CRuTerrain_Chunk::PickSplatID(const CRuVector3 &center)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	return m_splatIDMap->PickSplatID(localCenter);
}

void CRuTerrain_Chunk::SetTexture(INT32 layer, IRuBaseTexture *texture, REAL tileScale)
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

BOOL CRuTerrain_Chunk::MojoPaint(const CRuVector3 &center, float radius, CRuMojo_AttributeParcel<REAL> *refHF, CRuFractal_Base *srcFractal, const CRuSplatIDMap_PaintSetting &paintSetting)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	BOOL splatChanged = FALSE;

	if(m_splatIDMap->MojoPaint(localCenter, radius, refHF, m_hfObject, srcFractal, paintSetting))
	{
		splatChanged = TRUE;
	}

	return splatChanged;
}

void CRuTerrain_Chunk::PaintDoodad(const CRuVector3 &center, const CRuWorld_DoodadPaintSetting &paintSetting, CRuEntity *colObject)
{
	// Anchor new position to ground (this also checks for containment at the same time)
	CRuCollisionQuery_Segment_Nearest upQuery;
	upQuery.m_colSegment = CRuSegment3(CRuVector3(center.x, center.y - 0.1f, center.z), CRuVector3(center.x, ruMAXFLOATRANGE, center.z));
	upQuery.m_exclusionMask = paintSetting.m_colMask;

	CRuCollisionQuery_Segment_Nearest downQuery;
	downQuery.m_colSegment = CRuSegment3(CRuVector3(center.x, center.y + 0.1f, center.z), CRuVector3(center.x, -ruMAXFLOATRANGE, center.z));
	downQuery.m_exclusionMask = paintSetting.m_colMask;

	colObject->QueryCollision(&upQuery);
	colObject->QueryCollision(&downQuery);

	REAL colDist;
	CRuVector3 colPoint;
	CRuVector3 colNormal;

	if(upQuery.m_collisionT < downQuery.m_collisionT)
	{
		colDist = upQuery.m_collisionT;
		colPoint = upQuery.m_collisionPoint;
		colNormal = upQuery.m_collisionNormal;
	}
	else
	{
		colDist = downQuery.m_collisionT;
		colPoint = downQuery.m_collisionPoint;
		colNormal = downQuery.m_collisionNormal;
	}

	if(colDist < FLT_MAX)
	{
		CRuWorld_DoodadCluster *doodadCluster = GetDoodadListDescriptor(paintSetting.m_guid);

		CRuDoodad_Descriptor doodadDescriptor;
		doodadDescriptor.m_textureIndex = paintSetting.m_textureIndex;
		doodadDescriptor.m_scale = (paintSetting.m_maxScale - paintSetting.m_minScale) * RuGlobalMTRand().GenerateFloat() + paintSetting.m_minScale;

		if(paintSetting.m_randomRotation)
		{
			doodadDescriptor.m_rotation.FromAngleAxis(CRuVector3(0.0f, 1.0f, 0.0f), RuGlobalMTRand().GenerateFloat() * ruPI * 2.0f);
		}
		else
		{
			doodadDescriptor.m_rotation.FromAngleAxis(CRuVector3(0.0f, 1.0f, 0.0f), 0.0f);
		}

		if(paintSetting.m_alignUpVector)
		{
			CRuQuaternion alignQuat;

			alignQuat.RotateIntoVector(colNormal, CRuVector3(0.0f, 1.0f, 0.0f));

			doodadDescriptor.m_rotation = doodadDescriptor.m_rotation * alignQuat;
		}

		doodadDescriptor.m_translation = colPoint;
		doodadDescriptor.m_swayMagnitude = paintSetting.m_swayMagnitude;

		doodadCluster->m_doodads.Add(doodadDescriptor);
	}
}

void CRuTerrain_Chunk::SprayDoodad(const CRuVector3 &center, REAL radius, INT32 numDoodads, const CRuWorld_DoodadPaintSetting &paintSetting, CRuEntity *colObject)
{
	CRuWorld_DoodadCluster *doodadCluster = GetDoodadListDescriptor(paintSetting.m_guid);

	for(INT32 i = 0; i < numDoodads; ++i)
	{
		REAL r = RuGlobalMTRand().GenerateFloat() * radius;
		REAL theta = RuGlobalMTRand().GenerateFloat() * ruPI * 2.0f;

		CRuVector3 position(0.0f, 0.0f, r);

		CRuQuaternion rotQuat;
		rotQuat.FromAngleAxis(CRuVector3(0.0f, 1.0f, 0.0f), theta);

		rotQuat.GetMatrix().TransformPoint(position);

		position = position + center;

		// Anchor new position to ground (this also checks for containment at the same time)
		CRuCollisionQuery_Segment_Nearest upQuery;
		upQuery.m_colSegment = CRuSegment3(CRuVector3(position.x, position.y - 0.1f, position.z), CRuVector3(position.x, position.y + radius, position.z));
		upQuery.m_exclusionMask = paintSetting.m_colMask;

		CRuCollisionQuery_Segment_Nearest downQuery;
		downQuery.m_colSegment = CRuSegment3(CRuVector3(position.x, position.y + 0.1f, position.z), CRuVector3(position.x, position.y - radius, position.z));
		downQuery.m_exclusionMask = paintSetting.m_colMask;

		colObject->QueryCollision(&upQuery);
		colObject->QueryCollision(&downQuery);

		REAL colDist;
		CRuVector3 colPoint;
		CRuVector3 colNormal;

		if(upQuery.m_collisionT < downQuery.m_collisionT)
		{
			colDist = upQuery.m_collisionT;
			colPoint = upQuery.m_collisionPoint;
			colNormal = upQuery.m_collisionNormal;
		}
		else
		{
			colDist = downQuery.m_collisionT;
			colPoint = downQuery.m_collisionPoint;
			colNormal = downQuery.m_collisionNormal;
		}

		if(colDist < FLT_MAX)
		{
			// Create new doodad
			CRuDoodad_Descriptor doodadDescriptor;
			doodadDescriptor.m_textureIndex = paintSetting.m_textureIndex;
			doodadDescriptor.m_scale = (paintSetting.m_maxScale - paintSetting.m_minScale) * RuGlobalMTRand().GenerateFloat() + paintSetting.m_minScale;

			if(paintSetting.m_randomRotation)
			{
				doodadDescriptor.m_rotation.FromAngleAxis(CRuVector3(0.0f, 1.0f, 0.0f), RuGlobalMTRand().GenerateFloat() * ruPI * 2.0f);
			}
			else
			{
				doodadDescriptor.m_rotation.FromAngleAxis(CRuVector3(0.0f, 1.0f, 0.0f), 0.0f);
			}

			if(paintSetting.m_alignUpVector)
			{
				CRuQuaternion alignQuat;

				alignQuat.RotateIntoVector(colNormal, CRuVector3(0.0f, 1.0f, 0.0f));

				doodadDescriptor.m_rotation = doodadDescriptor.m_rotation * alignQuat;
			}

			doodadDescriptor.m_translation = colPoint;
			doodadDescriptor.m_swayMagnitude = paintSetting.m_swayMagnitude;

			doodadCluster->m_doodads.Add(doodadDescriptor);
		}
	}
}

void CRuTerrain_Chunk::DeleteDoodad(const CRuVector3 &center, REAL radius)
{
	REAL sqrRadius = radius * radius;

	for(INT32 i = 0; i < m_doodadLists.Count(); ++i)
	{
		CRuWorld_DoodadCluster &doodadCluster = *m_doodadLists[i];

		INT32 numDeleted = 0;
		for(INT32 j = 0; j < doodadCluster.m_doodads.Count(); ++j)
		{
			REAL sqrDist = (doodadCluster.m_doodads[j].m_translation - center).SquaredMagnitude();

			if(sqrDist <= sqrRadius)
			{
				// Move a doodad from the back to overwrite this slot
				doodadCluster.m_doodads[j] = doodadCluster.m_doodads[doodadCluster.m_doodads.Count() - 1];

				// Remove last entry
				doodadCluster.m_doodads.RemoveAt(doodadCluster.m_doodads.Count() - 1);

				// Update counters
				++numDeleted;
				--j;
			}
		}
	}
}

void CRuTerrain_Chunk::RealignDoodad(const CRuVector3 &center, REAL radius)
{
	REAL sqrRadius = radius * radius;

	for(INT32 i = 0; i < m_doodadLists.Count(); ++i)
	{
		CRuWorld_DoodadCluster &doodadCluster = *m_doodadLists[i];

		INT32 numDeleted = 0;
		for(INT32 j = 0; j < doodadCluster.m_doodads.Count(); ++j)
		{
			REAL sqrDist = (doodadCluster.m_doodads[j].m_translation - center).SquaredMagnitude();

			if(sqrDist <= sqrRadius)
			{
				CRuVector3 position = doodadCluster.m_doodads[j].m_translation;

				// Anchor new position to ground (this also checks for containment at the same time)
				CRuCollisionQuery_Segment_Nearest segQuery;
				segQuery.m_colSegment = CRuSegment3(CRuVector3(position.x, ruMAXFLOATRANGE, position.z), CRuVector3(position.x, -ruMAXFLOATRANGE, position.z));

				QueryCollision(&segQuery);

				if(segQuery.m_positiveHit)
				{
					doodadCluster.m_doodads[j].m_translation.m_y = segQuery.m_collisionPoint.m_y;
				}
			}
		}
	}
}

INT32 CRuTerrain_Chunk::GetNumDoodadClusters()
{
	return m_doodadLists.Count();
}

CRuWorld_DoodadCluster *CRuTerrain_Chunk::GetDoodadCluster(INT32 index)
{
	return m_doodadLists[index];
}

CRuWorld_DoodadCluster *CRuTerrain_Chunk::GetDoodadListDescriptor(const CRuGUID &guid)
{
	CRuWorld_DoodadCluster *doodadCluster = NULL;

	// Find appropriate doodad list to use
	for(INT32 i = 0; i < m_doodadLists.Count(); ++i)
	{
		if(m_doodadLists[i]->m_guid == guid)
		{
			doodadCluster = m_doodadLists[i];
			break;
		}
	}

	// No list was found, create a new one
	if(doodadCluster == NULL)
	{
		doodadCluster = ruNEW CRuWorld_DoodadCluster();
		doodadCluster->m_guid = guid;
		m_doodadLists.Add(doodadCluster);
	}

	return doodadCluster;
}

INT32 CRuTerrain_Chunk::GetNumWaterLayers() const
{
	return m_waterLayers.Count();
}

CRuEntity *CRuTerrain_Chunk::GetWaterLayerEntity(INT32 index)
{
	if(index >= 0 && index < m_waterLayers.Count())
	{
		return m_waterLayers[index];
	}

	return NULL;
}

CRuWorld_Water *CRuTerrain_Chunk::GetNearestWaterLayer(const CRuSegment3 &segment, CRuVector3 &intPtOut, float &intDistOut)
{
	CRuMatrix4x4 identityTransform;
	identityTransform.SetToIdentity();

	CRuWorld_Water *nearestWaterLayer = NULL;
	CRuVector3 nearestPoint;
	float nearestDistance = FLT_MAX;

	for(INT32 i = 0; i < m_waterLayers.Count(); ++i)
	{
		CRuCollisionQuery_Segment_Nearest segQuery;
		CRuMatrix4x4 identityMatrix;
		segQuery.m_worldTransform = &identityMatrix;
		segQuery.m_invWorldTransform = &identityMatrix;
		segQuery.m_ignoreCollisionFlag = TRUE;
		segQuery.m_colSegment = segment;

		// Perform collision query
		m_waterLayers[i]->QueryCollision(&segQuery);

		if(segQuery.m_positiveHit)
		{
			// If the nearest intersection is the current nearest, use it
			if(segQuery.m_collisionT < nearestDistance)
			{
				nearestWaterLayer = m_waterLayers[i];
				nearestPoint = segQuery.m_collisionPoint;
				nearestDistance = segQuery.m_collisionT;
			}
		}
	}

	if(nearestWaterLayer)
	{
		intPtOut = nearestPoint;
		intDistOut = nearestDistance;
	}

	return nearestWaterLayer;
}

CRuWorld_Water *CRuTerrain_Chunk::GetWaterLayerByGUID(const CRuGUID &waterBodyGUID)
{
	for(INT32 i = 0; i < m_waterLayers.Count(); ++i)
	{
		if(m_waterLayers[i]->GetWaterBodyGUID() == waterBodyGUID)
		{
			return m_waterLayers[i];
		}
	}

	return NULL;
}

CRuWorld_Water *CRuTerrain_Chunk::GetWaterLayerByIndex(INT32 index)
{
	return m_waterLayers[index];
}

INT32 CRuTerrain_Chunk::GetWaterLayerIndexByGUID(const CRuGUID &waterBodyGUID)
{
	for(INT32 i = 0; i < m_waterLayers.Count(); ++i)
	{
		if(m_waterLayers[i]->GetWaterBodyGUID() == waterBodyGUID)
		{
			return i;
		}
	}

	return -1;
}

BOOL CRuTerrain_Chunk::PaintWaterHeight(const CRuGUID &waterBodyGUID, const CRuVector3 &center, float radius, float delta, RuFalloffFunction falloffFunction)
{
	CRuWorld_Water *waterLayer = GetWaterLayerByGUID(waterBodyGUID);

	if(waterLayer == NULL)
		return FALSE;

	waterLayer->PaintHeight(center, radius, delta, falloffFunction);

	// Delete the water layer if the water model has no primitives
	DeleteWaterLayerIfUnused(waterBodyGUID);

	return TRUE;
}

BOOL CRuTerrain_Chunk::SetWaterHeight(const CRuGUID &waterBodyGUID, const CRuVector3 &center, float radius, float height, float brushStrength, RuFalloffFunction falloffFunction)
{
	CRuWorld_Water *waterLayer = GetWaterLayerByGUID(waterBodyGUID);

	if(waterLayer == NULL)
		return FALSE;

	waterLayer->SetHeight(center, radius, height, brushStrength, falloffFunction);

	// Delete the water layer if the water model has no primitives
	DeleteWaterLayerIfUnused(waterBodyGUID);

	return TRUE;
}

BOOL CRuTerrain_Chunk::SmoothWaterHeight(const CRuGUID &waterBodyGUID, const CRuVector3 &center, float radius, float delta)
{
	CRuWorld_Water *waterLayer = GetWaterLayerByGUID(waterBodyGUID);

	if(waterLayer == NULL)
		return FALSE;

	waterLayer->SmoothHeight(center, radius, delta);

	// Delete the water layer if the water model has no primitives
	DeleteWaterLayerIfUnused(waterBodyGUID);

	return TRUE;
}

BOOL CRuTerrain_Chunk::PaintWaterSurface(const CRuGUID &waterBodyGUID, const CRuVector3 &center, float radius, BOOL waterVisible, REAL waterBodyTessellationScale)
{
	CRuWorld_Water *waterLayer = GetWaterLayerByGUID(waterBodyGUID);

	if(waterLayer == NULL)
	{
		REAL waterBodySpacing = m_gridSize / waterBodyTessellationScale;
		INT32 waterBodyTessellation = static_cast<INT32>(m_dimension * waterBodyTessellationScale) + 1;

		// Create new water layer
		CRuVector3 waterPosition = this->GetTranslation();
		waterPosition.m_y = center.m_y;
		waterLayer = ruNEW CRuWorld_Water();
		waterLayer->Allocate(waterBodyGUID, waterPosition, waterBodyTessellation, waterBodySpacing, 1);

		// Append to water layer list
		m_waterLayers.Add(waterLayer);
	}

	// Perform flag painting
	waterLayer->PaintSurface(center, radius, waterVisible);

	// Delete the water layer if the water model has no primitives
	DeleteWaterLayerIfUnused(waterBodyGUID);

	return TRUE;
}

BOOL CRuTerrain_Chunk::PaintWaterBodyAlpha(const CRuGUID &waterBodyGUID, const CRuVector3 &center, REAL radius, REAL alpha, REAL delta, RuFalloffFunction falloffFunction)
{
	CRuWorld_Water *waterLayer = GetWaterLayerByGUID(waterBodyGUID);

	if(waterLayer == NULL)
		return FALSE;

	waterLayer->PaintAlpha(center, radius, alpha, delta, falloffFunction);

	return TRUE;
}

BOOL CRuTerrain_Chunk::PaintWaterBodyColor(const CRuGUID &waterBodyGUID, const CRuVector3 &center, REAL radius, RUCOLOR color, REAL delta, RuFalloffFunction falloffFunction)
{
	CRuWorld_Water *waterLayer = GetWaterLayerByGUID(waterBodyGUID);

	if(waterLayer == NULL)
		return FALSE;

	waterLayer->PaintColor(center, radius, color, delta, falloffFunction);

	return TRUE;
}

RUCOLOR CRuTerrain_Chunk::PickWaterBodyColor(const CRuGUID &waterBodyGUID, const CRuVector3 &center)
{
	CRuWorld_Water *waterLayer = GetWaterLayerByGUID(waterBodyGUID);

	if(waterLayer == NULL)
		return 0xFFFFFFFF;

	return waterLayer->PickColor(center);
}

BOOL CRuTerrain_Chunk::StitchWaterLayers(INT32 neighborID, CRuTerrain_Chunk *chunk)
{
	if(chunk == NULL)
		return FALSE;

	// Iterate through each water layer at this chunk
	for(INT32 i = 0; i < m_waterLayers.Count(); ++i)
	{
		// Find matching water layer at the neighbor chunk
		for(INT32 j = 0; j < chunk->m_waterLayers.Count(); ++j)
		{
			if(m_waterLayers[i]->GetWaterBodyGUID() == chunk->m_waterLayers[j]->GetWaterBodyGUID())
			{
				// Stitch water layer
				m_waterLayers[i]->StitchWaterLayer(neighborID, chunk->m_waterLayers[j]);

				break;
			}
		}
	}

	return TRUE;
}

BOOL CRuTerrain_Chunk::DeleteWaterLayerIfUnused(const CRuGUID &waterBodyGUID)
{
	INT32 layerIndex = GetWaterLayerIndexByGUID(waterBodyGUID);

	if(layerIndex >= 0)
	{
		// If the water mesh has no valid faces, destroy the water layer
		if(m_waterLayers[layerIndex]->ValidateMesh() == FALSE)
		{
			ruSAFE_RELEASE(m_waterLayers[layerIndex]);

			// Remove descriptor
			m_waterLayers.RemoveAt(layerIndex);

			return TRUE;
		}
	}

	return FALSE;
}

INT32 CRuTerrain_Chunk::GetChunkSize(INT32 dimension, INT32 splatResolution)
{
	INT32 chunkSize = 0;

	// GUID size
	chunkSize += 16;

	// Chunk dimension
	chunkSize += sizeof(INT32);

	// Chunk grid size
	chunkSize += sizeof(float);

	// Chunk lod level count
	chunkSize += sizeof(INT32);

	// Chunk origin offsets
	chunkSize += sizeof(float) * 2;

	// Height field minima and maxima
	chunkSize += sizeof(float) * 2;

	// Height field
	INT32 numVertices = (dimension + 3) * (dimension + 3);
	chunkSize += sizeof(UINT16) * numVertices;

	// Alpha
	chunkSize += sizeof(BYTE) * (dimension + 1) * (dimension + 1);

	// Splat resolution
	chunkSize += sizeof(INT32);

	// Splat data
	chunkSize += CRuSplatIDMap::GetSplatIDMapSize(splatResolution + 8);

	return chunkSize;
}

void CRuTerrain_Chunk::CalculateBounds()
{
	CRuVector3 *vertices = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();

	CRuVector3 origin = this->GetTranslation();

	// Adjust bounding box
	m_bounds.SetBounds(vertices[0], vertices[0]);
	for(INT32 i = 1, numRealVertices = m_numVertices; i < numRealVertices; ++i)
	{
		m_bounds.Merge(vertices[i]);
	}

	// Adjust height field bounds
	m_heightFieldBounds.Set(	
								origin.m_x - m_gridSize,
								0.0f,
								origin.m_z - m_gridSize,
								origin.m_x + m_dimension * m_gridSize + m_gridSize,
								0.0f,
								origin.m_z + m_dimension * m_gridSize + m_gridSize
							);

	// Adjust bounding sphere for the chunk entity
	CRuSphere localBounds((m_bounds.Maxima() + m_bounds.Minima()) / 2.0f, (m_bounds.Maxima() - m_bounds.Minima()).Magnitude() / 2.0f);
	this->SetLocalEntityBounds(localBounds);

	// Override bounding sphere for the chunk mesh
	m_mesh->SetBounds(localBounds);

	// Transform bounding box into world space WRONG WRONG WRONG!!! use local translation! not the origin!
	m_bounds.Minima() += origin;
	m_bounds.Maxima() += origin;
}

void CRuTerrain_Chunk::CalculateNormals()
{
	CRuVector3 *vertices = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
	CRuVector3 *normals = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetNormal();

	float normal0_h1;
	float normal0_h2;
	float normal0_h3;
	float normal0_h4;
	float normal0_Y = 2.0f * m_gridSize;
	CRuVector3 normal0;

	CRuVector3 v1, v2, v3, v4;
	CRuVector3 n1, n2, n3, n4;

	// Calculate normals
	for(INT32 z = 0, srcIdx = m_heightFieldDimension + 1, destIdx = 0; z <= m_dimension; ++z, srcIdx += 2)
	{
		for(INT32 x = 0; x <= m_dimension; ++x, ++srcIdx, ++destIdx)
		{
			// Normal 0 - along orthogonal axis

			normal0_h1 = m_heightField[srcIdx + 1];
			normal0_h2 = m_heightField[srcIdx - m_heightFieldDimension];
			normal0_h3 = m_heightField[srcIdx - 1];
			normal0_h4 = m_heightField[srcIdx + m_heightFieldDimension];

			normal0.Set(normal0_h3 - normal0_h1, normal0_Y, normal0_h2 - normal0_h4);
			normal0.Normalize();

/*
			// Slow way of calculating normals
			normal0_h1 = m_heightField[srcIdx + 1].m_y - m_heightField[srcIdx].m_y;
			normal0_h2 = m_heightField[srcIdx - m_heightFieldDimension].m_y - m_heightField[srcIdx].m_y;
			normal0_h3 = m_heightField[srcIdx - 1].m_y - m_heightField[srcIdx].m_y;
			normal0_h4 = m_heightField[srcIdx + m_heightFieldDimension].m_y - m_heightField[srcIdx].m_y;

			v1.Set(10.0f, normal0_h1, 0.0f);
			v2.Set(0.0f, normal0_h2, -10.0f);
			v3.Set(-10.0f, normal0_h3, 0.0f);
			v4.Set(0.0f, normal0_h4, 10.0f);

			v1.Normalize();
			v2.Normalize();
			v3.Normalize();
			v4.Normalize();

			n1.SetToCrossProduct(v1, v2);
			n2.SetToCrossProduct(v2, v3);
			n3.SetToCrossProduct(v3, v4);
			n4.SetToCrossProduct(v4, v1);

			n1.Normalize();
			n2.Normalize();
			n3.Normalize();
			n4.Normalize();

			normal0 = (n1 + n2 + n3 + n4) / 4.0f;
*/          
			normals[destIdx] = normal0;
		}
	}
}

void CRuTerrain_Chunk::CalculateChunkParameters()
{
	// Calculate height field dimension
	m_heightFieldDimension = m_dimension + 3;

	// Number of vertices = (m_dimension + 1)^2 + (m_dimension + 1) * 4
	m_numVertices = (m_dimension + 1) * (m_dimension + 1) + (m_dimension + 1) * 4;

	// Calculate number of indices
	m_numIndices = 0;
	for(INT32 lod = 0, curDimension = m_dimension; lod < m_lodLevels; ++lod)
	{
		// Number of indices per LOD level = (LOD dimension^2) * 6 + skirt indices(LOD dimension * 6 * 4)
		m_numIndices += ((m_dimension * m_dimension * 2) * 3) + (((m_dimension * 2) * 3) * 4);
	}
}

void CRuTerrain_Chunk::InitializeMeshVertices(float xOrigin, float zOrigin)
{
	// Set chunk origin
	m_origin.Set(xOrigin, 0.0f, zOrigin);

	// Allocate height field
	m_hfObject = ruNEW CRuMojo_AttributeParcel<REAL>(m_heightFieldDimension, m_heightFieldDimension);
	m_hfObject->SetParcelOrigin(CRuVector3(-m_gridSize, 0.0f, -m_gridSize));
	m_hfObject->SetParcelSpacing(m_gridSize);
	m_hfObject->SetParcelOverlapGap(1);
	m_heightField = m_hfObject->GetParcel();

	// Allocate height auto-offset field
	m_heightAutoOffset = ruNEW REAL [m_heightFieldDimension * m_heightFieldDimension];

	// Allocate auto-painted flag field
	m_heightAutoPainted = ruNEW BYTE [m_heightFieldDimension * m_heightFieldDimension];

	// Allocate height field flags
	m_vertexFlags = ruNEW BYTE [(m_dimension + 1) * (m_dimension + 1)];
	memset(m_vertexFlags, 0xFF, sizeof(BYTE) * (m_dimension + 1) * (m_dimension + 1));

	// Allocate mesh
	m_mesh = ruNEW CRuMesh();
	m_mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, m_numVertices, m_numIndices / 3, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_NORMAL | ruVERTEXFORMAT_DIFFUSE);
	m_mesh->AddTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);

	// Initialize height field to zero
	for(INT32 z = 0, vertexIndex = 0; z < m_heightFieldDimension; ++z)
	{
		for(INT32 x = 0; x < m_heightFieldDimension; ++x, ++vertexIndex)
		{
			m_heightField[vertexIndex] = 0.0f;
		}
	}

	// Initialize height auto-offset to zero
	memset(m_heightAutoOffset, 0, sizeof(REAL) * m_heightFieldDimension * m_heightFieldDimension);

	memset(m_heightAutoPainted, 0, sizeof(BYTE) * m_heightFieldDimension * m_heightFieldDimension);

	// Initialize mesh vertices
	CRuVector3 *vertices = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetDiffuse();
	float *uv = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
	for(INT32 z = 0, i = 0, srcIdx = m_heightFieldDimension + 1, destIdx = 0; z <= m_dimension; ++z, srcIdx += 2)
	{
		for(INT32 x = 0; x <= m_dimension; ++x, ++destIdx, ++srcIdx)
		{
			// Copy vertex data
			vertices[destIdx].Set(x * m_gridSize, m_heightField[srcIdx], z * m_gridSize);

			// Set diffuse
			diffuse[destIdx] = 0xFFFFFFFF;

			// Set uv data
			uv[i++] = (float) x / m_dimension;
			uv[i++] = (float) z / m_dimension;
		}
	}

	// Update skirt vertices
	UpdateSkirtVertices();

	// Allocate outline mesh
	m_outlineMesh = ruNEW CRuMesh();
	m_outlineMesh->Initialize(ruPRIMTYPE_LINELIST, (m_dimension + 1) * 4, m_dimension * 4, ruVERTEXFORMAT_POSITION, 1);

	CRuVector3 *srcVertices = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
	vertices = static_cast<CRuMorphTarget *>(m_outlineMesh->GetMorphTarget(0))->GetPosition();

	INT32 destIdx2 = 0;
	for(INT32 i = 0, srcIdx = 0; i <= m_dimension; ++i, ++srcIdx, ++destIdx2)
	{
		// Copy vertex data
		vertices[destIdx2] = srcVertices[srcIdx];
	}

	for(INT32 i = 0, srcIdx = m_dimension * (m_dimension + 1); i <= m_dimension; ++i, ++srcIdx, ++destIdx2)
	{
		// Copy vertex data
		vertices[destIdx2] = srcVertices[srcIdx];
	}

	for(INT32 i = 0, srcIdx = 0; i <= m_dimension; ++i, srcIdx += (m_dimension + 1), ++destIdx2)
	{
		// Copy vertex data
		vertices[destIdx2] = srcVertices[srcIdx];
	}

	for(INT32 i = 0, srcIdx = m_dimension; i <= m_dimension; ++i, srcIdx += (m_dimension + 1), ++destIdx2)
	{
		// Copy vertex data
		vertices[destIdx2] = srcVertices[srcIdx];
	}
}

void CRuTerrain_Chunk::BuildChunkTopology()
{
	m_material = ruNEW CRuMaterial();
	m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_TERRAIN"));

	// Set default values for UV stretch scales
	for(INT32 i = 0; i < 6; ++i)
	{
		m_textureUVScale[i][0] = 24.0f;
		m_textureUVScale[i][1] = -24.0f;
		m_textureUVScale[i][2] = 1.0f;
		m_textureUVScale[i][3] = 1.0f;
	}
/*
	m_textureUVScale[1][0] = 8.0f;
	m_textureUVScale[1][1] = -8.0f;
	m_textureUVScale[1][2] = 1.0f;
	m_textureUVScale[1][3] = 1.0f;

	m_textureUVScale[2][0] = 2.0f;
	m_textureUVScale[2][1] = -2.0f;
	m_textureUVScale[2][2] = 2.0f;
	m_textureUVScale[2][3] = 2.0f;
*/
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_RAW_OFFSET, 0);
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_RAW_COUNT, 6);
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_RAW_DATA, reinterpret_cast<PTRVALUE>(m_textureUVScale));

	// Clear all batches in the mesh (we will rebuild all of the batches)
	m_mesh->ClearBatches();

	// Start building the LOD mesh at the base level
	UINT16 *indices = m_mesh->GetIndices();
	INT32 indexCount = 0;

	m_lodMeshes = ruNEW CRuProxyMesh * [m_lodLevels];

	for(INT32 lod = 0, vertexGap = 1, gridDimension = m_dimension; lod < m_lodLevels; ++lod, vertexGap *= 2, gridDimension /= 2)
	{
		INT32 batchNumIndices = BuildGridIndices(gridDimension, vertexGap, m_dimension + 1, &indices[indexCount]);
		INT32 batchOffset = indexCount;
		INT32 batchIndex = m_mesh->AddBatch(batchOffset / 3, batchNumIndices / 3);

		// Setup the LOD proxy mesh
		m_lodMeshes[lod] = ruNEW CRuProxyMesh();
		m_lodMeshes[lod]->SetProxiedMesh(m_mesh, batchIndex);

		indexCount += batchNumIndices;
	}

	// Setup outline material
	m_outlineMaterial = ruNEW CRuMaterial();
	m_outlineMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_NULL"));

	// Setup indices for the outline mesh
	indices = m_outlineMesh->GetIndices();
	for(INT32 i = 0, k = 0; i < 4; ++i)
	{
		for(INT32 j = 0, l = i * (m_dimension + 1); j < m_dimension; ++j, ++l)
		{
			indices[k++] = l;
			indices[k++] = l + 1;
		}
	}
}

INT32 CRuTerrain_Chunk::BuildGridIndices(INT32 gridDimension, INT32 vertexGap, INT32 stride, UINT16 *indicesOut)
{
	INT32 evenFaceConstruction[]	= { 0, 3, 4, 0, 4, 1, 1, 4, 2, 2, 4, 5, 0xFF };
	INT32 oddFaceConstruction[]		= { 0, 3, 1, 1, 3, 4, 1, 5, 2, 1, 4, 5, 0xFF };

	INT32 curIndex = 0;
	UINT16 indexSet[6];

	INT32 zStride = stride * vertexGap;

	// Build indices for the mesh body
	for(INT32 z = 0; z < gridDimension; ++z)
	{
		// Build indices for four faces at a time
		for(INT32 x = 0; x < gridDimension; x += 2)
		{
			// Calculate the indices used in these four faces

			indexSet[0] = (z * zStride) + (x * vertexGap);
			indexSet[1] = indexSet[0] + vertexGap;
			indexSet[2] = indexSet[1] + vertexGap;
			indexSet[3] = indexSet[0] + zStride;
			indexSet[4] = indexSet[1] + zStride;
			indexSet[5] = indexSet[2] + zStride;

			// Alternate between even and odd rows
			INT32 *faceConstruction = (z % 2) ? oddFaceConstruction : evenFaceConstruction;

			for(INT32 j = 0; faceConstruction[j] != 0xFF; j += 6)
			{
				BOOL visible = FALSE;

				if(m_vertexFlags)
				{
					for(INT32 k = j; (k < j + 6) && faceConstruction[k] != 0xFF; ++k)
					{
						if(m_vertexFlags[indexSet[faceConstruction[k]]])
						{
							visible = TRUE;
							break;
						}
					}
				}
				else
				{
					visible = TRUE;
				}

				for(INT32 k = j; (k < j + 6) && faceConstruction[k] != 0xFF; ++k)
				{
					indicesOut[curIndex++] = visible ? indexSet[faceConstruction[k]] : 0;
				}
			}
		}
	}

	INT32 topSkirtVertexArrayOffset = (m_dimension + 1) * (m_dimension + 1);
	INT32 bottomSkirtVertexArrayOffset = topSkirtVertexArrayOffset + (m_dimension + 1);
	INT32 leftSkirtVertexArrayOffset = bottomSkirtVertexArrayOffset + (m_dimension + 1);
	INT32 rightSkirtVertexArrayOffset = leftSkirtVertexArrayOffset + (m_dimension + 1);

	// Build indices for the mesh skirt (top)
	for(INT32 x = 0, z = 0; x < gridDimension; ++x)
	{
		if(m_vertexFlags[(z * zStride) + (x * vertexGap)] == 0 && m_vertexFlags[(z * zStride) + (x * vertexGap) + vertexGap] == 0)
		{
			indicesOut[curIndex++] = 0;
			indicesOut[curIndex++] = 0;
			indicesOut[curIndex++] = 0;

			indicesOut[curIndex++] = 0;
			indicesOut[curIndex++] = 0;
			indicesOut[curIndex++] = 0;
		}
		else
		{
			indicesOut[curIndex++] = (z * zStride) + (x * vertexGap);
			indicesOut[curIndex++] = (z * zStride) + (x * vertexGap) + vertexGap;
			indicesOut[curIndex++] = (x * vertexGap) + topSkirtVertexArrayOffset;

			indicesOut[curIndex++] = (z * zStride) + (x * vertexGap) + vertexGap;
			indicesOut[curIndex++] = (x * vertexGap) + topSkirtVertexArrayOffset + vertexGap;
			indicesOut[curIndex++] = (x * vertexGap) + topSkirtVertexArrayOffset;
		}
	}

	// Build indices for the mesh skirt (bottom)
	for(INT32 x = 0, z = gridDimension; x < gridDimension; ++x)
	{
		if(m_vertexFlags[(z * zStride) + (x * vertexGap)] == 0 && m_vertexFlags[(z * zStride) + (x * vertexGap) + vertexGap] == 0)
		{
			indicesOut[curIndex++] = 0;
			indicesOut[curIndex++] = 0;
			indicesOut[curIndex++] = 0;

			indicesOut[curIndex++] = 0;
			indicesOut[curIndex++] = 0;
			indicesOut[curIndex++] = 0;
		}
		else
		{
			indicesOut[curIndex++] = (z * zStride) + (x * vertexGap);
			indicesOut[curIndex++] = (x * vertexGap) + bottomSkirtVertexArrayOffset;
			indicesOut[curIndex++] = (z * zStride) + (x * vertexGap) + vertexGap;

			indicesOut[curIndex++] = (z * zStride) + (x * vertexGap) + vertexGap;
			indicesOut[curIndex++] = (x * vertexGap) + bottomSkirtVertexArrayOffset;
			indicesOut[curIndex++] = (x * vertexGap) + bottomSkirtVertexArrayOffset + vertexGap;
		}
	}

	// Build indices for the mesh skirt (left)
	for(INT32 z = 0, x = 0; z < gridDimension; ++z)
	{
		if(m_vertexFlags[(z * zStride) + (x * vertexGap)] == 0 && m_vertexFlags[(z * zStride) + (x * vertexGap) + zStride] == 0)
		{
			indicesOut[curIndex++] = 0;
			indicesOut[curIndex++] = 0;
			indicesOut[curIndex++] = 0;

			indicesOut[curIndex++] = 0;
			indicesOut[curIndex++] = 0;
			indicesOut[curIndex++] = 0;
		}
		else
		{
			indicesOut[curIndex++] = (z * zStride) + (x * vertexGap);
			indicesOut[curIndex++] = (z * vertexGap) + leftSkirtVertexArrayOffset;
			indicesOut[curIndex++] = (z * zStride) + (x * vertexGap) + zStride;

			indicesOut[curIndex++] = (z * zStride) + (x * vertexGap) + zStride;
			indicesOut[curIndex++] = (z * vertexGap) + leftSkirtVertexArrayOffset;
			indicesOut[curIndex++] = (z * vertexGap) + leftSkirtVertexArrayOffset + vertexGap;
		}
	}

	// Build indices for the mesh skirt (right)
	for(INT32 z = 0, x = gridDimension; z < gridDimension; ++z)
	{
		if(m_vertexFlags[(z * zStride) + (x * vertexGap)] == 0 && m_vertexFlags[(z * zStride) + (x * vertexGap) + zStride] == 0)
		{
			indicesOut[curIndex++] = 0;
			indicesOut[curIndex++] = 0;
			indicesOut[curIndex++] = 0;

			indicesOut[curIndex++] = 0;
			indicesOut[curIndex++] = 0;
			indicesOut[curIndex++] = 0;
		}
		else
		{
			indicesOut[curIndex++] = (z * zStride) + (x * vertexGap);
			indicesOut[curIndex++] = (z * zStride) + (x * vertexGap) + zStride;
			indicesOut[curIndex++] = (z * vertexGap) + rightSkirtVertexArrayOffset;

			indicesOut[curIndex++] = (z * zStride) + (x * vertexGap) + zStride;
			indicesOut[curIndex++] = (z * vertexGap) + rightSkirtVertexArrayOffset + vertexGap;
			indicesOut[curIndex++] = (z * vertexGap) + rightSkirtVertexArrayOffset;
		}
	}

	return curIndex;
}

void CRuTerrain_Chunk::UpdateVertices()
{
	// Propagate height field to mesh
	CRuVector3 *vertices = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
	for(INT32 z = 0, i = 0, srcIdx = m_heightFieldDimension + 1, destIdx = 0; z <= m_dimension; ++z, srcIdx += 2)
	{
		for(INT32 x = 0; x <= m_dimension; ++x, ++destIdx, ++srcIdx)
		{
			// Copy vertex data
			vertices[destIdx].m_y = m_heightField[srcIdx];
		}
	}

	CRuVector3 *srcVertices = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
	vertices = static_cast<CRuMorphTarget *>(m_outlineMesh->GetMorphTarget(0))->GetPosition();

	INT32 destIdx2 = 0;
	for(INT32 i = 0, srcIdx = 0; i <= m_dimension; ++i, ++srcIdx, ++destIdx2)
	{
		// Copy vertex data
		vertices[destIdx2] = srcVertices[srcIdx];
	}

	for(INT32 i = 0, srcIdx = m_dimension * (m_dimension + 1); i <= m_dimension; ++i, ++srcIdx, ++destIdx2)
	{
		// Copy vertex data
		vertices[destIdx2] = srcVertices[srcIdx];
	}

	for(INT32 i = 0, srcIdx = 0; i <= m_dimension; ++i, srcIdx += (m_dimension + 1), ++destIdx2)
	{
		// Copy vertex data
		vertices[destIdx2] = srcVertices[srcIdx];
	}

	for(INT32 i = 0, srcIdx = m_dimension; i <= m_dimension; ++i, srcIdx += (m_dimension + 1), ++destIdx2)
	{
		// Copy vertex data
		vertices[destIdx2] = srcVertices[srcIdx];
	}
}

void CRuTerrain_Chunk::UpdateSkirtVertices()
{
	CRuVector3 *vertices = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
	float *uv = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);

	INT32 topSkirtVertexArrayOffset = (m_dimension + 1) * (m_dimension + 1);
	INT32 bottomSkirtVertexArrayOffset = topSkirtVertexArrayOffset + (m_dimension + 1);
	INT32 leftSkirtVertexArrayOffset = bottomSkirtVertexArrayOffset + (m_dimension + 1);
	INT32 rightSkirtVertexArrayOffset = leftSkirtVertexArrayOffset + (m_dimension + 1);

	// Duplicate skirt vertices (top row)
	for(INT32 i = 0, destOffset = topSkirtVertexArrayOffset, srcOffset = 0; i <= m_dimension; ++i, ++destOffset, ++srcOffset)
	{
		vertices[destOffset].Set(vertices[srcOffset].x, vertices[srcOffset].y - 20.0f, vertices[srcOffset].z);
	}

	// Duplicate skirt vertices (bottom row)
	for(INT32 i = 0, destOffset = bottomSkirtVertexArrayOffset, srcOffset = m_dimension * (m_dimension + 1); i <= m_dimension; ++i, ++destOffset, ++srcOffset)
	{
		vertices[destOffset].Set(vertices[srcOffset].x, vertices[srcOffset].y - 20.0f, vertices[srcOffset].z);
	}

	// Duplicate skirt vertices (left row)
	for(INT32 i = 0, destOffset = leftSkirtVertexArrayOffset, srcOffset = 0; i <= m_dimension; ++i, ++destOffset, srcOffset += (m_dimension + 1))
	{
		vertices[destOffset].Set(vertices[srcOffset].x, vertices[srcOffset].y - 20.0f, vertices[srcOffset].z);
	}

	// Duplicate skirt vertices (right row)
	for(INT32 i = 0, destOffset = rightSkirtVertexArrayOffset, srcOffset = m_dimension; i <= m_dimension; ++i, ++destOffset, srcOffset += (m_dimension + 1))
	{
		vertices[destOffset].Set(vertices[srcOffset].x, vertices[srcOffset].y - 20.0f, vertices[srcOffset].z);
	}

	// Duplicate skirt uvs (top row)
	for(INT32 i = 0, destOffset = topSkirtVertexArrayOffset * 2, srcOffset = 0; i <= m_dimension * 2; ++i, ++destOffset, ++srcOffset)
	{
		uv[destOffset] = uv[srcOffset];
	}

	// Duplicate skirt uvs (bottom row)
	for(INT32 i = 0, destOffset = bottomSkirtVertexArrayOffset * 2, srcOffset = m_dimension * (m_dimension + 1) * 2; i <= m_dimension; ++i)
	{
		uv[destOffset++] = uv[srcOffset++];
		uv[destOffset++] = uv[srcOffset++];
	}

	// Duplicate skirt uvs (left row)
	for(INT32 i = 0, destOffset = leftSkirtVertexArrayOffset * 2, srcOffset = 0; i <= m_dimension; ++i, srcOffset += (m_dimension + 1) * 2 - 2)
	{
		uv[destOffset++] = uv[srcOffset++];
		uv[destOffset++] = uv[srcOffset++];
	}

	// Duplicate skirt uvs (right row)
	for(INT32 i = 0, destOffset = rightSkirtVertexArrayOffset * 2, srcOffset = m_dimension * 2; i <= m_dimension; ++i, srcOffset += (m_dimension + 1) * 2 - 2)
	{
		uv[destOffset++] = uv[srcOffset++];
		uv[destOffset++] = uv[srcOffset++];
	}
}

void CRuTerrain_Chunk::InitializeSplatIDMap(INT32 splatResolution)
{
	// Store splat resolution and allocate splat ID maps
	m_splatResolution = splatResolution;

	REAL worldSpaceChunkDimension = m_dimension * m_gridSize;

	// Setup ID map's painting bounds
	CRuRectangle splatBounds;

	splatBounds.m_left = 0.0f;
	splatBounds.m_top = 0.0f;
	splatBounds.m_right = splatBounds.m_left + worldSpaceChunkDimension;
	splatBounds.m_bottom = splatBounds.m_top + worldSpaceChunkDimension;

	// Create splat map
	m_splatIDMap = ruNEW CRuSplatIDMap();
	m_splatIDMap->Initialize(m_splatResolution, splatBounds);
}

void CRuTerrain_Chunk::FixSplatIDMapBounds(INT32 splatResolution)
{
	// Store splat resolution and allocate splat ID maps
	m_splatResolution = splatResolution;

	REAL worldSpaceChunkDimension = m_dimension * m_gridSize;

	// Setup ID map's painting bounds
	CRuRectangle splatBounds;

	splatBounds.m_left = 0.0f;
	splatBounds.m_top = 0.0f;
	splatBounds.m_right = splatBounds.m_left + worldSpaceChunkDimension;
	splatBounds.m_bottom = splatBounds.m_top + worldSpaceChunkDimension;

	m_splatIDMap->SetSplatRect(splatBounds);
}

// ************************************************************************************************************************************************************

CRuTerrain_DataSource_Remote::CRuTerrain_DataSource_Remote()
:	m_dataStore(NULL),
	m_commonCollisionStructure(NULL),
	m_chunkDimension(0),
	m_numXChunks(0),
	m_numZChunks(0),
	m_chunkQueue(NULL),
	m_undoActive(FALSE)
{
	// Initialize chunk queue
	m_chunkQueue = ruNEW CRuLRUQueue<DWORD, CRuTerrain_Chunk *>(384, 1024);

	// Initialize stream buffers
	m_streamCompressor = ruNEW CRuStreamCompressor();
	m_compressedChunkStream = ruNEW CRuMemoryStream();
	m_decompressedChunkStream = ruNEW CRuMemoryStream();
}

CRuTerrain_DataSource_Remote::~CRuTerrain_DataSource_Remote()
{
	ruSAFE_RELEASE(m_commonCollisionStructure);

	ruSAFE_RELEASE(m_dataStore);

	// Purge undo stack
	Undo_Flush();

	// Purge chunk LRU queue
	ClearChunkQueue();

	// Purge locked chunks
	while(m_lockedChunks.Count() > 0)
	{
		ruSAFE_RELEASE(m_lockedChunks[0]);
		m_lockedChunks.RemoveAt(0);
	}

	// Purge marked chunks
	while(m_markedChunks.Count() > 0)
	{
		ruSAFE_RELEASE(m_markedChunks[0]);
		m_markedChunks.RemoveAt(0);
	}

	delete m_chunkQueue;

	delete m_streamCompressor;
	delete m_compressedChunkStream;
	delete m_decompressedChunkStream;
}

BOOL CRuTerrain_DataSource_Remote::Undo_Begin()
{
	if(m_undoActive == FALSE)
	{
		// Push a NULL onto the undo stack
		m_undoStack.Push(0xFFFFFFFF);

		// Flag undo mode as active
		m_undoActive = TRUE;

		// Clear the current undo chunk key list
		m_currentUndoList.Clear();

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTerrain_DataSource_Remote::Undo_End()
{
	if(m_undoActive)
	{
		// Push a NULL onto the undo stack
		m_undoStack.Push(0xFFFFFFFF);

		// Flag undo mode as not active
		m_undoActive = FALSE;

		// Trim
		Undo_Trim(4);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTerrain_DataSource_Remote::Undo()
{
	if(m_undoStack.IsEmpty() == FALSE)
	{
		if(m_undoStack.Peek() == 0xFFFFFFFF)
		{
			// Pop undo block marker
			m_undoStack.Pop();

			if(m_undoStack.IsEmpty())
				return FALSE;

			while(m_undoStack.Peek() != 0xFFFFFFFF)
			{
				INT32 x = static_cast<INT32>(m_undoStack.Pop());
				INT32 z = static_cast<INT32>(m_undoStack.Pop());
				CRuTerrain_Chunk *undoChunk = reinterpret_cast<CRuTerrain_Chunk *>(m_undoStack.Pop());

				// Build chunk key from X and Z coordinates
				DWORD chunkKey = (x << 16) | z;
				DWORD chunkKeyHash = RuGlobalHashGen().GetHashCode(chunkKey);

				// Lock the chunk so that it is in memory and ready for editing
				if(LockChunk(x, z))
				{
					// Look for the chunk we just locked
					for(INT32 i = 0; i < m_lockedChunks.Count(); ++i)
					{
						if(m_lockedChunks[i]->GetChunkKey() == chunkKey)
						{
							// Release the old chunk
							ruSAFE_RELEASE(m_lockedChunks[i]);

							// Replace chunk pointer
							m_lockedChunks[i] = undoChunk;
							m_lockedChunks[i]->AddRef();
						}
					}

					// Release the old chunk
					CRuTerrain_Chunk *oldChunk;
					m_chunkQueue->Get(chunkKey, chunkKeyHash, oldChunk);
					ruSAFE_RELEASE(oldChunk);

					// Replace the chunk from the chunk cache
					m_chunkQueue->Set(chunkKey, chunkKeyHash, undoChunk);
				}
			}

			// Pop undo block marker
			m_undoStack.Pop();
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTerrain_DataSource_Remote::Redo()
{
	return TRUE;
}

CRuTerrain_Chunk *CRuTerrain_DataSource_Remote::LockChunk(INT32 x, INT32 z)
{
	// Make sure X and Z are in valid domain
	if(x < 0 || x >= m_numXChunks || z < 0 || z >= m_numZChunks)
		return NULL;

	// Build chunk key from X and Z coordinates
	DWORD chunkKey = (x << 16) | z;

	// Make sure it is not already locked
	for(INT32 i = 0; i < m_lockedChunks.Count(); ++i)
	{
		// If the chunk is already locked, simply return a pointer to it
		if(m_lockedChunks[i]->GetChunkKey() == chunkKey)
		{
			// If undo mode is active, push the chunk onto the undo stack
			if(m_undoActive)
			{
				Undo_PushChunk(x, z, m_lockedChunks[i]);
			}

			return m_lockedChunks[i];
		}
	}

	// Make sure the chunk is loaded into memory
	CRuTerrain_Chunk *terrainChunk = LoadChunk(x, z);

	if(terrainChunk)
	{
		// Put chunk into the locked chunks list
		terrainChunk->AddRef();
		m_lockedChunks.Add(terrainChunk);

		// If undo mode is active, push the chunk onto the undo stack
		if(m_undoActive)
		{
			Undo_PushChunk(x, z, terrainChunk);
		}

		return terrainChunk;
	}

	return NULL;
}

BOOL CRuTerrain_DataSource_Remote::UnlockChunk(INT32 x, INT32 z)
{
	// Make sure X and Z are in valid domain
	if(x < 0 || x >= m_numXChunks || z < 0 || z >= m_numZChunks)
		return NULL;

	// Build chunk key from X and Z coordinates
	DWORD chunkKey = (x << 16) | z;

	for(INT32 i = 0; i < m_lockedChunks.Count(); ++i)
	{
		if(m_lockedChunks[i]->GetChunkKey() == chunkKey)
		{
			// Expel chunk from the locked list
			ruSAFE_RELEASE(m_lockedChunks[i]);
			m_lockedChunks.RemoveAt(i);

			return TRUE;
		}
	}

	return FALSE;
}

CRuTerrain_Chunk *CRuTerrain_DataSource_Remote::MarkChunk(INT32 x, INT32 z)
{
	// Make sure X and Z are in valid domain
	if(x < 0 || x >= m_numXChunks || z < 0 || z >= m_numZChunks)
		return NULL;

	// Build chunk key from X and Z coordinates
	DWORD chunkKey = (x << 16) | z;

	// Make sure it is not already marked
	for(INT32 i = 0; i < m_markedChunks.Count(); ++i)
	{
		// If the chunk is already marked, simply return a pointer to it
		if(m_markedChunks[i]->GetChunkKey() == chunkKey)
		{
			return m_markedChunks[i];
		}
	}

	// Make sure the chunk is loaded into memory
	CRuTerrain_Chunk *terrainChunk = LoadChunk(x, z);

	if(terrainChunk)
	{
		// Put chunk into the marked chunks list
		terrainChunk->AddRef();
		m_markedChunks.Add(terrainChunk);

		return terrainChunk;
	}

	return NULL;
}

BOOL CRuTerrain_DataSource_Remote::UnmarkChunk(INT32 x, INT32 z)
{
	// Make sure X and Z are in valid domain
	if(x < 0 || x >= m_numXChunks || z < 0 || z >= m_numZChunks)
		return NULL;

	// Build chunk key from X and Z coordinates
	DWORD chunkKey = (x << 16) | z;

	for(INT32 i = 0; i < m_markedChunks.Count(); ++i)
	{
		if(m_markedChunks[i]->GetChunkKey() == chunkKey)
		{
			// Expel chunk from the marked list
			ruSAFE_RELEASE(m_markedChunks[i]);
			m_markedChunks.RemoveAt(i);

			return TRUE;
		}
	}

	return FALSE;
}

CRuTerrain_Chunk *CRuTerrain_DataSource_Remote::LoadChunk(INT32 x, INT32 z)
{
	// Make sure X and Z are in valid domain
	if(x < 0 || x >= m_numXChunks || z < 0 || z >= m_numZChunks)
		return NULL;

	// Build chunk key and its hash
	DWORD chunkKey = (x << 16) | z;
	DWORD chunkKeyHash = RuGlobalHashGen().GetHashCode(chunkKey);
	CRuTerrain_Chunk *existingChunk = NULL;

	// Attempt to locate chunk in the chunk queue
	if(m_chunkQueue->Get(chunkKey, chunkKeyHash, existingChunk))
	{
		return existingChunk;
	}

	// Attempt to locate chunk in the locked chunk queue
	for(INT32 i = 0; i < m_lockedChunks.Count(); ++i)
	{
		if(m_lockedChunks[i]->GetChunkKey() == chunkKey)
		{
			return m_lockedChunks[i];
		}
	}

	// Attempt to locate chunk in the marked chunk queue
	for(INT32 i = 0; i < m_markedChunks.Count(); ++i)
	{
		if(m_markedChunks[i]->GetChunkKey() == chunkKey)
		{
			return m_markedChunks[i];
		}
	}

	// Retrieve the chunk
	IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_Chunk, reinterpret_cast<BYTE *>(&chunkKey));

	if(stream)
	{
		// Decompress chunk data block
		m_streamCompressor->Decompress(stream, 0, stream->GetStreamSize(), m_decompressedChunkStream);
		delete stream;

		// Construct the chunk from the decompressed stream
		IRuObject *terrainObject = RuObjectFactory().CreateObject(m_decompressedChunkStream);

		if(terrainObject)
		{
			if(terrainObject->GetType() == CRuTerrain_Chunk::Type())
			{
				// Create the common collision structure if it does not already exist
				if(m_commonCollisionStructure == NULL)
				{
					CRuAABB gridBounds = static_cast<CRuTerrain_Chunk *>(terrainObject)->GetBoundingBox();
					gridBounds.Maxima() = gridBounds.Maxima() - gridBounds.Minima();
					gridBounds.Minima().Set(0.0f, 0.0f, 0.0f);
					gridBounds.Minima().m_y = -ruMAXFLOATRANGE;
					gridBounds.Maxima().m_y = ruMAXFLOATRANGE;

					m_commonCollisionStructure = ruNEW CRuCollisionStructure_Quadtree();
					m_commonCollisionStructure->BuildCollisionStructureForRegularGrid(gridBounds, this->GetChunkDimension(), this->GetChunkDimension());
				}

				// Attach common collision structure to the chunk
				static_cast<CRuTerrain_Chunk *>(terrainObject)->AttachCollisionStructure(m_commonCollisionStructure);

				// Try to free up space in the LRU queue for the new chunk, if necessary
				CRuTerrain_Chunk *reclaimedChunk;
				while(m_chunkQueue->Reclaim(chunkKey, chunkKeyHash, 1, reclaimedChunk))
				{
					// Release the chunk
					ruSAFE_RELEASE(reclaimedChunk);
				}

				// Set the chunk key for the chunk
				static_cast<CRuTerrain_Chunk *>(terrainObject)->SetChunkKey(chunkKey);

				// Put chunk into the chunk queue
				m_chunkQueue->Add(chunkKey, chunkKeyHash, static_cast<CRuTerrain_Chunk *>(terrainObject), 1);

				// Return the chunk
				return static_cast<CRuTerrain_Chunk *>(terrainObject);
			}
			else
			{
				// The object is not a terrain chunk, release and abort
				terrainObject->Release();
			}
		}
	}

	return NULL;
}

CRuTerrain_Chunk *CRuTerrain_DataSource_Remote::LoadMinimalChunk(INT32 x, INT32 z)
{
	// Make sure X and Z are in valid domain
	if(x < 0 || x >= m_numXChunks || z < 0 || z >= m_numZChunks)
		return NULL;

	// Build chunk key and its hash
	DWORD chunkKey = (x << 16) | z;
	DWORD chunkKeyHash = RuGlobalHashGen().GetHashCode(chunkKey);

	// Retrieve the chunk
	IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_Chunk, reinterpret_cast<BYTE *>(&chunkKey));

	if(stream)
	{
		// Decompress chunk data block
		m_streamCompressor->Decompress(stream, 0, stream->GetStreamSize(), m_decompressedChunkStream);
		delete stream;

		// Construct the chunk from the decompressed stream
		CRuTerrain_Chunk *chunk = ruNEW CRuTerrain_Chunk();
		chunk->SerializeFrom(m_decompressedChunkStream, ruTERRAINCHUNKLOADMODE_MINIMAL);

		return chunk;
	}

	return NULL;
}

BOOL CRuTerrain_DataSource_Remote::SaveChunk(INT32 x, INT32 z, CRuTerrain_Chunk *chunk)
{
	// Make sure X and Z are in valid domain
	if(x < 0 || x >= m_numXChunks || z < 0 || z >= m_numZChunks)
		return FALSE;

	// Build chunk key from X and Z coordinates
	DWORD chunkKey = (x << 16) | z;

	// Serialize chunk to memory stream
	m_decompressedChunkStream->SetStreamSize(0);
	chunk->SerializeTo(m_decompressedChunkStream);

	// Compress memory stream
	m_compressedChunkStream->SetStreamSize(0);
	m_streamCompressor->Compress(m_decompressedChunkStream, m_compressedChunkStream);

	// Store
	m_dataStore->Store(m_tableHandle_Chunk, reinterpret_cast<BYTE *>(&chunkKey), m_compressedChunkStream);
/*
	// Increment iteration number for the chunk
	INT32 chunkIndex = x + z * m_numXChunks;
	++m_chunkHeaders[chunkIndex].m_iterationNumber;

	// Write the chunk header
	m_decompressedChunkStream->SetStreamSize(0);
	m_decompressedChunkStream->Write(&m_chunkHeaders[chunkIndex].m_iterationNumber, sizeof(INT32));
	m_decompressedChunkStream->Write(&m_chunkHeaders[chunkIndex].m_bounds.Minima().x, sizeof(float));
	m_decompressedChunkStream->Write(&m_chunkHeaders[chunkIndex].m_bounds.Minima().y, sizeof(float));
	m_decompressedChunkStream->Write(&m_chunkHeaders[chunkIndex].m_bounds.Minima().z, sizeof(float));
	m_decompressedChunkStream->Write(&m_chunkHeaders[chunkIndex].m_bounds.Maxima().x, sizeof(float));
	m_decompressedChunkStream->Write(&m_chunkHeaders[chunkIndex].m_bounds.Maxima().y, sizeof(float));
	m_decompressedChunkStream->Write(&m_chunkHeaders[chunkIndex].m_bounds.Maxima().z, sizeof(float));

	m_dataStore->Store(m_tableHandle_ChunkHeader, reinterpret_cast<BYTE *>(&chunkKey), m_decompressedChunkStream);
*/
	return TRUE;
}

void CRuTerrain_DataSource_Remote::SetEditingEnvironmentColors(DWORD primaryColor, DWORD secondaryColor, DWORD specularColor, DWORD fogColor, float theta, float phi)
{
	m_editSetting_PrimaryColor = primaryColor;
	m_editSetting_SecondaryColor = secondaryColor;
	m_editSetting_SpecularColor = specularColor;
	m_editSetting_fogColor = fogColor;
	m_editSetting_Theta = theta;
	m_editSetting_Phi = phi;
}

void CRuTerrain_DataSource_Remote::GetEditingEnvironmentColors(DWORD &primaryColor, DWORD &secondaryColor, DWORD &specularColor, DWORD &fogColor, float &theta, float &phi)
{
	primaryColor = m_editSetting_PrimaryColor;
	secondaryColor = m_editSetting_SecondaryColor;
	specularColor = m_editSetting_SpecularColor;
	fogColor = m_editSetting_fogColor;
	theta = m_editSetting_Theta;
	phi = m_editSetting_Phi;
}

BOOL CRuTerrain_DataSource_Remote::LockTextureList()
{
	return TRUE;
}

BOOL CRuTerrain_DataSource_Remote::UnlockTextureList()
{
	return TRUE;
}

BOOL CRuTerrain_DataSource_Remote::LoadTextureList(CRuTextureList *textureList)
{
	if(textureList)
	{
		return textureList->Load(m_dataStore);
	}

	return FALSE;
}

BOOL CRuTerrain_DataSource_Remote::SaveTextureList(CRuTextureList *textureList, BOOL forceSave)
{
	if(textureList)
	{
		return textureList->Save(m_dataStore, FALSE);
	}

	return FALSE;
}

INT32 CRuTerrain_DataSource_Remote::GetNumWaterBodyDescriptors()
{
	return m_dataStore->GetNumRecords(m_tableHandle_WaterDescriptor);
}

CRuWorld_WaterBodyDescriptor *CRuTerrain_DataSource_Remote::LoadWaterBodyDescriptorByIndex(INT32 index)
{
	CRuWorld_WaterBodyDescriptor *waterBodyDescriptor = NULL;
	IRuStream *stream = m_dataStore->RetrieveByIndex(m_tableHandle_WaterDescriptor, index);

	if(stream)
	{
		waterBodyDescriptor = ruNEW CRuWorld_WaterBodyDescriptor();

		if(waterBodyDescriptor->SerializeFrom(stream) == FALSE)
		{
			ruSAFE_DELETE(waterBodyDescriptor);
		}

		ruSAFE_DELETE(stream);
	}

	return waterBodyDescriptor;
}

BOOL CRuTerrain_DataSource_Remote::SaveWaterBodyDescriptor(CRuWorld_WaterBodyDescriptor *waterBodyDescriptor)
{
	// Serialize water body descriptor
	m_decompressedChunkStream->SetStreamSize(0);
	waterBodyDescriptor->SerializeTo(m_decompressedChunkStream);

	// Store
	return m_dataStore->Store(m_tableHandle_WaterDescriptor, waterBodyDescriptor->m_guid.Data(), m_decompressedChunkStream);;
}

BOOL CRuTerrain_DataSource_Remote::DeleteWaterBodyDescriptorByIndex(const CRuGUID &waterBodyGUID)
{
	return m_dataStore->Delete(m_tableHandle_WaterDescriptor, waterBodyGUID.Data());
}

BOOL CRuTerrain_DataSource_Remote::LoadHeader()
{
	INT32 dummyKey = 0;
	IRuStream *stream = m_dataStore->Retrieve(m_tableHandle_Header, reinterpret_cast<BYTE *>(&dummyKey));

	if(stream)
	{
		// Read four UINT32 values from the stream to determine class GUID
		UINT32 guidValues[4];

		for(INT32 i = 0; i < 4; ++i)
		{
			if(stream->Read(&guidValues[i], sizeof(UINT32)) != sizeof(UINT32))
			{
				delete stream;
				return FALSE;
			}
		}

		// Construct class GUID
		CRuGUID classGUID(guidValues[0], guidValues[1], guidValues[2], guidValues[3]);

		if(classGUID != IRuTerrain_DataSource::ClassGUID())
		{
			delete stream;
			return FALSE;
		}

		// Read header information
		stream->Read(&m_version, sizeof(INT32));

		switch(m_version)
		{
			case 1:
			case 2:
				{
					stream->Read(&m_chunkDimension, sizeof(INT32));
					stream->Read(&m_numXChunks, sizeof(INT32));
					stream->Read(&m_numZChunks, sizeof(INT32));
					stream->Read(&m_splatResolution, sizeof(INT32));

					m_gridSize = 7.5f;
					m_lodLevels = 3;

					// Read lighting info for editing
					stream->Read(&m_editSetting_PrimaryColor, sizeof(INT32));
					stream->Read(&m_editSetting_SecondaryColor, sizeof(INT32));
					stream->Read(&m_editSetting_SpecularColor, sizeof(INT32));
					stream->Read(&m_editSetting_fogColor, sizeof(INT32));
					stream->Read(&m_editSetting_Theta, sizeof(float));
					stream->Read(&m_editSetting_Phi, sizeof(float));
				}

				break;

			case 3:
				{
					stream->Read(&m_origin.m_x, sizeof(REAL));
					stream->Read(&m_origin.m_y, sizeof(REAL));
					stream->Read(&m_origin.m_z, sizeof(REAL));
					stream->Read(&m_chunkDimension, sizeof(INT32));
					stream->Read(&m_numXChunks, sizeof(INT32));
					stream->Read(&m_numZChunks, sizeof(INT32));
					stream->Read(&m_splatResolution, sizeof(INT32));
					stream->Read(&m_gridSize, sizeof(REAL));
					stream->Read(&m_lodLevels, sizeof(INT32));

					// Read lighting info for editing
					stream->Read(&m_editSetting_PrimaryColor, sizeof(INT32));
					stream->Read(&m_editSetting_SecondaryColor, sizeof(INT32));
					stream->Read(&m_editSetting_SpecularColor, sizeof(INT32));
					stream->Read(&m_editSetting_fogColor, sizeof(INT32));
					stream->Read(&m_editSetting_Theta, sizeof(float));
					stream->Read(&m_editSetting_Phi, sizeof(float));
				}

				break;
		}

		delete stream;

/*
		// Read bounding box information for chunks
		m_chunkHeaders = ruNEW ChunkHeader [m_numXChunks * m_numZChunks];
		for(INT32 z = 0, chunkIndex = 0; z < m_numZChunks; ++z)
		{
			for(INT32 x = 0; x < m_numXChunks; ++x, ++chunkIndex)
			{
				// Retrieve chunk header
				DWORD chunkKey = (x << 16) | z;
				IRuStream *chunkStream = m_dataStore->Retrieve(m_tableHandle_ChunkHeader, reinterpret_cast<BYTE *>(&chunkKey));

				if(chunkStream)
				{
					chunkStream->Read(&m_chunkHeaders[chunkIndex].m_iterationNumber, sizeof(INT32));
					chunkStream->Read(&m_chunkHeaders[chunkIndex].m_bounds.Minima().x, sizeof(float));
					chunkStream->Read(&m_chunkHeaders[chunkIndex].m_bounds.Minima().y, sizeof(float));
					chunkStream->Read(&m_chunkHeaders[chunkIndex].m_bounds.Minima().z, sizeof(float));
					chunkStream->Read(&m_chunkHeaders[chunkIndex].m_bounds.Maxima().x, sizeof(float));
					chunkStream->Read(&m_chunkHeaders[chunkIndex].m_bounds.Maxima().y, sizeof(float));
					chunkStream->Read(&m_chunkHeaders[chunkIndex].m_bounds.Maxima().z, sizeof(float));

					delete chunkStream;
				}
			}
		}

		// Build the maximal bounding box
		m_terrainBounds = m_chunkHeaders[0].m_bounds;
		for(INT32 i = 1, numChunks = m_numXChunks * m_numZChunks; i < numChunks; ++i)
		{
			m_terrainBounds.Merge(m_chunkHeaders[i].m_bounds);
		}
*/
		// HACK
		if(m_version < 3)
		{
			CRuTerrain_Chunk *chunk = this->LoadChunk(0, 0);

			if(chunk)
			{
				CRuAABB bounds = chunk->GetBoundingBox();
				m_origin.Set(bounds.Minima().x, 0.0f, bounds.Minima().z);
			}
		}

		// Build the maximal bounding box
		REAL verticalLimit = 20000.0f;
		REAL chunkWorldSpaceSize = m_chunkDimension * m_gridSize;

		m_terrainBounds.Minima().Set(m_origin.m_x, -verticalLimit, m_origin.m_z);
		m_terrainBounds.Maxima().Set(m_origin.m_x + m_chunkDimension * m_gridSize * m_numXChunks, verticalLimit, m_origin.m_z + m_chunkDimension * m_gridSize * m_numZChunks);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTerrain_DataSource_Remote::SaveHeader(BOOL saveChunkHeaders, BOOL saveEditingEnvironmentOnly)
{
	// Set header version
	m_version = 3;

	// Prepare stream for writing
	m_decompressedChunkStream->SetStreamSize(0);

	// Write class GUID
	m_decompressedChunkStream->Write(IRuTerrain_DataSource::ClassGUID().Data(), IRuTerrain_DataSource::ClassGUID().Size());

	// Write header information
	m_decompressedChunkStream->Write(&m_version, sizeof(INT32));
	m_decompressedChunkStream->Write(&m_origin.m_x, sizeof(REAL));
	m_decompressedChunkStream->Write(&m_origin.m_y, sizeof(REAL));
	m_decompressedChunkStream->Write(&m_origin.m_z, sizeof(REAL));
	m_decompressedChunkStream->Write(&m_chunkDimension, sizeof(INT32));
	m_decompressedChunkStream->Write(&m_numXChunks, sizeof(INT32));
	m_decompressedChunkStream->Write(&m_numZChunks, sizeof(INT32));
	m_decompressedChunkStream->Write(&m_splatResolution, sizeof(INT32));
	m_decompressedChunkStream->Write(&m_gridSize, sizeof(REAL));
	m_decompressedChunkStream->Write(&m_lodLevels, sizeof(INT32));

	// Write lighting info for editing
	m_decompressedChunkStream->Write(&m_editSetting_PrimaryColor, sizeof(INT32));
	m_decompressedChunkStream->Write(&m_editSetting_SecondaryColor, sizeof(INT32));
	m_decompressedChunkStream->Write(&m_editSetting_SpecularColor, sizeof(INT32));
	m_decompressedChunkStream->Write(&m_editSetting_fogColor, sizeof(INT32));
	m_decompressedChunkStream->Write(&m_editSetting_Theta, sizeof(float));
	m_decompressedChunkStream->Write(&m_editSetting_Phi, sizeof(float));

	// Store header
	INT32 dummyKey = 0;
	m_dataStore->Store(m_tableHandle_Header, reinterpret_cast<BYTE *>(&dummyKey), m_decompressedChunkStream);
/*
	if(saveChunkHeaders)
	{
		// Write real bounding box information
		for(INT32 z = 0; z < m_numZChunks; ++z)
		{
			for(INT32 x = 0; x < m_numXChunks; ++x)
			{
				INT32 offset = x + z * m_numXChunks;
				m_decompressedChunkStream->SetStreamSize(0);
				m_decompressedChunkStream->Write(&m_chunkHeaders[offset].m_iterationNumber, sizeof(INT32));
				m_decompressedChunkStream->Write(&m_chunkHeaders[offset].m_bounds.Minima().x, sizeof(float));
				m_decompressedChunkStream->Write(&m_chunkHeaders[offset].m_bounds.Minima().y, sizeof(float));
				m_decompressedChunkStream->Write(&m_chunkHeaders[offset].m_bounds.Minima().z, sizeof(float));
				m_decompressedChunkStream->Write(&m_chunkHeaders[offset].m_bounds.Maxima().x, sizeof(float));
				m_decompressedChunkStream->Write(&m_chunkHeaders[offset].m_bounds.Maxima().y, sizeof(float));
				m_decompressedChunkStream->Write(&m_chunkHeaders[offset].m_bounds.Maxima().z, sizeof(float));

				DWORD chunkKey = (x << 16) | z;
				m_dataStore->Store(m_tableHandle_ChunkHeader, reinterpret_cast<BYTE *>(&chunkKey), m_decompressedChunkStream);
			}
		}
	}
*/
	return TRUE;
}

BOOL CRuTerrain_DataSource_Remote::FlushChunkQueue()
{
	ClearChunkQueue();

	return TRUE;
}

BOOL CRuTerrain_DataSource_Remote::FlushLockedChunks()
{
	while(m_lockedChunks.Count() > 0)
	{
		// Decode chunk X, Z
		DWORD chunkKey = m_lockedChunks[0]->GetChunkKey();
		INT32 x = (chunkKey >> 16) & 0x0000FFFF;
		INT32 z = chunkKey & 0x0000FFFF;

		// Stream chunk to disk
		SaveChunk(x, z, m_lockedChunks[0]);

		// Unlock chunk
		UnlockChunk(x, z);
	}

	return TRUE;
}

BOOL CRuTerrain_DataSource_Remote::FlushEditingEnvironment()
{
	return SaveHeader(FALSE, TRUE);
}

const CRuVector3 &CRuTerrain_DataSource_Remote::GetOrigin() const
{
	return m_origin;
}

INT32 CRuTerrain_DataSource_Remote::GetChunkDimension() const
{
	return m_chunkDimension;
}

INT32 CRuTerrain_DataSource_Remote::GetNumXChunks() const
{
	return m_numXChunks;
}

INT32 CRuTerrain_DataSource_Remote::GetNumZChunks() const
{
	return m_numZChunks;
}

INT32 CRuTerrain_DataSource_Remote::GetSplatResolution() const
{
	return m_splatResolution;
}

INT32 CRuTerrain_DataSource_Remote::SetSplatResolution(INT32 splatResolution)
{
	m_splatResolution = splatResolution;
	return m_splatResolution;
}

REAL CRuTerrain_DataSource_Remote::GetGridSize() const
{
	return m_gridSize;
}

INT32 CRuTerrain_DataSource_Remote::GetLODLevels() const
{
	return m_lodLevels;
}

const CRuAABB &CRuTerrain_DataSource_Remote::GetTerrainBounds() const
{
	return m_terrainBounds;
}

CRuAABB CRuTerrain_DataSource_Remote::GetChunkBounds(INT32 x, INT32 z) const
{
	CRuAABB chunkBounds;
	REAL verticalLimit = 20000.0f;
	REAL chunkWorldSpaceSize = m_chunkDimension * m_gridSize;

	// Calculate chunk bounds based on integer (X, Z) coordinates
	chunkBounds.Minima().Set(x * chunkWorldSpaceSize + m_origin.m_x, -verticalLimit, z * chunkWorldSpaceSize + m_origin.m_z);
	chunkBounds.Maxima().Set(chunkBounds.Minima().m_x + chunkWorldSpaceSize, verticalLimit, chunkBounds.Minima().m_z + chunkWorldSpaceSize);

	return chunkBounds;
}

INT32 CRuTerrain_DataSource_Remote::GetChunkIterationNumber(INT32 x, INT32 z)
{
	CRuTerrain_Chunk *chunk = this->LoadMinimalChunk(x, z);

	INT32 iteration = chunk->GetChunkIteration();
	ruSAFE_RELEASE(chunk);

	return  iteration;
}

BOOL CRuTerrain_DataSource_Remote::ChunkIsMarked(CRuTerrain_Chunk *chunk) const
{
	for(INT32 i = 0; i < m_markedChunks.Count(); ++i)
	{
		if(m_markedChunks[i]->GetChunkKey() == chunk->GetChunkKey())
		{
			return TRUE;
		}
	}

	return FALSE;
}

INT32 CRuTerrain_DataSource_Remote::GetNumMarkedChunks() const
{
	return m_markedChunks.Count();
}

CRuTerrain_Chunk *CRuTerrain_DataSource_Remote::GetMarkedChunkByIndex(INT32 index)
{
	return m_markedChunks[index];
}

BOOL CRuTerrain_DataSource_Remote::Initialize(const char *serverName, INT32 port, const char *filename, const CRuVector3 &origin, float gridSize, INT32 chunkDimension, INT32 numXChunks, INT32 numZChunks, INT32 lodLevels, INT32 splatResolution)
{
	// Connect
	CRuDataClient *dataClient = ruNEW CRuDataClient();

	if(dataClient->Connect(serverName, port) == FALSE)
	{
		return FALSE;
	}

	// Create data store
	if(dataClient->CreateDataStore(filename) == FALSE)
	{
		return FALSE;
	}

	BOOL retVal = Initialize(dataClient, origin, gridSize, chunkDimension, numXChunks, numZChunks, lodLevels, splatResolution);

	ruSAFE_RELEASE(dataClient);

	return retVal;
}

BOOL CRuTerrain_DataSource_Remote::Initialize(CRuDataStore_Base *dataStore, const CRuVector3 &origin, float gridSize, INT32 chunkDimension, INT32 numXChunks, INT32 numZChunks, INT32 lodLevels, INT32 splatResolution)
{
	ruSAFE_RELEASE(m_dataStore);
	m_dataStore = dataStore;
	m_dataStore->AddRef();

	// Initialize terrain information
	m_origin = origin;
	m_chunkDimension = chunkDimension;
	m_numXChunks = numXChunks;
	m_numZChunks = numZChunks;
	m_splatResolution = splatResolution;
	m_gridSize = gridSize;
	m_lodLevels = lodLevels;

	// Create tables
	m_tableHandle_Header = m_dataStore->CreateTable("header", 4);
	m_tableHandle_Chunk = m_dataStore->CreateTable("chunks", 4);
	m_tableHandle_TextureList = m_dataStore->CreateTable("texture_list", 4);
	m_tableHandle_WaterDescriptor = m_dataStore->CreateTable("water_descriptor", 16);

	// Save header
	SaveHeader();

	// Write initial chunk information for each chunk
	for(INT32 z = 0, chunkIndex = 0; z < m_numZChunks; ++z)
	{
		for(INT32 x = 0; x < m_numXChunks; ++x, ++chunkIndex)
		{
			// Construct chunk at (X, Z)
			CRuTerrain_Chunk *terrainChunk = ruNEW CRuTerrain_Chunk();
			terrainChunk->Allocate(chunkDimension, gridSize, CRuVector3(x * m_chunkDimension * gridSize + origin.x, origin.y, z * m_chunkDimension * gridSize + origin.z), lodLevels, splatResolution);

			// Store chunk
			SaveChunk(x, z, terrainChunk);

			terrainChunk->Release();
		}
	}

	// Reload chunk headers
	LoadHeader();

	return TRUE;
}

BOOL CRuTerrain_DataSource_Remote::Open(const char *serverName, INT32 port, const char *filename)
{
	BOOL retVal = FALSE;

	// Connect
	CRuDataClient *dataClient = ruNEW CRuDataClient();

	if(dataClient->Connect(serverName, port) == FALSE)
	{
		return FALSE;
	}

	// Open data store
	if(dataClient->OpenDataStore(filename) == FALSE)
	{
		return FALSE;
	}

	ruSAFE_RELEASE(m_dataStore);
	m_dataStore = dataClient;

	// Get table handles
	m_tableHandle_Header = m_dataStore->GetTableHandle("header");
	m_tableHandle_Chunk = m_dataStore->GetTableHandle("chunks");
	m_tableHandle_TextureList = m_dataStore->GetTableHandle("texture_list");
	m_tableHandle_WaterDescriptor = m_dataStore->GetTableHandle("water_descriptor");

	// Create data tables if they do not exist
	if(m_tableHandle_WaterDescriptor < 0)
	{
		m_tableHandle_WaterDescriptor = m_dataStore->CreateTable("water_descriptor", 16);
	}

	// Load header information
	retVal = LoadHeader();

	return retVal;
}

BOOL CRuTerrain_DataSource_Remote::SaveToOptimizedTerrain(CRuDataStore_Base *dataStore)
{
	// copy water table
	INT32 srcTableList[1];
	srcTableList[0] = m_tableHandle_WaterDescriptor;

	// Create tables
	INT32 targetTableList[1];
	targetTableList[0] = dataStore->CreateTable("water_descriptor", 16);

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
				dataStore->Store(targetTableList[i], key, stream);
				delete stream;
			}
		}

		delete[] key;
	}

	return TRUE;
}

CRuDataStore_Base *CRuTerrain_DataSource_Remote::GetDataStore()
{
	return m_dataStore;
}

BOOL CRuTerrain_DataSource_Remote::RestreamChunks()
{
	// Re-stream all chunks
	for(INT32 z = 0, chunkIndex = 0; z < m_numZChunks; ++z)
	{
		for(INT32 x = 0; x < m_numXChunks; ++x, ++chunkIndex)
		{
			// Construct chunk at (X, Z)
			CRuTerrain_Chunk *terrainChunk = this->LoadChunk(x, z);

			if(terrainChunk)
			{
				// Store chunk
				SaveChunk(x, z, terrainChunk);
			}
		}
	}

	return TRUE;
}

BOOL CRuTerrain_DataSource_Remote::CloneFromDS(const char *serverName, INT32 port, const char *filename, IRuTerrain_DataSource *dataSource)
{
	// Initialize terrain information
	m_chunkDimension = dataSource->GetChunkDimension();
	m_numXChunks = dataSource->GetNumXChunks();
	m_numZChunks = dataSource->GetNumZChunks();
	m_splatResolution = dataSource->GetSplatResolution();

	// Connect
	CRuDataClient *dataClient = ruNEW CRuDataClient();

	if(dataClient->Connect(serverName, port) == FALSE)
	{
		return FALSE;
	}

	// Create data store
	if(dataClient->CreateDataStore(filename) == FALSE)
	{
		return FALSE;
	}

	ruSAFE_RELEASE(m_dataStore);
	m_dataStore = dataClient;

	// Create tables
	m_tableHandle_Header = m_dataStore->CreateTable("header", 4);
	m_tableHandle_Chunk = m_dataStore->CreateTable("chunks", 4);
	m_tableHandle_TextureList = m_dataStore->CreateTable("texture_list", 4);
	m_tableHandle_WaterDescriptor = m_dataStore->CreateTable("water_descriptor", 16);

	// Save header
	SaveHeader();

	// Copy chunks from data source
	for(INT32 z = 0, chunkIndex = 0; z < m_numZChunks; ++z)
	{
		for(INT32 x = 0; x < m_numXChunks; ++x, ++chunkIndex)
		{
			// Construct chunk at (X, Z)
			CRuTerrain_Chunk *terrainChunk = dataSource->LoadChunk(x, z);

			if(terrainChunk)
			{
				// Store chunk
				SaveChunk(x, z, terrainChunk);
			}
		}
	}

	// Copy texture list from data source
	CRuTextureList *textureList = ruNEW CRuTextureList();

	// Prepend 2048 NULL textures into the texture list
	for(INT32 i = 0; i < 2048; ++i)
	{
		textureList->AddTexture(NULL);
	}

	dataSource->LoadTextureList(textureList);

	textureList->SetTextureDirty();
	SaveTextureList(textureList);

	return TRUE;
}

BOOL CRuTerrain_DataSource_Remote::ExpandFromDS(const char *serverName, INT32 port, const char *filename, INT32 xSize, INT32 zSize, IRuTerrain_DataSource *dataSource)
{
	INT32 srcNumXChunks = dataSource->GetNumXChunks();
	INT32 srcNumZChunks = dataSource->GetNumZChunks();

	// Initialize terrain information
	m_chunkDimension = dataSource->GetChunkDimension();
	m_numXChunks = dataSource->GetNumXChunks() + xSize * 2;
	m_numZChunks = dataSource->GetNumZChunks() + zSize * 2;
	m_splatResolution = dataSource->GetSplatResolution();
	m_gridSize = dataSource->GetGridSize();
	m_lodLevels = dataSource->GetLODLevels();

	m_origin.Set(dataSource->GetOrigin().m_x - xSize * m_chunkDimension * m_gridSize, dataSource->GetOrigin().m_y, dataSource->GetOrigin().m_z - zSize * m_chunkDimension * m_gridSize);

	// Connect
	CRuDataClient *dataClient = ruNEW CRuDataClient();

	if(dataClient->Connect(serverName, port) == FALSE)
	{
		return FALSE;
	}

	// Create data store
	if(dataClient->CreateDataStore(filename) == FALSE)
	{
		return FALSE;
	}

	ruSAFE_RELEASE(m_dataStore);
	m_dataStore = dataClient;

	// Create tables
	m_tableHandle_Header = m_dataStore->CreateTable("header", 4);
	m_tableHandle_Chunk = m_dataStore->CreateTable("chunks", 4);
	m_tableHandle_TextureList = m_dataStore->CreateTable("texture_list", 4);
	m_tableHandle_WaterDescriptor = m_dataStore->CreateTable("water_descriptor", 16);

	// Save header
	SaveHeader();

	// Write initial chunk information for each chunk
	CRuTerrain_Chunk *sampleChunk = dataSource->LoadChunk(0, 0);
	REAL gridSize = sampleChunk->GetGridSize();
	INT32 lodLevels = sampleChunk->GetLODLevels();

	CRuVector3 origin = dataSource->GetTerrainBounds().Minima() - CRuVector3(xSize * m_chunkDimension * gridSize, 0.0f, zSize * m_chunkDimension * gridSize);
	origin.m_y = 0.0f;

	for(INT32 z = 0, chunkIndex = 0; z < m_numZChunks; ++z)
	{
		for(INT32 x = 0; x < m_numXChunks; ++x, ++chunkIndex)
		{
			// Construct chunk at (X, Z)
			CRuTerrain_Chunk *terrainChunk = ruNEW CRuTerrain_Chunk();
			terrainChunk->Allocate(m_chunkDimension, gridSize, CRuVector3(x * m_chunkDimension * gridSize + origin.x, origin.y, z * m_chunkDimension * gridSize + origin.z), lodLevels, m_splatResolution);

			// Store chunk
			SaveChunk(x, z, terrainChunk);

			// Free chunk
			ruSAFE_RELEASE(terrainChunk);
		}
	}

	// Copy chunks from data source
	for(INT32 z = 0, chunkIndex = 0; z < srcNumZChunks; ++z)
	{
		for(INT32 x = 0; x < srcNumXChunks; ++x, ++chunkIndex)
		{
			// Construct chunk at (X, Z)
			CRuTerrain_Chunk *terrainChunk = dataSource->LoadChunk(x, z);

			if(terrainChunk)
			{
				// Store chunk
				SaveChunk(x + xSize, z + zSize, terrainChunk);
			}
		}
	}

	// Copy texture list from data source
	CRuTextureList *textureList = ruNEW CRuTextureList();

	// Prepend 2048 NULL textures into the texture list
	for(INT32 i = 0; i < 2048; ++i)
	{
		textureList->AddTexture(NULL);
	}

	dataSource->LoadTextureList(textureList);

	textureList->SetTextureDirty();
	SaveTextureList(textureList);

	return TRUE;
}

BOOL CRuTerrain_DataSource_Remote::CopyTo(CRuDataStore_Base *dstDataStore)
{
	if(m_dataStore)
	{
		INT32 numTables = m_dataStore->GetNumTables();

		for(INT32 i = 0; i < numTables; ++i)
		{
			const char *tableName = m_dataStore->GetTableNameByIndex(i);
			INT32 tableKeyLength = m_dataStore->GetTableKeyLengthByIndex(i);

			INT32 srcTableHandle = m_dataStore->GetTableHandle(tableName);
			INT32 dstTableHandle = dstDataStore->CreateTable(tableName, tableKeyLength);

			INT32 numEntries = m_dataStore->GetNumRecords(srcTableHandle);
			BYTE *key = ruNEW BYTE [tableKeyLength];

			for(INT32 j = 0; j < numEntries; ++j)
			{
				// Retrieve key and data
				IRuStream *stream = m_dataStore->RetrieveByIndex(srcTableHandle, j);

				if(stream)
				{
					m_dataStore->RetrieveKeyByIndex(srcTableHandle, j, key);
					dstDataStore->Store(dstTableHandle, key, stream);
					delete stream;
				}
			}

			delete[] key;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTerrain_DataSource_Remote::Undo_PushChunk(INT32 x, INT32 z, CRuTerrain_Chunk *chunk)
{
	if(m_undoActive)
	{
		// Build chunk key from X and Z coordinates
		DWORD chunkKey = (x << 16) | z;

		// Check list of chunks already in undo stack
		for(INT32 i = 0; i < m_currentUndoList.Count(); ++i)
		{
			if(m_currentUndoList[i] == chunkKey)
				return TRUE;
		}

		// Make a clone of the chunk
		CRuTerrain_Chunk *clonedChunk = static_cast<CRuTerrain_Chunk *>(chunk->Clone());

		// Push chunk onto stack
		m_undoStack.Push(reinterpret_cast<INT64>(clonedChunk));

		// Push z onto stack
		m_undoStack.Push(static_cast<INT64>(z));

		// Push x onto stack
		m_undoStack.Push(static_cast<INT64>(x));

		m_currentUndoList.Add(chunkKey);

		return TRUE;
	}

	return FALSE;
}

/*!
	Flushes the entire undo stack. This operation will not succeed if the undo stack
	is not in a valid state. That is, if this call is made between a Undo_Begin and
	Undo_End pair, the call will fail.

	@retval TRUE The operation succeeded.
	@retval FALSE The operation failed.
*/
BOOL CRuTerrain_DataSource_Remote::Undo_Flush()
{
	if(!m_undoActive)
	{
		while(m_undoStack.IsEmpty() == FALSE)
		{
			if(m_undoStack.Peek() == 0xFFFFFFFF)
			{
				// Pop undo block marker
				m_undoStack.Pop();

				if(m_undoStack.IsEmpty())
					return FALSE;

				while(m_undoStack.Peek() != 0xFFFFFFFF)
				{
					INT32 x = static_cast<INT32>(m_undoStack.Pop());
					INT32 z = static_cast<INT32>(m_undoStack.Pop());
					CRuTerrain_Chunk *undoChunk = reinterpret_cast<CRuTerrain_Chunk *>(m_undoStack.Pop());
					undoChunk->Release();
				}

				// Pop undo block marker
				m_undoStack.Pop();
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTerrain_DataSource_Remote::Undo_Trim(INT32 undoDepth)
{
	if(!m_undoActive)
	{
		INT32 curUndoDepth = 0;
		CRuStack<INT64> newUndoStack;

		while(m_undoStack.IsEmpty() == FALSE)
		{
			if(m_undoStack.Peek() == 0xFFFFFFFF)
			{
				// Increment current undo depth
				++curUndoDepth;

				if(curUndoDepth <= undoDepth)
				{
					// Pop undo block marker
					newUndoStack.Push(m_undoStack.Pop());

					if(m_undoStack.IsEmpty())
						return FALSE;

					while(m_undoStack.Peek() != 0xFFFFFFFF)
					{
						newUndoStack.Push(m_undoStack.Pop());
						newUndoStack.Push(m_undoStack.Pop());
						newUndoStack.Push(m_undoStack.Pop());
					}

					// Pop undo block marker
					newUndoStack.Push(m_undoStack.Pop());
				}
				else
				{
					// Pop undo block marker
					m_undoStack.Pop();

					if(m_undoStack.IsEmpty())
						return FALSE;

					while(m_undoStack.Peek() != 0xFFFFFFFF)
					{
						INT32 x = static_cast<INT32>(m_undoStack.Pop());
						INT32 z = static_cast<INT32>(m_undoStack.Pop());
						CRuTerrain_Chunk *undoChunk = reinterpret_cast<CRuTerrain_Chunk *>(m_undoStack.Pop());
						undoChunk->Release();
					}

					// Pop undo block marker
					m_undoStack.Pop();
				}
			}
		}

		// Push trimmed undo stack back
		while(newUndoStack.IsEmpty() == FALSE)
		{
			m_undoStack.Push(newUndoStack.Pop());
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTerrain_DataSource_Remote::ClearChunkQueue()
{
	if(m_chunkQueue)
	{
		m_chunkQueue->ForAllElements(ClearChunkQueueCallback, this);
		m_chunkQueue->Clear();
	}

	return TRUE;
}

BOOL CRuTerrain_DataSource_Remote::ClearChunkQueueCallback(const void *key, const void *data, void *userData)
{
	CRuTerrain_Chunk *cd = *((CRuTerrain_Chunk **) data);

	cd->Release();

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuTerrain::CRuTerrain()
:	m_splatMapBuilder(NULL),
	m_dataSource(NULL),
	m_chunkTree(NULL),
	m_textureList(NULL),
	m_objectContainer(NULL),
	m_objectManager(NULL),
	m_doodadContainer(NULL),
	m_doodadManager(NULL),
	m_zoneDescriptorContainer(NULL),
	m_pathHierarchy(NULL),
	m_highlightFlags(0xFFFFFFFF),
	m_enableWaterReflection(TRUE),
	m_enableWaterRefraction(TRUE),
	m_renderTerrain(TRUE),
	m_renderObjects(TRUE),
	m_renderGrass(TRUE),
	m_renderWater(TRUE)
{
	m_viewVolumeMarginSize = 120.0f;
	m_viewVolume.Set(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	// Construct texture list
	m_textureList = new CRuTextureList();

	// Prepend 2048 NULL textures into the texture list
	for(INT32 i = 0; i < 2048; ++i)
	{
		m_textureList->AddTexture(NULL);
	}

	m_chunkContainer = ruNEW CRuEntity();
	this->AddChild(m_chunkContainer);

	m_waterContainer = ruNEW CRuEntity();
	this->AddChild(m_waterContainer);

	// Create a hash map for water body descriptors
	m_waterBodyDescriptorMap = ruNEW CRuHashMap<CRuGUID, CRuWorld_WaterBodyDescriptor *>(1024);

	// Construct height field queue
	m_hfQueue = ruNEW CRuLRUQueue<DWORD, CRuMojo_AttributeParcel<REAL> *>(256, 1024);

	// Set self as decal target
	this->SetOption(ruENTITYOPTION_DECALTARGET, TRUE);

	// Register listener on engine settings
	g_ruEngineSettings->RegisterSettingsChangeHandler(this->GetGUID(), boost::bind(&CRuTerrain::EngineSettingsChangedHandler, this, _1, _2));

	// Apply settings from the engine
	ApplyEngineSettings();
}

CRuTerrain::~CRuTerrain()
{
	// Unregister listener on engine settings
	g_ruEngineSettings->UnregisterSettingsChangeHandler(this->GetGUID());

	ClearHeightFieldQueue();

	ruSAFE_RELEASE(m_chunkContainer);
	ruSAFE_RELEASE(m_waterContainer);

	// Release map builder
	ruSAFE_RELEASE(m_splatMapBuilder);

	// Release data source
	ruSAFE_RELEASE(m_dataSource);

	// Release chunk tree
	ruSAFE_RELEASE(m_chunkTree);

	ruSAFE_RELEASE(m_textureList);

	for(INT32 i = 0; i < m_waterBodyDescriptors.Count(); ++i)
	{
		ruSAFE_RELEASE(m_waterBodyDescriptors[i]);
	}
	m_waterBodyDescriptors.Clear();

	ruSAFE_DELETE(m_waterBodyDescriptorMap);
	ruSAFE_DELETE(m_hfQueue);

	// Release entity container and manager
	ruSAFE_RELEASE(m_objectContainer);
	ruSAFE_RELEASE(m_objectManager);

	// Release doodad container and manager
	ruSAFE_RELEASE(m_doodadContainer);
	ruSAFE_RELEASE(m_doodadManager);

	// Release zone descriptor container
	ruSAFE_RELEASE(m_zoneDescriptorContainer);

	// Release path hierarchy
	ruSAFE_RELEASE(m_pathHierarchy);
}

BOOL CRuTerrain::SerializeFrom(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuTerrain::SerializeTo(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuTerrain::ExportWorker::WorkerFunction()
{
	CRuTerrain_Chunk *chunk;
	IRuStream *outputStream;

	if(m_terrain->m_queue->DequeueWork(chunk))
	{
		// Initialize splat map builder if necessary
		if(m_splatMapBuilder == NULL)
		{
			CRuSplatIDMap *splatMap = chunk->GetSplatIDMap();
			m_splatMapBuilder = ruNEW CRuSplatMapBuilder();
			m_splatMapBuilder->Initialize(splatMap->GetSplatLayerSize());
		}

		// Serialize chunk to stream
		m_decompressedStream->SetStreamSize(0);
		chunk->SerializeToOptimizedChunk(m_decompressedStream, m_splatMapBuilder);

		// Compress stream
		outputStream = ruNEW CRuMemoryStream();
		m_streamCompressor->Compress(m_decompressedStream, outputStream);

		// Queue result
		m_terrain->m_queue->QueueResult(chunk, outputStream);
	}

	return TRUE;
}

BOOL CRuTerrain::SerializeToOptimizedTerrain(CRuDataStore_Base *dataStore, BOOL incremental)
{
	INT32 splatFactor = 2;

	// Initialize work queue
	m_queue = ruNEW CRuThread_WorkQueue<CRuTerrain_Chunk *, IRuStream *>(64, 64, 64);

	// Initialize worker threads
	INT32 numWorkerThreads = 4;
	ExportWorker **exportWorkers = ruNEW ExportWorker * [numWorkerThreads];
	CRuThread_Worker **workerThreads = ruNEW CRuThread_Worker * [numWorkerThreads];

	for(INT32 i = 0; i < numWorkerThreads; ++i)
	{
		// Create export worker object
		exportWorkers[i] = ruNEW ExportWorker();

		exportWorkers[i]->m_splatMapBuilder = NULL;

		exportWorkers[i]->m_streamCompressor = ruNEW CRuStreamCompressor();
		exportWorkers[i]->m_compressedStream = ruNEW CRuMemoryStream();
		exportWorkers[i]->m_decompressedStream = ruNEW CRuMemoryStream();

		exportWorkers[i]->m_terrain = this;

		// Register export worker with its thread
		CRuGUID guid;
		workerThreads[i] = ruNEW CRuThread_Worker();
		workerThreads[i]->RegisterWorkerFunction(guid, boost::bind(&ExportWorker::WorkerFunction, exportWorkers[i]));
		workerThreads[i]->SetSleepPeriod(5);
	}

	// Initialize stream compressor and workspace streams
	CRuStreamCompressor *streamCompressor = ruNEW CRuStreamCompressor();
	CRuMemoryStream *compressedStream = ruNEW CRuMemoryStream();
	CRuMemoryStream *decompressedStream = ruNEW CRuMemoryStream();

	compressedStream->SetBufferSize(1048576);
	decompressedStream->SetBufferSize(1048576);

	// Initialize data store
	INT32 tableHandle_Header;
	INT32 tableHandle_Water;
	INT32 tableHandle_Chunk;
	INT32 tableHandle_ChunkIteration;
	INT32 tableHandle_BackdropHeader;
	INT32 tableHandle_Backdrop;
	INT32 tableHandle_BackdropIteration;
	INT32 tableHandle_TextureList;

	tableHandle_Header = dataStore->CreateTable("header", 4);
	tableHandle_Water = dataStore->CreateTable("water_descriptor", 16);
	tableHandle_Chunk = dataStore->CreateTable("chunks", 4);
	tableHandle_ChunkIteration = dataStore->CreateTable("chunk_iterations", 4);
	tableHandle_BackdropHeader = dataStore->CreateTable("backdrop_header", 4);
	tableHandle_Backdrop = dataStore->CreateTable("backdrop", 4);
	tableHandle_BackdropIteration = dataStore->CreateTable("backdrop_iteration", 4);
	tableHandle_TextureList = dataStore->CreateTable("texture_list", 4);

	// Setup terrain parameters
	CRuVector3 origin = m_dataSource->GetOrigin();
	INT32 chunkDimension = m_dataSource->GetChunkDimension();
	INT32 numXChunks = m_dataSource->GetNumXChunks();
	INT32 numZChunks = m_dataSource->GetNumZChunks();
	INT32 splatResolution = m_dataSource->GetSplatResolution();
	REAL gridSize = m_dataSource->GetGridSize();
	INT32 lodLevels = m_dataSource->GetLODLevels();

	INT32 backdropGranularity = 4;
	INT32 numXGroups = numXChunks / backdropGranularity + ((numXChunks % backdropGranularity) ? 1 : 0);
	INT32 numZGroups = numZChunks / backdropGranularity + ((numZChunks % backdropGranularity) ? 1 : 0);

	INT32 backdropBaseLOD = 2;
	INT32 backdropPageStride = chunkDimension >> backdropBaseLOD;
	INT32 backdropPagePitch = backdropPageStride * (backdropPageStride * backdropGranularity + 1);
	INT32 backdropDimension = backdropPageStride * backdropGranularity;
	INT32 backdropCopyStride = 1 << backdropBaseLOD;
	REAL backdropGridSpacing = gridSize * backdropCopyStride;
	REAL *backdropHeightField = ruNEW REAL [(backdropDimension + 1) * (backdropDimension + 1)];

	// Write terrain header information
	decompressedStream->SetStreamSize(0);
	decompressedStream->Write(&origin.m_x, sizeof(REAL));
	decompressedStream->Write(&origin.m_y, sizeof(REAL));
	decompressedStream->Write(&origin.m_z, sizeof(REAL));
	decompressedStream->Write(&chunkDimension, sizeof(INT32));
	decompressedStream->Write(&numXChunks, sizeof(INT32));
	decompressedStream->Write(&numZChunks, sizeof(INT32));
	decompressedStream->Write(&splatResolution, sizeof(INT32));
	decompressedStream->Write(&gridSize, sizeof(REAL));
	decompressedStream->Write(&lodLevels, sizeof(INT32));

	DWORD dummyHeaderKey = 0;
	dataStore->Store(tableHandle_Header, reinterpret_cast<BYTE *>(&dummyHeaderKey), decompressedStream);

	// Write backdrop header information
	decompressedStream->SetStreamSize(0);
	decompressedStream->Write(&origin.m_x, sizeof(REAL));
	decompressedStream->Write(&origin.m_y, sizeof(REAL));
	decompressedStream->Write(&origin.m_z, sizeof(REAL));
	decompressedStream->Write(&backdropDimension, sizeof(INT32));
	decompressedStream->Write(&backdropGridSpacing, sizeof(INT32));
	decompressedStream->Write(&numXGroups, sizeof(INT32));
	decompressedStream->Write(&numZGroups, sizeof(INT32));

	DWORD dummyBackdropHeaderKey = 0;
	dataStore->Store(tableHandle_BackdropHeader, reinterpret_cast<BYTE *>(&dummyBackdropHeaderKey), decompressedStream);

	// Stuff...
	((CRuTerrain_DataSource_Remote *) m_dataSource)->SaveToOptimizedTerrain(dataStore);

	// Iterate through chunks in a grouped manner (e.g. in n by n blocks)
	for(INT32 zg = 0; zg < numZGroups; ++zg)
	{
		for(INT32 xg = 0; xg < numXGroups; ++xg)
		{
			BOOL generateBackdrop = FALSE;

			INT32 expectedResultCount = 0;

			for(INT32 z = 0; z < backdropGranularity; ++z)
			{
				for(INT32 x = 0; x < backdropGranularity; ++x)
				{
					// Re-construct chunk (X, Z) index
					INT32 cx = x + xg * backdropGranularity;
					INT32 cz = z + zg * backdropGranularity;

					// Construct chunk key
					DWORD chunkKey = (cx << 16) | cz;

					if(incremental)
					{
						// Retrieve source chunk iteration number
						INT32 sourceIterationNumber = m_dataSource->GetChunkIterationNumber(cx, cz);

						// Retrieve target chunk iteration number
						INT32 targetIterationNumber = 0;
						IRuStream *iterationStream = dataStore->Retrieve(tableHandle_ChunkIteration, reinterpret_cast<BYTE *>(&chunkKey));
						iterationStream->Read(&targetIterationNumber, sizeof(INT32));
						delete iterationStream;

						if(sourceIterationNumber == targetIterationNumber)
						{
							continue;
						}
					}

					CRuTerrain_Chunk *chunk = m_dataSource->LoadChunk(cx, cz);

					if(chunk)
					{
						// Mark backdrop generation as necessary
						generateBackdrop = TRUE;

						++expectedResultCount;
						m_queue->QueueWork(chunk);
					}
				}
			}

			// Wait for work to come back
			while(expectedResultCount > 0)
			{
				CRuTerrain_Chunk *chunk;
				IRuStream *stream;

				while(m_queue->DequeueResult(chunk, stream))
				{
					DWORD chunkKey = chunk->GetChunkKey();

					// Store chunk to data store
					dataStore->Store(tableHandle_Chunk, reinterpret_cast<BYTE *>(&chunkKey), stream);
					ruSAFE_DELETE(stream);

					// Store chunk iteration to data store
					INT32 chunkIterationNumber = chunk->GetChunkIteration();
					decompressedStream->SetStreamSize(0);
					decompressedStream->Write(&chunkIterationNumber, sizeof(INT32));
					dataStore->Store(tableHandle_ChunkIteration, reinterpret_cast<BYTE *>(&chunkKey), decompressedStream);

					// decrement
					--expectedResultCount;
				}

				Sleep(100);
			}

			if(generateBackdrop)
			{
				for(INT32 z = 0; z < backdropGranularity; ++z)
				{
					for(INT32 x = 0; x < backdropGranularity; ++x)
					{
						// Re-construct chunk (X, Z) index
						INT32 cx = x + xg * backdropGranularity;
						INT32 cz = z + zg * backdropGranularity;

						CRuTerrain_Chunk *chunk = m_dataSource->LoadChunk(cx, cz);

						if(chunk)
						{
							// Copy chunk height field to backdrop
							CRuMojo_AttributeParcel<REAL> *chunkHF = chunk->GetHeightField();
							REAL *hf = chunkHF->GetParcel();
							INT32 hfOffset = chunkHF->GetParcelOverlapGap() + chunkHF->GetParcelOverlapGap() * chunkHF->GetParcelWidth();
							INT32 hfPitch = chunkHF->GetParcelWidth();

							INT32 pageOffset = x * backdropPageStride + z * backdropPagePitch;

							for(INT32 j = 0; j <= backdropPageStride; ++j)
							{
								for(INT32 i = 0; i <= backdropPageStride; ++i)
								{
									backdropHeightField[pageOffset + i + j * (backdropDimension + 1)] = hf[hfOffset + i * backdropCopyStride + (j * backdropCopyStride) * hfPitch];
								}
							}
						}
					}
				}

				// Construct backdrop key
				DWORD backdropKey = (xg << 16) | zg;

				// Calculate backdrop origin
				CRuAABB originChunkBounds = m_dataSource->GetChunkBounds(xg * backdropGranularity, zg * backdropGranularity);
				CRuVector3 origin(originChunkBounds.Minima().m_x, 0.0f, originChunkBounds.Minima().m_z);

				// Create backdrop
				CRuWorld_Backdrop *backdrop = ruNEW CRuWorld_Backdrop();
				backdrop->SetBackdropKey(backdropKey);
				backdrop->Initialize(origin, backdropGridSpacing, backdropDimension, 2, backdropHeightField, NULL);

				// Serialize backdrop to stream
				decompressedStream->SetStreamSize(0);
				backdrop->SerializeTo(decompressedStream);

				compressedStream->SetStreamSize(0);
				streamCompressor->Compress(decompressedStream, compressedStream);

				// Store backdrop to data store
				dataStore->Store(tableHandle_Backdrop, reinterpret_cast<BYTE *>(&backdropKey), compressedStream);

				ruSAFE_RELEASE(backdrop);
			}

			GetDataSource()->FlushChunkQueue();
			GetObjectContainer()->ClearInstanceQueue();

			g_ruResourceManager->ClearTextureDictionary();
			g_ruResourceManager->ClearEntityDictionary();
			g_ruResourceManager->ClearAnimationDictionary();
		}
	}

	delete[] backdropHeightField;

	// Save texture list to target data store
	m_textureList->Save(dataStore, TRUE);

	// Copy entity container to data store
	m_objectContainer->CopyTo(dataStore);

	// Copy doodad container to data store
	m_doodadContainer->CopyTo(dataStore);

	// Copy zone descriptor container to data store
	m_zoneDescriptorContainer->CopyTo(dataStore);

	// Copy path hierarchy to data store
	m_pathHierarchy->CopyTo(dataStore);

	// Free resources
	delete streamCompressor;
	delete compressedStream;
	delete decompressedStream;

	// Clean up worker threads..
	for(INT32 i = 0; i < numWorkerThreads; ++i)
	{
		ruSAFE_RELEASE(exportWorkers[i]->m_splatMapBuilder);
		ruSAFE_DELETE(exportWorkers[i]->m_streamCompressor);
		ruSAFE_DELETE(exportWorkers[i]->m_compressedStream);
		ruSAFE_DELETE(exportWorkers[i]->m_decompressedStream);
		ruSAFE_DELETE(exportWorkers[i]);

		ruSAFE_DELETE(workerThreads[i]);
	}

	ruSAFE_DELETE(m_queue);
	ruSAFE_DELETE(exportWorkers);
	ruSAFE_DELETE(workerThreads);

	// Clear visible entities list, since cached terrain chunks may have changed
	m_viewVolume.Set(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	m_enumeratedChunks.Clear();

	m_visibleChunks.Clear();
	m_visibleGrass.Clear();
	m_visibleEntities.Clear();
	m_visibleWaterEntities.Clear();

	return TRUE;
}

BOOL CRuTerrain::CopyTo(CRuDataStore_Base *dstDataStore)
{
	return static_cast<CRuTerrain_DataSource_Remote *>(m_dataSource)->CopyTo(dstDataStore);
}

BOOL CRuTerrain::ResetStates()
{
	return TRUE;
}

BOOL CRuTerrain::Update(float elapsedTime)
{
	// Update active water descriptors
	for(INT32 i = 0; i < m_waterBodyDescriptors.Count(); ++i)
	{
		m_waterBodyDescriptors[i]->Update(elapsedTime);
	}

	return IRuEntity_Container::Update(elapsedTime);
}

BOOL CRuTerrain::QueryCollision(CRuCollisionQuery *colQuery)
{
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
				{
					CRuCollisionQuery_Ray *rayQuery = static_cast<CRuCollisionQuery_Ray *>(colQuery);

					// Get a list intersecting chunks
					CRuArrayList<DWORD> chunkKeyList;
					m_chunkTree->EnumerateChunks(rayQuery->m_colRay, chunkKeyList);

					for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
					{
						// Decode the chunk key
						INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
						INT32 z = chunkKeyList[i] & 0x0000FFFF;

						// Load the chunk
						CRuTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z);
						if(chunk)
						{
							// Query ray collision with the chunk
							chunk->QueryCollision(rayQuery);
						}
					}

					// Query collision against instanced entities
//					m_dataSource->GetEntityContainer()->QueryCollision(rayQuery);
				}

				return TRUE;

			case ruCOLQUERY_RAY_SUBOBJECT:
				{
//					return m_dataSource->GetEntityContainer()->QueryCollision(colQuery);
					return TRUE;
				}

				break;

			case ruCOLQUERY_SEGMENT:
			case ruCOLQUERY_SEGMENT_HITTEST:
			case ruCOLQUERY_SEGMENT_NEAREST:
				{
					CRuCollisionQuery_Segment_Base *segQuery = static_cast<CRuCollisionQuery_Segment_Base *>(colQuery);

					// Get a list intersecting chunks
					CRuArrayList<DWORD> chunkKeyList;
					m_chunkTree->EnumerateChunks(segQuery->m_colSegment, chunkKeyList);

					for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
					{
						// Decode the chunk key
						INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
						INT32 z = chunkKeyList[i] & 0x0000FFFF;

						// Load the chunk
						CRuTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z);
						if(chunk)
						{
							// Query ray collision with the chunk
							chunk->QueryCollision(segQuery);
						}
					}

					// Query collision against object manager
					m_objectManager->QueryCollision(segQuery);
				}

				return TRUE;

			case ruCOLQUERY_SEGMENT_SUBOBJECT:
				{
//					return m_dataSource->GetEntityContainer()->QueryCollision(colQuery);
					return TRUE;
				}

				break;

			case ruCOLQUERY_OBB_HITTEST:
			case ruCOLQUERY_OBB_TRIENUM:
				{
					CRuCollisionQuery_OBB_Base *boxQuery = static_cast<CRuCollisionQuery_OBB_Base *>(colQuery);

					// Get a list intersecting chunks
					CRuArrayList<DWORD> chunkKeyList;
					m_chunkTree->EnumerateChunks(boxQuery->m_colOBB, chunkKeyList);

					for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
					{
						// Decode the chunk key
						INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
						INT32 z = chunkKeyList[i] & 0x0000FFFF;

						// Load the chunk
						CRuTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z);
						if(chunk)
						{
							// Query ray collision with the chunk
							chunk->QueryCollision(boxQuery);
						}
					}

					// Query collision against object manager
					m_objectManager->QueryCollision(boxQuery);
				}

				return TRUE;

			case ruCOLQUERY_SPHERE_HITTEST:
			case ruCOLQUERY_SPHERE_TRIENUM:
				{
					CRuCollisionQuery_Sphere_Base *sphereQuery = static_cast<CRuCollisionQuery_Sphere_Base *>(colQuery);

					// Get a list intersecting chunks
					CRuArrayList<DWORD> chunkKeyList;
					m_chunkTree->EnumerateChunks(sphereQuery->m_colSphere, chunkKeyList);

					for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
					{
						// Decode the chunk key
						INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
						INT32 z = chunkKeyList[i] & 0x0000FFFF;

						// Load the chunk
						CRuTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z);
						if(chunk)
						{
							// Query ray collision with the chunk
							chunk->QueryCollision(sphereQuery);
						}
					}

					// Query collision against object manager
					m_objectManager->QueryCollision(sphereQuery);
				}

				return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuTerrain::Initialize()
{
	return TRUE;
}

BOOL CRuTerrain::AttachDataSource(IRuTerrain_DataSource *dataSource)
{
	// Release previous map builder
	ruSAFE_RELEASE(m_splatMapBuilder);

	// Release previous data source
	ruSAFE_RELEASE(m_dataSource);

	// Release previously allocated data




	// Attach to new data source
	m_dataSource = dataSource;
	m_dataSource->AddRef();

	// Initialize new map builder
//	m_splatMapBuilder = ruNEW CRuSplatMapBuilder();
//	m_splatMapBuilder->Initialize(m_dataSource->GetSplatResolution() * 2);

	// Get terrain dimensions
	INT32 numXChunks = m_dataSource->GetNumXChunks();
	INT32 numZChunks = m_dataSource->GetNumZChunks();
	INT32 totalChunks = numXChunks * numZChunks;

	// Force total chunks to be a power of 2
	totalChunks = RoundUpToPow2(totalChunks);

	m_chunkTree = ruNEW CRuWorld_ChunkTree();
	m_chunkTree->Initialize(m_dataSource->GetTerrainBounds(), 32);

	// Insert chunk descriptors into partitioning structure
	for(INT32 z = 0; z < numZChunks; ++z)
	{
		for(INT32 x = 0; x < numXChunks; ++x)
		{
			m_chunkTree->InsertChunk(x, z, m_dataSource->GetChunkBounds(x, z));
		}
	}

	// Load texture list
	dataSource->LoadTextureList(m_textureList);

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

	// Construct object container
	m_objectContainer = ruNEW CRuWorld_ObjectContainer(m_dataSource->GetTerrainBounds(), m_dataSource->GetGridSize() * m_dataSource->GetChunkDimension() * ruWORLD_OBJECTCELLMULTIPLIER);
	m_objectContainer->Attach(static_cast<CRuTerrain_DataSource_Remote *>(m_dataSource)->GetDataStore());

	// Construct object manager
	m_objectManager = ruNEW CRuWorld_ObjectManager();
	m_objectManager->Attach(m_objectContainer);

	// Construct doodad container
	m_doodadContainer = ruNEW CRuWorld_DoodadContainer();
	m_doodadContainer->Attach(static_cast<CRuTerrain_DataSource_Remote *>(m_dataSource)->GetDataStore(), FALSE);

	// Construct doodad manager
	m_doodadManager = ruNEW CRuWorld_DoodadManager();
	m_doodadManager->Attach(m_doodadContainer);
	m_doodadManager->SetEnumerator(boost::bind(&CRuTerrain::GetDoodadEnumeration, this, _1, _2));

	// Construct zone descriptor container
	m_zoneDescriptorContainer = ruNEW CRuWorld_ZoneDescriptorContainer();
	m_zoneDescriptorContainer->Attach(static_cast<CRuTerrain_DataSource_Remote *>(m_dataSource)->GetDataStore());

	// Construct path hierarchy
	m_pathHierarchy = ruNEW CRuPathHierarchy_DS();
	m_pathHierarchy->Bind(static_cast<CRuTerrain_DataSource_Remote *>(m_dataSource)->GetDataStore(), m_dataSource->GetTerrainBounds());
	m_pathHierarchy->SetCollisionObject(this);

	return TRUE;
}

void CRuTerrain::SetCamera(CRuCamera *viewCamera, CRuAABB *overrideViewVolume)
{
	// Enumerate only if the view volume has changed significantly
	if(m_viewVolume.Contains(viewCamera->GetSweepBounds()) == FALSE)
	{
		// Clear the enumeration cache
		this->ClearEnumerationCache();

		// Set view volume to the camera sweep bounds plus margin
		if(overrideViewVolume)
		{
			m_viewVolume = *overrideViewVolume;
		}
		else
		{
			m_viewVolume = viewCamera->GetSweepBounds();
		}

		m_viewVolume.Minima().m_x -= m_viewVolumeMarginSize;
		m_viewVolume.Minima().m_y -= m_viewVolumeMarginSize;
		m_viewVolume.Minima().m_z -= m_viewVolumeMarginSize;
		m_viewVolume.Maxima().m_x += m_viewVolumeMarginSize;
		m_viewVolume.Maxima().m_y += m_viewVolumeMarginSize;
		m_viewVolume.Maxima().m_z += m_viewVolumeMarginSize;

		// Remove all children
		m_chunkContainer->RemoveAllChildren();

		if(m_chunkTree)
		{
			// Get a list of keys to visible chunks
			CRuArrayList<DWORD> chunkKeyList;
			m_chunkTree->EnumerateChunks(m_viewVolume, chunkKeyList);

			for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
			{
				// Decode the chunk key
				INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
				INT32 z = chunkKeyList[i] & 0x0000FFFF;

				// Retrieve the chunk from the data source
				CRuTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z);
				if(chunk)
				{
					m_enumeratedChunks.Add(chunk);

					// Add chunk to tree
					m_chunkContainer->AddChild(chunk);

					CRuVector3 camPos = viewCamera->GetCameraPosition();
					CRuAABB bBox = chunk->GetBoundingBox();
					camPos.y = 0.0f;
					bBox.Minima().y = 0.0f;
					bBox.Maxima().y = 0.0f;

					float distanceToChunk = RuSqrt(SquaredDistance(camPos, bBox));

					if(distanceToChunk < 900.0f)
					{
						// Iterate through doodad lists
						for(INT32 j = 0; j < chunk->GetNumDoodadClusters(); ++j)
						{
							m_enumeratedDoodadKeys.Add(chunkKeyList[i]);
							m_enumeratedDoodadClusters.Add(chunk->GetDoodadCluster(j));
						}
					}
				}
			}
		}
	}

	m_waterContainer->RemoveAllChildren();

	// Clear visible entities list
	m_visibleChunks.Clear();
	m_visibleGrass.Clear();
	m_visibleEntities.Clear();
	m_visibleWaterEntities.Clear();

	INT32 chunkDimension = m_dataSource->GetChunkDimension();

	for(INT32 i = 0; i < m_enumeratedChunks.Count(); ++i)
	{
		// Retrieve the chunk from the data source
		CRuTerrain_Chunk *chunk = m_enumeratedChunks[i];
		if(chunk)
		{
			// Force LOD selection
			chunk->SetCamera(viewCamera);

			if(m_highlightOn)
			{
				CRuCollisionQuery_Segment_Nearest segQuery;
				CRuMatrix4x4 identityMatrix;
				segQuery.m_worldTransform = &identityMatrix;
				segQuery.m_invWorldTransform = &identityMatrix;
				segQuery.m_ignoreCollisionFlag = TRUE;
				segQuery.m_colSegment = m_highlightSegment;

				// Perform collision query
				chunk->QueryCollision(&segQuery);

				if(segQuery.m_positiveHit)
				{
					chunk->SetHighlight(TRUE);
				}
				else
				{
					chunk->SetHighlight(FALSE);
				}
			}
			else
			{
				BOOL chunkIsMarked = m_dataSource->ChunkIsMarked(chunk);

				if(m_highlightFlags != 0xFFFFFFFF && RuWorld_ZoneFlags_GetID(chunk->GetChunkFlags()) == m_highlightFlags)
				{
					chunk->SetHighlight(TRUE, 0xFFFF0000, 0.2f);
				}
				else if(chunkIsMarked)
				{
					chunk->SetHighlight(TRUE, 0xFFFF0099, 0.2f);
				}
				else
				{
					chunk->SetHighlight(FALSE);
				}
			}

			chunk->SetOutline(m_outlineOn);

			// Fill in initial mesh information
			IRuMesh *mesh;
			IRuMaterial *material;
			chunk->GetMesh(0, &mesh, &material);

			// Set number of layers
			CRuVector3 camPos = viewCamera->GetCameraPosition();
			CRuAABB bBox = chunk->GetBoundingBox();
			camPos.y = 0.0f;
			bBox.Minima().y = 0.0f;
			bBox.Maxima().y = 0.0f;

			float distanceToChunk = RuSqrt(SquaredDistance(camPos, bBox));

			if(distanceToChunk > 2500)
//			if(distanceToChunk > g_ruRendererOptions.m_terrain_TextureDistance)
			{
				material->GetParameterBlock()->SetParameter(ruPARAMTYPE_SPLAT_LAYERCOUNT, 0);
			}
			else
			{
				CRuSplatIDMap *idMap = chunk->GetSplatIDMap();

				// If the builder has not yet been initialized, do it now
				if(m_splatMapBuilder == NULL)
				{
					m_splatMapBuilder = ruNEW CRuSplatMapBuilder();
					m_splatMapBuilder->Initialize(idMap->GetSplatLayerSize());
				}

				// Build alpha maps
				m_splatMapBuilder->BuildAlphaMaps(idMap, m_enableSplatFilter);

				// Setup the alpha texture UV modifier
				float alphaUVModifier[4] = { (float) chunkDimension, (float) chunkDimension, 0.0f, 0.0f };
				mesh->GetBatchParameterBlock(0)->SetParameter(ruPARAMTYPE_SPLAT_ALPHAUVMODIFIER, (UINT64) alphaUVModifier);

				// Setup data references
				INT32 numAlphaMaps = idMap->GetNumAlphaMaps();
				INT32 numLayers = idMap->GetNumLayers();

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

			// Display water layers
			for(INT32 j = 0; j < chunk->GetNumWaterLayers(); ++j)
			{
				CRuWorld_Water *waterLayer = chunk->GetWaterLayerByIndex(j);

				// Override material
				CRuWorld_WaterBodyDescriptor *descriptor = NULL;
				if(m_waterBodyDescriptorMap->Get(waterLayer->GetWaterBodyGUID(), waterLayer->GetWaterBodyGUID().HashCode(), descriptor))
				{
					// Set water layer material
					waterLayer->SetWaterLayerMaterial(descriptor->m_waterMaterial);

					// Update collision flags
					waterLayer->UpdateCollisionFlags(descriptor->m_bodyType);
				}

				m_waterContainer->AddChild(waterLayer);

				// Add water layer to list of visible water entities
				m_visibleWaterEntities.Add(waterLayer);
			}

			// Add to visible entities list
			m_visibleChunks.Add(chunk);
		}
	}

	// Update entity enumeration
	m_objectManager->UpdateEnumeration(m_viewVolume);

	// Update doodad enumeration
	m_doodadManager->UpdateEnumeration(m_viewVolume);

	m_renderTerrain ? this->AddChild(m_chunkContainer) : m_chunkContainer->DetachFromParent();
	m_renderObjects ? this->AddChild(m_objectManager) : m_objectManager->DetachFromParent();
	m_renderGrass ? this->AddChild(m_doodadManager) : m_doodadManager->DetachFromParent();
	m_renderWater ? this->AddChild(m_waterContainer) : m_waterContainer->DetachFromParent();
}

void CRuTerrain::SetHighlightOn(BOOL highlightOn)
{
	m_highlightOn = highlightOn;
}

void CRuTerrain::SetHighlightSegment(const CRuSegment3 &segment)
{
	m_highlightSegment = segment;
}

void CRuTerrain::SetHighlightFlags(UINT32 highlightFlags)
{
	m_highlightFlags = highlightFlags;
}

void CRuTerrain::SetOutlineOn(BOOL outlineOn)
{
	m_outlineOn = outlineOn;
}

INT32 CRuTerrain::GetNumSplatsAtCursor(const CRuSegment3 &segment, char *splatIDListOut)
{
	if(splatIDListOut)
	{
		splatIDListOut[0] = NULL;
	}

	if(m_chunkTree)
	{
		CRuVector3 nearestPoint;
		float nearestDistance = FLT_MAX;
		INT32 nearestSplatCount = 0;

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(segment, chunkKeyList);

		// Iterate through each chunk and find the nearest collision point
		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z);
			if(chunk)
			{

				CRuCollisionQuery_Segment_Nearest segQuery;
				CRuMatrix4x4 identityMatrix;
				segQuery.m_worldTransform = &identityMatrix;
				segQuery.m_invWorldTransform = &identityMatrix;
				segQuery.m_ignoreCollisionFlag = TRUE;
				segQuery.m_colSegment = segment;

				// Query segment collision with the chunk
				chunk->QueryCollision(&segQuery);

				if(segQuery.m_positiveHit)
				{
					// If the nearest intersection is the current nearest, use it
					if(segQuery.m_collisionT < nearestDistance)
					{
						nearestPoint = segQuery.m_collisionPoint;
						nearestDistance = segQuery.m_collisionT;

						if(chunk->GetSplatIDMap())
						{
							nearestSplatCount = chunk->GetSplatIDMap()->GetNumLayers();
							chunk->GetSplatIDMap()->GetIDListAsString(splatIDListOut);
						}
						else
						{
							nearestSplatCount = 0;
						}
					}
				}
			}
		}

		if(nearestDistance < FLT_MAX)
		{
			return nearestSplatCount;
		}
	}

	return 0;
}

void CRuTerrain::ToggleOption_RenderTerrain()
{
	m_renderTerrain = !m_renderTerrain;
}

void CRuTerrain::ToggleOption_RenderObjects()
{
	m_renderObjects = !m_renderObjects;
}

void CRuTerrain::ToggleOption_RenderGrass()
{
	m_renderGrass = !m_renderGrass;
}

void CRuTerrain::ToggleOption_RenderWater()
{
	m_renderWater = !m_renderWater;
}

void CRuTerrain::ToggleOption_RenderTriggers(BOOL renderTriggers)
{
	if(m_objectManager)
	{
		m_objectManager->EnableTriggerDisplay(renderTriggers);
	}
}

void CRuTerrain::ToggleOption_FilterTriggers(BOOL filterTriggers)
{
	if(m_objectManager)
	{
		m_objectManager->EnableTriggerDisplayFilter(filterTriggers);
		m_trigger_EnableFilter = filterTriggers;
	}
}

void CRuTerrain::SetOption_FilterTriggerID(INT32 id)
{
	if(m_objectManager)
	{
		m_objectManager->SetTriggerDisplayFilterID(id);
		m_trigger_FilterID = id;
	}
}

void CRuTerrain::SetEditingEnvironmentColors(DWORD primaryColor, DWORD secondaryColor, DWORD specularColor, DWORD fogColor, float theta, float phi)
{
	m_dataSource->SetEditingEnvironmentColors(primaryColor, secondaryColor, specularColor, fogColor, theta, phi);
}

void CRuTerrain::GetEditingEnvironmentColors(DWORD &primaryColor, DWORD &secondaryColor, DWORD &specularColor, DWORD &fogColor, float &theta, float &phi)
{
	m_dataSource->GetEditingEnvironmentColors(primaryColor, secondaryColor, specularColor, fogColor, theta, phi);
}

BOOL CRuTerrain::FindIntersection(const CRuSegment3 &segment, CRuVector3 &intPtOut, float &intDistOut)
{
	if(m_chunkTree)
	{
		CRuVector3 nearestPoint;
		float nearestDistance = FLT_MAX;

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(segment, chunkKeyList);

		// Iterate through each chunk and find the nearest collision point
		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z);
			if(chunk)
			{
				CRuCollisionQuery_Segment_Nearest segQuery;
				CRuMatrix4x4 identityMatrix;
				segQuery.m_worldTransform = &identityMatrix;
				segQuery.m_invWorldTransform = &identityMatrix;
				segQuery.m_ignoreCollisionFlag = FALSE;
				segQuery.m_exclusionMask = RuWorld::ruTRIFLAGS_WATER;
				segQuery.m_colSegment = segment;

				// Query segment collision with the chunk
				chunk->QueryCollision(&segQuery);

				if(segQuery.m_positiveHit)
				{
					// If the nearest intersection is the current nearest, use it
					if(segQuery.m_collisionT < nearestDistance)
					{
						nearestPoint = segQuery.m_collisionPoint;
						nearestDistance = segQuery.m_collisionT;
					}
				}
			}
		}

		if(nearestDistance < FLT_MAX)
		{
			intPtOut = nearestPoint;
			intDistOut = nearestDistance;

			return TRUE;
		}
	}

	return FALSE;
}

CRuGUID CRuTerrain::FindIntersectingEntityDescriptor(const CRuSegment3 &segment, CRuVector3 &intPtOut, float &intDistOut)
{
	// Get the nearest intersecting object
	IRuObject *nearestObj = m_objectContainer->PickObject(segment, intPtOut, intDistOut);

	// If the intersecting object is a CRuWorld_EntityDescriptor, return its GUID
	if(nearestObj && nearestObj->GetType() == CRuWorld_EntityDescriptor::Type())
	{
		return nearestObj->GetGUID();
	}

	// Otherwise return an invalid GUID
	return CRuGUID::Invalid;
}

CRuGUID CRuTerrain::FindWaterBodyIntersection(const CRuSegment3 &segment, CRuVector3 &intPtOut, float &intDistOut)
{
	if(m_chunkTree)
	{
		CRuVector3 nearestPoint;
		float nearestDistance = FLT_MAX;
		CRuGUID nearestWaterBodyGUID = CRuGUID::Invalid;

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(segment, chunkKeyList);

		// Iterate through each chunk and find the nearest collision point
		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z);
			if(chunk)
			{
				CRuVector3 pt;
				REAL t;
				
				CRuWorld_Water *waterLayer = chunk->GetNearestWaterLayer(segment, pt, t);

				if(waterLayer && t < nearestDistance)
				{
					nearestPoint = pt;
					nearestDistance = t;
					nearestWaterBodyGUID = waterLayer->GetWaterBodyGUID();
				}
			}
		}

		if(nearestDistance < FLT_MAX)
		{
			intPtOut = nearestPoint;
			intDistOut = nearestDistance;

			return nearestWaterBodyGUID;
		}
	}

	return CRuGUID::Invalid;
}

CRuTerrain_Chunk *CRuTerrain::FindIntersectingChunk(const CRuSegment3 &segment, CRuVector3 &intPtOut, float &intDistOut)
{
	if(m_chunkTree)
	{
		CRuVector3 nearestPoint;
		float nearestDistance = FLT_MAX;
		CRuTerrain_Chunk *nearestChunk = NULL;

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(segment, chunkKeyList);

		// Iterate through each chunk and find the nearest collision point
		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z);
			if(chunk)
			{
				CRuCollisionQuery_Segment_Nearest segQuery;
				CRuMatrix4x4 identityMatrix;
				segQuery.m_worldTransform = &identityMatrix;
				segQuery.m_invWorldTransform = &identityMatrix;
				segQuery.m_ignoreCollisionFlag = TRUE;
				segQuery.m_colSegment = segment;

				// Query segment collision with the chunk
				chunk->QueryCollision(&segQuery);

				if(segQuery.m_positiveHit)
				{
					// If the nearest intersection is the current nearest, use it
					if(segQuery.m_collisionT < nearestDistance)
					{
						nearestPoint = segQuery.m_collisionPoint;
						nearestDistance = segQuery.m_collisionT;
						nearestChunk = chunk;
					}
				}
			}
		}

		if(nearestDistance < FLT_MAX)
		{
			intPtOut = nearestPoint;
			intDistOut = nearestDistance;

			return nearestChunk;
		}
	}

	return NULL;
}

void CRuTerrain::EnableSplatFilter(BOOL enableSplatFilter)
{
	m_enableSplatFilter = enableSplatFilter;
}

BOOL CRuTerrain::Undo_Begin()
{
	return m_dataSource->Undo_Begin();
}

BOOL CRuTerrain::Undo_End()
{
	return m_dataSource->Undo_End();
}

BOOL CRuTerrain::Undo()
{
	BOOL undoResult = m_dataSource->Undo();

	m_visibleChunks.Clear();

	// Clear the enumeration cache
	this->ClearEnumerationCache();

	return undoResult;
}

BOOL CRuTerrain::Redo()
{
	return m_dataSource->Redo();
}

void CRuTerrain::MarkChunkAsChanged(const CRuAABB &box)
{
	// Perform frustum culling on the terrain
	if(m_chunkTree)
	{
		// Get a list visible chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(box, chunkKeyList);

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk
			m_dataSource->LockChunk(x, z);
		}
	}
}

void CRuTerrain::PaintFlags(const CRuVector3 &center, float radius, UINT32 flags, UINT32 flagMask)
{
	if(m_chunkTree)
	{
		float effectPadding = 15.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				BOOL chunkChanged = chunk->PaintFlags(center, radius, flags, flagMask);
			}
		}
	}
}

void CRuTerrain::PaintHeight(const CRuVector3 &center, float radius, float delta, float refHeight, BOOL adaptivePainting, RuFalloffFunction falloffFunction)
{
	if(m_chunkTree)
	{
		float effectPadding = 15.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				BOOL chunkChanged = chunk->PaintHeight(center, radius, delta, refHeight, adaptivePainting, falloffFunction);
			}
		}
	}
}

void CRuTerrain::NoiseHeight(const CRuVector3 &center, float radius, float variation)
{
	if(m_chunkTree)
	{
		float effectPadding = 15.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				chunk->NoiseHeight(center, radius, variation);
			}
		}

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				// Stitch chunks together
				chunk->Stitch(0, m_dataSource->LoadChunk(x - 1, z - 1));
				chunk->Stitch(1, m_dataSource->LoadChunk(x, z - 1));
				chunk->Stitch(2, m_dataSource->LoadChunk(x + 1, z - 1));
				chunk->Stitch(3, m_dataSource->LoadChunk(x - 1, z));
				chunk->Stitch(4, m_dataSource->LoadChunk(x + 1, z));
				chunk->Stitch(5, m_dataSource->LoadChunk(x - 1, z + 1));
				chunk->Stitch(6, m_dataSource->LoadChunk(x, z + 1));
				chunk->Stitch(7, m_dataSource->LoadChunk(x + 1, z + 1));
			}
		}
	}
}

void CRuTerrain::SetHeight(const CRuVector3 &center, float radius, float height, float brushStrength, RuFalloffFunction falloffFunction)
{
	if(m_chunkTree)
	{
		float effectPadding = 15.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				chunk->SetHeight(center, radius, height, brushStrength, falloffFunction);
			}
		}

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				// Stitch chunks together
				chunk->Stitch(0, m_dataSource->LoadChunk(x - 1, z - 1));
				chunk->Stitch(1, m_dataSource->LoadChunk(x, z - 1));
				chunk->Stitch(2, m_dataSource->LoadChunk(x + 1, z - 1));
				chunk->Stitch(3, m_dataSource->LoadChunk(x - 1, z));
				chunk->Stitch(4, m_dataSource->LoadChunk(x + 1, z));
				chunk->Stitch(5, m_dataSource->LoadChunk(x - 1, z + 1));
				chunk->Stitch(6, m_dataSource->LoadChunk(x, z + 1));
				chunk->Stitch(7, m_dataSource->LoadChunk(x + 1, z + 1));
			}
		}
	}
}

void CRuTerrain::SmoothHeight(const CRuVector3 &center, float radius, float delta)
{
	if(m_chunkTree)
	{
		float effectPadding = 15.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				chunk->SmoothHeight(center, radius, delta);
			}
		}

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				// Stitch chunks together
				chunk->Stitch(0, m_dataSource->LoadChunk(x - 1, z - 1));
				chunk->Stitch(1, m_dataSource->LoadChunk(x, z - 1));
				chunk->Stitch(2, m_dataSource->LoadChunk(x + 1, z - 1));
				chunk->Stitch(3, m_dataSource->LoadChunk(x - 1, z));
				chunk->Stitch(4, m_dataSource->LoadChunk(x + 1, z));
				chunk->Stitch(5, m_dataSource->LoadChunk(x - 1, z + 1));
				chunk->Stitch(6, m_dataSource->LoadChunk(x, z + 1));
				chunk->Stitch(7, m_dataSource->LoadChunk(x + 1, z + 1));
			}
		}
	}
}

void CRuTerrain::AutoPaintHeight(const CRuVector3 &center, float radius, CRuFractal_Base *srcFractal, const CRuSplatIDMap_PaintSetting &paintSetting)
{
	if(m_chunkTree)
	{
		float effectPadding = 15.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				BOOL chunkChanged = chunk->AutoPaintHeight(center, radius, srcFractal, paintSetting);
			}
		}
	}
}

void CRuTerrain::PaintMask(const CRuVector3 &center, float radius, BOOL maskOn)
{
	if(m_chunkTree)
	{
		float effectPadding = 15.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				chunk->PaintMask(center, radius, maskOn);
			}
		}
	}
}

void CRuTerrain::PaintOffset(const CRuVector3 &center, float radius, REAL offset)
{
	if(m_chunkTree)
	{
		float effectPadding = 15.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				chunk->PaintOffset(center, radius, offset);
			}
		}
	}
}

void CRuTerrain::PaintSplat(const CRuVector3 &center, float radius, REAL power, RuFalloffFunction falloffFunc, INT32 iteration, UINT16 splatID)
{
	if(m_chunkTree)
	{
		float effectPadding = 15.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		CRuVector3 paintCenter(center);
		paintCenter.y = 0.0f;

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				chunk->PaintSplat(paintCenter, radius, power, falloffFunc, iteration, splatID);
			}
		}
	}
}

void CRuTerrain::SprayPaintSplat(const CRuVector3 &center, float radius, REAL power, RuFalloffFunction falloffFunc, INT32 iteration, UINT16 splatID, float sprayProbability)
{
	if(m_chunkTree)
	{
		float effectPadding = 15.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		CRuVector3 paintCenter(center);
		paintCenter.y = 0.0f;

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				chunk->SprayPaintSplat(paintCenter, radius, power, falloffFunc, iteration, splatID, sprayProbability);
			}
		}
	}
}

void CRuTerrain::BlockFillSplat(const CRuVector3 &center, float radius, UINT16 splatID)
{
	if(m_chunkTree)
	{
		float effectPadding = 15.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		CRuVector3 paintCenter(center);
		paintCenter.y = 0.0f;

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				chunk->BlockFillSplat(paintCenter, radius, splatID);
			}
		}
	}
}

void CRuTerrain::FloodFillSplat(const CRuVector3 &center, float radius, UINT16 splatID)
{
	if(m_chunkTree)
	{
		// Build segment for the fill
		CRuSegment3 fillSegment(CRuVector3(center.m_x, ruMAXFLOATRANGE, center.m_z), CRuVector3(center.m_x, -ruMAXFLOATRANGE, center.m_z));

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(fillSegment, chunkKeyList);

		// Transform paint center to local coordinates
		CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = ruWORLD_CHUNKKEYX(chunkKeyList[i]);
			INT32 z = ruWORLD_CHUNKKEYZ(chunkKeyList[i]);

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				chunk->GetSplatIDMap()->FloodFillSplat(localCenter, radius, splatID, chunkKeyList[i], boost::bind(&CRuTerrain::FloodFill_GetSplatIDMap, this, _1, _2));
			}
		}
	}
}

CRuSplatIDMap *CRuTerrain::FloodFill_GetSplatIDMap(INT32 chunkKey, INT32 neighborID)
{
	INT32 x = ruWORLD_CHUNKKEYX(chunkKey);
	INT32 z = ruWORLD_CHUNKKEYZ(chunkKey);

	switch(neighborID)
	{
		case 0:
			--x;
			--z;
			break;

		case 1:
			--z;
			break;

		case 2:
			++x;
			--z;
			break;

		case 3:
			--x;
			break;

		case 4:
			++x;
			break;

		case 5:
			--x;
			++z;
			break;

		case 6:
			++z;
			break;

		case 7:
			++x;
			++z;
			break;
	}

	CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
	if(chunk)
	{
		return chunk->GetSplatIDMap();
	}

	return NULL;
}

void CRuTerrain::SmudgeSplat(const CRuVector3 &center, float radius, float smudgeProbability)
{
	if(m_chunkTree)
	{
		float effectPadding = 15.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		CRuVector3 paintCenter(center);
		paintCenter.y = 0.0f;

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				chunk->SmudgeSplat(paintCenter, radius, smudgeProbability);
			}
		}
	}
}

void CRuTerrain::ReplaceSplat(const CRuVector3 &center, float radius, UINT16 splatID)
{
	if(m_chunkTree)
	{
		float effectPadding = 15.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		CRuVector3 paintCenter(center);
		paintCenter.y = 0.0f;

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				chunk->ReplaceSplat(paintCenter, radius, splatID);
			}
		}
	}
}

void CRuTerrain::AttractSplat(const CRuVector3 &center, float radius, UINT16 attractedSplatID, float attractProbability)
{
	if(m_chunkTree)
	{
		float effectPadding = 15.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		CRuVector3 paintCenter(center);
		paintCenter.y = 0.0f;

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				chunk->AttractSplat(paintCenter, radius, attractedSplatID, attractProbability);
			}
		}
	}
}

UINT16 CRuTerrain::PickSplatID(const CRuVector3 &center)
{
	if(m_chunkTree)
	{
		float radius = 0.0f;
		float effectPadding = 0.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		CRuVector3 paintCenter(center);
		paintCenter.y = 0.0f;

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;


			CRuTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z);
			if(chunk)
			{
				return chunk->PickSplatID(paintCenter);
			}
		}
	}

	return 0;
}

void CRuTerrain::MojoPaint(const CRuVector3 &center, float radius, CRuFractal_Base *srcFractal, const CRuSplatIDMap_PaintSetting &paintSetting)
{
	if(m_chunkTree)
	{
		float effectPadding = 15.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		CRuVector3 paintCenter(center);
		paintCenter.y = 0.0f;

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;
			DWORD chunkKeyHash = RuGlobalHashGen().GetHashCode(chunkKeyList[i]);

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				CRuMojo_AttributeParcel<REAL> *refHF = NULL;

				// Look for the chunk's reference height field
				if(m_hfQueue->ContainsKey(chunkKeyList[i], chunkKeyHash))
				{
					refHF = m_hfQueue->Get(chunkKeyList[i], chunkKeyHash);
				}
				else
				{
					// The chunk does not have a reference height field, create one
					refHF = static_cast<CRuMojo_AttributeParcel<REAL> *>(chunk->GetHeightField()->Clone());

					// Try to free up space in the LRU queue for the new reference height field, if necessary
					CRuMojo_AttributeParcel<REAL> *reclaimedHF;
					while(m_hfQueue->Reclaim(chunkKeyList[i], chunkKeyHash, 1, reclaimedHF))
					{
						// Release the reclaimed height field
						ruSAFE_RELEASE(reclaimedHF);
					}

					// Insert reference height field into LRU queue
					m_hfQueue->Add(chunkKeyList[i], chunkKeyHash, refHF, 1);
				}

				// Execute mojo paint
				chunk->MojoPaint(paintCenter, radius, refHF, srcFractal, paintSetting);
			}
		}
	}
}

void CRuTerrain::PaintDoodad(const CRuVector3 &center, const CRuWorld_DoodadPaintSetting &paintSetting)
{
	if(m_chunkTree)
	{
		// Build collision segment for the paint brush
		CRuSegment3 effectSegment(CRuVector3(center.m_x, ruMAXFLOATRANGE, center.m_z), CRuVector3(center.m_x, -ruMAXFLOATRANGE, center.m_z));

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectSegment, chunkKeyList);

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				chunk->PaintDoodad(center, paintSetting, this);
			}
		}

		// Clear enumeration hash
		this->ClearEnumerationCache();
	}
}

void CRuTerrain::SprayDoodad(const CRuVector3 &center, REAL radius, INT32 numDoodads, const CRuWorld_DoodadPaintSetting &paintSetting)
{
	if(m_chunkTree)
	{
		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius, -FLT_MAX, center.z - radius, center.x + radius, FLT_MAX, center.z + radius);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				chunk->SprayDoodad(center, radius, numDoodads, paintSetting, this);
			}
		}

		// Clear enumeration hash
		this->ClearEnumerationCache();
	}
}

void CRuTerrain::DeleteDoodad(const CRuVector3 &center, REAL radius)
{
	if(m_chunkTree)
	{
		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius, -FLT_MAX, center.z - radius, center.x + radius, FLT_MAX, center.z + radius);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				chunk->DeleteDoodad(center, radius);
			}
		}

		// Clear enumeration hash
		this->ClearEnumerationCache();
	}
}

void CRuTerrain::RealignDoodad(const CRuVector3 &center, REAL radius)
{
	if(m_chunkTree)
	{
		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius, -FLT_MAX, center.z - radius, center.x + radius, FLT_MAX, center.z + radius);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				chunk->RealignDoodad(center, radius);
			}
		}

		// Clear enumeration hash
		this->ClearEnumerationCache();
	}
}

const CRuGUID &CRuTerrain::AddEntity(const char *resName, const CRuMatrix4x4 &transform, const CRuSphere &bounds)
{
	// Decompose orientation transform
	CRuVector3 scale, shear, rotation, translation;
	CRuQuaternion rotationQuat;

	transform.GetDecomposition(scale, shear, rotation, translation);
	rotationQuat.MakeEulerRotation(rotation);

	// Calculate local bounds
	CRuSphere localBounds;
	CRuMatrix4x4 invTransform = transform.GetInverse();
	invTransform.TransformPoint(bounds.Center(), localBounds.Center());
	localBounds.Radius() = bounds.Radius() * max(scale.m_x, max(scale.m_y, scale.m_z));

	// Create entity descriptor
	CRuWorld_EntityDescriptor *descriptor = ruNEW CRuWorld_EntityDescriptor();

	descriptor->SetResourceName(resName);
	descriptor->SetScale(scale);
	descriptor->SetRotation(rotationQuat);
	descriptor->SetTranslation(translation);
	descriptor->SetLocalBounds(localBounds);

	// Insert descriptor into entity container
	BOOL insertResult = m_objectContainer->InsertObject(descriptor, bounds);

	if(insertResult)
	{
		// Clear the enumeration cache
		this->ClearEnumerationCache();

		// Return descriptor GUID
		return descriptor->GetGUID();
	}

	// Print an error message
	char debugString[512];
	sprintf(debugString, "[ERROR] Object insertion into object container failed!\n");
	OutputDebugString(debugString);

	return CRuGUID::Invalid;
}

BOOL CRuTerrain::DetachEntityFromTerrainChunks(const CRuGUID &guid)
{
	return RemoveEntity(guid);
}

BOOL CRuTerrain::RemoveEntity(const CRuGUID &guid)
{
	BOOL asyncPending;
	CRuWorld_EntityDescriptor *descriptor = m_objectContainer->GetEntityDescriptor(guid, asyncPending);

	if(descriptor)
	{
		// Clear the enumeration cache
		this->ClearEnumerationCache();

		// Remove descriptor
		return m_objectContainer->DeleteObject(guid, descriptor->GetWorldBounds());
	}

	return FALSE;
}

BOOL CRuTerrain::UpdateEntity(const CRuGUID &guid, const CRuMatrix4x4 &transform, const CRuSphere &bounds)
{
	// Decompose orientation transform
	CRuVector3 scale, shear, rotation, translation;
	CRuQuaternion rotationQuat;

	transform.GetDecomposition(scale, shear, rotation, translation);
	rotationQuat.MakeEulerRotation(rotation);

	// Limit scale factor to some arbitrary value
	scale.m_x = max(0.001f, scale.m_x);
	scale.m_y = max(0.001f, scale.m_y);
	scale.m_z = max(0.001f, scale.m_z);

	// Calculate local bounds
	CRuSphere localBounds;
	CRuMatrix4x4 invTransform = transform.GetInverse();
	invTransform.TransformPoint(bounds.Center(), localBounds.Center());
	localBounds.Radius() = bounds.Radius() / max(scale.m_x, max(scale.m_y, scale.m_z));

	// Get the entity descriptor by GUID
	BOOL asyncPending;
	CRuWorld_EntityDescriptor *descriptor = m_objectContainer->GetEntityDescriptor(guid, asyncPending);

	if(descriptor)
	{
		// Determine if update is necessary
		if(	fabs(scale.m_x - descriptor->GetScale().m_x) < ruEPSILON &&
			fabs(scale.m_y - descriptor->GetScale().m_y) < ruEPSILON &&
			fabs(scale.m_z - descriptor->GetScale().m_z) < ruEPSILON &&
			fabs(rotationQuat.m_x - descriptor->GetRotation().m_x) < ruEPSILON &&
			fabs(rotationQuat.m_y - descriptor->GetRotation().m_y) < ruEPSILON &&
			fabs(rotationQuat.m_z - descriptor->GetRotation().m_z) < ruEPSILON &&
			fabs(rotationQuat.m_w - descriptor->GetRotation().m_w) < ruEPSILON &&
			fabs(translation.m_x - descriptor->GetTranslation().m_x) < ruEPSILON &&
			fabs(translation.m_y - descriptor->GetTranslation().m_y) < ruEPSILON &&
			fabs(translation.m_z - descriptor->GetTranslation().m_z) < ruEPSILON)
			return TRUE;

		// Make sure we have a local reference to the object so that it does not get deleted by the object container
		descriptor->AddRef();

		// Delete the object from container
		m_objectContainer->DeleteObject(descriptor->GetGUID(), descriptor->GetWorldBounds());

		// Set new positioning information
		descriptor->SetScale(scale);
		descriptor->SetRotation(rotationQuat);
		descriptor->SetTranslation(translation);
		descriptor->SetLocalBounds(localBounds);
		
		// Re-insert descriptor into entity container
		m_objectContainer->InsertObject(descriptor, bounds);

		// Clear the enumeration cache
		this->ClearEnumerationCache();

		return TRUE;
	}

	return FALSE;
}

CRuWorld_EntityDescriptor *CRuTerrain::GetEntityDescriptor(const CRuGUID &guid)
{
	// Retrieve descriptor from entity container
	BOOL asyncPending;
	return m_objectContainer->GetEntityDescriptor(guid, asyncPending);
}

CRuEntity *CRuTerrain::GetEntityInstance(const CRuGUID &guid)
{
	// Retrieve descriptor from entity container
	BOOL asyncPending;
	CRuWorld_EntityDescriptor *descriptor = m_objectContainer->GetEntityDescriptor(guid, asyncPending);
	if(descriptor)
	{
		CRuEntity *instancedEntity = m_objectContainer->GetEntityInstance(guid, asyncPending);
		if(instancedEntity)
		{
			return instancedEntity;
		}
	}

	return NULL;
}

BOOL CRuTerrain::MarkEntityDescriptorForUpdate(const CRuGUID &guid)
{
	return m_objectContainer->MarkObjectForUpdate(guid);
}

BOOL CRuTerrain::EnumerateVisibleEntityDescriptors(CRuArrayList<CRuWorld_EntityDescriptor *> &descriptorsOut)
{
	// Enumerate all visible objects in the container
	CRuArrayList<IRuObject *> objects;
	m_objectContainer->EnumerateVisibleObjects(objects);

	// Pick out CRuWorld_EntityDescriptor objects
	for(INT32 i = 0; i < objects.Count(); ++i)
	{
		if(objects[i]->GetType() == CRuWorld_EntityDescriptor::Type())
		{
			descriptorsOut.Add(static_cast<CRuWorld_EntityDescriptor *>(objects[i]));
		}
	}

	return TRUE;
}

void CRuTerrain::RemoveEntitiesByResourceName(const char *resourceName)
{
	// Enumerate all visible objects in the container
	CRuArrayList<IRuObject *> objects;
	m_objectContainer->EnumerateVisibleObjects(objects);

	// Pick out CRuWorld_EntityDescriptor objects
	for(INT32 i = 0; i < objects.Count(); ++i)
	{
		if(objects[i]->GetType() == CRuWorld_EntityDescriptor::Type())
		{
			CRuWorld_EntityDescriptor *descriptor = static_cast<CRuWorld_EntityDescriptor *>(objects[i]);

			if(strcmp(descriptor->GetResourceName(), resourceName) == 0)
			{
				// Remove descriptor
				m_objectContainer->DeleteObject(descriptor->GetGUID(), descriptor->GetWorldBounds());
			}
		}
	}

	m_objectContainer->ClearInstanceQueue();

	// Clear the enumeration cache
	this->ClearEnumerationCache();
}

void CRuTerrain::ReplaceEntitiesByResourceName(const char *oldResourceName, const char *newResourceName, const CRuSphere &localBounds)
{
	// Enumerate all visible objects in the container
	CRuArrayList<IRuObject *> objects;
	m_objectContainer->EnumerateVisibleObjects(objects);

	// Pick out CRuWorld_EntityDescriptor objects
	for(INT32 i = 0; i < objects.Count(); ++i)
	{
		if(objects[i]->GetType() == CRuWorld_EntityDescriptor::Type())
		{
			BOOL asyncPending;
			CRuWorld_EntityDescriptor *descriptor = static_cast<CRuWorld_EntityDescriptor *>(objects[i]);
			CRuEntity *entityInstance = m_objectContainer->GetEntityInstance(descriptor->GetGUID(), asyncPending);

			if(strcmp(descriptor->GetResourceName(), oldResourceName) == 0)
			{
				CRuMatrix4x4 worldTransform;
				worldTransform.SetToIdentity();
				worldTransform.Scale(descriptor->GetScale());
				worldTransform.Rotate(descriptor->GetRotation());
				worldTransform.Translate(descriptor->GetTranslation());

				CRuSphere worldBounds;
				worldTransform.TransformPoint(localBounds.Center(), worldBounds.Center());
				worldBounds.Radius() = localBounds.Radius() * max(descriptor->GetScale().m_x, max(descriptor->GetScale().m_y, descriptor->GetScale().m_z));

				// Add replaced entity
				this->AddEntity(newResourceName, entityInstance->GetLocalTransform(), worldBounds);

				// Remove old entity
				this->RemoveEntity(descriptor->GetGUID());
			}
		}
	}

	// Clear the enumeration cache
	this->ClearEnumerationCache();
}

BOOL CRuTerrain::CreateWaterBody(const CRuVector3 &center, REAL radius, REAL tessellationScale)
{
	// Generate new GUID for water body
	m_selectedWaterBodyGUID.Generate();

	if(m_chunkTree)
	{
		float effectPadding = 15.0f;

		// Build bounding box for the paint brush
		CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

		// Get a list intersecting chunks
		CRuArrayList<DWORD> chunkKeyList;
		m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

		for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
		{
			// Decode the chunk key
			INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
			INT32 z = chunkKeyList[i] & 0x0000FFFF;

			// Lock the chunk for change
			CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
			if(chunk)
			{
				// Paint mask
				chunk->PaintWaterSurface(m_selectedWaterBodyGUID, center, radius, TRUE, tessellationScale);
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTerrain::ExtendWaterBody(const CRuVector3 &center, REAL radius, REAL tessellationScale)
{
	if(m_selectedWaterBodyGUID != CRuGUID::Invalid)
	{
		if(m_chunkTree)
		{
			float effectPadding = 15.0f;

			// Build bounding box for the paint brush
			CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

			// Get a list intersecting chunks
			CRuArrayList<DWORD> chunkKeyList;
			m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

			for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
			{
				// Decode the chunk key
				INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
				INT32 z = chunkKeyList[i] & 0x0000FFFF;

				// Lock the chunk for change
				CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
				if(chunk)
				{
					// Paint mask
					chunk->PaintWaterSurface(m_selectedWaterBodyGUID, center, radius, TRUE, tessellationScale);
				}
			}

			// Stitch together water bodies
			StitchWaterBodies(chunkKeyList);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTerrain::ShrinkWaterBody(const CRuVector3 &center, REAL radius, REAL tessellationScale)
{
	if(m_selectedWaterBodyGUID != CRuGUID::Invalid)
	{
		if(m_chunkTree)
		{
			float effectPadding = 15.0f;

			// Build bounding box for the paint brush
			CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

			// Get a list intersecting chunks
			CRuArrayList<DWORD> chunkKeyList;
			m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

			for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
			{
				// Decode the chunk key
				INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
				INT32 z = chunkKeyList[i] & 0x0000FFFF;

				// Lock the chunk for change
				CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
				if(chunk)
				{
					// Paint mask
					chunk->PaintWaterSurface(m_selectedWaterBodyGUID, center, radius, FALSE, tessellationScale);
				}
			}

			// Stitch together water bodies
			StitchWaterBodies(chunkKeyList);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTerrain::PaintWaterBodyHeight(const CRuVector3 &center, float radius, float delta, RuFalloffFunction falloffFunction)
{
	if(m_selectedWaterBodyGUID != CRuGUID::Invalid)
	{
		if(m_chunkTree)
		{
			float effectPadding = 15.0f;

			// Build bounding box for the paint brush
			CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

			// Get a list intersecting chunks
			CRuArrayList<DWORD> chunkKeyList;
			m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

			for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
			{
				// Decode the chunk key
				INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
				INT32 z = chunkKeyList[i] & 0x0000FFFF;

				// Lock the chunk for change
				CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
				if(chunk)
				{
					// Paint mask
					chunk->PaintWaterHeight(m_selectedWaterBodyGUID, center, radius, delta, falloffFunction);
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTerrain::SetWaterBodyHeight(const CRuVector3 &center, float radius, float height, float brushStrength, RuFalloffFunction falloffFunction)
{
	if(m_selectedWaterBodyGUID != CRuGUID::Invalid)
	{
		if(m_chunkTree)
		{
			float effectPadding = 15.0f;

			// Build bounding box for the paint brush
			CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

			// Get a list intersecting chunks
			CRuArrayList<DWORD> chunkKeyList;
			m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

			for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
			{
				// Decode the chunk key
				INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
				INT32 z = chunkKeyList[i] & 0x0000FFFF;

				// Lock the chunk for change
				CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
				if(chunk)
				{
					// Paint mask
					chunk->SetWaterHeight(m_selectedWaterBodyGUID, center, radius, height, brushStrength, falloffFunction);
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTerrain::SmoothWaterBodyHeight(const CRuVector3 &center, float radius, float delta)
{
	if(m_selectedWaterBodyGUID != CRuGUID::Invalid)
	{
		if(m_chunkTree)
		{
			float effectPadding = 15.0f;

			// Build bounding box for the paint brush
			CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

			// Get a list intersecting chunks
			CRuArrayList<DWORD> chunkKeyList;
			m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

			for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
			{
				// Decode the chunk key
				INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
				INT32 z = chunkKeyList[i] & 0x0000FFFF;

				// Lock the chunk for change
				CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
				if(chunk)
				{
					// Paint mask
					chunk->SmoothWaterHeight(m_selectedWaterBodyGUID, center, radius, delta);
				}
			}

			// Stitch together water bodies
			StitchWaterBodies(chunkKeyList);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTerrain::PaintWaterBodyAlpha(const CRuVector3 &center, REAL radius, REAL alpha, REAL delta, RuFalloffFunction falloffFunction)
{
	if(m_selectedWaterBodyGUID != CRuGUID::Invalid)
	{
		if(m_chunkTree)
		{
			float effectPadding = 15.0f;

			// Build bounding box for the paint brush
			CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

			// Get a list intersecting chunks
			CRuArrayList<DWORD> chunkKeyList;
			m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

			for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
			{
				// Decode the chunk key
				INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
				INT32 z = chunkKeyList[i] & 0x0000FFFF;

				// Lock the chunk for change
				CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
				if(chunk)
				{
					// Paint mask
					chunk->PaintWaterBodyAlpha(m_selectedWaterBodyGUID, center, radius, alpha, delta, falloffFunction);
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTerrain::PaintWaterBodyColor(const CRuVector3 &center, REAL radius, RUCOLOR color, REAL delta, RuFalloffFunction falloffFunction)
{
	if(m_selectedWaterBodyGUID != CRuGUID::Invalid)
	{
		if(m_chunkTree)
		{
			float effectPadding = 15.0f;

			// Build bounding box for the paint brush
			CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

			// Get a list intersecting chunks
			CRuArrayList<DWORD> chunkKeyList;
			m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

			for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
			{
				// Decode the chunk key
				INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
				INT32 z = chunkKeyList[i] & 0x0000FFFF;

				// Lock the chunk for change
				CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
				if(chunk)
				{
					// Paint mask
					chunk->PaintWaterBodyColor(m_selectedWaterBodyGUID, center, radius, color, delta, falloffFunction);
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

RUCOLOR CRuTerrain::PickWaterBodyColor(const CRuVector3 &center)
{
	if(m_selectedWaterBodyGUID != CRuGUID::Invalid)
	{
		if(m_chunkTree)
		{
			float radius = 0.0f;
			float effectPadding = 0.0f;

			// Build bounding box for the paint brush
			CRuAABB effectBox(center.x - radius - effectPadding, -FLT_MAX, center.z - radius - effectPadding, center.x + radius + effectPadding, FLT_MAX, center.z + radius + effectPadding);

			// Get a list intersecting chunks
			CRuArrayList<DWORD> chunkKeyList;
			m_chunkTree->EnumerateChunks(effectBox, chunkKeyList);

			for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
			{
				// Decode the chunk key
				INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
				INT32 z = chunkKeyList[i] & 0x0000FFFF;

				// Load the chunk
				CRuTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z);
				if(chunk)
				{
					// Pick color
					return chunk->PickWaterBodyColor(m_selectedWaterBodyGUID, center);
				}
			}
		}

		return 0xFFFFFFFF;
	}

	return 0xFFFFFFFF;
}

BOOL CRuTerrain::StitchWaterBodies(CRuArrayList<DWORD> &chunkKeyList)
{
	for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
	{
		// Decode the chunk key
		INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
		INT32 z = chunkKeyList[i] & 0x0000FFFF;

		// Lock the chunk for change
		CRuTerrain_Chunk *chunk = m_dataSource->LockChunk(x, z);
		if(chunk)
		{
			// Stitch chunks together
			chunk->StitchWaterLayers(0, m_dataSource->LoadChunk(x - 1, z - 1));
			chunk->StitchWaterLayers(1, m_dataSource->LoadChunk(x, z - 1));
			chunk->StitchWaterLayers(2, m_dataSource->LoadChunk(x + 1, z - 1));
			chunk->StitchWaterLayers(3, m_dataSource->LoadChunk(x - 1, z));
			chunk->StitchWaterLayers(4, m_dataSource->LoadChunk(x + 1, z));
			chunk->StitchWaterLayers(5, m_dataSource->LoadChunk(x - 1, z + 1));
			chunk->StitchWaterLayers(6, m_dataSource->LoadChunk(x, z + 1));
			chunk->StitchWaterLayers(7, m_dataSource->LoadChunk(x + 1, z + 1));
		}
	}

	return TRUE;
}

BOOL CRuTerrain::SelectWaterBody(const CRuSegment3 &segment, CRuVector3 &intPtOut)
{
	REAL intDist;
	m_selectedWaterBodyGUID = FindWaterBodyIntersection(segment, intPtOut, intDist);

	return (m_selectedWaterBodyGUID != CRuGUID::Invalid);
}

CRuWorld_WaterBodyDescriptor *CRuTerrain::GetSelectedWaterBodyDescriptor()
{
	// If no water body is selected, return NULL
	if(m_selectedWaterBodyGUID == CRuGUID::Invalid)
		return NULL;

	// If the descriptor exists, return it
	if(m_waterBodyDescriptorMap->ContainsKey(m_selectedWaterBodyGUID, m_selectedWaterBodyGUID.HashCode()))
	{
		return m_waterBodyDescriptorMap->Get(m_selectedWaterBodyGUID, m_selectedWaterBodyGUID.HashCode());
	}

	// No descriptor exists for the water body, create one
	CRuWorld_WaterBodyDescriptor *descriptor = ruNEW CRuWorld_WaterBodyDescriptor();

	descriptor->m_guid = m_selectedWaterBodyGUID;

	// Append descriptor to list
	m_waterBodyDescriptors.Add(descriptor);

	// Map water body descriptor
	m_waterBodyDescriptorMap->Add(descriptor->m_guid, descriptor->m_guid.HashCode(), descriptor);

	return descriptor;
}

CRuWorld_WaterBodyDescriptor *CRuTerrain::GetWaterBodyDescriptorAtPoint(const CRuVector3 &point)
{
	CRuEntity *waterEntity = m_waterContainer->GetFirstChild();

	CRuWorld_WaterBodyDescriptor *nearestWaterBodyDescriptor = NULL;
	REAL nearestT = FLT_MAX;

	CRuCollisionQuery_Segment_Nearest colQuery;
	colQuery.m_colSegment = CRuSegment3(point, point + CRuVector3(0.0f, 10000.0f, 0.0f));
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

BOOL CRuTerrain::IsUnderWaterSurface(const CRuVector3 &position)
{
	// Construct query to find nearest collision with water or extent marker surfaces
	CRuCollisionQuery_Segment_Nearest query;
	query.m_colSegment.SetFromPoints(position, position + CRuVector3(0.0f, 10000.0f, 0.0f));
	query.m_inclusionMask = RuWorld::ruTRIFLAGS_WATER | RuWorld::ruTRIFLAGS_EXTENTMARKER;
	query.m_ignoreBackface = FALSE;

	// Perform collision query
	this->QueryCollision(&query);

	// If a surface is found, return based on its flag
	if(query.m_positiveHit)
	{
		return (query.m_collisionFlag & RuWorld::ruTRIFLAGS_WATER) ? TRUE : FALSE;
	}

	return FALSE;
}

CRuWorld_Trigger *CRuTerrain::FindIntersectingTrigger(const CRuSegment3 &segment, CRuVector3 &intPtOut, float &intDistOut)
{
	if(m_chunkTree)
	{
		REAL nearestDistance = FLT_MAX;
		CRuVector3 nearestPoint;
		CRuWorld_Trigger *nearestTrigger = NULL;

		// Build collision query
		CRuCollisionQuery_Segment_Nearest segQuery;
		CRuMatrix4x4 identityMatrix;
		segQuery.m_worldTransform = &identityMatrix;
		segQuery.m_invWorldTransform = &identityMatrix;
		segQuery.m_ignoreCollisionFlag = TRUE;
		segQuery.m_colSegment = segment;

		// Enumerate all triggers in the visible objects list
		CRuArrayList<IRuObject *> triggerList;
		m_objectContainer->EnumerateVisibleObjectsByType(triggerList, CRuWorld_Trigger::Type());

		// Query against all triggers
		for(INT32 i = 0; i < triggerList.Count(); ++i)
		{
			CRuWorld_Trigger *trigger = static_cast<CRuWorld_Trigger *>(triggerList[i]);

			if(!m_trigger_EnableFilter || trigger->GetZoneID() == m_trigger_FilterID)
			{
				segQuery.m_positiveHit = FALSE;
				segQuery.m_collisionT = FLT_MAX;
				trigger->QueryCollision(&segQuery);

				if(segQuery.m_positiveHit)
				{
					if(segQuery.m_collisionT < nearestDistance)
					{
						nearestDistance = segQuery.m_collisionT;
						nearestPoint = segQuery.m_collisionPoint;
						nearestTrigger = trigger;
					}
				}
			}
		}

		// Return query results
		if(nearestDistance < FLT_MAX)
		{
			intPtOut = nearestPoint;
			intDistOut = nearestDistance;

			return nearestTrigger;
		}
	}

	return NULL;
}

CRuWorld_Trigger *CRuTerrain::CreateTrigger(const CRuSegment3 &segment, BOOL snapToGrid, REAL initialSize, REAL initialHeight)
{
	// Setup collision query
	CRuCollisionQuery_Segment_Nearest colQuery;
	colQuery.m_colSegment = segment;
	colQuery.m_collisionFlag = 0x00A00000;				// Ignore water and foliage

	// Perform collision query
	this->QueryCollision(&colQuery);

	if(colQuery.m_positiveHit)
	{
		if(snapToGrid)
		{
			CRuAABB chunkBounds = GetDataSource()->GetChunkBounds(0, 0);
			REAL chunkSize = chunkBounds.Maxima().m_x - chunkBounds.Minima().m_x;

			if(colQuery.m_collisionPoint.m_x >= 0.0f)
			{
				colQuery.m_collisionPoint.m_x = colQuery.m_collisionPoint.m_x - (float)fmod(colQuery.m_collisionPoint.m_x, chunkSize) + chunkSize * 0.5f;
			}
			else
			{
				colQuery.m_collisionPoint.m_x = colQuery.m_collisionPoint.m_x - (float)fmod(colQuery.m_collisionPoint.m_x, chunkSize) - chunkSize * 0.5f;
			}

			if(colQuery.m_collisionPoint.m_z >= 0.0f)
			{
				colQuery.m_collisionPoint.m_z = colQuery.m_collisionPoint.m_z - (float)fmod(colQuery.m_collisionPoint.m_z, chunkSize) + chunkSize * 0.5f;
			}
			else
			{
				colQuery.m_collisionPoint.m_z = colQuery.m_collisionPoint.m_z - (float)fmod(colQuery.m_collisionPoint.m_z, chunkSize) - chunkSize * 0.5f;
			}
		}

		// Initialize the OBB as specified
		CRuOBB initialOBB;
		initialOBB.Set(CRuAABB(CRuVector3(colQuery.m_collisionPoint.m_x - initialSize, colQuery.m_collisionPoint.m_y - initialHeight, colQuery.m_collisionPoint.m_z - initialSize), CRuVector3(colQuery.m_collisionPoint.m_x + initialSize, colQuery.m_collisionPoint.m_y + initialHeight, colQuery.m_collisionPoint.m_z + initialSize)));

		// Create new trigger
		CRuWorld_Trigger *trigger = ruNEW CRuWorld_Trigger();
		trigger->SetTriggerBounds(initialOBB);

		// Insert descriptor into entity container
		m_objectContainer->InsertObject(trigger, trigger->GetWorldBounds());

		// Clear the enumeration cache
		this->ClearEnumerationCache();

		return trigger;
	}

	return NULL;
}

BOOL CRuTerrain::InsertTrigger(CRuWorld_Trigger *trigger)
{
	// Insert descriptor into entity container
	m_objectContainer->InsertObject(trigger, trigger->GetWorldBounds());

	// Clear the enumeration cache
	this->ClearEnumerationCache();

	return TRUE;
}

BOOL CRuTerrain::DeleteTrigger(CRuWorld_Trigger *trigger)
{
	// Clear the enumeration cache
	this->ClearEnumerationCache();

	// Remove trigger
	return m_objectContainer->DeleteObject(trigger->GetGUID(), trigger->GetWorldBounds());
}

BOOL CRuTerrain::StartTriggerUpdate(CRuWorld_Trigger *trigger)
{
	m_trigger_ActiveBounds = trigger->GetWorldBounds();

	return TRUE;
}

BOOL CRuTerrain::UpdateTrigger(CRuWorld_Trigger *trigger)
{
	return m_objectContainer->MarkObjectForUpdate(trigger->GetGUID());
}

BOOL CRuTerrain::EndTriggerUpdate(CRuWorld_Trigger *trigger)
{
	return m_objectContainer->UpdateObjectCellLinks(trigger, trigger->GetWorldBounds());
}

BOOL CRuTerrain::EnumerateTriggers(RuWorld::TriggerType triggerType, CRuArrayList<CRuWorld_Trigger *> &triggersOut)
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

CRuWorld_Trigger *CRuTerrain::GetApplicableTrigger(RuWorld::TriggerType triggerType, const CRuVector3 &point, BOOL limitRange)
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

INT32 CRuTerrain::GetZoneID(const CRuVector3 &point)
{
	CRuWorld_Trigger *trigger = GetApplicableTrigger(RuWorld::ruTRIGGERTYPE_ZONE, point, FALSE);

	if(trigger)
	{
		return trigger->GetZoneID();
	}

	return 0;
}

RuWorld::ZoneType CRuTerrain::GetZoneType(const CRuVector3 &point)
{
	CRuWorld_Trigger *trigger = GetApplicableTrigger(RuWorld::ruTRIGGERTYPE_ZONETYPE, point, TRUE);

	if(trigger)
	{
		return trigger->GetZoneType();
	}

	return RuWorld::ruZONETYPE_OUTDOOR;
}

CRuWorld_ZoneDescriptor *CRuTerrain::GetZoneDescriptor(const CRuVector3 &point)
{
	INT32 zoneID = GetZoneID(point);

	return m_zoneDescriptorContainer->GetZoneDescriptor(zoneID);
}

CRuWorld_ZoneDescriptor *CRuTerrain::GetZoneDescriptorByID(INT32 zoneID)
{
	return m_zoneDescriptorContainer->GetZoneDescriptor(zoneID);
}

BOOL CRuTerrain::IsSubZoneOf(INT32 subZoneID, INT32 parentZoneID)
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

BOOL CRuTerrain::MarkIntersectingChunk(const CRuSegment3 &segment)
{
	if(m_chunkTree)
	{
		// Perform collision query against terrain to find nearest pick point
		CRuCollisionQuery_Segment_Nearest colQuery;

		colQuery.m_colSegment = segment;
		colQuery.m_ignoreCollisionFlag = FALSE;
		colQuery.m_collisionFlag = 0x00A00000;

		this->QueryCollision(&colQuery);

		if(colQuery.m_positiveHit)
		{
			CRuSegment3 vertPickSegment;
			vertPickSegment.Origin() = colQuery.m_collisionPoint;
			vertPickSegment.Direction() = CRuVector3(0.0f, -100.0f, 0.0f);

			DWORD chunkKey;
			CRuAABB chunkBounds;

			if(m_chunkTree->PickNearestChunk(vertPickSegment, chunkKey, chunkBounds))
			{
				// Decode the chunk key
				INT32 x = (chunkKey >> 16) & 0x0000FFFF;
				INT32 z = chunkKey & 0x0000FFFF;

				// Mark the chunk
				CRuTerrain_Chunk *markedChunk = m_dataSource->MarkChunk(x, z);
			}
		}
	}

	return TRUE;
}

CRuWorld_ObjectContainer *CRuTerrain::GetObjectContainer()
{
	return m_objectContainer;
}

CRuWorld_ObjectManager *CRuTerrain::GetObjectManager()
{
	return m_objectManager;
}

CRuWorld_DoodadContainer *CRuTerrain::GetDoodadContainer()
{
	return m_doodadContainer;
}

CRuWorld_DoodadManager *CRuTerrain::GetDoodadManager()
{
	return m_doodadManager;
}

CRuWorld_ZoneDescriptorContainer *CRuTerrain::GetZoneDescriptorContainer()
{
	return m_zoneDescriptorContainer;
}

IRuPathHierarchy_Base *CRuTerrain::GetPathHierarchy()
{
	return m_pathHierarchy;
}

CRuTextureList *CRuTerrain::GetTextureList()
{
	return m_textureList;
}

BOOL CRuTerrain::Save()
{
	if(m_dataSource)
	{
		// Flush all data to data source
		BOOL flushOK = m_dataSource->FlushLockedChunks();
		BOOL saveTexturesOK = m_dataSource->SaveTextureList(m_textureList);
		BOOL editingEnvironmentFlushOK = m_dataSource->FlushEditingEnvironment();

		BOOL entityFlushOK = m_objectContainer->SaveChanges();
		BOOL doodadFlushOK = m_doodadContainer->SaveChanges();

		BOOL zoneDescriptorFlushOK = m_zoneDescriptorContainer->Save();

		BOOL pathHierarchyFlushOK = m_pathHierarchy->Save();

		// Flush water body descriptor changes to the data source
		for(INT32 i = 0; i < m_waterBodyDescriptors.Count(); ++i)
		{
			if(m_waterBodyDescriptors[i]->m_isDirty)
			{
				// Save descriptor to data source
				m_dataSource->SaveWaterBodyDescriptor(m_waterBodyDescriptors[i]);

				// Mark descriptor as clean
				m_waterBodyDescriptors[i]->m_isDirty = FALSE;
			}
		}

		// Clear the enumeration cache
		this->ClearEnumerationCache();

		// Clear visible entities list, since cached terrain chunks may have changed
		m_visibleChunks.Clear();
		m_visibleGrass.Clear();
		m_visibleEntities.Clear();
		m_visibleWaterEntities.Clear();

		return (flushOK && saveTexturesOK && entityFlushOK && doodadFlushOK && editingEnvironmentFlushOK);
	}

	return FALSE;
}

BOOL CRuTerrain::CopyTerrainSubsetToDataStore(CRuDataStore_Base *dataStore, const CRuAABB &subsetBounds)
{
	// Enumerate the list of chunks in the subset
	CRuArrayList<DWORD> chunkKeyList;
	m_chunkTree->EnumerateChunks(subsetBounds, chunkKeyList);

	// Determine actual containing bounding box for the enumerated chunks
	CRuAABB actualSubsetBounds;

	INT32 xMin = 65535, xMax = 0;
	INT32 zMin = 65535, zMax = 0;

	for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
	{
		INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
		INT32 z = chunkKeyList[i]& 0x0000FFFF;

		if(i == 0)
			actualSubsetBounds = m_dataSource->GetChunkBounds(x, z);
		else
			actualSubsetBounds.Merge(m_dataSource->GetChunkBounds(x, z));

		xMin = min(xMin, x);
		xMax = max(xMax, x);

		zMin = min(zMin, z);
		zMax = max(zMax, z);
	}

	CRuVector3 subsetOrigin(0.0f, 0.0f, 0.0f);
	INT32 numXChunks = xMax - xMin + 1;
	INT32 numZChunks = zMax - zMin + 1;

	CRuVector3 positionOffset = subsetOrigin - actualSubsetBounds.Minima();
	positionOffset.m_y = 0.0f;

	// Initialize new data source
	CRuTerrain_DataSource_Remote *outputDataSource = ruNEW CRuTerrain_DataSource_Remote();
	outputDataSource->Initialize(dataStore, subsetOrigin, m_dataSource->GetGridSize(), m_dataSource->GetChunkDimension(), numXChunks, numZChunks, m_dataSource->GetLODLevels(), m_dataSource->GetSplatResolution());

	// Copy terrain texture list
	outputDataSource->SaveTextureList(this->m_textureList, TRUE);

	REAL chunkSize = m_dataSource->GetGridSize() * m_dataSource->GetChunkDimension();

	CRuArrayList<CRuGUID> usedWaterBodyDescriptors;
	CRuArrayList<CRuGUID> usedObjectDescriptors;
	CRuArrayList<CRuGUID> usedDoodadDescriptors;

	// Copy terrain chunks
	for(INT32 i = 0; i < chunkKeyList.Count(); ++i)
	{
		INT32 x = (chunkKeyList[i] >> 16) & 0x0000FFFF;
		INT32 z = chunkKeyList[i]& 0x0000FFFF;

		CRuTerrain_Chunk *chunk = m_dataSource->LoadChunk(x, z);

		if(chunk)
		{
			CRuVector3 chunkTranslation(subsetOrigin.m_x + (x - xMin) * chunkSize, subsetOrigin.m_y, subsetOrigin.m_z + (z - zMin) * chunkSize);

			CRuTerrain_Chunk *clonedChunk = static_cast<CRuTerrain_Chunk *>(chunk->Clone());
			clonedChunk->SetTranslation(chunkTranslation);
			clonedChunk->CalculateBounds();
			clonedChunk->Update(0.0f);

			// Process water layers
			for(INT32 j = 0; j < clonedChunk->GetNumWaterLayers(); ++j)
			{
				// Re-position water layer
				CRuWorld_Water *waterLayer = clonedChunk->GetWaterLayerByIndex(j);
				CRuVector3 newWaterLayerTranslation(chunkTranslation.m_x, waterLayer->GetTranslation().m_y, chunkTranslation.m_z);
				waterLayer->SetTranslation(newWaterLayerTranslation);

				// Add water body GUID to the list of used descriptors
				usedWaterBodyDescriptors.Add(waterLayer->GetWaterBodyGUID());
			}
/*
			// Store list of object descriptor guids used by the chunk
			// Iterate through entity descriptors
			for(INT32 j = 0; j < clonedChunk->GetNumEntityDescriptorGUIDs(); ++j)
			{
				usedObjectDescriptors.Add(clonedChunk->GetEntityDescriptorGUID(j));
			}
*/
			// Store list of doodad descriptor guids used by the chunk
			// Iterate through doodad lists
			for(INT32 j = 0; j < chunk->GetNumDoodadClusters(); ++j)
			{
				CRuWorld_DoodadCluster *doodadCluster = clonedChunk->GetDoodadCluster(j);

				if(doodadCluster)
				{
					// Offset doodad positions
					for(INT32 k = 0; k < doodadCluster->m_doodads.Count(); ++k)
					{
						doodadCluster->m_doodads[k].m_translation += positionOffset;
					}

					usedDoodadDescriptors.Add(doodadCluster->m_guid);
				}
			}

			outputDataSource->SaveChunk(x - xMin, z - zMin, clonedChunk);
			ruSAFE_RELEASE(clonedChunk);
		}
	}

	// Copy water body descriptors
	for(INT32 i = 0; i < usedWaterBodyDescriptors.Count(); ++i)
	{
		CRuWorld_WaterBodyDescriptor *descriptor = NULL;
		if(m_waterBodyDescriptorMap->Get(usedWaterBodyDescriptors[i], usedWaterBodyDescriptors[i].HashCode(), descriptor))
		{
			outputDataSource->SaveWaterBodyDescriptor(descriptor);
		}
	}

	// Copy object descriptors
	CRuWorld_ObjectContainer *outputObjectContainer = ruNEW CRuWorld_ObjectContainer(outputDataSource->GetTerrainBounds(), outputDataSource->GetGridSize() * outputDataSource->GetChunkDimension() * ruWORLD_OBJECTCELLMULTIPLIER);
	outputObjectContainer->Attach(dataStore);

	for(INT32 i = 0; i < usedObjectDescriptors.Count(); ++i)
	{
		// Get entity descriptor from the source
		BOOL asyncPending = FALSE;
		CRuWorld_EntityDescriptor *entityDescriptor = NULL;
		do 
		{
			entityDescriptor = m_objectContainer->GetEntityDescriptor(usedObjectDescriptors[i], asyncPending);
		} while(asyncPending);

		// Store entity descriptor to destination
		if(entityDescriptor)
		{
			// Clone the entity descriptor and adjust its position
			CRuWorld_EntityDescriptor *clonedDescriptor = static_cast<CRuWorld_EntityDescriptor *>(entityDescriptor->Clone());
			clonedDescriptor->SetTranslation(clonedDescriptor->GetTranslation() + positionOffset);

			// Store
			outputObjectContainer->InsertObject(clonedDescriptor, clonedDescriptor->GetWorldBounds());
		}
	}

	// Copy doodad descriptors
	CRuWorld_DoodadContainer *outputDoodadContainer = ruNEW CRuWorld_DoodadContainer();
	outputDoodadContainer->Attach(dataStore, FALSE);

	for(INT32 i = 0; i < usedDoodadDescriptors.Count(); ++i)
	{
		// Get doodad template from the source
		BOOL asyncPending = FALSE;
		CRuDoodad_Template *doodadTemplate = NULL;
		do 
		{
			doodadTemplate = m_doodadContainer->GetDoodadTemplate(usedDoodadDescriptors[i], asyncPending);
		} while(asyncPending);

		// Store doodad template to destination
		if(doodadTemplate)
		{
			// Clone the doodad template
			CRuDoodad_Template *clonedTemplate = static_cast<CRuDoodad_Template *>(doodadTemplate->Clone());

			// Store
			outputDoodadContainer->InsertDoodadTemplate(clonedTemplate);
		}
	}

	// Flush all data to data source
	outputDataSource->FlushLockedChunks();
	outputObjectContainer->SaveChanges();
	outputDoodadContainer->SaveChanges();

	ruSAFE_RELEASE(outputDataSource);
	ruSAFE_RELEASE(outputObjectContainer);
	ruSAFE_RELEASE(outputDoodadContainer);

	return TRUE;
}

void CRuTerrain::GetDoodadEnumeration(CRuArrayList<INT32> **enumeratedKeys, CRuArrayList<CRuWorld_DoodadCluster *> **enumeratedClusters)
{
	*enumeratedKeys = &m_enumeratedDoodadKeys;
	*enumeratedClusters = &m_enumeratedDoodadClusters;
}

BOOL CRuTerrain::ClearEnumerationCache()
{
	// Reset view volume
	m_viewVolume.Set(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	// Clear internal enumerations
	m_enumeratedChunks.Clear();
	m_enumeratedDoodadKeys.Clear();
	m_enumeratedDoodadClusters.Clear();

	// Clear external enumerations
	m_objectManager->ClearEnumerationCache();
	m_doodadManager->ClearEnumerationCache();

	return TRUE;
}

BOOL CRuTerrain::ClearHeightFieldQueue()
{
	if(m_hfQueue)
	{
		m_hfQueue->ForAllElements(ClearHeightFieldQueueCallback, this);
		m_hfQueue->Clear();
	}

	return TRUE;
}

BOOL CRuTerrain::ClearHeightFieldQueueCallback(const void *key, const void *data, void *userData)
{
	CRuMojo_AttributeParcel<REAL> *hf = *((CRuMojo_AttributeParcel<REAL> **) data);

	ruSAFE_RELEASE(hf);

	return TRUE;
}

BOOL CRuTerrain::EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting)
{
	return ApplyEngineSettings();
}

BOOL CRuTerrain::ApplyEngineSettings()
{
	RuPropertyType propertyValue;
	BOOL rebuildWaterMaterials = FALSE;

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

#pragma managed
