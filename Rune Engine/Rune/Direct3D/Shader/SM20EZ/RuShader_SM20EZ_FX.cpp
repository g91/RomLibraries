#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_FX.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_FX_GhostTrail::CRuShader_SM20EZ_FX_GhostTrail(CRuEngineD3D *engine)
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

	// Vertex shader source
	char *vsSource = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position 0							\n\
	dcl_texcoord0		v4											; Declare input texture coordinates 0			\n\
	dcl_color			v12											; Declare diffuse								\n\
																													\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	mov					r2,			c20.yyyy						; Move 1 into r1								\n\
																													\n\
	; Calculate position																							\n\
	m4x3				r1,			v0,			c0					; Transform vertex to world space				\n\
	m4x3				r0,			r1,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r0,			c6					; Transform vertex to homogeneous space			\n"
	"																												\n\
	; Fog factor																									\n\
	mov					oFog,		c20.y							; Output fog factor (constant 1.0)				\n"
	"																												\n\
	; Diffuse color																									\n\
	mul					r4,			v12,		c26					; Multiply diffuse by global diffuse			\n\
	mov					oD0,		r4								; Set diffuse									\n\
																													\n\
	; Calculate texture coordinates																					\n\
	mov					oT0,		v4																				\n\
	";

	// Assemble vertex shader
	m_vertexShader = RuD3DAssembleVertexShader(vsSource);

	// Overrides
	m_override_ZWrite = TRUE;
}

CRuShader_SM20EZ_FX_GhostTrail::~CRuShader_SM20EZ_FX_GhostTrail()
{
}

void CRuShader_SM20EZ_FX_GhostTrail::BeginPass(INT32 passNumber)
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
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		m_engine->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		m_engine->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);

		m_engine->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
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

void CRuShader_SM20EZ_FX_GhostTrail::EndPass(INT32 passNumber)
{
	CRuShader_SM20EZ_Base::EndPass(passNumber);

	m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CRuShader_SM20EZ_FX_GhostTrail::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SM20EZ_Base::SetParameters(paramCache);
}

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_FX_Decal::CRuShader_SM20EZ_FX_Decal(CRuEngineD3D *engine)
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

	// Vertex shader source
	char *vsSource = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position 0							\n\
	dcl_texcoord0		v4											; Declare input texture coordinates 0			\n\
	dcl_color			v12											; Declare diffuse								\n\
																													\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	mov					r2,			c20.yyyy						; Move 1 into r1								\n\
																													\n\
	; Calculate position																							\n\
	m4x3				r0,			v0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r0,			c6					; Transform vertex to homogeneous space			\n\
																													\n\
	; Fog factor																									\n\
	mov					oFog,		c20.y							; Output fog factor (constant 1.0)				\n\
																													\n\
	; Diffuse color																									\n\
	mul					r4,			v12,		c26					; Multiply diffuse by global diffuse			\n\
	mov					oD0,		r4								; Set diffuse									\n\
																													\n\
	; Calculate texture coordinates																					\n\
	m4x4				oT0,		v0,			c27					; Transform vertex to uv space into T0			\n\
	";

	char *psSource = "\
																													\n\
	ps_2_0																											\n\
																													\n\
	; Declare texture samplers																						\n\
	dcl_2d				s0											; DIFFUSE										\n\
																													\n\
	; Declare texture coordinate registers																			\n\
	dcl					t0.xyzw																						\n\
																													\n\
	dcl					v0.xyzw																						\n\
																													\n\
	rcp					r0,			t0.w							; Get 1/w										\n\
	mul					r0,			t0,			r0					; Divide by w									\n\
																													\n\
	texld				r1,			r0,			s0					; Sample DIFFUSE into R1						\n\
	mul					r1,			r1,			v0					; Modulate by diffuse color						\n\
																													\n\
	mov					oC0,		r1								; Move result into output						\n\
	";

	// Assemble vertex shader
	m_vertexShader = RuD3DAssembleVertexShader(vsSource);

	// Assemble pixel shader
	m_pixelShader = RuD3DAssemblePixelShader(psSource);

	// Overrides
	m_override_ZWrite = TRUE;
}

CRuShader_SM20EZ_FX_Decal::~CRuShader_SM20EZ_FX_Decal()
{
}

void CRuShader_SM20EZ_FX_Decal::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants + 1);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
/*
		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
*/
		m_engine->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, RuCastFloatToINT64(-1.0f));

		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}
}

void CRuShader_SM20EZ_FX_Decal::EndPass(INT32 passNumber)
{
	CRuShader_SM20EZ_Base::EndPass(passNumber);

	m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_engine->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, RuCastFloatToINT64(0.0f));
}

void CRuShader_SM20EZ_FX_Decal::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SM20EZ_Base::SetParameters(paramCache);

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

CRuShader_SM20EZ_FX_DecalShadow::CRuShader_SM20EZ_FX_DecalShadow(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Base(engine)
{
	m_override_AlphaTest = TRUE;
	m_override_AlphaBlend = TRUE;

	// Setup vertex elements
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },
		{ 0,	12,	ruELEMENTTYPE_D3DCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexDecl = m_engine->CreateVertexDeclaration(vertexElements);

	// Vertex shader source
	char *vsSource = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position 0							\n\
	dcl_texcoord0		v4											; Declare input texture coordinates 0			\n\
	dcl_color			v12											; Declare diffuse								\n\
																													\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	mov					r2,			c20.yyyy						; Move 1 into r1								\n\
																													\n\
	; Calculate position																							\n\
	m4x3				r0,			v0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r0,			c6					; Transform vertex to homogeneous space			\n\
																													\n\
	; Fog factor																									\n\
	mov					oFog,		c20.y							; Output fog factor (constant 1.0)				\n\
																													\n\
	; Diffuse color																									\n\
	mul					r4,			v12,		c26					; Multiply diffuse by global diffuse			\n\
	mov					oD0,		r4								; Set diffuse									\n\
																													\n\
	; Calculate texture coordinates																					\n\
	m4x4				oT0,		v0,			c27					; Transform vertex to map uv space into T0		\n\
	m4x4				oT1,		v0,			c31					; Transform vertex to clamp uv space into T1	\n\
	";

	char *psSource = "\
																													\n\
	ps_2_0																											\n\
																													\n\
	; Declare texture samplers																						\n\
	dcl_2d				s0											; DIFFUSE										\n\
	dcl_2d				s1											; CLAMP											\n\
																													\n\
	; Declare texture coordinate registers																			\n\
	dcl					t0.xyzw																						\n\
	dcl					t1.xyzw																						\n\
																													\n\
	dcl					v0.xyzw																						\n\
																													\n\
	rcp					r0,			t0.w							; Get 1/w										\n\
	mul					r0,			t0,			r0					; Divide by w									\n\
																													\n\
	rcp					r1,			t1.w							; Get 1/w										\n\
	mul					r1,			t1,			r1					; Divide by w									\n\
																													\n\
	texld				r2,			r0,			s0					; Sample DIFFUSE into R2						\n\
	texld				r3,			r1,			s1					; Sample CLAMP into R3							\n\
	mul					r0,			r2,			r3					; Modulate DIFFUSE with CLAMP					\n\
	mul					r0,			r0,			v0					; Modulate by diffuse color						\n\
																													\n\
	mov					oC0,		r0								; Move result into output						\n\
	";

	// Assemble vertex shader
	m_vertexShader = RuD3DAssembleVertexShader(vsSource);

	// Assemble pixel shader
	m_pixelShader = RuD3DAssemblePixelShader(psSource);

	// Overrides
	m_override_ZWrite = TRUE;
}

CRuShader_SM20EZ_FX_DecalShadow::~CRuShader_SM20EZ_FX_DecalShadow()
{
}

void CRuShader_SM20EZ_FX_DecalShadow::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants + 5);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);

		m_engine->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
		m_engine->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ZERO);
		m_engine->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

		m_engine->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, RuCastFloatToINT64(-1.0f));

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

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

void CRuShader_SM20EZ_FX_DecalShadow::EndPass(INT32 passNumber)
{
	CRuShader_SM20EZ_Base::EndPass(passNumber);

	m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_engine->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);

	m_engine->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, RuCastFloatToINT64(0.0f));
}

void CRuShader_SM20EZ_FX_DecalShadow::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SM20EZ_Base::SetParameters(paramCache);

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

CRuShader_SM20EZ_FX_Lightning::CRuShader_SM20EZ_FX_Lightning(CRuEngineD3D *engine)
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
		float4 worldPos = float4(1, 1, 1, 1);																		\n\
		float4 viewPos = float4(1, 1, 1, 1);																		\n\
																													\n\
		// Transform position to projected space																	\n\
		worldPos.xyz = mul(v.position, (float4x3) g_worldTransform);												\n\
		viewPos.xyz = mul(worldPos, (float4x3) g_viewTransform);													\n\
		output.position = mul(viewPos, (float4x4) g_projectionTransform);											\n\
																													\n\
		// Calculate fog (should set to one)																		\n\
		float1 rcpFogRange = 1.0f / (g_globalSettings.z - g_globalSettings.y);										\n\
		float1 relativeZ = max(viewPos.z - g_globalSettings.y, 0.0f);												\n\
		output.fog = clamp(1.0f - (relativeZ * rcpFogRange), 0.0f, 1.0f);											\n\
																													\n\
		// Accumulate ambient color																					\n\
		output.diffuse = g_globalDiffuse;																			\n\
																													\n\
		output.texcoord0 = v.texcoord;																				\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	char *psSource = "\
																													\n\
	sampler2D diffuseTex : register(s0);																			\n\
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
		float4 texColor = tex2D(diffuseTex, v.texcoord0);															\n\
		output.color = v.diffuse * texColor;																		\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile vertex shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Compile pixel shader
	m_pixelShader = RuD3DCompilePixelShader(psSource, "main", "ps_2_0", NULL);
}

CRuShader_SM20EZ_FX_Lightning::~CRuShader_SM20EZ_FX_Lightning()
{
}

void CRuShader_SM20EZ_FX_Lightning::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants + 1);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		m_engine->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}
}

void CRuShader_SM20EZ_FX_Lightning::EndPass(INT32 passNumber)
{
	CRuShader_SM20EZ_Base::EndPass(passNumber);

	m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CRuShader_SM20EZ_FX_Lightning::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SM20EZ_Base::SetParameters(paramCache);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
