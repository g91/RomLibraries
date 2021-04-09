#include "../../Engine/Base/RuEngineBase.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(IRuEngineResource, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuEngineResource, "IRuEngineResource", "IRuObject")
ruCLASSGUID_DEFINE(IRuEngineResource, 0x40FA1887, 0xC58042f2, 0x9556DBEA, 0x6E528AE2)

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

ruRTTI_DEFINE_SUBCLASS(CRuTexture_Reference, IRuBaseTexture)
ruHEAP_DEFINE_SUBCLASS(CRuTexture_Reference, "CRuTexture_Reference", "IRuBaseTexture")
ruCLASSGUID_DEFINE(CRuTexture_Reference, 0x2AD544A2, 0x09FB42e2, 0xAD6C7450, 0xCDAB580E)

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

ruRTTI_DEFINE_SUBCLASS(IRuEngine, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuEngine, "IRuEngine", "IRuObject")
ruCLASSGUID_DEFINE(IRuEngine, 0xFBD395BC, 0xF238441e, 0xBB9E4B65, 0x9EE58B02)

// ************************************************************************************************************************************************************

CRuTexture_Reference::CRuTexture_Reference()
:	m_textureName(NULL)
{
}

CRuTexture_Reference::~CRuTexture_Reference()
{
	delete[] m_textureName;
}

void CRuTexture_Reference::Invalidate()
{
}

BOOL CRuTexture_Reference::IsInvalidated()
{
	return FALSE;
}

BOOL CRuTexture_Reference::Restore()
{
	return TRUE;
}

const char *CRuTexture_Reference::GetTextureName()
{
	if(m_textureName)
	{
		return m_textureName;
	}

	return "";
}

void CRuTexture_Reference::SetTextureName(const char *textureName)
{
	if(m_textureName)
	{
		delete[] m_textureName;
	}

	UINT32 textureNameLength = strlen(textureName) + 1;
	m_textureName = ruNEW char [textureNameLength];
	strcpy(m_textureName, textureName);
}

void CRuTexture_Reference::Select(UINT32 samplerIndex)
{
}

BOOL CRuTexture_Reference::GenerateMipLevels()
{
	return FALSE;
}

BOOL CRuTexture_Reference::IsDynamic()
{
	return FALSE;
}

BOOL CRuTexture_Reference::IsRenderTarget()
{
	return FALSE;
}

UINT32 CRuTexture_Reference::GetLevelCount()
{
	return 0;
}

UINT32 CRuTexture_Reference::GetLOD()
{
	return 0;
}

UINT32 CRuTexture_Reference::SetLOD(UINT32 lod)
{
	return 0;
}

RuResourcePool CRuTexture_Reference::GetPool()
{
	return ruRESPOOL_SCRATCH;
}

RuSurfaceFormat CRuTexture_Reference::GetSurfaceFormat()
{
	return ruSURFACEFMT_A8R8G8B8;
}

// ************************************************************************************************************************************************************

CRuUtility_FunctionScheduler*	g_ruFunctionSchedule = NULL;
CRuThread_Worker*				g_ruGlobalWorkerThread = NULL;
IRuEngine*						g_ruEngine = NULL;

// ************************************************************************************************************************************************************

#pragma managed
