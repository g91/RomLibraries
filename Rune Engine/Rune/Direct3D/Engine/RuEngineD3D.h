/*!
	@project Rune
	@file RuEngineD3D.h

	Copyright (c) 2004 All rights reserved

	@author John Tang
	@date 2004/06/15
*/

#pragma once

#include "../../Core/Collection/RuCollections.h"
#include "../../Core/Collection/RuCollectionsHash.h"
#include "../../Core/Collection/RuCollectionsLRU.h"

#include "../../Engine/Base/RuEngine_Engine.h"
#include "../../Engine/Base/RuEngine_DeviceBufferPool.h"
#include "../../Engine/Base/RuEngine_EngineResourceArray.h"
#include "../../Engine/Base/RuEngine_TexturePool.h"

#include <d3d9.h>
#include <d3dx9.h>

#pragma managed(push, off)

// ************************************************************************************************************************************************************

#pragma comment (lib, "d3d9")
#pragma comment (lib, "d3dx9")

// ************************************************************************************************************************************************************

enum RuEngineMode
{
	ruENGINEMODE_SOFTWARE			= 0,
	ruENGINEMODE_MIXED				= 1,
	ruENGINEMODE_HARDWARE			= 2,
	ruENGINEMODE_PURE				= 3,
	ruENGINEMODE_FORCE_DWORD		= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************
// HELPERS

inline D3DFORMAT ToD3DFormat(RuSurfaceFormat surfaceFormat)
{
	return (D3DFORMAT) surfaceFormat;
}

inline RuSurfaceFormat ToRuSurfaceFormat(D3DFORMAT surfaceFormat)
{
	return (RuSurfaceFormat) surfaceFormat;
}

inline RuResourcePool ToRuResourcePool(D3DPOOL pool)
{
	switch(pool)
	{
		case D3DPOOL_DEFAULT:
			return ruRESPOOL_DEFAULT;
		case D3DPOOL_MANAGED:
			return ruRESPOOL_MANAGED;
		case D3DPOOL_SYSTEMMEM:
			return ruRESPOOL_SYSTEMMEMORY;
		case D3DPOOL_SCRATCH:
			return ruRESPOOL_SCRATCH;
	}

	return ruRESPOOL_DEFAULT;
}

// ************************************************************************************************************************************************************

typedef BOOL(* RuEngineConfirmDeviceCallback)(D3DCAPS9 *caps, RuEngineMode engineMode, RuSurfaceFormat adapterFormat, RuSurfaceFormat backBufferFormat);

// Forward declarations
class CRuEngineD3D;

// ************************************************************************************************************************************************************

class CRuVertexDeclarationD3D : public IRuVertexDeclaration
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IDirect3DVertexDeclaration9*			m_vertexDeclaration;	// Direct3D vertex declaration object

	INT32									m_numStreams;
	IRuVertexStreamLayout*					m_streamLayouts[16];

public:
											CRuVertexDeclarationD3D(IDirect3DVertexDeclaration9 *vertexDeclaration);
	virtual									~CRuVertexDeclarationD3D();

	virtual void							Invalidate();
	virtual BOOL							IsInvalidated();
	virtual BOOL							Restore();

	virtual void							Select() const;

	virtual INT32							GetNumStreams() const;
	virtual INT32							GetStreamStride(INT32 streamIndex) const;

	virtual BOOL							BindVertexStreamLayout(INT32 streamIndex, IRuVertexStreamLayout *vertexStreamLayout);
	virtual IRuVertexStreamLayout*			GetVertexStreamLayout(INT32 streamIndex);

	virtual BOOL							IsEqualTo(IRuVertexDeclaration *vertexDecl);
};

// ************************************************************************************************************************************************************

/*!
	CRuVertexBufferD3D
	@author John Tang
	@version 2004/05/12
*/
class CRuVertexBufferD3D : public IRuVertexBuffer
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IDirect3DDevice9*				m_parentDevice;				// Parent device of this resource

	IDirect3DVertexBuffer9*			m_vertexBuffer;				// Direct3D vertex buffer object
	BOOL							m_isDynamic;				// Indicates whether this buffer is a dynamic buffer
	UINT32							m_size;						// Size of the vertex buffer, in bytes

public:
									CRuVertexBufferD3D(IDirect3DVertexBuffer9 *vertexBuffer, BOOL isDynamic);
	virtual							~CRuVertexBufferD3D();

	virtual void					Invalidate();
	virtual BOOL					IsInvalidated();
	virtual BOOL					Restore();

	virtual void*					Lock(UINT32 offset = 0, UINT32 size = 0, DWORD lockFlags = 0);
	virtual void					Unlock();

	virtual UINT32					GetSize() const;

	virtual void					Select(UINT32 streamIndex, UINT32 stride);

									operator IDirect3DVertexBuffer9*() const;
};

// ************************************************************************************************************************************************************

/*!
	CRuIndexBufferD3D
	@author John Tang
	@version 2004/05/12
*/
class CRuIndexBufferD3D : public IRuIndexBuffer
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IDirect3DDevice9*				m_parentDevice;				// Parent device of this resource

	IDirect3DIndexBuffer9*			m_indexBuffer;				// Direct3D index buffer object
	BOOL							m_isDynamic;				// Indicates whether this buffer is a dynamic buffer
	UINT32							m_size;						// Size of the index buffer, in bytes

public:
									CRuIndexBufferD3D(IDirect3DIndexBuffer9 *indexBuffer, BOOL isDynamic);
	virtual							~CRuIndexBufferD3D();

	virtual void					Invalidate();
	virtual BOOL					IsInvalidated();
	virtual BOOL					Restore();

	virtual void*					Lock(UINT32 offset = 0, UINT32 size = 0, DWORD lockFlags = 0);
	virtual void					Unlock();

	virtual UINT32					GetSize() const;

	virtual void					Select();

									operator IDirect3DIndexBuffer9*() const;
};

// ************************************************************************************************************************************************************

class CRuViewportD3D : public IRuViewport
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	D3DVIEWPORT9					m_viewport;

	DWORD							m_clearFlags;
	RUCOLOR							m_clearColor;
	REAL							m_clearZ;
	DWORD							m_clearStencil;

public:
									CRuViewportD3D();
	virtual							~CRuViewportD3D();

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
	CRuSurfaceD3D
	@author John Tang
	@version 2004/11/18
*/
class CRuSurfaceD3D : public IRuSurface
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IDirect3DDevice9*				m_parentDevice;				// Parent device of this resource

	// Direct3D surface object
	IDirect3DSurface9*				m_surface;					// Surface object

	// D3D-specific information
	D3DSURFACE_DESC					m_surfaceDesc;				// Surface descriptor

	// Surface information
	RuResourcePool					m_resourcePool;				// Resource pool this texture belongs to
	RuSurfaceFormat					m_surfaceFormat;			// Surface format

public:
									CRuSurfaceD3D(IDirect3DSurface9 *surface);
	virtual							~CRuSurfaceD3D();

	virtual void					Invalidate();
	virtual BOOL					IsInvalidated();
	virtual BOOL					Restore();

	virtual RuResourcePool			GetPool();
	virtual	RuSurfaceFormat			GetSurfaceFormat();

	virtual	UINT32					GetWidth() const;
	virtual	UINT32					GetHeight() const;

	virtual BOOL					Lock(void **bitsOut, INT32 *pitchOut);
	virtual BOOL					Unlock();

	virtual BOOL					SelectAsRenderTarget(INT32 rtIndex);
	virtual BOOL					SelectDepthStencil();

	IDirect3DSurface9*				GetIDirect3DSurface();
};

// ************************************************************************************************************************************************************

/*!
	CRuTextureD3D
	@author John Tang
	@version 2004/05/12
*/
class CRuTextureD3D : public IRuTexture
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IDirect3DDevice9*				m_parentDevice;				// Parent device of this resource

	// Direct3D texture object
	IDirect3DTexture9*				m_texture;

	// D3D-specific information
	DWORD							m_levelCount;
	D3DSURFACE_DESC					m_topSurfaceDesc;
	CRuSurfaceD3D**					m_surfaces;
	D3DSURFACE_DESC*				m_surfaceDesc;

	// Texture information
	char*							m_textureName;
	BOOL							m_isDynamic;
	BOOL							m_isRenderTarget;			// Is this texture a render target?
	BOOL							m_isMatchedToBackBuffer;	// Does this texture have dimensions always matched to the backbuffer?
	RuResourcePool					m_resourcePool;				// Resource pool this texture belongs to
	RuSurfaceFormat					m_surfaceFormat;

public:
									CRuTextureD3D(IDirect3DTexture9 *texture, BOOL isDynamic, BOOL isMatchedToBackBuffer = FALSE);
	virtual							~CRuTextureD3D();

	// IRuObject interface
	virtual INT32					GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	// Engine resource interface
	virtual void					Invalidate();
	virtual BOOL					IsInvalidated();
	virtual BOOL					Restore();

	// Texture interface
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

	// Driver-specific access
	IDirect3DTexture9*				GetIDirect3DTexture();
};

// ************************************************************************************************************************************************************

/*!
	CRuVolumeTextureD3D
	@author John Tang
	@version 2004/05/12
*/
class CRuVolumeTextureD3D : public IRuVolumeTexture
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IDirect3DDevice9*				m_parentDevice;				// Parent device of this resource

	// Direct3D texture object
	IDirect3DVolumeTexture9*		m_texture;

	// D3D-specific information
	DWORD							m_levelCount;
	D3DVOLUME_DESC					m_topSurfaceDesc;
//	CRuSurfaceD3D**					m_surfaces;

	// Texture information
	char*							m_textureName;
	BOOL							m_isDynamic;
	RuResourcePool					m_resourcePool;				// Resource pool this texture belongs to
	RuSurfaceFormat					m_surfaceFormat;

public:
									CRuVolumeTextureD3D(IDirect3DVolumeTexture9 *texture, BOOL isDynamic, BOOL isMatchedToBackBuffer = FALSE);
	virtual							~CRuVolumeTextureD3D();

	// IRuObject interface
	virtual INT32					GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	// Engine resource interface
	virtual void					Invalidate();
	virtual BOOL					IsInvalidated();
	virtual BOOL					Restore();

	// Texture interface
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
	virtual	UINT32					GetLevelDepth(UINT32 level);

	virtual BOOL					Lock(UINT32 level, void **bitsOut, INT32 *rowPitchOut, INT32 *slicePitchOut);
	virtual BOOL					Unlock(UINT32 level);

	IDirect3DVolumeTexture9*		GetIDirect3DVolumeTexture();
};

// ************************************************************************************************************************************************************

/*!
	CRuTextureViewD3D
	@author John Tang
	@version 2006/09/21
*/
class CRuTextureViewD3D : public IRuTextureView
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuTextureD3D*					m_basisTexture;												//!< Basis texture
	INT32							m_x;
	INT32							m_y;
	INT32							m_width;
	INT32							m_height;
	RuSurfaceFormat					m_viewFormat;
	IRuViewport*					m_viewport;													//!< Viewport

	BOOL							m_textureViewTransformDirty;
	CRuMatrix4x4					m_textureViewTransform;										//!< Transform to go from texture view uv space to basis texture uv space

public:
									CRuTextureViewD3D();
	virtual							~CRuTextureViewD3D();

	virtual void					Invalidate();												// Invalidate the resource
	virtual BOOL					IsInvalidated();										// Check whether the resource is lost
	virtual BOOL					Restore();													// Attempt to restore the resource

	virtual BOOL					SetBasisTexture(IRuTexture *basisTexture);
	virtual BOOL					SetViewFormat(RuSurfaceFormat viewFormat);
	virtual BOOL					SetViewOrigin(INT32 x, INT32 y);
	virtual BOOL					SetViewSize(INT32 width, INT32 height);

	virtual INT32					GetViewWidth() const;
	virtual INT32					GetViewHeight() const;
	virtual RuSurfaceFormat			GetViewFormat() const;

	virtual IRuTexture*				GetTexture();												// Gets the texture that is the basis of the view
	virtual IRuViewport*			GetViewport();												// Gets the viewport that represents the view on the texture

	virtual const CRuMatrix4x4&		GetTextureViewTransform();

protected:
	void							CalculateTextureViewTransform();
};

// ************************************************************************************************************************************************************

/*!
	CRuSwapChainD3D
	@author John Tang
	@version 2004/05/18
*/
class CRuSwapChainD3D : public IRuSwapChain
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IDirect3DDevice9*				m_parentDevice;				//!< Parent device of this resource
	IDirect3DSwapChain9*			m_swapChain;				//!< Swap chain object
	IDirect3DSurface9*				m_backBuffer;				//!< Back buffer
	
	D3DPRESENT_PARAMETERS			m_presentParameters;		//!< Currently selected settings
	BOOL							m_isPrimarySwapChain;		//!< Is this swap chain the primary swap chain?

public:
									CRuSwapChainD3D(IDirect3DSwapChain9 *swapChain, BOOL isPrimarySwapChain);
	virtual							~CRuSwapChainD3D();

	virtual void					Invalidate();												// Invalidate the resource
	virtual BOOL					IsInvalidated();										// Check whether the resource is lost
	virtual BOOL					Restore();

	virtual BOOL					SelectAsRenderTarget(INT32 rtIndex);

	virtual UINT32					GetWidth() const;
	virtual UINT32					GetHeight() const;

	virtual BOOL					Present();

	virtual BOOL					SaveToFile(const char *fileName, RuImageFileFormat imageFormat);

	IDirect3DSurface9*				GetIDirect3DSurface();
	D3DPRESENT_PARAMETERS&			PresentParameters();

protected:
	BOOL							SetD3DSwapChain(IDirect3DSwapChain9 *swapChain);

	friend CRuEngineD3D;
};

// ************************************************************************************************************************************************************

class CRuGPUQueryD3D : public IRuGPUQuery
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IDirect3DDevice9*				m_parentDevice;				//!< Parent device of this resource
	IDirect3DQuery9*				m_query;
	D3DQUERYTYPE					m_queryType;

public:
									CRuGPUQueryD3D(IDirect3DQuery9 *query, D3DQUERYTYPE queryType);
	virtual							~CRuGPUQueryD3D();

	virtual void					Invalidate();
	virtual BOOL					IsInvalidated();
	virtual BOOL					Restore();

	virtual BOOL					Begin();
	virtual BOOL					End();

	virtual INT32					GetData(void *dataOut, DWORD dataSize, DWORD getDataFlags);
};

// ************************************************************************************************************************************************************

/*!
	This is the detailed class description



	@author John Tang
	@version 2004/06/15
*/
class CRuEngineD3D : public IRuEngine
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct RuMultisampleConflictD3D
	{
		D3DFORMAT								m_depthStencilFormat;				// Depth/Stencil format with conflict
		D3DMULTISAMPLE_TYPE						m_multisampleType;					// Multisample type with conflict
	};

	struct RuDeviceComboD3D
	{
		INT32									m_adapterOrdinal;					// Adapter ordinal
		D3DDEVTYPE								m_deviceType;						// Device type
		D3DFORMAT								m_adapterFormat;					// Adapter surface format
		D3DFORMAT								m_backBufferFormat;					// Back buffer surface format
		BOOL									m_isWindowed;						// Whether the combination is a windowed mode
		CRuArrayList<D3DFORMAT>					m_depthStencilFormatList;			// List of D3DFORMATs
		CRuArrayList<D3DMULTISAMPLE_TYPE>		m_multisampleTypeList;				// List of D3DMULTISAMPLE_TYPEs
		CRuArrayList<DWORD>						m_multisampleQualityList;			// List of DWORDs (number of quality levels for each multisample type)
		CRuArrayList<RuMultisampleConflictD3D>	m_multisampleConflictList;			// List of RuMultisampleConflictD3Ds
		CRuArrayList<RuEngineMode>				m_engineModeList;					// List of RuEngineModes
		CRuArrayList<DWORD>						m_presentIntervalList;				// List of D3DPRESENT_INTERVALs
	};

	struct RuDeviceInfoD3D
	{
		INT32									m_adapterOrdinal;					// Adapter ordinal
		D3DDEVTYPE								m_deviceType;						// Direct3D device type
		D3DCAPS9								m_caps;								// Direct3D cap bits
		CRuArrayList<RuDeviceComboD3D *>		m_deviceComboList;					// List of D3DDeviceCombo pointers

												~RuDeviceInfoD3D();
	};

	struct RuAdapterInfoD3D
	{
		INT32									m_adapterOrdinal;					// Adapter ordinal
		D3DADAPTER_IDENTIFIER9					m_adapterIdentifier;				// Adapter identifier
		CRuArrayList<D3DDISPLAYMODE>			m_displayModeList;					// List of D3DDISPLAYMODEs
		CRuArrayList<RuDeviceInfoD3D *>			m_deviceInfoList;					// List of D3DDeviceInfo pointers

												~RuAdapterInfoD3D();
	};

	struct RuEngineSettingD3D
	{
		BOOL									m_isWindowed;						// Is windowed mode selected?

		INT32									m_windowedWidth;					// Primary window width in windowed mode
		INT32									m_windowedHeight;					// Primary window height in windowed mode

		DWORD									m_preferredFullscreenWidth;			// Preferred fullscreen width
		DWORD									m_preferredFullscreenHeight;		// Preferred fullscreen height
		DWORD									m_preferredFullscreenRefreshRate;	// Preferred fullscreen refresh rate

		// Windowed mode information
		RuAdapterInfoD3D*						m_windowedAdapterInfo;
		RuDeviceInfoD3D*						m_windowedDeviceInfo;
		RuDeviceComboD3D*						m_windowedDeviceCombo;

		D3DDISPLAYMODE							m_windowedDisplayMode;				// Not changable by the user
		D3DFORMAT								m_windowedDepthStencilBufferFormat;
		D3DMULTISAMPLE_TYPE						m_windowedMultisampleType;
		DWORD									m_windowedMultisampleQuality;
		RuEngineMode							m_windowedEngineMode;
		UINT32									m_windowedPresentInterval;

		// Fullscreen mode information
		RuAdapterInfoD3D*						m_fullscreenAdapterInfo;
		RuDeviceInfoD3D*						m_fullscreenDeviceInfo;
		RuDeviceComboD3D*						m_fullscreenDeviceCombo;

		D3DDISPLAYMODE							m_fullscreenDisplayMode;			// Changable by the user
		D3DFORMAT								m_fullscreenDepthStencilBufferFormat;
		D3DMULTISAMPLE_TYPE						m_fullscreenMultisampleType;
		DWORD									m_fullscreenMultisampleQuality;
		RuEngineMode							m_fullscreenEngineMode;
		UINT32									m_fullscreenPresentInterval;

		// Property access methods
		RuAdapterInfoD3D*						AdapterInfo();
		RuDeviceInfoD3D*						DeviceInfo();
		RuDeviceComboD3D*						DeviceCombo();
		D3DDISPLAYMODE&							DisplayMode();
		D3DFORMAT&								DepthStencilBufferFormat();
		D3DMULTISAMPLE_TYPE&					MultisampleType();
		DWORD&									MultisampleQuality();
		RuEngineMode&							EngineMode();
		UINT32&									PresentInterval();

		// Value retrieval shortcuts
		INT32									GetAdapterOrdinal();
		D3DDEVTYPE								GetDeviceType();
		D3DFORMAT								GetBackBufferFormat();

		// Mode selection
		BOOL									FindBestWindowedMode(IDirect3D9 *d3dObject, CRuArrayList<RuAdapterInfoD3D *> &adapterInfoList, BOOL requireHAL, BOOL requireREF);
		BOOL									FindBestFullscreenMode(IDirect3D9 *d3dObject, CRuArrayList<RuAdapterInfoD3D *> &adapterInfoList, BOOL requireHAL, BOOL requireREF);
		BOOL									FindBestFullscreenMode();
		void									BuildPresentParamsFromSettings(HWND deviceWindow, D3DPRESENT_PARAMETERS &presentParamsOut);
		void									BuildPresentParamsForWindowed(HWND deviceWindow, UINT32 width, UINT32 height,  D3DPRESENT_PARAMETERS &presentParamsOut);
	};

	struct RuVEC
	{
		const CRuVertexElement*					m_vertexElements;

		BOOL									operator==(const RuVEC &ve) const;
	};

	struct RuEngineMetric
	{
		INT32									m_vb_TotalPages;
		INT32									m_vb_TotalBytes;
		INT32									m_vb_Allocations;					//!< Number of vertex buffer allocations
		INT32									m_vb_CommittedPages;				//!< Number of committed pages
		INT32									m_vb_CommittedBytes;
		INT32									m_vb_UtilizedBytes;					//!< Number of utilized bytes
		REAL									m_vb_PercentWaste;

		INT32									m_ib_TotalPages;
		INT32									m_ib_TotalBytes;
		INT32									m_ib_Allocations;					//!< Number of vertex buffer allocations
		INT32									m_ib_CommittedPages;				//!< Number of committed pages
		INT32									m_ib_CommittedBytes;
		INT32									m_ib_UtilizedBytes;					//!< Number of utilized bytes
		REAL									m_ib_PercentWaste;
	};

	const static DWORD							s_maxRenderTargets = 8;
	const static DWORD							s_maxRenderStates = 256;
	const static DWORD							s_maxStages = 16;
	const static DWORD							s_maxTextureStates = 33;
	const static DWORD							s_maxSamplerStates = 16;
	const static DWORD							s_maxVertexStreams = 8;
	const static DWORD							s_maxSamplers = 16;

	// ...
	HWND										m_primaryWindow;					// Handle to primary swap chain window

	// Direct3D objects
	IDirect3D9*									m_d3dObject;						// Direct3D object
	IDirect3DDevice9*							m_d3dDevice;						// Direct3D device
	IDirect3DSurface9*							m_defaultDepthStencilSurface;		// Default depth stencil surface
	D3DCAPS9									m_d3dDeviceCaps;					// Direct3D device caps

	// Engine state cache
	BOOL										m_renderTargetsOK[s_maxRenderTargets];
	IDirect3DSurface9*							m_renderTargets[s_maxRenderTargets];

	BOOL										m_currentDepthStencilOK;
	IDirect3DSurface9*							m_currentDepthStencil;

	BOOL										m_currentViewportOK;
	D3DVIEWPORT9*								m_currentViewport;

	BOOL										m_renderStatesChanged[s_maxRenderStates];
	BOOL										m_renderStatesOK[s_maxRenderStates];
	DWORD										m_renderStates[s_maxRenderStates];

	BOOL										m_textureStageStatesChanged[s_maxStages][s_maxTextureStates];
	BOOL										m_textureStageStatesOK[s_maxStages][s_maxTextureStates];
	DWORD										m_textureStageStates[s_maxStages][s_maxTextureStates];

	BOOL										m_samplerStageStatesChanged[s_maxStages][s_maxSamplerStates];
	BOOL										m_samplerStageStatesOK[s_maxStages][s_maxSamplerStates];
	DWORD										m_samplerStageStates[s_maxStages][s_maxSamplerStates];

	BOOL										m_currentVertexDeclarationOK;
	IDirect3DVertexDeclaration9*				m_currentVertexDeclaration;

	BOOL										m_currentVertexShaderOK;
	IDirect3DVertexShader9*						m_currentVertexShader;

	BOOL										m_currentPixelShaderOK;
	IDirect3DPixelShader9*						m_currentPixelShader;

	BOOL										m_currentVertexStreamOK[s_maxVertexStreams];
	IRuVertexBuffer*							m_currentVertexStream[s_maxVertexStreams];
	INT32										m_currentVertexStreamStride[s_maxVertexStreams];

	BOOL										m_currentIndexStreamOK;
	IRuIndexBuffer*								m_currentIndexStream;

	BOOL										m_currentTextureOK[s_maxSamplers];
	IDirect3DBaseTexture9*						m_currentTexture[s_maxSamplers];

	INT32										m_vsConstants_Min;
	INT32										m_vsConstants_Max;
	REAL										m_vsConstants[256 * 4];

	INT32										m_psConstants_Min;
	INT32										m_psConstants_Max;
	REAL										m_psConstants[256 * 4];

	// Resource management data structures
	BOOL										m_resourceArenaInvalidated;			// Is the resource arena in an invalidated state?

	CRuEngine_DeviceBufferPool					m_staticVertexBufferPool;
	CRuEngine_DeviceBufferPool					m_staticIndexBufferPool;

	CRuEngine_DeviceBufferPool					m_dynamicVertexBufferPool;
	CRuEngine_DeviceBufferPool					m_dynamicIndexBufferPool;

	CRuEngine_TexturePool						m_rtTexturePool;
	CRuEngine_TexturePool						m_2dTexturePool;
	CRuEngine_TexturePool						m_3dTexturePool;

	CRuEngine_EngineResourceArray				m_swapChains;						//!< Array of swap chains, there is always one primary swap chain at index 0
	CRuEngine_EngineResourceArray				m_rtTextures;
	CRuEngine_EngineResourceArray				m_depthStencilSurfaces;

//	CRuHashMap<RuVEC, IRuVertexDeclaration *>*	m_vertexDeclarations;				// Hash of existing vertex declarations

	CRuArrayList<CRuVertexStreamLayout *>		m_vertexStreamLayouts;				//!< List of existing vertex stream layouts
	CRuArrayList<CRuVertexDeclarationD3D *>		m_vertexDeclarations;				//!< List of existing vertex declarations

	// Cursor data
	INT32										m_cursor_xHotSpot;
	INT32										m_cursor_yHotSpot;
	CRuTextureD3D*								m_cursor_Texture;
	INT32										m_cursor_TextureIteration;

	// Metric values
	INT32										m_numRequestedStateChanges;			// Number of state change requests made during the last BeginScene/EndScene block
	INT32										m_numStateChanges;					// Number of state changes made during the last BeginScene/EndScene block
	INT32										m_numVSConstantChanges;
	INT32										m_numPSConstantChanges;

	// Probes
	INT32										m_probe_StateChanges;
	INT32										m_probe_VSConstantChanges;
	INT32										m_probe_PSConstantChanges;

	// Enumerated adapter information
	CRuArrayList<RuAdapterInfoD3D *>			m_adapterInfoList;					// List of adapter info structures
	CRuArrayList<D3DFORMAT>						m_allowedAdapterFormatList;			// List of allowable adapter formats
	RuEngineSettingD3D							m_engineSetting;

	// Filtering values
	UINT32										m_appMinFullscreenWidth;			// Minimum fullscreen width
	UINT32										m_appMinFullscreenHeight;			// Minimum fullscreen height
	UINT32										m_appMinColorChannelBits;			// Minimum color bits per channel in adapter format
	UINT32										m_appMinAlphaChannelBits;			// Minimum alpha bits per pixel in back buffer format
	UINT32										m_appMinDepthBits;					// Minimum depth buffer bits
	UINT32										m_appMinStencilBits;				// Minimum stencil bits
	BOOL										m_appUsesDepthBuffer;				// Is depth buffer required?
	BOOL										m_appUsesMixedMode;					// Is mixed vertex processing mode required?
	BOOL										m_appRequiresWindowed;				// Is windowed mode required?
	BOOL										m_appRequiresFullscreen;			// Is fullscreen mode required?
	RuEngineConfirmDeviceCallback				m_confirmDeviceCallback;			// Device confirmation callback

	// Query test code
	CRuArrayList<IDirect3DQuery9 *>				m_outstandingQueries;
	CRuStack<IDirect3DQuery9 *>					m_unusedQueries;

public:
												CRuEngineD3D();
	virtual										~CRuEngineD3D();

	// Engine startup & shutdown
	BOOL										Startup(HWND primaryWindow);
	BOOL										Shutdown();

	HWND										GetPrimaryWindow();

	// Direct3D access
	IDirect3DDevice9*							GetD3DDevice();
	const D3DCAPS9&								GetD3DDeviceCaps();

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

	// Environment enumeration and selection
	BOOL										EnumerateAdapters();
	BOOL										SelectInitialDisplaySettings(BOOL fullscreen);

	// Display resolution
	virtual INT32								GetNumAvailableDisplayModes();
	virtual RuDisplayMode						GetAvailableDisplayModeByIndex(INT32 idx);

	void										FlushStateChanges();

	// Preliminary Stuff
	virtual BOOL								UseDefaultSamplerFilterState(DWORD stage);


	// Render state access
	HRESULT										SetRenderTarget(INT32 rtIndex, IDirect3DSurface9 *surface);
	HRESULT										SetDepthStencil(IDirect3DSurface9 *surface);
	HRESULT										SetViewport(D3DVIEWPORT9 *viewport);
	HRESULT										SetRenderState(D3DRENDERSTATETYPE state, DWORD value);
	HRESULT										SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE state, DWORD value);
	HRESULT										SetSamplerState(DWORD stage, D3DSAMPLERSTATETYPE state, DWORD value);
	HRESULT										SetVertexDeclaration(IDirect3DVertexDeclaration9 *decl);
	HRESULT										SetVertexShader(IDirect3DVertexShader9 *vs);
	HRESULT										SetPixelShader(IDirect3DPixelShader9 *ps);
	HRESULT										SetTexture(DWORD sampler, IDirect3DBaseTexture9 *texture);

	HRESULT										SetVertexShaderConstants(INT32 index, REAL *constants, INT32 numConstants);
	HRESULT										SetPixelShaderConstants(INT32 index, REAL *constants, INT32 numConstants);

protected:
	BOOL										EnumerateDevices(RuAdapterInfoD3D *adapterInfo, CRuArrayList<D3DFORMAT> &adapterFormatList);
	BOOL										EnumerateDeviceCombos(RuDeviceInfoD3D *deviceInfo, CRuArrayList<D3DFORMAT> &adapterFormatList);
	void										BuildDepthStencilFormatList(RuDeviceComboD3D *deviceCombo);
	void										BuildMultisampleTypeList(RuDeviceComboD3D *deviceCombo);
	void										BuildMultisampleConflictList(RuDeviceComboD3D *deviceCombo);
	void										BuildEngineModeList(RuDeviceInfoD3D *deviceInfo, RuDeviceComboD3D *deviceCombo);
	void										BuildPresentIntervalList(RuDeviceInfoD3D *deviceInfo, RuDeviceComboD3D *deviceCombo);

	IRuDeviceBuffer*							CreateVertexBuffer_Internal(INT32 size);
	IRuDeviceBuffer*							CreateDynamicVertexBuffer_Internal(INT32 size);
	IRuDeviceBuffer*							CreateIndexBuffer_Internal(INT32 size);
	IRuDeviceBuffer*							CreateDynamicIndexBuffer_Internal(INT32 size);

	IRuBaseTexture*								CreateRenderTarget_Internal(INT32 dim0, INT32 dim1, INT32 dim2, INT32 levelCount, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool);
	IRuBaseTexture*								CreateTexture_Internal(INT32 dim0, INT32 dim1, INT32 dim2, INT32 levelCount, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool);
	IRuBaseTexture*								CreateVolumeTexture_Internal(INT32 dim0, INT32 dim1, INT32 dim2, INT32 levelCount, RuSurfaceFormat surfaceFormat, RuResourcePool resourcePool);

	void										ClearVertexDeclarationCache();
	BOOL										ClearVertexDeclarationCacheCallback(const void *key, void *data);

	static UINT32								ColorChannelBits(D3DFORMAT format);
	static UINT32								AlphaChannelBits(D3DFORMAT format);
	static UINT32								DepthBits(D3DFORMAT format);
	static UINT32								StencilBits(D3DFORMAT format);
	static int									SortModesCallback(const void *arg1, const void *arg2);
};

// ************************************************************************************************************************************************************

extern CRuEngineD3D*							g_ruEngineD3D;

// ************************************************************************************************************************************************************

inline void CRuVertexBufferD3D::Invalidate()
{
	ruSAFE_RELEASE(m_vertexBuffer);
}

inline BOOL CRuVertexBufferD3D::IsInvalidated()
{
	return m_vertexBuffer == NULL;
}

inline BOOL CRuVertexBufferD3D::Restore()
{
	return FALSE;
}

inline UINT32 CRuVertexBufferD3D::GetSize() const
{
	return m_size;
}

inline void CRuVertexBufferD3D::Select(UINT32 streamIndex, UINT32 stride)
{
	// Set the vertex buffer through the engine
	m_engine->SetVertexStream(streamIndex, this, stride);
}

inline CRuVertexBufferD3D::operator IDirect3DVertexBuffer9*() const
{
	return m_vertexBuffer;
}

// ************************************************************************************************************************************************************

inline void CRuIndexBufferD3D::Invalidate()
{
	ruSAFE_RELEASE(m_indexBuffer);
}

inline BOOL CRuIndexBufferD3D::IsInvalidated()
{
	return m_indexBuffer == NULL;
}

inline BOOL CRuIndexBufferD3D::Restore()
{
	return FALSE;
}

inline UINT32 CRuIndexBufferD3D::GetSize() const
{
	return m_size;
}

inline void CRuIndexBufferD3D::Select()
{
	// Set the index buffer through the engine
	m_engine->SetIndexStream(this);
}

inline CRuIndexBufferD3D::operator IDirect3DIndexBuffer9*() const
{
	return m_indexBuffer;
}

// ************************************************************************************************************************************************************

inline IDirect3DDevice9 *CRuEngineD3D::GetD3DDevice()
{
	return m_d3dDevice;
}

inline const D3DCAPS9 &CRuEngineD3D::GetD3DDeviceCaps()
{
	return m_d3dDeviceCaps;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
