#include "../../../Direct3D/Shader/SMUNI/RuShader_SMUNI_FX.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SMUNI_FX_GhostTrail::CRuShader_SMUNI_FX_GhostTrail(CRuEngineD3D *engine)
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
	float4				g_ambientColor			: register(c10);													\n\
	float4				g_shCoefficients[9]		: register(c11);	// Spherical harmonics coefficients				\n\
	float4				g_specularDirection		: register(c21);	// Object space specular position				\n\
	float4				g_specularColor			: register(c22);													\n\
	float4				g_selfIllumination		: register(c23);	// Self-illumination term						\n\
	float4				g_globalSettings		: register(c25);	// (Fog Constant, Start, End, Extrude Scale)	\n\
	float4				g_globalDiffuse			: register(c26);													\n\
	float4x4			g_uvTransform0			: register(c27);													\n\
																													\n\
	struct VS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR0;															\n\
		float2			uv_albedo				: TEXCOORD0;														\n\
	};																												\n\
																													\n\
	struct VS_OUTPUT																								\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR0;															\n\
		float2			uv_albedo				: TEXCOORD0;														\n\
		float1			fog						: TEXCOORD1;														\n\
	};																												\n\
																													\n\
	VS_OUTPUT main(const VS_INPUT v)																				\n\
	{																												\n\
		VS_OUTPUT output;																							\n\
																													\n\
		// Initialize world-view vectors to 1																		\n\
		float4 worldPos = float4(1, 1, 1, 1);																		\n\
		float4 viewPos = float4(1, 1, 1, 1);																		\n\
																													\n\
		// Transform position to projected space																	\n\
		worldPos.xyz = mul(v.position.xyz, (float4x3) g_worldTransform);											\n\
		viewPos.xyz = mul(worldPos, (float4x3) g_viewTransform);													\n\
		output.position = mul(viewPos, (float4x4) g_projectionTransform);											\n\
																													\n\
		// Calculate fog																							\n\
		float1 rcpFogRange = 1.0f / (g_globalSettings.z - g_globalSettings.y);										\n\
		float1 relativeZ = max(viewPos.z - g_globalSettings.y, 0.0f);												\n\
		output.fog = clamp(min(1.0f - (relativeZ * rcpFogRange), 1.0f - g_globalSettings.x), 0.0f, 1.0f);			\n\
																													\n\
		// Modulate final diffuse color by the global diffuse modifier												\n\
		output.diffuse = v.diffuse * g_globalDiffuse;																\n\
																													\n\
		// Transform vertex to uv space																				\n\
		output.uv_albedo = v.uv_albedo;																				\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource = "\
																													\n\
	float4				g_fogColor				: register(c1);														\n\
																													\n\
	sampler2D			texDiffuse				: register(s0);														\n\
																													\n\
	struct PS_INPUT																									\n\
	{																												\n\
		float4			diffuse					: COLOR;															\n\
		float2			uv_albedo				: TEXCOORD0;														\n\
		float1			fog						: TEXCOORD1;														\n\
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
		// Sample maps																								\n\
		float4 diffuse = tex2D(texDiffuse, v.uv_albedo);															\n\
																													\n\
		// Multiply by vertex color																					\n\
		float4 finalColor = diffuse * v.diffuse;																	\n\
																													\n\
		// Pre-multiply diffuse by alpha																			\n\
		finalColor.xyz = finalColor.xyz * finalColor.w;																\n\
																													\n\
		// Multiply fog by alpha																					\n\
		float4 fogColor = g_fogColor * finalColor.w;																\n\
																													\n\
		// Interpolate between diffuse color and fog color															\n\
		output.color = lerp(fogColor.xyzw, finalColor.xyzw, v.fog.xxxx);											\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile vertex shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Compile pixel shader
	m_pixelShader = RuD3DCompilePixelShader(psSource, "main", "ps_2_0", NULL);

	// Set constants count
	m_numBaseConstants = 27;
	m_numBasePixelShaderConstants = 2;
}

CRuShader_SMUNI_FX_GhostTrail::~CRuShader_SMUNI_FX_GhostTrail()
{
}

void CRuShader_SMUNI_FX_GhostTrail::BeginPass(INT32 passNumber)
{
	// Material is always translucent
	m_material_Translucent = TRUE;

	// Foce 2-sided polygon
	m_2Sided = TRUE;

	// Overrides
	m_override_ZWrite = TRUE;

	// Call base BeginPass to perform any additional setup
	CRuShader_SMUNI_Base::BeginPass(passNumber);

	// Setup SH9 + zero + 0.5 constants
	m_shaderConstants[80] = 3.0f;
	m_shaderConstants[81] = 1.0f;
	m_shaderConstants[82] = 0.5f;
	m_shaderConstants[83] = 0.0f;

	// Pixel shader basic constants
	m_pixelShaderConstants[0] = 1.0f;
	m_pixelShaderConstants[1] = 0.5f;
	m_pixelShaderConstants[2] = 0.0f;
	m_pixelShaderConstants[3] = 0.0f;

	// Set fog parameters
	INT32 fogConstantOffset = 100;
	m_shaderConstants[fogConstantOffset] = m_fog_Constant;

	INT32 fogStartOffset = 101;
	m_shaderConstants[fogStartOffset] = m_fog_Start;

	INT32 fogEndOffset = 102;
	m_shaderConstants[fogEndOffset] = m_fog_End;

	INT32 fogColorOffset = 4;
	m_pixelShaderConstants[fogColorOffset++] = m_fog_Color.m_r;
	m_pixelShaderConstants[fogColorOffset++] = m_fog_Color.m_g;
	m_pixelShaderConstants[fogColorOffset++] = m_fog_Color.m_b;
	m_pixelShaderConstants[fogColorOffset++] = m_fog_Color.m_a;

	// Set effective material diffuse
	INT32 globalDiffuseOffset = 104;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_r;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_g;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_b;

	// Set effective material alpha
	INT32 globalAlphaOffset = 107;
	m_shaderConstants[globalAlphaOffset] = m_material_EffectiveAlpha;

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		m_engine->UseDefaultSamplerFilterState(0);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

void CRuShader_SMUNI_FX_GhostTrail::EndPass(INT32 passNumber)
{
	CRuShader_SMUNI_Base::EndPass(passNumber);
}

void CRuShader_SMUNI_FX_GhostTrail::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SMUNI_Base::SetParameters(paramCache);
}

// ************************************************************************************************************************************************************

CRuShader_SMUNI_FX_Decal::CRuShader_SMUNI_FX_Decal(CRuEngineD3D *engine)
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
	float4				g_ambientColor			: register(c10);													\n\
	float4				g_shCoefficients[9]		: register(c11);	// Spherical harmonics coefficients				\n\
	float4				g_specularDirection		: register(c21);	// Object space specular position				\n\
	float4				g_specularColor			: register(c22);													\n\
	float4				g_selfIllumination		: register(c23);	// Self-illumination term						\n\
	float4				g_globalSettings		: register(c25);	// (Fog Constant, Start, End, Extrude Scale)	\n\
	float4				g_globalDiffuse			: register(c26);													\n\
	float4x4			g_uvTransform0			: register(c27);													\n\
																													\n\
	struct VS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR0;															\n\
	};																												\n\
																													\n\
	struct VS_OUTPUT																								\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR0;															\n\
		float4			uv0						: TEXCOORD0;														\n\
		float1			fog						: TEXCOORD1;														\n\
	};																												\n\
																													\n\
	VS_OUTPUT main(const VS_INPUT v)																				\n\
	{																												\n\
		VS_OUTPUT output;																							\n\
																													\n\
		// Initialize world-view vectors to 1																		\n\
		float4 worldPos = float4(1, 1, 1, 1);																		\n\
		float4 viewPos = float4(1, 1, 1, 1);																		\n\
																													\n\
		// Transform position to projected space																	\n\
		worldPos.xyz = v.position.xyz;																				\n\
		viewPos.xyz = mul(worldPos, (float4x3) g_viewTransform);													\n\
		output.position = mul(viewPos, (float4x4) g_projectionTransform);											\n\
																													\n\
		// Calculate fog																							\n\
		float1 rcpFogRange = 1.0f / (g_globalSettings.z - g_globalSettings.y);										\n\
		float1 relativeZ = max(viewPos.z - g_globalSettings.y, 0.0f);												\n\
		output.fog = clamp(min(1.0f - (relativeZ * rcpFogRange), 1.0f - g_globalSettings.x), 0.0f, 1.0f);			\n\
																													\n\
		// Modulate final diffuse color by the global diffuse modifier												\n\
		output.diffuse = v.diffuse * g_globalDiffuse;																\n\
																													\n\
		// Transform vertex to uv space																				\n\
		output.uv0 = mul(v.position, (float4x4) g_uvTransform0);													\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource = "\
																													\n\
	float4				g_fogColor				: register(c1);														\n\
																													\n\
	sampler2D			texDiffuse				: register(s0);														\n\
																													\n\
	struct PS_INPUT																									\n\
	{																												\n\
		float4			diffuse					: COLOR0;															\n\
		float4			uv_albedo				: TEXCOORD0;														\n\
		float1			fog						: TEXCOORD1;														\n\
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
		float4 uv_pers = v.uv_albedo / v.uv_albedo.w;																\n\
																													\n\
		// Sample diffuse map																						\n\
		float4 diffuse = tex2D(texDiffuse, uv_pers);																\n\
																													\n\
		// Multiply by vertex color																					\n\
		float4 finalColor = diffuse * v.diffuse;																	\n\
																													\n\
		// Pre-multiply diffuse by alpha																			\n\
		finalColor.xyz = finalColor.xyz * finalColor.w;																\n\
																													\n\
		// Multiply fog by alpha																					\n\
		float4 fogColor = g_fogColor * finalColor.w;																\n\
																													\n\
		// Interpolate between diffuse color and fog color															\n\
		output.color = lerp(fogColor.xyzw, finalColor.xyzw, v.fog.xxxx);											\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile vertex shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Compile pixel shader
	m_pixelShader = RuD3DCompilePixelShader(psSource, "main", "ps_2_0", NULL);

	// Set constants count
	m_numBaseConstants = 31;
	m_numBasePixelShaderConstants = 2;
}

CRuShader_SMUNI_FX_Decal::~CRuShader_SMUNI_FX_Decal()
{
}

void CRuShader_SMUNI_FX_Decal::BeginPass(INT32 passNumber)
{
	// Force 2-sided polygon
	m_2Sided = TRUE;

	// Overrides
	m_override_ZWrite = TRUE;

	// Call base BeginPass to perform any additional setup
	CRuShader_SMUNI_Base::BeginPass(passNumber);

	// Setup SH9 + zero + 0.5 constants
	m_shaderConstants[80] = 3.0f;
	m_shaderConstants[81] = 1.0f;
	m_shaderConstants[82] = 0.5f;
	m_shaderConstants[83] = 0.0f;

	// Pixel shader basic constants
	m_pixelShaderConstants[0] = 1.0f;
	m_pixelShaderConstants[1] = 0.5f;
	m_pixelShaderConstants[2] = 0.0f;
	m_pixelShaderConstants[3] = 0.0f;

	// Set fog parameters
	INT32 fogConstantOffset = 100;
	m_shaderConstants[fogConstantOffset] = m_fog_Constant;

	INT32 fogStartOffset = 101;
	m_shaderConstants[fogStartOffset] = m_fog_Start;

	INT32 fogEndOffset = 102;
	m_shaderConstants[fogEndOffset] = m_fog_End;

	INT32 fogColorOffset = 4;
	m_pixelShaderConstants[fogColorOffset++] = m_fog_Color.m_r;
	m_pixelShaderConstants[fogColorOffset++] = m_fog_Color.m_g;
	m_pixelShaderConstants[fogColorOffset++] = m_fog_Color.m_b;
	m_pixelShaderConstants[fogColorOffset++] = m_fog_Color.m_a;

	// Set effective material diffuse
	INT32 globalDiffuseOffset = 104;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_r;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_g;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_b;

	// Set effective material alpha
	INT32 globalAlphaOffset = 107;
	m_shaderConstants[globalAlphaOffset] = m_material_EffectiveAlpha;

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, (DWORD)RuCastFloatToINT64(-1.0f));

		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

void CRuShader_SMUNI_FX_Decal::EndPass(INT32 passNumber)
{
	CRuShader_SMUNI_Base::EndPass(passNumber);

	m_engine->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, (DWORD)RuCastFloatToINT64(0.0f));
}

void CRuShader_SMUNI_FX_Decal::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SMUNI_Base::SetParameters(paramCache);

	UINT64 param;

	if(paramCache->GetParameter(ruPARAMTYPE_TEXTURE_TRANSFORM_0, param))
	{
		INT32 uvTransformOffset = 108;

		CRuMatrix4x4 *transform = (CRuMatrix4x4 *) param;

		for(UINT32 i = 0, k = uvTransformOffset; i < 4; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k] = transform->m_elements[j][i];
			}
		}
	}

}

// ************************************************************************************************************************************************************

CRuShader_SMUNI_FX_DecalShadow::CRuShader_SMUNI_FX_DecalShadow(CRuEngineD3D *engine)
:	CRuShader_SMUNI_Base(engine)
{
	// Setup vertex elements
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },
		{ 0,	12,	ruELEMENTTYPE_RUCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },
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
	float4				g_globalSettings		: register(c25);	// (Fog Constant, Start, End, Extrude Scale)	\n\
	float4				g_globalDiffuse			: register(c26);													\n\
	float4x4			g_mappingTransform		: register(c27);													\n\
	float4x4			g_clampingTransform		: register(c31);													\n\
																													\n\
	struct VS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR;															\n\
		float2			uv_albedo				: TEXCOORD;															\n\
	};																												\n\
																													\n\
	struct VS_OUTPUT																								\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR;															\n\
		float4			uv_albedo				: TEXCOORD0;														\n\
		float4			uv_clamp				: TEXCOORD1;														\n\
		float1			fog						: TEXCOORD2;														\n\
	};																												\n\
																													\n\
	VS_OUTPUT main(const VS_INPUT v)																				\n\
	{																												\n\
		VS_OUTPUT output;																							\n\
																													\n\
		// Initialize world-view vectors to 1																		\n\
		float4 worldPos = float4(1, 1, 1, 1);																		\n\
		float4 viewPos = float4(1, 1, 1, 1);																		\n\
																													\n\
		// Transform position to projected space																	\n\
		worldPos.xyz = v.position.xyz;																				\n\
		viewPos.xyz = mul(worldPos, (float4x3) g_viewTransform);													\n\
		output.position = mul(viewPos, (float4x4) g_projectionTransform);											\n\
																													\n\
		// Calculate fog																							\n\
		float1 rcpFogRange = 1.0f / (g_globalSettings.z - g_globalSettings.y);										\n\
		float1 relativeZ = max(viewPos.z - g_globalSettings.y, 0.0f);												\n\
		output.fog = clamp(min(1.0f - (relativeZ * rcpFogRange), 1.0f - g_globalSettings.x), 0.0f, 1.0f);			\n\
																													\n\
		// Modulate final diffuse color by the global diffuse modifier												\n\
		output.diffuse = v.diffuse * g_globalDiffuse;																\n\
																													\n\
		// Calculate albedo texture coordinates																		\n\
		output.uv_albedo = mul(v.position, (float4x4) g_mappingTransform);											\n\
																													\n\
		// Calculate clamping texture coordinates																	\n\
		output.uv_clamp = mul(v.position, (float4x4) g_clampingTransform);											\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource = "\
																													\n\
	float4				g_fogColor				: register(c1);														\n\
																													\n\
	sampler2D			texDiffuse				: register(s0);														\n\
	sampler2D			texClamp				: register(s1);														\n\
																													\n\
	struct PS_INPUT																									\n\
	{																												\n\
		float4			diffuse					: COLOR;															\n\
		float4			uv_albedo				: TEXCOORD0;														\n\
		float4			uv_clamp				: TEXCOORD1;														\n\
		float1			fog						: TEXCOORD2;														\n\
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
		// Perform perspective divide																				\n\
		float2 uv_albedo = v.uv_albedo.xy / v.uv_albedo.w;															\n\
		float2 uv_clamp = v.uv_clamp.xy / v.uv_clamp.w;																\n\
																													\n\
		// Sample maps																								\n\
		float4 diffuse = tex2D(texDiffuse, uv_albedo);																\n\
		float4 clamp = tex2D(texClamp, uv_clamp);																	\n\
																													\n\
		// Multiply by vertex color																					\n\
		float4 finalColor = float4(1, 1, 1, 1);																		\n\
		finalColor.xyz = float3(0.0f, 0.0f, 0.0f);																	\n\
		finalColor.w = diffuse.x * clamp.x * v.diffuse.x;															\n\
																													\n\
		// Pre-multiply diffuse by alpha																			\n\
		finalColor.xyz = finalColor.xyz * finalColor.w;																\n\
																													\n\
		// Multiply fog by alpha																					\n\
		float4 fogColor = g_fogColor * finalColor.w;																\n\
																													\n\
		// Interpolate between diffuse color and fog color															\n\
		output.color = lerp(fogColor.xyzw, finalColor.xyzw, v.fog.xxxx);											\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile vertex shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Compile pixel shader
	m_pixelShader = RuD3DCompilePixelShader(psSource, "main", "ps_2_0", NULL);

	// Overrides
	m_override_AlphaTest = TRUE;
	m_override_AlphaBlend = TRUE;
	m_override_ZWrite = TRUE;

	// Set constants count
	m_numBaseConstants = 35;
	m_numBasePixelShaderConstants = 2;
}

CRuShader_SMUNI_FX_DecalShadow::~CRuShader_SMUNI_FX_DecalShadow()
{
}

void CRuShader_SMUNI_FX_DecalShadow::BeginPass(INT32 passNumber)
{
	// Force 2-sided polygon
	m_2Sided = TRUE;

	// Force translucency
	m_material_Translucent = TRUE;

	// Disable alpha test
	m_alphaTestEnabled = FALSE;

	// Call base BeginPass to perform any additional setup
	CRuShader_SMUNI_Base::BeginPass(passNumber);

	// Setup SH9 + zero + 0.5 constants
	m_shaderConstants[80] = 3.0f;
	m_shaderConstants[81] = 1.0f;
	m_shaderConstants[82] = 0.5f;
	m_shaderConstants[83] = 0.0f;

	// Set effective material diffuse
	INT32 globalDiffuseOffset = 104;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_r;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_g;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_b;

	// Set effective material alpha
	INT32 globalAlphaOffset = 107;
	m_shaderConstants[globalAlphaOffset] = m_material_EffectiveAlpha;

	// Set fog parameters
	INT32 fogConstantOffset = 100;
	m_shaderConstants[fogConstantOffset] = m_fog_Constant;

	INT32 fogStartOffset = 101;
	m_shaderConstants[fogStartOffset] = m_fog_Start;

	INT32 fogEndOffset = 102;
	m_shaderConstants[fogEndOffset] = m_fog_End;

	m_engine->SetRenderState(D3DRS_FOGCOLOR, m_fog_Color.GetARGB());

	INT32 fogColorOffset = 4;
	m_pixelShaderConstants[fogColorOffset++] = m_fog_Color.m_r;
	m_pixelShaderConstants[fogColorOffset++] = m_fog_Color.m_g;
	m_pixelShaderConstants[fogColorOffset++] = m_fog_Color.m_b;
	m_pixelShaderConstants[fogColorOffset++] = m_fog_Color.m_a;

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		m_engine->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, (DWORD)RuCastFloatToINT64(-1.0f));

		m_engine->UseDefaultSamplerFilterState(1);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		m_engine->UseDefaultSamplerFilterState(1);
		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

void CRuShader_SMUNI_FX_DecalShadow::EndPass(INT32 passNumber)
{
	CRuShader_SMUNI_Base::EndPass(passNumber);

	m_engine->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, (DWORD)RuCastFloatToINT64(0.0f));
}

void CRuShader_SMUNI_FX_DecalShadow::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SMUNI_Base::SetParameters(paramCache);

	UINT64 param;

	if(paramCache->GetParameter(ruPARAMTYPE_TEXTURE_TRANSFORM_0, param))
	{
		INT32 uvTransformOffset = 108;

		CRuMatrix4x4 *transform = (CRuMatrix4x4 *) param;

		for(UINT32 i = 0, k = uvTransformOffset; i < 4; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k] = transform->m_elements[j][i];
			}
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_TEXTURE_TRANSFORM_1, param))
	{
		INT32 uvTransformOffset = 124;

		CRuMatrix4x4 *transform = (CRuMatrix4x4 *) param;

		for(UINT32 i = 0, k = uvTransformOffset; i < 4; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k] = transform->m_elements[j][i];
			}
		}
	}
}

// ************************************************************************************************************************************************************

CRuShader_SMUNI_FX_Lightning::CRuShader_SMUNI_FX_Lightning(CRuEngineD3D *engine)
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
	float4				g_globalSettings		: register(c25);	// (Fog Constant, Start, End, Extrude Scale)	\n\
	float4				g_globalDiffuse			: register(c26);													\n\
																													\n\
	struct VS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR;															\n\
		float2			uv_albedo				: TEXCOORD;															\n\
	};																												\n\
																													\n\
	struct VS_OUTPUT																								\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR;															\n\
		float2			uv_albedo				: TEXCOORD0;														\n\
		float1			fog						: TEXCOORD1;														\n\
	};																												\n\
																													\n\
	VS_OUTPUT main(const VS_INPUT v)																				\n\
	{																												\n\
		VS_OUTPUT output;																							\n\
																													\n\
		// Initialize world-view vectors to 1																		\n\
		float4 worldPos = float4(1, 1, 1, 1);																		\n\
		float4 viewPos = float4(1, 1, 1, 1);																		\n\
																													\n\
		// Transform position to projected space																	\n\
		worldPos.xyz = mul(v.position, (float4x3) g_worldTransform);												\n\
		viewPos.xyz = mul(worldPos, (float4x3) g_viewTransform);													\n\
		output.position = mul(viewPos, (float4x4) g_projectionTransform);											\n\
																													\n\
		// Calculate fog																							\n\
		float1 rcpFogRange = 1.0f / (g_globalSettings.z - g_globalSettings.y);										\n\
		float1 relativeZ = max(viewPos.z - g_globalSettings.y, 0.0f);												\n\
		output.fog = clamp(min(1.0f - (relativeZ * rcpFogRange), 1.0f - g_globalSettings.x), 0.0f, 1.0f);			\n\
																													\n\
		// Accumulate ambient color																					\n\
		output.diffuse = g_globalDiffuse;																			\n\
																													\n\
		output.uv_albedo = v.uv_albedo;																				\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource = "\
																													\n\
	float4				g_fogColor				: register(c1);														\n\
																													\n\
	sampler2D			texDiffuse				: register(s0);														\n\
																													\n\
	struct PS_INPUT																									\n\
	{																												\n\
		float4			diffuse					: COLOR;															\n\
		float2			uv_albedo				: TEXCOORD0;														\n\
		float1			fog						: TEXCOORD1;														\n\
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
		// Sample maps																								\n\
		float4 diffuse = tex2D(texDiffuse, v.uv_albedo);															\n\
																													\n\
		// Multiply by vertex color																					\n\
		float4 finalColor = diffuse * v.diffuse;																	\n\
																													\n\
		// Pre-multiply diffuse by alpha																			\n\
		finalColor.xyz = finalColor.xyz * finalColor.w;																\n\
																													\n\
		// Multiply fog by alpha																					\n\
		float4 fogColor = g_fogColor * finalColor.w;																\n\
																													\n\
		// Interpolate between diffuse color and fog color															\n\
		output.color = lerp(fogColor.xyzw, finalColor.xyzw, v.fog.xxxx);											\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile vertex shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Compile pixel shader
	m_pixelShader = RuD3DCompilePixelShader(psSource, "main", "ps_2_0", NULL);

	// Set constants count
	m_numBaseConstants = 30;
	m_numBasePixelShaderConstants = 2;
}

CRuShader_SMUNI_FX_Lightning::~CRuShader_SMUNI_FX_Lightning()
{
}

void CRuShader_SMUNI_FX_Lightning::BeginPass(INT32 passNumber)
{
	// Force 2-sided polygon
	m_2Sided = TRUE;

	// Call base BeginPass to perform any additional setup
	CRuShader_SMUNI_Base::BeginPass(passNumber);

	// Setup SH9 + zero + 0.5 constants
	m_shaderConstants[80] = 3.0f;
	m_shaderConstants[81] = 1.0f;
	m_shaderConstants[82] = 0.5f;
	m_shaderConstants[83] = 0.0f;

	// Pixel shader basic constants
	m_pixelShaderConstants[0] = 1.0f;
	m_pixelShaderConstants[1] = 0.5f;
	m_pixelShaderConstants[2] = 0.0f;
	m_pixelShaderConstants[3] = 0.0f;

	// Set effective material diffuse
	INT32 globalDiffuseOffset = 104;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_r;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_g;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_b;

	// Set effective material alpha
	INT32 globalAlphaOffset = 107;
	m_shaderConstants[globalAlphaOffset] = m_material_EffectiveAlpha;

	// Set fog parameters
	INT32 fogConstantOffset = 100;
	m_shaderConstants[fogConstantOffset] = m_fog_Constant;

	INT32 fogStartOffset = 101;
	m_shaderConstants[fogStartOffset] = m_fog_Start;

	INT32 fogEndOffset = 102;
	m_shaderConstants[fogEndOffset] = m_fog_End;

	m_engine->SetRenderState(D3DRS_FOGCOLOR, m_fog_Color.GetARGB());

	INT32 fogColorOffset = 4;
	m_pixelShaderConstants[fogColorOffset++] = m_fog_Color.m_r;
	m_pixelShaderConstants[fogColorOffset++] = m_fog_Color.m_g;
	m_pixelShaderConstants[fogColorOffset++] = m_fog_Color.m_b;
	m_pixelShaderConstants[fogColorOffset++] = m_fog_Color.m_a;

	// Set self-illumination parameters
	INT32 selfIllumRGBOffset = 23 * 4;
	m_shaderConstants[selfIllumRGBOffset++] = m_material_SelfIlluminationColor.m_r;
	m_shaderConstants[selfIllumRGBOffset++] = m_material_SelfIlluminationColor.m_g;
	m_shaderConstants[selfIllumRGBOffset++] = m_material_SelfIlluminationColor.m_b;

	INT32 selfIllumOffset = 23 * 4 + 3;
	m_shaderConstants[selfIllumOffset] = m_material_SelfIllumination;

	// Set extrude scale
	INT32 extrudeScaleOffset = 103;
	m_shaderConstants[extrudeScaleOffset] = m_fx_ExtrudeScale;

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		m_engine->UseDefaultSamplerFilterState(0);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

void CRuShader_SMUNI_FX_Lightning::EndPass(INT32 passNumber)
{
	CRuShader_SMUNI_Base::EndPass(passNumber);
}

void CRuShader_SMUNI_FX_Lightning::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SMUNI_Base::SetParameters(paramCache);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
