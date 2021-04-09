#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Standard.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Static::CRuShader_SM20EZ_Static(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Static_Base(engine)
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
	float4x4			g_uvTransform0			: register(c27);													\n\
																													\n\
	struct VS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float3			normal_packed			: NORMAL;															\n\
		float4			diffuse					: COLOR0;															\n\
		float2			uv0						: TEXCOORD0;														\n\
		float2			uv1						: TEXCOORD1;														\n\
	};																												\n\
																													\n\
	struct VS_OUTPUT																								\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR0;															\n\
		float4			specular				: COLOR1;															\n\
		float2			uv0						: TEXCOORD0;														\n\
		float2			uv1						: TEXCOORD1;														\n\
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
		// Unpack normal																							\n\
		float3 normal = (v.normal_packed.xyz - 127.0f) / 127.0f;													\n\
																													\n\
		// Transform position to projected space																	\n\
		worldPos.xyz = v.position.xyz + normal.xyz * g_globalSettings.w;											\n\
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
		// Modulate lighting color with vertex color																\n\
		output.diffuse *= v.diffuse;																				\n\
																													\n\
		// Modulate final diffuse color by the global diffuse modifier												\n\
		output.diffuse *= g_globalDiffuse;																			\n\
																													\n\
		// Transform normal to view space																			\n\
		float3 viewNormal = float4(1, 1, 1, 1);																		\n\
																													\n\
		viewNormal.xyz = mul(mul(normal, (float3x3) g_worldTransform), (float3x3) g_viewTransform);					\n\
		viewNormal.xyz = normalize(viewNormal.xyz);																	\n\
																													\n\
		// Specular																									\n\
		float3 V = normalize(-viewPos.xyz);																			\n\
		float3 L = g_specularDirection;																				\n\
																													\n\
		L = normalize(mul(mul(L, (float3x3) g_worldTransform), (float3x3) g_viewTransform));						\n\
																													\n\
		float3 H = normalize(V + L);																				\n\
		float1 NL = dot(viewNormal, L);																				\n\
																													\n\
		if(NL > 0.0f)																								\n\
			NL = 1.0f;																								\n\
		else																										\n\
			NL = 0.0f;																								\n\
																													\n\
		float1 HN = clamp(dot(H, viewNormal), 0.0f, 1.0f);															\n\
		float1 specularPower = pow(HN, 16) * NL * 3.0f;																\n\
																													\n\
		output.specular.xyz = g_specularColor * specularPower.xxxx;													\n\
		output.specular.w = 1.0f;																					\n\
																													\n\
		// Transform uv coordinates by uv transform																	\n\
		float4 texCoordTemp = float4(v.uv0.x, v.uv0.y, 0, 1);														\n\
		texCoordTemp = mul(texCoordTemp, (float4x4) g_uvTransform0);												\n\
		output.uv0.x = texCoordTemp.x;																				\n\
		output.uv0.y = texCoordTemp.y;																				\n\
																													\n\
		output.uv1 = v.uv1;																							\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource = "\
																													\n\
	float4				g_fogColor				: register(c1);														\n\
																													\n\
	sampler2D			texDiffuse				: register(s0);														\n\
	sampler2D			texGloss				: register(s1);														\n\
	sampler2D			texGlow					: register(s2);														\n\
	sampler2D			texLightMap				: register(s3);														\n\
																													\n\
	struct PS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR0;															\n\
		float4			specular				: COLOR1;															\n\
		float2			uv0						: TEXCOORD0;														\n\
		float2			uv1						: TEXCOORD1;														\n\
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
		// Sample diffuse, gloss, and light map																		\n\
		float4 diffuse = tex2D(texDiffuse, v.uv0);																	\n\
		float4 gloss = tex2D(texGloss, v.uv0);																		\n\
		float4 glow = tex2D(texGlow, v.uv0);																		\n\
		float4 lightMap = tex2D(texLightMap, v.uv1);																\n\
																													\n\
		// Multiply albedo by vertex color																			\n\
		diffuse.xyzw = diffuse.xyzw * v.diffuse.xyzw;																\n\
																													\n\
		// Add specular component and multiply by light map															\n\
		diffuse.xyz = (diffuse.xyz + gloss.xyz * v.specular.xyz) * lightMap.xyz;									\n\
																													\n\
		// Add glow color																							\n\
		diffuse.xyz = diffuse.xyz + glow.xyz;																		\n\
																													\n\
		// Pre-multiply diffuse by alpha																			\n\
		diffuse.xyz = diffuse.xyz * diffuse.w;																		\n\
																													\n\
		// Multiply fog by alpha																					\n\
		float4 fogColor = g_fogColor * diffuse.w;																	\n\
																													\n\
		// Interpolate between diffuse color and fog color															\n\
		output.color = lerp(fogColor.xyzw, diffuse.xyzw, v.fog.xxxx);												\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile vertex shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Compile pixel shader
	m_pixelShader = RuD3DCompilePixelShader(psSource, "main", "ps_2_0", NULL);
}

CRuShader_SM20EZ_Static::~CRuShader_SM20EZ_Static()
{
}

void CRuShader_SM20EZ_Static::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Static_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		m_engine->UseDefaultSamplerFilterState(0);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->UseDefaultSamplerFilterState(1);
		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->UseDefaultSamplerFilterState(2);
		m_engine->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->UseDefaultSamplerFilterState(3);
		m_engine->SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

void CRuShader_SM20EZ_Static::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SM20EZ_Static_Base::SetParameters(paramCache);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
