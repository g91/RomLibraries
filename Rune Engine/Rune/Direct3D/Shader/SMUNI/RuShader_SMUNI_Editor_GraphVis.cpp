#include "../../../Direct3D/Shader/SMUNI/RuShader_SMUNI_Editor_GraphVis.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SMUNI_Editor_GraphVis::CRuShader_SMUNI_Editor_GraphVis(CRuEngineD3D *engine)
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
	ruSAFE_RELEASE(m_vertexDecl);
	m_vertexDecl = m_engine->CreateVertexDeclaration(vertexElements);

	char *vsSource = "\
																													\n\
	// Globals																										\n\
	float4x3			g_worldTransform		: register(c0);														\n\
	float4x3			g_viewTransform			: register(c3);														\n\
	float4x4			g_projectionTransform	: register(c6);														\n\
																													\n\
	float4				g_globalDiffuse			: register(c26);		// (?, Fog Start, Fog End, Tween Factor)	\n\
																													\n\
	float4				g_instanceData[200]		: register(c32);		// Instance data							\n\
																													\n\
	struct VS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR0;															\n\
		float2			texcoord0				: TEXCOORD0;														\n\
	};																												\n\
																													\n\
	struct VS_OUTPUT																								\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR;															\n\
		float2			texcoord0				: TEXCOORD0;														\n\
		float1			fog						: FOG;																\n\
	};																												\n\
																													\n\
	VS_OUTPUT main(const VS_INPUT v)																				\n\
	{																												\n\
		VS_OUTPUT output;																							\n\
																													\n\
		// Initialize world-view vectors to 1																		\n\
		float4 objectPos = float4(0, 0, 0, 1);																		\n\
		float4 worldPos = float4(1, 1, 1, 1);																		\n\
		float4 viewPos = float4(1, 1, 1, 1);																		\n\
																													\n\
		// Calculate instance data indices																			\n\
		float1 scaleOffset = (v.diffuse.w * 256.0f) * 2.0f;															\n\
		float1 posOffset = scaleOffset + 1.0f;																		\n\
																													\n\
		objectPos.xyz = v.position.xyz * g_instanceData[(int) scaleOffset].xyz;										\n\
		objectPos.xyz = objectPos.xyz + g_instanceData[(int) posOffset].xyz;										\n\
																													\n\
		// Transform position to projected space																	\n\
		viewPos.xyz = mul(objectPos, (float4x3) g_viewTransform);													\n\
		output.position = mul(viewPos, (float4x4) g_projectionTransform);											\n\
																													\n\
		// Multiply vertex diffuse by global diffuse and use global alpha as output alpha							\n\
		output.diffuse.xyz = v.diffuse.xyz * g_globalDiffuse.xyz;													\n\
		output.diffuse.w = g_globalDiffuse.w;																		\n\
																													\n\
		// Copy UV																									\n\
		output.texcoord0 = v.texcoord0;																				\n\
																													\n\
		// Set fog to constant 1																					\n\
		output.fog = 1.0f;																							\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

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
		// Sample distortion map																					\n\
		float4 color = tex2D(tex0, v.texcoord0);																	\n\
																													\n\
		output.color = color * v.diffuse;																			\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Assemble vertex shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Assemble pixel shader
	m_pixelShader = RuD3DCompilePixelShader(psSource, "main", "ps_2_0", NULL);
}

CRuShader_SMUNI_Editor_GraphVis::~CRuShader_SMUNI_Editor_GraphVis()
{
}

void CRuShader_SMUNI_Editor_GraphVis::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SMUNI_Base::BeginPass(passNumber);

	INT32 globalDiffuseOffset = 104;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_r;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_g;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_b;

	INT32 globalAlphaOffset = 107;
	m_shaderConstants[globalAlphaOffset] = m_material_EffectiveAlpha;

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, 32 + m_numInstances * 2);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);
	}
}

void CRuShader_SMUNI_Editor_GraphVis::SetParameters(const CRuParameterCache *paramCache)
{
	// Chain set parameters call to parent
	CRuShader_SMUNI_Base::SetParameters(paramCache);

	UINT64 param;

	if(paramCache->GetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, param))
	{
		m_numInstances = (UINT32) param;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_GEOINSTANCE_S_DATA, param))
	{
		float *instanceData = reinterpret_cast<float *>(param);

		// Number of constants = instance dataset + 24 constants for the per-draw data
		INT32 numConstants = 4 + m_numInstances * 8;
		INT32 constantOffset = 31 * 4;

		RuMemcpy(&m_shaderConstants[constantOffset], instanceData, sizeof(float) * numConstants);
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
