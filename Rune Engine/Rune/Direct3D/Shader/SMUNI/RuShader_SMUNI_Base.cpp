#include "../../../Direct3D/Shader/SMUNI/RuShader_SMUNI_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

IDirect3DVertexShader9 *RuD3DAssembleVertexShader(const char *vsString)
{
	IDirect3DVertexShader9 *vertexShader = NULL;

	LPD3DXBUFFER assembledShader;
	LPD3DXBUFFER errorMessages;
	if(D3DXAssembleShader(vsString, (UINT)strlen(vsString), NULL, NULL, 0, &assembledShader,	&errorMessages) != D3D_OK)
	{
		MessageBox(NULL, "Error assembling VS", "", MB_OK);

		if(assembledShader)
			assembledShader->Release();
		if(errorMessages)
		{
			MessageBox(NULL, (LPCSTR) errorMessages->GetBufferPointer(), "", MB_OK);
			errorMessages->Release();
		}

		return NULL;
	}

	// Create shader
	if(g_ruEngineD3D->GetD3DDevice()->CreateVertexShader((DWORD *) assembledShader->GetBufferPointer(), &vertexShader) != D3D_OK)
	{
	}

	if(assembledShader)
		assembledShader->Release();
	if(errorMessages)
		errorMessages->Release();

	return vertexShader;
}

IDirect3DPixelShader9 *RuD3DAssemblePixelShader(const char *psString)
{
	IDirect3DPixelShader9 *pixelShader = NULL;

	LPD3DXBUFFER assembledShader;
	LPD3DXBUFFER errorMessages;
	if(D3DXAssembleShader(psString, (UINT)strlen(psString), NULL, NULL, 0, &assembledShader,	&errorMessages) != D3D_OK)
	{
		if(assembledShader)
			assembledShader->Release();
		if(errorMessages)
		{
			MessageBox(NULL, (LPCSTR) errorMessages->GetBufferPointer(), "", MB_OK);
			errorMessages->Release();
		}

		return NULL;
	}

	// Create shader
	if(g_ruEngineD3D->GetD3DDevice()->CreatePixelShader((DWORD *) assembledShader->GetBufferPointer(), &pixelShader) != D3D_OK)
	{
		// Handle error here
	}

	if(assembledShader)
		assembledShader->Release();
	if(errorMessages)
		errorMessages->Release();

	return pixelShader;
}

IDirect3DVertexShader9 *RuD3DCompileVertexShader(const char *vsString, const char *entryFunction, const char *profile, ID3DXConstantTable **constantTableOut)
{
	IDirect3DVertexShader9 *vertexShader = NULL;

	LPD3DXBUFFER compiledShader;
	LPD3DXBUFFER errorMessages;
	if(D3DXCompileShader(vsString, (UINT)strlen(vsString), NULL, NULL, entryFunction, profile, 0, &compiledShader, &errorMessages, constantTableOut) != D3D_OK)
	{
		MessageBox(NULL, "Error assembling VS", "", MB_OK);

		if(compiledShader)
			compiledShader->Release();
		if(errorMessages)
		{
			MessageBox(NULL, (LPCSTR) errorMessages->GetBufferPointer(), "", MB_OK);
			errorMessages->Release();
		}

		return NULL;
	}

	// Create shader
	if(g_ruEngineD3D->GetD3DDevice()->CreateVertexShader((DWORD *) compiledShader->GetBufferPointer(), &vertexShader) != D3D_OK)
	{
	}

	if(compiledShader)
		compiledShader->Release();
	if(errorMessages)
		errorMessages->Release();

	return vertexShader;
}

IDirect3DPixelShader9 *RuD3DCompilePixelShader(const char *psString, const char *entryFunction, const char *profile, ID3DXConstantTable **constantTableOut)
{
	IDirect3DPixelShader9 *pixelShader = NULL;

	LPD3DXBUFFER compiledShader;
	LPD3DXBUFFER errorMessages;
	if(D3DXCompileShader(psString, (UINT)strlen(psString), NULL, NULL, entryFunction, profile, 0, &compiledShader, &errorMessages, constantTableOut) != D3D_OK)
	{
		if(compiledShader)
			compiledShader->Release();
		if(errorMessages)
		{
			MessageBox(NULL, (LPCSTR) errorMessages->GetBufferPointer(), "", MB_OK);
			errorMessages->Release();
		}

		return NULL;
	}

	// Create shader
	if(g_ruEngineD3D->GetD3DDevice()->CreatePixelShader((DWORD *) compiledShader->GetBufferPointer(), &pixelShader) != D3D_OK)
	{
		// Handle error here
	}

	if(compiledShader)
		compiledShader->Release();
	if(errorMessages)
		errorMessages->Release();

	return pixelShader;
}

// ************************************************************************************************************************************************************

CRuShader_SMUNI_Base::CRuShader_SMUNI_Base(CRuEngineD3D *engine)
{
	// Store engine reference
	m_engine = engine;

	// Clear variables
	m_vertexDecl = NULL;
	m_vertexShader = NULL;
	m_pixelShader = NULL;

	// Clear vertex shader constants
	m_numBaseConstants = 0;
	memset(m_shaderConstants, 0, sizeof(REAL) * 4 * 256);

	// Clear pixel shader constants
	m_numBasePixelShaderConstants = 0;
	memset(m_pixelShaderConstants, 0, sizeof(REAL) * 4 * 32);

	// Set default parameters
	SetDefaultParameters();
}

CRuShader_SMUNI_Base::~CRuShader_SMUNI_Base()
{
	// Release resources
	ruSAFE_RELEASE(m_vertexDecl);
	ruSAFE_RELEASE(m_vertexShader);
	ruSAFE_RELEASE(m_pixelShader);
}

INT32 CRuShader_SMUNI_Base::GetShaderClass() const
{
	return 1000;
}

INT32 CRuShader_SMUNI_Base::GetShaderPriority() const
{
	return 1000;
}

void CRuShader_SMUNI_Base::BeginPass(INT32 passNumber)
{
	// Calculate effective material alpha
	m_material_EffectiveAlpha = m_material_Alpha * m_material_AlphaModifier;

	// Material is translucent if the global alpha is less than one, or if the material was already marked translucent
	m_material_Translucent = (m_material_EffectiveAlpha < 1.0f) || m_material_Translucent;

	// Select default source blend mode
	D3DBLEND defaultSrcBlendMode = D3DBLEND_ONE;

	// Setup fog
	if(m_override_Fog == FALSE)
	{
		m_engine->SetRenderState(D3DRS_FOGENABLE, TRUE);
	}

	if(m_override_AlphaBlend == FALSE)
	{
		switch(m_material_AlphaMode)
		{
			case ruALPHAMODE_NORMAL:
			case ruALPHAMODE_PREMULTIPLIED_NORMAL:
				{
					// Material translucency is off
					if(m_material_Translucent == FALSE || m_fx_AlphaZPass)
					{
						// Disable blending and enable Z write
						m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
						m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

						SetZTestMode(D3DCMP_LESSEQUAL);
						SetZWriteMode(TRUE);
					}
					// Material translucency is on
					else
					{
						m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
						m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

						m_engine->SetRenderState(D3DRS_SRCBLEND, defaultSrcBlendMode);
						m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

						SetZTestMode(D3DCMP_LESSEQUAL);
						SetZWriteMode((m_material_Translucent == FALSE) ? TRUE : FALSE);
					}
				}

				break;

			case ruALPHAMODE_ADD:
			case ruALPHAMODE_PREMULTIPLIED_ADD:
				{
					m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
					m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
					m_engine->SetRenderState(D3DRS_SRCBLEND, defaultSrcBlendMode);
					m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

					SetZTestMode(D3DCMP_LESSEQUAL);
					SetZWriteMode(FALSE);
				}

				break;

			case ruALPHAMODE_SUBTRACT:
				{
					m_engine->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
					m_engine->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
					m_engine->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					m_engine->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

					SetZTestMode(D3DCMP_LESSEQUAL);
					SetZWriteMode(FALSE);
				}

				break;
		}
	}
	else
	{
		SetZTestMode(D3DCMP_LESSEQUAL);
	}

	if(m_override_AlphaTest == FALSE)
	{
		// Setup alpha test states
		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, m_alphaTestEnabled);

		if(m_alphaTestEnabled)
		{
			// Modify alpha test threshold based on material alpha?
			INT32 modifiedAlphaThreshold = (BYTE) (m_alphaTestThreshold * m_material_EffectiveAlpha);

			m_engine->SetRenderState(D3DRS_ALPHAREF, modifiedAlphaThreshold);

			switch(m_alphaTestMode)
			{
				case ruALPHATESTMODE_NONE:
					m_engine->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
					break;

				case ruALPHATESTMODE_GREATER:
					m_engine->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
					break;

				case ruALPHATESTMODE_GREATEREQUAL:
					m_engine->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
					break;

				case ruALPHATESTMODE_EQUAL:
					m_engine->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_EQUAL);
					break;

				case ruALPHATESTMODE_LESSEQUAL:
					m_engine->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESSEQUAL);
					break;

				case ruALPHATESTMODE_LESS:
					m_engine->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);
					break;

				case ruALPHATESTMODE_NOTEQUAL:
					m_engine->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL);
					break;
			}
		}
	}

	if(m_2Sided)
	{
		m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	}
	else
	{
		if(m_fx_ReverseCull)
		{
			m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		}
		else
		{
			m_engine->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		}
	}

	if(m_wire)
	{
		m_engine->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	else
	{
		m_engine->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if(m_fx_StencilEnable)
	{
		// Enable stencil testing
		m_engine->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		m_engine->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		m_engine->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		m_engine->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		m_engine->SetRenderState(D3DRS_STENCILREF, m_fx_StencilID);
		m_engine->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
		m_engine->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
		m_engine->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	}
	else
	{
		m_engine->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}

	if(m_material_ColorWriteEnabled)
	{
		m_engine->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
	}
	else
	{
		m_engine->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
	}

	if(m_rawData_Source)
	{
		RuMemcpy(&m_shaderConstants[m_rawData_Offset * 4], m_rawData_Source, sizeof(REAL) * m_rawData_Count * 4);
	}

	if(m_rawPSData_Source)
	{
		RuMemcpy(&m_pixelShaderConstants[m_rawPSData_Offset * 4], m_rawPSData_Source, sizeof(REAL) * m_rawPSData_Count * 4);
	}
}

void CRuShader_SMUNI_Base::EndPass(INT32 passNumber)
{
}

void CRuShader_SMUNI_Base::SetDefaultParameters()
{
	m_override_Fog = FALSE;
	m_override_AlphaTest = FALSE;
	m_override_AlphaBlend = FALSE;
	m_override_ZTest = FALSE;
	m_override_ZWrite = FALSE;

	m_2Sided = FALSE;
	m_faceted = FALSE;
	m_wire = FALSE;
	m_alphaTestEnabled = FALSE;
	m_alphaTestThreshold = 200;
	m_alphaTestMode = ruALPHATESTMODE_NONE;
	m_material_AlphaMode = ruALPHAMODE_NORMAL;
	m_material_Translucent = FALSE;
	m_material_ZTestMode = ruZTESTMODE_DEFAULT;
	m_material_ZWriteMode = ruZWRITEMODE_DEFAULT;
	m_material_Alpha = 1.0f;
	m_material_AlphaModifier = 1.0f;
	m_material_EffectiveAlpha = 1.0f;
	m_material_ColorWriteEnabled = TRUE;

	m_material_Diffuse.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_material_SelfIlluminationColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_material_SelfIllumination = 0.0f;

	m_rawData_Offset = 0;
	m_rawData_Count = 0;
	m_rawData_Source = NULL;

	m_rawPSData_Offset = 0;
	m_rawPSData_Count = 0;
	m_rawPSData_Source = NULL;

	m_fog_Constant = 0;
	m_fog_Start = 0;
	m_fog_End = 0;
	m_fog_Color.Set(0.0f, 0.0f, 0.0f, 0.0f);

	m_fx_AlphaZPass = FALSE;
	m_fx_ReverseCull = FALSE;
	m_fx_StencilEnable = FALSE;
	m_fx_StencilID = 0;
	m_fx_ExtrudeScale = 0.0f;
}

void CRuShader_SMUNI_Base::SetParameters(const CRuParameterCache *paramCache)
{
	UINT64 param;

	if(paramCache->GetParameter(ruPARAMTYPE_TRANSFORM_WORLD, param))
	{
		CRuMatrix4x4 *transform = (CRuMatrix4x4 *) param;

		for(UINT32 i = 0, k = 0; i < 3; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k] = transform->m_elements[j][i];
			}
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_TRANSFORM_VIEW, param))
	{
		CRuMatrix4x4 *transform = (CRuMatrix4x4 *) param;

		for(UINT32 i = 0, k = 12; i < 3; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k] = transform->m_elements[j][i];
			}
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_TRANSFORM_PROJECTION, param))
	{
		CRuMatrix4x4 *transform = (CRuMatrix4x4 *) param;

		for(UINT32 i = 0, k = 24; i < 4; ++i)
		{
			for(UINT32 j = 0; j < 4; ++j, ++k)
			{
				m_shaderConstants[k] = transform->m_elements[j][i];
			}
		}
	}

	if(paramCache->GetParameter(ruPARAMTYPE_FOG_CONSTANT, param))
	{
		m_fog_Constant = RuCastINT64ToFloat(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_FOG_START, param))
	{
		m_fog_Start = RuCastINT64ToFloat(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_FOG_END, param))
	{
		m_fog_End = RuCastINT64ToFloat(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_FOG_COLOR, param))
	{
		m_fog_Color.Set(static_cast<RUCOLOR>(param));
	}

	if(paramCache->GetParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, param))
	{
		m_material_Diffuse.Set(static_cast<RUCOLOR>(param));
	}

	if(paramCache->GetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATIONCOLOR, param))
	{
		m_material_SelfIlluminationColor.Set(static_cast<RUCOLOR>(param));
	}

	if(paramCache->GetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, param))
	{
		m_material_SelfIllumination = RuCastINT64ToFloat(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_MATERIAL_2SIDED, param))
	{
		m_2Sided = static_cast<BOOL>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_MATERIAL_FACETED, param))
	{
		m_faceted = static_cast<BOOL>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_MATERIAL_WIRE, param))
	{
		m_wire = static_cast<BOOL>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, param))
	{
		m_alphaTestMode = static_cast<RuAlphaTestMode>(param);
		m_alphaTestEnabled = m_alphaTestMode != ruALPHATESTMODE_NONE;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, param))
	{
		m_alphaTestThreshold = static_cast<UINT32>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, param))
	{
		m_material_AlphaMode = static_cast<RuAlphaMode>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_MATERIAL_TRANSLUCENT, param))
	{
		m_material_Translucent = (BOOL)param;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_MATERIAL_ZTESTMODE, param))
	{
		m_material_ZTestMode = static_cast<RuZTestMode>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_MATERIAL_ZWRITEMODE, param))
	{
		m_material_ZWriteMode = static_cast<RuZWriteMode>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_MATERIAL_ALPHA, param))
	{
		m_material_Alpha = RuCastINT64ToFloat(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODIFIER, param))
	{
		m_material_AlphaModifier = RuCastINT64ToFloat(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_MATERIAL_COLORWRITEENABLE, param))
	{
		m_material_ColorWriteEnabled = static_cast<BOOL>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_RAW_OFFSET, param))
	{
		m_rawData_Offset = (INT32)param;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_RAW_COUNT, param))
	{
		m_rawData_Count = (INT32)param;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_RAW_DATA, param))
	{
		m_rawData_Source = reinterpret_cast<REAL *>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_RAW_PS_OFFSET, param))
	{
		m_rawPSData_Offset = (INT32)param;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_RAW_PS_COUNT, param))
	{
		m_rawPSData_Count = (INT32)param;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_RAW_PS_DATA, param))
	{
		m_rawPSData_Source = reinterpret_cast<REAL *>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_FX_REVERSECULL, param))
	{
		m_fx_ReverseCull = (BOOL)param;
	}

	if(paramCache->GetParameter(ruPARAMTYPE_FX_EXTRUDESCALE, param))
	{
		m_fx_ExtrudeScale = RuCastINT64ToFloat(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SYSTEM_ALPHAZPASS, param))
	{
		m_fx_AlphaZPass = static_cast<BOOL>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SYSTEM_STENCILENABLE, param))
	{
		m_fx_StencilEnable = static_cast<BOOL>(param);
	}

	if(paramCache->GetParameter(ruPARAMTYPE_SYSTEM_STENCILID, param))
	{
		m_fx_StencilID = static_cast<UINT8>(param);
	}
}

void CRuShader_SMUNI_Base::SetZTestMode(D3DCMPFUNC defaultValue)
{
	if(m_override_ZTest == FALSE)
	{
		switch(m_material_ZTestMode)
		{
			case ruZTESTMODE_DEFAULT:
				m_engine->SetRenderState(D3DRS_ZFUNC, defaultValue);
				break;

			case ruZTESTMODE_ALWAYS:
				m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
				break;

			case ruZTESTMODE_GREATER:
				m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATER);
				break;

			case ruZTESTMODE_GREATEREQUAL:
				m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATEREQUAL);
				break;

			case ruZTESTMODE_EQUAL:
				m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
				break;

			case ruZTESTMODE_LESSEQUAL:
				m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
				break;

			case ruZTESTMODE_LESS:
				m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
				break;

			case ruZTESTMODE_NOTEQUAL:
				m_engine->SetRenderState(D3DRS_ZFUNC, D3DCMP_NOTEQUAL);
				break;
		}
	}
}

void CRuShader_SMUNI_Base::SetZWriteMode(BOOL defaultValue)
{
	if(m_override_ZWrite == FALSE)
	{
		switch(m_material_ZWriteMode)
		{
			case ruZWRITEMODE_DEFAULT:
				m_engine->SetRenderState(D3DRS_ZWRITEENABLE, defaultValue);
				break;

			case ruZWRITEMODE_DISABLE:
				m_engine->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
				break;

			case ruZWRITEMODE_ENABLE:
				m_engine->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
				break;
		}
	}
}

// ************************************************************************************************************************************************************

CRuShader_SMUNI_NULL::CRuShader_SMUNI_NULL(CRuEngineD3D *engine)
:	CRuShader_SMUNI_Base(engine)
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

	char *vsSource = "\
																													\n\
	// Globals																										\n\
	float4x3			g_worldTransform		: register(c0);														\n\
	float4x3			g_viewTransform			: register(c3);														\n\
	float4x4			g_projectionTransform	: register(c6);														\n\
																													\n\
	float4				g_globalSettings		: register(c25);	// (Fog Constant, Start, End, Extrude Scale)	\n\
																													\n\
	struct VS_INPUT																									\n\
	{																												\n\
		float4			position				: POSITION;															\n\
		float3			normal					: NORMAL;															\n\
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
		worldPos.xyz = mul(v.position, (float4x3) g_worldTransform);												\n\
		viewPos.xyz = mul(worldPos, (float4x3) g_viewTransform);													\n\
		output.position = mul(viewPos, (float4x4) g_projectionTransform);											\n\
																													\n\
		// Calculate fog																							\n\
		float1 rcpFogRange = 1.0f / (g_globalSettings.z - g_globalSettings.y);										\n\
		float1 relativeZ = max(viewPos.z - g_globalSettings.y, 0.0f);												\n\
		output.fog = clamp(1.0f - (relativeZ * rcpFogRange) - g_globalSettings.x, 0.0f, 1.0f);						\n\
																													\n\
		// Accumulate ambient color																					\n\
		output.diffuse = float4(1, 1, 1, 1);																		\n\
																													\n\
		// Copy texture coordinates																					\n\
		output.texcoord = v.texcoord;																				\n\
																													\n\
		return output;																								\n\
	}																												\n\
	";

	// Compile vertex shader
	m_vertexShader = RuD3DCompileVertexShader(vsSource, "main", "vs_2_0", NULL);

	// Set constants count
	m_numBaseConstants = 26;
	m_numBasePixelShaderConstants = 0;
}

CRuShader_SMUNI_NULL::~CRuShader_SMUNI_NULL()
{
}

void CRuShader_SMUNI_NULL::BeginPass(INT32 passNumber)
{
	// Call base BeginPass to perform any additional setup
	CRuShader_SMUNI_Base::BeginPass(passNumber);

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
		m_vertexDecl->Select();
		m_engine->SetVertexShader(m_vertexShader);
		m_engine->SetPixelShader(NULL);
		m_engine->SetVertexShaderConstants(0, m_shaderConstants, m_numBaseConstants);

		m_engine->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_engine->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		m_engine->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		m_engine->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		m_engine->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		m_engine->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_engine->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		m_engine->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

void CRuShader_SMUNI_NULL::EndPass(INT32 passNumber)
{
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
