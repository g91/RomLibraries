#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Water.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Water_Base::CRuShader_SM20EZ_Water_Base(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Base(engine)
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

	m_alphaTestEnabled = FALSE;

	m_override_AlphaTest = TRUE;
	m_override_AlphaBlend = TRUE;
}

CRuShader_SM20EZ_Water_Base::~CRuShader_SM20EZ_Water_Base()
{
}

void CRuShader_SM20EZ_Water_Base::SetParameters(const CRuParameterCache *paramCache)
{
	UINT64 param;

	CRuShader_SM20EZ_Base::SetParameters(paramCache);

	if(paramCache->GetParameter(ruPARAMTYPE_TRANSFORM_PROJECTION, param))
	{
		// Setup texture thingy
		CRuMatrix4x4 texTransform = *((CRuMatrix4x4 *) param);

		texTransform._11 *= 0.5f;
		texTransform._22 *= -0.5f;
		texTransform._31 += 0.5f;
		texTransform._32 += 0.51f;	// HACK, param)) offset a bit to hide the water line

		INT32 uvTransformOffset = 27 * 4;

		for(UINT32 i = 0, k = 0; i < 4; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k + uvTransformOffset] = texTransform.m_elements[j][i];
			}
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_WATER_BUMPFACTOR, param))
	{
		INT32 factorOffset = 3 * 4;
		float factor = RuCastINT64ToFloat(param);

		m_pixelShaderConstants[factorOffset] = factor;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_WATER_WAVECOLOR, param))
	{
		INT32 waveColorOffset = 4 * 4;

		CRuColor4 waveColor(static_cast<RUCOLOR>(param));

		m_pixelShaderConstants[waveColorOffset++] = waveColor.m_r;
		m_pixelShaderConstants[waveColorOffset++] = waveColor.m_g;
		m_pixelShaderConstants[waveColorOffset++] = waveColor.m_b;
		m_pixelShaderConstants[waveColorOffset++] = waveColor.m_a;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_CAMERA, param))
	{
		INT32 camPosOffset = 32 * 4;
		CRuCamera *camera = (CRuCamera *) param;

		m_shaderConstants[camPosOffset++] = camera->GetCameraPosition().m_x;
		m_shaderConstants[camPosOffset++] = camera->GetCameraPosition().m_y;
		m_shaderConstants[camPosOffset++] = camera->GetCameraPosition().m_z;
		m_shaderConstants[camPosOffset++] = 0.0f;
	}

	// Bump T
	if(paramCache->GetParameter(ruPARAMTYPE_CUSTOM_0, param))
	{
		INT32 bumpTOffset = 27 * 4;

		m_shaderConstants[bumpTOffset] = RuCastINT64ToFloat(param);
	}

	// Bump paremters
	if(paramCache->GetParameter(ruPARAMTYPE_CUSTOM_1, param))
	{
		INT32 bumpWeightsOffset = 28 * 4;
		REAL *bumpParams = reinterpret_cast<REAL *>(param);

		m_shaderConstants[bumpWeightsOffset++] = bumpParams[0];
		m_shaderConstants[bumpWeightsOffset++] = bumpParams[1];
		m_shaderConstants[bumpWeightsOffset++] = bumpParams[2];
		m_shaderConstants[bumpWeightsOffset++] = bumpParams[3];
	}

	if(paramCache->GetParameter(ruPARAMTYPE_CUSTOM_2, param))
	{
		INT32 bumpWeightsOffset = 29 * 4;
		REAL *bumpParams = reinterpret_cast<REAL *>(param);

		m_shaderConstants[bumpWeightsOffset++] = bumpParams[0];
		m_shaderConstants[bumpWeightsOffset++] = bumpParams[1];
		m_shaderConstants[bumpWeightsOffset++] = bumpParams[2];
		m_shaderConstants[bumpWeightsOffset++] = bumpParams[3];
	}

	if(paramCache->GetParameter(ruPARAMTYPE_CUSTOM_3, param))
	{
		INT32 bumpWeightsOffset = 30 * 4;
		REAL *bumpParams = reinterpret_cast<REAL *>(param);

		m_shaderConstants[bumpWeightsOffset++] = bumpParams[0];
		m_shaderConstants[bumpWeightsOffset++] = bumpParams[1];
		m_shaderConstants[bumpWeightsOffset++] = bumpParams[2];
		m_shaderConstants[bumpWeightsOffset++] = bumpParams[3];
	}

	if(paramCache->GetParameter(ruPARAMTYPE_CUSTOM_4, param))
	{
		INT32 bumpWeightsOffset = 31 * 4;
		REAL *bumpParams = reinterpret_cast<REAL *>(param);

		m_shaderConstants[bumpWeightsOffset++] = bumpParams[0];
		m_shaderConstants[bumpWeightsOffset++] = bumpParams[1];
		m_shaderConstants[bumpWeightsOffset++] = bumpParams[2];
		m_shaderConstants[bumpWeightsOffset++] = bumpParams[3];
	}

	// Bump weights
	if(paramCache->GetParameter(ruPARAMTYPE_CUSTOM_5, param))
	{
		INT32 bumpWeightsOffset = 2 * 4;

		m_pixelShaderConstants[bumpWeightsOffset++] = ((param >> 24) & 0xFF) / 255.0f;
		m_pixelShaderConstants[bumpWeightsOffset++] = ((param >> 16) & 0xFF) / 255.0f;
		m_pixelShaderConstants[bumpWeightsOffset++] = ((param >> 8) & 0xFF) / 255.0f;
		m_pixelShaderConstants[bumpWeightsOffset++] = ((param >> 0) & 0xFF) / 255.0f;
	}
}

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Water_Full::CRuShader_SM20EZ_Water_Full(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Water_Base(engine)
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
																													\n\
	float4				g_bumpT					: register(c27);													\n\
																													\n\
	float4				g_bumpOctaveParams0		: register(c28);													\n\
	float4				g_bumpOctaveParams1		: register(c29);													\n\
	float4				g_bumpOctaveParams2		: register(c30);													\n\
	float4				g_bumpOctaveParams3		: register(c31);													\n\
																													\n\
	float3				g_camPos				: register(c32);													\n\
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
		float4			projUV					: TEXCOORD0;			// Projected UV								\n\
		float2			bumpUV0					: TEXCOORD1;			// Bump octave 0 UV							\n\
		float2			bumpUV1					: TEXCOORD2;			// Bump octave 1 UV							\n\
		float2			bumpUV2					: TEXCOORD3;			// Bump octave 2 UV							\n\
		float2			bumpUV3					: TEXCOORD4;			// Bump octave 3 UV							\n\
		float3			camPos					: TEXCOORD5;			// Camera world position					\n\
		float3			ptPos					: TEXCOORD6;			// Point world position						\n\
		float1			fog						: TEXCOORD7;			// Fog										\n\
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
		output.projUV.x = (output.position.w + output.position.x) * 0.5f;											\n\
		output.projUV.y = (output.position.w - output.position.y) * 0.5f;											\n\
		output.projUV.z = 0.0f;																						\n\
		output.projUV.w = output.position.w;																		\n\
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
		float4 normal = float4(0.0f, 1.0f, 0.0f, 0.0f);																\n\
																													\n\
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
		// Copy texture coordinates																					\n\
		output.bumpUV0 = worldPos.xz * g_bumpOctaveParams0.z + g_bumpOctaveParams0.xy * g_bumpT.x;					\n\
		output.bumpUV1 = worldPos.xz * g_bumpOctaveParams1.z + g_bumpOctaveParams1.xy * g_bumpT.x;					\n\
		output.bumpUV2 = worldPos.xz * g_bumpOctaveParams2.z + g_bumpOctaveParams2.xy * g_bumpT.x;					\n\
		output.bumpUV3 = worldPos.xz * g_bumpOctaveParams3.z + g_bumpOctaveParams3.xy * g_bumpT.x;					\n\
																													\n\
		// Copy camera and vertex position																			\n\
		output.camPos = g_camPos;																					\n\
		output.ptPos = worldPos.xyz;																				\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource = "\
																													\n\
	sampler2D			reflectMap				: register(s0);														\n\
	sampler2D			refractMap				: register(s1);														\n\
	sampler2D			bumpMap					: register(s2);														\n\
																													\n\
	float4				g_commonConstants		: register(c0);			// Constants (1.0f, 0.5f, 0.0f, 0.05f)		\n\
	float4				g_fogColor				: register(c1);			// Fog color								\n\
																													\n\
	float4				g_bumpWeights			: register(c2);														\n\
	float4				g_factors				: register(c3);			// (Bump factor, 0, 0, 0)					\n\
	float4				g_waveColor				: register(c4);			// Wave color								\n\
																													\n\
	struct PS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR0;															\n\
		float4			projUV					: TEXCOORD0;			// Projected UV								\n\
		float2			bumpUV0					: TEXCOORD1;			// Bump octave 0 UV							\n\
		float2			bumpUV1					: TEXCOORD2;			// Bump octave 1 UV							\n\
		float2			bumpUV2					: TEXCOORD3;			// Bump octave 2 UV							\n\
		float2			bumpUV3					: TEXCOORD4;			// Bump octave 3 UV							\n\
		float3			camPos					: TEXCOORD5;			// Camera world position					\n\
		float3			ptPos					: TEXCOORD6;			// Point world position						\n\
		float1			fog						: TEXCOORD7;			// Fog										\n\
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
																													\n\
		// Sample bump maps																							\n\
		float3 n0 = tex2D(bumpMap, v.bumpUV0);																		\n\
		float3 n1 = tex2D(bumpMap, v.bumpUV1);																		\n\
		float3 n2 = tex2D(bumpMap, v.bumpUV2);																		\n\
		float3 n3 = tex2D(bumpMap, v.bumpUV3);																		\n\
																													\n\
		// Combine bump octaves according to their weights															\n\
		float3 n = (n0 * g_bumpWeights.x + n1 * g_bumpWeights.y + n2 * g_bumpWeights.z + n3 * g_bumpWeights.w) - 0.5f;		\n\
																													\n\
		// Normalize bump normal																					\n\
		n = normalize(n);																							\n\
																													\n\
		// Divide projected UV by W																					\n\
		float2 projUVdiv = v.projUV.xy / v.projUV.w;																\n\
																													\n\
		// Perturb projected UV by bump normal																		\n\
		projUVdiv = projUVdiv + n.xy * 0.025f;																		\n\
																													\n\
		// Sample reflection texture																				\n\
		float4 reflectSample = tex2D(reflectMap, projUVdiv);														\n\
																													\n\
		// Sample refraction texture																				\n\
		float4 refractSample = tex2D(refractMap, projUVdiv);														\n\
																													\n\
		// Exchange bump normal's y and z so it can be used as a normal vector										\n\
		float1 swapTemp = n.y;																						\n\
		n.y = n.z;																									\n\
		n.z = swapTemp;																								\n\
																													\n\
		// Weaken the bump normal vector by specified factor														\n\
		n = lerp(n, float3(0.0f, 1.0f, 0.0f), g_factors[0]);														\n\
																													\n\
		// Unpack cam to water vec																					\n\
		float3 camToWaterVecNrm = normalize(v.camPos - v.ptPos);													\n\
																													\n\
		// Dot cam-to-water vector with bump normal																	\n\
		float1 cosTheta = clamp(dot(camToWaterVecNrm, n), 0.0f, 1.0f);												\n\
		float4 fresnelTerm = (0.02037f + 0.97963f * pow(1 - cosTheta, 5));											\n\
																													\n\
		// Lerp between refraction sample and water diffuse color													\n\
		float4 refractColor = lerp(refractSample, v.diffuse, v.diffuse.w);											\n\
																													\n\
		// Lerp between reflection & refraction based on fresnel term												\n\
		output.color = lerp(refractColor, reflectSample, fresnelTerm);												\n\
																													\n\
		// Lerp between output color and fog																		\n\
		output.color = lerp(g_fogColor.xyzw, output.color, v.fog.xxxx);												\n\
																													\n\
		// Use diffuse alpha as output alpha																		\n\
		output.color.w = v.diffuse.w;																				\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile vertex shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Compile pixel shader
	m_pixelShader = RuD3DCompilePixelShader(psSource, "main", "ps_2_0", NULL);

	// Overrides
	m_override_ZWrite = TRUE;
}

CRuShader_SM20EZ_Water_Full::~CRuShader_SM20EZ_Water_Full()
{
}

void CRuShader_SM20EZ_Water_Full::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Water_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants + 5);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants + 3);

		m_engine->UseDefaultSamplerFilterState(0);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		m_engine->UseDefaultSamplerFilterState(1);
		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		m_engine->UseDefaultSamplerFilterState(2);
		m_engine->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->UseDefaultSamplerFilterState(3);
		m_engine->SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

void CRuShader_SM20EZ_Water_Full::EndPass(INT32 passNumber)
{
}

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Water_Minimal::CRuShader_SM20EZ_Water_Minimal(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Water_Base(engine)
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
																													\n\
	float4				g_bumpT					: register(c27);													\n\
																													\n\
	float4				g_bumpOctaveParams0		: register(c28);													\n\
	float4				g_bumpOctaveParams1		: register(c29);													\n\
	float4				g_bumpOctaveParams2		: register(c30);													\n\
	float4				g_bumpOctaveParams3		: register(c31);													\n\
																													\n\
	float3				g_camPos				: register(c32);													\n\
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
		float4			projUV					: TEXCOORD0;			// Projected UV								\n\
		float2			bumpUV0					: TEXCOORD1;			// Bump octave 0 UV							\n\
		float2			bumpUV1					: TEXCOORD2;			// Bump octave 1 UV							\n\
		float2			bumpUV2					: TEXCOORD3;			// Bump octave 2 UV							\n\
		float2			bumpUV3					: TEXCOORD4;			// Bump octave 3 UV							\n\
		float3			camPos					: TEXCOORD5;			// Camera world position					\n\
		float3			ptPos					: TEXCOORD6;			// Point world position						\n\
		float1			fog						: TEXCOORD7;			// Fog										\n\
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
		output.projUV.x = (output.position.w + output.position.x) * 0.5f;											\n\
		output.projUV.y = (output.position.w - output.position.y) * 0.5f;											\n\
		output.projUV.z = 0.0f;																						\n\
		output.projUV.w = output.position.w;																		\n\
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
		float4 normal = float4(0.0f, 1.0f, 0.0f, 0.0f);																\n\
																													\n\
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
		// Copy texture coordinates																					\n\
		output.bumpUV0 = worldPos.xz * g_bumpOctaveParams0.z + g_bumpOctaveParams0.xy * g_bumpT.x;					\n\
		output.bumpUV1 = worldPos.xz * g_bumpOctaveParams1.z + g_bumpOctaveParams1.xy * g_bumpT.x;					\n\
		output.bumpUV2 = worldPos.xz * g_bumpOctaveParams2.z + g_bumpOctaveParams2.xy * g_bumpT.x;					\n\
		output.bumpUV3 = worldPos.xz * g_bumpOctaveParams3.z + g_bumpOctaveParams3.xy * g_bumpT.x;					\n\
																													\n\
		// Copy camera and vertex position																			\n\
		output.camPos = g_camPos;																					\n\
		output.ptPos = worldPos.xyz;																				\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource = "\
																													\n\
	sampler2D			bumpMap					: register(s0);														\n\
																													\n\
	float4				g_commonConstants		: register(c0);			// Constants (1.0f, 0.5f, 0.0f, 0.05f)		\n\
	float4				g_fogColor				: register(c1);			// Fog color								\n\
																													\n\
	float4				g_bumpWeights			: register(c2);														\n\
	float4				g_factors				: register(c3);			// (Bump factor, 0, 0, 0)					\n\
	float4				g_waveColor				: register(c4);			// Wave color								\n\
																													\n\
	struct PS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR0;															\n\
		float4			projUV					: TEXCOORD0;			// Projected UV								\n\
		float2			bumpUV0					: TEXCOORD1;			// Bump octave 0 UV							\n\
		float2			bumpUV1					: TEXCOORD2;			// Bump octave 1 UV							\n\
		float2			bumpUV2					: TEXCOORD3;			// Bump octave 2 UV							\n\
		float2			bumpUV3					: TEXCOORD4;			// Bump octave 3 UV							\n\
		float3			camPos					: TEXCOORD5;			// Camera world position					\n\
		float3			ptPos					: TEXCOORD6;			// Point world position						\n\
		float1			fog						: TEXCOORD7;			// Fog										\n\
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
																													\n\
		// Sample bump maps																							\n\
		float3 n0 = tex2D(bumpMap, v.bumpUV0);																		\n\
		float3 n1 = tex2D(bumpMap, v.bumpUV1);																		\n\
		float3 n2 = tex2D(bumpMap, v.bumpUV2);																		\n\
		float3 n3 = tex2D(bumpMap, v.bumpUV3);																		\n\
																													\n\
		// Combine bump octaves according to their weights															\n\
		float3 n = (n0 * g_bumpWeights.x + n1 * g_bumpWeights.y + n2 * g_bumpWeights.z + n3 * g_bumpWeights.w) - 0.5f;		\n\
																													\n\
		// Normalize bump normal																					\n\
		n = normalize(n);																							\n\
																													\n\
		// Exchange bump normal's y and z so it can be used as a normal vector										\n\
		float1 swapTemp = n.y;																						\n\
		n.y = n.z;																									\n\
		n.z = swapTemp;																								\n\
																													\n\
		// Weaken the bump normal vector by specified factor														\n\
		n = lerp(n, float3(0.0f, 1.0f, 0.0f), g_factors[0]);														\n\
																													\n\
		// Unpack cam to water vec																					\n\
		float3 camToWaterVecNrm = normalize(v.camPos - v.ptPos);													\n\
																													\n\
		// Dot cam-to-water vector with bump normal																	\n\
		float1 cosTheta = clamp(dot(camToWaterVecNrm, n), 0.0f, 1.0f);												\n\
		float4 fresnelTerm = (0.02037f + 0.97963f * pow(1 - cosTheta, 5));											\n\
																													\n\
		// Lerp between reflection & refraction based on fresnel term												\n\
		output.color = lerp(v.diffuse, g_waveColor, fresnelTerm);													\n\
																													\n\
		// Lerp between output color and fog																		\n\
		output.color = lerp(g_fogColor.xyzw, output.color, v.fog.xxxx);												\n\
																													\n\
		// Use diffuse alpha as output alpha																		\n\
		output.color.w = v.diffuse.w;																				\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile vertex shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Compile pixel shader
	m_pixelShader = RuD3DCompilePixelShader(psSource, "main", "ps_2_0", NULL);

	// Overrides
	m_override_ZWrite = TRUE;
}

CRuShader_SM20EZ_Water_Minimal::~CRuShader_SM20EZ_Water_Minimal()
{
}

void CRuShader_SM20EZ_Water_Minimal::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Water_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants + 5);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants + 3);

		m_engine->UseDefaultSamplerFilterState(0);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

void CRuShader_SM20EZ_Water_Minimal::EndPass(INT32 passNumber)
{
}

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Water_Lava::CRuShader_SM20EZ_Water_Lava(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Water_Base(engine)
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
																													\n\
	float4				g_bumpT					: register(c27);													\n\
																													\n\
	float4				g_bumpOctaveParams0		: register(c28);													\n\
	float4				g_bumpOctaveParams1		: register(c29);													\n\
	float4				g_bumpOctaveParams2		: register(c30);													\n\
	float4				g_bumpOctaveParams3		: register(c31);													\n\
																													\n\
	float3				g_camPos				: register(c32);													\n\
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
		float4			projUV					: TEXCOORD0;			// Projected UV								\n\
		float2			bumpUV0					: TEXCOORD1;			// Bump octave 0 UV							\n\
		float2			bumpUV1					: TEXCOORD2;			// Bump octave 1 UV							\n\
		float2			bumpUV2					: TEXCOORD3;			// Bump octave 2 UV							\n\
		float2			bumpUV3					: TEXCOORD4;			// Bump octave 3 UV							\n\
		float3			camPos					: TEXCOORD5;			// Camera world position					\n\
		float3			ptPos					: TEXCOORD6;			// Point world position						\n\
		float1			fog						: TEXCOORD7;			// Fog										\n\
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
		output.projUV.x = (output.position.w + output.position.x) * 0.5f;											\n\
		output.projUV.y = (output.position.w - output.position.y) * 0.5f;											\n\
		output.projUV.z = 0.0f;																						\n\
		output.projUV.w = output.position.w;																		\n\
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
		float4 normal = float4(0.0f, 1.0f, 0.0f, 0.0f);																\n\
																													\n\
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
		// Copy texture coordinates																					\n\
		output.bumpUV0 = worldPos.xz * g_bumpOctaveParams0.z + g_bumpOctaveParams0.xy * g_bumpT.x;					\n\
		output.bumpUV1 = worldPos.xz * g_bumpOctaveParams1.z + g_bumpOctaveParams1.xy * g_bumpT.x;					\n\
		output.bumpUV2 = worldPos.xz * g_bumpOctaveParams2.z + g_bumpOctaveParams2.xy * g_bumpT.x;					\n\
		output.bumpUV3 = worldPos.xz * g_bumpOctaveParams3.z + g_bumpOctaveParams3.xy * g_bumpT.x;					\n\
																													\n\
		// Copy camera and vertex position																			\n\
		output.camPos = g_camPos;																					\n\
		output.ptPos = worldPos.xyz;																				\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource = "\
																													\n\
	sampler2D			bumpMap					: register(s0);														\n\
																													\n\
	float4				g_commonConstants		: register(c0);			// Constants (1.0f, 0.5f, 0.0f, 0.05f)		\n\
	float4				g_fogColor				: register(c1);			// Fog color								\n\
																													\n\
	float4				g_bumpWeights			: register(c2);														\n\
	float4				g_factors				: register(c3);			// (Bump factor, 0, 0, 0)					\n\
	float4				g_waveColor				: register(c4);			// Wave color								\n\
																													\n\
	struct PS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR0;															\n\
		float4			projUV					: TEXCOORD0;			// Projected UV								\n\
		float2			bumpUV0					: TEXCOORD1;			// Bump octave 0 UV							\n\
		float2			bumpUV1					: TEXCOORD2;			// Bump octave 1 UV							\n\
		float2			bumpUV2					: TEXCOORD3;			// Bump octave 2 UV							\n\
		float2			bumpUV3					: TEXCOORD4;			// Bump octave 3 UV							\n\
		float3			camPos					: TEXCOORD5;			// Camera world position					\n\
		float3			ptPos					: TEXCOORD6;			// Point world position						\n\
		float1			fog						: TEXCOORD7;			// Fog										\n\
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
																													\n\
		// Sample bump maps																							\n\
		float4 c0 = tex2D(bumpMap, v.bumpUV0);																		\n\
		float4 c1 = tex2D(bumpMap, v.bumpUV1);																		\n\
		float4 c2 = tex2D(bumpMap, v.bumpUV2);																		\n\
		float4 c3 = tex2D(bumpMap, v.bumpUV3);																		\n\
																													\n\
		// Combine bump octaves according to their weights															\n\
		float4 lavaColor = (c0 * g_bumpWeights.x + c1 * g_bumpWeights.y + c2 * g_bumpWeights.z + c3 * g_bumpWeights.w);		\n\
																													\n\
		// Lerp between output color and fog																		\n\
		output.color = lerp(g_fogColor.xyzw, lavaColor * v.diffuse, v.fog.xxxx);									\n\
																													\n\
		// Use diffuse alpha as output alpha																		\n\
		output.color.w = v.diffuse.w;																				\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile vertex shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Compile pixel shader
	m_pixelShader = RuD3DCompilePixelShader(psSource, "main", "ps_2_0", NULL);

	// Overrides
	m_override_ZWrite = TRUE;
}

CRuShader_SM20EZ_Water_Lava::~CRuShader_SM20EZ_Water_Lava()
{
}

void CRuShader_SM20EZ_Water_Lava::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Water_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants + 5);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants + 3);

		m_engine->UseDefaultSamplerFilterState(0);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->UseDefaultSamplerFilterState(1);
		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

void CRuShader_SM20EZ_Water_Lava::EndPass(INT32 passNumber)
{
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
