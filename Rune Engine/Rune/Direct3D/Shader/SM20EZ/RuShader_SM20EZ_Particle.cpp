#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Particle.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Particle_Sprite::CRuShader_SM20EZ_Particle_Sprite(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Base(engine),
	m_orientation(ruAXIS_Z_POSITIVE),
	m_billboardType(ruBILLBOARDTYPE_UNCONSTRAINED)
{
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
	char *vsSource_X = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_color			v2											; Declare diffuse								\n\
																													\n\
	; Calculate index to instance data																				\n\
	mul					r2.x,		v2.w,		c32.x				; Scale diffuse alpha by 256 to get an integer	\n\
	mad					r2.x,		r2.x,		c32.y,		c32.z	; Calculate address index for row 0 in r2.x		\n\
	add					r2.y,		r2.x,		c20.y				; Calculate address index for row 1 in r2.y		\n\
																													\n\
	; Transform position into homogeneous space																		\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	mov					r3,			c20.yyyy						; Move 1 into r1								\n\
	mov					r4,			c20.wwww						; Move 1 into r1								\n\
	mov					r5,			c20.wwww						; Move 1 into r1								\n\
																													\n\
	; Calculate t factors																							\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
	mov					r7.y,		c[a0.x].z						; Move t1 into r7.y								\n\
	sub					r7.x,		c20.y,		r7.y				; Calculate t0 into r7.x						\n\
	mov					r4.y,		c[a0.x].w						; Move cos(phi) into r4.y						\n\
	mov					r5.z,		c[a0.x].w						; Move cos(phi) into r5.z						\n\
																													\n\
	; Calculate position																							\n\
	mov					a0.x,		r2.x							; Copy row 0 index to index register			\n\
	mov					r4.z,		-c[a0.x].w						; Move -sin(phi) into r4.z						\n\
	mov					r5.y,		c[a0.x].w						; Move sin(phi) into r5.y						\n\
	mul					r1.x,		c31.z,		r7.x				; Calculate scale								\n\
	mad					r1.x,		c31.w,		r7.y,		r1.x													\n\
	mul					r0.xyz,		v0,			r1.x				; Scale											\n\
																													\n\
	dp3					r1.y,		r0.xyz,		r4.xyz				; Rotate point about view space z-axis			\n\
	dp3					r1.z,		r0.xyz,		r5.xyz				; Rotate point about view space z-axis			\n\
	mov					r1.x,		r0.x																			\n\
																													\n\
	m3x3				r3,			r1,			c27					; Billboard transform							\n\
	add					r0.xyz,		r3,			c[a0.x]				; Translate to world space						\n\
	m4x3				r1,			r0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r1,			c6					; Transform vertex to homogeneous space			\n"
	"																												\n\
	; Fog factor																									\n\
	mov					oFog,		c20.y							; Output fog factor								\n"
	"																												\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
																													\n\
	; Diffuse color																									\n\
	mov					r1.x,		c[a0.x].z						; Move t (end color factor) into r1.x			\n\
	mul					r0,			c35,		r1.x				; Blend end color								\n\
	sub					r1.x,		c20.y,		r1.x				; Invert t to get start color factor into r1.x	\n\
	mad					r4,			c34,		r1.x,		r0		; Blend start color								\n\
	mul					r4,			r4,			c26					; Multiply diffuse by global diffuse			\n\
	mov					oD0,		r4								; Move output									\n\
																													\n\
	; Calculate texture coordinates																					\n\
	mov					r3.xy,		c31.xy							; Move uv increments into r3					\n\
	mul					r1.xy,		c[a0.x].xy,	r3.xy				; Calculate subtexture uv origin				\n\
	mad					r0.xy,		v2.xy,		r3.xy,		r1.xy													\n\
	mov					oT0,		r0																				\n\
	";

	char *vsSource_Y = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_color			v2											; Declare diffuse								\n\
																													\n\
	; Calculate index to instance data																				\n\
	mul					r2.x,		v2.w,		c32.x				; Scale diffuse alpha by 256 to get an integer	\n\
	mad					r2.x,		r2.x,		c32.y,		c32.z	; Calculate address index for row 0 in r2.x		\n\
	add					r2.y,		r2.x,		c20.y				; Calculate address index for row 1 in r2.y		\n\
																													\n\
	; Transform position into homogeneous space																		\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	mov					r3,			c20.yyyy						; Move 1 into r1								\n\
	mov					r4,			c20.wwww						; Move 1 into r1								\n\
	mov					r5,			c20.wwww						; Move 1 into r1								\n\
																													\n\
	; Calculate t factors																							\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
	mov					r7.y,		c[a0.x].z						; Move t1 into r7.y								\n\
	sub					r7.x,		c20.y,		r7.y				; Calculate t0 into r7.x						\n\
	mov					r4.x,		c[a0.x].w						; Move cos(phi) into r4.x						\n\
	mov					r5.z,		c[a0.x].w						; Move cos(phi) into r5.y						\n\
																													\n\
	; Calculate position																							\n\
	mov					a0.x,		r2.x							; Copy row 0 index to index register			\n\
	mov					r4.z,		c[a0.x].w						; Move sin(phi) into r4.z						\n\
	mov					r5.x,		-c[a0.x].w						; Move -sin(phi) into r5.x						\n\
	mul					r1.x,		c31.z,		r7.x				; Calculate scale								\n\
	mad					r1.x,		c31.w,		r7.y,		r1.x													\n\
	mul					r0.xyz,		v0,			r1.x				; Scale											\n\
																													\n\
	dp3					r1.x,		r0.xyz,		r4.xyz				; Rotate point about view space z-axis			\n\
	dp3					r1.z,		r0.xyz,		r5.xyz				; Rotate point about view space z-axis			\n\
	mov					r1.y,		r0.y																			\n\
																													\n\
	m3x3				r3,			r1,			c27					; Billboard transform							\n\
	add					r0.xyz,		r3,			c[a0.x]				; Translate to world space						\n\
	m4x3				r1,			r0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r1,			c6					; Transform vertex to homogeneous space			\n"
	"																												\n\
	; Fog factor																									\n\
	mov					oFog,		c20.y							; Output fog factor								\n"
	"																												\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
																													\n\
	; Diffuse color																									\n\
	mov					r1.x,		c[a0.x].z						; Move t (end color factor) into r1.x			\n\
	mul					r0,			c35,		r1.x				; Blend end color								\n\
	sub					r1.x,		c20.y,		r1.x				; Invert t to get start color factor into r1.x	\n\
	mad					r4,			c34,		r1.x,		r0		; Blend start color								\n\
	mul					r4,			r4,			c26					; Multiply diffuse by global diffuse			\n\
	mov					oD0,		r4								; Move output									\n\
																													\n\
	; Calculate texture coordinates																					\n\
	mov					r3.xy,		c31.xy							; Move uv increments into r3					\n\
	mul					r1.xy,		c[a0.x].xy,	r3.xy				; Calculate subtexture uv origin				\n\
	mad					r0.xy,		v2.xy,		r3.xy,		r1.xy													\n\
	mov					oT0,		r0																				\n\
	";

	char *vsSource_Z = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_color			v2											; Declare diffuse								\n\
																													\n\
	; Calculate index to instance data																				\n\
	mul					r2.x,		v2.w,		c32.x				; Scale diffuse alpha by 256 to get an integer	\n\
	mad					r2.x,		r2.x,		c32.y,		c32.z	; Calculate address index for row 0 in r2.x		\n\
	add					r2.y,		r2.x,		c20.y				; Calculate address index for row 1 in r2.y		\n\
																													\n\
	; Transform position into homogeneous space																		\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	mov					r3,			c20.yyyy						; Move 1 into r1								\n\
	mov					r4,			c20.wwww						; Move 1 into r1								\n\
	mov					r5,			c20.wwww						; Move 1 into r1								\n\
																													\n\
	; Calculate t factors																							\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
	mov					r7.y,		c[a0.x].z						; Move t1 into r7.y								\n\
	sub					r7.x,		c20.y,		r7.y				; Calculate t0 into r7.x						\n\
	mov					r4.x,		c[a0.x].w						; Move cos(phi) into r4.x						\n\
	mov					r5.y,		c[a0.x].w						; Move cos(phi) into r5.y						\n\
																													\n\
	; Calculate position																							\n\
	mov					a0.x,		r2.x							; Copy row 0 index to index register			\n\
	mov					r4.y,		-c[a0.x].w						; Move -sin(phi) into r4.y						\n\
	mov					r5.x,		c[a0.x].w						; Move sin(phi) into r5.x						\n\
	mul					r1.x,		c31.z,		r7.x				; Calculate scale								\n\
	mad					r1.x,		c31.w,		r7.y,		r1.x													\n\
	mul					r0.xyz,		v0,			r1.x				; Scale											\n\
																													\n\
	dp3					r1.x,		r0.xyz,		r4.xyz				; Rotate point about view space z-axis			\n\
	dp3					r1.y,		r0.xyz,		r5.xyz				; Rotate point about view space z-axis			\n\
	mov					r1.z,		r0.z																			\n\
																													\n\
	m3x3				r3,			r1,			c27					; Billboard transform							\n\
	add					r0.xyz,		r3,			c[a0.x]				; Translate to world space						\n\
	m4x3				r1,			r0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r1,			c6					; Transform vertex to homogeneous space			\n"
	"																												\n\
	; Fog factor																									\n\
	mov					oFog,		c20.y							; Output fog factor								\n"
	"																												\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
																													\n\
	; Diffuse color																									\n\
	mov					r1.x,		c[a0.x].z						; Move t (end color factor) into r1.x			\n\
	mul					r0,			c35,		r1.x				; Blend end color								\n\
	sub					r1.x,		c20.y,		r1.x				; Invert t to get start color factor into r1.x	\n\
	mad					r4,			c34,		r1.x,		r0		; Blend start color								\n\
	mul					r4,			r4,			c26					; Multiply diffuse by global diffuse			\n\
	mov					oD0,		r4								; Move output									\n\
																													\n\
	; Calculate texture coordinates																					\n\
	mov					r3.xy,		c31.xy							; Move uv increments into r3					\n\
	mul					r1.xy,		c[a0.x].xy,	r3.xy				; Calculate subtexture uv origin				\n\
	mad					r0.xy,		v2.xy,		r3.xy,		r1.xy													\n\
	mov					oT0,		r0																				\n\
	";

	// Assemble vertex shader
	m_vertexShaders[0] = RuD3DAssembleVertexShader(vsSource_X);
	m_vertexShaders[1] = RuD3DAssembleVertexShader(vsSource_Y);
	m_vertexShaders[2] = RuD3DAssembleVertexShader(vsSource_Z);

	// Overrides
	m_override_ZWrite = TRUE;
}

CRuShader_SM20EZ_Particle_Sprite::~CRuShader_SM20EZ_Particle_Sprite()
{
	for(INT32 i = 0; i < 3; ++i)
	{
		ruSAFE_RELEASE(m_vertexShaders[i]);
	}
}

void CRuShader_SM20EZ_Particle_Sprite::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();

		switch(m_orientation)
		{
			case ruAXIS_X_POSITIVE:
				m_engine->SetVertexShader(m_vertexShaders[0]);
				break;
			case ruAXIS_Y_POSITIVE:
				m_engine->SetVertexShader(m_vertexShaders[1]);
				break;
			default:
			case ruAXIS_Z_POSITIVE:
				m_engine->SetVertexShader(m_vertexShaders[2]);
				break;
		}

		m_engine->SetPixelShader(NULL);
//		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, 256);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		switch(m_material_AlphaMode)
		{
			case ruALPHAMODE_NORMAL:
				{
					m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				}

				break;
			case ruALPHAMODE_ADD:
				{
					m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				}

				break;
			case ruALPHAMODE_SUBTRACT:
				{
					m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
					m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				}

				break;
		}

		m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
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

		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}
}

void CRuShader_SM20EZ_Particle_Sprite::EndPass(INT32 passNumber)
{
	CRuShader_SM20EZ_Base::EndPass(passNumber);

	m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CRuShader_SM20EZ_Particle_Sprite::SetParameters(const CRuParameterCache *paramCache)
{
	UINT64 param;

	CRuShader_SM20EZ_Base::SetParameters(paramCache);

	if(paramCache->GetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, param))
	{
		m_numInstances = (UINT32) param;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_GEOINSTANCE_S_DATA, param))
	{
		float *instanceData = reinterpret_cast<float *>(param);

		// Number of constants = instance dataset + 24 constants for the per-draw data
		INT32 numConstants = 24 + m_numInstances * 8;
		INT32 constantOffset = 31 * 4;

		RuMemcpy(&m_shaderConstants[constantOffset], instanceData, sizeof(float) * numConstants);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_DWORD_0, param))
	{
		m_orientation = static_cast<RuOrthogonalAxis>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_FX_BILLBOARD, param))
	{
		m_billboardType = static_cast<RuBillboardType>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_TRANSFORM_VIEW, param))
	{
		// Setup billboard transform
		CRuMatrix4x4 billboardTransform;

		switch(m_billboardType)
		{
			default:
			case ruBILLBOARDTYPE_NONE:
				billboardTransform.SetToIdentity();
				break;

			case ruBILLBOARDTYPE_UNCONSTRAINED:
				billboardTransform.SetToBillboard(*((CRuMatrix4x4 *) param), CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 1.0f, 0.0f));
				break;

			case ruBILLBOARDTYPE_AXIAL:
				billboardTransform.SetToAxialBillboard(*((CRuMatrix4x4 *) param), CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 1.0f, 0.0f));
				break;
		}

		// Copy billboard transform to shader constants
		INT32 uvTransformOffset = 27 * 4;

		for(UINT32 i = 0, k = 0; i < 4; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k + uvTransformOffset] = billboardTransform.m_elements[j][i];
			}
		}
	}
}

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Particle_SpriteEx::CRuShader_SM20EZ_Particle_SpriteEx(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Base(engine),
	m_orientation(ruAXIS_Z_POSITIVE),
	m_billboardType(ruBILLBOARDTYPE_UNCONSTRAINED),
	m_billboardAxis(ruAXIS_Y_POSITIVE)
{
	// Overrides
	m_override_Fog = FALSE;
	m_override_ZWrite = TRUE;

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
	char *vsSource_X0 = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_color			v2											; Declare diffuse								\n\
																													\n\
	; Calculate index to instance data																				\n\
	mul					r2.x,		v2.w,		c32.x				; Scale diffuse alpha by 256 to get an integer	\n\
	mad					r2.x,		r2.x,		c32.y,		c32.z	; Calculate address index for row 0 in r2.x		\n\
	add					r2.y,		r2.x,		c20.y				; Calculate address index for row 1 in r2.y		\n\
	add					r2.z,		r2.y,		c20.y				; Calculate address index for row 2 in r2.z		\n\
																													\n\
	; Transform position into homogeneous space																		\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	mov					r3,			c20.yyyy						; Move 1 into r1								\n\
	mov					r4,			c20.wwww						; Move 1 into r1								\n\
	mov					r5,			c20.wwww						; Move 1 into r1								\n\
																													\n\
	; Calculate t factors																							\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
	mov					r4.y,		c[a0.x].w						; Move cos(phi) into r4.y						\n\
	mov					r4.z,		-c[a0.x].z						; Move -sin(phi) into r4.z						\n\
	mov					r5.y,		c[a0.x].z						; Move sin(phi) into r5.y						\n\
	mov					r5.z,		c[a0.x].w						; Move cos(phi) into r5.z						\n\
																													\n\
	; Calculate position																							\n\
	mov					a0.x,		r2.x							; Copy row 0 index to index register			\n\
	mul					r0.xyz,		v0,			c[a0.x].w			; Scale											\n\
																													\n\
	mov					r1.x,		r0.x																			\n\
	dp3					r1.y,		r0.xyz,		r4.xyz				; Rotate point about view space z-axis			\n\
	dp3					r1.z,		r0.xyz,		r5.xyz				; Rotate point about view space z-axis			\n\
																													\n\
	m3x3				r3,			r1,			c27					; Billboard transform							\n\
	mov					r4.xyz,		c[a0.x]							; Move particle position into r4				\n\
	mov					r4.w,		c20.y							; Move 1 into r4.w								\n\
	m4x3				r0.xyz,		r4,			c0					; Transform particle position to world space	\n\
	add					r0.xyz,		r3,			r0					; Translate particle vertex to world space		\n\
	m4x3				r1,			r0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r1,			c6					; Transform vertex to homogeneous space			\n\
																													\n\
	; Fog factor																									\n\
	sub					r3.x,		c25.z,		c25.y				; Fog End - Fog Start into r3.x					\n\
	rcp					r3.x,		r3.x							; Reciprocal of fog range into r0.x				\n\
	sub					r4.x,		r1.z,		c25.y				; Offset vertex z by fog start into r3.x		\n\
	mul					r3.x,		r4.x,		r3.x				; Divide z by fog range into r0					\n\
	max					r3.x,		r3.x,		c20.w				; Clamp to [0, 1]								\n\
	min					r3.x,		r3.x,		c20.y																\n\
	add					r3.x,		r3.x,		c25.x				; Add constant fog term							\n\
	max					r3.x,		r3.x,		c20.w				; Clamp to [0, 1]								\n\
	min					r3.x,		r3.x,		c20.y																\n\
	sub					r3.x,		c20.y,		r3.x				; Invert fog factor								\n\
	mov					oFog,		r3.x							; Output fog factor								\n\
	; Fog factor																									\n\
	;mov					oFog,		c20.y							; Output fog factor								\n\
																													\n\
	; Diffuse color																									\n\
	mov					a0.x,		r2.z							; Copy row 2 index to index register			\n\
	mov					r4,			c[a0.x]							; Move particle color into r4					\n\
	mul					r4,			r4,			c26					; Multiply diffuse by global diffuse			\n\
	mov					oD0,		r4								; Move output									\n\
																													\n\
	; Calculate texture coordinates																					\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
	mov					r3.xy,		c31.xy							; Move uv increments into r3					\n\
	mul					r1.xy,		c[a0.x].xy,	r3.xy				; Calculate subtexture uv origin				\n\
	mad					r0.xy,		v2.xy,		r3.xy,		r1.xy													\n\
	mov					oT0,		r0																				\n\
	";

	char *vsSource_X1 = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_color			v2											; Declare diffuse								\n\
																													\n\
	; Calculate index to instance data																				\n\
	mul					r2.x,		v2.w,		c32.x				; Scale diffuse alpha by 256 to get an integer	\n\
	mad					r2.x,		r2.x,		c32.y,		c32.z	; Calculate address index for row 0 in r2.x		\n\
	add					r2.y,		r2.x,		c20.y				; Calculate address index for row 1 in r2.y		\n\
	add					r2.z,		r2.y,		c20.y				; Calculate address index for row 2 in r2.z		\n\
																													\n\
	; Transform position into homogeneous space																		\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	mov					r3,			c20.yyyy						; Move 1 into r1								\n\
	mov					r4,			c20.wwww						; Move 1 into r1								\n\
	mov					r5,			c20.wwww						; Move 1 into r1								\n\
																													\n\
	; Calculate t factors																							\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
	mov					r4.y,		c[a0.x].w						; Move cos(phi) into r4.y						\n\
	mov					r4.z,		-c[a0.x].z						; Move -sin(phi) into r4.z						\n\
	mov					r5.y,		c[a0.x].z						; Move sin(phi) into r5.y						\n\
	mov					r5.z,		c[a0.x].w						; Move cos(phi) into r5.z						\n\
																													\n\
	; Calculate position																							\n\
	mov					a0.x,		r2.x							; Copy row 0 index to index register			\n\
	mul					r0.xyz,		v0,			c[a0.x].w			; Scale											\n\
																													\n\
	mov					r1.x,		r0.x																			\n\
	dp3					r1.y,		r0.xyz,		r4.xyz				; Rotate point about view space z-axis			\n\
	dp3					r1.z,		r0.xyz,		r5.xyz				; Rotate point about view space z-axis			\n\
																													\n\
	m3x3				r4,			r1,			c0					; World transform without translation			\n\
	m3x3				r3,			r4,			c27					; Billboard transform							\n\
	mov					r4.xyz,		c[a0.x]							; Move particle position into r4				\n\
	mov					r4.w,		c20.y							; Move 1 into r4.w								\n\
	m4x3				r0.xyz,		r4,			c0					; Transform particle position to world space	\n\
	add					r0.xyz,		r3,			r0					; Translate particle vertex to world space		\n\
	m4x3				r1,			r0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r1,			c6					; Transform vertex to homogeneous space			\n\
																													\n\
	; Fog factor																									\n\
	sub					r3.x,		c25.z,		c25.y				; Fog End - Fog Start into r3.x					\n\
	rcp					r3.x,		r3.x							; Reciprocal of fog range into r0.x				\n\
	sub					r4.x,		r1.z,		c25.y				; Offset vertex z by fog start into r3.x		\n\
	mul					r3.x,		r4.x,		r3.x				; Divide z by fog range into r0					\n\
	max					r3.x,		r3.x,		c20.w				; Clamp to [0, 1]								\n\
	min					r3.x,		r3.x,		c20.y																\n\
	add					r3.x,		r3.x,		c25.x				; Add constant fog term							\n\
	max					r3.x,		r3.x,		c20.w				; Clamp to [0, 1]								\n\
	min					r3.x,		r3.x,		c20.y																\n\
	sub					r3.x,		c20.y,		r3.x				; Invert fog factor								\n\
	mov					oFog,		r3.x							; Output fog factor								\n\
	; Fog factor																									\n\
	;mov					oFog,		c20.y							; Output fog factor								\n\
																													\n\
	; Diffuse color																									\n\
	mov					a0.x,		r2.z							; Copy row 2 index to index register			\n\
	mov					r4,			c[a0.x]							; Move particle color into r4					\n\
	mul					r4,			r4,			c26					; Multiply diffuse by global diffuse			\n\
	mov					oD0,		r4								; Move output									\n\
																													\n\
	; Calculate texture coordinates																					\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
	mov					r3.xy,		c31.xy							; Move uv increments into r3					\n\
	mul					r1.xy,		c[a0.x].xy,	r3.xy				; Calculate subtexture uv origin				\n\
	mad					r0.xy,		v2.xy,		r3.xy,		r1.xy													\n\
	mov					oT0,		r0																				\n\
	";

	char *vsSource_Y0 = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_color			v2											; Declare diffuse								\n\
																													\n\
	; Calculate index to instance data																				\n\
	mul					r2.x,		v2.w,		c32.x				; Scale diffuse alpha by 256 to get an integer	\n\
	mad					r2.x,		r2.x,		c32.y,		c32.z	; Calculate address index for row 0 in r2.x		\n\
	add					r2.y,		r2.x,		c20.y				; Calculate address index for row 1 in r2.y		\n\
	add					r2.z,		r2.y,		c20.y				; Calculate address index for row 2 in r2.z		\n\
																													\n\
	; Transform position into homogeneous space																		\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	mov					r3,			c20.yyyy						; Move 1 into r1								\n\
	mov					r4,			c20.wwww						; Move 1 into r1								\n\
	mov					r5,			c20.wwww						; Move 1 into r1								\n\
																													\n\
	; Calculate t factors																							\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
	mov					r4.x,		c[a0.x].w						; Move cos(phi) into r4.x						\n\
	mov					r4.z,		c[a0.x].z						; Move sin(phi) into r4.z						\n\
	mov					r5.z,		c[a0.x].w						; Move cos(phi) into r5.y						\n\
	mov					r5.x,		-c[a0.x].z						; Move -sin(phi) into r5.x						\n\
																													\n\
	; Calculate position																							\n\
	mov					a0.x,		r2.x							; Copy row 0 index to index register			\n\
	mul					r0.xyz,		v0,			c[a0.x].w			; Scale											\n\
																													\n\
	dp3					r1.x,		r0.xyz,		r4.xyz				; Rotate point about view space z-axis			\n\
	mov					r1.y,		r0.y																			\n\
	dp3					r1.z,		r0.xyz,		r5.xyz				; Rotate point about view space z-axis			\n\
																													\n\
	m3x3				r3,			r1,			c27					; Billboard transform							\n\
	mov					r4.xyz,		c[a0.x]							; Move particle position into r4				\n\
	mov					r4.w,		c20.y							; Move 1 into r4.w								\n\
	m4x3				r0.xyz,		r4,			c0					; Transform particle position to world space	\n\
	add					r0.xyz,		r3,			r0					; Translate particle vertex to world space		\n\
	m4x3				r1,			r0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r1,			c6					; Transform vertex to homogeneous space			\n\
																													\n\
	; Fog factor																									\n\
	sub					r3.x,		c25.z,		c25.y				; Fog End - Fog Start into r3.x					\n\
	rcp					r3.x,		r3.x							; Reciprocal of fog range into r0.x				\n\
	sub					r4.x,		r1.z,		c25.y				; Offset vertex z by fog start into r3.x		\n\
	mul					r3.x,		r4.x,		r3.x				; Divide z by fog range into r0					\n\
	max					r3.x,		r3.x,		c20.w				; Clamp to [0, 1]								\n\
	min					r3.x,		r3.x,		c20.y																\n\
	add					r3.x,		r3.x,		c25.x				; Add constant fog term							\n\
	max					r3.x,		r3.x,		c20.w				; Clamp to [0, 1]								\n\
	min					r3.x,		r3.x,		c20.y																\n\
	sub					r3.x,		c20.y,		r3.x				; Invert fog factor								\n\
	mov					oFog,		r3.x							; Output fog factor								\n\
	; Fog factor																									\n\
	;mov					oFog,		c20.y							; Output fog factor								\n\
																													\n\
	; Diffuse color																									\n\
	mov					a0.x,		r2.z							; Copy row 2 index to index register			\n\
	mov					r4,			c[a0.x]							; Move particle color into r4					\n\
	mul					r4,			r4,			c26					; Multiply diffuse by global diffuse			\n\
	mov					oD0,		r4								; Move output									\n\
																													\n\
	; Calculate texture coordinates																					\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
	mov					r3.xy,		c31.xy							; Move uv increments into r3					\n\
	mul					r1.xy,		c[a0.x].xy,	r3.xy				; Calculate subtexture uv origin				\n\
	mad					r0.xy,		v2.xy,		r3.xy,		r1.xy													\n\
	mov					oT0,		r0																				\n\
	";

	char *vsSource_Y1 = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_color			v2											; Declare diffuse								\n\
																													\n\
	; Calculate index to instance data																				\n\
	mul					r2.x,		v2.w,		c32.x				; Scale diffuse alpha by 256 to get an integer	\n\
	mad					r2.x,		r2.x,		c32.y,		c32.z	; Calculate address index for row 0 in r2.x		\n\
	add					r2.y,		r2.x,		c20.y				; Calculate address index for row 1 in r2.y		\n\
	add					r2.z,		r2.y,		c20.y				; Calculate address index for row 2 in r2.z		\n\
																													\n\
	; Transform position into homogeneous space																		\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	mov					r3,			c20.yyyy						; Move 1 into r1								\n\
	mov					r4,			c20.wwww						; Move 1 into r1								\n\
	mov					r5,			c20.wwww						; Move 1 into r1								\n\
																													\n\
	; Calculate t factors																							\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
	mov					r4.x,		c[a0.x].w						; Move cos(phi) into r4.x						\n\
	mov					r4.z,		c[a0.x].z						; Move sin(phi) into r4.z						\n\
	mov					r5.z,		c[a0.x].w						; Move cos(phi) into r5.y						\n\
	mov					r5.x,		-c[a0.x].z						; Move -sin(phi) into r5.x						\n\
																													\n\
	; Calculate position																							\n\
	mov					a0.x,		r2.x							; Copy row 0 index to index register			\n\
	mul					r0.xyz,		v0,			c[a0.x].w			; Scale											\n\
																													\n\
	dp3					r1.x,		r0.xyz,		r4.xyz				; Rotate point about view space z-axis			\n\
	mov					r1.y,		r0.y																			\n\
	dp3					r1.z,		r0.xyz,		r5.xyz				; Rotate point about view space z-axis			\n\
																													\n\
	m3x3				r4,			r1,			c0					; World transform without translation			\n\
	m3x3				r3,			r4,			c27					; Billboard transform							\n\
	mov					r4.xyz,		c[a0.x]							; Move particle position into r4				\n\
	mov					r4.w,		c20.y							; Move 1 into r4.w								\n\
	m4x3				r0.xyz,		r4,			c0					; Transform particle position to world space	\n\
	add					r0.xyz,		r3,			r0					; Translate particle vertex to world space		\n\
	m4x3				r1,			r0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r1,			c6					; Transform vertex to homogeneous space			\n\
																													\n\
	; Fog factor																									\n\
	sub					r3.x,		c25.z,		c25.y				; Fog End - Fog Start into r3.x					\n\
	rcp					r3.x,		r3.x							; Reciprocal of fog range into r0.x				\n\
	sub					r4.x,		r1.z,		c25.y				; Offset vertex z by fog start into r3.x		\n\
	mul					r3.x,		r4.x,		r3.x				; Divide z by fog range into r0					\n\
	max					r3.x,		r3.x,		c20.w				; Clamp to [0, 1]								\n\
	min					r3.x,		r3.x,		c20.y																\n\
	add					r3.x,		r3.x,		c25.x				; Add constant fog term							\n\
	max					r3.x,		r3.x,		c20.w				; Clamp to [0, 1]								\n\
	min					r3.x,		r3.x,		c20.y																\n\
	sub					r3.x,		c20.y,		r3.x				; Invert fog factor								\n\
	mov					oFog,		r3.x							; Output fog factor								\n\
	; Fog factor																									\n\
	;mov					oFog,		c20.y							; Output fog factor								\n\
																													\n\
	; Diffuse color																									\n\
	mov					a0.x,		r2.z							; Copy row 2 index to index register			\n\
	mov					r4,			c[a0.x]							; Move particle color into r4					\n\
	mul					r4,			r4,			c26					; Multiply diffuse by global diffuse			\n\
	mov					oD0,		r4								; Move output									\n\
																													\n\
	; Calculate texture coordinates																					\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
	mov					r3.xy,		c31.xy							; Move uv increments into r3					\n\
	mul					r1.xy,		c[a0.x].xy,	r3.xy				; Calculate subtexture uv origin				\n\
	mad					r0.xy,		v2.xy,		r3.xy,		r1.xy													\n\
	mov					oT0,		r0																				\n\
	";

	char *vsSource_Z0 = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_color			v2											; Declare diffuse								\n\
																													\n\
	; Calculate index to instance data																				\n\
	mul					r2.x,		v2.w,		c32.x				; Scale diffuse alpha by 256 to get an integer	\n\
	mad					r2.x,		r2.x,		c32.y,		c32.z	; Calculate address index for row 0 in r2.x		\n\
	add					r2.y,		r2.x,		c20.y				; Calculate address index for row 1 in r2.y		\n\
	add					r2.z,		r2.y,		c20.y				; Calculate address index for row 2 in r2.z		\n\
																													\n\
	; Transform position into homogeneous space																		\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	mov					r3,			c20.yyyy						; Move 1 into r1								\n\
	mov					r4,			c20.wwww						; Move 1 into r1								\n\
	mov					r5,			c20.wwww						; Move 1 into r1								\n\
																													\n\
	; Calculate t factors																							\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
	mov					r4.x,		c[a0.x].w						; Move cos(phi) into r4.x						\n\
	mov					r4.y,		-c[a0.x].z						; Move -sin(phi) into r4.y						\n\
	mov					r5.x,		c[a0.x].z						; Move sin(phi) into r5.x						\n\
	mov					r5.y,		c[a0.x].w						; Move cos(phi) into r5.y						\n\
																													\n\
	; Calculate position																							\n\
	mov					a0.x,		r2.x							; Copy row 0 index to index register			\n\
	mul					r0.xyz,		v0,			c[a0.x].w			; Scale											\n\
																													\n\
	dp3					r1.x,		r0.xyz,		r4.xyz				; Rotate point about view space z-axis			\n\
	dp3					r1.y,		r0.xyz,		r5.xyz				; Rotate point about view space z-axis			\n\
	mov					r1.z,		r0.z																			\n\
																													\n\
	m3x3				r3,			r1,			c27					; Billboard transform							\n\
	mov					r4.xyz,		c[a0.x]							; Move particle position into r4				\n\
	mov					r4.w,		c20.y							; Move 1 into r4.w								\n\
	m4x3				r0.xyz,		r4,			c0					; Transform particle position to world space	\n\
	add					r0.xyz,		r3,			r0					; Translate particle vertex to world space		\n\
	m4x3				r1,			r0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r1,			c6					; Transform vertex to homogeneous space			\n\
																													\n\
	; Fog factor																									\n\
	sub					r3.x,		c25.z,		c25.y				; Fog End - Fog Start into r3.x					\n\
	rcp					r3.x,		r3.x							; Reciprocal of fog range into r0.x				\n\
	sub					r4.x,		r1.z,		c25.y				; Offset vertex z by fog start into r3.x		\n\
	mul					r3.x,		r4.x,		r3.x				; Divide z by fog range into r0					\n\
	max					r3.x,		r3.x,		c20.w				; Clamp to [0, 1]								\n\
	min					r3.x,		r3.x,		c20.y																\n\
	add					r3.x,		r3.x,		c25.x				; Add constant fog term							\n\
	max					r3.x,		r3.x,		c20.w				; Clamp to [0, 1]								\n\
	min					r3.x,		r3.x,		c20.y																\n\
	sub					r3.x,		c20.y,		r3.x				; Invert fog factor								\n\
	mov					oFog,		r3.x							; Output fog factor								\n\
	; Fog factor																									\n\
	;mov					oFog,		c20.y							; Output fog factor								\n\
																													\n\
	; Diffuse color																									\n\
	mov					a0.x,		r2.z							; Copy row 2 index to index register			\n\
	mov					r4,			c[a0.x]							; Move particle color into r4					\n\
	mul					r4,			r4,			c26					; Multiply diffuse by global diffuse			\n\
	mov					oD0,		r4								; Move output									\n\
																													\n\
	; Calculate texture coordinates																					\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
	mov					r3.xy,		c31.xy							; Move uv increments into r3					\n\
	mul					r1.xy,		c[a0.x].xy,	r3.xy				; Calculate subtexture uv origin				\n\
	mad					r0.xy,		v2.xy,		r3.xy,		r1.xy													\n\
	mov					oT0,		r0																				\n\
	";

	char *vsSource_Z1 = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_color			v2											; Declare diffuse								\n\
																													\n\
	; Calculate index to instance data																				\n\
	mul					r2.x,		v2.w,		c32.x				; Scale diffuse alpha by 256 to get an integer	\n\
	mad					r2.x,		r2.x,		c32.y,		c32.z	; Calculate address index for row 0 in r2.x		\n\
	add					r2.y,		r2.x,		c20.y				; Calculate address index for row 1 in r2.y		\n\
	add					r2.z,		r2.y,		c20.y				; Calculate address index for row 2 in r2.z		\n\
																													\n\
	; Transform position into homogeneous space																		\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	mov					r3,			c20.yyyy						; Move 1 into r1								\n\
	mov					r4,			c20.wwww						; Move 1 into r1								\n\
	mov					r5,			c20.wwww						; Move 1 into r1								\n\
																													\n\
	; Calculate t factors																							\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
	mov					r4.x,		c[a0.x].w						; Move cos(phi) into r4.x						\n\
	mov					r4.y,		-c[a0.x].z						; Move -sin(phi) into r4.y						\n\
	mov					r5.x,		c[a0.x].z						; Move sin(phi) into r5.x						\n\
	mov					r5.y,		c[a0.x].w						; Move cos(phi) into r5.y						\n\
																													\n\
	; Calculate position																							\n\
	mov					a0.x,		r2.x							; Copy row 0 index to index register			\n\
	mul					r0.xyz,		v0,			c[a0.x].w			; Scale											\n\
																													\n\
	dp3					r1.x,		r0.xyz,		r4.xyz				; Rotate point about view space z-axis			\n\
	dp3					r1.y,		r0.xyz,		r5.xyz				; Rotate point about view space z-axis			\n\
	mov					r1.z,		r0.z																			\n\
																													\n\
	m3x3				r4,			r1,			c0					; World transform without translation			\n\
	m3x3				r3,			r4,			c27					; Billboard transform							\n\
	mov					r4.xyz,		c[a0.x]							; Move particle position into r4				\n\
	mov					r4.w,		c20.y							; Move 1 into r4.w								\n\
	m4x3				r0.xyz,		r4,			c0					; Transform particle position to world space	\n\
	add					r0.xyz,		r3,			r0					; Translate particle vertex to world space		\n\
	m4x3				r1,			r0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r1,			c6					; Transform vertex to homogeneous space			\n\
																													\n\
	; Fog factor																									\n\
	sub					r3.x,		c25.z,		c25.y				; Fog End - Fog Start into r3.x					\n\
	rcp					r3.x,		r3.x							; Reciprocal of fog range into r0.x				\n\
	sub					r4.x,		r1.z,		c25.y				; Offset vertex z by fog start into r3.x		\n\
	mul					r3.x,		r4.x,		r3.x				; Divide z by fog range into r0					\n\
	max					r3.x,		r3.x,		c20.w				; Clamp to [0, 1]								\n\
	min					r3.x,		r3.x,		c20.y																\n\
	add					r3.x,		r3.x,		c25.x				; Add constant fog term							\n\
	max					r3.x,		r3.x,		c20.w				; Clamp to [0, 1]								\n\
	min					r3.x,		r3.x,		c20.y																\n\
	sub					r3.x,		c20.y,		r3.x				; Invert fog factor								\n\
	mov					oFog,		r3.x							; Output fog factor								\n\
	; Fog factor																									\n\
	;mov					oFog,		c20.y							; Output fog factor								\n\
																													\n\
	; Diffuse color																									\n\
	mov					a0.x,		r2.z							; Copy row 2 index to index register			\n\
	mov					r4,			c[a0.x]							; Move particle color into r4					\n\
	mul					r4,			r4,			c26					; Multiply diffuse by global diffuse			\n\
	mov					oD0,		r4								; Move output									\n\
																													\n\
	; Calculate texture coordinates																					\n\
	mov					a0.x,		r2.y							; Copy row 1 index to index register			\n\
	mov					r3.xy,		c31.xy							; Move uv increments into r3					\n\
	mul					r1.xy,		c[a0.x].xy,	r3.xy				; Calculate subtexture uv origin				\n\
	mad					r0.xy,		v2.xy,		r3.xy,		r1.xy													\n\
	mov					oT0,		r0																				\n\
	";

	// Assemble vertex shader
	m_vertexShaders[0][0] = RuD3DAssembleVertexShader(vsSource_X0);
	m_vertexShaders[0][1] = RuD3DAssembleVertexShader(vsSource_X1);
	m_vertexShaders[1][0] = RuD3DAssembleVertexShader(vsSource_Y0);
	m_vertexShaders[1][1] = RuD3DAssembleVertexShader(vsSource_Y1);
	m_vertexShaders[2][0] = RuD3DAssembleVertexShader(vsSource_Z0);
	m_vertexShaders[2][1] = RuD3DAssembleVertexShader(vsSource_Z1);
}

CRuShader_SM20EZ_Particle_SpriteEx::~CRuShader_SM20EZ_Particle_SpriteEx()
{
	for(INT32 i = 0; i < 3; ++i)
	{
		for(INT32 j = 0; j < 2; ++j)
		{
			ruSAFE_RELEASE(m_vertexShaders[i][j]);
		}
	}
}

void CRuShader_SM20EZ_Particle_SpriteEx::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();

		switch(m_orientation)
		{
			case ruAXIS_X_POSITIVE:
				m_engine->SetVertexShader(m_allowWorldRotation ? m_vertexShaders[0][1] : m_vertexShaders[0][0]);
				break;
			case ruAXIS_Y_POSITIVE:
				m_engine->SetVertexShader(m_allowWorldRotation ? m_vertexShaders[1][1] : m_vertexShaders[1][0]);
				break;
			default:
			case ruAXIS_Z_POSITIVE:
				m_engine->SetVertexShader(m_allowWorldRotation ? m_vertexShaders[2][1] : m_vertexShaders[2][0]);
				break;
		}

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
						switch(m_orientation)
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
						}

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
		INT32 uvTransformOffset = 27 * 4;

		for(UINT32 i = 0, k = 0; i < 4; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k + uvTransformOffset] = billboardTransform.m_elements[j][i];
			}
		}

		m_engine->SetPixelShader(NULL);
//		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, 256);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		switch(m_material_AlphaMode)
		{
			case ruALPHAMODE_NORMAL:
				{
					m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
					m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				}

				break;
			case ruALPHAMODE_ADD:
				{
					m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
					m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				}

				break;
			case ruALPHAMODE_SUBTRACT:
				{
					m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
					m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				}

				break;
		}

		m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
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

		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}
}

void CRuShader_SM20EZ_Particle_SpriteEx::EndPass(INT32 passNumber)
{
	CRuShader_SM20EZ_Base::EndPass(passNumber);

	m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CRuShader_SM20EZ_Particle_SpriteEx::SetParameters(const CRuParameterCache *paramCache)
{
	UINT64 param;

	CRuShader_SM20EZ_Base::SetParameters(paramCache);

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
	}

	if(paramCache->GetParameter(ruPARAMTYPE_DWORD_0, param))
	{
		m_orientation = static_cast<RuOrthogonalAxis>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_DWORD_1, param))
	{
		m_billboardAxis = static_cast<RuOrthogonalAxis>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_DWORD_2, param))
	{
		m_allowWorldRotation = static_cast<BOOL>(param);
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
