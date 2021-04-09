#include "../../Engine/Geometry/RuMeshBase.h"
#include "../../NvTriStrip/NvTriStrip.h"
#include <set>

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(IRuMorphTarget, IRuObject)
ruCLASSGUID_DEFINE(IRuMorphTarget, 0x01AB7C30, 0x295A4f41, 0x8E5046BC, 0xA9FE2880)

ruRTTI_DEFINE_SUBCLASS(IRuMeshTopology, IRuObject)
ruCLASSGUID_DEFINE(IRuMeshTopology, 0x0975D518, 0xDA8D4df1, 0x877847C7, 0x1135713A)

ruRTTI_DEFINE_SUBCLASS(IRuMesh, IRuObject)
ruCLASSGUID_DEFINE(IRuMesh, 0x88279D3D, 0x5A234bc6, 0x81997A96, 0xA74A06FF)

ruRTTI_DEFINE_SUBCLASS(CRuMorphTarget, IRuMorphTarget)
ruHEAP_DEFINE_SUBCLASS(CRuMorphTarget, "CRuMorphTarget", "IRuMorphTarget")
ruCLASSGUID_DEFINE(CRuMorphTarget, 0x3B2A099C, 0xAF1E49ac, 0x893B5C66, 0x7201CE3B)

ruRTTI_DEFINE_SUBCLASS(CRuMeshTopology, IRuMeshTopology)
ruHEAP_DEFINE_SUBCLASS(CRuMeshTopology, "CRuMeshTopology", "IRuMeshTopology")
ruCLASSGUID_DEFINE(CRuMeshTopology, 0xB15A8F0B, 0x16E046da, 0xB7C0FA6E, 0x2BBF5D21)

ruRTTI_DEFINE_SUBCLASS(CRuMesh, IRuMesh)
ruHEAP_DEFINE_SUBCLASS(CRuMesh, "CRuMesh", "IRuMesh")
ruCLASSGUID_DEFINE(CRuMesh, 0xA8C66048, 0x2D234147, 0xAB0D3AC5, 0xD4812700)

ruRTTI_DEFINE_SUBCLASS(CRuSkinnableMesh, IRuMesh)
ruHEAP_DEFINE_SUBCLASS(CRuSkinnableMesh, "CRuSkinnableMesh", "IRuMesh")
ruCLASSGUID_DEFINE(CRuSkinnableMesh, 0xF9D6E89C, 0x0CB94eb2, 0xA7EB32FD, 0x829825AF)

ruRTTI_DEFINE_SUBCLASS(CRuDynamicMesh, CRuMesh)
ruHEAP_DEFINE_SUBCLASS(CRuDynamicMesh, "CRuDynamicMesh", "CRuMesh")
ruCLASSGUID_DEFINE(CRuDynamicMesh, 0x1F3E3EB3, 0x10B24ab1, 0x852E8A36, 0x51F8ED5F)

ruRTTI_DEFINE_SUBCLASS(CRuMorphingMesh, CRuMesh)
ruHEAP_DEFINE_SUBCLASS(CRuMorphingMesh, "CRuMorphingMesh", "CRuMesh")
ruCLASSGUID_DEFINE(CRuMorphingMesh, 0x617ED9C2, 0xD8864a79, 0x808A28AB, 0x4D54EE43)

ruRTTI_DEFINE_SUBCLASS(CRuProxyMesh, IRuMesh)
ruHEAP_DEFINE_SUBCLASS(CRuProxyMesh, "CRuProxyMesh", "IRuMesh")
ruCLASSGUID_DEFINE(CRuProxyMesh, 0x7B95A9D1, 0xE7814aac, 0xBA2C8D1C, 0x83CEEECF)

ruRTTI_DEFINE_SUBCLASS(CRuSkinMap, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuSkinMap, "CRuSkinMap", "IRuObject")
ruCLASSGUID_DEFINE(CRuSkinMap, 0xD2E448E3, 0x0D874981, 0xA4E900FB, 0xB310D79A)

// ************************************************************************************************************************************************************

CRuMorphTarget::CRuMorphTarget()
:	m_iterationNumber(0),
	m_vertexFormat(ruVERTEXFORMAT_POSITION),
	m_vertexCount(0),
	m_position(NULL),
	m_normal(NULL),
	m_diffuse(NULL),
	m_specular(NULL),
	m_blendWeight(NULL),
	m_blendIndex(NULL)
{
}

CRuMorphTarget::~CRuMorphTarget()
{
	delete[] m_position;
	delete[] m_normal;
	delete[] m_diffuse;
	delete[] m_specular;
	delete[] m_blendWeight;
	delete[] m_blendIndex;

	for(INT32 i = 0; i < m_textureCoordinates.Count(); ++i)
	{
		delete[] m_textureCoordinates[i];
	}

	m_textureChannels.Clear();
	m_textureCoordinates.Clear();
}

BOOL CRuMorphTarget::Initialize(INT32 numVertices, DWORD vertexFormat)
{
	// Destroy previously allocated memory
	delete[] m_position;
	delete[] m_normal;
	delete[] m_diffuse;
	delete[] m_specular;
	delete[] m_blendWeight;
	delete[] m_blendIndex;

	for(INT32 i = 0; i < m_textureCoordinates.Count(); ++i)
	{
		delete[] m_textureCoordinates[i];
	}

	m_textureChannels.Clear();
	m_textureCoordinates.Clear();

	m_vertexFormat = vertexFormat;
	m_vertexCount = numVertices;
	m_position = NULL;
	m_normal = NULL;
	m_diffuse = NULL;
	m_specular = NULL;
	m_blendWeight = NULL;
	m_blendIndex = NULL;

	// Re-allocate memory according to the specified vertex format
	if(m_vertexFormat & ruVERTEXFORMAT_POSITION)
		m_position = ruNEW CRuVector3 [m_vertexCount];

	if(m_vertexFormat & ruVERTEXFORMAT_NORMAL)
		m_normal = ruNEW CRuVector3 [m_vertexCount];

	if(m_vertexFormat & ruVERTEXFORMAT_DIFFUSE)
		m_diffuse = ruNEW DWORD [m_vertexCount];

	if(m_vertexFormat & ruVERTEXFORMAT_SPECULAR)
		m_specular = ruNEW DWORD [m_vertexCount];

	// Allocate blend weight array
	if(m_vertexFormat & ruVERTEXFORMAT_BLENDWEIGHT)
		m_blendWeight = ruNEW float [m_vertexCount * 4];

	// Allocate blend index array
	if(m_vertexFormat & ruVERTEXFORMAT_BLENDINDICES)
		m_blendIndex = ruNEW UINT16 [m_vertexCount * 4];

	return TRUE;
}

BOOL CRuMorphTarget::SetVertexFormat(DWORD vertexFormat)
{
	m_vertexFormat = vertexFormat;

	// Re-allocate memory according to the specified vertex format
	if(m_vertexFormat & ruVERTEXFORMAT_POSITION)
	{
		if(m_position == NULL)
			m_position = ruNEW CRuVector3 [m_vertexCount];
	}
	else
	{
		delete[] m_position;
		m_position = NULL;
	}

	if(m_vertexFormat & ruVERTEXFORMAT_NORMAL)
	{
		if(m_normal == NULL)
			m_normal = ruNEW CRuVector3 [m_vertexCount];
	}
	else
	{
		delete[] m_normal;
		m_normal = NULL;
	}

	if(m_vertexFormat & ruVERTEXFORMAT_DIFFUSE)
	{
		if(m_diffuse == NULL)
			m_diffuse = ruNEW DWORD [m_vertexCount];
	}
	else
	{
		delete[] m_diffuse;
		m_diffuse = NULL;
	}

	if(m_vertexFormat & ruVERTEXFORMAT_SPECULAR)
	{
		if(m_specular == NULL)
			m_specular = ruNEW DWORD [m_vertexCount];
	}
	else
	{
		delete[] m_specular;
		m_specular = NULL;
	}

	// Allocate blend weight array
	if(m_vertexFormat & ruVERTEXFORMAT_BLENDWEIGHT)
	{
		if(m_blendWeight == NULL)
			m_blendWeight = ruNEW float [m_vertexCount * 4];
	}
	else
	{
		delete[] m_blendWeight;
		m_blendWeight = NULL;
	}

	// Allocate blend index array
	if(m_vertexFormat & ruVERTEXFORMAT_BLENDINDICES)
	{
		if(m_blendIndex == NULL)
			m_blendIndex = ruNEW UINT16 [m_vertexCount * 4];
	}
	else
	{
		delete[] m_blendIndex;
		m_blendIndex = NULL;
	}

	return TRUE;
}

IRuObject *CRuMorphTarget::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuMorphTarget();

	// Chain cloning call to parent class
	IRuMorphTarget::Clone(clonedObject);

	// Shortcut
	CRuMorphTarget *clonedMT = static_cast<CRuMorphTarget *>(clonedObject);

	// Initialize cloned morph target
	clonedMT->Initialize(m_vertexCount, m_vertexFormat);

	// Execute copy function
	clonedMT->CopyFrom(this);

	return clonedObject;
}

BOOL CRuMorphTarget::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	INT32 vertexFormat;
	INT32 vertexCount;
	INT32 texChannelCount;

	// Read signature and version
	INT32 signature;
	INT32 version = 1;
	stream->Read(&signature, sizeof(INT32));

	if(signature == 0xFFFFFFFF)
	{
		// Signature found, read version 
		stream->Read(&version, sizeof(INT32));
	}
	else
	{
		// No signature, this is an old model stream, so rewind
		stream->Seek(-4, ruSSM_Current);
	}

	switch(version)
	{
		case 1:
			{
				stream->Read(&vertexFormat, sizeof(INT32));
				stream->Read(&vertexCount, sizeof(INT32));

				// Reinitialize vertex storage if necessary
				if(vertexCount != m_vertexCount)
				{
					Initialize(vertexCount, vertexFormat);
				}

				// Read positions
				for(INT32 j = 0; j < vertexCount; ++j)
				{
					stream->Read(&m_position[j].x, sizeof(float));
					stream->Read(&m_position[j].y, sizeof(float));
					stream->Read(&m_position[j].z, sizeof(float));
				}

				// Read normals
				if(m_vertexFormat & ruVERTEXFORMAT_NORMAL)
				{
					for(INT32 j = 0; j < vertexCount; ++j)
					{
						stream->Read(&m_normal[j].x, sizeof(float));
						stream->Read(&m_normal[j].y, sizeof(float));
						stream->Read(&m_normal[j].z, sizeof(float));
					}
				}
			}

			break;

		case 2:
			{
				stream->Read(&vertexFormat, sizeof(INT32));
				stream->Read(&vertexCount, sizeof(INT32));
				stream->Read(&texChannelCount, sizeof(INT32));

				// Reinitialize vertex storage if necessary
				if(vertexCount != m_vertexCount)
				{
					Initialize(vertexCount, vertexFormat);
				}

				// Read positions
				for(INT32 j = 0; j < vertexCount; ++j)
				{
					stream->Read(&m_position[j].x, sizeof(float));
					stream->Read(&m_position[j].y, sizeof(float));
					stream->Read(&m_position[j].z, sizeof(float));
				}

				// Read normals
				if(m_vertexFormat & ruVERTEXFORMAT_NORMAL)
				{
					for(INT32 j = 0; j < vertexCount; ++j)
					{
						stream->Read(&m_normal[j].x, sizeof(float));
						stream->Read(&m_normal[j].y, sizeof(float));
						stream->Read(&m_normal[j].z, sizeof(float));
					}
				}

				// Read diffuse
				if(m_vertexFormat & ruVERTEXFORMAT_DIFFUSE)
				{
					stream->Read(m_diffuse, sizeof(DWORD) * vertexCount);

					// Perform a check to see if the alpha or color data is bad (e.g. all vertices have alpha value of zero)
					BOOL badAlphaData = TRUE;
					BOOL badColorData = TRUE;
					for(INT32 i = 0; i < vertexCount; ++i)
					{
						if((m_diffuse[i] & 0xFF000000) != 0x00000000)
						{
							badAlphaData = FALSE;
						}

						if((m_diffuse[i] & 0x00FFFFFF) != 0x00000000)
						{
							badColorData = FALSE;
						}
					}

					if(badAlphaData)
					{
						for(INT32 i = 0; i < vertexCount; ++i)
						{
							m_diffuse[i] = m_diffuse[i] | 0xFF000000;
						}
					}

					if(badColorData)
					{
						for(INT32 i = 0; i < vertexCount; ++i)
						{
							m_diffuse[i] = m_diffuse[i] | 0x00FFFFFF;
						}
					}
				}

				// Read specular
				if(m_vertexFormat & ruVERTEXFORMAT_SPECULAR)
				{
					stream->Read(m_specular, sizeof(DWORD) * vertexCount);
				}

				if(m_vertexFormat & ruVERTEXFORMAT_BLENDWEIGHT)
				{
					stream->Read(m_blendWeight, sizeof(float) * vertexCount * 4);
				}

				if(m_vertexFormat & ruVERTEXFORMAT_BLENDINDICES)
				{
					stream->Read(m_blendIndex, sizeof(UINT16) * vertexCount * 4);
				}

				// Read texture channels
				for(INT32 i = 0; i < texChannelCount; ++i)
				{
					RuTextureChannel texChannel;
					INT32 numComponents;
					stream->Read(&texChannel, sizeof(RuTextureChannel));
					stream->Read(&numComponents, sizeof(INT32));

					// Read coordinate component data
					AddTextureCoordinate(texChannel);
					stream->Read(GetTextureCoordinate(texChannel), sizeof(float) * m_vertexCount * numComponents);
				}
			}

			break;
	}

	return TRUE;
}

BOOL CRuMorphTarget::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write signature
	INT32 signature = 0xFFFFFFFF;
	stream->Write(&signature, sizeof(INT32));

	// Write version
	INT32 version = 2;
	stream->Write(&version, sizeof(INT32));

	// Write vertex format flags
	stream->Write(&m_vertexFormat, sizeof(INT32));

	// Write vertex count
	stream->Write(&m_vertexCount, sizeof(INT32));

	// Write number of texture channels
	INT32 numTexChannels = m_textureChannels.Count();
	stream->Write(&numTexChannels, sizeof(INT32));

	// Write positions
	for(INT32 j = 0; j < m_vertexCount; ++j)
	{
		stream->Write(&m_position[j].x, sizeof(float));
		stream->Write(&m_position[j].y, sizeof(float));
		stream->Write(&m_position[j].z, sizeof(float));
	}

	if(m_vertexFormat & ruVERTEXFORMAT_NORMAL)
	{
		// Write normals
		for(INT32 j = 0; j < m_vertexCount; ++j)
		{
			stream->Write(&m_normal[j].x, sizeof(float));
			stream->Write(&m_normal[j].y, sizeof(float));
			stream->Write(&m_normal[j].z, sizeof(float));
		}
	}

	if(m_vertexFormat & ruVERTEXFORMAT_DIFFUSE)
	{
		stream->Write(m_diffuse, sizeof(DWORD) * m_vertexCount);
	}

	if(m_vertexFormat & ruVERTEXFORMAT_SPECULAR)
	{
		stream->Write(m_specular, sizeof(DWORD) * m_vertexCount);
	}

	if(m_vertexFormat & ruVERTEXFORMAT_BLENDWEIGHT)
	{
		stream->Write(m_blendWeight, sizeof(float) * m_vertexCount * 4);
	}

	if(m_vertexFormat & ruVERTEXFORMAT_BLENDINDICES)
	{
		stream->Write(m_blendIndex, sizeof(UINT16) * m_vertexCount * 4);
	}

	// Write texture channels
	for(INT32 i = 0; i < m_textureChannels.Count(); ++i)
	{
		RuTextureChannel texChannel = m_textureChannels[i];
		INT32 numComponents = RuTextureChannel_GetNumComponents(texChannel);
		stream->Write(&texChannel, sizeof(RuTextureChannel));
		stream->Write(&numComponents, sizeof(INT32));

		// Read coordinate component data
		stream->Write(GetTextureCoordinate(texChannel), sizeof(float) * m_vertexCount * numComponents);
	}

	return TRUE;
}

INT32 CRuMorphTarget::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuMorphTarget::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	footprint += sizeof(m_iterationNumber);

	footprint += sizeof(m_vertexFormat);
	footprint += sizeof(m_vertexCount);

	footprint += sizeof(m_position) + (m_position ? sizeof(CRuVector3) * m_vertexCount : 0);
	footprint += sizeof(m_normal) + (m_normal ? sizeof(CRuVector3) * m_vertexCount : 0);
	footprint += sizeof(m_diffuse) + (m_diffuse ? sizeof(DWORD) * m_vertexCount : 0);
	footprint += sizeof(m_specular) + (m_specular ? sizeof(DWORD) * m_vertexCount : 0);
	footprint += sizeof(m_blendWeight) + (m_blendWeight ? sizeof(REAL) * m_vertexCount * 4 : 0);
	footprint += sizeof(m_blendIndex) + (m_blendIndex ? sizeof(UINT16) * m_vertexCount * 4 : 0);

	footprint += m_textureChannels.GetMemoryFootprint();
	footprint += m_textureCoordinates.GetMemoryFootprint();
	for(INT32 i = 0; i < m_textureCoordinates.Count(); ++i)
	{
		footprint += sizeof(REAL) * m_vertexCount * RuTextureChannel_GetNumComponents(m_textureChannels[i]);
	}

	return footprint;
}

BOOL CRuMorphTarget::AddTextureCoordinate(RuTextureChannel type)
{
	if(!m_textureChannels.Contains(type))
	{
		m_textureChannels.Add(type);
		m_textureCoordinates.Add(ruNEW float [m_vertexCount * RuTextureChannel_GetNumComponents(type)]);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuMorphTarget::CopyFrom(IRuMorphTarget *morphTarget)
{
	if(morphTarget->GetType() == CRuMorphTarget::Type())
	{
		// Determine the number of vertices to copy
		INT32 vertsToCopy = min(m_vertexCount, static_cast<CRuMorphTarget *>(morphTarget)->m_vertexCount);

		// If the vertex formats do not match, re-initialize
		if(m_vertexFormat != static_cast<CRuMorphTarget *>(morphTarget)->m_vertexFormat)
		{
			Initialize(vertsToCopy, static_cast<CRuMorphTarget *>(morphTarget)->m_vertexFormat);
		}

		// Copy position
		if(m_vertexFormat & ruVERTEXFORMAT_POSITION)
		{
			memcpy(m_position, morphTarget->GetPosition(), sizeof(CRuVector3) * vertsToCopy);
		}

		// Copy normal
		if(m_vertexFormat & ruVERTEXFORMAT_NORMAL)
		{
			memcpy(m_normal, morphTarget->GetNormal(), sizeof(CRuVector3) * vertsToCopy);
		}

		if(m_vertexFormat & ruVERTEXFORMAT_DIFFUSE)
		{
			memcpy(m_diffuse, morphTarget->GetDiffuse(), sizeof(DWORD) * vertsToCopy);
		}

		if(m_vertexFormat & ruVERTEXFORMAT_SPECULAR)
		{
			memcpy(m_specular, morphTarget->GetSpecular(), sizeof(DWORD) * vertsToCopy);
		}

		if(m_vertexFormat & ruVERTEXFORMAT_BLENDWEIGHT)
		{
			memcpy(m_blendWeight, morphTarget->GetBlendWeight(), sizeof(float) * 4 * vertsToCopy);
		}

		if(m_vertexFormat & ruVERTEXFORMAT_BLENDINDICES)
		{
			memcpy(m_blendIndex, morphTarget->GetBlendIndex(), sizeof(UINT16) * 4 * vertsToCopy);
		}

		for(INT32 i = 0, numTexChannels = static_cast<CRuMorphTarget *>(morphTarget)->m_textureChannels.Count(); i < numTexChannels; ++i)
		{
			AddTextureCoordinate(static_cast<CRuMorphTarget *>(morphTarget)->m_textureChannels[i]);
			memcpy(m_textureCoordinates[i], static_cast<CRuMorphTarget *>(morphTarget)->m_textureCoordinates[i], sizeof(float) * RuTextureChannel_GetNumComponents(static_cast<CRuMorphTarget *>(morphTarget)->m_textureChannels[i]) * vertsToCopy);
		}

		return TRUE;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

CRuMeshTopology::CRuMeshTopology()
:	m_iteration(0),
	m_primitiveType(ruPRIMTYPE_TRIANGLELIST),
	m_numPrimitives(0),
	m_numIndices(0),
	m_indices(NULL)
{
}

CRuMeshTopology::CRuMeshTopology(RuPrimitiveType primType, INT32 numPrimitives)
:	m_iteration(0),
	m_primitiveType(ruPRIMTYPE_TRIANGLELIST),
	m_numPrimitives(0),
	m_numIndices(0),
	m_indices(NULL)
{
	Initialize(primType, numPrimitives);
}

CRuMeshTopology::~CRuMeshTopology()
{
	// Delete index data
	delete[] m_indices;

	// Delete batch data
	for(INT32 i = 0; i < m_batches.Count(); ++i)
	{
		delete m_batches[i];
	}

	m_batches.Clear();
}

BOOL CRuMeshTopology::Initialize(RuPrimitiveType primType, INT32 numPrimitives)
{
	delete[] m_indices;

	m_primitiveType = primType;
	m_numPrimitives = numPrimitives;
	m_numIndices = RuPrimitiveToIndexCount(m_numPrimitives, m_primitiveType);
	m_indices = ruNEW UINT16 [m_numIndices];

	// Setup one default batch
	this->ClearBatches();
	this->AddBatch(0, m_numPrimitives, NULL, 0);

	return TRUE;
}

IRuObject *CRuMeshTopology::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuMeshTopology();

	// Chain cloning call to parent class
	IRuMeshTopology::Clone(clonedObject);

	// Shortcut
	CRuMeshTopology *clonedTopology = static_cast<CRuMeshTopology *>(clonedObject);

	// Copy topology data
	clonedTopology->m_iteration = m_iteration;
	clonedTopology->m_primitiveType = m_primitiveType;
	clonedTopology->m_numPrimitives = m_numPrimitives;
	clonedTopology->m_numIndices = m_numIndices;

	clonedTopology->m_indices = ruNEW UINT16 [m_numIndices];
	memcpy(clonedTopology->m_indices, m_indices, sizeof(UINT16) * m_numIndices);

	for(INT32 i = 0; i < m_batches.Count(); ++i)
	{
		// Copy batch info
		BatchInfo *clonedBatchInfo = ruNEW BatchInfo();

		clonedBatchInfo->m_offset = m_batches[i]->m_offset;
		clonedBatchInfo->m_numPrimitives = m_batches[i]->m_numPrimitives;

		if(m_batches[i]->m_boneMap)
		{
			clonedBatchInfo->m_boneMap = ruNEW UINT16 [m_batches[i]->m_boneMapSize];
			memcpy(clonedBatchInfo->m_boneMap, m_batches[i]->m_boneMap, sizeof(UINT16) * m_batches[i]->m_boneMapSize);
			clonedBatchInfo->m_boneMapSize = m_batches[i]->m_boneMapSize;
		}

		clonedBatchInfo->m_parameterBlock = m_batches[i]->m_parameterBlock;

		// Insert batch info to batch list
		clonedTopology->m_batches.Add(clonedBatchInfo);
	}

	return clonedObject;
}

INT32 CRuMeshTopology::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuMeshTopology::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	footprint += sizeof(m_iteration);

	footprint += sizeof(m_primitiveType);
	footprint += sizeof(m_numPrimitives);
	footprint += sizeof(m_numIndices);
	footprint += sizeof(m_indices) + (m_indices ? sizeof(UINT16) * m_numIndices : 0);

	footprint += m_batches.GetMemoryFootprint();
	for(INT32 i = 0; i < m_batches.Count(); ++i)
	{
		footprint += sizeof(m_batches[i]->m_offset);
		footprint += sizeof(m_batches[i]->m_numPrimitives);
		footprint += sizeof(m_batches[i]->m_boneMap) + (m_batches[i]->m_boneMap ? sizeof(UINT16) * m_batches[i]->m_boneMapSize : 0);
		footprint += sizeof(m_batches[i]->m_boneMapSize);
		footprint += sizeof(m_batches[i]->m_parameterBlock);
	}

	return footprint;
}

INT32 CRuMeshTopology::GetIteration() const
{
	return m_iteration;
}

INT32 CRuMeshTopology::IncrementIteration()
{
	++m_iteration;
	return m_iteration;
}

RuPrimitiveType CRuMeshTopology::GetPrimitiveType() const
{
	return m_primitiveType;
}

INT32 CRuMeshTopology::GetNumPrimitives() const
{
	return m_numPrimitives;
}

INT32 CRuMeshTopology::GetNumIndices() const
{
	return m_numIndices;
}

UINT16 *CRuMeshTopology::GetIndices()
{
	return m_indices;
}

const UINT16 *CRuMeshTopology::GetIndices() const
{
	return m_indices;
}

INT32 CRuMeshTopology::AddBatch(INT32 offset, INT32 numPrimitives, UINT16 *boneMap, INT32 boneMapSize)
{
	BatchInfo *newBatch = ruNEW BatchInfo();

	newBatch->m_offset = offset;
	newBatch->m_numPrimitives = numPrimitives;
	newBatch->m_boneMap = boneMap;
	newBatch->m_boneMapSize = boneMapSize;

	m_batches.Add(newBatch);

	return m_batches.Count() - 1;
}

BOOL CRuMeshTopology::SetBatch(INT32 index, INT32 offset, INT32 numPrimitives)
{
	m_batches[index]->m_offset = offset;
	m_batches[index]->m_numPrimitives = numPrimitives;

	return TRUE;
}

void CRuMeshTopology::ClearBatches()
{
	// Clear existing batches
	for(INT32 i = 0; i < m_batches.Count(); ++i)
	{
		delete m_batches[i];
	}

	m_batches.Clear();
}

BOOL CRuMeshTopology::GetBatch(INT32 index, INT32 &offsetOut, INT32 &numPrimitivesOut) const
{
	if(index >= 0 && index < m_batches.Count())
	{
		offsetOut = m_batches[index]->m_offset;
		numPrimitivesOut = m_batches[index]->m_numPrimitives;

		return TRUE;
	}

	return FALSE;
}

CRuParameterBlock *CRuMeshTopology::GetBatchParameterBlock(INT32 index)
{
	if(index >= 0 && index < m_batches.Count())
	{
		return &m_batches[index]->m_parameterBlock;
	}

	return NULL;
}

// ************************************************************************************************************************************************************

CRuMesh::CRuMesh()
:	m_meshIterationNumber(0),
	m_numVertices(0),
	m_vertexFormat(0x00000000),
	m_meshTopology(NULL),
	m_collisionStructure(NULL)
{
}

CRuMesh::~CRuMesh()
{
	// Release morph targets
	for(INT32 i = 0; i < m_morphTargets.Count(); i++)
	{
		m_morphTargets[i]->Release();
	}
	m_morphTargets.Clear();

	// Release mesh topology object
	ruSAFE_RELEASE(m_meshTopology);

	// Release reference to the collision structure
	ruSAFE_RELEASE(m_collisionStructure);
}

BOOL CRuMesh::Initialize(RuPrimitiveType primitiveType, INT32 numVertices, INT32 numPrimitives, DWORD vertexFormat, INT32 numMorphTargets)
{
	// Delete previous morph targets
	for(INT32 i = 0; i < m_morphTargets.Count(); i++)
	{
		m_morphTargets[i]->Release();
	}
	m_morphTargets.Clear();

	m_numVertices = numVertices;
	m_vertexFormat = vertexFormat;

	// Create morph targets and add to morph target list
	while(numMorphTargets > 0)
	{
		CRuMorphTarget *morphTarget = ruNEW CRuMorphTarget();
		morphTarget->Initialize(m_numVertices, m_vertexFormat);
		m_morphTargets.Add(morphTarget);

		--numMorphTargets;
	}

	// Allocate mesh topology object
	ruSAFE_RELEASE(m_meshTopology);
	m_meshTopology = ruNEW CRuMeshTopology(primitiveType, numPrimitives);

	return TRUE;
}

IRuObject *CRuMesh::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuMesh();

	// Chain cloning call to parent class
	IRuMesh::Clone(clonedObject);

	// Shortcut
	CRuMesh *clonedMesh = static_cast<CRuMesh *>(clonedObject);

	// Copy mesh parameters
	clonedMesh->m_meshIterationNumber = m_meshIterationNumber;
	clonedMesh->m_numVertices = m_numVertices;
	clonedMesh->m_vertexFormat = m_vertexFormat;

	// Clone morph targets
	for(INT32 i = 0; i < m_morphTargets.Count(); ++i)
	{
		CRuMorphTarget *mt = static_cast<CRuMorphTarget *>(m_morphTargets[i]->Clone());
		clonedMesh->m_morphTargets.Add(mt);
	}

	// Clone mesh topology
	clonedMesh->m_meshTopology = static_cast<IRuMeshTopology *>(m_meshTopology->Clone());

	// Copy bounds
	clonedMesh->m_bounds = m_bounds;

	// Clone collision structure (CAUTION: None of the collision structures have cloning implemented!)
	if(m_collisionStructure)
	{
		clonedMesh->m_collisionStructure = static_cast<IRuCollisionStructure *>(m_collisionStructure->Clone());
	}

	return clonedObject;
}

BOOL CRuMesh::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	RuPrimitiveType primitiveType;
	DWORD vertexFormatFlags;
	INT32 vertexCount, texChannelCount, primitiveCount, mtCount;

	// Read signature and version
	INT32 signature;
	INT32 version = 1;
	stream->Read(&signature, sizeof(INT32));

	if(signature == 0xFFFFFFFF)
	{
		// Signature found, read version 
		stream->Read(&version, sizeof(INT32));
	}
	else
	{
		// No signature, this is an old model stream, so rewind
		stream->Seek(-4, ruSSM_Current);
	}

	switch(version)
	{
		case 1:
			{
				DWORD *diffuse = NULL;
				DWORD *specular = NULL;
				float *blendWeight = NULL;
				UINT16 *blendIndex = NULL;
				CRuArrayList<RuTextureChannel> textureChannels;
				CRuArrayList<float *> textureCoordinates;

				// Read mesh header
				stream->Read(&primitiveType, sizeof(RuPrimitiveType));
				stream->Read(&vertexFormatFlags, sizeof(DWORD));
				stream->Read(&vertexCount, sizeof(INT32));
				stream->Read(&texChannelCount, sizeof(INT32));
				stream->Read(&primitiveCount, sizeof(INT32));
				stream->Read(&mtCount, sizeof(INT32));

				// Initialize mesh storage
				Initialize(primitiveType, vertexCount, primitiveCount, vertexFormatFlags, 0);

				if(vertexFormatFlags & ruVERTEXFORMAT_DIFFUSE)
				{
					diffuse = ruNEW DWORD [vertexCount];
					stream->Read(diffuse, sizeof(DWORD) * vertexCount);
				}

				if(vertexFormatFlags & ruVERTEXFORMAT_SPECULAR)
				{
					specular = ruNEW DWORD [vertexCount];
					stream->Read(specular, sizeof(DWORD) * vertexCount);
				}

				if(vertexFormatFlags & ruVERTEXFORMAT_BLENDWEIGHT)
				{
					blendWeight = ruNEW float [vertexCount * 4];
					stream->Read(blendWeight, sizeof(float) * vertexCount * 4);
				}

				if(vertexFormatFlags & ruVERTEXFORMAT_BLENDINDICES)
				{
					blendIndex = ruNEW UINT16 [vertexCount * 4];
					stream->Read(blendIndex, sizeof(UINT16) * vertexCount * 4);
				}

				// Read texture channels
				for(INT32 i = 0; i < texChannelCount; ++i)
				{
					RuTextureChannel texChannel;
					INT32 numComponents;
					stream->Read(&texChannel, sizeof(RuTextureChannel));
					stream->Read(&numComponents, sizeof(INT32));

					// Read coordinate component data
					textureChannels.Add(texChannel);
					textureCoordinates.Add(ruNEW float [m_numVertices * numComponents]);
					stream->Read(textureCoordinates[i], sizeof(float) * m_numVertices * numComponents);
				}

				// Read face index array
				INT32 numIndices = m_meshTopology->GetNumIndices();
				UINT16 *indices = m_meshTopology->GetIndices();
				stream->Read(indices, sizeof(UINT16) * numIndices);

				// Read morph targets
				for(INT32 i = 0; i < mtCount; ++i)
				{
					IRuObject *object = RuObjectFactory().CreateObject(stream);

					if(object->GetType().IsSubClassOf(CRuMorphTarget::Type()))
					{
						// Add morph target to list of morph targets
						m_morphTargets.Add((CRuMorphTarget *) object);
						m_morphTargets[i]->SetVertexFormat(vertexFormatFlags);

						// Set other channel data into morph target
						if(diffuse)
						{
							memcpy(m_morphTargets[i]->GetDiffuse(), diffuse, sizeof(DWORD) * vertexCount);
						}

						if(specular)
						{
							memcpy(m_morphTargets[i]->GetSpecular(), specular, sizeof(DWORD) * vertexCount);
						}

						if(blendWeight)
						{
							memcpy(m_morphTargets[i]->GetBlendWeight(), blendWeight, sizeof(float) * vertexCount * 4);
						}

						if(blendIndex)
						{
							memcpy(m_morphTargets[i]->GetBlendIndex(), blendIndex, sizeof(UINT16) * vertexCount * 4);
						}

						for(INT32 j = 0; j < textureChannels.Count(); ++j)
						{
							m_morphTargets[i]->AddTextureCoordinate(textureChannels[j]);
							memcpy(m_morphTargets[i]->GetTextureCoordinate(textureChannels[j]), textureCoordinates[j], sizeof(float) * vertexCount * RuTextureChannel_GetNumComponents(textureChannels[j]));
						}
					}
				}

				// Free temporary resources
				delete[] diffuse;
				delete[] specular;
				delete[] blendWeight;
				delete[] blendIndex;

				for(INT32 i = 0; i < textureChannels.Count(); ++i)
				{
					delete[] textureCoordinates[i];
				}
			}

			break;

		case 2:
			{
				// Read mesh header
				stream->Read(&primitiveType, sizeof(RuPrimitiveType));
				stream->Read(&vertexFormatFlags, sizeof(DWORD));
				stream->Read(&vertexCount, sizeof(INT32));
				stream->Read(&primitiveCount, sizeof(INT32));
				stream->Read(&mtCount, sizeof(INT32));

				// Initialize mesh storage
				Initialize(primitiveType, vertexCount, primitiveCount, vertexFormatFlags, 0);

				// Read face index array
				INT32 numIndices = m_meshTopology->GetNumIndices();
				UINT16 *indices = m_meshTopology->GetIndices();
				stream->Read(indices, sizeof(UINT16) * numIndices);

				// Read morph targets
				for(INT32 i = 0; i < mtCount; ++i)
				{
					IRuObject *object = RuObjectFactory().CreateObject(stream);

					if(object->GetType().IsSubClassOf(CRuMorphTarget::Type()))
					{
						// Add morph target to list of morph targets
						m_morphTargets.Add((CRuMorphTarget *) object);
					}
				}
			}

			break;

		case 3:	// Version 2005.09.30
			{
				// Read mesh header
				stream->Read(&primitiveType, sizeof(RuPrimitiveType));
				stream->Read(&vertexFormatFlags, sizeof(DWORD));
				stream->Read(&vertexCount, sizeof(INT32));
				stream->Read(&primitiveCount, sizeof(INT32));
				stream->Read(&mtCount, sizeof(INT32));

				// Initialize mesh storage
				Initialize(primitiveType, vertexCount, primitiveCount, vertexFormatFlags, 0);

				// Read face index array
				INT32 numIndices = m_meshTopology->GetNumIndices();
				UINT16 *indices = m_meshTopology->GetIndices();
				stream->Read(indices, sizeof(UINT16) * numIndices);

				// Read morph targets
				for(INT32 i = 0; i < mtCount; ++i)
				{
					IRuObject *object = RuObjectFactory().CreateObject(stream);

					if(object->GetType().IsSubClassOf(CRuMorphTarget::Type()))
					{
						// Add morph target to list of morph targets
						m_morphTargets.Add((CRuMorphTarget *) object);
					}
				}

				// Read whether collision data exists
				INT32 colDataExists = 0;
				stream->Read(&colDataExists, sizeof(INT32));

				// If collision data exists, read it
				if(colDataExists)
				{
					IRuObject *object = RuObjectFactory().CreateObject(stream);

					if(object && object->GetType().IsSubClassOf(IRuCollisionStructure::Type()))
					{
						// Attach collision structure
						m_collisionStructure = static_cast<IRuCollisionStructure *>(object);
						m_collisionStructure->AttachToMesh(this);
					}
				}
			}

			break;
	}

	// Build bounding sphere using morph target 0 (for now)
	CalculateBounds();

/*
	// Optimize
	unsigned short numGroups;
	PrimitiveGroup *primGroup;
	SetListsOnly(true);
	if(GenerateStrips(m_indices, m_numIndices * 3, &primGroup, &numGroups, false))
	{
		memcpy(m_indices, primGroup->indices, sizeof(UINT16) * m_numIndices * 3);
	}
	else
	{
		MessageBox(NULL, "MOO", "", MB_OK);
	}
*/

	return TRUE;
}

BOOL CRuMesh::SerializeTo(IRuStream *stream)
{
	RuPrimitiveType primitiveType = m_meshTopology->GetPrimitiveType();
	INT32 numIndices = m_meshTopology->GetNumIndices();
	UINT16 *indices = m_meshTopology->GetIndices();

	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write signature
	INT32 signature = 0xFFFFFFFF;
	stream->Write(&signature, sizeof(INT32));

	// Write version
	INT32 version = 3;
	stream->Write(&version, sizeof(INT32));

	// Write primitive type
	stream->Write(&primitiveType, sizeof(RuPrimitiveType));

	// Write vertex format flags
	stream->Write(&m_vertexFormat, sizeof(DWORD));

	// Write vertex count
	stream->Write(&m_numVertices, sizeof(INT32));

	// Write face count
	INT32 primitiveCount = RuIndexToPrimitiveCount(numIndices, primitiveType);
	stream->Write(&primitiveCount, sizeof(INT32));

	// Write morph target count
	INT32 mtCount = m_morphTargets.Count();
	stream->Write(&mtCount, sizeof(INT32));

	// Write face index array
	stream->Write(indices, sizeof(UINT16) * numIndices);

	// Write morph targets
	for(INT32 i = 0; i < m_morphTargets.Count(); ++i)
	{
		m_morphTargets[i]->SerializeTo(stream);
	}

	// Write collision data existence flag
	INT32 colDataExists = (m_collisionStructure != NULL);
	stream->Write(&colDataExists, sizeof(INT32));

	// Write collision data
	if(m_collisionStructure)
	{
		m_collisionStructure->SerializeTo(stream);
	}

	return TRUE;
}

INT32 CRuMesh::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuMesh::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	footprint += sizeof(m_meshIterationNumber);

	footprint += sizeof(m_numVertices);
	footprint += sizeof(m_vertexFormat);

	footprint += m_morphTargets.GetMemoryFootprint();
	for(INT32 i = 0; i < m_morphTargets.Count(); ++i)
	{
		footprint += m_morphTargets[i]->GetMemoryFootprint(enumHash);
	}

	footprint += sizeof(m_meshTopology) + (m_meshTopology ? m_meshTopology->GetMemoryFootprint(enumHash) : 0);

	footprint += sizeof(m_bounds);

	footprint += sizeof(m_collisionStructure) + (m_collisionStructure ? m_collisionStructure->GetMemoryFootprint(enumHash) : 0);

	return footprint;
}

IRuMeshTopology *CRuMesh::GetMeshTopology()
{
	return m_meshTopology;
}

void CRuMesh::OverrideMeshTopology(IRuMeshTopology *meshTopology)
{
	if(meshTopology)
	{
		meshTopology->AddRef();
		ruSAFE_RELEASE(m_meshTopology);
		m_meshTopology = meshTopology;
	}
}

const IRuMeshTopology *CRuMesh::GetMeshTopology() const
{
	return m_meshTopology;
}

BOOL CRuMesh::AddTextureCoordinate(RuTextureChannel type)
{
	// Iterate through all morph targets and add the specified texture channel
	for(INT32 i = 0; i < m_morphTargets.Count(); ++i)
	{
		m_morphTargets[i]->AddTextureCoordinate(type);
	}

	return TRUE;
}

INT32 CRuMesh::AddBatch(INT32 offset, INT32 numPrimitives)
{
	return m_meshTopology->AddBatch(offset, numPrimitives, NULL, 0);
}

void CRuMesh::ClearBatches()
{
	m_meshTopology->ClearBatches();
}

BOOL CRuMesh::GetBatch(INT32 index, INT32 &offsetOut, INT32 &numPrimitivesOut) const
{
	return m_meshTopology->GetBatch(index, offsetOut, numPrimitivesOut);
}

CRuParameterBlock *CRuMesh::GetBatchParameterBlock(INT32 index)
{
	return m_meshTopology->GetBatchParameterBlock(index);
}

IRuCollisionStructure *CRuMesh::GetCollisionStructure()
{
	return m_collisionStructure;
}

BOOL CRuMesh::SetCollisionStructure(IRuCollisionStructure *collisionStructure)
{
	ruSAFE_RELEASE(m_collisionStructure);

	if(collisionStructure)
	{
		m_collisionStructure = collisionStructure;
		m_collisionStructure->AddRef();
	}

	return TRUE;
}

BOOL CRuMesh::QueryCollision(CRuCollisionQuery *colQuery)
{
	// If no collision structure exists, build a default one
	if(m_collisionStructure == NULL)
	{
		// Use a simple collision structure
		m_collisionStructure = ruNEW CRuCollisionStructure_Simple();
		m_collisionStructure->BuildCollisionStructure(this, NULL);
	}

	return m_collisionStructure->QueryCollision(colQuery);
}

void CRuMesh::CalculateBounds()
{
	// Build bounding box using morph target 0 (for now)
	if(m_morphTargets.Count() > 0)
	{
		// Build initial bounds using morph target 0
		CRuMinSphereBuilder minSphereBuilder;
		minSphereBuilder.BuildSphere(m_morphTargets[0]->GetPosition(), m_numVertices, m_bounds);

		// Merge any additional morph target bounds
		for(INT32 i = 1; i < m_morphTargets.Count(); ++i)
		{
			CRuSphere mtBounds;
			minSphereBuilder.BuildSphere(m_morphTargets[i]->GetPosition(), m_numVertices, mtBounds);
			m_bounds.Merge(mtBounds);
		}
	}
	else
	{
		m_bounds.Set(CRuVector3(0.0f, 0.0f, 0.0f), 0.0f);
	}
}

void CRuMesh::SetBounds(const CRuSphere &bounds)
{
	m_bounds = bounds;
}

BOOL CRuMesh::SkinnedMesh_IsSkinned()
{
	if(m_morphTargets.Count() > 0 && m_morphTargets[0]->GetBlendIndex() != NULL && m_morphTargets[0]->GetBlendWeight() != NULL)
		return TRUE;

	return FALSE;
}

INT32 CRuMesh::SkinnedMesh_GetNumBones()
{
	INT32 maxBoneIndex = 0;

	if(m_morphTargets.Count() > 0)
	{
		const UINT16 *blendIndex = m_morphTargets[0]->GetBlendIndex();

		if(blendIndex)
		{
			// Iterate through each blend index and find the largest value
			INT32 numBlendIndices = m_numVertices * 4;
			for(INT32 i = 0; i < numBlendIndices; ++i)
			{
				maxBoneIndex = max(maxBoneIndex, blendIndex[i] + 1);
			}
		}
	}

	return maxBoneIndex;
}

void CRuMesh::SkinnedMesh_Subdivide(INT32 maxBones, CRuArrayList<IRuMesh *> *subMeshListOut)
{
	// Can only perform split on a skinned mesh
	if(!SkinnedMesh_IsSkinned())
		return;

	const INT32 indicesPerPrimitive = 3;
	const INT32 numPrimitives = this->GetNumPrimitives();

	const INT32 numBones = SkinnedMesh_GetNumBones();

	// Blend weight & index shorthand
	float *blendWeight = m_morphTargets[0]->GetBlendWeight();
	UINT16 *blendIndex = m_morphTargets[0]->GetBlendIndex();

	// Iterate through every face
	CRuBitField boneUsed(numBones);
	CRuBitField processedPrimitives(numPrimitives);

	CRuBitField copyMask(numPrimitives);
	CRuBitField vertexUsed(m_numVertices);

	INT32 vertexOffset = 0,indexOffset = 0;
	INT32 numProcessedPrimitives = 0;

	UINT16 *indices = m_meshTopology->GetIndices();

	// Iterate until all primitives have been processed
	while(numProcessedPrimitives < numPrimitives)
	{
		INT32 totalBonesUsed = 0;

		INT32 verticesThisBatch = 0;
		INT32 primitivesThisBatch = 0;

		// Clear bone usage array
		boneUsed.ClearAll();

		// Clear primitive mask
		copyMask.ClearAll();

		// Clear vertex mask
		vertexUsed.ClearAll();

		CRuArrayList<INT32> newVertices;
		CRuArrayList<UINT16> newBones;

		// Iterate through all primitives
		for(INT32 curPrimIdx = 0; curPrimIdx < numPrimitives; ++curPrimIdx)
		{
			INT32 i = curPrimIdx * indicesPerPrimitive;

			if(!processedPrimitives[curPrimIdx])
			{
				// Clear new vertex & bone list
				newVertices.Clear();
				newBones.Clear();

				// Iterate through each vertex of the primitive
				for(INT32 curVertIdx = 0; curVertIdx < indicesPerPrimitive; ++curVertIdx)
				{
					// Store the current vertex index
					INT32 vIdx = indices[i + curVertIdx];

					// Save index of the new vertex
					if(!vertexUsed[vIdx] && !newVertices.Contains(vIdx))
					{
						newVertices.Add(vIdx);
					}

					// Iterate through each blend index of the vertex
					for(INT32 j = vIdx * 4, k = 0; k < 4; ++j, ++k)
					{
						// Save index of the new bone
						if(blendWeight[j] > ruEPSILON && !boneUsed[blendIndex[j]] && !newBones.Contains(blendIndex[j]))
						{
							newBones.Add(blendIndex[j]);
						}
					}
				}

				// Add the primitive if does not exceed the maximum bone allowance
				if(newBones.Count() + totalBonesUsed <= maxBones)
				{
					// Iterate through new vertices
					for(INT32 j = 0; j < newVertices.Count(); ++j)
					{
						vertexUsed.Set(newVertices[j]);
					}

					// Iterate through new bones
					for(INT32 j = 0; j < newBones.Count(); ++j)
					{
						// Mark bone as used
						boneUsed.Set(newBones[j]);
					}

					// Mark primitive as processed
					processedPrimitives.Set(curPrimIdx);

					// Add primitive to the copy mask
					copyMask.Set(curPrimIdx);

					// Update total number of bones
					totalBonesUsed += newBones.Count();

					// Increment number of processed primitives
					++numProcessedPrimitives;

					// Increment number of primitives this batch
					++primitivesThisBatch;
					verticesThisBatch += newVertices.Count();
				}
			}
		}

		// Create a temporary mesh with identical data storage to this mesh
		CRuMesh *dstMesh = ruNEW CRuMesh();
		dstMesh->Initialize(m_meshTopology->GetPrimitiveType(), verticesThisBatch, primitivesThisBatch, m_vertexFormat, m_morphTargets.Count());
		for(INT32 i = 0; i < m_morphTargets[0]->GetNumTextureCoordinates(); ++i)
		{
			dstMesh->AddTextureCoordinate(m_morphTargets[0]->GetTextureChannelByIndex(i));
		}

		// Split the mesh
		INT32 verticesCopied = 0, indicesCopied = 0;
		MeshUtility_CopyMeshSubset(copyMask, dstMesh, 0, 0, verticesCopied, indicesCopied);

		// Build bone map
		UINT16 *boneMap = ruNEW UINT16 [numBones];
		UINT16 *indexMap = ruNEW UINT16 [totalBonesUsed];

		for(INT32 i = 0, j = 0; i < numBones; ++i)
		{
			if(boneUsed[i])
			{
				boneMap[i] = j;
				indexMap[j] = i;
				++j;
			}
			else
			{
				boneMap[i] = 0;
			}
		}

		// Re-map bone indices
		dstMesh->MeshUtility_RemapBones(0, verticesCopied, boneMap);

		// Copy index map into batch structure
		IRuMeshTopology *dstMeshTopology = dstMesh->GetMeshTopology();
		dstMeshTopology->ClearBatches();
		dstMeshTopology->AddBatch(0, primitivesThisBatch, indexMap, totalBonesUsed);

		CRuParameterBlock *dstParamBlock = dstMeshTopology->GetBatchParameterBlock(0);
		dstParamBlock->SetParameter(ruPARAMTYPE_SKIN_INDEXMAP, reinterpret_cast<UINT64>(indexMap));
		dstParamBlock->SetParameter(ruPARAMTYPE_SKIN_NODECOUNT, static_cast<UINT64>(totalBonesUsed));

		// Add mesh to list of output meshes
		subMeshListOut->Add(dstMesh);

		delete[] boneMap;
	}
}

BOOL CRuMesh::SkinnedMesh_GenerateBoneUsageMap(CRuBitField &boneUsageMap)
{
	// Verify that the mesh has skin data
	if(!SkinnedMesh_IsSkinned())
		return FALSE;

	const INT32 indicesPerPrimitive = 3;
	const INT32 numPrimitives = this->GetNumPrimitives();
	const INT32 numBones = SkinnedMesh_GetNumBones();

	// Blend weight & index shorthand
	REAL *blendWeight = m_morphTargets[0]->GetBlendWeight();
	UINT16 *blendIndex = m_morphTargets[0]->GetBlendIndex();

	// Iterate through every face
	boneUsageMap.SetNumBits(numBones);


	CRuBitField processedPrimitives(numPrimitives);

	CRuBitField copyMask(numPrimitives);
	CRuBitField vertexUsed(m_numVertices);

	INT32 vertexOffset = 0,indexOffset = 0;
	INT32 numProcessedPrimitives = 0;

	UINT16 *indices = m_meshTopology->GetIndices();

	// Iterate until all primitives have been processed
	while(numProcessedPrimitives < numPrimitives)
	{
	}

	return TRUE;
}

void CRuMesh::SkinnedMesh_GenerateBoneMap(CRuArrayList<UINT8> &boneMap)
{
	// Can only perform split on a skinned mesh
	if(!SkinnedMesh_IsSkinned())
		return;


}

void CRuMesh::SkinnedMesh_Split(INT32 maxBones)
{

}

BOOL CRuMesh::MeshUtility_MeshCopy(IRuMesh *srcMesh)
{
	return FALSE;
}

BOOL CRuMesh::MeshUtility_DataSwap(CRuMesh *srcMesh)
{
/*
	// Declarations
	INT32								oldMeshIterationNumber;
	INT32								oldTopologyIterationNumber;

	RuPrimitiveType						oldPrimitiveType;
	INT32								oldNumVertices;
	INT32								oldNumIndices;
	DWORD								oldVertexFormat;

	CRuArrayList<CRuMorphTarget *>		oldMorphTargets;
	UINT16*								oldIndices;

	CRuSphere							oldBounds;

	CRuArrayList<BatchInfo *>			oldBatches;

	// Save original data
	oldMeshIterationNumber				= this->m_meshIterationNumber;
	oldTopologyIterationNumber			= this->m_topologyIterationNumber;

	oldPrimitiveType					= this->m_primitiveType;
	oldNumVertices						= this->m_numVertices;
	oldNumIndices						= this->m_numIndices;
	oldVertexFormat						= this->m_vertexFormat;

	oldMorphTargets.CloneFrom(this->m_morphTargets);
	oldIndices							= this->m_indices;

	oldBounds							= this->m_bounds;

	oldBatches.CloneFrom(this->m_batches);

	// Copy new data
	this->m_meshIterationNumber			= srcMesh->m_meshIterationNumber;
	this->m_topologyIterationNumber		= srcMesh->m_topologyIterationNumber;

	this->m_primitiveType				= srcMesh->m_primitiveType;
	this->m_numVertices					= srcMesh->m_numVertices;
	this->m_numIndices					= srcMesh->m_numIndices;
	this->m_vertexFormat				= srcMesh->m_vertexFormat;

	this->m_morphTargets.CloneFrom(srcMesh->m_morphTargets);
	this->m_indices						= srcMesh->m_indices;

	this->m_bounds						= srcMesh->m_bounds;

	this->m_batches.CloneFrom(srcMesh->m_batches);

	ruSAFE_RELEASE(this->m_collisionStructure);

	// Copy old data
	srcMesh->m_meshIterationNumber		= oldMeshIterationNumber;
	srcMesh->m_topologyIterationNumber	= oldTopologyIterationNumber;

	srcMesh->m_primitiveType			= oldPrimitiveType;
	srcMesh->m_numVertices				= oldNumVertices;
	srcMesh->m_numIndices				= oldNumIndices;
	srcMesh->m_vertexFormat				= oldVertexFormat;

	srcMesh->m_morphTargets.CloneFrom(oldMorphTargets);
	srcMesh->m_indices					= oldIndices;

	srcMesh->m_bounds					= oldBounds;

	srcMesh->m_batches.CloneFrom(oldBatches);

	ruSAFE_RELEASE(srcMesh->m_collisionStructure);
*/
	return TRUE;
}

BOOL CRuMesh::MeshUtility_CopyMeshSubset(const CRuBitField &copyMask, CRuMesh *dstMesh, INT32 vertexOffset, INT32 indexOffset, INT32 &verticesCopiedOut, INT32 &indicesCopiedOut)
{
	INT32 srcNumPrimitives = this->GetNumPrimitives();
	INT32 numCopiedPrimitives = 0;
	INT32 indicesPerPrimitive = 3;

	// Reset copy counters
	verticesCopiedOut = 0;
	indicesCopiedOut = 0;

	INT32 numSetVertices = 0;

	UINT16 *indices = m_meshTopology->GetIndices();
	UINT16 *dstIndices = dstMesh->GetMeshTopology()->GetIndices();

	// Build mask of vertices which are to be copied
	CRuBitField vertexMask(m_numVertices);

	for(INT32 i = 0; i < srcNumPrimitives; ++i)
	{
		// If the copy mask contains the current primitive, add its vertices to the vertex mask
		if(copyMask[i])
		{
			// Iterate through all vertices used by the primitive
			for(INT32 j = 0, k = i * indicesPerPrimitive; j < indicesPerPrimitive; ++j, ++k)
			{
				if(!vertexMask[indices[k]])
					++numSetVertices;

				// Mark vertex as used
				vertexMask.Set(indices[k]);
			}

			// Accumulate the number of primitives to be copied
			++numCopiedPrimitives;
		}
	}

	// Build vertex mapping for the current subset
	UINT16 *vertexMap = ruNEW UINT16 [m_numVertices];

	for(INT32 i = 0, j = vertexOffset; i < m_numVertices; ++i)
	{
		if(vertexMask[i])
		{
			++verticesCopiedOut;

			vertexMap[i] = j;
			++j;
		}
	}

	// Copy remapped vertices
	CRuMorphTarget *srcMT = this->m_morphTargets[0];
	CRuMorphTarget *dstMT = dstMesh->m_morphTargets[0];

	for(INT32 i = 0; i < m_numVertices; ++i)
	{
		if(vertexMask[i])
		{
			if(this->m_vertexFormat & ruVERTEXFORMAT_POSITION)
			{
				dstMT->GetPosition()[vertexMap[i]] = srcMT->GetPosition()[i];
			}

			if(this->m_vertexFormat & ruVERTEXFORMAT_NORMAL)
			{
				dstMT->GetNormal()[vertexMap[i]] = srcMT->GetNormal()[i];
			}

			if(this->m_vertexFormat & ruVERTEXFORMAT_DIFFUSE)
			{
				dstMT->GetDiffuse()[vertexMap[i]] = srcMT->GetDiffuse()[i];
			}

			if(this->m_vertexFormat & ruVERTEXFORMAT_SPECULAR)
			{
				dstMT->GetSpecular()[vertexMap[i]] = srcMT->GetSpecular()[i];
			}

			if(this->m_vertexFormat & ruVERTEXFORMAT_BLENDWEIGHT)
			{
				INT32 srcIdx = i * 4;
				INT32 dstIdx = vertexMap[i] * 4;

				dstMT->GetBlendWeight()[dstIdx++] = srcMT->GetBlendWeight()[srcIdx++];
				dstMT->GetBlendWeight()[dstIdx++] = srcMT->GetBlendWeight()[srcIdx++];
				dstMT->GetBlendWeight()[dstIdx++] = srcMT->GetBlendWeight()[srcIdx++];
				dstMT->GetBlendWeight()[dstIdx++] = srcMT->GetBlendWeight()[srcIdx++];
			}

			if(this->m_vertexFormat & ruVERTEXFORMAT_BLENDINDICES)
			{
				INT32 srcIdx = i * 4;
				INT32 dstIdx = vertexMap[i] * 4;

				dstMT->GetBlendIndex()[dstIdx++] = srcMT->GetBlendIndex()[srcIdx++];
				dstMT->GetBlendIndex()[dstIdx++] = srcMT->GetBlendIndex()[srcIdx++];
				dstMT->GetBlendIndex()[dstIdx++] = srcMT->GetBlendIndex()[srcIdx++];
				dstMT->GetBlendIndex()[dstIdx++] = srcMT->GetBlendIndex()[srcIdx++];
			}

			for(INT32 j = 0; j < srcMT->GetNumTextureCoordinates(); ++j)
			{
				INT32 numElements = RuTextureChannel_GetNumComponents(srcMT->GetTextureChannelByIndex(j));
				INT32 srcIdx = i * numElements;
				INT32 dstIdx = vertexMap[i] * numElements;

				for(INT32 k = 0; k < numElements; ++k, ++dstIdx, ++srcIdx)
				{
					dstMT->GetTextureCoordinateByIndex(j)[dstIdx] = srcMT->GetTextureCoordinateByIndex(j)[srcIdx];
				}
			}
		}
	}

	// Copy remapped indices
	for(INT32 i = 0, j = indexOffset; i < srcNumPrimitives; ++i)
	{
		if(copyMask[i])
		{
			for(INT32 k = 0, l = i * indicesPerPrimitive; k < indicesPerPrimitive; ++j, ++k, ++l)
			{
				++indicesCopiedOut;

				dstIndices[j] = vertexMap[indices[l]];
			}
		}
	}

	// Delete temporaries
	delete[] vertexMap;

	return TRUE;
}

BOOL CRuMesh::MeshUtility_RemapBones(INT32 startVertex, INT32 vertexCount, const UINT16 *boneMap)
{
	if(this->m_vertexFormat & ruVERTEXFORMAT_BLENDINDICES)
	{
		UINT16 *blendIndices = m_morphTargets[0]->GetBlendIndex();

		for(INT32 i = startVertex * 4, j = 0; j < vertexCount * 4; ++i, ++j)
		{
			blendIndices[i] = boneMap[blendIndices[i]];
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuSkinnableMesh::CRuSkinnableMesh()
:	m_ignoreNormalTransforms(FALSE),
	m_srcMesh(NULL),
	m_cachedX(NULL),
	m_cachedY(NULL),
	m_cachedZ(NULL),
	m_cachedNX(NULL),
	m_cachedNY(NULL),
	m_cachedNZ(NULL),
	m_cachedBlendIndex(NULL),
	m_cachedBlendWeight(NULL),
	m_cachedBoneMatrixArraySize(0),
	m_cachedBoneMatrixArray(NULL),
	m_iterationNumber(0),
	m_skinnedMT(NULL),
	m_setPoseEnabled(TRUE)
{
}

CRuSkinnableMesh::~CRuSkinnableMesh()
{
	FreeResources();
}

void CRuSkinnableMesh::EnableSetPose()
{
	m_setPoseEnabled = TRUE;
}

void CRuSkinnableMesh::SetSourceMesh(IRuMesh *srcMesh)
{
	// Release old resources
	FreeResources();

	// Store reference to the new source mesh
	m_srcMesh = srcMesh;
	m_srcMesh->AddRef();

	// Initialize new skinned morph target
	m_skinnedMT = ruNEW CRuMorphTarget();

	// TODO: Initialize SSE-optimized vertex and normal arrays
	m_cachedSrcIteration = m_srcMesh->GetMeshIteration();
	m_numVertexGroups = (m_srcMesh->GetNumVertices() / 4) + ((m_srcMesh->GetNumVertices() % 4) ? 1 : 0);

	m_skinnedMT->Initialize(m_numVertexGroups * 4, m_srcMesh->GetVertexFormat());


	m_cachedX = (__m128 *) _aligned_malloc(m_numVertexGroups * sizeof(__m128), 16);
	m_cachedY = (__m128 *) _aligned_malloc(m_numVertexGroups * sizeof(__m128), 16);
	m_cachedZ = (__m128 *) _aligned_malloc(m_numVertexGroups * sizeof(__m128), 16);
	m_cachedNX = (__m128 *) _aligned_malloc(m_numVertexGroups * sizeof(__m128), 16);
	m_cachedNY = (__m128 *) _aligned_malloc(m_numVertexGroups * sizeof(__m128), 16);
	m_cachedNZ = (__m128 *) _aligned_malloc(m_numVertexGroups * sizeof(__m128), 16);

	const CRuVector3 *position = m_srcMesh->GetMorphTarget(0)->GetPosition();
	const CRuVector3 *normal = m_srcMesh->GetMorphTarget(0)->GetNormal();

	INT32 i = 0, j = 0;
	for(INT32 k = 1; k < m_numVertexGroups; ++k, i += 4, ++j)
	{
		RuSSE_LoadFourFloats(&position[i].x, &position[i + 1].x, &position[i + 2].x, &position[i + 3].x, &m_cachedX[j]);
		RuSSE_LoadFourFloats(&position[i].y, &position[i + 1].y, &position[i + 2].y, &position[i + 3].y, &m_cachedY[j]);
		RuSSE_LoadFourFloats(&position[i].z, &position[i + 1].z, &position[i + 2].z, &position[i + 3].z, &m_cachedZ[j]);

		RuSSE_LoadFourFloats(&normal[i].x, &normal[i + 1].x, &normal[i + 2].x, &normal[i + 3].x, &m_cachedNX[j]);
		RuSSE_LoadFourFloats(&normal[i].y, &normal[i + 1].y, &normal[i + 2].y, &normal[i + 3].y, &m_cachedNY[j]);
		RuSSE_LoadFourFloats(&normal[i].z, &normal[i + 1].z, &normal[i + 2].z, &normal[i + 3].z, &m_cachedNZ[j]);
	}

	CRuVector3 leftoverPosition[4];
	CRuVector3 leftoverNormal[4];
	for(INT32 k = 0; i < m_srcMesh->GetNumVertices(); ++i, ++k)
	{
		leftoverPosition[k] = position[i];
		leftoverNormal[k] = normal[i];
	}

	RuSSE_LoadFourFloats(&leftoverPosition[0].x, &leftoverPosition[1].x, &leftoverPosition[2].x, &leftoverPosition[3].x, &m_cachedX[j]);
	RuSSE_LoadFourFloats(&leftoverPosition[0].y, &leftoverPosition[1].y, &leftoverPosition[2].y, &leftoverPosition[3].y, &m_cachedY[j]);
	RuSSE_LoadFourFloats(&leftoverPosition[0].z, &leftoverPosition[1].z, &leftoverPosition[2].z, &leftoverPosition[3].z, &m_cachedZ[j]);

	RuSSE_LoadFourFloats(&leftoverNormal[0].x, &leftoverNormal[1].x, &leftoverNormal[2].x, &leftoverNormal[3].x, &m_cachedNX[j]);
	RuSSE_LoadFourFloats(&leftoverNormal[0].y, &leftoverNormal[1].y, &leftoverNormal[2].y, &leftoverNormal[3].y, &m_cachedNY[j]);
	RuSSE_LoadFourFloats(&leftoverNormal[0].z, &leftoverNormal[1].z, &leftoverNormal[2].z, &leftoverNormal[3].z, &m_cachedNZ[j]);

	m_cachedBlendIndex = (UINT16 *) malloc(m_numVertexGroups * 4 * sizeof(UINT16) * 4);
	m_cachedBlendWeight = (float *) malloc(m_numVertexGroups * 4 * sizeof(float) * 4);

	memset(m_cachedBlendIndex, 0, m_numVertexGroups * 4 * sizeof(UINT16) * 4);
	memset(m_cachedBlendWeight, 0, m_numVertexGroups * 4 * sizeof(float) * 4);

	if(m_srcMesh->GetMorphTarget(0)->GetBlendIndex())
	{
		memcpy(m_cachedBlendIndex, m_srcMesh->GetMorphTarget(0)->GetBlendIndex(), m_srcMesh->GetNumVertices() * sizeof(UINT16) * 4);
	}

	if(m_srcMesh->GetMorphTarget(0)->GetBlendWeight())
	{
		memcpy(m_cachedBlendWeight, m_srcMesh->GetMorphTarget(0)->GetBlendWeight(), m_srcMesh->GetNumVertices() * sizeof(float) * 4);
	}

	// Copy morph target zero into the skinned morph target
	m_skinnedMT->CopyFrom(m_srcMesh->GetMorphTarget(0));
}

void CRuSkinnableMesh::SetPose(CRuMatrix4x4 *skinMatrices, INT32 numSkinMatrices)
{
	if(m_setPoseEnabled == FALSE)
		return;

	m_setPoseEnabled = FALSE;

	// Resize the cached bone matrix array if necessary
	if(m_cachedBoneMatrixArraySize < numSkinMatrices)
	{
		m_cachedBoneMatrixArraySize = numSkinMatrices;
		m_cachedBoneMatrixArray = (__m128 *) _aligned_malloc(numSkinMatrices * 3 * sizeof(__m128), 16);
	}

	// Fill bone matrix array
	for(INT32 i = 0, j = 0; i < numSkinMatrices; ++i)
	{
		RuSSE_LoadFourFloats(&skinMatrices[i]._11, &skinMatrices[i]._21, &skinMatrices[i]._31, &skinMatrices[i]._41, &m_cachedBoneMatrixArray[j++]);
		RuSSE_LoadFourFloats(&skinMatrices[i]._12, &skinMatrices[i]._22, &skinMatrices[i]._32, &skinMatrices[i]._42, &m_cachedBoneMatrixArray[j++]);
		RuSSE_LoadFourFloats(&skinMatrices[i]._13, &skinMatrices[i]._23, &skinMatrices[i]._33, &skinMatrices[i]._43, &m_cachedBoneMatrixArray[j++]);
	}

	__m128 collapsedMatrices[12];
	CRuVector3 *position = m_skinnedMT->GetPosition();
	CRuVector3 *normal = m_skinnedMT->GetNormal();

	const UINT16 *blendIndex = m_cachedBlendIndex;
	const float *blendWeight = m_cachedBlendWeight;

	if(blendWeight && blendIndex)
	{
		for(INT32 i = 0, j = 0, b0 = 0, b1 = 1, b2 = 2, b3 = 3; i < m_numVertexGroups; ++i, j += 4)
		{
			RuSSE_Collapse4Mat(&m_cachedBoneMatrixArray[blendIndex[b0] * 3], &m_cachedBoneMatrixArray[blendIndex[b1] * 3], &m_cachedBoneMatrixArray[blendIndex[b2] * 3], &m_cachedBoneMatrixArray[blendIndex[b3] * 3], blendWeight[b0], blendWeight[b1], blendWeight[b2], blendWeight[b3], &collapsedMatrices[0]);
			b0 += 4;
			b1 += 4;
			b2 += 4;
			b3 += 4;

			RuSSE_Collapse4Mat(&m_cachedBoneMatrixArray[blendIndex[b0] * 3], &m_cachedBoneMatrixArray[blendIndex[b1] * 3], &m_cachedBoneMatrixArray[blendIndex[b2] * 3], &m_cachedBoneMatrixArray[blendIndex[b3] * 3], blendWeight[b0], blendWeight[b1], blendWeight[b2], blendWeight[b3], &collapsedMatrices[3]);
			b0 += 4;
			b1 += 4;
			b2 += 4;
			b3 += 4;

			RuSSE_Collapse4Mat(&m_cachedBoneMatrixArray[blendIndex[b0] * 3], &m_cachedBoneMatrixArray[blendIndex[b1] * 3], &m_cachedBoneMatrixArray[blendIndex[b2] * 3], &m_cachedBoneMatrixArray[blendIndex[b3] * 3], blendWeight[b0], blendWeight[b1], blendWeight[b2], blendWeight[b3], &collapsedMatrices[6]);
			b0 += 4;
			b1 += 4;
			b2 += 4;
			b3 += 4;

			RuSSE_Collapse4Mat(&m_cachedBoneMatrixArray[blendIndex[b0] * 3], &m_cachedBoneMatrixArray[blendIndex[b1] * 3], &m_cachedBoneMatrixArray[blendIndex[b2] * 3], &m_cachedBoneMatrixArray[blendIndex[b3] * 3], blendWeight[b0], blendWeight[b1], blendWeight[b2], blendWeight[b3], &collapsedMatrices[9]);
			b0 += 4;
			b1 += 4;
			b2 += 4;
			b3 += 4;

			if(m_ignoreNormalTransforms)
			{
				RuSSE_Vert4Mat4Normalise(&collapsedMatrices[0], &m_cachedX[i], &m_cachedY[i], &m_cachedZ[i], &position[j]);
			}
			else
			{
				RuSSE_Vert4Mat4Normalise(&collapsedMatrices[0], &m_cachedX[i], &m_cachedY[i], &m_cachedZ[i], &m_cachedNX[i], &m_cachedNY[i], &m_cachedNZ[i], &position[j], &normal[j]);
			}
		}

		// Increment iteration
		++m_iterationNumber;
	}
}

INT32 CRuSkinnableMesh::GetMeshIteration() const
{
	return m_iterationNumber;
}

BOOL CRuSkinnableMesh::IsMeshDynamic() const
{
	return TRUE;
}

INT32 CRuSkinnableMesh::GetTopologyIteration() const
{
	return m_srcMesh->GetTopologyIteration();
}

BOOL CRuSkinnableMesh::IsTopologyDynamic() const
{
	return m_srcMesh->IsTopologyDynamic();
}

INT32 CRuSkinnableMesh::GetNumMorphTargets() const
{
	return 1;
}

IRuMorphTarget *CRuSkinnableMesh::GetMorphTarget(INT32 morphTargetIndex)
{
	return m_skinnedMT;
}

BOOL CRuSkinnableMesh::SetMorphCurveTime(REAL morphCurveTime)
{
	return TRUE;
}

const UINT16 *CRuSkinnableMesh::GetIndices() const
{
	return m_srcMesh->GetIndices();
}

BOOL CRuSkinnableMesh::GetBatch(INT32 index, INT32 &offsetOut, INT32 &numPrimitivesOut) const
{
	return m_srcMesh->GetBatch(index, offsetOut, numPrimitivesOut);
}

CRuParameterBlock *CRuSkinnableMesh::GetBatchParameterBlock(INT32 index)
{
	return m_srcMesh->GetBatchParameterBlock(index);
}

RuPrimitiveType CRuSkinnableMesh::GetPrimitiveType() const
{
	return m_srcMesh->GetPrimitiveType();
}

INT32 CRuSkinnableMesh::GetNumPrimitives() const
{
	return m_srcMesh->GetNumPrimitives();
}

INT32 CRuSkinnableMesh::GetNumVertices() const
{
	return m_srcMesh->GetNumVertices();
}

INT32 CRuSkinnableMesh::GetNumIndices() const
{
	return m_srcMesh->GetNumIndices();
}

const CRuSphere &CRuSkinnableMesh::GetBounds() const
{
	return m_srcMesh->GetBounds();
}
DWORD CRuSkinnableMesh::GetVertexFormat() const
{
	return m_srcMesh->GetVertexFormat();
}

BOOL CRuSkinnableMesh::QueryCollision(CRuCollisionQuery *colQuery)
{
	return TRUE;
}

void CRuSkinnableMesh::FreeResources()
{
	// Release reference to source mesh
	ruSAFE_RELEASE(m_srcMesh);

	// Release reference to the skinned morph target
	ruSAFE_RELEASE(m_skinnedMT);

	// Free SSE-optimized vertex and normal arrays
	if(m_cachedX)
		_aligned_free(m_cachedX);

	if(m_cachedY)
		_aligned_free(m_cachedY);

	if(m_cachedZ)
		_aligned_free(m_cachedZ);

	if(m_cachedNX)
		_aligned_free(m_cachedNX);

	if(m_cachedNY)
		_aligned_free(m_cachedNY);

	if(m_cachedNZ)
		_aligned_free(m_cachedNZ);

	if(m_cachedBoneMatrixArray)
		_aligned_free(m_cachedBoneMatrixArray);

	if(m_cachedBlendIndex)
		free(m_cachedBlendIndex);

	if(m_cachedBlendWeight)
		free(m_cachedBlendWeight);

	m_cachedX = NULL;
	m_cachedY = NULL;
	m_cachedZ = NULL;
	m_cachedNX = NULL;
	m_cachedNY = NULL;
	m_cachedNZ = NULL;
	m_cachedBlendIndex = NULL;
	m_cachedBlendWeight = NULL;
	m_cachedBoneMatrixArraySize = 0;
	m_cachedBoneMatrixArray = NULL;
}

// ************************************************************************************************************************************************************

CRuDynamicMesh::CRuDynamicMesh(BOOL forceStaticPool)
:	m_forceStaticPool(forceStaticPool),
	m_actualNumVertices(0),
	m_actualNumIndices(0)
{
}

CRuDynamicMesh::~CRuDynamicMesh()
{
}

IRuObject *CRuDynamicMesh::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuDynamicMesh();

	// Chain cloning call to parent class
	CRuMesh::Clone(clonedObject);

	// Shortcut
	CRuDynamicMesh *clonedMesh = static_cast<CRuDynamicMesh *>(clonedObject);

	// Copy dynamic mesh properties
	clonedMesh->m_forceStaticPool = m_forceStaticPool;
	clonedMesh->m_actualNumVertices = m_actualNumVertices;
	clonedMesh->m_actualNumIndices = m_actualNumIndices;

	return clonedObject;
}

BOOL CRuDynamicMesh::IsMeshDynamic() const
{
	if(m_forceStaticPool)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CRuDynamicMesh::IsTopologyDynamic() const
{
	if(m_forceStaticPool)
	{
		return FALSE;
	}

	return TRUE;
}

INT32 CRuDynamicMesh::GetMaxPrimitives() const
{
	RuPrimitiveType primitiveType = m_meshTopology->GetPrimitiveType();
	INT32 numIndices = m_meshTopology->GetNumIndices();

	// Return maximum number of allowed primitives by this mesh
	switch(primitiveType)
	{
		case ruPRIMTYPE_TRIANGLELIST:
			return numIndices / 3;

		case ruPRIMTYPE_TRIANGLEFAN:
			return numIndices - 2;

		case ruPRIMTYPE_LINELIST:
			return numIndices / 2;
	}

	return 0;
}

INT32 CRuDynamicMesh::GetMaxVertices() const
{
	// Return maximum number of allowed vertices by this mesh
	return m_numVertices;
}

INT32 CRuDynamicMesh::GetMaxIndices() const
{
	// Return maximum number of allowed indices by this mesh
	return m_meshTopology->GetNumIndices();
}

INT32 CRuDynamicMesh::GetNumPrimitives() const
{
	RuPrimitiveType primitiveType = m_meshTopology->GetPrimitiveType();

	switch(primitiveType)
	{
		case ruPRIMTYPE_TRIANGLELIST:
			return m_actualNumIndices / 3;

		case ruPRIMTYPE_TRIANGLEFAN:
			return m_actualNumIndices - 2;

		case ruPRIMTYPE_LINELIST:
			return m_actualNumIndices / 2;
	}

	return 0;
}

INT32 CRuDynamicMesh::GetNumVertices() const
{
	return m_actualNumVertices;
}

INT32 CRuDynamicMesh::GetNumIndices() const
{
	return m_actualNumIndices;
}

BOOL CRuDynamicMesh::SetNumPrimitives(INT32 numPrimitives)
{
	RuPrimitiveType primitiveType = m_meshTopology->GetPrimitiveType();
	INT32 maxIndices = m_meshTopology->GetNumIndices();

	// Determine the actual number of indices required by the number of primitives
	INT32 numIndices = 0;
	switch(primitiveType)
	{
		case ruPRIMTYPE_TRIANGLELIST:
			numIndices = numPrimitives * 3;
			break;

		case ruPRIMTYPE_TRIANGLEFAN:
			numIndices = numPrimitives + 2;
			break;

		case ruPRIMTYPE_LINELIST:
			numIndices = numPrimitives * 2;
			break;
	}

	// Only set the index count if it is within the maximum size limit
	if(numIndices > 0 && maxIndices >= numIndices)
	{
		// Set number of indices actually used
		m_actualNumIndices = numIndices;

		// Update default batch with the new number of primitives
		m_meshTopology->SetBatch(0, 0, numPrimitives);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuDynamicMesh::SetNumVertices(INT32 numVertices)
{
	if(m_numVertices >= numVertices)
	{
		m_actualNumVertices = numVertices;

		return TRUE;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

CRuMorphingMesh::CRuMorphingMesh()
:	m_curveTimeScale(1.0f),
	m_positionCurves(NULL),
	m_normalCurves(NULL),
	m_diffuseCurves(NULL)
{
}

CRuMorphingMesh::~CRuMorphingMesh()
{
	if(m_positionCurves)
	{
		for(INT32 i = 0; i < m_numVertices; ++i)
		{
			if(m_positionCurves[i])
			{
				GrannyFreeCurve(m_positionCurves[i]);
			}
		}

		delete[] m_positionCurves;
	}

	if(m_normalCurves)
	{
		for(INT32 i = 0; i < m_numVertices; ++i)
		{
			if(m_normalCurves[i])
			{
				GrannyFreeCurve(m_normalCurves[i]);
			}
		}

		delete[] m_normalCurves;
	}

	if(m_diffuseCurves)
	{
		for(INT32 i = 0; i < m_numVertices; ++i)
		{
			if(m_diffuseCurves[i])
			{
				GrannyFreeCurve(m_diffuseCurves[i]);
			}
		}

		delete[] m_diffuseCurves;
	}

	for(INT32 i = 0; i < m_textureCurves.Count(); ++i)
	{
		for(INT32 j = 0; j < m_numVertices; ++j)
		{
			if(m_textureCurves[i][j])
			{
				GrannyFreeCurve(m_textureCurves[i][j]);
			}
		}

		delete[] m_textureCurves[i];
	}
}

BOOL CRuMorphingMesh::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}
	if(classGUID != ClassGUID())
	{
		// Seek back
		stream->Seek(-((INT32) classGUID.Size()), ruSSM_Current);

		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuMesh::SerializeFrom(stream);

	// Read curve time scale
	stream->Read(&m_curveTimeScale, sizeof(REAL));

	CRuMemoryStream *tempStream = ruNEW CRuMemoryStream();
	tempStream->SetBufferSize(65536);

	BOOL hasPositionCurves = FALSE;
	stream->Read(&hasPositionCurves, sizeof(BOOL));

	if(hasPositionCurves)
	{
		m_positionCurves = ruNEW granny_curve2 * [m_numVertices];
		memset(m_positionCurves, 0, sizeof(granny_curve2 *) * m_numVertices);

		for(INT32 i = 0; i < m_numVertices; ++i)
		{
			INT32 curveStreamSize = 0;
			stream->Read(&curveStreamSize, sizeof(INT32));

			if(curveStreamSize > 0)
			{
				tempStream->SetStreamSize(curveStreamSize);
				stream->Read(tempStream->GetBuffer(), curveStreamSize);

				m_positionCurves[i] = ConvertStreamToCurve(tempStream);
			}
		}
	}

	BOOL hasNormalCurves = FALSE;
	stream->Read(&hasNormalCurves, sizeof(BOOL));

	if(hasNormalCurves)
	{
		m_normalCurves = ruNEW granny_curve2 * [m_numVertices];
		memset(m_normalCurves, 0, sizeof(granny_curve2 *) * m_numVertices);

		for(INT32 i = 0; i < m_numVertices; ++i)
		{
			INT32 curveStreamSize = 0;
			stream->Read(&curveStreamSize, sizeof(INT32));

			if(curveStreamSize > 0)
			{
				tempStream->SetStreamSize(curveStreamSize);
				stream->Read(tempStream->GetBuffer(), curveStreamSize);

				m_normalCurves[i] = ConvertStreamToCurve(tempStream);
			}
		}
	}

	BOOL hasDiffuseCurves = FALSE;
	stream->Read(&hasDiffuseCurves, sizeof(BOOL));

	if(hasDiffuseCurves)
	{
		m_diffuseCurves = ruNEW granny_curve2 * [m_numVertices];
		memset(m_diffuseCurves, 0, sizeof(granny_curve2 *) * m_numVertices);

		for(INT32 i = 0; i < m_numVertices; ++i)
		{
			INT32 curveStreamSize = 0;
			stream->Read(&curveStreamSize, sizeof(INT32));

			if(curveStreamSize > 0)
			{
				tempStream->SetStreamSize(curveStreamSize);
				stream->Read(tempStream->GetBuffer(), curveStreamSize);

				m_diffuseCurves[i] = ConvertStreamToCurve(tempStream);
			}
		}
	}

	INT32 numTexChannelCurves = 0;
	stream->Read(&numTexChannelCurves, sizeof(INT32));

	for(INT32 i = 0; i < numTexChannelCurves; ++i)
	{
		// Initialize to NULL
		m_textureChannels.Add(ruTEXCHANNEL_INVALID);
		m_textureCurves.Add(NULL);

		// Read texture channel
		stream->Read(&m_textureChannels[i], sizeof(RuTextureChannel));

		BOOL hasCurves = FALSE;
		stream->Read(&hasCurves, sizeof(BOOL));

		if(hasCurves)
		{
			m_textureCurves[i] = ruNEW granny_curve2 * [m_numVertices];
			memset(m_textureCurves[i], 0, sizeof(granny_curve2 *) * m_numVertices);

			for(INT32 j = 0; j < m_numVertices; ++j)
			{
				INT32 curveStreamSize = 0;
				stream->Read(&curveStreamSize, sizeof(INT32));

				if(curveStreamSize > 0)
				{
					tempStream->SetStreamSize(curveStreamSize);
					stream->Read(tempStream->GetBuffer(), curveStreamSize);

					m_textureCurves[i][j] = ConvertStreamToCurve(tempStream);
				}
			}
		}
	}

	// Free temporary memory stream
	delete tempStream;

	return TRUE;
}

BOOL CRuMorphingMesh::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuMesh::SerializeTo(stream);

	// Write curve time scale
	stream->Write(&m_curveTimeScale, sizeof(REAL));

	// Write position curves
	if(m_positionCurves)
	{
		BOOL hasPositionCurves = TRUE;
		stream->Write(&hasPositionCurves, sizeof(BOOL));

		for(INT32 i = 0; i < m_numVertices; ++i)
		{
			if(m_positionCurves[i])
			{
				CRuMemoryStream *curveStream = ConvertCurveToStream(m_positionCurves[i]);

				INT32 curveStreamSize = curveStream->GetStreamSize();
				stream->Write(&curveStreamSize, sizeof(INT32));
				stream->Write(curveStream->GetBuffer(), curveStreamSize);

				delete curveStream;
			}
			else
			{
				INT32 curveStreamSize = 0;
				stream->Write(&curveStreamSize, sizeof(INT32));
			}
		}
	}
	else
	{
		BOOL hasPositionCurves = FALSE;
		stream->Write(&hasPositionCurves, sizeof(BOOL));
	}

	// Write normal curves
	if(m_normalCurves)
	{
		BOOL hasNormalCurves = TRUE;
		stream->Write(&hasNormalCurves, sizeof(BOOL));

		for(INT32 i = 0; i < m_numVertices; ++i)
		{
			if(m_normalCurves[i])
			{
				CRuMemoryStream *curveStream = ConvertCurveToStream(m_normalCurves[i]);

				INT32 curveStreamSize = curveStream->GetStreamSize();
				stream->Write(&curveStreamSize, sizeof(INT32));
				stream->Write(curveStream->GetBuffer(), curveStreamSize);

				delete curveStream;
			}
			else
			{
				INT32 curveStreamSize = 0;
				stream->Write(&curveStreamSize, sizeof(INT32));
			}
		}
	}
	else
	{
		BOOL hasNormalCurves = FALSE;
		stream->Write(&hasNormalCurves, sizeof(BOOL));
	}

	// Write diffuse curves
	if(m_diffuseCurves)
	{
		BOOL hasDiffuseCurves = TRUE;
		stream->Write(&hasDiffuseCurves, sizeof(BOOL));

		for(INT32 i = 0; i < m_numVertices; ++i)
		{
			if(m_diffuseCurves[i])
			{
				CRuMemoryStream *curveStream = ConvertCurveToStream(m_diffuseCurves[i]);

				INT32 curveStreamSize = curveStream->GetStreamSize();
				stream->Write(&curveStreamSize, sizeof(INT32));
				stream->Write(curveStream->GetBuffer(), curveStreamSize);

				delete curveStream;
			}
			else
			{
				INT32 curveStreamSize = 0;
				stream->Write(&curveStreamSize, sizeof(INT32));
			}
		}
	}
	else
	{
		BOOL hasDiffuseCurves = FALSE;
		stream->Write(&hasDiffuseCurves, sizeof(BOOL));
	}

	// Write number of texture channel curves
	INT32 numTexChannelCurves = m_textureChannels.Count();
	stream->Write(&numTexChannelCurves, sizeof(INT32));

	for(INT32 i = 0; i < numTexChannelCurves; ++i)
	{
		// Write texture channel
		stream->Write(&m_textureChannels[i], sizeof(RuTextureChannel));

		// Write texture curves
		if(m_textureCurves[i])
		{
			BOOL hasCurves = TRUE;
			stream->Write(&hasCurves, sizeof(BOOL));

			for(INT32 j = 0; j < m_numVertices; ++j)
			{
				if(m_textureCurves[i][j])
				{
					CRuMemoryStream *curveStream = ConvertCurveToStream(m_textureCurves[i][j]);

					INT32 curveStreamSize = curveStream->GetStreamSize();
					stream->Write(&curveStreamSize, sizeof(INT32));
					stream->Write(curveStream->GetBuffer(), curveStreamSize);

					delete curveStream;
				}
				else
				{
					INT32 curveStreamSize = 0;
					stream->Write(&curveStreamSize, sizeof(INT32));
				}
			}
		}
		else
		{
			BOOL hasCurves = FALSE;
			stream->Write(&hasCurves, sizeof(BOOL));
		}
	}

	return TRUE;
}

BOOL CRuMorphingMesh::SetCurveTimeScale(REAL curveTimeScale)
{
	m_curveTimeScale = curveTimeScale;
	return TRUE;
}

BOOL CRuMorphingMesh::SetPositionCurve(INT32 vertexIndex, granny_curve2 *curve)
{
	if(vertexIndex >= 0 && vertexIndex < m_numVertices && curve)
	{
		// Allocate curve array if it has not already been allocated
		if(m_positionCurves == NULL)
		{
			m_positionCurves = ruNEW granny_curve2 * [m_numVertices];
			memset(m_positionCurves, 0, sizeof(granny_curve2 *) * m_numVertices);
		}

		// Free existing curve
		if(m_positionCurves[vertexIndex])
		{
			GrannyFreeCurve(m_positionCurves[vertexIndex]);
			m_positionCurves[vertexIndex] = NULL;
		}

		// Copy Curve
		granny_curve_builder *builder = GrannyBeginCurveCopy(curve);
		m_positionCurves[vertexIndex] = GrannyEndCurve(builder);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuMorphingMesh::SetNormalCurve(INT32 vertexIndex, granny_curve2 *curve)
{
	if(vertexIndex >= 0 && vertexIndex < m_numVertices && curve)
	{
		// Allocate curve array if it has not already been allocated
		if(m_normalCurves == NULL)
		{
			m_normalCurves = ruNEW granny_curve2 * [m_numVertices];
			memset(m_normalCurves, 0, sizeof(granny_curve2 *) * m_numVertices);
		}

		// Free existing curve
		if(m_normalCurves[vertexIndex])
		{
			GrannyFreeCurve(m_normalCurves[vertexIndex]);
			m_normalCurves[vertexIndex] = NULL;
		}

		// Copy Curve
		granny_curve_builder *builder = GrannyBeginCurveCopy(curve);
		m_normalCurves[vertexIndex] = GrannyEndCurve(builder);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuMorphingMesh::SetDiffuseCurve(INT32 vertexIndex, granny_curve2 *curve)
{
	if(vertexIndex >= 0 && vertexIndex < m_numVertices && curve)
	{
		// Allocate curve array if it has not already been allocated
		if(m_diffuseCurves == NULL)
		{
			m_diffuseCurves = ruNEW granny_curve2 * [m_numVertices];
			memset(m_diffuseCurves, 0, sizeof(granny_curve2 *) * m_numVertices);
		}

		// Free existing curve
		if(m_diffuseCurves[vertexIndex])
		{
			GrannyFreeCurve(m_diffuseCurves[vertexIndex]);
			m_diffuseCurves[vertexIndex] = NULL;
		}

		// Copy Curve
		granny_curve_builder *builder = GrannyBeginCurveCopy(curve);
		m_diffuseCurves[vertexIndex] = GrannyEndCurve(builder);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuMorphingMesh::SetTextureCurve(RuTextureChannel texChannel, INT32 vertexIndex, granny_curve2 *curve)
{
	if(vertexIndex >= 0 && vertexIndex < m_numVertices && curve)
	{
		// Find out whether there is an existing texture channel
		INT32 texChanIndex = -1;
		for(INT32 i = 0; i < m_textureChannels.Count(); ++i)
		{
			if(m_textureChannels[i] == texChannel)
			{
				texChanIndex = i;
				break;
			}
		}

		if(texChanIndex == -1)
		{
			texChanIndex = m_textureChannels.Count();
			m_textureChannels.Add(texChannel);
			m_textureCurves.Add(ruNEW granny_curve2 * [m_numVertices]);
			memset(m_textureCurves[texChanIndex], 0, sizeof(granny_curve2 *) * m_numVertices);
		}

		// Free existing curve
		if(m_textureCurves[texChanIndex][vertexIndex])
		{
			GrannyFreeCurve(m_textureCurves[texChanIndex][vertexIndex]);
			m_textureCurves[texChanIndex][vertexIndex] = NULL;
		}

		// Copy Curve
		granny_curve_builder *builder = GrannyBeginCurveCopy(curve);
		m_textureCurves[texChanIndex][vertexIndex] = GrannyEndCurve(builder);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuMorphingMesh::IsMeshDynamic() const
{
	return TRUE;
}

BOOL CRuMorphingMesh::SetMorphCurveTime(REAL morphCurveTime)
{
	if(m_morphTargets.Count() == 0)
	{
		return FALSE;
	}

	// Enter critical section
	RuExtLink_Granny_GlobalCS()->Enter();

	// Re-sample morph target
	if(m_positionCurves)
	{
		CRuVector3 *position = m_morphTargets[0]->GetPosition();

		for(INT32 i = 0; i < m_numVertices; ++i)
		{
			INT32 dimension = 3;
			granny_triple result;

			if(m_positionCurves[i])
			{
				GrannyEvaluateCurveAtT(dimension, false, false, m_positionCurves[i], false, m_curveTimeScale, morphCurveTime, result, GrannyCurveIdentityPosition);
				position[i].Set(result[0], result[1], result[2]);
			}
		}
	}

	for(INT32 i = 0; i < m_textureCurves.Count(); ++i)
	{
		REAL *uv = m_morphTargets[0]->GetTextureCoordinate(m_textureChannels[i]);
		INT32 numComponents = RuTextureChannel_GetNumComponents(m_textureChannels[i]);

		REAL result[4];
		REAL identity[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		for(INT32 j = 0, k = 0; j < m_numVertices; ++j)
		{
			GrannyEvaluateCurveAtT(4, false, false, m_textureCurves[i][j], false, m_curveTimeScale, morphCurveTime, result, identity);

			uv[k++] = result[0];
			uv[k++] = result[1];
		}
	}

	// Leave critical section
	RuExtLink_Granny_GlobalCS()->Leave();

	this->IncrementMeshIteration();

	return TRUE;
}

CRuMemoryStream *CRuMorphingMesh::ConvertCurveToStream(granny_curve2 *curve)
{
	CRuMemoryStream *memStream = NULL;

	// Enter critical section
	RuExtLink_Granny_GlobalCS()->Enter();

	// Write curve out
	granny_file_data_tree_writer *dataTreeWriter = GrannyBeginFileDataTreeWriting(GrannyCurve2Type, curve, 0, 0);

	if(dataTreeWriter)
	{
		// Output to temporary file
		char tempPath[1024], tempFilename[MAX_PATH];
		GetTempPathA(1024, tempPath);
		GetTempFileName(tempPath, "grx", 0, tempFilename);

		if(GrannyWriteDataTreeToFile(dataTreeWriter, GrannyCurrentGRNStandardTag, GrannyGRNFileMV_32Bit_LittleEndian, tempFilename, 1) == false)
		{
		}

		GrannyEndFileDataTreeWriting(dataTreeWriter);

		// Read-back stream into memory
		CRuFileStream *fileStream = ruNEW CRuFileStream();
		fileStream->Open(tempFilename);

		memStream = ruNEW CRuMemoryStream();
		*memStream = *fileStream;

		delete fileStream;

		// Delete temporary file
		DeleteFileA(tempFilename);
	}

	// Leave critical section
	RuExtLink_Granny_GlobalCS()->Leave();

	return memStream;
}

granny_curve2 *CRuMorphingMesh::ConvertStreamToCurve(CRuMemoryStream *stream)
{
	granny_file *gfile = GrannyReadEntireFileFromMemory(stream->GetStreamSize(), stream->GetBuffer());

	granny_curve2 *convertedCurve = NULL;

	// Enter critical section
	RuExtLink_Granny_GlobalCS()->Enter();

	// Get curve data
	granny_variant gVariant;
	GrannyGetDataTreeFromFile(gfile, &gVariant);

	bool srcCurveIsLocalCopy = false;
	granny_curve2 *srcCurve;

	// Perform version check
	UINT32 fileTag = GrannyGetFileTypeTag(gfile);

	// Leave critical section
	RuExtLink_Granny_GlobalCS()->Leave();

	if(fileTag == GrannyCurrentGRNStandardTag)
	{
		// Same version, so we simply assign the pointer
		srcCurve = (granny_curve2 *) gVariant.Object;
	}
	else
	{
		// Old version, perform data conversion
		srcCurve = (granny_curve2 *) GrannyConvertTree(gVariant.Type, gVariant.Object, GrannyCurve2Type);
		srcCurveIsLocalCopy = true;
	}

	if(srcCurve->CurveData.Type != NULL)
	{
		// Copy curve data
		granny_curve_builder *builder = GrannyBeginCurveCopy(srcCurve);
		convertedCurve = GrannyEndCurve(builder);

		// After we copy the curve, we must replace the type definition, since Granny does not clone the type, and the
		// type used by data trees is freed when the file is freed.
		granny_data_type_definition *curveType = NULL;

		if(GrannyDataTypesAreEqual(srcCurve->CurveData.Type, GrannyCurveDataDaK32fC32fType))
		{
			curveType = GrannyCurveDataDaK32fC32fType;
		}
		else if(GrannyDataTypesAreEqual(srcCurve->CurveData.Type, GrannyCurveDataDaK16uC16uType))
		{
			curveType = GrannyCurveDataDaK16uC16uType;
		}
		else if(GrannyDataTypesAreEqual(srcCurve->CurveData.Type, GrannyCurveDataDaK8uC8uType))
		{
			curveType = GrannyCurveDataDaK8uC8uType;
		}
		else if(GrannyDataTypesAreEqual(srcCurve->CurveData.Type, GrannyCurveDataD3K16uC16uType))
		{
			curveType = GrannyCurveDataD3K16uC16uType;
		}
		else if(GrannyDataTypesAreEqual(srcCurve->CurveData.Type, GrannyCurveDataD3K8uC8uType))
		{
			curveType = GrannyCurveDataD3K8uC8uType;
		}
		else if(GrannyDataTypesAreEqual(srcCurve->CurveData.Type, GrannyCurveDataD4nK16uC15uType))
		{
			curveType = GrannyCurveDataD4nK16uC15uType;
		}
		else if(GrannyDataTypesAreEqual(srcCurve->CurveData.Type, GrannyCurveDataD4nK8uC7uType))
		{
			curveType = GrannyCurveDataD4nK8uC7uType;
		}
		else if(GrannyDataTypesAreEqual(srcCurve->CurveData.Type, GrannyCurveDataD9I1K16uC16uType))
		{
			curveType = GrannyCurveDataD9I1K16uC16uType;
		}
		else if(GrannyDataTypesAreEqual(srcCurve->CurveData.Type, GrannyCurveDataD9I3K16uC16uType))
		{
			curveType = GrannyCurveDataD9I3K16uC16uType;
		}
		else if(GrannyDataTypesAreEqual(srcCurve->CurveData.Type, GrannyCurveDataD9I1K8uC8uType))
		{
			curveType = GrannyCurveDataD9I1K8uC8uType;
		}
		else if(GrannyDataTypesAreEqual(srcCurve->CurveData.Type, GrannyCurveDataD9I3K8uC8uType))
		{
			curveType = GrannyCurveDataD9I3K8uC8uType;
		}
		else if(GrannyDataTypesAreEqual(srcCurve->CurveData.Type, GrannyCurveDataDaConstant32fType))
		{
			curveType = GrannyCurveDataDaConstant32fType;
		}
		else if(GrannyDataTypesAreEqual(srcCurve->CurveData.Type, GrannyCurveDataD3Constant32fType))
		{
			curveType = GrannyCurveDataD3Constant32fType;
		}
		else if(GrannyDataTypesAreEqual(srcCurve->CurveData.Type, GrannyCurveDataD4Constant32fType))
		{
			curveType = GrannyCurveDataD4Constant32fType;
		}

		assert(curveType);

		convertedCurve->CurveData.Type = curveType;
	}

	GrannyFreeFile(gfile);

	if(srcCurveIsLocalCopy)
	{
		GrannyFreeBuilderResult(srcCurve);
	}

	return convertedCurve;
}

// ************************************************************************************************************************************************************

CRuProxyMesh::CRuProxyMesh()
:	m_mesh(NULL),
	m_proxiedBatchIndex(-1),
	m_proxiedBatchOffset(0),
	m_proxiedBatchNumPrimitives(0),
	m_proxiedBatchParameterBlock(NULL),
	m_useProxiedMeshBounds(TRUE)
{
}

CRuProxyMesh::~CRuProxyMesh()
{
	// Release reference to source mesh
	ruSAFE_RELEASE(m_mesh);

	delete m_proxiedBatchParameterBlock;
}


BOOL CRuProxyMesh::SetProxiedMesh(IRuMesh *proxiedMesh, INT32 proxiedBatchIndex)
{
	// Release reference to prior source mesh
	ruSAFE_RELEASE(m_mesh);

	// Verify the proxy information is correct
	INT32 batchOffset;
	INT32 batchNumPrimitives;
	if(proxiedMesh->GetBatch(proxiedBatchIndex, batchOffset, batchNumPrimitives))
	{
		// Store proxy information
		m_mesh = proxiedMesh;
		m_proxiedBatchIndex = proxiedBatchIndex;

		// Increment reference count on the proxied mesh
		m_mesh->AddRef();

		return TRUE;
	}

	return FALSE;
}

BOOL CRuProxyMesh::SetProxiedMesh(IRuMesh *proxiedMesh, INT32 proxiedBatchOffset, INT32 proxiedBatchNumPrimitives)
{
	// Increment reference count on the proxied mesh
	proxiedMesh->AddRef();

	// Release reference to prior source mesh
	ruSAFE_RELEASE(m_mesh);

	// Store explicit batch offset and primitive count
	m_proxiedBatchIndex = -1;
	m_proxiedBatchOffset = proxiedBatchOffset;
	m_proxiedBatchNumPrimitives = proxiedBatchNumPrimitives;

	if(m_proxiedBatchParameterBlock == NULL)
		m_proxiedBatchParameterBlock = ruNEW CRuParameterBlock();

	// Store reference to proxied mesh
	m_mesh = proxiedMesh;

	return TRUE;
}

INT32 CRuProxyMesh::GetProxiedBatchIndex() const
{
	return m_proxiedBatchIndex;
}

INT32 CRuProxyMesh::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuMesh::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	footprint += sizeof(m_mesh) + (m_mesh ? m_mesh->GetMemoryFootprint(enumHash) : 0);
	footprint += sizeof(m_proxiedBatchIndex);

	footprint += sizeof(m_proxiedBatchOffset);
	footprint += sizeof(m_proxiedBatchNumPrimitives);
	footprint += sizeof(m_proxiedBatchParameterBlock);

	footprint += sizeof(m_useProxiedMeshBounds);
	footprint += sizeof(m_proxiedBatchBounds);

	return footprint;
}

const CRuGUID &CRuProxyMesh::GetGUID() const
{
	return m_mesh->GetGUID();
}

INT32 CRuProxyMesh::GetMeshIteration() const
{
	return m_mesh->GetMeshIteration();
}

BOOL CRuProxyMesh::IsMeshDynamic() const
{
	return m_mesh->IsMeshDynamic();
}

INT32 CRuProxyMesh::GetTopologyIteration() const
{
	return m_mesh->GetTopologyIteration();
}

BOOL CRuProxyMesh::IsTopologyDynamic() const
{
	return m_mesh->IsTopologyDynamic();
}

INT32 CRuProxyMesh::GetNumMorphTargets() const
{
	return m_mesh->GetNumMorphTargets();
}

IRuMorphTarget *CRuProxyMesh::GetMorphTarget(INT32 morphTargetIndex)
{
	return m_mesh->GetMorphTarget(morphTargetIndex);
}

BOOL CRuProxyMesh::SetMorphCurveTime(REAL morphCurveTime)
{
	return TRUE;
}

IRuMeshTopology *CRuProxyMesh::GetMeshTopology()
{
	return m_mesh->GetMeshTopology();
}

const IRuMeshTopology *CRuProxyMesh::GetMeshTopology() const
{
	return m_mesh->GetMeshTopology();
}

const UINT16 *CRuProxyMesh::GetIndices() const
{
	return m_mesh->GetIndices();
}

BOOL CRuProxyMesh::GetBatch(INT32 index, INT32 &offsetOut, INT32 &numPrimitivesOut) const
{
	if(index == 0)
	{
		if(m_proxiedBatchIndex >= 0)
		{
			return m_mesh->GetBatch(m_proxiedBatchIndex, offsetOut, numPrimitivesOut);
		}
		else
		{
			offsetOut = m_proxiedBatchOffset;
			numPrimitivesOut = m_proxiedBatchNumPrimitives;
			return TRUE;
		}
	}

	return FALSE;
}

CRuParameterBlock *CRuProxyMesh::GetBatchParameterBlock(INT32 index)
{
	if(index == 0)
	{
		if(m_proxiedBatchIndex >= 0)
		{
			return m_mesh->GetBatchParameterBlock(m_proxiedBatchIndex);
		}
		else
		{
			return m_proxiedBatchParameterBlock;
		}
	}

	return NULL;
}

RuPrimitiveType CRuProxyMesh::GetPrimitiveType() const
{
	return m_mesh->GetPrimitiveType();
}

INT32 CRuProxyMesh::GetNumPrimitives() const
{
	return m_mesh->GetNumPrimitives();
}

INT32 CRuProxyMesh::GetNumVertices() const
{
	return m_mesh->GetNumVertices();
}

INT32 CRuProxyMesh::GetNumIndices() const
{
	return m_mesh->GetNumIndices();
}

const CRuSphere &CRuProxyMesh::GetBounds() const
{
	if(m_useProxiedMeshBounds)
	{
		return m_mesh->GetBounds();
	}
	else
	{
		return m_proxiedBatchBounds;
	}
}

DWORD CRuProxyMesh::GetVertexFormat() const
{
	return m_mesh->GetVertexFormat();
}

BOOL CRuProxyMesh::QueryCollision(CRuCollisionQuery *colQuery)
{
	return TRUE;
}

void CRuProxyMesh::SetBounds(const CRuSphere &bounds)
{
	m_useProxiedMeshBounds = FALSE;
	m_proxiedBatchBounds = bounds;
}

// ************************************************************************************************************************************************************

CRuSkinMap::CRuSkinMap()
:	m_boneMap(NULL),
	m_invBoneMap(NULL)
{
}

CRuSkinMap::~CRuSkinMap()
{
	ruSAFE_DELETE_ARRAY(m_boneMap);
	ruSAFE_DELETE_ARRAY(m_invBoneMap);
}

BOOL CRuSkinMap::Initialize(IRuMesh *mesh)
{
	// Verify that the mesh has skinning data
	if(HasSkinningData(mesh) == FALSE)
	{
		return FALSE;
	}

	IRuMeshTopology *meshTopology = mesh->GetMeshTopology();
	IRuMorphTarget *morphTarget = mesh->GetMorphTarget(0);

	const INT32 numPrimitives = meshTopology->GetNumPrimitives();
	const INT32 indicesPerPrimitive = meshTopology->GetNumIndices() / numPrimitives;
	const INT32 numBones = GetBoneCount(mesh);

	REAL *blendWeight = morphTarget->GetBlendWeight();
	UINT16 *blendIndex = morphTarget->GetBlendIndex();

	// Build the bone usage map
	m_boneUsageMap.SetNumBits(numBones);
	m_boneUsageMap.ClearAll();

	for(INT32 i = 0, numBlendIndices = mesh->GetNumVertices() * 4; i < numBlendIndices; ++i)
	{
		if(blendWeight[i] > ruEPSILON)
		{
			m_boneUsageMap.Set(blendIndex[i]);
		}
	}

	// Update bone map
	UpdateBoneMap();

	return TRUE;
}

BOOL CRuSkinMap::Merge(CRuSkinMap *srcSkinMap, INT32 boneLimit)
{
	// Create a bone usage map large enough to hold the combined bone usage map
	CRuBitField combinedBoneUsageMap(max(this->GetBoneCount(), srcSkinMap->GetBoneCount()));

	// Merge bone usage map into the combined usage map
	for(INT32 i = 0, boneCount = m_boneUsageMap.GetNumBits(); i < boneCount; ++i)
	{
		if(m_boneUsageMap[i])
		{
			combinedBoneUsageMap.Set(i);
		}
	}

	// Merge source bone map into the combined usage map
	for(INT32 i = 0, boneCount = srcSkinMap->m_boneUsageMap.GetNumBits(); i < boneCount; ++i)
	{
		if(srcSkinMap->m_boneUsageMap[i])
		{
			combinedBoneUsageMap.Set(i);
		}
	}

	// Iterate through combined bone usage map and get a total bone count
	INT32 usedBoneCount = 0;
	for(INT32 i = 0, boneCount = combinedBoneUsageMap.GetNumBits(); i < boneCount; ++i)
	{
		if(combinedBoneUsageMap[i])
		{
			++usedBoneCount;
		}
	}

	// If used bone count exceeds the bone limit, abort
	if(boneLimit > 0 && usedBoneCount > boneLimit)
	{
		return FALSE;
	}

	// Copy combined bone usage map
	m_boneUsageMap = combinedBoneUsageMap;

	// Update bone map
	UpdateBoneMap();

	return TRUE;
}

BOOL CRuSkinMap::Split(INT32 boneLimit, CRuArrayList<CRuSkinMap *> &skinMapsOut)
{
	// If the used bone count is under the limit, return error
	if(GetUsedBoneCount() <= boneLimit)
	{
		return FALSE;
	}

	INT32 usedBoneCount = 0;
	for(INT32 i = 0, j = 0, boneCount = m_boneUsageMap.GetNumBits(); i < boneCount; ++i)
	{
		// Increment used bone count
		if(m_boneUsageMap[i])
		{
			++usedBoneCount;
		}

		// If we have reached the bone count limit or the end of the bone list, do some work
		if(usedBoneCount == boneLimit || (i + 1) == boneCount)
		{
			// Create new skin map
			CRuSkinMap *skinMap = ruNEW CRuSkinMap();

			// Set bone count to equal to the current skin map's bone count
			skinMap->m_boneUsageMap.SetNumBits(m_boneUsageMap.GetNumBits());
			skinMap->m_boneUsageMap.ClearAll();

			// Flag bone usage up to the bone limit
			for(INT32 k = j; k <= i; ++k)
			{
				skinMap->m_boneUsageMap.Set(k);
			}

			// Update bone map
			skinMap->UpdateBoneMap();

			// Add to output array
			skinMapsOut.Add(skinMap);

			// Reset starting index and used bone count
			j = i + 1;
			usedBoneCount = 0;
		}
	}

	return TRUE;
}

IRuMesh *CRuSkinMap::GenerateRemappedMesh(IRuMesh *mesh)
{
	return NULL;
}

INT32 CRuSkinMap::GetBoneCount()
{
	return m_boneUsageMap.GetNumBits();
}

INT32 CRuSkinMap::GetUsedBoneCount()
{
	return m_usedBoneCount;
}

BOOL CRuSkinMap::HasSkinningData(IRuMesh *mesh)
{
	IRuMorphTarget *morphTarget = mesh->GetMorphTarget(0);
	return (morphTarget && morphTarget->GetBlendIndex() && morphTarget->GetBlendWeight());
}

INT32 CRuSkinMap::GetBoneCount(IRuMesh *mesh)
{
	INT32 boneCount = 0;

	IRuMorphTarget *morphTarget = mesh->GetMorphTarget(0);
	if(morphTarget)
	{
		const UINT16 *blendIndex = morphTarget->GetBlendIndex();
		if(blendIndex)
		{
			// Iterate through each blend index and find the largest value
			INT32 numBlendIndices = mesh->GetNumVertices() * 4;
			for(INT32 i = 0; i < numBlendIndices; ++i)
			{
				boneCount = max(boneCount, blendIndex[i] + 1);
			}
		}
	}

	return boneCount;
}

BOOL CRuSkinMap::UpdateBoneMap()
{
	INT32 numBones = m_boneUsageMap.GetNumBits();

	// Update the forward and inverse bone map base on the bone usage map
	ruSAFE_DELETE_ARRAY(m_boneMap);
	m_boneMap = ruNEW UINT16 [numBones];
	memset(m_boneMap, 0, sizeof(UINT16) * numBones);

	ruSAFE_DELETE_ARRAY(m_invBoneMap);
	m_invBoneMap = ruNEW UINT16 [numBones];
	memset(m_invBoneMap, 0, sizeof(UINT16) * numBones);

	m_usedBoneCount = 0;

	for(INT32 i = 0, j = 0; i < numBones; ++i)
	{
		if(m_boneUsageMap[i])
		{
			m_boneMap[i] = j;
			m_invBoneMap[j] = i;

			++m_usedBoneCount;

			++j;
		}
		else
		{
			m_boneMap[i] = 0;
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
