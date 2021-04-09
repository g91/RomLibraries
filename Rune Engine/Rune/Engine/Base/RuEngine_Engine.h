/*!
	@project Rune
	@file RuEngineBase.h

	Copyright (c) 2004 All rights reserved

	@author John Tang
	@date 2004/07/08
*/

#pragma once

#include "../../Engine/Base/RuEngine_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

// Forward declarations
class IRuEngine;

// ************************************************************************************************************************************************************

/*!
	IRuEngineResource
	@author John Tang
	@version 2004/05/12
*/
class IRuEngineResource : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuEngine*								m_engine;													//!< Engine that owns this resource
	BOOL									m_discarded;
	UINT32									m_priority;													//!< Resource priority
	INT32									m_iteration;												//!< Resource iteration
	INT32									m_gcMarker;													//!< Garbage-collection age

	PTRVALUE								m_instanceData;												//!< Pointer to the instanced version of the resource

public:
											IRuEngineResource();
	virtual									~IRuEngineResource();

	IRuEngine*								GetEngine();												// Gets the engine that owns this resource
	void									SetEngine(IRuEngine *engine);								// Sets the engine that owns this resource

	PTRVALUE								GetInstanceData();
	void									SetInstanceData(PTRVALUE instanceData);

	void									Discard();													//!< Mark the engine resource as discarded (e.g. do not use)
	BOOL									IsDiscarded();												//!< Is the engine resource marked as discarded?

	UINT32									GetPriority() const;										// Gets resource priority
	void									SetPriority(UINT32 priority);								// Sets resource priority

	INT32									GetIteration() const;
	void									SetIteration(INT32 iteration);

	INT32									GetGCMarker();
	void									IncrementGCMarker();
	void									ResetGCMarker();
	void									SetGCMarker(INT32 gcMarker);

	virtual void							Invalidate() = 0;											// Invalidate the resource
	virtual BOOL							IsInvalidated() = 0;										// Check whether the resource is lost
	virtual BOOL							Restore() = 0;												// Attempt to restore the resource
};

// ************************************************************************************************************************************************************

class IRuVertexStreamLayout : public IRuEngineResource
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual										~IRuVertexStreamLayout();

	virtual INT32								GetStreamStride() = 0;
	virtual BOOL								IsEqualTo(const IRuVertexStreamLayout *vertexStreamLayout) = 0;
};

// ************************************************************************************************************************************************************

/*!
	IRuVertexDeclaration
	@author John Tang
	@version 2004/10/07
*/
class IRuVertexDeclaration : public IRuEngineResource
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuVertexDeclaration();

	virtual void							Select() const = 0;											// Selects the vertex declaration for use

	virtual INT32							GetNumStreams() const = 0;									// Gets the number of streams required by the vertex declaration
	virtual INT32							GetStreamStride(INT32 streamIndex) const = 0;				// Gets the size of each vertex in the specified stream

	virtual BOOL							BindVertexStreamLayout(INT32 streamIndex, IRuVertexStreamLayout *vertexStreamLayout) = 0;
	virtual IRuVertexStreamLayout*			GetVertexStreamLayout(INT32 streamIndex) = 0;

	virtual BOOL							IsEqualTo(IRuVertexDeclaration *vertexDecl) = 0;
};

// ************************************************************************************************************************************************************

/*!
	IRuDeviceBuffer
	@author John Tang
	@version 2008.11.07
*/
class IRuDeviceBuffer : public IRuEngineResource
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuDeviceBuffer();

	virtual void*							Lock(UINT32 offset = 0, UINT32 size = 0, DWORD lockFlags = 0) = 0;	// Lock buffer
	virtual void							Unlock() = 0;														// Unlock buffer

	virtual UINT32							GetSize() const = 0;												// Get the size of the buffer
};

// ************************************************************************************************************************************************************

/*!
	IRuVertexBuffer
	@author John Tang
	@version 2004/05/12
*/
class IRuVertexBuffer : public IRuDeviceBuffer
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuVertexBuffer();

	virtual void							Select(UINT32 streamIndex, UINT32 stride) = 0;			// Select the buffer into the specified stream for use
};

// ************************************************************************************************************************************************************

/*!
	IRuIndexBuffer
	@author John Tang
	@version 2004/05/12
*/
class IRuIndexBuffer : public IRuDeviceBuffer
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuIndexBuffer();

	virtual void							Select() = 0;												// Select the buffer for use
};

// ************************************************************************************************************************************************************

/*!
	IRuVertexShader
	@author John Tang
	@version 2004/05/14
*/
class IRuVertexShader : public IRuEngineResource
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuVertexShader() { }

	virtual void							Select() const = 0;											// Select the vertex shader for use
};

// ************************************************************************************************************************************************************

/*!
	IRuPixelShader
	@author John Tang
	@version 2004/05/14
*/
class IRuPixelShader : public IRuEngineResource
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuPixelShader() { }

	virtual void							Select() const = 0;											// Select the pixel shader for use
};

// ************************************************************************************************************************************************************

class IRuViewport : public IRuEngineResource
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuViewport();

	virtual INT32							GetViewportWidth() = 0;
	virtual INT32							GetViewportHeight() = 0;
	virtual void							SetViewportParameters(INT32 x, INT32 y, INT32 width, INT32 height, REAL minZ, REAL maxZ) = 0;

	virtual DWORD							GetClearFlags() = 0;
	virtual void							SetClearFlags(DWORD clearFlags) = 0;

	virtual RUCOLOR							GetClearColor() = 0;
	virtual void							SetClearColor(RUCOLOR color) = 0;

	virtual REAL							GetClearZ() = 0;
	virtual void							SetClearZ(REAL z) = 0;

	virtual DWORD							GetClearStencil() = 0;
	virtual void							SetClearStencil(DWORD stencil) = 0;

	virtual BOOL							SelectAsViewport() = 0;
};

// ************************************************************************************************************************************************************

/*!
	IRuSurface
	@author John Tang
	@version 2004/05/18
*/
class IRuSurface : public IRuEngineResource
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuSurface();

	virtual RuResourcePool					GetPool() = 0;										// Gets the resource pool this texture resides in
	virtual	RuSurfaceFormat					GetSurfaceFormat() = 0;								// Gets the surface format of this texture

	virtual	UINT32							GetWidth() const = 0;										// Gets the width for the surface
	virtual	UINT32							GetHeight() const = 0;										// Gets the height for the surface

	virtual BOOL							Lock(void **bitsOut, INT32 *pitchOut) = 0;					// Lock the surface
	virtual BOOL							Unlock() = 0;												// Unlock the surface

	virtual BOOL							SelectAsRenderTarget(INT32 rtIndex) = 0;					// Selects the surface as a render target
	virtual BOOL							SelectDepthStencil() = 0;									// Selects the surface as a depth stencil surface
};

// ************************************************************************************************************************************************************

/*!
	IRuBaseTexture
	@author John Tang
	@version 2004/05/12
*/
class IRuBaseTexture : public IRuEngineResource
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuBaseTexture();

	virtual const char*						GetTextureName() = 0;									// Gets the texture name
	virtual void							SetTextureName(const char *textureName) = 0;				// Sets the texture name

	virtual void							Select(UINT32 samplerIndex) = 0;							// Select the texture into the specified sampler for use

	virtual BOOL							GenerateMipLevels() = 0;									// Generates the mipmap levels

	virtual BOOL							IsDynamic() = 0;
	virtual BOOL							IsRenderTarget() = 0;

	virtual UINT32							GetLevelCount() = 0;									// Gets the number of available mipmap levels
	virtual UINT32							GetLOD() = 0;											// Gets the currently set maximum LOD level
	virtual UINT32							SetLOD(UINT32 lod) = 0;										// Sets the maximum LOD level

	virtual RuResourcePool					GetPool() = 0;										// Gets the resource pool this texture resides in
	virtual	RuSurfaceFormat					GetSurfaceFormat() = 0;								// Gets the surface format of this texture
};

// ************************************************************************************************************************************************************

/*!
	IRuTexture
	@author John Tang
	@version 2004/05/12
*/
class IRuTexture : public IRuBaseTexture
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuTexture();

	virtual BOOL							SelectAsRenderTarget(INT32 rtIndex, UINT32 level) = 0;			// Selects the specified level as a render target

	virtual	UINT32							GetLevelWidth(UINT32 level) = 0;							// Gets the width for the specified level
	virtual	UINT32							GetLevelHeight(UINT32 level) = 0;							// Gets the height for the specified level
	virtual IRuSurface*						GetLevelSurface(UINT32 level) = 0;								// Gets the surface for the specified level

	virtual BOOL							Lock(UINT32 level, void **bitsOut, INT32 *pitchOut) = 0;		// Lock the texture level
	virtual BOOL							Unlock(UINT32 level) = 0;										// Unlock the texture level
};

// ************************************************************************************************************************************************************

/*!
	IRuCubeTexture
	@author John Tang
	@version 2004/05/12
*/
class IRuCubeTexture : public IRuBaseTexture
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuCubeTexture();

	virtual BOOL							SelectAsRenderTarget(INT32 rtIndex, RuCubeTextureFace cubeFace, UINT32 level) = 0;						// Selects the specified level as a render target

	virtual	UINT32							GetLevelWidth(UINT32 level) = 0;						// Gets the width for the specified level
	virtual	UINT32							GetLevelHeight(UINT32 level) = 0;						// Gets the height for the specified level

	virtual BOOL							Lock(RuCubeTextureFace cubeFace, UINT32 level, void **bitsOut, INT32 *pitchOut) = 0;
	virtual BOOL							Unlock(RuCubeTextureFace cubeFace, UINT32 level) = 0;		// Unlock the texture level
};

// ************************************************************************************************************************************************************

/*!
	IRuVolumeTexture
	@author John Tang
	@version 2004/05/12
*/
class IRuVolumeTexture : public IRuBaseTexture
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuVolumeTexture();

	virtual	UINT32							GetLevelWidth(UINT32 level) = 0;						// Gets the width for the specified level
	virtual	UINT32							GetLevelHeight(UINT32 level) = 0;						// Gets the height for the specified level
	virtual	UINT32							GetLevelDepth(UINT32 level) = 0;						// Gets the depth for the specified level

	virtual BOOL							Lock(UINT32 level, void **bitsOut, INT32 *rowPitchOut, INT32 *slicePitchOut) = 0;	// Lock the texture level
	virtual BOOL							Unlock(UINT32 level) = 0;									// Unlock the texture level
};

// ************************************************************************************************************************************************************

/*!
	IRuTextureView
	@author John Tang
	@version 2006/09/21
*/
class IRuTextureView : public IRuEngineResource
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuTextureView();

	virtual BOOL							SetBasisTexture(IRuTexture *basisTexture) = 0;
	virtual BOOL							SetViewFormat(RuSurfaceFormat viewFormat) = 0;
	virtual BOOL							SetViewOrigin(INT32 x, INT32 y) = 0;
	virtual BOOL							SetViewSize(INT32 width, INT32 height) = 0;

	virtual INT32							GetViewWidth() const = 0;
	virtual INT32							GetViewHeight() const = 0;
	virtual RuSurfaceFormat					GetViewFormat() const = 0;

	virtual IRuTexture*						GetTexture() = 0;								// Gets the texture that is the basis of the view
	virtual IRuViewport*					GetViewport() = 0;								// Gets the viewport that represents the view on the texture

	virtual const CRuMatrix4x4&				GetTextureViewTransform() = 0;
};

// ************************************************************************************************************************************************************

/*!
	IRuSwapChain
	@author John Tang
	@version 2004/05/18
*/
class IRuSwapChain : public IRuEngineResource
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuSwapChain();

	virtual BOOL							SelectAsRenderTarget(INT32 rtIndex) = 0;		// Selects the specified swap chain as a render target

	virtual UINT32							GetWidth() const = 0;							// Gets the width for the swap chain
	virtual UINT32							GetHeight() const = 0;							// Gets the height for the swap chain

	virtual BOOL							Present() = 0;									// Presents the swap chain to the display

	virtual BOOL							SaveToFile(const char *fileName, RuImageFileFormat imageFormat) = 0;
};

// ************************************************************************************************************************************************************

class IRuGPUQuery : public IRuEngineResource
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuGPUQuery();

	virtual BOOL							Begin() = 0;
	virtual BOOL							End() = 0;

	virtual INT32							GetData(void *dataOut, DWORD dataSize, DWORD getDataFlags) = 0;
};

// ************************************************************************************************************************************************************

class IRuVertexAssembly : public IRuEngineResource
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~IRuVertexAssembly();

	virtual BOOL							BindVertexBuffer(INT32 streamIdx, IRuVertexBuffer *vertexBuffer, INT32 vertexStride) = 0;

	virtual void							Select() = 0;
};

// ************************************************************************************************************************************************************

class CRuVertexAssembly : public IRuVertexAssembly
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	BOOL									m_isInvalidated;
	IRuVertexBuffer*						m_vertexStreams[16];
	INT32									m_vertexStrides[16];

public:
											CRuVertexAssembly();
	virtual									~CRuVertexAssembly();

	virtual void							Invalidate();
	virtual BOOL							IsInvalidated();
	virtual BOOL							Restore();

	virtual BOOL							BindVertexBuffer(INT32 streamIdx, IRuVertexBuffer *vertexBuffer, INT32 vertexStride);

	virtual void							Select();
};

// ************************************************************************************************************************************************************

class CRuVertexStreamLayout : public IRuVertexStreamLayout
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuVertexElement*							m_vertexElements;
	INT32										m_streamStride;

public:
												CRuVertexStreamLayout(const CRuVertexElement *vertexElements, INT32 streamIdx);
	virtual										~CRuVertexStreamLayout();

	virtual void								Invalidate();
	virtual BOOL								IsInvalidated();
	virtual BOOL								Restore();

	virtual INT32								GetStreamStride();
	virtual BOOL								IsEqualTo(const IRuVertexStreamLayout *vertexStreamLayout);
};

// ************************************************************************************************************************************************************

/*!
	IRuEngine
	@author John Tang
	@version 2004/07/08
*/
class IRuEngine : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEvent								m_event_BeginScene;
	CRuEvent								m_event_DeviceReset;
	CRuEvent								m_event_DeviceRestore;
	CRuEvent								m_event_Shutdown;

public:
	virtual									~IRuEngine();

	CRuEvent&								Event_BeginScene();
	CRuEvent&								Event_DeviceReset();
	CRuEvent&								Event_DeviceRestore();
	CRuEvent&								Event_Shutdown();

	// State management
	virtual void							ClearStateCache() = 0;
	virtual void							ClearSampler(DWORD sampler) = 0;

	// Scene management
	virtual BOOL							BeginScene() = 0;
	virtual BOOL							EndScene() = 0;
	virtual BOOL							Clear(DWORD clearFlags, RUCOLOR color, float z, DWORD stencil) = 0;
	virtual BOOL							ResetViewport() = 0;

	// Depth-stencil
	virtual BOOL							RestoreDefaultDepthStencil() = 0;

	// Scissor test
	virtual BOOL							SetScissorRect(RECT *scissorRect) = 0;

	// Clip planes
	virtual BOOL							SetClipPlane(INT32 index, const CRuPlane4 &plane) = 0;
	virtual BOOL							SetClipPlaneEnable(INT32 index, BOOL enable) = 0;

	// Resource management
	virtual IRuVertexStreamLayout*			CreateVertexStreamLayout(const CRuVertexElement *vertexElements, INT32 streamIdx) = 0;
	virtual IRuVertexDeclaration*			CreateVertexDeclaration(const CRuVertexElement *vertexElements) = 0;

	virtual IRuVertexBuffer*				CreateVertexBuffer(UINT32 size) = 0;						// Creates a pooled static vertex buffer
	virtual IRuVertexBuffer*				CreateDynamicVertexBuffer(UINT32 size) = 0;					// Creates a dynamic vertex buffer
	virtual IRuIndexBuffer*					CreateIndexBuffer(UINT32 size) = 0;							// Creates a pooled static index buffer
	virtual IRuIndexBuffer*					CreateDynamicIndexBuffer(UINT32 size) = 0;					// Creates a dynamic index buffer

	virtual IRuViewport*					CreateViewport(INT32 x, INT32 y, INT32 width, INT32 height) = 0;

	virtual IRuTexture*						CreateTexture(UINT32 width, UINT32 height, UINT32 mipLevels, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool) = 0;
	virtual IRuVolumeTexture*				CreateVolumeTexture(UINT32 width, UINT32 height, UINT32 depth, UINT32 mipLevels, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool) = 0;
	virtual IRuTexture*						CreateRenderTarget(UINT32 width, UINT32 height, UINT32 mipLevels, RuSurfaceFormat surfaceFormat) = 0;
	virtual IRuTexture*						CreateMatchedRenderTarget(UINT32 mipLevels, RuSurfaceFormat surfaceFormat) = 0;

	virtual IRuTextureView*					CreateTextureView() = 0;

	virtual IRuSurface*						CreateDepthStencilSurface(UINT32 width, UINT32 height, RuSurfaceFormat surfaceFormat) = 0;

	// Texture utilities
	virtual BOOL							CopyTexture(IRuEngineResource *srcTexture, IRuEngineResource *destTexture, INT32 surfLevel) = 0;
	virtual BOOL							CopyTextureWithConversion(IRuEngineResource *srcTexture, IRuEngineResource *destTexture, INT32 surfLevel) = 0;
	virtual BOOL							UpdateTexture(IRuBaseTexture *srcTexture, IRuBaseTexture *dstTexture) = 0;

	// Rendering
	virtual BOOL							SetVertexStream(INT32 streamIdx, IRuVertexBuffer *vertexBuffer, INT32 stride) = 0;
	virtual BOOL							SetIndexStream(IRuIndexBuffer *indexBuffer) = 0;
	virtual UINT32							Render(UINT32 numVertices, UINT32 numPrimitives, UINT32 primitiveOffset, RuPrimitiveType primType) = 0;

	// Environment setup and management
	virtual IRuSwapChain*					CreateSwapChain(UINT32 width, UINT32 height, HWND deviceWindow) = 0;
	virtual BOOL							IsFullscreen() = 0;
	virtual BOOL							SetResolution(IRuSwapChain *swapChain, UINT32 width, UINT32 height, BOOL fullscreen, HWND deviceWindow = NULL) = 0;
	virtual BOOL							ResetDevice() = 0;
	virtual BOOL							RestoreDevice() = 0;

	// GPU query mechanism
	virtual IRuGPUQuery*					CreateQuery(RuGPUQueryType queryType) = 0;

	// Environment queries
	virtual BOOL							GetRenderTargetSize(UINT32 &widthOut, UINT32 &heightOut) = 0;
	virtual IRuSwapChain*					GetPrimarySwapChain() = 0;

	// Device queries
	virtual DWORD							GetAdapterVendorID() = 0;
	virtual DWORD							GetAdapterDeviceID() = 0;

	// Cursor
	virtual BOOL							SetCursor(INT32 xHotSpot, INT32 yHotSpot, IRuTexture *cursorTexture) = 0;
	virtual BOOL							ShowCursor(BOOL showCursor) = 0;
	virtual BOOL							SetCursorPosition(INT32 x, INT32 y) = 0;

	// Metric
	virtual INT32							Metric_GetNumRequestedStateChanges() = 0;
	virtual INT32							Metric_GetNumStateChanges() = 0;
	virtual void							Metric_OutputAllocationReport() = 0;
	virtual INT32							Metric_GetDeviceBufferPoolFootprint() = 0;
	virtual INT32							Metric_GetTexturePoolFootprint() = 0;

	// Display resolution
	virtual INT32							GetNumAvailableDisplayModes() = 0;
	virtual RuDisplayMode					GetAvailableDisplayModeByIndex(INT32 idx) = 0;
};

// ************************************************************************************************************************************************************

// Globals
extern IRuEngine*					g_ruEngine;

// ************************************************************************************************************************************************************

#pragma managed(pop)
