#include "../../Direct3D/Engine/RuEngineD3D.h"
#include "../../Engine/Texture/RuTexture_Proxy.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuVertexDeclarationD3D, IRuVertexDeclaration)
ruHEAP_DEFINE_SUBCLASS(CRuVertexDeclarationD3D, "CRuVertexDeclarationD3D", "IRuVertexDeclaration")
ruCLASSGUID_DEFINE(CRuVertexDeclarationD3D, 0x7F1172C4, 0x06A043b8, 0x8C0EFCE5, 0x31F150F0)

ruRTTI_DEFINE_SUBCLASS(CRuVertexBufferD3D, IRuVertexBuffer)
ruHEAP_DEFINE_SUBCLASS(CRuVertexBufferD3D, "CRuVertexBufferD3D", "IRuVertexBuffer")
ruCLASSGUID_DEFINE(CRuVertexBufferD3D, 0xC5971A69, 0x12AA4c66, 0xBE304EA4, 0x7D0D3C60)

ruRTTI_DEFINE_SUBCLASS(CRuIndexBufferD3D, IRuIndexBuffer)
ruHEAP_DEFINE_SUBCLASS(CRuIndexBufferD3D, "CRuIndexBufferD3D", "IRuIndexBuffer")
ruCLASSGUID_DEFINE(CRuIndexBufferD3D, 0x59C671F0, 0x31194b38, 0xB582F7B1, 0xCCC18DAA)

ruRTTI_DEFINE_SUBCLASS(CRuViewportD3D, IRuViewport)
ruHEAP_DEFINE_SUBCLASS(CRuViewportD3D, "CRuViewportD3D", "IRuViewport")
ruCLASSGUID_DEFINE(CRuViewportD3D, 0x34DE9371, 0x7D41452f, 0x9AB53449, 0x1A6D0817)

ruRTTI_DEFINE_SUBCLASS(CRuSurfaceD3D, IRuSurface)
ruHEAP_DEFINE_SUBCLASS(CRuSurfaceD3D, "CRuSurfaceD3D", "IRuSurface")
ruCLASSGUID_DEFINE(CRuSurfaceD3D, 0xFD8C5893, 0xF4384506, 0xA0BF500C, 0xFB76B226)

ruRTTI_DEFINE_SUBCLASS(CRuTextureD3D, IRuTexture)
ruHEAP_DEFINE_SUBCLASS(CRuTextureD3D, "CRuTextureD3D", "IRuTexture")
ruCLASSGUID_DEFINE(CRuTextureD3D, 0x3DCCB679, 0xD5A34a14, 0x9B3B2221, 0x3D5E0D77)

ruRTTI_DEFINE_SUBCLASS(CRuVolumeTextureD3D, IRuVolumeTexture)
ruHEAP_DEFINE_SUBCLASS(CRuVolumeTextureD3D, "CRuVolumeTextureD3D", "IRuVolumeTexture")
ruCLASSGUID_DEFINE(CRuVolumeTextureD3D, 0xBAE288B6, 0xF7FD4e93, 0x95A209DE, 0xD123CBE8)

ruRTTI_DEFINE_SUBCLASS(CRuTextureViewD3D, IRuTextureView)
ruHEAP_DEFINE_SUBCLASS(CRuTextureViewD3D, "CRuTextureViewD3D", "IRuTextureView")
ruCLASSGUID_DEFINE(CRuTextureViewD3D, 0x43D4A401, 0xAFBE4d32, 0x9018EF00, 0xB4BE38A2)

ruRTTI_DEFINE_SUBCLASS(CRuSwapChainD3D, IRuSwapChain)
ruHEAP_DEFINE_SUBCLASS(CRuSwapChainD3D, "CRuSwapChainD3D", "IRuSwapChain")
ruCLASSGUID_DEFINE(CRuSwapChainD3D, 0xEAED0EFB, 0xFA524c9c, 0xBD03FFFE, 0x90B9D875)

ruRTTI_DEFINE_SUBCLASS(CRuGPUQueryD3D, IRuGPUQuery)
ruHEAP_DEFINE_SUBCLASS(CRuGPUQueryD3D, "CRuGPUQueryD3D", "IRuGPUQuery")
ruCLASSGUID_DEFINE(CRuGPUQueryD3D, 0xBC6798C2, 0x75D44a6c, 0xBAB963C2, 0x86473241)

ruRTTI_DEFINE_SUBCLASS(CRuEngineD3D, IRuEngine)
ruHEAP_DEFINE_SUBCLASS(CRuEngineD3D, "CRuEngineD3D", "IRuEngine")
ruCLASSGUID_DEFINE(CRuEngineD3D, 0xBF583062, 0x346C4504, 0x8778DFFC, 0x79C8102F)

// ************************************************************************************************************************************************************

CRuVertexDeclarationD3D::CRuVertexDeclarationD3D(IDirect3DVertexDeclaration9 *vertexDeclaration)
:	m_vertexDeclaration(vertexDeclaration),
	m_numStreams(0)
{
	if(m_vertexDeclaration)
	{
		// Increment reference count on the declaration interface
		m_vertexDeclaration->AddRef();
	}

	// Clear stream layout array
	memset(m_streamLayouts, 0, sizeof(IRuVertexStreamLayout *) * 16);
}

CRuVertexDeclarationD3D::~CRuVertexDeclarationD3D()
{
	// Release Direct3D9-specific vertex declaration
	ruSAFE_RELEASE(m_vertexDeclaration);

	// Release vertex stream layouts
	for(INT32 i = 0; i < 16; ++i)
	{
		ruSAFE_RELEASE(m_streamLayouts[i]);
	}
}

void CRuVertexDeclarationD3D::Invalidate()
{
}

BOOL CRuVertexDeclarationD3D::IsInvalidated()
{
	return FALSE;
}

BOOL CRuVertexDeclarationD3D::Restore()
{
	return TRUE;
}

void CRuVertexDeclarationD3D::Select() const
{
	if(m_vertexDeclaration)
	{
		// Set the vertex declaration through the engine
		static_cast<CRuEngineD3D *>(m_engine)->SetVertexDeclaration(m_vertexDeclaration);
	}
}

INT32 CRuVertexDeclarationD3D::GetNumStreams() const
{
	return m_numStreams;
}

INT32 CRuVertexDeclarationD3D::GetStreamStride(INT32 streamIndex) const
{
	return m_streamLayouts[streamIndex]->GetStreamStride();
}

BOOL CRuVertexDeclarationD3D::BindVertexStreamLayout(INT32 streamIndex, IRuVertexStreamLayout *vertexStreamLayout)
{
	// Increment ref count on vertex stream layout
	if(vertexStreamLayout)
	{
		vertexStreamLayout->AddRef();
	}

	// Release old vertex stream layout object and store pointer to new one
	ruSAFE_RELEASE(m_streamLayouts[streamIndex]);
	m_streamLayouts[streamIndex] = vertexStreamLayout;

	// Re-count number of streams
	for(INT32 i = 0; i < 16; ++i)
	{
		if(m_streamLayouts[i])
		{
			m_numStreams = i + 1;
		}
	}

	return TRUE;
}

IRuVertexStreamLayout *CRuVertexDeclarationD3D::GetVertexStreamLayout(INT32 streamIndex)
{
	return m_streamLayouts[streamIndex];
}

BOOL CRuVertexDeclarationD3D::IsEqualTo(IRuVertexDeclaration *vertexDecl)
{
	if(vertexDecl->GetType() == CRuVertexDeclarationD3D::Type())
	{
		for(INT32 i = 0; i < 16; ++i)
		{
			if(m_streamLayouts[i] && static_cast<CRuVertexDeclarationD3D *>(vertexDecl)->m_streamLayouts[i])
			{
				if(m_streamLayouts[i]->IsEqualTo(static_cast<CRuVertexDeclarationD3D *>(vertexDecl)->m_streamLayouts[i]) == FALSE)
				{
					return FALSE;
				}
			}
			else if(m_streamLayouts[i] || static_cast<CRuVertexDeclarationD3D *>(vertexDecl)->m_streamLayouts[i])
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

CRuVertexBufferD3D::CRuVertexBufferD3D(IDirect3DVertexBuffer9 *vertexBuffer, BOOL isDynamic)
: 	m_parentDevice(NULL),
	m_vertexBuffer(vertexBuffer),
	m_isDynamic(isDynamic),
	m_size(0)
{
	if(m_vertexBuffer)
	{
		D3DVERTEXBUFFER_DESC desc;
		m_vertexBuffer->GetDesc(&desc);
		m_size = desc.Size;

		m_vertexBuffer->AddRef();

		// Store a handle to the parent device
		m_vertexBuffer->GetDevice(&m_parentDevice);
		m_parentDevice->Release();
	}
}

CRuVertexBufferD3D::~CRuVertexBufferD3D()
{
	ruSAFE_RELEASE(m_vertexBuffer);
}

void *CRuVertexBufferD3D::Lock(UINT32 offset, UINT32 size, DWORD lockFlags)
{
	if(m_vertexBuffer != NULL)
	{
		// Setup D3D lock flags, but ignore the DISCARD flag if the vertex buffer is sharing the D3D VB with other objects!
		DWORD d3dLockFlags = D3DLOCK_NOSYSLOCK;

		if(lockFlags & ruLOCK_READONLY)
			d3dLockFlags |= D3DLOCK_READONLY;
		if((lockFlags & ruLOCK_DISCARD) || m_isDynamic)
			d3dLockFlags |= D3DLOCK_DISCARD;
		if(lockFlags & ruLOCK_NOOVERWRITE)
			d3dLockFlags |= D3DLOCK_NOOVERWRITE;

		void *data;

		// If lock size is zero, set it to equal the vertex buffer size
		if(size == 0)
			size = m_size;

		if(FAILED(m_vertexBuffer->Lock(offset, size, (void **) &data, d3dLockFlags)))
			return NULL;

		return data;
	}

	return NULL;
}

void CRuVertexBufferD3D::Unlock()
{
	if(m_vertexBuffer != NULL)
		m_vertexBuffer->Unlock();

	// Increment iteration
	++m_iteration;
}

// ************************************************************************************************************************************************************

CRuIndexBufferD3D::CRuIndexBufferD3D(IDirect3DIndexBuffer9 *indexBuffer, BOOL isDynamic)
:	m_parentDevice(NULL),
	m_indexBuffer(indexBuffer),
	m_isDynamic(isDynamic),
	m_size(0)
{
	if(m_indexBuffer)
	{
		D3DINDEXBUFFER_DESC desc;
		m_indexBuffer->GetDesc(&desc);
		m_size = desc.Size;
		m_indexBuffer->AddRef();

		// Store a handle to the parent device
		m_indexBuffer->GetDevice(&m_parentDevice);
		m_parentDevice->Release();
	}
}

CRuIndexBufferD3D::~CRuIndexBufferD3D()
{
	ruSAFE_RELEASE(m_indexBuffer);
}

void *CRuIndexBufferD3D::Lock(UINT32 offset, UINT32 size, DWORD lockFlags)
{
	if(m_indexBuffer != NULL)
	{
		// Setup D3D lock flags, but ignore the DISCARD flag if the vertex buffer is sharing the D3D VB with other objects!
		DWORD d3dLockFlags = D3DLOCK_NOSYSLOCK;

		if(lockFlags & ruLOCK_READONLY)
			d3dLockFlags |= D3DLOCK_READONLY;
		if((lockFlags & ruLOCK_DISCARD) || m_isDynamic)
			d3dLockFlags |= D3DLOCK_DISCARD;
		if(lockFlags & ruLOCK_NOOVERWRITE)
			d3dLockFlags |= D3DLOCK_NOOVERWRITE;

		void *data;

		// If lock size is zero, set it to equal the vertex buffer size
		if(size == 0)
			size = m_size;

		if(FAILED(m_indexBuffer->Lock(offset, size, (void **) &data, d3dLockFlags)))
			return NULL;

		return data;
	}

	return NULL;
}

void CRuIndexBufferD3D::Unlock()
{
	if(m_indexBuffer != NULL)
		m_indexBuffer->Unlock();

	// Increment iteration
	++m_iteration;
}

// ************************************************************************************************************************************************************

CRuViewportD3D::CRuViewportD3D()
{
	m_viewport.X = 0;
	m_viewport.Y = 0;
	m_viewport.Width = 0;
	m_viewport.Height = 0;
	m_viewport.MinZ = 0.0f;
	m_viewport.MaxZ = 1.0f;

	m_clearFlags = 0;
	m_clearColor = 0x00000000;
	m_clearZ = 1.0f;
	m_clearStencil = 0;
}

CRuViewportD3D::~CRuViewportD3D()
{
}

void CRuViewportD3D::Invalidate()
{
}

BOOL CRuViewportD3D::IsInvalidated()
{
	return FALSE;
}

BOOL CRuViewportD3D::Restore()
{
	return TRUE;
}

INT32 CRuViewportD3D::GetViewportWidth()
{
	return m_viewport.Width;
}

INT32 CRuViewportD3D::GetViewportHeight()
{
	return m_viewport.Height;
}

void CRuViewportD3D::SetViewportParameters(INT32 x, INT32 y, INT32 width, INT32 height, REAL minZ, REAL maxZ)
{
	m_viewport.X = x;
	m_viewport.Y = y;
	m_viewport.Width = width;
	m_viewport.Height = height;
	m_viewport.MinZ = minZ;
	m_viewport.MaxZ = maxZ;
}

DWORD CRuViewportD3D::GetClearFlags()
{
	return m_clearFlags;
}

void CRuViewportD3D::SetClearFlags(DWORD clearFlags)
{
	m_clearFlags = clearFlags;
}

RUCOLOR CRuViewportD3D::GetClearColor()
{
	return m_clearColor;
}

void CRuViewportD3D::SetClearColor(RUCOLOR color)
{
	m_clearColor = color;
}

REAL CRuViewportD3D::GetClearZ()
{
	return m_clearZ;
}

void CRuViewportD3D::SetClearZ(REAL z)
{
	m_clearZ = z;
}

DWORD CRuViewportD3D::GetClearStencil()
{
	return m_clearStencil;
}

void CRuViewportD3D::SetClearStencil(DWORD stencil)
{
	m_clearStencil = stencil;
}

BOOL CRuViewportD3D::SelectAsViewport()
{
	if(m_engine)
	{
		if(static_cast<CRuEngineD3D *>(m_engine)->SetViewport(&m_viewport) == D3D_OK)
		{
			return TRUE;
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

CRuSurfaceD3D::CRuSurfaceD3D(IDirect3DSurface9 *surface)
:	m_parentDevice(NULL),
	m_surface(NULL),
	m_resourcePool(ruRESPOOL_DEFAULT),
	m_surfaceFormat(ruSURFACEFMT_A8R8G8B8)
{
	if(surface)
	{
		m_surface = surface;

		// Get the surface description
		m_surface->GetDesc(&m_surfaceDesc);

		m_resourcePool = ToRuResourcePool(m_surfaceDesc.Pool);
		m_surfaceFormat = ToRuSurfaceFormat(m_surfaceDesc.Format);

		// Store a handle to the parent device
		m_surface->GetDevice(&m_parentDevice);
		m_parentDevice->Release();
	}
}

CRuSurfaceD3D::~CRuSurfaceD3D()
{
	if(m_surface)
	{
		m_surface->Release();
	}
}

void CRuSurfaceD3D::Invalidate()
{
	if(m_surface)
	{
		m_surface->Release();
		m_surface = NULL;
	}
}

BOOL CRuSurfaceD3D::IsInvalidated()
{
	return (m_surface == NULL);
}

BOOL CRuSurfaceD3D::Restore()
{
	if(m_parentDevice && m_surface == NULL && m_resourcePool == ruRESPOOL_DEFAULT)
	{
		if(m_surfaceDesc.Usage == D3DUSAGE_DEPTHSTENCIL)
		{
			// Create depth stencil surface
			HRESULT hr = m_parentDevice->CreateDepthStencilSurface(m_surfaceDesc.Width, m_surfaceDesc.Height, m_surfaceDesc.Format, m_surfaceDesc.MultiSampleType, m_surfaceDesc.MultiSampleQuality, TRUE, &m_surface, NULL);
			if(FAILED(hr))
				return FALSE;

			return TRUE;
		}
	}

	return FALSE;
}

RuResourcePool CRuSurfaceD3D::GetPool()
{
	return m_resourcePool;
}

RuSurfaceFormat CRuSurfaceD3D::GetSurfaceFormat()
{
	return m_surfaceFormat;
}

UINT32 CRuSurfaceD3D::GetWidth() const
{
	return m_surfaceDesc.Width;
}

UINT32 CRuSurfaceD3D::GetHeight() const
{
	return m_surfaceDesc.Height;
}

BOOL CRuSurfaceD3D::Lock(void **bitsOut, INT32 *pitchOut)
{
	if(m_surface)
	{
		D3DLOCKED_RECT lockedRect;
		if(m_surface->LockRect(&lockedRect, NULL, 0) == D3D_OK)
		{
			*bitsOut = lockedRect.pBits;
			*pitchOut = lockedRect.Pitch;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuSurfaceD3D::Unlock()
{
	if(m_surface)
	{
		if(m_surface->UnlockRect() == D3D_OK)
		{
			// Increment iteration
			++m_iteration;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuSurfaceD3D::SelectAsRenderTarget(INT32 rtIndex)
{
	if(m_engine)
	{
		if(static_cast<CRuEngineD3D *>(m_engine)->SetRenderTarget(rtIndex, m_surface) == D3D_OK)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuSurfaceD3D::SelectDepthStencil()
{
	if(m_engine)
	{
		if(static_cast<CRuEngineD3D *>(m_engine)->SetDepthStencil(m_surface) == D3D_OK)
		{
			return TRUE;
		}
	}

	return FALSE;
}

IDirect3DSurface9 *CRuSurfaceD3D::GetIDirect3DSurface()
{
	return m_surface;
}

// ************************************************************************************************************************************************************

CRuTextureD3D::CRuTextureD3D(IDirect3DTexture9 *texture, BOOL isDynamic, BOOL isMatchedToBackBuffer)
:	m_parentDevice(NULL),
	m_texture(NULL),
	m_levelCount(0),
	m_surfaces(NULL),
	m_surfaceDesc(NULL),
	m_textureName(NULL),
	m_isDynamic(FALSE),
	m_isRenderTarget(FALSE),
	m_isMatchedToBackBuffer(FALSE),
	m_resourcePool(ruRESPOOL_DEFAULT),
	m_surfaceFormat(ruSURFACEFMT_A8R8G8B8)
{
	if(texture)
	{
		m_texture = texture;
		m_isDynamic = isDynamic;
		m_isMatchedToBackBuffer = isMatchedToBackBuffer;

		// Get the surface description
		m_levelCount = m_texture->GetLevelCount();
		m_texture->GetLevelDesc(0, &m_topSurfaceDesc);

		if(m_topSurfaceDesc.Usage & D3DUSAGE_RENDERTARGET)
			m_isRenderTarget = TRUE;

		m_resourcePool = ToRuResourcePool(m_topSurfaceDesc.Pool);
		m_surfaceFormat = ToRuSurfaceFormat(m_topSurfaceDesc.Format);

		// Allocate surfaces
		m_surfaces = ruNEW CRuSurfaceD3D * [m_levelCount];
		m_surfaceDesc = ruNEW D3DSURFACE_DESC [m_levelCount];
		for(UINT32 i = 0; i < m_levelCount; ++i)
		{
			IDirect3DSurface9 *d3dSurface = NULL;
			if(m_texture->GetSurfaceLevel(i, &d3dSurface) == D3D_OK)
			{
				m_surfaces[i] = ruNEW CRuSurfaceD3D(d3dSurface);
				m_texture->GetLevelDesc(i, &m_surfaceDesc[i]);
			}
		}

		// Store a handle to the parent device
		m_texture->GetDevice(&m_parentDevice);
		m_parentDevice->Release();
	}
}

CRuTextureD3D::~CRuTextureD3D()
{
	if(m_texture)
	{
		m_texture->Release();
	}

	if(m_textureName)
	{
		delete[] m_textureName;
	}

	// Release surfaces
	if(m_surfaces)
	{
		for(UINT32 i = 0; i < m_levelCount; ++i)
		{
			if(m_surfaces[i])
			{
				m_surfaces[i]->Release();
			}
		}

		delete[] m_surfaces;
	}

	if(m_surfaceDesc)
	{
		delete[] m_surfaceDesc;
	}
}

INT32 CRuTextureD3D::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = 0;

	for(UINT32 i = 0, width = this->GetLevelWidth(0), height = this->GetLevelHeight(0); i < m_levelCount; ++i)
	{
		footprint += RuSurfaceFormat_GetBitsPerTexel(this->GetSurfaceFormat()) * width * height / 8;

		width = width >> 1;
		height = height >> 1;
	}

	return footprint;
}

void CRuTextureD3D::Invalidate()
{
	if(m_texture && m_resourcePool == ruRESPOOL_DEFAULT)
	{
		// Release surfaces
		if(m_surfaces)
		{
			for(UINT32 i = 0; i < m_levelCount; ++i)
			{
				if(m_surfaces[i])
				{
					m_surfaces[i]->Release();
				}
			}

			delete[] m_surfaces;
			m_surfaces = NULL;
		}

		if(m_surfaceDesc)
		{
			delete[] m_surfaceDesc;
			m_surfaceDesc = NULL;
		}

		// Release texture
		m_texture->Release();
		m_texture = NULL;
	}
}

BOOL CRuTextureD3D::IsInvalidated()
{
	return m_texture == NULL;
}

BOOL CRuTextureD3D::Restore()
{
	if(m_parentDevice && m_texture == NULL && m_resourcePool == ruRESPOOL_DEFAULT)
	{
		// If this texture is always matched to the backbuffer, adjust its dimensions
		if(m_isMatchedToBackBuffer)
		{
			IDirect3DSurface9 *backBuffer;
			if(m_parentDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer) == D3D_OK)
			{
				D3DSURFACE_DESC bbDesc;
				backBuffer->GetDesc(&bbDesc);

				m_topSurfaceDesc.Width = bbDesc.Width;
				m_topSurfaceDesc.Height = bbDesc.Height;

				backBuffer->Release();
			}
		}

		if(m_parentDevice->CreateTexture(m_topSurfaceDesc.Width, m_topSurfaceDesc.Height, m_levelCount, m_isRenderTarget ? D3DUSAGE_RENDERTARGET : 0, m_topSurfaceDesc.Format, m_topSurfaceDesc.Pool, &m_texture, NULL) == D3D_OK)
		{
			// Allocate surfaces
			m_surfaces = ruNEW CRuSurfaceD3D * [m_levelCount];
			m_surfaceDesc = ruNEW D3DSURFACE_DESC [m_levelCount];
			for(UINT32 i = 0; i < m_levelCount; ++i)
			{
				IDirect3DSurface9 *d3dSurface = NULL;
				if(m_texture->GetSurfaceLevel(i, &d3dSurface) == D3D_OK)
				{
					m_surfaces[i] = ruNEW CRuSurfaceD3D(d3dSurface);
					m_texture->GetLevelDesc(i, &m_surfaceDesc[i]);
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}

const char *CRuTextureD3D::GetTextureName()
{
	if(m_textureName)
	{
		return m_textureName;
	}

	return "";
}

void CRuTextureD3D::SetTextureName(const char *textureName)
{
	if(m_textureName)
	{
		delete[] m_textureName;
	}

	UINT32 textureNameLength = (UINT32)strlen(textureName) + 1;
	m_textureName = ruNEW char [textureNameLength];
	strcpy(m_textureName, textureName);
}

void CRuTextureD3D::Select(UINT32 samplerIndex)
{
	// Set the texture through the engine
	static_cast<CRuEngineD3D *>(m_engine)->SetTexture(samplerIndex, m_texture);
}

BOOL CRuTextureD3D::GenerateMipLevels()
{
	if(m_texture)
	{
		D3DXFilterTexture(m_texture, NULL, 0, D3DX_DEFAULT);
		return TRUE;
	}

	return FALSE;
}

BOOL CRuTextureD3D::IsDynamic()
{
	return m_isDynamic;
}

BOOL CRuTextureD3D::IsRenderTarget()
{
	return m_isRenderTarget;
}

UINT32 CRuTextureD3D::GetLevelCount()
{
	if(m_texture)
	{
		return m_texture->GetLevelCount();
	}

	return 0;
}

UINT32 CRuTextureD3D::GetLOD()
{
	if(m_texture)
	{
		return m_texture->GetLOD();
	}

	return 0;
}

UINT32 CRuTextureD3D::SetLOD(UINT32 lod)
{
	if(m_texture)
	{
		HRESULT result = m_texture->SetLOD(lod);
		return SUCCEEDED(result);
	}

	return 0;
}

RuResourcePool CRuTextureD3D::GetPool()
{
	return m_resourcePool;
}

RuSurfaceFormat CRuTextureD3D::GetSurfaceFormat()
{
	return m_surfaceFormat;
}

BOOL CRuTextureD3D::SelectAsRenderTarget(INT32 rtIndex, UINT32 level)
{
	if(m_texture)
	{
		IDirect3DSurface9 *rtSurface = NULL;

		if(m_texture->GetSurfaceLevel(level, &rtSurface) == D3D_OK)
		{
			if(m_engine)
			{
				if(static_cast<CRuEngineD3D *>(m_engine)->SetRenderTarget(rtIndex, rtSurface) == D3D_OK)
				{
					rtSurface->Release();
					return TRUE;
				}
			}

			rtSurface->Release();
		}
	}

	return FALSE;
}

UINT32 CRuTextureD3D::GetLevelWidth(UINT32 level)
{
/*
	if(m_texture)
	{
		D3DSURFACE_DESC surfDesc;
		m_texture->GetLevelDesc(level, &surfDesc);
		return surfDesc.Width;
	}

	return 0;
*/
	return m_surfaceDesc[level].Width;
}

UINT32 CRuTextureD3D::GetLevelHeight(UINT32 level)
{
/*
	if(m_texture)
	{
		D3DSURFACE_DESC surfDesc;
		m_texture->GetLevelDesc(level, &surfDesc);
		return surfDesc.Height;
	}

	return 0;
*/
	return m_surfaceDesc[level].Height;
}

IRuSurface *CRuTextureD3D::GetLevelSurface(UINT32 level)
{
	if(m_texture && level < m_levelCount)
	{
		return m_surfaces[level];
	}

	return NULL;
}

BOOL CRuTextureD3D::Lock(UINT32 level, void **bitsOut, INT32 *pitchOut)
{
	if(m_texture)
	{
		D3DLOCKED_RECT lockedRect;
		HRESULT lockResult = m_texture->LockRect(level, &lockedRect, NULL, 0);
		if(lockResult == D3D_OK)
		{
			*bitsOut = lockedRect.pBits;
			*pitchOut = lockedRect.Pitch;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuTextureD3D::Unlock(UINT32 level)
{
	if(m_texture)
	{
		if(m_texture->UnlockRect(level) == D3D_OK)
		{
			// Increment iteration
			++m_iteration;

			return TRUE;
		}
	}

	return FALSE;
}

IDirect3DTexture9 *CRuTextureD3D::GetIDirect3DTexture()
{
	return m_texture;
}

// ************************************************************************************************************************************************************

CRuVolumeTextureD3D::CRuVolumeTextureD3D(IDirect3DVolumeTexture9 *texture, BOOL isDynamic, BOOL isMatchedToBackBuffer)
:	m_parentDevice(NULL),
	m_texture(NULL),
	m_levelCount(0),
	m_textureName(NULL),
	m_isDynamic(FALSE),
	m_resourcePool(ruRESPOOL_DEFAULT),
	m_surfaceFormat(ruSURFACEFMT_A8R8G8B8)
{
	if(texture)
	{
		m_texture = texture;
		m_isDynamic = isDynamic;

		// Get the surface description
		m_levelCount = m_texture->GetLevelCount();
		m_texture->GetLevelDesc(0, &m_topSurfaceDesc);

		m_resourcePool = ToRuResourcePool(m_topSurfaceDesc.Pool);
		m_surfaceFormat = ToRuSurfaceFormat(m_topSurfaceDesc.Format);
/*
		// Allocate surfaces
		m_surfaces = ruNEW CRuSurfaceD3D * [m_levelCount];
		for(UINT32 i = 0; i < m_levelCount; ++i)
		{
			IDirect3DSurface9 *d3dSurface = NULL;
			if(m_texture->GetSurfaceLevel(0, &d3dSurface) == D3D_OK)
			{
				m_surfaces[i] = ruNEW CRuSurfaceD3D(d3dSurface);
			}
		}
*/
		// Store a handle to the parent device
		m_texture->GetDevice(&m_parentDevice);
		m_parentDevice->Release();
	}
}

CRuVolumeTextureD3D::~CRuVolumeTextureD3D()
{
	if(m_texture)
	{
		m_texture->Release();
	}

	if(m_textureName)
	{
		delete[] m_textureName;
	}
/*
	// Release surfaces
	if(m_surfaces)
	{
		for(UINT32 i = 0; i < m_levelCount; ++i)
		{
			if(m_surfaces[i])
			{
				m_surfaces[i]->Release();
			}
		}

		delete[] m_surfaces;
	}
*/
}

INT32 CRuVolumeTextureD3D::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = 0;

	for(UINT32 i = 0, width = this->GetLevelWidth(0), height = this->GetLevelHeight(0), depth = this->GetLevelDepth(0); i < m_levelCount; ++i)
	{
		footprint += RuSurfaceFormat_GetBitsPerTexel(this->GetSurfaceFormat()) * width * height / 8;

		width = width >> 1;
		height = height >> 1;
		depth = depth >> 1;
	}

	return footprint;
}

void CRuVolumeTextureD3D::Invalidate()
{
	if(m_texture && m_resourcePool == ruRESPOOL_DEFAULT)
	{
/*
		// Release surfaces
		if(m_surfaces)
		{
			for(UINT32 i = 0; i < m_levelCount; ++i)
			{
				if(m_surfaces[i])
				{
					m_surfaces[i]->Release();
				}
			}

			delete[] m_surfaces;
			m_surfaces = NULL;
		}
*/
		// Release texture
		m_texture->Release();
		m_texture = NULL;
	}
}

BOOL CRuVolumeTextureD3D::IsInvalidated()
{
	return m_texture == NULL;
}

BOOL CRuVolumeTextureD3D::Restore()
{
	if(m_parentDevice && m_texture == NULL && m_resourcePool == ruRESPOOL_DEFAULT)
	{
		if(m_parentDevice->CreateVolumeTexture(m_topSurfaceDesc.Width, m_topSurfaceDesc.Height, m_topSurfaceDesc.Depth, m_levelCount, 0, m_topSurfaceDesc.Format, m_topSurfaceDesc.Pool, &m_texture, NULL) == D3D_OK)
		{
/*
			// Allocate surfaces
			m_surfaces = ruNEW CRuSurfaceD3D * [m_levelCount];
			for(UINT32 i = 0; i < m_levelCount; ++i)
			{
				IDirect3DSurface9 *d3dSurface = NULL;
				if(m_texture->GetSurfaceLevel(0, &d3dSurface) == D3D_OK)
				{
					m_surfaces[i] = ruNEW CRuSurfaceD3D(d3dSurface);
				}
			}
*/
			return TRUE;
		}
	}

	return FALSE;
}

const char *CRuVolumeTextureD3D::GetTextureName()
{
	if(m_textureName)
	{
		return m_textureName;
	}

	return "";
}

void CRuVolumeTextureD3D::SetTextureName(const char *textureName)
{
	if(m_textureName)
	{
		delete[] m_textureName;
	}

	UINT32 textureNameLength = (UINT32)strlen(textureName) + 1;
	m_textureName = ruNEW char [textureNameLength];
	strcpy(m_textureName, textureName);
}

void CRuVolumeTextureD3D::Select(UINT32 samplerIndex)
{
	// Set the texture through the engine
	static_cast<CRuEngineD3D *>(m_engine)->SetTexture(samplerIndex, m_texture);
}

BOOL CRuVolumeTextureD3D::GenerateMipLevels()
{
	if(m_texture)
	{
		D3DXFilterTexture(m_texture, NULL, 0, D3DX_DEFAULT);
		return TRUE;
	}

	return FALSE;
}

BOOL CRuVolumeTextureD3D::IsDynamic()
{
	return m_isDynamic;
}

BOOL CRuVolumeTextureD3D::IsRenderTarget()
{
	return FALSE;
}

UINT32 CRuVolumeTextureD3D::GetLevelCount()
{
	if(m_texture)
	{
		return m_texture->GetLevelCount();
	}

	return 0;
}

UINT32 CRuVolumeTextureD3D::GetLOD()
{
	if(m_texture)
	{
		return m_texture->GetLOD();
	}

	return 0;
}

UINT32 CRuVolumeTextureD3D::SetLOD(UINT32 lod)
{
	if(m_texture)
	{
		return m_texture->SetLOD(lod);
	}

	return 0;
}

RuResourcePool CRuVolumeTextureD3D::GetPool()
{
	return m_resourcePool;
}

RuSurfaceFormat CRuVolumeTextureD3D::GetSurfaceFormat()
{
	return m_surfaceFormat;
}

BOOL CRuVolumeTextureD3D::SelectAsRenderTarget(INT32 rtIndex, UINT32 level)
{
	return FALSE;
}

UINT32 CRuVolumeTextureD3D::GetLevelWidth(UINT32 level)
{
/*
	if(m_texture)
	{
		D3DVOLUME_DESC surfDesc;
		m_texture->GetLevelDesc(level, &surfDesc);
		return surfDesc.Width;
	}

	return 0;
*/
	return m_topSurfaceDesc.Width;
}

UINT32 CRuVolumeTextureD3D::GetLevelHeight(UINT32 level)
{
/*
	if(m_texture)
	{
		D3DVOLUME_DESC surfDesc;
		m_texture->GetLevelDesc(level, &surfDesc);
		return surfDesc.Height;
	}

	return 0;
*/
	return m_topSurfaceDesc.Height;
}

UINT32 CRuVolumeTextureD3D::GetLevelDepth(UINT32 level)
{
/*
	if(m_texture)
	{
		D3DVOLUME_DESC surfDesc;
		m_texture->GetLevelDesc(level, &surfDesc);
		return surfDesc.Depth;
	}

	return 0;
*/
	return m_topSurfaceDesc.Depth;
}

BOOL CRuVolumeTextureD3D::Lock(UINT32 level, void **bitsOut, INT32 *rowPitchOut, INT32 *slicePitchOut)
{
	if(m_texture)
	{
		D3DLOCKED_BOX lockedBox;
		if(m_texture->LockBox(level, &lockedBox, NULL, 0) == D3D_OK)
		{
			*bitsOut = lockedBox.pBits;
			*rowPitchOut = lockedBox.RowPitch;
			*slicePitchOut = lockedBox.SlicePitch;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuVolumeTextureD3D::Unlock(UINT32 level)
{
	if(m_texture)
	{
		if(m_texture->UnlockBox(level) == D3D_OK)
		{
			// Increment iteration
			++m_iteration;

			return TRUE;
		}
	}

	return FALSE;
}

IDirect3DVolumeTexture9 *CRuVolumeTextureD3D::GetIDirect3DVolumeTexture()
{
	return m_texture;
}

// ************************************************************************************************************************************************************

CRuTextureViewD3D::CRuTextureViewD3D()
:	m_basisTexture(NULL),
	m_x(0),
	m_y(0),
	m_width(0),
	m_height(0),
	m_viewFormat(ruSURFACEFMT_A8R8G8B8),
	m_viewport(NULL),
	m_textureViewTransformDirty(TRUE)
{
}

CRuTextureViewD3D::~CRuTextureViewD3D()
{
	ruSAFE_RELEASE(m_basisTexture);
	ruSAFE_RELEASE(m_viewport);
}

void CRuTextureViewD3D::Invalidate()
{
	ruSAFE_RELEASE(m_basisTexture);
}

BOOL CRuTextureViewD3D::IsInvalidated()
{
	if(m_basisTexture)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CRuTextureViewD3D::Restore()
{
	if(m_basisTexture)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CRuTextureViewD3D::SetBasisTexture(IRuTexture *basisTexture)
{
	if(basisTexture && basisTexture->GetType().IsSubClassOf(CRuTextureD3D::Type()))
	{
		// Store reference to new basis texture
		basisTexture->AddRef();
		ruSAFE_RELEASE(m_basisTexture);
		m_basisTexture = static_cast<CRuTextureD3D *>(basisTexture);

		// Mark the texture view transform as dirty
		m_textureViewTransformDirty = TRUE;

		return TRUE;
	}
	else
	{
		ruSAFE_RELEASE(m_basisTexture);
	}

	return FALSE;
}

BOOL CRuTextureViewD3D::SetViewFormat(RuSurfaceFormat viewFormat)
{
	// Store new view format
	m_viewFormat = viewFormat;

	return TRUE;
}

BOOL CRuTextureViewD3D::SetViewOrigin(INT32 x, INT32 y)
{
	// Store new view origin
	m_x = x;
	m_y = y;

	// Mark the texture view transform as dirty
	m_textureViewTransformDirty = TRUE;

	return TRUE;
}

BOOL CRuTextureViewD3D::SetViewSize(INT32 width, INT32 height)
{
	// Store new view size
	m_width = width;
	m_height = height;

	// Mark the texture view transform as dirty
	m_textureViewTransformDirty = TRUE;

	return TRUE;
}

INT32 CRuTextureViewD3D::GetViewWidth() const
{
	return m_width;
}

INT32 CRuTextureViewD3D::GetViewHeight() const
{
	return m_height;
}

RuSurfaceFormat CRuTextureViewD3D::GetViewFormat() const
{
	return m_viewFormat;
}

IRuTexture *CRuTextureViewD3D::GetTexture()
{
	return m_basisTexture;
}

IRuViewport *CRuTextureViewD3D::GetViewport()
{
	// Create viewport if it does not already exist
	if(m_viewport == NULL && m_engine)
	{
		m_viewport = static_cast<CRuEngineD3D *>(m_engine)->CreateViewport(0, 0, m_width, m_height);
		if(m_viewport == NULL)
		{
			return FALSE;
		}
	}

	// Set internal viewport parameters
	m_viewport->SetViewportParameters(m_x, m_y, m_width, m_height, 0.0f, 1.0f);

	return m_viewport;
}

const CRuMatrix4x4 &CRuTextureViewD3D::GetTextureViewTransform()
{
	if(m_textureViewTransformDirty)
	{
		CalculateTextureViewTransform();

		m_textureViewTransformDirty = FALSE;
	}

	return m_textureViewTransform;
}

void CRuTextureViewD3D::CalculateTextureViewTransform()
{
	// Re-calculate texture view transform
	m_textureViewTransform.SetToIdentity();

	if(m_basisTexture)
	{
		m_textureViewTransform._11 = (REAL) m_width / m_basisTexture->GetLevelWidth(0);
		m_textureViewTransform._22 = (REAL) m_height / m_basisTexture->GetLevelHeight(0);
		m_textureViewTransform._41 = (REAL) m_x / m_basisTexture->GetLevelWidth(0);
		m_textureViewTransform._42 = (REAL) m_y / m_basisTexture->GetLevelHeight(0);
	}
}

// ************************************************************************************************************************************************************

CRuSwapChainD3D::CRuSwapChainD3D(IDirect3DSwapChain9 *swapChain, BOOL isPrimarySwapChain)
:	m_parentDevice(NULL),
	m_backBuffer(NULL),
	m_isPrimarySwapChain(isPrimarySwapChain)
{
	SetD3DSwapChain(swapChain);
}

CRuSwapChainD3D::~CRuSwapChainD3D()
{
	Invalidate();
}

void CRuSwapChainD3D::Invalidate()
{
	// Release swap chain object
	if(m_swapChain)
		m_swapChain->Release();

	// Reset member variables
	m_swapChain = NULL;
	m_backBuffer = NULL;
}

BOOL CRuSwapChainD3D::IsInvalidated()
{
	// Swap chain is lost if there is no swap chain object
	return (m_swapChain == NULL);
}

BOOL CRuSwapChainD3D::Restore()
{
	if(m_parentDevice)
	{
		IDirect3DSwapChain9 *d3dSwapChain = NULL;
		HRESULT hr;

		if(m_isPrimarySwapChain)
		{
			// Restore primary swap chain
			m_parentDevice->GetSwapChain(0, &d3dSwapChain);
			this->SetD3DSwapChain(d3dSwapChain);
		}
		else
		{
			// Create secondary D3D swap chain
			hr = m_parentDevice->CreateAdditionalSwapChain(&m_presentParameters, &d3dSwapChain);
			if(FAILED(hr))
				return FALSE;

			if(!this->SetD3DSwapChain(d3dSwapChain))
				return FALSE;
		}
	}

	return FALSE;
}

BOOL CRuSwapChainD3D::SelectAsRenderTarget(INT32 rtIndex)
{
	if(m_backBuffer && m_engine)
	{
		if(static_cast<CRuEngineD3D *>(m_engine)->SetRenderTarget(rtIndex, m_backBuffer) == D3D_OK)
		{
			return TRUE;
		}
	}

	return FALSE;
}

UINT32 CRuSwapChainD3D::GetWidth() const
{
	return m_presentParameters.BackBufferWidth;
}

UINT32 CRuSwapChainD3D::GetHeight() const
{
	return m_presentParameters.BackBufferHeight;
}

BOOL CRuSwapChainD3D::Present()
{
	BOOL retVal = FALSE;

	if(m_swapChain)
	{
		// Present the back buffer
//		m_swapChain->Present(NULL, NULL, NULL, NULL, 0);
//		HRESULT result = m_swapChain->Present(NULL, NULL, NULL, NULL, D3DPRESENT_DONOTWAIT);
		HRESULT result = m_swapChain->Present(NULL, NULL, NULL, NULL, 0);

		if(result == D3D_OK)
		{
			retVal = TRUE;
		}
		else
		{
//			MessageBox(NULL, "Present error!", "Error", MB_OK);
/*
			if(result != D3DERR_WASSTILLDRAWING)
			{
				this->Invalidate();
			}
*/
			OutputDebugString("Still waiting!\n");
		}

		return retVal;
	}

	return retVal;
}

BOOL CRuSwapChainD3D::SaveToFile(const char *fileName, RuImageFileFormat imageFormat)
{
	BOOL retVal = FALSE;

	if(m_backBuffer)
	{
		HRESULT result = D3DXSaveSurfaceToFile(fileName, static_cast<D3DXIMAGE_FILEFORMAT>(imageFormat), m_backBuffer, NULL, NULL);

		if(result == D3D_OK)
		{
			retVal = TRUE;
		}
	}

	return retVal;
}

IDirect3DSurface9 *CRuSwapChainD3D::GetIDirect3DSurface()
{
	return m_backBuffer;
}

D3DPRESENT_PARAMETERS &CRuSwapChainD3D::PresentParameters()
{
	return m_presentParameters;
}

BOOL CRuSwapChainD3D::SetD3DSwapChain(IDirect3DSwapChain9 *swapChain)
{
	if(swapChain)
	{
		m_swapChain = swapChain;

		// Store a pointer to the parent device
		if(m_swapChain->GetDevice(&m_parentDevice) == D3D_OK)
		{
			// Release the parent device reference count, since we do not need to actually hold the device
			m_parentDevice->Release();
		}

		// Store a pointer to the back buffer (assume double-buffering only)
		if(m_swapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_backBuffer) == D3D_OK)
		{
			// Release the back buffer reference count, since we do not need to actually hold the buffer
			m_backBuffer->Release();
		}

		// Store a copy of the present parameters
		m_swapChain->GetPresentParameters(&m_presentParameters);

		return TRUE;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

CRuGPUQueryD3D::CRuGPUQueryD3D(IDirect3DQuery9 *query, D3DQUERYTYPE queryType)
:	m_query(query),
	m_queryType(queryType)
{
	// Store a pointer to the parent device
	if(m_query->GetDevice(&m_parentDevice) == D3D_OK)
	{
		// Release the parent device reference count, since we do not need to actually hold the device
		m_parentDevice->Release();
	}
}

CRuGPUQueryD3D::~CRuGPUQueryD3D()
{
	Invalidate();
}

void CRuGPUQueryD3D::Invalidate()
{
	ruSAFE_RELEASE(m_query);
}

BOOL CRuGPUQueryD3D::IsInvalidated()
{
	return m_query == NULL;
}

BOOL CRuGPUQueryD3D::Restore()
{
	if(m_parentDevice)
	{
		HRESULT hr = m_parentDevice->CreateQuery(m_queryType, &m_query);

		if(FAILED(hr))
		{
			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuGPUQueryD3D::Begin()
{
	m_query->Issue(D3DISSUE_BEGIN);
	return TRUE;
}

BOOL CRuGPUQueryD3D::End()
{
	m_query->Issue(D3DISSUE_END);
	return TRUE;
}

INT32 CRuGPUQueryD3D::GetData(void *dataOut, DWORD dataSize, DWORD getDataFlags)
{
	// Convert getDataFlags to D3D9-compatible flags
	DWORD d3dGetDataFlags = 0;

	if(getDataFlags & ruASYNCGETDATAFLAG_DONOTFLUSH)
	{
		// Do nothing
	}

	// Perform GetData call
	HRESULT hr = m_query->GetData(dataOut, dataSize, d3dGetDataFlags);
	if(FAILED(hr))
	{
		return -1;
	}

	return 0;
}

// ************************************************************************************************************************************************************

/*!
	CRuEngineD3D::RuDeviceInfoD3D::~RuDeviceInfoD3D()
	Standard destructor
*/
CRuEngineD3D::RuDeviceInfoD3D::~RuDeviceInfoD3D()
{
	for(INT32 i = 0; i < m_deviceComboList.Count(); i++)
	{
		delete m_deviceComboList[i];
	}

	m_deviceComboList.Clear();
}

// ************************************************************************************************************************************************************

/*!
	CRuEngineD3D::RuAdapterInfoD3D::~RuAdapterInfoD3D()
	Standard destructor
*/
CRuEngineD3D::RuAdapterInfoD3D::~RuAdapterInfoD3D()
{
	for(INT32 i = 0; i < m_deviceInfoList.Count(); i++)
	{
		delete m_deviceInfoList[i];
	}

	m_deviceInfoList.Clear();
}

// ************************************************************************************************************************************************************

/*!
	CRuEngineD3D::RuAdapterInfoD3D *CRuEngineD3D::RuEngineSettingD3D::AdapterInfo()
	Returns pointer to the currently selected mode's adapter info
	@return RuAdapterInfoD3D*
*/
CRuEngineD3D::RuAdapterInfoD3D *CRuEngineD3D::RuEngineSettingD3D::AdapterInfo()
{
	return m_isWindowed ? m_windowedAdapterInfo : m_fullscreenAdapterInfo;
}

/*!
	CRuEngineD3D::RuDeviceInfoD3D *CRuEngineD3D::RuEngineSettingD3D::DeviceInfo()
	Returns pointer to the currently selected mode's device info
	@return RuDeviceInfoD3D*
*/
CRuEngineD3D::RuDeviceInfoD3D *CRuEngineD3D::RuEngineSettingD3D::DeviceInfo()
{
	return m_isWindowed ? m_windowedDeviceInfo : m_fullscreenDeviceInfo;
}

/*!
	CRuEngineD3D::RuDeviceComboD3D *CRuEngineD3D::RuEngineSettingD3D::DeviceCombo()
	Returns pointer to the currently selected mode's device combo
	@return RuDeviceComboD3D*
*/
CRuEngineD3D::RuDeviceComboD3D *CRuEngineD3D::RuEngineSettingD3D::DeviceCombo()
{
	return m_isWindowed ? m_windowedDeviceCombo : m_fullscreenDeviceCombo;
}

/*!
	D3DDISPLAYMODE &CRuEngineD3D::RuEngineSettingD3D::DisplayMode()
	Returns reference to the currently selected mode's display mode
	@return D3DDISPLAYMODE&
*/
D3DDISPLAYMODE &CRuEngineD3D::RuEngineSettingD3D::DisplayMode()
{
	return m_isWindowed ? m_windowedDisplayMode : m_fullscreenDisplayMode;
}

/*!
	D3DFORMAT &CRuEngineD3D::RuEngineSettingD3D::DepthStencilBufferFormat()
	Returns reference to the currently selected mode's depth/stencil buffer format
	@return D3DFORMAT&
*/
D3DFORMAT &CRuEngineD3D::RuEngineSettingD3D::DepthStencilBufferFormat()
{
	return m_isWindowed ? m_windowedDepthStencilBufferFormat : m_fullscreenDepthStencilBufferFormat;
}

/*!
	D3DMULTISAMPLE_TYPE &CRuEngineD3D::RuEngineSettingD3D::MultisampleType()
	Returns reference to the currently selected mode's multisample type
	@return D3DMULTISAMPLE_TYPE&
*/
D3DMULTISAMPLE_TYPE &CRuEngineD3D::RuEngineSettingD3D::MultisampleType()
{
	return m_isWindowed ? m_windowedMultisampleType : m_fullscreenMultisampleType;
}

/*!
	DWORD &CRuEngineD3D::RuEngineSettingD3D::MultisampleQuality()
	Returns reference to the currently selected mode's multisample quality
	@return DWORD&
*/
DWORD &CRuEngineD3D::RuEngineSettingD3D::MultisampleQuality()
{
	return m_isWindowed ? m_windowedMultisampleQuality : m_fullscreenMultisampleQuality;
}

/*!
	RuEngineMode &CRuEngineD3D::RuEngineSettingD3D::EngineMode()
	Returns reference to the currently selected mode's engine mode
	@return RuEngineMode&
*/
RuEngineMode &CRuEngineD3D::RuEngineSettingD3D::EngineMode()
{
	return m_isWindowed ? m_windowedEngineMode : m_fullscreenEngineMode;
}

/*!
	UINT32 &CRuEngineD3D::RuEngineSettingD3D::PresentInterval()
	Returns reference to the currently selected mode's present interval
	@return UINT32&
*/
UINT32 &CRuEngineD3D::RuEngineSettingD3D::PresentInterval()
{
	return m_isWindowed ? m_windowedPresentInterval : m_fullscreenPresentInterval;
}

/*!
	INT32 CRuEngineD3D::RuEngineSettingD3D::GetAdapterOrdinal()
	Returns the currently selected mode's adapter ordinal
	@return INT32
*/
INT32 CRuEngineD3D::RuEngineSettingD3D::GetAdapterOrdinal()
{
	return m_isWindowed ? m_windowedDeviceCombo->m_adapterOrdinal : m_fullscreenDeviceCombo->m_adapterOrdinal;
}

/*!
	D3DDEVTYPE CRuEngineD3D::RuEngineSettingD3D::GetDeviceType()
	Returns the currently selected mode's device type
	@return D3DDEVTYPE
*/
D3DDEVTYPE CRuEngineD3D::RuEngineSettingD3D::GetDeviceType()
{
	return m_isWindowed ? m_windowedDeviceCombo->m_deviceType : m_fullscreenDeviceCombo->m_deviceType;
}

/*!
	D3DFORMAT CRuEngineD3D::RuEngineSettingD3D::GetBackBufferFormat()
	Returns the currently selected mode's back buffer format
	@return D3DFORMAT
*/
D3DFORMAT CRuEngineD3D::RuEngineSettingD3D::GetBackBufferFormat()
{
	return m_isWindowed ? m_windowedDeviceCombo->m_backBufferFormat : m_fullscreenDeviceCombo->m_backBufferFormat;
}

BOOL CRuEngineD3D::RuEngineSettingD3D::FindBestWindowedMode(IDirect3D9 *d3dObject, CRuArrayList<RuAdapterInfoD3D *> &adapterInfoList, BOOL requireHAL, BOOL requireREF)
{
	D3DDISPLAYMODE primaryDesktopDisplayMode;
	RuAdapterInfoD3D *bestAdapterInfo = NULL;
	RuDeviceInfoD3D *bestDeviceInfo = NULL;
	RuDeviceComboD3D *bestDeviceCombo = NULL;

	// Get display mode of primary adapter (which is assumed to be where the window will appear)
	d3dObject->GetAdapterDisplayMode(0, &primaryDesktopDisplayMode);

	for(INT32 adapterIndex = 0; adapterIndex < adapterInfoList.Count(); adapterIndex++)
	{
		RuAdapterInfoD3D *adapterInfo = adapterInfoList[adapterIndex];

		for(INT32 deviceIndex = 0; deviceIndex < adapterInfo->m_deviceInfoList.Count(); deviceIndex++)
		{
			RuDeviceInfoD3D *deviceInfo = adapterInfo->m_deviceInfoList[deviceIndex];

			// Check for HAL & REF constraints
			if(requireHAL && deviceInfo->m_deviceType != D3DDEVTYPE_HAL)
				continue;
			if(requireREF && deviceInfo->m_deviceType != D3DDEVTYPE_REF)
				continue;

			for(INT32 dcIndex = 0; dcIndex < deviceInfo->m_deviceComboList.Count(); dcIndex++)
			{
				RuDeviceComboD3D *deviceCombo = deviceInfo->m_deviceComboList[dcIndex];

				BOOL adapterMatchesBB = (deviceCombo->m_backBufferFormat == deviceCombo->m_adapterFormat);

				// Skip non-windowed combinations
				if(!deviceCombo->m_isWindowed)
					continue;

				// Skip formats which does not match the current desktop format
				if(deviceCombo->m_adapterFormat != primaryDesktopDisplayMode.Format)
					continue;

				// If we haven't found a compatible DeviceCombo yet, or if this set is better
				// (beCRuse it's a HAL, and/or beCRuse formats match better), save it
				if(	bestDeviceCombo == NULL ||
					(bestDeviceCombo->m_deviceType != D3DDEVTYPE_HAL && deviceCombo->m_deviceType == D3DDEVTYPE_HAL) ||
					(deviceCombo->m_deviceType == D3DDEVTYPE_HAL && adapterMatchesBB))
				{
					bestAdapterInfo = adapterInfo;
					bestDeviceInfo = deviceInfo;
					bestDeviceCombo = deviceCombo;

					// This windowed device combo looks great -- take it
					if(deviceCombo->m_deviceType == D3DDEVTYPE_HAL && adapterMatchesBB)
					{
						adapterIndex = adapterInfoList.Count();
						deviceIndex = adapterInfo->m_deviceInfoList.Count();
						break;
					}
				}
			}
		}
	}

	// No good combo found
	if(bestDeviceCombo == NULL)
		return FALSE;

	// Set D3D settings according to the selected device combo
	m_isWindowed = TRUE;
	m_windowedAdapterInfo = bestAdapterInfo;
	m_windowedDeviceInfo = bestDeviceInfo;
	m_windowedDeviceCombo = bestDeviceCombo;
	m_windowedDisplayMode = primaryDesktopDisplayMode;
	m_windowedDepthStencilBufferFormat = bestDeviceCombo->m_depthStencilFormatList[0];
	m_windowedMultisampleType = bestDeviceCombo->m_multisampleTypeList[0];
	m_windowedMultisampleQuality = 0;
	m_windowedEngineMode =  bestDeviceCombo->m_engineModeList[0];
	m_windowedPresentInterval = bestDeviceCombo->m_presentIntervalList[0];

	return TRUE;
}

BOOL CRuEngineD3D::RuEngineSettingD3D::FindBestFullscreenMode(IDirect3D9 *d3dObject, CRuArrayList<RuAdapterInfoD3D *> &adapterInfoList, BOOL requireHAL, BOOL requireREF)
{
	// For fullscreen, default to first HAL DeviceCombo that supports the current desktop 
	// display mode, or any display mode if HAL is not compatible with the desktop mode, or 
	// non-HAL if no HAL is available
	D3DDISPLAYMODE adapterDesktopDisplayMode;
	D3DDISPLAYMODE bestAdapterDesktopDisplayMode;
	D3DDISPLAYMODE bestDisplayMode;
	RuAdapterInfoD3D *bestAdapterInfo = NULL;
	RuDeviceInfoD3D *bestDeviceInfo = NULL;
	RuDeviceComboD3D *bestDeviceCombo = NULL;

	bestAdapterDesktopDisplayMode.Width = 0;
	bestAdapterDesktopDisplayMode.Height = 0;
	bestAdapterDesktopDisplayMode.Format = D3DFMT_UNKNOWN;
	bestAdapterDesktopDisplayMode.RefreshRate = 0;

	for(INT32 adapterIndex = 0; adapterIndex < adapterInfoList.Count(); adapterIndex++)
	{
		RuAdapterInfoD3D *adapterInfo = adapterInfoList[adapterIndex];
		d3dObject->GetAdapterDisplayMode(adapterInfo->m_adapterOrdinal, &adapterDesktopDisplayMode);

		for(INT32 deviceIndex = 0; deviceIndex < adapterInfo->m_deviceInfoList.Count(); deviceIndex++)
		{
			RuDeviceInfoD3D *deviceInfo = adapterInfo->m_deviceInfoList[deviceIndex];

			// Check for HAL & REF constraints
			if(requireHAL && deviceInfo->m_deviceType != D3DDEVTYPE_HAL)
				continue;
			if(requireREF && deviceInfo->m_deviceType != D3DDEVTYPE_REF)
				continue;

			for(INT32 dcIndex = 0; dcIndex < deviceInfo->m_deviceComboList.Count(); dcIndex++)
			{
				RuDeviceComboD3D *deviceCombo = deviceInfo->m_deviceComboList[dcIndex];

				BOOL adapterMatchesBB = (deviceCombo->m_backBufferFormat == deviceCombo->m_adapterFormat);
				BOOL adapterMatchesDesktop = (deviceCombo->m_adapterFormat == adapterDesktopDisplayMode.Format);

				// Skip windowed combinations
				if(deviceCombo->m_isWindowed)
					continue;

				// If we haven't found a compatible DeviceCombo yet, or if this set is better
				// (beCRuse it's a HAL, and/or beCRuse formats match better), save it
				if(	bestDeviceCombo == NULL ||
					(bestDeviceCombo->m_deviceType != D3DDEVTYPE_HAL && deviceCombo->m_deviceType == D3DDEVTYPE_HAL) ||
					(deviceCombo->m_deviceType == D3DDEVTYPE_HAL && bestDeviceCombo->m_adapterFormat != adapterDesktopDisplayMode.Format && adapterMatchesDesktop) ||
					(deviceCombo->m_deviceType == D3DDEVTYPE_HAL && adapterMatchesDesktop && adapterMatchesBB))
				{
					bestAdapterInfo = adapterInfo;
					bestDeviceInfo = deviceInfo;
					bestDeviceCombo = deviceCombo;

					// This fullscreen device combo looks great -- take it
					if(deviceCombo->m_deviceType == D3DDEVTYPE_HAL && adapterMatchesDesktop && adapterMatchesBB)
					{
						adapterIndex = adapterInfoList.Count();
						deviceIndex = adapterInfo->m_deviceInfoList.Count();
						break;
					}
				}
			}
		}
	}

	// No good combo found
	if(bestDeviceCombo == NULL)
		return FALSE;

	// Find a display mode on the best adapter that uses bestDeviceCombo->m_adapterFormat and is as close to our preferred resolution/refresh as possible
	if(m_preferredFullscreenWidth == 0 || m_preferredFullscreenHeight == 0)
	{
		m_preferredFullscreenWidth = bestAdapterDesktopDisplayMode.Width;
		m_preferredFullscreenHeight = bestAdapterDesktopDisplayMode.Height;
		m_preferredFullscreenRefreshRate = bestAdapterDesktopDisplayMode.RefreshRate;
	}
	bestDisplayMode.Width = 0;
	bestDisplayMode.Height = 0;
	bestDisplayMode.Format = D3DFMT_UNKNOWN;
	bestDisplayMode.RefreshRate = 0;
	for(INT32 displayModeIndex = 0; displayModeIndex < bestAdapterInfo->m_displayModeList.Count(); displayModeIndex++)
	{
		D3DDISPLAYMODE &dm = bestAdapterInfo->m_displayModeList[displayModeIndex];

		// Skip incompatible formats
		if(dm.Format != bestDeviceCombo->m_adapterFormat)
			continue;

		if(dm.Width == m_preferredFullscreenWidth &&
			dm.Height == m_preferredFullscreenHeight && 
			dm.RefreshRate == m_preferredFullscreenRefreshRate)
		{
			// Found a perfect match
			bestDisplayMode = dm;
			break;
		}
		else if(dm.Width == m_preferredFullscreenWidth &&
			dm.Height == m_preferredFullscreenHeight && 
			dm.RefreshRate > m_preferredFullscreenRefreshRate)
		{
			// Refresh rate doesn't match, but width/height match, keep this and keep looking
			bestDisplayMode = dm;
		}
		else if(dm.Width == m_preferredFullscreenWidth)
		{
			// Width matches, so keep this and keep looking
			bestDisplayMode = dm;
		}
		else if(bestDisplayMode.Width == 0)
		{
			// We don't have anything better yet, so keep this and keep looking
			bestDisplayMode = dm;
		}
	}

	// Set D3D settings according to the selected device combo
	m_isWindowed = FALSE;
	m_fullscreenAdapterInfo = bestAdapterInfo;
	m_fullscreenDeviceInfo = bestDeviceInfo;
	m_fullscreenDeviceCombo = bestDeviceCombo;
	m_fullscreenDisplayMode = bestDisplayMode;
	m_fullscreenDepthStencilBufferFormat = bestDeviceCombo->m_depthStencilFormatList[0];
	m_fullscreenMultisampleType = bestDeviceCombo->m_multisampleTypeList[0];
	m_fullscreenMultisampleQuality = 0;
	m_fullscreenEngineMode =  bestDeviceCombo->m_engineModeList[0];
	m_fullscreenPresentInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	return TRUE;
}

BOOL CRuEngineD3D::RuEngineSettingD3D::FindBestFullscreenMode()
{
	D3DDISPLAYMODE bestDisplayMode;

	// Find a display mode on the current adapter that uses bestDeviceCombo->m_adapterFormat and is as close to our preferred resolution/refresh as possible
	bestDisplayMode.Width = 0;
	bestDisplayMode.Height = 0;
	bestDisplayMode.Format = D3DFMT_UNKNOWN;
	bestDisplayMode.RefreshRate = 0;
	for(INT32 displayModeIndex = 0; displayModeIndex < m_fullscreenAdapterInfo->m_displayModeList.Count(); displayModeIndex++)
	{
		D3DDISPLAYMODE &dm = m_fullscreenAdapterInfo->m_displayModeList[displayModeIndex];

		// Skip incompatible formats
		if(dm.Format != m_fullscreenDeviceCombo->m_adapterFormat)
			continue;

		if(dm.Width == m_preferredFullscreenWidth &&
			dm.Height == m_preferredFullscreenHeight && 
			dm.RefreshRate == m_preferredFullscreenRefreshRate)
		{
			// Found a perfect match
			bestDisplayMode = dm;
			break;
		}
		else if(dm.Width == m_preferredFullscreenWidth &&
			dm.Height == m_preferredFullscreenHeight && 
			dm.RefreshRate > m_preferredFullscreenRefreshRate)
		{
			// Refresh rate doesn't match, but width/height match, keep this and keep looking
			bestDisplayMode = dm;
		}
		else if(dm.Width == m_preferredFullscreenWidth)
		{
			// Width matches, so keep this and keep looking
			bestDisplayMode = dm;
		}
		else if(bestDisplayMode.Width == 0)
		{
			// We don't have anything better yet, so keep this and keep looking
			bestDisplayMode = dm;
		}
	}

	return TRUE;
}

void CRuEngineD3D::RuEngineSettingD3D::BuildPresentParamsFromSettings(HWND deviceWindow, D3DPRESENT_PARAMETERS &presentParamsOut)
{
	presentParamsOut.Windowed = m_isWindowed;
	presentParamsOut.BackBufferCount = 1;
	presentParamsOut.MultiSampleType = MultisampleType();
	presentParamsOut.MultiSampleQuality = MultisampleQuality();
	presentParamsOut.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParamsOut.EnableAutoDepthStencil = TRUE;
	presentParamsOut.hDeviceWindow = deviceWindow;
	presentParamsOut.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	presentParamsOut.AutoDepthStencilFormat = DepthStencilBufferFormat();

	if(m_isWindowed)
	{
		presentParamsOut.BackBufferWidth = m_windowedWidth;
		presentParamsOut.BackBufferHeight = m_windowedHeight;
		presentParamsOut.BackBufferFormat = DeviceCombo()->m_backBufferFormat;
		presentParamsOut.FullScreen_RefreshRateInHz = 0;
		presentParamsOut.PresentationInterval = PresentInterval();
	}
	else
	{
		presentParamsOut.BackBufferWidth  = DisplayMode().Width;
		presentParamsOut.BackBufferHeight = DisplayMode().Height;
		presentParamsOut.BackBufferFormat = DeviceCombo()->m_backBufferFormat;
		presentParamsOut.FullScreen_RefreshRateInHz = m_fullscreenDisplayMode.RefreshRate;
		presentParamsOut.PresentationInterval = PresentInterval();
	}
}

void CRuEngineD3D::RuEngineSettingD3D::BuildPresentParamsForWindowed(HWND deviceWindow, UINT32 width, UINT32 height,  D3DPRESENT_PARAMETERS &presentParamsOut)
{
	presentParamsOut.Windowed = TRUE;
	presentParamsOut.BackBufferCount = 1;
	presentParamsOut.MultiSampleType = MultisampleType();
	presentParamsOut.MultiSampleQuality = MultisampleQuality();
	presentParamsOut.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParamsOut.EnableAutoDepthStencil = TRUE;
	presentParamsOut.hDeviceWindow = deviceWindow;
	presentParamsOut.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	presentParamsOut.AutoDepthStencilFormat = DepthStencilBufferFormat();
	presentParamsOut.BackBufferWidth = width;
	presentParamsOut.BackBufferHeight = height;
	presentParamsOut.BackBufferFormat = DeviceCombo()->m_backBufferFormat;
	presentParamsOut.FullScreen_RefreshRateInHz = 0;
	presentParamsOut.PresentationInterval = PresentInterval();
}

// ************************************************************************************************************************************************************

BOOL CRuEngineD3D::RuVEC::operator==(const RuVEC &ve) const
{
	return IsEqual(m_vertexElements,  ve.m_vertexElements);
}

// ************************************************************************************************************************************************************

/*!
	CRuEngineD3D::CRuEngineD3D()
	Standard constructor
*/
CRuEngineD3D::CRuEngineD3D()
:	m_d3dObject(NULL),
	m_d3dDevice(NULL),
	m_defaultDepthStencilSurface(NULL),
	m_resourceArenaInvalidated(TRUE),
	m_cursor_xHotSpot(0),
	m_cursor_yHotSpot(0),
	m_cursor_Texture(NULL),
	m_cursor_TextureIteration(0)
{
	INT32 vbPoolSizes[] = { 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 0 };
	m_staticVertexBufferPool.Initialize(vbPoolSizes, this, boost::bind(&CRuEngineD3D::CreateVertexBuffer_Internal, this, _1));

	INT32 ibPoolSizes[] = { 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 0 };
	m_staticIndexBufferPool.Initialize(ibPoolSizes, this, boost::bind(&CRuEngineD3D::CreateIndexBuffer_Internal, this, _1));

	m_dynamicVertexBufferPool.Initialize(vbPoolSizes, this, boost::bind(&CRuEngineD3D::CreateDynamicVertexBuffer_Internal, this, _1));

	m_dynamicIndexBufferPool.Initialize(ibPoolSizes, this, boost::bind(&CRuEngineD3D::CreateDynamicIndexBuffer_Internal, this, _1));

	m_rtTexturePool.Initialize(this, boost::bind(&CRuEngineD3D::CreateRenderTarget_Internal, this, _1, _2, _3, _4, _5, _6), TRUE);
	m_2dTexturePool.Initialize(this, boost::bind(&CRuEngineD3D::CreateTexture_Internal, this, _1, _2, _3, _4, _5, _6), FALSE);
	m_3dTexturePool.Initialize(this, boost::bind(&CRuEngineD3D::CreateVolumeTexture_Internal, this, _1, _2, _3, _4, _5, _6), FALSE);

	m_swapChains.Initialize(this);
	m_rtTextures.Initialize(this);
	m_depthStencilSurfaces.Initialize(this);

	// Initialize base application settings
	m_appMinFullscreenWidth = 800;
	m_appMinFullscreenHeight = 600;
//	m_appMinColorChannelBits = 8;
//	m_appMinAlphaChannelBits = 8;
	m_appMinColorChannelBits = 5;
	m_appMinAlphaChannelBits = 0;
	m_appMinDepthBits = 24;
	m_appMinStencilBits = 8;
	m_appUsesDepthBuffer = TRUE;
	m_appUsesMixedMode = FALSE;
	m_appRequiresWindowed = FALSE;
	m_appRequiresFullscreen = FALSE;
	
	// Set default values for engine settings
	m_engineSetting.m_windowedWidth = 800;
	m_engineSetting.m_windowedHeight = 600;
	m_engineSetting.m_preferredFullscreenWidth = 800;
	m_engineSetting.m_preferredFullscreenHeight = 600;
}

/*!
	CRuEngineD3D::~CRuEngineD3D()
	Standard destructor
*/
CRuEngineD3D::~CRuEngineD3D()
{
	Shutdown();
}

/*!
	BOOL CRuEngineD3D::Startup()
	Starts the engine core
	@return BOOL
	@retval TRUE The startup succeeded
	@retval FALSE The startup failed
*/
BOOL CRuEngineD3D::Startup(HWND primaryWindow)
{
	m_d3dObject = Direct3DCreate9(D3D_SDK_VERSION);

	if(m_d3dObject == NULL)
	{
		MessageBoxA(NULL, "Direct3D version does not match requirements.", "Error", MB_OK);
		return FALSE;
	}

	m_primaryWindow = primaryWindow;

	// Setup hashmap for vertex declarations
//	m_vertexDeclarations = ruNEW CRuHashMap<RuVEC, IRuVertexDeclaration *>(1024);

	// Reset state cache
	ClearStateCache();

	// Create profiling probes
	m_probe_StateChanges = g_ruProfiler->CreateProbe("State Changes", "Engine");
	m_probe_VSConstantChanges = g_ruProfiler->CreateProbe("VS Const Changes", "Engine");
	m_probe_PSConstantChanges = g_ruProfiler->CreateProbe("PS Const Changes", "Engine");

	return TRUE;
}

/*!
	BOOL CRuEngineD3D::Shutdown()
	Shuts down the engine core
	@return BOOL
	@retval TRUE The shutdown succeeded
	@retval FALSE The shutdown failed
*/
BOOL CRuEngineD3D::Shutdown()
{
	// Reset device
	ResetDevice();

	// Trigger shutdown event
	RuEventArgs eventArgs = { NULL };
	Event_Shutdown().Trigger(&eventArgs);

	// Release cursor
	ruSAFE_RELEASE(m_cursor_Texture);

	// Release all vertex declarations
	ClearVertexDeclarationCache();

//	delete m_vertexDeclarations;

	// Release d3d objects
	if(m_defaultDepthStencilSurface)
		m_defaultDepthStencilSurface->Release();

	if(m_d3dDevice)
		m_d3dDevice->Release();

	if(m_d3dObject)
		m_d3dObject->Release();

	// Clear adapter information
	for(INT32 i = 0; i < m_adapterInfoList.Count(); i++)
	{
		delete m_adapterInfoList[i];
	}
	m_adapterInfoList.Clear();
	m_allowedAdapterFormatList.Clear();

	m_d3dObject = NULL;
	m_d3dDevice = NULL;
	m_defaultDepthStencilSurface = NULL;

	return TRUE;
}

HWND CRuEngineD3D::GetPrimaryWindow()
{
	return m_primaryWindow;
}

void CRuEngineD3D::ClearStateCache()
{
	// Set render targets to uninitialized values
	for(INT32 i = 0; i < s_maxRenderTargets; ++i)
	{
		m_renderTargetsOK[i] = FALSE;
	}

	// Clear current depth stencil
	m_currentDepthStencilOK = FALSE;

	// CLear current viewport
	m_currentViewportOK = FALSE;

	// Set render states to uninitialized values
	for(INT32 i = 0; i < s_maxRenderStates; ++i)
	{
		m_renderStatesChanged[i] = FALSE;
		m_renderStatesOK[i] = FALSE;
	}

	// Set texture & sampler stage states to uninitialized values
	for(INT32 i = 0; i < s_maxStages; ++i)
	{
		for(INT32 j = 0; j < s_maxTextureStates; ++j)
		{
			m_textureStageStatesChanged[i][j] = FALSE;
			m_textureStageStatesOK[i][j] = FALSE;
		}

		for(INT32 j = 0; j < s_maxSamplerStates; ++j)
		{
			m_samplerStageStatesChanged[i][j] = FALSE;
			m_samplerStageStatesOK[i][j] = FALSE;
		}
	}

	// Reset vertex and pixel shader settings
	m_currentVertexDeclarationOK = FALSE;
	m_currentVertexShaderOK = FALSE;
	m_currentPixelShaderOK = FALSE;

	m_currentVertexDeclaration = NULL;
	m_currentVertexShader = NULL;
	m_currentPixelShader = NULL;

	// Reset vertex buffer states
	for(INT32 i = 0; i < s_maxVertexStreams; ++i)
	{
		m_currentVertexStreamOK[i] = FALSE;
		m_currentVertexStream[i] = NULL;
		m_currentVertexStreamStride[i] = 0;
	}

	// Reset index buffer states
	m_currentIndexStreamOK = FALSE;
	m_currentIndexStream = NULL;

	// Reset sampler texture states
	for(INT32 i = 0; i < s_maxSamplers; ++i)
	{
		m_currentTextureOK[i] = FALSE;
		m_currentTexture[i] = NULL;
	}

	// Reset vertex shader constants buffer
	m_vsConstants_Min = 256;
	m_vsConstants_Max = 0;
	memset(m_vsConstants, 0, sizeof(REAL) * 256 * 4);

	// Reset pixel shader constants buffer
	m_psConstants_Min = 256;
	m_psConstants_Max = 0;
	memset(m_psConstants, 0, sizeof(REAL) * 256 * 4);
}

void CRuEngineD3D::ClearSampler(DWORD sampler)
{
	SetTexture(sampler, NULL);
}

BOOL CRuEngineD3D::BeginScene()
{
	if(m_d3dDevice == NULL)
		return FALSE;
/*
	// 
	for(INT32 i = 0; i < m_outstandingQueries.Count(); ++i)
	{
		BOOL eventData;
		if(m_outstandingQueries[i]->GetData(reinterpret_cast<void *>(&eventData), sizeof(BOOL), 0) == S_OK)
		{
			if(eventData)
			{
				m_unusedQueries.Push(m_outstandingQueries[i]);
				m_outstandingQueries.RemoveAt(i);
			}
		}
	}

	if(m_outstandingQueries.Count() > 1)
	{
//		OutputDebugString("A\n");
		return FALSE;
	}

	g_ruCounter_3 = m_outstandingQueries.Count();
*/



	if(m_d3dDevice->BeginScene() != D3D_OK)
		return FALSE;

	// Reset metric values
	m_numRequestedStateChanges = 0;
	m_numStateChanges = 0;

	// Trigger begin scene event
	RuEventArgs eventArgs = { NULL };
	Event_BeginScene().Trigger(&eventArgs);



/*
	// Create query
	IDirect3DQuery9 *query = NULL;
	if(m_unusedQueries.IsEmpty())
	{
		m_d3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, &query);
	}
	else
	{
		query = m_unusedQueries.Pop();
	}

	query->Issue(D3DISSUE_END);
	m_outstandingQueries.Add(query);
*/



	return TRUE;
}

BOOL CRuEngineD3D::EndScene()
{
	if(m_d3dDevice == NULL)
		return FALSE;

	if(m_d3dDevice->EndScene() != D3D_OK)
		return FALSE;

	return TRUE;
}

BOOL CRuEngineD3D::Clear(DWORD clearFlags, RUCOLOR color, float z, DWORD stencil)
{
	// Forcibly flush state changes before performing clear operation
	FlushStateChanges();

	if(m_d3dDevice == NULL)
		return FALSE;

	if(m_d3dDevice->Clear(0, NULL, clearFlags, color, z, stencil) != D3D_OK)
		return FALSE;

	// Clear specific states affected upon a clear call
	m_currentViewportOK = FALSE;

	return TRUE;
}

BOOL CRuEngineD3D::ResetViewport()
{
	// Get local reference to the current render target
	IDirect3DSurface9 *currentRenderTarget;
	if(FAILED(m_d3dDevice->GetRenderTarget(0, &currentRenderTarget)))
	{
		return FALSE;
	}

	// Get surface description
	D3DSURFACE_DESC surfaceDesc;
	if(FAILED(currentRenderTarget->GetDesc(&surfaceDesc)))
	{
		return FALSE;
	}

	// Construct viewport structure
	D3DVIEWPORT9 viewport;

	viewport.X = 0;
	viewport.Y = 0;
	viewport.Width = surfaceDesc.Width;
	viewport.Height = surfaceDesc.Height;
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;

	// Apply viewport setting and clear viewport cache
	m_currentViewportOK = FALSE;
	m_d3dDevice->SetViewport(&viewport);

	// Release local reference to current render target
	currentRenderTarget->Release();

	return TRUE;
}

BOOL CRuEngineD3D::RestoreDefaultDepthStencil()
{
	return SetDepthStencil(m_defaultDepthStencilSurface);
}

BOOL CRuEngineD3D::SetScissorRect(RECT *scissorRect)
{
	if(m_d3dDevice)
	{
		if(scissorRect)
		{
			m_d3dDevice->SetScissorRect(scissorRect);
			SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
		}
		else
		{
			SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		}
	}

	return FALSE;
}

BOOL CRuEngineD3D::SetClipPlane(INT32 index, const CRuPlane4 &plane)
{
	// Set clip plane
	if(FAILED(m_d3dDevice->SetClipPlane(index, plane.m_elements)))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CRuEngineD3D::SetClipPlaneEnable(INT32 index, BOOL enable)
{
	DWORD clipPlaneFlags = m_renderStatesOK[D3DRS_CLIPPLANEENABLE] ? m_renderStates[D3DRS_CLIPPLANEENABLE] : 0;
	DWORD bitMask = 1 << index;

	clipPlaneFlags = (clipPlaneFlags & ~bitMask) | (((enable ? 1 : 0) << index) & bitMask);

	return this->SetRenderState(D3DRS_CLIPPLANEENABLE, clipPlaneFlags);
}

IRuVertexStreamLayout *CRuEngineD3D::CreateVertexStreamLayout(const CRuVertexElement *vertexElements, INT32 streamIdx)
{
	CRuVertexStreamLayout *streamLayout = NULL;

	if(StreamHasElements(vertexElements, streamIdx))
	{
		// Construct vertex stream layout object from the vertex elements
		streamLayout = ruNEW CRuVertexStreamLayout(vertexElements, streamIdx);

		// Find an existing equivalent vertex stream layout object
		CRuVertexStreamLayout *equivStreamLayout = NULL;
		for(INT32 j = 0; j < m_vertexStreamLayouts.Count(); ++j)
		{
			if(streamLayout->IsEqualTo(m_vertexStreamLayouts[j]))
			{
				equivStreamLayout = m_vertexStreamLayouts[j];
				break;
			}
		}

		if(equivStreamLayout)
		{
			// If an existing equivalent vertex stream layout object exists, use the existing object instead
			ruSAFE_RELEASE(streamLayout);
			streamLayout = equivStreamLayout;
		}
		else
		{
			// Add to list of vertex stream layouts
			m_vertexStreamLayouts.Add(streamLayout);
		}
	}

	return streamLayout;
}

/*!
	IRuVertexDeclaration *CRuEngineD3D::CreateVertexDeclaration(const CRuVertexElement *vertexElements)
	Creates a vertex declaration
	@param vertexElements
	@return IRuVertexDeclaration*
	@retval NULL Could not create the vertex declaration
*/
IRuVertexDeclaration *CRuEngineD3D::CreateVertexDeclaration(const CRuVertexElement *vertexElements)
{
	// Create D3D vertex declaration
	IDirect3DVertexDeclaration9	*d3dDecl;
	if(m_d3dDevice->CreateVertexDeclaration((D3DVERTEXELEMENT9 *) vertexElements, &d3dDecl) == D3D_OK)
	{
		// Create new vertex declaration object
		CRuVertexDeclarationD3D *vertexDeclaration = ruNEW CRuVertexDeclarationD3D(d3dDecl);
		vertexDeclaration->SetEngine(this);
		ruSAFE_RELEASE(d3dDecl);

		// Process all vertex streams
		INT32 numVertexStreams = GetNumVertexStreams(vertexElements);

		for(INT32 i = 0; i < numVertexStreams; ++i)
		{
			// Create vertex stream layout object form the vertex elements for the given stream index
			IRuVertexStreamLayout *streamLayout = CreateVertexStreamLayout(vertexElements, i);

			// Insert into vertex declaration
			vertexDeclaration->BindVertexStreamLayout(i, streamLayout);
		}

		// Find an existing equivalent vertex declaration object
		CRuVertexDeclarationD3D *equivVertexDecl = NULL;
		for(INT32 i = 0; i < m_vertexDeclarations.Count(); ++i)
		{
			if(vertexDeclaration->IsEqualTo(m_vertexDeclarations[i]))
			{
				equivVertexDecl = m_vertexDeclarations[i];
				break;
			}
		}

		if(equivVertexDecl)
		{
			ruSAFE_RELEASE(vertexDeclaration);
			vertexDeclaration = equivVertexDecl;
		}
		else
		{
			m_vertexDeclarations.Add(vertexDeclaration);
		}

		// Increment ref count on vertex declaration and return it
		vertexDeclaration->AddRef();
		return vertexDeclaration;
	}

	return NULL;
}

/*!
	IRuVertexBuffer *CRuEngineD3D::CreateVertexBuffer(UINT32 size)
	Creates a static vertex buffer
	@param size The size of the vertex buffer
	@return IRuVertexBuffer*
	@retval NULL Could not create the vertex buffer
*/
IRuVertexBuffer *CRuEngineD3D::CreateVertexBuffer(UINT32 size)
{
	CRuVertexBufferD3D *vertexBuffer = static_cast<CRuVertexBufferD3D *>(m_staticVertexBufferPool.CreateBuffer(size));

	return vertexBuffer;
}

/*!
	IRuVertexBuffer *CRuEngineD3D::CreateDynamicVertexBuffer(UINT32 size)
	Creates a dynamic vertex buffer
	@param size The size of the vertex buffer
	@return IRuVertexBuffer*
	@retval NULL Could not create the vertex buffer
*/
IRuVertexBuffer *CRuEngineD3D::CreateDynamicVertexBuffer(UINT32 size)
{
	CRuVertexBufferD3D *vertexBuffer = static_cast<CRuVertexBufferD3D *>(m_dynamicVertexBufferPool.CreateBuffer(size));

	return vertexBuffer;
}

/*!
	IRuIndexBuffer *CRuEngineD3D::CreateIndexBuffer(UINT32 size)
	Creates a static index buffer
	@param size The size of the index buffer
	@return IRuIndexBuffer*
	@retval NULL Could not create the index buffer
*/
IRuIndexBuffer *CRuEngineD3D::CreateIndexBuffer(UINT32 size)
{
	CRuIndexBufferD3D *indexBuffer = static_cast<CRuIndexBufferD3D *>(m_staticIndexBufferPool.CreateBuffer(size));

	return indexBuffer;
}

/*!
	IRuIndexBuffer *CRuEngineD3D::CreateDynamicIndexBuffer(UINT32 size)
	Creates a dynamic index buffer
	@param size The size of the index buffer
	@return IRuIndexBuffer*
	@retval NULL Could not create the index buffer
*/
IRuIndexBuffer *CRuEngineD3D::CreateDynamicIndexBuffer(UINT32 size)
{
	CRuIndexBufferD3D *indexBuffer = static_cast<CRuIndexBufferD3D *>(m_dynamicIndexBufferPool.CreateBuffer(size));

	return indexBuffer;
}

/*!
	IRuViewport *CRuEngineD3D::CreateViewport(INT32 x, INT32 y, INT32 width, INT32 height)
	Creates a viewport
	@param x
	@param y
	@param width
	@param height
	@return IRuViewport*
	@retval NULL Could not create the viewport
*/
IRuViewport *CRuEngineD3D::CreateViewport(INT32 x, INT32 y, INT32 width, INT32 height)
{
	// Construct viewport object
	CRuViewportD3D *viewport = ruNEW CRuViewportD3D();
	viewport->SetEngine(this);
	viewport->SetViewportParameters(x, y, width, height, 0.0f, 1.0f);

	return viewport;
}

/*!
	IRuTexture *CRuEngineD3D::CreateTexture(UINT32 width, UINT32 height, UINT32 mipLevels, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool)
	Creates a 1D or 2D texture
	@param width
	@param height
	@param mipLevels
	@param surfaceFormat
	@param resourcePool
	@return IRuTexture*
	@retval NULL Could not create the texture
*/
IRuTexture *CRuEngineD3D::CreateTexture(UINT32 width, UINT32 height, UINT32 mipLevels, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool)
{
	IRuTexture *texture = static_cast<IRuTexture *>(m_2dTexturePool.CreateTexture(width, height, 0, mipLevels, surfaceFormat, resourcePool));

	return texture;
}

/*!
	IRuTexture *CRuEngineD3D::CreateVolumeTexture(UINT32 width, UINT32 height, UINT32 depth, UINT32 mipLevels, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool)
	Creates a volume texture
	@param width
	@param height
	@param depth
	@param mipLevels
	@param surfaceFormat
	@param resourcePool
	@return IRuTexture*
	@retval NULL Could not create the texture
*/
IRuVolumeTexture *CRuEngineD3D::CreateVolumeTexture(UINT32 width, UINT32 height, UINT32 depth, UINT32 mipLevels, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool)
{
	IRuVolumeTexture *texture = static_cast<IRuVolumeTexture *>(m_3dTexturePool.CreateTexture(width, height, depth, mipLevels, surfaceFormat, resourcePool));

	return texture;
}

/*!
	IRuTexture *CRuEngineD3D::CreateRenderTarget(UINT32 width, UINT32 height, UINT32 mipLevels, RuSurfaceFormat surfaceFormat)
	Creates a render target
	@param width
	@param height
	@param mipLevels
	@param surfaceFormat
	@return IRuTexture*
	@retval NULL Could not create the render target
*/
IRuTexture *CRuEngineD3D::CreateRenderTarget(UINT32 width, UINT32 height, UINT32 mipLevels, RuSurfaceFormat surfaceFormat)
{
	IRuTexture *texture = static_cast<IRuTexture *>(m_rtTexturePool.CreateTexture(width, height, 0, 1, surfaceFormat, ruRESPOOL_DEFAULT));

	return texture;
}

/*!
	IRuTexture *CRuEngineD3D::CreateMatchedRenderTarget(UINT32 mipLevels, RuSurfaceFormat surfaceFormat)
	Creates a render target matched to the dimensions of the back buffer
	@param mipLevels
	@param surfaceFormat
	@return IRuTexture*
	@retval NULL Could not create the render target
*/
IRuTexture *CRuEngineD3D::CreateMatchedRenderTarget(UINT32 mipLevels, RuSurfaceFormat surfaceFormat)
{
	IDirect3DTexture9 *d3dTexture = NULL;

	// Retrieve information about the back buffer
	D3DSURFACE_DESC bbDesc;
	IDirect3DSurface9 *backBuffer = NULL;
	if(m_d3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer) == D3D_OK)
	{
		backBuffer->GetDesc(&bbDesc);
		backBuffer->Release();
	}

	// Create the D3D texture
	if(m_d3dDevice->CreateTexture(bbDesc.Width, bbDesc.Height, mipLevels, D3DUSAGE_RENDERTARGET, (D3DFORMAT) surfaceFormat, D3DPOOL_DEFAULT, &d3dTexture, NULL) == D3D_OK)
	{
		// Create the texture object
		CRuTextureD3D *texture = ruNEW CRuTextureD3D(d3dTexture, FALSE, TRUE);
		texture->SetEngine(this);

		// Add to list of render targets
		m_rtTextures.Add(texture);

		return texture;
	}

	return NULL;
}

/*!
	IRuTextureView *CRuEngineD3D::CreateTextureView()
	Creates a default, empty texture view
	@return IRuTextureView*
	@retval NULL Could not create the texture view
*/
IRuTextureView *CRuEngineD3D::CreateTextureView()
{
	CRuTextureViewD3D *textureView = ruNEW CRuTextureViewD3D();
	textureView->SetEngine(this);
	return textureView;
}

IRuSurface *CRuEngineD3D::CreateDepthStencilSurface(UINT32 width, UINT32 height, RuSurfaceFormat surfaceFormat)
{
	// Create depth stencil surface
	IDirect3DSurface9 *depthStencilSurface = NULL;
	HRESULT hr = m_d3dDevice->CreateDepthStencilSurface(width, height, (D3DFORMAT) surfaceFormat, D3DMULTISAMPLE_NONE, 0, TRUE, &depthStencilSurface, NULL);
	if(FAILED(hr))
		return NULL;

	// Create depth stencil surface object
	CRuSurfaceD3D *surfaceObject = ruNEW CRuSurfaceD3D(depthStencilSurface);

	// Add to list of depth stencil surfaces
	m_depthStencilSurfaces.Add(surfaceObject);

	return surfaceObject;
}

BOOL CRuEngineD3D::CopyTexture(IRuEngineResource *srcTexture, IRuEngineResource *destTexture, INT32 surfLevel)
{
	HRESULT retVal = E_FAIL;

	IDirect3DSurface9 *srcSurface = NULL;
	IDirect3DSurface9 *dstSurface = NULL;

	if(srcTexture->GetType().IsSubClassOf(IRuTexture::Type()))
	{
		IRuSurface *ruSurface = static_cast<IRuTexture *>(srcTexture)->GetLevelSurface(surfLevel);
		if(ruSurface && ruSurface->GetType() == CRuSurfaceD3D::Type())
		{
			srcSurface = static_cast<CRuSurfaceD3D *>(ruSurface)->GetIDirect3DSurface();
		}
	}
	else if(srcTexture->GetType() == CRuSwapChainD3D::Type())
	{
		srcSurface = static_cast<CRuSwapChainD3D *>(srcTexture)->GetIDirect3DSurface();
	}

	if(destTexture->GetType().IsSubClassOf(IRuTexture::Type()))
	{
		IRuSurface *ruSurface = static_cast<IRuTexture *>(destTexture)->GetLevelSurface(surfLevel);
		if(ruSurface && ruSurface->GetType() == CRuSurfaceD3D::Type())
		{
			dstSurface = static_cast<CRuSurfaceD3D *>(ruSurface)->GetIDirect3DSurface();
		}
	}

	if(srcSurface && dstSurface)
	{
		retVal = m_d3dDevice->StretchRect(srcSurface, NULL, dstSurface, NULL, D3DTEXF_LINEAR);
	}

	return retVal == D3D_OK;
}

BOOL CRuEngineD3D::CopyTextureWithConversion(IRuEngineResource *srcTexture, IRuEngineResource *destTexture, INT32 surfLevel)
{
	if(srcTexture->GetType() == CRuTextureD3D::Type() && destTexture->GetType() == CRuTextureD3D::Type())
	{
		IDirect3DSurface9 *dstSurface = NULL;
		IDirect3DSurface9 *srcSurface = NULL;

		static_cast<CRuTextureD3D *>(destTexture)->GetIDirect3DTexture()->GetSurfaceLevel(surfLevel, &dstSurface);
		static_cast<CRuTextureD3D *>(srcTexture)->GetIDirect3DTexture()->GetSurfaceLevel(surfLevel, &srcSurface);

		D3DXLoadSurfaceFromSurface(dstSurface , NULL, NULL, srcSurface, NULL, NULL, D3DX_FILTER_TRIANGLE, 0);

		dstSurface->Release();
		srcSurface->Release();

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEngineD3D::UpdateTexture(IRuBaseTexture *srcTexture, IRuBaseTexture *dstTexture)
{
	if(srcTexture->GetType() == CRuTextureD3D::Type() && dstTexture->GetType() == CRuTextureD3D::Type())
	{
		IDirect3DTexture9 *srcTextureD3D = static_cast<CRuTextureD3D *>(srcTexture)->GetIDirect3DTexture();
		IDirect3DTexture9 *dstTextureD3D = static_cast<CRuTextureD3D *>(dstTexture)->GetIDirect3DTexture();

		HRESULT updateResult = m_d3dDevice->UpdateTexture(srcTextureD3D, dstTextureD3D);

		if(updateResult == D3D_OK)
		{
			return TRUE;
		}
	}
	else if(srcTexture->GetType() == CRuVolumeTextureD3D::Type() && dstTexture->GetType() == CRuVolumeTextureD3D::Type())
	{
		IDirect3DVolumeTexture9 *srcTextureD3D = static_cast<CRuVolumeTextureD3D *>(srcTexture)->GetIDirect3DVolumeTexture();
		IDirect3DVolumeTexture9 *dstTextureD3D = static_cast<CRuVolumeTextureD3D *>(dstTexture)->GetIDirect3DVolumeTexture();

		HRESULT updateResult = m_d3dDevice->UpdateTexture(srcTextureD3D, dstTextureD3D);

		if(updateResult == D3D_OK)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/*!
	BOOL CRuEngineD3D::SetVertexStream(INT32 streamIdx, IRuVertexBuffer *vertexBuffer, INT32 stride)
	Sets the vertex stream for the specified stream index.
	@param streamIdx
	@param vertexBuffer
	@param stride
	@return BOOL TRUE if successful, FALSE if failure
*/
BOOL CRuEngineD3D::SetVertexStream(INT32 streamIdx, IRuVertexBuffer *vertexBuffer, INT32 stride)
{
	assert(m_d3dDevice);

	++m_numRequestedStateChanges;

	if(m_currentVertexStream[streamIdx] != vertexBuffer || m_currentVertexStreamStride[streamIdx] != stride || m_currentVertexStreamOK[streamIdx] == FALSE)
	{
		++m_numStateChanges;

		RUPROFILE_SAMPLE_BEGIN(m_probe_StateChanges, 3);

		HRESULT result = m_d3dDevice->SetStreamSource(streamIdx, static_cast<IDirect3DVertexBuffer9 *>(*static_cast<CRuVertexBufferD3D *>(vertexBuffer)), 0, stride);

		RUPROFILE_SAMPLE_END(m_probe_StateChanges, 3);

		if(FAILED(result))
			return result;

		m_currentVertexStreamOK[streamIdx] = TRUE;
		m_currentVertexStream[streamIdx] = vertexBuffer;
		m_currentVertexStreamStride[streamIdx] = stride;
	}

	return TRUE;
}

/*!
	BOOL CRuEngineD3D::SetIndexStream(IRuIndexBuffer *indexBuffer)
	Sets the current index stream.
	@param indexBuffer
	@return BOOL TRUE if successful, FALSE if failure
*/
BOOL CRuEngineD3D::SetIndexStream(IRuIndexBuffer *indexBuffer)
{
	// Assert for correct type
	assert(indexBuffer->GetType() == CRuIndexBufferD3D::Type());

	// Assert for valid device
	assert(m_d3dDevice);

	++m_numRequestedStateChanges;

	// Check against cached entries
	if(m_currentIndexStream != indexBuffer || m_currentIndexStreamOK == FALSE)
	{
		++m_numStateChanges;

		RUPROFILE_SAMPLE_BEGIN(m_probe_StateChanges, 3);

		HRESULT result = m_d3dDevice->SetIndices(static_cast<IDirect3DIndexBuffer9 *>(*static_cast<CRuIndexBufferD3D *>(indexBuffer)));

		RUPROFILE_SAMPLE_END(m_probe_StateChanges, 3);

		if(FAILED(result))
			return FALSE;

		m_currentIndexStreamOK = TRUE;
		m_currentIndexStream = indexBuffer;
	}

	return TRUE;
}

/*!
	UINT32 CRuEngineD3D::Render(UINT32 numVertices, UINT32 numPrimitives, UINT32 primitiveOffset, RuPrimitiveType primType)
	Renders the currently selected buffers.
	@param numVertices
	@param numPrimitives
	@param primitiveOffset
	@param primType
	@return UINT32 The number of primitives rendered
*/
UINT32 CRuEngineD3D::Render(UINT32 numVertices, UINT32 numPrimitives, UINT32 primitiveOffset, RuPrimitiveType primType)
{
	// Flush all state changes to device
	FlushStateChanges();

	// Draw primitive based on primitive type
	switch(primType)
	{
		case ruPRIMTYPE_LINELIST:
			m_d3dDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, numVertices, primitiveOffset * 2, numPrimitives);
			return numPrimitives;

		case ruPRIMTYPE_TRIANGLELIST:
			m_d3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, numVertices, primitiveOffset * 3, numPrimitives);
			return numPrimitives;

		case ruPRIMTYPE_POINTLIST:
			m_d3dDevice->DrawPrimitive(D3DPT_POINTLIST, 0, numVertices);
			return numVertices;
	}

	return 0;
}

/*!
	IRuSwapChain *CRuEngineD3D::CreateSwapChain(UINT32 width, UINT32 height, HWND focusWindow)
	Creates a new swap chain
	@param width
	@param height
	@param focusWindow Window to which the swap chain is attached
	@return IRuSwapChain*
	@retval NULL Operation failed
*/
IRuSwapChain *CRuEngineD3D::CreateSwapChain(UINT32 width, UINT32 height, HWND deviceWindow)
{
	IDirect3DSwapChain9 *d3dSwapChain = NULL;
	CRuSwapChainD3D *swapChain = NULL;
	D3DPRESENT_PARAMETERS presentParams;
	HRESULT hr;

	// If there are no swap chains, this is the primary swap chain
	if(m_swapChains.Count() == 0)
	{
		RuDeviceInfoD3D *deviceInfo = m_engineSetting.DeviceInfo();

		// Check against device cap bits to ensure at least VS & PS 2 compatibility
		DWORD vsMajor = (deviceInfo->m_caps.VertexShaderVersion & 0x0000FF00) >> 8;
		DWORD psMajor = (deviceInfo->m_caps.PixelShaderVersion & 0x0000FF00) >> 8;

		if(vsMajor < 2 || psMajor < 2)
		{
			MessageBox(NULL, "Your GPU does not meet minimum specification!", "ERROR", MB_OK | MB_ICONERROR);
		}

		// Build present parameters
		m_engineSetting.m_isWindowed = TRUE;			// Force windowed mode on creation of the swap chain
		m_engineSetting.m_windowedWidth = width;
		m_engineSetting.m_windowedHeight = height;
		m_engineSetting.BuildPresentParamsFromSettings(deviceWindow, presentParams);

		DWORD behaviorFlags;

		if (m_engineSetting.EngineMode() == ruENGINEMODE_SOFTWARE)
			behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		else if (m_engineSetting.EngineMode() == ruENGINEMODE_MIXED)
			behaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
		else if (m_engineSetting.EngineMode() == ruENGINEMODE_HARDWARE)
			behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else if (m_engineSetting.EngineMode() == ruENGINEMODE_PURE)
			behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
		else
			behaviorFlags = 0; // TODO: throw exception

		// Force D3D to preserve FPU precision setting
		behaviorFlags |= D3DCREATE_FPU_PRESERVE;

		// Create the device
/*
		// Profiling only
		for(UINT i = 0; i < m_d3dObject->GetAdapterCount(); ++i)
		{
			D3DADAPTER_IDENTIFIER9 ident;
			m_d3dObject->GetAdapterIdentifier(i,0, &ident);
			if(strcmp(ident.Description, "NVIDIA PerfHUD") == 0)
			{
				hr = m_d3dObject->CreateDevice(i, D3DDEVTYPE_REF, m_primaryWindow, behaviorFlags, &presentParams, &m_d3dDevice);
				if(FAILED(hr))
					return NULL;

				break;
			}
		}
*/
		if(m_d3dDevice == NULL)
		{
			hr = m_d3dObject->CreateDevice(m_engineSetting.GetAdapterOrdinal(), deviceInfo->m_deviceType, m_primaryWindow, behaviorFlags, &presentParams, &m_d3dDevice);

			if(FAILED(hr))
			{
				// Forcibly use software vertex processing (work-around for Intel) and try again
				behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;

				hr = m_d3dObject->CreateDevice(m_engineSetting.GetAdapterOrdinal(), deviceInfo->m_deviceType, m_primaryWindow, behaviorFlags, &presentParams, &m_d3dDevice);

				if(FAILED(hr))
				{
					return NULL;
				}
			}
		}

		// Retrieve the device's caps
		if(FAILED(hr = m_d3dDevice->GetDeviceCaps(&m_d3dDeviceCaps)))
			return NULL;

		// Retrieve Direct3D's implicit swap chain
		if(FAILED(hr = m_d3dDevice->GetSwapChain(0, &d3dSwapChain)))
			return NULL;

		// Retrieve Direct3D's implicit depth stencil surface
		if(FAILED(hr = m_d3dDevice->GetDepthStencilSurface(&m_defaultDepthStencilSurface)))
			return NULL;

		// Create swap chain object
		swapChain = ruNEW CRuSwapChainD3D(d3dSwapChain, TRUE);
		swapChain->SetEngine(this);

		// Add swap chain to the swap chain list
		m_swapChains.Add(swapChain);
	}
	else
	{
		// Build present parameters
		m_engineSetting.BuildPresentParamsForWindowed(deviceWindow, width, height, presentParams);

		// Create additional swap chain
		if(FAILED(hr = m_d3dDevice->CreateAdditionalSwapChain(&presentParams, &d3dSwapChain)))
			return NULL;

		// Create swap chain object
		swapChain = ruNEW CRuSwapChainD3D(d3dSwapChain, FALSE);
		swapChain->SetEngine(this);

		// Add swap chain to the swap chain list
		m_swapChains.Add(swapChain);
	}

	return swapChain;
}

BOOL CRuEngineD3D::IsFullscreen()
{
	return m_engineSetting.m_isWindowed ? FALSE : TRUE;
}

/*!
	BOOL CRuEngineD3D::SetResolution(IRuSwapChain *swapChain, UINT32 width, UINT32 height, BOOL fullscreen, HWND deviceWindow)
	Sets the resolution for the given swap chain
	@param swapChain Swap chain to set resolution on
	@param width
	@param height
	@param fullscreen Whether to set the swap chain into fullscreen mode, only valid for the primary swap chain
	@param deviceWindow If non-NULL, designates the new device window for the swap chain
	@return BOOL
	@retval TRUE Operation succeeded
	@retval FALSE Operation failed
*/
BOOL CRuEngineD3D::SetResolution(IRuSwapChain *swapChain, UINT32 width, UINT32 height, BOOL fullscreen, HWND deviceWindow)
{
	// Ensure the swap chain is owned by the engine
	INT32 swapChainIndex = m_swapChains.IndexOf(static_cast<CRuSwapChainD3D *>(swapChain));

	if(swapChainIndex >= 0)
	{
		// Reset the device
		ResetDevice();

		CRuSwapChainD3D *primarySwapChain = static_cast<CRuSwapChainD3D *>(m_swapChains[0]);
		CRuSwapChainD3D *swapChain = static_cast<CRuSwapChainD3D *>(m_swapChains[swapChainIndex]);

		swapChain->PresentParameters().BackBufferWidth = width;
		swapChain->PresentParameters().BackBufferHeight = height;
		swapChain->PresentParameters().Windowed = TRUE;

		if(deviceWindow != NULL)
		{
			swapChain->PresentParameters().hDeviceWindow = deviceWindow;
		}

		// If fullscreen mode is enabled, set its properties and find the best display mode
		if(swapChainIndex == 0 && fullscreen)
		{
			swapChain->PresentParameters().Windowed = FALSE;

			// Setup resolution selection
			m_appMinFullscreenWidth = width;
			m_appMinFullscreenHeight = height;

			m_engineSetting.m_isWindowed = FALSE;
			m_engineSetting.m_preferredFullscreenWidth = m_appMinFullscreenWidth;
			m_engineSetting.m_preferredFullscreenHeight = m_appMinFullscreenHeight;
			m_engineSetting.FindBestFullscreenMode();
		}
		else
		{
			swapChain->PresentParameters().FullScreen_RefreshRateInHz = 0;

			m_engineSetting.m_isWindowed = TRUE;
		}

		// Restore the device
		RestoreDevice();

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEngineD3D::ResetDevice()
{
	// Release implicit depth stencil surface
	if(m_defaultDepthStencilSurface)
	{
		m_defaultDepthStencilSurface->Release();
		m_defaultDepthStencilSurface = NULL;
	}

	// Evict all managed resources
	m_d3dDevice->EvictManagedResources();

	for(INT32 i = 0; i < m_outstandingQueries.Count(); ++i)
	{
		m_outstandingQueries[i]->Release();
	}

	m_outstandingQueries.Clear();

	while(m_unusedQueries.IsEmpty() == FALSE)
	{
		IDirect3DQuery9 *query = m_unusedQueries.Pop();

		query->Release();
	}

	// Trigger device reset
	RuEventArgs eventArgs = { NULL };
	Event_DeviceReset().Trigger(&eventArgs);

	return TRUE;
}

BOOL CRuEngineD3D::RestoreDevice()
{
	HRESULT hr;

	// Reset state cache
	ClearStateCache();

	// Reset D3D device
	CRuSwapChainD3D *primarySwapChain = static_cast<CRuSwapChainD3D *>(m_swapChains[0]);
	hr = m_d3dDevice->Reset(&primarySwapChain->PresentParameters());

	if(FAILED(hr))
	{
		OutputDebugString("Reset failed\n");
		return FALSE;
	}

	// Retrieve Direct3D's implicit depth stencil surface
	if(FAILED(hr = m_d3dDevice->GetDepthStencilSurface(&m_defaultDepthStencilSurface)))
		return FALSE;

	// Restore cursor
	if(m_cursor_Texture)
	{
		IRuTexture *cursorTexture = m_cursor_Texture;
		cursorTexture->AddRef();
		m_cursor_Texture = NULL;
		this->SetCursor(m_cursor_xHotSpot, m_cursor_yHotSpot, cursorTexture);
		ruSAFE_RELEASE(cursorTexture);
	}

	// Enable dialog box mode
	m_d3dDevice->SetDialogBoxMode(TRUE);

	// Trigger device restore event
	RuEventArgs eventArgs = { NULL };
	Event_DeviceRestore().Trigger(&eventArgs);

	return TRUE;
}

IRuGPUQuery *CRuEngineD3D::CreateQuery(RuGPUQueryType queryType)
{
	HRESULT hr;

	// Convert queryType enum to D3DQUERYTYPE
	D3DQUERYTYPE d3dQueryType;
	switch(queryType)
	{
		case ruGPUQUERYTYPE_EVENT:
			d3dQueryType = D3DQUERYTYPE_EVENT;
			break;

		case ruGPUQUERYTYPE_OCCLUSION:
			d3dQueryType = D3DQUERYTYPE_OCCLUSION;
			break;
	}

	// Create query
	IDirect3DQuery9 *d3dQuery = NULL;
	hr = m_d3dDevice->CreateQuery(d3dQueryType, &d3dQuery);

	if(FAILED(hr))
	{
		return NULL;
	}
	return NULL;
}

/*!
	BOOL CRuEngineD3D::GetRenderTargetSize(UINT32 &widthOut, UINT32 &heightOut)
	Retrieves the current render target's (always retrieves RT0) dimensions.
	@param widthOut Width of the current render target
	@param heightOut Height of the current render target
	@return BOOL
	@retval TRUE Operation succeeded
	@retval FALSE Operation failed
*/
BOOL CRuEngineD3D::GetRenderTargetSize(UINT32 &widthOut, UINT32 &heightOut)
{
	if(m_d3dDevice == NULL)
		return FALSE;

	IDirect3DSurface9 *renderTarget;
	if(m_d3dDevice->GetRenderTarget(0, &renderTarget) == D3D_OK)
	{
		// Get surface description
		D3DSURFACE_DESC surfDesc;
		if(renderTarget->GetDesc(&surfDesc) == D3D_OK)
		{
			// Store dimensions
			widthOut = surfDesc.Width;
			heightOut = surfDesc.Height;

			// Release the render target interface
			renderTarget->Release();

			return TRUE;
		}

		// Release the render target interface
		renderTarget->Release();

		return FALSE;
	}

	return TRUE;
}

IRuSwapChain *CRuEngineD3D::GetPrimarySwapChain()
{
	return m_swapChains.Count() ? static_cast<CRuSwapChainD3D *>(m_swapChains[0]) : NULL;
}

DWORD CRuEngineD3D::GetAdapterVendorID()
{
	return m_engineSetting.AdapterInfo()->m_adapterIdentifier.VendorId;
}

DWORD CRuEngineD3D::GetAdapterDeviceID()
{
	return m_engineSetting.AdapterInfo()->m_adapterIdentifier.DeviceId;
}

BOOL CRuEngineD3D::SetCursor(INT32 xHotSpot, INT32 yHotSpot, IRuTexture *cursorTexture)
{
	if(m_d3dDevice == NULL)
		return FALSE;

	// TODO: If the specified cursor is NULL, reset cursor (not done)
	if(cursorTexture == NULL)
		return FALSE;

	// If the specified cursor is already set and the iteration has not changed, simply return
	if(cursorTexture == m_cursor_Texture && cursorTexture->GetIteration() == m_cursor_TextureIteration)
		return TRUE;

	if(cursorTexture->GetType() == CRuTextureD3D::Type())
	{
		CRuTextureD3D *d3dTexture = static_cast<CRuTextureD3D *>(cursorTexture);
		IDirect3DSurface9 *surface = static_cast<CRuSurfaceD3D *>(d3dTexture->GetLevelSurface(0))->GetIDirect3DSurface();

		// Store hot spot data
		m_cursor_xHotSpot = xHotSpot;
		m_cursor_yHotSpot = yHotSpot;

		// Store reference to cursor
		d3dTexture->AddRef();
		ruSAFE_RELEASE(m_cursor_Texture);
		m_cursor_Texture = d3dTexture;
		m_cursor_TextureIteration = m_cursor_Texture->GetIteration();

		// Apply cursor
		HRESULT result = m_d3dDevice->SetCursorProperties(m_cursor_xHotSpot, m_cursor_yHotSpot, surface);

		if(FAILED(result))
		{
			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEngineD3D::ShowCursor(BOOL showCursor)
{
	if(m_d3dDevice == NULL)
		return FALSE;

	m_d3dDevice->ShowCursor(showCursor);

	return TRUE;
}

BOOL CRuEngineD3D::SetCursorPosition(INT32 x, INT32 y)
{
	if(m_d3dDevice == NULL)
		return FALSE;

	m_d3dDevice->SetCursorPosition(x, y, 0);

	return TRUE;
}

INT32 CRuEngineD3D::Metric_GetNumRequestedStateChanges()
{
	return m_numRequestedStateChanges;
}

INT32 CRuEngineD3D::Metric_GetNumStateChanges()
{
	return m_numStateChanges;
}

void CRuEngineD3D::Metric_OutputAllocationReport()
{
}

INT32 CRuEngineD3D::Metric_GetDeviceBufferPoolFootprint()
{
	INT32 footprint =	m_staticVertexBufferPool.GetBufferPoolFootprint() +
						m_staticIndexBufferPool.GetBufferPoolFootprint() +
						m_dynamicVertexBufferPool.GetBufferPoolFootprint() +
						m_dynamicIndexBufferPool.GetBufferPoolFootprint();

	return footprint;
}

INT32 CRuEngineD3D::Metric_GetTexturePoolFootprint()
{
	INT32 footprint =	m_2dTexturePool.GetTexturePoolFootprint() +
						m_3dTexturePool.GetTexturePoolFootprint();

	return footprint;
}

/*!
	BOOL CRuEngineD3D::EnumerateAdapters()
	Enumerates available adapters and their associated display mode combinations
	@return BOOL
	@retval TRUE The enumeration succeeded
	@retval FALSE The enumeration failed
*/
BOOL CRuEngineD3D::EnumerateAdapters()
{
	CRuArrayList<D3DFORMAT> adapterFormatList;

	if(m_d3dObject == NULL)
	{
		MessageBoxA(NULL, "Direct3D interface does not exist.", "Error", MB_OK);
		return FALSE;
	}

	// Setup list of allowable adapter formats
	m_allowedAdapterFormatList.Add(D3DFMT_X8R8G8B8);
	m_allowedAdapterFormatList.Add(D3DFMT_X1R5G5B5);
	m_allowedAdapterFormatList.Add(D3DFMT_R5G6B5);
	m_allowedAdapterFormatList.Add(D3DFMT_A2R10G10B10);

	// Iterate through every adapter and enumerate each one
	RuAdapterInfoD3D *adapterInfo = NULL;
	UINT32 numAdapters = m_d3dObject->GetAdapterCount();

	for(UINT32 adapterOrdinal = 0; adapterOrdinal < numAdapters; adapterOrdinal++)
	{


		// Allocate new adapter info structure
		adapterInfo = ruNEW RuAdapterInfoD3D();
		if(adapterInfo == NULL)
			return NULL;

		// Setup adapter ordinal and identifier
		adapterInfo->m_adapterOrdinal = adapterOrdinal;
		m_d3dObject->GetAdapterIdentifier(adapterOrdinal, 0, &adapterInfo->m_adapterIdentifier);

		// Enumerate all display modes on this adapter
		adapterFormatList.Clear();
		for(INT32 i = 0; i < m_allowedAdapterFormatList.Count(); i++)
		{
			UINT32 numAdapterModes = m_d3dObject->GetAdapterModeCount(adapterOrdinal, m_allowedAdapterFormatList[i]);
			for(UINT32 mode = 0; mode < numAdapterModes; mode++)
			{
				D3DDISPLAYMODE displayMode;
				m_d3dObject->EnumAdapterModes(adapterOrdinal, m_allowedAdapterFormatList[i], mode, &displayMode);

				// If color channel or display dimensions do not satisfy requirements, skip
				if(	displayMode.Width < m_appMinFullscreenWidth ||
					displayMode.Height < m_appMinFullscreenHeight ||
					ColorChannelBits(displayMode.Format) < m_appMinColorChannelBits)
				{
					continue;
				}

				// Add display mode to the adapter's display mode list
				adapterInfo->m_displayModeList.Add(displayMode);

				// Add display mode's format to the list of all adapter formats
				if(!adapterFormatList.Contains(displayMode.Format))
					adapterFormatList.Add(displayMode.Format);
			}
		}
		// Sort display mode list
		qsort(adapterInfo->m_displayModeList.GetArray(), adapterInfo->m_displayModeList.Count(), sizeof(D3DDISPLAYMODE), SortModesCallback);

		// Get info for each device on this adapter
		if(!EnumerateDevices(adapterInfo, adapterFormatList))
		{
			delete adapterInfo;
			return FALSE;
		}

		// If at least one device on this adapter is available and satisfies requirements, add the adapterInfo to the list
		if(adapterInfo->m_deviceInfoList.Count() == 0)
			delete adapterInfo;
		else
			m_adapterInfoList.Add(adapterInfo);
	}

	return TRUE;
}

/*!
	BOOL CRuEngineD3D::SelectInitialDisplaySettings(BOOL fullscreen)
	Selects suitable initial display settings
	@param fullscreen Whether to operate in fullscreen mode
	@return BOOL
	@retval TRUE Selection succeeded
	@retval FALSE Selection failed
*/
BOOL CRuEngineD3D::SelectInitialDisplaySettings(BOOL fullscreen)
{
	BOOL foundFullscreen = m_engineSetting.FindBestFullscreenMode(m_d3dObject, m_adapterInfoList, FALSE, FALSE);
	BOOL foundWindowed = m_engineSetting.FindBestWindowedMode(m_d3dObject, m_adapterInfoList, FALSE, FALSE);

	if(fullscreen && foundFullscreen)
		m_engineSetting.m_isWindowed = FALSE;
	if(!foundWindowed && foundFullscreen)
		m_engineSetting.m_isWindowed = FALSE;

	if(!foundFullscreen && !foundWindowed)
		return FALSE;

	return TRUE;
}

INT32 CRuEngineD3D::GetNumAvailableDisplayModes()
{
	return m_engineSetting.m_fullscreenAdapterInfo->m_displayModeList.Count();
}

RuDisplayMode CRuEngineD3D::GetAvailableDisplayModeByIndex(INT32 idx)
{
	RuDisplayMode dispMode;

	dispMode.m_width = m_engineSetting.m_fullscreenAdapterInfo->m_displayModeList[idx].Width;
	dispMode.m_height = m_engineSetting.m_fullscreenAdapterInfo->m_displayModeList[idx].Height;
	dispMode.m_refreshRate = m_engineSetting.m_fullscreenAdapterInfo->m_displayModeList[idx].RefreshRate;
	dispMode.m_format = ToRuSurfaceFormat(m_engineSetting.m_fullscreenAdapterInfo->m_displayModeList[idx].Format);

	return dispMode;
}

void CRuEngineD3D::FlushStateChanges()
{
	RUPROFILE_SAMPLE_BEGIN(m_probe_StateChanges, 3);

	HRESULT result;

	// Flush render state changes
	for(INT32 i = 0; i < s_maxRenderStates; ++i)
	{
		if(m_renderStatesChanged[i])
		{
			// Set the render state
			result = m_d3dDevice->SetRenderState(static_cast<D3DRENDERSTATETYPE>(i), m_renderStates[i]);

			// Reset render state changes flag
			m_renderStatesChanged[i] = FALSE;
		}
	}

	// Flush texture and sampler state changes
	for(INT32 i = 0; i < s_maxStages; ++i)
	{
		for(INT32 j = 0; j < s_maxTextureStates; ++j)
		{
			if(m_textureStageStatesChanged[i][j])
			{
				// Set the state
				result = m_d3dDevice->SetTextureStageState(i, static_cast<D3DTEXTURESTAGESTATETYPE>(j), m_textureStageStates[i][j]);

				// Reset changed flag
				m_textureStageStatesChanged[i][j] = FALSE;
			}
		}

		for(INT32 j = 0; j < s_maxSamplerStates; ++j)
		{
			if(m_samplerStageStatesChanged[i][j])
			{
				// Set the state
				result = m_d3dDevice->SetSamplerState(i, static_cast<D3DSAMPLERSTATETYPE>(j), m_samplerStageStates[i][j]);

				// Reset changed flag
				m_samplerStageStatesChanged[i][j] = FALSE;
			}
		}
	}

	// Flush vertex shader constants
	if(m_vsConstants_Max - m_vsConstants_Min > 0)
	{
		result = m_d3dDevice->SetVertexShaderConstantF(m_vsConstants_Min, &m_vsConstants[m_vsConstants_Min * 4], m_vsConstants_Max - m_vsConstants_Min);
	}

	// Clear vertex shader dirty range
	m_vsConstants_Min = 256;
	m_vsConstants_Max = 0;

	// Flush pixel shader constants
	if(m_psConstants_Max - m_psConstants_Min > 0)
	{
		result = m_d3dDevice->SetPixelShaderConstantF(m_psConstants_Min, &m_psConstants[m_psConstants_Min * 4], m_psConstants_Max - m_psConstants_Min);
	}

	// Clear pixel shader dirty range
	m_psConstants_Min = 256;
	m_psConstants_Max = 0;

	RUPROFILE_SAMPLE_END(m_probe_StateChanges, 3);

	return;
}

BOOL CRuEngineD3D::UseDefaultSamplerFilterState(DWORD stage)
{
	// Use anisotropic minification filter
	this->SetSamplerState(stage, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);

	// Use linear magnification filter
	this->SetSamplerState(stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	// Use linear mip filter
	this->SetSamplerState(stage, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// Set maximum anisotropy
	this->SetSamplerState(stage, D3DSAMP_MAXANISOTROPY, 4);

	return TRUE;
}

HRESULT CRuEngineD3D::SetRenderTarget(INT32 renderTargetIndex, IDirect3DSurface9 *surface)
{
	if(m_d3dDevice == NULL)
		return D3DERR_INVALIDCALL;

	++m_numRequestedStateChanges;

	if(renderTargetIndex > s_maxRenderTargets || m_renderTargets[renderTargetIndex] != surface || m_renderTargetsOK[renderTargetIndex] == FALSE)
	{
		++m_numStateChanges;

		RUPROFILE_SAMPLE_BEGIN(m_probe_StateChanges, 3);

		HRESULT result = m_d3dDevice->SetRenderTarget(renderTargetIndex, surface);

		RUPROFILE_SAMPLE_END(m_probe_StateChanges, 3);

		if(FAILED(result))
			return result;

		if(renderTargetIndex < s_maxRenderTargets)
		{
			m_renderTargetsOK[renderTargetIndex] = TRUE;
			m_renderTargets[renderTargetIndex] = surface;
		}
	}

	return D3D_OK;
}

HRESULT CRuEngineD3D::SetDepthStencil(IDirect3DSurface9 *surface)
{
	if(m_d3dDevice == NULL)
		return D3DERR_INVALIDCALL;

	++m_numRequestedStateChanges;

	if(m_currentDepthStencil != surface || m_currentDepthStencilOK == FALSE)
	{
		++m_numStateChanges;

		RUPROFILE_SAMPLE_BEGIN(m_probe_StateChanges, 3);

		HRESULT result = m_d3dDevice->SetDepthStencilSurface(surface);

		RUPROFILE_SAMPLE_END(m_probe_StateChanges, 3);

		if(FAILED(result))
			return result;

		m_currentDepthStencilOK = TRUE;
		m_currentDepthStencil = surface;
	}

	return D3D_OK;
}

HRESULT CRuEngineD3D::SetViewport(D3DVIEWPORT9 *viewport)
{
	if(m_d3dDevice == NULL)
		return D3DERR_INVALIDCALL;

	++m_numRequestedStateChanges;

	if(m_currentViewport != viewport || m_currentViewportOK == FALSE)
	{
		++m_numStateChanges;

		RUPROFILE_SAMPLE_BEGIN(m_probe_StateChanges, 3);

		HRESULT result = m_d3dDevice->SetViewport(viewport);

		RUPROFILE_SAMPLE_END(m_probe_StateChanges, 3);

		if(FAILED(result))
			return result;

		m_currentViewportOK = TRUE;
		m_currentViewport = viewport;
	}

	return D3D_OK;
}

HRESULT CRuEngineD3D::SetRenderState(D3DRENDERSTATETYPE state, DWORD value)
{
	if(m_d3dDevice == NULL)
		return D3DERR_INVALIDCALL;

	++m_numRequestedStateChanges;

	if(state < s_maxRenderStates)
	{
		if(m_renderStates[state] != value || m_renderStatesOK[state] == FALSE)
		{
			++m_numStateChanges;

			m_renderStatesChanged[state] = TRUE;
			m_renderStatesOK[state] = TRUE;
			m_renderStates[state] = value;
		}
	}
	else
	{
//		MessageBox(NULL, "WARNING: Some render states are not being cached", "CDirect3DStateCache", MB_OK);

		RUPROFILE_SAMPLE_BEGIN(m_probe_StateChanges, 3);

		HRESULT result = m_d3dDevice->SetRenderState(state, value);

		RUPROFILE_SAMPLE_END(m_probe_StateChanges, 3);

		if(FAILED(result))
			return result;
	}

	return D3D_OK;
}

HRESULT CRuEngineD3D::SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE state, DWORD value)
{
	if(m_d3dDevice == NULL || stage >= s_maxStages)
		return D3DERR_INVALIDCALL;

	++m_numRequestedStateChanges;

	if(state < s_maxTextureStates)
	{
		if(m_textureStageStates[stage][state] != value || m_textureStageStatesOK[stage][state] == FALSE)
		{
			++m_numStateChanges;

			m_textureStageStatesChanged[stage][state] = TRUE;
			m_textureStageStatesOK[stage][state] = TRUE;
			m_textureStageStates[stage][state] = value;
		}
	}
	else
	{
//		MessageBox(NULL, "WARNING: Some texture states are not being cached", "CDirect3DStateCache", MB_OK);
		return D3DERR_INVALIDCALL;
	}

	return D3D_OK;
}

HRESULT CRuEngineD3D::SetSamplerState(DWORD stage, D3DSAMPLERSTATETYPE state, DWORD value)
{
	if(m_d3dDevice == NULL || stage >= s_maxStages)
		return D3DERR_INVALIDCALL;

	++m_numRequestedStateChanges;

	if(state < s_maxSamplerStates)
	{
		if(m_samplerStageStates[stage][state] != value || m_samplerStageStatesOK[stage][state] == FALSE)
		{
			++m_numStateChanges;

			m_samplerStageStatesChanged[stage][state] = TRUE;
			m_samplerStageStatesOK[stage][state] = TRUE;
			m_samplerStageStates[stage][state] = value;
		}
	}
	else
	{
//		MessageBox(NULL, "WARNING: Some sampler states are not being cached", "CDirect3DStateCache", MB_OK);
		return D3DERR_INVALIDCALL;
	}

	return D3D_OK;
}

HRESULT CRuEngineD3D::SetVertexDeclaration(IDirect3DVertexDeclaration9 *decl)
{
	if(m_d3dDevice == NULL)
		return D3DERR_INVALIDCALL;

	++m_numRequestedStateChanges;

	if(m_currentVertexDeclaration != decl || m_currentVertexDeclarationOK == FALSE)
	{
		++m_numStateChanges;

		RUPROFILE_SAMPLE_BEGIN(m_probe_StateChanges, 3);

		HRESULT result = m_d3dDevice->SetVertexDeclaration(decl);

		RUPROFILE_SAMPLE_END(m_probe_StateChanges, 3);

		if(FAILED(result))
			return result;

		m_currentVertexDeclarationOK = TRUE;
		m_currentVertexDeclaration = decl;
	}

	return D3D_OK;
}

HRESULT CRuEngineD3D::SetVertexShader(IDirect3DVertexShader9 *vs)
{
	if(m_d3dDevice == NULL)
		return D3DERR_INVALIDCALL;

	++m_numRequestedStateChanges;

	if(m_currentVertexShader != vs || m_currentVertexShaderOK == FALSE)
	{
		++m_numStateChanges;

		RUPROFILE_SAMPLE_BEGIN(m_probe_StateChanges, 3);

		HRESULT result = m_d3dDevice->SetVertexShader(vs);

		RUPROFILE_SAMPLE_END(m_probe_StateChanges, 3);

		if(FAILED(result))
			return result;

		m_currentVertexShaderOK = TRUE;
		m_currentVertexShader = vs;
	}

	return D3D_OK;
}

HRESULT CRuEngineD3D::SetPixelShader(IDirect3DPixelShader9 *ps)
{
	if(m_d3dDevice == NULL)
		return D3DERR_INVALIDCALL;

	++m_numRequestedStateChanges;

	if(m_currentPixelShader != ps || m_currentPixelShaderOK == FALSE)
	{
		++m_numStateChanges;

		RUPROFILE_SAMPLE_BEGIN(m_probe_StateChanges, 3);

		HRESULT result = m_d3dDevice->SetPixelShader(ps);

		RUPROFILE_SAMPLE_END(m_probe_StateChanges, 3);

		if(FAILED(result))
			return result;

		m_currentPixelShaderOK = TRUE;
		m_currentPixelShader = ps;
	}

	return D3D_OK;
}

HRESULT CRuEngineD3D::SetTexture(DWORD sampler, IDirect3DBaseTexture9 *texture)
{
	if(m_d3dDevice == NULL)
		return D3DERR_INVALIDCALL;

	++m_numRequestedStateChanges;

	if(m_currentTexture[sampler] != texture || m_currentTextureOK[sampler] == FALSE)
	{
		++m_numStateChanges;

		RUPROFILE_SAMPLE_BEGIN(m_probe_StateChanges, 3);

		HRESULT result = m_d3dDevice->SetTexture(sampler, texture);

		RUPROFILE_SAMPLE_END(m_probe_StateChanges, 3);

		if(FAILED(result))
			return result;

		m_currentTextureOK[sampler] = TRUE;
		m_currentTexture[sampler] = texture;
	}

	return D3D_OK;
}

HRESULT CRuEngineD3D::SetVertexShaderConstants(INT32 index, REAL *constants, INT32 numConstants)
{
	if(m_d3dDevice == NULL)
		return D3DERR_INVALIDCALL;

	++m_numVSConstantChanges;

	RUPROFILE_SAMPLE_BEGIN(m_probe_VSConstantChanges, 3);

	// Buffer the constant change
	RuMemcpy(&m_vsConstants[index * 4], constants, numConstants * 4 * sizeof(REAL));

	// Update dirty range
	m_vsConstants_Min = min(m_vsConstants_Min, index);
	m_vsConstants_Max = max(m_vsConstants_Max, index + numConstants);

	RUPROFILE_SAMPLE_END(m_probe_VSConstantChanges, 3);

	return D3D_OK;
}

HRESULT CRuEngineD3D::SetPixelShaderConstants(INT32 index, REAL *constants, INT32 numConstants)
{
	if(m_d3dDevice == NULL)
		return D3DERR_INVALIDCALL;

	++m_numPSConstantChanges;

	RUPROFILE_SAMPLE_BEGIN(m_probe_PSConstantChanges, 3);

	// Buffer the constant change
	RuMemcpy(&m_psConstants[index * 4], constants, numConstants * 4 * sizeof(REAL));

	// Update dirty range
	m_psConstants_Min = min(m_psConstants_Min, index);
	m_psConstants_Max = max(m_psConstants_Max, index + numConstants);

	RUPROFILE_SAMPLE_END(m_probe_PSConstantChanges, 3);

	return D3D_OK;
}

/*!
	BOOL CRuEngineD3D::EnumerateDevices(RuAdapterInfoD3D *adapterInfo, CRuArrayList<D3DFORMAT> &adapterFormatList)
	Enumerates available devices for the specified adapter and format list
	@return BOOL
	@retval TRUE The enumeration succeeded
	@retval FALSE The enumeration failed
*/
BOOL CRuEngineD3D::EnumerateDevices(RuAdapterInfoD3D *adapterInfo, CRuArrayList<D3DFORMAT> &adapterFormatList)
{
	const D3DDEVTYPE devTypeArray[] = { D3DDEVTYPE_HAL, D3DDEVTYPE_SW, D3DDEVTYPE_REF };
	const UINT32 devTypeArrayCount = sizeof(devTypeArray) / sizeof(devTypeArray[0]);

	RuDeviceInfoD3D *deviceInfo = NULL;
	for(UINT32 i = 0; i < devTypeArrayCount; i++)
	{
		// Allocate new device info structure
		deviceInfo = ruNEW RuDeviceInfoD3D();
		if(deviceInfo == NULL)
			return FALSE;

		// Setup device information and retrieve device caps
		deviceInfo->m_adapterOrdinal = adapterInfo->m_adapterOrdinal;
		deviceInfo->m_deviceType = devTypeArray[i];
		if(FAILED(m_d3dObject->GetDeviceCaps(adapterInfo->m_adapterOrdinal, deviceInfo->m_deviceType, &deviceInfo->m_caps)))
		{
			delete deviceInfo;
			continue;
		}

		// Get info for each device combination on this device
		if(FAILED(EnumerateDeviceCombos(deviceInfo, adapterFormatList)))
		{
			delete deviceInfo;
			return FALSE;
		}

		// If there are no compatible device combinations, skip
		if(deviceInfo->m_deviceComboList.Count() == 0)
		{
			delete deviceInfo;
			continue;
		}

		// Add the device info to the list
		adapterInfo->m_deviceInfoList.Add(deviceInfo);
	}

	return TRUE;
}

/*!
	BOOL CRuEngineD3D::EnumerateDeviceCombos(RuDeviceInfoD3D *deviceInfo, CRuArrayList<D3DFORMAT> &adapterFormatList)
	Enumerates available device combinations for the specified device and format list
	@return BOOL
	@retval TRUE The enumeration succeeded
	@retval FALSE The enumeration failed
*/
BOOL CRuEngineD3D::EnumerateDeviceCombos(RuDeviceInfoD3D *deviceInfo, CRuArrayList<D3DFORMAT> &adapterFormatList)
{
	const D3DFORMAT backBufferFormatArray[] =
	{
//		D3DFMT_A8R8G8B8,
		D3DFMT_X8R8G8B8,
		D3DFMT_A2R10G10B10,
		D3DFMT_R5G6B5,
		D3DFMT_A1R5G5B5,
		D3DFMT_X1R5G5B5
	};
	const UINT32 backBufferFormatArrayCount = sizeof(backBufferFormatArray) / sizeof(backBufferFormatArray[0]);
	const BOOL isWindowedArray[] = { FALSE, TRUE };

	// Iterate through each format supported by the adapter
	D3DFORMAT adapterFormat;
	for(INT32 iaf = 0; iaf < adapterFormatList.Count(); iaf++)
	{
		adapterFormat = adapterFormatList[iaf];
		D3DFORMAT backBufferFormat;
		for(INT32 ibbf = 0; ibbf < backBufferFormatArrayCount; ibbf++)
		{
			// Skip if the backbuffer does not support the required number of alpha bits
			backBufferFormat = backBufferFormatArray[ibbf];
			if(AlphaChannelBits(backBufferFormat) < m_appMinAlphaChannelBits)
				continue;

			// Iterate through windowed and fullscreen
			BOOL isWindowed;
			for(UINT32 iiw = 0; iiw < 2; iiw++)
			{
				// Skip if the mode does not match the current windowed mode setting
				isWindowed = isWindowedArray[iiw];
				if(!isWindowed && m_appRequiresWindowed)
					continue;
				if(isWindowed && m_appRequiresFullscreen)
					continue;

				// Ensure the formats are compatible
				if(FAILED(m_d3dObject->CheckDeviceType(deviceInfo->m_adapterOrdinal, deviceInfo->m_deviceType, adapterFormat, backBufferFormat, isWindowed)))
					continue;

				// Allocate new device combo structure
				RuDeviceComboD3D *deviceCombo = NULL;
				deviceCombo = ruNEW RuDeviceComboD3D();
				if(deviceCombo == NULL)
					return FALSE;

				// Setup device combo base information
				deviceCombo->m_adapterOrdinal = deviceInfo->m_adapterOrdinal;
				deviceCombo->m_deviceType = deviceInfo->m_deviceType;
				deviceCombo->m_adapterFormat = adapterFormat;
				deviceCombo->m_backBufferFormat = backBufferFormat;
				deviceCombo->m_isWindowed = isWindowed;

				// Build depth buffer list and verify compatibility
				if(m_appUsesDepthBuffer)
				{
					BuildDepthStencilFormatList(deviceCombo);
					if(deviceCombo->m_depthStencilFormatList.Count() == 0)
					{
						delete deviceCombo;
						continue;
					}
				}

				// Build multisampling type list and its conflicts list
				BuildMultisampleTypeList(deviceCombo);
				if(deviceCombo->m_multisampleTypeList.Count() == 0)
				{
					delete deviceCombo;
					continue;
				}
				BuildMultisampleConflictList(deviceCombo);

				// Build engine mode list
				BuildEngineModeList(deviceInfo, deviceCombo);
				if(deviceCombo->m_engineModeList.Count() == 0)
				{
					delete deviceCombo;
					continue;
				}

				// Build presentation interval list
				BuildPresentIntervalList(deviceInfo, deviceCombo);

				// Add device combo to the list
				deviceInfo->m_deviceComboList.Add(deviceCombo);
			}
		}
	}

	return TRUE;
}

/*!
	void CRuEngineD3D::BuildDepthStencilFormatList(RuDeviceComboD3D *deviceCombo)
	Builds the depth/stencil buffer format list for the specified device combo
	@param deviceCombo Device combo to operate on
	@return void
*/
void CRuEngineD3D::BuildDepthStencilFormatList(RuDeviceComboD3D *deviceCombo)
{
	const D3DFORMAT depthStencilFormatArray[] = 
	{
		D3DFMT_D16,
		D3DFMT_D15S1,
		D3DFMT_D24X8,
		D3DFMT_D24S8,
		D3DFMT_D24X4S4,
		D3DFMT_D32,
	};
	const UINT32 depthStencilFormatArrayCount = sizeof(depthStencilFormatArray) / sizeof(depthStencilFormatArray[0]);

	D3DFORMAT depthStencilFormat;
	for(UINT32 idsf = 0; idsf < depthStencilFormatArrayCount; idsf++)
	{
		// Skip if the depth/stencil buffer is not compatible
		depthStencilFormat = depthStencilFormatArray[idsf];
		if(DepthBits(depthStencilFormat) < m_appMinDepthBits)
			continue;
		if(StencilBits(depthStencilFormat) < m_appMinStencilBits)
			continue;

		// Ensure the format combination is actually supported by the device
		if(SUCCEEDED(m_d3dObject->CheckDeviceFormat(deviceCombo->m_adapterOrdinal, deviceCombo->m_deviceType, deviceCombo->m_adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, depthStencilFormat)))
		{
			if(SUCCEEDED(m_d3dObject->CheckDepthStencilMatch(deviceCombo->m_adapterOrdinal, deviceCombo->m_deviceType, deviceCombo->m_adapterFormat, deviceCombo->m_backBufferFormat, depthStencilFormat)))
			{
				deviceCombo->m_depthStencilFormatList.Add(depthStencilFormat);
			}
		}
	}
}

/*!
	void CRuEngineD3D::BuildMultisampleTypeList(RuDeviceComboD3D *deviceCombo)
	Builds the multisample type list for the specified device combo
	@param deviceCombo Device combo to operate on
	@return void
*/
void CRuEngineD3D::BuildMultisampleTypeList(RuDeviceComboD3D *deviceCombo)
{
	const D3DMULTISAMPLE_TYPE msTypeArray[] =
	{
		D3DMULTISAMPLE_NONE,
		D3DMULTISAMPLE_NONMASKABLE,
		D3DMULTISAMPLE_2_SAMPLES,
		D3DMULTISAMPLE_3_SAMPLES,
		D3DMULTISAMPLE_4_SAMPLES,
		D3DMULTISAMPLE_5_SAMPLES,
		D3DMULTISAMPLE_6_SAMPLES,
		D3DMULTISAMPLE_7_SAMPLES,
		D3DMULTISAMPLE_8_SAMPLES,
		D3DMULTISAMPLE_9_SAMPLES,
		D3DMULTISAMPLE_10_SAMPLES,
		D3DMULTISAMPLE_11_SAMPLES,
		D3DMULTISAMPLE_12_SAMPLES,
		D3DMULTISAMPLE_13_SAMPLES,
		D3DMULTISAMPLE_14_SAMPLES,
		D3DMULTISAMPLE_15_SAMPLES,
		D3DMULTISAMPLE_16_SAMPLES,
	};
	const UINT32 msTypeArrayCount = sizeof(msTypeArray) / sizeof(msTypeArray[0]);

	D3DMULTISAMPLE_TYPE msType;
	DWORD msQuality;
	for(UINT32 imst = 0; imst < msTypeArrayCount; imst++)
	{
		msType = msTypeArray[imst];
		if(SUCCEEDED(m_d3dObject->CheckDeviceMultiSampleType(deviceCombo->m_adapterOrdinal, deviceCombo->m_deviceType, deviceCombo->m_backBufferFormat, deviceCombo->m_isWindowed, msType, &msQuality)))
		{
			deviceCombo->m_multisampleTypeList.Add(msType);
			deviceCombo->m_multisampleQualityList.Add(msQuality);
		}
	}
}

/*!
	void CRuEngineD3D::BuildDSMSConflictList(RuDeviceComboD3D *deviceCombo)
	Builds the multisample conflict list for the specified device combo
	@param deviceCombo Device combo to operate on
	@return void
*/
void CRuEngineD3D::BuildMultisampleConflictList(RuDeviceComboD3D *deviceCombo)
{
	RuMultisampleConflictD3D msConflict;

	for(INT32 ids = 0; ids < deviceCombo->m_depthStencilFormatList.Count(); ids++)
	{
		D3DFORMAT dsFmt = deviceCombo->m_depthStencilFormatList[ids];
		for(INT32 ims = 0; ims < deviceCombo->m_multisampleTypeList.Count(); ims++)
		{
			D3DMULTISAMPLE_TYPE msType = deviceCombo->m_multisampleTypeList[ims];
			if(FAILED(m_d3dObject->CheckDeviceMultiSampleType(deviceCombo->m_adapterOrdinal, deviceCombo->m_deviceType, dsFmt, deviceCombo->m_isWindowed, msType, NULL)))
			{
				msConflict.m_depthStencilFormat = dsFmt;
				msConflict.m_multisampleType = msType;
				deviceCombo->m_multisampleConflictList.Add(msConflict);
			}
		}
	}
}

/*!
	void CRuEngineD3D::BuildEngineModeList(RuDeviceInfoD3D *deviceInfo, RuDeviceComboD3D *deviceCombo)
	Builds the engine mode list for the specified device combo
	@param deviceCombo Device combo to operate on
	@return void
*/
void CRuEngineD3D::BuildEngineModeList(RuDeviceInfoD3D *deviceInfo, RuDeviceComboD3D *deviceCombo)
{
	deviceCombo->m_engineModeList.Add(ruENGINEMODE_PURE);
}

/*!
	void CRuEngineD3D::BuildPresentIntervalList(RuDeviceInfoD3D *deviceInfo, RuDeviceComboD3D *deviceCombo)
	Builds the presentation interval list for the specified device combo
	@param deviceCombo Device combo to operate on
	@return void
*/
void CRuEngineD3D::BuildPresentIntervalList(RuDeviceInfoD3D *deviceInfo, RuDeviceComboD3D *deviceCombo)
{
	const UINT32 piArray[] =
	{
		D3DPRESENT_INTERVAL_IMMEDIATE,
		D3DPRESENT_INTERVAL_DEFAULT,
		D3DPRESENT_INTERVAL_ONE,
		D3DPRESENT_INTERVAL_TWO,
		D3DPRESENT_INTERVAL_THREE,
		D3DPRESENT_INTERVAL_FOUR,
	};
	const UINT32 piArrayCount = sizeof(piArray) / sizeof(piArray[0]);

	UINT32 pi;
	for(UINT32 ipi = 0; ipi < piArrayCount; ipi++)
	{
		pi = piArray[ipi];
		if(deviceCombo->m_isWindowed)
		{
			// These intervals are not supported in windowed mode.
			if(pi == D3DPRESENT_INTERVAL_TWO || pi == D3DPRESENT_INTERVAL_THREE || pi == D3DPRESENT_INTERVAL_FOUR)
			{
				continue;
			}
		}

		// Note that D3DPRESENT_INTERVAL_DEFAULT is zero, so you can't do a caps check for it -- it is always available.
		if(pi == D3DPRESENT_INTERVAL_DEFAULT || (deviceInfo->m_caps.PresentationIntervals & pi))
		{
			deviceCombo->m_presentIntervalList.Add(pi);
		}
	}
}

IRuDeviceBuffer *CRuEngineD3D::CreateVertexBuffer_Internal(INT32 size)
{
	CRuVertexBufferD3D *vertexBuffer = NULL;
	IDirect3DVertexBuffer9 *vb = NULL;
//	HRESULT result = m_d3dDevice->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &vb, NULL);
	HRESULT result = m_d3dDevice->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &vb, NULL);

	if(result == D3D_OK)
	{
		vertexBuffer = ruNEW CRuVertexBufferD3D(vb, FALSE);
		vertexBuffer->SetEngine(this);
		ruSAFE_RELEASE(vb);
	}

	return vertexBuffer;
}

IRuDeviceBuffer *CRuEngineD3D::CreateDynamicVertexBuffer_Internal(INT32 size)
{
	CRuVertexBufferD3D *vertexBuffer = NULL;
	IDirect3DVertexBuffer9 *vb = NULL;
	HRESULT result = m_d3dDevice->CreateVertexBuffer(size, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &vb, NULL);

	if(result == D3D_OK)
	{
		vertexBuffer = ruNEW CRuVertexBufferD3D(vb, TRUE);
		vertexBuffer->SetEngine(this);
		ruSAFE_RELEASE(vb);
	}

	return vertexBuffer;
}

IRuDeviceBuffer *CRuEngineD3D::CreateIndexBuffer_Internal(INT32 size)
{
	CRuIndexBufferD3D *indexBuffer = NULL;
	IDirect3DIndexBuffer9 *ib = NULL;
//	HRESULT result = m_d3dDevice->CreateIndexBuffer(size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &ib, NULL);
	HRESULT result = m_d3dDevice->CreateIndexBuffer(size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &ib, NULL);

	if(result == D3D_OK)
	{
		indexBuffer = ruNEW CRuIndexBufferD3D(ib, FALSE);
		indexBuffer->SetEngine(this);
		ruSAFE_RELEASE(ib);
	}

	return indexBuffer;
}

IRuDeviceBuffer *CRuEngineD3D::CreateDynamicIndexBuffer_Internal(INT32 size)
{
	CRuIndexBufferD3D *indexBuffer = NULL;
	IDirect3DIndexBuffer9 *ib = NULL;
	HRESULT result = m_d3dDevice->CreateIndexBuffer(size, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &ib, NULL);

	if(result == D3D_OK)
	{
		indexBuffer = ruNEW CRuIndexBufferD3D(ib, TRUE);
		indexBuffer->SetEngine(this);
		ruSAFE_RELEASE(ib);
	}

	return indexBuffer;
}

IRuBaseTexture *CRuEngineD3D::CreateRenderTarget_Internal(INT32 dim0, INT32 dim1, INT32 dim2, INT32 levelCount, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool)
{
	IDirect3DTexture9 *d3dTexture = NULL;

	if(m_d3dDevice->CreateTexture(dim0, dim1, levelCount, D3DUSAGE_RENDERTARGET, (D3DFORMAT) surfaceFormat, D3DPOOL_DEFAULT, &d3dTexture, NULL) == D3D_OK)
	{
		CRuTextureD3D *texture = ruNEW CRuTextureD3D(d3dTexture, FALSE);
		texture->SetEngine(this);

		return texture;
	}

	return NULL;
}

IRuBaseTexture *CRuEngineD3D::CreateTexture_Internal(INT32 dim0, INT32 dim1, INT32 dim2, INT32 levelCount, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool)
{
	IDirect3DTexture9 *d3dTexture = NULL;

	if(m_d3dDevice->CreateTexture(dim0, dim1, levelCount, 0, (D3DFORMAT) surfaceFormat, (D3DPOOL) resourcePool, &d3dTexture, NULL) == D3D_OK)
	{
		CRuTextureD3D *texture = ruNEW CRuTextureD3D(d3dTexture, FALSE);
		texture->SetEngine(this);

		return texture;
	}

	return NULL;
}

IRuBaseTexture *CRuEngineD3D::CreateVolumeTexture_Internal(INT32 dim0, INT32 dim1, INT32 dim2, INT32 levelCount, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool)
{
	IDirect3DVolumeTexture9 *d3dTexture = NULL;

	if(m_d3dDevice->CreateVolumeTexture(dim0, dim1, dim2, levelCount, 0, (D3DFORMAT) surfaceFormat, (D3DPOOL) resourcePool, &d3dTexture, NULL) == D3D_OK)
	{
		CRuVolumeTextureD3D *texture = ruNEW CRuVolumeTextureD3D(d3dTexture, FALSE);
		texture->SetEngine(this);

		return texture;
	}

	return NULL;
}

void CRuEngineD3D::ClearVertexDeclarationCache()
{
//	m_vertexDeclarations->ForAllElements(boost::bind(&CRuEngineD3D::ClearVertexDeclarationCacheCallback, this, _1, _2));
//	m_vertexDeclarations->Clear();
}

BOOL CRuEngineD3D::ClearVertexDeclarationCacheCallback(const void *key, void *data)
{
	const RuVEC *vec = reinterpret_cast<const RuVEC *>(key);
	CRuVertexDeclarationD3D *vertexDecl = *reinterpret_cast<CRuVertexDeclarationD3D **>(data);

	delete[] vec->m_vertexElements;
	ruSAFE_RELEASE(vertexDecl);

	return TRUE;
}

/*!
	UINT32 CRuEngineD3D::ColorChannelBits(D3DFORMAT format)
	Determines the number of bits per color channel as specified by D3DFORMAT
	@param format Surface format
	@return UINT32 The number of bits per color channel as specified by D3DFORMAT
*/
UINT32 CRuEngineD3D::ColorChannelBits(D3DFORMAT format)
{
	switch(format)
	{
		case D3DFMT_R8G8B8:
			return 8;
		case D3DFMT_A8R8G8B8:
			return 8;
		case D3DFMT_X8R8G8B8:
			return 8;
		case D3DFMT_R5G6B5:
			return 5;
		case D3DFMT_X1R5G5B5:
			return 5;
		case D3DFMT_A1R5G5B5:
			return 5;
		case D3DFMT_A4R4G4B4:
			return 4;
		case D3DFMT_R3G3B2:
			return 2;
		case D3DFMT_A8R3G3B2:
			return 2;
		case D3DFMT_X4R4G4B4:
			return 4;
		case D3DFMT_A2B10G10R10:
			return 10;
		case D3DFMT_A2R10G10B10:
			return 10;
		default:
			return 0;
	}
}

/*!
	UINT32 CRuEngineD3D::AlphaChannelBits(D3DFORMAT format)
	Determines the number of bits in the alpha channel as specified by D3DFORMAT
	@param format Surface format
	@return UINT32 The number of bits in the alpha channel as specified by D3DFORMAT
*/
UINT32 CRuEngineD3D::AlphaChannelBits(D3DFORMAT format)
{
    switch(format)
    {
        case D3DFMT_R8G8B8:
            return 0;
        case D3DFMT_A8R8G8B8:
            return 8;
        case D3DFMT_X8R8G8B8:
            return 0;
        case D3DFMT_R5G6B5:
            return 0;
        case D3DFMT_X1R5G5B5:
            return 0;
        case D3DFMT_A1R5G5B5:
            return 1;
        case D3DFMT_A4R4G4B4:
            return 4;
        case D3DFMT_R3G3B2:
            return 0;
        case D3DFMT_A8R3G3B2:
            return 8;
        case D3DFMT_X4R4G4B4:
            return 0;
        case D3DFMT_A2B10G10R10:
            return 2;
        case D3DFMT_A2R10G10B10:
            return 2;
        default:
            return 0;
    }
}

/*!
	UINT32 CRuEngineD3D::DepthBits(D3DFORMAT format)
	Determines the number of bits in the depth buffer as specified by D3DFORMAT
	@param format Surface format
	@return UINT32 The number of bits in the depth buffer as specified by D3DFORMAT
*/
UINT32 CRuEngineD3D::DepthBits(D3DFORMAT format)
{
	switch(format)
	{
		case D3DFMT_D16:
			return 16;
		case D3DFMT_D15S1:
			return 15;
		case D3DFMT_D24X8:
			return 24;
		case D3DFMT_D24S8:
			return 24;
		case D3DFMT_D24X4S4:
			return 24;
		case D3DFMT_D32:
			return 32;
		default:
			return 0;
	}
}

/*!
	UINT32 CRuEngineD3D::StencilBits(D3DFORMAT format)
	Determines the number of bits in the stencil buffer as specified by D3DFORMAT
	@param format Surface format
	@return UINT32 The number of bits in the stencil buffer as specified by D3DFORMAT
*/
UINT32 CRuEngineD3D::StencilBits(D3DFORMAT format)
{
	switch(format)
	{
		case D3DFMT_D16:
			return 0;
		case D3DFMT_D15S1:
			return 1;
		case D3DFMT_D24X8:
			return 0;
		case D3DFMT_D24S8:
			return 8;
		case D3DFMT_D24X4S4:
			return 4;
		case D3DFMT_D32:
			return 0;
		default:
			return 0;
	}
}

/*!
	int CRuEngineD3D::SortModesCallback(const void *arg1, const void *arg2)
	Comparison callback used for sorting display modes with qsort()
	@param arg1 Element 1
	@param arg2 Element 2
	@return int Comparison result
*/
int CRuEngineD3D::SortModesCallback(const void *arg1, const void *arg2)
{
	D3DDISPLAYMODE *pdm1 = (D3DDISPLAYMODE *) arg1;
	D3DDISPLAYMODE *pdm2 = (D3DDISPLAYMODE *) arg2;

	if(pdm1->Width > pdm2->Width)
		return 1;

	if(pdm1->Width < pdm2->Width)
		return -1;

	if(pdm1->Height > pdm2->Height)
		return 1;

	if(pdm1->Height < pdm2->Height)
		return -1;

	if(pdm1->Format > pdm2->Format)
		return 1;

	if(pdm1->Format < pdm2->Format)
		return -1;

	if(pdm1->RefreshRate > pdm2->RefreshRate)
		return 1;

	if(pdm1->RefreshRate < pdm2->RefreshRate)
		return -1;

	return 0;
}

// ************************************************************************************************************************************************************

CRuEngineD3D*							g_ruEngineD3D = NULL;

// ************************************************************************************************************************************************************

#pragma managed(pop)
