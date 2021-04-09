/*!
	@file RuTexture_Proxy.h

	Copyright (c) 2006-2007 All rights reserved
*/

#ifndef _RUTEXTURE_PROXY_H_
#define _RUTEXTURE_PROXY_H_

#include "../../Engine/Base/RuCore.h"
#include "../../Engine/Base/RuEngine_Engine.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuSurface : public IRuSurface
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	UINT32									m_width;
	UINT32									m_height;
	RuSurfaceFormat							m_surfaceFormat;
	BYTE*									m_surfaceData;

	BOOL									m_sharedMemory;

public:
											CRuSurface();
											CRuSurface(UINT32 width, UINT32 height, RuSurfaceFormat surfaceFormat);
											CRuSurface(UINT32 width, UINT32 height, RuSurfaceFormat surfaceFormat, BYTE *surfaceData);
	virtual									~CRuSurface();

	// IRuObject interface
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// IRuEngineResource interface
	virtual void							Invalidate();
	virtual BOOL							IsInvalidated();
	virtual BOOL							Restore();

	// IRuSurface interface
	virtual RuResourcePool					GetPool();
	virtual	RuSurfaceFormat					GetSurfaceFormat();

	virtual	UINT32							GetWidth() const;
	virtual	UINT32							GetHeight() const;

	virtual BOOL							Lock(void **bitsOut, INT32 *pitchOut);
	virtual BOOL							Unlock();

	virtual BOOL							SelectAsRenderTarget(INT32 rtIndex);
	virtual BOOL							SelectDepthStencil();
};

// ************************************************************************************************************************************************************

/*!
	Proxy texture class. This class can be directly instantiated independent of the
	underlying engine by using new. Instantiation of the engine-specific texture object is
	deferred until the texture is selected into a sampler and is handled by the instance
	manager.

	@author John Tang
	@version 2008/04/23
*/
class CRuTexture_Proxy : public IRuTexture
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuThread_CriticalSection		m_cs;											//!< Object-level critical section

	BOOL							m_isInvalidated;

	// Texture information (proxy mode)
	IRuTexture*						m_proxyTexture;

	// Texture information (data mode)
	UINT32							m_textureWidth;
	UINT32							m_textureHeight;
	BYTE*							m_textureData;

	DWORD							m_levelCount;
	RuSurfaceFormat					m_surfaceFormat;
	UINT32							m_lod;

	CRuSurface**					m_levelSurfaces;

public:
									CRuTexture_Proxy();
	virtual							~CRuTexture_Proxy();

	BOOL							Allocate(IRuTexture *proxyTexture);
	BOOL							Allocate(UINT32 width, UINT32 height, UINT32 mipLevels, RuSurfaceFormat surfaceFormat);
	BOOL							Deallocate();

	// Serialization interface
	virtual BOOL					SerializeFrom(IRuStream *stream);				//!< Serializes the object from the stream
	virtual BOOL					SerializeTo(IRuStream *stream);					//!< Serializes the object to the stream

	// Engine resource interface
	virtual void					Invalidate();
	virtual BOOL					IsInvalidated();
	virtual BOOL					Restore();

	// IRuBaseTexture interface
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

	// IRuTexture interface
	virtual BOOL					SelectAsRenderTarget(INT32 rtIndex, UINT32 level);

	virtual	UINT32					GetLevelWidth(UINT32 level);
	virtual	UINT32					GetLevelHeight(UINT32 level);
	virtual IRuSurface*				GetLevelSurface(UINT32 level);

	virtual BOOL					Lock(UINT32 level, void **bitsOut, INT32 *pitchOut);
	virtual BOOL					Unlock(UINT32 level);
};

// ************************************************************************************************************************************************************

class CRuVolumeTexture_Proxy : public IRuVolumeTexture
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Texture information (proxy mode)
	IRuVolumeTexture*				m_proxyTexture;

	// Texture information (data mode)
	UINT32							m_textureWidth;
	UINT32							m_textureHeight;
	UINT32							m_textureDepth;
	BYTE*							m_textureData;

	DWORD							m_levelCount;
	RuSurfaceFormat					m_surfaceFormat;
	UINT32							m_lod;

public:
									CRuVolumeTexture_Proxy();
									CRuVolumeTexture_Proxy(UINT32 width, UINT32 height, UINT32 depth, UINT32 mipLevels, RuSurfaceFormat surfaceFormat);
	virtual							~CRuVolumeTexture_Proxy();

	BOOL							Allocate(IRuVolumeTexture *proxyTexture);
	BOOL							Allocate(UINT32 width, UINT32 height, UINT32 depth, UINT32 mipLevels, RuSurfaceFormat surfaceFormat);
	BOOL							Deallocate();

	// Serialization interface
	virtual BOOL					SerializeFrom(IRuStream *stream);				//!< Serializes the object from the stream
	virtual BOOL					SerializeTo(IRuStream *stream);					//!< Serializes the object to the stream

	// Engine resource interface
	virtual void					Invalidate();
	virtual BOOL					IsInvalidated();
	virtual BOOL					Restore();

	// IRuBaseTexture interface
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

	// IRuVolumeTexture interface
	virtual	UINT32					GetLevelWidth(UINT32 level);
	virtual	UINT32					GetLevelHeight(UINT32 level);
	virtual	UINT32					GetLevelDepth(UINT32 level);

	virtual BOOL					Lock(UINT32 level, void **bitsOut, INT32 *rowPitchOut, INT32 *slicePitchOut);
	virtual BOOL					Unlock(UINT32 level);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Surface, CRuSurface)
ruOBJECTMODULE_DEFINE(CRuObjectModule_Texture_Proxy, CRuTexture_Proxy)
ruOBJECTMODULE_DEFINE(CRuObjectModule_VolumeTexture_Proxy, CRuVolumeTexture_Proxy)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
