#include "../../../Direct3D/Shader/SMUNI/RuShader_SMUNI_Sky.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SMUNI_SkyDome::CRuShader_SMUNI_SkyDome(CRuEngineD3D *engine)
:	CRuShader_SMUNI_Base(engine)
{
	char *vsSource = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_color			v1											; Declare diffuse								\n\
																													\n\
	; Transform position into homogeneous space																		\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	m3x3				r1,			v0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r1,			c6					; Transform vertex to homogeneous space			\n\
																													\n\
	; Compute uv																									\n\
	mov					oT0.xy,		v1.xy							; Output UV0									\n\
	mov					oT1.xy,		v1.xy							; Output UV1									\n\
																													\n\
	; Compute diffuse																								\n\
	mov					oD0,		c27.x							; Copy blend factor to diffuse					\n\
																													\n\
	; Compute fog																									\n\
	mov					oFog,		c20.y							; Output fog factor (always 1)					\n\
	";

	// Setup vertex elements
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },
		{ 0,	12,	ruELEMENTTYPE_RUCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexDecl = m_engine->CreateVertexDeclaration(vertexElements);

	// Assemble vertex shader
	m_vertexShader = RuD3DAssembleVertexShader(vsSource);

	// Overrides
	m_override_ZWrite = TRUE;

	// Set base constants count
	m_numBaseConstants = 30;
}

CRuShader_SMUNI_SkyDome::~CRuShader_SMUNI_SkyDome()
{
}

void CRuShader_SMUNI_SkyDome::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SMUNI_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_shaderConstants[20 * 4 + 1] = 1.0f;

		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(NULL);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		m_engine->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDCURRENTALPHA);
		m_engine->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
		m_engine->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);

		m_engine->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		m_engine->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
		m_engine->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

		m_engine->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_engine->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		m_engine->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}
}

void CRuShader_SMUNI_SkyDome::EndPass(INT32 passNumber)
{
	CRuShader_SMUNI_Base::EndPass(passNumber);
}

void CRuShader_SMUNI_SkyDome::SetParameters(const CRuParameterCache *paramCache)
{
	UINT64 param;

	CRuShader_SMUNI_Base::SetParameters(paramCache);

	if(paramCache->GetParameter(ruPARAMTYPE_SKYDOME_GRADIENT_T, param))
	{
		INT32 gradientTConstantsOffset = 27 * 4;

		m_shaderConstants[gradientTConstantsOffset++] = RuCastINT64ToFloat(param);
		m_shaderConstants[gradientTConstantsOffset++] = 0.0f;
		m_shaderConstants[gradientTConstantsOffset++] = 0.0f;
		m_shaderConstants[gradientTConstantsOffset++] = 0.0f;
	}
}

// ************************************************************************************************************************************************************

CRuShader_SMUNI_CloudDome::CRuShader_SMUNI_CloudDome(CRuEngineD3D *engine)
:	CRuShader_SMUNI_Base(engine)
{
	// Setup vertex elements
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },
		{ 0,	12,	ruELEMENTTYPE_RUCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },
		{ 0,	16,	ruELEMENTTYPE_FLOAT2,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_TEXCOORD,	0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexDecl = m_engine->CreateVertexDeclaration(vertexElements);

	char *vsSource = "\
																													\n\
	// Globals																										\n\
	float4x3			g_worldTransform		: register(c0);														\n\
	float4x3			g_viewTransform			: register(c3);														\n\
	float4x4			g_projectionTransform	: register(c6);														\n\
																													\n\
	float4				g_layerOffset[4]		: register(c27);													\n\
	float4				g_layerScale[2]			: register(c31);													\n\
	float4				g_cloudColor0			: register(c33);													\n\
	float4				g_cloudColor1			: register(c34);													\n\
																													\n\
	struct VS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR;															\n\
		float2			texcoord				: TEXCOORD;															\n\
	};																												\n\
																													\n\
	struct VS_OUTPUT																								\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse0				: COLOR0;															\n\
		float4			diffuse1				: COLOR1;															\n\
		float2			texcoord0				: TEXCOORD0;														\n\
		float2			texcoord1				: TEXCOORD1;														\n\
		float2			texcoord2				: TEXCOORD2;														\n\
		float2			texcoord3				: TEXCOORD3;														\n\
		float2			texcoord4				: TEXCOORD4;														\n\
		float2			texcoord5				: TEXCOORD5;														\n\
		float2			texcoord6				: TEXCOORD6;														\n\
		float2			texcoord7				: TEXCOORD7;														\n\
		float1			fog						: FOG;																\n\
	};																												\n\
																													\n\
	VS_OUTPUT main(const VS_INPUT v)																				\n\
	{																												\n\
		VS_OUTPUT output;																							\n\
																													\n\
		// Initialize world-view vectors to 1																		\n\
		float4 viewPos = float4(1, 1, 1, 1);																		\n\
																													\n\
		// Transform position to projected space																	\n\
		viewPos.xyz = mul(v.position, (float3x3) g_viewTransform);													\n\
		output.position = mul(viewPos, (float4x4) g_projectionTransform);											\n\
																													\n\
		// Set fog to 1																								\n\
		output.fog = 1.0f;																							\n\
																													\n\
		// Accumulate ambient color																					\n\
		output.diffuse0.xyz = g_cloudColor0;																		\n\
		output.diffuse1.xyz = g_cloudColor1;																		\n\
																													\n\
		output.diffuse0.w = v.diffuse.w;																			\n\
		output.diffuse1.w = v.diffuse.w;																			\n\
																													\n\
		// Copy texture coordinates																					\n\
		output.texcoord0 = (v.texcoord.xy + g_layerOffset[0].xy) * g_layerScale[0].x;								\n\
		output.texcoord1 = (v.texcoord.xy + g_layerOffset[0].zw) * g_layerScale[0].y;								\n\
		output.texcoord2 = (v.texcoord.xy + g_layerOffset[1].xy) * g_layerScale[0].z;								\n\
		output.texcoord3 = (v.texcoord.xy + g_layerOffset[1].zw) * g_layerScale[0].w;								\n\
		output.texcoord4 = (v.texcoord.xy + g_layerOffset[2].xy) * g_layerScale[1].x;								\n\
		output.texcoord5 = (v.texcoord.xy + g_layerOffset[2].zw) * g_layerScale[1].y;								\n\
		output.texcoord6 = (v.texcoord.xy + g_layerOffset[3].xy) * g_layerScale[1].z;								\n\
		output.texcoord7 = (v.texcoord.xy + g_layerOffset[3].zw) * g_layerScale[1].w;								\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource = "\
																													\n\
	sampler2D tex0 : register(s0);																					\n\
	sampler2D tex1 : register(s1);																					\n\
	sampler2D tex2 : register(s2);																					\n\
	sampler2D tex3 : register(s3);																					\n\
	sampler2D tex4 : register(s4);																					\n\
	sampler2D tex5 : register(s5);																					\n\
	sampler2D tex6 : register(s6);																					\n\
	sampler2D tex7 : register(s7);																					\n\
																													\n\
	// Globals																										\n\
	float4				g_cloudParameters		: register(c3);		// (cover, sharpness, 0, 0)						\n\
	float4				g_octaveWeights0		: register(c4);		// (octave0, octave1, octave2, octave3)			\n\
	float4				g_octaveWeights1		: register(c5);		// (octave4, octave5, octave6, octave7)			\n\
																													\n\
	struct PS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse0				: COLOR0;															\n\
		float4			diffuse1				: COLOR1;															\n\
		float2			texcoord0				: TEXCOORD0;														\n\
		float2			texcoord1				: TEXCOORD1;														\n\
		float2			texcoord2				: TEXCOORD2;														\n\
		float2			texcoord3				: TEXCOORD3;														\n\
		float2			texcoord4				: TEXCOORD4;														\n\
		float2			texcoord5				: TEXCOORD5;														\n\
		float2			texcoord6				: TEXCOORD6;														\n\
		float2			texcoord7				: TEXCOORD7;														\n\
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
		// Accumulate noise octaves																					\n\
		float4 fBm = tex2D(tex0, v.texcoord0) * g_octaveWeights0.x;													\n\
		fBm += tex2D(tex1, v.texcoord1) * g_octaveWeights0.y;														\n\
		fBm += tex2D(tex2, v.texcoord2) * g_octaveWeights0.z;														\n\
		fBm += tex2D(tex3, v.texcoord3) * g_octaveWeights0.w;														\n\
		fBm += tex2D(tex4, v.texcoord4) * g_octaveWeights1.x;														\n\
		fBm += tex2D(tex5, v.texcoord5) * g_octaveWeights1.y;														\n\
		fBm += tex2D(tex6, v.texcoord6) * g_octaveWeights1.z;														\n\
		fBm += tex2D(tex7, v.texcoord7) * g_octaveWeights1.w;														\n\
																													\n\
		// Subtract cloud cover value																				\n\
		fBm = max((fBm - g_cloudParameters.xxxx), 0.0f);															\n\
																													\n\
		// Exponentiate density value																				\n\
		fBm = 1.0f - pow(g_cloudParameters.yyyy, (fBm * 255.0f));													\n\
																													\n\
		// Output cloud color and opacity																			\n\
		output.color.xyz = lerp(v.diffuse0.xyz, v.diffuse1.xyz, fBm.x) * v.diffuse0.w;								\n\
		output.color.w = fBm.x * v.diffuse0.w;																		\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile vertex shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Compile pixel shader
	m_pixelShader = RuD3DCompilePixelShader(psSource, "main", "ps_2_0", NULL);

	// Set base constants count
	m_numBaseConstants = 35;
	m_numBasePixelShaderConstants = 6;
}

CRuShader_SMUNI_CloudDome::~CRuShader_SMUNI_CloudDome()
{
}

void CRuShader_SMUNI_CloudDome::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SMUNI_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_engine->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_REVSUBTRACT);
		m_engine->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
		m_engine->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(3, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetSamplerState(4, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(4, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(4, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(4, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(4, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetSamplerState(5, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(5, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(5, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(5, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(5, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetSamplerState(6, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(6, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(6, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(6, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(6, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetSamplerState(7, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(7, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(7, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(7, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(7, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}
}

void CRuShader_SMUNI_CloudDome::EndPass(INT32 passNumber)
{
	CRuShader_SMUNI_Base::EndPass(passNumber);

	m_engine->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
}

void CRuShader_SMUNI_CloudDome::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SMUNI_Base::SetParameters(paramCache);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
