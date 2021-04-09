#include "../../../Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Base::CRuShader_SM20EZ_Base(CRuEngineD3D *engine)
:	CRuShader_SMUNI_Base(engine)
{
	m_numBaseConstants = 30;
	m_numBasePixelShaderConstants = 2;
}

CRuShader_SM20EZ_Base::~CRuShader_SM20EZ_Base()
{
}

INT32 CRuShader_SM20EZ_Base::GetShaderClass() const
{
	return 1000;
}

INT32 CRuShader_SM20EZ_Base::GetShaderPriority() const
{
	return 1000;
}

void CRuShader_SM20EZ_Base::BeginPass(INT32 passNumber)
{
	// Chain to base class
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

	// Set self-illumination parameters
	INT32 selfIllumRGBOffset = 23 * 4;
	m_shaderConstants[selfIllumRGBOffset++] = m_material_SelfIlluminationColor.m_r;
	m_shaderConstants[selfIllumRGBOffset++] = m_material_SelfIlluminationColor.m_g;
	m_shaderConstants[selfIllumRGBOffset++] = m_material_SelfIlluminationColor.m_b;

	INT32 selfIllumOffset = 23 * 4 + 3;
	m_shaderConstants[selfIllumOffset] = m_material_SelfIllumination;

	// Set extrude scale
	INT32 extrudeScaleOffset = 103;
	m_shaderConstants[extrudeScaleOffset] = m_fx_ExtrudeScale;
}

void CRuShader_SM20EZ_Base::EndPass(INT32 passNumber)
{
}

void CRuShader_SM20EZ_Base::SetParameters(const CRuParameterCache *paramCache)
{
	CRuShader_SMUNI_Base::SetParameters(paramCache);

	UINT64 param;

	if(paramCache->GetParameter(ruPARAMTYPE_LIGHT_AMBIENT, param))
	{
		INT32 ambientOffset = 40;
		CRuColor4 *ambient = (CRuColor4 *) param;
		if(ambient)
		{
			m_shaderConstants[ambientOffset++] = ambient->m_r;
			m_shaderConstants[ambientOffset++] = ambient->m_g;
			m_shaderConstants[ambientOffset++] = ambient->m_b;
			m_shaderConstants[ambientOffset++] = ambient->m_a;
		}
		else
		{
			for(INT32 i = 0; i < 4; ++i)
			{
				m_shaderConstants[ambientOffset++] = 0.0f;
			}
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_LIGHT_OBJ_SH9, param))
	{
		INT32 sh9Offset = 44;
		CRuColor4 *shCoefficients = (CRuColor4 *) param;
		if(shCoefficients)
		{
			for(INT32 i = 0; i < 9; ++i)
			{
				m_shaderConstants[sh9Offset++] = shCoefficients[i].m_r;
				m_shaderConstants[sh9Offset++] = shCoefficients[i].m_g;
				m_shaderConstants[sh9Offset++] = shCoefficients[i].m_b;
				m_shaderConstants[sh9Offset++] = shCoefficients[i].m_a;
			}
		}
		else
		{
			for(INT32 i = 0; i < 36; ++i)
			{
				m_shaderConstants[sh9Offset++] = 0.0f;
			}
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_LIGHT_SPECULAR_DIR, param))
	{
		INT32 specularOffset = 84;
		CRuVector3 *specularDir = (CRuVector3 *) param;
		if(specularDir)
		{
			m_shaderConstants[specularOffset++] = specularDir->m_x;
			m_shaderConstants[specularOffset++] = specularDir->m_y;
			m_shaderConstants[specularOffset++] = specularDir->m_z;
			m_shaderConstants[specularOffset++] = 1.0f;
		}
		else
		{
			for(INT32 i = 0; i < 4; ++i)
			{
				m_shaderConstants[specularOffset++] = 0.0f;
			}
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_LIGHT_SPECULAR_COLOR, param))
	{
		INT32 specularOffset = 88;
		CRuColor4 specularColor(static_cast<RUCOLOR>(param));

		m_shaderConstants[specularOffset++] = specularColor.m_r;
		m_shaderConstants[specularOffset++] = specularColor.m_g;
		m_shaderConstants[specularOffset++] = specularColor.m_b;
		m_shaderConstants[specularOffset++] = specularColor.m_a;
	}
}

// ************************************************************************************************************************************************************

CRuShader_SM20EZ_Static_Base::CRuShader_SM20EZ_Static_Base(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Base(engine)
{
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },	// Position
		{ 0,	12,	ruELEMENTTYPE_UBYTE4,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_NORMAL,		0 },	// Packed normal
		{ 0,	16,	ruELEMENTTYPE_RUCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },	// Diffuse Color
		{ 0,	20,	ruELEMENTTYPE_FLOAT2,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_TEXCOORD,	0 },	// UV0 (Diffuse)
		{ 0,	28,	ruELEMENTTYPE_SHORT2N,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_TEXCOORD,	1 },	// UV1 (Lightmap)
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexDecl = m_engine->CreateVertexDeclaration(vertexElements);

	// Alter base constant count
	m_numBaseConstants = 31;

	// Set identity matrix to identity
	m_identityMatrix.SetToIdentity();
}

CRuShader_SM20EZ_Static_Base::~CRuShader_SM20EZ_Static_Base()
{
}

void CRuShader_SM20EZ_Static_Base::SetDefaultParameters()
{
	CRuShader_SM20EZ_Base::SetDefaultParameters();

	INT32 uvTransformOffset = 108;
	for(UINT32 i = 0, k = uvTransformOffset; i < 4; ++i)
	{
		for(UINT32 j = 0; j < 4; ++j, ++k)
		{
			m_shaderConstants[k] = m_identityMatrix.m_elements[j][i];
		}
	}
}

void CRuShader_SM20EZ_Static_Base::SetParameters(const CRuParameterCache *paramCache)
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

CRuShader_SM20EZ_Skin_Base::CRuShader_SM20EZ_Skin_Base(CRuEngineD3D *engine)
:	CRuShader_SM20EZ_Base(engine),
	m_maxSubFrames(0),
	m_numSubFrames(0),
	m_skinTransforms(NULL),
	m_indexMap(NULL)
{
/*
	// Setup vertex elements
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,		0 },
		{ 0,	12,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_NORMAL,			0 },
		{ 0,	24,	ruELEMENTTYPE_FLOAT4,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_BLENDINDICES,	0 },
		{ 0,	40,	ruELEMENTTYPE_FLOAT4,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_BLENDWEIGHT,		0 },
		{ 0,	56,	ruELEMENTTYPE_FLOAT2,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_TEXCOORD,		0 },
		RUVERTEXDECL_END()
	};
*/
	// Setup vertex elements
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,		0 },
		{ 0,	12,	ruELEMENTTYPE_UBYTE4,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_NORMAL,			0 },
		{ 0,	16,	ruELEMENTTYPE_RUCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,			0 },
		{ 0,	20,	ruELEMENTTYPE_UBYTE4,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_BLENDINDICES,	0 },
		{ 0,	24,	ruELEMENTTYPE_UBYTE4,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_BLENDWEIGHT,		0 },
		{ 0,	28,	ruELEMENTTYPE_SHORT2N,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_TEXCOORD,		0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexDecl = m_engine->CreateVertexDeclaration(vertexElements);

	// Alter base constant count
	m_numBaseConstants = 32;

	// Calculate max bone count
	m_maxSubFrames = (m_engine->GetD3DDeviceCaps().MaxVertexShaderConst - m_numBaseConstants) / 3;

	// Set identity matrix to identity
	m_identityMatrix.SetToIdentity();
}

CRuShader_SM20EZ_Skin_Base::~CRuShader_SM20EZ_Skin_Base()
{
}

void CRuShader_SM20EZ_Skin_Base::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SM20EZ_Base::BeginPass(passNumber);

	if(passNumber == 0)
	{
		INT32 skinPaletteStart = 32;
//		INT32 skinPaletteCoefficientOffset = 27 * 4;
		INT32 skinConstantsOffset = skinPaletteStart * 4;
/*
		// Setup coefficients used in indexed palette skinning
		m_shaderConstants[skinPaletteCoefficientOffset++] = static_cast<REAL>(skinPaletteStart);
		m_shaderConstants[skinPaletteCoefficientOffset++] = 3.0f;
		m_shaderConstants[skinPaletteCoefficientOffset++] = 1.0f;
		m_shaderConstants[skinPaletteCoefficientOffset++] = 0.0f;
*/
		if(m_skinTransforms)
		{
			// Setup skin transform constants
			if(m_indexMap)
			{
				for(INT32 n = 0, k = skinConstantsOffset; n < m_numSubFrames; ++n)
				{
					// Iterate through each column
					for(INT32 i = 0; i < 3; ++i)
					{
						// Iterate through each row
						for(INT32 j = 0; j < 4; ++j, ++k)
						{
							m_shaderConstants[k] = m_skinTransforms[m_indexMap[n]].m_elements[j][i];
						}
					}
				}
			}
			else
			{
				// Index map is NULL, so copy skin transforms directly into shader constants
				for(INT32 n = 0, k = skinConstantsOffset; n < m_numSubFrames; ++n)
				{
					// Iterate through each column
					for(INT32 i = 0; i < 3; ++i)
					{
						// Iterate through each row
						for(INT32 j = 0; j < 4; ++j, ++k)
						{
							m_shaderConstants[k] = m_skinTransforms[n].m_elements[j][i];
						}
					}
				}
			}
		}
		else
		{
			// No skin transform is supplied, set everything to identity
			memset(&m_shaderConstants[skinConstantsOffset], 0, sizeof(float) * m_maxSubFrames * 12);
			for(INT32 n = 0, k = skinConstantsOffset; n < m_maxSubFrames; ++n)
			{
				// Iterate through each register row
				for(INT32 i = 0; i < 3; ++i, k += 5)
				{
					m_shaderConstants[k] = 1.0f;
				}

				k -= 3;
			}

			m_numSubFrames = m_maxSubFrames;
		}
	}
}

void CRuShader_SM20EZ_Skin_Base::SetDefaultParameters()
{
	CRuShader_SM20EZ_Base::SetDefaultParameters();

	INT32 uvTransformOffset = 108;
	for(UINT32 i = 0, k = uvTransformOffset; i < 4; ++i)
	{
		for(UINT32 j = 0; j < 4; ++j, ++k)
		{
			m_shaderConstants[k] = m_identityMatrix.m_elements[j][i];
		}
	}
}

void CRuShader_SM20EZ_Skin_Base::SetParameters(const CRuParameterCache *paramCache)
{
	// Chain to parent class
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

	if(paramCache->GetParameter(ruPARAMTYPE_SKIN_NODECOUNT, param))
	{
		m_numSubFrames = (INT32) param;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SKIN_TRANSFORMS, param))
	{
		m_skinTransforms = (CRuMatrix4x4 *) param;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SKIN_INDEXMAP, param))
	{
		m_indexMap = (UINT16 *) param;
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
