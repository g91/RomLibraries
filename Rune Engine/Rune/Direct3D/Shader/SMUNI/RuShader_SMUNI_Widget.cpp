#include "../../../Direct3D/Shader/SMUNI/RuShader_SMUNI_Widget.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SMUNI_Widget_Label::CRuShader_SMUNI_Widget_Label(CRuEngineD3D *engine)
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
	float4x4			g_billboardTransform	: register(c27);													\n\
																													\n\
	float4				g_labelScale			: register(c31);													\n\
																													\n\
	struct VS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR0;															\n\
		float2			texcoord				: TEXCOORD;															\n\
	};																												\n\
																													\n\
	struct VS_OUTPUT																								\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float4			diffuse					: COLOR;															\n\
		float2			texcoord				: TEXCOORD;															\n\
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
		// Transform position to projected space																	\n\
		worldPos.xyz = v.position.xyz * g_labelScale;																\n\
		worldPos.xyz = mul(worldPos, (float3x3) g_billboardTransform);												\n\
		worldPos.x = worldPos.x + g_worldTransform._m30;															\n\
		worldPos.y = worldPos.y + g_worldTransform._m31;															\n\
		worldPos.z = worldPos.z + g_worldTransform._m32;															\n\
																													\n\
		viewPos.xyz = mul(worldPos, (float4x3) g_viewTransform);													\n\
		output.position = mul(viewPos, (float4x4) g_projectionTransform);											\n\
																													\n\
		// Disable fog																								\n\
		output.fog = 1.0f;																							\n\
																													\n\
		output.diffuse = v.diffuse;																					\n\
																													\n\
		// Copy texture coordinates																					\n\
		output.texcoord = v.texcoord;																				\n\
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
		// Sample diffuse map																						\n\
		float4 color = tex2D(tex0, v.texcoord0);																	\n\
																													\n\
		// Multiply RGBA components																					\n\
		color = color * v.diffuse;																					\n\
																													\n\
		output.color = color;																						\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource_NoAlphaChannel = "\
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
		// Sample diffuse map																						\n\
		float4 color = tex2D(tex0, v.texcoord0);																	\n\
																													\n\
		// Take the minima of alpha and multiply by diffuse alpha													\n\
		color.w = min(color.x, color.w) * v.diffuse.w;																\n\
																													\n\
		// Multiply RGB components																					\n\
		color.xyz = color.xyz * v.diffuse.xyz;																		\n\
																													\n\
		output.color = color;																						\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile vertex shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Compile pixel shaders
	m_pixelShader = RuD3DCompilePixelShader(psSource, "main", "ps_2_0", NULL);
	m_pixelShader_NoAlphaChannel = RuD3DCompilePixelShader(psSource_NoAlphaChannel, "main", "ps_2_0", NULL);

	// Set constants count
	m_numBaseConstants = 32;
	m_numBasePixelShaderConstants = 1;
}

CRuShader_SMUNI_Widget_Label::~CRuShader_SMUNI_Widget_Label()
{
	ruSAFE_RELEASE(m_pixelShader_NoAlphaChannel);
}

void CRuShader_SMUNI_Widget_Label::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SMUNI_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_useAlphaChannel ? m_pixelShader : m_pixelShader_NoAlphaChannel);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}
}

void CRuShader_SMUNI_Widget_Label::EndPass(INT32 passNumber)
{
}

void CRuShader_SMUNI_Widget_Label::SetParameters(const CRuParameterCache *paramCache)
{
	UINT64 param;

	CRuShader_SMUNI_Base::SetParameters(paramCache);

	if(paramCache->GetParameter(ruPARAMTYPE_TRANSFORM_VIEW, param))
	{
		// Setup texture thingy
		CRuMatrix4x4 billboardTransform;

		billboardTransform.SetToBillboard(*((CRuMatrix4x4 *) param), CRuVector3(0.0f, 0.0f, -1.0f), CRuVector3(0.0f, 1.0f, 0.0f));

		INT32 billboardTransformOffset = 27 * 4;

		for(UINT32 i = 0, k = billboardTransformOffset; i < 4; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k] = billboardTransform.m_elements[j][i];
			}
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_CUSTOM_0, param))
	{
		INT32 labelScaleOffset = 31 * 4;

		REAL labelScale = RuCastINT64ToFloat(param);

		m_shaderConstants[labelScaleOffset++] = labelScale;
		m_shaderConstants[labelScaleOffset++] = labelScale;
		m_shaderConstants[labelScaleOffset++] = labelScale;
		m_shaderConstants[labelScaleOffset++] = 1.0f;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_CUSTOM_1, param))
	{
		m_useAlphaChannel = static_cast<BOOL>(param);
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
