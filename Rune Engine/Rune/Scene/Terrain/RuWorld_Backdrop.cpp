/*!
	@file RuEntity_Backdrop.cpp

	Copyright (c) 2004-2007 All rights reserved
*/

#include "../../Scene/Terrain/RuWorld_Backdrop.h"
#include "../../Scene/Terrain/RuEntityTerrain_Common.h"
#include "../../Helper/RuHelper_StreamIO.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuWorld_Backdrop, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_Backdrop, "CRuWorld_Backdrop", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuWorld_Backdrop, 0x2B9C200B, 0x11E14d8d, 0x99722024, 0x0B1ECE89)

// ************************************************************************************************************************************************************

CRuWorld_Backdrop::CRuWorld_Backdrop()
:	m_mesh(NULL),
	m_material(NULL),
	m_currentLOD(0)
{
	// Initialize material
	m_material = ruNEW CRuMaterial();
	m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_TERRAIN_PACKED_BACKDROP"));
}

CRuWorld_Backdrop::~CRuWorld_Backdrop()
{
	ruSAFE_RELEASE(m_mesh);
	ruSAFE_RELEASE(m_material);

	for(INT32 i = 0; i < m_lodMeshes.Count(); ++i)
	{
		ruSAFE_RELEASE(m_lodMeshes[i]);
	}
	m_lodMeshes.Clear();
}

INT32 CRuWorld_Backdrop::GetBackdropKey()
{
	return m_backdropKey;
}

void CRuWorld_Backdrop::SetBackdropKey(INT32 backdropKey)
{
	m_backdropKey = backdropKey;
}

BOOL CRuWorld_Backdrop::Initialize(const CRuVector3 &origin, REAL gridSpacing, INT32 dimension, INT32 lodLevels, REAL *heightField, IRuMeshTopology *meshTopology)
{
	// Store backdrop parameters
	m_origin = origin;
	m_gridSpacing = gridSpacing;
	m_dimension = dimension;
	m_lodLevels = lodLevels;

	// Setup bounding box for the backdrop
	REAL backdropSize = gridSpacing * dimension;
	m_boundingBox.Set(origin.m_x, 0.0f, origin.m_z, origin.m_x + backdropSize, 0.0f, origin.m_z + backdropSize);

	// Number of vertices = (dimension + 1)^2 + (dimension + 1) * 4
	INT32 numVertices = (dimension + 1) * (dimension + 1) + (dimension + 1) * 4;

	// Calculate number of indices
	INT32 numIndices = 0;
	for(INT32 lod = 0, curDimension = dimension; lod < lodLevels; ++lod, curDimension /= 2)
	{
		// Main body
		numIndices += ((curDimension * curDimension * 2) * 3);

		// Skirt
		numIndices += (curDimension * 2) * 4 * 3;
	}

	// Construct mesh object
	m_mesh = ruNEW CRuMesh();
	m_mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, numVertices, numIndices / 3, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE | ruVERTEXFORMAT_BLENDINDICES);

	// Setup mesh vertices
	REAL minY = FLT_MAX, maxY = -FLT_MAX;

	CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();
	DWORD *diffuse = m_mesh->GetMorphTarget(0)->GetDiffuse();

	for(INT32 z = 0, vertexIndex = 0, i = 0; z <= dimension; ++z)
	{
		float xF = 0.0f;
		float zF = gridSpacing * z;

		for(INT32 x = 0; x <= dimension; ++x, ++vertexIndex, xF += gridSpacing)
		{
			// Set XZ position
			vertices[vertexIndex].m_x = xF;
			vertices[vertexIndex].m_y = heightField[x + z * (dimension + 1)];
			vertices[vertexIndex].m_z = zF;
			diffuse[vertexIndex] = 0xFFFFFFFF;

			// Keep track of min-max of Y
			minY = min(minY, vertices[vertexIndex].m_y);
			maxY = max(maxY, vertices[vertexIndex].m_y);
		}
	}

	// Store y min-max
	m_boundingBox.Minima().m_y = minY;
	m_boundingBox.Maxima().m_y = maxY;

	INT32 topSkirtVertexArrayOffset = (dimension + 1) * (dimension + 1);
	INT32 bottomSkirtVertexArrayOffset = topSkirtVertexArrayOffset + (dimension + 1);
	INT32 leftSkirtVertexArrayOffset = bottomSkirtVertexArrayOffset + (dimension + 1);
	INT32 rightSkirtVertexArrayOffset = leftSkirtVertexArrayOffset + (dimension + 1);

	// Duplicate skirt vertices (top row)
	for(INT32 i = 0, destOffset = topSkirtVertexArrayOffset, srcOffset = 0; i <= dimension; ++i, ++destOffset, ++srcOffset)
	{
		vertices[destOffset].Set(vertices[srcOffset].x, vertices[srcOffset].y - 1.0f, vertices[srcOffset].z);
		diffuse[destOffset] = diffuse[srcOffset];
	}

	// Duplicate skirt vertices (bottom row)
	for(INT32 i = 0, destOffset = bottomSkirtVertexArrayOffset, srcOffset = dimension * (dimension + 1); i <= dimension; ++i, ++destOffset, ++srcOffset)
	{
		vertices[destOffset].Set(vertices[srcOffset].x, vertices[srcOffset].y - 1.0f, vertices[srcOffset].z);
		diffuse[destOffset] = diffuse[srcOffset];
	}

	// Duplicate skirt vertices (left row)
	for(INT32 i = 0, destOffset = leftSkirtVertexArrayOffset, srcOffset = 0; i <= dimension; ++i, ++destOffset, srcOffset += (dimension + 1))
	{
		vertices[destOffset].Set(vertices[srcOffset].x, vertices[srcOffset].y - 1.0f, vertices[srcOffset].z);
		diffuse[destOffset] = diffuse[srcOffset];
	}

	// Duplicate skirt vertices (right row)
	for(INT32 i = 0, destOffset = rightSkirtVertexArrayOffset, srcOffset = dimension; i <= dimension; ++i, ++destOffset, srcOffset += (dimension + 1))
	{
		vertices[destOffset].Set(vertices[srcOffset].x, vertices[srcOffset].y - 1.0f, vertices[srcOffset].z);
		diffuse[destOffset] = diffuse[srcOffset];
	}

	// Generate compressed vertices
	{
		CRuVector3 *vertices = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0))->GetPosition();
		UINT16 *compressedVertices = m_mesh->GetMorphTarget(0)->GetBlendIndex();

		REAL chunk_Y_Base = m_boundingBox.Minima().m_y;
		REAL chunk_Y_Increment = (m_boundingBox.Maxima().m_y - m_boundingBox.Minima().m_y) / 65025;

		for(INT32 vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex, compressedVertices += 4)
		{
			UINT16 discreteY = (UINT16) ((vertices[vertexIndex].m_y - chunk_Y_Base) / chunk_Y_Increment);

			compressedVertices[0] = (BYTE) (vertices[vertexIndex].m_x / gridSpacing);
			compressedVertices[1] = (BYTE) (vertices[vertexIndex].m_z / gridSpacing);
			compressedVertices[2] = discreteY / 255;
			compressedVertices[3] = discreteY % 255;
		}
	}

	// Initialize LOD proxy meshes
	UINT16 *indices = m_mesh->GetIndices();
	INT32 indexCount = 0;

	INT32 batchNumIndices;
	INT32 batchOffset;
	INT32 batchIndex;

	for(INT32 lod = 0, gridDimension = m_dimension, stride = 1, pitch = m_dimension + 1; lod < m_lodLevels; ++lod, gridDimension /= 2, stride *= 2, pitch *= 2)
	{
		// Build main body indices
		batchNumIndices = RuTerrainUtility_BuildGridIndicesEx(gridDimension, stride, pitch, 0, NULL, &indices[indexCount]);

		// Build skirt indices
		batchNumIndices += RuTerrainUtility_BuildIndices_Skirt(gridDimension, stride, pitch, m_dimension + 1, NULL, &indices[indexCount + batchNumIndices]);
		batchOffset = indexCount;
		indexCount += batchNumIndices;

		batchIndex = m_mesh->AddBatch(batchOffset / 3, batchNumIndices / 3);

		// Setup the LOD proxy mesh
		if(lod >= m_lodMeshes.Count())
		{
			m_lodMeshes.Add(ruNEW CRuProxyMesh());
		}

		m_lodMeshes[lod]->SetProxiedMesh(m_mesh, batchIndex);
	}

	if(meshTopology)
	{
		m_mesh->OverrideMeshTopology(meshTopology);
	}

	// Set mesh bounds (temporary?)
	m_mesh->CalculateBounds();
	this->SetLocalEntityBounds(m_mesh->GetBounds());

	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_CHUNK_SPACING, RuCastFloatToINT64(gridSpacing));
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_CHUNK_Y_BASE, RuCastFloatToINT64(m_boundingBox.Minima().m_y));
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_CHUNK_Y_INCREMENT, RuCastFloatToINT64((m_boundingBox.Maxima().m_y - m_boundingBox.Minima().m_y) / 65025));

	// Translate backdrop to correct location
	this->ResetAllTransforms();
	this->Translate(m_origin);

	return TRUE;
}

INT32 CRuWorld_Backdrop::GetLOD()
{
	return m_currentLOD;
}

void CRuWorld_Backdrop::SetLOD(INT32 lod)
{
	m_currentLOD = lod;
}

const CRuAABB &CRuWorld_Backdrop::GetBoundingBox() const
{
	return m_boundingBox;
}

BOOL CRuWorld_Backdrop::SerializeFrom(IRuStream *stream, IRuMeshTopology *meshTopology)
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
	IRuEntity_Renderable::SerializeFrom(stream);

	// Read backdrop key
	stream->Read(&m_backdropKey, sizeof(INT32));

	// Read backdrop origin
	::SerializeFrom(stream, &m_origin);

	// Read backdrop grid spacing
	stream->Read(&m_gridSpacing, sizeof(REAL));

	// Read backdrop dimension
	stream->Read(&m_dimension, sizeof(INT32));

	// Read backdrop lod levels
	stream->Read(&m_lodLevels, sizeof(INT32));

	// Read height field values
	REAL *heightField = ruNEW REAL [(m_dimension + 1) * (m_dimension + 1)];
	stream->Read(heightField, sizeof(REAL) * (m_dimension + 1) * (m_dimension + 1));

	// Initialize mesh
	this->Initialize(m_origin, m_gridSpacing, m_dimension, m_lodLevels, heightField, meshTopology);

	// Release height field values
	delete[] heightField;

	return TRUE;
}

BOOL CRuWorld_Backdrop::SerializeFrom(IRuStream *stream)
{
	return SerializeFrom(stream, NULL);
}

BOOL CRuWorld_Backdrop::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Renderable::SerializeTo(stream);

	// Write backdrop key
	stream->Write(&m_backdropKey, sizeof(INT32));

	// Write backdrop origin
	::SerializeTo(stream, &m_origin);

	// Write backdrop grid spacing
	stream->Write(&m_gridSpacing, sizeof(REAL));

	// Write backdrop dimension
	stream->Write(&m_dimension, sizeof(INT32));

	// Write backdrop lod levels
	stream->Write(&m_lodLevels, sizeof(INT32));

	// Write height field values
	REAL *heightField = ruNEW REAL [(m_dimension + 1) * (m_dimension + 1)];

	if(m_mesh)
	{
		CRuVector3 *vertices = m_mesh->GetMorphTarget(0)->GetPosition();

		for(INT32 i = 0, vertexCount = (m_dimension + 1) * (m_dimension + 1); i < vertexCount; ++i)
		{
			heightField[i] = vertices[i].m_y;
		}
	}
	else
	{
		memset(heightField, 0, sizeof(REAL) * (m_dimension + 1) * (m_dimension + 1));
	}

	stream->Write(heightField, sizeof(REAL) * (m_dimension + 1) * (m_dimension + 1));
	delete[] heightField;

	return TRUE;
}

INT32 CRuWorld_Backdrop::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuEntity_Renderable::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	footprint += sizeof(m_backdropKey);

	footprint += sizeof(m_origin);
	footprint += sizeof(m_gridSpacing);
	footprint += sizeof(m_dimension);
	footprint += sizeof(m_lodLevels);

	footprint += sizeof(m_mesh) + (m_mesh ? m_mesh->GetMemoryFootprint(enumHash) : 0);
	footprint += sizeof(m_material) + (m_material ? m_material->GetMemoryFootprint(enumHash) : 0);

	footprint += m_lodMeshes.GetMemoryFootprint();
	for(INT32 i = 0; i < m_lodMeshes.Count(); ++i)
	{
		if(m_lodMeshes[i])
		{
			footprint += m_lodMeshes[i]->GetMemoryFootprint(enumHash);
		}
	}
	footprint += sizeof(m_currentLOD);

	footprint += sizeof(m_boundingBox);

	return footprint;
}

INT32 CRuWorld_Backdrop::GetNumMeshes() const
{
	return 1;
}

BOOL CRuWorld_Backdrop::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	*meshOut = m_lodMeshes[m_currentLOD];
	*materialOut = m_material;

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
