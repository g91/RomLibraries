/*!
	@file RuRendererNode_UI.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../../Renderer/RendererNode/RuRendererNode_UI.h"

#pragma managed(push, off)


extern INT32							g_probe_C0;
extern INT32							g_probe_C1;
extern INT32							g_probe_C2;
extern INT32							g_probe_C3;


// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuRendererNode_UI, CRuRendererNode)
ruHEAP_DEFINE_SUBCLASS(CRuRendererNode_UI, "CRuRendererNode_UI", "CRuRendererNode")
ruCLASSGUID_DEFINE(CRuRendererNode_UI, 0x8D9922D9, 0x6DFB42a1, 0x8BBD9316, 0x46B0E878)

// ************************************************************************************************************************************************************

CRuRendererNode_UI::CRuRendererNode_UI()
:	m_renderTarget(NULL),
	m_curRenderContext(NULL),
	m_lastScissorRect(NULL),
	m_cameraStackDirty(FALSE),
	m_ru2D_CurrentRenderTarget(NULL),
	m_ru2D_Viewport_Width(0.0f),
	m_ru2D_Viewport_Height(0.0f),
	m_ru2D_TextureAtlas(NULL),
	m_ru2D_ActiveTexture(NULL),
	m_ru2D_UVMode(ru2DUVMODE_OFFSET),
	m_ru2D_EffectType(ru2DFXTYPE_NULL),
	m_ru2D_EffectShader(NULL),
	m_ru2D_MaxPrimitiveCount(84),
	m_ru2D_PrimitiveCount(0),
	m_ru2D_RefMaterial(NULL),
	m_ru2D_Mesh(NULL),
	m_ru2D_Material(NULL),
	m_ru2D_MaterialConstants(NULL)
{
	m_renderGraph = this;

	// Create reference material
	m_ru2D_RefMaterial = ruNEW CRuMaterial();

	// Initialize variables required by 2D toolkit
	INT32 ru2DVertexCount = m_ru2D_MaxPrimitiveCount * 4;
	INT32 ru2DPrimitiveCount = m_ru2D_MaxPrimitiveCount * 2;

	m_ru2D_Mesh = ruNEW CRuMesh();
	m_ru2D_Mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, ru2DVertexCount, ru2DPrimitiveCount, ruVERTEXFORMAT_DIFFUSE);

	// Construct particle mesh geometry
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_ru2D_Mesh->GetMorphTarget(0))->GetDiffuse();
	UINT16 *indices = m_ru2D_Mesh->GetIndices();

	for(INT32 i = 0, vIdx = 0, fIdx = 0; i < m_ru2D_MaxPrimitiveCount; ++i)
	{
		diffuse[vIdx++] = (i << 24) | (0x00 << 16) | (0x00 << 8);
		diffuse[vIdx++] = (i << 24) | (0x00 << 16) | (0xFF << 8);
		diffuse[vIdx++] = (i << 24) | (0xFF << 16) | (0xFF << 8);
		diffuse[vIdx++] = (i << 24) | (0xFF << 16) | (0x00 << 8);

		INT32 vertexOffset = i * 4;

		indices[fIdx++] = vertexOffset + 0;
		indices[fIdx++] = vertexOffset + 2;
		indices[fIdx++] = vertexOffset + 1;

		indices[fIdx++] = vertexOffset + 0;
		indices[fIdx++] = vertexOffset + 3;
		indices[fIdx++] = vertexOffset + 2;
	}

	m_ru2D_Mesh->IncrementMeshIteration();
	m_ru2D_Mesh->IncrementTopologyIteration();

	// Setup a proxy mesh for each possible primitive count starting with zero
	m_ru2D_Mesh->ClearBatches();
	for(INT32 i = 0; i <= m_ru2D_MaxPrimitiveCount; ++i)
	{
		// Create new batch
		m_ru2D_Mesh->AddBatch(0, i * 2);

		// Create proxy mesh for the batch
		CRuProxyMesh *proxyMesh = ruNEW CRuProxyMesh();
		proxyMesh->SetProxiedMesh(m_ru2D_Mesh, i);

		m_ru2D_ProxyMeshes.Add(proxyMesh);
	}

	// Create 2D toolkit texture atlas
	m_ru2D_TextureAtlas = ruNEW CRuTextureAtlas_Aggregate();

	m_ru2D_32BitAtlasTexture = ruNEW CRuTexture_Proxy();
	m_ru2D_32BitAtlasTexture->Allocate(1024, 1024, 1, ruSURFACEFMT_A8R8G8B8);
	m_ru2D_32BitAtlasTexture->SetTextureName("ruSCENERENDERER_ATLAS_32BIT");
	g_ruResourceManager->AddTexture(m_ru2D_32BitAtlasTexture);

	if(m_ru2D_32BitAtlasTexture)
	{
		CRuTextureAtlas_Paged *ddsAtlas = ruNEW CRuTextureAtlas_Paged();
		ddsAtlas->Initialize(m_ru2D_32BitAtlasTexture, 256, 256, 0, 0);
		m_ru2D_TextureAtlas->BindSubAtlas(ddsAtlas);
		ruSAFE_RELEASE(ddsAtlas);
	}

	m_ru2D_32BitAtlasTexture_Text = ruNEW CRuTexture_Proxy();
	m_ru2D_32BitAtlasTexture_Text->Allocate(2048, 1024, 3, ruSURFACEFMT_L8);
	m_ru2D_32BitAtlasTexture_Text->SetTextureName("ruSCENERENDERER_ATLAS_32BIT_TEXT");
	g_ruResourceManager->AddTexture(m_ru2D_32BitAtlasTexture_Text);

	if(m_ru2D_32BitAtlasTexture_Text)
	{
		CRuTextureAtlas_Paged *ddsAtlas = ruNEW CRuTextureAtlas_Paged();
		ddsAtlas->Initialize(m_ru2D_32BitAtlasTexture_Text, 256, 256, 1024, 1024);
		m_ru2D_TextureAtlas->BindSubAtlas(ddsAtlas);
		ruSAFE_RELEASE(ddsAtlas);
	}

	// Setup shader array
	m_effectShaders[ru2DFXTYPE_NULL] = g_ruShaderManager->GetShader("ru2DFXTYPE_NULL");
	m_effectShaders[ru2DFXTYPE_FLAT] = g_ruShaderManager->GetShader("ru2DFXTYPE_FLAT");
	m_effectShaders[ru2DFXTYPE_FLAT_MASKED] = g_ruShaderManager->GetShader("ru2DFXTYPE_FLAT_MASKED");
	m_effectShaders[ru2DFXTYPE_BLIT] = g_ruShaderManager->GetShader("ru2DFXTYPE_BLIT");
	m_effectShaders[ru2DFXTYPE_BLIT_ADD] = g_ruShaderManager->GetShader("ru2DFXTYPE_BLIT_ADD");
	m_effectShaders[ru2DFXTYPE_BLIT_FILTER] = g_ruShaderManager->GetShader("ru2DFXTYPE_BLIT_FILTER");
	m_effectShaders[ru2DFXTYPE_BLIT_MASK] = g_ruShaderManager->GetShader("ru2DFXTYPE_BLIT_MASK");
	m_effectShaders[ru2DFXTYPE_BLIT_MASKED] = g_ruShaderManager->GetShader("ru2DFXTYPE_BLIT_MASKED");
	m_effectShaders[ru2DFXTYPE_BLIT_MASK_Z] = g_ruShaderManager->GetShader("ru2DFXTYPE_BLIT_MASK_Z");
	m_effectShaders[ru2DFXTYPE_BLIT_LUMINANCE] = g_ruShaderManager->GetShader("ru2DFXTYPE_BLIT_LUMINANCE");
	m_effectShaders[ru2DFXTYPE_TEXTBLIT_FILTER] = g_ruShaderManager->GetShader("ru2DFXTYPE_TEXTBLIT_FILTER");
	m_effectShaders[ru2DFXTYPE_DARKEN] = g_ruShaderManager->GetShader("ru2DFXTYPE_DARKEN");
	m_effectShaders[ru2DFXTYPE_EXPONENTIATE] = g_ruShaderManager->GetShader("ru2DFXTYPE_EXPONENTIATE");
	m_effectShaders[ru2DFXTYPE_GAUSSIAN] = g_ruShaderManager->GetShader("ru2DFXTYPE_GAUSSIAN");
	m_effectShaders[ru2DFXTYPE_DISTORTION] = g_ruShaderManager->GetShader("ru2DFXTYPE_DISTORTION");

	// Set up profiler probes
	m_probe_2D_Submit = g_ruProfiler->CreateProbe("2D Submit", "Scene");
	m_probe_2D_BatchSize = g_ruProfiler->CreateProbe("2D Batch Size", "Scene");
	m_probe_2D_BatchCount = g_ruProfiler->CreateProbe("2D Batches", "Scene");
	m_probe_2D_Flush = g_ruProfiler->CreateProbe("2D Flush", "Scene");
}

CRuRendererNode_UI::~CRuRendererNode_UI()
{
	ruSAFE_RELEASE(m_renderTarget);
	ruSAFE_RELEASE(m_curRenderContext);
	delete m_lastScissorRect;

	// Release 2D reference material
	ruSAFE_RELEASE(m_ru2D_RefMaterial);

	ruSAFE_RELEASE(m_ru2D_32BitAtlasTexture);
	ruSAFE_RELEASE(m_ru2D_32BitAtlasTexture_Text);

	// Release 2d toolkit texture atlas
	ruSAFE_RELEASE(m_ru2D_TextureAtlas);

	// Release resources used by the 2D toolkit
	if(m_ru2D_Mesh)
		m_ru2D_Mesh->Release();

	for(INT32 i = 0; i < m_ru2D_ProxyMeshes.Count(); ++i)
	{
		ruSAFE_RELEASE(m_ru2D_ProxyMeshes[i]);
	}

	// Release material stack
	FlushMaterialStack();
	while(m_unusedMaterialStack.IsEmpty() == FALSE)
	{
		CRuMaterial *material = m_unusedMaterialStack.Pop();

		UINT64 instanceDataParam;
		material->GetParameterBlock()->GetParameter(ruPARAMTYPE_GEOINSTANCE_S_DATA, instanceDataParam);
		delete[] reinterpret_cast<REAL *>(instanceDataParam);

		ruSAFE_RELEASE(material);
	}

	// Release render context stack
	FlushRenderContextStack();
	while(m_unusedRenderContexts.IsEmpty() == FALSE)
	{
		CRuRendererNode_RenderContext *renderContext = m_unusedRenderContexts.Pop();
		assert(renderContext->GetRefCount() == 1);
		ruSAFE_RELEASE(renderContext);
	}
}

BOOL CRuRendererNode_UI::SetRenderTarget(IRuObject *renderTarget)
{
	if(renderTarget)
	{
		renderTarget->AddRef();
		ruSAFE_RELEASE(m_renderTarget);
		m_renderTarget = renderTarget;
	}
	else
	{
		ruSAFE_RELEASE(m_renderTarget);
	}

	return TRUE;
}

IRuTexture *CRuRendererNode_UI::GetReservedDXT5Texture()
{
	return m_ru2D_32BitAtlasTexture_Text;
}

BOOL CRuRendererNode_UI::IsolationRender_Begin()
{
	m_renderGraph = ruNEW CRuRendererNode();
	return TRUE;
}

CRuRendererNode *CRuRendererNode_UI::IsolationRender_End()
{
	CRuRendererNode *retNode = m_renderGraph;
	m_renderGraph = this;

	return retNode;
}

BOOL CRuRendererNode_UI::RenderTarget_Push(IRuObject *renderTarget)
{
	if ( renderTarget == NULL )
		return FALSE;

	// Terminate the current render context
	TerminateRenderContext();

	// Push old render target onto stack
	m_ru2D_RTStack.Push(m_renderTarget);

	// Set new render target
	renderTarget->AddRef();
	m_renderTarget = renderTarget;

	// Allocate new render context
	AllocateRenderContext();

	return TRUE;
}

BOOL CRuRendererNode_UI::RenderTarget_Pop()
{
	// Terminate the current render context
	TerminateRenderContext();

	// Release current render target
	ruSAFE_RELEASE(m_renderTarget);

	// Pop render target off stack
	m_renderTarget = m_ru2D_RTStack.Pop();

	// Allocate new render context
	AllocateRenderContext();

	return TRUE;
}

BOOL CRuRendererNode_UI::Viewport_Push(IRuViewport *viewport)
{
	// Terminate the current render context
	TerminateRenderContext();

	// Allocate new render context
	AllocateRenderContext();

	// Push viewport onto stack
	m_ru2D_ViewportStack.Push(viewport);

	// Set viewport for the current render context
	m_curRenderContext->SetViewport(m_ru2D_ViewportStack.Peek());

	return TRUE;
}

BOOL CRuRendererNode_UI::Viewport_Pop()
{
	// Terminate the current render context
	TerminateRenderContext();

	// Allocate new render context
	AllocateRenderContext();

	// Pop viewport from stack
	m_ru2D_ViewportStack.Pop();

	// Set viewport for the current render context
	m_curRenderContext->SetViewport(m_ru2D_ViewportStack.IsEmpty() ? NULL : m_ru2D_ViewportStack.Peek());

	return TRUE;
}

BOOL CRuRendererNode_UI::SetScissorRect(CRuRectangle *scissorRect)
{
	// Terminate the current render context
	TerminateRenderContext();
/*
	// Create a set scissor node
	CRuRendererNode_SetScissor *setScissor = ruNEW CRuRendererNode_SetScissor();

	if(scissorRect)
	{
		// Convert scissor coordinates...
		CRuRectangle temp(scissorRect->m_left, scissorRect->m_top, scissorRect->m_right, scissorRect->m_bottom);
		ConvertCoordinates(temp);

		setScissor->SetScissorRect(&temp);
	}
	else
	{
		setScissor->SetScissorRect(NULL);
	}

	m_renderGraph->AddChild(setScissor);
	ruSAFE_RELEASE(setScissor);
*/
	// Allocate new render context
	AllocateRenderContext();

	if(scissorRect)
	{
		// Convert scissor coordinates...
		CRuRectangle temp(scissorRect->m_left, scissorRect->m_top, scissorRect->m_right, scissorRect->m_bottom);
		ConvertCoordinates(temp);

		// Set scissor rect into the new render context
		m_curRenderContext->SetScissorRect(&temp);

		delete m_lastScissorRect;
		m_lastScissorRect = ruNEW CRuRectangle();
		*m_lastScissorRect = *scissorRect;
	}
	else
	{
		m_curRenderContext->SetScissorRect(NULL);

		delete m_lastScissorRect;
		m_lastScissorRect = NULL;
	}

	return TRUE;
}

BOOL CRuRendererNode_UI::ClearViewport(IRuViewport *viewport)
{
	// Terminate the current render context
	TerminateRenderContext();

	CRuRendererNode_ViewportClear *viewportClear = ruNEW CRuRendererNode_ViewportClear();
	viewportClear->SetViewport(viewport);
	viewportClear->SetParentRenderer(m_parentRenderer);

	m_renderGraph->AddChild(viewportClear);
	ruSAFE_RELEASE(viewportClear);

	// Allocate new render context
	AllocateRenderContext();

	return TRUE;
}

BOOL CRuRendererNode_UI::Camera_Push(CRuCamera *camera)
{
	m_cameraStackDirty = TRUE;
	m_cameraStack.Push(camera);
	return TRUE;
}

BOOL CRuRendererNode_UI::Camera_Pop()
{
	if(m_cameraStack.IsEmpty() == FALSE)
	{
		m_cameraStackDirty = TRUE;
		m_cameraStack.Pop();
		return TRUE;
	}

	return FALSE;
}

BOOL CRuRendererNode_UI::ViewportScale_Push(REAL width, REAL height)
{
	// Push current 2D viewport size onto stack
	m_ru2D_Viewport_Width_Stack.Push(m_ru2D_Viewport_Width);
	m_ru2D_Viewport_Height_Stack.Push(m_ru2D_Viewport_Height);

	// Set new 2D viewport size
	m_ru2D_Viewport_Width = width;
	m_ru2D_Viewport_Height = height;

	return TRUE;
}

BOOL CRuRendererNode_UI::ViewportScale_Pop()
{
	if(m_ru2D_Viewport_Width_Stack.IsEmpty() == FALSE && m_ru2D_Viewport_Height_Stack.IsEmpty() == FALSE)
	{
		// Pop previous 2D viewport size off stack
		m_ru2D_Viewport_Width = m_ru2D_Viewport_Width_Stack.Pop();
		m_ru2D_Viewport_Height = m_ru2D_Viewport_Height_Stack.Pop();

		return TRUE;
	}

	return FALSE;
}

/*!
	Sets the current uv mapping mode for the 2D toolkit. This takes effect
	immediately, does not flush the rendering pipeline, and will only affect
	new 2D submissions.

	@retval TRUE The operation succeeded.
	@retval FALSE The operation failed.
*/
BOOL CRuRendererNode_UI::SetUVMode(Ru2D_UVMode uvMode)
{
	m_ru2D_UVMode = uvMode;

	return TRUE;
}

/*!
	Sets the current effect type for the 2D toolkit. Calling this function will cause the
	renderer buffer to be flushed.

	@retval TRUE The operation succeeded.
	@retval FALSE The operation failed.
*/
BOOL CRuRendererNode_UI::SetEffectType(Ru2D_EffectType effectType)
{
	if(m_ru2D_EffectType != effectType)
	{
		// Flush current batch
		RenderBatch();

		// Store effect type
		m_ru2D_EffectType = effectType;

		// Select the active shader based on effect type
		m_ru2D_EffectShader = m_effectShaders[effectType];

		// Set default parameters
		switch(effectType)
		{
			default:
			case ru2DFXTYPE_FLAT:
			case ru2DFXTYPE_FLAT_MASKED:
			case ru2DFXTYPE_BLIT:
			case ru2DFXTYPE_BLIT_ADD:
			case ru2DFXTYPE_BLIT_FILTER:
			case ru2DFXTYPE_BLIT_MASK:
			case ru2DFXTYPE_BLIT_MASKED:
			case ru2DFXTYPE_BLIT_MASK_Z:
			case ru2DFXTYPE_BLIT_LUMINANCE:
			case ru2DFXTYPE_TEXTBLIT_FILTER:
			case ru2DFXTYPE_DISTORTION:
				{
					m_ru2D_DiffuseColor = 0xFFFFFFFF;
					m_ru2D_StencilID = 1;
				}
				break;

			case ru2DFXTYPE_DARKEN:
			case ru2DFXTYPE_EXPONENTIATE:
			case ru2DFXTYPE_GAUSSIAN:
				{
					m_ru2D_DiffuseColor = 0x7777777F;
					m_ru2D_StencilID = 1;
				}
				break;
		}
	}

	// If the shader was properly selected, return TRUE
	if(m_ru2D_EffectShader)
	{
		m_ru2D_Material->SetShader(m_ru2D_EffectShader);
		m_ru2D_RefMaterial->SetShader(m_ru2D_EffectShader);

		return TRUE;
	}

	return FALSE;
}

/*!
	Sets an effect parameter.

	@param effectParam The effect parameter to set.
	@param parameter The value to set
	@retval TRUE The operation succeeded.
	@retval FALSE The operation failed.
*/
BOOL CRuRendererNode_UI::SetEffectParameter(Ru2D_EffectParameter effectParam, DWORD parameter)
{
	switch(effectParam)
	{
		case ru2DFXPARAM_STRENGTH:	// Parameter 0% to 100%
			{
				UINT8 strength = (UINT8) ((parameter / 100.0f) * 255.0f);
				m_ru2D_DiffuseColor = RUCOLOR_ARGB(strength, strength, strength, strength);
			}

			break;
		case ru2DFXPARAM_COLOR:
			{
				m_ru2D_DiffuseColor = parameter;
			}

			break;

		case ru2DFXPARAM_STENCILID:
			{
				m_ru2D_StencilID = static_cast<UINT8>(parameter);
			}

			break;
	}

	return TRUE;
}

/*!
	Bypass the 2D toolkit interface and set a parameter directly into the 2D effect shader.
	Under normal circumstances, this function should never be invoked, as the function will
	fail if the shader does not support the requested parameter.

	@param paramType The shader parameter to set.
	@param parameter The value to set
	@retval TRUE The operation succeeded.
	@retval FALSE The operation failed.
*/
BOOL CRuRendererNode_UI::SetShaderParameter(RuParameterType paramType, UINT64 param)
{
	m_ru2D_Material->GetParameterBlock()->SetParameter(paramType, param);

	return TRUE;
}

/*!
	Sets the current texture used by the 2D toolkit. Calling this function will
	cause the renderer buffer to be flushed if the specified texture changes
	the texture at the stage.

	@param texture The texture to set into the texture channel.
	@param texChannel The texture channel whose texture will be changed.
	@retval TRUE The operation succeeded.
	@retval FALSE The operation failed.
*/
BOOL CRuRendererNode_UI::SetTexture(IRuTexture *texture, BOOL allowBinding, RuTextureChannel texChannel)
{
	if(m_ru2D_ActiveTexture != texture)
	{
		IRuTexture *boundTexture = NULL;

		if(texture)
		{
			if(allowBinding)
			{
				// Store active texture handle
				m_ru2D_ActiveTextureHandle = m_ru2D_TextureAtlas->BindIntoAtlas(texture, FALSE);
				m_ru2D_ActiveTextureWidth = m_ru2D_TextureAtlas->GetTextureWidth(m_ru2D_ActiveTextureHandle);
				m_ru2D_ActiveTextureHeight = m_ru2D_TextureAtlas->GetTextureHeight(m_ru2D_ActiveTextureHandle);
				boundTexture = m_ru2D_TextureAtlas->GetTexture(m_ru2D_ActiveTextureHandle);
			}
			else
			{
				m_ru2D_ActiveTextureHandle = -1;
				boundTexture = texture;
			}
		}

		// Flush current batch if necessary
		if(m_ru2D_Material->GetTexture(texChannel) != boundTexture)
		{
			RenderBatch();
		}

		// Set texture into texture channel
		m_ru2D_Material->SetTexture(texChannel, boundTexture);
		m_ru2D_RefMaterial->SetTexture(texChannel, boundTexture);

		if(texture)
		{
			// Set reference texture dimensions
			switch(texChannel)
			{
				// Normally, use the [0, texture size] as [0, 1] in our UV coordinates
				case ruTEXCHANNEL_DIFFUSEMAP:
				case ruTEXCHANNEL_TERRAIN_ALPHA_0:
				case ruTEXCHANNEL_TERRAIN_ALPHA_1:
					{
						m_ru2D_RefTextureWidth = texture->GetLevelWidth(0);
						m_ru2D_RefTextureHeight = texture->GetLevelHeight(0);
					}

					break;

				// In the case of terrain layers, do not alter the reference texture size
				case ruTEXCHANNEL_TERRAIN_LAYER_0:
				case ruTEXCHANNEL_TERRAIN_LAYER_1:
				case ruTEXCHANNEL_TERRAIN_LAYER_2:
				case ruTEXCHANNEL_TERRAIN_LAYER_3:
				case ruTEXCHANNEL_TERRAIN_LAYER_4:
				case ruTEXCHANNEL_TERRAIN_LAYER_5:
					break;
			}
		}
		else
		{
			// If the texture is NULL, then just pick sensible texture width and height values to use in UV calculations
			if(m_ru2D_RefTextureWidth == 0 || m_ru2D_RefTextureHeight == 0)
			{
				m_ru2D_RefTextureWidth = 4;
				m_ru2D_RefTextureHeight = 4;
			}
		}

		m_ru2D_ActiveTexture = texture;
	}

	return TRUE;
}

BOOL CRuRendererNode_UI::SetRotation(REAL rotation)
{
	// Flush current batch if necessary
	if(-rotation != m_ru2D_Rotation)
	{
		RenderBatch();
	}

	// Store new rotation
	m_ru2D_Rotation = -rotation;

	return TRUE;
}

/*!
	Submits a single rectangle to the 2D toolkit for rendering.

	@param srcRect Source area of the texture to render from, in screen coordinates.
	@param destRect Destination area of the render target to render to, in screen coordinates.
*/
void CRuRendererNode_UI::Submit(const CRuRectangle &srcRect, const CRuRectangle &destRect)
{
	Submit(1, &srcRect, &destRect, NULL);
}

/*!
	Submits a list of rectangles to the 2D toolkit for rendering.

	@param numRects Number of rectangles to submit.
	@param srcRect Source area of the texture to render from, in screen coordinates.
	@param destRect Destination area of the render target to render to, in screen coordinates.
	@param color List of RUCOLOR values to use for each rectangular region. If NULL, then the color set by ru2DFXPARAM_COLOR is used.
*/
void CRuRendererNode_UI::Submit(INT32 numRects, const CRuRectangle *srcRect, const CRuRectangle *destRect, const DWORD *color)
{
	// Accumulate batch count
//	g_ruProfiler->AccumulateProbeValue(m_ru2D_PrimitiveCount, static_cast<REAL>(1));


	REAL baseGeometryOffset = 0.5f;
	REAL baseTexelOffset = 0.75f;

	switch(m_ru2D_UVMode)
	{
		case ru2DUVMODE_OFFSET:
			baseGeometryOffset = 0.5f;
			baseTexelOffset = 1.0f;
			break;
		case ru2DUVMODE_DIRECT:
			baseGeometryOffset = 0.0f;
			baseTexelOffset = 0.0f;
			break;
	}

	// Iterate through all rectangles
	for(INT32 i = 0; i < numRects; ++i)
	{
		// Flush current batch if necessary
		if(m_ru2D_PrimitiveCount >= m_ru2D_MaxPrimitiveCount)
		{
			RenderBatch();
		}

		// Perform coordinate conversion
		CRuRectangle src = srcRect[i];
		CRuRectangle dst = destRect[i];

		ConvertCoordinates(dst);

		// Calculate destination vertices

		REAL pixelOffsetX = baseGeometryOffset / m_ru2D_RefRTWidth;
		REAL pixelOffsetY = baseGeometryOffset / m_ru2D_RefRTHeight;

		REAL x0 = ((dst.m_left / m_ru2D_RefRTWidth) - 0.5f) * 2.0f - pixelOffsetX;
		REAL x1 = ((dst.m_right / m_ru2D_RefRTWidth) - 0.5f) * 2.0f - pixelOffsetX;
		REAL y0 = -(((dst.m_top / m_ru2D_RefRTHeight) - 0.5f) * 2.0f - pixelOffsetY);
		REAL y1 = -(((dst.m_bottom / m_ru2D_RefRTHeight) - 0.5f) * 2.0f - pixelOffsetY);

		// Calculate source UV
		REAL u0 = src.m_left / (REAL) m_ru2D_RefTextureWidth;
		REAL u1 = src.m_right / (REAL) m_ru2D_RefTextureWidth;
		REAL v0 = src.m_top / (REAL) m_ru2D_RefTextureHeight;
		REAL v1 = src.m_bottom / (REAL) m_ru2D_RefTextureHeight;

		if(m_ru2D_ActiveTextureHandle >= 0)
		{
			// Choose UV offset based on whether we do linear filtering or not
			switch(m_ru2D_EffectType)
			{
				default:
					m_ru2D_TextureAtlas->TranslateUV(m_ru2D_ActiveTextureHandle, src.m_left / (REAL) m_ru2D_ActiveTextureWidth, src.m_top / (REAL) m_ru2D_ActiveTextureHeight, 0.0f, 0.0f, baseTexelOffset, u0, v0);
					m_ru2D_TextureAtlas->TranslateUV(m_ru2D_ActiveTextureHandle, src.m_right / (REAL) m_ru2D_ActiveTextureWidth, src.m_bottom / (REAL) m_ru2D_ActiveTextureHeight, 0.0f, 0.0f, baseTexelOffset, u1, v1);
					break;

				case ru2DFXTYPE_BLIT_FILTER:
				case ru2DFXTYPE_BLIT_ADD:
				case ru2DFXTYPE_BLIT_MASK:
				case ru2DFXTYPE_BLIT_MASKED:
				case ru2DFXTYPE_BLIT_MASK_Z:
				case ru2DFXTYPE_TEXTBLIT_FILTER:
				case ru2DFXTYPE_GAUSSIAN:
				case ru2DFXTYPE_DISTORTION:
					m_ru2D_TextureAtlas->TranslateUV(m_ru2D_ActiveTextureHandle, src.m_left / (REAL) m_ru2D_ActiveTextureWidth, src.m_top / (REAL) m_ru2D_ActiveTextureHeight, 1.0f, 1.0f, baseTexelOffset, u0, v0);
					m_ru2D_TextureAtlas->TranslateUV(m_ru2D_ActiveTextureHandle, src.m_right / (REAL) m_ru2D_ActiveTextureWidth, src.m_bottom / (REAL) m_ru2D_ActiveTextureHeight, -1.0f, -1.0f, baseTexelOffset, u1, v1);
					break;
			}
		}
		else
		{
			// Choose UV offset based on whether we do linear filtering or not
			switch(m_ru2D_EffectType)
			{
				default:
					u0 = src.m_left / (REAL) m_ru2D_RefTextureWidth;
					u1 = src.m_right / (REAL) m_ru2D_RefTextureWidth;
					v0 = src.m_top / (REAL) m_ru2D_RefTextureHeight;
					v1 = src.m_bottom / (REAL) m_ru2D_RefTextureHeight;
					break;

				case ru2DFXTYPE_BLIT_FILTER:
				case ru2DFXTYPE_BLIT_ADD:
				case ru2DFXTYPE_TEXTBLIT_FILTER:
					{
						REAL texelOffsetX = 0.0f / m_ru2D_RefTextureWidth;
						REAL texelOffsetY = 0.0f / m_ru2D_RefTextureHeight;

						u0 = src.m_left / (REAL) m_ru2D_RefTextureWidth + texelOffsetX;
						u1 = src.m_right / (REAL) m_ru2D_RefTextureWidth + texelOffsetX;
						v0 = src.m_top / (REAL) m_ru2D_RefTextureHeight + texelOffsetY;
						v1 = src.m_bottom / (REAL) m_ru2D_RefTextureHeight + texelOffsetY;
					}

					break;
			}

		}

		INT32 constantOffset = m_ru2D_PrimitiveCount * 12;
		DWORD vertexColor = m_ru2D_DiffuseColor;

		if(color)
		{
			vertexColor = color[i];
		}

		m_ru2D_MaterialConstants[constantOffset++] = x0;
		m_ru2D_MaterialConstants[constantOffset++] = u0;
		m_ru2D_MaterialConstants[constantOffset++] = y0;
		m_ru2D_MaterialConstants[constantOffset++] = v0;

		m_ru2D_MaterialConstants[constantOffset++] = x1;
		m_ru2D_MaterialConstants[constantOffset++] = u1;
		m_ru2D_MaterialConstants[constantOffset++] = y1;
		m_ru2D_MaterialConstants[constantOffset++] = v1;

		m_ru2D_MaterialConstants[constantOffset++] = static_cast<REAL>((vertexColor >> 16) & 0x000000FF) / 255.0f;
		m_ru2D_MaterialConstants[constantOffset++] = static_cast<REAL>((vertexColor >>  8) & 0x000000FF) / 255.0f;
		m_ru2D_MaterialConstants[constantOffset++] = static_cast<REAL>((vertexColor >>  0) & 0x000000FF) / 255.0f;
		m_ru2D_MaterialConstants[constantOffset++] = static_cast<REAL>((vertexColor >> 24) & 0x000000FF) / 255.0f;

		// Update primitive information
		++m_ru2D_PrimitiveCount;
	}

	// End time sampling for the flush
//	RUPROFILE_SAMPLE_END(m_probe_2D_Submit, 2);
}

void CRuRendererNode_UI::Flush()
{
	// Flush the last batch to render queue
	RenderBatch();
}

void CRuRendererNode_UI::Submit(CRuEntity *entity)
{
	// Terminate the current render context
	TerminateRenderContext();

	// Allocate new render context
	AllocateRenderContext();

	// Ensure render context settings are up-to-date
	UpdateRenderContextSettings();

	// Perform submission
	m_curRenderContext->Submit(entity);

	// Terminate the current render context
	TerminateRenderContext();

	// Allocate new render context
	AllocateRenderContext();
}

void CRuRendererNode_UI::Submit(IRuMesh *mesh, IRuMaterial *material)
{
	// Terminate the current render context
	TerminateRenderContext();

	// Allocate new render context
	AllocateRenderContext();

	// Ensure render context settings are up-to-date
	UpdateRenderContextSettings();

	// Perform submission
	m_curRenderContext->Submit(NULL, mesh, material);

	// Terminate the current render context
	TerminateRenderContext();

	// Allocate new render context
	AllocateRenderContext();
}

void CRuRendererNode_UI::ConvertCoordinates(CRuVector3 &point)
{
	if(m_ru2D_Viewport_Width > 0.0f && m_ru2D_Viewport_Height > 0.0f)
	{
		REAL xFactor = m_ru2D_RefRTWidth / m_ru2D_Viewport_Width;
		REAL yFactor = m_ru2D_RefRTHeight / m_ru2D_Viewport_Height;

//		point.m_x = floor(point.m_x * xFactor);
//		point.m_y = floor(point.m_y * yFactor);

		point.m_x = (point.m_x * xFactor);
		point.m_y = (point.m_y * yFactor);
	}
}

void CRuRendererNode_UI::ConvertCoordinates(CRuRectangle &rectangle)
{
	if(m_ru2D_Viewport_Width > 0.0f && m_ru2D_Viewport_Height > 0.0f)
	{
		REAL xFactor = m_ru2D_RefRTWidth / m_ru2D_Viewport_Width;
		REAL yFactor = m_ru2D_RefRTHeight / m_ru2D_Viewport_Height;

//		rectangle.m_left = floor(rectangle.m_left * xFactor);
//		rectangle.m_right = floor(rectangle.m_right * xFactor);
//		rectangle.m_top = floor(rectangle.m_top * yFactor);
//		rectangle.m_bottom = floor(rectangle.m_bottom * yFactor);

		rectangle.m_left = (rectangle.m_left * xFactor);
		rectangle.m_right = (rectangle.m_right * xFactor);
		rectangle.m_top = (rectangle.m_top * yFactor);
		rectangle.m_bottom = (rectangle.m_bottom * yFactor);
	}
}

/*!
	Converts a point from world coordinate from to viewport coordinate frame

	@param camera Source camera to use for conversion.
	@param point Point to convert.
*/
void CRuRendererNode_UI::WorldToViewport(CRuCamera *camera, CRuVector3 &point)
{
	// Transform point into homogeneous coordinates
	const CRuMatrix4x4 &frustumTransform = camera->GetFrustumTransform();
	frustumTransform.TransformPointProjected(point);

	// Convert coordinates into viewport scale
	if(m_ru2D_Viewport_Width > 0.0f && m_ru2D_Viewport_Height > 0.0f)
	{
		point.m_x = ((point.m_x + 1.0f) / 2.0f) * m_ru2D_Viewport_Width;
		point.m_y = ((-point.m_y + 1.0f) / 2.0f) * m_ru2D_Viewport_Height;
		point.m_z = 0.0f;
	}
	else
	{
		point.m_x = ((point.m_x + 1.0f) / 2.0f) * m_ru2D_RefRTWidth;
		point.m_y = ((-point.m_y + 1.0f) / 2.0f) * m_ru2D_RefRTHeight;
		point.m_z = 0.0f;
	}
}

void CRuRendererNode_UI::WorldToScreen(CRuCamera *camera, CRuVector3 &point)
{
	// Transform point into homogeneous coordinates
	const CRuMatrix4x4 &frustumTransform = camera->GetFrustumTransform();
	frustumTransform.TransformPointProjected(point);

	// Convert coordinates into screen scale
	point.m_x = ((point.m_x + 1.0f) / 2.0f) * m_ru2D_RefRTWidth;
	point.m_y = ((-point.m_y + 1.0f) / 2.0f) * m_ru2D_RefRTHeight;
	point.m_z = 0.0f;
}

void CRuRendererNode_UI::HomogeneousToViewport(CRuVector3 &point)
{
	REAL pixelOffsetX = 0.5f / m_ru2D_RefRTWidth;
	REAL pixelOffsetY = 0.5f / m_ru2D_RefRTHeight;

	point.m_x = ((point.m_x + pixelOffsetX) / 2.0f + 0.5f) * m_ru2D_RefRTWidth;
	point.m_y = ((-point.m_y + pixelOffsetY) / 2.0f + 0.5f) * m_ru2D_RefRTHeight;
}

void CRuRendererNode_UI::HomogeneousToViewport(CRuRectangle &rectangle)
{
	REAL pixelOffsetX = 0.5f / m_ru2D_RefRTWidth;
	REAL pixelOffsetY = 0.5f / m_ru2D_RefRTHeight;

	rectangle.m_left = ((rectangle.m_left + pixelOffsetX) / 2.0f + 0.5f) * m_ru2D_RefRTWidth;
	rectangle.m_right = ((rectangle.m_right + pixelOffsetX) / 2.0f + 0.5f) * m_ru2D_RefRTWidth;
	rectangle.m_top = ((-rectangle.m_top + pixelOffsetY) / 2.0f + 0.5f) * m_ru2D_RefRTHeight;
	rectangle.m_bottom = ((-rectangle.m_bottom + pixelOffsetY) / 2.0f + 0.5f) * m_ru2D_RefRTHeight;
}

void CRuRendererNode_UI::ViewportToHomogeneous(CRuVector3 &point)
{
	REAL pixelOffsetX = 0.5f / m_ru2D_RefRTWidth;
	REAL pixelOffsetY = 0.5f / m_ru2D_RefRTHeight;

	point.m_x = ((point.m_x / m_ru2D_RefRTWidth) - 0.5f) * 2.0f - pixelOffsetX;
	point.m_y = -(((point.m_y / m_ru2D_RefRTHeight) - 0.5f) * 2.0f - pixelOffsetY);
}

void CRuRendererNode_UI::ViewportToHomogeneous(CRuRectangle &rectangle)
{
	REAL pixelOffsetX = 0.5f / m_ru2D_RefRTWidth;
	REAL pixelOffsetY = 0.5f / m_ru2D_RefRTHeight;

	rectangle.m_left = ((rectangle.m_left / m_ru2D_RefRTWidth) - 0.5f) * 2.0f - pixelOffsetX;
	rectangle.m_right = ((rectangle.m_right / m_ru2D_RefRTWidth) - 0.5f) * 2.0f - pixelOffsetX;
	rectangle.m_top = -(((rectangle.m_top / m_ru2D_RefRTHeight) - 0.5f) * 2.0f - pixelOffsetY);
	rectangle.m_bottom = -(((rectangle.m_bottom / m_ru2D_RefRTHeight) - 0.5f) * 2.0f - pixelOffsetY);
}

UINT8 CRuRendererNode_UI::GetNextStencilID()
{
	++m_nextStencilID;
	return m_nextStencilID;
}

CRuRendererNode_RenderContext *CRuRendererNode_UI::GetCurrentRenderContext()
{
	return m_curRenderContext;
}

void CRuRendererNode_UI::SetParentRenderer(IRuObject *renderer)
{
	// Chain call to base class
	CRuRendererNode::SetParentRenderer(renderer);

	// Store pointer to the parent renderer
	m_parentRenderer = renderer;
}

void CRuRendererNode_UI::Begin()
{
	// Chain call to base class
	CRuRendererNode::Begin();

	// Setup initial render context
	AllocateRenderContext();

	// Refresh pointers to each component
	m_ru2D_Mesh_Position = m_ru2D_Mesh->GetMorphTarget(0)->GetPosition();
	m_ru2D_Mesh_Diffuse = m_ru2D_Mesh->GetMorphTarget(0)->GetDiffuse();
	m_ru2D_Mesh_UV0 = m_ru2D_Mesh->GetMorphTarget(0)->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
	m_ru2D_Mesh_Indices = m_ru2D_Mesh->GetIndices();

	// Set reference render target dimensions
	g_ruEngine->GetRenderTargetSize(m_ru2D_RefRTWidth, m_ru2D_RefRTHeight);

	// Reset UV mode
	m_ru2D_UVMode = ru2DUVMODE_OFFSET;

	// Reset effect type
	m_ru2D_EffectType = ru2DFXTYPE_FLAT;

	m_ru2D_Rotation = 0.0f;

	// Allocate new material for use by the 2D system
	if(m_ru2D_Material == NULL)
	{
		AllocateMaterial();
	}

	// Reset next stencil ID
	m_nextStencilID = 0;
}

void CRuRendererNode_UI::End()
{
	// Chain call to base class
	CRuRendererNode::End();

	// Detach all children
	this->RemoveAllChildren();

	// Release current render context
	ruSAFE_RELEASE(m_curRenderContext);

	// Clear camera stack
	m_cameraStack.Clear();

	// Flush material stack
	FlushMaterialStack();

	// Flush render context stack
	FlushRenderContextStack();

	// Clear 2D viewport size stack
	m_ru2D_Viewport_Width_Stack.Clear();
	m_ru2D_Viewport_Height_Stack.Clear();
}

void CRuRendererNode_UI::Execute(CRuKeyHash<PTRVALUE> &executionMap)
{
	// Make sure we flush the last batch
	Flush();

	// Chain call to base class
	CRuRendererNode::Execute(executionMap);
}

BOOL CRuRendererNode_UI::AllocateRenderContext()
{
	if(m_curRenderContext == NULL)
	{
		// Create render context
		m_curRenderContext = m_unusedRenderContexts.IsEmpty() ? ruNEW CRuRendererNode_RenderContext() : m_unusedRenderContexts.Pop();

		// Push render context onto used stack
		m_curRenderContext->AddRef();
		m_usedRenderContexts.Push(m_curRenderContext);

		// Setup render context
		m_curRenderContext->SetParentRenderer(m_parentRenderer);
		m_curRenderContext->SetRenderTarget(m_renderTarget);
		m_curRenderContext->SetDefaultClearParameters(0, 0, 0.0f, 0);
		m_curRenderContext->SetViewport(m_ru2D_ViewportStack.IsEmpty() ? NULL : m_ru2D_ViewportStack.Peek());

		if(m_lastScissorRect)
		{
			// Convert scissor coordinates...
			CRuRectangle temp(m_lastScissorRect->m_left, m_lastScissorRect->m_top, m_lastScissorRect->m_right, m_lastScissorRect->m_bottom);
			ConvertCoordinates(temp);

			// Set scissor rect into the new render context
			m_curRenderContext->SetScissorRect(&temp);
		}
		else
		{
			m_curRenderContext->SetScissorRect(NULL);
		}

		// Disable queue sorting
		m_curRenderContext->EnableRenderQueueSorting(FALSE);

		// Forcibly invoke begin
		m_curRenderContext->Begin();

		// Attach to node graph
		m_renderGraph->AddChild(m_curRenderContext);

		// Mark camera stack as dirty
		m_cameraStackDirty = TRUE;

		// Update the render target dimensions
		if(m_renderTarget->GetType().IsSubClassOf(IRuSurface::Type()))
		{
			m_ru2D_RefRTWidth = static_cast<IRuSurface *>(m_renderTarget)->GetWidth();
			m_ru2D_RefRTHeight = static_cast<IRuSurface *>(m_renderTarget)->GetHeight();
		}
		else if(m_renderTarget->GetType().IsSubClassOf(IRuSwapChain::Type()))
		{
			m_ru2D_RefRTWidth = static_cast<IRuSwapChain *>(m_renderTarget)->GetWidth();
			m_ru2D_RefRTHeight = static_cast<IRuSwapChain *>(m_renderTarget)->GetHeight();
		}
		else if(m_renderTarget->GetType().IsSubClassOf(IRuTexture::Type()))
		{
			m_ru2D_RefRTWidth = static_cast<IRuTexture *>(m_renderTarget)->GetLevelWidth(0);
			m_ru2D_RefRTHeight = static_cast<IRuTexture *>(m_renderTarget)->GetLevelHeight(0);
		}
		else if(m_renderTarget->GetType().IsSubClassOf(IRuTextureView::Type()))
		{
			IRuTextureView *textureView = static_cast<IRuTextureView *>(m_renderTarget);
			IRuTexture *rtTexture = textureView->GetTexture();
			IRuViewport *viewport = textureView->GetViewport();

			if(rtTexture)
			{
				m_ru2D_RefRTWidth = rtTexture->GetLevelWidth(0);
				m_ru2D_RefRTHeight = rtTexture->GetLevelHeight(0);
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			// Log an error
			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuRendererNode_UI::TerminateRenderContext()
{
	// Flush
	RenderBatch();

	// Release reference to the current render context
	ruSAFE_RELEASE(m_curRenderContext);

	return TRUE;
}

BOOL CRuRendererNode_UI::UpdateRenderContextSettings()
{
	if(m_cameraStackDirty && m_curRenderContext)
	{
		// Pop cameras off camera stack
		CRuStack<CRuCamera *> flushQueue;

		while(m_cameraStack.IsEmpty() == FALSE)
		{
			flushQueue.Push(m_cameraStack.Pop());
		}

		// Push cameras back on to camera stack
		while(flushQueue.IsEmpty() == FALSE)
		{
			CRuCamera *camera = flushQueue.Pop();
			m_cameraStack.Push(camera);
			m_curRenderContext->Camera_Push(camera);
		}

		// Mark camera stack as not dirty
		m_cameraStackDirty = FALSE;

		return TRUE;
	}

	return FALSE;
}

void CRuRendererNode_UI::FlushRenderContextStack()
{
	while(m_usedRenderContexts.IsEmpty() == FALSE)
	{
		m_unusedRenderContexts.Push(m_usedRenderContexts.Pop());
	}
}

CRuMaterial *CRuRendererNode_UI::AllocateMaterial()
{
	CRuMaterial *material = NULL;

	// Get a material object
	if(m_unusedMaterialStack.IsEmpty())
	{
		material = ruNEW CRuMaterial();

		REAL *instanceData = ruNEW REAL [256 * 4];
		material->GetParameterBlock()->SetParameter(ruPARAMTYPE_GEOINSTANCE_S_DATA, reinterpret_cast<UINT64>(instanceData));
	}
	else
	{
		material = m_unusedMaterialStack.Pop();
	}

	// Push material object onto used stack
	m_usedMaterialStack.Push(material);

	// Copy textures from the reference material
	for(INT32 i = 0; i < m_ru2D_RefMaterial->GetNumTextureChannels(); ++i)
	{
		RuTextureChannel texChannel = m_ru2D_RefMaterial->GetTextureChannel(i);
		IRuBaseTexture *texture = m_ru2D_RefMaterial->GetTexture(texChannel);

		material->SetTexture(texChannel, texture);
	}

	// Copy shader from the reference material
	material->SetShader(m_ru2D_RefMaterial->GetShader());

	// Set material pointer
	m_ru2D_Material = material;

	// Set instance data pointer
	UINT64 instanceDataParam;
	m_ru2D_Material->GetParameterBlock()->GetParameter(ruPARAMTYPE_GEOINSTANCE_S_DATA, instanceDataParam);
	m_ru2D_MaterialConstants = reinterpret_cast<REAL *>(instanceDataParam);

	return material;
}

void CRuRendererNode_UI::FlushMaterialStack()
{
	while(m_usedMaterialStack.IsEmpty() == FALSE)
	{
		m_unusedMaterialStack.Push(m_usedMaterialStack.Pop());
	}

	m_ru2D_Material = NULL;
	m_ru2D_MaterialConstants = NULL;
}

void CRuRendererNode_UI::RenderBatch()
{
	if(m_ru2D_PrimitiveCount > 0)
	{
		// Accumulate batch size
		g_ruProfiler->AccumulateProbeValue(m_probe_2D_BatchSize, static_cast<REAL>(m_ru2D_PrimitiveCount));

		// Accumulate batch count
		g_ruProfiler->AccumulateProbeValue(m_probe_2D_BatchCount, 1.0f);

		// Begin time sampling for the flush
//		RUPROFILE_SAMPLE_BEGIN(m_probe_2D_Flush, 0);

		CRuParameterBlock *paramBlock = m_ru2D_Material->GetParameterBlock();

		IRuBaseTexture *texture = m_ru2D_Material->GetTexture(ruTEXCHANNEL_DIFFUSEMAP);
		if(texture)
		{
			paramBlock->SetParameter(ruPARAMTYPE_2DFX_TEXTURE_WIDTH, static_cast<IRuTexture *>(texture)->GetLevelWidth(0));
			paramBlock->SetParameter(ruPARAMTYPE_2DFX_TEXTURE_HEIGHT, static_cast<IRuTexture *>(texture)->GetLevelHeight(0));
		}
		else
		{
			paramBlock->SetParameter(ruPARAMTYPE_2DFX_TEXTURE_WIDTH, 512);
			paramBlock->SetParameter(ruPARAMTYPE_2DFX_TEXTURE_HEIGHT, 512);
		}

		// Set rendering parameters
		REAL aspectRatio_XY = (REAL) m_ru2D_RefRTWidth / m_ru2D_RefRTHeight;

		paramBlock->SetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, m_ru2D_PrimitiveCount);
		paramBlock->SetParameter(ruPARAMTYPE_CUSTOM_0, RuCastFloatToINT64(m_ru2D_Rotation));
		paramBlock->SetParameter(ruPARAMTYPE_CUSTOM_1, RuCastFloatToINT64(aspectRatio_XY));
		paramBlock->SetParameter(ruPARAMTYPE_CUSTOM_2, RuCastFloatToINT64(sin(m_ru2D_Rotation)));
		paramBlock->SetParameter(ruPARAMTYPE_CUSTOM_3, RuCastFloatToINT64(cos(m_ru2D_Rotation)));

		paramBlock->SetParameter(ruPARAMTYPE_SYSTEM_STENCILID, m_ru2D_StencilID);

		// Ensure render context settings are up-to-date
		UpdateRenderContextSettings();

		// Submit
		m_curRenderContext->Submit(NULL, m_ru2D_ProxyMeshes[m_ru2D_PrimitiveCount], m_ru2D_Material);

		// Allocate new material for use by the 2D system
		AllocateMaterial();

		// Reset primitive count
		m_ru2D_PrimitiveCount = 0;

		// End time sampling for the flush
//		RUPROFILE_SAMPLE_END(m_probe_2D_Flush, 0);
	}
}

// ************************************************************************************************************************************************************

CRuRendererNode_UI*				g_ruUIRenderer = NULL;

// ************************************************************************************************************************************************************

#pragma managed(pop)
