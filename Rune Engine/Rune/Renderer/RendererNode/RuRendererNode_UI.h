/*!
	@file RuRendererNode_UI.h

	Copyright (c) 2004-2008 All rights reserved
*/

#ifndef _RURENDERERNODE_UI_H_
#define _RURENDERERNODE_UI_H_

#include "../../Renderer/RuRenderer.h"
#include "../../Renderer/RendererNode/RuRendererNode_RenderContext.h"
#include "../../Renderer/RendererNode/RuRendererNode_SetScissor.h"
#include "../../Renderer/RendererNode/RuRendererNode_ViewportClear.h"

#include "../../Engine/Texture/RuTextureAtlas_Aggregate.h"
#include "../../Engine/Texture/RuTextureAtlas_Paged.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Enumeration of effect types supported by the 2D toolkit.
*/
enum Ru2D_EffectType
{
	ru2DFXTYPE_NULL								= 0,				//!< Invalid effect type
	ru2DFXTYPE_FLAT								= 1,				//!< Diffuse color only, blended using SRCALPHA:INVSRCALPHA
	ru2DFXTYPE_FLAT_MASKED						= 2,				//!< Diffuse color only, blended using SRCALPHA:INVSRCALPHA
	ru2DFXTYPE_BLIT								= 3,				//!< Image transfer modulated by color, blended using SRCALPHA:INVSRCALPHA
	ru2DFXTYPE_BLIT_ADD							= 4,				//!< Image transfer modulated by color, blended using ONE:ONE
	ru2DFXTYPE_BLIT_FILTER						= 5,				//!< Image transfer modulated by color, blended using SRCALPHA:INVSRCALPHA, with linear filtering
	ru2DFXTYPE_BLIT_MASK						= 6,				//!< 
	ru2DFXTYPE_BLIT_MASKED						= 7,				//!< 
	ru2DFXTYPE_BLIT_MASK_Z						= 8,				//!< Image transfer as a Z mask, texels that pass alpha test are set with Z = 0.0
	ru2DFXTYPE_BLIT_LUMINANCE					= 9,				//!< Image transfer using luminance only modulated by color, blended using SRCALPHA:INVSRCALPHA, with linear filtering
	ru2DFXTYPE_TEXTBLIT_FILTER					= 10,				//!< Image transfer modulated by color, blended using SRCALPHA:INVSRCALPHA, with linear filtering
	ru2DFXTYPE_DARKEN							= 11,				//!< Area darkening with strength specified in ru2DFXPARAM_STRENGTH
	ru2DFXTYPE_EXPONENTIATE						= 12,				//!< Exponentiation with power specified in ru2DFXPARAM_STRENGTH
	ru2DFXTYPE_GAUSSIAN							= 13,				//!< Gaussian filter
	ru2DFXTYPE_DISTORTION						= 14,				//!< Distortion filter
	ru2DFXTYPE_COUNT							= 15,
	ru2DFXTYPE_FORCE_DWORD						= 0x7FFFFFFF
};

/*!
	Enumeration of effect parameters.
*/
enum Ru2D_EffectParameter
{
	ru2DFXPARAM_STRENGTH						= 0,				//!< Effect strength, parameter is a DWORD in the range [0, 100]
	ru2DFXPARAM_COLOR							= 1,				//!< Effect color, parameter is a RUCOLOR value (0xAARRGGBB)
	ru2DFXPARAM_ROTATE							= 2,				//!< Rotation, parameter is a Ru2D_RotateMode value
	ru2DFXPARAM_STENCILID						= 3,				//!< Stencil ID, parameter is a UINT8
	ru2DFXPARAM_FORCE_DWORD						= 0x7FFFFFFF
};

/*!
	Enumeration of rotate modes.
*/
enum Ru2D_RotateMode
{
	ru2DROTATE_NONE								= 0,				//!< No rotation
	ru2DROTATE_CLOCKWISE_90						= 1,				//!< Clockwise 90 degrees
	ru2DROTATE_CLOCKWISE_180					= 2,				//!< Clockwise 180 degrees
	ru2DROTATE_CLOCKWISE_270					= 3,				//!< Clockwise 270 degrees
	ru2DROTATE_FLIP_U							= 4,				//!< Flip U axis
	ru2DROTATE_FLIP_V							= 5,				//!< Flip V axis
	ru2DROTATE_FORCE_DWORD						= 0x7FFFFFFF
};

/*!
	Enumeration of uv coordinate modes for 2D output.
*/
enum Ru2D_UVMode
{
	ru2DUVMODE_OFFSET							= 0,				//!< UV coordinates are offset according to rasterization rules
	ru2DUVMODE_DIRECT							= 1,				//!< UV coordinates are fed as-is

	ru2DUVMODE_FORCE_DWORD						= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

class CRuRendererNode_UI : public CRuRendererNode
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuObject*								m_parentRenderer;
	IRuObject*								m_renderTarget;

	IRuShader*								m_effectShaders[ru2DFXTYPE_COUNT];

	// Current state
	CRuRendererNode*						m_renderGraph;									//!< Render graph to apply to, NULL if none
	CRuRendererNode_RenderContext*			m_curRenderContext;

	// Camera stack
	BOOL									m_cameraStackDirty;
	CRuStack<CRuCamera *>					m_cameraStack;									//!< Camera stack

	CRuRectangle*							m_lastScissorRect;

	// ???
	CRuStack<IRuObject *>					m_ru2D_RTStack;
	IRuObject*								m_ru2D_CurrentRenderTarget;

	CRuStack<IRuViewport *>					m_ru2D_ViewportStack;

	// 2D toolkit objects
	CRuStack<REAL>							m_ru2D_Viewport_Width_Stack;
	CRuStack<REAL>							m_ru2D_Viewport_Height_Stack;
	REAL									m_ru2D_Viewport_Width;
	REAL									m_ru2D_Viewport_Height;

	CRuTexture_Proxy*						m_ru2D_32BitAtlasTexture;
	CRuTexture_Proxy*						m_ru2D_32BitAtlasTexture_Text;
	CRuTextureAtlas_Aggregate*				m_ru2D_TextureAtlas;							//!< Texture atlas for the 2D toolkit

	IRuTexture*								m_ru2D_ActiveTexture;
	INT32									m_ru2D_ActiveTextureHandle;
	UINT32									m_ru2D_ActiveTextureWidth;
	UINT32									m_ru2D_ActiveTextureHeight;

	Ru2D_UVMode								m_ru2D_UVMode;
	Ru2D_EffectType							m_ru2D_EffectType;								//!< Currently selected 2D effect type
	IRuShader*								m_ru2D_EffectShader;							//!< Currently selected effect shader (determined by effect type)
	DWORD									m_ru2D_DiffuseColor;							//!< Currently used diffuse color
	UINT8									m_ru2D_StencilID;
	REAL									m_ru2D_Rotation;								//!< Current rotation angle in radians
	INT32									m_ru2D_MaxPrimitiveCount;						//!< Maximum number of primitives in the buffer
	INT32									m_ru2D_PrimitiveCount;							//!< Number of primitives in the buffer
	UINT32									m_ru2D_RefRTWidth;								//!< Render target width
	UINT32									m_ru2D_RefRTHeight;								//!< Render target height
	UINT32									m_ru2D_RefTextureWidth;							//!< Texture width used for UV calculation
	UINT32									m_ru2D_RefTextureHeight;						//!< Texture height used for UV calculation

	CRuMaterial*							m_ru2D_RefMaterial;

	CRuMesh*								m_ru2D_Mesh;									//!< Renderer buffer mesh
	CRuArrayList<CRuProxyMesh *>			m_ru2D_ProxyMeshes;
	CRuMaterial*							m_ru2D_Material;								//!< Renderer buffer material
	REAL*									m_ru2D_MaterialConstants;
	CRuRectangle							m_ru2D_Fan_SrcRegion;							//!< Fan stitcher source region
	CRuRectangle							m_ru2D_Fan_DestRegion;							//!< Fan stitcher destination region
	CRuArrayList<CRuVector3>				m_ru2D_VertexList;								//!< Piecewise submission vertex list

	CRuVector3*								m_ru2D_Mesh_Position;
	DWORD*									m_ru2D_Mesh_Diffuse;
	REAL*									m_ru2D_Mesh_UV0;
	UINT16*									m_ru2D_Mesh_Indices;

	// Renderer state
	UINT8									m_nextStencilID;								//!< Next ID to use for mask-stencil operations

	// Stacks
	CRuStack<CRuMaterial *>					m_usedMaterialStack;
	CRuStack<CRuMaterial *>					m_unusedMaterialStack;

	CRuStack<CRuRendererNode_RenderContext *>	m_usedRenderContexts;
	CRuStack<CRuRendererNode_RenderContext *>	m_unusedRenderContexts;

	// Statistics
	UINT32									m_numPrimitivesRendered;						//!< Total number of rendered primitives this frame
	UINT32									m_numBatchesIssued;								//!< Total number of batches issued this frame
	INT32									m_probe_2D_Submit;
	INT32									m_probe_2D_BatchSize;
	INT32									m_probe_2D_BatchCount;
	INT32									m_probe_2D_Flush;

public:
											CRuRendererNode_UI();
	virtual									~CRuRendererNode_UI();

	BOOL									SetRenderTarget(IRuObject *renderTarget);

	IRuTexture*								GetReservedDXT5Texture();

	// Misc
	BOOL									IsolationRender_Begin();
	CRuRendererNode*						IsolationRender_End();

	// These require a render context change
	BOOL									RenderTarget_Push(IRuObject *renderTarget);
	BOOL									RenderTarget_Pop();

	BOOL									Viewport_Push(IRuViewport *viewport);
	BOOL									Viewport_Pop();

	BOOL									SetScissorRect(CRuRectangle *scissorRect);
	BOOL									ClearViewport(IRuViewport *viewport);

	// These don't
	BOOL									Camera_Push(CRuCamera *camera);
	BOOL									Camera_Pop();

	BOOL									ViewportScale_Push(REAL width, REAL height);
	BOOL									ViewportScale_Pop();

	BOOL									SetUVMode(Ru2D_UVMode uvMode);
	BOOL									SetEffectType(Ru2D_EffectType effectType);
	BOOL									SetEffectParameter(Ru2D_EffectParameter effectParam, DWORD parameter);
	BOOL									SetShaderParameter(RuParameterType paramType, UINT64 param);
	BOOL									SetTexture(IRuTexture *texture, BOOL allowBinding = TRUE, RuTextureChannel texChannel = ruTEXCHANNEL_DIFFUSEMAP);
	BOOL									SetRotation(REAL rotation);
	void									Submit(const CRuRectangle &srcRect, const CRuRectangle &destRect);
	void									Submit(INT32 numRects, const CRuRectangle *srcRect, const CRuRectangle *destRect, const DWORD *color);
	void									Flush();

	// 3D rendering
	void									Submit(CRuEntity *entity);
	void									Submit(IRuMesh *mesh, IRuMaterial *material);

	// Coordinate conversion
	void									ConvertCoordinates(CRuVector3 &point);
	void									ConvertCoordinates(CRuRectangle &rectangle);
	void									WorldToViewport(CRuCamera *camera, CRuVector3 &point);
	void									WorldToScreen(CRuCamera *camera, CRuVector3 &point);
	void									HomogeneousToViewport(CRuVector3 &point);
	void									HomogeneousToViewport(CRuRectangle &rectangle);
	void									ViewportToHomogeneous(CRuVector3 &point);
	void									ViewportToHomogeneous(CRuRectangle &rectangle);

	// Renderer state
	UINT8									GetNextStencilID();
	CRuRendererNode_RenderContext*			GetCurrentRenderContext();

	// CRuRendererNode overridable interface
	virtual void							SetParentRenderer(IRuObject *renderer);
	virtual void							Begin();
	virtual void							End();
	virtual void							Execute(CRuKeyHash<PTRVALUE> &executionMap);

protected:
	BOOL									AllocateRenderContext();
	BOOL									TerminateRenderContext();
	BOOL									UpdateRenderContextSettings();
	void									FlushRenderContextStack();

	CRuMaterial*							AllocateMaterial();
	void									FlushMaterialStack();

	void									RenderBatch();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_RendererNode_UI, CRuRendererNode_UI)

// ************************************************************************************************************************************************************

extern CRuRendererNode_UI*				g_ruUIRenderer;

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
