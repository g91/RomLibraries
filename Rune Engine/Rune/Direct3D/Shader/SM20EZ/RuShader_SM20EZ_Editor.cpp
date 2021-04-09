#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Editor.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Editor_PolyHighlight::CRuShader_SM20EZ_Editor_PolyHighlight(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Static_Base(engine)
{
	char *vsSource = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_color			v4											; Declare diffuse color							\n\
																													\n\
	def					c22,		3.0, 1.0, 1.0, 1.0																\n\
																													\n\
	mov					r0,			c22																				\n\
	mov					r1,			c22																				\n\
	m4x3				r0.xyz,		v0,			c0					; Transform vertex to world space				\n\
	m4x3				r1.xyz,		r0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r1,			c6					; Transform vertex to homogeneous space			\n\
																													\n\
	mov					oD0,		v4								; Set diffuse									\n\
																													\n\
	; Fog factor																									\n\
	mov					oFog,		c20.y							; Output fog factor								\n\
	";

	LPD3DXBUFFER assembledShader;
	LPD3DXBUFFER errorMessages;
	if(D3DXAssembleShader(vsSource, strlen(vsSource), NULL, NULL, 0, &assembledShader,	&errorMessages) != D3D_OK)
	{
		MessageBox(NULL, "Error assembling VS", "", MB_OK);

		if(assembledShader)
			assembledShader->Release();
		if(errorMessages)
		{
			MessageBox(NULL, (LPCSTR) errorMessages->GetBufferPointer(), "", MB_OK);
			errorMessages->Release();
		}

		return;
	}

	// Create shader
	IDirect3DVertexShader9 *vertexShader = NULL;
	if(m_engine->GetD3DDevice()->CreateVertexShader((DWORD *) assembledShader->GetBufferPointer(), &m_vertexShader) != D3D_OK)
	{
//		MessageBox(NULL, "Error creating VS", "", MB_OK);
	}

	if(assembledShader)
		assembledShader->Release();
	if(errorMessages)
		errorMessages->Release();

	// Overrides
	m_override_ZWrite = TRUE;
}

CRuShader_SM20EZ_Editor_PolyHighlight::~CRuShader_SM20EZ_Editor_PolyHighlight()
{
}

void CRuShader_SM20EZ_Editor_PolyHighlight::BeginPass(INT32 passNumber)
{
	if(passNumber == 0)
	{
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(NULL);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, 13);

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		m_engine->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);

		m_engine->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

		m_engine->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);

		m_engine->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_engine->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	}
	else if(passNumber == 1)
	{
		float slopeScaleDepthBias = -1.0f;

		m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		m_engine->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFF0000);
		m_engine->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*) (&slopeScaleDepthBias)));

		m_engine->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

		m_engine->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);

		m_engine->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_engine->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	}
}

void CRuShader_SM20EZ_Editor_PolyHighlight::EndPass(INT32 passNumber)
{
	if(passNumber == 1)
	{
		float slopeScaleDepthBias = 0.0f;

		m_engine->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		m_engine->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*) (&slopeScaleDepthBias)));
	}
}

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Editor_ColVis::CRuShader_SM20EZ_Editor_ColVis(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Base(engine)
{
	// Setup vertex elements
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },
		{ 0,	12,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_NORMAL,		0 },
		{ 0,	24,	ruELEMENTTYPE_D3DCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },
		{ 0,	28,	ruELEMENTTYPE_D3DCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		1 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexDecl = m_engine->CreateVertexDeclaration(vertexElements);

	char *vsSource = "\
																													\n\
	vs_1_1															; Vertex shader version 1.1						\n\
																													\n\
	dcl_position0		v0											; Declare position								\n\
	dcl_normal			v1											; Declare normal								\n\
	dcl_color			v4											; Declare diffuse color							\n\
	dcl_texcoord0		v5											; Declare input texcoord 0						\n\
																													\n\
	; Transform position into homogeneous space																		\n\
	mov					r0,			c20.yyyy						; Move 1 into r0								\n\
	mov					r1,			c20.yyyy						; Move 1 into r1								\n\
	m4x3				r0.xyz,		v0,			c0					; Transform vertex to world space				\n\
	m4x3				r1.xyz,		r0,			c3					; Transform vertex to view space				\n\
	m4x4				oPos,		r1,			c6					; Transform vertex to homogeneous space			\n\
																													\n\
	; Ambient lighting into r4																						\n\
	mov					r4,			c10																				\n"
	"																												\n\
	; Object-level spherical harmonics																				\n\
	add					r4,			c11,		r4					; Constant term									\n\
	mad					r4,			c12,		v1.x,		r4		; 1st-order terms								\n\
	mad					r4,			c13,		v1.y,		r4														\n\
	mad					r4,			c14,		v1.z,		r4														\n\
	mul					r5.x,		v1.x,		v1.z				; X * Z											\n\
	mul					r5.y,		v1.z,		v1.y				; Z * Y											\n\
	mul					r5.z,		v1.y,		v1.x				; Y * X											\n\
	mad					r4,			c15,		r5.x,		r4		; 2nd-order terms								\n\
	mad					r4,			c16,		r5.y,		r4														\n\
	mad					r4,			c17,		r5.z,		r4														\n\
	mul					r5.xyz,		v1,			v1																	\n\
	mul					r5.z,		r5.z,		c20.x																\n\
	sub					r5.z,		r5.z,		c20.y																\n\
	mad					r4,			c18,		r5.z,		r4														\n\
	sub					r5.x,		r5.x,		r5.y																\n\
	mad					r4,			c19,		r5.x,		r4														\n\
																													\n\
	; Diffuse color																									\n\
	mul					r4,			v4,			c26					; Multiply diffuse by global diffuse			\n\
	mov					oD0,		r4								; Set diffuse									\n\
																													\n\
	; Fog factor																									\n\
	mov					oFog,		c20.y							; Output fog factor								\n\
	";

	char *psSource = "\
																													\n\
	ps_1_1																											\n\
																													\n\
	; Arithmetic ops																								\n\
	mov					r0,			v0								; Modulate texture with diffuse					\n\
	";

	// Assemble vertex shader
	m_vertexShader = RuD3DAssembleVertexShader(vsSource);

	// Assemble pixel shader
	m_pixelShader = RuD3DAssemblePixelShader(psSource);

	// Overrides
	m_override_ZWrite = TRUE;
}

CRuShader_SM20EZ_Editor_ColVis::~CRuShader_SM20EZ_Editor_ColVis()
{
}

void CRuShader_SM20EZ_Editor_ColVis::BeginPass(INT32 passNumber)
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

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

//		m_engine->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
}

void CRuShader_SM20EZ_Editor_ColVis::EndPass(INT32 passNumber)
{
//	m_engine->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}
/*
void CRuShader_SM20EZ_Editor_ColVis::Instance(INT32 streamIndex, IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const
{
	if(streamIndex == 0)
	{
		CRuVertex_Static_NoTexture *vertices = (CRuVertex_Static_NoTexture *) buffer;
		IRuMorphTarget *morphTarget = mesh->GetMorphTarget(morphTargetIndex);
		const DWORD *diffuse = morphTarget->GetDiffuse();

		if(morphTarget)
		{
			const CRuVector3 *position = morphTarget->GetPosition();
			const CRuVector3 *normal = morphTarget->GetNormal();

			if(normal && diffuse)
			{
				for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
				{
					vertices[i].x = position[i].x;
					vertices[i].y = position[i].y;
					vertices[i].z = position[i].z;

					vertices[i].nx = normal[i].x;
					vertices[i].ny = normal[i].y;
					vertices[i].nz = normal[i].z;

					vertices[i].diffuse = diffuse[i];
					vertices[i].specular = 0;
				}
			}
			else if(normal)
			{
				for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
				{
					vertices[i].x = position[i].x;
					vertices[i].y = position[i].y;
					vertices[i].z = position[i].z;

					vertices[i].nx = normal[i].x;
					vertices[i].ny = normal[i].y;
					vertices[i].nz = normal[i].z;

					vertices[i].diffuse = 0;
					vertices[i].specular = 0;
				}
			}
			else if(diffuse)
			{
				for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
				{
					vertices[i].x = position[i].x;
					vertices[i].y = position[i].y;
					vertices[i].z = position[i].z;

					vertices[i].nx = 0.0f;
					vertices[i].ny = 1.0f;
					vertices[i].nz = 0.0f;

					vertices[i].diffuse = diffuse[i];
					vertices[i].specular = 0;
				}
			}
		}
	}
}
*/
// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Editor_Translucent::CRuShader_SM20EZ_Editor_Translucent(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Base(engine)
{
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
}

CRuShader_SM20EZ_Editor_Translucent::~CRuShader_SM20EZ_Editor_Translucent()
{
}

void CRuShader_SM20EZ_Editor_Translucent::BeginPass(INT32 passNumber)
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

void CRuShader_SM20EZ_Editor_Translucent::EndPass(INT32 passNumber)
{
}

void CRuShader_SM20EZ_Editor_Translucent::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SM20EZ_Base::SetParameters(paramCache);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
