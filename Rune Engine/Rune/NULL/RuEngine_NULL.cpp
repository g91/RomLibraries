#include "../NULL/RuEngine_NULL.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuVertexDeclaration_NULL, IRuVertexDeclaration)
ruHEAP_DEFINE_SUBCLASS(CRuVertexDeclaration_NULL, "CRuVertexDeclaration_NULL", "IRuVertexDeclaration")
ruCLASSGUID_DEFINE(CRuVertexDeclaration_NULL, 0xEAA0A22F, 0xBED44536, 0x8EB60B4E, 0xCD87A957)

ruRTTI_DEFINE_SUBCLASS(CRuVertexBuffer_NULL, IRuVertexBuffer)
ruHEAP_DEFINE_SUBCLASS(CRuVertexBuffer_NULL, "CRuVertexBuffer_NULL", "IRuVertexBuffer")
ruCLASSGUID_DEFINE(CRuVertexBuffer_NULL, 0x6498CAB9, 0x45994eb3, 0x867C97BD, 0x1C1C9CCA)

ruRTTI_DEFINE_SUBCLASS(CRuIndexBuffer_NULL, IRuIndexBuffer)
ruHEAP_DEFINE_SUBCLASS(CRuIndexBuffer_NULL, "CRuIndexBuffer_NULL", "IRuIndexBuffer")
ruCLASSGUID_DEFINE(CRuIndexBuffer_NULL, 0x7D79218A, 0x7BDC498c, 0x8165CB43, 0xE22378C2)

ruRTTI_DEFINE_SUBCLASS(CRuViewport_NULL, IRuViewport)
ruHEAP_DEFINE_SUBCLASS(CRuViewport_NULL, "CRuViewport_NULL", "IRuViewport")
ruCLASSGUID_DEFINE(CRuViewport_NULL, 0xA21AB046, 0x30514413, 0xB7D1D16D, 0x02824823)

ruRTTI_DEFINE_SUBCLASS(CRuTexture_NULL, IRuTexture)
ruHEAP_DEFINE_SUBCLASS(CRuTexture_NULL, "CRuTexture_NULL", "IRuTexture")
ruCLASSGUID_DEFINE(CRuTexture_NULL, 0xB8A37A6D, 0x88234da8, 0x9BE24494, 0x76166FE4)

ruRTTI_DEFINE_SUBCLASS(CRuSwapChain_NULL, IRuSwapChain)
ruHEAP_DEFINE_SUBCLASS(CRuSwapChain_NULL, "CRuSwapChain_NULL", "IRuSwapChain")
ruCLASSGUID_DEFINE(CRuSwapChain_NULL, 0xEAED0EFB, 0xFA524c9c, 0xBD03FFFE, 0x90B9D875)

ruRTTI_DEFINE_SUBCLASS(CRuEngine_NULL, IRuEngine)
ruHEAP_DEFINE_SUBCLASS(CRuEngine_NULL, "CRuEngine_NULL", "IRuEngine")
ruCLASSGUID_DEFINE(CRuEngine_NULL, 0x90C04445, 0x67B345ee, 0xB795CC4D, 0xB5EE1D62)

// ************************************************************************************************************************************************************

CRuVertexDeclaration_NULL::CRuVertexDeclaration_NULL()
{
}

CRuVertexDeclaration_NULL::~CRuVertexDeclaration_NULL()
{
}

void CRuVertexDeclaration_NULL::Invalidate()
{
}

BOOL CRuVertexDeclaration_NULL::IsInvalidated()
{
	return FALSE;
}

BOOL CRuVertexDeclaration_NULL::Restore()
{
	return TRUE;
}

const CRuVertexElement *CRuVertexDeclaration_NULL::GetVertexElements() const
{
	return NULL;
}

void CRuVertexDeclaration_NULL::Select() const
{
}

INT32 CRuVertexDeclaration_NULL::GetNumStreams() const
{
	return 0;
}

INT32 CRuVertexDeclaration_NULL::GetStreamStride(INT32 streamIndex) const
{
	return 0;
}

// ************************************************************************************************************************************************************

CRuVertexBuffer_NULL::CRuVertexBuffer_NULL()
{
}

CRuVertexBuffer_NULL::~CRuVertexBuffer_NULL()
{
}

void *CRuVertexBuffer_NULL::Lock(UINT offset, UINT size)
{
	return NULL;
}

void *CRuVertexBuffer_NULL::Lock(UINT offset, UINT size, DWORD lockFlags)
{
	return NULL;
}

void CRuVertexBuffer_NULL::Unlock()
{
}

BOOL CRuVertexBuffer_NULL::Subdivide(INT32 numChildVBs, IRuVertexBuffer **childVBOut)
{
	return FALSE;
}

// ************************************************************************************************************************************************************

CRuIndexBuffer_NULL::CRuIndexBuffer_NULL()
{
}

CRuIndexBuffer_NULL::~CRuIndexBuffer_NULL()
{
}

void *CRuIndexBuffer_NULL::Lock(UINT offset, UINT size)
{
	return NULL;
}

void *CRuIndexBuffer_NULL::Lock(UINT offset, UINT size, DWORD lockFlags)
{
	return NULL;
}

void CRuIndexBuffer_NULL::Unlock()
{
}

// ************************************************************************************************************************************************************

CRuViewport_NULL::CRuViewport_NULL()
{
	m_clearFlags = 0;
	m_clearColor = 0x00000000;
	m_clearZ = 1.0f;
	m_clearStencil = 0;
}

CRuViewport_NULL::~CRuViewport_NULL()
{
}

void CRuViewport_NULL::Invalidate()
{
}

BOOL CRuViewport_NULL::IsInvalidated()
{
	return FALSE;
}

BOOL CRuViewport_NULL::Restore()
{
	return TRUE;
}

INT32 CRuViewport_NULL::GetViewportWidth()
{
	return 0;
}

INT32 CRuViewport_NULL::GetViewportHeight()
{
	return 0;
}

void CRuViewport_NULL::SetViewportParameters(INT32 x, INT32 y, INT32 width, INT32 height, REAL minZ, REAL maxZ)
{
}

DWORD CRuViewport_NULL::GetClearFlags()
{
	return m_clearFlags;
}

void CRuViewport_NULL::SetClearFlags(DWORD clearFlags)
{
	m_clearFlags = clearFlags;
}

RUCOLOR CRuViewport_NULL::GetClearColor()
{
	return m_clearColor;
}

void CRuViewport_NULL::SetClearColor(RUCOLOR color)
{
	m_clearColor = color;
}

REAL CRuViewport_NULL::GetClearZ()
{
	return m_clearZ;
}

void CRuViewport_NULL::SetClearZ(REAL z)
{
	m_clearZ = z;
}

DWORD CRuViewport_NULL::GetClearStencil()
{
	return m_clearStencil;
}

void CRuViewport_NULL::SetClearStencil(DWORD stencil)
{
	m_clearStencil = stencil;
}

BOOL CRuViewport_NULL::SelectAsViewport()
{
	return FALSE;
}

// ************************************************************************************************************************************************************

CRuTexture_NULL::CRuTexture_NULL()
{
}

CRuTexture_NULL::~CRuTexture_NULL()
{
}

void CRuTexture_NULL::Invalidate()
{
}

BOOL CRuTexture_NULL::IsInvalidated()
{
	return FALSE;
}

BOOL CRuTexture_NULL::Restore()
{
	return FALSE;
}

const char *CRuTexture_NULL::GetTextureName()
{
	return "";
}

void CRuTexture_NULL::SetTextureName(const char *textureName)
{
}

void CRuTexture_NULL::Select(UINT32 samplerIndex)
{
}

BOOL CRuTexture_NULL::GenerateMipLevels()
{
	return FALSE;
}

BOOL CRuTexture_NULL::IsDynamic()
{
	return FALSE;
}

BOOL CRuTexture_NULL::IsRenderTarget()
{
	return FALSE;
}

UINT32 CRuTexture_NULL::GetLevelCount()
{
	return 0;
}

UINT32 CRuTexture_NULL::GetLOD()
{
	return 0;
}

UINT32 CRuTexture_NULL::SetLOD(UINT32 lod)
{
	return 0;
}

RuResourcePool CRuTexture_NULL::GetPool()
{
	return ruRESPOOL_SCRATCH;
}

RuSurfaceFormat CRuTexture_NULL::GetSurfaceFormat()
{
	return ruSURFACEFMT_A8R8G8B8;
}

BOOL CRuTexture_NULL::SelectAsRenderTarget(INT32 rtIndex, UINT32 level)
{
	return FALSE;
}

UINT32 CRuTexture_NULL::GetLevelWidth(UINT32 level)
{
	return 0;
}

UINT32 CRuTexture_NULL::GetLevelHeight(UINT32 level)
{
	return 0;
}

IRuSurface *CRuTexture_NULL::GetLevelSurface(UINT32 level)
{
	return NULL;
}

BOOL CRuTexture_NULL::Lock(UINT32 level, void **bitsOut, INT32 *pitchOut)
{
	return FALSE;
}

BOOL CRuTexture_NULL::Unlock(UINT32 level)
{
	return FALSE;
}

// ************************************************************************************************************************************************************

CRuSwapChain_NULL::CRuSwapChain_NULL()
{
}

CRuSwapChain_NULL::~CRuSwapChain_NULL()
{
}

void CRuSwapChain_NULL::Invalidate()
{
}

BOOL CRuSwapChain_NULL::IsInvalidated()
{
	return FALSE;
}

BOOL CRuSwapChain_NULL::Restore()
{
	return FALSE;
}

BOOL CRuSwapChain_NULL::SelectAsRenderTarget(INT32 rtIndex)
{
	return FALSE;
}

UINT32 CRuSwapChain_NULL::GetWidth() const
{
	return 0;
}

UINT32 CRuSwapChain_NULL::GetHeight() const
{
	return 0;
}

BOOL CRuSwapChain_NULL::Present()
{
	return FALSE;
}

BOOL CRuSwapChain_NULL::SaveToFile(const char *fileName, RuImageFileFormat imageFormat)
{
	return FALSE;
}

// ************************************************************************************************************************************************************

/*!
	CRuEngine_NULL::CRuEngine_NULL()
	Standard constructor
*/
CRuEngine_NULL::CRuEngine_NULL()
{
}

/*!
	CRuEngine_NULL::~CRuEngine_NULL()
	Standard destructor
*/
CRuEngine_NULL::~CRuEngine_NULL()
{
}

void CRuEngine_NULL::ClearStateCache()
{
}

void CRuEngine_NULL::ClearSampler(DWORD sampler)
{
}

BOOL CRuEngine_NULL::BeginScene()
{
	return TRUE;
}

BOOL CRuEngine_NULL::EndScene()
{
	return TRUE;
}

BOOL CRuEngine_NULL::Clear(DWORD clearFlags, RUCOLOR color, float z, DWORD stencil)
{
	return TRUE;
}

BOOL CRuEngine_NULL::ResetViewport()
{
	return TRUE;
}

BOOL CRuEngine_NULL::RestoreDefaultDepthStencil()
{
	return TRUE;
}

BOOL CRuEngine_NULL::SetScissorRect(RECT *scissorRect)
{
	return TRUE;
}

BOOL CRuEngine_NULL::SetClipPlane(INT32 index, const CRuPlane4 &plane)
{
	return TRUE;
}

BOOL CRuEngine_NULL::SetClipPlaneEnable(INT32 index, BOOL enable)
{
	return TRUE;
}

IRuVertexStreamLayout *CRuEngine_NULL::CreateVertexStreamLayout(const CRuVertexElement *vertexElements, INT32 streamIdx)
{
	return NULL;
}

/*!
	IRuVertexDeclaration *CRuEngine_NULL::CreateVertexDeclaration(const CRuVertexElement *vertexElements)
	Creates a vertex declaration
	@param vertexElements
	@return IRuVertexDeclaration*
	@retval NULL Could not create the vertex declaration
*/
IRuVertexDeclaration *CRuEngine_NULL::CreateVertexDeclaration(const CRuVertexElement *vertexElements)
{
	return NULL;
}

/*!
	IRuVertexBuffer *CRuEngine_NULL::CreateVertexBuffer(UINT32 size)
	Creates a static vertex buffer
	@param size The size of the vertex buffer
	@return IRuVertexBuffer*
	@retval NULL Could not create the vertex buffer
*/
IRuVertexBuffer *CRuEngine_NULL::CreateVertexBuffer(UINT32 size)
{
	// Cannot satisfy request, return NULL
	return NULL;
}

/*!
	IRuVertexBuffer *CRuEngine_NULL::CreateDynamicVertexBuffer(UINT32 size)
	Creates a dynamic vertex buffer
	@param size The size of the vertex buffer
	@return IRuVertexBuffer*
	@retval NULL Could not create the vertex buffer
*/
IRuVertexBuffer *CRuEngine_NULL::CreateDynamicVertexBuffer(UINT32 size)
{
	// Cannot satisfy request, return NULL
	return NULL;
}

/*!
	IRuIndexBuffer *CRuEngine_NULL::CreateIndexBuffer(UINT32 size)
	Creates a static index buffer
	@param size The size of the index buffer
	@return IRuIndexBuffer*
	@retval NULL Could not create the index buffer
*/
IRuIndexBuffer *CRuEngine_NULL::CreateIndexBuffer(UINT32 size)
{
	// Cannot satisfy request, return NULL
	return NULL;
}

/*!
	IRuIndexBuffer *CRuEngine_NULL::CreateDynamicIndexBuffer(UINT32 size)
	Creates a dynamic index buffer
	@param size The size of the index buffer
	@return IRuIndexBuffer*
	@retval NULL Could not create the index buffer
*/
IRuIndexBuffer *CRuEngine_NULL::CreateDynamicIndexBuffer(UINT32 size)
{
	// Cannot satisfy request, return NULL
	return NULL;
}

/*!
	IRuViewport *CRuEngine_NULL::CreateViewport(INT32 x, INT32 y, INT32 width, INT32 height)
	Creates a viewport
	@param x
	@param y
	@param width
	@param height
	@return IRuViewport*
	@retval NULL Could not create the viewport
*/
IRuViewport *CRuEngine_NULL::CreateViewport(INT32 x, INT32 y, INT32 width, INT32 height)
{
	return NULL;
}

/*!
	IRuTexture *CRuEngine_NULL::CreateTexture(UINT32 width, UINT32 height, UINT32 mipLevels, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool)
	Creates a 1D or 2D texture
	@param width
	@param height
	@param mipLevels
	@param surfaceFormat
	@param resourcePool
	@return IRuTexture*
	@retval NULL Could not create the texture
*/
IRuTexture *CRuEngine_NULL::CreateTexture(UINT32 width, UINT32 height, UINT32 mipLevels, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool)
{
	return NULL;
}

/*!
	IRuTexture *CRuEngine_NULL::CreateVolumeTexture(UINT32 width, UINT32 height, UINT32 depth, UINT32 mipLevels, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool)
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
IRuVolumeTexture *CRuEngine_NULL::CreateVolumeTexture(UINT32 width, UINT32 height, UINT32 depth, UINT32 mipLevels, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool)
{
	return NULL;
}

/*!
	IRuTexture *CRuEngine_NULL::CreateRenderTarget(UINT32 width, UINT32 height, UINT32 mipLevels, RuSurfaceFormat surfaceFormat)
	Creates a render target
	@param width
	@param height
	@param mipLevels
	@param surfaceFormat
	@return IRuTexture*
	@retval NULL Could not create the render target
*/
IRuTexture *CRuEngine_NULL::CreateRenderTarget(UINT32 width, UINT32 height, UINT32 mipLevels, RuSurfaceFormat surfaceFormat)
{
	return NULL;
}

/*!
	IRuTexture *CRuEngine_NULL::CreateMatchedRenderTarget(UINT32 mipLevels, RuSurfaceFormat surfaceFormat)
	Creates a render target matched to the dimensions of the back buffer
	@param mipLevels
	@param surfaceFormat
	@return IRuTexture*
	@retval NULL Could not create the render target
*/
IRuTexture *CRuEngine_NULL::CreateMatchedRenderTarget(UINT32 mipLevels, RuSurfaceFormat surfaceFormat)
{
	return NULL;
}

/*!
	IRuTextureView *CRuEngine_NULL::CreateTextureView()
	Creates a default, empty texture view
	@return IRuTextureView*
	@retval NULL Could not create the texture view
*/
IRuTextureView *CRuEngine_NULL::CreateTextureView()
{
	return NULL;
}

IRuSurface *CRuEngine_NULL::CreateDepthStencilSurface(UINT32 width, UINT32 height, RuSurfaceFormat surfaceFormat)
{
	return NULL;
}

BOOL CRuEngine_NULL::CopyTexture(IRuEngineResource *srcTexture, IRuEngineResource *destTexture, INT32 surfLevel)
{
	return TRUE;
}

BOOL CRuEngine_NULL::CopyTextureWithConversion(IRuEngineResource *srcTexture, IRuEngineResource *destTexture, INT32 surfLevel)
{
	return TRUE;
}

BOOL CRuEngine_NULL::UpdateTexture(IRuBaseTexture *srcTexture, IRuBaseTexture *dstTexture)
{
	return TRUE;
}

BOOL CRuEngine_NULL::SetVertexStream(INT32 streamIdx, IRuVertexBuffer *vertexBuffer, INT32 stride)
{
	return TRUE;
}

BOOL CRuEngine_NULL::SetIndexStream(IRuIndexBuffer *indexBuffer)
{
	return TRUE;
}

/*!
	UINT32 CRuEngine_NULL::Render(UINT32 numVertices, UINT32 numPrimitives, RuPrimitiveType primType)
	Renders the currently selected buffers.
	@param numVertices
	@param numPrimitives
	@param primType
	@return UINT32 The number of primitives rendered
*/
UINT32 CRuEngine_NULL::Render(UINT32 numVertices, UINT32 numPrimitives, UINT32 primitiveOffset, RuPrimitiveType primType)
{
	return 0;
}

/*!
	IRuSwapChain *CRuEngine_NULL::CreateSwapChain(UINT32 width, UINT32 height, HWND focusWindow)
	Creates a new swap chain
	@param width
	@param height
	@param focusWindow Window to which the swap chain is attached
	@return IRuSwapChain*
	@retval NULL Operation failed
*/
IRuSwapChain *CRuEngine_NULL::CreateSwapChain(UINT32 width, UINT32 height, HWND deviceWindow)
{
	return NULL;
}

BOOL CRuEngine_NULL::IsFullscreen()
{
	return FALSE;
}

/*!
	BOOL CRuEngine_NULL::SetResolution(IRuSwapChain *swapChain, UINT32 width, UINT32 height, BOOL fullscreen, HWND deviceWindow)
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
BOOL CRuEngine_NULL::SetResolution(IRuSwapChain *swapChain, UINT32 width, UINT32 height, BOOL fullscreen, HWND deviceWindow)
{
	return TRUE;
}

BOOL CRuEngine_NULL::ResetDevice()
{
	return TRUE;
}

BOOL CRuEngine_NULL::RestoreDevice()
{
	return TRUE;
}

IRuGPUQuery *CRuEngine_NULL::CreateQuery(RuGPUQueryType queryType)
{
	return NULL;
}

/*!
	BOOL CRuEngine_NULL::GetRenderTargetSize(UINT32 &widthOut, UINT32 &heightOut)
	Retrieves the current render target's (always retrieves RT0) dimensions.
	@param widthOut Width of the current render target
	@param heightOut Height of the current render target
	@return BOOL
	@retval TRUE Operation succeeded
	@retval FALSE Operation failed
*/
BOOL CRuEngine_NULL::GetRenderTargetSize(UINT32 &widthOut, UINT32 &heightOut)
{
	return TRUE;
}

IRuSwapChain *CRuEngine_NULL::GetPrimarySwapChain()
{
	return NULL;
}

DWORD CRuEngine_NULL::GetAdapterVendorID()
{
	return 0;
}

DWORD CRuEngine_NULL::GetAdapterDeviceID()
{
	return 0;
}

BOOL CRuEngine_NULL::SetCursor(INT32 xHotSpot, INT32 yHotSpot, IRuTexture *cursorTexture)
{
	return TRUE;
}

BOOL CRuEngine_NULL::ShowCursor(BOOL showCursor)
{
	return TRUE;
}

BOOL CRuEngine_NULL::SetCursorPosition(INT32 x, INT32 y)
{
	return TRUE;
}

INT32 CRuEngine_NULL::Metric_GetNumRequestedStateChanges()
{
	return 0;
}

INT32 CRuEngine_NULL::Metric_GetNumStateChanges()
{
	return 0;
}

void CRuEngine_NULL::Metric_OutputAllocationReport()
{
}

INT32 CRuEngine_NULL::Metric_GetDeviceBufferPoolFootprint()
{
	return 0;
}

INT32 CRuEngine_NULL::Metric_GetTexturePoolFootprint()
{
	return 0;
}

INT32 CRuEngine_NULL::GetNumAvailableDisplayModes()
{
	return 0;
}

RuDisplayMode CRuEngine_NULL::GetAvailableDisplayModeByIndex(INT32 idx)
{
	RuDisplayMode dispMode = { 640, 480, 60, ruSURFACEFMT_A8R8G8B8 };
	return dispMode;
}

// ************************************************************************************************************************************************************

CRuEngine_NULL*							g_ruEngineNULL = NULL;

// ************************************************************************************************************************************************************

#pragma managed
