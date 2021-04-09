/*!
	@file RuShader_D3D_FX2D_UIFX.cpp

	Copyright (c) 2004-2006 All rights reserved
*/

#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_FX2D_UIFX.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_FX2D_UIFX_Darken::CRuShader_SM20EZ_FX2D_UIFX_Darken(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Base(engine)
{
	// Setup vertex elements
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },
		{ 0,	12,	ruELEMENTTYPE_D3DCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },
		{ 0,	16,	ruELEMENTTYPE_FLOAT2,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_TEXCOORD,	0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexDecl = m_engine->CreateVertexDeclaration(vertexElements);

	char *vsSource = "\
																													\n\
	// Globals																										\n\
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
		output.position.xyz = v.position.xyz;																		\n\
		output.position.w = 1.0f;																					\n\
																													\n\
		output.texcoord = v.texcoord;																				\n\
																													\n\
		output.diffuse = v.diffuse;																					\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Create shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Overrides
	m_override_ZWrite = TRUE;
}

CRuShader_SM20EZ_FX2D_UIFX_Darken::~CRuShader_SM20EZ_FX2D_UIFX_Darken()
{
}

void CRuShader_SM20EZ_FX2D_UIFX_Darken::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(NULL);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants + 1);

		m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_engine->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

		m_engine->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);

		m_engine->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

		m_engine->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_engine->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}
}

void CRuShader_SM20EZ_FX2D_UIFX_Darken::EndPass(INT32 passNumber)
{
	CRuShader_SM20EZ_Base::EndPass(passNumber);

	m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CRuShader_SM20EZ_FX2D_UIFX_Darken::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SM20EZ_Base::SetParameters(paramCache);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
