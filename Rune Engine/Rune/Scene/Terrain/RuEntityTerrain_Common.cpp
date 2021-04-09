#include "../../Scene/Terrain/RuEntityTerrain_Common.h"

#include "../../Engine/Texture/RuTexture_Proxy.h"
#include "../../Engine/Texture/RuTextureUtility.h"

#include <dxtlib/dxtlib.h>
//#include "../../../Tools/ati_compress/ATI_Compress.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuSplatIDMap, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuSplatIDMap, "CRuSplatIDMap", "IRuObject")
ruCLASSGUID_DEFINE(CRuSplatIDMap, 0x7CD84C24, 0x7D9841cb, 0xB3B04ECB, 0xEA72A60F)

ruRTTI_DEFINE_SUBCLASS(CRuSplatMapBuilder, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuSplatMapBuilder, "CRuSplatMapBuilder", "IRuObject")
ruCLASSGUID_DEFINE(CRuSplatMapBuilder, 0xB2566073, 0x8BB94243, 0xABD39B2A, 0xF218782E)

ruRTTI_DEFINE_SUBCLASS(CRuTextureList, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuTextureList, "CRuTextureList", "IRuObject")
ruCLASSGUID_DEFINE(CRuTextureList, 0x2AE2F39B, 0x53D74887, 0x8A59457A, 0x5475DC5F)

// ************************************************************************************************************************************************************

CRuTerrainUtility_ChunkInfo::CRuTerrainUtility_ChunkInfo(CRuVector3 *normalField, BYTE *autoPaintField, CRuVector3 &origin, INT32 fieldWidth, REAL gridSize)
:	m_normalField(normalField),
	m_autoPaintField(autoPaintField),
	m_origin(origin),
	m_fieldWidth(fieldWidth),
	m_gridSize(gridSize)
{
}

REAL CRuTerrainUtility_ChunkInfo::EvaluateSlope(const CRuVector3 &position, REAL range)
{
	CRuVector3 upVec(0.0f, 1.0f, 0.0f);
	CRuVector3 relPos = position - m_origin;

	INT32 iX0 = min(m_fieldWidth, max(0, static_cast<INT32>(relPos.x / m_gridSize)));
	INT32 iZ0 = min(m_fieldWidth, max(0, static_cast<INT32>(relPos.z / m_gridSize)));
	INT32 iX1 = (iX0 == m_fieldWidth) ? m_fieldWidth : iX0 + 1;
	INT32 iZ1 = (iZ0 == m_fieldWidth) ? m_fieldWidth : iZ0 + 1;

	REAL p0 = 1.0f - DotProduct(upVec, m_normalField[iX0 + iZ0 * (m_fieldWidth + 1)]);
	REAL p1 = 1.0f - DotProduct(upVec, m_normalField[iX1 + iZ0 * (m_fieldWidth + 1)]);
	REAL p2 = 1.0f - DotProduct(upVec, m_normalField[iX0 + iZ1 * (m_fieldWidth + 1)]);
	REAL p3 = 1.0f - DotProduct(upVec, m_normalField[iX1 + iZ1 * (m_fieldWidth + 1)]);

	REAL p0Weight = m_gridSize - max(fabs(iX0 * m_gridSize - relPos.x), fabs(iZ0 * m_gridSize - relPos.z));
	REAL p1Weight = m_gridSize - max(fabs(iX1 * m_gridSize - relPos.x), fabs(iZ0 * m_gridSize - relPos.z));
	REAL p2Weight = m_gridSize - max(fabs(iX0 * m_gridSize - relPos.x), fabs(iZ1 * m_gridSize - relPos.z));
	REAL p3Weight = m_gridSize - max(fabs(iX1 * m_gridSize - relPos.x), fabs(iZ1 * m_gridSize - relPos.z));

	return (p0 * p0Weight + p1 * p1Weight + p2 * p2Weight + p3 * p3Weight) / (p0Weight + p1Weight + p2Weight + p3Weight);
}

BOOL CRuTerrainUtility_ChunkInfo::PaintOK(const CRuVector3 &position)
{
	CRuVector3 relPos = position - m_origin;

	relPos.x += m_gridSize;
	relPos.z += m_gridSize;

	INT32 iX0 = min(m_fieldWidth + 2, max(0, static_cast<INT32>(relPos.x / m_gridSize)));
	INT32 iZ0 = min(m_fieldWidth + 2, max(0, static_cast<INT32>(relPos.z / m_gridSize)));
	INT32 iX1 = (iX0 == m_fieldWidth + 2) ? m_fieldWidth + 2 : iX0 + 1;
	INT32 iZ1 = (iZ0 == m_fieldWidth + 2) ? m_fieldWidth + 2 : iZ0 + 1;

	INT32 changeCount = 0;

	if(m_autoPaintField[iX0 + iZ0 * (m_fieldWidth + 3)])
	{
		++changeCount;
	}

	if(m_autoPaintField[iX1 + iZ0 * (m_fieldWidth + 3)])
	{
		++changeCount;
	}

	if(m_autoPaintField[iX0 + iZ1 * (m_fieldWidth + 3)])
	{
		++changeCount;
	}

	if(m_autoPaintField[iX1 + iZ1 * (m_fieldWidth + 3)])
	{
		++changeCount;
	}

	if(changeCount > 0)
	{
		return TRUE;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

CRuImageConvolver::CRuImageConvolver(INT32 kernelSize, float *kernel, INT32 imageSize)
{
	// Store kernel
	m_kernelSize = kernelSize;
	m_kernel = ruNEW float [m_kernelSize];
	memcpy(m_kernel, kernel, sizeof(float) * m_kernelSize);

	// Initialize image
	m_dimension = imageSize;
	m_paddingSize = m_kernelSize / 2;
	m_realDimension = m_dimension + m_paddingSize * 2;
	m_image = ruNEW UINT8 [m_realDimension * m_realDimension];
	m_workBuffer = ruNEW UINT8 [m_realDimension];

	// Clear memory
	memset(m_image, 0, sizeof(UINT8) * m_realDimension * m_realDimension);
	memset(m_workBuffer, 0, sizeof(UINT8) * m_realDimension);
}

CRuImageConvolver::~CRuImageConvolver()
{
	delete[] m_kernel;
	delete[] m_image;
	delete[] m_workBuffer;
}

void CRuImageConvolver::ClearImageData()
{
	if(m_image)
	{
		memset(m_image, 0, sizeof(UINT8) * m_realDimension * m_realDimension);
	}
}

UINT8 *CRuImageConvolver::GetImageData()
{
	return &m_image[m_paddingSize * m_realDimension + m_paddingSize];
}

UINT8 *CRuImageConvolver::GetRawImageData()
{
	return m_image;
}

INT32 CRuImageConvolver::GetImagePitch()
{
	return m_realDimension;
}

BOOL CRuImageConvolver::Convolve()
{
	// Convolution along X
	for(INT32 y = 0; y < m_dimension; ++y)
	{
		INT32 srcOffset = (m_paddingSize + y) * m_realDimension;
		INT32 destOffset = m_paddingSize;

		for(INT32 x = 0; x < m_dimension; ++x, ++srcOffset, ++ destOffset)
		{
			// Convolve and accumulate
			float sum = 0.0f;

			for(INT32 i = 0; i < m_kernelSize; ++i)
			{
				sum += m_kernel[i] * m_image[srcOffset + i];
			}

			m_workBuffer[destOffset] = (UINT8) sum;
		}

		// Write-back
		memcpy(&m_image[(m_paddingSize + y) * m_realDimension], m_workBuffer, m_realDimension);
	}

	// Convolution along Y
	for(INT32 x = 0; x < m_dimension; ++x)
	{
		INT32 srcOffset = m_paddingSize + x;
		INT32 destOffset = m_paddingSize;

		for(INT32 y = 0; y < m_dimension; ++y, srcOffset += m_realDimension, ++destOffset)
		{
			// Convolve and accumulate
			float sum = 0.0f;

			for(INT32 i = 0; i < m_kernelSize; ++i)
			{
				sum += m_kernel[i] * m_image[srcOffset + i * m_realDimension];
			}

			m_workBuffer[destOffset] = (UINT8) sum;
		}

		// Write-back
		srcOffset = m_paddingSize;
		destOffset = m_paddingSize * m_realDimension + m_paddingSize + x;
		for(INT32 y = 0; y < m_dimension; ++y, ++srcOffset, destOffset += m_realDimension)
		{
			m_image[destOffset] = m_workBuffer[srcOffset];
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuSplatIDMap::CRuSplatIDMap()
:	m_mapType(0),
	m_dimension(72),
	m_texelAlterationIteration(0),
	m_texelAlterationMap(NULL),
	m_isFiltered(FALSE),
	m_isDirty(TRUE),
	m_numAlphaMaps(0),
	m_alphaMaps(NULL),
	m_maximumAlphaMapSize(0),
	m_alphaMapDataVersion(0),
	m_alphaMapData(NULL),
	m_lastCleanupTime(0)
{
}

CRuSplatIDMap::~CRuSplatIDMap()
{
	delete[] m_texelAlterationMap;

	// Release memory allocated for splat layers
	for(INT32 i = 0; i < m_splatLayers.Count(); ++i)
	{
		delete[] m_splatLayers[i].m_map;
	}

	// Release alpha maps
	if(m_alphaMaps)
	{
		for(INT32 i = 0; i < m_numAlphaMaps; ++i)
		{
			m_alphaMaps[i]->Release();
		}
		delete[] m_alphaMaps;
	}

	// Reset variables
	m_dimension = 0;
	m_numAlphaMaps = 0;
	m_alphaMaps = NULL;

	// Release temporaries
	delete m_alphaMapData;
}

IRuObject *CRuSplatIDMap::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuSplatIDMap();

	// Chain cloning call to parent class
	IRuObject::Clone(clonedObject);

	CRuSplatIDMap *clonedMap = static_cast<CRuSplatIDMap *>(clonedObject);

	// Initialize cloned map
	clonedMap->Initialize(m_dimension, m_splatRect);

	// Copy each splat layer
	for(INT32 i = 0; i < m_splatLayers.Count(); ++i)
	{
		SplatLayerDesc layerDesc;
		layerDesc.m_map = ruNEW UINT8 [m_splatLayerSize * m_splatLayerSize];

		layerDesc.m_id = m_splatLayers[i].m_id;
		memcpy(layerDesc.m_map, m_splatLayers[i].m_map, m_splatLayerSize * m_splatLayerSize);

		clonedMap->m_splatLayers.Add(layerDesc);
	}

	return clonedObject;
}

BOOL CRuSplatIDMap::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read map type (0 = splat ID, 1 = compressed alpha textures, 2 = uncompressed alpha textures)
	stream->Read(&m_mapType, sizeof(INT32));

	// Map type zero is no longer supported
	assert(m_mapType != 0);

	if(m_mapType == 1)
	{
		// Read map dimension
		stream->Read(&m_dimension, sizeof(INT32));

		// Read number of layers
		INT32 numLayers = 0;
		stream->Read(&numLayers, sizeof(INT32));

		// Read layer IDs
		for(INT32 i = 0; i < numLayers; ++i)
		{
			UINT16 layerID;
			stream->Read(&layerID, sizeof(UINT16));
			m_layerIDs.Add(layerID);
		}

		// Determine size of alpha map data stream
		INT32 alphaMapDataSize = 0;
		alphaMapDataSize += stream->Read(&m_numAlphaMaps, sizeof(INT32));

		for(INT32 i = 0; i < m_numAlphaMaps; ++i)
		{
			// Read number of mip levels
			INT32 numMipLevels = 1;
			alphaMapDataSize += stream->Read(&numMipLevels, sizeof(INT32));

			// Read alpha map dimension
			UINT32 alphaMapWidth, alphaMapHeight;
			alphaMapDataSize += stream->Read(&alphaMapWidth, sizeof(UINT32));
			alphaMapDataSize += stream->Read(&alphaMapHeight, sizeof(UINT32));

			// Read alpha map mip levels
			for(INT32 j = 0; j < numMipLevels; ++j)
			{
				INT32 bytesPerBlock = RuSurfaceFormat_GetBitsPerTexel(ruSURFACEFMT_DXT1) << 1;
				INT32 columns = alphaMapWidth >> 2;
				INT32 rows = alphaMapHeight >> 2;
				INT32 mipLevelSize = rows * columns * bytesPerBlock;

				// Skip alpha map data
				stream->Seek(mipLevelSize, ruSSM_Current);
				alphaMapDataSize += mipLevelSize;

				alphaMapWidth = alphaMapWidth >> 1;
				alphaMapHeight = alphaMapHeight >> 1;
			}
		}

		// Copy alpha map data
		m_alphaMapData = ruNEW CRuMemoryStream();
		m_alphaMapData->SetStreamSize(alphaMapDataSize);

		stream->Seek(-alphaMapDataSize, ruSSM_Current);
		stream->Read(m_alphaMapData->GetBuffer(), alphaMapDataSize);

		// Set flags
		m_isFiltered = TRUE;
		m_isDirty = FALSE;
	}
	else if(m_mapType == 2)
	{
		// Read map dimension
		stream->Read(&m_dimension, sizeof(INT32));
		stream->Read(&m_splatLayerSize, sizeof(INT32));

		m_texelAlterationMap = ruNEW UINT8 [m_splatLayerSize * m_splatLayerSize];

		// Read splat rect
		stream->Read(&m_splatRect.m_left, sizeof(float));
		stream->Read(&m_splatRect.m_top, sizeof(float));
		stream->Read(&m_splatRect.m_right, sizeof(float));
		stream->Read(&m_splatRect.m_bottom, sizeof(float));

		INT32 numSplatLayers = 0;
		stream->Read(&numSplatLayers, sizeof(INT32));

		for(INT32 i = 0; i < numSplatLayers; ++i)
		{
			SplatLayerDesc layerDesc;
			layerDesc.m_map = ruNEW UINT8 [m_splatLayerSize * m_splatLayerSize];

			stream->Read(&layerDesc.m_id, sizeof(INT32));
			stream->Read(layerDesc.m_map, m_splatLayerSize * m_splatLayerSize);

			m_splatLayers.Add(layerDesc);
		}
	}

	return TRUE;
}

BOOL CRuSplatIDMap::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write map type (0 = splat ID, 1 = compressed alpha textures, 2 = uncompressed alpha textures)
	stream->Write(&m_mapType, sizeof(INT32));

	switch(m_mapType)
	{
		case 2:
			{
				// Write map dimension
				stream->Write(&m_dimension, sizeof(INT32));
				stream->Write(&m_splatLayerSize, sizeof(INT32));

				// Write splat rect
				stream->Write(&m_splatRect.m_left, sizeof(float));
				stream->Write(&m_splatRect.m_top, sizeof(float));
				stream->Write(&m_splatRect.m_right, sizeof(float));
				stream->Write(&m_splatRect.m_bottom, sizeof(float));

				INT32 numSplatLayers = m_splatLayers.Count();
				stream->Write(&numSplatLayers, sizeof(INT32));

				for(INT32 i = 0; i < m_splatLayers.Count(); ++i)
				{
					stream->Write(&m_splatLayers[i].m_id, sizeof(INT32));
					stream->Write(m_splatLayers[i].m_map, m_splatLayerSize * m_splatLayerSize);
				}
			}

			break;
	}

	return TRUE;
}

BOOL CRuSplatIDMap::SerializeToAlphaMaps(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write map type (0 = normal, 1 = alpha textures only)
	INT32 idMapType = 1;
	stream->Write(&idMapType, sizeof(INT32));

	// Write id map dimension
	stream->Write(&m_dimension, sizeof(INT32));

	// Write number of layers
	INT32 numLayers = m_layerIDs.Count();
	stream->Write(&numLayers, sizeof(INT32));

	// Write layer IDs
	for(INT32 i = 0; i < m_layerIDs.Count(); ++i)
	{
		stream->Write(&m_layerIDs[i], sizeof(UINT16));
	}

	// Write number of alpha maps
	stream->Write(&m_numAlphaMaps, sizeof(INT32));

	for(INT32 i = 0; i < m_numAlphaMaps; ++i)
	{
		UINT32 alphaMapWidth = m_alphaMaps[i]->GetLevelWidth(0);
		UINT32 alphaMapHeight = m_alphaMaps[i]->GetLevelHeight(0);

		// Convert alpha map to DXT1
		CRuTexture_Proxy *dxtAlphaMap = ruNEW CRuTexture_Proxy();
		dxtAlphaMap->Allocate(alphaMapWidth, alphaMapHeight, 1, ruSURFACEFMT_DXT1);
		CRuDXTCompressor::ConvertToDXT(m_alphaMaps[i], dxtAlphaMap);

		// Write number of mip levels
		INT32 numMipLevels = dxtAlphaMap->GetLevelCount();
		stream->Write(&numMipLevels, sizeof(INT32));

		// Write alpha map dimension
		stream->Write(&alphaMapWidth, sizeof(UINT32));
		stream->Write(&alphaMapHeight, sizeof(UINT32));

		// Write alpha map
		INT32 bytesPerBlock = RuSurfaceFormat_GetBitsPerTexel(ruSURFACEFMT_DXT1) << 1;

		// Write alpha map mip levels
		for(INT32 j = 0; j < numMipLevels; ++j)
		{
			BYTE *srcBits;
			INT32 srcPitch;

			dxtAlphaMap->Lock(j, reinterpret_cast<void **>(&srcBits), &srcPitch);

			for(INT32 k = 0, rows = dxtAlphaMap->GetLevelHeight(j) >> 2, columns = dxtAlphaMap->GetLevelWidth(j) >> 2; k < rows; ++k)
			{
				stream->Write(srcBits, bytesPerBlock * columns);
				srcBits += srcPitch;
			}

			dxtAlphaMap->Unlock(j);
		}

		ruSAFE_RELEASE(dxtAlphaMap);
	}

	return TRUE;
}

BOOL CRuSplatIDMap::SkipStream(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read map type (0 = normal, 1 = alpha textures only)
	INT32 mapType;
	stream->Read(&mapType, sizeof(INT32));

	switch(mapType)
	{
		case 0:
			assert(false);
			break;

		case 1:
			{
				// Read map dimension
				INT32 dimension;
				stream->Read(&dimension, sizeof(INT32));

				// Read number of layers
				INT32 numLayers = 0;
				stream->Read(&numLayers, sizeof(INT32));

				// Read layer IDs
				for(INT32 i = 0; i < numLayers; ++i)
				{
					UINT16 layerID;
					stream->Read(&layerID, sizeof(UINT16));
				}

				// Read number of alpha maps
				INT32 numAlphaMaps;
				stream->Read(&numAlphaMaps, sizeof(INT32));

				for(INT32 i = 0; i < numAlphaMaps; ++i)
				{
					// Read number of mip levels
					INT32 numMipLevels = 1;
					stream->Read(&numMipLevels, sizeof(INT32));

					// Read alpha map dimension
					UINT32 alphaMapWidth, alphaMapHeight;
					stream->Read(&alphaMapWidth, sizeof(UINT32));
					stream->Read(&alphaMapHeight, sizeof(UINT32));

					// Read alpha map mip levels
					for(INT32 j = 0; j < numMipLevels; ++j)
					{
						INT32 bytesPerBlock = RuSurfaceFormat_GetBitsPerTexel(ruSURFACEFMT_DXT1) << 1;
						INT32 columns = alphaMapWidth >> 2;
						INT32 rows = alphaMapHeight >> 2;
						INT32 mipLevelSize = rows * columns * bytesPerBlock;

						// Skip alpha map data
						stream->Seek(mipLevelSize, ruSSM_Current);

						alphaMapWidth = alphaMapWidth >> 1;
						alphaMapHeight = alphaMapHeight >> 1;
					}
				}
			}

			break;

		case 2:
			{
				// Uh huh... finish me
			}

			break;
	}

	return TRUE;
}

BOOL CRuSplatIDMap::Initialize(INT32 dimension, const CRuRectangle &splatRect)
{
	if(dimension > 0)
	{
		// Save specified dimension
		m_dimension = dimension;			// legacy crap...
		m_splatLayerSize = m_dimension * 2;

		// Save specified splat rect
		m_splatRect = splatRect;

		// Map type is always 2
		m_mapType = 2;

		// Setup texel alteration map
		m_texelAlterationMap = ruNEW UINT8 [m_splatLayerSize * m_splatLayerSize];

		return TRUE;
	}

	return FALSE;
}

BOOL CRuSplatIDMap::SetSplatRect(const CRuRectangle &splatRect)
{
	m_splatRect = splatRect;
	return TRUE;
}

INT32 CRuSplatIDMap::GetNumLayers() const
{
	switch(m_mapType)
	{
		case 0:
			assert(false);
			break;

		case 1:
			return m_layerIDs.Count();

		case 2:
			return m_splatLayers.Count();
	}

	return 0;
}

void CRuSplatIDMap::GetIDListAsString(char *splatIDListOut)
{
	if(splatIDListOut)
	{
		splatIDListOut[0] = NULL;

		switch(m_mapType)
		{
			case 0:
				assert(false);
				break;

			case 1:
				{
					for(INT32 i = 0; i < m_layerIDs.Count(); ++i)
					{
						char temp[16];
						sprintf(temp, "%d ", m_layerIDs[i]);
						strcat(splatIDListOut, temp);
					}
				}

				break;

			case 2:
				{
					for(INT32 i = 0; i < m_splatLayers.Count(); ++i)
					{
						char temp[16];
						sprintf(temp, "%d ", m_splatLayers[i].m_id);
						strcat(splatIDListOut, temp);
					}
				}

				break;
		}
	}
}

UINT16 CRuSplatIDMap::GetLayerID(INT32 layerIndex) const
{
	switch(m_mapType)
	{
		case 0:
			assert(false);
			break;

		case 1:
			if(layerIndex >= 0 && layerIndex < m_layerIDs.Count())
			{
				return m_layerIDs[layerIndex];
			}

			break;

		case 2:
			if(layerIndex >= 0 && layerIndex < m_splatLayers.Count())
			{
				return m_splatLayers[layerIndex].m_id;
			}

			break;
	}

	return 0;
}

INT32 CRuSplatIDMap::GetNumAlphaMaps() const
{
	if(m_alphaMapData)
		return 0;

	return m_numAlphaMaps;
}

BOOL CRuSplatIDMap::FinishAlphaMapLoad()
{
	// If there is alpha map data in the temporaries, unpack it now
	if(m_alphaMapData)
	{
		// Rewind
		m_alphaMapData->Seek(0, ruSSM_Begin);

		// Read number of alpha maps
		m_alphaMapData->Read(&m_numAlphaMaps, sizeof(INT32));
		m_alphaMaps = ruNEW CRuTexture_Proxy * [m_numAlphaMaps];

		for(INT32 i = 0; i < m_numAlphaMaps; ++i)
		{
			// Read number of mip levels
			INT32 numMipLevels = 1;
			m_alphaMapData->Read(&numMipLevels, sizeof(INT32));

			// Read alpha map dimension
			UINT32 alphaMapWidth, alphaMapHeight;
			m_alphaMapData->Read(&alphaMapWidth, sizeof(UINT32));
			m_alphaMapData->Read(&alphaMapHeight, sizeof(UINT32));

			// Create alpha map texture
			m_alphaMaps[i] = ruNEW CRuTexture_Proxy();
			m_alphaMaps[i]->Allocate(alphaMapWidth, alphaMapHeight, numMipLevels, ruSURFACEFMT_DXT1);

			// Read alpha map
			assert(m_alphaMaps[i]);

			// Read alpha map mip levels
			for(INT32 j = 0; j < numMipLevels; ++j)
			{
				INT32 bytesPerBlock = RuSurfaceFormat_GetBitsPerTexel(ruSURFACEFMT_DXT1) << 1;
				INT32 columns = alphaMapWidth >> 2;
				INT32 rows = alphaMapHeight >> 2;
				INT32 mipLevelSize = rows * columns * bytesPerBlock;

				// Lock the alpha map surface
				BYTE *alphaBits;
				INT32 alphaPitch;
				m_alphaMaps[i]->Lock(j, (void **) &alphaBits, &alphaPitch);

				for(INT32 y = 0; y < columns; ++y)
				{
					INT32 offset = (y * alphaPitch);

					m_alphaMapData->Read(&alphaBits[offset], columns * bytesPerBlock);
				}

				// Unlock alpha map surface
				m_alphaMaps[i]->Unlock(j);

				alphaMapWidth = alphaMapWidth >> 1;
				alphaMapHeight = alphaMapHeight >> 1;
			}
		}

		delete m_alphaMapData;
		m_alphaMapData = NULL;

		return TRUE;
	}

	return FALSE;
}

IRuTexture *CRuSplatIDMap::GetAlphaMap(INT32 mapIndex)
{
	// Return requested alpha map
	if(mapIndex >= 0 && mapIndex < m_numAlphaMaps)
		return m_alphaMaps[mapIndex];

	return NULL;
}

INT32 CRuSplatIDMap::GetDimension() const
{
	return m_dimension;
}

INT32 CRuSplatIDMap::GetSplatLayerSize() const
{
	return m_splatLayerSize;
}

BOOL CRuSplatIDMap::ReduceSizeToPowerOfTwo()
{
	// Calculate new reduced splat layer size
	INT32 newSplatLayerSize = 1;

	while(newSplatLayerSize <= m_splatLayerSize)
	{
		newSplatLayerSize = newSplatLayerSize << 1;
	}

	newSplatLayerSize = newSplatLayerSize >> 1;

	// Calculate padding on each side
	INT32 padding = (m_splatLayerSize - newSplatLayerSize) / 2;

	// If padding is non-zero, reduce the splat map
	if(padding != 0)
	{
		// Reduce texture alteration map
		UINT8 *reducedTexelAlterationMap = ruNEW UINT8 [newSplatLayerSize * newSplatLayerSize];
		for(INT32 z = 0, j = 0; z < newSplatLayerSize; ++z)
		{
			for(INT32 x = 0; x < newSplatLayerSize; ++x, ++j)
			{
				reducedTexelAlterationMap[j] = m_texelAlterationMap[x + padding + (z + padding) * m_splatLayerSize];
			}
		}

		delete[] m_texelAlterationMap;
		m_texelAlterationMap = reducedTexelAlterationMap;

		// Reduce layers
		for(INT32 i = 0; i < m_splatLayers.Count(); ++i)
		{
			UINT8 *reducedLayer = ruNEW UINT8 [newSplatLayerSize * newSplatLayerSize];

			for(INT32 z = 0, j = 0; z < newSplatLayerSize; ++z)
			{
				for(INT32 x = 0; x < newSplatLayerSize; ++x, ++j)
				{
					reducedLayer[j] = m_splatLayers[i].m_map[x + padding + (z + padding) * m_splatLayerSize];
				}
			}

			delete[] m_splatLayers[i].m_map;
			m_splatLayers[i].m_map = reducedLayer;
		}
	}

	m_dimension = newSplatLayerSize >> 1;
	m_splatLayerSize = newSplatLayerSize;

	return TRUE;
}

void CRuSplatIDMap::ClearTexelAlterationMap(INT32 iteration)
{
	if(iteration != m_texelAlterationIteration)
	{
		m_texelAlterationIteration = iteration;
		memset(m_texelAlterationMap, 0, m_splatLayerSize * m_splatLayerSize);
	}
}

BOOL CRuSplatIDMap::MarkAsDirty()
{
	m_isDirty = TRUE;
	return TRUE;
}

BOOL CRuSplatIDMap::FreeAlphaMaps()
{
	m_isDirty = TRUE;

	// Release alpha maps
	if(m_alphaMaps)
	{
		for(INT32 i = 0; i < m_numAlphaMaps; ++i)
		{
			m_alphaMaps[i]->Release();
		}
		delete[] m_alphaMaps;

		m_alphaMaps = NULL;
		m_numAlphaMaps = 0;
	}

	return TRUE;
}

INT32 CRuSplatIDMap::GetSplatLayerIndex(INT32 splatID)
{
	// Locate matching layer
	INT32 dstLayerIdx = -1;
	for(INT32 i = 0; i < m_splatLayers.Count(); ++i)
	{
		if(m_splatLayers[i].m_id == splatID)
		{
			dstLayerIdx = i;
			break;
		}
	}

	// Create layer if it does not exist
	if(dstLayerIdx == -1)
	{
		SplatLayerDesc layerDesc;
		layerDesc.m_id = splatID;
		layerDesc.m_map = ruNEW UINT8 [m_splatLayerSize * m_splatLayerSize];

		// If this is the first layer, clear values to 0xFF, otherwise clear to 0x00
		if(m_splatLayers.Count() == 0)
		{
			memset(layerDesc.m_map, 0xFF, m_splatLayerSize * m_splatLayerSize);
		}
		else
		{
			memset(layerDesc.m_map, 0, m_splatLayerSize * m_splatLayerSize);
		}

		m_splatLayers.Add(layerDesc);
		dstLayerIdx = m_splatLayers.Count() - 1;
	}

	return dstLayerIdx;
}

void CRuSplatIDMap::SumSplatTexelToOne(INT32 excludedLayerIdx, INT32 texelIdx)
{
	// Total up the weights of other splat layers
	INT32 paintValue = 0;
	INT32 remainderTotal = 0;
	for(INT32 i = 0; i < m_splatLayers.Count(); ++i)
	{
		if(i != excludedLayerIdx)
		{
			remainderTotal += m_splatLayers[i].m_map[texelIdx];
		}
		else
		{
			paintValue = m_splatLayers[i].m_map[texelIdx];
		}
	}

	// Adjust intensity values of other splat layers such that the total adds to close to 255
	if(remainderTotal > 0)
	{
		for(INT32 i = 0; i < m_splatLayers.Count(); ++i)
		{
			if(i != excludedLayerIdx)
			{
				m_splatLayers[i].m_map[texelIdx] = (INT32) ((255 - paintValue) * ((REAL) m_splatLayers[i].m_map[texelIdx] / remainderTotal));
			}
		}
	}
}

void CRuSplatIDMap::RemoveUnusedSplatLayers()
{
	DWORD t = timeGetTime();
	if(t - m_lastCleanupTime > 1000)
	{
		m_lastCleanupTime = t;

		for(INT32 i = 0; i < m_splatLayers.Count(); ++i)
		{
			BOOL cleanThisLayer = TRUE;
			for(INT32 j = 0, k = m_splatLayerSize * m_splatLayerSize; j < k; ++j)
			{
				if(m_splatLayers[i].m_map[j] > 0)
				{
					cleanThisLayer = FALSE;
					break;
				}
			}

			if(cleanThisLayer)
			{
				delete[] m_splatLayers[i].m_map;
				m_splatLayers.RemoveAt(i);
				--i;
			}
		}
	}
}

BOOL CRuSplatIDMap::PaintSplat(const CRuVector3 &center, float radius, REAL power, RuFalloffFunction falloffFunc, UINT16 splatID)
{
	CRuVector3 relativeCenter(center.x - m_splatRect.m_left, 0.0f, center.z - m_splatRect.m_top);

	CRuRectangle effectRect;
	effectRect.m_left = relativeCenter.x - radius;
	effectRect.m_right = relativeCenter.x + radius;
	effectRect.m_top = relativeCenter.z - radius;
	effectRect.m_bottom = relativeCenter.z + radius;

	if(	effectRect.m_right < m_splatRect.m_left - m_splatRect.m_left ||
		effectRect.m_left > m_splatRect.m_right - m_splatRect.m_left ||
		effectRect.m_bottom < m_splatRect.m_top - m_splatRect.m_top ||
		effectRect.m_top > m_splatRect.m_bottom - m_splatRect.m_top)
		return FALSE;

	switch(m_mapType)
	{
		case 2:
			{
				float xGridSize = (m_splatRect.m_right - m_splatRect.m_left) / m_splatLayerSize;
				float zGridSize = (m_splatRect.m_bottom - m_splatRect.m_top) / m_splatLayerSize;

				INT32 sX = max(0, (INT32) (effectRect.m_left / xGridSize));
				INT32 sZ = max(0, (INT32) (effectRect.m_top / zGridSize));
				INT32 eX = min(m_splatLayerSize, (INT32) (effectRect.m_right / xGridSize) + 1);
				INT32 eZ = min(m_splatLayerSize, (INT32) (effectRect.m_bottom / zGridSize) + 1);

				// clamp power
				power = max(0.0f, min(1.0f, power));

				// Locate matching layer
				INT32 dstLayerIdx = GetSplatLayerIndex(splatID);

				for(INT32 z = sZ; z < eZ; ++z)
				{
					for(INT32 x = sX; x < eX; ++x)
					{
						CRuVector3 gridPoint(x * xGridSize + xGridSize / 2.0f, 0.0f, z * zGridSize + zGridSize / 2.0f);

						float distance = (gridPoint - relativeCenter).Magnitude();

						// Ignore texel if it is out of modification area
						if(distance > radius)
							continue;

						UINT8 paintValue = (UINT8) (255 * falloffFunc(distance, radius) * power);

						// Store maximum intensity for the texel
						if(m_texelAlterationMap[x + z * m_splatLayerSize] == 0)
							m_texelAlterationMap[x + z * m_splatLayerSize] = min(255, (UINT32) (m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] + 255 * power));

						m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = min(m_texelAlterationMap[x + z * m_splatLayerSize], m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] + paintValue);
						SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);

						m_isFiltered = FALSE;
						m_isDirty = TRUE;
					}
				}

				RemoveUnusedSplatLayers();
			}

			break;
	}

	return m_isDirty;
}

BOOL CRuSplatIDMap::SprayPaintSplat(const CRuVector3 &center, float radius, REAL power, RuFalloffFunction falloffFunc, UINT16 splatID, float sprayProbability)
{
	CRuVector3 relativeCenter(center.x - m_splatRect.m_left, 0.0f, center.z - m_splatRect.m_top);

	CRuRectangle effectRect;
	effectRect.m_left = relativeCenter.x - radius;
	effectRect.m_right = relativeCenter.x + radius;
	effectRect.m_top = relativeCenter.z - radius;
	effectRect.m_bottom = relativeCenter.z + radius;

	if(	effectRect.m_right < m_splatRect.m_left - m_splatRect.m_left ||
		effectRect.m_left > m_splatRect.m_right - m_splatRect.m_left ||
		effectRect.m_bottom < m_splatRect.m_top - m_splatRect.m_top ||
		effectRect.m_top > m_splatRect.m_bottom - m_splatRect.m_top)
		return FALSE;

	switch(m_mapType)
	{
		case 2:
			{
				float xGridSize = (m_splatRect.m_right - m_splatRect.m_left) / m_splatLayerSize;
				float zGridSize = (m_splatRect.m_bottom - m_splatRect.m_top) / m_splatLayerSize;

				INT32 sX = max(0, (INT32) (effectRect.m_left / xGridSize));
				INT32 sZ = max(0, (INT32) (effectRect.m_top / zGridSize));
				INT32 eX = min(m_splatLayerSize, (INT32) (effectRect.m_right / xGridSize) + 1);
				INT32 eZ = min(m_splatLayerSize, (INT32) (effectRect.m_bottom / zGridSize) + 1);

				// clamp power
				power = max(0.0f, min(1.0f, power));

				// Locate matching layer
				INT32 dstLayerIdx = GetSplatLayerIndex(splatID);

				for(INT32 z = sZ; z < eZ; ++z)
				{
					for(INT32 x = sX; x < eX; ++x)
					{
						CRuVector3 gridPoint(x * xGridSize + xGridSize / 2.0f, 0.0f, z * zGridSize + zGridSize / 2.0f);

						float probability = RuGlobalMTRand().GenerateFloat();

						if(probability <= sprayProbability)
						{
							float distance = (gridPoint - relativeCenter).Magnitude();

							// Ignore texel if it is out of modification area
							if(distance > radius)
								continue;

							UINT8 paintValue = (UINT8) (255 * falloffFunc(distance, radius) * power);

							// Store maximum intensity for the texel
							if(m_texelAlterationMap[x + z * m_splatLayerSize] == 0)
								m_texelAlterationMap[x + z * m_splatLayerSize] = min(255, (UINT32) (m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] + 255 * power));

							m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = min(m_texelAlterationMap[x + z * m_splatLayerSize], m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] + paintValue);
							SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);

							m_isFiltered = FALSE;
							m_isDirty = TRUE;
						}
					}
				}

				RemoveUnusedSplatLayers();
			}

			break;
	}

	return m_isDirty;
}

BOOL CRuSplatIDMap::BlockFillSplat(const CRuVector3 &center, float radius, UINT16 splatID)
{
	CRuVector3 relativeCenter(center.x - m_splatRect.m_left, 0.0f, center.z - m_splatRect.m_top);

	CRuRectangle effectRect;
	effectRect.m_left = relativeCenter.x - radius;
	effectRect.m_right = relativeCenter.x + radius;
	effectRect.m_top = relativeCenter.z - radius;
	effectRect.m_bottom = relativeCenter.z + radius;

	if(	effectRect.m_right < m_splatRect.m_left - m_splatRect.m_left ||
		effectRect.m_left > m_splatRect.m_right - m_splatRect.m_left ||
		effectRect.m_bottom < m_splatRect.m_top - m_splatRect.m_top ||
		effectRect.m_top > m_splatRect.m_bottom - m_splatRect.m_top)
		return FALSE;

	switch(m_mapType)
	{
		case 2:
			{
				float xGridSize = (m_splatRect.m_right - m_splatRect.m_left) / m_splatLayerSize;
				float zGridSize = (m_splatRect.m_bottom - m_splatRect.m_top) / m_splatLayerSize;

				INT32 sX = 0;
				INT32 sZ = 0;
				INT32 eX = m_splatLayerSize;
				INT32 eZ = m_splatLayerSize;

				// Delete all layers but one
				for(INT32 i = 1; i < m_splatLayers.Count(); ++i)
				{
					delete[] m_splatLayers[i].m_map;
					m_splatLayers.RemoveAt(i);
					--i;
				}

				// Set remaining layer ID to target ID
				m_splatLayers[0].m_id = splatID;

				// Flood fill full intensity
				for(INT32 z = sZ; z < eZ; ++z)
				{
					for(INT32 x = sX; x < eX; ++x)
					{
						m_splatLayers[0].m_map[x + z * m_splatLayerSize] = 255;
					}
				}

				m_isFiltered = FALSE;
				m_isDirty = TRUE;
			}

			break;
	}

	return m_isDirty;
}

BOOL CRuSplatIDMap::FloodFillSplat(const CRuVector3 &center, float radius, UINT16 splatID, INT32 chunkKey, RuWorld::GetNeighborSplat getSplatCallback)
{
/*
	if(m_idMap == NULL)
		return FALSE;

	INT32 maxFillBlocks = 4;

	CRuVector3 relativeCenter(center.x - m_splatRect.m_left, 0.0f, center.z - m_splatRect.m_top);

	REAL xGridSize = (m_splatRect.m_right - m_splatRect.m_left) / m_dimension;
	REAL zGridSize = (m_splatRect.m_bottom - m_splatRect.m_top) / m_dimension;

	INT32 sX = min(max(0, (INT32) (relativeCenter.m_x / xGridSize)), m_dimension - 1);
	INT32 sZ = min(max(0, (INT32) (relativeCenter.m_z / zGridSize)), m_dimension - 1);

	UINT16 fillID = m_idMap[sX + sZ * m_dimension];

	FloodFillDesc *fillDesc = ruNEW FloodFillDesc();

	fillDesc->m_chunkKey = chunkKey;
	fillDesc->m_splatIDMap = this;
	fillDesc->m_texelStack.Push((sX << 16) | sZ);

	CRuStack<FloodFillDesc *> fillDescStack;
	fillDescStack.Push(fillDesc);

	while(fillDescStack.IsEmpty() == FALSE)
	{
		// Pop next fill descriptor off the stack
		FloodFillDesc *curFillDesc = fillDescStack.Pop();

		CRuSplatIDMap *splatIDMap = curFillDesc->m_splatIDMap;
		CRuStack<INT32> &texelStack = curFillDesc->m_texelStack;
		UINT16 *idMap = splatIDMap->m_idMap;

		INT32 cx = ruWORLD_CHUNKKEYX(curFillDesc->m_chunkKey);
		INT32 cz = ruWORLD_CHUNKKEYZ(curFillDesc->m_chunkKey);

		INT32 distanceToChunk = max(abs(cx - ruWORLD_CHUNKKEYX(chunkKey)), abs(cz - ruWORLD_CHUNKKEYZ(chunkKey)));
		if(distanceToChunk > maxFillBlocks)
		{
			delete curFillDesc;
			continue;
		}

		while(texelStack.IsEmpty() == FALSE)
		{
			INT32 texelKey = texelStack.Pop();
			INT32 x = (texelKey >> 16) & 0x0000FFFF;
			INT32 z = texelKey & 0x0000FFFF;
			INT32 texelIndex = x + z * m_dimension;

			if(idMap[texelIndex] == fillID && idMap[texelIndex] != splatID)
			{
				// Replace splat
				idMap[texelIndex] = splatID;

				// Push neighbors onto stack
				for(INT32 i = 0, k = 0; i < 3; ++i)
				{
					for(INT32 j = 0; j < 3; ++j, ++k)
					{
						INT32 nx = x + j - 1;
						INT32 nz = z + i - 1;

						if(nx < 0)
						{
							if(nz < 0)
							{
								CRuSplatIDMap *neighborSplatIDMap = getSplatCallback(curFillDesc->m_chunkKey, 0);
								if(neighborSplatIDMap)
								{
									// Find padding along x and z
									INT32 xPadding = static_cast<INT32>(fabs(((neighborSplatIDMap->m_splatRect.m_right - splatIDMap->m_splatRect.m_left) / xGridSize) / 2));
									INT32 zPadding = static_cast<INT32>(fabs(((neighborSplatIDMap->m_splatRect.m_bottom - splatIDMap->m_splatRect.m_top) / zGridSize) / 2));

									// Adjust texel coordinates
									nx = m_dimension - xPadding;
									nz = m_dimension - zPadding;

									FloodFillDesc *newFillDesc = ruNEW FloodFillDesc();
									newFillDesc->m_chunkKey = ruWORLD_MAKECHUNKKEY(cx - 1, cz - 1);
									newFillDesc->m_splatIDMap = neighborSplatIDMap;
									newFillDesc->m_texelStack.Push((nx << 16) | nz);
									fillDescStack.Push(newFillDesc);
								}
							}
							else if(nz >= m_dimension)
							{
								CRuSplatIDMap *neighborSplatIDMap = getSplatCallback(curFillDesc->m_chunkKey, 5);
								if(neighborSplatIDMap)
								{
									// Find padding along x and z
									INT32 xPadding = static_cast<INT32>(fabs(((neighborSplatIDMap->m_splatRect.m_right - splatIDMap->m_splatRect.m_left) / xGridSize) / 2));
									INT32 zPadding = static_cast<INT32>(fabs(((splatIDMap->m_splatRect.m_bottom - neighborSplatIDMap->m_splatRect.m_top) / zGridSize) / 2));

									// Adjust texel coordinates
									nx = m_dimension - xPadding;
									nz = zPadding;

									FloodFillDesc *newFillDesc = ruNEW FloodFillDesc();
									newFillDesc->m_chunkKey = ruWORLD_MAKECHUNKKEY(cx - 1, cz + 1);
									newFillDesc->m_splatIDMap = neighborSplatIDMap;
									newFillDesc->m_texelStack.Push((nx << 16) | nz);
									fillDescStack.Push(newFillDesc);
								}
							}
							else
							{
								CRuSplatIDMap *neighborSplatIDMap = getSplatCallback(curFillDesc->m_chunkKey, 3);
								if(neighborSplatIDMap)
								{
									// Find padding along x and z
									INT32 xPadding = static_cast<INT32>(fabs(((neighborSplatIDMap->m_splatRect.m_right - splatIDMap->m_splatRect.m_left) / xGridSize) / 2));

									// Adjust texel coordinates
									nx = m_dimension - xPadding;

									FloodFillDesc *newFillDesc = ruNEW FloodFillDesc();
									newFillDesc->m_chunkKey = ruWORLD_MAKECHUNKKEY(cx - 1, cz);
									newFillDesc->m_splatIDMap = neighborSplatIDMap;
									newFillDesc->m_texelStack.Push((nx << 16) | nz);
									fillDescStack.Push(newFillDesc);
								}
							}
						}
						else if(nx >= m_dimension)
						{
							if(nz < 0)
							{
								CRuSplatIDMap *neighborSplatIDMap = getSplatCallback(curFillDesc->m_chunkKey, 2);
								if(neighborSplatIDMap)
								{
									// Find padding along x and z
									INT32 xPadding = static_cast<INT32>(fabs(((splatIDMap->m_splatRect.m_right - neighborSplatIDMap->m_splatRect.m_left) / xGridSize) / 2));
									INT32 zPadding = static_cast<INT32>(fabs(((neighborSplatIDMap->m_splatRect.m_bottom - splatIDMap->m_splatRect.m_top) / zGridSize) / 2));

									// Adjust texel coordinates
									nx = xPadding;
									nz = m_dimension - zPadding;

									FloodFillDesc *newFillDesc = ruNEW FloodFillDesc();
									newFillDesc->m_chunkKey = ruWORLD_MAKECHUNKKEY(cx + 1, cz - 1);
									newFillDesc->m_splatIDMap = neighborSplatIDMap;
									newFillDesc->m_texelStack.Push((nx << 16) | nz);
									fillDescStack.Push(newFillDesc);
								}
							}
							else if(nz >= m_dimension)
							{
								CRuSplatIDMap *neighborSplatIDMap = getSplatCallback(curFillDesc->m_chunkKey, 7);
								if(neighborSplatIDMap)
								{
									// Find padding along x and z
									INT32 xPadding = static_cast<INT32>(fabs(((splatIDMap->m_splatRect.m_right - neighborSplatIDMap->m_splatRect.m_left) / xGridSize) / 2));
									INT32 zPadding = static_cast<INT32>(fabs(((splatIDMap->m_splatRect.m_bottom - neighborSplatIDMap->m_splatRect.m_top) / zGridSize) / 2));

									// Adjust texel coordinates
									nx = xPadding;
									nz = zPadding;

									FloodFillDesc *newFillDesc = ruNEW FloodFillDesc();
									newFillDesc->m_chunkKey = ruWORLD_MAKECHUNKKEY(cx + 1, cz + 1);
									newFillDesc->m_splatIDMap = neighborSplatIDMap;
									newFillDesc->m_texelStack.Push((nx << 16) | nz);
									fillDescStack.Push(newFillDesc);
								}
							}
							else
							{
								CRuSplatIDMap *neighborSplatIDMap = getSplatCallback(curFillDesc->m_chunkKey, 4);
								if(neighborSplatIDMap)
								{
									// Find padding along x and z
									INT32 xPadding = static_cast<INT32>(fabs(((splatIDMap->m_splatRect.m_right - neighborSplatIDMap->m_splatRect.m_left) / xGridSize) / 2));

									// Adjust texel coordinates
									nx = xPadding;

									FloodFillDesc *newFillDesc = ruNEW FloodFillDesc();
									newFillDesc->m_chunkKey = ruWORLD_MAKECHUNKKEY(cx + 1, cz);
									newFillDesc->m_splatIDMap = neighborSplatIDMap;
									newFillDesc->m_texelStack.Push((nx << 16) | nz);
									fillDescStack.Push(newFillDesc);
								}
							}
						}
						else
						{
							if(nz < 0)
							{
								CRuSplatIDMap *neighborSplatIDMap = getSplatCallback(curFillDesc->m_chunkKey, 1);
								if(neighborSplatIDMap)
								{
									// Find padding along x and z
									INT32 zPadding = static_cast<INT32>(fabs(((neighborSplatIDMap->m_splatRect.m_bottom - splatIDMap->m_splatRect.m_top) / zGridSize) / 2));

									// Adjust texel coordinates
									nz = m_dimension - zPadding;

									FloodFillDesc *newFillDesc = ruNEW FloodFillDesc();
									newFillDesc->m_chunkKey = ruWORLD_MAKECHUNKKEY(cx, cz - 1);
									newFillDesc->m_splatIDMap = neighborSplatIDMap;
									newFillDesc->m_texelStack.Push((nx << 16) | nz);
									fillDescStack.Push(newFillDesc);
								}
							}
							else if(nz >= m_dimension)
							{
								CRuSplatIDMap *neighborSplatIDMap = getSplatCallback(curFillDesc->m_chunkKey, 6);
								if(neighborSplatIDMap)
								{
									// Find padding along x and z
									INT32 zPadding = static_cast<INT32>(fabs(((splatIDMap->m_splatRect.m_bottom - neighborSplatIDMap->m_splatRect.m_top) / zGridSize) / 2));

									// Adjust texel coordinates
									nz = zPadding;

									FloodFillDesc *newFillDesc = ruNEW FloodFillDesc();
									newFillDesc->m_chunkKey = ruWORLD_MAKECHUNKKEY(cx, cz + 1);
									newFillDesc->m_splatIDMap = neighborSplatIDMap;
									newFillDesc->m_texelStack.Push((nx << 16) | nz);
									fillDescStack.Push(newFillDesc);

								}
							}
							else
							{
								texelStack.Push((nx << 16) | nz);
							}
						}
					}
				}
			}
		}

		splatIDMap->m_isFiltered = FALSE;
		splatIDMap->m_isDirty = TRUE;

		delete curFillDesc;
	}

	return m_isDirty;
*/
	return FALSE;
}

void swap(UINT16 &obj0, UINT16 &obj1)
{
	UINT16 temp = obj0;
	obj0 = obj1;
	obj1 = temp;
}

BOOL CRuSplatIDMap::SmudgeSplat(const CRuVector3 &center, float radius, float smudgeProbability)
{
/*
	if(m_idMap == NULL)
		return FALSE;

	CRuVector3 relativeCenter(center.x - m_splatRect.m_left, 0.0f, center.z - m_splatRect.m_top);

	CRuRectangle effectRect;
	effectRect.m_left = relativeCenter.x - radius;
	effectRect.m_right = relativeCenter.x + radius;
	effectRect.m_top = relativeCenter.z - radius;
	effectRect.m_bottom = relativeCenter.z + radius;

	if(	effectRect.m_right < m_splatRect.m_left - m_splatRect.m_left ||
		effectRect.m_left > m_splatRect.m_right - m_splatRect.m_left ||
		effectRect.m_bottom < m_splatRect.m_top - m_splatRect.m_top ||
		effectRect.m_top > m_splatRect.m_bottom - m_splatRect.m_top)
		return FALSE;

	float xGridSize = (m_splatRect.m_right - m_splatRect.m_left) / m_dimension;
	float zGridSize = (m_splatRect.m_bottom - m_splatRect.m_top) / m_dimension;

	INT32 sX = max(0, (INT32) (effectRect.m_left / xGridSize));
	INT32 sZ = max(0, (INT32) (effectRect.m_top / zGridSize));
	INT32 eX = min(m_dimension, (INT32) (effectRect.m_right / xGridSize) + 1);
	INT32 eZ = min(m_dimension, (INT32) (effectRect.m_bottom / zGridSize) + 1);

	for(INT32 z = sZ; z < eZ; ++z)
	{
		for(INT32 x = sX; x < eX; ++x)
		{
			CRuVector3 gridPoint(x * xGridSize + xGridSize / 2.0f, 0.0f, z * zGridSize + zGridSize / 2.0f);

			float probability = RuGlobalMTRand().GenerateFloat();

			if(probability <= smudgeProbability)
			{
				float distance = (gridPoint - relativeCenter).Magnitude();
				if(distance <= radius)
				{
					float directionProbability = RuGlobalMTRand().GenerateFloat();

					if(directionProbability <= 0.250f)
					{
						if(z - 1 >= 0)
						{
							swap(m_idMap[x + z * m_dimension], m_idMap[x + (z - 1) * m_dimension]);
							m_isFiltered = FALSE;
							m_isDirty = TRUE;
						}
					}
					else if(directionProbability <= 0.500f)
					{
						if(z + 1 < m_dimension)
						{
							swap(m_idMap[x + z * m_dimension], m_idMap[x + (z + 1) * m_dimension]);
							m_isFiltered = FALSE;
							m_isDirty = TRUE;
						}
					}
					else if(directionProbability <= 0.750f)
					{
						if(x - 1 >= 0)
						{
							swap(m_idMap[x + z * m_dimension], m_idMap[x - 1 + z * m_dimension]);
							m_isFiltered = FALSE;
							m_isDirty = TRUE;
						}
					}
					else
					{
						if(x + 1 < m_dimension)
						{
							swap(m_idMap[x + z * m_dimension], m_idMap[x + 1 + z * m_dimension]);
							m_isFiltered = FALSE;
							m_isDirty = TRUE;
						}
					}
				}
			}
		}
	}

	return m_isDirty;
*/
	return FALSE;
}

BOOL CRuSplatIDMap::ReplaceSplat(const CRuVector3 &center, float radius, UINT16 splatID)
{
/*
	if(m_idMap == NULL)
		return FALSE;

	UINT16 oldSplatID = PickSplatID(center);

	CRuVector3 relativeCenter(center.x - m_splatRect.m_left, 0.0f, center.z - m_splatRect.m_top);

	CRuRectangle effectRect;
	effectRect.m_left = relativeCenter.x - radius;
	effectRect.m_right = relativeCenter.x + radius;
	effectRect.m_top = relativeCenter.z - radius;
	effectRect.m_bottom = relativeCenter.z + radius;

	if(	effectRect.m_right < m_splatRect.m_left - m_splatRect.m_left ||
		effectRect.m_left > m_splatRect.m_right - m_splatRect.m_left ||
		effectRect.m_bottom < m_splatRect.m_top - m_splatRect.m_top ||
		effectRect.m_top > m_splatRect.m_bottom - m_splatRect.m_top)
		return FALSE;

	float xGridSize = (m_splatRect.m_right - m_splatRect.m_left) / m_dimension;
	float zGridSize = (m_splatRect.m_bottom - m_splatRect.m_top) / m_dimension;

	INT32 sX = 0;
	INT32 sZ = 0;
	INT32 eX = m_dimension;
	INT32 eZ = m_dimension;

	for(INT32 z = sZ; z < eZ; ++z)
	{
		for(INT32 x = sX; x < eX; ++x)
		{
			if(m_idMap[x + z * m_dimension] == oldSplatID)
			{
				m_idMap[x + z * m_dimension] = splatID;
			}
		}
	}

	m_isFiltered = FALSE;
	m_isDirty = TRUE;

	return m_isDirty;
*/
	return FALSE;
}

BOOL CRuSplatIDMap::AttractSplat(const CRuVector3 &center, float radius, UINT16 attractedSplatID, float attractProbability)
{
/*
	if(m_idMap == NULL)
		return FALSE;

	CRuVector3 relativeCenter(center.x - m_splatRect.m_left, 0.0f, center.z - m_splatRect.m_top);

	CRuRectangle effectRect;
	effectRect.m_left = relativeCenter.x - radius;
	effectRect.m_right = relativeCenter.x + radius;
	effectRect.m_top = relativeCenter.z - radius;
	effectRect.m_bottom = relativeCenter.z + radius;

	if(	effectRect.m_right < m_splatRect.m_left - m_splatRect.m_left ||
		effectRect.m_left > m_splatRect.m_right - m_splatRect.m_left ||
		effectRect.m_bottom < m_splatRect.m_top - m_splatRect.m_top ||
		effectRect.m_top > m_splatRect.m_bottom - m_splatRect.m_top)
		return FALSE;

	float xGridSize = (m_splatRect.m_right - m_splatRect.m_left) / m_dimension;
	float zGridSize = (m_splatRect.m_bottom - m_splatRect.m_top) / m_dimension;

	INT32 sX = max(0, (INT32) (effectRect.m_left / xGridSize));
	INT32 sZ = max(0, (INT32) (effectRect.m_top / zGridSize));
	INT32 eX = min(m_dimension, (INT32) (effectRect.m_right / xGridSize) + 1);
	INT32 eZ = min(m_dimension, (INT32) (effectRect.m_bottom / zGridSize) + 1);

	for(INT32 z = sZ; z < eZ; ++z)
	{
		for(INT32 x = sX; x < eX; ++x)
		{
			CRuVector3 gridPoint(x * xGridSize + xGridSize / 2.0f, 0.0f, z * zGridSize + zGridSize / 2.0f);

			float probability = RuGlobalMTRand().GenerateFloat();

			if(m_idMap[x + z * m_dimension] == attractedSplatID && probability <= attractProbability)
			{
				float distance = (gridPoint - relativeCenter).Magnitude();
				if(distance <= radius)
				{
					if(gridPoint.x < relativeCenter.x)
					{
						if(gridPoint.z < relativeCenter.z)
						{
							if(z + 1 < m_dimension && x + 1 < m_dimension)
							{
								swap(m_idMap[x + z * m_dimension], m_idMap[(x + 1) + (z + 1) * m_dimension]);
								m_isFiltered = FALSE;
								m_isDirty = TRUE;
							}
						}
						else if(gridPoint.z > relativeCenter.z)
						{
							if(z - 1 >= 0 && x + 1 < m_dimension)
							{
								swap(m_idMap[x + z * m_dimension], m_idMap[(x + 1) + (z - 1) * m_dimension]);
								m_isFiltered = FALSE;
								m_isDirty = TRUE;
							}
						}
						else
						{
							if(x + 1 < m_dimension)
							{
								swap(m_idMap[x + z * m_dimension], m_idMap[(x + 1) + z * m_dimension]);
								m_isFiltered = FALSE;
								m_isDirty = TRUE;
							}
						}
					}
					else if(gridPoint.x > relativeCenter.x)
					{
						if(gridPoint.z < relativeCenter.z)
						{
							if(z + 1 < m_dimension && x - 1 >= 0)
							{
								swap(m_idMap[x + z * m_dimension], m_idMap[(x - 1) + (z + 1) * m_dimension]);
								m_isFiltered = FALSE;
								m_isDirty = TRUE;
							}
						}
						else if(gridPoint.z > relativeCenter.z)
						{
							if(z - 1 >= 0 && x - 1 >= 0)
							{
								swap(m_idMap[x + z * m_dimension], m_idMap[(x - 1) + (z - 1) * m_dimension]);
								m_isFiltered = FALSE;
								m_isDirty = TRUE;
							}
						}
						else
						{
							if(x - 1 >= 0)
							{
								swap(m_idMap[x + z * m_dimension], m_idMap[(x - 1) + z * m_dimension]);
								m_isFiltered = FALSE;
								m_isDirty = TRUE;
							}
						}
					}
				}
			}
		}
	}

	return m_isDirty;
*/
	return FALSE;
}

UINT16 CRuSplatIDMap::PickSplatID(const CRuVector3 &center)
{
	CRuVector3 relativeCenter(center.x - m_splatRect.m_left, 0.0f, center.z - m_splatRect.m_top);

	switch(m_mapType)
	{
		case 2:
			{
				float xGridSize = (m_splatRect.m_right - m_splatRect.m_left) / m_splatLayerSize;
				float zGridSize = (m_splatRect.m_bottom - m_splatRect.m_top) / m_splatLayerSize;

				INT32 x = max(0, (INT32) (relativeCenter.x / xGridSize));
				INT32 z = max(0, (INT32) (relativeCenter.z / zGridSize));

				INT32 curSplatID = 0;
				INT32 curSplatIDWeight = 0;

				for(INT32 i = 0; i < m_splatLayers.Count(); ++i)
				{
					if(m_splatLayers[i].m_map[x + z * m_splatLayerSize] > curSplatIDWeight)
					{
						curSplatIDWeight = m_splatLayers[i].m_map[x + z * m_splatLayerSize];
						curSplatID = m_splatLayers[i].m_id;
					}
				}

				return curSplatID;
			}
	}

	return 0;
}

BOOL CRuSplatIDMap::MojoPaint(const CRuVector3 &center, float radius, CRuMojo_AttributeParcel<REAL> *refHF, CRuMojo_AttributeParcel<REAL> *chunkHF, CRuFractal_Base *srcFractal, const CRuSplatIDMap_PaintSetting &paintSetting)
{
	CRuVector3 relativeCenter(center.x - m_splatRect.m_left, 0.0f, center.z - m_splatRect.m_top);

	CRuRectangle effectRect;
	effectRect.m_left = relativeCenter.x - radius;
	effectRect.m_right = relativeCenter.x + radius;
	effectRect.m_top = relativeCenter.z - radius;
	effectRect.m_bottom = relativeCenter.z + radius;

	if(	effectRect.m_right < m_splatRect.m_left - m_splatRect.m_left ||
		effectRect.m_left > m_splatRect.m_right - m_splatRect.m_left ||
		effectRect.m_bottom < m_splatRect.m_top - m_splatRect.m_top ||
		effectRect.m_top > m_splatRect.m_bottom - m_splatRect.m_top)
		return FALSE;

	switch(m_mapType)
	{
		case 2:
			{
				float xGridSize = (m_splatRect.m_right - m_splatRect.m_left) / m_splatLayerSize;
				float zGridSize = (m_splatRect.m_bottom - m_splatRect.m_top) / m_splatLayerSize;

				INT32 sX = max(0, (INT32) (effectRect.m_left / xGridSize));
				INT32 sZ = max(0, (INT32) (effectRect.m_top / zGridSize));
				INT32 eX = min(m_splatLayerSize, (INT32) (effectRect.m_right / xGridSize) + 1);
				INT32 eZ = min(m_splatLayerSize, (INT32) (effectRect.m_bottom / zGridSize) + 1);

				for(INT32 z = sZ; z < eZ; ++z)
				{
					for(INT32 x = sX; x < eX; ++x)
					{
						CRuVector3 gridPoint(x * xGridSize + xGridSize / 2.0f, 0.0f, z * zGridSize + zGridSize / 2.0f);
						CRuVector3 worldPoint(gridPoint.x + m_splatRect.m_left, 1000.0f, gridPoint.z + m_splatRect.m_top);
						CRuVector3 relativePos = worldPoint - center;

						float distance = (gridPoint - relativeCenter).Magnitude();
						if(distance <= radius)
						{
							REAL refHeight = Mojo_GetHeightAtPoint(refHF, worldPoint);
							REAL chunkHeight = Mojo_GetHeightAtPoint(chunkHF, worldPoint);
							REAL relativeHeight = chunkHeight - refHeight;

							if(fabs(relativeHeight) > ruEPSILON)
							{
								REAL noiseLevel = relativeHeight;
								REAL slope = Mojo_GetSlopeAtPoint_Multisampled(chunkHF, worldPoint);
								REAL valleyDepth = Mojo_GetValleyDepth_Multisampled(chunkHF, worldPoint);
								INT32 resultingSplatID = 0;

								// convert slope to degrees
								slope = slope * 180.0f / ruPI;

								// jitter the slope by some noise
//								slope = slope + (RuGlobalMTRand().GenerateFloat() - 0.5f) * 5.0f;

								// --- This is for slope only


								if(paintSetting.m_splat_0 >= 0 && ((slope >= paintSetting.m_slope_Lower_0 && slope <= paintSetting.m_slope_Upper_0) || paintSetting.m_slope_Lower_0 <= 0.0f || paintSetting.m_slope_Upper_0 <= 0.0f))
								{
									REAL slopeT = paintSetting.m_slope_Upper_0 > paintSetting.m_slope_Lower_0 ? fabs((slope - paintSetting.m_slope_Lower_0) / (paintSetting.m_slope_Upper_0 - paintSetting.m_slope_Lower_0) * 2.0f - 1.0f) : 0.0f;
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_0, slopeT, 1.0f);

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_0);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * slopeWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);

								}

								if(paintSetting.m_splat_1 >= 0 && ((slope >= paintSetting.m_slope_Lower_1 && slope <= paintSetting.m_slope_Upper_1) || paintSetting.m_slope_Lower_1 <= 0.0f || paintSetting.m_slope_Upper_1 <= 0.0f))
								{
									REAL slopeT = fabs((slope - paintSetting.m_slope_Lower_1) / (paintSetting.m_slope_Upper_1 - paintSetting.m_slope_Lower_1) * 2.0f - 1.0f);
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_1, slopeT, 1.0f);

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_1);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * slopeWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);

								}

								if(paintSetting.m_splat_2 >= 0 && ((slope >= paintSetting.m_slope_Lower_2 && slope <= paintSetting.m_slope_Upper_2) || paintSetting.m_slope_Lower_2 <= 0.0f || paintSetting.m_slope_Upper_2 <= 0.0f))
								{
									REAL slopeT = fabs((slope - paintSetting.m_slope_Lower_2) / (paintSetting.m_slope_Upper_2 - paintSetting.m_slope_Lower_2) * 2.0f - 1.0f);
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_2, slopeT, 1.0f);

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_2);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * slopeWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);

								}

								if(paintSetting.m_splat_3 >= 0 && ((slope >= paintSetting.m_slope_Lower_3 && slope <= paintSetting.m_slope_Upper_3) || paintSetting.m_slope_Lower_3 <= 0.0f || paintSetting.m_slope_Upper_3 <= 0.0f))
								{
									REAL slopeT = fabs((slope - paintSetting.m_slope_Lower_3) / (paintSetting.m_slope_Upper_3 - paintSetting.m_slope_Lower_3) * 2.0f - 1.0f);
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_3, slopeT, 1.0f);

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_3);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * slopeWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);

								}

								if(paintSetting.m_splat_4 >= 0 && ((slope >= paintSetting.m_slope_Lower_4 && slope <= paintSetting.m_slope_Upper_4) || paintSetting.m_slope_Lower_4 <= 0.0f || paintSetting.m_slope_Upper_4 <= 0.0f))
								{
									REAL slopeT = fabs((slope - paintSetting.m_slope_Lower_4) / (paintSetting.m_slope_Upper_4 - paintSetting.m_slope_Lower_4) * 2.0f - 1.0f);
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_4, slopeT, 1.0f);

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_4);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * slopeWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);

								}

								if(paintSetting.m_splat_5 >= 0 && ((slope >= paintSetting.m_slope_Lower_5 && slope <= paintSetting.m_slope_Upper_5) || paintSetting.m_slope_Lower_5 <= 0.0f || paintSetting.m_slope_Upper_5 <= 0.0f))
								{
									REAL slopeT = fabs((slope - paintSetting.m_slope_Lower_5) / (paintSetting.m_slope_Upper_5 - paintSetting.m_slope_Lower_5) * 2.0f - 1.0f);
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_5, slopeT, 1.0f);

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_5);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * slopeWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);

								}

/*
								if(paintSetting.m_splat_1 >= 0 && ((slope >= paintSetting.m_slope_Lower_1 && slope <= paintSetting.m_slope_Upper_1) || paintSetting.m_slope_Lower_1 <= 0.0f || paintSetting.m_slope_Upper_1 <= 0.0f))
								{
									// Calculate painting weights
									REAL thresholdT = fabs(((noiseLevel - paintSetting.m_threshold_Lower_1) / max(1.0f, paintSetting.m_threshold_Upper_1 - paintSetting.m_threshold_Lower_1)) * 2.0f - 1.0f);
									REAL thresholdWeight = paintSetting.EvaluateFalloff(paintSetting.m_threshold_Falloff_1, thresholdT, 1.0f);
									REAL slopeT = fabs((slope > 0.0f ? (slope - paintSetting.m_slope_Lower_1) / max(1.0f, paintSetting.m_slope_Upper_1 - paintSetting.m_slope_Lower_1) : 1.0f) * 2.0f - 1.0f);
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_1, slopeT, 1.0f);
									REAL aggregateWeight = slopeWeight;

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_1);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * aggregateWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);
								}

								if(paintSetting.m_splat_2 >= 0 && ((slope >= paintSetting.m_slope_Lower_2 && slope <= paintSetting.m_slope_Upper_2) || paintSetting.m_slope_Lower_2 <= 0.0f || paintSetting.m_slope_Upper_2 <= 0.0f))
								{
									// Calculate painting weights
									REAL thresholdT = fabs(((noiseLevel - paintSetting.m_threshold_Lower_2) / max(1.0f, paintSetting.m_threshold_Upper_2 - paintSetting.m_threshold_Lower_2)) * 2.0f - 1.0f);
									REAL thresholdWeight = paintSetting.EvaluateFalloff(paintSetting.m_threshold_Falloff_2, thresholdT, 1.0f);
									REAL slopeT = fabs((slope > 0.0f ? (slope - paintSetting.m_slope_Lower_2) / max(1.0f, paintSetting.m_slope_Upper_2 - paintSetting.m_slope_Lower_2) : 1.0f) * 2.0f - 1.0f);
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_2, slopeT, 1.0f);
									REAL aggregateWeight = slopeWeight;

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_2);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * aggregateWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);
								}

								if(paintSetting.m_splat_3 >= 0 && ((slope >= paintSetting.m_slope_Lower_3 && slope <= paintSetting.m_slope_Upper_3) || paintSetting.m_slope_Lower_3 <= 0.0f || paintSetting.m_slope_Upper_3 <= 0.0f))
								{
									// Calculate painting weights
									REAL thresholdT = fabs(((noiseLevel - paintSetting.m_threshold_Lower_3) / max(1.0f, paintSetting.m_threshold_Upper_3 - paintSetting.m_threshold_Lower_3)) * 2.0f - 1.0f);
									REAL thresholdWeight = paintSetting.EvaluateFalloff(paintSetting.m_threshold_Falloff_3, thresholdT, 1.0f);
									REAL slopeT = fabs((slope > 0.0f ? (slope - paintSetting.m_slope_Lower_3) / max(1.0f, paintSetting.m_slope_Upper_3 - paintSetting.m_slope_Lower_3) : 1.0f) * 2.0f - 1.0f);
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_3, slopeT, 1.0f);
									REAL aggregateWeight = thresholdWeight * slopeWeight;

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_3);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * aggregateWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);
								}

								if(paintSetting.m_splat_4 >= 0 && ((slope >= paintSetting.m_slope_Lower_4 && slope <= paintSetting.m_slope_Upper_4) || paintSetting.m_slope_Lower_4 <= 0.0f || paintSetting.m_slope_Upper_4 <= 0.0f))
								{
									// Calculate painting weights
									REAL thresholdT = fabs(((noiseLevel - paintSetting.m_threshold_Lower_4) / max(1.0f, paintSetting.m_threshold_Upper_4 - paintSetting.m_threshold_Lower_4)) * 2.0f - 1.0f);
									REAL thresholdWeight = paintSetting.EvaluateFalloff(paintSetting.m_threshold_Falloff_4, thresholdT, 1.0f);
									REAL slopeT = fabs((slope > 0.0f ? (slope - paintSetting.m_slope_Lower_4) / max(1.0f, paintSetting.m_slope_Upper_4 - paintSetting.m_slope_Lower_4) : 1.0f) * 2.0f - 1.0f);
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_4, slopeT, 1.0f);
									REAL aggregateWeight = slopeWeight;

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_4);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * aggregateWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);
								}

								if(paintSetting.m_splat_5 >= 0 && ((slope >= paintSetting.m_slope_Lower_5 && slope <= paintSetting.m_slope_Upper_5) || paintSetting.m_slope_Lower_5 <= 0.0f || paintSetting.m_slope_Upper_5 <= 0.0f))
								{
									// Calculate painting weights
									REAL thresholdT = fabs(((noiseLevel - paintSetting.m_threshold_Lower_5) / max(1.0f, paintSetting.m_threshold_Upper_5 - paintSetting.m_threshold_Lower_5)) * 2.0f - 1.0f);
									REAL thresholdWeight = paintSetting.EvaluateFalloff(paintSetting.m_threshold_Falloff_5, thresholdT, 1.0f);
									REAL slopeT = fabs((slope > 0.0f ? (slope - paintSetting.m_slope_Lower_5) / max(1.0f, paintSetting.m_slope_Upper_5 - paintSetting.m_slope_Lower_5) : 1.0f) * 2.0f - 1.0f);
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_5, slopeT, 1.0f);
									REAL aggregateWeight = slopeWeight;

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_5);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * aggregateWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);
								}
*/
/*
								if(paintSetting.m_splat_0 >= 0 && noiseLevel >= paintSetting.m_threshold_Lower_0 && noiseLevel <= paintSetting.m_threshold_Upper_0 && ((slope >= paintSetting.m_slope_Lower_0 && slope <= paintSetting.m_slope_Upper_0) || paintSetting.m_slope_Lower_0 <= 0.0f || paintSetting.m_slope_Upper_0 <= 0.0f))
								{
									// Calculate painting weights
									REAL thresholdT = fabs(((noiseLevel - paintSetting.m_threshold_Lower_0) / max(1.0f, paintSetting.m_threshold_Upper_0 - paintSetting.m_threshold_Lower_0)) * 2.0f - 1.0f);
									REAL thresholdWeight = paintSetting.EvaluateFalloff(paintSetting.m_threshold_Falloff_0, thresholdT, 1.0f);
									REAL slopeT = fabs((slope > 0.0f ? (slope - paintSetting.m_slope_Lower_0) / max(1.0f, paintSetting.m_slope_Upper_0 - paintSetting.m_slope_Lower_0) : 1.0f) * 2.0f - 1.0f);
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_0, slopeT, 1.0f);
									REAL aggregateWeight = thresholdWeight * slopeWeight;

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_0);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * aggregateWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);
								}

								if(paintSetting.m_splat_1 >= 0 && noiseLevel >= paintSetting.m_threshold_Lower_1 && noiseLevel <= paintSetting.m_threshold_Upper_1 && ((slope >= paintSetting.m_slope_Lower_1 && slope <= paintSetting.m_slope_Upper_1) || paintSetting.m_slope_Lower_1 <= 0.0f || paintSetting.m_slope_Upper_1 <= 0.0f))
								{
									// Calculate painting weights
									REAL thresholdT = fabs(((noiseLevel - paintSetting.m_threshold_Lower_1) / max(1.0f, paintSetting.m_threshold_Upper_1 - paintSetting.m_threshold_Lower_1)) * 2.0f - 1.0f);
									REAL thresholdWeight = paintSetting.EvaluateFalloff(paintSetting.m_threshold_Falloff_1, thresholdT, 1.0f);
									REAL slopeT = fabs((slope > 0.0f ? (slope - paintSetting.m_slope_Lower_1) / max(1.0f, paintSetting.m_slope_Upper_1 - paintSetting.m_slope_Lower_1) : 1.0f) * 2.0f - 1.0f);
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_1, slopeT, 1.0f);
									REAL aggregateWeight = thresholdWeight * slopeWeight;

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_1);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * aggregateWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);
								}

								if(paintSetting.m_splat_2 >= 0 && noiseLevel >= paintSetting.m_threshold_Lower_2 && noiseLevel <= paintSetting.m_threshold_Upper_2 && ((slope >= paintSetting.m_slope_Lower_2 && slope <= paintSetting.m_slope_Upper_2) || paintSetting.m_slope_Lower_2 <= 0.0f || paintSetting.m_slope_Upper_2 <= 0.0f))
								{
									// Calculate painting weights
									REAL thresholdT = fabs(((noiseLevel - paintSetting.m_threshold_Lower_2) / max(1.0f, paintSetting.m_threshold_Upper_2 - paintSetting.m_threshold_Lower_2)) * 2.0f - 1.0f);
									REAL thresholdWeight = paintSetting.EvaluateFalloff(paintSetting.m_threshold_Falloff_2, thresholdT, 1.0f);
									REAL slopeT = fabs((slope > 0.0f ? (slope - paintSetting.m_slope_Lower_2) / max(1.0f, paintSetting.m_slope_Upper_2 - paintSetting.m_slope_Lower_2) : 1.0f) * 2.0f - 1.0f);
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_2, slopeT, 1.0f);
									REAL aggregateWeight = thresholdWeight * slopeWeight;

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_2);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * aggregateWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);
								}

								if(paintSetting.m_splat_3 >= 0 && noiseLevel >= paintSetting.m_threshold_Lower_3 && noiseLevel <= paintSetting.m_threshold_Upper_3 && ((slope >= paintSetting.m_slope_Lower_3 && slope <= paintSetting.m_slope_Upper_3) || paintSetting.m_slope_Lower_3 <= 0.0f || paintSetting.m_slope_Upper_3 <= 0.0f))
								{
									// Calculate painting weights
									REAL thresholdT = fabs(((noiseLevel - paintSetting.m_threshold_Lower_3) / max(1.0f, paintSetting.m_threshold_Upper_3 - paintSetting.m_threshold_Lower_3)) * 2.0f - 1.0f);
									REAL thresholdWeight = paintSetting.EvaluateFalloff(paintSetting.m_threshold_Falloff_3, thresholdT, 1.0f);
									REAL slopeT = fabs((slope > 0.0f ? (slope - paintSetting.m_slope_Lower_3) / max(1.0f, paintSetting.m_slope_Upper_3 - paintSetting.m_slope_Lower_3) : 1.0f) * 2.0f - 1.0f);
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_3, slopeT, 1.0f);
									REAL aggregateWeight = thresholdWeight * slopeWeight;

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_3);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * aggregateWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);
								}

								if(paintSetting.m_splat_4 >= 0 && noiseLevel >= paintSetting.m_threshold_Lower_4 && noiseLevel <= paintSetting.m_threshold_Upper_4 && ((slope >= paintSetting.m_slope_Lower_4 && slope <= paintSetting.m_slope_Upper_4) || paintSetting.m_slope_Lower_4 <= 0.0f || paintSetting.m_slope_Upper_4 <= 0.0f))
								{
									// Calculate painting weights
									REAL thresholdT = fabs(((noiseLevel - paintSetting.m_threshold_Lower_4) / max(1.0f, paintSetting.m_threshold_Upper_4 - paintSetting.m_threshold_Lower_4)) * 2.0f - 1.0f);
									REAL thresholdWeight = paintSetting.EvaluateFalloff(paintSetting.m_threshold_Falloff_4, thresholdT, 1.0f);
									REAL slopeT = fabs((slope > 0.0f ? (slope - paintSetting.m_slope_Lower_4) / max(1.0f, paintSetting.m_slope_Upper_4 - paintSetting.m_slope_Lower_4) : 1.0f) * 2.0f - 1.0f);
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_4, slopeT, 1.0f);
									REAL aggregateWeight = thresholdWeight * slopeWeight;

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_4);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * aggregateWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);
								}

								if(paintSetting.m_splat_5 >= 0 && noiseLevel >= paintSetting.m_threshold_Lower_5 && noiseLevel <= paintSetting.m_threshold_Upper_5 && ((slope >= paintSetting.m_slope_Lower_5 && slope <= paintSetting.m_slope_Upper_5) || paintSetting.m_slope_Lower_5 <= 0.0f || paintSetting.m_slope_Upper_5 <= 0.0f))
								{
									// Calculate painting weights
									REAL thresholdT = fabs(((noiseLevel - paintSetting.m_threshold_Lower_5) / max(1.0f, paintSetting.m_threshold_Upper_5 - paintSetting.m_threshold_Lower_5)) * 2.0f - 1.0f);
									REAL thresholdWeight = paintSetting.EvaluateFalloff(paintSetting.m_threshold_Falloff_5, thresholdT, 1.0f);
									REAL slopeT = fabs((slope > 0.0f ? (slope - paintSetting.m_slope_Lower_5) / max(1.0f, paintSetting.m_slope_Upper_5 - paintSetting.m_slope_Lower_5) : 1.0f) * 2.0f - 1.0f);
									REAL slopeWeight = paintSetting.EvaluateFalloff(paintSetting.m_slope_Falloff_5, slopeT, 1.0f);
									REAL aggregateWeight = thresholdWeight * slopeWeight;

									// Locate matching splat layer
									INT32 dstLayerIdx = GetSplatLayerIndex(paintSetting.m_splat_5);

									m_splatLayers[dstLayerIdx].m_map[x + z * m_splatLayerSize] = (INT32) min(255, 255 * aggregateWeight);
									SumSplatTexelToOne(dstLayerIdx, x + z * m_splatLayerSize);
								}
*/
								m_isFiltered = FALSE;
								m_isDirty = TRUE;
							}
						}
					}
				}
			}
	}

	return m_isDirty;
}

INT32 CRuSplatIDMap::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuObject::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	footprint += sizeof(m_mapType);
	footprint += sizeof(m_dimension);
	footprint += sizeof(m_splatLayerSize);
	footprint += sizeof(m_splatRect);

	footprint += sizeof(m_texelAlterationIteration);
	footprint += sizeof(m_texelAlterationMap) + (m_texelAlterationMap ? sizeof(UINT8) * m_splatLayerSize * m_splatLayerSize : 0);
	footprint += m_splatLayers.GetMemoryFootprint();

	footprint += sizeof(m_isFiltered);
	footprint += sizeof(m_isDirty);
	footprint += m_layerIDs.GetMemoryFootprint();
	footprint += sizeof(m_numAlphaMaps);
	footprint += sizeof(m_alphaMaps);
	if(m_alphaMaps)
	{
		for(INT32 i = 0; i < m_numAlphaMaps; ++i)
		{
			footprint += RuTextureUtility_GetMemoryFootprint(m_alphaMaps[i], enumHash);
		}
	}

	footprint += sizeof(m_maximumAlphaMapSize);
	footprint += sizeof(m_alphaMapDataVersion);
	footprint += sizeof(m_alphaMapData) + (m_alphaMapData ? m_alphaMapData->GetBufferSize() : 0);

	footprint += sizeof(m_lastCleanupTime);

	return footprint;
}

INT32 CRuSplatIDMap::GetSplatIDMapSize(INT32 dimension)
{
	// NOTE: This function returns the correct size for id map type 0 (normal) only!
	//       Type 1 maps (alpha textures only) cannot have an accurate size returned
	//       because the size varies depending on the number of layers.

	INT32 idMapSize = 0;

	// GUID size
	idMapSize += 16;

	// ID map type
	idMapSize += sizeof(INT32);

	// Map dimension
	idMapSize += sizeof(INT32);

	// Splat rect
	idMapSize += sizeof(float) * 4;

	// Map data
	idMapSize += dimension * dimension * sizeof(UINT16);

	return idMapSize;
}

// ************************************************************************************************************************************************************

CRuSplatMapBuilder::CRuSplatMapBuilder()
{
	m_imageConvolvers = NULL;
}

CRuSplatMapBuilder::~CRuSplatMapBuilder()
{
	if(m_imageConvolvers)
	{
		for(INT32 i = 0; i < m_maxLayers; ++i)
		{
			delete m_imageConvolvers[i];
		}
		delete[] m_imageConvolvers;
	}
}

BOOL CRuSplatMapBuilder::Initialize(INT32 alphaMapDimension)
{
	m_alphaMapDimension = alphaMapDimension;

	// Setup image convolvers
	float gaussianKernel1D[7] = { 0.025f, 0.075f, 0.150f, 0.500f, 0.150f, 0.075f, 0.025f };

	m_maxLayers = 6;
	m_imageConvolvers = ruNEW CRuImageConvolver * [m_maxLayers];

	for(INT32 i = 0; i < m_maxLayers; ++i)
	{
		m_imageConvolvers[i] = ruNEW CRuImageConvolver(7, gaussianKernel1D, m_alphaMapDimension);
	}

	return TRUE;
}

BOOL CRuSplatMapBuilder::BuildAlphaMaps(CRuSplatIDMap *splatIDMap, BOOL enableFilter, RuSurfaceFormat surfaceFormat, BOOL useProxyTexture)
{
	// Perform the alpha map build only if necessary
	if((splatIDMap->m_isFiltered == FALSE && enableFilter) || splatIDMap->m_isDirty || splatIDMap->m_numAlphaMaps == 0)
	{
		// Store pointer to the original alpha maps array
		INT32 oldAlphaMapCount = splatIDMap->m_numAlphaMaps;
		CRuTexture_Proxy **oldAlphaMaps = splatIDMap->m_alphaMaps;

		// Setup data references (shortcuts)
		CRuArrayList<UINT16>&		layerIDs = splatIDMap->m_layerIDs;
		INT32&						numAlphaMaps = splatIDMap->m_numAlphaMaps;
		CRuTexture_Proxy**&			alphaMaps = splatIDMap->m_alphaMaps;

		// Cap unique splats to m_maxLayers
		INT32 numUniqueSplatIDs = min(m_maxLayers, splatIDMap->m_splatLayers.Count());

		// Calculate number of alpha maps required
		numAlphaMaps = (numUniqueSplatIDs / 3) + ((numUniqueSplatIDs % 3) ? 1 : 0);

		// Create new alpha maps array
		alphaMaps = ruNEW CRuTexture_Proxy * [numAlphaMaps];
		memset(alphaMaps, 0, sizeof(CRuTexture_Proxy *) * numAlphaMaps);

		if(oldAlphaMaps)
		{
			for(INT32 i = 0; i < oldAlphaMapCount; ++i)
			{
				if(i < numAlphaMaps && oldAlphaMaps[i]->GetSurfaceFormat() == surfaceFormat)
				{
					alphaMaps[i] = oldAlphaMaps[i];
				}
				else
				{
					ruSAFE_RELEASE(oldAlphaMaps[i]);
				}
			}

			ruSAFE_DELETE_ARRAY(oldAlphaMaps);
		}

		layerIDs.Clear();

		// Allocate any alpha maps which have not yet been allocated
		for(INT32 i = 0; i < numAlphaMaps; ++i)
		{
			if(alphaMaps[i] == NULL)
			{
				alphaMaps[i] = ruNEW CRuTexture_Proxy();
				alphaMaps[i]->Allocate(m_alphaMapDimension, m_alphaMapDimension, 1, surfaceFormat);
			}
		}

		// Clear all convolvers
		UINT8 *imageDataArray[12];
		for(INT32 curSplatIndex = 0; curSplatIndex < m_maxLayers; ++curSplatIndex)
		{
			imageDataArray[curSplatIndex] = m_imageConvolvers[curSplatIndex]->GetRawImageData();
			m_imageConvolvers[curSplatIndex]->ClearImageData();
		}

		switch(splatIDMap->m_mapType)
		{
			case 2:
				{
					// Add splat ID to the list of layers
					for(INT32 i = 0; i < splatIDMap->m_splatLayers.Count(); ++i)
					{
						layerIDs.Add(splatIDMap->m_splatLayers[i].m_id);
					}

					// Copy layer maps to alpha maps
					for(INT32 curSplatIndex = 0; curSplatIndex < numUniqueSplatIDs; ++curSplatIndex)
					{
						// Setup target pointers
						UINT8 *image = m_imageConvolvers[curSplatIndex]->GetImageData();
						INT32 imagePitch = m_imageConvolvers[curSplatIndex]->GetImagePitch();

						for(INT32 i = 0; i < splatIDMap->GetSplatLayerSize(); ++i)
						{
							memcpy(&image[i * imagePitch], &splatIDMap->m_splatLayers[curSplatIndex].m_map[i * splatIDMap->GetSplatLayerSize()], m_alphaMapDimension);
						}
					}
				}

				break;
		}

		// Examine splat maps to make sure all elements add up to one
		INT32 imagePitch = m_imageConvolvers[0]->GetImagePitch();
		for(INT32 y = 0, offset = 0; y < imagePitch; ++y)
		{
			for(INT32 x = 0; x < imagePitch; ++x, ++offset)
			{
				INT32 totalValue = 0, maxValue = 255;

				// Truncate values
				for(INT32 curSplatIndex = 0; curSplatIndex < numUniqueSplatIDs; ++curSplatIndex)
				{
					imageDataArray[curSplatIndex][offset] = imageDataArray[curSplatIndex][offset] & 0xF8;
				}

				// Calculate truncation error
				for(INT32 curSplatIndex = 0; curSplatIndex < numUniqueSplatIDs; ++curSplatIndex)
				{
					totalValue += imageDataArray[curSplatIndex][offset];
				}
				INT32 deltaError = maxValue - totalValue;

				// Attempt to fix-up the error caused by bit truncation
				for(INT32 curSplatIndex = 0; curSplatIndex < numUniqueSplatIDs && deltaError >= 0x08; ++curSplatIndex, deltaError = deltaError - 0x08)
				{
					if(imageDataArray[curSplatIndex][offset] < 0xF8)
					{
						imageDataArray[curSplatIndex][offset] += 0x08;
					}
				}
			}
		}

		// Copy splat maps to alpha maps
		for(INT32 curSplatIndex = 0, alphaMapIndex = 0; curSplatIndex < numUniqueSplatIDs; ++alphaMapIndex)
		{
			// Retrieve pointers and pitch for the source data
			UINT8 *sourceChannels[3];
			for(INT32 i = 0; i < 3; ++i, ++curSplatIndex)
				sourceChannels[i] = m_imageConvolvers[curSplatIndex]->GetImageData();
			INT32 sourcePitch = m_imageConvolvers[0]->GetImagePitch();

			switch(surfaceFormat)
			{
				case ruSURFACEFMT_X1R5G5B5:
					{
						// Lock the alpha map surface
						UINT16 *alphaBits;
						INT32 alphaPitch;
						alphaMaps[alphaMapIndex]->Lock(0, (void **) &alphaBits, &alphaPitch);

						// Copy to output channel
						INT32 pixelPadding = 0;
						for(INT32 y = 0; y < m_alphaMapDimension; ++y)
						{
							INT32 srcOffset = (y + pixelPadding) * sourcePitch + pixelPadding;
							INT32 destOffset = (y * alphaPitch) / 2;

							for(INT32 x = 0; x < m_alphaMapDimension; ++x, ++destOffset, ++srcOffset)
							{
								UINT32 c0 = (sourceChannels[0][srcOffset] & 0xF8) >> 3;
								UINT32 c1 = (sourceChannels[1][srcOffset] & 0xF8) >> 3;
								UINT32 c2 = (sourceChannels[2][srcOffset] & 0xF8) >> 3;

								alphaBits[destOffset] = (c0 << 10) | (c1 << 5) | c2;
							}
						}

						// Unlock alpha map surface
						alphaMaps[alphaMapIndex]->Unlock(0);
					}

					break;

				case ruSURFACEFMT_A8R8G8B8:
					{
						// Lock the alpha map surface
						UINT32 *alphaBits;
						INT32 alphaPitch;
						alphaMaps[alphaMapIndex]->Lock(0, (void **) &alphaBits, &alphaPitch);

						// Copy to output channel
						INT32 pixelPadding = 0;
						for(INT32 y = 0; y < m_alphaMapDimension; ++y)
						{
							INT32 srcOffset = (y + pixelPadding) * sourcePitch + pixelPadding;
							INT32 destOffset = (y * alphaPitch) / 4;

							for(INT32 x = 0; x < m_alphaMapDimension; ++x, ++destOffset, ++srcOffset)
							{
								UINT32 c0 = sourceChannels[0][srcOffset];
								UINT32 c1 = sourceChannels[1][srcOffset];
								UINT32 c2 = sourceChannels[2][srcOffset];

								alphaBits[destOffset] = RUCOLOR_ARGB(0xFF, c0, c1, c2);
							}
						}

						// Unlock alpha map surface
						alphaMaps[alphaMapIndex]->Unlock(0);
					}

					break;
			}
		}

		splatIDMap->m_isFiltered = enableFilter;
		splatIDMap->m_isDirty = FALSE;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuTextureList::CRuTextureList()
:	m_enableDelayedBinding(FALSE),
	m_enableGarbageCollection(FALSE)
{
}

CRuTextureList::~CRuTextureList()
{
	// Release all references to textures
	for(INT32 i = 0; i < m_textureList.Count(); ++i)
	{
		// Release texture name array
		ruSAFE_DELETE_ARRAY(m_textureList[i].m_textureName);

		// Release texture instance
		ruSAFE_RELEASE(m_textureList[i].m_texture);
	}
}

BOOL CRuTextureList::SerializeFrom(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuTextureList::SerializeTo(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuTextureList::Load(CRuDataStore_Base *dataStore)
{
	// Get texture list table handle
	INT32 tableHandle_TextureList = dataStore->CreateTable("texture_list", 4);

	BOOL isDeprecatedFormat = FALSE;

	for(INT32 i = 0; i < m_textureList.Count(); ++i)
	{
		// Get texture name using the index as the key
		IRuStream *stream = dataStore->Retrieve(tableHandle_TextureList, reinterpret_cast<BYTE *>(&i));

		if(stream)
		{
			// Validate GUID
			if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
			{
				char textureName[1024] = "";

				if(stream)
				{
					stream->Read(textureName, stream->GetStreamSize());
					ruSAFE_DELETE(stream);
				}

				if(strlen(textureName) > 0)
				{
					// Load the texture and set it into the texture list
					IRuBaseTexture *texture = g_ruResourceManager->LoadTexture(textureName);
					this->SetTexture(i, texture);
					ruSAFE_RELEASE(texture);
				}
				else
				{
					this->SetTexture(i, NULL);
				}

				// Set default tile scale
				this->SetTileScale(i, 24.0f);

				// Mark deprecated foramt
				isDeprecatedFormat = TRUE;
			}
			else
			{
				// Read version
				INT32 version = 1;
				stream->Read(&version, sizeof(INT32));

				// Read texture name length
				INT32 textureNameLen = 0;
				stream->Read(&textureNameLen, sizeof(INT32));

				// Read texture name
				if(textureNameLen > 1)
				{
					char *textureName = ruNEW char [textureNameLen];
					stream->Read(textureName, textureNameLen);
					this->SetTextureByName(i, textureName);
					ruSAFE_DELETE_ARRAY(textureName);
				}
				else
				{
					this->SetTexture(i, NULL);
				}

				// Read tile scale
				REAL tileScale = 1.0f;
				stream->Read(&tileScale, sizeof(REAL));
				this->SetTileScale(i, tileScale);
			}

			ruSAFE_DELETE(stream);
		}
	}

	if(isDeprecatedFormat)
	{
		// Mark all textures dirty
		this->SetTextureDirty();
	}
	else
	{
		// Reset all texture dirty flags
		this->ClearTextureDirty();
	}

	return TRUE;
}

BOOL CRuTextureList::Save(CRuDataStore_Base *dataStore, BOOL forceSave)
{
	// Get texture list table handle
	INT32 tableHandle_TextureList = dataStore->CreateTable("texture_list", 4);

	CRuMemoryStream *outStream = ruNEW CRuMemoryStream();

	for(INT32 i = 0; i < m_textureList.Count(); ++i)
	{
		TextureDescriptor &texDesc = m_textureList[i];

		if(texDesc.m_dirty || forceSave)
		{
			// Reset stream size
			outStream->SetStreamSize(0);

			// Write class GUID
			outStream->Write(ClassGUID().Data(), ClassGUID().Size());

			// Write version
			INT32 version = 1;
			outStream->Write(&version, sizeof(INT32));

			// Write texture name with its length
			const char *textureName = texDesc.m_textureName ? texDesc.m_textureName : "";
			INT32 textureNameLen = (INT32)strlen(textureName) + 1;

			outStream->Write(&textureNameLen, sizeof(INT32));
			outStream->Write(textureName, textureNameLen);

			// Write tile scale
			outStream->Write(&texDesc.m_tileScale, sizeof(REAL));

			// Output to data store
			dataStore->Store(tableHandle_TextureList, reinterpret_cast<BYTE *>(&i), outStream);
		}
	}

	ruSAFE_DELETE(outStream);

	// Reset all texture dirty flags
	this->ClearTextureDirty();

	return TRUE;
}

void CRuTextureList::EnableDelayedBinding(BOOL enableDelayedBinding)
{
	m_enableDelayedBinding = enableDelayedBinding;
}

void CRuTextureList::EnableGarbageCollection(BOOL enableGarbageCollection)
{
	m_enableGarbageCollection = enableGarbageCollection;
}

INT32 CRuTextureList::AddTexture(IRuBaseTexture *texture)
{
	TextureDescriptor texDesc = { NULL, 24.0f, 0, TRUE, NULL };

	if(texture)
	{
		// Increment reference on texture
		texture->AddRef();

		// Copy texture name
		INT32 textureNameLen = (INT32)strlen(texture->GetTextureName() + 1);
		texDesc.m_textureName = ruNEW char [textureNameLen];
		memcpy(texDesc.m_textureName, texture->GetTextureName(), textureNameLen);

		// Store pointer to texture
		texDesc.m_texture = texture;
	}

	m_textureList.Add(texDesc);

	return m_textureList.Count() - 1;
}

INT32 CRuTextureList::GetNumTextures() const
{
	return m_textureList.Count();
}

IRuBaseTexture *CRuTextureList::GetTexture(INT32 index)
{
	if(index >= 0 && index < m_textureList.Count())
	{
		// If the texture has not been bound, and the texture name is valid, bind the texture now
		if(m_textureList[index].m_texture == NULL)
		{
			if(m_textureList[index].m_textureName && m_textureList[index].m_textureName[0])
			{
				// Load texture
				m_textureList[index].m_texture = g_ruResourceManager->LoadTexture(m_textureList[index].m_textureName, ruSURFACEFMT_DEFAULT, 0, m_enableDelayedBinding);
			}
		}

		// Set GC marker
		m_textureList[index].m_gcMarker = m_currentGCCycle;

		return m_textureList[index].m_texture;
	}

	return NULL;
}

BOOL CRuTextureList::SetTexture(INT32 index, IRuBaseTexture *texture)
{
	if(index >= 0 && index < m_textureList.Count())
	{
		// Mark entry dirty
		m_textureList[index].m_dirty = TRUE;

		if(texture)
		{
			// Increment reference on texture
			ruSAFE_ADDREF(texture);

			// Decrement reference on old texture
			ruSAFE_RELEASE(m_textureList[index].m_texture);

			// Copy texture name
			ruSAFE_DELETE_ARRAY(m_textureList[index].m_textureName);
			INT32 textureNameLen = (INT32)strlen(texture->GetTextureName()) + 1;
			m_textureList[index].m_textureName = ruNEW char [textureNameLen];
			memcpy(m_textureList[index].m_textureName, texture->GetTextureName(), textureNameLen);

			// Replace texture pointer
			m_textureList[index].m_texture = texture;
		}
		else
		{
			// Decrement reference on old texture
			ruSAFE_RELEASE(m_textureList[index].m_texture);

			// Delete old texture name
			ruSAFE_DELETE_ARRAY(m_textureList[index].m_textureName);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTextureList::SetTextureByName(INT32 index, const char *textureName)
{
	if(index >= 0 && index < m_textureList.Count())
	{
		// Mark entry dirty
		m_textureList[index].m_dirty = TRUE;

		if(textureName)
		{
			// Decrement reference on old texture
			ruSAFE_RELEASE(m_textureList[index].m_texture);

			// Copy texture name
			ruSAFE_DELETE_ARRAY(m_textureList[index].m_textureName);
			INT32 textureNameLen = (INT32)strlen(textureName) + 1;
			m_textureList[index].m_textureName = ruNEW char [textureNameLen];
			memcpy(m_textureList[index].m_textureName, textureName, textureNameLen);
		}
		else
		{
			// Decrement reference on old texture
			ruSAFE_RELEASE(m_textureList[index].m_texture);

			// Delete old texture name
			ruSAFE_DELETE_ARRAY(m_textureList[index].m_textureName);
		}

		return TRUE;
	}

	return FALSE;
}

REAL CRuTextureList::GetTileScale(INT32 index)
{
	if(index >= 0 && index < m_textureList.Count())
	{
		return m_textureList[index].m_tileScale;
	}

	return 24.0f;
}

void CRuTextureList::SetTileScale(INT32 index, REAL tileScale)
{
	if(index >= 0 && index < m_textureList.Count())
	{
		// Mark entry dirty
		m_textureList[index].m_dirty = TRUE;

		m_textureList[index].m_tileScale = tileScale;
	}
}

void CRuTextureList::ClearTextureDirty()
{
	for(INT32 i = 0; i < m_textureList.Count(); ++i)
	{
		m_textureList[i].m_dirty = FALSE;
	}
}

void CRuTextureList::SetTextureDirty()
{
	for(INT32 i = 0; i < m_textureList.Count(); ++i)
	{
		m_textureList[i].m_dirty = TRUE;
	}
}

BOOL CRuTextureList::IsTextureDirty(INT32 index) const
{
	if(index >= 0 && index < m_textureList.Count())
	{
		return m_textureList[index].m_dirty;
	}

	return FALSE;
}

BOOL CRuTextureList::GarbageCollect()
{
	if(m_enableGarbageCollection)
	{
		// Increment current GC cycle
		++m_currentGCCycle;

		// Perform garbage collection once every 30 cycles
		INT32 collectionPeriod = 30;
		if((m_currentGCCycle % collectionPeriod) == 0)
		{
			for(INT32 i = 0; i < m_textureList.Count(); ++i)
			{
				if(m_textureList[i].m_texture)
				{
					INT32 gcAge = m_currentGCCycle - m_textureList[i].m_gcMarker;
					if(gcAge > 500)
					{
						ruSAFE_RELEASE(m_textureList[i].m_texture);
					}
				}
			}
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

INT32 RuTerrainUtility_BuildGridIndices(INT32 chunkDimension, INT32 gridDimension, INT32 vertexGap, INT32 stride, BYTE *vertexFlags, UINT16 *indicesOut, INT32 *numMainBodyIndices, BOOL ignoreSkirt)
{
	INT32 evenFaceConstruction[]	= { 0, 3, 4, 0, 4, 1, 1, 4, 2, 2, 4, 5 };
	INT32 oddFaceConstruction[]		= { 0, 3, 1, 1, 3, 4, 1, 5, 2, 1, 4, 5 };

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

			BOOL visible = FALSE;
			for(INT32 j = 0; j < 12; ++j)
			{
				if(vertexFlags[indexSet[faceConstruction[j]]] != 0)
				{
					visible = TRUE;
				}
			}

			if(visible)
			{
				for(INT32 j = 0; j < 12; ++j)
				{
					indicesOut[curIndex++] = indexSet[faceConstruction[j]];
				}
			}
			else
			{
				for(INT32 j = 0; j < 12; ++j)
				{
					indicesOut[curIndex++] = 0;
				}
			}
		}
	}

	// If a pointer is specified for mainBodyIndexEnd, record the end of the main terrain chunk body indices into it
	if(numMainBodyIndices)
	{
		*numMainBodyIndices = curIndex;
	}

	if(ignoreSkirt == FALSE)
	{
		INT32 topSkirtVertexArrayOffset = (chunkDimension + 1) * (chunkDimension + 1);
		INT32 bottomSkirtVertexArrayOffset = topSkirtVertexArrayOffset + (chunkDimension + 1);
		INT32 leftSkirtVertexArrayOffset = bottomSkirtVertexArrayOffset + (chunkDimension + 1);
		INT32 rightSkirtVertexArrayOffset = leftSkirtVertexArrayOffset + (chunkDimension + 1);

		// Build indices for the mesh skirt (top)
		for(INT32 x = 0, z = 0; x < gridDimension; ++x)
		{
			if(vertexFlags[(z * zStride) + (x * vertexGap)] == 0 && vertexFlags[(z * zStride) + (x * vertexGap) + vertexGap] == 0)
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
			if(vertexFlags[(z * zStride) + (x * vertexGap)] == 0 && vertexFlags[(z * zStride) + (x * vertexGap) + vertexGap] == 0)
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
			if(vertexFlags[(z * zStride) + (x * vertexGap)] == 0 && vertexFlags[(z * zStride) + (x * vertexGap) + zStride] == 0)
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
			if(vertexFlags[(z * zStride) + (x * vertexGap)] == 0 && vertexFlags[(z * zStride) + (x * vertexGap) + zStride] == 0)
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
	}

	return curIndex;
}

// ************************************************************************************************************************************************************

INT32 RuTerrainUtility_BuildGridIndicesEx(INT32 gridDimension, INT32 stride, INT32 pitch, INT32 vertexOffset, BYTE *vertexFlags, UINT16 *indicesOut)
{
	INT32 evenFaceConstruction[]    = { 0, 3, 4, 0, 4, 1, 1, 4, 2, 2, 4, 5, 0xFF };
	INT32 oddFaceConstruction[]     = { 0, 3, 1, 1, 3, 4, 1, 5, 2, 1, 4, 5, 0xFF };

	INT32 curIndex = 0;
	UINT16 indexSet[6];

	// Build indices for the mesh body
	for(INT32 z = 0; z < gridDimension; ++z)
	{
		// Build indices for four faces at a time
		for(INT32 x = 0; x < gridDimension; x += 2)
		{
			// Calculate the indices used in these four faces
			indexSet[0] = (z * pitch) + (x * stride) + vertexOffset;
			indexSet[1] = indexSet[0] + stride;
			indexSet[2] = indexSet[1] + stride;
			indexSet[3] = indexSet[0] + pitch;
			indexSet[4] = indexSet[1] + pitch;
			indexSet[5] = indexSet[2] + pitch;

			// Alternate between even and odd rows
			INT32 *faceConstruction = (z % 2) ? oddFaceConstruction : evenFaceConstruction;

			// Determine if the face is visible
			for(INT32 j = 0; faceConstruction[j] != 0xFF; j += 6)
			{
				BOOL visible = FALSE;

				if(vertexFlags)
				{
					for(INT32 k = j; (k < j + 6) && faceConstruction[k] != 0xFF; ++k)
					{
						if(vertexFlags[indexSet[faceConstruction[k]]])
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

	return curIndex;
}

// ************************************************************************************************************************************************************

INT32 RuTerrainUtility_BuildIndices_TileLink(INT32 linkMask, INT32 gridDimension, INT32 stride, INT32 pitch, INT32 originalPitch, BYTE *vertexFlags, UINT16 *indicesOut)
{
	INT32 masks[] = { 0x1, 0x2, 0x4, 0x8 };

	INT32 tileIdxOrder_Border_0[] = { 0, 4, 2, 2, 4, 5, 0xFF };
	INT32 tileIdxOrder_Border_1[] = { 0, 3, 4, 0, 4, 2, 0xFF };
	INT32 tileIdxOrder_Interior[] = { 0, 3, 4, 0, 4, 2, 2, 4, 5, 0xFF };

	INT32 tileIdxOrder_Original_Border_0[] = { 0, 4, 1, 1, 4, 2, 2, 4, 5, 0xFF };
	INT32 tileIdxOrder_Original_Border_1[] = { 0, 3, 4, 0, 4, 1, 1, 4, 2, 0xFF };
	INT32 tileIdxOrder_Original_Interior[] = { 0, 3, 4, 0, 4, 1, 1, 4, 2, 2, 4, 5, 0xFF };

	INT32 skirtIdxOrder_Link[] = { 0, 8, 6, 2, 8, 0, 0xFF };
	INT32 skirtIdxOrder_Original[] = { 0, 7, 6, 0, 1, 7, 1, 2, 7, 2, 8, 7, 0xFF };

	INT32 blankOrder[] = { 0xFF };

	INT32 curIndex = 0;
	UINT16 indexSet[9];

	INT32 skirtVtxOffset[] =	{
									(originalPitch + 0) * (originalPitch + 0),	// "Top"
									(originalPitch + 0) * (originalPitch + 3),	// "Left"
									(originalPitch + 0) * (originalPitch + 1),	// "Bottom"
									(originalPitch + 0) * (originalPitch + 2)	// "Right"
								};

	for(INT32 i = 0; i < 4; ++i)
	{
		INT32 tileMask = masks[i];

		// Build indices for two quads at a time
		for(INT32 x = 0; x < gridDimension; x += 2)
		{
			// Calculate the indices used in these two quads
			switch(tileMask)
			{
				case 0x1:	// X = min
					{
						/*
							indexSet points defined as follows:

							6----7----8
							   SKIRT
							0----1----2
							  SURFACE
							3----4----5
						*/

						indexSet[0] = (x * stride);
						indexSet[1] = indexSet[0] + stride;
						indexSet[2] = indexSet[1] + stride;
						indexSet[3] = indexSet[0] + pitch;
						indexSet[4] = indexSet[1] + pitch;
						indexSet[5] = indexSet[2] + pitch;

						// Setup indices to skirt vertices
						indexSet[6] = skirtVtxOffset[i] + (x * stride);
						indexSet[7] = indexSet[6] + stride;
						indexSet[8] = indexSet[7] + stride;
					}

					break;

				case 0x2:	// Y = max?
					{
						indexSet[0] = (x * pitch) + originalPitch - 1;
						indexSet[1] = indexSet[0] + pitch;
						indexSet[2] = indexSet[1] + pitch;
						indexSet[3] = indexSet[0] - stride;
						indexSet[4] = indexSet[1] - stride;
						indexSet[5] = indexSet[2] - stride;

						// Setup indices to skirt vertices
						indexSet[6] = skirtVtxOffset[i] + (x * stride);
						indexSet[7] = indexSet[6] + stride;
						indexSet[8] = indexSet[7] + stride;
					}

					break;

				case 0x4:	// X = max
					{
						indexSet[0] = (originalPitch * originalPitch - 1) - (x * stride);
						indexSet[1] = indexSet[0] - stride;
						indexSet[2] = indexSet[1] - stride;
						indexSet[3] = indexSet[0] - pitch;
						indexSet[4] = indexSet[1] - pitch;
						indexSet[5] = indexSet[2] - pitch;

						// Setup indices to skirt vertices
						indexSet[6] = (skirtVtxOffset[i] + originalPitch - 1) - (x * stride);
						indexSet[7] = indexSet[6] - stride;
						indexSet[8] = indexSet[7] - stride;
					}

					break;

				case 0x8:	// Y = min?
					{
						indexSet[0] = (originalPitch * (originalPitch - 1)) - x * pitch;
						indexSet[1] = indexSet[0] - pitch;
						indexSet[2] = indexSet[1] - pitch;
						indexSet[3] = indexSet[0] + stride;
						indexSet[4] = indexSet[1] + stride;
						indexSet[5] = indexSet[2] + stride;

						// Setup indices to skirt vertices
						indexSet[6] = (skirtVtxOffset[i] + originalPitch - 1) - (x * stride);
						indexSet[7] = indexSet[6] - stride;
						indexSet[8] = indexSet[7] - stride;
					}

					break;
			}

			// Select the construction array to use
			INT32 *tileIdxOrder = (linkMask & tileMask) ? ((x == 0) ? tileIdxOrder_Border_0 : ((x == gridDimension - 2) ? tileIdxOrder_Border_1 : tileIdxOrder_Interior)) : ((x == 0) ? tileIdxOrder_Original_Border_0 : ((x == gridDimension - 2) ? tileIdxOrder_Original_Border_1 : tileIdxOrder_Original_Interior));
			INT32 *skirtIdxOrder = (linkMask & tileMask) ? skirtIdxOrder_Link : skirtIdxOrder_Original;

			// Determine if the face is visible (is this correct? we're checking four faces/two quads at the same time!)
			BOOL visible = FALSE;
			for(INT32 j = 0; tileIdxOrder[j] != 0xFF; ++j)
			{
				if(vertexFlags[indexSet[tileIdxOrder[j]]] != 0)
				{
					visible = TRUE;
				}
			}

			if(visible)
			{
				for(INT32 j = 0; tileIdxOrder[j] != 0xFF; ++j)
				{
					indicesOut[curIndex++] = indexSet[tileIdxOrder[j]];
				}

				for(INT32 j = 0; skirtIdxOrder[j] != 0xFF; ++j)
				{
					indicesOut[curIndex++] = indexSet[skirtIdxOrder[j]];
				}
			}
		}
	}

	return curIndex;
}

// ************************************************************************************************************************************************************

INT32 RuTerrainUtility_BuildIndices_Skirt(INT32 gridDimension, INT32 stride, INT32 pitch, INT32 originalPitch, BYTE *vertexFlags, UINT16 *indicesOut)
{
	INT32 masks[] = { 0x1, 0x2, 0x4, 0x8 };

	INT32 tileIdxOrder_Border_0[] = { 0, 4, 2, 2, 4, 5, 0xFF };
	INT32 tileIdxOrder_Border_1[] = { 0, 3, 4, 0, 4, 2, 0xFF };
	INT32 tileIdxOrder_Interior[] = { 0, 3, 4, 0, 4, 2, 2, 4, 5, 0xFF };

	INT32 tileIdxOrder_Original_Border_0[] = { 0, 4, 1, 1, 4, 2, 2, 4, 5, 0xFF };
	INT32 tileIdxOrder_Original_Border_1[] = { 0, 3, 4, 0, 4, 1, 1, 4, 2, 0xFF };
	INT32 tileIdxOrder_Original_Interior[] = { 0, 3, 4, 0, 4, 1, 1, 4, 2, 2, 4, 5, 0xFF };

	INT32 skirtIdxOrder_Link[] = { 0, 7, 6, 0, 2, 7, 2, 8, 7, 0xFF };
	INT32 skirtIdxOrder_Original[] = { 0, 7, 6, 0, 1, 7, 1, 2, 7, 2, 8, 7, 0xFF };

	INT32 blankOrder[] = { 0xFF };

	INT32 curIndex = 0;
	UINT16 indexSet[9];

	INT32 skirtVtxOffset[] =	{
									(originalPitch + 0) * (originalPitch + 0),	// "Top"
									(originalPitch + 0) * (originalPitch + 3),	// "Left"
									(originalPitch + 0) * (originalPitch + 1),	// "Bottom"
									(originalPitch + 0) * (originalPitch + 2)	// "Right"
								};

	for(INT32 i = 0; i < 4; ++i)
	{
		INT32 tileMask = masks[i];

		// Build indices for two quads at a time
		for(INT32 x = 0; x < gridDimension; x += 2)
		{
			// Calculate the indices used in these two quads
			switch(tileMask)
			{
				case 0x1:	// X = min
					{
						indexSet[0] = (x * stride);
						indexSet[1] = indexSet[0] + stride;
						indexSet[2] = indexSet[1] + stride;
						indexSet[3] = indexSet[0] + pitch;
						indexSet[4] = indexSet[1] + pitch;
						indexSet[5] = indexSet[2] + pitch;

						// Setup indices to skirt vertices
						indexSet[6] = skirtVtxOffset[i] + (x * stride);
						indexSet[7] = indexSet[6] + stride;
						indexSet[8] = indexSet[7] + stride;
					}

					break;

				case 0x2:	// Y = max?
					{
						indexSet[0] = (x * pitch) + originalPitch - 1;
						indexSet[1] = indexSet[0] + pitch;
						indexSet[2] = indexSet[1] + pitch;
						indexSet[3] = indexSet[0] - stride;
						indexSet[4] = indexSet[1] - stride;
						indexSet[5] = indexSet[2] - stride;

						// Setup indices to skirt vertices
						indexSet[6] = skirtVtxOffset[i] + (x * stride);
						indexSet[7] = indexSet[6] + stride;
						indexSet[8] = indexSet[7] + stride;
					}

					break;

				case 0x4:	// X = max
					{
						indexSet[0] = (originalPitch * originalPitch - 1) - (x * stride);
						indexSet[1] = indexSet[0] - stride;
						indexSet[2] = indexSet[1] - stride;
						indexSet[3] = indexSet[0] - pitch;
						indexSet[4] = indexSet[1] - pitch;
						indexSet[5] = indexSet[2] - pitch;

						// Setup indices to skirt vertices
						indexSet[6] = (skirtVtxOffset[i] + originalPitch - 1) - (x * stride);
						indexSet[7] = indexSet[6] - stride;
						indexSet[8] = indexSet[7] - stride;
					}

					break;

				case 0x8:	// Y = min?
					{
						indexSet[0] = (originalPitch * (originalPitch - 1)) - x * pitch;
						indexSet[1] = indexSet[0] - pitch;
						indexSet[2] = indexSet[1] - pitch;
						indexSet[3] = indexSet[0] + stride;
						indexSet[4] = indexSet[1] + stride;
						indexSet[5] = indexSet[2] + stride;

						// Setup indices to skirt vertices
						indexSet[6] = (skirtVtxOffset[i] + originalPitch - 1) - (x * stride);
						indexSet[7] = indexSet[6] - stride;
						indexSet[8] = indexSet[7] - stride;
					}

					break;
			}

			// Select the construction array to use
			INT32 *tileIdxOrder = tileIdxOrder_Original_Interior;
			INT32 *skirtIdxOrder = skirtIdxOrder_Original;

			// Determine if the face is visible (is this correct? we're checking four faces/two quads at the same time!)
			BOOL visible = FALSE;
			if(vertexFlags)
			{
				for(INT32 j = 0; tileIdxOrder[j] != 0xFF; ++j)
				{
					if(vertexFlags[indexSet[tileIdxOrder[j]]] != 0)
					{
						visible = TRUE;
					}
				}
			}
			else
			{
				visible = TRUE;
			}

			if(visible)
			{
				for(INT32 j = 0; skirtIdxOrder[j] != 0xFF; ++j)
				{
					indicesOut[curIndex++] = indexSet[skirtIdxOrder[j]];
				}
			}
		}
	}

	return curIndex;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
