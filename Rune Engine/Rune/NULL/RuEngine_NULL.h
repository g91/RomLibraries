/*!
	@project Rune
	@file RuEngine_NULL.h

	Copyright (c) 2004 All rights reserved

	@author John Tang
	@date 2006/10/31
*/


#ifndef _RUENGINE_NULL_H_
#define _RUENGINE_NULL_H_

#include "../Engine/Base/RuEngine_Engine.h"

#pragma unmanaged


// Forward declarations
class CRuEngine_NULL;

// ************************************************************************************************************************************************************

class CRuVertexDeclaration_NULL : public IRuVertexDeclaration
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
											CRuVertexDeclaration_NULL();
	virtual									~CRuVertexDeclaration_NULL();

	virtual void							Invalidate();
	virtual BOOL							IsInvalidated();
	virtual BOOL							Restore();

	virtual const CRuVertexElement*			GetVertexElements() const;
	virtual void							Select() const;

	virtual INT32							GetNumStreams() const;
	virtual INT32							GetStreamStride(INT32 streamIndex) const;
};

// ************************************************************************************************************************************************************

/*!
	CRuVertexBuffer_NULL
	@author John Tang
	@version 2006/10/31
*/
class CRuVertexBuffer_NULL : public IRuVertexBuffer
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
									CRuVertexBuffer_NULL();
	virtual							~CRuVertexBuffer_NULL();

	virtual void					Invalidate();
	virtual BOOL					IsInvalidated();
	virtual BOOL					Restore();

	virtual void*					Lock(UINT offset = 0, UINT size = 0);
	virtual void*					Lock(UINT offset, UINT size, DWORD lockFlags);
	virtual void					Unlock();

	virtual void					Select(UINT32 streamIndex, UINT32 stride) const;

	virtual UINT32					GetSize() const;
	virtual UINT32					GetOffset() const;

	virtual BOOL					Subdivide(INT32 numChildVBs, IRuVertexBuffer **childVBOut);
};

// ************************************************************************************************************************************************************


/*!
	CRuIndexBuffer_NULL
	@author John Tang
	@version 2006/10/31
*/
class CRuIndexBuffer_NULL : public IRuIndexBuffer
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
									CRuIndexBuffer_NULL();
	virtual							~CRuIndexBuffer_NULL();

	virtual void					Invalidate();
	virtual BOOL					IsInvalidated();
	virtual BOOL					Restore();

	virtual void*					Lock(UINT offset = 0, UINT size = 0);
	virtual void*					Lock(UINT offset, UINT size, DWORD lockFlags);
	virtual void					Unlock();

	virtual void					Select();

	virtual UINT32					GetSize() const;
	virtual UINT32					GetOffset() const;
	virtual UINT32					GetIndexOffset() const;
};

// ************************************************************************************************************************************************************

class CRuViewport_NULL : public IRuViewport
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	DWORD							m_clearFlags;
	RUCOLOR							m_clearColor;
	REAL							m_clearZ;
	DWORD							m_clearStencil;

public:
									CRuViewport_NULL();
	virtual							~CRuViewport_NULL();

	virtual void					Invalidate();
	virtual BOOL					IsInvalidated();
	virtual BOOL					Restore();

	virtual INT32					GetViewportWidth();
	virtual INT32					GetViewportHeight();
	virtual void					SetViewportParameters(INT32 x, INT32 y, INT32 width, INT32 height, REAL minZ, REAL maxZ);

	virtual DWORD					GetClearFlags();
	virtual void					SetClearFlags(DWORD clearFlags);

	virtual RUCOLOR					GetClearColor();
	virtual void					SetClearColor(RUCOLOR color);

	virtual REAL					GetClearZ();
	virtual void					SetClearZ(REAL z);

	virtual DWORD					GetClearStencil();
	virtual void					SetClearStencil(DWORD stencil);

	virtual BOOL					SelectAsViewport();
};

// ************************************************************************************************************************************************************

/*!
	CRuTexture_NULL
	@author John Tang
	@version 2006/10/31
*/
class CRuTexture_NULL : public IRuTexture
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
									CRuTexture_NULL();
	virtual							~CRuTexture_NULL();

	virtual void					Invalidate();
	virtual BOOL					IsInvalidated();
	virtual BOOL					Restore();

	virtual const char*				GetTextureName();
	virtual void					SetTextureName(const char *textureName);

	virtual void					Select(UINT32 samplerIndex);

	virtual BOOL					GenerateMipLevels();

	virtual BOOL					IsDynamic();
	virtual BOOL					IsRenderTarget();

	virtual UINT32					GetLevelCount();
	virtual UINT32					GetLOD();
	virtual UINT32					SetLOD(UINT32 lod);

	virtual RuResourcePool			GetPool();
	virtual	RuSurfaceFormat			GetSurfaceFormat();

	virtual BOOL					SelectAsRenderTarget(INT32 rtIndex, UINT32 level);

	virtual	UINT32					GetLevelWidth(UINT32 level);
	virtual	UINT32					GetLevelHeight(UINT32 level);
	virtual IRuSurface*				GetLevelSurface(UINT32 level);

	virtual BOOL					Lock(UINT32 level, void **bitsOut, INT32 *pitchOut);
	virtual BOOL					Unlock(UINT32 level);
};

// ************************************************************************************************************************************************************

/*!
	CRuSwapChain_NULL
	@author John Tang
	@version 2006/10/31
*/
class CRuSwapChain_NULL : public IRuSwapChain
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
									CRuSwapChain_NULL();
	virtual							~CRuSwapChain_NULL();

	virtual void					Invalidate();												// Invalidate the resource
	virtual BOOL					IsInvalidated();										// Check whether the resource is lost
	virtual BOOL					Restore();

	virtual BOOL					SelectAsRenderTarget(INT32 rtIndex);

	virtual UINT32					GetWidth() const;
	virtual UINT32					GetHeight() const;

	virtual BOOL					Present();

	virtual BOOL					SaveToFile(const char *fileName, RuImageFileFormat imageFormat);
};

// ************************************************************************************************************************************************************

/*!
	This is the detailed class description



	@author John Tang
	@version 2004/06/15
*/
class CRuEngine_NULL : public IRuEngine
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
												CRuEngine_NULL();
	virtual										~CRuEngine_NULL();

	// State management
	virtual void								ClearStateCache();
	virtual void								ClearSampler(DWORD sampler);

	// Scene management
	virtual BOOL								BeginScene();
	virtual BOOL								EndScene();
	virtual BOOL								Clear(DWORD clearFlags, RUCOLOR color, float z, DWORD stencil);
	virtual BOOL								ResetViewport();

	// Depth-stencil
	virtual BOOL								RestoreDefaultDepthStencil();

	// Scissor test
	virtual BOOL								SetScissorRect(RECT *scissorRect);

	// Clip planes
	virtual BOOL								SetClipPlane(INT32 index, const CRuPlane4 &plane);
	virtual BOOL								SetClipPlaneEnable(INT32 index, BOOL enable);

	// Resource management
	virtual IRuVertexStreamLayout*				CreateVertexStreamLayout(const CRuVertexElement *vertexElements, INT32 streamIdx);
	virtual IRuVertexDeclaration*				CreateVertexDeclaration(const CRuVertexElement *vertexElements);

	virtual IRuVertexBuffer*					CreateVertexBuffer(UINT32 size);						// Creates a pooled static vertex buffer
	virtual IRuVertexBuffer*					CreateDynamicVertexBuffer(UINT32 size);					// Creates a dynamic vertex buffer
	virtual IRuIndexBuffer*						CreateIndexBuffer(UINT32 size);							// Creates a pooled static index buffer
	virtual IRuIndexBuffer*						CreateDynamicIndexBuffer(UINT32 size);					// Creates a dynamic index buffer

	virtual IRuViewport*						CreateViewport(INT32 x, INT32 y, INT32 width, INT32 height);

	virtual IRuTexture*							CreateTexture(UINT32 width, UINT32 height, UINT32 mipLevels, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool);
	virtual IRuVolumeTexture*					CreateVolumeTexture(UINT32 width, UINT32 height, UINT32 depth, UINT32 mipLevels, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool);
	virtual IRuTexture*							CreateRenderTarget(UINT32 width, UINT32 height, UINT32 mipLevels, RuSurfaceFormat surfaceFormat);
	virtual IRuTexture*							CreateMatchedRenderTarget(UINT32 mipLevels, RuSurfaceFormat surfaceFormat);

	virtual IRuTextureView*						CreateTextureView();

	virtual IRuSurface*							CreateDepthStencilSurface(UINT32 width, UINT32 height, RuSurfaceFormat surfaceFormat);

	// Texture utilities
	virtual BOOL								CopyTexture(IRuEngineResource *srcTexture, IRuEngineResource *destTexture, INT32 surfLevel);
	virtual BOOL								CopyTextureWithConversion(IRuEngineResource *srcTexture, IRuEngineResource *destTexture, INT32 surfLevel);
	virtual BOOL								UpdateTexture(IRuBaseTexture *srcTexture, IRuBaseTexture *dstTexture);

	// Rendering
	virtual BOOL								SetVertexStream(INT32 streamIdx, IRuVertexBuffer *vertexBuffer, INT32 stride);
	virtual BOOL								SetIndexStream(IRuIndexBuffer *indexBuffer);
	virtual UINT32								Render(UINT32 numVertices, UINT32 numPrimitives, UINT32 primitiveOffset, RuPrimitiveType primType);

	// Environment setup and management
	virtual IRuSwapChain*						CreateSwapChain(UINT32 width, UINT32 height, HWND deviceWindow);
	virtual BOOL								IsFullscreen();
	virtual BOOL								SetResolution(IRuSwapChain *swapChain, UINT32 width, UINT32 height, BOOL fullscreen, HWND deviceWindow = NULL);
	virtual BOOL								ResetDevice();
	virtual BOOL								RestoreDevice();

	// GPU query mechanism
	virtual IRuGPUQuery*						CreateQuery(RuGPUQueryType queryType);

	// Environment queries
	virtual BOOL								GetRenderTargetSize(UINT32 &widthOut, UINT32 &heightOut);
	virtual IRuSwapChain*						GetPrimarySwapChain();

	// Device queries
	virtual DWORD								GetAdapterVendorID();
	virtual DWORD								GetAdapterDeviceID();

	// Cursor
	virtual BOOL								SetCursor(INT32 xHotSpot, INT32 yHotSpot, IRuTexture *cursorTexture);
	virtual BOOL								ShowCursor(BOOL showCursor);
	virtual BOOL								SetCursorPosition(INT32 x, INT32 y);

	// Metrics
	virtual INT32								Metric_GetNumRequestedStateChanges();
	virtual INT32								Metric_GetNumStateChanges();
	virtual void								Metric_OutputAllocationReport();
	virtual INT32								Metric_GetDeviceBufferPoolFootprint();
	virtual INT32								Metric_GetTexturePoolFootprint();

	// Display resolution
	virtual INT32								GetNumAvailableDisplayModes();
	virtual RuDisplayMode						GetAvailableDisplayModeByIndex(INT32 idx);
};

// ************************************************************************************************************************************************************

extern CRuEngine_NULL*							g_ruEngineNULL;

// ************************************************************************************************************************************************************

inline void CRuVertexBuffer_NULL::Invalidate()
{
}

inline BOOL CRuVertexBuffer_NULL::IsInvalidated()
{
	return FALSE;
}

inline BOOL CRuVertexBuffer_NULL::Restore()
{
	return FALSE;
}

inline UINT32 CRuVertexBuffer_NULL::GetSize() const
{
	return 0;
}

inline UINT32 CRuVertexBuffer_NULL::GetOffset() const
{
	return 0;
}

inline void CRuVertexBuffer_NULL::Select(UINT32 streamIndex, UINT32 stride) const
{
}

// ************************************************************************************************************************************************************

inline void CRuIndexBuffer_NULL::Invalidate()
{
}

inline BOOL CRuIndexBuffer_NULL::IsInvalidated()
{
	return FALSE;
}

inline BOOL CRuIndexBuffer_NULL::Restore()
{
	return FALSE;
}

inline UINT32 CRuIndexBuffer_NULL::GetSize() const
{
	return 0;
}

inline UINT32 CRuIndexBuffer_NULL::GetOffset() const
{
	return 0;
}

inline UINT32 CRuIndexBuffer_NULL::GetIndexOffset() const
{
	return 0;
}

inline void CRuIndexBuffer_NULL::Select()
{
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
