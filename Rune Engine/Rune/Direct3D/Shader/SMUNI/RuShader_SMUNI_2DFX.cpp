#include "../../../Direct3D/Shader/SMUNI/RuShader_SMUNI_2DFX.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SMUNI_ru2DFX_Base::CRuShader_SMUNI_ru2DFX_Base(CRuEngineD3D *engine)
{
	m_engine = engine;
	m_pixelShader = NULL;

	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },
		{ 0,	12,	ruELEMENTTYPE_RUCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },
		{ 0,	16,	ruELEMENTTYPE_FLOAT2,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_TEXCOORD,	0 },
		RUVERTEXDECL_END()
	};

	m_vertexDecl = m_engine->CreateVertexDeclaration(vertexElements);

	char *vsSource = "\
																													\n\
	// Globals																										\n\
	float4				g_globalParams : register(c0);		// (rotation, aspect ratio X:Y, sin(t), cos(t))			\n\
	float4				g_texParams : register(c1);			// (1 / w, 1 / h, 0, 0)									\n\
	float4				g_instanceData[252] : register(c2);															\n\
																													\n\
	struct VS_INPUT																									\n\
	{																												\n\
		float4			position : POSITION;																		\n\
		float4			diffuse : COLOR;																			\n\
		float2			texcoord : TEXCOORD;																		\n\
	};																												\n\
																													\n\
	struct VS_OUTPUT																								\n\
	{																												\n\
		float4			position : POSITION;																		\n\
		float4			diffuse : COLOR;																			\n\
		float2			texcoord : TEXCOORD;																		\n\
	};																												\n\
																													\n\
	VS_OUTPUT main(const VS_INPUT v)																				\n\
	{																												\n\
		VS_OUTPUT output;																							\n\
																													\n\
		float i0 = (v.diffuse.w * 255.5) * 3;																		\n\
		float i1 = i0 + 1;																							\n\
		float i2 = i1 + 1;																							\n\
																													\n\
		float4 var_x0_u0_y0_v0 = g_instanceData[i0];																\n\
		float4 var_x1_u1_y1_v1 = g_instanceData[i1];																\n\
																													\n\
		float centroidX = (var_x1_u1_y1_v1[0] + var_x0_u0_y0_v0[0]) * 0.5f;											\n\
		float centroidY = (var_x1_u1_y1_v1[2] + var_x0_u0_y0_v0[2]) * 0.5f;											\n\
																													\n\
		float2 lrp_x_u = lerp(var_x1_u1_y1_v1, var_x0_u0_y0_v0, v.diffuse.x).xy;									\n\
		float2 lrp_y_v = lerp(var_x1_u1_y1_v1, var_x0_u0_y0_v0, v.diffuse.y).zw;									\n\
																													\n\
		float relX = lrp_x_u[0] - centroidX;																		\n\
		float relY = (lrp_y_v[0] - centroidY) / g_globalParams[1];													\n\
																													\n\
		output.position.x = relX * g_globalParams[3] - relY * g_globalParams[2] + centroidX;						\n\
		output.position.y = (relX * g_globalParams[2] + relY * g_globalParams[3]) * g_globalParams[1] + centroidY;	\n\
		output.position.z = 0.0f;																					\n\
		output.position.w = 1.0f;																					\n\
																													\n\
		output.texcoord.x = lrp_x_u[1];																				\n\
		output.texcoord.y = lrp_y_v[1];																				\n\
																													\n\
		output.diffuse = g_instanceData[i2];																		\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Create shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);
}

CRuShader_SMUNI_ru2DFX_Base::~CRuShader_SMUNI_ru2DFX_Base()
{
	if(m_vertexDecl)
		m_vertexDecl->Release();

	if(m_vertexShader)
		m_vertexShader->Release();

	if(m_pixelShader)
		m_pixelShader->Release();
}

void CRuShader_SMUNI_ru2DFX_Base::BeginPass(INT32 passNumber)
{
	m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_engine->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 4);

	m_engine->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, (DWORD)RuCastFloatToINT64(-0.5f));
}

void CRuShader_SMUNI_ru2DFX_Base::EndPass(INT32 passNumber)
{
	m_engine->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	m_engine->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, (DWORD)RuCastFloatToINT64(0.0f));
}

void CRuShader_SMUNI_ru2DFX_Base::SetDefaultParameters()
{
}

void CRuShader_SMUNI_ru2DFX_Base::SetParameters(const CRuParameterCache *paramCache)
{
	UINT64 param;

	// NOTE: Gaussian blur needs this or something
	if(paramCache->GetParameter(ruPARAMTYPE_2DFX_TEXTURE_WIDTH, param))
	{
		INT32 texWidthOffset = 1 * 4 + 0;
		m_vsConstants[texWidthOffset] = 1.0f / static_cast<REAL>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_2DFX_TEXTURE_HEIGHT, param))
	{
		INT32 texHeightOffset = 1 * 4 + 1;
		m_vsConstants[texHeightOffset] = 1.0f / static_cast<REAL>(param);
		m_vsConstants[texHeightOffset + 1] = 0.0f;
		m_vsConstants[texHeightOffset + 2] = 0.0f;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, param))
	{
		m_numInstances = (UINT32) param;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_GEOINSTANCE_S_DATA, param))
	{
		float *instanceData = reinterpret_cast<float *>(param);

		// Number of constants = instance dataset
		INT32 numConstants = m_numInstances * 12;
		INT32 constantOffset = 2 * 4;

		RuMemcpy(&m_vsConstants[constantOffset], instanceData, sizeof(float) * numConstants);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_CUSTOM_0, param))
	{
		INT32 constantOffset = 0 * 4;
		REAL rotation = RuCastINT64ToFloat(param);
		m_vsConstants[constantOffset] = rotation;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_CUSTOM_1, param))
	{
		INT32 constantOffset = 0 * 4 + 1;
		REAL aspectRatio_XY = RuCastINT64ToFloat(param);
		m_vsConstants[constantOffset] = aspectRatio_XY;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_CUSTOM_2, param))
	{
		INT32 constantOffset = 0 * 4 + 2;
		REAL sinTheta = RuCastINT64ToFloat(param);
		m_vsConstants[constantOffset] = sinTheta;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_CUSTOM_3, param))
	{
		INT32 constantOffset = 0 * 4 + 3;
		REAL cosTheta = RuCastINT64ToFloat(param);
		m_vsConstants[constantOffset] = cosTheta;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SYSTEM_STENCILID, param))
	{
		m_stencilID = static_cast<UINT8>(param);
	}
}

// ************************************************************************************************************************************************************

const char *CRuShader_SMUNI_ru2DFXTYPE_FLAT::GetShaderName() const
{
	return "ru2DFXTYPE_FLAT";
}

void CRuShader_SMUNI_ru2DFXTYPE_FLAT::BeginPass(INT32 passNumber)
{
	// Chain call to base class
	CRuShader_SMUNI_ru2DFX_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(NULL);
		m_engine->SetVertexShaderConstants(0, m_vsConstants, 2 + m_numInstances * 3);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

		// Disable stencil
		m_engine->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		m_engine->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_engine->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

// ************************************************************************************************************************************************************

const char *CRuShader_SMUNI_ru2DFXTYPE_FLAT_MASKED::GetShaderName() const
{
	return "ru2DFXTYPE_FLAT_MASKED";
}

void CRuShader_SMUNI_ru2DFXTYPE_FLAT_MASKED::BeginPass(INT32 passNumber)
{
	// Chain call to base class
	CRuShader_SMUNI_ru2DFX_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(NULL);
		m_engine->SetVertexShaderConstants(0, m_vsConstants, 2 + m_numInstances * 3);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

		// Enable stencil testing
		m_engine->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		m_engine->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		m_engine->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		m_engine->SetRenderState(D3DRS_STENCILREF, m_stencilID);
		m_engine->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
		m_engine->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
		m_engine->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);

		m_engine->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_engine->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

void CRuShader_SMUNI_ru2DFXTYPE_FLAT_MASKED::EndPass(INT32 passNumber)
{
	if(passNumber == 0)
	{
		m_engine->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}
}

// ************************************************************************************************************************************************************

const char *CRuShader_SMUNI_ru2DFXTYPE_BLIT::GetShaderName() const
{
	return "ru2DFXTYPE_BLIT";
}

void CRuShader_SMUNI_ru2DFXTYPE_BLIT::BeginPass(INT32 passNumber)
{
	// Chain call to base class
	CRuShader_SMUNI_ru2DFX_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(NULL);
		m_engine->SetVertexShaderConstants(0, m_vsConstants, 2 + m_numInstances * 3);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

		m_engine->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_engine->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		// Enable point filtering
		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

		// Set addressing mode to wrap
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

// ************************************************************************************************************************************************************

const char *CRuShader_SMUNI_ru2DFXTYPE_BLIT_ADD::GetShaderName() const
{
	return "ru2DFXTYPE_BLIT_ADD";
}

void CRuShader_SMUNI_ru2DFXTYPE_BLIT_ADD::BeginPass(INT32 passNumber)
{
	// Chain call to base class
	CRuShader_SMUNI_ru2DFX_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(NULL);
		m_engine->SetVertexShaderConstants(0, m_vsConstants, 2 + m_numInstances * 3);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

		// Disable stencil
		m_engine->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		m_engine->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_engine->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		// Enable point filtering
		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// Set addressing mode to wrap
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

// ************************************************************************************************************************************************************

const char *CRuShader_SMUNI_ru2DFXTYPE_BLIT_FILTER::GetShaderName() const
{
	return "ru2DFXTYPE_BLIT_FILTER";
}

void CRuShader_SMUNI_ru2DFXTYPE_BLIT_FILTER::BeginPass(INT32 passNumber)
{
	// Chain call to base class
	CRuShader_SMUNI_ru2DFX_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(NULL);
		m_engine->SetVertexShaderConstants(0, m_vsConstants, 2 + m_numInstances * 3);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

		// Disable stencil
		m_engine->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		m_engine->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_engine->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		// Enable linear filtering
		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// Set addressing mode to wrap
//		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
//		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

// ************************************************************************************************************************************************************

CRuShader_SMUNI_ru2DFXTYPE_TEXTBLIT_FILTER::CRuShader_SMUNI_ru2DFXTYPE_TEXTBLIT_FILTER(CRuEngineD3D *engine)
:	CRuShader_SMUNI_ru2DFX_Base(engine)
{
	char *psSource = "\
																													\n\
	sampler2D tex0 : register(s0);																					\n\
																													\n\
	struct PS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR;															\n\
		float2			texcoord0				: TEXCOORD0;														\n\
	};																												\n\
																													\n\
	struct PS_OUTPUT																								\n\
	{																												\n\
		float4			color					: COLOR;															\n\
	};																												\n\
																													\n\
	PS_OUTPUT main(const PS_INPUT v)																				\n\
	{																												\n\
		PS_OUTPUT output;																							\n\
																													\n\
		// Sample diffuse map with a -0.5 level bias																\n\
		float4 texc2 = float4(v.texcoord0.x, v.texcoord0.y, 0.0f, -0.5f);											\n\
		float4 color = tex2Dbias(tex0, texc2);																		\n\
																													\n\
		// Replicate color to alpha																					\n\
		color.w = color.x * v.diffuse.w;																			\n\
		color.x = v.diffuse.x;																						\n\
		color.y = v.diffuse.y;																						\n\
		color.z = v.diffuse.z;																						\n\
																													\n\
		output.color = color;																						\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile pixel shader
	m_pixelShader = RuD3DCompilePixelShader(psSource, "main", "ps_2_0", NULL);
}

const char *CRuShader_SMUNI_ru2DFXTYPE_TEXTBLIT_FILTER::GetShaderName() const
{
	return "ru2DFXTYPE_TEXTBLIT_FILTER";
}

void CRuShader_SMUNI_ru2DFXTYPE_TEXTBLIT_FILTER::BeginPass(INT32 passNumber)
{
	// Chain call to base class
	CRuShader_SMUNI_ru2DFX_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_vsConstants, 2 + m_numInstances * 3);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

		// Disable stencil
		m_engine->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		// Enable linear filtering
		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// Set addressing mode to clamp
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

// ************************************************************************************************************************************************************

const char *CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASK::GetShaderName() const
{
	return "ru2DFXTYPE_BLIT_MASK";
}

void CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASK::BeginPass(INT32 passNumber)
{
	// Chain call to base class
	CRuShader_SMUNI_ru2DFX_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(NULL);
		m_engine->SetVertexShaderConstants(0, m_vsConstants, 2 + m_numInstances * 3);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		m_engine->SetRenderState(D3DRS_ALPHAREF, 0);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

		// Set render states to write stencil value of 1
		m_engine->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		m_engine->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		m_engine->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
		m_engine->SetRenderState(D3DRS_STENCILREF, m_stencilID);
		m_engine->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
		m_engine->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
		m_engine->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);

		m_engine->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_engine->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		// Enable linear filtering
		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// Set addressing mode to clamp
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

void CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASK::EndPass(INT32 passNumber)
{
	if(passNumber == 0)
	{
		m_engine->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}
}

// ************************************************************************************************************************************************************

const char *CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASKED::GetShaderName() const
{
	return "ru2DFXTYPE_BLIT_MASKED";
}

void CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASKED::BeginPass(INT32 passNumber)
{
	// Chain call to base class
	CRuShader_SMUNI_ru2DFX_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(NULL);
		m_engine->SetVertexShaderConstants(0, m_vsConstants, 2 + m_numInstances * 3);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

		// Enable stencil testing
		m_engine->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		m_engine->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		m_engine->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		m_engine->SetRenderState(D3DRS_STENCILREF, m_stencilID);
		m_engine->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
		m_engine->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
		m_engine->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);

		m_engine->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_engine->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		// Enable linear filtering
		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// Set addressing mode to clamp
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

void CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASKED::EndPass(INT32 passNumber)
{
	if(passNumber == 0)
	{
		m_engine->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}
}

// ************************************************************************************************************************************************************

const char *CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASK_Z::GetShaderName() const
{
	return "ru2DFXTYPE_BLIT_MASK_Z";
}

void CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASK_Z::BeginPass(INT32 passNumber)
{
	// Chain call to base class
	CRuShader_SMUNI_ru2DFX_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(NULL);
		m_engine->SetVertexShaderConstants(0, m_vsConstants, 2 + m_numInstances * 3);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_EQUAL);
		m_engine->SetRenderState(D3DRS_ALPHAREF, 0);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

		// Disable color writes
		m_engine->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

		m_engine->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_engine->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		// Enable linear filtering
		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// Set addressing mode to clamp
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

void CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASK_Z::EndPass(INT32 passNumber)
{
	// Enable color writes
	m_engine->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
}

// ************************************************************************************************************************************************************

CRuShader_SMUNI_ru2DFXTYPE_BLIT_LUMINANCE::CRuShader_SMUNI_ru2DFXTYPE_BLIT_LUMINANCE(CRuEngineD3D *engine)
:	CRuShader_SMUNI_ru2DFX_Base(engine)
{
	char *psSource = "\
																													\n\
	ps_1_1																											\n\
																													\n\
	def					c1,			1.0, 1.0, 1.0, 0.0																\n\
	def					c2,			0.33, 0.33, 0.33, 1.0															\n\
																													\n\
	; Texture sampling																								\n\
	tex					t0											; Sample diffuse texture						\n\
																													\n\
	; Arithmetic ops																								\n\
	dp3					r1,			t0,			c1					; Add RGB into r1								\n\
	mul					r0,			r1,			c2					; Divide sum by 3 to get luminance (sort of)	\n\
	mov					r0.w,		t0.w							; Get texture alpha into r0.w					\n\
																													\n\
	mul					r0,			r0,			v0					; Modulate luminance with diffuse color			\n\
	";

	// Assemble pixel shader
	m_pixelShader = RuD3DAssemblePixelShader(psSource);
}

const char *CRuShader_SMUNI_ru2DFXTYPE_BLIT_LUMINANCE::GetShaderName() const
{
	return "ru2DFXTYPE_BLIT_LUMINANCE";
}

void CRuShader_SMUNI_ru2DFXTYPE_BLIT_LUMINANCE::BeginPass(INT32 passNumber)
{
	// Chain call to base class
	CRuShader_SMUNI_ru2DFX_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_vsConstants, 2 + m_numInstances * 3);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

		// Disable stencil
		m_engine->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		// Enable point filtering
		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

		// Set addressing mode to wrap
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

// ************************************************************************************************************************************************************

const char *CRuShader_SMUNI_ru2DFXTYPE_DARKEN::GetShaderName() const
{
	return "ru2DFXTYPE_DARKEN";
}

void CRuShader_SMUNI_ru2DFXTYPE_DARKEN::BeginPass(INT32 passNumber)
{
	// Chain call to base class
	CRuShader_SMUNI_ru2DFX_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(NULL);
		m_engine->SetVertexShaderConstants(0, m_vsConstants, 2 + m_numInstances * 3);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

		// Disable stencil
		m_engine->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		m_engine->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_engine->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

// ************************************************************************************************************************************************************

CRuShader_SMUNI_ru2DFXTYPE_EXPONENTIATE::CRuShader_SMUNI_ru2DFXTYPE_EXPONENTIATE(CRuEngineD3D *engine)
:	CRuShader_SMUNI_ru2DFX_Base(engine)
{
	char *psSource = "\
																													\n\
	ps_1_1																											\n\
																													\n\
	def					c2,			0.75, 0.75, 0.75, 0.0															\n\
																													\n\
	; Texture sampling																								\n\
	tex					t0											; Sample diffuse texture						\n\
																													\n\
	; Arithmetic ops																								\n\
	mul					r0,			t0,			v0					; Modulate diffuse color by texture color		\n\
	mul					r0.xyz,		r0,			r0					; x^2											\n\
	mul					r0.xyz,		r0,			r0					; x^4											\n\
	mul					r0.xyz,		r0,			r0					; x^8											\n\
	";

	// Assemble pixel shader
	m_pixelShader = RuD3DAssemblePixelShader(psSource);
}

const char *CRuShader_SMUNI_ru2DFXTYPE_EXPONENTIATE::GetShaderName() const
{
	return "ru2DFXTYPE_EXPONENTIATE";
}

void CRuShader_SMUNI_ru2DFXTYPE_EXPONENTIATE::BeginPass(INT32 passNumber)
{
	// Chain call to base class
	CRuShader_SMUNI_ru2DFX_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_vsConstants, 2 + m_numInstances * 3);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

		// Disable stencil
		m_engine->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		// Enable point filtering
		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

		// Set addressing mode to wrap
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

// ************************************************************************************************************************************************************

CRuShader_SMUNI_ru2DFXTYPE_GAUSSIAN::CRuShader_SMUNI_ru2DFXTYPE_GAUSSIAN(CRuEngineD3D *engine)
:	CRuShader_SMUNI_ru2DFX_Base(engine),
	m_blurDirection(0)
{
	char *vsSource_Pass_0 = "\
																													\n\
	// Globals																										\n\
	float4				g_globalParams : register(c0);		// (rotation, aspect ratio X:Y, sin(t), cos(t))			\n\
	float4				g_texParams : register(c1);			// (1 / w, 1 / h, 0, 0)									\n\
	float4				g_instanceData[252] : register(c2);															\n\
																													\n\
	struct VS_INPUT																									\n\
	{																												\n\
		float4			position : POSITION;																		\n\
		float4			diffuse : COLOR;																			\n\
		float2			texcoord : TEXCOORD;																		\n\
	};																												\n\
																													\n\
	struct VS_OUTPUT																								\n\
	{																												\n\
		float4			position : POSITION;																		\n\
		float4			diffuse  : COLOR;																			\n\
		float2			tex_L1   : TEXCOORD0;																		\n\
		float2			tex_L2   : TEXCOORD1;																		\n\
		float2			tex_L3   : TEXCOORD2;																		\n\
		float2			tex_C    : TEXCOORD3;																		\n\
		float2			tex_R3   : TEXCOORD4;																		\n\
		float2			tex_R2   : TEXCOORD5;																		\n\
		float2			tex_R1   : TEXCOORD6;																		\n\
	};																												\n\
																													\n\
	VS_OUTPUT main(const VS_INPUT v)																				\n\
	{																												\n\
		VS_OUTPUT output;																							\n\
																													\n\
		float i0 = (v.diffuse.w * 255.5) * 3;																		\n\
		float i1 = i0 + 1;																							\n\
		float i2 = i1 + 1;																							\n\
																													\n\
		float4 var_x0_u0_y0_v0 = g_instanceData[i0];																\n\
		float4 var_x1_u1_y1_v1 = g_instanceData[i1];																\n\
																													\n\
		float centroidX = (var_x1_u1_y1_v1[0] + var_x0_u0_y0_v0[0]) * 0.5f;											\n\
		float centroidY = (var_x1_u1_y1_v1[2] + var_x0_u0_y0_v0[2]) * 0.5f;											\n\
																													\n\
		float2 lrp_x_u = lerp(var_x1_u1_y1_v1, var_x0_u0_y0_v0, v.diffuse.x).xy;									\n\
		float2 lrp_y_v = lerp(var_x1_u1_y1_v1, var_x0_u0_y0_v0, v.diffuse.y).zw;									\n\
																													\n\
		float relX = lrp_x_u[0] - centroidX;																		\n\
		float relY = (lrp_y_v[0] - centroidY) / g_globalParams[1];													\n\
																													\n\
		output.position.x = relX * g_globalParams[3] - relY * g_globalParams[2] + centroidX;						\n\
		output.position.y = (relX * g_globalParams[2] + relY * g_globalParams[3]) * g_globalParams[1] + centroidY;	\n\
		output.position.z = 0.0f;																					\n\
		output.position.w = 1.0f;																					\n\
																													\n\
		output.tex_C.x = lrp_x_u[1];																				\n\
		output.tex_C.y = lrp_y_v[1];																				\n\
																													\n\
		float2 hOffset = g_texParams.xz;																			\n\
																													\n\
		output.tex_L3 = output.tex_C - hOffset;																		\n\
		output.tex_L2 = output.tex_L3 - hOffset;																	\n\
		output.tex_L1 = output.tex_L2 - hOffset;																	\n\
		output.tex_R3 = output.tex_C + hOffset;																		\n\
		output.tex_R2 = output.tex_R3 + hOffset;																	\n\
		output.tex_R1 = output.tex_R2 + hOffset;																	\n\
																													\n\
		output.diffuse = g_instanceData[i2];																		\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *vsSource_Pass_1 = "\
																													\n\
	// Globals																										\n\
	float4				g_globalParams : register(c0);		// (rotation, aspect ratio X:Y, sin(t), cos(t))			\n\
	float4				g_texParams : register(c1);			// (1 / w, 1 / h, 0, 0)									\n\
	float4				g_instanceData[252] : register(c2);															\n\
																													\n\
	struct VS_INPUT																									\n\
	{																												\n\
		float4			position : POSITION;																		\n\
		float4			diffuse : COLOR;																			\n\
		float2			texcoord : TEXCOORD;																		\n\
	};																												\n\
																													\n\
	struct VS_OUTPUT																								\n\
	{																												\n\
		float4			position : POSITION;																		\n\
		float4			diffuse  : COLOR;																			\n\
		float2			tex_L1   : TEXCOORD0;																		\n\
		float2			tex_L2   : TEXCOORD1;																		\n\
		float2			tex_L3   : TEXCOORD2;																		\n\
		float2			tex_C    : TEXCOORD3;																		\n\
		float2			tex_R3   : TEXCOORD4;																		\n\
		float2			tex_R2   : TEXCOORD5;																		\n\
		float2			tex_R1   : TEXCOORD6;																		\n\
	};																												\n\
																													\n\
	VS_OUTPUT main(const VS_INPUT v)																				\n\
	{																												\n\
		VS_OUTPUT output;																							\n\
																													\n\
		float i0 = (v.diffuse.w * 255.5) * 3;																		\n\
		float i1 = i0 + 1;																							\n\
		float i2 = i1 + 1;																							\n\
																													\n\
		float4 var_x0_u0_y0_v0 = g_instanceData[i0];																\n\
		float4 var_x1_u1_y1_v1 = g_instanceData[i1];																\n\
																													\n\
		float centroidX = (var_x1_u1_y1_v1[0] + var_x0_u0_y0_v0[0]) * 0.5f;											\n\
		float centroidY = (var_x1_u1_y1_v1[2] + var_x0_u0_y0_v0[2]) * 0.5f;											\n\
																													\n\
		float2 lrp_x_u = lerp(var_x1_u1_y1_v1, var_x0_u0_y0_v0, v.diffuse.x).xy;									\n\
		float2 lrp_y_v = lerp(var_x1_u1_y1_v1, var_x0_u0_y0_v0, v.diffuse.y).zw;									\n\
																													\n\
		float relX = lrp_x_u[0] - centroidX;																		\n\
		float relY = (lrp_y_v[0] - centroidY) / g_globalParams[1];													\n\
																													\n\
		output.position.x = relX * g_globalParams[3] - relY * g_globalParams[2] + centroidX;						\n\
		output.position.y = (relX * g_globalParams[2] + relY * g_globalParams[3]) * g_globalParams[1] + centroidY;	\n\
		output.position.z = 0.0f;																					\n\
		output.position.w = 1.0f;																					\n\
																													\n\
		output.tex_C.x = lrp_x_u[1];																				\n\
		output.tex_C.y = lrp_y_v[1];																				\n\
																													\n\
		float2 hOffset = g_texParams.zy;																			\n\
																													\n\
		output.tex_L3 = output.tex_C - hOffset;																		\n\
		output.tex_L2 = output.tex_L3 - hOffset;																	\n\
		output.tex_L1 = output.tex_L2 - hOffset;																	\n\
		output.tex_R3 = output.tex_C + hOffset;																		\n\
		output.tex_R2 = output.tex_R3 + hOffset;																	\n\
		output.tex_R1 = output.tex_R2 + hOffset;																	\n\
																													\n\
		output.diffuse = g_instanceData[i2];																		\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource = "\
																													\n\
	ps_2_0																											\n\
																													\n\
	; Declare texture samplers																						\n\
	dcl_2d				s0											; DIFFUSE MAP									\n\
																													\n\
	; Declare texture coordinate registers																			\n\
	dcl					t0.xy										; L2											\n\
	dcl					t1.xy										; L1											\n\
	dcl					t2.xy										; L0											\n\
	dcl					t3.xy										; C												\n\
	dcl					t4.xy										; R0											\n\
	dcl					t5.xy										; R1											\n\
	dcl					t6.xy										; R2											\n\
																													\n\
	dcl					v0.xyzw																						\n\
																													\n\
	texld				r0,			t0,			s0					; Sample L2										\n\
	texld				r1,			t1,			s0					; Sample L1										\n\
	texld				r2,			t2,			s0					; Sample L0										\n\
	texld				r3,			t3,			s0					; Sample C										\n\
	texld				r4,			t4,			s0					; Sample R0										\n\
	texld				r5,			t5,			s0					; Sample R1										\n\
	texld				r6,			t6,			s0					; Sample R2										\n\
																													\n\
	mul					r7,			r0,			c0.x				; Accumulate L2									\n\
	mad					r7,			r1,			c0.y,		r7		; Accumulate L1									\n\
	mad					r7,			r2,			c0.z,		r7		; Accumulate L0									\n\
	mad					r7,			r3,			c0.w,		r7		; Accumulate C									\n\
	mad					r7,			r4,			c0.z,		r7		; Accumulate R0									\n\
	mad					r7,			r5,			c0.y,		r7		; Accumulate R1									\n\
	mad					r7,			r6,			c0.x,		r7		; Accumulate R2									\n\
																													\n\
	;mov					r7.w,		c1.x							; Force alpha to one							\n\
																													\n\
	mov					oC0,		r7								; Move result into output						\n\
	";

	// Create shader
	m_vertexShaders[0] = RuD3DCompileVertexShader(vsSource_Pass_0, "main", "vs_2_0", NULL);
	m_vertexShaders[1]= RuD3DCompileVertexShader(vsSource_Pass_1, "main", "vs_2_0", NULL);

	// Assemble pixel shader
	m_pixelShader = RuD3DAssemblePixelShader(psSource);
}

CRuShader_SMUNI_ru2DFXTYPE_GAUSSIAN::~CRuShader_SMUNI_ru2DFXTYPE_GAUSSIAN()
{
	if(m_vertexShaders[0])
		m_vertexShaders[0]->Release();
	if(m_vertexShaders[1])
		m_vertexShaders[1]->Release();
}

const char *CRuShader_SMUNI_ru2DFXTYPE_GAUSSIAN::GetShaderName() const
{
	return "ru2DFXTYPE_GAUSSIAN";
}

void CRuShader_SMUNI_ru2DFXTYPE_GAUSSIAN::BeginPass(INT32 passNumber)
{
/*
	if(passNumber == 0)
	{
		m_vsConstants[4] = 1.0f;
		m_vsConstants[5] = 0.0f;
		m_vsConstants[6] = 0.0f;
		m_vsConstants[7] = 0.0f;
	}
	else
	{
		m_vsConstants[4] = 0.0f;
		m_vsConstants[5] = 0.0f;
		m_vsConstants[6] = 0.0f;
		m_vsConstants[7] = 0.0f;
	}


	m_vsConstants[2] = 0.0f;
	m_vsConstants[3] = 0.0f;
*/
	m_psConstants[0] = 0.006f;
	m_psConstants[1] = 0.061f;
	m_psConstants[2] = 0.242f;
	m_psConstants[3] = 0.383f;

	m_psConstants[4] = 1.000f;
	m_psConstants[5] = 0.000f;
	m_psConstants[6] = 0.000f;
	m_psConstants[7] = 0.000f;

//	m_psConstants[0] = 0.025f;
//	m_psConstants[1] = 0.075f;
//	m_psConstants[2] = 0.200f;
//	m_psConstants[3] = 0.400f;

	m_vertexDecl->Select();
	m_engine->SetVertexShader(m_vertexShaders[m_blurDirection]);
	m_engine->SetPixelShader(m_pixelShader);
	m_engine->SetVertexShaderConstants(0, m_vsConstants, 2 + m_numInstances * 3);
	m_engine->SetPixelShaderConstants(0, m_psConstants, 2);

	m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

	// Disable stencil
	m_engine->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	// Enable POINT filtering
	m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// Set addressing mode to clamp
	m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	// Disable fog
	m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
}

void CRuShader_SMUNI_ru2DFXTYPE_GAUSSIAN::EndPass(INT32 passNumber)
{
}

void CRuShader_SMUNI_ru2DFXTYPE_GAUSSIAN::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SMUNI_ru2DFX_Base::SetParameters(paramCache);

	UINT64 param;

	if(paramCache->GetParameter(ruPARAMTYPE_DWORD_0, param))
	{
		m_blurDirection = (INT32)param;
	}
}

// ************************************************************************************************************************************************************

CRuShader_SMUNI_ru2DFXTYPE_DISTORTION::CRuShader_SMUNI_ru2DFXTYPE_DISTORTION(CRuEngineD3D *engine)
:	CRuShader_SMUNI_ru2DFX_Base(engine)
{
	char *psSource = "\
																													\n\
	sampler2D			texDiffuse				: register(s0);														\n\
	sampler2D			texBump					: register(s1);														\n\
																													\n\
	float4				g_bumpStrength			: register(c0);			// Constants (bump strength, 0.5f, 0.0f, 0.05f)		\n\
																													\n\
	struct PS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR0;															\n\
		float2			uv0						: TEXCOORD0;														\n\
	};																												\n\
																													\n\
	struct PS_OUTPUT																								\n\
	{																												\n\
		float4			color					: COLOR;															\n\
	};																												\n\
																													\n\
	PS_OUTPUT main(const PS_INPUT v)																				\n\
	{																												\n\
		PS_OUTPUT output;																							\n\
																													\n\
		// Sample bump																								\n\
		float4 bump = tex2D(texBump, v.uv0);																		\n\
																													\n\
		// Perturb uv coordinates																					\n\
		float2 effectiveUV = v.uv0 + (bump.xy - 0.5f) * g_bumpStrength.xx;											\n\
																													\n\
		// Sample diffuse and bump																					\n\
		float4 diffuse = tex2D(texDiffuse, effectiveUV);															\n\
																													\n\
		output.color = diffuse * v.diffuse;																			\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile pixel shader
	m_pixelShader = RuD3DCompilePixelShader(psSource, "main", "ps_2_0", NULL);
}

const char *CRuShader_SMUNI_ru2DFXTYPE_DISTORTION::GetShaderName() const
{
	return "ru2DFXTYPE_DISTORTION";
}

void CRuShader_SMUNI_ru2DFXTYPE_DISTORTION::BeginPass(INT32 passNumber)
{
	// Chain call to base class
	CRuShader_SMUNI_ru2DFX_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_vsConstants, 2 + m_numInstances * 3);
		m_engine->SetPixelShaderConstants(0, m_psConstants, 1);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

		// Disable stencil
		m_engine->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		// Enable point filtering
		m_engine->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// Set addressing mode to wrap
		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

void CRuShader_SMUNI_ru2DFXTYPE_DISTORTION::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SMUNI_ru2DFX_Base::SetParameters(paramCache);

	UINT64 param;

	if(paramCache->GetParameter(ruPARAMTYPE_CUSTOM_4, param))
	{
		INT32 constantOffset = 0 * 4;
		REAL strength = RuCastINT64ToFloat(param);
		m_psConstants[constantOffset] = strength;
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
