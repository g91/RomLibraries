#include "../../Engine/Base/RuEngine_Engine.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(IRuEngineResource, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuEngineResource, "IRuEngineResource", "IRuObject")
ruCLASSGUID_DEFINE(IRuEngineResource, 0x40FA1887, 0xC58042f2, 0x9556DBEA, 0x6E528AE2)

ruRTTI_DEFINE_SUBCLASS(IRuVertexStreamLayout, IRuEngineResource)
ruHEAP_DEFINE_SUBCLASS(IRuVertexStreamLayout, "IRuVertexStreamLayout", "IRuEngineResource")
ruCLASSGUID_DEFINE(IRuVertexStreamLayout, 0xEFE450BF, 0x29564ddd, 0xA8A562D4, 0xD9A5E309)

ruRTTI_DEFINE_SUBCLASS(IRuVertexDeclaration, IRuEngineResource)
ruHEAP_DEFINE_SUBCLASS(IRuVertexDeclaration, "IRuVertexDeclaration", "IRuEngineResource")
ruCLASSGUID_DEFINE(IRuVertexDeclaration, 0x0D8E6D7B, 0xEA394106, 0x87D9FD6C, 0xE5B94803)

ruRTTI_DEFINE_SUBCLASS(IRuDeviceBuffer, IRuEngineResource)
ruHEAP_DEFINE_SUBCLASS(IRuDeviceBuffer, "IRuDeviceBuffer", "IRuEngineResource")
ruCLASSGUID_DEFINE(IRuDeviceBuffer, 0x5BC0F858, 0x00944809, 0xB4720CC2, 0x536302F1)

ruRTTI_DEFINE_SUBCLASS(IRuVertexBuffer, IRuDeviceBuffer)
ruHEAP_DEFINE_SUBCLASS(IRuVertexBuffer, "IRuVertexBuffer", "IRuDeviceBuffer")
ruCLASSGUID_DEFINE(IRuVertexBuffer, 0xAEB4BD40, 0x1DEB41c3, 0xA287ECBE, 0x73F0ED85)

ruRTTI_DEFINE_SUBCLASS(IRuIndexBuffer, IRuDeviceBuffer)
ruHEAP_DEFINE_SUBCLASS(IRuIndexBuffer, "IRuIndexBuffer", "IRuDeviceBuffer")
ruCLASSGUID_DEFINE(IRuIndexBuffer, 0x132C7512, 0x85F54974, 0x8BFB16BE, 0xD6F0406B)

ruRTTI_DEFINE_SUBCLASS(IRuVertexShader, IRuEngineResource)
ruHEAP_DEFINE_SUBCLASS(IRuVertexShader, "IRuVertexShader", "IRuEngineResource")
ruCLASSGUID_DEFINE(IRuVertexShader, 0x1404283F, 0x6AD54d20, 0xBEEA1B6E, 0xA4CE23F8)

ruRTTI_DEFINE_SUBCLASS(IRuPixelShader, IRuEngineResource)
ruHEAP_DEFINE_SUBCLASS(IRuPixelShader, "IRuPixelShader", "IRuEngineResource")
ruCLASSGUID_DEFINE(IRuPixelShader, 0xBFB9BC92, 0x4D9C4002, 0x949F154E, 0x3CA8D2D9)

ruRTTI_DEFINE_SUBCLASS(IRuViewport, IRuEngineResource)
ruHEAP_DEFINE_SUBCLASS(IRuViewport, "IRuViewport", "IRuEngineResource")
ruCLASSGUID_DEFINE(IRuViewport, 0x1292662A, 0x4BC74d38, 0x817041D4, 0x5101A5A5)

ruRTTI_DEFINE_SUBCLASS(IRuSurface, IRuEngineResource)
ruHEAP_DEFINE_SUBCLASS(IRuSurface, "IRuSurface", "IRuEngineResource")
ruCLASSGUID_DEFINE(IRuSurface, 0xDC1E804B, 0xA0BE480b, 0xB4D550B1, 0x653FD8EB)

ruRTTI_DEFINE_SUBCLASS(IRuBaseTexture, IRuEngineResource)
ruHEAP_DEFINE_SUBCLASS(IRuBaseTexture, "IRuBaseTexture", "IRuEngineResource")
ruCLASSGUID_DEFINE(IRuBaseTexture, 0xAB89A2B9, 0xE70E43ba, 0x870293F7, 0x364271C9)

ruRTTI_DEFINE_SUBCLASS(IRuTexture, IRuBaseTexture)
ruHEAP_DEFINE_SUBCLASS(IRuTexture, "IRuTexture", "IRuBaseTexture")
ruCLASSGUID_DEFINE(IRuTexture, 0x3244F3B6, 0x8DCB4fdf, 0x86E05FBB, 0x503BCAB3)

ruRTTI_DEFINE_SUBCLASS(IRuCubeTexture, IRuBaseTexture)
ruHEAP_DEFINE_SUBCLASS(IRuCubeTexture, "IRuCubeTexture", "IRuBaseTexture")
ruCLASSGUID_DEFINE(IRuCubeTexture, 0xE69B7973, 0xB75C479c, 0x8CE89CA2, 0x06D58862)

ruRTTI_DEFINE_SUBCLASS(IRuVolumeTexture, IRuBaseTexture)
ruHEAP_DEFINE_SUBCLASS(IRuVolumeTexture, "IRuVolumeTexture", "IRuBaseTexture")
ruCLASSGUID_DEFINE(IRuVolumeTexture, 0x6FF849B6, 0x6C294577, 0xA418FA9D, 0xA12F99D9)

ruRTTI_DEFINE_SUBCLASS(IRuTextureView, IRuEngineResource)
ruHEAP_DEFINE_SUBCLASS(IRuTextureView, "IRuTextureView", "IRuEngineResource")
ruCLASSGUID_DEFINE(IRuTextureView, 0x67C72C6C, 0xD34244aa, 0x89EF20F0, 0x7831D942)

ruRTTI_DEFINE_SUBCLASS(IRuSwapChain, IRuEngineResource)
ruHEAP_DEFINE_SUBCLASS(IRuSwapChain, "IRuSwapChain", "IRuEngineResource")
ruCLASSGUID_DEFINE(IRuSwapChain, 0xD453F18B, 0xEBD54aed, 0x83212145, 0xB1100646)

ruRTTI_DEFINE_SUBCLASS(IRuGPUQuery, IRuEngineResource)
ruHEAP_DEFINE_SUBCLASS(IRuGPUQuery, "IRuGPUQuery", "IRuEngineResource")
ruCLASSGUID_DEFINE(IRuGPUQuery, 0x903E9AD9, 0x20124ede, 0x831E0811, 0xA3945164)

ruRTTI_DEFINE_SUBCLASS(IRuVertexAssembly, IRuEngineResource)
ruHEAP_DEFINE_SUBCLASS(IRuVertexAssembly, "IRuVertexAssembly", "IRuEngineResource")
ruCLASSGUID_DEFINE(IRuVertexAssembly, 0xCC7E6A7F, 0xFFDD460e, 0x83FA604E, 0x3D6E9AAB)

ruRTTI_DEFINE_SUBCLASS(CRuVertexAssembly, IRuVertexAssembly)
ruHEAP_DEFINE_SUBCLASS(CRuVertexAssembly, "CRuVertexAssembly", "IRuVertexAssembly")
ruCLASSGUID_DEFINE(CRuVertexAssembly, 0x6288CF1F, 0x22554187, 0x89BA5C06, 0x897F6208)

ruRTTI_DEFINE_SUBCLASS(CRuVertexStreamLayout, IRuVertexStreamLayout)
ruHEAP_DEFINE_SUBCLASS(CRuVertexStreamLayout, "CRuVertexStreamLayout", "IRuVertexStreamLayout")
ruCLASSGUID_DEFINE(CRuVertexStreamLayout, 0xE3D183C0, 0x67804a49, 0xB64A7C4D, 0x57762D61)

ruRTTI_DEFINE_SUBCLASS(IRuEngine, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuEngine, "IRuEngine", "IRuObject")
ruCLASSGUID_DEFINE(IRuEngine, 0xFBD395BC, 0xF238441e, 0xBB9E4B65, 0x9EE58B02)

// ************************************************************************************************************************************************************

IRuEngineResource::IRuEngineResource()
:	m_engine(NULL),
	m_discarded(FALSE),
	m_priority(0),
	m_iteration(0),
	m_gcMarker(0),
	m_instanceData(0)
{
}

IRuEngineResource::~IRuEngineResource()
{
}

IRuEngine *IRuEngineResource::GetEngine()
{
	return m_engine;
}

void IRuEngineResource::SetEngine(IRuEngine *engine)
{
	// Allow setting the owning engine only once
	if(m_engine == NULL)
	{
		m_engine = engine;
	}
}

PTRVALUE IRuEngineResource::GetInstanceData()
{
	return m_instanceData;
}

void IRuEngineResource::SetInstanceData(PTRVALUE instanceData)
{
	m_instanceData = instanceData;
}

void IRuEngineResource::Discard()
{
	m_discarded = TRUE;
}

BOOL IRuEngineResource::IsDiscarded()
{
	return m_discarded;
}

UINT32 IRuEngineResource::GetPriority() const
{
	return m_priority;
}

void IRuEngineResource::SetPriority(UINT32 priority)
{
	m_priority = priority;
}

INT32 IRuEngineResource::GetIteration() const
{
	return m_iteration;
}

void IRuEngineResource::SetIteration(INT32 iteration)
{
	m_iteration = iteration;
}

INT32 IRuEngineResource::GetGCMarker()
{
	return m_gcMarker;
}

void IRuEngineResource::IncrementGCMarker()
{
	++m_gcMarker;
}

void IRuEngineResource::ResetGCMarker()
{
	m_gcMarker = 0;
}

void IRuEngineResource::SetGCMarker(INT32 gcMarker)
{
	m_gcMarker = gcMarker;
}

// ************************************************************************************************************************************************************

IRuVertexStreamLayout::~IRuVertexStreamLayout()
{
}

// ************************************************************************************************************************************************************

IRuVertexDeclaration::~IRuVertexDeclaration()
{
}

// ************************************************************************************************************************************************************

IRuDeviceBuffer::~IRuDeviceBuffer()
{
}

// ************************************************************************************************************************************************************

IRuVertexBuffer::~IRuVertexBuffer()
{
}

// ************************************************************************************************************************************************************

IRuIndexBuffer::~IRuIndexBuffer()
{
}

// ************************************************************************************************************************************************************

IRuViewport::~IRuViewport()
{
}

// ************************************************************************************************************************************************************

IRuSurface::~IRuSurface()
{
}

// ************************************************************************************************************************************************************

IRuBaseTexture::~IRuBaseTexture()
{
}

// ************************************************************************************************************************************************************

IRuTexture::~IRuTexture()
{
}

// ************************************************************************************************************************************************************

IRuCubeTexture::~IRuCubeTexture()
{
}

// ************************************************************************************************************************************************************

IRuVolumeTexture::~IRuVolumeTexture()
{
}

// ************************************************************************************************************************************************************

IRuTextureView::~IRuTextureView()
{
}

// ************************************************************************************************************************************************************

IRuSwapChain::~IRuSwapChain()
{
}

// ************************************************************************************************************************************************************

IRuGPUQuery::~IRuGPUQuery()
{
}

// ************************************************************************************************************************************************************

IRuVertexAssembly::~IRuVertexAssembly()
{
}

// ************************************************************************************************************************************************************

CRuVertexAssembly::CRuVertexAssembly()
:	m_isInvalidated(FALSE)
{
	memset(m_vertexStreams, 0, sizeof(IRuVertexBuffer *) * 16);
	memset(m_vertexStrides, 0, sizeof(INT32) * 16);
}

CRuVertexAssembly::~CRuVertexAssembly()
{
	Invalidate();
}

void CRuVertexAssembly::Invalidate()
{
	for(INT32 i = 0; i < 16; ++i)
	{
		ruSAFE_RELEASE(m_vertexStreams[i]);
	}

	m_isInvalidated = TRUE;
}

BOOL CRuVertexAssembly::IsInvalidated()
{
	return m_isInvalidated;
}

BOOL CRuVertexAssembly::Restore()
{
	return FALSE;
}

BOOL CRuVertexAssembly::BindVertexBuffer(INT32 streamIdx, IRuVertexBuffer *vertexBuffer, INT32 vertexStride)
{
	// Increment reference count on vertex buffer
	if(vertexBuffer)
	{
		vertexBuffer->AddRef();
	}

	// Release previously bound vertex buffer
	ruSAFE_RELEASE(m_vertexStreams[streamIdx]);

	// Store pointer to vertex buffer and its vertex stride
	m_vertexStreams[streamIdx] = vertexBuffer;
	m_vertexStrides[streamIdx] = vertexStride;

	return TRUE;
}

void CRuVertexAssembly::Select()
{
	for(INT32 i = 0; i < 16; ++i)
	{
		if(m_vertexStreams[i])
		{
			m_vertexStreams[i]->Select(i, m_vertexStrides[i]);
		}
	}
}

// ************************************************************************************************************************************************************

CRuVertexStreamLayout::CRuVertexStreamLayout(const CRuVertexElement *vertexElements, INT32 streamIdx)
:	m_vertexElements(NULL),
	m_streamStride(0)
{
	// Determine number of vertex elements for the specified stream index
	INT32 numStreamElements = 0;
	UINT32 numTotalElements = GetNumVertexElements(vertexElements);
	for(UINT32 i = 0; i < numTotalElements; ++i)
	{
		if(vertexElements[i].m_stream == streamIdx)
		{
			++numStreamElements;
		}
	}

	// Copy vertex elements for the specified stream index
	m_vertexElements = ruNEW CRuVertexElement [numStreamElements + 1];
	for(UINT32 i = 0, j = 0; i < numTotalElements; ++i)
	{
		if(vertexElements[i].m_stream == streamIdx)
		{
			// Copy vertex element
			m_vertexElements[j] = vertexElements[i];

			// Forcibly set stream index to zero (stream index is always determined by the stream binding, not by the vertex element stream index)
			m_vertexElements[j].m_stream = 0;

			// Re-calculate stream stride
			m_streamStride = max(m_streamStride, m_vertexElements[j].m_offset + RuGetVertexElementSize(static_cast<RuVertexElementType>(m_vertexElements[j].m_type)));

			++j;
		}
	}

	// Terminate the element array
	CRuVertexElement &terminatingElement = m_vertexElements[numStreamElements];
	terminatingElement.m_stream = 0xFF;
	terminatingElement.m_offset = 0;
	terminatingElement.m_type = ruELEMENTTYPE_UNUSED;
	terminatingElement.m_method = 0;
	terminatingElement.m_usage = 0;
	terminatingElement.m_usageIndex = 0;
}

CRuVertexStreamLayout::~CRuVertexStreamLayout()
{
	ruSAFE_DELETE_ARRAY(m_vertexElements);
}

void CRuVertexStreamLayout::Invalidate()
{
}

BOOL CRuVertexStreamLayout::IsInvalidated()
{
	return FALSE;
}

BOOL CRuVertexStreamLayout::Restore()
{
	return TRUE;
}

INT32 CRuVertexStreamLayout::GetStreamStride()
{
	return m_streamStride;
}

BOOL CRuVertexStreamLayout::IsEqualTo(const IRuVertexStreamLayout *vertexStreamLayout)
{
	if(vertexStreamLayout->GetType() == CRuVertexStreamLayout::Type())
	{
		return IsEqual(this->m_vertexElements, static_cast<const CRuVertexStreamLayout *>(vertexStreamLayout)->m_vertexElements);
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

IRuEngine::~IRuEngine()
{
}

CRuEvent &IRuEngine::Event_BeginScene()
{
	return m_event_BeginScene;
}

CRuEvent &IRuEngine::Event_DeviceReset()
{
	return m_event_DeviceReset;
}

CRuEvent &IRuEngine::Event_DeviceRestore()
{
	return m_event_DeviceRestore;
}

CRuEvent &IRuEngine::Event_Shutdown()
{
	return m_event_Shutdown;
}

// ************************************************************************************************************************************************************

IRuEngine*						g_ruEngine = NULL;

// ************************************************************************************************************************************************************

#pragma managed(pop)
