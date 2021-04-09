#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Gizmo.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Gizmo_CodedLine::CRuShader_SM20EZ_Gizmo_CodedLine(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Static_Base(engine)
{
	// Setup vertex elements
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },
		{ 0,	12,	ruELEMENTTYPE_D3DCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	ruSAFE_RELEASE(m_vertexDecl);
	m_vertexDecl = m_engine->CreateVertexDeclaration(vertexElements);

	char *vsSource = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_color			v2											; Declare diffuse								\n\
																													\n\
	; Transform position into homogeneous space																		\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	m4x3				r1.xyz,		v0,			c0					; Transform vertex to world space				\n\
	m4x3				r0.xyz,		r1,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r0,			c6					; Transform vertex to homogeneous space			\n\
																													\n\
	; Fog factor																									\n\
	mov					oFog,		c20.y							; Output fog factor (constant 1.0)				\n\
																													\n\
	; Diffuse color																									\n\
	mov					oD0,		v2								; Output global diffuse as diffuse color		\n\
	";

	char *psSource = "\
																													\n\
	ps_1_1																											\n\
																													\n\
	; Arithmetic ops																								\n\
	mov					r0,			v0								; Copy diffuse color to output					\n\
	";

	// Assemble vertex shader
	m_vertexShader = RuD3DAssembleVertexShader(vsSource);

	// Assemble pixel shader
	m_pixelShader = RuD3DAssemblePixelShader(psSource);
}

CRuShader_SM20EZ_Gizmo_CodedLine::~CRuShader_SM20EZ_Gizmo_CodedLine()
{
}

void CRuShader_SM20EZ_Gizmo_CodedLine::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Static_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(m_pixelShader);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants + 8);
		m_engine->SetPixelShaderConstants(0, m_pixelShaderConstants, m_numBasePixelShaderConstants);

		m_engine->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_engine->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_engine->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}
}

void CRuShader_SM20EZ_Gizmo_CodedLine::SetParameters(const CRuParameterCache *paramCache)
{
	// Chain set parameters call to parent
	CRuShader_SM20EZ_Static_Base::SetParameters(paramCache);

	UINT64 param;

	if(paramCache->GetParameter(ruPARAMTYPE_TRANSFORM_0, param))
	{
		CRuMatrix4x4 *transform = (CRuMatrix4x4 *) param;

		for(UINT32 i = 0, k = 28 * 4; i < 3; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k] = transform->m_elements[j][i];
			}
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_DWORD_0, param))
	{
		INT32 highlightColorOffset = 27 * 4;
		RUCOLOR highlightColor = static_cast<RUCOLOR>(param);

		m_shaderConstants[highlightColorOffset++] = ((highlightColor >> 16) & 0xFF) / 255.0f;
		m_shaderConstants[highlightColorOffset++] = ((highlightColor >>  8) & 0xFF) / 255.0f;
		m_shaderConstants[highlightColorOffset++] = ((highlightColor >>  0) & 0xFF) / 255.0f;
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
