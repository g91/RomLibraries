#include "../../../Direct3D/Shader/SMUNI/RuShader_SMUNI_Editor.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SMUNI_Editor_Translucent::CRuShader_SMUNI_Editor_Translucent(CRuEngineD3D *engine)
:	CRuShader_SMUNI_Base(engine)
{
	m_override_AlphaBlend = TRUE;

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
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position 0							\n\
	dcl_color			v12											; Declare diffuse								\n\
																													\n\
	; Transform position into homogeneous space																		\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	m4x3				r0.xyz,		v0,			c0					; Transform vertex to world space				\n\
	m4x3				r1.xyz,		r0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r1,			c6					; Transform vertex to homogeneous space			\n\
																													\n\
	; Fog factor																									\n\
	mov					oFog,		c20.y							; Output 1.0 as fog factor						\n"
	"																												\n\
	; Diffuse color																									\n\
	mul					r4,			v12,		c26					; Multiply diffuse by global diffuse			\n\
	mov					oD0,		r4								; Set diffuse									\n\
	";

	char *psSource = "\
																													\n\
	ps_1_1																											\n\
																													\n\
	; Arithmetic ops																								\n\
	mov					r0,			v0								; Copy diffuse to output						\n\
	";

	// Assemble vertex shader
	m_vertexShader = RuD3DAssembleVertexShader(vsSource);

	// Assemble pixel shader
	m_pixelShader = RuD3DAssemblePixelShader(psSource);

	// Overrides
	m_override_ZWrite = TRUE;

	// Set constants count
	m_numBaseConstants = 27;
	m_numBasePixelShaderConstants = 1;
}

CRuShader_SMUNI_Editor_Translucent::~CRuShader_SMUNI_Editor_Translucent()
{
}

void CRuShader_SMUNI_Editor_Translucent::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SMUNI_Base::BeginPass(passNumber);

	// Setup SH9 + zero + 0.5 constants
	m_shaderConstants[80] = 3.0f;
	m_shaderConstants[81] = 1.0f;
	m_shaderConstants[82] = 0.5f;
	m_shaderConstants[83] = 0.0f;

	// Pixel shader basic constants
	m_pixelShaderConstants[0] = 1.0f;
	m_pixelShaderConstants[1] = 0.5f;
	m_pixelShaderConstants[2] = 0.0f;
	m_pixelShaderConstants[3] = 0.0f;

	// Set effective material diffuse
	INT32 globalDiffuseOffset = 104;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_r;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_g;
	m_shaderConstants[globalDiffuseOffset++] = m_material_Diffuse.m_b;

	// Set effective material alpha
	INT32 globalAlphaOffset = 107;
	m_shaderConstants[globalAlphaOffset] = m_material_EffectiveAlpha;

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		switch(m_material_AlphaMode)
		{
			case ruALPHAMODE_NORMAL:
				{
					m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
					m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				}

				break;

			case ruALPHAMODE_ADD:
				{
					m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
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

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
}

void CRuShader_SMUNI_Editor_Translucent::EndPass(INT32 passNumber)
{
}

void CRuShader_SMUNI_Editor_Translucent::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SMUNI_Base::SetParameters(paramCache);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
