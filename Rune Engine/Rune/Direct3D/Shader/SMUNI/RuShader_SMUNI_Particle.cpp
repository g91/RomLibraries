#include "../../../Direct3D/Shader/SMUNI/RuShader_SMUNI_Particle.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SMUNI_Particle_SpriteEx::CRuShader_SMUNI_Particle_SpriteEx(CRuEngineD3D *engine)
:	CRuShader_SMUNI_Base(engine),
	m_billboardType(ruBILLBOARDTYPE_UNCONSTRAINED),
	m_billboardAxis(ruAXIS_Y_POSITIVE)
{
	// Overrides
//	m_override_Fog = FALSE;
//	m_override_ZWrite = TRUE;

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
	float4				g_ambientColor			: register(c10);													\n\
	float4				g_shCoefficients[9]		: register(c11);	// Spherical harmonics coefficients				\n\
	float4				g_specularDirection		: register(c21);	// Object space specular position				\n\
	float4				g_specularColor			: register(c22);													\n\
	float4				g_selfIllumination		: register(c23);	// Self-illumination term						\n\
	float4				g_globalSettings		: register(c25);	// (Fog Constant, Start, End, Extrude Scale)	\n\
	float4				g_globalDiffuse			: register(c26);													\n\
	float4x4			g_billboardTransform	: register(c27);	// Billboard transform							\n\
	float4				g_prtParam				: register(c31);	// (u increment, v increment, AR, world ROT)	\n\
	float4				g_quadScaleOffset		: register(c32);	// (scaleX, scaleY, offsetX, offsetY)			\n\
	float4x4			g_orientationTransform	: register(c33);	// Particle poly orientation transform			\n\
	float4				g_instanceData[210]		: register(c37);	// Instance data								\n\
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
																													\n\
		// Calculate instance data indices																			\n\
		float1 posScaleOffset = (v.diffuse.w * 256.0f) * 3.0f;														\n\
		float1 uvRotOffset = posScaleOffset + 1.0f;																	\n\
		float1 colorOffset = uvRotOffset + 1.0f;																	\n\
																													\n\
		// Scale & translate position by quad scale/offset															\n\
		float3 scaledPosition = v.position.xyz;																		\n\
		scaledPosition.y = (scaledPosition.y + g_quadScaleOffset[2]) * g_quadScaleOffset[0];						\n\
		scaledPosition.z = (scaledPosition.z + g_quadScaleOffset[3]) * g_quadScaleOffset[1] * g_prtParam.z;			\n\
																													\n\
		// Scale position by particle scale																			\n\
		scaledPosition = scaledPosition * g_instanceData[(int) posScaleOffset].www;									\n\
																													\n\
		// Rotate position																							\n\
		float3 rotRow0 = float3(0.0f, g_instanceData[(int) uvRotOffset].w, -g_instanceData[(int) uvRotOffset].z);	\n\
		float3 rotRow1 = float3(0.0f, g_instanceData[(int) uvRotOffset].z, g_instanceData[(int) uvRotOffset].w);	\n\
		worldPos.x = scaledPosition.x;																				\n\
		worldPos.y = dot(scaledPosition, rotRow0);																	\n\
		worldPos.z = dot(scaledPosition, rotRow1);																	\n\
																													\n\
		// Orientation transform																					\n\
		worldPos.xyz = mul(worldPos, (float4x3) g_orientationTransform);											\n\
																													\n\
		// Handle world rotation																					\n\
		float4 rotatedWorldPos = float4(1, 1, 1, 1);																\n\
		rotatedWorldPos.xyz = mul(worldPos, (float3x3) g_worldTransform);											\n\
		worldPos = lerp(worldPos, rotatedWorldPos, g_prtParam.w);													\n\
																													\n\
		// Billboard transform																						\n\
		worldPos.xyz = mul(worldPos, (float3x3) g_billboardTransform);												\n\
																													\n\
		// Transform position to world space																		\n\
		float4 particleWorldPos = float4(1, 1, 1, 1);																\n\
		particleWorldPos.xyz = g_instanceData[(int) posScaleOffset].xyz;											\n\
		worldPos.xyz = worldPos.xyz + mul(particleWorldPos, (float4x3) g_worldTransform);							\n\
																													\n\
		// Transform position to view space																			\n\
		float4 viewPos = float4(1, 1, 1, 1);																		\n\
		viewPos.xyz = mul(worldPos, (float4x3) g_viewTransform);													\n\
																													\n\
		// Transform position to homogeneous space																	\n\
		output.position = mul(viewPos, (float4x4) g_projectionTransform);											\n\
																													\n\
		// Calculate fog																							\n\
		float1 rcpFogRange = 1.0f / (g_globalSettings.z - g_globalSettings.y);										\n\
		float1 relativeZ = max(viewPos.z - g_globalSettings.y, 0.0f);												\n\
		output.fog = clamp(min(1.0f - (relativeZ * rcpFogRange), 1.0f - g_globalSettings.x), 0.0f, 1.0f);			\n\
																													\n\
		// Multiply particle color by global diffuse																\n\
		output.diffuse = g_instanceData[(int) colorOffset] * g_globalDiffuse;										\n\
																													\n\
		// Calculate UV based on current frame index and texture uv increment										\n\
		output.uv_albedo.xy = (g_instanceData[(int) uvRotOffset].xy * g_prtParam.xy) + v.diffuse.xy * g_prtParam.xy;\n\
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
	m_numBaseConstants = 31;
	m_numBasePixelShaderConstants = 2;
}

CRuShader_SMUNI_Particle_SpriteEx::~CRuShader_SMUNI_Particle_SpriteEx()
{
}

void CRuShader_SMUNI_Particle_SpriteEx::BeginPass(INT32 passNumber)
{
	// Force 2-sided polygon
	m_2Sided = TRUE;

	// Force translucency
	m_material_Translucent = TRUE;

	// Force alpha test off
	m_alphaTestEnabled = FALSE;

	// Not al alpha Z pass
	m_fx_AlphaZPass = FALSE;

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
		// Setup billboard transform
		CRuMatrix4x4 billboardTransform;

		if(m_viewTransform)
		{
			switch(m_billboardType)
			{
				default:
				case ruBILLBOARDTYPE_NONE:
					billboardTransform.SetToIdentity();
					break;

				case ruBILLBOARDTYPE_UNCONSTRAINED:
					billboardTransform.SetToBillboard(*((CRuMatrix4x4 *) m_viewTransform), CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 1.0f, 0.0f));
					break;

				case ruBILLBOARDTYPE_AXIAL:
					billboardTransform.SetToAxialBillboard(*((CRuMatrix4x4 *) m_viewTransform), CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 1.0f, 0.0f));
					break;
				case ruBILLBOARDTYPE_AXIAL_CUSTOM:
					{
						CRuVector3 forwardAxis;
						forwardAxis.Set(1.0f, 0.0f, 0.0f);
/*						switch(m_orientation)
						{
							case ruAXIS_X_POSITIVE:
								forwardAxis.Set(1.0f, 0.0f, 0.0f);
								break;
							case ruAXIS_Y_POSITIVE:
								forwardAxis.Set(0.0f, 1.0f, 0.0f);
								break;
							default:
							case ruAXIS_Z_POSITIVE:
								forwardAxis.Set(0.0f, 0.0f, 1.0f);
								break;
						}*/

						switch(m_billboardAxis)
						{
							case ruAXIS_X_POSITIVE:
								billboardTransform.SetToAxialBillboard(*((CRuMatrix4x4 *) m_viewTransform), forwardAxis, CRuVector3(1.0f, 0.0f, 0.0f));
								break;
							case ruAXIS_Y_POSITIVE:
								billboardTransform.SetToAxialBillboard(*((CRuMatrix4x4 *) m_viewTransform), forwardAxis, CRuVector3(0.0f, 1.0f, 0.0f));
								break;
							case ruAXIS_Z_POSITIVE:
								billboardTransform.SetToAxialBillboard(*((CRuMatrix4x4 *) m_viewTransform), forwardAxis, CRuVector3(0.0f, 0.0f, 1.0f));
								break;
							case ruAXIS_X_NEGATIVE:
								billboardTransform.SetToAxialBillboard(*((CRuMatrix4x4 *) m_viewTransform), forwardAxis, CRuVector3(-1.0f, 0.0f, 0.0f));
								break;
							case ruAXIS_Y_NEGATIVE:
								billboardTransform.SetToAxialBillboard(*((CRuMatrix4x4 *) m_viewTransform), forwardAxis, CRuVector3(0.0f, -1.0f, 0.0f));
								break;
							case ruAXIS_Z_NEGATIVE:
								billboardTransform.SetToAxialBillboard(*((CRuMatrix4x4 *) m_viewTransform), forwardAxis, CRuVector3(0.0f, 0.0f, -1.0f));
								break;
						}
					}
					break;
			}
		}
		else
		{
			billboardTransform.SetToIdentity();
		}

		// Copy billboard transform to shader constants
		INT32 billboardTransformOffset = 27 * 4;

		for(UINT32 i = 0, k = 0; i < 4; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k + billboardTransformOffset] = billboardTransform.m_elements[j][i];
			}
		}

		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants + m_extraConstantCount);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		m_engine->UseDefaultSamplerFilterState(0);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

void CRuShader_SMUNI_Particle_SpriteEx::EndPass(INT32 passNumber)
{
	CRuShader_SMUNI_Base::EndPass(passNumber);

	m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CRuShader_SMUNI_Particle_SpriteEx::SetParameters(const CRuParameterCache *paramCache)
{
	UINT64 param;

	CRuShader_SMUNI_Base::SetParameters(paramCache);

	if(paramCache->GetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, param))
	{
		m_numInstances = (UINT32) param;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_GEOINSTANCE_S_DATA, param))
	{
		float *instanceData = reinterpret_cast<float *>(param);

		// Number of constants = instance dataset + 24 constants for the per-draw data
		INT32 numConstants = 24 + m_numInstances * 12;
		INT32 constantOffset = 31 * 4;

		RuMemcpy(&m_shaderConstants[constantOffset], instanceData, sizeof(float) * numConstants);

		m_extraConstantCount = numConstants >> 2;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_DWORD_1, param))
	{
		m_billboardAxis = static_cast<RuOrthogonalAxis>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_FX_BILLBOARD, param))
	{
		m_billboardType = static_cast<RuBillboardType>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_TRANSFORM_VIEW, param))
	{
		m_viewTransform = reinterpret_cast<CRuMatrix4x4 *>(param);
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
