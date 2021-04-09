#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Doodad.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Particle_Doodad::CRuShader_SM20EZ_Particle_Doodad(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Base(engine)
{
	// Overrides
	m_override_Fog = TRUE;
	m_override_AlphaTest = TRUE;
	m_override_AlphaBlend = TRUE;
	m_override_ZWrite = TRUE;

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
	float4				g_uvParams				: register(c27);													\n\
	float4				g_instanceData[210]		: register(c33);		// Instance data							\n\
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
		float2			texcoord0				: TEXCOORD0;														\n\
		float2			fogParam				: TEXCOORD1;														\n\
		float1			fog						: FOG;																\n\
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
		// Calculate instance data indices																			\n\
		float1 transScaleOffset = (v.diffuse.w * 256.0f) * 3.0f;													\n\
		float1 rotQuatOffset = transScaleOffset + 1.0f;																\n\
		float1 paramOffset = rotQuatOffset + 1.0f;																	\n\
																													\n\
		// Load translation-scale																					\n\
		float3 translation = g_instanceData[(int) transScaleOffset].xyz;											\n\
		float1 scale = g_instanceData[(int) transScaleOffset].w;													\n\
																													\n\
		// Load rotation quaternion																					\n\
		float3 qV = g_instanceData[(int) rotQuatOffset].xyz;														\n\
		float1 qW = g_instanceData[(int) rotQuatOffset].w;															\n\
																													\n\
		// Scale																									\n\
		float3 P = v.position.xyz * scale.xxx;																		\n\
																													\n\
		// Rotate																									\n\
		P = (2.0f * qW * qW - 1.0f).xxx * P + (float3) 2.0f * dot(qV, P) * qV + cross(P, qV) * qW.xxx;				\n\
																													\n\
		// Translate																								\n\
		worldPos.xyz = P + translation;																				\n\
																													\n\
		// Transform position to projected space																	\n\
		viewPos.xyz = mul(worldPos, (float4x3) g_viewTransform);													\n\
		output.position = mul(viewPos, (float4x4) g_projectionTransform);											\n\
																													\n\
		// Calculate fog																							\n\
		float1 rcpFogRange = 1.0f / (g_globalSettings.z - g_globalSettings.y);										\n\
		float1 relativeZ = max(viewPos.z - g_globalSettings.y, 0.0f);												\n\
		output.fog = clamp(min(1.0f - (relativeZ * rcpFogRange), 1.0f - g_globalSettings.x), 0.0f, 1.0f);			\n\
		output.fogParam = float4(1, 1, 1, 1);																		\n\
		output.fogParam.x = output.fog;																				\n\
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
		// Modulate final diffuse color by the global diffuse modifier												\n\
		output.diffuse *= g_globalDiffuse;																			\n\
																													\n\
		// Transform uv coordinates by uv transform																	\n\
		output.texcoord0.xy = g_instanceData[(int) paramOffset].xy * g_uvParams.xy + v.texcoord.xy * g_uvParams.xy; \n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource = "\
																													\n\
	float4				g_fogColor				: register(c1);														\n\
																													\n\
	sampler2D			diffuseTex				: register(s0);														\n\
																													\n\
	struct PS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR0;															\n\
		float2			texcoord0				: TEXCOORD0;														\n\
		float4			fog						: TEXCOORD1;														\n\
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
		// Sample diffuse map																						\n\
		float4 diffuse = tex2D(diffuseTex, v.texcoord0);															\n\
																													\n\
		// Multiply by vertex color																					\n\
		diffuse = diffuse * v.diffuse;																				\n\
																													\n\
		// Multiply fog by alpha																					\n\
		float4 fogColor = g_fogColor * diffuse.w;																	\n\
																													\n\
		// Interpolate between diffuse color and for color															\n\
		output.color = lerp(fogColor, diffuse, v.fog.xxxx);															\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile vertex shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Compile pixel shader
	m_pixelShader = RuD3DCompilePixelShader(psSource, "main", "ps_2_0", NULL);
}

CRuShader_SM20EZ_Particle_Doodad::~CRuShader_SM20EZ_Particle_Doodad()
{
}

void CRuShader_SM20EZ_Particle_Doodad::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);

		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numConstantRows);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		m_engine->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		m_engine->SetRenderState(D3DRS_ALPHAREF, 50);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		m_engine->UseDefaultSamplerFilterState(0);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}
	else
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);

		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numConstantRows);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		m_engine->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESSEQUAL);
		m_engine->SetRenderState(D3DRS_ALPHAREF, 150);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		m_engine->UseDefaultSamplerFilterState(0);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}
}

void CRuShader_SM20EZ_Particle_Doodad::EndPass(INT32 passNumber)
{
	CRuShader_SM20EZ_Base::EndPass(passNumber);
}

void CRuShader_SM20EZ_Particle_Doodad::SetParameters(const CRuParameterCache *paramCache)
{
	UINT64 param;

	CRuShader_SM20EZ_Base::SetParameters(paramCache);

	if(paramCache->GetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, param))
	{
		m_numInstances = (UINT32) param;
		CRuMatrix4x4 *transform = (CRuMatrix4x4 *) param;

	}

	if(paramCache->GetParameter(ruPARAMTYPE_GEOINSTANCE_S_DATA, param))
	{
		float *instanceData = reinterpret_cast<float *>(param);

		// Number of constants = instance dataset + 24 constants for the per-draw data
		INT32 numConstants = 24 + m_numInstances * 12;
		INT32 constantOffset = 27 * 4;

		m_numConstantRows = (constantOffset + numConstants) / 4;

		RuMemcpy(&m_shaderConstants[constantOffset], instanceData, sizeof(float) * numConstants);
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
