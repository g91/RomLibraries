/*!
	@file RuTexture_Proxy.cpp

	Copyright (c) 2006-2007 All rights reserved
*/

#include "../../Engine/Texture/RuTexture_Proxy.h"
#include "../../Engine/Texture/RuTextureUtility.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuSurface, IRuSurface)
ruHEAP_DEFINE_SUBCLASS(CRuSurface, "CRuSurface", "IRuSurface")
ruCLASSGUID_DEFINE(CRuSurface, 0x5F1BEAD4, 0x303D4e1f, 0xAEEB609A, 0xFE6490AB)

ruRTTI_DEFINE_SUBCLASS(CRuTexture_Proxy, IRuTexture)
ruHEAP_DEFINE_SUBCLASS(CRuTexture_Proxy, "CRuTexture_Proxy", "IRuTexture")
ruCLASSGUID_DEFINE(CRuTexture_Proxy, 0x5E0E0094, 0x1CAB45f1, 0xB766B517, 0xDC78ED25)

ruRTTI_DEFINE_SUBCLASS(CRuVolumeTexture_Proxy, IRuVolumeTexture)
ruHEAP_DEFINE_SUBCLASS(CRuVolumeTexture_Proxy, "CRuVolumeTexture_Proxy", "IRuVolumeTexture")
ruCLASSGUID_DEFINE(CRuVolumeTexture_Proxy, 0xC593266F, 0x06DD4bde, 0x83F6A99A, 0x93795BB3)

// ************************************************************************************************************************************************************

CRuSurface::CRuSurface()
:	m_width(4),
	m_height(4),
	m_surfaceFormat(ruSURFACEFMT_A8R8G8B8),
	m_surfaceData(NULL),
	m_sharedMemory(FALSE)
{
}

CRuSurface::CRuSurface(UINT32 width, UINT32 height, RuSurfaceFormat surfaceFormat)
:	m_width(width),
	m_height(height),
	m_surfaceFormat(surfaceFormat),
	m_surfaceData(NULL)
{
	INT32 bitsPerTexel = RuSurfaceFormat_GetBitsPerTexel(m_surfaceFormat);
	m_surfaceData = ruNEW BYTE [m_width * m_height * bitsPerTexel / 8];
}

CRuSurface::CRuSurface(UINT32 width, UINT32 height, RuSurfaceFormat surfaceFormat, BYTE *surfaceData)
:	m_width(width),
	m_height(height),
	m_surfaceFormat(surfaceFormat),
	m_surfaceData(surfaceData),
	m_sharedMemory(TRUE)
{
}

CRuSurface::~CRuSurface()
{
	if(!m_sharedMemory)
	{
		delete[] m_surfaceData;
	}
}

BOOL CRuSurface::SerializeFrom(IRuStream *stream)
{
	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuSurface::SerializeFrom(stream);

	switch(version)
	{
		case 1:
			{
				// Read surface parameters
				stream->Read(&m_width, sizeof(UINT32));
				stream->Read(&m_height, sizeof(UINT32));
				stream->Read(&m_surfaceFormat, sizeof(RuSurfaceFormat));

				// Calculate space required for texture
				INT32 bitsPerTexel = RuSurfaceFormat_GetBitsPerTexel(m_surfaceFormat);
				INT32 bytesForImage = m_width * m_height * bitsPerTexel / 8;

				// Allocate space for surface data
				m_surfaceData = ruNEW BYTE [bytesForImage];

				// Read surface data
				RuSurfaceUtility_SerializeFrom(this, stream);
			}

			break;
	}

	return TRUE;
}

BOOL CRuSurface::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuSurface::SerializeTo(stream);

	// Write surface parameters
	stream->Write(&m_width, sizeof(UINT32));
	stream->Write(&m_height, sizeof(UINT32));
	stream->Write(&m_surfaceFormat, sizeof(RuSurfaceFormat));

	// Write surface data
	RuSurfaceUtility_SerializeTo(this, stream);

	return TRUE;
}

void CRuSurface::Invalidate()
{
}

BOOL CRuSurface::IsInvalidated()
{
	return FALSE;
}

BOOL CRuSurface::Restore()
{
	return TRUE;
}

RuResourcePool CRuSurface::GetPool()
{
	return ruRESPOOL_SYSTEMMEMORY;
}

RuSurfaceFormat CRuSurface::GetSurfaceFormat()
{
	return m_surfaceFormat;
}

UINT32 CRuSurface::GetWidth() const
{
	return m_width;
}

UINT32 CRuSurface::GetHeight() const
{
	return m_height;
}

BOOL CRuSurface::Lock(void **bitsOut, INT32 *pitchOut)
{
	INT32 bitsPerTexel = RuSurfaceFormat_GetBitsPerTexel(m_surfaceFormat);

	*bitsOut = reinterpret_cast<void *>(m_surfaceData);

	switch(m_surfaceFormat)
	{
		default:
			*pitchOut = m_width * bitsPerTexel / 8;
			break;

		case ruSURFACEFMT_DXT1:
		case ruSURFACEFMT_DXT2:
		case ruSURFACEFMT_DXT3:
		case ruSURFACEFMT_DXT4:
		case ruSURFACEFMT_DXT5:
			*pitchOut = (m_width >> 2) * (bitsPerTexel << 1);
			break;
	}

	return TRUE;
}

BOOL CRuSurface::Unlock()
{
	return TRUE;
}

BOOL CRuSurface::SelectAsRenderTarget(INT32 rtIndex)
{
	return FALSE;
}

BOOL CRuSurface::SelectDepthStencil()
{
	return FALSE;
}

// ************************************************************************************************************************************************************

CRuTexture_Proxy::CRuTexture_Proxy()
:	m_isInvalidated(FALSE),
	m_proxyTexture(NULL),
	m_textureWidth(0),
	m_textureHeight(0),
	m_textureData(NULL),
	m_levelCount(0),
	m_surfaceFormat(ruSURFACEFMT_DEFAULT),
	m_lod(0),
	m_levelSurfaces(NULL)
{
}

CRuTexture_Proxy::~CRuTexture_Proxy()
{
	Deallocate();
}

BOOL CRuTexture_Proxy::Allocate(IRuTexture *proxyTexture)
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	if(proxyTexture)
	{
		// Increment ref count on texture
		proxyTexture->AddRef();

		// Deallocate existing texture space
		Deallocate();

		// Store reference to texture
		m_proxyTexture = proxyTexture;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuTexture_Proxy::Allocate(UINT32 width, UINT32 height, UINT32 mipLevels, RuSurfaceFormat surfaceFormat)
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	// Deallocate existing texture space
	Deallocate();

	// Set new texture parameters
	m_textureWidth = width;
	m_textureHeight = height;
	m_levelCount = mipLevels;
	m_surfaceFormat = surfaceFormat;
	m_lod = 0;

	INT32 levelCount = m_levelCount == 0 ? 256 : m_levelCount;

	INT32 bitsPerTexel = RuSurfaceFormat_GetBitsPerTexel(m_surfaceFormat);
	INT32 bytesForImage = 0;

	// Clear level count
	m_levelCount = 0;

	while(width > 0 && height > 0 && levelCount > 0)
	{
		// Accumulate space required for image
		bytesForImage += (width * height * bitsPerTexel / 8);

		// Move on to next level
		width = width >> 1;
		height = height >> 1;
		--levelCount;

		// Accumulate level count
		++m_levelCount;
	}

	m_textureData = ruNEW BYTE [bytesForImage];
	memset(m_textureData, 0, bytesForImage);

	// Setup level surfaces
	m_levelSurfaces = ruNEW CRuSurface * [m_levelCount];
	for(UINT32 i = 0, width = m_textureWidth, height = m_textureHeight, offset = 0; i < m_levelCount; ++i)
	{
		m_levelSurfaces[i] = ruNEW CRuSurface(width, height, m_surfaceFormat, &m_textureData[offset]);

		// Move on to next level
		offset += (width * height * bitsPerTexel / 8);
		width = width >> 1;
		height = height >> 1;
	}

	return TRUE;
}

BOOL CRuTexture_Proxy::Deallocate()
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	// Release texture that we are proxying for
	ruSAFE_RELEASE(m_proxyTexture);

	// Release texture data
	ruSAFE_DELETE_ARRAY(m_textureData);

	// Release level surfaces
	if(m_levelSurfaces)
	{
		for(UINT32 i = 0; i < m_levelCount; ++i)
		{
			ruSAFE_RELEASE(m_levelSurfaces[i]);
		}

		ruSAFE_DELETE_ARRAY(m_levelSurfaces);
	}

	// Reset parameters
	m_textureWidth = 0;
	m_textureHeight = 0;
	m_levelCount = 0;
	m_lod = 0;

	return TRUE;
}

BOOL CRuTexture_Proxy::SerializeFrom(IRuStream *stream)
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuTexture::SerializeFrom(stream);

	switch(version)
	{
		case 1:
			{
				// Read texture parameters
				stream->Read(&m_textureWidth, sizeof(UINT32));
				stream->Read(&m_textureHeight, sizeof(UINT32));
				stream->Read(&m_levelCount, sizeof(DWORD));
				stream->Read(&m_surfaceFormat, sizeof(RuSurfaceFormat));
				stream->Read(&m_lod, sizeof(UINT32));

				// Calculate space required for texture
				INT32 width = m_textureWidth;
				INT32 height = m_textureHeight;
				INT32 levelCount = m_levelCount == 0 ? 256 : m_levelCount;

				INT32 bitsPerTexel = RuSurfaceFormat_GetBitsPerTexel(m_surfaceFormat);
				INT32 bytesForImage = 0;

				while(width > 0 && height > 0 && levelCount > 0)
				{
					// Accumulate space required for image
					bytesForImage += (width * height * bitsPerTexel / 8);

					// Move on to next level
					width = width >> 1;
					height = height >> 1;
					--levelCount;
				}

				// Allocate space for texture
				m_textureData = ruNEW BYTE [bytesForImage];

				// Read each surface level
				for(UINT32 i = 0; i < m_levelCount; ++i)
				{
					RuTextureUtility_SerializeFrom(this, i, stream);
				}

				// Setup level surfaces
				m_levelSurfaces = ruNEW CRuSurface * [m_levelCount];
				for(UINT32 i = 0, width = m_textureWidth, height = m_textureHeight, offset = 0; i < m_levelCount; ++i)
				{
					m_levelSurfaces[i] = ruNEW CRuSurface(width, height, m_surfaceFormat, &m_textureData[offset]);

					// Move on to next level
					offset += (width * height * bitsPerTexel / 8);
					width = width >> 1;
					height = height >> 1;
				}
			}

			break;
	}

	return TRUE;
}

BOOL CRuTexture_Proxy::SerializeTo(IRuStream *stream)
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuTexture::SerializeTo(stream);

	// Write texture parameters
	stream->Write(&m_textureWidth, sizeof(UINT32));
	stream->Write(&m_textureHeight, sizeof(UINT32));
	stream->Write(&m_levelCount, sizeof(DWORD));
	stream->Write(&m_surfaceFormat, sizeof(RuSurfaceFormat));
	stream->Write(&m_lod, sizeof(UINT32));

	// Write each surface level
	for(UINT32 i = 0; i < m_levelCount; ++i)
	{
		RuTextureUtility_SerializeTo(this, i, stream);
	}

	return TRUE;
}

void CRuTexture_Proxy::Invalidate()
{
	if(m_proxyTexture)
	{
		return m_proxyTexture->Invalidate();
	}
	else
	{
		m_isInvalidated = TRUE;
	}
}

BOOL CRuTexture_Proxy::IsInvalidated()
{
	if(m_proxyTexture)
	{
		return m_proxyTexture->IsInvalidated();
	}
	else
	{
		return m_isInvalidated;
	}
}

BOOL CRuTexture_Proxy::Restore()
{
	if(m_proxyTexture)
	{
		return m_proxyTexture->Restore();
	}
	else
	{
		m_isInvalidated = FALSE;
	}

	return TRUE;
}

const char *CRuTexture_Proxy::GetTextureName()
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	return GetObjectName();
}

void CRuTexture_Proxy::SetTextureName(const char *textureName)
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	SetObjectName(textureName);
}

void CRuTexture_Proxy::Select(UINT32 samplerIndex)
{
	if(m_proxyTexture)
	{
		m_proxyTexture->Select(samplerIndex);
	}
}

BOOL CRuTexture_Proxy::GenerateMipLevels()
{
	return FALSE;
}

BOOL CRuTexture_Proxy::IsDynamic()
{
	return FALSE;
}

BOOL CRuTexture_Proxy::IsRenderTarget()
{
	return FALSE;
}

UINT32 CRuTexture_Proxy::GetLevelCount()
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	if(m_proxyTexture)
	{
		return m_proxyTexture->GetLevelCount();
	}
	else
	{
		return m_levelCount;
	}
}

UINT32 CRuTexture_Proxy::GetLOD()
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	if(m_proxyTexture)
	{
		return m_proxyTexture->GetLOD();
	}
	else
	{
		return m_lod;
	}
}

UINT32 CRuTexture_Proxy::SetLOD(UINT32 lod)
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	if(m_proxyTexture)
	{
		return m_proxyTexture->SetLOD(lod);
	}
	else
	{
		return m_lod = lod;
	}
}

RuResourcePool CRuTexture_Proxy::GetPool()
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	if(m_proxyTexture)
	{
		return m_proxyTexture->GetPool();
	}
	else
	{
		return ruRESPOOL_SYSTEMMEMORY;
	}
}

RuSurfaceFormat CRuTexture_Proxy::GetSurfaceFormat()
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	if(m_proxyTexture)
	{
		return m_proxyTexture->GetSurfaceFormat();
	}
	else
	{
		return m_surfaceFormat;
	}
}

BOOL CRuTexture_Proxy::SelectAsRenderTarget(INT32 rtIndex, UINT32 level)
{
	return FALSE;
}

UINT32 CRuTexture_Proxy::GetLevelWidth(UINT32 level)
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	if(m_proxyTexture)
	{
		return m_proxyTexture->GetLevelWidth(level);
	}
	else
	{
		return m_textureWidth >> level;
	}
}

UINT32 CRuTexture_Proxy::GetLevelHeight(UINT32 level)
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	if(m_proxyTexture)
	{
		return m_proxyTexture->GetLevelHeight(level);
	}
	else
	{
		return m_textureHeight >> level;
	}
}

IRuSurface *CRuTexture_Proxy::GetLevelSurface(UINT32 level)
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	if(m_proxyTexture)
	{
		return m_proxyTexture->GetLevelSurface(level);
	}
	else
	{
		return level < m_levelCount ? m_levelSurfaces[level] : NULL;
	}
}

BOOL CRuTexture_Proxy::Lock(UINT32 level, void **bitsOut, INT32 *pitchOut)
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	if(m_proxyTexture)
	{
		return m_proxyTexture->Lock(level, bitsOut, pitchOut);
	}
	else
	{
		INT32 width = m_textureWidth;
		INT32 height = m_textureHeight;
		INT32 curLevel = 0;

		INT32 bitsPerTexel = RuSurfaceFormat_GetBitsPerTexel(m_surfaceFormat);
		INT32 offset = 0;

		while(width > 0 && height > 0 && curLevel != level)
		{
			// Accumulate offset required for mip level
			offset += (width * height * bitsPerTexel / 8);

			// Move on to next level
			width = width >> 1;
			height = height >> 1;
			++curLevel;
		}

		if(width > 0 && height > 0)
		{
			*bitsOut = &m_textureData[offset];

			switch(m_surfaceFormat)
			{
				default:
					*pitchOut = width * bitsPerTexel / 8;
					break;

				case ruSURFACEFMT_DXT1:
				case ruSURFACEFMT_DXT2:
				case ruSURFACEFMT_DXT3:
				case ruSURFACEFMT_DXT4:
				case ruSURFACEFMT_DXT5:
					*pitchOut = (width >> 2) * (RuSurfaceFormat_GetBitsPerTexel(m_surfaceFormat) << 1);
					break;
			}

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuTexture_Proxy::Unlock(UINT32 level)
{
	// Initialize scope lock
	CRuThread_ScopeLock scopeLock(&m_cs);

	if(m_proxyTexture)
	{
		return m_proxyTexture->Unlock(level);
	}
	else
	{
		++m_iteration;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuVolumeTexture_Proxy::CRuVolumeTexture_Proxy()
:	m_proxyTexture(NULL),
	m_textureWidth(0),
	m_textureHeight(0),
	m_textureDepth(0),
	m_textureData(NULL),
	m_levelCount(0),
	m_surfaceFormat(ruSURFACEFMT_DEFAULT),
	m_lod(0)
{
}

CRuVolumeTexture_Proxy::CRuVolumeTexture_Proxy(UINT32 width, UINT32 height, UINT32 depth, UINT32 mipLevels, RuSurfaceFormat surfaceFormat)
:	m_proxyTexture(NULL),
	m_textureWidth(0),
	m_textureHeight(0),
	m_textureDepth(0),
	m_textureData(NULL),
	m_levelCount(0),
	m_surfaceFormat(ruSURFACEFMT_DEFAULT),
	m_lod(0)
{
	Allocate(width, height, depth, mipLevels, surfaceFormat);
}

CRuVolumeTexture_Proxy::~CRuVolumeTexture_Proxy()
{
	// Deallocate existing texture space
	Deallocate();
}

BOOL CRuVolumeTexture_Proxy::Allocate(IRuVolumeTexture *proxyTexture)
{
	if(proxyTexture)
	{
		// Increment ref count on texture
		proxyTexture->AddRef();

		// Deallocate existing texture space
		Deallocate();

		// Store reference to texture
		m_proxyTexture = proxyTexture;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuVolumeTexture_Proxy::Allocate(UINT32 width, UINT32 height, UINT32 depth, UINT32 mipLevels, RuSurfaceFormat surfaceFormat)
{
	// Deallocate existing texture space
	Deallocate();

	// Set parameters
	m_textureWidth = width;
	m_textureHeight = height;
	m_textureDepth = depth;
	m_textureData = NULL;
	m_levelCount = mipLevels;
	m_surfaceFormat = surfaceFormat;
	m_lod = 0;

	INT32 levelCount = m_levelCount == 0 ? 256 : m_levelCount;

	INT32 bitsPerTexel = RuSurfaceFormat_GetBitsPerTexel(m_surfaceFormat);
	INT32 bytesForImage = 0;

	// Clear level count
	m_levelCount = 0;

	while(width > 0 && height > 0 && depth > 0 && levelCount > 0)
	{
		// Accumulate space required for image
		bytesForImage += (width * height * depth * bitsPerTexel / 8);

		// Move on to next level
		width = width >> 1;
		height = height >> 1;
		depth = depth >> 1;
		--levelCount;

		// Accumulate level count
		++m_levelCount;
	}

	m_textureData = ruNEW BYTE [bytesForImage];

	return TRUE;
}

BOOL CRuVolumeTexture_Proxy::Deallocate()
{
	ruSAFE_RELEASE(m_proxyTexture);

	ruSAFE_DELETE_ARRAY(m_textureData);

	// Reset parameters
	m_textureWidth = 0;
	m_textureHeight = 0;
	m_textureDepth = 0;
	m_levelCount = 0;
	m_surfaceFormat = ruSURFACEFMT_DEFAULT;
	m_lod = 0;

	return TRUE;
}

BOOL CRuVolumeTexture_Proxy::SerializeFrom(IRuStream *stream)
{
	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuVolumeTexture::SerializeFrom(stream);

	switch(version)
	{
		case 1:
			{
				// Read texture parameters
				stream->Read(&m_textureWidth, sizeof(UINT32));
				stream->Read(&m_textureHeight, sizeof(UINT32));
				stream->Read(&m_textureDepth, sizeof(UINT32));
				stream->Read(&m_levelCount, sizeof(DWORD));
				stream->Read(&m_surfaceFormat, sizeof(RuSurfaceFormat));
				stream->Read(&m_lod, sizeof(UINT32));

				// Calculate space required for texture
				INT32 width = m_textureWidth;
				INT32 height = m_textureHeight;
				INT32 depth = m_textureDepth;
				INT32 levelCount = m_levelCount == 0 ? 256 : m_levelCount;

				INT32 bitsPerTexel = RuSurfaceFormat_GetBitsPerTexel(m_surfaceFormat);
				INT32 bytesForImage = 0;

				while(width > 0 && height > 0 && depth > 0 && levelCount > 0)
				{
					// Accumulate space required for image
					bytesForImage += (width * height * depth * bitsPerTexel / 8);

					// Move on to next level
					width = width >> 1;
					height = height >> 1;
					depth = height >> 1;
					--levelCount;
				}

				// Allocate space for texture
				m_textureData = ruNEW BYTE [bytesForImage];

				// Read each surface level
				for(UINT32 i = 0; i < m_levelCount; ++i)
				{
					RuTextureUtility_SerializeFrom(this, i, stream);
				}
			}

			break;
	}

	return TRUE;
}

BOOL CRuVolumeTexture_Proxy::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuVolumeTexture::SerializeTo(stream);

	// Write texture parameters
	stream->Write(&m_textureWidth, sizeof(UINT32));
	stream->Write(&m_textureHeight, sizeof(UINT32));
	stream->Write(&m_textureDepth, sizeof(UINT32));
	stream->Write(&m_levelCount, sizeof(DWORD));
	stream->Write(&m_surfaceFormat, sizeof(RuSurfaceFormat));
	stream->Write(&m_lod, sizeof(UINT32));

	// Write each surface level
	for(UINT32 i = 0; i < m_levelCount; ++i)
	{
		RuTextureUtility_SerializeTo(this, i, stream);
	}

	return TRUE;
}

void CRuVolumeTexture_Proxy::Invalidate()
{
	if(m_proxyTexture)
	{
		m_proxyTexture->Invalidate();
	}
	else
	{
	}
}

BOOL CRuVolumeTexture_Proxy::IsInvalidated()
{
	if(m_proxyTexture)
	{
		return m_proxyTexture->IsInvalidated();
	}
	else
	{
		return FALSE;
	}
}

BOOL CRuVolumeTexture_Proxy::Restore()
{
	if(m_proxyTexture)
	{
		return m_proxyTexture->Restore();
	}
	else
	{
		return TRUE;
	}
}

const char *CRuVolumeTexture_Proxy::GetTextureName()
{
	return GetObjectName();
}

void CRuVolumeTexture_Proxy::SetTextureName(const char *textureName)
{
	SetObjectName(textureName);
}

void CRuVolumeTexture_Proxy::Select(UINT32 samplerIndex)
{
	if(m_proxyTexture)
	{
		m_proxyTexture->Select(samplerIndex);
	}
}

BOOL CRuVolumeTexture_Proxy::GenerateMipLevels()
{
	return FALSE;
}

BOOL CRuVolumeTexture_Proxy::IsDynamic()
{
	return FALSE;
}

BOOL CRuVolumeTexture_Proxy::IsRenderTarget()
{
	return FALSE;
}

UINT32 CRuVolumeTexture_Proxy::GetLevelCount()
{
	if(m_proxyTexture)
	{
		return m_proxyTexture->GetLevelCount();
	}
	else
	{
		return m_levelCount;
	}
}

UINT32 CRuVolumeTexture_Proxy::GetLOD()
{
	if(m_proxyTexture)
	{
		return m_proxyTexture->GetLOD();
	}
	else
	{
		return m_lod;
	}
}

UINT32 CRuVolumeTexture_Proxy::SetLOD(UINT32 lod)
{
	if(m_proxyTexture)
	{
		return m_proxyTexture->SetLOD(lod);
	}
	else
	{
		return m_lod = lod;
	}
}

RuResourcePool CRuVolumeTexture_Proxy::GetPool()
{
	if(m_proxyTexture)
	{
		return m_proxyTexture->GetPool();
	}
	else
	{
		return ruRESPOOL_SYSTEMMEMORY;
	}
}

RuSurfaceFormat CRuVolumeTexture_Proxy::GetSurfaceFormat()
{
	if(m_proxyTexture)
	{
		return m_proxyTexture->GetSurfaceFormat();
	}
	else
	{
		return m_surfaceFormat;
	}
}

UINT32 CRuVolumeTexture_Proxy::GetLevelWidth(UINT32 level)
{
	if(m_proxyTexture)
	{
		return m_proxyTexture->GetLevelWidth(level);
	}
	else
	{
		return m_textureWidth >> level;
	}
}

UINT32 CRuVolumeTexture_Proxy::GetLevelHeight(UINT32 level)
{
	if(m_proxyTexture)
	{
		return m_proxyTexture->GetLevelHeight(level);
	}
	else
	{
		return m_textureHeight >> level;
	}
}

UINT32 CRuVolumeTexture_Proxy::GetLevelDepth(UINT32 level)
{
	if(m_proxyTexture)
	{
		return m_proxyTexture->GetLevelDepth(level);
	}
	else
	{
		return m_textureDepth >> level;
	}
}

BOOL CRuVolumeTexture_Proxy::Lock(UINT32 level, void **bitsOut, INT32 *rowPitchOut, INT32 *slicePitchOut)
{
	if(m_proxyTexture)
	{
		return m_proxyTexture->Lock(level, bitsOut, rowPitchOut, slicePitchOut);
	}
	else
	{
		INT32 width = m_textureWidth;
		INT32 height = m_textureHeight;
		INT32 depth = m_textureDepth;
		INT32 curLevel = 0;

		INT32 bitsPerTexel = RuSurfaceFormat_GetBitsPerTexel(m_surfaceFormat);
		INT32 offset = 0;

		while(width > 0 && height > 0 && depth > 0 && curLevel != level)
		{
			// Accumulate offset required for mip level
			offset += (width * height * depth * bitsPerTexel / 8);

			// Move on to next level
			width = width >> 1;
			height = height >> 1;
			depth = depth >> 1;
			++curLevel;
		}

		if(width > 0 && height > 0 && depth > 0)
		{
			*bitsOut = &m_textureData[offset];

			switch(m_surfaceFormat)
			{
				default:
					*rowPitchOut = width * bitsPerTexel / 8;
					*slicePitchOut = width * height * bitsPerTexel / 8;
					break;

				case ruSURFACEFMT_DXT1:
				case ruSURFACEFMT_DXT2:
				case ruSURFACEFMT_DXT3:
				case ruSURFACEFMT_DXT4:
				case ruSURFACEFMT_DXT5:
					*rowPitchOut = (width >> 2) * (RuSurfaceFormat_GetBitsPerTexel(m_surfaceFormat) << 1);
					*slicePitchOut = (height >> 2) * (*rowPitchOut);
					break;
			}

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuVolumeTexture_Proxy::Unlock(UINT32 level)
{
	if(m_proxyTexture)
	{
		return m_proxyTexture->Unlock(level);
	}
	else
	{
		++m_iteration;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
