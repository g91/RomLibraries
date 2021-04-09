#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Terrain.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Terrain_Base::CRuShader_SM20EZ_Terrain_Base(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Base(engine)
{
	// Setup vertex elements
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },
		{ 0,	12,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_NORMAL,		0 },
		{ 0,	24,	ruELEMENTTYPE_RUCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },
		{ 0,	28,	ruELEMENTTYPE_FLOAT2,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_TEXCOORD,	0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexDecl = m_engine->CreateVertexDeclaration(vertexElements);

	// Set base constants count
	m_numBaseConstants = 34;
}

CRuShader_SM20EZ_Terrain_Base::~CRuShader_SM20EZ_Terrain_Base()
{
}

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Terrain::CRuShader_SM20EZ_Terrain(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Terrain_Base(engine),
	m_maxLayersPerPass(6),
	m_maxTextureLayers(12),
	m_numTextureLayers(0)
{
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
	float4				g_alphaUVModifier		: register(c27);													\n\
	float4				g_uvStretchModifiers[6]	: register(c28);													\n\
																													\n\
	struct VS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float3			normal					: NORMAL;															\n\
		float4			diffuse					: COLOR0;															\n\
		float2			texcoord				: TEXCOORD;															\n\
	};																												\n\
																													\n\
	struct VS_OUTPUT																								\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR0;															\n\
		float4			specular				: COLOR1;															\n\
		float2			uv_splat				: TEXCOORD0;														\n\
		float2			uv_layer0				: TEXCOORD1;														\n\
		float2			uv_layer1				: TEXCOORD2;														\n\
		float2			uv_layer2				: TEXCOORD3;														\n\
		float2			uv_layer3				: TEXCOORD4;														\n\
		float2			uv_layer4				: TEXCOORD5;														\n\
		float2			uv_layer5				: TEXCOORD6;														\n\
		float1			fog						: TEXCOORD7;														\n\
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
		worldPos.xyz = v.position.xyz + v.normal.xyz * g_globalSettings.w;											\n\
		worldPos.xyz = mul(worldPos, (float4x3) g_worldTransform);													\n\
		viewPos.xyz = mul(worldPos, (float4x3) g_viewTransform);													\n\
		output.position = mul(viewPos, (float4x4) g_projectionTransform);											\n\
																													\n\
		// Calculate fog																							\n\
		float1 rcpFogRange = 1.0f / (g_globalSettings.z - g_globalSettings.y);										\n\
		float1 relativeZ = max(viewPos.z - g_globalSettings.y, 0.0f);												\n\
		output.fog = clamp(min(1.0f - (relativeZ * rcpFogRange), 1.0f - g_globalSettings.x), 0.0f, 1.0f);			\n\
																													\n\
		// Accumulate ambient color																					\n\
		output.diffuse = g_ambientColor;																			\n\
																													\n\
		// Accumulate SH color																						\n\
		output.diffuse += g_shCoefficients[0];																		\n\
		output.diffuse += g_shCoefficients[1] * (float4) v.normal.x;												\n\
		output.diffuse += g_shCoefficients[2] * (float4) v.normal.y;												\n\
		output.diffuse += g_shCoefficients[3] * (float4) v.normal.z;												\n\
		output.diffuse += g_shCoefficients[4] * (float4) (v.normal.x * v.normal.z);									\n\
		output.diffuse += g_shCoefficients[5] * (float4) (v.normal.z * v.normal.y);									\n\
		output.diffuse += g_shCoefficients[6] * (float4) (v.normal.y * v.normal.x);									\n\
		output.diffuse += g_shCoefficients[7] * (float4) (3 * v.normal.z * v.normal.z - 1);							\n\
		output.diffuse += g_shCoefficients[8] * (float4) (v.normal.x * v.normal.x - v.normal.y * v.normal.y);		\n\
																													\n\
		// Lerp between real lighting color and self-illumination color based on self-illumination strength			\n\
		output.diffuse.xyz = lerp(output.diffuse.xyz, g_selfIllumination.xyz, g_selfIllumination.w);				\n\
																													\n\
		// Set lighting alpha to 1																					\n\
		output.diffuse.w = 1;																						\n\
																													\n\
		// Modulate lighting color with vertex color																\n\
		output.diffuse *= v.diffuse;																				\n\
																													\n\
		// Modulate final diffuse color by the global diffuse modifier												\n\
		output.diffuse *= g_globalDiffuse;																			\n\
																													\n\
		// Transform normal to view space																			\n\
		float3 N = normalize(mul(mul(v.normal, g_worldTransform), g_viewTransform));								\n\
																													\n\
		// Specular																									\n\
		float3 V = normalize(-viewPos.xyz);																			\n\
		float3 L = g_specularDirection;																				\n\
																													\n\
		L = normalize(mul(mul(L, (float3x3) g_worldTransform), (float3x3) g_viewTransform));						\n\
																													\n\
		float3 H = normalize(V + L);																				\n\
		float1 NL = dot(N, L);																						\n\
																													\n\
		if(NL > 0.0f)																								\n\
			NL = 1.0f;																								\n\
		else																										\n\
			NL = 0.0f;																								\n\
																													\n\
		float1 HN = clamp(dot(H, N), 0.0f, 1.0f);																	\n\
		float1 specularPower = pow(HN, 32) * NL * 3.0f;																\n\
																													\n\
		output.specular.xyz = g_specularColor * specularPower.xxxx;													\n\
		output.specular.w = 1.0f;																					\n\
																													\n\
		// Output splat map UV																						\n\
		output.uv_splat = v.texcoord;																				\n\
																													\n\
		// Output texture UV																						\n\
		output.uv_layer0 = v.texcoord.xy * g_uvStretchModifiers[0].xy;												\n\
		output.uv_layer1 = v.texcoord.xy * g_uvStretchModifiers[1].xy;												\n\
		output.uv_layer2 = v.texcoord.xy * g_uvStretchModifiers[2].xy;												\n\
		output.uv_layer3 = v.texcoord.xy * g_uvStretchModifiers[3].xy;												\n\
		output.uv_layer4 = v.texcoord.xy * g_uvStretchModifiers[4].xy;												\n\
		output.uv_layer5 = v.texcoord.xy * g_uvStretchModifiers[5].xy;												\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource_SM2_D2 = "\
																													\n\
	float4				g_constants				: register(c0);														\n\
	float4				g_fogColor				: register(c1);														\n\
																													\n\
	sampler2D			texAlpha0				: register(s0);														\n\
	sampler2D			texLayer0				: register(s1);														\n\
	sampler2D			texLayer1				: register(s2);														\n\
	sampler2D			texLayer2				: register(s3);														\n\
	sampler2D			texAlpha1				: register(s4);														\n\
	sampler2D			texLayer3				: register(s5);														\n\
	sampler2D			texLayer4				: register(s6);														\n\
	sampler2D			texLayer5				: register(s7);														\n\
																													\n\
	struct PS_INPUT																									\n\
	{																												\n\
		float4			diffuse					: COLOR0;															\n\
		float4			specular				: COLOR1;															\n\
		float2			uv_splat				: TEXCOORD0;														\n\
		float2			uv_layer0				: TEXCOORD1;														\n\
		float2			uv_layer1				: TEXCOORD2;														\n\
		float2			uv_layer2				: TEXCOORD3;														\n\
		float2			uv_layer3				: TEXCOORD4;														\n\
		float2			uv_layer4				: TEXCOORD5;														\n\
		float2			uv_layer5				: TEXCOORD6;														\n\
		float1			fog						: TEXCOORD7;														\n\
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
		// Sample layer weight textures																				\n\
		float4 weight0 = tex2D(texAlpha0, v.uv_splat);																\n\
		float4 weight1 = tex2D(texAlpha1, v.uv_splat);																\n\
																													\n\
		// Sample layer textures																					\n\
		float4 layer0 = tex2D(texLayer0, v.uv_layer0);																\n\
		float4 layer1 = tex2D(texLayer1, v.uv_layer1);																\n\
		float4 layer2 = tex2D(texLayer2, v.uv_layer2);																\n\
		float4 layer3 = tex2D(texLayer3, v.uv_layer3);																\n\
		float4 layer4 = tex2D(texLayer4, v.uv_layer4);																\n\
		float4 layer5 = tex2D(texLayer5, v.uv_layer5);																\n\
																													\n\
		// Calculate sum of all layer weights																		\n\
		float1 alphaSum = dot(weight0, g_constants.xxx) + dot(weight1, g_constants.xxx);							\n\
																													\n\
		// Scale weights such that the total adds up to one															\n\
		weight0 /= alphaSum.xxxx;																					\n\
		weight1 /= alphaSum.xxxx;																					\n\
																													\n\
		// Accumulate albedo																						\n\
		float4 albedo = layer0 * weight0.xxxx;																		\n\
		albedo += layer1 * weight0.yyyy;																			\n\
		albedo += layer2 * weight0.zzzz;																			\n\
		albedo += layer3 * weight1.xxxx;																			\n\
		albedo += layer4 * weight1.yyyy;																			\n\
		albedo += layer5 * weight1.zzzz;																			\n\
																													\n\
		// Calculate output color																					\n\
		output.color = albedo * v.diffuse + v.specular * albedo.wwww;												\n\
																													\n\
		// Lerp between output color and fog																		\n\
		output.color = lerp(g_fogColor.xyzw, output.color, v.fog.xxxx);												\n\
																													\n\
		// Set output alpha to one																					\n\
		output.color.w = 1.0f;																						\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile vertex shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Compile pixel shader
	m_pixelShader_SM2 = RuD3DCompilePixelShader(psSource_SM2_D2, "main", "ps_2_0", NULL);

	// Overrides
	m_override_ZWrite = TRUE;
}

CRuShader_SM20EZ_Terrain::~CRuShader_SM20EZ_Terrain()
{
	ruSAFE_RELEASE(m_pixelShader_SM2);
}

void CRuShader_SM20EZ_Terrain::BeginPass(INT32 passNumber)
{
	// Override raw data offset
	m_rawData_Offset = 28;

	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Terrain_Base::BeginPass(passNumber);

	m_vertexDecl->Select();
	m_engine->SetVertexShader(m_vertexShader);
	m_engine->SetPixelShader(m_pixelShader_SM2);

	// Set shader constants
	m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants);
	m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

	for(INT32 i = 0; i < 8; ++i)
	{
		m_engine->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->UseDefaultSamplerFilterState(i);
	}

	m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_engine->SetSamplerState(4, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_engine->SetSamplerState(4, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	m_engine->SetSamplerState(4, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_engine->SetSamplerState(4, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_engine->SetSamplerState(4, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_engine->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
}

void CRuShader_SM20EZ_Terrain::EndPass(INT32 passNumber)
{
	CRuShader_SM20EZ_Terrain_Base::EndPass(passNumber);
}

void CRuShader_SM20EZ_Terrain::SetParameters(const CRuParameterCache *paramCache)
{
	UINT64 param;

	CRuShader_SM20EZ_Terrain_Base::SetParameters(paramCache);

	if(paramCache->GetParameter(ruPARAMTYPE_SPLAT_ALPHAUVMODIFIER, param))
	{
		if(param)
		{
			INT32 uvModifierConstantsOffset = 27 * 4;
			float *uvModifier = (float *) param;

			m_shaderConstants[uvModifierConstantsOffset++] = uvModifier[0];
			m_shaderConstants[uvModifierConstantsOffset++] = uvModifier[1];
			m_shaderConstants[uvModifierConstantsOffset++] = uvModifier[2];
			m_shaderConstants[uvModifierConstantsOffset++] = uvModifier[3];
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SPLAT_LAYERCOUNT, param))
	{
		// Set the number of texture layers to blend
		m_numTextureLayers = (INT32)param;

		if(m_numTextureLayers == 0)
		{
			m_numPasses = 1;
		}
		else
		{
			// If the number of texture layers requested exceeds the maximum allowed, cap it
			if(m_numTextureLayers > m_maxTextureLayers)
			{
				m_numTextureLayers = m_maxTextureLayers;
			}

			// Calculate the number of passes
			m_numPasses = (m_numTextureLayers / m_maxLayersPerPass) + ((m_numTextureLayers % m_maxLayersPerPass) ? 1 : 0);

			// Pixel Shader 1.1 has one extra fog pass
			if(m_maxLayersPerPass == 3)
			{
				m_numPasses += 1;
			}
		}
	}
}

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Terrain_Packed_Base::CRuShader_SM20EZ_Terrain_Packed_Base(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Base(engine)
{
	// Setup vertex elements
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_UBYTE4,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },
		{ 0,	4,	ruELEMENTTYPE_RUCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexDecl = m_engine->CreateVertexDeclaration(vertexElements);

	// Overrides
	m_override_ZWrite = TRUE;

	// Set base constants count
	m_numBaseConstants = 35;
}

CRuShader_SM20EZ_Terrain_Packed_Base::~CRuShader_SM20EZ_Terrain_Packed_Base()
{
}

void CRuShader_SM20EZ_Terrain_Packed_Base::SetParameters(const CRuParameterCache *paramCache)
{
	// Chain to base class
	CRuShader_SM20EZ_Base::SetParameters(paramCache);

	UINT64 param;

	if(paramCache->GetParameter(ruPARAMTYPE_CHUNK_SPACING, param))
	{
		m_chunk_Spacing = RuCastINT64ToFloat(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_CHUNK_Y_BASE, param))
	{
		m_chunk_Y_Base = RuCastINT64ToFloat(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_CHUNK_Y_INCREMENT, param))
	{
		m_chunk_Y_Increment = RuCastINT64ToFloat(param);
	}

	INT32 constOffset_spc_base_inc_x = 27 * 4;
	m_shaderConstants[constOffset_spc_base_inc_x++] = m_chunk_Spacing;
	m_shaderConstants[constOffset_spc_base_inc_x++] = m_chunk_Y_Base;
	m_shaderConstants[constOffset_spc_base_inc_x++] = m_chunk_Y_Increment;
}

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Terrain_Packed::CRuShader_SM20EZ_Terrain_Packed(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Terrain_Packed_Base(engine),
	m_maxLayersPerPass(6),
	m_maxTextureLayers(12),
	m_numTextureLayers(0)
{
	m_override_Fog = FALSE;
	m_override_AlphaTest = TRUE;
	m_override_AlphaBlend = TRUE;

	char *vsSource_Simple = "\
																													\n\
	// Globals																										\n\
	float4x3			g_worldTransform		: register(c0);														\n\
	float4x3			g_viewTransform			: register(c3);														\n\
	float4x4			g_projectionTransform	: register(c6);														\n\
																													\n\
	float4				g_ambientColor			: register(c10);													\n\
	float4				g_shCoefficients[9]		: register(c11);		// Spherical harmonics coefficients			\n\
	float4				g_specularDirection		: register(c21);		// Object space specular position			\n\
	float4				g_specularColor			: register(c22);													\n\
	float4				g_selfIllumination		: register(c23);		// Self-illumination term					\n\
	float4				g_globalSettings		: register(c25);		// (?, Fog Start, Fog End, Tween Factor)	\n\
	float4				g_globalDiffuse			: register(c26);													\n\
	float4				g_spc_base_inc_x		: register(c27);		// (spacing, y base, y increment, undefined)\n\
	float4				g_alphaUVModifier		: register(c28);													\n\
	float4				g_uvStretchModifiers[6]	: register(c29);													\n\
																													\n\
	struct VS_INPUT																									\n\
	{																												\n\
		float4			x_z_yH_yL				: POSITION;															\n\
		float4			diffuse					: COLOR0;				// Packed as (0xAAXXYYZZ)					\n\
	};																												\n\
																													\n\
	struct VS_OUTPUT																								\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR0;															\n\
		float4			specular				: COLOR1;															\n\
		float1			fog						: TEXCOORD7;														\n\
	};																												\n\
																													\n\
	VS_OUTPUT main(const VS_INPUT v)																				\n\
	{																												\n\
		VS_OUTPUT output;																							\n\
																													\n\
		// Initialize world-view vectors to 1																		\n\
		float4 localPos = float4(1, 1, 1, 1);																		\n\
		float4 worldPos = float4(1, 1, 1, 1);																		\n\
		float4 viewPos = float4(1, 1, 1, 1);																		\n\
																													\n\
		// Unpack world position																					\n\
		localPos.x = v.x_z_yH_yL.x * g_spc_base_inc_x.x;															\n\
		localPos.z = v.x_z_yH_yL.y * g_spc_base_inc_x.x;															\n\
		localPos.y = (v.x_z_yH_yL.z * 255 + v.x_z_yH_yL.w) * g_spc_base_inc_x.z + g_spc_base_inc_x.y;				\n\
																													\n\
		// Transform position to projected space																	\n\
		worldPos.xyz = mul(localPos, (float4x3) g_worldTransform);													\n\
		viewPos.xyz = mul(worldPos, (float4x3) g_viewTransform);													\n\
		output.position = mul(viewPos, (float4x4) g_projectionTransform);											\n\
																													\n\
		// Output full fog																							\n\
		output.fog = 1.0f;																							\n\
																													\n\
		// Accumulate global diffuse and self illumination															\n\
		output.diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);															\n\
		output.specular = float4(1.0f, 1.0f, 1.0f, 1.0f);															\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *vsSource = "\
																													\n\
	// Globals																										\n\
	float4x3			g_worldTransform		: register(c0);														\n\
	float4x3			g_viewTransform			: register(c3);														\n\
	float4x4			g_projectionTransform	: register(c6);														\n\
																													\n\
	float4				g_ambientColor			: register(c10);													\n\
	float4				g_shCoefficients[9]		: register(c11);		// Spherical harmonics coefficients			\n\
	float4				g_specularDirection		: register(c21);		// Object space specular position			\n\
	float4				g_specularColor			: register(c22);													\n\
	float4				g_selfIllumination		: register(c23);		// Self-illumination term					\n\
	float4				g_globalSettings		: register(c25);		// (Fog Constant, Fog Start, Fog End, Tween Factor)	\n\
	float4				g_globalDiffuse			: register(c26);													\n\
	float4				g_spc_base_inc_x		: register(c27);		// (spacing, y base, y increment, undefined)\n\
	float4				g_alphaUVModifier		: register(c28);													\n\
	float4				g_uvStretchModifiers[6]	: register(c29);													\n\
																													\n\
	struct VS_INPUT																									\n\
	{																												\n\
		float4			x_z_yH_yL				: POSITION;															\n\
		float4			diffuse					: COLOR0;				// Packed as (0xAAXXYYZZ)					\n\
	};																												\n\
																													\n\
	struct VS_OUTPUT																								\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR0;															\n\
		float4			specular				: COLOR1;															\n\
		float2			uv_splat				: TEXCOORD0;														\n\
		float2			uv_layer0				: TEXCOORD1;														\n\
		float2			uv_layer1				: TEXCOORD2;														\n\
		float2			uv_layer2				: TEXCOORD3;														\n\
		float2			uv_layer3				: TEXCOORD4;														\n\
		float2			uv_layer4				: TEXCOORD5;														\n\
		float2			uv_layer5				: TEXCOORD6;														\n\
		float1			fog						: TEXCOORD7;														\n\
	};																												\n\
																													\n\
	VS_OUTPUT main(const VS_INPUT v)																				\n\
	{																												\n\
		VS_OUTPUT output;																							\n\
																													\n\
		// Initialize world-view vectors to 1																		\n\
		float4 localPos = float4(1, 1, 1, 1);																		\n\
		float4 worldPos = float4(1, 1, 1, 1);																		\n\
		float4 viewPos = float4(1, 1, 1, 1);																		\n\
																													\n\
		// Unpack world position																					\n\
		localPos.x = v.x_z_yH_yL.x * g_spc_base_inc_x.x;															\n\
		localPos.z = v.x_z_yH_yL.y * g_spc_base_inc_x.x;															\n\
		localPos.y = (v.x_z_yH_yL.z * 255 + v.x_z_yH_yL.w) * g_spc_base_inc_x.z + g_spc_base_inc_x.y;				\n\
																													\n\
		// Transform position to projected space																	\n\
		worldPos.xyz = mul(localPos, (float4x3) g_worldTransform);													\n\
		viewPos.xyz = mul(worldPos, (float4x3) g_viewTransform);													\n\
		output.position = mul(viewPos, (float4x4) g_projectionTransform);											\n\
																													\n\
		// Calculate fog																							\n\
		float1 rcpFogRange = 1.0f / (g_globalSettings.z - g_globalSettings.y);										\n\
		float1 relativeZ = max(viewPos.z - g_globalSettings.y, 0.0f);												\n\
		output.fog = clamp(min(1.0f - (relativeZ * rcpFogRange), 1.0f - g_globalSettings.x), 0.0f, 1.0f);			\n\
																													\n\
		// Unpack normal																							\n\
		float3 normal = (v.diffuse.xyz - 0.5f) * 2.0f;																\n\
																													\n\
		// Accumulate ambient color																					\n\
		output.diffuse = g_ambientColor;																			\n\
																													\n\
		// Accumulate SH color																						\n\
		output.diffuse += g_shCoefficients[0];																		\n\
		output.diffuse += g_shCoefficients[1] * (float4) normal.x;													\n\
		output.diffuse += g_shCoefficients[2] * (float4) normal.y;													\n\
		output.diffuse += g_shCoefficients[3] * (float4) normal.z;													\n\
		output.diffuse += g_shCoefficients[4] * (float4) (normal.x * normal.z);										\n\
		output.diffuse += g_shCoefficients[5] * (float4) (normal.z * normal.y);										\n\
		output.diffuse += g_shCoefficients[6] * (float4) (normal.y * normal.x);										\n\
		output.diffuse += g_shCoefficients[7] * (float4) (3 * normal.z * normal.z - 1);								\n\
		output.diffuse += g_shCoefficients[8] * (float4) (normal.x * normal.x - normal.y * normal.y);				\n\
																													\n\
		// Lerp between real lighting color and self-illumination color based on self-illumination strength			\n\
		output.diffuse.xyz = lerp(output.diffuse.xyz, g_selfIllumination.xyz, g_selfIllumination.w);				\n\
																													\n\
		// Set lighting alpha to 1																					\n\
		output.diffuse.w = 1;																						\n\
																													\n\
		// Modulate final diffuse color by the global diffuse modifier												\n\
		output.diffuse *= g_globalDiffuse;																			\n\
																													\n\
		// Transform normal to view space																			\n\
		float3 N = normalize(mul(mul(normal, g_worldTransform), g_viewTransform));									\n\
																													\n\
		// Specular																									\n\
		float3 V = normalize(-viewPos.xyz);																			\n\
		float3 L = g_specularDirection;																				\n\
																													\n\
		L = normalize(mul(mul(L, (float3x3) g_worldTransform), (float3x3) g_viewTransform));						\n\
																													\n\
		float3 H = normalize(V + L);																				\n\
		float1 NL = dot(N, L);																						\n\
																													\n\
		if(NL > 0.0f)																								\n\
			NL = 1.0f;																								\n\
		else																										\n\
			NL = 0.0f;																								\n\
																													\n\
		float1 HN = clamp(dot(H, N), 0.0f, 1.0f);																	\n\
		float1 specularPower = pow(HN, 32) * NL * 3.0f;																\n\
																													\n\
		output.specular.xyz = g_specularColor * specularPower.xxxx;													\n\
		output.specular.w = 1.0f;																					\n\
																													\n\
		float2 normPos_XZ = localPos.xz * g_alphaUVModifier.zz;														\n\
																													\n\
		// Calculate splat map UV																					\n\
		output.uv_splat = normPos_XZ;																				\n\
																													\n\
		// Copy texture coordinates																					\n\
		output.uv_layer0 = normPos_XZ * g_uvStretchModifiers[0].xy;													\n\
		output.uv_layer1 = normPos_XZ * g_uvStretchModifiers[1].xy;													\n\
		output.uv_layer2 = normPos_XZ * g_uvStretchModifiers[2].xy;													\n\
		output.uv_layer3 = normPos_XZ * g_uvStretchModifiers[3].xy;													\n\
		output.uv_layer4 = normPos_XZ * g_uvStretchModifiers[4].xy;													\n\
		output.uv_layer5 = normPos_XZ * g_uvStretchModifiers[5].xy;													\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource_SM2_D1 = "\
																													\n\
	float4				g_constants				: register(c0);														\n\
	float4				g_fogColor				: register(c1);														\n\
																													\n\
	sampler2D			texLightMap				: register(s0);														\n\
	sampler2D			texAlpha0				: register(s1);														\n\
	sampler2D			texLayer0				: register(s2);														\n\
	sampler2D			texLayer1				: register(s3);														\n\
	sampler2D			texLayer2				: register(s4);														\n\
	sampler2D			texAlpha1				: register(s5);														\n\
	sampler2D			texLayer3				: register(s6);														\n\
	sampler2D			texLayer4				: register(s7);														\n\
	sampler2D			texLayer5				: register(s8);														\n\
																													\n\
	struct PS_INPUT																									\n\
	{																												\n\
		float4			diffuse					: COLOR0;															\n\
		float4			specular				: COLOR1;															\n\
		float2			uv_splat				: TEXCOORD0;														\n\
		float2			uv_layer0				: TEXCOORD1;														\n\
		float2			uv_layer1				: TEXCOORD2;														\n\
		float2			uv_layer2				: TEXCOORD3;														\n\
		float2			uv_layer3				: TEXCOORD4;														\n\
		float2			uv_layer4				: TEXCOORD5;														\n\
		float2			uv_layer5				: TEXCOORD6;														\n\
		float1			fog						: TEXCOORD7;														\n\
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
		// Sample layer weight textures																				\n\
		float4 weight0 = tex2D(texAlpha0, v.uv_splat);																\n\
		float4 weight1 = tex2D(texAlpha1, v.uv_splat);																\n\
																													\n\
		// Sample layer textures																					\n\
		float4 layer0 = tex2D(texLayer0, v.uv_layer0);																\n\
		float4 layer1 = tex2D(texLayer1, v.uv_layer1);																\n\
		float4 layer2 = tex2D(texLayer2, v.uv_layer2);																\n\
		float4 layer3 = tex2D(texLayer3, v.uv_layer3);																\n\
		float4 layer4 = tex2D(texLayer4, v.uv_layer4);																\n\
		float4 layer5 = tex2D(texLayer5, v.uv_layer5);																\n\
																													\n\
		// Calculate sum of all layer weights																		\n\
		float1 alphaSum = dot(weight0.xyz, g_constants.xxx) + dot(weight1.xyz, g_constants.xxx);					\n\
																													\n\
		// Scale weights such that the total adds up to one															\n\
		weight0 /= alphaSum.xxxx;																					\n\
		weight1 /= alphaSum.xxxx;																					\n\
																													\n\
		// Accumulate albedo																						\n\
		float4 albedo = layer0 * weight0.xxxx;																		\n\
		albedo += layer1 * weight0.yyyy;																			\n\
		albedo += layer2 * weight0.zzzz;																			\n\
		albedo += layer3 * weight1.xxxx;																			\n\
		albedo += layer4 * weight1.yyyy;																			\n\
		albedo += layer5 * weight1.zzzz;																			\n\
																													\n\
		// Calculate output color																					\n\
		output.color = albedo * v.diffuse;																			\n\
																													\n\
		// Lerp between output color and fog																		\n\
		output.color = lerp(g_fogColor.xyzw, output.color, v.fog.xxxx);												\n\
																													\n\
		// Set output alpha to one																					\n\
		output.color.w = 1.0f;																						\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource_SM2_D2 = "\
																													\n\
	float4				g_constants				: register(c0);														\n\
	float4				g_fogColor				: register(c1);														\n\
																													\n\
	sampler2D			texLightMap				: register(s0);														\n\
	sampler2D			texAlpha0				: register(s1);														\n\
	sampler2D			texLayer0				: register(s2);														\n\
	sampler2D			texLayer1				: register(s3);														\n\
	sampler2D			texLayer2				: register(s4);														\n\
	sampler2D			texAlpha1				: register(s5);														\n\
	sampler2D			texLayer3				: register(s6);														\n\
	sampler2D			texLayer4				: register(s7);														\n\
	sampler2D			texLayer5				: register(s8);														\n\
																													\n\
	struct PS_INPUT																									\n\
	{																												\n\
		float4			diffuse					: COLOR0;															\n\
		float4			specular				: COLOR1;															\n\
		float2			uv_splat				: TEXCOORD0;														\n\
		float2			uv_layer0				: TEXCOORD1;														\n\
		float2			uv_layer1				: TEXCOORD2;														\n\
		float2			uv_layer2				: TEXCOORD3;														\n\
		float2			uv_layer3				: TEXCOORD4;														\n\
		float2			uv_layer4				: TEXCOORD5;														\n\
		float2			uv_layer5				: TEXCOORD6;														\n\
		float1			fog						: TEXCOORD7;														\n\
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
		// Sample layer weight textures																				\n\
		float4 weight0 = tex2D(texAlpha0, v.uv_splat);																\n\
		float4 weight1 = tex2D(texAlpha1, v.uv_splat);																\n\
																													\n\
		// Sample layer textures																					\n\
		float4 layer0 = tex2D(texLayer0, v.uv_layer0);																\n\
		float4 layer1 = tex2D(texLayer1, v.uv_layer1);																\n\
		float4 layer2 = tex2D(texLayer2, v.uv_layer2);																\n\
		float4 layer3 = tex2D(texLayer3, v.uv_layer3);																\n\
		float4 layer4 = tex2D(texLayer4, v.uv_layer4);																\n\
		float4 layer5 = tex2D(texLayer5, v.uv_layer5);																\n\
																													\n\
		// Sample light map texture																					\n\
		float2 lightMapUV = float2(v.uv_splat.x, 1.0f - v.uv_splat.y);												\n\
		float4 lightMap = tex2D(texLightMap, lightMapUV);															\n\
																													\n\
		// Calculate sum of all layer weights																		\n\
		float1 alphaSum = dot(weight0.xyz, g_constants.xxx) + dot(weight1.xyz, g_constants.xxx);					\n\
																													\n\
		// Scale weights such that the total adds up to one															\n\
		weight0 /= alphaSum.xxxx;																					\n\
		weight1 /= alphaSum.xxxx;																					\n\
																													\n\
		// Accumulate albedo																						\n\
		float4 albedo = layer0 * weight0.xxxx;																		\n\
		albedo += layer1 * weight0.yyyy;																			\n\
		albedo += layer2 * weight0.zzzz;																			\n\
		albedo += layer3 * weight1.xxxx;																			\n\
		albedo += layer4 * weight1.yyyy;																			\n\
		albedo += layer5 * weight1.zzzz;																			\n\
																													\n\
		// Calculate output color																					\n\
		output.color = (albedo * v.diffuse + v.specular * albedo.wwww) * clamp(lightMap + (float4) 0.75f, 0.0f, 1.0f);	\n\
																													\n\
		// Lerp between output color and fog																		\n\
		output.color = lerp(g_fogColor.xyzw, output.color, v.fog.xxxx);												\n\
																													\n\
		// Set output alpha to one																					\n\
		output.color.w = 1.0f;																						\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile vertex shader
	m_vertexShaders[0] = RuD3DCompileVertexShader(vsSource_Simple, "main", "vs_2_0", NULL);
	m_vertexShaders[1] = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Compile pixel shader
	m_pixelShader_SM2_D1 = RuD3DCompilePixelShader(psSource_SM2_D1, "main", "ps_2_0", NULL);

	// Compile pixel shader
	m_pixelShader_SM2_D2 = RuD3DCompilePixelShader(psSource_SM2_D2, "main", "ps_2_0", NULL);
}

CRuShader_SM20EZ_Terrain_Packed::~CRuShader_SM20EZ_Terrain_Packed()
{
	for(INT32 i = 0; i < 2; ++i)
	{
		if(m_vertexShaders[i])
		{
			m_vertexShaders[i]->Release();
		}
	}

	ruSAFE_RELEASE(m_pixelShader_SM2_D1);
	ruSAFE_RELEASE(m_pixelShader_SM2_D2);
}

void CRuShader_SM20EZ_Terrain_Packed::BeginPass(INT32 passNumber)
{
	// Override raw data offset
	m_rawData_Offset = 29;

	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Terrain_Packed_Base::BeginPass(passNumber);

	m_vertexDecl->Select();

	if(m_numTextureLayers == 0)
	{
		m_engine->SetVertexShader(m_vertexShaders[0]);
	}
	else
	{
		m_engine->SetVertexShader(m_vertexShaders[1]);
	}

	if(m_maxLayersPerPass == 6)
	{
		RuPropertyType propertyValue;
		INT32 shaderLevel = g_ruEngineSettings->GetSetting("terrain:splat technique", propertyValue) ? boost::get<INT32>(propertyValue) : 2;

		// If the number of textures remaining is greater than the maximum number of layers per pass, then ...
		if(shaderLevel < 2)
		{
			m_engine->SetPixelShader(m_pixelShader_SM2_D1);
		}
		else
		{
			m_engine->SetPixelShader(m_pixelShader_SM2_D2);
		}
	}

	// Set shader constants
	m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants);
	m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

	for(INT32 i = 2; i <= 4; ++i)
	{
		m_engine->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->UseDefaultSamplerFilterState(i);
	}

	for(INT32 i = 6; i <= 8; ++i)
	{
		m_engine->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->UseDefaultSamplerFilterState(i);
	}

	m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_engine->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_engine->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	m_engine->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_engine->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_engine->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_engine->SetSamplerState(5, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_engine->SetSamplerState(5, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	m_engine->SetSamplerState(5, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_engine->SetSamplerState(5, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_engine->SetSamplerState(5, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_engine->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
}

void CRuShader_SM20EZ_Terrain_Packed::EndPass(INT32 passNumber)
{
	CRuShader_SM20EZ_Terrain_Packed_Base::EndPass(passNumber);
}

void CRuShader_SM20EZ_Terrain_Packed::SetParameters(const CRuParameterCache *paramCache)
{
	UINT64 param;

	CRuShader_SM20EZ_Terrain_Packed_Base::SetParameters(paramCache);

	if(paramCache->GetParameter(ruPARAMTYPE_SPLAT_ALPHAUVMODIFIER, param))
	{
		if(param)
		{
			INT32 uvModifierConstantsOffset = 28 * 4;
			float *uvModifier = (float *) param;

			m_shaderConstants[uvModifierConstantsOffset++] = uvModifier[0];
			m_shaderConstants[uvModifierConstantsOffset++] = uvModifier[1];
			m_shaderConstants[uvModifierConstantsOffset++] = uvModifier[2];
			m_shaderConstants[uvModifierConstantsOffset++] = uvModifier[3];
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SPLAT_LAYERCOUNT, param))
	{
		// Set the number of texture layers to blend
		m_numTextureLayers = (INT32)param;

		if(m_numTextureLayers == 0)
		{
			m_numPasses = 1;
		}
		else
		{
			// If the number of texture layers requested exceeds the maximum allowed, cap it
			if(m_numTextureLayers > m_maxTextureLayers)
			{
				m_numTextureLayers = m_maxTextureLayers;
			}

			// Calculate the number of passes
			m_numPasses = (m_numTextureLayers / m_maxLayersPerPass) + ((m_numTextureLayers % m_maxLayersPerPass) ? 1 : 0);

			// Pixel Shader 1.1 has one extra fog pass
			if(m_maxLayersPerPass == 3)
			{
				m_numPasses += 1;
			}
		}
	}
}

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Terrain_Packed_Backdrop::CRuShader_SM20EZ_Terrain_Packed_Backdrop(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Terrain_Packed_Base(engine)
{
	m_override_Fog = FALSE;
	m_override_AlphaTest = TRUE;
	m_override_AlphaBlend = TRUE;

	char *vsSource = "\
																													\n\
	// Globals																										\n\
	float4x3			g_worldTransform		: register(c0);														\n\
	float4x3			g_viewTransform			: register(c3);														\n\
	float4x4			g_projectionTransform	: register(c6);														\n\
																													\n\
	float4				g_ambientColor			: register(c10);													\n\
	float4				g_shCoefficients[9]		: register(c11);		// Spherical harmonics coefficients			\n\
	float4				g_specularDirection		: register(c21);		// Object space specular position			\n\
	float4				g_specularColor			: register(c22);													\n\
	float4				g_selfIllumination		: register(c23);		// Self-illumination term					\n\
	float4				g_globalSettings		: register(c25);		// (?, Fog Start, Fog End, Tween Factor)	\n\
	float4				g_globalDiffuse			: register(c26);													\n\
	float4				g_spc_base_inc_x		: register(c27);		// (spacing, y base, y increment, undefined)\n\
	float4				g_alphaUVModifier		: register(c28);													\n\
	float4				g_uvStretchModifier		: register(c29);													\n\
																													\n\
	struct VS_INPUT																									\n\
	{																												\n\
		float4			x_z_yH_yL				: POSITION;															\n\
		float4			diffuse					: COLOR0;				// Packed as (0xAAXXYYZZ)					\n\
	};																												\n\
																													\n\
	struct VS_OUTPUT																								\n\
	{																												\n\
		float4			position				: POSITION;															\n\
	};																												\n\
																													\n\
	VS_OUTPUT main(const VS_INPUT v)																				\n\
	{																												\n\
		VS_OUTPUT output;																							\n\
																													\n\
		// Initialize world-view vectors to 1																		\n\
		float4 localPos = float4(1, 1, 1, 1);																		\n\
		float4 worldPos = float4(1, 1, 1, 1);																		\n\
		float4 viewPos = float4(1, 1, 1, 1);																		\n\
																													\n\
		// Unpack world position																					\n\
		localPos.x = v.x_z_yH_yL.x * g_spc_base_inc_x.x;															\n\
		localPos.z = v.x_z_yH_yL.y * g_spc_base_inc_x.x;															\n\
		localPos.y = (v.x_z_yH_yL.z * 255 + v.x_z_yH_yL.w) * g_spc_base_inc_x.z + g_spc_base_inc_x.y;				\n\
																													\n\
		// Transform position to projected space																	\n\
		worldPos.xyz = mul(localPos, (float4x3) g_worldTransform);													\n\
		viewPos.xyz = mul(worldPos, (float4x3) g_viewTransform);													\n\
		output.position = mul(viewPos, (float4x4) g_projectionTransform);											\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource = "\
																													\n\
	float4				g_constants				: register(c0);														\n\
	float4				g_fogColor				: register(c1);														\n\
																													\n\
	struct PS_INPUT																									\n\
	{																												\n\
		float4			diffuse					: COLOR0;															\n\
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
		// Directly output fog color																				\n\
		output.color = g_fogColor;																					\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Assemble vertex shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	m_pixelShader = RuD3DCompilePixelShader(psSource, "main", "ps_2_0", NULL);
}

CRuShader_SM20EZ_Terrain_Packed_Backdrop::~CRuShader_SM20EZ_Terrain_Packed_Backdrop()
{
}

void CRuShader_SM20EZ_Terrain_Packed_Backdrop::BeginPass(INT32 passNumber)
{
	// Force 2-sided rendering
	m_2Sided = TRUE;

	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Terrain_Packed_Base::BeginPass(passNumber);

	m_vertexDecl->Select();
	m_engine->SetVertexShader(m_vertexShader);
	m_engine->SetPixelShader(m_pixelShader);

	// Set shader constants
	m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants);
	m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

	m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
}

void CRuShader_SM20EZ_Terrain_Packed_Backdrop::EndPass(INT32 passNumber)
{
	CRuShader_SM20EZ_Terrain_Packed_Base::EndPass(passNumber);
}

void CRuShader_SM20EZ_Terrain_Packed_Backdrop::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SM20EZ_Terrain_Packed_Base::SetParameters(paramCache);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
