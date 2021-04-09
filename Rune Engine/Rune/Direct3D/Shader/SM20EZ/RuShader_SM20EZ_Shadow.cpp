#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Shadow.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Static_DepthMap::CRuShader_SM20EZ_Static_DepthMap(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Static_Base(engine)
{
	m_override_Fog = TRUE;
	m_override_AlphaTest = FALSE;
	m_override_AlphaBlend = TRUE;

	char *vsSource = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_normal			v1											; Declare normal								\n\
	dcl_texcoord0		v5											; Declare input texcoord 0						\n\
																													\n\
	; Transform position into homogeneous space																		\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	m4x3				r0.xyz,		v0,			c0					; Transform vertex to world space				\n\
	m4x3				r1.xyz,		r0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r0,			c26					; Transform vertex to trapezoidal space			\n\
																													\n\
	mov					oT0,		v5								; Copy texture coordinates to output			\n\
	m4x4				oT1,		r1,			c6					; Transform vertex to homogeneous space			\n\
	mov					r0,			c30								; Copy object ID encoding to diffuse out		\n\
	mov					r0.w,		c20.y							; Copy object ID encoding to diffuse out		\n\
	mov					oD0,		r0								; Copy object ID encoding to diffuse out		\n\
																													\n\
	; Fog factor																									\n\
	mov					oFog,		c20.y							; Output fog factor								\n"
	"																												\n\
	";

	char *psSource = "\
																													\n\
	ps_2_0																											\n\
																													\n\
	def					c2,			0.5, 0.0, 0.0, 0.0																\n\
																													\n\
	; Declare texture samplers																						\n\
	dcl_2d				s0											; DIFFUSE										\n\
																													\n\
	; Declare texture coordinate registers																			\n\
	dcl					t0.xyzw																						\n\
	dcl					t1.xyzw																						\n\
																													\n\
	; Alpha test against diffuse texture's alpha channel															\n\
	texld				r0,			t0,			s0					; Sample DIFFUSE into R0						\n\
	sub					r0,			r0.w,		c2.x																\n\
	texkill				r0																							\n\
																													\n\
	; Copy z into red channel																						\n\
	rcp					r0.x,		t1.w																			\n\
	mul					r1.xyzw,	t1.z,		r0.x																\n\
	mov					oC0,		r1																				\n\
	";

	// Assemble vertex shader
	m_vertexShader = RuD3DAssembleVertexShader(vsSource);

	// Assemble pixel shader
	m_pixelShader = RuD3DAssemblePixelShader(psSource);
}

CRuShader_SM20EZ_Static_DepthMap::~CRuShader_SM20EZ_Static_DepthMap()
{
}

void CRuShader_SM20EZ_Static_DepthMap::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Static_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants + 1);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
}

void CRuShader_SM20EZ_Static_DepthMap::EndPass(INT32 passNumber)
{
}

void CRuShader_SM20EZ_Static_DepthMap::SetParameters(const CRuParameterCache *paramCache)
{
	UINT64 param;

	CRuShader_SM20EZ_Static_Base::SetParameters(paramCache);

	if(paramCache->GetParameter(ruPARAMTYPE_TRANSFORM_SHADOW, param))
	{
		CRuMatrix4x4 *transform = (CRuMatrix4x4 *) param;

		for(UINT32 i = 0, k = 0; i < 4; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k + (26 * 4)] = transform->m_elements[j][i];
			}
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SHADOW_OBJECTID, param))
	{
		INT32 constantOffset = 30 * 4;

		m_shaderConstants[constantOffset++] = ((param / (35 * 35)) * 7) / 255.0f;
		m_shaderConstants[constantOffset++] = (((param % (35 * 35)) / 35) * 7) / 255.0f;
		m_shaderConstants[constantOffset++] = ((param % 35) * 7) / 255.0f;
		m_shaderConstants[constantOffset++] = 0.0f;
	}
}

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Static_Shadow::CRuShader_SM20EZ_Static_Shadow(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Static_Base(engine)
{
	m_override_Fog = TRUE;
	m_override_AlphaTest = FALSE;
	m_override_AlphaBlend = TRUE;

	char *vsSource = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_normal0			v1											; Declare normal								\n\
	dcl_texcoord0		v5											; Declare input texcoord 0						\n\
																													\n\
	; Transform position into homogeneous space																		\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	m4x3				r0.xyz,		v0,			c0					; Transform vertex to world space				\n\
	m4x3				r1.xyz,		r0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r1,			c6					; Transform vertex to homogeneous space			\n"
	"																												\n\
	; Distance factor																								\n\
	sub					r2.x,		c36.y,		c36.x				; Fog End - Fog Start into r2.x					\n\
	rcp					r2.x,		r2.x							; Reciprocal of fog range into r0.x				\n\
	sub					r3.x,		r1.z,		c36.x				; Offset vertex z by fog start into r3.x		\n\
	mul					r2.x,		r3.x,		r2.x				; Divide z by fog range into r0					\n\
	sub					r2.x,		c20.y,		r2.x				; Invert fog factor								\n\
	max					r2.x,		r2.x,		c20.w				; Clamp to [0, 1]								\n\
	min					r2.x,		r2.x,		c20.y																\n\
	sub					r3,			c20.y,		r2.x																\n\
																													\n\
	m4x4				oT0,		r0,			c26					; Transform vertex to trapezoidal space			\n\
	m4x4				oT1,		r0,			c32					; Transform vertex to light homogeneous space	\n\
																													\n\
	; shadow factor																									\n\
	m3x3				r0.xyz,		v1,			c0					; Transform normal to world space				\n\
	dp3					r1.x,		r0.xyz,		c31.xyz				; Dot normal with shadow direction				\n\
	max					r1.x,		r1.x,		c20.w				; Clamp to [0, 1]								\n\
	min					r1.x,		r1.x,		c20.y																\n\
	sub					r1.x,		c20.y,		r1.x				; Invert the shadow factor						\n\
	max					r1.x,		r1.x,		c36.z				; Clamp to [0.5, 1]								\n\
	min					r1.x,		r1.x,		c20.y																\n\
	mov					oT2,		r1								; Output shadow factor							\n\
	mov					oT3,		r3								; Output distance factor						\n\
	mov					oT4,		v5								; Copy texture coordinates to output			\n\
	";

	char *psSource = "\
																													\n\
	ps_2_0																											\n\
																													\n\
	; Declare texture samplers																						\n\
	dcl_2d				s0											; DIFFUSE										\n\
	dcl_2d				s1											; SHADOW										\n\
																													\n\
	; Declare texture coordinate registers																			\n\
	dcl					t0.xyzw																						\n\
	dcl					t1.xyzw																						\n\
	dcl					t2.xyzw																						\n\
	dcl					t3.xyzw																						\n\
	dcl					t4.xyzw																						\n\
																													\n\
	dcl					v0.xyzw																						\n\
																													\n\
	def					c2,			1.0, 0.2, 0.001, 0.0005															\n\
																													\n\
	; Convert texture coordinates																					\n\
	rcp					r0,			t0.w							; Get 1/w										\n\
	mul					r0,			t0,			r0					; Divide by w									\n\
	mul					r4,			r0,			c0.y				; Scale t0 by 0.5								\n\
	add					r4,			r4,			c0.y				; Bias by 0.5									\n\
	add					r4.y,		c0.x,		-r4.y				; Invert y										\n\
																													\n\
	; Calculate light space position and replace z																	\n\
	rcp					r1,			t1.w							; Get 1/w										\n\
	mul					r2,			t1,			r1					; Divide by w									\n\
	mov					r0.z,		r2.z																			\n\
																													\n\
	mov					r5,			r4								; Texel (0.0, -1.0)								\n\
	sub					r5.y,		r5.y,		c2.z				; Offset uv										\n\
																													\n\
	mov					r6,			r4								; Texel (0.0, +1.0)								\n\
	add					r6.y,		r6.y,		c2.z				; Offset uv										\n\
																													\n\
	mov					r7,			r4								; Texel (-1.0, 0.0)								\n\
	sub					r7.x,		r7.x,		c2.z				; Offset uv										\n\
																													\n\
	mov					r8,			r4								; Texel (+1.0, 0.0)								\n\
	add					r8.x,		r8.x,		c2.z				; Offset uv										\n"
	"																												\n\
	; Sample center																									\n\
	texld				r4,			r4,			s1					; Sample SHADOW depth into r4					\n\
																													\n\
	; Sample top																									\n\
	texld				r5,			r5,			s1					; Sample SHADOW depth into R2					\n\
																													\n\
	; Sample bottom																									\n\
	texld				r6,			r6,			s1					; Sample SHADOW depth into R2					\n\
																													\n\
	; Sample left																									\n\
	texld				r7,			r7,			s1					; Sample SHADOW depth into R2					\n\
																													\n\
	; Sample right																									\n\
	texld				r8,			r8,			s1					; Sample SHADOW depth into R2					\n"
	"																												\n\
	; Process center																								\n\
	add					r4.x,		r4.x,		c2.w				; Offset shadow z a little						\n\
	sub					r3.z,		r4.x,		r0.z				; Shadow z - object light z						\n\
	cmp					r1.x,		r3.z,		c0.x,		t2.x	;												\n\
																													\n\
	; Process top																									\n\
	add					r5.x,		r5.x,		c2.w				; Offset shadow z a little						\n\
	sub					r3.z,		r5.x,		r0.z				; Shadow z - object light z						\n\
	cmp					r0.x,		r3.z,		c0.x,		t2.x	;												\n\
	mul					r1.x,		r1.x,		r0.x				; Accumulate shadow factor into R4				\n\
																													\n\
	; Process bottom																								\n\
	add					r6.x,		r6.x,		c2.w				; Offset shadow z a little						\n\
	sub					r3.z,		r6.x,		r0.z				; Shadow z - object light z						\n\
	cmp					r0.x,		r3.z,		c0.x,		t2.x	;												\n\
	mul					r1.x,		r1.x,		r0.x				; Accumulate shadow factor into R4				\n\
																													\n\
	; Process left																									\n\
	add					r7.x,		r7.x,		c2.w				; Offset shadow z a little						\n\
	sub					r3.z,		r7.x,		r0.z				; Shadow z - object light z						\n\
	cmp					r0.x,		r3.z,		c0.x,		t2.x	;												\n\
	mul					r1.x,		r1.x,		r0.x				; Accumulate shadow factor into R4				\n\
																													\n\
	; Process right																									\n\
	add					r8.x,		r8.x,		c2.w				; Offset shadow z a little						\n\
	sub					r3.z,		r8.x,		r0.z				; Shadow z - object light z						\n\
	cmp					r0.x,		r3.z,		c0.x,		t2.x	;												\n\
	mul					r1.x,		r1.x,		r0.x				; Accumulate shadow factor into R4				\n"
	"																												\n\
	texld				r2,			t4,			s0					; Sample DIFFUSE into r2						\n\
	add					r0,			r1.x,		t3.x				; Offset shadow factor by distance to camera	\n\
	add					r0,			r0,			c2.y				; Threshold shadow factor to remove artifacts	\n\
	mul					r0,			r0,			r0					; Square shadow factor to get darker shadows	\n\
	mov					r0.w,		r2.w							; Set alpha to be diffuse alpha					\n\
																													\n\
	mov					oC0,		r0								; Move result into output						\n\
	";
/*
	char *psSource = "\
																													\n\
	ps_2_0																											\n\
																													\n\
	; Declare texture samplers																						\n\
	dcl_2d				s0											; DIFFUSE										\n\
	dcl_2d				s1											; SHADOW										\n\
																													\n\
	def					c2,			0.01, 0.2, 0, 0.002																\n\
																													\n\
	; Declare texture coordinate registers																			\n\
	dcl					t0.xyzw																						\n\
	dcl					t1.xyzw																						\n\
	dcl					t2.xyzw																						\n\
																													\n\
	rcp					r0,			t1.w							; Get 1/w										\n\
	mul					r0,			t1,			r0					; Divide by w									\n\
	mul					r1,			r0,			c0.y				; Scale t1 by 0.5								\n\
	add					r1,			r1,			c0.y				; Bias by 0.5									\n\
	add					r1.y,		c0.x,		-r1.y				; Invert y										\n\
																													\n\
	; Sample center																									\n\
	texld				r2,			r1,			s1					; Sample SHADOW object ID into R2				\n\
																													\n\
	sub					r3,			r2,			t2					; Subtract object IDs							\n\
	abs					r3,			r3								; Get absolute value							\n\
	dp3					r3,			r3,			c0.x				; Add up difference								\n\
	sub					r3,			c2.x,		r3					; Negate										\n\
	cmp					r4,			r3.x,		c2.z,		c2.y	; Set shadow factor								\n\
																													\n\
	; Adjust r4																										\n\
	sub					r4.xyz,		c0.x,		r4																	\n\
	mov					r4.w,		c0.x																			\n\
																													\n\
	texld				r1,			t0,			s0					; Sample DIFFUSE into R1						\n\
	mul					r0,			r4,			r1					; 												\n\
																													\n\
	mov					oC0,		r0								; Move result into output						\n\
	";
*/
	// Assemble vertex shader
	m_vertexShader = RuD3DAssembleVertexShader(vsSource);

	// Assemble pixel shader
	m_pixelShader = RuD3DAssemblePixelShader(psSource);
}

CRuShader_SM20EZ_Static_Shadow::~CRuShader_SM20EZ_Static_Shadow()
{
}

void CRuShader_SM20EZ_Static_Shadow::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Static_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants + 10);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}
}

void CRuShader_SM20EZ_Static_Shadow::EndPass(INT32 passNumber)
{
}

void CRuShader_SM20EZ_Static_Shadow::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SM20EZ_Static_Base::SetParameters(paramCache);

	UINT64 param;

	if(paramCache->GetParameter(ruPARAMTYPE_TRANSFORM_SHADOW, param))
	{
		CRuMatrix4x4 *transform = (CRuMatrix4x4 *) param;

		for(UINT32 i = 0, k = 0; i < 4; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k + (26 * 4)] = transform->m_elements[j][i];
			}
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_TRANSFORM_SHADOW2, param))
	{
		CRuMatrix4x4 *transform = (CRuMatrix4x4 *) param;

		for(UINT32 i = 0, k = 0; i < 4; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k + (32 * 4)] = transform->m_elements[j][i];
			}
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SHADOW_OBJECTID, param))
	{
		INT32 constantOffset = 30 * 4;
/*
		m_shaderConstants[constantOffset++] = ((param / (35 * 35)) * 7 + 8) / 255.0f;
		m_shaderConstants[constantOffset++] = (((param % (35 * 35)) / 35) * 7 + 8) / 255.0f;
		m_shaderConstants[constantOffset++] = ((param % 35) * 7 + 8) / 255.0f;
		m_shaderConstants[constantOffset++] = 0.0f;
*/
		m_shaderConstants[constantOffset++] = ((param / (35 * 35)) * 7) / 255.0f;
		m_shaderConstants[constantOffset++] = (((param % (35 * 35)) / 35) * 7) / 255.0f;
		m_shaderConstants[constantOffset++] = ((param % 35) * 7) / 255.0f;
		m_shaderConstants[constantOffset++] = 0.0f;

	}

	if(paramCache->GetParameter(ruPARAMTYPE_SHADOW_DIR, param))
	{
		INT32 constantOffset = 31 * 4;

		CRuVector3 *shadowDir = (CRuVector3 *) param;

		m_shaderConstants[constantOffset++] = -shadowDir->x;
		m_shaderConstants[constantOffset++] = -shadowDir->y;
		m_shaderConstants[constantOffset++] = -shadowDir->z;
		m_shaderConstants[constantOffset++] = 0.0f;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SHADOW_FADE_START, param))
	{
		INT32 constantOffset = 36 * 4;

		m_shaderConstants[constantOffset] = RuCastINT64ToFloat(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SHADOW_FADE_END, param))
	{
		INT32 constantOffset = 36 * 4 + 1;

		m_shaderConstants[constantOffset] = RuCastINT64ToFloat(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SHADOW_STRENGTH, param))
	{
		INT32 constantOffset = 36 * 4 + 2;

		m_shaderConstants[constantOffset] = (1.0f - RuCastINT64ToFloat(param)) * 0.1f + 0.85f;
	}
}

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Terrain_Packed_DepthMap::CRuShader_SM20EZ_Terrain_Packed_DepthMap(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Terrain_Packed_Base(engine)
{
	m_override_Fog = TRUE;
	m_override_AlphaTest = FALSE;
	m_override_AlphaBlend = TRUE;

	char *vsSource = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_color			v2											; Declare diffuse (0xAAXXYYZZ)					\n\
																													\n\
	mov					r1,			c20.yyyy																		\n\
	m4x3				r1.xyz,		v0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		v0,			c26					; Transform vertex to trapezoidal space			\n\
																													\n\
	m4x4				oT0,		r1,			c6					; Transform vertex to homogeneous space			\n\
																													\n\
	mov					r0,			c30								; Copy object ID encoding to diffuse out		\n\
	mov					r0.w,		c20.y							; Copy object ID encoding to diffuse out		\n\
	mov					oD0,		r0								; Copy object ID encoding to diffuse out		\n\
	";

	char *psSource = "\
																													\n\
	ps_2_0																											\n\
																													\n\
	; Declare texture coordinate registers																			\n\
	dcl					t0.xyzw																						\n\
																													\n\
	; Copy z into red channel																						\n\
	rcp					r0.x,			t0.w																		\n\
	mul					r1.xyzw,		t0.z,		r0.x															\n\
	mov					oC0,			r1																			\n\
	";

	// Assemble vertex shader
	m_vertexShader = RuD3DAssembleVertexShader(vsSource);

	// Assemble pixel shader
	m_pixelShader = RuD3DAssemblePixelShader(psSource);
}

CRuShader_SM20EZ_Terrain_Packed_DepthMap::~CRuShader_SM20EZ_Terrain_Packed_DepthMap()
{
}

void CRuShader_SM20EZ_Terrain_Packed_DepthMap::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Terrain_Packed_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants + 1);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
}

void CRuShader_SM20EZ_Terrain_Packed_DepthMap::EndPass(INT32 passNumber)
{
}

void CRuShader_SM20EZ_Terrain_Packed_DepthMap::SetParameters(const CRuParameterCache *paramCache)
{
	UINT64 param;

	CRuShader_SM20EZ_Terrain_Packed_Base::SetParameters(paramCache);

	if(paramCache->GetParameter(ruPARAMTYPE_TRANSFORM_SHADOW, param))
	{
		CRuMatrix4x4 *transform = (CRuMatrix4x4 *) param;

		for(UINT32 i = 0, k = 0; i < 4; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k + (26 * 4)] = transform->m_elements[j][i];
			}
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SHADOW_OBJECTID, param))
	{
		INT32 constantOffset = 30 * 4;

		m_shaderConstants[constantOffset++] = ((param / (35 * 35)) * 7) / 255.0f;
		m_shaderConstants[constantOffset++] = (((param % (35 * 35)) / 35) * 7) / 255.0f;
		m_shaderConstants[constantOffset++] = ((param % 35) * 7) / 255.0f;
		m_shaderConstants[constantOffset++] = 0.0f;
	}
}

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Terrain_Packed_Shadow::CRuShader_SM20EZ_Terrain_Packed_Shadow(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Terrain_Packed_Base(engine)
{
	m_override_Fog = TRUE;
	m_override_AlphaTest = FALSE;
	m_override_AlphaBlend = TRUE;

	char *vsSource = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_color			v2											; Declare diffuse (0xAAXXYYZZ)					\n\
																													\n\
	mov					r1,			c20.yyyy																		\n\
	m4x3				r1.xyz,		v0,			c3					; Transform vertex to view space				\n\
	m4x4				r3,			r1,			c6					; Transform vertex to homogeneous space			\n\
	mov					oPos,		r3								; Copy object ID								\n"
	"																												\n\
	; Distance factor																								\n\
	sub					r2.x,		c36.y,		c36.x				; Fog End - Fog Start into r2.x					\n\
	rcp					r2.x,		r2.x							; Reciprocal of fog range into r0.x				\n\
	sub					r3.x,		r1.z,		c36.x				; Offset vertex z by fog start into r3.x		\n\
	mul					r2.x,		r3.x,		r2.x				; Divide z by fog range into r0					\n\
	sub					r2.x,		c20.y,		r2.x				; Invert fog factor								\n\
	max					r2.x,		r2.x,		c20.w				; Clamp to [0, 1]								\n\
	min					r2.x,		r2.x,		c20.y																\n\
	sub					r3,			c20.y,		r2.x																\n\
																													\n\
	m4x4				oT0,		v0,			c26					; Transform vertex to trapezoidalspace			\n\
	m4x4				oT1,		v0,			c32					; Transform vertex to light homogeneous space	\n\
																													\n\
	; Unpack normal into r1																							\n\
	mov					r1,			v2								; Get packed data into r1						\n\
	sub					r1,			r1,			c20.z				; Bias by -0.5 to move into [-0.5, 0.5]			\n\
	rcp					r2,			c20.z							; Get 2.0 into r2								\n\
	mul					r1,			r1,			r2					; Multiply to move into [-1.0, 1.0]				\n\
																													\n\
	; shadow factor																									\n\
	dp3					r1.x,		r1.xyz,		c31.xyz				; Dot normal with shadow direction				\n\
																													\n\
	max					r1.x,		r1.x,		c20.w				; Clamp to [0, 1]								\n\
	min					r1.x,		r1.x,		c20.y																\n\
	sub					r1.x,		c20.y,		r1.x				; Invert the shadow factor						\n\
	max					r1.x,		r1.x,		c36.z				; Clamp to [0.5, 1]								\n\
	min					r1.x,		r1.x,		c20.y																\n\
	mov					oT2,		r1								; Output shadow factor							\n\
																													\n\
	mov					oT3,		r3								; Output distance factor						\n\
	";

	char *psSource = "\
																													\n\
	ps_2_0																											\n\
																													\n\
	; Declare texture samplers																						\n\
	dcl_2d				s0											; DIFFUSE										\n\
	dcl_2d				s1											; SHADOW										\n\
																													\n\
	; Declare texture coordinate registers																			\n\
	dcl					t0.xyzw																						\n\
	dcl					t1.xyzw																						\n\
	dcl					t2.xyzw																						\n\
	dcl					t3.xyzw																						\n\
																													\n\
	dcl					v0.xyzw																						\n\
																													\n\
	def					c2,			1.0, 0.2, 0.001, 0.0005															\n\
																													\n\
	; Convert texture coordinates																					\n\
	rcp					r0,			t0.w							; Get 1/w										\n\
	mul					r0,			t0,			r0					; Divide by w									\n\
	mul					r4,			r0,			c0.y				; Scale t1 by 0.5								\n\
	add					r4,			r4,			c0.y				; Bias by 0.5									\n\
	add					r4.y,		c0.x,		-r4.y				; Invert y										\n\
																													\n\
	; Calculate light space position and replace z																	\n\
	rcp					r1,			t1.w							; Get 1/w										\n\
	mul					r2,			t1,			r1					; Divide by w									\n\
	mov					r0.z,		r2.z																			\n\
																													\n\
	mov					r5,			r4								; Texel (0.0, -1.0)								\n\
	sub					r5.y,		r5.y,		c2.z				; Offset uv										\n\
																													\n\
	mov					r6,			r4								; Texel (0.0, +1.0)								\n\
	add					r6.y,		r6.y,		c2.z				; Offset uv										\n\
																													\n\
	mov					r7,			r4								; Texel (-1.0, 0.0)								\n\
	sub					r7.x,		r7.x,		c2.z				; Offset uv										\n\
																													\n\
	mov					r8,			r4								; Texel (+1.0, 0.0)								\n\
	add					r8.x,		r8.x,		c2.z				; Offset uv										\n"
	"																												\n\
	; Sample center																									\n\
	texld				r4,			r4,			s1					; Sample SHADOW depth into R2					\n\
																													\n\
	; Sample top																									\n\
	texld				r5,			r5,			s1					; Sample SHADOW depth into R2					\n\
																													\n\
	; Sample bottom																									\n\
	texld				r6,			r6,			s1					; Sample SHADOW depth into R2					\n\
																													\n\
	; Sample left																									\n\
	texld				r7,			r7,			s1					; Sample SHADOW depth into R2					\n\
																													\n\
	; Sample right																									\n\
	texld				r8,			r8,			s1					; Sample SHADOW depth into R2					\n"
	"																												\n\
	; Process center																								\n\
	add					r4.x,		r4.x,		c2.w				; Offset shadow z a little						\n\
	sub					r3.z,		r4.x,		r0.z				; Shadow z - object light z						\n\
	cmp					r1.x,		r3.z,		c0.x,		t2.x	;												\n\
																													\n\
	; Process top																									\n\
	add					r5.x,		r5.x,		c2.w				; Offset shadow z a little						\n\
	sub					r3.z,		r5.x,		r0.z				; Shadow z - object light z						\n\
	cmp					r0.x,		r3.z,		c0.x,		t2.x	;												\n\
	mul					r1.x,		r1.x,		r0.x				; Accumulate shadow factor into R4				\n\
																													\n\
	; Process bottom																								\n\
	add					r6.x,		r6.x,		c2.w				; Offset shadow z a little						\n\
	sub					r3.z,		r6.x,		r0.z				; Shadow z - object light z						\n\
	cmp					r0.x,		r3.z,		c0.x,		t2.x	;												\n\
	mul					r1.x,		r1.x,		r0.x				; Accumulate shadow factor into R4				\n\
																													\n\
	; Process left																									\n\
	add					r7.x,		r7.x,		c2.w				; Offset shadow z a little						\n\
	sub					r3.z,		r7.x,		r0.z				; Shadow z - object light z						\n\
	cmp					r0.x,		r3.z,		c0.x,		t2.x	;												\n\
	mul					r1.x,		r1.x,		r0.x				; Accumulate shadow factor into R4				\n\
																													\n\
	; Process right																									\n\
	add					r8.x,		r8.x,		c2.w				; Offset shadow z a little						\n\
	sub					r3.z,		r8.x,		r0.z				; Shadow z - object light z						\n\
	cmp					r0.x,		r3.z,		c0.x,		t2.x	;												\n\
	mul					r1.x,		r1.x,		r0.x				; Accumulate shadow factor into R4				\n"
	"																												\n\
	add					r0,			r1.x,		t3.x				; Offset shadow factor by distance to camera	\n\
	add					r0,			r0,			c2.y				; Threshold shadow factor to remove artifacts	\n\
	mul					r0,			r0,			r0					; Square shadow factor to get darker shadows	\n\
	mov					r0.w,		c2.x							; Set alpha to 1 (this isn't really right)		\n\
																													\n\
	mov					oC0,		r0								; Move result into output						\n\
	";

	// Assemble vertex shader
	m_vertexShader = RuD3DAssembleVertexShader(vsSource);

	// Assemble pixel shader
	m_pixelShader = RuD3DAssemblePixelShader(psSource);
}

CRuShader_SM20EZ_Terrain_Packed_Shadow::~CRuShader_SM20EZ_Terrain_Packed_Shadow()
{
}

void CRuShader_SM20EZ_Terrain_Packed_Shadow::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Terrain_Packed_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants + 10);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		m_engine->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		m_engine->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		m_engine->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_engine->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}
}

void CRuShader_SM20EZ_Terrain_Packed_Shadow::EndPass(INT32 passNumber)
{
}

void CRuShader_SM20EZ_Terrain_Packed_Shadow::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SM20EZ_Terrain_Packed_Base::SetParameters(paramCache);

	UINT64 param;

	if(paramCache->GetParameter(ruPARAMTYPE_TRANSFORM_SHADOW, param))
	{
		CRuMatrix4x4 *transform = (CRuMatrix4x4 *) param;

		for(UINT32 i = 0, k = 0; i < 4; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k + (26 * 4)] = transform->m_elements[j][i];
			}
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_TRANSFORM_SHADOW2, param))
	{
		CRuMatrix4x4 *transform = (CRuMatrix4x4 *) param;

		for(UINT32 i = 0, k = 0; i < 4; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k + (32 * 4)] = transform->m_elements[j][i];
			}
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SHADOW_OBJECTID, param))
	{
		INT32 constantOffset = 30 * 4;
		m_shaderConstants[constantOffset++] = ((param / (35 * 35)) * 7) / 255.0f;
		m_shaderConstants[constantOffset++] = (((param % (35 * 35)) / 35) * 7) / 255.0f;
		m_shaderConstants[constantOffset++] = ((param % 35) * 7) / 255.0f;
		m_shaderConstants[constantOffset++] = 0.0f;

	}

	if(paramCache->GetParameter(ruPARAMTYPE_SHADOW_DIR, param))
	{
		INT32 constantOffset = 31 * 4;

		CRuVector3 *shadowDir = (CRuVector3 *) param;

		m_shaderConstants[constantOffset++] = -shadowDir->x;
		m_shaderConstants[constantOffset++] = -shadowDir->y;
		m_shaderConstants[constantOffset++] = -shadowDir->z;
		m_shaderConstants[constantOffset++] = 0.0f;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SHADOW_FADE_START, param))
	{
		INT32 constantOffset = 36 * 4;

		m_shaderConstants[constantOffset] = RuCastINT64ToFloat(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SHADOW_FADE_END, param))
	{
		INT32 constantOffset = 36 * 4 + 1;

		m_shaderConstants[constantOffset] = RuCastINT64ToFloat(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SHADOW_STRENGTH, param))
	{
		INT32 constantOffset = 36 * 4 + 2;

		m_shaderConstants[constantOffset] = (1.0f - RuCastINT64ToFloat(param)) * 0.1f + 0.85f;
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
